#ifndef AEEIASTREAM_H
#define AEEIASTREAM_H

#include "AEEIBase.h"
//***********************************************************************************************
//
//  IAStream Base Class - This class is exposed as a simple way to read
//  file, network and memory streams.
//
//***********************************************************************************************

#define INHERIT_IAStream(iname) \
   INHERIT_IBase(iname);\
   void     (*Readable) (iname * po, void (*pfnNotify)(void*), void * pUser);\
   int32    (*Read)     (iname * po, void * pDest, uint32 nWant);\
   void     (*Cancel)   (iname * po, void (*pfnNotify)(void*), void * pUser)

AEEINTERFACE_DEFINE(IAStream);

static __inline uint32 IASTREAM_AddRef(IAStream *me) 
{
   return AEEGETPVTBL(me, IAStream)->AddRef(me);
}
static __inline uint32 IASTREAM_Release(IAStream *me) 
{
   return AEEGETPVTBL(me, IAStream)->Release(me);
}
static __inline void IASTREAM_Readable(IAStream *me, void (*pfnNotify)(void*), 
                                       void* pUser) 
{
   AEEGETPVTBL(me, IAStream)->Readable(me, pfnNotify, pUser);
}

static __inline int32 IASTREAM_Read(IAStream *me, void* pDest, int32 nWant) 
{
   return AEEGETPVTBL(me, IAStream)->Read(me, pDest, nWant);
}

static __inline void IASTREAM_Cancel(IAStream *me) 
{
   AEEGETPVTBL(me, IAStream)->Cancel(me, 0, 0);
}

/*=======================================================================
 INTERFACES DOCUMENTATION
=======================================================================

IAStream Interface

The IAStream Interface reads data from an asynchronous stream. It is an abstract
interface implemented by classes providing access to data that may not all be available
at once and must be retrieved asynchronously. At present, the IFile and ISocket
classes implement the IAStream Interface. The IImage Interface and ISoundPlayer
Interface implement IImage_SetStream() and ISOUNDPLAYER_SetStream() functions that
allow an IAStream Interface to be supplied as the source of image or sound input.

IASTREAM_Read() reads data from the stream and returns the number of bytes read.
If no data is available for reading, this function returns the value AEE_STREAM_WOULDBLOCK.
In the latter case, call IASTREAM_Readable() to schedule a callback function that
is invoked when there is more data available. IASTREAM_Cancel() cancels a callback
that was scheduled with IASTREAM_Readable().
===H2>
To use an IAStream instance to retrieve data asynchronously
===/H2>
===p>
1.   Call ISHELL_CreateInstance() to create an instance of a class that implements
the IAStream interface.

2.   Call IASTREAM_Read() to read the required number of bytes of data from the
stream. Because IAStream Interface is an abstract interface, call IFILE_Read() for
files, or ISOCKET_Read() for sockets.

3.   If IASTREAM_Read() returns AEE_STREAM_WOULDBLOCK, call IASTREAM_Readable()
to schedule a callback to try again later.

4.   If IASTREAM_Read() reads fewer than the required number of bytes, call the
function again to read the remaining data.

5.   Repeat steps 2-4 until all the data has been received on the stream.
===H2>
To use an asynchronous stream as the source of image or sound data
===/H2>
===p>
1.   Call ISHELL_CreateInstance() to create an instance of a class that implements
the IAStream Interface.

2.   Call ISHELL_CreateInstance() to create an instance of IImage Interface or ISoundPlayer
Interface.

3.   Call IImage_Notify() or ISOUNDPLAYER_RegisterNotify() to schedule a callback
that is invoked when the image or sound data has been completely retrieved.

4.   Call IImage_SetStream() or ISOUNDPLAYER_SetStream() to associate the stream
created in step 1 with the IImage or ISoundPlayer instance created in step 2. This
initiates the retrieval of image or sound data on the stream.

5.   When retrieval is complete, the callback registered in step 3 is invoked. Use
functions in the IImage Interface or the ISoundPlayer Interface to access the retrieved
image or sound data.

The following header file is required:~
AEE.h

=============================================================================
IASTREAM_AddRef()
This function is inherited from IBase_AddRef().
=======================================================================
IASTREAM_Release()
This function is inherited from IBase_Release().
=======================================================================
IASTREAM_Cancel()

Description:
    This function cancels a callback that was scheduled with IASTREAM_Readable().

Prototype:
    void IASTREAM_Cancel(IAStream * pIAStream)

Parameters:
    pIAStream :  Pointer to the IAStream Interface object.

Return Value:
    None

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   PFNNOTIFY
    IASTREAM_Read()
    IASTREAM_Readable()

=======================================================================
IASTREAM_Read()

Description:
    This function attempts to read data from a stream, and returns the number of
bytes read. If no data is available for reading, it returns the value AEE_STREAM_WOULDBLOCK.

Prototype:
    int32 IASTREAM_Read
    (
    IAStream * pIAStream,
    void * pBuffer,
    uint32 dwCount
    )

Parameters:
    pIAStream :  Pointer to the IAStream Interface object.
    pBuffer :  Pointer to the buffer into which the data is read.
    dwCount :  Number of bytes to read.

Return Value:
    Number of bytes read, if data is available for reading.

    0 (zero), if all available data has been read.

    AEE_STREAM_WOULDBLOCK, if no data is currently available.

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
    IASTREAM_Readable()
    IASTREAM_Cancel()

=======================================================================
IASTREAM_Readable()

Description:
    This function registers a callback that checks whether data is available to
be read. It is called when the IASTREAM_Read() returns AEE_STREAM_WOULDBLOCK.

Prototype:
    void IASTREAM_Readable
    (
    IAStream * pIAStream,
    PFNNOTIFY pfn,
    void * pUser
    )

Parameters:
    pIAStream :  Pointer to the IAStream Interface object.
    pfn :  Pointer to the callback function.
    pUser :  Pointer to user-specified data that is passed as a parameter to the
callback function.

Return Value:
   None

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
    PFNNOTIFY
    IASTREAM_Read()
    IASTREAM_Cancel()

=======================================================================*/

#endif //AEEIASTREAM_H

