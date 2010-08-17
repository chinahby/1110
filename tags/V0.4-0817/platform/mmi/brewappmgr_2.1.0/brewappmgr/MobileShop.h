#ifndef MSHOP_H
#define MSHOP_H
/*======================================================

                    IMPORTANT NOTICE

The contents of this file are provided under legal agreement
between QUALCOMM Incorporated and selected licensed carrier
and handset OEM partners. Under the terms of that agreement, 
this file MUST NOT be distributed in full or in part to anyone
not specifically authorized by QUALCOMM Incorporated.

FILE:  MobileShop.h

SERVICES:  Mobile Download Applet

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

        Copyright © 2000-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEAppGen.h"
#include "AEEDownload.h"
#include "AEEStdLib.h"
#include "AEEText.h"
#include "AEEFlip.h"
#include "BrewAppMgrUtils.h"
#include "BrewAppMgrSettings.h"

#if defined USE_TT_FONTS
#include "AEETypeface.h"
#endif // USE_TT_FONTS

#define SCRATCH_SPACE   0x1000

#define MSHOP_MAX_DEPTH 10

typedef struct _MShop  MShop;

// Enum BrowserAction - specifies action to be performed in response to EVT_APP_BROWSE_URL
typedef enum _BrowserAction
{
   NONE,
   CATALOG,
#ifndef SEARCH_FROM_CATALOG
   SEARCH,
#endif // SEARCH_FROM_CATALOG
   CREDITBACK,
   LOG,
   SERVER,
   ITEMID,
   RESTORE,
   UPGRADECHECK,
   REMOVE,
   VERIFY
} BrowserAction;

// SilentUpgradeCheck info
typedef struct _SilentUpgradeCheckInfo
{
   DLITEMID iID;          // Item ID being checked
   char * pszURL;         // URL to post back
    IShell    *    pIShell;   // IShell interface to use
} SilentUpgradeCheckInfo;

// Acquire info
typedef struct _AcquireInfo
{
   DLITEMID iID;
   DLPRICEID priceHandle;
} AcquireInfo;

// Category Struct
typedef struct
{
   DLITEMID iID;        // ID of this directory
   AECHAR * pszName;    // Name of this directory...
} MShopCat;

// State Machine Handler Function
typedef boolean   (*PFNREQ)(MShop * pme);

// State Handler. State Machine is an array of state handlers. 
// Array Index specifies state where as these structs specify
// state handler

typedef struct 
{
   PFNREQ   pfnReq;  // Pointer to Handler Function
   int16    nBack;   // State to go back
   boolean  bClear;  // Clear Display before invoking state handler
} MShopState;

typedef enum _SwitchDisplaysResult
{
   DISPLAY_SWITCH_FAILED,
   DISPLAY_SWITCH_SUCCEEDED,
   DISPLAY_SWITCH_WOULD_EXIT
} SwitchDisplaysResult;

// MobileShop States
#define ST_CATEGORIES         0
#define ST_APP_OPTIONS        1
#define ST_CHECK_SPACE        2
#define ST_PURCHASE_CONFIRM   3
#define ST_PURCHASE_EULA      4
#define ST_PURCHASE           5
#define ST_PURCHASE_RESUME    6
#define ST_RESTORE_APP        7
#define ST_VIEW_LOG           8
#define ST_SERVER             9
#define ST_SEARCH             10
#define ST_CREDITBACK         11
#define ST_IN_PROGRESS        12
#define ST_SEARCHSTRING       13
#define ST_SEARCH_CMD         14
#define ST_MAX                (ST_SEARCH_CMD + 1)

struct _MShop
{
   AEEApplet         a;

   IDisplay*         m_pIDisplay;         // Currently active display unless display2 is non-null
                                          // then it is the display that the normal controls are
                                          // pointing to, but may not be active
   IDisplay*         m_pIDisplay2;        // Display used to show "downloading" text on front display
                                          // if the user closes the display while downloading
                                          // and we continue downloading.

   IMenuCtl    *     m_pMenu;             // Standard Menu
   IMenuCtl    *     m_pSK;               // Softkey Menu
   IDownload   *     m_pDownload;         // IDownload Interface
#ifdef USE_IDL2
   IDownload2  *     m_pDownload2;        // IDownload2 Interface
#endif
   INetMgr     *     m_pINetMgr;

#ifdef FEATURE_UPGRADE_AUTO_PROMPT
   IUpgradeMgr  *    m_pUm;               // pointer to UpgradeMgr
#endif // FEATURE_UPGRADE_AUTO_PROMPT
   IImage      *     m_pStatus;           // Download Progress Status Image
   IStatic     *     m_pStatic;           // Static Control
   IStatic     *     m_pStaticDlInfo;     // Static Control to display "downloading" text on fron display
   ITextCtl    *     m_pText;             // Text Control - Search Menu

   uint16            m_cyFont;            // Bold Font Height
   uint16            m_cx;                // Screen Width
   uint16            m_cy;                // Screen Height
   uint16            m_nColorDepth;       // Color Depth

   AEERect           m_rcStatus;          // Status Area
   AEERect           m_rcHist;            // Histogram Area
   int16             m_wState;            // Key that got us here...
   int16             m_wLastState;        // Last State
   uint16            m_wPct;              // % download complete

   boolean           m_bInitStatus;       // MShop_InitStatus called flag
   boolean           m_bDownloading;      // MShop_Downloading
   boolean           m_bGotStatus;        // Status flag - Indicates if got info from ADS
   boolean           m_bDemo;             // Currently viewed app is demo
   boolean           m_bSubscription;     // Currently viewed app is subscription app
   boolean           m_bSearch;           // Search flag - Set true for search
   boolean           m_bSearchAny;        // Search any(==1) or all(==0)
   boolean           m_bUpgradeCheck;     // Upgrade Check flag - Set true for upgrade check
   boolean           m_bGotEULA;          // Got EULA of currently viewed app
   boolean           m_bAIPrompt;         // Auto install flag
   boolean           m_bRepaint;          // Repaint TextControl banner and title
   boolean           m_bCarrierMessage;   // Display Carrier Message
   boolean           m_bDisplayInfo;      // Display Information Flag
#ifdef MSHOP_CMD_ITEMINFO
   boolean           m_bDisplayInfoOnly;  // Display Information Only Flag
#endif
   boolean           m_bShowBusy;         // Flag to indicate if MobileShop needs to show busy
                                          // in between UI states
   boolean           m_bVerify;           // Set true for application verification
#if defined USE_TT_FONTS
   boolean           m_bNormalFontSaved;  // has normal font been saved?
   boolean           m_bBoldFontSaved;    // has bold font been saved?
#endif // USE_TT_FONTS

#ifdef FEATURE_HAPTIC_SUPPORT
   ISound*           m_pHapticResponse;      // ISound object for haptic support on actionable items
   AEESoundToneData  m_ToneData;             // Tone data for haptic response
#endif

#ifdef FEATURE_CANCELONCLOSE
   boolean           m_bCancelOnClose;    // Cancel operations and exit on flip or slide close
#endif
#ifdef FEATURE_HAS_MULTIPLE_DISPLAYS
   boolean           m_bHasMultipleDisplays;    // Does the phone have multiple displays?
#endif
   boolean           m_bConfirmDownload;  // Confirm Download even if it is free
   boolean           m_bExitOnCancel;     // Exit MobileShop when the user presses Cancel

   AEEEvent          m_evtStart;          // Event that started the MobileShop

   DLItem *          m_pItem;             // Currently viewed app's info
   Price *           m_pPrice;            // Currently viewed app's price info
   AECHAR            m_szApp[32];         // Currently viewed app's name
   DLITEMID          m_iApp;              // Currently viewed app's download ID

   AEECLSID          m_clsOpenAtExit;     // Start this applet at EVT_APP_STOP

   AECHAR *          m_pszEULA;           // Cached EULA of currently viewed app

#if defined(MSHOP_CMD_SEARCH_STRING) || defined(MSHOP_CMD_CATEGORY)
   AECHAR      *     m_pszSearch;         // Search string for non-interactive search
   AECHAR      *     m_pszTitle;          // Title string for results
#endif

#ifdef MSHOP_CMD_ACQUIRE
   AcquireInfo *     m_pAcquireInfo;
#endif // MSHOP_CMD_ACQUIRE

   AECHAR      *     m_pLogBuff;          // Log Buffer
   uint16            m_wLogItemID;        // Log Buffer ItemID

   MShopCat          m_catList[MSHOP_MAX_DEPTH];   // Category List
   int               m_nCatIdx;                    // Category List Index
   int               m_nCatItems;                  // # of category items
   MShopState        m_states[ST_MAX];             // State Machine
   IImage*           m_pMShopLogo;                 // MShop Logo
   AEERect           m_rcMShopLogo;                // MShop Logo Rect
#if defined USE_TT_FONTS
   IHFont       *    m_pSaveNormalFont;            // saved normal font
   IHFont       *    m_pSaveBoldFont;              // saved bold font
#endif // USE_TT_FONTS

                                                // For a better understanding of the possible values
                                                // values for these three vars see the comments
                                                // in BrewAppMgrUtils.h for the macro IS_DISPLAY_BIT_SET
   uint16            m_wTouchDisplays;          // Bit field representing which displays are Touch enabled
   uint16            m_wEnabledDisplays;        // Bit field representing whith displays we are not to run on
   uint16            m_wNotEnoughKeys;          // Displays that don't have all the keys necessary that we need to
                                                // work around.
   boolean           m_bHasDPad;                // True if the display has all of up/down/left/right

#ifdef FEATURE_LESS_THAN_6_KEYS
   boolean           m_bUseAltKeys;             // We have less than 6 keys, so use an alt key mapping
   uint16            m_wAltKeyUp;               // Alternate key code for up
   uint16            m_wAltKeyDown;             // Alternate key code for down
   uint16            m_wAltKeySelect;           // Alternate key code for Select
   uint16            m_wAltKeyClr;              // Alternate key code for Clr
   uint16            m_wAltKeyUpList[4];        // The keys, if any, that map to up for each display
   uint16            m_wAltKeyDownList[4];      // The keys, if any, that map to up for each display
   uint16            m_wAltKeySelectList[4];    // The keys, if any, that map to up for each display
   uint16            m_wAltKeyClrList[4];       // The keys, if any, that map to up for each display
#endif // FEATURE_LESS_THAN_6_KEYS

   boolean           m_bSuspending;             // We are in the act of Suspending
   boolean           m_bResuming;               // We are in the act of Resuming
   boolean           m_bFlipOpenWhenSuspended;  // The state of the flip when we were suspended
   boolean           m_bExitAfterDownload;      // When true MobileShop is still downloading
                                                // event though the user closed the flip/slider and we should
                                                // have exited.

   boolean           m_bDispTouchEnabled;		// notify whether current display is touch enabled or not	
   boolean           m_bRedrawingUI;         // This is a flag telling us that we are in the process
                                             // of redrawing the UI (normally from an EVT_FLIP or an
                                             // EVT_SCR_ROTATE message).  So any functions
                                             // that would normally only be called when state changed
                                             // can be called to update their display only, if this is TRUE.

#ifdef FEATURE_SCREEN_ROTATE
   AEEScrOrientation m_orentDisplayDefault;  // The default orentation for the currently active display
#endif //FEATURE_SCREEN_ROTATE

#ifdef FEATURE_DISABLE_SS_DURING_DOWNLOAD
   AEECLSID m_clsScreenSaver;                // The system screen saver which we turned off for download
#endif // FEATURE_DISABLE_SS_DURING_DOWNLOAD

#ifdef FEATURE_SILENT_UPGRADE_CHECK
   DLITEMID iID;                             // Item ID being checked
   char * pszURL;                            // URL to post back, as given to us
   char * m_pszUrlPostAtExit;                // Filled in URL to post at exit
#endif // FEATURE_SILENT_UPGRADE_CHECK

   // Below are Vars for saving some values sent to the controls
   // This allows us to better reproduce the control state
   // when we switch screens
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   AEECallback       *cbDisplayEnabled;
   AEECallback       m_cbUpdateUI;
   boolean           m_bPromptIsDisplayed;   // This is a flag telling us that the AppMgr_Prompt() display is up
                                             // and that it is waiting for user response.  If we are telling
                                             // the user something in a timed message, we don't set this
   char*             m_pTitleResFile;
   uint16            m_wTitleResId;
   AECHAR*           m_pTitleText;
   char*             m_pMenuResFile;
   uint16            m_wMenuResID;
   AECHAR*           m_pMenuText;
   AECHAR*           m_pStaticTitle;
   AECHAR*           m_pStaticText;
   AEEFont           m_pStaticFntTitle;
   AEEFont           m_pStaticFntText;
   boolean           m_bStaticSizeToFitCalled;
   AEERect           m_rcStaticSizeToFit;
   AECHAR*           m_pPromptTitle;
   uint16            m_wPromptTitleID;
   AECHAR*           m_pPromptText;
   uint16            m_wPromptTextID;
   uint16*           m_pPromptButtons;
   uint16*           m_pPromptItemIDs;
   uint16            m_wPromptDefID;
   uint32            m_dwPromptProps;
   uint16            m_wExDisplayResId;
#endif // FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   IFileMgr        * m_pIfm;
};

//
// Status definitions...
//

#define HIST_SEP                 5        // Status - Histogram separator
#define FRAME_SEP                2        // Status - separator space

#define STATUS_DELAY             100      // Delay before status animation pops up...
#define STATUS_ANIMATION_FRAMES  5        // Animation frames (BMP only)
#define STATUS_ANIMATION_RATE    250      // Animation rate (milliseconds)

#define IDC_LOG               10000
//
// Events/Keys
//
#define IDC_ITEM              11000

// 11/12/01 (Upgrade - Discount the installed stuff)

#define IDC_PRICE             12000
#define IDC_DEMO              IDC_PRICE
#define IDC_UPGRADE           (IDC_PRICE + 1)
#define IDC_SUBSCRIPTION      (IDC_PRICE + 2)
#define IDC_PURCHASE          (IDC_PRICE + 3)

// 11/12/01 End

#define IDC_SERVER            13000

// Function Prototypes
static boolean       MShop_AppOptionsDialog(MShop * pme);
static boolean       MShop_LogDialog(MShop * pme);
static boolean       MShop_HandleEvent(MShop * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static void          MShop_Free(MShop * pme);
static boolean       MShop_Status(MShop * pme,DownloadStatus * ps);
static void          MShop_Reset(MShop * pme,boolean bRestoreState);
static boolean       MShop_InProgress(MShop * pme);
static void          MShop_Prompt(MShop * pme, const AECHAR* szTitle, uint16 wIDTitle, const AECHAR* szText, uint16 wIDText, uint16* wButtons, uint16* wItemIDs, uint16 wDefID, uint32 dwProps);
static void          MShop_Failed(MShop * pme, uint16 wIDFailed, int nErr,const AECHAR * pszError);
static void          MShop_RecallMessage(MShop * pme, AECHAR * pszRecalled);
static void          MShop_CarrierMessage(MShop * pme, AECHAR * pszText);
static void          MShop_InitStatus(MShop * pme);
static boolean       MShop_SetState(MShop * pme, int nState);

static boolean       MShop_SearchDialog(MShop * pme);
#ifdef MSHOP_CMD_SEARCH_STRING
static boolean       MShop_SearchString(MShop * pme);
#endif
static void          MShop_StartSearch(MShop * pme);

static boolean       MShop_CreditBackDialog(MShop * pme);
static void          MShop_StartCreditBack(MShop * pme);

static void          MShop_EnumCleanup(MShop * pme);
static boolean       MShop_Enum(MShop * pme);
static void          MShop_EnumCB(void * pcxt, int nErr, DLEnumItem * pi);
static void          MShop_CatUp(MShop * pme);
static void          MShop_CatDown(MShop * pme, DLEnumItem * piOwner);
static void          MShop_ShowItemInfo(MShop * pme, DLEnumItem * pi);
#ifdef APPINFO_IN_APPOPTIONS
static void          MShop_ShowItemInfoInAppOptions(MShop* pme, DLEnumItem* pi, AEERect* infoRect);
#endif //APPINFO_IN_APPOPTIONS
static AECHAR *      MShop_CreateItemInfoString(MShop * pme, DLEnumItem * pi);

static boolean       MShop_GetEULA(MShop * pme);
static void          MShop_EULACB(void * pcxt, int nErr, const AECHAR * pszMsg);

static boolean       MShop_CheckSpaceDialog(MShop * pme);
static boolean       MShop_ConfirmPurchaseDialog(MShop * pme);

static boolean       MShop_Purchase(MShop * pme);
static void          MShop_PurchaseCB(void * pcxt,int nErr,void * pMsg);

static boolean       MShop_PurchaseResume(MShop * pme);

static boolean       MShop_Withdraw(MShop * pme);
static void          MShop_WithdrawCB(void * pcxt,int nErr,void * pUnused);

static boolean       MShop_RunApp(MShop * pme);

static boolean       MShop_CheckDownloadToPhone(MShop* pme, DLITEMID iID, AECHAR* pszName);
static uint16        MShop_AddOptionMenuItem(MShop * pme,uint16 wItemID, DLItem * pItem, Price * p);
#if defined(MSHOP_OPTION_MENU_ICONS)
static uint16        MShop_AddSimpleOptionMenuItem(MShop * pme, uint16 wItemID, uint16 wTextID, Price * p, uint16 wIconID);
#else
static uint16        MShop_AddSimpleOptionMenuItem(MShop * pme, uint16 wItemID, uint16 wTextID, Price * p);
#endif

static void          MShop_AddNextLogItem(MShop * pme);
static void          MShop_FreeLogBuff(MShop * pme);
static void          MShop_ItemInfoCB(void * pcxt, int nErr, DLEnumItem * pItem);
static void          MShop_ItemInfoResumeCB(void * pcxt, int nErr, DLEnumItem * pItem);
#ifdef MSHOP_CMD_ACQUIRE
static void MShop_AcquireItemInfoCB(void * pcxt, int nErr, DLEnumItem * pItem);
#endif

static boolean       MShop_SetServerDialog(MShop * pme);
static void          MShop_SetServer(MShop * pme, uint16 wID);

static void          MShop_Browse(MShop * pme, const char * url);
static void          MShop_BrowseRemove(MShop* pme, const char* pszVal);

static boolean       MShop_ProcessMenuSel(MShop* pme, uint16 wParam);
static boolean       MShop_ProcessDefaultCmd(MShop* pme, uint16 wParam, uint32 dwParam);
static boolean       MShop_ProcessKeyEvent(MShop* pme, uint16 wParam, uint32 dwParam); 
static boolean       MShop_ProcessSuspend(MShop* pme);
static boolean       MShop_ProcessResume(MShop* pme);
static boolean       MShop_SetStateAfterResume(MShop* pme);
static boolean       MShop_SetTextctlMenuStyle(MShop* pme, uint32 dwParam);
static boolean       MShop_DisplayTextInputForm(MShop * pme, boolean bSearch);
static void          MShop_SetTitle(MShop* pme, const char* pszRes, uint16 nTitleID, const AECHAR* pTitle);
static boolean       MShop_ValidateCmshopCmd(MShop * pme, char * pszArg);
   
static boolean       IsFree(AECHAR * pszPrice);
static AECHAR *      FillPrice(IShell * ps,const char * pszRes, AECHAR * pszPrice, AECHAR * pszDest,int nSize);
static void          CheckIcon(IShell * ps, uint16 wIcon, CtlAddItem * pai, uint16 wDef);

static boolean       MShop_GetMenuRect(MShop * pme, IMenuCtl* pSKMenu, AEERect* prc);
static boolean       MShop_GetMenuRectForAppOptions(MShop* pme, AEERect* prc);
static boolean       MShop_GetMenuRectForLogDialog(MShop* pme, IMenuCtl* pSKMenu, AEERect* prc);
static boolean       MShop_GetMenuRectForSetServer(MShop* pme, AEERect* prc);

#if defined USE_TT_FONTS
static void          MShop_InstallTTFonts(MShop* pme);
static void          MShop_RestoreFonts(MShop* pme);
#endif // USE_TT_FONTS

static boolean       MShop_SetKeyList(MShop* pme, IFlip* pIFlip, AEECLSID cls);

#ifdef FEATURE_SCREEN_ROTATE
static boolean       MShop_HandleScreenRotate(MShop* pme, uint16 wParam, uint32 dwParam);
static boolean       MShop_RotateScreen(MShop* pme);
static void          MShop_RedrawDisplay(MShop* pme);
#endif //FEATURE_SCREEN_ROTATE

#if defined(FEATURE_SCREEN_ROTATE) || defined(FEATURE_SUPPORT_MULTIPLE_DISPLAYS)
static void          MShop_RefreshDisplay(MShop* pme);
#endif //defined(FEATURE_SCREEN_ROTATE) || defined(FEATURE_SUPPORT_MULTIPLE_DISPLAYS)


#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
static void          MShop_RecreateUICallback(void* pData);
static void          MShop_EnableDispResumingCallback(void* pData);
static void          MShop_EnableDispBeforeSetStateCallback(void* pData);
static void          MShop_EnableDispCallback(void* pData);
static void          MShop_EnableSecDispCallback(void* pData);
static void          MShop_CreateDownloadInfoDisplay(MShop* pme);
static boolean       MShop_SetExternalStaticText(MShop* pme, uint16 resId);
static boolean       MShop_RecreateUI(MShop* pme);
static boolean       MShop_RecreateResumingUI(MShop* pme);
static SwitchDisplaysResult MShop_SwitchDisplays(MShop* pme, boolean useDisplay1);
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
static void          MShop_StoreMenuTitleInfo(MShop* pme, char* pszResFile, uint16 wResID, AECHAR* pwszText);
#ifdef APPINFO_IN_APPOPTIONS
static void          MShop_StoreStaticTextInfo(MShop* pme, AECHAR* pwszTitle, AECHAR* pwszText, AEEFont fntTitle, AEEFont fntText);
#endif //APPINFO_IN_APPOPTIONS
static void          MShop_StorePromptInfo(MShop * pme, const AECHAR* szTitle, uint16 wIDTitle, const AECHAR* szText, uint16 wIDText, uint16* wButtons, uint16* wItemIDs, uint16 wDefID, uint32 dwProps);

#ifdef FEATURE_OEMCFG
static boolean       MShop_LoadOEMCfg(MShop *pme);
#endif



#endif // MSHOP_H

