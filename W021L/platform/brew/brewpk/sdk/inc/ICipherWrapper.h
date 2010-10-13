#ifndef ICIPHERWRAPPER_H
#define ICIPHERWRAPPER_H
/*======================================================
FILE:  ICipherWrapper.h

SERVICES:  Cipher wrapper Interface

DESCRIPTION: This file provides definitions for the ICipherWrapper
interface used by cipher developers for creating cipher
wrappers such as CBC mode and padding algorithms.

ICipherWrapper  - Wraps Block Cipher with modes of operation.


CLASS IDs:
      AEEIID_CIPHERWRAPPER

      See CIPHERWRAPPER.BID for a list of supported wrappers.

Copyright © 1999-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

#include "ICipher1.h"

//*********************************************************************
//
// ICipherWrapper Interface
//
//*********************************************************************
#define AEEIID_CIPHERWRAPPER     0x0102ccdf


// Define ICipherWrapper derived from ICipher1
typedef struct ICipherWrapper ICipherWrapper;

AEEINTERFACE(ICipherWrapper)
{
   INHERIT_ICipher1(ICipherWrapper);

   int      (*SetCipher)(ICipherWrapper* po, IQueryInterface *pCipher);
};

static __inline uint32 ICIPHERWRAPPER_AddRef(ICipherWrapper *me)
{
   return AEEGETPVTBL(me,ICipherWrapper)->AddRef(me);
}

static __inline uint32 ICIPHERWRAPPER_Release(ICipherWrapper *me)
{
   return AEEGETPVTBL(me,ICipherWrapper)->Release(me);
}

static __inline uint32 ICIPHERWRAPPER_QueryInterface(ICipherWrapper *me,
                                    AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,ICipherWrapper)->QueryInterface(me, cls, ppo);
}

static __inline int ICIPHERWRAPPER_GetParam(ICipherWrapper *me,
                                    int id, void *pParam, unsigned *puParamLen)
{
   return AEEGETPVTBL(me,ICipherWrapper)->GetParam(me, id, pParam, puParamLen);
}

static __inline int ICIPHERWRAPPER_SetParam(ICipherWrapper *me,
                                    int id, const void *pParam,
                                    unsigned uParamLen)
{
   return AEEGETPVTBL(me,ICipherWrapper)->SetParam(me, id, pParam, uParamLen);
}

static __inline int ICIPHERWRAPPER_Process(ICipherWrapper* po, 
                                    const byte* pbIn, unsigned cbIn,
                                    byte* pbOut, unsigned* pcbOut)
{
   return AEEGETPVTBL(po,ICipherWrapper)->Process(po, pbIn, cbIn, pbOut, pcbOut);
}

static __inline int ICIPHERWRAPPER_ProcessLast(ICipherWrapper* po, byte* pbOut, unsigned* pcbOut)
{
   return AEEGETPVTBL(po,ICipherWrapper)->ProcessLast(po, pbOut, pcbOut);
}

static __inline int ICIPHERWRAPPER_SetCipher(ICipherWrapper* po,
                                    IQueryInterface *pCipher)
{
   return AEEGETPVTBL(po,ICipherWrapper)->SetCipher(po, pCipher);
}

/*
=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

======================================================================= 

ICipherWrapper Interface

   This interface supports the chaining modes and padding for block ciphers
   which do not directly support the modes.  It is derived from 
   ICipher1.

   A developer would not directly create an instance of this type.  Instead,
   the ICipherFactory will create an instance when the algorithm implementation
   doesn't directly support the mode and use it to wrap the algorithm.
   
See Also:
   ICipher
   
The following header file is required:~
   ICipherWrapper.h 

======================================================================= 

ICIPHERWRAPPER_AddRef()

Description:
	This function is inherited from IQI_AddRef().
	
======================================================================= 

ICIPHERWRAPPER_Release()

Description:
	This function is inherited from IQI_Release().

======================================================================= 

ICIPHERWRAPPER_QueryInterface()
   
Description:
	This function is inherited from IQI_QueryInterface().
	
======================================================================= 

ICIPHERWRAPPER_GetParam()

Description:
   This function is inherited from IPARAMETERS_GetParam().
   
======================================================================= 

ICIPHERWRAPPER_SetParam()

Description:
   This function is inherited from IPARAMETERS_SetParam().
   
======================================================================= 

ICIPHERWRAPPER_Process()

Description:
   Encrypt or decrypt data.

   The output buffer must always be the same size or larger than the input buffer.  If it is not
   an error occurs and no processing is done at all.

   This requirement simplifies the buffering and error handling for the user of a cipher. The 
   code calling the cipher can assume that all the input is always processed. The caller
   can also assume that if there is an error that it is catestrophic for the encryption/
   decryption and abort the whole operation.

   The size of the input buffer does not have to be a multiple of the block size of the 
   underlying algorithm. This frees the caller entirely of having to worry about algorithm
   block sizes and aligning buffers by them.

   For some ICipherWrapper implementations making the input buffer size  a multiple of the 
   block size, if it is convenient and efficient to do, may increase performance because
   some internal buffer copying is not invoked.

   Making the buffers processed large will give some throughput gain because the per-call
   overhead is proportionally less. 

   Some padding modes may buffer up to 2 blocks of input before it appears in the output.
   
   The input and output buffers may point to the same buffer area.

   ICIPHERWRAPPER_ProcessLast() should always be called after all the input data has been fed
   into ICIPHERWRAPPER_Process() to flush out any buffered blocks. For many ICipherWrapper implementations,
   such as stream ciphers, it will not do anything because these ciphers do no buffering,
   however it should still always be called. The advantage of always calling it is that it
   allows the calling code to function correctly no matter whether the cipher used is 
   a stream cipher or block cipher.
        
Prototype:
   int ICIPHERWRAPPER_Process(ICipherWrapper* pICipher, 
               const byte *pchIn, unsigned cbIn,
               byte *pbOut, unsigned* pcbOut)

Parameters:
   pICipher: Pointer to ICipherWrapper interface object
   pchIn:[in]  Pointer to data to be encrypted/decrypted
   cbIn: [in]  Input data size in bytes
   pbOut: [out]     Buffer to receive encrypted/decrypted data
   pcbOut: [in/out]  size of output buffer in bytes, returns number of
            bytes actually written.

Return Value:
   SUCCESS: all input bytes processed
   EBADSTATE:  cipher is not properly initialized/no key
   EBADPARM:   pointer parameter is null
   EBUFFERTOOSMALL: output buffer not large enough

Comments:
   None    

Side Effects: 
   None

   
======================================================================= 

ICIPHERWRAPPER_ProcessLast()
   
Description:
   Encrypt or decrypt last partial block(s) remaining from call to ICIPHERWRAPPER_Process().
   For stream ciphers and streaming block modes (CTR,CFB,OFB), this
   usually is a no-op.

   After all input has been passed to the cipher via ICIPHERWRAPPER_Process(), ICIPHERWRAPPER_ProcessLast() 
   should be called in a loop until no data is returned. In some cases it is possible to call 
   ICIPHERWRAPPER_ProcessLast once with a sufficiently large buffer, but this is not recommended 
   because this may fail for ciphers that internally buffer a lot of data or have very 
   large block sizes. However, any code intended to work with arbitrary ciphers adhering to
   the ICipher1 interface must call ProcessLast in a loop.

Prototype:
   int ICIPHERWRAPPER_ProcessLast(ICipherWrapper* pICipher, byte *pbOut, unsigned* pcbOut)

Parameters:
   pICipher: Pointer to ICipher1 interface object
   pbOut: [out]     Buffer to receive encrypted/decrypted data
   pcbOut: [in/out]  size of output buffer in bytes, returns number of
            bytes actually written.

Return Value:
   SUCCESS: any remaining data has been processed.
   EBADSTATE:  cipher is not properly initialized/no key
   EBADPARM:   pointer parameter is null
   EBUFFERTOOSMALL: output buffer not large enough
   

Comments:
   None    

Side Effects: 
   None

   
======================================================================= 

ICIPHERWRAPPER_SetCipher()

Description:
   Sets the cipher used by ICipherWrapper to the ICipher1 specified.

Prototype:
   int ICIPHERWRAPPER_SetCipher(ICipherWrapper* pICipherWrapper, IQueryInterface* pCipher)

Parameters:
   pICipherWrapper: Pointer to ICipherWrapper interface object
   pCipher: Pointer to ICipher1 or IRawCipher interface object

Return Value:
   SUCCESS:  Cipher is successfully associated with the chaining mode
   ECLASSNOTSUPPORT: Class is not supported or is not a valid object.

Comments:
   The caller should release the cipher after calling this function
   to avoid memory leaks.  ICIPHERWRAPPER_SetCipher performs its own 
   IQUERYINTERFACE_AddRef().
   
   The cipher key must be set after calling ICIPHERWRAPPER_SetCipher() in
   order for the object to be properly initialized.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1.3

See Also:
   ICipherFactory

=======================================================================*/
#endif

