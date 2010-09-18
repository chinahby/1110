#ifndef AEEUNZIPSTREAM_H
#define AEEUNZIPSTREAM_H
/*============================================================================
FILE:        AEEUnzipStream.h

SERVICES:  Unzip Stream Services

DESCRIPTION:
   This file provides definitions for an uncompressed stream for a given
   compressed IAStream


PUBLIC CLASSES:  Not Applicable


INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 1999-2004,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

#include "AEE.h"

typedef struct _IUnzipAStream   IUnzipAStream;

/******************************************************************************

  IUnzipAStream - Interface to read zlib compressed data streams as
                  uncompressed streams
*******************************************************************************/

QINTERFACE(IUnzipAStream)
{
   DECLARE_IBASE(IUnzipAStream)
   DECLARE_IASTREAM(IUnzipAStream)

   void     (*SetStream)(IUnzipAStream * po, IAStream * pIAStream);
};

#define IUNZIPASTREAM_AddRef(p)           GET_PVTBL(p,IUnzipAStream)->AddRef(p)
#define IUNZIPASTREAM_Release(p)          GET_PVTBL(p,IUnzipAStream)->Release(p)
#define IUNZIPASTREAM_Readable(p,pfn,pu)  GET_PVTBL(p,IUnzipAStream)->Readable(p,pfn,pu)
#define IUNZIPASTREAM_Read(p,pd,n)        GET_PVTBL(p,IUnzipAStream)->Read(p,pd,n)
#define IUNZIPASTREAM_Cancel(p)           GET_PVTBL(p,IUnzipAStream)->Cancel(p,NULL,NULL)
#define IUNZIPASTREAM_SetStream(p,ps)     GET_PVTBL(p,IUnzipAStream)->SetStream(p,ps)

/*=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IUnzipAStream

Description:
   The IUnzipAStream interface extends the IAStream interface to allow
   a compressed IAStream to be uncompressed and read as a stream. An
   instance of the IUnzipAStream can be created using the
   ISHELL_CreateInstance method (with class ID AEECLSID_UNZIPSTREAM).
   Only data compressed using the deflate algorithm (used by gzip)
   can be decompressed using this interface.

   In addition to the standard IBase methods (AddRef() and Release()),
   and the standard IAStream methods (Readable(), Read(), and Cancel()),
   the IUnzipAStream interface has the following additional method(s):

      IUNZIPASTREAM_SetStream() - Sets the compressed input stream

      The following header file is required:

AEEUnzipStream.h    

====================================================

IUNZIPASTREAM_AddRef()

Description:
This function is inherited from IBASE_AddRef().
====================================================

IUNZIPASTREAM_Release()

Description:
This function is inherited from IBASE_Release().
====================================================

IUNZIPASTREAM_Readable()

Description:
This function is inherited from IASTREAM_Readable().
====================================================

IUNZIPASTREAM_Read()

Description:
This function is inherited from IASTREAM_Read().
====================================================

IUNZIPASTREAM_Cancel()

Description:
This function is inherited from IASTREAM_Cancel().
====================================================



Function: IUNZIPASTREAM_SetStream()

Description:
   This method allows the user to set an input compressed stream as the
   source for the IUnzipAStream. The user can create an IAStream of a
   compressed file (such as a .gzip file), memory buffer, or a socket,
   and pass that as input to the IUnzipAStream object. Subsequently,
   IUNZIPASTREAM_Read would uncompress the input stream and read the number
   of bytes requested, IUNZIPASTREAM_Readable would set a callback when
   more uncompressed data is available to be read, etc.

Prototype:
   void     IUNZIPASTREAM_SetStream(
                             IUnzipAStream * pIUnzipAStream,
                             IAStream * pInIAStream
                           )

Parameters:
   pIUnzipAStream : Pointer to the IUnzipAStream interface
   pInIAStream    : Pointer to the compressed input stream


Return Value:
   None

Comments:
   None

Side Effects:
   None

Version
   Introduced BREW Client 2.0

See Also
   None

=======================================================================*/

#endif //AEEUNZIPSTREAM_H
