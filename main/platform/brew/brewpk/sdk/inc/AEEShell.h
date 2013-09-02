#ifndef AEESHELL_H
#define AEESHELL_H
/*======================================================
FILE:  AEEShell.h

SERVICES:  Shell Interface to AEE

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for AEEShell

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

#include "AEE.h"
#include "AEEVCodes.h"
#include "AEEDisp.h"
#include "AEEImage.h"
#include "AEESoundPlayer.h"
#include "AEEControls_res.h"
#include "AEELngCode.h"
#include "../../inc/AEEDeviceItems.h"

typedef struct _IDialog    IDialog;
typedef struct _IStatic    IStatic;

//Dialog Properties

// Event Handler to handle all events. If this property is set, the event handler
// that is set using IDIALOG_SetEventHandler() will get all events that are not
// handled by the dialog.
#define DLG_HANDLE_ALL_EVENTS 0x00010000
#ifdef CUST_EDITION	
#define DLG_NOT_SET_FOCUS_AUTO          0x00001000
#define DLG_NOT_REDRAW_AFTER_START      0x00002000
#define DLG_NOT_CLEARSCREEN_ONREDRAW    0x00004000
#define DLG_NOT_UPDATE_ONREDRAW         0x00008000
#endif /*CUST_EDITION*/
//
// Arguments sent on EVT_APP_START/EVT_APP_RESUME
//

// wParam field - Bitmask of start codes

#define AEE_START_OEM         0x0001   // Launched externally by OEM software
#define AEE_START_RESTART     0x0002   // Re-started - Failed to respond to EVT_APP_SUSPEND or closed to recover memory
#define AEE_START_SSAVER      0x0004   // App launched as screen-saver

typedef struct
{
   int         error;            // Filled by app if there is an error...
   AEECLSID    clsApp;           // Class ID of the app started
   IDisplay *  pDisplay;         // Display class
   AEERect     rc;               // Rectangle for the App
   const char *pszArgs;          // Arguments (ISHELL_StartAppletArgs)
} AEEAppStart;


#define EVTFLG_UNIQUE  0x0001   // Only one of this type should be posted...
#define EVTFLG_ASYNC   0x0002   // Event is asynchronous...

//
// Basic Resource Types
//

#define AEE_RES_EXT     ".bar"      // Extension of BREW Application Resources

#define AEE_RES_LANGDIR "{langdir}" /* string to be replaced in resource file
                                       names with "lang/" */
#define AEE_RES_LANGSUF "{langsuf}" /* string to be replaced in resource file
                                       names with "_lang" */

enum {
   RESTYPE_STRING = 1,
   RESTYPE_IMAGE  = 6,
   RESTYPE_DIALOG = 0x2000,
   RESTYPE_CONTROL= 0x2001,
   RESTYPE_LISTITEM=0x2002,
   RESTYPE_BINARY = 0x5000
};
#ifdef _WIN32
typedef unsigned ResType;
#else
typedef int16 ResType;
#endif

//
// This is the entrypoint for a module....
//

typedef int (*PFNMODENTRY)(IShell * pIShell, void * pv, IModule ** ppIModule);

//
// Screen Saver preferences
//

#define SSINFO_VER    (0x0001)

typedef struct _AEEScreenSaverInfo
{
   uint16   wSecs;
   uint16   wSecsMin;      // Read-only
   uint16   wSecsMax;      // Read-only
} AEEScreenSaverInfo;

//
// Applet Flags
//
#include "../../inc/AEEAFlags.h"

#define AFLAG_HIDDEN          AEE_AFLAG_HIDDEN        // Applet is hidden
#define AFLAG_CFG             AEE_AFLAG_CFG           // Applet has a CFG menu
#define AFLAG_SYSTEM_CFG      AEE_AFLAG_SYSTEM_CFG    // Applet has a hidden CFG menu
#define AFLAG_TOOL            AEE_AFLAG_TOOL          // Applet is a tool
#define AFLAG_GAME            AEE_AFLAG_GAME          // Applet is a game
#define AFLAG_PIM             AEE_AFLAG_PIM           // Applet is a PIM
#define AFLAG_WEB             AEE_AFLAG_WEB           // Applet is a Web Applet
#define AFLAG_PHONE           AEE_AFLAG_PHONE         // Applet is a Phone Applet (requires PL_SYSTEM)
#define AFLAG_SCREENSAVER     AEE_AFLAG_SCREENSAVER   // Applet is a Screen Saver
#define AFLAG_RUNNING         AEE_AFLAG_RUNNING       // Applet is top-visible or Suspended
#define AFLAG_POPUP           AEE_AFLAG_POPUP         // Applet is popup - no display clear
#define AFLAG_STATIC          AEE_AFLAG_STATIC        // System use only - Static application
#define AFLAG_BADSHUTDOWN     AEE_AFLAG_BADSHUTDOWN   // System use only - Applet failed to shutdown properly

//
// Applet resource ID offsets
//

#include "../../inc/AEEMIF.h"
/*
#define IDS_MIF_COMPANY       (6)         // Company Name
#define IDS_MIF_COPYRIGHT     (7)         // Copyright Date
#define IDS_MIF_VERSION       (8)         // Module version
*/

typedef struct
{
   AEECLSID       cls;
   char  *        pszMIF;           // App Resource file
   uint16         wIDBase;          // Base ID for locating title, icon, etc
   uint16         wAppType;         // Extended OEM/Carrier App Type
   uint16         wHostLo;
   uint16         wHostHi;
   uint16         wIDPrivSet;
   uint16         wFlags;           // Applet Flags (AFLAG_...)
} AEEAppInfo;

//
// Standard Applet Resource Offsets - These offsets begin at AEEAppInfo.wIDBase.
//
#define IDR_NAME_OFFSET          0
#define IDR_ICON_OFFSET          1
#define IDR_IMAGE_OFFSET         2
#define IDR_THUMB_OFFSET         3
#define IDR_SETTINGS_OFFSET      4
#define IDR_VERSION_OFFSET       5
#define IDR_ENVIRONMENT_OFFSET   6
#define IDR_OFFSET_STEP          20

//
// Applet Resource Macros
//
#define APPR_NAME(ai)         (uint16)(((ai).wIDBase + IDR_NAME_OFFSET))
#define APPR_ICON(ai)         (uint16)(((ai).wIDBase + IDR_ICON_OFFSET))
#define APPR_IMAGE(ai)        (uint16)(((ai).wIDBase + IDR_IMAGE_OFFSET))
#define APPR_THUMB(ai)        (uint16)(((ai).wIDBase + IDR_THUMB_OFFSET))
#define APPR_SETTINGS(ai)     (uint16)(((ai).wIDBase + IDR_SETTINGS_OFFSET))
#define APPR_VERSION(ai)      (uint16)(((ai).wIDBase + IDR_VERSION_OFFSET))
#define APPR_ENVIRONMENT(ai)  (uint16)(((ai).wIDBase + IDR_ENVIRONMENT_OFFSET))

//********************************************************************************************************************************
//
// for ISHELL_CheckPrivLevel, deprecated privilege bits
//
//********************************************************************************************************************************

#define PL_FILE            (0x0001)       // IFileMgr, IDBMgr
#define PL_NETWORK         (0x0002)       // INetMgr
#define PL_TAPI            (0x0004)       // ITAPI (simple) Access
#define PL_WEB             (0x0008)       // IWeb Access (Also available if PL_NETWORK)
#define PL_DOWNLOAD        (0x0020)       // IDownload
#define PL_SHARED_WRITE    (0x0040)       // Write Access - Shared Directory
#define PL_POS_LOCATION    (0x0080)       // Position location access (ISHELL_GetLocation)
#define PL_RINGER_WRITE    (0x0100)       // Write Access - Ringer Directory
#define PL_RINGER          (0x0100)       // Access - Ringer Directory,
#define PL_ADDRBOOK        (0x0200)       // Access to Address Book
#define PL_SECTORINFO      (0x0400)       // GetSectorInfo
#define PL_SYSTEM          (0xffff)


//
// These privilege levels are required to access derived AKey, ESN, BKEY, SID, DLFLAGS or MEIDS
// ESN and MEIDS may also be accessed with the AEECLSID_HWIDS privilege
#define AEECLSID_CDMACREDENTIALS       0x0101595b
#define AEECLSID_BREWCREDENTIALS       0x0101595c

//
// This privilege level required to access hardware IDs
// via AEE_DEVICEITEM_HWID and AEE_DEVICEITEM_HWIDLIST
// This privilege will also allow access to the equivalent
// AEE_DEVICEITEM_ESN, AEE_DEVICEITEM_IMEI AND AEE_DEVICEITEM_MEIDS
//
#define AEECLSID_HWIDS       0x0102febd

//
// This privilege level required to access IMEI
// IMEI may also be accessed with AEECLSID_HWIDS privilege
//
#define AEECLSID_GSMCREDENTIALS        0x0101e4e1

// This privilege level required to access MDN
#define AEECLSID_MDN                   0x0101b3ad

//********************************************************************************************************************************
//
// IDialog Definition
//
//********************************************************************************************************************************

typedef struct _DListItem
{
   uint16      wID;           // ID of the item
   uint16      wTextID;       // Text
   uint16      wIconID;       // Icon ID
   uint16      pad;
   uint32      dwData;        // 32-bit data
} DListItem;

typedef struct
{
   AEECLSID    cls;           // class of the control
   uint16      wID;           // ID of the control
   uint16      nItems;        // Item count
   uint32      dwProps;       // See IControl (or specific control)
   uint16      wTextID;       // ID of initial text
   uint16      wTitleID;      // ID of initial title
   AEERect     rc;            // Rect - Relative to Dialog (-1, -1, -1, -1) is default
} DialogItemHead;

//
// Dialog Item passed directly to ISHELL_Dialog
//
typedef struct
{
   DialogItemHead h;
   DListItem      items[1];
} DialogItem;

//
// Dialog Item found in resource file...
//

typedef struct
{
   DialogItemHead h;
   uint16         wItemIDs[1];
} DialogItemRes;

typedef struct
{
   uint16      wID;           // Dialog ID
   uint16      nControls;     // Number of controls
   AEERect     rc;            // Rect (-1, -1, -1, -1) is default
   uint32      dwProps;       // See IControl
   uint16      wTitle;        // Title
   uint16      wFocusID;      // ID of focus control
} DialogInfoHead;

//
// Dialog Header passed directly to ISHELL_Dialog
//

typedef struct
{
   DialogInfoHead h;
   DialogItem     controls[1];
} DialogInfo;

//
// Dialog Header found in resource file...
//
typedef struct
{
   DialogInfoHead h;
   uint16         wControlIDs[1];
} DialogInfoRes;

typedef boolean (*PFNAEEEVENT)(void * pUser, AEEEvent evt, uint16 w, uint32 dw);

QINTERFACE(IDialog)
{
   DECLARE_IBASE(IDialog)

   int16       (*SetFocus)(IDialog * po, int16 wID);
   IControl *  (*GetControl)(IDialog * po, int16 wID);
   void        (*SetEventHandler)(IDialog * po, PFNAEEEVENT pfn, void * pData);
   uint16      (*GetID)(IDialog * po);
   void        (*SetProperties)(IDialog *po, uint32 props);
   uint32      (*GetProperties)(IDialog *po);
   void        (*Redraw)(IDialog * po);
};

#define IDIALOG_AddRef(p)                       GET_PVTBL(p,IDialog)->AddRef(p)
#define IDIALOG_Release(p)                      GET_PVTBL(p,IDialog)->Release(p)
#define IDIALOG_SetFocus(p,wID)                 GET_PVTBL(p,IDialog)->SetFocus(p, wID)
#define IDIALOG_GetControl(p,wID)               GET_PVTBL(p,IDialog)->GetControl(p, wID)
#define IDIALOG_SetEventHandler(p, pfn, pData)  GET_PVTBL(p,IDialog)->SetEventHandler(p,pfn,pData)
#define IDIALOG_GetID(p)                        GET_PVTBL(p,IDialog)->GetID(p)
#define IDIALOG_SetProperties(p,props)          GET_PVTBL(p,IDialog)->SetProperties(p,props)
#define IDIALOG_GetProperties(p)                GET_PVTBL(p,IDialog)->GetProperties(p)
#define IDIALOG_Redraw(p)                       GET_PVTBL(p,IDialog)->Redraw(p)

//********************************************************************************************************************************
//
// IShell Definition
//
//********************************************************************************************************************************

// Standard Busy Cursors

enum {
   CT_NONE,
   CT_BUSY
};
#ifdef _WIN32
typedef unsigned AEECursor;
#else
typedef int8 AEECursor;
#endif

//
// Item Properties - This structure controls how items in many standard objects
// such as lists, icon views, softkeys, etc. are drawn.  Note that the OEM will
// likely default these settings to those appropriate for the existing User Interface.
//

enum {
   AEE_IT_MENU,
   AEE_IT_SOFTKEY,
   AEE_IT_ICONVIEW
};
#ifdef _WIN32
typedef unsigned AEEItemType;
#else
typedef int8 AEEItemType;
#endif

typedef struct
{
   AEEFrameType   ft;         // Frame type for the item
   uint16         xOffset;    // X padding inside item (does not include frame size)
   uint16         yOffset;    // Y padding inside item (does not include frame size)
   AEERasterOp    roImage;    // Raster operation for drawing images inside the item
} AEEItemStyle;

// Encoding constants are returned in AEEDeviceInfo.wEncoding

#if !defined(AEE_ENC_UNICODE)
#define AEE_ENC_UNICODE   0xFF
#define AEE_ENC_UNICODE2  0xFE
#endif

#if !defined(AEE_ENC_KSC5601) || !defined(AEE_ENC_EUC_KR)
#define AEE_ENC_KSC5601   0xFE
#define AEE_ENC_EUC_KR    AEE_ENC_KSC5601
#endif

#if !defined(AEE_ENC_S_JIS)
#define AEE_ENC_S_JIS     0xFD
#endif

#if !defined(AEE_ENC_EUC_CN) // Simplified Chinese
#define AEE_ENC_EUC_CN     0xFC
#endif

#if !defined(OEM_RAW_TEXT)
#define OEM_RAW_TEXT      0x0FA
#endif

#if !defined(AEE_ENC_UTF8)
#define AEE_ENC_UTF8      2
#endif

#if !defined(AEE_ENC_ISOLATIN1)
#define AEE_ENC_ISOLATIN1 3
#endif

#if !defined(AEE_ENC_LATIN_HEBREW)
#define AEE_ENC_LATIN_HEBREW 4
#endif

#if !defined(AEE_ENC_GSM_7_BIT_DEFAULT)
#define AEE_ENC_GSM_7_BIT_DEFAULT 5
#endif

#if !defined(AEE_ENC_OCTET)
#define AEE_ENC_OCTET 6
#endif

//
// Device Information
//

enum {
   UNUSED1,
   UNUSED2
};
#ifdef _WIN32
typedef unsigned EmptyEnum;
#else
typedef int8 EmptyEnum;
#endif

typedef struct
{
   uint16            cxScreen;         // Physical screen size (pixels)
   uint16            cyScreen;         // Physical screen size (pixels)
   uint16            cxAltScreen;      // Physical screen size of 2nd display (Pager)
   uint16            cyAltScreen;      // Physical screen size of 2nd display (Pager)
   uint16            cxScrollBar;      // Width of standard scroll bars
   uint16            wEncoding;        // Character set encoding (AEE_ENC_UNICODE, ....)
   uint16            wMenuTextScroll;  // Milliseconds that should be used for auto-scroll
   uint16            nColorDepth;      // Color Depth (1 = mono, 2 = grey, etc.)
   EmptyEnum         unused2;
   uint32            wMenuImageDelay;  // Milliseconds image Draw Delay for SINGLE_FRAME Icon View 
                                       // with MP_ICON_DELAY_IMAGE property
   uint32            dwRAM;            // Initial size of BREW heap
   flg               bAltDisplay:1;    // Device has an alternate display (Pager)
   flg               bFlip:1;          // Device sends the flip event (EVT_FLIP) when the clamshell is opened/closed
   flg               bVibrator:1;      // Vibrator installed
   flg               bExtSpeaker:1;    // External speaker installed
   flg               bVR:1;            // Voice recognition supported
   flg               bPosLoc:1;        // Position location supported
   flg               bMIDI:1;          // MIDI file formats supported
   flg               bCMX:1;           // CMX audio supported
   flg               bPen:1;           // Pen device supported
   uint32            dwPromptProps;    // Default prompt properties
   uint16            wKeyCloseApp;     // Key to close current app
   uint16            wKeyCloseAllApps; // Key to close all apps (AVK_END is default)
   uint32            dwLang;           // Used by Resource Loader - See AEE_LNG_XXX below

   //
   // NOTE: In order to use the following fields, you MUST fill-in the wStructSize
   // element of the structure before passing this to the GetDeviceInfo call.
   //

   uint16            wStructSize;      // Size of this structure.
   uint32            dwNetLinger;      // PPP linger timer in milliseconds
   uint32            dwSleepDefer;     // Active non-sleep in milliseconds
   uint16            wMaxPath;         // Max length of file path
   uint32            dwPlatformID;     // Platform ID
} AEEDeviceInfo;

//Parameter definitions to be passed to ISHELL_GetDeviceInfoEx()
typedef int AEEDeviceItem;

#define AEE_MANNER_MODE_NORMAL                0  // Normal audio
#define AEE_MANNER_MODE_VIBRATE               1  // Vibrate only mode
#define AEE_MANNER_MODE_SILENT                2  // Silent mode -- no audio/vibrate

//
// KeySupportType - passed to ISHELL_GetDeviceInfoEx() for AEE_DEVICEITEM_KEY_SUPPORT
//
typedef struct
{
   AVKType  key;        // input
   boolean  supported;  // output
} KeySupportType;

//
// AEEKeyInfoType - passed to ISHELL_GetDeviceInfoEx() for AEE_DEVICESTATE_KEY_INFO
//
typedef struct
{
   AVKType wKey;       // can be any valid AVK key code (BREW or OEM range).
   uint32  flag;       // flag for which info is needed.
   void*   pData;      // flag specific data (in/out).
   uint32* pnDataSize; // size of data.
} AEEKeyInfoType;

// Flags used in AEEKeyInfoType
#define AEE_DEVICESTATEFLAG_KEY_PRESSED            0x00000001
#define AEE_DEVICESTATEFLAG_KEY_AVAILABLE          0x00000020
#define AEE_DEVICESTATEFLAG_KEY_OVERLOADED_INFO    0x00000040

// Type of Data to be returned for a specific flag in AEEKeyInfoType:
// Data Type for AEE_DEVICESTATEFLAG_KEY_PRESSED and AEE_DEVICESTATEFLAG_KEY_AVAILABLE
// is boolean.
// Data Type for AEE_DEVICESTATEFLAG_KEY_OVERLOADED_INFO is AVKType.

//
// Items for AEE_DEVICEITEM_REPEAT_TIME
//
#define AEEREPEAT_SCROLLBAR                  (0)         // Scrollbar auto-repeat values

#define OEMREPEAT_BASE                       (0x8000)    // Base for OEM specific auto-repeat values
//
// Defaults for AEE_DEVICEITEM_REPEAT_TIME items
//
// Scrollbar [AEEREPEAT_SCROLLBAR] default auto-repeat rates
#define SB_AUTOREPEAT_START                  (200)
#define SB_AUTOREPEAT_RATE                   (75)

//
// AEEAutoRepeat - passed to ISHELL_GetDeviceInfoEx() for AEE_DEVICEITEM_REPEAT_TIME
//
typedef struct AEEAutoRepeat
{
   uint16   wItem;            // In parameter
   uint32   dwStart;          // MilliSeconds, Default depends on item request
   uint32   dwRate;           // MilliSeconds, Default depends on item request
} AEEAutoRepeat;

//
// AEEFSFreeSpace - passed to ISHELL_GetDeviceInfoEx() for AEE_DEVICEITEM_FSSPACE_CARD0
// , AEE_DEVICEITEM_FSSPACE_ROOT, AEE_DEVICEITEM_FSSPACE_USBHMS1 and
// AEE_DEVICEITEM_FSSPACE_USBHMS2
//
typedef struct AEEFSFreeSpace
{
   uint64   qwTotalSpace; // [out] Total Space in bytes on the filesystem
   uint64   qwFreeSpace;  // [out] Free Space in bytes on the filesystem
} AEEFSFreeSpace;

//
// AEECardInfo - passed to ISHELL_GetDeviceInfoEx() for AEE_DEVICEITEM_CARD0_INFO
//
typedef struct _AEECardInfo {
   char *   pszSerialNum;        // char buffer for the serial number of the MMC card. Buffer memory must be provided by user. (in)
   int		nSerialNumSize;		 // Size of the serial number buffer. If error is returned, this value is set to the recommended size. (in/out)
   char *   pszModelNum;         // char buffer for the model number (product name) of the MMC card. Buffer memory must be provided by user. (in)
   int		nModelNumSize;		 // Size of the model number buffer. If error is returned, this value is set to the recommended size. (in/out)
} AEECardInfo;

//
// BEEP TYPES - Note that in-call issues are handled automatically.  The caller does not
// need to deal with this.
//
// Vibrate Beeps will NOT function when a call is in-progress...
//

enum {
   BEEP_OFF,
   BEEP_ALERT,
   BEEP_REMINDER,
   BEEP_MSG,
   BEEP_ERROR,
   BEEP_VIBRATE_ALERT,
   BEEP_VIBRATE_REMIND
};
#ifdef _WIN32
typedef unsigned BeepType;
#else
typedef int8 BeepType;
#endif


#define ONE_YEAR_SECS      31536000L    /* one year in seconds */


// This bias value was chosen to set the Sunday before
// 1/1/70 (the time-zero value) at zero
#define JDAY_OFFSET        719465

// Some constants for julian date converion
#define DAYS_PER_5_MONTHS  153
#define DAYS_PER_4_YEARS   1461
#define DAYS_PER_400_YEARS 146097

// Handler Type - Mime/Scheme, ***deprecated****
//
// HTYPE_VIEWER - Returns class of associated IViewer, match mimetypes
//                 of the form "image/*"
//
// HTYPE_SOUND - Returns class of associated ISoundPlayer, match mimetypes
//                 of the form "snd/*"
//
// HTYPE_BROWSE - Returns class of IApplet (extension or scheme),
//                  matches everything else
//

typedef AEECLSID AEEHandlerType;

#define HTYPE_VIEWER 0
#define HTYPE_SOUND  1
#define HTYPE_BROWSE 2


typedef struct
{
   const char  *  pszRes;
   const AECHAR * pTitle;
   const AECHAR * pText;
   uint16         wTitleID;
   uint16         wTextID;
   uint16         wDefBtn;       // Command to send when CLR is hit (default is first entry)
   const uint16 * pBtnIDs;
   uint32         dwProps;
   AEEFont        fntTitle;
   AEEFont        fntText;
   uint32         dwTimeout;     // Timeout - Default button is sent/dialog dismissed
} AEEPromptInfo;

//
// Position Location Information
//
typedef struct
{
   int32          dwLat;
   int32          dwLon;
   uint32         dwTimeStamp;
} AEEPositionInfo;


typedef void (*PFNPOSITIONCB)(void * pUser, AEEPositionInfo * pli, int nErr);

enum {
   AEE_ACCURACY_LOW,
   AEE_ACCURACY_MED,
   AEE_ACCURACY_HIGH
};
#ifdef _WIN32
typedef unsigned AEEPosAccuracy;
#else
typedef int8 AEEPosAccuracy;
#endif

// Start Applet Flags

#define SSA_NORMAL            0
#define SSA_NO_VOICE_CALL     0x0001      // Do not start if a voice call is in progress
#define SSA_AFTER_VOICE_CALL  0x0002      // Start app after a voice call if there is one in progress
#define SSA_CONFIG            0x0004      // Configuration screen (if applicable)
#define SSA_HIDDEN_CONFIG     0x0008      // Hidden configuration (if applicable)

#define SSA_UNIQUE            0x0020      // Force a new/unique instance of app
#define SSA_SSAVER            0x0040      // App started in screen saver mode

#define SSA_BACKGROUND        0x0100      // App started in background

// AEEResBlob, internal resource structure

typedef struct AEEResBlob
{
   byte  bDataOffset;     /* how far into the Blob the real data starts */
   byte  bZero;           /* always zero, is padding in 1.x BREW */

   char  szMimeType[1];   /* null-terminated string (not just [1]) mimetype,
                             can be up to 249 characters in length */
} AEEResBlob;

#define RESBLOB_DATA(b) ((void *)((byte *)(b) + (b)->bDataOffset))

//
// Network IO tracking
//

typedef struct
{
   uint32   dwTCPSent;
   uint32   dwTCPRecv;
   uint32   dwUDPSent;
   uint32   dwUDPRecv;
} AEENetUsage;

//
// App usage
//

typedef struct
{
   uint32   dwRunCount;       // Number of times run
   uint32   dwMinsOfUse;      // Total minutes of use
   uint32   dwLastUsedTime;   // Time of last use
} AEEAppUsage;

//
// Module Properties
//

#define MPROP_PROTECTED    (0x8000)                // Requires system-level privs.
#define MPROP_SID          (1 | (MPROP_PROTECTED))
#define MPROP_NETUSAGE     (2)
#define MPROP_USAGE        (3)                     // App runtime usage
#define MPROP_USESSTRING   (4)
#define MPROP_STATUS       (5)                     // App runtime status string (used by app mgr)

// Note: To obtain the status string for an app, do the following...
//
// if(!ISHELL_GetProperty(pShell, cls, NULL, &nSize) && nSize){
//    psz = (AECHAR *)MALLOC(nSize);
//    if(psz)
//       ISHELL_GetProperty(pShell, cls, (void *)psz, nSize);
//

//
// Direct-call system event notifications.  This mechanism is provided to allow components to
// request direct callbacks for system-level events.  The events are defined below.  The callback
// is a one-shot callback.  If necessary, the component may re-register the notification during
// the callback.
//

#define AEE_SCB_AEE_INIT               (0)
#define AEE_SCB_AEE_EXIT               (1)
#define AEE_SCB_LOW_RAM                (2)
#define AEE_SCB_LOW_STORAGE            (3)
#define AEE_SCB_APP_CLOSED             (4)
#define AEE_SCB_MOD_UNLOAD             (5)
#define AEE_SCB_DEVICE_INFO_CHANGED    (6)
#define AEE_SCB_LOW_RAM_CRITICAL       (7)
#define AEE_SCB_MAX                    (AEE_SCB_LOW_RAM_CRITICAL)
#define AEE_SCB_APP_EXIT               (8)


//
//  Shell Notifications - Can be registered using AEECLSID_SHELL and the mask.
//

#define NMASK_SHELL_INIT                  (0x0001)
#define NMASK_SHELL_START_STATUS          (0x0002)
#define NMASK_SHELL_APP_EXPIRED           (0x0004) // Sent to app when it expires.  This mask is not implemented.
#define NMASK_SHELL_APP_LICENSE_CHANGED   (0x0008) // Sent when app's license is updated
#define NMASK_SHELL_MOD_LIST_CHANGED      (0x0010) // Sent when list of modules changes
#define NMASK_SHELL_SYS_ERROR             (0x0020) // System error (See SYSERR_XXXX - AEE.h)
#define NMASK_SHELL_KEY                   (0x0040) // Register to receive EVT_KEY_xxx for
                                                   // a single key - keycode in upper 16 bits
                                                   // use NOTIFIER_VAL_ANY for keycode to
                                                   // receive all key events.

//
// special ISHELL_CancelAlarm() codes for AEEGROUPID_ALARM_MGR
//
#define AEE_ALARM_SUSPEND                 0
#define AEE_ALARM_RESUME                  1

//Notification Data for NMASK_SHELL_START_STATUS
typedef struct _NotifyStartStatus
{
   AEECLSID    cls;  // The Class which had problems in being started
   int         nErr; // The Error Code
} NotifyStartStatus;

//Notification Data for NMASK_SHELL_KEY
typedef struct _NotifyKeyEvent {
   AEEEvent eCode;
   uint16   wParam;
   uint32   dwParam;
   AEECLSID clsHandled;
} NotifyKeyEvent;

typedef enum
{
   AEERESET_HARD,
   AEERESET_SOFT
} AEEResetType;

// Hardare ID Types (see AEE_DEVICEITEM_HWID)
#define AEE_HWID_NONE   0     // 
#define AEE_HWID_ESN    1     // 32 bit ESN
#define AEE_HWID_PESN   2     // 32 bit Pseudo ESN
#define AEE_HWID_IMEI   3     // 56 bit IMEI
#define AEE_HWID_MEID   4     // 56 bit MEID
//#define AEE_HWID_UIMID  5   // 32 bit RUIM ID - not supported

typedef struct {
   uint16   uLength;
   byte     bHWIDLength;
   byte     bHWIDType;     // AEE_HWID_xxx
   uint32   HWID[1];
} AEEHWIDType;

#define INHERIT_IShell(iname ) \
   INHERIT_IBase(iname); \
   int            (*CreateInstance)(iname * po, AEECLSID ClsId, void ** ppobj); \
   boolean        (*QueryClass)(iname * po, AEECLSID cls, AEEAppInfo * pai); \
   void           (*GetDeviceInfo)(iname * po, AEEDeviceInfo * pi); \
   int            (*StartApplet)(iname * po, AEECLSID cls,uint16 wFlags, const char * pszArgs); \
   int            (*CloseApplet)(iname * po, boolean bReturnToIdle); \
   boolean        (*CanStartApplet)(iname * po, AEECLSID cls); \
   AEECLSID       (*ActiveApplet)(iname * po); \
   void           (*EnumAppletInit)(iname * po); \
   AEECLSID       (*EnumNextApplet)(iname * po, AEEAppInfo * pai); \
   int            (*SetTimer)(iname * po, int32 dwMsecs, PFNNOTIFY pfn, void * pUser); \
   int            (*CancelTimer)(iname * po, PFNNOTIFY pfn, void * pUser); \
   uint32         (*GetTimerExpiration)(iname * po, PFNNOTIFY pfn, void * pUser); \
   int            (*CreateDialog)(iname * po, const char * pszRes, uint16 wID, DialogInfo * pInfo); \
   IDialog *      (*GetActiveDialog)(iname * po); \
   int            (*EndDialog)(iname * po); \
   int            (*LoadResString)(iname * po, const char * pszBaseFile, uint16 nResID, AECHAR * pBuff, int nSize); \
   void *         (*LoadResData)(iname * po, const char * pszResFile, uint16 nResID, ResType nType); \
   IBase *        (*LoadResObject)(iname * po,const char * pszResFile, uint16 nResID, AEECLSID cls); \
   void           (*FreeResData)(iname * po, void * pData); \
   boolean        (*SendEvent)(iname * po, uint16 wFlags, AEECLSID clsApp,AEEEvent evt, uint16 wParam, uint32 dwParam); \
   boolean        (*Beep)(iname * po, BeepType nBeepType, boolean bLoud); \
   int            (*GetPrefs)(iname * po, AEECLSID id,uint16 wVer, void * pCfg, uint16 nSize); \
   int            (*SetPrefs)(iname * po, AEECLSID id,uint16 wVer, void * pCfg, uint16 nSize); \
   void           (*GetItemStyle)(iname * po, AEEItemType t, AEEItemStyle * pNormal, AEEItemStyle * pSel); \
   boolean        (*Prompt)(iname * po, AEEPromptInfo * pi); \
   boolean        (*MessageBox)(iname * po, const char * pszRes, uint16 wTitle, uint16 wText); \
   boolean        (*MessageBoxText)(iname * po, const AECHAR * pTitle, const AECHAR * pText); \
   int            (*SetAlarm)(iname * po, AEECLSID cls, uint16 nUserCode, uint32 nMins); \
   int            (*CancelAlarm)(iname * po, AEECLSID cls, uint16 nUserCode); \
   boolean        (*AlarmsActive)(iname * po); \
   AEECLSID       (*GetHandler)(iname * po, AEECLSID cls, const char * pszIn); \
   int            (*RegisterHandler)(iname * po, AEECLSID clsBase, const char * pszIn, AEECLSID clsHandler); \
   int            (*RegisterNotify)(iname * po, AEECLSID clsNotify, AEECLSID clsType, uint32 dwMask); \
   int            (*Notify)(iname * po,AEECLSID clsType, uint32 dwMask, void * pData); \
   void           (*Resume)(iname * po, AEECallback * pcb); \
   boolean        (*ForceExit)(iname * po); \
   int            (*GetPosition)(iname * po, AEEPosAccuracy nPres, PFNPOSITIONCB pfn, void * pUser); \
   boolean        (*CheckPrivLevel)(iname * po, AEECLSID clsIDWant, boolean bQueryOnly); \
   boolean        (*IsValidResource)(iname * po, const char * pszRes, uint16 wID,ResType t, AEECLSID cls); \
   void *         (*LoadResDataEx)(iname * po, const char * pszResFile, uint16 nResID, ResType nType, void *pBuf, uint32 *pnLen); \
   void           (*RegisterSystemCallback)(iname * po, AEECallback * pcb, int nSCBType); \
   int            (*DetectType)(iname * po, const void * cpBuf, uint32 * pdwSize, const char * cpszName, const char ** pcpszMIME); \
   int            (*GetDeviceInfoEx)(iname * po, AEEDeviceItem nItem, void *pBuff, int *nSize); \
   uint32         (*GetClassItemID)(iname * po,AEECLSID cls); \
   int            (*Obsolete)(iname * po); \
   int            (*GetProperty)(iname * po,AEECLSID cls,uint16 wID, void * pDest, uint16 * pwSize); \
   int            (*SetProperty)(iname * po,AEECLSID cls,uint16 wID, void * pSrc, uint16 wSize); \
   AEEEvent       (*RegisterEvent)(iname * po, const char * psz, int * pnCount); \
   int            (*Reset)(iname * po,AEEResetType t); \
   int            (*AppIsInGroup)(iname * po, AEECLSID idApp, AEECLSID idGroup)

QINTERFACE(IShell)
{
   INHERIT_IShell(IShell);
};

// Access macros for IShell

#define ISHELL_AddRef(p)                        GET_PVTBL(p,IShell)->AddRef(p)
#define ISHELL_Release(p)                       GET_PVTBL(p,IShell)->Release(p)
#define ISHELL_CreateInstance(p,id,ppo)         GET_PVTBL(p,IShell)->CreateInstance(p,id,ppo)
#define ISHELL_QueryClass(p, cls, pai)          GET_PVTBL(p,IShell)->QueryClass(p, cls, pai)
#define ISHELL_GetDeviceInfo(p,pi)              GET_PVTBL(p,IShell)->GetDeviceInfo(p,pi)
#define ISHELL_GetDeviceInfoEx(p,n,pBuf,pn)     GET_PVTBL(p,IShell)->GetDeviceInfoEx(p,n,pBuf,pn)
#define ISHELL_StartApplet(p,cls)               GET_PVTBL(p,IShell)->StartApplet(p,cls,SSA_NORMAL,NULL)
#define ISHELL_StartAppletEx(p,cls,flags)       GET_PVTBL(p,IShell)->StartApplet(p,cls,flags,NULL)
#define ISHELL_StartBackgroundApplet(p,cls,psz) GET_PVTBL(p,IShell)->StartApplet(p,cls,SSA_BACKGROUND,psz)
#define ISHELL_BrowseFile(p,psz)                GET_PVTBL(p,IShell)->StartApplet(p,(AEECLSID)0,SSA_NORMAL, psz)
#define ISHELL_BrowseURL(p,psz)                 GET_PVTBL(p,IShell)->StartApplet(p,(AEECLSID)0,SSA_NORMAL, psz)
#define ISHELL_StartAppletArgs(p,cls,psz)       GET_PVTBL(p,IShell)->StartApplet(p,cls,SSA_NORMAL,psz)
#define ISHELL_CloseApplet(p,bi)                GET_PVTBL(p,IShell)->CloseApplet(p,bi)

#define ISHELL_ActiveApplet(p)                  GET_PVTBL(p,IShell)->ActiveApplet(p)
#define ISHELL_EnumAppletInit(p)                GET_PVTBL(p,IShell)->EnumAppletInit(p)
#define ISHELL_EnumNextApplet(p, pai)           GET_PVTBL(p,IShell)->EnumNextApplet(p,pai)
#define ISHELL_CanStartApplet(p,cls)            GET_PVTBL(p,IShell)->CanStartApplet(p,cls)
#define ISHELL_SetTimer(p,s,pfn,pu)             GET_PVTBL(p,IShell)->SetTimer(p,s,pfn,pu)
#define ISHELL_SetTimerEx(p,s,pcb)              GET_PVTBL(p,IShell)->SetTimer(p,s,(PFNNOTIFY)(void *)pcb,(void *)pcb)
#define ISHELL_CancelTimer(p,pfn,pu)            GET_PVTBL(p,IShell)->CancelTimer(p,pfn,pu)
#define ISHELL_GetTimerExpiration(p, pfn, pu)   GET_PVTBL(p,IShell)->GetTimerExpiration(p, pfn,pu)
#define ISHELL_IsValidResource(p,psz,n,t,ht)    GET_PVTBL(p,IShell)->IsValidResource(p,psz,n,t,ht)

#define ISHELL_LoadResString(p,pname,id,b,s)    GET_PVTBL(p,IShell)->LoadResString(p,pname,id,b,s)
#define ISHELL_LoadResData(p,psz,id,t)          GET_PVTBL(p,IShell)->LoadResData(p, psz, id, t)
#define ISHELL_LoadResObject(p,psz,id,t)        GET_PVTBL(p,IShell)->LoadResObject(p,psz,id,t)
#define ISHELL_FreeResData(p,pbm)               GET_PVTBL(p,IShell)->FreeResData(p,pbm)
#define ISHELL_LoadResDataEx(p,psz,id,t,b,l)    GET_PVTBL(p,IShell)->LoadResDataEx((p),(psz),(id),(t),(b),(l))
#define ISHELL_GetResSize(p,psz,id,t,l)         (GET_PVTBL(p,IShell)->LoadResDataEx((p),(psz),(id),(t),(void *)-1,(l)),*l)

#define ISHELL_Beep(p,t,b)                      GET_PVTBL(p,IShell)->Beep(p,t,b)
#define ISHELL_GetPrefs(p,id,v,pc,n)            GET_PVTBL(p,IShell)->GetPrefs(p,id,v,pc,n)
#define ISHELL_GetAppPrefs(p,v,pc,n)            GET_PVTBL(p,IShell)->GetPrefs(p,(AEECLSID)0,v,pc,n)
#define ISHELL_SetPrefs(p,id,v,pc,n)            GET_PVTBL(p,IShell)->SetPrefs(p,id,v,pc,n)
#define ISHELL_SetAppPrefs(p,v,pc,n)            GET_PVTBL(p,IShell)->SetPrefs(p,(AEECLSID)0,v,pc,n)
#define ISHELL_Prompt(p,pi)                     GET_PVTBL(p,IShell)->Prompt(p,pi)
#define ISHELL_ShowCopyright(p)                 GET_PVTBL(p,IShell)->Prompt(p,NULL)
#define ISHELL_MessageBoxText(p,pti,ptx)        GET_PVTBL(p,IShell)->MessageBoxText(p,pti,ptx)
#define ISHELL_MessageBox(p,pr,wt,wtx)          GET_PVTBL(p,IShell)->MessageBox(p,pr,wt,wtx)
#define ISHELL_GetItemStyle(p,t,pn,ps)          GET_PVTBL(p,IShell)->GetItemStyle(p,t,pn,ps)

#define ISHELL_SetAlarm(p,cls,uc,nm)            GET_PVTBL(p,IShell)->SetAlarm(p,cls,uc,nm)
#define ISHELL_CancelAlarm(p,cls,uc)            GET_PVTBL(p,IShell)->CancelAlarm(p,cls,uc)
#define ISHELL_SuspendAlarms(p)                 GET_PVTBL(p,IShell)->CancelAlarm(p,AEEGROUPID_ALARM_MGR,AEE_ALARM_SUSPEND)
#define ISHELL_ResumeAlarms(p)                  GET_PVTBL(p,IShell)->CancelAlarm(p,AEEGROUPID_ALARM_MGR,AEE_ALARM_RESUME)
#define ISHELL_AlarmsActive(p)                  GET_PVTBL(p,IShell)->AlarmsActive(p)

#define ISHELL_GetHandler(p,t,psz)              GET_PVTBL(p,IShell)->GetHandler(p,t,psz)
#define ISHELL_RegisterHandler(p,t,psz,cls)     GET_PVTBL(p,IShell)->RegisterHandler(p,t,psz,cls)
#define ISHELL_RegisterNotify(p,cls,clst,dw)    GET_PVTBL(p,IShell)->RegisterNotify(p,cls,clst,dw)
#define ISHELL_Notify(p,ct,dw,pd)               GET_PVTBL(p,IShell)->Notify(p,ct,dw,pd)

#define ISHELL_RegisterSystemCallback(p,pcb,i)  GET_PVTBL(p,IShell)->RegisterSystemCallback(p,pcb,i)
#define ISHELL_OnLowRAM(ps,pcb)                 ISHELL_RegisterSystemCallback(ps,pcb,AEE_SCB_LOW_RAM)
#define ISHELL_OnLowRAMCritical(ps,pcb)         ISHELL_RegisterSystemCallback(ps,pcb,AEE_SCB_LOW_RAM_CRITICAL)
#define ISHELL_OnLowStorage(ps,pcb)             ISHELL_RegisterSystemCallback(ps,pcb,AEE_SCB_LOW_STORAGE)
#define ISHELL_OnAppClose(ps,pcb)               ISHELL_RegisterSystemCallback(ps,pcb,AEE_SCB_APP_CLOSED)
#define ISHELL_OnModUnload(ps,pcb)              ISHELL_RegisterSystemCallback(ps,pcb,AEE_SCB_MOD_UNLOAD)
#define ISHELL_OnExit(ps,pcb)                   ISHELL_RegisterSystemCallback(ps,pcb,AEE_SCB_AEE_EXIT)
#define ISHELL_OnAppExit(ps,pcb)                ISHELL_RegisterSystemCallback(ps,pcb,AEE_SCB_APP_EXIT)

#define ISHELL_Resume(p,pr)                     GET_PVTBL(p,IShell)->Resume(p,pr)

#define ISHELL_ForceExit(p)                     GET_PVTBL(p,IShell)->ForceExit(p)

#define ISHELL_CreateDialog(p,psz,wID,pi)       GET_PVTBL(p,IShell)->CreateDialog(p,psz,wID,pi)
#define ISHELL_GetActiveDialog(p)               GET_PVTBL(p,IShell)->GetActiveDialog(p)
#define ISHELL_EndDialog(p)                     GET_PVTBL(p,IShell)->EndDialog(p)

#define ISHELL_SendEvent(p,cls,ec,wp,dw)        GET_PVTBL(p,IShell)->SendEvent(p,0,cls,ec,wp, dw)
#define ISHELL_PostEvent(p,cls,ec,wp,dw)        GET_PVTBL(p,IShell)->SendEvent(p,(EVTFLG_ASYNC|EVTFLG_UNIQUE),cls,ec,wp,dw)
#define ISHELL_PostEventEx(p,flgs,cls,ec,wp,dw) GET_PVTBL(p,IShell)->SendEvent(p,(uint16)((EVTFLG_ASYNC)|(flgs)),cls,ec,wp,dw)
#define ISHELL_HandleEvent(p,ec,wp,dw)          GET_PVTBL(p,IShell)->SendEvent(p,0,(AEECLSID)0,ec,wp,dw)
#define ISHELL_GetPosition(p,np,pfn,pu)         GET_PVTBL(p,IShell)->GetPosition(p,np,pfn,pu)

#define ISHELL_Busy(p)                          (ISHELL_ForceExit(p))

#define ISHELL_LoadResImage(p,psz,id)           (IImage *)(void *)ISHELL_LoadResObject(p,psz,id,HTYPE_VIEWER)
#define ISHELL_LoadImage(p,psz)                 ISHELL_LoadResImage(p,psz,0)
#define ISHELL_LoadBusyImage(p)                 ISHELL_LoadResImage(p, AEECONTROLS_RES_FILE, AEE_IDB_BUSY)
#define ISHELL_LoadResSound(p,psz,id)           (ISoundPlayer *)(void *)ISHELL_LoadResObject(p,psz,id,HTYPE_SOUND)
#define ISHELL_LoadSound(p,psz)                 ISHELL_LoadResSound(p,psz,0)
#define ISHELL_LoadResBitmap(p,psz,id)          (IBitmap *)(void *)ISHELL_LoadResObject(p,psz,id,AEECLSID_BITMAP)
#define ISHELL_LoadBitmap(p,psz)                ISHELL_LoadResBitmap(p,psz,0)


#define ISHELL_CheckPrivLevel(p, w,b)           GET_PVTBL(p,IShell)->CheckPrivLevel(p,w,b)
#define ISHELL_AppIsInGroup(p,cls,grp)          GET_PVTBL(p,IShell)->AppIsInGroup(p,cls,grp)

#define ISHELL_DetectType(ps,cpBuf,pdwSize,pszName,pcpszMIME)   GET_PVTBL(ps,IShell)->DetectType(ps,cpBuf,pdwSize,pszName,pcpszMIME)

#define ISHELL_GetAppAuthor(ps,psz,nSize)       GET_PVTBL(ps,IShell)->LoadResString(ps,NULL,IDS_MIF_COMPANY,psz,nSize)
#define ISHELL_GetAppCopyright(ps,psz,nSize)    GET_PVTBL(ps,IShell)->LoadResString(ps,NULL,IDS_MIF_COPYRIGHT,psz,nSize)
#define ISHELL_GetAppVersion(ps,psz,nSize)      GET_PVTBL(ps,IShell)->LoadResString(ps,NULL,IDS_MIF_VERSION,psz,nSize)

#define ISHELL_GetClassItemID(ps,cls)           GET_PVTBL(ps,IShell)->GetClassItemID(ps,cls)

#define ISHELL_GetProperty(ps,cls,id,dest,pws)   GET_PVTBL(ps,IShell)->GetProperty(ps,cls,id,dest,pws)
#define ISHELL_SetProperty(ps,cls,id,src,ws)    GET_PVTBL(ps,IShell)->SetProperty(ps,cls,id,src,ws)

#define ISHELL_RegisterEvent(ps,psz,pn)         GET_PVTBL(ps,IShell)->RegisterEvent(ps,psz,pn)

#define ISHELL_Reset(ps,t)                      GET_PVTBL(ps,IShell)->Reset(ps,t)

static __inline boolean ISHELL_SendURLEx( IShell *pShell, const char *pszURL, boolean bSync )
{
   boolean bRetVal = FALSE;
   AEECLSID AppCls = 0;
   char szURLScheme[64];
   char c; unsigned i;

   if (!pszURL || !*pszURL) {
      return bRetVal;
   }

   for (i = 0; (':' != (c = *pszURL)); i++, pszURL++) {

      if (('\0' == c) || (i >= sizeof(szURLScheme)-1)) {
         return bRetVal;
      }

      szURLScheme[i] = c;
   }

   szURLScheme[i] = '\0';
   pszURL -= i;

   AppCls = ISHELL_GetHandler(pShell,AEECLSID_APP,szURLScheme);

   if (AppCls) {
      if (bSync) {
         bRetVal = ISHELL_SendEvent(pShell,AppCls,EVT_APP_POST_URL,0,(uint32)pszURL);
      }
      else {
         bRetVal = ISHELL_PostEvent(pShell,AppCls,EVT_APP_POST_URL,0,(uint32)pszURL);
      }
   }

   return bRetVal;
}

static __inline boolean ISHELL_PostURL( IShell *pShell, const char *pszURL )
{
   return ISHELL_SendURLEx(pShell, pszURL, FALSE);
}

static __inline boolean ISHELL_SendURL( IShell *pShell, const char *pszURL )
{
   return ISHELL_SendURLEx(pShell, pszURL, TRUE);
}

//
// Pre-1.0 compatibility macros.  No longer used...
//

#define ISHELL_GetTimeMS(p)                     GETTIMEMS()
#define ISHELL_GetUpTimeMS(p)                   GETUPTIMEMS()
#define ISHELL_GetSeconds(p)                    GETTIMESECONDS()
#define ISHELL_GetJulianDate(p,s,pd)            GETJULIANDATE(s,pd)
#define ISHELL_GetRand(p,d,n)                   GETRAND(d,n)

//
// Static Text Control
//
#ifdef CUST_EDITION	
#define ST_DISPLAY_CONT_DETAIL         0x00001000
#endif /*CUST_EDITION*/
#define ST_GRAPHIC_BLUE    0x00002000
#define ST_CENTERTEXT      0x00010000  // Center Text
#define ST_CENTERTITLE     0x00020000  // Center Title
#define ST_NOSCROLL        0x00040000  // Do not scroll text
#define ST_TEXTALLOC       0x00080000  // Text allocated - Dialog now owns it
#define ST_TITLEALLOC      0x00100000  // Title allocated - Dialog now owns it
#define ST_MIDDLETEXT      0x00200000  // Text is drawn in the middle of the screen
#define ST_UNDERLINE       0x00400000  // Underline the title
#define ST_ICONTEXT        0x00800000  // pText is IImage *
#define ST_ASCII           0x01000000  // Text is single-byte
#define ST_ENABLETAB       0x02000000  // Generate EVT_CTL_TAB when at top or bottom
#define ST_ENABLE_HLGHT    0x04000000  // Highlights the static if it has focus
#define ST_WRAP_NOBREAK    0x08000000  // Wraps long lines without regard to Space characters
#define ST_SCROLL_RATE     0x30000000  // use bits to describe scroll rate

//valid scroll rate values
#define ST_SCROLL_ONE_SEC     0x00000000  // One second scroll rate (default)
#define ST_SCROLL_QUARTER_SEC 0x10000000  // Quarter second scroll rate
#define ST_SCROLL_HALF_SEC    0x20000000  // Half second scroll rate
#define ST_SCROLL_TWO_SEC     0x30000000  // Two second scroll rate
#ifdef CUST_EDITION	
#define ST_DISPLATSMS         0x20000000  // Current static text control used for displaying sms
#define ST_TRANSPARENTBACK    0x40000000  // set static text background transparent, liubin 2008.10.23
#define ST_TRANSPARENT        0x80000000  // Transparent Text
#define ST_GRAPHIC_BG        0x00000010  // use graphic background
#define ST_SPECIAL_BG        0x00000020  // use special background	Add By zzg 2011_12_31
#endif /*CUST_EDITION*/
QINTERFACE(IStatic)
{
   DECLARE_IBASE(IStatic)
   DECLARE_ICONTROL(IStatic)

   boolean        (*SetText)(IStatic * po, AECHAR * pTitle, AECHAR * pText, AEEFont fntTitle, AEEFont fntText);

   // New 1.10 interfaces...

   void           (*SetFont)(IStatic * po, AEEFont fntText, AEEFont fntTitle);
   boolean        (*SetTextEx)(IStatic * po, byte * pbText, IAStream * pStream, boolean bAppend);
   int            (*GoToLine)(IStatic * po, int nLine);
   void           (*SizeToFit)(IStatic * po, AEERect * prc);
   boolean        (*IsScrollable)(IStatic * po);
#ifdef CUST_EDITION	   
   void           (*SetBackGround)(IStatic * po, char *pstrImgResFile, uint16 nImgResID);
   void           (*SetFontColor)(IStatic * po, RGBVAL nFontColor);
#endif /*CUST_EDITION*/ 
};

#define ISTATIC_AddRef(p)                            GET_PVTBL(p,IStatic)->AddRef(p)
#define ISTATIC_Release(p)                           GET_PVTBL(p,IStatic)->Release(p)
#define ISTATIC_HandleEvent(p,ec,wp,dw)              GET_PVTBL(p,IStatic)->HandleEvent(p,ec,wp,dw)
#define ISTATIC_Redraw(p)                            GET_PVTBL(p,IStatic)->Redraw(p)
#define ISTATIC_SetActive(p,b)                       GET_PVTBL(p,IStatic)->SetActive(p,b)
#define ISTATIC_IsActive(p)                          GET_PVTBL(p,IStatic)->IsActive(p)
#define ISTATIC_SetRect(p,prc)                       GET_PVTBL(p,IStatic)->SetRect(p,prc)
#define ISTATIC_GetRect(p,prc)                       GET_PVTBL(p,IStatic)->GetRect(p,prc)
#define ISTATIC_SetProperties(p,props)               GET_PVTBL(p,IStatic)->SetProperties(p,props)
#define ISTATIC_GetProperties(p)                     GET_PVTBL(p,IStatic)->GetProperties(p)
#define ISTATIC_Reset(p)                             GET_PVTBL(p,IStatic)->Reset(p)
#define ISTATIC_SetText(p,ptitle, ptext,ft,ftt)      GET_PVTBL(p,IStatic)->SetText(p,ptitle,ptext,ft,ftt)

// New 1.10 interfaces

#define ISTATIC_SetFont(p,f1,f2)                     GET_PVTBL(p,IStatic)->SetFont(p,f1,f2)
#define ISTATIC_SetTextEx(p,pb,ps,b)                 GET_PVTBL(p,IStatic)->SetTextEx(p,pb,ps,b)
#define ISTATIC_GoToLine(p,n)                        GET_PVTBL(p,IStatic)->GoToLine(p,n)
#define ISTATIC_SizeToFit(p,prc)                     GET_PVTBL(p,IStatic)->SizeToFit(p,prc)

// New 2.0 interfaces
#define ISTATIC_IsScrollable(p)                      GET_PVTBL(p,IStatic)->IsScrollable(p)

#ifdef CUST_EDITION	 
#define ISTATIC_SetBackGround(p,s,n)          GET_PVTBL(p,IStatic)->SetBackGround(p, s, n)
#define ISTATIC_SetFontColor(p,n)                GET_PVTBL(p,IStatic)->SetFontColor(p, n)
#endif /*CUST_EDITION*/ 

#endif    // AEESHELL_H

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
NMASK_SHELL_INIT

Description:
   This is a notification mask that can be used by apps to register to
be notified when the system has been initialized.

   To register for this notification, applications must use the
NMASK_SHELL_INIT mask. As soon as BREW has completely started up,
applications receive the EVT_NOTIFY event. The dwParam of this event
is of type AEENotify. The pData member inside this AEENotify structure
will be set to NULL.

Definition:
   #define NMASK_SHELL_INIT     0x0001

Members:
   None

Comments:
   None

See Also:
   None

=============================================================================
NMASK_SHELL_START_STATUS

   The applications can use this notification mask to receive notification 
whenever an application attempts to become top visible. The notification is
sent for success as well as failure to become top visible.

The following scenarios can cause this notification to be sent:

1. Calling ISHELL_StartApplet()

2. Using IAppHistory_Move() to cause an app to become top-visible

3. A suspended app is resumed to become top-visible

   The data sent to the applications when they register for this notification is of
type NotifyStartStatus.

Definition:
   #define NMASK_SHELL_START_STATUS     0x0002

Members:
   None

Comments:
   None

See Also:
   NotifyStartStatus

=============================================================================
NMASK_SHELL_APP_EXPIRED

Description:
   This is a notification mask that can be used by apps to register to
be notified when they expire.

   To register for this notification, applications must use the
NMASK_SHELL_APP_EXPIRED mask. When the application expires, it will receive
the EVT_NOTIFY event. The dwParam of this event is of type AEENotify.
The pData member inside this AEENotify structure  will be set to NULL.

Definition:
   #define NMASK_SHELL_APP_EXPIRED     0x0004

Members:
   None

Comments:
   This mask is not implemented.

See Also:
   None

=============================================================================
NMASK_SHELL_APP_LICENSE_CHANGED

Description:
   This is a notification mask that can be used by apps to register to
be notified when the application's license is changed.

   To register for this notification, applications must use the
NMASK_SHELL_APP_LICENSE_CHANGED mask.  When the application's license is
upated, the application will receive the EVT_NOTIFY event. The dwParam of
this event is of type AEENotify. The pData member inside this AEENotify
structure will point to an AEELicenseInfo structure containing the app's
current license information.

Definition:
   #define NMASK_SHELL_APP_LICENSE_CHANGED     0x0008

Members:
   None

Comments:
   None

See Also:
   None

=============================================================================
NMASK_SHELL_MOD_LIST_CHANGED

Description:
   This is a notification mask that can be used by apps to register to
be notified when the list of installed applications changes.  Applications
that wish to be notified when BREW applets are installed or deleted should
register for this notification.

   To register for this notification, applications must use the
NMASK_SHELL_MOD_LIST_CHANGED mask.  When the list of installed applications
changes, applications receive the EVT_NOTIFY event. The dwParam of this
event is of type AEENotify.  The pData member inside this AEENotify structure
will be set to NULL.

Definition:
   #define NMASK_SHELL_MOD_LIST_CHANGED     0x0010

Members:
   None

Comments:
   None

See Also:
   None

=============================================================================
NMASK_SHELL_SYS_ERROR

Description:
   This is a notification mask that can be used by apps to register to
be notified when there is a system error.
When there is a system error, the registered applications will receive the EVT_NOTIFY event.
The dwParam of this event is of type AEENotify.  The pData member inside this AEENotify
structure is of type AEESysError.
Please see documentation for AEESysError for supported system errors and their data structures.

Definition:
   #define NMASK_SHELL_SYS_ERROR             (0x0020)

Members:
   None

Comments:
   None

See Also:
   AEESysError

=============================================================================
NMASK_SHELL_KEY

Description:
   This is a notification mask that can be used by apps to register to
be notified when an individual key is pressed, even when the app is in the background.
When the specified key is pressed or released, the registered applications will receive
the EVT_NOTIFY event. The AVK code of the desired key is specified in the upper 16 bits
of dwMask when calling ISHELL_RegisterNotify.  Use NOTIFYIER_VAL_ANY for the
AVK code to receive all key events.
The dwParam of this event is of type AEENotify.  The pData member inside this AEENotify
structure is of type NotifyKeyEvent.


Definition:
   #define NMASK_SHELL_KEY             (0x0040)

Members:
   None

Comments:
   Apps may register additional times for multiple keys.

See Also:
   NotifyKeyEvent

=============================================================================
MPROP_NETUSAGE

Description:
   This is a property ID that can be used in ISHELL_GetProperty() to get the network
   statistics for an app. This returns the structure of type: AEENetUsage


Definition:
   #define MPROP_NETUSAGE     (2)

Members:
   None

Comments:
   None

See Also:
   ISHELL_GetProperty()

=============================================================================
MPROP_USAGE

Description:
   This is a property ID that can be used in ISHELL_GetProperty() to get the usage
   statistics for an app. This returns the structure of type: AEEAppUsage


Definition:
   #define MPROP_USAGE        (3)

Members:
   None

Comments:
   None

See Also:
   ISHELL_GetProperty()

=============================================================================
MPROP_USESSTRING

Description:
   This is a property ID that can be used in ISHELL_GetProperty() to get the uses
   string for an app. Uses string returned is an AECHAR string of
   <Singular Use>'\0'<Plural Use>\0' format e.g. ringer'\0'ringers'\0'.

Definition:
   #define MPROP_USAGE        (4)

Members:
   None

Comments:
   None

See Also:
   ISHELL_GetProperty()

=============================================================================
MPROP_STATUS

Description:
   This is a property ID that can be used in ISHELL_GetProperty() to get the status string
   of an app. Each app can set this property to its desired string. The AppMgr queries
   this string at run time and displays this string next along with the app name so as to present
   app-specific information at run time. For ex: an email client can store the number
   of new messages as a status string so that when the AppMgr presents the email client app,
   the updated status string can be shown to the user. So, without launching the email app,
   the user will be made aware of updated status info ex: number of new emails available.


Definition:
   #define MPROP_STATUS       (5)

Members:
   None

Comments:
   None

See Also:
   ISHELL_SetProperty()
   ISHELL_GetProperty()

=============================================================================
AEENetUsage

Description:
   This structure contains information about the network usage of a given app.
   When the function ISHELL_GetProperty() is invoked with the property ID: MPROP_NETUSAGE,
   this structure is returned.


Definition:
typedef struct
{
   uint32   dwTCPSent;
   uint32   dwTCPRecv;
   uint32   dwUDPSent;
   uint32   dwUDPRecv;
} AEENetUsage;

Members:
   dwTCPSent: Total Number of Bytes sent over TCP
   dwTCPRecv: Total Number of Bytes received over TCP
   dwUDPSent: Total Number of Bytes sent over UDP
   dwUDPRecv: Total Number of Bytes received over UDP

Comments:
   None

See Also:
   ISHELL_SetProperty()
   ISHELL_GetProperty()
   MPROP_NETUSAGE

=============================================================================
AEEAppUsage

Description:
   This structure contains information about the app-usage statistics of a given app.
   When the function ISHELL_GetProperty() is invoked with the property ID: MPROP_USAGE,
   this structure is returned.


Definition:
typedef struct
{
   uint32   dwRunCount;
   uint32   dwMinsOfUse;
   uint32   dwLastUsedTime;
} AEEAppUsage;

Members:
   dwRunCount: Number of times this app has been run
   dwMinsOfUse: Total minutes of use of this app
   dwLastUsedTime: The time of last use for this app

Comments:
   None

See Also:
   ISHELL_SetProperty()
   ISHELL_GetProperty()
   MPROP_USAGE

=============================================================================
AEE Applet Flags

Description:
Applet specific flags.

Definition:

AFLAG_HIDDEN:     Applet is hidden
AFLAG_CFG:        Applet has a CFG menu
AFLAG_SYSTEM_CFG: Applet has a hidden CFG menu
AFLAG_TOOL:       Applet is a tool
AFLAG_GAME:       Applet is a game
AFLAG_PIM:        Applet is a PIM
AFLAG_WEB:        Applet is a Web Applet
AFLAG_STATIC:     System use only - Static applet
AFLAG_SCREENSAVER Applet is a Screen Saver.
AFLAG_DYNAMIC     System use only - Dynamic applet
AFLAG_BADSHUTDOWN System use only - Applet failed to shutdown properly

Members:
None

Comments:
None

See Also:
None

=============================================================================
NotifyKeyEvent

Description:
This data is sent to the apps when they register for NMASK_SHELL_KEY
notifications.

Definition:
typedef struct _NotifyKeyEvent {
   AEEEvent eCode;
   uint16   wParam;
   uint32   dwParam;
   AEECLSID clsHandled
} NotifyKeyEvent;

Members:

eCode:  The key event (EVT_KEY, EVT_KEY_PRESS, EVT_KEY_RELEASE)
wParam: The AVK keycode
dwParam: Bitflags for modifier keys
clsHandled: If this value is non-zero, it denotes the classId of the app that has already handled
this key event prior to the notification being sent. The following rules apply for this member:
If clsHandled is set to 0, it denotes that they key event was not handled by any application
If clsHandled is set to AEECLSID_SHELL, it denotes that the key event was handled
by BREW. The only case this is possible is when the CloseAllApps Key (typically, the END key) 
is pressed and this key was consumed by BREW to close all the active apps.
If clsHandled is set to a non-zero value (and not AEECLSID_SHELL), it denotes the classID of the
app that handled this event. As denoted in the documentation for Key Filtering and EVT_KEY_HOOK,
the key events are first sent to any applications that are registered for key hook events. if 
any of these apps handle the key event, the clsHandled is set to that of the handling application.
If none of the apps that have registered for key hook events handle the key event, then the key 
event is sent to the current top-visible app. If that top-visible app handles the key event, then
clsHandled is set to the classId of the top-visible app.  
Please note that the clsHandled member has been to this structure starting with BREW 
version 3.1.4. It may also be available on specific FeaturePackage releases of 3.1.2 and 3.1.3. 

Comments:
None

See Also:
NMASK_SHELL_KEY

=============================================================================
NotifyStartStatus

Description:
This data is sent to the apps when they register for NMASK_SHELL_START_STATUS notification.

Definition:
typedef struct _NotifyStartStatus
{
   AEECLSID    cls;
   int         nErr;
} NotifyStartStatus;

Members:

cls:  The ClassID of the app being started
nErr: Error code in starting the app


Comments:
None

See Also:
NMASK_SHELL_START_STATUS

=======================================================================
PL_

PL_XXXXX is the format for AEE Privilege Levels
=======================================================================
  AEE Privilege Levels

Description:
    These flags define the privilege levels that an application can have. An
application can have 0 (zero) or more privilege levels, which are stored internally
in a bit vector. These flags can be used to test and set the value of each privilege
bit in this vector.

Definition:
    None

Members:
PL_FILE           :The application has create and write access to files and databases.
PL_NETWORK        :The application has access to the functions in the INetMgr Interface
                   and ISocket Interface.
PL_TAPI           :The application has access to telephony functionality.
PL_DOWNLOAD       :The application has access to the IDownload interface, which contains
                   functions for accessing BREW application download servers (this privilege
                   level is available only to carriers and device manufacturers).
PL_SHARED_WRITE   :The application has access to the shared application directory, which
                    allows applications to share files.
PL_POS_LOCATION   :The application has access to position-location functionality.
PL_SYSTEM         :The application has all of the above privilege levels and additional
                    functionality (this privilege level is only available to carriers and
                    device manufacturers).
PL_WEB            :If set, allows an application to instantiate IWeb. PL_NETWORK is a
                    super-permission of PL_WEB.
                    NOTE: If you have PL_WEB, you can get IWeb. If you have PL_NETWORK you
                    can get INetMgr/ISocket and IWeb.
PL_RINGER_WRITE   :If set, allows application to create IRINGERMGR_Create() ringer tones.
                    One doesn't need the privilege level set to play ringer tones.
PL_SECTORINFO     :If set, allows application to access IPOSDET_GetSectorInfo().
PL_ADDRBOOK       :If set, allows application to access to Address Book


Comments:
    None

See Also:
    ISHELL_CheckPrivLevel()
    ISHELL_AppIsInGroup()

=======================================================================
ResType

Description:
This ENUM specifies the resource type to identify resources in a Resource file.

Definition:
typedef enum {
   RESTYPE_STRING=1,
   RESTYPE_IMAGE=6,
   RESTYPE_DIALOG=0x2000,
   RESTYPE_CONTROL=0x2001,
   RESTYPE_LISTITEM=0x2002,
   RESTYPE_BINARY=0x5000
} ResType;

Members:

RESTYPE_STRING : String resource, made of UNICODE (UCS2 encoding) or ISOLATIN strings
RESTYPE_IMAGE : A binary resource with an AEEResBlob header, typically used for images
RESTYPE_DIALOG : Dialog resource, made up of one or more dialog controls
RESTYPE_CONTROL : Control resource
RESTYPE_LISTITEM : List Item Type resource
RESTYPE_BINARY : Binary data resource

Comments:
None

See Also:
None

=======================================================================
AEEResBlob

Description:
    This binary data resource structure is returned from
    ISHELL_LoadResDataEx() for RESTYPE_IMAGE resources.

Definition:

  typedef struct AEEResBlob
  {
     byte  bDataOffset;
     byte  bZero;
     char  szMimeType[1];
  } AEEResBlob;

Members:

    bDataOffset: how far into the Blob the real data starts
    bZero: always zero, is padding in 1.x BREW
    szMimeType[1]: null-terminated string (not just [1]) mimetype,
                   can be up to 249 characters in length

Comments:
    RESTYPE_IMAGE resources use this structure by convention.
    Use the macro RESBLOB_DATA() to get a pointer to the actual data, which
    starts after the null-terminated mime type.

See Also:
    ISHELL_LoadResDataEx()

=======================================================================
DListItem

Description:
This structure defines a dialog list item.

Definition:
typedef struct _DListItem
{
   uint16 wID;
   uint16 wTextID; // Text
   uint16 wIconID; // Icon ID
   uint16 pad;
   uint32 dwData; // 32-bit data
} DListItem;

Members:

wID : ID of the dialog list item.
wTextID : Resource ID of the text string.
wIconID : Resource ID of the Icon.
pad : Reserved
dwData : Data that is sent with the event when the item is selected.


Comments:
None

See Also:
None

=======================================================================
DialogItemHead

Description:
This structure defines the dialog item header.

Definition:
typedef struct
{
   AEECLSID cls;
   uint16 wID;
   uint16 nItems;
   uint32 dwProps;
   uint16 wTextID;
   uint16 wTitleID;
   AEERect rc;
} DialogItemHead;

Members:

cls : ClassID of the control.
wID : Control ID (Resource ID of the control).
nItems : Number of dialog list items.
dwProps : Property of the dialog control (CP_BORDER, CP_STATIC, etc)
wTextID : Resource ID of text string.
wTitleID : Resource ID of title string.
rc : Rectangle relative to the dialog ((-1, -1, -1, -1) is default).

Comments:
None

See Also:
AEERect
AEECLSID List

=======================================================================
DialogItem

Description:
This structure contains a dialog item.

Definition:
typedef struct
{
   DialogItemHead h;
   DListItem items[1];
} DialogItem;

Members:
h : Dialog Item header
items : Array of dialog list items

Comments:
None

See Also:
DListItem

=======================================================================
DialogInfoHead

Description:
This structure specifies the dialog information header.

Definition:
typedef struct
{
   uint16 wID;
   uint16 nControls;
   AEERect rc;
   uint32 dwProps;
   uint16 wTitle;
   uint16 wFocusID;
} DialogInfoHead;

Members:

wID: Dialog ID (Resource ID of the Dialog)
nControls : Number of controls
rc : Dialog rectangle ((-1, -1, -1, -1) is default)
dwProps : Property of the dialog (Dialogs currently supports only the CP_BORDER,
          CP_3D_BORDER control parameters.)
wTitle : Dialog Title. This field is not supported currently
wFocusID : ID of the control that is active when the dialog is started

Comments:
None

See Also:
AEE Standard Control Properties

=======================================================================
DialogInfo

Description:
This ENUM specifies the resource type to identify resources in a resource file.

Definition:
typedef struct
{
   DialogInfoHead h;
   DialogItem controls[1];
} DialogInfo;

Members:

h : Dialog Information header
controls : Array of dialog items


Comments:
None

See Also:
None

=======================================================================
AEEItemType

Description:
This ENUM specifies the item type whose style is requested using the
ISHELL_GetItemStyle().

Definition:
typedef enum {
   AEE_IT_MENU,
   AEE_IT_SOFTKEY,
   AEE_IT_ICONVIEW
} AEEItemType;

Members:

AEE_IT_MENU : Menu Item type
AEE_IT_SOFTKEY : Soft key item type
AEE_IT_ICONVIEW : Icon View Item type

Comments:
None

See Also:
None

=======================================================================
AEEItemStyle

Description:
This structure specifies the item style.

Definition:
typedef struct
{
   AEEFrameType ft;
   uint16 xOffset;
   uint16 yOffset;
   AEERasterOp roImage;
} AEEItemStyle;

Members:

ft : Frame type of item
xOffset : X padding inside item (does not include frame size)
yOffset : Y padding inside item (does not include frame size)
roImage : Raster operation for drawing images inside the item

Comments:
None

See Also:
   AEEFrameType
   AEERasterOp

=======================================================================
AEEDeviceInfo

Description:
This structure contains mobile device information requested in ISHELL_GetDeviceInfo().

Definition:
typedef struct
{
   uint16            cxScreen;         // Physical screen size (pixels)
   uint16            cyScreen;         // Physical screen size (pixels)
   uint16            cxAltScreen;      // Physical screen size of 2nd display (Pager)
   uint16            cyAltScreen;      // Physical screen size of 2nd display (Pager)
   uint16            cxScrollBar;      // Width of standard scroll bars
   uint16            wEncoding;        // Character set encoding (AEE_ENC_UNICODE, ....)
   uint16            wMenuTextScroll;  // Milliseconds that should be used for auto-scroll
   uint16            nColorDepth;      // Color Depth (1 = mono, 2 = grey, etc.)
   EmptyEnum         unused2;
   uint32            wMenuImageDelay;  // Milliseconds image Draw Delay for SINGLE_FRAME Icon View 
                                       // with MP_ICON_DELAY_IMAGE property
   uint32            dwRAM;            // Total RAM installed  (RAM)
   flg               bAltDisplay:1;    // Device has an alternate display (Pager)
   flg               bFlip:1;          // Device sends the flip event (EVT_FLIP) when the clamshell is opened/closed
   flg               bVibrator:1;      // Vibrator installed
   flg               bExtSpeaker:1;    // External speaker installed
   flg               bVR:1;            // Voice recognition supported
   flg               bPosLoc:1;        // Position location supported
   flg               bMIDI:1;          // MIDI file formats supported
   flg               bCMX:1;           // CMX audio supported
   flg               bPen:1;           // Pen device supported
   uint32            dwPromptProps;    // Default prompt properties
   uint16            wKeyCloseApp;     // Key to close current app
   uint16            wKeyCloseAllApps; // Key to close all apps (AVK_END is default)
   uint32            dwLang;           // Used by Resource Loader - See AEE_LNG_XXX below

   //
   // NOTE: In order to use the following fields, you MUST fill-in the wStructSize
   // element of the structure before passing this to the GetDeviceInfo call.
   //

   uint16            wStructSize;      // Size of this structure.
   uint32            dwNetLinger;      // PPP linger timer in milliseconds
   uint32            dwSleepDefer;     // Active non-sleep in milliseconds
   uint16            wMaxPath;         // Max length of file path
   uint32            dwPlatformID;     // Platform ID
} AEEDeviceInfo;

Members:

cxScreen : Physical screen size (pixels)
cyScreen : Physical screen size (pixels)
cxAltScreen : Physical screen size of 2nd display
cyAltScreen : Physical screen size of 2nd display
cxScrollBar : Width of standard scroll bars
wEncoding : Character set encoding (AEE_ENC_UNICODE, ....)
nColorDepth : Color Depth (1 = mono, 2 = grey, etc.)
dwRAM : Total RAM installed (RAM)
bAltDisplay : Device has an alternate display (Pager)
bFlip : Device sends the flip event (EVT_FLIP) when the clamshell is opened/closed
bVibrator : Vibrator installed
bExtSpeaker : External speaker installed
bVR : Voice recognition supported
bPosLoc : Position location supported
bMIDI : MIDI file formats supported
bCMX: CMX audio supported
dwPromptProps : Default prompt properties
wKeyCloseApp : Key to close current app
wKeyCloseAllApps : Key to close all apps (AVK_END is default)
dwLang : ISO defined language ID
wStructSize:Size of the struct. Need to be filled for the following fields to work
dwNetLinger: PPP Linger Time in milliseconds
dwSleepDefer: Time in milliseconds prior to the handset attempting to go into sleep mode
wMaxPath: Maximum length of the file name (including path name) supported on the device
dwPlatformID: ID used to uniquely identify this device platform.

Comments:
None

See Also:
None
=======================================================================
AEE_DEVICEITEM_

AEE_DEVICEITEM_XXXX  is the format for an AEEDeviceItem.
=======================================================================
AEEDeviceItem

Description:
This specifies the ID of the item whose information is needed. This is used
in the function ISHELL_GetDeviceInfoEx()


Definition:
typedef int AEEDeviceItem

Members:

The following Items are supported:

~
   Name:
   AEE_DEVICEITEM_CHIP_ID
~
   Description:
   This returns a String identifying the ID of the chipset. For Qualcomm chipsets,
   these strings are of the form: MSM3100, MSM3300, MSM5100, etc.This information is returned
   as a AECHAR when ISHELL_GetDeviceInfoEx() is invoked with this ID.
   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_DEVICEITEM_CHIP_ID
      pBuff: Buffer capable of holding a AECHAR string
*

   Name:
   AEE_DEVICEITEM_MOBILE_ID
~
   Description:
   For CDMA networks represents 15-byte mobile number which consists of the following components:
      - mcc: mobile country code (3 digits)
      - mnc: mobile network code (2 digits)
      - min2: area code (3 digits)s
      - min1: phone number (7 digits) 
   For UMTS networks represents International Mobile Subscriber Identity (IMSI).
   Applications that need the MDN information should use ISHELL_GetDeviceInfoEx with AEE_DEVICEITEM_MDN.
   This is the same information that is returned in the szMobileID member of the TAPIStatus structure when the function
   ITAPI_GetStatus() is invoked. The benefit of putting it here is to enable applications to obtain the MobiledID
   without having to create an ITAPI instance (and hence without needing the PL_TAPI privilege level).
   No privilege level is required to obtain the MobileID using this function.
   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_DEVICEITEM_MOBILE_ID
      pBuff: Buffer capable of holding a character (ASCII) string denoting the MobileID.
*

Name:
IDs for vocoder support information
~

   AEE_DEVICEITEM_AMR_VOCODER_SUPPORT: Is AMR (Adaptive Multi-Rate)
   Vocoder Supported?  ~
   AEE_DEVICEITEM_EVRC_VOCODER_SUPPORT: Is EVRC (Enhanced Variable Rate Codec)
   Vocoder Supported ?   ~
   AEE_DEVICEITEM_IS96_VOCODER_SUPPORT: Is QCELP-IS96(8K)(Qualcomm Code
   Excited Linear Predictive Coding) Vocoder Supported ? ~
   AEE_DEVICEITEM_IS96A_VOCODER_SUPPORT: Is QCELP-IS96A(8K)(Qualcomm Code
   Excited Linear Predictive Coding) Vocoder Supported ?  ~
   AEE_DEVICEITEM_IS733_VOCODER_SUPPORT: Is QCELP-IS733(13K)(Qualcomm Code
   Excited Linear Predictive Coding) Vocoder Supported ?  ~
   AEE_DEVICEITEM_SMV_VOCODER_SUPPORT: Is Selectable Mode Vocoder Supported ? ~
*


   Name:
   IDs for system colors associated with a particular display
~
         AEE_DEVICEITEM_SYS_COLORS_DISP1 ~
         AEE_DEVICEITEM_SYS_COLORS_DISP2  ~
         AEE_DEVICEITEM_SYS_COLORS_DISP3 ~
         AEE_DEVICEITEM_SYS_COLORS_DISP4 ~


   Description:
   This returns a list of system colors associated with a particular display.  The pBuff
   pointer is an array of RGBVALs.  The array is indexed with color items from AEEClrItem.

*
   Name:
   IDs for a particular display
~
         AEE_DEVICEITEM_DISPINFO1   ~
         AEE_DEVICEITEM_DISPINFO2  ~
         AEE_DEVICEITEM_DISPINFO3  ~
         AEE_DEVICEITEM_DISPINFO4

   NOTE:
   These device items are deprecated in BREW Client 3,1 and may be removed in future versions.

   Description:
   This returns information about a particular display.  The pBuff pointer is an AEEBitmapInfo
   structure.  NOTE: When specifying display settings in an application's MIF file, the settings
   do not take effect until the application creates an instance of the corresponding display.
   Therefore, calling ISHELL_GetDeviceInfoEx() with one of these device items before creating
   an instance of the corresponding display will return the default display dimensions.
*

   Name:
   AEE_DEVICEITEM_CARRIER_ID
~
   Description:
   This returns an uint32 value identifying the CarrierID of the handset.
   No privilege level is required to obtain the CarrierID using this function.
   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_DEVICEITEM_CARRIER_ID
      pBuff: pointer to a uint32, to receive carrier ID
*

   Name:
   AEE_DEVICEITEM_SIDHASH
~
   Description:
        This returns a hash of the billing identity of the handset. It is useful to determine if
   the billing identity has changed, if for example the RUIM card was swappped. Note the
   exact identity is not available, only a hash of the identity string.

*

   Name:
   AEE_DEVICEITEM_BKEY
~
   Description:
  This returns the B-Key for the device.
  AEECLSID_BREWCREDENTIALS is required privilege level.
===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      nItem: Specifies AEE_DEVICEITEM_BKEY
      pBuff: Buffer capable of holding BKey - a byte array
*

   Name:
   AEE_DEVICEITEM_SID
~
   Description:
   This returns the SID, as needed by the ADS.
   AEECLSID_BREWCREDENTIALS is required privilege level.
===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      nItem: Specifies AEE_DEVICEITEM_SID
      pBuff: Buffer capable of holding the SID - a byte array
*

   Name:
   AEE_DEVICEITEM_DLFLAGS
~
  Description:
  This returns the download config flags.
  AEECLSID_BREWCREDENTIALS is required privilege level.
===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>
    Parameters:
      nItem: Specifies AEE_DEVICEITEM_DLFLAGS
      pBuff: Address of word to hold the flags
*

  Name:
  AEE_DEVICEITEM_IMEI
~
   Description:
  This returns the IMEI for the device in ASCII format.
  AEECLSID_GSMCREDENTIALS or AEECLSID_HWIDS is the required privilege level.

===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      nItem: Specifies AEE_DEVICEITEM_IMEI
      pBuff: Buffer capable of holding IMEI - a null-terminated 16 char string

*

  Name:
  AEE_DEVICEITEM_HWID
~
   Description:
  This returns the primary HW ID for the device.  Depending on the device, the
  primary ID may be a MEID, IMEI or ESN.  In any case, this is the ID required
  by BREW to identify the device.  The primary ID on a device does
  not change even if, for example, the network type changes or an identity
  card is swapped.
  
  AEECLSID_HWIDS is the required privilege level.

===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      nItem: Specifies AEE_DEVICEITEM_HWID
      pBuff: Buffer capable of holding the HWID - see AEEHWIDType structure

*

  Name:
  AEE_DEVICEITEM_HWIDLIST
~
   Description:
  This returns a list of all of the HW IDs defined for the device.  The first item
  on the list is always the primary hardware ID (the same id returned for
  AEE_DEVICEITEM_HWID). 
  
  AEECLSID_HWIDS is the required privilege level.

===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      nItem: Specifies AEE_DEVICEITEM_HWIDLIST
      pBuff: Buffer capable of holding list of HWIDs - see AEEHWIDType.
             The last element in the list has a uLength == 0.  If the 
             buffer is not large enough for the entire list, it will
             return those items which do fit.  Regardless, the order
             items appear on the list does not change.

*

  Name:
  AEE_DEVICEITEM_MANUFACTURER_NAME
~
   Description:
  This returns the name of the Manufacturer of the device (AECHAR string).  
  
===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      po: Pointer to the IShell object
      pnSize: Place holder for number of bytes required (if pBuff is NULL or *pnSize < required size)
              or number of bytes written in the provided buffer
      nItem: Specifies AEE_DEVICEITEM_MANUFACTURER_NAME
      pBuff: Buffer capable of holding a AECHAR string

*

Name:
  AEE_DEVICEITEM_MODEL_NAME
~
   Description:
  This returns the model name of the device (AECHAR string).  
  
===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      po: Pointer to the IShell object
      pnSize: Place holder for number of bytes required (if pBuff is NULL or *pnSize < required size)
              or number of bytes written in the provided buffer
      nItem: Specifies AEE_DEVICEITEM_MODEL_NAME
      pBuff: Buffer capable of holding a AECHAR string

*

Name:
  AEE_DEVICEITEM_FRIENDLY_NAME 
~
   Description:
  This returns the friendly device name (AECHAR string).  
  
===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      po: Pointer to the IShell object
      pnSize: Place holder for number of bytes required (if pBuff is NULL or *pnSize < required size)
              or number of bytes written in the provided buffer
      nItem: Specifies AEE_DEVICEITEM_FRIENDLY_NAME
      pBuff: Buffer capable of holding a AECHAR string

*
  Name:
  AEE_DEVICEITEM_MEIDS
~
   Description:
  This returns the MEID for the device in ASCII format.
  AEECLSID_CDMACREDENTIALS or AEECLSID_HWIDS is the required privilege level.

===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      nItem: Specifies AEE_DEVICEITEM_MEIDS
      pBuff: Buffer capable of holding MEIDS - a null-terminated 15 char string

*

   Name:
   AEE_DEVICEITEM_USER

   Description:
   Starting point for OEM defined items

*

   Name:
   AEE_DEVICESTATE_KEYS_PRESSED
~
   Description:
   This returns values of keys in pressed state.

   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_DEVICESTATE_KEYS_PRESSED
      pBuff: Place holder for keys in pressed state. Each key is identified by its key code
             and at the most key code of 16 keys is returned. So, buffer specified must be
             of 64 bytes (16 * sizeof(uint32)) at least.
*

   Name:
   AEE_DEVICESTATE_FLIP_OPEN
~
   Description:
   This returns TRUE if a device flip is in an open state, and FALSE otherwise.
   This returns EUNSUPPORTED for slider device.

   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_DEVICESTATE_FLIP_OPEN
      pBuff: Place holder for boolean value for flip being in open state. Buffer must be of
             sizeof(boolean) size at least.
*

   Name:
   AEE_DEVICESTATE_HEADPHONE_ON
~
   Description:
   This returns TRUE if device head phone is on and FALSE otherwise.

   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_DEVICESTATE_HEADPHONE_ON
      pBuff: Place holder for boolean value for device head phone being in on state. Buffer
             must be of sizeof(boolean) size at least.
*

   Name:
   AEE_DEVICESTATE_KEYGUARD_ON
~
   Description:
   This returns TRUE if device keyguard is on and FALSE otherwise.

   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_DEVICESTATE_KEYGUARD_ON
      pBuff: Place holder for boolean value for device keyguard being in on state. Buffer
             must be of sizeof(boolean) size at least.
*

   Name:
   AEE_DEVICESTATE_SPEAKER_ON
~
   Description:
   This returns TRUE if device speaker is on and FALSE otherwise.

   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>
   Parameters:
      nItem: Specifies AEE_DEVICESTATE_SPEAKER_ON
      pBuff: Place holder for boolean value for device speaker being in on state. Buffer
             must be of sizeof(boolean) size at least.
*

   Name:
   AEE_DEVICESTATE_VIBRATOR_ON
~
   Description:
   This returns TRUE if device vibrator is on and FALSE otherwise.

   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>
   Parameters:
      nItem: Specifies AEE_DEVICESTATE_VIBRATOR_ON
      pBuff: Place holder for boolean value for device vibrator being in on state. Buffer
             must be of sizeof(boolean) size at least.
*

   Name:
   AEE_DEVICESTATE_SCR_ORIENTATION
~
   Description:
   This returns a value of the type AEEScrOrientation for the main display. The returned
   value is a recommendation, not the actual screen orientation. Based on this information,
   each app has to call IDISPLAY_SetPref() to swap the "w" and "h" values.

   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>
   Parameters:
      nItem: Specifies AEE_DEVICESTATE_SCR_ORIENTATION
      pBuff: Place holder for AEEScrOrientaton value for the main display. Buffer
             must be of sizeof(AEEScrOrientation) size at least.
*

   Name:
   AEE_DEVICEITEM_MANNER_MODE
~
   Description:
   This returns the device manner mode setting.

   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_DEVICEITEM_MANNER_MODE
      pBuff: Place holder for AEE_MANNER_MODE_XXX of uint32 type. Buffer
             must be of sizeof(uint32) size at least.
*

   Name:
   AEE_DEVICEITEM_MDN
~
   Description:
   This returns the Mobile Directory Number.
   AEECLSID_MDN is required privilege level.

   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_DEVICEITEM_MDN
      pBuff: Place holder for AEE_DEVICEITEM_MDN of char string type.
*

   Name:
   AEE_DEVICEITEM_FIRMWARE_VERSION
~
   Description:
   This returns the Firmware version of the device. This is a zero-terminated OEM-specific string to identify the
   firmware running on the device.

   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_DEVICEITEM_FIRMWARE_VERSION
      pBuff: Place holder for AEE_DEVICEITEM_FIRMWARE_VERSION of char string type.

      pnSize: Place holder for number of bytes required (if pBuff is NULL or *pnSize < required size)
              or number of bytes written in the provided buffer.

   Return Value:
      EUNSUPPORTED : Device doesn't support specified item
      EBADPARM : *pnSize == 0
      SUCCESS : Otherwise
*

   Name:
   AEE_DEVICEITEM_ADS
~
   Description:
   This returns the Application Download Server (ADS) hostname for the device.
  No privilege required to access this information.

===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      nItem: Specifies AEE_DEVICEITEM_ADS
      pBuff: Buffer capable of holding ADS hostname - a variable length null-terminated ASCII string
      pnSize: Place holder for number of bytes required (if pBuff is NULL or *pnSize < required size)
              or number of bytes written in the provided buffer.
*

   Name:
   AEE_DEVICEITEM_RUIMID
~
   Description:
  This returns the RUIM ID for the device.
  No privilege required to access this information.

===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      nItem: Specifies AEE_DEVICEITEM_RUIMID
      pBuff: Buffer capable of holding RUIM ID - variable byte length data.
      pnSize: Place holder for number of bytes required (if pBuff is NULL or *pnSize < required size)
              or number of bytes written in the provided buffer.
*

   Name:
   AEE_DEVICEITEM_SSDA
~
   Description:
  This returns SSD_A derived from the A-Key - or all zeros if the AKey is not provisioned.
  AEECLSID_CDMACREDENTIALS privilege is required to access this information.

===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      nItem: Specifies AEE_DEVICEITEM_SSDA
      pBuff: Buffer capable of holding SSDA - 8 bytes of data.
      pnSize: Place holder for number of bytes required.
*

   Name:
   AEE_DEVICEITEM_ESN
~
   Description:
  This returns the ESN for the device.  For devices which have a pseudo-ESN (and not a
  true ESN), this function will return the pseudo-ESN.
  The AEECLSID_CDMACREDENTIALS or AEECLSID_HWIDS privilege is required to access this information.

===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      nItem: Specifies AEE_DEVICEITEM_ESN
      pBuff: Address of uint32 to hold the ESN
      pnSize: Place holder for number of bytes required
*

   Name:
   AEE_DEVICEITEM_SOFTKEY_COUNT
~
   Description:
   This returns how many softkeys the device has.
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      po: Pointer to the IShell object

      nItem: Specifies AEE_DEVICEITEM_SOFTKEY_COUNT

      pBuff: Pointer to a buffer large enough to hold a uint8, to get the number of
                 softkeys that the device has.

      pnSize: Place holder for number of bytes required (if pBuff is NULL or *pnSize < required size)
              or number of bytes written in the provided buffer.

   Return Value:
      EUNSUPPORTED : Device doesn't support specified item
      EBADPARM : pnSize == NULL
      SUCCESS : Otherwise
*

   Name:
   AEE_DEVICEITEM_KEY_SUPPORT
~
   Description:

   This indicates if the key specified by a key code is supported.
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      po: Pointer to the IShell object

      nItem: Specifies AEE_DEVICEITEM_KEY_SUPPORT

      pBuff: Pointer to a KeySupportType struct. A key code is provided on input and a
             boolean result is returned indicating if that key is supported.

      pnSize: Place holder for number of bytes required (if pBuff is NULL or *pnSize < required size)
              or number of bytes written in the provided buffer.

*
   Name:
   AEE_DEVICEITEM_REPEAT_TIME
~
   Description:

   This gives the preferred repeat times for components.
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      po: Pointer to the IShell object

      nItem: Specifies AEE_DEVICEITEM_REPEAT_TIME

      pBuff: Pointer to a AEEAutoRepeat struct. An item code is provided on input and a
             preferred default value is specified in the remaining structure members. If 0,
             no preference for defaults.

      pnSize: Place holder for number of bytes required (if pBuff is NULL or *pnSize < required size)
              or number of bytes written in the provided buffer.

   Return Value:
      EUNSUPPORTED : Device doesn't support specified item
      EBADPARM : pnSize == NULL
      SUCCESS : Otherwise

   Comments:
      None

   See Also:
      ISHELL_GetDeviceInfoEx()

*

  Name:
  AEE_DEVICEITEM_ICCID
~
   Description:
  This returns the ICCID for the device.
  AEECLSID_GSMCREDENTIALS is required privilege level.

===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      nItem: Specifies AEE_DEVICEITEM_ICCID
      pBuff: Buffer capable of holding ICCID

*

   Name:
   AEE_DEVICEITEM_CAMERA_INFO
~
   Description:

   This returns a list of supported camera sensors installed on the device. Each
   camera is identified by AEECLSID. The list is null-terminated.
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      po: Pointer to the IShell object

      nItem: Specifies AEE_DEVICEITEM_CAMERA_INFO

      pBuff: Pointer to null-terminated list of AEECLSIDs

      pnSize: Place holder for number of bytes required (if pBuff is NULL or *pnSize < required size)
              or number of bytes written in the provided buffer.

*

   Name:
   AEE_DEVICEITEM_POSDET_PRIVACY_ALERT
~
   Description:

   This returns a boolean value indicating whether users will be prompted before a 
   position determination request is placed.

===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      po: Pointer to the IShell object

      nItem: Specifies AEE_DEVICEITEM_POSDET_PRIVACY_ALERT

      pBuff: Pointer to a buffer for a boolean value, buffer must be at least a big as sizeof(boolean)

      pnSize: [in/out] On input, when pBuff is non-NULL, pnSize specifies the size of pBuff in 
      bytes.   On output, pnSize will be set to the number of bytes required to hold the 
      DeviceInfoEx() item data.

*

   Name:
   AEE_DEVICEITEM_POSDET_EMERGENCY_ONLY
~
   Description:

   This returns a boolean value indicating whether the device is set to restrict position 
   determination requests to emergency services such as E911.

===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      po: Pointer to the IShell object

      nItem: Specifies AEE_DEVICEITEM_POSDET_EMERGENCY_ONLY

      pBuff: Pointer to a buffer for a boolean value, buffer must be at least a big as sizeof(boolean)

      pnSize: [in/out] On input, when pBuff is non-NULL, pnSize specifies the size of pBuff in 
      bytes.   On output, pnSize will be set to the number of bytes required to hold the 
      DeviceInfoEx() item data.  
      
*

  Name:
  AEE_DEVICEITEM_TAPI_SMS_ENCODING
~
   Description:
  This returns the encoding ITAPI_SendSMS() expects for the MO SMS payload
  provided to it.

===pre>
  int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

  Parameters:
      nItem: Specifies AEE_DEVICEITEM_TAPI_SMS_ENCODING
      pBuff: Buffer capable of holding AEE_ENC_XXX values      
      pnSize: Place holder for number of bytes required (if pBuff is NULL or *pnSize < required size)
              or number of bytes written in the provided buffer.
              
*

   Name:
   AEE_DEVICESTATE_KEY_INFO
~
   Description:
   This returns information for the specified key based on the flag chosen. Following
   flags are defined:
   AEE_DEVICESTATEFLAG_KEY_PRESSED - returns TRUE/FALSE to indicate if a key is pressed or not.       
   AEE_DEVICESTATEFLAG_KEY_AVAILABLE - returns TRUE/FALSE to indicate if a key is available or not.
      (e.g when key guard is active a key may be unavailable).
   AEE_DEVICESTATEFLAG_KEY_OVERLOADED_INFO - returns AVKType of the key code resulting from using
      the specified key with the modiferKey (Alt, Shift, etc.).
                                             
   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
   
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>
   Parameters:
      nItem: Specifies AEE_DEVICESTATE_KEY_INFO
      pBuff: Place holder for AEEKeyInfoType value. Buffer must be of sizeof(AEEKeyInfoType) 
      size at least.
         AEEKeyInfoType.wKey       - The AVKType code for key being queried for pressed state.
         AEEKeyInfoType.flag       - Specifies AEE_DEVICESTATEFLAG_KEY_PRESSED
         AEEKeyInfoType.pData      - Place holder for boolean value. Buffer must be of sizeof(boolean)
                                     at least.
         AEEKeyInfoType.pnDataSize - Sizeof(boolean) at least.
    
    
===pre>
    int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>
    Parameters:
    nItem: Specifies AEE_DEVICESTATE_KEY_INFO
    pBuff: Place holder for AEEKeyInfoType value. Buffer must be of sizeof(AEEKeyInfoType) 
    size at least.
       AEEKeyInfoType.wKey       - The AVKType code for key being queried for availability.
       AEEKeyInfoType.flag       - Specifies AEE_DEVICESTATEFLAG_KEY_AVAILABLE
       AEEKeyInfoType.pData      - Place holder for boolean value. Buffer must be of sizeof(boolean)
                                   at least.
       AEEKeyInfoType.pnDataSize - Sizeof(boolean) at least.

    
===pre>
    int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>
    Parameters:
    nItem: Specifies AEE_DEVICESTATE_KEY_INFO
    pBuff: Place holder for AEEKeyInfoType value. Buffer must be of sizeof(AEEKeyInfoType) 
    size at least.
       AEEKeyInfoType.wKey       - The AVKType code for primary key.
       AEEKeyInfoType.flag       - Specifies AEE_DEVICESTATEFLAG_KEY_OVERLOADED_INFO
       AEEKeyInfoType.pData      - Place holder AVKType. Buffer must be of sizeof(AVKType)
                                   at least. Value on input is AVKType of modifier key.
                                   Value on output is the AVKType code resulting when the
                                   primary key is used with modifier key.
       AEEKeyInfoType.pnDataSize - Sizeof(AVKType) at least.

*

   Name:
   AEE_DEVICESTATE_TESTSIG_MOD
~
   Description:
   Determines if device has loaded a module with test signature. pBuff is used
   as a boolean and returns TRUE if the device has loaded a module with test
   signature. Once a device loads a test signature module, the value returned
   for AEE_DEVICESTATE_TESTSIG_MOD is TRUE till the phone is power cycled
   without the test signature module.

   When this ID is passed to ISHELL_GetDeviceInfoEx(), the following details apply:
===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_DEVICESTATE_TESTSIG_MOD
      pBuff: Place holder for boolean value for. Buffer must be of 
             sizeof(boolean) size at least.
*

   Name:
   EE_DEVICEITEM_FSSPACE_CARD0
~
   Description:

   This returns the total and free space on the primary mmc/sd card supported on
   the device. This API works even if the filesystem is greater than 4GB in size.

===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      po: Pointer to the IShell object

      nItem: Specifies EE_DEVICEITEM_FSSPACE_CARD0

      pBuff: Pointer to a AEEFSFreeSpace

      pnSize: [in/out] On input, when pBuff is non-NULL, pnSize specifies the size of pBuff in 
      bytes.   On output, pnSize will be set to the number of bytes required to hold the 
      DeviceInfoEx() item data.  

*

   Name:
   AEE_DEVICEITEM_FSSPACE_ROOT
~
   Description:

   This returns the total and free space on the main internal flash on the device. 
   This API works even if the filesystem is greater than 4GB in size.

===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      po: Pointer to the IShell object

      nItem: Specifies AEE_DEVICEITEM_FSSPACE_ROOT

      pBuff: Pointer to a AEEFSFreeSpace

      pnSize: [in/out] On input, when pBuff is non-NULL, pnSize specifies the size of pBuff in 
      bytes.   On output, pnSize will be set to the number of bytes required to hold the 
      DeviceInfoEx() item data.  

*

   Name:
   AEE_DEVICEITEM_FSSPACE_USBHMS1
~
   Description:

   This returns the total and free space on the first supported USB Host Mass
   Storage Device - AEEFS_USB_HOST_MASS_STORAGE1. This device item is supported
   from BREW version 3.1.5 onwards.

===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      po: Pointer to the IShell object

      nItem: Specifies AEE_DEVICEITEM_FSSPACE_USBHMS1

      pBuff: Pointer to a AEEFSFreeSpace

      pnSize: [in/out] On input, when pBuff is non-NULL, pnSize specifies the size of pBuff in 
      bytes.   On output, pnSize will be set to the number of bytes required to hold the 
      DeviceInfoEx() item data.  

*

   Name:
   AEE_DEVICEITEM_FSSPACE_USBHMS2
~
   Description:

   This returns the total and free space on the second supported USB Host Mass
   Storage Device - AEEFS_USB_HOST_MASS_STORAGE2. This device item is supported
   from BREW version 3.1.5 onwards.

===pre>
   int   ISHELL_GetDeviceInfoEx(IShell *po, AEEDeviceItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      po: Pointer to the IShell object

      nItem: Specifies AEE_DEVICEITEM_FSSPACE_USBHMS2

      pBuff: Pointer to a AEEFSFreeSpace

      pnSize: [in/out] On input, when pBuff is non-NULL, pnSize specifies the size of pBuff in 
      bytes.   On output, pnSize will be set to the number of bytes required to hold the 
      DeviceInfoEx() item data.  
=======================================================================
KeySupportType

Description:
This structure is passed to ISHELL_GetDeviceInfoEx() for AEE_DEVICEITEM_KEY_SUPPORT.
A key code is provided on input and a boolean result is returned indicating
if that key is supported.

Definition:
typedef struct
{
   AVKType  key;        // input
   boolean  supported;  // output
} KeySupportType;

Members:

key :       key code of key to check
supported : TRUE if key is supported on device


Comments:
None

See Also:
None

=======================================================================
AEEKeyInfoType

Description:
This structure is passed to ISHELL_GetDeviceInfoEx() for AEE_DEVICESTATE_KEY_INFO.
A key code and flag is provided on input and a flag specific result is expected on
output.

Definition:
typedef struct
{
AVKType wKey;       // input.
uint32  flag;       // input.
void*   pData;      // input, output.
uint32* pnDataSize; // input, output.
} AEEKeyInfoType;

Members:

wKey       : Input parameter. Any valid AVK key code (BREW or OEM range).
flag       : Input parameter. Flag for which info is needed.  Following flags are supported.
             AEE_DEVICESTATEFLAG_KEY_PRESSED         - determines if a key is pressed.
             AEE_DEVICESTATEFLAG_KEY_AVAILABLE       - determines if a key is available
                (e.g when key guard is active a key may be unavailable).
             AEE_DEVICESTATEFLAG_KEY_OVERLOADED_INFO - returns the overloaded key info. E.g
                when a key is pressed with a modifier key like Alt, Shift a different key code
                may be generated from when the key is pressed without the modifier key.
pData      : Input, Output. Data specific to flag chosen.

             For the flags AEE_DEVICESTATEFLAG_KEY_PRESSED, AEE_DEVICESTATEFLAG_KEY_AVAILABLE:
             - On input valid pData expected of type pointer to boolean. Any value in *pData is ignored.
             - On output the data in boolean indicates TRUE/FALSE.
             
             For the flag AEE_DEVICESTATEFLAG_KEY_OVERLOADED_INFO:
             - On input *pData contains the AVKType of the modifier key.
             - On output *pData contains the AVKType of key which will be generated when the key is pressed
               with the modifier key.
pnDataSize : For non NULL pData contains pData size on input and size of data returned in pData
             on output.
             For NULL pData, where the size of pData is not predetermined, contains the maximum
             size of paData which is needed on output. pData should not be NULL for flags where 
             the pData size is predetermined such as for flags: AEE_DEVICESTATEFLAG_KEY_PRESSED, 
             AEE_DEVICESTATEFLAG_KEY_AVAILABLE and  AEE_DEVICESTATEFLAG_KEY_OVERLOADED_INFO.

Comments:
None

See Also:
None

=======================================================================
AEEAutoRepeat

Description:
This structure is passed to ISHELL_GetDeviceInfoEx() for AEE_DEVICEITEM_REPEAT_TIME.
An item code is provided on input and a preferred default value is specified in the
remaining structure members. If 0, no preference for defaults.

Definition:
typedef struct AEEAutoRepeat
{
   uint16   wItem;
   uint32   dwStart;
   uint32   dwRate;
} AEEAutoRepeat;

Members:

wItem :        Item to obtain repeat timer values, see AEEREPEAT_ below
dwStart :      Time in Milliseconds before initial repeat action is taken
dwRate :       Time in Milliseconds between each subsequent repeat action

Comments:
If dwStart or dwRate is 0, no preference is specified for that interval.
The dwStart is given as the time AFTER the first action is taken, it is the
interval to the beginning of the actual first REPEATED action.
     |--------------dwStart time--------------|---dwRate time---|---dwRate time---|...
     ^                                        ^                 ^                 ^
event occurs                             first repeat       next repeat       next repeat...

AEEREPEAT_SCROLLBAR :   Pen held in region above or below thumb in scrollbar for action to repeat
                        Default:
                        dwStart = SB_AUTOREPEAT_START (200)
                        dwRate = SB_AUTOREPEAT_RATE (75)

See Also:
ISHELL_GetDeviceInfoEx()

=======================================================================
AEEFSFreeSpace

Description:
his structure is passed to ISHELL_GetDeviceInfoEx() for 
AEE_DEVICEITEM_FSSPACE_ROOT and AEE_DEVICEITEM_FSSPACE_CARD0. On 
output the structure provides the total and free space on the 
respective filesystems. The API works for filesystem greater than 4GB 
in size.

Definition:
typedef struct AEEFSFreeSpace
{
uint64   qwTotalSpace; 
uint64   qwFreeSpace;  
} AEEFSFreeSpace;

Members:

qwTotalSpace : // [out] Total Space in bytes on the filesystem
qwFreeSpace  : // [out] Free Space in bytes on the filesystem

Comments:
None

See Also:
None
=======================================================================
JulianType

Description:
This structure contains Julian date information.

Definition:
typedef struct
{
   uint16 wYear;
   uint16 wMonth;
   uint16 wDay;
   uint16 wHour;
   uint16 wMinute;
   uint16 wSecond;
   uint16 wWeekDay;
} JulianType;

Members:

wYear : 4-digit year
wMonth  :  Month 1-12(January=1, December=12)
wDay : Day 1-31
wHour : Hour 0-23
wMinute : Minute 0-59
wSecond : Seconds 0-59
wWeekDay : Day of the week 0-6 (0=Monday, 6=Sunday)

Comments:
None

See Also:
None

=======================================================================
AEEHandlerType

Description:

This former ENUM specified the handler type in ISHELL_RegisterHandler(),
   ISHELL_GetHandler(), etc.

Definition:
    typedef AEEHandlerType enum
    {
       HTYPE_VIEWER,
       HTYPE_SOUND
       HTYPE_BROWSE
    } AEEHandlerType;

Members:

  HTYPE_VIEWER: To specify a handler of class IViewer that provides image
               viewing services (for un-qualified "image/\*" mimetypes)
  HTYPE_SOUND : To specify a handler of class ISoundPlayer that provides sound
               playing services (for un-qualified "snd/\*" mimetypes)
  HTYPE_BROWSE: To specify a handler of class IApplet, supporting either
               schemes (http, mailto, etc.) or file extensions (.gif,
               .htm, etc.). HTYPE_BROWSE is the most poorly supported and
               should be avoided

Comments:
  This type is deprecated in BREW Client 3.1 and AEECLSID is used instead

See Also:
  None

=======================================================================
BeepType

Description:
This ENUM specifies the beep type in ISHELL_Beep() function.

Definition:
typedef enum {
   BEEP_OFF,
   BEEP_ALERT,
   BEEP_REMINDER,
   BEEP_MSG,
   BEEP_ERROR,
   BEEP_VIBRATE_ALERT,
   BEEP_VIBRATE_REMIND
} BeepType;

Members:

BEEP_OFF : Beep Off
BEEP_ALERT : Beep to alert mobile user
BEEP_REMINDER : Reminder beep
BEEP_MSG : Beep to indicate arrival of SMS message
BEEP_ERROR : Beep to indicate error
BEEP_VIBRATE_ALERT : Silent mode vibration to alert mobile user
BEEP_VIBRATE_REMIND : Silent mode vibration to remind mobile user


Comments:
On a target device, BeepType is dependent on device manufacturers. In the BREW Simulator,
device manufacturers or application developers can define their own BeepType tones.
For example, to create your own BEEP_ALERT tone, save your .WAV file as "BEEP_ALERT.wav"
in \Brew\bin\DataFiles directory. Sample tones for all beep types are provided and
their usage is illustrated in the sample sound app.

See Also:
None

=======================================================================
AEEPromptInfo

Description:
This structure specifies the prompt information used by the ISHELL_Prompt() function.

Definition:
typedef struct
{
   const char * pszRes;
   const AECHAR * pTitle;
   const AECHAR * pText;
   uint16 wTitleID;
   uint16 wTextID;
   uint16 wDefBtn;
   const uint16 * pBtnIDs;
   uint32 dwProps;
   AEEFont fntTitle;
   AEEFont fntText;
   uint32 dwTimeout;
} AEEPromptInfo;

Members:

pszRes : Resource file name used for prompt title and text information
pTitle : If pszRes is not specified, the pointer to wide string containing the prompt
title
pText : If pszRes is not specified, the pointer to wide string containing the prompt
text
wTitleID : If pszRes is specified, the ID of the title string in the resource file
wTextID : If pszRes is specified, the ID of the text string in the resource file
wDefBtn : The ID of the default (or selected button)
pBtnIDs : Pointer to array of button IDs
dwProps : Property of the prompt (the properties for the IStatic Interface )
fntTitle : Title font
fntText : Text font
dwTimeout : Timeout for the prompt. If set to 0 (zero), the prompt has no timeout


Comments:
If pszRes ,pTitle, and pText are non-NULL, the prompt title and text are read from
the resource file.

See Also:
None

=======================================================================
AEEPosAccuracy

Description:
This data structure describes the Position Location Information Accuracy

Definition:
typedef enum {
   AEE_ACCURACY_LOW,
   AEE_ACCURACY_MED,
   AEE_ACCURACY_HIGH
} AEEPosAccuracy;

Members:
None.

Comments:
The position location information precision is directly related to the time it will
take to satisfy the ISHELL_GetPosition() request.

See Also:
ISHELL_GetPosition()

=============================================================================

AEEPositionInfo

Description:
This data structure describes thePosition Location Information

Definition:
typedef struct
{
   int32 dwLat;
   int32 dwLon;
   uint32dwTimeStamp;
} AEEPositionInfo;

Members:

dwLat: Latitude, 180/2^25 degrees, WGS-84 ellipsoid
dwLon: Longitude, 360/2^26 degrees, WGS-84 ellipsoid
dwTimeStamp: Time Stamp, seconds since 1/6/1980


Comments:
None.

See Also:
ISHELL_GetPosition() ,

=======================================================================
PFNAEEEVENT

Description:
PFNAEEEVENT specifies the type of the event callback passed to IDIALOG_SetEventHandler().

Definition:
typedef boolean ( * PFNAEEEVENT)
(
   void * pUser,
   AEEEvent evt,
   uint16 w,
   uint32 dw
);

Members:
pUser: User Data
evt: Event code
w: 16-bit event-specific parameter
dw: 32-bit event-specific parameter

Comments:
None.

See Also:
IDIALOG_SetEventHandler()

=============================================================================

PFNPOSITIONCB

Description:
PFNPOSITIONCB specifies the type of the callback function passed to ISHELL_GetPosition().

Definition:
typedef void ( * PFNPOSITIONCB)
(
   void * pUser,
   AEEPositionInfo * pli,
   int nErr
);

Members:
pUser: User Data
pli: Position Location Information
nErr: Error Code

Comments:
None.

See Also:
AEEPositionInfo
ISHELL_GetPosition()

=======================================================================
AEEAppInfo

Description:
This structure is used for storing information about the applet.

Definition:
typedef struct
{
   AEECLSID       cls;
   char  *        pszMIF;           // App Resource file
   uint16         wIDBase;          // Base ID for locating title, icon, etc
   uint16         wAppType;         // Extended OEM/Carrier App Type
   uint16         wHostLo;
   uint16         wHostHi;
   uint16         wIDPrivSet;
   uint16         wFlags;           // Applet Flags (AFLAG_...)
} AEEAppInfo;

Members:

cls:              Applet ClassID
pszMIF:           Applet Resource file
wIDBase:          Base ID for locating title, icon, and other items in applet resource file
wAppType:         Enhanced (carrier-defined) app type.
wHostLo:          Low word of the applet host.
wHostHi:          Hi word of the applet host.
wIDPrivSet:       Resource ID of the applet privset.
wFlags:           AEE Applet Flags

Comments:
None

See Also:
AEE Applet Flags
=============================================================================

AEEAppStart

Description:
   A pointer to this structure is passed to applications in the dwParam field
on EVT_APP_START/EVT_APP_RESUME.

Definition:
typedef struct
{
   int            error;
   AEECLSID       clsApp;
   IDisplay *     pDisplay;
   AEERect        rc;
   const char *   pszArgs;
} AEEAppStart;

Members:

   error : Filled by app if there is an error
   clsApp: Applet ID
   pDisplay: Pointer to IDisplay Interface object
   rc: Rectangle for the Applet
   pszArgs: Pointer to character string of arguments.  These arguments are also
            passed via the EVT_APP_BROWSE_FILE/EVT_APP_BROWSE_URL inputs.

Comments:
   AEE_START_OEM - This bit is passed to apps during EVT_APP_START or
   EVT_APP_RESUME in the wParam field if the app was started externally
   by OEM software.

   AEE_START_RESTART - This bit is passed to apps during EVT_APP_START
   in the wParam field if the app was started as a result of failure
   to process EVT_APP_SUSPEND or if the app was closed due to low
   memory or application belongs to an inserted history entry.

   AEE_START_SSAVER - This bit is passed to apps during EVT_APP_START in the
   wParam field if the app was started as a screen saver. This is to help
   the app distinguish between when the app is started as a Screen Saver and
   when the app is normally launched (say, through AppMgr).


See Also:
    AEECLSID List
    AEERect
=============================================================================
AEEResetType

Description:
This structure is used in the function ISHELL_Reset() to reset the device

Definition:
typedef enum
{
   AEERESET_HARD,
   AEERESET_SOFT
} AEEResetType;

Members:

AEERESET_SOFT: This is the SoftReset option. It termintaes BREW and re-initializes it.
The calling application is stopped when BREW terminates.

AEERESET_HARD: This is the Hard Reset option. In this case, the device is
power-cycled.


In either Reset Options, the calling application is not re-started automatically.


Comments:
None

See Also:
ISHELL_Reset

=============================================================================
AEEHWIDType

Description:
This structure is returned from ISHELL_GetDeviceInfoEx() for AEE_DEVICEITEM_HWID
and AEE_DEVICEITEM_HWIDLIST.

Definition:
typedef struct {
   uint16 uLength;      
   byte bHWIDLength;
   byte bHWIDType;
   uint32 HWID[1];      // variable length == (bHWIDLength + 3)/4
} AEEHWIDType;

Members:

uLength: length of this AEEHWIDType structure in bytes;  used to compute offset of
         next element on the HWIDList.
bHWIDLength: length of HWID in bytes 
bHWIDType: type of this HWID.
HWDI: array containing the HWID.  This array is always aligned to a 32 bit boundary. 
      bHWIDLength gives the actual number of bytes used.

Comments:
   HWID format:
===pre>
      AEE_HWID_ESN:     uint32
      AEE_HWID_PESN:    uint32.  MSB == 0x80
      AEE_HWID_IMEI:    8 byte array:
                        ---------------------
                        | digit 1 |  type   | byte 0
                        | digit 3 | digit 2 | byte 1
                        ...
                        | digit 15| digit 14| byte 7
                        --------------------
                        The type field matches the GSM mobile identity type.
                        For IMEI with check digit, it is 0xa, without a check-
                        digit, it is 0x2.
      AEE_HWID_MEID:    uint64.  MEID is a 56 bit value;
                        the most significant 8 bits are always 0
===/pre>
See Also:
AEE_DEVICEITEM_HWID
AEE_DEVICEITEM_HWIDLIST

=====================================================================
   MACROS DOCUMENTATION
=======================================================================

AEE_RES_LANGDIR

Description:
   The string that ISHELL_LoadResXXX functions recognize in filenames
    as directives to searching for localized resource files.  Any 
    occurrence of this string in the filename argument will be replaced 
    by "<lang>/", where <lang> is a string representation of 
    AEEDeviceInfo.dwLang.

Definition:
   AEE_RES_LANGDIR "{langdir}"

Version:
    Introduced BREW Client 3.1

Comments:
   BREW makes two passes over the filename passed to the ISHELL_LoadResXX
   APIs.  Once to replace {langdir} and {langsuf} with their dwLang
   equivalents, and once to replace {langdir} and {langsuf} with the empty 
   string.  After each pass, BREW attempts to open the resource file with
   the resulting string.  If opening the named file succeeds, BREW proceeds
   to load the requested resource from that file.  If the resource doesn't 
   exist in the opened file, an error is returned.  There is no further 
   fallback once the file is successfully opened.

   For example: given the filename "{langdir}app.bar" on an "en" handset, 
   BREW's ISHELL_LoadResXX() logic can be expressed as:

      int nErr;

      if (AEE_SUCCESS == (nErr == openresfile("en/app.bar", &file)) ||
          AEE_SUCCESS == (nErr == openresfile("app.bar", &file))) {
         return loadresdata(file, ...);
      }
      return nErr;

   Similarly: given the filename "app{langsuf}.bar" on an "en" handset, 
   BREW's ISHELL_LoadResXX() logic can be expressed as:

      int nErr;

      if (AEE_SUCCESS == (nErr == openresfile("app_en.bar", &file)) ||
          AEE_SUCCESS == (nErr == openresfile("app.bar", &file))) {
         return loadresdata(file, ...);
      }
      return nErr;


   Using these strings in your resource file names allows for a 
   localized application to automatically load the correct resources
   for the handset's language.  Note that applications should use
   {langdir} or {langsuf}, probably not both.

See Also:
   AEE_RES_LANGSUF
   ISHELL_LoadResDataEx()
   ISHELL_LoadResData()

=======================================================================

AEE_RES_LANGSUF

Description:
   The string that ISHELL_LoadResXXX functions recognize in filenames
    as directives to searching for localized resource files.  Any 
    occurrence of this string in the filename argument will be replaced 
    by "_<lang>", where <lang> is a string representation of 
    AEEDeviceInfo.dwLang.

Definition:
   AEE_RES_LANGDIR "{langsuf}"

Version:
    Introduced BREW Client 3.1

See Also:
   AEE_RES_LANGDIR
   ISHELL_LoadResDataEx()
   ISHELL_LoadResData()

=======================================================================

RESBLOB_DATA()

Description:
   This macro retrieves a pointer to the raw data from an AEEResBlob

Definition:
   RESBLOB_DATA(b) ((void *)((byte *)(b) + b->bDataOffset))


Parameters:
   b: the AEEResBlob

Evaluation Value:
   a void * that points to the blob's data

Comments:
   None

Side Effects:
   None

Version:
    Introduced BREW Client 3.1

See Also:
   AEEResBlob
   ISHELL_LoadResDataEx()
   ISHELL_LoadResData()

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

IShell Interface

   The functions in the IShell Interface provide a variety of services to BREW applications.
   This section includes a high-level overview of the IShell Interface functions, with
   each subsection describing a group of related functions.
===H2>
Alarms
===/H2>

   The AEE Shell’s alarm functions enable an application to be notified when the current
   time reaches a specified value. Unlike timers, which can be active only while your
   application is running, you can receive notification that an alarm has expired even
   when your application is not running. Alarms are typically used when the time of
   notification is in the distant future. For example, a calendar application uses
   an alarm to alert the user when a calendar appointment is about to be reached.

   To set an alarm, you call ISHELL_SetAlarm() and specify the number of minutes from
   the current time at which the alarm notification is to occur, a 16-bit alarm code,
   and the BREW ClassID of the application (yours or another) that receives notification
   when the alarm time is reached. At notification time, the IAPPLET_HandleEvent()
   function of the notified application is called with an EVT_ALARM event and the 16-bit
   alarm code as parameters. The latter parameter allows an application to distinguish
   between more than one simultaneously active alarm. If the notified application is
   not currently running, the AEE Shell creates an instance of it to process the notification
   event, after which it is terminated. The application may choose to activate itself
   if necessary. The AEE Shell stores alarms in a BREW database and continuously checks
   for alarm expirations while the BREW-enabled device is turned on. If an alarm’s
   expiration time passes while the device is turned off, the AEE Shell generates the
   notification the next time the device is turned on.

   ISHELL_CancelAlarm() cancels a currently active alarm. ISHELL_AlarmsActive() checks
   whether any of BREW built-in annunciators (alarm clock, countdown timer, or stopwatch)
   are currently active.
===H2>
Application management
===/H2>

   The AEE Shell’s application management functions have a number of purposes, including
   the following:

    -    Creating, starting, and stopping BREW classes and applications

    -    Obtaining information about the modules and classes present on the device

    -    Allowing applications to send events to each other

    -    Allowing BREW applications to run without interfering with other activities
   that the device must perform

   ISHELL_CreateInstance() creates instances of both BREW classes and of user-defined
   classes supported by the modules present on the device. ISHELL_StartApplet() allows
   a specified applet to run. It creates an instance of the applet if necessary, suspends
   the currently running applet (if any), and then invokes the specified applet’s IAPPLET_HandleEvent()
   function with the EVT_APP_START event, which allows it to run. ISHELL_CloseApplet()
   sends the currently running applet the EVT_APP_STOP event and calls its Release
   function. ISHELL_CloseApplet() is used primarily by the AEE Shell itself,

   ISHELL_ActiveApplet() is used to obtain the ClassID of the applet that is currently top
   visible. ISHELL_EnumAppletInit() and ISHELL_EnumNextApplet() can be used to enumerate
   the applets in the modules that are present on the device. As each applet is enumerated,
   ISHELL_EnumNextApplet() returns a pointer to an AEEAppInfo data structure for the
   applet, which identifies the applet’s MIF file, titles, icons, and type (game, tool,
   PIM, and other types). ISHELL_QueryClass() determines if a particular class is available
   on the device. If the class is an applet, supply a pointer to an AEEAppInfo data
   structure, which is populated with the relevant information if the applet is available.

   Use ISHELL_SendEvent() to send an event to a specified class. If an instance of
   that class is not currently present, the AEE Shell creates one and then invokes
   its IAPPLET_HandleEvent() function with the specified event code and data parameters.
   Unless it chooses to start itself, the application terminates after it completes
   processing the event. ISHELL_PostEvent() is similar, except that the destination
   class’s IAPPLET_HandleEvent() function is not called immediately. The event is placed
   in a queue and is sent at a later time, which allows the calling application to
   continue its processing without interruption. ISHELL_HandleEvent() is used when
   the identity of the class that is to receive the event is not known. In this case,
   the AEE Shell sends the event to the currently running applet or its active dialog,
   if any.

   The AEE Shell includes several functions that enable BREW applications to coexist
   with other activities on the device. The BREW application model is based on cooperative
   multitasking, which means that each application must be designed to run for as short
   a time as possible when processing an event, and then exit to give other device
   activities a chance to run.

   ISHELL_Busy() lets a BREW application determine if any activities are in progress
   on the device that requires it to exit immediately. ISHELL_ForceExit() is identical
   to ISHELL_Busy(). ISHELL_Resume() allows an application to break time-consuming
   tasks into smaller, interruptible chunks. Each chunk is represented by a callback
   function and an associated data pointer. Calling ISHELL_Resume() schedules the callback
   function to be invoked at a later time with the data pointer as its only parameter.
   Use ISHELL_CanStartApplet() or IAppletCtl_Control() to check whether it is
   possible to start a BREW application, which may not be true if higher priority
   activities are in progress on the device.
===H2>
Dialogs, message boxes, and prompts
===/H2>

   A dialog consists of a screen containing one or more BREW controls that allow the
   device user to enter data or select an item from a menu. Although you may create
   such a screen with the BREW control interfaces (IDateCtl Interface, IMenuCtl Interface,
   ITextCtl Interface and ITimeCtl Interface), IDialog Interface simplifies this task:

    -    Create a dialog using the BREW Resource Editor including specification of
   each of the controls in the dialog. See the BREW Resource Editor Guide for more
   information.

    -    The BREW AEE maintains a stack of the dialogs associated with the currently
   running application. When you create a dialog, it is placed at the top of the stack;
   when a dialog ends, it is removed from the stack and the dialog below it is restored.
   This makes it easy to implement applications that step the device user through a
   sequence of screens (such as a menu hierarchy).

    -    When a dialog is active, it receives all events and distributes them to the
   currently active controls. The dialog also handles control tabbing, which allows
   the device user to move between controls in a multicontrol dialog. This frees you
   from having to implement code to distribute and handle these events.

   Use ISHELL_CreateDialog() to create a dialog. This function accepts either the identifier
   of a dialog you have created in the BREW Resource Editor or a pointer to a data
   structure that you populate in your code to specify the controls in the dialog.
   If successful, the function shows the dialog’s controls on the screen and pushes
   it onto the dialog stack. To end the dialog, you call ISHELL_EndDialog(), which
   terminates the dialog at the top of the stack and shows the dialog immediately below
   it on the stack. This function also sends the EVT_DIALOG_END event to your application,
   which allows you to perform any processing associated with the dialog’s termination,
   such as retrieving values entered by the device user in the dialog’s controls. ISHELL_EndDialog()
   also frees all the resources that are being used by the dialog.

   ISHELL_CreateDialog() does not return an interface pointer to the dialog it creates.
   ISHELL_GetActiveDialog() is used to obtain an IDialog Interface pointer for the
   dialog at the top of the stack. Use this pointer to invoke the functions that comprise
   the IDialog Interface: IDIALOG_GetControl(), which returns interface pointers to
   any of the dialog’s controls, and IDIALOG_SetFocus(), which is used to specify which
   control in a multicontrol dialog receives input from the device user.

   The AEE Shell also provides functions that can be used to create simple, commonly
   used dialogs with a single function call. ISHELL_Prompt() shows a dialog with a
   Soft Key control menu that prompts the device user to make a selection. When the
   device user does so, the selection is returned to your application using an EVT_COMMAND
   and the dialog is automatically terminated. There are also two functions that create
   a dialog that shows a read-only text message and title. ISHELL_MessageBox() reads
   the title and message text from a BREW resource file, while ISHELL_MessageBoxText()
   accepts pointers to title and message text strings that you specify in your code.
   The dialogs created by these functions end when the device user presses a key.
===H2>
Device and application configuration information
===/H2>

   These functions allow configuration information about the device and specific applications
   to be obtained. ISHELL_GetDeviceInfo() returns a pointer to an AEEDeviceInfo structure
   for the device, which includes information about its screen size and color support,
   amount of available memory, character encoding, and other items.

   ISHELL_GetPrefs() and ISHELL_SetPrefs() provide a general-purpose mechanism for
   applications to register configuration information. Use ISHELL_SetPrefs() to associate
   a pointer to configuration data and a version number for this data with the ClassID
   of your application. Other applications can obtain this data by calling ISHELL_GetPrefs()
   with the specified ClassID and version number. You also must provide a structure
   declaration that defines the content of each version of your application’s configuration
   data.
===H2>
Miscellaneous
===/H2>

   ISHELL_Beep() allows an application to provide several types of audible or vibrating
   signals to the device user. Depending on what a given device supports, there are
   audible signals that correspond to device-off, alert, reminder, message arrival
   and error events, and vibrating signals for alerts and reminders. There is also
   a Boolean loudness parameter that can be specified.
===H2>
Notifications
===/H2>

   IShell’s notification mechanism allows a BREW class to notify other classes that
   certain events have occurred. A class activated to receive a notification must register
   its interest with the AEE Shell specifying the ClassID of the notifier class and
   the events for which notification is specified. When an event requiring notification
   occurs, the notifier class calls ISHELL_Notify(), which sends notification to each
   class that has registered to be notified of the occurrence of that event.

   The AEE Shell provides two ways for a class to register for notification of an event:

    -    Register by specifying information about the notification in your application’s
   MIF file using the MIF Editor. This method of registering is used by applications
   that must be notified of events even when they are not running. One example is a
   call-logging application that receives notification of each incoming and outgoing
   call. Such an application needs to process notifications even while the device user
   is not running the application to display the call log.

    -    If notification is required only at certain times while your application is
   running, call ISHELL_RegisterNotify() to initiate event notification. For example,
   a game application might display a message when an incoming call arrives that allows
   the device user to accept the call or continue playing the game. This application
   requires notification of incoming calls only while the device user is actually playing
   the game, so it calls ISHELL_RegisterNotify() when the device user starts to play
   the game.

   The events for which a notifier class provides notifications are represented by
   a 32-bit variable. The low-order 16 bits of this variable contain the notification
   mask with each bit corresponding to one event. The high-order 16 bits contain the
   notifier match value, which can be used for data associated with an event. For example,
   events representing activity on a UDP socket use the match value for the port on
   which the activity has occurred. When it registers for notification, a class provides
   a value for the mask variable with the bit for each event of interest set to 1.
   If a class no longer requires notification of an event, it can call ISHELL_RegisterNotify()
   with that event’s bit set to 0 (zero) in the mask variable. For example, an application
   might call ISHELL_RegisterNotify() with the mask variable set to 0 (zero) upon termination.

   NOTE: The AEE Shell creates an instance of the notifier class when another class
   registers for notification of its events. This instance is released when all classes
   have unregistered for event notification.

   When a notifier class calls ISHELL_Notify(), the AEE Shell sends an event of type
   EVT_NOTIFY to each application that has registered for the event. This results in
   the invocation of the application’s IAPPLET_HandleEvent() function. If the application
   is not currently running, the AEE Shell creates an instance of it to process the
   event, after which the application terminates. The application can send itself a
   start event to continue running. In the call to IAPPLET_HandleEvent(), the application
   also is passed a pointer to a structure of type AEENotify, which identifies the
   event that occurred, the class that generated the notification, and some additional
   data specific to the notification.

   At present, the INetMgr class sends notifications to other interested classes of
   changes in the network connection state and the arrival of data on particular UDP
   ports. You can also implement your own notifier classes. Your class must implement
   the INotifier Interface (see the sample notifications application for details on
   how to declare this interface when defining your class). The INotifier Interface
   requires you to implement a INOTIFIER_SetMask() function for your class. The AEE
   Shell calls this function whenever another class registers for event notification
   from your class. INOTIFIER_SetMask() has a single argument, which is a 32-bit variable
   that is the logical OR of the mask variables that each class used when it registered
   for notifications from your class. This variable has a value of 1 for a bit if at
   least one class has registered for notification of the corresponding event. You
   can use this variable to perform any initialization needed when a class first registers
   to be notified of an event, and any finalization when there are no longer any classes
   requiring notification of a particular event.
===H2>
   To provide notifier functionality in your class
===/H2>

   1.   Provide an include file that contains constants defining the bits assigned
   to each notification event, and typedefs for the event-specific data that is provided
   as part of an event notification. This include file is used by applications that
   register for notifications from your class.

   2.   Call ISHELL_Notify() whenever a notification event occurs and at least one
   class has registered to be notified of that event’s occurrence.
===H2>
   Resource files and file handlers
===/H2>

   The AEE Shell provides a number of functions that your application can use to read
   various types of data from files. These files can be BREW resource (.bar) files
   created with the BREW Resource Editor, or they can be files whose content is associated
   with a MIME type or identified by the file’s extension. You can also extend the
   set of file types that the BREW AEE recognizes by defining your own handler classes
   and using them to manipulate files of particular MIME types.

   Use the following functions to access BREW resource files. Each function’s parameters
   include the name of the resource file and the integer resource ID:

    -    ISHELL_LoadResData() loads resources other than strings and bitmap images.
   At present, these include several resource types associated with dialogs. The memory
   used to store the resource information is freed by calling ISHELL_FreeResData().

    -    ISHELL_LoadResImage() loads a bitmap image from a specified resource file
   and returns a pointer to an instance of the IImage Interface that contains the bitmap.
   IIMAGE_Release() frees the data used to store the bitmap.

    -    ISHELL_LoadResObject() implements the sound and image loading functions. ISHELL_LoadImage(),
   ISHELL_LoadResImage(), ISHELL_LoadSound(), and ISHELL_LoadResSound() are all macros
   that invoke this function with different parameters.

    -    ISHELL_LoadResSound() loads a sound resource from the specified resource file
   and returns a pointer to an instance of the ISoundPlayer Interface that contains
   the sound file. ISOUNDPLAYER_Release() frees the data used to store the sound data.

    -    ISHELL_LoadResString() reads a string resource into a character buffer, a
   pointer to which is one of the function’s arguments.

   ISHELL_LoadImage() and ISHELL_LoadSound() can be used to load image and sound files
   directly without first placing their contents into a BREW resource file. The files
   must contain one of the built-in MIME types supported by the BREW AEE, which include
   Windows bitmap (BMP) or a device-specific native bitmap format for images and MIDI
   (.mid) or CMX (.pdm) sound files.

   Use ISHELL_RegisterHandler() to associate a MIME file type with the ClassID of the
   BREW handler class you have implemented to handle files of that type. ISHELL_GetHandler()
   returns the ClassID of the handler class associated with a given MIME type, including
   the BREW built-in types mentioned above.

   Call ISHELL_GetHandler() to get the ClassID of the handler class for the file’s
   MIME type.

   Call ISHELL_CreateInstance() to create an instance of the handler class.
===H2>
Timers
===/H2>

   The AEE Shell’s timer is used by a currently instantiated application (an application
   whose reference count is non-zero) to perform an action when a specified amount
   of time has passed. These time periods are typically short (seconds or milliseconds).
   Use the AEE Shell’s alarm functions to obtain notification when longer time periods
   have passed, even when your application is not currently instantiated.

   To start a timer, you call ISHELL_SetTimer() and specify the timer duration in milliseconds,
   the address of a callback function, and a pointer to an application-specific data
   structure. When the timer expires, the AEE Shell calls the callback function with
   the application-specific data pointer as its only parameter. BREW timers are one-time,
   nonrecurring timers, so it is not possible to specify a timer that repeats at a
   fixed interval. However, to obtain this behavior, call ISHELL_SetTimer() within
   your callback function and specify the function’s own address as ISHELL_SetTimer()’s
   callback function parameter.

   ISHELL_GetTimerExpiration() can be used to determine the number of milliseconds
   remaining before a particular timer expires. A timer is identified by the callback
   function and data structure addresses supplied when it was created. ISHELL_CancelTimer()
   cancels a running timer. If a NULL value is supplied for the callback function parameter,
   all timers associated with the specified data structure address are canceled. When
   an application instance’s reference count drops to 0 (zero), all timers associated
   with that application are canceled.



=============================================================================

ISHELL_AddRef()

Description:
    This function is inherited from IBASE_AddRef().
	
See Also:
    ISHELL_Release()

=============================================================================

ISHELL_Release()

Description:
    This function decrements the reference count of the IShell Interface object.
	The object is freed from memory and is no longer valid when the reference count 
reaches 0 (zero).

Prototype:
    uint32 ISHELL_Release(IShell * pIShell)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.

Return Value:
    Decremented reference count for the object.

    0 (zero), If the object has been freed and is no longer valid.

Comments:
    This function doesn't do anything. A call to this function is ignored by AEE.

Version:
    Introduced BREW Client 1.0

See Also:
    ISHELL_AddRef()

=======================================================================

ISHELL_CreateInstance()

Description:
   This function is called to create an object associated with the specified 32-bit
   ClassID. The object returned must match the interface supported by the ClassID.
   When successful, ppobj is filled with an object of the specified class and the object
   is returned with a positive reference count.

Prototype:
    int ISHELL_CreateInstance
    (
    IShell * pIShell,
    AEECLSID cls,
    void * * ppobj
    )

Parameters:
    pIShell : [in] : Pointer to the IShell Interface object.
    cls : [in] : 32-bit ClassID of the requested interface.
    ppobj : [out] : Pointer to the memory to fill with the pointer to the object.

Return Value:
    SUCCESS: class created.
    ENOMEMORY: insufficient memory.
    ECLASSNOTSUPPORT: class specified is not supported.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    AEECLSID List
    IMODULE_CreateInstance()

=======================================================================

ISHELL_QueryClass()

Description:
   This function queries the AEE Shell to determine if the specified class or applet
   is supported. If the AEEAppInfo data structure pointer is provided, AEE Shell assumes
   that the requested class is an applet. In that case, the structure is filled if
   the requested applet class was found. If the class is supported but is not an applet,
   the function returns FALSE. If the AEEAppInfo pointer is not passed, the AEE Shell
   assumes the request is made to check the availability of the class, regardless of
   whether it is an applet. This function also checks if the requester has the required
   privilege to create the requested class. If the requester does not have the right
   privilege, this function returns FALSE even if the class is actually available in
   the system.

Prototype:
    boolean ISHELL_QueryClass
    (
    IShell * pIShell,
    AEECLSID cls,
    AEEAppInfo * pai
    )

Parameters:
    pIShell : in] : Pointer to the IShell Interface object.
    cls : [in] : 32-bit ClassID of the requested interface.
    pai : [out] : Pointer to the AEEAppInfo structure to fill with the applet
                  information.

Return Value
    TRUE: class is supported.
    FALSE: class not supported.

Comments:
   If you provide an AEEAppInfo, this function returns FALSE if the class requested
   is available but is not an applet.

Version:
    Introduced BREW Client 1.0

See Also:
    AEEAppInfo
    ISHELL_CreateInstance()

=======================================================================

ISHELL_GetDeviceInfo()

Description:
   This function queries the AEE Shell for information about the capabilities of
   the device, such as the amount of supported RAM, the screen display, and other items.
   To obtain values for dwNetLinger and dwSleepDefer of the AEEDeviceInfo structure,
   wStructSize must be equal to sizeof(AEEDeviceInfo) before making the call to this
   function.
===pre>
 For example:

      AEEDeviceInfo di;
      di.wStructSize = sizeof(AEEDeviceInfo);
      ISHELL_GetDeviceInfo(pIShell, &di);
===/pre>

Prototype:
    void ISHELL_GetDeviceInfo(IShell * pIShell, AEEDeviceInfo * pi)

Parameters:
    pIShell :  Pointer to the IShell Interface object.
    pi  :  Pointer to the AEEDeviceInfo structure to fill.

Return Value:
    None

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    AEEDeviceInfo
    AEEDeviceItem

=======================================================================
ISHELL_GetDeviceInfoEx()

Description:
   This method is used to get specific information about the device. This function
   takes an ID that specifies what information is needed. The buffer contains the
   corresponding information on return.

Prototype:
    int ISHELL_GetDeviceInfoEx
    (
    IShell *pIShell,
    AEEDeviceItem nItem,
    void *pBuff,
    int *pnSize
    );

Parameters:
    pIShell : Pointer to the IShell Interface object
    nItem : Specifies the Item whose info is needed. Please see documentation of
            AEEDeviceItem for the supported Device Items.
    pBuff : Contains the corresponding information on return, may be NULL, if querying
            for size of the item.
    pnSize : [in/out] On input, when pBuff is non-NULL, pnSize specifies the
             size of pBuff in bytes.   On output, pnSize will be set to the
             number of bytes required to hold the DeviceInfoEx() item data.

Return Value:
    SUCCESS: if successful
    EBADPARM: if bad parameters are passed in (pnSize is NULL)
    EUNSUPPORTED: if this item is not supported

    Other errors from AEEError.h are possible, for example ENOMEMORY or EFAILED
     when errors gathering the device information occur.

Comments:
    This function may partially fill pBuff in the case where
     pnSize is not large enough to hold all item data.  In this case, pnSize
     will be set to the size required.

Version:
    Introduced BREW Client 2.0

See Also:
    AEEDeviceInfo
    AEEDeviceItem

=======================================================================
ISHELL_GetItemStyle()

Description:
   This function queries the AEE Shell for information about the default style
   for menu, icon, or list items. The information is placed in the two specified
   pointers. The first, pNormal, contains information for drawing the item in a
   normal (non-selected) case. The second, pSel, contains information for drawing
   the item in the selected case.

Prototype:
    void ISHELL_GetItemStyle
    (
    IShell * pIShell,
    AEEItemType t,
    AEEItemStyle * pNormal,
    AEEItemStyle * pSel
    )

Parameters:
    pIShell : [in] : Pointer to the IShell Interface object.
    t : [in] : Item type.
    pNormal : [out] : Pointer to the AEEItemStyle to fill for items that are
                      not selected.
    pSel : [out] : Pointer to the AEEItemStyle to fill for selected items.

Return Value:
    None

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    AEEItemType
    AEEItemStyle

=======================================================================
ISHELL_StartApplet()

Description:
   This function instructs the AEE Shell to start the applet associated with the
   specified 32-bit ClassID. If the requested class is supported and can be started,
   the AEE Shell loads and starts the applet. This call returns immediately to the
   caller before starting the applet.

   The applet is started asynchronously, so if the applet classID being specified
   is not found, the function returns TRUE but the applet is not started. Asynchronous
   applet start success or failure is notified to applications registered with IShell
   with NMASK_SHELL_START_STATUS mask. When the applet is started or when it is resumed, 
   the display is cleared (if the AFLAG_POPUP flag is not set) and the IAPPLET_HandleEvent() 
   is sent the EVT_APP_START or EVT_APP_RESUME event along with the AEEAppStart parameter 
   block. If an applet is started by another applet, the currently active applet is 
   suspended.

   Please note the following:

   - ISHELL_StartApplet is an asynchronous function.  The new applet
   will not be started until control is released to the system.

   - ISHELL_StartApplet will cause the current applet to receive an EVT_APP_SUSPEND
   event immediately before the new application is started.

   - If an applet has failed to exit properly the last time it ran, the return
   code of EBADSHUTDOWN will be returned by ISHELL_StartApplet.
   
   - If the applet being started is the screen saver app (SSA_SSAVER flag is set) the 
   top visible app will be sent EVT_APP_NO_CLOSE(EVT_BUSY) event. If the SSA_SSAVER flag
   of the applet being started is not set the top visible app will NOT be sent 
   EVT_APP_NO_CLOSE(EVT_BUSY).

Prototype:
    int ISHELL_StartApplet(IShell * pIShell, AEECLSID cls)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    cls  :  32-bit applet ClassID.

Return Value:
   SUCCESS: if Applet created and started
   EALREADY: if Another applet is already starting
   EBADCLASS: if Shell class supplied invalid
   ENOMEMORY: if Insufficient memory
   ECLASSNOTSUPPORT: if Class specified is not supported
   EEXPIRED: if Applet has expired
   EBADSHUTDOWN: if Applet did not exit properly the last time it ran.
   EFILENOEXISTS: if The applet has been disabled
   EUNABLETOLOAD: if OEM layer refused to load applet
   EBADSID: if Applet downloaded from different SIM card
   EITEMBUSY: If current top visible application will not release control

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
   ISHELL_CreateInstance
   ISHELL_StartAppletEx
   ISHELL_StartBackgroundApplet
   ISHELL_BrowseFile
   ISHELL_BrowseURL
   NMASK_SHELL_START_STATUS

=======================================================================

Function: ISHELL_StartAppletEx()

Description:
   This method instructs the AEE to start the applet associated
   with the specified 32-bit class ID using the flags specified.  This
   call behaves identically to the ISHELL_StartApplet function with the
   following differences:

   1)  If the SSA_NO_VOICE_CALL flag is specified, the applet will not
   be started during a voice call.

   2)  If the SSA_AFTER_VOICE_CALL flag is specified, the applet will
   be started after the current voice call ends and any OEM-defined call
   ended dialogs are dismissed.

   3)  If the SSA_CONFIG flag is called, the applet is started with the
   EVT_APP_CONFIG event.

   4)  If the SSA_HIDDEN_CONFIG flag is specified, the applet is started with
   the EVT_APP_HIDDEN_CONFIG event.

   5)  If the SSA_BACKGROUND flag is specified, the applet is started
   in the background. This flag cannot be specified on an application that
   is already in the app-history. Doing so will return ENOTALLOWED.

   6)  If the SSA_SSAVER flag is specified, the applet is started
   in screen saver mode.

   7)  If the SSA_UNIQUE flag is specified, the applet is started
   with new/unique instance.

   Note that if neither the SSA_CONFIG or SSA_HIDDEN_CONFIG flags are
   specified, the app is started with the standard EVT_APP_START event.

Prototype:

   int ISHELL_StartAppletEx(IShell *po, AEECLSID cls,uint16 wFlags);

Parameters:
   po: Pointer to the IShell object
   cls: 32-bit applet class ID
   wFlags:  Behavior flags (SSA_XXXX)

Return Value:
   SUCCESS: if Applet created and started
   EALREADY: if Another applet is already starting
   EBADCLASS: if Shell class supplied invalid
   ENOMEMORY: if Insufficient memory
   ECLASSNOTSUPPORT: if Class specified is not supported
   EEXPIRED: if Applet has expired
   EITEMBUSY: If current top visible application will not release control
   EFILENOEXISTS: if The applet has been disabled
   EUNABLETOLOAD: if OEM or current app won't allow loading this applet
   ENOTALLOWED: if the flag SSA_BACKGROUND is passed to start an applet that
   is already in the app-history
   AEE_STREAM_WOULDBLOCK: if the flag SSA_AFTER_VOICE_CALL is specified and
   thus application start is deferred because of ongoing voice call

Comments:
   None

Side Effects:
   None

Version:
    Introduced BREW Client 2.0

See Also:
   ISHELL_StartApplet
   ISHELL_BrowseFile
   ISHELL_BrowseURL
   ISHELL_StartBackgroundApplet


=======================================================================

ISHELL_StartAppletArgs()

Description:
   This method allows passing command line arguments to a BREW application when
   it is started. This method instructs the AEE to start the applet associated with
   the specified 32-bit class ID, passing it the arguments. If the requested class
   is supported and can be started, the AEE loads and starts the applet. Note that
   this call returns immediately to the caller before starting the applet. The applet
   is started asynchronously. Hence, if the applet class ID being specified is not
   found, this function will still return TRUE but the applet will not be started.

   When the applet is started or when it is resumed the display is cleared 
   (if the AFLAG_POPUP flag is not set) and the ISHELL_HandleEvent() is sent the 
   EVT_APP_START or EVT_APP_RESUME along with the AEEAppStart parameter block.

   NOTE:
   ISHELL_StartApplet() is an asynchronous function. The new applet is not started
   until control is released to the system.

   -    ISHELL_StartApplet() causes the current applet to receive an EVT_APP_SUSPEND
        event immediately before the new application is started.
   -    Start arguments are passed as a character string in the AEEAppStart block.
        The application may also receive an EVT_APP_BROWSE_FILE or EVT_APP_BROWSE_URL
        if the arguments are non-NULL.

   Applications that do not handle the EVT_APP_SUSPEND command, do not receive
   the argument block upon subsequent EVT_APP_START notifications when the application
   is restarted as part of the resume process.

Prototype:
    int ISHELL_StartAppletArgs
    (
    IShell *pIShell,
    AEECLSID cls,
    const char * pszArgs
    )

Parameters:
    pIShell :  Pointer to the IShell object
    cls :  32-bit applet class ID
    pszArgs :  ASCIIZ string of arguments to the application.

Return Value:
   SUCCESS: if Applet created and started
   EALREADY: if Another applet is already starting
   EBADCLASS: if Shell class supplied invalid
   ENOMEMORY: if Insufficient memory
   ECLASSNOTSUPPORT: if Class specified is not supported
   EEXPIRED: if Applet has expired
   EITEMBUSY: If current top visible application will not release control
   EFILENOEXISTS: if The applet has been disabled
   EUNABLETOLOAD: if OEM or current app won't allow loading this applet

Comments:
    None

Version:
    Introduced BREW Client 1.1

See Also:
    ISHELL_CreateInstance
    ISHELL_StartAppletEx
    ISHELL_StartBackgroundApplet
    ISHELL_BrowseFile
    ISHELL_BrowseURL

=======================================================================

ISHELL_StartBackgroundApplet()

Description:
   This method is identical to ISHELL_StartAppletArgs with the exception
   that the specified application is started in the background. In this case, the 
   app receives the EVT_APP_START_BACKGROUND event instead of the EVT_APP_START event.
   Calling this function on an application that has already started and is currently in the 
   app-history will fail and  return ENOTALLOWED

Prototype:
    int ISHELL_StartBackgroundApplet(IShell *pIShell,AEECLSID cls,const char * pszArgs)

Parameters:
    pIShell :  Pointer to the IShell object
    cls :  32-bit applet class ID
    pszArgs :  ASCIIZ string of arguments to the application.

Return Value:
   SUCCESS: if Applet created and started
   EALREADY: if Another applet is already starting
   EBADCLASS: if Shell class supplied invalid
   ENOMEMORY: if Insufficient memory
   ECLASSNOTSUPPORT: if Class specified is not supported
   EEXPIRED: if Applet has expired
   EFILENOEXISTS: if The applet has been disabled
   EUNABLETOLOAD: if OEM won't allow loading this applet
   ENOTALLOWED: This application is already in the app-history and hence cannot be started
   in the background
Comments:
    None

Version:
    Introduced BREW Client 3.1

See Also:
    ISHELL_CreateInstance
    ISHELL_StartAppletEx
    ISHELL_BrowseFile
    ISHELL_BrowseURL

=======================================================================

Function: ISHELL_BrowseFile()

Description:
   This method instructs the AEE to start the applet associated with the
   file type specified (by extension).  The applet will be started and will
   receive the EVT_BROWSE_FILE event.  Startup behavior is identical to the
   ISHELL_StartApplet function.

Prototype:

   int ISHELL_BrowseFile(IShell *po, const char * pszFile);

Parameters:
   po: Pointer to the IShell object
   pszFile: File name

Return Value:

  SUCCESS: Applet created and started
  EALREADY: Another applet is already starting
  EBADCLASS: Shell class supplied invalid
  ENOMEMORY: Insufficient memory
  ECLASSNOTSUPPORT: Class specified is not supported
  EEXPIRED: Applet has expired

Comments:
   None

Side Effects:
   None

Version:
    Introduced BREW Client 1.0

See Also:
   ISHELL_StartApplet
   ISHELL_StartAppletEx
   ISHELL_BrowseURL

=======================================================================

Function: ISHELL_BrowseURL()

Description:
   This method instructs the AEE to start the applet associated with the
   URL scheme specified.  The applet will be started and will receive the
   EVT_BROWSE_URL event.  Startup behavior is identical to the
   ISHELL_StartApplet function.

Prototype:
   int ISHELL_BrowseURL(IShell *po, const char * pszURL);

Parameters:
   po: Pointer to the IShell object
   pszURL: Pointer to the URL

Return Value:

  SUCCESS: Applet created and started
  EALREADY: Another applet is already starting
  EBADCLASS: Shell class supplied invalid
  ENOMEMORY: Insufficient memory
  ECLASSNOTSUPPORT: Class specified is not supported
  EEXPIRED: Applet has expired

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   ISHELL_StartApplet
   ISHELL_StartAppletEx
   ISHELL_BrowseFile

=======================================================================

Function: ISHELL_PostURL()

Description:
   This function posts an asynchronous event, EVT_APP_POST_URL, to the 
   applet associated with the URL's scheme.  BREW will make a copy of 
   the URL before passing it to the application handling the event.
   Unlike ISHELL_BrowseURL, this function will not cause the app to 
   become top-visible. This function works similar to ISHELL_PostEvent(). 
   This means:
   a. If the application is already loaded, the event EVT_APP_POST_URL 
      will be sent to it
   b. If the application is not currently loaded, the app will be loaded 
      and the event EVT_APP_POST_URL will be sent to it.  The application 
      will not be automatically made top-visible.  On receiving this event, 
      if the app desires to become top-visible, it needs to call 
      ISHELL_StartApplet() on itself.  On receiving this event, if the app 
      desires to stay in the background even after handling this event, it 
      needs to call ISHELL_StartBackgroundApplet on itself.

Prototype:
   boolean ISHELL_PostURL(IShell *po, const char * pszURL);

Parameters:
   po: Pointer to the IShell object
   pszURL: Pointer to the URL

Return Value:
    TRUE: if the event was posted. The return status only indicates whether the
event was successfully placed in the queue. It does not indicate that the event
was successfully sent to the receiving applet.
    FALSE: if the event was not posted.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   ISHELL_PostEvent

=======================================================================

Function: ISHELL_SendURL()

Description:
   This function posts a synchronous event, EVT_APP_POST_URL, to the 
   applet associated with the URL's scheme.  BREW will make a copy of 
   the URL before passing it to the application handling the event.
   Unlike ISHELL_BrowseURL, this function will not cause the app to 
   become top-visible. This function works similar to ISHELL_SendEvent(). 
   This means:
   a. If the application is already loaded, the event EVT_APP_POST_URL 
      will be sent to it
   b. If the application is not currently loaded, the app will be loaded 
      and the event EVT_APP_POST_URL will be sent to it.  The application 
      will not be automatically made top-visible.  On receiving this event, 
      if the app desires to become top-visible, it needs to call 
      ISHELL_StartApplet() on itself.  On receiving this event, if the app 
      desires to stay in the background even after handling this event, it 
      needs to call ISHELL_StartBackgroundApplet on itself.

Prototype:
   boolean ISHELL_SendURL(IShell *po, const char * pszURL);

Parameters:
   po: Pointer to the IShell object
   pszURL: Pointer to the URL

Return Value:
    TRUE: if the event was processed.
    FALSE: if the event was not posted.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   ISHELL_SendEvent

=======================================================================

ISHELL_CloseApplet()

Description:
   This function instructs the AEE Shell to close the active applet. The applet
   is sent the EVT_APP_STOP message and IAPPLET_Release() is called. The bReturnToIdle
   parameter indicates whether the AEE must close all other active applications and
   return the idle screen.

Prototype:
    int ISHELL_CloseApplet(IShell * pIShell, boolean bReturnToIdle)

Parameters:
    pIShell : Pointer to the IShell Interface object.
    bReturnToIdle : This parameter indicates whether BREW must close
                    all active applications and return to the idle screen.
                    If FALSE, only the currently running applet is closed.
                    If TRUE, all applications having a history entry are closed 
                    (background applications don't have history entries). 
                    PL_SYSTEM is required to call this function with bReturnToIdle 
                    set to TRUE.

Return Value:
    SUCCESS: operation was successful.
    EFAILED: operation failed.
    EPRIVLEVEL: if caller does not have sufficient priv level to call with bReturnToIdle
                set to TRUE
Comments:
    None.

Version:
    Introduced BREW Client 1.0

See Also:
    ISHELL_StartApplet()
    ISHELL_CanStartApplet()
    ISHELL_ActiveApplet()

=======================================================================

ISHELL_CanStartApplet()

Description:
   This function queries the AEE Shell to determine whether it is safe to start
   an applet. Under normal conditions, this call returns TRUE. However, in some cases,
   new applet startup may be prevented when critical dialogs are displayed or other
   applets are running. This function does NOT send EVT_APP_NO_CLOSE event to the
   top visible app.

Prototype:
    boolean ISHELL_CanStartApplet(IShell * pIShell, AEECLSID cls)

Parameters:
    pIShell : Pointer to the IShell Interface object.
    cls : ClassID of the applet that needs to be checked.

Return Value:
    TRUE: if it is possible to start the applet.
    FALSE: if it is not possible to start the applet.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    AEECLSID List
    ISHELL_StartApplet()
    ISHELL_CloseApplet()
    ISHELL_ActiveApplet()

=======================================================================

ISHELL_ActiveApplet()

Description:
    This function returns the AEECLSID associated with the currently top visible applet.

Prototype:
    AEECLSID ISHELL_ActiveApplet(IShell * pIShell)

Parameters:
    pIShell :  Pointer to the IShell Interface object.

Return Value:
    applet ID of the top visible applet, if applets are running.

    0 (zero), if no applet is running.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    AEECLSID List
    ISHELL_StartApplet()
    ISHELL_CloseApplet()
    ISHELL_CanStartApplet()

=======================================================================

ISHELL_EnumAppletInit()

Description:
    This function resets the AEE Shell’s internal applet enumeration index.

Prototype:
    void ISHELL_EnumAppletInit(IShell * pIShell)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.

Return Value:
    None

Comments:
    This call is used in conjunction with ISHELL_EnumNextApplet(), as shown in the
    following example:
===pre>

    ISHELL_EnumAppletInit(pShell);

    while(ISHELL_EnumNextApplet(pShell, &ai)){process...}

===/pre>
Version:
    Introduced BREW Client 1.0

See Also:
    ISHELL_EnumNextApplet()


=======================================================================

ISHELL_EnumNextApplet()

Description:
    This function returns information about the next applet.

Prototype:
    AEECLSID ISHELL_EnumNextApplet(IShell * pIShell, AEEAppInfo * pai)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    pai  :  Pointer to the AEEAppInfo structure to fill.

Return Value:
    ClassID of the next applet, if successful.

    0 (zero), when there are no more applets to enumerate or if pai is NULL.

Comments:
    This call is used in conjunction with ISHELL_EnumAppletInit(), as shown in the
    following example:
    ISHELL_EnumAppletInit(pShell);
    while(ISHELL_EnumNextApplet(pShell, &ai)){process...};

Version:
    Introduced BREW Client 1.0

See Also:
    AEECLSID List
    AEEAppInfo
    ISHELL_EnumAppletInit()

=======================================================================

ISHELL_SetTimer()

Description:
   This method allows the caller to set a short-term timer. Upon expiration, the
   specified callback function is called, passing it the specified user data pointer
   as its first argument. Note the following:
     -    Timer will expire at Current Time + <Milliseconds specified>
     -    Timer callbacks are made in the application's task state. The system shields
          the application developer from managing non-task callbacks.
     -    Any normal processing can be done in the callback. This includes drawing
          to the screen, writing to files, etc.
     -    Timers do not repeat. The user must reset the timer if they desire a repeating
          timer.
     -    Specifying the same callback/data pointers will automatically override
          a pending timer with the same callback/data pointers.
     -    Upon termination of the currently active applet, the shell scans the timer
          list. If the terminated applet was deleted as a result of its termination (ie.
          its reference count went to 0), and an associated timer was found with the data
          pointer pointing to the IApplet, the timer is deleted.
     -    Negative timeout values are treated the same as 0 timeout values

Prototype:
    int ISHELL_SetTimer
    (
    IShell * pIShell,
    int32 dwMSecs,
    PFNNOTIFY pfn,
    void * pUser
    )

Parameters:
    pIShell : Pointer to the IShell Interface object.
    dwMSecs : Timer expiration in milliseconds. The expiration occurs at Current Time +dwMSecs.  Negative values are treated as 0.
    pfn : The user callback that is called when the timer expires.
    pUser : The user data pointer that is passed as the only parameter to the callback.

Return Value:
    SUCCESS: timer successfully set.
    EBADPARM: invalid parameter.
    ENOMEMORY: not enough memory left on heap to create timer.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    PFNNOTIFY
    ISHELL_GetTimerExpiration()
    ISHELL_CancelTimer()

=============================================================================

ISHELL_SetTimerEx()

Description:
    This method allows the caller to set a short-term timer. Upon expiration, the
specified callback function, in AEECallback, is called with the user data (also
specified in AEECallback).
    Note: The timer will expire at Current Time + <Milliseconds specified> Timer
callbacks are made in the application's task state. The system shields the application
developer from managing non-task callbacks.
    Any normal processing can be done in the callback. This includes drawing to
the screen, writing to files, etc.
    Timers do not repeat. The user must reset the timer if they desire a repeating
timer.
    Specifying the same callback/data pointers will automatically override a pending
timer with the same callback/data pointers.
    Upon termination of the currently active applet, the shell scans the timer list.
If the terminated applet was deleted as a result of its termination. For example,
its reference count went to 0), and an associated timer was found with the data
pointer pointing to the IApplet, the timer is deleted.
    Negative timeout values are treated the same as 0 timeout values

Prototype:
    int ISHELL_SetTimerEx
    (
    IShell * pIShell,
    int32 dwMSecs,
    AEECallback * pcb
    );

Parameters:
    pIShell :  Pointer to the IShell object
    dwMSecs :  Timer expiration in milliseconds. The expiration occurs at Current Time +dwMSecs.  Negative values are treated as 0.
    pcb :  AEECallback structure allocated by user

Return Value:
    SUCCESS: timer successfully set.
    EBADPARM: invalid parameter.
    ENOMEMORY: not enough memory left on heap to create timer.

Comments:
    Preferably, use this method rather than ISHELL_SetTimer() and ISHELL_CancelTimer().
    You cancel the timer by canceling the callback: CALLBACK_Cancel(pcb).

Version:
    Introduced BREW Client 2.0

See Also:
    ISHELL_SetTimer()
    ISHELL_CancelTimer()

=======================================================================

ISHELL_CancelTimer()

Description:
   This function cancels a timer that has been set by ISHELL_SetTimer(). If pfn
   is non-NULL, the timer associated with pfn and pUser is canceled. If pfn is NULL,
   all timers associated with pUser are canceled.

Prototype:
    int ISHELL_CancelTimer(IShell * pIShell, PFNNOTIFY pfn, void * pUser)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    pfn  :  User callback.
    pUser  :  User data.

Return Value:
    SUCCESS: always.

Comments:
    Attempting to cancel timers that have not been set is harmless.

Version:
    Introduced BREW Client 1.0

See Also:
    PFNNOTIFY
    ISHELL_SetTimer()
    ISHELL_GetTimerExpiration()

=======================================================================

ISHELL_GetTimerExpiration()

Description:
    This function returns the remaining time in milliseconds before the timer associated
    with the device user callback/data expires.

Prototype:
    uint32 ISHELL_GetTimerExpiration
    (
    IShell * pIShell,
    PFNNOTIFY pfn,
    void * pUser
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    pfn  :  User callback.
    pUser  :  User data.

Return Value:
    Time, if a timer is associated with the specified device user callback/data
    and it hasn’t expired, this function returns the remaining expiration time.

    0 (zero), if otherwise.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    PFNNOTIFY
    ISHELL_SetTimer()
    ISHELL_CancelTimer()

=======================================================================

ISHELL_GetTimeMS()

Description:
   This function returns the current time of day in milliseconds. The value returned
   depends on the device’s current time of day value on BREW-enabled mobile devices
   which is obtained from the base station. This value may change dramatically when
   the device first acquires system coverage.

Prototype:
    uint32 ISHELL_GetTimeMS(IShell * pIShell)

Parameters:
    pIShell : Pointer to the IShell Interface object.

Return Value:
    Millisecond elapsed since midnight.

Comments:
    None

Version:
    Introduced BREW Client 1.1

See Also:
    ISHELL_GetUpTimeMS()
    ISHELL_GetJulianDate()
    ISHELL_GetSeconds()

=======================================================================

ISHELL_GetUpTimeMS()

Description:
   This function returns the millisecond elapsed since the time the device was
   powered on. Unlike ISHELL_GetTimeMS(), this value does not change dramatically because
   of initial acquisition of system coverage.

Prototype:
    uint32 ISHELL_GetUpTimeMS(IShell * pIShell)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.

Return Value:
    Milliseconds elapsed since the time the device was powered on.

Comments:
    None

Version:
    Introduced BREW Client 1.1

See Also:
    ISHELL_GetTimeMS()
    ISHELL_GetSeconds()
    ISHELL_GetJulianDate()

=======================================================================

ISHELL_GetSeconds()
    NOTE: This function is deprecated. Use the helper function, GETTIMESECONDS(),
instead.

Description:
    This function returns the number of seconds, adjusted for local time, since
    January 6, 1980 00:00:00 GMT.

Prototype:
    uint32 ISHELL_GetSeconds(IShell * pIShell)

Parameters:
    pIShell :  Pointer to the IShell Interface object.

Return Value:
    Seconds elapsed since January 6, 1980 00:00:00 GMT.

Version:
    Introduced BREW Client 1.1
	Deprecated BREW Client 2.1
Comments:
    None

See Also:
    ISHELL_GetTimeMS()
    ISHELL_GetJulianDate()
    ISHELL_GetUpTimeMS()

=======================================================================

ISHELL_GetJulianDate()

Description:
   This function fills a JulianType data structure based on the input second value.
   This value represents the number of seconds since January 6 1980 00:00:00 GMT, the
   device reference time. If the input value is 0 (zero), the current system time is
   returned.

Prototype:
    void ISHELL_GetJulianDate
    (
    IShell * pIShell,
    uint32 dwSecs,
    JulianType * pDate
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    dwSecs  :  Seconds since January 6 1980, the device reference time.
    pDate  :  Pointer to the structure that needs to be filled on return.

Return Value:
    None

Comments:
    In Windows, the upper limit for dwSecs is the difference between the maximum
value allowed by the uint32, and the difference between device reference time January
6, 1980 00:00:00 GMT and PC reference time January 1, 1970 00:00:00 GMT.

Version:
    Introduced BREW Client 1.1

See Also:
    JulianType
    GETJULIANDATE()
    ISHELL_GetSeconds()
    ISHELL_GetTimeMS()
    ISHELL_GetUpTimeMS()
    GET_SECONDS()
    GET_TIMEMS()
    ISHELL_GetUpTimeMS()
    GETTIMESECONDS()
    GETTIMEMS()

=======================================================================

ISHELL_CreateDialog()

Description:
   This function instructs the AEE Shell to start a dialog associate with the input
   DialogInfo data structure or the dialog information in the associated resource file.
   This call causes the AEE Shell to create a IDialog Interface. The interface processes
   the dialog input parameters to create all associated controls and other items. The
   dialog is placed at the top of the active dialog stack.

   During creation, execution, and termination, the IDialog Interface sends a number
   of dialog-related AEEEvent items (such as EVT_DIALOG_INIT, EVT_DIALOG_START, and EVT_DIALOG_END)
   to the active applet. This allows the applet to control the initial contents of
   controls, manage control changes, and retrieve the contents of controls at termination.
   The applet can obtain pointers to the underlying dialog controls by calling IDIALOG_GetControl().

Prototype:
    int ISHELL_CreateDialog
    (
    IShell * pIShell,
    const char * pszResFile,
    uint16 wID,
    DialogInfo * pInfo
    )

Parameters:
    pIShell : Pointer to the IShell Interface object.
    pszResFile : Pointer to the resource file containing the dialog information.
    wID : ID of the dialog inside the resource file.
    pInfo : Dialog information structure. 

Return Value:
    SUCCESS: if successful.
    ENOMEMORY: insufficient memory.
    EBADPARM: invalid parameter.
    EFAILED: if unsuccessful.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    DialogInfo
    ISHELL_GetActiveDialog()
    ISHELL_EndDialog()
    IDIALOG_SetFocus()
    IDIALOG_GetControl()
    EVT_DIALOG_INIT
    EVT_DIALOG_START
    EVT_DIALOG_END

=======================================================================

ISHELL_GetActiveDialog()

Description:
    This function returns the current active dialog. It is provided so that applets
    can query the dialog for underlying controls and other items.

Prototype:
    IDialog * ISHELL_GetActiveDialog(IShell * pIShell)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.

Return Value:
    Pointer to the currently active dialog, if successful.
    NULL: if there is no active dialog or if unsuccessful.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    ISHELL_CreateDialog()
    ISHELL_EndDialog()
    IDIALOG_SetFocus()
    IDIALOG_GetControl()

=======================================================================

ISHELL_EndDialog()

Description:
    This function closes the currently active dialog.

Prototype:
    ISHELL_EndDialog(IShell * pIShell)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.

Return Value:
    SUCCESS: active dialog successfully closes.
    EFAILED: no dialog is active.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    ISHELL_CreateDialog()
    ISHELL_GetActiveDialog()
    IDIALOG_SetFocus()
    IDIALOG_GetControl()

=======================================================================

ISHELL_LoadResString()

Description:
   This function allows the caller to retrieve UNICODE (UCS2 encoding) or ISOLATIN strings stored
   in the specified resource file. The returned string is placed in the buffer provided.

Prototype:
    int ISHELL_LoadResString
    (
    IShell * pIShell,
    const char * pszResFile,
    int16 nResID,
    AECHAR * pBuff,
    int nSize
    )

Parameters:
    pIShell : [in] : Pointer to the IShell Interface object.
    pszResFile : [in] : Resource file containing the string.
    nResID : [in] : ID of the string in the resource file.
    pBuff : [out] : Buffer to fill with the string.
    nSize : [in] :  Size in bytes of the input buffer.

Return Value:
    Number of characters filled, if successful.

    0 (zero), if otherwise.

Comments:
   For KSC5601, S_JIS and EUC_CN encoding LoadResString will fail to 
   correctly load the string unless the encoding is also the handsets
   native encoding.

Version:
    Introduced BREW Client 1.0

See Also:
    AEE_RES_LANGSUF
    AEE_RES_LANGDIR
    ISHELL_LoadImage()
    ISHELL_LoadResData()
    ISHELL_LoadResImage()
    ISHELL_LoadResObject()
    ISHELL_LoadResSound()
    ISHELL_LoadSound()

=======================================================================

ISHELL_LoadResObject()

Description:
    This function loads the specified resource and creates a handler that can be
used on the resource data. The type of handler created is indicated by hType.

Prototype:
    IBase * ISHELL_LoadResObject
    (
    IShell * pIShell,
    const char * pszResFile,
    int16 nResID,
    AEEHandlerType hType
    )

Parameters:
    pIShell : Pointer to the IShell Interface object.
    pszResFile : Resource file containing the specified resource.
    nResID : ID of the resource in the resource file.
    hType : Type of handler that must be created and associated with the resource data.

    The interface pointer returned from this function depends on the specified handler
    type. The AEEHandlerType must coincide with the actual type of the resource data.


Return Value:
    Pointer to handle the resource data, if successful.
    NULL: if unsuccessful.

Comments:
    For example:
    If the resource data is of type HTYPE_SOUND and if hType parameter is set to
HTYPE_VIEWER, the behavior of this function is unpredictable.
    If hType is HTYPE_VIEWER, it indicates that the resource data is a bitmap resource.
This function creates an IImage Interface and associates the resource data with
it. The IImage Interface pointer is then returned from this function. This interface
pointer can be used to view the image.
    If hType is HTYPE_SOUND, it indicates that the resource data is a sound stream.
In this case, this function creates and returns a ISoundPlayer Interface pointer
that can then be used to play the sound.
    ISHELL_LoadResImage() and ISHELL_LoadResSound() are specific usages of this
function with the hType set to HTYPE_VIEWER and HTYPE_SOUND, respectively.

Version:
    Introduced BREW Client 1.0

See Also:
    AEE_RES_LANGSUF
    AEE_RES_LANGDIR
    ISHELL_RegisterHandler()
    ISHELL_LoadImage()
    ISHELL_LoadResData()
    ISHELL_LoadResImage()
    ISHELL_LoadResSound()
    ISHELL_LoadResString()
    ISHELL_LoadSound()
    ISHELL_FreeResData()

=======================================================================
ISHELL_LoadResData()

Description:
   This function returns a void * data block associated with the specified resource
file, ID, and type. The memory returned must be freed using ISHELL_FreeResData().

Prototype:
    void * ISHELL_LoadResData
    (
    IShell * pIShell,
    const char * pszResFile,
    int16 nResID,
    ResType nType
    )

Parameters:
    pIShell : Pointer to the IShell Interface object.
    pszResFile : Resource file containing the data.
    nResID : ID of the data in the resource file.
    nType : The type of resource:~
~
            RESTYPE_IMAGE, ~
            RESTYPE_DIALOG,~
            RESTYPE_CONTROL, ~
            RESTYPE_ LISTITEM, (currently not supported)~
            RESTYPE_BINARY, ~
*
    If nType is RESTYPE_IMAGE, the first byte indicates the offset where the
    actual image data begins, and the second byte is 0 (zero). Starting from
    the third byte, the string indicates the MIME type followed by the actual
    image data.

Return Value:
    A void pointer, pointing to the resource data, if successful.
    NULL: if otherwise.

Comments:
    This function loads the *raw* contents of the resource.  Any parsing
    or interpretation of the data is up to the application to perform.

Side Effects:
   None

Version:
    Introduced BREW Client 1.0

See Also:
   AEE_RES_LANGSUF
   AEE_RES_LANGDIR
   ISHELL_LoadResString
   ISHELL_FreeResData

=======================================================================

ISHELL_LoadResDataEx()

Description:
    This function attempts to load the resource specified by type and ID from the
    specified resource context. The function either allocates memory or loads into a
    pre-allocated buffer. If the function allocates memory, it must be freed with
    ISHELL_FreeResData().

Prototype:
    void *ISHELL_LoadResDataEx
    (
    IShell * pIShell,
    const char * pszResFile,
    uint16 nResID,
    ResType nType,
    void * pBuf,
    uint32 * pnBufSize
    )

Parameters:
    pIShell :  Pointer to the IShell object.
    pszResFile :  Resource file containing the data.
    nResID :  ID of the data in the resource file.
    nType :  The type of resource:~
~
             RESTYPE_STRING, ~
             RESTYPE_IMAGE, ~
             RESTYPE_DIALOG, ~
             RESTYPE_CONTROL, ~
             RESTYPE_LISTITEM, (currently not supported)~
             RESTYPE_BINARY, ~
*
    pBuf :  Buffer into which to load the resource data.
            If pBuf is NULL, a buffer is allocated to hold the loaded resource, and must
            be freed with ISHELL_FreeResData().
            If pBuf is -1, no buffer is allocated, but size information is calculated and
            returned in pnBufSize.
    pnBufSize :  Cannot be NULL.
               [in] if pBuf is not NULL or -1. INPUT Points to the sizeof(pBuf) in bytes.
					If pBuf is NULL or -1, pnBufSize is ignored on INPUT.
			   [out] Contains the size of the resource data read.
					If pBuf was not NULL on INPUT and *pnBufSize was too small to hold
					the resource, the function returns NULL. If pBuf was -1, pnBufSize 
					is filled with the actual resource size, the function returns -1 (0xffffffff).

Return Value:
    void pointer to the resource data, if successful.
    NULL: if loading resource fails or buffer size is too small to hold the resource.

Comments:
    This function loads the *raw* contents of the resource. Any parsing or interpretation
of the data is up to the application to perform.
    If loading a string resource, the contents of pBuf (and the returned void *)
is the bytes of the string without NULL termation.
    If loading an image resource, the contents of pBuf is an AEEResBlob structure,
whose raw data lies at RESBLOB_DATA(pBuf).

Version:
    Introduced BREW Client 1.1

See Also:
    AEE_RES_LANGSUF
    AEE_RES_LANGDIR
    ISHELL_FreeResData()
    AEEResBlob

=======================================================================

ISHELL_GetResSize()

Description:
    This function is a wrapper for ISHELL_LoadResDataEx(). It evaluates to the size
of the found resource, or 0 if the resource is not found.

Prototype:
    uint32 ISHELL_GetResSize
    (
    IShell * pIShell,
    const char * pszResFile,
    uint16 nResID,
    ResType nType,
    uint32 *pnBufSize
    );

Parameters:
    pIShell :  Pointer to the IShell object
    pszResFile :  Resource file containing the data
    nResID :  ID of the data in the resource file
    nType :  The type of resource~
~
             RESTYPE_STRING~
             RESTYPE_IMAGE~
             RESTYPE_DIALOG~
             RESTYPE_CONTROL~
             RESTYPE_LISTITEM (currently not supported)~
             RESTYPE_BINARY ~
*

    pnBufSize :  May not be null

Return Value:
    If failed, returns 0

    If successful, returns size of the resource.

Comments:
    None

Version:
    Introduced BREW Client 2.0

See Also:
    AEE_RES_LANGSUF
    AEE_RES_LANGDIR
    ISHELL_LoadResDataEx()

=======================================================================

ISHELL_LoadResImage()

Description:
    This function loads a bitmap resource from the given resource file and returns
a valid IImage Interface pointer. This pointer can then be used for viewing the
image.

Prototype:
    IImage * ISHELL_LoadResImage
    (
    IShell * pIShell,
    const char * pszResFile,
    int16 nResID
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    pszResFile :  Resource file containing the bitmap image.
    nResID  :  ID of the bitmap in the resource file.

Return Value:
    pointer to an IImage Interface that can be used for viewing the image, if succesful.
    NULL: if unsuccessful.

Comments:
    The image may not be fully decoded at the time that ISHELL_LoadResImage()
    returns.  You should register for a notification of decoding completion
    with IIMAGE_Notify(), and not use the IImage until you have received the
    notification.

    This function uses ISHELL_LoadResObject.

Version:
    Introduced BREW Client 1.0

See Also:
    AEE_RES_LANGSUF
    AEE_RES_LANGDIR
    IImage
    ISHELL_LoadImage()
    ISHELL_LoadResData()
    ISHELL_LoadResObject()
    ISHELL_LoadResSound()
    ISHELL_LoadResString()
    ISHELL_LoadSound()
    ISHELL_FreeResData()

=======================================================================

ISHELL_LoadImage()

Description:
    This function loads an image directly from a file and returns a pointer to an
IImage handler that can be used to display the image.

Prototype:
    IImage * ISHELL_LoadImage(IShell * pIShell, const char * pszImageFile)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    pszImageFile  :  Image file.

Return Value:
    Pointer to an IImage Interface that can be used for viewing the image.

Comments:
    The image may not be fully decoded at the time that ISHELL_LoadImage()
    returns.  You should register for a notification of decoding completion
    with IIMAGE_Notify(), and not use the IImage until you have received the
    notification.

    This function uses ISHELL_LoadResObject.

Version:
    Introduced BREW Client 1.0

See Also:
    AEE_RES_LANGSUF
    AEE_RES_LANGDIR
    IImage
    ISHELL_LoadResData()
    ISHELL_LoadResImage()
    ISHELL_LoadResObject()
    ISHELL_LoadResSound()
    ISHELL_LoadResString()
    ISHELL_LoadSound()
    ISHELL_FreeResData()

=============================================================================

ISHELL_LoadBusyImage()

Description:
    This method loads the image used for indicating that BREW is busy. This is the
hour-glass image. Applications can use this image to be displayed on the screen
while they are busy performing an operation.

Prototype:
    IImage *IShell_LoadBusyImage(IShell *pIShell)

Parameters:
    pIShell :  Pointer to the IShell object

Return Value:
    IImage Pointer, if successful.
    NULL: if failed.

Comments:
    None

Version:
     Introduced BREW Client 2.0

See Also:
    None

=======================================================================
ISHELL_LoadResSound()

Description:
    This function can be used when the sound data is included in a resource file
as a raw stream of bytes. The function loads a raw sound buffer from the given resource
file, creates an ISoundPlayer Interface pointer, and sets the sound data to this
interface by using ISOUNDPLAYER_Set() with the AEESoundPlayerInput data parameter
set to SDT_BUFFER. The ISoundPlayer Interface pointer can then be used for playing
the sound.

    NOTE: This function reads sound resources from a BREW resource file. Because
the BREW Resource Editor does not support sound resources in the current version
of BREW, this function is not supported.

Prototype:
    ISoundPlayer * ISHELL_LoadResSound
    (
    IShell * pIShell,
    const char * pszResFile,
    int16 nResID
    )

Parameters:
    pIShell :  Pointer to the IShell Interface object.
    pszResFile  :  Resource file containing the raw sound data.
    nResID  :  ID of the raw sound data.

Return Value:
    Pointer to an ISoundPlayer Interface used for playing the sound, if successful.
    NULL: if unsuccessful.

Comments:
    This function uses ISHELL_LoadResObject().

Version:
    Introduced BREW Client 1.0

See Also:
    AEE_RES_LANGSUF
    AEE_RES_LANGDIR
    ISHELL_LoadImage()
    ISHELL_LoadResData()
    ISHELL_LoadResImage()
    ISHELL_LoadResObject()
    ISHELL_LoadResString()
    ISHELL_LoadSound()

=======================================================================
ISHELL_LoadSound()

Description:
    This function loads a sound file from the file system and returns the ISoundPlayer
Interface object for the file.

Prototype:
    ISoundPlayer * ISHELL_LoadSound
    (
    IShell * pIShell,
    const char * pszResFile
    )

Parameters:
    pIShell :  Pointer to the IShell Interface object.
    pszResFile  :  Sound file.

Return Value:
    Pointer to an ISoundPlayer Interface that can be used for playing the sound,
if successful.
    NULL: if otherwise.

Comments:
    This function uses ISHELL_LoadResObject().

Version:
    Introduced BREW Client 1.0

See Also:
    AEE_RES_LANGSUF
    AEE_RES_LANGDIR
    ISHELL_LoadImage()
    ISHELL_LoadResData()
    ISHELL_LoadResImage()
    ISHELL_LoadResObject()
    ISHELL_LoadResSound()
    ISHELL_LoadResString()
    ISHELL_FreeResData()

=======================================================================

ISHELL_LoadResBitmap()

Description:
    This function loads a bitmap resource from the given resource file and returns
a valid IBitmap interface pointer.

Prototype:
    ISHELL_LoadResBitmap(p,psz,id)

Parameters:
    PIshell  :  Pointer to Ishell interface object
    psz  :  Resource file contain bitmap
    ID  :  ID of bitmap in resource file

Return Value
    A pointer to an IBitmap interface (image), If successful.
    NULL: If it fails.

Comments:
    This is a MACRO around ISHELL_LoadResObject().

Version:
    Introduced BREW Client 2.0

See Also:
    AEE_RES_LANGSUF
    AEE_RES_LANGDIR
    ISHELL_LoadBitmap()

=======================================================================

ISHELL_LoadBitmap()

Description:
    This function loads an bitmap file directly and returns the IBitmap * handler
for the bitmap.

Prototype:
    IBitmap * ISHELL_LoadBitmap(IShell * pIShell, const char * pszFile)

Parameters:
    pIShell :  Pointer to the IShell Interface object
    pszFile :  Image file

Return Value:
    Pointer to an IBitmap interface.

Comments:
    This is a MACRO

Version:
    Introduced BREW Client 2.0

See Also:
    AEE_RES_LANGSUF
    AEE_RES_LANGDIR
    ISHELL_LoadResBitmap()

=======================================================================

ISHELL_FreeResData()

Description:
    This function frees the data previously returned by ISHELL_LoadResData().

Prototype:
    void ISHELL_FreeResData(IShell * pIShell, void * pData)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    pData  :  Resource Data that is to be freed must have been returned by a prior
call to ISHELL_LoadResData().

Return Value:
    None

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    ISHELL_LoadResData()

=======================================================================

ISHELL_SendEvent()

Description:
    The entire execution model of the AEE is based on a semi-cooperative
    event-passing model. Under this model, events are sent to the active
    dialog or applet using ISHELL_SendEvent(), which allows for control
    over the destination applet. This function sends the event directly
    to the destination applet. Events can be sent to a specific applet by
    specifying the destination applet. If the applet is not currently
    running, it is loaded and the event is sent directly to it. Under
    these conditions, the applet is started in the background (EVT_APP_START
    and EVT_APP_STOP events are not sent to the applet). Private events can
    be sent to an applet by defining the event at or above EVT_USER, and
    specifying the applet ClassID. An error is returned if events in the
    range of EVT_USER and above are sent without an associated ClassID.
    If a dialog is active through ISHELL_CreateDialog(), the event is
    passed to the dialog before being passed to the applet. The event is
    passed to the applet only if the dialog did not process the event.
    The wp and dwp parameters associated with the event are specific to the
    event. The AEE Shell does not examine these values.

Prototype:
    boolean ISHELL_SendEvent
    (
    IShell * pIShell,
    AEECLSID clsApp,
    AEEEvent evt,
    uint16 wp,
    uint32 dwp
    )

Parameters:
    pIShell : Pointer to the IShell Interface object.
    clsApp :  Class of the applet for the event. This parameter is required for
               events in the range of EVT_USER and above.
    evt : AEEEvent items.
    wp : Event-specific 16-bit value.
    dwp : Event-specific 32-bit value.

Return Value:
    TRUE: if the event was processed.
    FALSE: if the event was not processed.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
        AEEEvent
    ISHELL_PostEvent()

=======================================================================

ISHELL_PostEvent()

Description:
    This function posts an asynchronous event to the specified applet.
    The function is similar to ISHELL_SendEvent(). The main difference is
    that this function posts the event to the applet, and ISHELL_SendEvent()
    immediately sends the event to the applet. Event posting is provided for
    special cases where the caller either posts an event from another task or
    defers the processing of the event until the next iteration of the event
    loop. This is useful in providing continued execution while allowing other
    events to be processed. Private events can be sent to an applet by defining
    the event at or above EVT_USER level, and specifying the applet ClassID. An
    error is returned if events in the range of EVT_USER and above are sent
    without an associated ClassID.

Prototype:
    boolean ISHELL_PostEvent
    (
    IShell * pIShell,
    AEECLSID clsApp,
    AEEEvent evt,
    uint16 wp,
    uint32 dwp
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    clsApp  :  ClassID of the applet for the event. This parameter is required and
               must denote an applet. If the clsApp specified here does not belong
               to an applet, this function may still return TRUE, but the actual
               sending of the event fails when the event is popped out of the queue
               and an attempt is made to send to the applet specified by clsApp.
    evt  :  AEEEvent items.
    wp  :  Event-specific 16-bit value.
    dwp  :  Event-specific 32-bit value.

Return Value:
    TRUE: if the event was posted. The return status only indicates whether the
event was successfully placed in the queue. It does not indicate that the event
was successfully sent to the receiving applet.
    FALSE: if the event was not posted.

Comments:
    This function doesn't allow multiple events of the same type (same event code) 
    to be posted and pending to be dispatched for the same app. Aforementioned
    capability is provided by ISHELL_PostEventEx().
    
    On application closure after delivery of EVT_APP_STOP, events pending for posting
    to the application are canceled and thus such events are not delivered to the
    application.

Version:
    Introduced BREW Client 1.0

See Also:
    AEECLSID List
    AEEEvent
    ISHELL_SendEvent()

=======================================================================
ISHELL_PostEventEx()

Description:
    This function posts an asynchronous event to the specified app. This function
is very similar to ISHELL_PostEvent(). The main difference is that this function
allows multiple events of the same type (same event code) to be posted and pending
to be dispatched for the same app. Private events can be sent to an applet by defining
the event at or above EVT_USER and specifying the applet class. An error is returned
if events in the range of EVT_USER and above are sent without an associated class
ID.

Prototype:
    boolean ISHELL_PostEventEx
    (IShell * pIShell,
    uint16 wFlags,
    AEECLSID clsApp,
    AEEEvent evt,
    uint16 wp,
    uint32 dwp
    );

Parameters:
    pIShell :  Pointer to the IShell object
    wFlags :  Flags to specify how the event must be posted. If no flags are used,
              this parameter is set to 0. The typical usage is to set this value to 0.
              The supported flags are
               EVTFLG_UNIQUE - This flag indicates that only one of this event type can
               be pending at any time. If this flag is set, there cannot be multiple
               events of the same event codepending for an app. To allow posting of
               multiple events of the same event code,this flag must not be set.
               Setting this flag causes this function to work justlike ISHELL_PostEvent().
    clsApp :  Class of the applet for the event. This parameter is required for
              events in the range of EVT_USER and above. This must denote an applet.
              It cannot be an ordinary class.
    evt :  Event type (EVT_XXXX)
    wp :  Event-specific 16-bit value
    dwp :  Event-specific 32-bit value

Return Value:
    TRUE: if the event was posted. The return status only indicates whether or not
the eventwas successfully placed in the queue. It does not indicate that the event
wassuccessfully sent to the receiving classID. If the clsID specified here does
not belongto an applet, this function may still return TRUE but the actual sending
of the eventwill fail when the event is popped out of the queue and an attempt is
made to send to thegiven classID.
    FALSE: if the event was not posted

Comments:
    On application closure after delivery of EVT_APP_STOP, events pending for posting
    to the application are canceled and thus such events are not delivered to the
    application.

Version:
    Introduced BREW Client 2.1

See Also:
    AEEEvent
    ISHELL_PostEvent()
    ISHELL_SendEvent()

=======================================================================
ISHELL_HandleEvent()

Description:
    This function sends one of the specified standard AEEEvent items to the currently
active applet. It internally invokes ISHELL_SendEvent() with ClassID set to 0 (zero).
This function can only send events in the range: EVT_COMMAND to EVT_USER. It cannot
send events equal to or above EVT_USER. This function is meant to be used only by
user-implemented controls. For example, if a control wants to send the event EVT_CTL_TAB
to the currently active application, it can use this function. Applications must
generally use the function ISHELL_SendEvent() or ISHELL_PostEvent() to send events
to themselves or to other specific applications.

Prototype:
    boolean ISHELL_HandleEvent
    (
    IShell * pIShell,
    AEEEvent evt,
    uint16 wp,
    uint32 dwp
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    evt  :  AEEEvent items.
    wp  :  Event-specific 16-bit value.
    dwp  :  Event-specific 32-bit value.

Return Value:
    TRUE: if the event was processed.
    FALSE: if the event was not processed.

Comments:
    The event sent must be one of the standard AEEEvent items. For example, it must
be < EVT_USER; it cannot be an EVT_USER event.

Use ISHELL_PostEvent() to send user events.

Version:
    Introduced BREW Client 1.0

See Also:
    ISHELL_SendEvent()
    ISHELL_PostEvent()

=======================================================================

ISHELL_Beep()

Description:
    This function provides a simple interface to play system beeps or vibrate the
device.

Prototype:
    boolean ISHELL_Beep(IShell * pIShell, BeepType nBeep, boolean bLoud)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    nBeep  :  Type of beep to play.
    bLoud  :  TRUE if the beep can be played at a higher volume.

Return Value:
    TRUE: if the beep was played.
    FALSE: if the beep was not played.

Comments:
    In the BREW Simulator, users can define their own beep type tones in WAV format.
The beep types correspond to BeepType enumeration, which is required in ISHELL_Beep().
The tones are saved in the <$BREWDIR\bin\DataFiles> directory. For example, to create
your own BEEP_ALERT tone, save your WAV file as BEEP_ALERT.wav.

    BREW SDK is shipped with tone files for all beep types. ISHELL_Beep() is illustrated
in the sample Sound applet.

Version:
    Introduced BREW Client 1.0

See Also:
    BeepType

=======================================================================

ISHELL_GetPrefs()

Description:
    This function provides a method for retrieving a structure containing applet
or class level preferences.

Prototype:
    int ISHELL_GetPrefs
    (
    IShell * pIShell,
    AEECLSID cls,
    uint16 wVer,
    void * pCfg,
    uint16 nSize
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    cls  :  AEECLSID of the preference type.
    wVer  :  Version of the preference.
    pCfg  :  Pointer to the memory to fill with preference data.
    nSize  :  Size of memory block to fill.

Return Value:
    SUCCESS: if operation is successful.
    EFAILED: if operation fails.

    Non-Zero size of the record, if pCfg == NULL or nSize is insufficient


Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    AEECLSID List
    ISHELL_SetPrefs()

=======================================================================

ISHELL_GetAppPrefs()

Description:
    This method provides a means of retrieving a structure containing applet preferences.
Applications can use this means to store/retrieve application-specific settings
or preferences information persistently. The information is stored in a file. Use
ISHELL_SetAppPrefs() to store the preferences.

Prototype:
    int ISHELL_GetAppPrefs
    (
    IShell * pIShell,
    uint16 wVer,
    void * pCfg,
    uint16 nSize
    )

Parameters:
    pIShell :  Pointer to the IShell object
    wVer :  Version of the preference
    pCfg :  Pointer to memory to fill with preference data
    nSize :  Size of memory block to fill

Return Value:
    SUCCESS: if the operation succeeded.

    Non-Zero size of the record, if pCfg == NULL or nSize is insufficient

Comments:
    None

Version:
    Introduced BREW Client 2.0

See Also:
    ISHELL_SetAppPrefs()

=======================================================================

ISHELL_SetPrefs()

Description:
    This function provides a method for storing a structure containing applet or
class level preferences.

Prototype:
    int ISHELL_SetPrefs
    (
    IShell * pIShell,
    AEECLSID cls,
    uint16 wVer,
    void * pCfg,
    uint16 nSize
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    cls  :  AEECLSID of the preference type.
    wVer  :  Version of the preference.
    pCfg  :  Pointer to the preference data to be stored.
    nSize  :  Size of memory block to store.

Return Value:
    SUCCESS: operation successful.
    EBADPARM: invalid parameter.
    ENOMEMORY: insufficient memory.
    EFAILED: Operation failed.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    ISHELL_GetPrefs()

=======================================================================

ISHELL_SetAppPrefs()

Description:
    This method provides a means of stroing a structure containing applet specific
preferences. Applications can use this means to store/retrieve application-specific
settings or preferences information persistently. The information is stored in a
file. Use ISHELL_GetAppPrefs() to retrieve the preferences.

Prototype:
    int ISHELL_SetPrefs
    (
    IShell * pIShell,
    uint16 wVer,
    void * pCfg,
    uint16 nSize
    )

Parameters:
    pIShell :  Pointer to the IShell object
    wVer :  Version of the preference
    pCfg :  Pointer to the preference data to be stored
    nSize :  Size of memory block to store

Return Value:
    SUCCESS: Operation Succeeded

Comments:
    Using different version(wVer) for the same class id would over write the previous
    record if any

Version:
    Introduced BREW Client 2.0

See Also:
    ISHELL_GetAppPrefs()

=======================================================================

ISHELL_Prompt()

Description:
    This function provides a mechanism for an application to display a multi selection
prompt. The text can be specified from a resource file or directly passed in the
AEEPromptInfo data structure. The AEE Shell first examines the text pointers before
attempting to load the text from the resource file. The buttons are specified by
the values in the AEEPromptInfo->pBtnIDs list. For convenience, it is assumed that
the IDs for the buttons are associated with both the text and command ID for the
button. If the passed button pointer is NULL, the prompt must successfully be displayed
without any button. The prompt dialog is automatically dismissed when the device
user selects a button or presses the CLR key. In this case, the EVT_COMMAND message
is sent to the application with the 16-bit extra parameter indicating the ID of
the selection.

Prototype:
    boolean ISHELL_Prompt(IShell * pIShell, AEEPromptInfo * pi)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    pi  :  Pointer to the AEEPromptInfo data structure.

Return Value:
    TRUE: if the prompt is created.
    FALSE: if the prompt is not created.

Comments:
    The first button in the pBtnIDs array is always selected (highlighted) when
the prompt is instantiated.

Version:
    Introduced BREW Client 1.0

See Also:
    AEEPromptInfo

=======================================================================

ISHELL_ShowCopyright()

Description:
    This function allows the application to display the copyright information about
the applet. An applet can use this function to show copyright and other applet related
information on the screen. This information is obtained from the Module Information
File (MIF) for that applet. The following pieces of information can be shown:
     -    Application Name
     -    Application Icon
     -    Copyright String
     -    Company Name
     -    Module Version

    NOTE: The lengths of the strings must be less than 24 characters. This information
appears in the form of a dialog box, and it stays on the screen until the device
user dismisses the dialog by pressing the clear button.

Prototype:
    boolean ISHELL_ShowCopyright(IShell * pIShell)

Parameters:
    pIShell :  Pointer to the IShell Interface object.

Return Value:
    TRUE: if the copyright information was successfully displayed.
    FALSE: if the copyright information was not successfully displayed.

Comments:
    This function internally uses ISHELL_Prompt() for the implementation.

Version:
Introduced BREW Client 1.0

See Also:
    None

=======================================================================

ISHELL_MessageBox()

Description:
    This function instructs the AEE Shell to display a message box to the device
user. A message box is a simple window with a title and text. The device user dismisses
it using the CLR keys. The title and text are retrieved from the specified resource
file.

Prototype:
    boolean ISHELL_MessageBox
    (
    IShell * pIShell,
    const char * pszResFile,
    uint16 wTitleID,
    uint16 wTextID
    )

Parameters:
    pIShell : Pointer to the IShell Interface object.
    pszResFile : Resource file containing the title and text. If this is NULL,
                 the resource ID of the text (such as wTextID) is used to retrieve
                 an error message from the BREW resource file (AEEControls.bar)
                 located in the language directory (for example, "en" for English).
                 This error message is then displayed in the message box.
    wTitleID : ID of the title, which can be a maximum size of 128 bytes (64 UNICODE
               (UCS2 encoding) characters).
    wTextID : ID of the text, which can be a maximum size of 256 bytes (128 UNICODE
              (UCS2 encoding) characters).

Return Value:
    TRUE: if the message box was created.
    FALSE: otherwise.

Comments:
    If either the title or text exceeds the maximum limit, the message box will
    not be created.

Version:
    Introduced BREW Client 1.0

See Also:
    ISHELL_MessageBoxText()

=======================================================================

ISHELL_MessageBoxText()

Description:
    This function instructs the AEE Shell to display a message box to the device
user. A message box is a simple window with a title and text. The device user dismisses
it using the CLR keys. Unlike ISHELL_MessageBox(), this function uses the title
and text strings provided.

Prototype:
    boolean ISHELL_MessageBoxText
    (
    IShell * pIShell,
    const AECHAR * pTitle,
    const AECHAR * pText
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    pTitle  :  Pointer to the title string.
    pText  :  Pointer to the text string.

Return Value:
    TRUE: if successful.
    FALSE: otherwise.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    ISHELL_MessageBox()

=======================================================================

ISHELL_SetAlarm()

Description:
    This function allows the caller to set a long-term alarm for an applet. When
the alarm expires, the applet is loaded and passed an EVT_ALARM event with the specified
16-bit nUserCode as the user parameter. If the applet is not active at the time
of the alarm, it is loaded but is not sent an EVT_APP_START event. If the applet
is to be activated, it must call ISHELL_StartApplet(). More than one alarm can be
set for an applet by specifying a different 16-bit alarm nUserCode.

Prototype:
    int ISHELL_SetAlarm
    (
    IShell * pIShell,
    AEECLSID cls,
    uint16 nUserCode,
    uint32 nMins
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    cls  :  Applet class to call when the alarm expires.
    nUserCode  :  16-bit code passed to the applet.
    nMins  :  Number of minutes to set the alarm from the current time.

Return Value:
    SUCCESS: if successful.
    EBADPARM: invalid parameter.
    ENOMEMORY: insufficient memory.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    ISHELL_CancelAlarm()

=======================================================================

ISHELL_CancelAlarm()

Description:
    This function cancels an alarm set through ISHELL_SetAlarm(). The proper class
and 16-bit user code must be specified.

Prototype:
    int ISHELL_CancelAlarm
    (
    IShell * pIShell,
    AEECLSID cls,
    uint16 nUserCode
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    cls  :  Applet class of alarm to cancel.
    nUserCode  :  16-bit use code of alarm to cancel.

Return Value:
    SUCCESS: if an alarm is found and canceled.
    EFAILED: if an alarm cannot be found or canceled.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    AEECLSID List
    ISHELL_SetAlarm()


=======================================================================

ISHELL_SuspendAlarms()

Description:
    This function allows the caller to suspend operation of the BREW
    alarms subsystem.  This function does not impact applications' ability
    to schedule alarms, it merely postpones any alarm notifications
    until ISHELL_ResumeAlarms() is called, or the phone is reset.

Prototype:
    int ISHELL_SuspendAlarms
    (
    IShell * pIShell
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.

Return Value:
    SUCCESS: if successful.
    EPRIVLEVEL: insufficient privilege (must have AEEGROUPID_ALARM_MGR)
    Other: AEEError if appropriate

Comments:
    None

Version:
    Introduced BREW Client 3.1

See Also:
    ISHELL_ResumeAlarms()
        ISHELL_SetAlarm()
        ISHELL_CancelAlarm()

=======================================================================

ISHELL_ResumeAlarms()

Description:
    This function allows the caller to resume operation the BREW alarms
    subsystem.  Any alarms that have already come due passed will be
    scheduled for notification.

Prototype:
    int ISHELL_ResumeAlarms
    (
    IShell * pIShell
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.

Return Value:
    SUCCESS: if successful.
    EPRIVLEVEL: insufficient privilege (must have AEEGROUPID_ALARM_MGR)
    Other: AEEError if appropriate

Comments:
    None

Version:
    Introduced BREW Client 3.1

See Also:
    ISHELL_SuspendAlarms()
        ISHELL_SetAlarm()
        ISHELL_CancelAlarm()

=======================================================================

ISHELL_AlarmsActive()

Description:
    This function returns TRUE if an alarm is set for the calling
    applet.

Prototype:
    boolean ISHELL_AlarmsActive(IShell * pIShell)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.

Return Value:
    TRUE: if there are pending alarms
    FALSE: if there are no pending alarms

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    None

=======================================================================

ISHELL_GetHandler()

Description:
    This function provides query access to the AEE Shell’s database of registered
content viewers and protocol scheme handlers. This database allows browsers and
content viewers to expose handlers for content or data protocols they may support.
    The AEEHandlerType data structure indicates whether the input string is a viewer
(such as image type) or a sound type.

Prototype:
    AEECLSID ISHELL_GetHandler
    (
    IShell * pIShell,
    AEECLSID clsBase,
    const char * pszIn
    )

Parameters:
    pIShell : Pointer to the IShell Interface object.
    clsBase : Handler type (HTYPE_VIEWER, HTYPE_SOUND) or an AEECLSID base class
              for the handler to meet.  The AEECLSID may be an interface id that supports
              IQueryInterface.
    pszIn : Input string.

Return Value:
    AEECLSID of the associated handler class.

    0 (zero), if otherwise.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    AEECLSID List
    AEEHandlerType
    ISHELL_RegisterHandler()

=======================================================================

ISHELL_RegisterHandler()

Description:
    This function provides a mechanism for a content viewer or protocol engine to
register itself with the AEE Shell. This allows other components to share its functionality
when content or protocol types are encountered. As the handler is specified by class,
it can be loaded dynamically on an as-needed basis. IShell allows only one handler to be
associated with a scheme. To update a handler, the existing handler must be deleted from 
the database by calling ISHELL_RegisterHandler() with a 0 (zero) ClassID.

Prototype:
    int ISHELL_RegisterHandler
    (
    IShell * pIShell,
    AEECLSID clsBase,
    const char * pszIn,
    AEECLSID cls
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    clsBase  :  Handler type (HTYPE_VIEWER, HTYPE_SOUND) base class, or interface id.  
                The interface must be IQueryInterface based.
    pszIn  :  Input string. If this is NULL, EBADPARM is returned. The string contains
              a comma-delimited list of the MIME types or schemes handled by the specified
              class.
    cls  :  AEECLSID of the handler. No validation performed on this parameter.

Return Value:
    SUCCESS: operation is successful.
    EBADCLASS: invalid ClassID.
    EBADPARM: invalid parameter.
    ENOMEMORY: insufficient memory.
    EFAILED: operation failed.
    EALREADY: handler already set.

Comments:
    Supported handler types:
      AEECLSID_APP 
      AEECLSID_BITMAP
      AEECLSID_SCREEN_SAV
      AEECLSID_SOUNDPLAYER, HTYPE_SOUND.  Both must be registered.
      AEECLSID_VIEW, HTYPE_VIEWER.  Both must be registered.
      HTYPE_BROWSE
      Any IQI based interface.

Version:
    Introduced BREW Client 1.0

See Also:
    AEECLSID List
    AEEHandlerType
    ISHELL_GetHandler()

=======================================================================

ISHELL_RegisterNotify()

Description:
    This function provides a mechanism for applets to register for notifications
that are issued from other classes. For example, an applet can register for notification
from the AEECLSID_NET interface for events specific to that class. The notification
mask indicates the type of events of interest to the caller. These bits are defined
on a per-class basis, allowing classes to define and share notifications without
requiring all such notifications to be defined by the IShell Interface.
When a notification is requested of a specific object class,
the object class is created or the reference count is
incremented. When the notification is removed (mask=0), the object’s reference count
is decremented. To de-register from all notifications, invoke this function with dwMask set
to 0. Please look at documentation of dwMask below for information on how it is used.
   When an application registers for notification using its mif, RegisterNotify cannot
directly alert the application if a failure occurred.  So in the case of mif notify 
registrations, if a failure occurs the event EVT_NOTIFIER_FAILURE is sent to the 
application.  The dwParam contains a pointer to an AEENotifyFailure.

Prototype:
    int ISHELL_RegisterNotify
    (
    IShell * pIShell,
    AEECLSID clsNotify,
    AEECLSID clsType,
    uint32 dwMask
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    clsNotify  :  Applet to create and notify when the event occurs. clsNotify
                  should belong to the same module as the application calling
                  ISHELL_RegisterNotify or the calling application should be running in 
                  system context.
    clsType  :  Class that issues the event. This is the Notifier class, which must
                implement the INotifier Interface functions.
    dwMask  :  Mask of events to trigger. Setting this to zero causes the caller
               to be de-registered from all notifications from the Notifier class. The mask
               is comprised of two 16 bit components. The upper 16 bits is called
               the Notify Value and the lower 16 bits is called the Notify Mask.

Return Value:
    SUCCESS: if successful.
    EBADCLASS: invalid ClassID.
    EBADPARM: invalid parameter.
    ENOMEMORY: insufficient memory.
    EFAILED: Operation Failed
    EPRIVLEVEL: Insufficient privileges to register notify from notifier

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    AEECLSID List
    ISHELL_Notify()
    INOTIFIER_SetMask()
    GET_NOTIFIER_MASK
    GET_NOTIFIER_VAL

=======================================================================

ISHELL_Notify()

Description:
    This function is called by an object when it detects an event that may be associated
with a notification requested by another class of object. For example, the INetMgr
Interface calls this function when INetMgr-related events occur. The result is that
any applets that have requested notification, whether actively loaded or not, are
sent the EVT_NOTIFY with data about the specific event. The notification mask indicates
which event occurred. The data pointer provided is specific to the mask of the event
that occurred and is defined by the class that triggered the notification. All registered
applets for a specific type of notification are called. If the applet is not currently
active, it is loaded and the event is sent to it. It is not sent the EVT_APP_START
or EVT_APP_STOP events under these conditions. If the applet is to start based on
the event, it must call ISHELL_StartApplet().

Prototype:
    int ISHELL_Notify
    (
    IShell * pIShell,
    AEECLSID clsType,
    uint32 dwMask,
    void * pData
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    clsType  :  Class that issued the notification.
    dwMask  :  Mask of events in which typically only 1 bit is set for any given
               event.
    pData  :  Context sensitive data.

Return Value:
    SUCCESS: if successful.
    EBADCLASS: invalid ClassID.
    EBADPARM: invalid parameter.
    ENOMEMORY: insufficient memory.
    EREENTERED: attempt to re-enter ISHELL_Notify(). If a call to
    ISHELL_Notify() is made from within a current notification (for ex, calling
    ISHELL_Notify() from within the EVT_NOTIFY event handler of the app), this
    problem will occur.
    EBADTASK: invalid task issuing notify.
    EFAILED: operation failed

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    ISHELL_RegisterNotify()

=============================================================================

ISHELL_OnAppClose()

Description:
    This function enables an applet to register a system callback that gets called
when applet closes.

Prototype:
    void ISHELL_OnAppClose(IShell * pIShell, AEECallback * pcb);

Parameters:
    pIShell :  Pointer to the IShell object
    pcb :  pointer to the callback

Return Value:
    None

Comments:
    None

Version:
    Introduced BREW Client 2.0

See Also:
    AEECallback

=============================================================================

ISHELL_OnAppExit()

Description:
    This function enables an applet to register a callback that gets called
after applet destructor is executed.

Prototype:
    void ISHELL_OnAppExit(IShell * pIShell, AEECallback * pcb);

Parameters:
    pIShell :  Pointer to the IShell object
    pcb :  pointer to the callback

Return Value:
    None

Comments:
    None

Version:
    Introduced BREW Client 3.1.5SP01 

See Also:
    AEECallback

=============================================================================

ISHELL_OnInit()

Description:
    This function is deprecated.

Version:
    Introduced BREW Client 2.0
    Deprecated BREW Client 3.1

=============================================================================
ISHELL_OnExit()

Description:
    This function enables an applet to register a system callback that gets called
when BREW exits.

Prototype:
    void ISHELL_OnExit(IShell * pIShell, AEECallback * pcb);

Parameters:
    pIShell :  Pointer to the IShell object
    pcb :  pointer to the callback

Return Value:
    None

Comments:
    None

Version:
    Introduced BREW Client 2.0


See Also:
    AEECallback

=============================================================================

ISHELL_RegisterSystemCallback()

Description:
    This function allows clients to register callbacks for important system events
like: when BREW starts-up, when application exits, when mod is unloaded, etc.

Prototype:
    void ISHELL_RegisterSystemCallback
    (
    IShell * pIShell,
    AEECallback * pcb,
    int nSCBType
    );

Parameters:
    pIShell :  Pointer to the IShell object
    pcb :  Pointer to the AEECallback structure
    nSCBType :  Registered system callback type

Return Value:
    None

Comments:
    The callback must be allocated by the client.
    Following values are possible for nSCBType:~
~
    AEE_SCB_AEE_INIT: Call when BREW gets initialized~
    AEE_SCB_AEE_EXIT: Call when BREW exits~
    AEE_SCB_LOW_RAM: Call when device RAM goes below a pre-set threshold~
    AEE_SCB_LOW_RAM_CRITICAL: Call when device RAM goes below a pre-set threshold
                                   and AEE_SCB_LOW_RAM callbacks do not provide sufficient RAM~
    AEE_SCB_LOW_STORAGE: Call when device storage goes below a pre-set threshold~
    AEE_SCB_APP_CLOSED: Call when application is about to be closed~
    AEE_SCB_MOD_UNLOAD: Call when module is about to be unloaded~
    AEE_SCB_DEVICE_INFO_CHANGED: Call when device information such as language is changed~
    AEE_SCB_APP_EXIT: Call after application destructor is executed~
*

Version:
    Introduced BREW Client 2.0

See Also:
    ISHELL_OnLowRAM()
    ISHELL_OnLowRAMCritical()
    ISHELL_OnLowStorage()
    ISHELL_OnModUnload()

=============================================================================

ISHELL_OnLowRAM()

Description:
    This method can be used to register for the callback to be invoked when the
system reaches low memory condition. This allows the application to do memory cleanup
so as to free-up more memory for the system

Prototype:
    void ISHELL_OnLowRAM(IShell * pIShell, AEECallback * pcb);

Parameters:
    pIShell: Pointer to the IShell object
    pcb: Callback to be invoked

Return Value:
    None

Comments:
    None

Version:
    Introduced BREW Client 2.0

See Also:
    ISHELL_OnLowRAMCritical

=============================================================================

ISHELL_OnLowRAMCritical()

Description:
    This method can be used to register for the callback to be invoked when the
system reaches low memory condition and sufficent RAM has not been gathered
by the AEE_SCB_LOW_RAM callback. This allows the application to do memory cleanup
so as to free-up more memory for the system.

Prototype:
    void ISHELL_OnLowRAMCritical(IShell * pIShell, AEECallback * pcb);

Parameters:
    pIShell: Pointer to the IShell object
    pcb: Callback to be invoked

Return Value:
    None

Comments:
        The calling application must belong to the AEEGROUPID_LOW_RAM
group in order to successfully register for the callback.

Version:
    Introduced BREW Client 3.1

See Also:
    ISHELL_OnLowRAM()

=============================================================================

ISHELL_OnLowStorage()

Description:
    This method can be used to register for the callback to be invoked when the
system reaches low file-system space condition. This allows the application to do
file system cleanup so as to free-up more file system space for the system. The
callback is triggered if the creation of a file or directory failed due to lack of 
space on the file system or if the number of bytes which was written to a file on
a write request was less than the number of bytes specified in that write request.

Prototype:
    void ISHELL_SetOnLowStorage(IShell * pIShell, AEECallback * pcb);

Parameters:
    pIShell: Pointer to the IShell object
    pcb: Callback to be invoked

Return Value:
    None

Comments:
    None

Version:
    Introduced BREW Client 2.0

See Also:
    None

=============================================================================

ISHELL_OnModUnload()

Description:
    This function allows clients to register a callback to be called when any
    module is unloaded.

Prototype:
    void ISHELL_OnModUnload(IShell * pIShell, AEECallback * pcb);

Parameters:
    pIShell :  Pointer to the IShell object
    pcb :  Pointer to the AEECallback structure

Return Value:
    None

Comments:
    The callback must be allocated by the client.

Version:
    Introduced BREW Client 2.0

See Also:
    ISHELL_RegisterSystemCallback()

=======================================================================
ISHELL_Resume()

Description:
    This function allows a callback to be registered with the AEE Shell. It adds
the callback to the AEE Shell's list of pending operations. The AEE Shell invokes
the callback function the next time the event loop is called, allowing an application
or object to cooperatively multitask. If the callback has already been registered,
it is cancelled (de-registered) and then re-registered.

Prototype:
    void ISHELL_Resume(IShell * pIShell, AEECallback * pcb)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    pcb  :  Pointer to the AEECallback structure. When the application invokes this
            function, the following members in the AEECallback structure must be set
            by the caller:
            -    pfnNotify member must be filled by the caller. This is the callback
                 function that is invoked by AEE when the event loop is called.
            -    pNotifyData must be filled by caller. This is the data that is passed
                 to the callback function.
            -    pfnCancel member must be set to NULL by the caller before registering
                 the callback for the first time. The AEE Shell fills this member with
                 the right value when this function returns.

Return Value:
    None

Comments:
    When ISHELL_Resume() is used, the AEE Shell automatically fills up some of the
data members of the AEECallback structure passed to it (for example, the members
pfnCancel and pCancelData are filled by the AEE Shell.) To cancel a callback that
has been registered, use the pfnCancel member of the AEECallback structure and pass
the pCancelData as a parameter to it.

    Example:
    Assuming that pme refers to applet struct and pme->m_cb refers to AEECallback
    structure in applet data
===pre>
    // Fill up the members of the callback structure
    pme->m_cb.pfnNotify = MyCB;    // My callback function. It must be declared
                                   // as void MyCB(void *)
    pme->m_cb.pNotifyData = pme;   // Applet Specific data
    pme->m_cb.pfnCancel = NULL;    // Initialize to NULL. It is updated by Shell

    //To register the callback do the following:
    ISHELL_Resume(pme->a.m_pIShell,&pme->m_cb);

    // To cancel the CB do the following:
    if(pme->m_cb.pfnCancel)
      pme->m_cb.pfnCancel(&pme->m_cb);
===/pre>

Version:
    Introduced BREW Client 1.0

See Also:
    AEECallback

=======================================================================
ISHELL_ForceExit()

Description:
    This function returns TRUE if the applet must exit. It is provided only for
special use by browsers or other applications that can do substantial processing
before returning from ISHELL_HandleEvent().

Prototype:
    boolean ISHELL_ForceExit(IShell * pIShell)

Parameters:
    pIShell : Pointer to the IShell Interface object.

Return Value:
    TRUE: if the applet must stop processing and exit.
    FALSE: if the applet can continue to do processing.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    ISHELL_Busy()

=======================================================================

ISHELL_Busy()

Description:
    This function returns TRUE if the applet must exit. It is provided only for
special use by browsers or other applications that can do substantial processing
before returning from ISHELL_HandleEvent().

Prototype:
    boolean ISHELL_Busy(IShell * pIShell)

Parameters:
    pIShell  :  Pointer to the IShell Interface object.

Return Value:
    TRUE: if the applet must stop processing and exit.
    FALSE: if the applet can continue to do processing.

Comments:
    This function uses ISHELL_ForceExit().

Version:
    Introduced BREW Client 1.0

See Also:
    ISHELL_ForceExit()

=======================================================================

ISHELL_GetRand()
    NOTE: This function is deprecated. Use the helper function, GETRAND(), instead.

Description:
    This function leverages the random number generator on the device. It fills
an input buffer with an array of random values.

Prototype:
    void ISHELL_GetRand(IShell * pIShell, byte * pDest, int nSize)

Parameters:
    pIShell :  Pointer to the IShell Interface object.
    pDest :  Destination buffer.
    nSize :  Size in bytes of the buffer.

Return Value:
    None

Comments:
    This function uses the random number generator on the device to fill an input
buffer with a series of random bytes. The sequence is seeded by the current time,
so multiple calls may give identical results unless separated by at least several
milliseconds. For best results, call it once per invocation of the event handler
function.

Version:
    Introduced BREW Client 1.1
	Deprecated BREW Client 2.1
See Also:
    GETRAND()

=======================================================================
ISHELL_GetPosition()
    This method is deprecated. Please use IPOSDET_GetGPSInfo() to get GPS based
location information.

Description:
    This method provides access to the gpsOne location feature on the handset.
The precision specified indicates how exact the location is returned. The precision
is also directly related to the time it will take to satisfy the request. Position
Determination privileges are required to use this interface.
    NOTE: Requests for position location may be protected by the privacy policies
determined by the OEM or carrier. Upon completion, the callback is made to the user
with the position or an appropriate error.

Prototype:
    int ISHELL_GetPosition
    (
    IShell * pIShell,
    AEEPosAccuracy prc,
    PFNPOSITIONCB pfn,
    void * pUser
    )

Parameters:
    pIShell :  Pointer to the IShell Interface object.
    prc :  Precision.
    pfn :  Pointer to the callback.
    pUser :  Pointer to the callback data.

Return Value:
    SUCCESS: call is in progress.
    EUNSUPPORTED: GPS is not supported.
    EFAILED: cannot get GPS info.
    EPRIVLEVEL: application doesn't have required privileges.

Comments:
    See IPosDet Interface documentation, as well the SDK User's guide and Utilities
Guide for more information on GPS emulation on the SDK.

Version:
    Introduced BREW Client 1.0
	Deprecated BREW Client 2.1

See Also:
    IPosDet Interface
    AEEPosAccuracy
    PFNPOSITIONCB

=======================================================================

ISHELL_CheckPrivLevel()

Description:
    This function checks the privilege level of the currently running application
against the specified value. If the privilege level matches, the function returns
TRUE. If the privilege level does not match, the function returns FALSE and conditionally
terminates the application with a system message.

Prototype:
    boolean ISHELL_CheckPrivLevel
    (
    IShell * pIShell,
    AEECLSID clsIDWant,
    boolean bQueryOnly
    )

Parameters:
    pIShell  :  Pointer to the IShell Interface object.
    clsIDWant :  class to verify access for
    bQueryOnly :  If TRUE, this is a query only. If FALSE, the application terminates
if it does not have the privileges specified in wPrivWant.

Return Value:
    TRUE: application supports this privilege level mask.
    FALSE: application does not support this privilege level.

Comments:
    If bQueryOnly is FALSE and the function returns FALSE, no further processing
    is done on the application while it is terminating.

    If the high-order word of clsIDWant is 0, the value is treated as a bit-mask
    of the old-style privilege bits:~
~
       PL_FILE, ~
       PL_NETWORK,~
       PL_TAPI,~
       PL_DOWNLOAD,~
       PL_SHARED_WRITE,~
       PL_POS_LOCATION,~
       PL_SYSTEM, ~
       PL_WEB, ~
       PL_RINGER_WRITE,~
       PL_SECTORINFO,~
       PL_ADDRBOOK~
*
    ISHELL_CheckPrivLevel() can be thought of as "ISHELL_AppIsInGroup".
    An application becomes a member of a "group" in one of three ways:

       1. Every application is a member of its own group: the group
          that is equal to the application's class id.

       2. Each Privilege Level set in a module's MIF denotes a group,
          PL_FILE is a group, as is PL_NETWORK.

       3. Adding a class id to the module's "Dependencies" adds the
          module to the group denoted by that class id.

    A return value of SUCCESS from ISHELL_CheckPrivLevel() does not in itself
    imply that ISHELL_CreateInstance() will succeed.  The class id desired
    may not be present on the device, may not be instantiable (e.g. PL_FILE),
    or may be restricted (Applet's class ids may only be instantiated by
    the system).

    Every application is a member of the group 0.

Version:
    Introduced BREW Client 1.0

See Also:
    AEE Privilege Levels
        IFILEMGR_CheckPathAccess()
    ISHELL_AppIsInGroup()

=======================================================================

ISHELL_AppIsInGroup()

Description:
    This function checks the specified application class id for membership with
the specified group.  If the application belongs to the group, the function returns
SUCCESS, otherwise it returns an error.

Prototype:
    int ISHELL_AppIsInGroup
    (
    IShell * pIShell,
    AEECLSID idApp,
    AEECLSID idGroup
    )

Parameters
    pIShell  :  Pointer to the IShell Interface object.
    idApp:     app class id to verify access for
    idGroup:   group id

Return Value:
    SUCCESS: application supports this privilege level mask.
    ECLASSNOTSUPPORT: idApp is not a valid application class ID
    EPRIVLEVEL: application does not support this privilege

Comments:

    If the high-order word of idGroup is 0, idGroup is treated as a bit-mask
    of the old-style privilege bits:~
~
Version:
    Introduced BREW Client 3.1

See Also:
    AEE Privilege Levels
        ISHELL_CheckPrivLevel()

=======================================================================

ISHELL_IsValidResource()

Description:
    This function checks to see if the specified resource file entry is valid for
the type specified.

Prototype:
    boolean ISHELL_IsValidResource
    (
    IShell * pIShell,
    const char * pszResFile,
    uint16 wID,
    ResType t,
    AEEHandlerType ht
    )

Parameters:
    pIShell :  Pointer to the IShell Interface object.
    pszResFile :  Resource file name.
    wID :  ID of the resource.
    t :  Resource type.
    ht :  Handler type (ignored if t != RESTYPE_IMAGE).

Return Value:
    TRUE: resource exists in file.
    FALSE: resource does not exist.

Version:
    Introduced BREW Client 1.0

See Also:
    ResType
    AEEHandlerType

=======================================================================

ISHELL_DetectType()

Description:
    Given data in a buffer or the name of an object, this function detects the MIME
type.
    This function is typically used to get the handler associated with the data
type. For example, if the data represents standard MIDI format, then this function
returns the MIME "audio/mid". Using the MIME type, you can query Shell registry
to obtain the handler (Class ID) of type AEECLSID_MEDIA.

Prototype:
    int ISHELL_DetectType
    (
    IShell * pIShell,
    const void * cpBuf,
    uint32 * pdwSize,
    const char * cpszName,
    const char ** pcpszMIME
    );

Parameters:
    pIShell  :  [in] :  Pointer to the IShell object
    cpBuf  :  [in] :  Buffer containing the data whose type needs to be determined
    pdwSize  :  [in/out] :  On input: Size of data in pBuf, unless pBuf is NULL,
                            then ignored
                            On output: number of additional data bytes needed to
                            perform type detection
    cpszName  :  [in] :  Name of the object whose type needs to be determined (may
                         be null, if unknown).
    pcpszMIME  :  [out] :  MIME string returned to caller, on return, filled with
                           a pointer to a constant string (do not free)

Return Value:
    SUCCESS: Data type is detected and MIME is returned
    ENOTYPE: There is no type associated with this data
    EBADPARM: Wrong input data (parameter(s))
    ENEEDMORE: Need more data to perform type detection. *pdwSize contains the required
               number of additional bytes.
    EUNSUPPORTED: Type detection for the specified input is not supported

Comments:
    pBuf takes precedence over pszName. If both of them are specified, then first
pBuf is used for type detection followed by pszName.
    If the function returns ENEEDMORE, then *pdwSize is filled with the required
additional bytes to carry out the operation. Call this function again with (original
dwSize + *pdwSize) bytes.
    To determine the maximum number of bytes required to enable type detection,
you can do the following:
===pre>
    if (ENEEDMORE == ISHELL_DetectType(ps, NULL, &dwReqSize, NULL, NULL))
    {
    // dwReqSize contains the max bytes needed for type detection
    }
===/pre>
    A typical usage: The following code creates an IMedia-based object based on
the data in the buffer and starts the media playback.
===pre>
    //--------------- BEGIN -----------------------------
    // Assume,
    // pIShell: pointer to IShell
    // pBuf: buffer containing data multimedia format (IMedia-based)
    // dwSize: size of the buffer
    //
          const char * cpszMIME;
          AEECLSID clsHandler;
          IMedia * pMedia;
    if (SUCCESS == ISHELL_DetectType(pIShell, pBuf, &dwSize, NULL, &cpszMIME)
       {
          // Query for AEECLSID_MEDIA (i.e., IMedia) type handler
          clsHandler = ISHELL_GetHandler(pIShell, AEECLSID_MEDIA,
          cpszMIME);
             if (clsHandler)
             {
                if (SUCCESS == ISHELL_CreateInstance(pIShell, clsHandler,
                (void **)&pMedia))
                   {
                   // Start using IMedia interface...
                   IMEDIA_Play(pMedia); // starts the playback
                   :
                   :
                   }
             }
       }
    //--------------- END --------------------------------
===/pre>

Version:
    Introduced BREW Client 2.0

See Also:
    ISHELL_GetHandler()
    ISHELL_CreateInstance()

=============================================================================

ISHELL_GetAppAuthor()

Description:
    This method allows the caller to retrieve the name of the author of the active
applet. This information is obtained from the MIF of the current applet. The string
entered in the MIF under the field "Author Name" ("Company Name" in a previous versions
of MIF Editor) is returned by this function in the specified string buffer.

Prototype:
    int ISHELL_GetAppAuthor(IShell * pIShell, AECHAR * pBuff, int nSize);

Parameters:
    pIShell :  Pointer to the IShell object
    pBuff :  Buffer to fill with the author name string
    nSize :  Size in bytes of the input buffer

Return Value:
    Number of characters (AECHARs) filled.

    0 if the author name string was not found
or if the buffer pBuff is NULL

Comments:
    None

Version:
    Introduced BREW Client 2.0

See Also:
    None

=============================================================================

ISHELL_GetAppCopyright()

Description:
    This method allows the caller to retrieve the copyright string of the active
applet. This information is obtained from the MIF.

Prototype:
    int ISHELL_GetAppCopyright
    (
    IShell * pIShell,
    AECHAR * pBuff,
    int nSize
    )

Parameters:
    pIShell :  Pointer to the IShell object
    pBuff :  Buffer to fill with the string
    nSize :  Size in bytes of the input buffer

Return Value:
    Number of characters (AECHARs) filled.

    0 if the copyright string was not found or if the buffer pBuff is NULL

Comments:
    None

Version:
    Introduced BREW Client 2.0

See Also:
    None

=============================================================================

ISHELL_GetAppVersion()

Description:
    This method allows the caller to retrieve the version string of the active applet.
This information is obtained from the MIF.

Prototype:
    int ISHELL_GetAppVersion(IShell * pIShell, AECHAR * pBuff, int nSize);

Parameters:
    pIShell :  Pointer to the IShell object
    pBuff :  Buffer to fill with the string
    nSize :  Size in bytes of the input buffer

Return Value:
    Number of characters (AECHARs) filled.

    0 if the version string was not found or if the buffer pBuff is NULL

Comments:
    None

Version:
    Introduced BREW Client 2.0

See Also:
    None
=======================================================================

ISHELL_GetClassItemID()

Description:
    This method returns a 32-bit unique identifier associated with the owning module
for the specified class ID.

Prototype:
    uint32 ISHELL_GetClassItemID
    (
    IShell * pIShell,
    AEECLSID cls
    );

Parameters:
    pIShell :  Pointer to the IShell object
    cls :  Class ID to query

Return Value:
    0 - Class not found or module is static

Version:
    Introduced BREW Client 2.1

See Also:
    None

=======================================================================

ISHELL_GetProperty

Description:
   This method retrieves the property data for the underlying
   module associated with the input class.  Note the following:

- The returned information applies to all classes owned by the specified
  module containing the specified class.

- Data is only persistent across handset resets for dynamically downloaded
  modules.

- Specifying a NULL input buffer will return the size of the data

- In the case where the client supplied buffer is not large enough, 
  the method will attempt a partial fill where possible. In this case, 
  the method shall return SUCCESS, and *pwSize will contain the buffer 
  size required for complete retrieval. 

- Properties with the high-order bit set are reserved for use by caller's
  with system-level privs.

- Specifying 0 for the input AEECLSID will assume the currently
  running application.

If getting a protected property (ex: MPROP_SID), then the caller needs to have System Privileges. 
For all other properties, the caller must belong to the same group as the class whose property is requested.
i.e. either the calling app's classID is the same as cls or the
module that contains the calling app belongs to the same groupID as cls. 
This can be done by listing cls as a dependency in the MIF of the calling app.

Prototype:

   int  ISHELL_GetProperty
   (
   IShell *po,
   AEECLSID cls,
   uint16 wID,
   void * pDest,
   uint16 * pwSize
   );

Parameters:
   po: Pointer to the IShell object
   cls: Class of the application to query
   wID: Property ID. The supported properties are:
   MPROP_NETUSAGE,MPROP_USAGE,MPROP_STATUS

   pDest: Destination to fill
   pwSize: Pointer to size of input buffer.  Filled with data size

Return Value:

   The most likely error codes are -

   SUCCESS: the value 0 (zero)
   EBADCLASS: Input class not found
   EBUFFERTOOSMALL: Destination buffer size too small. *pwSize filled with proper size.
                    This error is only returned if a partial fill is not possible.
   EBADPARM: NULL pwSize
   EPRIVLEVEL: Caller does not have sufficient privileges

Side Effects:
   None

Version:
    Introduced BREW Client 3.1

See Also:
   MPROP_NETUSAGE,
   MPROP_USAGE,
   MPROP_STATUS,
   ISHELL_SetProperty

=======================================================================

ISHELL_SetProperty

Description:
   This method sets the property data for the underlying module associated
with the input class.  Note the following:

- Properties set on non-dynamic class modules are not persistent across
handset power cycles/resets.

- Specifying a NULL input buffer will clear the property

- Properties with the high-order bit set are reserved for use by caller's
with system-level privs.

- Specifying 0 for the input AEECLSID will assume the currently
running application.

If setting a protected property (ex: MPROP_SID), then the caller needs to have System Privileges. For all other
properties, the caller must belong to the same group as the class whose property is requested.
i.e. either the calling app's classID is the same as cls or the
module that contains the calling app belongs to the same groupID as cls. This can be done by listing cls
as a dependency in the MIF of the calling app.

Prototype:

   int  ISHELL_SetProperty
   (
   IShell *po,
   AEECLSID cls,
   uint16 wID,
   void * pSrc,
   uint16 wSize
   );

Parameters:
   po: Pointer to the IShell object
   cls: Class of the application to query
   wID: Property ID. The supported properties are:
   MPROP_NETUSAGE,MPROP_USAGE,MPROP_STATUS
   pSrc: Destination to fill
   wSize: Size of the buffer

Return Value:
   The value 0 (zero) - No Error

   EBADCLASS: Input class not found
   EPRIVLEVEL: Caller does not have sufficient privileges
   ENOMEMORY: Insufficient memory

Side Effects:
   None

Version:
    Introduced BREW Client 3.1

See Also:
   MPROP_NETUSAGE
   MPROP_USAGE,MPROP_STATUS
   ISHELL_GetProperty()

=======================================================================

ISHELL_RegisterEvent()

Description:
   This method registers a custom AEEEvent based upon the input
string.  This allows applications to dynamically reserve events for
use without fear of conflict with other EVT_USER range event codes.
Note the following:

- Events are only valid between power-cycles.  The same event registered
following another power cycle will very likley return a different AEEEvent.

- 0 indicates that the event was not registered

- Previously registered events are returned with the same values.

- If supplied, the pointer to pnCount is updated with the number
of times the event was registered.

Prototype:

   AEEEvent  ISHELL_RegisterEvent
   (
   IShell *po,
   const char * pszName,
   int * pnCount
   );

Parameters:
   po: Pointer to the IShell object
   pszName: Pointer to ASCIIZ name of the event
   pnCount: Filled with the count of attempts to register this event (if non-NULL)

Return Value:
   0 (zero) - Unable to register event.

   Non-0 - AEEEvent associated with the input name.

Side Effects:
   None

Version:
    Introduced BREW Client 3.1

See Also:
   None

=======================================================================
Function:  ISHELL_Reset()

Description: This method resets the device. There are two reset options available:
Soft Reset, Hard Reset.

The SoftReset options termintaes BREW and re-initializes it. The calling application is
stopped when BREW terminates.

The Hard Reset option power-cycles the device.

In either Reset Options, the calling application is not re-started automatically.

The calling application must have SYSTEM privilege level (PL_SYSTEM) to be able to
use this function.

Prototype:
   int ISHELL_Reset(IShell * po, AEEResetType t)


Parameters:
   po: Pointer to the IShell object
   t:  AEEResetType. This indicates the type of reset.

Return Value:
  SUCCESS: 0(zero)
  EBADPARM: Bad parameter is passed in
  EPRIVLEVEL:   Insufficient privilege to use this function. The calling application must have SYSTEM privilege level to use this function.

Side Effects: None

See Also:
AEEResetType
=======================================================================
=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEE Static Properties

Description:
    The properties defined for IStatic Interface.

Definition:
ST_CENTERTEXT     :Center Text
ST_CENTERTITLE    :Center Title
ST_NOSCROLL       :Do not scroll text
ST_TEXTALLOC      :Text allocated on heap - dialog takes responsibility of freeing it.
ST_TITLEALLOC     :Title allocated on heap - dialog takes responsibility of freeing it.
ST_MIDDLETEXT     :Text is drawn in the middle of the screen
ST_UNDERLINE      :Underline the title
ST_ICONTEXT       :Text is IImage *
ST_ASCII          :Text is single-byte
ST_ENABLETAB      :Generate EVT_CTL_TAB when at top or bottom
ST_ENABLE_HLGHT   :Highlights the static if it has focus
ST_WRAP_NOBREAK   :Wraps long lines without regard to Space characters

Members:
    None

Comments
    After setting ST_WRAP_NOBREAK you should call ISTATIC_Redraw(), so 
    the changes will be reflected on the display.

See Also:
    ISTATIC_SetProperties()
    ISTATIC_GetProperties()
    ISTATIC_Redraw()

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

IDialog Interface

   The IDialog Interface consists of two functions that operate on dialogs created
   using the functions in the IShell Interface (refer to the description of the IShell
   interface for more details about its dialog-related functions).

   IDIALOG_GetControl() obtains interface pointers to the date, menu, text, and time
   controls that make up the dialog. Use these pointers to modify the properties and
   rectangles of the controls or to retrieve the data the device user has entered or
   selected in each control.

   IDIALOG_SetFocus() specifies which control in a multicontrol currently has the focus
   (this control receives keypad input from the device user). Many controls generate
   control tabbing events when the device user presses the LEFT and RIGHT arrow keys.
   Dialogs use these control tabbing events to enable the device user to navigate between
   controls. IDIALOG_SetFocus() can be used with controls that do not support control
   tabbing (for example, Soft Key menus), or to designate the control that has focus
   initially.
===H2>
   To use IDIALOG_GetControl() and IDIALOG_SetFocus()
===/H2>

   1.   Create the controls in your dialog using the BREW Resource Editor or by populating
   the required dialog data structures in your code.

   2.   Call ISHELL_CreateDialog() to create the dialog and display it on the screen.

   3.   Call ISHELL_GetActiveDialog() to obtain an interface pointer to the
   dialog created in step 2.(The ISHELL_CreateDialog() does not return such a pointer.) 

   4.   Call IDIALOG_GetControl() to access the controls in the dialog, supplying as
   input the interface pointer obtained in step 3 and the control IDs you specified
   when you created the controls in step 1. Use this function immediately after the
   dialog has been created to customize the appearance and properties of its controls.

   5.   Call IDIALOG_SetFocus() as needed while the dialog is active to allow the device
   user to select the control in which data is to be selected or entered. For example,
   if your dialog uses a Soft Key menu, provide a menu item that allows the device
   user to return to a previous control to change the data entered.

   6.   Call IDIALOG_GetControl() when data entry is complete to obtain the values
   the device user has entered or selected in each control.

   7.   Call ISHELL_EndDialog() to terminate the dialog.

   NOTE: Dialog titles are not supported in the current version of BREW.

   NOTE: In a multi-control dialog, the left and right arrow keys should tab up and
   down in the list of the dialog controls. In the current version of BREW, the left
   arrow key tabs down, and the right arrow key does nothing.



=============================================================================

IDIALOG_AddRef()

Description:
    This function is inherited from IBASE_AddRef().

Version:
   Introduced BREW Client 1.0

See Also:
    IDIALOG_Release()

=======================================================================

IDIALOG_SetFocus()

Description:
   This function sets the active control focus to the control associated with the
   specified identifier.

Prototype:
   int16  IDIALOG_SetFocus(IDialog * pIDialog, int16 wID);

Parameters:
   pIDialog: Pointer to the IDialog object
   wID:      ID of the control

Return Value:
    Identifier of the control that had focus.

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   ISHELL_CreateDialog

=======================================================================

IDIALOG_GetControl()

Description:
   This function retrieves the IControl pointer for the control associated with
the specified identifier.

Prototype:
   IControl * IDIALOG_GetControl(IDialog * pIDialog, int16 wID);

Parameters:
   pIDialog: Pointer to the IDialog object
   wID: ID of the control

Return Value:
    IControl pointer if successful.

    NULL, if unsuccessful.

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   ISHELL_CreateDialog()
   IDIALOG_SetFocus()

=======================================================================

IDIALOG_SetEventHandler()

Description:
    This function sets or resets the event handler for a dialog. This function can
   be used to select an alternate event callback for application events sent by a dialog
   to an application or for events not processed by the dialog. If this event handler
   must receive all events that are not handled by the dialog, then the property DLG_HANDLE_ALL_EVENTS
   must be set using IDIALOG_SetProperties(). If this property is set, the event handler
   that is set using IDIALOG_SetEventHandler() will get all events that are not handled
   by the dialog.If this property is NOT set, then the event handler registered using
   IDIALOG_SetEventHandler() will only receive events sent from the dialog (EVT_DIALOG_END,
   EVT_COMMAND).

   While a dialog is active, the chain in which events are passed around is:
===pre>
    1.   The event is given to the controls in the dialog.
    2.   The event is sent to the event handler registered
       - If the dialog did not handle the event, and
       - If the property DLG_HANDLE_ALL_EVENTS is set and
       - If an event handler is registered.
    3.   The Event is sent to the current application
       - If this event handler did not handle this event.
===/pre>

Prototype:
   void IDIALOG_SetEventHandler(IDialog * pIDialog, PFNAEEEVENT pfn, void * pUser);

Parameters:
   pIDialog: Pointer to the IDialog object
   pfn:    Pointer to event callback
   pUser:     User data pointer sent as first argument to even handler

Return Value:
   None

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   ISHELL_CreateDialog()
   IDIALOG_SetFocus()

=======================================================================

IDIALOG_GetID()

Description:
   This function returns the resource ID of the dialog.

Prototype:
   uint16 IDIALOG_GetID(IDialog * po);

Parameters:
   po: Pointer to the IDialog object

Return Value:
   16-bit resource ID of the dialog.

Comments:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   ISHELL_CreateDialog
   IDIALOG_SetFocus

=======================================================================
Function: IDIALOG_SetProperties()

Description:
   This function sets the properties of the dialog. The properties that are supported are:

   DLG_HANDLE_ALL_EVENTS : If this property is set, the event handler
   that is set using IDIALOG_SetEventHandler() will get all events that are not handled by the dialog.
   If this property is NOT set, then the event handler registered using IDIALOG_SetEventHandler()
   will only receive events originating from the dialog (EVT_DIALOG_END, EVT_COMMAND).

Prototype:
   void IDIALOG_SetProperties(IDialog * po, uint32 dwProps);

Parameters:
   pIDialog: Pointer to the IDialog object
   dwProps:  Properties of the dialog

Return Value:
	None

Comments:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   ISHELL_CreateDialog()
   IDIALOG_GetProperties()
   IDIALOG_SetEventHandler()

=======================================================================

IDIALOG_GetProperties()

Description:
   Returns the current properties of the dialog

Prototype:
   uint32 IDIALOG_GetProperties(IDialog * pIDialog);

Parameters:
   pIDialog: Pointer to the IDialog object

Return Value:
   Current Properties of the Dialog

Comments:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   ISHELL_CreateDialog()
   IDIALOG_SetProperties()
   IDIALOG_SetEventHandler()

=============================================================================

IDIALOG_Redraw()

Description:
    This method redraws the specified dialog and all of its controls

Prototype:
    void IDIALOG_Redraw(IDialog *pIDialog)

Parameters:
    pIDialog: Pointer to the IDialog object

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    None

=============================================================================

IDIALOG_Release()

Description:
    This function is inherited from IBASE_Release().

Version:
   Introduced BREW Client 1.1

See Also:
    IDIALOG_AddRef()

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

IStatic Interface

The IStatic Interface allows you to create a static text control, that consists
of a read-only text message and a title that appears at the top of the control's
rectangle. Unlike the text controls created with the ITextCtl Interface, the text
in a static text control cannot be entered or modified by the device user. The text
message is broken into lines within the control rectangle with each line containing
an integral number of words (that is, words are not broken across lines) except
when the word is longer than the width of the control rectangle in which case the 
word is wrapped to the next line. If the text message is too long to completely fit
on the device screen, the static text control automatically scrolls the message. 
When the end of the message is reached, the continuous scrolling starts again from 
the beginning. Instead of using a text message as the control's contents, choose to 
display an animated bitmap created with the IImage Interface. If this option is selected, 
the bitmap is displayed centered inside the static text control's rectangle.

The following steps are typically followed to use a static text control (refer to
the function descriptions in the following subsections and the IStatic Interface
usage examples for details on the parameter lists and return values of each IStatic
Interface function).
===H2>
To use a static text control
===/H2>

1.   Create an instance of an IStatic Interface using the ISHELL_CreateInstance().

2.   Set the dimensions of the control's rectangle using the ISTATIC_SetRect().

3.   Obtain the text for the control's title and text message strings either by
reading them from a BREW resource file or from text strings in your code. If a bitmap
is being used in place of the text message, read in the bitmap and use IIMAGE_SetParm()
function to set its animation properties (number of frames, animation rate, and
other items).

4.   Call ISTATIC_SetText() to specify the control's title string, message string
or bitmap, and the fonts to be used for the title and text. You must call ISTATIC_SetRect()
(see step 2) before calling ISTATIC_SetText(), because the latter uses information
about the control rectangle to determine how to display the control.

5.   Call ISTATIC_Redraw() to display the control's contents on the screen.

6.   When the control is no longer needed, call ISTATIC_Release(). This function
frees the memory for the control's message text and title, but it does NOT release
an animated bitmap. You must explicitly release the IImage Interface if you created
an instance of one in step 3.

List of Header files to be included~
AEEShell.h

=============================================================================

ISTATIC_AddRef()

Description:
    This function is inherited from IBASE_AddRef().

See Also:
    ISTATIC_Release()

=============================================================================

ISTATIC_GetProperties()

Description:
    This function retrieves the properties of the IStatic Interface object. The
properties determine the physical look and feel of the object.

Prototype:
    uint32 ISTATIC_GetProperties(IStatic * pIStatic)

Parameters:
    pIStatic :  Pointer to the IStatic Interface object.

Return Value:
    ST_CENTERTEXT: if center text.
    ST_CENTERTITLE: if center title.
    ST_NOSCROLL: if sets the text does not scroll.
    ST_TEXTALLOC: if text allocated on heap - dialog takes responsibility of freeing it.
    ST_TITLEALLOC: if title allocated on heap - dialog takes responsibility of freeing it.
    ST_MIDDLETEXT: if text is drawn in the middle of the screen.
    ST_UNDERLINE: if underline the title is set.
    ST_ICONTEXT: if text is llmage.
    ST_ASCII: if text is single-byte
    ST_ENABLETAB: if EVT_CTL_TAB is generated when at top or bottom.
    ST_ENABLE_HLGHT: if highlights the static has focus.
    ST_WRAP_NOBREAK: Wraps long lines without regard to Space characters

Comments:
    None

See Also:
    ISTATIC_SetProperties()

=============================================================================

ISTATIC_GetRect()

Description:
    This function retrieves the location and size of the IStatic Interface object.

Prototype:
    void ISTATIC_GetRect(IStatic * pIStatic, const AEERect * prc)

Parameters:
    pIStatic  :  Pointer to the IStatic Interface object.
    prc  :  Pointer to the IStatic object rectangle (location and size).

Return Value:
    None

Comments:
    None

See Also:
    ISTATIC_SetRect()
    AEERect

=============================================================================

ISTATIC_GoToLine()

Description:
    This function allows the caller to auto-scroll to a specific line. The nLine
parameter is a 0-based index value.

Prototype:
    int ISTATIC_GoToLine(IStatic * pIStatic, int nLineIdx)

Parameters:
    pIStatic :  Pointer to the IStatic object
    nLineIdx :  Index to the scroll to display.

Return Value:
    Previous index value.

Comments:
    None

See Also:
    None

=============================================================================

ISTATIC_HandleEvent()

Description:
    This function handles key events for the active IStatic Interface object.

Prototype:
    boolean ISTATIC_HandleEvent
    (
    IStatic * pIStatic,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
    )

Parameters:
    pIStatic  :  Pointer to the IStatic Interface object.
    eCode  :  Event code.
    wParam  :  16-bit first event parameter.
    dwParam  :  32-bit second event parameter.

Return Value:
    TRUE: if event has been processed.
    FALSE: if event has not been processed.

Comments:
    None

See Also:
    AEEEvent

=============================================================================

ISTATIC_IsActive()

Description:
    This method returns TRUE if the ISTATIC is currently active and FALSE if it
is not active.

Prototype:
    boolean ISTATIC_IsActive(IStatic * pIStatic)

Parameters:
    pIStatic :  Pointer to the IStatic Interface object.

Return Value:
    TRUE: if the ISTATIC is active.
    FALSE: if the ISTATIC is not active.

Comments:
    None

See Also:
    ISTATIC_SetActive()

=============================================================================

ISTATIC_IsScrollable()

Description:
    This method allows the caller to determine if the text of the static control
fits in space allocated to the control.

Prototype:
    boolean ISTATIC_IsScrollable(IStatic * pIStatic);

Parameters:
    pIStatic :  Pointer to the IStatic object

Return Value:
    TRUE: if the text does not fit and the control scrolls.
    FALSE: if the text does fit.

Comments:
    None

See Also:
    None

=============================================================================

ISTATIC_Redraw()

Description:
    This function refreshes the IStatic Interface object on the screen.

Prototype:
    boolean ISTATIC_Redraw(IStatic * pIStatic)

Parameters:
    pIStatic  :  Pointer to the IStatic Interface object.

Return Value:
    TRUE: always.

Comments:
    None

See Also:
    None

=============================================================================

ISTATIC_Release()

Description:
    This function decrements the reference count of the IStatic Interface object.
The object is freed from memory and is no longer valid when the reference count
reaches 0 (zero).

Prototype:
    uint32 ISTATIC_Release(IStatic * pIStatic)

Parameters:
    pIStatic  :  Pointer to the IStatic Interface object.

Return Value:
    Decremented reference count for the object.

    0 (zero), If the object has been freed and is no longer valid.

Comments:
    The object has been freed and is no longer valid if 0 (zero) is returned.

See Also:
    ISTATIC_AddRef()

=============================================================================

ISTATIC_Reset()

Description:
    This function sets to null the contents of the IStatic object. The title is
left unchanged.

Prototype:
    void ISTATIC_Reset(IStatic * pIStatic)

Parameters:
    pIStatic  :  Pointer to the IStatic Interface object.

Return Value:
    None

Comments:
    None

See Also:
    None

=============================================================================

ISTATIC_SetActive()

Description:
    This method will set the selected ISTATIC as active or inactive.

Prototype:
    void ISTATIC_SetActive(IStatic * pIStatic, boolean bActive)

Parameters:
    pIStatic  :  Pointer to the IStatic Interface object.
    bActive  :  TRUE sets the ISTATIC to active,
    FALSE sets it to inactive.

Return Value:
    None

Comments:
    An ISTATIC object cannot be edited, but when selected, it is displayed with
a box around it if the ST_ENABLE_HLGHT property is turned on. This is useful when
many objects are on the screen to inform the user when object is currently selected.

See Also:
    ISTATIC_IsActive()
    ISTATIC_SetProperties()
    ST_ENABLE_HLGHT

=============================================================================

ISTATIC_SetFont()

Description:
    This function allows the caller to set the title and text fonts.

Prototype:
    void ISTATIC_SetFont
    (
    IStatic * pIStatic,
    AEEFont fntText,
    AEEFont fntTitle
    )

Parameters:
    pIStatic :  Pointer to the IStatic Interface object.
    fntText :  Font to use for the text.
    fntTitle :  Font to use for the title.

Return Value:
    None

Comments:
    None

See Also:
    None

=============================================================================

ISTATIC_SetProperties()

Description:
    This function set the properties of the IStatic Interface object. The properties
value determines the physical look and feel of the object.

Prototype:
    void ISTATIC_SetProperties(IStatic * pIStatic, uint32 nProperties)

Parameters:
    pIStatic :  Pointer to the IStatic Interface object.
    nProperties :  Properties value.

     ST_CENTERTEXT - Center text.
     ST_CENTERTITLE - Center title.
     ST_NOSCROLL - Do not scroll text.
     ST_TEXTALLOC - Text allocated on heap - dialog takes responsibility of freeing it.
     ST_TITLEALLOC - Title allocated on heap - dialog takes responsibility of freeing it.
     ST_MIDDLETEXT - Text is drawn in the middle of the screen.
     ST_UNDERLINE - Underline the title.
     ST_ICONTEXT - Text is llmage.
     ST_ASCII - Text is single-byte
     ST_ENABLETAB - Generate EVT_CTL_TAB when at top or bottom.
     ST_ENABLE_HLGHT - Highlights the static if it has focus.
     ST_WRAP_NOBREAK - Wraps long lines without regard to Space characters

Return Value:
    None

Comments:
The property ST_NOSCROLL enables the manual scrolling of long text messages 
with the up and down arrow keys.

See Also:
    ISTATIC_GetProperties()

=============================================================================

ISTATIC_SetRect()

Description:
    This function set the location and size of the IStatic Interface object.

Prototype:
    void ISTATIC_SetRect(IStatic * pIStatic, const AEERect * prc)

Parameters:
    pIStatic  :  Pointer to the IStatic Interface object.
    prc  :  Pointer to the IStatic Interface object rectangle (location and size).

Return Value:
    None

Comments:
    None

See Also:
    ISTATIC_GetRect()
    AEERect

=============================================================================

ISTATIC_SetText()

Description:
    This function sets the text and title of the IStatic Interface object.

Prototype:
    boolean ISTATIC_SetText
    (
    IStatic * pIStatic,
    AECHAR * pTitle,
    AECHAR * pText,
    AEEFont fntTitle,
    AEEFont fntText
    )

Parameters:
    pIStatic  :  Pointer to the IStatic Interface object.
    pTitle  :  Pointer to the title string.
    pText  :  Pointer to the text string.
    fntTitle  :  Title font type.
    fntText  :  Text font type.

Return Value:
    TRUE: if text and title were set correctly.
    FALSE: if otherwise.

Comments:
    None

See Also:
    AEEFont

=============================================================================

ISTATIC_SetTextEx()

Description:
    This method sets the text of the IStatic object. The function allows the caller
to:
     -    Specify text of either single-byte or double-byte width according to the
flags passed to the ISTATIC_SetProperties function. By default, text is considered
to be double-byte unless the ST_ASCII property has been set.
     -    Append to or replace the existing text
     -    Read text from an input stream. This allows the caller to avoid buffering
text from a file or socket.
    Please note the following:
     -    Title text is ALWAYS assumed to be double-byte. ST_ASCII only impacts
the text and not the title.
     -    Changing the ST_ASCII property automatically frees/resets the text in
the control.
     -    After the ST_ASCII property has been set or cleared, all text operations
are assumed to be either single or double-byte.
     -    Passing both pbText and pStream parameters add/replace the existing text
from pbText and then read from the stream.

Prototype:
    boolean ISTATIC_SetTextEx
    (
    IStatic * pIStatic,
    byte * pbText,
    IAStream * pStream,
    boolean bAppend
    )

Parameters:
    pIStatic :  Pointer to the IStatic Interface object.
    pbText :  Pointer to the text.
    pStream :  Pointer to the text stream.
    bAppend :  TRUE to append text, FALSE to reset text.

Return Value:
    TRUE: the text was set or added.
    FALSE: the text was not set or added.

Comments:
    None

See Also:
    None

=============================================================================

ISTATIC_SizeToFit()

Description:
    This method allows the caller to adjust the size of the bounding rectangle so
that it can
    fit all the text in the IStatic.

Prototype:
    void ISTATIC_SizeToFit(IStatic * pIStatic, AEERect *prc);

Parameters:
    pIStatic  :  Pointer to the IStatic Interface object
    prc :  pointer to location to set to bounding rectangle

Return Value:
    none

Comments:
    None

See Also:
    None

=====================================================================*/

