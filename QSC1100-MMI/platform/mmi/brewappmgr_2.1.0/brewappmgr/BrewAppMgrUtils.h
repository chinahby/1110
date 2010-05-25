#ifndef BREWAPPMGRUTILS_H
#define BREWAPPMGRUTILS_H
/*======================================================

                    IMPORTANT NOTICE

The contents of this file are provided under legal agreement
between QUALCOMM Incorporated and selected licensed carrier
and handset OEM partners. Under the terms of that agreement, 
this file MUST NOT be distributed in full or in part to anyone
not specifically authorized by QUALCOMM Incorporated.

FILE:  BrewAppMgrUtils.h

SERVICES:  This files has common stuff between AppMgr and MobileShop
   applets.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

        Copyright © 2000-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEDownload.h"
#include "AEE.h"
#include "AEEMenu.h"
#include "AEEFile.h"
#include "BrewAppMgrSettings.h"
#ifdef FEATURE_UPGRADE_AUTO_PROMPT
#include "../upgrademgr/UpgradeMgr.h"
#endif // FEATURE_UPGRADE_AUTO_PROMPT
#ifdef FEATURE_COREAPPUI
#include "../coreappui/coreappui.h"
#endif // FEATURE_COREAPPUI
#include "MobileShop.bid"
#include "BrewAppMgr.bid"
#include "brewappmgrln.brh"
#include "brewappmgrls.brh"
#include "brewappmgrcommon.brh"

#include "AEEAnnunciator.h"
#if defined(BREWAPPMGRLS_RES_FILE)
#undef BREWAPPMGRLS_RES_FILE
#define  BREWAPPMGRLS_RES_FILE (AEE_RES_LANGDIR "brewappmgrls.bar")
#endif

#define        BAM_EVENT_BASE             EVT_USER
// MobileShop Added an app
#define        BAM_EVT_APP_ADDED          (BAM_EVENT_BASE + 1)
// MobileShop deleted an app
#define        BAM_EVT_APP_DELETED        (BAM_EVENT_BASE + 2)
// MobileShop launched an app
#define        BAM_EVT_APP_LAUNCHED       (BAM_EVENT_BASE + 3)
// MobleShop is "Not dead yet"
#define        BAM_EVT_CONTINUE_DOWNLOAD  (BAM_EVENT_BASE + 4)


#define        APPMGR_CURRENT_DISPLAY     0

#define        CONFIG_FILE_VER_1          "ver:1"
#define        CONFIG_FILE_VER_2          "ver:2"
#define        CONFIG_FILE_VER_3          "ver:3"
#define        CONFIG_FILE                "config.txt"
#define        CLSID_LENGTH               11

#define        OEMCFG_FILE                "oemcfg.txt"

#define        APPLIST_FILE               "applist.lst"

#define        MARGIN                     3

#define        ONE_K                      (1024l)
#define        ONE_MB                     (ONE_K * ONE_K)
#define        ONE_GB                     (ONE_K * ONE_K * ONE_K)

#define        SECS_PER_DAY               (60l * 60l * 24l)

#define        PREV_ENTRY                 -1 // use the previous item
#define        NO_ENTRY                   -2 // cannot go BACK on this item

// Color Mask
#define        MENU_COLOR_MASK            (MC_BACK | MC_SEL_BACK | MC_SEL_TEXT)

// 256 Color Menu
#define        COLOR_MENU_BACKGROUND            RGB_WHITE
#define        COLOR_MENU_SELECT_BACKGROUND     MAKE_RGB(192, 192, 192)
#define        COLOR_MENU_SELECT_TEXT           RGB_BLACK

// 256 Color Softkey
#define        COLOR_SK_BACKGROUND              MAKE_RGB(192, 192, 192)
#define        COLOR_SK_SELECT_BACKGROUND       MAKE_RGB(192, 192, 192)
#define        COLOR_SK_SELECT_TEXT             RGB_BLACK

// 256 Color IconView
#define        COLOR_ICON_BACKGROUND            RGB_WHITE
#define        COLOR_ICON_SELECT_BACKGROUND     MAKE_RGB(192, 192, 192)
#define        COLOR_ICON_SELECT_TEXT           RGB_BLACK

// 4Grey/Mono Menu/Softkey Colors
#define        MENU_BACKGROUND                  RGB_WHITE
#define        MENU_SELECT_BACKGROUND           RGB_BLACK
#define        MENU_SELECT_TEXT                 RGB_WHITE

#define        MAX_TOUCH_SOFTMENUSTRING         20
#define        MAX_STR_CHARS                    (512)
#define        MAX_STR_SIZE                     (sizeof(AECHAR) * MAX_STR_CHARS)

#define        ADD_MENUITEM_TIMER               2
#define        ITEM_HEIGHT                      12



// cmshop commads:
#define        CMSHOP_CMD_BASE                  "cmshop"
#define        CMSHOP_CMD_WILDCARD              "cmshop*"
#define        CMSHOP_CMD_COMMA                 ','
#define        CMSHOP_CMD_COLON                 ':'
#define        CMSHOP_CMD_EQUAL                 '='
#define        CMSHOP_CMD_SEMICOLON             ';'
#define        CMSHOP_CMD_AMPERSAND             '&'
#define        CMSHOP_CMD_ITEMID                "ItemID"
#define        CMSHOP_CMD_ITEMINFO              "ItemInfo"
#define        CMSHOP_CMD_RESTORE               "Restore"
#define        CMSHOP_CMD_CATALOG               "Catalog"
#define        CMSHOP_CMD_SEARCH                "Search"
#define        CMSHOP_CMD_CREDITBACK            "CreditBack"
#define        CMSHOP_CMD_LOG                   "Log"
#define        CMSHOP_CMD_SERVER                "Server"
#define        CMSHOP_CMD_REMOVE                "Remove"
#define        CMSHOP_CMD_UPGRADECHECK          "UpgradeCheck"
#define        CMSHOP_CMD_SILENTUPGRADECHECK    "SilentUpgradeCheck"
#define        CMSHOP_CMD_VERIFY                "Verify"
#define        CMSHOP_CMD_CATEGORY              "Category"
#define        CMSHOP_CMD_TITLE                 "Title"

#define        CMSHOP_CMD_CATALOGID             "CatalogID"
#define        CMSHOP_CMD_CATEGORYID            "CategoryID"
#define        CMSHOP_CMD_ACQUIRE               "Acquire"
#define        CMSHOP_CMD_PRICEHANDLE           "PriceHandle"

#define        APPMGR_CMD_BASE                  "BAM"
#define        APPMGR_CMD_WILDCARD              "BAM*"
#define        APPMGR_CMD_COMMA                 ','
#define        APPMGR_CMD_COLON                 ':'
#define        APPMGR_CMD_EQUAL                 '='
#define        APPMGR_CMD_SEMICOLON             ';'
#define        APPMGR_CMD_AMPERSAND             '&'
#define        APPMGR_CMD_LISTAPPS              "ListApps"
#define        APPMGR_CMD_SHORTCUT              "Shortcut"
#define        APPMGR_CMD_ACTIVE                "active"
#define        APPMGR_CMD_TRUE                  "true"
#define        APPMGR_CMD_LAUNCHAPP             "LaunchApp"

#ifdef FEATURE_CANCELONCLOSE
#define        OEMCFG_CANCELONCLOSE             "CancelOnClose"
#endif
#ifdef FEATURE_INTERACT_WITH_SA_APP
#define        OEMCFG_SA_APP_SMS_URL            "AppsUpdatedUrl"
#endif
#ifdef FEATURE_TOUCHSCREEN_SUPPORT
#define        OEMCFG_TOUCH_DISPLAYS            "TouchDisplays"
#endif
#ifdef FEATURE_HAS_MULTIPLE_DISPLAYS
#define        OEMCFG_BAM_DISPLAYS              "BamEnabledDisplays"
#endif
#ifdef FEATURE_LESS_THAN_6_KEYS
#define        OEMCFG_BAM_DISPLAY1_KEY_MAP      "KeyMapDisplay1"
#define        OEMCFG_BAM_DISPLAY2_KEY_MAP      "KeyMapDisplay2"
#define        OEMCFG_BAM_DISPLAY3_KEY_MAP      "KeyMapDisplay3"
#define        OEMCFG_BAM_DISPLAY4_KEY_MAP      "KeyMapDisplay4"
#endif

#ifdef FEATURE_HAPTIC_SUPPORT
#define		   OEMCFG_HAPTIC_RESPONSE_TONE		 "HapticTone"  	
#define		   OEMCFG_HAPTIC_RESPONSE_DURATION	 "HapticDuration" 
#endif

// Macro that initilizes state handlers in state machine
#define SET_STATE(p,nIdx,pfn,nb,bc)      {(p)->m_states[(nIdx)].pfnReq = pfn; (p)->m_states[(nIdx)].nBack = nb; (p)->m_states[(nIdx)].bClear = bc;}

// There are several Things we need to know about each individual display, that are boolean in nature
// Things like:
// * is the display touch enabled
// * is BAM alowed to run on the display
// * are there less than the 6 keys we need to run (or, are the keys that are there no u/d/l/r/Select/CRL)
// Since these are boolean in nature, and BREW supports 4 displays, these were implemented as bit fields
// instead of having a boolean for each value for each display.  We have 3 vars instead of 12.  It also 
// cleans up getting the data out of the oemcfg.txt file.
// The way these work is that each bit (0-4) is the bool value for that display (all examples use
// IsDisplayTouchEabled but are identical for the rest).
// so if the value of the var is 1 the bits are 0001, thus Display1 is touch enabled and 2,3,4 are not.
// This does not imply that they are there.  If the display does not exist, it obviously is not touch.
// More examples of the values and what they mean...
// 1 (0001) -> Is true for display 1
// 2 (0010) -> is true for display 2
// 3 (0011) -> is true for display 1 and display 2
// 4 (0100) -> is true for display 3
// 5 (0101) -> is true for display 1 and display 3
//
// The IS_DISPLAY_BIT_SET macro takes the display number as the first parameter, 1-4, and the 
// value/bit field as the second.  It returns TRUE/1 if the bit for that display is set, and 
// FALSE/0 if it is not
// so IS_DISPLAY_BIT_SET(1, 5) would return the exact same value as IS_DISPLAY_BIT_SET(3, 5)
// which is 1.
// The other macros are just more user friendly names for the ID_DISPLAY_BIT_SET so that 
// when looking at code, you can see what you are checking for.
// Macro that returns the 
#define IS_DISPLAY_BIT_SET(d, lst) (((d) > 4) || ((d) < 1))? 0 : ((lst) >> ((d) - 1) & 0x01)
#define IS_DISPLAY_TOUCH IS_DISPLAY_BIT_SET
#define IS_BAM_ENABLED_DISPLAY IS_DISPLAY_BIT_SET
#define IS_DISPLAY_SHORT_KEYS IS_DISPLAY_BIT_SET

// Configuration Version
#define MSHOP_CFG_VER   0x0001

// Configuration Struct
typedef struct MShopCfg
{
   boolean      bSearch;     // Search for apps
   boolean      bUpgrade;    // Upgrade Apps
} MShopCfg;

#if (defined(FEATURE_OEMCFG) || defined(FEATURE_INTERACT_WITH_SA_APP))
typedef struct OEMCfg
{
   IFile*            m_pIFile;                  // File pointer to get data
   char *            m_pCfgFileBuf;             // String buffer for config file
   int               m_nCfgBytes;               // Count of bytes in config file buffer
   int               m_iCfgFileBufPos;          // Current position within config file buffer
} OEMCfg;

#ifdef FEATURE_INTERACT_WITH_SA_APP
typedef struct OEMCfg AppListFile;
#endif //FEATURE_INTERACT_WITH_SA_APP
#endif // (defined(FEATURE_OEMCFG) || defined(FEATURE_INTERACT_WITH_SA_APP))

AECHAR *       AppendWStr(AECHAR * pszIn, AECHAR * pszAdd, AECHAR chAppend);
AECHAR *       LoadAppNames(IShell * pIShell, AEEAppInfo * pai, int nCount);
boolean        GetDateString(IShell * ps, uint32 dwDate, AECHAR * pszDest, int nSize);
boolean        GetExpireString(IShell* pIShell, const char* pResFile, uint32 dwNow, AEELicenseType lt, uint32 dwValue, AECHAR * pszDest, int nSize, boolean bShowInvalid, boolean bAdjust);

void           DrawHist(IDisplay * pd, AEERect * prc, uint16 wPct);
void           FormatMem(uint64 qwSize, AECHAR * pszDest, uint32 dwDestLen, boolean bWrap);
AECHAR *       FormatString(IShell * ps,const char * pszRes, uint16 wFormat, AECHAR * psz);
AECHAR *       FormatStringEx(IShell * ps, const char * pszRes, uint16 wFormat, AECHAR * psz1, AECHAR * psz2);
void           FreePtr(void ** ppObj);
void           FreeObj(void ** ppObj);
void           SetMenuStyleAndColor(IMenuCtl* pMenu, AEECLSID nMenuCls, uint16 nColorDepth, boolean isTouch);

#ifdef USE_IDL2
DLITEMID       GetItemIDFromClassID(IDownload2* pIDownload2, AEECLSID cls, DL2AppModInfo** ppai);
boolean        LockApp(IDownload2* pIDownload2, DLITEMID id, boolean bLock);
#else
DLITEMID       GetItemIDFromClassID(IDownload* pIDownload, AEECLSID cls, AppModInfo** ppai);
boolean        LockApp(IDownload* pIDownload, DLITEMID id, boolean bLock);
#endif


char *         AllocSecretString(IShell*  piShell, boolean  bUsePrevWindow);
boolean        AppendSecretString(IShell* piShell, char* pszCommand);
boolean        CheckSecretString(IShell* piShell, char* pszReceivedSecretString);

boolean        OEMCfg_NextNVPair(OEMCfg *pme, char** ppszName, char **ppszValue);
boolean        IsDisplayTouchEnabled(AEECLSID checkDisplayID, uint16 touchField);

//boolean WriteAppList(IFileMgr* pIFileMgr);
boolean WriteAppListLine(IFile* file, AECHAR* appName, AEECLSID clsid, boolean hasShortcut, uint32 numLaunches, uint32 lastLaunch);
boolean DoesAppHaveShortcut(IFileMgr* pIFileMgr, AEECLSID clsid);
boolean GetAppProperties(IFileMgr* pIFileMgr, AEECLSID clsid, boolean* hasShortcut, uint32* numLaunches, uint32* lastLaunch);



//	Deep copy / deep free download item information.

DLItem* CopyDLItem(DLItem* pItem);
void FreeDLItem(DLItem* pItem);


// Deep copy / deep free download Price information
Price* CopyPrice(Price* pPrice);
void FreePrice(Price** ppPrice);


// Copy a int16 null terminated array
uint16* CopyUInt16Array(uint16* src);


boolean DisplayEnabled(IShell* pIShell, boolean bHasMultipleDisplays,
	uint16 wEnabledDisplays, AEECLSID clsDisplay);

boolean GetKeyPressValues(const char* listOfKeys, size_t strLen, 
   uint16* upKey, uint16* downKey, uint16* selectKey, uint16* clrKey);


AEECLSID GetMenuCtlClassID(boolean bTouchEnabled);
AEECLSID GetIconViewClassID(boolean bTouchEnabled);
AEECLSID GetSoftKeyClassID(boolean bTouchEnabled);

//this function will create an object instance based on the class ID
int CreateInstance(IShell * pIShell, AEECLSID clsID, void ** ppobj);

// return TRUE if pt is in/on rc, otherwise return FALSE
boolean IsInRect(int16 wXp,int16 wYp, AEERect* pRect);
boolean IsInMenuCtl(int16 wXp,int16 wYp, IMenuCtl* pMenu);
boolean IsInStaticCtl(int16 wXp, int16 wYp, IStatic* pStatic);

boolean GetListViewMenuRect(IMenuCtl* pMenu, AEERect* prc);

boolean GetDisplayToUse(IFlip *pIFlip, AEECLSID* newDisplayId);

#ifdef FEATURE_HAPTIC_SUPPORT
void PlyHapticResponse(ISound* iSoundPtr, AEESoundToneData toneData);
#endif

#endif // BREWAPPMGRUTILS_H
