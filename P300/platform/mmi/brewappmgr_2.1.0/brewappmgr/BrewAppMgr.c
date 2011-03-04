/*=========================================================================
FILE:  BrewAppMgr.c

SERVICES:  BREW Application Manager Application
  
GENERAL DESCRIPTION:
    This is implementation of AppMgr/MShop as described in "BREW Core Carrier 
    Applications - Process, Features and User Interface Specifications".
    
PUBLIC CLASSES AND STATIC FUNCTIONS:
      List the class(es) and static functions included in this file
      
INITIALIZATION & SEQUENCING REQUIREMENTS:
          
              Copyright ?2000-2008 QUALCOMM Incorporated.
              All Rights Reserved.
              QUALCOMM Proprietary/GTDR
===========================================================================*/
#include "BrewAppMgrSettings.h"
#include "BrewAppMgr.h"
#include "AEEConfig.h"
#include "AEEDeviceNotifier.h"
#include "AEECLSID_FLIP.bid"
#if defined(FEATURE_FLEXI_STATIC_BREW_APP) || defined(FEATURE_SMARTFREN_STATIC_BREW_APP)
#include "OEMCFGI.H"
#endif
#ifdef USE_IDL2
#include "AEEDownload2.bid"
#endif

#ifdef FEATURE_TRUSTED_TIME
#include "BREWVersion.h"
#include "AEESysClock.h"
#endif // FEATURE_TRUSTED_TIME


#ifdef FEATURE_TOUCHSCREEN_SUPPORT
#include "AEEPointerHelpers.h"
#endif // FEATURE_TOUCHSCREEN_SUPPORT


#ifdef USE_TT_FONTS
#include "AEECLSID_TYPEFACE.BID"
#include "AEECLSID_QCSANSTTF.BID"
#include "AEECLSID_QCSANSBOLDTTF.BID"
#endif // USE_TT_FONTS

#ifdef FEATURE_HAPTIC_SUPPORT
#include "AEESound.bid"
#endif

#ifdef FEATURE_SCREEN_ROTATE
#include "AEEBitmapRot.h"
#include "AEEDeviceItems.h"
#endif //FEATURE_SCREEN_ROTATE

#ifdef FEATURE_TRUSTED_TIME

#if MIN_BREW_BUILD(4,0,0,0)
   // Get definition for secure clock class ID
   #include "AEESecureSysClock.bid"
   #define SECSYSCLOCK_CLSID AEECLSID_SecureSysClock
#else
   // BAM built against pre-4.0 BREW has no need to use a secure clock
   // regardless of the run time BREW version.
#endif

// Start paying attention to the secure clock for time based license validation
// if the run time AEE version is 4.x or higher.
//
#define AEEVERSION_USE_TRUSTED_TIME 0x04000000

#endif // FEATURE_TRUSTED_TIME


#ifndef CFGI_DOWNLOAD_FS_INFOEX
#define   CFGI_DOWNLOAD_FS_INFOEX            112   // 64 bit version of CFGI_DOWNLOAD_FS_INFO for efs > 4GB
#endif //CFGI_DOWNLOAD_FS_INFOEX

#define FARF_BAM     1
#define FARF(x, p)

//#define BREWAPPMGRLS_RES_LOC AEE_RES_LANGDIR BREWAPPMGRLS_RES_LOC
//#define BREWAPPMGRLS_RES_LOC "brewappmgrls"AEE_RES_LANGSUF".bar"
#define BREWAPPMGRLS_RES_LOC BREWAPPMGRLS_RES_FILE


#ifdef FEATURE_TOUCHSCREEN_SUPPORT
static void AppMgr_CreateSoftmenu(AppMgr * pme, uint16* resID, int16 nCount);
static void AppMgr_SetMenuRect(AppMgr * pme);
static void AppMgr_AddSoftKey(AppMgr * pme);
#endif 

static boolean gbENDKey_RecievedInSuspend = FALSE;
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

static const AEEAppInfo    gaiAppMgr[] =
{
   {AEECLSID_BREWAPPMGR_BID,BREWAPPMGRLS_RES_LOC,IDS_APPLICATION_MANAGER,0,0,0,0,0},
   {AEECLSID_MOBILESHOP_BID,BREWAPPMGRLS_RES_LOC,IDS_MOBILESHOP,0,0,0,0,AFLAG_HIDDEN|AFLAG_POPUP}
};



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
   int nErr = AEE_SUCCESS;

   *m_pApps = (AEEAppInfo *)&gaiAppMgr;
   *pnApps = sizeof(gaiAppMgr) / sizeof(AEEAppInfo);

   // Register wild-carded handler so that we will also process incoming URLs.
   nErr = ISHELL_RegisterHandler(ps, HTYPE_BROWSE, CMSHOP_CMD_WILDCARD, AEECLSID_MOBILESHOP_BID);
   FARF(BAM, ("Registered Hanlder for %s, with return code of %d", CMSHOP_CMD_WILDCARD, nErr));
   nErr = ISHELL_RegisterHandler(ps, HTYPE_BROWSE, APPMGR_CMD_WILDCARD, AEECLSID_BREWAPPMGR_BID);
   FARF(BAM, ("Registered Hanlder for %s, with return code of %d", APPMGR_CMD_WILDCARD, nErr));

#ifdef NMASK_SHELL_START_STATUS
   ISHELL_RegisterNotify(ps, AEECLSID_BREWAPPMGR_BID, AEECLSID_SHELL, NMASK_SHELL_START_STATUS);
#endif // NMASK_SHELL_START_STATUS

#ifdef NMASK_SHELL_SYS_ERROR
   ISHELL_RegisterNotify(ps, AEECLSID_BREWAPPMGR_BID, AEECLSID_SHELL, NMASK_SHELL_SYS_ERROR);
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
   if (ClsId == AEECLSID_BREWAPPMGR_BID)
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

   if (!AEEApplet_New(sizeof(AppMgr), ClsId, pIShell, po,
      (IApplet**)ppObj, (AEEHANDLER)AppMgr_HandleEvent,
      (PFNFREEAPPDATA)AppMgr_Free))
   {
      return ENOMEMORY;
   }
   
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

   IFlip       *pIFlip = NULL;
   AEECLSID    *pClsList = NULL;
   AEEFlipInfo *pFlipInfo = NULL;
   int32       dwNumDisplays = 0;
   int16       wNumFlips  = 0;
   int16       i = 0;
   AEECLSID newDisplayId = AEECLSID_DISPLAY1;

   pme->m_pIDisplay = NULL;

   FARF(BAM, ("Init()"));


#ifdef AEE_SIMULATOR
   // It seems that sometimes in the simulator the GetModInfo is not called so our SMS
   // handlers are not registered.  We do not care if there is an error in the return from
   // these calls, as it means that they were called twice.
   {
      int nErr = AEE_SUCCESS;

      nErr = ISHELL_RegisterHandler(pIShell, HTYPE_BROWSE, CMSHOP_CMD_WILDCARD, AEECLSID_MOBILESHOP_BID);
      FARF(BAM, ("Registered Hanlder for %s, with return code of %d", CMSHOP_CMD_WILDCARD, nErr));
      nErr = ISHELL_RegisterHandler(pIShell, HTYPE_BROWSE, APPMGR_CMD_WILDCARD, AEECLSID_BREWAPPMGR_BID);
      FARF(BAM, ("Registered Hanlder for %s, with return code of %d", APPMGR_CMD_WILDCARD, nErr));
   }
#endif

   // Initialize State
   pme->m_wState = NO_ENTRY;
   pme->m_wEditSoundImageSubState = ST_EDITSOUNDIMAGEBROWSENONE;

   // Explicitly initialize all our pointers to NULL and vars to 0
   pme->m_bConfigMenu = FALSE;
   pme->m_bWarningDisplayed = FALSE;
   pme->m_bMIDI = FALSE;
   pme->m_bMP3 = FALSE;
   pme->m_bResume = FALSE;
   pme->m_bSuspended = FALSE;
   pme->m_bShowBusy = FALSE;
   pme->m_bFreeCurrentApp = FALSE;
   pme->m_bValidCache = FALSE;
   pme->m_bHandledLaunchError = FALSE;
   pme->m_bNormalFontSaved = FALSE;
   pme->m_bBoldFontSaved = FALSE;

   pme->m_bLaunchClsOnStart = FALSE;
   pme->m_bHasStarted = FALSE;

   pme->m_pAnimatedImage = NULL;
   pme->m_pCurrentApp = NULL;
   pme->m_pDownload = NULL;
#ifdef USE_IDL2
   pme->m_pDownload2 = NULL;
#endif
   pme->m_pFileList = NULL;
   pme->m_pIfm = NULL;
#ifdef USE_TT_FONTS
   pme->m_pSaveNormalFont = NULL;
   pme->m_pSaveBoldFont = NULL;
#endif // USE_TT_FONTS

#ifdef FEATURE_HAPTIC_SUPPORT
   pme->m_pHapticResponse = NULL;
   // ToneData would be read from oemcfg.txt, To start with, initialize with 0.
   pme->m_ToneData.eTone = 0;     
   pme->m_ToneData.wDuration = 0;
#endif //FEATURE_HAPTIC_SUPPORT

   pme->m_wTouchDisplays = 0;       // Default value, no displays are touch
   pme->m_wEnabledDisplays = 0x01;  // Default value, only run on display1
   pme->m_wNotEnoughKeys = 0;       // Default value, all displays have enough keys to run on.
   pme->m_bDispTouchEnabled = FALSE; 
   pme->m_pMainMenu = NULL;
   pme->m_pMainMenuSK = NULL;
   pme->m_pMenu = NULL;
   pme->m_pMShopLogo = NULL;
   pme->m_pSK = NULL;
   pme->m_pStatic = NULL;

   pme->m_pArgs = NULL;
   pme->m_pAutoStart = NULL;
   MEMSET(&pme->m_cba, '\0', sizeof(pme->m_cba));

   pme->m_bHasDPad = TRUE;          // Default to having a DPad

#ifdef FEATURE_LESS_THAN_6_KEYS
   pme->m_bUseAltKeys = FALSE;
   pme->m_wAltKeyUp = AVK_UP;
   pme->m_wAltKeyDown = AVK_DOWN;
   pme->m_wAltKeySelect = AVK_SELECT;
   pme->m_wAltKeyClr = AVK_CLR;
   for (i = 0; i < 4; ++i)
   {
      pme->m_wAltKeyUpList[i] = AVK_UP;
      pme->m_wAltKeyDownList[i] = AVK_DOWN;
      pme->m_wAltKeySelectList[i] = AVK_SELECT;
      pme->m_wAltKeyClrList[i] = AVK_CLR;
   }
#endif

   pme->m_bRedrawingUI = FALSE;
   pme->m_bExitAfterSANotification = FALSE;
   pme->m_wEditSoundImageSubState = ST_EDITSOUNDIMAGEBROWSENONE;
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   pme->cbDisplayEnabled = NULL;
   MEMSET(&pme->m_cbUpdateUI, '\0', sizeof(pme->m_cbUpdateUI));
   pme->m_bFlipOpenWhenSuspended = FALSE;
   pme->m_bPromptIsDisplayed = FALSE;
   pme->m_pMainMenuResFile = NULL;
   pme->m_pMainMenuResID = 0;
   pme->m_pMainMenuText = NULL;
   pme->m_pMenuResFile = NULL;
   pme->m_pMenuResID = 0;
   pme->m_pMenuText = NULL;
   pme->m_bMainMenuSKEnableCommandCalled = FALSE;
   pme->m_bMainMenuSKEnableCommandValue = FALSE;
   pme->m_bSKEnableCommandCalled = FALSE;
   pme->m_bSKEnableCommandValue = FALSE;
   pme->m_pStaticTitle = NULL;
   pme->m_pStaticText = NULL;
   pme->m_bStaticSizeToFitCalled = FALSE;
   pme->m_pPromptTitle = NULL;
   pme->m_pPromptText = NULL;
   pme->m_pPromptButtons = NULL;
#endif

#ifdef FEATURE_CANCELONCLOSE
   // When user flips or slides the phone closed, cancel the current operation and exit
   pme->m_bCancelOnClose = TRUE;
#endif

#ifdef FEATURE_HAS_MULTIPLE_DISPLAYS
   // Does the phone have multiple displays?
   pme->m_bHasMultipleDisplays = FALSE;
#endif

#ifdef FEATURE_INTERACT_WITH_SA_APP
   // The URL to post to notify the ShortcutApp that the applist file has been updated
   pme->m_szSMSAppUrl = NULL;
#endif
   pme->m_bMShopChangedNumApps = FALSE;
   pme->m_clsMShopLaunched = 0;


   // Load the IFileMgr so that LoadOEMCfg can use it
   if (CreateInstance(pIShell, AEECLSID_FILEMGR,
      (void**)&pme->m_pIfm) != AEE_SUCCESS)
   {
      return FALSE;
   }
#ifdef FEATURE_OEMCFG
   AppMgr_LoadOEMCfg(pme);
#endif

   CreateInstance(pme->a.m_pIShell, AEECLSID_FLIP, 
                            (void **)&pIFlip);

   if (NULL != pIFlip)
   {

      // This code gets the list of all the displays that are accessable, given the flip position
      // We normally expect this to return a list of one class ID that is the display that we should
      // draw to, but it could be more if we are allowed to draw to more than one at a time.
      // This result of this chunk is that we will get a list of class ids for the displays that
      // we can access at this point
      if ((SUCCESS == IFLIP_GetDisplayList(pIFlip, NULL, &dwNumDisplays)) && (dwNumDisplays > 0))
      {

         pClsList = (AEECLSID*) MALLOC(sizeof(AEECLSID) * dwNumDisplays);

         if (NULL != pClsList)
         {

            if (SUCCESS != IFLIP_GetDisplayList(pIFlip, (void*)pClsList, &dwNumDisplays))
            {
               FARF(BAM, ("Unable to get the display IDs for this flip"));
            }
            else
            {
               newDisplayId = pClsList[0];
               FARF(BAM, ("IFlip told us we are using %d, and that there are %d displays", newDisplayId, dwNumDisplays));
               // We need to do two things.  
               // 1) make sure that we are drawing to the correct display
               // Display1 is currently "active" according to BREW, but we may need to set another display as active
               // so that we actualy display on the proper display.
               // 2) Figure out if there are any key mappings that need to happen.

               AppMgr_SetKeyList(pme, pIFlip, newDisplayId);

               if (SUCCESS == CreateInstance(pIShell, newDisplayId, (void**)&(pme->m_pIDisplay)))
               {
                  IDISPLAY_MakeDefault(pme->m_pIDisplay);

                  if (!IDISPLAY_IsEnabled(pme->m_pIDisplay))
                  {
                     // If our display is not yet enabled, we need to wait for it to be enabled before
                     // trying to draw on it.
                     FARF(BAM, ("Our display is not enabled, we are enabling it"));
                     pme->cbDisplayEnabled = (AEECallback*) MALLOC(sizeof(AEECallback));
                     MEMSET(pme->cbDisplayEnabled, 0, sizeof(AEECallback));
                     CALLBACK_Init(pme->cbDisplayEnabled, AppMgr_EnableDispCallback, pme);
                     IDISPLAY_NotifyEnable(pme->m_pIDisplay, pme->cbDisplayEnabled);
                  }
               }
               else
               {
                  FARF(BAM, ("Unable to create display that we should be using, exiting."));
                  return FALSE;
               }
            }
         }


         // This code Gets the list of flips (not displays) and the status/position of the axis' for that flip
         // note, that this is not displays but flips.
         if ((SUCCESS == IFLIP_GetFlipIDList(pIFlip, NULL, &wNumFlips )) || (0 == wNumFlips ))
         {

            pFlipInfo = (AEECLSID*) MALLOC(sizeof(AEEFlipInfo) * wNumFlips );

            if (NULL != pFlipInfo)
            {

               if (SUCCESS == IFLIP_GetFlipIDList(pIFlip, pFlipInfo, &wNumFlips ))
               {
                  for ( i = 0; i < wNumFlips ; ++i)
                  {
                     FARF(BAM, ("Display index %d has ID %d, x = %d, y = %d, z = %d", i, DEVNOTIFY_GET_ID(pFlipInfo[i]),
                                 DEVNOTIFY_GET_X(pFlipInfo[i]), DEVNOTIFY_GET_Y(pFlipInfo[i]), DEVNOTIFY_GET_Z(pFlipInfo[i])));
                  }
               }
            }
         }
         else
         {
            FARF(BAM, ("There are no Flips for this device"));
            
         }
      }
      else
      {
         FARF(BAM, ("There are no displays available for this flip"));
      }

      FreePtr((void**)&pFlipInfo);
      FreePtr((void**)&pClsList);
      FreeObj((void**)& pIFlip);
   }

   // If we don't have a display yet, all we can do is use the one they passed us.
   // This should only happen if IFLIP is not fully implemented, like might happen
   // on a CandyBar phone.
   if (NULL == pme->m_pIDisplay)
   {
      newDisplayId = AEECLSID_DISPLAY1;
      pme->m_pIDisplay = pme->a.m_pIDisplay;
      (void)IDISPLAY_AddRef(pme->m_pIDisplay);
   }
   
   // Get Prefs

   MEMSET(&pme->m_cfg, '0', sizeof (pme->m_cfg));

   if (ISHELL_GetPrefs(pIShell, AEECLSID_BREWAPPMGR_BID, APPMGR_CFG_VER,
      &pme->m_cfg, sizeof(pme->m_cfg)) != SUCCESS)
   {
      // Try filling in Ver 1 info
      if (ISHELL_GetPrefs(pIShell, AEECLSID_BREWAPPMGR_BID, APPMGR_CFG_VER_1,
      &pme->m_cfg, sizeof(pme->m_cfg)) != SUCCESS)
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
   }

#ifndef ENABLE_LARGE_ICON_VIEW
   // Force small icon view to true if large icon mode is disabled
   pme->m_cfg.bSmallIconView = TRUE;
#endif // ENABLE_LARGE_ICON_VIEW

   // Creating classes which we would need later

    // No saved fonts yet.
   pme->m_bNormalFontSaved = FALSE;
   pme->m_bBoldFontSaved = FALSE;

#ifdef USE_TT_FONTS
   // Install the TrueType fonts here, before making the call to
   // GetFontMetrics() and before the menu controls are instantiated.
   AppMgr_InstallTTFonts(pme);
#endif // USE_TT_FONTS

#ifdef FEATURE_HAPTIC_SUPPORT
   if(ISHELL_CreateInstance(pIShell, AEECLSID_Sound, (void**)&pme->m_pHapticResponse) != AEE_SUCCESS)
   {
      return FALSE;
   }
#endif


#ifdef FEATURE_TOUCHSCREEN_SUPPORT
   // newDisplayId is the current active display, check whether it is touch enabled or not
    if(newDisplayId )
   {
      pme->m_bDispTouchEnabled = IsDisplayTouchEnabled(newDisplayId,pme->m_wTouchDisplays);
      FARF(BAM, ("newDisplayId = pme->m_bDispTouchEnabled = %d   &   %d", newDisplayId, pme->m_bDispTouchEnabled));

   }
#endif

   if(pme->m_bDispTouchEnabled)
   {
      IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_BACKGROUND, TOUCHSCREEN_BACKGROUND);
   }



   // Creating controls we need
   //if ((CreateInstance(pIShell, GetMenuCtlClassID(pme->m_bDispTouchEnabled),
   //      (void**)&pme->m_pMenu) != AEE_SUCCESS) ||
   //   (CreateInstance(pIShell, GetSoftKeyClassID(pme->m_bDispTouchEnabled),
   //      (void**)&pme->m_pSK) != AEE_SUCCESS)||
   //   (CreateInstance(pIShell,
   //      pme->m_cfg.bListView ? GetMenuCtlClassID(pme->m_bDispTouchEnabled) : GetIconViewClassID(pme->m_bDispTouchEnabled),
   //      (void**)&pme->m_pMainMenu) != AEE_SUCCESS))
   //{
   //    return FALSE;
   //}


   if ((CreateInstance(pIShell, GetMenuCtlClassID(pme->m_bDispTouchEnabled),
         (void**)&pme->m_pMenu) != AEE_SUCCESS) ||
      (CreateInstance(pIShell, GetSoftKeyClassID(pme->m_bDispTouchEnabled),
         (void**)&pme->m_pSK) != AEE_SUCCESS)||
      (CreateInstance(pIShell,
         pme->m_cfg.bListView ? GetMenuCtlClassID(pme->m_bDispTouchEnabled) : GetIconViewClassID(pme->m_bDispTouchEnabled),
         (void**)&pme->m_pMainMenu) != AEE_SUCCESS))
   {
     FreeObj((void**)&pme->m_pMenu);
     FreeObj((void**)&pme->m_pSK);
     FreeObj((void**)&pme->m_pMainMenu);
     return FALSE;
   }

#ifdef ENABLE_LARGE_ICON_VIEW
   // This code should never get hit if ENABLE_ICON_VIEW is not defined
   // anyway, but we'll make it explicitly unreachable here to protect
   // m_pMainMenuSK from being initialized.
   if (!pme->m_cfg.bListView && !pme->m_cfg.bSmallIconView)
   {
      if (CreateInstance(pIShell, GetSoftKeyClassID(pme->m_bDispTouchEnabled),
         (void**)&pme->m_pMainMenuSK) != AEE_SUCCESS)
      {
         return FALSE;
      }
   }
#endif // ENABLE_LARGE_ICON_VIEW

   // Device Information
   ISHELL_GetDeviceInfo(pIShell, &di);
   FARF(BAM, ("di.bAltDisplay = %d, di.bFlip = %d", di.bAltDisplay, di.bFlip));
   pme->m_nColorDepth = di.nColorDepth;
   SETAEERECT(&pme->m_rc, 0, 0, di.cxScreen, di.cyScreen);

#ifdef FEATURE_SCREEN_ROTATE
   // Our screen is landscape if cx is > cy (this implies that cx = cy is a 
   // portrait screen
   if (di.cxScreen > di.cyScreen)
   {
      pme->m_orentDisplayDefault = AEE_SO_LANDSCAPE;
   }
   else
   {
      pme->m_orentDisplayDefault = AEE_SO_PORTRAIT;
   }
#endif //FEATURE_SCREEN_ROTATE


#ifdef FEATURE_HAS_MULTIPLE_DISPLAYS
#ifdef AEE_SIMULATOR
   // If we are in the simulator, always default to TRUE as we have no way of knowing
   pme->m_bHasMultipleDisplays = TRUE;
#else
   pme->m_bHasMultipleDisplays = di.bAltDisplay;
#endif // AEE_SIMULATOR
#endif // FEATURE_HAS_MULTIPLE_DISPLAYS

   // Set Menu Selection History Index
   pme->m_nIndex = -1;

   // Get MShop Configuration
   if (SUCCESS != ISHELL_GetPrefs(pme->a.m_pIShell, AEECLSID_MOBILESHOP_BID,
      MSHOP_CFG_VER, &pme->m_MShopCfg, sizeof(pme->m_MShopCfg)))
   {
      pme->m_MShopCfg.bSearch = OEM_ADS_SEARCH_APPS;
      pme->m_MShopCfg.bUpgrade = OEM_ADS_UPGRADE_CHECK;
   }

   // Set Menu Style
   SetMenuStyleAndColor(pme->m_pMenu, GetMenuCtlClassID(pme->m_bDispTouchEnabled), pme->m_nColorDepth, pme->m_bDispTouchEnabled);
   
   // Set Softkey Style
   SetMenuStyleAndColor(pme->m_pSK, GetSoftKeyClassID(pme->m_bDispTouchEnabled), pme->m_nColorDepth, pme->m_bDispTouchEnabled);

   if (pme->m_pMainMenuSK)
      // Settings specific to large icon view
      AppMgr_LargeIconViewSettings(pme, pme->m_pMainMenu, pme->m_pMainMenuSK);
   else
      // Set Main Menu Style
      SetMenuStyleAndColor(pme->m_pMainMenu, pme->m_cfg.bListView ? GetMenuCtlClassID(pme->m_bDispTouchEnabled) : GetIconViewClassID(pme->m_bDispTouchEnabled), pme->m_nColorDepth, pme->m_bDispTouchEnabled);

   // Create State Machine by specifying state (index in state machine array) and
   // members of state handler struct
      
   SET_STATE(pme,ST_MAINMENU,          AppMgr_MainMenu,LAST_STATE,TRUE);
   SET_STATE(pme,ST_FAVORITES,         AppMgr_Launcher,LAST_STATE,TRUE);
   SET_STATE(pme,ST_WORK,              AppMgr_Launcher,LAST_STATE,TRUE);
   SET_STATE(pme,ST_FUN,               AppMgr_Launcher,LAST_STATE,TRUE);
   SET_STATE(pme,ST_OPTIONS,           AppMgr_Options,ST_MAINMENU,TRUE);
   SET_STATE(pme,ST_HELP,              AppMgr_Help,LAST_STATE,TRUE);
#ifdef MSHOP_STATES
   SET_STATE(pme,ST_MSHOP,             AppMgr_MShop,ST_MAINMENU,TRUE);
   SET_STATE(pme,ST_MSHOPGUIDE,        AppMgr_MShopGuide,LAST_STATE,TRUE);
#endif
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
   SET_STATE(pme,ST_UPGRADEAUTOPROMPT, AppMgr_UpgradeAutoPrompt,LAST_STATE,TRUE);
   SET_STATE(pme,ST_AIRTIMEWARNINGS,   AppMgr_AirtimeWarnings,ST_OPTIONS,TRUE);
   SET_STATE(pme,ST_AUTOSTART,         AppMgr_AutoStart,NO_ENTRY,TRUE);

   // Initialize Font
   pme->m_cyFont = IDISPLAY_GetFontMetrics(pme->m_pIDisplay,
      AEE_FONT_NORMAL, NULL, NULL);   

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
         CreateInstance(pIShell, clsPlayer, (void **)&(pme->m_pISp));

      if(pme->m_pISp) 
         ISOUNDPLAYER_RegisterNotify(pme->m_pISp, NULL, NULL);

      // Set ISoundPlayer audio source info. 
      // eInput and dwSize do not change. Set pData to the audio file name.
      pme->m_spInfo.eInput = SDT_FILE;
   }
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

#ifdef FEATURE_UPGRADE_AUTO_PROMPT
   // Release the UpgradeMgr
   if (NULL != pme->m_pUm)
   {
      IUM_Release(pme->m_pUm);
   }
#endif // FEATURE_UPGRADE_AUTO_PROMPT

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
   int ret = SUCCESS;
   // Set Preferences
   ret = ISHELL_SetPrefs(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID, APPMGR_CFG_VER,
      &pme->m_cfg, sizeof(pme->m_cfg));

#ifdef USE_TT_FONTS
   // Restore the original fonts
   AppMgr_RestoreFonts(pme);
#endif // USE_TT_FONTS

   // Free Apps Cache
   if (pme->m_bValidCache)
      AppMgr_FreeAppsCache(pme);

   // Free IDownload
   FreeObj((void**)&pme->m_pDownload);
#ifdef USE_IDL2
   FreeObj((void**)&pme->m_pDownload2);
#endif

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

#ifdef FEATURE_INTERACT_WITH_SA_APP
   FreePtr((void**)&(pme->m_szSMSAppUrl));
#endif


#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   if (pme->cbDisplayEnabled)
   {
      CALLBACK_Cancel(pme->cbDisplayEnabled);
      FreePtr((void**)&pme->cbDisplayEnabled);
   }
   if (pme->m_cbUpdateUI.pfnCancel)
   {
      pme->m_cbUpdateUI.pfnCancel(&pme->m_cbUpdateUI);
      pme->m_cbUpdateUI.pfnCancel = NULL;
   }
   // Free control stored control values
   FreePtr((void**)&(pme->m_pMainMenuResFile));
   FreePtr((void**)&(pme->m_pMainMenuText));
   FreePtr((void**)&(pme->m_pMenuResFile));
   FreePtr((void**)&(pme->m_pMenuText));
   FreePtr((void**)&(pme->m_pStaticTitle));
   FreePtr((void**)&(pme->m_pStaticText));
   FreePtr((void**)&(pme->m_pPromptTitle));
   FreePtr((void**)&(pme->m_pPromptText));
   FreePtr((void**)&(pme->m_pPromptButtons));
#endif

   // Free File List
   FreePtr((void**)&pme->m_pFileList);

   // Close Image Cache 
   FreeObj((void**)&pme->m_pImageCacheFile);

   if (NULL != pme->m_pIDisplay)
   {
      IDISPLAY_Release(pme->m_pIDisplay);
      pme->m_pIDisplay = NULL;
   }

#ifdef FEATURE_HAPTIC_SUPPORT
   if (pme->m_pHapticResponse != NULL)
   {
      ISound_Release(pme->m_pHapticResponse);
      pme->m_pHapticResponse = NULL;
   }
#endif

   // Any args that were passed
   FreePtr((void**)&pme->m_pArgs);
   gbENDKey_RecievedInSuspend = FALSE;
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
  boolean bRet = FALSE;

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
  boolean bSKUpdate = FALSE;
  boolean bSKDoNotUpdate = FALSE;
  int16 lastState;
#endif //#ifdef FEATURE_TOUCHSCREEN_SUPPORT

  if(NULL == pme)
  {
    return bRet;
  }

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
  lastState = pme->m_wState;
#endif //#ifdef FEATURE_TOUCHSCREEN_SUPPORT

   // IF THINGS START GOING TERRIBLY WRONG WHEN PRESSING KEYS THIS IS THE FIRST
   // PLACE TO LOOK.  WE ARE OVERRIDING THE ACTUAL KEY PRESSED WITH THE ONE
   // WE WANT THAT KEY PRESS TO ACT LIKE
#ifdef FEATURE_LESS_THAN_6_KEYS
   // We we got a key press and we need to use alternate keys
   // change the key code to be the one that it maps to
   // for this display
   if ((EVT_KEY == eCode) && (pme->m_bUseAltKeys))
   {
      if (wParam == pme->m_wAltKeyUp)
      {
         wParam = AVK_UP;
      }
      else if (wParam == pme->m_wAltKeyDown)
      {
         wParam = AVK_DOWN;
      }
      else if (wParam == pme->m_wAltKeySelect)
      {
         wParam = AVK_SELECT;
      }
      else if (wParam == pme->m_wAltKeyClr)
      {
         wParam = AVK_CLR;
      }
   }
#endif //FEATURE_LESS_THAN_6_KEYS

   switch (eCode) 
   { 

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
      case EVT_POINTER_UP:
      case EVT_POINTER_DOWN:
      case EVT_POINTER_MOVE:
      {
         int16 wXp = (int16)AEE_POINTER_GET_X((const char *)dwParam);
         int16 wYp = (int16)AEE_POINTER_GET_Y((const char *)dwParam);


         // If static text control exists give it first shot at the event
         if (pme->m_pStatic)
         {
            if (IsInStaticCtl(wXp, wYp, pme->m_pStatic))
            {
               bRet = ISTATIC_HandleEvent(pme->m_pStatic, eCode, wParam, dwParam);

#ifdef FEATURE_HAPTIC_SUPPORT
               if (bRet && (EVT_POINTER_DOWN == eCode) && ISTATIC_IsScrollable(pme->m_pStatic))
               {
                  PlyHapticResponse(pme->m_pHapticResponse,pme->m_ToneData);
               }
#endif //FEATURE_HAPTIC_SUPPORT

               return bRet;
            }

            if (pme->m_bPromptIsDisplayed)
            {
               // We have some special work to do to make sure that we don't
               // process any clicks behind a prompt.  We know that only one 
               // menu can be usable, so figure out whch one to pass the
               // event off to.

               if ((IMENUCTL_GetItemCount(pme->m_pMenu) > 0) &&
                   (IMENUCTL_GetItemCount(pme->m_pSK) > 0))
               {
                  AEERect mRect;
                  AEERect sRect;

                  ISTATIC_GetRect(pme->m_pStatic, &sRect);
                  IMENUCTL_GetRect(pme->m_pMenu, &mRect);

                  if (sRect.y < mRect.y + mRect.dy)
                  {
                     // The static overlaps the Menu
                     // so don't pass on to it
                     if (IsInMenuCtl(wXp, wYp, pme->m_pSK))
                     {
                        IMENUCTL_SetActive(pme->m_pSK, TRUE);
                        bRet = IMENUCTL_HandleEvent(pme->m_pSK, eCode, wParam, dwParam);

#ifdef FEATURE_HAPTIC_SUPPORT
                        if (bRet && (EVT_POINTER_DOWN == eCode))
                        {
                           PlyHapticResponse(pme->m_pHapticResponse,pme->m_ToneData);
                        }
#endif //FEATURE_HAPTIC_SUPPORT

                        return bRet;
                     }

                     return FALSE;
                  }
                  else
                  {
                     // the menu is not covered up by the prompt, so give it
                     // to the Menu and don't pass it onto any SK that might be there
                     if (IsInMenuCtl(wXp, wYp, pme->m_pMenu)) 
                     {
                        IMENUCTL_SetActive(pme->m_pMenu, TRUE);
                        bRet = IMENUCTL_HandleEvent(pme->m_pMenu, eCode, wParam, dwParam);

#ifdef FEATURE_HAPTIC_SUPPORT
                        if (bRet && (EVT_POINTER_DOWN == eCode))
                        {
                           PlyHapticResponse(pme->m_pHapticResponse,pme->m_ToneData);
                        }
#endif //FEATURE_HAPTIC_SUPPORT

                        return bRet;
                     }

                     return FALSE;
                  }
               }

               // Next the softkey control
               if (IsInMenuCtl(wXp, wYp, pme->m_pSK))
               {
                  IMENUCTL_SetActive(pme->m_pSK, TRUE);
                  bRet = IMENUCTL_HandleEvent(pme->m_pSK, eCode, wParam, dwParam);

#ifdef FEATURE_HAPTIC_SUPPORT
                  if (bRet && (EVT_POINTER_DOWN == eCode))
                  {
                     PlyHapticResponse(pme->m_pHapticResponse,pme->m_ToneData);
                  }
#endif //FEATURE_HAPTIC_SUPPORT

                  return bRet;
               }

               // Next General Menu
               if (IsInMenuCtl(wXp, wYp, pme->m_pMenu)) 
               {
                  IMENUCTL_SetActive(pme->m_pMenu, TRUE);
                  bRet = IMENUCTL_HandleEvent(pme->m_pMenu, eCode, wParam, dwParam);

#ifdef FEATURE_HAPTIC_SUPPORT
                  if (bRet && (EVT_POINTER_DOWN == eCode))
                  {
                     PlyHapticResponse(pme->m_pHapticResponse,pme->m_ToneData);
                  }
#endif //FEATURE_HAPTIC_SUPPORT

                  return bRet;
               }

               return FALSE;
            }
         }

         // Next the softkey control
         if (IsInMenuCtl(wXp, wYp, pme->m_pSK))
         {
            IMENUCTL_SetActive(pme->m_pSK, TRUE);
            bRet = IMENUCTL_HandleEvent(pme->m_pSK, eCode, wParam, dwParam);

#ifdef FEATURE_HAPTIC_SUPPORT
            if (bRet && (EVT_POINTER_DOWN == eCode))
            {
               PlyHapticResponse(pme->m_pHapticResponse,pme->m_ToneData);
            }
#endif //FEATURE_HAPTIC_SUPPORT

            return bRet;
         }

         // Next General Menu
         if (IsInMenuCtl(wXp, wYp, pme->m_pMenu)) 
         {
            IMENUCTL_SetActive(pme->m_pMenu, TRUE);
            bRet = IMENUCTL_HandleEvent(pme->m_pMenu, eCode, wParam, dwParam);

#ifdef FEATURE_HAPTIC_SUPPORT
            if (bRet && (EVT_POINTER_DOWN == eCode))
            {
               PlyHapticResponse(pme->m_pHapticResponse,pme->m_ToneData);
            }
#endif //FEATURE_HAPTIC_SUPPORT

            return bRet;
         }

         // Next Main Menu
         if (IsInMenuCtl(wXp, wYp, pme->m_pMainMenu))
         {
            IMENUCTL_SetActive(pme->m_pMainMenu, TRUE);
            bRet = IMENUCTL_HandleEvent(pme->m_pMainMenu, eCode, wParam, dwParam);

#ifdef FEATURE_HAPTIC_SUPPORT
            if (bRet && (EVT_POINTER_DOWN == eCode))
            {
               PlyHapticResponse(pme->m_pHapticResponse,pme->m_ToneData);
            }
#endif //FEATURE_HAPTIC_SUPPORT

            return bRet;
         }
         // Next Main Menu SK
         if ((NULL != pme->m_pMainMenuSK) && IsInMenuCtl(wXp, wYp, pme->m_pMainMenuSK))
         {
            //IMENUCTL_SetActive(pme->m_pMainMenuSK, TRUE);
            bRet = IMENUCTL_HandleEvent(pme->m_pMainMenuSK, eCode, wParam, dwParam);
            // If we handled this we need to update the MainMenu
            if (bRet && (EVT_POINTER_UP == eCode))
            {
               //uint16 numItems = IMENUCTL_GetItemCount(pme->m_pMainMenu);
               uint16 sel = IMENUCTL_GetSel(pme->m_pMainMenuSK);
               IMENUCTL_SetSel(pme->m_pMainMenu, sel);
               IMENUCTL_Redraw(pme->m_pMainMenu);
            }

#ifdef FEATURE_HAPTIC_SUPPORT
            if (bRet && (EVT_POINTER_DOWN == eCode))
            {
               PlyHapticResponse(pme->m_pHapticResponse,pme->m_ToneData);
            }
#endif //FEATURE_HAPTIC_SUPPORT

            return bRet;
         }
      }
      break;
#endif // FEATURE_TOUCHSCREEN_SUPPORT

      case EVT_NOTIFY:
         {
            AEENotify* pae = (AEENotify*)dwParam;
            if (pae)
            {
               switch (pae->dwMask)
               {
#ifdef NMASK_SHELL_START_STATUS
                  case NMASK_SHELL_START_STATUS:
                  {
                     NotifyStartStatus* ps = (NotifyStartStatus*)pae->pData;
                     if (NULL == ps)
                     {
                        FARF(BAM, ("Received NMASK_SHELL_START_STATUS Notifier"));
                     }
                     else
                     {
                        FARF(BAM, ("Received NMASK_SHELL_START_STATUS Notifier for class 0x%x, err = %d", ps->cls, ps->nErr));
                     }
                     
                     if (ps && ps->cls && ps->nErr)
                     {
                        AEEAppInfo ai;

                        // Don't act if notification is for hidden app or
                        // a screen saver
                        AppMgr_GetAEEAppInfo(pme, ps->cls, &ai);
                        if ((ai.wFlags & AFLAG_HIDDEN) ||
                           (ai.wFlags & AFLAG_SCREENSAVER))
                        {
                           return TRUE;
                        }

                        if (ISHELL_ActiveApplet(pme->a.m_pIShell) !=
                           AEECLSID_BREWAPPMGR_BID)
                        {
                           // BAM is not the active applet.  Cache the
                           // class ID and error code associated with
                           // the notifier for later processing.
                           pme->m_nErrCls = ps->cls;
                           pme->m_nErr = ps->nErr;
                           // CR 84264.  BAM needs to handle launch
                           // errors from apps launched by MobileShop.
                           // If BAM is suspended here we know it's
                           // because BAM launched MobileShop.  MobileShop
                           // tells BAM about apps it launches with
                           // BAM_EVT_APP_LAUNCHED.  So if BAM is
                           // suspended AND the class ID in the notifier
                           // matches the ID of an app launched by
                           // MobileShop we infer BAM needs to handle
                           // the launch error.  Further, we know that
                           // BAM will get a resume event.  So in this
                           // particular circumstance BAM should not
                           // start itself, but wait for the resume
                           // to handle the launch error.  See additional
                           // comments for this CR in AppMgr_Resume().
                           if ((TRUE == pme->m_bSuspended) &&
                              (0 != pme->m_nErrCls) &&
                              (pme->m_clsMShopLaunched == pme->m_nErrCls))
                           {
                              FARF(BAM,("NOT Starting for start status"));
                           }
                           else if (pme->m_cfg.clsLaunchedApp == pme->m_nErrCls)
                           {
                              // we started this app, so we need to display the 
                              // error for it.
                              FARF(BAM,("Starting for start status"));
                              ISHELL_StartApplet(pme->a.m_pIShell,
                                 AEECLSID_BREWAPPMGR_BID);
                           }
                           else
                           {
                              FARF(BAM,("NOT Starting for start status, as we did not launch"));
                           }
                        }
                        else
                        {
                           FARF(BAM, ("Show error dialog"));
                           if(AppMgr_FindItemCategory(pme, ps->cls,
                              &pme->m_pCurrentApp))
                           {
                              AppMgr_HandleLaunchError(pme, ps->cls, ps->nErr);
                           }
                        }
                     }
                  }
                     return TRUE;
#endif // NMASK_SHELL_START_STATUS

#ifdef NMASK_SHELL_SYS_ERROR
                  case NMASK_SHELL_SYS_ERROR:
                  {
                     AEECLSID activeApplet;
                     AEESysError * ps = (AEESysError *)pae->pData;
                     FARF(BAM, ("Received NMASK_SHELL_SYS_ERROR Notification"));
                     if (ps)
                     {
#if !defined(AEE_SIMULATOR)
                        if (ps->nErr == EALLOCATED)
                        {
                           return TRUE;
                        }
#endif // AEE_SIMULATOR

                        activeApplet = ISHELL_ActiveApplet(pme->a.m_pIShell);

                        if ((activeApplet != AEECLSID_BREWAPPMGR_BID) &&
                           (0 != activeApplet))
                        {
                           FARF(BAM, ("Start BAM with error for class 0x%x, err = %d, type = %d", ps->cls, ps->nErr, ps->nType));
                           pme->m_nErrCls = ps->cls;
                           pme->m_nErr = ps->nErr;
                           ISHELL_StartApplet(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID);
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
         bRet = AppMgr_SetState(pme, PREV_ENTRY);
         break;
      // Fired by browser
      case EVT_APP_POST_URL:
      case EVT_APP_BROWSE_URL:
         //pme->m_evtStart = eCode;
         return (AppMgr_Browse(pme,(const char *)dwParam));
         

      case EVT_APP_START:
      {
         boolean bResult;
         AEEAppStart* pStartInfo = (AEEAppStart*)dwParam;
         AEECLSID cls = pme->m_nLaunchCls; // store our launch class in case we need it
         FARF(BAM, ("APP_START"));
#ifdef FEATURE_COREAPPUI
         // Notify CoreAppUI that BAM is starting or resuming so it can
         // do whatever it thinks needs to be done.
         if (FALSE == ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_COREAPPUI,
               EVT_APPMGR_PING, 0xdead, AEECLSID_BREWAPPMGR_BID))
         {
            FARF(BAM, ("APP_START: PostEvent() failed"));
         }
#endif // FEATURE_COREAPPUI

         // We have gotten our start event, so we can draw to the screen
         pme->m_bHasStarted = TRUE;
         
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

         if (pStartInfo && pStartInfo->pszArgs)
         {
            pme->m_pAutoStart = pme->m_pArgs = STRDUP(pStartInfo->pszArgs);
         }

         bResult = AppMgr_Start(pme);
         if (FALSE == bResult)
         {
            // If AppMgr_Start() returns FALSE we don't want to run,
            // so report a general error and unregister for notifications.
            int deregister = ISHELL_RegisterNotify(pme->a.m_pIShell,
               AEECLSID_BREWAPPMGR_BID, AEECLSID_SHELL, 0);
            if (SUCCESS != deregister)
            {
               FARF(BAM, ("APP_START: deregister failed"));
            }
            pStartInfo->error = EFAILED;
         }
         else
         {
            if (pme->m_bLaunchClsOnStart)
            {
               pme->m_bLaunchClsOnStart = FALSE;
               if (0 != AppMgr_FindItemCategory(pme, cls, &pme->m_pCurrentApp))
               {
                  FARF(BAM, ("BAM is going to launch app with clsid of 0x%x.", pme->m_nLaunchCls));
                  // We know about the app, And it is now the CurrentApp,
                  // So launch it.
                  (void)AppMgr_LaunchCurrentApplet(pme, FALSE);
               }
            }
         }

         FARF(BAM, ("Returning %d from EVT_APP_START", bResult));
         return bResult;
        }

      case EVT_APP_STOP:
         // Stop Sound & Animation
         FARF(BAM, ("APP_STOP"));
         AppMgr_StopSoundAnimation(pme);
         return TRUE;

      case EVT_APP_SUSPEND:
         FARF(BAM, ("APP_SUSPEND"));

#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
         // Default to True
         pme->m_bFlipOpenWhenSuspended = TRUE;
#endif         
         return AppMgr_Suspend(pme);

      case EVT_APP_RESUME:
         {
            boolean resumeRet = FALSE;
            boolean bProcessFlip = FALSE;
            
            IAnnunciator    *pIAnn=NULL;
            
            if (ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ANNUNCIATOR, 
                    (void **) &pIAnn) == SUCCESS)
            {
                IANNUNCIATOR_Redraw(pIAnn);
                IANNUNCIATOR_Release(pIAnn);
            }
            FARF(BAM, ("APP_RESUME"));
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
#ifdef FEATURE_COREAPPUI
            // Notify CoreAppUI that BAM is starting or resuming so it can
            // do whatever it thinks needs to be done.
            if (FALSE == ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_COREAPPUI,
                  EVT_APPMGR_PING, 0xbeef, AEECLSID_BREWAPPMGR_BID))
            {
               FARF(BAM, ("APP_RESUME: PostEvent() failed"));
            }
#endif // FEATURE_COREAPPUI

            FARF(BAM, ("Our current display thinks that its Enabled state is %d", IDISPLAY_IsEnabled(pme->m_pIDisplay)));

            bProcessFlip = AppMgr_IsDifferentFlipPosition(pme);

#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
            if (bProcessFlip)
            {
               resumeRet = AppMgr_Resume(pme);

               if (pme->m_bExitAfterSANotification)
					{
						FARF(BAM, ("Calling CloseApplet as we need to exit after download"));
						// We have notified the ShortcutApp, so we can now exit
						ISHELL_CloseApplet(pme->a.m_pIShell, TRUE);
						return FALSE;
					}

               resumeRet &= AppMgr_SwitchDisplays(pme, TRUE);
            }
            else if (!IDISPLAY_IsEnabled(pme->m_pIDisplay) && !pme->m_bExitAfterSANotification)
            {
               // Our display was "un-enabled" when we were suspended, so we need to make
               // sure that our display is enabled again before we do anything. I don't think that 
               // we need to call MakeDefault, here but I am doing it just to make sure that it 
               // does happen, and we get the call back.
               FARF(BAM, ("Our display is not enabled, we are enabling it"));
               IDISPLAY_MakeDefault(pme->m_pIDisplay);
               pme->cbDisplayEnabled = (AEECallback*) MALLOC(sizeof(AEECallback));
               MEMSET(pme->cbDisplayEnabled, 0, sizeof(AEECallback));
               CALLBACK_Init(pme->cbDisplayEnabled, AppMgr_EnableDispRedrawCallback, pme);
               IDISPLAY_NotifyEnable(pme->m_pIDisplay, pme->cbDisplayEnabled);

               // we have to fake that we are done here and return true;
               // AppMgr_EnableDispRedrawCallback, will exit bam if we fail to "resume"
               resumeRet = TRUE;

            }
            else
#endif
            {
               if (gbENDKey_RecievedInSuspend &&
                   (pme->m_wState != ST_MAINMENU))
               {
                  pme->m_wState = ST_MAINMENU;
                  gbENDKey_RecievedInSuspend = FALSE;
               }
               resumeRet = AppMgr_Resume(pme);
               if (!resumeRet)
               {
                  ISHELL_CloseApplet(pme->a.m_pIShell, TRUE);
                  return TRUE;
               }

               if (pme->m_bExitAfterSANotification)
    		   {
    		      FARF(BAM, ("Calling CloseApplet as we need to exit after download"));
    			  // We have notified the ShortcutApp, so we can now exit
    			  ISHELL_CloseApplet(pme->a.m_pIShell, TRUE);
    			  return FALSE;
    		   }
            }

            return resumeRet;
         }

      // Menu Selection Changed
      case EVT_CTL_SEL_CHANGED:
         return AppMgr_CtlSelChanged(pme, wParam, dwParam);

      case EVT_CTL_TAB:
			{
				boolean handled = FALSE;
            // Ignore CTL_TAB if the display is not
            // touch-enabled.
            if (TRUE == pme->m_bDispTouchEnabled)
            {
				   if (IMENUCTL_IsActive(pme->m_pSK))
				   {
					   if(pme->m_pStatic)
					   {
						   ISTATIC_SetActive(pme->m_pStatic, TRUE);
						   IMENUCTL_SetActive(pme->m_pSK, FALSE);

						   handled = TRUE;
					   }
					   else if (IMENUCTL_GetItemCount(pme->m_pMenu) > 0)
					   {
						   IMENUCTL_SetActive(pme->m_pMenu, TRUE);
						   IMENUCTL_SetActive(pme->m_pSK, FALSE);

						   handled = TRUE;
					   }
                  else if (ST_MAINMENU == pme->m_wState ||
                           ST_FAVORITES == pme->m_wState ||
                           ST_WORK == pme->m_wState ||
                           ST_FUN == pme->m_wState)
					   {
						   IMENUCTL_SetActive(pme->m_pMainMenu, TRUE);
						   IMENUCTL_SetActive(pme->m_pSK, FALSE);

						   handled = TRUE;
					   }
				   }
				   else if (IMENUCTL_IsActive(pme->m_pMenu))
				   {
					   if(pme->m_pStatic)
					   {
						   ISTATIC_SetActive(pme->m_pStatic, TRUE);
						   IMENUCTL_SetActive(pme->m_pMenu, FALSE);

						   handled = TRUE;
					   }
					   else if (IMENUCTL_GetItemCount(pme->m_pSK) > 0)
					   {
						   IMENUCTL_SetActive(pme->m_pSK, TRUE);
						   IMENUCTL_SetActive(pme->m_pMenu, FALSE);
						   handled = TRUE;
					   }
				   }
				   else if (IMENUCTL_IsActive(pme->m_pMainMenu))
				   {
                  if (IMENUCTL_GetItemCount(pme->m_pSK) > 0)
					   {
						   IMENUCTL_SetActive(pme->m_pSK, TRUE);
						   IMENUCTL_SetActive(pme->m_pMainMenu, FALSE);
						   handled = TRUE;
					   }
                  else
                  {
					      handled = FALSE;
                  }
				   }
            }

				return handled;
			}
			break;

#ifdef FEATURE_SCREEN_ROTATE
      case EVT_SCR_ROTATE:
      {
         return AppMgr_HandleScreenRotate(pme, wParam, dwParam);
      }

#endif // FEATURE_SCREEN_ROTATE

      case EVT_KEYGUARD:
         // We got a keyguard event.  This will happen on a Slide and after a flip.
         FARF(BAM, ("Got EVT_KEYGUARD with wParam %d", wParam));

         // If we get a keyguard event, ensure that we are not going to exit on resume
         pme->m_bExitAfterSANotification = FALSE;

         if (TRUE == wParam)
         {
            // The slider/clam was closed and KEYGUARD is on.  Check to make sure that we
            // should continue running.
            if (pme->m_bCancelOnClose)
            {
               // We are not supose to continue to run.  So return false.  BREW
               // will then kill us and the rest of the BREW stack.
               FARF(BAM, ("Exiting BAM on KeyGuard as CancelOnClose is True"));

               // Return that We don't handle the Keyguard event, and let 
               // BREW take care of the rest.
               return FALSE;
            }
         }

#ifdef FEATURE_LESS_THAN_6_KEYS
         // Map keys, if any need to be mapped
         AppMgr_SetKeyList(pme, NULL, 0);
#endif
         
         return TRUE;

      case EVT_FLIP:
         {
            //AEEFlipInfo flipInfo = dwParam;
            FARF(BAM, ("Got EVT_FLIP with wParam %d and FlipId of %d", wParam, DEVNOTIFY_GET_ID(dwParam)));

            // If we get a flip event, ensure that we are not going to exit on resume
            pme->m_bExitAfterSANotification = FALSE;


            if ((FALSE == wParam) &&(pme->m_bCancelOnClose))
            {
               FARF(BAM, ("Exiting BAM as CancelOnClose is True"));

               // Return that We don't handle the Keyguard event, and let 
               // BREW take care of the rest.
               return FALSE;
            }
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
            if (TRUE == pme->m_bSuspended)
            {
               // Some how we got a flip event even though we are suspended
               // so ignore it.  If we did not we would update the display and 
               // cause problems for the currently running app.
               return TRUE;
            }
 
            FARF(BAM, ("Trying to switch displays"));

            return AppMgr_SwitchDisplays(pme, (boolean)wParam);
#else
            // Return that We don't handle the Flip event, and let 
            // BREW take care of the rest.
            return FALSE;
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
         }

      case BAM_EVT_APP_ADDED:
         pme->m_bMShopChangedNumApps = TRUE;
         break;

      case BAM_EVT_APP_DELETED:
         pme->m_bMShopChangedNumApps = TRUE;
         break;

      case BAM_EVT_APP_LAUNCHED:
         pme->m_clsMShopLaunched = dwParam;
         break;

      case BAM_EVT_CONTINUE_DOWNLOAD:
         FARF(BAM, ("Got the BAM_EVT_CONTINUE_DOWNLOAD event"));
         pme->m_bExitAfterSANotification = TRUE;
         break;


      case EVT_KEY:
         switch (wParam)
         {
            case AVK_CLR:
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
               // Set to false always.  If the prompt was not being displayed
               // it should have already been false.  If it was was being displayed
               // it is now about to exit.
               pme->m_bPromptIsDisplayed = FALSE;
#endif // FEATURE_SUPPORT_MULTIPLE_DISPLAYS

               // Clear when airtime use warning is getting displayed
               if (pme->m_bWarningDisplayed)
               {
                  // Reset warning displayed flag
                  pme->m_bWarningDisplayed = FALSE;
                  // Show current state UI
                  bRet = AppMgr_SetState(pme, pme->m_wState);
               }
               else
               {
                  if ((pme->m_wState == ST_EDITSOUNDIMAGE) &&
                     IMENUCTL_IsActive(pme->m_pMenu))
                  {
                     // Previous State - No File Selected
                     return AppMgr_FileSelected(pme, 0);
                  }
                  else if ((pme->m_wState >= ST_ORDERMAINMENU) &&
                     (pme->m_wState <= ST_ORDERFUN) &&
                     (pme->m_wLastState == ST_OPTIONS))
                  {
                     
                     AppMgr_VisualConfirmation(pme, IDS_ORDER_CONFIRM);
                     return TRUE;
                  }
                  else if (pme->m_wState == ST_UPGRADEAUTOPROMPT)
                  {
                     // Pressing CLR while in the upgrade auto-prompt should
                     // return the user to the screen the app was launched
                     // from.  We save that state before transitioning to
                     // the upgrade auto-prompt state.
                     bRet = AppMgr_SetState(pme, PREV_ENTRY);
                  }
                  else
                  {
                     if (pme->m_bFreeCurrentApp)
                     {
                        FreePtr((void**)&pme->m_pCurrentApp);
                        pme->m_bFreeCurrentApp = FALSE;
                     }
                     bRet = AppMgr_SetState(pme, PREV_ENTRY);
                     /* Patch for CR107765 -- CLR key does not cause BAM to exit from main menu. */
                     /* I hate doing it this way, but due to how this routine is structured and */
                     /* in the interest of making the smallest/safest change possible, here we are. */
                     if (!bRet){
                        return bRet;
                     }
                     /* End patch for CR107765 */
#ifdef FEATURE_TOUCHSCREEN_SUPPORT
                     bSKUpdate = TRUE;
#endif //#ifdef FEATURE_TOUCHSCREEN_SUPPORT
                  }
               }
               break;
            case AVK_LEFT:
            case AVK_RIGHT:
               // First chance to softkey
               if (IMENUCTL_IsActive(pme->m_pSK))
                  return IMENUCTL_HandleEvent(pme->m_pSK, eCode, wParam, dwParam);

               // next chance to the Menu
               if (IMENUCTL_IsActive(pme->m_pMenu))
                  return IMENUCTL_HandleEvent(pme->m_pMenu, eCode, wParam, dwParam);

               // Provide chance to main menu
               if (IMENUCTL_IsActive(pme->m_pMainMenu))
               {
                  boolean bHandled = IMENUCTL_HandleEvent(pme->m_pMainMenu, eCode, wParam, dwParam);

                  if (bHandled && pme->m_pMainMenuSK)
                     IMENUCTL_HandleEvent(pme->m_pMainMenuSK, eCode, wParam, dwParam);

                  return bHandled;
               }

               // no menu is active, if the prompt is up activeate the menu that has keys
					if (pme->m_pStatic)
					{
						boolean handled = ISTATIC_HandleEvent(pme->m_pStatic, eCode, wParam, dwParam);
						if (!handled)
						{
							if (IMENUCTL_GetItemCount(pme->m_pSK) > 0)
							{
								IMENUCTL_SetActive(pme->m_pSK, TRUE);
								ISTATIC_SetActive(pme->m_pStatic, FALSE);
								handled = TRUE;
							}
							else if (IMENUCTL_GetItemCount(pme->m_pMenu) > 0)
							{
								IMENUCTL_SetActive(pme->m_pMenu, TRUE);
								ISTATIC_SetActive(pme->m_pStatic, FALSE);
								handled = TRUE;
							}
						}

						return handled;
					}

               //if nothimg is active, set it io the soft key
               // if it has any  items in it
               if (IMENUCTL_GetItemCount(pme->m_pSK) > 0)
					{
						IMENUCTL_SetActive(pme->m_pSK, TRUE);
                  return TRUE;
					}
               


               break;

            case AVK_INFO:
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

               // Screen savers in non-touch has both the menu and the SK,
               // So we need to pass up/down to the Menu before we pass it
               // to the SK.
               if (ST_SCREENSAVERS == pme->m_wState && !pme->m_bDispTouchEnabled)
               {
                  if (IMENUCTL_IsActive(pme->m_pMenu)) {
                     if (IMENUCTL_HandleEvent(pme->m_pMenu, eCode, wParam, dwParam)) {
                        return TRUE;
                     }
                  }
                  return FALSE;
               }

               if (IMENUCTL_IsActive(pme->m_pSK))
						return IMENUCTL_HandleEvent(pme->m_pSK, eCode, wParam, dwParam);

#ifdef PROMPT_ALWAYS_USE_SK
               // Provide a chance to static control
               if (pme->m_pStatic)
               {
                  if (ISTATIC_HandleEvent(pme->m_pStatic, eCode, wParam, dwParam))
                  {
                     return TRUE;
                  }
               }
#endif // PROMPT_ALWAYS_USE_SK

               // Give a chance to menu control
               if (IMENUCTL_IsActive(pme->m_pMenu)) {
                  if (IMENUCTL_HandleEvent(pme->m_pMenu, eCode, wParam, dwParam)) {
                     if (!OEM_SMALL_SCREEN_DEVICE && (pme->m_wState == ST_MANAGETOOL))
                        AppMgr_DisplayMemoryInfo(pme);
                     return(TRUE);
                  }
                  return FALSE;
               }

#ifndef PROMPT_ALWAYS_USE_SK
               // Provide a chance to static control
               if (pme->m_pStatic)
               {
                  if (ISTATIC_HandleEvent(pme->m_pStatic, eCode, wParam, dwParam))
                  {
                     return TRUE;
                  }
               }
#endif // PROMPT_ALWAYS_USE_SK

               // Provide a chance to main menu
               if (IMENUCTL_IsActive(pme->m_pMainMenu))
                  return IMENUCTL_HandleEvent(pme->m_pMainMenu, eCode, wParam, dwParam);

               if (pme->m_bPromptIsDisplayed)
               {
                  // We have some special work to do to make sure that we don't
                  // set a menu active that might be below the prompt

                  if ((IMENUCTL_GetItemCount(pme->m_pMenu) > 0) &&
                      (IMENUCTL_GetItemCount(pme->m_pSK) > 0))
                  {
                     AEERect mRect;
                     AEERect sRect;

                     ISTATIC_GetRect(pme->m_pStatic, &sRect);
                     IMENUCTL_GetRect(pme->m_pMenu, &mRect);

                     if (sRect.y < mRect.y + mRect.dy)
                     {
                        // The static overlaps the Menu, so don't activate it
                        IMENUCTL_SetActive(pme->m_pSK, TRUE);
                        return TRUE;
                     }
                        
                     // the menu is not covered up by the prompt, so give it
                     // to the Menu and don't pass it onto any SK that might be there
                     IMENUCTL_SetActive(pme->m_pMenu, TRUE);
                     return TRUE;
                  }
               }

               //if nothing is active, set it io the Menu
               // if it has any  items in it, unless we are
               // using the MainMenu, then set it active
               if (IMENUCTL_GetItemCount(pme->m_pMenu) > 0)
					{
						IMENUCTL_SetActive(pme->m_pMenu, TRUE);
                  return TRUE;
					}
               else if (ST_MAINMENU == pme->m_wState ||
                        ST_FAVORITES == pme->m_wState ||
                        ST_WORK == pme->m_wState ||
                        ST_FUN == pme->m_wState)
               {
                  IMENUCTL_SetActive(pme->m_pMainMenu, TRUE);
               }

               
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
#ifdef MSHOP_STATES
               // Allow credit back only when user is in MShop
               if ((pme->m_wState == ST_MSHOP) &&
                  (IMENUCTL_GetSel(pme->m_pMenu) == IDC_MSHOP_CATALOG))
#else
               // Allow credit back only when user is in Help
               if (pme->m_wState == ST_HELP)
#endif
               {
                  if ((pme->m_pCS[pme->m_nCSBufIndx] - '0') == (wParam - AVK_0))
                     pme->m_nCSBufIndx++;
                  else
                     pme->m_nCSBufIndx = 0;

                  if (pme->m_nCSBufIndx == STRLEN(CREDIT_SEQUENCE))
                  {
                     pme->m_nCSBufIndx = 0;
                     ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID, EVT_COMMAND, IDC_MSHOP_CREDITBACK, 0);
                  }

               }
               return TRUE;
            // 中国电信要求，但会修改mif文件，暂做成通用功能
            case AVK_END:
                if (pme->m_wState != ST_MAINMENU)
                {
                    return AppMgr_SetState(pme, ST_MAINMENU);
                }
                else
                {
                    ISHELL_CloseApplet(pme->a.m_pIShell, TRUE);
                }
                return TRUE;
         }
         
         // TODO: not sure why we need to return FALSE 
         bRet = TRUE;
         break;
      case EVT_COMMAND:
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
         // Set to false always.  If the prompt was not being displayed
         // it should have already been false.  If it was was being displayed
         // it is now about to exit.
         // This might be an aggressive place to place it, but it is easier than 
         // figureing out all of the commands that could possibly close
         // the Prompt "dialog".  Also when new ones are added they 
         // are already picked up here
         pme->m_bPromptIsDisplayed = FALSE;
#endif // FEATURE_SUPPORT_MULTIPLE_DISPLAYS

         switch (wParam)
         {
            case IDC_OPTIONS:
               pme->m_nMenuSel[++pme->m_nIndex] = 0;
               bRet = AppMgr_SetState(pme, ST_OPTIONS);
               break;

            case IDC_MSHOP:
#ifdef MSHOP_STATES
               pme->m_nMenuSel[++pme->m_nIndex] = 0;
               bRet = AppMgr_SetState(pme, ST_MSHOP);
#else
               bRet = AppMgr_RequestMShop(pme, wParam, (boolean)pme->m_cfg.bBrowseWarning);
#endif
              break;            
            case IDC_HELP:
               pme->m_nMenuSel[++pme->m_nIndex] = 0;
               bRet = AppMgr_SetState(pme, ST_HELP);
               break;

            case IDC_BREW_APPS:
            case IDC_FAVORITES:
            case IDC_WORK:
            case IDC_FUN:
               switch (pme->m_wState)
               {
                  case ST_MAINMENU:
                     pme->m_nMenuSel[++pme->m_nIndex] = 0;
                     pme->m_pCurrentApp = pme->m_pList[wParam - IDC_BREW_APPS]; 
                     bRet = AppMgr_SetState(pme, ST_MAINMENU + wParam - IDC_BREW_APPS);
                     break;

                  case ST_ORDERAPPS:                     
                     // Cache Current Menu Selection
                     pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
                     pme->m_wLastState = pme->m_wState;
                     bRet = AppMgr_SetState(pme, wParam - IDC_BREW_APPS + ST_ORDERMAINMENU);
  //                   bSKDoNotUpdate = TRUE;
                     break;

                  case ST_MOVEAPPS:                     
                     // Cache Current Menu Selection
                     pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
                     bRet = AppMgr_SetState(pme, wParam - IDC_BREW_APPS + ST_MOVEMAINMENU);
                     break;

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
               bRet = AppMgr_SetState(pme, pme->m_wState);
               break;

            case IDC_LAUNCHER_RUN_NOW:
               pme->m_bPromptIsDisplayed = FALSE;
               return AppMgr_LaunchCurrentApplet(pme, TRUE);

            case IDC_LAUNCHER_RESTORE:
               pme->m_bPromptIsDisplayed = FALSE;
               return AppMgr_RequestMShop(pme, wParam, (boolean)pme->m_cfg.bRestoreWarning);

            case IDC_LAUNCHER_BUY:
            case IDC_LAUNCHER_BUY_NOW:
               pme->m_bPromptIsDisplayed = FALSE;
               return AppMgr_RequestMShop(pme, wParam, (boolean)pme->m_cfg.bBuyWarning);

            case IDC_LAUNCHER_VERIFY:
               pme->m_bPromptIsDisplayed = FALSE;
               return AppMgr_RequestMShop(pme, wParam, FALSE);

            case IDC_UPGRADE_CHECK:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               return AppMgr_RequestMShop(pme, wParam,
                  (boolean)pme->m_cfg.bUpgradeWarning);

#ifdef MSHOP_STATES
            case IDC_MSHOP_CATALOG:
               return AppMgr_RequestMShop(pme, wParam, (boolean)pme->m_cfg.bBrowseWarning);
               
            case IDC_MSHOP_FIND:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               return AppMgr_RequestMShop(pme, wParam, (boolean)pme->m_cfg.bSearchWarning);

            case IDC_MSHOP_GUIDE:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               // Set State to ST_MSHOPGUIDE
               (void)AppMgr_SetState(pme, ST_MSHOPGUIDE);
               bRet = TRUE;
               break;

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
#endif

            case IDC_MSHOP_CREDITBACK:
               return AppMgr_RequestMShop(pme, wParam, (boolean)pme->m_cfg.bCreditBackWarning);

            case IDC_OPTIONS_ORDER_APPS:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               bRet = AppMgr_SetState(pme, ST_ORDERAPPS);
               break;

            case IDC_OPTIONS_MOVE_APPS:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               bRet = AppMgr_SetState(pme, ST_MOVEAPPS);
               break;

            case IDC_OPTIONS_APP_MANAGEMENT:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               (void)AppMgr_SetState(pme, ST_MANAGETOOL);
               bRet = TRUE;
               break;

            case IDC_OPT_INFO:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               bRet = AppMgr_SetState(pme, ST_SYSTEMINFO);
               break;

            case IDC_LOCK_APP:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               bRet = AppMgr_SetState(pme, ST_LOCKUNLOCK);
               break;

            case IDC_LOCKED:
            case IDC_UNLOCKED:
               if (AppMgr_GetItemID(pme, pme->m_pCurrentApp))
               {                             
#ifdef USE_IDL2
                  if (!LockApp(pme->m_pDownload2, AppMgr_GetItemID(pme, pme->m_pCurrentApp), (boolean)(wParam == IDC_LOCKED)))
#else
                  if (!LockApp(pme->m_pDownload, AppMgr_GetItemID(pme, pme->m_pCurrentApp), (boolean)(wParam == IDC_LOCKED)))
#endif
                  {
                     uint16 msgId = 0;
                     uint16 wButtons[2];

                     // we failed to lock/unlock.  set an error message
                     if (wParam == IDC_LOCKED)
                     {
                        msgId = IDS_LOCK_APP_FAILED;
                     }
                     else
                     {
                        msgId = IDS_UNLOCK_APP_FAILED;
                     }

                     MEMSET(wButtons, 0, sizeof(wButtons));
                     wButtons[0] = IDC_OK;
                     
                     AppMgr_Prompt(pme, AppMgr_GetAppName(pme, pme->m_pCurrentApp), 0, 0, msgId, wButtons, wButtons[0], ST_NOSCROLL, FALSE);
                  }
                  else
                  {
                     AppMgr_VisualConfirmation(pme, 0);
                  }
               }
               return TRUE;

            case IDC_REMOVE_WARNING:
               return AppMgr_RemoveWarning(pme);

            case IDC_REMOVE:
               pme->m_wLastState = ST_APPINFO;
               bRet = AppMgr_SetState(pme, ST_REMOVE);
               break;

            case IDC_VIEW_PROPERTIES:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               bRet = AppMgr_SetState(pme, ST_VIEWPROPERTIES);
               break;

            case IDC_SET_CATEGORY:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               bRet = AppMgr_SetState(pme, ST_SETCATEGORY);
               break;

            case IDC_OPTIONS_VIEW_LOG: {
               char *   pBuffer = (char*)MALLOC(MAX_STR_SIZE);
            
               MEMSET(pBuffer, 0, MAX_STR_SIZE);
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               // Start App Manager as if resuming
               pme->m_bResume = TRUE;
               SPRINTF(pBuffer, "%s%c%s", CMSHOP_CMD_BASE, CMSHOP_CMD_COLON, CMSHOP_CMD_LOG);
               if (pBuffer) {
#ifdef CMSHOP_SECURE_ALL
                  if (AppendSecretString(pme->a.m_pIShell, pBuffer)) 
#endif //CMSHOP_SECURE_ALL
                  {
                     ISHELL_BrowseURL(pme->a.m_pIShell, pBuffer);
                  }
                  FreePtr((void**)&pBuffer);
               }
               return TRUE;
            }
            case IDC_OPTIONS_MSHOP_SERVER: {
              char *   pBuffer = (char*)MALLOC(MAX_STR_SIZE);
               
               MEMSET(pBuffer, 0, MAX_STR_SIZE);
               // Reset capability flags
               pme->m_MShopCfg.bSearch = OEM_ADS_SEARCH_APPS;
               pme->m_MShopCfg.bUpgrade = OEM_ADS_UPGRADE_CHECK;
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               // Start App Manager as if resuming
               pme->m_bResume = TRUE;
               SPRINTF(pBuffer, "%s%c%s", CMSHOP_CMD_BASE, CMSHOP_CMD_COLON, CMSHOP_CMD_SERVER);
               if (pBuffer) {
#ifdef CMSHOP_SECURE_ALL
                  if (AppendSecretString(pme->a.m_pIShell, pBuffer)) 
#endif //CMSHOP_SECURE_ALL 
                  {
                     ISHELL_BrowseURL(pme->a.m_pIShell, pBuffer);
                  }
                  FreePtr((void**)&pBuffer);
               }
               return TRUE;
            }
            case IDC_OPTIONS_MAINMENU_STYLE:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               bRet = AppMgr_SetState(pme, ST_MAINMENUVIEW);
               break;

            case IDC_LIST_VIEW:
               if (!pme->m_cfg.bListView)
               {
                  pme->m_cfg.bListView = TRUE;
                  FreeObj((void**)&pme->m_pMainMenu);
                  FreeObj((void**)&pme->m_pMainMenuSK);
                  if (CreateInstance(pme->a.m_pIShell, GetMenuCtlClassID(pme->m_bDispTouchEnabled), (void**)&pme->m_pMainMenu) != AEE_SUCCESS)
                     ISHELL_CloseApplet(pme->a.m_pIShell, TRUE);
                  else
                     SetMenuStyleAndColor(pme->m_pMainMenu, GetMenuCtlClassID(pme->m_bDispTouchEnabled), pme->m_nColorDepth, pme->m_bDispTouchEnabled);

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
                  if (CreateInstance(pme->a.m_pIShell, GetIconViewClassID(pme->m_bDispTouchEnabled), (void**)&pme->m_pMainMenu) != AEE_SUCCESS)
                     ISHELL_CloseApplet(pme->a.m_pIShell, TRUE);
                  else
                     SetMenuStyleAndColor(pme->m_pMainMenu, GetIconViewClassID(pme->m_bDispTouchEnabled), pme->m_nColorDepth, pme->m_bDispTouchEnabled);
                  
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
#ifdef ENABLE_LARGE_ICON_VIEW
//If ENABLE_LARGE_ICON_VIEW is not defined we should never hit this case,
//but if we do make it we return without changing modes.
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
                  if ((CreateInstance(pme->a.m_pIShell, GetIconViewClassID(pme->m_bDispTouchEnabled), (void**)&pme->m_pMainMenu) != AEE_SUCCESS) ||
                      (CreateInstance(pme->a.m_pIShell, GetSoftKeyClassID(pme->m_bDispTouchEnabled), (void**)&pme->m_pMainMenuSK) != AEE_SUCCESS))
                  {
                     ISHELL_CloseApplet(pme->a.m_pIShell, TRUE);
                  }
                  else
                  {
                     AppMgr_LargeIconViewSettings(pme, pme->m_pMainMenu, pme->m_pMainMenuSK);
                  }
               }
#endif //ENABLE_LARGE_ICON_VIEW
               AppMgr_VisualConfirmation(pme, 0);
               return TRUE;

            case IDC_OPTIONS_SCREEN_SAVERS:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               bRet = AppMgr_SetState(pme, ST_SCREENSAVERS);
               break;

            case IDC_AIRTIME_WARNINGS:
               // Cache Current Menu Selection
               pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
               bRet = AppMgr_SetState(pme, ST_AIRTIMEWARNINGS);
               break;

            case IDC_AIRTIME_WARNINGS_DISABLED:
               pme->m_cfg.bBrowseWarning = FALSE;
               pme->m_cfg.bBuyWarning = FALSE;
               pme->m_cfg.bCreditBackWarning = FALSE;
               pme->m_cfg.bRestoreWarning = FALSE;
               pme->m_cfg.bSearchWarning = FALSE;
               pme->m_cfg.bUpgradeWarning = FALSE;
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
               bRet = AppMgr_SetState(pme, ST_EDITSOUNDIMAGE);
               break;

            case IDC_CONFIGURE_DONE:
               bRet = AppMgr_SetState(pme, PREV_ENTRY);
               break;

            case IDC_CONFIGURE_RESET:
               STRCPY(pme->m_pCurrentApp->szIcon, "");
               STRCPY(pme->m_pCurrentApp->szSound, "");
               pme->m_nSelCache = IDC_CONFIGURE_RESET;
               (void)AppMgr_SetState(pme, ST_EDITSOUNDIMAGE);
               bRet = TRUE;
               break;

            case IDC_CONFIGURE_IMAGE:
               pme->m_wEditSoundImageSubState = ST_EDITSOUNDIMAGEBROWSEIMAGES;
               AppMgr_ShowFiles(pme, TRUE);
               return TRUE;

            case IDC_CONFIGURE_SOUND:
               pme->m_wEditSoundImageSubState = ST_EDITSOUNDIMAGEBROWSESOUND;
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
                     (void)AppMgr_SetState(pme, pme->m_wState);
                     // Post cached event
                     bRet = ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID, pme->m_appEvent.evt, pme->m_appEvent.wParam, pme->m_appEvent.dwParam);
                  }
                  else
                     // Ask if user want's to see it again
                     bRet = AppMgr_ShowAirtimeWarning(pme, IDS_AIRTIME_WARNING_AGAIN);
               }
               else if (pme->m_pMenu && IMENUCTL_IsActive(pme->m_pMenu) &&
                 (pme->m_wState >= ST_ORDERMAINMENU) &&(pme->m_wState <= ST_ORDERFUN))
               {
                  // we are on order menu to reordering
                  //(void)AppMgr_HandleEvent(pme, EVT_KEY, AVK_SELECT, 0);
                  AppMgr_Order(pme);
                  AppMgr_SetState(pme, pme->m_wState);
                  bRet = TRUE;
#ifdef FEATURE_TOUCHSCREEN_SUPPORT
                  bSKUpdate = TRUE;
#endif //FEATURE_TOUCHSCREEN_SUPPORT
               }
               else
                  bRet = AppMgr_SetState(pme, PREV_ENTRY);
              break;

            case IDC_CLOSE:
               bRet = AppMgr_SetState(pme, pme->m_wState);
#ifdef FEATURE_TOUCHSCREEN_SUPPORT
               // after viewing help, the menu is recreated, however, this is in same state
               // we need to force to recreate SK menu
               bSKUpdate = TRUE;
#endif //FEATURE_TOUCHSCREEN_SUPPORT
               break;

            case IDC_YES:
               if (pme->m_bWarningDisplayed)
               {
                  // Show current state UI
                  (void)AppMgr_SetState(pme, pme->m_wState);

                  // Post cached event
                  bRet = ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID, pme->m_appEvent.evt, pme->m_appEvent.wParam, pme->m_appEvent.dwParam);
               }
               else
               {
                  // Coming here means affirmation for app removal
                  // Request MShop to remove app
                  AppMgr_RequestMShop(pme, IDC_REMOVE, FALSE);

                  // Fix for CR88685.  The back state is set to APPINFO
                  // on transition to ST_REMOVE (see CL390256 or CR87483).
                  // That works when the remove is cancelled, but if it
                  // proceeds we want to resume in ST_MANAGETOOL.
                  // See also changes in MobileShop.c
                  pme->m_wLastState = ST_MANAGETOOL;

                  // Set Previous state as state to resume from
                  (void)AppMgr_SetState(pme, PREV_ENTRY);
                  bRet = TRUE;
               }
               break;

            case IDC_NO:
               if (ST_UPGRADEAUTOPROMPT == pme->m_wState)
               {
                  // Choosing "No" in the upgrade auto-prompt means
                  // the user just wants to run the app, not upgrade.
                  return AppMgr_LaunchCurrentApplet(pme, TRUE);
               }
               if (pme->m_bFreeCurrentApp)
               {
                  FreePtr((void**)&pme->m_pCurrentApp);
                  pme->m_bFreeCurrentApp = FALSE;
               }
               (void)AppMgr_SetState(pme, PREV_ENTRY);
               bRet = TRUE;
               break;

            case IDC_SET_SS:
               {
                  uint16 nSel = IMENUCTL_GetSel(pme->m_pMenu);

                  AppMgr_SetScreensaver(pme, nSel);
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
#ifdef USE_IDL2
                     dwItemID = GetItemIDFromClassID(pme->m_pDownload2, cls, NULL);
#else
                     dwItemID = GetItemIDFromClassID(pme->m_pDownload, cls, NULL);
#endif

                     if (dwItemID)
                     {
                        pme->m_pCurrentApp = (AppMgrItem*)MALLOC(sizeof(AppMgrItem));
                        pme->m_bFreeCurrentApp = TRUE;

                        pme->m_pCurrentApp->cls = cls;
                        pme->m_pCurrentApp->dwItemID = dwItemID;
                        pme->m_pCurrentApp->dwUpgradeID = 0;
                        pme->m_pCurrentApp->bHasShortcut = FALSE;
                        pme->m_pCurrentApp->dwNumLaunches = 0;
                        pme->m_pCurrentApp->dwLastLaunch = 0;
                     
                        pme->m_wLastState = pme->m_wState;
                        (void)AppMgr_SetState(pme, ST_REMOVE);
                     }
                     else
                        AppMgr_VisualConfirmation(pme, IDS_PRELOADED_SS_REMOVE);
                  }
#else // FEATURE_BREW_DOWNLOAD
                        AppMgr_VisualConfirmation(pme, IDS_PRELOADED_SS_REMOVE);
#endif // FEATURE_BREW_DOWNLOAD
               }
               bRet = TRUE;
               break;

            case IDC_UPGRADEAUTOPROMPT_PURCHASE:
               if (ST_UPGRADEAUTOPROMPT == pme->m_wState)
               {
                  // This choice in the upgrade auto-prompt means the user
                  // wants to do the upgrade.
                  DLITEMID dlItem = 0;

                  // Cache Current Menu Selection
                  pme->m_nMenuSel[++pme->m_nIndex] =
                     IMENUCTL_GetSel(pme->m_pMenu);

                  // There should be a current applet and it should have its
                  // upgrade item stashed inside of it.
                  if (NULL == pme->m_pCurrentApp)
                  {
                     FARF(BAM, ("No current app to upgrade"));
                     (void)AppMgr_SetState(pme, PREV_ENTRY);
                  }
                  dlItem = pme->m_pCurrentApp->dwUpgradeID;
                  if (0 == dlItem)
                  {
                     FARF(BAM, ("No upgrade item in current app"));
                     AppMgr_SetState(pme, ST_MAINMENU);
                  }
                  else
                  {
                     FARF(BAM, ("Upgrade item found in current app"));
                     // Request MShop to download the upgrade item
                     AppMgr_RequestMShop(pme, IDC_UPGRADEAUTOPROMPT_PURCHASE,
                        FALSE);
                  }
               }
               else
               {
                  FARF(BAM, ("Invalid state for upgrade auto-prompt purchase"));
               }
               bRet = TRUE;
               break;

            case IDS_UP:
              {
                //IMENUCTL_HandleEvent(pme->m_pMenu, EVT_KEY, AVK_UP, 0);
                AppMgr_HandleEvent(pme, EVT_KEY, AVK_UP, 0);
              }
              break;

            case IDS_DOWN:
              {
               // IMENUCTL_HandleEvent(pme->m_pMenu, EVT_KEY, AVK_DOWN, 0);
                AppMgr_HandleEvent(pme, EVT_KEY, AVK_DOWN, 0);
              }
              break;

            case IDC_BACK:
               if (pme->m_pMenu && IMENUCTL_IsActive(pme->m_pMenu) && pme->m_bDispTouchEnabled &&
                   (pme->m_wState >= ST_ORDERMAINMENU) &&(pme->m_wState <= ST_ORDERFUN))
               {
                  // we are on order menu (in touchscreen mode) and they were changing
                  // the order, set the order by calling order
                  AppMgr_Order(pme);
               }
              AppMgr_HandleEvent(pme, EVT_KEY, AVK_CLR, 0);
              break;

            default:
               {
                  if(wParam >= IDC_MOVE_APP){             // Configure Move App list
                     IMENUCTL_GetItemData(pme->m_pMenu, IMENUCTL_GetSel(pme->m_pMenu), (uint32*)&pme->m_pCurrentApp);
                     pme->m_wLastState = pme->m_wState;
                     (void)AppMgr_SetState(pme, ST_MOVE); 
                     break;
                  }
                  if(wParam >= IDC_ORDER_APP){            // Configure Order App list
                     // The user clicked on an item to change its position in the menu

                     // If m_bConfigMenu is set, we were in the process
                     // of moving another item.  Call order() to set the current
                     // order (and turn off m_bConfigMenu
                     if (pme->m_bConfigMenu && pme->m_bDispTouchEnabled)
                     {
                        AppMgr_Order(pme);
                     }
 
                     // Start alowing the user to modify the order of the items
                     AppMgr_Order(pme);

                     break;
                  }
                  if(wParam >= IDC_CONFIGURE_APP){        // Configure Sound/Image list
                     AppMgr_FileSelected(pme, wParam);
                     break;
                  }
                  if (wParam >= IDC_SCREEN_SAVER_APP) {
                     AppMgr_SetScreensaver(pme, wParam);
                     break;
                  }
                  if(wParam >= IDC_LAUNCH_APP){           // App list
                     uint32 dwItem;
                     IMENUCTL_GetItemData(pme->m_pMainMenu, wParam, &dwItem);
                     // Cache Current Menu Selection
                     pme->m_nMenuSel[++pme->m_nIndex] = wParam;
                     pme->m_pCurrentApp = (AppMgrItem*)dwItem;


                     // Ensure that we are not trying to resume, leave as much space
                     // as possible for the app we are about to start
                     pme->m_bResume = FALSE;

                     AppMgr_LaunchCurrentApplet(pme, FALSE);
                     break;
                  }
                  if(wParam >= IDC_APP){                  // App list in options menu
                     uint32 dwItem = 0;

                     IMENUCTL_GetItemData(pme->m_pMenu, wParam, &dwItem);
                     // Cache Current Menu Selection
                     pme->m_nMenuSel[++pme->m_nIndex] = IMENUCTL_GetSel(pme->m_pMenu);
                     pme->m_pCurrentApp = (AppMgrItem*)dwItem;    // Set app as current app
                     (void)AppMgr_SetState(pme, ST_APPINFO);               // Set ST_APPINFO state
                     break;
                  }
               }
               return TRUE;
         }
   }

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
   if(/*TRUE == bRet && */(bSKDoNotUpdate == FALSE) && (bSKUpdate || pme->m_wState != lastState) &&
      !pme->m_bPromptIsDisplayed)
   {
      //AppMgr_AddSoftKey(pme);
   }

#endif //#ifdef FEATURE_TOUCHSCREEN_SUPPORT


   return bRet;
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

      // Do we need to clear the screen if we are on a touch display?

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
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
      pme->m_bPromptIsDisplayed = FALSE;
#endif
   pme->m_bShowBusy = FALSE;
   if (pme->m_wState == ST_SCREENSAVERS)
   {
      AppMgr_SetState(pme, ST_SCREENSAVERS);
   }
   else
   {
      AppMgr_SetState(pme, PREV_ENTRY);
   }
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
   if (pme->m_bConfigMenu && !pme->m_bRedrawingUI)
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
      IMENUCTL_SetTitle(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDS_SELECT_APP, NULL);
      // Store the menu's title for use with flipping the cover
      AppMgr_StoreMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, IDS_SELECT_APP, NULL);
      IMENUCTL_Redraw(pme->m_pMenu);
   }
   else
   {
      if (IMENUCTL_GetItemCount(pme->m_pMenu) > 1)
      {
#ifdef FEATURE_TOUCHSCREEN_SUPPORT
         if (pme->m_bDispTouchEnabled)
         {
            uint16 id[3];
            id[0] = IDS_DOWN;
            id[1] = IDS_UP;
            id[2] = IDC_BACK;
      
            // add 3 buttons to SK menu
            AppMgr_CreateSoftmenu(pme, id, 4);
            AppMgr_SetMenuRect(pme);
         }
#endif //FEATURE_TOUCHSCREEN_SUPPORT

         IMENUCTL_SetTitle(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDS_REORDER_APP, NULL);
         // Store the menu's title for use with flipping the cover
         AppMgr_StoreMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, IDS_REORDER_APP, NULL);

         if (pme->m_bDispTouchEnabled)
         {
            IMENUCTL_SetActive(pme->m_pSK, TRUE);
            IMENUCTL_Redraw(pme->m_pSK);
            IMENUCTL_Redraw(pme->m_pMenu);
         }
         else
         {
            IMENUCTL_Redraw(pme->m_pMenu);
            AppMgr_DrawVisualIndication(pme);
         }

         pme->m_bConfigMenu = TRUE;
      }
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
   IImage* pImage = NULL;
   // Don't display the up/Down indication if we are in touch mode
   // as the user might try to use it. 
   if (pme->m_bDispTouchEnabled)
   {
      return;
   }

   pImage = ISHELL_LoadResImage(pme->a.m_pIShell, BREWAPPMGRLN_RES_FILE, IDB_UPDOWN_ARROW);

   if (pImage)
   {
      AEEImageInfo ii;
      IIMAGE_GetInfo(pImage, &ii);

      // Draw Image
      IIMAGE_Draw(pImage, (pme->m_rc.dx - ii.cx)/2, (pme->m_rc.dy - ii.cy)/2);

      // Free Image
      IIMAGE_Release(pImage);

      // Update Display
      IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
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
   boolean bFlipOpen = TRUE;
   int nSize = sizeof(boolean);

   // Don't handle if this is not the result of AppManager delegating control 
   // to hidden MShop applet
   if (!pme->m_bResume)
   {
      return FALSE;
   }

   // Get the state of the flip, if there is one
   if (SUCCESS == ISHELL_GetDeviceInfoEx(pme->a.m_pIShell, AEE_DEVICESTATE_FLIP_OPEN,
                                         &bFlipOpen, &nSize))
   {
      pme->m_bFlipOpenWhenSuspended = bFlipOpen;
   }

   // Save server specific settings
   ISHELL_SetPrefs(pme->a.m_pIShell, AEECLSID_MOBILESHOP_BID, MSHOP_CFG_VER,
      &pme->m_MShopCfg, sizeof(pme->m_MShopCfg));

   // Reset Controls
   AppMgr_Reset(pme, FALSE);

   // If a Prmpt is displayed, we don't want to display it when we come back
   pme->m_bPromptIsDisplayed = FALSE;

#ifdef USE_TT_FONTS
   // Restore the original fonts before suspending.
   AppMgr_RestoreFonts(pme);
#endif // USE_TT_FONTS

   // Free the Apps Cache
   pme->m_nResumeCls = (pme->m_pCurrentApp ? pme->m_pCurrentApp->cls : 0);
   pme->m_pCurrentApp = NULL;

   // Free Apps Cache
   if (pme->m_bValidCache)
   {
      AppMgr_FreeAppsCache(pme);
   }

   // Free IDownload
   FreeObj((void **)(&pme->m_pDownload));
#ifdef USE_IDL2
   FreeObj((void **)(&pme->m_pDownload2));
#endif


   // Reset Resume Flag
   pme->m_bResume = FALSE;

   // We are suspended
   pme->m_bSuspended = TRUE;
   gbENDKey_RecievedInSuspend = FALSE;

   return TRUE;
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
#ifdef USE_IDL2_4GB_SUPPORT
                  DL2AppModInfo * pai = IDOWNLOAD2_GetModInfoEx(pme->m_pDownload2, *pdw, GMI_ALL);
#else
                  AppModInfo * pai = IDOWNLOAD_GetModInfoEx(pme->m_pDownload, *pdw, GMI_ALL);
#endif

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
//    Determine if phone is on a different dispay from when it was suspended
//    We return TRUE iff the the return bFlipOpen value from the FLIP
//    device states do not match.  If we cannot get the device state, we
//    assume that there is not a flip, and thus we are not on a different
//    screen
// 
// PARAMETERS
//    pme:  AppMgr
//
//=========================================================================== 
static boolean AppMgr_IsDifferentFlipPosition(AppMgr* pme)
{
   boolean bFlipOpen = TRUE;
   int nSize = sizeof(boolean);

   // If multiple display support is not enabled, we are not on a 
   // different display.
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   // Get the state of the flip, if there is one
   if (SUCCESS == ISHELL_GetDeviceInfoEx(pme->a.m_pIShell, AEE_DEVICESTATE_FLIP_OPEN,
                                         &bFlipOpen, &nSize))
   {
      if (pme->m_bFlipOpenWhenSuspended != bFlipOpen)
      {
         return TRUE;
      }
   }
#endif // FEATURE_SUPPORT_MULTIPLE_DISPLAYS

   return FALSE;
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
   // Restore server specific settings
   if (ISHELL_GetPrefs(pme->a.m_pIShell, AEECLSID_MOBILESHOP_BID, MSHOP_CFG_VER, &pme->m_MShopCfg, sizeof(pme->m_MShopCfg)) != SUCCESS)
   {
      pme->m_MShopCfg.bSearch = OEM_ADS_SEARCH_APPS;
      pme->m_MShopCfg.bUpgrade = OEM_ADS_UPGRADE_CHECK;
   }

   // reset the suspended flag
   pme->m_bSuspended = FALSE;

#ifdef USE_TT_FONTS
   // Install TrueType fonts when resuming.
   AppMgr_InstallTTFonts(pme);
#endif // USE_TT_FONTS

   // Create IDownload Instance
#ifdef USE_IDL2
   if (CreateInstance(pme->a.m_pIShell, AEECLSID_DOWNLOAD2, (void**)&pme->m_pDownload2) != AEE_SUCCESS)
   {
      return FALSE;
   }

   if (IDOWNLOAD2_QueryInterface(pme->m_pDownload2, AEECLSID_DOWNLOAD, (void**)&pme->m_pDownload) != AEE_SUCCESS)
   {
      return FALSE;
   }
#else
   if (CreateInstance(pme->a.m_pIShell, AEECLSID_DOWNLOAD, (void**)&pme->m_pDownload) != AEE_SUCCESS)
   {
      return FALSE;
   }
#endif

   // recreate the app list

   // Load Applets Info
   AppMgr_LoadAppletsInfo(pme);

   // now that we have told the Shortcut app any info needed
	// we can now stop processing the resume if we are supose to 
	// exit because we delayed exiting while the user was downloading
	// an app
	if (pme->m_bExitAfterSANotification)
	{
		FARF(BAM, ("exiting AppMgr_Resume() becase we should now exit"));
		return FALSE;
	}

   // Get Current Applet Pointer
   if (pme->m_nResumeCls)
   {
      uint16 nCatID = AppMgr_FindItemCategory(pme, pme->m_nResumeCls,
         &pme->m_pCurrentApp);

      // CR89097.  When resuming in the APPINFO state after an
      // upgrade check that resulted in a license "upsell" only,
      // AppMgr erroneously thinks the app's itemID has changed.
      // Calling IsValidItemIDForManageApps() will ensure that AppMgr's
      // itemID for the upgraded app matches IDownload's.
      if ((0 != nCatID) && (ST_APPINFO == pme->m_wState))
      {
         if (TRUE != IsValidItemIDForManageApps(pme, pme->m_pCurrentApp))
         {
            FARF(BAM, ("IsValidItemForManageApps() returned FALSE."));  
         }
      }

      // If successful and we're in the upgrade auto-prompt we want
      // to go ahead and launch the app when resuming.
      if ((0 != nCatID) && (ST_UPGRADEAUTOPROMPT == pme->m_wState))
      {
         return AppMgr_LaunchCurrentApplet(pme, TRUE);
      }
   }
   
   // Check to see if MobileShop lauched an app
   if (pme->m_clsMShopLaunched)
   {
      // MobileShop launched this app that it just downloaded
      // we need to make note of that.
      uint16 nCatID = AppMgr_FindItemCategory(pme, pme->m_clsMShopLaunched,
                                                &pme->m_pCurrentApp);
      // If successful and we're in the upgrade auto-prompt we want
      // to go ahead and launch the app when resuming.
      if (0 != nCatID)
      {
         pme->m_pCurrentApp->dwNumLaunches++;
         pme->m_pCurrentApp->dwLastLaunch = GETUTCSECONDS();
         AppMgr_WriteAppList(pme);
         // CR 84264  If MobileShop reported an app launch AND a
         // launch error has been reported AND the class IDs match
         // we infer that the MobileShop launch has failed and report
         // the error here on behalf of MobileShop.  See related
         // comments in the handler for the NMASK_SHELL_START_STATUS
         // notifier event.
         if (pme->m_nErrCls == pme->m_clsMShopLaunched)
         {
            if (0 != pme->m_nErr)
            {
               boolean result;
               // The next two lines are necessary to make
               // HandleLaunchError() happy.
               pme->m_bHandledLaunchError = FALSE;
               pme->m_nLaunchCls = pme->m_clsMShopLaunched;
               FARF(BAM, ("BAM reporting launch error for MobileShop."));  
               result =
                  AppMgr_HandleLaunchError(pme, pme->m_nErrCls, pme->m_nErr);
               pme->m_nErrCls = pme->m_nErr = 0;
               return result;
            }
         }
      }
      pme->m_clsMShopLaunched = 0;
   }

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
      {
         AppMgr_StopSoundAnimation(pme);
      }

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

#ifdef MSHOP_STATES
      case IDC_MSHOP_CATALOG:
         pme->m_cfg.bBrowseWarning = FALSE;
         break;

      case IDC_MSHOP_FIND:
         pme->m_cfg.bSearchWarning = FALSE;
         break;
#else
      case IDC_MSHOP:
         pme->m_cfg.bBrowseWarning = FALSE;
         break;
#endif

      case IDC_MSHOP_CREDITBACK:
         pme->m_cfg.bCreditBackWarning = FALSE;
         break;
   }

   // Reset Warning Displayed
   pme->m_bWarningDisplayed = FALSE;

   // Set current state UI
   AppMgr_SetState(pme, pme->m_wState);

   // Post cached event
   return ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID, pme->m_appEvent.evt, pme->m_appEvent.wParam, pme->m_appEvent.dwParam);
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
static boolean AppMgr_RequestMShop(AppMgr* pme, uint16 wParam, boolean bWarning)
{
   // Check for valid current app as we would need to access its members
   switch (wParam)
   {
      case IDC_LAUNCHER_RESTORE:
      case IDC_LAUNCHER_BUY:
      case IDC_LAUNCHER_BUY_NOW:
      case IDC_UPGRADE_CHECK:
      case IDC_REMOVE:
      case IDC_UPGRADEAUTOPROMPT_PURCHASE:
         if (!pme->m_pCurrentApp)
         {
            FARF(BAM, ("Missing needed current app in RequestMShop()"));  
            return FALSE;
         }
         break;
      default:
         break;
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

#ifdef MSHOP_STATES
         case IDC_MSHOP_CATALOG:
            wWarningID = IDS_BROWSE_AIRTIME_WARNING;
            break;

         case IDC_MSHOP_FIND:
            wWarningID = IDS_SEARCH_AIRTIME_WARNING;
            break;
#else
         case IDC_MSHOP:
            wWarningID = IDS_BROWSE_AIRTIME_WARNING;
            break;
#endif

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

      MEMSET(pBuffer, 0, MAX_STR_SIZE);

      if (bWarning) 
      {
         pme->m_bWarningDisplayed = FALSE;   // Reset warning displayed flag
      }

      if (pBuffer)
      {
         DLITEMID dw = 0;
         switch (wParam)
         {
            case IDC_LAUNCHER_RESTORE:
               if ((dw = AppMgr_GetItemID(pme, pme->m_pCurrentApp)) != 0)
               {
                  SPRINTF(pBuffer, "%s%c%s%c%u", CMSHOP_CMD_BASE,
                     CMSHOP_CMD_COLON, CMSHOP_CMD_RESTORE, CMSHOP_CMD_EQUAL,dw);
               }
               break;

            case IDC_LAUNCHER_BUY:
            case IDC_LAUNCHER_BUY_NOW:
               if ((dw = AppMgr_GetItemID(pme, pme->m_pCurrentApp)) != 0)
               {
                  SPRINTF(pBuffer, "%s%c%s%c%u", CMSHOP_CMD_BASE,
                     CMSHOP_CMD_COLON, CMSHOP_CMD_ITEMID, CMSHOP_CMD_EQUAL, dw);
               }
               break;
                                          
            case IDC_LAUNCHER_VERIFY:
               if ((dw = AppMgr_GetItemID(pme, pme->m_pCurrentApp)) != 0)
               {
                  SPRINTF(pBuffer, "%s%c%s%c%u", CMSHOP_CMD_BASE,
                     CMSHOP_CMD_COLON, CMSHOP_CMD_VERIFY, CMSHOP_CMD_EQUAL, dw);
               }
               break;

            case IDC_UPGRADE_CHECK:
               pme->m_bResume = TRUE;
               if ((dw = AppMgr_GetItemID(pme, pme->m_pCurrentApp)) != 0)
               {
                  SPRINTF(pBuffer, "%s%c%s%c%u", CMSHOP_CMD_BASE,
                     CMSHOP_CMD_COLON, CMSHOP_CMD_UPGRADECHECK,
                     CMSHOP_CMD_EQUAL, dw);
               }
               break;
            case IDC_UPGRADEAUTOPROMPT_PURCHASE:
               pme->m_bResume = TRUE;
               dw = pme->m_pCurrentApp->dwUpgradeID;
               if (0 != dw)
               {
                  SPRINTF(pBuffer, "%s%c%s%c%u", CMSHOP_CMD_BASE,
                     CMSHOP_CMD_COLON, CMSHOP_CMD_ITEMID, CMSHOP_CMD_EQUAL, dw);
               }
               else
               {
                  FARF(BAM, ("Upgrade item ID missing"));  
               }
               break;

            case IDC_REMOVE:
               pme->m_bResume = TRUE;                  
               if ((dw = AppMgr_GetItemID(pme, pme->m_pCurrentApp)) != 0)
               {
                  SPRINTF(pBuffer, "%s%c%s%c%u%", CMSHOP_CMD_BASE,
                     CMSHOP_CMD_COLON, CMSHOP_CMD_REMOVE, CMSHOP_CMD_EQUAL, dw); 
               }    
               if (pme->m_bFreeCurrentApp)
               {
                  // If active screen saver is being removed, reset active
                  // screen saver
                  if (pme->m_pCurrentApp->cls ==
                     ISHELL_GetHandler(pme->a.m_pIShell, AEECLSID_SCREEN_SAVER,
                        MT_SCREEN_SAVER))
                  {
                     ISHELL_RegisterHandler(pme->a.m_pIShell,
                        AEECLSID_SCREEN_SAVER, MT_SCREEN_SAVER, 0);
                  }
                  FreePtr((void**)&pme->m_pCurrentApp);
                  pme->m_bFreeCurrentApp = FALSE;
               }
               break;

#ifdef MSHOP_STATES
            case IDC_MSHOP_CATALOG:
               pme->m_bResume = TRUE;
               SPRINTF(pBuffer, "%s%c%s", CMSHOP_CMD_BASE, CMSHOP_CMD_COLON, 
                       CMSHOP_CMD_CATALOG);
               break;
            case IDC_MSHOP_FIND:
               pme->m_bResume = TRUE;
               SPRINTF(pBuffer, "%s%c%s", CMSHOP_CMD_BASE, CMSHOP_CMD_COLON, 
                       CMSHOP_CMD_SEARCH);
               break;
#else
            case IDC_MSHOP:
               pme->m_bResume = TRUE;
               SPRINTF(pBuffer, "%s%c%s", CMSHOP_CMD_BASE, CMSHOP_CMD_COLON, 
                       CMSHOP_CMD_CATALOG);
               break;
#endif

            case IDC_MSHOP_CREDITBACK:
               pme->m_bResume = TRUE;
               SPRINTF(pBuffer, "%s%c%s", CMSHOP_CMD_BASE, CMSHOP_CMD_COLON, 
                       CMSHOP_CMD_CREDITBACK);
               break;
         }
         if (*pBuffer)
         {
            // Add secret string to end of pBuffer and browse with it:
#ifdef CMSHOP_SECURE_ALL
            if (!AppendSecretString(pme->a.m_pIShell, pBuffer)) {
               FreePtr((void**)&pBuffer);
               return FALSE;
            }
#elif defined CMSHOP_SECURE_REMOVE
            if (wParam == IDC_REMOVE &&
               !AppendSecretString(pme->a.m_pIShell, pBuffer))
            {
               FreePtr((void**)&pBuffer);
               return FALSE;
            }
#endif //CMSHOP_SECURE_ALL
            ISHELL_BrowseURL(pme->a.m_pIShell, pBuffer);
         }
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
boolean AppMgr_Start(AppMgr* pme)
{ 
   boolean bCanRun = TRUE; 

   FARF(BAM, ("In AppMgr_Start"));
   AppMgr_LoadAppletsInfo(pme); 

   // After the OEM config stuff is loaded check to see if we can
   // run on the current display.  If not, refuse to start.
   bCanRun = DisplayEnabled(pme->a.m_pIShell, pme->m_bHasMultipleDisplays,
      pme->m_wEnabledDisplays, APPMGR_CURRENT_DISPLAY);
   if (FALSE == bCanRun)
   {
      FARF(BAM, ("Can't start on this display"));
      return FALSE;
   }
   else
   {
      FARF(BAM, ("Able to start on this display"));
   }	 
   
#if defined(FEATURE_BREW_DOWNLOAD) // Gemsea move here from AppMgr_Init
#ifdef USE_IDL2
   if (CreateInstance(pme->a.m_pIShell, AEECLSID_DOWNLOAD2,
      (void**)&pme->m_pDownload2) != AEE_SUCCESS)
   {
      return FALSE;
   }

   if (IDOWNLOAD2_QueryInterface(pme->m_pDownload2, AEECLSID_DOWNLOAD, 
                                 (void**)&pme->m_pDownload) != AEE_SUCCESS)
   {
      return FALSE;
   }
#else
   if (CreateInstance(pme->a.m_pIShell, AEECLSID_DOWNLOAD,
      (void**)&pme->m_pDownload) != AEE_SUCCESS)
   {
      return FALSE;
   }
#endif //USE_IDL2
#endif // FEATURE_BREW_DOWNLOAD

   // Don't paint UI if AppMgr is started to display error
   if (!pme->m_nErrCls)
   {
      //AppMgr_SetState(pme, pme->m_wState);
      // If we have a parameter and it is not "none", try to auto start
      if (pme->m_pAutoStart && STRICMP("none", pme->m_pAutoStart))
      {
         pme->m_wState = ST_AUTOSTART;
         AppMgr_AutoStartApp(pme);
      }
      else
      {
         pme->m_wState = ST_MAINMENU;
         AppMgr_SetState(pme, pme->m_wState);
      }
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
   return TRUE;
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

#if defined(FEATURE_UPGRADE_AUTO_PROMPT)
   IUpgradeMgr* pUm = NULL;
#endif // FEATURE_UPGRADE_AUTO_PROMPT

   // Add items in general, favorites, work and fun config files to 
   // their respective applet lists
   AppMgr_ReadConfigFile(pme);

   // Enumerate applets and add any that were not in the file.
   AppMgr_ReadApplets(pme);

   if (pme->m_bMShopChangedNumApps)
   {
#ifdef FEATURE_INTERACT_WITH_SA_APP
      // Write The list of Apps to the AppList file so that the shortcut
      // app can use it
      int nErr = AEE_SUCCESS;
      AppMgr_WriteAppList(pme);
      if (pme->m_szSMSAppUrl)
      {
         nErr = ISHELL_PostURL(pme->a.m_pIShell, pme->m_szSMSAppUrl);
         FARF(BAM, ("PostUrl(%s) returned %d", pme->m_szSMSAppUrl, nErr));
      }
#endif
      pme->m_bMShopChangedNumApps = FALSE;
   }

#if defined(FEATURE_UPGRADE_AUTO_PROMPT)
   // Instantiate the UpgradeMgr if the upgrade auto-prompt feature
   // is enabled.  This instantiation is the one that tries to connect
   // to the ADS for the latest/greatest upgrade info.
   if (AEE_SUCCESS != CreateInstance(pme->a.m_pIShell,
      AEECLSID_UPGRADEMGR, (void**)&(pme->m_pUm)))
   {
      FARF(BAM, ("Error creating UpgradeMgr instance") );
   }
   pUm = pme->m_pUm;
   // Set the UpgradeMgr's update interval, then tell it update.
   IUM_SetUpdateInterval(pUm, DB_UPDATE_TIMER_INTERVAL);
   IUM_Update(pUm);
#endif // FEATURE_UPGRADE_AUTO_PROMPT

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
      nSel = 0;
   }

   // Set Menu Title
   IMENUCTL_SetTitle(pme->m_pMainMenu, BREWAPPMGRLS_RES_LOC, (uint16)(IDC_BREW_APPS + nCatIndex), NULL);
   // Store the main menu's title for use with flipping the cover
   AppMgr_StoreMainMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, (uint16)(IDC_BREW_APPS + nCatIndex), NULL);

   // Draw Main Menu
   while (nCount)
   {
      AppMgr_AddMenuItem(pme, pme->m_pMainMenu, pi, itemID, nType, FALSE);

      if (pme->m_pMainMenuSK)
      {
         //AppMgr_AddMenuItem(pme, pme->m_pMainMenuSK, pi, nCount, THUMB, TRUE);
         AppMgr_AddMenuItem(pme, pme->m_pMainMenuSK, pi, itemID, THUMB, TRUE);
      }

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
            }

            // Update Display
            IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
         }
         else
            nDraw--;
      }
   }

   if (!pme->m_cfg.bListView)
   {
      CtlAddItem        ai;
      // Add the MainMenu icon so that the user can get back to the main menu
      // this is necessary as there is no nice place for a back button in SmallIconView
      MEMSET(&ai, 0, sizeof(ai));
      ai.pszResText  = BREWAPPMGRLS_RES_LOC;
      ai.pszResImage = BREWAPPMGRLN_RES_FILE;
      ai.wText       = IDS_MAIN_MENU;
      ai.wImage      = ((nType == THUMB) ? IDB_MAINMENU_THUMB : ((nType == ICON) ? IDB_MAINMENU_ICON : IDB_MAINMENU_LARGE));
      ai.wItemID     = IDC_BACK;
      IMENUCTL_AddItemEx(pme->m_pMainMenu, &ai);

      if (pme->m_pMainMenuSK)
      {
         ai.wText       = NULL;
         ai.wImage      = IDB_MAINMENU_THUMB;
         IMENUCTL_AddItemEx(pme->m_pMainMenuSK, &ai);
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
      }

      // Update UI
      IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

      if (pme->m_bDispTouchEnabled /*&& !pme->m_bHasDPad*/)
      {
         IMENUCTL_SetActive(pme->m_pMainMenu, FALSE);
      }
   }

   // Set Last State to ST_MAINMENU
   pme->m_wLastState = ST_MAINMENU;

   return TRUE;
}

#ifdef MSHOP_STATES

//===========================================================================
//
// DESCRIPTION
//    Function for determing Menu size when in state ST_MSHOP
// 
// PARAMETERS
//    pme:  AppMgr
//    prc:  AEERect*
//
//===========================================================================
static boolean AppMgr_GetMenuRectForMShop(AppMgr* pme, AEERect* prc)
{
   if ((NULL == pme) || (NULL == prc))
   {
      return FALSE;
   }

   // Set Menu Size
   SETAEERECT(prc, 0, pme->m_rcMShopLogo.dy, pme->m_rc.dx, pme->m_rc.dy - pme->m_rcMShopLogo.dy);

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
      pme->m_pMShopLogo = ISHELL_LoadResImage(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, IDB_MSHOP_LOGO);

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
      if (AppMgr_GetMenuRectForMShop(pme, &rc))
      {
         //SETAEERECT(&rc, 0, pme->m_rcMShopLogo.dy, pme->m_rc.dx, pme->m_rc.dy - pme->m_rcMShopLogo.dy);
         IMENUCTL_SetRect(pme->m_pMenu, &rc);
      }
   }
   else
   {
      IMENUCTL_SetTitle(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_MSHOP, NULL);
      // Store the menu's title for use with flipping the cover
      AppMgr_StoreMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, IDC_MSHOP, NULL);
   }

   // Add Items
   MEMSET(&ai, 0, sizeof(ai));
   ai.pszResText  = BREWAPPMGRLS_RES_LOC;
   ai.pszResImage = BREWAPPMGRLN_RES_FILE;

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
   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

   return(TRUE);
}
#endif
      
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
   IMENUCTL_SetTitle(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_OPTIONS, NULL);
   // Store the menu's title for use with flipping the cover
   AppMgr_StoreMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, IDC_OPTIONS, NULL);

   // Add Menu Items
   MEMSET(&ai, 0, sizeof(ai));
   ai.pszResImage = BREWAPPMGRLN_RES_FILE;
   ai.pszResText  = BREWAPPMGRLS_RES_LOC;
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

   IMENUCTL_SetActive(pme->m_pMenu, TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

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
   IMENUCTL_SetTitle(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_HELP, NULL);
   // Store the menu's title for use with flipping the cover
   AppMgr_StoreMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, IDC_HELP, NULL);

   // add menu items
   IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_HELP_BUYING_NEW_APPS,
      IDC_HELP_BUYING_NEW_APPS, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_HELP_WHAT_DO_I_PAY,
      IDC_HELP_WHAT_DO_I_PAY, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_HELP_AIRTIME_CHARGES,
      IDC_HELP_AIRTIME_CHARGES, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC,
      IDC_HELP_CANCELING_SUBSCRIPTION, IDC_HELP_CANCELING_SUBSCRIPTION,
      NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_HELP_DEMO_APPS,
      IDC_HELP_DEMO_APPS, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_HELP_REMOVING_APPS,
      IDC_HELP_REMOVING_APPS, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_HELP_DISABLED_APPS,
      IDC_HELP_DISABLED_APPS, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_HELP_RESTORING_APPS,
      IDC_HELP_RESTORING_APPS, NULL, NULL);
   IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_HELP_EXPIRED_APPS,
      IDC_HELP_EXPIRED_APPS, NULL, NULL);

   if (pme->m_nIndex > 0)
   {
      IMENUCTL_SetSel(pme->m_pMenu, pme->m_nMenuSel[pme->m_nIndex--]);
   }
               
   // Draw Help Menu
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

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

   FARF(BAM, ("In AppMgr_MainMenu"));

   // Let's not set selection for small icon view and list view so that we can make menu
   // active sooner without worrying about selection
   if (pme->m_nIndex > -1)
   {
      nSel = pme->m_nMenuSel[pme->m_nIndex--];
      nSel = 0;
   }
   
   // Set Menu Title
   IMENUCTL_SetTitle(pme->m_pMainMenu, BREWAPPMGRLS_RES_LOC, IDS_MAIN_MENU, NULL);
   // Store the main menu's title for use with flipping the cover
   AppMgr_StoreMainMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, IDS_MAIN_MENU, NULL);

   // Add the menu items.
   MEMSET(&ai, 0, sizeof(ai));

   ai.pszResText  = BREWAPPMGRLS_RES_LOC;
   ai.pszResImage = BREWAPPMGRLN_RES_FILE;

#if defined(FEATURE_BREW_DOWNLOAD)
   if (OEM_LOCK_MOBILESHOP_LOCATION)
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
      nDraw--;
   }
#endif // FEATURE_BREW_DOWNLOAD

   // Show root level apps in
   if (pme->m_nItem[0])
   {
      // Draw List View
      AppMgrItem * pi = pme->m_pList[0];
      uint16 itemID = IDC_LAUNCH_APP;
      uint16 nCount = pme->m_nItem[0];
      
      while (nCount)
      {
         uint16 nItemID = (!OEM_LOCK_MOBILESHOP_LOCATION && pi->cls == AEECLSID_MOBILESHOP_BID) ? IDC_MSHOP : itemID;

         AppMgr_AddMenuItem(pme, pme->m_pMainMenu, pi, nItemID,  nType, FALSE);

         if (pme->m_pMainMenuSK)
         {
            //AppMgr_AddMenuItem(pme, pme->m_pMainMenuSK, pi, nCount,  THUMB, TRUE);
            AppMgr_AddMenuItem(pme, pme->m_pMainMenuSK, pi, nItemID,  THUMB, TRUE);
         }

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
               }

               if (pme->m_bDispTouchEnabled && pme->m_cfg.bListView/* && !pme->m_bHasDPad*/)
               {
                  IMENUCTL_Redraw(pme->m_pMainMenu);
                  IMENUCTL_SetActive(pme->m_pMainMenu, FALSE);
               }

               // Update Display
               IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);               
            }
            else
               nDraw--;
         }
      }
   }

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
         //ai.wItemID     = pme->m_nItem[0] + IDC_FAVORITES;
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
         //ai.wItemID     = pme->m_nItem[0] + IDC_WORK;
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
         //ai.wItemID     = pme->m_nItem[0] + IDC_FUN;
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
      //ai.wItemID     = pme->m_nItem[0] + IDC_OPTIONS;
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
      //ai.wItemID     = pme->m_nItem[0] + IDC_HELP;
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
      }

      if (pme->m_bDispTouchEnabled /* && !pme->m_bHasDPad*/)
      {
         IMENUCTL_Redraw(pme->m_pMainMenu);
         IMENUCTL_SetActive(pme->m_pMainMenu, FALSE);
         IMENUCTL_Redraw(pme->m_pMainMenu);
      }

      // Update Display
      IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);      
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
   AEEAppInfo     ai;
#ifdef USE_IDL2_DISABLED_SUPPORT
	DLITEMID* disItemIDPtr = NULL;
#endif


#if defined(FEATURE_BREW_DOWNLOAD)
   if (!OEM_LOCK_MOBILESHOP_LOCATION)
   {
      AppMgr_GetAEEAppInfo(pme, AEECLSID_MOBILESHOP_BID, &ai);

      // Add item to general category if its not present in any category
      if (!AppMgr_FindItemCategory(pme, ai.cls, NULL))
      {
         AppMgr_AddItem(pme, IDC_BREW_APPS, ai.cls, 0, 0xffffffff, 0, 0,
            (AECHAR*)"", "", "", FALSE);
      }
   }
#endif // FEATURE_BREW_DOWNLOAD

   // Enumerate all applets and add new in general list
   ISHELL_EnumAppletInit(pme->a.m_pIShell);
   while(ISHELL_EnumNextApplet(pme->a.m_pIShell, &ai)) {

      // Don't add hidden apps and screen savers
      if ((ai.wFlags & AFLAG_HIDDEN) || (ai.wFlags & AFLAG_SCREENSAVER))
         continue;

      // Do not add self

      // We can be running when we are not the active applet (for example
      // if we are handling the PostURL() SMS to crate the Applist
      // We are running, but are not the active appliet
      //if (ai.cls == ISHELL_ActiveApplet(pIShell))
      if (ai.cls == AEECLSID_BREWAPPMGR_BID)
      {
         continue;
      }

      // Guard against ISHELL_EnumNextApplet() giving us info about a
      // deleted application
      if (FALSE == AppMgr_GetAEEAppInfo(pme, ai.cls, &ai))
      {
         continue;
      }

      // Add item to general category if its not present in any category
      if (!AppMgr_FindItemCategory(pme, ai.cls, NULL))
      {
         AppMgr_AddItem(pme, IDC_BREW_APPS, ai.cls, 0, 0xffffffff, 0, 0,
            (AECHAR*)"", "", "", ADD_ITEM_REVERSE_ORDER);
      }
   }

#ifdef USE_IDL2_DISABLED_SUPPORT
   // Enumerate disabled applets and add new in general list
   disItemIDPtr = IDOWNLOAD2_GetDisabledApps(pme->m_pDownload2);
   if (NULL != disItemIDPtr)
   {
      DLITEMID disItemID = *disItemIDPtr;
      while (0 != disItemID)
      {
         AEECLSID clsID = AppMgr_GetClassIDForDLItem(pme, disItemID);
         if (0 != clsID)
         {
            // Add item to general category if not present in any category
            if (!AppMgr_FindItemCategory(pme, clsID, NULL))
            {
               AppMgr_AddItem(pme, IDC_BREW_APPS, clsID, 0, 0xffffffff, 0, 0,
                  (AECHAR*)"", "", "", ADD_ITEM_REVERSE_ORDER);
            }
         }
         else
         {
            FARF(BAM, ("Failed to convert item ID to class ID"));
         }

         disItemIDPtr++;
         disItemID = *disItemIDPtr;
      }
   }
#endif // USE_IDL2_DISABLED_SUPPORT

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
               else if (!STRNCMP(fileBuf, CONFIG_FILE_VER_3, STRLEN(CONFIG_FILE_VER_3)))
               {
                  nErr = AppMgr_ParseConfigFile(pme, (char*)fileBuf + STRLEN(CONFIG_FILE_VER_3) + 1, 2);
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
         pmi->dwUpgradeID = 0;
         pmi->nOffset = 0xffffffff;
         pmi->pNext = pNext;
         pmi->pPrev = pPrev;
      }
      else
      {
         // Don't add hidden apps, screen savers and self 
         if (AppMgr_GetAEEAppInfo(pme, nCls, &ai) &&
             !(ai.wFlags & AFLAG_HIDDEN) && 
             !(ai.wFlags & AFLAG_SCREENSAVER) && 
              (ai.cls != ISHELL_ActiveApplet(pme->a.m_pIShell)))
         {
            AppMgr_AddItem(pme, IDC_BREW_APPS, nCls, nDLItemID, 0xffffffff, 0,
               0, (AECHAR*)"", "", "", ADD_ITEM_REVERSE_ORDER);
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
   uint32   nCls, nDLItemID, nOffset, nSize, nAutoPromptTimer;
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

      //*** Extract Upgrade Auto-Prompt Timer ***
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
      nAutoPromptTimer = STRTOUL(szCls, NULL, 16);

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

      if (AppMgr_GetAEEAppInfo(pme, nCls, &ai))
      {
#if defined(FEATURE_BREW_DOWNLOAD)
         if (nCls == AEECLSID_MOBILESHOP_BID)
            AppMgr_AddItem(pme, (uint16)(index+IDC_BREW_APPS), (AEECLSID) nCls,
               (DLITEMID) nDLItemID, nOffset, nSize, nAutoPromptTimer,
               wszName, szIcon, szSound, FALSE);
#endif // FEATURE_BREW_DOWNLOAD         
         // Don't add hidden apps, screen savers and self
         if ((nCls != AEECLSID_MOBILESHOP_BID) &&
             !(ai.wFlags & AFLAG_HIDDEN) && 
             !(ai.wFlags & AFLAG_SCREENSAVER) && 
              (ai.cls != ISHELL_ActiveApplet(pme->a.m_pIShell)))
         {
            AppMgr_AddItem(pme, (uint16)(index+IDC_BREW_APPS), (AEECLSID) nCls,
               (DLITEMID) nDLItemID, nOffset, nSize, nAutoPromptTimer,
               wszName, szIcon, szSound, FALSE);
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
   uint32         nNeededStrLen = 0;
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
      nSize += ((5*CLSID_LENGTH + 2*MAX_FILE_NAME + 2*MAX_APP_NAME + 3)*pme->m_nItem[index]);
      nSize++; // For NEW_LINE
   }

   nSize++; // For NULL

   nSize += (STRLEN(CONFIG_FILE_VER_3) + 1); // For version string and space char

   nSize += CLSID_LENGTH; // For launched class ID and space char

   nSize = ((nSize + 3)/4)*4; // Make nSize a 4 multiple

   pBuffer = MALLOC(nSize);

   if (pBuffer)
   {
      IFILE_Truncate(pIFile, 0); // reset the file
      
      nNeededStrLen = SNPRINTF(pBuffer + nBuffer, nSize - nBuffer, "%s ", CONFIG_FILE_VER_3);
      if (nNeededStrLen + 1 > (nSize - nBuffer)) {
         // Buffer too small.
      }
      if (nNeededStrLen != STRLEN(pBuffer + nBuffer)) {
         // Internal error.
      }
      nBuffer += STRLEN(pBuffer + nBuffer);

      nNeededStrLen = SNPRINTF(pBuffer + nBuffer, nSize - nBuffer, "0x%lx ", pme->m_nLaunchCls);
      if (nNeededStrLen + 1 > (nSize - nBuffer)) {
         // Buffer too small.
      }
      if (nNeededStrLen != STRLEN(pBuffer + nBuffer)) {
         // Internal error.
      }
      nBuffer += STRLEN(pBuffer + nBuffer);

      for (index = 0; index < MAX_APP_CATS; index++)
      {
         nCount = pme->m_nItem[index];
         pList = pme->m_pList[index];
            
         for(i = 0; i < nCount; i++) {
            nNeededStrLen = SNPRINTF(pBuffer + nBuffer, nSize - nBuffer,
               "0x%lx,0x%lx,0x%lx,0x%lx,0x%lx,%s,%s,", pList->cls,
               pList->dwItemID, pList->nOffset, pList->nSize,
               pList->dwAutoPromptTimer, pList->szIcon, pList->szSound);
            if (nNeededStrLen + 1 > (nSize - nBuffer)) {
               // Buffer too small.
            }
            if (nNeededStrLen != STRLEN(pBuffer + nBuffer)) {
               // Internal error.
            }
            nBuffer += STRLEN(pBuffer + nBuffer);
            if (2*(WSTRLEN(pList->wszName) + 1) > sizeof(pList->wszName)) {
               // Name overflows name array.
            }
            if (2*(uint32)WSTRLEN(pList->wszName) > (nSize - nBuffer)) {
               // Buffer too small.
            }
            MEMCPY(pBuffer + nBuffer, pList->wszName, 2*WSTRLEN(pList->wszName));
            nBuffer += 2*WSTRLEN(pList->wszName);
            nNeededStrLen = SNPRINTF(pBuffer + nBuffer, nSize - nBuffer, ";");
            if (nNeededStrLen + 1 > (nSize - nBuffer)) {
               // Buffer too small.
            }
            if (nNeededStrLen != 1) {
               // Internal error.
            }
            nBuffer++;          
            pList = pList->pNext;
         }
         nNeededStrLen = SNPRINTF(pBuffer + nBuffer, nSize - nBuffer, "%s", "\n");
         if (nNeededStrLen + 1 > (nSize - nBuffer)) {
            // Buffer too small.
         }
         if (nNeededStrLen != STRLEN("\n")) {
            // Internal error.
         }
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
//		Function that actually starts the auto start app
// 
// PARAMETERS
//		pme:	AppMgr
//
//===========================================================================
void AppMgr_AutoStartApp(void * po)
{
   AppMgr * pme = (AppMgr *)po;
   char * pApp, * pAppArg;
   
   pApp = STRCHR(pme->m_pAutoStart, ';');

   if (pApp)
      *pApp = 0;

   pAppArg = STRCHR(pme->m_pAutoStart, ':');
   
   if (pAppArg)
   {
      *pAppArg = 0;
      pAppArg++;
   }

   // This is necessary because if we don't handle the suspend when BAM is suspended
   // for this app starting, we will be re-started with the same parmeters
   // and we will end up in an infinate loop of starting the application that
   // is being started here.
   pme->m_bResume = TRUE;

   if (pAppArg)
      ISHELL_StartAppletArgs(pme->a.m_pIShell, ATOI(pme->m_pAutoStart), pAppArg);
   else
      ISHELL_StartApplet(pme->a.m_pIShell, ATOI(pme->m_pAutoStart));

   if (pApp)
      pme->m_pAutoStart = ++pApp;
   else
      pme->m_pAutoStart = NULL;

   if (!pme->m_pAutoStart || !*pme->m_pAutoStart)
      pme->m_wState = ST_MAINMENU;
}

//===========================================================================
//
// DESCRIPTION
//		State Handler for ST_AUTOSTART
// 
// PARAMETERS
//		pme:	AppMgr
//
//===========================================================================
static boolean AppMgr_AutoStart(AppMgr* pme)
{
   CALLBACK_Init(&pme->m_cba,AppMgr_AutoStartApp,pme);
   ISHELL_Resume(pme->a.m_pIShell,&pme->m_cba);
   return(TRUE);
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
static int AppMgr_AddItem(AppMgr * pme, uint16 nCategory, AEECLSID cls,
      uint32 dwItemID, uint32 nOffset, uint32 nSize, uint32 nAutoPromptTimer,
      AECHAR * pName, char* pszIcon, char* pszSound, boolean atBeginning)
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
      pNewItem->dwUpgradeID = 0;
      pNewItem->nOffset = nOffset;
      pNewItem->nSize = nSize;
      pNewItem->dwAutoPromptTimer = nAutoPromptTimer;
#ifdef FEATURE_INTERACT_WITH_SA_APP
      {
         // If we are unable to get the properties for this app set to defaults
         if (!GetAppProperties(pme->m_pIfm, cls, &(pNewItem->bHasShortcut), &(pNewItem->dwNumLaunches), &(pNewItem->dwLastLaunch)))
         {
            pNewItem->bHasShortcut = FALSE;
            pNewItem->dwNumLaunches = 0;
            pNewItem->dwLastLaunch = 0;
         }
      }
#endif //FEATURE_INTERACT_WITH_SA_APP
      if(pszIcon)
         STRCPY(pNewItem->szIcon, pszIcon);
      if(pszSound)
         STRCPY(pNewItem->szSound, pszSound);

      if (atBeginning)
      {
         // Put at beginning of list
         p = *pList; // former first item
         *pList = pNewItem;
         pNewItem->pNext = p;
         if (p)
            p->pPrev = pNewItem;
      }
      else
      {
         pNewItem->pPrev = p;

         if (p)
            p->pNext = pNewItem;
         else
            *pList = pNewItem; // First Item
      }

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
         {
            return (i+IDC_BREW_APPS);
         }
         pElem = pElem->pNext;
      }
   }

   // Item not found.
   return 0;
}





#ifdef FEATURE_OEMCFG

//===========================================================================
//
// DESCRIPTION
//    Read the OEM configuration info from the oemcfg.txt file.  This file contains
//    a number of name/value pairs that specify OEM specific behaviors.
//
// PARAMETERS
//    pme:     AppMgr
//
// Return Value
//    [TRUE|FALSE]
//
//===========================================================================
static boolean AppMgr_LoadOEMCfg(AppMgr *pme)
{
   char*    pszName;
   char*    pszValue;
   OEMCfg   *pOEMCfg = NULL;

   pOEMCfg = MALLOC(sizeof(OEMCfg));
   if (pOEMCfg == NULL)
      return FALSE;
   pOEMCfg->m_pCfgFileBuf = NULL;
   pOEMCfg->m_iCfgFileBufPos = 0;
   pOEMCfg->m_nCfgBytes = 0;
   //Look in root directory
   if((pOEMCfg->m_pIFile = IFILEMGR_OpenFile(pme->m_pIfm, OEMCFG_FILE, _OFM_READ)) == NULL)
   {
      // if the file doesn't exist, that's fine, we just use default values.  Other errors
      // indicate failure.
      FARF(BAM, ("Unable to open file %s", OEMCFG_FILE));
      FreePtr((void **) &pOEMCfg);
      return (IFILEMGR_GetLastError(pme->m_pIfm) == EFILENOEXISTS);
   }
   while (OEMCfg_NextNVPair(pOEMCfg, &pszName, &pszValue))
   {
      FARF(BAM, ("Found config pair '%s', '%s'", pszName, pszValue));
#ifdef FEATURE_CANCELONCLOSE
      if (!STRICMP(pszName, OEMCFG_CANCELONCLOSE))
      {
         if (!STRICMP(pszValue, "false") || !STRICMP(pszValue, "0"))
            pme->m_bCancelOnClose = FALSE;

         FARF(BAM, ("Our CancelOnClose value is %d", pme->m_bCancelOnClose));
      }
#endif

#ifdef FEATURE_INTERACT_WITH_SA_APP
      if (!STRICMP(pszName, OEMCFG_SA_APP_SMS_URL))
      {
         FreePtr((void**)&pme->m_szSMSAppUrl); // ensure that we free any old data
         pme->m_szSMSAppUrl = STRDUP(pszValue);
         FARF(BAM, ("Our SMSAppUrl value is %s", pme->m_szSMSAppUrl));
      }
#endif //FEATURE_INTERACT_WITH_SA_APP

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
      if (!STRICMP(pszName, OEMCFG_TOUCH_DISPLAYS))
      {
         pme->m_wTouchDisplays = (uint16)ATOI(pszValue);
         FARF(BAM, ("Our %s value is %d", OEMCFG_TOUCH_DISPLAYS, pme->m_wTouchDisplays));
      }
#endif

#ifdef FEATURE_HAPTIC_SUPPORT
      if (!STRICMP(pszName, OEMCFG_HAPTIC_RESPONSE_TONE))
      {
         pme->m_ToneData.eTone = (uint16)ATOI(pszValue);
         FARF(BAM, ("Our %s value is %d", OEMCFG_HAPTIC_RESPONSE_TONE, pme->m_ToneData.eTone));
      }
      if (!STRICMP(pszName, OEMCFG_HAPTIC_RESPONSE_DURATION))
      {
         pme->m_ToneData.wDuration = (uint16)ATOI(pszValue);
         FARF(BAM, ("Our %s value is %d", OEMCFG_HAPTIC_RESPONSE_DURATION, pme->m_ToneData.wDuration));
      }
#endif

#ifdef FEATURE_HAS_MULTIPLE_DISPLAYS
      if (!STRICMP(pszName, OEMCFG_BAM_DISPLAYS))
      {
         pme->m_wEnabledDisplays = (uint16)ATOI(pszValue);
         FARF(BAM, ("Our %s value is %d", OEMCFG_BAM_DISPLAYS, pme->m_wEnabledDisplays));
      }
#endif

#ifdef FEATURE_LESS_THAN_6_KEYS
      if (!STRICMP(pszName, OEMCFG_BAM_DISPLAY1_KEY_MAP))
      {
         GetKeyPressValues(pszValue, STRLEN(pszValue), &pme->m_wAltKeyUpList[0],
            &pme->m_wAltKeyDownList[0], &pme->m_wAltKeySelectList[0], &pme->m_wAltKeyClrList[0]);

         FARF(BAM, ("Our Display1 key mappings are %d, %d, %d, %d", pme->m_wAltKeyUpList[0],
            pme->m_wAltKeyDownList[0], pme->m_wAltKeySelectList[0], pme->m_wAltKeyClrList[0]));
      }
      if (!STRICMP(pszName, OEMCFG_BAM_DISPLAY2_KEY_MAP))
      {
         GetKeyPressValues(pszValue, STRLEN(pszValue), &pme->m_wAltKeyUpList[1],
            &pme->m_wAltKeyDownList[1], &pme->m_wAltKeySelectList[1], &pme->m_wAltKeyClrList[1]);

         FARF(BAM, ("Our Display2 key mappings are %d, %d, %d, %d", pme->m_wAltKeyUpList[1],
            pme->m_wAltKeyDownList[1], pme->m_wAltKeySelectList[1], pme->m_wAltKeyClrList[1]));
      }
      if (!STRICMP(pszName, OEMCFG_BAM_DISPLAY3_KEY_MAP))
      {
         GetKeyPressValues(pszValue, STRLEN(pszValue), &pme->m_wAltKeyUpList[2],
            &pme->m_wAltKeyDownList[2], &pme->m_wAltKeySelectList[2], &pme->m_wAltKeyClrList[2]);

         FARF(BAM, ("Our Display3 key mappings are %d, %d, %d, %d", pme->m_wAltKeyUpList[2],
            pme->m_wAltKeyDownList[2], pme->m_wAltKeySelectList[2], pme->m_wAltKeyClrList[2]));
      }
      if (!STRICMP(pszName, OEMCFG_BAM_DISPLAY4_KEY_MAP))
      {
         GetKeyPressValues(pszValue, STRLEN(pszValue), &pme->m_wAltKeyUpList[3],
            &pme->m_wAltKeyDownList[3], &pme->m_wAltKeySelectList[3], &pme->m_wAltKeyClrList[3]);

         FARF(BAM, ("Our Display4 key mappings are %d, %d, %d, %d", pme->m_wAltKeyUpList[3],
            pme->m_wAltKeyDownList[3], pme->m_wAltKeySelectList[3], pme->m_wAltKeyClrList[3]));
      }
#endif

      

      FREE(pszName);
      FREE(pszValue);
   }
   IFILE_Release(pOEMCfg->m_pIFile);
   if (pOEMCfg->m_pCfgFileBuf != NULL)
      FreePtr((void **) &pOEMCfg->m_pCfgFileBuf);
   FreePtr((void **) &pOEMCfg);
   return TRUE;
}

#endif











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
#ifdef USE_IDL2_4GB_SUPPORT
boolean IsExpired(DL2AppModInfo* pai)
#else
boolean IsExpired(AppModInfo* pai)
#endif
{
   if(pai->li.pt == PT_SUBSCRIPTION || pai->li.dwExpire == BV_UNLIMITED)
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

      case LT_USES:
         FARF(BAM, ("License is for num uses, and dwExpire is %d", pai->li.dwExpire));
         return FALSE;  // LT_USES licensed apps are expected to manage their usage-based
                        // license within the application itself.
      case LT_MINUTES_OF_USE:
         FARF(BAM, ("License is for num Minutes, and dwExpire is %d", pai->li.dwExpire));
         return(pai->li.dwExpire == 0);
   }
   return FALSE;
}




#ifdef FEATURE_TRUSTED_TIME

//===========================================================================
// 
// DESCRIPTION
//    Determines if an application's status depends on clock time
//    as returned by GETUTCSECONDS()
//
//    The logic here is stolen/modified from IsExpired().
//
// PARAMETERS
//    pai:     AppModInfo received from ADS.
//
// Return Value:
//    [TRUE|FALSE]
//
//===========================================================================
boolean IsTimeBased(DL2AppModInfo* pai)
{
   if (pai->li.pt == PT_SUBSCRIPTION || pai->li.dwExpire == BV_UNLIMITED)
   {
      return FALSE;
   }

   switch(pai->li.lt)
   {
      case LT_DATE:
      case LT_DAYS:  
      {
         // Check for pre-loaded apps
         if (pai->li.dwExpire < ONE_YEAR_SECS * 5)
         {
            return FALSE;
         }
         else
         {
            return TRUE;
         }
      }
      default:
         break;
   }
   return FALSE;
}

#endif // FEATURE_TRUSTED_TIME






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
//    [NORMAL|DISABLED|DEMO|EXPIRED|EXPIREDDEMO|INDETERMINATE]
//
//===========================================================================
AppStatus AppMgr_AppStatus(AppMgr* pme, uint32 itemID)
{
#if defined(FEATURE_BREW_DOWNLOAD)
#ifdef USE_IDL2_4GB_SUPPORT
   DL2AppModInfo* pai;
#else
   AppModInfo* pai;
#endif
   boolean bDemo = FALSE;

   if (!itemID)
      return NORMAL;

#ifdef USE_IDL2_4GB_SUPPORT
   pai = IDOWNLOAD2_GetModInfoEx(pme->m_pDownload2, itemID, GMI_ALL);
#else
   pai = IDOWNLOAD_GetModInfoEx(pme->m_pDownload, itemID, GMI_ALL);
#endif
   if (pai)
   {
#ifdef FEATURE_TRUSTED_TIME
      // If the license is time based and we have secure clock support,
      // check the clock for validity.  If it's not valid we return a
      // value indicating such.
      if (IsTimeBased(pai))
      {
         uint32 nAEEVersion = GETAEEVERSION(NULL, 0, GAV_LATIN1);
         if (nAEEVersion >= AEEVERSION_USE_TRUSTED_TIME)
         {
#if MIN_BREW_BUILD(4,0,0,0)
            // BAM built using BREW 4.0 should be able
            // to instantiate a secure clock when
            // running under this BREW version.
            // If instantiation fails we assume it's for
            // malicious reasons - so we return the value
            // indicating the time can't be trusted.
            int nStatus;
            uint32 dwSecs;
            ISysClock* pSecClock = NULL;
            int nCreate = CreateInstance(pme->a.m_pIShell,
               SECSYSCLOCK_CLSID, (void**)&pSecClock);
            if (nCreate != AEE_SUCCESS )
            {
               FARF(BAM, ("Secure clock instantiation failed"));
               return INDETERMINATE;
            }
            // Read the secure clock to get its status.  If the clock
            // status is bad we can't trust the time.
            // nStatus = ISYSCLOCK_GetTimeS(pSecClock, &dwSecs);
            nStatus = ISysClock_GetTimeS(pSecClock, &dwSecs);
                if (EBADSTATE == nStatus)
                {
                   // Free the clock object
                   FreeObj((void**)&pSecClock);
                   return INDETERMINATE;
                }
            // Free the clock object
            FreeObj((void**)&pSecClock);
#else
            // The secure clock class ID available
            // when compiling with pre-4.0 BREW isn't
            // supported in BREW 4.0.
            FARF(BAM, ("AEEVERSION mismatch for secure clock instantiation"));
            return INDETERMINATE;
#endif
         }
         else  // running under pre-4.0 BREW
         {
#if MIN_BREW_BUILD(4,0,0,0)
            // The secure clock class ID available
            // when compiling with 4.0 BREW isn't
            // supported in BREW pre-4.0.
            FARF(BAM, ("AEEVERSION mismatch for secure clock instantiation"));
            return INDETERMINATE;
#else
            // Here we preserve the pre-BAM 2.1 behavior
            // by doing ... nothing.  There was no notion
            // of secure clocks, therefore all clocks
            // were secure.
            FARF(BAM, ("Secure clock not used in this AEEVERSION"));
#endif
         }
      }
#endif // FEATURE_TRUSTED_TIME

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
static void AppMgr_Prompt(AppMgr * pme, const AECHAR* szTitle, uint16 wIDTitle,
   const AECHAR* szText, uint16 wIDText, uint16* wButtons, uint16 wDefID,
   uint32 dwProps, boolean bBox)
{
   AEERect rc, rcm;
   uint16  nHeight, x, y;
   uint16* origButtons = wButtons;
   AECHAR* szTitleBuf = NULL;
   AECHAR* szTextBuf = NULL;
   IMenuCtl* pMenu = NULL;
#ifndef PROMPT_ALWAYS_USE_SK
   int nItems = 0;
#endif // PROMPT_ALWAYS_USE_SK

   // Create Static control for prompt, only if we are not in the process of switching displays
   // As we have already re-created the static control
   if (!pme->m_bRedrawingUI)
   {
      FreeObj((void**)&pme->m_pStatic);
      if (CreateInstance(pme->a.m_pIShell, AEECLSID_STATIC, (void**)&pme->m_pStatic))
      {
         return;
      }

      // Store the prompt info we were passed so that it can be re-used when handling the flip evnet
      AppMgr_StorePromptInfo(pme, szTitle, wIDTitle, szText, wIDText, wButtons, wDefID, dwProps, bBox);
   } //if (!pme->m_bRedrawingUI)

   // Initialize to 0 to make ARM compiler happy
   nHeight = x = y = 0;

   // clean softkey
#ifdef PROMPT_ALWAYS_USE_SK
   pMenu = pme->m_pSK;
#else
   if (wButtons)
   {
      // Calculate number of items
      while(*(wButtons + nItems))
      {
         nItems++;
      }

      // Choose appropriate menu type on the basis of the number of items
      if (nItems > 1)
      {
         pMenu = pme->m_pMenu;
         dwProps &= ~ST_NOSCROLL;
      }
      else
      {
         pMenu = pme->m_pSK;
      }
   }
#endif // PROMPT_ALWAYS_USE_SK

   

   if (!szTitle && wIDTitle)
      szTitleBuf = (AECHAR *)MALLOC(MAX_STR_SIZE);
   else
      szTitleBuf = (AECHAR *)szTitle;
   
   if (!szText)
      szTextBuf = (AECHAR *)MALLOC(MAX_STR_SIZE);
   else
      szTextBuf = (AECHAR *)szText;

   // Add Items
   if (wButtons)
   {
      // The buttons should already be in the menu, so don't re-add them
      // If we are processing the flip event
      if (!pme->m_bRedrawingUI)
      {
         while (*wButtons)
         {
               IMENUCTL_AddItem(pMenu, BREWAPPMGRLS_RES_LOC, *wButtons, *wButtons, NULL, 0);
            wButtons++;
         }
      } // if (!pme->m_bRedrawingUI)

      if (pMenu == pme->m_pSK) // Using SoftKey control
      {
         // Get SoftKey Rect
         IMENUCTL_GetRect(pMenu, &rcm);

         // SoftKey Height
         nHeight = rcm.dy;
      }
      else
      {
#ifndef PROMPT_ALWAYS_USE_SK
         AEERect itemRect;

         IMENUCTL_GetItemRect(pMenu, 0, &itemRect);
         nHeight = nItems * itemRect.dy;
      
         SETAEERECT(&rcm, 0, pme->m_rc.dy - nHeight, pme->m_rc.dx, nHeight);
         IMENUCTL_SetRect(pMenu, &rcm);
#else
         // Should never execute this
         FARF(BAM, ("Unexpected control"));
#endif // PROMPT_ALWAYS_USE_SK
      }

      // The selection should already be selected if we are re-displaying the 
      // prompt becuase the user flipped the screen while it was active.
      if (!pme->m_bRedrawingUI)
      {
         // Set Default Selection
         IMENUCTL_SetSel(pMenu, wDefID);
      } // if (!pme->m_bRedrawingUI)
   }

   // Draw Surrounding Box
   if (bBox && wButtons)
   {
      x = (pme->m_rc.dx - (pme->m_rc.dx*DISPLAY_AREA_PERCENT/100))/2;
      y = (pme->m_rc.dy - (pme->m_rc.dy*DISPLAY_AREA_PERCENT/100))/2;

      // Draw Border
      SETAEERECT(&rcm, x-1, y-1, pme->m_rc.dx - 2*(x-1), pme->m_rc.dy - 2*(y-1));

#ifdef USE_TT_FONTS
      IDISPLAY_FillRect(pme->m_pIDisplay, &rcm, RGB_WHITE);
#else
      IDISPLAY_EraseRect(pme->m_pIDisplay, &rcm);
#endif // USE_TT_FONTS
      IDISPLAY_DrawRect(pme->m_pIDisplay, &rcm, RGB_BLACK, 0, IDF_RECT_FRAME);

      SETAEERECT(&rcm, x, pme->m_rc.dy - y - nHeight, pme->m_rc.dx - 2*x, nHeight);
      IMENUCTL_SetRect(pMenu, &rcm);
   }

   // Set Static Control Properties
   if (dwProps)
   {
      ISTATIC_SetProperties(pme->m_pStatic, dwProps);
   }

   // Set Static Control title and text and display it
   if (!szTitle && wIDTitle)        
   {
      ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, wIDTitle, szTitleBuf, MAX_STR_SIZE);
   }

   if (!szText)
   {
      ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, wIDText, szTextBuf, MAX_STR_SIZE);
   }

   if (wButtons)
   {
      // Set Static Control Size
      if (bBox)
      {
         SETAEERECT(&rc, x + MARGIN, y + MARGIN, pme->m_rc.dx - 2*(x + MARGIN),
            pme->m_rc.dy - 2*y - nHeight - 2*MARGIN);
      }
      else
      {
         if (pme->m_bDispTouchEnabled)
         {
            SETAEERECT(&rc, pme->m_rc.x, pme->m_rc.y, pme->m_rc.dx, pme->m_rc.dy - (nHeight  + 2 * TOUCH_PIXELPAD ));
         }
         else
         {
            SETAEERECT(&rc, pme->m_rc.x, pme->m_rc.y, pme->m_rc.dx,
                       pme->m_rc.dy - nHeight);
         }
      }
      ISTATIC_SetRect(pme->m_pStatic, &rc);
      ISTATIC_SetText(pme->m_pStatic, szTitleBuf, szTextBuf, AEE_FONT_BOLD, AEE_FONT_NORMAL);
      // Store the static's text for use with flipping the cover
      AppMgr_StoreStaticTextInfo(pme, szTitleBuf, szTextBuf, AEE_FONT_BOLD, AEE_FONT_NORMAL);
      // Store the static's size to fit setting for use with flipping the cover
      AppMgr_StoreStaticSizeToFitInfo(pme, FALSE, NULL);
   }
   else
   {
      x = (pme->m_rc.dx - (pme->m_rc.dx*DISPLAY_AREA_PERCENT/100))/2;
      SETAEERECT(&rc, x + MARGIN, pme->m_rc.y, pme->m_rc.dx - 2*(x + MARGIN),
         pme->m_rc.dy);    
      ISTATIC_SetRect(pme->m_pStatic, &rc);
      ISTATIC_SetText(pme->m_pStatic, szTitleBuf, szTextBuf, AEE_FONT_BOLD,
         AEE_FONT_NORMAL);
      // Store the static's text for use with flipping the cover
      AppMgr_StoreStaticTextInfo(pme, szTitleBuf, szTextBuf, AEE_FONT_BOLD, AEE_FONT_NORMAL);
      ISTATIC_SizeToFit(pme->m_pStatic, &rc);
      // Store the static's size to fit setting for use with flipping the cover
      AppMgr_StoreStaticSizeToFitInfo(pme, TRUE, &rc);
      SETAEERECT(&rcm, rc.x, (pme->m_rc.dy - rc.dy)/2, rc.dx, rc.dy);
      ISTATIC_SetRect(pme->m_pStatic, &rcm);

      // Draw Border
      SETAEERECT(&rc, rcm.x - MARGIN - 1, rcm.y - MARGIN - 1,
         rcm.dx + 2*(MARGIN + 1), rcm.dy + 2*(MARGIN + 1));
#ifdef USE_TT_FONTS
      IDISPLAY_FillRect(pme->m_pIDisplay, &rc, RGB_WHITE);
#else
      IDISPLAY_EraseRect(pme->m_pIDisplay, &rc);
#endif // USE_TT_FONTS
      IDISPLAY_DrawRect(pme->m_pIDisplay, &rc, RGB_BLACK, 0, IDF_RECT_FRAME);
   }

   ISTATIC_SetActive(pme->m_pStatic, TRUE);
   ISTATIC_Redraw(pme->m_pStatic);

#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
    pme->m_bPromptIsDisplayed = TRUE;
#endif

   // Make SoftKey Active, and note thta the propt is displayed
   // don't need to note this for "timed messages"
   if (wButtons)
   {
      if (pme->m_bDispTouchEnabled)
      {
         if (pMenu == pme->m_pMenu)
         {
            IMENUCTL_SetActive(pMenu, TRUE);
         }
         else
         {
            if( pme->m_wState != ST_SCREENSAVERS &&  pme->m_wState != ST_MOVEWORK && pme->m_wState != ST_MOVEFAVORITES && pme->m_wState != ST_MOVEFUN	&&
		          pme->m_wState != ST_ORDERFAVORITES && pme->m_wState != ST_ORDERWORK && pme->m_wState != ST_ORDERFUN	&&  pme->m_wState != ST_MAINMENU
		         ) 
	         {
	            // CR90583 fix: Resizing softmenu width dynamically according to string being displayed; Set width to default SOFTMENU_WIDTH if the string is smaller - for being tangible!!!
		         int nStrWidth = 0;
               int pnFits = 0;
               int nSoftmenuWidth = 0;
		         AECHAR	 szTemp[MAX_TOUCH_SOFTMENUSTRING];
		         
               
               for (; NULL != *origButtons ; origButtons++)
               {

                  ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, *origButtons, szTemp, sizeof(szTemp));
		            nStrWidth = IDisplay_MeasureTextEx(pme->m_pIDisplay,
							                                AEE_FONT_NORMAL,  
							                                szTemp,
							                                -1,
							                                -1, 
							                                &pnFits);

		            nSoftmenuWidth += (((nStrWidth * 2 + TOUCH_PIXELPAD *2) < TOUCH_SOFTMENU_WIDTH ) ? TOUCH_SOFTMENU_WIDTH :(nStrWidth * 2 + TOUCH_PIXELPAD *2));
               }

		         IMENUCTL_GetRect(pMenu, &rcm);
		         rcm.x = 0;
		         rcm.dx =  nSoftmenuWidth;
	            IMENUCTL_SetRect(pMenu, &rcm);
            }
         }
      }
      else
      {
         // Make SoftKey Active
	      IMENUCTL_SetActive(pMenu, TRUE);
      }
      
      IMENUCTL_Redraw(pMenu);

      if (pme->m_bDispTouchEnabled /*&& !pme->m_bHasDPad*/)
      {
         IMENUCTL_SetActive(pMenu, FALSE);
      }
   }

   if (!szTitle && wIDTitle)
      FREE(szTitleBuf);

   if (!szText)
      FREE(szTextBuf);

   IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
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
   DLITEMID nID = AppMgr_GetItemID(pme, pCurrentApp);
   uint32 bReturn = AEE_SUCCESS;

   pme->m_bHandledLaunchError = FALSE;

   if (bForceRun)
   {
      pme->m_cfg.clsLaunchedApp = pme->m_nLaunchCls = cls;

      bReturn = ISHELL_StartApplet(pme->a.m_pIShell, cls);
      
      pCurrentApp->dwNumLaunches++;
      FARF(BAM, ("BAM from ForceRun set numLunches for clsid of %d to %d.", cls, pCurrentApp->dwNumLaunches));
      pCurrentApp->dwLastLaunch = GETUTCSECONDS();
      (void)AppMgr_WriteAppList(pme);
      
      FARF(BAM, ("BAM ISHELL_StartAppliet() returned %d.", bReturn));
   }
   else
   {
      AppStatus nAppStatus = AppMgr_AppStatus(pme, nID);
      switch (nAppStatus)
      {
         case NORMAL:
         {

#ifdef FEATURE_UPGRADE_AUTO_PROMPT
            // If the item ID is nozero AND there's an UpgradeMgr see
            // if it's appropriate to display the upgrade auto-prompt.
            if ((0 != nID) && (NULL != pme->m_pUm))
            {
               // See if the auto-prompt timer for this app has expired.
               uint32 dwNow = GETUTCSECONDS();
               uint32 dwTimerStart = pCurrentApp->dwAutoPromptTimer;
               uint32 dwElapsed = dwNow - dwTimerStart;

               // If the timer has elapsed ....
               if (dwElapsed > UPGRADE_AUTO_PROMPT_INTERVAL)
               {
                  // Check for an available upgrade for this app
                  DLITEMID ugItem = IUM_LatestUpgradeItem(pme->m_pUm, nID);
                  if (0 != ugItem)  // if an upgrade is available....
                  {
                     // Reset the prompt timer for this app
                     pCurrentApp->dwAutoPromptTimer = dwNow;
                     // Remember what state we're in before the auto-prompt
                     pme->m_wLastState = pme->m_wState;
                     // Install the upgrade item into the current app 
                     pCurrentApp->dwUpgradeID = ugItem;
                     // Prompt the user about getting the upgrade
                     return AppMgr_SetState(pme, ST_UPGRADEAUTOPROMPT);
                  }
               }
            }
#endif // FEATURE_UPGRADE_AUTO_PROMPT

            pme->m_cfg.clsLaunchedApp = pme->m_nLaunchCls = cls;
            bReturn = ISHELL_StartApplet(pme->a.m_pIShell, cls);
            pCurrentApp->dwNumLaunches++;
            FARF(BAM, ("BAM from set numLunches for clsid of 0x%x to %d.", cls, pCurrentApp->dwNumLaunches));
            pCurrentApp->dwLastLaunch = GETUTCSECONDS();
            (void)AppMgr_WriteAppList(pme);
            FARF(BAM, ("BAM ISHELL_StartAppliet() returned %d.", bReturn));
            break;
         }
         
         default:
            if (!pme->m_bHasStarted)
            {
               // We have not started yet, so we need to start up and make
               // use of the AutoStart feature to launch the app
               FARF(BAM, ("Bringing BAM to the forground so that we can launch app."));

               pme->m_bLaunchClsOnStart = TRUE;
               pme->m_cfg.clsLaunchedApp = pme->m_nLaunchCls = cls;

               ISHELL_StartApplet(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID);
               return TRUE;
            }

            pme->m_wLastState = pme->m_wState;
            AppMgr_DisplayAppStatusDialog(pme, nAppStatus);
            break;
      }
   }

   if (bReturn != SUCCESS)      
   {
      FARF(BAM, ("BAM Launch Current Applet failed."));
      AppMgr_HandleLaunchError(pme, cls, bReturn);
   }

   return (bReturn == AEE_SUCCESS);
}







/*===========================================================================

   State handler for ST_UPGRADEAUTOPROMPT. 

===========================================================================*/
static boolean AppMgr_UpgradeAutoPrompt(AppMgr* pme)
{
   uint16 wButtons[3];
   uint16 titleID = IDS_UPGRADEAUTOPROMPT_TITLE;
   uint16 txtID = IDS_UPGRADEAUTOPROMPT_TEXT;
   AppMgrItem* currItem = pme->m_pCurrentApp;
   AECHAR* bodyTextTemplate = NULL;
   AECHAR* bodyText = NULL;

   // There should be a current applet with an upgrade item installed.
   if (NULL == currItem)
   {
      FARF(BAM, ("UpgradeAutoPrompt(): no current item."));
      return TRUE;
   }
   if (0 == currItem->dwUpgradeID)
   {
      FARF(BAM, ("No upgrade item in current app."));
      return TRUE;
   }

   bodyTextTemplate = (AECHAR *)MALLOC(MAX_STR_SIZE);
   bodyText = (AECHAR *)MALLOC(MAX_STR_SIZE);

   ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, txtID,
      bodyTextTemplate, MAX_STR_SIZE);

   // Incorporate the app name into the body text.
   WSPRINTF(bodyText, MAX_STR_SIZE, bodyTextTemplate, currItem->wszName);

   MEMSET(wButtons, 0, sizeof(wButtons));
   wButtons[0] = IDC_UPGRADEAUTOPROMPT_PURCHASE;  // Button text is 'No':-)
   wButtons[1] = IDC_NO;

   AppMgr_Prompt(pme, NULL, titleID, bodyText, 0, wButtons, wButtons[0],
      ST_NOSCROLL, FALSE);

   FREE(bodyTextTemplate);
   FREE(bodyText);

   return(TRUE);
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
      FARF(BAM, ("Ignoring System error as it is for a different App"));
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

   AppMgr_Prompt(pme, pTitle, 0, pText, 0, NULL, 0, CP_USE_DEFAULT, FALSE);

   // If we are on a touch display we need to provide some "ok/exit" button
#ifdef FEATURE_TOUCHSCREEN_SUPPORT
   if (pme->m_bDispTouchEnabled)
   {
      uint16 id[1];
      id[0] = IDC_OK;

      AppMgr_CreateSoftmenu(pme,id, 1);
      IMENUCTL_SetActive(pme->m_pSK, FALSE);
      IMENUCTL_Redraw(pme->m_pSK);
   }
#endif //FEATURE_TOUCHSCREEN_SUPPORT

   FreePtr((void**)&pText);

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
   if (pme->m_bHandledLaunchError) {
      return TRUE;
   }
   else {
      pme->m_bHandledLaunchError = TRUE;
   }

   pme->m_wLastState = pme->m_wState;

   // Don't display error if notification is for app launched by someone else
   if (cls != pme->m_nLaunchCls)
   {
      FARF(BAM, ("Ignoring Lauch error as it is for a different App"));
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
         AppMgr_Prompt(pme, NULL, 0, NULL, IDS_LICENSE_NOT_FOUND, wButtons, wButtons[1], ST_NOSCROLL, TRUE);
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
   return FALSE;
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

   if (!ppImage || !cls || !AppMgr_GetAEEAppInfo(pme, cls, &ai))
   {
      return;
   }

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
   IShell* pIShell = NULL; 
   AEECLSID cls = 0;

   if (!pi)
      return NULL;

   if (*pi->wszName)
      return pi->wszName;

   if (!pme)
      return NULL;

   cls = pi->cls;
   pIShell = pme->a.m_pIShell;

   if (AEECLSID_MOBILESHOP_BID == cls)
   {
      ISHELL_LoadResString(pIShell, BREWAPPMGRLS_RES_LOC, IDS_MOBILESHOP, pi->wszName, sizeof(pi->wszName));
   }
   else
   if (cls && AppMgr_GetAEEAppInfo(pme, cls, &ai))
   {
      ISHELL_LoadResString(pIShell, ai.pszMIF, APPR_NAME(ai), pi->wszName, sizeof(pi->wszName));
   }

   if(!*pi->wszName)
   {
      ISHELL_LoadResString(pIShell, AEECONTROLS_RES_FILE, AEE_IDS_UNKNOWN, pi->wszName, sizeof(pi->wszName));
   }

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
      CreateInstance(pIShell, ISHELL_GetHandler(pIShell, HTYPE_VIEWER, (char*)(pByte + 2)), (void**)&pImage);

      if (pImage)
      {
         IMemAStream * pmi = NULL;
         CreateInstance(pIShell, AEECLSID_MEMASTREAM, (void**)&pmi);
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

   if (!cls || !AppMgr_GetAEEAppInfo(pme, cls, &ai))
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
      (CreateInstance(pIShell, ISHELL_GetHandler(pIShell, HTYPE_VIEWER, (char*)(pByte + 2)), (void**)&pImage) != AEE_SUCCESS) ||
      !pImage)
   {
      FreePtr((void**)&pByte);
      return NULL;
   }
         
   CreateInstance(pIShell, AEECLSID_MEMASTREAM, (void**)&pmi);
         
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
//    Gets The Menu rectangle, depending on State
//
// PARAMETERS
//    pme      : the AppMgr
//    pSKMenu  : the SK Menu, may no 
//    prc      : The AEERect to fill in, must not be NULL
//
// Return Value:
//    True if we are able to fill in the Rect.
//
//===========================================================================
boolean AppMgr_GetMenuRect(AppMgr * pme, IMenuCtl* pSKMenu, AEERect* prc)
{

   if ((NULL == pme) || (NULL == prc))
   {
      return FALSE;
   }

   switch (pme->m_wState)
   {
      case ST_MANAGETOOL:
      {
         return AppMgr_GetMenuRectForManageTool(pme, prc);
      }

      case ST_ORDERAPPS:
      case ST_MOVEAPPS:
      case ST_MOVE:
      case ST_SETCATEGORY: 
      {
         return AppMgr_GetMenuRectForCategories(pme, prc);
      }
#ifdef MSHOP_STATES
      case ST_MSHOP:
      {
         return AppMgr_GetMenuRectForMShop(pme, prc);
      }
      
      case ST_MSHOPGUIDE:
      {
         return AppMgr_GetMenuRectForMShopGuide(pme, prc);
      }

#endif // MSHOP_STATES
      case ST_SCREENSAVERS:
      {
         return AppMgr_GetMenuRectForScreensavers(pme, pSKMenu, prc);
      }
      
      case ST_ORDERMAINMENU:
      case ST_ORDERFAVORITES:
      case ST_ORDERWORK:
      case ST_ORDERFUN:
      case ST_MOVEMAINMENU:
      case ST_MOVEFAVORITES:
      case ST_MOVEWORK:
      case ST_MOVEFUN:
      {
         return AppMgr_GetMenuRectForConfig(pme, prc);
      }

      case ST_EDITSOUNDIMAGE:
      {
         // If we have the ShowFiles dialog open...
         if (ST_EDITSOUNDIMAGEBROWSENONE != pme->m_wEditSoundImageSubState)\
         {
            return AppMgr_GetMenuRectForShowFiles(pme, prc);
         }

         // otherwise fall through and pick up the defaults
      }

      default:
      {
         SETAEERECT(prc, pme->m_rc.x, pme->m_rc.y, pme->m_rc.dx, pme->m_rc.dy);
         return TRUE;
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
         IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
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


   // We are about to delete everything that is needed
   // for redisplaying the prompt, so make sure
   // we set it to false;
   pme->m_bPromptIsDisplayed = FALSE;

#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   // Clear all of our stored Info, as it is no longer valid
   // Don't know yet about cbDisplayEnabled
   //if (pme->cbDisplayEnabled)
   //{
   //   CALLBACK_Cancel(pme->cbDisplayEnabled);
   //   FreePtr((void**)&pme->cbDisplayEnabled);
   //}
   // Free control stored control values
   FreePtr((void**)&(pme->m_pMainMenuResFile));
   pme->m_pMainMenuResID = 0;
   FreePtr((void**)&(pme->m_pMainMenuText));
   FreePtr((void**)&(pme->m_pMenuResFile));
   pme->m_pMenuResID = 0;
   FreePtr((void**)&(pme->m_pMenuText));
   FreePtr((void**)&(pme->m_pStaticTitle));
   FreePtr((void**)&(pme->m_pStaticText));
   FreePtr((void**)&(pme->m_pPromptTitle));
   pme->m_wPromptTitleID = 0;
   FreePtr((void**)&(pme->m_pPromptText));
   pme->m_wPromptTextID = 0;
   FreePtr((void**)&(pme->m_pPromptButtons));
   pme->m_wPromptDefID = 0;
#endif // FEATURE_SUPPORT_MULTIPLE_DISPLAYS

   // For the case where user hits clear key on config menu
   pme->m_bConfigMenu = FALSE;

   // Clear Display Area
   if (bClear)
   {
#ifdef USE_TT_FONTS
      // When TrueType fonts are enabled stuff gets included from
      // BUIW that makes ClearScreen misbehave.  I think.  Maybe.
      IDISPLAY_FillRect(pme->m_pIDisplay, NULL, RGB_WHITE);
#else
      //IDISPLAY_ClearScreen(pme->m_pIDisplay);
      IDISPLAY_EraseRect(pme->m_pIDisplay, &pme->m_rc);
#endif // USE_TT_FONTS
   }

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
      if (ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC,
         IDS_SYSTEMINFO_RAM, szTemp, sizeof(szTemp)))
      {
         pText = AppendWStr(pText, szTemp, '\n');
      }
      FormatMem(dwRAM, szTemp, 32, FALSE);
      pText = AppendWStr(pText, szTemp, ' ');
   }

   if (ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC,
      IDS_OPTIONS_BREW_INFO, szTemp, sizeof(szTemp)))
   {
      pText = AppendWStr(pText, szTemp, '\n');
   }

   // Get AEE Version
#if defined(GAV_UPDATE)
   GETAEEVERSION((byte*)szVersion, sizeof(szVersion), GAV_LATIN1|GAV_UPDATE);
#else
   GETAEEVERSION((byte*)szVersion, sizeof(szVersion), GAV_LATIN1);
#endif

   STRTOWSTR(szVersion, szTemp, sizeof(szTemp));
   pText = AppendWStr(pText, szTemp, ' ');

   if (ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC,
      IDS_APPLICATION_MANAGER, szTemp, sizeof(szTemp)))
   {
      pText = AppendWStr(pText, szTemp, '\n');
   }

   if (ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRCOMMON_RES_FILE,
      IDS_APPMGR_VERSION, szTemp, sizeof(szTemp)))
   {
      pText = AppendWStr(pText, szTemp, ' ');
   }

   // Display Prompt
   MEMSET(wButtons, 0, sizeof(wButtons));
   wButtons[0] = IDC_OK;

   // Cache Current Size
   rc = pme->m_rc;

   // Change Prompt Size
   pme->m_rc = *prc;
   AppMgr_Prompt(pme, NULL, NULL, pText, 0, wButtons, wButtons[0],
      ST_NOSCROLL|ST_CENTERTEXT, FALSE);

   FreePtr((void**)&pText);

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

#ifdef FEATURE_TRUSTED_TIME
      case INDETERMINATE:
         wIDText = IDS_INDETERMINATE_APP_STATUS_TEXT;
         wIDTitle = IDS_INDETERMINATE_APP_STATUS_TITLE;
         wButtons[0] = IDC_OK;
         break;
#endif // FEATURE_TRUSTED_TIME

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
   AppMgr_Prompt(pme, NULL, wIDTitle, NULL, wIDText, wButtons, wButtons[0],
      ST_NOSCROLL, FALSE);
}

#ifdef MSHOP_STATES
//===========================================================================
//
// DESCRIPTION
//    Function for determing Menu size when in state ST_MSHOPGUIDE
// 
// PARAMETERS
//    pme:  AppMgr
//    prc:  AEERect*
//
//===========================================================================
static boolean AppMgr_GetMenuRectForMShopGuide(AppMgr* pme, AEERect* prc)
{
   IImage* pMShop = NULL;

   if ((NULL == pme) || (NULL == prc))
   {
      return FALSE;
   }

   pMShop = ISHELL_LoadResImage(pme->a.m_pIShell, BREWAPPMGRLN_RES_FILE, IDB_MSHOP_THUMB);

   if (pMShop)
   {
      AEEImageInfo ii;

      // Get Image Info
      IIMAGE_GetInfo(pMShop, &ii);

      // Release Image
      IIMAGE_Release(pMShop);


      // Menu Size/Location
      SETAEERECT(prc, 0, ii.cy, pme->m_rc.dx, pme->m_rc.dy - ii.cy);      
   }
   else
   {
      SETAEERECT(prc, pme->m_rc.x, pme->m_rc.y, pme->m_rc.dx, pme->m_rc.dy);
   }

   return TRUE;
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
   IImage* pMShop =
      ISHELL_LoadResImage(pme->a.m_pIShell, BREWAPPMGRLN_RES_FILE, IDB_MSHOP_THUMB);

   // Draw MShop Logo
   if (pMShop)
   {
      AEEImageInfo ii;
      AECHAR pTitle[32];
      AEERect rc;

      // Get Image Info
      IIMAGE_GetInfo(pMShop, &ii);

      // Set Image Background Transparent
      IIMAGE_SetParm(pMShop, IPARM_ROP, AEE_RO_TRANSPARENT, 0);

      // Draw Image
      IIMAGE_Draw(pMShop, 0, 0);

      // Release Image
      IIMAGE_Release(pMShop);

      // Draw Title
      ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, IDC_HELP,
         pTitle, sizeof(pTitle));
      IDISPLAY_DrawText(pme->m_pIDisplay, AEE_FONT_BOLD, pTitle, -1,
         ii.cx + 1, ii.cy - pme->m_cyFont, NULL, 0);

      if (OEM_UNDERLINE_MENU_TITLE)
      {
         IDISPLAY_DrawHLine(pme->m_pIDisplay, 0, ii.cy + 1, pme->m_rc.dx);
         ii.cy += 2;
      }

      //SETAEERECT(&rc, 0, ii.cy, pme->m_rc.dx, pme->m_rc.dy - ii.cy);
      // Menu Size/Location
      AppMgr_GetMenuRectForMShopGuide(pme, &rc);
      IMENUCTL_SetRect(pme->m_pMenu, &rc);
   }

   // If we are in the process of processing the flip event we don't want to modify the data.  The MenuControl items
   // have already been copied over and the current selection has been set (and the last state should already be the last state)
   if (!pme->m_bRedrawingUI)
   {

      // add menu items
      IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_MSHOPGUIDE_MSHOP, IDC_MSHOPGUIDE_MSHOP, NULL, NULL);
      IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_MSHOPGUIDE_CATALOG, IDC_MSHOPGUIDE_CATALOG, NULL, NULL);
      IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_MSHOPGUIDE_FIND, IDC_MSHOPGUIDE_FIND, NULL, NULL);
      IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_MSHOPGUIDE_PREVIEW, IDC_MSHOPGUIDE_PREVIEW, NULL, NULL);
      IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_MSHOPGUIDE_PURCHASE_OPTIONS, IDC_MSHOPGUIDE_PURCHASE_OPTIONS, NULL, NULL);
      IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_MSHOPGUIDE_PAYMENT, IDC_MSHOPGUIDE_PAYMENT, NULL, NULL);
      IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_MSHOPGUIDE_MEMORY_FULL, IDC_MSHOPGUIDE_MEMORY_FULL, NULL, NULL);
      IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_MSHOPGUIDE_RESTORING_APPS, IDC_MSHOPGUIDE_RESTORING_APPS, NULL, NULL);
      IMENUCTL_AddItem(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_MSHOPGUIDE_CAN_I_RETURN_APPS, IDC_MSHOPGUIDE_CAN_I_RETURN_APPS, NULL, NULL);

      // Set menu selection
      if (pme->m_nIndex > 1)
         IMENUCTL_SetSel(pme->m_pMenu, pme->m_nMenuSel[pme->m_nIndex--]);
                  
      // Set Menu Active
      IMENUCTL_SetActive(pme->m_pMenu, TRUE);

      // Update UI
      IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

      // Set Last State to MShop
      pme->m_wLastState = ST_MSHOP;
   } // if (!pme->m_bRedrawingUI)

   return(TRUE);
}
#endif



//===========================================================================
//
// DESCRIPTION
//    Function for determing Menu size when in state ST_CATEGORIES
// 
// PARAMETERS
//    pme:  AppMgr
//    prc:  AEERect*
//
//===========================================================================
static boolean AppMgr_GetMenuRectForCategories(AppMgr* pme, AEERect* prc)
{
   if ((NULL == pme) || (NULL == prc))
   {
      return FALSE;
   }

    switch (pme->m_wState)
   {
      case ST_MOVE:
      case ST_SETCATEGORY:
         SETAEERECT(prc, 0, pme->m_cyFont, pme->m_rc.dx, pme->m_rc.dy - pme->m_cyFont);
         break;

      default:
         SETAEERECT(prc, 0, 0, pme->m_rc.dx, pme->m_rc.dy);
         break;
   }

   return TRUE;
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

   ai.pszResText  = BREWAPPMGRLS_RES_LOC;
   ai.pszResImage = BREWAPPMGRLN_RES_FILE;

   // add title to the menu
   switch (pme->m_wState)
   {
      case ST_MOVE:
      case ST_SETCATEGORY:
      {
         // Draw App Name
         IDISPLAY_DrawText(pme->m_pIDisplay, AEE_FONT_BOLD, AppMgr_GetAppName(pme, pme->m_pCurrentApp), -1, 0, 0, NULL, 0);

         // Set Title
         IMENUCTL_SetTitle(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, (uint16)((pme->m_wState == ST_MOVE) ? IDS_MOVE : IDS_SET_CATEGORY), NULL);
         // Store the menu's title for use with flipping the cover
         AppMgr_StoreMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, (uint16)((pme->m_wState == ST_MOVE) ? IDS_MOVE : IDS_SET_CATEGORY), NULL);

         // Set bCheck
         bCheck = TRUE;
      }
      break;

      default:
         // Set Title
         IMENUCTL_SetTitle(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDS_SELECT, NULL);
         // Store the menu's title for use with flipping the cover
         AppMgr_StoreMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, IDS_SELECT, NULL);
   }

   // Get the Menu Control Rect Size
   AppMgr_GetMenuRectForCategories(pme, &rc);


   // If we are processing the flip evet, don't modify any data/state as it is already set
   // we are just trying to make sure that the display is refreshed
   if (!pme->m_bRedrawingUI)
   {
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
         ai.wImage   = IDB_MAINMENU_SMALL;
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

   } //if (!pme->m_bRedrawingUI)

   // Set Menu Active
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

   return(TRUE);
}

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

   ai.pszResText  = BREWAPPMGRLS_RES_LOC;
   ai.pszResImage = BREWAPPMGRLN_RES_FILE;

   // add title to the menu
   IMENUCTL_SetTitle(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_OPTIONS_MAINMENU_STYLE, NULL);
   // Store the menu's title for use with flipping the cover
   AppMgr_StoreMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, IDC_OPTIONS_MAINMENU_STYLE, NULL);

   // add menu items
   ai.wFont    = AEE_FONT_NORMAL;

   ai.wText    = IDC_SMALL_ICON_VIEW;
   ai.wImage   = (!pme->m_cfg.bListView &&  pme->m_cfg.bSmallIconView) ? IDB_RADIO_SEL : IDB_RADIO_UNSEL;
   ai.wItemID  = IDC_SMALL_ICON_VIEW;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

#ifdef ENABLE_LARGE_ICON_VIEW
// ENABLE_LARGE_ICON_VIEW determines whether large icon view is an option in the Main Menu View 
   ai.wText    = IDC_LARGE_ICON_VIEW;
   ai.wImage   = (!pme->m_cfg.bListView &&  !pme->m_cfg.bSmallIconView) ? IDB_RADIO_SEL : IDB_RADIO_UNSEL;
   ai.wItemID  = IDC_LARGE_ICON_VIEW;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
#endif //ENABLE_LARGE_ICON_VIEW

   ai.wText    = IDC_LIST_VIEW;
   ai.wImage   = pme->m_cfg.bListView ? IDB_RADIO_SEL : IDB_RADIO_UNSEL;
   ai.wItemID  = IDC_LIST_VIEW;
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   // Set Proper Selection
   IMENUCTL_SetSel(pme->m_pMenu, (uint16)(pme->m_cfg.bListView ? IDC_LIST_VIEW : (uint16)(pme->m_cfg.bSmallIconView ? IDC_SMALL_ICON_VIEW : IDC_LARGE_ICON_VIEW)));

   // Set Menu Active
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);   
   
   // Update UI
   IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);

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
#ifdef USE_IDL2_4GB_SUPPORT
   DL2AppModInfo * pmi;
#else
   AppModInfo * pmi;
#endif

   if (!AppMgr_GetItemID(pme, pme->m_pCurrentApp))
      return TRUE;

   // Get module information for the current app
#ifdef USE_IDL2_4GB_SUPPORT
   pmi = IDOWNLOAD2_GetModInfoEx(pme->m_pDownload2, AppMgr_GetItemID(pme, pme->m_pCurrentApp), GMI_ALL);
#else
   pmi = IDOWNLOAD_GetModInfoEx(pme->m_pDownload, AppMgr_GetItemID(pme, pme->m_pCurrentApp), GMI_ALL);
#endif

   if (!pmi)
   {
      if (pme->m_pCurrentApp)
         pme->m_pCurrentApp->dwItemID = 0;
      AppMgr_SetState(pme, PREV_ENTRY);
      return TRUE;
   }

   // add title to the menu
   IMENUCTL_SetTitle(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_LOCK_APP, NULL);
   // Store the menu's title for use with flipping the cover
   AppMgr_StoreMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, IDC_LOCK_APP, NULL);

   // add menu items
   MEMSET(&ai, 0, sizeof(ai));

   ai.pszResText  = BREWAPPMGRLS_RES_LOC;
   ai.pszResImage = BREWAPPMGRLN_RES_FILE;

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
   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

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
   IMENUCTL_SetTitle(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDC_AIRTIME_WARNINGS, NULL);
   // Store the menu's title for use with flipping the cover
   AppMgr_StoreMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, IDC_AIRTIME_WARNINGS, NULL);

   // add menu items

   MEMSET(&ai, 0, sizeof(ai));

   ai.pszResText  = BREWAPPMGRLS_RES_LOC;
   ai.pszResImage = BREWAPPMGRLN_RES_FILE;

   ai.wFont    = AEE_FONT_NORMAL;
   
   bWarning = (pme->m_cfg.bBrowseWarning || pme->m_cfg.bBuyWarning ||
      pme->m_cfg.bCreditBackWarning || pme->m_cfg.bRestoreWarning ||
      pme->m_cfg.bSearchWarning || pme->m_cfg.bUpgradeWarning);

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
   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

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
         AppMgr_GetAEEAppInfo(pme, pi->cls, &ai);
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
         AppMgr_GetAEEAppInfo(pme, pi->cls, &ai);
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
      AppMgr_GetAEEAppInfo(pme, pi->cls, &ai);
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
//    Sets the screen saver based off of the ItemId in the Menu
// 
// PARAMETERS
//    pme:  AppMgr
//    wItemId:  itemId in m_pMenu to set as the screen saver
//
//===========================================================================
static void AppMgr_SetScreensaver(AppMgr* pme, uint16 wItemId)
{
   uint32 dwItem = 0;

   IMENUCTL_GetItemData(pme->m_pMenu, wItemId, &dwItem);

   // Reset Current Handler
   if (dwItem)
      ISHELL_RegisterHandler(pme->a.m_pIShell, AEECLSID_SCREEN_SAVER, MT_SCREEN_SAVER, 0);

   // Set New Handler
   ISHELL_RegisterHandler(pme->a.m_pIShell, AEECLSID_SCREEN_SAVER, MT_SCREEN_SAVER, dwItem);

   AppMgr_VisualConfirmation(pme, 0);
}

//===========================================================================
//
// DESCRIPTION
//    Function for determing Menu size when in state ST_SCREENSAVERS
// 
// PARAMETERS
//    pme:  AppMgr
//    prc:  AEERect*
//
//===========================================================================
static boolean AppMgr_GetMenuRectForScreensavers(AppMgr* pme, IMenuCtl* pSKMenu, AEERect* prc)
{
   AEERect rcm;


   if ((NULL == pme) || (NULL == prc) ||(NULL == pSKMenu))
   {
      return FALSE;
   }

   // Get Softkey Menu Size
   IMENUCTL_GetRect(pSKMenu, &rcm);

   // Set Menu Size
   SETAEERECT(prc, 0, 0, pme->m_rc.dx, rcm.y - 1);

   return TRUE;
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
   AEERect rc;
   uint16 sel = 0;

   uint16 wItemID = IDC_SCREEN_SAVER_APP;
   AEECLSID currentScreenSaver = ISHELL_GetHandler(pme->a.m_pIShell, AEECLSID_SCREEN_SAVER, MT_SCREEN_SAVER);

   // add title to the menu
   IMENUCTL_SetTitle(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDS_SELECT, NULL);
   // Store the menu's title for use with flipping the cover
   AppMgr_StoreMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, IDS_SELECT, NULL);
   
   // Add Menu Items
   MEMSET(&ai, 0, sizeof(ai));

   ai.pszResImage = BREWAPPMGRLN_RES_FILE;
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
      ai.pszResText = BREWAPPMGRLS_RES_LOC;
      ai.wText      = IDS_OPTIONS_NO_SCREEN_SAVER;
      ai.wItemID    = wItemID++;
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

      if (!pme->m_bDispTouchEnabled)
      {
         // Add Set
         IMENUCTL_AddItem(pme->m_pSK, BREWAPPMGRLS_RES_LOC, IDC_SET_SS, IDC_SET_SS, NULL, 0);

         // Add Remove
         IMENUCTL_AddItem(pme->m_pSK, BREWAPPMGRLS_RES_LOC, IDC_REMOVE_SS, IDC_REMOVE_SS, NULL, 0);

         AppMgr_GetMenuRectForScreensavers(pme, pme->m_pSK, &rc);
         IMENUCTL_SetRect(pme->m_pMenu, &rc);

         // Set Selection
         IMENUCTL_SetSel(pme->m_pMenu, sel);

         // Set SK Menu's Active
         IMENUCTL_SetActive(pme->m_pSK, TRUE);   
      }

      IMENUCTL_SetActive(pme->m_pMenu, TRUE);
   
      // Update UI
      IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);
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

   // If we are in the process of procesing the flip event
   // we don't need to recreate all of the data, we just need to update
   // the display so it is now on the new display that the user has
   // flipped to.
   if (!pme->m_bRedrawingUI)
   {
      pme->m_wEditSoundImageSubState = ST_EDITSOUNDIMAGEBROWSENONE;

   // Set properties to display maximum fully displayable items
#ifdef MP_ALLOW_DUPE_IDS
   IMENUCTL_SetProperties(pme->m_pSK, MP_MAXSOFTKEYITEMS|MP_ALLOW_DUPE_IDS);
#else
   IMENUCTL_SetProperties(pme->m_pSK, MP_MAXSOFTKEYITEMS);
#endif // MP_ALLOW_DUPE_IDS

   // Provide Softkey Options

   // Add Done
   IMENUCTL_AddItem(pme->m_pSK, BREWAPPMGRLS_RES_LOC, IDC_CONFIGURE_DONE, IDC_CONFIGURE_DONE, NULL, 0);

   // Add Configure Image
   if (OEM_SET_IMAGE)
      IMENUCTL_AddItem(pme->m_pSK, BREWAPPMGRLS_RES_LOC, IDC_CONFIGURE_IMAGE, IDC_CONFIGURE_IMAGE, NULL, 0);

   //Do not give an option to choose sound files if ISoundPlayer cannot be
   //created on the handset
   if(pme->m_pISp) 
      IMENUCTL_AddItem(pme->m_pSK, BREWAPPMGRLS_RES_LOC, IDC_CONFIGURE_SOUND, IDC_CONFIGURE_SOUND, NULL, 0);

   // Add Reset
   IMENUCTL_AddItem(pme->m_pSK, BREWAPPMGRLS_RES_LOC, IDC_CONFIGURE_RESET, IDC_CONFIGURE_RESET, NULL, 0);
   } //if (!pme->m_bRedrawingUI)

   // Get Softkey Menu Size
   IMENUCTL_GetRect(pme->m_pSK, &rcm);

   AppMgr_GetAppImage(pme, pme->m_pCurrentApp, &pImage);

   strwd = IDISPLAY_MeasureText(pme->m_pIDisplay, AEE_FONT_BOLD, AppMgr_GetAppName(pme, pme->m_pCurrentApp));

   IDISPLAY_DrawText(pme->m_pIDisplay, AEE_FONT_BOLD, AppMgr_GetAppName(pme, pme->m_pCurrentApp), -1, 
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

   // If we are in the process of procesing the flip event
   // we don't need to re-play any sounds and the IMenuCtl already
   // has the selection set.
   if (!pme->m_bRedrawingUI)
   {

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
   } //if (!pme->m_bRedrawingUI)

   // Set Softkey Menu Active
   IMENUCTL_SetActive(pme->m_pSK, TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

   return TRUE;
}

//===========================================================================
//
// DESCRIPTION
//    Function for determing Menu size when in states ST_ORDERMAINMENU, 
//    ST_ORDERFAVORITES, ST_ORDERWORK, ST_ORDERFUN, ST_MOVEMAINMENU,
//    ST_MOVEFAVORITES, ST_MOVEWORK, ST_MOVEFUN
// 
// PARAMETERS
//    pme:  AppMgr
//    prc:  AEERect*
//
//===========================================================================
boolean AppMgr_GetMenuRectForConfig(AppMgr* pme, AEERect* prc)
{
   if ((NULL == pme) || (NULL == prc))
   {
      return FALSE;
   }

   // Set Menu Size
   SETAEERECT(prc, pme->m_rc.x, pme->m_rc.y, pme->m_rc.dx, pme->m_rc.dy);

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
   AEERect rc;

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

      // If we are processing a flip event, thus changing displays, we don't need to build up the menus
      // as they are already built up, we just need to make sure that the displays are updated
      if (!pme->m_bRedrawingUI)
      {
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
      } //if (!pme->m_bRedrawingUI)

      if (IMENUCTL_GetItemCount(pme->m_pMenu))
      {
         // add title to the menu
         IMENUCTL_SetTitle(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDS_SELECT_APP, NULL);
         // Store the menu's title for use with flipping the cover
         AppMgr_StoreMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, IDS_SELECT_APP, NULL);
         
         // Set Size
         AppMgr_GetMenuRectForConfig(pme, &rc);
         IMENUCTL_SetRect(pme->m_pMenu, &rc);

         // Set Main Menu Active
         IMENUCTL_SetActive(pme->m_pMenu, TRUE);

         // Update UI
         IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);
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
      AppMgr_Prompt(pme, NULL, 0, NULL, IDS_EMPTY_CATEGORY, wButtons,
         wButtons[0], ST_NOSCROLL, TRUE);
   }

   return(TRUE);
}

/*===========================================================================

Displays memory use information in options menu

===========================================================================*/
static void AppMgr_DisplayMemoryInfo(AppMgr * pme)
{
   AEERect     rc;
   uint64      qwFree, qwTotal, qwUsed;
   AECHAR      szBuff[32], szFormat[32], szFree[10], szUsed[10], szTotal[10];
   uint16      y, wHeight, wSel;

   // Get height of the rectangle
   wHeight = 2*(pme->m_cyFont + MARGIN);

   // Get y-coordinate of the rectangle
   y = pme->m_rc.dy - wHeight - MARGIN;

   // If we have a softkey, adjust things up.
   if (pme->m_bDispTouchEnabled && (0 < IMENUCTL_GetItemCount(pme->m_pSK)))
   {
      AEERect rcSK;
      IMENUCTL_GetRect(pme->m_pSK, &rcSK);
      y = rcSK.y - wHeight - MARGIN;
   }

   // Draw Horizontal line
   IDISPLAY_DrawHLine(pme->m_pIDisplay, 0, y - MARGIN, pme->m_rc.dx);

   // Erase Rectangle area
   SETAEERECT(&rc, 0, y, pme->m_rc.dx, wHeight);
#ifdef USE_TT_FONTS
   IDISPLAY_FillRect(pme->m_pIDisplay, &rc, RGB_WHITE);
#else
   IDISPLAY_EraseRect(pme->m_pIDisplay, &rc);
#endif // USE_TT_FONTS

    // Draw Rectangle
   SETAEERECT(&rc, MARGIN, y, pme->m_rc.dx - 2*MARGIN, wHeight);
   IDISPLAY_DrawRect(pme->m_pIDisplay, &rc, RGB_BLACK, 0, IDF_RECT_FRAME);

   wSel = IMENUCTL_GetSel(pme->m_pMenu);

   if (wSel == IDC_OPT_INFO)
   {
      // Get Available Space
#ifdef USE_IDL2_4GB_SUPPORT
      DL2ItemSize  is;

      IDOWNLOAD2_GetAvailable(pme->m_pDownload2, &is);

      // Get total space (we don't want to round off, FormatMem should do that for us)
      qwTotal = is.qwFSSize;
            
      // Get free space (we don't want to round off, FormatMem should do that for us)
      qwFree = is.qwFSAvail;
#else
      DLItemSize  is;

      // Get Available Space
      // This is per logic specified by the IDOWNLOAD team of
      // How to figure out the free space since it is not fully
      // supported in 3.1.4 version of IDOWNLOAD
      // -- Begin Logic
      //IDOWNLOAD_GetAvailable()
      //if (value is UINT32_MAX) 
      //  size1= call ISHELL_GetDeviceInfoEx
      //  size 2= IDOWNLOAD_GetConfigItem(CFGI_DOWNLOAD_FS_INFOEX)
      //  if (GetConfigItem() does not return SUCCESS) 
      //    size2= call IDOWNLOAD_GetConfigItem(CFGI_DOWNLOAD_FS_INFO)
      //  if size2 is available from OEM
      //    take min(size1, size2)
      //  else 
      //    take size1
      // -- End Logic
      IDOWNLOAD_GetAvailable(pme->m_pDownload, &is);
      if (is.dwFSSize == MAX_UINT32)
      {
         // We are on a 4 GB+ system and cannot always trust the IDOWNLOAD values
         AEEFSFreeSpace fsFreeSpace;   // size 1 in logic above
         AEEFSFreeSpace fsConfigItemFreeSpace;
         int nSize = sizeof(fsFreeSpace);

         // Initialize fsConfigItemFreeSpace to be what IDownload returns.
         fsConfigItemFreeSpace.qwTotalSpace = is.dwFSSize;
         fsConfigItemFreeSpace.qwFreeSpace = is.dwFSAvail;

         if (SUCCESS != ISHELL_GetDeviceInfoEx(pme->a.m_pIShell, AEE_DEVICEITEM_FSSPACE_ROOT, (void*)&fsFreeSpace, &nSize))
         {
            // We cannot use the IShell values, they are not in the build
            // so copy the IDOWNLOAD values
            fsFreeSpace.qwFreeSpace = is.dwFSAvail;
            fsFreeSpace.qwTotalSpace = is.dwFSSize;
         }
         if (SUCCESS != IDOWNLOAD_GetConfigItem(pme->m_pDownload, CFGI_DOWNLOAD_FS_INFOEX, &fsConfigItemFreeSpace, sizeof(fsConfigItemFreeSpace)))
         {
            if (SUCCESS == IDOWNLOAD_GetConfigItem(pme->m_pDownload, CFGI_DOWNLOAD_FS_INFO, &is, sizeof(is)))
            {
               // use the min of the Device values and the values returned by FS_INFO, as the 
               // OEM may have limited the size available for download.
               fsConfigItemFreeSpace.qwTotalSpace = is.dwFSSize;
               fsConfigItemFreeSpace.qwFreeSpace = is.dwFSAvail;
               
            }
         }


         if (fsConfigItemFreeSpace.qwTotalSpace != 0)
         {
            fsFreeSpace.qwTotalSpace = MIN(fsFreeSpace.qwTotalSpace, fsConfigItemFreeSpace.qwTotalSpace);
            fsFreeSpace.qwFreeSpace = MIN(fsFreeSpace.qwFreeSpace, fsConfigItemFreeSpace.qwFreeSpace);
         }


         // Get total space
         qwTotal = fsFreeSpace.qwTotalSpace;

         // Get free space & round it off
         qwFree = fsFreeSpace.qwFreeSpace;

      }
      else
      {
         // Get total space
         qwTotal = is.dwFSSize;

         // Get free space
         qwFree = is.dwFSAvail;
      }
#endif //USE_IDL2_4GB_SUPPORT
      
      // Calculate used space
      qwUsed = qwTotal - qwFree;

      FormatMem(qwFree, szFree, 10, FALSE);
      FormatMem(qwUsed, szUsed, 10, FALSE);
      FormatMem(qwTotal, szTotal, 10, FALSE);

      // Display "%s available"
      ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC,
         IDS_MT_AVAILABLE, szFormat, sizeof(szFormat));
      WSPRINTF(szBuff, sizeof(szBuff), szFormat, szFree);
      IDISPLAY_DrawText(pme->m_pIDisplay, AEE_FONT_NORMAL, szBuff, -1,
         2*MARGIN, y + MARGIN, NULL, 0);

      // Display "%s used (%s total)"
      ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, IDS_MT_USED,
         szFormat, sizeof(szFormat));
      WSPRINTF(szBuff, sizeof(szBuff), szFormat, szUsed, szTotal);
      IDISPLAY_DrawText(pme->m_pIDisplay, AEE_FONT_NORMAL, szBuff, -1,
         2*MARGIN, y + pme->m_cyFont + MARGIN, NULL, 0);
   }
   else
   {

#ifdef USE_IDL2_4GB_SUPPORT
      DL2AppModInfo  *  pmi;
#else
      AppModInfo  *  pmi;
#endif //USE_IDL2_4GB_SUPPORT
      AECHAR *       pText;

      // Get item data for the selection
      IMENUCTL_GetItemData(pme->m_pMenu, wSel, (uint32*)&pme->m_pCurrentApp);

      // Get module information for the current app
      if (!AppMgr_GetItemID(pme, pme->m_pCurrentApp))
         return;

#ifdef USE_IDL2_4GB_SUPPORT
      pmi = IDOWNLOAD2_GetModInfoEx(pme->m_pDownload2,
         AppMgr_GetItemID(pme, pme->m_pCurrentApp), GMI_ALL);

      if (!pmi)
      {
         if (pme->m_pCurrentApp)
            pme->m_pCurrentApp->dwItemID = 0;
         AppMgr_SetState(pme, PREV_ENTRY);
         return;
      }

      // App size string
      FormatMem(pmi->qwSpaceTotal, szTotal, 10, FALSE);

#else
      pmi = IDOWNLOAD_GetModInfoEx(pme->m_pDownload,
         AppMgr_GetItemID(pme, pme->m_pCurrentApp), GMI_ALL);

      if (!pmi)
      {
         if (pme->m_pCurrentApp)
            pme->m_pCurrentApp->dwItemID = 0;
         AppMgr_SetState(pme, PREV_ENTRY);
         return;
      }

      // App size string
      FormatMem(pmi->dwSpaceTotal, szTotal, 10, FALSE);
#endif

      

      pText = FormatString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, IDS_MT_MEMORYUSE, szTotal); 

      if (pText)
      {
         IDISPLAY_DrawText(pme->m_pIDisplay, AEE_FONT_NORMAL, pText, -1, 2*MARGIN, y + MARGIN, NULL, 0);
         FREE(pText);
      }

      // Expire String
      *szBuff = 0;
      GetExpireString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, 0, pmi->li.lt, pmi->li.dwExpire, szBuff, sizeof(szBuff), TRUE,FALSE);
      IDISPLAY_DrawText(pme->m_pIDisplay, AEE_FONT_NORMAL, szBuff, -1,
         2*MARGIN, y + pme->m_cyFont + MARGIN, NULL, 0);
   }
}


//===========================================================================
//
// DESCRIPTION
//    Function for determing Menu size when in state ST_MANAGETOOL
// 
// PARAMETERS
//    pme:  AppMgr
//    prc:  AEERect*
//
//===========================================================================
static boolean AppMgr_GetMenuRectForManageTool(AppMgr* pme, AEERect* prc)
{
   if ((NULL == pme) || (NULL == prc))
   {
      return FALSE;
   }

    // Set Menu Size
   if (!OEM_SMALL_SCREEN_DEVICE)
   {
      // Set Menu Control size so that memory information can be displayed
      SETAEERECT(prc, 0, 0, pme->m_rc.dx,
         pme->m_rc.dy - 2*pme->m_cyFont - 4*MARGIN);
   }
   else
   {
      // Set Menu Control size
      SETAEERECT(prc, 0, 0, pme->m_rc.dx, pme->m_rc.dy);
   }

   return TRUE;
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
/*
   if (!OEM_SMALL_SCREEN_DEVICE)
      // Set Menu Control size so that memory information can be displayed
      SETAEERECT(&rc, 0, 0, pme->m_rc.dx,
         pme->m_rc.dy - 2*pme->m_cyFont - 4*MARGIN);
   else
      // Set Menu Control size
      SETAEERECT(&rc, 0, 0, pme->m_rc.dx, pme->m_rc.dy);
*/
   if (!AppMgr_GetMenuRectForManageTool(pme, &rc))
   {
      return FALSE;
   }
   // Set Menu Rectangle
   IMENUCTL_SetRect(pme->m_pMenu, &rc);

   nDraw = (pme->m_cyFont ? rc.dy/pme->m_cyFont : rc.dy/ITEM_HEIGHT);
   

   // If we are the in process of process an screen flip, thus in the process of changing displays
   // don't change any of the data/state, as all of that is already set,
   // we just need to redraw the contents of the screen.
   if (!pme->m_bRedrawingUI)
   {
   
      // Add "System Info." in options menu
      MEMSET(&ai,0,sizeof(ai));
      ai.pszResText  = BREWAPPMGRLS_RES_LOC;
      ai.pszResImage = BREWAPPMGRLN_RES_FILE;

      ai.wItemID     = IDC_OPT_INFO;
      ai.wText       = IDC_OPT_INFO;
      ai.wImage      = IDB_MGMT_SYSINFO;

      // Add Item
      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

      nDraw--;

      // Load Disabled and Default Images
      // pDisabled = ISHELL_LoadResImage(pme->a.m_pIShell, BREWAPPMGRLN_RES_FILE, IDB_DISABLED);
      pDefault = ISHELL_LoadResImage(pme->a.m_pIShell, BREWAPPMGRLN_RES_FILE, IDB_APP_DEFAULT); 

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

                     IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);
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
   } //if (!pme->m_bRedrawingUI)

   // Set Menu Active            
   if (nDraw != 0xffffffff)
   {
      IMENUCTL_SetActive(pme->m_pMenu, TRUE); 

      // Preview memory info for large screen devices
      if (!OEM_SMALL_SCREEN_DEVICE)
         AppMgr_DisplayMemoryInfo(pme);
   
      // Update UI
      IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);
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
#ifdef USE_IDL2_4GB_SUPPORT
   DL2AppModInfo*    pmi = NULL;
#else
   AppModInfo*    pmi = NULL;
#endif

   MEMSET(&ai,0,sizeof(ai));

   ai.pszResText  = BREWAPPMGRLS_RES_LOC;
   ai.pszResImage = BREWAPPMGRLN_RES_FILE;

   // Check pme->m_pCurrentApp and pme->m_pCurrentApp->dwItemID as we may be
   // coming here after an upgrade check followed by download

   if (!pme->m_pCurrentApp || !AppMgr_GetItemID(pme, pme->m_pCurrentApp))
   {
      AppMgr_SetState(pme, PREV_ENTRY);
      return TRUE;
   }

   // Get module information for the current app
#ifdef USE_IDL2_4GB_SUPPORT
   pmi = IDOWNLOAD2_GetModInfoEx(pme->m_pDownload2, AppMgr_GetItemID(pme, pme->m_pCurrentApp), GMI_ALL);
#else
   pmi = IDOWNLOAD_GetModInfoEx(pme->m_pDownload, AppMgr_GetItemID(pme, pme->m_pCurrentApp), GMI_ALL);
#endif

   if (!pmi)
   {
      if (pme->m_pCurrentApp)
         pme->m_pCurrentApp->dwItemID = 0;
      AppMgr_SetState(pme, PREV_ENTRY);
      return TRUE;
   }

   // Set Menu Title
   IMENUCTL_SetTitle(pme->m_pMenu, NULL, 0, AppMgr_GetAppName(pme, pme->m_pCurrentApp));
   // Store the menu's title for use with flipping the cover
   AppMgr_StoreMenuTitleInfo(pme, NULL, 0, AppMgr_GetAppName(pme, pme->m_pCurrentApp));

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
   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

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
   AppMgr_Prompt(pme, NULL, nCmd, NULL, wCommandStr, wButtons, wButtons[1],
      ST_NOSCROLL, FALSE);
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
   boolean bRet = FALSE;

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

   ps = (AppMgrState *)&pme->m_states[pme->m_wState];

   if(ps->bClear)
   {
      AppMgr_Reset(pme, TRUE); // Reset display
   }

   // Call state handler
   bRet = ps->pfnReq(pme);

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
   if (!pme->m_bPromptIsDisplayed)
   {
      AppMgr_AddSoftKey(pme);
   }
#endif //FEATURE_TOUCHSCREEN_SUPPORT

   return bRet;
}

/*===========================================================================

State handler for  ST_VIEWPROPERTIES. Displays app properties.

===========================================================================*/
static boolean AppMgr_ViewProperties(AppMgr * pme)
{
   IShell *       ps = pme->a.m_pIShell;
   uint16         wButtons[2];
#ifdef USE_IDL2_4GB_SUPPORT
   DL2AppModInfo  *  pmi;
#else
   AppModInfo  *  pmi;
#endif
   AEEAppInfo *   pai;
   AECHAR         szBuff[32],szFormat[32],szMem[10];
   char *         pszRes;
   AECHAR   *     pText = NULL;
   uint64            qwSpaceTotal = 0;
   uint64            qwSpaceApp = 0;
   uint64            qwSpaceData = 0;

   MEMSET(wButtons,0,sizeof(wButtons));

   if (!AppMgr_GetItemID(pme, pme->m_pCurrentApp))
      return TRUE;

   // Get module information for the current app
#ifdef USE_IDL2_4GB_SUPPORT
   pmi = IDOWNLOAD2_GetModInfo(pme->m_pDownload2, AppMgr_GetItemID(pme, pme->m_pCurrentApp));

   if (!pmi)
   {
      if (pme->m_pCurrentApp)
         pme->m_pCurrentApp->dwItemID = 0;
      AppMgr_SetState(pme, PREV_ENTRY);
      return TRUE;
   }
   qwSpaceTotal = pmi->qwSpaceTotal;
   qwSpaceApp = pmi->qwSpaceApp;
   qwSpaceData = pmi->qwSpaceTotal - pmi->qwSpaceApp;
#else
   pmi = IDOWNLOAD_GetModInfo(pme->m_pDownload, AppMgr_GetItemID(pme, pme->m_pCurrentApp));

   if (!pmi)
   {
      if (pme->m_pCurrentApp)
         pme->m_pCurrentApp->dwItemID = 0;
      AppMgr_SetState(pme, PREV_ENTRY);
      return TRUE;
   }
   qwSpaceTotal = pmi->dwSpaceTotal;
   qwSpaceApp = pmi->dwSpaceApp;
   qwSpaceData = pmi->dwSpaceTotal - pmi->dwSpaceApp;
#endif //USE_IDL2_4GB_SUPPORT

   pai      = pmi->apps;
   pszRes   = pai->pszMIF;

   // Set prompt structure
   wButtons[0] = IDC_OK;       // OK

   pText = NULL;

   // Total app size string
   ISHELL_LoadResString(ps, BREWAPPMGRLS_RES_LOC, IDS_APP_TOTALSIZE, szFormat, sizeof(szFormat));
   FormatMem(qwSpaceTotal, szMem, 10, FALSE);
   WSPRINTF(szBuff, sizeof(szBuff), szFormat, szMem);
   pText = AppendWStr(pText,szBuff,'\n');

   // App size string
   ISHELL_LoadResString(ps, BREWAPPMGRLS_RES_LOC, IDS_APP_APPSIZE, szFormat, sizeof(szFormat));
   FormatMem(qwSpaceApp, szMem, 10, FALSE);
   WSPRINTF(szBuff, sizeof(szBuff), szFormat, szMem);
   pText = AppendWStr(pText,szBuff,'\n');

   // App data size string
   ISHELL_LoadResString(ps, BREWAPPMGRLS_RES_LOC, IDS_APP_APPDATASIZE, szFormat, sizeof(szFormat));
#ifdef USE_IDL2_4GB_SUPPORT
   FormatMem(qwSpaceData, szMem, 10, FALSE);
#else
   if ((MAX_UINT32 == qwSpaceTotal) || (MAX_UINT32 == qwSpaceApp))
   {
      WSTRCPY(szMem, L"?");
   }
   else
   {
      FormatMem(qwSpaceData, szMem, 10, FALSE);
   }
#endif //USE_IDL2_4GB_SUPPORT
   WSPRINTF(szBuff, sizeof(szBuff), szFormat, szMem);
   pText = AppendWStr(pText,szBuff,'\n');

   // Expire String
   *szBuff = 0;
   GetExpireString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, 0, pmi->li.lt, pmi->li.dwExpire, szBuff, sizeof(szBuff), TRUE,FALSE);
   if(*szBuff)
   {
      AECHAR szTemp[32];

      // App License String
      ISHELL_LoadResString(ps, BREWAPPMGRLS_RES_LOC, IDS_APP_LICENSE, szFormat, sizeof(szFormat));
      WSPRINTF(szTemp, sizeof(szTemp), szFormat, szBuff);
      pText = AppendWStr(pText,szTemp,'\n');
   }

   if(pmi->dwLastUsedTime){
      AECHAR szTemp[32];

      // Last Used
      ISHELL_LoadResString(ps, BREWAPPMGRLS_RES_LOC, IDS_APP_LASTUSED, szFormat, sizeof(szFormat));
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
         if(ISHELL_LoadResString(ps, BREWAPPMGRLS_RES_LOC, IDS_LAUNCHER_DISABLED, szBuff, sizeof(szBuff)))
            pText = AppendWStr(pText,szBuff,'\n');
         break;

      case DEMO:
         if(ISHELL_LoadResString(ps, BREWAPPMGRLS_RES_LOC, IDS_LAUNCHER_DEMO, szBuff, sizeof(szBuff)))
            pText = AppendWStr(pText,szBuff,'\n');
         break;

      case EXPIRED:
         if(ISHELL_LoadResString(ps, BREWAPPMGRLS_RES_LOC, IDS_LAUNCHER_EXPIRED, szBuff, sizeof(szBuff)))
            pText = AppendWStr(pText,szBuff,'\n');
         break;

      case EXPIREDDEMO:
         if(ISHELL_LoadResString(ps, BREWAPPMGRLS_RES_LOC, IDS_LAUNCHER_EXPIREDDEMO, szBuff, sizeof(szBuff)))
            pText = AppendWStr(pText,szBuff,'\n');
         break;
   }

   // Display Prompt with Properties Information
   AppMgr_Prompt(pme, AppMgr_GetAppName(pme, pme->m_pCurrentApp), 0, pText, 0, wButtons, wButtons[0], CP_USE_DEFAULT|ST_NOSCROLL, FALSE);

   FreePtr((void**)&pText);

   return TRUE;
}

/*===========================================================================

   State handler for  ST_REMOVE. 

===========================================================================*/
static boolean AppMgr_Remove(AppMgr * pme)
{
   AECHAR*     pTitle;
   uint16      wButtons[3], wRemoveID;  
#ifdef USE_IDL2_4GB_SUPPORT
   DL2AppModInfo* pmi = NULL;
#else
   AppModInfo* pmi = NULL;
#endif
   AEEAppInfo  ai;
   boolean hasShortcut = FALSE;

   if (!AppMgr_GetItemID(pme, pme->m_pCurrentApp))
      return TRUE;

   // Get module information for the current app
#ifdef USE_IDL2_4GB_SUPPORT
   pmi = IDOWNLOAD2_GetModInfoEx(pme->m_pDownload2, AppMgr_GetItemID(pme, pme->m_pCurrentApp), GMI_ALL);
#else
   pmi = IDOWNLOAD_GetModInfoEx(pme->m_pDownload, AppMgr_GetItemID(pme, pme->m_pCurrentApp), GMI_ALL);
#endif

   if (!pmi)
   {
      if (pme->m_pCurrentApp)
         pme->m_pCurrentApp->dwItemID = 0;
      AppMgr_SetState(pme, PREV_ENTRY);
      return TRUE;
   }

   // Get prompt title
   pTitle = LoadAppNames(pme->a.m_pIShell, pmi->apps, pmi->nApps);

   // Note that you can have multiple "apps" in a downloaded item,
   // but everyone believes that only one can be visible, and thus have 
   // a shortcut assigned to it.  So I don't need to iterate through all 
   // of the apps in the pmi->apps[] array to see if any of the other
   // apps have shortcuts. Tianyu D'amore, tianyu, had this to say
   // "Yeah I think BDS prevents you from having more than one non-hidden applet.
   // Also BREW code itself won抰 really handle it either.   It would be a nice 
   // feature (like the midlet suite concept) but don抰 think we support it."
#ifdef FEATURE_INTERACT_WITH_SA_APP
   hasShortcut = DoesAppHaveShortcut(pme->m_pIfm, pme->m_pCurrentApp->cls);
#endif

   // Check if request is for screen saver
   AppMgr_GetAEEAppInfo(pme, pme->m_pCurrentApp->cls, &ai);

   if (ai.wFlags & AFLAG_SCREENSAVER)
      wRemoveID = (hasShortcut)? IDS_SS_REMOVE_SHORTCUT : IDS_SS_REMOVE;
   else
   {
      AppStatus   nAppStatus = AppMgr_AppStatus(pme, AppMgr_GetItemID(pme, pme->m_pCurrentApp));

      if ((nAppStatus == EXPIRED) || (nAppStatus == EXPIREDDEMO))
         wRemoveID = (hasShortcut) ? IDS_MT_REMOVE_EXPIRED_SHORTCUT : IDS_MT_REMOVE_EXPIRED;
      else
      {   
         if (hasShortcut)
            wRemoveID = (pmi->li.pt == PT_SUBSCRIPTION) ? IDS_MT_CANCEL_SUBSCRIPTION_SHORTCUT : IDS_MT_REMOVE_SHORTCUT;
      else
         wRemoveID = (pmi->li.pt == PT_SUBSCRIPTION) ? IDS_MT_CANCEL_SUBSCRIPTION : IDS_MT_REMOVE;
   }
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
   uint64      qwUsed, qwTotal, qwFree;
   AECHAR      szBuff[32], szFormat[32];
   IDisplay *  pd;
   int         cyFont, wPct;
#ifdef USE_IDL2_4GB_SUPPORT
   DL2ItemSize  is;
#else
   DLItemSize  is;
#endif

   pd = pme->m_pIDisplay;
   cyFont = pme->m_cyFont;

   SETAEERECT(&rc, 0, 0, pme->m_rc.dx, pme->m_rc.dy);

   // Draw the title on first line in the center
   ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, IDC_OPT_INFO,
      szBuff, sizeof(szBuff));
   IDISPLAY_DrawText(pd, AEE_FONT_BOLD, szBuff, -1, 0, rc.y, &rc,
      IDF_ALIGN_CENTER);

   // Underline title
   rc.y += cyFont + 1;
   rc.dy = 1;
   IDISPLAY_FillRect(pd, &rc, CLR_USER_FRAME);

   // Draw the memory information
   rc.y += 1;
   rc.dy = cyFont;

   // Get Available Space
   // TODO: Maybe move to IDOWNLOAD2 usage for 64 bit info
#ifdef USE_IDL2_4GB_SUPPORT
   IDOWNLOAD2_GetAvailable(pme->m_pDownload2, &is);

   // Get total space (we don't want to round off, DrawMemHist and FormatMem should take care of it)
   qwTotal = is.qwFSSize;

   // Get free space (we don't want to round off, DrawMemHist and FormatMem should take care of it)
   qwFree = is.qwFSAvail;
#else
   // This is per logic specified by the IDOWNLOAD team of
   // How to figure out the free space since it is not fully
   // supported in 3.1.4 version of IDOWNLOAD
   //
   // --Begin Logic
   //IDOWNLOAD_GetAvailable()
   //if (value is UINT32_MAX) 
   //  size1= call ISHELL_GetDeviceInfoEx
   //  size 2= IDOWNLOAD_GetConfigItem(CFGI_DOWNLOAD_FS_INFOEX)
   //  if (GetConfigItem() does not return SUCCESS) 
   //    size2= call IDOWNLOAD_GetConfigItem(CFGI_DOWNLOAD_FS_INFO)
   //  if size2 is available from OEM
   //    take min(size1, size2)
   //  else 
   //    take size1
   // -- End Logic

   IDOWNLOAD_GetAvailable(pme->m_pDownload, &is);

   if (is.dwFSSize == MAX_UINT32)
   {
      // We are on a 4 GB+ system and cannot always trust the IDOWNLOAD values
      AEEFSFreeSpace fsFreeSpace;   // size 1 in logic above
      AEEFSFreeSpace fsConfigItemFreeSpace;
      int nSize = sizeof(fsFreeSpace);

      // Initialize fsConfigItemFreeSpace to be what IDownload returns.
      fsConfigItemFreeSpace.qwTotalSpace = is.dwFSSize;
      fsConfigItemFreeSpace.qwFreeSpace = is.dwFSAvail;


      if (SUCCESS != ISHELL_GetDeviceInfoEx(pme->a.m_pIShell, AEE_DEVICEITEM_FSSPACE_ROOT, (void*)&fsFreeSpace, &nSize))
      {
         // We cannot use the IShell values, they are not in the build
         // so copy the IDOWNLOAD values
         fsFreeSpace.qwFreeSpace = is.dwFSAvail;
         fsFreeSpace.qwTotalSpace = is.dwFSSize;
      }
      if (SUCCESS != IDOWNLOAD_GetConfigItem(pme->m_pDownload, CFGI_DOWNLOAD_FS_INFOEX, &fsConfigItemFreeSpace, sizeof(fsConfigItemFreeSpace)))
      {
         if (SUCCESS == IDOWNLOAD_GetConfigItem(pme->m_pDownload, CFGI_DOWNLOAD_FS_INFO, &is, sizeof(is)))
         {
            fsConfigItemFreeSpace.qwTotalSpace = is.dwFSSize;
            fsConfigItemFreeSpace.qwFreeSpace = is.dwFSAvail;
            
         }
      }


      // use the min of the Device values and the values returned by FS_INFO, as the 
      // OEM may have limited the size available for download.
      if (fsConfigItemFreeSpace.qwTotalSpace != 0)
      {
         fsFreeSpace.qwTotalSpace = MIN(fsFreeSpace.qwTotalSpace, fsConfigItemFreeSpace.qwTotalSpace);
         fsFreeSpace.qwFreeSpace = MIN(fsFreeSpace.qwFreeSpace, fsConfigItemFreeSpace.qwFreeSpace);
      }


      // Get total space 
      qwTotal = fsFreeSpace.qwTotalSpace;

      // Get free space 
      qwFree = fsFreeSpace.qwFreeSpace;

   }
   else
   {
      // Get total space
      qwTotal = is.dwFSSize;

      // Get free space
      qwFree = is.dwFSAvail;
   }
#endif

   // Calculate used space
   qwUsed = qwTotal - qwFree;

   if(qwTotal)
      wPct = (uint16)(((qwUsed / ONE_K) * 100) / (qwTotal / ONE_K));
   else
      wPct = 100;

   // Display "Memory wPct% Full"
   ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC,
      IDS_MEM_FORMAT, szFormat, sizeof(szFormat));
   WSPRINTF(szBuff, sizeof(szBuff), szFormat, wPct);
   IDISPLAY_DrawText(pd,AEE_FONT_BOLD, szBuff, -1, 0, rc.y, &rc,
      IDF_ALIGN_CENTER);

   // Draw memory histogram
   rc.y += cyFont + 1;
   rc.x = 5;
   rc.dx = pme->m_rc.dx - 10;
   rc.dy = cyFont;
   AppMgr_DrawMemHist(pme, qwUsed, qwTotal, &rc);

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
static void AppMgr_DrawMemHist(AppMgr * pme, uint64 qwUsed, uint64 qwTotal,
   AEERect * prc)
{
   IDisplay *  pd = pme->m_pIDisplay;
   AECHAR      szBuff[32];
   AECHAR      szTotal[10];
   AECHAR *    psz;
   uint16      wPct;
   AEERect     rc;

   rc = *prc;

   // Format the memory strings
   FormatMem(qwUsed, szBuff, 32, FALSE);
   FormatMem(qwTotal, szTotal, 10, FALSE);

   psz = szBuff + WSTRLEN(szBuff);
   *(psz++) = (AECHAR)' ';
   *(psz++) = (AECHAR)'/';
   *(psz++) = (AECHAR)' ';
   WSTRCPY(psz,szTotal);

   // Draw the memory histogram...
   qwUsed /= ONE_K;
   qwTotal /= ONE_K;

   if(qwTotal)
      wPct = (uint16)((qwUsed * 100) / qwTotal);
   else
      wPct = 100;

   DrawHist(pd, &rc, wPct);

   // Draw the memory string as "memory used"/"memory available"

   rc.y += rc.dy + 2;
   rc.x = 0;
   rc.dx = pme->m_rc.dx;
   rc.dy = pme->m_cyFont;

   *prc = rc;

   IDISPLAY_DrawText(pd,AEE_FONT_NORMAL, szBuff, -1, 0, rc.y, &rc,
      IDF_ALIGN_CENTER);
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
//
// DESCRIPTION
//    Function for determing Menu size when in state ST_EDITSOUNDIMAGE
// 
// PARAMETERS
//    pme:  AppMgr
//    prc:  AEERect*
//
//===========================================================================
static boolean AppMgr_GetMenuRectForShowFiles(AppMgr* pme, AEERect* prc)
{
   if ((NULL == pme) || (NULL == prc))
   {
      return FALSE;
   }

   // Set Menu Size
   SETAEERECT(prc, pme->m_rc.x, pme->m_rc.y, pme->m_rc.dx, pme->m_rc.dy);

   return TRUE;
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

   // If we are in the process of switching displays because of a flip
   // don't modify any of the state or change the values  in the controls
   // as we already have the data we need.  We just need to refresh the display
   // properly
   if (!pme->m_bRedrawingUI)
   {
      // Reset File List
      pme->m_nFileCount = 0;
      if(pme->m_pFileList) 
      {
         FREE(pme->m_pFileList);
         pme->m_pFileList = NULL;
      }

      // Disable Softkey Menu
      IMENUCTL_SetActive(pme->m_pSK, FALSE);
   } // if (!pme->m_bRedrawingUI)
   // Clear Display
#ifdef USE_TT_FONTS
   // When TrueType fonts are enabled stuff gets included from
   // BUIW that makes ClearScreen misbehave.  I think.  Maybe.
   IDISPLAY_FillRect(pme->m_pIDisplay, NULL, RGB_WHITE);
#else
      //IDISPLAY_ClearScreen(pme->m_pIDisplay);
   IDISPLAY_EraseRect(pme->m_pIDisplay, &pme->m_rc);
#endif // USE_TT_FONTS

   // If we are in the process of switching displays because of a flip
   // don't modify any of the state or change the values  in the controls
   // as we already have the data we need.  We just need to refresh the display
   // properly
   if (!pme->m_bRedrawingUI)
   {
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
   } //if (!pme->m_bRedrawingUI)

   if (IMENUCTL_GetItemCount(pme->m_pMenu))
   {
      AEERect rc;

      // Set Title
      IMENUCTL_SetTitle(pme->m_pMenu, BREWAPPMGRLS_RES_LOC,(uint16)(bImage ? IDC_CONFIGURE_IMAGE : IDC_CONFIGURE_SOUND), NULL);
      // Store the menu's title for use with flipping the cover
      AppMgr_StoreMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, (uint16)(bImage ? IDC_CONFIGURE_IMAGE : IDC_CONFIGURE_SOUND), NULL);

      // Sort Menu
      IMENUCTL_Sort(pme->m_pMenu, MCS_NAME_ASCENDING);

      // Set Size
      AppMgr_GetMenuRectForShowFiles(pme, &rc);
      IMENUCTL_SetRect(pme->m_pMenu, &rc);

      // Set Menu Active
      IMENUCTL_SetActive(pme->m_pMenu, TRUE);
   }
   else
   {
      AECHAR szBuf[25];
      if (ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC,
         IDS_CONFIGURE_NO_FILES, szBuf, sizeof(szBuf)))
      {
         IDISPLAY_DrawText(pme->m_pIDisplay, AEE_FONT_NORMAL,szBuf, -1,0,0,0, IDF_ALIGN_CENTER);
      }
   }

   // Update UI
   IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
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

   if (!pImage  && pIShell) {
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
void AppMgr_LargeIconViewSettings(AppMgr* pme, IMenuCtl* pMenu, IMenuCtl* pSKMenu)
{
   AEEItemStyle normal, select;
   AEEMenuColors  col;
   
   // Menu Properties - Text On Top and No Arrows
   IMENUCTL_SetProperties(pMenu, MP_ICON_SINGLE_FRAME|MP_NO_ARROWS|MP_ICON_ANIMATED);

   // Main Menu Softkey - Properties
   IMENUCTL_SetProperties(pSKMenu, MP_MAXSOFTKEYITEMS);
   IMENUCTL_EnableCommand(pSKMenu, FALSE);
   // Store the Main Menu SK's title for use with flipping the cover
   AppMgr_StoreMainMenuSKEnableCommandInfo(pme, FALSE);

   // Remove offset to make more room for title
   ISHELL_GetItemStyle(pme->a.m_pIShell,AEE_IT_ICONVIEW,&normal,&select);
   normal.yOffset = select.yOffset = 0;
   IMENUCTL_SetStyle(pMenu,&normal,&select);

   col.wMask = MENU_COLOR_MASK | MC_FRAME;

   if(pme->m_nColorDepth > 2) {

      col.cBack     = COLOR_ICON_BACKGROUND;
      col.cSelBack  = COLOR_ICON_SELECT_BACKGROUND;
      col.cSelText  = COLOR_ICON_SELECT_TEXT;
      col.cFrame    = COLOR_ICON_BACKGROUND;
      IMENUCTL_SetColors(pMenu,&col);

      col.cBack     = MAKE_RGB(23,0,94);
      col.cSelBack  = COLOR_SK_SELECT_BACKGROUND;
      col.cSelText  = COLOR_SK_SELECT_TEXT;
      col.cFrame    = COLOR_SK_SELECT_BACKGROUND;
      IMENUCTL_SetColors(pSKMenu,&col);
   }
   else
   {
      // Same colors for menu/softkey/icon view
      col.cBack =             MENU_BACKGROUND;
      col.cSelBack =          MENU_SELECT_BACKGROUND;
      col.cSelText =          MENU_SELECT_TEXT;
      col.cFrame =            MENU_BACKGROUND;
      IMENUCTL_SetColors(pMenu,&col);
      IMENUCTL_SetColors(pSKMenu,&col);
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
#ifdef USE_IDL2_4GB_SUPPORT
         DL2AppModInfo* pai;
#else
         AppModInfo* pai;
#endif

         //Its not a valid app (with valid license), no point checking for the license
         if (dw >= FAKE_MODULE_ID)
            continue;

#ifdef USE_IDL2_4GB_SUPPORT
         pai = IDOWNLOAD2_GetModInfoEx(pme->m_pDownload2, dw, GMI_ALL);
#else
         pai = IDOWNLOAD_GetModInfoEx(pme->m_pDownload, dw, GMI_ALL);
#endif

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


//===========================================================================
//
// DESCRIPTION
//    Fills in the AEEAppInfo structure for the given classID.
//
// PARAMETERS
//    pIShell:  IShell *
//    pIdl2:    IDownload2 *
//    cls:      AEECLSID to query for
//    pai:      AEEAppInfo * that will be filled in on success
//
//  Return Value: boolean
//===========================================================================
boolean AppMgr_GetAEEAppInfo(AppMgr* pme, AEECLSID cls, AEEAppInfo* pai)
{
  if ((NULL == pme) || (NULL == pai))
  {
    return FALSE;
  }
  
  if (!ISHELL_QueryClass(pme->a.m_pIShell, cls, pai))
  {
    boolean found = FALSE;
#ifdef USE_IDL2_DISABLED_SUPPORT
    DL2AppModInfo * pModInfo = NULL;
    if (0 != GetItemIDFromClassID(pme->m_pDownload2, cls, &pModInfo))
    {
      int i = 0;
      
      for (i = 0; i < pModInfo->nApps; ++i)
      {
        if (pModInfo->apps[i].cls == cls)
        {
          MEMCPY(pai, &(pModInfo->apps[i]), sizeof(AEEAppInfo));
          found = TRUE;
          break;
        }
      }
    }
#endif
    return found;
  }
  
  return TRUE;
}









#ifdef USE_TT_FONTS

//===========================================================================
//
// DESCRIPTION
//    Installs TrueType fonts for use by the AppMgr, saving the previously
//    installed fonts for later restoration.
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================

static void AppMgr_InstallTTFonts(AppMgr* pme)
{
   ITypeface* pTypeFace = NULL;
   IShell* pIShell = pme->a.m_pIShell;
   IHFont* pNormalFont = NULL;
   IHFont* pBoldFont = NULL;
   if (SUCCESS == CreateInstance(pIShell, AEECLSID_TYPEFACE,
      (void**)&pTypeFace))
   {
      // Create and install the normal font
      if (SUCCESS == ITYPEFACE_NewFontFromClassID(pTypeFace,
         APPMGR_NORMAL_FONT_CLSID, APPMGR_TT_FONTSIZE, &pNormalFont))
      {
         // Install the font and save the original.
         pme->m_pSaveNormalFont = CAST(IHFont*,
            IDISPLAY_SetFont(pme->m_pIDisplay, AEE_FONT_NORMAL,
               CAST(IFont*, pNormalFont)));
         if (NULL == pme->m_pSaveNormalFont)
         {
            FARF(BAM, ("Error installing normal font"));
            pme->m_bNormalFontSaved = FALSE;
            // Release the new normal font since we didn't install it
            // FreeObj(pNormalFont);
            IHFONT_Release(pNormalFont);
         }
         else
         {
            pme->m_bNormalFontSaved = TRUE;
         }
      }
      else
      {
         FARF(BAM, ("Error creating normal font from class ID"));
      }

      // IFF the normal font installed OK do the same for bold
      if ((TRUE == pme->m_bNormalFontSaved) &&
         (SUCCESS == ITYPEFACE_NewFontFromClassID(pTypeFace,
            APPMGR_BOLD_FONT_CLSID, APPMGR_TT_FONTSIZE, &pBoldFont)))
      {
         // Install the font and save the original.
         pme->m_pSaveBoldFont = CAST(IHFont*,
            IDISPLAY_SetFont(pme->m_pIDisplay, AEE_FONT_BOLD,
               CAST(IFont*, pBoldFont)));
         if (NULL == pme->m_pSaveBoldFont)
         {
            FARF(BAM, ("Error installing bold font"));
            pme->m_bBoldFontSaved = FALSE;
            // Release the new bold font since we didn't install it
            IHFONT_Release(pBoldFont);
         }
         else
         {
            pme->m_bBoldFontSaved = TRUE;
         }
      }
      else
      {
         FARF(BAM, ("Error creating bold font from class ID"));
      }
      ITYPEFACE_Release(pTypeFace);
   }
   else
   {
      FARF(BAM, ("Error instantiating ITypeface object"));
   }
}






//===========================================================================
//
// DESCRIPTION
//    Restores the fonts saved by InstallTTFonts()
// 
// PARAMETERS
//    pme:  AppMgr
//
//===========================================================================

static void AppMgr_RestoreFonts(AppMgr* pme)
{
   // The flags m_bNormalFontSaved and m_bBoldFontSaved tell us which
   // of the two fonts need to be restored.  Note that we might be
   // restoring a NULL IFont*.  If that's what was there on startup
   // that's what we put back.  The fonts that get de-installed here
   // are those we instantiated and installed earlier so we can release
   // them.

   IHFont* pFont = NULL;

   if (TRUE == pme->m_bNormalFontSaved)
   {
      pFont = CAST(IHFont*, IDISPLAY_SetFont(pme->m_pIDisplay,
         AEE_FONT_NORMAL, CAST(IFont*, pme->m_pSaveNormalFont)));
      // We don't expect the result to be NULL, but ...
      if (NULL != pFont)
      {
         IHFONT_Release(pFont);
         pme->m_bNormalFontSaved = FALSE;
      }
      else
      {
         FARF(BAM, ("Unexpected NULL normal font at Restore() time"));
      }
   }

   if (TRUE == pme->m_bBoldFontSaved)
   {
      pFont = CAST(IHFont*, IDISPLAY_SetFont(pme->m_pIDisplay,
         AEE_FONT_BOLD, CAST(IFont*, pme->m_pSaveBoldFont)));
      // We don't expect the result to be NULL, but ...
      if (NULL != pFont)
      {
         IHFONT_Release(pFont);
         pme->m_bBoldFontSaved = FALSE;
      }
      else
      {
         FARF(BAM, ("Unexpected NULL bold font at Restore() time"));
      }
   }
}

#endif // USE_TT_FONTS

/*===========================================================================

  BAM URL Format

  BAM:ListApps
  BAM:Shortcut=<clsid>&active=<true|false>

===========================================================================*/
static boolean AppMgr_Browse(AppMgr * pme, const char * url)
{
   char*          pszTemp                 = NULL;
   char*          pszArg                  = NULL;
   char*          pszVal                  = NULL;
   char*          pszArg2                 = NULL;
   char*          pszVal2                 = NULL;
   char*          pszNext                 = NULL;
   AEECLSID       cls                     = 0;
   boolean        hasShortcut             = FALSE;
   int            nErr                    = AEE_SUCCESS;
   boolean        bHandled                = FALSE;
 
   // No URL - Nothing to do
   if(!url)
      return FALSE;

   // Duplicate the URL as we will modify it
   pszTemp = STRDUP(url);
   if(!pszTemp)
      return FALSE;

   FARF(BAM, ("Got Brew SMS of %s", url));
   pszArg = STRCHR(pszTemp, APPMGR_CMD_COLON); // Get URL devoid of BAM
   pszArg++; // Ignore ':', '0'

   // Do We need this for BAM stuff?
/*
   // Test cmshop command validity for various security macros
#ifdef CMSHOP_SECURE_ALL
   if (!MShop_ValidateCmshopCmd(pme, pszArg)) {
      FREEIF(pszTemp);
      return; // Failed security check - do nothing
   }
#elif defined CMSHOP_SECURE_REMOVE
   if (!STRNCMP(pszArg, CMSHOP_CMD_REMOVE, STRLEN(CMSHOP_CMD_REMOVE))) {
      if (!MShop_ValidateCmshopCmd(pme, pszArg)) {
         FREEIF(pszTemp);
         return; // Failed security check - do nothing
      }
   }
#endif //CMSHOP_SECURE_REMOVE 
*/

   // Process all of the arguments
   while(pszArg){
      //Multiple commands not used in BrewAppMgr.c, but we'll preserve ability to handle them:
      pszNext = STRCHR(pszArg, APPMGR_CMD_SEMICOLON); // Get next in ';' separated list

      if (pszNext) {
         *pszNext = 0;  // Overwrite ';' with NULL leaving pszArg as NULL terminated 
                        // single entry string "<action>=<value>" string 
      }

      // See if there is a second argument
      pszArg2 = STRCHR(pszArg, APPMGR_CMD_AMPERSAND);
      if (pszArg2) {
         *pszArg2 = 0;
         pszArg2++;
      }

      // Look for value to arg 1
      pszVal = STRCHR(pszArg, APPMGR_CMD_EQUAL);  // Get value
      if(pszVal){
         *pszVal = 0;               // Overwrite '=' leaving pszArg as NULL terminated "<action>" string
         pszVal++;                  // Ignore '='
      }

      // Look for value to arg 2
      if (pszArg2)
      {
         pszVal2 = STRCHR(pszArg2, APPMGR_CMD_EQUAL);  // Get value
         if(pszVal2)
         {
            *pszVal2 = 0;               // Overwrite '=' leaving pszArg as NULL terminated "<action>" string
            pszVal2++;                  // Ignore '='
         }
      }

      if(!STRICMP(pszArg, APPMGR_CMD_LISTAPPS)) {
#ifdef FEATURE_INTERACT_WITH_SA_APP
         // We need to load the list of apps that we know about, then
         // We need to create the Apps that we manage, 
         // And post the "AppsUpdated" SMS to the ShortcutApp
         AppMgr_LoadAppletsInfo(pme);
         (void)AppMgr_WriteAppList(pme);


         // This is now always done at startup
//#ifdef FEATURE_OEMCFG
//         AppMgr_LoadOEMCfg(pme);
//#endif

         if (pme->m_szSMSAppUrl)
         {
            nErr = ISHELL_PostURL(pme->a.m_pIShell, pme->m_szSMSAppUrl);
            FARF(BAM, ("PostUrl(%s) returned %d", pme->m_szSMSAppUrl, nErr));
         }
         bHandled = TRUE;
#endif //FEATURE_INTERACT_WITH_SA_APP
      }
      else if (!STRICMP(pszArg, APPMGR_CMD_SHORTCUT)) {
#ifdef FEATURE_INTERACT_WITH_SA_APP
         if (pszVal && pszVal2) 
         {

            // Read in the list of apps that we have.
            AppMgr_LoadAppletsInfo(pme);

            cls = STRTOUL(pszVal,NULL,10);
            hasShortcut = (STRICMP(pszVal2, APPMGR_CMD_TRUE))? FALSE:TRUE; // STRICMP return 0 if they match
            
            // Update our internal version of what apps have shortcuts
            AppMgr_SetAppShortcutStatus(pme, cls, hasShortcut);

            //Re-write the app list
            (void)AppMgr_WriteAppList(pme);

            // This is now always done at startup
//#ifdef FEATURE_OEMCFG
//            AppMgr_LoadOEMCfg(pme);
//#endif

            // And post the "AppsUpdated" SMS to the ShortcutApp
            if (pme->m_szSMSAppUrl)
            {
               nErr = ISHELL_PostURL(pme->a.m_pIShell, pme->m_szSMSAppUrl);
               FARF(BAM, ("PostUrl(%s) returned %d", pme->m_szSMSAppUrl, nErr));
            }
            bHandled = TRUE;
         }
         else
         {
            // What do you do with a poorly formated request?
         }
#endif //FEATURE_INTERACT_WITH_SA_APP
      }
      else if (!STRICMP(pszArg, APPMGR_CMD_LAUNCHAPP)) {
#ifdef FEATURE_INTERACT_WITH_SA_APP
         if (pszVal)
         {
            uint16 nCatID = 0;

            // Read in the list of apps that we have.
            AppMgr_LoadAppletsInfo(pme);

            cls = STRTOUL(pszVal, NULL, 10);

            FARF(BAM, ("BAM got a launch app request for clsID %d", cls));
            // See if we can find the app we are being launched to find
            nCatID = AppMgr_FindItemCategory(pme, cls, &pme->m_pCurrentApp);

            if (0 != nCatID)
            {
               FARF(BAM, ("BAM is going to launch app with clsid of %d.", cls));
               // We know about the app, And it is now the CurrentApp,
               // So launch it.
               (void)AppMgr_LaunchCurrentApplet(pme, FALSE);
            }
            else
            {
               // We don't know about this app, so just call StartAppllet
               FARF(BAM, ("BAM does not know about this app so we are just going to launch it"));
               nErr = ISHELL_StartApplet(pme->a.m_pIShell, cls);               
               FARF(BAM, ("BAM ISHELL_StartAppliet() returned %d.", nErr));
            }
            bHandled = TRUE;
         }
         else
         {
            // What do you do with a poorly formated request?
         }
#endif
      }

      pszArg = pszNext;
   }
   FREEIF(pszTemp);

   return bHandled;
}


//===========================================================================
//
// DESCRIPTION
//    Figure out the keys that we need to pay attention to, for this display
//    This could mean useing the standard up/down/select/clr keys or maybe
//    something like FF/REW/Play/Stop
//
//===========================================================================
static boolean AppMgr_SetKeyList(AppMgr* pme, IFlip* pIFlip, AEECLSID cls)
{
   AVKType     *pKeyList = NULL;
   int32       dwNumKeys = 0;
   int16       displayIndex = 0;
   int16       i = 0;
   IFlip       *pLocalIFlip = NULL;

   // Default to display/phone has a DPad
   pme->m_bHasDPad = TRUE;

#ifdef FEATURE_LESS_THAN_6_KEYS
   pme->m_bUseAltKeys = FALSE;
   pme->m_wAltKeyUp = AVK_UP;
   pme->m_wAltKeyDown = AVK_DOWN;
   pme->m_wAltKeySelect = AVK_SELECT;
   pme->m_wAltKeyClr = AVK_CLR;
#endif //FEATURE_LESS_THAN_6_KEYS

   if (NULL == pIFlip)
   {
      CreateInstance(pme->a.m_pIShell, AEECLSID_FLIP, (void **)&pLocalIFlip);
      pIFlip = pLocalIFlip;
   }

   if (NULL == pIFlip)
   {
      FARF(BAM, ("AppMgr_SetKeyList was not passed an IFlip and we were unable to create one"));
      return FALSE;
   }

   if (0 == cls)
   {
      int32    dwNumDisplays = 0;
      AEECLSID *pClsList = NULL;
      if ((SUCCESS == IFLIP_GetDisplayList(pIFlip, NULL, &dwNumDisplays)) && (dwNumDisplays > 0))
      {
         pClsList = (AEECLSID*) MALLOC(sizeof(AEECLSID) * dwNumDisplays);

         if (NULL != pClsList)
         {
            if (SUCCESS == IFLIP_GetDisplayList(pIFlip, (void*)pClsList, &dwNumDisplays))
            {
               cls = pClsList[0];
            }
         }
      }

      FreePtr((void**)&pClsList);
   }

   if (0 == cls)
   {
      FARF(BAM, ("AppMgr_SetKeyList was not passed a display and was not able to get one, defaulting to Display1"));
      cls = AEECLSID_DISPLAY1;
   }


   
   switch (cls)
   {
      case AEECLSID_DISPLAY1:
         FARF(BAM, ("AppMgr_SetKeyList: display 1"));
         displayIndex = 0;
         break;
      case AEECLSID_DISPLAY2:
         FARF(BAM, ("AppMgr_SetKeyList: display 2"));
         displayIndex = 1;
         break;
      case AEECLSID_DISPLAY3:
         FARF(BAM, ("AppMgr_SetKeyList: display 3"));
         displayIndex = 2;
         break;
      case AEECLSID_DISPLAY4:
         FARF(BAM, ("AppMgr_SetKeyList: display 4"));
         displayIndex = 3;
         break;
      default:
         FARF(BAM, ("AppMgr_SetKeyList: Invalid display index"));
         FreeObj((void**)&pLocalIFlip);
         return FALSE;
   }

   if (SUCCESS == IFLIP_GetKeyList(pIFlip, NULL, &dwNumKeys))
   {
      FARF(BAM, ("GetKeyList is stating that there are %d numKeys", dwNumKeys));
      pKeyList = (AVKType*)MALLOC(sizeof(AVKType) * dwNumKeys);

      if (SUCCESS == IFLIP_GetKeyList(pIFlip, (int32*)pKeyList, &dwNumKeys))
      {
         boolean bHasUp = FALSE;
         boolean bHasDown = FALSE;
         boolean bHasSelect = FALSE;
         boolean bHasClr = FALSE;

         for( i = 0; i < dwNumKeys; ++i)
         {
            //FARF(BAM, ("Key %d of %d is %d", i, dwNumKeys, pKeyList[i]));
            switch(pKeyList[i])
            {
            case AVK_UP:
               bHasUp = TRUE;
               break;
            case AVK_DOWN:
               bHasDown = TRUE;
               break;
            case AVK_SELECT:
               bHasSelect = TRUE;
               break;
            case AVK_CLR:
               bHasClr = TRUE;
               break;
            }
         }

         if (!bHasUp || !bHasDown || !bHasSelect || !bHasClr)
         {

            pme->m_bHasDPad = FALSE;

#ifdef FEATURE_LESS_THAN_6_KEYS
            // Bring in the mappings
            pme->m_bUseAltKeys = TRUE;
            pme->m_wAltKeyUp = pme->m_wAltKeyUpList[displayIndex];
            pme->m_wAltKeyDown = pme->m_wAltKeyDownList[displayIndex];
            pme->m_wAltKeySelect = pme->m_wAltKeySelectList[displayIndex];
            pme->m_wAltKeyClr = pme->m_wAltKeyClrList[displayIndex];

            FARF(BAM, ("We are using alt keys of %d, %d, %d, %d", pme->m_wAltKeyUp, 
               pme->m_wAltKeyDown, pme->m_wAltKeySelect, pme->m_wAltKeyClr));
#endif //FEATURE_LESS_THAN_6_KEYS
         }

         FreePtr((void**)&pKeyList);
         FreeObj((void**)&pLocalIFlip);
         return TRUE;
      }
   }

   FreeObj((void**)&pLocalIFlip);
   FreePtr((void**)&pKeyList);

   return FALSE;
}

#ifdef FEATURE_INTERACT_WITH_SA_APP
static boolean AppMgr_SetAppShortcutStatus(AppMgr* pme, AEECLSID cls, boolean hasShortcut)
{
   AppMgrItem     *pList = NULL;
   AppMgrItem     *p = NULL;
   uint16         i;
   uint16         count = 0;
   uint16         nCatIndex;

   // Go through all of the apps that we have, by category
   // looking for a matching cls id
   // when/if we find it, set its hasShortcut value
   for (nCatIndex = 0; nCatIndex < MAX_APP_CATS; ++nCatIndex)
   {
      count = pme->m_nItem[nCatIndex];
      p = pList = pme->m_pList[nCatIndex];
      if ((0 == count) || (NULL == pList))
      {
         continue;
      }

      // Set the shortcut status
      for (i = 0; (p != NULL) && i < count; ++i, p = p->pNext)
      {
         // we found the cls ID, now set its shortcut status
         if (p->cls == cls)
         {
            p->bHasShortcut = hasShortcut;
            return TRUE;
         }   
      }
   }

   // We did not find the app
   return FALSE;
}



static boolean AppMgr_WriteAppList(AppMgr* pme)
{
   AppMgrItem     *pList = NULL;
   AppMgrItem     *p = NULL;
   uint16         i;
   uint16         count = 0;
   uint16         nCatIndex;
   IFile          *pIFile = NULL;

   // Open the file, and truncate it

   pIFile = IFILEMGR_OpenFile(pme->m_pIfm, APPLIST_FILE, _OFM_READWRITE);   
   if (NULL == pIFile)
   {
      pIFile = IFILEMGR_OpenFile(pme->m_pIfm, APPLIST_FILE, _OFM_CREATE);
      if (NULL == pIFile)
      {
         FARF(BAM, ("Unable to crete AppList file %s, result %d", APPLIST_FILE, IFILEMGR_GetLastError(pme->m_pIfm)));
         return FALSE;
      }
   }

   (void)IFILE_Truncate(pIFile, 0);



   // Go through all of the apps that we have, by category
   // and write them to the AppList
   for (nCatIndex = 0; nCatIndex < MAX_APP_CATS; ++nCatIndex)
   {
      count = pme->m_nItem[nCatIndex];
      p = pList = pme->m_pList[nCatIndex];
      if ((0 == count) || (NULL == pList))
      {
         continue;
      }

      // Write this app to the file
      for (i = 0; (p != NULL) && i < count; ++i, p = p->pNext)
      {
         // Don't add MobileShop to the list of apps
         if (p->cls == AEECLSID_MOBILESHOP_BID)
            continue;
         if (p->cls == AEECLSID_BREWAPPMGR_BID)
            continue;

         if (!WriteAppListLine(pIFile, AppMgr_GetAppName(pme, p), p->cls, p->bHasShortcut, p->dwNumLaunches, p->dwLastLaunch))
         {
            FARF(BAM, ("Unable to Write to AppList %d", IFILEMGR_GetLastError(pme->m_pIfm)));
         }

      }
   }

   // Close the file
   IFILE_Release(pIFile);
   return TRUE;
}
#endif //FEATURE_INTERACT_WITH_SA_APP

#ifdef FEATURE_SCREEN_ROTATE

//===========================================================================
//
// Handle the EVT_SCR_ROTATE event.  
//
//===========================================================================
static boolean AppMgr_HandleScreenRotate(AppMgr* pme, uint16 wParam, uint32 dwParam)
{
   

   if (!AppMgr_RotateScreen(pme))
   {
      FARF(BAM, ("Failed to Rotate Screen, returning FALSE for Event"));
      return FALSE;
   }
    
   // We have successfully rotated the display, now redraw it   
   AppMgr_RedrawDisplay(pme);

   return TRUE;
}

//===========================================================================
//
// Rotate the screen.  
//
//===========================================================================
static boolean AppMgr_RotateScreen(AppMgr* pme)
{
   int nCurrent = 0;
   int nAfter = 0;
   int rotateValue = 0;
   char buf[10];
   uint32 nErr = SUCCESS;
   AEEScrOrientation scrOrent;
   int size = sizeof(AEEScrOrientation);
   IBitmapRot* pIBitmapRot = NULL;
   IBitmap* pIBitmap = NULL;
   
   MEMSET(buf, '\0', 10);
  
   nErr = ISHELL_GetDeviceInfoEx(pme->a.m_pIShell,
                                 AEE_DEVICESTATE_SCR_ORIENTATION, &scrOrent, &size);

   if (SUCCESS != nErr)
   {
      // We cannot determine our orientation, so we cannot rotate
      FARF(BAM, ("GetDeviceInfoEx(AEE_DEVICESTATE_SCR_ORIENTATION) returned error %d", nErr));
      return FALSE;
   }

   switch (scrOrent)
   {
      case AEE_SO_LANDSCAPE:
         FARF(BAM, ("Our Screen has been rotated to SCREEN_ORIENTATION_LANDSCAPE mode"));
         if (AEE_SO_LANDSCAPE == pme->m_orentDisplayDefault)
         {
            rotateValue = 0;
         }
         else
         {
            rotateValue = 90;
         }
         break;

      case AEE_SO_PORTRAIT:
      default:
         FARF(BAM, ("Our Screen has been rotated to SCREEN_ORIENTATION_PORTRAIT mode"));
         if (AEE_SO_PORTRAIT == pme->m_orentDisplayDefault)
         {
            rotateValue = 0;
         }
         else
         {
            rotateValue = 90;
         }
         break;
   }

   nErr = IDISPLAY_GetDeviceBitmap(pme->m_pIDisplay, &pIBitmap);

   if (nErr || (NULL == pIBitmap))
   {
      FARF(BAM, ("Error Getting Device Bitmap, %d", nErr));
      return FALSE;
   }

   nErr = IBITMAP_QueryInterface(pIBitmap, AEEIID_BITMAPROT, (void**)&pIBitmapRot);

   if (nErr || (NULL == pIBitmapRot))
   {
      FARF(BAM, ("Error Getting IBitmapRot, %d", nErr));
      FreePtr((void**)&pIBitmap);
      return FALSE;
   }

   nCurrent = IBITMAPROT_GetRotation(pIBitmapRot);
   FARF(BAM, ("Our old rotation was %d, and our new rotation will be %d", nCurrent, rotateValue));

   if (nCurrent == rotateValue)
   {
      // We don't need to do any rotation, as we are already
      // on the proper rotation.  But still return TRUE;
      FreePtr((void**)&pIBitmapRot);
      FreePtr((void**)&pIBitmap);
      return TRUE;
   }

   SNPRINTF(buf, 10, "r:%d", rotateValue);

   // Do the physical rotation
   nErr = IDISPLAY_SetPrefs(pme->m_pIDisplay, buf, STRLEN(buf));


   // Verify the rotation
   nAfter = IBITMAPROT_GetRotation(pIBitmapRot);
   if (nAfter != rotateValue)
   { 
      // Rotation unsuccessful
      FARF(BAM, ("Failed to Rotate"));
      FreePtr((void**)&pIBitmapRot);
      FreePtr((void**)&pIBitmap);
      return FALSE;
   }

   FARF(BAM, ("Display is rotated"));

   // Do our cleanup
   FreeObj((void**)&pIBitmapRot);
   FreeObj((void**)&pIBitmap);
   return TRUE;
}



//===========================================================================
//
// Redraw the screen after rotating.  Making sure to update the necessary
// rectangles that we store
//
//===========================================================================
static void AppMgr_RedrawDisplay(AppMgr* pme)
{
   AEERect  rc;
   AEEDeviceInfo di;

   // Initilize Device Info
   // In the simulator the GetDeviceInfo is always returning the info for Display1, so using GetClipRect
   // to get the size of the display instead.
   IDISPLAY_GetClipRect(pme->m_pIDisplay, &rc);
   ISHELL_GetDeviceInfo(pme->a.m_pIShell, &di);
   pme->m_nColorDepth = di.nColorDepth;
   //SETAEERECT(&pme->m_rc, 0, 0, di.cxScreen, di.cyScreen);
   SETAEERECT(&pme->m_rc, 0, 0, rc.dx, rc.dy);

   // Clear the screen before redrawing
   //IDISPLAY_ClearScreen(pme->m_pIDisplay);
   IDISPLAY_EraseRect(pme->m_pIDisplay, &pme->m_rc);
   FARF(BAM, ("We have just cleared the display"));

   // Our screen is landscape if cx is > cy (this implies that dx = dy is a 
   // portrait screen
   if (rc.dx > rc.dy)
   {
      pme->m_orentDisplayDefault = AEE_SO_LANDSCAPE;
   }
   else
   {
      pme->m_orentDisplayDefault = AEE_SO_PORTRAIT;
   }

   pme->m_cyFont = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL);

   // Update the MShopLogo rectangle
   if (pme->m_pMShopLogo)
   {
      AEEImageInfo ii;
      IIMAGE_GetInfo(pme->m_pMShopLogo, &ii);
      SETAEERECT(&pme->m_rcMShopLogo, (pme->m_rc.dx - ii.cx)/2, 0, ii.cx, ii.cy);
   }

    // Make sure that the new main menu has been resized properly
   AppMgr_SetMainMenuSize(pme);

   // Set the Menu Rect
   if (AppMgr_GetMenuRect(pme, pme->m_pSK, &rc))
   {
      IMENUCTL_SetRect(pme->m_pMenu, &rc);
   }
   else
   {
      // We were unable to determine rect, set to default
      IMENUCTL_SetRect(pme->m_pMenu, &pme->m_rc);
   }

   if (pme->m_wState == ST_MAINMENU)
   {
      IMENUCTL_Redraw(pme->m_pMainMenu);
      if (IMENUCTL_GetItemCount(pme->m_pMainMenuSK) > 0)
      {
         IMENUCTL_Redraw(pme->m_pMainMenuSK);
      }
   }
   else
   {
      if (IMENUCTL_GetItemCount(pme->m_pMenu) > 0)
      {
         IMENUCTL_Redraw(pme->m_pMenu);
      }
      if (IMENUCTL_GetItemCount(pme->m_pSK) > 0)
      {
         IMENUCTL_Redraw(pme->m_pSK);
      }
   }

   AppMgr_RefreshDisplay(pme);

   IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
}
#endif //FEATURE_SCREEN_ROTATE

#if defined(FEATURE_SCREEN_ROTATE) || defined(FEATURE_SUPPORT_MULTIPLE_DISPLAYS)
//===========================================================================
//
// Ensure that the proper things are displayed on the screen after a flip
// or a rotate.  
//
//===========================================================================
static void AppMgr_RefreshDisplay(AppMgr* pme)
{
   boolean bCreateBackButton = TRUE;
   // Depending on state we may need to call the state functions
   // so that the display is updated properly, and if the prompt
   // is currently being displayed, we need to re-display the prompt
   pme->m_bRedrawingUI = TRUE;
   if (pme->m_wState == ST_EDITSOUNDIMAGE)
   {
      if (pme->m_wEditSoundImageSubState == ST_EDITSOUNDIMAGEBROWSEIMAGES)
      {
         AppMgr_ShowFiles(pme, TRUE);
      }
      else if (pme->m_wEditSoundImageSubState == ST_EDITSOUNDIMAGEBROWSESOUND)
      {
         AppMgr_ShowFiles(pme, FALSE);
      }
      else
      {
         AppMgr_EditSoundImage(pme);
      }
   }
   else if (pme->m_wState == ST_SYSTEMINFO)
   {
      // To ensure that there are not things left around
      // If not done, the top part will have menu color around
      // the lines.
      //IDISPLAY_ClearScreen(pme->m_pIDisplay);
      IDISPLAY_EraseRect(pme->m_pIDisplay, &pme->m_rc);

      AppMgr_SystemInfo(pme);     
   }
   else if (pme->m_wState == ST_MANAGETOOL)
   {
      AppMgr_ManageTool(pme);
   }
   else if ((pme->m_wState == ST_ORDERAPPS) || (pme->m_wState == ST_MOVEAPPS) ||
            (pme->m_wState == ST_MOVE) || (pme->m_wState == ST_SETCATEGORY))
   {
      AppMgr_Categories(pme);
   }
   else if ((pme->m_wState == ST_ORDERMAINMENU) || (pme->m_wState == ST_ORDERFAVORITES) ||
            (pme->m_wState == ST_ORDERWORK) || (pme->m_wState == ST_ORDERFUN) ||
            (pme->m_wState == ST_MOVEMAINMENU) || (pme->m_wState == ST_MOVEFAVORITES) ||
            (pme->m_wState == ST_MOVEWORK) || (pme->m_wState == ST_MOVEFUN))
   {
      if (pme->m_bConfigMenu && ((pme->m_wState >= ST_ORDERMAINMENU) && (pme->m_wState <= ST_ORDERFUN)))
      {
         // We have already selected an app to move, update the look
         // by going into Order
         AppMgr_Order(pme);
         bCreateBackButton = FALSE;
      }
      else
      {
         // An app has not been selected or we are on the move screens
         AppMgr_Config(pme);
      }
   }
   else if ((ST_MAINMENU == pme->m_wState ||
             ST_FAVORITES == pme->m_wState ||
             ST_WORK == pme->m_wState ||
             ST_FUN == pme->m_wState))
   {
      IMENUCTL_Redraw(pme->m_pMainMenu);
   }
#ifdef MSHOP_STATES
   else if (pme->m_wState == ST_MSHOP)
   {
      if (pme->m_pMShopLogo)
      {
         IIMAGE_Draw(pme->m_pMShopLogo, pme->m_rcMShopLogo.x, pme->m_rcMShopLogo.y);
      }
   }
   else if (pme->m_wState == ST_MSHOPGUIDE)
   {
      AppMgr_MShopGuide(pme);
   }
#endif


   if (pme->m_bPromptIsDisplayed && (ST_SYSTEMINFO != pme->m_wState))
   {
      AppMgr_Prompt(pme, pme->m_pPromptTitle, pme->m_wPromptTitleID, pme->m_pPromptText, pme->m_wPromptTextID, 
                    pme->m_pPromptButtons, pme->m_wPromptDefID, pme->m_dwPromptProps, pme->m_bPromptBox);

      // Redraw the OK button for the "Failed to launch dialog
#ifdef FEATURE_TOUCHSCREEN_SUPPORT
      if (NO_ENTRY == pme->m_wState && NULL == pme->m_pPromptButtons && pme->m_bDispTouchEnabled)
      {
         uint16 id[1];
         id[0] = IDC_OK;

         AppMgr_CreateSoftmenu(pme,id, 1);
         IMENUCTL_SetActive(pme->m_pSK, FALSE);
         IMENUCTL_Redraw(pme->m_pSK);
      }
#endif //FEATURE_TOUCHSCREEN_SUPPORT
   }
#ifdef FEATURE_TOUCHSCREEN_SUPPORT
   // recreate the softkey
   else if (pme->m_bDispTouchEnabled && bCreateBackButton)
   {
      AppMgr_AddSoftKey(pme);
   }
#endif //FEATURE_TOUCHSCREEN_SUPPORT

   pme->m_bRedrawingUI = FALSE;
}
#endif //defined(FEATURE_SCREEN_ROTATE) || defined(FEATURE_SUPPORT_MULTIPLE_DISPLAYS)



#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS

//===========================================================================
//
// Callback for when the new Display is enabled after an EVT_APP_RESUME.
// We just need to do the normal Resume code here, now that the display
// is enabled
//
//===========================================================================
static void AppMgr_EnableDispRedrawCallback(void* pData)
{
   AppMgr* pme = (AppMgr*)pData;

   FARF(BAM, ("Got EnableDisplayRedraw Callback"));

   if (pme->cbDisplayEnabled)
   {
      CALLBACK_Cancel(pme->cbDisplayEnabled);
      FreePtr((void**)&pme->cbDisplayEnabled);
   }

   FARF(BAM, ("Our current display thinks that its Enabled state is %d", IDISPLAY_IsEnabled(pme->m_pIDisplay)));

   // Do our resume code.  And if it fails, exit BAM.
   if (!AppMgr_Resume(pme))
   {
      ISHELL_CloseApplet(pme->a.m_pIShell, TRUE);
   }
}

//===========================================================================
//
// Callback for when the new Display is enabled after a flip.
//
//===========================================================================
static void AppMgr_EnableDispCallback(void* pData)
{
   AppMgr* pme = (AppMgr*)pData;

   FARF(BAM, ("Got EnableDisplay Callback"));

   if (pme->cbDisplayEnabled)
   {
      CALLBACK_Cancel(pme->cbDisplayEnabled);
      FreePtr((void**)&pme->cbDisplayEnabled);
   }

   FARF(BAM, ("Our current display thinks that its Enabled state is %d", IDISPLAY_IsEnabled(pme->m_pIDisplay)));

   // It seems necessary to use the Resume Callback here in stead of 
   // calling RecreateUI directly as sometimes when flipping the controls
   // were not being fully created (like a menu control's internal display pointer
   // was not valid
   //AppMgr_RecreateUI(pme);
   MEMSET(&pme->m_cbUpdateUI, '\0', sizeof(pme->m_cbUpdateUI));
   pme->m_cbUpdateUI.pfnNotify = AppMgr_RecreateUICallback; // My callback function
   pme->m_cbUpdateUI.pNotifyData = pme;
   pme->m_cbUpdateUI.pfnCancel = NULL;  //Initialize fo NULL.  It is updated by Shell
   ISHELL_Resume(pme->a.m_pIShell, &pme->m_cbUpdateUI);
}

//===========================================================================
//
// The callback function used to Call AppMgr_RecreateUI() after a flip
// This is so that "brew" is fully finished Activating/enabling the new
// display.  Before using a callback function, sometimes the controls
// would not be fully created, underlying member of the IStaticCtl and the
// IMenuCtl would not have valid values
//
//===========================================================================
static void AppMgr_RecreateUICallback(void* pData)
{
   AppMgr* pme = (AppMgr*)pData;

   FARF(BAM, ("Got RecreateUI Callback"));

   if (pme->m_cbUpdateUI.pfnCancel)
   {
      pme->m_cbUpdateUI.pfnCancel(&pme->m_cbUpdateUI);
      pme->m_cbUpdateUI.pfnCancel = NULL;
   }

   AppMgr_RecreateUI(pme);
}


//===========================================================================
//
// DESCRIPTION
//    Do the dirty work of switching displays when the user closes the clamshell
//    or slider.  This involves re-creating all of the menu, static, and text
//    controls so that they pick up the new "active" display.
//    Then all of the old data is copied over to the new controls and
//    the old controls are deleted in the RecreateUI method
//
//===========================================================================
static boolean AppMgr_SwitchDisplays(AppMgr* pme, boolean useDisplay1)
{
   IDisplay *pNewDisplay = NULL;
   IFlip    *pIFlip = NULL;
   AEECLSID *pClsList = NULL;
   AEEFlipInfo *pFlipInfo = NULL;
   int32    dwNumDisplays = 0;
   int16    wNumFlips = 0;
   int16    i = 0;
   AEECLSID newDisplayId = 0;


   // Clear the ProcessFlip flag
   //pme->m_bProcessFlip = FALSE;

#ifdef FEATURE_HAS_MULTIPLE_DISPLAYS
   // If we can get a flip, but we do not have mult displays (like a slider)
   // don't try to switch displays, but return TRUE like we did.  We should just
   // stay on the display then.
   if (!pme->m_bHasMultipleDisplays)
      return TRUE;
#endif

   FARF(BAM, ("Trying to switch displays, useDisplay1 is %d", useDisplay1));


   // Ensure that We don't have a pending callback
   if (pme->cbDisplayEnabled)
   {
      CALLBACK_Cancel(pme->cbDisplayEnabled);
      FreePtr((void**)&pme->cbDisplayEnabled);
   }
   if (pme->m_cbUpdateUI.pfnCancel)
   {
      pme->m_cbUpdateUI.pfnCancel(&pme->m_cbUpdateUI);
      pme->m_cbUpdateUI.pfnCancel = NULL;
   }


   // Clear the old display and use the new one
   //IDISPLAY_ClearScreen(pme->m_pIDisplay);
   IDISPLAY_EraseRect(pme->m_pIDisplay, &pme->m_rc);
   IDISPLAY_Update(pme->m_pIDisplay);

   CreateInstance(pme->a.m_pIShell, AEECLSID_FLIP, 
                            (void **)&pIFlip);

   // If we cannot get an iFlip object, we cannot flip.
   if (NULL == pIFlip)
   {
      return FALSE;
   }

   if ((SUCCESS != IFLIP_GetDisplayList(pIFlip, NULL, &dwNumDisplays)) || (0 == dwNumDisplays))
   {
      FARF(BAM, ("There are no displays available for this flip"));
      if (useDisplay1)
      {
         FARF(BAM, ("I was going to use %d", AEECLSID_DISPLAY1)); 
         newDisplayId = AEECLSID_DISPLAY1;
      }
      else
      {
         FARF(BAM, ("I was going to use %d", AEECLSID_DISPLAY2));

         newDisplayId = AEECLSID_DISPLAY2;
      }
   }
   else
   {

      pClsList = (AEECLSID*) MALLOC(sizeof(AEECLSID) * dwNumDisplays);

      if (NULL == pClsList)
      {
         FARF(BAM, ("Out of memory"));
         FreeObj((void**)& pIFlip);
         return FALSE;
      }

      if (SUCCESS != IFLIP_GetDisplayList(pIFlip, (void*)pClsList, &dwNumDisplays))
      {
         FARF(BAM, ("Unable to get the display IDs for this flip"));

         if (useDisplay1)
         {
            FARF(BAM, ("I was going to use %d", AEECLSID_DISPLAY1)); 
            newDisplayId = AEECLSID_DISPLAY1;
         }
         else
         {
            FARF(BAM, ("I was going to use %d", AEECLSID_DISPLAY2));

            newDisplayId = AEECLSID_DISPLAY2;
         }
         
      }
      else
      {
         FARF(BAM, ("IFlip told me to use %d", pClsList[0]));
         newDisplayId = pClsList[0];      
      }
   }
   
   // We know which display is becoming current.  Check to see if BAM
   // is configured to run on this display, and if not just return FALSE
   if (FALSE == DisplayEnabled(pme->a.m_pIShell, pme->m_bHasMultipleDisplays,
      pme->m_wEnabledDisplays, newDisplayId))
   {
      FARF(BAM, ("Can't run on this display"));
      FreePtr((void**)&pClsList);
      FreeObj((void**)& pIFlip);
      return FALSE;
   }

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
   // newDisplayId is the current active display, check whether it is touch enabled or not
    if(newDisplayId )
   {
      pme->m_bDispTouchEnabled = IsDisplayTouchEnabled(newDisplayId,pme->m_wTouchDisplays);
      FARF(BAM, ("newDisplayId = pme->m_bDispTouchEnabled = %d   &   %d", newDisplayId, pme->m_bDispTouchEnabled));

   }
#endif

   // Set up the key mappings for this new display that we are about to use.
   AppMgr_SetKeyList(pme, pIFlip, newDisplayId);


   CreateInstance(pme->a.m_pIShell, newDisplayId, 
                            (void **)&pNewDisplay);
   if (NULL == pNewDisplay)
   {
      FARF(BAM, ("Unable to create IDisplay for with clsid %d", newDisplayId));
      FreePtr((void**)&pClsList);
      FreeObj((void**)& pIFlip);
      return FALSE;
   }

   if ((SUCCESS != IFLIP_GetFlipIDList(pIFlip, NULL, &wNumFlips)) || (0 == wNumFlips))
   {
      FARF(BAM, ("There are no flips for this device"));
   }
   else
   {

      pFlipInfo = (AEECLSID*) MALLOC(sizeof(AEEFlipInfo) * wNumFlips);

      if (NULL != pFlipInfo)
      {
         if (SUCCESS == IFLIP_GetFlipIDList(pIFlip, pFlipInfo, &wNumFlips))
         {
            for ( i = 0; i < wNumFlips; ++i)
            {
               FARF(BAM, ("Display index %d has ID %d, x = %d, y = %d, z = %d", i, DEVNOTIFY_GET_ID(pFlipInfo[i]),
                           DEVNOTIFY_GET_X(pFlipInfo[i]), DEVNOTIFY_GET_Y(pFlipInfo[i]), DEVNOTIFY_GET_Z(pFlipInfo[i])));
            }
         }
         else
         {
            FARF(BAM, ("Unable to get the list of flips"));
         }
      }
      else
      {
         FARF(BAM, ("No memory to get flip possitions"));
      }
   }

   FreePtr((void**)&pFlipInfo);
   FreePtr((void**)&pClsList);
   FreeObj((void**)& pIFlip);

#ifdef USE_TT_FONTS
   // Restore the original fonts
   AppMgr_RestoreFonts(pme);
#endif // USE_TT_FONTS

   IDISPLAY_MakeDefault(pNewDisplay);

   IDISPLAY_Release(pme->m_pIDisplay);

   pme->m_pIDisplay = pNewDisplay;
   pNewDisplay = NULL;

   FARF(BAM, ("Our current display thinks that its Enabled state is %d", IDISPLAY_IsEnabled(pme->m_pIDisplay)));

   if (IDISPLAY_IsEnabled(pme->m_pIDisplay))
   {
      return AppMgr_RecreateUI(pme);
   }

   // If our display is not yet enabled, we need to wait for it to be enabled before
   // trying to draw on it.
   pme->cbDisplayEnabled = (AEECallback*) MALLOC(sizeof(AEECallback));
   MEMSET(pme->cbDisplayEnabled, 0, sizeof(AEECallback));
   CALLBACK_Init(pme->cbDisplayEnabled, AppMgr_EnableDispCallback, pme);
   IDISPLAY_NotifyEnable(pme->m_pIDisplay, pme->cbDisplayEnabled);

   return TRUE;
}

static boolean AppMgr_RecreateUI(AppMgr* pme)
{
   IMenuCtl *pNewMainMenu = NULL;
   IMenuCtl *pNewMainMenuSK = NULL;
   IMenuCtl *pNewMenu = NULL;
   IMenuCtl *pNewSK = NULL;
   IStatic  *pNewStatic = NULL;    // Static Control
   AEEDeviceInfo  di;
   int result;
   AEERect  dispRect = {0,0,0,0};
   boolean bResetActive = FALSE;


   FARF(BAM, ("Recreating the UI to use the new display"));

   // Crete the new instances that use the new display
   if (pme->m_pMainMenu)
   {
      result = CreateInstance(pme->a.m_pIShell, pme->m_cfg.bListView ? GetMenuCtlClassID(pme->m_bDispTouchEnabled) : GetIconViewClassID(pme->m_bDispTouchEnabled), (void **)&pNewMainMenu);
      if ((result != AEE_SUCCESS) || (NULL == pNewMainMenu))
      {         
         return FALSE;
      }
   }
   if (pme->m_pMainMenuSK)
   {
      result = CreateInstance(pme->a.m_pIShell, GetSoftKeyClassID(pme->m_bDispTouchEnabled),(void **)&pNewMainMenuSK);
      if ((result != AEE_SUCCESS) || (NULL == pNewMainMenuSK))
      {
         FreeObj((void**)&pNewMainMenu);
         return FALSE;
      }
   }

   if (pme->m_pMenu)
   {
      result = CreateInstance(pme->a.m_pIShell, GetMenuCtlClassID(pme->m_bDispTouchEnabled), (void **)&pNewMenu);
      if ((result != AEE_SUCCESS) || (NULL == pNewMenu))
      {
         FreeObj((void**)&pNewMainMenu);
         FreeObj((void**)&pNewMainMenuSK);
         return FALSE;
      }
   }
   if (pme->m_pSK);
   {
      result = CreateInstance(pme->a.m_pIShell, GetSoftKeyClassID(pme->m_bDispTouchEnabled),(void **)&pNewSK);
      if ((result != AEE_SUCCESS) || (NULL == pNewSK))
      {
         FreeObj((void**)&pNewMainMenu);
         FreeObj((void**)&pNewMainMenuSK);
         FreeObj((void**)&pNewMenu);
         return FALSE;
      }
   }
   if (pme->m_pStatic)
   {
      result = CreateInstance(pme->a.m_pIShell, AEECLSID_STATIC,(void **)&pNewStatic);
      if ((result != AEE_SUCCESS) || (NULL == pNewStatic))
      {
         FreeObj((void**)&pNewMainMenu);
         FreeObj((void**)&pNewMainMenuSK);
         FreeObj((void**)&pNewMenu);         
         FreeObj((void**)&pNewSK);
         return FALSE;
      }
   }

#ifdef USE_TT_FONTS
   // Install the TrueType fonts here, before making the call to
   // GetFontMetrics() and before any menu controls are instantiated.
   AppMgr_InstallTTFonts(pme);
#endif // USE_TT_FONTS

   if(pme->m_bDispTouchEnabled)
   {
      IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_BACKGROUND, TOUCHSCREEN_BACKGROUND);
   }


   // Initilize Device Info
   // In the simulator the GetDeviceInfo is always returning the info for Display1, so using GetClipRect
   // to get the size of the display instead.
   IDISPLAY_GetClipRect(pme->m_pIDisplay, &dispRect);
   ISHELL_GetDeviceInfo(pme->a.m_pIShell, &di);
   pme->m_nColorDepth = di.nColorDepth;
   //SETAEERECT(&pme->m_rc, 0, 0, di.cxScreen, di.cyScreen);
   SETAEERECT(&pme->m_rc, 0, 0, dispRect.dx, dispRect.dy);

   // For testing purposes Clear the display
   //IDISPLAY_ClearScreen(pme->m_pIDisplay);
   IDISPLAY_EraseRect(pme->m_pIDisplay, &pme->m_rc);
   FARF(BAM, ("We have just cleared the new display"));

#ifdef FEATURE_SCREEN_ROTATE
   // Our screen is landscape if cx is > cy (this implies that dx = dy is a 
   // portrait screen
   if (dispRect.dx > dispRect.dy)
   {
      pme->m_orentDisplayDefault = AEE_SO_LANDSCAPE;
   }
   else
   {
      pme->m_orentDisplayDefault = AEE_SO_PORTRAIT;
   }
#endif //FEATURE_SCREEN_ROTATE

   pme->m_cyFont = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL);




   // Set the IIMAGE objects to point to the new display
   if (pme->m_pAnimatedImage)
   {
      IIMAGE_SetDisplay(pme->m_pAnimatedImage, pme->m_pIDisplay);
   }

   if (pme->m_pMShopLogo)
   {
      AEEImageInfo ii;
      IIMAGE_GetInfo(pme->m_pMShopLogo, &ii);
      SETAEERECT(&pme->m_rcMShopLogo, (pme->m_rc.dx - ii.cx)/2, 0, ii.cx, ii.cy);

      IIMAGE_SetDisplay(pme->m_pMShopLogo, pme->m_pIDisplay);
   }

   //
   // Copy all of the data over to the new instances
   //
   if (pNewMainMenu)
   {
      int numItems;
      int curItem;
      uint16 curItemId;
      CtlAddItem ai;

      if (pNewMainMenuSK)
      {
         // Settings specific to large icon view
         AppMgr_LargeIconViewSettings(pme, pNewMainMenu, pNewMainMenuSK);
      }
      else
      {
         // Set Main Menu Style
         SetMenuStyleAndColor(pNewMainMenu, pme->m_cfg.bListView ? GetMenuCtlClassID(pme->m_bDispTouchEnabled) : GetIconViewClassID(pme->m_bDispTouchEnabled), pme->m_nColorDepth, pme->m_bDispTouchEnabled);
      }

      numItems = IMENUCTL_GetItemCount(pme->m_pMainMenu);
      for (curItem = 0; curItem < numItems; ++curItem)
      {
         curItemId = IMENUCTL_GetItemID(pme->m_pMainMenu, curItem);
         if (IMENUCTL_GetItem(pme->m_pMainMenu, curItemId, &ai))
         {
            if (ai.pImage)
            {
               IIMAGE_SetDisplay(ai.pImage, pme->m_pIDisplay); 
            }
            IMENUCTL_AddItemEx(pNewMainMenu, &ai);
         }
      }

      IMENUCTL_SetTitle(pNewMainMenu, pme->m_pMainMenuResFile, pme->m_pMainMenuResID, pme->m_pMainMenuText);
      if (!pme->m_bDispTouchEnabled)
      {
         IMENUCTL_SetSel(pNewMainMenu, IMENUCTL_GetSel(pme->m_pMainMenu));
      }
      IMENUCTL_SetActive(pNewMainMenu, IMENUCTL_IsActive(pme->m_pMainMenu));
   }

   if (pNewMainMenuSK)
   {
      int numItems;
      int curItem;
      uint16 curItemId;
      CtlAddItem ai;

      // If exists, the settings/styles are set in the pNewMainMenu section

      numItems = IMENUCTL_GetItemCount(pme->m_pMainMenuSK);
      for (curItem = 0; curItem < numItems; ++curItem)
      {
         curItemId = IMENUCTL_GetItemID(pme->m_pMainMenuSK, curItem);
         if (IMENUCTL_GetItem(pme->m_pMainMenuSK, curItemId, &ai))
         {
            if (ai.pImage)
            {
               IIMAGE_SetDisplay(ai.pImage, pme->m_pIDisplay); 
            }
            IMENUCTL_AddItemEx(pNewMainMenuSK, &ai);
         }
      }


      // Don't know if this is needed.  I think it is called above
      if (pme->m_bMainMenuSKEnableCommandCalled)
      {
         IMENUCTL_EnableCommand(pNewMainMenuSK, pme->m_bMainMenuSKEnableCommandValue);
      }

      IMENUCTL_SetSel(pNewMainMenuSK, IMENUCTL_GetSel(pme->m_pMainMenuSK));
      IMENUCTL_SetActive(pNewMainMenuSK, IMENUCTL_IsActive(pme->m_pMainMenuSK));
   }

   if (pNewSK)
   {
      int numItems;
      int curItem;
      uint16 curItemId;
      CtlAddItem ai;
      boolean isActive;
      boolean onlyBack = TRUE;

      SetMenuStyleAndColor(pNewSK, GetSoftKeyClassID(pme->m_bDispTouchEnabled), pme->m_nColorDepth, pme->m_bDispTouchEnabled);

      numItems = IMENUCTL_GetItemCount(pme->m_pSK);
      for (curItem = 0; curItem < numItems; ++curItem)
      {
         curItemId = IMENUCTL_GetItemID(pme->m_pSK, curItem);
         if (IMENUCTL_GetItem(pme->m_pSK, curItemId, &ai))
         {
            
            if (IDC_BACK != ai.wItemID)
            {
               onlyBack = FALSE;
            }
            if (ai.pImage)
            {
               IIMAGE_SetDisplay(ai.pImage, pme->m_pIDisplay); 
            }
            IMENUCTL_AddItemEx(pNewSK, &ai);
         }
      }

      if (pme->m_bSKEnableCommandCalled)
      {
         IMENUCTL_EnableCommand(pNewSK, pme->m_bSKEnableCommandValue);
      }

      IMENUCTL_SetSel(pNewSK, IMENUCTL_GetSel(pme->m_pSK));
      isActive = IMENUCTL_IsActive(pme->m_pSK);
      // If the prompt dialog is active we need to re-draw what is below it
      if(!pme->m_bDispTouchEnabled)
      {
         // If we are on the order screens or the only button is the back button
         // we need to clear out the SK
         if ((numItems > 0 && onlyBack && !pme->m_bPromptIsDisplayed) ||
             ((pme->m_wState >= ST_ORDERMAINMENU) && (pme->m_wState <= ST_ORDERFUN)))
         {
            IMENUCTL_DeleteAll(pNewSK);
            IMENUCTL_SetActive(pNewSK, FALSE);
            // insure that m_pMenu will be active
            bResetActive = TRUE;
         }
         else
         {
            if (!pme->m_bPromptIsDisplayed && numItems > 0)
            {
               IMENUCTL_SetActive(pNewSK, TRUE);
               IMENUCTL_Redraw(pNewSK);

               // since we are not active, insure that m_pMenu will be active
               if (!isActive)
               {
                  bResetActive = TRUE;
               }
            }
            IMENUCTL_SetActive(pNewSK, isActive);
         }
      }
   }


   // Do after the pNewSK because one of the states requires checks
   // the height of the SK menu, to determine the height of the menu
   if (pNewMenu)
   {
      int numItems;
      int curItem;
      uint16 curItemId;
      CtlAddItem ai;
      boolean isActive;
      AEERect rc;

      SetMenuStyleAndColor(pNewMenu, GetMenuCtlClassID(pme->m_bDispTouchEnabled), pme->m_nColorDepth, pme->m_bDispTouchEnabled);

      // Set the Menu Rect
      if (AppMgr_GetMenuRect(pme, pNewSK, &rc))
      {
         IMENUCTL_SetRect(pNewMenu, &rc);
      }
      else
      {
         // We were unable to determine rect, set to default
         IMENUCTL_SetRect(pNewMenu, &pme->m_rc);
      }

      numItems = IMENUCTL_GetItemCount(pme->m_pMenu);
      for (curItem = 0; curItem < numItems; ++curItem)
      {
         curItemId = IMENUCTL_GetItemID(pme->m_pMenu, curItem);
         if (IMENUCTL_GetItem(pme->m_pMenu, curItemId, &ai))
         {
            if (ai.pImage)
            {
               IIMAGE_SetDisplay(ai.pImage, pme->m_pIDisplay); 
            }
            IMENUCTL_AddItemEx(pNewMenu, &ai);
         }
      }

      IMENUCTL_SetTitle(pNewMenu, pme->m_pMenuResFile, pme->m_pMenuResID, pme->m_pMenuText);
      IMENUCTL_SetSel(pNewMenu, IMENUCTL_GetSel(pme->m_pMenu));
      isActive = IMENUCTL_IsActive(pme->m_pMenu);
      // If the prompt dialog is active we need to re-draw what is below it
      if (!pme->m_bDispTouchEnabled && pme->m_bPromptIsDisplayed)
      {
         IMENUCTL_SetActive(pNewMenu, TRUE);
         IMENUCTL_Redraw(pNewMenu);
      }
      IMENUCTL_SetActive(pNewMenu, isActive);

   }

   if (pNewStatic)
   {
      uint32 props;

      props = ISTATIC_GetProperties(pme->m_pStatic);
      ISTATIC_SetProperties(pNewStatic, props);

      ISTATIC_SetText(pNewStatic, pme->m_pStaticTitle, pme->m_pStaticText, pme->m_pStaticFntTitle, pme->m_pStaticFntText);

      if (pme->m_bStaticSizeToFitCalled)
      {
         ISTATIC_SizeToFit(pNewStatic, &(pme->m_rcStaticSizeToFit));
      }

      ISTATIC_SetActive(pNewStatic, ISTATIC_IsActive(pme->m_pStatic));
   }


   // Delete the old instances and store the new ones
   if (pNewStatic)
   {
     ISTATIC_Release(pme->m_pStatic);
     pme->m_pStatic = pNewStatic;
   }

   if (pNewMainMenu)
   {
      IMENUCTL_Release(pme->m_pMainMenu);
      pme->m_pMainMenu = pNewMainMenu;
   }
   if (pNewMainMenuSK)
   {
      IMENUCTL_Release(pme->m_pMainMenuSK);
      pme->m_pMainMenuSK = pNewMainMenuSK;
   }
   if (pNewMenu)
   {
      IMENUCTL_Release(pme->m_pMenu);
      pme->m_pMenu = pNewMenu;
   }
   if (pNewSK)
   {
      IMENUCTL_Release(pme->m_pSK);
      pme->m_pSK = pNewSK;
   }

   // Make sure that the new main menu has been resized properly
   AppMgr_SetMainMenuSize(pme);

   // If the SoftKey was active and it only had a back button, we need to set a new
   // active control as we cleard out the SK and set it to not be active (we don't
   // want it displayed.  Since this only happens when the prompt is not displayed
   // the only control that could be set active is the the menu controls.  (If we
   // were displaying the main menu, there would not be a back button so don't
   // have to worry about those controls
   // bResetActive is never set if we are in a TouchScreen
   if (bResetActive)
   {
      IMENUCTL_SetActive(pme->m_pMenu, TRUE);
   }

   AppMgr_RefreshDisplay(pme);

   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);


   FARF(BAM, ("We have successfully switched displays"));
   // We are done
   return TRUE;
}
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS


//===========================================================================
//
// DESCRIPTION
//    Store the values set as the MenuTitle call on the m_pMainMenu object so that
//    they can be re-used when we are trying to re-create the display on a new
//    screen when the user flips the phone cover to utilize a different display
//
// PARAMETERS
//    The same as IMENUCTL_SetTitle()
//
//
//===========================================================================
static void AppMgr_StoreMainMenuTitleInfo(AppMgr* pme, char* pszResFile, uint16 wResID, AECHAR* pwszText)
{
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   // If we are processing the flip event we don't want to save the data since the pointers that might be coming
   // in are the ones we have saved (are aliases) so bad things can happen
   if (pme->m_bRedrawingUI)
   {
      return;
   }

   FreePtr((void**)&(pme->m_pMainMenuResFile));
   FreePtr((void**)&(pme->m_pMainMenuText));

   if (NULL != pszResFile)
   {
      pme->m_pMainMenuResFile = STRDUP(pszResFile);
   }
   pme->m_pMainMenuResID = wResID;
   if (NULL != pwszText)
   {
      pme->m_pMainMenuText = WSTRDUP(pwszText);
   }
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
}

//===========================================================================
//
// DESCRIPTION
//    Store the values set as the MenuTitle call on the m_pMenu object so that
//    they can be re-used when we are trying to re-create the display on a new
//    screen when the user flips the phone cover to utilize a different display
//
// PARAMETERS
//    The same as IMENUCTL_SetTitle()
//
//
//===========================================================================
static void AppMgr_StoreMenuTitleInfo(AppMgr* pme, char* pszResFile, uint16 wResID, AECHAR* pwszText)
{
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   // If we are processing the flip event we don't want to save the data since the pointers that might be coming
   // in are the ones we have saved (are aliases) so bad things can happen
   if (pme->m_bRedrawingUI)
   {
      return;
   }

   FreePtr((void**)&(pme->m_pMenuResFile));
   FreePtr((void**)&(pme->m_pMenuText));

   if (NULL != pszResFile)
   {
      pme->m_pMenuResFile = STRDUP(pszResFile);
   }
   pme->m_pMenuResID = wResID;
   if (NULL != pwszText)
   {
      pme->m_pMenuText = WSTRDUP(pwszText);
   }
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
}


//===========================================================================
//
// DESCRIPTION
//    Store the values set as the EnableCommand call on the MainMenuSK menu,  
//    so that they can be re-used when we are trying to re-create the
//    display on a new screen when the user
//    flips the phone cover to utilize a different display
//
// PARAMETERS
//    The same as IMENUCTL_EnableCommand()
//
//
//===========================================================================
static void AppMgr_StoreMainMenuSKEnableCommandInfo(AppMgr* pme, boolean value)
{
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   pme->m_bMainMenuSKEnableCommandCalled = TRUE;
   pme->m_bMainMenuSKEnableCommandValue = value;
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
}

//===========================================================================
//
// DESCRIPTION
//    Store the values set as the EnableCommand call on the SK menu,  
//    so that they can be re-used when we are trying to re-create the
//    display on a new screen when the user
//    flips the phone cover to utilize a different display
//
// PARAMETERS
//    The same as IMENUCTL_EnableCommand()
//
//
//===========================================================================
/*
static void AppMgr_StoreSKEnableCommandInfo(AppMgr* pme, boolean value)
{
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   pme->m_bSKEnableCommandCalled = TRUE;
   pme->m_bSKEnableCommandValue = value;
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
}
*/

//===========================================================================
//
// DESCRIPTION
//    Store the values set as the Static Text so that they can be re-used when
//    we are trying to re-create the display on a new screen when the user
//    flips the phone cover to utilize a different display
//
// PARAMETERS
//    The same as ISTATIC_SetText()
//
//
//===========================================================================
static void AppMgr_StoreStaticTextInfo(AppMgr* pme, AECHAR* pwszTitle, AECHAR* pwszText, AEEFont fntTitle, AEEFont fntText)
{
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   // If we are processing the flip event we don't want to save the data since the pointers that might be coming
   // in are the ones we have saved (are aliases) so bad things can happen
   if (pme->m_bRedrawingUI)
   {
      return;
   }

   FreePtr((void**)&(pme->m_pStaticTitle));
   FreePtr((void**)&(pme->m_pStaticText));

   if (NULL != pwszTitle )
   {
      pme->m_pStaticTitle = WSTRDUP(pwszTitle);
   }
   if (NULL != pwszText)
   {
      pme->m_pStaticText = WSTRDUP(pwszText);
   }
   pme->m_pStaticFntTitle = fntTitle;
   pme->m_pStaticFntText = fntText;
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
}

//===========================================================================
//
// DESCRIPTION
//    Store the values set as the Static SizeToFit values so that they can be re-used when
//    we are trying to re-create the display on a new screen when the user
//    flips the phone cover to utilize a different display
//
// PARAMETERS
//    The same as ISTATIC_SizeToFit()
//
//
//===========================================================================
static void AppMgr_StoreStaticSizeToFitInfo(AppMgr* pme, boolean called, AEERect* prc)
{
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   // If we are processing the flip event we don't want to save the data since the pointers that might be coming
   // in are the ones we have saved (are aliases) so bad things can happen
   if (pme->m_bRedrawingUI)
   {
      return;
   }

   pme->m_bStaticSizeToFitCalled = called;
   if (called)
   {
      if (NULL != prc)
      {
         pme->m_rcStaticSizeToFit = *prc;
      }
      else
      {
         // they set it, but did not give us the rect.  So we will store it like they did not set it
         pme->m_bStaticSizeToFitCalled = FALSE;
      }
   }
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
}


//===========================================================================
//
// DESCRIPTION
//    Store the values set as the passed into the Prompt() functino so that they can be re-used when
//    we are trying to re-create the display on a new screen when the user
//    flips the phone cover to utilize a different display
//
// PARAMETERS
//    The same as AppMgr_Prompt()
//
//
//===========================================================================
static void AppMgr_StorePromptInfo(AppMgr* pme, const AECHAR* szTitle, uint16 wIDTitle, const AECHAR* szText,
                                   uint16 wIDText, uint16* wButtons, uint16 wDefID, uint32 dwProps, boolean bBox)
{
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   // If we are processing the flip event we don't want to save the data since the pointers that might be coming
   // in are the ones we have saved (are aliases) so bad things can happen
   if (pme->m_bRedrawingUI)
   {
      return;
   }

   FreePtr((void**)&(pme->m_pPromptTitle));
   FreePtr((void**)&(pme->m_pPromptText));
   FreePtr((void**)&(pme->m_pPromptButtons));

   if (NULL != szTitle )
   {
      pme->m_pPromptTitle = WSTRDUP(szTitle);
   }
   pme->m_wPromptTitleID = wIDTitle;
   if (NULL != szText )
   {
      pme->m_pPromptText = WSTRDUP(szText);
}
   pme->m_wPromptTextID = wIDText;
   if (wButtons)
   {
      size_t numButtons = 0;

      // Calculate number of items
      while(*(wButtons + numButtons))
      {
         numButtons++;
      }
      if (numButtons > 0)
      {
         pme->m_pPromptButtons = (uint16*)MALLOC((numButtons + 1) * sizeof(uint16));
         if (NULL != pme->m_pPromptButtons)
         {
            (void)MEMCPY(pme->m_pPromptButtons, wButtons, numButtons * sizeof(uint16));
            pme->m_pPromptButtons[numButtons] = 0;
         }
      }
   }
   pme->m_wPromptDefID = wDefID;
   pme->m_dwPromptProps = dwProps;
   pme->m_bPromptBox = bBox;
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
}

#ifdef USE_IDL2_DISABLED_SUPPORT

// Convert a valid DLITEMID to the class ID of the non-hidden app
// in the download item.

AEECLSID AppMgr_GetClassIDForDLItem(AppMgr* pme, DLITEMID itemID)
{
   AEECLSID result = 0;
   DL2AppModInfo* pAMI =
      IDOWNLOAD2_GetModInfoEx(pme->m_pDownload2, itemID, GMI_APPS);
   if (NULL != pAMI)
   {
      int index;
      AEEAppInfo* pAI = NULL;
      for (index = 0; index < pAMI->nApps; index++)
      {
         pAI = &(pAMI->apps[index]);
         if (0 == (pAI->wFlags & AEE_AFLAG_HIDDEN))
         {
            result = pAI->cls;
            break;
         }
      }
   }
   return result;
}

#endif // USE_IDL2_DISABLED_SUPPORT

#ifdef FEATURE_TOUCHSCREEN_SUPPORT

void AppMgr_AddSoftKey(AppMgr * pme)
{
   if(ST_MAINMENU != pme->m_wState)
   {
      //if we are not in main screen, we need to make sure the back button is presented
      //state changed
      boolean bMainMenuactive = FALSE;
      bMainMenuactive = (pme->m_pMenu && IMENUCTL_IsActive(pme->m_pMenu))? TRUE:FALSE;


      //boolean bStaticCtl;
      //bStaticCtl = (pme->m_pStatic && ISTATIC_IsActive(pme->m_pStatic))? TRUE:FALSE;
      
      //Favorites uses m_pMainMenu
      //bMainMenuactive |=(pme->m_pMenu && IMENUCTL_IsActive(pme->m_pMainMenu))? TRUE:FALSE;

      if(pme->m_bDispTouchEnabled && (bMainMenuactive || 
                                      (ST_FAVORITES == pme->m_wState ||
                                       ST_WORK == pme->m_wState ||
                                       ST_FUN == pme->m_wState) &&
                                      pme->m_cfg.bListView))
      {
         //if(pme->m_wState != ST_SCREENSAVERS)
         {
            uint16 id[1];
            id[0] = IDC_BACK;

            AppMgr_CreateSoftmenu(pme,id, 1);
            IMENUCTL_SetActive(pme->m_pSK, FALSE);
            IMENUCTL_Redraw(pme->m_pSK);
         }
         AppMgr_SetMenuRect(pme);
         //if (!pme->m_bHasDPad)
         {
            IMENUCTL_SetActive(pme->m_pMenu, FALSE);
         }
         IMENUCTL_SetActive(pme->m_pSK, FALSE);
         if (ST_FAVORITES == pme->m_wState ||
             ST_WORK == pme->m_wState ||
             ST_FUN == pme->m_wState)
         {
            //if (!pme->m_bHasDPad)
            {
               IMENUCTL_SetActive(pme->m_pMainMenu, FALSE);
            }
         }
      }
   }
   else
   {
      IMENUCTL_Reset(pme->m_pSK);
      IMENUCTL_SetActive(pme->m_pSK, FALSE);
   }
}

//===========================================================================
//
// DESCRIPTION
//    Sets the Rect for m_pMenu for when we have touch menus, 
//
//===========================================================================
void AppMgr_SetMenuRect(AppMgr * pme)
{
   AEERect rSoft, rRect;
   int nMenuHt; 
   int manageInfoHeight = 2*(pme->m_cyFont + MARGIN);
   IMenuCtl* pMenu;

   MEMSET(&rSoft, 0,  sizeof(AEERect));
   MEMSET(&rRect, 0,  sizeof(AEERect));

   // get menu and softkey rc
   if (ST_FAVORITES == pme->m_wState ||
       ST_WORK == pme->m_wState ||
       ST_FUN == pme->m_wState)
   {
      if (pme->m_cfg.bListView)
      {
         pMenu = pme->m_pMainMenu;
      }
      else
      {
         // We only want to do this for listview,
         // so exit
         return;
      }
   }
   else
   {
      pMenu = pme->m_pMenu;
   }

   if(pMenu)
     IMENUCTL_GetRect(pMenu, &rRect);
   if(pme->m_pSK)
	  IMENUCTL_GetRect(pme->m_pSK, &rSoft);

   // clear previous drawing
   IDISPLAY_EraseRect(pme->m_pIDisplay, &pme->m_rc);

#ifdef MSHOP_STATES
   if (pme->m_wState == ST_MSHOP)
   {
      if (pme->m_pMShopLogo)
      {
         IIMAGE_Draw(pme->m_pMShopLogo, pme->m_rcMShopLogo.x, pme->m_rcMShopLogo.y);
      }
   }
   else if (pme->m_wState == ST_MSHOPGUIDE)
   {
      AppMgr_MShopGuide(pme);
   }
#endif

   // Determine total area left for menuctl :excluding  softmenu and the pixelpad
   if(((rRect.y + rRect.dy) > (rSoft.y - TOUCH_PIXELPAD)) ||
      (((rRect.y + rRect.dy + manageInfoHeight) > (rSoft.y - TOUCH_PIXELPAD)) &&
       (!OEM_SMALL_SCREEN_DEVICE && (ST_MANAGETOOL == pme->m_wState))))
   {
      nMenuHt = (rSoft.y - TOUCH_PIXELPAD - rRect.y);

      // Adjust if we are on the ManageApps screen
      if (!OEM_SMALL_SCREEN_DEVICE && (ST_MANAGETOOL == pme->m_wState))
      {
         // reduce the menu height by the height of the text being displayed
         nMenuHt -= manageInfoHeight;
      }

     SETAEERECT(&rRect, rRect.x, rRect.y, rRect.dx, nMenuHt);
     IMENUCTL_SetRect(pMenu, &rRect);

   }

   // Redraw the Memory Info
   if (!OEM_SMALL_SCREEN_DEVICE && (pme->m_wState == ST_MANAGETOOL))
   {
      AppMgr_DisplayMemoryInfo(pme);
   }


   if(GetListViewMenuRect(pMenu, &rRect))
   {
      IMENUCTL_SetRect(pMenu, &rRect);
   }


   IMENUCTL_SetActive(pme->m_pSK, TRUE);
   IMENUCTL_SetActive(pMenu, TRUE);
   IMENUCTL_Redraw(pme->m_pSK);
   IMENUCTL_Redraw(pMenu);
}


//===========================================================================
//
// DESCRIPTION
//    Create the Softmenu for Touch Screen displays.
//
// PARAMETERS
//    resID: An array of resouce IDs to load for each button
//    nCount: Then number of buttons/size of the resID array.
//
//
//===========================================================================
void AppMgr_CreateSoftmenu(AppMgr * pme, uint16* resID, int16 nCount)
{
   AEERect rRect;
   CtlAddItem ai;
   int  nStrWidth, nTotalWidth;
   int  pnFits, nSoftmenuWidth;
   AECHAR	 szTemp[128];
   int i;
   int nMaxWidth;
   uint32 dwProps;

   // Clear current
   IMENUCTL_Reset(pme->m_pSK);


   dwProps = IMENUCTL_GetProperties(pme->m_pSK);
   IMENUCTL_SetProperties(pme->m_pSK, dwProps|MP_NO_ARROWS|MP_MAXSOFTKEYITEMS);

   // Initialize ai
   for(i = 0; i < nCount; i++)
   {
      MEMSET(&ai, 0, sizeof(ai));
      ai.pszResText  = BREWAPPMGRLS_RES_LOC;
      ai.wText = resID[i];
      ai.wItemID = resID[i];

      IMENUCTL_AddItemEx(pme->m_pSK, &ai);
   }

   // override the rect area, if rect holds the values for BACK button in the previous call
   rRect.x = 0;
   rRect.dx =  pme->m_rc.dx;
   rRect.y = pme->m_rc.dy - (TOUCH_SOFTMENU_HEIGHT + TOUCH_PIXELPAD );
   rRect.dy = TOUCH_SOFTMENU_HEIGHT;

   // get with for all strings
   nTotalWidth = 0;
   nMaxWidth = 0;
   for(i = 0; i < nCount; i++)
   {
      // CR90583 fix: Resizing softmenu width dynamically according to string being displayed; Set width to default TOUCH_SOFTMENU_WIDTH if the string is smaller - for being tangible!!!
      ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, resID[i], szTemp, sizeof(szTemp));
      nStrWidth = IDisplay_MeasureTextEx(pme->m_pIDisplay,
                              AEE_FONT_NORMAL,  
                              szTemp,
                              -1,
                              -1, 
                              &pnFits);
      
      nTotalWidth += nStrWidth;
   }


   nSoftmenuWidth = ((((nTotalWidth * 2) + (TOUCH_PIXELPAD * 2) * nCount) < TOUCH_SOFTMENU_WIDTH ) ? TOUCH_SOFTMENU_WIDTH :((nTotalWidth * 2) + (TOUCH_PIXELPAD * 2) * nCount));

   if(nCount > 1)
   {
      rRect.x =  pme->m_rc.x;
      rRect.dx =  pme->m_rc.dx;
   }
   else
   {
      if (resID[0] == IDC_BACK || resID[0] == IDC_CANCEL)
      {
         rRect.x = pme->m_rc.dx - nSoftmenuWidth;
      }
      else
      {
         rRect.x  = 0;
      }

      rRect.dx =  nSoftmenuWidth;
   }


   IMENUCTL_SetRect(pme->m_pSK, &rRect);

   // Redraw Softkey
   IMENUCTL_Redraw(pme->m_pSK);

   IDISPLAY_Update(pme->m_pIDisplay);
}

#endif //#ifdef FEATURE_TOUCHSCREEN_SUPPORT

/*@@@{ Tue Feb 12 08:20:26 PST 2008 


//===========================================================================
//
// DESCRIPTION
//    Determines whether BAM is allowed to run on the specified display
//    based on the OEMCFG information available (or its default).  If the
//    specified display ID is APP_MGR_CURRENT_DISPLAY the check is made on
//    the current display (see code for how the current display is
//    determined).
//
// PARAMETERS
//
//    pme - the usual
//
//    clsDisplay - display ID of the display to be checked, or 0 to
//    indicate that the current display should be checked.  The only
//    acceptable values other than APPMGR_CURRENT_DISPLAY are:
//
//    AEECLSID_DISPLAY1
//    AEECLSID_DISPLAY2
//    AEECLSID_DISPLAY3
//    AEECLSID_DISPLAY4
//
//
//===========================================================================

static boolean AppMgr_CanUseDisplay(AppMgr* pme, AEECLSID clsDisplay)
{
   int displayIndex = 0;
   boolean result = FALSE;

   // Default to display 1
   AEECLSID checkDisplayID = AEECLSID_DISPLAY1;

   // If APPMGR_CURRENT_DISPLAY is specified use the current display's ID.
   // Stick with the default if unable to determine the current display
   if (APPMGR_CURRENT_DISPLAY == clsDisplay)
   {
#ifdef FEATURE_HAS_MULTIPLE_DISPLAYS
      if (pme->m_bHasMultipleDisplays)
      {
         // Use the IFlip interface to determine the current display ID.
         // If we can't use IFlip for any reason fall back to the default.
         IFlip* pIFlip = NULL;
         int tempResult = CreateInstance(pme->a.m_pIShell,
            AEECLSID_FLIP, (void **)&pIFlip);
         if (SUCCESS == tempResult)
         {
            int32 numDisplays = 0;
            tempResult = IFLIP_GetDisplayList(pIFlip, NULL, &numDisplays);
            if (SUCCESS == tempResult)
            {
               if (0 < numDisplays)
               {
                  AEECLSID* idList = (AEECLSID*)
                     MALLOC(sizeof(AEECLSID) * numDisplays);
                  if (NULL != idList)
                  {
                     tempResult = IFLIP_GetDisplayList(pIFlip, (void*)idList,
                        &numDisplays);
                     if (SUCCESS == tempResult)
                     {
                        FARF(BAM, ("CanUseDisplay() returning IFlip value"));
                        checkDisplayID = idList[0];
                     }
                     else
                     {
                        FARF(BAM, ("Second GetDisplayList() failed"));
                     }
                     FreePtr((void**) &idList);
                  }
                  else
                  {
                     FARF(BAM, ("Unable to allocate space for display list"));
                  }
               }
            }
            else
            {
               FARF(BAM, ("First GetDisplayList() failed"));
            }
            FreeObj((void**)& pIFlip);
         }
         else
         {
            FARF(BAM, ("Unable to create IFlip"));
         }
      }
      else
      {
         // if there aren't multiple displays stick with the default
      }
#else
   // If the multiple display feature isn't supported we assume that
   // there is one display and use the default.
#endif
   }
   else
   {
      // Use the specified display ID
      checkDisplayID = clsDisplay;
   }

   // At this point we should be looking at a well known display ID.
   // Map it to a value we can use in the OEMCFG bit twiddling macros.
   switch (checkDisplayID)
   {
      case AEECLSID_DISPLAY1:
         FARF(BAM, ("Check enabled: display 1"));
         displayIndex = 1;
         break;
      case AEECLSID_DISPLAY2:
         FARF(BAM, ("Check enabled: display 2"));
         displayIndex = 2;
         break;
      case AEECLSID_DISPLAY3:
         FARF(BAM, ("Check enabled: display 3"));
         displayIndex = 3;
         break;
      case AEECLSID_DISPLAY4:
         FARF(BAM, ("Check enabled: display 4"));
         displayIndex = 4;
         break;
      default:
         FARF(BAM, ("Invalid display index"));
         return result;
   }

   // Ultimately the result is based on OEM config info...
   result = IS_BAM_ENABLED_DISPLAY(displayIndex, pme->m_wEnabledDisplays);
   return result;
}

}@@@*/


void AppMgr_SetEndKeyFlg(boolean bFlg)
{
    gbENDKey_RecievedInSuspend = TRUE;
}





