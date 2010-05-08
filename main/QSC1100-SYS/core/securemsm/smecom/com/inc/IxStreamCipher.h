#ifndef __IXSTREAMCIPHER_H__
#define __IXSTREAMCIPHER_H__

/*===========================================================================

                          I X S T R E A M C I P H E R                

                              H E A D E R  F I L E

FILE:  IxStreamCipher.h


GENERAL DESCRIPTION

  This header file defines the IxStreamCipher class which inherits 
  from IxStream class.

  All reference documentation for API functions is contained in the
  file IxStream.h

PUBLIC CLASSES AND STATIC FUNCTIONS
  
INITIALIZATION & SEQUENCING REQUIREMENTS
  
  Copyright (c) 2005 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxStreamCipher.h#10 $
  $DateTime: 2008/04/25 16:30:01 $
  $Author: rajeevg $

===========================================================================*/


/*===========================================================================
                           
               INCLUDE FILES

============================================================================*/
#include "customer.h"
#include "IxErrno.h"
#include "IxStream.h"
#include "UxBuf.h"

#ifdef __cplusplus
  extern "C"
  {
    #include "secapi.h"
  }
#endif

/*===========================================================================

                           CLASS DEFINITION

===========================================================================*/

class IxStreamCipher : public IxStream
{
  #define KEY_SIZE 16
  #define IV_SIZE SECAPI_ENC_AES128_BLOCK_BYTE_LEN

  /* Structure to collect all the data needed to recreate an IxStreamCipher
  ** object.
  */
  typedef struct
  {
    IxStreamKindE                streamType;
    uint32                       access;
    uint32                       initialOffset;
    secapi_algo_enum_type        algo;
    secapi_enc_op_mode_enum_type opMode;
    uint32                       blockSize;
    secapi_enc_padding_enum_type padType;
    unsigned char                padValue;
    uint8                        origIV[IV_SIZE];
    uint8                        curIV[IV_SIZE];
    bool                         origIVSet;
    bool                         dirtyIV;
    uint8*                       pKey;
    bool                         bObfuscatedKey;
    uint8*                       pStreamState;
    uint32                       streamStateLen;
    uint8*                       pBufState;
    uint32                       bufStateLen;
    uint32                       plainTextLen;
  } IxStreamCipherStateDataT;

#ifndef FEATURE_DRM_NO_BREW
  friend IxErrnoType IxStreamCreateFromSource (ISource *pSource,
                                               IxStream **pStream);
#endif
  
  friend void IxStreamReadPostProc (IxStreamReadRspT *pReadRsp);

  friend void IxStreamCreateRefPostProc (IxCallback *pRsp);

public :
  IxStreamCipher(IxStream* dataStream);
  virtual ~IxStreamCipher();
  
/*------------------------------------------------------------------------
                               PUBLIC METHODS
 ------------------------------------------------------------------------*/

/*==============================================================
FUNCTION:  IxStreamCipher::SetCipher()

DESCRIPTION:
  Associates a cipher with this cipher stream. The specified cipher
  object will be used to encrypt and decrypt the data stream
  associated with this cipher stream.

RETURN VALUE:
  Error code. E_SUCCESS is returned on success.

SIDE EFFECTS: None
==============================================================*/
IxErrnoType SetCipher (secapi_handle_type pCipher);

/*==============================================================
* FUNCTION:  IxStreamCipher::SetupCipher()

DESCRIPTION:
  Initializes a cipher object using the passed in parameters
  and then associates the cipher with this cipher stream.

  The bEncryptedKey param indicates whether the key (CEK) passed in has been 
  encrypted with the device hardware key.

RETURN VALUE:
  Error code. E_SUCCESS is returned on success.

SIDE EFFECTS: None
==============================================================*/
IxErrnoType SetupCipher (
  secapi_algo_enum_type         cipherAlgo,
  secapi_enc_op_mode_enum_type  cipherOpMode,
  uint8*                        pCipherKey,
  uint8*                        pCipherIV,
  secapi_enc_padding_enum_type  padType,
  uint8                         padValue,
  boolean                       bEncryptedKey = FALSE,
  boolean                       bObfuscatedKey = FALSE
);

/*==============================================================
* FUNCTION:  IxStreamCipher::Cipher()

DESCRIPTION:
  Returns a handle to the cipher object associated with this
  cipher stream.

RETURN VALUE:
  Handle to associated cipher object.

SIDE EFFECTS: None
==============================================================*/
secapi_handle_type Cipher();

/*==============================================================
* FUNCTION:  IxStreamCipher::Open()

DESCRIPTION:
  Opens a cipher stream. This method just opens the underlying
  data stream. See Open in IxStream.h for details.

  Note that there are two versions of the Open method: a
  synchronous version and an asynchronous version.

RETURN VALUE:
  Error code. E_SUCCESS or E_AGAIN is returned on success.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Open (const char *uriString, 
                          IxStreamCbT pfn,	
                          IxStreamOpenRspT *pOpenRsp,
                          IxStreamAccessRights access=IX_STRM_READWRITE);
  
virtual IxErrnoType Open (const char *uriString, 
                          IxStreamAccessRights access=IX_STRM_READWRITE);
  
/*==============================================================
* FUNCTION:  IxStreamCipher::Close()

DESCRIPTION:
  Closes a cipher stream. This method just closes the underlying
  data stream. See Close in IxStream.h for details.

  Note that there are two versions of the Close method: a
  synchronous version and an asynchronous version.

RETURN VALUE:
  Error code. E_SUCCESS or E_AGAIN is returned on success.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Close (IxStreamCbT pfn, IxStreamCloseRspT *pCloseRsp);

virtual IxErrnoType Close ( );
  
/*==============================================================
* FUNCTION:  IxStreamCipher::Remove()

DESCRIPTION:
  Deletes a cipher stream. This method deletes the underlying
  data stream and the associated buffer. See Remove in IxStream.h
  for details.

RETURN VALUE:
  Error code. E_AGAIN or E_SUCCESS is returned on success.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Remove (IxStreamCbT pfn, IxStreamRemoveRspT *pRemoveRsp);
  
/*==============================================================
* FUNCTION:  IxStreamCipher::Remove()

DESCRIPTION:
  Sync version of the Remove method.

RETURN VALUE:
  Error code. E_SUCCESS is returned on success.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Remove ();

/*==============================================================
* FUNCTION:  IxStreamCipher::Read()

DESCRIPTION:
  Reads data from a cipher stream. There are four variants of the Read
  method: synchronous and asynchronous versions, and versions that
  read into a UxBuf or into a byte array.

  The underlying encrypted data is decrypted before being returned.

  For details on the behavior of Read and error codes returned, see Read
  in IxStream.h.

RETURN VALUE:
  Error code. E_SUCCESS or E_AGAIN is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Read (UxBuf *pBuf, uint32 noOfBytesToRead,
  uint32 *pNBytesRead, IxStreamCbT pfn, IxStreamReadRspT *pReadRsp);
  
virtual IxErrnoType Read (byte *pBuf, uint32 noOfBytesToRead,
  uint32 *pNBytesRead, IxStreamCbT pfn, IxStreamReadRspT *pReadRsp);
  
virtual IxErrnoType Read (UxBuf *pBuf, uint32 noOfBytesToRead,
  uint32 *pNBytesRead, bool *pEndOfStream);
  
virtual IxErrnoType Read (byte *pBuf, uint32 noOfBytesToRead,
  uint32 *pNBytesRead, bool *pEndOfStream);
 
/*==============================================================
* FUNCTION:  IxStreamCipher::Write()

DESCRIPTION:
  Encrypts the passed in plain text data and writes the encrypted data
  into this cipher stream. There are four variants of the Write
  method: synchronous and asynchronous versions, and versions that
  write from a UxBuf or from a byte array.

  For details on the behavior of Write and error codes returned, see Write
  in IxStream.h.

RETURN VALUE:
  Error code. E_SUCCESS or E_AGAIN is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Write (UxBuf *pBuf, uint32 noOfBytesToWrite,
  uint32 *pNBytesWritten);
  
virtual IxErrnoType Write (byte *pBuf, uint32 noOfBytesToWrite,
  uint32 *pNBytesWritten);
  
virtual IxErrnoType Write (UxBuf *pBuf, uint32 noOfBytesToWrite,
  uint32 *pNBytesWritten, IxStreamCbT pfn, IxStreamWriteRspT *pWriteRsp);
  
virtual IxErrnoType Write (byte *pBuf, uint32 noOfBytesToWrite,
  uint32 *pNBytesWritten, IxStreamCbT pfn, IxStreamWriteRspT *pWriteRsp);
  
/*==============================================================
* FUNCTION:  IxStreamCipher::Seek()

DESCRIPTION:
  Seeks to the specified offset in this cipher stream. See Seek in
  IxStream.h for details on input parameters and error codes returned.

RETURN VALUE:
  Error code. E_SUCCESS is returned on success.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Seek (const int32 offset, IxStreamWhenceType whence);
  
/*==============================================================
* FUNCTION:  IxStreamCipher::Tell()

DESCRIPTION:
  Returns the current offset of this cipher stream. See Tell in
  IxStream.h for details on error codes returned.

RETURN VALUE:
  Error code. E_SUCCESS is returned on success.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Tell (uint32* pOffset);
  
/*==============================================================
* FUNCTION:  IxStreamCipher::Size()

DESCRIPTION:
  Returns the size of this cipher stream. This method just calls
  Size on the underlying data stream. See Size in IxStream.h for details.

RETURN VALUE:
  Error code. E_SUCCESS is returned on success.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Size (uint32* pSize);
  
/*==============================================================
* FUNCTION:  IxStreamCipher::Length()

DESCRIPTION:
  Returns the length of this cipher stream. This method just calls
  Length on the underlying data stream. See Length in IxStream.h for details.

RETURN VALUE:
  Error code. E_SUCCESS is returned on success.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Length (uint32* pLength, bool *pEndOfData = NULL);
  
/*==============================================================
* FUNCTION:  IxStreamCipher::UnreadLength()

DESCRIPTION:
  Returns the number of unread bytes in this cipher stream. This method
  just calls UnreadLength on the underlying data stream.
  See UnreadLength in IxStream.h for details.

RETURN VALUE:
  Error code. E_SUCCESS is returned on success.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType UnreadLength (uint32* pLength);
  
/*==============================================================
* FUNCTION:  IxStreamCipher::CreateRef()

DESCRIPTION:
  Creates a reference to this cipher stream. See CreateRef in IxStream.h
  for details on the semantics of CreateRef, the input parameters and
  error codes returned.

  Note that there are two versions of the CreateRef method: a
  synchronous version and an asynchronous version.

RETURN VALUE:
  Error code. E_SUCCESS or E_AGAIN is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType CreateRef(uint32 startOffset, uint32 endOffset,
  IxStream **ppStream, IxStreamCbT pfn, IxCallback *pRsp);
 
virtual IxErrnoType CreateRef(uint32 startOffset, uint32 endOffset,
  IxStream **ppStream);
  
/*==============================================================
* FUNCTION:  IxStreamCipher::CreateCopy()

DESCRIPTION:
  Creates a copy of this cipher stream. See CreateCopy in IxStream.h
  for details.

RETURN VALUE:
  Error code. E_SUCCESS is returned on success.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType CreateCopy(uint32 startOffset, uint32 endOffset,
  IxStream **ppStream, IxStreamAccessRights access);
  
/*==============================================================
FUNCTION:  IxStreamCipher::GetState()

DESCRIPTION:
  Gets state data for the stream. This data can be used to make a copy
  of the stream using the SetState method.

INPUT/OUTPUT PARAMETERS:
  @param  uint8** ppStateData   - double pointer to buffer containing
                                  state data
  @param  uint32* pStateDataLen - pointer to length of state data

RETURN VALUE: None

SIDE EFFECTS: None
==============================================================*/
virtual void GetState (uint8** ppStateData, uint32* pStateDataLen);

/*==============================================================
FUNCTION:  IxStreamCipher::SetState()

DESCRIPTION:
  Creates a stream from specified state data. Use the GetState method
  to obtain the state data from a stream.

INPUT/OUTPUT PARAMETERS:
  @param  uint8* pStateData    - pointer to buffer containing
                                 state data
  @param  uint32* StateDataLen - length of state data

RETURN VALUE: None

SIDE EFFECTS: None
==============================================================*/
virtual void SetState (uint8* pStateData, uint32 stateDataLen);

/*==============================================================
FUNCTION:  IxStreamCipher::GetURI()

DESCRIPTION:
  Returns the URI associated with the stream.

INPUT/OUTPUT PARAMETERS:
  @param  char** ppURIPath        - pointer to URI path
  @param  char** ppURIScheme      - pointer to URI scheme

RETURN VALUE: E_SUCCESS on success, error code otherwise

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType GetURI (char** ppURIPath, char **ppURIScheme);

/*==============================================================
FUNCTION:  IxStreamCipher::MarkProgressive()

DESCRIPTION:
  Marks the stream as a progressively downloaded stream. This means
  that data can be appended to the end of the stream with the passage
  of time.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE: E_SUCCESS on success, error code otherwise

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType MarkProgressive ();

/*==============================================================
FUNCTION:  IxStreamCipher::IsProgressive()

DESCRIPTION:
  Checks if the stream is a progressively downloaded stream.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE: TRUE on success, FALSE otherwise

SIDE EFFECTS: None
==============================================================*/
virtual boolean IsProgressive ();

private:

  IxErrnoType ReadPostProc ( IxStreamReadRspT *pReadRsp );

  IxErrnoType CreateRefPostProc ( IxCallback *pRsp );

  IxErrnoType IxStreamCipher::ReadInternal ( void *pReadBuf,
    IxStreamInterfaceE interfaceType, uint32 noOfBytesToRead,
    uint32 *pNBytesRead, bool *pEndOfStream,
    IxStreamCbT pfn, IxStreamReadRspT *pReadRsp );

  IxErrnoType IxStreamCipher::CreateRefInternal ( uint32 firstOffset,
    uint32 lastOffset, IxStream **ppStream, IxStreamCbT pfn, IxCallback *pRsp );

  IxErrnoType IxStreamCipher::SetIV ( uint8 *pIV = NULL );

  IxStream*                    __dataStream;
  UxBuf*                       __buf;
  uint32                       __access;
  uint32                       __initialOffset;

  secapi_handle_type           __pCipher;
  secapi_algo_enum_type        __algo;
  secapi_enc_op_mode_enum_type __opMode;
  uint32                       __blockSize;
  secapi_enc_padding_enum_type __padType;
  unsigned char                __padValue;
  uint8                        __origIV[IV_SIZE];
  uint8                        __curIV[IV_SIZE];
  bool                         __origIVSet;
  bool                         __dirtyIV;
  uint8                       *__pKey;
  bool                         __bObfuscatedKey;
  uint32                       __plainTextLen;
};

#endif // _IXSTREAMCIPHER_H_
