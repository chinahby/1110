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

        Copyright © 2000-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "OEMFeatures.h"
#include "MobileShop.h"
#include "AppMgrSettings.h"
#include "AEEFile.h"
#ifdef CUST_EDITION
#include "Appscommon.h"
#include "OEMFS.h"

static boolean MShop_CheckDiskSpace(void)
{
    uint32 dwFree = 0;
    int nRet = OEMFS_GetFreeSpaceEx(AEEFS_ROOT_DIR, NULL, &dwFree);
    DBGPRINTF("MShop_CheckDiskSpace %d %d",nRet,dwFree);
    if(nRet == SUCCESS)
    {
        return (dwFree >= 20480); //20k
    }
    return TRUE; //20k
}

static void MShop_DisCheckDispInfo(MShop * pme)
{
   uint16    wButtons[2];
   AEERect   rcm;
   AECHAR	 promptText[256];
   AECHAR	 formatString[256];
   if( ISHELL_LoadResString( pme->a.m_pIShell, 
				    APPMGRLS_RES_FILE,
				    IDS_FILE_MEMORY_FULL,
				    formatString,
				    sizeof(AECHAR) * 256 ) > 0 )
   {
	   WSPRINTF( promptText, sizeof(AECHAR) * 256, formatString, pme->m_szApp);
   }
   else
   {
	   WSTRCPY( promptText, pme->m_szApp);
   }
   
   MShop_Reset(pme,FALSE);      
   MEMSET(wButtons, 0, sizeof(wButtons));
   wButtons[0] = IDC_CLOSE;

   IMENUCTL_GetRect(pme->m_pSK, &rcm);
   MEMCPY( &pme->m_backupSoftkeyRect, &rcm, sizeof(AEERect));
   MShop_Prompt(pme, 
		   NULL, NULL, 
		   promptText, NULL, 
		   wButtons, wButtons, 
		   IDC_CLOSE, 
		   ST_CENTERTEXT|ST_NOSCROLL );
}
#endif
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
int MShopClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
{
   MShop *       pme = NULL;
   AEEDeviceInfo  di;

   *ppObj = NULL;

   // Create MShop Object
   if(!AEEApplet_New(sizeof(MShop), ClsId, pIShell,po,(IApplet**)ppObj,(AEEHANDLER)MShop_HandleEvent,(PFNFREEAPPDATA)MShop_Free))
      return(ENOMEMORY);

   pme = (MShop *)*ppObj;

   // Create IDownload & Control Interfaces
   if((ISHELL_CreateInstance(pIShell, AEECLSID_DOWNLOAD, (void **)&pme->m_pDownload) != AEE_SUCCESS) ||
      (ISHELL_CreateInstance(pIShell, AEECLSID_MENUCTL, (void **)&pme->m_pMenu) != AEE_SUCCESS) ||
      (ISHELL_CreateInstance(pIShell, AEECLSID_SOFTKEYCTL, (void **)&pme->m_pSK) != AEE_SUCCESS))
   {
      IAPPLET_Release((IApplet *)pme);
      *ppObj = NULL;
      return(EFAILED);
   }
   
   // Initialize Category Index
   pme->m_nCatIdx = -1;

   // Initilize Device Info
   ISHELL_GetDeviceInfo(pIShell, &di);
   pme->m_cx = di.cxScreen;
   pme->m_cy = di.cyScreen;
   pme->m_nColorDepth = di.nColorDepth;

#ifdef CUST_EDITION
   {
      int16 i;

      for(i = 0;i < 4;i++)
      {
         pme->m_menuSelItem[i] = 0;
      }
   }
#endif

   // Initilize Font
   pme->m_cyFont = IDISPLAY_GetFontMetrics(pme->a.m_pIDisplay, AEE_FONT_BOLD, NULL, NULL);

   // Initilize IDownload interface with callback function & context.
   // Callback is called when any activity occurs as a result of calls to the IDownload class.
   IDOWNLOAD_OnStatus(pme->m_pDownload, (PFNDLSTATUS)(MShop_Status),pme);

   // Set style & color for menu 
   SetMenuStyleAndColor(pme->m_pMenu, AEECLSID_MENUCTL, pme->m_nColorDepth);
   SetMenuStyleAndColor(pme->m_pSK, AEECLSID_SOFTKEYCTL, pme->m_nColorDepth);

   // Create State Machine by specifying state (index in state machine array) and
   // members of state handler struct
   SET_STATE(pme,ST_CATEGORIES,        MShop_Enum,NO_ENTRY,FALSE);
   SET_STATE(pme,ST_APP_OPTIONS,       MShop_AppOptionsDialog,ST_CATEGORIES,TRUE);
   SET_STATE(pme,ST_PURCHASE_CONFIRM,  MShop_ConfirmPurchaseDialog,ST_APP_OPTIONS,TRUE);
   SET_STATE(pme,ST_PURCHASE_EULA,     MShop_GetEULA,ST_APP_OPTIONS,FALSE);
   SET_STATE(pme,ST_PURCHASE,          MShop_Purchase,ST_APP_OPTIONS,FALSE);
   SET_STATE(pme,ST_RESTORE_APP,       MShop_Restore,NO_ENTRY,FALSE);
   SET_STATE(pme,ST_VIEW_LOG,          MShop_LogDialog,NO_ENTRY,TRUE);
   SET_STATE(pme,ST_SERVER,            MShop_SetServerDialog,NO_ENTRY,TRUE);
   SET_STATE(pme,ST_SEARCH,            MShop_SearchDialog,NO_ENTRY,TRUE);
   SET_STATE(pme,ST_CREDITBACK,        MShop_CreditBackDialog,NO_ENTRY,TRUE);
   SET_STATE(pme,ST_IN_PROGRESS,       MShop_InProgress,NO_ENTRY,TRUE);

   // Load MShopLogo
   pme->m_pMShopLogo = ISHELL_LoadResImage(pme->a.m_pIShell, APPMGRLN_RES_FILE, IDB_MSHOP_THUMB);

   if (pme->m_pMShopLogo)
   {
      AEEImageInfo ii;
      IIMAGE_GetInfo(pme->m_pMShopLogo, &ii);
      SETAEERECT(&pme->m_rcMShopLogo, 0, 0, ii.cx, ii.cy);
   }
   else
      SETAEERECT(&pme->m_rcMShopLogo, 0, 0, 0, pme->m_cyFont);

   if (OEM_UNDERLINE_MENU_TITLE)
      pme->m_rcMShopLogo.dy += 2;

   // Show Busy between UI states by default
   pme->m_bShowBusy = TRUE;

   return(AEE_SUCCESS);
}

/*===========================================================================

Releases MobileShop

===========================================================================*/
static void MShop_Free(MShop * pme)
{
   // Cancel pending timers
   ISHELL_CancelTimer(pme->a.m_pIShell, NULL, pme);

   // Free text/static controls
   FreeObj((void **)&pme->m_pStatic);
   FreeObj((void **)&pme->m_pText);

   // Free EULA buffer
   FreePtr((void **)&pme->m_pszEULA);
   pme->m_bGotEULA = FALSE;

   // Reset category list
   MShop_EnumCleanup(pme);

   // If MSHOP_PPP_LINGER is not defined as -1, make it default linger and
   // release IDownload interface
   if(pme->m_pDownload){ 
#if MSHOP_PPP_LINGER >= 0
      {
      INetMgr *   pn;

      if(!ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_NET, (void **)(&pn))){
         INETMGR_SetLinger(pn,(uint16)MSHOP_PPP_LINGER);         
         INETMGR_Release(pn);
      }
      }
#endif
      IDOWNLOAD_Release(pme->m_pDownload);
   }

   // Cancel timers used for adding items to log menu and free log buffer
   MShop_FreeLogBuff(pme);

   // Free menu controls
   FreeObj((void **)&pme->m_pMenu);
   FreeObj((void **)&pme->m_pSK);
   FreeObj((void **)&pme->m_pStatus);

   // Free Logo
   FreeObj((void **)&pme->m_pMShopLogo);
#ifdef CUST_EDITION
   if(pme->m_CurrStatus.pszText)
   {
      FREE(pme->m_CurrStatus.pszText);
   }
#endif
}

/*===========================================================================

Clears startup arguments for MobileShop

===========================================================================*/
static void MShop_ClearStartArgs(MShop * pme)
{
   pme->m_clsOpenAtExit = (AEECLSID)0;
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

   DBGPRINTF("MShop_HandleEvent 0x%x 0x%x 0x%x",eCode,wParam,dwParam);
   if((eCode == EVT_DIALOG_END) && (wParam == OEM_IME_DIALOG))
   {
       IDISPLAY_ClearScreen(pme->a.m_pIDisplay);
       MShop_SetTitle(pme, APPMGRLS_RES_FILE,(uint16)(IDS_SEARCH_TITLE),NULL);
       IMENUCTL_Redraw(pme->m_pSK);
       ITEXTCTL_SetActive(pme->m_pText, TRUE);
       IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
       return TRUE;
   }
   
   // Handle softkey events for a non text control softkey
   if(IMENUCTL_IsActive(pme->m_pSK) && IMENUCTL_HandleEvent(pme->m_pSK, eCode, wParam, dwParam))
      return(TRUE);
#else
   // Handle softkey events for a non text control softkey
   if(IMENUCTL_HandleEvent(pme->m_pSK, eCode, wParam, dwParam))
      return(TRUE);
#endif
   // Handle standard menu events
   if(IMENUCTL_HandleEvent(pme->m_pMenu, eCode, wParam, dwParam))
      return(TRUE);

   // Handle text control events
   if(pme->m_pText && ITEXTCTL_HandleEvent(pme->m_pText, eCode, wParam, dwParam))
   {
      // This is when Text Control returns from its mode/symbol selection menus
      if (ITEXTCTL_IsActive(pme->m_pText) && pme->m_bRepaint) {
         MShop_SetTitle(pme, APPMGRLS_RES_FILE, (uint16)((pme->m_wState == ST_SEARCH) ? IDS_SEARCH_TITLE : IDS_CREDITBACK_TITLE), NULL);

         // Reset repaint flag
         pme->m_bRepaint = FALSE;
      }
      else
      {
         // If Select key press made Text Control inactive and softkey active
         // Change Title as per first softkey item
         if ((wParam == AVK_SELECT) && IMENUCTL_IsActive(pme->m_pSK))
            MShop_SetTitle(pme, APPMGRLS_RES_FILE, (uint16)((pme->m_wState == ST_SEARCH) ? IDC_SEARCH_ANY : IDC_MSHOP_CREDITBACK), NULL);

         // If Clear key press made Text Control inactive and softkey active
         // Redraw Title
         if ((wParam == AVK_CLR) && IMENUCTL_IsActive(pme->m_pSK))
            MShop_SetTitle(pme, APPMGRLS_RES_FILE, IDC_INPUT_MODE, NULL);
#ifdef CUST_EDITION
         if ((wParam == AVK_SOFT2) && IMENUCTL_IsActive(pme->m_pSK))
            MShop_SetTitle(pme, APPMGRLS_RES_FILE, IDC_INPUT_MODE, NULL);
         // If up key press made Text Control active and softkey inactive
         
         if ((wParam == AVK_UP) && ITEXTCTL_IsActive(pme->m_pText))
            MShop_SetTitle(pme, APPMGRLS_RES_FILE, (uint16)((pme->m_wState == ST_SEARCH) ? IDS_SEARCH_TITLE : IDS_CREDITBACK_TITLE), NULL);
#endif
      }
      return(TRUE);
   }

   // Handle static control events
   if(pme->m_pStatic && ISTATIC_HandleEvent(pme->m_pStatic, eCode, wParam, dwParam))
      return(TRUE);

   switch (eCode) 
   {
      // Start MobileShop only if requested by App Manager
      case EVT_APP_START:
      {
         AEEAppStart * pStartArgs = (AEEAppStart*)dwParam;
#ifdef CUST_EDITION
         IDISPLAY_ClearScreen(pme->a.m_pIDisplay);
#endif
         if (pStartArgs && !pStartArgs->pszArgs)
            return FALSE;
         return TRUE;
      }

      // Fired by browser
      case EVT_APP_BROWSE_URL:
         pme->m_evtStart = eCode;
         MShop_Browse(pme,(const char *)dwParam);
         return(TRUE);
         
      // Stop MobileShop - Start any app we want to start when we close
      case EVT_APP_STOP:
         pme->m_evtStart = 0;
         if(pme->m_clsOpenAtExit)
            ISHELL_StartApplet(pme->a.m_pIShell, pme->m_clsOpenAtExit);
         return TRUE;

      // A dialog ended.  Show the last screen
      case EVT_DIALOG_END:
         return MShop_SetState(pme, PREV_ENTRY);

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
                  MShop_SetTitle(pme, APPMGRLS_RES_FILE, IDC_SEARCH_ANY, NULL);
                  break;

               // Credit Back Menu
               case IDC_MSHOP_CREDITBACK:
                  MShop_SetTitle(pme, APPMGRLS_RES_FILE, IDC_MSHOP_CREDITBACK, NULL);
                  break;

               default:
                  MShop_SetTitle(pme, APPMGRLS_RES_FILE, IDC_INPUT_MODE, NULL);
                  break;
            }
            return TRUE;
         }
         break; 

      case EVT_COMMAND:
         if(pme->m_bAIPrompt){
            pme->m_bAIPrompt = FALSE;     // Reset Auto-Install flag
            MShop_InProgress(pme);       // Display in progress graphics
            IDOWNLOAD_Continue(pme->m_pDownload,(boolean)(wParam == IDC_YES));
            return(TRUE);
         }

         switch(wParam){

            case IDC_OK:
               if (pme->m_bCarrierMessage)
               {  // Dismiss Carrier Message
                  pme->m_bCarrierMessage = FALSE;
                  return MShop_SetState(pme, pme->m_wState);
               }
               else
               {                  
                  if (pme->m_wState == ST_PURCHASE)
                  {
                     DBGPRINTF("MShop_HandleEvent IDC_OK");
                     MShop_ForceExit(pme);
                     return TRUE;
                  }
                  else
                     return MShop_SetState(pme, PREV_ENTRY);
               }

            // Auto Disable and Proceed with current state
            case IDC_PROCEED:
               IDOWNLOAD_AutoDisable(pme->m_pDownload, pme->m_iApp);
               return MShop_SetState(pme, pme->m_wState);

            // Perform Search
            case IDC_SEARCH_ANY:
               MShop_StartSearch(pme,TRUE);
               return TRUE;

            // Credit Back
            case IDC_MSHOP_CREDITBACK:
               MShop_StartCreditBack(pme);
               return TRUE;

            // Handling of close is state dependent
            case IDC_CLOSE:
               // Reset Information display flag
               pme->m_bDisplayInfo = FALSE;

               // displaying app options
               return MShop_SetState(pme, pme->m_wState);

            case IDC_NO:
               if (pme->m_wState == ST_PURCHASE)
               {
                  DBGPRINTF("MShop_HandleEvent IDC_NO");
                  MShop_ForceExit(pme);
                  return TRUE;
               }
               break;

            // Cancel
            case IDC_CANCEL:
               return MShop_SetState(pme,PREV_ENTRY);

            // Next on EULA dialog
            case IDC_NEXT:          
               return MShop_SetState(pme,ST_PURCHASE_EULA);

            // Accept on purchase confirmation dialog
            case IDC_ACCEPT:        
               return MShop_SetState(pme, ST_PURCHASE);

            // Processing of "Yes" is dependent on current state
            case IDC_YES:
               if (pme->m_wState == ST_PURCHASE)
                  return MShop_RunApp(pme);
               break;

            // Process Open/Info of app/category list
            case IDC_INFO:
               return MShop_ProcessMenuSel(pme, wParam);

            default:
               return MShop_ProcessDefaultCmd(pme, wParam, dwParam);
         } // Closes wParam Switch
         break;
        
      // Key Events
      case EVT_KEY:
         return MShop_ProcessKeyEvent(pme, wParam, dwParam); 
#ifdef CUST_EDITION
      case EVT_APP_SUSPEND:
         return TRUE;
      case EVT_APP_RESUME:
         IDISPLAY_SetClipRect(pme->a.m_pIDisplay, NULL);
         IDISPLAY_ClearScreen(pme->a.m_pIDisplay);
         if(pme->m_bDownloading)
         {
            pme->m_bDownloading = FALSE;
            MShop_Status(pme, &pme->m_CurrStatus);
         }
         else
         {
            MShop_SetState(pme, pme->m_wState);
         }
         IDISPLAY_Update(pme->a.m_pIDisplay);
         return TRUE;
#ifdef FEATURE_OEMOMH
      case EVT_OMH_PROMPT:
         //ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
         DBGPRINTF("MShop_HandleEvent EVT_OMH_PROMPT");
         MShop_ForceExit(pme);
         return TRUE;
#endif
#endif
   }
   return(FALSE);
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
#ifdef CUST_EDITION
   if ((pme->m_nCatIdx < 4) && (pme->m_nCatIdx >= 0)) //hou.chunfeng adds these @07.11.12
   {
      pme->m_menuSelItem[pme->m_nCatIdx] = IMENUCTL_GetSel(pme->m_pMenu);
   }
#endif
   // Server list in "Set Server" menu
   if(wParam >= IDC_SERVER){             
      MShop_SetServer(pme, wParam);
      return TRUE;
   }

   // App price list in app options menu
   if(wParam >= IDC_PRICE){               
#ifdef CUST_EDITION
      if( !MShop_CheckDiskSpace())
      {
         MShop_DisCheckDispInfo( pme);         
         IMENUCTL_SetRect( pme->m_pSK, &pme->m_backupSoftkeyRect);
         return TRUE;
      }
#endif

      pme->m_pPrice = (Price *)dwItem;
      pme->m_bDemo = (boolean)(wParam == IDC_DEMO);
      pme->m_iApp = pme->m_pItem->iID;
      return MShop_SetState(pme, ST_PURCHASE_CONFIRM);
   }

   // Items in category/app list 
   if(wParam >= IDC_ITEM)
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
#ifdef CUST_EDITION
      case AVK_SOFT2: //oem
#endif
      case AVK_CLR:
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
               pme->m_wState = pme->m_wLastState;
               if ((ST_PURCHASE == pme->m_wState) && (100 == pme->m_wPct)) {
                  DBGPRINTF("MShop_ProcessKeyEvent ST_IN_PROGRESS");
                  MShop_ForceExit(pme);
                  return TRUE;
               }
               else {
                  return MShop_SetState(pme, PREV_ENTRY);
               }

            case ST_APP_OPTIONS:
               if (pme->m_nCatIdx < 0)
               {
                  DBGPRINTF("MShop_ProcessKeyEvent ST_APP_OPTIONS");
                  MShop_ForceExit(pme);
                  return TRUE;
               }
               else
                  return MShop_SetState(pme, PREV_ENTRY);

            case ST_PURCHASE:
               DBGPRINTF("MShop_ProcessKeyEvent ST_PURCHASE");
               MShop_ForceExit(pme);
               return TRUE;

            default:
               return MShop_SetState(pme, PREV_ENTRY);
         }
   }
   return FALSE;
}

/*===========================================================================

// Sets TextCtl Menu Style

===========================================================================*/
static boolean MShop_SetTextctlMenuStyle(MShop* pme, uint32 dwParam)
{
   if (pme->m_wState == ST_SEARCH || pme->m_wState == ST_CREDITBACK)
   {
      AEERect rc;

      IMenuCtl* pMenu = (IMenuCtl*)dwParam;

      // Set Style & Color for menu
      SetMenuStyleAndColor(pMenu, AEECLSID_MENUCTL, pme->m_nColorDepth);

      // Erase Title
      IMENUCTL_SetTitle(pMenu, NULL, NULL, 0);

      // Specify Rect
      if (pme->m_rcMShopLogo.dy >= pme->m_cyFont)
      {
         SETAEERECT(&rc, 0, pme->m_rcMShopLogo.dy, pme->m_cx, pme->m_cy - pme->m_rcMShopLogo.dy);
      } 
      else
      {
         SETAEERECT(&rc, 0, pme->m_cyFont, pme->m_cx, pme->m_cy - pme->m_cyFont);
      }
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
   DBGPRINTF("MShop_SetState 0x%x",nState);
   if(nState == PREV_ENTRY){
      // Coming here means state is to be set to the back state

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
      if(nState == NO_ENTRY){
         DBGPRINTF("MShop_SetState NO_ENTRY");
         MShop_ForceExit(pme);
         return(TRUE);
      }

      // Set back state as current state.
      pme->m_wState = (int16)nState;
   }
   else
      pme->m_wState = (int16)nState; // Set specified state

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
               ISHELL_LoadResString(pme->a.m_pIShell, APPMGRLS_RES_FILE, wIDDef,(AECHAR *)pszMsg,MAX_STR_SIZE);
               if(WSTRSIZE(sz) + WSTRSIZE(pszMsg) < MAX_STR_SIZE)
                  WSTRCAT((AECHAR *)pszMsg,sz);
               dwProps |= ST_TEXTALLOC;
            }
            else
               pszMsg = (const AECHAR *)sz;
            }
      }
   }  

   wButtons[0] = IDC_OK;
   // Display single button prompt
   MShop_Prompt(pme, NULL, IDC_MSHOP, pszMsg, wIDDef, wButtons, wButtons, IDC_OK, dwProps|ST_NOSCROLL);
}

/*===========================================================================

RECALLED Response Dialog (General)

Called in response to DEVT_ADS_RECALL (app recall) event received from ADS.

===========================================================================*/
static void MShop_RecallMessage(MShop * pme, AECHAR * pszRecalled)
{
   uint16         wButtons[2];
   AECHAR*        pText;

   // Reset UI setting last state as current state
   MShop_Reset(pme,TRUE);

   // Display recall message in single button ("OK") prompt
   MEMSET(wButtons, 0, sizeof(wButtons));

   pText = FormatString(pme->a.m_pIShell, APPMGRLS_RES_FILE, IDS_RECALLED, pszRecalled);
   wButtons[0] = IDC_OK;
   MShop_Prompt(pme, NULL, IDC_MSHOP, pText, 0, wButtons, wButtons, IDC_OK, ST_TEXTALLOC|ST_NOSCROLL);

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
   AECHAR*        pText;

   // Reset Application ID
   pme->m_iApp = 0;

   // Reset UI setting last state as current state
   MShop_Reset(pme,TRUE);

   // Display verification failed in single button ("OK") prompt
   MEMSET(wButtons, 0, sizeof(wButtons));

   pText = FormatString(pme->a.m_pIShell, APPMGRLS_RES_FILE, IDS_VERIFICATION_FAILED, pszAppName);
   wButtons[0] = IDC_OK;
   MShop_Prompt(pme, NULL, IDC_MSHOP, pText, 0, wButtons, wButtons, IDC_OK, ST_TEXTALLOC|ST_NOSCROLL);

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
   if(pme->m_nCatIdx >= 0)
      id = pme->m_catList[pme->m_nCatIdx].iID;
   else{
      id = DL_CATEGORY_ROOT;
      pme->m_nCatIdx = 0;
      pme->m_catList[0].iID = id;
   }
   pme->m_nCatItems = 0;         // Reset number of category items at this level
   pme->m_bSearch = FALSE;       // Reset search

   if (pme->m_bUpgradeCheck)
      // Check for item upgrade
      IDOWNLOAD_CheckItemUpgrade(pme->m_pDownload, id, MShop_EnumCB,pme);
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
#ifdef CUST_EDITION
   if ((pme->m_nCatIdx < 4) && (pme->m_nCatIdx >= 0))
   {
      pme->m_menuSelItem[pme->m_nCatIdx] = 0;
   }
#endif
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
   if(pi->t != DLI_CATEGORY){
      pme->m_pItem = (DLItem *)pi; // Save app Info
      MShop_SetState(pme, ST_APP_OPTIONS);
      return;
   }

   if(pme->m_nCatIdx < MSHOP_MAX_DEPTH - 1){
      pme->m_nCatIdx++; // Increment Category Index

      pc = &pme->m_catList[pme->m_nCatIdx]; 

      pc->pszName = WSTRDUP(pi->pszName); // Save category name
      pc->iID = pi->id;                   // Save category ID

      // Enumerate
      MShop_Enum(pme);
   }
}

/*===========================================================================

 Display item info

===========================================================================*/
static void MShop_ShowItemInfo(MShop * pme, DLEnumItem * pi)
{
   uint16         wButtons[2];

   // Reset Display
   MShop_Reset(pme,FALSE);

   // Set Information Display Flag TRUE
   pme->m_bDisplayInfo = TRUE;

   // Setup prompt structure
   MEMSET(wButtons, 0, sizeof(wButtons));
   wButtons[0] = IDC_CLOSE;

   // If there is a description for this application, Show it.
   if(pi->pszDescription){
      DLItemSize is;
      AECHAR szSize[10];
      AECHAR * pNewText = NULL;
      AECHAR * psz = NULL;
      AECHAR * pszText = NULL;
      AECHAR * pText = NULL;
      AECHAR * pTitle = NULL;

      // Set title & text for the prompt
      pszText = WSTRRCHR(pi->pszDescription,(AECHAR)'|');

      if(pszText){
         // Description is setup as <title>|<text>
         *pszText = 0;
         pText = pszText + 1;
         pTitle = pi->pszDescription;
      }
      else{
         pText    = pi->pszDescription;
         pTitle   = pi->pszName;
      }

      // Get Text on heap
      pNewText = AppendWStr(pNewText, pText, '\n');

      // Append Size Info
      IDOWNLOAD_GetSizeEx(pme->m_pDownload, pi->id, &is);
      FormatMem(is.dwFSInstall, szSize, FALSE);  // Get in 5K. 5MB, 5.4MB format
         
      // Get formatted string for free memory
      psz = FormatString(pme->a.m_pIShell, APPMGRLS_RES_FILE, IDS_APP_SIZE_PREVIEW, szSize);
         
      // Append psz
      if (psz)
      {
         pNewText = AppendWStr(pNewText, psz, '\n');
         FREE(psz);
      }
      MShop_Prompt(pme, pTitle, 0, pNewText, 0, wButtons, wButtons, IDC_CLOSE, ST_TEXTALLOC|ST_NOSCROLL);

      if(pszText)
         *pszText = (AECHAR)'|';
   }
   else
      MShop_Prompt(pme, NULL, IDC_INFO, NULL, IDS_NOAPPINFO_AVAILABLE, wButtons, wButtons, IDC_CLOSE, ST_NOSCROLL);
}

/*===========================================================================

 Callback from the enumeration engine.  Fill the items as they come in.

===========================================================================*/
static void MShop_EnumCB(void * pcxt, int nErr, DLEnumItem * pe)
{
   MShop *    pme = (MShop *)pcxt;
   CtlAddItem  ai;

   // On first item, clear display and set previous state as
   // current state.
   if(!pme->m_nCatItems)
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
      if (pme->m_bSearch && !(IDOWNLOAD_GetADSCapabilities(pme->m_pDownload) & ADS_CAP_SEARCH))
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

   if(!pe){
      // Coming here means item returned is NULL. Do following and get out of here.

      // If no category items found, display no categories found.
      // Otherwise just get out of here.
      if(!pme->m_nCatItems)
      {
         uint16 wButtons[2];

         // Prompt structure with information that app is restored and OK button to dismiss
         MEMSET(wButtons, 0, sizeof(wButtons));
         wButtons[0] = IDC_OK;

         MShop_Prompt(pme, NULL, IDC_MSHOP, NULL, IDS_NO_CATEGORIES, wButtons, wButtons, IDC_OK, ST_NOSCROLL);
      }
      else
      {         
         if (IMENUCTL_IsActive(pme->m_pMenu) == FALSE)
         {
            // Set menu active
            IMENUCTL_SetActive(pme->m_pMenu, TRUE);

            // Update UI
            IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
         }
      }
      return;
   }

   // On first item, if this is search, set search result as menu title
   // Otherwise set category name is menu title. If there is no category name,
   // Set catalog as menu title
   if (!pme->m_nCatItems)
   {
      if(pme->m_bSearch)
         MShop_SetTitle(pme, APPMGRLS_RES_FILE, IDS_SEARCH_RESULTS, NULL);
      else{
         AECHAR * psz;

         // Set the title
         psz = pme->m_catList[pme->m_nCatIdx].pszName;
         if(psz)
            MShop_SetTitle(pme, NULL, 0, psz);
         else
            MShop_SetTitle(pme, APPMGRLS_RES_FILE, IDC_MSHOP_CATALOG, NULL);
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
#ifdef CUST_EDITION
   if((pme->m_menuSelItem[pme->m_nCatIdx] != 0)&&(pme->m_wState == ST_CATEGORIES)&&
      (ai.wItemID == pme->m_menuSelItem[pme->m_nCatIdx]))
   {
      IMENUCTL_SetSel(pme->m_pMenu,pme->m_menuSelItem[pme->m_nCatIdx]);
   }
#endif
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
         // Set menu active
         IMENUCTL_SetActive(pme->m_pMenu, TRUE);

         // Update UI
         IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
      }
   }
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

   pi = pme->m_pItem;

   // Set app name as title
   MShop_SetTitle(pme, NULL, 0, pi->pszName);

   // If there is no upgrade option and demo option is available, add that in menu
   if(pi->UpgradeOption.lt == LT_NONE){
      if(pi->DemoOption.lt != LT_NONE){
        MShop_AddOptionMenuItem(pme, IDC_DEMO, pi, &pi->DemoOption.Price); 
        DBGPRINTF("MShop_AppOptionsDialog  IDC_DEMO");
      }
   }

   // If upgrade option is available, add that in menu
   if(pi->UpgradeOption.lt != LT_NONE){
      MShop_AddSimpleOptionMenuItem(pme, IDC_UPGRADE, IDS_EXP_UPGRADE, &pi->UpgradeOption.Price, IDB_MS_UPGRADE);
      DBGPRINTF("MShop_AppOptionsDialog  IDC_UPGRADE");
   }

   // If subscription option is available, add that in menu
   if(pi->SubscriptionOption.lt != LT_NONE){
      MShop_AddSimpleOptionMenuItem(pme, IDC_SUBSCRIPTION, IDS_EXP_SUBSCRIPTION, &pi->SubscriptionOption.Price, IDB_MS_SUBSCRIBE);
      DBGPRINTF("MShop_AppOptionsDialog  IDC_SUBSCRIPTION");
   }

   // If purchase option is available, add all of the purchase prices in the menu
   if(pi->PurchaseOption.lt != LT_NONE){
      for(i = 0, p = pi->PurchaseOption.pPrices; i < pi->PurchaseOption.nPrices; i++, p++){
         DBGPRINTF("MShop_AppOptionsDialog %d",wID);
         wID = MShop_AddOptionMenuItem(pme, wID, pi, p);
      }
   }

   // Add Info if information is available
   if (pi->pszDescription)
   {
      CtlAddItem ai;

      // Initialize ai
      MEMSET(&ai, 0, sizeof(ai));

      ai.pszResText  = APPMGRLS_RES_FILE;
      ai.pszResImage = APPMGRLN_RES_FILE;
      ai.wFont = AEE_FONT_NORMAL;
      ai.wText = IDC_INFO;
      ai.wImage = IDB_MS_INFO;
      ai.wItemID = IDC_INFO;
      ai.dwData = (uint32)pi;

      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   }

   // Activate Menu
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);

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
   psz = FillPrice(pme->a.m_pIShell, APPMGRLS_RES_FILE, p->pszPrice, pszOption, MAX_STR_SIZE);

   nLeft = MAX_STR_SIZE - WSTRSIZE(pszOption);

   // Prepage for getting expiration string
   dwNow = GET_SECONDS();
   if(lt == LT_DAYS && pItem->currentLicense.lt == LT_DATE){
      dwExpire = pItem->currentLicense.dwExpire;
      if(dwExpire > dwNow)
         dwNow = dwExpire;
   }

   // Add "<price info> - < expiration string for non expired items> in menu and
   // increase item ID for next items.
   if(GetExpireString(pme->a.m_pIShell, APPMGRLS_RES_FILE, dwNow, lt, p->dwValue, psz, nLeft, FALSE, TRUE))
   {
      CtlAddItem ai;

      // Initialize ai
      MEMSET(&ai, 0, sizeof(ai));

      ai.pText = pszOption;
      ai.pszResImage = APPMGRLN_RES_FILE;
      ai.wFont = AEE_FONT_NORMAL;
      ai.wImage = (uint16)((wItemID == IDC_DEMO) ? IDB_MS_DEMO : IDB_MS_PURCHASE);
      ai.wItemID = wItemID++;
      ai.dwData = (uint32)p;

      IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   }

   FREE(pszOption);
   return(wItemID);
}

/*===========================================================================

Adds "FREE - <wIDText>" or "<price info> - <wIDText>" to the app options menu
and returns ItemID for added menu item. This is used for "Subscription" and
"Upgrade"

===========================================================================*/
static uint16 MShop_AddSimpleOptionMenuItem(MShop * pme, uint16 wItemID, uint16 wTextID, Price * p, uint16 wIconID)
{
   AECHAR *    psz;
   AECHAR *    pszOption;
   int         nLeft;
   CtlAddItem  ai;

   pszOption = (AECHAR *)MALLOC(MAX_STR_SIZE);
   if(!pszOption)
      return(wItemID);

   psz = FillPrice(pme->a.m_pIShell,APPMGRLS_RES_FILE, p->pszPrice, pszOption, MAX_STR_SIZE);

   nLeft = MAX_STR_SIZE - WSTRSIZE(pszOption);

   ISHELL_LoadResString(pme->a.m_pIShell, APPMGRLS_RES_FILE, wTextID, psz, nLeft);

   // Initialize ai
   MEMSET(&ai, 0, sizeof(ai));

   ai.pText = pszOption;
   ai.pszResImage = APPMGRLN_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;
   ai.wImage = wIconID;
   ai.wItemID = wItemID++;
   ai.dwData = (uint32)p;

   IMENUCTL_AddItemEx(pme->m_pMenu, &ai);
   
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
         pai->pszResImage = APPMGRLN_RES_FILE;
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
      WSTRCPY(pszDest, pszPrice);

   pszDest += WSTRLEN(pszDest);

   *(pszDest++) = (AECHAR)' ';
   *(pszDest++) = (AECHAR)'-';
   *(pszDest++) = (AECHAR)' ';
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

   // Reset UI while setting last state as current state
   MShop_Reset(pme,TRUE);

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
   DBGPRINTF("MShop_EULACB %d",nErr);
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
   DBGPRINTF("MShop_GetEULA %d %d %d %d",pi->bEULA,pi->currentLicense.pt,pme->m_bGotEULA,pme->m_pszEULA);
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
State handler for ST_PURCHASE_CONFIRM.

===========================================================================*/
static boolean MShop_ConfirmPurchaseDialog(MShop * pme)
{
   uint16         wButtons[3];
   DLItem   *     pi = pme->m_pItem;
   uint16         wID;
   AECHAR   *     pText = NULL;    

   // First - See if the app even fits
   if(!MShop_CheckDownload(pme, pi->iID, pi->pszName))
      return(FALSE);      
      
   // If it's a demo, then just send them to the EULA
   if(pme->m_bDemo || IsFree(pme->m_pPrice->pszPrice)){
      MShop_SetState(pme, ST_PURCHASE_EULA);
      return(TRUE);
   }

   // We would display "You would be charged %s for this app
   wID = IDS_PURCHASE_CONFIRM;

   // Fetch EULA if there is EULA associated with this app and we havn't got it.
   if(pi->bEULA && !pme->m_bGotEULA)
      IDOWNLOAD_GetEULA(pme->m_pDownload,pme->m_iApp,MShop_EULACB,pme);

   // If app is already installed, we would display "This app is already installed.  
   // You will be charged  %s for the additional purchase.  Are you sure?"
   if(pi->currentLicense.pt != PT_NONE && pi->currentLicense.pt != PT_DEMO)
      wID = IDS_ALREADY;

   // Prepare a purchase confirmation prompt with "Next" and "Cancel" buttons. Make "Cancel"
   // default.
   MEMSET(wButtons, 0, sizeof(wButtons));
   pText = FormatString(pme->a.m_pIShell,APPMGRLS_RES_FILE, wID, pme->m_pPrice->pszPrice);
   if(pText){
      wButtons[0] = IDC_NEXT;
      wButtons[1] = IDC_CANCEL;
      MShop_Prompt(pme, pi->pszName, 0, pText, 0, wButtons, wButtons, wButtons[0], ST_TEXTALLOC|ST_NOSCROLL);
      return(TRUE);
   }
   return(FALSE);
}


/*===========================================================================

State handler for ST_PURCHASE state

===========================================================================*/
static boolean MShop_Purchase(MShop * pme)
{
   DBGPRINTF("MShop_Purchase 0x%x",pme->m_pItem);
   if(pme->m_pItem){
      // Display progress graphics
      MShop_InProgress(pme);
      // Acquire the application
      IDOWNLOAD_Acquire(pme->m_pDownload, pme->m_pItem->iID,pme->m_pPrice->dwHandle,MShop_PurchaseCB,pme);   
   }
   return(TRUE);
}

/*===========================================================================

State handler for ST_RESTORE_APP

===========================================================================*/
static boolean MShop_Restore(MShop * pme)
{
   if (pme->m_iApp)
   {   
      AppModInfo * pmi = IDOWNLOAD_GetModInfo(pme->m_pDownload, pme->m_iApp);
      if (pmi)
      {
         boolean bDownload;

         // Load App Name
         AECHAR* pszName = LoadAppNames(pme->a.m_pIShell, pmi->apps, pmi->nApps);

         // Check for RAM
         bDownload = MShop_CheckDownload(pme, pme->m_iApp, pszName);

         // Free Non Null name buffer
         FreePtr((void**)&pszName);

         if (bDownload)
         {
            // Display progress graphics
            MShop_InProgress(pme);

            // Restore
            IDOWNLOAD_Restore(pme->m_pDownload, pme->m_iApp,MShop_RestoredCB,pme);
            return(TRUE);
         }
      }    
   }
   return FALSE;
}

/*===========================================================================

IDOWNLOAD_Restore callback

===========================================================================*/
static void MShop_RestoredCB(void * pcxt,int nErr, void * pUnused)
{
   MShop * pme = (MShop *)pcxt;
   
   // Don't Show Busy
   pme->m_bShowBusy = FALSE;

   // Reset UI setting last state as current state
   MShop_Reset(pme,TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);

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
   AppModInfo *   pmi;

   // Don't Show Busy
   pme->m_bShowBusy = FALSE;
   DBGPRINTF("MShop_PurchaseCB %d",nErr);
   // Reset UI while setting last state as current state
   MShop_Reset(pme,TRUE);

   if(nErr)
      MShop_Failed(pme,IDS_DOWNLOAD_FAILED,nErr,pMsg); // Download failed
   else{
      uint16 wButtons[3];

      MEMSET(wButtons, 0, sizeof(wButtons));

      // Cache the download ID
      pme->m_iApp = pme->m_pItem->iID;

      // Cache the name
      WSTRNCOPYN(pme->m_szApp,sizeof(pme->m_szApp) / sizeof(AECHAR),pme->m_pItem->pszName, -1);

      // Get module information
      pmi = IDOWNLOAD_GetModInfo(pme->m_pDownload, pme->m_iApp);

      if (pmi && pmi->nApps)
      {
         IFileMgr * pIfm = NULL;
         IFile * pIFile = NULL;
         int nPos;

         if ((ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void**)&pIfm) == AEE_SUCCESS) &&
            ((pIFile = IFILEMGR_OpenFile(pIfm, CONFIG_FILE, _OFM_READWRITE)) != NULL) &&
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
                  SPRINTF(pBuffer + nBuffer, "0x%x,0x%x;", pmi->apps[i].cls, pme->m_iApp);
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
                  if (IFILE_Write(pIFile, szLine, STRLEN(szLine)) != STRLEN(szLine))
                  {
                     IFILE_Truncate(pIFile, nPos);
                     break;
                  }
               }
            }
         }

         FreeObj((void**)&pIfm);
         FreeObj((void**)&pIFile);
      }
      if(pmi && pmi->nApps && !(pmi->apps[0].wFlags & AFLAG_HIDDEN)) {            
         //Display installed message with chance to run it "Yes"/"No" prompt
         wButtons[0] = IDC_YES;
         wButtons[1] = IDC_NO;
         MShop_Prompt(pme, pme->m_szApp, 0, NULL, IDS_INSTALLED_APP, wButtons, wButtons, IDC_YES, ST_NOSCROLL);
      }
      else {
         wButtons[0] = IDC_OK;
         // App hidden - Display installed message with "OK" button to dismiss
         MShop_Prompt(pme, pme->m_szApp, 0, NULL, IDS_INSTALLED, wButtons, wButtons, IDC_OK, ST_NOSCROLL);
      }
   }
}

/*===========================================================================

Starts an installed application (closes MobileShop)

===========================================================================*/
static boolean MShop_RunApp(MShop * pme)
{
   AppModInfo * pmi;

   // If no applet pending for start at close of MobileShop, set current
   // applet as one
   if(!pme->m_clsOpenAtExit){
      pmi = IDOWNLOAD_GetModInfo(pme->m_pDownload, pme->m_iApp);
      if(pmi && pmi->nApps)
         pme->m_clsOpenAtExit = pmi->apps[0].cls;
      else
         MShop_ClearStartArgs(pme);
   }
   DBGPRINTF("MShop_RunApp");
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
   uint16  nSKHeight, x, y;
   AECHAR* szTextBuf = NULL;

   // Create Static Control
   if (ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_STATIC, (void**)&pme->m_pStatic))
      return;
   
   // Initialize to 0 to make ARM compiler happy
   nSKHeight = x = y = 0;
   
   if (!szText)
      szTextBuf = (AECHAR *)MALLOC(MAX_STR_SIZE);
   else
      szTextBuf = (AECHAR *)szText;

   // Add items in soft key
   while (*wButtons && *wItemIDs)
   {
      IMENUCTL_AddItem(pme->m_pSK, APPMGRLS_RES_FILE, *wButtons, *wItemIDs, NULL, 0);
      wButtons++;
      wItemIDs++;
   }

   // Get Softkey Size
   IMENUCTL_GetRect(pme->m_pSK, &rcm);

   // SoftKey Height
   nSKHeight = rcm.dy;

   // Set Static Control Size
   if (szTitle || wIDTitle)
   {
      if (pme->m_rcMShopLogo.dy >= pme->m_cyFont)
      {
         SETAEERECT(&rc, 0, pme->m_rcMShopLogo.dy, pme->m_cx, rcm.y - pme->m_rcMShopLogo.dy);
      } 
      else 
      {
         SETAEERECT(&rc, 0, pme->m_cyFont, pme->m_cx, rcm.y - pme->m_cyFont);
      }
   }
   else
   {
      x = (pme->m_cx - (pme->m_cx*DISPLAY_AREA_PERCENT/100))/2;
      y = (pme->m_cy - (pme->m_cy*DISPLAY_AREA_PERCENT/100))/2;

      // Draw Border
      SETAEERECT(&rcm, x-1, y-1, pme->m_cx - 2*(x-1), pme->m_cy - 2*(y-1));
      IDISPLAY_EraseRect(pme->a.m_pIDisplay, &rcm);
      IDISPLAY_DrawRect(pme->a.m_pIDisplay, &rcm, RGB_BLACK, 0, IDF_RECT_FRAME);

      // Set Softkey Size
      SETAEERECT(&rcm, x, pme->m_cy - y - nSKHeight, pme->m_cx - 2*x, nSKHeight);
      IMENUCTL_SetRect(pme->m_pSK, &rcm);

      // Calculate Static Control Size
      SETAEERECT(&rc, x + MARGIN, y + MARGIN, pme->m_cx - 2*(x + MARGIN), pme->m_cy - 2*(y + MARGIN) - nSKHeight);
   }
   ISTATIC_SetRect(pme->m_pStatic, &rc);

   // Set Static Control Properties
   if (dwProps)
      ISTATIC_SetProperties(pme->m_pStatic, dwProps);

   // Set Static Control Text and display it
   if (!szText)
      ISHELL_LoadResString(pme->a.m_pIShell, APPMGRLS_RES_FILE, wIDText, szTextBuf, MAX_STR_SIZE);

   ISTATIC_SetText(pme->m_pStatic, NULL, szTextBuf, 0, AEE_FONT_NORMAL);
   ISTATIC_SetActive(pme->m_pStatic, TRUE);
   ISTATIC_Redraw(pme->m_pStatic);

   // Make Softkey Active
   IMENUCTL_SetSel(pme->m_pSK, wDefID);
   IMENUCTL_SetActive(pme->m_pSK, TRUE);

   if (szTitle || wIDTitle)
      MShop_SetTitle(pme, APPMGRLS_RES_FILE, wIDTitle, szTitle);

   if (!szText)
      FREE(szTextBuf);

   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
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
         IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
      }
   }
   else
      pme->m_bShowBusy = TRUE;

   // Reset AI Prompt Flag
   pme->m_bAIPrompt = FALSE;

   // End active dialog
   ISHELL_EndDialog(pme->a.m_pIShell);

   // Free Text and Static Controls
   FreeObj((void **)&pme->m_pText);
   FreeObj((void **)&pme->m_pStatic);

   if(bRestoreState)
      pme->m_wState = pme->m_wLastState; // Set last state as current state

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
   if (pme->m_rcMShopLogo.dy >= pme->m_cyFont) 
   {
      SETAEERECT(&rc, 0, pme->m_rcMShopLogo.dy, pme->m_cx, pme->m_cy - pme->m_rcMShopLogo.dy);
   } 
   else 
   {
      SETAEERECT(&rc, 0, pme->m_cyFont, pme->m_cx, pme->m_cy - pme->m_cyFont);
   }
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
   IDISPLAY_EraseRect(pme->a.m_pIDisplay, &rc);
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
      pme->m_wLastState    = pme->m_wState;
      pme->m_wState        = ST_IN_PROGRESS;
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
   pi = ISHELL_LoadResImage(pme->a.m_pIShell, APPMGRLN_RES_FILE, (uint16)(pme->m_bDownloading ? IDB_DOWNLOADING : IDB_CONNECTING));

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

   // Setup status area rect
   cyHist = pme->m_cyFont;

   // Measure "Downloading ..." to set status rectangle width
   ISHELL_LoadResString(pme->a.m_pIShell, APPMGRLS_RES_FILE, IDS_DOWNLOADING, szBuff, sizeof(szBuff));
   cxStatus = IDISPLAY_MeasureText(pme->a.m_pIDisplay, AEE_FONT_NORMAL, szBuff);

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
   pme->m_wPct = 0;

   // Draw frame around status area
   rc = pme->m_rcStatus;
   IDISPLAY_DrawFrame(pme->a.m_pIDisplay, &rc, AEE_FT_RAISED, CLR_SYS_WIN);

   // Load "Connecting ..." and display that in histogram area.
   ISHELL_LoadResString(pme->a.m_pIShell, APPMGRLS_RES_FILE, (uint16)(pme->m_bDownloading ? (pme->m_wLastState ? (pme->m_bVerify ? IDS_VERIFYING : IDS_DOWNLOADING ) : IDS_CANCELLING): IDS_CONNECTING), szBuff, sizeof(szBuff));
   IDISPLAY_DrawText(pme->a.m_pIDisplay,AEE_FONT_NORMAL, szBuff, -1, 0, 0, &pme->m_rcHist, IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);

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
   
#ifdef CUST_EDITION
   if(pme->m_CurrStatus.pszText)
   {
      FREE(pme->m_CurrStatus.pszText);
   }
   MEMCPY(&pme->m_CurrStatus,ps,sizeof(pme->m_CurrStatus));
   if(ps->pszText)
   {
      pme->m_CurrStatus.pszText = WSTRDUP(ps->pszText);
   }
#endif

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
               DrawHist(pme->a.m_pIDisplay, &rc, wPct);  // Draw Histogram with new % complete
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
               ISHELL_LoadResString(pme->a.m_pIShell, APPMGRLS_RES_FILE, IDS_READING, szBuff, sizeof(szBuff));
               IDISPLAY_DrawText(pme->a.m_pIDisplay,AEE_FONT_NORMAL, szBuff, -1, 0, 0, &pme->m_rcHist, IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|IDF_RECT_FILL);
            }
         }
         if(bUpdate)
            IDISPLAY_Update(pme->a.m_pIDisplay);
         break;
   }
   return(TRUE);
}

/*===========================================================================

State handler for ST_VIEW_LOG

===========================================================================*/
static boolean MShop_LogDialog(MShop * pme)
{
   AEERect  rc, rcm;

   // Display OK prompt
   IMENUCTL_AddItem(pme->m_pSK, APPMGRLS_RES_FILE, IDC_OK, IDC_OK, 0, 0);
   IMENUCTL_SetActive(pme->m_pSK, TRUE);

   // Get softkey height for setting main menu size
   IMENUCTL_GetRect(pme->m_pSK, &rcm);

   // Set main menu size
   SETAEERECT(&rc, 0, 0, rcm.dx, rcm.y);
   IDISPLAY_EraseRect(pme->a.m_pIDisplay, &rc);
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

      // Activate Menu
      IMENUCTL_SetActive(pme->m_pMenu, TRUE);

      // Update UI
      IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);

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
   ISHELL_LoadResString(pme->a.m_pIShell, APPMGRLS_RES_FILE, wID, szType, sizeof(szType));
   
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

   // Get Text Control Pointer
   pt = pme->m_pText;

   // Create text control if necessary
   if(!pt) {
      if (ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_TEXTCTL,(void **)(&pt)) != AEE_SUCCESS)
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
   MShop_SetTitle(pme, APPMGRLS_RES_FILE,(uint16)(bSearch ? IDS_SEARCH_TITLE : IDS_CREDITBACK_TITLE),NULL);
   ITEXTCTL_SetMaxSize(pt, MAX_STR_CHARS);

   if (bSearch)
      // Setup softkey menu for this text control with any/mode options
      IMENUCTL_AddItem(pme->m_pSK, APPMGRLS_RES_FILE, IDS_ANY, IDC_SEARCH_ANY, NULL, NULL);
   else
      // Setup softkey menu for this text control with ok/mode options
      IMENUCTL_AddItem(pme->m_pSK, APPMGRLS_RES_FILE, IDC_OK, IDC_MSHOP_CREDITBACK, NULL, NULL);

   // Get softkey size
   IMENUCTL_GetRect(pme->m_pSK, &rcm);

   if (bSearch)
      // Set Text Control Properties      
      ITEXTCTL_SetProperties(pt, TP_FRAME|TP_MULTILINE|TP_T9_MODE);
   else
      // Set Text Control Properties      
      ITEXTCTL_SetProperties(pt, TP_FRAME|TP_MULTILINE);

   // Set Text Control Size
   if (pme->m_rcMShopLogo.dy >= pme->m_cyFont) 
   {
      SETAEERECT(&rc, MARGIN, pme->m_rcMShopLogo.dy + MARGIN, pme->m_cx - 2*MARGIN, pme->m_cyFont*((pme->m_cy - pme->m_rcMShopLogo.dy - rcm.dy - MARGIN)/pme->m_cyFont));
   } 
   else 
   {
      SETAEERECT(&rc, MARGIN, pme->m_cyFont + MARGIN, pme->m_cx - 2*MARGIN, pme->m_cyFont*((pme->m_cy - pme->m_cyFont - rcm.dy - MARGIN)/pme->m_cyFont));
   }


   ITEXTCTL_SetRect(pt, &rc);
 
   // Set Softkey to Text Control
   ITEXTCTL_SetSoftKeyMenu(pme->m_pText, pme->m_pSK);

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

State handler for ST_SEARCH

===========================================================================*/
static boolean MShop_SearchDialog(MShop * pme)
{
   return MShop_DisplayTextInputForm(pme, TRUE);
}

/*===========================================================================

Performs search using specified criteria

===========================================================================*/
static void MShop_StartSearch(MShop * pme, boolean bSearchAny)
{
   AECHAR * psz;

   if(!pme->m_pText)
      return;

   // Read text control buffer
   psz = ITEXTCTL_GetTextPtr(pme->m_pText);

   //Duplicate the string 'coz MShop_Reset() frees the original ptr
   if(psz)
      psz = WSTRDUP(psz);

   if(psz){
      // Reset UI
      MShop_Reset(pme,FALSE);

      // Display progress praphics
      MShop_InProgress(pme);

      pme->m_nCatIdx = 0;     // Reset category index
      pme->m_nCatItems = 0;   // Reset category items
      pme->m_bSearch = TRUE;  // Set search flag

      // Perform search
      IDOWNLOAD_Search(pme->m_pDownload,psz,(DLSearchType)(bSearchAny ? DL_SEARCH_ANY : DL_SEARCH_ALL),MShop_EnumCB,pme);
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
Returns TRUE if there is enough space. Displays a single button prompt otherwise.

===========================================================================*/
static boolean MShop_CheckDownload(MShop * pme, DLITEMID iID, AECHAR * pszName)
{
   uint32         dwSize,dwFree;
   AECHAR         szFree[10], szSize[10];
   DLItemSize     is;
   int            nErr;
   AECHAR *       psz = NULL;
   boolean        bProceed = FALSE;

   nErr = IDOWNLOAD_GetSizeEx(pme->m_pDownload, iID, &is);

   switch(nErr){
      case SUCCESS:
         return(TRUE);                 // No problem

      case ENOMEMORY:
         {
            dwSize = is.dwEstRAMRequired; // RAM Required
            dwFree = is.dwRAMAvail;       // RAM available
         
            FormatMem(dwSize,szSize,FALSE);  // Get in 5K. 5MB, 5.4MB format
            FormatMem(dwFree,szFree,FALSE);  // Get in 5K. 5MB, 5.4MB format

            // Check if szSize is same as szFree. This is to prevent 
            // the error such as "Your app needs 10K. You only have 10K".
            while(!WSTRICMP(szSize,szFree) && dwFree) {
                dwFree -= ONE_K;                // Ensure that Free is less than available
                FormatMem(dwFree,szFree,FALSE); // Get new string
            }
            // Get formatted string for free memory
            psz = FormatStringEx(pme->a.m_pIShell,APPMGRLS_RES_FILE,IDS_APP_RAM_SIZE,szSize,szFree);
         }
         break;

      case EFSFULL:
         {
            int nCount;
            DLDisableEntry * pDisableList = IDOWNLOAD_GetAutoDisableList(pme->m_pDownload, iID, 0, &nCount, &nErr);
            if (nErr)
            {
               dwSize = is.dwFSInstall;      // File Memory Required
               dwFree = is.dwFSAvail;        // File memory Available
               FormatMem(dwSize,szSize,FALSE);  // Get in 5K. 5MB, 5.4MB format
               FormatMem(dwFree,szFree,FALSE);  // Get in 5K. 5MB, 5.4MB format

               // Check if szSize is same as szFree. This is to prevent 
               // the error such as "Your app needs 10K. You only have 10K".
               while(!WSTRICMP(szSize,szFree) && dwFree) {
                   dwFree -= ONE_K;                // Ensure that Free is less than available
                   FormatMem(dwFree,szFree,FALSE); // Get new string
               }
               // Get formatted string for free memory
               psz = FormatStringEx(pme->a.m_pIShell,APPMGRLS_RES_FILE,IDS_APP_EFS_SIZE,szSize,szFree);
            }
            else
            {
               int i = 0;
               AppModInfo * pai = NULL;
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
                     pai = IDOWNLOAD_GetModInfo(pme->m_pDownload, pDisableList->iID);
                     if (pai)
                        pTemp = LoadAppNames(pme->a.m_pIShell, pai->apps, pai->nApps);
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
                  psz = FormatString(pme->a.m_pIShell,APPMGRLS_RES_FILE,IDS_FILE_MEMORY_FULL,pApps);
                  FREE(pApps);
               }
            }
         }
         break;
   
      case EOUTOFNODES:
         {            
            AECHAR szFormat[10];
            dwSize = is.nEstFilesRequired;// Files Required
            dwFree = is.nFilesAvail;      // Files Available
            STRTOWSTR("%d",szFormat,sizeof(szFormat));
            WSPRINTF(szFree,sizeof(szFree),szFormat,dwFree);   // Get #count string
            WSPRINTF(szSize,sizeof(szSize),szFormat,dwSize);   // Get #count string
            // Get formatted string for free files
            psz = FormatStringEx(pme->a.m_pIShell,APPMGRLS_RES_FILE,IDS_APP_FILES_NEEDED,szSize,szFree);
         }
         break;

      default:
         return(FALSE);                // Can't download
   }
   
   if(psz){
      // Display a single button prompt

      uint16 wButtons[2];
      MEMSET(wButtons, 0, sizeof(wButtons));
      wButtons[0] = IDC_OK;
      MShop_Reset(pme,FALSE);
      if (!bProceed)
         MShop_Prompt(pme, pszName, 0, psz, 0, wButtons, wButtons, IDC_OK, ST_TEXTALLOC|ST_NOSCROLL);
      else
      {
         uint16 wItemIDs[2];
         MEMSET(wItemIDs, 0, sizeof(wItemIDs));
         wItemIDs[0] = IDC_PROCEED;
         MShop_Prompt(pme, pszName, 0, psz, 0, wButtons, wItemIDs, IDC_PROCEED, ST_TEXTALLOC|ST_NOSCROLL);
      }
   }
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

   DBGPRINTF("MShop_SetServer");
   MShop_ForceExit(pme);
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
      return(FALSE);

   // Selection
   nSel = IDC_SERVER + nIdx;

   // Reset UI
   MShop_Reset(pme,FALSE);

   // Set Title
   IMENUCTL_SetTitle(pme->m_pMenu, APPMGRLS_RES_FILE, IDS_SELECT, NULL);

   // Initialize CtlAddItem
   MEMSET(&ai, 0, sizeof(ai));
   ai.pszResImage = APPMGRLN_RES_FILE;
   ai.wFont = AEE_FONT_NORMAL;

   // Set main menu size to entire display area
   SETAEERECT(&rc, 0, 0, pme->m_cx, pme->m_cy);
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
         
   // Set index of current server as selection
   IMENUCTL_SetSel(pme->m_pMenu, nSel);
               
   // Activate Menu
   IMENUCTL_SetActive(pme->m_pMenu, TRUE);

   // Update UI
   IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);

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

   pme->m_pItem = (DLItem *)pItem;        // Set as current item    
   pme->m_iApp = pItem->id;               // Set as current app ID
   MShop_SetState(pme, ST_APP_OPTIONS);  // Display App Options
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
   DBGPRINTF("MShop_RemoveItemInfoCB");
   // Exit Otherwise
   MShop_ForceExit(pme);
}

/*===========================================================================

  MobileShop URL Format

  ccamshop:ItemID=<ItemID>
  ccamshop:Search=Search String
  ccamshop:Restore=<ItemID>

===========================================================================*/
static void MShop_Browse(MShop * pme, const char * url)
{
   DLITEMID       iID                     = 0;
   char*          pszTemp                 = NULL;
   char*          pszArg                  = NULL;
   char*          pszVal                  = NULL;
   char*          pszNext                 = NULL;
   AEELicenseType lt                      = 0;
   BrowserAction  ba                      = NONE;
   AppModInfo*    pmi                     = NULL;
   char*          pszReceivedSecretString = NULL;
   char*          pszSecretString         = NULL;
   uint32         bReturn                 = AEE_SUCCESS;

   // No URL - Nothing to do
   if(!url)
      return;

   // Duplicate the URL as we will modify it
   pszTemp = STRDUP(url);
   if(!pszTemp)
      return;

// Todd Davis 2006/06/22 It looks like the following loop will not work for multiple commands.
//    Only the last command will be processed by the subsequent switch statement. This code
//    should be restructured into function calls for each command. The switch statement should
//    be chopped up into multiple function bodies.

   // Process all of the arguments
   pszArg = STRCHR(pszTemp,':'); // Get URL devoid of cmshop
   while(pszArg){
      pszArg++; // Ignore ':', '0'

      pszNext = STRCHR(pszArg,';'); // Get next in ';' separated list

      if (pszNext) {
         *pszNext = 0;  // Overwrite ';' with NULL leaving pszArg as NULL terminated 
                        // single entry string "<action>=<value>" string 
      }

      pszVal = STRCHR(pszArg,'=');  // Get value

      if(pszVal){
         *pszVal = 0;               // Overwrite '=' leaving pszArg as NULL terminated "<action>" string
         pszVal++;                  // Ignore '='
      }

      if(!STRICMP(pszArg,"ItemID"))
         ba = ITEMID;
      else if (!STRICMP(pszArg, "Restore"))
         ba = RESTORE;
      else if (!STRICMP(pszArg, "Catalog"))
         ba = CATALOG;
      else if (!STRICMP(pszArg, "Search"))
         ba = SEARCH;
      else if (!STRICMP(pszArg, "CreditBack"))
         ba = CREDITBACK;
      else if (!STRICMP(pszArg, "Log"))
         ba = LOG;
      else if (!STRICMP(pszArg, "Server"))
         ba = SERVER;
      else if (!STRICMP(pszArg, "Remove"))
         ba = REMOVE;
      else if (!STRICMP(pszArg, "UpgradeCheck"))
         ba = UPGRADECHECK;
      else if (!STRICMP(pszArg, "Verify"))
         ba = VERIFY;
      else
         ba = NONE;

      pszArg = pszNext;
   }

   switch (ba)
   {
      case CATALOG:
         // Set Carrier Message TRUE so if there is a carrier message
         // its displayed
         pme->m_bCarrierMessage = TRUE;
         MShop_SetState(pme, ST_CATEGORIES);   // Display Catalog
         break;

      case RESTORE:
         iID = STRTOUL(pszVal,NULL,10);
         pme->m_iApp = iID;
         MShop_SetState(pme, ST_RESTORE_APP);     // Restore App
         break;

      case SEARCH:
         MShop_SetState(pme, ST_SEARCH);       // Provide search option
         break;

      case LOG:
         MShop_SetState(pme, ST_VIEW_LOG);     // Provide log view dialog
         break;

      case SERVER:
         MShop_SetState(pme, ST_SERVER);       // Provide "set server" dialog
         break;

      case REMOVE:
         if (pszVal == NULL) {
            break;
         }

         pszReceivedSecretString = STRCHR(pszVal, ',');
         if (pszReceivedSecretString == NULL) {
            break;
         }
         *pszReceivedSecretString = '\0';
         ++pszReceivedSecretString;

         pszSecretString = AllocSecretString(pme->a.m_pIShell, FALSE);
         if ((pszSecretString == NULL) || (STRCMP(pszReceivedSecretString, pszSecretString) != 0)) {
            FREEIF(pszSecretString);
            pszSecretString = AllocSecretString(pme->a.m_pIShell, TRUE);
            if ((pszSecretString == NULL) || (STRCMP(pszReceivedSecretString, pszSecretString) != 0)) {
               FREEIF(pszSecretString);
               break;
            }
         }
         FREEIF(pszSecretString);

         iID = STRTOUL(pszVal,NULL,10);
         pmi = IDOWNLOAD_GetModInfo(pme->m_pDownload, iID);
         lt = (pmi ? pmi->li.pt : PT_NONE);
         bReturn = IDOWNLOAD_Delete(pme->m_pDownload, iID, TRUE);
         if (bReturn == AEE_SUCCESS)
         {
            if(lt != PT_SUBSCRIPTION){
               DBGPRINTF("MShop_Browse %d",lt);
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
            if (bReturn != EBADSID){
               DBGPRINTF("MShop_Browse %d",bReturn);
               MShop_ForceExit(pme);
            }
            else
            {
               uint16 wButtons[2];

               MEMSET(wButtons, 0, sizeof(wButtons));

               wButtons[0] = IDC_OK;
               
               // Display single button prompt
               MShop_Prompt(pme, NULL, 0, NULL, IDS_LICENSE_NOT_FOUND, wButtons, wButtons, wButtons[0], ST_NOSCROLL);
            }
         }
         break;

      case UPGRADECHECK:
         iID = STRTOUL(pszVal,NULL,10);

         // Prepare for catalog enumeration
         pme->m_bUpgradeCheck = TRUE;
         pme->m_nCatIdx = 0;
         pme->m_catList[pme->m_nCatIdx].iID = iID;
         MShop_SetState(pme, ST_CATEGORIES);
         break;

      case ITEMID:
         iID = STRTOUL(pszVal,NULL,10);
         pme->m_iApp = iID;
         MShop_Reset(pme,FALSE);               // Reset UI
         MShop_InProgress(pme);                // Display progress graphics
         IDOWNLOAD_GetItemInfo(pme->m_pDownload, iID, MShop_ItemInfoCB,pme);          // Get item info
         break;

      case CREDITBACK:
         MShop_SetState(pme, ST_CREDITBACK);   // Provide Credit Back Dialog
         break;

      case VERIFY:
         iID = STRTOUL(pszVal,NULL,10);
         pme->m_iApp = iID;
         pme->m_bVerify = TRUE;
         MShop_SetState(pme, ST_CATEGORIES);   // Display Catalog
         break;
   }

   FREEIF(pszTemp);
}

/*==========================================================================

   Draws Title String

===========================================================================*/
void MShop_SetTitle(MShop* pme, const char* pszRes, uint16 nTitleID, const AECHAR* pTitle)
{
   AEERect rc;
   AECHAR * pTitleBuf = (AECHAR*)pTitle;   

   // Clear Background
   if (pme->m_rcMShopLogo.dy >= pme->m_cyFont) 
   {
      SETAEERECT(&rc, 0, 0, pme->m_cx, pme->m_rcMShopLogo.dy); 
   }
   else 
   {
      SETAEERECT(&rc, 0, 0, pme->m_cx, pme->m_cyFont);
   }
   IDISPLAY_EraseRect(pme->a.m_pIDisplay, &rc);

   // Draw MobileShop
   if (pme->m_pMShopLogo)
   {      
      // Set Image Background Transparent
      IIMAGE_SetParm(pme->m_pMShopLogo, IPARM_ROP, AEE_RO_TRANSPARENT, 0);

      // If image height greater or equal to font height, start image at top of window 
      if (pme->m_rcMShopLogo.dy >= pme->m_cyFont) 
      {
         IIMAGE_Draw(pme->m_pMShopLogo, 0, 0);
      }

      // Otherwise, align bottom of image with bottom of text 
      else 
      {
         IIMAGE_Draw(pme->m_pMShopLogo, 0, pme->m_cyFont - pme->m_rcMShopLogo.dy); 
      }
   }

   if (!pTitle)
   {
      if ((pTitleBuf = (AECHAR *)MALLOC(MAX_STR_SIZE)) != NULL)
         ISHELL_LoadResString(pme->a.m_pIShell, pszRes, nTitleID, pTitleBuf, MAX_STR_SIZE);
   }      
   
   if (OEM_UNDERLINE_MENU_TITLE)   
   {      
      // If image height greater or equal to font height and extra space, align bottom of text with bottom of image
      if (pme->m_rcMShopLogo.dy - 2 >= pme->m_cyFont) 
      {
         IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_BOLD, pTitleBuf, -1, pme->m_rcMShopLogo.dx + 1, pme->m_rcMShopLogo.dy - pme->m_cyFont - 2, NULL, 0);
         IDISPLAY_DrawHLine(pme->a.m_pIDisplay, 0, pme->m_rcMShopLogo.dy - 1, pme->m_cx);
      }

      // Otherwise, start text at top of window 
      else 
      {
         IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_BOLD, pTitleBuf, -1, pme->m_rcMShopLogo.dx + 1, 0, NULL, 0);
         IDISPLAY_DrawHLine(pme->a.m_pIDisplay, 0, pme->m_cyFont + 1, pme->m_cx);
      }      
   }
   else
   {
      // If image height greater or equal to font height and extra space, align bottom of text with bottom of image
      if (pme->m_rcMShopLogo.dy >= pme->m_cyFont) 
      {
         IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_BOLD, pTitleBuf, -1, pme->m_rcMShopLogo.dx + 1, pme->m_rcMShopLogo.dy - pme->m_cyFont, NULL, 0);   
      }

      // Otherwise, start text at top of window 
      else 
      {
          IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_BOLD, pTitleBuf, -1, pme->m_rcMShopLogo.dx + 1, 0, NULL, 0);
      }
      
   }

   if (!pTitle)
      FREE(pTitleBuf);

   // Update UI
   IDISPLAY_Update(pme->a.m_pIDisplay);
}
