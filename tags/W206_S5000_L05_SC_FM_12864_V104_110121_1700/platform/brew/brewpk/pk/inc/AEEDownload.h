/*======================================================
                    IMPORTANT NOTICE

               OEM/CARRIER DISTRIBUTION ONLY

The contents of this file are provided under legal agreement
between QUALCOMM Incorporated and selected licensed carrier
and handset OEM partners. Under the terms of that agreement,
this file MUST NOT be distributed in full or in part to anyone
not specifically authorized by QUALCOMM Incorporated.

FILE:  AEEDownload.h

SERVICES:  AEE IDownload Interface

GENERAL DESCRIPTION:

  This file is provided by QUALCOMM Incorporated to selected carrier partners ONLY.

  It is NOT to be distributed as part of any software development kit.

  It is NOT to be distributed outside of the context specifically noted in licensing
  terms with QUALCOMM.

  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef AEEDOWNLOAD_H
#define AEEDOWNLOAD_H

#include "AEE.h"
#include "AEEDownload2.h"
#include "AEELicense.h"
#include "AEENet.h"
#include "AEEWeb.h"
#include "AEEFile.h"

#define AEEIID_DOWNLOAD    	0x01038c49

// This ID is used for fake, developer-installed apps...

#define FAKE_MODULE_ID            (0xfffffeff)

// Name of category icons resource file

#define ADS_ICONS_FILE            "adsicons.bar"

typedef struct _IDownload  IDownload;
typedef struct _IADSQuery  IADSQuery;

//*****************************************************************************
//
// IDOWNLOAD Interface
//
//*****************************************************************************
//===========================================================================
//
// The IDownload interface provides a mechanism to query and retreive application
// (or other object) lists.  

// Applications using IDownload needs PL_DOWNLOAD privilege to create the interface and 
// PL_SYSTEM privilege to perform actions such as acquire and restore (as PL_SYSTEM 
// allows the app to create/write in other app files/directories)
// 
// Organization of application data is provided in a hierarchy as follows:
//
// Category-> App (1-n) -> Price Basis (1-n) -> Prices (MAX_PRICES)
//
// For example:
//
// Catagory - "Games"
//     App - "Brick Attack"
//        Basis - DEMO
//           Price - $0 (1 use)
//        Basis - PURCHASE
//           Price - $5.00 (10 uses)
//           Price - $20.00 (unlimited use)
//
//     App - "Space Dudes"
//        Basis - PURCHASE
//           Price - $1.00 (10 uses)
//           Price - $5.00 (unlimited use)
//
// Category - "Tools"
//     App - "Scheduler"
//        Basis - PURCHASE
//           Price - $1.00 (10 uses)
//           Price - $5.00 (unlimited use)
//
// As outlined below, the query functions correspond to retrieval of category,
// application list and application "catalog" information.
//
//===========================================================================

//===========================================================================
//
// Events and Corresponding Data Structures
//
//===========================================================================

typedef uint32 DLMODID;
typedef uint32 DLPKGID;
typedef uint32 DLSSNID;
typedef uint32 DLCATID;

//
// DLITEMIDs - Upper-24 bits are the item ID and lower 8 bits are the version
//

#define DLITEMID_VER_MASK        (0x000000ffl)
#define DLITEMID_FAMILY_MASK     (0xffffff00l)

#define DLITEMID_RESERVED_RANGE  2000
#define DLITEMID_RESERVED(dw)    ((dw) < DLITEMID_RESERVED_RANGE || (dw) > FAKE_MODULE_ID)
#define GET_DLITEMID_FAMILY(dw)  (DLITEMID_RESERVED(dw) ? (dw) : ((dw) & DLITEMID_FAMILY_MASK))
#define GET_DLITEMID_VER(dw)     (byte)(DLITEMID_RESERVED(dw) ? 0 : ((dw) & DLITEMID_VER_MASK))


typedef struct _CatItem       CatItem;
typedef struct _DLItem        DLItem;
typedef struct _DLEnumItem    DLEnumItem;
typedef struct _DLList        DLList;
typedef struct _CategoryList  CategoryList;

//===========================================================================
//
// Category List Retrieval (IDOWNLOAD_GetCategoryList())
//
// Upon Success:
//
// nErr = 0 (Success)
//
// If the error code is 0, the CategoryList is passed as a pointer to the
// caller.  This pointer is valid until a subsequent call is made to
// IDOWNLOAD_GetCategoryList or until the interface is released.
//
// Upon Failure:
//
// nErr != 0 (AEEError.h)
//
//===========================================================================
struct _CategoryList
{
   int         nCount;
   DLITEMID    id;
   CatItem *   pList;
};

struct _DLList
{
   int           nCount;
   DLITEMID      id;
   DLItem *      pList;
};

typedef void (*PFNDLCATEGORYLIST)(void * pcxt,int nErr,CategoryList * pl);

//===========================================================================
//
// Application List Retrieval (IDOWNLOAD_GetItemList())
//
// Upon Success:
//
// nErr = 0 (Success)
//
// The DLList pointer is passed to the callback.  This pointer is valid
// until a subsequent call is made to the IDOWNLOAD_GetItemList or
// until the interface is released.
//
// Upon Failure:
//
// nErr != 0 (AEEError.h)
//
//===========================================================================

// Note - These two typedefs are for internal use only...

typedef struct _PackageInfoStruct PackageInfo;

//
// Note that there is NO currency value listed.  This is because the server
// provides the data localized for the local currency.  Thus, the UI need
// only provide the string and allow the user to select the value.
//
// The DLITEMID and selected DLPRICEID are passed to IDOWNLOAD_Acquire to
// tell the server what application price plan has been selected.
//

#define MAX_PRICES   (3)

typedef struct
{
   DLPRICEID   dwHandle;
   uint32      dwValue;
   AECHAR *    pszPrice;
} Price;

// This is the information provided for each price lt.

typedef struct
{
   AEELicenseType   lt;
   Price            Price;
} DemoPrice;

typedef struct
{
   AEELicenseType   lt;
   Price            Price;
} UpgradePrice;

typedef struct
{
   AEELicenseType   lt;
   int              nPrices;  // Number of prices actually specified.
   Price            pPrices[MAX_PRICES];
} PurchasePrice;

typedef struct
{
   AEELicenseType   lt;
   Price            Price;    //Price.dwValue holds subscription ID
} SubscriptionPrice;

typedef struct
{
   AEELicenseType   lt;
   Price            Price;    //Price.dwValue holds subscription ID
} AutoInstallPrice;


typedef void (*PFNDLITEMLIST)(void * pcxt,int nErr,DLList * pl);

// This is the master information for each application in the DLList

enum {
   DLI_NONE,
   DLI_APPLET,
   DLI_RINGER,
   DLI_JAPPLET,
   DLI_SHARED_FILE,
   DLI_OEM,
   DLI_CATEGORY,
   DLI_FOTA,
   DLI_CONTENT
};

#define DLI_SPITEM DLI_CONTENT

#ifdef WIN32
typedef unsigned DLItemType;
#else
typedef int8 DLItemType;
#endif

//
// PackageInfo
//

struct _PackageInfoStruct
{
   DLItemType  t;                // Set to DLI_APPLET if installed app...
   DLITEMID    iID;              // Item ID
   DLMODID     mID;              // Module ID
   DLPKGID     pID;              // Package ID
   uint16      wFlags;           // MIF - license flags
   uint32      nBytesFlash;
   flg         bMIFExists:1;
   flg         bRestartApps:1;   // Must restart apps that were stopped

   // These flags indicate how the installation was processed.  

   flg         bRestore:1;
   flg         bInstall:1;
   flg         bUpgrade:1;
};

//
// Version 1.10 and beyond definitions
//

struct _DLEnumItem
{
   DLEnumItem *      pNext;
   DLItemType        t;
   DLITEMID          idParent;
   DLITEMID          id;
   uint16            nIcon;
   AECHAR *          pszName;
   AECHAR *          pszDescription;
};


// Note - Base structure mirrors DLEnumItem above

struct _CatItem
{
   CatItem  *        pNext;
   DLItemType        t;
   DLITEMID          idParent;
   DLITEMID          dwID;
   uint16            nIcon;
   AECHAR *          pszName;
   AECHAR *          pszDescription;
};

// Note - Base structure mirrors DLEnumItem above

#define IFLAG_REQUIRED     0x0001   // Required item.  Processing fails if it fails to download.
#define IFLAG_RECOVERABLE  0x0002   // Recoverable item.  Must be backed-up before processed.
#define IFLAG_TEMP         0x8000   // Temporary item

struct _DLItem
{
   DLItem *          pNext;
   DLItemType        t;
   DLITEMID          idParent;
   DLITEMID          iID;              // App ID
   uint16            nIcon;
   AECHAR *          pszName;          // Item name
   AECHAR *          pszDescription;   // Long multiline descriptive text

   // Application-specific fields...

   AEELicenseInfo    currentLicense;   // App's current license
   DemoPrice         DemoOption;
   SubscriptionPrice SubscriptionOption;
   UpgradePrice      UpgradeOption;
   PurchasePrice     PurchaseOption;
   AutoInstallPrice  AutoInstallOption;
   boolean           bEULA;            // Has EULA
   int               nPackages;        // Internal use
   PackageInfo *     pPkgList;         // Internal use
   uint32            dwRAMRequired;    // RAM required to load this application
   uint32            dwFlags;
   AECHAR *          pszUses;          // 3.0 uses string
   
   //Action List support
   AECHAR *          pszActionType;    // ActionList action type
};



//===========================================================================
//
// Status Notification (All Asynchronous Calls)
//
//===========================================================================

enum {
   DEVT_NETEVENT,    // Network status change
   DEVT_READ,        // Download IO
   DEVT_ADS_MSG,     // Message from ADS/Carrier
   DEVT_ADS_RECALL,  // Recalled list of apps
   DEVT_AI_START,    // Auto-Install Started
   DEVT_AI_ASK,      // Auto-Install Notification (pszText != NULL)
   DEVT_AI_DENY,     // Confirmation (Yes/No) Must call IDOWNLOAD_Continue()
   DEVT_AI_SUCCESS,
   DEVT_AI_FAILURE,
   DEVT_ADS_BROWSE,  // Temporary - browse redirect from ADS
   DEVT_COMPLETE,
   DEVT_DL_STATUS,   // Delayed status of a previous download
   DEVT_SNOOP
};
#ifdef WIN32
typedef unsigned AEEDownloadEvt;
#else
typedef int8 AEEDownloadEvt;
#endif

typedef struct
{
   AEEDownloadEvt evt;

   // DEVT_NETEVENT

   NetMgrEvent    netEvent;         // DSTAT_NETEVENT
   NetState       netState;         // Network state

   // DEVT_READ

   uint32         nTotalBytes;      // Total bytes to read
   uint32         nBytes;           // Number read
   uint16         wPctComplete;     // Percent complete

   // DEVT_ADS_MSG, DEVT_ADS_RECALL, DEVT_AUTOINST, DEVT_AUTOINST_CONFIRM

   AECHAR *       pszText;          // Message data (AECHAR * or char *)
   int            nErr;             // Set by handler to indicate error
} DownloadStatus;

typedef boolean (*PFNDLSTATUS)(void * pcxt, DownloadStatus * pst);

//===========================================================================
//
// Download Complete (IDOWNLOAD_Acquire)
//
// This event is sent to the application upon completion of a download
// acquisition request.
//
// wParam - 16-bit status code (0, AEE_SUCCESS on success)
//
//===========================================================================

//
// This event is sent to the MobileShop application when an application was
// not found.  The MobileShop application should activate itself and attempt
// to re-load the requested application module(s)
//
// wParam = 0;
// dwParam = AEEReloadInfo *

typedef struct
{
   DLITEMID    iID;
   AEEAppInfo  ai;
   int         nReason;       // Why we got here...
} AEEReloadInfo;

#define EVT_DOWNLOAD_RELOAD  (AEEEvent)(EVT_USER + 7)

//
// This structure returned by IDOWNLOAD_GetModInfo, IDOWNLOAD_GetModInfoEx
// 
// Note: As indicated below, some fields are only valid if the flags are specified
//

//
// Flags for IDOWNLOAD_GetModInfoEx
//

#define GMI_LICENSE        (0x01)
#define GMI_CHECK_REMOVED  (0x02)      // Checks to see if executables removed
#define GMI_SIZE           (0x04)
#define GMI_APPS           (0x08)
#define GMI_ALL            (0xffffffffl)

typedef struct
{
   AEELicenseInfo li;            // (GMI_LICENSE)
   uint32         dwSpaceTotal;  // (GMI_SIZE) - total space occupied by this application
   uint32         dwSpaceApp;    // (GMI_SIZE) - total space occupied by application code/resources
   flg            bRemoved:1;    // (GMT_CHECK_REMOVED) - TRUE if the executables have been removed
   flg            bLocked:1;     // TRUE if user has locked app from auto-disable
   flg            bRemovable:1;  // (GMI_SIZE) - FALSE if the file(s) are persistent
   flg            bUpgradable:1; // (GMI_SIZE) - FALSE if the file(s) are fixed
   uint32         dwLastUsedTime; 
   uint32         dwInstalledTime;
   int            nApps;         // (GMI_APPS)
   AEEAppInfo     apps[1];       // (GMI_APPS)
} AppModInfo;

//
// This structure is filled by IDOWNLOAD_LogEnumNext
//

enum {
   DL_PURCHASE,            // Purchasing new item or extending existing license
   DL_RECALL,              // Removing item from phone due to server command
   DL_DELETE,              // Removing item from phone user controlled
   DL_FREEMEMORY,          // Deleting item files which can be reloaded
   DL_RESTORE,             // Reloading item which had its memory freed
   DL_ENDSUBSCRIPTION,     // End of subscription
   DL_UPGRADE              // Upgrade of existing app
};
#ifdef WIN32
typedef unsigned DLAction;
#else
typedef int8 DLAction;
#endif

#define DL_MAX_LOG_NAME 15

typedef struct
{
   uint32          dwTime;                       //local time when action initiated
   DLITEMID        iID;                          //item
   DLAction        action;                       //action causing logging
   int             nResult;                      //result code of action (AEEError.h)
   AECHAR          szName[DL_MAX_LOG_NAME + 1];  //name of item  /* MUST BE AT END OF STRUCT */
} DLLogItem;

// Authentication Policy - Whether the ADS requires user-level authentication.
//
// APOLICY_NONE - No authentication required
// APOLICY_SID - User's "subscriber ID" is passed to ADS before any set of transactions started
// APOLICY_TEXT - User should be prompted for text "key" and this sent to ADS
// APOLICY_NUM - User should be prompted for numeric "key" and this sent to ADS

enum {
   APOLICY_NONE,
   APOLICY_SID,
   APOLICY_TEXT,
   APOLICY_NUM
};
#ifdef WIN32
typedef unsigned AEEAuthPolicy;
#else
typedef int8 AEEAuthPolicy;
#endif

#define DL_MAX_AUTH  16


// This function called for each item found...

#define DL_CATEGORY_ROOT   (DLCATID)(0)

#if !defined(DL_MAX_SERVER)
#define DL_MAX_SERVER            64
#endif

typedef void (*PFNDLENUM)(void * pcxt, int nErr, DLEnumItem * pItem);

typedef struct
{
   uint32            dwCarrierID;
   uint32            dwPlatformID;
   uint32            dwAuth;        // AEEAuthPolicy
   uint16            wFlags;
   char              szServer[DL_MAX_SERVER];
} ADSInfo;

//
// This is the structure of records stored in "dlservers.dat"
//

typedef struct
{
   ADSInfo     s;
   AECHAR      szName[32];
} ADSInfoEntry;


#define DL_SERVER_FILE  "dlservers.dat"

//
// ADS Capabilities
//

#define ADS_CAP_MESSAGES         (0x00000001)
#define ADS_CAP_EULA             (0x00000002)
#define ADS_CAP_PREPAY           (0x00000004)
#define ADS_CAP_ICONS            (0x00000008)
#define ADS_CAP_ENUM             (0x00000010)
#define ADS_CAP_SEARCH           (0x00000020)
#define ADS_CAP_TIMEZONE         (0x00000040)
#define ADS_CAP_UPGRADECHECK     (0x00000080)
#define ADS_CAP_CREDITBACK       (0x00000100)
#define ADS_CAP_RUIM             (0x00000200)
#define ADS_CAP_MUTUALAUTH       (0x00000400)
#define ADS_CAP_ACTIONLIST       (0x00000800)
#define ADS_CAP_PURCHASEAUTH     (0x00002000)    // can do purchase auth CI
//#define ADS_CAP_DEVICECFG        (0x00002000)  // can do prepay stat from DC

#define ADS_CAP_LABELLED_OPT_MA  (0x00004000)    // Can do optimized MA. Handsets running the 
                                                 // patch ignore ADS_CAP_MUTUALAUTH & work off this instead. 
#define ADS_CAP_PRIV_PARAM       (0x00008000)    // move sensitive data (such as SID) from URL in MA X-BREWCLIENT header

//
// ADS Search Type
//
enum {
   DL_SEARCH_ANY,
   DL_SEARCH_ALL
};
#ifdef WIN32
typedef unsigned DLSearchType;
#else
typedef int8 DLSearchType;
#endif


//
// Callback for EULA and other text fetching routines...
//
typedef void (*PFNDLTEXT)(void * pcxt, int nErr, const AECHAR * pszText);

//
// Input block (filled) by IDOWNLOAD_CheckStorage
//
typedef struct _DLItemSize
{
   uint32         dwEstRAMRequired;
   uint32         dwRAMAvail;       // Largest block
   uint32         dwFSTotal;
   uint32         dwFSInstall;
   uint32         dwFSAvail;        // FS space avail for use
   uint32         dwFSSize;         // FS space total in file system
   int            nEstFilesRequired;
   int            nFilesAvail;      // FS files avail
} DLItemSize;

//
// Auto-Disable List Entries
//
typedef struct _DLDisableEntry
{
   DLITEMID       iID;
   uint32         dwLastUsedTime;
   uint32         dwSpace;
   boolean        bDisable;
} DLDisableEntry;

// Privacy policy mask...

typedef enum { PPOLICY_BREW,
               PPOLICY_CARRIER,
               PPOLICY_BREW_AND_CARRIER,
               PPOLICY_BREW_OR_CARRIER} AEEPrivacyPolicy;

#define DL_BKEY_SIZE             16
#define DL_AKEY_SIZE             8
#if !defined(DL_MAX_SERVER)
#define DL_MAX_SERVER            64
#endif

//
//
// CFGI_DOWNLOAD type - ADS Download Configuration...
//
//
 
typedef PACKED struct {
   uint32            dwCarrierID;
   uint32            dwPlatformID;
   uint8             bBKey[DL_BKEY_SIZE];       // default: all zero or OEM programmed BKey
   uint8             bAKey[DL_AKEY_SIZE];       // Should be SSD_A derived from A-key if bUseAKey==TRUE
   char              szServer[DL_MAX_SERVER];   // Server host names or IPs (ASCIIZ, commas if more than one)
   uint16            wFlags;                    // DIF_XXX flags defined above
   AEEAuthPolicy     nAuth;                     // APOLICY_XXX enum (AEEDownload.h)
   AEEPrivacyPolicy  nPolicy;                   // PPOLICY_XXX enum above...
} AEEDownloadInfo;


//
// Auto-Disable
//
// Applications are "auto-disabled" in the following order...
//
// 1)  List is scanned and apps are marked for "auto-disable" in least-recently-used
//     order until enough space is recovered.
// 2)  This sub-list is scanned backward and apps are "unmarked" if the space the
//     necessary space can be achieved without them.
//
// This covers the following example:
//
//  Space Required: 33K
//
//  App A    10K
//  App B    11K
//  App C    23K
//
// After Step 1, all three apps are marked for disable.
// After Step 2, only Apps A and Apps C are marked.  App B is no longer marked because
// it can be left enabled and the space can still be recovered.
//

//=============================================================================
//=============================================================================
QINTERFACE(IDownload)
{
   DECLARE_IBASE(IDownload)

   void           (*OnStatus)(IDownload * po, PFNDLSTATUS pfn, void * pUser);

   // Resident/Installed App functions...

   DLITEMID *     (*GetAppIDList)(IDownload *po, boolean bShowProtected);
   AppModInfo *   (*GetModInfo)(IDownload * po,DLITEMID iID);

   // Retrieval Functions...

   void           (*Cancel)(IDownload *po);
   void           (*GetCategoryList)(IDownload *po,PFNDLCATEGORYLIST pfn, void * pcxt);
   void           (*GetItemList)(IDownload *po, DLCATID dwCatCode, DLItemType t,PFNDLITEMLIST pfn, void * pcxt);
   uint32         (*GetSize)(IDownload * po,DLITEMID iID, uint32 * pdwTotal);
   void           (*Acquire)(IDownload *po, DLITEMID iID, DLPRICEID dwHandle, PFNDLCOMMAND pfn, void * pcxt);
   void           (*Restore)(IDownload * po, DLITEMID iID, PFNDLCOMMAND pfn, void * pcxt);
   int            (*Delete)(IDownload *po, DLITEMID iID, boolean bRemoveAllFiles);

   // Log file access...

   int            (*LogEnumInit)(IDownload * po);
   boolean        (*LogEnumNext)(IDownload * po, DLLogItem * pli);

   // Server re-direct

   uint32         (*GetADSCapabilities)(IDownload * po);
   ADSInfoEntry * (*GetADSList)(IDownload * po, int * pnCount,int * pnCurrIdx);
   boolean        (*SetADS)(IDownload * po, ADSInfo * ps);
   void           (*SetSubscriberID)(IDownload * po, const char * psz,int nSize);
   int            (*GetConfigItem)(IDownload * po, int i,void * pItem, int nSize);

   // >= 1.10 Enhanced enumeration mechanism...

   void           (*Enum)(IDownload * po, DLITEMID id, PFNDLENUM pfn, void * pcxt,boolean bUnfiltered);
   void           (*GetItemInfo)(IDownload * po, DLITEMID id,PFNDLENUM pfn, void * pcxt);
   void           (*CheckUpgrade)(IDownload * po, DLITEMID id, PFNDLENUM pfn, void * pcxt);
   void           (*Credit)(IDownload * po,const char * psz,PFNDLCOMMAND pfn,void * pcxt);

   void           (*GetEULA)(IDownload * po, DLITEMID id, PFNDLTEXT pfn, void * pcxt);
   void           (*SetHeaders)(IDownload * po, const char * pszHeaders);
   void           (*Search)(IDownload * po, const AECHAR * psz,DLSearchType st, PFNDLENUM pfn,void * pcxt);

   const char *   (*GetHeaders)(IDownload * po);
   void           (*Continue)(IDownload * po, boolean bContinue);

   int            (*GetSizeEx)(IDownload * po,DLITEMID id,DLItemSize * prs);

   // >= 2.0 Interfaces

   boolean           (*Lock)(IDownload * po, DLITEMID id,boolean bLock);
   DLDisableEntry *  (*GetAutoDisableList)(IDownload * po,DLITEMID idWant,uint32 dwWant,int * pnCount,int * pnErr);
   int               (*AutoDisable)(IDownload * po,DLITEMID iID);
   int              (*DeleteEx)(IDownload *po, DLITEMID iID, boolean bRemoveAllFiles,PFNDLCOMMAND pfn, void * pcxt);

   // 3.0 Interface

   AppModInfo *      (*GetModInfoEx)(IDownload * po, DLITEMID iID, uint32 dwFlags);
};

//=============================================================================
// The interface functions.
//=============================================================================

// Standard BREW Methods

#define IDOWNLOAD_AddRef(po)                          GET_PVTBL(po,IDownload)->AddRef(po)
#define IDOWNLOAD_Release(po)                         GET_PVTBL(po,IDownload)->Release(po)

// Resident App List Methods

#define IDOWNLOAD_OnStatus(po,pfn,pu)                 GET_PVTBL(po,IDownload)->OnStatus(po,pfn,pu)

#define IDOWNLOAD_GetAppIDList(po)                    GET_PVTBL(po,IDownload)->GetAppIDList(po,FALSE)
#define IDOWNLOAD_GetAppIDListEx(po)                  GET_PVTBL(po,IDownload)->GetAppIDList(po,TRUE)

#define IDOWNLOAD_GetModInfo(po,iID)                  GET_PVTBL(po,IDownload)->GetModInfo(po, iID)
#define IDOWNLOAD_GetModInfoEx(po,iID,dw)             GET_PVTBL(po,IDownload)->GetModInfoEx(po,iID,dw) 

#define IDOWNLOAD_GetConfigItem(po,i,pi,n)            GET_PVTBL(po,IDownload)->GetConfigItem(po,i,pi,n)
#define IDOWNLOAD_Cancel(po)                          GET_PVTBL(po,IDownload)->Cancel(po)
#define IDOWNLOAD_GetSize(po,iID,pdt)                 GET_PVTBL(po,IDownload)->GetSize(po, iID, pdt)
#define IDOWNLOAD_GetCategoryList(po,pfn,pcxt)        GET_PVTBL(po,IDownload)->GetCategoryList(po,pfn,pcxt)
#define IDOWNLOAD_GetItemList(po, dwCatID,t,pfn,pcxt) GET_PVTBL(po,IDownload)->GetItemList(po, dwCatID,t,pfn,pcxt)
#define IDOWNLOAD_Acquire(po, iID, dwHandle,pfn,pcxt) GET_PVTBL(po,IDownload)->Acquire(po, iID, dwHandle,pfn,pcxt)
#define IDOWNLOAD_Restore(po, iID,pfn,pcxt)           GET_PVTBL(po,IDownload)->Restore(po, iID,pfn,pcxt)
#define IDOWNLOAD_Delete(po, dwModID,b)               GET_PVTBL(po,IDownload)->Delete(po, dwModID,b)
#define IDOWNLOAD_DeleteEx(po, iID, b, pfn, pcxt)     GET_PVTBL(po,IDownload)->DeleteEx(po,iID,b,pfn,pcxt)
#define IDOWNLOAD_LogEnumInit(po)                     GET_PVTBL(po,IDownload)->LogEnumInit(po)
#define IDOWNLOAD_LogEnumNext(po,pli)                 GET_PVTBL(po,IDownload)->LogEnumNext(po,pli)

#define IDOWNLOAD_GetADSCapabilities(po)              GET_PVTBL(po,IDownload)->GetADSCapabilities(po)
#define IDOWNLOAD_GetADSList(po,pn,pnIdx)             GET_PVTBL(po,IDownload)->GetADSList(po, pn,pnIdx)
#define IDOWNLOAD_SetADS(po,ps)                       GET_PVTBL(po,IDownload)->SetADS(po, ps)
#define IDOWNLOAD_SetSubscriberID(po,psz,n)           GET_PVTBL(po,IDownload)->SetSubscriberID(po, psz,n)

#define IDOWNLOAD_GetCategory(po, dwCatID,pfn,pcxt)   GET_PVTBL(po,IDownload)->GetItemList(po, dwCatID,DLI_NONE,pfn,pcxt)
#define IDOWNLOAD_Get(po, iID,pfn,pcxt)               GET_PVTBL(po,IDownload)->Acquire(po, iID, 0,pfn,pcxt)

// >= 1.10

#define IDOWNLOAD_EnumRaw(po,id,pfn,pcxt)             GET_PVTBL(po,IDownload)->Enum(po,id,pfn,pcxt,TRUE)
#define IDOWNLOAD_Enum(po,id,pfn,pcxt)                GET_PVTBL(po,IDownload)->Enum(po,id,pfn,pcxt,FALSE)
#define IDOWNLOAD_GetItemInfo(po,id,pfn,pcxt)         GET_PVTBL(po,IDownload)->GetItemInfo(po,id,pfn,pcxt)
#define IDOWNLOAD_GetEULA(po,id,pfn,pcxt)             GET_PVTBL(po,IDownload)->GetEULA(po,id,pfn,pcxt)
#define IDOWNLOAD_SetHeaders(po,psz)                  GET_PVTBL(po,IDownload)->SetHeaders(po,psz)
#define IDOWNLOAD_Search(po,psz,st,pfn,pcxt)          GET_PVTBL(po,IDownload)->Search(po,psz,st,pfn,pcxt)
#define IDOWNLOAD_CheckItemUpgrade(po,id,pfm,pcxt)    GET_PVTBL(po,IDownload)->CheckUpgrade(po,id,pfm,pcxt)
#define IDOWNLOAD_CheckUpgrades(po,pfm,pcxt)          GET_PVTBL(po,IDownload)->CheckUpgrade(po,(DLITEMID)0,pfm,pcxt)
#define IDOWNLOAD_Credit(po,pszAccess,pfm,pcxt)       GET_PVTBL(po,IDownload)->Credit(po,pszAccess,pfm,pcxt)

// >= 1.20

#define IDOWNLOAD_GetHeaders(po)                      GET_PVTBL(po,IDownload)->GetHeaders(po)
#define IDOWNLOAD_Continue(po,a)                      GET_PVTBL(po,IDownload)->Continue(po,a)
#define IDOWNLOAD_GetSizeEx(po,id,prs)                GET_PVTBL(po,IDownload)->GetSizeEx(po,id,prs)

// >= 2.0

#define IDOWNLOAD_Lock(po,id,bl)                      GET_PVTBL(po,IDownload)->Lock(po,id,bl)
#define IDOWNLOAD_GetAutoDisableList(po,id,n,pn,pe)   GET_PVTBL(po,IDownload)->GetAutoDisableList(po,id,n,pn,pe)
#define IDOWNLOAD_AutoDisable(po,iID)                 GET_PVTBL(po,IDownload)->AutoDisable(po,iID)
#define IDOWNLOAD_GetAvailable(po,prs)                GET_PVTBL(po,IDownload)->GetSizeEx(po,(DLITEMID)0,prs)

//
// ADS Query Interface
//
typedef enum { ADSQUERY_NOTHING_PENDING,
               ADSQUERY_RECALL,
               ADSQUERY_PRELOAD_ACK,
               ADSQUERY_ACK,
               ADSQUERY_ACTIONLIST
} ADSStatus;

//=============================================================================
//=============================================================================
QINTERFACE(IADSQuery)
{
   DECLARE_IBASE(IADSQuery)

   ADSStatus   (*CheckPending)(IADSQuery * po);
   void        (*OnStatus)(IADSQuery * po, PFNDLSTATUS pfn, void * pUser);
   boolean     (*ProcessSMSAlert)(IADSQuery * po, const char * psz);
   void        (*ProcessPending)(IADSQuery * po, PFNDLCOMMAND pfn, void * pUser);
   void        (*Continue)(IADSQuery * po,boolean bContinue);
};

#define IADSQUERY_AddRef(po)                    GET_PVTBL(po,IADSQuery)->AddRef(po)
#define IADSQUERY_Release(po)                   GET_PVTBL(po,IADSQuery)->Release(po)
#define IADSQUERY_CheckPending(po)              GET_PVTBL(po,IADSQuery)->CheckPending(po)
#define IADSQUERY_OnStatus(po,pfn,pu)           GET_PVTBL(po,IADSQuery)->OnStatus(po,pfn,pu)
#define IADSQUERY_ProcessSMSAlert(po,psz)       GET_PVTBL(po,IADSQuery)->ProcessSMSAlert(po,psz)
#define IADSQUERY_ProcessPending(po,pfn,pu)     GET_PVTBL(po,IADSQuery)->ProcessPending(po,pfn,pu)
#define IADSQUERY_Continue(po,b)                GET_PVTBL(po,IADSQuery)->Continue(po,b)

#endif

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

PFNDLTEXT

Description:

Calback Function that is invoked when the EULA text is obtained from the server

Definition:

typedef void (*PFNDLTEXT)(void * pcxt, int nErr, const AECHAR * pszText);

Members:
pcxt: User Data passed when registering thic callback function( using IDOWNLOAD_GetEULA)
nErr: Error code (if any) in obtaining the EULA text. If successful, it returns AEE_SUCCESS. if failed, it could return one of the following Errors:

EFAILED
ENOMEMORY
EUNSUPPORTED

Comments:
None

See Also:
IDOWNLOAD_GetEULA()

=======================================================================*/

/*=======================================================================
  INTERFACE DOCUMENTATION
=======================================================================

Interface Name: IDownload

Description:
   This interface provides access to BREW application and file download
   mechanisms.  It is for use ONLY by QUALCOMM, Handset Manufacturer and
   selected partners.

   The IDownload interface consists of interface functions to query list(s)
   of application categories and/or items, download items and remove/disable
   items.  The interface has been developed on the IWeb interface and
   shields the developer from all of the complexities involved in
   downloading applications.

=======================================================================
Function: IDOWNLOAD_OnStatus()

Description:
    This function is passed a callback function that is called when
    any activity occurs as a result of calls to the IDownload class.  This
    includes status regarding recalled applications, download status, etc.

Prototype:
   void   IDOWNLOAD_OnStatus(IDownload * po, PFNDLSTATUS pfn, void * pUser);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    pfn [in]: Pointer to status callback function.
    pUser [in]: Pointer to user data context passed as first argument to function.

Return Value:
    None

Comments:
    None

Side Effects:
    None

=======================================================================
Function: IDOWNLOAD_GetAppIDList()

Description:
    This function returns a NULL terminated list of application IDs.  Each
    DLITEMID can be used to query specific information about the application
    via the IDOWNLOAD_GetModInfo() function.

Prototype:
   DLITEMID * IDOWNLOAD_GetAppIDList(IDownload *po);

Parameters:
    po [in]: Pointer to the IDownload interface object.

Return Value:
    SUCCESS: Returns a NULL terminated list of DLITEMID values.
    FAILURE: Returns NULL

Comments:
    The list returned is valid until a subsequent call is made to
    IDOWNLOAD_GetAppIDList() or the interface is released.

Side Effects:
    None

See Also:
    IDOWNLOAD_GetAppIDListEx()

========================================================================
Function: IDOWNLOAD_GetModInfo()

Description:
    This function allocates and returns a structure containing information
    regarding applications associated with a particular DLITEMID.

Prototype:
   AppModInfo * IDOWNLOAD_GeModInfo(IDownload *po, DLITEMID appID);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    appID [in]: App ID (returned from IDOWNLOAD_GetAppIDList())

Return Value:
    SUCCESS: Returns a pointer to the AppModInfo.
    FAILURE: NULL

Comments:
    The pointer returned is valid until a subsequent call is made to
    IDOWNLOAD_GetModInfo() or the interface is released.

Side Effects:
    None

========================================================================
Function: IDOWNLOAD_GetModInfoEx()

Description:
    This function allocates and returns a structure containing information
    regarding applications associated with a particular DLITEMID.  The 
    information returned is controlled by the flags specified.  The following
    flags are supported:


   GMI_LICENSE       - Return the license information
   GMI_CHECK_REMOVED - Determines if module executable has been removed
   GMI_SIZE          - Return the file size information
   GMI_APPS          - Return the applications for this module
   GMI_ALL           - Populate all fields

Prototype:
   AppModInfo * IDOWNLOAD_GeModInfoEx(IDownload *po, DLITEMID appID,uint32 dwFlags);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    appID [in]: App ID (returned from IDOWNLOAD_GetAppIDList())
    dwFlags [in]: Flags (see above)

Return Value:
    SUCCESS: Returns a pointer to the AppModInfo.
    FAILURE: NULL

Comments:
    The pointer returned is valid until a subsequent call is made to
    IDOWNLOAD_GetModInfo() or IDOWNLOAD_GetModInfoEx or the interface
    is released.

Side Effects:
    None

========================================================================
Function: IDOWNLOAD_Cancel()

Description:
   Cancel any pending download operation. This is a synchronous operation. The
   canceled operations may post events such as EVT_DOWNLOAD_COMPLETE with
   EFAILED status upon cancelation.

Prototype:
   void IDOWNLOAD_Cancel(IDownload *po);

Parameters:
    po [in]: Pointer to the IDownload interface object.

Return Value:
    None

Comments:
    None

Side Effects:
    None

See Also:
    IDOWNLOAD_Acquire()

========================================================================
Function: IDOWNLOAD_GetCategoryList()

Description:

   ** Version 1.0 Legacy Function - Use IDOWNLOAD_Enum() instead **

   Asynchronously retrieves the list of categories available on the server.

Prototype:
   void IDOWNLOAD_GetCategoryList(IDownload *po,PFNDLCATEGORYLIST pfn, void * pcxt);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    pfn [in]: Pointer to callback function.
    pctxt [in]: Pointer to user data context passed as first argument to function.

Return Value:
    None.

Comments:
    Upon completion the callback is called with the appropriate error value.
    If the error code is 0 (success), the category list pointer provided is
    valid.

Side Effects:

   This call will attempt to initiate a network connection

========================================================================
Function: IDOWNLOAD_GetItemList()

Description:

   ** Version 1.0 Legacy Function - Use IDOWNLOAD_Enum() instead **

    Asynchronously retrieves the list of applications for the specified
    category ID.  Note that if the specified id is 0, all known apps will
    be retrieved.

Prototype:
   void IDOWNLOAD_GetItemList(IDownload *po, DLCATID id,DLItemType t,PFNDLITEMLIST pfn, void * pcxt);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    id [in]: Category ID (0 - all)
    t [in]: Type of item to download (0 - all)
    pfn [in]: Pointer to callback function.
    pctxt [in]: Pointer to user data context passed as first argument to function.

Return Value:
    None

Comments:
    Upon completion the callback is called with the appropriate error value.
    If the error code is 0 (success), the item list pointer provided is
    valid.

Side Effects:
    This call will attempt to initiate a network connection

See Also:
    IDOWNLOAD_Enum()

========================================================================
Function: IDOWNLOAD_Acquire()

Description:
   Asynchronously downloads an application from the server.  This involves
   getting the package file, verifying it, and extracting the content files.
   The application's MIF file is the last file written to the file system. It
   registers a billing record with the server for the associated price value.

Prototype:
   void IDOWNLOAD_Acquire(IDownload *po, DLITEMID id, uint32 dwHandle, PFNDLCOMMAND pfn, void * pcxt);

Parameters:
    po       [in]: Pointer to the IDownload interface object.
    id       [in]: Application ID
    dwHandle [in]: Price value ID
    pfn      [in]: Callback function pointer
    pctxt    [in]: Pointer to be passed back in the callback function

Return Value:
    Upon completion, the callback specified is called with the associated
    completion code/error value.  A code of 0 indicates success.

Comments:
   During operation several events are posted to the client app class
   specified by clsID.  These events are:
   EVT_DOWNLOAD_COMPLETE to nofity client that the download has
                         finished, either successfully or not. Will be posted
                         once.

Side Effects:
   This call will attempt to initiate a network connection

See Also:
   IDOWNLOAD_Cancel()

========================================================================
Function: IDOWNLOAD_Restore()

Description:
    Asynchronously restores an applet that has been removed to save
    memory.  The application is downloaded free of charge and no
    billing transaction is generated.

Prototype:
   void IDOWNLOAD_Restore(IDownload *po, DLITEMID id,PFNDLCOMMAND pfn, void * pcxt);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    id [in]: Application ID
    pfn [in]: Pointer to callback function.
    pctxt [in]: Pointer to user data context passed as first argument to function.

Return Value:
    None

Comments:
    Upon completion, the callback specified is called with the associated
    completion code/error value.  A code of 0 indicates success.

Side Effects:
   This call will attempt to initiate a network connection

========================================================================
Function: IDOWNLOAD_Delete()

Description:
    Removes application files from persistent memory.  If the boolean
    "bRemoveAllFiles" parameter is specified, all files and sub-directories
    for the application are removed.  If not, the main resource and module
    files are removed.

Prototype:
   int IDOWNLOAD_Delete(IDownload *po, DLITEMID id, boolean bRemoveAllFiles);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    id [in]: Application Item ID
    bRemoveAllFiles [in]: Indicates whether all files should be removed.

Return Value:
    SUCCESS - Item disabled or removed
    EBADPARM - Item not found
    EITEMBUSY - Item specified is running/active
    EFAILED - Item cannot be disabled or removed

Comments:
    None

Side Effects:

========================================================================
Function: IDOWNLOAD_DeleteEx()

Description:
    Removes application files from persistent memory, along with sending 
    a delete ACK to the server.  If the boolean "bRemoveAllFiles" parameter
    is specified, all files and sub-directories for the application are 
    removed.  If not, the main resource and module files are removed.

    If this function returns SUCCESS value, then it continues the deletion
    process.  The given callback function is called to inform the deletion
    result.  On a non-success return code, the callback will not be called.

Prototype:
   int IDOWNLOAD_DeleteEx(IDownload *po, DLITEMID iID, boolean bRemoveAllFiles,
                        PFNDLCOMMAND pfn, void * pcxt);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    iID [in]: Application Item ID
    bRemoveAllFiles [in]: Indicates whether all files should be removed.
    pfn [in]: User callback to be called for item is deleted or on error.
    pcxt [in]:  User context handle passed as first parameter to callback.

Return Value:
    SUCCESS - Item disabled or removed
    EBADPARM - Item not found
    EBADSID  - If trying to delete an item belonging to another user
    EITEMBUSY - Item specified is running/active
    EFAILED - Item cannot be disabled or removed

Comments:
    None

Side Effects:

========================================================================
Function: IDOWNLOAD_LogEnumInit()

Description:
    Initializes the enumeration of download log entries.

Prototype:
   int IDOWNLOAD_LogEnumInit(IDownload *po);

Parameters:
    po [in]: Pointer to the IDownload interface object.

Return Value:
    SUCCESS - Successfully initialized
    EFAILED - Log file does not exist

Comments:
    None

Side Effects:
    None

See Also:
    IDOWNLOAD_LogEnumNext()

========================================================================
Function: IDOWNLOAD_LogEnumNext()

Description:
    Returns the next available log entry.  Returns FALSE if there are no
    more log items to enumerate.

Prototype:
   boolean IDOWNLOAD_LogEnumNext(IDownload *po,DLLogItem * pli);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    pli [out]: Pointer to log item entry to fill.

Return Value:
    TRUE - success
    FALSE - no more items or pli is NULL

Comments:
    None

Side Effects:

========================================================================
Function: IDOWNLOAD_GetADSList()

Description:
    This function returns a list of ADS servers.  It is supported only
    on test enabled handsets.  It is unsupported in production releases.

Prototype:
   ADSInfoEntry * IDOWNLOAD_GetADSList(IDownload * po, int * pnCount, int * pnIdx);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    pnCount [in/out]: Pointer to fill with count of servers.  If NULL, the
    existing list is freed.
    pnIdx [out]: Pointer to index into the server list of the current server

Return Value:
    Pointer to list of ADS servers or NULL if unsupported.

Comments:
    None

Side Effects:
    None

See Also:
    IDOWNLOAD_SetADS()

========================================================================
Function: IDOWNLOAD_SetADS()

Description:
    This function allows the caller to set the server that the download
    mechanism will use.  It is provided ONLY for debug purposes and is
    NOT supported on production handsets.

Prototype:
   boolean IDOWNLOAD_SetADS(IDownload * po, ADSInfo * ps);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    ps: [in]: Pointer to structure containing server information

Return Value:
    TRUE - success
    FALSE - function not supported

Comments:
    None

Side Effects:
    None

See Also:
    IDOWNLOAD_GetADSList()

========================================================================
Function: IDOWNLOAD_SetSubscriberID()

Description:
    This function allows the caller to set the subscriber ID.

Prototype:
   void IDOWNLOAD_SetSubscriberID(IDownload * po, const char * pszSID,int nSize);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    pszSID: [in]: Pointer to SID
    nSize: [in]: Size in bytes of subscriber ID (if <= 0 assumed to be DEFAULT_SUBSCRIBERID_LEN)

Return Value:
    None

Comments:
    None

Side Effects:

========================================================================
Function: IDOWNLOAD_Enum()

Description:
    This is the post-1.0.1 mechanism that supports advanced category and
    item enumeration.  The PFNDLENUM function callback is called for each
    item retrieved with a pointer to the item retreived.  When the operation
    is complete, the callback is called one final time with a NULL DLEnumItem
    pointer.

    Enumeration of the base/root category is initiated by calling the function
    with the DL_CATEGORY_ROOT item ID value.

    Calls to this function passing an item ID for an item of type DLI_CATEGORY
    will enumerate the items inside that category.  This allows the caller
    to enumerate the list of categories and applications using a mechanism similar
    to that used for file/directory enumeration.

    A call to this function for an item of type other than DLI_CATEGORY will
    return the DLItemInfo for that particular item.

    A call to this mechanism cancels any other pending calls.

    NOTE:  Caching of information is provided inside the protocol.  There is
    no need to cache information.  Information will not be retrieved from the
    network unless the cache is invalid.

Prototype:
   void IDOWNLOAD_Enum(IDownload * po, DLITEMID id, PFNDLENUM pfn, void * pcxt);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    id [in]: Item ID for the operation.
    pfn [in]: User callback to be called for each item retrieved.
    pcxt [in]:  User context handle passed as first parameter to callback.

Return Value:
    None

Comments:
    None

Side Effects:
    None

See Also:
    IDOWNLOAD_EnumRaw()

========================================================================
Function: IDOWNLOAD_GetItemInfo()

Description:
    This is the post-1.01 mechanism that allows the caller to query information
    about the item associated with the specified item ID.  The download engine
    will either retrieve cached information regarding the item or request
    the information from the server.

    This call is passed a user callback.  This callback will be called when
    the information for the associated item has been retreived.

    Unlike a call to IDOWNLOAD_Enum, this call will retrieve information ONLY
    about the specified item.  It will not enumerate the contents of an item
    of type DLI_CATEGORY.

    A call to this mechanism cancels any other pending calls.

    NOTE:  Caching of information is provided inside the protocol.  There is
    no need to cache information.  Information will not be retrieved from the
    network unless the cache is invalid.

Prototype:
   void IDOWNLOAD_GetItemInfo(IDownload * po, DLITEMID id, PFNDLENUM pfn, void * pcxt);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    id [in]: Item ID for the operation.
    pfn [in]: User callback to be called for each item retrieved.
    pcxt [in]:  User context handle passed as first parameter to callback.

Return Value:
    None

Comments:
    None

Side Effects:

========================================================================
Function: IDOWNLOAD_GetADSCapabilities()

Description:
    This is the post-1.0.1 function which returns the ADS capabilities.  This
    allows the application to determine whether some menu items, such as
    search should be displayed.

Prototype:
   uint32 IDOWNLOAD_GetADSCapabilities(IDownload * po);

Parameters:
    po [in]: Pointer to the IDownload interface object.

Return Value:
    ADS_CAP_XXXX flags - ADS_CAP_MESSAGES, ADS_CAP_EULA, ADS_CAP_PREPAY,
                         ADS_CAP_ICONS, ADS_CAP_ENUM, ADS_CAP_SEARCH,
                         ADS_CAP_TIMEZONE, ADS_CAP_MUTUALAUTH
    0 = No ADS server connected

Comments:
    None

Side Effects:

========================================================================
Function: IDOWNLOAD_SetHeaders()

Description:
    This is the post-1.0.1 function that allows the application to specify a
    special HTTP header that will be sent to the ADS on all requests. The
    format is as follows:

    "Name:Val\r\nName:Val\r\nName:Val\r\n"

    Passing a NULL value for the header removes the extra headers.

Prototype:
   void IDOWNLOAD_SetHeaders(IDownload * po,const char * pszHeader);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    pszHeaders [in]: Pointer to HTTP headers

Return Value:
    None

Comments:
    None

Side Effects:
    None

See Also:
    IDOWNLOAD_GetHeaders()

========================================================================
Function: IDOWNLOAD_Search()

Description:
    This is the post-1.0.1 function which initiates a search of ADS items that
    fulfill the search criteria specified.  The PFNDLENUM function callback
    is called for each item retrieved with a pointer to the item retreived.
    When the operation is complete, the callback is called one final time
    with a NULL DLEnumItem pointer.

Prototype:
   void IDOWNLOAD_Search(IDownload * po,const AECHAR * psz, DLSearchType st, PFNDLENUM pfn, void * pcxt);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    psz [in]: Search string
    st [in]: Search type (ANY, OR)
    pfn [in] : Search callback
    pcxt [in]: User context

Return Value:
    None

Comments:
    None

Side Effects:

========================================================================
Function: IDOWNLOAD_GetHeaders()

Description:
    Returns the current HTTP headers set via the IDOWNLOAD_SetHeaders() call.

Prototype:
   const char * IDOWNLOAD_GetHeaders(IDownload * po);

Parameters:
    po [in]: Pointer to the IDownload interface object.

Return Value:
    NULL - No headers set
    Header strings

Comments:
    None

Side Effects:
    None

See Also:
    IDOWNLOAD_SetHeaders()

========================================================================
Function: IDOWNLOAD_Continue()

Description:
    This method is called to indicate how the download engine should
    process an in-progress request.  It is intended for use following a
    status callback of type DEVT_AI_START,DEVT_AI_ASK, DEVT_AI_DENY, 
    DEVT_AI_SUCCESS, DEVT_AI_FAILURE.

Prototype:
   void IDOWNLOAD_Continue(IDownload * po,boolean bContinue);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    bContinue [in]: TRUE to continue

Return Value:
    None

Comments:
    This API is currently not supported.

Side Effects:

========================================================================
Function: IDOWNLOAD_GetSize()

Description:
    This function returns the total size required to download any missing
    portions of the specified item.  If non-NULL, the pdwTotal is filled
    with the total size of all files for the item.

Prototype:
   uint32 IDOWNLOAD_GetSize(IDownload *po, DLITEMID iID, uint32 * pdwTotal);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    iID [in]: Item ID
    pdwTotal [out]: Total bytes of all files (resident and non-resident)

Return Value:
    Size of non-resident file(s) to be downloaded.

Comments:
    None

Side Effects:
    None

========================================================================
Function: IDOWNLOAD_GetSizeEx()

Description:
    This method calculates the size required to store an item of 1-N packages.
    It returns an error if the size required is unavailable on the device.

Prototype:
   int IDOWNLOAD_GetSizeEx(IDownload * po,DLITEMID iID, DLItemSize * psi);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    iID [in]: Item ID to check
    psi [out]: Size information

Return Value:
    SUCCESS - If the size is obtained
    EFSFULL - Insufficient FS storage space (dwFSAvail < dwFSRequired)
    ENOMEMORY - Insufficient contiguous RAM for the item (dwRAM < dwEstRAMRequired)
    EOUTOFNODES - Insufficient file handles available (nFilesAvail < nEstFileRequired)
    EBADPARM - If psi is NULL or the package is invalid

Comments:
    None

Side Effects:

========================================================================
Function: IDOWNLOAD_GetAvailable()

Description:
    This method is called to populate available file and RAM space for
    potential downloads.

Prototype:
   int IDOWNLOAD_GetAvailable(IDownload * po,DLSizeInfo * psi);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    psi [out]: Size information

Return Value:
    EFSFULL - Insufficient FS storage space (dwFSAvail < dwFSRequired)
    ENOMEMORY - Insufficient contiguous RAM for the item (dwRAM < dwEstRAMRequired)
    EOUTOFNODES - Insufficient file handles available (nFilesAvail < nEstFileRequired)
    EBADPARM - If psi is NULL or the package is invalid

Comments:
    None

Side Effects:
========================================================================
Function: IDOWNLOAD_GetEULA()

Description:
    This function gets the EULA for the given ItemID. When the text is
    fetched, it invokes the callback function passed as argument to this
    function.

Prototype:
   void IDOWNLOAD_GetEULA(IDownload * po, DLITEMID id, PFNDLTEXT pfn, void * pcxt);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    id [in]: Item ID for the operation.
    pfn [in]: User callback to be called for item retrieved.
    pcxt [in]:  User context handle passed as first parameter to callback.

Return Value:
   None

Comments:
   None

Side Effects:

========================================================================
Function: IDOWNLOAD_GetConfigItem()

Description:
   This function retrieves the device configuration information related to
   the download services.

Prototype:
   int IDOWNLOAD_GetConfigItem(IDownload * po, int i,void * pItem, int nSize);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    i [in]: Item to be retrieved. It can be one of the following values:

===pre>
      - CFGI_DNS_IP1: 32-bit main Domain Name Server (DNS) IP Addresses in
        network byte-order
      - CFGI_DNS_IP2: 32-bit backup Domain Name Server (DNS) IP Address in
        network byte-order
      - CFGI_DOWNLOAD: information pertinent to the download service with the
        following information:
        + dwCarrierID: 32-bit carrier ID
        + dwPlatformID: 32-bit handset platform ID
        + bBKey: string of OEM programmed B-Key or all zeros
        + bAKey: string of SSD_A derived from A-Key or all zeros
        + szServer: string of server name
        + wFlags: 16-bit download related flags which can have one or more of the following
          values set:
          * DIF_USE_A_KEY: if set, use A-Key; otherwise use B-Key
          * DIF_TEST_ALLOWED: if set, the handset can be used to test local apps
          * DIF_MIN_FOR_SID: if set, use the MIN for the SID
          * DIF_PREPAY: if set, it is a prepay phone
          * DIF_NO_AUTO_ACK: if set, do not force ACKs until user runs MobileShop
          * DIF_SID_ENCODE: Uses SID rather than ESN for app encoding
          * DIF_SID_VALIDATE_ALL: Validates all apps rather than just SSN apps
          * DIF_RUIM_DEL_OVERRIDE: Allows one RUIM user to delete apps owned by another
          * DIF_AUTO_UPGRADE: Perform automatic upgrade when new uses purchased
          * DIF_NO_LAUNCH_MOD_ACK_DISABLED: If set, do not start an app with flag MOD_ACK_DISABLED set
          * DIF_SENSITIVE_SID: If set, do not send SID in download and delete ACK url.
        + nAuth: download authentication policy which can be one of the following values:
          * APOLICY_NONE: No authentication required
          * APOLICY_SID: User's "subscriber ID" is passed to ADS before any set of transactions started
          * APOLICY_TEXT: User should be prompted for text "key" and this sent to ADS
          * APOLICY_NUM- User should be prompted for numeric "key" and this sent to ADS
        + nPolicy: privacy policy which determines the type of certification required
          to run applet on the handset:
          * PPOLICY_BREW: TRUE-BREW-signed applet only
          * PPOLICY_CARRIER: carrier-signed applet only
          * PPOLICY_BREW_AND_CARRIER: TRUE-BREW- and carrier-signed applet only
          * PPOLICY_BREW_OR_CARRIER: TRUE-BREW- or carrier-signed applet
      - CFGI_SUBSCRIBERID: 32-byte subscriber ID in ASCII
      - CFGI_MOBILEINFO: information about the handset which include the following:
        + nCurrNAM: 8-bit handset's NAM
        + dwESN: 32-bit handset's ESN
        + szMobileID: 16-byte mobile number which consists of the following components:
          * mcc: 3-digit mobile country code
          * mnc: 2-digit mobile network code
          * min2: 3-digit mobile area code
          * min1: 7-digit mobile phone number
      - CFGI_AUTOSTART: class ID of the applet to be auto-started when AEE is initialized
        through AEE_Init().
      - CFGI_BUSY_CURSOR_OFFSET: offset position of the hourglass from the center of the
        screen. It is of the type AEERect which has the following fields:
        + x: x-offset from the center of the screen
        + y: y-offset from the center of the screen
        + dx: ignored
        + dy: ignored
      - CFGI_DOWNLOAD_BUFFER:32-bit unsigned integer value for the download buffer size, in bytes.
        The default is 10 kilobytes.
      - CFGI_HTTP_BUFFER: 32-bit unsigned integer value for the HTTP read buffer size, in bytes.
        The default is 4 kilobytes.
      - CFGI_NET_CONNTIMEOUT: 32-bit unsigned integer value for the network connection timeout,
        in milliseconds. The default is 30 seconds.
      - CFGI_SUBSCRIBERID_LEN: 32-bit signed integer value for size in bytes of subscriber ID
        The default length is 32.
      - CFGI_MAX_DISPATCH_TIME: 32-bit unsigned integer value for the maximum time BREW should
        spend in the dispatcher before relinquishing control. The default is 250 msecs.
      - CFGI_MIN_IDLE_TIME: 32-bit unsigned integer value for the minimum time BREW must relinquish
        from dispatcher. The default is 35 msecs.
      - CFGI_SLEEP_TIMER_RESOLUTION: 32-bit unsigned integer value for timer resolution during when
        processor/os is in SLEEP mode. The default is 1.2 seconds.
      - CFGI_SYSMEM_SIZE: 32-bit unsigned integer value for size in bytes reserved to the system
        in low-memory. The default is 2K bytes.
      - CFGI_DOWNLOAD_FS_INFOEX: AEEFSFreeSpace structure. If this is not supported by the OEM, BREW will try to 
        calculate available space with the CFGI_DOWNLOAD_FS_INFO config item. CFGI_DOWNLOAD_FS_INFOEX is the 
        64 bit version of CFGI_DOWNLOAD_FS_INFO for devices with an efs > 4GB.
      - CFGI_DOWNLOAD_FS_INFO: DLItemSize structure, only the dwFSAvail and dwFSSize will be used. If
        this is not supported by the OEM, BREW will try to calculate available space with ISHELL_GetDeviceInfoEx().    
      - CFGI_SCREEN_SAVER: AEEScreenSaverInfo structure, this is used to configure screensaver settings.
        Default values are: wSecs:30seconds, wSecsMin:5seconds, wSecsMax:300seconds.
      - CFGI_CLOSE_KEYS: OEMCloseKeys structure, the default values will be obtained from AEEDeviceInfo
        otherwise will be set to the following defaults:   wCloseApp:AVK_CLR, evtCloseApp:EVT_KEY, wCloseAllApps:AVK_END,
        evtCloseAllApps:EVT_KEY. The OEMCloseKeys structure will be filled before calling OEM_GetConfig, so you can check
        the defaults and change if necessary.
      - CFGI_FILE_CACHE_INFO: OEMFileCacheInfo structure, used to determine file caching levels.
        The default values are nCacheDefault:4k, nCacheMin:-1,nCacheMax:-1
      - CFGI_GPSONE_TRANSPORT: uint32 OEM GPS transport method (IP, Data burst)
      - CFGI_GPSONE_SVRIP: uint32 GPS server IP address
      - CFGI_GPSONE_SVRPORT: uint32 GPS server IP port
      - CFGI_GPSONE_LOCK: GPS lock
      - CFGI_MODULE_FSLIMIT: MIFFSLimit structure to indicate Maximum File usage and Maximum Space usage
        for the module. Default is unlimited space usage.
      - CFGI_CARDID_LEN: Length of the card ID of the RUIM/USIM card (if any) on the device.
      - CFGI_CARDID: The Card ID of the RUIM/USIM card (if any) on the device.
        For RUIM cards this will be the contents of the Removable UIMID record (6F31)
        The first byte in the UIMID record will not be present in 
        CFGI_CARDID as it represents the length of the record. The rest of the 
        record will be copied in as stored on the RUIM (little-endian).
      - CFGI_DEBUG_KEY: OEMDebugKey structure. Debug key sequence
      - CFGI_ALLOW_3G_2G_FAILOVER: boolean, if TRUE, allow failover. 
      - CFGI_ISTATIC_SCROLL: 32-bit unsigned integer value for the auto-scrolling speed of each line
        of text in the IStatic control, in milliseconds. The default is 1 second per visible line.
        This is useful for reducing flickering that may happen when the default value is too low/high.
      - CFGI_APPX5_MPC_ADDR: where the MPC specified in CU Appx5 lives (IP addr in network-byte order)
      - CFGI_APPX5_MPC_PORT: where the MPC specified in CU Appx5 lives (IP port in network-byte order)
      - CFGI_MAX_DEMO_ITEMS: Count of demo apps/items supported on the handset.  This
        number is used to store a list of previously downloaded demo items.  The default
        size of the list is 20 entries.  This means that at least 19 other demos must
        be downloaded before the same demo can be downloaded again.
      - CFGI_WEB_IDLECONNTIMEOUT: default IWeb IDLECONNTIMEOUT, in seconds, used if unspecified by application (see AEEWeb.h)
      - CFGI_APP_KEY_1: OEMConfigData structure for a secret to be used by applications to authenticate
        or sign content.  This is sensitive key material, and so should not be stored in
        predictable locations (i.e. file system or fixed memory locations).
      - CFGI_GSM1X_PRL: PRL access
      - CFGI_GSM1X_RTRE_CONFIG: RTRE_CONFIG variable access
      - CFGI_GSM1X_IDENTITY_PARAMS: Writing IMSI, ACCOLC and MSISDN to GSM1x assigned NAM
      - CFGI_GSM1X_SID_NID_PARAMS: Read/write home or locked SID/NID stored in NV.
      - CFGI_OFFLINE_PPP_TIMEOUT: time in milliseconds! to wait in offline state before closing PPP
      - CFGI_CACHED_RESOURCES: Number of resource files to cache in the BREW resource file layer manages.
        The default value for this number is 5.  However, on devices with more RAM and file handles, increasing
        this value may greatly improve the usability of the device.
      - CFGI_SAFEMODE_STARTMODE: Starting mode for the handset (Normal, Probation, or safe)
      - CFGI_SAFEMODE_TIMER: Time (in msecs) to wait before restoring device to normal mode
      - CFGI_AUTOSTARTARGS: Auto start args
      - CFGI_KB_AUTOREPEAT: OEMAutoRepeat structure 
      - CFGI_MOBILE_DIR_NUM: The Mobile Directory Number (MDN) for this device. As per NANPA specification
        this number should be 10 digit number. It should be usable as is through BREW APIs to make a voice call.
      - CFGI_MOBILE_DIR_NUM_LEN: The length of the MDN string.
      - CFGI_FIRMWARE_ID: OEM specified Firmware ID for this device.
      - CFGI_FIRMWARE_ID_LEN: Length of the Firmware ID string.
      - CFGI_MAX_ACK_ATTEMPTS: Max retries attempted to flush the preinstall/delete acks
      - CFGI_SMS_EMAIL_DEST_LENGTH: Length of destination referred by CFGI_SMS_EMAIL_DEST
      - CFGI_SMS_EMAIL_DEST: Destination to use if email needs to be sent as email address copied to payload 
      - CFGI_SMS_MAX_PAYLOAD_LENGTH: Maximum payload length in terms of number of bytes
      - CFGI_SMS_DEFAULT_MO_ENC: Default encoding for MO SMS
      - CFGI_SMS_MO_ENC_TYPES: Encoding types available for MO SMS
      - CFGI_SMS_MO_ENC_TYPES_COUNT: Count of encoding types available for MO SMS
      - CFGI_BREWDIR_SMS_TELESERVICE: Teleservice ID of BREW directed SMS
      - CFGI_SMS_MO_ON_ACCESS_CHANNEL: boolean, if TRUE, SMS message is sent on access channel
      - CFGI_SMS_MO_ON_TRAFFIC_CHANNEL: boolean, if TRUE, SMS message is sent on traffic channel. If both of 
        CFGI_SMS_MO_ON_ACCESS_CHANNEL and CFGI_SMS_MO_ON_TRAFFIC_CHANNEL are TRUE, SMS message is first sent 
        on access channel and on being too large for access channel is sent on traffic channel.
      - CFGI_DORMANCY_TIMEOUT_DEFAULT: 8-bit unsigned integer value (uint8) for the default 
        packet data inactivity timeout (which initiates dormancy), in seconds. 0 means inactivity timeout disabled.
        Default is 30 seconds.
      - CFGI_DOWNLOAD_CACHE:32-bit unsigned integer value for the download buffer cache, in bytes.  Used to configure 
        the maximum download page size that can be cached. The default is 1kilobytes and maximum is 10 kilobytes.
      - CFGI_LNG: Device Language Code
      - CFGI_MANNER_MODE: Set device manner mode. pSrc = AEE_MANNER_MODE_XXX, nSize = sizeof(uint32)
      - CFGI_APPX5_ON: User plane position determination ON (boolean)
      - CFGI_GPSONE_LOCATION_SERVER_NAME1: Fully qualified name of location server. If provided will
        be resolved and used by position determination process when application choses to use the default
        server.
      - CFGI_GPSONE_LOCATION_SERVER_NAME2: Fully qualified name of location server. If provided will
        be used by position determination process when CFGI_GPSONE_LOCATION_SERVER_NAME1 cannot be resolved.
      - CFGI_SMS_GW_SCADDRESS_LENGTH: length of default SC address for GW messages.
      - CFGI_SMS_GW_SCADDRESS: default SC address for GW messages. Gives a hook to OEM for providing service 
      - CFGI_SMS_GW_DOMAIN_PREF: center address for messages for which its not specified by the application. If 
        both of the application and OEM don't specify SC address, message is still provided to service layer 
        for processing.
      - CFGI_BACKLIGHT_CLASSIDS_LIST: 0 terminated list of class IDs implementing IBacklight interface
      - CFGI_RESERVED_1: 
      - CFGI_BREWMA_CAPABILITIES: BREWMA specific client capabilities. Bitflag Based.
      - CFGI_LAST_VALID_TIME: Last valid time stored in non-volatile storage. Updated with the secure clock's 
        value during successful synchronizations and during AEE_Init - if the secure clock is valid.
      - CFGI_BANNERNAME: AECHAR* - Banner text (null terminated) to be displayed in the idle screen
      - CFGI_BANNERNAME_LEN: 32 bit signed integer value. Banner Length in bytes
      - CFGI_THEMENAME: char* - Current themename copied onto the buffer (null terminated). The theme string is 
        a sequence of one or more (semi-colon ';' separated) filenames to the files making up the current theme.
      - CFGI_THEMENAME_LEN: 32 bit signed integer value. Theme name length in bytes. A size of 0 indicates to 
        the caller that the Default Theme is currently active.
      - CFGI_SUBLCD_PROPS: 32 bit unsigned integer value. Provides the visual properties of the sub-lcd of the device.  
        The data pointer holds a sequence of visual property key (uint32), value (uint32) pairs. The size field is the 
        number of bytes in the buffer. The property keys and their meaning and expected values are defined in 
        AEEWProperties.h in the UI Toolkit. 
      - CFGI_ANNUN_PROPS: 32 bit unsigned integer value. Provides the visual properties of the annunciator region 
        (also called the icon row) of the device.  The data pointer holds a sequence of visual property key 
        (uint32), value (uint32) pairs. The size field is the number of bytes in the buffer. The property keys 
        and their meaning and expected values are defined in AEEWProperties.h in the UI Toolkit.                                            //      - PROP_FGCOLOR
      - CFGI_THEMES_INI: char* - Current themes INI filename copied onto the buffer (null terminated).
      - CFGI_THEMES_INI_LEN: 32 bit signed integer value.  Themes INI filename length in bytes.
        A size of 0 indicates to the caller that there is no themes ini file configured.
      - CFGI_WALLPAPERS_INI: char* - Current wallpapers INI filename copied onto the buffer (null terminated).
      - CFGI_WALLPAPERS_INI_LEN: 32 bit signed integer value.  Wallpapers INI filename length in bytes.
        A size of 0 indicates to the caller that there is no wallpapers ini file configured.
      - CFGI_RINGTONES_INI: char* - Current ringtones INI filename copied onto the buffer (null terminated).
      - CFGI_RINGTONES_INI_LEN: 32 bit signed integer value.  Ringtones INI filename length in bytes.
        A size of 0 indicates to the caller that there is no ringtones ini file configured.
	  - CFGI_DOWNLOAD_PDP_ID: 32bit unsigned integer value to identify the PDP profile to be used
      - CFGI_DOWNLOAD_PROXY: char * - web proxy's address to be used (null terminated "http://addr:port")
      - CFGI_DOWNLOAD_PROXY_LEN: 32 bit signed integer value, Length of web proxy's address in bytes (null terminated "http://addr:port")
      - CFGI_DOWNLOAD_PROXYAUTH: char * - web proxy's credential (user:pass) to be used (null terminated "username:password")
      - CFGI_DOWNLOAD_PROXYAUTH_LEN: 32 bit signed integer value, Length of web proxy's credential in bytes.
           A size of 0 indicates to the caller that there is not proxy auth.

===/pre>
    pItem [in]: Buffer in which the return values are to be stored.
    nSize [in]: Size of the buffer

Return Value:
   SUCCESS  - on retrieving the information
   EBADPARM - if the config item is unknown
   EFAILED  - if the buffer is not big enough to hold the data to be returned.

Comments:
   None

Side Effects:
   None

See Also:
   OEM_GetConfig

========================================================================
Function: IDOWNLOAD_Lock()

Description:
    This function is used to either lock or unlock a module. Locking a
    module prevents it from being auto-disabled.

Prototype:
   boolean IDOWNLOAD_Lock(IDownload * po, DLITEMID id, boolean bLock);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    id [in]: Item ID for the operation.
    bLock[in]: TRUE if needs to be locked, FALSE otherwise

Return Value:
   TRUE  - Module locked or unlocked
   FALSE - Lock or unlock failed

Comments:
   None

Side Effects:

========================================================================
Function: IDOWNLOAD_GetAutoDisableList()

Description:
    This function retrieves the list of entries that can be auto-disabled.  The
idWant/dwExtra parameters can be specified in order to mark those items
that would be candidates to disable based upon size and date/time last used.

The list is returned sorted in least-recently used order.

By using DLITEMID, this call is equivalent to the IDOWNLOAD_AutoDisable function
without the function automatically disabling the items.

Prototype:
   DLDisableEntry * IDOWNLOAD_GetAutoDisableList(IDownload * po,DLITEMID idWant, uint32 dwExtra,int * pnCount,int * pnErr);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    idWant[in]: ID of item that may be downloaded. 0 if all entries
    dwExtra[in] : Extra number of bytes desired.
    pnCount[out]: Number of entries in the list
    pnErr[out] : Pointer to error

Return Value:
   Returns the list that can be auto-disabled
   NULL - Indicates an error
   *pnErr:
     SUCCESS - idWant + dwExtra size is available in file system
     EFSFULL - idWant + dwExtra cannot be satisified by disable

Comments:
   None

Side Effects:
   None

See Also:
   IDOWNLOAD_Lock()

========================================================================
Function: IDOWNLOAD_AutoDisable()

Description:
    This function is used to auto-disable applications. Applications are
    "auto-disabled" in the following order...
     1)  List is scanned and apps are marked for "auto-disable" in least-recently-used
         order until enough space is recovered.
     2)  This sub-list is scanned backward and apps are "unmarked" if the
         necessary space can be achieved without them.

     This covers the following example:

     Space Required: 33K

     App A    10K
     App B    11K
     App C    23K

     After Step 1, all three apps are marked for disable.
     After Step 2, only Apps A and Apps C are marked.  App B is no longer marked because
     it can be left enabled and the space can still be recovered.

Prototype:
   int IDOWNLOAD_AutoDisable(IDownload * po,DLITEMID iID);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    id [in]: Item ID for the operation.

Return Value:
    SUCCESS - If applications are successfully auto-disabled
    EFSFULL - Insufficient FS storage space (dwFSAvail < dwFSRequired)
    ENOMEMORY - Insufficient contiguous RAM for the item (dwRAM < dwEstRAMRequired)

Comments:
   None

Side Effects:
   None

See Also:
   IDOWNLOAD_GetAutoDisableList()
   IDOWNLOAD_Lock()

========================================================================
Function: IDOWNLOAD_GetCategory()

Description:
    ** Version 1.0 Legacy Function - Use IDOWNLOAD_Enum() instead **

    Asynchronously retrieves the list of categories for the specified
    category ID.

Prototype:
   void IDOWNLOAD_GetCategory(IDownload *po, DLCATID id, PFNDLITEMLIST pfn, void * pcxt);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    id [in]: Category ID (0 - all)
    pfn [in]: Pointer to callback function.
    pctxt [in]: Pointer to user data context passed as first argument to function.

Return Value:
    None

Comments:
    Upon completion the callback is called with the appropriate error value.
    If the error code is 0 (success), the item list pointer provided is
    valid.

Side Effects:
    This call will attempt to initiate a network connection

See Also:
    IDOWNLOAD_Enum()

========================================================================
Function: IDOWNLOAD_Get()

Description:
   Asynchronously downloads an application from the server.  This involves
   getting the package file, verifying it, and extracting the content files.
   The application's MIF file is the last file written to the file system.

Prototype:
   void IDOWNLOAD_Get(IDownload *po, DLITEMID id, PFNDLCOMMAND pfn, void * pcxt);

Parameters:
    po       [in]: Pointer to the IDownload interface object.
    id       [in]: Application ID
    pfn      [in]: Callback function pointer
    pctxt    [in]: Pointer to be passed back in the callback function

Return Value:
    Upon completion, the callback specified is called with the associated
    completion code/error value.  A code of 0 indicates success.

Comments:
   During operation several events are posted to the client app class
   specified by clsID.  These events are:
   EVT_DOWNLOAD_COMPLETE to nofity client that the download has
                         finished, either successfully or not. Will be posted
                         once.

Side Effects:
   This call will attempt to initiate a network connection

See Also:
   IDOWNLOAD_Acquire()

========================================================================
Function: IDOWNLOAD_EnumRaw()

Description:
    This is the post-1.0.1 mechanism that supports advanced category and
    item enumeration.  Returns all possible purchasing method for every known
    app with out any checks such as inconsistent purchasing methods, if it is
    an already resident app, etc.,. The PFNDLENUM function callback is called
    for each item retrieved with a pointer to the item retreived.  When the
    operation is complete, the callback is called one final time with a NULL
    DLEnumItem pointer.

    Enumeration of the base/root category is initiated by calling the function
    with the DL_CATEGORY_ROOT item ID value.

    Calls to this function passing an item ID for an item of type DLI_CATEGORY
    will enumerate the items inside that category.  This allows the caller
    to enumerate the list of categories and applications using a mechanism similar
    to that used for file/directory enumeration.

    A call to this function for an item of type other than DLI_CATEGORY will
    return the DLItemInfo for that particular item.

    A call to this mechanism cancels any other pending calls.

    NOTE:  Caching of information is provided inside the protocol.  There is
    no need to cache information.  Information will not be retrieved from the
    network unless the cache is invalid.

Prototype:
   void IDOWNLOAD_EnumRaw(IDownload * po, DLITEMID id, PFNDLENUM pfn, void * pcxt);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    id [in]: Item ID for the operation.
    pfn [in]: User callback to be called for each item retrieved.
    pcxt [in]:  User context handle passed as first parameter to callback.

Return Value:
    None

Comments:
    None

Side Effects:
    None

See Also:
    IDOWNLOAD_Enum()

=======================================================================
Function: IDOWNLOAD_GetAppIDListEx()

Description:
    This function returns a NULL terminated list of application IDs including
    those that are protected (modules that cannot be deleted by the user).  Each
    DLITEMID can be used to query specific information about the application via
    the IDOWNLOAD_GetModInfo() function.

Prototype:
   DLITEMID * IDOWNLOAD_GetAppIDListEx(IDownload *po);

Parameters:
    po [in]: Pointer to the IDownload interface object.

Return Value:
    SUCCESS: Returns a NULL terminated list of DLITEMID values.
    FAILURE: Returns NULL

Comments:
    The list returned is valid until a subsequent call is made to
    IDOWNLOAD_GetAppIDListEx() or the interface is released.

Side Effects:
    None

See Also:
    IDOWNLOAD_GetAppIDList()

========================================================================
Function: IDOWNLOAD_CheckItemUpgrade()

Description:
   This function checks whether there is an upgrade for the given ItemID.

Prototype:
   void IDOWNLOAD_CheckItemUpgrade(IDownload * po, DLITEMID id, PFNDLENUM pfn, void * pcxt);

Parameters:
   po [in]: Pointer to the IDownload interface object.
   id [in]: Item ID for the operation.
   pfn [in]: User callback to be called for item retrieved.
   pcxt [in]:  User context handle passed as first parameter to callback.

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   IDOWNLOAD_CheckUpgrades()

========================================================================
Function: IDOWNLOAD_CheckUpgrades()

Description:
   This function checks for upgrades for all the items.

Prototype:
   void IDOWNLOAD_CheckUpgrades(IDownload * po, PFNDLENUM pfn, void * pcxt);

Parameters:
   po [in]: Pointer to the IDownload interface object.
   pfn [in]: User callback to be called for item retrieved.
   pcxt [in]:  User context handle passed as first parameter to callback.

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   IDOWNLOAD_CheckItemUpgrade()

========================================================================
Function: IDOWNLOAD_Credit()

Description:
   This function credits the user.

Prototype:
   void IDOWNLOAD_Credit(IDownload * po,const char * psz,PFNDLCOMMAND pfn,void * pcxt);

Parameters:
   po [in]: Pointer to the IDownload interface object.
   psz[in]: Credit-back access ticket.
   pfn [in]: User callback to be called for item retrieved.
   pcxt [in]:  User context handle passed as first parameter to callback.

Return Value:
   None

Comments:
   None

Side Effects:
   None

=====================================================================*/


