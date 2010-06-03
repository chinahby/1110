#ifndef AEERSA_H
#define AEERSA_H
/*======================================================
FILE:        AEERSA.h

SERVICES:    RSA and modular exponentiation crypto

DESCRIPTION: Use this interface/implementation for
             public key crypto functions. In particular
             RSA and modular exponentiation. Various
             forms of padding common with RSA are
             supported. DSP-based crypto is also
             supported.

CLASS IDs:   AEECLSID_RSA

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

#include "AEE.h"


//************************************************************************************************
//
// IRSA Interface
//
//************************************************************************************************

// IRSA Object
typedef struct _IRSA       IRSA;

// RSA Padding Types.
enum
{
   // PKCS #1 v1.5 padding. This currently is the most widely used mode.
   AEE_RSA_PKCS1TYPE1_PADDING    =  1,

   // PKCS #1 v2.0 padding. This currently is the most widely used mode.
   AEE_RSA_PKCS1TYPE2_PADDING    =  2,

   // PKCS #1 v1.5 padding with an SSL-specific modification that denotes that the server is SSL3 capable
   AEE_RSA_SSLV23_PADDING        =  3,

   //No padding used.  Application should only use cryptographically sound padding modes
   AEE_RSA_NO_PADDING            =  4,

   // PKCS #1 v2.1 OAEP padding.
   AEE_RSA_PKCS1_OAEP_SHA1_MGF1_PADDING =  5,

   // PKCS #1 v2.1 PSS padding.
   AEE_RSA_PKCS1_PSS_SHA1_MGF1_PADDING =  6
};


// Different signature types; used with IRSA_VerifySig
enum
{
   // MD2 with RSA encryption as defined in PKCS#1 version 1.5 RFC2313
   AEE_RSA_SIG_PKCS1_MD2    = 1,

   // MD5 with RSA encryption as defined in PKCS#1 version 1.5 RFC2313
   AEE_RSA_SIG_PKCS1_MD5    = 2,

   // SHA1 with RSA encryption as defined in PKCS#1 version 2.0 RFC2437
   AEE_RSA_SIG_PKCS1_SHA1   = 3,

   // SHA256 with RSA encryption as defined in PKCS#1 version 2.1 RFC3447
   AEE_RSA_SIG_PKCS1_SHA256 = 4,

   // SHA384 with RSA encryption as defined in PKCS#1 version 2.1 RFC3447
   AEE_RSA_SIG_PKCS1_SHA384 = 5,

   // SHA512 with RSA encryption as defined in PKCS#1 version 2.1 RFC3447
   AEE_RSA_SIG_PKCS1_SHA512 = 6,

   // SHA224 with RSA encryption as defined in PKCS#1 version 2.1 (see www.rsalabs.com)
   AEE_RSA_SIG_PKCS1_SHA224 = 7,

   // PSS SHA1 with MGF1 with RSA encryption as defined in PKCS#1 version 2.1 RFC3447
   AEE_RSA_SIG_PKCS1_PSS_SHA1_MGF1 = 8

};


QINTERFACE(IRSA)
{
   DECLARE_IBASE(IRSA)

   int      (*Init)(IRSA* po, const byte* pbMod, int cbMod, const byte* pbExp, int cbExp);
   int      (*RSA)(IRSA* po, const byte* pbIn, int cbIn, byte** ppbOut, int* pcbOut, int reserved);
   void     (*ModExp)(IRSA *po, const byte* pbIn, int cbIn, byte** ppbOut, int* pcbOut, uint32 *pdwResult, AEECallback *pCB);
   void     (*Encrypt)(IRSA* po, const byte* pbIn, int cbIn, byte* pbOut, int *pcbOut, int ePadType, uint32* pdwResult, AEECallback *pCB);
   void     (*Decrypt)(IRSA* po, const byte* pbIn, int cbIn, byte* pbOut, int *pcbOut, int ePadType, uint32* pdwResult, AEECallback *pCB);
   void     (*VerifySig)(IRSA *po, const byte* pbSig, int cbSigLen, const byte* digest, int cbDigestLen, int eSigType, uint32* pdwResult, AEECallback *pCB);
};

#define IRSA_AddRef(p)                                      GET_PVTBL(p,IRSA)->AddRef(p)
#define IRSA_Release(p)                                     GET_PVTBL(p,IRSA)->Release(p)
#define IRSA_Init(p, pbMod, cbMod, pbExp, cbExp)            GET_PVTBL(p,IRSA)->Init(p, pbMod, cbMod, pbExp, cbExp)
#define IRSA_RSA(p, pbin, cbin, ppbout, pcbout, pad)        GET_PVTBL(p,IRSA)->RSA(p, pbin, cbin, ppbout, pcbout, pad)
#define IRSA_ModExp(p,pbin,cbin,pbout,pcout,pr,pcb)         GET_PVTBL(p,IRSA)->ModExp(p,pbin,cbin,pbout,pcout,pr,pcb)
#define IRSA_Encrypt(p,pbin,cbin,pbout,pcout,pt,pr,pcb)     GET_PVTBL(p,IRSA)->Encrypt(p,pbin,cbin,pbout,pcout,pt,pr,pcb)
#define IRSA_Decrypt(p,pbin,cbin,pbout,pcout,pt,pr,pcb)     GET_PVTBL(p,IRSA)->Decrypt(p,pbin,cbin,pbout,pcout,pt,pr,pcb)
#define IRSA_VerifySig(p,pbin,cbin,pbdig,cbdig,pt,pr,pcb)   GET_PVTBL(p,IRSA)->VerifySig(p,pbin,cbin,pbdig,cbdig,pt,pr,pcb)


// return 1 if a common tame exponent, 0 if big and slow
// exp must be of type byte *
#define IRSA_FAST_EXPONENT(exp, expLen) \
  (( (expLen) == 1 && (exp)[0] == 3) || \
   ( (expLen) == 3 && (exp)[0] == 1 && (exp)[1] == 0 && (exp)[2] == 1) )



#endif /* AEERSA_H */


/*
=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

IRSA Interface

Description:

   This interface provides access to the RSA public key cryptographic
   algorithm and the basic underlying modular exponentiation. It also
   provides some of the padding and encoding commonly used with
   the RSA algorithm. Last it is modestly useful for managing memory in
   which keys are stored. For details on the algorithm itself and its
   uses see a good cryptography text book. If you are not familiar
   with RSA this is highly recommended.

   Software based RSA operations can take a long time and the time varies
   depending on the modulus/key size and the exponent. On most handsets,
   the public operations encrypt and verify will perform reasonably fast.
   The private operations, sign and decrypt, will take prohibitively long
   and performing them in software is discouraged. This is because public
   keys usually have a small well chosen exponent such as 3 or 2^16+1.
   For example keys in X.509 certificates typically use the exponent 2^16+1.

   Some platforms offer support for hardware based modular exponentiation.


   Typical use involves instantiating an IRSA with the usual call to
   ISHELL_CreateInstance(). The key material is added to the IRSA
   instance by calling IRSA_Init(). Then any of the RSA operations may
   be performed including modular exponentiation, encrypting,
   decrypting and signature verification. A separate interface for
   signing is not provided because it is the same as encrypt.

   The data to be encrypted must be less than the modulus. In other words,
   when using MEMCMP on the data and modulus buffers, the modulus must be
   bigger. RSA algorithm can only be applied when data is less than modulus.

   The IRSA_RSA() function is supported, but not recommended because
   it is a blocking synchronous function that doesn't support
   cancelling. The other functions take a call back record and are
   non-blocking. The callback is called when the operation is complete.
   The callback also allows the operation to be cancelled. It also
   allows the handset to process other events such as key presses while
   the RSA calculation is going on.

   An operation that takes a callback can be cancelled anytime by calling
   CALLBACK_Cancel() on that callback. The processing yields to the main
   Brew event loop periodically. This allows cancel requests to be
   processed and other scheduled events to be processed.

   An IRSA instance is a ref counted object that stores an RSA key.

   The returns codes for IRSA functions are described below with each function.
   The IRSA functions may also return other BREW error codes, so the caller must
   take proper precautions dealing with them.

   Finally a few notes on what is absent. There is no key generation.
   This would be extremely slow in software, but might be acceptable
   on a DSP. There is no way to retrieve a key once stored. Some of
   the modern padding types, OEAP and PSS in particular, are not
   available. There is also no way to store a full private/public
   key pair to enable faster private key operations using the
   Chinese Remainder Theorem.

The following header file is required for IX509Chain:

   AEERSA.h


=======================================================================

IRSA_AddRef()

Description:
   This function is inherited from IBASE_AddRef().

=======================================================================

IRSA_Release()

Description:
   This function is inherited from IBASE_Release().

=======================================================================

IRSA_Init()

Description:
   Initialize with key for RSA or modular exponentiation operation.
   Key consists of a modulus and an exponent input as byte arrays.
   The byte array is the binary representation of the modulus/exponent
   with most significant byte being the 0th byte.

Prototype:
   int IRSA_Init
   (
      IRSA* pIRSA,
      const byte* pchModulus,
      int cbModulus,
      const byte* pchExponent,
      int cbExponent
   )

Parameters:
   pIRSA: Pointer to IRSA interface object
   pchModulus:  [in] Pointer to modulus
   cbModulus:   [in] Modulus size in bytes
   pchExponent: [in] Pointer to public or private exponent
   chExponent:  [in] Public or Private exponent size in bytes

Return Value:
	SUCCESS:                 RSA key initialization successful.
   
	AEE_CRYPT_INVALID_KEY:   Key NULL or zero length
   
	ENOMEMORY:              Storage for the key could not be allocated

Comments:
   Call this function before calling other functions that perform
   cryptographic operations.

   If the key size is too large for an operation, the error will be
   returned when the operation is attempted, not here.

   The arguments are copies and must only persist for the duration of the
   call to IRSA_Init.  A chunk of memory is allocated to store the key
   and the anticipated result.

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

=======================================================================

IRSA_RSA()

Description:
   Obsoleted interface that performs modular exponentiation with the
   initialized key.  Because this function is blocking and syncrhonous it
   is not recommended. Refer to the IRSA_ModExp() documentation.

   Please use IRSA_ModExp() instead.

Prototype:
   int IRSA_RSA
   (
      IRSA* pIRSA,
      const byte *pbIn,
      int cbIn,
      byte **ppbOut,
      int* pcbOut,
      int reserved
   )

Parameters:
   pIRSA:    Pointer to IRSA interface object
   pbIn:     [in]  Pointer to data to be encrypted/decrypted
   cbIn:     [in]  Input data size in bytes
   ppbOut:   [out] Place for pointer to pointer of encrypted/decrypted data
   pcbOut:   [out] size of output in bytes
   reserved: [in]  should be 0

 Return Value:

	SUCCESS: if task is successful
   
	AEE_CRYPT_INVALID_KEY:  Invalid encryption/decryption key.
   
	AEE_CRYPT_INVALID_SIZE:    
		cbIn is not equal to the modulus size
		or pbIn is bigger than the modulus
	
   EFAILED:	encryption/decryption failed


Comments:
   A pointer to a buffer containing the result is returned. This
   buffer persists until the IRSA is released.

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

=======================================================================

IRSA_ModExp()

Description:
   This performs modular exponentiation, the basic operation
   for the RSA algorithm. This is a non-blocking replacement for
   IRSA_RSA(). The operation can be cancelled by calling CALLBACK_Cancel()
   on pCB. The contents of pbIn must be valid until the callback completes.
   The data is not copied. The result is returned in an internal buffer
   that is valid until the IRSA object is destroyed or another operation
   is performed.

   IRSA_ModExp() encrypts in blocks whose size is equivalent to the modulus size
   as set in the IRSA_Init(). This implies that the cbIn parameter must be
   equal to the modulus size. To encrypt data less than the modulus size,
   use IRSA_Encrypt() with a valid padding type.

Prototype:
   void IRSA_ModExp
   (
      IRSA* pIRSA,
      const byte* pbIn,
      int cbIn,
      byte** ppbOut,
      int* pcbOut,
      uint32 *pdwResult,
      AEECallback *pCB
   )

Parameters:
   pIRSA: pointer to the IRSA object to operate on
   pbIn:      [in]  pointer to the input buffer
   cbIn:      [in]  length of the input data
   ppbOut:    [out] pointer to the internal result buffer
   pcbOut:    [out] length of the resulting data
   pdwResult: [out] storage for the result of the operation
   pCB:       [in]  user defined callback to call when complete

Return Value:
The return value is in the uint32 pointed to by pdwResult.

     SUCCESS: if task is sucessful
	 
     AEE_CRYPT_INVALID_KEY:  key not set; Init not called
	 
     AEE_CRYPT_INVALID_SIZE: 
		cbIn is not equal to the modulus size
		or pbIn is bigger than the modulus
	
     ENOMEMORY:  ran out of memory while processing

Comments:
   Please be aware that the ppbOut will contain the pointer
   to the internal buffer and must not be freed by the user. It will be
   valid until another IRSA function is called on the same interface.
   Obviously it would be invalid, if the interface is released.

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

====================================================================

IRSA_Encrypt()

Description:
   This routine encrypts given data, which must be less than the size of the
   modulus specified in the IRSA object. It pads the data with the specified
   padding type provided in the ePadType parameter, and encrypts it using the
   modululs and exponent the user set up the IRSA object with in a
   previous call to IRSA_Init.

   If padding type is AEE_RSA_NO_PADDING, the cbIn must be of the same
   size as the modulus. For other padding types, the input length must be
   less than the block size. For the padding types provided, there must be
   atleast 11 spare bytes below the block size (ie for 512 bit(64byte) modulus,
   the data must be <= 53 (64-11) bytes).

   Use this function with a private key for the signing operation.

Prototype:
   void IRSA_Encrypt
   (
      IRSA* pIRSA,
      const byte* pbIn,
      int cbIn,
      byte* pbOut,
      int *pcbOut,
      int ePadType,
      uint32* pdwResult,
      AEECallback *pCB
   )

Parameters:
   pIRSA:  Pointer to the IRSA interface
   pbIn:   [in]      pointer to buffer of data to encrypt
   cbIn:   [in]      length of input buffer
   pbOut:  [out]     pointer to buffer to store encrpypted data, caller allocated
   pcbOut: [in/out]  length of output buffer on input, length of data on output
   ePadType:  [in]   type of padding to use
   pdwResult: [out]  result code
   pCB:       [in]   completion callback


Return Value:
None.  The result code is passed back in the pdwResult parameter.

     SUCCESS: if task is successful
	 
     AEE_CRYPT_INVALID_KEY:  key not set, Init not called
	 
     AEE_CRYPT_INVALID_SIZE:  
		cbIn is not equal to the modulus size
		or pbIn is bigger than the modulus
		(in case of ePadType being AEE_RSA_NO_PADDING)
	
     ENOMEMORY:  ran out of memory while processing
	 
     AEE_CRYPT_BUFFER_TOO_SMALL:  output buffer to small
	 
     AEE_CRYPT_INVALID_PADTYPE:  padding type given was wrong
	 
     ECLASSNOTSUPPORT:  
		Internal depedency failure, unable to 
		instantiate an object of class 
		implementing hash function needed to 
		complete the operation. 
     
     Also it can return errors returned by CreateInstance of internal dependency classes.  

Comments:
   This routine does a private or public encrypt based on how the
   caller has configured the IRSA object beforehand and the padding
   type the user specifies.

   The input data, pbIn, must be valid until the callback is called.
   The data is not copied.

   ePadType must be one of the following:

   AEE_RSA_PKCS1TYPE1_PADDING
      - PKCS #1 v1.5 padding. This currently is the most widely used mode.

   AEE_RSA_PKCS1TYPE2_PADDING
      - PKCS #1 v2.0 padding. This currently is the most widely used mode.

   AEE_RSA_SSLV23_PADDING
      - PKCS #1 v1.5 padding with an SSL-specific modification that denotes 
        that the server is SSL3 capable

   AEE_RSA_PKCS1_OAEP_SHA1_MGF1_PADDING
    - PKCS #1 v2.1 padding with OAEP
    - This padding type added in Brew 3.1.4

   AEE_RSA_NO_PADDING
      - No padding used.  Application should only use cryptographically sound 
        padding modes

   The operation may be cancelled by calling CALLBACK_Cancel on pCB.

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

=======================================================================

IRSA_Decrypt()

Description:
   This routine decrypts given data using the exponent and modulus specified
   previously using the IRSA_Init().  It strips the padding according the the
   pad type parameter, and returns the plaintext data to the user.

Prototype:
   void IRSA_Decrypt
   (
      IRSA* pIRSA,
      const byte* pbIn,
      int cbIn,
      byte* pbOut,
      int *pcbOut,
      int ePadType,
      uint32* pdwResult,
      AEECallback *pCB
   )

Parameters:
   pIRSA: Pointer to the IRSA interface
   pbIn:    [in]     pointer to buffer of data to decrypt
   cbIn:    [in]     length of input buffer
   pbOut:   [out]    pointer to buffer to store encrpypted data, caller allocated
   pcbOut:  [in/out] length of output buffer on input, length of data on output
   ePadType:  [in]   type of padding to use
   pdwResult: [out]  result code
   pCB:        [in]  completion callback

Return Value:
None.  Result codes are returned through the pdwResult parameter.

     SUCCESS: if task is successful
	 
     AEE_CRYPT_INVALID_KEY:  key not set, Init not called
	 
     AEE_CRYPT_INVALID_SIZE:  
		cbIn is not equal to the modulus size
		or pbIn is bigger than the modulus
								   
     ENOMEMORY:  ran out of memory while processing
	 
     AEE_CRYPT_BUFFER_TOO_SMALL:  output buffer to small
	 
     AEE_CRYPT_INVALID_PADTYPE:  padding type given was wrong
	 
     AEE_CRYPT_ROLLBACKATTACK:  when decoding SSL padding
	 
     AEE_CRYPT_PAD_ERROR:  
		after decryption, padding decoding failed
		(probably means decryption was with wrong 
		key or otherwise failed)
	
     ECLASSNOTSUPPORT: 
		Internal depedency failure, unable to 
		instantiate an object of internal class
		needed to complete the operation. 
     
     Also it can return errors returned by CreateInstance of internal 
     dependency classes.  


  Comments:
   This routine does a private or public decrypt based on how the
   caller has configured the IRSA object beforehand and the padding
   type the user specifies. With typical key pairs decrypt is usually
   a private operation and is very slow.

   The input data, pbIn, must be valid until the callback is called.
   The data is not copied.

   ePadType should be as described in IRSA_Encrypt().

   The operation may be cancelled by calling CALLBACK_Cancel() on pCB.

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

=======================================================================

IRSA_VerifySig()

Description:
   Verify a PKCS1 format signature.  This routine takes a signature and
   a digest, and verifies that the signature is valid. It decrypts the
   signature and strips padding decodes any underlying signature
   formatting (e.g. PKCS1) and compares the digest signature type.

Prototype:
   void IRSA_VerifySig
   (
      IRSA* pIRSA,
      const byte* pbSig,
      int cbSigLen,
      const byte* pbDigest,
      int cbDigestLen,
      int eSigType,
      uint32* pdwResult,
      AEECallback *pCB
   )

Parameters:
      pIRSA: pointer to the RSA object
      pbSig:      [in]  pointer to a buffer containing a signature
      cbSigLen:   [in]  the length of the signature
      pbDigest:   [in]  pointer to a buffer containing the expected digest
      cbDigestLen:[in]  the length of the digest
      eSigType:   [in]  One of AEE_RSA_SIG_
      pdwResult:  [out] a result code
      pCB:        [in]  pointer to a user defined callback routine

Return Value:
None.  Result codes are placed in the pdwResult parameter.

     SUCCESS: if task is successful
	 
     ENOMEMORY:  ran out of memory while processing
	 
     EBADPARM:  Init not called, signature is invalid
	 
     AEE_CRYPT_INVALID_KEY:  key not set, Init not called
	 
     AEE_CRYPT_BUFFER_TOO_SMALL:  output buffer to small
	 
     AEE_CRYPT_INVALID_PADTYPE:  padding type given was wrong
	 
     AEE_CRYPT_ROLLBACKATTACK:  when decoding SSL padding
	 
     SIG_INVALID:  signature verify failed
	 
     AEE_CRYPT_PAD_ERROR: 
		after decryption, padding decoding failed
		(probably means decryption was with wrong 
		key or otherwise failed)
	
     ECLASSNOTSUPPORT:  
		Internal depedency failure, unable to 
		instantiate an object of internal class
		needed to complete the operation. 
     
Also it can return errors returned by CreateInstance of internal 
     dependency classes.                          

Comments:

   eSigType must be one of:

   AEE_RSA_SIG_PKCS1_MD2
      - MD2 with RSA encryption as defined in PKCS#1 RFC2313

   AEE_RSA_SIG_PKCS1_MD5
      - MD5 with RSA encryption as defined in PKCS#1 RFC2313

   AEE_RSA_SIG_PKCS1_SHA1
      - SHA1 with RSA encryption as defined in PKCS#1 RFC2313
      - This signature type was added in Brew 3.1.4
   
   AEE_RSA_SIG_PKCS1_PSS_SHA1_MGF1
      - PSS mode SHA1 as defined by PKCS #1 v2.1. 
      - This signature type was added in Brew 3.1.4
      - This mode is a special case as it does not deal with OID's. 

   This is only used to compare with the signature type encoded in the
   PKCS1 padding. The comparison allows for the algorithm OID in the PKCS1
   signature format to be from one of several OID spaces that identify
   the above algorihms.

   The signature type is required because it is part of the
   data signed and thus needs to be checked.

   If you want to verify signatures not based on PKCS1 with type 1 padding, use
   IRSA_Decrypt() and compare its output with the expected value or hash.

   The input data, pbSig, must be valid until the callback is called.
   The data is not copied.

   The operation may be cancelled by calling CALLBACK_Cancel() on pCB.

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   IRSA_Decrypt()

=============================================================================*/
