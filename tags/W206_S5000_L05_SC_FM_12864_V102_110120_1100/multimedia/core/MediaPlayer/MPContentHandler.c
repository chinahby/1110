/*=============================================================================
  FILE: MPContentHandler.c

  SERVICES: MPContentHandler object and function implementations

  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPContentHandler.c#19 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "AEEStdLib.h"
#include "MPContentHandler.h"

#ifdef MP_FEATURE_ICONTENTHANDLER

#define MAX_MIME_STRING_LENGTH 64

#ifdef FEATURE_SME_DRMMS
#define DRM_PROTECTED_FILE_ASF          "asf"
#define DRM_PROTECTED_FILE_WMA          "wma"
#define DRM_PROTECTED_FILE_WMV          "wmv"
#endif //#ifdef FEATURE_SME_DRMMS

typedef struct stIContentHandlerSelectFile
{
  CMediaPlayer*       m_pme;              // point to MediaPlayer
  IContentHandler*    m_pIContentHandler; // IContent ptr

  AEECallback         m_cbObject;
  AEEContentType      m_cbObjectType;
  void*               m_pObject;          // IMedia object at callback from ICONTENT_GetObject
  int                 m_nRet;             // ret information

  char*               m_pszFile;
  boolean             m_bPlay;         // TRUE for playing selection
}stIContentHandlerSelectFile;


static stIContentHandlerSelectFile* MPContentHandlerSelectFileMake(stContent* pct);
static void MPContentHandlerSelectFileFree(stIContentHandlerSelectFile* pSF);
static void MPContentGetObjectCB ( void * pData );
static void MPContentSelectFileUrlCB(void *p);
extern void PS_Free_web(CHttpBlock *psBlk);

#ifdef FEATURE_SME_DRMMS
static boolean MPContentIsProtectedFile(const char* filepath);
#endif //#ifdef FEATURE_SME_DRMMS

// this function creates and initialize stContent object
stContent* MPContentMake(CMediaPlayer* pme)
{
  stContent* pct = (stContent*)MALLOC(sizeof(stContent));
  if(pct)
  {
    MEMSET(pct, 0, sizeof(stContent));
    pct->m_pOwner       = pme;
  }
  else
  {
    DBGPRINTF_ERROR("MP: failed to create stContent.");
  }
  return pct;
}

// Free allocated memories and object handler
// this function must be called in pair with MPContentHandlerMake
void MPContentFree(stContent* pct)
{
  if(!pct)
  {
    return;
  }
  
  if(pct->m_pSF)
    MPContentHandlerSelectFileFree(pct->m_pSF);
  FREEIF(pct);
}

// this function creates and initialize stIContentHandlerSelectFile object
// the stIContentHandlerSelectFile is used to pass information when
// rendering a media object
static stIContentHandlerSelectFile* MPContentHandlerSelectFileMake(stContent* pst)
{
  stIContentHandlerSelectFile* pSF;
  pSF = (stIContentHandlerSelectFile*)MALLOC(sizeof(stIContentHandlerSelectFile));
  if(pSF)
  {
    MEMSET(pSF, 0, sizeof(stIContentHandlerSelectFile));
    if(pst)
    {
      // save owner ptr
      pSF->m_pme = pst->m_pOwner;
      pst->m_pSF = pSF;
    }
  }
  else
  {
    DBGPRINTF_ERROR("MP: failed to alloc memory.");
  }
  return pSF;
}

static void MPContentHandlerSelectFileFree(stIContentHandlerSelectFile* pSF)
{
  if(pSF)
  {
    if((pSF->m_pme) && (pSF->m_pme->m_pContent))
      pSF->m_pme->m_pContent->m_pSF = NULL;
    DBGPRINTF_MED("MP: MPContentHandlerSelectFileFree psf %p cb %p handler %p.", pSF, pSF->m_cbObject.pfnCancel, pSF->m_pIContentHandler) ;
    CALLBACK_Cancel(&pSF->m_cbObject);
    RELEASEIF(pSF->m_pIContentHandler);
    FREEIF(pSF->m_pszFile);
    FREEIF(pSF);
  }
}

// this function is called when user select and click a file from the list win
int MPContentSelectFile(stContent* pct, const char* szURL, uint16 nFileIdx, boolean bPlay)
{
  int nErr = EFAILED;
  IContentHandler *pIContentHandler = NULL;
  stIContentHandlerSelectFile *psf = NULL;
  char *pExtName = NULL;
  char strMIME[MAX_MIME_STRING_LENGTH];
  int nLength;

  DBGPRINTF_MED("MP: MPContentSelectFile %s.", szURL) ;

  // Make sure ContentISource is cleaned up
  if(pct->m_pOwner->m_pContentISource)
  {
    RELEASEIF(pct->m_pOwner->m_pContentISource);
    pct->m_pOwner->m_pContentISource = NULL;
  }

  // If we are dealing with a URL file, call the MPContentSelectFileISource function instead
  pExtName = MP_GetFileNameExtension(szURL);
  if(!STRICMP(pExtName, "url"))
    return(MPContentSelectFileUrl(pct, szURL, nFileIdx, bPlay));

  // create IContentHandler object
  if(SUCCESS != ISHELL_CreateInstance(pct->m_pOwner->a.m_pIShell, AEECLSID_CONTENTHANDLER, (void**)(&pIContentHandler)))
  {
    DBGPRINTF_ERROR("MP: failed to create IContentHandler.") ;
    return nErr;
  }

#ifdef FEATURE_SME_DRMMS
  {
    if (TRUE == MPContentIsProtectedFile(szURL))
    {
      if( SUCCESS != ICONTENTHANDLER_InitEx(pIContentHandler, NULL, FALSE, DRM_SYSTEM_JANUS))
      {
        DBGPRINTF_ERROR("MP: failed to initialize IContentHandler.") ;
        goto cleanup;
      }
    }
    else
    {
      if( SUCCESS != ICONTENTHANDLER_InitEx(pIContentHandler, NULL, FALSE, DRM_SYSTEM_OMA_V1_V2))
      {
        DBGPRINTF_ERROR("MP: failed to initialize IContentHandler.") ;
        goto cleanup;
      }
    }
  }
#else
  if( SUCCESS != ICONTENTHANDLER_Init(pIContentHandler, NULL, FALSE))
  {
    DBGPRINTF_ERROR("MP: failed to initialize IContentHandler.") ;
    goto cleanup;
  }
#endif //#ifdef FEATURE_SME_DRMMS

  // create IContentHandlerSelectFile object
  if(NULL == (psf = MPContentHandlerSelectFileMake(pct)))
  {
    DBGPRINTF_ERROR("MP: call to MPContentHandlerSelectFileMake failed.") ;
    goto cleanup;
  }
  psf->m_pIContentHandler = pIContentHandler;
  psf->m_bPlay = bPlay;

  nLength = STRLEN(szURL) + 1;
  psf->m_pszFile = (char*)MALLOC(nLength);
  if(psf->m_pszFile)
  {
    (void)MEMSET(psf->m_pszFile, 0, nLength);
    // save URL
    STRLCPY(psf->m_pszFile, szURL, nLength);
  }
  else
  {
    DBGPRINTF_ERROR("MP: failed to alloc memory.");
    goto cleanup;
  }


  MEMSET(strMIME, 0, sizeof(strMIME));
  // Get the filename extension.
  pExtName = MP_GetFileNameExtension(psf->m_pszFile);
  if (!STRCMP(pExtName, "dcf"))
  {
    // .dcf (OMA DRM v1)
    (void)STRLCPY(strMIME, "application/vnd.oma.drm.content", sizeof(strMIME));
  }
  else if(!STRCMP(pExtName, "odf"))
  {
    //.odf (OMA DRM v2)
    (void)STRLCPY(strMIME, "application/vnd.oma.drm.dcf", sizeof(strMIME));
  }
  else if(!STRCMP(pExtName, "o4a")) 
  {
    //.o4a (OMA DRM v2)
    (void)STRLCPY(strMIME, "application/vnd.oma.drm.dcf", sizeof(strMIME));
  }
  else if(!STRCMP(pExtName, "o4v"))
  {
    //.o4v (OMA DRM v2)
    (void)STRLCPY(strMIME, "application/vnd.oma.drm.dcf", sizeof(strMIME));
  }
  else
  {
    (void)CMediaPlayer_GetMIMETypeString(pct->m_pOwner, szURL, sizeof(strMIME), strMIME);
  }

  // set attributes
  if(SUCCESS != ICONTENTHANDLER_SetAttr(psf->m_pIContentHandler, CONTENTATTRIB_CONTENT_URL, (void*)szURL))
  {
    DBGPRINTF_ERROR("MP: failed on setting CONTENTATTRIB_CONTENT_URL attribute.") ;
  }
  if(SUCCESS != ICONTENTHANDLER_SetAttr(psf->m_pIContentHandler, CONTENTATTRIB_MIME_TYPE, strMIME))
  {
    DBGPRINTF_ERROR("MP: failed on setting CONTENTATTR_MIME_TYPE attribute.") ;
  }


  // initialize callback
  CALLBACK_Init(&psf->m_cbObject, MPContentGetObjectCB, psf);

  // retrive media object
  nErr = ICONTENTHANDLER_GetObject( psf->m_pIContentHandler,
                                    FALSE,
                                    &psf->m_cbObject,
                                    &psf->m_cbObjectType,
                                    &psf->m_pObject,
                                    &psf->m_nRet );

cleanup:
  if(EFAILED == nErr)
  {
    if(psf)
      MPContentHandlerSelectFileFree(psf);
    else
      RELEASEIF(pIContentHandler);
  }

  return nErr;
}

// this function is called when user select and click a file from the list win
// it is used to handle files through the IContentHandler with a IWEB connection
int MPContentSelectFileUrl(stContent* pct, const char* szURL, uint16 nFileIdx, boolean bPlay)
{
  int            nErr = EFAILED;
  IFileMgr      *pIFileMgr = NULL;
  IFile         *pIFile = NULL;
  AEEFileInfoEx  fi;
  char          *tmp_szURL = NULL;
  int            rsize = 0;
  int            nLength;

  // Note: szURL is the file selected here, not the link internal to the file
  // Note allocate here before we cleanup possible existing CHttpBlock just in case
  nLength = STRLEN(szURL) + 1;
  tmp_szURL = (char*)MALLOC(nLength + 1);
  if(tmp_szURL == NULL)
  {
    DBGPRINTF_ERROR("MP: Content select URL failed to MALLOC szURL.") ;
    goto cleanup;
  }
  STRLCPY(tmp_szURL, szURL, nLength);
  pct->m_bPlay = bPlay;

  // Allocate a CHttpBlock structure to manage the IWeb interface
  // Note, we won't need all the structure, but it is easier to reuse an
  // existing structure.  Initialize ourselves as the http has code specific
  // to the pseudo streaming environment that we won't be using.

  // Protect against a loop, check if exists and cleanup if it does.
  if(pct->m_psHttpBlock != NULL)
  {
    if(pct->m_psHttpBlock->szURL);
      FREE(pct->m_psHttpBlock->szURL);
    pct->m_psHttpBlock->szURL = NULL;
    PS_Free_web(pct->m_psHttpBlock);
    FREE(pct->m_psHttpBlock);
  }

  pct->m_psHttpBlock = (CHttpBlock *)MALLOC(sizeof(CHttpBlock));
  if(pct->m_psHttpBlock == NULL)
  {
    DBGPRINTF_ERROR("MP: Content select URL failed to MALLOC CHttpBlock.") ;
    goto cleanup;
  }
  (void)MEMSET(pct->m_psHttpBlock, 0, sizeof(CHttpBlock));

  // Note: szURL is the file selected here, not the link internal to the file
  pct->m_psHttpBlock->szURL = tmp_szURL;
  tmp_szURL = NULL;

  // We need to get the URL from the file
  if(SUCCESS != ISHELL_CreateInstance(pct->m_pOwner->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr))
  {
    DBGPRINTF_ERROR("MP: Content select URL failed to create IFileMgr.") ;
    goto cleanup;
  }

  (void)MEMSET(&fi,0,sizeof(fi));
  fi.nStructSize = sizeof(fi);
  if((pIFile = IFILEMGR_OpenFile(pIFileMgr, pct->m_psHttpBlock->szURL, _OFM_READ)) == NULL)
  {
    DBGPRINTF_ERROR("MP: Content select URL failed to create open pIFile.") ;
    goto cleanup;
  }

  if(SUCCESS != IFILEMGR_GetInfoEx(pIFileMgr, pct->m_psHttpBlock->szURL, &fi))
  {
    DBGPRINTF_ERROR("MP: Content select URL failed to GetInfoEx.") ;
    goto cleanup;
  } 

  if(fi.dwSize > PS_MAX_URL_LEN)
  {
    DBGPRINTF_ERROR("MP: Content select URL size of URL to large.") ;
    goto cleanup;
  }

  rsize = IFILE_Read(pIFile, pct->m_psHttpBlock->m_pUrl, fi.dwSize);
  pct->m_psHttpBlock->m_pUrl[rsize] = '\0';

  (void)IFILE_Release(pIFile);
  pIFile = NULL;
  MP_RELEASEIF(pIFileMgr);
  pIFileMgr = NULL;

  // If the URL does not start with http:// or file:/ then assume it is just
  // a file and pass back to the MPContentSelectFile with the filename. We will
  // need to free http block after passing in as it needs value.
  if((STRNICMP(pct->m_psHttpBlock->m_pUrl, "http://", 7) != 0) &&
     (STRNICMP(pct->m_psHttpBlock->m_pUrl, "file:/", 6) != 0))
  {
    nErr = MPContentSelectFile(pct, pct->m_psHttpBlock->m_pUrl, nFileIdx, bPlay);
    goto cleanup;
  }

  // Create an IWEB interface and start a connection to the URL contained within szURL file
  // The Content handling will be handled in the MPContentSelectFileUrlCB function
  if(SUCCESS != ISHELL_CreateInstance(pct->m_pOwner->a.m_pIShell, AEECLSID_WEB, 
                                      (void **)(&(pct->m_psHttpBlock->m_pIWeb))))
  {
    DBGPRINTF_ERROR("MP: Content select URL failed to create IWeb.") ;
    goto cleanup;
  }

  // Initialize the callback function to pass response to content handler
  CALLBACK_Init(&pct->m_psHttpBlock->m_webCallback, MPContentSelectFileUrlCB, pct);

  DBGPRINTF_MED("MP: MPContentSelectFileUrl %s", pct->m_psHttpBlock->m_pUrl);
  // Request the URL from IWEB
  IWEB_GetResponse(pct->m_psHttpBlock->m_pIWeb,
                       (pct->m_psHttpBlock->m_pIWeb,
                        &pct->m_psHttpBlock->m_pIWebResp,
                        &pct->m_psHttpBlock->m_webCallback,
                        pct->m_psHttpBlock->m_pUrl,
                        WEBOPT_END));

  nErr = SUCCESS;

  // Further processing will be handled in the MPContentSelectFileUrlCB function
cleanup:
    if(tmp_szURL != NULL) 
      FREE(tmp_szURL);
    if(pIFile != NULL) 
      (void)IFILE_Release(pIFile);
    if(pIFileMgr != NULL) 
      MP_RELEASEIF(pIFileMgr);
    if(EFAILED == nErr)
    {
      if(pct->m_psHttpBlock != NULL)
      {
        if(pct->m_psHttpBlock->szURL != NULL) 
        {
          FREE(pct->m_psHttpBlock->szURL);
          pct->m_psHttpBlock->szURL = NULL;
        }
        PS_Free_web(pct->m_psHttpBlock);
        FREE(pct->m_psHttpBlock);
      }
      pct->m_psHttpBlock = NULL;
    }
  return nErr;
}

// Callback function to continue processing the IWEB connection setup in MPContentSelectFileUrl
static void MPContentSelectFileUrlCB(void *p)
{
  stContent *pct = (stContent *)p;
  int nErr = EFAILED;
  IContentHandler *pIContentHandler = NULL;
  stIContentHandlerSelectFile *psf = NULL;
  WebRespInfo *pWebResp = NULL;
  char strMIME[MAX_MIME_STRING_LENGTH];
  char *pExtName = NULL;

  if(p == NULL)
    return;

  // For any error, cleanup and release CHttpBlock structure, 
  // though we should be able to use PS_Free_web.

  // Get the web response info structure
  pWebResp = IWEBRESP_GetInfo(pct->m_psHttpBlock->m_pIWebResp);
  if(pWebResp == NULL)
  {
    DBGPRINTF_ERROR("MP: failed to get web response info structure.") ;
    goto cleanup;
  }

  if(WEB_ERROR_SUCCEEDED(pWebResp->nCode) == FALSE)
  {
    DBGPRINTF_ERROR("MP: The IWEB GetResponse failed.") ;
    goto cleanup;
  }

  // create IContentHandler object
  if(SUCCESS != ISHELL_CreateInstance(pct->m_pOwner->a.m_pIShell, AEECLSID_CONTENTHANDLER, 
                                      (void**)(&pIContentHandler)))
  {
    DBGPRINTF_ERROR("MP: failed to create IContentHandler.") ;
    goto cleanup;
  }

#ifdef FEATURE_SME_DRMMS
  {
    if (TRUE == MPContentIsProtectedFile(pct->m_psHttpBlock->m_pUrl))
    {
      if( SUCCESS != ICONTENTHANDLER_InitEx(pIContentHandler, NULL, FALSE, DRM_SYSTEM_JANUS))
      {
        DBGPRINTF_ERROR("MP: failed to initialize IContentHandler.") ;
        goto cleanup;
      }
    }
    else
    {
      if( SUCCESS != ICONTENTHANDLER_InitEx(pIContentHandler, NULL, FALSE, DRM_SYSTEM_OMA_V1_V2))
      {
        DBGPRINTF_ERROR("MP: failed to initialize IContentHandler.") ;
        goto cleanup;
      }
    }
  }
#else
  if( SUCCESS != ICONTENTHANDLER_Init(pIContentHandler, NULL, FALSE))
  {
    DBGPRINTF_ERROR("MP: failed to initialize IContentHandler.") ;
    goto cleanup;
  }
#endif //#ifdef FEATURE_SME_DRMMS

  // create IContentHandlerSelectFile object
  if(NULL == (psf = MPContentHandlerSelectFileMake(pct)))
  {
    DBGPRINTF_ERROR("MP: call to MPContentHandlerSelectFileMake failed.") ;
    goto cleanup;
  }
  psf->m_pIContentHandler = pIContentHandler;
  psf->m_bPlay = pct->m_bPlay;

  // Note: szURL is the file selected here, not the link internal to the file
  // We allocated this memory in the MPContentSelectFileUrl function, just move
  // it from the CHttpBlock struct to this one.
  psf->m_pszFile = pct->m_psHttpBlock->szURL;
  pct->m_psHttpBlock->szURL = NULL; 

  // We need to open the file and create an ISOURCE interface to it
  // Save a copy of it into psf struct, we need to clean it up later

  // Set the ISource pointer in the main data struct
  psf->m_pme->m_pContentISource = pWebResp->pisMessage;
  ISOURCE_AddRef(psf->m_pme->m_pContentISource);

  // set attributes 
  if(SUCCESS != ICONTENTHANDLER_SetAttr(psf->m_pIContentHandler, CONTENTATTRIB_CONTENT_SOURCE, 
                                        (void*)(pWebResp->pisMessage)))
  {
    DBGPRINTF_ERROR("MP: failed on setting CONTENTATTRIB_CONTENT_URL attribute.") ;
    goto cleanup;
  }

  // set attributes 
#ifdef MP_FEATURE_ICONTENT_PROG_DL
  {
    boolean prog_dl = TRUE;
    if(SUCCESS != ICONTENTHANDLER_SetAttr(psf->m_pIContentHandler, CONTENTATTRIB_PROG_DL, 
                                          (void*)(prog_dl)))
    {
      DBGPRINTF_ERROR("MP: failed on setting CONTENTATTRIB_PROG_DL attribute.") ;
      goto cleanup;
    }
  }
#endif
 
  MEMSET(strMIME, 0, sizeof(strMIME));
  // Get the filename extension.
  pExtName = MP_GetFileNameExtension(pct->m_psHttpBlock->m_pUrl);
  if (!STRNCMP(pExtName, "dcf", 3))
  {
    // .dcf (OMA DRM v1)
    (void)STRLCPY(strMIME, "application/vnd.oma.drm.content", sizeof(strMIME));
  }
  else if(!STRNCMP(pExtName, "odf", 3))
  {
    //.odf (OMA DRM v2)
    (void)STRLCPY(strMIME, "application/vnd.oma.drm.dcf", sizeof(strMIME));
  }
  else if(!STRNCMP(pExtName, "o4a", 3)) 
  {
    //.o4a (OMA DRM v2)
    (void)STRLCPY(strMIME, "application/vnd.oma.drm.dcf", sizeof(strMIME));
  }
  else if(!STRNCMP(pExtName, "o4v", 3))
  {
    //.o4v (OMA DRM v2)
    (void)STRLCPY(strMIME, "application/vnd.oma.drm.dcf", sizeof(strMIME));
  }
  else
  {
    (void)CMediaPlayer_GetMIMETypeString(pct->m_pOwner, pct->m_psHttpBlock->m_pUrl, sizeof(strMIME), strMIME);
  }

  DBGPRINTF_MED("MP: MPContentSelectFileUrlCB set MIME as %s", strMIME);
  if(SUCCESS != ICONTENTHANDLER_SetAttr(psf->m_pIContentHandler, CONTENTATTRIB_MIME_TYPE, 
                                        (void*)(strMIME)))
  {
    DBGPRINTF_ERROR("MP: failed on setting CONTENTATTR_MIME_TYPE attribute.") ;
    goto cleanup;
  }

  // initialize callback
  CALLBACK_Init(&psf->m_cbObject, MPContentGetObjectCB, psf);

  // retrive media object
  nErr = ICONTENTHANDLER_GetObject( psf->m_pIContentHandler,
                                    FALSE,
                                    &psf->m_cbObject,
                                    &psf->m_cbObjectType,
                                    &psf->m_pObject,
                                    &psf->m_nRet );

cleanup:
  // Release the web interfaces, the ISOURCE to the data has a referece 
  // added to it to ensure it stays around after the response is released
  PS_Free_web(pct->m_psHttpBlock);
  FREE(pct->m_psHttpBlock);
  pct->m_psHttpBlock = NULL;

  if(EFAILED == nErr)
  {
    RELEASEIF(pct->m_pOwner->m_pContentISource);
    if(psf)
      MPContentHandlerSelectFileFree(psf);
    else
      RELEASEIF(pIContentHandler);
  }

  return;
}

// Callback function for ICONTENT_GetObject.
static void MPContentGetObjectCB ( void * pData )
{
  boolean bRet = FALSE;
  stIContentHandlerSelectFile *psf;
  CMediaPlayer *pme = NULL;

  psf = (stIContentHandlerSelectFile *) pData ;

  if(psf == NULL)
  {
    DBGPRINTF_ERROR ("MP: GetObject() callback returns NULL ptr.");
    return;
  }

  DBGPRINTF_MED("MP: MPContentGetObjectCB nRet %d type %d.", psf->m_nRet, psf->m_cbObjectType) ;

  if(SUCCESS == psf->m_nRet)
  {
    switch(psf->m_cbObjectType)
    {
      case AEECONTENT_TYPE_MEDIA:
      {
        if(TRUE == (bRet = MPCreatePlayerWin(psf->m_pme, psf->m_pObject, MPPW_PLAY, psf->m_pszFile)))
        {
          if(psf->m_bPlay)
          {
            (void)ISHELL_PostEvent(psf->m_pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
          }
        }
        else
        {
		  if(psf->m_pszFile)
            DBGPRINTF_ERROR ("MP: MPCreatePlayerWin() failed with file = %s.", psf->m_pszFile);
          else
            DBGPRINTF_ERROR ("MP: MPCreatePlayerWin() failed with file = NULL.");
        }
        break;
      }
      case AEECONTENT_TYPE_IMAGE:
        if(TRUE != (bRet = MPCreatePlayerWin(psf->m_pme, psf->m_pObject, MPPW_IMAGE, NULL)))
        {
          DBGPRINTF_ERROR ("MP: MPCreatePlayerWin() failed.");
        }
        break;
      default:
        DBGPRINTF_ERROR ("MP: unsupported type.") ;
        break;
    }
  }
  else
  {
    DBGPRINTF_ERROR ("MP: Content callback return error, nRet = %d", psf->m_nRet) ;
  }

  pme = psf->m_pme;
  if(bRet == FALSE)
  {
    MP_RELEASEIF(pme->m_pContentISource);
    MP_RELEASEIF(psf->m_pObject);
    MPContentHandlerSelectFileFree(psf);
    // If we were in continuous file mode and we failed (Image, etc) then
    // try and play next file.
    if(pme->m_nContMode == CONT_MODE_ALL_FILES)
      MPContentPlayNext(pme);
  }
  else
  {
    MPContentHandlerSelectFileFree(psf);
  }
}

#ifdef FEATURE_SME_DRMMS
static boolean MPContentIsProtectedFile(const char* filepath)
{
  char     *pszName;
  char     szExt[32]; // set max. extension to 32

  pszName = MP_GetFileNameExtension(filepath);
  if(NULL != pszName)
  {
    (void)MEMSET(szExt, 0, sizeof(szExt));
    (void)STRLCPY(szExt, pszName, sizeof(szExt));
    (void)STRLOWER(szExt);
  }

  // compare extension with DRM supported file types
  if(STRCMP(szExt, DRM_PROTECTED_FILE_ASF) == 0)
  {
    return TRUE;
  }
  else if(STRCMP(szExt, DRM_PROTECTED_FILE_WMA) == 0)
  {
    return TRUE;
  }
  else if(STRCMP(szExt, DRM_PROTECTED_FILE_WMV) == 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

  return FALSE;
}
#endif//#ifdef FEATURE_SME_DRMMS

#endif //MP_FEATURE_ICONTENTHANDLER
