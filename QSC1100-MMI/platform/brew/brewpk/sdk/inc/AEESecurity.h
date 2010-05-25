#ifndef AEESECURITY_H
#define AEESECURITY_H
/*======================================================
FILE:  AEESecurity.h

SERVICES:  AEE Security Interface

DESCRIPTION: This file provides definitions for the Security interfaces made available by
the AEE to application developers. This is a standard header file that must be included by
all applications using the security services of the AEE.

There are 3 interfaces

IHash - Does MD5, MD2, and SHA1.
        Also Supports HMAC for all of the above
ICipher - Does ArcFour (Stream cipher), DES, 3DES (block ciphers)


CLASS IDs:	AEECLSID_MD5
            AEECLSID_MD2
            AEECLSID_SHA1
            AEECLSID_SHA1CTX
            AEECLSID_HMAC_SHA1
            AEECLSID_HMAC_SHA1CTX
            AEECLSID_ARC4
            AEECLSID_DES_ENCRYPT
            AEECLSID_DES_DECRYPT
            AEECLSID_3DES_ENCRYPT
            AEECLSID_3DES_DECRYPT

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEE.h"

#include "hmac.h"

//************************************************************************************************
//
// IHashCtx
//
//************************************************************************************************

// IHashCTX Object
typedef struct IHashCTX	IHashCTX;
AEEINTERFACE(IHashCTX)
{
   INHERIT_IQueryInterface(IHashCTX);
   void		(*Init)(IHashCTX* po, void *pCTX, int nCTXSize);
   void		(*Update)(IHashCTX* po, void *pCTX, int nCTXSize, const void* pData, int nDataLen);
   int		(*Final)(IHashCTX* po, void *pCTX, int nCTXSize, byte* pbData, int* pcbData);
   int		(*SetKey)(IHashCTX *po, void *pCTX, int nCTXSize, const byte *pbKey, int cbKey);
};

#define IHASHCTX_AddRef(p)							AEEGETPVTBL((p),IHashCTX)->AddRef(p)
#define IHASHCTX_Release(p)						AEEGETPVTBL((p),IHashCTX)->Release(p)
#define IHASHCTX_QueryInterface(p,i,o)       AEEGETPVTBL((p),IHashCTX)->QueryInterface((p),(i),(o))
#define IHASHCTX_Init(p,pc,nc)  			      AEEGETPVTBL((p),IHashCTX)->Init(p,pc,nc)
#define IHASHCTX_Update(p,pc,nc,pv,vl)   	   AEEGETPVTBL((p),IHashCTX)->Update(p,pc,nc,pv,vl)
#define IHASHCTX_Final(p,pc,nc,pd,dl)			AEEGETPVTBL((p),IHashCTX)->Final(p,pc,nc,pd,dl)
#define IHASHCTX_SetKey(p,pc,nc,pbk,cb)         AEEGETPVTBL((p),IHashCTX)->SetKey(p,pc,nc,pbk,cb)
//
// MD5 Hash Context
//

// Note: OEMMD5_CTX is defined in OEMMD5.h.  This structure is a copy of that
// same structure, and must be in order for the IHashCTX interface not to crash.
// If you change something in the AEE_MD5_STRUCTURE, be sure to make the same change
// in the OEMMD5_CTX structure as well.
//
// A compile time check in AEESecurity.c compares the sizes of these two structures.
// If they differ, you will see a compile warning while compiling AEESecurity.c.

typedef struct
{
  uint32 state[4];
  uint32 count[2];
  uint8  buffer[64];
} AEE_MD5_CTX;


//
// MD2 Hash Context
//

#define MD2_DIGEST_LENGTH	16
#define MD2_BLOCK       	16
#if !defined(MD2_INT)
#define MD2_INT unsigned int
#endif
typedef struct
{
	int num;
	unsigned char data[MD2_BLOCK];
	MD2_INT cksm[MD2_BLOCK];
	MD2_INT state[MD2_BLOCK];
} AEE_MD2_CTX;


//
// SHA1 Hash Context
//

typedef struct
{
    uint32 state[5];
    uint32 count[2];
    uint8  buffer[64];
    uint8  scratchbuffer[64];    // added so we don't have to step on data when hashing more than 64 bytes.
} AEE_SHA1_CTX;


//
// HMAC Specific Structures
//

//
// MD5 HMAC Hash Context
//

typedef struct
{
   AEE_MD5_CTX md5_ctx;
   HMacData m_stHMac;
} AEE_MD5_HMAC_CTX;


//
// MD2 HMAC Hash Context
//

typedef struct
{
   AEE_MD2_CTX md2_ctx;
   HMacData m_stHMac;
} AEE_MD2_HMAC_CTX;


//
// SHA1 Hash Context
//

typedef struct
{
   AEE_SHA1_CTX sha1_ctx;
   HMacData m_stHMac;
} AEE_SHA1_HMAC_CTX;

//
// General Hash Context
//

typedef struct
{
   union
   {
      AEE_MD5_CTX md5_ctx;
      AEE_MD2_CTX md2_ctx;
      AEE_SHA1_CTX sha1_ctx;
      AEE_MD5_HMAC_CTX md5_hmac_ctx;
      AEE_MD2_HMAC_CTX md2_hmac_ctx;
      AEE_SHA1_HMAC_CTX sha1_hmac_ctx;
   } ctx;
} AEE_HASH_CTX;

//************************************************************************************************
//
// IHash Interface
//
//************************************************************************************************

// IHash Object
typedef struct _IHash		IHash;

#define MD5_LENGTH	16	   // Length of MD5 hash  (16 * 8 == 128 bits)
#define MD2_LENGTH	16    // Length of MD2 hash  (16 * 8 == 128 bits)
#define SHA1_LENGTH   20   // Length of SHA1 hash (20 * 8 == 160 bits)

QINTERFACE(IHash)
{
   DECLARE_IBASE(IHash)

   void		(*Update)(IHash* po, const byte* pbData, int cbData);
   int		(*GetResult)(IHash* po, byte* pbData, int* pcbData);
   void		(*Restart)(IHash* po);
   int		(*SetKey)(IHash *po, const byte *pbKey, int cbKey);
};

#define IHASH_AddRef(p)								GET_PVTBL(p,IHash)->AddRef(p)
#define IHASH_Release(p)							GET_PVTBL(p,IHash)->Release(p)
#define IHASH_Update(p, pbd, cb)					GET_PVTBL(p,IHash)->Update(p, pbd, cb)
#define IHASH_GetResult(p, pbd, pcb)			GET_PVTBL(p,IHash)->GetResult(p, pbd, pcb)
#define IHASH_Restart(p)							GET_PVTBL(p,IHash)->Restart(p)
#define IHASH_SetKey(p, pbk, cb)             GET_PVTBL(p,IHash)->SetKey(p, pbk, cb)

//************************************************************************************************
//
// ICipher Interface
//
//************************************************************************************************

typedef struct _ICipher ICipher;

// structure for Cipher initialization
typedef struct _AEECipherInit
{
   const byte *pbIV;
   int cbIV;
   const byte *pbKey1;
   int cbKey1;
   const byte *pbKey2;
   int cbKey2;
   const byte *pbKey3;
   int cbKey3;
} AEECipherInit;

QINTERFACE(ICipher)
{
   DECLARE_IBASE(ICipher)

   int      (*Init)(ICipher* po, const byte* pbKey, int cbKey);
   int      (*Cipher)(ICipher* po, const byte* pbIn, int cbIn, byte* pbOut, int* pcbOut);
   void     (*Flush)(ICipher* po);
};

#define ICIPHER_AddRef(p)                         GET_PVTBL(p,ICipher)->AddRef(p)
#define ICIPHER_Release(p)                        GET_PVTBL(p,ICipher)->Release(p)
#define ICIPHER_Init(p, pbk, cb)                  GET_PVTBL(p,ICipher)->Init(p, pbk, cb)
#define ICIPHER_InitCipher(p, pi)                 GET_PVTBL(p,ICipher)->Init(p, (const byte*)pi, 0)
#define ICIPHER_Cipher(p,pbin,cbin,pbout,pcbout)  GET_PVTBL(p,ICipher)->Cipher(p,pbin,cbin,pbout,pcbout)
#define ICIPHER_Flush(p)                          GET_PVTBL(p,ICipher)->Flush(p)

#endif


/*
=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IHashCTX

Description:
   IHashCTX is an interface to cryptographic hashes. MD5, MD2, and SHA1
   are available with this interface. This interface can be used in a way
   similar to the common open source implementations of these hashes where
   the caller allocates and manages the storages for the hash's internal
   context. For a given type of hash you can create only one instance
   of this interface in your app and use it with multiple contexts.

   This interface also supports HMAC as specified by RFC 2104. HMAC is
   supported for MD5, MD2, and SHA1. When using HMAC you must call
   IHASHCTX_SetKey().

   Which hash functions is provided depends on the the class ID passed
   to ISHELL_CreateInstance() to instantiate the interface. The following
   are supported:
~
      AEECLSID_MD5CTX~
      AEECLSID_MD2CTX~
      AEECLSID_SHA1CTX~
      AEECLSID_HMAC_MD5CTX~
      AEECLSID_HMAC_MD2CTX~
      AEECLSID_HMAC_SHA1CTX~
*


   The IHashCTX interface is a lot like the IHash interface, except the context
   is not a part of the IHashCTX object.

Functions in this interface include:
~
   IHASHCTX_AddRef() ~
   IHASHCTX_Release() ~
   IHASHCTX_QueryInterface()~
   IHASHCTX_Init() ~
   IHASHCTX_Update() ~
   IHASHCTX_Final() ~
   IHASHCTX_SetKey()~
*

The following header files are required:~
   AEEClassIDs.h~
   AEESecurity.h~

Version:
   Introduced BREW Client 2.0
   HMAC classes introduced in Brew 3.1

==============================================================================

IHASHCTX_AddRef()

Description:
    This function is inherited from IBASE_AddRef().

See Also:
    IHASHCTX_Release()

=======================================================================

IHASHCTX_Release()

Description:
    This function is inherited from IBASE_AddRef().

See Also:
    IHASHCTX_AddRef()

===========================================================================

IHASHCTX_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface().

See Also:
    None

=======================================================================

IHASHCTX_Init()

Description:
    This function should be called before hashing or to restart the
    hashing process. This is required if you need to hash new streams
    of data.
    	
Prototype:
	 void		IHASHCTX_Init(IHashCTX* po, void *pCTX, int nCTXSize);
	
Parameters:
    po       : Pointer to the IHashCTX Interface object
    pCTX     : pointer to a hash context structure
    nCTXSize : the size of the hash context structure

Return Value:
   None

Comments:
    This must be called before any calls to IHASHCTX_Update() to initialize the
    user-allocated context.

    The context pointed to by pCTX must be equal or greater in size than the
    required size based on the algorithm.  If the context is too small the
    calls to IHASHCTX_Init() and IHASHCTX_Update() will fail silently and
    IHASHCTX_Final() will return the error code AEE_HASH_INVALID_CTX.

    You can use one of AEE_MD2_CTX, AEE_MD5_CTX, AEE_SHA1_CTX,
    AEE_MD5_HMAC_CTX, AEE_MD2_HMAC_CTX or AEE_SHA1_HMAC_CTX for pCTX based on
    the IHASHCTX class id. If in doubt, use AEE_HASH_CTX size buffer for pCTX,
    which is a union of these structures.

Side Effects:
   None

See Also:
	None

==================================================================

IHASHCTX_Update()

Description:
    This function should be used to feed data to the hash object.
	 This function can be called multiple times to hash long streams or
	 discontinous streams of data.

Prototype:
	 void	IHASHCTX_Update(
      IHashCTX* po,
      void *pCTX,
      int nCTXSize,
      const void* pData,
      int nDataLen);

Parameters:
    po:            Pointer to the IHashCTX Interface object
    pCTX:          pointer to a hash context structure
    nCTXSize:      the size of the hash context structure
    pData: [in]    Pointer to the data to be hashed
    nDataLen: [in] Length of the data to be hashed(stored in pbData)

Return Value:
   None

Comments:
	None

See Also:
	None

=======================================================================

IHASHCTX_Final()

Description:
    This function should be called to get the hashed result after all
	 the calls to IHASHCTX_Update().
	
Prototype:
	 int	IHASHCTX_Final(
      IHashCTX* po,
      void *pCTX,
      int nCTXSize,
      byte* pbData,
      int* pcbData);
	
Parameters:
    po:               Pointer to the IHashCTX Interface object
    pCTX:             pointer to a hash context structure
    nCTXSize:         the size of the hash context structure
    pbData:[out]      Pointer to the data to get the hashed result
    pcbData: [in/out] Pointer to the size of the buffer pbData. On return, this contains
				          the size of the hashed result.

Return Value:
   SUCCESS : if succussful

   AEE_HASH_MORE_DATA   : Buffer not large enough to hold hashed result.

   AEE_HASH_INVALID_CTX : if nCTXSize is less than the required context size

Comments:
	None

See Also:
	None

=======================================================================

IHASHCTX_SetKey()

Description:
	Initializes key for HMAC keyed hashes.

Prototype:
	int IHASHCTX_SetKey(
      IHashCTX* po,
      void *pCTX,
      int nCTXSize,
      const byte* pchKey,
      int cbKey)

Parameters:
   po:               Pointer to the IHashCTX Interface object
   pCTX:             pointer to a hash context structure
   nCTXSize:         the size of the hash context structure
	pchKey:[in]        Pointer to key
	cbKey:[in]        key size in bytes

Return Value:
	SUCCESS: key initialization successful.

	AEE_CRYPT_INVALID_KEY: Invalid key

	ENOMEMORY: Memory error					

Comments:
	This must be called before any call to IHASHCTX_Update(). The result
   returned are undefined if you call the IHashCTX functions out of
   the usual order of IHASHCTX_SetKey(), IHASHCTX_Update() and
   IHASHCTX_Final().  You must call IHASHCTX_SetKey() after calling
   IHASHCTX_Init() with HMAC hashes. The key is not retained internally.

   To reuse an IHashCTX instance for a new hash with HMAC you must call
   IHASHCTX_Init() and IHASHCTX_SetKey() in that order.


   It is only used with HMAC hashes. It has no affect on non-
   HMAC hashes. Whether a hash is HMAC or not depends on
   the class ID used to create the inteface instance.

Version:
   Introduced in Brew 3.1 (calling this function on
   earlier versions of Brew will cause your app to crash)

See Also:
	None

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IHash

Description:
   IHash is an interface to cryptographic hashes. MD5, MD2 and SHA1
   are available with this interface. With this interface the internal
   context used by the hash function is allocated with the instance. There
   is no need to allocate it. Also see IHashCTX where the context is not
   allocated.

   This interface also supports HMAC as specified by RFC 2104. HMAC is
   supported for MD5, MD2, and SHA1. When using HMAC you must call
   IHASH_SetKey().

   Which hash functions is provided depends on the the class ID passed
   to ISHELL_CreateInstance() to instantiate the interface. The following
   are supported:
~
      AEECLSID_MD5~
      AEECLSID_MD2~
      AEECLSID_SHA1~
      AEECLSID_HMAC_MD5~
      AEECLSID_HMAC_MD2~
      AEECLSID_HMAC_SHA1~
*

The following header files are required:~
   AEEClassIDs.h~
   AEESecurity.h

Version:
   Introduced in BREW 1.0 supporting only MD5
   MD2 and SHA1 added in Brew 2.0
   HMAC classes introduced in Brew 3.1

==============================================================================

IHASH_AddRef()

Description:
    This function is inherited from IBASE_AddRef().

See Also:
    IHASH_Release()

=======================================================================

IHASH_Release()

Description:
    This function is inherited from IBASE_AddRef().

See Also:
    IHASH_AddRef()

===========================================================================

IHASH_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface().

See Also:
    None

==================================================================

IHASH_Update()

Description:
    This function should be used to feed data to the hash object.
	 This function can be called multiple times to hash long streams or
	 discontinous streams of data.

Prototype:
	 void	IHASH_Update(IHash* po,  byte* pbData, int cbData)

Parameters:
    po:         Pointer to the IHash Interface object
    pbData: [in] Pointer to the data to be hashed
    cbData: [in] Length of the data to be hashed(stored in pbData)

Return Value:
   None

Comments:
	None

See Also:
	None

=======================================================================

IHASH_GetResult()

Description:
    This function should be called to get the hashed result after all
	 the calls to IHASH_Update().
	
Prototype:
	 int	IHASH_GetResult(IHash* po,  byte* pbData, int* pcbData)
	
Parameters:
    po: Pointer to the IHash Interface object
    pbData:[out]     Pointer to the data to get the hashed result
    pcbData: [in/out] Pointer to the size of the buffer pbData. On return, this contains
				          the size of the hashed result.

Return Value:
   SUCCESS: if successful

   AEE_HASH_MORE_DATA : Buffer not large enough to hold hashed result.

Comments:
	None

See Also:
	None

=======================================================================

IHASH_Restart()

Description:
    Start a new hash.
	
Prototype:
	 void		IHASH_Restart(IHash* po);
	
Parameters:
    po : Pointer to the IHash Interface object

Return Value:
   None

Comments:
	Call this to reuse an instance of IHash for a hash over some new data.
   If the hash is an HMAC, you must call IHASH_SetKey() again.

See Also:
	None

=======================================================================

IHASH_SetKey()

Description:
	Initializes key for HMAC keyed hashes.

Prototype:
	int IHASH_SetKey(IHash* pIHash,  const byte* pchKey, int cbKey)

Parameters:
	pIHash:       Pointer to IHash interface object
	pchKey: [in]  Pointer to key
	cbKey: [in]   key size in bytes

Return Value:
	SUCCESS:	key initialization successful.
	AEE_CRYPT_INVALID_KEY:	Invalid key
	ENOMEMORY:					

Comments:
	This must be called before any call to IHASH_Update(). The result
   returned are undefined if you call the IHASH functions out of
   the usual order of IHASH_SetKey(), IHASH_Update() and IHASH_GetResult().
   You must call IHASH_SetKey() after calling IHASH_Restart() with
   HMAC hashes. The key is not retained internally.

   To reuse an IHash instance for a new hash with HMAC you must call
   IHASH_Restart() and IHASH_SetKey() in that order.

   It is only used with HMAC hashes. It has no affect on non-
   HMAC hashes. Whether a hash is HMAC or not depends on
   the class ID used to create the inteface instance.

Version:
   Introduced in Brew 3.1 (calling this function on
   earlier versions of Brew will cause your app to crash)

See Also:
	None

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

ICipher Interface


   This interface was introduced in 1.1 and deprecated in 3.1.3.
   See ICipher1 for a description of the new interface.

   The functions in ICipher enable BREW developers to perform encryption
   or decryption in a flexible manner without having to know the internals
   of the encryption algorithm.

   An instance of ICipher is created using ISHELL_CreateInstance() which
   takes a 32-bit class id as one of the input parameters. This 32-bit id
   determines the type of service that ICipher provides. If ISHELL_CreateInstance()
   succeeds, it returns an ICipher interface pointer. This interface pointer
   should be used in calls to ICipher functions. Call ICIPHER_Init() to set
   the key. After initializing the key, call ICIPHER_Cipher() to encrypt or
   decrypt data. When done with ICipher, call ICIPHER_Release().
   
The following header file is required:~
   AEESecurity.h

=======================================================================

ICIPHER_AddRef()

Description:
	This function increments the reference count of the ICipher Interface object.
   This  allows the object to be shared by multiple callers. The object is freed
   when the reference count reaches 0 (zero).

Prototype:
   uint32 ICIPHER_AddRef(ICipher * pICipher)

Parameters:
   pICipher: Pointer to the ICipher interface

Return Value:
   The update reference count.

Comments:
	None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   	None

=======================================================================

ICIPHER_Release()

Description:
	This function decrements the reference count for the ICipher object and does
   appropriate cleanup if the reference count reaches 0 (zero).

Prototype:
   uint32 ICIPHER_Release(ICipher * pICipher)

Parameters:
	pICipher: Pointer to the ICipher interface

Return Value:
	The updated reference count
	
Comments:
	None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
	None

=======================================================================

ICIPHER_Init()

Description:
	Initializes the key for encryption or decryption using ArcFour.
	ArcFour key sizes are usually 1 to 256 bytes long. The key content
	directly affects the internal state table of ArcFour because it is
	involved in the swapping of bytes in the internal table. The BREW
	implementation of ArcFour does not restrict the key size to 256 bytes.
	Call this function before calling ICipher_Cipher().

Prototype:
	int ICIPHER_Init(ICipher* pICipher,  const byte* pchKey, int cbKey)

Parameters:
	pICipher:  Pointer to ICipher interface object
	pchKey:  Pointer to key
	cbKey: key size in bytes

Return Value:

	SUCCESS:	RSA key initialization successful.

	AEE_CRYPT_INVALID_KEY:	Invalid key

	ENOMEMORY:					

Comments:
	None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
	None

=======================================================================


ICIPHER_Cipher()

Description:
	Encrypt or decrypt data using the key previously initialized.
	With stream ciphers (like ArcFour), it is recommended that the
	output buffer size be at least the length of the input data.
	Otherwise, only data of length equal to size of the output buffer
	is processed.
	
	With block ciphers, the working of this function depends
	on the specific algorithm. With DES (not supported yet) that has a
	block size of 64-bits, this function processes one block at a time and
	buffers one block size of data at both the input and output end. This allows
	progressively inputting/outputting data and allows input data length
	and output buffer sizes to be smaller than one block size. It is however
	recommended that the output buffer be at least the size of a block.

	This function returns the number of input bytes processed.
	If it returns 0(zero), no more input data can be processed unless
	previously processed data is written out. If this happens, call it again
	without changing the input. If this function is called after a call to
	ICipher_Flush(), no new input is used, only preexisting, unprocessed data
	is passed through the cipher algorithm.

Prototype:
	int ICIPHER_Cipher(ICipher* pICipher, const byte *pchIn, int cbIn, byte *pchOut, int* pcbOut)

Parameters:
	pICipher  : Pointer to ICipher interface object
	pchIn:[in] Pointer to data to be encrypted/decrypted
	cbIn: [in] Input data size in bytes
	pchOut: [out]  Buffer to receive encrypted/decrypted data
	pcbOut: [in/out]  size of output in bytes

Return Value:
	(-1):    Invalid encryption/decryption key.

	Count: Number of input bytes processed.

Comments:
	None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
	None

=======================================================================

ICIPHER_Flush()

Description:
	Use with block ciphers during the encryption process to pad the last
   unprocessed block with predetermined bytes to form a complete block. This
   function does nothing if the block is already complete (64-bits in DES).

Prototype:
   void ICIPHER_Flush(ICipher *pICipher)

Parameters:
	pICipher  : Pointer to ICipher interface object

Return Value:
   None				

Comments:
	None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
	None

=============================================================================*/

