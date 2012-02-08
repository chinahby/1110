/*=========================================================================
FILE:  AppMgr.c

SERVICES:  BREW Application Manager Application
  
GENERAL DESCRIPTION:
    This is implementation of AppMgr/MShop as described in "BREW Core Carrier 
    Applications - Process, Features and User Interface Specifications".
    
PUBLIC CLASSES AND STATIC FUNCTIONS:
      List the class(es) and static functions included in this file
      
INITIALIZATION & SEQUENCING REQUIREMENTS:
          
              Copyright ?2000-2002 QUALCOMM Incorporated.
              All Rights Reserved.
              QUALCOMM Proprietary/GTDR
===========================================================================*/
#include "AppMgr.h"
#include "AppMgrSettings.h"
#ifdef CUST_EDITION
#include "OEMSVC.h"
#include "Appscommon.h"
#include "Msg.h"
#endif

#define FARF_BAM     1 
#define FARF(x, p)   if (1 == FARF_##x) DBGPRINTF p

//
// If SDK_COMMERCIAL_BUILD isn't defined, define FEATURE_BREW_DOWNLOAD
//
#if !defined(SDK_COMMERCIAL_BUILD) && !defined(FEATURE_BREW_DOWNLOAD)
#define FEATURE_BREW_DOWNLOAD 1
#endif // SDK_COMMERCIAL_BUILD
 
//
// Static Applet Functions - These functions need only be declared
// when an applet is being linked by the Manufacturer during their
// main build process.  They are not required for dynamically downloaded
// applets.
//
#if defined(AEE_STATIC)

static int     AppMgr_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);
#if !defined(AEE_FA_CONST)
static int     AppManager_Load(IShell *ps, void * pHelpers, IModule ** pMod);

static const AEEAppInfo    gaiAppMgr[] = {{AEECLSID_APPMGR_BID,AEE_APPMGRLS_RES_FILE,IDS_APPLICATION_MANAGER,0,0,0,0,0},
                                          {AEECLSID_MOBILESHOP_BID,AEE_APPMGRLS_RES_FILE,IDS_MOBILESHOP,0,0,0,0,AFLAG_HIDDEN|AFLAG_POPUP}};

//===========================================================================
//
// DESCRIPTION
//    Standard function called by BREW. Returns static module information  
//
// Return Value:
//    Returns the static module entry point.
// 
//===========================================================================
#if defined(AEE_SIMULATOR)
__declspec(dllexport)
#endif /* #if defined(AEE_SIMULATOR)*/
PFNMODENTRY AppManager_GetModInfo(IShell * ps,AEECLSID ** ppClasses, AEEAppInfo ** m_pApps, uint16 * pnApps,uint16 * pwMinPriv)
{
   *m_pApps = (AEEAppInfo *)&gaiAppMgr;
   *pnApps = sizeof(gaiAppMgr) / sizeof(AEEAppInfo);

   // Register wild-carded handler so that we will also process incoming URLs.
   ISHELL_RegisterHandler(ps, HTYPE_BROWSE, "cmshop*", AEECLSID_MOBILESHOP_BID);

#ifdef NMASK_SHELL_START_STATUS
   ISHELL_RegisterNotify(ps, AEECLSID_APPMGR_BID, AEECLSID_SHELL, NMASK_SHELL_START_STATUS);
#endif // NMASK_SHELL_START_STATUS

#ifdef NMASK_SHELL_SYS_ERROR
   ISHELL_RegisterNotify(ps, AEECLSID_APPMGR_BID, AEECLSID_SHELL, NMASK_SHELL_SYS_ERROR);
#endif // NMASK_SHELL_SYS_ERROR

   return((PFNMODENTRY)AppManager_Load);
}

//===========================================================================
//
// DESCRIPTION
//    Standard function called by BREW to initialize the static module.
//
// Return Value:
//    0 - Success, non-0 on failure
// 
//===========================================================================
static int AppManager_Load(IShell *ps, void * pHelpers, IModule ** pMod)
#else // AEE_FA_CONST
int AppMgr_Load(IShell *ps, void * pHelpers, IModule ** pMod)
#endif // AEE_FA_CONST
{
   return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,(PFNMODCREATEINST)AppMgr_CreateInstance,(PFNFREEMODDATA)NULL));
}

#endif   // AEE_STATIC

//===========================================================================
// 
// DESCRIPTION
//    This function is invoked while the app is being loaded. All Modules must provide this 
//    function. Ensure to retain the same name and parameters for this function.
//
// PARAMETERS
//    ClsId: [in]: Specifies the ClassID of the applet which is being loaded
//    pIShell: [in]: Contains pointer to the IShell interface. 
//    pIModule: [in]: Contains pointer to the IModule interface to the current module to which
//    this app belongs
//    ppObj: [out]: On return, *ppObj must point to a valid AEEApplet structure. Allocation
//    of memory for this structure and initializing the base data members is done by AEEApplet_New().
//
// Return Value:
//    
//===========================================================================
#if defined(AEE_STATIC)
static int AppMgr_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#endif
{
   if (ClsId == AEECLSID_APPMGR_BID)
      return AppMgrClsCreateInstance(ClsId, pIShell, po, ppObj);
#if defined(FEATURE_BREW_DOWNLOAD)
   else if (ClsId == AEECLSID_MOBILESHOP_BID)
      return MShopClsCreateInstance(ClsId, pIShell, po, ppObj);
#endif // FEATURE_BREW_DOWNLOAD
   else
      return EUNSUPPORTED;
}

//===========================================================================
//
// DESCRIPTION
//    AppMgr CreateInstance
// 
// PARAMETERS
//    ClsId:   Class Identifier
//    pIShell: IShell Pointer
//    po:      IModule Pointer
//    ppObj:   Place holder for AppMgr object
//
//===========================================================================
int AppMgrClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
{
   AppMgr *   pme; 

   if(!AEEApplet_New(sizeof(AppMgr), ClsId, pIShell,po,(IApplet**)ppObj,(AEEHANDLER)AppMgr_HandleEvent,(PFNFREEAPPDATA)AppMgr_Free))
      return ENOMEMORY;
   
   pme = (AppMgr *)*ppObj;

   if (!AppMgr_Init(pme)) {
      IAPPLET_Release((IApplet*)pme);
      *ppObj = NULL;
      return EFAILED;
   }

   return(AEE_SUCCESS);
}
//===========================================================================
//
// DESCRIPTION
//    Initializes AppMgr
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_Init(AppMgr * pme)
{
   AEEDeviceInfo  di;

   IShell* pIShell = pme->a.m_pIShell;

   // Get Prefs
   if (ISHELL_GetPrefs(pIShell, AEECLSID_APPMGR_BID, APPMGR_CFG_VER, &pme->m_cfg, sizeof(pme->m_cfg)) != SUCCESS)
   {
      // Default - Grid View        
      pme->m_cfg.bListView = FALSE;        
      pme->m_cfg.bSmallIconView = TRUE;

      // Default - configure warnings as per factory settings             
      pme->m_cfg.bBrowseWarning = OEM_SHOW_BROWSE_WARNING;
      pme->m_cfg.bSearchWarning = OEM_SHOW_SEARCH_WARNING;
      pme->m_cfg.bBuyWarning = OEM_SHOW_BUY_WARNING;
      pme->m_cfg.bRestoreWarning = OEM_SHOW_RESTORE_WARNING;
      pme->m_cfg.bUpgradeWarning = OEM_SHOW_UPGRADE_WARNING;
      pme->m_cfg.bCreditBackWarning = OEM_SHOW_CREDITBACK_WARNING;
   }

   // Creating classes which we would need later
   if (ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void**)&pme->m_pIfm) != AEE_SUCCESS)
      return FALSE;

   // Creating controls we need
   if ((ISHELL_CreateInstance(pIShell, AEECLSID_MENUCTL, (void**)&pme->m_pMenu) != AEE_SUCCESS) ||
       (ISHELL_CreateInstance(pIShell, AEECLSID_SOFTKEYCTL, (void**)&pme->m_pSK) != AEE_SUCCESS)||
       (ISHELL_CreateInstance(pIShell, pme->m_cfg.bListView ? AEECLSID_MENUCTL : AEECLSID_ICONVIEWCTL, (void**)&pme->m_pMainMenu) != AEE_SUCCESS))
       return FALSE;

   if (!pme->m_cfg.bListView && !pme->m_cfg.bSmallIconView)
   {
      if (ISHELL_CreateInstance(pIShell, AEECLSID_SOFTKEYCTL, (void**)&pme->m_pMainMenuSK) != AEE_SUCCESS)
         return FALSE;
   }

   // Device Information
   ISHELL_GetDeviceInfo(pIShell, &di);
   pme->m_nColorDepth = di.nColorDepth;
   SETAEERECT(&pme->m_rc, 0, 0, di.cxScreen, di.cyScreen);

   // Set Menu Selection History Index
   pme->m_nIndex = -1;

   // Get MShop Configuration
   if (ISHELL_GetPrefs(pme->a.m_pIShell, AEECLSID_MOBILESHOP_BID, MSHOP_CFG_VER, &pme->m_MShopCfg, sizeof(pme->m_MShopCfg)) != SUCCESS)
   {
      pme->m_MShopCfg.bSearch = OEM_ADS_SEARCH_APPS;
      pme->m_MShopCfg.bUpgrade = OEM_ADS_UPGRADE_CHECK;
   }

   // Set Menu Style
   SetMenuStyleAndColor(pme->m_pMenu, AEECLSID_MENUCTL, pme->m_nColorDepth);
#ifdef CUST_EDITION
   IMENUCTL_SetBottomBarType( pme->m_pMenu, BTBAR_SELECT_BACK);
#endif
   
   // Set Softkey Style
   SetMenuStyleAndColor(pme->m_pSK, AEECLSID_SOFTKEYCTL, pme->m_nColorDepth);

   if (pme->m_pMainMenuSK)
      // Settings specific to large icon view
      AppMgr_LargeIconViewSettings(pme);
   else
      // Set Main Menu Style
      SetMenuStyleAndColor(pme->m_pMainMenu, pme->m_cfg.bListView ? AEECLSID_MENUCTL : AEECLSID_ICONVIEWCTL, pme->m_nColorDepth);

   // Create State Machine by specifying state (index in state machine array) and
   // members of state handler struct
      
   SET_STATE(pme,ST_MAINMENU,          AppMgr_MainMenu,LAST_STATE,TRUE);
   SET_STATE(pme,ST_FAVORITES,         AppMgr_Launcher,LAST_STATE,TRUE);
   SET_STATE(pme,ST_WORK,              AppMgr_Launcher,LAST_STATE,TRUE);
   SET_STATE(pme,ST_FUN,               AppMgr_Launcher,LAST_STATE,TRUE);
   SET_STATE(pme,ST_MSHOP,             AppMgr_MShop,ST_MAINMENU,TRUE);
   SET_STATE(pme,ST_OPTIONS,           AppMgr_Options,ST_MAINMENU,TRUE);
   SET_STATE(pme,ST_HELP,              AppMgr_Help,LAST_STATE,TRUE);
   SET_STATE(pme,ST_MSHOPGUIDE,        AppMgr_MShopGuide,LAST_STATE,TRUE);
   SET_STATE(pme,ST_ORDERAPPS,         AppMgr_Categories,ST_OPTIONS,TRUE);
   SET_STATE(pme,ST_MOVEAPPS,          AppMgr_Categories,ST_OPTIONS,TRUE);
   SET_STATE(pme,ST_ORDERMAINMENU,     AppMgr_Config,LAST_STATE,TRUE);
   SET_STATE(pme,ST_ORDERFAVORITES,    AppMgr_Config,LAST_STATE,TRUE);
   SET_STATE(pme,ST_ORDERWORK,         AppMgr_Config,LAST_STATE,TRUE);
   SET_STATE(pme,ST_ORDERFUN,          AppMgr_Config,LAST_STATE,TRUE);
   SET_STATE(pme,ST_MOVEMAINMENU,      AppMgr_Config,ST_MOVEAPPS,TRUE);
   SET_STATE(pme,ST_MOVEFAVORITES,     AppMgr_Config,ST_MOVEAPPS,TRUE);
   SET_STATE(pme,ST_MOVEWORK,          AppMgr_Config,ST_MOVEAPPS,TRUE);
   SET_STATE(pme,ST_MOVEFUN,           AppMgr_Config,ST_MOVEAPPS,TRUE);
   SET_STATE(pme,ST_MOVE,              AppMgr_Categories,LAST_STATE,TRUE);
   SET_STATE(pme,ST_MANAGETOOL,        AppMgr_ManageTool,ST_OPTIONS,TRUE);
   SET_STATE(pme,ST_SYSTEMINFO,        AppMgr_SystemInfo,ST_MANAGETOOL,TRUE);
   SET_STATE(pme,ST_APPINFO,           AppMgr_AppInfo,ST_MANAGETOOL,TRUE);
   SET_STATE(pme,ST_LOCKUNLOCK,        AppMgr_LockUnlock,ST_APPINFO,TRUE);
   SET_STATE(pme,ST_REMOVE,            AppMgr_Remove,LAST_STATE,TRUE);
   SET_STATE(pme,ST_VIEWPROPERTIES,    AppMgr_ViewProperties,ST_APPINFO,TRUE);
   SET_STATE(pme,ST_EDITSOUNDIMAGE,    AppMgr_EditSoundImage,ST_APPINFO,TRUE);
   SET_STATE(pme,ST_SETCATEGORY,       AppMgr_Categories,ST_APPINFO,TRUE);
   SET_STATE(pme,ST_MAINMENUVIEW,      AppMgr_MainMenuView,ST_OPTIONS,TRUE);
   SET_STATE(pme,ST_SCREENSAVERS,      AppMgr_ScreenSavers,ST_OPTIONS,TRUE);
   SET_STATE(pme,ST_AIRTIMEWARNINGS,   AppMgr_AirtimeWarnings,ST_OPTIONS,TRUE);

   // Initilize Font
   pme->m_cyFont = IDISPLAY_GetFontMetrics(pme->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL);   
   
   // Initialize State
   pme->m_wState = NO_ENTRY;

   // Show Busy between UI states by default
   pme->m_bShowBusy = FALSE;

   // Cache Credit Sequence
   STRCPY(pme->m_pCS, CREDIT_SEQUENCE);

   if (OEM_SET_SOUND)
   {
      AEECLSID       clsPlayer;

      // MIDI Player
      clsPlayer = ISHELL_GetHandler(pIShell, HTYPE_SOUND, MIDI_EXTENSION);
      if(clsPlayer)
         pme->m_bMIDI = TRUE;

      // MP3 Player
      clsPlayer = ISHELL_GetHandler(pIShell, HTYPE_SOUND, MP3_EXTENSION);
      if(clsPlayer)
         pme->m_bMP3 = TRUE;

      // Create Sound Player Class
      if(pme->m_bMIDI || pme->m_bMP3)
         ISHELL_CreateInstance(pIShell, clsPlayer, (void **)&(pme->m_pISp));

      if(pme->m_pISp) 
         ISOUNDPLAYER_RegisterNotify(pme->m_pISp, NULL, NULL);

      // Set ISoundPlayer audio source info. 
      // eInput and dwSize do not change. Set pData to the audio file name.
      pme->m_spInfo.eInput = SDT_FILE;
   }

   pme->m_pIAnn = NULL;
   ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ANNUNCIATOR, (void **)&pme->m_pIAnn);
   return TRUE;
}

//===========================================================================
//
// DESCRIPTION
//    Frees apps cache
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static void AppMgr_FreeAppsCache(AppMgr * pme)
{
   int i;
   AppMgrItem* pList;
   AppMgrItem* p;

   // Write Config File
   AppMgr_WriteConfigFile(pme);

   // Free App Manager Items from general, favorites, work and fun lists
   for (i = 0; i < MAX_APP_CATS; i++)
   {
      pList = pme->m_pList[i];
      while(pList) {
         p = pList;
         pList = pList->pNext;
         FREE(p);
      }
      pme->m_pList[i] = 0;
      pme->m_nItem[i] = 0;
   }

   // Set app cache invalid
   pme->m_bValidCache = FALSE;
}

//===========================================================================
//
// DESCRIPTION
//    Frees all private resources
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static void AppMgr_Free(AppMgr * pme)
{
   // Set Preferences
   ISHELL_SetPrefs(pme->a.m_pIShell, AEECLSID_APPMGR_BID, APPMGR_CFG_VER, &pme->m_cfg, sizeof(pme->m_cfg));

   // Free Apps Cache
   if (pme->m_bValidCache)
      AppMgr_FreeAppsCache(pme);

   // Free IDownload
   FreeObj((void**)&pme->m_pDownload);

   // Free MShop Logo
   FreeObj((void**)&pme->m_pMShopLogo);

   // Free Animated Image
   FreeObj((void**)&pme->m_pAnimatedImage);

   // Free Classes
   FreeObj((void**)&pme->m_pIfm);
   FreeObj((void**)&pme->m_pISp);

   // Free Menus
   FreeObj((void**)&pme->m_pMainMenu);
   FreeObj((void**)&pme->m_pMenu);
   FreeObj((void**)&pme->m_pSK);
   FreeObj((void**)&pme->m_pStatic);
   FreeObj((void**)&pme->m_pMainMenuSK);

   // Free File List
   FreePtr((void**)&pme->m_pFileList);

   // Close Image Cache 
   FreeObj((void**)&pme->m_pImageCacheFile);
   if(NULL != pme->m_pIAnn)
   {
       IANNUNCIATOR_Release(pme->m_pIAnn);
       pme->m_pIAnn = NULL;
   }
}

//===========================================================================
//
// DESCRIPTION
//    Destroy's Image Cache
//
// PARAMETERS
//    pme:  AppMgr
//    
//===========================================================================
static void AppMgr_DestroyImageCache(AppMgr * pme)
{
   // Destroy Image Cache
   if (pme->m_pImageCacheFile)
      IFILE_Truncate(pme->m_pImageCacheFile, 0);
   else                  
      IFILEMGR_Remove(pme->m_pIfm, IMAGE_CACHE);
}

//===========================================================================
//
// DESCRIPTION
//    AppMgr event handler
//
// PARAMETERS
//    pme:  AppMgr
//    eCode:   event code
//    wParam:  first parameter
//    dwParam: second parameter
//
// Return Value:
//    TRUE - Event Handled
//    FALSE - Event Not Handled
//    
//===========================================================================
static boolean AppMgr_HandleEvent(AppMgr * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
#ifdef CUST_EDITION
   switch(eCode){
   case EVT_KEY:
   case EVT_KEY_PRESS:
   case EVT_KEY_RELEASE:
       switch(wParam){
       case AVK_INFO:
           wParam = AVK_SELECT;
           break;
       default:
           break;
       }
       break;
   default:
       break;
   }
#endif
   FARF(BAM, ("AppMgr_HandleEvent 0x%x 0x%x 0x%x",eCode,wParam,dwParam));
   MSG_FATAL("AppMgr_HandleEvent->eCode = %x, wParam = %x, dwParam = %x",eCode, wParam, dwParam);
   switch (eCode) 
   { 
      case EVT_NOTIFY:
         {
            AEENotify * pae = (AEENotify *)dwParam;

            if (pae)
            {
               switch (pae->dwMask)
               {
#ifdef NMASK_SHELL_START_STATUS
                  case NMASK_SHELL_START_STATUS:
                     {
                        NotifyStartStatus * ps = (NotifyStartStatus *)pae->pData;
               
                        FARF(BAM, ("Received NMASK_SHELL_START_STATUS Notification 0x%x %d",ps->cls,ps->nErr));

                        if (ps->cls && ps->nErr)
                        {
                           AEEAppInfo ai;

                           // Don't act if notification is for hidden app or a screen saver
                           ISHELL_QueryClass(pme->a.m_pIShell, ps->cls, &ai);
                           FARF(BAM, ("ISHELL_QueryClass 0x%x",ai.wFlags));
#ifdef CUST_EDITION
                           if ((ai.wFlags & AFLAG_HIDDEN) || (ai.wFlags & AFLAG_SCREENSAVER) || (ai.wFlags & AFLAG_STATIC))
#else
                           if ((ai.wFlags & AFLAG_HIDDEN) || (ai.wFlags & AFLAG_SCREENSAVER))
#endif
                              return TRUE;

                           if (ISHELL_ActiveApplet(pme->a.m_pIShell) != AEECLSID_APPMGR_BID)
                           {
                              FARF(BAM, ("Start BAM with error"));
                              pme->m_nErrCls = ps->cls;
                              pme->m_nErr = ps->nErr;
                              ISHELL_StartApplet(pme->a.m_pIShell, AEECLSID_APPMGR_BID);
                           }
                           else
                           {
                              FARF(BAM, ("Show error dialog"));
                              if(AppMgr_FindItemCategory(pme, ps->cls, &pme->m_pCurrentApp))
                                 AppMgr_HandleLaunchError(pme, ps->cls, ps->nErr);
                           }
                        }
#ifdef CUST_EDITION
                        else if(ps->cls)
                        {
                            AEEAppInfo ai;

                            // Don't act if notification is for hidden app or a screen saver
                            ISHELL_QueryClass(pme->a.m_pIShell, ps->cls, &ai);
                            
                            if (ai.wFlags & AFLAG_STATIC)
                            {
                                return TRUE;
                            }
                            
                            if(pme->m_pIAnn)
                            {
                                IANNUNCIATOR_SetFieldIsActiveEx(pme->m_pIAnn,FALSE);   
                			    IANNUNCIATOR_SetHasTitleText(pme->m_pIAnn,FALSE);
                			    IANNUNCIATOR_SetFieldText(pme->m_pIAnn,NULL);
                            }
                        }
#endif
                     }
                     return TRUE;
#endif // NMASK_SHELL_START_STATUS

#ifdef NMASK_SHELL_SYS_ERROR
                  case NMASK_SHELL_SYS_ERROR:
                     {
                        AEESysError * ps = (AEESysError *)pae->pData;
                        FARF(BAM, ("Received NMASK_SHELL_SYS_ERROR Notification"));         
                        if(ps)
                        {
#if !defined(AEE_SIMULATOR)
                           if (ps->nErr == EALLOCATED || ps->nErr >= ERROR_USER || ps->nErr < 0)
                              return TRUE;
#endif // AEE_SIMULATOR

                           if (ISHELL_ActiveApplet(pme->a.m_pIShell) != AEECLSID_APPMGR_BID)
                           {
                              FARF(BAM, ("Start BAM with error"));
                              pme->m_nErrCls = ps->cls;
                              pme->m_nErr = ps->nErr;
                              ISHELL_StartApplet(pme->a.m_pIShell, AEECLSID_APPMGR_BID);
                           }
                           else
                           {
                              FARF(BAM, ("Show error dialog"));
                              if (AppMgr_FindItemCategory(pme, ps->cls, &pme->m_pCurrentApp))
                                 return(AppMgr_HandleLaunchError(pme, ps->cls, ps->nErr));
                              else
                                 return(AppMgr_HandleSystemError(pme, ps->cls, ps->nErr, NULL));
                           }
                        }
                     }
                     return TRUE;
#endif // NMASK_SHELL_SYS_ERROR

                  default:
                     break;
               }
            }
         }
         return FALSE;

      case EVT_DIALOG_END:
         return AppMgr_SetState(pme, PREV_ENTRY);

      case EVT_APP_START:
         {
            int nErr;
            nErr = AppMgr_Start(pme);
            MSG_FATAL("EVT_APP_START",0,0,0);
   		    IANNUNCIATOR_SetFieldIsActiveEx(pme->m_pIAnn,FALSE);   
			IANNUNCIATOR_SetHasTitleText(pme->m_pIAnn,FALSE);
			IANNUNCIATOR_SetFieldText(pme->m_pIAnn,NULL);
            if (AEE_SUCCESS != nErr) {
               AEEAppStart *st = (AEEAppStart *)dwParam;
               st->error = nErr;
               return FALSE;
            } 
            else 
            {
               return TRUE;
            }
         }


      case EVT_APP_STOP:
         // Stop Sound & Animation
         AppMgr_StopSoundAnimation(pme);
		 IANNUNCIATOR_SetHasTitleText(pme->m_pIAnn,TRUE);
         return TRUE;

      case EVT_APP_SUSPEND:
         return AppMgr_Suspend(pme);

      case EVT_APP_RESUME:
         return AppMgr_Resume(pme);

      // Menu Selection Changed
      case EVT_CTL_SEL_CHANGED:
         return AppMgr_CtlSelChanged(pme, wParam, dwParam);

      case EVT_KEY:
         switch (wParam)
         {
#ifdef CUST_EDITION
			case AVK_SOFT2:
#endif
            case AVK_CLR:
               if (pme->m_bWarningDisplayed)       // Clear when airtime use warning is getting displayed
               {
                  // Reset warning displayed flag
                  pme->m_bWarningDisplayed = FALSE;
                  // Show current state UI
                  return AppMgr_SetState(pme, pme->m_wState);
               }
               else
               {
                  if ((pme->m_wState == ST_EDITSOUNDIMAGE) && IMENUCTL_IsActive(pme->m_pMenu))
                     return AppMgr_FileSelected(pme, 0);    // Previous State - No File Selected
                  else if ((pme->m_wState >= ST_ORDERMAINMENU) && (pme->m_wState <= ST_ORDERFUN) && (pme->m_wLastState == ST_OPTIONS))
                  {
                     pme->m_wState = ST_ORDERAPPS;
                     AppMgr_VisualConfirmation(pme, IDS_ORDER_CONFIRM);
                     return TRUE;
                  }
                  else
                  {
                     if (pme->m_bFreeCurrentApp)
                     {
                        FreePtr((void**)&pme->m_pCurrentApp);
                        pme->m_bFreeCurrentApp = FALSE;
                     }
                     return AppMgr_SetState(pme, PREV_ENTRY);
                  }
               }

            case AVK_LEFT:
            case AVK_RIGHT:
               // First chance to softkey
               if (IMENUCTL_IsActive(pme->m_pSK))
                  return IMENUCTL_HandleEvent(pme->m_pSK, eCode, wParam, dwParam);

               // Provide chance to main menu
               if (IMENUCTL_IsActive(pme->m_pMainMenu))
               {
                  boolean bHandled = IMENUCTL_HandleEvent(pme->m_pMainMenu, eCode, wParam, dwParam);

                  if (bHandled && pme->m_pMainMenuSK)
                     IMENUCTL_HandleEvent(pme->m_pMainMenuSK, eCode, wParam, dwParam);

                  return bHandled;
               }
               break;

            case AVK_SELECT:
               // First chance softkey
               if (IMENUCTL_IsActive(pme->m_pSK))
                  return IMENUCTL_HandleEvent(pme->m_pSK, eCode, wParam, dwParam);

               // Next General Menu
               if (IMENUCTL_IsActive(pme->m_pMenu))
                  return IMENUCTL_HandleEvent(pme->m_pMenu, eCode, wParam, dwParam);

               // Next Main Menu
               if (IMENUCTL_IsActive(pme->m_pMainMenu))
                  return IMENUCTL_HandleEvent(pme->m_pMainMenu, eCode, wParam, dwParam);
               break;

            case AVK_UP:
            case AVK_DOWN:
               // Check if we are in config menu
               if (pme->m_bConfigMenu) {
                  uint16 sel = IMENUCTL_GetSel(pme->m_pMenu);
                  IMENUCTL_MoveItem(pme->m_pMenu, sel, ((wParam == AVK_UP) ? -1 : 1));
                  AppMgr_DrawVisualIndication(pme);
                  return TRUE;
               }

               // Provide a chance to static control
               if (pme->m_pStatic)
                  return ISTATIC_HandleEvent(pme->m_pStatic, eCode, wParam, dwParam);

               // Give a chance to menu control
               if (IMENUCTL_IsActive(pme->m_pMenu)) {
                  if (IMENUCTL_HandleEvent(pme->m_pMenu, eCode, wParam, dwParam)) {
                     if (!OEM_SMALL_SCREEN_DEVICE && (pme->m_wState == ST_MANAGETOOL))
                        AppMgr_DisplayMemoryInfo(pme);
                     return(TRUE);
                  }
                  return FALSE;
               }

               // Provide a chance to main menu
               if (IMENUCTL_IsActive(pme->m_pMainMenu))
                  return IMENUCTL_HandleEvent(pme->m_pMainMenu, eCode, wParam, dwParam);
               break;
               
            case AVK_0:
            case AVK_1:
            case AVK_2:
            case AVK_3:
            case AVK_4:
            case AVK_5:
            case AVK_6:
            case AVK_7:
            case AVK_8:
            case AVK_9:
               // Allow credit back only when user is in MShop
               if ((pme->m_wState == ST_MSHOP) && (IMENUCTL_GetSel(pme->m_pMenu) == IDC_MSHOP_CATALOG))
               {
                  if ((pme->m_pCS[pme->m_nCSBufIndx] - '0') == (wParam - AVK_0))
                     pme->m_nCSBufIndx++;
                  else
                     pme->m_nCSBufIndx = 0;

                  if (pme->m_nCSBufIndx == STRLEN(CREDIT_SEQUENCE))
                  {
                     pme->m_nCSBufIndx = 0;
                     ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_APPMGR_BID, EVT_COMMAND, IDC_MSHOP_CREDITBACK, 0);
                  }

               }
               return TRUE;
         }
         return FALSE;

      case EVT_COMMAND:
        MSG_FATAL("wParam = %d",wParam,0,0);
         switch (wParam)
         {
            case IDC_OPTIONS:
               pme->m_nMenuSel[++pme->m_nIndex] = 0;
               return AppMgr_SetState(pme, ST_OPTIONS);
                        
            case IDC_MSHOP:
               pme->m_nMenuSel[++pme->m_nIndex] = 0;
               return AppMgr_SetState(pme, ST_MSHOP);
                        
            case IDC_HELP:
               pme->m_nMenuSel[++pme->m_nIndex] = 0;
               return AppMgr_SetState(pme, ST_HELP);
               
            case IDC_BREW_APPS:
            case IDC_FAVORITES:
            case IDC_WORK:
            case IDC_FUN:
               switch (pme->m_wState)
               {
                  case ST_MAINMENU:
                     pme->m_nMenuSel[++pme->m_nIndex] = 0;
                     pme->m_pCurrentApp = pme->m_pList[wParam - IDC_BREW_APPS]; 
                     return AppMgr_SetState(pme, ST_MAINMENU + wParam - IDC_BREW_APPS);

                  case ST_ORDERAPPS:                     
                     // Cache Current Menu Selection
                     pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
                     pme->m_wLastState = pme->m_wState;
                     MSG_FATAL("ST_ORDERAPPS",0,0,0);
                     return AppMgr_SetState(pme, wParam - IDC_BREW_APPS + ST_ORDERMAINMENU);

                  case ST_MOVEAPPS:                     
                     // Cache Current Menu Selection
                     MSG_FATAL("ST_MOVEAPPS",0,0,0);
                     pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
                     return AppMgr_SetState(pme, wParam - IDC_BREW_APPS + ST_MOVEMAINMENU);

                  case ST_SETCATEGORY:
                  case ST_MOVE:
                     if (pme->m_pCurrentApp && AppMgr_SetCategory(pme, pme->m_pCurrentApp->cls, wParam) == AEE_SUCCESS)
                     {
                        if (pme->m_wState == ST_MOVE)
                        {
                           pme->m_wLastState = ST_OPTIONS;
                           AppMgr_VisualConfirmation(pme, IDS_MOVE_CONFIRM);
                        }
                        else
                           AppMgr_VisualConfirmation(pme, IDS_FOLDERSET_CONFIRM);
                        return TRUE;
                     }
               }
               break;

            case IDC_CANCEL:
               return AppMgr_SetState(pme, pme->m_wState);

            case IDC_LAUNCHER_RUN_NOW:
               FARF(BAM, ("IDC_LAUNCHER_RUN_NOW"));
               return AppMgr_LaunchCurrentApplet(pme, TRUE);

            case IDC_LAUNCHER_RESTORE:
               return AppMgr_RequestMShop(pme, wParam, (boolean)pme->m_cfg.bRestoreWarning);

            case IDC_LAUNCHER_BUY:
            case IDC_LAUNCHER_BUY_NOW:
               return AppMgr_RequestMShop(pme, wParam, (boolean)pme->m_cfg.bBuyWarning);

            case IDC_LAUNCHER_VERIFY:
               return AppMgr_RequestMShop(pme, wParam, FALSE);

            case IDC_UPGRADE_CHECK:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               return AppMgr_RequestMShop(pme, wParam, (boolean)pme->m_cfg.bUpgradeWarning);

            case IDC_MSHOP_CATALOG:
               return AppMgr_RequestMShop(pme, wParam, (boolean)pme->m_cfg.bBrowseWarning);
               
            case IDC_MSHOP_FIND:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               return AppMgr_RequestMShop(pme, wParam, (boolean)pme->m_cfg.bSearchWarning);

            case IDC_MSHOP_CREDITBACK:
               return AppMgr_RequestMShop(pme, wParam, (boolean)pme->m_cfg.bCreditBackWarning);

            case IDC_MSHOP_GUIDE:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               // Set State to ST_MSHOPGUIDE
               AppMgr_SetState(pme, ST_MSHOPGUIDE);
               return TRUE;

            case IDC_MSHOPGUIDE_MSHOP:
            case IDC_MSHOPGUIDE_CATALOG:
            case IDC_MSHOPGUIDE_FIND:
            case IDC_MSHOPGUIDE_PREVIEW:
            case IDC_MSHOPGUIDE_PURCHASE_OPTIONS:
            case IDC_MSHOPGUIDE_PAYMENT:
            case IDC_MSHOPGUIDE_MEMORY_FULL:
            case IDC_MSHOPGUIDE_RESTORING_APPS:
            case IDC_MSHOPGUIDE_CAN_I_RETURN_APPS:
               pme->m_wLastState = ST_MSHOPGUIDE;
               AppMgr_HelpDisplay(pme, wParam);
               return TRUE;

            case IDC_OPTIONS_ORDER_APPS:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               return AppMgr_SetState(pme, ST_ORDERAPPS);

            case IDC_OPTIONS_MOVE_APPS:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               return AppMgr_SetState(pme, ST_MOVEAPPS);

            case IDC_OPTIONS_APP_MANAGEMENT:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               AppMgr_SetState(pme, ST_MANAGETOOL);
               return TRUE;

            case IDC_OPT_INFO:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               return AppMgr_SetState(pme, ST_SYSTEMINFO);

            case IDC_LOCK_APP:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               return AppMgr_SetState(pme, ST_LOCKUNLOCK);

            case IDC_LOCKED:
            case IDC_UNLOCKED:
               if (AppMgr_GetItemID(pme, pme->m_pCurrentApp))
               {
                  IDOWNLOAD_Lock(pme->m_pDownload, AppMgr_GetItemID(pme, pme->m_pCurrentApp), (boolean)(wParam == IDC_LOCKED));
                  AppMgr_VisualConfirmation(pme, 0);
               }
               return TRUE;

            case IDC_REMOVE_WARNING:
               return AppMgr_RemoveWarning(pme);

            case IDC_REMOVE:
               pme->m_wLastState = ST_MANAGETOOL;
               return AppMgr_SetState(pme, ST_REMOVE);

            case IDC_VIEW_PROPERTIES:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               return AppMgr_SetState(pme, ST_VIEWPROPERTIES);

            case IDC_SET_CATEGORY:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               return AppMgr_SetState(pme, ST_SETCATEGORY);

            case IDC_OPTIONS_VIEW_LOG:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               // Start App Manager as if resuming
               pme->m_bResume = pme->m_bRetain = TRUE;
               ISHELL_BrowseURL(pme->a.m_pIShell, "cmshop:Log");
               return TRUE;

            case IDC_OPTIONS_MSHOP_SERVER:
               // Reset capability flags
               pme->m_MShopCfg.bSearch = OEM_ADS_SEARCH_APPS;
               pme->m_MShopCfg.bUpgrade = OEM_ADS_UPGRADE_CHECK;
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               // Start App Manager as if resuming
               pme->m_bResume = pme->m_bRetain = TRUE;
               ISHELL_BrowseURL(pme->a.m_pIShell, "cmshop:Server");
               return TRUE;

            case IDC_OPTIONS_MAINMENU_STYLE:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               return AppMgr_SetState(pme, ST_MAINMENUVIEW);

            case IDC_LIST_VIEW:
               if (!pme->m_cfg.bListView)
               {
                  pme->m_cfg.bListView = TRUE;
                  FreeObj((void**)&pme->m_pMainMenu);
                  FreeObj((void**)&pme->m_pMainMenuSK);
                  if (ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_MENUCTL, (void**)&pme->m_pMainMenu) != AEE_SUCCESS)
                     ISHELL_CloseApplet(pme->a.m_pIShell, TRUE);
                  else
                     SetMenuStyleAndColor(pme->m_pMainMenu, AEECLSID_MENUCTL, pme->m_nColorDepth);

                  // Destroy Image Cache                  
                  if (pme->m_cfg.bSmallIconView)
                  {  
                     int i;
                     AppMgrItem * p = NULL;
                
                     AppMgr_DestroyImageCache(pme);

                     for (i = 0; i < MAX_APP_CATS; i++)
                     {
                        p = pme->m_pList[i];
                        while(p) {
                           p->nOffset = 0xffffffff;
                           p->nSize = 0;
                           p = p->pNext;
                        }
                     }
                  }
               }
               AppMgr_VisualConfirmation(pme, 0);
               return TRUE;

            case IDC_SMALL_ICON_VIEW:
               if (pme->m_cfg.bListView || !pme->m_cfg.bSmallIconView)
               {  
                  int i;
                  AppMgrItem * p = NULL;

                  pme->m_cfg.bListView = FALSE;
                  pme->m_cfg.bSmallIconView = TRUE;
                  FreeObj((void**)&pme->m_pMainMenu);
                  FreeObj((void**)&pme->m_pMainMenuSK);
                  if (ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ICONVIEWCTL, (void**)&pme->m_pMainMenu) != AEE_SUCCESS)
                     ISHELL_CloseApplet(pme->a.m_pIShell, TRUE);
                  else
                     SetMenuStyleAndColor(pme->m_pMainMenu, AEECLSID_ICONVIEWCTL, pme->m_nColorDepth);    
                  
                  AppMgr_DestroyImageCache(pme);

                  for (i = 0; i < MAX_APP_CATS; i++)
                  {
                     p = pme->m_pList[i];
                     while(p) {
                        p->nOffset = 0xffffffff;
                        p->nSize = 0;
                        p = p->pNext;
                     }
                  }
               }               
               AppMgr_VisualConfirmation(pme, 0);
               return TRUE;

            case IDC_LARGE_ICON_VIEW:
               if (pme->m_cfg.bListView || pme->m_cfg.bSmallIconView)
               {                  
                  // Destroy Image Cache if view is changing from SmallIconView  
                  if (pme->m_cfg.bSmallIconView)
                  {
                     int i;
                     AppMgrItem * p = NULL;

                     if (pme->m_pImageCacheFile)
                        IFILE_Truncate(pme->m_pImageCacheFile, 0);
                     else                  
                        IFILEMGR_Remove(pme->m_pIfm, IMAGE_CACHE);

                     for (i = 0; i < MAX_APP_CATS; i++)
                     {
                        p = pme->m_pList[i];
                        while(p) {
                           p->nOffset = 0xffffffff;
                           p->nSize = 0;
                           p = p->pNext;
                        }
                     }
                  }

                  pme->m_cfg.bListView = FALSE;
                  pme->m_cfg.bSmallIconView = FALSE;
                  FreeObj((void**)&pme->m_pMainMenu);
                  if ((ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ICONVIEWCTL, (void**)&pme->m_pMainMenu) != AEE_SUCCESS) ||
                      (ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_SOFTKEYCTL, (void**)&pme->m_pMainMenuSK) != AEE_SUCCESS))
                     ISHELL_CloseApplet(pme->a.m_pIShell, TRUE);
                  else
                     AppMgr_LargeIconViewSettings(pme);
               }
               AppMgr_VisualConfirmation(pme, 0);
               return TRUE;

            case IDC_OPTIONS_SCREEN_SAVERS:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               return AppMgr_SetState(pme, ST_SCREENSAVERS);

            case IDC_AIRTIME_WARNINGS:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               return AppMgr_SetState(pme, ST_AIRTIMEWARNINGS);

            case IDC_AIRTIME_WARNINGS_DISABLED:
               pme->m_cfg.bBrowseWarning = pme->m_cfg.bBuyWarning = pme->m_cfg.bCreditBackWarning = FALSE;
               pme->m_cfg.bRestoreWarning = pme->m_cfg.bSearchWarning = pme->m_cfg.bUpgradeWarning = FALSE;
               AppMgr_VisualConfirmation(pme, 0);
               return TRUE;

            case IDC_AIRTIME_WARNINGS_ENABLED:
               pme->m_cfg.bBrowseWarning = OEM_SHOW_BROWSE_WARNING;
               pme->m_cfg.bSearchWarning = OEM_SHOW_SEARCH_WARNING;
               pme->m_cfg.bBuyWarning = OEM_SHOW_BUY_WARNING;
               pme->m_cfg.bRestoreWarning = OEM_SHOW_RESTORE_WARNING;
               pme->m_cfg.bUpgradeWarning = OEM_SHOW_UPGRADE_WARNING;
               pme->m_cfg.bCreditBackWarning = OEM_SHOW_CREDITBACK_WARNING;
               AppMgr_VisualConfirmation(pme, 0);
               return TRUE;

            case IDC_SET_SOUND_IMAGE:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               return AppMgr_SetState(pme, ST_EDITSOUNDIMAGE);

            case IDC_CONFIGURE_DONE:
               return AppMgr_SetState(pme, PREV_ENTRY);

            case IDC_CONFIGURE_RESET:
               STRCPY(pme->m_pCurrentApp->szIcon, "");
               STRCPY(pme->m_pCurrentApp->szSound, "");
               pme->m_nSelCache = IDC_CONFIGURE_RESET;
               AppMgr_SetState(pme, ST_EDITSOUNDIMAGE);
               return TRUE;

            case IDC_CONFIGURE_IMAGE:
               AppMgr_ShowFiles(pme, TRUE);
               return TRUE;

            case IDC_CONFIGURE_SOUND:
               AppMgr_ShowFiles(pme, FALSE);
               return TRUE;

            case IDC_HELP_BUYING_NEW_APPS:
            case IDC_HELP_WHAT_DO_I_PAY:
            case IDC_HELP_AIRTIME_CHARGES:
            case IDC_HELP_CANCELING_SUBSCRIPTION:
            case IDC_HELP_DEMO_APPS:
            case IDC_HELP_REMOVING_APPS:
            case IDC_HELP_DISABLED_APPS:
            case IDC_HELP_RESTORING_APPS:
            case IDC_HELP_EXPIRED_APPS:
               pme->m_wLastState = ST_HELP;
               AppMgr_HelpDisplay(pme, wParam);
               return TRUE;

            case IDC_OK:
               if (pme->m_bWarningDisplayed) 
               {
                  if (pme->m_appEvent.wParam == IDC_MSHOP_CREDITBACK)
                  { 
                     // Show current state UI
                     AppMgr_SetState(pme, pme->m_wState);
                     // Post cached event
                     return ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_APPMGR_BID, pme->m_appEvent.evt, pme->m_appEvent.wParam, pme->m_appEvent.dwParam);
                  }
                  else
                     // Ask if user want's to see it again
                     return AppMgr_ShowAirtimeWarning(pme, IDS_AIRTIME_WARNING_AGAIN);
               }
               else
                  return AppMgr_SetState(pme, PREV_ENTRY);

            case IDC_CLOSE:
               return AppMgr_SetState(pme, pme->m_wState);

            case IDC_YES:
               if (pme->m_bWarningDisplayed)
               {
                  // Show current state UI
                  AppMgr_SetState(pme, pme->m_wState);

                  // Post cached event
                  return ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_APPMGR_BID, pme->m_appEvent.evt, pme->m_appEvent.wParam, pme->m_appEvent.dwParam);
               }
               else
               {
                  // Coming here means affirmation for app removal

                  // Request MShop to remove app
                  AppMgr_RequestMShop(pme, IDC_REMOVE, FALSE);

                  // Set Previous state as state to resume from
                  AppMgr_SetState(pme, PREV_ENTRY);
                  return TRUE;
               }

            case IDC_NO:
               if (pme->m_bFreeCurrentApp)
               {
                  FreePtr((void**)&pme->m_pCurrentApp);
                  pme->m_bFreeCurrentApp = FALSE;
               }
               AppMgr_SetState(pme, PREV_ENTRY);
               return TRUE;

            case IDC_SET_SS:
               {
                  uint32 dwItem = 0;
                  uint16 nSel = IMENUCTL_GetSel(pme->m_pMenu);

                  IMENUCTL_GetItemData(pme->m_pMenu, nSel, &dwItem);

                  // Reset Current Handler
                  if (dwItem)
                     ISHELL_RegisterHandler(pme->a.m_pIShell, AEECLSID_SCREEN_SAVER, MT_SCREEN_SAVER, 0);

                  // Set New Handler
                  ISHELL_RegisterHandler(pme->a.m_pIShell, AEECLSID_SCREEN_SAVER, MT_SCREEN_SAVER, dwItem);

                  AppMgr_VisualConfirmation(pme, 0);
               }
               return TRUE;

            case IDC_REMOVE_SS:
               {
#if defined(FEATURE_BREW_DOWNLOAD)
                  AEECLSID cls = 0;
                  IMENUCTL_GetItemData(pme->m_pMenu, IMENUCTL_GetSel(pme->m_pMenu), &cls);

                  // Reset Current Handler
                  if (cls)
                  {
                     DLITEMID dwItemID;

                     dwItemID = GetItemIDFromClassID(pme->m_pDownload, cls);

                     if (dwItemID)
                     {
                        pme->m_pCurrentApp = (AppMgrItem*)MALLOC(sizeof(AppMgrItem));
                        pme->m_bFreeCurrentApp = TRUE;

                        pme->m_pCurrentApp->cls = cls;
                        pme->m_pCurrentApp->dwItemID = dwItemID;
                     
                        pme->m_wLastState = pme->m_wState;
                        AppMgr_SetState(pme, ST_REMOVE);
                     }
                     else
                        AppMgr_VisualConfirmation(pme, IDS_PRELOADED_SS_REMOVE);
                  }
#else // FEATURE_BREW_DOWNLOAD
                        AppMgr_VisualConfirmation(pme, IDS_PRELOADED_SS_REMOVE);
#endif // FEATURE_BREW_DOWNLOAD
               }
               return TRUE;

            default:
               {
                  FARF(BAM, ("AppMgr_HandleEvent %d",wParam));
                  if(wParam >= IDC_MOVE_APP){             // Configure Move App list
                     IMENUCTL_GetItemData(pme->m_pMenu, IMENUCTL_GetSel(pme->m_pMenu), (uint32*)&pme->m_pCurrentApp);
                     pme->m_wLastState = pme->m_wState;
                     AppMgr_SetState(pme, ST_MOVE); 
                     break;
                  }
                  if(wParam >= IDC_ORDER_APP){            // Configure Order App list
                     AppMgr_Order(pme);
                     break;
                  }
                  if(wParam >= IDC_CONFIGURE_APP){        // Configure Sound/Image list
                     AppMgr_FileSelected(pme, wParam);
                     break;
                  }
                  if(wParam >= IDC_LAUNCH_APP){           // App list
                     uint32 dwItem;
                     IMENUCTL_GetItemData(pme->m_pMainMenu, wParam, &dwItem);
                     // Cache Current Menu Selection
                     pme->m_nMenuSel[++pme->m_nIndex] = wParam;
                     pme->m_pCurrentApp = (AppMgrItem*)dwItem;
                     AppMgr_LaunchCurrentApplet(pme, FALSE);
                     break;
                  }
                  if(wParam >= IDC_APP){                  // App list in options menu
                     uint32 dwItem = 0;

                     IMENUCTL_GetItemData(pme->m_pMenu, wParam, &dwItem);
                     // Cache Current Menu Selection
                     pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
                     pme->m_pCurrentApp = (AppMgrItem*)dwItem;    // Set app as current app
                     AppMgr_SetState(pme, ST_APPINFO);               // Set ST_APPINFO state
                     break;
                  }
               }
               return TRUE;
         }
   }
   return FALSE;
}

//===========================================================================
//
// DESCRIPTION
//    Delays state transition to provide a visual indication of change
// 
// PARAMETERS
//    pme:  AppMgr
//
//=========================================================================== 
void AppMgr_VisualConfirmation(AppMgr* pme, uint16 nMsgID)
{
   if (nMsgID)
   {
      // Make Sure that underlying menu's aren't active
      IMENUCTL_SetActive(pme->m_pMenu, FALSE);
      IMENUCTL_SetActive(pme->m_pSK, FALSE);

      // Display Prompt
      AppMgr_Prompt(pme, NULL, 0, NULL, nMsgID, NULL, 0, ST_NOSCROLL, TRUE);
      ISHELL_SetTimer(pme->a.m_pIShell, VISUAL_MSG_CONFIRM_TIME, AppMgr_VisualConfirmationCB, (void**)pme);
   }
   else
   {
      pme->m_bShowBusy = FALSE;
      AppMgr_SetState(pme, pme->m_wState);
      ISHELL_SetTimer(pme->a.m_pIShell, VISUAL_RB_CONFIRM_TIME, AppMgr_VisualConfirmationCB, (void**)pme);
   }
}

//===========================================================================
//
// DESCRIPTION
//    Visual Confirmation Callback
// 
// PARAMETERS
//    pme:  AppMgr
//
//=========================================================================== 
void AppMgr_VisualConfirmationCB(void* po)
{
   AppMgr* pme = (AppMgr*)po;
   pme->m_bShowBusy = FALSE;
   if (pme->m_wState == ST_SCREENSAVERS)
      AppMgr_SetState(pme, ST_SCREENSAVERS);
   else
      AppMgr_SetState(pme, PREV_ENTRY);
}

//===========================================================================
//
// DESCRIPTION
//    Handles application reordering
// 
// PARAMETERS
//    pme:  AppMgr
//
//=========================================================================== 
static void AppMgr_Order(AppMgr* pme)
{
   if (pme->m_bConfigMenu)
   {
      uint16 i, itemID;
      AppMgrItem * pTemp = NULL;
      uint16 nCount = pme->m_nItem[pme->m_wState - ST_ORDERMAINMENU];
      AppMgrItem * p = NULL;

      for(i = 0; i < nCount; i++)
      {
         // Get item ID
         itemID = IMENUCTL_GetItemID(pme->m_pMenu, i);

         // Get item data
         IMENUCTL_GetItemData(pme->m_pMenu, itemID, (uint32*)&pTemp);

         pTemp->pPrev = p;
         pTemp->pNext = NULL;

         if (p)
         {
            p->pNext = pTemp;
            p = p->pNext;
         }
         else
            pme->m_pList[pme->m_wState - ST_ORDERMAINMENU] = p = pTemp;
      }
      pme->m_bConfigMenu = FALSE;
      pme->m_wLastState = ST_OPTIONS;

      // Set New Title
      IMENUCTL_SetTitle(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDS_SELECT_APP, NULL);
      IMENUCTL_Redraw(pme->m_pMenu);
   }
   else
   {
      IMENUCTL_SetTitle(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDS_REORDER_APP, NULL);
      IMENUCTL_Redraw(pme->m_pMenu);
      AppMgr_DrawVisualIndication(pme);
      pme->m_bConfigMenu = TRUE;
   }
}

//===========================================================================
//
// DESCRIPTION
//    Draws Up/Down visual indication in Order Menu
// 
// PARAMETERS
//    pme:  AppMgr
//
//=========================================================================== 
void AppMgr_DrawVisualIndication(AppMgr* pme)
{
   IImage* pImage = ISHELL_LoadResImage(pme->a.m_pIShell, APPMGRLN_RES_FILE, IDB_UPDOWN_ARROW);

   if (pImage)
   {
      AEEImageInfo ii;
      IIMAGE_GetInfo(pImage, &ii);

      // Draw Image
      IIMAGE_Draw(pImage, (pme->m_rc.dx - ii.cx)/2, (pme->m_rc.dy - ii.cy)/2);

      // Free Image
      IIMAGE_Release(pImage);

      // Update Display
      IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
   }
}

//===========================================================================
//
// DESCRIPTION
//    Handles EVT_APP_SUSPEND. We want AppMgr to be suspended only when its
//    delegating control to hidden MShop applet. Otherwise it should return
//    FALSE to make room available to the application being launched.
// 
// PARAMETERS
//    pme:  AppMgr
//
//=========================================================================== 
static boolean AppMgr_Suspend(AppMgr* pme)
{  
   // Don't handle if this is not the result of AppManager delegating control 
   // to hidden MShop applet
   if (!pme->m_bResume)
      return FALSE;

   // Save server specific settings
   ISHELL_SetPrefs(pme->a.m_pIShell, AEECLSID_MOBILESHOP_BID, MSHOP_CFG_VER, &pme->m_MShopCfg, sizeof(pme->m_MShopCfg));

   // Reset Controls
   AppMgr_Reset(pme, FALSE);

   // Free Apps Cache if we don't need to retain it
   if (!pme->m_bRetain)
   {
      pme->m_nResumeCls = (pme->m_pCurrentApp ? pme->m_pCurrentApp->cls : 0);
      pme->m_pCurrentApp = NULL;

      // Free Apps Cache
      if (pme->m_bValidCache)
         AppMgr_FreeAppsCache(pme);

      // Free IDownload
      FreeObj((void **)(&pme->m_pDownload));
   }

   // Reset Resume Flag
   pme->m_bResume = FALSE;

   return TRUE;
}

//===========================================================================
//
// DESCRIPTION
//    Handles EVT_APP_RESUME. This would be the case only when AppMgr was 
//    suspended when it delegated control to MShop.
// 
// PARAMETERS
//    pme:  AppMgr
//
//=========================================================================== 
static boolean AppMgr_Resume(AppMgr* pme)
{
#ifdef CUST_EDITION
 {
#ifdef FEATURE_OEMOMH
    extern void OEM_SetBAM_ADSAccount(void);

    OEM_SetBAM_ADSAccount();
#elif defined(FEATURE_FLEXI_STATIC_BREW_APP)
    extern void OEM_SetBAM_ADSAccount(STATIC_BREW_APP_e eApp);
            
    OEM_SetBAM_ADSAccount(STATIC_BREW_APP_DEFAULT);
#elif defined(FEATURE_SMARTFREN_STATIC_BREW_APP)
	extern void OEM_SetBAM_ADSAccount(STATIC_BREW_APP_e eApp);
            
    OEM_SetBAM_ADSAccount(STATIC_BREW_APP_DEFAULT);
#else	 
    extern void OEM_SetBAM_ADSAccount(void);
            
    OEM_SetBAM_ADSAccount();
#endif			
 }
#endif
   // Restore server specific settings
   if (ISHELL_GetPrefs(pme->a.m_pIShell, AEECLSID_MOBILESHOP_BID, MSHOP_CFG_VER, &pme->m_MShopCfg, sizeof(pme->m_MShopCfg)) != SUCCESS)
   {
      pme->m_MShopCfg.bSearch = OEM_ADS_SEARCH_APPS;
      pme->m_MShopCfg.bUpgrade = OEM_ADS_UPGRADE_CHECK;
   }

   // Create App list if we didn't retain it
   if (!pme->m_bRetain)
   {
      // Create IDownload Instance
      if (ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_DOWNLOAD, (void**)&pme->m_pDownload) != AEE_SUCCESS)
         return FALSE;

      // Load Applets Info
      AppMgr_LoadAppletsInfo(pme);

      // Get Current Applet Pointer
      if (pme->m_nResumeCls)
         AppMgr_FindItemCategory(pme, pme->m_nResumeCls, &pme->m_pCurrentApp);
   }
   else
      pme->m_bRetain = FALSE;

   // Don't show busy
   pme->m_bShowBusy = FALSE;

   // Resume from right state
   AppMgr_SetState(pme, pme->m_wState);

   return TRUE;
}

//===========================================================================
//
// DESCRIPTION
//    Handles EVT_CTL_SEL_CHANGED
// 
// PARAMETERS
//    pme:  AppMgr
//
//=========================================================================== 
static boolean AppMgr_CtlSelChanged(AppMgr* pme, uint16 wParam, uint32 dwParam)
{
   // Get Menu
   IMenuCtl * pMenu = (IMenuCtl*)dwParam;
   boolean bHandle;

   if (pme->m_wState == ST_MAINMENU)
   {
      bHandle = (wParam == IDC_MSHOP || wParam == IDC_OPTIONS || wParam == IDC_HELP || 
                 wParam == IDC_FAVORITES || wParam == IDC_WORK || wParam == IDC_FUN || 
                 ((wParam >= IDC_LAUNCH_APP) && (wParam < IDC_SCREEN_SAVER_APP)));
   }
   else
      bHandle = ((wParam >= IDC_LAUNCH_APP) && (wParam < IDC_SCREEN_SAVER_APP));

   if (bHandle)
   {
      // Stop Sound & Animation
      if (pme->m_pISp)
         AppMgr_StopSoundAnimation(pme);

      if ((wParam >= IDC_LAUNCH_APP) && (wParam < IDC_SCREEN_SAVER_APP))
      {
         uint32 dwItem = 0;
         AppMgrItem* pi = NULL;

         IMENUCTL_GetItemData(pMenu, wParam, &dwItem);
         pi = (AppMgrItem*)dwItem;

         // Play Sound
         if (pme->m_pISp && pi && *pi->szSound)
         {
            pme->m_spInfo.pData = (void *)pi->szSound;
            AppMgr_PlayRetry(pme);
         }

#if 0 // Drawing status is expensive and slows UI
         if (pme->m_pMainMenuSK && !OEM_SMALL_SCREEN_DEVICE && pi)
         {
            AppMgr_DrawAppStatus(pme, AppMgr_AppStatus(pme, pi->dwItemID));

            // Update Display
            IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
         }
#endif
      }
   }
   return TRUE;
}

//===========================================================================
//
// DESCRIPTION
//    Removes warnings associated with the cached command and posts cached event
// 
// PARAMETERS
//    pme:  AppMgr
//
//=========================================================================== 
static boolean AppMgr_RemoveWarning(AppMgr* pme)
{
   switch (pme->m_appEvent.wParam)
   {
      case IDC_LAUNCHER_RESTORE:
         pme->m_cfg.bRestoreWarning = FALSE;               
         break;

      case IDC_LAUNCHER_BUY:
      case IDC_LAUNCHER_BUY_NOW:
         pme->m_cfg.bBuyWarning = FALSE;
         break;

      case IDC_UPGRADE_CHECK:
         pme->m_cfg.bUpgradeWarning = FALSE;
         break;

      case IDC_MSHOP_CATALOG:
         pme->m_cfg.bBrowseWarning = FALSE;
         break;
                     
      case IDC_MSHOP_FIND:
         pme->m_cfg.bSearchWarning = FALSE;
         break;
                                          
      case IDC_MSHOP_CREDITBACK:
         pme->m_cfg.bCreditBackWarning = FALSE;
         break;
   }

   // Reset Warning Displayed
   pme->m_bWarningDisplayed = FALSE;

   // Set current state UI
   AppMgr_SetState(pme, pme->m_wState);

   // Post cached event
   return ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_APPMGR_BID, pme->m_appEvent.evt, pme->m_appEvent.wParam, pme->m_appEvent.dwParam);
}

//===========================================================================
//
// DESCRIPTION
//    Handles Airtime Warning display and corresponding delegation of the
//    task to the Mobileshop applet.
// 
// PARAMETERS
//    pme:  AppMgr
//
//=========================================================================== 
static boolean AppMgr_RequestMShop(AppMgr * pme, uint16 wParam, boolean bWarning)
{
   // Check for valid current app as we would need to access its members
   switch (wParam)
   {
      case IDC_LAUNCHER_RESTORE:
      case IDC_LAUNCHER_BUY:
      case IDC_LAUNCHER_BUY_NOW:
      case IDC_UPGRADE_CHECK:
      case IDC_REMOVE:
         if (!pme->m_pCurrentApp)
            return FALSE;
   }

   if (bWarning && !pme->m_bWarningDisplayed)    // Need to display warning
   {
      uint16 wWarningID = 0;

      // Save Event
      pme->m_appEvent.evt = EVT_COMMAND;
      pme->m_appEvent.wParam = wParam;
      pme->m_appEvent.dwParam = NULL;

      switch (wParam)
      {
         case IDC_LAUNCHER_RESTORE:
            wWarningID = IDS_RESTORE_AIRTIME_WARNING;               
            break;

         case IDC_LAUNCHER_BUY:
         case IDC_LAUNCHER_BUY_NOW:
            wWarningID = IDS_BUY_AIRTIME_WARNING;
            break;

         case IDC_UPGRADE_CHECK:
            wWarningID = IDS_UPGRADE_AIRTIME_WARNING;
            break;

         case IDC_MSHOP_CATALOG:
            wWarningID = IDS_BROWSE_AIRTIME_WARNING;
            break;
                     
         case IDC_MSHOP_FIND:
            wWarningID = IDS_SEARCH_AIRTIME_WARNING;
            break;
                                          
         case IDC_MSHOP_CREDITBACK:
            wWarningID = IDS_CREDITBACK_AIRTIME_WARNING;
            break;
      }

      // Make Sure that underlying menu isn't active
      IMENUCTL_SetActive(pme->m_pMenu, FALSE);

      // Display warning
      AppMgr_ShowAirtimeWarning(pme, wWarningID);
   }
   else
   {  
      char *   pBuffer = (char*)MALLOC(MAX_STR_SIZE);      

      if (bWarning) 
         pme->m_bWarningDisplayed = FALSE;                  // Reset warning displayed flag

      if (pBuffer)
      {         
         DLITEMID dw = 0;

         MEMSET(pBuffer, 0, MAX_STR_SIZE);

         switch (wParam)
         {
            case IDC_LAUNCHER_RESTORE:
               if ((dw = AppMgr_GetItemID(pme, pme->m_pCurrentApp)) != 0)
                  SPRINTF(pBuffer, "cmshop:Restore=%u", dw);
               break;

            case IDC_LAUNCHER_BUY:
            case IDC_LAUNCHER_BUY_NOW:
               if ((dw = AppMgr_GetItemID(pme, pme->m_pCurrentApp)) != 0)
                  SPRINTF(pBuffer, "cmshop:ItemID=%u", dw);
               break;
                                          
            case IDC_LAUNCHER_VERIFY:
               if ((dw = AppMgr_GetItemID(pme, pme->m_pCurrentApp)) != 0)
                  SPRINTF(pBuffer, "cmshop:Verify=%u", dw);
               break;

            case IDC_UPGRADE_CHECK:
               pme->m_bResume = TRUE;
               if ((dw = AppMgr_GetItemID(pme, pme->m_pCurrentApp)) != 0)
                  SPRINTF(pBuffer, "cmshop:UpgradeCheck=%u", dw);               
               break;

            case IDC_REMOVE:
               if ((dw = AppMgr_GetItemID(pme, pme->m_pCurrentApp)) != 0)
               {
                  const char* cpszFormatString  = "cmshop:Remove=%u,%s";
                  char*       pszSecretString   = NULL;

                  pme->m_bResume = TRUE;

                  pszSecretString = AllocSecretString(pme->a.m_pIShell, FALSE);
                  if (pszSecretString != NULL) {
                     SNPRINTF(pBuffer, MAX_STR_SIZE, cpszFormatString, dw, pszSecretString);
                     FREEIF(pszSecretString);
                  }
               }
               if (pme->m_bFreeCurrentApp)
               {
                  // If active screen saver is being removed, reset active screen saver
                  if (pme->m_pCurrentApp->cls == ISHELL_GetHandler(pme->a.m_pIShell, AEECLSID_SCREEN_SAVER, MT_SCREEN_SAVER))
                     ISHELL_RegisterHandler(pme->a.m_pIShell, AEECLSID_SCREEN_SAVER, MT_SCREEN_SAVER, 0);
                  FreePtr((void**)&pme->m_pCurrentApp);
                  pme->m_bFreeCurrentApp = FALSE;
               }
               break;

            case IDC_MSHOP_CATALOG:
               pme->m_bResume = TRUE;
               SPRINTF(pBuffer, "cmshop:Catalog");
               break;
                     
            case IDC_MSHOP_FIND:
               pme->m_bResume = TRUE;
               SPRINTF(pBuffer, "cmshop:Search");
               break;
                                          
            case IDC_MSHOP_CREDITBACK:
               pme->m_bResume = TRUE;
               SPRINTF(pBuffer, "cmshop:CreditBack");
               break;
         }
         if (*pBuffer)
            ISHELL_BrowseURL(pme->a.m_pIShell, pBuffer);
      }                     
      FreePtr((void**)&pBuffer);
   }
   return TRUE;
}

//===========================================================================
//
// DESCRIPTION
//    Called on EVT_APP_START. Loads applet information and sets state to 
//    ST_MAINMENU.
// 
// PARAMETERS
//    pme:  AppMgr
//
//=========================================================================== 
int AppMgr_Start(AppMgr* pme)
{
   #if defined(FEATURE_BREW_DOWNLOAD)
   int nErr = ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_DOWNLOAD, (void**)&pme->m_pDownload);

   if (AEE_SUCCESS != nErr) {
      return nErr;
   }
   #endif // FEATURE_BREW_DOWNLOAD
   
#ifdef CUST_EDITION
 {
#ifdef FEATURE_OEMOMH
    extern void OEM_SetBAM_ADSAccount(void);

    OEM_SetBAM_ADSAccount();
#elif defined(FEATURE_FLEXI_STATIC_BREW_APP)
    extern void OEM_SetBAM_ADSAccount(STATIC_BREW_APP_e eApp);
            
    OEM_SetBAM_ADSAccount(STATIC_BREW_APP_DEFAULT);
#elif defined(FEATURE_SMARTFREN_STATIC_BREW_APP)
	extern void OEM_SetBAM_ADSAccount(STATIC_BREW_APP_e eApp);
            
    OEM_SetBAM_ADSAccount(STATIC_BREW_APP_DEFAULT);
#else	 
    extern void OEM_SetBAM_ADSAccount(void);
            
    OEM_SetBAM_ADSAccount();
#endif			
 }
#endif

   AppMgr_LoadAppletsInfo(pme); 
   pme->m_wState = ST_MAINMENU;   
   
   // Don't paint UI if AppMgr is started to display error
   if (!pme->m_nErrCls)
   {
      FARF(BAM, ("BAM Main Window"));
      AppMgr_SetState(pme, pme->m_wState);
   }
   else
   {
      FARF(BAM, ("BAM Error Window"));

      if (AppMgr_FindItemCategory(pme, pme->m_nErrCls, &pme->m_pCurrentApp))
         AppMgr_HandleLaunchError(pme, pme->m_nErrCls, pme->m_nErr);
      else
         AppMgr_HandleSystemError(pme, pme->m_nErrCls, pme->m_nErr, NULL);

      pme->m_nErrCls = pme->m_nErr = 0;
   }

   return AEE_SUCCESS;
}

//===========================================================================
//
// DESCRIPTION
//    Loads information about app manager items.
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static void AppMgr_LoadAppletsInfo(AppMgr * pme)
{
   // Discover all of the applets.  Here is how this is done:
   //
   // 1 - Read the configuration file.  Add an entry for each valid item.
   // 2 - Enumerate all applets and add any that were not in the file.

   // Add items in general, favorites, work and fun config files to 
   // their respective applet lists
   AppMgr_ReadConfigFile(pme);

   // Enumerate applets and add any that were not in the file.
   AppMgr_ReadApplets(pme);

   // Set flag to indicate that app cache is valid
   pme->m_bValidCache = TRUE;
}

//===========================================================================
//
// DESCRIPTION
//    State Handler for ST_FAVORITES, ST_WORK and ST_FUN
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_Launcher(AppMgr * pme)
{
   uint16         nCatIndex = pme->m_wState - ST_MAINMENU;
   uint16         nCount = pme->m_nItem[nCatIndex]; 
   AppMgrItem *   pi = pme->m_pList[nCatIndex];
   uint16         itemID = IDC_LAUNCH_APP;
   uint16         nSel = 0;
   AppImageType   nType = pme->m_cfg.bListView ? THUMB : (pme->m_cfg.bSmallIconView ? ICON : IMAGE);
   uint32         nDraw = pme->m_cfg.bListView ? (pme->m_rc.dy/APP_THUMB_Y_THRESHOLD) : pme->m_cfg.bSmallIconView ? (pme->m_rc.dy/APP_ICON_Y_THRESHOLD)*(pme->m_rc.dx/APP_ICON_X_THRESHOLD) : (pme->m_rc.dx/APP_THUMB_X_THRESHOLD); 

   // Let's not set selection for small icon view and list view so that we can make menu
   // active sooner without worrying about selection
   if (pme->m_nIndex > -1)
   {
      nSel = pme->m_nMenuSel[pme->m_nIndex--];
#ifndef CUST_EDITION
      nSel = 0;
#endif
   }

   // Set Menu Title
   IMENUCTL_SetTitle(pme->m_pMainMenu, AEE_APPMGRLS_RES_FILE, (uint16)(IDC_BREW_APPS + nCatIndex), NULL);

   // Draw Main Menu
   while (nCount)
   {
      AppMgr_AddMenuItem(pme, pme->m_pMainMenu, pi, itemID, nType, FALSE);
      
      if (pme->m_pMainMenuSK)
         AppMgr_AddMenuItem(pme, pme->m_pMainMenuSK, pi, nCount, THUMB, TRUE);

      pi = pi->pNext;
      itemID++;
      nCount--;
      
      if (nDraw != 0xffffffff)
      {
         if (!nDraw)
         {
            nDraw = 0xffffffff;

            // Set Main Menu Size
            AppMgr_SetMainMenuSize(pme);

            // Set Menus Active
            IMENUCTL_SetActive(pme->m_pMainMenu, TRUE);

            if (pme->m_pMainMenuSK)
               IMENUCTL_SetActive(pme->m_pMainMenuSK, TRUE);

            if ((nSel = IMENUCTL_GetSel(pme->m_pMainMenu)) >= IDC_LAUNCH_APP)
            {
               IMENUCTL_GetItemData(pme->m_pMainMenu, nSel, (uint32*)&pme->m_pCurrentApp);

               // Play Sound Associated with current app
               if (pme->m_pISp && *pme->m_pCurrentApp->szSound)
               {
                  pme->m_spInfo.pData = (void *)pme->m_pCurrentApp->szSound;
                  AppMgr_PlayRetry(pme);
               }
#if 0 // Drawing status is expensive and slows UI
               if (pme->m_pMainMenuSK && !OEM_SMALL_SCREEN_DEVICE)
                  AppMgr_DrawAppStatus(pme, AppMgr_AppStatus(pme, pme->m_pCurrentApp->dwItemID));
#endif
            }

            // Update Display
            IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
         }
         else
            nDraw--;
      }
   }

   if (nDraw != 0xffffffff)
   {
      // Set Main Menu Size
      AppMgr_SetMainMenuSize(pme);

      // Make Menu(s) Active
      IMENUCTL_SetActive(pme->m_pMainMenu, TRUE);

      if (pme->m_pMainMenuSK)
         IMENUCTL_SetActive(pme->m_pMainMenuSK, TRUE);

      if ((nSel = IMENUCTL_GetSel(pme->m_pMainMenu)) >= IDC_LAUNCH_APP)
      {
         IMENUCTL_GetItemData(pme->m_pMainMenu, nSel, (uint32*)&pme->m_pCurrentApp);

         // Play Sound Associated with current app
         if (pme->m_pISp && *pme->m_pCurrentApp->szSound)
         {
            pme->m_spInfo.pData = (void *)pme->m_pCurrentApp->szSound;
            AppMgr_PlayRetry(pme);
         }
#if 0 // Drawing status is expensive and slows UI
         if (pme->m_pMainMenuSK && !OEM_SMALL_SCREEN_DEVICE)
            AppMgr_DrawAppStatus(pme, AppMgr_AppStatus(pme, pme->m_pCurrentApp->dwItemID));
#endif
      }

      // Update UI
      IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
   }

   // Set Last State to ST_MAINMENU
   pme->m_wLastState = ST_MAINMENU;

   return TRUE;
}

//===========================================================================
//
// DESCRIPTION
//    State handler for ST_MSHOP
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_MShop(AppMgr * pme)
{
   AEERect rc; 
   CtlAddItem ai;
   uint32 dwADSCaps;

   // Load MShop Logo first time
   if (pme->m_pMShopLogo == NULL)
   {
      pme->m_pMShopLogo = ISHELL_LoadResImage(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDB_MSHOP_LOGO);

      if (pme->m_pMShopLogo)
      {
         AEEImageInfo ii;
         IIMAGE_GetInfo(pme->m_pMShopLogo, &ii);
         SETAEERECT(&pme->m_rcMShopLogo, (pme->m_rc.dx - ii.cx)/2, 0, ii.cx, ii.cy);
      }
   }

   // Draw MobileShop logo
   if (pme->m_pMShopLogo)
   { 
      // Set Image Background Transparent
      IIMAGE_SetParm(pme->m_pMShopLogo, IPARM_ROP, AEE_RO_TRANSPARENT, 0);

      // Draw Image
      IIMAGE_Draw(pme->m_pMShopLogo, pme->m_rcMShopLogo.x, pme->m_rcMShopLogo.y);

      // Set Menu Control Size
      SETAEERECT(&rc, 0, pme->m_rcMShopLogo.dy, pme->m_rc.dx, pme->m_rc.dy - pme->m_rcMShopLogo.dy);
      IMENUCTL_SetRect(pme->m_pMenu, &rc);
   }
   else
      IMENUCTL_SetTitle(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_MSHOP, NULL);

   // Add Items
   MEMSET(&ai, 0, sizeof(ai));
   ai.pszResText  = AEE_APPMGRLS_RES_FILE;
   ai.pszResImage = APPMGRLN_RES_FILE;

   ai.wFont    = AEE_FONT_NORMAL;

   ai.wText    = IDC_MSHOP_CATALOG;
   ai.wImage   = IDB_MS_CATALOG;
   ai.wItemID  = IDC_MSHOP_CATALOG;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   // Include Search only if: 
   // 1. we can access server capabilities and this is supported by server
   // 2. we can't access server capabilities and OEM factory settings want it
   dwADSCaps = IDOWNLOAD_GetADSCapabilities(pme->m_pDownload);
   if (dwADSCaps)
   {
      if (dwADSCaps & ADS_CAP_SEARCH)
      {
         ai.wText    = IDC_MSHOP_FIND;
         ai.wImage   = IDB_START_SEARCH;
         ai.wItemID  = IDC_MSHOP_FIND;
         IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
      }
   }
   else
   {
      if (pme->m_MShopCfg.bSearch)
      {
         ai.wText    = IDC_MSHOP_FIND;
         ai.wImage   = IDB_START_SEARCH;
         ai.wItemID  = IDC_MSHOP_FIND;
         IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
      }
   }

   ai.wText    = IDC_MSHOP_GUIDE;
   ai.wImage   = IDB_MS_HELP;
   ai.wItemID  = IDC_MSHOP_GUIDE;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   // Set menu selection
   if (pme->m_nIndex > 0)
      IMENUCTL_SetSel(pme->m_pMenu, pme->m_nMenuSel[pme->m_nIndex--]);

   // Set Menu Active
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);
   
   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);

   return(TRUE);
}
      
//===========================================================================
//
// DESCRIPTION
//    State Handler for ST_OPTIONS
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_Options(AppMgr * pme)
{
   int nCount, nIdx;
   CtlAddItem ai;

   nCount = nIdx = 0;

   // Set Menu Title
   IMENUCTL_SetTitle(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_OPTIONS, NULL);

   // Add Menu Items
   MEMSET(&ai, 0, sizeof(ai));
   ai.pszResImage = APPMGRLN_RES_FILE;
   ai.pszResText  = AEE_APPMGRLS_RES_FILE;
   ai.wFont    = AEE_FONT_NORMAL;

   // Add order apps
   ai.wText    = IDC_OPTIONS_ORDER_APPS;
   ai.wImage   = IDB_OPTIONS_ORDER_APPS;
   ai.wItemID  = IDC_OPTIONS_ORDER_APPS;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   // Add move apps
   ai.wText    = IDC_OPTIONS_MOVE_APPS;
   ai.wImage   = IDB_OPTIONS_MOVE_APPS;
   ai.wItemID  = IDC_OPTIONS_MOVE_APPS;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

#if defined(FEATURE_BREW_DOWNLOAD)
   ai.wText    = IDC_OPTIONS_APP_MANAGEMENT;
   ai.wImage   = IDB_OPTIONS_APP_MANAGEMENT;
   ai.wItemID  = IDC_OPTIONS_APP_MANAGEMENT;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
#endif // FEATURE_BREW_DOWNLOAD
   
   ai.wText    = IDC_OPTIONS_MAINMENU_STYLE;
   ai.wImage   = IDB_OPTIONS_TOOLBAR_STYLE;
   ai.wItemID  = IDC_OPTIONS_MAINMENU_STYLE;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   if (OEM_SCREEN_SAVERS)
   {
      ai.wText    = IDC_OPTIONS_SCREEN_SAVERS;
      ai.wImage   = IDB_OPTIONS_SCREEN_SAVERS;
      ai.wItemID  = IDC_OPTIONS_SCREEN_SAVERS;
      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   }

#if defined(FEATURE_BREW_DOWNLOAD)
   if (OEM_SHOW_BROWSE_WARNING||OEM_SHOW_SEARCH_WARNING||OEM_SHOW_BUY_WARNING||OEM_SHOW_RESTORE_WARNING||OEM_SHOW_UPGRADE_WARNING||OEM_SHOW_CREDITBACK_WARNING)
   {
      ai.wText    = IDC_AIRTIME_WARNINGS;
      ai.wImage   = IDB_ENABLE_AIRTIME_WARNINGS;
      ai.wItemID  = IDC_AIRTIME_WARNINGS;
      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   }
   
   ai.wText    = IDC_OPTIONS_VIEW_LOG;
   ai.wImage   = IDB_OPTIONS_VIEW_LOG;
   ai.wItemID  = IDC_OPTIONS_VIEW_LOG;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   if (OEM_CONFIGURE_TEST_DEVICE && IDOWNLOAD_GetADSList(pme->m_pDownload,&nCount,&nIdx))
   {
      ai.wText    = IDC_OPTIONS_MSHOP_SERVER;
      ai.wImage   = IDB_OPTIONS_MSHOP_SERVER;
      ai.wItemID  = IDC_OPTIONS_MSHOP_SERVER;
      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   }
#endif // FEATURE_BREW_DOWNLOAD

   // Set menu selection
   if (pme->m_nIndex > 0)
   {
      pme->m_nIndex = 1;
      IMENUCTL_SetSel(pme->m_pMenu, pme->m_nMenuSel[pme->m_nIndex--]);
   }

   // Set Menu Active
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);

   return(TRUE);
}

//===========================================================================
//
// DESCRIPTION
//    State handler for ST_HELP
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_Help(AppMgr * pme)
{
   // Set Menu Title
   IMENUCTL_SetTitle(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_HELP, NULL);

   // add menu items
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_HELP_BUYING_NEW_APPS, IDC_HELP_BUYING_NEW_APPS, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_HELP_WHAT_DO_I_PAY, IDC_HELP_WHAT_DO_I_PAY, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_HELP_AIRTIME_CHARGES, IDC_HELP_AIRTIME_CHARGES, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_HELP_CANCELING_SUBSCRIPTION, IDC_HELP_CANCELING_SUBSCRIPTION, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_HELP_DEMO_APPS, IDC_HELP_DEMO_APPS, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_HELP_REMOVING_APPS, IDC_HELP_REMOVING_APPS, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_HELP_DISABLED_APPS, IDC_HELP_DISABLED_APPS, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_HELP_RESTORING_APPS, IDC_HELP_RESTORING_APPS, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_HELP_EXPIRED_APPS, IDC_HELP_EXPIRED_APPS, NULL, NULL);

   if (pme->m_nIndex > 0)
      IMENUCTL_SetSel(pme->m_pMenu, pme->m_nMenuSel[pme->m_nIndex--]);
               
   // Draw Help Menu
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);

   // Set Last State ST_MAINMENU for simple main menu configuration
   pme->m_wLastState = ST_MAINMENU;

   return(TRUE);
}

//===========================================================================
//
// DESCRIPTION
//    State handler for ST_MAINMENU
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_MainMenu(AppMgr * pme)
{
   CtlAddItem        ai;
   uint16            nSel = 0;
   AppImageType      nType = pme->m_cfg.bListView ? THUMB : (pme->m_cfg.bSmallIconView ? ICON : IMAGE);
   uint32            nDraw = pme->m_cfg.bListView ? (pme->m_rc.dy/APP_THUMB_Y_THRESHOLD) : pme->m_cfg.bSmallIconView ? (pme->m_rc.dy/APP_ICON_Y_THRESHOLD)*(pme->m_rc.dx/APP_ICON_X_THRESHOLD) : (pme->m_rc.dx/APP_THUMB_X_THRESHOLD); 

   // Let's not set selection for small icon view and list view so that we can make menu
   // active sooner without worrying about selection
   if (pme->m_nIndex > -1)
   {
      nSel = pme->m_nMenuSel[pme->m_nIndex--];
#ifdef CUST_EDITION
      nSel = 0;
#endif
   }
   
   // Set Menu Title
   IMENUCTL_SetTitle(pme->m_pMainMenu, AEE_APPMGRLS_RES_FILE, IDS_MAIN_MENU, NULL);

   // Add the menu items.
   MEMSET(&ai, 0, sizeof(ai));

   ai.pszResText  = AEE_APPMGRLS_RES_FILE;
   ai.pszResImage = APPMGRLN_RES_FILE;


#if defined(FEATURE_BREW_DOWNLOAD)
   //if (OEM_LOCK_MOBILESHOP_LOCATION)	//Del by zzg 2012_02_08
   {
      ai.wText       = IDC_MSHOP;
      ai.wImage      = ((nType == THUMB) ? IDB_MSHOP_THUMB : ((nType == ICON) ? IDB_MSHOP_ICON : IDB_MSHOP_LARGE));
	  ai.wItemID     = IDC_MSHOP;
	  
      IMENUCTL_AddItemEx(pme->m_pMainMenu, &ai);


      if (pme->m_pMainMenuSK)
      {
         ai.wText       = NULL;
         ai.wImage      = IDB_MSHOP_THUMB;
         ai.wItemID     = pme->m_nItem[0] + IDC_MSHOP;
         IMENUCTL_AddItemEx(pme->m_pMainMenuSK, &ai);

		 
      }
      //nDraw--;	//Del by zzg 2012_02_08
   }
#endif // FEATURE_BREW_DOWNLOAD

//Del by zzg 2012_02_08
/*		
   // Show root level apps in
   if (pme->m_nItem[0])
   {
      // Draw List View
      AppMgrItem * pi = pme->m_pList[0];
      uint16 itemID = IDC_LAUNCH_APP;
      uint16 nCount = pme->m_nItem[0];
      
      while (nCount)
      {
         //uint16 nItemID = (!OEM_LOCK_MOBILESHOP_LOCATION && pi->cls == AEECLSID_MOBILESHOP_BID) ? IDC_MSHOP : itemID;
		 uint16 nItemID = IDC_WORK;

		 MSG_FATAL("***zzg AppMgr_MainMenu 3 nItemID=%d, nCount=%d***", nItemID, nCount, 0);
		 
         AppMgr_AddMenuItem(pme, pme->m_pMainMenu, pi, nItemID,  nType, FALSE);

         if (pme->m_pMainMenuSK)
         {
         	AppMgr_AddMenuItem(pme, pme->m_pMainMenuSK, pi, nCount,  THUMB, TRUE);
			MSG_FATAL("***zzg AppMgr_MainMenu 4 nCount=%d***", nCount, 0, 0);
         }

         pi = pi->pNext;
         itemID++;
         nCount--;
*/
//Del End
         if (nDraw != 0xffffffff)
         {
            if (!nDraw)
            {
               nDraw = 0xffffffff;

               // Set Main Menu Size
               AppMgr_SetMainMenuSize(pme);

               // Set Menus Active
               IMENUCTL_SetActive(pme->m_pMainMenu, TRUE);

               if (pme->m_pMainMenuSK)
                  IMENUCTL_SetActive(pme->m_pMainMenuSK, TRUE);

               if ((nSel = IMENUCTL_GetSel(pme->m_pMainMenu)) >= IDC_LAUNCH_APP)
               {
                  IMENUCTL_GetItemData(pme->m_pMainMenu, nSel, (uint32*)&pme->m_pCurrentApp);

                  // Play Sound Associated with current app
                  if (pme->m_pISp && *pme->m_pCurrentApp->szSound)
                  {
                     pme->m_spInfo.pData = (void *)pme->m_pCurrentApp->szSound;
                     AppMgr_PlayRetry(pme);
                  }
#if 0 //Drawing status is expensive and slows UI
                  if (pme->m_pMainMenuSK && !OEM_SMALL_SCREEN_DEVICE)
                     AppMgr_DrawAppStatus(pme, AppMgr_AppStatus(pme, pme->m_pCurrentApp->dwItemID));
#endif
               }

               // Update Display
               IDISPLAY_UpdateEx(pme->a.m_pIDisplay, TRUE);
            }
            else
               nDraw--;
         }
      //}	//Del by zzg 2012_02_08
   //}    	//Del by zzg 2012_02_08

   // Add Categories. Categories are added in following order
   // Favorites, Work, Fun, General

   // Add Favorites only if there are apps in favorites category
   if (pme->m_nItem[1]) {
      ai.wText       = IDC_FAVORITES;
      ai.wImage      = ((nType == THUMB) ? IDB_FAVORITES_THUMB : ((nType == ICON) ? IDB_FAVORITES_ICON : IDB_FAVORITES_LARGE));
      ai.wItemID     = IDC_FAVORITES;
      IMENUCTL_AddItemEx(pme->m_pMainMenu, &ai);

      if (pme->m_pMainMenuSK)
      {
         ai.wText       = NULL;
         ai.wImage      = IDB_FAVORITES_THUMB;
         ai.wItemID     = pme->m_nItem[0] + IDC_FAVORITES;
         IMENUCTL_AddItemEx(pme->m_pMainMenuSK, &ai);
      }
   }

   // Add Work only if there are apps in work category
   if (pme->m_nItem[2]) {
      ai.wText       = IDC_WORK;
      ai.wImage      = ((nType == THUMB) ? IDB_WORK_THUMB : ((nType == ICON) ? IDB_WORK_ICON : IDB_WORK_LARGE));
      ai.wItemID     = IDC_WORK;
      IMENUCTL_AddItemEx(pme->m_pMainMenu, &ai);

      if (pme->m_pMainMenuSK)
      {
         ai.wText       = NULL;
         ai.wImage      = IDB_WORK_THUMB;
         ai.wItemID     = pme->m_nItem[0] + IDC_WORK;
         IMENUCTL_AddItemEx(pme->m_pMainMenuSK, &ai);
      }
   }

   // Add Fun only if there are apps in fun category
   if (pme->m_nItem[3]) {
      ai.wText       = IDC_FUN;
      ai.wImage      = ((nType == THUMB) ? IDB_FUN_THUMB : ((nType == ICON) ? IDB_FUN_ICON : IDB_FUN_LARGE));
      ai.wItemID     = IDC_FUN;
      IMENUCTL_AddItemEx(pme->m_pMainMenu, &ai);

      if (pme->m_pMainMenuSK)
      {
         ai.wText       = NULL;
         ai.wImage      = IDB_FUN_THUMB;
         ai.wItemID     = pme->m_nItem[0] + IDC_FUN;
         IMENUCTL_AddItemEx(pme->m_pMainMenuSK, &ai);
      }
   }

   // Add Settings
   ai.wText       = IDC_OPTIONS;
   ai.wImage      = ((nType == THUMB) ? IDB_OPTIONS_THUMB : ((nType == ICON) ? IDB_OPTIONS_ICON : IDB_OPTIONS_LARGE));
   ai.wItemID     = IDC_OPTIONS;
   IMENUCTL_AddItemEx(pme->m_pMainMenu, &ai);

   if (pme->m_pMainMenuSK)
   {
      ai.wText       = NULL;
      ai.wImage      = IDB_OPTIONS_THUMB;
      ai.wItemID     = pme->m_nItem[0] + IDC_OPTIONS;
      IMENUCTL_AddItemEx(pme->m_pMainMenuSK, &ai);
   }

#if defined(FEATURE_BREW_DOWNLOAD)
   // Add Help
   ai.wText       = IDC_HELP;
   ai.wImage      = ((nType == THUMB) ? IDB_HELP_THUMB : ((nType == ICON) ? IDB_HELP_ICON : IDB_HELP_LARGE));
   ai.wItemID     = IDC_HELP;
   IMENUCTL_AddItemEx(pme->m_pMainMenu, &ai);

   if (pme->m_pMainMenuSK)
   {
      ai.wText       = NULL;
      ai.wImage      = IDB_HELP_THUMB;
      ai.wItemID     = pme->m_nItem[0] + IDC_HELP;
      IMENUCTL_AddItemEx(pme->m_pMainMenuSK, &ai);
   }
#endif // FEATURE_BREW_DOWNLOAD

   if (nDraw != 0xffffffff)
   {
      // Set Main Menu Size
      AppMgr_SetMainMenuSize(pme);

      // Set Menus Active
      IMENUCTL_SetActive(pme->m_pMainMenu, TRUE);

      if (pme->m_pMainMenuSK)
         IMENUCTL_SetActive(pme->m_pMainMenuSK, TRUE);
     
      if ((nSel = IMENUCTL_GetSel(pme->m_pMainMenu)) >= IDC_LAUNCH_APP)
      {
         IMENUCTL_GetItemData(pme->m_pMainMenu, nSel, (uint32*)&pme->m_pCurrentApp);

         // Play Sound Associated with current app
         if (pme->m_pISp && *pme->m_pCurrentApp->szSound)
         {
            pme->m_spInfo.pData = (void *)pme->m_pCurrentApp->szSound;
            AppMgr_PlayRetry(pme);
         }
#if 0 // Drawing status is expensive and slows UI
         if (pme->m_pMainMenuSK && !OEM_SMALL_SCREEN_DEVICE)
            AppMgr_DrawAppStatus(pme, AppMgr_AppStatus(pme, pme->m_pCurrentApp->dwItemID));
#endif
      }

      // Update Display
      IDISPLAY_UpdateEx(pme->a.m_pIDisplay, TRUE);
   }

   // Set Last State to NO_ENTRY
   pme->m_wLastState = NO_ENTRY;

   return TRUE;
}

//===========================================================================
//
// DESCRIPTION
//    Reads BREW applets
// 
// PARAMETERS
//    pme:  AppMgr
//       
//
//===========================================================================
static void AppMgr_ReadApplets(AppMgr * pme) 
{
   IShell *       pIShell = pme->a.m_pIShell;
   AEEAppInfo     ai;

#if defined(FEATURE_BREW_DOWNLOAD)   
   if (!OEM_LOCK_MOBILESHOP_LOCATION)
   {
      ISHELL_QueryClass(pme->a.m_pIShell, AEECLSID_MOBILESHOP_BID, &ai);

      // Add item to general category if its not present in any category
      if (!AppMgr_FindItemCategory(pme, ai.cls, NULL))
         AppMgr_AddItem(pme, IDC_BREW_APPS, ai.cls, 0, 0xffffffff, 0, (AECHAR*)"", "", "");
   }
#endif // FEATURE_BREW_DOWNLOAD

   // Enumerate all applets and add new in general list
   ISHELL_EnumAppletInit(pIShell);
   while(ISHELL_EnumNextApplet(pIShell, &ai)) {

      // Don't add hidden apps and screen savers
#ifdef CUST_EDITION
      if ((ai.wFlags & AFLAG_HIDDEN) || (ai.wFlags & AFLAG_SCREENSAVER) || (ai.wFlags & AFLAG_STATIC))
#else
      if ((ai.wFlags & AFLAG_HIDDEN) || (ai.wFlags & AFLAG_SCREENSAVER))
#endif
         continue;

      // Do not add self
      if (ai.cls == ISHELL_ActiveApplet(pIShell))
         continue;

      // Guard against ISHELL_EnumNextApplet() giving us info about a deleted application
      if (FALSE == ISHELL_QueryClass(pme->a.m_pIShell, ai.cls, &ai))
         continue;

      // Add item to general category if its not present in any category
      if (!AppMgr_FindItemCategory(pme, ai.cls, NULL))
         AppMgr_AddItem(pme, IDC_BREW_APPS, ai.cls, 0, 0xffffffff, 0, (AECHAR*)"", "", "");
   }
}

//===========================================================================
//
//
//
//
//  Configuration File Processing
//    
//
//
//
//
//===========================================================================


//===========================================================================
//
// DESCRIPTION
//    Read the config file and add items to category list.  This function 
//    calls AppMgr_ParseConfigFile()      
//
// PARAMETERS
//    pme         :  AppMgr
//    nCategory   :  Category
//
// RETURN VALUE
//    None     
//===========================================================================
static void AppMgr_ReadConfigFile(AppMgr * pme) 
{
   int      nErr     = SUCCESS;
   IFile*   pIFile   = 0;
   FileInfo fi;
   void*    fileBuf  = 0;
   int      size     = 0;

   //Look in root directory
   if((pIFile = IFILEMGR_OpenFile(pme->m_pIfm, CONFIG_FILE, _OFM_READ)) == NULL)
   {
      // Remove Config File
      IFILEMGR_Remove(pme->m_pIfm, CONFIG_FILE);

      // Destroy Image Cache
      AppMgr_DestroyImageCache(pme);

      return;
   }

   if( IFILE_GetInfo(pIFile, &fi) == SUCCESS) 
   {
      size = fi.dwSize;

      if (!size)
         AppMgr_DestroyImageCache(pme);
      else
      {
         // fileBuf has one extra NULL byte
         if ((fileBuf = MALLOC(size+1)) != NULL)  
         {
            if (IFILE_Read(pIFile, fileBuf, size) ==  size)
            {
               if (!STRNCMP(fileBuf, CONFIG_FILE_VER_1, STRLEN(CONFIG_FILE_VER_1)))
               {
                  nErr = AppMgr_ParseConfigFile(pme, (char*)fileBuf + STRLEN(CONFIG_FILE_VER_1) + 1, 1);
               }
               else if (!STRNCMP(fileBuf, CONFIG_FILE_VER_2, STRLEN(CONFIG_FILE_VER_2)))
               {
                  nErr = AppMgr_ParseConfigFile(pme, (char*)fileBuf + STRLEN(CONFIG_FILE_VER_2) + 1, 2);
               }
               else
               {
                  nErr = EFAILED;
               }

               if (SUCCESS != nErr)
               {
                  // Remove Config File
                  IFILEMGR_Remove(pme->m_pIfm, CONFIG_FILE);

                  // Destroy Image Cache
                  AppMgr_DestroyImageCache(pme);
               }
            }
            FREE(fileBuf);
         }
      }
   }
   IFILE_Release(pIFile);
}

//===========================================================================
//
// DESCRIPTION
// Parses recent download information. This information is provided by
// MobileShop.
//
// PARAMETERS
//    pme      :  AppMgr
//    fileBuff :  the buffer in memory that contains the entire config file
//                content, plus an ending NULL byte.
//
// RETURN VALUE
//    None
//    
//===========================================================================
static void AppMgr_ParseRecentDownloadInfo(AppMgr * pme, void* fileBuff)
{
   char  *  szCur = (char*) fileBuff;
   char  *  szPrev;
   uint32   nCls, nDLItemID;       
   AEEAppInfo ai; 

   while (szCur && *szCur)
   {
      char * pBuf = NULL;
      AppMgrItem * pmi = NULL;

      szPrev = szCur;

      szCur = STRCHR(szPrev, ITEM_SEPARATOR);

      if (!szCur)
         break;
      else
         *szCur = 0;

      pBuf = STRCHR(szPrev, FIELD_SEPARATOR);

      if (!pBuf)
         break;
      else
         *pBuf = 0;

      nCls = STRTOUL(szPrev, NULL, 16);

      szPrev += STRLEN(szPrev);

      szPrev++;

      nDLItemID = STRTOUL(szPrev, NULL, 16);

      if (AppMgr_FindItemCategory(pme, nCls, &pmi))
      {
         AppMgrItem *pNext, *pPrev;

         pNext = pmi->pNext;
         pPrev = pmi->pPrev;

         MEMSET(pmi, 0, sizeof(AppMgrItem));

         pmi->cls = nCls;
         pmi->dwItemID = nDLItemID;
         pmi->nOffset = 0xffffffff;
         pmi->pNext = pNext;
         pmi->pPrev = pPrev;
      }
      else
      {
         // Don't add hidden apps, screen savers and self 
         if (ISHELL_QueryClass(pme->a.m_pIShell, nCls, &ai) &&
             !(ai.wFlags & AFLAG_HIDDEN) && 
             !(ai.wFlags & AFLAG_SCREENSAVER) && 
#ifdef CUST_EDITION
             !(ai.wFlags & AFLAG_STATIC)  &&
#endif
              (ai.cls != ISHELL_ActiveApplet(pme->a.m_pIShell)))
         {
            AppMgr_AddItem(pme, IDC_BREW_APPS, nCls, nDLItemID, 0xffffffff, 0, (AECHAR*)"", "", "");
         }
      }
      szCur++;
   }
}

//===========================================================================
//
// DESCRIPTION
//    Parse the config file info and add to category list
//
// PARAMETERS
//    pme      :  AppMgr
//    fileBuff :  the buffer in memory that contains the entire config file
//                content, plus an ending NULL byte.
//    nCategory:  category
//
// RETURN VALUE
//    None
//    
//===========================================================================
static int AppMgr_ParseConfigFile(AppMgr* pme, void* fileBuff, int nVersion)
{
   char  *  szCur = (char*) fileBuff;
   char  *  szPrev;
   uint32   nCls, nDLItemID, nOffset, nSize;
   char     szCls[CLSID_LENGTH];
   AECHAR   wszName[MAX_APP_NAME];
   char     szIcon[MAX_FILE_NAME];
   char     szSound[MAX_FILE_NAME];
   int      len;
   int      index = 0;
   AEEAppInfo ai;

   pme->m_nLaunchCls = 0;
   if (nVersion >= 2)
   {
      MEMSET(szCls, 0, sizeof(szCls));

      //*** Extract ClassID String ***
      szPrev = szCur;

      while (szCur && *szCur != ' ')
         szCur++;

      if(!szCur)
         return EFAILED;

      //Compute and validate length of string
      len = szCur - szPrev;
      if(len > CLSID_LENGTH - 1)
         len = CLSID_LENGTH - 1;

      STRNCPY(szCls,szPrev,len);

      // convert token to hex 
      pme->m_nLaunchCls = STRTOUL(szCls, NULL, 16);

      // Pass the ','
      szCur++;

      // Pass White Space
      while (*szCur == WHITE_SPACE)
         szCur++;
   }

   while(szCur && *szCur && (index < MAX_APP_CATS))
   {
      // Reset contents of szCls, wszName, szIcon, szSound
      MEMSET(szCls, 0, sizeof(szCls));
      MEMSET(wszName, 0, sizeof(wszName));
      MEMSET(szIcon, 0, sizeof(szIcon));
      MEMSET(szSound, 0, sizeof(szSound));

      // Check for new line
      if (*szCur == NEW_LINE) {
         index++;
         szCur++;
         continue;
      }

      //*** Extract ClassID String ***
      szPrev = szCur;

      while (szCur && *szCur != FIELD_SEPARATOR)
         szCur++;

      if(!szCur)
         return EFAILED;

      //Compute and validate length of string
      len = szCur - szPrev;
      if(len > CLSID_LENGTH - 1)
         len = CLSID_LENGTH - 1;

      STRNCPY(szCls,szPrev,len);

      // convert token to hex 
      nCls = STRTOUL(szCls, NULL, 16);

      // Pass the ','
      szCur++;

      // Pass White Space
      while (*szCur == WHITE_SPACE)
         szCur++;

      //*** Extract Download Item ID String ***
      MEMSET(szCls, 0, sizeof(szCls));
      szPrev = szCur;

      while (szCur && *szCur != FIELD_SEPARATOR)
         szCur++;

      if(!szCur)
         return EFAILED;

      //Compute and validate length of string
      len = szCur - szPrev;
      if(len > CLSID_LENGTH - 1)
         len = CLSID_LENGTH - 1;

      STRNCPY(szCls,szPrev,len);

      // convert token to hex 
      nDLItemID = STRTOUL(szCls, NULL, 16);

      // Pass the ','
      szCur++;

      // Pass White Space
      while (*szCur == WHITE_SPACE)
         szCur++;

      //*** Extract Offset ID String ***
      MEMSET(szCls, 0, sizeof(szCls));
      szPrev = szCur;

      while (szCur && *szCur != FIELD_SEPARATOR)
         szCur++;

      if(!szCur)
         return EFAILED;

      //Compute and validate length of string
      len = szCur - szPrev;
      if(len > CLSID_LENGTH - 1)
         len = CLSID_LENGTH - 1;

      STRNCPY(szCls,szPrev,len);

      // convert token to hex 
      nOffset = STRTOUL(szCls, NULL, 16);

      // Pass the ','
      szCur++;

      // Pass White Space
      while (*szCur == WHITE_SPACE)
         szCur++;

      //*** Extract Image Size String ***
      MEMSET(szCls, 0, sizeof(szCls));
      szPrev = szCur;

      while (szCur && *szCur != FIELD_SEPARATOR)
         szCur++;

      if(!szCur)
         return EFAILED;

      //Compute and validate length of string
      len = szCur - szPrev;
      if(len > CLSID_LENGTH - 1)
         len = CLSID_LENGTH - 1;

      STRNCPY(szCls,szPrev,len);

      // convert token to hex 
      nSize = STRTOUL(szCls, NULL, 16);

      // Pass the ','
      szCur++;

      // Pass White Space
      while (*szCur == WHITE_SPACE)
         szCur++;

      //*** Extract Icon file name ***
      szPrev = szCur;

      while (szCur && *szCur != FIELD_SEPARATOR)
         szCur++;

      if(!szCur)
         return EFAILED;

      len = szCur - szPrev;

      if(len > MAX_FILE_NAME - 1)
         len = MAX_FILE_NAME - 1;

      STRNCPY(szIcon,szPrev,len);
      
      // Pass the ',' byte.
      szCur++;

      // Pass White Space
      while (*szCur == WHITE_SPACE)
         szCur++; 
      
      //*** Extract Sound file name ***
      szPrev = szCur;

      while (szCur && *szCur != FIELD_SEPARATOR)
         szCur++;

      if(!szCur)
         return EFAILED;

      len = szCur - szPrev;

      if(len > MAX_FILE_NAME - 1)
         len = MAX_FILE_NAME - 1;

      STRNCPY(szSound,szPrev,len);
      
      // Pass the ',' byte.
      szCur++;

      // Pass White Space
      while (*szCur == WHITE_SPACE)
         szCur++; 

      //*** Extract App Name String ***
      szPrev = szCur;

      while (szCur && *szCur != ITEM_SEPARATOR)
         szCur++;

      if(!szCur)
         return EFAILED;

      //Compute and validate length of string
      len = szCur - szPrev;
      if(len > 2*(MAX_APP_NAME - 1))
         len = 2*(MAX_APP_NAME - 1) ;

      MEMCPY(wszName,szPrev,len);

      if (ISHELL_QueryClass(pme->a.m_pIShell, nCls, &ai))
      {
#if defined(FEATURE_BREW_DOWNLOAD)
         if (nCls == AEECLSID_MOBILESHOP_BID)
            AppMgr_AddItem(pme, (uint16)(index+IDC_BREW_APPS), (AEECLSID) nCls, (DLITEMID) nDLItemID, nOffset, nSize, wszName, szIcon, szSound);
#endif // FEATURE_BREW_DOWNLOAD         
         // Don't add hidden apps, screen savers and self
         if ((nCls != AEECLSID_MOBILESHOP_BID) &&
             !(ai.wFlags & AFLAG_HIDDEN) && 
             !(ai.wFlags & AFLAG_SCREENSAVER) && 
#ifdef CUST_EDITION
             !(ai.wFlags & AFLAG_STATIC)  &&
#endif
              (ai.cls != ISHELL_ActiveApplet(pme->a.m_pIShell)))
         {
            AppMgr_AddItem(pme, (uint16)(index+IDC_BREW_APPS), (AEECLSID) nCls, (DLITEMID) nDLItemID, nOffset, nSize, wszName, szIcon, szSound);
         }
      }

      // Pass the ';'
      szCur++;
   }

   if ((index == MAX_APP_CATS) && szCur && *szCur)
      AppMgr_ParseRecentDownloadInfo(pme, szCur);

   return SUCCESS;
}




//===========================================================================
//
// DESCRIPTION
//    Write the info of category list to the config file.  
//
// PARAMETERS
//    pme         :  AppMgr
//    nCategory   :  Category
//
//    
//===========================================================================
static void AppMgr_WriteConfigFile(AppMgr * pme) 
{
   IFile *        pIFile = NULL;
   char *         pBuffer = NULL;
   int            i, nCount, index;
   uint32         nBuffer = 0, nSize = 0;
   AppMgrItem *   pList;

   // Try to open existing file
   pIFile = IFILEMGR_OpenFile(pme->m_pIfm, CONFIG_FILE, _OFM_READWRITE);

   if(!pIFile)
   {
      // Failed - try to create file
      pIFile = IFILEMGR_OpenFile(pme->m_pIfm, CONFIG_FILE, _OFM_CREATE);

      //All options failed. just return
      if(!pIFile)
      {
         AppMgr_DestroyImageCache(pme);
         return;
      }
   }

   for (index = 0; index < MAX_APP_CATS; index++)
   {
      nSize += ((4*CLSID_LENGTH + 2*MAX_FILE_NAME + 2*MAX_APP_NAME + 3)*pme->m_nItem[index]);
      nSize++; // For NEW_LINE
   }

   nSize++; // For NULL

   nSize += (STRLEN(CONFIG_FILE_VER_2) + 1); // For version string and space char

   nSize += CLSID_LENGTH; // For launched class ID and space char

   nSize = ((nSize + 3)/4)*4; // Make nSize a 4 multiple

   pBuffer = MALLOC(nSize);

   if (pBuffer)
   {
      IFILE_Truncate(pIFile, 0); // reset the file
      
      SNPRINTF(pBuffer + nBuffer, nSize - nBuffer, "%s ", CONFIG_FILE_VER_2);
      nBuffer += STRLEN(pBuffer + nBuffer);

      SNPRINTF(pBuffer + nBuffer, nSize - nBuffer, "0x%lx ", pme->m_nLaunchCls);
      nBuffer += STRLEN(pBuffer + nBuffer);

      for (index = 0; index < MAX_APP_CATS; index++)
      {
         nCount = pme->m_nItem[index];
         pList = pme->m_pList[index];
            
         for(i = 0; i < nCount; i++) {
            SNPRINTF(pBuffer + nBuffer, nSize - nBuffer, "0x%lx,0x%lx,0x%lx,0x%lx,%s,%s,", pList->cls, pList->dwItemID, pList->nOffset, pList->nSize, pList->szIcon, pList->szSound);
            nBuffer += STRLEN(pBuffer + nBuffer);
            MEMCPY(pBuffer + nBuffer, pList->wszName, 2*WSTRLEN(pList->wszName));
            nBuffer += 2*WSTRLEN(pList->wszName);
            SNPRINTF(pBuffer + nBuffer, nSize - nBuffer, ";");
            nBuffer++;          
            pList = pList->pNext;
         }
         SNPRINTF(pBuffer + nBuffer, nSize - nBuffer, "%s", "\n");
         nBuffer += STRLEN("\n");
      }

      // write to file
      if (IFILE_Write(pIFile, pBuffer, nBuffer) != nBuffer)
      {
         IFILE_Truncate(pIFile, 0); // reset the file 
         AppMgr_DestroyImageCache(pme); // Destroy Image Cache
      }

      FreePtr((void**)&pBuffer);
   }
   IFILE_Release(pIFile);
}

//===========================================================================
//
// DESCRIPTION
//    Add an applet item to the config list 
//
// PARAMETERS
//    pme      :  AppMgr
//    cls      :  the applet class ID
//    dwKey    :  key associated with the applet
//    nCategory:  applet category
//
// Return Value:
//    AEE_SUCCESS: if successful
//    EFAILED or ENOMEMORY: if failed
//
//===========================================================================
static int AppMgr_AddItem(AppMgr * pme, uint16 nCategory, AEECLSID cls, uint32 dwItemID, uint32 nOffset, uint32 nSize, AECHAR * pName, char* pszIcon, char* pszSound)
{
   AppMgrItem **  pList = NULL;
   AppMgrItem *   p = NULL;
   uint16*        pCount = NULL;
   uint16         nCatIndex;

   nCatIndex = nCategory - IDC_BREW_APPS;

   pCount = &pme->m_nItem[nCatIndex];
   pList = &pme->m_pList[nCatIndex];

   // If the item was not already added, add it to the list
   if(!AppMgr_FindItem(pme, cls, nCatIndex, &p))
   {
      AppMgrItem* pNewItem = (AppMgrItem*)MALLOC(sizeof(AppMgrItem));

      if (!pNewItem)
         return ENOMEMORY;

      MEMSET(pNewItem, 0, sizeof(AppMgrItem));

      pNewItem->cls = cls;
      pNewItem->dwItemID = dwItemID;
      pNewItem->nOffset = nOffset;
      pNewItem->nSize = nSize;
      if(pszIcon)
         STRCPY(pNewItem->szIcon, pszIcon);
      if(pszSound)
         STRCPY(pNewItem->szSound, pszSound);
      pNewItem->pPrev = p;

      if (p)
         p->pNext = pNewItem;
      else
         *pList = pNewItem; // First Item

      (*pCount)++;
   }
   return AEE_SUCCESS;
}

//===========================================================================
//
// DESCRIPTION
//    Sets applet category and moves applet item from the config list of old 
//    category to the config list of another category.
//
// PARAMETERS
//    pme:     AppMgr
//    cls:     the applet class ID
//    nAdd: new category
//
// Return Value:
//    AEE_SUCCESS
//    EFAILED
//
//===========================================================================
static int AppMgr_SetCategory(AppMgr * pme, AEECLSID cls, uint16 nCategory)
{
   AppMgrItem *   pRem = NULL;
   AppMgrItem **  pRemList = NULL;
   uint16*        pRemCount = NULL;

   AppMgrItem *   pAdd = NULL;
   AppMgrItem **  pAddList = NULL;
   uint16*        pAddCount = NULL;

   uint16 nPresent = AppMgr_FindItemCategory(pme, cls, &pRem);

   if (!nPresent)
      return EFAILED;

   // If both the categories are same, claim success
   if (nPresent == nCategory)
      return AEE_SUCCESS;

   pAddCount = &pme->m_nItem[nCategory-IDC_BREW_APPS];
   pAddList = &pme->m_pList[nCategory-IDC_BREW_APPS];

   pRemCount = &pme->m_nItem[nPresent-IDC_BREW_APPS];
   pRemList = &pme->m_pList[nPresent-IDC_BREW_APPS];
   
   // Take out item from old list
   if (pRem->pPrev)
   {
      (pRem->pPrev)->pNext = pRem->pNext;
      if (pRem->pNext)
         (pRem->pNext)->pPrev = pRem->pPrev;
   }
   else
   {
      // Case when pRem is first node
      *pRemList = pRem->pNext;
      if (pRem->pNext)
         (pRem->pNext)->pPrev = NULL;
   }

   --(*pRemCount);   // Reduce count

   pRem->pPrev = pRem->pNext = NULL; // Clean Pointers

   if (*pAddList)
   {
      pAdd = *pAddList; // Assign First Node

      while(pAdd && pAdd->pNext)    // Find last node
         pAdd = pAdd->pNext;

      pAdd->pNext = pRem; // Add pRem in the list
      pRem->pPrev = pAdd;
   }
   else
      *pAddList = pRem; // List is empty. Make pRem first node

   ++(*pAddCount);   // Increase count

   return AEE_SUCCESS;
}

//===========================================================================
// 
// DESCRIPTION
//    Finds cls item in the specified list. If item is found, item index is
//    returned. Otherwise 0 is returned. If ppItem is valid, *ppItem is assigned
//    item address in case of success and address of last item in the list in 
//    case of a failure.
//
// PARAMETERS
//    pme:        the AppMgr
//    cls:        applet class ID
//    nCatIndex:  category index
//    ppItem:     place holder for item
//
// Return Value:
//    item index  - if item found
//    0           - if item not found
//
//===========================================================================
static uint16 AppMgr_FindItem(AppMgr * pme, AEECLSID cls, uint16 nCatIndex, AppMgrItem** ppItem)
{
   uint16 nIndex = 0;
   AppMgrItem* pElem = pme->m_pList[nCatIndex]; // First Element

   while (pElem)
   {
      nIndex++;
      if (ppItem)
         *ppItem = pElem;
      if (pElem->cls == cls) 
         return nIndex;
      pElem = pElem->pNext;
   }
   return 0;
}

//===========================================================================
// 
// DESCRIPTION
//    Searches for item corresponding to specified class ID. If item is found,
//    Item category is returned. Otherwise 0 is returned. If ppItem is valid,
//    *ppItem is assigned with item address in case of success.
//
// PARAMETERS
//    pme:     the AppMgr
//    cls:     applet class ID
//    ppItem:   place holder for item
//
// Return Value:
//    Category ID - if item found
//    0           - if item not found
//
//===========================================================================
static uint16 AppMgr_FindItemCategory(AppMgr * pme, AEECLSID cls, AppMgrItem** ppItem)
{
   uint16 i;

   for (i = 0; i < MAX_APP_CATS; i++)
   {
      AppMgrItem* pElem = pme->m_pList[i];

      while (pElem)
      {
         if (ppItem)
            *ppItem = pElem;
         if (pElem->cls == cls) 
            return (i+IDC_BREW_APPS);
         pElem = pElem->pNext;
      }
   }

   // Item not found.
   return 0;
}

//===========================================================================
// 
// DESCRIPTION
//    Determines if an application has expired.
//
// PARAMETERS
//    pai:     AppModInfo received from ADS.
//
// Return Value:
//    [TRUE|FALSE]
//
//===========================================================================
boolean IsExpired(AppModInfo* pai)
{
   if(pai->li.pt == PT_SUBSCRIPTION || (uint32) pai->li.dwExpire == BV_UNLIMITED)
      return FALSE;

   switch(pai->li.lt){
      case LT_DATE:
      case LT_DAYS:  
         {
            uint32 dwTime;

#ifdef GETUTCSECONDS
            dwTime = GETUTCSECONDS();
#else
            dwTime = GETTIMESECONDS();
#endif

            // Check if device has service
            if(dwTime <= ONE_YEAR_SECS)
               return(FALSE);

            if(pai->li.dwExpire < ONE_YEAR_SECS * 5)  // Check for pre-loaded apps
               return FALSE;
            else
               return (dwTime >= pai->li.dwExpire);
         }

      case LT_MINUTES_OF_USE:
         return(pai->li.dwExpire == 0);
   }
   return FALSE;
}

//===========================================================================
// 
// DESCRIPTION
//    Determines status of the application
//
// PARAMETERS
//    pme:     the AppMgr
//    itemID:  item ID   
//
// Return Value:
//    [NORMAL|DISABLED|DEMO|EXPIRED|EXPIREDDEMO]
//
//===========================================================================
AppStatus AppMgr_AppStatus(AppMgr* pme, uint32 itemID)
{
#if defined(FEATURE_BREW_DOWNLOAD)
   AppModInfo* pai;
   boolean bDemo = FALSE;

   if (!itemID)
      return NORMAL;

   pai = IDOWNLOAD_GetModInfo(pme->m_pDownload, itemID);

   if (pai)
   {
      if (pai->li.pt == PT_DEMO)
         bDemo = TRUE;

      if (IsExpired(pai))
         return (bDemo ? EXPIREDDEMO : EXPIRED);

      if (pai->bRemoved)
         return DISABLED;

      if (bDemo)
         return DEMO;
   }
#endif // FEATURE_BREW_DOWNLOAD
   return NORMAL;
}

//===========================================================================
// 
// DESCRIPTION
//    General Purpose Prompt
//
// PARAMETERS
//    pme:     the AppMgr
//    szTitle: title string
//    wIDTitle:title string resource ID  
//    szText:  text string
//    wIDText: text string resource ID  
//    wButtons:array of button resource IDs
//    wIDDef:  resource ID of default button
//    dwProps: static control properties
//    bBox:  Display prompt in a box
//
// Return Value:
//    None
//
//===========================================================================
static void AppMgr_Prompt(AppMgr * pme, const AECHAR* szTitle, uint16 wIDTitle, const AECHAR* szText, uint16 wIDText, uint16* wButtons, uint16 wDefID, uint32 dwProps, boolean bBox)
{
   AEERect rc, rcm;
   uint16  nSKHeight, x, y;
   AECHAR* szTitleBuf = NULL;
   AECHAR* szTextBuf = NULL;

   // Create Static control for prompt
   if (ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_STATIC, (void**)&pme->m_pStatic))
      return;

   // Initialize to 0 to make ARM compiler happy
   nSKHeight = x = y = 0;

   if (!szTitle && wIDTitle)
      szTitleBuf = (AECHAR *)MALLOC(MAX_STR_SIZE);
   else
      szTitleBuf = (AECHAR *)szTitle;
   
   if (!szText)
      szTextBuf = (AECHAR *)MALLOC(MAX_STR_SIZE);
   else
      szTextBuf = (AECHAR *)szText;

   // Add Items in SoftKey
   if (wButtons)
   {
      while (*wButtons)
      {
         IMENUCTL_AddItem(pme->m_pSK, AEE_APPMGRLS_RES_FILE, *wButtons, *wButtons, NULL, 0);
         wButtons++;
      }

      // Get SoftKey Rect
      IMENUCTL_GetRect(pme->m_pSK, &rcm);

      // SoftKey Height
      nSKHeight = rcm.dy;

      // Set Default Selection
      IMENUCTL_SetSel(pme->m_pSK, wDefID);
   }

   // Draw Surrounding Box
   if (bBox && wButtons)
   {
      x = (pme->m_rc.dx - (pme->m_rc.dx*DISPLAY_AREA_PERCENT/100))/2;
      y = (pme->m_rc.dy - (pme->m_rc.dy*DISPLAY_AREA_PERCENT/100))/2;

      // Draw Border
      SETAEERECT(&rcm, x-1, y-1, pme->m_rc.dx - 2*(x-1), pme->m_rc.dy - 2*(y-1));
      IDISPLAY_EraseRect(pme->a.m_pIDisplay, &rcm);
      IDISPLAY_DrawRect(pme->a.m_pIDisplay, &rcm, RGB_BLACK, 0, IDF_RECT_FRAME);

      SETAEERECT(&rcm, x, pme->m_rc.dy - y - nSKHeight, pme->m_rc.dx - 2*x, nSKHeight);
      IMENUCTL_SetRect(pme->m_pSK, &rcm);
   }

   // Set Static Control Properties
   if (dwProps)
      ISTATIC_SetProperties(pme->m_pStatic, dwProps);

   // Set Static Control title and text and display it
   if (!szTitle && wIDTitle)        
      ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, wIDTitle, szTitleBuf, MAX_STR_SIZE);

   if (!szText)
      ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, wIDText, szTextBuf, MAX_STR_SIZE);

   if (wButtons)
   {
      // Set Static Control Size
      if (bBox)
         SETAEERECT(&rc, x + MARGIN, y + MARGIN, pme->m_rc.dx - 2*(x + MARGIN), pme->m_rc.dy - 2*y - nSKHeight - 2*MARGIN);
      else
         SETAEERECT(&rc, pme->m_rc.x, pme->m_rc.y, pme->m_rc.dx, pme->m_rc.dy - nSKHeight);

      ISTATIC_SetRect(pme->m_pStatic, &rc);
      ISTATIC_SetText(pme->m_pStatic, szTitleBuf, szTextBuf, AEE_FONT_BOLD, AEE_FONT_NORMAL);
   }
   else
   {
      x = (pme->m_rc.dx - (pme->m_rc.dx*DISPLAY_AREA_PERCENT/100))/2;
      SETAEERECT(&rc, x + MARGIN, pme->m_rc.y, pme->m_rc.dx - 2*(x + MARGIN), pme->m_rc.dy);    
      ISTATIC_SetRect(pme->m_pStatic, &rc);
      ISTATIC_SetText(pme->m_pStatic, szTitleBuf, szTextBuf, AEE_FONT_BOLD, AEE_FONT_NORMAL);
      ISTATIC_SizeToFit(pme->m_pStatic, &rc);
      SETAEERECT(&rcm, rc.x, (pme->m_rc.dy - rc.dy)/2, rc.dx, rc.dy);
      ISTATIC_SetRect(pme->m_pStatic, &rcm);

      // Draw Border
      SETAEERECT(&rc, rcm.x - MARGIN - 1, rcm.y - MARGIN - 1, rcm.dx + 2*(MARGIN + 1), rcm.dy + 2*(MARGIN + 1));
      IDISPLAY_EraseRect(pme->a.m_pIDisplay, &rc);
      IDISPLAY_DrawRect(pme->a.m_pIDisplay, &rc, RGB_BLACK, 0, IDF_RECT_FRAME);
   }

   ISTATIC_SetActive(pme->m_pStatic, TRUE);
   ISTATIC_Redraw(pme->m_pStatic);

   // Make SoftKey Active
   if (wButtons)
      IMENUCTL_SetActive(pme->m_pSK, TRUE);

   if (!szTitle && wIDTitle)
      FREE(szTitleBuf);

   if (!szText)
      FREE(szTextBuf);

   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
}

//===========================================================================
// 
// DESCRIPTION
//    Attempts to launch launcher view applet.
//
// PARAMETERS
//    pme      :  the AppMgr
//    bForceRun:  if TRUE, start the applet
//                if FALSE, start the applet if status is normal otherwise
//                display status dialog
//
// Return Value:
//    TRUE if applet successfully launched
//    FALSE if applet couldn't be launched
//
//===========================================================================
boolean AppMgr_LaunchCurrentApplet(AppMgr* pme,  boolean bForceRun)
{
   AppMgrItem* pCurrentApp = pme->m_pCurrentApp;
   AEECLSID cls = pCurrentApp->cls;
   uint32 bReturn = AEE_SUCCESS;

   if (bForceRun)
   {
      pme->m_nLaunchCls = cls;
      bReturn = ISHELL_StartApplet(pme->a.m_pIShell, cls);
   }
   else
   {
      AppStatus nAppStatus = AppMgr_AppStatus(pme, AppMgr_GetItemID(pme, pme->m_pCurrentApp));

      switch (nAppStatus)
      {
         case NORMAL:
            pme->m_nLaunchCls = cls;
            MSG_FATAL("--->startapplet",0,0,0);
            bReturn = ISHELL_StartApplet(pme->a.m_pIShell, cls);
            #if 0
            if(SUCCESS == bReturn && pme->m_pIAnn != NULL)
            {
                MSG_FATAL("--->Update",0,0,0);
                IANNUNCIATOR_SetFieldText(pme->m_pIAnn,NULL);
            }
            #endif
            break;
         
         default:
            pme->m_wLastState = pme->m_wState;
            AppMgr_DisplayAppStatusDialog(pme, nAppStatus);
            break;
      }
   }
   
   FARF(BAM, ("AppMgr_LaunchCurrentApplet %d 0x%x",bReturn,cls));
   
   if (bReturn != SUCCESS)      
      AppMgr_HandleLaunchError(pme, cls, bReturn);

   return (bReturn == AEE_SUCCESS);
}

//===========================================================================
// 
// DESCRIPTION
//    Handles system errors
//
// PARAMETERS
//    pme      :  the AppMgr
//    nErr     :  error
//
// Return Value:
//    None
//
//===========================================================================
static boolean AppMgr_HandleSystemError(AppMgr* pme, AEECLSID cls, int nErr, AECHAR* pTitle)
{
   uint16            wID;
   AECHAR *          pText = NULL;
   int               nSize = 128*sizeof(AECHAR);

   // Don't display error if notification is for app launched by someone else
   if (cls != pme->m_nLaunchCls)
   {
      return TRUE;
   }

   if ((pText = MALLOC(nSize)) == NULL)
      return FALSE;

   switch(nErr){
      case EALLOCATED:
         wID = AEE_IDS_STDM_ALLOCATED;
         break;

      case EEXPIRED:
         wID = AEE_IDS_STDM_EXPIRED;
         break;

      case EFILENOEXISTS:
         wID = AEE_IDS_STDM_NOT_FOUND;
         break;

      case ERESOURCENOTFOUND:
         wID = AEE_IDS_STDM_RES_NOT_FOUND;
         break;

      case SIG_NO_MEMORY:
      case ENOMEMORY:
         wID = AEE_IDS_STDM_MEM;
         break;
   
      case EINVALIDTIME:
         wID = AEE_IDS_STDM_DIG_REQ;
         break;

      case EUNABLETOLOAD:
         wID = AEE_IDS_STDM_CANNOT_START;
         break;

      case EPRIVLEVEL:
         wID = AEE_IDS_PRIV_ERROR;
         break;

      default:
      {
         AECHAR * pFilter = NULL;
   
         if ((pFilter = MALLOC(nSize)) != NULL)
         {
            if(nErr >= SIG_ERROR_BASE && nErr <= SIG_ERROR_LAST)
               wID = AEE_IDS_SECURITY_ERROR;
            else
               wID = AEE_IDS_STDM_UNKNOWN;
 
            if (ISHELL_LoadResString(pme->a.m_pIShell, AEECONTROLS_RES_FILE, wID, pFilter, nSize))
               WSPRINTF(pText, nSize, pFilter, nErr);
            FreePtr((void**)&pFilter);
         }
         wID = 0;
      }      
   }

   if(wID)
      ISHELL_LoadResString(pme->a.m_pIShell, AEECONTROLS_RES_FILE, wID, pText, nSize);

   // If there was a module specified, the error is in there...

   AppMgr_Prompt(pme, pTitle, 0, pText, 0, NULL, 0, CP_USE_DEFAULT|ST_TEXTALLOC, FALSE);

   return TRUE;
}

//===========================================================================
// 
// DESCRIPTION
//    Handles error encountered in applet launch
//
// PARAMETERS
//    pme      :  the AppMgr
//    nErr     :  error
//
// Return Value:
//    None
//
//===========================================================================
static boolean AppMgr_HandleLaunchError(AppMgr* pme, AEECLSID cls, int nErr)
{
   pme->m_wLastState = pme->m_wState;

   // Don't display error if notification is for app launched by someone else
   if (cls != pme->m_nLaunchCls)
   {
      return TRUE;
   }

   // Reset Controls
   AppMgr_Reset(pme, FALSE);

   switch (nErr)
   {

   // Notification from shell that an app has expired...

      case EEXPIRED:
         AppMgr_DisplayAppStatusDialog(pme, EXPIRED);
         return TRUE;

      case EBADSID: // ownership error
         {
         uint16 wButtons[2];

         MEMSET(wButtons, 0, sizeof(wButtons));
         wButtons[0] = IDC_OK;

         // Display Prompt
#ifdef CUST_EDITION
         AppMgr_Prompt(pme, NULL, 0, NULL, IDS_LICENSE_NOT_FOUND, wButtons, wButtons[1], ST_MIDDLETEXT, TRUE);
#else
         AppMgr_Prompt(pme, NULL, 0, NULL, IDS_LICENSE_NOT_FOUND, wButtons, wButtons[1], ST_NOSCROLL, TRUE);
#endif
         }
         return TRUE;

      case EFILENOEXISTS:
         AppMgr_DisplayAppStatusDialog(pme, DISABLED);
         return TRUE;

#ifdef EACKPENDING
      case EACKPENDING:
         AppMgr_DisplayAppStatusDialog(pme, ACKPENDING);
         return TRUE;
#endif // EACKPENDING

      default:
         return AppMgr_HandleSystemError(pme, cls, nErr, AppMgr_GetAppName(pme, pme->m_pCurrentApp));
   }
}
//===========================================================================
//
// DESCRIPTION
//    Fetches image associated with the specified application
//    manager item.
//
// PARAMETERS
//    pme:     AppMgr
//    pi:      AppMgr Item
//    ppImage: place holder for image associated with the current app
//
// Return Value: NONE
//===========================================================================
void AppMgr_GetAppImage(AppMgr* pme, AppMgrItem* pi, IImage** ppImage)
{
   AEEAppInfo ai;
   int width, height;
   IShell* pIShell = pme->a.m_pIShell;
   AEECLSID cls = pi->cls;

   if (!ppImage || !cls || !ISHELL_QueryClass(pIShell, cls, &ai))
      return;

   width = OEM_SMALL_SCREEN_DEVICE ? APP_ICON_X_THRESHOLD : APP_IMAGE_X_THRESHOLD;
   height = OEM_SMALL_SCREEN_DEVICE ? APP_ICON_Y_THRESHOLD : APP_IMAGE_Y_THRESHOLD;

   // Nullify
   *ppImage = NULL;

   // If image file is specified, load that
   if (*pi->szIcon)
      *ppImage = ISHELL_LoadImage(pme->a.m_pIShell, pi->szIcon);

   // Ensure that Image is not bigger than width & height
   if (*ppImage && ImageFits(*ppImage, NULL, NULL, 0, width, height))
      return;
      
   FreeObj((void**)ppImage);

   // If device is not small screen, get App Image
   if (!OEM_SMALL_SCREEN_DEVICE)
      *ppImage = ISHELL_LoadResImage(pIShell, ai.pszMIF, APPR_IMAGE(ai));

   // Ensure that Image is not bigger than width & height
   if (*ppImage && ImageFits(*ppImage, NULL, NULL, 0, width, height))
      return;
      
   FreeObj((void**)ppImage);
      
   // If no image, get App Icon
   *ppImage = ISHELL_LoadResImage(pIShell, ai.pszMIF, APPR_ICON(ai));

   // Ensure that Image is not bigger than width & height
   if (*ppImage && ImageFits(*ppImage, NULL, NULL, 0, width, height))
      return;

   FreeObj((void**)ppImage);

   // If no app image, lets show default icon
   *ppImage = ISHELL_LoadResImage(pIShell, AEECONTROLS_RES_FILE, AEE_IDB_BREW);
}

//===========================================================================
//
// DESCRIPTION
//    Fetches App Name associated with the specified application manager item.
//
// PARAMETERS
//    pme:     AppMgr
//    pi:      AppMgr Item
//
// Return Value: App Name
//===========================================================================
AECHAR * AppMgr_GetAppName(AppMgr* pme, AppMgrItem* pi)
{
   AEEAppInfo ai;
   IShell* pIShell = pme->a.m_pIShell;
   AEECLSID cls = pi->cls;

   if (!pi)
      return NULL;

   if (*pi->wszName)
      return pi->wszName;

   if (cls && ISHELL_QueryClass(pIShell, cls, &ai))
      ISHELL_LoadResString(pIShell, ai.pszMIF, APPR_NAME(ai), pi->wszName, sizeof(pi->wszName));      

   if(!*pi->wszName)
      ISHELL_LoadResString(pIShell, AEECONTROLS_RES_FILE, AEE_IDS_UNKNOWN, pi->wszName, sizeof(pi->wszName));

   return pi->wszName;
}

//===========================================================================
//
// DESCRIPTION
//    Fetches icon associated with the specified application manager item.
//
// PARAMETERS
//    pme:     AppMgr
//    pi:      AppMgr Item
//
// Return Value: Image associated with the current app
//===========================================================================
IImage* AppMgr_GetAppIcon(AppMgr* pme, AppMgrItem* pi)
{
   byte * pByte = NULL;
   IImage * pImage = NULL;
   IShell* pIShell = pme->a.m_pIShell;

   // Return if we know image offset and user doesn't want pImage
   if (pi->nOffset == 0xfffffffe)
      return NULL;

   // If we know image offset, give pImage to user
   if (pi->nOffset != 0xffffffff)
   {
      if (!pi->nSize)
      {  
         // Invalid Size. Reset offset and load icon.
         pi->nOffset = 0xffffffff;
         return AppMgr_LoadAppIcon(pme, pi);
      }

      // Open image cache file if its not already open
      if (!pme->m_pImageCacheFile)
      {
         pme->m_pImageCacheFile = IFILEMGR_OpenFile(pme->m_pIfm, IMAGE_CACHE, _OFM_READWRITE);
         if (!pme->m_pImageCacheFile)
         {  
            // File doesn't exist. Reset offset and load icon.
            pi->nOffset = 0xffffffff;
            pi->nSize = 0;
            return AppMgr_LoadAppIcon(pme, pi);
         }
      }

      pByte = MALLOC(pi->nSize);

      // Read image data
      if (!pByte || 
         (IFILE_Seek(pme->m_pImageCacheFile, _SEEK_START, pi->nOffset) != AEE_SUCCESS) ||
         ((uint32)IFILE_Read(pme->m_pImageCacheFile, pByte, pi->nSize) != pi->nSize))
      {  
         // Can't load image data. Invalid offset. Reset offset and load icon.
         FreePtr((void**)&pByte);
         pi->nOffset = 0xffffffff;
         pi->nSize = 0;
         return AppMgr_LoadAppIcon(pme, pi);
      }
       
      // Create IImage
      ISHELL_CreateInstance(pIShell, ISHELL_GetHandler(pIShell, HTYPE_VIEWER, (char*)(pByte + 2)), (void**)&pImage);

      if (pImage)
      {
         IMemAStream * pmi = NULL;
         ISHELL_CreateInstance(pIShell, AEECLSID_MEMASTREAM, (void**)&pmi);
         if (pmi)
         {
            IMEMASTREAM_Set(pmi, pByte, pi->nSize, *pByte, FALSE);
            IIMAGE_SetStream(pImage, (IAStream*)pmi);
            FreeObj((void**)&pmi);
         }
         else
         {
            FreeObj((void**)&pImage);
            FreePtr((void**)&pByte);
         }
         return pImage;
      }
      else
      {  
         // Image data corrupt. Invalid offset. Reset offset and load icon.
         FreePtr((void**)&pByte);
         pi->nOffset = 0xffffffff;
         pi->nSize = 0;
         return AppMgr_LoadAppIcon(pme, pi);
      }
   }
   else
      return AppMgr_LoadAppIcon(pme, pi);
}

//===========================================================================
//
// DESCRIPTION
//    Loads icon associated with the specified application manager item from
// its mif and cache into image file
//
// PARAMETERS
//    pme:     AppMgr
//    pi:      AppMgr Item
//
// Return Value: Image associated with the current app
//===========================================================================
IImage * AppMgr_LoadAppIcon(AppMgr* pme, AppMgrItem* pi)
{
   byte * pByte = NULL;
   IImage * pImage = NULL;
   uint32 nByte;
   IShell* pIShell = pme->a.m_pIShell;
   AEECLSID cls = pi->cls;
   AEEAppInfo ai;      
   IMemAStream * pmi = NULL;

   if (!cls || !ISHELL_QueryClass(pIShell, cls, &ai))
      return NULL;

   // Load image data size
   if (!ISHELL_LoadResDataEx(pIShell, ai.pszMIF, (uint16)(pme->m_cfg.bListView ? APPR_THUMB(ai) : (pme->m_cfg.bSmallIconView ? APPR_ICON(ai) : APPR_THUMB(ai))), RESTYPE_IMAGE, (void *)-1, &nByte) || !nByte)
   {
      pi->nOffset = 0xfffffffe;
      pi->nSize = 0;
      return NULL;
   }

   pByte = MALLOC(nByte);

   // Load image data
   if (!pByte || 
      !ISHELL_LoadResDataEx(pIShell, ai.pszMIF, (uint16)(pme->m_cfg.bListView ? APPR_THUMB(ai) : (pme->m_cfg.bSmallIconView ? APPR_ICON(ai) : APPR_THUMB(ai))), RESTYPE_IMAGE, (void *)pByte, &nByte) ||
      (ISHELL_CreateInstance(pIShell, ISHELL_GetHandler(pIShell, HTYPE_VIEWER, (char*)(pByte + 2)), (void**)&pImage) != AEE_SUCCESS) ||
      !pImage)
   {
      FreePtr((void**)&pByte);
      return NULL;
   }
         
   ISHELL_CreateInstance(pIShell, AEECLSID_MEMASTREAM, (void**)&pmi);
         
   if (pmi)
   {
      int w, h;

      if (pme->m_cfg.bListView)
      {
         w = APP_THUMB_X_THRESHOLD;
         h = APP_THUMB_Y_THRESHOLD;
      }
      else if (pme->m_cfg.bSmallIconView)
      {
         w = APP_ICON_X_THRESHOLD;
         h = APP_ICON_Y_THRESHOLD;
      }
      else
      {
         w = APP_THUMB_X_THRESHOLD;
         h = APP_THUMB_Y_THRESHOLD;
      }

      IMEMASTREAM_Set(pmi, pByte, nByte, *pByte, FALSE);
      IIMAGE_SetStream(pImage, (IAStream*)pmi);
      if (ImageFits(pImage, pIShell, NULL, 0, w, h))
      {
         // Open image cache if not already open
         if (!pme->m_pImageCacheFile)
            pme->m_pImageCacheFile = IFILEMGR_OpenFile(pme->m_pIfm, IMAGE_CACHE, _OFM_READWRITE);

         if (!pme->m_pImageCacheFile)
            pme->m_pImageCacheFile = IFILEMGR_OpenFile(pme->m_pIfm, IMAGE_CACHE, _OFM_CREATE);

         if (pme->m_pImageCacheFile)
         {
            if (IFILE_Seek(pme->m_pImageCacheFile, _SEEK_END, 0) == AEE_SUCCESS)
            {
               pi->nOffset = IFILE_Seek(pme->m_pImageCacheFile, _SEEK_CURRENT, 0);
               pi->nSize = nByte;
            }

            if ((IFILE_Write(pme->m_pImageCacheFile, pByte, nByte) != nByte))
            {
               pi->nOffset = 0xffffffff;
               pi->nSize = 0;
            }
         }
      }
      else
      {
         FreeObj((void**)&pImage);
         pi->nOffset = 0xfffffffe;
         pi->nSize = 0;
      }           
      FreeObj((void**)&pmi);
   }
   else
   {
      FreeObj((void**)&pImage);
      FreePtr((void**)&pByte);
   }
   return pImage;
}


//===========================================================================
// 
// DESCRIPTION
//    Sets Main Menu size according to the main menu view
//
// PARAMETERS
//    pme      : the AppMgr
//
// Return Value:
//    None
//
//===========================================================================
void AppMgr_SetMainMenuSize(AppMgr * pme)
{
   if (pme->m_cfg.bListView)
      IMENUCTL_SetRect(pme->m_pMainMenu, &pme->m_rc);
   else
   {
      if (pme->m_cfg.bSmallIconView)
         IMENUCTL_SetRect(pme->m_pMainMenu, &pme->m_rc);
      else
      {
         AEERect rc, rcm;

         // Set Main Menu Size
         IMENUCTL_GetRect(pme->m_pMainMenuSK, &rcm);
         SETAEERECT(&rc, 0, 0, pme->m_rc.dx, rcm.y - 1);
         IMENUCTL_SetRect(pme->m_pMainMenu, &rc);
      }
   }     
}

//===========================================================================
// 
// DESCRIPTION
//    Resets all the menus in AppMgr except options menu which we use as such
//    during the life of the application.
//
// PARAMETERS
//    pme      : the AppMgr
//    bClear   : if TRUE, clear the display area
//
// Return Value:
//    None
//
//===========================================================================
void AppMgr_Reset(AppMgr* pme, boolean bClear)
{
   if (pme->m_bShowBusy)
   {
      // Show Busy Image
      IImage * pImage = ISHELL_LoadBusyImage(pme->a.m_pIShell);

      if (pImage)
      {
         AEEImageInfo ii;
         IIMAGE_GetInfo(pImage, &ii);
         IIMAGE_SetParm(pImage, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
         IIMAGE_Draw(pImage, (pme->m_rc.dx - ii.cx)/2, (pme->m_rc.dy - ii.cy)/2);
         IIMAGE_Release(pImage);
         IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
      }
   }
   else
      pme->m_bShowBusy = TRUE;

   // Cancel Timers
   ISHELL_CancelTimer(pme->a.m_pIShell, NULL, (void**)pme);

   // Reset shared controls
   IMENUCTL_Reset(pme->m_pMenu);
   IMENUCTL_Reset(pme->m_pMainMenu);
   IMENUCTL_Reset(pme->m_pSK);

   // Reset Main Menu Softkey
   if (pme->m_pMainMenuSK)
      IMENUCTL_Reset(pme->m_pMainMenuSK);

   // Set Size for Menu
   IMENUCTL_SetRect(pme->m_pMenu, &pme->m_rc);
   IMENUCTL_SetRect(pme->m_pSK, &pme->m_rc);

   // Free Static Text
   if (pme->m_pStatic)
      FreeObj((void**)&pme->m_pStatic);

   // For the case where user hits clear key on config menu
   pme->m_bConfigMenu = FALSE;

   // Clear Display Area
   if (bClear)
      IDISPLAY_ClearScreen(pme->a.m_pIDisplay);

   // Stop Sound & Animation
   AppMgr_StopSoundAnimation(pme);
}

//===========================================================================
//
// DESCRIPTION
//    Display System Information in following format
//    RAM %s\nBREW %s\nApplication Manager%s
//
// PARAMETERS
//    pme:  AppMgr
//    prc: Bounding Rectangle
//
// Return Value: NONE
//===========================================================================
void AppMgr_MoreInfo(AppMgr* pme, AEERect* prc)
{
   char szVersion[64];
   AECHAR szTemp[32];
   uint16 wButtons[2];
   AECHAR * pText = NULL;
   uint32 dwRAM;
   AEERect rc;

   // Determine largest free block of available RAM. 
   GETRAMFREE(NULL, &dwRAM);

   // Display available RAM
   if (dwRAM)
   {
      if (ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDS_SYSTEMINFO_RAM, szTemp, sizeof(szTemp)))
         pText = AppendWStr(pText, szTemp, '\n');
      FormatMem(dwRAM, szTemp, FALSE);
      pText = AppendWStr(pText, szTemp, ' ');
   }

   if (ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDS_OPTIONS_BREW_INFO, szTemp, sizeof(szTemp)))
      pText = AppendWStr(pText, szTemp, '\n');

   // Get AEE Version
#if defined(GAV_UPDATE)
   GETAEEVERSION((byte*)szVersion, sizeof(szVersion), GAV_LATIN1|GAV_UPDATE);
#else
   GETAEEVERSION((byte*)szVersion, sizeof(szVersion), GAV_LATIN1);
#endif

   STRTOWSTR(szVersion, szTemp, sizeof(szTemp));
   pText = AppendWStr(pText, szTemp, ' ');

   if (ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDS_APPLICATION_MANAGER, szTemp, sizeof(szTemp)))
      pText = AppendWStr(pText, szTemp, '\n');

   if (ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDS_APPMGR_VERSION, szTemp, sizeof(szTemp)))
      pText = AppendWStr(pText, szTemp, ' ');

   // Display Prompt
   MEMSET(wButtons, 0, sizeof(wButtons));
   wButtons[0] = IDC_OK;

   // Cache Current Size
   rc = pme->m_rc;

   // Change Prompt Size
   pme->m_rc = *prc;
   AppMgr_Prompt(pme, NULL, NULL, pText, 0, wButtons, wButtons[0], ST_TEXTALLOC|ST_NOSCROLL|ST_CENTERTEXT, FALSE);

   // Restore Size
   pme->m_rc = rc;
}

//===========================================================================
//
// DESCRIPTION
//    Display dialog corresponding to specified app status.
//
// PARAMETERS
//    pme:  AppMgr
//
// Return Value: NONE
//===========================================================================
void AppMgr_DisplayAppStatusDialog(AppMgr* pme, AppStatus status)
{
   uint16   wIDTitle = 0;
   uint16   wIDText = 0;
   uint16   wButtons[3];

   MEMSET(wButtons, 0, sizeof(wButtons));

   switch (status)
   {
      case DISABLED:
         wIDText = IDS_LAUNCHER_DISABLED_MSG;
         wIDTitle = IDS_LAUNCHER_DISABLED;
         wButtons[0] = IDC_LAUNCHER_RESTORE;
         wButtons[1] = IDC_CANCEL;
         break;
         
      case DEMO:
         wIDText = IDS_LAUNCHER_DEMO_MSG;
         wIDTitle = IDS_LAUNCHER_DEMO;
         wButtons[0] = IDC_LAUNCHER_BUY_NOW;
         wButtons[1] = IDC_LAUNCHER_RUN_NOW;
         break;

      case EXPIRED:
         wIDText = IDS_LAUNCHER_EXPIRED_MSG;
         wIDTitle = IDS_LAUNCHER_EXPIRED;
         wButtons[0] = IDC_LAUNCHER_BUY;
         wButtons[1] = IDC_CANCEL;
         break;

      case EXPIREDDEMO:
         wIDText = IDS_LAUNCHER_EXPIREDDEMO_MSG;
         wIDTitle = IDS_LAUNCHER_EXPIREDDEMO;
         wButtons[0] = IDC_LAUNCHER_BUY;
         wButtons[1] = IDC_CANCEL;
         break;

#ifdef EACKPENDING
      case ACKPENDING:
         wIDText = IDS_LAUNCHER_ACKPENDING_MSG;
         wIDTitle = IDS_LAUNCHER_ACKPENDING;
         wButtons[0] = IDC_LAUNCHER_VERIFY;
         wButtons[1] = IDC_CANCEL;
         break;
#endif // EACKPENDING
   }

   // Reset UI
   AppMgr_Reset(pme, TRUE);

   // Display prompt
   AppMgr_Prompt(pme, NULL, wIDTitle, NULL, wIDText, wButtons, wButtons[0], ST_NOSCROLL, FALSE);
}

//===========================================================================
//
// DESCRIPTION
//    State Handler for ST_MSHOPGUIDE
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_MShopGuide(AppMgr * pme)
{
   IImage* pMShop = ISHELL_LoadResImage(pme->a.m_pIShell, APPMGRLN_RES_FILE, IDB_MSHOP_THUMB);

   // Draw MShop Logo
   if (pMShop)
   {
      AEEImageInfo ii;
      AECHAR pTitle[32];
      AEERect rc;
      uint16 uMenuStartY = 0; 

      // Get Image Info
      IIMAGE_GetInfo(pMShop, &ii);

      // Set Image Background Transparent
      IIMAGE_SetParm(pMShop, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
 
      // If image height greater or equal to font height, start image at top of window 
      if (ii.cy >= pme->m_cyFont) 
      {
         IIMAGE_Draw(pMShop, 0, 0);
      }
      
      // Otherwise, align bottom of image with bottom of text 
      else 
      {
         IIMAGE_Draw(pMShop, 0, pme->m_cyFont - ii.cy); 
      }

      // Release Image
      IIMAGE_Release(pMShop);

      // Draw Title
      ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDC_HELP, pTitle, sizeof(pTitle));

      // If image height greater or equal to font height, align bottom of text with bottom of image
      if (ii.cy >= pme->m_cyFont) 
      {         
         IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_BOLD, pTitle, -1, ii.cx + 1, ii.cy - pme->m_cyFont, NULL, 0);
         uMenuStartY = ii.cy; 
      }

      // Otherwise, start text at top of window 
      else 
      {
         IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_BOLD, pTitle, -1, ii.cx + 1, 0, NULL, 0);
         uMenuStartY = pme->m_cyFont; 
      }
    
      if (OEM_UNDERLINE_MENU_TITLE)
      {
         // If image height greater or equal to font height, draw underline based on image heigh
         if (ii.cy >= pme->m_cyFont) 
         {
            IDISPLAY_DrawHLine(pme->a.m_pIDisplay, 0, ii.cy + 1, pme->m_rc.dx);
         }
         // Otherwise, draw underline based on font height
         else 
         {
            IDISPLAY_DrawHLine(pme->a.m_pIDisplay, 0, pme->m_cyFont + 1, pme->m_rc.dx);
         }

         uMenuStartY += 2; 
      }
      
      SETAEERECT(&rc, 0, uMenuStartY, pme->m_rc.dx, pme->m_rc.dy - uMenuStartY);
      // Menu Size/Location
      IMENUCTL_SetRect(pme->m_pMenu, &rc);
   }

   // add menu items
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_MSHOPGUIDE_MSHOP, IDC_MSHOPGUIDE_MSHOP, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_MSHOPGUIDE_CATALOG, IDC_MSHOPGUIDE_CATALOG, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_MSHOPGUIDE_FIND, IDC_MSHOPGUIDE_FIND, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_MSHOPGUIDE_PREVIEW, IDC_MSHOPGUIDE_PREVIEW, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_MSHOPGUIDE_PURCHASE_OPTIONS, IDC_MSHOPGUIDE_PURCHASE_OPTIONS, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_MSHOPGUIDE_PAYMENT, IDC_MSHOPGUIDE_PAYMENT, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_MSHOPGUIDE_MEMORY_FULL, IDC_MSHOPGUIDE_MEMORY_FULL, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_MSHOPGUIDE_RESTORING_APPS, IDC_MSHOPGUIDE_RESTORING_APPS, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_MSHOPGUIDE_CAN_I_RETURN_APPS, IDC_MSHOPGUIDE_CAN_I_RETURN_APPS, NULL, NULL);

   // Set menu selection
   if (pme->m_nIndex > 1)
      IMENUCTL_SetSel(pme->m_pMenu, pme->m_nMenuSel[pme->m_nIndex--]);
               
   // Set Menu Active
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);

   // Set Last State to MShop
   pme->m_wLastState = ST_MSHOP;

   return(TRUE);
}

//===========================================================================
//
// DESCRIPTION
//    State Handler for ST_CATEGORIES
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_Categories(AppMgr * pme)
{
   AEERect rc;
   CtlAddItem ai;
   uint16 nCategory = 0;
   boolean bAdd = TRUE;
   boolean bCheck = FALSE;

   MEMSET(&ai, 0, sizeof(ai));

   ai.pszResText  = AEE_APPMGRLS_RES_FILE;
   ai.pszResImage = APPMGRLN_RES_FILE;

   // add title to the menu
   switch (pme->m_wState)
   {
      case ST_MOVE:
      case ST_SETCATEGORY:
      {
         // Draw App Name
         IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_BOLD, AppMgr_GetAppName(pme, pme->m_pCurrentApp), -1, 0, 0, NULL, 0);

         // Set Title
         IMENUCTL_SetTitle(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, (uint16)((pme->m_wState == ST_MOVE) ? IDS_MOVE : IDS_SET_CATEGORY), NULL);

         // Menu Control Rect Size
         SETAEERECT(&rc, 0, pme->m_cyFont, pme->m_rc.dx, pme->m_rc.dy - pme->m_cyFont);

         // Set bCheck
         bCheck = TRUE;
      }
      break;

      default:
         // Set Title
         IMENUCTL_SetTitle(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDS_SELECT, NULL);

         // Menu Control Rect Size
         SETAEERECT(&rc, 0, 0, pme->m_rc.dx, pme->m_rc.dy);
   }

   // add menu items
   ai.wFont    = AEE_FONT_NORMAL;

   // Launcher Category
   if (bCheck)
   {
      // Find category of current app
      nCategory = AppMgr_FindItemCategory(pme, pme->m_pCurrentApp->cls, NULL);
      bAdd = (nCategory != IDC_BREW_APPS);
   }

   if (bAdd)
   {
      ai.wText    = IDS_MAIN_MENU;
      ai.wImage   = IDB_MAINMENU;
      ai.wItemID  = IDC_BREW_APPS;
      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   }

   // Favorites Category

   if (bCheck)
      bAdd = (nCategory != IDC_FAVORITES);
   
   if (bAdd)
   {
      ai.wText    = IDC_FAVORITES;
      ai.wImage   = IDB_FAVORITES_SMALL;
      ai.wItemID  = IDC_FAVORITES;
      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   }

   // Work Category

   if (bCheck)
      bAdd = (nCategory != IDC_WORK);

   if (bAdd)
   {
      ai.wText    = IDC_WORK;
      ai.wImage   = IDB_WORK_SMALL;
      ai.wItemID  = IDC_WORK;
      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   }

   // Fun Category

   if (bCheck)
      bAdd = (nCategory != IDC_FUN);

   if (bAdd)
   {
      ai.wText    = IDC_FUN;
      ai.wImage   = IDB_FUN_SMALL;
      ai.wItemID  = IDC_FUN;
      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   }

   // Set Selection
   if (!bCheck && (pme->m_nIndex > 1))
      IMENUCTL_SetSel(pme->m_pMenu, pme->m_nMenuSel[pme->m_nIndex--]);

   // Menu Size/Location
   IMENUCTL_SetRect(pme->m_pMenu, &rc);

   // Set Menu Active
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);

   return(TRUE);
}

#if 0
//===========================================================================
//
// DESCRIPTION
//    Draws AppStatus in large icon view
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
void AppMgr_DrawAppStatus(AppMgr* pme, const AppStatus status)
{
#if defined(FEATURE_BREW_DOWNLOAD)
   AECHAR szText[32];
   AEERect rc;
   IImage * pImage = NULL;

   // Get Main Menu Rect
   IMENUCTL_GetRect(pme->m_pMainMenu, &rc);

   switch (status)
   {
      case DISABLED:
         pImage = ISHELL_LoadResImage(pme->a.m_pIShell, APPMGRLN_RES_FILE, IDB_DISABLED);
         ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDS_LAUNCHER_DISABLED, szText, sizeof(szText));
         break;

      case DEMO:
         pImage = ISHELL_LoadResImage(pme->a.m_pIShell, APPMGRLN_RES_FILE, IDB_DEMO);
         ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDS_LAUNCHER_DEMO, szText, sizeof(szText));
         break;

      case EXPIRED:
         pImage = ISHELL_LoadResImage(pme->a.m_pIShell, APPMGRLN_RES_FILE, IDB_EXPIRED);
         ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDS_LAUNCHER_EXPIRED, szText, sizeof(szText));
         break;

      case EXPIREDDEMO:
         pImage = ISHELL_LoadResImage(pme->a.m_pIShell, APPMGRLN_RES_FILE, IDB_EXPIREDDEMO);
         ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDS_LAUNCHER_EXPIREDDEMO, szText, sizeof(szText));
         break;
   }

   if(pImage != NULL)
   {
      AEEImageInfo ii;
      IIMAGE_GetInfo(pImage, &ii);
      IIMAGE_SetParm(pImage, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
      IIMAGE_Draw(pImage, MARGIN, rc.dy - 2*pme->m_cyFont);
      IIMAGE_Release(pImage);
      IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_NORMAL, szText, -1, 2*MARGIN + ii.cx, rc.dy - 2*pme->m_cyFont, NULL, IDF_TEXT_TRANSPARENT);
   }
#endif // FEATURE_BREW_DOWNLOAD
}
#endif

//===========================================================================
//
// DESCRIPTION
//    State Handler for ST_MAINMENUVIEW
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_MainMenuView(AppMgr * pme)
{
   CtlAddItem ai;

   MEMSET(&ai, 0, sizeof(ai));

   ai.pszResText  = AEE_APPMGRLS_RES_FILE;
   ai.pszResImage = APPMGRLN_RES_FILE;

   // add title to the menu
   IMENUCTL_SetTitle(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_OPTIONS_MAINMENU_STYLE, NULL);

   // add menu items
   ai.wFont    = AEE_FONT_NORMAL;

   ai.wText    = IDC_SMALL_ICON_VIEW;
   ai.wImage   = (!pme->m_cfg.bListView &&  pme->m_cfg.bSmallIconView) ? IDB_RADIO_SEL : IDB_RADIO_UNSEL;
   ai.wItemID  = IDC_SMALL_ICON_VIEW;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   ai.wText    = IDC_LARGE_ICON_VIEW;
   ai.wImage   = (!pme->m_cfg.bListView &&  !pme->m_cfg.bSmallIconView) ? IDB_RADIO_SEL : IDB_RADIO_UNSEL;
   ai.wItemID  = IDC_LARGE_ICON_VIEW;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   ai.wText    = IDC_LIST_VIEW;
   ai.wImage   = pme->m_cfg.bListView ? IDB_RADIO_SEL : IDB_RADIO_UNSEL;
   ai.wItemID  = IDC_LIST_VIEW;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   // Set Proper Selection
   IMENUCTL_SetSel(pme->m_pMenu, (uint16)(pme->m_cfg.bListView ? IDC_LIST_VIEW : (uint16)(pme->m_cfg.bSmallIconView ? IDC_SMALL_ICON_VIEW : IDC_LARGE_ICON_VIEW)));

   // Set Menu Active
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);   
   
   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);

   return(TRUE);
}

//===========================================================================
//
// DESCRIPTION
//    State Handler for ST_LOCKUNLOCK
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_LockUnlock(AppMgr * pme)
{
   CtlAddItem ai;
   AppModInfo * pmi;

   if (!AppMgr_GetItemID(pme, pme->m_pCurrentApp))
      return TRUE;

   // Get module information for the current app
   pmi = IDOWNLOAD_GetModInfo(pme->m_pDownload, AppMgr_GetItemID(pme, pme->m_pCurrentApp));

   if (!pmi)
   {
      if (pme->m_pCurrentApp)
         pme->m_pCurrentApp->dwItemID = 0;
      AppMgr_SetState(pme, PREV_ENTRY);
      return TRUE;
   }

   // add title to the menu
   IMENUCTL_SetTitle(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_LOCK_APP, NULL);

   // add menu items
   MEMSET(&ai, 0, sizeof(ai));

   ai.pszResText  = AEE_APPMGRLS_RES_FILE;
   ai.pszResImage = APPMGRLN_RES_FILE;

   ai.wFont    = AEE_FONT_NORMAL;

   ai.wItemID     = ai.wText = IDC_LOCKED;
   ai.wImage      = pmi->bLocked ? IDB_RADIO_SEL : IDB_RADIO_UNSEL;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   ai.wItemID     = ai.wText = IDC_UNLOCKED;
   ai.wImage      = !pmi->bLocked ? IDB_RADIO_SEL : IDB_RADIO_UNSEL;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   // Set Proper Selection
   IMENUCTL_SetSel(pme->m_pMenu, (uint16)(pmi->bLocked ? IDC_LOCKED : IDC_UNLOCKED));

   // Set Menu Active
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);   
   
   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);

   return(TRUE);
}

//===========================================================================
//
// DESCRIPTION
//    State Handler for ST_AIRTIMEWARNING
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_AirtimeWarnings(AppMgr * pme)
{
   CtlAddItem ai;
   boolean bWarning;
   
   // add title to the menu
   IMENUCTL_SetTitle(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDC_AIRTIME_WARNINGS, NULL);

   // add menu items

   MEMSET(&ai, 0, sizeof(ai));

   ai.pszResText  = AEE_APPMGRLS_RES_FILE;
   ai.pszResImage = APPMGRLN_RES_FILE;

   ai.wFont    = AEE_FONT_NORMAL;
   
   bWarning = (pme->m_cfg.bBrowseWarning || pme->m_cfg.bBuyWarning || pme->m_cfg.bCreditBackWarning || pme->m_cfg.bRestoreWarning || pme->m_cfg.bSearchWarning || pme->m_cfg.bUpgradeWarning);

   ai.wText    = IDC_AIRTIME_WARNINGS_DISABLED;
   ai.wImage   = !bWarning ? IDB_RADIO_SEL : IDB_RADIO_UNSEL;
   ai.wItemID  = IDC_AIRTIME_WARNINGS_DISABLED;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   ai.wText    = IDC_AIRTIME_WARNINGS_ENABLED;
   ai.wImage   = bWarning ? IDB_RADIO_SEL : IDB_RADIO_UNSEL;
   ai.wItemID  = IDC_AIRTIME_WARNINGS_ENABLED;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   // Set Proper Selection
   IMENUCTL_SetSel(pme->m_pMenu, (uint16)(bWarning ? IDC_AIRTIME_WARNINGS_ENABLED : IDC_AIRTIME_WARNINGS_DISABLED));

   // Set Menu Active
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);   
   
   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);

   return(TRUE);
}

//===========================================================================
// DESCRIPTION
//    Add an app entry to the app menu.  
//
//PARAMETERS
//    pme      :  the AppMgr
//    pMenu    :  IMenuCtl pointer
//    pi       :  pointer to app manager item
//    itemID   :  Item ID
//    nType    :  Type of Image to use
//    bSK      :  If TRUE, don't insert item name
//
// Return Value:
//    None
//===========================================================================
static void AppMgr_AddMenuItem(AppMgr * pme, IMenuCtl * pMenu, AppMgrItem* pi, uint16 itemID, AppImageType nType, boolean bSK)
{
   CtlAddItem mai;
   int width = 0;
   int height = 0;
   IImage* pImage = NULL;

   if (nType == THUMB)
   {
      width = APP_THUMB_X_THRESHOLD;
      height = APP_THUMB_Y_THRESHOLD;

      if (pme->m_cfg.bListView)
         pImage = AppMgr_GetAppIcon(pme, pi);
      else if (pme->m_cfg.bSmallIconView)
      {
         AEEAppInfo ai; 
         ISHELL_QueryClass(pme->a.m_pIShell, pi->cls, &ai);
         pImage = ISHELL_LoadResImage(pme->a.m_pIShell, ai.pszMIF, APPR_THUMB(ai));
      }
      else
         pImage = AppMgr_GetAppIcon(pme, pi);
   }
   else if (nType == ICON)
   {
      width = APP_ICON_X_THRESHOLD;
      height = APP_ICON_Y_THRESHOLD;
      pImage = AppMgr_GetAppIcon(pme, pi);
   }
   else
   {
      if (*pi->szIcon)
         AppMgr_GetAppImage(pme, pi, &pImage);
      else
      {
         AEEAppInfo ai; 
         ISHELL_QueryClass(pme->a.m_pIShell, pi->cls, &ai);
         width = OEM_SMALL_SCREEN_DEVICE ? APP_ICON_X_THRESHOLD : APP_IMAGE_X_THRESHOLD;
         height = OEM_SMALL_SCREEN_DEVICE ? APP_ICON_Y_THRESHOLD : APP_IMAGE_Y_THRESHOLD; 
         pImage = ISHELL_LoadResImage(pme->a.m_pIShell, ai.pszMIF, APPR_IMAGE(ai));
      }
   }

   MEMSET(&mai, 0, sizeof(mai));
   mai.pText = (bSK ? NULL : AppMgr_GetAppName(pme, pi));
   mai.wItemID = itemID; 
   mai.dwData = (uint32)pi;

   //To menu, add image corresponding to the app's soft key image.
   if (pImage && ImageFits(pImage, NULL, NULL, 0, width, height))
      mai.pImage = pImage;
   else
   {
      AEEAppInfo ai; 
      ISHELL_QueryClass(pme->a.m_pIShell, pi->cls, &ai);
      mai.pszResImage = AEECONTROLS_RES_FILE;
      if(ai.wFlags & AFLAG_TOOL)
         mai.wImage = AEE_IDB_TOOL_THUMB;
      else
      {
         if(ai.wFlags & AFLAG_GAME)
            mai.wImage = AEE_IDB_GAME_THUMB;
         else
         {
            if(ai.wFlags & AFLAG_PIM)
               mai.wImage = AEE_IDB_PIMS_THUMB;
            else
               mai.wImage = ((nType == THUMB) ? AEE_IDB_BREW_THUMB : AEE_IDB_BREW);
         }
      }
   }
   
   // Add Menu Item
   IMENUCTL_AddItemEx(pMenu, &mai);

   // Free Image if Loaded
   FreeObj((void**)&pImage);
}

//===========================================================================
//
// DESCRIPTION
//    State Handler for ST_SCREENSAVERS
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_ScreenSavers(AppMgr * pme)
{
   CtlAddItem ai;
   AEEAppInfo appInfo;
   AEERect rc, rcm;
   uint16 sel = 0;

   uint16 wItemID = IDC_SCREEN_SAVER_APP;
   AEECLSID currentScreenSaver = ISHELL_GetHandler(pme->a.m_pIShell, AEECLSID_SCREEN_SAVER, MT_SCREEN_SAVER);

   // add title to the menu
   IMENUCTL_SetTitle(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDS_SELECT, NULL);
   
   // Add Menu Items
   MEMSET(&ai, 0, sizeof(ai));

   ai.pszResImage = APPMGRLN_RES_FILE;
   ai.pText    = NULL;
   ai.pImage   = NULL;
   ai.wFont    = AEE_FONT_NORMAL;

   // Enumerate all applets and add screen savers in this menu
   ISHELL_EnumAppletInit(pme->a.m_pIShell);
   while(ISHELL_EnumNextApplet(pme->a.m_pIShell, &appInfo)) {        
      // Add only screen savers in menu
      if(appInfo.wFlags & AFLAG_SCREENSAVER)
      {
         ai.pszResText  = appInfo.pszMIF;
         ai.wText    = APPR_NAME(appInfo);         
         ai.wItemID  = wItemID++;
         if (currentScreenSaver == appInfo.cls)
         {
            ai.wImage = IDB_RADIO_SEL;
            sel = ai.wItemID;
         }
         else          
            ai.wImage   = IDB_RADIO_UNSEL;
         ai.dwData   = appInfo.cls;
         IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
      }
   }

   if (IMENUCTL_GetItemCount(pme->m_pMenu) > 0)
   {
      // Add no screen saver option
      ai.pszResText = AEE_APPMGRLS_RES_FILE;
      ai.wText		= IDS_OPTIONS_NO_SCREEN_SAVER;
      ai.wItemID  = wItemID++;
      if (currentScreenSaver == 0)
      {
         ai.wImage = IDB_RADIO_SEL;
         sel = ai.wItemID;
      }
      else          
         ai.wImage   = IDB_RADIO_UNSEL;
      ai.dwData   = 0;
      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

      // Provide Set and Remove Softkey Options

      // Add Set
      IMENUCTL_AddItem(pme->m_pSK, AEE_APPMGRLS_RES_FILE, IDC_SET_SS, IDC_SET_SS, NULL, 0);

      // Add Remove
      IMENUCTL_AddItem(pme->m_pSK, AEE_APPMGRLS_RES_FILE, IDC_REMOVE_SS, IDC_REMOVE_SS, NULL, 0);

      // Get Softkey Menu Size
      IMENUCTL_GetRect(pme->m_pSK, &rcm);

      // Set Menu Size
      SETAEERECT(&rc, 0, 0, pme->m_rc.dx, rcm.y - 1);
      IMENUCTL_SetRect(pme->m_pMenu, &rc);

      // Set Selection
      IMENUCTL_SetSel(pme->m_pMenu, sel);

      // Set Both Menu's Active
      IMENUCTL_SetActive(pme->m_pSK, TRUE);   
      IMENUCTL_SetActive(pme->m_pMenu, TRUE);   
   
      // Update UI
      IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
   }
   else
   {
      uint16 wButtons[2];

      // Redraw Options Menu
      AppMgr_Options(pme);

      // Make Sure that underlying menu isn't active
      IMENUCTL_SetActive(pme->m_pMenu, FALSE);

      // Prompt structure with information that there are no screen savers
      MEMSET(wButtons, 0, sizeof(wButtons));
      wButtons[0] = IDC_OK;

      AppMgr_Prompt(pme, NULL, NULL, NULL, IDS_OPTIONS_SCREEN_SAVERS, wButtons, IDC_OK, ST_NOSCROLL, TRUE);
   }
   return(TRUE);
}

//===========================================================================
//
// DESCRIPTION
//    State Handler for ST_EDITSOUNDIMAGE
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
boolean AppMgr_EditSoundImage(AppMgr * pme)
{
   AEERect rcm;
   //AECHAR szName[64];
   IImage* pImage = NULL;
   AEEImageInfo ii;
   int strwd;

   // Set properties to display maximum fully displayable items
#ifdef MP_ALLOW_DUPE_IDS
   IMENUCTL_SetProperties(pme->m_pSK, MP_MAXSOFTKEYITEMS|MP_ALLOW_DUPE_IDS);
#else
   IMENUCTL_SetProperties(pme->m_pSK, MP_MAXSOFTKEYITEMS);
#endif // MP_ALLOW_DUPE_IDS

   // Provide Softkey Options

   // Add Done
   IMENUCTL_AddItem(pme->m_pSK, AEE_APPMGRLS_RES_FILE, IDC_CONFIGURE_DONE, IDC_CONFIGURE_DONE, NULL, 0);

   // Add Configure Image
   if (OEM_SET_IMAGE)
      IMENUCTL_AddItem(pme->m_pSK, AEE_APPMGRLS_RES_FILE, IDC_CONFIGURE_IMAGE, IDC_CONFIGURE_IMAGE, NULL, 0);

   //Do not give an option to choose sound files if ISoundPlayer cannot be
   //created on the handset
   if(pme->m_pISp) 
      IMENUCTL_AddItem(pme->m_pSK, AEE_APPMGRLS_RES_FILE, IDC_CONFIGURE_SOUND, IDC_CONFIGURE_SOUND, NULL, 0);

   // Add Reset
   IMENUCTL_AddItem(pme->m_pSK, AEE_APPMGRLS_RES_FILE, IDC_CONFIGURE_RESET, IDC_CONFIGURE_RESET, NULL, 0);

   // Get Softkey Menu Size
   IMENUCTL_GetRect(pme->m_pSK, &rcm);

   AppMgr_GetAppImage(pme, pme->m_pCurrentApp, &pImage);

   strwd = IDISPLAY_MeasureText(pme->a.m_pIDisplay, AEE_FONT_BOLD, AppMgr_GetAppName(pme, pme->m_pCurrentApp));

   IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_BOLD, AppMgr_GetAppName(pme, pme->m_pCurrentApp), -1, 
                     (pme->m_rc.dx - strwd)/2, MARGIN - 1, NULL, IDF_TEXT_TRANSPARENT);

   // Start/Draw Image
   if(pImage != NULL)
   {
      IIMAGE_GetInfo(pImage, &ii);
      IIMAGE_SetParm(pImage, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
      // Start/Draw It
      if (ii.bAnimated)
      {
         IIMAGE_Start(pImage, (pme->m_rc.dx - ii.cx)/2, (rcm.y - ii.cy)/2);
         pme->m_pAnimatedImage = pImage;
      }
      else
      {
         IIMAGE_Draw(pImage, (pme->m_rc.dx - ii.cx)/2, (rcm.y - ii.cy)/2);
         IIMAGE_Release(pImage);
      }
      pImage = NULL;
   }

   // Play Sound if possible and necessary
   if(pme->m_pISp && *pme->m_pCurrentApp->szSound)
   {
      pme->m_spInfo.pData = (void *)pme->m_pCurrentApp->szSound;
      AppMgr_PlayRetry(pme);
   }

   // Set Selection if specified
   if (pme->m_nSelCache)
   {
      IMENUCTL_SetSel(pme->m_pSK, pme->m_nSelCache);
      pme->m_nSelCache = 0;
   }

   // Set Softkey Menu Active
   IMENUCTL_SetActive(pme->m_pSK, TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);

   return TRUE;
}

//===========================================================================
//
// DESCRIPTION
//    State Handler for ST_ORDERMAINMENU, ST_ORDERFAVORITES, ST_ORDERWORK, 
//    ST_ORDERFUN, ST_MOVEMAINMENU, ST_MOVEFAVORITES, ST_MOVEWORK, ST_MOVEFUN
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_Config(AppMgr * pme)
{
   uint16 nIndex, i, nCount, itemID;
   AppMgrItem * pi = NULL;
   boolean bShowPrompt = FALSE;

   if (pme->m_wState >= ST_MOVEMAINMENU)
   {
      nIndex = pme->m_wState - ST_MOVEMAINMENU;
      itemID = IDC_MOVE_APP;
   }
   else
   {
      nIndex = pme->m_wState - ST_ORDERMAINMENU;
      itemID = IDC_ORDER_APP;
   }

   nCount = pme->m_nItem[nIndex];

   if (nCount)
   {
      pi = pme->m_pList[nIndex];

      for (i = 0; i < nCount; i++)
      {
         // Don't let customer move MobileShop
         if (!OEM_LOCK_MOBILESHOP_LOCATION && (pme->m_wState == ST_MOVEMAINMENU) && (pi->cls == AEECLSID_MOBILESHOP_BID))
         {
            pi = pi->pNext;
            continue;
         }

         AppMgr_AddMenuItem(pme, pme->m_pMenu, pi, itemID++, THUMB, FALSE);
         pi = pi->pNext;
      }

      if (IMENUCTL_GetItemCount(pme->m_pMenu))
      {
         // add title to the menu
         IMENUCTL_SetTitle(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, IDS_SELECT_APP, NULL);      
         
         // Set Size
         IMENUCTL_SetRect(pme->m_pMenu, &pme->m_rc);

         // Set Main Menu Active
         IMENUCTL_SetActive(pme->m_pMenu, TRUE);

         // Update UI
         IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
      }
      else
         bShowPrompt = TRUE;
   }
   else
      bShowPrompt = TRUE;

   if (bShowPrompt)
   {
      uint16 wButtons[2];
      MEMSET(wButtons, 0, sizeof(wButtons));
      wButtons[0] = IDC_OK;

      // Redraw category menu to display prompt above category list
      AppMgr_Categories(pme);
      
      // Make Sure that underlying menu isn't active
      IMENUCTL_SetActive(pme->m_pMenu, FALSE);

      // Display a prompt indicating that there are no apps in this category
      AppMgr_Prompt(pme, NULL, 0, NULL, IDS_EMPTY_CATEGORY, wButtons, wButtons[0], ST_NOSCROLL, TRUE);
   }

   return(TRUE);
}

/*===========================================================================

Displays memory use information in options menu

===========================================================================*/
static void AppMgr_DisplayMemoryInfo(AppMgr * pme)
{
   AEERect     rc;
   uint32      dwFree, dwTotal, dwUsed;
   AECHAR      szBuff[32], szFormat[32], szFree[10], szUsed[10], szTotal[10];
   uint16      y, wHeight, wSel;

   // Get height of the rectangle
   wHeight = 2*(pme->m_cyFont + MARGIN);

   // Get y-coordinate of the rectangle
   y = pme->m_rc.dy - wHeight - MARGIN;

   // Draw Horizontal line
   IDISPLAY_DrawHLine(pme->a.m_pIDisplay, 0, y - MARGIN, pme->m_rc.dx);

   // Erase Rectangle area
   SETAEERECT(&rc, 0, y, pme->m_rc.dx, wHeight);
   IDISPLAY_EraseRect(pme->a.m_pIDisplay, &rc);

    // Draw Rectangle
   SETAEERECT(&rc, MARGIN, y, pme->m_rc.dx - 2*MARGIN, wHeight);
   IDISPLAY_DrawRect(pme->a.m_pIDisplay, &rc, RGB_BLACK, 0, IDF_RECT_FRAME);

   wSel = IMENUCTL_GetSel(pme->m_pMenu);

   if (wSel == IDC_OPT_INFO)
   {
      DLItemSize  is;

      // Get Available Space
      IDOWNLOAD_GetAvailable(pme->m_pDownload, &is);

      // Get total space & round it off
      dwTotal = (is.dwFSSize/ONE_K)*ONE_K;

      // Get free space & round it off
      dwFree = (is.dwFSAvail/ONE_K)*ONE_K;

      // Calculate used space
      dwUsed = dwTotal - dwFree;

      FormatMem(dwFree, szFree, FALSE);
      FormatMem(dwUsed, szUsed, FALSE);
      FormatMem(dwTotal, szTotal, FALSE);

      // Display "%s available"
      ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDS_MT_AVAILABLE, szFormat, sizeof(szFormat));
      WSPRINTF(szBuff, sizeof(szBuff), szFormat, szFree);
      IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_NORMAL, szBuff, -1, 2*MARGIN, y + MARGIN, NULL, 0);

      // Display "%s used (%s total)"
      ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDS_MT_USED, szFormat, sizeof(szFormat));
      WSPRINTF(szBuff, sizeof(szBuff), szFormat, szUsed, szTotal);
      IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_NORMAL, szBuff, -1, 2*MARGIN, y + pme->m_cyFont + MARGIN, NULL, 0);
   }
   else
   {
      AppModInfo  *  pmi;
      AECHAR *       pText;

      // Get item data for the selection
      IMENUCTL_GetItemData(pme->m_pMenu, wSel, (uint32*)&pme->m_pCurrentApp);

      // Get module information for the current app
      if (!AppMgr_GetItemID(pme, pme->m_pCurrentApp))
         return;

      pmi = IDOWNLOAD_GetModInfo(pme->m_pDownload, AppMgr_GetItemID(pme, pme->m_pCurrentApp));

      if (!pmi)
      {
         if (pme->m_pCurrentApp)
            pme->m_pCurrentApp->dwItemID = 0;
         AppMgr_SetState(pme, PREV_ENTRY);
         return;
      }

      // App size string
      FormatMem(pmi->dwSpaceTotal, szTotal, FALSE);

      pText = FormatString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDS_MT_MEMORYUSE, szTotal); 

      if (pText)
      {
         IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_NORMAL, pText, -1, 2*MARGIN, y + MARGIN, NULL, 0);
         FREE(pText);
      }

      // Expire String
      *szBuff = 0;
      GetExpireString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, 0, pmi->li.lt, pmi->li.dwExpire, szBuff, sizeof(szBuff), TRUE,FALSE);
      IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_NORMAL, szBuff, -1, 2*MARGIN, y + pme->m_cyFont + MARGIN, NULL, 0);
   }
}

/*===========================================================================

Validates an DLITEMID against list of IDs returned by IDOWNLOAD_GetAppIDList()
to ensure that DLITEMID doesn't belong to a protected pre-installed application.

===========================================================================*/
static boolean IsValidItemIDForManageApps(AppMgr * pme, AppMgrItem* pAppMgrItem)
{
   uint32* pIDs;
   uint32* pdw;
   uint32 dwItemID;

   if (!pAppMgrItem)
      return FALSE;

   dwItemID = AppMgr_GetItemID(pme, pAppMgrItem);

   if (!dwItemID)
      return FALSE;

   if (pme->m_pDownload)
   {
      pIDs = IDOWNLOAD_GetAppIDList(pme->m_pDownload);
      if(pIDs)
      {
         for(pdw = pIDs; *pdw != 0; pdw++)
         {
            // Ignore versions in first pass
            if (GET_DLITEMID_FAMILY(*pdw) == GET_DLITEMID_FAMILY(dwItemID))
            {
               // Test for exact match now
               if (*pdw == dwItemID)
               {
                  return TRUE;
               }
               else
               {
                  // Guard against our mapping of class ID and item ID having gone sore
                  // because of ever evolving IDownload features like auto upgrade,
                  // license update on DIF_AUTO_UPGRADE, etc
                  AppModInfo * pai = IDOWNLOAD_GetModInfo(pme->m_pDownload, *pdw);

                  if (pai)
                  {
                     uint16 index;

                     for (index = 0; index < pai->nApps; index++)
                     {
                        if (pai->apps[index].cls == pAppMgrItem->cls)
                        {
                           pAppMgrItem->dwItemID = *pdw;
                           return TRUE;
                        }
                     }
                  }
               }
            }
         }
      }
   }

   return FALSE;
}


//===========================================================================
//
// DESCRIPTION
//    State Handler for ST_MANAGETOOL
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_ManageTool(AppMgr * pme)
{
   CtlAddItem  ai;
   AEERect     rc;   
   uint16      index;
   uint32      nDraw;

   AppMgrItem* pAppMgrItem = NULL;
   IImage*     pDisabled = NULL;
   IImage*     pDefault = NULL;

   uint16      wID = IDC_APP;

   // Set Menu Size
   if (!OEM_SMALL_SCREEN_DEVICE)
      // Set Menu Control size so that memory information can be displayed
      SETAEERECT(&rc, 0, 0, pme->m_rc.dx, pme->m_rc.dy - 2*pme->m_cyFont - 4*MARGIN);
   else
      // Set Menu Control size
      SETAEERECT(&rc, 0, 0, pme->m_rc.dx, pme->m_rc.dy);

   // Set Menu Rectangle
   IMENUCTL_SetRect(pme->m_pMenu, &rc);

   nDraw = (pme->m_cyFont ? rc.dy/pme->m_cyFont : rc.dy/ITEM_HEIGHT);
   
   // Add "System Info." in options menu
   MEMSET(&ai,0,sizeof(ai));
   ai.pszResText  = AEE_APPMGRLS_RES_FILE;
   ai.pszResImage = APPMGRLN_RES_FILE;

   ai.wItemID     = IDC_OPT_INFO;
   ai.wText       = IDC_OPT_INFO;
   ai.wImage      = IDB_MGMT_SYSINFO;

   // Add Item
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   nDraw--;

   // Load Disabled and Default Images
   // pDisabled = ISHELL_LoadResImage(pme->a.m_pIShell, APPMGRLN_RES_FILE, IDB_DISABLED);
   pDefault = ISHELL_LoadResImage(pme->a.m_pIShell, APPMGRLN_RES_FILE, IDB_APP_DEFAULT); 

   // Add app names in menu with "Disabled"/"Installed" icons
   for (index = 0; index < MAX_APP_CATS; index++)
   {
      pAppMgrItem = pme->m_pList[index];
      while (pAppMgrItem)
      { 
         // Ensure that only the applications present in pIDs are added in "ManageApps"
         // as we don't want "ManageApps" operations on MOD_PROTECTED applications
         if ((pAppMgrItem->cls != AEECLSID_MOBILESHOP_BID) &&
             IsValidItemIDForManageApps(pme, pAppMgrItem))
         {
            ai.pText = AppMgr_GetAppName(pme, pAppMgrItem);
         
            // Disabling AppMgr_AppStatus use here as this results in IDownload_GetModInfo() 
            // being called for every app with valid DLITEMID and significantly delays UI
            // construction

            /*if(AppMgr_AppStatus(pme, AppMgr_GetItemID(pme, pAppMgrItem)) == DISABLED)
               ai.pImage = pDisabled;
            else*/
               ai.pImage = pDefault;

            ai.wItemID = wID++;
            ai.dwData = (uint32)pAppMgrItem;      // Set app manager item as data
               
            IMENUCTL_AddItemEx(pme->m_pMenu, &ai);         
            
            if (nDraw != 0xffffffff)
            {
               if (!nDraw)
               {
                  nDraw = 0xffffffff;

                  // Make Menu Active
                  IMENUCTL_SetActive(pme->m_pMenu, TRUE);

                  // Preview memory info for large screen devices
                  if (!OEM_SMALL_SCREEN_DEVICE)
                     AppMgr_DisplayMemoryInfo(pme);

                  IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
               }
               else
                  nDraw--;
            }
         }
         pAppMgrItem = pAppMgrItem->pNext;
      }
   }

   // Free Disabled and Default Images
   FreeObj((void**)&pDisabled);
   FreeObj((void**)&pDefault);

   // Set Selection
   if (pme->m_nIndex > 1)
   {
      // IMENUCTL_SetSel(pme->m_pMenu, pme->m_nMenuSel[pme->m_nIndex--]);
      pme->m_nIndex--;
   }

   // Set Menu Active            
   if (nDraw != 0xffffffff)
   {
      IMENUCTL_SetActive(pme->m_pMenu, TRUE); 

      // Preview memory info for large screen devices
      if (!OEM_SMALL_SCREEN_DEVICE)
         AppMgr_DisplayMemoryInfo(pme);
   
      // Update UI
      IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
   }
   return(TRUE);
}

//===========================================================================
//
// DESCRIPTION
//    State Handler for ST_APPINFO
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
static boolean AppMgr_AppInfo(AppMgr * pme)
{
   CtlAddItem     ai;
   uint32         dwADSCaps;
   AppModInfo*    pmi = NULL;

   MEMSET(&ai,0,sizeof(ai));

   ai.pszResText  = AEE_APPMGRLS_RES_FILE;
   ai.pszResImage = APPMGRLN_RES_FILE;

   // Check pme->m_pCurrentApp and pme->m_pCurrentApp->dwItemID as we may be
   // coming here after an upgrade check followed by download

   if (!pme->m_pCurrentApp || !AppMgr_GetItemID(pme, pme->m_pCurrentApp))
   {
      AppMgr_SetState(pme, PREV_ENTRY);
      return TRUE;
   }

   // Get module information for the current app
   pmi = IDOWNLOAD_GetModInfo(pme->m_pDownload, AppMgr_GetItemID(pme, pme->m_pCurrentApp));

   if (!pmi)
   {
      if (pme->m_pCurrentApp)
         pme->m_pCurrentApp->dwItemID = 0;
      AppMgr_SetState(pme, PREV_ENTRY);
      return TRUE;
   }

   // Set Menu Title
   IMENUCTL_SetTitle(pme->m_pMenu, NULL, 0, AppMgr_GetAppName(pme, pme->m_pCurrentApp));

   // Add View Properties
   ai.wItemID     = ai.wText = IDC_VIEW_PROPERTIES;
   ai.wImage      = IDB_MGMT_VIEW_PROPERTIES;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   // Add Set Sound/Image
   ai.wItemID     = IDC_SET_SOUND_IMAGE;
   if (OEM_SET_IMAGE && pme->m_pISp)
   {
      ai.wText    = IDC_SET_SOUND_IMAGE;
      ai.wImage   = IDB_SET_SOUND_IMAGE;
      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   }
   else if (pme->m_pISp)
   {
      ai.wText    = IDS_SET_SOUND;
      ai.wImage   = IDB_SET_SOUND;
      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   }
   else if (OEM_SET_IMAGE)
   {
      ai.wText    = IDS_SET_IMAGE;
      ai.wImage   = IDB_SET_IMAGE;
      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   }

   // Add Set Category
   ai.wItemID     = ai.wText = IDC_SET_CATEGORY;
   ai.wImage      = IDB_MGMT_SET_CATEGORY;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   // Add Lock Apps
   ai.wItemID     = ai.wText = IDC_LOCK_APP;
   ai.wImage      = IDB_MGMT_LOCK;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   // Add Remove
   ai.wItemID     = IDC_REMOVE;
   ai.wText       = (pmi && (pmi->li.pt == PT_SUBSCRIPTION)) ? IDS_CANCEL_SUBSCRIPTION : IDC_REMOVE;
   ai.wImage      = IDB_MGMT_REMOVE;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   // Include UpgradeCheck only if:
   // 1. we can access server capabilities and this is supported by server
   // 2. we can't access server capabilities and OEM factory settings want it
   dwADSCaps = IDOWNLOAD_GetADSCapabilities(pme->m_pDownload);
   if (dwADSCaps)
   {
      if (dwADSCaps & ADS_CAP_UPGRADECHECK)
      {
         ai.wItemID     = ai.wText = IDC_UPGRADE_CHECK;
         ai.wImage      = IDB_MGMT_UPGRADE_CHECK;
         IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
      }
   }
   else
   {
      if (pme->m_MShopCfg.bUpgrade)
      {
         ai.wItemID     = ai.wText = IDC_UPGRADE_CHECK;
         ai.wImage      = IDB_MGMT_UPGRADE_CHECK;
         IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
      }
   }

   if (pme->m_nIndex > 2)
      IMENUCTL_SetSel(pme->m_pMenu, pme->m_nMenuSel[pme->m_nIndex--]);

   // Set menu control active
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);

   return(TRUE);
}

//===========================================================================
//
// DESCRIPTION
//    Displays help information
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================
void AppMgr_HelpDisplay(AppMgr* pme, uint16 nCmd)
{
   uint16 wCommandStr;
   uint16 wButtons[2];

   wCommandStr = nCmd + COMMAND_STR_DELTA;

   MEMSET(wButtons, 0, sizeof(wButtons));
   wButtons[0] = IDC_CLOSE;

   // Cache Current Menu Selection
   pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);

   // Reset Controls
   AppMgr_Reset(pme, TRUE);

   // Display Prompt
   AppMgr_Prompt(pme, NULL, nCmd, NULL, wCommandStr, wButtons, wButtons[1], ST_NOSCROLL, FALSE);
}

//===========================================================================
//
// DESCRIPTION
//    Sets the state machines state according to the specified value
// 
// PARAMETERS
//    pme   :  AppMgr
//    nState:  state
//
//===========================================================================
static boolean AppMgr_SetState(AppMgr * pme, int nState)
{
   AppMgrState  * ps;

   if(nState == PREV_ENTRY){
      // Coming here means state is to be set to the back state

      // Get back state of the current state
      nState = (int)pme->m_states[pme->m_wState].nBack;

      // If back state is LAST_STATE, set saved last state as back state.
      // Otherwise set back state as current state.
      if(nState == LAST_STATE)
         pme->m_wState = pme->m_wLastState;
      else
         pme->m_wState = (int16)nState;

      // If back state of the current state is NO_ENTRY, return FALSE
      // so that OEMs close the App Manager. Also reset pme->m_wState
      // and pme->m_wLastState so that App Manager is healthy if it
      // wasn't closed.
      if(pme->m_wState == NO_ENTRY)
      {
         pme->m_wLastState = NO_ENTRY;
         pme->m_wState = ST_MAINMENU;
         return FALSE;
      }
   }
   else
      pme->m_wState = (int16)nState;      // Set specified state

#ifdef CUST_EDITION
   if(pme->m_wState == NO_ENTRY)
   {
       pme->m_wState = ST_MAINMENU;
   }
#endif

   ps = (AppMgrState *)&pme->m_states[pme->m_wState];

   if(ps->bClear)
      AppMgr_Reset(pme, TRUE); // Reset display

   // Call state handler
   return(ps->pfnReq(pme));
}

/*===========================================================================

State handler for  ST_VIEWPROPERTIES. Displays app properties.

===========================================================================*/
static boolean AppMgr_ViewProperties(AppMgr * pme)
{
   IShell *       ps = pme->a.m_pIShell;
   uint16         wButtons[2];
   AppModInfo  *  pmi;
   AEEAppInfo *   pai;
   AECHAR         szBuff[32],szFormat[32],szMem[10];
   char *         pszRes;
   AECHAR   *     pText = NULL;

   MEMSET(wButtons,0,sizeof(wButtons));

   if (!AppMgr_GetItemID(pme, pme->m_pCurrentApp))
      return TRUE;

   // Get module information for the current app
   pmi = IDOWNLOAD_GetModInfo(pme->m_pDownload, AppMgr_GetItemID(pme, pme->m_pCurrentApp));

   if (!pmi)
   {
      if (pme->m_pCurrentApp)
         pme->m_pCurrentApp->dwItemID = 0;
      AppMgr_SetState(pme, PREV_ENTRY);
      return TRUE;
   }

   pai      = pmi->apps;
   pszRes   = pai->pszMIF;

   // Set prompt structure
   wButtons[0] = IDC_OK;       // OK

   pText = NULL;

   // Total app size string
   ISHELL_LoadResString(ps, AEE_APPMGRLS_RES_FILE, IDS_APP_TOTALSIZE, szFormat, sizeof(szFormat));
   FormatMem(pmi->dwSpaceTotal, szMem, FALSE);
   WSPRINTF(szBuff, sizeof(szBuff), szFormat, szMem);
   pText = AppendWStr(pText,szBuff,'\n');

   // App size string
   ISHELL_LoadResString(ps, AEE_APPMGRLS_RES_FILE, IDS_APP_APPSIZE, szFormat, sizeof(szFormat));
   FormatMem(pmi->dwSpaceApp, szMem, FALSE);
   WSPRINTF(szBuff, sizeof(szBuff), szFormat, szMem);
   pText = AppendWStr(pText,szBuff,'\n');

   // App data size string
   ISHELL_LoadResString(ps, AEE_APPMGRLS_RES_FILE, IDS_APP_APPDATASIZE, szFormat, sizeof(szFormat));
   FormatMem(pmi->dwSpaceTotal - pmi->dwSpaceApp, szMem, FALSE);
   WSPRINTF(szBuff, sizeof(szBuff), szFormat, szMem);
   pText = AppendWStr(pText,szBuff,'\n');

   // Expire String
   *szBuff = 0;
   GetExpireString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, 0, pmi->li.lt, pmi->li.dwExpire, szBuff, sizeof(szBuff), TRUE,FALSE);
   if(*szBuff)
   {
      AECHAR szTemp[32];

      // App License String
      ISHELL_LoadResString(ps, AEE_APPMGRLS_RES_FILE, IDS_APP_LICENSE, szFormat, sizeof(szFormat));
      WSPRINTF(szTemp, sizeof(szTemp), szFormat, szBuff);
      pText = AppendWStr(pText,szTemp,'\n');
   }

   if(pmi->dwLastUsedTime){
      AECHAR szTemp[32];

      // Last Used
      ISHELL_LoadResString(ps, AEE_APPMGRLS_RES_FILE, IDS_APP_LASTUSED, szFormat, sizeof(szFormat));
      GetDateString(pme->a.m_pIShell, pmi->dwLastUsedTime, szBuff,sizeof(szBuff));
      WSPRINTF(szTemp, sizeof(szTemp), szFormat, szBuff);
      pText = AppendWStr(pText,szTemp,'\n');
   }

   // Copyright string
   if(ISHELL_LoadResString(ps, pszRes, IDS_MIF_COPYRIGHT, szBuff, sizeof(szBuff)))
      pText = AppendWStr(pText,szBuff,'\n');

   // Company string
   if(ISHELL_LoadResString(ps, pszRes, IDS_MIF_COMPANY, szBuff, sizeof(szBuff)))
      pText = AppendWStr(pText,szBuff,'\n');

   // Version string
   if(ISHELL_LoadResString(ps, pszRes, IDS_MIF_VERSION, szBuff, sizeof(szBuff)))
      pText = AppendWStr(pText,szBuff,'\n');

   // Application Status
   switch (AppMgr_AppStatus(pme, AppMgr_GetItemID(pme, pme->m_pCurrentApp)))
   {
      case DISABLED:
         if(ISHELL_LoadResString(ps, AEE_APPMGRLS_RES_FILE, IDS_LAUNCHER_DISABLED, szBuff, sizeof(szBuff)))
            pText = AppendWStr(pText,szBuff,'\n');
         break;

      case DEMO:
         if(ISHELL_LoadResString(ps, AEE_APPMGRLS_RES_FILE, IDS_LAUNCHER_DEMO, szBuff, sizeof(szBuff)))
            pText = AppendWStr(pText,szBuff,'\n');
         break;

      case EXPIRED:
         if(ISHELL_LoadResString(ps, AEE_APPMGRLS_RES_FILE, IDS_LAUNCHER_EXPIRED, szBuff, sizeof(szBuff)))
            pText = AppendWStr(pText,szBuff,'\n');
         break;

      case EXPIREDDEMO:
         if(ISHELL_LoadResString(ps, AEE_APPMGRLS_RES_FILE, IDS_LAUNCHER_EXPIREDDEMO, szBuff, sizeof(szBuff)))
            pText = AppendWStr(pText,szBuff,'\n');
         break;
   }

   // Display Prompt with Properties Information
   AppMgr_Prompt(pme, AppMgr_GetAppName(pme, pme->m_pCurrentApp), 0, pText, 0, wButtons, wButtons[0], CP_USE_DEFAULT|ST_TEXTALLOC|ST_NOSCROLL, FALSE);

   return TRUE;
}

/*===========================================================================

   State handler for  ST_REMOVE. 

===========================================================================*/
static boolean AppMgr_Remove(AppMgr * pme)
{
   AECHAR*     pTitle;
   uint16      wButtons[3], wRemoveID;  
   AppModInfo* pmi = NULL;
   AEEAppInfo  ai;

   if (!AppMgr_GetItemID(pme, pme->m_pCurrentApp))
      return TRUE;

   // Get module information for the current app
   pmi = IDOWNLOAD_GetModInfo(pme->m_pDownload, AppMgr_GetItemID(pme, pme->m_pCurrentApp));

   if (!pmi)
   {
      if (pme->m_pCurrentApp)
         pme->m_pCurrentApp->dwItemID = 0;
      AppMgr_SetState(pme, PREV_ENTRY);
      return TRUE;
   }

   // Get prompt title
   pTitle = LoadAppNames(pme->a.m_pIShell, pmi->apps, pmi->nApps);

   // Check if request is for screen saver
   ISHELL_QueryClass(pme->a.m_pIShell, pme->m_pCurrentApp->cls, &ai);

   if (ai.wFlags & AFLAG_SCREENSAVER)
      wRemoveID = IDS_SS_REMOVE;
   else
   {
      AppStatus   nAppStatus = AppMgr_AppStatus(pme, AppMgr_GetItemID(pme, pme->m_pCurrentApp));

      if ((nAppStatus == EXPIRED) || (nAppStatus == EXPIREDDEMO))
         wRemoveID = IDS_MT_REMOVE_EXPIRED;
      else
         wRemoveID = (pmi->li.pt == PT_SUBSCRIPTION) ? IDS_MT_CANCEL_SUBSCRIPTION : IDS_MT_REMOVE;
   }

   MEMSET(wButtons, 0, sizeof(wButtons));
   wButtons[0] = IDC_YES;
   wButtons[1] = IDC_NO;

   AppMgr_Prompt(pme, pTitle, 0, NULL, wRemoveID, wButtons, wButtons[0], ST_NOSCROLL, FALSE);

   FreePtr((void**)&pTitle);

   return(TRUE);
}

/*===========================================================================

   Show Airtime Warning

===========================================================================*/
static boolean AppMgr_ShowAirtimeWarning(AppMgr * pme, uint16 wWarningID)
{
   // Reset UI
   AppMgr_Reset(pme, FALSE);

   if (!pme->m_bWarningDisplayed)
   {
      uint16 wButtons[2];

      // Set Warning Displayed TRUE
      pme->m_bWarningDisplayed = TRUE;

      // Prompt structure with information that displays warning
      MEMSET(wButtons, 0, sizeof(wButtons));
      wButtons[0] = IDC_OK;

      AppMgr_Prompt(pme, NULL, 0, NULL, wWarningID, wButtons, wButtons[0], ST_NOSCROLL, TRUE);
   }
   else
   {
      uint16 wButtons[3];

      // Prompt structure with information to disable airtime warning
      MEMSET(wButtons, 0, sizeof(wButtons));
      wButtons[0] = IDC_YES;
      wButtons[1] = IDC_REMOVE_WARNING;

      AppMgr_Prompt(pme, NULL, 0, NULL, wWarningID, wButtons, wButtons[0], ST_NOSCROLL, TRUE);
   }
   return TRUE;
}

/*===========================================================================

State handler for ST_SYSTEMINFO

===========================================================================*/
static boolean AppMgr_SystemInfo(AppMgr * pme)
{
   AEERect     rc;
   uint32      dwUsed, dwTotal, dwFree;
   AECHAR      szBuff[32], szFormat[32];
   IDisplay *  pd;
   int         cyFont, wPct;
   DLItemSize  is;

   pd = pme->a.m_pIDisplay;
   cyFont = pme->m_cyFont;

   SETAEERECT(&rc, 0, 0, pme->m_rc.dx, pme->m_rc.dy);

   // Draw the title on first line in the center
   ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDC_OPT_INFO, szBuff, sizeof(szBuff));
   IDISPLAY_DrawText(pd,AEE_FONT_BOLD, szBuff, -1, 0, rc.y, &rc, IDF_ALIGN_CENTER);

   // Underline title
   rc.y += cyFont + 1;
   rc.dy = 1;
   IDISPLAY_FillRect(pd,&rc,CLR_USER_FRAME);

   // Draw the memory information
   rc.y += 1;
   rc.dy = cyFont;

   // Get Available Space
   IDOWNLOAD_GetAvailable(pme->m_pDownload, &is);

   // Get total space & round it off
   dwTotal = (is.dwFSSize/ONE_K)*ONE_K;

   // Get free space & round it off
   dwFree = (is.dwFSAvail/ONE_K)*ONE_K;

   // Calculate used space
   dwUsed = dwTotal - dwFree;

   if(dwTotal)
      wPct = (uint16)((dwUsed * 100) / dwTotal);
   else
      wPct = 100;

   // Display "Memory wPct% Full"
   ISHELL_LoadResString(pme->a.m_pIShell, AEE_APPMGRLS_RES_FILE, IDS_MEM_FORMAT, szFormat, sizeof(szFormat));
   WSPRINTF(szBuff, sizeof(szBuff), szFormat, wPct);
   IDISPLAY_DrawText(pd,AEE_FONT_BOLD, szBuff, -1, 0, rc.y, &rc, IDF_ALIGN_CENTER);

   // Draw memory histogram
   rc.y += cyFont + 1;

   rc.x = 5;
   rc.dx = pme->m_rc.dx - 10;
   rc.dy = cyFont;

   AppMgr_DrawMemHist(pme, dwUsed, dwTotal, &rc);

   rc.y += rc.dy + 1;
   rc.dy = pme->m_rc.dy - rc.y;

   AppMgr_MoreInfo(pme, &rc);

   IDISPLAY_UpdateEx(pd, FALSE);

   return(TRUE);
}

/*===========================================================================

Draws histogram in specified rectangle according to the memory used and memory
available.

===========================================================================*/
static void AppMgr_DrawMemHist(AppMgr * pme, uint32 dwUsed, uint32 dwTotal, AEERect * prc)
{
   IDisplay *  pd = pme->a.m_pIDisplay;
   AECHAR      szBuff[32];
   AECHAR      szTotal[10];
   AECHAR *    psz;
   uint16      wPct;
   AEERect     rc;

   rc = *prc;

   // Format the memory strings
   FormatMem(dwUsed, szBuff, FALSE);
   FormatMem(dwTotal, szTotal, FALSE);

   psz = szBuff + WSTRLEN(szBuff);
   *(psz++) = (AECHAR)' ';
   *(psz++) = (AECHAR)'/';
   *(psz++) = (AECHAR)' ';
   WSTRCPY(psz,szTotal);

   // Draw the memory histogram...
   dwUsed /= ONE_K;
   dwTotal /= ONE_K;

   if(dwTotal)
      wPct = (uint16)((dwUsed * 100) / dwTotal);
   else
      wPct = 100;

   DrawHist(pd, &rc, wPct);

   // Draw the memory string as "memory used"/"memory available"

   rc.y += rc.dy + 2;
   rc.x = 0;
   rc.dx = pme->m_rc.dx;
   rc.dy = pme->m_cyFont;

   *prc = rc;

   IDISPLAY_DrawText(pd,AEE_FONT_NORMAL, szBuff, -1, 0, rc.y, &rc, IDF_ALIGN_CENTER);
}

//===========================================================================
//
// DESCRIPTION
//     Add an file to file list
//
// PARAMETERS
//    pme:  the app mgr
//    szfile:  the file name
//
//Return Value:
//    SUCCESS - 0
//    FAILURE - Error code
//   
//===========================================================================
static int AppMgr_AddFile(AppMgr * pme, char * szfile)
{
   FileItem * p;

   if(!szfile)
      return EFAILED;

   pme->m_nFileCount++;
   p = (FileItem *) REALLOC(pme->m_pFileList, pme->m_nFileCount * sizeof(FileItem));
   if(p == NULL) 
   {
      pme->m_nFileCount--;
      return ENOMEMORY;
   }

   pme->m_pFileList = p;
   p = &pme->m_pFileList[pme->m_nFileCount - 1];
   MEMSET(p, 0, sizeof(FileItem));

   STRCPY(p->szFile, szfile);
   return AEE_SUCCESS;
     
}
//===========================================================================
// DESCRIPTION
//    Show all image or sound files    
//
// PARAMETERS
//    pme:  the app mgr
//    image:   if set, show image files, otherwise, show sound files
//
//===========================================================================
static void AppMgr_ShowFiles(AppMgr * pme, boolean bImage )
{
   FileInfo    fi;
   uint16      i = IDC_CONFIGURE_APP;
   AECHAR      wszName[MAX_FILE_NAME];
   char *      subStr = NULL;
   char *      ext = NULL;
   char        szBuf[MAX_FILE_NAME];
   int         imageIndex, soundIndex;

   // Reset File List
   pme->m_nFileCount = 0;
   if(pme->m_pFileList) 
   {
      FREE(pme->m_pFileList);
      pme->m_pFileList = NULL;
   }

   // Disable Softkey Menu
   IMENUCTL_SetActive(pme->m_pSK, FALSE);

   // Clear Display
   IDISPLAY_ClearScreen(pme->a.m_pIDisplay);

   // Stop Sound & Animation
   AppMgr_StopSoundAnimation(pme);

   //Initialize index values
   imageIndex = 0;
   soundIndex = 0;

   while(TRUE)
   {
      *szBuf = 0;

      if(bImage)
      {
         switch(imageIndex)
         {
         case 0:
            STRCPY(szBuf,BMP_FILES);
            imageIndex++;
            break;

         case 1:
            STRCPY(szBuf,GIF_FILES);
            imageIndex++;
            break;

         case 2:
            STRCPY(szBuf,PNG_FILES);
            imageIndex++;
            break;

         case 3:
            STRCPY(szBuf,BCI_FILES);
            imageIndex++;
            break;
         
         case 4:
            STRCPY(szBuf,JPG_FILES);
            imageIndex++;
            break;

         case 5:
            STRCPY(szBuf,JPEG_FILES);
            imageIndex++;
            break;
         //Add a new case here for each additional type of image files supported
         //case 2:......

         default:
            *szBuf = 0;
         }
      }
      else  //Sound Files
      {
         switch(soundIndex)
         {
         case 0:
            if(pme->m_bMIDI) {
               STRCPY(szBuf,MIDI_FILES);
               soundIndex++;
            }
            else if(pme->m_bMP3) {
               STRCPY(szBuf,MIDI_FILES);
               soundIndex++;
            }
            else *szBuf = 0;

            break;

         case 1:
            if(pme->m_bMP3) {
               STRCPY(szBuf,MP3_FILES);
               soundIndex++;
            }
            else *szBuf = 0;
            break;

         //Add a new case here for each additional type of sound files supported
         //case 2: ......

         default:
            *szBuf = 0;
         }
      }

      if(!*szBuf) // All Files Processed
         break;

      IFILEMGR_EnumInit(pme->m_pIfm, szBuf, FALSE);
      while (IFILEMGR_EnumNext(pme->m_pIfm, &fi)) 
      {
         int nret = AppMgr_AddFile(pme,fi.szName);
         if(nret == ENOMEMORY)
            break;
         else 
            if(nret != AEE_SUCCESS)
               continue;

         //strip out the "shared\" part
         subStr = STRCHR(fi.szName,DIRECTORY_CHAR);
         if(!subStr)
            subStr = fi.szName;
         else
            subStr +=1;

         //Remove file extension
         ext = STRCHR(subStr,'.');
         if(ext)
            *ext = 0;

         STR_TO_WSTR(subStr, wszName, MAX_FILE_NAME);
         IMENUCTL_AddItem(pme->m_pMenu, 0,0,i, wszName, 0);
         i++;
      }
   }

   if (IMENUCTL_GetItemCount(pme->m_pMenu))
   {
      // Set Title
      IMENUCTL_SetTitle(pme->m_pMenu, AEE_APPMGRLS_RES_FILE, (uint16)(bImage ? IDC_CONFIGURE_IMAGE : IDC_CONFIGURE_SOUND), NULL);

      // Sort Menu
      IMENUCTL_Sort(pme->m_pMenu, MCS_NAME_ASCENDING);

      // Set Size
      IMENUCTL_SetRect(pme->m_pMenu, &pme->m_rc);

      // Set Menu Active
      IMENUCTL_SetActive(pme->m_pMenu, TRUE);
   }
   else
   {
      AECHAR szBuf[25];
      if(ISHELL_LoadResString(pme->a.m_pIShell,AEE_APPMGRLS_RES_FILE,IDS_CONFIGURE_NO_FILES,szBuf,sizeof(szBuf)))
         IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_NORMAL,szBuf, -1,0,0,0, IDF_ALIGN_CENTER);
   }

   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
}

//===========================================================================
//DESCRIPTION
//    Called when an image or sound file is selected in the file menu.
//    It modifies the corresponding data member in the AppMgrItem
//PARAMETERS
//    pme:  the app mgr
//    nSel: the selected menu item ID
//Return Value:
//    
//===========================================================================
static boolean AppMgr_FileSelected(AppMgr * pme, uint16 nSel)
{
   boolean bImage = (IMENUCTL_GetSel(pme->m_pSK) == IDC_CONFIGURE_IMAGE);

   if (nSel)
   {
      char* newFile = (&pme->m_pFileList[nSel-IDC_CONFIGURE_APP])->szFile;
      if (pme->m_pCurrentApp)
      {
         if(bImage) 
            STRCPY(pme->m_pCurrentApp->szIcon, newFile);
         else 
            STRCPY(pme->m_pCurrentApp->szSound, newFile);
      }
   }

   pme->m_nSelCache = (bImage ? IDC_CONFIGURE_IMAGE : IDC_CONFIGURE_SOUND);

   AppMgr_SetState(pme, ST_EDITSOUNDIMAGE);

   return TRUE;
}

//===========================================================================
//DESCRIPTION
//    Plays sound if it can set sound info. Otherwise sets a self timer.
//
//PARAMETERS
//    pme:  the app mgr
//
//Return Value:
//    none
//    
//===========================================================================
void AppMgr_PlayRetry(void *po)
{
   AppMgr *pme = (AppMgr*)po;

   if (AEE_SUCCESS == ISOUNDPLAYER_SetInfo(pme->m_pISp, &pme->m_spInfo))
      ISOUNDPLAYER_Play(pme->m_pISp);
   else
      ISHELL_SetTimer(pme->a.m_pIShell,200,AppMgr_PlayRetry,pme);
}

//===========================================================================
//DESCRIPTION
//    Stops sound and animation
//
//PARAMETERS
//    pme:  the app mgr
//
//Return Value:
//    none
//    
//===========================================================================
void AppMgr_StopSoundAnimation(AppMgr* pme)
{
   FreeObj((void**)&pme->m_pAnimatedImage);
   ISHELL_CancelTimer(pme->a.m_pIShell, AppMgr_PlayRetry,pme);
   if (pme->m_pISp)
      ISOUNDPLAYER_Stop(pme->m_pISp);
}

//===========================================================================
//DESCRIPTION
//    Tests if given image fits in specified dimension
//
//PARAMETERS
//    pImage      : the IImage pointer
//    pIShell     : Shell Pointer
//    pszResFile  : Resource File Name
//    wImage      : Image ID
//    w           : width threshold
//    h           : height threshold
//
//Return Value:
//    none
//    
//===========================================================================
boolean ImageFits(IImage * pImage, IShell* pIShell, const char* pszResFile, uint16 wImage, int w, int h)
{
   AEEImageInfo ii;
   boolean bLoaded = FALSE;

   if (!pImage) {
      if (! pIShell)
         return FALSE;

      if ((pImage = ISHELL_LoadResImage(pIShell, pszResFile, wImage)) != NULL)
         bLoaded = TRUE;
   }

   if (!pImage)
      return FALSE;
   else
   {
      IIMAGE_GetInfo(pImage, &ii);
      if (bLoaded)
         IIMAGE_Release(pImage);
      return ((ii.cx <= w) && (ii.cy <= h));
   }
}

//===========================================================================
//DESCRIPTION
//    Sets special properties, style and color for large icon view menus
//
//PARAMETERS
//    pme   : the AppMgr
//
//Return Value:
//    none
//    
//===========================================================================
void AppMgr_LargeIconViewSettings(AppMgr* pme)
{
   AEEItemStyle normal, select;
   AEEMenuColors  col;
   
   // Menu Properties - Text On Top and No Arrows
   IMENUCTL_SetProperties(pme->m_pMainMenu, MP_ICON_SINGLE_FRAME|MP_NO_ARROWS|MP_ICON_ANIMATED);

   // Main Menu Softkey - Properties
   IMENUCTL_SetProperties(pme->m_pMainMenuSK, MP_MAXSOFTKEYITEMS);
   IMENUCTL_EnableCommand(pme->m_pMainMenuSK, FALSE);

   // Remove offset to make more room for title
   ISHELL_GetItemStyle(pme->a.m_pIShell,AEE_IT_ICONVIEW,&normal,&select);
   normal.yOffset = select.yOffset = 0;
   IMENUCTL_SetStyle(pme->m_pMainMenu,&normal,&select);

   col.wMask = MENU_COLOR_MASK | MC_FRAME;

   if(pme->m_nColorDepth > 2) {

      col.cBack     = COLOR_ICON_BACKGROUND;
      col.cSelBack  = COLOR_ICON_SELECT_BACKGROUND;
      col.cSelText  = COLOR_ICON_SELECT_TEXT;
      col.cFrame    = COLOR_ICON_BACKGROUND;
      IMENUCTL_SetColors(pme->m_pMainMenu,&col);

      col.cBack     = MAKE_RGB(23,0,94);
      col.cSelBack  = COLOR_SK_SELECT_BACKGROUND;
      col.cSelText  = COLOR_SK_SELECT_TEXT;
      col.cFrame    = COLOR_SK_SELECT_BACKGROUND;
      IMENUCTL_SetColors(pme->m_pMainMenuSK,&col);
   }
   else
   {
      // Same colors for menu/softkey/icon view
      col.cBack =             MENU_BACKGROUND;
      col.cSelBack =          MENU_SELECT_BACKGROUND;
      col.cSelText =          MENU_SELECT_TEXT;
      col.cFrame =            MENU_BACKGROUND;
      IMENUCTL_SetColors(pme->m_pMainMenu,&col);
      IMENUCTL_SetColors(pme->m_pMainMenuSK,&col);
   }
}

//===========================================================================
// 
// DESCRIPTION
//    Gets item ID of the specified AppMgrItem.
//
// PARAMETERS
//    pme: the app mgr
//    pmi: AppMgrItem
//
// Return Value:
//    None
//
//===========================================================================
DLITEMID AppMgr_GetItemID(AppMgr * pme, AppMgrItem * pmi)
{
   uint32* pIDs;
   uint32* pdw;
   uint32 dw;
   
   // Guard against NULL pmi
   if (!pmi)
      return (0);

   // Return if we already know the item ID. Return 0 for FAKE_MODULE_ID so that we don't
   // have to check for FAKE_MODULE_ID at other places
   if (pmi->dwItemID)
      return ((pmi->dwItemID < FAKE_MODULE_ID) ? pmi->dwItemID : 0);

   // Guard against NULL IDownload
   if (!pme->m_pDownload)
      return (0);

   // We don't know the item ID. Start going through item ID list to
   // find app that matches class ID
   // This action will also give us DLITEMID for protected pre-installed
   // applications
   pIDs = IDOWNLOAD_GetAppIDListEx(pme->m_pDownload);
   if(pIDs)
   {
      for(pdw = pIDs; ((dw = *pdw) != 0) && !pmi->dwItemID; pdw++)
      {
         AppModInfo* pai;

         //Its not a valid app (with valid license), no point checking for the license
         if (dw >= FAKE_MODULE_ID)
            continue;

         pai = IDOWNLOAD_GetModInfo(pme->m_pDownload, dw);

         if (pai)
         {
            uint16 index;

            for (index = 0; index < pai->nApps; index++)
            {
               if (pai->apps[index].cls == pmi->cls)
               {
                  pmi->dwItemID = dw;  // We found the match
                  break;
               }
               else
               {
                  // Doesn't match class ID of specified AppMgrItem. Find AppMgrItem
                  // that matches this class ID and update dwItemID of that. This may
                  // not be needed as found AppMgrItem may already have valid item ID.
                  // Still do this to make best use of expensive IDOWNLOAD_GetModInfo().
                  AppMgrItem * pi = NULL;

                  if (AppMgr_FindItemCategory(pme, pai->apps[index].cls, &pi))
                     pi->dwItemID = dw;
               }
            }
         }
      }
   }

   // Didn't find any match. May be this is a test app put through serial cable.
   // Assign it FAKE_MODULE_ID so that we don't do this exercise again for this
   // app
   if (!pmi->dwItemID)
      pmi->dwItemID = FAKE_MODULE_ID;

   return ((pmi->dwItemID < FAKE_MODULE_ID) ? pmi->dwItemID : 0);
}
