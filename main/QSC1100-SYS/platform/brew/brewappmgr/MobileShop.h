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

        Copyright © 2000-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEAppGen.h"
#include "AEEDownload.h"
#include "AEEStdLib.h"
#include "AEEText.h"
#include "AppMgrUtils.h"

#define SCRATCH_SPACE   0x1000

#define MSHOP_MAX_DEPTH 10

typedef struct _MShop  MShop;

// Enum BrowserAction - specifies action to be performed in response to EVT_APP_BROWSE_URL
typedef enum _BrowserAction
{
   NONE,
   CATALOG,
   SEARCH,
   CREDITBACK,
   LOG,
   SERVER,
   ITEMID,
   RESTORE,
   UPGRADECHECK,
   REMOVE,
   VERIFY
} BrowserAction;

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

// MobileShop States
#define ST_CATEGORIES         0
#define ST_APP_OPTIONS        1
#define ST_PURCHASE_CONFIRM   2
#define ST_PURCHASE_EULA      3
#define ST_PURCHASE           4
#define ST_RESTORE_APP        5
#define ST_VIEW_LOG           6
#define ST_SERVER             7
#define ST_SEARCH             8
#define ST_CREDITBACK         9
#define ST_IN_PROGRESS        10
#define ST_MAX                (ST_IN_PROGRESS + 1)

struct _MShop
{
   AEEApplet         a;

   IMenuCtl    *     m_pMenu;             // Standard Menu
   IMenuCtl    *     m_pSK;               // Softkey Menu
   IDownload   *     m_pDownload;         // IDownload Interface
   IImage      *     m_pStatus;           // Download Progress Status Image
   IStatic     *     m_pStatic;           // Static Control
   ITextCtl    *     m_pText;             // Text Control - Search Menu

   uint16            m_cyFont;            // Bold Font Height
   uint16            m_cx;                // Screen Width
   uint16            m_cy;                // Screen Height
   uint16            m_nColorDepth;       // Color Depth

   AEERect           m_rcStatus;          // Status Area
   AEERect           m_rcHist;            // Histogram Area
   uint16            m_wState;            // Key that got us here...
   uint16            m_wLastState;        // Last State
   uint16            m_wPct;              // % download complete

   flg               m_bInitStatus:1;     // MShop_InitStatus called flag
   flg               m_bDownloading:1;    // MShop_Downloading
   flg               m_bGotStatus:1;      // Status flag - Indicates if got info from ADS
   flg               m_bDemo:1;           // Currently viewed app is demo
   flg               m_bSearch:1;         // Search flag - Set true for search
   flg               m_bUpgradeCheck:1;   // Upgrade Check flag - Set true for upgrade check
   flg               m_bGotEULA:1;        // Got EULA of currently viewed app
   flg               m_bAIPrompt:1;       // Auto install flag
   flg               m_bRepaint:1;        // Repaint TextControl banner and title
   flg               m_bCarrierMessage:1; // Display Carrier Message
   flg               m_bDisplayInfo:1;    // Display Information Flag
   flg               m_bShowBusy:1;       // Flag to indicate if MobileShop needs to show busy
                                          // in between UI states
   flg               m_bVerify:1;         // Set true for application verification


   AEEEvent          m_evtStart;          // Event that started the MobileShop

   DLItem *          m_pItem;             // Currently viewed app's info
   Price *           m_pPrice;            // Currently viewed app's price info
   AECHAR            m_szApp[32];         // Currently viewed app's name
   DLITEMID          m_iApp;              // Currently viewed app's download ID

   AEECLSID          m_clsOpenAtExit;     // Start this applet at EVT_APP_STOP

   AECHAR *          m_pszEULA;           // Cached EULA of currently viewed app

   AECHAR      *     m_pLogBuff;          // Log Buffer
   uint16            m_wLogItemID;        // Log Buffer ItemID

   MShopCat          m_catList[MSHOP_MAX_DEPTH];   // Category List
   int               m_nCatIdx;                    // Category List Index
   int               m_nCatItems;                  // # of category items
   MShopState        m_states[ST_MAX];             // State Machine
   IImage*           m_pMShopLogo;                 // MShop Logo
   AEERect           m_rcMShopLogo;                // MShop Logo Rect
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
static void          MShop_StartSearch(MShop * pme, boolean bSearchAny);

static boolean       MShop_CreditBackDialog(MShop * pme);
static void          MShop_StartCreditBack(MShop * pme);

static void          MShop_EnumCleanup(MShop * pme);
static boolean       MShop_Enum(MShop * pme);
static void          MShop_EnumCB(void * pcxt, int nErr, DLEnumItem * pi);
static void          MShop_CatUp(MShop * pme);
static void          MShop_CatDown(MShop * pme, DLEnumItem * piOwner);
static void          MShop_ShowItemInfo(MShop * pme,DLEnumItem * pi);

static boolean       MShop_GetEULA(MShop * pme);
static void          MShop_EULACB(void * pcxt, int nErr, const AECHAR * pszMsg);

static boolean       MShop_ConfirmPurchaseDialog(MShop * pme);

static boolean       MShop_Purchase(MShop * pme);
static void          MShop_PurchaseCB(void * pcxt,int nErr,void * pMsg);

static boolean       MShop_Restore(MShop * pme);
static void          MShop_RestoredCB(void * pcxt,int nErr,void * pUnused);

static boolean       MShop_RunApp(MShop * pme);

static boolean       MShop_CheckDownload(MShop * pme, DLITEMID iID, AECHAR * pszName);
static uint16        MShop_AddOptionMenuItem(MShop * pme,uint16 wItemID, DLItem * pItem, Price * p);
static uint16        MShop_AddSimpleOptionMenuItem(MShop * pme, uint16 wItemID, uint16 wTextID, Price * p, uint16 wIconID);

static void          MShop_AddNextLogItem(MShop * pme);
static void          MShop_FreeLogBuff(MShop * pme);
static void          MShop_ItemInfoCB(void * pcxt, int nErr, DLEnumItem * pItem);

static boolean       MShop_SetServerDialog(MShop * pme);
static void          MShop_SetServer(MShop * pme, uint16 wID);

static void          MShop_Browse(MShop * pme, const char * url);

static boolean       MShop_ProcessMenuSel(MShop* pme, uint16 wParam);
static boolean       MShop_ProcessDefaultCmd(MShop* pme, uint16 wParam, uint32 dwParam);
static boolean       MShop_ProcessKeyEvent(MShop* pme, uint16 wParam, uint32 dwParam); 
static boolean       MShop_SetTextctlMenuStyle(MShop* pme, uint32 dwParam);
static boolean       MShop_DisplayTextInputForm(MShop * pme, boolean bSearch);
static void          MShop_SetTitle(MShop* pme, const char* pszRes, uint16 nTitleID, const AECHAR* pTitle);
   
static boolean       IsFree(AECHAR * pszPrice);
static AECHAR *      FillPrice(IShell * ps,const char * pszRes, AECHAR * pszPrice, AECHAR * pszDest,int nSize);
static void          CheckIcon(IShell * ps, uint16 wIcon, CtlAddItem * pai, uint16 wDef);

#endif // MSHOP_H
