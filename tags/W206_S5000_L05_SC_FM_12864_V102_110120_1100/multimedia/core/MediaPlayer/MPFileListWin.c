/*=============================================================================
  FILE: MPFileListWin.cpp

  SERVICES: file list win object Handlers
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPFileListWin.c#34 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "MPFileListWin.h"
#include "MediaPlayer.h"
#include "MPIO.h"

#ifdef MP_FEATURE_ICONTENT
#include "MPContent.h"
#endif //MP_FEATURE_ICONTENT

#ifdef MP_FEATURE_ICONTENTHANDLER
#include "MPContentHandler.h"
#endif //MP_FEATURE_ICONTENTHANDLER


/*===========================================================================

                     CFileListWin Functions

===========================================================================*/

static boolean CMediaPlayer_HandleExplorerEvt(CMediaPlayer *pme, AEEEvent eCode, uint16 wItemID);
static void MPFileListPlayFile(CMediaPlayer* pme, const char* szPath);

/*===========================================================================
   This function constucts the file list window.
===========================================================================*/
IWindow * CFileListWin_New(CMediaPlayer *pOwner)
{
  CFileListWin *pme;

  VTBL(IWindow)vtbl;
  uint32    dwProps = 0;

  MP_IWINDOW_SETVTBL(&vtbl, CFileListWin_Enable, CFileListWin_Redraw, CFileListWin_HandleEvent, CFileListWin_Delete);

  pme = (CFileListWin *)CWindow_New(sizeof(CFileListWin), pOwner, &vtbl);
  if (!pme)
    return NULL;

  {
    int       cx = pme->m_pOwner->m_cxWidth;
    int       cy = pme->m_pOwner->m_cyHeight;
    AEERect   rect;
    int       y;

    /*
     * set to browsing file state
     */
    pme->m_pOwner->m_State = MP_STATE_BROWSE_FILE;
    EMIT_MEDIAPLAYER_STATE(pme->m_pOwner->m_State);
    if (ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_MENUCTL, (void **)&pme->m_pFileMenu))
      MP_WINERR_RETURN(pme);

    y = 1;
    SETAEERECT(&rect, 0, y, cx, cy - y);

#ifdef FEATURE_MULTISEQUENCER
    if (pme->m_pOwner->m_bIsMultiSequencer)
    {
      dwProps = MP_MULTI_SEL;
    }
#endif

    MP_SetMenuAttr(pme->m_pFileMenu, AEECLSID_MENUCTL, pme->m_pOwner->m_nColorDepth, &rect, dwProps);
    if(pme->m_pOwner->m_bDeleteFile == TRUE)
    {
      (void)IMENUCTL_SetTitle(pme->m_pFileMenu, MEDIAPLAYER_RES_FILE, IDS_FILELISTDELETE_TITLE, NULL);
    }
    else
    {
#if defined(MP_FEATURE_MBMS)
      (void)IMENUCTL_SetTitle(pme->m_pFileMenu, MEDIAPLAYER_RES_FILE, IDS_CHANNELLISTMENU_TITLE, NULL);
#else
      (void)IMENUCTL_SetTitle(pme->m_pFileMenu, MEDIAPLAYER_RES_FILE, IDS_FILELISTMENU_TITLE, NULL);
#endif
    }

#ifdef MP_FEATURE_ICONTENT
    if(pme->m_pOwner->m_bIContentMode)
      CMediaPlayer_RefreshExplorer(pme->m_pOwner,"");
    else
#endif
    {
#ifdef MP_FEATURE_MBMS
      // If the mbms dir exists Move to that directory
      // This is a hack for the MBMS Demo
      IFileMgr *pIFileMgr;

      (void)ISHELL_CreateInstance(pme->m_pOwner->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
      if (pIFileMgr && IFILEMGR_Test(pIFileMgr, MP_MBMS_MEDIA_DIR) == SUCCESS)
      {
        STRLCPY(pme->m_pOwner->m_FileExplorer.m_nCurDir, MP_MBMS_MEDIA_DIR, sizeof(pme->m_pOwner->m_FileExplorer.m_nCurDir));
        pme->m_pOwner->m_FileExplorer.m_nCurDepth = 2;
      }
#endif //MP_FEATURE_MBMS
      CMediaPlayer_RefreshExplorer(pme->m_pOwner,pme->m_pOwner->m_FileExplorer.m_nCurDir);
    }


    if (!CFileListWin_MakeList(pme, &pme->m_pOwner->m_FileExplorer))
      MP_WINERR_RETURN(pme);
  }

  return (IWindow *)pme;
}

/*===========================================================================
 Prepares the menu with list of files and directory in explorer
===========================================================================*/
boolean CFileListWin_MakeList(CFileListWin *pme, CFileExplore *pFileExp)
{
  uint16    wItemID = 0;
  AECHAR   *szBuf;

  szBuf = MALLOC(MP_MAX_FILE_NAME *sizeof(AECHAR));
  if (!szBuf)
    return FALSE;

  (void)IMENUCTL_DeleteAll(pme->m_pFileMenu);

  while (wItemID < pme->m_pOwner->m_FileExplorer.m_NumFiles)
  {
    char     *szName;

    if (pme->m_pOwner->m_FileExplorer.m_FileArray[wItemID].m_szName)
    {
      if (pme->m_pOwner->m_FileExplorer.m_FileArray[wItemID].m_bIsFile)
      {
        uint16    wIconID;

#ifdef MP_FEATURE_ICONTENT
        char      filepath[MP_MAX_FILE_NAME];
        stContent* pCT;
        
        if(pme->m_pOwner->m_bIContentMode)
        {
          pCT = MPContentMake(pme->m_pOwner);

          filepath[0] = '\0';
          (void)MPContentGetLocalURLfromURI(pCT, pme->m_pOwner->m_FileExplorer.m_FileArray[wItemID].m_szName, filepath, MP_MAX_FILE_NAME);
          MPContentFree(pCT);

          szName = MP_GetFileName(filepath);
          STRTOWSTR(szName, szBuf, MP_MAX_FILE_NAME);
        }
        else
#endif //MP_FEATURE_ICONTENT
        {

          szName = MP_GetFileName(pme->m_pOwner->m_FileExplorer.m_FileArray[wItemID].m_szName);

          // If we are showing the extension, just copy over the file name
          // otherwise remove the extention from the filename
          if (pme->m_pOwner->m_bShowExtension)
          {
            (void)STRTOWSTR(szName, szBuf, MP_MAX_FILE_NAME);
          }
          else
          {
            char fileTitle[MP_MAX_FILE_NAME + 1];
            (void)MEMSET(fileTitle, '\0', MP_MAX_FILE_NAME + 1);
            (void)MP_GetFileNameNoExtension(pme->m_pOwner->m_FileExplorer.m_FileArray[wItemID].m_szName, fileTitle, MP_MAX_FILE_NAME);
            (void)STRTOWSTR(fileTitle, szBuf, MP_MAX_FILE_NAME);
          }
        }

        wIconID = CMediaPlayer_FindHandlerType(pme->m_pOwner, szName);
        if (wIconID)
        {
          if(FALSE == MP_AddMenuItem(pme->m_pFileMenu, 0, szBuf, wIconID, (uint16)(EVT_START_FILELIST + wItemID), 0))
          {
            DBGPRINTF_ERROR("MP: failed to add file into menu, %s, id = %d.", szBuf, wIconID);
          }
        }
      }
      else
      {
        szName = pme->m_pOwner->m_FileExplorer.m_FileArray[wItemID].m_szName;
        (void)STRTOWSTR(szName, szBuf, MP_MAX_FILE_NAME);
        (void)MP_AddMenuItem(pme->m_pFileMenu, 0, szBuf, IDB_FOLDER, (uint16)(EVT_START_FILELIST + wItemID), 0);
      }
    }
    wItemID++;
  }

  FREE(szBuf);
  return TRUE;
}


/*===========================================================================
   This function deletes the file list window.
===========================================================================*/
void CFileListWin_Delete(IWindow *po)
{
  CFileListWin *pme = (CFileListWin *)po;

  // Reset delete flag when file window deleted
  pme->m_pOwner->m_bDeleteFile = FALSE;

  if (pme->m_pFileMenu)
    pme->m_pOwner->m_wFileListWin = IMENUCTL_GetSel(pme->m_pFileMenu);
  MP_RELEASEMENU(pme->m_pOwner, pme->m_pFileMenu);

  FREE(pme);
}

/*===========================================================================
   This function enables/disables the file list window.
===========================================================================*/
void CFileListWin_Enable(IWindow *po, boolean bEnable)
{
  CFileListWin *pme = (CFileListWin *)po;

  if (!CWindow_ProcessEnable(po, bEnable))
    return;

  if (!pme->m_bActive)
  {
    IMENUCTL_SetActive(pme->m_pFileMenu, FALSE);
    return;
  }

  if (pme->m_pOwner->m_FileExplorer.m_NumFiles)
  {
    IMENUCTL_SetActive(pme->m_pFileMenu, TRUE);
    IMENUCTL_SetSel(pme->m_pFileMenu, pme->m_pOwner->m_wFileListWin);
  }
  else
  {
    MP_ErrorDlg(pme->m_pOwner, IDS_ERR_NOFILES);
    pme->m_pOwner->m_FileExplorer.m_nCurDepth = 0;
  }
}

/*===========================================================================
   This function redraws the file list window.
===========================================================================*/
void CFileListWin_Redraw(IWindow *po)
{
  CFileListWin *pme = (CFileListWin *)po;

  if (!pme->m_bActive)
    return;

  IDISPLAY_ClearScreen(pme->m_pIDisplay);

  (void)IMENUCTL_Redraw(pme->m_pFileMenu);

  IDISPLAY_Update(pme->m_pIDisplay);
}

/*===========================================================================
   This function processes events routed to the file list window.
===========================================================================*/
boolean CFileListWin_HandleEvent(IWindow *po, AEEEvent eCode, uint16 wParam,
                         uint32 dwParam)
{
#ifdef FEATURE_KEY_HOLD_SCROLL
  int       ret;
#endif //FEATURE_KEY_HOLD_SCROLL
  CFileListWin *pme = (CFileListWin *)po;

  if(pme == NULL || pme->m_pOwner == NULL)
  {
    DBGPRINTF_ERROR("MP: File list win, NULL ptr.");
    return FALSE;
  }

#ifdef FEATURE_KEY_HOLD_SCROLL
  if (MP_ISONEOFKEYEVENT(eCode) && (wParam == AVK_SELECT || wParam == AVK_CLR))
  {
    /*
     * if timer is enabled for auto-scrolling, do nothing here
     */
    if (pme->m_pOwner->m_KeyStore.m_wParam_Key)
      return TRUE;
  }
#endif

  if (CMediaPlayer_HandleExplorerEvt(pme->m_pOwner, eCode, wParam))
    return TRUE;

  if (MP_ISCLR(eCode) || MP_ISCMD(eCode, IDS_OK))
  {
    return CMediaPlayer_SetWindow(pme->m_pOwner, MPW_MAIN, 0);
  }

#ifdef FEATURE_MULTISEQUENCER
  if ((pme->m_pOwner->m_bIsMultiSequencer) && (eCode == EVT_CTL_TAB))
  {
    uint16    nIndex;

    pme->m_pOwner->m_MSFiles.m_wNumSequences = 0;
    /*
     * Get all the selections from the MultiSelection
     */
    if (IMENUCTL_EnumSelInit(pme->m_pFileMenu))
    {
      while (((nIndex = IMENUCTL_EnumNextSel(pme->m_pFileMenu)) != 0xFFFF) &&
             (pme->m_pOwner->m_MSFiles.m_wNumSequences < MAX_NUM_SEQUENCES))
      {
        (void)STRLCPY(pme->m_pOwner->m_MSFiles.m_szSequenceFiles[pme->m_pOwner->m_MSFiles.m_wNumSequences],
                     pme->m_pOwner->m_FileExplorer.m_FileArray[nIndex - EVT_START_FILELIST].m_szName, 
                     sizeof(pme->m_pOwner->m_MSFiles.m_szSequenceFiles[pme->m_pOwner->m_MSFiles.m_wNumSequences]));
        (pme->m_pOwner->m_MSFiles.m_wNumSequences)++;
      }
#ifdef MP_FEATURE_ICONTENT
      if(pme->m_pOwner->m_bIContentMode)
        MPContentSequencerSelectFiles(pme->m_pOwner->m_pContent);
      else
#endif //MP_FEATURE_ICONTENT
      CMediaPlayer_PlayMultiSequence(pme->m_pOwner);

      /*
       * we are entering the new player win, return from here
       */
      return TRUE;
    }
  }
#endif

#ifdef FEATURE_KEY_HOLD_SCROLL
  ret = MP_HandleKeyEventRepeat(po, eCode, wParam, dwParam, pme->m_pFileMenu,
                                KEY_MENU_REPEAT_TIME_MS);
  if (ret >= 0)
    return TRUE;
#else
  if (MP_ISEVTKEY(eCode))
  {
    return IMENUCTL_HandleEvent(pme->m_pFileMenu, eCode, wParam, dwParam);
  }
#endif

  if (MP_ISEVTCMD(eCode) && (wParam == EVT_UPDATE_FILELIST))
  {
    if (!CFileListWin_MakeList(pme, &pme->m_pOwner->m_FileExplorer))
      return FALSE;
    CFileListWin_Redraw(po);
    return TRUE;
  }

  return FALSE;
}

static boolean CMediaPlayer_HandleExplorerEvt(CMediaPlayer *pme, AEEEvent eCode, uint16 wParam)
{
  char      szPath[MP_MAX_FILE_NAME];

  szPath[0] = '\0';
  if (MP_ISCLR(eCode))
  {
    /*
     * go one level up
     */
#ifdef MP_FEATURE_ICONTENT
    if(pme->m_bIContentMode)
    {
      AECHAR* paeStr;
      // remove top cat item from stack
      paeStr = MPStackPop(pme->m_pContent->m_pStack);
      if(paeStr)
      {
        FREEIF(paeStr);
      }
      else
      {
        pme->m_FileExplorer.m_nCurDir[0] = '\0';
        return FALSE;
      }
    }
    else
#endif //MP_FEATURE_ICONTENT
    {
      if (!pme->m_FileExplorer.m_nCurDepth)
        return FALSE;

      (void)MP_GetParentDir(pme, pme->m_FileExplorer.m_nCurDir, szPath, sizeof(szPath));
    }

    pme->m_FileExplorer.m_nCurDepth--;
    CMediaPlayer_RefreshExplorer(pme, szPath);
    (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, EVT_UPDATE_FILELIST, 0);
    return TRUE;
  }

  if (MP_ISEVTCMD(eCode) && (wParam >= EVT_START_FILELIST))
  {
    uint16    nIndex = (uint16) (wParam - EVT_START_FILELIST);

    (void)STRLCPY(szPath, pme->m_FileExplorer.m_FileArray[nIndex].m_szName, sizeof(szPath));
    pme->m_CurFileIndex = nIndex;
    if (pme->m_FileExplorer.m_FileArray[nIndex].m_bIsFile)
    {
      // If delete file, delete and refresh. Else play
      if(pme->m_bDeleteFile == TRUE)
      {
        IFileMgr *pIFileMgr;

        (void)ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
        if (pIFileMgr)
        {
          if(SUCCESS != IFILEMGR_Remove(pIFileMgr, szPath))
            DBGPRINTF_ERROR("MP: Failed to delete %s", szPath);
          CMediaPlayer_RefreshExplorer(pme, pme->m_FileExplorer.m_nCurDir);
          MP_RELEASEIF(pIFileMgr);
          (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, EVT_UPDATE_FILELIST, 0);
        }
        else
        {
          DBGPRINTF_ERROR("MP: Failed to create FILEMGR to delete %s", szPath);
        }
      }
      else
      {
        MPFileListPlayFile(pme, szPath);
      }
    }
    else
    {
      /*
       * go one directory down
       */
      pme->m_FileExplorer.m_nCurDepth++;
      CMediaPlayer_RefreshExplorer(pme, szPath);
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, EVT_UPDATE_FILELIST, 0);
    }
    return TRUE;
  }

  return FALSE;
}

static void MPFileListPlayFile(CMediaPlayer* pme, const char* szPath)
{
#ifdef MP_FEATURE_ICONTENT
  if(pme->m_bIContentMode)
  {
#ifdef FEATURE_MULTISEQUENCER
    if(!pme->m_bIsMultiSequencer) // select a single file and play
#endif //FEATURE_MULTISEQUENCER
      (void)MPContentSelectFile(pme->m_pContent, szPath, 0, FALSE);
  }
  else
#endif //MP_FEATURE_ICONTENT
  {
#ifdef MP_FEATURE_ICONTENTHANDLER
    if(pme->m_bIContentMode)
    {
      (void)MPContentSelectFile(pme->m_pContent, szPath, 0, FALSE);
    }
    else
#endif //#ifdef MP_FEATURE_ICONTENTHANDLER
    CMediaPlayer_OnSelectFile(pme, szPath);
  }
}
/*===========================================================================
   This function creates the MP_MEDIA_DIR and MP_MMC_MEDIA_DIR root directories
===========================================================================*/
void CMediaPlayer_InitializeExplorer(CMediaPlayer *pme)
{
  IFileMgr *pIFileMgr;

#if MIN_BREW_VERSION(4,0)
  char pszUIMStoragePath[MP_MAX_FILE_NAME];
#endif //#if MIN_BREW_VERSION(4,0)

#ifdef MP_FEATURE_ICONTENT
  if(pme->m_bIContentMode)
  {
    pme->m_FileExplorer.m_nCurDir[0] = '\0';
    return;
  }
#endif //MP_FEATURE_ICONTENT

  (void)ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);

  /*
   * free if any memory used
   */
  CMediaPlayer_FreeExplorerData(pme);

  /*
   * allocate memory for root directories
   */
  pme->m_FileExplorer.m_FileArray = (CFileArray *)MALLOC(sizeof(CFileArray) * MP_MAX_ROOTDIRS);
  if (!pme->m_FileExplorer.m_FileArray)
  {
    /*
     * clean up on failure
     */
    MP_RELEASEIF(pIFileMgr);
    return;
  }

  /*
   * add root(s)for explorer
   */
  if (MP_MakeDir(pIFileMgr, MP_MEDIA_DIR))
  {    
    (void)STRLCPY(pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles], MP_MEDIA_DIR, 
                  sizeof(pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles]));
    (void)STRLCPY(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName, MP_MEDIA_DIR, 
                  sizeof(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName));
    pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_bIsFile = FALSE;
    pme->m_FileExplorer.m_NumFiles++;
    DBGPRINTF_MED("MP: Adding root for %s", MP_MEDIA_DIR);
  }
  else
  {
    pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles][0] = '\0';
    DBGPRINTF_MED("MP: No root for %s", MP_MEDIA_DIR);
  }

  if (IFILEMGR_Test(pIFileMgr, MP_MMC_MEDIA_DIR) == SUCCESS)
  {
    (void)STRLCPY(pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles], MP_MMC_MEDIA_DIR, 
                  sizeof(pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles]));
    (void)STRLCPY(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName, MP_MMC_MEDIA_DIR, 
                  sizeof(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName));
    pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_bIsFile = FALSE;
    pme->m_FileExplorer.m_NumFiles++;
    DBGPRINTF_MED("MP: Adding root for %s", MP_MMC_MEDIA_DIR);
  }
  else
  {
    pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles][0] = '\0';
    DBGPRINTF_MED("MP: No root for %s", MP_MMC_MEDIA_DIR);
  }

  // Attempting to add the USB UICC card (usb1) as a mass storage.  If the device has it, we look for the media subfolder.
  // the usb1 drive is normally translated into "fs:/mod/26108"
  // The feature is only defined in BREW 4+ 
#if MIN_BREW_VERSION(4,0)
  (void)MEMSET(pszUIMStoragePath, 0, sizeof (pszUIMStoragePath) );
  if (MP_GetUIMMediaPlayerRoot(pIFileMgr, pszUIMStoragePath, sizeof (pszUIMStoragePath)))
  {
    (void)STRLCPY(pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles], pszUIMStoragePath, 
                  sizeof(pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles]));
    (void)STRLCPY(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName, pszUIMStoragePath, 
                  sizeof(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName));
    pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_bIsFile = FALSE;
    pme->m_FileExplorer.m_NumFiles++;
  }
  else
  {
     pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles][0] = '\0';
  }
#endif // #if MIN_BREW_VERSION(4,0)

#ifdef MP_FEATURE_ICONTENTHANDLER
  if (IFILEMGR_Test(pIFileMgr, MP_ICONTENT_MEDIA_DIR) == SUCCESS)
  {
    (void)STRLCPY(pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles], MP_ICONTENT_MEDIA_DIR, 
                  sizeof(pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles]));
    (void)STRLCPY(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName, MP_ICONTENT_MEDIA_DIR, 
                  sizeof(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName));
    pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_bIsFile = FALSE;
    pme->m_FileExplorer.m_NumFiles++;
  }
  else
  {
    pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles][0] = '\0';
  }

  if (IFILEMGR_Test(pIFileMgr, MP_MMC_ICONTENT_MEDIA_MUSIC_DIR) == SUCCESS)
  {
    (void)STRLCPY(pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles], MP_MMC_ICONTENT_MEDIA_MUSIC_DIR, 
                  sizeof(pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles]));
    (void)STRLCPY(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName, MP_MMC_ICONTENT_MEDIA_MUSIC_DIR, 
                  sizeof(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName));
    pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_bIsFile = FALSE;
    pme->m_FileExplorer.m_NumFiles++;
  }
  else
  {
    pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles][0] = '\0';
  }

  if (IFILEMGR_Test(pIFileMgr, MP_MMC_ICONTENT_MEDIA_VIDEO_DIR) == SUCCESS)
  {
    (void)STRLCPY(pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles], MP_MMC_ICONTENT_MEDIA_VIDEO_DIR, 
                  sizeof(pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles]));
    (void)STRLCPY(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName, MP_MMC_ICONTENT_MEDIA_VIDEO_DIR, 
                  sizeof(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName));
    pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_bIsFile = FALSE;
    pme->m_FileExplorer.m_NumFiles++;
  }
  else
  {
    pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles][0] = '\0';
  }
#endif //MP_FEATURE_ICONTENTHANDLER


#ifdef FEATURE_EFS_FTL
  if (IFILEMGR_Test(pIFileMgr, MP_FTL_MEDIA_DIR) == SUCCESS)
  {
    (void)STRLCPY(pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles], MP_FTL_MEDIA_DIR, 
                  sizeof(pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles]));
    (void)STRLCPY(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName, MP_FTL_MEDIA_DIR, 
                  sizeof(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName));
    pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_bIsFile = FALSE;
    pme->m_FileExplorer.m_NumFiles++;
  }
  else
  {
    pme->m_FileExplorer.m_szRoots[pme->m_FileExplorer.m_NumFiles][0] = '\0';
  }
#endif

  pme->m_FileExplorer.m_nCurDir[0] = '\0';
  pme->m_FileExplorer.m_nCurDepth = 0;

  MP_RELEASEIF(pIFileMgr);
}


/*===========================================================================
   This function refreshes the list of files in MediaPlayer.
===========================================================================*/
void CMediaPlayer_RefreshExplorer(CMediaPlayer *pme, const char *pszDir)
{
  int     nFCount,  i;
  stMPIO* pIO;

  (void)STRLCPY(pme->m_FileExplorer.m_nCurDir, pszDir, sizeof(pme->m_FileExplorer.m_nCurDir));

#ifdef MP_FEATURE_ICONTENT
  if(pme->m_bIContentMode)
  {
    AECHAR* pwStr;
    if(pme->m_pContent)
    {
      // save current cat to stack
      if(pszDir && STRLEN(pszDir) > 0)
      {
        pwStr = (AECHAR*)MALLOC(sizeof(AECHAR)*(STRLEN(pszDir) + 1));
        if(pwStr)
        {
          STRTOWSTR(pszDir, pwStr, sizeof(AECHAR)*(STRLEN(pszDir) + 1));
          MPStackPush(pme->m_pContent->m_pStack, pwStr);
        }
        else
          DBGPRINTF_ERROR("MP: cannot allocate memory.");
      }
      MPContentBrowse(pme->m_pContent);
    }
    return;
  }
#endif //MP_FEATURE_ICONTENT

  if (pme->m_FileExplorer.m_nCurDepth == 0)
  {
    CMediaPlayer_InitializeExplorer(pme);
    return;
  }

  /*
   * free if any memory used
   */
  CMediaPlayer_FreeExplorerData(pme);

  /*
   * allocate memory for files
   */
  if (pszDir == NULL)
   DBGPRINTF_HIGH("MP: current directory is NULL!");
  else
   DBGPRINTF_MED("MP: current directory is %s", pszDir);
  nFCount = MP_GetNumFiles(pme, pszDir);
  if(nFCount >= MP_MAX_FILE_COUNT)
  {
    nFCount = MP_MAX_FILE_COUNT - 1;
    DBGPRINTF_MED("MP: exceed max. file count.");
  }

  if (!nFCount)
  {
    DBGPRINTF_MED("MP: current directory contains no file.");
    return;
  }
  pme->m_FileExplorer.m_FileArray = (CFileArray *)MALLOC(sizeof(CFileArray) * (unsigned int)nFCount);
  if (!pme->m_FileExplorer.m_FileArray)
    return;

  pIO = MPMakeIO(pme->a.m_pIShell);
  if(!pIO)
    return;
 
  /*
   * i == 0 - Directories
   * i == 1 - Files
   */
  for (i = 0; i < 2; i++)
  {
    /*
     * first get the list of sub-directories
     */
    if (i == 0)
      pIO->m_EnumeType = IO_ENUME_DIR;
    else
      pIO->m_EnumeType = IO_ENUME_FILE;
    if(!MPIOInit(pIO, pszDir))
    {
      MPIO_FREEIF(pIO);
      return;
    }

    while ((pme->m_FileExplorer.m_NumFiles < nFCount) && MPIOEnumNext(pIO))
    {
#ifndef FEATURE_BREW_3_0
      if (!STRNCMP(pszDir, MP_MMC_MEDIA_DIR, sizeof(MP_MMC_MEDIA_DIR) - 1))
      {
        char     *pBuf;
        int       nSize;

        nSize =  sizeof(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName);
        pBuf = MALLOC(nSize);
        if (pBuf)
        {
          (void)STRLCPY(pBuf, "mmc1/", nSize);
          (void)STRLCAT(pBuf, fi.szName, nSize);
          (void)STRLCPY(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName, pBuf, nSize);
          FREE(pBuf);
          pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_bIsFile = (boolean)i;
          pme->m_FileExplorer.m_NumFiles++;
        }
      }
      else
#endif     // FEATURE_BREW_3_0
      {
        if(pIO->m_FileInfo.nMaxFile && pIO->m_FileInfo.pszFile)
        {          
          (void)STRLCPY(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName, pIO->m_FileInfo.pszFile, 
                        sizeof(pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName));
        }
        else
        {
          pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_szName[0] = '\0';
        }

        pme->m_FileExplorer.m_FileArray[pme->m_FileExplorer.m_NumFiles].m_bIsFile = (boolean)i;
        pme->m_FileExplorer.m_NumFiles++;
      
      }
    }
  }

  MPIO_FREEIF(pIO);
}


/*************************************************************************/

/*****   This function returns number of files and sub directories  ******/

/*****   in given directory                                         ******/

/*************************************************************************/
int MP_GetNumFiles(CMediaPlayer *MP, const char *pszDir)
{
  int     nCount = 0;
  stMPIO* pIO;
  
  pIO= MPMakeIO(MP->a.m_pIShell);
  if(pIO)
  {
    nCount = MPIOGetNumItems(pIO, pszDir);
    MPIO_FREEIF(pIO);
  }
  return nCount;
}


/*************************************************************************/

/*****   This function returns the index of the file in             ******/

/*****   the given directory                                        ******/

/*************************************************************************/
int MP_GetCurrentIndex(CMediaPlayer *pme, const char *pszDirName, const char *pszFile)
{
  stMPIO* pIO;
  int     i = 0;

  pIO= MPMakeIO(pme->a.m_pIShell);
  if(!pIO)
    return 0;

  pIO->m_EnumeType = IO_ENUME_FILE;
  if(!MPIOInit(pIO, pszDirName))
  {
    MPIO_FREEIF(pIO);
    return 0;
  }

  while (MPIOEnumNext(pIO))
  {
    char     *fiName,
             *pszFileName;

    /*
     * Get the filenames.
     */
    fiName = MP_GetFileName(pIO->m_FileInfo.pszFile);
    pszFileName = MP_GetFileName(pszFile);

    /*
     * Compare filenames...
     */
    if (STRCMP(fiName, pszFileName) == 0)
    {
      break;
    }
    i++;
  }

  MPIO_FREEIF(pIO);
  return i;
}

/*************************************************************************/

/*****   This function returns the depth of the given directory     ******/

/*************************************************************************/
int MP_GetCurrentDepth(const char* pszDir)
{
  int         numForwardSlashes = 0;
  const char* pszDirTmp = pszDir;

  if (pszDirTmp != NULL)
  {
    while (*pszDirTmp != '\0')
    {
      if (*pszDirTmp == '/')
      {
        numForwardSlashes++;
      }
      pszDirTmp++;
    }
    if (!STRNCMP(pszDir, MP_MMC_MEDIA_DIR, sizeof(MP_MMC_MEDIA_DIR) - 1))
    {
      numForwardSlashes--;
    }
  }

  return numForwardSlashes;
}

boolean MP_GetParentDir(CMediaPlayer *pme, const char *pszFilePath, char *pszDest, uint16 nSizeDest)
{
  int       i;
  int       nIndex = 0;

#ifdef MP_FEATURE_ICONTENT
  AECHAR* paeStr;
  uint32  nItemSize;
  if(pme->m_bIContentMode)
  {
    // get stored previous URI string
    nItemSize = MPItemStoreGetSize(pme->m_pContent->m_pStack);
    paeStr = MPItemStoreGetAt(pme->m_pContent->m_pStack, nItemSize - 2);
    if(paeStr != NULL && pszDest != NULL)
    {
      WSTRTOSTR(paeStr, pszDest, nSizeDest);
      return TRUE;
    }
    return FALSE;
  }
#endif //MP_FEATURE_ICONTENT

  for (i = 0; i < (int)strlen(pszFilePath); i++)
  {
    if (pszFilePath[i] == DIRECTORY_CHAR)
      nIndex = i;
  }

  if (!nIndex)
    return FALSE;

  nIndex++;
  if(nSizeDest >= (nIndex + 1))
    (void)STRLCPY(pszDest, pszFilePath, (unsigned int)nIndex);
  else
    return FALSE;
  return TRUE;
}

/*===========================================================================
 Frees the memory allocated by file explorer and sets NumFile to zero.
==============================================================================*/
void CMediaPlayer_FreeExplorerData(CMediaPlayer *pme)
{
  if (pme->m_FileExplorer.m_FileArray)
  {
    FREE((void *)pme->m_FileExplorer.m_FileArray);
    pme->m_FileExplorer.m_FileArray = NULL;
  }
  pme->m_FileExplorer.m_NumFiles = 0;
}
/*===========================================================================
 this function will reallocate the file explorer memory
==============================================================================*/
boolean CMediaPlayer_ResetExplorerData(CMediaPlayer *pme, int size)
{
  // free existing data
  CMediaPlayer_FreeExplorerData(pme);
  pme->m_FileExplorer.m_FileArray = (CFileArray *)MALLOC(sizeof(CFileArray) * (unsigned int)size);
  if (!pme->m_FileExplorer.m_FileArray)
  {
    DBGPRINTF_ERROR("MP: failed to allocate memory, size = %d.", sizeof(CFileArray)*size);
    return FALSE;
  }
  return TRUE;
}
