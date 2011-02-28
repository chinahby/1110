#ifndef APPMGR_H
#define APPMGR_H
/*=========================================================================
FILE:  AppMgr.h

SERVICES:  BREW Application Manager Application

GENERAL DESCRIPTION:
	 This is implementation of AppMgr/MShop as described in "BREW Core Carrier
	 Applications - Process, Features and User Interface Specifications".
	
PUBLIC CLASSES AND STATIC FUNCTIONS:
		List the class(es) and static functions included in this file
		
INITIALIZATION & SEQUENCING REQUIREMENTS:
			
				  Copyright © 2000-2002 QUALCOMM Incorporated.
				  All Rights Reserved.
				  QUALCOMM Proprietary/GTDR
===========================================================================*/
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AEEStdLib.h"
#include "AEEFile.h"
#include "AEEDownload.h"
#include "AEESoundPlayer.h"
#include "AEEMimeTypes.h"
#include "AppMgrUtils.h"

/*===========================================================================

  Constant and Configuration Declarations

===========================================================================*/

/*===========================================================================

  Following defines are implementation specific and any change in these would
  require corresponding code changes

===========================================================================*/
#define        COMMAND_STR_DELTA          100
#define			FIELD_SEPARATOR			   ','
#define			ITEM_SEPARATOR			      ';'
#define			NEW_LINE					      '\n'
#define        WHITE_SPACE                ' '
#define        MAX_APP_NAME               32

#define        MAX_APP_CATS               4
#define        IMAGE_CACHE                "imagecache.bin"

#define        MAX_MENU_DEPTH             5

#define        IDC_APP                    10000
#define        IDC_LAUNCH_APP             11000
#define        IDC_SCREEN_SAVER_APP       12000
#define        IDC_CONFIGURE_APP          13000
#define        IDC_ORDER_APP              14000
#define        IDC_MOVE_APP               15000

#define        CS_BUF_SIZE                10

#define        APPMGR_CFG_VER             0x0001

//The following types of Image Files will be supported

#define        BMP_FILES                  OEM_IMAGE_FILE_PATH"\\*.bmp"
#define        GIF_FILES                  OEM_IMAGE_FILE_PATH"\\*.gif"
#define        PNG_FILES                  OEM_IMAGE_FILE_PATH"\\*.png"
#define        BCI_FILES                  OEM_IMAGE_FILE_PATH"\\*.bci"
#define        JPG_FILES                  OEM_IMAGE_FILE_PATH"\\*.jpg"
#define        JPEG_FILES                 OEM_IMAGE_FILE_PATH"\\*.jpeg"

//The following types of Sound Files will be supported

#define        MIDI_FILES                 (OEM_SOUND_FILE_PATH"\\*." MIDI_EXTENSION)
#define        MP3_FILES                  (OEM_SOUND_FILE_PATH"\\*." MP3_EXTENSION)

//
// Struct to make selections persistent
//
typedef struct AppMgrCfg
{
   flg				   bListView:1;		            // Display Main Menu as list (List vs Grid Toggle)
   flg               bSmallIconView:1;             // Display Main Menu as grid (Grid vs Large Icon Toggle)
   flg               bBrowseWarning:1;             // Show Browse Airtime Warning
   flg               bSearchWarning:1;             // Show Search Airtime Warning
   flg               bBuyWarning:1;                // Show Buy Airtime Warning
   flg               bRestoreWarning:1;            // Show Restore Airtime Warning
   flg               bUpgradeWarning:1;            // Show Upgrade Airtime Warning
   flg               bCreditBackWarning:1;         // Show CreditBack Warning
} AppMgrCfg;

//
// Enum for App Status
//
typedef enum _appstatus
{
   NORMAL,
   DISABLED,
   DEMO,
   EXPIRED,
   EXPIREDDEMO,
   ACKPENDING
} AppStatus;

//
// Enum for App Image Type
//
typedef enum _appimagetype
{
   THUMB,
   ICON,
   IMAGE
} AppImageType;

//
// Structure to store applet information
//
typedef  struct _appmgritem AppMgrItem;

struct _appmgritem
{
   AEECLSID    cls;							   // class ID
   uint32      dwItemID;                  // Item ID of this application
   uint32      nOffset;                   // Image Offset
   uint32      nSize;                     // Image Size
   AECHAR      wszName[MAX_APP_NAME];     // App Name
   char        szSound[MAX_FILE_NAME];    // Sound file name
   char        szIcon[MAX_FILE_NAME];     // Image file name
   AppMgrItem* pNext;                     // Pointer to next node
   AppMgrItem* pPrev;                     // Pointer to previous node
};

typedef struct _AppMgr  AppMgr;

// State Machine Handler Function
typedef boolean   (*PFNREQ)(AppMgr * pme);

// State Handler. State Machine is an array of state handlers.
// Array Index specifies state where as these structs specify
// state handler

typedef struct
{
   PFNREQ   pfnReq;  // Pointer to Handler Function
   int16    nBack;   // State to go back
   boolean  bClear;  // Clear Display before invoking state handler
} AppMgrState;

// AppMgr States
// ST_MAINMENU, ST_FAVORITES, ST_WORK and ST_FUN must be kept consecutive
// ST_ORDERMAINMENU, ST_ORDERFAVORITES, ST_ORDERWORK, ST_ORDERFUN,
// ST_MOVEMAINMENU, ST_MOVEFAVORITES, ST_MOVEWORK and ST_MOVEFUN must be
// kept consecutive

#define  ST_MAINMENU             0
#define  ST_FAVORITES            1
#define  ST_WORK                 2
#define  ST_FUN                  3
#define  ST_MSHOP                4
#define  ST_OPTIONS              5
#define  ST_HELP                 6
#define  ST_MSHOPGUIDE           7
#define  ST_ORDERAPPS            8
#define  ST_MOVEAPPS             9
#define  ST_ORDERMAINMENU        10
#define  ST_ORDERFAVORITES       11
#define  ST_ORDERWORK            12
#define  ST_ORDERFUN             13
#define  ST_MOVEMAINMENU         14
#define  ST_MOVEFAVORITES        15
#define  ST_MOVEWORK             16
#define  ST_MOVEFUN              17
#define  ST_MOVE                 18
#define  ST_MANAGETOOL           19
#define  ST_SYSTEMINFO           20
#define  ST_APPINFO              21
#define  ST_LOCKUNLOCK           22
#define  ST_REMOVE               23
#define  ST_VIEWPROPERTIES       24
#define  ST_EDITSOUNDIMAGE       25
#define  ST_SETCATEGORY          26
#define  ST_MAINMENUVIEW         27
#define  ST_SCREENSAVERS         28
#define  ST_AIRTIMEWARNINGS      29
#define  ST_MAX                  (ST_AIRTIMEWARNINGS + 1)

#define  LAST_STATE           -3 // last state

//
// File Data Structure
//
typedef struct _fileitem
{
   char     szFile[MAX_FILE_NAME]; // File name
} FileItem;

//
// Event Structure
//
typedef struct _AppEvent
{
   AEEEvent          evt;
   uint16            wParam;
   uint32            dwParam;
} AppEvent;

//
// Main Applet Structure
//
struct _AppMgr
{
   AEEApplet         a;                         // AEEApplet Structure

   AEERect           m_rcMShopLogo;             // MShop Logo Rect
	AEERect				m_rc;					         // Display Area
   AEESoundPlayerInfo m_spInfo;                 // Audio source info
   AppMgrState       m_states[ST_MAX];          // AppMgr States
   AppMgrCfg         m_cfg;                     // Applet Config Information
   MShopCfg          m_MShopCfg;                // MShop Config Information
   AppEvent          m_appEvent;                // Event place holder

   uint16            m_nColorDepth;             // Color Depth
   uint16            m_nCSBufIndx;              // Index on Credit Sequence Buffer
	uint16				m_nFileCount;              // number of files
   uint16            m_nSelCache;               // Saves a selection for use later
   uint16				m_nItem[MAX_APP_CATS];     // applets items in general, favorites, work, fun lists
   uint16            m_nMenuSel[MAX_MENU_DEPTH];// Menu Selection History

   int16             m_wState;                  // Current State
   int16             m_wLastState;              // Last State
   int16             m_cyFont;                  // Font Height
   int16             m_nIndex;                  // Index in Menu Selection History

   flg               m_bConfigMenu:1;           // Flag to indicate that menu is config menu
   flg               m_bWarningDisplayed:1;     // Airtime warning shown
   flg               m_bMIDI:1;                 // Flag to indicate if MIDI is supported
   flg               m_bMP3:1;                  // Flag to indicate if MP3 is supported
   flg               m_bResume:1;               // Flag to indicate if App Manager has to resume
   flg               m_bRetain:1;               // Flag to indicate if App Manager can retain Apps Cache
   flg               m_bShowBusy:1;             // Flag to indicate if App Manager needs to show busy
                                                // in between UI states
   flg               m_bFreeCurrentApp:1;       // Free memory referred by m_pCurrentApp
   flg               m_bValidCache:1;           // Indicates that apps cache is valid

   AEECLSID          m_nResumeCls;              // Class to restore current app on resume

	AppMgrItem   *		m_pList[MAX_APP_CATS];     // general, favorites, work, fun lists of applet items
	AppMgrItem   *	   m_pCurrentApp;		         // Current App
	IMenuCtl     *	   m_pMainMenu;			      // Main Menu
   IMenuCtl     *    m_pMainMenuSK;             // Main Menu Softkey - Used only in large icon view
	IMenuCtl     *		m_pMenu;		               // General Menu - Used for transitory menus
   IMenuCtl     *    m_pSK;                     // prompt buttons
   IStatic      *    m_pStatic;                 // prompt info
   IDownload    *    m_pDownload;               // pointer to IDownload interface
   ISoundPlayer *    m_pISp;                    // Sound Player Class
	IImage       *    m_pAnimatedImage;          // Current image being animated while in edit menu
	IImage       *		m_pMShopLogo;		         // MShop Logo
	IFileMgr     *		m_pIfm;				         // File Manager Interface
   IFile        *    m_pImageCacheFile;         // Image Cache
	FileItem     *		m_pFileList;               // list of all image/sound file names
   char              m_pCS[CS_BUF_SIZE];        // Credit Sequence Buffer
   AEECLSID          m_nErrCls;                 // Error Class
   int               m_nErr;                    // Error
   AEECLSID          m_nLaunchCls;              // Launch Class
};

//
// Class creation functions
//
int AppMgrClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);
extern int MShopClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);

//
// Local/Class Functions
//
static boolean			AppMgr_Init(AppMgr * pme);
static void          AppMgr_Free(AppMgr * pme);
static boolean       AppMgr_HandleEvent(AppMgr * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);

//
// State Handlers
//
static boolean		   AppMgr_MainMenu(AppMgr * pme);
static boolean       AppMgr_Launcher(AppMgr * pme);
static boolean       AppMgr_MShop(AppMgr * pme);
static boolean       AppMgr_Options(AppMgr * pme);
static boolean       AppMgr_Help(AppMgr * pme);
static boolean       AppMgr_MShopGuide(AppMgr * pme);
static boolean       AppMgr_Categories(AppMgr * pme);
static boolean       AppMgr_Config(AppMgr * pme);
static boolean       AppMgr_ManageTool(AppMgr * pme);
static boolean       AppMgr_SystemInfo(AppMgr * pme);
static boolean       AppMgr_AppInfo(AppMgr * pme);
static boolean       AppMgr_Remove(AppMgr * pme);
static boolean       AppMgr_ViewProperties(AppMgr * pme);
static boolean       AppMgr_MainMenuView(AppMgr * pme);
static boolean       AppMgr_ScreenSavers(AppMgr * pme);
static boolean       AppMgr_EditSoundImage(AppMgr* pme);
static boolean       AppMgr_LockUnlock(AppMgr* pme);
static boolean       AppMgr_AirtimeWarnings(AppMgr* pme);

static boolean       AppMgr_SetState(AppMgr * pme, int nState);

static int           AppMgr_Start(AppMgr* pme);
static void          AppMgr_LoadAppletsInfo(AppMgr* pme);

static void				AppMgr_ReadApplets(AppMgr * pme);
static void				AppMgr_ReadConfigFile(AppMgr * pme);
static int				AppMgr_ParseConfigFile(AppMgr* pme, void* fileBuff, int nVersion);
static void				AppMgr_WriteConfigFile(AppMgr * pme);
static uint16        AppMgr_FindItem(AppMgr * pme, AEECLSID cls, uint16 nCatIndex, AppMgrItem** ppItem);
static int				AppMgr_AddItem(AppMgr * pme, uint16 nCategory, AEECLSID cls, uint32 dwItemID, uint32 nOffset, uint32 nSize, AECHAR * pName, char* pszIcon, char* pszSound);
static int           AppMgr_SetCategory(AppMgr * pme, AEECLSID cls, uint16 nCategory);
static uint16        AppMgr_FindItemCategory(AppMgr * pme, AEECLSID cls, AppMgrItem** ppItem);
static boolean       AppMgr_HandleLaunchError(AppMgr* pme, AEECLSID cls, int nErr);
static boolean       AppMgr_HandleSystemError(AppMgr* pme, AEECLSID cls, int nErr, AECHAR* pTitle);

static AppStatus     AppMgr_AppStatus(AppMgr* pme, uint32 itemID);

static boolean       AppMgr_LaunchCurrentApplet(AppMgr* pme, boolean bForceRun);
static AECHAR *      AppMgr_GetAppName(AppMgr* pme, AppMgrItem* pi);
static IImage *      AppMgr_GetAppIcon(AppMgr* pme, AppMgrItem* pi);
static IImage *      AppMgr_LoadAppIcon(AppMgr* pme, AppMgrItem* pi);
static void          AppMgr_GetAppImage(AppMgr* pme, AppMgrItem* pi, IImage** pImage);
static void          AppMgr_Reset(AppMgr* pme, boolean bClear);

static void          AppMgr_AddMenuItem(AppMgr * pme, IMenuCtl* pMenu, AppMgrItem* pi, uint16 itemID, AppImageType nType, boolean bSK);
static void          AppMgr_DisplayAppStatusDialog(AppMgr* pme, AppStatus status);
static void          AppMgr_Prompt(AppMgr * pme, const AECHAR* szTitle, uint16 wIDTitle, const AECHAR* szText, uint16 wIDText, uint16* wButtons, uint16 wDefID, uint32 dwProps, boolean bBox);
static void          AppMgr_HelpDisplay(AppMgr* pme, uint16 nCmd);
static void          AppMgr_DisplayMemoryInfo(AppMgr * pme);
static boolean       AppMgr_ShowAirtimeWarning(AppMgr * pme, uint16 wWarningID);
static void          AppMgr_DrawMemHist(AppMgr * pme, uint32 dwUsed, uint32 dwTotal, AEERect * prc);
static boolean       AppMgr_RequestMShop(AppMgr * pme, uint16 wParam, boolean bWarning);
static boolean       AppMgr_RemoveWarning(AppMgr* pme);
static void          AppMgr_ShowFiles(AppMgr * pme, boolean bImage);
static boolean       AppMgr_FileSelected(AppMgr * pme, uint16 nSel);
static void          AppMgr_StopSoundAnimation(AppMgr* pme);
static boolean       AppMgr_CtlSelChanged(AppMgr* pme, uint16 wParam, uint32 dwParam);
static boolean       AppMgr_Suspend(AppMgr* pme);
static boolean       AppMgr_Resume(AppMgr* pme);
static void          AppMgr_PlayRetry(void *po);
static void          AppMgr_Order(AppMgr* pme);
static void          AppMgr_MoreInfo(AppMgr * pme, AEERect * prc);
static void          AppMgr_SetMainMenuSize(AppMgr * pme);
#if 0
static void          AppMgr_DrawAppStatus(AppMgr* pme, const AppStatus status);
#endif
static void          AppMgr_DrawVisualIndication(AppMgr* pme);
static void          AppMgr_VisualConfirmation(AppMgr* pme, uint16 nMsgID);
static void          AppMgr_VisualConfirmationCB(void* po);
static void          AppMgr_LargeIconViewSettings(AppMgr* pme);

DLITEMID             AppMgr_GetItemID(AppMgr * pme, AppMgrItem * pmi);

static boolean       IsExpired(AppModInfo* pai);
static boolean       ImageFits(IImage * pImage, IShell* pIShell, const char* pszResFile, uint16 wImage, int w, int h);

#endif // APPMGR_H
