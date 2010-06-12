#ifndef BREWAPPMGR_H
#define BREWAPPMGR_H 
/*=========================================================================
FILE:  BrewAppMgr.h

SERVICES:  BREW Application Manager Application
  
GENERAL DESCRIPTION:
    This is implementation of AppMgr/MShop as described in "BREW Core Carrier 
    Applications - Process, Features and User Interface Specifications".
    
PUBLIC CLASSES AND STATIC FUNCTIONS:
      List the class(es) and static functions included in this file
      
INITIALIZATION & SEQUENCING REQUIREMENTS:
          
              Copyright © 2000-2008 QUALCOMM Incorporated.
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
#include "AEEFlip.h"
#include "BrewAppMgrUtils.h"

#if defined USE_TT_FONTS
#include "AEETypeface.h"
#endif // USE_TT_FONTS

/*===========================================================================

  Constant and Configuration Declarations
  
===========================================================================*/

/*===========================================================================

  Following defines are implementation specific and any change in these would
  require corresponding code changes
  
===========================================================================*/
#define        COMMAND_STR_DELTA          100
#define        FIELD_SEPARATOR            ','
#define        ITEM_SEPARATOR             ';'
#define        NEW_LINE                   '\n'
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

#define        APPMGR_CFG_VER_1           0x0001
#define        APPMGR_CFG_VER             0x0002

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
   boolean            bListView;                    // Display Main Menu as list (List vs Grid Toggle)
   boolean            bSmallIconView;               // Display Main Menu as grid (Grid vs Large Icon Toggle)
   //bListView overrides bSmallIconView.  So we have two flags to control three possible settings:
   //       bListView==1                         :- we are in List View, regardless of bSmallIconView
   //       bListView==0 and bSmallIconView==0    :- we are in Large Icon view
   //       bListView==0 and bSmallIconView==1    :- we are in small Icon view
   //Note:  If ENABLE_LARGE_ICON_VIEW is not defined then we are not given the option to switch to large icon view.
   //It is defined or not defined in BrewAppMgrSettings.h based on the view (specialized or general) which is chosen.
   boolean            bBrowseWarning;               // Show Browse Airtime Warning
   boolean            bSearchWarning;               // Show Search Airtime Warning
   boolean            bBuyWarning;                  // Show Buy Airtime Warning
   boolean            bRestoreWarning;              // Show Restore Airtime Warning
   boolean            bUpgradeWarning;              // Show Upgrade Airtime Warning
   boolean            bCreditBackWarning;           // Show CreditBack Warning
   AEECLSID           clsLaunchedApp;               // The ClassId of the app we launched
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
#ifdef FEATURE_TRUSTED_TIME
   INDETERMINATE,
#endif // FEATURE_TRUSTED_TIME
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
   AEECLSID    cls;                       // class ID   
   uint32      dwItemID;                  // Item ID of this application
   uint32      dwUpgradeID;               // Item ID of available upgrade
   uint32      nOffset;                   // Image Offset
   uint32      nSize;                     // Image Size
   uint32      dwAutoPromptTimer;         // upgrade auto-prompt "prompt timer"
   boolean     bHasShortcut;              // Has a shortcut been assigned to this app
   uint32      dwNumLaunches;             // Number of times this app has been launched
   uint32      dwLastLaunch;              // The last time this app was launched
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
//
// ST_MAINMENU, ST_FAVORITES, ST_WORK and ST_FUN must be kept consecutive.
//
// ST_ORDERMAINMENU, ST_ORDERFAVORITES, ST_ORDERWORK, ST_ORDERFUN, 
// ST_MOVEMAINMENU, ST_MOVEFAVORITES, ST_MOVEWORK and ST_MOVEFUN must be
// kept consecutive.

enum AppMgrStateValue
{

   ST_MAINMENU = 0,
   ST_FAVORITES,
   ST_WORK,
   ST_FUN,
   ST_OPTIONS,
   ST_HELP,
#ifdef MSHOP_STATES
   ST_MSHOP,
   ST_MSHOPGUIDE,
#endif // MSHOP_STATES
   ST_ORDERAPPS,
   ST_MOVEAPPS,
   ST_ORDERMAINMENU,
   ST_ORDERFAVORITES,
   ST_ORDERWORK,
   ST_ORDERFUN,
   ST_MOVEMAINMENU,
   ST_MOVEFAVORITES,
   ST_MOVEWORK,
   ST_MOVEFUN,
   ST_MOVE,
   ST_MANAGETOOL,
   ST_SYSTEMINFO,
   ST_APPINFO,
   ST_LOCKUNLOCK,
   ST_REMOVE,
   ST_VIEWPROPERTIES,
   ST_EDITSOUNDIMAGE,
   ST_SETCATEGORY,
   ST_MAINMENUVIEW,
   ST_SCREENSAVERS,
   ST_UPGRADEAUTOPROMPT,
   ST_AIRTIMEWARNINGS,
   ST_AUTOSTART,
   ST_MAX,                 // should follow last "real state"
   LAST_STATE = -3         // last state
};

enum AppMgrEditSoundImageSubStateValue
{
   ST_EDITSOUNDIMAGEBROWSENONE = 0,
   ST_EDITSOUNDIMAGEBROWSEIMAGES,
   ST_EDITSOUNDIMAGEBROWSESOUND,
   ST_EDITSOUNDIMAGEMAX
};

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

   IDisplay*         m_pIDisplay;               // Currently active display
   
   AEERect           m_rcMShopLogo;             // MShop Logo Rect
   AEERect           m_rc;                      // Display Area
   AEESoundPlayerInfo m_spInfo;                 // Audio source info
   AppMgrState       m_states[ST_MAX];          // AppMgr States
   AppMgrCfg         m_cfg;                     // Applet Config Information
   MShopCfg          m_MShopCfg;                // MShop Config Information
   AppEvent          m_appEvent;                // Event place holder

   uint16            m_nColorDepth;             // Color Depth
   uint16            m_nCSBufIndx;              // Index on Credit Sequence Buffer
   uint16            m_nFileCount;              // number of files
   uint16            m_nSelCache;               // Saves a selection for use later
   uint16            m_nItem[MAX_APP_CATS];     // applets items in general, favorites, work, fun lists
   uint16            m_nMenuSel[MAX_MENU_DEPTH];// Menu Selection History 
   
   int16             m_wState;                  // Current State
   int16             m_wLastState;              // Last State
   int16             m_cyFont;                  // Font Height
   int16             m_nIndex;                  // Index in Menu Selection History

   boolean           m_bConfigMenu;             // Flag to indicate that menu is config menu
   boolean           m_bWarningDisplayed;       // Airtime warning shown
   boolean           m_bMIDI;                   // Flag to indicate if MIDI is supported
   boolean           m_bMP3;                    // Flag to indicate if MP3 is supported
   boolean           m_bResume;                 // Flag to indicate if App Manager has to resume
   boolean           m_bSuspended;              // Flag to indicate that we are currently suspended
   boolean           m_bShowBusy;               // Flag to indicate if App Manager needs to show busy 
                                                // in between UI states
   boolean           m_bFreeCurrentApp;         // Free memory referred by m_pCurrentApp
   boolean           m_bValidCache;             // Indicates that apps cache is valid

   boolean           m_bHandledLaunchError;     // Tracks whether the last launch error was handled yet.
   boolean           m_bNormalFontSaved;        // has normal font been saved?
   boolean           m_bBoldFontSaved;          // has bold font been saved?

#ifdef FEATURE_CANCELONCLOSE
   boolean           m_bCancelOnClose;          // Cancel operations and exit on flip or slide close
#endif
#ifdef FEATURE_HAS_MULTIPLE_DISPLAYS
   boolean           m_bHasMultipleDisplays;    // Does the phone have multiple displays?
#endif

#ifdef FEATURE_INTERACT_WITH_SA_APP
   char *            m_szSMSAppUrl;
#endif

                                                // For a better understanding of the possible values
                                                // values for these three vars see the comments
                                                // in BrewAppMgrUtils.h for the macro IS_DISPLAY_BIT_SET
   uint16            m_wTouchDisplays;          // Bit field representing which displays are Touch enabled
   uint16            m_wEnabledDisplays;        // Bit field representing whith displays we are not to run on
   uint16            m_wNotEnoughKeys;          // Displays that don't have all the keys necessary that we need to
                                                // work around.

   boolean           m_bMShopChangedNumApps;    // MobileShop either added or removed an app
   boolean           m_bDispTouchEnabled;		    // notify whether current display is touch enabled or not	
   AEECLSID          m_clsMShopLaunched;        // Class MobileShop launched after downloading

   AEECLSID          m_nResumeCls;              // Class to restore current app on resume

   AppMgrItem   *    m_pList[MAX_APP_CATS];     // general, favorites, work, fun lists of applet items
   AppMgrItem   *    m_pCurrentApp;             // Current App
   IMenuCtl     *    m_pMainMenu;               // Main Menu
   IMenuCtl     *    m_pMainMenuSK;             // Main Menu Softkey - Used only in large icon view
   IMenuCtl     *    m_pMenu;                   // General Menu - Used for transitory menus
   IMenuCtl     *    m_pSK;                     // prompt buttons
   IStatic      *    m_pStatic;                 // prompt info
   IDownload    *    m_pDownload;               // pointer to IDownload interface
#ifdef USE_IDL2
   IDownload2   *    m_pDownload2;              // pointer to IDownload2 interface
#endif
#ifdef FEATURE_UPGRADE_AUTO_PROMPT
   IUpgradeMgr*      m_pUm;                     // pointer to UpgradeMgr
#endif // FEATURE_UPGRADE_AUTO_PROMPT
   ISoundPlayer *    m_pISp;                    // Sound Player Class
   IImage       *    m_pAnimatedImage;          // Current image being animated while in edit menu
   IImage       *    m_pMShopLogo;              // MShop Logo
   IFileMgr     *    m_pIfm;                    // File Manager Interface
   IFile        *    m_pImageCacheFile;         // Image Cache
   FileItem     *    m_pFileList;               // list of all image/sound file names
#ifdef USE_TT_FONTS
   IHFont       *    m_pSaveNormalFont;         // saved normal font
   IHFont       *    m_pSaveBoldFont;           // saved bold font
#endif // USE_TT_FONTS

#ifdef FEATURE_HAPTIC_SUPPORT
   ISound*           m_pHapticResponse;         // ISound object for haptic support on actionable items
   AEESoundToneData  m_ToneData ;               // Tone data for haptic response
#endif

   char              m_pCS[CS_BUF_SIZE];        // Credit Sequence Buffer
   AEECLSID          m_nErrCls;                 // Error Class
   int               m_nErr;                    // Error
   AEECLSID          m_nLaunchCls;              // Launch Class
   boolean           m_bLaunchClsOnStart;       // Launch class in m_nLaunchCls on startup;
   boolean           m_bHasStarted;             // BAM has gotten the EVT_APP_START event

   // AutoStart variables
   char         *    m_pArgs;                   // App list from command line
   char         *    m_pAutoStart;              // Auto start app in app list
   AEECallback       m_cba;                     // Auto start callback

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


   boolean           m_bRedrawingUI;         // This is a flag telling us that we are in the process
                                             // of redrawing the UI (normally from an EVT_FLIP or an
                                             // EVT_SCR_ROTATE message).  So any functions
                                             // that would normally only be called when state changed
                                             // can be called to update their display only, if this is TRUE.

   boolean           m_bExitAfterSANotification; // The user closed the flip/slide that would normally have 
                                             // caused us to exit, but since MobileShop was downloading
                                             // we are delying the exit.  When we resume, we just need
                                             // update the applist.lst file and send the Apps updated 
                                             // Brew SMS, and then exit.  We don't need to do the rest 
                                             // of the resume stuff.


   int16             m_wEditSoundImageSubState; // There are multipe "states" for the ST_EDITSOUNDIMAGE
                                                // state.  Clairfy which one we are in

#ifdef FEATURE_SCREEN_ROTATE
   AEEScrOrientation m_orentDisplayDefault;  // The default orentation for the currently active display
#endif //FEATURE_SCREEN_ROTATE

   // Below are Vars for saving some values sent to the controls
   // This allows us to better reproduce the control state
   // when we switch screens
#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
   AEECallback       *cbDisplayEnabled;
   AEECallback       m_cbUpdateUI;
   boolean           m_bFlipOpenWhenSuspended;  // Flag to indicate that we need to process a flip event when we resume
   boolean           m_bPromptIsDisplayed;      // This is a flag telling us that the AppMgr_Prompt() display is up
                                                // and that it is waiting for user response.  If we are telling
                                                // the user something in a timed message, we don't set this
   char*             m_pMainMenuResFile;
   uint16            m_pMainMenuResID;
   AECHAR*           m_pMainMenuText;
   char*             m_pMenuResFile;
   uint16            m_pMenuResID;
   AECHAR*           m_pMenuText;
   boolean           m_bMainMenuSKEnableCommandCalled;
   boolean           m_bMainMenuSKEnableCommandValue;
   boolean           m_bSKEnableCommandCalled;
   boolean           m_bSKEnableCommandValue;
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
   uint16            m_wPromptDefID;
   uint32            m_dwPromptProps;
   boolean           m_bPromptBox;
#endif // FEATURE_SUPPORT_MULTIPLE_DISPLAYS

};

//
// Class creation functions
//
int AppMgrClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);
extern int MShopClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);



//
// Local/Class Functions
//
static boolean       AppMgr_Init(AppMgr * pme);
static void          AppMgr_Free(AppMgr * pme);
static boolean       AppMgr_HandleEvent(AppMgr * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);

//
// State Handlers
//
static boolean       AppMgr_MainMenu(AppMgr * pme);
static boolean       AppMgr_Launcher(AppMgr * pme);
static boolean       AppMgr_Options(AppMgr * pme);
static boolean       AppMgr_Help(AppMgr * pme);
#ifdef MSHOP_STATES
static boolean       AppMgr_MShop(AppMgr * pme);
static boolean       AppMgr_MShopGuide(AppMgr * pme);
#endif
static boolean       AppMgr_Categories(AppMgr * pme);
static boolean       AppMgr_Config(AppMgr * pme);
static boolean       AppMgr_ManageTool(AppMgr * pme);
static boolean       AppMgr_SystemInfo(AppMgr * pme);
static boolean       AppMgr_AppInfo(AppMgr * pme);
static boolean       AppMgr_Remove(AppMgr * pme);
static boolean       AppMgr_ViewProperties(AppMgr * pme);
static boolean       AppMgr_MainMenuView(AppMgr * pme);
static boolean       AppMgr_ScreenSavers(AppMgr * pme);
static boolean       AppMgr_UpgradeAutoPrompt(AppMgr* pme);
static boolean       AppMgr_EditSoundImage(AppMgr* pme);
static boolean       AppMgr_LockUnlock(AppMgr* pme);
static boolean       AppMgr_AirtimeWarnings(AppMgr* pme);
static boolean       AppMgr_AutoStart(AppMgr* pme);

static boolean       AppMgr_SetState(AppMgr * pme, int nState);

static boolean       AppMgr_Start(AppMgr* pme);
static void          AppMgr_LoadAppletsInfo(AppMgr* pme);

#ifdef USE_TT_FONTS
static void          AppMgr_InstallTTFonts(AppMgr* pme);
static void          AppMgr_RestoreFonts(AppMgr* pme);
#endif // USE_TT_FONTS

static void          AppMgr_ReadApplets(AppMgr * pme);
static void          AppMgr_ReadConfigFile(AppMgr * pme);
static int           AppMgr_ParseConfigFile(AppMgr* pme, void* fileBuff, int nVersion);
static void          AppMgr_WriteConfigFile(AppMgr * pme);
static uint16        AppMgr_FindItem(AppMgr * pme, AEECLSID cls, uint16 nCatIndex, AppMgrItem** ppItem);
static int           AppMgr_AddItem(AppMgr * pme, uint16 nCategory, AEECLSID cls, uint32 dwItemID, uint32 nOffset, uint32 nSize, uint32 nAutoPromptTimer, AECHAR * pName, char* pszIcon, char* pszSound, boolean atBeginning);
static int           AppMgr_SetCategory(AppMgr * pme, AEECLSID cls, uint16 nCategory);
static uint16        AppMgr_FindItemCategory(AppMgr * pme, AEECLSID cls, AppMgrItem** ppItem);
static boolean       AppMgr_HandleLaunchError(AppMgr* pme, AEECLSID cls, int nErr);
static boolean       AppMgr_HandleSystemError(AppMgr* pme, AEECLSID cls, int nErr, AECHAR* pTitle);
static void          AppMgr_AutoStartApp(void * po);

static boolean AppMgr_LoadOEMCfg(AppMgr *pme);

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
static void          AppMgr_DrawMemHist(AppMgr * pme, uint64 qwUsed, uint64 qwTotal, AEERect * prc);
static boolean       AppMgr_RequestMShop(AppMgr * pme, uint16 wParam, boolean bWarning);
static boolean       AppMgr_RemoveWarning(AppMgr* pme);
static void          AppMgr_ShowFiles(AppMgr * pme, boolean bImage);
static boolean       AppMgr_FileSelected(AppMgr * pme, uint16 nSel);
static void          AppMgr_StopSoundAnimation(AppMgr* pme);
static boolean       AppMgr_CtlSelChanged(AppMgr* pme, uint16 wParam, uint32 dwParam);
static boolean       AppMgr_Suspend(AppMgr* pme);
static boolean       AppMgr_Resume(AppMgr* pme);
static boolean       AppMgr_IsDifferentFlipPosition(AppMgr* pme);
static void          AppMgr_PlayRetry(void *po);
static void          AppMgr_Order(AppMgr* pme);
static void          AppMgr_MoreInfo(AppMgr * pme, AEERect * prc);
static void          AppMgr_SetScreensaver(AppMgr* pme, uint16 wItemId);
static void          AppMgr_SetMainMenuSize(AppMgr * pme);
static boolean       AppMgr_GetMenuRect(AppMgr * pme, IMenuCtl* pSKMenu, AEERect* prc);
static boolean       AppMgr_GetMenuRectForManageTool(AppMgr* pme, AEERect* prc);
static boolean       AppMgr_GetMenuRectForCategories(AppMgr* pme, AEERect* prc);
#ifdef MSHOP_STATES
static boolean       AppMgr_GetMenuRectForMShop(AppMgr* pme, AEERect* prc);
static boolean       AppMgr_GetMenuRectForMShopGuide(AppMgr* pme, AEERect* prc);
#endif //MSHOP_STATES
static boolean       AppMgr_GetMenuRectForScreensavers(AppMgr* pme, IMenuCtl* pSKMenu, AEERect* prc);
static boolean       AppMgr_GetMenuRectForConfig(AppMgr* pme, AEERect* prc);
static boolean       AppMgr_GetMenuRectForShowFiles(AppMgr* pme, AEERect* prc);
static void          AppMgr_DrawVisualIndication(AppMgr* pme);
static void          AppMgr_VisualConfirmation(AppMgr* pme, uint16 nMsgID);
static void          AppMgr_VisualConfirmationCB(void* po);
static void          AppMgr_LargeIconViewSettings(AppMgr* pme, IMenuCtl* pMenu, IMenuCtl* pSKMenu);

DLITEMID             AppMgr_GetItemID(AppMgr * pme, AppMgrItem * pmi);
boolean              AppMgr_GetAEEAppInfo(AppMgr* pme, AEECLSID cls, AEEAppInfo* pai);

#ifdef USE_IDL2_DISABLED_SUPPORT
AEECLSID             AppMgr_GetClassIDForDLItem(AppMgr* pme, DLITEMID itemID);
#endif // USE_IDL2_DISABLED_SUPPORT


#ifdef USE_IDL2_4GB_SUPPORT
static boolean       IsExpired(DL2AppModInfo* pai);
#else
static boolean       IsExpired(AppModInfo* pai);
#endif

#ifdef FEATURE_TRUSTED_TIME
static boolean       IsTimeBased(DL2AppModInfo* pai);
#endif // FEATURE_TRUSTED_TIME

static boolean       ImageFits(IImage * pImage, IShell* pIShell, const char* pszResFile, uint16 wImage, int w, int h);

static boolean       AppMgr_Browse(AppMgr * pme, const char * url);
#if 0
static boolean       AppMgr_SetKeyList(AppMgr* pme, IFlip* pIFlip, AEECLSID cls);
#endif
#ifdef FEATURE_INTERACT_WITH_SA_APP
static boolean       AppMgr_SetAppShortcutStatus(AppMgr* pme, AEECLSID cls, boolean hasShortcut);
static boolean       AppMgr_WriteAppList(AppMgr* pme);
#endif // FEATURE_INTERACT_WITH_SA_APP

#ifdef FEATURE_SCREEN_ROTATE
static boolean       AppMgr_HandleScreenRotate(AppMgr* pme, uint16 wParam, uint32 dwParam);
static boolean       AppMgr_RotateScreen(AppMgr* pme);
static void          AppMgr_RedrawDisplay(AppMgr* pme);
#endif //FEATURE_SCREEN_ROTATE

#if defined(FEATURE_SCREEN_ROTATE) || defined(FEATURE_SUPPORT_MULTIPLE_DISPLAYS)
static void          AppMgr_RefreshDisplay(AppMgr* pme);
#endif //defined(FEATURE_SCREEN_ROTATE) || defined(FEATURE_SUPPORT_MULTIPLE_DISPLAYS)

#ifdef FEATURE_SUPPORT_MULTIPLE_DISPLAYS
static void          AppMgr_RecreateUICallback(void* pData);
static void          AppMgr_EnableDispRedrawCallback(void* pData);
static void          AppMgr_EnableDispCallback(void* pData);
static boolean       AppMgr_RecreateUI(AppMgr* pme);
#if 0
static boolean       AppMgr_SwitchDisplays(AppMgr* pme, boolean useDisplay1);
#endif
#endif //FEATURE_SUPPORT_MULTIPLE_DISPLAYS
static void          AppMgr_StoreMainMenuTitleInfo(AppMgr* pme, char* pszResFile, uint16 wResID, AECHAR* pwszText);
static void          AppMgr_StoreMenuTitleInfo(AppMgr* pme, char* pszResFile, uint16 wResID, AECHAR* pwszText);
static void          AppMgr_StoreMainMenuSKEnableCommandInfo(AppMgr* pme, boolean value);
//static void          AppMgr_StoreSKEnableCommandInfo(AppMgr* pme, boolean value);
static void          AppMgr_StoreStaticTextInfo(AppMgr* pme, AECHAR* pwszTitle, AECHAR* pwszText, AEEFont fntTitle, AEEFont fntText);
static void          AppMgr_StoreStaticSizeToFitInfo(AppMgr* pme, boolean called, AEERect* prc);
static void          AppMgr_StorePromptInfo(AppMgr* pme, const AECHAR* szTitle, uint16 wIDTitle, const AECHAR* szText,
                                            uint16 wIDText, uint16* wButtons, uint16 wDefID, uint32 dwProps, boolean bBox);


#endif // BREWAPPMGR_H
