/*======================================================

                    IMPORTANT NOTICE

The contents of this file are provided under legal agreement
between QUALCOMM Incorporated and selected licensed carrier
and handset OEM partners. Under the terms of that agreement, 
this file MUST NOT be distributed in full or in part to anyone
not specifically authorized by QUALCOMM Incorporated.

FILE:  MobileShop.c

SERVICES:  Mobile Download Applet

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

        Copyright ?2000-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "MobileShop.h"
#include "BrewAppMgrSettings.h"
#include "AEEFile.h"
#include "appscommon.h"

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
#include "AEEPointerHelpers.h"
#endif // FEATURE_TOUCHSCREEN_SUPPORT

#ifdef FEATURE_DISABLE_SS_DURING_DOWNLOAD
#include "AEEMimeTypes.h"
#endif // FEATURE_DISABLE_SS_DURING_DOWNLOAD

#include "AEECLSID_FLIP.bid"
#ifdef USE_IDL2
#include "AEEDownload2.bid"
#endif

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

#define FARF_MS     1 
#define FARF(x, p)

//#define BREWAPPMGRLS_RES_LOC AEE_RES_LANGDIR BREWAPPMGRLS_RES_LOC
//#define BREWAPPMGRLS_RES_LOC "brewappmgrls"AEE_RES_LANGSUF".bar"
#define BREWAPPMGRLS_RES_LOC BREWAPPMGRLS_RES_FILE

// this function will add softmenu buttons
static void MShop_AddSoftKey(MShop* pme);

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
static void MShop_SetMenuRect(MShop* pme);
static void MShop_CreateSoftmenu(MShop * pme, uint16* resID, int16 nCount);
#endif 

extern void AppMgr_SetEndKeyFlg(boolean bFlg);
//===========================================================================
//
// DESCRIPTION
//    MShop CreateInstance
// 
// PARAMETERS
//    ClsId:   Class Identifier
//    pIShell: IShell Pointer
//    po:      IModule Pointer
//    ppObj:   Place holder for MShop object
//
//===========================================================================
int MShopClsCreateInstance(AEECLSID ClsId,IShell* pIShell, IModule* po,
   void** ppObj)
{
   MShop*       pme = NULL;
   AEEDeviceInfo  di;
   IFlip       *pIFlip = NULL;
   AEECLSID    *pClsList = NULL;
   AEEFlipInfo *pFlipInfo = NULL;
   int32       dwNumDisplays = 0;
   int16       wNumFlips  = 0;
   int16       i = 0;
   AEECLSID newDisplayId = AEECLSID_DISPLAY1;

   *ppObj = NULL;

   // Create MShop Object
   if (!AEEApplet_New(sizeof(MShop), ClsId, pIShell, po, (IApplet**)ppObj,
      (AEEHANDLER)MShop_HandleEvent,(PFNFREEAPPDATA)MShop_Free))
   {
      return(ENOMEMORY);
   }

   pme = (MShop *)*ppObj;

   pme->m_pIDisplay = NULL;
   pme->m_pIDisplay2 = NULL;


   // Initialize State
   pme->m_wState = NO_ENTRY;

   // Initialize ponter so NULL
   pme->m_pMenu = NULL;
   pme->m_pSK = NULL;
#ifdef FEATURE_UPGRADE_AUTO_PROMPT
   pme->m_pUm = NULL;
#endif // FEATURE_UPGRADE_AUTO_PROMPT
   pme->m_pStatus = NULL;
   pme->m_pStatic = NULL;
   pme->m_pStaticDlInfo = NULL;
   pme->m_pText = NULL;
   pme->m_pDownload = NULL;
#ifdef USE_IDL2
   pme->m_pDownload2 = NULL;
#endif
   pme->m_pINetMgr = NULL;
   pme->m_pItem = NULL;
   pme->m_pPrice = NULL;
   pme->m_pLogBuff = NULL;
   pme->m_pszEULA = NULL;

   pme->m_bInitStatus = FALSE;
   pme->m_bDownloading = FALSE;
   pme->m_bGotStatus = FALSE;
   pme->m_bDemo = FALSE;
   pme->m_bSubscription = FALSE;
   pme->m_bSearch = FALSE;
   pme->m_bSearchAny = FALSE;
   pme->m_bUpgradeCheck = FALSE;
   pme->m_bGotEULA = FALSE;
   pme->m_bAIPrompt = FALSE;
   pme->m_bRepaint = FALSE;
   pme->m_bCarrierMessage = FALSE;
   pme->m_bDisplayInfo = FALSE;
#ifdef MSHOP_CMD_ITEMINFO
   pme->m_bDisplayInfoOnly = FALSE;
#endif

   pme->m_bVerify = FALSE;

#if defined USE_TT_FONTS
   pme->m_bNormalFontSaved = FALSE;
   pme->m_bBoldFontSaved = FALSE;
#endif // USE_TT_FONTS

#ifdef FEATURE_HAPTIC_SUPPORT
   pme->m_pHapticResponse = NULL;
   // ToneData would be read from oemcfg.txt, To start with, initialize with 0.
   pme->m_ToneData.eTone = 0;
   pme->m_ToneData.wDuration = 0;
#endif // FEATURE_HAPTIC_SUPPORT

   pme->m_bConfirmDownload = FALSE;
   pme->m_bExitOnCancel = FALSE;

#ifdef MSHOP_CMD_ACQUIRE
   pme->m_pAcquireInfo = NULL;
#endif // MSHOP_CMD_ACQUIRE

   MEMSET(pme->m_catList, 0, sizeof(pme->m_catList));

#if defined USE_TT_FONTS
   // No saved fonts yet.
   pme->m_pSaveNormalFont = NULL;
   pme->m_pSaveBoldFont = NULL;
   pme->m_bNormalFontSaved = FALSE;
   pme->m_bBoldFontSaved = FALSE;
#endif // USE_TT_FONTS

   pme->m_wTouchDisplays = 0;       // Default value, no displays are touch
   pme->m_wEnabledDisplays = 0x01;  // Default value, only run on display1
   pme->m_wNotEnoughKeys = 0;       // Default value, all displays have enough keys to run on.

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

   pme->m_bSuspending = FALSE;
   pme->m_bResuming = FALSE;
   pme->m_bFlipOpenWhenSuspended = TRUE;
   pme->m_bExitAfterDownload = FALSE;
   pme->m_bRedrawingUI = FALSE;

   pme->m_bDispTouchEnabled  = FALSE;

#ifdef FEATURE_DISABLE_SS_DURING_DOWNLOAD
   pme->m_clsScreenSaver = 0;
#endif // FEATURE_DISABLE_SS_DURING_DOWNLOAD

#ifdef FEATURE_SILENT_UPGRADE_CHECK
   pme->iID = 0;
   pme->pszURL = NULL;
   pme->m_pszUrlPostAtExit = NULL;
#endif // FEATURE_SILENT_UPGRADE_CHECK

#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   pme->cbDisplayEnabled = NULL;
   MEMSET(&pme->m_cbUpdateUI, '\0', sizeof(pme->m_cbUpdateUI));
   pme->m_bPromptIsDisplayed = FALSE;
   pme->m_pTitleResFile = NULL;
   pme->m_wTitleResId = 0;
   pme->m_pTitleText = NULL;
   pme->m_pMenuResFile = NULL;
   pme->m_wMenuResID = 0;
   pme->m_pMenuText = NULL;
   pme->m_pStaticTitle = NULL;
   pme->m_pStaticText = NULL;
   pme->m_bStaticSizeToFitCalled = FALSE;
   pme->m_pPromptTitle = NULL;
   pme->m_pPromptText = NULL;
   pme->m_pPromptButtons = NULL;
   pme->m_pPromptItemIDs = NULL;
   pme->m_wExDisplayResId = 0;
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS



#ifdef FEATURE_CANCELONCLOSE
   // When user flips or slides the phone closed, cancel the current operation and exit
   pme->m_bCancelOnClose = TRUE;
#endif

#ifdef FEATURE_HAS_MULTIPLE_DISPLAYS
   // When user flips or slides the phone closed, cancel the current operation and exit
   pme->m_bHasMultipleDisplays = FALSE;
#endif

   if (CreateInstance(pIShell, AEECLSID_FILEMGR,
      (void**)&pme->m_pIfm) != AEE_SUCCESS)
   {
      IAPPLET_Release((IApplet *)pme);
      *ppObj = NULL;
      return(EFAILED);
   }

#ifdef FEATURE_OEMCFG
   if (!MShop_LoadOEMCfg(pme))
   {
      FreeObj((void**)&pme->m_pIfm);
      IAPPLET_Release((IApplet *)pme);
      *ppObj = NULL;
      return(EFAILED);
   }
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
               FARF(MS, ("Unable to get the display IDs for this flip"));
            }
            else
            {
               newDisplayId = pClsList[0];
               FARF(MS, ("IFlip told us we are using %d, and that there are %d displays", newDisplayId, dwNumDisplays));
               // We need to do two things.  
               // 1) make sure that we are drawing to the correct display
               // Display1 is currently "active" according to BREW, but we may need to set another display as active
               // so that we actualy display on the proper display.
               // 2) Figure out if there are any key mappings that need to happen.

               MShop_SetKeyList(pme, pIFlip, newDisplayId);

               if (SUCCESS == CreateInstance(pIShell, newDisplayId, (void**)&(pme->m_pIDisplay)))
               {
                  IDISPLAY_MakeDefault(pme->m_pIDisplay);

                  if (!IDISPLAY_IsEnabled(pme->m_pIDisplay))
                  {
                     // If our display is not yet enabled, we need to wait for it to be enabled before
                     // trying to draw on it.
                     FARF(MS, ("Our display is not enabled, we are enabling it"));
                     pme->cbDisplayEnabled = (AEECallback*) MALLOC(sizeof(AEECallback));
                     MEMSET(pme->cbDisplayEnabled, 0, sizeof(AEECallback));
                     CALLBACK_Init(pme->cbDisplayEnabled, MShop_EnableDispCallback, pme);
                     IDISPLAY_NotifyEnable(pme->m_pIDisplay, pme->cbDisplayEnabled);
                  }
               }
               else
               {
                  FARF(MS, ("Unable to create display that we should be using, exiting."));
                  FreeObj((void**)&pme->m_pIfm);
                  IAPPLET_Release((IApplet *)pme);
                  *ppObj = NULL;
                  return EFAILED;
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
                     FARF(MS, ("Display index %d has ID %d, x = %d, y = %d, z = %d", i, DEVNOTIFY_GET_ID(pFlipInfo[i]),
                                 DEVNOTIFY_GET_X(pFlipInfo[i]), DEVNOTIFY_GET_Y(pFlipInfo[i]), DEVNOTIFY_GET_Z(pFlipInfo[i])));
                  }
               }
            }
         }
         else
         {
            FARF(MS, ("There are no Flips for this device"));
            
         }
      }
      else
      {
         FARF(MS, ("There are no displays available for this flip"));
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

#if defined(MSHOP_CMD_SEARCH_STRING) || defined(MSHOP_CMD_CATEGORY)
    pme->m_pszSearch = (AECHAR *)MALLOC(MAX_STR_SIZE);
    pme->m_pszTitle = (AECHAR *)MALLOC(MAX_STR_SIZE);
#endif

#ifdef USE_TT_FONTS
   // Install the TrueType fonts here, before making the call to
   // GetFontMetrics() and before any menu controls are instantiated.
   MShop_InstallTTFonts(pme);
#endif // USE_TT_FONTS


   // Create IDownload & Control Interfaces
#ifdef USE_IDL2
   if (CreateInstance(pIShell, AEECLSID_DOWNLOAD2,
      (void**)&pme->m_pDownload2) != AEE_SUCCESS)
   {
      FreeObj((void**)&pme->m_pIfm);
      IAPPLET_Release((IApplet *)pme);
      *ppObj = NULL;
      return(EFAILED);
   }

   if (IDOWNLOAD2_QueryInterface(pme->m_pDownload2, AEECLSID_DOWNLOAD, 
                                 (void**)&pme->m_pDownload) != AEE_SUCCESS)
   {
      FreeObj((void**)&pme->m_pDownload2);
      FreeObj((void**)&pme->m_pIfm);
      IAPPLET_Release((IApplet *)pme);
      *ppObj = NULL;
      return(EFAILED);
   }
#else
   if (CreateInstance(pIShell, AEECLSID_DOWNLOAD,
      (void**)&pme->m_pDownload) != AEE_SUCCESS)
   {
      FreeObj((void**)&pme->m_pIfm);
      IAPPLET_Release((IApplet *)pme);
      *ppObj = NULL;
      return(EFAILED);
   }
#endif // USE_IDL2

   // It is not the end of the world if we fail to create the INetMgr.
   (void)CreateInstance(pIShell, AEECLSID_NET, (void **)(&pme->m_pINetMgr));


#ifdef FEATURE_TOUCHSCREEN_SUPPORT
   // newDisplayId is the current active display, check whether it is touch enabled or not
   if(newDisplayId)
   {
      pme->m_bDispTouchEnabled = IsDisplayTouchEnabled(newDisplayId, pme->m_wTouchDisplays);
      FARF(MS, ("newDisplayId = pme->m_bDispTouchEnabled = %d   &   %d", newDisplayId, pme->m_bDispTouchEnabled));
   }
#endif

   if(pme->m_bDispTouchEnabled)
   {
      IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_BACKGROUND, TOUCHSCREEN_BACKGROUND);
   }

#ifdef FEATURE_HAPTIC_SUPPORT
   if(ISHELL_CreateInstance(pIShell, AEECLSID_Sound, (void**)&pme->m_pHapticResponse) != AEE_SUCCESS)
   {
      FreeObj((void**)&pme->m_pIfm);
      FreeObj((void**)&pme->m_pDownload);
#ifdef USE_IDL2
      FreeObj((void**)&pme->m_pDownload2);
#endif
      FreeObj((void**)&pme->m_pINetMgr);
      IAPPLET_Release((IApplet *)pme);
      *ppObj = NULL;
      return (EFAILED);
   }
#endif


   if((CreateInstance(pIShell, GetMenuCtlClassID(pme->m_bDispTouchEnabled),
      (void **)&pme->m_pMenu) != AEE_SUCCESS) ||
      (CreateInstance(pIShell, GetSoftKeyClassID(pme->m_bDispTouchEnabled),
      (void **)&pme->m_pSK) != AEE_SUCCESS))
   {
#ifdef FEATURE_HAPTIC_SUPPORT
      FreeObj((void**)&pme->m_pHapticResponse);
#endif
      FreeObj((void**)&pme->m_pIfm);
      FreeObj((void**)&pme->m_pMenu);
      FreeObj((void**)&pme->m_pSK);
      FreeObj((void**)&pme->m_pDownload);
#ifdef USE_IDL2
      FreeObj((void**)&pme->m_pDownload2);
#endif
      FreeObj((void**)&pme->m_pINetMgr);
      IAPPLET_Release((IApplet *)pme);
      *ppObj = NULL;
      return(EFAILED);
   }

   // Instantiate the UpgradeMgr if the upgrade auto-prompt feature
   // is enabled. This instance should not try to update its data store.
#if defined(FEATURE_UPGRADE_AUTO_PROMPT)
   if (AEE_SUCCESS != CreateInstance(pme->a.m_pIShell,
      AEECLSID_UPGRADEMGR, (void**)&(pme->m_pUm)))
   {
#ifdef FEATURE_HAPTIC_SUPPORT
      FreeObj((void**)&pme->m_pHapticResponse);
#endif
      FreeObj((void**)&pme->m_pIfm);
      FreeObj((void**)&pme->m_pMenu);
      FreeObj((void**)&pme->m_pSK);
      FreeObj((void**)&pme->m_pDownload);
#ifdef USE_IDL2
      FreeObj((void**)&pme->m_pDownload2);
#endif
      FreeObj((void**)&pme->m_pINetMgr);
      IAPPLET_Release((IApplet *)pme);
      *ppObj = NULL;
      return(EFAILED);
   }
#endif // FEATURE_UPGRADE_AUTO_PROMPT

   // Initialize Category Index
   pme->m_nCatIdx = -1;

   // Initilize Device Info
   ISHELL_GetDeviceInfo(pIShell, &di);
   pme->m_cx = di.cxScreen;
   pme->m_cy = di.cyScreen;
   pme->m_nColorDepth = di.nColorDepth;

#ifdef FEATURE_SCREEN_ROTATE
   // Our screen is landscape if cx is > cy (this implies that dx = dy is a 
   // portrait screen
   if (pme->m_cx > pme->m_cy)
   {
      pme->m_orentDisplayDefault = AEE_SO_LANDSCAPE;
   }
   else
   {
      pme->m_orentDisplayDefault = AEE_SO_PORTRAIT;
   }
#endif //FEATURE_SCREEN_ROTATE

   // Initialize Font
   pme->m_cyFont = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, AEE_FONT_BOLD, NULL, NULL);

#ifdef FEATURE_HAS_MULTIPLE_DISPLAYS
#ifdef AEE_SIMULATOR
   // If we are in the simulator, always default to TRUE as we have no way of knowing
   pme->m_bHasMultipleDisplays = TRUE;
#else
   pme->m_bHasMultipleDisplays = di.bAltDisplay;
#endif // AEE_SIMULATOR
#endif // FEATURE_HAS_MULTIPLE_DISPLAYS

   // Initilize IDownload interface with callback function & context.
   // Callback is called when any activity occurs as a result of calls to
   // the IDownload class.
   IDOWNLOAD_OnStatus(pme->m_pDownload, (PFNDLSTATUS)(MShop_Status),pme);

   // Set style & color for menu 
   SetMenuStyleAndColor(pme->m_pMenu, GetMenuCtlClassID(pme->m_bDispTouchEnabled), pme->m_nColorDepth, pme->m_bDispTouchEnabled);
   SetMenuStyleAndColor(pme->m_pSK, GetSoftKeyClassID(pme->m_bDispTouchEnabled), pme->m_nColorDepth, pme->m_bDispTouchEnabled);

   // Create State Machine by specifying state (index in state machine array) and
   // members of state handler struct
   SET_STATE(pme, ST_CATEGORIES,        MShop_Enum, NO_ENTRY, FALSE);
   SET_STATE(pme, ST_APP_OPTIONS,       MShop_AppOptionsDialog, ST_CATEGORIES, TRUE);
   SET_STATE(pme, ST_CHECK_SPACE,       MShop_CheckSpaceDialog, ST_APP_OPTIONS, TRUE);   
   SET_STATE(pme, ST_PURCHASE_CONFIRM,  MShop_ConfirmPurchaseDialog, ST_APP_OPTIONS, TRUE);
   SET_STATE(pme, ST_PURCHASE_EULA,     MShop_GetEULA, ST_APP_OPTIONS, FALSE);
   SET_STATE(pme, ST_PURCHASE,          MShop_Purchase, ST_APP_OPTIONS, FALSE);
   SET_STATE(pme, ST_PURCHASE_RESUME,   MShop_PurchaseResume, ST_CHECK_SPACE, FALSE);
   SET_STATE(pme, ST_RESTORE_APP,       MShop_Withdraw, NO_ENTRY, FALSE);
   SET_STATE(pme, ST_VIEW_LOG,          MShop_LogDialog, NO_ENTRY, TRUE);
   SET_STATE(pme, ST_SERVER,            MShop_SetServerDialog, NO_ENTRY, TRUE);
#ifdef SEARCH_FROM_CATALOG
   SET_STATE(pme, ST_SEARCH,            MShop_SearchDialog, ST_CATEGORIES, TRUE);
#else             
   SET_STATE(pme, ST_SEARCH,            MShop_SearchDialog, NO_ENTRY, TRUE); // This is here so we still have a ST_SEARCH entry
#endif // SEARCH_FROM_CATALOG
   SET_STATE(pme, ST_SEARCH_CMD,        MShop_SearchDialog, NO_ENTRY, TRUE);
   SET_STATE(pme, ST_CREDITBACK,        MShop_CreditBackDialog, NO_ENTRY, TRUE);
   SET_STATE(pme, ST_IN_PROGRESS,       MShop_InProgress, NO_ENTRY, TRUE);
#ifdef MSHOP_CMD_SEARCH_STRING
   SET_STATE(pme, ST_SEARCHSTRING,      MShop_SearchString, NO_ENTRY, TRUE);
#endif

#ifdef LOAD_MSHOP_LOGO
   //Load MShopLogo
   pme->m_pMShopLogo = ISHELL_LoadResImage(pme->a.m_pIShell, BREWAPPMGRLN_RES_FILE, IDB_MSHOP_THUMB);
#endif

   if (pme->m_pMShopLogo)
   {
      AEEImageInfo ii;
      IIMAGE_GetInfo(pme->m_pMShopLogo, &ii);
      SETAEERECT(&pme->m_rcMShopLogo, 0, 0, ii.cx, ii.cy);
   }
   else
   {
      SETAEERECT(&pme->m_rcMShopLogo, 0, 0, 0, pme->m_cyFont);
   }

   if (OEM_UNDERLINE_MENU_TITLE)
   {
      pme->m_rcMShopLogo.dy += 2;
   }


   // Show Busy between UI states by default
   pme->m_bShowBusy = TRUE;

   return(AEE_SUCCESS);
}

/*===========================================================================

Releases MobileShop

===========================================================================*/
static void MShop_Free(MShop * pme)
{
   FARF(MS, ("In MShop_Free"));
   // Cancel pending timers
   ISHELL_CancelTimer(pme->a.m_pIShell, NULL, pme);

   // Free text/static controls
   FreeObj((void **)&pme->m_pStatic);
   FreeObj((void **)&pme->m_pStaticDlInfo);
   FreeObj((void **)&pme->m_pText);

#ifdef FEATURE_SILENT_UPGRADE_CHECK
   FreePtr((void**)&pme->pszURL);
   FreePtr((void**)&pme->m_pszUrlPostAtExit);;
#endif // FEATURE_SILENT_UPGRADE_CHECK

   // Free EULA buffer
   FreePtr((void **)&pme->m_pszEULA);
   pme->m_bGotEULA = FALSE;

#if defined(MSHOP_CMD_SEARCH_STRING) || defined(MSHOP_CMD_CATEGORY)
   // Free Search buffer
   FreePtr((void **)&pme->m_pszSearch);
   FreePtr((void **)&pme->m_pszTitle);
#endif

#ifdef MSHOP_CMD_ACQUIRE
   FreePtr((void**)&pme->m_pAcquireInfo);
#endif // MSHOP_CMD_ACQUIRE

   // Reset category list
   MShop_EnumCleanup(pme);

#ifdef USE_TT_FONTS
   // Restore the original fonts
   MShop_RestoreFonts(pme);
#endif // USE_TT_FONTS

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
   FreePtr((void**)&pme->m_pTitleResFile);
   FreePtr((void**)&pme->m_pTitleText);
   FreePtr((void**)&(pme->m_pMenuResFile));
   FreePtr((void**)&(pme->m_pMenuText));
   FreePtr((void**)&(pme->m_pStaticTitle));
   FreePtr((void**)&(pme->m_pStaticText));
   FreePtr((void**)&(pme->m_pPromptTitle));
   FreePtr((void**)&(pme->m_pPromptText));
   FreePtr((void**)&(pme->m_pPromptButtons));
   FreePtr((void**)&(pme->m_pPromptItemIDs));
#endif

   FreeObj((void**)&pme->m_pDownload);
#ifdef USE_IDL2
   FreeObj((void**)&pme->m_pDownload2);
#endif

   // If MSHOP_PPP_LINGER is not defined as -1, make it default linger and
   // release IDownload interface
   if (pme->m_pINetMgr)
   {
#if MSHOP_PPP_LINGER >= 0
      INETMGR_SetLinger(pme->m_pINetMgr, (uint16)MSHOP_PPP_LINGER);         
#endif
      FreeObj((void**)&pme->m_pINetMgr);
   }

   // Cancel timers used for adding items to log menu and free log buffer
   MShop_FreeLogBuff(pme);

   // Free menu controls
   FreeObj((void **)&pme->m_pMenu);
   FreeObj((void **)&pme->m_pSK);
   FreeObj((void **)&pme->m_pStatus);

   // Free Logo
   FreeObj((void **)&pme->m_pMShopLogo);

#ifdef FEATURE_UPGRADE_AUTO_PROMPT
   // Release the UpgradeMgr, if any
   if (NULL != pme->m_pUm)
   {
      IUM_Release(pme->m_pUm);
   }
#endif // FEATURE_UPGRADE_AUTO_PROMPT

   FreeObj((void **)&pme->m_pIDisplay2);

   if (NULL != pme->m_pIDisplay)
   {
      IDISPLAY_Release(pme->m_pIDisplay);
      pme->m_pIDisplay = NULL;
   }


   FreeObj((void**)&pme->m_pIfm);

   // Free cached download item info, if any
   if (NULL != pme->m_pItem)
   {
      FreeDLItem(pme->m_pItem);
   }

   // Free cached Price info, if any
   if (NULL != pme->m_pPrice)
   {
      FreePrice(&pme->m_pPrice);
   }

#ifdef FEATURE_HAPTIC_SUPPORT
   FreeObj((void**)&pme->m_pHapticResponse);
#endif
}

/*===========================================================================

Clears startup arguments for MobileShop

===========================================================================*/
static void MShop_ClearStartArgs(MShop * pme)
{
   pme->m_clsOpenAtExit = (AEECLSID)0;
#ifdef FEATURE_SILENT_UPGRADE_CHECK
   FreePtr((void**)&pme->m_pszUrlPostAtExit);
#endif // FEATURE_SILENT_UPGRADE_CHECK
   pme->m_iApp = 0;
}

/*===========================================================================

Close MobileShop and don't start any other applet. 

===========================================================================*/
static void MShop_ForceExit(MShop * pme)
{
   // Stop any download activity
   IDOWNLOAD_Cancel(pme->m_pDownload);

   // Close the app but do not run any other
   MShop_ClearStartArgs(pme);

   ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
}

/*===========================================================================

 Event handler for MobileShop

===========================================================================*/
static boolean MShop_HandleEvent(MShop * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   boolean bRet = FALSE;

   if(NULL == pme)
   {
     FARF(MS, ("MShop_HandleEvent, NULL ptr."));
     return bRet;
   }
   // 中国电信要求，但会修改mif文件，暂做成通用功能
   if ((EVT_KEY == eCode) && (wParam == AVK_END))
   {
       AppMgr_SetEndKeyFlg(TRUE);
       MShop_ForceExit(pme);
       return TRUE;
   }

   // IF THINGS START GOING TERRIBLY WRONG WHEN PRESSING KEYS THIS IS THE FIRST
   // PLACE TO LOOK.  WE ARE OVERRIDING THE ACTUAL KEY PRESSED WITH THE ONE
   // WE WANT THAT KEY PRESS TO ACT LIKE.
#ifdef FEATURE_LESS_THAN_6_KEYS
   // We we got a key press and we need to use alternate keys
   // change the key code to be the one that it maps to
   // for this display
   if ((EVT_KEY == eCode) && (pme->m_bUseAltKeys))
   {
      if (wParam == pme->m_wAltKeyUp)
      {
         FARF(MS, ("mapping %d to AVK_UP", wParam));
         wParam = AVK_UP;
      }
      else if (wParam == pme->m_wAltKeyDown)
      {
         FARF(MS, ("mapping %d to AVK_DOWN", wParam));
         wParam = AVK_DOWN;
      }
      else if (wParam == pme->m_wAltKeySelect)
      {
         FARF(MS, ("mapping %d to AVK_SELECT", wParam));
         wParam = AVK_SELECT;
      }
      else if (wParam == pme->m_wAltKeyClr)
      {
         FARF(MS, ("mapping %d to AVK_CLR", wParam));
         wParam = AVK_CLR;
      }
   }
#endif //FEATURE_LESS_THAN_6_KEYS

   // If the user has closed the flip/slide while downloading
   // we want to ignore all Key and pointer events
   // until they re-open the device
   if (pme->m_bExitAfterDownload)
   {
      switch (eCode)
      {
         case EVT_POINTER_DOWN:
         case EVT_POINTER_UP:
         case EVT_POINTER_MOVE:
            // Ignore the event
            return FALSE;

         case EVT_KEY:
            if (AVK_CLR == wParam)
            {
               // We got the clr key, so exit BREW
               //ISHELL_CloseApplet(pme->a.m_pIShell, TRUE);
               MShop_ForceExit(pme);
               return TRUE;
            }
            return FALSE;

         default:
            // Continue letting the function handle the event
            break;
      }

   }



   if (!pme->m_bDispTouchEnabled)
   {
      // Handle softkey events for a non text control softkey
      if(IMENUCTL_HandleEvent(pme->m_pSK, eCode, wParam, dwParam))
      {
         return(TRUE);
      }

      // Handle standard menu events, unless we are in the "In Progress" state
      if((ST_IN_PROGRESS != pme->m_wState) && IMENUCTL_HandleEvent(pme->m_pMenu, eCode, wParam, dwParam))
      {
         return(TRUE);
      }
   }

   // Handle text control events
   if(pme->m_pText && ITEXTCTL_HandleEvent(pme->m_pText, eCode, wParam, dwParam))
   {
      // This is when Text Control returns from its mode/symbol selection menus
      if (ITEXTCTL_IsActive(pme->m_pText) && pme->m_bRepaint) {
         uint16 titleId = IDS_CREDITBACK_TITLE;
         if ((pme->m_wState == ST_SEARCH) || (pme->m_wState == ST_SEARCH_CMD))
         {
            titleId = IDS_SEARCH_TITLE;
         }
         ITEXTCTL_SetTitle(pme->m_pText, BREWAPPMGRLS_RES_LOC, titleId, NULL);
		 MShop_SetTitle(pme, BREWAPPMGRLS_RES_LOC, (uint16)( (titleId == IDS_SEARCH_TITLE) ? IDC_SEARCH_ANY : IDS_CREDITBACK_TITLE), NULL);	//CR96116: Set the correct MShop title.	 

         // Reset repaint flag
         pme->m_bRepaint = FALSE;
      }
      else
      {
         // If Select key or Clear key press made Text Control inactive and softkey active
         // Change Title as per softkey item
         if (((wParam == AVK_SELECT) || (wParam == AVK_CLR)) && IMENUCTL_IsActive(pme->m_pSK))
         {                        
            uint16 titleId = IDC_MSHOP_CREDITBACK;
            if ((pme->m_wState == ST_SEARCH) || (pme->m_wState == ST_SEARCH_CMD))
            {
               titleId = IDS_SEARCH_TITLE; //CR96115: Replace IDC_SEARCH_ANY with IDS_SEARCH_TITLE
            }
            ITEXTCTL_SetTitle(pme->m_pText, BREWAPPMGRLS_RES_LOC, titleId, NULL);

			if (wParam == AVK_CLR)
			{
				MShop_SetTitle(pme, BREWAPPMGRLS_RES_LOC, IDC_INPUT_MODE, NULL); //CR96116: Set the correct MShop title.
			}

			ITEXTCTL_Redraw(pme->m_pText);
         }
      }

#ifdef FEATURE_HAPTIC_SUPPORT
      if (EVT_POINTER_DOWN == eCode)
      {
         PlyHapticResponse(pme->m_pHapticResponse,pme->m_ToneData);
      }
#endif //FEATURE_HAPTIC_SUPPORT

      return(TRUE);
   }

   // Handle static control events, only if we are not in progress.  If we are In progress,
   // we don't want the static control to handle any events and then re-draw itself over the
   // in-progress graphic.
   if(pme->m_pStatic && (ST_IN_PROGRESS != pme->m_wState))
   {
      if (!pme->m_bDispTouchEnabled)
      {
         if (ISTATIC_HandleEvent(pme->m_pStatic, eCode, wParam, dwParam))
         {
            return(TRUE);
         }
      }
   }

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

                  // if the static overlaps the Menu, or we are "In Progress" don't
                  // give m_pMenu a chance
                  if ((sRect.y < mRect.y + mRect.dy) || (ST_IN_PROGRESS == pme->m_wState))
                  {
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
                     if (IsInMenuCtl(wXp, wYp, pme->m_pMenu) && (ST_IN_PROGRESS != pme->m_wState)) 
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
               if (IsInMenuCtl(wXp, wYp, pme->m_pMenu) && (ST_IN_PROGRESS != pme->m_wState))
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
         if (IsInMenuCtl(wXp, wYp, pme->m_pMenu) && (ST_IN_PROGRESS != pme->m_wState))
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
      }
      break;
#endif // FEATURE_TOUCHSCREEN_SUPPORT
      // Start MobileShop only if requested by App Manager
      case EVT_APP_START:
      {
         AEEAppStart * pStartArgs = (AEEAppStart*)dwParam;
         FARF(MS, ("Got EVT_APP_START"));
         if (pStartArgs && !pStartArgs->pszArgs)
         {
            FARF(MS, ("Invalid Args to Start"));
            pStartArgs->error = EFAILED;
            return FALSE;
         }

         // Check to see if we can run on the current display.  If not,
         // refuse to start.
         if (FALSE == DisplayEnabled(pme->a.m_pIShell,
            pme->m_bHasMultipleDisplays, pme->m_wEnabledDisplays,
            APPMGR_CURRENT_DISPLAY))
         {
            FARF(MS, ("Can't start on this display"));
            pStartArgs->error = EFAILED;
            return FALSE;
         }
         else
         {
            FARF(MS, ("Able to start on this display"));
         }

         FARF(MS, ("Everything came in as expected"));
         return TRUE;
      }

      case EVT_APP_START_BACKGROUND:
      {
         AEEAppStart* st  = (AEEAppStart*)dwParam;
         FARF(MS, ("Got EVT_APP_START_BACKGROUND"));
         st->error = AEE_SUCCESS;
         return TRUE;
      }

      // Fired by browser
      case EVT_APP_POST_URL:
      case EVT_APP_BROWSE_URL:
         pme->m_evtStart = eCode;
         MShop_Browse(pme,(const char *)dwParam);
         return(TRUE);
         
      // Stop MobileShop - Start any app we want to start when we close
      case EVT_APP_STOP:
         FARF(MS, ("Got EVT_APP_STOP"));
         pme->m_evtStart = 0;
         if(pme->m_clsOpenAtExit)
         {
            ISHELL_StartApplet(pme->a.m_pIShell, pme->m_clsOpenAtExit);
         }
         else if(NULL != pme->m_pszUrlPostAtExit)
         {
            FARF(MS, ("Calling PostUrl(%s) on exit", pme->m_pszUrlPostAtExit));
            ISHELL_PostURL(pme->a.m_pIShell, pme->m_pszUrlPostAtExit);
         }
         return TRUE;

      // If we get a suspend event, just exit MobileShop so we don't
      // get re-started by BREW becase we did not handle suspend.
      case EVT_APP_SUSPEND:
         FARF(MS, ("Got Evt App Suspend"));
         //MShop_ForceExit(pme);
         return MShop_ProcessSuspend(pme);

      case EVT_APP_RESUME:
         {
            IAnnunciator    *pIAnn=NULL;
            
            if (ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ANNUNCIATOR, 
                    (void **) &pIAnn) == SUCCESS)
            {
                IANNUNCIATOR_Redraw(pIAnn);
                IANNUNCIATOR_Release(pIAnn);
            }
            FARF(MS, ("Got EVT_APP_RESUME"));
         }
         return MShop_ProcessResume(pme);

      // A dialog ended.  Show the last screen
      case EVT_DIALOG_END:
         bRet = MShop_SetState(pme, PREV_ENTRY);
         break;

      // Text Control Menu Open
      case EVT_CTL_MENU_OPEN:
         return MShop_SetTextctlMenuStyle(pme, dwParam);

      // Text Control SoftKey Selection Changed
      case EVT_CTL_SEL_CHANGED:
         if (pme->m_pText && IMENUCTL_IsActive(pme->m_pSK))
         {
            // Get Text Control Softkey
            IMenuCtl * pSK = (IMenuCtl*)dwParam;

            switch (IMENUCTL_GetSel(pSK))
            {
               // Search Menu
               case IDC_SEARCH_ANY:
                  MShop_SetTitle(pme, BREWAPPMGRLS_RES_LOC, IDC_SEARCH_ANY, NULL);
                  break;

               // Credit Back Menu
               case IDC_MSHOP_CREDITBACK:
                  MShop_SetTitle(pme, BREWAPPMGRLS_RES_LOC, IDC_MSHOP_CREDITBACK, NULL);
                  break;

               case IDC_BACK:
                  {
                     uint16 titleId = IDC_MSHOP_CREDITBACK;
                     if ((pme->m_wState == ST_SEARCH) || (pme->m_wState == ST_SEARCH_CMD))
                     {
                        titleId = IDC_SEARCH_ANY;
                     }
                     MShop_SetTitle(pme, BREWAPPMGRLS_RES_LOC, titleId, NULL);
                     break;
                  }

               default:
                  MShop_SetTitle(pme, BREWAPPMGRLS_RES_LOC, IDC_INPUT_MODE, NULL);
				  pme->m_bRepaint = TRUE;// CR96116: To ensure elements of search screen are redrawn correctly after return from menu softkey.
                  break;
            }
            return TRUE;
         }
         break;

#ifdef FEATURE_SCREEN_ROTATE
      case EVT_SCR_ROTATE:
      {
         return MShop_HandleScreenRotate(pme, wParam, dwParam);
      }

#endif // FEATURE_SCREEN_ROTATE

      case EVT_KEYGUARD:
         // We got a keyguard event.  This will happen on a Slide and after a flip.
         FARF(MS, ("Got EVT_KEYGUARD with wParam %d", wParam));

         // Ensure that our flag for exit after download
         // becuase the user flipped/slid is set to false
         // that way if the re-open the flip while it is still
         // downloading, we don't exit when we finished
         // if it needs to be true, it will be set to it later
         pme->m_bExitAfterDownload = FALSE;

         if (TRUE == wParam)
         {
            // The slider/clam was closed and KEYGUARD is on.  Check to make sure that we
            // should continue running.
            if (pme->m_bCancelOnClose)
            {
#ifdef FEATURE_EXIT_AFTER_DOWNLOAD
               // If we are in the process of downloading an APP, don't quite exit.
               // Wait until the download is over then exit
               if ((ST_IN_PROGRESS == pme->m_wState) && (ST_PURCHASE == pme->m_wLastState))
               {
                  FARF(MS, ("Not Exiting MobileShop as we are downloading an app"));
                  pme->m_bExitAfterDownload = TRUE;
                  ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID, BAM_EVT_CONTINUE_DOWNLOAD, 0, 0);
                  return TRUE;
               }
#endif //FEATURE_EXIT_AFTER_DOWNLOAD

               // We are not supose to continue to run.  So return false.  BREW
               // will then kill us and the rest of the BREW stack.
               FARF(MS, ("Exiting BAM on KeyGuard as CancelOnClose is True"));

               // Return that We don't handle the Keyguard event, and let 
               // BREW take care of the rest.
               return FALSE;
            }
         }

#ifdef FEATURE_LESS_THAN_6_KEYS
         // Map keys, if any need to be mapped
         MShop_SetKeyList(pme, NULL, 0);
#endif

         // Send the Keyguard event to BAM so that it can handle it the proper way also
         ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID, EVT_KEYGUARD, wParam, dwParam);
         
         return TRUE;

      case EVT_FLIP:
         {
            SwitchDisplaysResult handled = DISPLAY_SWITCH_FAILED;
            

            FARF(MS, ("Got EVT_FLIP with wParam = %d and CancelOnClose = %d", wParam, pme->m_bCancelOnClose));
            if ((FALSE == wParam) &&(pme->m_bCancelOnClose))
            {
#ifdef FEATURE_EXIT_AFTER_DOWNLOAD
               // If we are in the process of downloading an APP, don't quite exit.
               // Wait until the download is over then exit
               FARF(MS, ("wState = %d and wLastState = %d", pme->m_wState, pme->m_wLastState));
               if ((ST_IN_PROGRESS == pme->m_wState) && (ST_PURCHASE == pme->m_wLastState))
               {
                  FARF(MS, ("Not Exiting MobileShop as we are downloading an app"));
                  MShop_CreateDownloadInfoDisplay(pme);
                  ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID, BAM_EVT_CONTINUE_DOWNLOAD, 0, 0);
                  pme->m_bExitAfterDownload = TRUE;
                  return TRUE;
               }
#endif //FEATURE_EXIT_AFTER_DOWNLOAD

               FARF(MS, ("Exiting MobileShop as CancelOnClose is True"));
               // Return that We don't handle the Keyguard event, and let 
               // BREW take care of the rest.
               return FALSE;
            }

#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
            FARF(MS, ("Trying to switch displays"));

            handled = MShop_SwitchDisplays(pme, (boolean)wParam);
            if (DISPLAY_SWITCH_SUCCEEDED == handled)
            {
               // we handled it so send the flip event to BAM
               // and return true;
               ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID, EVT_FLIP, wParam, dwParam);
               return TRUE;
            }
#ifdef FEATURE_EXIT_AFTER_DOWNLOAD
            else if (DISPLAY_SWITCH_WOULD_EXIT == handled)
            {
               // We are now in the "I'm not dead yet" state
               // We would have exited, but since the user is
               // downloading, we are still running.  Let BAM
               // know that we are in this state and return TRUE
               // so we are not killed
               MShop_CreateDownloadInfoDisplay(pme);
               ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID, BAM_EVT_CONTINUE_DOWNLOAD, 0, 0);
               return TRUE;
            }
#endif //FEATURE_EXIT_AFTER_DOWNLOAD
            else
            {
               // We did not handle the Flip, so return false, and let
               // BREW kills us (and BAM)
               return FALSE;
            }

#else
            // Return that We don't handle the Keyguard event, and let 
            // BREW take care of the rest.
            return FALSE;
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
         }

      case EVT_CTL_TAB:
         {
            boolean handled = FALSE;
            // CR89371.  Ignore CTL_TAB if the display is not
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
               }
               else if (IMENUCTL_IsActive(pme->m_pMenu))
               {
                  if(pme->m_pStatic && (ST_APP_OPTIONS == pme->m_wState))
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
            }

            return handled;
         }
         break;

      case EVT_COMMAND:
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
         // Set to false if the user did not select IDC_ACCEPT from the EULA screen
         // or IDC_YES from the PurchaseConfirm screen.  The reason these two screens
         // don't want it cleared is that they proceed to the Download page.  And when
         // the user flips to a non-touch screen we want the Eula/PurchaseConfirm screen
         // in the background.  This is the same behavior if they had been on one of those
         // two screends on a non-touch display and selected ACCEPT/OK.
         if (!((IDC_ACCEPT == wParam) || ((IDC_YES == wParam) && (ST_PURCHASE_CONFIRM == pme->m_wState))))
         {
            pme->m_bPromptIsDisplayed = FALSE;
         }
#endif // FEATURE_SUPPORT_MULTIPLE_DISPLAYS

         // If we have a static text, turn off autoscroll
         // so that if we get a connecting dialog, or something else that just
         // goes on top of it, then the static does not scroll and 
         // partially overwrite what is on top of the static

         if (pme->m_pStatic)
         {
            uint32 props;

            props = ISTATIC_GetProperties(pme->m_pStatic);
            props |=ST_NOSCROLL;
            ISTATIC_SetProperties(pme->m_pStatic, props);
         }


         if(pme->m_bAIPrompt){
            pme->m_bAIPrompt = FALSE;     // Reset Auto-Install flag
            MShop_InProgress(pme);       // Display in progress graphics
            IDOWNLOAD_Continue(pme->m_pDownload,(boolean)(wParam == IDC_YES));
            return(TRUE);
         }

         switch(wParam)
         {

            case IDC_OK:
               if (pme->m_bCarrierMessage)
               {  // Dismiss Carrier Message
                  pme->m_bCarrierMessage = FALSE;
                  bRet = MShop_SetState(pme, pme->m_wState);
               }
               else
               {                  
                  if (pme->m_wState == ST_PURCHASE)
                  {
                     MShop_ForceExit(pme);
                     return TRUE;
                  }
                  else
                     bRet = MShop_SetState(pme, PREV_ENTRY);
               }
               break;

            // Auto Disable and Proceed with current state
            case IDC_PROCEED:
            {
               IShell* pIShell = pme->a.m_pIShell;
               AEEAppInfo ai;
               uint32 existingItem = 0;
               uint32 high24Bits = 0xFFFFFF00;

               // CR 80447.  Lock BAM/MobileShop module during autodisable
               // list creation to prevent IDownload from including it in
               // the list.
               uint32 modID = ISHELL_GetClassItemID(pme->a.m_pIShell,
                  AEECLSID_BREWAPPMGR_BID);
               if (0 != modID)
               {
                  IDOWNLOAD_Lock(pme->m_pDownload, modID, TRUE);
               }

               // CR 38899.  Search the list of applets to see if this
               // is an upgrade.  If it is we want to lock the existing
               // version to keep it out of the disable list.
               ISHELL_EnumAppletInit(pIShell);
               while (ISHELL_EnumNextApplet(pIShell, &ai))
               {
                  AEECLSID cls = ai.cls;
                  existingItem = ISHELL_GetClassItemID(pIShell, cls);
                  // Items with identical high 24 bits are different versions
                  // of the same app
                  if ((existingItem & high24Bits) == (pme->m_iApp & high24Bits))
                  {
                     // Lock existing item
                     IDOWNLOAD_Lock(pme->m_pDownload, existingItem, TRUE);
                     break;
                  }
                  else
                  {
                     existingItem = 0;
                  }
               }
#ifdef USE_IDL2_4GB_SUPPORT
               IDOWNLOAD2_AutoDisable(pme->m_pDownload2, pme->m_iApp);
#else
               IDOWNLOAD_AutoDisable(pme->m_pDownload, pme->m_iApp);
#endif

               // CR 80447.  Unlock BAM/MobileShop module
               if (0 != modID)
               {
                  IDOWNLOAD_Lock(pme->m_pDownload, modID, FALSE);
               }
               // CR 38899.  Unlock existing item if it was locked.
               if (0 != existingItem)
               {
                  IDOWNLOAD_Lock(pme->m_pDownload, existingItem, FALSE);
               }

               // In case our orig projections as to what apps need to be
               // disabled to allow things to download were off
               // stay in this state.  This also helps because
               // Restore and CheckSpace both come in here right this second.
               bRet = MShop_SetState(pme, pme->m_wState);
            }
            break;

#ifdef SEARCH_FROM_CATALOG
            case IDC_MSHOP_FIND:
               bRet = MShop_SetState(pme, ST_SEARCH);
               break;
#endif // SEARCH_FROM_CATALOG

            // Perform Search
            case IDC_SEARCH_ANY:
#if defined(MSHOP_CMD_SEARCH_STRING) || defined(MSHOP_CMD_CATEGORY)
               *(pme->m_pszSearch) = 0;      // Empty search string
               *(pme->m_pszTitle) = 0;       // Empty title string
#endif
               pme->m_bSearchAny = TRUE;
               MShop_StartSearch(pme);
               return TRUE;

               // Credit Back
            case IDC_MSHOP_CREDITBACK:
               MShop_StartCreditBack(pme);
               return TRUE;

            // Handling of close is state dependent
            case IDC_CLOSE:
               // Reset Information display flag
               pme->m_bDisplayInfo = FALSE;

#ifdef MSHOP_CMD_ITEMINFO
               if (pme->m_bDisplayInfoOnly)
               {
                  MShop_ForceExit(pme);
                  pme->m_bDisplayInfoOnly = FALSE;
                  return TRUE;
               }
               else
#endif
               {
                  if (pme->m_bExitOnCancel)
                  {
                     MShop_ForceExit(pme);
                     return TRUE;
                  }
                  // displaying app options
                  bRet = MShop_SetState(pme, pme->m_wState);
               }
               break;

            case IDC_NO:
               if ((pme->m_wState == ST_PURCHASE) || pme->m_bExitOnCancel)
               {
                  MShop_ForceExit(pme);
                  return TRUE;
               }
               break;

            // Cancel
            case IDC_CANCEL:
               if (pme->m_bExitOnCancel)
               {
                  FARF(MS, ("ExitOnCancel is set, so exiting"));
                  MShop_ForceExit(pme);
                  return TRUE;
               }
               if (pme->m_bDispTouchEnabled)
               {
                  // Cancel Download
                  IDOWNLOAD_Cancel(pme->m_pDownload);

                  if(pme->m_wState == ST_IN_PROGRESS)
                  {
                     pme->m_wState = pme->m_wLastState;
                     FARF(MS, ("State is now %d", pme->m_wState));
                     if ((ST_PURCHASE == pme->m_wState) && (100 == pme->m_wPct)) {
                        FARF(MS, ("We are done with the downloading and the user hit cancel, so we exit"));
                        MShop_ForceExit(pme);
                        return TRUE;
                     }
                     else 
                     {
                        FARF(MS, ("Setting State to PREV_ENTRY"));
                        return MShop_SetState(pme, PREV_ENTRY);
                     }
                  }
                  else                   
                  {
                     FARF(MS, ("Setting State to PREV_ENTRY"));
                     return MShop_SetState(pme, PREV_ENTRY);
                  }
               }
               FARF(MS, ("Setting State to PREV_ENTRY"));
               bRet = MShop_SetState(pme,PREV_ENTRY);
               break;

            // Accept on purchase confirmation dialog
            case IDC_ACCEPT:        
               bRet = MShop_SetState(pme, ST_PURCHASE);
               break;

            // Processing of "Yes" is dependent on current state.
            case IDC_YES:
               if (ST_PURCHASE == pme->m_wState)
               {
                  return MShop_RunApp(pme);
               }
               else if (ST_PURCHASE_CONFIRM == pme->m_wState)
               {
                  bRet = MShop_SetState(pme, ST_PURCHASE_EULA);
               }
               break;

            // Process Open/Info of app/category list
            case IDC_INFO:
               return MShop_ProcessMenuSel(pme, wParam);

            case IDC_BACK:
              MShop_HandleEvent(pme, EVT_KEY, AVK_CLR, 0);
              break;

            default:
               return MShop_ProcessDefaultCmd(pme, wParam, dwParam);
         } // Closes wParam Switch
         break;
        
      // Key Events
      case EVT_KEY:
         return MShop_ProcessKeyEvent(pme, wParam, dwParam); 
   }

   return bRet;
  }

/*===========================================================================

// Processes Info

===========================================================================*/
static boolean MShop_ProcessMenuSel(MShop* pme, uint16 wParam)
{
   uint32 dwItem;

   IMENUCTL_GetItemData(pme->m_pMenu, wParam, &dwItem);

   MShop_ShowItemInfo(pme,(DLEnumItem *)dwItem);

   return TRUE;
}

/*===========================================================================

// Default Command Handler

===========================================================================*/
static boolean MShop_ProcessDefaultCmd(MShop* pme, uint16 wParam, uint32 dwParam)
{
   uint32   dwItem;

   IMENUCTL_GetItemData(pme->m_pMenu, wParam, &dwItem);

   // Server list in "Set Server" menu
   if(wParam >= IDC_SERVER){             
      if ((wParam-20)>IDC_SERVER)
      {// 测试时发现软件存在一个问题：在访问高通测试服务器时，由 Catalog 进入软件
       // 目录列表，选 Get Going -> AUTODISABLEOTA -> 闪过 软件信息界面后即返回。
       // 根据 TRACE 发现收到一个暂不能确定来源的命令事件，wParam = 0xFFFF, 导致异
       // 常。这里如此修改以暂时避开。(BAM2.0.5同样存在此问题)。
         return TRUE;
      }
      MShop_SetServer(pme, wParam);
      return TRUE;
   }

   // App price list in app options menu
   if(wParam >= IDC_PRICE){               
      if (NULL != pme->m_pPrice)
      {
         FreePrice(&pme->m_pPrice);
      }
      pme->m_pPrice = CopyPrice((Price *)dwItem);
      pme->m_bDemo = (boolean)(wParam == IDC_DEMO);
      pme->m_bSubscription = (boolean)(wParam == IDC_SUBSCRIPTION);
      pme->m_iApp = pme->m_pItem->iID;
      return MShop_SetState(pme, ST_CHECK_SPACE);
   }

   // Items in category/app list 
   if(wParam >= IDC_ITEM && ST_IN_PROGRESS != pme->m_wState)
   {
      MShop_CatDown(pme,(DLEnumItem *)dwItem);
      return TRUE;
   }

   return(FALSE);
}

/*===========================================================================

// Processes Key Events

===========================================================================*/
static boolean MShop_ProcessKeyEvent(MShop* pme, uint16 wParam, uint32 dwParam)
{
   switch(wParam)
   {
      // Clear Key
      case AVK_CLR:
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
         // Set to false always.  If the prompt was not being displayed
         // it should have already been false.  If it was was being displayed
         // it is now about to exit.
         pme->m_bPromptIsDisplayed = FALSE;
#endif // FEATURE_SUPPORT_MULTIPLE_DISPLAYS

         // If we have a static text, turn off autoscroll
         // so that if we get a connecting dialog, or something else that just
         // goes on top of it, then the static does not scroll and 
         // partially overwrite what is on top of the static

         if (pme->m_pStatic)
         {
            uint32 props;

            props = ISTATIC_GetProperties(pme->m_pStatic);
            props |=ST_NOSCROLL;
            ISTATIC_SetProperties(pme->m_pStatic, props);
         }

         // Clear During Information Display
         if (pme->m_bDisplayInfo)
            return ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MOBILESHOP_BID, EVT_COMMAND, IDC_CLOSE, 0);

         // Cancel Download
         IDOWNLOAD_Cancel(pme->m_pDownload);

         switch(pme->m_wState){
            case ST_CATEGORIES:
               MShop_CatUp(pme);
               return TRUE;

            case ST_IN_PROGRESS:
               FARF(MS, ("State going from %d -> %d", pme->m_wState, pme->m_wLastState));
               pme->m_wState = pme->m_wLastState;               
               if ((ST_PURCHASE == pme->m_wState) && (100 == pme->m_wPct)) {
                  MShop_ForceExit(pme);
                  return TRUE;
               }
               else {
                  return MShop_SetState(pme, PREV_ENTRY);
               }

            case ST_APP_OPTIONS:
               if (pme->m_nCatIdx < 0)
               {
                  MShop_ForceExit(pme);
                  return TRUE;
               }
               else
                  return MShop_SetState(pme, PREV_ENTRY);

            case ST_PURCHASE:
               MShop_ForceExit(pme);
               return TRUE;

            default:
               return MShop_SetState(pme, PREV_ENTRY);
         }
         break;

      case AVK_UP:
      case AVK_DOWN:
      case AVK_LEFT:
      case AVK_RIGHT:
         if ((pme->m_wState == ST_SEARCH) || 
             (pme->m_wState == ST_SEARCH_CMD) || 
             (pme->m_wState == ST_CREDITBACK))
         {
            if (IMENUCTL_IsActive(pme->m_pSK))
            {
                if (IMENUCTL_HandleEvent(pme->m_pSK, EVT_KEY, wParam, dwParam))
                {
                    return TRUE;
                }
                if (pme->m_pText != NULL)
                {
                    ITEXTCTL_SetActive(pme->m_pText, TRUE);
                    IMENUCTL_SetActive(pme->m_pSK, FALSE);
                }
            }
            else
            {
                IMENUCTL_SetActive(pme->m_pSK, TRUE);
                if (pme->m_pText != NULL)
                {
                    ITEXTCTL_SetActive(pme->m_pText, FALSE);
                }
            }
            
            return TRUE;
         }
        
         // First chance to softkey
         if (IMENUCTL_IsActive(pme->m_pSK))
            return IMENUCTL_HandleEvent(pme->m_pSK, EVT_KEY, wParam, dwParam);

         if (IMENUCTL_IsActive(pme->m_pMenu))
            return IMENUCTL_HandleEvent(pme->m_pMenu, EVT_KEY, wParam, dwParam);
         
         // CR89783 & CR89841.  When state is IN_PROGRESS we don't want
         // to activate m_pMenu nor have m_pStatic handle the event.
         if(pme->m_wState == ST_IN_PROGRESS )
         {
            if (IMENUCTL_GetItemCount(pme->m_pSK) > 0 && pme->m_bDispTouchEnabled)
            {
               IMENUCTL_SetActive(pme->m_pSK, TRUE);
               return TRUE;
            }
         }
         else if (pme->m_pStatic && (ST_APP_OPTIONS != pme->m_wState))
         {
            // no menu is active, if the prompt is up activate the menu
            // that has keys
            boolean handled =
               ISTATIC_HandleEvent(pme->m_pStatic, EVT_KEY, wParam, dwParam);
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


         // CR95964 if the display is not
         // touch-enabled don't try to set a control active.
         if (TRUE == pme->m_bDispTouchEnabled)
         {
            if ((AVK_UP == wParam) || (AVK_DOWN == wParam))
            {
               //if nothimg is active, set it io the Menu
               // if it has any  items in it
               if (IMENUCTL_GetItemCount(pme->m_pMenu) > 0)
				   {
					   IMENUCTL_SetActive(pme->m_pMenu, TRUE);
                  return TRUE;
				   }
            }
            else
            {
               //if nothimg is active, set it io the softkey
               // if it has any  items in it
               if (IMENUCTL_GetItemCount(pme->m_pSK) > 0)
				   {
					   IMENUCTL_SetActive(pme->m_pSK, TRUE);
                  return TRUE;
				   }
            }
         }
         break;

      default:
         if (((wParam == AVK_SELECT) || (wParam == AVK_INFO)) &&
             ((pme->m_wState == ST_SEARCH) || (pme->m_wState == ST_SEARCH_CMD) || (pme->m_wState == ST_CREDITBACK)))
         {
            if (pme->m_pText != NULL)
            {
                ITEXTCTL_SetActive(pme->m_pText, FALSE);
            }
            IMENUCTL_SetActive(pme->m_pSK, TRUE);
            return TRUE;
         }
         // Handle softkey events for a non text control softkey
         if(IMENUCTL_HandleEvent(pme->m_pSK, EVT_KEY, wParam, dwParam))
            return(TRUE);

         // Handle standard menu events
         if(IMENUCTL_HandleEvent(pme->m_pMenu, EVT_KEY, wParam, dwParam))
            return(TRUE);
   }
   return FALSE;
}


//===========================================================================
//
// Processes the EVT_APP_SUSPEND event
//
//===========================================================================
static boolean MShop_ProcessSuspend(MShop* pme)
{
   boolean bFlipOpen = TRUE;
   int nSize = sizeof(boolean);

   pme->m_bSuspending = TRUE;
   // Reset Controls
   MShop_Reset(pme, FALSE);

#ifdef FEATURE_DISABLE_SS_DURING_DOWNLOAD
   // If we have a screen saver class saved, re-turn on the screen saver
   if (0 != pme->m_clsScreenSaver)
   {
      ISHELL_RegisterHandler(pme->a.m_pIShell, AEECLSID_SCREEN_SAVER, MT_SCREEN_SAVER, pme->m_clsScreenSaver);
   }
   pme->m_clsScreenSaver = 0; // clear our memory, so we don't try to re-set it again
#endif // FEATURE_DISABLE_SS_DURING_DOWNLOAD

   // Get the state of the flip, if there is one
   if (SUCCESS == ISHELL_GetDeviceInfoEx(pme->a.m_pIShell, AEE_DEVICESTATE_FLIP_OPEN,
                                         &bFlipOpen, &nSize))
   {
      pme->m_bFlipOpenWhenSuspended = bFlipOpen;
   }

#ifdef USE_TT_FONTS
   // Restore the original fonts before suspending.
   MShop_RestoreFonts(pme);
#endif // USE_TT_FONTS

         // Free IDownload
   FreeObj((void **)(&pme->m_pDownload));
#ifdef USE_IDL2
   FreeObj((void **)(&pme->m_pDownload2));
#endif


   pme->m_bSuspending = FALSE;

   return TRUE;
}


//===========================================================================
//
// Processes the EVT_APP_RESUME event
//
//===========================================================================
static boolean MShop_ProcessResume(MShop* pme)
{
   boolean retVal = FALSE;
   boolean bFlipOpen = TRUE;
   int nSize = sizeof(boolean);

   pme->m_bResuming = TRUE;

    // Create IDownload Instance
#ifdef USE_IDL2
   if (CreateInstance(pme->a.m_pIShell, AEECLSID_DOWNLOAD2, (void**)&pme->m_pDownload2) != AEE_SUCCESS)
   {
      pme->m_bResuming = FALSE;
      return FALSE;
   }

   if (IDOWNLOAD2_QueryInterface(pme->m_pDownload2, AEECLSID_DOWNLOAD, (void**)&pme->m_pDownload) != AEE_SUCCESS)
   {
      pme->m_bResuming = FALSE;
      return FALSE;
   }
#else
   if (CreateInstance(pme->a.m_pIShell, AEECLSID_DOWNLOAD, (void**)&pme->m_pDownload) != AEE_SUCCESS)
   {
      pme->m_bResuming = FALSE;
      return FALSE;
   }
#endif


   // Since we are resuming, we don't want to display the Carrier Message
   pme->m_bCarrierMessage = FALSE;
   IDOWNLOAD_OnStatus(pme->m_pDownload, (PFNDLSTATUS)(MShop_Status),pme);


   // Get the state of the flip, if there is one
   if (SUCCESS == ISHELL_GetDeviceInfoEx(pme->a.m_pIShell, AEE_DEVICESTATE_FLIP_OPEN,
                                         &bFlipOpen, &nSize))
   {
      if (pme->m_bFlipOpenWhenSuspended != bFlipOpen)
      {
         // Handle the case of where the user closed the flip while we were suspended
         // Leave m_bResuming as true, as we are still resuming
         // we will set it to false when we finish "switching" displays
         SwitchDisplaysResult handled =  MShop_SwitchDisplays(pme, TRUE);
         if (DISPLAY_SWITCH_SUCCEEDED == handled)
         {
            retVal = TRUE;
         }
         else
         {
            retVal = FALSE;
            pme->m_bResuming = FALSE;
         }

         return retVal;
      }
   }

   // They may have flipped close and then open on the display
   // so our display my be active, but not enabled yet
   // I don't think that we need to call MakeDefault, here but 
   // I am doing it just to make sure that it does happen, and 
   // we get the call back.
   if (!IDISPLAY_IsEnabled(pme->m_pIDisplay))
   {
      pme->cbDisplayEnabled = (AEECallback*) MALLOC(sizeof(AEECallback));
      MEMSET(pme->cbDisplayEnabled, 0, sizeof(AEECallback));
      CALLBACK_Init(pme->cbDisplayEnabled, MShop_EnableDispBeforeSetStateCallback, pme);
      IDISPLAY_NotifyEnable(pme->m_pIDisplay, pme->cbDisplayEnabled);
      return TRUE;
   }


#ifdef USE_TT_FONTS
   // Install TrueType fonts when resuming.
   MShop_InstallTTFonts(pme);
#endif // USE_TT_FONTS

   retVal = MShop_SetStateAfterResume(pme);

   

   pme->m_bResuming = FALSE;
   
   return retVal;
}

//===========================================================================
//
// Gets the state back to the state before the suspend/resume happened
// If We need to call IDL_GetItemInfo() before we re-setup the state
// do so, then the state stuff will finalize in MShop_ItemInfoResumeCB
//
//===========================================================================
static boolean MShop_SetStateAfterResume(MShop* pme)
{
   boolean bRetVal = FALSE;

   switch (pme->m_wState)
   {
   case ST_PURCHASE:
      if (pme->m_bPromptIsDisplayed)
      {
         // The download has finished, so re-display the prompt
         AECHAR* title = (NULL != pme->m_pPromptTitle)? WSTRDUP(pme->m_pPromptTitle) : NULL;
         AECHAR* text = (NULL != pme->m_pPromptText)? WSTRDUP(pme->m_pPromptText) : NULL;
         uint16* wButtons = (NULL != pme->m_pPromptButtons)? CopyUInt16Array(pme->m_pPromptButtons) : NULL;
         uint16* wItemIDs = (NULL != pme->m_pPromptItemIDs)? CopyUInt16Array(pme->m_pPromptItemIDs) : NULL;

         // The menu needs to be-recreated so turn off bRedrawingUI
         // and send in value that can be saved, thus deleting the stored
         // values.
         pme->m_bRedrawingUI = FALSE;
         MShop_Prompt(pme, title, pme->m_wPromptTitleID, text, pme->m_wPromptTextID, 
                      wButtons, wItemIDs, pme->m_wPromptDefID, pme->m_dwPromptProps);

         FreePtr((void**)&title);
         FreePtr((void**)&text);
         FreePtr((void**)&wButtons);
         FreePtr((void**)&wItemIDs);
         pme->m_bResuming = FALSE;
         return TRUE;
      }
      // fall through
   case ST_APP_OPTIONS:
   case ST_CHECK_SPACE:
   case ST_PURCHASE_CONFIRM:
   case ST_PURCHASE_EULA:
      // We need to make sure that we call GetItemInfo() so subsequent
      // calls succeed.  As Acquire(), GetSizeEx(), GetAutoDisableList(), and AutoDisable()
      // rely on cached data inside IDL that we need to re-create.
      IDOWNLOAD_GetItemInfo(pme->m_pDownload, pme->m_pItem->iID, MShop_ItemInfoResumeCB, pme);
      return TRUE;

   case ST_IN_PROGRESS:
      // This needs to be figured out
      if (ST_CATEGORIES == pme->m_wLastState)
      {
         FARF(MS, ("RESUMING WHEN THE STATE IS ST_IN_PROGRESS"));
         bRetVal = MShop_SetState(pme, ST_CATEGORIES);
         pme->m_bResuming = FALSE;
         return bRetVal;

      }
      // We need to make sure that we call GetItemInfo() so subsequent
      // calls succeed.  As Acquire(), GetSizeEx(), GetAutoDisableList(), and AutoDisable()
      // rely on cached data inside IDL that we need to re-create.
      IDOWNLOAD_GetItemInfo(pme->m_pDownload, pme->m_pItem->iID, MShop_ItemInfoResumeCB, pme);
      return TRUE;

   default:
      // Resume from the state we are in
      // just re-draw and re-create everything
      bRetVal = MShop_SetState(pme, pme->m_wState);
      pme->m_bResuming = FALSE;
      return bRetVal;
   }
}

/*===========================================================================

// Sets TextCtl Menu Style

===========================================================================*/
static boolean MShop_SetTextctlMenuStyle(MShop* pme, uint32 dwParam)
{
   if (pme->m_wState == ST_SEARCH || pme->m_wState == ST_SEARCH_CMD || pme->m_wState == ST_CREDITBACK)
   {
      AEERect rc;

      IMenuCtl* pMenu = (IMenuCtl*)dwParam;

      // Set Style & Color for menu
      SetMenuStyleAndColor(pMenu, AEECLSID_MENUCTL, pme->m_nColorDepth, pme->m_bDispTouchEnabled);

      // Erase Title
      IMENUCTL_SetTitle(pMenu, NULL, NULL, 0);
      // Store the title info for use with flipping the cover
      MShop_StoreMenuTitleInfo(pme, NULL, NULL, 0);

      // Specify Rect
      SETAEERECT(&rc, 0, 0, pme->m_cx, pme->m_cy);
      IMENUCTL_SetRect(pMenu, &rc);

      // Set repaint flag TRUE so that TextControl can repaint its title and banner when it
      // comes back from its internal menus
      pme->m_bRepaint = TRUE;

      return TRUE;
   }
   return FALSE;
}

/*===========================================================================

Sets the state machines state according to the specified value

===========================================================================*/
static boolean MShop_SetState(MShop * pme, int nState)
{
   MShopState  * ps;

   if(nState == PREV_ENTRY){
      // Coming here means state is to be set to the back state

      // CR 88507.  If the current state is NO_ENTRY, as when MobileShop
      // is responding to a remove request (URL) from AppMgr, returning
      // to the previous state ( in response to CLR key, or Cancel)
      // doesn't make sense.  Just exit.
      if (NO_ENTRY == pme->m_wState)
      {
         FARF(MS, ("Exiting MobileShop because our state is NO_ENTRY"));
         MShop_ForceExit(pme);
         return(TRUE);
      }

      // If current state is ST_CATEGORIES and category index is
      // non zero, move up in category list
      if(pme->m_wState == ST_CATEGORIES && pme->m_nCatIdx >= 0){
         MShop_CatUp(pme);
         return(TRUE);
      }

      // Get back state of the current state
      nState = (int)pme->m_states[pme->m_wState].nBack;

      // If back state of the current state is NO_ENTRY, close mobile shop with 
      // bReturnToIdle == FALSE so that any suspended applet can get active.
      if(nState == NO_ENTRY)
      {
         FARF(MS, ("Exiting MobileShop because our back state is NO_ENTRY"));
         MShop_ForceExit(pme);
         return(TRUE);
      }


      // Set back state as current state.
      FARF(MS, ("State going from %d -> %d", pme->m_wState, nState));
      pme->m_wState = (int16)nState;
   }
   else
   {

      // If we are trying to go to the NO_ENTRY state, just exit.
      if(nState == NO_ENTRY)
      {
         FARF(MS, ("Exiting MobileShop because our new state is NO_ENTRY"));
         MShop_ForceExit(pme);
         return(TRUE);
      }

      FARF(MS, ("State going from %d -> %d", pme->m_wState, nState));
      pme->m_wState = (int16)nState; // Set specified state
   }

   ps = (MShopState *)&pme->m_states[pme->m_wState];

   if(ps->bClear)
      MShop_Reset(pme,FALSE); // Reset display

   // Call state handler
   return(ps->pfnReq(pme));
}

/*===========================================================================

FAILED Response Dialog (General)

===========================================================================*/
static void MShop_Failed(MShop * pme, uint16 wIDDef, int nErr, const AECHAR * pszMsg)
{
   uint16         wButtons[2];
   AECHAR         sz[10];
   uint32         dwProps = 0;
   boolean        bFreePszMsg = FALSE;
   
   MEMSET(wButtons, 0, sizeof(wButtons));

   // If no message, determine error message from resource file
   if(!pszMsg){
      switch(nErr){

         case WEB_ERROR_CONNECT:
            wIDDef = IDS_PPP_ERROR;
            break;

         case EPRIVLEVEL:
            wIDDef = IDS_AUTH_FAILED;
            break;

         case EBADSID:
            wIDDef = (wIDDef == IDS_RESTORE_FAILED ? IDS_LICENSE_NOT_FOUND : IDS_NEED_ORIGINAL_CARD);
            break;

         default:
         {
            AECHAR   szNumFormat[10];

            STRTOWSTR(" (%d)",szNumFormat,sizeof(szNumFormat));
            WSPRINTF(sz,sizeof(sz),szNumFormat,nErr);

            pszMsg = (const AECHAR *)MALLOC(MAX_STR_SIZE);
            if(pszMsg){
               ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, wIDDef,(AECHAR *)pszMsg,MAX_STR_SIZE);
               if(WSTRSIZE(sz) + WSTRSIZE(pszMsg) < MAX_STR_SIZE)
               {
                  WSTRCAT((AECHAR *)pszMsg,sz);
               }

               bFreePszMsg = TRUE;
            }
            else
            {
               pszMsg = (const AECHAR *)sz;
            }
         }
      }
   }  

   wButtons[0] = IDC_OK;
   // Display single button prompt
   MShop_Prompt(pme, NULL, IDC_MSHOP, pszMsg, wIDDef, wButtons, wButtons, IDC_OK, dwProps|ST_NOSCROLL);

   if (bFreePszMsg)
   {
      FreePtr((void**)&pszMsg);
   }
}

/*===========================================================================

RECALLED Response Dialog (General)

Called in response to DEVT_ADS_RECALL (app recall) event received from ADS.

===========================================================================*/
static void MShop_RecallMessage(MShop * pme, AECHAR * pszRecalled)
{
   uint16         wButtons[2];
   AECHAR*        pText = NULL;

   // Reset UI setting last state as current state
   MShop_Reset(pme,TRUE);

   // Display recall message in single button ("OK") prompt
   MEMSET(wButtons, 0, sizeof(wButtons));

   pText = FormatString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, IDS_RECALLED, pszRecalled);
   wButtons[0] = IDC_OK;
   MShop_Prompt(pme, NULL, IDC_MSHOP, pText, 0, wButtons, wButtons, IDC_OK, ST_NOSCROLL);

   FreePtr((void**)&pText);

   // Cancel any downloading
   IDOWNLOAD_Cancel(pme->m_pDownload);
}

/*===========================================================================

DEVT_DL_STATUS Response Dialog (General)

Called in response to DEVT_DL_STATUS (verification failed) event received from ADS.

===========================================================================*/
static void MShop_VerificationFailedMessage(MShop * pme, AECHAR * pszAppName)
{
   uint16         wButtons[2];
   AECHAR*        pText = NULL;

   // Reset Application ID
   pme->m_iApp = 0;

   // Reset UI setting last state as current state
   MShop_Reset(pme,TRUE);

   // Display verification failed in single button ("OK") prompt
   MEMSET(wButtons, 0, sizeof(wButtons));

   pText = FormatString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, IDS_VERIFICATION_FAILED, pszAppName);
   wButtons[0] = IDC_OK;
   MShop_Prompt(pme, NULL, IDC_MSHOP, pText, 0, wButtons, wButtons, IDC_OK, ST_NOSCROLL);

   FreePtr((void**)&pText);

   // Cancel any downloading
   IDOWNLOAD_Cancel(pme->m_pDownload);
}

/*===========================================================================

Carrier Message Dialog (General)

Called in response to DEVT_ADS_MSG received from ADS

===========================================================================*/
static void MShop_CarrierMessage(MShop * pme, AECHAR * pszText)
{
   uint16         wButtons[2];
   AECHAR *       pszTitle;

   // Reset UI setting previous state as current state
   MShop_Reset(pme,TRUE);

   // Display carrier message with "OK" button to dismiss that.
   // Handle following types of messages:
   // <title>\n<text>, <title>|<text> or <text>
   MEMSET(wButtons, 0, sizeof(wButtons));

   pszTitle = pszText;

   pszText = WSTRCHR(pszTitle,(AECHAR)'\n');
   if(!pszText || !pszText[1])
      pszText = WSTRCHR(pszTitle,(AECHAR)'|');

   if(pszText)
      *(pszText++) = 0;
   else{
      pszText = pszTitle;
      pszTitle = NULL;
   }

   wButtons[0] = IDC_OK;

   MShop_Prompt(pme, pszTitle, IDC_MSHOP, pszText, 0, wButtons, wButtons, IDC_OK, ST_NOSCROLL);

   // Do this here as the text may be freed in the Cancel call
   IDOWNLOAD_Cancel(pme->m_pDownload);

}

/*===========================================================================

CATEGORY DIALOG LOGIC

===========================================================================*/

/*===========================================================================

 New Style (post-1.01) Nested Category Logic

===========================================================================*/

/*===========================================================================

 Resets category list. Frees category name buffers.

===========================================================================*/
static void MShop_EnumCleanup(MShop * pme)
{
   MShopCat *  pc;
   int         i;

   // Reset category index 
   pme->m_nCatIdx = -1;

   // Free category name buffers
   for(i = 0, pc = pme->m_catList; i < MSHOP_MAX_DEPTH; i++, pc++)
      FreePtr((void **)&pc->pszName);

   // Reset category list
   MEMSET(pme->m_catList, 0, sizeof(pme->m_catList));
}

/*===========================================================================

 Callback from the enumeration engine on catalog browsing for forcing application
 validation. Error means failure to connect web for verification. Success means
 successful enumeration. This means successful acking if DEVT_DL_STATUS isn't
 received. Otherwise, this means application got removed during DEVT_DL_STATUS.

===========================================================================*/
static void MShop_VerifyCB(void * pcxt, int nErr, DLEnumItem * pe)
{
   MShop * pme = (MShop *)pcxt;

   // Reset UI
   MShop_Reset(pme,TRUE);


   // Cancel Download Callback as we don't need any more info
   IDOWNLOAD_Cancel(pme->m_pDownload);

   if (nErr)
   {
      MShop_Failed(pme,IDS_APP_VERIFY_FAILED,nErr,NULL);
   }
   else
   {
      // If verification resulted in DEVT_DL_STATUS and thus application removal
      // pme->m_iApp is reset on DEVT_DL_STATUS to indicate MShop_VerifyCB of
      // this situation

      if (pme->m_iApp)
      {
         // Run the application
         MShop_RunApp(pme);
      }
   }
}

/*===========================================================================

Re-display the currently enumerated directory of sub-categories and/or items.
State handler for ST_CATEGORIES and ST_APPS.

===========================================================================*/
static boolean MShop_Enum(MShop * pme)
{
   DLITEMID id;

   // Free EULA buffer
   FreePtr((void **)&pme->m_pszEULA);
   pme->m_bGotEULA = FALSE;

   // Display progress graphics
   MShop_InProgress(pme);

   // Get category ID at current level. If this is -1, we used DL_CATEGORY_ROOT.
   if (pme->m_nCatIdx >= 0)
   {
      id = pme->m_catList[pme->m_nCatIdx].iID;
   }
   else
   {
      id = DL_CATEGORY_ROOT;
      pme->m_nCatIdx = 0;
      pme->m_catList[0].iID = id;
   }
   pme->m_nCatItems = 0;         // Reset number of category items at this level
   pme->m_bSearch = FALSE;       // Reset search
   if (pme->m_bUpgradeCheck)
   {
      // Cache the download ID.  If the check comes up empty we'll notify
      // the UpgradeMgr.
      pme->m_iApp = id;

      // Check for item upgrade
      IDOWNLOAD_CheckItemUpgrade(pme->m_pDownload, id, MShop_EnumCB,pme);
   }
   else if (pme->m_bVerify)
      // Enumerate category to force application validation
      IDOWNLOAD_Enum(pme->m_pDownload,id,MShop_VerifyCB,pme);
   else
      // Enumerate category items using category ID
      IDOWNLOAD_Enum(pme->m_pDownload,id,MShop_EnumCB,pme);
   return(TRUE);
}

/*===========================================================================

 Cleanup the category at the current level

===========================================================================*/
static void MShop_ClearCat(MShop * pme)
{
   MShopCat * pc;

   // Free the category name buffer at current level and move index up.
   pc = &pme->m_catList[pme->m_nCatIdx];
   FreePtr((void **)&pc->pszName);
   MEMSET(pc,0,sizeof(MShopCat));
   pme->m_nCatIdx--;
}

/*===========================================================================

 Display the owner/parent category of items and sub-categories

===========================================================================*/
static void MShop_CatUp(MShop * pme)
{
   if(pme->m_nCatIdx <= 0){
      // Coming here means that we are already at the top of category list.
      // Reset category index and set state to NO_ENTRY.
      MEMSET(pme->m_catList, 0, sizeof(pme->m_catList));
      pme->m_nCatIdx = -1;
      MShop_SetState(pme, PREV_ENTRY);
   }
   else{
      // Clear category list at current level and move up
      MShop_ClearCat(pme);
      // Enumerate at new level
      MShop_Enum(pme);
   }
}
   
/*===========================================================================

 Display a sub-level of items

===========================================================================*/
static void MShop_CatDown(MShop * pme,DLEnumItem * pi)
{
   MShopCat *  pc;

   // If pi is not referring to a category, this is referring to an
   // application. Set state to ST_APP_OPTIONS and get out of here.
   if (pi->t != DLI_CATEGORY)
   {
      // Free cached download item, if any
      if (NULL != pme->m_pItem)
      {
         FreeDLItem(pme->m_pItem);
      }
      // Cache a copy of the current download item.  See CR 83921, e.g.
      pme->m_pItem = CopyDLItem((DLItem*)pi);
      if (NULL == pme->m_pItem)
      {
         FARF(MS, ("CopyDLItem() failed"));
         MShop_Reset(pme,TRUE);
         MShop_Failed(pme, IDS_MSHOP_OUT_OF_MEMORY, ENOMEMORY, NULL);
      }

      // IDOWNLOAD_Cancel() that was done here for CR84758 has been
      // removed to work around an issue with IDownload - see CR 85884.

      MShop_SetState(pme, ST_APP_OPTIONS);
      return;
   }

   if (pme->m_nCatIdx < MSHOP_MAX_DEPTH - 1)
   {
      pme->m_nCatIdx++; // Increment Category Index

      pc = &pme->m_catList[pme->m_nCatIdx]; 

      pc->pszName = WSTRDUP(pi->pszName); // Save category name
      pc->iID = pi->id;                   // Save category ID

#ifdef SEARCH_FROM_CATALOG
      // Change state if entering a category from a search results screen.
      // See CR83673.
      if (pme->m_wState == ST_SEARCH )
      {
         MShop_SetState(pme, ST_CATEGORIES);
      }
      else
      {
         // Enumerate
         MShop_Enum(pme);
      }
#else
      // Enumerate
      MShop_Enum(pme);
#endif   // SEARCH_FROM_CATALOG

   }
}

/*===========================================================================

 Display item info 

===========================================================================*/
static void MShop_ShowItemInfo(MShop * pme, DLEnumItem * pi)
{
   uint16         wButtons[2];
   AECHAR *       pszInfo = NULL;
   AECHAR *       pTitle;
   // Reset Display
   MShop_Reset(pme,FALSE);

   // Set Information Display Flag TRUE
   pme->m_bDisplayInfo = TRUE;

   // Setup prompt structure
   MEMSET(wButtons, 0, sizeof(wButtons));
   wButtons[0] = IDC_CLOSE;

   pszInfo = MShop_CreateItemInfoString(pme, pi);

   // If there is a description for this application, Show it.
   if(pszInfo){
      pTitle = pi->pszName;
      MShop_Prompt(pme, pTitle, 0, pszInfo, 0, wButtons, wButtons, IDC_CLOSE, ST_NOSCROLL);
   }
   else {
      MShop_Prompt(pme, NULL, IDC_INFO, NULL, IDS_NOAPPINFO_AVAILABLE, wButtons, wButtons, IDC_CLOSE, ST_NOSCROLL);
   }
   FreePtr((void**)&pszInfo);
}





#ifdef APPINFO_IN_APPOPTIONS
/*=====================================================================

   Displays item info on the app options screen

   The DLEnumItem is the item whose info is to be displayed

   The AEERect defines the screen space available for the info
   display.

=====================================================================*/
static void MShop_ShowItemInfoInAppOptions(MShop* pme, DLEnumItem* pi,
   AEERect* infoRect)
{
   AEERect     rc;
   AECHAR *    pszHeader = NULL;
   AECHAR *    pszInfo = NULL;

   // Create Static Control, after ensuring that we don't already have one
   FreeObj((void **)&pme->m_pStatic);
   if (SUCCESS != CreateInstance(pme->a.m_pIShell, AEECLSID_STATIC,
      (void**)&pme->m_pStatic))
   {
      return;
   }

   // Frame the info rectangle.
   IDISPLAY_DrawRect(pme->m_pIDisplay, infoRect, RGB_BLACK, 0, IDF_RECT_FRAME);

   // Set Static text rectangle inside the specified infoRect
   SETAEERECT(&rc,
      infoRect->x + MARGIN,   // x coord of left side
      infoRect->y + MARGIN,   // y coord of top
      infoRect->dx - 2*MARGIN,// rectangle width
      infoRect->dy - 2*MARGIN // rectangle height
      );

   ISTATIC_SetRect(pme->m_pStatic, &rc);

#ifdef TITLE_4_APPINFO_IN_APPOTIONS
   // Load product info header 
   pszHeader = (AECHAR *)MALLOC(MAX_STR_SIZE);
   ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, IDC_INFO, 
                        pszHeader, MAX_STR_SIZE);     
#endif //TITLE_4_APPINFO_IN_APPOTIONS

   pszInfo = MShop_CreateItemInfoString(pme, pi);

   // Draw Header and text in static control
   ISTATIC_SetText(pme->m_pStatic, pszHeader, pszInfo, AEE_FONT_BOLD, AEE_FONT_NORMAL);
   // Store the static textfor use with flipping the cover
   MShop_StoreStaticTextInfo(pme, pszHeader, pszInfo, AEE_FONT_BOLD, AEE_FONT_NORMAL);

   ISTATIC_SetActive(pme->m_pStatic, TRUE);
   ISTATIC_Redraw(pme->m_pStatic);
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);

   // Clean up.  IStatic gets released in Mshop_Reset
   FREEIF(pszInfo);
   FREEIF(pszHeader);
}
#endif //APPINFO_IN_APPOPTIONS






/*===========================================================================

 Generate a AECHAR string for a DLEnumItem's Item Info

===========================================================================*/
static AECHAR * MShop_CreateItemInfoString(MShop * pme, DLEnumItem * pi) {
   AECHAR szSize[10] = {0};
   AECHAR * pszInfo = NULL;
   AECHAR * pszSize = NULL;
   AECHAR * pszText = NULL;
#ifdef USE_IDL2_4GB_SUPPORT
   DL2ItemSize  is;
#else
   DLItemSize  is;
#endif

   if(pi->pszDescription){
     
      // Set text for the info button
      pszText = WSTRRCHR(pi->pszDescription,(AECHAR)'|');

      if(pszText){
         // Description is setup as <title>|<text>
         pszText = pszText + 1;
      }
      else{
         pszText    = pi->pszDescription;
      }

      // Get Text on heap
      pszInfo = AppendWStr(pszInfo, pszText, '\n');
   }
   // Otherwise use resource string
   else {

      pszInfo = (AECHAR *)MALLOC(MAX_STR_SIZE);

      ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC,   
         IDS_NOAPPINFO_AVAILABLE, pszInfo, MAX_STR_SIZE);     
   }  
   
   // Append Size Info.  Get in 5K. 5MB, 5.4MB format
   // TODO: Maybe move to IDOWNLOAD2 usage for 64 bit info
#ifdef USE_IDL2_4GB_SUPPORT
   IDOWNLOAD2_GetSizeEx(pme->m_pDownload2, pi->id, &is);
   FormatMem(is.qwFSInstall, szSize, 10, FALSE);
#else
   IDOWNLOAD_GetSizeEx(pme->m_pDownload, pi->id, &is);
   FormatMem(is.dwFSInstall, szSize, 10, FALSE);
#endif
      
   // Get formatted string for free memory
   pszSize = FormatString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC,  
                          IDS_APP_SIZE_PREVIEW, szSize);
      
   // Append pszSize
   if (pszSize)
   {
      pszInfo = AppendWStr(pszInfo, pszSize, '\n');
      FREE(pszSize);
   }

   return pszInfo;
}

/*===========================================================================

 Callback from the enumeration engine.  Fill the items as they come in.

===========================================================================*/
static void MShop_EnumCB(void * pcxt, int nErr, DLEnumItem * pe)
{
   const int nRedrawInterval = 5;
   MShop *    pme = (MShop *)pcxt;
   CtlAddItem  ai;

   // Workaround for CR 85884.  See also change made in MShop_CatDown()
   // Ignore enum callbacks that come in after an app is selected.
   // Modified Mar 3 2008 to cover CR 88591.
   if ((ST_APP_OPTIONS == pme->m_wState) ||
      (ST_PURCHASE_CONFIRM == pme->m_wState) ||
      (ST_CHECK_SPACE == pme->m_wState))
   {
      return;
   }

   // On first item, clear display and set previous state as
   // current state.
   if (!pme->m_nCatItems)
   {
      MShop_Reset(pme,TRUE);
      // Reset carrier message flag
      pme->m_bCarrierMessage = FALSE;
   }

   if(nErr){
      // We encountered an error in the enumeration.  

      // Reset carrier message flag
      pme->m_bCarrierMessage = FALSE;
      
      // Display error
#ifdef SEARCH_FROM_CATALOG
      if (FALSE)
#else
      if (pme->m_bSearch && !(IDOWNLOAD_GetADSCapabilities(pme->m_pDownload) & ADS_CAP_SEARCH))
#endif // SEARCH_FROM_CATALOG
      {
         MShopCfg cfg;

         // Get server settings
         ISHELL_GetPrefs(pme->a.m_pIShell, AEECLSID_MOBILESHOP_BID, MSHOP_CFG_VER, &cfg, sizeof(cfg));

         // Disable Search
         cfg.bSearch = FALSE;

         // Save server settings
         ISHELL_SetPrefs(pme->a.m_pIShell, AEECLSID_MOBILESHOP_BID, MSHOP_CFG_VER, &cfg, sizeof(cfg));

         MShop_Failed(pme, IDS_SEARCH_NOT_SUPPORTED, 0, NULL);
      }
      else if (pme->m_bUpgradeCheck && !(IDOWNLOAD_GetADSCapabilities(pme->m_pDownload) & ADS_CAP_UPGRADECHECK))
      {
         MShopCfg cfg;

         // Get server settings
         ISHELL_GetPrefs(pme->a.m_pIShell, AEECLSID_MOBILESHOP_BID, MSHOP_CFG_VER, &cfg, sizeof(cfg));

         // Disable Upgrade
         cfg.bUpgrade = FALSE;

         // Save server settings
         ISHELL_SetPrefs(pme->a.m_pIShell, AEECLSID_MOBILESHOP_BID, MSHOP_CFG_VER, &cfg, sizeof(cfg));

         MShop_Failed(pme, IDS_UPGRADECHECK_NOT_SUPPORTED, 0, NULL);
      }
      else
         MShop_Failed(pme,IDS_LIST_FAILED,nErr,NULL);
      return;
   }

   MEMSET(&ai,0,sizeof(ai));

   if (!pe)
   {
      // Coming here means item returned is NULL. Do following and get out
      // of here.

      // If no category items found, display no categories found.
      // Otherwise just get out of here.
      if (!pme->m_nCatItems)
      {
         uint16 wButtons[2];

#ifdef FEATURE_UPGRADE_AUTO_PROMPT
         // The upgrade manager should be notified that there was no
         // upgrade available for this item.  The item ID is stashed
         // in m_iApp by MShop_Enum().
         if (NULL != pme->m_pUm)
         {
            IUM_NoUpgradeFound(pme->m_pUm, pme->m_iApp);
         }
#endif // FEATURE_UPGRADE_AUTO_PROMPT

         // Prompt structure with information that app is restored and OK
         // button to dismiss
         MEMSET(wButtons, 0, sizeof(wButtons));
         wButtons[0] = IDC_OK;
         MShop_Prompt(pme, NULL, IDC_MSHOP, NULL, IDS_NO_CATEGORIES,
            wButtons, wButtons, IDC_OK, ST_NOSCROLL);
      }
      else
      {
#ifdef SEARCH_FROM_CATALOG
         if(!pme->m_nCatIdx && !pme->m_bSearch){
            if(IDOWNLOAD_GetADSCapabilities(pme->m_pDownload) & ADS_CAP_SEARCH){
               ai.pszResText  = BREWAPPMGRLS_RES_LOC;
               ai.pszResImage = BREWAPPMGRLN_RES_FILE;
               ai.wImage = IDB_MS_FIND;
               ai.wItemID = ai.wText = IDC_MSHOP_FIND;
               IMENUCTL_AddItemEx(pme->m_pMenu,&ai);
            }
         }
#endif // SEARCH_FROM_CATALOG

         if (IMENUCTL_IsActive(pme->m_pMenu) == FALSE)
         {
            // Set menu active
            IMENUCTL_SetActive(pme->m_pMenu, TRUE);

            // Update UI
            IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);
         }
         else
         {
            // The enumeration is complete and the menu is already active,
            // so we redraw the menu to make sure the menu on screen
            // is drawn based on the number of items actually in the menu.
            // See CRs 83684 and 83710.  We skip this final redraw if
            // there was a redraw was triggered by the arrival of the
            // last item (see below).
            if (0 != pme->m_nCatItems % nRedrawInterval)
            {
               IMENUCTL_Redraw(pme->m_pMenu);
            }
         }

         MShop_AddSoftKey(pme);
      }
      return;
   }

   // On first item, if this is search, set search result as menu title
   // Otherwise set category name is menu title. If there is no category name,
   // Set catalog as menu title
   if (!pme->m_nCatItems)
   {
      if(pme->m_bSearch)
      {
#if defined(MSHOP_CMD_SEARCH_STRING) || defined(MSHOP_CMD_CATEGORY)
         if ((pme->m_pszTitle) && (*(pme->m_pszTitle)))
            MShop_SetTitle(pme, NULL, 0, pme->m_pszTitle);
         else
#endif
            MShop_SetTitle(pme, BREWAPPMGRLS_RES_LOC, IDS_SEARCH_RESULTS, NULL);
      }
      else{
         AECHAR * psz;

         // Set the title
         psz = pme->m_catList[pme->m_nCatIdx].pszName;
         if(psz)
            MShop_SetTitle(pme, NULL, 0, psz);
         else
            MShop_SetTitle(pme, BREWAPPMGRLS_RES_LOC, IDC_MSHOP_CATALOG, NULL);
      }
   }

   // Choose category/applet icon for returned item
   CheckIcon(pme->a.m_pIShell,pe->nIcon,&ai,(uint16)(pe->t == DLI_CATEGORY ? IDB_CATEGORY : IDB_APP_DEFAULT));

   // Decide ItemID for the returned item
   ai.wItemID  = IDC_ITEM + pme->m_nCatItems;

   // Add an entry in menu
   ai.pText    = pe->pszName;    // Name of the item
   ai.dwData   = (uint32)pe;     // pointer to DLEnumItem
   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);

   // Increment # of category items
   pme->m_nCatItems++;

   if (IMENUCTL_IsActive(pme->m_pMenu) == FALSE)
   {
      AEERect rc;
      int nDraw;

      IMENUCTL_GetRect(pme->m_pMenu, &rc);
      nDraw = (pme->m_cyFont ? rc.dy/pme->m_cyFont : rc.dy/ITEM_HEIGHT);

      if (pme->m_nCatItems > nDraw)
      {        
         // add soft key buttons
         MShop_AddSoftKey(pme);
         // Set menu active
         IMENUCTL_SetActive(pme->m_pMenu, TRUE);
         // Update UI
         IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

         if (pme->m_bDispTouchEnabled/* && !pme->m_bHasDPad*/)
         {
            IMENUCTL_SetActive(pme->m_pMenu, FALSE);
         }
      }
   }
   else
   {
      // The menu is already active but items continue to arrive so we
      // redraw the menu periodically so the menu on screen is drawn
      // based on the number of items actually in the menu.  See CRs
      // 83684 and 83710.
      if (0 == pme->m_nCatItems % nRedrawInterval)
      {
         FARF(MS, ("Redraw with item in EnumCB()"));
         IMENUCTL_Redraw(pme->m_pMenu);
      }
   }
}

#ifdef FEATURE_SILENT_UPGRADE_CHECK
/*===========================================================================

SILENT UPGRADE CHECK - Checks to see if an upgrade is available

===========================================================================*/

static void MShop_upgradeCheckCB(void* pcxt, int nErr, DLEnumItem * pe)
{
   //SilentUpgradeCheckInfo * pSilent = (SilentUpgradeCheckInfo *)pcxt;
   MShop* pme = (MShop*) pcxt;
   char *pszArg = 0;

   FARF(MS, ("%08x - %08x - %08x - %s", pme, pme->iID, pme->pszURL, pme->pszURL));
   
   // Only build the string the first time through
   if (pme->pszURL) 
   {
      // Convert encoded url to single-byte string
      int encLength = STRLEN(pme->pszURL);

      // Decode url from e.g. myapp%3AUpgradeAvailable%3D%25d
      IWebUtil *piWebUtil = 0;
      if (CreateInstance(pme->a.m_pIShell, AEECLSID_WEBUTIL, (void **) &piWebUtil) == 0)
      {
         char * url = (char *)MALLOC(encLength+1);
         int decLength = encLength;
         url = IWEBUTIL_UrlDecode(piWebUtil, pme->pszURL, &encLength, url, &decLength);
         IWEBUTIL_Release(piWebUtil);

         // Replace %d with 1 or 0 in:
         // myapp:UpgradeAvailable=%d
         pszArg = STRCHR(url, '%');
         if (pszArg)
         {
            if (*(pszArg+1) == 'd')
            {
               *(pszArg+1) = 0;
               if (nErr || pe == NULL)
                  *pszArg = '0';
               else
                  *pszArg = '1';
            }
         }

         // Save the URL to post when we exit
         FARF(MS, ("Silent Upgrade check is saving url '%s'", url));
         pme->m_pszUrlPostAtExit = url; // This is freed up on exit
      }
      // Prevent further calls of this callback doing anything
      // and free m_pszParam
      FREE(pme->pszURL);
      pme->pszURL = NULL;
   }
   
   // Last time (when pe is NULL), do this.
   if (NULL == pe)
   {
      FREEIF(pme->pszURL);

      // Close MobileShop as we are finished getting the callbacks
      ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
   }
}

static void MShop_doSilentUpgradeCheck(MShop * pme/*, SilentUpgradeCheckInfo * pSilent*/)
{
   FARF(MS, ("%08x - %08x - %08x - %s", pme, pme->iID, pme->pszURL, pme->pszURL));
   IDOWNLOAD_CheckItemUpgrade(pme->m_pDownload, pme->iID, MShop_upgradeCheckCB, pme);
   //
   // We're expecting callbacks from the IDownload call so we need to hang around.
   //
   ISHELL_StartBackgroundApplet(pme->a.m_pIShell, AEECLSID_MOBILESHOP_BID, "");
}

#endif

//===========================================================================
//
// DESCRIPTION
//    Function for determing Menu size when in state ST_APP_OPTIONS
// 
// PARAMETERS
//    pme:     MShop*
//    prc:     AEERect*
//
//===========================================================================
static boolean MShop_GetMenuRectForAppOptions(MShop* pme, AEERect* prc)
{
   if ((NULL == pme) || (NULL == prc))
   {
      return FALSE;
   }

#ifdef APPINFO_IN_APPOPTIONS
   {
      int            menuHeight;
      int            infoHeight;
      int            logoHeight = pme->m_rcMShopLogo.dy;

      // Partition display space exclusive of title bar into two fields
      // for the option list menu and the product info.  The option list
      // menu is on top with the product info below (CR 87879).  The info
      // height is calculated first and the menu height is what remains.

      infoHeight = NUM_APPINFO_LINES*(pme->m_cyFont + MARGIN) + 2*MARGIN;
      menuHeight = pme->m_cy - logoHeight - infoHeight;

      SETAEERECT(prc, 0, logoHeight, pme->m_cx, menuHeight);
   }
#else
   SETAEERECT(prc, 0, pme->m_rcMShopLogo.dy, pme->m_cx, pme->m_cy - pme->m_rcMShopLogo.dy);
#endif //APPINFO_IN_APPOPTIONS

   

   return TRUE;
}


/*===========================================================================

APP OPTIONS DIALOG - Shows Price, Uses, Expiration, etc.
State handler for ST_APP_OPTIONS.

===========================================================================*/
static boolean MShop_AppOptionsDialog(MShop * pme)
{
   Price *        p;
   int            i;
   DLItem *       pi;
   uint16         wID = IDC_PURCHASE;
   AEERect        menuRect;

#ifdef APPINFO_IN_APPOPTIONS
   int            infoHeight;
   //int            logoHeight = pme->m_rcMShopLogo.dy;
   AEERect        infoRect;
#endif //APPINFO_IN_APPOPTIONS

   MShop_GetMenuRectForAppOptions(pme, &menuRect);
   IMENUCTL_SetRect(pme->m_pMenu, &menuRect);

   pi = pme->m_pItem;

   // turn off force conformation
   pme->m_bConfirmDownload = FALSE;

   // Set app name as title
   MShop_SetTitle(pme, NULL, 0, pi->pszName);

#ifdef MSHOP_CMD_ITEMINFO
   if (pme->m_bDisplayInfoOnly)
   {
      MShop_ShowItemInfo(pme,(DLEnumItem *)pi);
      return(TRUE);
   }
#endif

   // We are not processing a flip, so we need to build up the contents
   // of the menu that we are displaying.
   // If this is true, we already have everything built up so we just
   // need to redisplay our text.
   if (!pme->m_bRedrawingUI)
   {
      // The overall logic of what to check and how is duplicated
      // in MShop_AcquireItemInfoCB.  If any changes are made
      // to what is added to the price options screen
      // please update that location also


      // If there is no upgrade option and demo option is available,
      // add that in menu
      if (pi->UpgradeOption.lt == LT_NONE)
      {
         if(pi->DemoOption.lt != LT_NONE)
         MShop_AddOptionMenuItem(pme, IDC_DEMO, pi, &pi->DemoOption.Price); 
      }

      // If upgrade option is available, add that in menu
      if (pi->UpgradeOption.lt != LT_NONE)
      {
#if defined(MSHOP_OPTION_MENU_ICONS)
         MShop_AddSimpleOptionMenuItem(pme, IDC_UPGRADE, IDS_EXP_UPGRADE,
            &pi->UpgradeOption.Price, IDB_MS_UPGRADE);
#else
         MShop_AddSimpleOptionMenuItem(pme, IDC_UPGRADE, IDS_EXP_UPGRADE,
            &pi->UpgradeOption.Price);
#endif // MSHOP_OPTION_MENU_ICONS
      }

      // If subscription option is available, add that in menu
      if (pi->SubscriptionOption.lt != LT_NONE)
      {
#if defined(MSHOP_OPTION_MENU_ICONS)
         MShop_AddSimpleOptionMenuItem(pme, IDC_SUBSCRIPTION,
            IDS_EXP_SUBSCRIPTION, &pi->SubscriptionOption.Price,
            IDB_MS_SUBSCRIBE);
#else
         MShop_AddSimpleOptionMenuItem(pme, IDC_SUBSCRIPTION,
            IDS_EXP_SUBSCRIPTION, &pi->SubscriptionOption.Price);
#endif // MSHOP_OPTION_MENU_ICONS
      }

      // If purchase option is available, add all of the purchase prices
      // in the menu
      if (pi->PurchaseOption.lt != LT_NONE)
      {
         for (i = 0, p = pi->PurchaseOption.pPrices;
            i < pi->PurchaseOption.nPrices; i++, p++)
         {
            wID = MShop_AddOptionMenuItem(pme, wID, pi, p);
         }
      }
   } // if (!pme->m_bRedrawingUI)

   MShop_AddSoftKey(pme);

#ifdef APPINFO_IN_APPOPTIONS
   // Partition display space exclusive of title bar into two fields
   // for the option list menu and the product info.  The option list
   // menu is on top with the product info below (CR 87879).  The info
   // height is calculated first and the menu height is what remains.
   
   // Using the old menu heght, to determine height of the text
   infoHeight = pme->m_cy - menuRect.y - menuRect.dy;

   // Get the new, to figure out the real start y.
   IMENUCTL_GetRect(pme->m_pMenu, &menuRect);
   SETAEERECT(&infoRect, 0, menuRect.y + menuRect.dy, pme->m_cx, infoHeight);

   // Add App Info
   MShop_ShowItemInfoInAppOptions(pme, (DLEnumItem*)pi, &infoRect);
#else
   if (!pme->m_bRedrawingUI)
   {
      // Add Info option
      CtlAddItem ai;

      MEMSET(&ai, 0, sizeof(ai));
      ai.pszResText  = BREWAPPMGRLS_RES_LOC;
      ai.pszResImage = BREWAPPMGRLN_RES_FILE;
      ai.wFont = AEE_FONT_NORMAL;
      ai.wText = IDC_INFO;
      ai.wImage = IDB_MS_INFO;
      ai.wItemID = IDC_INFO;
      ai.dwData = (uint32)pi;

      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   }
#endif //APPINFO_IN_APPOPTIONS

   // Activate Menu
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

   if (pme->m_bDispTouchEnabled /*&& !pme->m_bHasDPad*/)
   {
      IMENUCTL_SetActive(pme->m_pMenu, FALSE);
   }

   return(TRUE);
}








/*===========================================================================

Adds "<price info> - < expiration string for non expired items> in menu and
increase item ID for next items.

===========================================================================*/
static uint16 MShop_AddOptionMenuItem(MShop * pme, uint16 wItemID, DLItem * pItem, Price * p)
{
   AECHAR *       psz;
   AECHAR *       pszOption;
   int            nLeft;
   uint32         dwNow, dwExpire;
   AEELicenseType lt = ((wItemID == IDC_DEMO) ? pItem->DemoOption.lt : pItem->PurchaseOption.lt);

   pszOption = (AECHAR *)MALLOC(MAX_STR_SIZE);
   if(!pszOption)
      return(wItemID);

   // This will file pszOption with "<price info> - "
   psz = FillPrice(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, p->pszPrice, pszOption, MAX_STR_SIZE);

   nLeft = MAX_STR_SIZE - WSTRSIZE(pszOption);

   // Prepage for getting expiration string
#ifdef GETUTCSECONDS
            dwNow = GETUTCSECONDS();
#else
            dwNow = GETTIMESECONDS();
#endif

   if(lt == LT_DAYS && pItem->currentLicense.lt == LT_DATE){
      dwExpire = pItem->currentLicense.dwExpire;
      if(dwExpire > dwNow)
         dwNow = dwExpire;
   }

   // Add "<price info> - < expiration string for non expired items> in menu and
   // increase item ID for next items.
   if(GetExpireString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, dwNow, lt,
      p->dwValue, psz, nLeft, FALSE, TRUE))
   {
#if defined(MSHOP_OPTION_MENU_ICONS)
      CtlAddItem ai;

      // Initialize ai
      MEMSET(&ai, 0, sizeof(ai));

      ai.pText = pszOption;
      ai.pszResImage = BREWAPPMGRLN_RES_FILE;
      ai.wFont = AEE_FONT_NORMAL;
      ai.wImage = (uint16)((wItemID == IDC_DEMO) ? IDB_MS_DEMO : IDB_MS_PURCHASE);
      ai.wItemID = wItemID++;
      ai.dwData = (uint32)p;

      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
#else
      IMENUCTL_AddItem(pme->m_pMenu, NULL, 0, wItemID++, pszOption, (uint32)p);
#endif // MSHOP_OPTION_MENU_ICONS
   }

   FREE(pszOption);
   return(wItemID);
}

/*===========================================================================

Adds "FREE - <wIDText>" or "<price info> - <wIDText>" to the app options menu
and returns ItemID for added menu item. This is used for "Subscription" and
"Upgrade"

===========================================================================*/
static uint16 MShop_AddSimpleOptionMenuItem(MShop * pme, uint16 wItemID,
#if defined(MSHOP_OPTION_MENU_ICONS)
   uint16 wTextID, Price * p, uint16 wIconID)
#else
   uint16 wTextID, Price * p)
#endif // MSHOP_OPTION_MENU_ICONS
{
   AECHAR *    psz;
   AECHAR *    pszOption;
   int         nLeft;
#if defined(MSHOP_OPTION_MENU_ICONS)
   CtlAddItem  ai;

   // Initialize ai
   MEMSET(&ai, 0, sizeof(ai));
#endif // MSHOP_OPTION_MENU_ICONS

   pszOption = (AECHAR *)MALLOC(MAX_STR_SIZE);
   if(!pszOption)
      return(wItemID);

   if (IDC_SUBSCRIPTION == wItemID) {
#define SUBSCRIPTION_STR_LEN 256
      AECHAR  szSubscriptionRate[SUBSCRIPTION_STR_LEN];
      AECHAR  szMonthly[64];
    
      ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC,  
                           IDS_MONTHLY, szMonthly, sizeof(szMonthly));
      WSTRLCPY(szSubscriptionRate, p->pszPrice, SUBSCRIPTION_STR_LEN);
      WSTRLCAT(szSubscriptionRate, szMonthly, SUBSCRIPTION_STR_LEN);
      psz = FillPrice(pme->a.m_pIShell,BREWAPPMGRLS_RES_LOC,    
                      szSubscriptionRate, pszOption, MAX_STR_SIZE);
   }
   else {
      psz = FillPrice(pme->a.m_pIShell,BREWAPPMGRLS_RES_LOC, p->pszPrice, 
                      pszOption, MAX_STR_SIZE);
   }


   nLeft = MAX_STR_SIZE - WSTRSIZE(pszOption);

   ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, wTextID, psz, nLeft);

#if defined(MSHOP_OPTION_MENU_ICONS)
   ai.pText = pszOption;
   ai.pszResImage = BREWAPPMGRLN_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.wImage = wIconID;
   ai.wItemID = wItemID++;
   ai.dwData = (uint32)p;

   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
#else
   IMENUCTL_AddItem(pme->m_pMenu, NULL, 0, wItemID++, pszOption, (uint32)p);
#endif // MSHOP_OPTION_MENU_ICONS
   
   FREE(pszOption);
   return(wItemID);
}

/*===========================================================================

If wIcon is a valid resource in ADS_ICONS_FILE, specify this in pai.
Else if wIcon is a valid resource in CAT_ICONS_RES_FILE, specify this in pai.
Else specify wDef from APPMGR_RES_FILE in pai.

===========================================================================*/
static void CheckIcon(IShell * ps, uint16 wIcon, CtlAddItem * pai, uint16 wDef)
{
   if(wIcon && ISHELL_IsValidResource(ps, ADS_ICONS_FILE, wIcon, RESTYPE_IMAGE, HTYPE_VIEWER)){
      pai->pszResImage = ADS_ICONS_FILE;
      pai->wImage = wIcon;
   }
   else{
      if(wIcon && ISHELL_IsValidResource(ps, CAT_ICONS_RES_FILE, wIcon, RESTYPE_IMAGE, HTYPE_VIEWER)){
         pai->pszResImage = CAT_ICONS_RES_FILE;
         pai->wImage = wIcon;
      }
      else{
         pai->pszResImage = BREWAPPMGRLN_RES_FILE;
         pai->wImage = wDef;
      }
   }
}

/*===========================================================================

Returns TRUE if pszPrice doesn't contain any digits indicating price. Returns
FALSE otherwise.

===========================================================================*/
static boolean IsFree(AECHAR * pszPrice)
{
   AECHAR   ch;
   boolean  bFree = TRUE;

   if(pszPrice){
      while((ch = *pszPrice) != (AECHAR)0){
         if(ch >= (AECHAR)'1' && ch <= (AECHAR)'9'){
            bFree = FALSE;
            break;
         }
         pszPrice++;
      }
   }
   return(bFree);
}

/*===========================================================================

Fills specified string with "FREE - " if pszPrice doesn't contain any digits 
indicating price or with "<price info> - " otherwise. Returns end of the string.

===========================================================================*/
static AECHAR * FillPrice(IShell * ps, const char * pszRes, AECHAR * pszPrice, AECHAR * pszDest,int nSize)
{
   if(IsFree(pszPrice))
      ISHELL_LoadResString(ps, pszRes, IDS_FREE, pszDest, nSize);
   else
      WSTRLCPY(pszDest, pszPrice, nSize / sizeof(AECHAR));

   WSTRLCAT(pszDest, L" - ", nSize / sizeof(AECHAR));
   pszDest += WSTRLEN(pszDest);

   return(pszDest);
}

/*===========================================================================

APPLICATION PURCHASE CONFIRMATION DIALOG

===========================================================================*/

/*===========================================================================

Displays EULA with "Accept" and "Cancel" buttons with "Cancel" as default
button.

===========================================================================*/
static void MShop_EULADialog(MShop * pme, const AECHAR * pszMsg)
{
   uint16         wButtons[3];

   // CR84990 - conditionalize state change here.
   if (pme->m_wState == ST_IN_PROGRESS)
   {
      // Reset UI while setting last state as current state
      MShop_Reset(pme,TRUE);
   }
   else
   {
      // Reset UI without changing state
      MShop_Reset(pme, FALSE);
   }

   // Display EULA prompt with EULA and "Accept"/"Cancel" buttons.
   MEMSET(wButtons,0,sizeof(wButtons));
   wButtons[0] = IDC_ACCEPT;
   wButtons[1] = IDC_CANCEL;

   MShop_Prompt(pme, NULL, IDS_LICENSE, pszMsg, 0, wButtons, wButtons, IDC_ACCEPT, ST_NOSCROLL);
}

/*===========================================================================

Callback function for IDOWNLOAD_GetEULA

===========================================================================*/
static void MShop_EULACB(void * pcxt,int nErr,const AECHAR * pszMsg)
{
   MShop *       pme = (MShop *)pcxt;

   // Free EULA buffer
   FreePtr((void **)&pme->m_pszEULA);

   // Set got EULA flag to TRUE
   pme->m_bGotEULA = TRUE;

   // Cache EULA
   if(pszMsg && *pszMsg)
      pszMsg = pme->m_pszEULA = WSTRDUP(pszMsg);
   else
      pszMsg = NULL;

   // Waiting for EULA - Just show it, if we got it or move to
   // ST_PURCHASE state
   if(pme->m_wLastState == ST_PURCHASE_EULA){ 
      if(pszMsg)
         MShop_EULADialog(pme,pszMsg);
      else
         MShop_SetState(pme, ST_PURCHASE);
   }
}
   
/*===========================================================================

State handler for ST_PURCHASE_EULA state.

===========================================================================*/
static boolean MShop_GetEULA(MShop * pme)
{
   DLItem   * pi = pme->m_pItem;

   // If there is no EULA, or valid license or we have already got EULA,
   // move to Purchase state
   if(!pi->bEULA || pi->currentLicense.pt || (pme->m_bGotEULA && !pme->m_pszEULA))
      MShop_SetState(pme,ST_PURCHASE);
   else{
      if(!pme->m_pszEULA){
         // Show progress graphics
         MShop_InProgress(pme);
         // Get EULA
         IDOWNLOAD_GetEULA(pme->m_pDownload,pme->m_iApp,MShop_EULACB,pme);
      }
      else
         MShop_EULADialog(pme,pme->m_pszEULA); // Display EULA
         
   }      
   return(TRUE);
}






/*===========================================================================

Returns FALSE if app wouldn't fit or couldn't create price string. For
Demo apps, sends to get EULA. Otherwise display purchase confirmation dialog
after initiating call to fetch EULA if necessary. 
State handler for ST_CHECK_SPACE.

===========================================================================*/

static boolean MShop_CheckSpaceDialog(MShop * pme)
{
   DLItem * pi = pme->m_pItem;
   // Checking for adequate phone space
   if (MShop_CheckDownloadToPhone(pme, pi->iID, pi->pszName))
   {
      // We have enough space, just proceed to the next state
      return MShop_SetState(pme, ST_PURCHASE_CONFIRM);
   }

   return FALSE;
}



/*===========================================================================

Returns FALSE if app wouldn't fit or couldn't create price string. For
Demo apps, sends to get EULA. Otherwise display purchase confirmation dialog
after initiating call to fetch EULA if necessary. 
State handler for ST_PURCHASE_CONFIRM.

===========================================================================*/

static boolean MShop_ConfirmPurchaseDialog(MShop * pme)
{
   DLItem * pi = pme->m_pItem;
   uint16 wID;
   AECHAR *pText = NULL;
   boolean bIsFree;

   // If it's a demo, then just send them to the EULA
   FARF(MS, ("Checking to see if App is free"));
   bIsFree = IsFree(pme->m_pPrice->pszPrice);
   
   FARF(MS, ("Checking to see if if we need to confirm download"));
   if (!pme->m_bConfirmDownload && (pme->m_bDemo || bIsFree))
   {
      MShop_SetState(pme, ST_PURCHASE_EULA);
      return(TRUE);
   }

   // The confirmation dialog, whatever form it takes, is conditionalized
   // based on whether we already know the download destination or not.
   // If so there's no point offering the user a choice of destinations.
   // Otherwise the confirmation dialog needs to elicit that choice from
   // the user.
   if (pme->m_bSubscription)
   {
#if defined(CONFIRM_SUBSCRIPTION_PURCHASE)
      // You would be charged %s per month for this app. Proceed?
      wID = IDS_SUBSCRIPTION_PURCHASE_CONFIRM;
#else
      // You would be charged %s for this app. Proceed?
      wID = IDS_PURCHASE_CONFIRM;
#endif // CONFIRM_SUBSCRIPTION_PURCHASE
   }
   else
   {
      if (bIsFree)
      {
         //You are about to download this app. Proceed?
         wID = IDS_DOWNLOAD_CONFIRM;
      }
      else
      {
         // You would be charged %s for this app. Proceed?
         wID = IDS_PURCHASE_CONFIRM;
      }
   }

   FARF(MS, ("We have our purchase confirm string"));

   // Fetch EULA if there is EULA associated with this app and we havn't got it.
   if(pi->bEULA && !pme->m_bGotEULA)
   {
      FARF(MS, ("Downloading the EULA in the background"));
      IDOWNLOAD_GetEULA(pme->m_pDownload,pme->m_iApp,MShop_EULACB,pme);
   }

   // If app is already installed, display "This app is already installed.  
   // You will be charged  %s for the additional purchase.  Are you sure?"
   if (pi->currentLicense.pt != PT_NONE && pi->currentLicense.pt != PT_DEMO)
   {
      wID = IDS_ALREADY;
   }

   pText = FormatString(pme->a.m_pIShell,BREWAPPMGRLS_RES_LOC, wID,
      pme->m_pPrice->pszPrice);
   if (NULL != pText)
   {
      uint16 wButtons[3];
      MEMSET(wButtons, 0, sizeof(wButtons));
      wButtons[0] = IDC_YES;
      wButtons[1] = IDC_CANCEL;
      MShop_Prompt(pme, pi->pszName, 0, pText, 0, wButtons, wButtons,
         wButtons[0], ST_NOSCROLL);

      FreePtr((void**)&pText);

      return(TRUE);
   }
   return(FALSE);
}






/*===========================================================================

State handler for ST_PURCHASE state

===========================================================================*/
static boolean MShop_Purchase(MShop * pme)
{
   if (pme->m_pItem)
   {
      // Display progress graphics
      MShop_InProgress(pme);

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
      if(pme->m_bDispTouchEnabled)
      {
         uint16 resID[1];
         resID[0] = IDC_CANCEL;
         MShop_CreateSoftmenu(pme,resID, 1);
         MShop_SetMenuRect(pme);
      }
#endif

#ifdef FEATURE_DISABLE_SS_DURING_DOWNLOAD
      // Get the current screen saver, and if it is set to something, set it to 0
      // so it does not disrupt the download
      pme->m_clsScreenSaver = ISHELL_GetHandler(pme->a.m_pIShell, AEECLSID_SCREEN_SAVER, MT_SCREEN_SAVER);

      if (pme->m_clsScreenSaver)
      {
         ISHELL_RegisterHandler(pme->a.m_pIShell, AEECLSID_SCREEN_SAVER, MT_SCREEN_SAVER, 0);
      }
#endif // FEATURE_DISABLE_SS_DURING_DOWNLOAD

      // Acquire the application
      IDOWNLOAD_Acquire(pme->m_pDownload, pme->m_pItem->iID,
         pme->m_pPrice->dwHandle,MShop_PurchaseCB,pme);
   }
   return(TRUE);
}


/*===========================================================================

State handler for ST_PURCHASE_RESUME state

===========================================================================*/
static boolean MShop_PurchaseResume(MShop * pme)
{
   if (pme->m_pItem)
   {
      AEERect     rc;
      uint16 wButtonText[2];
      uint16 wButtonAction[2];
      MEMSET(wButtonText, 0, sizeof(wButtonText));
      MEMSET(wButtonAction, 0, sizeof(wButtonAction));
      wButtonText[0] = IDC_OK;
      wButtonAction[0] = IDC_CANCEL;

      SETAEERECT(&rc, 0, 0, pme->m_cx, pme->m_cy);

      // Erase the screen to get rid of any old info there.
#ifdef USE_TT_FONTS
      IDISPLAY_FillRect(pme->m_pIDisplay, &rc, RGB_WHITE);
#else
      IDISPLAY_EraseRect(pme->m_pIDisplay, &rc);
#endif // USE_TT_FONTS

      // turn on force conformation, so that the "you will be charged $ for this app" screen re-displays
      pme->m_bConfirmDownload = TRUE;

      MShop_Prompt(pme, NULL, IDS_PURCHASE_DONT_PANIC_TITLE, NULL, IDS_PURCHASE_DONT_PANIC, wButtonText, wButtonAction,
         IDC_CANCEL, ST_NOSCROLL);

      return(TRUE);   
   }
   return(TRUE);
}



/*===========================================================================

State handler for ST_RESTORE_APP

===========================================================================*/
static boolean MShop_Withdraw(MShop * pme)
{
   if (pme->m_iApp)
   {   
#ifdef USE_IDL2_4GB_SUPPORT
      DL2AppModInfo * pmi = IDOWNLOAD2_GetModInfoEx(pme->m_pDownload2, pme->m_iApp, GMI_ALL);
#else
      AppModInfo * pmi = IDOWNLOAD_GetModInfoEx(pme->m_pDownload, pme->m_iApp, GMI_ALL);
#endif
      if (pmi)
      {
         boolean bDownload;

         // Load App Name
         AECHAR* pszName = LoadAppNames(pme->a.m_pIShell, pmi->apps, pmi->nApps);

         // Check for RAM
         bDownload = MShop_CheckDownloadToPhone(pme, pme->m_iApp, pszName);

         // Free Non Null name buffer
         FreePtr((void**)&pszName);

         if (bDownload)
         {
            // Display progress graphics
            MShop_InProgress(pme);

            // Restore
            IDOWNLOAD_Restore(pme->m_pDownload, pme->m_iApp,MShop_WithdrawCB,pme);
            return(TRUE);
         }
      }    
   }
   return FALSE;
}

/*===========================================================================

IDOWNLOAD_Withdraw callback

===========================================================================*/
static void MShop_WithdrawCB(void * pcxt,int nErr, void * pUnused)
{
   MShop * pme = (MShop *)pcxt;
   
   // Don't Show Busy
   pme->m_bShowBusy = FALSE;

   // Reset UI setting last state as current state
   MShop_Reset(pme,TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);

   if(nErr){
      // Restore failed

      // Do not attempt to open this app when we close down
      MShop_ClearStartArgs(pme);

      // Display failure notice
      MShop_Failed(pme,IDS_RESTORE_FAILED,nErr,NULL);
      return;
   }

   // Run app
   MShop_RunApp(pme);
}


/*===========================================================================

IDOWNLOAD_Acquire callback function

===========================================================================*/
static void MShop_PurchaseCB(void * pcxt,int nErr,void * pMsgArg)
{
   const AECHAR * pMsg = (const AECHAR *)pMsgArg;
   MShop *       pme = (MShop *)pcxt;
#ifdef USE_IDL2_4GB_SUPPORT
   DL2AppModInfo *   pmi;
#else
   AppModInfo *   pmi;
#endif
   FARF(MS, ("In MShop_PurchaseCB, download finished with err %d", nErr));

   // Don't Show Busy
   pme->m_bShowBusy = FALSE;

   // Reset UI while setting last state as current state
   // The "Current state" is "InProgress" before this call
   // After it, it should be "Purchase"
   MShop_Reset(pme,TRUE);

#ifdef FEATURE_DISABLE_SS_DURING_DOWNLOAD
   // If we have a screen saver class set, re-turn on the screen saver
   if (0 != pme->m_clsScreenSaver)
   {
      ISHELL_RegisterHandler(pme->a.m_pIShell, AEECLSID_SCREEN_SAVER, MT_SCREEN_SAVER, pme->m_clsScreenSaver);
   }
   pme->m_clsScreenSaver = 0; // clear our memory, so we don't try to re-set it again
#endif // FEATURE_DISABLE_SS_DURING_DOWNLOAD


   if (nErr)
   {
      MShop_Failed(pme,IDS_DOWNLOAD_FAILED,nErr,pMsg); // Download failed

      // Do this second, so the Failed text shows up on primary screen
      if (pme->m_bExitAfterDownload)
      {
         FARF(MS, ("Not exiting MobileShop after download because of error"));

         // update exterior display with IDS_CONTINUE_DOWNLOAD_FAILED
         if (pme->m_pStaticDlInfo)
         {
            MShop_SetExternalStaticText(pme, IDS_CONTINUE_DOWNLOAD_FAILED);
         }
      }
   }
   else
   {
      uint16 wButtons[3];

      MEMSET(wButtons, 0, sizeof(wButtons));

      // Cache the download ID
      pme->m_iApp = pme->m_pItem->iID;

      // Cache the name
      WSTRNCOPYN(pme->m_szApp, sizeof(pme->m_szApp) / sizeof(AECHAR),
         pme->m_pItem->pszName, -1);

      // Get module information
#ifdef USE_IDL2_4GB_SUPPORT
      pmi = IDOWNLOAD2_GetModInfoEx(pme->m_pDownload2, pme->m_iApp, GMI_ALL);
#else
      pmi = IDOWNLOAD_GetModInfoEx(pme->m_pDownload, pme->m_iApp, GMI_ALL);
#endif

      if (pmi && pmi->nApps)
      {
         int nPos;
         IFile * pIFile = NULL;
         pIFile = IFILEMGR_OpenFile(pme->m_pIfm, CONFIG_FILE, _OFM_READWRITE);
         if ((NULL != pIFile) &&
            (IFILE_Seek(pIFile, _SEEK_END, 0) == AEE_SUCCESS) &&
            ((nPos = IFILE_Seek(pIFile, _SEEK_CURRENT, 0)) != 0))
         {
            char * pBuffer = NULL;
            int i;

            pBuffer = MALLOC(4*(((pmi->nApps*2*CLSID_LENGTH) + 1 + 3)/4));

            if (pBuffer)
            {
               uint32 nBuffer = 0;
               for (i = 0; i < pmi->nApps; i++)
               {
                  SPRINTF(pBuffer + nBuffer, "0x%x,0x%x;", pmi->apps[i].cls,
                     pme->m_iApp);
                  nBuffer += STRLEN(pBuffer + nBuffer);
               }

               if (IFILE_Write(pIFile, pBuffer, nBuffer) != nBuffer)
                  IFILE_Truncate(pIFile, nPos);

               FreePtr((void**)&pBuffer);
            }
            else
            {
               char szLine[32];
               for (i = 0; i < pmi->nApps; i++)
               {
                  SPRINTF(szLine, "0x%x,0x%x;", pmi->apps[i].cls, pme->m_iApp);
                  if (IFILE_Write(pIFile, szLine, STRLEN(szLine)) !=
                     STRLEN(szLine))
                  {
                     IFILE_Truncate(pIFile, nPos);
                     break;
                  }
               }
            }
         }
         FreeObj((void**)&pIFile);

         ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID, BAM_EVT_APP_ADDED, 0, 0);
         
         // If we make updating the AppList happen only when necessary
         // This is one of the spots that it needs to happen

      }

      // If the user had tried to exit MobileShop by closing the slider/flip
      // we now exit.  We have finished downloading and notifying BAM that 
      // an app was downloaded.
      if (pme->m_bExitAfterDownload)
      {
         FARF(MS, ("Exiting MobileShop after download"));
         MShop_ForceExit(pme);
         return;
      }


      if (pmi && pmi->nApps && !(pmi->apps[0].wFlags & AFLAG_HIDDEN))
      {            
         //Display installed message with chance to run it "Yes"/"No" prompt
         wButtons[0] = IDC_YES;
         wButtons[1] = IDC_NO;
         MShop_Prompt(pme, pme->m_szApp, 0, NULL, IDS_INSTALLED_APP, wButtons,
            wButtons, IDC_YES, ST_NOSCROLL);
      }
      else
      {
         wButtons[0] = IDC_OK;
         // App hidden - Display installed message with "OK" button to dismiss
         MShop_Prompt(pme, pme->m_szApp, 0, NULL, IDS_INSTALLED, wButtons,
            wButtons, IDC_OK, ST_NOSCROLL);
      }


#ifdef FEATURE_UPGRADE_AUTO_PROMPT
      // Notify the UpgradeMgr that the purchase was made.
      if (NULL != pme->m_pUm)
      {
         IUM_ItemPurchased(pme->m_pUm, pme->m_iApp);
      }
#endif // FEATURE_UPGRADE_AUTO_PROMPT

   }
}

/*===========================================================================

Starts an installed application (closes MobileShop)

===========================================================================*/
static boolean MShop_RunApp(MShop * pme)
{
#ifdef USE_IDL2_4GB_SUPPORT
   DL2AppModInfo * pmi;
#else
   AppModInfo * pmi;
#endif

   // If no applet pending for start at close of MobileShop, set current
   // applet as one
   if(!pme->m_clsOpenAtExit){
#ifdef USE_IDL2_4GB_SUPPORT
      pmi = IDOWNLOAD2_GetModInfoEx(pme->m_pDownload2, pme->m_iApp, GMI_ALL);
#else
      pmi = IDOWNLOAD_GetModInfoEx(pme->m_pDownload, pme->m_iApp, GMI_ALL);
#endif
      if(pmi && pmi->nApps)
      {
         pme->m_clsOpenAtExit = pmi->apps[0].cls;
         ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID, BAM_EVT_APP_LAUNCHED, 0, pme->m_clsOpenAtExit);
      }
      else
         MShop_ClearStartArgs(pme);
   }

   ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
   return(TRUE);
}

//===========================================================================
// 
// DESCRIPTION
//    General Purpose Prompt
//
// PARAMETERS
//    pme:     the MShop
//    szTitle: title string
//    wIDTitle:title string resource ID  
//    szText:  text string
//    wIDText: text string resource ID  
//    wButtons:array of button resource IDs
//    wIDDef:  resource ID of default button
//    dwProps: static control properties
//
// Return Value:
//    None
//
//===========================================================================
static void MShop_Prompt(MShop * pme, const AECHAR* szTitle, uint16 wIDTitle, const AECHAR* szText, uint16 wIDText, uint16* wButtons, uint16* wItemIDs, uint16 wDefID, uint32 dwProps)
{
   AEERect rc, rcm;
   uint16  nHeight, x, y;
   uint16* origButtons = wButtons;
   AECHAR* szTextBuf = NULL;
   IMenuCtl * pMenu = NULL;
#ifndef PROMPT_ALWAYS_USE_SK
   int nItems = 0;
#endif // PROMPT_ALWAYS_USE_SK

   // Create Static control for prompt
   FreeObj((void **)&pme->m_pStatic);
   if (CreateInstance(pme->a.m_pIShell, AEECLSID_STATIC, (void**)&pme->m_pStatic))
   {
      return;
   }

   // Only store the prompt info if we are not processing the flip event as
   // if we are processing the flip event, the values stored are the values being used
   if (!pme->m_bRedrawingUI)
   {
      // Store the info we were called with so that we can re-display the prmpt if the cover is flipped
      MShop_StorePromptInfo(pme, szTitle, wIDTitle, szText, wIDText, wButtons, wItemIDs, wDefID, dwProps);
   } //if (!pme->m_bRedrawingUI)
   
   // Initialize to 0 to make ARM compiler happy
   nHeight = x = y = 0;

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

   if (!szText)
   {
      szTextBuf = (AECHAR *)MALLOC(MAX_STR_SIZE);
   }
   else
   {
      szTextBuf = (AECHAR *)szText;
   }

   // Add items in soft key
   // The buttons should already be in the menu, so don't re-add them
   // If we are processing the flip event
   if (!pme->m_bRedrawingUI)
   {
      while (*wButtons && *wItemIDs)
      {
         IMENUCTL_AddItem(pMenu, BREWAPPMGRLS_RES_LOC, *wButtons, *wItemIDs, NULL, 0);
         wButtons++;
         wItemIDs++;
      }
   } //if (!pme->m_bRedrawingUI)

   if (pMenu == pme->m_pMenu)
   {
#ifndef PROMPT_ALWAYS_USE_SK
      AEERect itemRect;

      IMENUCTL_GetItemRect(pMenu, 0, &itemRect);
      nHeight = nItems * itemRect.dy;

      SETAEERECT(&rcm, 0, pme->m_cy - nHeight, pme->m_cx, nHeight);
      IMENUCTL_SetRect(pMenu, &rcm);
#else
      // Should never execute this
#endif // PROMPT_ALWAYS_USE_SK
   }
   else
   {
      // Get Softkey Size
      IMENUCTL_GetRect(pMenu, &rcm);

      // SoftKey Height
      nHeight = rcm.dy;
   }

   // Set Static Control Size
   if (szTitle || wIDTitle)
   {
      if(pme->m_bDispTouchEnabled)
      {
         SETAEERECT(&rc, 0, pme->m_rcMShopLogo.dy, pme->m_cx, pme->m_cy - nHeight - pme->m_rcMShopLogo.dy - TOUCH_PIXELPAD);
      }
      else
      {
         SETAEERECT(&rc, 0, pme->m_rcMShopLogo.dy, pme->m_cx, rcm.y - pme->m_rcMShopLogo.dy);
      }
   }
   else
   {
      x = (pme->m_cx - (pme->m_cx*DISPLAY_AREA_PERCENT/100))/2;
      y = (pme->m_cy - (pme->m_cy*DISPLAY_AREA_PERCENT/100))/2;

      // Draw Border
      SETAEERECT(&rcm, x-1, y-1, pme->m_cx - 2*(x-1), pme->m_cy - 2*(y-1));
#ifdef USE_TT_FONTS
      IDISPLAY_FillRect(pme->m_pIDisplay, &rcm, RGB_WHITE);
#else
      IDISPLAY_EraseRect(pme->m_pIDisplay, &rcm);
#endif // USE_TT_FONTS
      IDISPLAY_DrawRect(pme->m_pIDisplay, &rcm, RGB_BLACK, 0, IDF_RECT_FRAME);

      // Set Softkey Size
      SETAEERECT(&rcm, x, pme->m_cy - y - nHeight, pme->m_cx - 2*x, nHeight);
      IMENUCTL_SetRect(pMenu, &rcm);

      // Calculate Static Control Size
      SETAEERECT(&rc, x + MARGIN, y + MARGIN, pme->m_cx - 2*(x + MARGIN), pme->m_cy - 2*(y + MARGIN) - nHeight);
   }
   ISTATIC_SetRect(pme->m_pStatic, &rc);

   // Set Static Control Properties
   if (dwProps)
      ISTATIC_SetProperties(pme->m_pStatic, dwProps);

   // Set Static Control Text and display it
   if (!szText)
      ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, wIDText, szTextBuf, MAX_STR_SIZE);

   ISTATIC_SetText(pme->m_pStatic, NULL, szTextBuf, 0, AEE_FONT_NORMAL);
   // Don't store the static text as it is already being stored for use
   // by re-calling prompt on Flip.
   //MShop_StoreStaticTextInfo(pme, NULL, szTextBuf, 0, AEE_FONT_NORMAL);
   ISTATIC_SetActive(pme->m_pStatic, TRUE);
   ISTATIC_Redraw(pme->m_pStatic);

   
   // The selection should already be selected if we are re-displaying the 
   // prompt becuase the user flipped the screen while it was active.
   if (!pme->m_bRedrawingUI)
   {
      IMENUCTL_SetSel(pMenu, wDefID);
   }
   // Set up SoftKeys
   if(pme->m_bDispTouchEnabled)
   {
      if (pMenu == pme->m_pMenu)
      {
         IMENUCTL_SetActive(pMenu, TRUE);
      }
      else
      {
         // CR90583 fix: Resizing softmenu width dynamically according to string being displayed; Set width to default SOFTMENU_WIDTH if the string is smaller - for being tangible!!!
         int nStrWidth = 0;
         int pnFits = 0;
         int nSoftmenuWidth = 0;
         AECHAR    szTemp[MAX_TOUCH_SOFTMENUSTRING];
         
         
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
         IMENUCTL_SetActive(pMenu, TRUE);
      }

      IMENUCTL_Redraw(pMenu);
      //if (!pme->m_bHasDPad)
      {
         IMENUCTL_SetActive(pMenu, FALSE);
      }
   }
   else
   {
      IMENUCTL_SetActive(pMenu, TRUE);
      IMENUCTL_Redraw(pMenu);
   }

#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
      pme->m_bPromptIsDisplayed = TRUE;
#endif

   if (szTitle || wIDTitle)
   {
      MShop_SetTitle(pme, BREWAPPMGRLS_RES_LOC, wIDTitle, szTitle);
   }

   if (!szText)
   {
      FREE(szTextBuf);
   }

   // Update UI
   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);
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
boolean MShop_GetMenuRect(MShop * pme, IMenuCtl* pSKMenu, AEERect* prc)
{

   if ((NULL == pme) || (NULL == prc))
   {
      return FALSE;
   }

   // Set the default MenuRect
   switch (pme->m_wState)
   {
      case ST_SERVER:
      {
         return MShop_GetMenuRectForSetServer(pme, prc);
      }

      case ST_VIEW_LOG:
      {
         return MShop_GetMenuRectForLogDialog(pme, pSKMenu, prc);         
      }

      case ST_APP_OPTIONS:
      {
         return MShop_GetMenuRectForAppOptions(pme, prc);
      }

      case ST_SEARCH:
      case ST_SEARCH_CMD:
      case ST_CREDITBACK:
      {
         //return MShop_GetMenuRectForTextCtlMenu(pme, prc);
      }


      default:
      {
         SETAEERECT(prc, 0, pme->m_rcMShopLogo.dy, pme->m_cx, pme->m_cy - pme->m_rcMShopLogo.dy);
         return TRUE;
      }
   }
}

/*===========================================================================

Resets all MobileShop UI controls. If called with TRUE, last state is
set as current state.

===========================================================================*/
static void MShop_Reset(MShop * pme, boolean bRestoreState)
{
   IMenuCtl *  pm;
   uint32      dwProps;
   AEERect     rc;

   if (pme->m_bShowBusy)
   {
      // Show Busy Image
      IImage * pImage = ISHELL_LoadBusyImage(pme->a.m_pIShell);

      if (pImage)
      {
         AEEImageInfo ii;
         IIMAGE_GetInfo(pImage, &ii);
         IIMAGE_SetParm(pImage, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
         IIMAGE_Draw(pImage, (pme->m_cx - ii.cx)/2, (pme->m_cy - ii.cy)/2);
         IIMAGE_Release(pImage);
         IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
      }
   }
   else
      pme->m_bShowBusy = TRUE;

   // Reset AI Prompt Flag
   pme->m_bAIPrompt = FALSE;

   // End active dialog
   ISHELL_EndDialog(pme->a.m_pIShell);

   // Free the Text Control if we are not in a search or creditback
   // state AND we are not in the process of suspending or resuming.
   // This is because when we suspend it may be for an OEM bringing
   // up a keypad on the touch screen that they user can use for
   // filling in text.
   if (!(((ST_CREDITBACK == pme->m_wState) ||
          (ST_SEARCH == pme->m_wState) ||
          (ST_SEARCH_CMD == pme->m_wState) ||
          (ST_CREDITBACK == pme->m_wState)) &&
         (pme->m_bSuspending || pme->m_bResuming)))
   {
      FreeObj((void **)&pme->m_pText);
   }

   // Free Static Control
   FreeObj((void **)&pme->m_pStatic);

   if(bRestoreState)
   {
      FARF(MS, ("State going from %d -> %d", pme->m_wState, pme->m_wLastState));
      pme->m_wState = pme->m_wLastState; // Set last state as current state
   }

   // Cancel pending timers
   ISHELL_CancelTimer(pme->a.m_pIShell, NULL, pme);

   // Reset softkey
   IMENUCTL_Reset(pme->m_pSK);

   // Reset softkey menu
   pm = pme->m_pMenu;

   // Reset standard menu

   // Save properties and get rid of MP_UNDERLINE_TITLE
   dwProps = IMENUCTL_GetProperties(pm);
   dwProps &= (~MP_UNDERLINE_TITLE);

   // Reset
   IMENUCTL_Reset(pm);

   // No Redraw
   IMENUCTL_SetProperties(pm, dwProps | MP_NO_REDRAW);

   // Inactivate
   IMENUCTL_SetActive(pm, FALSE);

   // Calculate standard menu size
   SETAEERECT(&rc, 0, pme->m_rcMShopLogo.dy, pme->m_cx, pme->m_cy - pme->m_rcMShopLogo.dy);
   IMENUCTL_SetRect(pm, &rc);

   // Restore the properties
#ifdef MP_ALLOW_DUPE_IDS
   IMENUCTL_SetProperties(pm, dwProps|MP_ALLOW_DUPE_IDS);
#else
   IMENUCTL_SetProperties(pm, dwProps);
#endif // MP_ALLOW_DUPE_IDS

   // Note - Free/Unload status image here.  Prevents low-ram conditions...

   FreeObj((void **)&pme->m_pStatus);
   pme->m_bInitStatus = pme->m_bDownloading = FALSE;

   // Clear Display
#ifdef USE_TT_FONTS
   IDISPLAY_FillRect(pme->m_pIDisplay, &rc, RGB_WHITE);
#else
   IDISPLAY_EraseRect(pme->m_pIDisplay, &rc);
#endif // USE_TT_FONTS
}


/*===========================================================================

In-Progress Dialog

===========================================================================*/
static boolean MShop_InProgress(MShop * pme)
{
   // Inactivate both of the menus
   IMENUCTL_SetActive(pme->m_pSK, FALSE);
   IMENUCTL_SetActive(pme->m_pMenu, FALSE);
   
   // Don't Show Busy between this and next state
   pme->m_bShowBusy = FALSE;

   // If current state is anything other than ST_IN_PROGRESS, save that
   // as last state
   if(pme->m_wState != ST_IN_PROGRESS){
      pme->m_wLastState = pme->m_wState;
      FARF(MS, ("State going from %d -> %d", pme->m_wState, ST_IN_PROGRESS));
      pme->m_wState = ST_IN_PROGRESS;
   }
   pme->m_wPct = 0;
   pme->m_bInitStatus = pme->m_bGotStatus = pme->m_bDownloading = FALSE;

   // Start display status animation
#if defined(STATUS_DELAY)
   ISHELL_SetTimer(pme->a.m_pIShell, STATUS_DELAY, (PFNNOTIFY)(MShop_InitStatus), pme);
#else
   MShop_InitStatus(pme);
#endif

   return(TRUE);
}

/*===========================================================================

Async Init of Status Window

===========================================================================*/
static void MShop_InitStatus(MShop * pme)
{
   AEERect        rc;
   AECHAR         szBuff[32];
   IImage *       pi;
   AEEImageInfo   ii;
   int            cyHist, cxStatus;
   boolean        bNoSep;

   // Free Image
   FreeObj((void**)&pme->m_pStatus);

   // Load downloading/connecting image from the resource file
   pi = ISHELL_LoadResImage(pme->a.m_pIShell, BREWAPPMGRLN_RES_FILE, (uint16)(pme->m_bDownloading ? IDB_DOWNLOADING : IDB_CONNECTING));

   if(pi){
      pme->m_pStatus = pi; // Cache Image
      IIMAGE_SetParm(pi, IPARM_NFRAMES, STATUS_ANIMATION_FRAMES, 0); // Set Frames
      IIMAGE_SetParm(pi, IPARM_RATE,STATUS_ANIMATION_RATE,0);        // Set Frame Rate
      IIMAGE_GetInfo(pi, &ii);
   }
   else{
      ii.cxFrame = pme->m_cx / 2;   // Set as half of display area height
      ii.cy = pme->m_cy / 2;        // Set as half of the display area width
   }

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
   if(pme->m_bDispTouchEnabled)
   {
      if(pme->m_wState == ST_IN_PROGRESS)
      {
         uint16 resID[1];
         resID[0] = IDC_CANCEL;
         MShop_CreateSoftmenu(pme,resID, 1);
         MShop_SetMenuRect(pme);
         //if (!pme->m_bHasDPad)
         {
            IMENUCTL_SetActive(pme->m_pSK, FALSE);
         }
      }
   }
#endif

   // Setup status area rect
   cyHist = pme->m_cyFont;

   // Measure "Downloading ..." to set status rectangle width
   ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, IDS_DOWNLOADING, szBuff, sizeof(szBuff));
   cxStatus = IDISPLAY_MeasureText(pme->m_pIDisplay, AEE_FONT_NORMAL, szBuff);

   pme->m_rcStatus.dx = (cxStatus > ii.cxFrame ? cxStatus : ii.cxFrame) + ((AEE_3D_FRAME_SIZE + FRAME_SEP) * 2);
   pme->m_rcStatus.dy = ii.cy + (AEE_3D_FRAME_SIZE + (FRAME_SEP * 2)) + cyHist + (HIST_SEP * 2);

   if(pme->m_rcStatus.dy > pme->m_cy){
      pme->m_rcStatus.dy -= (HIST_SEP * 2);
      bNoSep = TRUE;
   }
   else
      bNoSep = FALSE;

   // Ensure that we can display it in the middle
   pme->m_rcStatus.x = (pme->m_cx - pme->m_rcStatus.dx) / 2;
   pme->m_rcStatus.y = (pme->m_cy - pme->m_rcStatus.dy) / 2;

   // Setup the histogram rect
   pme->m_rcHist.y = pme->m_rcStatus.y + (AEE_3D_FRAME_SIZE + FRAME_SEP) + ii.cy;
   if(!bNoSep)
      pme->m_rcHist.y += HIST_SEP;

   pme->m_rcHist.x = pme->m_rcStatus.x + AEE_3D_FRAME_SIZE + FRAME_SEP;
   pme->m_rcHist.dx = pme->m_rcStatus.dx - 2*(AEE_3D_FRAME_SIZE + FRAME_SEP); 
   pme->m_rcHist.dy = cyHist;       // Same as font height

   ISHELL_CancelTimer(pme->a.m_pIShell, NULL, pme);

   pme->m_bInitStatus = TRUE;
   if (!pme->m_bRedrawingUI)
   {
      pme->m_wPct = 0;
   }

   // Draw frame around status area
   rc = pme->m_rcStatus;
   IDISPLAY_DrawFrame(pme->m_pIDisplay, &rc, AEE_FT_RAISED, CLR_SYS_WIN);

   // Load "Connecting ..." and display that in histogram area.
   ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, (uint16)(pme->m_bDownloading ? (pme->m_wLastState ? (pme->m_bVerify ? IDS_VERIFYING : IDS_DOWNLOADING ) : IDS_CANCELLING): IDS_CONNECTING), szBuff, sizeof(szBuff));

   if(pme->m_bDispTouchEnabled)
   {
      IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_BACKGROUND, CLR_SYS_WIN);
   }

   IDISPLAY_DrawText(pme->m_pIDisplay,AEE_FONT_NORMAL, szBuff, -1, 0, 0, &pme->m_rcHist, IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);

   if(pme->m_bDispTouchEnabled)
   {
      IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_BACKGROUND, TOUCHSCREEN_BACKGROUND);
   }

   // Start image animation
   if(pi)
      IIMAGE_Start(pi, rc.x + ((rc.dx - ii.cxFrame) / 2), rc.y + 1);

}

/*===========================================================================

Auto-Install Response Dialog (General)
Called in response to following events
DEVT_AI_ASK:     Auto-Install Notification (pszText != NULL) 
                 (Yes/No) Must call IDOWNLOAD_Continue()
DEVT_AI_DENY:    Confirmation (Yes/No) Must call IDOWNLOAD_Continue()
DEVT_AI_SUCCESS: Auto-Install Success
DEVT_AI_FAILURE: Auto-Install Failure
===========================================================================*/
static void MShop_AIMessage(MShop * pme, AECHAR * psz,AEEDownloadEvt evt)
{
   uint16         wButtons[3];
   AECHAR *       pMsg = NULL;
   AECHAR *       pTitle = NULL;
   AECHAR *       pText = NULL;

   // Reset UI setting last state as current state
   MShop_Reset(pme,TRUE);

   // Prepare prompt for display
   MEMSET(wButtons, 0, sizeof(wButtons));

   pMsg = WSTRCHR(psz,(AECHAR)'|');
   if(!pMsg)
      pText = psz;
   else{
      *pMsg = 0;
      pText = pMsg + 1;
      pTitle = psz;
   }

   pme->m_bAIPrompt = TRUE;

   if(evt == DEVT_AI_ASK){
      wButtons[0] = IDC_YES;
      wButtons[1] = IDC_NO;
   }
   else
      wButtons[0] = IDC_OK;
   MShop_Prompt(pme, pTitle, 0, pText, 0, wButtons, wButtons, IDC_NO, ST_NOSCROLL);
}

/*===========================================================================

Registed with ADS using IDOWNLOAD_OnStatus(). Gets called on status update.

===========================================================================*/
static boolean MShop_Status(MShop * pme,DownloadStatus * ps)
{
   AEERect  rc;
   boolean  bUpdate = FALSE;
   uint16   wPct;

   if(!ps)
      return(FALSE);

   switch(ps->evt){  
      case DEVT_AI_ASK:          // Auto-Install Notification (pszText != NULL)
      case DEVT_AI_DENY:         // Confirmation (Yes/No) Must call IDOWNLOAD_Continue()
      case DEVT_AI_SUCCESS:      // Auto-Install Success
      case DEVT_AI_FAILURE:      // Auto-Install Failure
         MShop_AIMessage(pme,ps->pszText,ps->evt);
         break;

   // Received an app recall list...

      case DEVT_ADS_RECALL:
         if(ps->pszText)         
            MShop_RecallMessage(pme, ps->pszText);
         break;

   // Received an app verification failed msg...
#ifdef EACKPENDING
      case DEVT_DL_STATUS:
         if(ps->pszText)         
            MShop_VerificationFailedMessage(pme, ps->pszText);
         break;
#endif // EACKPENDING

   // Received an ADS message...

      case DEVT_ADS_MSG:
         if(ps->pszText && pme->m_bCarrierMessage)
            MShop_CarrierMessage(pme, ps->pszText);
         break;

   // Download complete - cleanup status image..

      case DEVT_COMPLETE:
         pme->m_wPct = 100;
         break;

   // Received info during download...

      case DEVT_READ:
         if(pme->m_wState != ST_IN_PROGRESS)
            break;

         // Show downloading image
         if (!pme->m_bDownloading) {
            pme->m_bDownloading = TRUE;
            MShop_InitStatus(pme);
            bUpdate = TRUE;
         }

         if(ps->nBytes || ps->nTotalBytes){
            wPct = ps->wPctComplete;
            if(!pme->m_bGotStatus || wPct != pme->m_wPct){
               rc = pme->m_rcHist;
               pme->m_bGotStatus = TRUE;                 // Set status flag TRUE
               pme->m_wPct = wPct;                       // Update % download complete
               DrawHist(pme->m_pIDisplay, &rc, wPct);  // Draw Histogram with new % complete
               bUpdate = TRUE;                           // Set display update required               
            }
         }
         else{
            if(!pme->m_bGotStatus){
               // Coming here we started reading from ADS in response to download request

               AECHAR   szBuff[32];
               pme->m_bGotStatus = TRUE; // Set status flag TRUE
               bUpdate = TRUE;           // Set display update required

               // Display "Reading..." in histogram area
               ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, IDS_READING, szBuff, sizeof(szBuff));

               if(pme->m_bDispTouchEnabled)
               {
                  IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_BACKGROUND, CLR_SYS_WIN);
               }

               IDISPLAY_DrawText(pme->m_pIDisplay,AEE_FONT_NORMAL, szBuff, -1, 0, 0, &pme->m_rcHist, IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|IDF_RECT_FILL);

               if(pme->m_bDispTouchEnabled)
               {
                  IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_BACKGROUND, TOUCHSCREEN_BACKGROUND);
               }
            }
         }
         if(bUpdate)
            IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);
         break;
   }
   return(TRUE);
}

//===========================================================================
//
// DESCRIPTION
//    Function for determing Menu size when in state ST_VIEW_LOG
// 
// PARAMETERS
//    pme:     MShop*
//    pSKMenu: IMenuCtl*, soft key menu that may be used to figure out Menu size
//    prc:     AEERect*
//
//===========================================================================
static boolean MShop_GetMenuRectForLogDialog(MShop* pme, IMenuCtl* pSKMenu, AEERect* prc)
{
   AEERect rcm;

   if ((NULL == pme) || (NULL == prc))
   {
      return FALSE;
   }

   if (pSKMenu && (IMENUCTL_GetItemCount(pSKMenu) > 0))
   {
      // Get softkey height for setting main menu size
      IMENUCTL_GetRect(pSKMenu, &rcm);

      // Set main menu size
      SETAEERECT(prc, 0, 0, pme->m_cx, rcm.y);
   }
   else
   {
      // If we don't have a menu, take up the whole display
      SETAEERECT(prc, 0, 0, pme->m_cx, pme->m_cy);
   }

   return TRUE;
}

/*===========================================================================

State handler for ST_VIEW_LOG

===========================================================================*/
static boolean MShop_LogDialog(MShop * pme)
{
   AEERect  rc;

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
  if(pme->m_bDispTouchEnabled)
  {
    MShop_AddSoftKey(pme);
  }
  else
#endif
  {
     // Display OK prompt
     IMENUCTL_AddItem(pme->m_pSK, BREWAPPMGRLS_RES_LOC, IDC_OK, IDC_OK, 0, 0);
     IMENUCTL_SetActive(pme->m_pSK, TRUE);
  }

   // Set main menu size
   MShop_GetMenuRectForLogDialog(pme, pme->m_pSK, &rc);
#ifdef USE_TT_FONTS
   IDISPLAY_FillRect(pme->m_pIDisplay, &rc, RGB_WHITE);
#else
   IDISPLAY_EraseRect(pme->m_pIDisplay, &rc);
#endif // USE_TT_FONTS
   IMENUCTL_SetRect(pme->m_pMenu, &rc);

   // Initilize for the enumeration of log information
   if(!IDOWNLOAD_LogEnumInit(pme->m_pDownload)){

      // Allocate Log Buffer
      if(!pme->m_pLogBuff)
         pme->m_pLogBuff = (AECHAR *)MALLOC(MAX_STR_SIZE);

      if(pme->m_pLogBuff){
         pme->m_wLogItemID = IDC_LOG;
         MShop_AddNextLogItem(pme);
      }
   }
   return(TRUE);
}


/*===========================================================================

Cancel timer activated to add items to log menu and free log buffer

===========================================================================*/
static void MShop_FreeLogBuff(MShop * pme)
{
   ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)(MShop_AddNextLogItem), pme);
   FreePtr((void **)&pme->m_pLogBuff);
}
      
/*===========================================================================

Sorts log buffer menu on the basis of ItemID.

===========================================================================*/
static int SortLogCompare(void * pcxt, uint32 dw1, uint32 dw2)
{
   return((int32)dw2 - (int32)dw1);
}

/*===========================================================================

Adds log item in log buffer. If successful, increments the log buffer index
and sets a self timer to add next item in log buffer.

===========================================================================*/
static void MShop_AddNextLogItem(MShop * pme)
{
   AECHAR      szBuff[32],szType[32];
   DLLogItem   li;
   int         i;
   AECHAR *    pName = NULL;
   uint16      wID = 0;

   // Start enumeration of logging information
   if(!IDOWNLOAD_LogEnumNext(pme->m_pDownload, &li)){

      // Coming here means end of the enumeration
      // Free log buffer
      MShop_FreeLogBuff(pme);

      // Sort log menu on the basis of ItemID
      IMENUCTL_SortCustom(pme->m_pMenu,SortLogCompare,pme);

      // Add the Softkey and resize the menu
      if(pme->m_bDispTouchEnabled)
      {
         // Ensure that the menu is the full screen before we
         // re-add soft key.  It is possible for recreate-UI
         // to be called while we are looping through these
         // callbacks and it call AddSoftKey() which
         // may shorten the menu up, but since more items
         // may have come in, we reset to full screen
         // before we re-shrink below
         AEERect     rc;
         MShop_GetMenuRectForLogDialog(pme, pme->m_pSK, &rc);
         IMENUCTL_SetRect(pme->m_pMenu, &rc);

         // Re-add the softkey, which also resizes m_pMenu
         MShop_AddSoftKey(pme);
         IMENUCTL_SetActive(pme->m_pMenu, TRUE);
         IMENUCTL_Redraw(pme->m_pMenu);
         //if (!pme->m_bHasDPad)
         {
            IMENUCTL_SetActive(pme->m_pMenu, FALSE);
         }
      }
      else
      {
         // Activate Menu
         IMENUCTL_SetActive(pme->m_pMenu, TRUE);
      }

      // Update UI
      IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

      return;
   }

   // Coming here means that IDOWNLOAD_LogEnumNext fetched us logging info

   // If log info has downloaded items version number, allocate memory to
   // pName and initilize that with "<item name> (#<version number>)"
   if(GET_DLITEMID_VER(li.iID)){
      STRTOWSTR(" (#%d)",szType,sizeof(szType));
      WSPRINTF(szBuff,sizeof(szBuff),szType,GET_DLITEMID_VER(li.iID));
      i = (WSTRLEN(szBuff) + WSTRLEN(li.szName) + 1) * sizeof(AECHAR);      
      pName = (AECHAR *)MALLOC(i);
      if(pName){
         WSTRCPY(pName,li.szName);
         WSTRCAT(pName,szBuff);
      }
   }
   // If log info doesn't have downloaded items version number, initilize
   // pName with downloaded items name
   if(!pName)
      pName = li.szName;

   // Get date string in a temporary buffer
   GetDateString(pme->a.m_pIShell, li.dwTime, szBuff, sizeof(szBuff));

   // Get action string in another temporary buffer
   switch(li.action){
      case DL_UPGRADE:
         wID = (li.nResult ? IDS_DL_UPGRADE_FAILED : IDS_DL_UPGRADE);
         break;
      case DL_PURCHASE:
         wID = (li.nResult ? IDS_DL_PURCHASE_FAILED : IDS_DL_PURCHASE);
         break;
      case DL_RECALL:
         wID = (li.nResult ? IDS_DL_RECALL_FAILED : IDS_DL_RECALL);
         break;
      case DL_DELETE:
         wID = (li.nResult ? IDS_DL_DELETE_FAILED : IDS_DL_DELETE);
         break;
      case DL_FREEMEMORY:
         wID = (li.nResult ? IDS_DL_DISABLE_FAILED : IDS_DL_DISABLE);
         break;
      case DL_RESTORE:
         wID = (li.nResult ? IDS_DL_RESTORE_FAILED : IDS_DL_RESTORE);
         break;
      case DL_ENDSUBSCRIPTION:
         wID = (li.nResult ? IDS_DL_ENDSSN_FAILED : IDS_DL_ENDSSN);
         break;
   }

   // Load format string "%s: action (%s)"
   ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, wID, szType, sizeof(szType));
   
   // Assign log buffer using format string with name & date strings
   WSPRINTF(pme->m_pLogBuff, MAX_STR_SIZE, szType, pName, szBuff);

   // FREE temporary pName buffer allocated if item has a version
   if(pName != li.szName)
      FREE(pName);

   // Add item in log menu and redraw menu
   IMENUCTL_AddItem(pme->m_pMenu, NULL, pme->m_wLogItemID,pme->m_wLogItemID, pme->m_pLogBuff, li.dwTime);

   pme->m_wLogItemID++; // Increment ItemID for next log buffer item

   // Set a self timer to add next item
   ISHELL_SetTimer(pme->a.m_pIShell, ADD_MENUITEM_TIMER, (PFNNOTIFY)(MShop_AddNextLogItem),pme);
}

/*===========================================================================

Displays Text Input Form shown by Search and Credit Back UI

===========================================================================*/
static boolean MShop_DisplayTextInputForm(MShop * pme, boolean bSearch)
{
   ITextCtl *  pt;
   AEERect     rc, rcm;
   AEEDeviceInfo di; 
        
   ISHELL_GetDeviceInfo(pme->a.m_pIShell,&di);

   // Get Text Control Pointer
   pt = pme->m_pText;

   // Create text control if necessary
   if(!pt) {
      if (CreateInstance(pme->a.m_pIShell, AEECLSID_TEXTCTL,(void **)(&pt)) != AEE_SUCCESS)
         return FALSE;
      else
      {
         // Reset Text Control size so that when we assign some other size, it doesn't
         // erase anything
         SETAEERECT(&rc, 0, 0, 0, 0);
         ITEXTCTL_SetRect(pt, &rc);
      }
   }

   pme->m_pText = pt;

   // Set TextControl buffer size and title
   ITEXTCTL_SetTitle(pt, BREWAPPMGRLS_RES_LOC, (uint16)(bSearch ? IDS_SEARCH_TITLE : IDS_CREDITBACK_TITLE),NULL);                     
   ITEXTCTL_SetMaxSize(pt, MAX_STR_CHARS);

   MShop_SetTitle(pme, BREWAPPMGRLS_RES_LOC, (uint16)(bSearch ? IDC_SEARCH_ANY : IDS_CREDITBACK_TITLE), NULL);

   IMENUCTL_Reset(pme->m_pSK);
   if (bSearch)
   {
      // Setup softkey menu for this text control with any/mode options
      IMENUCTL_AddItem(pme->m_pSK, BREWAPPMGRLS_RES_LOC, IDS_ANY, IDC_SEARCH_ANY, NULL, NULL);
      if (pme->m_bDispTouchEnabled)
      {
         IMENUCTL_AddItem(pme->m_pSK, BREWAPPMGRLS_RES_LOC, IDC_BACK, IDC_BACK, NULL, NULL);
      }
   }
   else
   {
      // Setup softkey menu for this text control with ok/mode options
      IMENUCTL_AddItem(pme->m_pSK, BREWAPPMGRLS_RES_LOC, IDC_OK, IDC_MSHOP_CREDITBACK, NULL, NULL);
   }

   // Get softkey size
   IMENUCTL_GetRect(pme->m_pSK, &rcm);

#if 0   
   if (bSearch)
      // Set Text Control Properties      
      ITEXTCTL_SetProperties(pt, TP_FRAME|TP_MULTILINE|TP_T9_MODE|TP_FOCUS_NOSEL);
   else
      // Set Text Control Properties      
      ITEXTCTL_SetProperties(pt, TP_FRAME|TP_MULTILINE|TP_FOCUS_NOSEL);

   // Set Text Control Size
   SETAEERECT(&rc, MARGIN, pme->m_rcMShopLogo.dy, pme->m_cx - 2*MARGIN, pme->m_cyFont*((pme->m_cy - pme->m_rcMShopLogo.dy - rcm.dy - MARGIN)/pme->m_cyFont));
   ITEXTCTL_SetRect(pt, &rc);
 
   // Set Softkey to Text Control
   ITEXTCTL_SetSoftKeyMenu(pme->m_pText, pme->m_pSK);
#else
   if (bSearch)
   {
      // Set Text Control Properties      
      ITEXTCTL_SetProperties(pt, TP_FRAME|TP_MULTILINE|TP_STARKEY_SWITCH|TP_FOCUS_NOSEL);
   }
   else
   {
      // Set Text Control Properties      
      ITEXTCTL_SetProperties(pt, TP_FRAME|TP_MULTILINE|TP_FOCUS_NOSEL);
   }
   SETAEERECT(&rc, 0, 0, di.cxScreen, di.cyScreen-GetBottomBarHeight(pme->m_pIDisplay));
   ITEXTCTL_SetRect(pt, &rc);
#endif

   // Redraw Softkey
   IMENUCTL_Redraw(pme->m_pSK);

   // If this isn't search, let's make numbers default
   if (!bSearch)
      ITEXTCTL_SetInputMode(pt, AEE_TM_NUMBERS);

   // Make Text Control Active
   ITEXTCTL_SetActive(pt, TRUE);

   // Set repaint flag false to prevent TextControl from updating Title and Banner
   // for every event
   pme->m_bRepaint = FALSE;

   return(TRUE);
}

/*===========================================================================

State handler for ST_SEARCH and ST_SEARCH_CMD

===========================================================================*/
static boolean MShop_SearchDialog(MShop * pme)
{
   return MShop_DisplayTextInputForm(pme, TRUE);
}

#ifdef MSHOP_CMD_SEARCH_STRING
/*===========================================================================

State handler for ST_SEARCHSTRING

===========================================================================*/
static boolean MShop_SearchString(MShop * pme)
{
   MShop_StartSearch(pme);
   return(TRUE);
}
#endif

/*===========================================================================

Performs search using specified criteria

===========================================================================*/
static void MShop_StartSearch(MShop * pme)
{
   AECHAR * psz;

#if defined(MSHOP_CMD_SEARCH_STRING) || defined(MSHOP_CMD_CATEGORY)
   if (*(pme->m_pszSearch))
   {
      psz = pme->m_pszSearch;
   }
   else
#endif
   if(pme->m_pText)
   {
      // Read text control buffer
      psz = ITEXTCTL_GetTextPtr(pme->m_pText);
   }
   else
      return;


   //Duplicate the string 'coz MShop_Reset() frees the original ptr
   if(psz)
      psz = WSTRDUP(psz);

   if(psz){
     // This is ugly because it presumes that a space is the same code in all languages.  A better way to
     // do this might be to use a localized string and match that character or to have an array of space
     // characters from different languages and use WSTRCHR with each character in the input string.
      AECHAR aecBlank = (AECHAR) ' ';
      AECHAR * tmpsz;

     // Reset UI
      MShop_Reset(pme,FALSE);

     //
     // Trim leading and trailing spaces
      //
     for (tmpsz = psz; *tmpsz != 0; ++tmpsz);      // Find the end
     for (--tmpsz; (tmpsz > psz) && (*tmpsz == aecBlank); --tmpsz) *tmpsz = 0; // Nuke trailing spaces
     for (tmpsz = psz; (*tmpsz == aecBlank); ++tmpsz);      // Advance past leading spaces
     //
     // At this point, tmpsz points to first non-space and is trimmed.  Check for an empty search string
     //
     if (*tmpsz == (AECHAR) 0)
     {
         uint16         wButtons[2];

         // Prompt structure with information that app is restored and OK button to dismiss
         MEMSET(wButtons, 0, sizeof(wButtons));
         wButtons[0] = IDC_OK;

         MShop_Prompt(pme, NULL, IDS_SEARCH_TITLE, NULL, IDS_EMPTY_SEARCH, wButtons, wButtons, IDC_OK, ST_NOSCROLL);
     }
     else
     {
      // Display progress praphics
      MShop_InProgress(pme);

      pme->m_nCatIdx = 0;     // Reset category index
      pme->m_nCatItems = 0;   // Reset category items
      pme->m_bSearch = TRUE;  // Set search flag

      // Perform search
      IDOWNLOAD_Search(pme->m_pDownload,tmpsz,(DLSearchType)(pme->m_bSearchAny ? DL_SEARCH_ANY : DL_SEARCH_ALL),MShop_EnumCB,pme);
     }
      FREE(psz);
   }
}

/*===========================================================================

State handler for ST_CREDITBACK

===========================================================================*/
static boolean MShop_CreditBackDialog(MShop * pme)
{
   return MShop_DisplayTextInputForm(pme, FALSE);
}

/*===========================================================================

Credit Back Callback

===========================================================================*/
static void MShop_CreditBackCB(void * pcxt,int nErr, void * pUnused)
{
   MShop *       pme = (MShop *)pcxt;
   uint16         wButtons[2];

   // Reset UI setting last state as current state
   MShop_Reset(pme,TRUE);

   if(nErr){
      // CreditBack failed

      // Do not attempt to open this app when we close down
      MShop_ClearStartArgs(pme);

      // Display failure notice
      MShop_Failed(pme,IDS_CREDITBACK_FAILED,nErr,NULL);
      return;
   }

   // Prompt structure with information that app is restored and OK button to dismiss
   MEMSET(wButtons, 0, sizeof(wButtons));
   wButtons[0] = IDC_OK;

   MShop_Prompt(pme, NULL, IDC_MSHOP_CREDITBACK, NULL, IDS_CREDITBACK_SUCCESSFUL, wButtons, wButtons, IDC_OK, ST_NOSCROLL);
}

/*===========================================================================

Performs credit back using credit back code

===========================================================================*/
static void MShop_StartCreditBack(MShop * pme)
{
   AECHAR * psz;
   char * pCreditCode = NULL;

   if(!pme->m_pText)
      return;

   // Read text control buffer
   psz = ITEXTCTL_GetTextPtr(pme->m_pText);

   if(psz)
   {
      pCreditCode = (char*)MALLOC(MAX_STR_CHARS);
      if (pCreditCode)
         WSTRTOSTR(psz, pCreditCode, MAX_STR_CHARS);
   }

   if(pCreditCode){
      // Reset UI
      MShop_Reset(pme,FALSE);

      // Display progress praphics
      MShop_InProgress(pme);

      // Perform CreditBack
      IDOWNLOAD_Credit(pme->m_pDownload,pCreditCode,MShop_CreditBackCB,pme);
      FREE(pCreditCode);
   }
}





/*===========================================================================

Checks if there is enough EFS and RAM space to download specified application.
Returns TRUE if there is enough space. Displays a single button prompt
otherwise.

===========================================================================*/
static boolean MShop_CheckDownloadToPhone(MShop* pme, DLITEMID iID,
   AECHAR * pszName)
{
   uint64         qwSize,qwFree;
   AECHAR         szFree[10], szSize[10];
#ifdef USE_IDL2_4GB_SUPPORT
   DL2ItemSize     is;
#else
   DLItemSize      is;
#endif
   int            nErr;
   AECHAR *       psz = NULL;
   boolean        bProceed = FALSE;

   MEMSET(&is, 0, sizeof(is));
#ifdef USE_IDL2_4GB_SUPPORT
   nErr = IDOWNLOAD2_GetSizeEx(pme->m_pDownload2, iID, &is);
#else
   nErr = IDOWNLOAD_GetSizeEx(pme->m_pDownload, iID, &is);
#endif

   switch (nErr)
   {
      case SUCCESS:
         return(TRUE);                 // No problem

      case ENOMEMORY:
         {
#ifdef USE_IDL2_4GB_SUPPORT
            qwSize = is.qwEstRAMRequired; // RAM Required
            qwFree = is.qwRAMAvail;       // RAM available
#else
            qwSize = is.dwEstRAMRequired; // RAM Required
            qwFree = is.dwRAMAvail;       // RAM available
#endif
         
            FormatMem(qwSize, szSize, 10, FALSE);  // Get in 5K. 5MB, 5.4MB format
            FormatMem(qwFree, szFree, 10, FALSE);  // Get in 5K. 5MB, 5.4MB format

            // Check if szSize is same as szFree. This is to prevent 
            // the error such as "Your app needs 10K. You only have 10K".
            while (!WSTRICMP(szSize,szFree) && qwFree)
            {
               if (qwFree < ONE_K)
               {
                  qwFree = 0;
               }
               else
               {
                  qwFree -= ONE_K;                // Ensure that Free is less than available
               }
               
               FormatMem(qwFree, szFree, 10, FALSE); // Get new string
            }
            // Get formatted string for free memory
            psz = FormatStringEx(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC,
               IDS_APP_RAM_SIZE, szSize,szFree);
         }
         break;

      case EFSFULL:
         {
            int nCount;
#ifdef USE_IDL2_4GB_SUPPORT
            DL2DisableEntry* pDisableList = NULL;
#else
            DLDisableEntry* pDisableList = NULL;
#endif
            IShell* pIShell = pme->a.m_pIShell;
            AEEAppInfo ai;
            uint32 existingItem = 0;
            uint32 high24Bits = 0xFFFFFF00;  // pronounced "foo"

            // CR 80447.  Lock BAM/MobileShop module during autodisable list
            // creation to prevent IDownload from including it in the list.
            uint32 modID = ISHELL_GetClassItemID(pme->a.m_pIShell,
               AEECLSID_BREWAPPMGR_BID);
            if (0 != modID)
            {
               IDOWNLOAD_Lock(pme->m_pDownload, modID, TRUE);
            }

            // CR 38899.  Search the list of applets to see if this is
            // an upgrade.  If it is we want to lock the existing version
            // to keep it out of the disable list.
            ISHELL_EnumAppletInit(pIShell);
            while (ISHELL_EnumNextApplet(pIShell, &ai))
            {
               AEECLSID cls = ai.cls;
               existingItem = ISHELL_GetClassItemID(pIShell, cls);
               // Items with identical high 24 bits are different versions
               // of the same app
               if ((existingItem & high24Bits) == (iID & high24Bits))
               {
                  // Lock existing item
                  IDOWNLOAD_Lock(pme->m_pDownload, existingItem, TRUE);
                  break;
               }
               else
               {
                  existingItem = 0;
               }
            }

#ifdef USE_IDL2_4GB_SUPPORT
            pDisableList = IDOWNLOAD2_GetAutoDisableList( pme->m_pDownload2,
               iID, 0, &nCount, &nErr);
#else
            pDisableList = IDOWNLOAD_GetAutoDisableList( pme->m_pDownload,
               iID, 0, &nCount, &nErr);
#endif

            // CR 80447.  Unlock BAM/MobileShop module
            if (0 != modID)
            {
               IDOWNLOAD_Lock(pme->m_pDownload, modID, FALSE);
            }
            // CR 38899.  Unlock existing item if it was locked.
            if (0 != existingItem)
            {
               IDOWNLOAD_Lock(pme->m_pDownload, existingItem, FALSE);
            }

            if (nErr)
            {
               // Unable to create space by disabling apps, so we have
               // to put up a "delete stuff and try again" prompt.
#ifdef USE_IDL2_4GB_SUPPORT
               qwSize = is.qwFSInstall;      // File Memory Required
               qwFree = is.qwFSAvail;        // File memory Available
#else
               qwSize = is.dwFSInstall;      // File Memory Required
               qwFree = is.dwFSAvail;        // File memory Available
#endif
               FormatMem(qwSize, szSize, 10, FALSE);  // Get in 5K. 5MB, 5.4MB format
               FormatMem(qwFree, szFree, 10, FALSE);  // Get in 5K. 5MB, 5.4MB format

               // Check if szSize is same as szFree. This is to prevent 
               // the error such as "Your app needs 10K. You only have 10K".
               while (!WSTRICMP(szSize,szFree) && qwFree)
               {
                   qwFree -= ONE_K;  // Ensure that Free is less than available
                   FormatMem(qwFree, szFree, 10, FALSE); // Get new string
               }
               // Get formatted string for free memory
               psz = FormatStringEx(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC,
                  IDS_APP_EFS_SIZE, szSize, szFree);
            }
            else
            {
               int i = 0;
#ifdef USE_IDL2_4GB_SUPPORT
               DL2AppModInfo * pai = NULL;
#else
               AppModInfo * pai = NULL;
#endif
               AECHAR * pApps = NULL;
               AECHAR * pTemp = NULL;

               // If some how space got available, return TRUE;
               if (!nCount)
                  return TRUE;

               // Set bProceed TRUE
               bProceed = TRUE;

               while ((i < nCount) && pDisableList)
               {
                  if (pDisableList->bDisable)
                  {
#ifdef USE_IDL2_4GB_SUPPORT
                     pai = IDOWNLOAD2_GetModInfoEx(pme->m_pDownload2,
                        pDisableList->iID, GMI_ALL);
#else
                     pai = IDOWNLOAD_GetModInfoEx(pme->m_pDownload,
                        pDisableList->iID, GMI_ALL);
#endif
                     if (pai)
                     {
                        pTemp = LoadAppNames(pme->a.m_pIShell, pai->apps,
                           pai->nApps);
                     }
                     if (pTemp)
                     {
                        pApps = AppendWStr(pApps, pTemp, ',');
                        FREE(pTemp);
                     }
                  }
                  pDisableList++;
                  i++;
               }

               // Get formatted string for free memory
               if (pApps)
               {
                  psz = FormatString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC,
                     IDS_FILE_MEMORY_FULL, pApps);
                  FREE(pApps);
               }
            }
         }
         break;
   
      case EOUTOFNODES:
         {            
            AECHAR szFormat[10];
            qwSize = is.nEstFilesRequired;// Files Required
            qwFree = is.nFilesAvail;      // Files Available
            STRTOWSTR("%d",szFormat,sizeof(szFormat));
            WSPRINTF(szFree,sizeof(szFree),szFormat,qwFree);// Get #count string
            WSPRINTF(szSize,sizeof(szSize),szFormat,qwSize);// Get #count string
            // Get formatted string for free files
            psz = FormatStringEx(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC,
               IDS_APP_FILES_NEEDED, szSize, szFree);
         }
         break;

      default:
         return(FALSE);                // Can't download
   }
   
   if (psz)
   {
      // If proceed is TRUE we have a list of apps to disable so we want a cancel button,
      // otherwise the OK button is fine.
      if (!bProceed)
      {
         // Display a single button prompt

         uint16 wButtons[2];
         MEMSET(wButtons, 0, sizeof(wButtons));
         wButtons[0] = IDC_OK;
         MShop_Reset(pme,FALSE);

         MShop_Prompt(pme, pszName, 0, psz, 0, wButtons, wButtons, IDC_OK,
            ST_NOSCROLL);
      }
      else
      {
         uint16 wItemIDs[3];
         uint16 wButtons[3];
         
         MEMSET(wButtons, 0, sizeof(wButtons));
         MEMSET(wItemIDs, 0, sizeof(wItemIDs));
         
         if (pme->m_bDispTouchEnabled)
         {
            wButtons[0] = IDC_OK;
            wButtons[1] = IDC_CANCEL;
            wItemIDs[0] = IDC_PROCEED;
            wItemIDs[1] = IDC_CANCEL;
         }
         else
         {
            wButtons[0] = IDC_OK;
            wItemIDs[0] = IDC_PROCEED;
         }

         MShop_Reset(pme,FALSE);

         MShop_Prompt(pme, pszName, 0, psz, 0, wButtons, wItemIDs,
            IDC_PROCEED, ST_NOSCROLL);
      }
   }

   FreePtr((void**)&psz);

   return(FALSE);
}




 
/*===========================================================================

Sets selected server in server menu as server to be communicated with.

===========================================================================*/
static void MShop_SetServer(MShop * pme, uint16 wID)
{
   ADSInfoEntry * pe;

   // Get item data corresponding to selected item in server menu
   IMENUCTL_GetItemData(pme->m_pMenu, wID, (uint32 *)&pe);

   if(pe)
      IDOWNLOAD_SetADS(pme->m_pDownload, &pe->s); // Set Server

   MShop_ForceExit(pme);
}


//===========================================================================
//
// DESCRIPTION
//    Function for determing Menu size when in state ST_SERVER
// 
// PARAMETERS
//    pme:     MShop*
//    prc:     AEERect*
//
//===========================================================================
static boolean MShop_GetMenuRectForSetServer(MShop* pme, AEERect* prc)
{
   if ((NULL == pme) || (NULL == prc))
   {
      return FALSE;
   }

   SETAEERECT(prc, 0, 0, pme->m_cx, pme->m_cy);

   return TRUE;
}

/*===========================================================================

State handler for ST_SERVER

===========================================================================*/
static boolean MShop_SetServerDialog(MShop * pme)
{
   int               i,nIdx = 0;
   ADSInfoEntry   *  pe;
   uint16            nSel;
   uint16            wID = IDC_SERVER;
   int               nCount;
   AEERect           rc;
   CtlAddItem        ai;

   // Get server list, server count and index of current server
   if ((pe = IDOWNLOAD_GetADSList(pme->m_pDownload,&nCount,&nIdx)) == NULL)
   {
      uint16 wButtons[2];

      MEMSET(wButtons, 0, sizeof(wButtons));
      wButtons[0] = IDC_OK;
         
      // Display the text "No Items were found" as there was nothing in the 
      // dlservers.dat file (or it did not exist).
      MShop_Prompt(pme, NULL, 0, NULL, IDS_NO_CATEGORIES, wButtons, wButtons, wButtons[0], ST_NOSCROLL);

      return(FALSE);
   }

   // Selection
   nSel = IDC_SERVER + nIdx;

   // Reset UI
   MShop_Reset(pme,FALSE);

   // Set Title
   IMENUCTL_SetTitle(pme->m_pMenu, BREWAPPMGRLS_RES_LOC, IDS_SELECT, NULL);
   // Store the title info for use with flipping the cover
   MShop_StoreMenuTitleInfo(pme, BREWAPPMGRLS_RES_LOC, IDS_SELECT, NULL);

   // Initialize CtlAddItem
   MEMSET(&ai, 0, sizeof(ai));
   ai.pszResImage = BREWAPPMGRLN_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;

   // Set main menu size to entire display area
   MShop_GetMenuRectForSetServer(pme, &rc);
   IMENUCTL_SetRect(pme->m_pMenu, &rc);

   // Add server names in menu
   for(i = 0; nCount; i++, nCount--, pe++)
   {
      ai.pText = pe->szName;
      ai.wImage = (wID == nSel) ? IDB_RADIO_SEL : IDB_RADIO_UNSEL;
      ai.wItemID = wID++;
      ai.dwData = (uint32)pe;
      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   }

   MShop_AddSoftKey(pme);
         
   // Set index of current server as selection
   IMENUCTL_SetSel(pme->m_pMenu, nSel);
               
   // Activate Menu
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

   if (pme->m_bDispTouchEnabled/* && !pme->m_bHasDPad*/)
   {
      IMENUCTL_SetActive(pme->m_pMenu, FALSE);
   }

   return(TRUE);
}

/*==========================================================================

Callback for IDOWNLOAD_GetItemInfo() called for DLITEMID

===========================================================================*/
static void MShop_ItemInfoCB(void * pcxt, int nErr, DLEnumItem * pItem)
{
   MShop * pme = (MShop *)pcxt;

   // Display failure if error or got no info or item is a category
   if(nErr || !pItem || pItem->t == DLI_CATEGORY)
   {
      if (pItem && pItem->t == DLI_CATEGORY)
      {
         MShop_Reset(pme,TRUE);
         MShop_Failed(pme, IDS_APP_NOT_FOUND,nErr,NULL);
         return;
      }
      else
      {
         // Let's check for upgrade as its possible that app got upgraded on server
         MShop_Reset(pme,TRUE);
         pme->m_bUpgradeCheck = TRUE;
         pme->m_nCatIdx = 0;
         pme->m_catList[pme->m_nCatIdx].iID = pme->m_iApp;
         MShop_SetState(pme, ST_CATEGORIES);
         return;
      }
   }

   // Free cached download item, if any
   if (NULL != pme->m_pItem)
   {
      FreeDLItem(pme->m_pItem);
   }
   // Cache current download item
   pme->m_pItem = CopyDLItem((DLItem*)pItem);
   if (NULL == pme->m_pItem)
   {
      FARF(MS, ("CopyDLItem() failed"));
      MShop_Reset(pme,TRUE);
      MShop_Failed(pme, IDS_MSHOP_OUT_OF_MEMORY, ENOMEMORY, NULL);
   }
   pme->m_iApp = pItem->id;               // Set as current app ID
   MShop_SetState(pme, ST_APP_OPTIONS);   // Display App Options
}

/*==========================================================================

Callback for IDOWNLOAD_GetItemInfo() called for cancellation of subscription
application.

===========================================================================*/
static void MShop_RemoveItemInfoCB(void * pcxt, int nErr, DLEnumItem * pItem)
{
   MShop * pme = (MShop *)pcxt;

   // Display failure if error or got no info or item is a category
   if(nErr || !pItem){
      MShop_Reset(pme,TRUE);
      MShop_Failed(pme, IDS_SUBSCRIPTION_CANCEL_LATER, 0, NULL);
      return;
   }

   // Exit Otherwise
   MShop_ForceExit(pme);
}

/*==========================================================================

Callback for IDOWNLOAD_GetItemInfo() called while resuming

===========================================================================*/
static void MShop_ItemInfoResumeCB(void * pcxt, int nErr, DLEnumItem * pItem)
{
   MShop * pme = (MShop *)pcxt;

   // Display failure if error or got no info or item is a category
   if(nErr || !pItem || pItem->t == DLI_CATEGORY)
   {
      // we are finished doing anything that migh need to be kept around
      // from the suspend/resume. so set it to FALSE so we can cleanly
      // go to the failover state.
      pme->m_bResuming = FALSE;
      if (pItem && pItem->t == DLI_CATEGORY)
      {
         MShop_Reset(pme,TRUE);
         MShop_Failed(pme, IDS_APP_NOT_FOUND,nErr,NULL);         
         return;
      }
      else
      {
         // Let's check for upgrade as its possible that app got upgraded on server
         MShop_Reset(pme,TRUE);
         pme->m_bUpgradeCheck = TRUE;
         pme->m_nCatIdx = 0;
         pme->m_catList[pme->m_nCatIdx].iID = pme->m_iApp;
         MShop_SetState(pme, ST_CATEGORIES);
         return;
      }
   }

   // Free cached download item, if any
   if (NULL != pme->m_pItem)
   {
      FreeDLItem(pme->m_pItem);
   }
   // Cache current download item
   pme->m_pItem = CopyDLItem((DLItem*)pItem);
   if (NULL == pme->m_pItem)
   {
      FARF(MS, ("CopyDLItem() failed"));
      MShop_Reset(pme,TRUE);
      MShop_Failed(pme, IDS_MSHOP_OUT_OF_MEMORY, ENOMEMORY, NULL);
   }
   pme->m_iApp = pItem->id;

   switch (pme->m_wState)
   {
   case ST_IN_PROGRESS:
      // If we are not in one of these states, just restart the
      // state we were in that was "progressing"  This most likely
      // is Catalog (trying to go to one more level deaper)
      // or connecting originally.
      if ((ST_PURCHASE != pme->m_wLastState) &&
          (ST_PURCHASE_EULA != pme->m_wLastState) &&
          (ST_PURCHASE_CONFIRM != pme->m_wLastState))
      {
         MShop_SetState(pme, pme->m_wLastState);
         break;
      }
      // fall through
   case ST_PURCHASE_CONFIRM:
   case ST_PURCHASE_EULA:
   case ST_PURCHASE:
      // We have gotten too far to resume at that state
      // resume in the ST_PURCHASE_RESUME state that will
      // pop up a notice telling them that they have not been
      // charged for the download, then takes them back to the
      // Check Space state to allow them to restart downloading.
      MShop_SetState(pme, ST_PURCHASE_RESUME);
      break;

   default:
      MShop_SetState(pme, pme->m_wState);   // Redraw the current state
      break;
   }

   pme->m_bResuming = FALSE;
}



/*==========================================================================

Callback for IDOWNLOAD_GetItemInfo() called for DLITEMID as a result of an 
cmshop:Acquire command.

===========================================================================*/
#ifdef MSHOP_CMD_ACQUIRE
static void MShop_AcquireItemInfoCB(void * pcxt, int nErr, DLEnumItem * pItem)
{
   MShop * pme = (MShop *)pcxt;
   Price * p = NULL;

   // Display failure if error or got no info or item is a category
   if(nErr || !pItem || pItem->t == DLI_CATEGORY)
   {

      MShop_Reset(pme,TRUE);
      MShop_Failed(pme, IDS_APP_NOT_FOUND,nErr,NULL);
      return;
   }

   // Free cached download item, if any
   if (NULL != pme->m_pItem)
   {
      FreeDLItem(pme->m_pItem);
   }
   // Cache current download item
   pme->m_pItem = CopyDLItem((DLItem*)pItem);
   if (NULL == pme->m_pItem)
   {
      FARF(MS, ("CopyDLItem() failed"));
      MShop_Reset(pme,TRUE);
      MShop_Failed(pme, IDS_MSHOP_OUT_OF_MEMORY, ENOMEMORY, NULL);
      return;
   }

   // turn on force conformation
   pme->m_bConfirmDownload = TRUE;

   // If the user cancels we need to exit MobileShop.
   pme->m_bExitOnCancel = TRUE;

   // Look through all of the "options" and try to find our Purchase Options
   // The overall logic of how to look for things is the same as what is
   // done in MShop_AppOptionsDialog

   // If there is no upgrade option and demo option is available,
      // add that in menu
   if (pme->m_pItem->UpgradeOption.lt == LT_NONE)
   {
      if(pme->m_pItem->DemoOption.lt != LT_NONE)
      {
         if (pme->m_pItem->DemoOption.Price.dwHandle == pme->m_pAcquireInfo->priceHandle)
         {
            if (NULL != pme->m_pPrice)
            {
               FreePrice(&pme->m_pPrice);
            }
            pme->m_pPrice = CopyPrice(&pme->m_pItem->DemoOption.Price);
            pme->m_bDemo = TRUE;
            pme->m_bSubscription = FALSE;
            pme->m_iApp = pme->m_pItem->iID;       // Set as current app ID
            MShop_SetState(pme, ST_CHECK_SPACE);   // Display App Options
            return;
         }
      } 
   }

   // If upgrade option is available, add that in menu
   if (pme->m_pItem->UpgradeOption.lt != LT_NONE)
   {
      if (pme->m_pItem->UpgradeOption.Price.dwHandle == pme->m_pAcquireInfo->priceHandle)
      {
         if (NULL != pme->m_pPrice)
         {
            FreePrice(&pme->m_pPrice);
         }
         pme->m_pPrice = CopyPrice(&pme->m_pItem->UpgradeOption.Price);
         pme->m_bDemo = FALSE;
         pme->m_bSubscription = FALSE;
         pme->m_iApp = pme->m_pItem->iID;       // Set as current app ID
         MShop_SetState(pme, ST_CHECK_SPACE);   // Display App Options
         return;
      }
   }

   // If subscription option is available, add that in menu
   if (pme->m_pItem->SubscriptionOption.lt != LT_NONE)
   {
      if (pme->m_pItem->SubscriptionOption.Price.dwHandle == pme->m_pAcquireInfo->priceHandle)
      {
         if (NULL != pme->m_pPrice)
         {
            FreePrice(&pme->m_pPrice);
         }
         pme->m_pPrice = CopyPrice(&pme->m_pItem->SubscriptionOption.Price);
         pme->m_bDemo = FALSE;
         pme->m_bSubscription = TRUE;
         pme->m_iApp = pme->m_pItem->iID;       // Set as current app ID
         MShop_SetState(pme, ST_CHECK_SPACE);   // Display App Options
         return;
      }
   }

   // Look for our PriceOption then Set All the Vars as if the user had
   // selected the PriceOption from the AppOptions dialog
   if (pme->m_pItem->PurchaseOption.lt != LT_NONE)
   {
      int i;

      for (i = 0, p = pme->m_pItem->PurchaseOption.pPrices;
         i < pme->m_pItem->PurchaseOption.nPrices; i++, p++)
      {
         if (p->dwHandle == pme->m_pAcquireInfo->priceHandle)
         {
            if (NULL != pme->m_pPrice)
            {
               FreePrice(&pme->m_pPrice);
            }
            pme->m_pPrice = CopyPrice(p);
            pme->m_bDemo = FALSE;
            pme->m_bSubscription = FALSE;
            pme->m_iApp = pme->m_pItem->iID;       // Set as current app ID
            MShop_SetState(pme, ST_CHECK_SPACE);   // Display App Options
            return;
         }
      }
   }

   // We have not found what we were suppose to Acquire
   // so Fail, but as the user did not requst the "Acquire"
   // just Exit MShop.

   MShop_Reset(pme,TRUE);
   MShop_ForceExit(pme);
}
#endif // MSHOP_CMD_ACQUIRE

/*===========================================================================

  MobileShop URL Format

  cmshop:ItemID=<ItemID>
  cmshop:Search
  cmshop:Search=<Search String>
  cmshop:Restore=<ItemID>
  cmshop:SilentUpgradeCheck=<ItemId>&url=myapp:UpgradeAvailable=%d
  cmshop:ItemInfo=<ItemID>

===========================================================================*/
static void MShop_Browse(MShop * pme, const char * url)
{
   DLITEMID       iID                     = 0;
   char*          pszTemp                 = NULL;
   char*          pszArg                  = NULL;
   char*          pszVal                  = NULL;
   char*          pszArg2                 = NULL;
   char*          pszVal2                 = NULL;
   char*          pszNext                 = NULL;
 
   // Ignore mshop commands if we are already running
   FARF(MS, ("Our current state is %d while in Browse URL", pme->m_wState));
   if (pme->m_wState != NO_ENTRY)
   {
      return;
   }

   // No URL - Nothing to do
   if(!url)
   {
      MShop_ForceExit(pme);
      return;
   }

   FARF(MS, ("Our Browse URL is %s", url));

   // Duplicate the URL as we will modify it
   pszTemp = STRDUP(url);
   if(!pszTemp)
   {
      FARF(MS, ("Unable to dup the incoming string"));
      MShop_ForceExit(pme);
      return;
   }

   pszArg = STRCHR(pszTemp, CMSHOP_CMD_COLON); // Get URL devoid of cmshop
   pszArg++; // Ignore ':', '0'

   // Test cmshop command validity for various security macros
#ifdef CMSHOP_SECURE_ALL
   if (!MShop_ValidateCmshopCmd(pme, pszArg)) {
      FREEIF(pszTemp);
      MShop_ForceExit(pme);
      return; // Failed security check - do nothing
   }
#elif defined CMSHOP_SECURE_REMOVE
   if (!STRNCMP(pszArg, CMSHOP_CMD_REMOVE, STRLEN(CMSHOP_CMD_REMOVE))) {
      if (!MShop_ValidateCmshopCmd(pme, pszArg)) {
         FREEIF(pszTemp);
         MShop_ForceExit(pme);
         return; // Failed security check - do nothing
      }
   }
#endif //CMSHOP_SECURE_REMOVE 

   // Process all of the arguments
   while(pszArg){
      //Multiple commands not used in BrewAppMgr.c, but we'll preserve ability to handle them:
      pszNext = STRCHR(pszArg, CMSHOP_CMD_SEMICOLON); // Get next in ';' separated list

      if (pszNext) {
         *pszNext = 0;  // Overwrite ';' with NULL leaving pszArg as NULL terminated 
                        // single entry string "<action>=<value>" string 
      }

      // See if there is a second argument
      pszArg2 = STRCHR(pszArg,CMSHOP_CMD_AMPERSAND);
      if (pszArg2) {
         *pszArg2 = 0;
         pszArg2++;
      }

      // Look for value to arg 1
      pszVal = STRCHR(pszArg, CMSHOP_CMD_EQUAL);  // Get value
      if(pszVal){
         *pszVal = 0;               // Overwrite '=' leaving pszArg as NULL terminated "<action>" string
         pszVal++;                  // Ignore '='
      }

      // Look for value to arg 2
      if (pszArg2)
      {
         pszVal2 = STRCHR(pszArg2,CMSHOP_CMD_EQUAL);  // Get value
         if(pszVal2)
         {
            *pszVal2 = 0;               // Overwrite '=' leaving pszArg as NULL terminated "<action>" string
            pszVal2++;                  // Ignore '='
         }
      }

      FARF(MS, ("pszArg is %s", (NULL == pszArg) ? NULL : pszArg));
      FARF(MS, ("pszArg2 is %s", (NULL == pszArg2) ? NULL : pszArg2));
      FARF(MS, ("pszVal is %s", (NULL == pszVal) ? NULL : pszVal));
      FARF(MS, ("pszVal2 is %s", (NULL == pszVal2) ? NULL : pszVal2));

      if((!STRICMP(pszArg, CMSHOP_CMD_ITEMID))
#ifdef MSHOP_CMD_ITEMINFO
         || (!STRICMP(pszArg, CMSHOP_CMD_ITEMINFO))
#endif
         ) {
         FARF(MS, ("Processing Command %s", pszArg));
         
         // Try to convert the DLITEMID using base 10
         // if that does not work, try hex
         iID = STRTOUL(pszVal,NULL,10);
         if (0 == iID)
         {
            iID = STRTOUL(pszVal,NULL,16);
         }
         pme->m_iApp = iID;
         MShop_Reset(pme,FALSE);               // Reset UI
         MShop_InProgress(pme);                // Display progress graphics
#ifdef MSHOP_CMD_ITEMINFO
         pme->m_bDisplayInfoOnly = !STRICMP(pszArg, CMSHOP_CMD_ITEMINFO);             // Only display item info
#endif
         IDOWNLOAD_GetItemInfo(pme->m_pDownload, iID, MShop_ItemInfoCB,pme);          // Get item info
      }
      else if (!STRICMP(pszArg, CMSHOP_CMD_RESTORE)) {
         FARF(MS, ("Processing Command %s", pszArg));

         // Try to convert the DLITEMID using base 10
         // if that does not work, try hex
         iID = STRTOUL(pszVal,NULL,10);
         if (0 == iID)
         {
            iID = STRTOUL(pszVal,NULL,16);
         }
         pme->m_iApp = iID;
         MShop_SetState(pme, ST_RESTORE_APP);     // Restore App
      }
      else if (!STRICMP(pszArg, CMSHOP_CMD_CATALOG)) {
         FARF(MS, ("Processing Command %s", pszArg));
         pme->m_bCarrierMessage = TRUE;
         MShop_SetState(pme, ST_CATEGORIES);   // Display Catalog
      }
      else if (!STRICMP(pszArg, CMSHOP_CMD_SEARCH)) {
#ifdef MSHOP_CMD_SEARCH_STRING
         FARF(MS, ("Processing Command %s", pszArg));
         if (pszVal)
         {
            STRTOWSTR(pszVal, pme->m_pszSearch, MAX_STR_SIZE);
            pme->m_bSearchAny = TRUE;
            MShop_SetState(pme, ST_SEARCHSTRING);
         }
         else
#endif
         {
            MShop_SetState(pme, ST_SEARCH_CMD);       // Provide search option
         }
      }
      else if (!STRICMP(pszArg, CMSHOP_CMD_CREDITBACK)) {
         FARF(MS, ("Processing Command %s", pszArg));
         MShop_SetState(pme, ST_CREDITBACK);   // Provide Credit Back Dialog
      }
      else if (!STRICMP(pszArg, CMSHOP_CMD_LOG)) {
         FARF(MS, ("Processing Command %s", pszArg));
         MShop_SetState(pme, ST_VIEW_LOG);     // Provide log view dialog
      }
      else if (!STRICMP(pszArg, CMSHOP_CMD_SERVER)) {
         FARF(MS, ("Processing Command %s", pszArg));
         MShop_SetState(pme, ST_SERVER);       // Provide "set server" dialog
      }
      else if (!STRICMP(pszArg, CMSHOP_CMD_REMOVE)) {
         FARF(MS, ("Processing Command %s", pszArg));
         MShop_BrowseRemove(pme, pszVal);      // Call helper function for remove
      }
      else if (!STRICMP(pszArg, CMSHOP_CMD_UPGRADECHECK)) {
         FARF(MS, ("Processing Command %s", pszArg));
         // Try to convert the DLITEMID using base 10
         // if that does not work, try hex
         iID = STRTOUL(pszVal,NULL,10);
         if (0 == iID)
         {
            iID = STRTOUL(pszVal,NULL,16);
         }

         // Prepare for catalog enumeration
         pme->m_bUpgradeCheck = TRUE;
         pme->m_nCatIdx = 0;
         pme->m_catList[pme->m_nCatIdx].iID = iID;
         MShop_SetState(pme, ST_CATEGORIES);
      }
      else if (!STRICMP(pszArg, CMSHOP_CMD_VERIFY)) {
         FARF(MS, ("Processing Command %s", pszArg));

         // Try to convert the DLITEMID using base 10
         // if that does not work, try hex
         iID = STRTOUL(pszVal,NULL,10);
         if (0 == iID)
         {
            iID = STRTOUL(pszVal,NULL,16);
         }
         pme->m_iApp = iID;
         pme->m_bVerify = TRUE;
         MShop_SetState(pme, ST_CATEGORIES);   // Display Catalog
      }
#ifdef FEATURE_SILENT_UPGRADE_CHECK
      else if(!STRICMP(pszArg,CMSHOP_CMD_SILENTUPGRADECHECK))
      {         
         //SilentUpgradeCheckInfo * pSilent = NULL;

         FARF(MS, ("Processing Command %s", pszArg));

         if (pszVal && pszVal2) 
         {
            int bufSize2 = STRLEN(pszVal2) + 1;
            char * pszBuf;

            //pSilent = (SilentUpgradeCheckInfo *) MALLOC( sizeof(SilentUpgradeCheckInfo));
            pszBuf = (char *) MALLOC (bufSize2);

            if ((pme != NULL) && (pszBuf != NULL))
            {
               // Try to convert the DLITEMID using base 10
               // if that does not work, try hex
               pme->iID = STRTOUL(pszVal,NULL,10);
               if (0 == pme->iID)
               {
                  pme->iID = STRTOUL(pszVal,NULL,16);
               }
               pme->pszURL = pszBuf;
               //CreateInstance(pme->a.m_pIShell, AEECLSID_SHELL, (void **) &pSilent->pIShell);
               STRCPY(pszBuf, pszVal2);
               MShop_doSilentUpgradeCheck(pme/*, pSilent*/);
            }
            else 
            {
               // What do you do when there is a MALLOC failure?
            }
         }
         else 
         {
            // What do you do when there is an incorrectly formatted request?
            FARF(MS, ("Ignoring incorrectly formatted request"));
            MShop_ForceExit(pme);
         }
      }
#endif

#ifdef MSHOP_CMD_CATEGORY
      else if (!STRICMP(pszArg,CMSHOP_CMD_CATEGORY))
      {
         FARF(MS, ("Processing Command %s", pszArg));
         if (pszVal && *pszVal)
         {
            // If there is a second parameter/argument then use it for the title.  Otherwise use the category name.
            if (!pszVal2 || !(*pszVal2))
            {
               FARF(MS, ("We are defaulting the title to %s", pszVal));
               pszVal2 = pszVal;
            }
            else
            {
               FARF(MS, ("We were given the title %s", pszVal2));
            }
            STRTOWSTR(pszVal, pme->m_pszSearch, MAX_STR_SIZE);
            STRTOWSTR(pszVal2, pme->m_pszTitle, MAX_STR_SIZE);
            pme->m_bSearchAny = FALSE;       // Searching ALL
            MShop_SetState(pme, ST_SEARCHSTRING);
         }
         else
         {
            FARF(MS, ("%s needs more info to process", pszArg));
         }
      }
#endif

#if defined(MSHOP_CMD_CATALOGID) || defined(MSHOP_CMD_CATEGORYID)
#ifdef MSHOP_CMD_CATALOGID
      else if (!STRICMP(pszArg, CMSHOP_CMD_CATALOGID)
#ifdef MSHOP_CMD_CATEGORYID
         || !STRICMP(pszArg, CMSHOP_CMD_CATEGORYID)
#endif // MSHOP_CMD_CATEGORYID
#else // !MSHOP_CMD_CATALOGID
      else if (!STRICMP(pszArg, CMSHOP_CMD_CATEGORYID)
#endif // MSHOP_CMD_CATALOGID
            ) {
         FARF(MS, ("Processing Command %s", pszArg));

         // Try to convert the DLITEMID using base 10
         // if that does not work, try hex
         iID = STRTOUL(pszVal,NULL,10);
         if (0 == iID)
         {
            iID = STRTOUL(pszVal,NULL,16);
         }

         pme->m_bCarrierMessage = TRUE;
         pme->m_nCatIdx = 0;
         pme->m_catList[0].iID = iID;
         if (pszVal2 && *pszVal2)
         {
            int bufSize = (STRLEN(pszVal2) + 1) * sizeof(AECHAR);
            FARF(MS, ("We were given the cat name %s", pszVal2));
            pme->m_catList[0].pszName = MALLOC(bufSize);
            if (NULL != pme->m_catList[0].pszName)
            {
               MEMSET(pme->m_catList[0].pszName, '\0', bufSize);
               STRTOWSTR(pszVal2, pme->m_catList[0].pszName, bufSize);
            }
         }

         
         MShop_SetState(pme, ST_CATEGORIES);   // Display Catalog
      }
#endif // defined(MSHOP_CMD_CATALOGID) || defined(MSHOP_CMD_CATEGORYID)

#ifdef MSHOP_CMD_ACQUIRE
      else if (!STRICMP(pszArg, CMSHOP_CMD_ACQUIRE))
      {
         AcquireInfo* pAcquireInfo = NULL;

         FARF(MS, ("Processing Command %s", pszArg));

         if (pszVal && pszVal2)
         {

            pAcquireInfo = (AcquireInfo*)MALLOC(sizeof(AcquireInfo));

            if (NULL != pAcquireInfo)
            {
               MEMSET(pAcquireInfo, 0, sizeof(AcquireInfo));
               pme->m_pAcquireInfo = pAcquireInfo;
               // Try to convert the DLITEMID using base 10
               // if that does not work, try hex
               iID = STRTOUL(pszVal,NULL,10);
               if (0 == iID)
               {
                  iID = STRTOUL(pszVal,NULL,16);
               }

               pAcquireInfo->iID = iID;

               // Try to convert the price handle using base 10
               // if that does not work, try hex
               pAcquireInfo->priceHandle = STRTOUL(pszVal2, NULL, 10);
               if (0 == pAcquireInfo->priceHandle)
               {
                  pAcquireInfo->priceHandle = STRTOUL(pszVal2, NULL, 16);
               }

               pme->m_bCarrierMessage = TRUE;
               pme->m_pAcquireInfo = pAcquireInfo;

               MShop_Reset(pme,FALSE);               // Reset UI
               MShop_InProgress(pme);                // Display progress graphics
               IDOWNLOAD_GetItemInfo(pme->m_pDownload, iID, MShop_AcquireItemInfoCB, pme); // Get item info
            }
            else
            {
               // What to do if Fail to Malloc memory?
            }
         }
         else
         {
            // What to do if we don't have the proper parameters?
            FARF(MS, ("Ignoring incorrectly formatted request"));
            MShop_ForceExit(pme);
         }
      }
#endif // MSHOP_CMD_ACQUIRE
      // If the cmshop command is invalid, just exit Mobileshop. This will prevent Mshop starting and 
      // having a bad user experience.
      else
      {
         FARF(MS, ("we received an invalid cmshop command '%s', exiting", pszArg));
         FREEIF(pszTemp);
         MShop_ForceExit(pme);
      }

      pszArg = pszNext;
   }
   FREEIF(pszTemp);
}
 
/*===========================================================================

  MobileShop_Browse() helper function for remove functionality

===========================================================================*/
static void MShop_BrowseRemove(MShop* pme, const char* pszApp) {
#ifdef USE_IDL2_4GB_SUPPORT
   DL2AppModInfo* pmi                     = NULL;
#else
   AppModInfo* pmi                     = NULL;
#endif
   AEELicenseType lt                      = 0;
   DLITEMID       iID                     = 0;
   uint32         bReturn                 = AEE_SUCCESS;
   boolean        hasShortcut             = FALSE;

   if (pszApp == NULL)
   {
      uint16 wButtons[2];

      MEMSET(wButtons, 0, sizeof(wButtons));
      wButtons[0] = IDC_OK;
         
      // Display single button prompt, no app was specified thus none was found
      MShop_Prompt(pme, NULL, 0, NULL, IDS_APP_NOT_FOUND, wButtons, wButtons, wButtons[0], ST_NOSCROLL);

      return;
   }

   // Try to convert the DLITEMID using base 10
   // if that does not work, try hex
   iID = STRTOUL(pszApp,NULL,10);
   if (0 == iID)
   {
      iID = STRTOUL(pszApp,NULL,16);
   }

#ifdef USE_IDL2_4GB_SUPPORT
   pmi = IDOWNLOAD2_GetModInfoEx(pme->m_pDownload2, iID, GMI_ALL);
#else
   pmi = IDOWNLOAD_GetModInfoEx(pme->m_pDownload, iID, GMI_ALL);
#endif
   lt = (pmi ? pmi->li.pt : PT_NONE);

#ifdef FEATURE_INTERACT_WITH_SA_APP
   if (pmi)
   {
      uint16 index;
      for (index = 0; index < pmi->nApps && !hasShortcut; index++)
      {
         hasShortcut = DoesAppHaveShortcut(pme->m_pIfm, pmi->apps[index].cls);
      }
   }
#endif // FEATURE_INTERACT_WITH_SA_APP

   bReturn = IDOWNLOAD_Delete(pme->m_pDownload, iID, TRUE);
   if (bReturn == AEE_SUCCESS)
   {

      ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_BREWAPPMGR_BID, BAM_EVT_APP_DELETED, 0, 0);

      // If it is not a subscription app, just exit MShop.
      if(lt != PT_SUBSCRIPTION)
      {
         MShop_ForceExit(pme);
      }
      else
      {
         // For subscription app, let's check that we can connect to the server
         MShop_InProgress(pme);                // Display progress graphics
         IDOWNLOAD_GetItemInfo(pme->m_pDownload, iID, MShop_RemoveItemInfoCB, pme);
      }
   }
   else
   {
      if (bReturn != EBADSID)
         MShop_ForceExit(pme);
      else
      {
         uint16 wButtons[2];

         MEMSET(wButtons, 0, sizeof(wButtons));

         wButtons[0] = IDC_OK;
         
         // Display single button prompt
         MShop_Prompt(pme, NULL, 0, NULL, IDS_LICENSE_NOT_FOUND, wButtons, wButtons, wButtons[0], ST_NOSCROLL);
      }
   }
}

/*==========================================================================

   Draws Title String

===========================================================================*/
void MShop_SetTitle(MShop* pme, const char* pszRes, uint16 nTitleID, const AECHAR* pTitle)
{
   AEERect rc;
   AECHAR * pTitleBuf = (AECHAR*)pTitle;

#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   // Save the parameters being passed in for use when flipping screen
   // Only do this if we are not in the process of flipping the screen as
   // we are not changing the data at that point.  We are in fact making
   // use of the data we are saving
   if(!pme->m_bRedrawingUI)
   {
      FreePtr((void**)&pme->m_pTitleResFile);
      FreePtr((void**)&pme->m_pTitleText);
      pme->m_wTitleResId = 0;

      if (NULL != pszRes)
      {
         pme->m_pTitleResFile = STRDUP(pszRes);
      }
      pme->m_wTitleResId = nTitleID;
      if (NULL != pTitle)
      {
         pme->m_pTitleText = WSTRDUP(pTitle);
      }
   }
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
      
   // Clear Background
   SETAEERECT(&rc, 0, 0, pme->m_cx, pme->m_rcMShopLogo.dy);
#ifdef USE_TT_FONTS
   IDISPLAY_FillRect(pme->m_pIDisplay, &rc, RGB_WHITE);
#else
   IDISPLAY_EraseRect(pme->m_pIDisplay, &rc);
#endif // USE_TT_FONTS

   // Draw MobileShop
   if (pme->m_pMShopLogo)
   {
      // Set Image Background Transparent
      IIMAGE_SetParm(pme->m_pMShopLogo, IPARM_ROP, AEE_RO_TRANSPARENT, 0);

      // Draw Image
      IIMAGE_Draw(pme->m_pMShopLogo, 0, 0);
   }

   if (!pTitle)
   {
      if ((pTitleBuf = (AECHAR *)MALLOC(MAX_STR_SIZE)) != NULL)
         ISHELL_LoadResString(pme->a.m_pIShell, pszRes, nTitleID, pTitleBuf, MAX_STR_SIZE);
   }      
   
   if (OEM_UNDERLINE_MENU_TITLE)   
   {
      IDISPLAY_DrawText(pme->m_pIDisplay, AEE_FONT_BOLD, pTitleBuf, -1, pme->m_rcMShopLogo.dx + 1, pme->m_rcMShopLogo.dy - pme->m_cyFont - 2, NULL, 0);
      IDISPLAY_DrawHLine(pme->m_pIDisplay, 0, pme->m_rcMShopLogo.dy - 1, pme->m_cx);
   }
   else
      IDISPLAY_DrawText(pme->m_pIDisplay, AEE_FONT_BOLD, pTitleBuf, -1, pme->m_rcMShopLogo.dx + 1, pme->m_rcMShopLogo.dy - pme->m_cyFont, NULL, 0);

   if (!pTitle)
      FREE(pTitleBuf);

   // Update UI
   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);
}

/*==========================================================================

   Isolates cmshop command security tail and tests it for validity

===========================================================================*/
static boolean MShop_ValidateCmshopCmd(MShop * pme, char * pszArg)
{
   char * pszReceivedSecretString = NULL;

   // Isolate the secret string:
   pszReceivedSecretString = STRCHR(pszArg, CMSHOP_CMD_COMMA );
   if (pszReceivedSecretString == NULL) {
      return FALSE;  // No secret string
   }
   *pszReceivedSecretString = '\0';
   ++pszReceivedSecretString;

   //make sure secret string is valid:
   if ( !CheckSecretString(pme->a.m_pIShell, pszReceivedSecretString) ) {
      return FALSE;  // Secret string invalid
   }
   return TRUE;
}








#ifdef USE_TT_FONTS

//===========================================================================
//
// DESCRIPTION
//    Installs TrueType fonts for use by MobileShop, saving the previously
//    installed fonts for later restoration.
// 
// PARAMETERS
//    pme:  MShop
//
//===========================================================================

static void MShop_InstallTTFonts(MShop* pme)
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
            FARF(MS, ("Error installing normal font"));
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
         FARF(MS, ("Error creating normal font from class ID"));
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
            FARF(MS, ("Error installing bold font"));
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
         FARF(MS, ("Error creating bold font from class ID"));
      }
      ITYPEFACE_Release(pTypeFace);
   }
   else
   {
      FARF(MS, ("Error instantiating ITypeface object"));
   }
}






//===========================================================================
//
// DESCRIPTION
//    Restores the fonts saved by InstallTTFonts()
// 
// PARAMETERS
//    pme:  MShop
//
//===========================================================================

static void MShop_RestoreFonts(MShop* pme)
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
         FARF(MS, ("Unexpected NULL normal font at Restore() time"));
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
         FARF(MS, ("Unexpected NULL bold font at Restore() time"));
      }
   }
}



#endif // USE_TT_FONTS


//===========================================================================
//
// DESCRIPTION
//    Figure out the keys that we need to pay attention to, for this display
//    This could mean useing the standard up/down/select/clr keys or maybe
//    something like FF/REW/Play/Stop
//
//===========================================================================
static boolean MShop_SetKeyList(MShop* pme, IFlip* pIFlip, AEECLSID cls)
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
#endif // FEATURE_LESS_THAN_6_KEYS

   if (NULL == pIFlip)
   {
      CreateInstance(pme->a.m_pIShell, AEECLSID_FLIP, (void **)&pLocalIFlip);
      pIFlip = pLocalIFlip;
   }

   if (NULL == pIFlip)
   {
      FARF(MS, ("MShop_SetKeyList was not passed an IFlip and we were unable to create one"));
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
      FARF(MS, ("MShop_SetKeyList was not passed a display and was not able to get one, defaulting to Display1"));
      cls = AEECLSID_DISPLAY1;
   }
   
   switch (cls)
   {
      case AEECLSID_DISPLAY1:
         FARF(MS, ("MShop_SetKeyList: display 1"));
         displayIndex = 0;
         break;
      case AEECLSID_DISPLAY2:
         FARF(MS, ("MShop_SetKeyList: display 2"));
         displayIndex = 1;
         break;
      case AEECLSID_DISPLAY3:
         FARF(MS, ("MShop_SetKeyList: display 3"));
         displayIndex = 2;
         break;
      case AEECLSID_DISPLAY4:
         FARF(MS, ("MShop_SetKeyList: display 4"));
         displayIndex = 3;
         break;
      default:
         FARF(MS, ("MShop_SetKeyList: Invalid display index"));
         FreeObj((void**)&pLocalIFlip);
         return FALSE;
   }

   if (SUCCESS == IFLIP_GetKeyList(pIFlip, NULL, &dwNumKeys))
   {
      FARF(MS, ("GetKeyList is stating that there are %d numKeys", dwNumKeys));
      pKeyList = (AVKType*)MALLOC(sizeof(AVKType) * dwNumKeys);

      if (SUCCESS == IFLIP_GetKeyList(pIFlip, (int32*)pKeyList, &dwNumKeys))
      {
         boolean bHasUp = FALSE;
         boolean bHasDown = FALSE;
         boolean bHasSelect = FALSE;
         boolean bHasClr = FALSE;

         for( i = 0; i < dwNumKeys; ++i)
         {
            //FARF(MS, ("Key %d of %d is %d", i, dwNumKeys, pKeyList[i]));
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

            FARF(MS, ("We are using alt keys of %d, %d, %d, %d", pme->m_wAltKeyUp, 
               pme->m_wAltKeyDown, pme->m_wAltKeySelect, pme->m_wAltKeyClr));
#endif // FEATURE_LESS_THAN_6_KEYS
         }

         FreePtr((void**)&pKeyList);
         FreeObj((void**)&pLocalIFlip);

         return TRUE;
      }
   }

   FreePtr((void**)&pKeyList);
   FreeObj((void**)&pLocalIFlip);

   return FALSE;
}

#ifdef FEATURE_SCREEN_ROTATE

//===========================================================================
//
// Handle the EVT_SCR_ROTATE event.  
//
//===========================================================================
static boolean MShop_HandleScreenRotate(MShop* pme, uint16 wParam, uint32 dwParam)
{
   

   if (!MShop_RotateScreen(pme))
   {
      FARF(MS, ("Failed to Rotate Screen, returning FALSE for Event"));
      return FALSE;
   }
    
   // We have successfully rotated the display, now redraw it   
   MShop_RedrawDisplay(pme);

   return TRUE;
}

//===========================================================================
//
// Rotate the screen.  
//
//===========================================================================
static boolean MShop_RotateScreen(MShop* pme)
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
      FARF(MS, ("GetDeviceInfoEx(AEE_DEVICESTATE_SCR_ORIENTATION) returned error %d", nErr));
      return FALSE;
   }

   switch (scrOrent)
   {
      case AEE_SO_LANDSCAPE:
         FARF(MS, ("Our Screen has been rotated to SCREEN_ORIENTATION_LANDSCAPE mode"));
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
         FARF(MS, ("Our Screen has been rotated to SCREEN_ORIENTATION_PORTRAIT mode"));
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
      FARF(MS, ("Error Getting Device Bitmap, %d", nErr));
      return FALSE;
   }

   nErr = IBITMAP_QueryInterface(pIBitmap, AEEIID_BITMAPROT, (void**)&pIBitmapRot);

   if (nErr || (NULL == pIBitmapRot))
   {
      FARF(MS, ("Error Getting IBitmapRot, %d", nErr));
      FreePtr((void**)&pIBitmap);
      return FALSE;
   }

   nCurrent = IBITMAPROT_GetRotation(pIBitmapRot);
   FARF(MS, ("Our old rotation was %d, and our new rotation will be %d", nCurrent, rotateValue));

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
      FARF(MS, ("Failed to Rotate"));
      FreePtr((void**)&pIBitmapRot);
      FreePtr((void**)&pIBitmap);
      return FALSE;
   }

   FARF(MS, ("Display is rotated"));

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
static void MShop_RedrawDisplay(MShop* pme)
{
   AEERect  rc;
   AEEDeviceInfo di;

   // Initilize Device Info
   // In the simulator the GetDeviceInfo is always returning the info for Display1, so using GetClipRect
   // to get the size of the display instead.
   IDISPLAY_GetClipRect(pme->m_pIDisplay, &rc);
   ISHELL_GetDeviceInfo(pme->a.m_pIShell, &di);
   pme->m_nColorDepth = di.nColorDepth;
   pme->m_cx = rc.dx;
   pme->m_cy = rc.dy;

   // Clear the screen before redrawing
   //IDISPLAY_ClearScreen(pme->m_pIDisplay);
   IDISPLAY_EraseRgn(pme->m_pIDisplay, 0, 0, pme->m_cx, pme->m_cy);
   FARF(MS, ("We have just cleared the display"));

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

   pme->m_cyFont = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, AEE_FONT_BOLD, NULL, NULL);

   // Update the MShopLogo rectangle
   if (pme->m_pMShopLogo)
   {
      AEEImageInfo ii;
      IIMAGE_GetInfo(pme->m_pMShopLogo, &ii);
      SETAEERECT(&pme->m_rcMShopLogo, 0, 0, ii.cx, ii.cy);
   }
   else
   {
      SETAEERECT(&pme->m_rcMShopLogo, 0, 0, 0, pme->m_cyFont);
   }

   if (OEM_UNDERLINE_MENU_TITLE)
   {
      pme->m_rcMShopLogo.dy += 2;
   }

   // Set the Menu Rect
   if (MShop_GetMenuRect(pme, pme->m_pSK, &rc))
   {
      IMENUCTL_SetRect(pme->m_pMenu, &rc);
   }
   else
   {
      // We were unable to determine rect, set to default
      SETAEERECT(&rc, 0, pme->m_rcMShopLogo.dy, pme->m_cx, pme->m_cy - pme->m_rcMShopLogo.dy);
      IMENUCTL_SetRect(pme->m_pMenu, &rc);
   }

   if (IMENUCTL_GetItemCount(pme->m_pMenu) > 0)
   {
      IMENUCTL_Redraw(pme->m_pMenu);
   }
   if (IMENUCTL_GetItemCount(pme->m_pSK) > 0)
   {
      IMENUCTL_Redraw(pme->m_pSK);
   }


   MShop_RefreshDisplay(pme);

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
static void MShop_RefreshDisplay(MShop* pme)
{
   // Depending on state we may need to call the state functions
   // so that the display is updated properly, and if the prompt
   // is currently being displayed, we need to re-display the prompt
   
   pme->m_bRedrawingUI = TRUE;
   if (ST_APP_OPTIONS == pme->m_wState)
   {
      AEERect menuRect;
      // Make it non-active so hopefully it will completely re-draw
      // after all of the things AppOptionsDialog() does to it.
      IMENUCTL_GetRect(pme->m_pMenu, &menuRect);
      IDISPLAY_EraseRect(pme->m_pIDisplay, &menuRect);
      MShop_AppOptionsDialog(pme);
      IMENUCTL_Redraw(pme->m_pMenu);
      if (NULL != pme->m_pStatic)
      {
         ISTATIC_Redraw(pme->m_pStatic);
      }
   }
   else if (ST_VIEW_LOG == pme->m_wState)
   {
      IMENUCTL_Redraw(pme->m_pMenu);
   }
   
   // Don't need to do it for AppOptions, because it already happened in the re-call
   // to MShop_AppOptionsDialog()
   if ((ST_APP_OPTIONS != pme->m_wState) && (ST_SERVER != pme->m_wState))
   {
      if ((NULL != pme->m_pTitleResFile) || (0 != pme->m_wTitleResId) || (NULL != pme->m_pTitleText))
      {
         MShop_SetTitle(pme, pme->m_pTitleResFile, pme->m_wTitleResId, pme->m_pTitleText);
      }
   }
   // Special handling needs to happing for the InitStatus stuff, because on TouchScreen,
   // We need the background to be clear.  But on non-touch we want the last screen to
   // be displayed, which is a prompt
   if (pme->m_pStatus)
   {
      // If we have a Status object, re-create it so that All if the IDISPLAY_ calls happen.
      // It would be nice if we could just call SetDisplay() on the image, but that will not
      // update the text being writen to the screen at the same time.
      // What if we are in the middle of downloading?  Things will flicker then.  We will re-put up the
      // string "downloading..." and then it will refresh with the % downloaded graphic when the next
      // Status comes in.
      //IIMAGE_SetDisplay(pme->m_pStatus, pNewDisplay);
      if (!pme->m_bDispTouchEnabled && pme->m_bPromptIsDisplayed)
      {
         IMENUCTL_SetActive(pme->m_pSK, FALSE);
         MShop_Prompt(pme, pme->m_pPromptTitle, pme->m_wPromptTitleID, pme->m_pPromptText, pme->m_wPromptTextID,  
                     pme->m_pPromptButtons, pme->m_pPromptItemIDs, pme->m_wPromptDefID, pme->m_dwPromptProps);

         IMENUCTL_SetActive(pme->m_pMenu, FALSE);

         if (pme->m_pStatic)
         {
            uint32 props;

            props = ISTATIC_GetProperties(pme->m_pStatic);
            props |=ST_NOSCROLL;
            ISTATIC_SetProperties(pme->m_pStatic, props);
         }

         
         MShop_InitStatus(pme);
      }
      else
      {
         MShop_InitStatus(pme);
      }

      // If we have a download % redraw it. if We draw 0 or 100 we may end up 
      // drawing the bar when we don't want.  Because of not drawing 0 or 100
      // we may end up with the text "downloading..." for a short period of time,
      // but it should be a very edgy case.
      if ((pme->m_wPct > 0) && (pme->m_wPct < 100))
      {
         DrawHist(pme->m_pIDisplay, &pme->m_rcHist, pme->m_wPct);  // Draw Histogram with % complete
      }

   }
   else if (pme->m_bPromptIsDisplayed)
   {
      MShop_Prompt(pme, pme->m_pPromptTitle, pme->m_wPromptTitleID, pme->m_pPromptText, pme->m_wPromptTextID,  
                     pme->m_pPromptButtons, pme->m_pPromptItemIDs, pme->m_wPromptDefID, pme->m_dwPromptProps);

   }

   pme->m_bRedrawingUI = FALSE;
}
#endif //defined(FEATURE_SCREEN_ROTATE) || defined(FEATURE_SUPPORT_MULTIPLE_DISPLAYS)


#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
//===========================================================================
//
// The callback function used to Call MShop_RecreateUI() after a flip
// This is so that "brew" is fully finished Activating/enabling the new
// display.  Before using a callback function, sometimes the controls
// would not be fully created, underlying member of the IStaticCtl and the
// IMenuCtl would not have valid values
//
//===========================================================================
static void MShop_RecreateUICallback(void* pData)
{
   MShop* pme = (MShop*)pData;

   FARF(MS, ("Got RecreateUI Callback"));

   if (pme->m_cbUpdateUI.pfnCancel)
   {
      pme->m_cbUpdateUI.pfnCancel(&pme->m_cbUpdateUI);
      pme->m_cbUpdateUI.pfnCancel = NULL;
   }

   MShop_RecreateUI(pme);
}

//===========================================================================
//
// Callback for when the new Display is enabled after a flip when the flip
// happened while we were suspended.
//
//===========================================================================
static void MShop_EnableDispResumingCallback(void* pData)
{
   MShop* pme = (MShop*)pData;

   FARF(MS, ("Got EnableDisplayResuming Callback"));

   if (pme->cbDisplayEnabled)
   {
      CALLBACK_Cancel(pme->cbDisplayEnabled);
      FreePtr((void**)&pme->cbDisplayEnabled);
   }

   FARF(MS, ("Our current display thinks that its Enabled state is %d", IDISPLAY_IsEnabled(pme->m_pIDisplay)));

   MShop_RecreateResumingUI(pme);
}

//===========================================================================
//
// Callback for when the new Display is enabled after we resumed and the display
// was listed as not enabled, but is the same display we were suspended on
//
//===========================================================================
static void MShop_EnableDispBeforeSetStateCallback(void* pData)
{
   MShop* pme = (MShop*)pData;

   FARF(MS, ("Got EnableDisplayResuming Callback"));

   if (pme->cbDisplayEnabled)
   {
      CALLBACK_Cancel(pme->cbDisplayEnabled);
      FreePtr((void**)&pme->cbDisplayEnabled);
   }

   FARF(MS, ("Our current display thinks that its Enabled state is %d", IDISPLAY_IsEnabled(pme->m_pIDisplay)));

   MShop_SetStateAfterResume(pme);

   pme->m_bResuming = FALSE;
   
   return;
}


//===========================================================================
//
// Callback for when the new Display is enabled after a flip.
//
//===========================================================================
static void MShop_EnableDispCallback(void* pData)
{
   MShop* pme = (MShop*)pData;

   FARF(MS, ("Got EnableDisplay Callback"));

   if (pme->cbDisplayEnabled)
   {
      CALLBACK_Cancel(pme->cbDisplayEnabled);
      FreePtr((void**)&pme->cbDisplayEnabled);
   }

   FARF(MS, ("Our current display thinks that its Enabled state is %d", IDISPLAY_IsEnabled(pme->m_pIDisplay)));

   // It seems necessary to use the Resume Callback here in stead of 
   // calling RecreateUI directly as sometimes when flipping the controls
   // were not being fully created (like a menu control's internal display pointer
   // was not valid
   //MShop_RecreateUI(pme);
   MEMSET(&pme->m_cbUpdateUI, '\0', sizeof(pme->m_cbUpdateUI));
   pme->m_cbUpdateUI.pfnNotify = MShop_RecreateUICallback; // My callback function
   pme->m_cbUpdateUI.pNotifyData = pme;
   pme->m_cbUpdateUI.pfnCancel = NULL;  //Initialize fo NULL.  It is updated by Shell
   ISHELL_Resume(pme->a.m_pIShell, &pme->m_cbUpdateUI);
}

//===========================================================================
//
// Callback for when the secondary display is enabled after a flip when we
// are continuing to download when the flip the phone closed
//
//===========================================================================
static void MShop_EnableSecDispCallback(void* pData)
{
   MShop* pme = (MShop*)pData;

   FARF(MS, ("Got EnableSecDisplay Callback"));

   if (pme->cbDisplayEnabled)
   {
      CALLBACK_Cancel(pme->cbDisplayEnabled);
      FreePtr((void**)&pme->cbDisplayEnabled);
   }

   FARF(MS, ("Our secondary display thinks that its Enabled state is %d", IDISPLAY_IsEnabled(pme->m_pIDisplay2)));

   MShop_SetExternalStaticText(pme, pme->m_wExDisplayResId);


}

//===========================================================================
//
// DESCRIPTION
//    Create the display and set the "downloading" text that we need to 
//    display on the "secondary display" since the user closed the flip
//    but we are not exiting because they are downloading.
//
//===========================================================================
static void MShop_CreateDownloadInfoDisplay(MShop* pme)
{

   if (NULL == pme->m_pIDisplay2)
   {
      IFlip    *pIFlip = NULL;
      AEECLSID newDisplayId = 0;

      CreateInstance(pme->a.m_pIShell, AEECLSID_FLIP, (void **)&pIFlip);

      if (NULL == pIFlip)
      {
         return;
      }

      GetDisplayToUse(pIFlip, &newDisplayId);
      
      FreeObj((void**)&pIFlip);

      if (0 == newDisplayId)
      {
         return;
      }

      CreateInstance(pme->a.m_pIShell, newDisplayId, (void **)&pme->m_pIDisplay2);

      if (NULL == pme->m_pIDisplay2)
      {
         return;
      }
   }

   IDISPLAY_MakeDefault(pme->m_pIDisplay2);

   MShop_SetExternalStaticText(pme, IDS_STILL_DOWNLOADING);
      
   return;
}


//===========================================================================
//
// DESCRIPTION
//    Set the info text on the external display that is active
//    because we are continuing to download even thought they have
//    flipped the phone close
//
//===========================================================================
static boolean MShop_SetExternalStaticText(MShop* pme, uint16 resId)
{
   AECHAR* szTextBuf = NULL;
   AEERect rect;

   if ((NULL == pme) || (NULL == pme->m_pIDisplay2))
   {
      return FALSE;
   }

   if (!IDISPLAY_IsEnabled(pme->m_pIDisplay2))
   {
      pme->m_wExDisplayResId = resId;

      pme->cbDisplayEnabled = (AEECallback*) MALLOC(sizeof(AEECallback));
      MEMSET(pme->cbDisplayEnabled, 0, sizeof(AEECallback));

      CALLBACK_Init(pme->cbDisplayEnabled, MShop_EnableSecDispCallback, pme);
      IDISPLAY_NotifyEnable(pme->m_pIDisplay2, pme->cbDisplayEnabled);
      return FALSE;
   }

   if (NULL == pme->m_pStaticDlInfo)
   {
      if (CreateInstance(pme->a.m_pIShell, AEECLSID_STATIC, (void**)&pme->m_pStaticDlInfo))
      {
         return FALSE;
      }
   }

   szTextBuf = (AECHAR *)MALLOC(MAX_STR_SIZE);

   if (NULL == szTextBuf)
   {
      return FALSE;
   }

   IDISPLAY_GetClipRect(pme->m_pIDisplay2, &rect);
   
   ISHELL_LoadResString(pme->a.m_pIShell, BREWAPPMGRLS_RES_LOC, resId, szTextBuf, MAX_STR_SIZE);

   ISTATIC_SetRect(pme->m_pStaticDlInfo, &rect);
   ISTATIC_SetText(pme->m_pStaticDlInfo, NULL, szTextBuf, 0, AEE_FONT_NORMAL);
   ISTATIC_SetActive(pme->m_pStaticDlInfo, TRUE);
   ISTATIC_Redraw(pme->m_pStaticDlInfo);

   IDISPLAY_UpdateEx(pme->m_pIDisplay2, FALSE);

   FreePtr((void**)&szTextBuf);

   // Reset our main screen as the default
   IDISPLAY_MakeDefault(pme->m_pIDisplay);

   return TRUE;
}


//===========================================================================
//
// DESCRIPTION
//    Do the dirty work of switching displays when the user closes the clamshell
//    or slider.  This involves re-creating all of the menu, static, and text
//    controls so that they pick up the new "active" display.
//    Then all of the old data is copied over to the new controls in RecreateUI
//
//===========================================================================
static SwitchDisplaysResult MShop_SwitchDisplays(MShop* pme, boolean useDisplay1)
{
   IDisplay *pNewDisplay = NULL;
   IFlip    *pIFlip = NULL;
   AEECLSID *pClsList = NULL;
   AEEFlipInfo *pFlipInfo = NULL;
   int32    dwNumDisplays = 0;
   int16    wNumFlips = 0;
   int16    i = 0;
   AEECLSID newDisplayId = 0;

#ifdef FEATURE_HAS_MULTIPLE_DISPLAYS
   // If we can get a flip, but we do not have mult displays (like a slider)
   // don't try to switch displays, but return TRUE like we did.  We should just
   // stay on the display then.
   if (!pme->m_bHasMultipleDisplays)
   {
      return DISPLAY_SWITCH_SUCCEEDED;
   }
#endif

   FARF(MS, ("Trying to switch displays, useDisplay1 is %d", useDisplay1));

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
   IDISPLAY_EraseRgn(pme->m_pIDisplay, 0, 0, pme->m_cx, pme->m_cy);
   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);

   CreateInstance(pme->a.m_pIShell, AEECLSID_FLIP, (void **)&pIFlip);

   // If we cannot get an iFlip object, we cannot flip.
   if (NULL == pIFlip)
   {
      return DISPLAY_SWITCH_FAILED;
   }

   if ((SUCCESS != IFLIP_GetDisplayList(pIFlip, NULL, &dwNumDisplays)) || (0 == dwNumDisplays))
   {
      FARF(MS, ("There are no displays available for this flip"));
      if (useDisplay1)
      {
         FARF(MS, ("I was going to use %d", AEECLSID_DISPLAY1)); 
         newDisplayId = AEECLSID_DISPLAY1;
      }
      else
      {
         FARF(MS, ("I was going to use %d", AEECLSID_DISPLAY2));

         newDisplayId = AEECLSID_DISPLAY2;
      }
   }
   else
   {

      pClsList = (AEECLSID*) MALLOC(sizeof(AEECLSID) * dwNumDisplays);

      if (NULL == pClsList)
      {
         FARF(MS, ("Out of memory"));
         FreeObj((void**)& pIFlip);
         return DISPLAY_SWITCH_FAILED;
      }

      if (SUCCESS != IFLIP_GetDisplayList(pIFlip, (void*)pClsList, &dwNumDisplays))
      {
         FARF(MS, ("Unable to get the display IDs for this flip"));

         if (useDisplay1)
         {
            FARF(MS, ("I was going to use %d", AEECLSID_DISPLAY1)); 
            newDisplayId = AEECLSID_DISPLAY1;
         }
         else
         {
            FARF(MS, ("I was going to use %d", AEECLSID_DISPLAY2));

            newDisplayId = AEECLSID_DISPLAY2;
         }
         
      }
      else
      {
         FARF(MS, ("IFlip told me to use %d", pClsList[0]));
         newDisplayId = pClsList[0];      
      }
   }

   // We know which display is becoming current.  Check to see if MS is
   // configured to run on this display, and if not just return FALSE
   if (FALSE == DisplayEnabled(pme->a.m_pIShell, pme->m_bHasMultipleDisplays,
      pme->m_wEnabledDisplays, newDisplayId))
   {
      FARF(MS, ("Can't run on this display"));
      FreePtr((void**)&pClsList);
      FreeObj((void**)& pIFlip);

      // If we are in the process of downloading an APP, don't quite exit.
      // Wait until the download is over then exit
      if ((ST_IN_PROGRESS == pme->m_wState) && (ST_PURCHASE == pme->m_wLastState))
      {
         FARF(MS, ("Not Exiting MobileShop as we are downloading an app"));
         pme->m_bExitAfterDownload = TRUE;
         return DISPLAY_SWITCH_WOULD_EXIT;
      }

      return DISPLAY_SWITCH_FAILED;
   }

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
   // newDisplayId is the current active display, check whether it is touch enabled or not
   if(newDisplayId)
   {
      pme->m_bDispTouchEnabled = IsDisplayTouchEnabled(newDisplayId, pme->m_wTouchDisplays);
      FARF(MS, ("newDisplayId = pme->m_bDispTouchEnabled = %d   &   %d", newDisplayId, pme->m_bDispTouchEnabled));
   }
#endif

   MShop_SetKeyList(pme, pIFlip, newDisplayId);

   CreateInstance(pme->a.m_pIShell, newDisplayId, (void **)&pNewDisplay);

   if (NULL == pNewDisplay)
   {
      FARF(MS, ("Unable to create IDisplay for with clsid %d", newDisplayId));
      FreePtr((void**)&pClsList);
      FreeObj((void**)& pIFlip);
      return DISPLAY_SWITCH_FAILED;
   }

   if ((SUCCESS != IFLIP_GetFlipIDList(pIFlip, NULL, &wNumFlips)) || (0 == wNumFlips))
   {
      FARF(MS, ("There are no flips for this device"));
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
               FARF(MS, ("Display index %d has ID %d, x = %d, y = %d, z = %d", i, DEVNOTIFY_GET_ID(pFlipInfo[i]),
                           DEVNOTIFY_GET_X(pFlipInfo[i]), DEVNOTIFY_GET_Y(pFlipInfo[i]), DEVNOTIFY_GET_Z(pFlipInfo[i])));
            }
         }
         else
         {
            FARF(MS, ("Unable to get the list of flips"));
         }
      }
      else
      {
         FARF(MS, ("No memory to get flip possitions"));
      }
   }

   FreePtr((void**)&pFlipInfo);
   FreePtr((void**)&pClsList);
   FreeObj((void**)& pIFlip);

#ifdef USE_TT_FONTS
   // Restore the original fonts
   MShop_RestoreFonts(pme);
#endif // USE_TT_FONTS

   IDISPLAY_MakeDefault(pNewDisplay);

   IDISPLAY_Release(pme->m_pIDisplay);

   pme->m_pIDisplay = pNewDisplay;
   pNewDisplay = NULL;

   FARF(MS, ("Our current display thinks that its Enabled state is %d", IDISPLAY_IsEnabled(pme->m_pIDisplay)));

   if (IDISPLAY_IsEnabled(pme->m_pIDisplay))
   {
      boolean succeeded = FALSE;
      if (pme->m_bResuming)
      {
         succeeded = MShop_RecreateResumingUI(pme);
      }
      else
      {
         succeeded = MShop_RecreateUI(pme);
      }

      if (succeeded)
      {
         return DISPLAY_SWITCH_SUCCEEDED;
      }
      
      return DISPLAY_SWITCH_FAILED;
   }

   pme->cbDisplayEnabled = (AEECallback*) MALLOC(sizeof(AEECallback));
   MEMSET(pme->cbDisplayEnabled, 0, sizeof(AEECallback));
   if (pme->m_bResuming)
   {
      CALLBACK_Init(pme->cbDisplayEnabled, MShop_EnableDispResumingCallback, pme);
   }
   else
   {
      CALLBACK_Init(pme->cbDisplayEnabled, MShop_EnableDispCallback, pme);
   }
   IDISPLAY_NotifyEnable(pme->m_pIDisplay, pme->cbDisplayEnabled);

   return DISPLAY_SWITCH_SUCCEEDED;

}

static boolean MShop_RecreateUI(MShop* pme)
{
   IMenuCtl *pNewMenu = NULL;      // Standard Menu
   IMenuCtl *pNewSK = NULL;
   IStatic  *pNewStatic = NULL;    // Static Control
   ITextCtl *pNewText = NULL;      // Text Control - Search Menu
   int result;
   AEEDeviceInfo  di;
   AEERect  dispRect = {0,0,0,0};
   AEERect rc;
   boolean bResetActive = FALSE;

   FARF(MS, ("Recreating the UI to use the new display"));

   if (pme->m_pMenu)
   {
      result = CreateInstance(pme->a.m_pIShell, GetMenuCtlClassID(pme->m_bDispTouchEnabled), (void **)&pNewMenu);
      if ((result != AEE_SUCCESS) || (NULL == pNewMenu))
      {
         FARF(MS, ("Unable to create MenuCtl"));
         return FALSE;
      }
   }
   if (pme->m_pSK)
   {
      result = CreateInstance(pme->a.m_pIShell, GetSoftKeyClassID(pme->m_bDispTouchEnabled),(void **)&pNewSK);
      if ((result != AEE_SUCCESS) || (NULL == pNewSK))
      {
         FARF(MS, ("Unable to create SoftKeyCtl"));
         FreeObj((void**)&pNewMenu);
         return FALSE;
      }
   }

   if (pme->m_pStatic && !pme->m_bPromptIsDisplayed)
   {
      result = CreateInstance(pme->a.m_pIShell, AEECLSID_STATIC,(void **)&pNewStatic);
      if ((result != AEE_SUCCESS) || (NULL == pNewStatic))
      {
         FARF(MS, ("Unable to create Staticl"));
         FreeObj((void**)&pNewSK);
         FreeObj((void**)&pNewMenu);
         return FALSE;
      }
   }
   if (pme->m_pText)
   {
      result = CreateInstance(pme->a.m_pIShell, AEECLSID_TEXTCTL,(void **)&pNewText);
      if ((result != AEE_SUCCESS) || (NULL == pNewText))
      {
         FARF(MS, ("Unable to create TextCtl"));
         FreeObj((void**)&pNewStatic);
         FreeObj((void**)&pNewSK);
         FreeObj((void**)&pNewMenu);
         return FALSE;
      }
   }

#ifdef USE_TT_FONTS
   // Install the TrueType fonts here, before making the call to
   // GetFontMetrics() and before any menu controls are instantiated.
   MShop_InstallTTFonts(pme);
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
   //pme->m_cx = di.cxScreen;
   //pme->m_cy = di.cyScreen;
   pme->m_cx = dispRect.dx;
   pme->m_cy = dispRect.dy;
   pme->m_nColorDepth = di.nColorDepth;

   // Fix for CR88685.  See also changes in BrewAppMgr.c
   // Don't clear the screen if we're not in a "real" UI state
   if (NO_ENTRY != pme->m_wState)
   {
      // For testing purposes Clear the display
      //IDISPLAY_ClearScreen(pme->m_pIDisplay);
      IDISPLAY_EraseRgn(pme->m_pIDisplay, 0, 0, pme->m_cx, pme->m_cy);
   }

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

   pme->m_cyFont = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, AEE_FONT_BOLD, NULL, NULL);

   if (pme->m_pMShopLogo)
   {
      IIMAGE_SetDisplay(pme->m_pMShopLogo, pme->m_pIDisplay);
   }
   else
   {
      // As we are only changing the Rect here, we need to pay attention to 
      // the Menu Title adjustment only here
      SETAEERECT(&pme->m_rcMShopLogo, 0, 0, 0, pme->m_cyFont);

      if (OEM_UNDERLINE_MENU_TITLE)
         pme->m_rcMShopLogo.dy += 2;
   }

   //
   // Copy all of the data over to the new instances
   //

   if (pNewSK)
   {
      int numItems;
      int curItem;
      uint16 curItemId;
      CtlAddItem ai;
      boolean isActive = FALSE;
      boolean onlyBack = TRUE;
      
      SetMenuStyleAndColor(pNewSK, GetSoftKeyClassID(pme->m_bDispTouchEnabled), pme->m_nColorDepth, pme->m_bDispTouchEnabled);


      numItems = IMENUCTL_GetItemCount(pme->m_pSK);
      for (curItem = 0; curItem < numItems; ++curItem)
      {
         MEMSET(&ai, 0, sizeof(ai));
         curItemId = IMENUCTL_GetItemID(pme->m_pSK, curItem);
         if (IMENUCTL_GetItem(pme->m_pSK, curItemId, &ai))
         {
            // Ignore copying over anything this large.  Right this second
            // it is only the "MultiTap" button that the TextCtl
            // we re-add when we assign the SK back to it below
            if ((ai.wItemID > 0xFF00) && (pNewText))
            {
               continue;
            }
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

      isActive = IMENUCTL_IsActive(pme->m_pSK);
      
      if(!pme->m_bDispTouchEnabled && numItems > 0) 
      {
         if (onlyBack && !pme->m_bPromptIsDisplayed)
         {
            IMENUCTL_DeleteAll(pNewSK);
            if (ST_VIEW_LOG != pme->m_wState)
            {
               IMENUCTL_SetActive(pNewSK, FALSE);
               if (isActive)
               {
                  bResetActive = TRUE;
               }
            }
            else
            {
               IMENUCTL_AddItem(pNewSK, BREWAPPMGRLS_RES_LOC, IDC_OK, IDC_OK, 0, 0);
               IMENUCTL_SetActive(pNewSK, TRUE);
            }
            
         }
         else
         {
            IMENUCTL_SetSel(pNewSK, IMENUCTL_GetSel(pme->m_pSK));
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

      // If we are not sure about drawing the menu, deactivate it first
      if (pme->m_bDispTouchEnabled && ST_IN_PROGRESS == pme->m_wState)
      {
         IMENUCTL_SetActive(pNewMenu, FALSE);
      }

      SetMenuStyleAndColor(pNewMenu, GetMenuCtlClassID(pme->m_bDispTouchEnabled), pme->m_nColorDepth, pme->m_bDispTouchEnabled);


      if (!MShop_GetMenuRect(pme, pNewSK, &rc))
      {
         // Use the default
         SETAEERECT(&rc, 0, pme->m_rcMShopLogo.dy, pme->m_cx, pme->m_cy - pme->m_rcMShopLogo.dy);
      }

      IMENUCTL_SetRect(pNewMenu, &rc);


      numItems = IMENUCTL_GetItemCount(pme->m_pMenu);
      for (curItem = 0; curItem < numItems; ++curItem)
      {
         MEMSET(&ai, 0, sizeof(ai));
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

      IMENUCTL_SetTitle(pNewMenu, pme->m_pMenuResFile, pme->m_wMenuResID, pme->m_pMenuText);
      
      // Detain drawing menuctl for ST_APP_OPTIONS screen, as the menu will be drawn along with appinfo later    	  
      if(!pme->m_bDispTouchEnabled && ST_APP_OPTIONS != pme->m_wState && ST_IN_PROGRESS != pme->m_wState) 
      {
         boolean isActive = IMENUCTL_IsActive(pme->m_pMenu);
         IMENUCTL_SetSel(pNewMenu, IMENUCTL_GetSel(pme->m_pMenu));
         if (numItems > 0)
         {
           IMENUCTL_Redraw(pNewMenu);
         }
         IMENUCTL_SetActive(pNewMenu, isActive);
      }
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
      // This Redraw call here can be dangerous.  Currently if pme->m_pStatic
      // is around it should be displayed.  But if that ever changes this call here
      // might put text on the screen that should not be displayed.  We cannot
      // check If Active, since the ctrl is never active, the button menu below it
      // is active even when we are being displayed.
      //ISTATIC_SetActive(pNewStatic, TRUE);
      //ISTATIC_Redraw(pNewStatic);

      ISTATIC_SetActive(pNewStatic, ISTATIC_IsActive(pme->m_pStatic));
   }

   if (pNewText)
   {
      AEERect rcm;
      AECHAR * psz;
      uint32 props;
      boolean bIsActive;

      IMENUCTL_GetRect(pNewSK, &rcm);
      SETAEERECT(&rc, MARGIN, pme->m_rcMShopLogo.dy + MARGIN, pme->m_cx - 2*MARGIN, pme->m_cyFont*((pme->m_cy - pme->m_rcMShopLogo.dy - rcm.dy - MARGIN)/pme->m_cyFont));
      ITEXTCTL_SetRect(pNewText, &rc);
      props = ITEXTCTL_GetProperties(pme->m_pText);
      ITEXTCTL_SetProperties(pNewText, props|TP_FOCUS_NOSEL);
      ITEXTCTL_SetMaxSize(pNewText, MAX_STR_CHARS);
      ITEXTCTL_SetSoftKeyMenu(pNewText, pNewSK);

      ITEXTCTL_SetTitle(pNewText, BREWAPPMGRLS_RES_LOC, (uint16)((ST_CREDITBACK == pme->m_wState) ? IDS_CREDITBACK_TITLE : IDS_SEARCH_TITLE),NULL);

      psz = ITEXTCTL_GetTextPtr(pme->m_pText);
      if (psz)
      {
         ITEXTCTL_SetText(pNewText, psz, -1);
      }
      ITEXTCTL_SetInputMode(pNewText, ITEXTCTL_GetInputMode(pme->m_pText, NULL));
      bIsActive = ITEXTCTL_IsActive(pme->m_pText);
      ITEXTCTL_SetActive(pNewText, bIsActive);
      //if (bIsActive)
      //{
         // Redraw the menu control for this text control
         // Not sure if this is necessary
      //   IMENUCTL_Redraw(pNewSK);
      //}
   }

   // Set our "Processing Flag" so that we don't try to reset some vars and any of the
   // menus
   pme->m_bRedrawingUI = TRUE;

   // Delete the old instances and store the new ones
   if (pNewStatic)
   {
      ISTATIC_Release(pme->m_pStatic);
      pme->m_pStatic = pNewStatic;
   }
   if (pNewText)
   {
      ITEXTCTL_Release(pme->m_pText);
      pme->m_pText = pNewText;
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

   // If the SoftKey was active and it only had a back button, we need to set a new
   // active control as we cleard out the SK and set it to not be active (we don't
   // want it displayed.  Since this only happens when the prompt is not displayed
   // the only controls are the text and the menu controls.  Since it if text control
   // exits we are displaying it, set it active.  Otherwise (and the normal case)
   // set the menu active
   // bResetActive is never set if we are in a TouchScreen
   if (!pme->m_pText && !IMENUCTL_IsActive(pme->m_pSK) && !pme->m_bDispTouchEnabled)
   {
      // The soft key is not active and we are not in touchmode, so make sure that 
      // the menu is active
      bResetActive = TRUE;
   }
   if (bResetActive)
   {
      if (pme->m_pText)
      {
         ITEXTCTL_SetActive(pme->m_pText, TRUE);
      }
      else
      {
         IMENUCTL_SetActive(pme->m_pMenu, TRUE);
      }
   }

//--------------------------------------------------------------------
// handles state specific issues
   if((ST_CATEGORIES == pme->m_wState ||
       ST_VIEW_LOG == pme->m_wState ||
       ST_SERVER == pme->m_wState ) && 
      !pme->m_bPromptIsDisplayed)
   {  
     MShop_AddSoftKey(pme);
   }

//-------------------------------------------------------------------
   MShop_RefreshDisplay(pme);

   pme->m_bRedrawingUI = FALSE;

      // Fix for CR88685.  See also changes in BrewAppMgr.c
   // Don't update the display if we're not in a "real" UI state
   if (NO_ENTRY != pme->m_wState)
   {
      IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);
   }

   // We are done
   return TRUE;
}


//===========================================================================
//
// Recreate our controls on the new display.  Then re-set state to re-draw
// the UI
//
//===========================================================================
static boolean MShop_RecreateResumingUI(MShop* pme)
{
   IMenuCtl *pNewMenu = NULL;      // Standard Menu
   IMenuCtl *pNewSK = NULL;
   IStatic  *pNewStatic = NULL;    // Static Control
   ITextCtl *pNewText = NULL;      // Text Control - Search Menu
   int result = AEE_SUCCESS;
   AEEDeviceInfo  di;
   AEERect  dispRect = {0,0,0,0};

   FARF(MS, ("Recreating the UI to use the new display"));

   if (pme->m_pMenu)
   {
      result = CreateInstance(pme->a.m_pIShell, GetMenuCtlClassID(pme->m_bDispTouchEnabled), (void **)&pNewMenu);
      if ((result != AEE_SUCCESS) || (NULL == pNewMenu))
      {
         FARF(MS, ("Unable to create MenuCtl"));
         pme->m_bResuming = FALSE;
         return FALSE;
      }
   }
   if (pme->m_pSK)
   {
      result = CreateInstance(pme->a.m_pIShell, GetSoftKeyClassID(pme->m_bDispTouchEnabled),(void **)&pNewSK);
      if ((result != AEE_SUCCESS) || (NULL == pNewSK))
      {
         FARF(MS, ("Unable to create SoftKeyCtl"));
         FreeObj((void**)&pNewMenu);
         pme->m_bResuming = FALSE;
         return FALSE;
      }
   }

   if (pme->m_pStatic && !pme->m_bPromptIsDisplayed)
   {
      result = CreateInstance(pme->a.m_pIShell, AEECLSID_STATIC,(void **)&pNewStatic);
      if ((result != AEE_SUCCESS) || (NULL == pNewStatic))
      {
         FARF(MS, ("Unable to create Staticl"));
         FreeObj((void**)&pNewSK);
         FreeObj((void**)&pNewMenu);
         pme->m_bResuming = FALSE;
         return FALSE;
      }
   }
   if (pme->m_pText)
   {
      result = CreateInstance(pme->a.m_pIShell, AEECLSID_TEXTCTL,(void **)&pNewText);
      if ((result != AEE_SUCCESS) || (NULL == pNewText))
      {
         FARF(MS, ("Unable to create TextCtl"));
         FreeObj((void**)&pNewStatic);
         FreeObj((void**)&pNewSK);
         FreeObj((void**)&pNewMenu);
         pme->m_bResuming = FALSE;
         return FALSE;
      }
   }

   IDISPLAY_GetClipRect(pme->m_pIDisplay, &dispRect);
   ISHELL_GetDeviceInfo(pme->a.m_pIShell, &di);
   //pme->m_cx = di.cxScreen;
   //pme->m_cy = di.cyScreen;
   pme->m_cx = dispRect.dx;
   pme->m_cy = dispRect.dy;
   pme->m_nColorDepth = di.nColorDepth;

   // Fix for CR88685.  See also changes in BrewAppMgr.c
   // Don't clear the screen if we're not in a "real" UI state
   if (NO_ENTRY != pme->m_wState)
   {
      // For testing purposes Clear the display
      //IDISPLAY_ClearScreen(pme->m_pIDisplay);
      IDISPLAY_EraseRgn(pme->m_pIDisplay, 0, 0, pme->m_cx, pme->m_cy);
   }

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

   pme->m_cyFont = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, AEE_FONT_BOLD, NULL, NULL);

   if (pme->m_pMShopLogo)
   {
      IIMAGE_SetDisplay(pme->m_pMShopLogo, pme->m_pIDisplay);
   }
   else
   {
      // As we are only changing the Rect here, we need to pay attention to 
      // the Menu Title adjustment only here
      SETAEERECT(&pme->m_rcMShopLogo, 0, 0, 0, pme->m_cyFont);

      if (OEM_UNDERLINE_MENU_TITLE)
         pme->m_rcMShopLogo.dy += 2;
   }


   // Make srue that the styles are correct
   if (pNewSK)
   {
      SetMenuStyleAndColor(pNewSK, GetSoftKeyClassID(pme->m_bDispTouchEnabled), pme->m_nColorDepth, pme->m_bDispTouchEnabled);
   }

   if (pNewMenu)
   {
      SetMenuStyleAndColor(pNewMenu, GetMenuCtlClassID(pme->m_bDispTouchEnabled), pme->m_nColorDepth, pme->m_bDispTouchEnabled);
   }


   // If we have a text control copy over the text
   if (pNewText)
   {
      AECHAR * psz = NULL;

      psz = ITEXTCTL_GetTextPtr(pme->m_pText);
      if (psz)
      {
         ITEXTCTL_SetText(pNewText, psz, -1);
      }

   }

   // Delete the old instances and store the new ones
   if (pNewStatic)
   {
      ISTATIC_Release(pme->m_pStatic);
      pme->m_pStatic = pNewStatic;
   }
   if (pNewText)
   {
      ITEXTCTL_Release(pme->m_pText);
      pme->m_pText = pNewText;
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

   return MShop_SetStateAfterResume(pme);
}
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS


//===========================================================================
//
// DESCRIPTION
//    Store the values set as the MenuTitle so that they can be re-used when
//    we are trying to re-create the display on a new screen when the user
//    flips the phone cover to utilize a different display
//
// PARAMETERS
//    The same as IMENUCTL_SetTitle()
//
//
//===========================================================================
static void MShop_StoreMenuTitleInfo(MShop* pme, char* pszResFile, uint16 wResID, AECHAR* pwszText)
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

   pme->m_pMenuResFile = STRDUP(pszResFile);
   pme->m_wMenuResID = wResID;
   pme->m_pMenuText = WSTRDUP(pwszText);
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
}



#ifdef APPINFO_IN_APPOPTIONS

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
static void MShop_StoreStaticTextInfo(MShop* pme, AECHAR* pwszTitle, AECHAR* pwszText, AEEFont fntTitle, AEEFont fntText)
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

   pme->m_pStaticTitle = WSTRDUP(pwszTitle);
   pme->m_pStaticText = WSTRDUP(pwszText);
   pme->m_pStaticFntTitle = fntTitle;
   pme->m_pStaticFntText = fntText;
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
}
#endif //APPINFO_IN_APPOPTIONS


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
/*
static void MShop_StoreStaticSizeToFitInfo(MShop* pme, boolean called, AEERect* prc)
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
*/

//===========================================================================
//
// DESCRIPTION
//    Store the values set as the passed into the Prompt() functino so that they can be re-used when
//    we are trying to re-create the display on a new screen when the user
//    flips the phone cover to utilize a different display
//
// PARAMETERS
//    The same as MShop_Prompt()
//
//
//===========================================================================
static void MShop_StorePromptInfo(MShop * pme, const AECHAR* szTitle, uint16 wIDTitle, const AECHAR* szText, uint16 wIDText, uint16* wButtons, uint16* wItemIDs, uint16 wDefID, uint32 dwProps)
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
   FreePtr((void**)&(pme->m_pPromptItemIDs));

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
      pme->m_pPromptButtons = CopyUInt16Array(wButtons);
/*
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
*/
   }
   if (wItemIDs)
   {
      pme->m_pPromptItemIDs = CopyUInt16Array(wItemIDs);
/*
      size_t numIDs = 0;

      // Calculate number of items
      while(*(wItemIDs + numIDs))
      {
         numIDs++;
      }
      if (numIDs > 0)
      {
         pme->m_pPromptItemIDs = (uint16*)MALLOC((numIDs + 1) * sizeof(uint16));
         if (NULL != pme->m_pPromptItemIDs)
         {
            (void)MEMCPY(pme->m_pPromptItemIDs, wItemIDs, numIDs * sizeof(uint16));
            pme->m_pPromptItemIDs[numIDs] = 0;
         }
      }
*/
   }
   pme->m_wPromptDefID = wDefID;
   pme->m_dwPromptProps = dwProps;

#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
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
static boolean MShop_LoadOEMCfg(MShop *pme)
{
   int      nErr     = SUCCESS;
   char*    pszName  = NULL;
   char*    pszValue = NULL;
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
      nErr = IFILEMGR_GetLastError(pme->m_pIfm);

      // if the file doesn't exist, that's fine, we just use default values.  Other errors
      // indicate failure.
      FreePtr((void **) &pOEMCfg);
      return (nErr == EFILENOEXISTS);
   }
   while (OEMCfg_NextNVPair(pOEMCfg, &pszName, &pszValue))
   {
      FARF(MS, ("Found config pair '%s', '%s'", pszName, pszValue));

#ifdef FEATURE_CANCELONCLOSE
      if (!STRICMP(pszName, OEMCFG_CANCELONCLOSE))
      {
         if (!STRICMP(pszValue, "false") || !STRICMP(pszValue, "0"))
            pme->m_bCancelOnClose = FALSE;
      }
#endif

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
      if (!STRICMP(pszName, OEMCFG_TOUCH_DISPLAYS))
      {
         pme->m_wTouchDisplays = (uint16)ATOI(pszValue);
         FARF(MS, ("Our %s value is %d", OEMCFG_TOUCH_DISPLAYS, pme->m_wTouchDisplays));
      }
#endif

#ifdef FEATURE_HAPTIC_SUPPORT
      if (!STRICMP(pszName, OEMCFG_HAPTIC_RESPONSE_TONE))
      {
         pme->m_ToneData.eTone = (uint16)ATOI(pszValue);
         FARF(MS, ("Our %s value is %d", OEMCFG_HAPTIC_RESPONSE_TONE, pme->m_ToneData.eTone));
      }
      if (!STRICMP(pszName, OEMCFG_HAPTIC_RESPONSE_DURATION))
      {
         pme->m_ToneData.wDuration = (uint16)ATOI(pszValue);
         FARF(MS, ("Our %s value is %d", OEMCFG_HAPTIC_RESPONSE_DURATION, pme->m_ToneData.wDuration));
      }
#endif

#ifdef FEATURE_HAS_MULTIPLE_DISPLAYS
      if (!STRICMP(pszName, OEMCFG_BAM_DISPLAYS))
      {
         pme->m_wEnabledDisplays = (uint16)ATOI(pszValue);
         FARF(MS, ("Our %s value is %d", OEMCFG_BAM_DISPLAYS, pme->m_wEnabledDisplays));
      }
#endif

#ifdef FEATURE_LESS_THAN_6_KEYS
      if (!STRICMP(pszName, OEMCFG_BAM_DISPLAY1_KEY_MAP))
      {
         GetKeyPressValues(pszValue, STRLEN(pszValue), &pme->m_wAltKeyUpList[0],
            &pme->m_wAltKeyDownList[0], &pme->m_wAltKeySelectList[0], &pme->m_wAltKeyClrList[0]);

         FARF(MS, ("Our Display1 key mappings are %d, %d, %d, %d", pme->m_wAltKeyUpList[0],
            pme->m_wAltKeyDownList[0], pme->m_wAltKeySelectList[0], pme->m_wAltKeyClrList[0]));
      }
      if (!STRICMP(pszName, OEMCFG_BAM_DISPLAY2_KEY_MAP))
      {
         GetKeyPressValues(pszValue, STRLEN(pszValue), &pme->m_wAltKeyUpList[1],
            &pme->m_wAltKeyDownList[1], &pme->m_wAltKeySelectList[1], &pme->m_wAltKeyClrList[1]);

         FARF(MS, ("Our Display2 key mappings are %d, %d, %d, %d", pme->m_wAltKeyUpList[1],
            pme->m_wAltKeyDownList[1], pme->m_wAltKeySelectList[1], pme->m_wAltKeyClrList[1]));
      }
      if (!STRICMP(pszName, OEMCFG_BAM_DISPLAY3_KEY_MAP))
      {
         GetKeyPressValues(pszValue, STRLEN(pszValue), &pme->m_wAltKeyUpList[2],
            &pme->m_wAltKeyDownList[2], &pme->m_wAltKeySelectList[2], &pme->m_wAltKeyClrList[2]);

         FARF(MS, ("Our Display3 key mappings are %d, %d, %d, %d", pme->m_wAltKeyUpList[2],
            pme->m_wAltKeyDownList[2], pme->m_wAltKeySelectList[2], pme->m_wAltKeyClrList[2]));
      }
      if (!STRICMP(pszName, OEMCFG_BAM_DISPLAY4_KEY_MAP))
      {
         GetKeyPressValues(pszValue, STRLEN(pszValue), &pme->m_wAltKeyUpList[3],
            &pme->m_wAltKeyDownList[3], &pme->m_wAltKeySelectList[3], &pme->m_wAltKeyClrList[3]);

         FARF(MS, ("Our Display4 key mappings are %d, %d, %d, %d", pme->m_wAltKeyUpList[3],
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


void MShop_AddSoftKey(MShop* pme)
{

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
   if(pme->m_bDispTouchEnabled)
   {
      uint16 resID[1];

      resID[0] = IDC_BACK;
      MShop_CreateSoftmenu(pme, resID, 1);
      MShop_SetMenuRect(pme);
      //if (!pme->m_bHasDPad)
      {
         IMENUCTL_SetActive(pme->m_pMenu, FALSE);
      }
      IMENUCTL_SetActive(pme->m_pSK, FALSE);
   }
#endif //FEATURE_TOUCHSCREEN_SUPPORT
}

#ifdef FEATURE_TOUCHSCREEN_SUPPORT

void MShop_SetMenuRect(MShop *pme)
{
   AEERect rSoft, rRect;
   int nMenuHt;
#ifdef APPINFO_IN_APPOPTIONS
   int nAppInfoHeight = NUM_APPINFO_LINES*(pme->m_cyFont + MARGIN) + 2*MARGIN;
#endif

   MEMSET(&rSoft, 0,  sizeof(AEERect));
   MEMSET(&rRect, 0,  sizeof(AEERect));

   // get menu and softkey rc
   if(pme->m_pMenu)
      IMENUCTL_GetRect(pme->m_pMenu, &rRect);
   if(pme->m_pSK)
      IMENUCTL_GetRect(pme->m_pSK, &rSoft);

   // Determine total area left for menuctl excluding  softmenu and the pixelpad
#ifdef APPINFO_IN_APPOPTIONS
   if(((rRect.y + rRect.dy) > (rSoft.y - TOUCH_PIXELPAD)) ||
      (((rRect.y + rRect.dy + nAppInfoHeight) > (rSoft.y - TOUCH_PIXELPAD)) &&
      (ST_APP_OPTIONS == pme->m_wState)))
#else
   if((rRect.y + rRect.dy) > (rSoft.y - TOUCH_PIXELPAD))
#endif
   {
      nMenuHt = (rSoft.y - TOUCH_PIXELPAD - rRect.y);
#ifdef APPINFO_IN_APPOPTIONS
      if (ST_APP_OPTIONS == pme->m_wState)
      {
         nMenuHt -= nAppInfoHeight;
      }
#endif

      SETAEERECT(&rRect, rRect.x, rRect.y, rRect.dx, nMenuHt);
      IMENUCTL_SetRect(pme->m_pMenu, &rRect);

      // clear previous drawing
      IDISPLAY_EraseRgn(pme->m_pIDisplay, 0, pme->m_rcMShopLogo.dy, pme->m_cx, pme->m_cy);
   }


   // If we are "redrawing the UI" and the dsplay is TouchEnabled and we are in the "InProgress"
   // state don't update the menu rect
   if (ST_IN_PROGRESS != pme->m_wState || !pme->m_bRedrawingUI || !pme->m_bDispTouchEnabled)
   {
      if(GetListViewMenuRect(pme->m_pMenu, &rRect))
      {
         IMENUCTL_SetRect(pme->m_pMenu, &rRect);
         IMENUCTL_SetActive(pme->m_pMenu, TRUE);
         IMENUCTL_Redraw(pme->m_pMenu);
      }
   }

   IMENUCTL_SetActive(pme->m_pSK, TRUE);
   IMENUCTL_Redraw(pme->m_pSK);


   // We are not erasing the Title area, so no need to re-set it.  The else case
   // needs to be fixed if we un-comment the code.
/*
   if ((ST_APP_OPTIONS == pme->m_wState) && (NULL != pme->m_pItem))
   {
      MShop_SetTitle(pme, BREWAPPMGRLS_RES_LOC, 0, pme->m_pItem->pszName);
   }
   else if((ST_SERVER != pme->m_wState) && (ST_VIEW_LOG != pme->m_wState))
   {
     // set title and logo
     MShop_SetTitle(pme, BREWAPPMGRLS_RES_LOC, IDC_MSHOP_CATALOG, NULL);
   }
*/
}

void MShop_CreateSoftmenu(MShop * pme, uint16* resID, int16 nCount)
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
   rRect.dx =  pme->m_cx;
   rRect.y = pme->m_cy - (TOUCH_SOFTMENU_HEIGHT + TOUCH_PIXELPAD );
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
      rRect.x =  0;
      rRect.dx =  pme->m_cx;
   }
   else
   {
      if (resID[0] == IDC_BACK || resID[0] == IDC_CANCEL)
      {
         rRect.x = pme->m_cx - nSoftmenuWidth;
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

   IDISPLAY_UpdateEx(pme->m_pIDisplay, TRUE);
 
}

#endif //FEATURE_TOUCHSCREEN_SUPPORT

#endif
