#ifndef __IXSTREAMFILE_H__
#define __IXSTREAMFILE_H__

/*===========================================================================

                        I X  S T R E A M  F I L E               

                           H E A D E R  F I L E

FILE:  IxStreamFile.h

SERVICES : provides the service to read, write, open, close file streams

GENERAL DESCRIPTION : 

  Defines the IxStreamFile class which inherits from IxStream

 EXTERNALIZED FUNCTIONS

  Open()         - open the file stream

  Close()        - close the file stream

  Read()         - read data from the file stream

  Write()        - write data to a file stream

  Seek()         - seek to a specified location

  Tell()         - return current file offset

  Size()         - return the size of the file stream

  CreateRef()    - create a reference to a file stream

PUBLIC CLASSES AND STATIC FUNCTIONS
  
INITIALIZATION & SEQUENCING REQUIREMENTS
 
 Copyright (c) 2005-2006 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/src/IxStreamFile.h#4 $
  $DateTime: 2008/04/25 17:31:45 $
  $Author: rajeevg $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
2006/04/25  gr     Code clean-up, bug fixes, etc.
2006/04/24  gr     Updated Write function for new client-server architecture.
2006/04/24  gr     Updated Read function for new client-server architecture.
2006/04/24  gr     Added IPC messaging to Close, Seek and Length.
2006/04/20  gr     Removed functions that are no longer needed.
2006/04/19  gr     Renamed AsyncRead to Read and AsyncWrite to Write.
                   The sync versions should be removed later.
2006/03/29  gr     Deleted comments on public methods. These belong in
                   IxStream.h.
2006/03/29  gr     Renamed the IxContentStreamFile class to IxStreamFile.
2006/03/28  gr     Added support for remoting filestreams.
2006/01/12  ws     Include IxFileMgr instead of IxFile
2006/01/10  gr     Removed AEEFile dependencies.
2006/01/09  gr     Moved ISource-related functions into a separate library.
                   Modified to use IxFile interface. Removed dependencies on
                   AEEShell interface.
2005/12/22  gr     Added m_ prefix to all member variables.
2005/12/22  gr     Added support for CreateStreamFromSource.
2005/12/21  gr     Added support for creating an ISource object from an
                   IxStream object.
2005/12/19  gr     Added versions of Read and Write that take a pointer to
                   a character buffer rather than a UxBuf.
2005/12/15  gr     Added synchronous versions of Read and Write. Renamed
                   the Read and Write functions to distinguish between
                   the sync and async versions.
2005/12/12  gr     Completed renaming of all class names and local
                   variables from Qx* to Ix*.
2005/11/16  gr     Rewrote to match new API.
===========================================================================*/


/*===========================================================================
                           
               INCLUDE FILES

============================================================================*/

#include "customer.h"
#include "IxStream.h"
#include "IxStreamClient.h"
#include "IxFileMgr.h"
#include "UxBuf.h"
#include "IxErrno.h"

/*===========================================================================

                           CLASS DEFINITION

===========================================================================*/

class IxStreamFile
{
  public:

/*------------------------------------------------------------------------
                               PUBLIC METHODS
 ------------------------------------------------------------------------*/

  IxStreamFile ();

  ~IxStreamFile();

  int32 Open (const char* pPath, 
    IxStreamAccessRights access=IX_STRM_READWRITE);

  int32 Close ();

  int32 Read (UxBuf *pBuf, uint32 noOfBytesToRead);

  int32 Write (UxBuf *pBuf, uint32 noOfBytesToWrite);

  int32 Seek (const int32 offset, IxStreamWhenceType whence);

  int32 Size ();

  int32 Length ();

  int32 CreateRef(const char *pPath, IxStreamAccessRights access,
    uint32 firstOffset, uint32 lastOffset);

  int32 Remove ();

  IxErrnoType CreateCopy(uint32 startOffset, uint32 endOffset,
    IxStreamFile **ppStream, IxStreamAccessRights access = IX_STRM_READWRITE);

  /*------------------------------------------------------------------------
                               PRIVATE METHODS
  ------------------------------------------------------------------------*/
  private:

  int32 ReadIntoUxBuf (UxBuf *pBuf, uint32 nBytesToRead);

  int32 WriteFromUxBuf (UxBuf *pBuf, uint32 noOfBytesToWrite);

  int32 FileSize ();

  /*------------------------------------------------------------------------
                               PRIVATE MEMBERS
  ------------------------------------------------------------------------*/

  IxFileMgr*    m_pIxFileMgr;

  IxFile*       m_pIxFile;

  uint32        m_currOffset;

  uint32        m_startOffset;

  uint32        m_endOffset;

  int32         m_size;

  char          m_filePath[IXSTRM_PATH_MAX+1];
};

#endif
