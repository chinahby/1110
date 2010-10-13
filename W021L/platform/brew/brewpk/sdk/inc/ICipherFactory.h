#ifndef ICIPHERFACTORY_H
#define ICIPHERFACTORY_H
/*======================================================
FILE:  ICipherFactory.h

SERVICES:  Cipher Factory Interface

DESCRIPTION: The ICipherFactory is the mechanism for creating
an ICipher1 with the the required features.

ICipherFactory - creates ICipher1 interface per requested features.

Copyright © 2004-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

#include "AEEQueryInterface.h"
#include "ICipher1.h"

//****************************************************************************
//
// ICipherFactory Interface
//
//****************************************************************************


typedef struct CipherInfo {
   AEECLSID    cipherId;            // Class ID of algorithm
   AEECLSID    cipherModeId;        // Class ID of chaining mode
   int         padding;             // CIPHER_PADDING_XXX
   int         encryptDirection;    // CIPHER_DIRECTION_XXX
   byte        *pKey;               // pointer to key
   unsigned    cKey;                // key size
   byte        *pIV;                // pointer to Initialization Vector
   unsigned    cIV;                 // Initialization Vector size
} CipherInfo;

typedef struct ICipherFactory ICipherFactory;

AEEINTERFACE(ICipherFactory)
{
   INHERIT_IQueryInterface(ICipherFactory);
   int (*CreateCipher)(ICipherFactory *po, AEECLSID cipher, int direction,
                        AEECLSID mode, int padding,ICipher1 **ppCipher);
   int (*CreateCipher2)(ICipherFactory *po, 
                        CipherInfo *pInfo, unsigned cInfoSize, ICipher1 **ppCipher);
   int (*QueryCipher)(ICipherFactory *po,
                        AEECLSID cipher, AEECLSID mode, int padding, unsigned keysize);
};

static __inline uint32 ICIPHERFACTORY_AddRef(ICipherFactory *me)
{
   return AEEGETPVTBL(me,ICipherFactory)->AddRef(me);
}

static __inline uint32 ICIPHERFACTORY_Release(ICipherFactory *me)
{
   return AEEGETPVTBL(me,ICipherFactory)->Release(me);
}

static __inline uint32 ICIPHERFACTORY_QueryInterface(ICipherFactory *me,
                                              AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,ICipherFactory)->QueryInterface(me, cls, ppo);
}

static __inline int ICIPHERFACTORY_CreateCipher(ICipherFactory *po,
                        AEECLSID cipher, int direction, AEECLSID mode,
                        int padding,ICipher1 **ppCipher)
{
   return AEEGETPVTBL(po,ICipherFactory)->CreateCipher(po,
                        cipher, direction, mode, padding, ppCipher);
}

static __inline int ICIPHERFACTORY_CreateCipher2(ICipherFactory *po,
                        CipherInfo *pInfo, unsigned cInfoSize, ICipher1 **ppCipher)
{
   return AEEGETPVTBL(po,ICipherFactory)->CreateCipher2(po,
                        pInfo, cInfoSize,ppCipher);
}

static __inline int ICIPHERFACTORY_QueryCipher(ICipherFactory *po,
                        AEECLSID cipher, AEECLSID mode, int padding, unsigned keysize)
{
   return AEEGETPVTBL(po,ICipherFactory)->QueryCipher(po,
                        cipher, mode, padding, keysize);
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

CipherInfo

Description:
CipherInfo holds all of the information required to initialize an ICipher1.

Definition:
typedef struct CipherInfo {
   AEECLSID    cipherId;
   AEECLSID    cipherModeId;
   int         padding;
   int         encryptDirection;
   byte        *pKey;
   unsigned    cKey;
   byte        *pIV;
   unsigned    cIV;
} CipherInfo;

Members:
   cipherId       : Specifies class ID of algorithm
   cipherModeId   : Specifies class ID of chaining mode. This should be zero for
                    non-block (streaming) cipher algorithms.
   padding        : Specifies padding for block modes.  May be zero for
                    streaming cipher algorithm. See CIPHER_PADDING_XXX.
   encryptDirection : Identifies direction.  See CIPHER_DIRECTION_XXX
   pKey           : pointer to key.  If this is null, the key is not initialized
   cKey           : key size.  If this is zero, the key is not initialized
   pIV            : pointer to Initialization Vector.  If this is null, the
                    IV is not intialized.
   cIV            : Initialization Vector size.  IF zero, the IV is not
                    initialized

Comments:

See Also:
   ICipherFactory

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

ICipherFactory Interface

   Because of the number of options when creating the correct algorithm
   and modes for an ICipher1, the ICipherFactory is used to instanciate
   ICipher1s rather than using CreateInstance().  The factory selects from
   the available implementations the correct algorithm, mode and padding
   features.

   Each ICipher1 algorithm (i.e. DES, AES) may have multiple implementations,
   depending on the platform. Ciphers may be implemented in software, by a
   DSP or dedicated hardware. Implementations are not required to support
   all of the possible chaining modes or padding styles. The factory (in
   conjunction with the ICipherWrapper classes) determines how to best
   assemble the cipher components to provide the requested functionality.
   
   For each cipher algorithm there is an abstract 'best fit' class Id 
   which the factory uses to select the best available implementation 
   for the current platform (generally this implies the fastest 
   implementation).  This allows application developers to more easily 
   develop platform independent code.
   
See Also:
   ICipher1
   ICipherWrapper
   BLOCK_CIPHER.BID
   STREAM_CIPHER.BID
   CIPHERWRAPPER.BID

The following header file is required:~
   ICipherFactory.h

=======================================================================
Function: ICIPHERFACTORY_AddRef()

Description:
	This function is inherited from IQI_AddRef().

=======================================================================
Function: ICIPHERFACTORY_Release()

Description:
	This function is inherited from IQI_Release().

=======================================================================
Function: ICIPHERFACTORY_QueryInterface()

Description:
	This function is inherited from IQI_QueryInterface().
=======================================================================

ICIPHERFACTORY_CreateCipher()

Description:
   Creates a cipher with the specified characteristics.

Prototype:
   int ICIPHERFACTORY_CreateCipher(ICipherFactory *pICipherFactory,
                   AEECLSID cipher, int direction, AEECLSID mode,
                   int padding, ICipher1 **ppCipher);

Parameters:
   pICipherFactory:    Pointer to ICipherFactory interface object
   cipher:  Class ID of requested cipher algorithm
   direction: encryption direction.  See CIPHER_DIRECTION_XXX.
   mode:    Class ID of requested block chaining mode algorithm.  For stream ciphers
            this parameter should be 0.
   padding: Type of padding to use for block ciphers.  For stream ciphers or 
            streaming block modes, this parameter is ignored.
   ppCipher[out]: pointer to returned ICipher1 handle
           
Return Value:
   SUCCESS: class created. 

   ENOMEMORY: insufficient memory. 

   ECLASSNOTSUPPORT: class specified is not supported. 

Comments:
   None.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1.3

See Also:
   CIPHER_PADDING_XXX
   ICipher1

======================================================================= 

ICIPHERFACTORY_CreateCipher2()

Description:
   Creates a cipher with the characteristics specified in a CipherInfo
   structure.

Prototype:
   int ICIPHERFACTORY_CreateCipher2(ICipherFactory *pICipherFactory,
                   CipherInfo *pCipherInfo, unsigned cInfoSize, ICipher1 **ppCipher);

Parameters:
   pICipherFactory:  Pointer to ICipherFactory interface object
   pCipherInfo:      Pointer to information on type of cipher to create.
   cInfoSize:        Size of structure pointed to by pCipherInfo
   ppCipher[out]:    pointer to returned ICipher1 handle
           
Return Value:
   SUCCESS: class created. 

   ENOMEMORY: insufficient memory. 

   ECLASSNOTSUPPORT: class(es) specified are not supported. 

   EBADPARM: invalid parameter or invalid field for CipherInfo.
   
   AEE_CRYPT_INVALID_KEY: key is wrong length or otherwise invalid.
   
   
Comments:
   Not all of the fields in CipherInfo are required for all ciphers.
   If the CipherInfo field cKey is zero or pKey is null, the key is not 
   initialized.
   If the CipherInfo field cIV is zero or pIV is null, the IV is not
   initialized.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   ICIPHERFACTORY_CreateCipher()
   CipherInfo
   ICipher1

=======================================================================

ICIPHERFACTORY_QueryCipher()

Description:
   Checks the factory to determine if a cipher with the specified
   characteristics can be created.  This may be used, for example,
   by a service that needs to valid a database of available ciphers
   (such as SSL) without explicitly creating the cipher.

Prototype:
   int ICIPHERFACTORY_QueryCipher(ICipherFactory *pICipherFactory,
                   AEECLSID cipher, AEECLSID mode,
                   int padding, unsigned keyLen);

Parameters:
   pICipherFactory:    Pointer to ICipherFactory interface object
   cipher:  Class ID of requested cipher algorithm
   mode:    Class ID of requested block chaining mode algorithm.  For stream ciphers
            this parameter is ignored and should be 0.
   padding: Type of padding to use for block ciphers.  For stream ciphers or 
            streaming block modes, this parameter is ignored.
   keyLen: Required key length in bytes.  Unless an unusual key length is required,
            this value may be 0 to skip this check.
           
Return Value:
   SUCCESS: cipher can be created. 

   ENOMEMORY: insufficient memory. 

   ECLASSNOTSUPPORT: cipher is not supported as specified.  This could mean that
               the algorithm is not available, the specific mode or padding is 
               not supported or the key length is not supported.
               
Comments:
   None.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1.3

See Also:
   CIPHER_PADDING_XXX
   ICipher1

=======================================================================*/
#endif

