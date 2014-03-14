/**--------------------------------------------------------------------------
 * MediaGalleryApp.c -- It is media gallery applet, the application for handling
 *                      the media gallery.
 *
 * Copyright(c) 2008 Anylook
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2008.10.11       Create.
 *
 **--------------------------------------------------------------------------
 */

#include "MediaGalleryApp.h"
#include "AEEAppletCtl.h"
#include "AEEAnnunciator.h"
#include "AEECM.h"
#include "OEMHeap.h"
#ifdef FEATURE_SUPPORT_VC0848
#include "vc0848.h"
#endif
#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#endif
#include"Msg.h"
/*===========================================================================
 *                      MACRO DECLARATIONs
 * ==========================================================================
 */

/*===========================================================================
 *                      TYPE DECLARATIONs
 * ==========================================================================
 */
/*version number of file for store applet pref data*/
#define MG_APPPREF_VERSION      0x0001
typedef struct _MGAppPref{
   MGSortOrder    m_PrefSortOrder; //sort order type,  ascend , do not do descend now
   MGView         m_PrefView; //type for viewer, grid or list
}MGAppPref;

//#define FEATURE_TIMER_TEST 0
/*===========================================================================
 *
 *                      FUNCTION DECLARATIONS
 *
 * ==========================================================================
*/
#if defined(BREW_STATIC_APP)
extern int MediaGalleryApp_Load(IShell* ps, void* pHelpers, IModule** ppMod);
static int MediaGalleryApp_CreateInstance(AEECLSID ClsId, IShell* pIShell,
                                       IModule* po,   void** ppObj);
#endif//defined(BREW_STATIC_APP)

static int MediaGalleryApp_New(AEECLSID ClsId, IShell* pIShell,
                                 IModule* po, void** ppObj);
static uint32 CMediaGallery_AddRef(IMediaGallery* pi);
static uint32 CMediaGallery_Release(IMediaGallery* pi);
static boolean CMediaGallery_HandleEvent(IMediaGallery* pi, AEEEvent eCode,
                                       uint16 wParam,       uint32 dwParam);
static int MediaGalleryApp_InitAppData(CMediaGalleryApp* pMe);
static void MediaGalleryApp_FreeAppData(CMediaGalleryApp* pMe);
static void MediaGalleryApp_RunFSM(CMediaGalleryApp* pMe);
//static void MediaGalleryApp_BuildStartArgs(CMediaGalleryApp* pMe);
static boolean MediaGalleryApp_ParseStartArgs(CMediaGalleryApp* pMe,
                                          const char* pszStartArg);
static int MediaGalleryApp_RegisterMMCNotify(CMediaGalleryApp* pMe);
static int MediaGalleryApp_DeRegisterMMCNotify(CMediaGalleryApp* pMe);
static boolean MediaGalleryApp_CheckHsmmDir(CMediaGalleryApp* pMe);
static boolean MediaGalleryApp_CheckMassStorgeDir(CMediaGalleryApp* pMe);
/*test function*/
#ifdef FEATURE_MG_INTDBG
static void MediaGalleryApp_SetInterrupt(CMediaGalleryApp *pMe, boolean bStop);
static void MediaGalleryApp_Suspend(void *po);
static void MediaGalleryApp_Resume(void *po);
#endif
#ifdef FEATURE_TIMER_TEST
static void MediaGalleryApp_TestTimer(CMediaGalleryApp *pMe);
#endif
/*===========================================================================
 *
 *                              LOCAL/STATIC DATA
 *
 *===========================================================================
*/

/*data for restoring call back function when other applet interactive with
 * media gallery applet.*/
static PFNMGSELECTEDCB   gpfnSelect = NULL;
static void*             gpfnData = NULL;

/*U盘功能是否启动*/
static boolean           gbUDiskOn = FALSE;

/*USB口是否连接USB线*/
static boolean           gbUSBCableConnet = FALSE;

/*是否在播放音乐，仅对情景模式调用的铃声设置中的铃声预览，一般下播放铃声又
 * Music Player负责。此变量主要是插拔耳机时，声道切换(中星微848)。
 */
static boolean           gbPlayeMusic = FALSE;


#define MG_STARTARGPREFIX_REGISTERCALLBACK   ((char)'R')
#define MG_STARTARGPREFIX_FILEEXPLORER       ((char)'F')
#define MG_STARTARGPREFIX_SETAS              ((char)'S')
#define MG_STARTARGPREFIX_NORAMAL            ((char)'N')

/*===========================================================================
 * FUNCTION:MediaGalleryApp_Load
 * DESCRIPTION: Entry function called by BREW to initialize static module.
 *             Creates and Loads the Dialog
 * PARAMETERS:
 * RETURN VALUE: SUCCESS-- success to load
 *               other--failed, return error code
 * ==========================================================================
 */
#if defined(BREW_STATIC_APP)
int MediaGalleryApp_Load(IShell* ps, void* pHelpers, IModule** ppMod)
{
   return (AEEStaticMod_New((int16)(sizeof(AEEMod)),
            ps,
            pHelpers,
            ppMod,
            (PFNMODCREATEINST)MediaGalleryApp_CreateInstance,
            (PFNFREEMODDATA)NULL));
}//MediaGalleryApp_Load
#endif//BREW_STATIC_APP

/*===========================================================================
 * FUNCTION:MediaGalleryApp_CreateInstance
 * DESCRIPTION:
 *   This function is invoked while the app is being loaded. All Modules must
 *   provide this function. Ensure to retain the same name and parameters for
 *   this function
 * PARAMETERS:
 *   ClsId:    [in]: Specifies the ClassID of the applet which is being loaded
 *   pIShell:  [in]: Contains pointer to the IShell interface.
 *   pIModule: [in]: Contains pointer to the IModule interface to the current
 *                   module to whichthis app belongs
 *   ppObj:   [out]: On return, *ppObj must point to a valid AEEApplet
 *                   structure. Allocation of memory for this structure and
 *                   initializing the base data members is done by
 *                   AEEApplet_New().
 * RETURN VALUE:
 * ==========================================================================
 */
#if defined(BREW_STATIC_APP)
static int MediaGalleryApp_CreateInstance(AEECLSID ClsId, IShell* pIShell,
                                       IModule* po,      void** ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell* pIShell,IModule* po,void** ppObj)
#endif//BREW_STATIC_APP
{
   if(AEECLSID_MEDIAGALLERY == ClsId)
   {
      if( SUCCESS == MediaGalleryApp_New(ClsId,pIShell, po, ppObj))
      {
         int nRetVal = MediaGalleryApp_InitAppData((CMediaGalleryApp*)*ppObj);
         if( SUCCESS != nRetVal)
         {
            CMediaGallery_Release(*ppObj);
         }
         return nRetVal;
      }
      else
      {
         return EFAILED;
      }
   }
   else
   {
      return EUNSUPPORTED;
   }

   return SUCCESS;
}//MediaGalleryApp_CreateInstance

/*===========================================================================
 * FUNCTION:MediaGalleryApp_New
 * DESCRIPTION: creation of media gallery applet.
 * PARAMETERS:
 * RETURN VALUE:
 * ==========================================================================
 */
static int MediaGalleryApp_New(AEECLSID ClsId,
                              IShell* pIShell,
                              IModule* po,
                              void** ppObj)
{
   int   nRetVal = SUCCESS;
   CMediaGalleryApp* pMe = NULL;
   VTBL(IMediaGallery)* pAppFuncs;

   //Validate parameters
   if((NULL == ppObj) || (NULL == pIShell) || (NULL == po))
   {
      return EFAILED;
   }

   // Create an Applet object
   pMe = (CMediaGalleryApp*)MALLOC(sizeof(CMediaGalleryApp) +
                           sizeof(VTBL(IMediaGallery)));
   if(NULL == pMe)
   {
      return EFAILED;
   }

   pAppFuncs = (VTBL(IMediaGallery)*)((byte*)pMe + sizeof(CMediaGalleryApp));
   pAppFuncs->AddRef = CMediaGallery_AddRef;
   pAppFuncs->Release = CMediaGallery_Release;
   pAppFuncs->HandleEvent = CMediaGallery_HandleEvent;
   INIT_VTBL(pMe, IMediaGallery, *pAppFuncs);//Initialize the VTBL

   //Initialize the data members
   pMe->m_nRefs = 1;
   pMe->m_pShell = pIShell;
   pMe->m_pModule = po;
   pMe->m_pDisplay = NULL;
   pMe->clsID = ClsId;

   nRetVal = ISHELL_CreateInstance(pIShell,
                                    AEECLSID_DISPLAY,
                                    (void**)&pMe->m_pDisplay);

   if(!pMe->m_pDisplay)
   {
      //Cleanup
      FREE_VTBL(pMe, IMediaGallery);
      FREE(pMe);
      return EFAILED;
   }

   ISHELL_AddRef(pIShell);
   IMODULE_AddRef(po);

   *ppObj = (IMediaGallery*)pMe;

   return SUCCESS;
}//MediaGalleryApp_New

/*===========================================================================
 * FUNCTION:CMediaGallery_AddRef
 * DESCRIPTION: Increments Reference Count for IMediaGallery.
 *  PARAMETERS:
 * RETURN VALUE:
 * ==========================================================================
 */
static uint32 CMediaGallery_AddRef(IMediaGallery* pi)
{
   return ++(((CMediaGalleryApp*)pi)->m_nRefs);
}//CMediaGallery_AddRef

/*===========================================================================
 * FUNCTION:CMediaGallery_Release
 * DESCRIPTION: Decrements Reference Count for IMediaGallery. Does clean up
 *              when ref count reaches 0
 * PARAMETERS:
 * RETURN VALUE:
 * ==========================================================================
 */
static uint32 CMediaGallery_Release(IMediaGallery* pi)
{
   CMediaGalleryApp* pMe = (CMediaGalleryApp*)pi;
   IShell* pShell = pMe->m_pShell;

   if(--pMe->m_nRefs)
   {
      return pMe->m_nRefs;
   }

   MediaGalleryApp_FreeAppData(pMe);

   if(pMe->m_pDisplay)
      IDISPLAY_Release(pMe->m_pDisplay);

   IMODULE_Release(pMe->m_pModule);

   FREE_VTBL(pMe, IMediaGallery);
   FREEIF(pMe);

   ISHELL_Release(pShell);/*Release the Shell*/

   return 0;
}//CMediaGallery_Release

/*===========================================================================
 * FUNCTION:CMediaGallery_HandleEvent
 * DESCRIPTION:
 * PARAMETERS:
 * RETURN VALUE:
 * ==========================================================================
 */
static boolean CMediaGallery_HandleEvent(IMediaGallery* pi, AEEEvent eCode,
                                          uint16 wParam,    uint32 dwParam)
{
   CMediaGalleryApp* pMe = (CMediaGalleryApp*)pi;

   switch(eCode)
   {
      /* NOTICE>>
       * we may be need high cpu frequency, please refer OEM_Notify,
       * if define FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
       */
      case EVT_APP_START:
      {
         AEEAppStart* as = (AEEAppStart*) dwParam;
         boolean           bRet;
         pMe->m_rc = as->rc;
         pMe->m_bSuspending = FALSE;
         
         bRet = MediaGalleryApp_ParseStartArgs(pMe, as->pszArgs);
         if(FALSE == bRet)
            return FALSE;
         IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
         MediaGalleryApp_RunFSM(pMe);
#ifdef FEATURE_TIMER_TEST
         MediaGalleryApp_TestTimer(pMe);
#endif
         return TRUE;
      }

   case EVT_APP_STOP:
      {
         pMe->m_bSuspending = TRUE;
         MediaGalleryApp_DeRegisterMMCNotify(pMe);

#if 0//def FEATURE_APP_MUSICPLAYER
         if(GetMp3PlayerStatus()==MP3STATUS_RUNONBACKGROUND)
         {
            (void)ISHELL_PostEvent(pMe->m_pShell,
                                   AEECLSID_APP_MUSICPLAYER,
                                   EVT_ALARM,
                                   FALSE,
                                   0);
         }
#endif
         return TRUE;
      }

#ifdef FEATURE_AUTOEXIT_AFTER_BLDISABLE
   case EVT_APP_EXIT:
      {
         /*当Media gallery正常方式启动，并且不处在U盘模式下自动退出。*/
         if(AEE_Active() == AEECLSID_MEDIAGALLERY &&
            MediaGallery_CheckUdiskStat() == FALSE &&
            pMe->m_StartMode == MGSM_NORMAL_EXPLORER)
         {
            ISHELL_CloseApplet(pMe->m_pShell, TRUE);
         }
         return TRUE;
      }
      break;
#endif

   case EVT_APP_SUSPEND:
      {
         pMe->m_bSuspending = TRUE;
         /*
          * register key event here, for we do not want receive key event
          * notify when we are still active , it is also ok register at
          * EVT_DIALOG_END, for when applet suspend we first receive
          * EVT_APP_SUSPEND, then EVT_DIALOG_END
          * */
         MediaGalleryApp_RegisterPlayNext(pMe, FALSE);
         MediaGalleryApp_SetCallbackStateStop(pMe);

         RELEASEIF(pMe->m_pMenuPopup);
         ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);

         return TRUE;
      }

      /* NOTICE>>
       * we may be need high cpu frequency, please refer OEM_Notify,
       * if define FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
       */
   case EVT_APP_RESUME:
      {
         AEEAppStart* as = (AEEAppStart*) dwParam;
         boolean     bRet;

         pMe->m_rc = as->rc;
         pMe->m_bSuspending = FALSE;

         bRet = MediaGalleryApp_ParseStartArgs(pMe, as->pszArgs);

         if(FALSE == bRet)
            return FALSE;

         //MediaGalleryApp_RegisterPlayNext(pMe, TRUE);

         MediaGalleryApp_RunFSM(pMe);
         return TRUE;
      }
   case EVT_DIALOG_START:
      if(OEM_IME_DIALOG == wParam)
      {
          return TRUE;
      }
      (void) MediaGalleryApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);

       return TRUE;  

   case EVT_DIALOG_INIT:
      {
         if( wParam == OEM_IME_DIALOG)
    		{
    			return TRUE;
    		}
         // Update the active dialog info in the one and only Media gallery
         // applet object.
         pMe->m_pActiveDlg = (IDialog*)dwParam;
         pMe->m_nActiveDlgID = wParam;
         // Route the received event to the current active dialog handler.
         return MediaGalleryApp_RouteDialogEvent(pMe,eCode, wParam, dwParam);
      }

   case EVT_DIALOG_END:
      if(0 == wParam)
      {
         return TRUE;
      }
      if(OEM_IME_DIALOG == wParam)
      {
    	 return ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER_REDRAW,0,0);
      }
      MediaGalleryApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
      pMe->m_pActiveDlg = NULL;

      // Don't run the FSM if we are suspending (EVT_APP_SUSPEND is
      // sent before EVT_DIALOG_END).  Wait until we resume...
      if(pMe->m_bSuspending == FALSE)
      {
         /*Start the media Gallery state machine engine*/
         MediaGalleryApp_RunFSM(pMe);
      }

      return TRUE;

   case EVT_NOTIFY:
      {
         AEENotify* pNotify = (AEENotify*)dwParam;

         switch(pNotify->cls)
         {
         case AEECLSID_DEVICENOTIFIER:
            if(pNotify->dwMask & NMASK_DEVICENOTIFIER_MMC)
            {
               /*Just received an MMC notification*/
               AEEDeviceNotify* pDeviceNotify = pNotify->pData;

               if(AEE_INSERT_MMC == pDeviceNotify->wParam)
               {
                  /*Now MMC is ready to write!*/
                  pMe->m_bCardExist = TRUE;
                  MediaGalleryApp_CheckMassStorgeDir(pMe);
               }
               else
               {
                  /*AEE_REMOVE_MMC*/
                  /*Now we have to stop writing!*/
                  pMe->m_bCardExist = FALSE;
               }
            }
            return TRUE;

         case AEECLSID_BATT_NOTIFIER:
            switch (pNotify->dwMask)
            {
            case NMASK_BATTNOTIFIER_EXTPWR_CHANGE:
               MediaGallery_SetUSBCableConnect(*((boolean *)(pNotify->pData)));
               //(pMe->m_nActiveDlgID == IDD_MG_UDISK)
               if (FALSE == MediaGallery_CheckUSBCableConnect() &&
                   TRUE == MediaGallery_CheckUdiskStat() &&
                   pMe->m_nActiveDlgID == IDD_MG_UDISK)
               {
                  MG_FARF(ADDR, ("stop u-disk"));
                  MediaGalleryApp_RouteDialogEvent(pMe,
                                                   EVT_KEY,
                                                   AVK_CLR,
                                                   0);
               }
               break;

            default:
               return FALSE;
            }
            return TRUE;

         case AEECLSID_SHELL:
            {
               MG_FARF(ADDR, ("shell notify!"));
               if(pNotify->dwMask & NMASK_SHELL_KEY)
               {
                  NotifyKeyEvent *pKeyEvent = pNotify->pData;
                  AEECLSID nActiveID = 0;
                  boolean bNext;

                  //if(pMe->m_bSuspending == FALSE)
                  //{
                  //   MediaGalleryApp_DeRegisterKeyEvent(pMe);
                  //   return TRUE;
                  //}

                  MG_FARF(ADDR, ("Receive key event when suspend"));

                  if( pKeyEvent->eCode != EVT_KEY ||
                     (pKeyEvent->wParam != AVK_LEFT &&
                      pKeyEvent->wParam != AVK_RIGHT))
                  {
                     MG_FARF(ADDR, ("NOT need key"));
                     return FALSE;
                  }

                  nActiveID = ISHELL_ActiveApplet(pMe->m_pShell);

                  if(nActiveID == AEECLSID_APP_MUSICPLAYER ||
                     nActiveID == AEECLSID_VIDEOPLAYER)
                  {
                     MG_FARF(ADDR,
                             ("eCode:0x%x, wParam:0x%x, dwParam:0x%x",
                              pKeyEvent->eCode,
                              pKeyEvent->wParam,
                              pKeyEvent->dwParam));

                     /*
                      * It is better here, If put this statement in Dialog
                      * handler, it may not receive key event. This because
                      * when we are suspend, m_pActiveDlg is NULL,
                      * MediaGalleryApp_RouteDialogEvent and Dialog handler
                      * will check it. */
                     bNext = (boolean)(pKeyEvent->wParam == AVK_RIGHT ?
                                       MG_PLAY_NEXT : MG_PLAY_PREV);
                     return MediaGalleryApp_PlayNext(pMe, bNext);
                  }
               }
            }
            break;
         }
      }
      break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH

			  case EVT_USER:
			      MSG_FATAL("EVT_USER........wParam=%x",wParam,0,0);
				  if(wParam == AVK_CLR)
				  {
					  eCode = EVT_KEY;
				  }
				  else if((wParam == AVK_SELECT) || (wParam == AVK_INFO))
				  {
					  if(dwParam != 0)
						  {
							  eCode = EVT_COMMAND;
							  wParam = dwParam;
							  dwParam = 0;
						  }
						  else
						  {
							  eCode = EVT_KEY;
						  }
				  }
				  return MediaGalleryApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
			  break;
#endif

   default:
#ifdef FEATURE_MG_INTDBG
      if(eCode == EVT_KEY && wParam == AVK_POUND)
         MediaGalleryApp_SetInterrupt(pMe, TRUE);
#endif
      return MediaGalleryApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
   }

   return FALSE;
}//CMediaGallery_HandleEvent

/*===========================================================================
 * FUNCTION:
 * DESCRIPTION:
 * PARAMETERS:
 * RETURN VALUE:
 * ==========================================================================
 */
static boolean MediaGalleryApp_ParseStartArgs(CMediaGalleryApp* pMe,
                                             const char* pszStartArg)
{
   char* pszArg;
   char* pTemp;
   boolean bRet = TRUE;
   //char pData[11];//2^32 = 4294967296

   if(!pMe)
      return FALSE;

   if(!pszStartArg)
   {
      return TRUE;
   }
   DBGPRINTF("pszStartArg=%s", pszStartArg);
   pszArg = STRDUP(pszStartArg);

   if(!pszArg)
      return FALSE;

   pTemp = pszArg + 1;
   strtok(pTemp,",,");
   if(NULL == pTemp)
   {
      bRet =  FALSE;
      goto PARSEEXIT;
   }

   switch(*pszStartArg)
   {
#if   0
   case MG_STARTARGPREFIX_REGISTERCALLBACK:
      {
         strtok(pTemp,",,");
         pTemp++;
         gpfnSelect = (PFNMGSELECTEDCB)ATOI(pTemp);

         pTemp = strtok(NULL, ",,");
         gpfnData = (void*)ATOI(pTemp);

         pMe->m_eDlgRet = MGDLGRET_NONE;
      }
      break;
#endif

   case MG_STARTARGPREFIX_FILEEXPLORER:
      {
         GalleryType eType;
         MGStartMode       eStartMode = MGSM_NORMAL_EXPLORER;
         if(!pMe->m_pFileMgr || !pMe->m_pFolderList)
         {
            bRet =  FALSE;
            goto PARSEEXIT;
         }

         pTemp++;
         eType = ATOI(pTemp);
         MSG_FATAL("eType=%d",eType,0,0);
         switch(eType)
         {
         case GALLERY_PHOTO_BROWSE:
            eStartMode = MGSM_PHOTO_BROWSE;
            break;

         case GALLERY_VIDEO_BROWSE:
            eStartMode = MGSM_VIDEO_BROWSE;
            break;

         case GALLERY_MUSIC_ADDLIST:
            eStartMode = MGSM_MUSIC_ADD;
            break;

         case GALLERY_VIDEO_ADDLIST:
            eStartMode = MGSM_VIDEO_ADD;
            break;

         case GALLERY_RECORD_VIEWLIST:
            eStartMode = MGSM_RECORD_VIEW;
            break;

         case GALLERY_IMAGE_SETTING:
            eStartMode = MGSM_IMAGE_SETTING;
            break;

         case GALLERY_MUSIC_SETTING:
            eStartMode = MGSM_MUSIC_SETTING;
            break;

         case GALLERY_FILE_SELECT:
            eStartMode = MGSM_FILE_SELECT;
            break;

         default:
            eStartMode = MGSM_FILE_BROWSE;
            break;
         }
#ifdef FEATURE_USES_MMS  
         if((eType == GALLERY_IMAGE_SETTING) || (eType == GALLERY_VIDEO_BROWSE)
          ||(eType == GALLERY_MUSIC_SETTING) || (eType == GALLERY_FILE_SELECT))  
         {
             pTemp++;
             MSG_FATAL("ATOI(pTemp)=%d", ATOI(pTemp+1),0,0);
             if(ATOI(++pTemp) == 1)
             {
                pMe->m_isForMMS = TRUE;
             }
             else
             {
                pMe->m_isForMMS = FALSE;
             }
         }
#endif
         pTemp = STRRCHR(pszStartArg,',');
         if (NULL != pTemp)
         {
            pTemp++;
            if(pMe->m_pExpPara)
            {
               sys_free(pMe->m_pExpPara);
            }
            pMe->m_pExpPara = (ExplorerPara *)ATOI(pTemp);
         }

         pMe->m_StartMode = eStartMode;
      }
      break;

   case MG_STARTARGPREFIX_SETAS:
      {
         SettingsType eSType;
         char* pszName;

         pTemp++;
         eSType = ATOI(pTemp);

         pTemp = strtok(NULL, ",,");
         if(NULL == pTemp)
         {
            bRet =  FALSE;
            goto PARSEEXIT;
         }
         pszName = (void*)ATOI(pTemp);

         if(SETTING_WALLPAPER == eSType)
            pMe->m_StartMode = MGSM_SETTING_WALLPAPER;
         else if(SETTING_RINGTONE == eSType)
            pMe->m_StartMode = MGSM_SETTING_RINGTONE;
         else
         {
            bRet =  FALSE;
            goto PARSEEXIT;
         }
      }
      break;

   default:
      bRet = FALSE;
      goto PARSEEXIT;
   }

PARSEEXIT:
   FREEIF(pszArg);
   return bRet;
}//MediaGalleryApp_ParseStartArgs


/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_GetAppPref
 * DESCRIPTION  :  retrieve application-specific settings or preferences
 *                 information persistently
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int32 CMediaGallery_GetAppPref(CMediaGalleryApp* pMe)
{
   MGAppPref sCurPref;
   int nGetRet;

   if(!pMe || !pMe->m_pShell)
      return EFAILED;

   MEMSET(&sCurPref, 0, sizeof(MGAppPref));

   nGetRet = ISHELL_GetAppPrefs(pMe->m_pShell,
         MG_APPPREF_VERSION,
         &sCurPref,
         sizeof(MGAppPref));

   if(nGetRet == SUCCESS) {
      pMe->m_Explorer.m_eSortOrder = sCurPref.m_PrefSortOrder;
      pMe->m_Explorer.m_eView = sCurPref.m_PrefView;
   }
   else {
      pMe->m_Explorer.m_eSortOrder = MG_SORT_NONE;
      pMe->m_Explorer.m_eView = MG_VIEW_LIST;
      CMediaGallery_SetAppPref(pMe);
   }

   return nGetRet;
}//CMediaGallery_InitAppPref


/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_SetAppPref
 * DESCRIPTION  :  store applet preference data to file system
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int32 CMediaGallery_SetAppPref(CMediaGalleryApp* pMe)
{
   MGAppPref sCurPref;
   int nSetRet;

   if(!pMe || !pMe->m_pShell)
      return EFAILED;

   MEMSET(&sCurPref, 0, sizeof(MGAppPref));

   sCurPref.m_PrefSortOrder = pMe->m_Explorer.m_eSortOrder;
   sCurPref.m_PrefView = pMe->m_Explorer.m_eView;

   nSetRet = ISHELL_SetAppPrefs(pMe->m_pShell,
         MG_APPPREF_VERSION,
         &sCurPref,
         sizeof(MGAppPref));

   return nSetRet;
}//CMediaGallery_SetAppPref

/*===========================================================================
 * FUNCTION: CMediaGallery_RegisterCallback
 * DESCRIPTION: For client that invoke file explorer such as music player
 * regiseter call back function
 * PARAMETERS:
 * RETURN VALUE:
 * ==========================================================================
 */
int32 CMediaGallery_RegisterCallback(PFNMGSELECTEDCB pfn,
                                          void* pv)
{
   //char* pszArg;
   //IShell* pShell = AEE_GetShell();

   if(!pfn || !pv)
   {
      return EBADPARM;
   }

#if 0
   pszArg = (char*) MALLOC(30);

   SNPRINTF(pszArg, 30, "%c,%u,%u",MG_STARTARGPREFIX_REGISTERCALLBACK,pfn,pv);

   ISHELL_StartAppletArgs(pShell, AEECLSID_MEDIAGALLERY, pszArg);
   FREEIF(pszArg);
#endif

   gpfnSelect = pfn;
   gpfnData = pv;

   return SUCCESS;
}//CMediaGallery_RegisterCallback

/*===========================================================================
 * FUNCTION:CMediaGallery_FileExplorer
 * DESCRIPTION: invoke by other applet explorer file, such as music player to
 * add file to play list.
 * PARAMETERS: eType: browser file type, for example picture, or music file.
 * RETURN VALUE:
 * ==========================================================================
 */
int32 CMediaGallery_FileExplorer(GalleryType eType, ExplorerPara *pPara)
{
   char pszArg[100];
   ExplorerPara *pExpPara = NULL;
   int nRet = EFAILED;
   if (NULL != pPara)
   {
      pExpPara = (ExplorerPara *)sys_malloc(sizeof(ExplorerPara));
      if(NULL == pExpPara)
      {
         MG_FARF(ADDR, ("Allocate memory failed"));
         return EFAILED;
      }
      MEMCPY(pExpPara, pPara, sizeof(ExplorerPara));
   }
   
   MG_FARF(ADDR, ("copy explorer data, later just take pointer"));

   SNPRINTF(pszArg, sizeof(pszArg),
            "%c,%u,%u",MG_STARTARGPREFIX_FILEEXPLORER,eType,pExpPara);

   nRet = ISHELL_StartAppletArgs(AEE_GetShell(),
                                 AEECLSID_MEDIAGALLERY,
                                 pszArg);
   /*If failed to start applet, free the memory*/
   if(SUCCESS != nRet)
   {
      if(pExpPara)
      {
          sys_free(pExpPara);
      }
   }
   return SUCCESS;
}//CMediaGallery_FileExplorer


#ifdef FEATURE_USES_MMS
int32 CMediaGallery_FileExplorer_ForMMS(GalleryType eType, ExplorerPara *pPara)
{
   char pszArg[100];
   ExplorerPara *pExpPara = NULL;
   int nRet = EFAILED;
   if (NULL != pPara)
   {
      pExpPara = (ExplorerPara *)sys_malloc(sizeof(ExplorerPara));
      if(NULL == pExpPara)
      {
         MG_FARF(ADDR, ("Allocate memory failed"));
         return EFAILED;
      }
      MEMCPY(pExpPara, pPara, sizeof(ExplorerPara));
   }
   
   MG_FARF(ADDR, ("copy explorer data, later just take pointer"));

   SNPRINTF(pszArg, sizeof(pszArg),
            "%c,%u,%u,%u",MG_STARTARGPREFIX_FILEEXPLORER,eType,1,pExpPara);

   nRet = ISHELL_StartAppletArgs(AEE_GetShell(),
                                 AEECLSID_MEDIAGALLERY,
                                 pszArg);
   /*If failed to start applet, free the memory*/
   if(SUCCESS != nRet)
   {
      if(pExpPara)
      {
          sys_free(pExpPara);
      }
   }
   return SUCCESS;
}//CMediaGallery_FileExplorer_ForMMS
#endif
/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_GetTflashStatus
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean CMediaGallery_GetTflashStatus(void)
{
   boolean bExist = FALSE;
#if !defined(FEATURE_SUPPORT_VC0848)
   IShell *ps = AEE_GetShell();
   IFileMgr *pFileMgr = NULL;
#endif

   MG_FARF(ADDR, ("CMediaGallery_GetTflashStatus"));

#ifdef FEATURE_SUPPORT_VC0848
   bExist = MediaGalleryApp_CheckSDCard();
   MG_FARF(ADDR, ("stop sd card to save power!"));
   MediaGalleryApp_StopSDCard();
#else
   if(SUCCESS != ISHELL_CreateInstance(ps,
                                       AEECLSID_FILEMGR,
                                       (void **)&pFileMgr))
   {
      MG_FARF(ADDR, ("create file manager failed"));
      return FALSE;
   }

   if(SUCCESS == IFILEMGR_Test(pFileMgr, MG_MASSCARD_ROOTDIR))
      bExist = TRUE;
   else
      bExist = FALSE;
   IFILEMGR_Release(pFileMgr);
#endif

   return bExist;
}


/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_GetSaveDataFloder
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
const char * CMediaGallery_GetSaveDataFloder(uint8 nStoreMedium,
                                             uint16 nFileType)
{
   uint16 nMimeBase;
   const char *cpszPath = NULL;

   nMimeBase = MGMediaInfo_FileType2MimeBase(nFileType);
   MGMediaInfo_GetForderFromMimeBase(nMimeBase, nStoreMedium, &cpszPath);

   return cpszPath;
}


/*
 * ==========================================================================
 * FUNCTION     :  boolean CMediaGallery_GetReturnData
 * DESCRIPTION  :  when MMS call Media gallery, we return the select file
 * path, but MMS do not know the data struct, so we do for them . In fact , it
 * just do string copy.
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean CMediaGallery_GetReturnData(const void *cpSrc,
                                     void *pDest,
                                     uint16 nDestLen)
{
   char *pszDest;

   if(NULL == cpSrc || NULL == pDest || 0 == nDestLen)
   {
      MG_FARF(ADDR, ("ERROR parameter!"));
      FREEIF(cpSrc);
      return FALSE;
   }

   pszDest = (char *)pDest;

   STRNCPY(pszDest, cpSrc, nDestLen);
   *(pszDest + nDestLen -1 ) = '\0';
   FREEIF(cpSrc);
   return TRUE;
}

/*===========================================================================
 * FUNCTION: MediaGallery_DoExplorerAddCB
 * DESCRIPTION: call back for other applets start the applet for explorer
 * files, tell them the select files names list.
 * PARAMETERS: pszNmae - buffer for storing file name ,
 *             nNum - number of files
 * RETURN VALUE: None
 * ==========================================================================
 */
int MediaGallery_DoExplorerAddCB(FileNamesBuf pszName, uint16 nNum)
{
   if(gpfnData && gpfnSelect)
   {
      return gpfnSelect (gpfnData, pszName, nNum);
   }
   else
   {
      MG_FARF(ADDR, ("MediaGallery_DoExplorerAddCB: BAD PARAMETER!"));
      return EFAILED;
   }

   return SUCCESS;
}//MediaGallery_DoExplorerAddCB

/*===========================================================================
 * FUNCTION:
 * DESCRIPTION:
 * PARAMETERS:
 * RETURN VALUE:
 * ==========================================================================
 */
int32 CMediaGallery_SetAs(SettingsType eSType,
                              const char* cpszFileName)
{
   IShell* pShell = AEE_GetShell();
   int      nRet;
   IFileMgr* pfm;
   char* pszArg;

   if(!cpszFileName)
      return EFAILED;

   if(ISHELL_CreateInstance(pShell, AEECLSID_FILEMGR, (void **)&pfm))
      return ENOMEMORY;

   nRet = IFILEMGR_Test(pfm, cpszFileName);

   if(nRet)
   {
      MG_FARF(ADDR, ("File or Directory is not exist!"));
      RELEASEIF(pfm);
      return nRet;
   }

   pszArg = (char*)MALLOC(30);
   SNPRINTF(pszArg, 30, "%c,%u,%u",
         MG_STARTARGPREFIX_SETAS,eSType,cpszFileName);
   ISHELL_StartAppletArgs(pShell, AEECLSID_MEDIAGALLERY, pszArg);

   FREEIF(pszArg);
   RELEASEIF(pfm);

   return SUCCESS;
}//MeidaGallery_SetAs

int CMediaGallery_ClearMediaFiles(void *po)
{
   IShell *ps = AEE_GetShell();
   IFileMgr *pIFileMgr = NULL;
   IVector *pFolderList = NULL;
   int nRet = SUCCESS;

   if(SUCCESS != (nRet = ISHELL_CreateInstance(ps,
               AEECLSID_FILEMGR, (void **)&pIFileMgr)))
   {
      goto CLEARFILEEXIT;
   }

   if(SUCCESS != (nRet = ISHELL_CreateInstance(ps,
               AEECLSID_VECTOR, (void **)&pFolderList)))
   {
      goto CLEARFILEEXIT;
   }

   MGExplorer_RemovDir(pIFileMgr, pFolderList, MG_PHONEPICTURE_PATH, FALSE);
   MGExplorer_RemovDir(pIFileMgr, pFolderList, MG_PHONEMUSIC_PATH, FALSE);
   MGExplorer_RemovDir(pIFileMgr, pFolderList, MG_PHONEVIDEOS_PATH, FALSE);
   MGExplorer_RemovDir(pIFileMgr, pFolderList, MG_PHONECALLMEMO_PATH, FALSE);
#ifdef  FEATURE_MG_WAPMMS_SUPPORT
   MGExplorer_RemovDir(pIFileMgr, pFolderList, MG_PHONEDOWNLOAD_PATH, FALSE);
#endif
CLEARFILEEXIT:
   RELEASEIF(pIFileMgr);
   RELEASEIF(pFolderList);
   return nRet;
}//

/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_OpenFileInfoDatabase
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 *
 * bCreate : Specifies if database will be created if database is not found.
 *            The database is created only if this flag is set to true.
 *
 * ==========================================================================
 */
int32 CMediaGallery_OpenFileInfoDatabase(IShell *pShell,
                                          const char *cpszDBFile,
                                          boolean bCreate,
                                          IDatabase **ppFileDB)
{
   IDBMgr *pDBMgr;
   int nErr;

   if(!pShell || !cpszDBFile || !ppFileDB)
   {
      MG_FARF(STATE, ("OpenFileInfoDatabase error parameter"));
      return EFAILED;
   }

   if(NULL != *ppFileDB)
   {
      MG_FARF(STATE, ("Out parameter ppFileDB NOT null"));
      return EFAILED;
   }

   // Open the database
   nErr = ISHELL_CreateInstance(pShell, AEECLSID_DBMGR, (void**)&pDBMgr);
   if (nErr != SUCCESS) {
      MG_FARF(STATE, ("Create database manager failed"));
      return EFAILED;
   }

   *ppFileDB = IDBMGR_OpenDatabase(pDBMgr, cpszDBFile, bCreate);

   if(NULL == *ppFileDB)
   {
      MG_FARF(STATE, ("OpenFileInfoDatabase failed"));
      nErr = EFAILED;
   }
   else{
      IDATABASE_Reset(*ppFileDB);
   }

   RELEASEIF(pDBMgr);
   return nErr;
}//CMediaGallery_OpenFileInfoDatabase

/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_CreateFileInfoRecord
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int32 CMediaGallery_CreateFileInfoRecord(IDatabase *cpFilesDB,
                                          MGFileInfo *cpInfo)
{
   AEEDBField  fiField[FDF_MAX-FDF_BASE];
   IDBRecord *pFileDBRecord = NULL;
#if 0
   AECHAR wszName[128];
#endif

   if(!cpFilesDB || !cpInfo)
   {
      MG_FARF(STATE, ("CreateFileInfoRecord failed"));
      return EFAILED;
   }

   MEMSET(&fiField, 0, sizeof(fiField));

   fiField[FDF_NAME].fType = AEEDB_FT_STRING;
   fiField[FDF_NAME].fName = FDF_NAME;
   fiField[FDF_NAME].wDataLen =
      sizeof(char) * (STRLEN(cpInfo->szName) + 1);

#if 0
   STRTOWSTR(cpInfo->szName, wszName, sizeof(wszName));
   fiField[FDF_NAME].pBuffer = wszName;//L"Hello world";
#else
   fiField[FDF_NAME].pBuffer = (void *)cpInfo->szName;
#endif

   fiField[FDF_ATTRIB].fType = AEEDB_FT_BYTE;
   fiField[FDF_ATTRIB].fName = FDF_ATTRIB;
   fiField[FDF_ATTRIB].wDataLen = sizeof(char);
   fiField[FDF_ATTRIB].pBuffer = (void *)&cpInfo->attrib;

   fiField[FDF_SIZE].fType = AEEDB_FT_DWORD;
   fiField[FDF_SIZE].fName = FDF_SIZE;
   fiField[FDF_SIZE].wDataLen = sizeof(uint32);
   fiField[FDF_SIZE].pBuffer = (void *)&cpInfo->dwSize;

   fiField[FDF_CREATIONDATE].fType = AEEDB_FT_DWORD;
   fiField[FDF_CREATIONDATE].fName = FDF_CREATIONDATE;
   fiField[FDF_CREATIONDATE].wDataLen = sizeof(uint32);
   fiField[FDF_CREATIONDATE].pBuffer = (void *)&cpInfo->dwCreationDate;

   pFileDBRecord = IDATABASE_CreateRecord(cpFilesDB,
                                       fiField,
                                       sizeof(fiField)/sizeof(AEEDBField));

   MG_FARF(ADDR, ("ADD file record %x", pFileDBRecord));

   MG_FARF(STATE,
            ("File name %s, attrib %d, size %d, date %d\n",
            cpInfo->szName, cpInfo->attrib, cpInfo->dwSize,
            cpInfo->dwCreationDate));

   /*release, otherwise system complain memory leak*/
   RELEASEIF(pFileDBRecord);

   return SUCCESS;
}//CMediaGallery_CreateFileInfoDatabase

/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_GetFileInfoRecord
 * DESCRIPTION  :
 * PARAMETERS   : wRecID -- record ID, base index is 0.
 * RETURN VALUE :
 * ==========================================================================
 */
int32 CMediaGallery_GetFileInfoRecord(IDatabase *cpFilesDB,
                                       uint16 wRecID,
                                       MGFileInfo *pInfo)
{
   IDBRecord *pFileDBRecord = NULL;
   AEEDBField  fiField[FDF_MAX-FDF_BASE];
   AEEDBField sFileField;
   uint16 wID;
   uint32 dwCount;

   if(!cpFilesDB || !pInfo)
   {
      MG_FARF(STATE, ("GetFileInfoRecord bad parameter!"));
      return EFAILED;
   }

   MEMSET(&fiField, 0, sizeof(fiField));
   MEMSET(&sFileField, 0, sizeof(AEEDBField));
   MEMSET(pInfo, 0, sizeof(MGFileInfo));

   dwCount = IDATABASE_GetRecordCount(cpFilesDB);

   if(dwCount == 0)
   {
      MG_FARF(ADDR,("Database has only no record"));
      return EFAILED;
   }
   else if(wRecID > (uint16)dwCount)
   {
      MG_FARF(ADDR,("GetFileInfoRecord invalid wRecID,\
               dwCount:%d, wRecID:%d", dwCount, wRecID));
      return EFAILED;
   }

   pFileDBRecord = IDATABASE_GetRecordByID(cpFilesDB, wRecID);

   if(NULL != pFileDBRecord)
   {
      IDBRECORD_Reset(pFileDBRecord);
      wID = IDBRECORD_GetID(pFileDBRecord);

      sFileField.fType = IDBRECORD_NextField(pFileDBRecord,
            &sFileField.fName,
            &sFileField.wDataLen);
      fiField[FDF_NAME].pBuffer = IDBRECORD_GetField(pFileDBRecord,
            &fiField[FDF_NAME].fName,
            &fiField[FDF_NAME].fType,
            &fiField[FDF_NAME].wDataLen);
      MEMCPY((void *)pInfo->szName,
            fiField[FDF_NAME].pBuffer,
            fiField[FDF_NAME].wDataLen);

      sFileField.fType = IDBRECORD_NextField(pFileDBRecord,
            &sFileField.fName,
            &sFileField.wDataLen);
      fiField[FDF_ATTRIB].pBuffer = IDBRECORD_GetField(pFileDBRecord,
            &fiField[FDF_ATTRIB].fName,
            &fiField[FDF_ATTRIB].fType,
            &fiField[FDF_ATTRIB].wDataLen);
      MEMCPY((void *)&pInfo->attrib ,
            fiField[FDF_ATTRIB].pBuffer,
            fiField[FDF_ATTRIB].wDataLen);

      sFileField.fType = IDBRECORD_NextField(pFileDBRecord,
            &sFileField.fName,
            &sFileField.wDataLen);
      fiField[FDF_SIZE].pBuffer = IDBRECORD_GetField(pFileDBRecord,
            &fiField[FDF_SIZE].fName,
            &fiField[FDF_SIZE].fType,
            &fiField[FDF_SIZE].wDataLen);
      MEMCPY((void *)&pInfo->dwSize,
            fiField[FDF_SIZE].pBuffer,
            fiField[FDF_SIZE].wDataLen);

      sFileField.fType = IDBRECORD_NextField(pFileDBRecord,
            &sFileField.fName,&sFileField.wDataLen);
      fiField[FDF_CREATIONDATE].pBuffer = IDBRECORD_GetField(pFileDBRecord,
            &fiField[FDF_CREATIONDATE].fName,
            &fiField[FDF_CREATIONDATE].fType,
            &fiField[FDF_CREATIONDATE].wDataLen);
      MEMCPY((void *)&pInfo->dwCreationDate,
            fiField[FDF_CREATIONDATE].pBuffer,
            fiField[FDF_CREATIONDATE].wDataLen);

      MG_FARF(STATE,
            ("Record ID %d,File name %s, attrib %d, size %d, date %d\n",
             wID, pInfo->szName, pInfo->attrib, pInfo->dwSize,
             pInfo->dwCreationDate));

      RELEASEIF(pFileDBRecord);
   }
   else{
      MG_FARF(STATE, ("no record, may be last record or database empty"));
      return EFAILED;
   }

   return SUCCESS;
}//CMediaGallery_GetFileInfoRecord

int CMediaGallery_DelFileInfoRecord(IDatabase *cpFilesDB)
{
   return SUCCESS;
}//CMediaGallery_DelFileInfoRecord


/*===========================================================================
 * FUNCTION:MediaGalleryApp_InitAppData
 * DESCRIPTION:
 * PARAMETERS:
 * RETURN VALUE: SUCCESS - initialize applet relative data successfully.
 *                Other - failed to initialize.
 * ==========================================================================
 */
static int MediaGalleryApp_InitAppData(CMediaGalleryApp* pMe)
{
   int i=0;
   if(!pMe)
   {
      return EFAILED;
   }

#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)
   pMe->m_strPhonePWD = NULL;
#endif
   pMe->m_bSuspending = FALSE;
   pMe->m_StartMode = MGSM_NORMAL_EXPLORER;
   pMe->m_bNotOverwriteDlgRet = FALSE;
   pMe->m_eDlgRet = MGDLGRET_CREATE;
   pMe->m_ePreState = STATE_NONE;
   pMe->m_eCurState = STATE_INIT;
   pMe->m_MediaDlgStat = MG_DLGSTAT_UNKNOWN;
   pMe->m_pActiveDlg = NULL;
   pMe->m_nMsgBoxTextId = MG_TEXTID_INVALID;
   pMe->m_eMsgBoxType = MESSAGE_NONE;
   pMe->m_eMsgBoxBottomBar = BTBAR_NONE;
   pMe->m_pszMsgBoxResFile = NULL;
   pMe->m_USBConnect = FALSE;
   pMe->m_STOPUSB = FALSE;

   
   pMe->m_nCallbackDoFor = MG_CBT_NULL;
   pMe->m_nEnumeResult = MG_ENUM_UNKNOWN;
   pMe->m_StoreMedium = MG_STMED_ALL;
   pMe->m_pFolderList = NULL;
   pMe->m_pFolderInfo = NULL;
   pMe->m_pPathMenu = NULL;
   pMe->m_pMenuPopup = NULL;
   pMe->m_pMediaMenu = NULL;

   pMe->m_pPathExp = NULL;
   pMe->m_pFileMgr = NULL;
   pMe->m_pFileIcons = NULL;
   pMe->m_pExpPara = NULL;
   pMe->m_pfnNotify = NULL;
   pMe->m_pInputFileName = NULL;
   pMe->m_pszNameList = NULL;
   pMe->m_pFilesDB = NULL;
   pMe->m_pDetailText = NULL;
   pMe->m_videoLoadDone = FALSE;  //add by yangdecai 2011-0728

   pMe->m_pMedia = NULL;
   pMe->m_pMediaSpec = NULL;

   pMe->m_nRingVolume = MG_MAX_UINT16;
   pMe->m_nKeyBeepVol = MG_MAX_BYTE;
   pMe->m_Gif = FALSE;
   pMe->m_pImage = NULL;
#ifdef FEATURE_ANICTL
   pMe->m_pAniCtl = NULL;
#endif
   pMe->m_option_sel = 0;

   pMe->m_bKeepMediaMenu = FALSE;

   MEMSET(&pMe->m_Explorer, 0, sizeof(CFSExplorer));
   MEMSET(&pMe->m_Copy, 0, sizeof(CMGFileCopy));
   MEMSET(&pMe->m_CallBack, 0, sizeof(AEECallback));
   MEMSET(&pMe->m_nPrevMenuItemSel, 0xFF, sizeof(pMe->m_nPrevMenuItemSel));

   if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
            AEECLSID_VECTOR,
            (void**)&pMe->m_pFolderList))
   {
      return EFAILED;
   }
   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_ANNUNCIATOR,
                                            (void **)&pMe->m_pIAnn))
    {
        return EFAILED;
    }

   if( SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
            AEECLSID_FILEMGR,
            (void**)&pMe->m_pFileMgr))
      return EFAILED;

   //yuezhen.li add @20090206
#if 0//def FEATURE_APP_MUSICPLAYER
   if(GetMp3PlayerStatus() == MP3STATUS_RUNONBACKGROUND)
   {
       (void)ISHELL_PostEvent(pMe->m_pShell,
                              AEECLSID_APP_MUSICPLAYER,
                              EVT_ALARM,
                              TRUE,
                              0);
   }
#endif
   //yuezhen.li end
   for(i=0;i<50;i++)
   {
     MEMSET(pMe->m_Musiclist[i].pMusicName,0,257*sizeof(char));
   }
   IDisplay_GetClipRect(pMe->m_pDisplay, &pMe->m_ClipRect);
   CMediaGallery_GetAppPref(pMe);
   MediaGalleryApp_RegisterBatteryNotify(pMe);

   return SUCCESS;
}//MediaGalleryApp_InitAppData

/*===========================================================================
 * FUNCTION:    MediaGalleryApp_FreeAppData
 * DESCRIPTION: 释放Applet相关的接口和内存。
 * PARAMETERS:
 * RETURN VALUE:
 * ==========================================================================
 */
static void MediaGalleryApp_FreeAppData(CMediaGalleryApp* pMe)
{
   if(!pMe)
   {
      return;
   }
#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)
   FREEIF(pMe->m_strPhonePWD);
#endif

   ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
   CALLBACK_Cancel(&pMe->m_CallBack);

   /*store applet specific settings*/
   CMediaGallery_SetAppPref(pMe);

   MediaGalleryApp_DeRegisterKeyEvent(pMe);
   MediaGalleryApp_DeRegisterMMCNotify(pMe);
   MediaGalleryApp_DeRegisterBatteryNotify(pMe);

   MGExplorer_FreeVectorElements(pMe->m_pFolderList);
   RELEASEIF(pMe->m_pFolderList);
   FREEIF(pMe->m_pFolderInfo);

   RELEASEIF(pMe->m_pMenuPopup);

   MGExplorer_FreeMediaMenuItem(pMe->m_pPathMenu);
   RELEASEIF(pMe->m_pPathMenu);

   MGExplorer_FreeMediaMenuItem(pMe->m_pMediaMenu);
   RELEASEIF(pMe->m_pMediaMenu);

   if(NULL != pMe->m_pPathExp)
   {
      MEMSET(pMe->m_pPathExp, 0, sizeof(pMe->m_pPathExp));
      FREE(pMe->m_pPathExp);
      pMe->m_pPathExp = NULL;
   }
   if(pMe->m_pIAnn != NULL)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn = NULL;
    } 

   MGExplorer_ReleaseFileIcon(pMe->m_pFileIcons);
   FREEIF(pMe->m_pFileIcons);
   if(pMe->m_pExpPara)
   {
       sys_free(pMe->m_pExpPara);
       pMe->m_pExpPara = NULL;
   }

   RELEASEIF(pMe->m_pFileMgr);
   MG_CLEAR_COPYDATA(pMe->m_Copy);

   RELEASEIF(pMe->m_pFilesDB);
   RELEASEIF(pMe->m_pDetailText);

   FREEIF(pMe->m_pszNameList);
   FREEIF(pMe->m_pInputFileName);

   RELEASEIF(pMe->m_pMedia);

   FREEIF(pMe->m_pMediaSpec);

   RELEASEIF(pMe->m_pImage);
#ifdef FEATURE_ANICTL
   RELEASEIF(pMe->m_pAniCtl);
#endif

#ifdef FEATURE_SUPPORT_VC0848
   if(0 == Appscommon_bluetooth_used_sd(-1)) //jinqiang add,2009/03/13
   {
      MediaGalleryApp_StopSDCard();
   }
#endif
}//MediaGalleryApp_FreeAppData

/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_CheckHsmmDir
 * DESCRIPTION  :  检查SD卡中我们指定的文件夹是否存在，如果没有则创建，假如插
 * 入了存储卡。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MediaGalleryApp_CheckMassStorgeDir(CMediaGalleryApp* pMe)
{
   const char *pMassStorgeFolders[] = {
      MG_MASSCARDPICTURE_PATH,
      MG_MASSCARDMUSIC_PATH,
      MG_MASSCARDVIDEO_PATH ,
      MG_MASSCARDVOICEMEMO_PATH,
#ifdef FEATURE_VERSION_C316
      //MG_MASSCARDCALLRECOED_PATH,      //Add by pyuangui 2013-01-09
#endif
#ifdef FEATURE_MG_WAPMMS_SUPPORT
      MG_MASSCARDDOWNLOAD_PATH,
#endif
   };

   if(!pMe || !pMe->m_pFileMgr)
   {
      MG_FARF(ADDR, ("Check applet prefs failed!"));
      return FALSE;
   }

   if(pMe->m_bCardExist == TRUE)
   {
      MGExplorer_CheckFolderExist(pMe->m_pFileMgr,
            pMassStorgeFolders,
            ARRAYSIZE(pMassStorgeFolders),
            TRUE);
   }

//#ifndef FEATURE_MG_WAPMMS_SUPPORT
//   MGExplorer_RemovDir(pMe->m_pFileMgr,
//                       pMe->m_pFolderList,
//                       MG_MASSCARDDOWNLOAD_PATH,
//                       TRUE);
//#endif

   return TRUE;
}//MediaGalleryApp_CheckMassStorgeDir


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_CheckHsmmDir
 * DESCRIPTION  :  检查手机中我们指定的文件夹是否存在，如果没有则创建。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MediaGalleryApp_CheckHsmmDir(CMediaGalleryApp* pMe)
{
#if 0
   AEECardInfo CardInfo, MGCfg;
   boolean bPrefRet;
#endif
   const char *pHandsetFolders[] = {
                        MG_PHONEPICTURE_PATH,
                        MG_PHONEMUSIC_PATH,
                        MG_PHONEVIDEOS_PATH,
                        //MG_PHONECALLMEMO_PATH,
#ifdef  FEATURE_MG_WAPMMS_SUPPORT
                        MG_PHONEDOWNLOAD_PATH,
#endif
   };

   if(!pMe || !pMe->m_pFileMgr)
   {
      MG_FARF(ADDR, ("Check applet prefs failed!"));
      return FALSE;
   }

   MGExplorer_CheckFolderExist(pMe->m_pFileMgr,
                        pHandsetFolders,
                        ARRAYSIZE(pHandsetFolders),
                        TRUE);

   /*防止拷贝/移动时出现不是我们指定的目录，要删除一些目录，比较保险的办法是枚
    * 举整个目录，判断每个目录*/
#ifndef FEATURE_MG_WAPMMS_SUPPORT
   MGExplorer_RemovDir(pMe->m_pFileMgr,
                       pMe->m_pFolderList,
                       MG_PHONEDOWNLOAD_PATH,
                       TRUE);
#endif

#if 0
   ISHELL_GetDeviceInfoEx(pMe->m_pShell,
                          AEE_DEVICEITEM_CARD0_INFO,
                          &CardInfo,
                          sizeof(AEECardInfo));

   bPrefRet = ISHELL_GetAppPrefs(pMe->m_pShell,
                     MG_PREFS_VER,
                     (void *)&MGCfg,
                     sizeof(MGCfg));
#endif
   return TRUE;
}
/*===========================================================================
 * FUNCTION:MediaGalleryApp_RunFSM
 * DESCRIPTION:Run the finite state machine
 * PARAMETERS:
 * RETURN VALUE:
 * ==========================================================================
 */
static void MediaGalleryApp_RunFSM(CMediaGalleryApp* pMe)
{
   NextFSMAction nextFSMAction = NFSMACTION_WAIT;

   if(!pMe)
   {
      MG_FARF(ADDR, ("MediaGalleryApp_RunFSM: bad parameter!!!"));
      return;
   }

   for(;;)
   {
      nextFSMAction = MediaGalleryApp_ProcessState(pMe);

      // We do not want to complicate the media gallery state machine and
      // hence we added one boolean to indicate whether the dlgResult should
      // be overwritten here or not. In some specific cases (see the state
      // machine design) this avoids the unnecessary complexity and eases the
      // design.
      if (pMe->m_bNotOverwriteDlgRet)
      {
         // We allow only one iteration without touching this variable. So,
         // let's reset it back to indicate that we are going to overwrite the
         // result for all subsequent iterations.
         pMe->m_bNotOverwriteDlgRet = FALSE;
      }
      else
      {
         // Overwrite the m_eDlgRet to its default value. The next dialog that
         // we are going to display will set and return the m_eDlgRet based on
         // the users action.
         pMe->m_eDlgRet = MGDLGRET_CREATE;
      }

      // If the state machine processing engine indicates that it created a
      // dialog and requesting us to exit this loop, then we quit.
      if (nextFSMAction == NFSMACTION_WAIT)
      {
         break;
      }
   }
}//MediaGalleryApp_RunFSM

/*register with the device for the removable memory status notifications
 */
static int MediaGalleryApp_RegisterMMCNotify(CMediaGalleryApp* pMe)
{
   int nRet;

   if(!pMe)
   {
      MG_FARF(ADDR, ("Bad parameter!"));
      return EFAILED;
   }

   nRet = ISHELL_RegisterNotify(pMe->m_pShell,
         AEECLSID_MEDIAGALLERY,
         AEECLSID_DEVICENOTIFIER,
         NMASK_DEVICENOTIFIER_MMC);

   if(SUCCESS != nRet)
      MG_FARF(ADDR, ("Register key notify failed!"));

   return nRet;
}//MediaGalleryApp_RegisterMMCNotify

/*===========================================================================
 * FUNCTION:    MediaGalleryApp_DeRegisterMMCNotify
 *
 * DESCRIPTION: deregister for the notification when the application no longer
 * requires the removable storage status.
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static int MediaGalleryApp_DeRegisterMMCNotify(CMediaGalleryApp* pMe)
{
   if(!pMe)
   {
      MG_FARF(ADDR, ("Bad parameter!"));
      return EFAILED;
   }

   ISHELL_RegisterNotify(pMe->m_pShell,
         AEECLSID_MEDIAGALLERY,
         AEECLSID_DEVICENOTIFIER,
         0);

   return SUCCESS;
}//MediaGalleryApp_DeRegisterMMCNotify

/*===========================================================================
 * FUNCTION:MediaGalleryApp_RegisterKeyEvent
 *
 * DESCRIPTION: If receive AVK_INFO key in EVT_KEY event, you start video play
 * and then register key event, then applet immediately EVT_NOTIFY for EVT_KEY
 * with AVK_INFO key, next are EVT_APP_SUSPEND, EVT_DIALOG_END, EVT_NOTIFY for
 * EVT_KEY_RELEASE with AVK_INFO, there are may some EVT_APP_NO_SLEEP insert.
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
int MediaGalleryApp_RegisterKeyEvent(CMediaGalleryApp *pMe)
{
   int nRet;

   if (NULL == pMe)
   {
      MG_FARF(ADDR, ("RegisterKeyEvent Bad Parameter!!"));
      return FALSE;
   }

   nRet = ISHELL_RegisterNotify(pMe->m_pShell,
         AEECLSID_MEDIAGALLERY,
         AEECLSID_SHELL,
         ( ( (uint32)(NOTIFIER_VAL_ANY) ) << 16 ) | NMASK_SHELL_KEY);

   if(SUCCESS != nRet)
      MG_FARF(ADDR, ("Register key notify failed!"));

   return nRet;
}//MediaGalleryApp_DeRegisterKeyEvent

/*===========================================================================
 * FUNCTION:    MediaGalleryApp_DeRegisterKeyEvent
 *
 * DESCRIPTION: 通知BREW，App不需要按键（key）事件通知。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
int MediaGalleryApp_DeRegisterKeyEvent(CMediaGalleryApp *pMe)
{
   int nRet;

   if (NULL == pMe)
   {
      MG_FARF(ADDR, ("DeRegisterKeyEvent, bad parameter!!"));
      return FALSE;
   }

   MG_FARF(ADDR, ("DeRegisterKeyEvent!"));

   nRet = ISHELL_RegisterNotify(pMe->m_pShell,
         AEECLSID_MEDIAGALLERY,
         AEECLSID_SHELL,
         0);

   return nRet;
}//MediaGalleryApp_DeRegisterKeyEvent


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_RegisterBatteryNotify
 * DESCRIPTION  :  通过获取充电状态，来判断USB线是否断开。此函数向BREW注册有关
 * 电池充电事件通知
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MediaGalleryApp_RegisterBatteryNotify(CMediaGalleryApp *pMe)
{
   int nRet = SUCCESS;

   if (NULL == pMe)
   {
      return FALSE;
   }

   nRet = ISHELL_RegisterNotify(pMe->m_pShell,
                                AEECLSID_MEDIAGALLERY,
                                AEECLSID_BATT_NOTIFIER,
                                NMASK_BATTNOTIFIER_EXTPWR_CHANGE);

   return nRet;
}//MediaGalleryApp_RegisterBatteryNotify


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_DeRegisterBatteryNotify
 * DESCRIPTION  :  通知BREW，APP不再需要电池的事件通知。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MediaGalleryApp_DeRegisterBatteryNotify(CMediaGalleryApp *pMe)
{
   int nRet = SUCCESS;

   if (NULL == pMe)
   {
      return FALSE;
   }

   nRet = ISHELL_RegisterNotify(pMe->m_pShell,
                                AEECLSID_MEDIAGALLERY,
                                AEECLSID_BATT_NOTIFIER,
                                0);

   return nRet;
}//MediaGalleryApp_DeRegisterBatteryNotify

/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_SetCallbackStateStart
 * DESCRIPTION  :  设置回调数据结构的状态。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */

void MediaGalleryApp_SetCallbackStateStart(CMediaGalleryApp *pMe,
                                           uint8 nDoType)
{
   if(NULL == pMe)
   {
      MG_FARF(ADDR, ("Occur NULL parameter!"));
      return;
   }

   pMe->m_bCallbackDoing = TRUE;
   pMe->m_nCallbackDoFor = nDoType;
}//MediaGalleryApp_SetCallbackStateStart


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_GetCallbackState
 * DESCRIPTION  :  获取回调数据结构的状态。
 * PARAMETERS   :
 * RETURN VALUE : refer to macro define explain
 * ==========================================================================
 */
uint8 MediaGalleryApp_GetCallbackState(CMediaGalleryApp *pMe,
                                       uint8 nInqueryType)
{
   uint8 nStatus = MG_CBS_NULL;
   uint8 nDoFor;
   boolean bDoing;
   boolean bIsQueued;

   if(NULL == pMe)
      return MG_CBS_NULL;

   bIsQueued = CALLBACK_IsQueued(&pMe->m_CallBack);
   bDoing = pMe->m_bCallbackDoing;
   nDoFor = pMe->m_nCallbackDoFor;

   if(bDoing == FALSE && bIsQueued == FALSE)
   {
      nStatus = MG_CBS_DONE;
   }
   else if(bDoing == TRUE && bIsQueued == TRUE)
   {
      nStatus = MG_CBS_DOING;
   }
   else if(bDoing == TRUE || bIsQueued == FALSE)
   {
      nStatus = MG_CBS_STOP;
   }
   else if(bDoing == FALSE)
   {
      MG_FARF(ADDR, ("Bad status!!!!"));
      return MG_CBS_DONE;
   }

   MG_FARF(ADDR,("GetCallbackState %d,bDiong %d, bIsQueued %d",
                 nStatus, bDoing, bIsQueued));

   if(nDoFor == MG_CBT_NULL)
   {
      return MG_CBS_NULL;
   }
   else if(nDoFor != nInqueryType)
   {
      if(nStatus == MG_CBS_DONE)
      {
         return MG_CBS_CANDOFOR;
      }
      else
      {
         return MG_CBS_NOTDOFOR;
      }
   }

   return nStatus;
}//MediaGalleryApp_GetCallBackState


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_SetCallbackStateStop
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MediaGalleryApp_SetCallbackStateStop(CMediaGalleryApp *pMe)
{
   boolean bIsQueued;

   if(NULL == pMe)
      return EFAILED;

   bIsQueued = CALLBACK_IsQueued(&pMe->m_CallBack);

   if(TRUE == bIsQueued)
   {
      MG_FARF(ADDR, ("call back is still in queued, cancel call back"));
      CALLBACK_Cancel(&pMe->m_CallBack);
   }
//   pMe->m_bCallbackDoing = FALSE;
   return SUCCESS;
}//MediaGalleryApp_SetCallbackStateStop


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_IsCallbackResumed
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MediaGalleryApp_IsCallbackResumed(CMediaGalleryApp *pMe)
{
   if(NULL == pMe)
      return FALSE;

   return pMe->m_bCallbackResumed;
}//MediaGalleryApp_IsCallbackResumed


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_SetCallbackResume
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MediaGalleryApp_SetCallbackResume(CMediaGalleryApp *pMe, boolean bResume)
{
   if(NULL == pMe)
      return EFAILED;

   pMe->m_bCallbackResumed = bResume;

   return SUCCESS;
}//MediaGalleryApp_SetCallbackResume


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_SetCallbackStateDone
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MediaGalleryApp_SetCallbackStateDone(CMediaGalleryApp *pMe)
{

   if(NULL == pMe)
      return EFAILED;

   MediaGalleryApp_SetCallbackStateStop(pMe);
   pMe->m_bCallbackDoing = FALSE;
   return SUCCESS;
}//MediaGalleryApp_SetCallbackStateDone


/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_HideAnnunciatorBar
 * DESCRIPTION  :  hide or show annunciator bar which indicator battery,
 * signal & various status of handset.
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MediaGallery_HideAnnunciatorBar(IDisplay *pdisp,
      AEERect *pClipRect,
      boolean bHide)
{
   IShell *psh = AEE_GetShell();
   IAnnunciator *pIAnn = NULL;
   const char *pszSetting = NULL;
   int nResult;
   AEERect rc;

   pszSetting = (const char *)(bHide == TRUE ? "a:0" : "a:1");

   nResult = ISHELL_CreateInstance(psh, AEECLSID_ANNUNCIATOR, (void**)&pIAnn);

   if(SUCCESS == nResult && NULL != pIAnn)
   {
      MG_FARF(ADDR, ("Enable annunciator bar"));
      IANNUNCIATOR_EnableAnnunciatorBar(pIAnn, AEECLSID_DISPLAY1,  (boolean)(!bHide));
   }

   IDisplay_GetClipRect(pdisp, &rc);
   MG_FARF(ADDR, ("cliprect DX:%d, DY:%d", rc.dx, rc.dy));

   /*Actually, the following statement disable annunciator*/
   IDISPLAY_SetPrefs(pdisp, pszSetting, STRLEN(pszSetting));
   /*And we also need to exteed clip rect, otherwise it just move viewport to
    * the top, and the viewport size still unchange*/
   IDISPLAY_SetClipRect(pdisp,
         (const AEERect *)(bHide == TRUE ? NULL : pClipRect));

   RELEASEIF(pIAnn);
   return TRUE;
}//MediaGallery_DisplayAnnunciatorBar



/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_SetRingVolumn
 * DESCRIPTION  :  设置铃声预览时播放的音量，与情景模式相关。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MediaGallery_SetRingVolumn(CMediaGalleryApp *pMe)
{
#ifndef AEE_SIMULATOR
   IConfig *pConfig = NULL;
   byte     nActiveProfile;
#ifndef PROFILENUMBER
#define PROFILENUMBER 5
#endif
   boolean headsetPresent;
   byte  RingVols[PROFILENUMBER];
   byte nMute = OEMSOUND_MUTE_VOL;
   byte nCurBeepVol = OEMSOUND_MUTE_VOL;
#endif
   if(NULL == pMe)
   {
      MG_FARF(ADDR, ("Bad parameter"));
      return FALSE;
   }

   if(pMe->m_nRingVolume != MG_MAX_UINT16)
   {
      return FALSE;
   }

#ifndef AEE_SIMULATOR
   if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                       AEECLSID_CONFIG,
                                       (void **)&pConfig))
   {
      MG_FARF(ADDR, ("Create config interface failed"));
      return FALSE;
   }

   ICONFIG_GetItem(pConfig, CFGI_BEEP_VOL, &nCurBeepVol, sizeof(byte));
   if(nCurBeepVol != OEMSOUND_MUTE_VOL)
   {
      ICONFIG_SetItem(pConfig, CFGI_BEEP_VOL, &nMute, sizeof(byte));
      pMe->m_nKeyBeepVol = nCurBeepVol;
   }

   if (NULL == pMe->m_pExpPara)
   {
      ICONFIG_GetItem(pConfig,
                      CFGI_PROFILE_CUR_NUMBER,
                      &nActiveProfile,
                      sizeof(nActiveProfile));
   }
   else
   {
      nActiveProfile = pMe->m_pExpPara->nSelectProfile;
   }

   ICONFIG_GetItem(pConfig,
                   CFGI_HEADSET_PRESENT,
                   &headsetPresent,
                   sizeof(boolean));

   /*if just volumn use CFGI_RINGER_VOL much convenient*/
   ICONFIG_GetItem(pConfig,
                   (headsetPresent == TRUE) ? CFGI_PROFILE_EAR_VOL :
                   CFGI_PROFILE_RINGER_VOL,
                   RingVols,
                   sizeof(RingVols));

   if(nActiveProfile >= PROFILENUMBER)
   {
      nActiveProfile = 0;
   }

   pMe->m_nRingVolume = RingVols[nActiveProfile] * AEE_MAX_VOLUME / OEMSOUND_5TH_VOL;
   RELEASEIF(pConfig);
#endif

   MG_FARF(ADDR, ("volume, %d", pMe->m_nRingVolume));

   /*当pMe->m_nRingVolume == 0 时，说明是静音模式，不要作处理。*/
   if (pMe->m_nRingVolume > AEE_MAX_VOLUME)
   {
      MG_FARF(ADDR, ("Valid volume, set to half of max"));
      pMe->m_nRingVolume = AEE_MAX_VOLUME;
   }

   return TRUE;
}//MediaGallery_SetRingVolumn


/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_TestCard0
 * DESCRIPTION  :  query the external mass memory card whether has inserted.
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
void MediaGallery_TestCard0(void *po)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)po;
   boolean bExist = FALSE;

   if(!po || !pMe->m_pFileMgr)
      return;
   MG_FARF(ADDR, ("MediaGallery_TestCard0"));
#ifdef FEATURE_SUPPORT_VC0848
   bExist = MediaGalleryApp_CheckSDCard();
   MG_FARF(ADDR, ("stop sd card to save power!"));
   MediaGalleryApp_StopSDCard();
#else
   if(SUCCESS == IFILEMGR_Test(pMe->m_pFileMgr, MG_MASSCARD_ROOTDIR))
      bExist = TRUE;
   else
      bExist = FALSE;
#endif

   pMe->m_bCardExist = bExist;

   MediaGalleryApp_CheckHsmmDir(pMe);

   if(TRUE == bExist)
   {
      MediaGalleryApp_CheckMassStorgeDir(pMe);
   }
   MediaGalleryApp_RegisterMMCNotify(pMe);
   pMe->m_bCallbackDoing = FALSE;
}//MediaGallery_TestCard0

/*===========================================================================
 * FUNCTION:MediaGalleryApp_CheckSDCard
 *
 * DESCRIPTION: query sd exist or not by call 848 API
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
#ifdef FEATURE_SUPPORT_VC0848
boolean MediaGalleryApp_CheckSDCard(void)
{
   static vc_union_type vc_data;

   VC_DeviceControl(VC_ITM_SD_STAT_I, VC_FUNC_GET_PARM, &vc_data);

   if(vc_data.sd == VC_SD_FIND_OK)
   {
      // indicate that SD exist
      return TRUE;
   }

   return FALSE;


/*    static vc_union_type vc_data;
 *
 *    VC_DeviceControl(VC_ITM_SD_STAT_I, VC_FUNC_GET_PARM, &vc_data);
 *    if(vc_data.sd == VC_SD_FIND_OK)
 *    {
 *       pMe->m_bCardExist = TRUE;
 *    }
 *    else
 *    {
 *       pMe->m_bCardExist = FALSE;
 *    }
 */


}//MediaGalleryApp_CheckSDCard
#endif//ifdef FEATURE_SUPPORT_VC0848

/*============================================================================
 * FUNCTION:MediaGalleryApp_StopSDCard
 * DESCRIPTION: request VC0848 to stop SD Card
 * RETURN VALUE:
 *============================================================================
 */
#ifdef FEATURE_SUPPORT_VC0848
void MediaGalleryApp_StopSDCard(void)
{
   vc_union_type vc_data;
   vc_data.dev_run.curr_dev = VC_DEV_SD;
   VC_DeviceControl(VC_ITM_DEV_STOP_I, VC_FUNC_CONTROL, &vc_data);
}
#endif//ifdef FEATURE_SUPPORT_VC0848

#include "rdevmap.h"
#include "hsu_conf_sel_i.h"  /* Dynamic composition switching */
#include "hsu_conf_sel_ui.h" /* Dynamic composition switching */
static hsu_conf_sel_composition_id_enum g_comp_id;
static boolean      g_bcomp_change_done;
static AEECallback  g_rdm_ui_cb;
extern void hsu_api_spoof_reconnect(void);

static void MediaGallery_UDisk_CB(rdm_assign_status_type status)
{
    //rdm_assign_status_type status = (rdm_assign_status_type)pUser;
    switch(status) 
    {
    case RDM_DONE_S:
        /* Ignore subsequent RDM_DONE_S notifications after the initial one.
           Calling hsu_conf_sel_ui_rdm_done() multiple times will result in
           ERROR FATAL.
        */
        if (TRUE == g_bcomp_change_done)
        {
            break;
        }
        
        if(hsu_conf_sel_ui_rdm_done() == TRUE)
        {
            if(hsu_conf_sel_ui_comp_change_is_done() == TRUE)
            {
                /* Successful composition change */
                g_bcomp_change_done = TRUE;
                hsu_api_spoof_reconnect();
            }
        }
        break;

    case RDM_NOT_ALLOWED_S:
    case RDM_DEVMAP_BUSY_S:
    case RDM_APP_BUSY_S:
        hsu_conf_sel_ui_rdm_event_received((uint16)status);
        break;

    default:
        break;        
    }
}

static void MediaGallery_rdm_event_cb (rdm_assign_status_type status,
                                       rdm_service_enum_type service,
                                       rdm_device_enum_type device)
{
    CALLBACK_Init(&g_rdm_ui_cb,(CallbackNotifyFunc*)MediaGallery_UDisk_CB,(void *)status);
    AEE_SYS_RESUME(&g_rdm_ui_cb);
} /* MediaGallery_rdm_event_cb */

/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_StartUDisk
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MediaGallery_StartUDisk(CMediaGalleryApp *pMe)
{
    boolean bRet = TRUE;
#ifdef FEATURE_DRV_SDCC
    
    if(!pMe)
    {
        MG_FARF(STATE, ("ERR PARAMTER"));
        return FALSE;
    }

    //if(FALSE == pMe->m_bCardExist ||
    //   FALSE == MediaGallery_CheckUSBCableConnect())
    if(FALSE == pMe->m_bCardExist) // 只要卡存在就可以启动U盘
    {
        /*if no mass storge card, do not permit to turn UDisk function on*/
        MG_FARF(STATE, ("Card do not exist"));
        return FALSE;
    }
    
    // Dynamic switch
    while(1)
    {
        const hsu_conf_sel_comp_info_type * comp_info = hsu_conf_sel_get_cur_comp_details();

        if (NULL == comp_info)
        {
            bRet = FALSE;
            break;
        }
        
        if (HSU_CONF_SEL_MS_COMP == comp_info->hsu_comp_id)
        {
            bRet = TRUE;
            break;
        }
        
        /* Check if we can switch the composition dynamically right now */
        if (!hsu_conf_sel_ui_dynamic_switch_is_allowed())
        {
            bRet = FALSE;
            break;
        }
        
        /* Save current composition.  Restore it when disconnecting the printer */
        g_comp_id = comp_info->hsu_comp_id;
        g_bcomp_change_done = FALSE;

        /* Dynamically swtich over to PictBridge composition */
        if ( !hsu_conf_sel_ui_switch_comp(HSU_CONF_SEL_MS_COMP, MediaGallery_rdm_event_cb) )
        {
            bRet = FALSE;
            break;
        }
        else
        {
            break;
        }
    }
    gbUDiskOn = bRet;
#endif	
    return bRet;
}//MediaGallery_StartUDisk

/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_StopUDisk
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 *          TRUE : Success to stop
 *          FALSE : Failed to stop UDisk
 *==========================================================================
 */
boolean MediaGallery_StopUDisk(CMediaGalleryApp *pMe)
{
    boolean bRet = TRUE;
#ifdef FEATURE_DRV_SDCC
   
    if(!pMe)
    {
        MG_FARF(STATE, ("ERR PARAMTER"));
        return FALSE;
    }

    gbUDiskOn = FALSE;
    
    // Dynamic switch
    while(1)
    {
        const hsu_conf_sel_comp_info_type * comp_info = hsu_conf_sel_get_cur_comp_details();

        if (NULL == comp_info)
        {
            bRet = FALSE;
            break;
        }
        
        if(!g_comp_id || g_comp_id == HSU_CONF_SEL_MS_COMP)
        {
            if (HSU_CONF_SEL_DIAG_MDM_COMP == comp_info->hsu_comp_id)
            {
                bRet = TRUE;
                break;
            }
            
            g_comp_id = HSU_CONF_SEL_DIAG_MDM_COMP;
        }
        
        /* Check if we can switch the composition dynamically right now */
        if (!hsu_conf_sel_ui_dynamic_switch_is_allowed())
        {
            bRet = FALSE;
            break;
        }
        
        g_bcomp_change_done = FALSE;
        /* Dynamically swtich over to PictBridge composition */
        if ( !hsu_conf_sel_ui_switch_comp(g_comp_id, MediaGallery_rdm_event_cb) )
        {
            bRet = FALSE;
            g_comp_id = 0;
            break;
        }
        else
        {
            g_comp_id = 0;
            break;
        }
    }
#endif
    return bRet;
}//MediaGallery_StopUDisk

/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_CheckUdiskStat
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 *       TRUE : UDisk function run
 *       FALSE : UDisk function stop
 * ==========================================================================
 */
boolean MediaGallery_CheckUdiskStat(void)
{
   return gbUDiskOn;
}//MediaGallery_CheckUdiskStat

/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_CheckUSBCableConnect
 * DESCRIPTION  :  检查USB数据线是否连接。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MediaGallery_CheckUSBCableConnect(void)
{
   return gbUSBCableConnet;
}//MediaGallery_CheckUSBCableConnect


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_SetUSBCableConnect
 * DESCRIPTION  :  由于Meida gallery不会在后台运行，所以开始运行时不知道USB 线
 * 连接状态，需要外部维护这个状态。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
void MediaGallery_SetUSBCableConnect(boolean bConnect)
{
   gbUSBCableConnet = bConnect;
}//MediaGallery_SetUSBCableConnect


/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_SetPlayMusicStatus
 * DESCRIPTION  :  设置音乐播放的状态。
 * PARAMETERS   :  bPlay：FALSE -- 停止播放。
 *                        TRUE  -- 出于播放。
 * RETURN VALUE :
 * ==========================================================================
 */
void MediaGallery_SetPlayMusicStatus(boolean bPlay)
{
   gbPlayeMusic = bPlay;
}//MediaGallery_SetPlayMusicStatus


/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_GetPlayMusicStatus
 * DESCRIPTION  :  获取当前是否在播放音乐。
 * PARAMETERS   :
 * RETURN VALUE :  TRUE--正在播放。
 *                 FALSE -- 不在播放。
 * ==========================================================================
 */
boolean MediaGallery_GetPlayMusicStatus(void)
{
   return gbPlayeMusic;
}//MediaGallery_GetPlayMusicStatus

/*
 * ==========================================================================
 * FUNCTION     :  MGMediaInfo_GetForderFromMimeBase
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGMediaInfo_GetForderFromMimeBase(uint16 uMimeBase,
                                      uint8 nMedium,
                                      char const ** cppszPath)
{
   const char *cpszFolder = NULL;

   if(NULL == cppszPath)
   {
      MG_FARF(ADDR, ("ERR"));
      return EFAILED;
   }

   if(MG_STMED_HANDSET == nMedium)
   {
      switch(uMimeBase)
      {
      case MG_MIME_IMGBASE:
         cpszFolder = MG_PHONEPICTURE_PATH;
         break;

      case MG_MIME_SNDBASE:
         cpszFolder = MG_PHONEMUSIC_PATH;
         break;

      case MG_MIME_CALLREC:
         cpszFolder = MG_PHONECALLMEMO_PATH;
         break;

      case MG_MIME_VIDEOBASE:
         cpszFolder = MG_PHONEVIDEOS_PATH;
         break;

      default:
         cpszFolder = MG_PHONEDOWNLOAD_PATH;
         break;
      }
   }
   else if(MG_STMED_MASSCARD == nMedium)
   {
      switch(uMimeBase)
      {
      case MG_MIME_IMGBASE:
         cpszFolder = MG_MASSCARDPICTURE_PATH;
         break;

      case MG_MIME_SNDBASE:
         cpszFolder = MG_MASSCARDMUSIC_PATH;
         break;

      case MG_MIME_VOICEREC:
         cpszFolder = MG_MASSCARDVOICEMEMO_PATH;
         break;

      case MG_MIME_VIDEOBASE:
         cpszFolder = MG_MASSCARDVIDEO_PATH;
         break;

      default:
         cpszFolder = MG_MASSCARDDOWNLOAD_PATH;
         break;
      }
   }
   else
   {
      return EFAILED;
   }

   *cppszPath = cpszFolder;
   return SUCCESS;
}


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_ShrinkString
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MGAppUtil_ShrinkString(IDisplay *pDisp,
							   int16 nrcdx,
							   AECHAR *pwszStr,
							   AECHAR **ppwszShrStr)
{
   int nMaxWidth = nrcdx;
   int nChars;
   int nStrWidth;
   int nFits;
   int nExtLen;
   int nLeftStrLen;
   AECHAR *pwszExt = NULL;
   AECHAR *pwszLeftStr = NULL;
   AECHAR *pwszNewStr = NULL;

#define MG_STRSHR_LEFTLONG 5
#define MG_STRSHR_LEFTSHORT 3
#define MG_WSTR_DOT L"..."
   if(NULL == pDisp || NULL == ppwszShrStr)
   {
      MG_FARF(ADDR, ("Bad parameter!!!"));
      return FALSE;
   }

   //nMaxWidth = pMe->m_rc.dx - 36;/*reserve 36 pixels for arrow*/

   nStrWidth = IDisplay_MeasureText(pDisp,AEE_FONT_BOLD, pwszStr);

   if(nMaxWidth >= nStrWidth)
      return FALSE;

   pwszExt = WSTRRCHR(pwszStr, MG_DOT_WCHAR);

   if(NULL != pwszExt)
      nExtLen = MG_STRSHR_LEFTLONG;
   else
      nExtLen = MG_STRSHR_LEFTSHORT;

   nChars = WSTRLEN(pwszStr);
   if(nChars < nExtLen + 1)
   {
      MG_FARF(ADDR, ("STRING TOO SHOR"));
      return FALSE;
   }

   /*Do not need substract 1, otherwise it will cause memory invalidate*/
   pwszExt = pwszStr + nChars - nExtLen/* - 1*/;
   nLeftStrLen = nExtLen + WSTRLEN(MG_WSTR_DOT);

   MG_FARF(ADDR, ("pwszExt LEN %d, DOT LEN %d",
                  WSTRLEN(pwszExt), WSTRLEN(MG_WSTR_DOT)));
   MG_FARF(ADDR, ("nLeftStrLen %d", nLeftStrLen));

   pwszLeftStr = (AECHAR *)MALLOC(sizeof(AECHAR) * (nLeftStrLen + 1));

   if(pwszLeftStr == NULL)
   {
      MG_FARF(ADDR, ("No memory to allocate"));
      return FALSE;
   }

   WSTRCPY(pwszLeftStr, MG_WSTR_DOT);
   WSTRCAT(pwszLeftStr, pwszExt);

   nStrWidth = IDisplay_MeasureText(pDisp, AEE_FONT_BOLD, pwszLeftStr);
   if(nStrWidth > nMaxWidth)
   {
      FREEIF(pwszLeftStr);
      MG_FARF(ADDR, ("RECT too small"));
      return FALSE;
   }

   nMaxWidth -= nStrWidth;

   IDisplay_MeasureTextEx(pDisp,
                          AEE_FONT_BOLD,
                          pwszStr,
                          nChars - nExtLen,
                          nMaxWidth,
                          &nFits);
   pwszNewStr = (AECHAR *)MALLOC(sizeof(AECHAR) * (nLeftStrLen + nFits + 1));

   if(pwszNewStr == NULL)
   {
      MG_FARF(ADDR, ("No memory "));
      FREEIF(pwszLeftStr);
      return FALSE;
   }

   WSTRNCOPYN(pwszNewStr, nLeftStrLen + nFits + 1, pwszStr, nFits);
   WSTRCAT(pwszNewStr, pwszLeftStr);
   *ppwszShrStr = pwszNewStr;
   FREEIF(pwszLeftStr);
   return TRUE;
}

#ifdef FEATURE_MG_INTDBG
/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_SetInterrupt
 * DESCRIPTION  :  设置一个定时器，把APP自己挂起。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static void MediaGalleryApp_SetInterrupt(CMediaGalleryApp *pMe, boolean bStop)
{
   PFNNOTIFY pfn = MediaGalleryApp_Suspend;
   AEEPromptInfo pi;
   uint16    wButtonIDs[] = { IDS_OK, 0 };

   pi.pszRes = APPSCOMMON_RES_LANG_FILE;
   pi.pTitle = NULL;
   pi.pText = NULL;
   pi.wTitleID = IDS_GALLERY;                     // IDS_ERR_BASE;
   pi.wTextID = IDS_END;
   pi.wDefBtn = IDS_OK;
   pi.pBtnIDs = wButtonIDs;
   pi.dwProps = ST_MIDDLETEXT | ST_CENTERTITLE;
   pi.fntTitle = AEE_FONT_BOLD;
   pi.fntText = AEE_FONT_NORMAL;
   pi.dwTimeout = 3000;
   if (ISHELL_Prompt(pMe->m_pShell, &pi) == FALSE)
      return;

   IDISPLAY_Update(pMe->m_pDisplay);

   if(bStop == FALSE)
      pfn = MediaGalleryApp_Resume;

   AEE_SetTimer(600, pfn, (void *)pMe);
}//MediaGalleryApp_StratInt

static void MediaGalleryApp_Suspend(void *po)
{
   MG_FARF(ADDR, ("Suspend"));
   AEE_Suspend();
   AEE_SetTimer(3000, MediaGalleryApp_Resume, (void *)po);
}

static void MediaGalleryApp_Resume(void *po)
{
   MG_FARF(ADDR, ("Resum result %d", AEE_Resume()));
}
#endif//FEATRUE_MG_INTDBG

#ifdef FEATURE_TIMER_TEST
void MediaGalleryApp_TestTimerCB(void *po)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)po;

   MG_FARF(ADDR, ("test timer expire, time is : %d", AEEOS_GetTimeMS()));
}

/*
 * ==========================================================================
 * FUNCTION     :  MeidaGalleryApp_TestTimer
 * DESCRIPTION  :  if set a timer, and then do time-consuming operation, what
 * happen, we want to know BREW how to handle.
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static void MediaGalleryApp_TestTimer(CMediaGalleryApp *pMe)
{
   int32 i;
   double fCal;

   AEE_SetTimer(300,MediaGalleryApp_TestTimerCB, (void *)pMe);
   MG_FARF(ADDR, ("Start test timer, time is : %d", AEEOS_GetTimeMS()));

/* Test Result on Simulator,
 * without cancel
 * *mediagalleryap:1675 - Start test timer, time is : 53686988
 * *mediagalleryap:1694 - End operation, time is :53688770
 * *OEMDebug.c:183 - DBGEvent=0x0 cls=0x1006124 pl=0x0
 * *AEEShell.c:3672 - #*p:ECode:102, Key:e03f
 * *mediagalleryap:1657 - test timer expire, time is : 53688770
 *
 * with timer cancel
 * *mediagalleryap:1675 - Start test timer, time is : 54024297
 * *mediagalleryap:1696 - End operation, time is :54026078
 *
 */

   /*do some operation, waste some time*/
   //AEEOS_Sleep(300);
   for(i = 0; i < 5000000; i++)
   {
      fCal = fCal / 0.3 ;
      fCal = fCal * 1000;
      fCal = fCal / 0.333;
   }

   AEE_CancelTimer(MediaGalleryApp_TestTimerCB, (void *)pMe);

   MG_FARF(ADDR, ("End operation, time is :%d", AEEOS_GetTimeMS()));

}
#endif//FEATURE_TIMER_TEST
/* ===================== End of File ===================================== */
