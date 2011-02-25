#ifndef __IXSTREAM_H__
#define __IXSTREAM_H__

/*===========================================================================

                              I X  S T R E A M

                            H E A D E R  F I L E

FILE:  IxStream.h

GENERAL DESCRIPTION

  This header file defines the interface to IxStream.

  This interface has methods for opening, reading, writing, seeking into,
  creating a reference to, and closing a stream, among others. The purpose of
  this inferface is to provide an abstract API regardless of the type of
  stream being used. The stream could be a file stream or a memory stream
  or a socket stream, for instance.

 PUBLIC CLASSES AND STATIC FUNCTIONS:

 INITIALIZATION & SEQUENCING REQUIREMENTS:


  Copyright (c) 2005 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxStream.h#11 $
  $DateTime: 2008/05/07 17:39:52 $
  $Author: amundi $

===========================================================================*/


/*===========================================================================

               INCLUDE FILES

============================================================================*/

#include "customer.h"

/*
** Define FEATURE_DRM_NO_BREW to remove BREW dependencies.
*/

#include "rtedef.h"
#include "IxErrno.h"
#ifndef FEATURE_DRM_NO_BREW
  #include "AEESource.h"
#endif
#include "ipc.h"
#include "rex.h"
#include "ixipc.h"

#ifdef __cplusplus
  #include "UxBuf.h"
  #include "UxURI.h"
  #include "IxCallbackDef.h"
#endif

/*------------------------------------------------------------------------
                              DATA TYPES
------------------------------------------------------------------------*/

typedef void (*IxStreamCbT)(void* pUser);

/*
** Types of data structures supported in the IxStream interface functions.
*/
typedef enum
{
  IX_STRM_INTERFACE_UXBUF,
  IX_STRM_INTERFACE_BYTEARRAY
} IxStreamInterfaceE;

/*
** Types of streams supported statically. Other types of streams can be
** supported through dynamic registration.
*/
typedef enum
{
  IX_STRM_NONE,
  IX_STRM_FILE,
  IX_STRM_BUF,
  IX_STRM_CIPHER,
  IX_STRM_SOURCE,
  IX_STRM_MEDIA,
  IX_STRM_ENUM_SIZER = 0x7fffffff
} IxStreamKindE;

/* Type of structure passed to Read callback. The InternalT type contains
 * fields that are used internally and are not meant for the user's
 * consumption.
 */
typedef struct
{
  void*         pUxBuf;
  int32         interfaceType;
  byte*         pByteArray;
  IxStreamKindE streamType;
  void*         pObject;
  void*         pExtraData;
} IxStreamReadRspInternalT;

typedef struct
{
  IxErrnoType              error;
  uint32                   nBytesRead;
  void*                    pStream;
  void*                    pUserData;
  IxStreamReadRspInternalT internalFields;
} IxStreamReadRspT;

/* Type of structure passed to Write callback.
 */
typedef struct
{
  IxErrnoType error;
  uint32      nBytesWritten;
  void*       pStream;
  void*       pUserData;
} IxStreamWriteRspT;

/* Type of structure passed to Open callback.
 */
typedef struct
{
  IxErrnoType error;
  void*       pStream;
  void*       pUserData;
} IxStreamOpenRspT;

/* Type of structure passed to Close callback.
 */
typedef struct
{
  IxErrnoType error;
  void*       pStream;
  void*       pUserData;
} IxStreamCloseRspT;

/* Type of structure passed to Remove callback.
 */
typedef struct
{
  IxErrnoType error;
  void*       pStream;
  void*       pUserData;
} IxStreamRemoveRspT;

/* Type of structure passed to CreateRef callback. The InternalT type contains
 * fields that are used internally and are not meant for the user's
 * consumption.
 */
typedef struct
{
  IxStreamKindE streamType;
  void*         pObject;
  void*         pExtraData;
} IxStreamCreateRefRspInternalT;

/* Seek types.
 * IX_STRM_SEEK_START:   seek from the beginning of the stream.
 * IX_STRM_SEEK_END:     seek from the current offset.
 * IX_STRM_SEEK_CURRENT: seek from the end of the stream.
 */
typedef enum
{
  IX_STRM_SEEK_START,
  IX_STRM_SEEK_END,
  IX_STRM_SEEK_CURRENT
} IxStreamWhenceType;

#define IX_STRM_READ      0x0001  // opens file for reading
#define IX_STRM_WRITE     0x0002  // opens file for writing
#define IX_STRM_CREATE    0x0004  // creates a new file if it does not exist
#define IX_STRM_TRUNC     0x0008  // opens file and truncates it to zero length
#define IX_STRM_READWRITE (IX_STRM_READ | IX_STRM_WRITE)

/*
** Maximum number of bytes in a stream path.
*/
#define IXSTRM_PATH_MAX 256

/*
** Maximum number of bytes in a URI scheme string.
*/
#define IXSTRM_URI_SCHEME_MAX 16

#define IXSTRM_INT_MAX  0x7FFFFFFF

typedef uint32 IxStreamAccessRights;

/*===========================================================================

                           PUBLIC FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION:  IxStreamDispatchMsg()

DESCRIPTION:
  Dispatches IxStream callbacks. The appropriate callback function
  is called depending on the IPC message passed in.
===========================================================================*/
IxErrnoType IxStreamDispatchMsg (ixipc_msg_type* ipcRspMsg,
  rex_sigs_type ipcSig);

/*===========================================================================

                           CLASS DEFINITION

===========================================================================*/

#ifdef __cplusplus

class IxStream
{

#ifndef FEATURE_DRM_NO_BREW
  friend IxErrnoType IxStreamCreateFromSource (ISource *pSource,
    IxStream **pStream);
#endif

  public:

  virtual ~IxStream() {}

/*------------------------------------------------------------------------
                               PUBLIC METHODS
 ------------------------------------------------------------------------*/

/*==============================================================
FUNCTION:  IxStream::Open()

DESCRIPTION:
  Open the stream referred to by the path passed in. Access permissions
  for the stream can be optionally passed in.

  The return value indicates whether the Open request was successfully
  queued or not. If successfully queued, the callback function will be
  called with the result of the Open request.

  If Open() completes successfully, pOpenRsp->error will be set to
  E_SUCCESS.

INPUT/OUTPUT PARAMETERS:
  @param  QxChar*     pPath           - pointer to the path string
  @param  IxStreamCbT pfn             - open callback function
  @param  IxStreamOpenRspT* pOpenRsp  - pointer to structure to be filled
                                        in and passed to the open callback
  @param  IxStreamAccessRights access - access rights for the stream

RETURN VALUE:
  Error Code. E_AGAIN or E_SUCCESS is returned if there is no error.

SIDE EFFECTS:
==============================================================*/
virtual IxErrnoType Open (const char *pPath,
  IxStreamCbT pfn,
  IxStreamOpenRspT *pOpenRsp,
  IxStreamAccessRights access = IX_STRM_READWRITE) = 0;

/*==============================================================
FUNCTION:  IxStream::Open()

DESCRIPTION:

  Open the stream. Sync version of the Open method.

INPUT/OUTPUT PARAMETERS:
  @param  QxChar*     pPath           - pointer to the path string
  @param  IxStreamAccessRights access - access rights for the stream

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS:
==============================================================*/
virtual IxErrnoType Open (const char *pPath,
  IxStreamAccessRights access = IX_STRM_READWRITE) = 0;

/*==============================================================
FUNCTION:  IxStream::Close()

DESCRIPTION:

  Close the stream.

  The return value indicates whether the Close request was successfully
  queued or not. If successfully queued, the callback function will be
  called with the result of the Close request.

  If Close() completes successfully, pCloseRsp->error will be set to
  E_SUCCESS.

INPUT/OUTPUT PARAMETERS:
  @param  IxStreamCbT pfn              - close callback function
  @param  IxStreamCloseRspT* pCloseRsp - pointer to structure to be filled
                                         in and passed to the close callback

RETURN VALUE:
  Error Code. E_AGAIN or E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Close (IxStreamCbT pfn,
  IxStreamCloseRspT *pCloseRsp) = 0;

/*==============================================================
FUNCTION:  IxStream::Close()

DESCRIPTION:

  Sync version of the Close method.

INPUT/OUTPUT PARAMETERS: None

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Close () = 0;

/*==============================================================
FUNCTION:  IxStream::Remove()

DESCRIPTION:

  Deletes the underlying data of the stream. For instance, if the
  stream is a filestream, the underlying file is deleted.

  The return value indicates whether the Remove request was successfully
  queued or not. If successfully queued, the callback function will be
  called with the result of the Remove request.

  If Remove() completes successfully, pRemoveRsp->error will be set to
  E_SUCCESS.

INPUT/OUTPUT PARAMETERS:
  @param  IxStreamCbT pfn                - remove callback function
  @param  IxStreamRemoveRspT* pRemoveRsp - pointer to structure to be filled
                                           in and passed to the remove callback

RETURN VALUE:
  Error Code. E_AGAIN or E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Remove (IxStreamCbT pfn,
  IxStreamRemoveRspT *pRemoveRsp) = 0;

/*==============================================================
FUNCTION:  IxStream::Remove()

DESCRIPTION:

  Sync version of the Remove method.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Remove () = 0;

/*==============================================================
FUNCTION:  IxStream::Read()

DESCRIPTION:

  Request method to read a given number of bytes from the stream.

  If there is data available in the client, those bytes are copied into
  the specified UxBuf immediately and this method returns E_SUCCESS.
  Otherwise a request is sent to the appropriate server and E_AGAIN
  is returned. If this method returns E_AGAIN, the caller must call
  ipc_dispatch_msg() to wait for an IPC message from the server and
  to dispatch the received IPC message.

  It is not an error to return fewer bytes than requested.

  If Read() completes successfully, pReadRsp->error will be set to
  E_SUCCESS.

INPUT/OUTPUT PARAMETERS:
  @param  UxBuf*  pBuf : pointer to UxBuf into which data is to be read
  @param  uint32 noOfBytesToRead : number of bytes to read
  @param  IxStreamCbT pfn : callback function to be called when the read is
                            complete
  @param pReadRsp :  pointer to data passed to callback function

RETURN VALUE:
  Error Code. E_AGAIN or E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Read (UxBuf *pBuf, uint32 noOfBytesToRead,
  uint32 *pNBytesRead, IxStreamCbT pfn, IxStreamReadRspT *pReadRsp) = 0;

/*==============================================================
FUNCTION:  IxStream::Read()

DESCRIPTION:

  Sync version of the Read method. Returns the number of bytes read
  and whether end-of-stream was reached using the third and fourth
  parameters.

  It is not an error to return fewer bytes than requested.

INPUT/OUTPUT PARAMETERS:
  @param  UxBuf*  pBuf : pointer to UxBuf into which data is to be read
  @param  uint32  noOfBytesToRead : number of bytes to read
  @param  uint32* pNBytesRead : pointer to variable to hold the number
                                of bytes read
  @param  bool*   pEndOfStream: pointer to boolean used to indicate
                                end-of-stream

RETURN VALUE:
  Error Code. E_SUCCESS is returned if the Read completes successfully.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Read (UxBuf *pBuf, uint32 noOfBytesToRead,
  uint32 *pNBytesRead, bool *pEndOfStream) = 0;

/*==============================================================
FUNCTION:  IxStream::Read()

DESCRIPTION:

  Request method to read a given number of bytes from the stream. See
  comments for the UxBuf version of this method for details.

INPUT/OUTPUT PARAMETERS:
  @param  byte*  pBuf : pointer to char array into which to read data 
  @param  uint32  noOfBytesToRead : number of bytes to read
  @param  IxStreamCbT pfn : callback function to be called when the read is
                           complete
  @param pReadRsp : pointer to data passed to callback function

RETURN VALUE:
  Error Code. E_AGAIN or E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Read (byte *pBuf, uint32 noOfBytesToRead,
  uint32 *pNBytesRead, IxStreamCbT pfn, IxStreamReadRspT *pReadRsp) = 0;

/*==============================================================
FUNCTION:  IxStream::Read()

DESCRIPTION:

  Sync version of the Read method. See comments for the UxBuf version of
  this method for details.

INPUT/OUTPUT PARAMETERS:
  @param  byte*   pBuf : pointer to byte array into which data is to be read
  @param  uint32  noOfBytesToRead : number of bytes to read
  @param  uint32* pNBytesRead : pointer to variable to hold the number
                                of bytes read
  @param  bool*   pEndOfStream: pointer to boolean used to indicate
                                end-of-stream

RETURN VALUE:
  Error Code. E_SUCCESS is returned if the Read completes successfully.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Read (byte *pBuf, uint32 noOfBytesToRead,
  uint32 *pNBytesRead, bool *pEndOfStream) = 0;

/*==============================================================
FUNCTION:  IxStream::Write()

DESCRIPTION:

  Request method to write a given number of bytes from a given buffer to
  the stream. If the request can be fulfilled immediately, E_SUCCESS
  is returned. Otherwise a request is sent to the appropriate server
  and E_AGAIN is returned. If this method returns E_AGAIN, the caller
  must call ipc_dispatch_msg() to wait for an IPC message from the server
  and to dispatch the received IPC message.

  It is not an error to write fewer bytes than requested.

  If Write() completes successfully, pWriteRsp->error will be set to
  E_SUCCESS.

INPUT/OUTPUT PARAMETERS:

  @param  UxBuf*  pBuf : pointer to UxBuf containing data to be written
  @param  uint32  noOfBytesToWrite : number of bytes to write
  @param  IxStreamCbT pfn : callback function to be called when the write is
                           complete
  @param pWriteRsp : pointer to data passed to callback function

RETURN VALUE:
  Error Code. E_AGAIN or E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Write (UxBuf *pBuf, uint32 noOfBytesToWrite,
  uint32 *pNBytesWritten, IxStreamCbT pfn, IxStreamWriteRspT *pWriteRsp) = 0;

/*==============================================================
FUNCTION:  IxStream::Write()

DESCRIPTION:

  Sync version of the Write method. The number of bytes written to the
  stream is returned using the third parameter.

  It is not an error to write fewer bytes than requested.

INPUT/OUTPUT PARAMETERS:

  @param  UxBuf*  pBuf : pointer to UxBuf containing data to be written
  @param  uint32  noOfBytesToWrite : number of bytes to write
  @param  uint32* pNBytesWritten : pointer to variable to hold the number
                                   of bytes written

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Write (UxBuf *pBuf, uint32 noOfBytesToWrite,
  uint32 *pNBytesWritten) = 0;

/*==============================================================
FUNCTION:  IxStream::Write()

DESCRIPTION:

  Request method to write given number of bytes from given buffer to stream.
  See comments for the UxBuf version of this method for details.

INPUT/OUTPUT PARAMETERS:

  @param  byte*   pBuf : pointer to char array containing data to be written
  @param  uint32  noOfBytesToWrite : number of bytes to write
  @param  IxStreamCbT pfn : callback function to be called when the write is
                           complete
  @param pWriteRsp : pointer to data passed to callback function

RETURN VALUE:
  Error Code. E_AGAIN or E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Write (byte *pBuf, uint32 noOfBytesToWrite,
  uint32 *pNBytesWritten, IxStreamCbT pfn, IxStreamWriteRspT *pWriteRsp) = 0;

/*==============================================================
FUNCTION:  IxStream::Write()

DESCRIPTION:

  Sync version of the Write method. See comments for the UxBuf version
  of this method for details.

INPUT/OUTPUT PARAMETERS:

  @param  byte*  pBuf : pointer to UxBuf containing data to be written
  @param  uint32  noOfBytesToWrite : number of bytes to write
  @param  uint32* pNBytesWritten : pointer to variable to hold the number
                                   of bytes written

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Write (byte *pBuf, uint32 noOfBytesToWrite,
  uint32 *pNBytesWritten) = 0;

/*==============================================================
FUNCTION:  IxStream::Seek()

DESCRIPTION:
  Seek to a specified offset in the stream.  Negative numbers are
  allowed in the case of the IX_STRM_SEEK_SEND and IX_STRM_SEEK_CURRENT
  parameters, so that it is possible to seek backwards.

INPUT/OUTPUT PARAMETERS:
  @param  int32 offset - offset to seek to
  @param  IxStreamWhenceType whence - from where to begin the seek
    IX_STRM_SEEK_START   - offset specified moves forward from 
                           beginning of the stream.  Offset must be 
                           a non-negative integer.
    IX_STRM_SEEK_END     - offset specified moved backward from the
                           end of the file.  Offset must be zero or
                           negative.
    IX_STRM_SEEK_CURRENT - offset specified moves forward or 
                           backward from the current offset.

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS:
  The current offset is set to the specified value.
==============================================================*/
virtual IxErrnoType Seek (const int32 offset,
  IxStreamWhenceType whence) = 0;

/*==============================================================
FUNCTION:  IxStream::Tell()

DESCRIPTION:
  Return the current offset.

INPUT/OUTPUT PARAMETERS:
  @param  uint32*  pOffset - pointer to location to store current offset.

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Tell (uint32* pOffset) = 0;

/*==============================================================
FUNCTION:  IxStream::Size()

DESCRIPTION:
  Return the size of the stream in bytes.  For a file, this will
  be the actual size of the file.  For a memory buffer, this will
  be the current size of the buffer.

INPUT/OUTPUT PARAMETERS:
  @param  uint32* pSize - pointer to location to store size.

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Size (uint32* pSize) = 0;

/*==============================================================
FUNCTION:  IxStream::Length()

DESCRIPTION:
  Return the number of verified bytes in the stream.  In the case 
  of a file this will be the size of the file and identical 
  to the Size() method, but in the case of a memory based stream,
  this will be the number of bytes written into the stream.

  For a progressively downloaded stream, it is possible that
  successive calls to Length will return different values because
  more data may have been downloaded in in the interim.

INPUT/OUTPUT PARAMETERS:
  @param  uint32* pLength - pointer to location to store length.
  @param  bool*   pEndOfData - pointer to boolean that indicates
          whether all the data in the stream is available

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType Length (uint32* pLength,
  bool *pEndOfData = NULL) = 0;

/*==============================================================
FUNCTION:  IxStream::UnreadLength()

DESCRIPTION:
  Return the number of unread bytes in the stream -- this is the
  number of bytes in the stream from the current offset to the end
  of the stream.

INPUT/OUTPUT PARAMETERS:
  @param  uint32* pLength - pointer to location to store length.

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType UnreadLength (uint32* pLength) = 0;

/*==============================================================
FUNCTION:  IxStream::CreateRef()

DESCRIPTION:
  Request method for creating a reference to the stream.
  If the request can be fulfilled immediately, E_SUCCESS is returned.
  Otherwise, the request is sent to the appropriate server, and
  E_AGAIN is returned. If this method returns E_AGAIN, the caller
  must call ipc_dispatch_msg() to wait for an IPC message from the server
  and to dispatch the received IPC message.

  A reference is an IxStream object that points to the data in the 
  calling stream. Changes made in this derived object will change
  the data in the parent stream.

  A reference to an object is a little different than the original
  object, in that the derived object is considered a subset of the
  original, and therefore does not have the ability to append to
  this subset of the data.  The offset of this new object is set to
  zero, and the offset of this object is independent of the offset 
  of the original.  The derived object is created with READ_ONLY
  access permissions.

  If CreateRef() completes successfully, pCreateRefRsp->error will be
  set to E_SUCCESS.

INPUT/OUTPUT PARAMETERS:
  @param  uint32 startOffset - start offset for start of reference
  @param  uint32 endOffset   - end offset for end of reference
  @param  IxStream** ppStream  - pointer to new stream
  @param  IxStreamCbT pfn - callback function
  @param  IxCallback* pCreateRefRsp - pointer to structure to be passed
                                      to callback function

RETURN VALUE:
  Error Code. E_AGAIN or E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType CreateRef(uint32 startOffset, uint32 endOffset,
  IxStream **ppStream, IxStreamCbT pfn, IxCallback *pCreateRefRsp) = 0;

/*==============================================================
FUNCTION:  IxStream::CreateRef()

DESCRIPTION:
  Sync version of CreateRef. See the comments to the async version
  of CreateRef for information about the CreateRef operation itself.

INPUT/OUTPUT PARAMETERS:
  @param  uint32 startOffset - start offset for start of reference
  @param  uint32 endOffset   - end offset for end of reference
  @param  IxStream** ppStream  - pointer to new stream

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType CreateRef(uint32 startOffset, uint32 endOffset,
  IxStream **ppStream) = 0;

/*==============================================================
FUNCTION:  IxStream::CreateCopy()

DESCRIPTION:
  Make a copy of the stream.  This stream by default will inherit
  the access writes of the source stream, but the access param
  can be used to change this for any purpose.  The new stream 
  object is an actual duplication of the source stream, and no
  longer points to the source.  Any changes made will be on the 
  copy alone.  Because of this, there is no limitation on the size
  that the new object can grow to.

INPUT/OUTPUT PARAMETERS:
  @param  uint32 startOffset - start offset for copy
  @param  uint32 endOffset   - end offset for copy
  @param  IxStream** ppStream  - pointer to new stream
  @param  IxStreamAccessRights access - change in access writes 

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType CreateCopy(uint32 startOffset, uint32 endOffset,
  IxStream **ppStream, IxStreamAccessRights access = IX_STRM_READWRITE) = 0;

/*==============================================================
FUNCTION:  IxStream::GetState()

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
virtual void GetState (uint8** ppStateData, uint32* pStateDataLen) = 0;

/*==============================================================
FUNCTION:  IxStream::SetState()

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
virtual void SetState (uint8* pStateData, uint32 stateDataLen) = 0;

/*==============================================================
FUNCTION:  IxStream::GetURI()

DESCRIPTION:
  Returns the URI associated with the stream.

INPUT/OUTPUT PARAMETERS:
  @param  char** ppURIPath        - pointer to URI path
  @param  char** ppURIScheme      - pointer to URI scheme

RETURN VALUE: E_SUCCESS on success, error code otherwise

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType GetURI (char** ppURIPath, char **ppURIScheme) = 0;

/*==============================================================
FUNCTION:  IxStream::MarkProgressive()

DESCRIPTION:
  Marks the stream as a progressively downloaded stream. This means
  that data can be appended to the end of the stream with the passage
  of time.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE: E_SUCCESS on success, error code otherwise

SIDE EFFECTS: None
==============================================================*/
virtual IxErrnoType MarkProgressive () = 0;

/*==============================================================
FUNCTION:  IxStream::IsProgressive()

DESCRIPTION:
  Checks if the stream is a progressively downloaded stream.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE: TRUE on success, FALSE otherwise

SIDE EFFECTS: None
==============================================================*/
virtual boolean IsProgressive () = 0;

/*------------------------------------------------------------------------
                              PRIVATE METHODS
 ------------------------------------------------------------------------*/

};

#endif /* __cplusplus */

/*------------------------------------------------------------------------
                               C WRAPPERS
 ------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

/*
** These functions are C wrappers for a subset of the IxStream API. The
** name of each C wrapper function indicates in an obvious way which
** C++ method it corresponds to.
*/

IxErrnoType ixstream_open (void *pStream, const char *uriString,
  IxStreamCbT pfn, IxStreamOpenRspT *pOpenRsp, IxStreamAccessRights access);

IxErrnoType ixstream_close_sync (void *pStream);

IxErrnoType ixstream_close_async (void *pStream, IxStreamCbT pfn,
  IxStreamCloseRspT *pCloseRsp);

IxErrnoType ixstream_read_sync (void *pStream, byte *pBuf,
  uint32 noOfBytesToRead, uint32 *pNBytesRead, boolean *pEndOfStream);

IxErrnoType ixstream_read_async (void *PStream, byte *pBuf,
  uint32 noOfBytesToRead, uint32 *pNBytesRead,
  IxStreamCbT pfn, IxStreamReadRspT *pReadRsp);

IxErrnoType ixstream_write_sync (void *pStream, byte *pBuf,
  uint32 noOfBytesToWrite, uint32 *pNBytesWritten);

IxErrnoType ixstream_write_async (void *pStream, byte *pBuf,
  uint32 noOfBytesToWrite, uint32 *pNBytesWritten,
  IxStreamCbT pfn, IxStreamWriteRspT *pWriteRsp);

IxErrnoType ixstream_seek (void *pStream, const int32 offset,
  IxStreamWhenceType whence);

IxErrnoType ixstream_tell (void *pStream, uint32* pOffset);

IxErrnoType ixstream_size (void *pStream, uint32* pSize);

IxErrnoType ixstream_length (void *pStream, uint32* pLength,
  boolean *pEndOfData);

IxErrnoType ixstream_unread_length (void *pStream, uint32* pLength);

IxErrnoType ixstream_create_ref_sync(void *pOldStream,
  uint32 startOffset, uint32 endOffset, void **ppNewStream);

IxErrnoType ixstream_create_ref_async(void *pOldStream,
  uint32 startOffset, uint32 endOffset, void **ppNewStream,
  IxStreamCbT pfn, void *pCreateRefRsp);

IxErrnoType ixstream_create_copy(void *pOldStream,
  uint32 startOffset, uint32 endOffset,
  void **ppNewStream, IxStreamAccessRights access);

IxErrnoType ixstream_mark_progressive (void *pStream);

boolean ixstream_is_progressive (void *pStream);

void ixstream_delete_stream (void *pStream);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __IXSTREAM_H__ */
