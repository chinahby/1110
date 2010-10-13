/*=============================================================================
  FILE: MPUtils.cpp

  SERVICES: Util object Handlers
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPUtils.c#27 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#include "MPDefines.h"
#include "MPUtils.h"
#include "MPIO.h"
#ifndef MP_LTK_OEMEFS_CONDITIONAL
#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
  #include "AEEMediaMPEG4.h"
  #ifndef IMEDIA_GetPlaybackPositions
    #include "OEMMediaMPEG4.h"
  #endif //IMEDIA_GetPlaybackPositions
#endif //FEATURE_APP_MEDIA_VIDEO
#include "AEESource.h"     // ISource and IPeek definitions
#include "AEEFile.h"       // IFile, IFileMgr
#include "AEEMimeTypes.h"  // MIME definitions
#include "BREWVersion.h"
#include "fs.h"
#include "BREWVersion.h"
#if MIN_BREW_BUILD(4,0,1,5)
#include "AEEConstFile.h"
#else
#include "AEE_OEMFile.h"
#endif

#if MIN_BREW_VERSION(4,0)
#include "OEMefs.h"
#else
#include "OEMFS.h"
#endif //MIN_BREW_VERSION(4,0)

#endif //MP_LTK_OEMEFS_CONDITIONAL

#if MIN_BREW_VERSION(4,0)
#include "AEEGROUPID_USBHMS.bid"
#endif //#if MIN_BREW_VERSION(4,0)

static int MP_DetectAVIType(unsigned char *pBuf, uint32 dwBufSize, const char **type);

// this function creates itemstore object
stMPItemStore* MPItemStoreMake(IShell*  pIShell)
{
  stMPItemStore* pItemStore = NULL;
  
  if(pIShell == NULL)
    return pItemStore;

  pItemStore = (stMPItemStore*)MALLOC(sizeof(stMPItemStore));
  if(pItemStore)
  {
    pItemStore->m_pVector = NULL;
    if(SUCCESS != ISHELL_CreateInstance(pIShell, AEECLSID_VECTOR, (void **) &pItemStore->m_pVector))
    {
      DBGPRINTF_ERROR("MP: failed to create Ivector instance.");
      MPItemStoreFree(pItemStore);
    }
    else 
    {
      // initialize with size of 1, it will reallocated when adding more items
      if(SUCCESS != IVector_Init(pItemStore->m_pVector, 1))
      {
        DBGPRINTF_ERROR("MP: failed to init Ivector.");
        MPItemStoreFree(pItemStore);
      }
    }
  }
  return pItemStore;
}
void MPItemStoreFree(stMPItemStore* pItemStore)
{
  if(pItemStore && pItemStore->m_pVector)
  {
    (void)IVector_Release(pItemStore->m_pVector);
  }
  FREEIF(pItemStore);
}
// returns item count
uint32 MPItemStoreGetSize(const stMPItemStore* pItemStore)
{
  if(pItemStore && pItemStore->m_pVector)
  {
    return IVector_Size(pItemStore->m_pVector);
  }
  return 0;
}
// gets an item at given location
void* MPItemStoreGetAt(stMPItemStore* pItemStore, int idx)
{
  uint32 nSize;
  nSize = MPItemStoreGetSize(pItemStore);
  if((idx >= 0) && (nSize > 0) && (idx < (int)nSize))
    return IVector_ElementAt(pItemStore->m_pVector, (uint32)idx);
  return NULL;
}

// this function returns the latest added item pointer
void* MPItemStoreGetTop(stMPItemStore* pItemStore)
{
  int nSize = (int)MPItemStoreGetSize(pItemStore);
  return MPItemStoreGetAt(pItemStore, nSize - 1);
}
// add an item
boolean MPStackPush(const stMPItemStore* pItemStore, void* pData)
{
  if(pItemStore && pItemStore->m_pVector)
  {
    return (SUCCESS == IVector_AddElement(pItemStore->m_pVector, (void*)pData))?TRUE:FALSE;
  }
  return FALSE;
}
// return the latest added item and remove it from the list.
void* MPStackPop(stMPItemStore* pItemStore)
{
  uint32 nSize;
  nSize = MPItemStoreGetSize(pItemStore);
  if(nSize > 0)
  {
    return IVector_RemoveElementAt(pItemStore->m_pVector, nSize - 1);
  }
  return NULL;
}



void MPStackDump(stMPItemStore* pItemStore)
{
  int nSize;
  int i;

  nSize = (int)MPItemStoreGetSize(pItemStore);
  if(nSize > 0)
  {
    for(i = nSize - 1; i >=0; i--)
    {
      DBGPRINTF_HIGH("MP: stack dump, i = %3d, %s.", i, 
                IVector_ElementAt(pItemStore->m_pVector, i)); 
    }
  }
}


AECHAR* MPWStrDup(const char* pName)
{
  AECHAR * pwstr = NULL;
  if(pName)
  {
    pwstr = (AECHAR *) MALLOC((STRLEN(pName)+1)*sizeof(AECHAR));
    if(pwstr)
      (void)STRTOWSTR(pName, pwstr, (STRLEN(pName)+1)*sizeof(AECHAR));
  }
  return pwstr;
}

char* MPStrDup(const char* pName)
{
  char * pstr = NULL;
  if(pName)
  {
    pstr = (char *) MALLOC((STRLEN(pName)+1)*sizeof(char));
    if(pstr)
      (void)STRLCPY(pstr, pName, (STRLEN(pName)+1)*sizeof(char));
  }
  return pstr;
}

/*===========================================================================
  This function is used to help emit AEEMedia state to debug output.
===========================================================================*/
char *MP_MediaStateToString(int mediaState)
{
  switch (mediaState)
  {
  case MM_STATE_IDLE:
    return("MM_STATE_IDLE");
  case MM_STATE_READY:
    return("MM_STATE_READY");
  case MM_STATE_PLAY:
    return("MM_STATE_PLAY");
  case MM_STATE_RECORD:
    return("MM_STATE_RECORD");
  case MM_STATE_PLAY_PAUSE:
    return("MM_PLAY_PAUSE");
  case MM_STATE_RECORD_PAUSE:
    return("MM_STATE_RECORD_PAUSE");
  default:
    return("Undefined");
  }
}

/*===========================================================================
  This function is used to help emit error state to debug output.
===========================================================================*/
char *MP_ErrorStateToString(int errorState)
{
  switch (errorState)
  {
  // Multimedia (IMedia) Errors
  case MM_PENDING:          // Command accepted (queued). Result returned via registered callback.
    return("MM_PENDING");
  case MM_ERROR:            // Command failed
    return("MM_ERROR");
  case MM_EBADMEDIADATA:    // Bad media data
    return("MM_EBADMEDIADATA");
  case MM_ENOMEDIAMEMORY:   // No memory for IMedia objects
    return("MM_ENOMEDIAMEMORY");
  case MM_EMIDIOUTNOTOPEN:   // MIDI Out Device is not open
    return("MM_EMIDIOUTNOTOPEN");
  case MM_EPEEK:            // IPeek error occurred
    return("MM_EPEEK");
  case MM_ENOTENOUGHTDATA:  // Operation aborted due to insufficient data
    return("MM_ENOTENOUGHTDATA");
  // Basic error codes
  case SUCCESS:             // No error
    return("SUCCESS");
  case EFAILED:             // General failure
    return("EFAILED");
  case ENOMEMORY:           // Out of memory
    return("ENOMEMORY");
  case ECLASSNOTSUPPORT:    // Specified class unsupported
    return("ECLASSNOTSUPPORT");
  case EVERSIONNOTSUPPORT:  // Version not supported
    return("EVERSIONNOTSUPPORT");
  case EALREADYLOADED:      // Object already loaded
    return("EALREADYLOADED");
  case EUNABLETOLOAD:       // Unable to load object/applet
    return("EUNABLETOLOAD");
  case EUNABLETOUNLOAD:     // Unable to unload object/applet
    return("EUNABLETOUNLOAD");
  case EALARMPENDING:       // Alarm is pending
    return("EALARMPENDING");
  case EINVALIDTIME:        // Invalid time
    return("EINVALIDTIME");
  case EBADCLASS:           // NULL class object
    return("EBADCLASS");
  case EBADMETRIC:          // Invalid metric specified
    return("EBADMETRIC");
  case EEXPIRED:            // App/Component Expired
    return("EEXPIRED");
  case EBADSTATE:           // Invalid state
    return("EBADSTATE");
  case EBADPARM:            // Invalid parameter
    return("EBADPARM");
  case ESCHEMENOTSUPPORTED: // Invalid URL scheme
    return("ESCHEMENOTSUPPORTED");
  case EBADITEM:            // Invalid item
    return("EBADITEM");
  case EINVALIDFORMAT:      // Invalid format
    return("EINVALIDFORMAT");
  case EINCOMPLETEITEM:     // Incomplete item
    return("EINCOMPLETEITEM");
  case ENOPERSISTMEMORY:    // Insufficient flash
    return("ENOPERSISTMEMORY");
  case EUNSUPPORTED:        // API is not supported
    return("EUNSUPPORTED");
  case EPRIVLEVEL:          // Security violation
    return("EPRIVLEVEL");
  case ERESOURCENOTFOUND:
    return("ERESOURCENOTFOUND");
  case EREENTERED:
    return("EREENTERED");
  case EBADTASK:
    return("EBADTASK");
  case EALLOCATED:          // App/Module left memory allocated when released.
    return("EALLOCATED");
  case EALREADY:            // Operation is already in progress
    return("EALREADY");
  case EADSAUTHBAD:         // ADS mutual authorization failed
    return("EADSAUTHBAD");
  case ENEEDSERVICEPROG:    // Need service programming
    return("ENEEDSERVICEPROG");
  case EMEMPTR:             // bad memory pointer
    return("EMEMPTR");
  case EHEAP:               // heap corruption
    return("EHEAP");
  case EIDLE:               // Context (system, interface, etc.) is idle
    return("EIDLE");
  case EITEMBUSY:           // Context (system, interface, etc.) is busy
    return("EITEMBUSY");
  case EBADSID:             // Invalid subscriber ID
    return("EBADSID");
  case ENOTYPE:             // No type detected/found
    return("ENOTYPE");
  case ENEEDMORE:           // Need more data/info
    return("ENEEDMORE");
  case EADSCAPS:            // ADS Capabilities do not match those required for phone
    return("EADSCAPS");
  case EBADSHUTDOWN:        // App failed to close properly
    return("EBADSHUTDOWN");
#ifdef EBUFFERTOOSMALL
  case EBUFFERTOOSMALL:     // Destination buffer given is too small
    return("EBUFFERTOOSMALL");
#endif
#ifdef ENOSUCH
  case ENOSUCH:             // No such name/port/socket/service exists or valid
    return("ENOSUCH");
#endif
#ifdef EACKPENDING
  case EACKPENDING:         // ACK pending on application
    return("EACKPENDING");
#endif
#ifdef ENOTOWNER
  case ENOTOWNER:           // Not an owner authorized to perform the operation
    return("ENOTOWNER");
#endif
#ifdef EINVALIDITEM
  case EINVALIDITEM:        // Current item is invalid
    return("EINVALIDITEM");
#endif
#ifdef ENOTALLOWED
  case ENOTALLOWED:         // Not allowed to perform the operation
    return("ENOTALLOWED");
#endif
#ifdef ENOPURCHASEAUTH
  case ENOPURCHASEAUTH:
    return("ENOPURCHASEAUTH");
#endif
  default:
    return("Undefined");
  }
}

// Get the clip time in Seconds, if the timestamp is greater than 900 miliseconds
// round up to the next second
uint32 MPClipTimeInSeconds(uint32 timeMili)
{
  return (timeMili + 99) / 1000; 

}



#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#else
int MP_CreateMedia(IShell * ps, AEEMediaData * pmd, IMedia ** ppm)
{
   AEECLSID    cls;
   int         nRet;
   IMedia *    pMedia;

   if (!ps || !pmd || !ppm)
      return EBADPARM;

   *ppm = NULL;

   if(pmd->clsData == MMD_HTTP_PULL_BUFFER_PLAYBACK)
   {
     cls = AEECLSID_MEDIAMPEG4;
   }
   else
   {
     // Find the class ID. This function uses ISHELL_DetectType() API to get the IMedia
     // MIME and subsequently class ID...
     nRet = MP_FindClass(ps, pmd, &cls);
     if (nRet != SUCCESS)
        return nRet;
   }

   // Create IMedia-based object
   if (ISHELL_CreateInstance(ps, cls, (void **)&pMedia) || !pMedia)
      return MM_ENOMEDIAMEMORY;

   *ppm = pMedia;

   return SUCCESS;
}

#ifdef AEEMediaUtil_PEEK_RETURN
#undef AEEMediaUtil_PEEK_RETURN
#endif
#define AEEMediaUtil_PEEK_RETURN(p, lCode ) { MM_RELEASEIF(p); return (lCode); }

#ifdef MM_RELEASEIF
#undef MM_RELEASEIF
#endif
#define MM_RELEASEIF(p)          if (p) { IBASE_Release((IBase*)(p)); (p) = NULL; }

int MP_FindClass(IShell * ps, AEEMediaData * pmd, AEECLSID * pCls)
{
   void *      pBuf = NULL;
   uint32      dwReqSize = 0;
   AEECLSID    cls;
   char        szMIME[32];
   const char *cpszMIME;
   int         nErr = SUCCESS;

   *pCls = 0;

   if (ENEEDMORE != ISHELL_DetectType(ps, NULL, &dwReqSize, NULL, NULL) || !dwReqSize)
         return EFAILED;

   // If it is file name, then first check the extension.
   // If there is no extension, then read the file data and
   // analyze it to determine if it is MIDI, QCP, PMD or MP3 format.
   if (pmd->clsData == MMD_FILE_NAME)
   {
      IFileMgr *  pfm;
      IFile *     pIFile;
      int         nRead;
      char *      psz = (char *)pmd->pData;
      char *      pszExt;
      
      // Check the extension...

      pszExt = STRRCHR(psz, '.');
      if (pszExt)
         pszExt++;

      if (pszExt && *pszExt)
      {
         // Check audio MIME base
         STRCPY(szMIME, AUDIO_MIME_BASE);
         STRCAT(szMIME, pszExt);
         cls = ISHELL_GetHandler(ps, AEECLSID_MEDIA, szMIME);
         if (cls)
         {
            *pCls = cls;
            return SUCCESS;
         }

         // Check video MIME base also
         STRCPY(szMIME, VIDEO_MIME_BASE);
         STRCAT(szMIME, pszExt);
         cls = ISHELL_GetHandler(ps, AEECLSID_MEDIA, szMIME);
         if (cls)
         {
            *pCls = cls;
            return SUCCESS;
         }
      }

      // Now, read the file contents and try to determine the format...
      
      if (ISHELL_CreateInstance(ps, AEECLSID_FILEMGR, (void **)&pfm))
         return ENOMEMORY;

      pIFile = IFILEMGR_OpenFile(pfm, (char *)pmd->pData, _OFM_READ);
      if (!pIFile)
      {
         MM_RELEASEIF(pfm);
         return ENOMEMORY;
      }

      pBuf = MALLOC(dwReqSize);
      if (!pBuf)
      {
         MM_RELEASEIF(pfm);
         return ENOMEMORY;
      }

      nRead = IFILE_Read(pIFile, pBuf, dwReqSize);

      MM_RELEASEIF(pIFile);
      MM_RELEASEIF(pfm);

      if (nRead < (int)dwReqSize)
      {
         FREE(pBuf);
         return MM_ENOTENOUGHTDATA;
      }

   } // if (pmd->clsData == MMD_FILE_NAME)
   else if (pmd->clsData == MMD_BUFFER)
   {
      if (pmd->dwSize >= (uint32)dwReqSize)
         pBuf = pmd->pData;
      else
         return MM_ENOTENOUGHTDATA;
   }
   else if (pmd->clsData >= MMD_ISOURCE) // Streaming is asynchronous -- return.
   {
      ISource *   pSource = (ISource *)pmd->pData;
      IPeek *     pPeek;
      char *      pPeekedBuf;
      int32       lAvailable;
      int32       lCode;

      // Check if ISource is IPeek
      if (SUCCESS != ISOURCE_QueryInterface(pSource, AEECLSID_PEEK, (void **)&pPeek) || !pPeek)
         return MM_EBADMEDIADATA;
      
      // Peek to see if some data is available
      pPeekedBuf = IPEEK_Peek(pPeek, 0, &lAvailable, &lCode);
      if (lCode == IPEEK_END || lCode == IPEEK_ERROR || lCode == IPEEK_FULL)
         AEEMediaUtil_PEEK_RETURN(pPeek, MM_EPEEK);

      // Peek till required data is fetched
      while (lAvailable < (long)dwReqSize)
      {
         pPeekedBuf = IPEEK_Peek(pPeek, dwReqSize - lAvailable, &lAvailable, &lCode);
         if (lCode == IPEEK_END || lCode == IPEEK_ERROR || lCode == IPEEK_FULL)
            AEEMediaUtil_PEEK_RETURN(pPeek, MM_EPEEK);

         // Indicate the caller to do IPEEK_Peekable() and call this function again...
         if (lCode == IPEEK_WAIT)
            AEEMediaUtil_PEEK_RETURN(pPeek, ENEEDMORE);
      }

      if (!pPeekedBuf)
         AEEMediaUtil_PEEK_RETURN(pPeek, MM_EPEEK);


      pBuf = MALLOC(dwReqSize);
      if (!pBuf)
         AEEMediaUtil_PEEK_RETURN(pPeek, ENOMEMORY);

      MEMCPY(pBuf, pPeekedBuf, dwReqSize);

      MM_RELEASEIF(pPeek);
   }

   nErr = ISHELL_DetectType(ps, pBuf, &dwReqSize, NULL, &cpszMIME);
   if (!nErr)
   {
      cls = ISHELL_GetHandler(ps, AEECLSID_MEDIA, cpszMIME);
      if (cls)
         *pCls = cls;
   }
   else
   {
     // check for AVI MIME type 
	   nErr = MP_DetectAVIType(pBuf, dwReqSize, &cpszMIME);
     if (!nErr)
     {
       cls = ISHELL_GetHandler(ps, AEECLSID_MEDIA, cpszMIME);
       if (cls)
	     {
         *pCls = cls;
		   }
     }
   }

   if (pBuf && pBuf != pmd->pData)
      FREE(pBuf);
   
   return nErr;
}

#ifndef MP4_MIME_BUFFER_LENGTH

#define MP4_MIME_BUFFER_LENGTH            8
#define MAXIMUM_MIME_BUFFER_LENGTH        16
#define AVI_MIME_BUFFER_LENGTH            3
#define RIFF_MIME_BUFFER_LENGTH           4

#define FMT_RM_MARK    ".RMF"
#define FMT_RIFF_MARK  "RIFF"
#define FMT_AVI_MARK   "AVI"

#define QTV_PLAYER_MIME_TYPE  "video/mp4"

#endif //MP4_MIME_BUFFER_LENGTH

static int MP_DetectAVIType(unsigned char *pBuf, uint32 dwBufSize, const char **type)
{
  int           nRet = EFAILED;
  const char    *MIMEType = NULL;
  unsigned char buffer[MAXIMUM_MIME_BUFFER_LENGTH];

  if (pBuf)
  {
    /* Incase of AVI */
    if(dwBufSize >= RIFF_MIME_BUFFER_LENGTH)
    {
      memcpy(buffer, pBuf, MAXIMUM_MIME_BUFFER_LENGTH);
      if (!memcmp(buffer, FMT_RIFF_MARK, RIFF_MIME_BUFFER_LENGTH) && !memcmp(buffer+8, FMT_AVI_MARK, AVI_MIME_BUFFER_LENGTH))
      {
        MIMEType = QTV_PLAYER_MIME_TYPE;
        nRet = SUCCESS;
      }
    }
  }

  *type = (char*)MIMEType;
  return nRet;
}

#endif //FEATURE_APP_MEDIA_VIDEO

/*===========================================================================
   This function frames the rectangle with CLR_USER_FRAME and
   CLR_SYS_DK_SHADOW
===========================================================================*/
void MP_FrameRect(IDisplay *pd, AEERect *pRect)
{
  RGBVAL    clr = IDISPLAY_SetColor(pd, CLR_USER_FRAME, CLR_SYS_DK_SHADOW);

  IDISPLAY_FrameRect(pd, pRect);
  (void)IDISPLAY_SetColor(pd, CLR_USER_FRAME, clr);
}


/*===========================================================================
   This function fits the text within the IStatic rectangle so that the
   scroll bar is not drawn.  Assumes Normal font.
===========================================================================*/
void MP_FitStaticTextNormal(IDisplay *pd, IStatic *ps, AECHAR *pszText)
{
  MP_FitStaticText(pd, ps, AEE_FONT_NORMAL, pszText);
}

/*===========================================================================
   This function fits the text within the IStatic rectangle so that the
   scroll bar is not drawn.
===========================================================================*/
void MP_FitStaticText(IDisplay *pd, IStatic *ps, AEEFont font, AECHAR *pszText)
{
  int       nFits;
  AEERect   rect;
  int       nLen;
  AECHAR    chSave = (AECHAR) 0;

  if(!pszText)
  {
    DBGPRINTF_ERROR("MP:  AECHAR pointer NULL in MP_FitStaticText!");
    return;
  }
  if (!pd)
  {
    DBGPRINTF_ERROR("MP:  IDisplay pointer NULL in MP_FitStaticText!");
    return;
  }
  if (!ps)
  {
    DBGPRINTF_ERROR("MP:  IStatic pointer NULL in MP_FitStaticText!");
    return;
  }

  nLen = WSTRLEN(pszText);
  ISTATIC_GetRect(ps, &rect);
  (void)IDISPLAY_MeasureTextEx(pd, font, pszText, -1, rect.dx, &nFits);
  if (nFits < nLen)
  {
    chSave = pszText[nFits];
    pszText[nFits] = (AECHAR) 0;
  }
  (void)ISTATIC_SetText(ps, NULL, pszText, AEE_FONT_NORMAL, font);
  if (nFits < nLen)
    pszText[nFits] = chSave;
}

/*===========================================================================
   This function returns the file name portion of the path.
===========================================================================*/
char *MP_GetFileName(const char *psz)
{
  char     *pszName = STRRCHR(psz, (int)DIRECTORY_CHAR);

  if (pszName)
    pszName++;
  else
    pszName = (char *)psz;

  return pszName;
}

/*===========================================================================
   This function returns the file name portion of the path.
===========================================================================*/
char *MP_GetFileNameNoExtension(const char *psz, char *pszOut, int32 len)
{
  char     *pszName = STRRCHR(psz, (int)DIRECTORY_CHAR);
  char * ext;
  int extLen = 0;

  if (pszName)
    pszName++;
  else
    pszName = (char *)psz;

  (void)STRLCPY(pszOut, pszName, len);

  ext = MP_GetFileNameExtension(pszOut);

  if (NULL != ext)
  {
    extLen = STRLEN(ext);

    if (extLen > 0)
    {
      (void)MEMSET(&pszOut[STRLEN(pszOut) - (extLen + 1)], '\0', 1);
    }
  }

  return pszOut;
}

/*===========================================================================
         This function returns the extension of the file name.
===========================================================================*/
char *MP_GetFileNameExtension(const char *pszFileName)
{
  char *pszExtName;
 
  if(pszFileName == NULL)
    return NULL;

  pszExtName = STRRCHR(pszFileName, (int)'.');

  if (pszExtName)
    pszExtName++;
  else if (pszExtName == '\0')
    return NULL;

  return pszExtName;
}

/*===========================================================================
   This function adds the extension string to the given string.
===========================================================================*/
boolean MP_AddExtension(char **ppszExtList, char *psz)
{
  char     *pExt;
  char     *strExtSeparator = ", ";
  int      size;

  if (!ppszExtList || !psz)
    return FALSE;

  if (*ppszExtList)
  {
    size = STRLEN(*ppszExtList) + STRLEN(psz) + STRLEN(strExtSeparator) + 1;
    pExt = REALLOC(*ppszExtList, size);
  }
  else
  {
    size = STRLEN(psz) + STRLEN(strExtSeparator) + 1;
    pExt = MALLOC(size);
  }

  if (!pExt)
    return FALSE;

  (void)STRLCAT(pExt, psz, size);
  (void)STRLCAT(pExt, strExtSeparator, size);
  *ppszExtList = pExt;
  return TRUE;
}

/*---------------------------------------------------------------------

Method: MP_isValidRect

Description: This method checks specified rectangle to make sure that its
    origin is positive and that it meets the minimum specified size.

Parameters:
    rect       - Rectangle to check
    nMinHeight - Minimum height required
    nMinWidth  - Minimum width required

Return:
    true = Rectangle meets minimum requirements

Side Effects:
    None

-----------------------------------------------------------------------*/
boolean
MP_isValidRect(AEERect rect, int16 nMinHeight, int16 nMinWidth)
{
  if ((rect.x >= 0) && (rect.y >= 0) && (rect.dx >= nMinWidth) && (rect.dy >= nMinHeight))
    return TRUE;
  else
    return FALSE;
}

/*---------------------------------------------------------------------

Method: MP_IsMediaVideo

Description: This method checks to see if the given file name can be handled properly
             by QTV.

Parameters:
    ps         - Pointer to IShell
    pszFileName - Pointer to file name string

Return:
    true, if the cls ID is AEECLSID_MEDIAMPEG4

Side Effects:
    None

-----------------------------------------------------------------------*/
boolean 
MP_IsMediaVideo(IShell * ps, char* pszFileName)
{
  AEECLSID    cls;
  AEEMediaData md;
  md.clsData = MMD_FILE_NAME;
  md.pData = (void *)pszFileName;
  md.dwSize = 0;


  if (SUCCESS != MP_FindClass(ps, &md, &cls))
  {
    return FALSE;
  }

  if(cls == AEECLSID_MEDIAMPEG4)
  {
   return TRUE;
  }
  else
  {
   return FALSE;
  }
}


/*---------------------------------------------------------------------

Method: MP_GetUIMMediaPlayerRoot

Description: This function returns the base folder on the UIM storage device.

Parameters:
    pszFolderName - a pre-allocated buffer for the folder name.
    folderNameSize - the size of the buffer.  Used by the caller to reset the buffer content.

Return:
    true = if UIM device exists

PreCondition:
    Caller is expected to memset the folder name parameter to zero.

Side Effects:
    None

-----------------------------------------------------------------------*/
#if MIN_BREW_VERSION(4,0)

boolean MP_GetUIMMediaPlayerRoot(IFileMgr *pIFileMgr, char *pszFolderName, int folderNameSize)
{
  int length;
  int ret = FALSE;
  char szMedia [] = "/media";
 
  if(pszFolderName == NULL || pIFileMgr== NULL)
    return ret;

  if (SUCCESS == IFILEMGR_ResolvePath(pIFileMgr, AEEFS_USB_HOST_MASS_STORAGE1, pszFolderName, &length))
  {
    // Hard-coded for testing purpose
    (void)MEMSET(pszFolderName, 0, folderNameSize);
    (void)STRLCPY(pszFolderName, "fs:/usb1", folderNameSize);
    // Check to see if, in a rare case, the length is too large
    if (length <= (MP_MAX_FILE_NAME - sizeof (szMedia)) )
    {
      // We need to truncate the ending forward slash '/' if it present
      // For instance, "fs:/mod/26108/" is normalized into "fs:/mod/26108"
      if (STRRCHR (pszFolderName, (int)DIRECTORY_CHAR) == (pszFolderName + STRLEN(pszFolderName) - 1))
      {
        pszFolderName [(STRLEN(pszFolderName) - 1)] = '\0';
      }

      // We test the presence of the root folder, such as "fs:/mod/26108"
      if (IFILEMGR_Test(pIFileMgr, pszFolderName) == SUCCESS)
      {
        // Make the folder path of "<UIM_StorageRoot>/media".  
        // For instance, in BREW 4.x, it's likely "fs:/mod/26108" + "/media"
        (void)STRLCAT(pszFolderName, szMedia, folderNameSize);
        if (MP_MakeDir(pIFileMgr, pszFolderName))
        {
          DBGPRINTF_HIGH("MP: Folder %s either exists already or has been created successfully.", pszFolderName);
          ret = TRUE;
        }
      }
      else
      {
        DBGPRINTF_HIGH("MP: %s not added as it does not exist", pszFolderName);
      }
    }
    else
    {
      DBGPRINTF_MED("MP: Well-known path to UIM Storage is too big: %s", pszFolderName);
    }
  }
  else
  {
    DBGPRINTF_MED("MP: No root for UIM Storage due to non-existent base path (e.g., fs:/mod/26108)");
  }

  return ret;
}
#endif // #if MIN_BREW_VERSION(4,0)
