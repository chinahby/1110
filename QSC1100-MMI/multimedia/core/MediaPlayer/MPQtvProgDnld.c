/*===========================================================================
FILE: MPQtvProgDnld.c

SERVICES: Sample implementation of downloading media clip data via network
          using the QTV Progressive Download API.

DESCRIPTION
  This file contains the QTV implementation of downloading media clips via HTTP.

       Copyright  2000-2006 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

$Header: //depot/asic/msmshared/apps/MediaPlayer/MPQtvProgDnld.c#6 $

===========================================================================*/

/*===========================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/
#include "customer.h"

#include "MPDefines.h"
#include "MPUtils.h"

#include "AEEStdLib.h"
#ifdef FEATURE_APP_MPEG4
#include "AEEMediaMPEG4.h"
#endif
#include "AEEMedia.h"                  // AEE Multimedia Services

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2)

static char *archiveDataBuffer = NULL;
static uint32 archiveDataSize = 0;
static uint32 archiveEnd  = 0;

#define PS_BUF_DFLT_SIZE 1000000

/*-------------------------------------------------------------------
            Static methods
-------------------------------------------------------------------*/
#ifdef MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_SUPPORTED_TYPE
static AEEMediaHTTPDLAction MimeSupportedStopCB(char *MIMEtypeString)
{
  /* Return MM_MPEG4_QTV_TO_STOP */
  DBGPRINTF_MED("MP: MimeSupportedStopCB returns MM_MPEG4_QTV_TO_STOP");
  return MM_MPEG4_QTV_TO_STOP;
}

static AEEMediaHTTPDLAction NoMimeSupportedCB(char *MIMEtypeString)
{
  /* Return MM_MPEG4_QTV_TO_HANDLE_CONTENT and let QTV handle everything. */
  DBGPRINTF_MED("MP: NoMimeSupportedCB returns MM_MPEG4_QTV_TO_HANDLE_CONTENT");
  return MM_MPEG4_QTV_TO_HANDLE_CONTENT;
}

static AEEMediaHTTPDLAction IsMimeSupportedCB(char *MIMEtypeString)
{
  /* Returning MM_MPEG4_QTV_TO_HANDLE_CONTENT means not to use these callbacks. */
  if ((MIMEtypeString == NULL) || (MIMEtypeString[0] == 0))
  {
    DBGPRINTF_MED("MP: IsMimeSupportedCB returns MM_MPEG4_QTV_TO_HANDLE_CONTENT");
    return MM_MPEG4_QTV_TO_HANDLE_CONTENT;
  }

  /* Assume requested MIME type is valid, Mediaplayer will "simulate" decryption. */
  DBGPRINTF_MED("MP: IsMimeSupportedCB returns MM_MPEG4_APP_TO_HANDLE_CONTENT");
  return MM_MPEG4_APP_TO_HANDLE_CONTENT;
}
#endif  // MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_SUPPORTED_TYPE

#ifdef MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_DATA_SIZE
static void FetchBufferedDataSizeCB(uint32 *bufDataSize)
{
  if (bufDataSize)
  {
    *bufDataSize = archiveDataSize;
    DBGPRINTF_MED("MP: FetchBufferedDataSizeCB returns size = %d", archiveDataSize);
  }
  else
  {
    DBGPRINTF_HIGH("MP: FetchBufferedDataSizeCB give NULL size pointer!");
  }
}
#endif  // MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_DATA_SIZE

#ifdef MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_DATA
static uint32 FetchBufferedDataCB(void *dataBuf, uint32 readSize, uint32 readPos, uint32 trackId)
{
  int copySize;

  if (dataBuf == NULL)
  {
    DBGPRINTF_HIGH("MP: FetchBufferedDataCB receives NULL buffer pointer!");
    return 0;
  }
  if (readSize == 0)
  {
    DBGPRINTF_HIGH("MP: FetchBufferedDataCB receives zero read size!");
    return 0;
  }

  /* fetch frame from archive buffer and put into dataBuf */
  if (readPos + readSize <= archiveDataSize)
    copySize = readSize;
  else {
    /* if starting within archived data, but reading past end */
    if (readPos < archiveDataSize)
      copySize = archiveDataSize - readPos;
    else  /* else trying to read past end of archived data */
    {
      DBGPRINTF_HIGH("MP: FetchBufferedDataCB readPos=%d,readSize=%d past end of archive size=%d",
        readPos,readSize,archiveDataSize);
      return 0;
    }
  }

  (void)MEMCPY(dataBuf, &archiveDataBuffer[readPos], copySize);
  DBGPRINTF_MED("MP: FetchBufferedDataCB returns %d bytes copied", copySize);
  return copySize;
}
#endif  // MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_DATA

/*-------------------------------------------------------------------
            Exported methods
-------------------------------------------------------------------*/

void MP_WriteIntoArchiveBuffer(char *buffer, int size)
{
  if (size <= 0)
  {
    DBGPRINTF_HIGH("MP: MP_WriteIntoArchiveBuffer passed bad size = %d!", size);
    return;
  }
  if (!buffer)
  {
    DBGPRINTF_HIGH("MP: MP_WriteIntoArchiveBuffer passed NULL buffer pointer!");
    return;
  }

  /* Do we need to allocate the archive buffer for the first time? */
  if (archiveDataBuffer == NULL)
  {
    uint32 allocAmount = PS_BUF_DFLT_SIZE;

    if (size > PS_BUF_DFLT_SIZE)
      allocAmount = size;

    archiveDataBuffer = (char *)MALLOC(allocAmount);
    if (!archiveDataBuffer)
    {
      DBGPRINTF_HIGH("MP: MP_WriteIntoArchiveBuffer can't allocate archiveDataBuffer of size %d!", allocAmount);
      return;
    }

    archiveEnd = allocAmount;
    archiveDataSize = 0;
  }

  /* Do we need to increase the size of the archive buffer? */
  if ((archiveDataSize + size) > archiveEnd)
  {
    /* existing buffer too small to add incoming data to, so realloc it larger */
    char *newBuffer;
    uint32 newSize;

    newSize = archiveEnd + size;
    newBuffer = (char *)MALLOC(newSize);
    if (!newBuffer)
    {
      DBGPRINTF_HIGH("MP: MP_WriteIntoArchiveBuffer can't allocate newBuffer of size %d!", newSize);
      return;
    }

    (void)MEMCPY(newBuffer, archiveDataBuffer, archiveDataSize);
    FREE(archiveDataBuffer);
    archiveDataBuffer = newBuffer;
  }

  /* Data fits, archive the data (save buffer contents) into archiveDataBuffer. */
  (void)MEMCPY(&archiveDataBuffer[archiveDataSize], buffer, size);
  archiveDataSize += size;

  /* QTV buffer will be freed by caller */
  DBGPRINTF_HIGH("MP: MP_WriteIntoArchiveBuffer archive size = %d", archiveDataSize);
}

void  MP_ArchiveCleanup()
{
  FREEIF(archiveDataBuffer);
  archiveDataSize = 0;
  archiveEnd = 0;
}

void MP_RegisterCallbacksForQTVDownload(IMedia *pMedia, CMimicEncryptDnld mimicEncryptDnld)
{
  DBGPRINTF_MED("MP: register callbacks, mimicEncryptDnld = %d", mimicEncryptDnld);
  if (pMedia == NULL)
  {
    DBGPRINTF_HIGH("MP: pMedia is NULL!  Callbacks NOT registered!");
    return;
  }

#ifdef MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_DATA_SIZE
  {
    int nRet;
    nRet = IMEDIA_SetMediaParm(pMedia, MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_DATA_SIZE,
      (int32)FetchBufferedDataSizeCB, 0);
    DBGPRINTF_MED("MP: IMEDIA_SetMediaParm(MM_MP4_PARAM_HTTP_REGISTER...BUFFER_DATA_SIZE) returns %d = %s",
                  nRet, MP_ErrorStateToString(nRet));
  }
#endif
#ifdef MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_DATA
  {
    int nRet;
    nRet = IMEDIA_SetMediaParm(pMedia, MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_DATA,
      (int32)FetchBufferedDataCB, 0);
    DBGPRINTF_MED("MP: IMEDIA_SetMediaParm(MM_MP4_PARAM_HTTP_REGISTER...BUFFER_DATA) returns %d = %s",
                  nRet, MP_ErrorStateToString(nRet));
  }
#endif
#ifdef MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_SUPPORTED_TYPE
  {
    int nRet;
    if (mimicEncryptDnld == MIMIC_ENCRYPT_DNLD_ENABLE)
    {
      nRet = IMEDIA_SetMediaParm(pMedia, MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_SUPPORTED_TYPE,
          (int32)IsMimeSupportedCB, 0);
    }
    else if (mimicEncryptDnld == MIMIC_ENCRYPT_DNLD_STOP)
    {
      nRet = IMEDIA_SetMediaParm(pMedia, MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_SUPPORTED_TYPE,
          (int32)MimeSupportedStopCB, 0);
    }
    else
    {
      nRet = IMEDIA_SetMediaParm(pMedia, MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_SUPPORTED_TYPE,
        (int32)NoMimeSupportedCB, 0);
    }
    DBGPRINTF_MED("MP: IMEDIA_SetMediaParm(MM_MP4_PARAM_HTTP_REGISTER...SUPPORTED_TYPE) %d returns %d = %s",
                  mimicEncryptDnld, nRet, MP_ErrorStateToString(nRet));
  }
#endif
}

#endif  // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD || FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
#endif  // FEATURE_APP_MPEG4

