#ifndef __IXSTREAMBUF_H__
#define __IXSTREAMBUF_H__

/*===========================================================================

                       I X S T R E A M B U F            

                          H E A D E R  F I L E

FILE:  IxStreamBuf.h


GENERAL DESCRIPTION

  This header file defines the IxStreamBuf class which is inherited 
  from IxStream class.  This class works entirely out of memory buffers, 
  currently defined in UxBuf.h

  All reference documentation for API functions is contained in the
  file IxStream.h

PUBLIC CLASSES AND STATIC FUNCTIONS
  
INITIALIZATION & SEQUENCING REQUIREMENTS
  
  Copyright (c) 2005 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                     EDIT HISTORY FOR FILE
                     
when      who    what, where, why
--------   ---    ----------------------------------------------------------
2007/11/29  gr    Added MarkProgressive and IsProgressive methods.
2007/11/21  gr    Added UnreadLength method.
2006/10/25  gr    Added a sync version of Open.
2006/06/29  gr    Added a sync version of Close for internal use.
2006/06/16 ssm    Move to IxCallback
2006/05/03  gr    Added an extra parameter to the Read and Write methods
                  to allow an immediate successful return.
2006/04/19  gr    Renamed AsyncRead to Read and AsyncWrite to Write.
                  The sync versions should be removed later.
2005/12/29 jee    Initial version
===========================================================================*/


/*===========================================================================
                     
            INCLUDE FILES

============================================================================*/
#include "customer.h"
#include "IxErrno.h"
#include "IxStream.h"
#include "UxBuf.h"

class IxStreamBuf;

typedef struct StreamBufStateT {
   IxStreamKindE m_type;
   UxBuf        *m_buf;
   uint32        m_access;
   bool          m_opened;
   uint32        m_beg;
   uint32        m_end;
   uint32        m_pos;
   IxStreamBuf  *m_parent;
   uint32        m_refcount;
} StreamBufState;

/*===========================================================================

                     CLASS DEFINITION

===========================================================================*/

class IxStreamBuf : public IxStream
{
public :
   IxStreamBuf();
   IxStreamBuf(UxBuf *pBuf);
   
   virtual ~IxStreamBuf();

   virtual IxErrnoType Open (const char *uriString, 
     IxStreamCbT pfn,   
     IxStreamOpenRspT *pOpenRsp,
     IxStreamAccessRights access=IX_STRM_READWRITE);

   virtual IxErrnoType Open (const char *uriString, 
     IxStreamAccessRights access=IX_STRM_READWRITE);

   IxErrnoType Open (const uint32 allocSize, 
      IxStreamAccessRights access=IX_STRM_READWRITE);

   IxErrnoType Open (const uint32 allocSize,
      IxStreamCbT pfn,   
      IxStreamOpenRspT *pOpenRsp,
      IxStreamAccessRights access=IX_STRM_READWRITE);

   virtual IxErrnoType Close (IxStreamCbT pfn, IxStreamCloseRspT *pCloseRsp);

   virtual IxErrnoType Close ();

   virtual IxErrnoType Remove (IxStreamCbT pfn, IxStreamRemoveRspT *pRemoveRsp);

   virtual IxErrnoType Remove ();

   virtual IxErrnoType Read (UxBuf *pBuf, uint32 noOfBytesToRead,
     uint32 *pNBytesRead, IxStreamCbT pfn, IxStreamReadRspT *pReadRsp);

   virtual IxErrnoType Read (byte *pBuf, uint32 noOfBytesToRead,
     uint32 *pNBytesRead, IxStreamCbT pfn, IxStreamReadRspT *pReadRsp);

   virtual IxErrnoType Read (UxBuf *pBuf, uint32 noOfBytesToRead,
     uint32 *pNBytesRead, bool *pEndOfStream);

   virtual IxErrnoType Read (byte *pBuf, uint32 noOfBytesToRead,
     uint32 *pNBytesRead, bool *pEndOfStream);

   virtual IxErrnoType Write (UxBuf *pBuf, uint32 noOfBytesToWrite,
     uint32 *pNBytesWritten);

   virtual IxErrnoType Write (byte *pBuf, uint32 noOfBytesToWrite,
     uint32 *pNBytesWritten);

   virtual IxErrnoType Write (UxBuf *pBuf, uint32 noOfBytesToWrite,
     uint32 *pNBytesWritten, IxStreamCbT pfn, IxStreamWriteRspT *pWriteRsp);

   virtual IxErrnoType Write (byte *pBuf, uint32 noOfBytesToWrite,
     uint32 *pNBytesWritten, IxStreamCbT pfn, IxStreamWriteRspT *pWriteRsp);

   virtual IxErrnoType Seek (const int32 offset, IxStreamWhenceType whence);

   virtual IxErrnoType Tell (uint32* pOffset);

   virtual IxErrnoType Size (uint32* pSize);

   virtual IxErrnoType Length (uint32* pLength, bool *pEndOfData = NULL);

   virtual IxErrnoType UnreadLength (uint32* pLength);

   virtual IxErrnoType CreateRef(uint32 startOffset, uint32 endOffset,
     IxStream **ppStream, IxStreamCbT pfn,
     IxCallback *pRsp);

   virtual IxErrnoType CreateRef(uint32 startOffset, uint32 endOffset,
     IxStream **ppStream);

   virtual IxErrnoType CreateCopy(uint32 startOffset, uint32 endOffset,
     IxStream **ppStream, IxStreamAccessRights access);

   virtual void GetState( uint8** stateDataPtr, uint32* stateDataLenPtr );

   virtual void SetState( uint8* stateDataPtr, uint32 stateDataLen );

   virtual IxErrnoType GetURI (char **ppURIPath, char **ppURIScheme);

   virtual IxErrnoType MarkProgressive ();

   virtual boolean IsProgressive ();

private:
   IxErrnoType OpenInternal (
      uint32 allocSize,
      IxStreamCbT pfn,
      IxStreamOpenRspT *pOpenRsp,
      IxStreamAccessRights access
      );

   void AddRef(void);
   void ReleaseRef(void);

   UxBuf *m_buf;
   uint32 m_access;
   bool   m_opened;
   uint32 m_beg;
   uint32 m_end;
   uint32 m_pos;

   IxStreamBuf *m_parent;
   uint32      m_refcount;

   bool        m_isProgressive;
   bool        m_dataComplete;
};


#endif // _IXSTREAMBUF_H_
