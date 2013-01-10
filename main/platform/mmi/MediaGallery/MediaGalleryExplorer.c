/**--------------------------------------------------------------------------
 * MediaGalleryExplorer.c -- implement file explore. 
 *                          
 *
 * Copyright(c) 2008 Anylook
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2008.10.29       Create.
 *
 **--------------------------------------------------------------------------
 */
#include "MediaGallery.h"
#include "MediaGalleryApp.h"
#include "MediaGalleryExplorer.h"
#include "AppComFunc.h"
#include "Msg.h"

/*===========================================================================
 * 
 *                      FUNCTION DECLARATIONS
 *
 * ==========================================================================
*/

static int MGExplorer_GetItemIcon(CFileIcons *pFileIcons,
                                 MGMimeType eMime,
                                 IImage** ppIcon);
static boolean MGExplorer_GetSysDirStr(IShell *pIShell,
                                       char *pszFile,
                                       AECHAR *pwszFolder,
                                       uint16 nByte);
static void MGExplorer_BuildMediaMenu(void *po);
static void MGExplorer_BuildMediaMenuComplete(void *po);

static void MGExplorer_BuildFileList(void *po);
static void MGExplorer_BuildVideoDatabase(void *po);
static void MGExplorer_GetFolderInfo(void *po);
static int MGExplorer_CompareFilename(CMediaGalleryApp *pMe,
                                      MGFileInfo *pItemData1 ,
                                      MGFileInfo *pItemData2);
static int MGExplorer_MediaMenuSortCompare(void * pUser,
                                           uint32 dwData1,
                                           uint32 dwData2);
/*===========================================================================
 *
 *                              LOCAL/STATIC DATA
 *
 *============================================================================
*/

/*===========================================================================
 *                      Public Function Definitions
 *===========================================================================
 */
boolean MGExplorer_Init(CFSExplorer *pFSExplorer,
                        IFileMgr *pFileMgr,
                        const char *cpszPath,
                        uint16 eMimeType)
{
   boolean bExist;
#ifdef FEATURE_MG_LONGPATH
   AEEFileInfoEx fi;
#endif

   if(NULL == pFSExplorer || NULL == pFileMgr || NULL == cpszPath)
   {
      MG_FARF(ADDR, ("MGExplorer_Init failed!!!"));
      return FALSE;
   }

   /*Set all previous media menu select item id to MG_MENUITEM_NULL,
    * do not put in function MGAppUtil_InitMediaMenu, because we pop
    * up menu, after operation ,we may need rebuild media menu
    * dialog, and under this condtion, we do not want to reset item
    * index
    * */
   MGExplorer_ResetPreItemID(pFSExplorer, TRUE);

  // if(STRCMP(MG_MASSCARD_ROOTDIR, cpszPath) != 0)
   {
      /*check directory*/
      bExist = MGExplorer_CheckFolderExist(pFileMgr,
                                           &cpszPath,
                                           1,
                                           TRUE);

      if(FALSE == bExist)
         return bExist;
   }

#ifdef FEATURE_MG_LONGPATH
   MEMSET(&fi, 0, sizeof(fi));
   fi.nStructSize = sizeof(fi);
   fi.pszFile = pFSExplorer->m_RootNode.szName;
   fi.nMaxFile = sizeof(pFSExplorer->m_RootNode.szName);
   IFILEMGR_GetInfoEx(pFileMgr,cpszPath,&fi);
   pFSExplorer->m_RootNode.attrib = fi.attrib;
#else

   IFILEMGR_GetInfo(pFileMgr,
                    cpszPath,
                    &pFSExplorer->m_RootNode);
   //MEMCPY(&pFSExplorer->m_CurFolder,
   //      &pFSExplorer->m_RootNode,
   //      sizeof(MGFileInfo));

#endif
   MG_FARF(ADDR, ("Current folder %s, %d",
                  BASENAME(pFSExplorer->m_RootNode.szName),
                  pFSExplorer->m_RootNode.attrib));
   /*when get MG_MASSCARD_ROOTDIR information. the attrib is not correct*/
   pFSExplorer->m_RootNode.attrib = AEE_FA_DIR;

   MGExplorer_UpdateCurrentFolder(pFSExplorer,
                                  &pFSExplorer->m_RootNode,
                                  sizeof(MGFileInfo));
   MGExplorer_SetCurrentDepth(pFSExplorer, MG_CURDEPTH_INIT);
   MGExplorer_SetMime(pFSExplorer, eMimeType);

   return TRUE;
}


/*===========================================================================
 * FUNCTION:MGExplorer_InitBuildMediaMenu
 *
 * DESCRIPTION: do prepare work before build media menu that list media files
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
int MGExplorer_InitBuildMediaMenu(CMediaGalleryApp *pMe, PFNNOTIFY pfnNotify)
{
   const char *pszPath = NULL;

   if(!pMe || !pMe->m_pMediaMenu || !pMe->m_pFileMgr)
   {
      MG_FARF(ADDR, ("InitMediaMenu bad parameter!"));
      return EFAILED;
   }
   
   pszPath = MGExplorer_GetCurrentFolderPath(&pMe->m_Explorer);
   
   if(!pszPath || !STRLEN(pszPath))
   {
      MG_FARF(ADDR, ("BuildMediaMenu bad path name!!!"));
      return EFAILED;
   }

   CALLBACK_Cancel(&pMe->m_CallBack);
   CALLBACK_Init(&pMe->m_CallBack, MGExplorer_BuildMediaMenu, (void *)pMe);
   pMe->m_pfnNotify = pfnNotify;

   /*clean menu control*/
   MGExplorer_FreeMediaMenuItem(pMe->m_pMediaMenu);
   IMENUCTL_SetBottomBarType(pMe->m_pMediaMenu, BTBAR_BACK);

   pMe->m_nEnumeResult = MG_ENUM_UNKNOWN;
   pMe->m_bCallbackResumed = FALSE;
   pMe->m_bMediaMenuEmpty = TRUE;
   MediaGalleryApp_SetCallbackStateStart(pMe, MG_CBT_BUILDMENU);
   /*when first enumrate a folder, the first node is error, because m_pCurNode
    * point to previous menu item which have been freed.*/
   MediaGalleryApp_SetCurrentNode(pMe, NULL);
   pMe->m_Explorer.m_eType = MG_DOCTYPE_DIR;

   if(SUCCESS != IFILEMGR_EnumInit(pMe->m_pFileMgr, pszPath, TRUE/*directory*/))
   {
      pMe->m_Explorer.m_eType =  MG_DOCTYPE_MAX;
      pMe->m_nEnumeResult = MG_ENUM_FAILED;

      MG_FARF(ADDR, ("InitBuildMediaMenu failed!"));
      CALLBACK_Init(&pMe->m_CallBack,
                    MGExplorer_BuildMediaMenuComplete,
                    (void *)pMe);
      ISHELL_Resume(pMe->m_pShell, &pMe->m_CallBack);
      return EFAILED;
   }
 //  pMe->m_Explorer.m_eSortOrder = MG_SORT_NONE;
   MGExplorer_BuildMediaMenu(pMe);

   return SUCCESS;
}

/*===========================================================================
 * FUNCTION:MGExplorer_BuildMediaMenu
 *
 * DESCRIPTION: do build media menu, it list media files in the current
 * direcotry, and may not list some files for MIME type do not match the folder
 * MIME.
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static void MGExplorer_BuildMediaMenu(void *po)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)po;
   MGFileInfo    *pInfo = NULL;
   IImage* pImage;
   char* pszBasename;
   int nBasenameLen;
   CtlAddItem MenuItem={0};
#ifdef FEATURE_MG_LONGPATH
   AEEFileInfoEx fi;
#endif
   AECHAR FileName[MG_MAX_FILE_NAME];
   int16  nCount, nIndex;
   boolean bFinish = FALSE;
   int bGetMime = SUCCESS;
   MGMimeType eFileMime;
   uint16 uMimeBase;
   MGMimeType eExploreMime;
   IMenuCtl *pMenuCtl = NULL;
   
   if(!pMe || !pMe->m_pFileMgr || !pMe->m_pMediaMenu)
   {
      MG_FARF(ADDR, ("BuildMediaMenu bad parameter!!!"));
      return;
   }
   nIndex = 0;
   pMenuCtl = pMe->m_pMediaMenu;
   eExploreMime = MediaGalleryApp_GetExplorerMime(pMe);
   nIndex = IMENUCTL_GetItemCount(pMenuCtl);

#ifdef FEATURE_MG_LONGPATH
   MEMSET(&fi, 0, sizeof(fi));
   fi.nStructSize = sizeof(fi);
   fi.nMaxFile = sizeof(pInfo->szName);
#endif

/* WARNING: IF you find slowly, pls check compile feature AEE_DBG_HEAP whether
 * on, when you should turn off, the performance will be much better.
 */
   //enume nodes, add 8 item max once
   for(nCount = 0; nCount < 32; )
   {
      if(NULL == pInfo)
      {
         pInfo = (MGFileInfo *)MALLOC(sizeof(MGFileInfo));

         if(NULL == pInfo)
         {
            MG_FARF(ADDR, ("BuildMediaMenu memory allocate failed!"));
            goto EXITBUILDMEDIAMENU;
         }

         MEMSET (pInfo, 0, sizeof(MGFileInfo));
#ifdef FEATURE_MG_LONGPATH
         fi.pszFile = pInfo->szName;
#endif
      }

#ifdef FEATURE_MG_LONGPATH
      if(FALSE == IFILEMGR_EnumNextEx(pMe->m_pFileMgr, &fi))
#else
      if(FALSE == IFILEMGR_EnumNext(pMe->m_pFileMgr, pInfo))
#endif
      {
         if( pMe->m_Explorer.m_eType == MG_DOCTYPE_DIR)
         {
            const char *pszPath = NULL;

            pMe->m_Explorer.m_eType = MG_DOCTYPE_FILE;
            pszPath = MGExplorer_GetCurrentFolderPath(&pMe->m_Explorer);

            if(NULL == pszPath ||
               SUCCESS != IFILEMGR_EnumInit(pMe->m_pFileMgr,
                                            pszPath,
                                            FALSE/*enum file*/))
            {
               MG_FARF(ADDR, ("BuildMediaMenu file enum failed!"));

               pMe->m_nEnumeResult = MG_ENUM_FAILED;
               pMe->m_Explorer.m_eType = MG_DOCTYPE_MAX;
               bFinish = TRUE;
               FREEIF(pInfo);
               goto EXITBUILDMEDIAMENU;
            }
         }
         else if( pMe->m_Explorer.m_eType == MG_DOCTYPE_FILE)
         {
            MG_FARF(ADDR, ("BuildMediaMenu file enum complete!"));

            pMe->m_nEnumeResult = MG_ENUM_SUCCESS;
            pMe->m_Explorer.m_eType = MG_DOCTYPE_MAX;
            bFinish = TRUE;
            FREEIF(pInfo);
            goto EXITBUILDMEDIAMENU;
         }
         else
         {
            bFinish = TRUE;
            MG_FARF(ADDR, ("BuildMediaMenu m_eType error !"));
            FREEIF(pInfo);
            goto EXITBUILDMEDIAMENU;
         }
      }
      else
      {
         if(nIndex >= MG_MAX_FILES)
         {
            pMe->m_Explorer.m_eType = MG_DOCTYPE_MAX;
            pMe->m_nEnumeResult = MG_ENUM_OVERMAX;
            bFinish = TRUE;
            FREEIF(pInfo);
            goto EXITBUILDMEDIAMENU;
         }

         /*extract file name, and convert from UTF-8 code to AECHAR*/
         pszBasename = BASENAME(pInfo->szName);

         if(0 == (nBasenameLen = STRLEN(pszBasename)))
         {
            MG_FARF(ADDR, ("Basename null, %s", pInfo->szName));
            FREEIF(pInfo);
            continue;
         }

         UTF8TOWSTR((const byte*)pszBasename,
                    nBasenameLen+1,
                    FileName,
                    sizeof(FileName));

#ifdef FEATURE_MG_LONGPATH
         pInfo->attrib = fi.attrib;
         pInfo->dwCreationDate = fi.dwCreationDate;
         pInfo->dwSize = fi.dwSize;
#endif

         if(pInfo->attrib == AEE_FA_DIR)/*folder*/
         {
            eFileMime = MG_MIME_FOLDER;
            bGetMime = SUCCESS;
			
            if(pMe->m_StoreMedium == MG_STMED_MASSCARD)
            {
               /*848 will enumerate "." , "..", that is current directory and
                * parenet directory*/
               if(0 == STRCMP(pszBasename, MG_STR_DOT) ||
                  0 == STRCMP(pszBasename, MG_STR_DOUBLEDOT))
               {
                  continue;
               }
               else if(pMe->m_Explorer.m_nCurDepth == 1)
               {
                  MGExplorer_GetSysDirStr(pMe->m_pShell,
                                          pInfo->szName,
                                          FileName,
                                          sizeof(FileName));
               }
            }
         }
         else/*file*/
         {
            uMimeBase = MGMediaInfo_MimeBaseConvert(eExploreMime);

            bGetMime = MGMediaInfo_GetMimeType(pMe->m_pShell,
                                               pMe->m_pFileMgr,
                                               pInfo->szName,
                                               uMimeBase,
                                               &eFileMime);
         }

         if((SUCCESS != bGetMime && eExploreMime != MG_MIME_MISCELL) ||
            (SUCCESS != MGExplorer_GetItemIcon(pMe->m_pFileIcons,
                                               eFileMime, &pImage)))
         {
            continue;
         }

         MenuItem.wItemID = (uint16)(MG_EXPLORER_GENERALITEM + nIndex);
         MenuItem.pText = FileName;
         MenuItem.pImage = pImage;
         MenuItem.dwData = (uint32)pInfo;/*do not FREE pNode when use data*/
		 MG_FARF(ADDR, ("Enum directory name legth=%d",WSTRLEN(FileName)));
         MG_FARF(ADDR, ("Enum directory tree,%s", pInfo->szName));
         IMENUCTL_AddItemEx(pMenuCtl, &MenuItem);
         pInfo = NULL;

         nCount++, nIndex++;
      }
   }

EXITBUILDMEDIAMENU:
   if(bFinish)
   {
     // if(pMe->m_bCallbackResumed)
      {
         MGExplorer_BuildMediaMenuComplete(po);
      }
      #if 0
      else
      {
         CALLBACK_Cancel(&pMe->m_CallBack);
         CALLBACK_Init(&pMe->m_CallBack,
               MGExplorer_BuildMediaMenuComplete,
               po);
         ISHELL_Resume(pMe->m_pShell, &pMe->m_CallBack);
      }
      #endif
   }
   else
   {
      MGExplorer_UpdateMediaMenuTitle(pMe, pMenuCtl);
      IMENUCTL_Redraw(pMenuCtl);
      pMe->m_bCallbackResumed = TRUE;
      ISHELL_Resume(pMe->m_pShell, &pMe->m_CallBack);
   }
}//MGExplorer_BuildMediaMenu


/* FUNCTION : MGExplorer_BuildMediaMenuComplete
 * DESCRIPTION: At EVT_DIALOG_START, we begin to build menu, if the folder is
 * empty or less than file number enumrate at a time, and if we invike
 * pfnNotify immediately, handset will not show no file message, for AEEDialog
 * clear screen after EVT_DIALOG_START return.
 */
static void MGExplorer_BuildMediaMenuComplete(void *po)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)po;
   IMenuCtl *pMenuCtl = NULL;

   if(!pMe || !pMe->m_pMediaMenu)
   {
      MG_FARF(ADDR, ("BuildFileList Complete bad parameter!!!"));
      return;
   }

   pMenuCtl = pMe->m_pMediaMenu;

   if(MGExplorer_CheckMediaMenuEmpty(pMenuCtl))
   {
      //Media menu is empty
      pMe->m_bMediaMenuEmpty = TRUE;
      MGExplorer_UpdateMediaMenuTitle(pMe, pMenuCtl);
      IMENUCTL_Redraw(pMenuCtl);
   }
   else
   {
      if(MG_SORT_NONE != pMe->m_Explorer.m_eSortOrder)
      {
         MG_FARF(ADDR, ("Begin sort menu!"));
         IMENUCTL_SortCustom(pMenuCtl, MGExplorer_MediaMenuSortCompare, pMe);
         MG_FARF(ADDR, ("Sort finish!"));
      }

      pMe->m_bMediaMenuEmpty = FALSE;
      if(TRUE == MGExplorer_SetMediaMenuSelItem(&pMe->m_Explorer, pMenuCtl) ||
         (NULL == pMe->m_Explorer.m_pCurNode) )
      {
         /*it must update by hand at first!*/
         MGExplorer_GetMediaMenuSelectItem(&pMe->m_Explorer, pMenuCtl);
      }
      MGExplorer_UpdateMediaMenuTitle(pMe, pMenuCtl);
      IMENUCTL_SetActive(pMenuCtl, TRUE);
   }

      MediaGalleryApp_SetCallbackStateDone(pMe);
   if(pMe->m_pfnNotify)
      pMe->m_pfnNotify(pMe);

}//MGExplorer_BuildMediaMenuComplete



/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_BuildSubfolderMenu
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGExplorer_BuildSubfolderMenu(CFSExplorer *pFSExplorer,
                                  IFileMgr *pIFileMgr,
                                  IMenuCtl *pMenuCtl,
                                  CFileIcons *pIcons)
{
   IShell *pIShell = AEE_GetShell();
   const char* pszFolder = NULL;
   MGFileInfo    *pInfo = NULL;
   uint32 nIndex = 0;
#ifdef FEATURE_MG_LONGPATH
   AEEFileInfoEx fi;
#else
   MGFileInfo fi;
#endif
   CtlAddItem MenuItem={0};
   AECHAR FileName[MG_MAX_FILE_NAME];
   char* pszBasename;
   IImage* pImage = NULL;
   int nBasenameLen;
   uint8 nCurPath;
   boolean bSysDir = FALSE;

   if(NULL == pIFileMgr ||
      NULL == pFSExplorer ||
      NULL == pMenuCtl ||
      NULL == pIcons)
   {
      MG_FARF(ADDR, ("Bad parameter!"));
      return EFAILED;
   }

   pszFolder = MGExplorer_GetCurrentFolderPath(pFSExplorer);
   nCurPath = MGExplorer_GetCurrentDepth(pFSExplorer);

   MEMSET(&fi, 0, sizeof(fi));
#ifdef FEATURE_MG_LONGPATH
   fi.nStructSize = sizeof(fi);
   fi.nMaxFile = sizeof(pInfo->szName);
#endif

   MGExplorer_FreeMediaMenuItem(pMenuCtl);
   MGExplorer_GetItemIcon(pIcons, MG_MIME_FOLDER, &pImage);

   if( SUCCESS == IFILEMGR_EnumInit(pIFileMgr, pszFolder, TRUE))
   {
      while(1)
      {
         pInfo = (MGFileInfo*)MALLOC(sizeof(MGFileInfo));
         if(NULL == pInfo)
         {
            MG_FARF(ADDR, ("memory allocate failed!"));
            return ENOMEMORY;
         }
         MEMSET (pInfo, 0, sizeof(pInfo));
#ifdef FEATURE_MG_LONGPATH
         fi.pszFile = pInfo->szName;

         if(TRUE == IFILEMGR_EnumNextEx(pIFileMgr, &fi))
         {
#else
         if(TRUE == IFILEMGR_EnumNext(pIFileMgr, &fi))
         {
            MEMCPY(pInfo->mName, fi.szName, sizeof(fi.szName));
#endif

            if(nCurPath == MG_CURDEPTH_INIT)
            {
               bSysDir = MGExplorer_GetSysDirStr(pIShell,
                                                 pInfo->szName,
                                                 FileName,
                                                 sizeof(FileName));
            }

            if(FALSE == bSysDir)
            {
               /*extract file name, and convert from UTF-8 code to AECHAR*/
               pszBasename = BASENAME(pInfo->szName);

               if(0 == (nBasenameLen = STRLEN(pszBasename)))
               {
                  MG_FARF(ADDR, ("Basename null, %s", pInfo->szName));
                  FREEIF(pInfo);
                  continue;
               }

               UTF8TOWSTR((const byte*)pszBasename,
                          nBasenameLen+1,
                          FileName,
                          sizeof(FileName));
            }

            MenuItem.wItemID = (uint16)(MG_EXPLORER_GENERALITEM + nIndex++);
            MenuItem.pText = FileName;
            MenuItem.pImage = pImage;
            MenuItem.dwData = (uint32)pInfo;/*do not FREE pNode when use data*/
            MG_FARF(ADDR, ("Enum directory tree,%s", pInfo->szName));
            IMENUCTL_AddItemEx(pMenuCtl, &MenuItem);
         }
         else
         {
            /*Note: free it right, for it will be used by next statement, that get next folder*/
            FREEIF(pInfo);
            break;
         }
      }
   }

   if(TRUE == MGExplorer_SetMediaMenuSelItem(pFSExplorer, pMenuCtl) ||
      (NULL == pFSExplorer->m_pCurNode) )
   {
      /*it must update by hand at first!*/
      MGExplorer_GetMediaMenuSelectItem(pFSExplorer, pMenuCtl);
   }

   IMENUCTL_Redraw(pMenuCtl);
   return SUCCESS;
}//MGExplorer_BuildSubfolderMenu


/*===========================================================================
 * FUNCTION: MGExplorer_EnumFoldersList
 *
 * DESCRIPTION: enumerate all folders in the specific path recursely, store
 * them in the vector, and used to all files in that paht later.
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
int MGExplorer_EnumFoldersList(IFileMgr* pIFileMgr,
                               const char* cpszFilePath,
                               IVector* pFolderList,
                               boolean bRecursive)
{
    const char* pszFolder = cpszFilePath;
    NodeName* foldername = NULL;
    uint32 nIndex = 0;
    uint32 nSize;
#ifdef FEATURE_MG_LONGPATH
    AEEFileInfoEx fi;
#else
    MGFileInfo fi;
#endif

    if(!pIFileMgr || !cpszFilePath || !pFolderList)
        return EBADPARM;

 //  if(SUCCESS != IFILEMGR_Test(pIFileMgr, cpszFilePath))
 //     return EFAILED;
    MEMSET(&fi, 0, sizeof(fi));
#ifdef FEATURE_MG_LONGPATH
    fi.nStructSize = sizeof(fi);
    fi.nMaxFile = sizeof(foldername->m_szNodeName);
#endif

    MGExplorer_FreeVectorElements(pFolderList);

    foldername = (NodeName*)MALLOC(sizeof(NodeName));
    if(NULL == foldername)
    {
       return ENOMEMORY;
    }
    MEMCPY(foldername->m_szNodeName,
           cpszFilePath,
          sizeof(foldername->m_szNodeName));
    IVector_AddElement(pFolderList, (void*)foldername);
    nIndex++;

    do{
        if( SUCCESS == IFILEMGR_EnumInit(pIFileMgr, pszFolder, TRUE))
        {
            while(1)
            {
                foldername = (NodeName*)MALLOC(sizeof(NodeName));
                if(NULL == foldername)
                {
                    MG_FARF(ADDR, ("memory allocate failed!"));
                    return ENOMEMORY;
                }
                MEMSET (foldername, 0, sizeof(foldername));
#ifdef FEATURE_MG_LONGPATH
                fi.pszFile = foldername->m_szNodeName;

                if(TRUE == IFILEMGR_EnumNextEx(pIFileMgr, &fi))
                {
#else
                if(TRUE == IFILEMGR_EnumNext(pIFileMgr, &fi))
                {
                    MEMCPY(foldername->m_szNodeName, fi.szName, sizeof(fi.szName));
#endif
                    MG_FARF(ADDR, ("Enum directory list, folder:%s", foldername));
                    IVector_AddElement(pFolderList, (void*)foldername);
                }
                else
                {
                    /*Note: free it right, for it will be used by next statement, that get next folder*/
                    FREEIF(foldername);
                    break;
                }
            }
        }

        nSize = IVector_Size(pFolderList);

        if(nIndex < nSize)
        {
            foldername = IVector_ElementAt(pFolderList, nIndex);
            pszFolder = foldername->m_szNodeName;
        }

        nIndex++;
    }while(nIndex <= nSize && TRUE == bRecursive);
    
    return SUCCESS;
}//MGExplorer_EnumFoldersList

/*===========================================================================
 * FUNCTION:MGExplorer_InitBuildFileList
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
int MGExplorer_InitBuildFileList(CMediaGalleryApp* pMe, PFNNOTIFY pfnNotify)
{
   NodeName *pFolderName;
   uint32 nSize;
   boolean bRet = EFAILED;

   if(!pMe || !pMe->m_pMediaMenu || !pMe->m_pFileMgr || !pMe->m_pFolderList)
   {
      MG_FARF(ADDR, ("InitMediaMenu bad parameter!"));
      return EFAILED;
   }

   CALLBACK_Cancel(&pMe->m_CallBack);
   CALLBACK_Init(&pMe->m_CallBack, MGExplorer_BuildFileList, (void *)pMe);
   pMe->m_pfnNotify = pfnNotify;

   pMe->m_bCallbackResumed = FALSE;
   pMe->m_bMediaMenuEmpty = TRUE;
   pMe->m_nEnumeResult = MG_ENUM_UNKNOWN;
MediaGalleryApp_SetCallbackStateStart(pMe, MG_CBT_BUILDMENU);
   MediaGalleryApp_SetCurrentNode(pMe, NULL);

     /*clean menu control*/
   MGExplorer_FreeMediaMenuItem(pMe->m_pMediaMenu);
   IMENUCTL_SetBottomBarType(pMe->m_pMediaMenu, BTBAR_BACK);

   nSize = IVector_Size(pMe->m_pFolderList);

   if(0 == nSize)
   {
      MG_FARF(ADDR, ("folder list empty!"));
      bRet = EFAILED;
      goto EXITINITBUILDFILELIST;
   }

   pMe->m_nListIdx = 0;
   pFolderName = IVector_ElementAt(pMe->m_pFolderList, pMe->m_nListIdx++);

   if(NULL == pFolderName)
   {
      MG_FARF(ADDR, ("folder name pointer NULL!"));
      bRet = EFAILED;
      goto EXITINITBUILDFILELIST;
   }

   bRet = IFILEMGR_EnumInit(pMe->m_pFileMgr,
                           pFolderName->m_szNodeName,
                           FALSE);

   //FREEIF(pFolderName);
   //IVector_RemoveElementAt(pMe->m_pFolderList, nSize - 1);

   if(SUCCESS != bRet)
   {
      MG_FARF(ADDR, ("Enum init failed!"));
      bRet =  EFAILED;
      goto EXITINITBUILDFILELIST;
   }

   MGExplorer_BuildFileList(pMe);

EXITINITBUILDFILELIST:
   if(SUCCESS != bRet)
   {
      pMe->m_nEnumeResult = MG_ENUM_FAILED;
      MGExplorer_BuildMediaMenuComplete((void *)pMe);
   }

   return bRet;
}//MGExplorer_InitBuildFileList


/*===========================================================================
 * FUNCTION:MGExplorer_BuildFileList
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static void MGExplorer_BuildFileList(void *po)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)po;
   MGFileInfo    *pInfo = NULL;
   NodeName *pFolderName;
   //IImage* pImage;
   char *pszBasename;
   int nBasenameLen;
   CtlAddItem MenuItem={0};
#ifdef FEATURE_MG_LONGPATH
   AEEFileInfoEx fi;
#endif
   AECHAR FileName[MG_MAX_FILE_NAME];
   int16 nCount, nIndex;
   uint32 nSize;
   //uint32 dwProperties;
   //boolean bMulSel = TRUE; /*whether the menu control is multi-select*/
   boolean bFinish = FALSE;
   boolean bRet;
   int bMime = SUCCESS;
   MGMimeType eFileMime;
   uint16 uMimeBase;
   MGMimeType eExploreMime;
   IMenuCtl *pIMenu = NULL;

   if(!pMe || !pMe->m_pFileMgr || !pMe->m_pMediaMenu || !pMe->m_pFolderList)
   {
      MG_FARF(ADDR, ("BuildFileList bad parameter!!!"));
      return;
   }

   pIMenu = pMe->m_pMediaMenu;
   eExploreMime = MediaGalleryApp_GetExplorerMime(pMe);
   nIndex = IMENUCTL_GetItemCount(pIMenu);

#ifdef FEATURE_MG_LONGPATH
   MEMSET(&fi, 0, sizeof(fi));
   fi.nStructSize = sizeof(fi);
   fi.nMaxFile = sizeof(pInfo->szName);
#endif

   //dwProperties = IMENUCTL_GetProperties(pIMenu);
   //bMulSel = (MP_MULTI_SEL & dwProperties) ? TRUE : FALSE;
   nSize = IVector_Size(pMe->m_pFolderList);

   //enum file
   for(nCount = 0; nCount < 32;)
   {
      pInfo = (MGFileInfo *)MALLOC(sizeof(MGFileInfo));
      if(!pInfo)
      {
            pMe->m_nEnumeResult = MG_ENUM_FAILED;
         bFinish = TRUE;
         MG_FARF(ADDR, ("BuildMediaMenu memory allocate failed!"));
         goto EXITENUMFILESLIST;
      }

      MEMSET (pInfo, 0, sizeof(MGFileInfo));
#ifdef FEATURE_MG_LONGPATH
      fi.pszFile = pInfo->szName;

      if(FALSE == IFILEMGR_EnumNextEx(pMe->m_pFileMgr, &fi))
#else
      if(FALSE == IFILEMGR_EnumNext(pMe->m_pFileMgr, pInfo))
#endif
      {
          FREEIF(pInfo);

         if(pMe->m_nListIdx == nSize)
         {
            pMe->m_nEnumeResult = MG_ENUM_SUCCESS;
            bFinish = TRUE;
            FREEIF(pInfo);
            goto EXITENUMFILESLIST;
         }

         pFolderName = IVector_ElementAt(pMe->m_pFolderList,
                                         pMe->m_nListIdx++);

         if(NULL != pFolderName)
         {
            bRet = IFILEMGR_EnumInit(pMe->m_pFileMgr,
                                     pFolderName->m_szNodeName,
                                     FALSE);
         }

         if(NULL == pFolderName || SUCCESS != bRet)
         {
            pMe->m_nEnumeResult = MG_ENUM_FAILED;
            bFinish = TRUE;
            MG_FARF(ADDR, ("Enum init failed!"));
            FREEIF(pInfo);
            goto EXITENUMFILESLIST;
         }

         //FREEIF(pFolderName);
         //IVector_RemoveElementAt(pMe->m_pFolderList, nSize - 1);
      }
      else
      {
         if(nIndex >= MG_MAX_FILES)
         {
            pMe->m_nEnumeResult = MG_ENUM_OVERMAX;
            bFinish = TRUE;
            FREEIF(pInfo);
            goto EXITENUMFILESLIST;
         }

         /*extract file name, and convert from UTF-8 code to AECHAR*/
         pszBasename = BASENAME(pInfo->szName);

         if(0 == (nBasenameLen = STRLEN(pszBasename)))
         {
            MG_FARF(ADDR, ("Basename null, %s", pInfo->szName));
            FREEIF(pInfo);
            continue;
         }
         
         UTF8TOWSTR((const byte*)pszBasename,
                    nBasenameLen+1,
                    FileName,
                    sizeof(FileName));

#ifdef FEATURE_MG_LONGPATH
         pInfo->attrib = fi.attrib;
         pInfo->dwCreationDate = fi.dwCreationDate;
         pInfo->dwSize = fi.dwSize;
#endif

         if(pInfo->attrib == AEE_FA_DIR)/*folder*/
         {
            MG_FARF(ADDR, ("Encount folder when build file list"));
            FREEIF(pInfo);
            continue;
         }
         else/*file*/
         {
            uMimeBase = MGMediaInfo_MimeBaseConvert(eExploreMime);

            bMime = MGMediaInfo_GetMimeType(pMe->m_pShell,
                                            pMe->m_pFileMgr,
                                            pInfo->szName,
                                            uMimeBase,
                                            &eFileMime);
         }

         if((SUCCESS != bMime && eExploreMime != MG_MIME_MISCELL) /*||
           SUCCESS != MGExplorer_GetItemIcon(eFileMime, &pImage)*/)
         {
            FREEIF(pInfo);
            continue;
         }
         MenuItem.wItemID = (uint16)(MG_EXPLORER_GENERALITEM + nIndex);
         MenuItem.pText = FileName;
         MenuItem.pImage = NULL;
         MenuItem.dwData = (uint32)pInfo;/*do not FREE pNode when use data*/
         DBGPRINTF("Enum directory tree,%s", pInfo->szName);
         IMENUCTL_AddItemEx(pIMenu, &MenuItem);
         pInfo = NULL;

         nIndex++;
         nCount++;
         //if(TRUE == bMulSel)
         //{
         /*do not need, it will result in multi-selection menu very slow!
          * And place select at the last item!*/
         // IMENUCTL_SetSelEx(pIMenu, MenuItem.wItemID, FALSE);
         //}
      }
   }

EXITENUMFILESLIST:
   if(bFinish)
   {
   //   if(pMe->m_bCallbackResumed)
      {
         MGExplorer_BuildMediaMenuComplete(po);
      }
      #if 0
      else
      {
         CALLBACK_Cancel(&pMe->m_CallBack);
         CALLBACK_Init(&pMe->m_CallBack,
               MGExplorer_BuildMediaMenuComplete,
               po);
         ISHELL_Resume(pMe->m_pShell, &pMe->m_CallBack);
      }
      #endif
   }
   else
   {
      MGExplorer_UpdateMediaMenuTitle(pMe, pIMenu);

      IMENUCTL_Redraw(pIMenu);
      pMe->m_bCallbackResumed = TRUE;
      ISHELL_Resume(pMe->m_pShell, &pMe->m_CallBack);
   }
}//MGExplorer_BuildFileList

/*===========================================================================
 * FUNCTION:MGExplorer_InitBuildVideoDatabase
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
int MGExplorer_InitBuildVideoDatabase(CMediaGalleryApp* pMe,
                                       const char * pszDBFile,
                                       PFNNOTIFY pfnNotify)
{
   uint32 nSize;
   IDBMgr *pDBMgr;
   int nErr;

   nSize = IMENUCTL_GetItemCount(pMe->m_pMediaMenu);

   if(0 == nSize)
   {
      MG_FARF(ADDR, ("folder list empty!"));
      return EFAILED;
   }

#if 1
   // Open the database
   nErr = ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DBMGR, (void**)&pDBMgr);
   if (nErr != SUCCESS) {
      return FALSE;
   }

   //IDBMGR_SetCacheSize(pDBMgr, MG_DB_CACHE_SIZE);

   IDBMGR_Remove(pDBMgr, pszDBFile);

   RELEASEIF(pMe->m_pFilesDB);
   /*Specifies TRUE, for if the database must be created if the database is
    * not found.  */
   pMe->m_pFilesDB = IDBMGR_OpenDatabase(pDBMgr, pszDBFile, TRUE);

   IDBMGR_Release(pDBMgr);
   pDBMgr = NULL;

   if (pMe->m_pFilesDB == NULL) {
      MG_FARF(ADDR, ("no memory"));
#ifdef AEE_SIMULATOR
      if(pMe->m_pfnNotify)
         pMe->m_pfnNotify(pMe);
#endif
      return FALSE;
   }

   IDATABASE_Reset(pMe->m_pFilesDB);

#else

   if(SUCCESS != CMediaGallery_OpenFileInfoDatabase(pMe->m_pShell,
            MG_VIDEOFILE_DB_NAME,
            TRUE,
            &pMe->m_pFilesDB))
   {
      MG_FARF(STATE, ("Open database failed"));
      return EFAILED;
   }
#endif

   CALLBACK_Cancel(&pMe->m_CallBack);
   CALLBACK_Init(&pMe->m_CallBack, MGExplorer_BuildVideoDatabase, (void *)pMe);

   pMe->m_pfnNotify = pfnNotify;
   pMe->m_nEnumeResult = MG_ENUM_UNKNOWN;
   MediaGalleryApp_SetCallbackStateStart(pMe, MG_CBT_BUILDDATABASE);
   //ISHELL_Resume(pMe->m_pShell, &pMe->m_CallBack);
   MGExplorer_BuildVideoDatabase(pMe);
   return SUCCESS;
}//MGExplorer_InitBuildVideoDatabase

/*===========================================================================
 * FUNCTION:MGExplorer_BuildVideoDatabase
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static void MGExplorer_BuildVideoDatabase(void *po)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)po;
   uint32              nCount;
   uint16           uItemID;
   MGFileInfo         *pItemData;
   uint32           nSize;
   IMenuCtl         *pMediaMenu;

   if(!pMe || !pMe->m_pMediaMenu || !pMe->m_pFilesDB){
      MG_FARF(STATE, ("Error parameter"));
      return;
   }

   pMediaMenu = pMe->m_pMediaMenu;
   nSize = IMENUCTL_GetItemCount(pMediaMenu);

   for(nCount = 0; nCount < nSize; nCount++)
   {
      /*WARNING: use nSize instead of nCount, pItemData instead of &pItemData
       * will occur memory leak*/
      uItemID = IMENUCTL_GetItemID(pMediaMenu, nCount);
      IMENUCTL_GetItemData(pMediaMenu, uItemID, (uint32*)&pItemData);
      CMediaGallery_CreateFileInfoRecord(pMe->m_pFilesDB, pItemData);
   }

   if(pMe->m_pfnNotify)
      pMe->m_pfnNotify(pMe);

   MG_FARF(ADDR, ("build database list complete!"));

   //ISHELL_Resume(pMe->m_pShell, &pMe->m_CallBack);

}//MGExplorer_BuildVideoDatabase


/*===========================================================================
 * FUNCTION:MGExplorer_InitGetFolderInfo
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
int MGExplorer_InitGetFolderInfo(CMediaGalleryApp* pMe, PFNNOTIFY pfnNotify)
{
   const char *cpszPath = NULL;
   IVector* pFolderList = NULL;
   NodeName* foldername = NULL;

   if(!pMe )
   {
      MG_FARF(ADDR, ("MGExplorer_InitGetFolderInfo bad parameter!"));
      return EFAILED;
   }

   pFolderList = pMe->m_pFolderList;
   cpszPath = MGExplorer_GetCurrentNodeName(&pMe->m_Explorer);

   if(NULL == cpszPath || 0 == STRLEN(cpszPath) || NULL == pFolderList)
   {
      MG_FARF(ADDR, ("MGExplorer_InitGetFolderInfo bad path name!!!"));
      return EFAILED;
   }

   if(NULL ==  pMe->m_pFolderInfo)
       pMe->m_pFolderInfo = (MGFolderInfo *)MALLOC(sizeof(MGFolderInfo));

   if(NULL == pMe->m_pFolderInfo)
   {
      MG_FARF(ADDR, ("No memory"));
      return ENOMEMORY;
   }
   MEMSET(pMe->m_pFolderInfo, 0, sizeof(MGFolderInfo));

   MGExplorer_FreeVectorElements(pFolderList);

    foldername = (NodeName*)MALLOC(sizeof(NodeName));
    if(NULL == foldername)
    {
       MG_FARF(ADDR, ("No memory"));
       return ENOMEMORY;
    }
   MEMCPY(foldername->m_szNodeName, cpszPath, sizeof(foldername->m_szNodeName));
   IVector_AddElement(pFolderList, (void*)foldername);
   MediaGalleryApp_SetCallbackStateStart(pMe, MG_CBT_GETFOLDERINFO);

   CALLBACK_Cancel(&pMe->m_CallBack);
   CALLBACK_Init(&pMe->m_CallBack, MGExplorer_GetFolderInfo, (void *)pMe);
   pMe->m_pfnNotify = pfnNotify;

   pMe->m_Explorer.m_eType = MG_DOCTYPE_MAX;

   MGExplorer_GetFolderInfo(pMe);
   return SUCCESS;
}//MGExplorer_InitGetFolderInfo

/*===========================================================================
 * FUNCTION:MGExplorer_GetFolderInfo
 *
 * DESCRIPTION: release the media menu, and free the memory that attach item
 * data field
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static void MGExplorer_GetFolderInfo(void *po)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)po;
   int      nCount = 0;
   int      nSize = 0;
   MGDocType eEnumType;
   boolean  bInitEnum = FALSE;
   boolean bFinish = FALSE;
   const char *cpszPath = NULL;
   IVector* pFolderList = NULL;
   IFileMgr *pIFileMgr = NULL;
   NodeName *pFolderName;
#ifdef FEATURE_MG_LONGPATH
   NodeName sFileName;
   AEEFileInfoEx fi;
#else
   MGFileInfo    fi;
#endif


   if(NULL == pMe)
   {
      MG_FARF(ADDR, ("ReleaseMediaMenu bad parameter!"));
      return;
   }

   pFolderList = pMe->m_pFolderList;
   pIFileMgr = pMe->m_pFileMgr;
   eEnumType = pMe->m_Explorer.m_eType;

   if(NULL == pFolderList || NULL == pIFileMgr)
   {
      MG_FARF(ADDR, ("BAD PARAMETER"));
      return;
   }

   MEMSET(&fi, 0, sizeof(fi));
#ifdef FEATURE_MG_LONGPATH
   fi.nStructSize = sizeof(fi);
   fi.pszFile = sFileName.m_szNodeName;
   fi.nMaxFile = sizeof(sFileName.m_szNodeName);
#endif

   if(eEnumType == MG_DOCTYPE_MAX)
      bInitEnum = TRUE;

   for(;nCount < 64;)
   {
      if(TRUE == bInitEnum)
      {
         nSize = IVector_Size(pFolderList);
         if(0 == nSize)
         {
            bFinish = TRUE;
            goto EXITGETFOLDERINFO;
         }

         pFolderName = IVector_ElementAt(pFolderList, 0);
         cpszPath = pFolderName->m_szNodeName;

         switch(eEnumType)
         {
         case MG_DOCTYPE_FILE:/*FILE*/
            if(SUCCESS != IFILEMGR_EnumInit(pIFileMgr, cpszPath, FALSE))
            {
               bFinish = TRUE;
               goto EXITGETFOLDERINFO;
            }
            bInitEnum = FALSE;
            break;

         case MG_DOCTYPE_DIR:/*Directory*/
            if(SUCCESS != IFILEMGR_EnumInit(pIFileMgr, cpszPath, TRUE))
            {
               MG_FARF(ADDR, ("InitBuildMediaMenu failed!"));
               bFinish = TRUE;
               goto EXITGETFOLDERINFO;
            }
            bInitEnum = FALSE;
            break;

         case MG_DOCTYPE_MAX:
            eEnumType = MG_DOCTYPE_FILE;
            break;

         default:
            break;
         }
         continue;
      }

#ifdef FEATURE_MG_LONGPATH
      if(FALSE == IFILEMGR_EnumNextEx(pMe->m_pFileMgr, &fi))
#else
         if(FALSE == IFILEMGR_EnumNext(pMe->m_pFileMgr, &fi))
#endif
         {
            bInitEnum = TRUE;
            if(MG_DOCTYPE_DIR == eEnumType)
            {
               pFolderName = IVector_ElementAt(pFolderList, 0);
               FREEIF(pFolderName);
               IVector_RemoveElementAt(pFolderList, 0);
            }

            eEnumType = (MG_DOCTYPE_FILE == eEnumType) ?
               MG_DOCTYPE_DIR : MG_DOCTYPE_FILE;
         }
         else
         {
            if(fi.attrib == AEE_FA_DIR)
            {
               pMe->m_pFolderInfo->nSubDirsNum++;
               pFolderName = (NodeName*)MALLOC(sizeof(NodeName));
               if(NULL == pFolderName)
               {
                  MG_FARF(ADDR, ("No memory"));
                  bFinish = TRUE;
                  goto EXITGETFOLDERINFO;
               }
               MEMSET(pFolderName, 0, sizeof(NodeName));
#ifdef FEATURE_MG_LONGPATH
               MEMCPY(pFolderName->m_szNodeName,
                      sFileName.m_szNodeName,
                      sizeof(pFolderName->m_szNodeName));
#else
               MEMCPY(pFolderName->m_szNodeName,
                      fi.szName,
                      sizeof(pFolderName->m_szNodeName));
#endif
               IVector_AddElement(pFolderList, (void*)pFolderName);
            }
            else
            {
               pMe->m_pFolderInfo->nFilesNum++;
               pMe->m_pFolderInfo->lDirSize += fi.dwSize;
            }
            nCount++;
         }
   }

EXITGETFOLDERINFO:
   if(bFinish == TRUE)
   {
      pMe->m_Explorer.m_eType = MG_DOCTYPE_MAX;
      MediaGalleryApp_SetCallbackStateDone(pMe);
   }
   else
   {
      pMe->m_Explorer.m_eType = eEnumType;
      ISHELL_Resume(pMe->m_pShell, &pMe->m_CallBack);
   }
   MG_FARF(ADDR, ("DIR NUM %d, FILES NUM %d, SIZE %d",
                  pMe->m_pFolderInfo->nSubDirsNum,
                  pMe->m_pFolderInfo->nFilesNum,
                  pMe->m_pFolderInfo->lDirSize));
   if(pMe->m_pfnNotify)
      pMe->m_pfnNotify(pMe);
}//MGExplorer_GetFolderInfo


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_MediaMenuSort
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGExplorer_MediaMenuSort(CMediaGalleryApp *pMe,
                              IMenuCtl *pMenuCtl,
                              MGSortOrder eOrder)
{
   if(MG_SORT_NONE == eOrder)
      return SUCCESS;

   if(NULL == pMe ||NULL == pMenuCtl)
      return EFAILED;

   IMENUCTL_SortCustom(pMenuCtl,
                       MGExplorer_MediaMenuSortCompare,
                       pMe);

   /*after sort , menu lose focus, reset it*/
   MGExplorer_SetMediaMenuSelItem(&pMe->m_Explorer, pMenuCtl) ;

   /*And need update menu title, for the index of select itme may change*/
   MGExplorer_UpdateMediaMenuTitle(pMe, pMenuCtl);

   return SUCCESS;
}//MGExplorer_MediaMenuSort

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_CompareFilename
 * DESCRIPTION  :  compare two file name when sort media menu
 * PARAMETERS   :
 * RETURN VALUE :
 *   -1:  pItemData1 less than   pItemData2
 *   0 :  pItemData1 equal than  pItemData2
 *   1 :  pItemData1 greate than pItemData2
 * ==========================================================================
 */
static int MGExplorer_CompareFilename(CMediaGalleryApp *pMe,
      MGFileInfo *pItemData1 ,
      MGFileInfo *pItemData2)
{
   boolean bSysFolder;
   MGMimeType     eMimeBase;
   char *pszBasename;
   uint8  nDepth;
   uint8  nMedium;
   AECHAR FileName1[MG_MAX_FILE_NAME/2];
   AECHAR FileName2[MG_MAX_FILE_NAME/2];

   if(!pMe || !pItemData1 || !pItemData2)
   {
      MG_FARF(ADDR, ("Bad parameter"));
      return 0;
   }
   nMedium = pMe->m_StoreMedium;

   eMimeBase = MediaGalleryApp_GetExplorerMime(pMe);
   nDepth = MediaGalleryApp_GetCurrentDepth(pMe);

   /*check whethe the folder create by Media gallery, or Media gallery
    * specific folder*/
   bSysFolder = FALSE;
   if(pItemData1->attrib == AEE_FA_DIR)
   {
      if(nDepth == MG_CURDEPTH_INIT && nMedium == MG_STMED_MASSCARD)
      {
         bSysFolder = MGExplorer_GetSysDirStr(pMe->m_pShell,
                                              pItemData1->szName,
                                              FileName1,
                                              sizeof(FileName1));
      }
   }

   if(FALSE == bSysFolder)
   {
      pszBasename = BASENAME(pItemData1->szName);
      UTF8TOWSTR((const byte*)pszBasename,
                 STRLEN(pszBasename)+1,
                 FileName1,
                 sizeof(FileName1));
   }

   bSysFolder = FALSE;
   if(pItemData2->attrib == AEE_FA_DIR)
   {
      if(nDepth == MG_CURDEPTH_INIT && nMedium == MG_STMED_MASSCARD)
      {
         bSysFolder = MGExplorer_GetSysDirStr(pMe->m_pShell,
                                              pItemData2->szName,
                                              FileName2,
                                              sizeof(FileName2));
      }
   }

   if(FALSE == bSysFolder)
   {
      pszBasename = BASENAME(pItemData2->szName);
      UTF8TOWSTR((const byte*)pszBasename,
                 STRLEN(pszBasename)+1,
                 FileName2,
                 sizeof(FileName2));
   }

   return Appscommon_CompareName(FileName1, FileName2);
}//MGExplorer_CompareFilename


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_CompareFileType
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static int MGExplorer_CompareFileType(CMediaGalleryApp *pMe,
      MGFileInfo *pItemData1 ,
      MGFileInfo *pItemData2)
{
    const char *pszBasename = NULL;
    const char *pszFile1Ext = NULL;
    const char *pszFile2Ext = NULL;

   if(!pMe || !pItemData1 || !pItemData2)
   {
      MG_FARF(ADDR, ("Bad parameter"));
      return 0;
   }

   pszBasename = BASENAME(pItemData1->szName);
   if(NULL == pszBasename)
      pszBasename = pItemData1->szName;
   pszFile1Ext = STRRCHR(pszBasename, MG_DOT_CHAR);
   if(NULL == pszFile1Ext)
      ;

   pszBasename = BASENAME(pItemData2->szName);
   if(NULL == pszBasename)
      pszBasename = pItemData2->szName;
   pszFile2Ext = STRRCHR(pszBasename, MG_DOT_CHAR);
   if(NULL == pszFile2Ext)
      ;

   if(NULL == pszFile1Ext || NULL == pszFile2Ext)
   {
      if(NULL == pszFile1Ext && NULL == pszFile2Ext)
         return 0;
      else if(NULL == pszFile2Ext)
         return 1;
      else
         return -1;
   }
   else
   {
      return STRCMP(pszFile1Ext, pszFile2Ext);
   }

   return 0;
}



/*===========================================================================
 * FUNCTION:MGExplorer_MediaMenuSortCompare
 *
 * DESCRIPTION: CALL BACK function for menu sort. when mix various compare
 * type, name is the lowest, for example, if compare two foler by file size,
 * we will compare by their name accutally, not size, as size of folder always
 * is 0, in fact , Windwos Xp do this way.
 *
 * PARAMETERS:
 *   -1:  Data1 less than Data2
 *   0 :  Data1 equal than Data2
 *   1 :  Data1 greate than Data2
 * ==========================================================================
 */
static int MGExplorer_MediaMenuSortCompare(void * pUser,
                                             uint32 dwData1,
                                             uint32 dwData2)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)pUser;
   MGFileInfo *pItemData1 = (MGFileInfo *)dwData1;
   MGFileInfo *pItemData2 = (MGFileInfo *)dwData2;
   int nRet = 0;
   char cattrib1;
   char cattrib2;
   MGSortOrder eSortOrder = pMe->m_Explorer.m_eSortOrder;

   if(!pMe || !pItemData1 || !pItemData2)
   {
      MG_FARF(ADDR, ("MediaMenuSortCompare bad parameter! ret%d", nRet));
      return nRet;
   }

   cattrib1 = pItemData1->attrib & MG_FILEATTR_MASK;//(!(AEE_FA_FIXED | AEE_FA_CONST));
   cattrib2 = pItemData2->attrib & MG_FILEATTR_MASK;//(!(AEE_FA_FIXED | AEE_FA_CONST));

   if (cattrib1 != cattrib2)
   {
      nRet = (cattrib1 == AEE_FA_DIR ? -1 : 1);
   }
   else if (pItemData1->attrib != pItemData2->attrib)
   {
      nRet = (pItemData1->attrib > pItemData2->attrib) ? -1 : 1;
   }
   else//if(cattrib1 == cattrib2 && pItemData1->attrib == pItemData2->attrib)
   {
      /*we do ascend sort by default.*/
      switch(eSortOrder){
      case MG_SORT_NAME:
         nRet = MGExplorer_CompareFilename(pMe, pItemData1, pItemData2);
         break;

      case MG_SORT_DATE:
         //bug fix : change "<" to "==", other this code cause handset
         //restart automaticly and crash.
         if(pItemData1->dwCreationDate == pItemData2->dwCreationDate)
         {
            nRet = MGExplorer_CompareFilename(pMe, pItemData1, pItemData2);
         }
         else
         {
            /*new to old, new's time is big than old's, so big to -1, while
             * small to 1*/
            nRet = (pItemData1->dwCreationDate < pItemData2->dwCreationDate ?
                    1 : -1);
         }
         break;

      case MG_SORT_SIZE:
         if(pItemData1->dwSize == pItemData2->dwSize)
         {
            nRet = MGExplorer_CompareFilename(pMe, pItemData1, pItemData2);
         }
         else
         {
            nRet = pItemData1->dwSize < pItemData2->dwSize ? -1 : 1;
         }
         break;

      case MG_SORT_TYPE:
         //compare file extension, if file extension is not exist, get
         //mime, and contert to extension.then file name.
         if(cattrib1 != AEE_FA_DIR)
         {
            nRet = MGExplorer_CompareFileType(pMe, pItemData1, pItemData2);
         }

         if(cattrib1 == AEE_FA_DIR || nRet == 0)
         {
            nRet = MGExplorer_CompareFilename(pMe, pItemData1, pItemData2);
         }
         break;

      case MG_SORT_NONE:
      default:

         /*data1 less data2, that is the files enumerate.
          * NOTE: it is very odd, if we just return -1, code swith
          * to MG_SORT_SIZE, cause the phone crash.
          * WARNING: It shoud be a bug for QSORT, You can not always return
          * -1 to it.*/
         break;
      }
   }

   MG_FARF(ADDR, ("MediaMenuSortCompare RETURN:%d", nRet));
   return nRet;
}//MGExplorer_MediaMenuSortCompare


/*===========================================================================
 * FUNCTION: MGExplorer_GetMediaMenuSelectItem
 *
 * DESCRIPTION: get the select item of media menu
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
void MGExplorer_GetMediaMenuSelectItem(CFSExplorer *pFSExplorer,
                                       IMenuCtl* pMenuCtl)
{
   uint16 *pSelItemID = NULL;
   uint8 nDirDepth;
   boolean bGet;

   if(NULL == pFSExplorer || NULL == pMenuCtl)
      return ;

   nDirDepth = MGExplorer_GetCurrentDepth(pFSExplorer);

   if(TRUE == MGExplorer_CheckMediaMenuEmpty(pMenuCtl))
   {
      MG_FARF(ADDR, ("MENU have no item, set current node to NULL"));
      pFSExplorer->m_pCurNode = NULL;
      return;
   }

   if(nDirDepth >= MG_CURDEPTH_INIT &&
         nDirDepth < MG_CURDEPTH_INIT + MG_MAX_SUBDIR_LEVEL)
   {
      pSelItemID = pFSExplorer->m_nPreItemID + (nDirDepth - 1);
   }
   else
   {
      MG_FARF(ADDR, ("explorer folder depth error!"));
      return;
   }

   if(pSelItemID)
   {
      *pSelItemID =  IMENUCTL_GetSel(pMenuCtl);

      bGet = IMENUCTL_GetItemData(pMenuCtl,
                                  *pSelItemID,
                                  (uint32*)&pFSExplorer->m_pCurNode);

      MG_FARF(ADDR, ("Current Node addr is 0x%x", pFSExplorer->m_pCurNode));

      if(bGet == FALSE)
      {
         MG_FARF(ADDR, ("IMENUCTL_GetItemData Failed!"));
      }
   }
}//MGAppUtil_GetMenuCtlItem


/*===========================================================================
 * FUNCTION: MGExplorer_SetMediaMenuSelItem
 *
 * DESCRIPTION: set the select item of media menu
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
boolean MGExplorer_SetMediaMenuSelItem(CFSExplorer *pFSExplorer,
                                       IMenuCtl *pMenuCtl)
{
   uint16 *pSelItemID = NULL;
   uint8 nDirDepth;

   if(NULL == pFSExplorer || NULL == pMenuCtl)
   {
      return FALSE;
   }

   if(TRUE == MGExplorer_CheckMediaMenuEmpty(pMenuCtl))
   {
      MG_FARF(ADDR, ("Empty media menu!"));
      return FALSE;
   }

   nDirDepth = MGExplorer_GetCurrentDepth(pFSExplorer);

   if(nDirDepth >= MG_CURDEPTH_INIT &&
         nDirDepth < MG_CURDEPTH_INIT + MG_MAX_SUBDIR_LEVEL)
   {
      pSelItemID = pFSExplorer->m_nPreItemID + (nDirDepth - 1);
   }
   else
   {
      MG_FARF(ADDR, ("explorer folder depth error!"));
      return FALSE;
   }

   if(*pSelItemID != MG_MENUITEM_NULL)
   {
      CtlAddItem  ai;
      uint32 dwProperties;
      boolean bMulSel = TRUE; /*whether the menu control is multi-select*/

      /*check the item id whether exist*/
      if(FALSE ==IMENUCTL_GetItem(pMenuCtl, *pSelItemID, &ai))
      {
         return FALSE;
      }

      dwProperties = IMENUCTL_GetProperties(pMenuCtl);
      bMulSel = (MP_MULTI_SEL & dwProperties) ? TRUE : FALSE;

      if(TRUE == bMulSel)//multi-selection
      {
         /*check whether have set the previous item*/
         if(*pSelItemID != IMENUCTL_GetFocus(pMenuCtl))
         {
            IMENUCTL_SetFocus(pMenuCtl, *pSelItemID);
            return TRUE;
         }
      }
      else//single-selection
      {
         /*check whether have set the previous item*/
         if(*pSelItemID != IMENUCTL_GetSel(pMenuCtl))
         {
            IMENUCTL_SetSel(pMenuCtl, *pSelItemID);
            return TRUE;
         }
      }
   }

   return FALSE;
}//MGExplorer_SetMediaMenuSelItem

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_RecoverMediaMenu
 * DESCRIPTION  :  查看图片时，翻看后/前一张图片，这样可能当前图片对应的文件不
 * 是Media menu的focus项。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGExplorer_RecoverMediaMenu(CMediaGalleryApp *pMe, IMenuCtl *pMenuCtl)
{
   if(!pMe || !pMenuCtl)
      return EFAILED;

   /*TODO: after view image, it will change focus item*/

   MGExplorer_SetMediaMenuSelItem(&pMe->m_Explorer, pMenuCtl) ;
   MGExplorer_UpdateMediaMenuTitle(pMe, pMenuCtl);

   return SUCCESS;
}//MGExplorer_RecoverMediaMenu

/*===========================================================================
 * FUNCTION: MGExplorer_ResetPreItemID
 *
 * DESCRIPTION: set element in array m_nPreItemID of CFSExplorer to
 * MG_MENUITEM_NULL.
 *
 * PARAMETERS: bAll : TRUE , reset all element
 *                    FALSE, only reset current element.
 *
 * RETURN:
 * ==========================================================================
 */
boolean MGExplorer_ResetPreItemID(CFSExplorer *pFSExplorer, boolean bAll)
{
   if(!pFSExplorer)
   {
      MG_FARF(ADDR, ("Bad parameter"));
      return FALSE;
   }

   if(bAll)
   {
      MEMSET(pFSExplorer->m_nPreItemID,
            0xFF,
            sizeof(pFSExplorer->m_nPreItemID));

   }
   else
   {
      uint16 *pSelItemID = NULL;

      if(pFSExplorer->m_nCurDepth >= MG_CURDEPTH_INIT &&
            pFSExplorer->m_nCurDepth < MG_CURDEPTH_INIT + MG_MAX_SUBDIR_LEVEL)
      {
         pSelItemID = pFSExplorer->m_nPreItemID +
            (pFSExplorer->m_nCurDepth - 1);
         *pSelItemID = MG_MENUITEM_NULL;
      }
      else
      {
         MG_FARF(ADDR, ("m_nCurDepth is error!!!"));
         return FALSE;
      }
   }

   return TRUE;
}//MGExplorer_ResetPreItemID

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  MGExplorer_UpdateMediaMenuTitle
 *  Description:  draw title of media menu
 * =====================================================================================
 */
__inline int MGExplorer_UpdateMediaMenuTitle(CMediaGalleryApp *pMe,
                                             IMenuCtl *pMenuCtl)
{
   AECHAR *wpszTitle = NULL;
   char *pszNewTitle = NULL;
   MGFileInfo *pCurDir;
   int nItemCount;
   int nSelIdx;
   int nRet = SUCCESS;
   const char *cpszCountFormat = "(%d/%d)";
   char *pszCount;
   uint16 nLen = STRLEN(cpszCountFormat) + 11;
   uint16 nTitleLen = MG_MAX_FILE_NAME + nLen;
   int16 nResID = 0;
   boolean bSysFolder = FALSE;
   uint8  nDepth;
   uint8  nMedium;
   MGStartMode     eStartMode;
   MGMimeType     eMimeBase;
   AECHAR m_Annstr[40] = {0};	

   MSG_FATAL("MGExplorer_UpdateMediaMenuTitle Start",0,0,0);
   if(!pMe || !pMenuCtl){
      return EFAILED;
   }

   eStartMode = pMe->m_StartMode;
   nMedium = pMe->m_StoreMedium;
   eMimeBase = MediaGalleryApp_GetExplorerMime(pMe);
   nDepth = MediaGalleryApp_GetCurrentDepth(pMe);

   wpszTitle = MALLOC(sizeof(AECHAR) * (nTitleLen));
   pszNewTitle = MALLOC(sizeof(char) * (nTitleLen));
   pszCount = MALLOC(sizeof(char) * nLen);

   if(wpszTitle == NULL || pszNewTitle == NULL || pszCount == NULL)
   {
      MG_FARF(ADDR, ("Memory insufficient"));
      nRet = EFAILED;
      goto UPDATETITLEEXIT;
   }

   MEMSET(wpszTitle, 0, sizeof(AECHAR) * nTitleLen);
   MEMSET(pszNewTitle, 0, sizeof(char) * nTitleLen);
   MEMSET(pszCount, 0, sizeof(char) * nLen);

   nItemCount = IMENUCTL_GetItemCount(pMenuCtl);

   if(nItemCount > 0)
   {
      if(FALSE ==MGExplorer_GetSelItemIndex(pMenuCtl, &nSelIdx))
      {
         nRet = EFAILED;
         goto UPDATETITLEEXIT;
      }
      SNPRINTF(pszCount, sizeof(char) * nLen, cpszCountFormat,
               nSelIdx + 1, nItemCount);

	  DBGPRINTF("***zzg MediaMenuTitle pszCount=%s***", pszCount);
   }

   pCurDir = MGExplorer_GetCurrentFolder(&pMe->m_Explorer);

   if(pCurDir->attrib == AEE_FA_DIR){
      if(nDepth == MG_CURDEPTH_INIT)
      {
         if(nMedium == MG_STMED_MASSCARD)
         {
            if( eStartMode == MGSM_MUSIC_ADD)
               nResID = IDS_MG_MUSIC;
            else if(eStartMode == MGSM_PHOTO_BROWSE)
               nResID = IDS_MG_PHOTOGALLERY;
            else if(eStartMode == MGSM_VIDEO_BROWSE)
               nResID = IDS_MG_VIDEOGALLERY;
            else if(eStartMode == MGSM_VIDEO_ADD)
               nResID = IDS_MG_VIDEOS;
            else if(eStartMode == MGSM_IMAGE_SETTING)
            {
               MSG_FATAL("eStartMode == MGSM_IMAGE_SETTING",0,0,0);
               nResID = IDS_MG_PICTURES;
            }
            else if(eStartMode == MGSM_MUSIC_SETTING)
               nResID = IDS_MG_MUSIC;
            else
               nResID = IDS_MG_CARDMEMORY;

            bSysFolder = TRUE;

            ISHELL_LoadResString(pMe->m_pShell,
                                 MGRES_LANGFILE,
                                 nResID,
                                 wpszTitle,
                                 sizeof(AECHAR) * nTitleLen);
         }
         else if(nMedium == MG_STMED_HANDSET){
            bSysFolder = MGExplorer_GetSysDirStr(pMe->m_pShell,
                                                 pCurDir->szName,
                                                 wpszTitle,
                                                 (uint16)(sizeof(AECHAR) * nTitleLen));
         }
      }
      else if(nDepth == MG_CURDEPTH_INIT + 1 &&
              nMedium == MG_STMED_MASSCARD)
      {
         bSysFolder = MGExplorer_GetSysDirStr(pMe->m_pShell,
                                              pCurDir->szName,
                                              wpszTitle,
                                              (uint16)(sizeof(AECHAR) * nTitleLen));
      }
   }

   if(FALSE == bSysFolder)
   {
      STRCPY(pszNewTitle, BASENAME(pCurDir->szName));
      STRCAT(pszNewTitle, pszCount);

      UTF8TOWSTR((const byte *)pszNewTitle,
                 sizeof(char) * nTitleLen,
                 wpszTitle,
                 sizeof(AECHAR) * nTitleLen);
   }
   else
   {
      AECHAR wszBuffer[MG_MAX_LINETEXTNUM];

	  DBGPRINTF("***zzg MediaMenuTitle pszCount=%s, nLen=%d", pszCount, nLen); 
	  
      UTF8TOWSTR((const byte *)pszCount,
                 sizeof(char) * nLen,
                 wszBuffer,
                 sizeof(wszBuffer));
      WSTRCAT(wpszTitle, wszBuffer);

   }
   WSTRNCOPY(m_Annstr,40,wpszTitle);	//20
   IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,m_Annstr);
   //IMENUCTL_SetTitle(pMenuCtl, NULL, 0, wpszTitle);

UPDATETITLEEXIT:
   FREEIF(wpszTitle);
   FREEIF(pszNewTitle);
   FREEIF(pszCount);

   return nRet;
}//MGExplorer_UpdateMediaMenuTitle

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_UpdateCurrentMediaMenuItem
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGExplorer_UpdateCurrentMediaMenuItem(CMediaGalleryApp *pMe,
                                                   const char *pNewFilePath)
{
   IMenuCtl *pMenuCtl;
   MGFileInfo *pCurNode;
   CtlAddItem MenuItem = {0};
   AECHAR FileName[MG_MAX_FILE_NAME];
   char* pszBasename;
   int nBasenameLen;
   uint16 nItemID;
   if(!pMe || !pNewFilePath)
      return EFAILED;

   pMenuCtl = pMe->m_pMediaMenu;
   pCurNode = MediaGalleryApp_GetCurrentNode(pMe);

   if(!pCurNode || !pMenuCtl)
      return EFAILED;

   pszBasename = BASENAME(pNewFilePath);

   if(0 == (nBasenameLen = STRLEN(pszBasename)))
   {
      return EFAILED;
   }

   UTF8TOWSTR((const byte*)pszBasename,
         STRLEN(pszBasename)+1,
         FileName,
         sizeof(FileName));

   MenuItem.pText = FileName;

   nItemID = IMENUCTL_GetSel(pMenuCtl);
   IMENUCTL_SetItem(pMenuCtl, nItemID, MSIF_TEXT, &MenuItem);

   STRCPY(pCurNode->szName, pNewFilePath);
   return SUCCESS;
}//MGExplorer_UpdateCurrentMediaMenuItem

/*===========================================================================
 * FUNCTION: MGExplorer_FreeMediaMenuItem
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
int MGExplorer_FreeMediaMenuItem(IMenuCtl *pMediaMenu)
{
   uint16   uItemID;
   int      nCount;
   int      nIdx;
   MGFileInfo *pItemData = NULL;

   if(!pMediaMenu)
   {
      MG_FARF(ADDR, ("FreeMediaMenuItem bad parameter!"));
      return EFAILED;
   }

   nCount = IMENUCTL_GetItemCount(pMediaMenu);

   if(nCount == 0)
   {
      MG_FARF(ADDR, ("FreeMediaMenuItem no item!"));
      return SUCCESS;
   }

   for(nIdx = nCount - 1; nIdx >= 0 ; nIdx--)
   {
      //It shoud be nIdx+MG_EXPLORER_GENERALITEM
      uItemID = IMENUCTL_GetItemID(pMediaMenu, nIdx);
      IMENUCTL_GetItemData(pMediaMenu, uItemID, (uint32*)&pItemData);
      FREEIF(pItemData);
   }

   /*
    * do not use IMENUCTL_DeleteItem(pMediaMenu, uItemID); it will be do
    * much more thing, and result in very slowly. We also can not delete
    * asynchronous, because IMENUCTL_DeleteItem will redraw. What we can do is
    * free item data, and set item data to null, it seem that can not improve
    * efficient.
    */
   IMENUCTL_DeleteAll(pMediaMenu);

	return SUCCESS;
}//MGExplorer_FreeMediaMenuItem

/*===========================================================================
 * FUNCTION: MGExplorer_FreeVectorElements
 *
 * DESCRIPTION: free a vector, for a vector holde pointers to dynamic memory,
 * we need free them by ourself
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
int MGExplorer_FreeVectorElements(IVector* pList)
{
   uint32 nSize;
   uint32 nIndex;
   void* pObject;

   if(!pList)
      return EBADPARM;

   nSize = IVector_Size(pList);

   for(nIndex = 0; nIndex < nSize; nIndex++)
   {
      pObject = IVector_ElementAt(pList, nIndex);
      FREEIF(pObject);
   }

   IVector_RemoveAllElements(pList);
  return SUCCESS;
}//MGExplorer_FreeNodeNameVector

/*===========================================================================
 * FUNCTION:    MGExplorer_RemovDir
 * DESCRIPTION: For IFILEMGR_RmDir, if there are files or directories elements
 * beneath the directory to be removed, they should be removed prior to
 * calling this function, or this function call fails and the directory is not
 * removed.
 *
 * But for MGExplorer, the folder is just a node, can remove as general file.
 *
 * RETURN VALUE:
 *    SUCCESS:
 *    EFAILED:
 *===========================================================================
 */
int MGExplorer_RemovDir(IFileMgr *pIFlMr,
                        IVector *pIVec,
                        const char* pszDir,
                        boolean bDelRootDir)
{
   IFileMgr *pIFileMgr;
   IVector *pFolderList;
   NodeName *pFolderName;
   uint32 nSize;
   uint32 nIndex = 0;
   uint32 nMin;
   int nRet = SUCCESS;
#ifdef FEATURE_MG_LONGPATH
   AEEFileInfoEx fi;
   NodeName sFileName;
#else
   MGFileInfo fi;
#endif

   if(NULL == pszDir)
      return EFAILED;

   pIFileMgr = pIFlMr;
   pFolderList = pIVec;

   if(NULL == pIFileMgr || NULL == pFolderList )
   {
      nRet = EFAILED;
      goto RMDIREXIT;
   }

   if(SUCCESS != MGExplorer_EnumFoldersList(pIFileMgr,
                                            pszDir,
                                            pFolderList,
                                            TRUE))
   {
      MG_FARF(STATE, ("Enum folder failed!"));
      nRet = EFAILED;
      goto RMDIREXIT;
   }

   MEMSET(&fi, 0, sizeof(fi));
#ifdef FEATURE_MG_LONGPATH
   fi.nStructSize = sizeof(fi);
   fi.pszFile = sFileName.m_szNodeName;
   fi.nMaxFile = sizeof(sFileName.m_szNodeName);
#endif
   /*delete files*/
   nSize = IVector_Size(pFolderList);

   for(nIndex = 0; nIndex < nSize; nIndex++)
   {
      pFolderName = IVector_ElementAt(pFolderList, nIndex);

      if(NULL == pFolderName)
      {
         MG_FARF(ADDR, ("folder name pointer NULL!"));
         goto RMDIREXIT;
      }

      if(SUCCESS == IFILEMGR_EnumInit(pIFileMgr,
                                      pFolderName->m_szNodeName,
                                      FALSE))
      {
#ifdef FEATURE_MG_LONGPATH
         MEMSET(fi.pszFile, 0, fi.nMaxFile);
         while(IFILEMGR_EnumNextEx(pIFileMgr, &fi))
         {
            nRet = IFILEMGR_Remove(pIFileMgr, fi.pszFile);
         }
#else
         while(IFILEMGR_EnumNext(pIFileMgr, &fi))
         {
            nRet = IFILEMGR_Remove(pIFileMgr, fi.szName);
         }
#endif
      }
   }

   /*delete empty folder, do backward, otherwise empty subfolder can not been delete*/
   nMin = (uint32)(bDelRootDir == TRUE ? 0 : 1);
   for(nIndex = nSize - 1; nIndex >= nMin; nIndex--)
   {
      pFolderName = IVector_ElementAt(pFolderList, nIndex);
      //MGExplorer_IsSysDirInMassStorage
      nRet = IFILEMGR_RmDir(pIFileMgr, pFolderName->m_szNodeName);

      if(nIndex == 0)
      {
         break;
      }
   }

RMDIREXIT:
   MGExplorer_FreeVectorElements(pFolderList);
   return nRet;
}//MGExplorer_RemovDir

/*===========================================================================
 * FUNCTION: MGExplorer_ChangeCurDir
 * DESCRIPTION:  change the current directory hold by CFSExplorer variable
 * PARAMETERS: eType : TRUE -- go to parent folder
 *                      FALSE -- go to sub-folder
 * RETURN VALUE:
 *    SUCCESS:
 *    EFAILED:
 *===========================================================================
 */
int MGExplorer_ChangeCurDir(CFSExplorer* pFSExplorer,
                                 MGDirChange eType,
                                 const char *pszFilePath)
{
   char *pszTemp;
   MGFileInfo *pCurDir;

   if(!pFSExplorer || !pszFilePath)
   {
      MG_FARF(ADDR, ("ChangeCurDir bad parameter!"));
      return EFAILED;
   }

   pCurDir = MGExplorer_GetCurrentFolder(pFSExplorer);

   if(!pCurDir->szName || STRLEN(pCurDir->szName) == 0){
      return EFAILED;
   }

   if((MG_DIR_GOUP == eType) && (pFSExplorer->m_nCurDepth > MG_CURDEPTH_INIT))
   {
      pszTemp = STRRCHR(pCurDir->szName, MG_DIR_DELIMITER);

      if((NULL != pszTemp) && (0 == STRCMP(pszTemp,
                  pFSExplorer->m_RootNode.szName))){
         pszTemp = STRRCHR(pszTemp, MG_DIR_DELIMITER);
      }

      if(NULL != pszTemp)
      {
         MGExplorer_ResetPreItemID(pFSExplorer, FALSE);
         MEMSET(pszTemp, 0, sizeof(char) * STRLEN(pszTemp));
         *pszTemp = '\0';
         pFSExplorer->m_nCurDepth--;
      }
      else
      {
         return EFAILED;
      }
   }
   else if((MG_DIR_GODOWN == eType) && (NULL != pszFilePath) &&
      (pFSExplorer->m_nCurDepth + 1 < MG_CURDEPTH_INIT+ MG_MAX_SUBDIR_LEVEL))
   {
      pszTemp = STRSTR(pszFilePath, pCurDir->szName);

      if(pszTemp)
      {
         char* pszSave;

         pszTemp = STRDUP(pszFilePath);

         if(!pszTemp)
         {
            return EFAILED;
         }

         pszSave = pszTemp;

         pszTemp += STRLEN(pCurDir->szName);

         strtok(pszTemp, "/");
         STRCAT(pCurDir->szName, pszTemp);

         pFSExplorer->m_nCurDepth++;
         FREE(pszSave);
      }
      else
      {
         return EFAILED;
      }
   }
   else
   {
      return ERROR_DEEPDIR;
   }
   return SUCCESS;
}//MGExplorer_ChangeCurDir

/*===========================================================================
 * FUNCTION: MGExplorer_CheckMediaMenuEmpty
 *
 * DESCRIPTION: find out whether the menu is empty, has no item
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
boolean MGExplorer_CheckMediaMenuEmpty(IMenuCtl *pMenuCtl)
{
   if(!pMenuCtl)
   {
      MG_FARF(ADDR, ("CheckMediaMenuEmpty bad parameter"));
      return FALSE;
   }

   if(0 == IMENUCTL_GetItemCount(pMenuCtl))
   {
      return TRUE;
   }

   return FALSE;
}//MGExplorer_CheckMediaMenuEmpty


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_GetSelItemIndex
 * DESCRIPTION  :  get the select item index of the menu control
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
__inline boolean MGExplorer_GetSelItemIndex(IMenuCtl *pm, int *pnIdx)
{
   int nItemCount;
   int nSelIdx;
   uint16 nSelItemID;

   if(!pm || !pnIdx)
      return FALSE;

   nSelItemID = IMENUCTL_GetSel(pm);
   nItemCount = IMENUCTL_GetItemCount(pm);

   for(nSelIdx = 0; nSelIdx < nItemCount; nSelIdx++){
      if(nSelItemID == IMENUCTL_GetItemID(pm, nSelIdx)){
         *pnIdx = nSelIdx;
         return TRUE;
      }
   }

   return FALSE;
}//MGExplorer_GetSelItemIndex


/*===========================================================================
 * FUNCTION: MGExplorer_CheckFolderExist
 *
 * DESCRIPTION: check the default folder is exist
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
boolean MGExplorer_CheckFolderExist(IFileMgr* pFileMgr,
                                    const char** ppFolders,
                                    uint32 nNum,
                                    boolean bMkdir)
{
   uint32 i;

   if(!pFileMgr || !ppFolders)
   {
      return FALSE;
   }

   for(i = 0; i < nNum; i++)
   {
      if(!*(ppFolders+i))
      {
         continue;
      }

      if(EFAILED == IFILEMGR_Test(pFileMgr, *(ppFolders + i)))
      {
         MG_FARF(ADDR, ("File %s is not exist", *(ppFolders + i)));

         if(FALSE == bMkdir)
         {
            return FALSE;
         }

         if(SUCCESS != IFILEMGR_MkDir(pFileMgr, *(ppFolders + i)))
         {
            return FALSE;
         }
      }
   }

   return TRUE;
}//MediaGalleryApp_CheckFolderExist


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_GetSysDirStr
 * DESCRIPTION  :  获取我们OEM特定目录对应的字符串。此函数不检查全路径，只是根
 * 据文件夹名来获取字符串，所以调用者要确保传进来的文件夹所在路径是OEM指定的路
 * 径。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MGExplorer_GetSysDirStr(IShell *pIShell,
                                       char *pszFile,
                                       AECHAR *pwszFolder,
                                       uint16 nByte)
{
   const char *pBaseName = NULL;
   char *pszFileEnd = NULL;
   uint16 nFileNameLen = 0;
   int16 nResID = 0;
   int nRet = SUCCESS;
//   boolean bSlashEnd = FALSE;

   if(NULL == pIShell || NULL == pszFile || NULL == pwszFolder || 0 == nByte)
      return FALSE;

   pBaseName = BASENAME(pszFile);

   if(NULL == pBaseName)
   {
      return FALSE;
   }
   else if(0 == STRLEN(pBaseName))
   {
      nFileNameLen = STRLEN(pszFile);
      pszFileEnd = pszFile + nFileNameLen - 1;

      if (0 == nFileNameLen)
      {
         return FALSE;
      }
      else if (*pszFileEnd == MG_DIR_DELIMITER)
      {
         *pszFileEnd = '\0';
         pBaseName = BASENAME(pszFile);

         if(NULL == pBaseName || 0 == STRLEN(pBaseName))
         {
            return FALSE;
         }
         //bSlashEnd = TRUE;
      }
   }

   if(0 == STRCMP(pBaseName, MG_PICTURES_FOLDER))
   {
      nResID = IDS_MG_PICTURES;
   }
   else if(0 == STRCMP(pBaseName, MG_MUSIC_FOLDER))
   {
      nResID = IDS_MG_MUSIC;
   }
   else if(0 == STRCMP(pBaseName, MG_VIDEOS_FOLDER))
   {
      nResID = IDS_MG_VIDEOS;
   }
   else if(0 == STRCMP(pBaseName, MG_VOICEMEMO_FOLDER))
   {
      nResID = IDS_MG_VOICEMEMO;
   }
   else if(0 == STRCMP(pBaseName, MG_CALLMEMO_FOLDER))
   {
      nResID = IDS_MG_CALLMEMO;
   }
#ifdef FEATURE_VERSION_C316
   else if(0 == STRCMP(pBaseName, MG_AUTOCALLRECOED_FOLDER))
   {
      nResID = IDS_MG_AUTOCALLRECORD;
   }
#endif   
#ifdef FEATURE_MG_WAPMMS_SUPPORT
   else if(0 == STRCMP(pBaseName, MG_DOWNLOAD_FOLDER))
   {
      nResID = IDS_MG_DOWNLOAD;
   }
#endif

//   if(bSlashEnd == TRUE)
//   {
//      *pszFileEnd = MG_DIR_DELIMITER;
//   }

   if(nResID == 0)
   {
      return FALSE;
   }

   nRet = ISHELL_LoadResString(pIShell,
                               MGRES_LANGFILE,
                               nResID,
                               pwszFolder,
                               nByte);
   /*不清楚，在返回非SUCCESS，但实际还是成功的*/
   return TRUE;
}//MGExplorer_GetSysDirStr


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  MGExplorer_IsSysDirInMassStorge
 *  Description:  check on the folder whether is we created ,default folder.
 * =====================================================================================
 */
boolean MGExplorer_IsSysDirInMassStorage(const char *pszFile,
                                          uint8 nStoreMedium)
{
   boolean bSysDir = FALSE;
   const char *pBaseName = NULL;

   if(!pszFile)
      return FALSE;

   if(nStoreMedium == MG_STMED_MASSCARD)
   {
      pBaseName = BASENAME(pszFile);

      if(!pBaseName)
         return FALSE;

      if(0 == STRCMP(pBaseName, MG_PICTURES_FOLDER)){
         bSysDir = TRUE;
      }
      else if(0 == STRCMP(pBaseName, MG_MUSIC_FOLDER)){
         bSysDir = TRUE;
      }
      else if(0 == STRCMP(pBaseName, MG_VIDEOS_FOLDER)){
         bSysDir = TRUE;
      }
      else if(0 == STRCMP(pBaseName, MG_VOICEMEMO_FOLDER)){
         bSysDir = TRUE;
      }
#ifdef FEATURE_VERSION_C316
      else if(0 == STRCMP(pBaseName, MG_AUTOCALLRECOED_FOLDER)){
         bSysDir = TRUE;
      }
#endif
#ifdef FEATURE_MG_WAPMMS_SUPPORT
      else if(0 == STRCMP(pBaseName, MG_DOWNLOAD_FOLDER)){
         bSysDir = TRUE;
      }
#endif
   }

   return bSysDir;
}//MGExplorer_IsSysDirInMassStorage


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_InitFileIcon
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
void MGExplorer_InitFileIcon(CFileIcons **ppFileIcons, boolean bAllocate)
{
   CFileIcons *pFileIcons = NULL;

   if(!ppFileIcons)
      return;

   pFileIcons = *ppFileIcons;

   if(!pFileIcons)
   {
      if(bAllocate == FALSE)
         return;
      else
      {
         pFileIcons = (CFileIcons *)MALLOC(sizeof(CFileIcons));
         if(pFileIcons == NULL)
            return;
         *ppFileIcons = pFileIcons;
         MEMSET(pFileIcons, NULL, sizeof(CFileIcons));
      }
   }
}//MGExplorer_InitFileIcon

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_ReleaseFileIcon
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
void MGExplorer_ReleaseFileIcon(CFileIcons *pFileIcons)
{
   if(!pFileIcons)
      return;

   RELEASEIF(pFileIcons->pFolderIcon);
   RELEASEIF(pFileIcons->pUnkownIcon);
   RELEASEIF(pFileIcons->pImgIcon);
   RELEASEIF(pFileIcons->p3GPIcon);
   RELEASEIF(pFileIcons->pMp4Icon);
   RELEASEIF(pFileIcons->pMIDIIcon);
   RELEASEIF(pFileIcons->pMp3Icon);
   RELEASEIF(pFileIcons->pVoiceRecIcon);

}//MGExplorer_ReleaseFileIcon

/*===========================================================================
 * FUNCTION:MGExplorer_GetItemIcon
 * DESCRIPTION: Get IImage poiter according to MIME
 * RETURN VALUE:
 *    SUCCESS: we can return some thing as expect.
 *    EFAILED: Parameter error, or parameter is not valid.
 *===========================================================================
 */
static int MGExplorer_GetItemIcon(CFileIcons *pFileIcons,
      MGMimeType eMime,
      IImage **ppIcon)
{
   int16 nResID = IDI_MG_UNKNOWN;
   IImage **ppFileIcon = NULL;

   if(!ppIcon || !pFileIcons)
   {
      return EFAILED;
   }

   switch(eMime)
   {
      case MG_MIME_VOICEREC:
         nResID = IDI_MG_VOICEREC;
         ppFileIcon = &pFileIcons->pVoiceRecIcon;
         break;

      case MG_MIME_PNG:
      case MG_MIME_BMP:
      case MG_MIME_GIF:
      case MG_MIME_BCI:
      case MG_MIME_JPG:
      case MG_MIME_JPEG:
         nResID = IDI_MG_PICTURE;
         ppFileIcon = &pFileIcons->pImgIcon;
         break;

      case MG_MIME_MIDI:
         nResID = IDI_MG_MIDIICON;
         ppFileIcon = &pFileIcons->pMIDIIcon;
         break;

         //case MG_MIME_WAV:
         //  nResID = IDI_MG_WAVICON;
         //  break;

      case MG_MIME_MP3:
         nResID = IDI_MG_MP3ICON;
         ppFileIcon = &pFileIcons->pMp3Icon;
         break;

         //case MG_MIME_WMA:
         //case MG_MIME_AAC:
         //case MG_MIME_AACPLUS:
         // nResID = IDI_MG_UNKNOWN;
         //   break;
//#ifdef T_QSC1110
      case MG_MIME_3GP:
         nResID = IDI_MG_3GPICON;
         ppFileIcon = &pFileIcons->p3GPIcon;
         break;
//#endif
      case MG_MIME_MP4:
         nResID = IDI_MG_MP4ICON;
         ppFileIcon = &pFileIcons->pMp4Icon;
         break;

      case MG_MIME_FOLDER:
         nResID = IDI_MG_FOLDER;
         ppFileIcon = &pFileIcons->pFolderIcon;
         break;

      default:
         nResID = IDI_MG_UNKNOWN;
         ppFileIcon = &pFileIcons->pUnkownIcon;
         break;
   }

   if(*ppFileIcon == NULL)
   {
	*ppFileIcon = ISHELL_LoadResImage(AEE_GetShell(), MEDIAGALLERY_IMAGE_RES_FILE, nResID);
   }
   *ppIcon = *ppFileIcon;

   if(*ppFileIcon)
   {
      MSG_FATAL("ISHELL_LoadResImage SUCCESS", 0,0,0);
      return SUCCESS;
   }
   else
   {
      MSG_FATAL("ISHELL_LoadResImage EFAILED", 0,0,0);
      return EFAILED;
   }
}//MGExplorer_GetItemIcon


/*===========================================================================
 *  unit convert: convert the space value from byte to KB, MB, even GB, if it
 * is large enough.nUnitLen is the byte count of pszUnit.
 * ==========================================================================
 * */
boolean MGAppUtil_SpaceUnitConvert(IShell *pShell,
                        uint32 MemByte, AECHAR *pszUnit, size_t nUnitLen)
{
   uint32 MemConv = MemByte;
   double dConvertData = (double)MemByte;
   char szConBuf[MG_CONVERT_DATALEN];
   AECHAR wszConBuf[MG_CONVERT_DATALEN];
   AECHAR szUnit[MG_MEMUNIT_STRLENMAX];
   int16 nUnitID[MG_MENUNIT_MAX]={
      IDS_MG_BYTE,
      IDS_MG_KILOBYTE,
      IDS_MG_MEGABYTE,
      IDS_MG_GIGABYTE
   };
   MG_MEMUNIT Units[] = {
      MG_MEMUNIT_BYTE,
      MG_MEMUNIT_KILO,
      MG_MEMUNIT_MEGA,
      MG_MEMUNIT_GIGA,};
   MG_MEMUNIT* pMuTemp = Units;

   if(!pShell || !pszUnit)
   {
      MG_FARF(ADDR, ("Warming: bad parameter!"));
      return FALSE;
   }

   while(MemConv >= MG_CONVERT_CRITICAL)
   {
      MemConv >>= MG_CONVERT_SHIFTBITS;
      dConvertData = FDIV(dConvertData,MG_CONVERT_CRITICAL);
      pMuTemp++;
   }

   if(0 != ISHELL_LoadResString(pShell, MGRES_LANGFILE,
            nUnitID[*pMuTemp], szUnit, sizeof(szUnit)))
   {
      /*       AECHAR szNum[MG_MEMUNIT_STRLENMAX];
       *       double dNum = 0;
       *
       *       dNum += (double)(SubMemConv / 1000);
       *       dNum += MemConv;
       *       MEMSET(szNum, 0, sizeof(szNum));
       *       MEMSET(pszUnit, 0, nUnitLen);
       *       if(TRUE == FLOATTOWSTR(dNum, szNum, sizeof(szNum)))
       *       {
       *          WSPRINTF(pszUnit, nUnitLen, L"%S %S", szNum, szUnit);
       *       }
       *       else
       */
      {
         /*for either SPRINTF nor SNPRINTF SUPPORT %f print format, we have to
          * use sprintf, and then convert string to wide-string.*/
         if(nUnitID[*pMuTemp] != IDS_MG_BYTE)
         {
            snprintf( szConBuf, MG_CONVERT_DATALEN, " %4.3f ", dConvertData);
         }
         else
         {
            snprintf( szConBuf, MG_CONVERT_DATALEN, " %4.0f ", dConvertData);
         }
         STRTOWSTR(szConBuf, wszConBuf, sizeof(wszConBuf));

         WSTRCPY(pszUnit, wszConBuf);
         WSTRCAT(pszUnit, szUnit);
         /*
         if(SubMemConv != 0)
         {
            SubMemConv *= 1000;
            ///there are a little fault: if suMemConv is less then a hundred,
            //it can not display leading zero(s).
            SubMemConv >>= MG_CONVERT_SHIFTBITS;
            WSPRINTF(pszUnit, nUnitLen, L" %d.%3d %S",
                  MemConv, SubMemConv, szUnit);
         }
         else
         {
            WSPRINTF(pszUnit, nUnitLen, L"%S%S",
                  wszConBuf, szUnit);
         }
         */
      }

      return TRUE;
   }

   return FALSE;
}/*MGAppUtil_SpaceUnitConvert*/

/*===========================================================================
 * FUNCTION: MGAppUtil_FileAttribConvert
 *
 * DESCRIPTION: get string ID according to file attribute and load string into
 * buffer
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
boolean MGAppUtil_FileAttribConvert(IShell *pShell,
                  char cAttrib, AECHAR *pszAttrib, size_t nAttribLen)
{
   char attrib;
   int16 nStrID;
   int nRet;
   int nMaskStrLen;

   if(!pShell || !pszAttrib)
      return FALSE;

   attrib = cAttrib;
   nMaskStrLen = 0;

   //typedef enum {
   //   _FA_NORMAL=0,
   //   _FA_HIDDEN=0x0001,
   //   _FA_DIR=0x0002,
   //   _FA_READONLY=0x0004,
   //   _FA_SYSTEM=0x0008
   //} FileAttrib;
   //
   //Members:
   //
   //_FA_NORMAL : File is normal file
   //_FA_HIDDEN : File is a hidden file (reserved)
   //_FA_DIR : File is directory (reserved)
   //_FA_READONLY : File is read only file
   //_FA_SYSTEM : File is system file
   //
   /*
    * for FileAttrib may AND AEE_FA_CONST or AEE_FA_FIXED
    */
   if(AEE_FA_FIXED == (attrib & AEE_FA_FIXED))
      attrib &= !AEE_FA_FIXED;
   else if(AEE_FA_CONST == (attrib & AEE_FA_CONST))
      attrib &= !AEE_FA_CONST;

   if(attrib != cAttrib)
   {
      nRet = ISHELL_LoadResString(pShell,
                                  MGRES_LANGFILE,
                                  IDS_MG_CONST,
                                  pszAttrib,
                                  nAttribLen);
      WSTRCAT(pszAttrib, L", ");
      nMaskStrLen = WSTRLEN(pszAttrib);
   }

   switch(attrib)
   {
   case AEE_FA_NORMAL:
      nStrID = IDS_MG_ARCHIVE;
      break;

   case AEE_FA_READONLY:
      nStrID = IDS_MG_READONLY;
      break;

   case AEE_FA_HIDDEN:
      nStrID = IDS_MG_HIDDEN;
      break;

   case AEE_FA_SYSTEM:
      nStrID = IDS_MG_SYSTEM;
      break;

   default:
      nStrID = IDS_MG_UNKNOWN;
      break;
   }

   nRet = ISHELL_LoadResString(pShell,
                               MGRES_LANGFILE,
                               nStrID,
                               pszAttrib + nMaskStrLen,
                               nAttribLen - sizeof(AECHAR) * nMaskStrLen);

   return nRet == 0 ? FALSE : TRUE;
}//MGAppUtil_FileAttribConvert


/* ===================== End of File ===================================== */
