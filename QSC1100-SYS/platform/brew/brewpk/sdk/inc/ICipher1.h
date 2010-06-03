#ifndef ICIPHER1_H
#define ICIPHER1_H
/*======================================================
FILE:  ICipher1.h

SERVICES:  Cipher Interface

DESCRIPTION: This file provides definitions for the ICipher1 interface 
made available to application developers. This is a standard header file
that must be included by all applications using the ICipher1 services.

The ICipher1 interface supports symmetric streaming and block ciphers.

Supported Interfaces:
ICipher1 - standard interface for for symmetric ciphers including
         stream and block ciphers.

CLASS IDs:
         AEEIID_CIPHER

   stream ciphers:
         see STREAM_CIPHER.bid
   
   block ciphers:
         see BLOCK_CIPHER.bid
===============================================================================
Copyright © 2005-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
#include "IParameters.h"

//*********************************************************************
//
// ICipher1 Interface
// This interface is the public interface for stream and block 
// cipher implementations.
//
// 
//*********************************************************************

// parameter types for ICIPHER1_GetParam and ICIPHER1_SetParam.
// Not all parameter types are supported by all ciphers.
                                    // 
#define  CIPHER_PARAM_DIRECTION  0  // get set encryption/decryption direction.  Defaults to encryption
#define  CIPHER_PARAM_KEY        1  // --- set encryption key
#define  CIPHER_PARAM_KEY_SIZE   2  // get --- current key size
#define  CIPHER_PARAM_IV         3  // get set initialization vector
#define  CIPHER_PARAM_IV_SIZE    4  // get --- initialization vector size
#define  CIPHER_PARAM_PADDING    5  // get set block padding style
#define  CIPHER_PARAM_BLOCKSIZE  6  // get --- blocksize
#define  CIPHER_PARAM_KEYSCHED   7  // get set the keyschedule info
#define  CIPHER_PARAM_MODE       8  // get set the chaining mode w/ mode id.
#define  CIPHER_PARAM_BUFFERED   9  // get --- status for buffered data

// values associated with CIPHER_PARAM_DIRECTION.
// The direction determines whether this ICipher1 instance
// encrypts or decrypts.
#define CIPHER_DIRECTION_ENCRYPT 0
#define CIPHER_DIRECTION_DECRYPT 1


// values associated with CIPHER_PARAM_PADDING.
// The padding setting determines how a block cipher
// fills out the bytes in a final partial block.
#define  CIPHER_PADDING_NONE    0   // no special padding. Must always provide full blocks.
#define  CIPHER_PADDING_ZERO    1   // pad with zeros
#define  CIPHER_PADDING_LENGTH  2   // pad with zeros, except final byte is # bytes padded
                                    //   Always pads with at least 1 byte.  Padding is
                                    //   stripped on decrypt
#define  CIPHER_PADDING_RANDOM  3   // fill w/ random data.
#define  CIPHER_PADDING_RFC2630 4   // per RFC 2630, pad each byte with # bytes padded, 
                                    //   Always pads with at least 1 byte.  Padding is
                                    //   stripped on decrypt
#define  CIPHER_PADDING_SEQUENTIAL 5 // fill each pad byte with increasing numbers beginning 
                                    // at 1 (see RFC 2406 or 1570)
//#define  CIPHER_PADDING_CTS     6   // Ciphertext stealing.  Not currently supported
 
#define AEEIID_CIPHER            0x0102cce3

// Define ICipher1 derived from IParameters
typedef struct ICipher1 ICipher1;

#define INHERIT_ICipher1(iname) \
   INHERIT_IParameters(iname); \
   int      (*Process) (iname* po, const byte* pbIn, unsigned cbIn, byte* pbOut, unsigned * pcbOut); \
   int      (*ProcessLast)(iname* po, byte* pbOut, unsigned * pcbOut)
   
AEEINTERFACE(ICipher1)
{
   INHERIT_ICipher1(ICipher1);
};

static __inline uint32 ICIPHER1_AddRef(ICipher1 *me)
{
   return AEEGETPVTBL(me,ICipher1)->AddRef(me);
}

static __inline uint32 ICIPHER1_Release(ICipher1 *me)
{
   return AEEGETPVTBL(me,ICipher1)->Release(me);
}

static __inline uint32 ICIPHER1_QueryInterface(ICipher1 *me,
                                              AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,ICipher1)->QueryInterface(me, cls, ppo);
}

static __inline int ICIPHER1_GetParam(ICipher1 *me,
                                     int id, void *pParam, unsigned *puParamLen)
{
   return AEEGETPVTBL(me,ICipher1)->GetParam(me, id, pParam, puParamLen);
}

static __inline int ICIPHER1_SetParam(ICipher1 *me,
                                         int id, const void *pParam,
                                         unsigned uParamLen)
{
   return AEEGETPVTBL(me,ICipher1)->SetParam(me, id, pParam, uParamLen);
}

static __inline int ICIPHER1_Process(ICipher1* po, 
                                    const byte* pbIn, unsigned cbIn,
                                    byte* pbOut, unsigned * pcbOut)
{
   return AEEGETPVTBL(po,ICipher1)->Process(po, pbIn, cbIn, pbOut, pcbOut);
}

static __inline int ICIPHER1_ProcessLast(ICipher1* po, byte* pbOut, unsigned * pcbOut)
{
   return AEEGETPVTBL(po,ICipher1)->ProcessLast(po, pbOut, pcbOut);
}

/*
=======================================================================
DATA STRUCTURES   DOCUMENTATION
======================================================================= 
CIPHER_PARAM_XXX

Description:
This enum specifies the ID for the ICIPHER1_GetParam and ICIPHER1_SetParam functions.
For ICIPHER1_Getparam functions, if pParam is null, pcParam returns the number of
bytes required to hold the results.

The following ID's are supported:

===h1>Name: CIPHER_PARAM_DIRECTION
===/h1>
 
   Description:
   ICIPHER1_SetParam: This ID is used to set the encrypt/decrypt
   direction. 
  
   ICIPHER1_GetParam: This ID is used to get the current 
   encrypt/decrypt direction.

   All ICipher1 implementations must support getting this parameter. Some
   ICipher1 implementations may not support setting the cipher direction.
   
   If the ICipher1 instance is created via the ICipherFactory, then this 
   parameter need not be set directly.
  
   Return Value:
   SUCCESS: Direction set, or retrieved.
   EBADPARM: Invalid direction or pcParam is null.   

   Comments:
   The direction defaults to encryption unless the cipher supports only
   decryption.  For ciphers that use the
   same algorithm for encryption and decryption, this setting
   has no effect. Valid values for the parameter are
   CIPHER_DIRECTION_ENCRYPT - use this ICipher1 to encrypt
   CIPHER_DIRECTINO_DECRYPT - use this ICipher1 to decrypt
   
===h1>Name: CIPHER_PARAM_KEY
===/h1>  

   Description:
   ICIPHER1_SetParam:  This ID is used to set the Cipher key.  pParam points
   to a buffer holding the key, cParam is the length of the key in bytes.
   For ciphers which support variable key lengths, this function also
   sets the length.  For all other ciphers, the key length must match
   the supported length.  See specific cipher documentation for
   how hardware keys may be supported.
   
   ICIPHER1_GetParam: not supported
   
  
   Return Value:
   SUCCESS: key is set
   AEE_CRYPT_INVALID_KEY: key is wrong length or otherwise invalid.
   EBADPARM: 

   Comments:
   See the specific algorithm documentation for valid key lengths.
   
   Some very special implementations of ICipher1 may not support setting
   the key because they are intrinicly bound to some key. A implementation of
   ICipher1 bound to a hardware token holding a key is an example of this.
   
   The suggested approach for internal or hardware keys 
   (if supported on a device), is to define pParam as a pointer
   to integer which identifies the key.  cParam is then set to
   sizeof(int).

===h1>Name: CIPHER_PARAM_KEY_LEN
===/h1>

   ICIPHER1_SetParam: not supported
   
   ICIPHER1_GetParam: This ID is used to retrieve the key length supported by
   this cipher.  pParam points to a buffer capable of holding an int.  The number
   of bytes required for the key is placed in this buffer.  For ciphers which
   support variable key lengths, this function returns the largest supported
   key length.

   Most ICipher1 implementations should support fetching the key length, but
   it is not required of all.

   Return Value:
   SUCCESS: if task is successful
   EBADPARM: pcParam is null

===h1>Name: CIPHER_PARAM_IV
===/h1>  

   Description:
   This ID sets or retrieves the Initialization Vector parameter.  The 
   IV is used in block chaining modes to provide an initial state for
   the chaining algorithm.  The initialization vector is updated with 
   each block processed.  If not set, the default vector is zeros.
   
   ICIPHER1_SetParam:  This ID is used to set the IV.  pParam points
   to a buffer containing the IV, cParam is the length of the vector.
   
   ICIPHER1_GetParam: This ID retrieves the updated Initializaton Vector.
   pParams points to a buffer large enough to hold the results.

   Support for this parameter in an ICipher1 implementation is optional.
  
   Return Value:
   SUCCESS: IV is retrieved/set
   EBADPARM: This cipher does not support chaining mode, the buffer is
   not the correct size or a required parameter is NULL.

===h1>Name: CIPHER_PARAM_IV_LEN
===/h1>

   Description:
   ICIPHER1_SetParam: not supported
   
   ICIPHER1_GetParam: This ID retrieves the initialization vector 
   length supported by this cipher.  pParam points to a buffer large enough
   to hold an int.  The number of bytes required for the IV is placed in
   this buffer.  This parameter ID is only valid for block ciphers with IVs.
   For other ciphers, this returns a value of zero.
   
  Support for this parameter in an ICipher1 implementation is optional.
   

   Return Value:
   SUCCESS: if task is successful
   EBADPARM: Buffer is invalid, or this cipher does not use an IV for the mode.
   modes.

===h1>Name: CIPHER_PARAM_PADDING
===/h1>

   Description:
   The padding parameter identifies how to pad the final block(s) of a
   block cipher.  This parameter is only valid for block ciphers.
   
   ICIPHER1_SetParam: This ID sets the current padding mode.
   pParam points to an int with the mode to set. If the ICipher1 instance is 
   created by the factory, then this parameter need not be set.

   Some implementations of ICipher1 may not allow the padding type to be set.
   
   ICIPHER1_GetParam: This ID retrieves the current padding mode.
   pParam points to a buffer capable of holding an int. All implementations
   must support fetching the padding type even if they indicate no padding
   is supported.
   
   Valid values for this parameter are
   CIPHER_PADDING_NONE - no padding is performed.  If used with a block cipher,
                         the unspecified bytes may contain data from previous blocks.
   CIPHER_PADDING_ZERO - The unused bytes are filled with zeros.  This is the default
                         for block ciphers.
   CIPHER_PADDING_LENGTH - Pad unused bytes with zeros, except for the final byte of
                         block which contains the number of bytes padded.  Always adds
                         at least one byte of padding.  The padding is stripped on
                         decrypt.
   CIPHER_PADDING_RANDOM - Pad with random bytes.  Last byte is length of padding.
                        Padding is stripped on decrypt
   CIPHER_PADDING_RFC2630 - Pad per RFC 2630.  Each byte is padded with the # of bytes
                         padded.  Always adds at least 1 byte of padding.  The padding
                         is stripped on decrypt.
   CIPHER_PADDING_SEQUENTIAL - Each pad byte is filled with increasing values, starting
                        at 1.  The last byte is the length of padding.  Always adds
                        at least 1 byte of padding. The padding is stripped on decrypt.


   Return Value:
   SUCCESS: if task is successful
   
   EBADPARM: Buffer is invalid, or this is not a block cipher.

===h1>Name: CIPHER_PARAM_BLOCKSIZE
===/h1>

   Description:
   For block ciphers, this ID retrieves the block size.  For stream
   ciphers, this returns 1.
   
   ICIPHER1_GetParam: This ID retrieves the block size.  For stream ciphers
   and block ciphers in a streaming mode, this returns 1.
   pParam points to a buffer large enough to hold an int.

   The block size cannot be set, however it can always be fetched. All 
   implementations of ICipher1 must return a block size.

   Return Value:
   SUCCESS: if task is successful
   EBADPARM: Null parameters.

===h1>Name: CIPHER_PARAM_KEYSCHED
===/h1>
  
   Description:
   The ID sets or retrieves the internal key schedule.  The key schedule
   is computed when the key is set.  Application developers should use
   CIPHER_PARAM_KEY instead.
   
   ICIPHER1_SetParam:  This ID is used to set the key schedule directly.
   pParam points to a buffer containing the key schedule, cParam is 
   the length of the key schedule.
   
   ICIPHER1_GetParam: This ID retrieves the current key schedule.
   pParams points to a buffer large enough to hold the results.
 
   Return Value:
   SUCCESS: key schedule is retrieved/set
   EBADPARM: The buffer is not the correct size or a required parameter is NULL.
   EBADSTATE:  The key has not yet been set.
   AEE_CRYPT_INVALID_KEY: keyschedule cannot be retrieved for current key.

===h1>Name: CIPHER_PARAM_MODE
===/h1>  

   Description:
   ICIPHER1_SetParam:  This ID is used to set the chaining mode.  pParam points
   the class id of the desired mode. This parameter is set by ICipherFactory
   during the creation process, if the factory was used to create the cipher. 

   It may not be possible to set the chaining mode for all ciphers.
   
   ICIPHER1_GetParam: This ID retrieves the chaining mode.
   pParams points to a buffer large enough to hold an AEECLSID.

   It is recommended that all ciphers return the chaining mode when ICIPHER1_GetParam is
   called, but it is not required.
  
   Return Value:
   SUCCESS: mode is valid
   EBADPARM: This cipher does not support the specified chaining mode or
             is not a block cipher
             
   See also:
      CIPHERWRAPPER.bid

===h1>Name: CIPHER_PARAM_BUFFERED
===/h1> 
 
   Description:
   ICIPHER1_SetParam: not supported
   
   ICIPHER1_GetParam: This ID returns the number of bytes currently buffered
   by the cipher.  Due to padding, this may not be the actual number of
   bytes returned from ICipher1_ProcessLast.  A non-zero value does indicate
   that ICIPHER1_ProcessLast() must be called to finish processing.
   pParam points to a buffer large enough to hold an int.

 
   Return Value:
   SUCCESS: data is buffered
   EFAILED: no data is buffered
   EBADPARM: cipher doesn't buffer data

=============================================================================*/
/*
=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: ICipher1

   The functions in ICipher1 enable BREW developers to perform
   symmetric cipher encryption or decryption in a flexible manner 
   without having to know the internals of the encryption algorithm or
   implementation.

   This interface supports both stream and block ciphers.

   ICipher1 is different from most BREW classes in that it is
   created via an instance of ICipherFactory instead of 
   ISHELL_CreateInstance.   Use ISHELL_CreateInstance to create
   the ICipherFactory and then call ICIPHERFACTORY_CreateCipher() or 
   ICIPHERFACTORY_CreateCipher2() to create the specific cipher with all 
   required features.  The factory simplifies the process of creating the
   ICipher1 which best fits the developer's requirements.  If the appropriate
   cipher can be created successfully, ICIPHERFACTORY_CreateCipher returns
   an ICipher1 interface pointer.
   See ICipherFactory for more details.
   
   Each ICipher1 algorithm (i.e. DES, AES) may have multiple implementations,
   depending on the platform. Ciphers may be implemented in software, by a
   DSP or dedicated hardware. For each algorithm there is an abstract 'best fit' 
   class Id which the factory uses to select the best available implementation
   for the current platform (generally this implies the fastest implementation).
   This allows application developers to develop platform independent code,
   without being concerned with the platform features.
   
   Should the developer require a specific implementation, they may use
   the class id which identifies a specific implementation, with the 
   understanding that a specific implementation may not be available 
   on all devices.
   
   The encrypt/decrypt key is set by calling ICIPHER1_SetParam() with 
   CIPHER_PARAM_KEY.  If using a block cipher, the initialization vector 
   (IV) is set with CIPHER_PARAM_IV.
   
   After initializing the key and optional IV, call ICIPHER1_Process() 
   as required to encrypt or decrypt the data.  Note that due to buffering
   and padding, the cipher may not return the same number of bytes passed
   in as returned on a given ICIPHER1_Process() call.
   
   
   For block ciphers, always call ICIPHER1_ProcessLast to
   flush out the final block(s).  This call is optional for stream ciphers.
   
   
   When done with an ICipher1, call ICIPHER1_Release().

The following header file is required:~
   ICipher1.h

=============================================================================
Function: ICIPHER1_AddRef()

This function is inherited from IQI_AddRef().

==============================================================================
Function: ICIPHER1_Release()

This function is inherited from IQI_Release().

==============================================================================
Function: ICIPHER1_QueryInterface()

This function is inherited from IQI_QueryInterface().

==============================================================================

ICIPHER1_GetParam()

Description:
   This function is inherited from IParameters.
   Retrieves information about the Cipher.  This method takes an
   ID that specifies what information is provided. The pParam contains
   the information.  See CIPHER_PARAM_* for details on the supported
   parameters.

Prototype:
   int ICIPHER1_GetParam(ICipher1* pICipher, int id, void *pParam, unsigned *pcParam)

Parameters:
   pICipher:  Pointer to ICipher1 interface object
   id:      Type of parameter to fetch
   pParam:  Pointer to a structure for storing the results.  If the
            pointer is NULL, pcParam will contain the required number of bytes
            to hold the parameter.
            
   pcParam: On input, size of structure passed in.  On return,
            contains the actual size of the result.

Return Value:

   SUCCESS:     Parameter fetched successfully
   EBADPARM:    Invalid parameter - bad set type
   AEE_CRYPT_INVALID_KEY: key is wrong length or otherwise invalid.
   EBADSTATE:  ICipher1 has not been initialized properly (i.e. key not set)

Comments:
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1.3

See Also:
   CIPHER_PARAM_XXX
   ICIPHER1_SetParam

==============================================================================

ICIPHER1_SetParam()

Description:
   This function is inherited from IPARAMETERS_SetParam().
   Sets information required by the Cipher.  This method takes an
   ID that specifies what information is provided. The pParam contains
   the information.  See CIPHER_PARAM_* for details on the supported
   parameters.

Prototype:
   int ICIPHER1_SetParam(ICipher1* pICipher, int id, void *pParam, unsigned *pcParam)

Parameters:
   pICipher:  Pointer to ICipher1 interface object
   id:      Type of parameter to set
   pParam:  Pointer to a structure containing the value to set.
            
   cParam: size of structure passed in.

Return Value:

   SUCCESS:     Parameter fetched successfully
   EBADPARM:    Invalid parameter - bad set type or pParam is null.
   AEE_CRYPT_INVALID_KEY: key is wrong length or otherwise invalid.
   EBADSTATE:  ICipher1 has not been initialized properly (i.e. key not set)

Comments:
   Calling SetParam between calls to Process or ProcessLast will given undefined
   results and should never be done. SetParam should be called either on a 
   freshly created ICipher1 or after ProcessLast has been called to fully drain
   the internel buffer.

   There are two exceptions allowed for calling SetParam between Process and 
   ProcessLast. A given cipher may define some meaning for this call or 
   a given parameter may define some meaning. However if the meaning is not
   defined in this way the results are entirely undefined.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1.3

See Also:
   CIPHER_PARAM_XXX
   ICIPHER1_GetParam

======================================================================= 

ICIPHER1_Process()

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

   For some ICipher1 implementations making the input buffer size  a multiple of the 
   block size, if it is convenient and efficient to do, may increase performance because
   some internal buffer copying is not invoked.

   Making the buffers processed large will give some throughput gain because the per-call
   overhead is proportionally less. 

   Some padding modes may buffer up to 2 blocks of input before it appears in the output.
   
   The input and output buffers may point to the same buffer area.

   ICIPHER1_ProcessLast() should always be called after all the input data has been fed
   into ICIPHER1_Process() to flush out any buffered blocks. For many ICipher1 implementations,
   such as stream ciphers, it will not do anything because these ciphers do no buffering,
   however it should still always be called. The advantage of always calling it is that it
   allows the calling code to function correctly no matter whether the cipher used is 
   a stream cipher or block cipher.
        
Prototype:
   int ICIPHER1_Process(ICipher1* pICipher, 
               const byte *pchIn, unsigned cbIn,
               byte *pbOut, unsigned* pcbOut)

Parameters:
   pICipher: Pointer to ICipher1 interface object
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

Version:
   Introduced BREW Client 3.1.3

See Also:
   ICIPHER1_ProcessLast()

======================================================================= 

ICIPHER1_ProcessLast()

Description:
   Encrypt or decrypt last partial block(s) remaining from call to ICIPHER1_Process().
   For stream ciphers and streaming block modes (CTR,CFB,OFB), this
   usually is a no-op.

   After all input has been passed to the cipher via ICIPHER1_Process(), ICIPHER1_ProcessLast() 
   should be called in a loop until no data is returned. In some cases it is possible to call 
   ICIPHER1_ProcessLast once with a sufficiently large buffer, but this is not recommended 
   because this may fail for ciphers that internally buffer a lot of data or have very 
   large block sizes. However, any code intended to work with arbitrary ciphers adhering to
   the ICipher1 interface must call ProcessLast in a loop.

Prototype:
   int ICIPHER1_ProcessLast(ICipher1* pICipher, byte *pbOut, unsigned* pcbOut)

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

Version:
   Introduced BREW Client 3.1.3

See Also:
   ICIPHER1_Process()

=======================================================================*/
#endif
