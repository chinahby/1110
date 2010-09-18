#ifndef __IXSTREAMCLIENT_H__
#define __IXSTREAMCLIENT_H__

/*===========================================================================

                              I X S T R E A M C L I E N T

                                  H E A D E R  F I L E

FILE:  IxStreamClient.h


GENERAL DESCRIPTION

  Defines the IxStreamClient class which is inherited from the IxStream class.
  Users of IxStream invoke methods of the IxStreamClient class.

  All reference documentation for API functions is contained in the
  file IxStream.h

PUBLIC CLASSES AND STATIC FUNCTIONS
  
INITIALIZATION & SEQUENCING REQUIREMENTS
  
  Copyright (c) 2006 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
===========================================================================*/


/*===========================================================================
                           
               INCLUDE FILES

============================================================================*/
#include "customer.h"
#include "IxErrno.h"
#include "IxStream.h"
#include "UxBuf.h"
#include "IxCallbackDef.h"
#include "UxURI.h"

/*===========================================================================

                           CLASS DEFINITION

===========================================================================*/

class IxStreamClient : public IxStream
{
  typedef struct
  {
    IxStreamKindE streamType;
    uint32        access;
    uint32        currOffset;
    uint32        startOffset;
    uint32        endOffset;
    char          path[IXSTRM_PATH_MAX+1];
    char          pURIScheme[IXSTRM_URI_SCHEME_MAX];
    uint32        pServerObject;
    uint8        *pBufState;
    uint32        bufStateLen;
  } IxStreamClientStateDataT;

  friend void IxStreamSetServerObject (IxStreamClient *pStreamClient,
    uint32 serverObject);

public :
  IxStreamClient (IxStreamKindE whichStream=IX_STRM_FILE);
  virtual ~IxStreamClient ();

  virtual IxErrnoType Open (const char *pPath,
    IxStreamCbT pfn,  
    IxStreamOpenRspT *pRsp,
    IxStreamAccessRights access);

  virtual IxErrnoType Open (const char *pPath, IxStreamAccessRights access);

  virtual IxErrnoType Close (IxStreamCbT pfn, IxStreamCloseRspT *pRsp);

  virtual IxErrnoType Close ();

  virtual IxErrnoType Remove (IxStreamCbT pfn, IxStreamRemoveRspT *pRsp);

  virtual IxErrnoType Remove ();

  virtual IxErrnoType Read (UxBuf *pBuf, uint32 noOfBytesToRead,
    uint32 *pNBytesRead, IxStreamCbT pfn, IxStreamReadRspT *pRsp);

  virtual IxErrnoType Read (byte *pBuf, uint32 noOfBytesToRead,
    uint32 *pNBytesRead, IxStreamCbT pfn, IxStreamReadRspT *pRsp);

  virtual IxErrnoType Read (UxBuf *pBuf, uint32 noOfBytesToRead,
    uint32 *pNBytesRead, bool *pEndOfStream);

  virtual IxErrnoType Read (byte *pBuf, uint32 noOfBytesToRead,
    uint32 *pNBytesRead, bool *pEndOfStream);

  virtual IxErrnoType Write (UxBuf *pBuf, uint32 noOfBytesToWrite,
    uint32 *pNBytesWritten);

  virtual IxErrnoType Write (byte *pBuf, uint32 noOfBytesToWrite,
    uint32 *pNBytesWritten);

  virtual IxErrnoType Write (UxBuf *pBuf, uint32 noOfBytesToWrite,
    uint32 *pNBytesWritten, IxStreamCbT pfn, IxStreamWriteRspT *pRsp);

  virtual IxErrnoType Write (byte *pBuf, uint32 noOfBytesToWrite,
    uint32 *pNBytesWritten, IxStreamCbT pfn, IxStreamWriteRspT *pRsp);

  virtual IxErrnoType Seek (const int32 offset, IxStreamWhenceType whence);

  virtual IxErrnoType Tell (uint32* pOffset);

  virtual IxErrnoType Size (uint32* pSize);

  virtual IxErrnoType Length (uint32* pLength, bool *pEndOfData = NULL);

  virtual IxErrnoType UnreadLength (uint32* pLength);

  virtual IxErrnoType CreateRef(uint32 startOffset, uint32 endOffset,
    IxStream **ppStream);

  virtual IxErrnoType CreateRef(uint32 startOffset, uint32 endOffset,
    IxStream **ppStream, IxStreamCbT pfn, IxCallback *pRsp);

  virtual IxErrnoType CreateCopy(uint32 startOffset, uint32 endOffset,
    IxStream **ppStream, IxStreamAccessRights access);

  virtual void GetState (uint8** ppStateData, uint32* pStateDataLen);

  virtual void SetState (uint8* pStateData, uint32 stateDataLen);

  virtual IxErrnoType GetURI (char** ppURIPath, char **ppURIScheme);

  virtual IxErrnoType MarkProgressive ();

  virtual boolean IsProgressive ();

private:

  IxErrnoType SetURIScheme (const char *pURIScheme);

  IxErrnoType CreateRefInternal(uint32 *pFirstOffset,
    uint32 *pLastOffset, IxStream **ppStream, IxStreamCbT pCb,
    IxCallback *pRsp, IxStreamClient **ppClientStream);

  UxBuf        *m_buf;
  uint32        m_access;
  uint32        m_currOffset;
  uint32        m_startOffset;
  uint32        m_endOffset;
  IxStreamKindE m_streamType;
  uint32        m_pServerObject;
  char          m_path[IXSTRM_PATH_MAX+1];
  char          m_pURIScheme[IXSTRM_URI_SCHEME_MAX+1];
  bool          m_isProgressive;

  static uint32 m_serverNodeId;
  static uint32 m_ipcMemPoolId;
};


#endif // _IXSTREAMCLIENT_H_
