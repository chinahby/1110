#ifndef AEERAWBLOCKCIPHER_H
#define AEERAWBLOCKCIPHER_H
/*======================================================
FILE:  IRawBlockCipher.h

SERVICES:  Raw Block Cipher Interface

DESCRIPTION: This file provides definitions for the minimal interface 
required for a block cipher implementation and made available to cipher developers.

IRawBlockCipher - minimum interface required for a block cipher.  As long
as a block cipher supports at least this interface, the ICipherFactory and
ICipherWrapper classes can provide a full ICipher1 implementation.

The interface is required by the ICipherFactory in order to assemble
a complete cipher from the ICipher1 and ICipherWrapper wrapper classes.

CLASS IDs:
            AEEIID_RAWBLOCKCIPHER
            
Copyright © 1999-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

#include "IParameters.h"
#include "ICipher1.h"

//************************************************************************************************
//
// IRawBlockCipher Interface
// This interface is the minimal interface required to implement a block cipher.
// Stream ciphers need only support the ICipher1 interface.
// 
//************************************************************************************************

#define AEEIID_RAWBLOCKCIPHER    	0x0102d154

// Define IRawBlockCipher derived from IParameters
typedef struct IRawBlockCipher IRawBlockCipher;

AEEINTERFACE(IRawBlockCipher)
{
   INHERIT_IParameters(IRawBlockCipher);
   int      (*ProcessBlocks)(IRawBlockCipher* po, const byte* pbIn, unsigned cbIn, byte* pbOut);
};

static __inline uint32 IRAWBLOCKCIPHER_AddRef(IRawBlockCipher *me)
{
   return AEEGETPVTBL(me,IRawBlockCipher)->AddRef(me);
}

static __inline uint32 IRAWBLOCKCIPHER_Release(IRawBlockCipher *me)
{
   return AEEGETPVTBL(me,IRawBlockCipher)->Release(me);
}

static __inline uint32 IRAWBLOCKCIPHER_QueryInterface(IRawBlockCipher *me,
                                              AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,IRawBlockCipher)->QueryInterface(me, cls, ppo);
}

static __inline int IRAWBLOCKCIPHER_GetParam(IRawBlockCipher *me,
                                     int id, void *pParam, unsigned *puParamLen)
{
   return AEEGETPVTBL(me,IRawBlockCipher)->GetParam(me, id, pParam, puParamLen);
}

static __inline int IRAWBLOCKCIPHER_SetParam(IRawBlockCipher *me,
                                         int id, const void *pParam,
                                         unsigned uParamLen)
{
   return AEEGETPVTBL(me,IRawBlockCipher)->SetParam(me, id, pParam, uParamLen);
}

static __inline int IRAWBLOCKCIPHER_ProcessBlocks(IRawBlockCipher* po, 
                                    const byte* pbIn, unsigned cbIn, byte* pbOut)
{
   return AEEGETPVTBL(po,IRawBlockCipher)->ProcessBlocks(po, pbIn, cbIn, pbOut);
}

/*=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

IRawBlockCipher Interface

   IRawBlockCipher is primarily intended as an interface by which implentors of ciphers, block
   ciphers in particular, make their implementation available. A caller or user of ciphers
   should use ICipher1 and ICipherFactory and not call IRawBlockCipher directly.

   An IRawBlockCipher provides the interface used by ICipherFactory 
   and ICipherWrapper to create a complete cipher implementation.
   
   By implementing the IRawBlockCipher, the implementor can leave the buffering
   and padding to the ICipherWrapper and can implement just a subset
   of the chaining modes.  The ICipherFactory will query the cipher and attach
   the necessary ICipherWrapper to complete the implementation.

   An implementation of IRawBlockCipher does not have to support any chaining modes or
   padding excpet for ECB mode which basically a degenerate mode that does no 
   chaining or padding. When the chaining mode parameter is fetched via GetParam, 
   it should say it supports ECB mode. It may support other modes too.

The following header file is required:~
   IRawBlockCipher.h 

==============================================================================
Function: IRAWBLOCKCIPHER_AddRef()

Description:
	This function is inherited from IQI_AddRef().

==============================================================================
Function: IRAWBLOCKCIPHER_Release()

Description:
	This function is inherited from IQI_Release().
	
==============================================================================
Function: IRAWBLOCKCIPHER_QueryInterface()

Description:
	This function is inherited from IQI_QueryInterface().
	
==============================================================================

IRAWBLOCKCIPHER_GetParam()

Description:
   Retrieves information about the Cipher.  This method takes an
   ID that specifies what information is provided. The pParam contains
   the information.  See CIPHER_PARAM_* for details on the supported
   parameters.  An IRawBlockCipher must support the parameters:
~   
	  CIPHER_PARAM_DIRECTION

      CIPHER_PARAM_KEY_SIZE

      CIPHER_PARAM_IV_SIZE    if supporting modes other than ECB

      CIPHER_PARAM_BLOCKSIZE
*
   
Prototype:
   int IRAWBLOCKCIPHER_GetParam(IRawBlockCipher* pIRawBlockCipher, int id, void *pParam, unsigned *pcParam)

Parameters:
   pIRawBlockCipher:  Pointer to IRawBlockCipher interface object
   id:      Type of parameter to fetch
   pParam:  Pointer to a structure for storing the results.  If the
            pointer is NULL, pcParam contains the size required
            for storing the parameter.
            
   pcParam: ON input, size of structure passed in.  On return,
            contains the actual size of the return results

Return Value:

   SUCCESS:    Parameter fetched successfully
   EBADPARM:    Invalid parameter - bad set type

Comments:
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   CIPHER_PARAM_XXX
   IRAWBLOCKCIPHER_SetParam

==============================================================================

IRAWBLOCKCIPHER_SetParam()

Description:
   Sets information required by the IRawBlockCipher.  This method takes an
   ID that specifies what information is provided. The pParam contains
   the information.  See CIPHER_PARAM_* for details on the supported
   parameters.  An IRawBlockCipher should support the parameters:
~
      CIPHER_PARAM_DIRECTION

      CIPHER_PARAM_KEY

      CIPHER_PARAM_IV         if supporting modes other than ECB

      CIPHER_PARAM_MODE       should support at least ECB
*

Prototype:
   int IRAWBLOCKCIPHER_SetParam(IRawBlockCipher* pIRawBlockCipher, int id, void *pParam, int *pcParam)

Parameters:
   pIRawBlockCipher:  Pointer to IRawBlockCipher interface object
   id:      Type of parameter to set
   pParam:  Pointer to a structure containing the value to set.
            
   cParam: size of structure passed in.

Return Value:

   SUCCESS:     Parameter fetched successfully
   EBADPARM:    Invalid parameter - bad set type or pParam is null.

Comments:
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   CIPHER_PARAM_XXX
   IRAWBLOCKCIPHER_GetParam

======================================================================= 

IRAWBLOCKCIPHER_ProcessBlocks()

Description:
   Encrypt or decrypt multiple blocks. The input count must be a multiple
   of the blocksize.  The output
   buffer must have enough space to hold the input count.  The
   input and output pointers may point to the same area.

Prototype:
   int IRAWBLOCKCIPHER_ProcessBlocks(IRawBlockCipher* pIRawBlockCipher, 
               const byte *pbIn, unsigned cbIn, byte *pbOut)

Parameters:
   pIRawBlockCipher: Pointer to IRawBlockCipher interface object
   pbIn:[in]  Pointer to data to be encrypted/decrypted
   cbIn: [in]  count of bytes to be encrypted (in multiples of block size)
   pbOut: [out]     Buffer to receive encrypted/decrypted data

Return Value:
   SUCCESS: block encrypted
   EBADSTATE: cipher is not properly initialized.
   EBADPARM: invalid parameters - null pointers or invalid count.

Comments:
   None    

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:

=======================================================================*/
#endif

