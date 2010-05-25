#ifndef __IXSTREAMSOURCE_H__
#define __IXSTREAMSOURCE_H__

/*===========================================================================

                              I X S T R E A M S O U R C E

                                  H E A D E R  F I L E

FILE:  IxStreamSource.h


GENERAL DESCRIPTION

  Defines the IxStreamSource class which is inherited from the IxStream class.

  All reference documentation for API functions is contained in the
  file IxStream.h

PUBLIC CLASSES AND STATIC FUNCTIONS
  
INITIALIZATION & SEQUENCING REQUIREMENTS
  
  Copyright (c) 2006 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
===========================================================================*/


/*===========================================================================
                           
               INCLUDE FILES

============================================================================*/
#include "IxErrno.h"
#include "IxStream.h"
#include "UxBuf.h"
#include "AEESource.h"
#include "IxCallbackDef.h"
#include "IxFile.h"
#include "IxFileMgr.h"

extern "C"
{
  #include "rex.h"
}

/* Number of bytes to read ahead with each call to Length()
*/
#define IXSTREAMSOURCE_BUFSIZE (32*1024)

typedef struct
{
  rex_crit_sect_type critSect;
  IxFile            *pWriteFile;
  unsigned long      nRefs;
} IxStreamSourceSharedDataT;

typedef struct StreamSourceStateT
{
  IxStreamKindE              m_type;
  ISource                   *m_pSource;
  char                      *m_pFileName;
  IxStreamSourceSharedDataT *m_pSharedData;
  uint32                     m_initialOffset;
  bool                       m_isOpen;
  bool                       m_allDataRead;
} StreamSourceState;

/*===========================================================================

                           CLASS DEFINITION

===========================================================================*/

class IxStreamSource : public IxStream
{
public:
  IxStreamSource (ISource *pSource);

  IxStreamSource (IxStreamSource *pParent);

  virtual ~IxStreamSource ();

  virtual IxErrnoType Open (const char *pPath,
    IxStreamCbT pfn,  
    IxStreamOpenRspT *pRsp,
    IxStreamAccessRights access=IX_STRM_READWRITE);

  virtual IxErrnoType Open (const char *pPath,
    IxStreamAccessRights access=IX_STRM_READWRITE);

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

  IxErrnoType SourceToFile (uint32 nBytesRequested,
    uint32 *pNBytesTransferred);

  ISource                   *m_pSource;
  uint8                      m_readAheadBuf[IXSTREAMSOURCE_BUFSIZE];
  bool                       m_isOpen;
  bool                       m_allDataRead;
  char                      *m_pFileName;
  IxFileMgr                 *m_pFileMgr;
  IxStreamSourceSharedDataT *m_pSharedData;
  IxFile                    *m_pReadFile;
  uint32                     m_initialOffset;
};


#endif // _IXSTREAMSOURCE_H_
