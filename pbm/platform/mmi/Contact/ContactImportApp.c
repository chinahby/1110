/*=============================================================================

FILE: ContactImportApp.c

SERVICES: Contact Database Import and Export Applet

GENERAL DESCRIPTION:
   Imports contacts from an ASCII text file into the Contact Database.
   Exports contacts from the Contact Database into an ASCII text file. 

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2002 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //tsg/UI/main/Contacts/src/ContactImportApp.c#11 $
$DateTime: 2002/09/19 16:24:34 $
  $Author: mvines $
  $Change: 59976 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/19/02   mjv     Mobile Phone Numbers now export correctly.
08/30/02   snb     Fixed build problems when FEATURE_RUIM_PHONEBOOK in not 
                   defined
08/29/02   mjv     Removed all the 'FEATURE_ADDRBOOK' defines.  Importing 
                   R-UIM ADN entries now works (this time for sure!).  Do 
                   not export R-UIM FDN entries.
08/27/02   mjv     Importing R-UIM ADN entries now works.
07/31/02   mjv     Fixed memory corruption in CImportApp_InitImportTable()
07/01/02   mjv     Don't handle EVT_APP_SUSPEND/EVT_APP_RESUME
06/14/02   snb     Migrated to BREW Address Book from IContactDB. Added function
                   headers.
06/13/02   mjv     Include customer.h instead of OEMFeatures.h 
05/28/02   mjv     Dynamically allocate temporary buffers instead of 
                   statically allocating them.
05/07/02   mjv     Added the Contact Export Applet
02/22/02   mjv     Verify that each imported phone number contains valid 
                   digits
11/26/01   mjv     Initial revision

=============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifndef AEE_SIMULATOR
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"  // Simulator Feature Definitions
#endif

#ifdef FEATURE_APP_CONTACT_IMPORTER

#include "AEE.h"

#include "AEEShell.h"           // Shell interface definitions
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEFile.h"		        // AEEFile Services
#include "AEEStdLib.h"          // AEE StdLib Services
#include "AEEMenu.h"

#if defined(BREW_STATIC_APP)
#include "OEMClassIds.h"
#include "OEMMacros.h"
#endif

#include "AEEAddrBook.h"


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


////
// The name of the ASCII contacts EFS file 
#define CONTACTS_EFS_FILE "contacts.txt"


////
// Each line of the import file cannot be longer than
// this value
#define MAX_LINE_LENGTH 512


////
// Total number of 'IMPORTITEM' #defines below
#define IMPORTITEM_COUNT     13

////
// List of tags that we recognize (case sensitive)
#define IMPORTITEM_NAME      "Name"
#define IMPORTITEM_BIZCONT   "Business Contact"
#define IMPORTITEM_PERCONT   "Personal Contact"
#define IMPORTITEM_DEFCONT   "Default Contact"
#define IMPORTITEM_GENNUM    "Generic"
#define IMPORTITEM_HOMENUM   "Home"
#define IMPORTITEM_WORKNUM   "Work"
#define IMPORTITEM_RUIMNUM   "ADN"
#define IMPORTITEM_MOBILENUM "Mobile"
#define IMPORTITEM_ADDRESS   "Address"
#define IMPORTITEM_URL       "URL"
#define IMPORTITEM_EMAIL     "Email"
#define IMPORTITEM_NEXTREC   "========================="

// Newline string (using DOS style newlines for now)
#define NL                   "\r\n"


////
// Size of the CImportApp::m_szMsgBuf field
#define MSGBUF_SIZE 128


////
// EVT_COMMAND code for the "Start Import" softkey
#define ID_SK_STARTIMPORT  10

////
// EVT_COMMAND code for the "Exit" softkey
#define ID_SK_EXIT         11

////
// EVT_COMMAND code for the "Cancel" softkey
#define ID_SK_CANCEL       12


////
// EVT_COMMAND code for the "Start Export" softkey
#define ID_SK_STARTEXPORT  13


////
// BREW event we send to ourselves to import the
// next entry
#define EVT_USER_IMPORT    (EVT_USER+100)

////
// BREW event we send to ourselves to export the
// next entry
#define EVT_USER_EXPORT    (EVT_USER+101)

////
// Determines how fast contacts are imported.  If this value is too 
// small, the phone will be unresponsive during the operation or the watchdog
// may timeout.  
#define TIMEOUT_MS_IMPORT_THROTTLE_TIMER  200

////
// Determines how fast contacts are exported.  If this value is too 
// small, the phone will be unresponsive during the operation or the watchdog
// may timeout.
#define TIMEOUT_MS_EXPORT_THROTTLE_TIMER  100

// RUIM Phonebook feature
//#undef FEATURE_RUIM_PHONEBOOK
/*===========================================================================

                    MACRO DEFINITIONS 

===========================================================================*/

#define IsImportApp()  (pMe->m_clsid == AEECLSID_APP_CONTACT_IMPORTER)


/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

struct _CImportApp;

////
// Typedef for an import item callback function
typedef boolean (*ItemImportFunc)(struct _CImportApp *pMe);


////
// Required information for each import item.
typedef struct {
    char           *item; // string to look for
    ItemImportFunc  func; // handler function to invoke
} ItemImportTableEntry;


// CImportApp applet object. This is the main struct for this applet. This
// will hold all the data members that need to be remembered throughout the
// life of the applet.
//
// THE FIRST DATA MEMBER OF THIS STRUCT MUST BE AN AEEApplet OBJECT.
//
// This is critical requirement which is related to an applet being
// dynamically loaded. The app developers should add their data members
// following the AEEApplet data member.
typedef struct _CImportApp {
    AEEApplet             a;
    IFileMgr             *m_pIFileMgr;
    IFile                *m_pIFile;

    AEECLSID              m_clsid;

    ItemImportTableEntry *m_importTbl;

    // The message that will be displayed on the screen
    char                  m_szMsgBuf[MSGBUF_SIZE];

    // Set to TRUE when the import is finished
    boolean               m_opCompleted;

    // Keep track of the number of contacts that have been imported/exported
    uint32                m_recordCount;


    // These two fields are used by the import item callback functions
    // to access the data associated with their item
    char                 *m_szItemData;
    AECHAR               *m_wItemData;

    IAddrBook            *m_addrBook;//BREW address Book
    IAddrRec             *m_rec;   // Database record for the current contact
    AECHAR               *m_name;  // name of the current contact

    //
    // UI elements
    //
    IStatic              *m_staticBox;
    IMenuCtl             *m_softKey;

    //
    // Temporary buffers
    //
    char                  m_szBuf[MAX_LINE_LENGTH];
    char                  m_szBuf96[96];  // 96 is just an arbitrary number
    // that is 'big'

    char                  m_szCookedData[MAX_LINE_LENGTH];
    AECHAR                m_wCookedData[MAX_LINE_LENGTH];
} CImportApp;


/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/


////
// Applet methods
////
static boolean CImportApp_HandleEvent(IApplet *pi,
                                      AEEEvent eCode,
                                      uint16 wParam,
                                      uint32 dwParam);
static void    CImportApp_FreeAppData(IApplet *pi);

////
// Static methods
////
static boolean CImportApp_InitImportTable(CImportApp *pMe);
static void    CImportApp_TrimStr        (char *src,
                                          char *dst,
                                          uint16 dstSize);
static void    CImportApp_CheckNumber    (AECHAR *num);
static char   *CImportApp_ReadLine       (char *szBuf, IFile *pIFile);
static void    CImportApp_ImportItem     (CImportApp *pMe);
static void    CImportApp_ExportRecord   (CImportApp *pMe);
static boolean CImportApp_StrStr         (char *haystack,
                                          char *needle);
static void    CImportApp_DrawScreen     (CImportApp *pMe);
static void    CImportApp_AddSoftKeyItem (IMenuCtl   *pSoftKey,
                                          char       *szText,
                                          uint16      nItemID);
static void    CImportApp_ImportTimerCB (void *pUser);
static void    CImportApp_ExportTimerCB (void *pUser);

static boolean CImportApp_ImportItemCB_Name        (CImportApp *pMe);
static boolean CImportApp_ImportItemCB_BizContact  (CImportApp *pMe);
static boolean CImportApp_ImportItemCB_PerContact  (CImportApp *pMe);
static boolean CImportApp_ImportItemCB_DefContact  (CImportApp *pMe);
static boolean CImportApp_ImportItemCB_HomeNumber  (CImportApp *pMe);
static boolean CImportApp_ImportItemCB_WorkNumber  (CImportApp *pMe);
static boolean CImportApp_ImportItemCB_MobileNumber(CImportApp *pMe);
static boolean CImportApp_ImportItemCB_RUIMNumber  (CImportApp *pMe);
static boolean CImportApp_ImportItemCB_Address     (CImportApp *pMe);
static boolean CImportApp_ImportItemCB_URL         (CImportApp *pMe);
static boolean CImportApp_ImportItemCB_Email       (CImportApp *pMe);
static boolean CImportApp_ImportItemCB_NextRec     (CImportApp *pMe);


#if defined(BREW_STATIC_APP)

static int ImportAppMod_CreateInstance(AEECLSID  ClsId,
                                       IShell   *pIShell,
                                       IModule  *po,
                                       void    **ppObj);
static int ImportAppMod_Load(IShell   *ps,
                             void     *pHelpers,
                             IModule **pMod);

#endif /*defined(BREW_STATIC_APP)*/



/*===========================================================================

                    STATIC/LOCAL DATA

===========================================================================*/

#if defined(BREW_STATIC_APP)
// Save pointers to the Import/Export applets so that if BREW wants to
// create a second instance of one of the applets while the first instance
// already exists, just reuse the first instance.
CImportApp *sImportApp = NULL;
CImportApp *sExportApp = NULL;

#endif /*defined(BREW_STATIC_APP)*/


/*===========================================================================

                    FUNCTION DEFINITIONS

===========================================================================*/

#if defined(BREW_STATIC_APP)

/*=============================================================================
FUNCTION: ImportAppMod_Load

DESCRIPTION: Module load function

PARAMETERS:
   *pIShell:
   *ph:
   **ppMod:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int ImportAppMod_Load(IShell   *ps,
        void     *pHelpers,
        IModule **pMod
)
{
    return AEEStaticMod_New(sizeof(AEEMod),
                ps,
                pHelpers,
                pMod,
                (PFNMODCREATEINST)ImportAppMod_CreateInstance,
                (PFNFREEMODDATA)NULL);
}

#endif /*defined(BREW_STATIC_APP)*/

/*===========================================================================

FUNCTION: ImportAppMod_CreateInstance/AEEClsCreateInstance

DESCRIPTION

PARAMETERS:
   clsID: [in]: Specifies the ClassID of the applet which is being loaded
   pIShell: [in]: Contains pointer to the IShell interface.
   pIModule: pin]: Contains pointer to the IModule interface to the current 
      module to which this app belongs
   ppApplet: [out]: On return, *ppApplet must point to a valid AEEApplet
      structure. Allocation
   of memory for this structure and initializing the base data members 
      is done by AEEApplet_New.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS: If the app needs to be loaded and if AEEApplet_New() 
     invocation was successful
  EFAILED: If the app does not need to be loaded or if errors occurred 
     in AEEApplet_New().
  If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(BREW_STATIC_APP)
static int ImportAppMod_CreateInstance(AEECLSID  ClsId,
        IShell   *pIShell,
        IModule  *po,
        void    **ppObj
)
#else
int AEEClsCreateInstance(AEECLSID  ClsId,
        IShell   *pIShell,
        IModule  *po,
        void    **ppObj
)
#endif
{
    *ppObj = NULL;

    // Check if the applet already exists
    switch (ClsId) 
    {
        case AEECLSID_APP_CONTACT_IMPORTER:
            if (sImportApp) 
            {
                *ppObj = sImportApp;
                IAPPLET_AddRef( ((IApplet *) *ppObj) );
                return SUCCESS;
            }
            break;
            
        case AEECLSID_APP_CONTACT_EXPORTER:
            if (sExportApp) 
            {
                *ppObj = sExportApp;
                IAPPLET_AddRef( ((IApplet *) *ppObj) );
                return AEE_SUCCESS;
            }
            break;

        default:
            return EFAILED;
    }


    if (AEEApplet_New(sizeof(CImportApp),
            ClsId,
            pIShell,
            po,
            (IApplet**) ppObj,
            (AEEHANDLER) CImportApp_HandleEvent,
            CImportApp_FreeAppData) == TRUE) 
    {
        // Save the Class Id so we know who we are
        ((CImportApp*)*ppObj)->m_clsid = ClsId;

        // Save the new applet interface locally in case BREW asks for it again
        switch (ClsId) 
        {
            case AEECLSID_APP_CONTACT_IMPORTER:
                sImportApp = *ppObj;
                break;
                
            case AEECLSID_APP_CONTACT_EXPORTER:
                sExportApp = *ppObj;
                break;
                
            default:
                ASSERT_NOT_REACHABLE;
                break;
        }

        return AEE_SUCCESS;
    }
    
    return EFAILED;
}


/*=============================================================================
FUNCTION: CImportApp_FreeAppData

DESCRIPTION:

PARAMETERS:
   *pi: 

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CImportApp_FreeAppData(IApplet *pi)
{
    // applet no longer exists, remove the local pointer to it
    switch (((CImportApp*)(void *) pi)->m_clsid) 
    {
        case AEECLSID_APP_CONTACT_IMPORTER:
            sImportApp = NULL;
            break;
            
        case AEECLSID_APP_CONTACT_EXPORTER:
            sExportApp = NULL;
            break;
            
        default:
            ASSERT_NOT_REACHABLE;
            break;
    }
}

/*=============================================================================
FUNCTION:  CImportApp_HandleEvent

DESCRIPTION: Applet event handler



PARAMETERS:
   pi [in]: Pointer to the Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet

   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_HandleEvent(IApplet *pi,
        AEEEvent eCode,
        uint16 wParam,
        uint32 dwParam
)
{
    CImportApp *pMe = (CImportApp *)(void *) pi;

    switch (eCode) 
    {
        case EVT_APP_START:
            {
                AEEAppStart *as = (AEEAppStart*)dwParam;
                AEERect      rc;
                boolean      bFileExists;
    
                if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                    AEECLSID_ADDRBOOK,
                    (void **)&pMe->m_addrBook)) 
                {
                    return FALSE;
                }
    
                if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                    AEECLSID_FILEMGR,
                    (void **)&pMe->m_pIFileMgr)) 
                {
                    return FALSE;
                }
    
                if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                    AEECLSID_SOFTKEYCTL,
                    (void**)&pMe->m_softKey)) 
                {
                    return FALSE;
                }
                
                IMENUCTL_GetRect(pMe->m_softKey, &rc);
                IMENUCTL_SetActive(pMe->m_softKey, TRUE);
    
                if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                    AEECLSID_STATIC,
                    (void**)&pMe->m_staticBox)) 
                {
                    return FALSE;
                }
    
                rc.dy = rc.y;
                rc.y = as->rc.y;
                ISTATIC_SetRect(pMe->m_staticBox,  &rc);
    
                if (IsImportApp()) 
                {
    
                    if (!CImportApp_InitImportTable(pMe)) 
                    {
                        return FALSE;
                    }
    
                    bFileExists = (SUCCESS == IFILEMGR_Test(pMe->m_pIFileMgr,
                        CONTACTS_EFS_FILE));
    
                    // Setup the initial screen
                    (void) IMENUCTL_DeleteAll(pMe->m_softKey);
    
                    if (bFileExists) 
                    {
                        CImportApp_AddSoftKeyItem(pMe->m_softKey,
                            "Start Import",
                            ID_SK_STARTIMPORT);
                    }
                    CImportApp_AddSoftKeyItem(pMe->m_softKey,
                        "Exit",
                        ID_SK_EXIT);
    
                    if (bFileExists) 
                    {
                        (void) SPRINTF(pMe->m_szMsgBuf,
                                "Contact Database Import Utility.");
                    } 
                    else 
                    {
                        (void) SPRINTF(pMe->m_szMsgBuf,
                        "Contact Database Import Utility. "
                        "To perform a database import, the file "
                        CONTACTS_EFS_FILE " must exist in the /brew EFS"
                        "directory.");
                    }
                } 
                else 
                {
                    // Setup the initial screen
                    (void) IMENUCTL_DeleteAll(pMe->m_softKey);
    
                    CImportApp_AddSoftKeyItem(pMe->m_softKey,
                            "Start Export",
                            ID_SK_STARTEXPORT);
                    CImportApp_AddSoftKeyItem(pMe->m_softKey,
                            "Exit",
                            ID_SK_EXIT);
    
                    (void) STRCPY(pMe->m_szMsgBuf,
                        "Contact Database Export Utility. "
                        "This utility will export the contact "
                        "database into the EFS file " CONTACTS_EFS_FILE ".");
    
                    //Initialize all the variable for the search to get the records
                    (void)IADDRBOOK_EnumRecInit(pMe->m_addrBook,
                        AEE_ADDR_CAT_NONE,
                        AEE_ADDRFIELD_NONE,
                        NULL,
                        0);
                }
                CImportApp_DrawScreen(pMe);
    
                return TRUE;
            }

        case EVT_APP_STOP:
            if (pMe->m_importTbl != NULL) 
            {
                FREE(pMe->m_importTbl);
                pMe->m_importTbl = NULL;
            }
    
            if (pMe->m_pIFile != NULL) 
            {
                IFILE_Release(pMe->m_pIFile);
                pMe->m_pIFile = NULL;
            }
    
            if (pMe->m_pIFileMgr != NULL) 
            {
                IFILEMGR_Release(pMe->m_pIFileMgr);
                pMe->m_pIFileMgr = NULL;
            }
    
            if (pMe->m_softKey != NULL) 
            {
                IMENUCTL_Release(pMe->m_softKey);
            }
    
            if (pMe->m_staticBox != NULL) 
            {
                ISTATIC_Release(pMe->m_staticBox);
            }
    
            if (pMe->m_rec != NULL) 
            {
                IADDRREC_Release(pMe->m_rec);
                pMe->m_rec = NULL;
            }
    
            if (pMe->m_name != NULL) 
            {
                FREE(pMe->m_name);
                pMe->m_name = NULL;
            }
    
            if (pMe->m_addrBook != NULL) 
            {
                IADDRBOOK_Release(pMe->m_addrBook);
                pMe->m_addrBook = NULL;
            }
            return TRUE;


        case EVT_COMMAND:
            switch (wParam) 
            {
                case ID_SK_STARTIMPORT:
                    pMe->m_opCompleted = FALSE;
                    pMe->m_recordCount = 0;
        
                    (void) IMENUCTL_DeleteAll(pMe->m_softKey);
                    CImportApp_AddSoftKeyItem(pMe->m_softKey,
                        "Cancel",
                        ID_SK_CANCEL);
        
                    (void) SPRINTF(pMe->m_szMsgBuf,
                        "Importing:");
                        CImportApp_DrawScreen(pMe);
        
        
                    pMe->m_pIFile = IFILEMGR_OpenFile(pMe->m_pIFileMgr,
                            CONTACTS_EFS_FILE,
                            _OFM_READ);
                    if (NULL == pMe->m_pIFile) 
                    {
                        pMe->m_opCompleted = TRUE;
                        (void) SPRINTF(pMe->m_szMsgBuf,
                            "ERROR: Unable to open " CONTACTS_EFS_FILE " .");
                    }
        
                    return ISHELL_PostEvent(pMe->a.m_pIShell,
                            AEECLSID_APP_CONTACT_IMPORTER,
                            EVT_USER_IMPORT,
                            0,
                            0);
    
    
                case ID_SK_STARTEXPORT:
                    pMe->m_opCompleted = FALSE;
                    pMe->m_recordCount = 0;
        
                    (void) IMENUCTL_DeleteAll(pMe->m_softKey);
                    CImportApp_AddSoftKeyItem(pMe->m_softKey,
                            "Cancel",
                            ID_SK_CANCEL);
                    STRCPY(pMe->m_szMsgBuf, "Exporting:");
        
                    CImportApp_DrawScreen(pMe);
        
                    (void) IFILEMGR_Remove(pMe->m_pIFileMgr,
                        CONTACTS_EFS_FILE);
        
        
                    pMe->m_pIFile = IFILEMGR_OpenFile(pMe->m_pIFileMgr,
                            CONTACTS_EFS_FILE,
                            _OFM_CREATE);
        
                    if (NULL == pMe->m_pIFile) 
                    {
                        pMe->m_opCompleted = TRUE;
                        (void) SPRINTF(pMe->m_szMsgBuf,
                        "ERROR: Unable to create " CONTACTS_EFS_FILE " .");
                    }
                    return ISHELL_PostEvent(pMe->a.m_pIShell,
                            AEECLSID_APP_CONTACT_EXPORTER,
                            EVT_USER_EXPORT,
                            0,
                            0);

                case ID_SK_EXIT:
                    (void) ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);                  
                    return TRUE;

                case ID_SK_CANCEL:
                    if (IsImportApp()) 
                    {
                        pMe->m_opCompleted = TRUE;
        
                        // Setup the cancel screen
                        (void) IMENUCTL_DeleteAll(pMe->m_softKey);
                        CImportApp_AddSoftKeyItem(pMe->m_softKey,
                            "Exit", ID_SK_EXIT);
        
                        (void) SPRINTF(pMe->m_szMsgBuf,
                        "Import Canceled, %d contacts imported.",
                        pMe->m_recordCount);
                    } 
                    else 
                    {
                        pMe->m_opCompleted = TRUE;
        
                        if (pMe->m_pIFile) 
                        {
                            IFILE_Release(pMe->m_pIFile);
                            pMe->m_pIFile = NULL;
                        }
        
                        (void) IFILEMGR_Remove(pMe->m_pIFileMgr,
                            CONTACTS_EFS_FILE);
        
                        // Setup the cancel screen
                        (void) IMENUCTL_DeleteAll(pMe->m_softKey);
                        CImportApp_AddSoftKeyItem(pMe->m_softKey,
                            "Exit", ID_SK_EXIT);
        
                        (void) SPRINTF(pMe->m_szMsgBuf, "Export canceled.");
                    }
        
                    CImportApp_DrawScreen(pMe);
                    return TRUE;

                default:
                    break;
            }
            return FALSE;

        case EVT_USER_IMPORT:
            if (!pMe->m_opCompleted) 
            {
                CImportApp_ImportItem(pMe);
            }
    
            if (pMe->m_opCompleted) 
            {
                (void) IMENUCTL_DeleteAll(pMe->m_softKey);
                CImportApp_AddSoftKeyItem(pMe->m_softKey,
                    "Exit", ID_SK_EXIT);
    
                CImportApp_DrawScreen(pMe);
            } 
            else 
            {
                (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                        TIMEOUT_MS_IMPORT_THROTTLE_TIMER,
                        CImportApp_ImportTimerCB,
                        pMe);
            }
            return TRUE;

        case EVT_USER_EXPORT:
            if (!pMe->m_opCompleted) 
            {
                CImportApp_ExportRecord(pMe);
            }
    
            if (pMe->m_opCompleted) 
            {
                (void) IMENUCTL_DeleteAll(pMe->m_softKey);
                CImportApp_AddSoftKeyItem(pMe->m_softKey,
                    "Exit", ID_SK_EXIT);
    
                CImportApp_DrawScreen(pMe);
            } 
            else 
            {
                (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                        TIMEOUT_MS_EXPORT_THROTTLE_TIMER,
                        CImportApp_ExportTimerCB,
                        pMe);
            }
            return TRUE;


        default:
            // First pass the event to the IStatic then to the IMenuCtl.
            // We want to give IStatic first crack at the UP/DOWN arrows
            // to scroll text, since the IMenuCtl will generate TAB events
            // on UP/DOWN arrows (and report that it handled the event)
            if (pMe->m_staticBox && ISTATIC_HandleEvent(pMe->m_staticBox,
                eCode,  wParam, dwParam)) 
            {
                return TRUE;
            }
    
            if (pMe->m_softKey && IMENUCTL_HandleEvent(pMe->m_softKey,
                eCode,  wParam, dwParam)) 
            {
                return TRUE;
            }
            break;
    }
    
    return TRUE;
}


/*=============================================================================
FUNCTION:  CImportApp_InitImportTable

DESCRIPTION: Initialize the item import table.

PARAMETERS:
   *pMe: CImportApp object pointer

RETURN VALUE:
   boolean: TRUE on success

COMMENTS:
   This function is required because dynamic BREW applets cannot have
   static data.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_InitImportTable(CImportApp *pMe)
{
    int i;

    pMe->m_importTbl = 
        MALLOC(sizeof(ItemImportTableEntry)*(IMPORTITEM_COUNT+1));
    if (NULL == pMe->m_importTbl) 
    {
        return FALSE;
    }

    i = 0;

    pMe->m_importTbl[i].item   = IMPORTITEM_NAME;
    pMe->m_importTbl[i++].func = CImportApp_ImportItemCB_Name;
    pMe->m_importTbl[i].item   = IMPORTITEM_BIZCONT;
    pMe->m_importTbl[i++].func = CImportApp_ImportItemCB_BizContact;
    pMe->m_importTbl[i].item   = IMPORTITEM_PERCONT;
    pMe->m_importTbl[i++].func = CImportApp_ImportItemCB_PerContact;
    pMe->m_importTbl[i].item   = IMPORTITEM_DEFCONT;
    pMe->m_importTbl[i++].func = CImportApp_ImportItemCB_DefContact;
    pMe->m_importTbl[i].item   = IMPORTITEM_GENNUM;
    pMe->m_importTbl[i++].func = CImportApp_ImportItemCB_HomeNumber;
    pMe->m_importTbl[i].item   = IMPORTITEM_HOMENUM;
    pMe->m_importTbl[i++].func = CImportApp_ImportItemCB_HomeNumber;
    pMe->m_importTbl[i].item   = IMPORTITEM_WORKNUM;
    pMe->m_importTbl[i++].func = CImportApp_ImportItemCB_WorkNumber;
    pMe->m_importTbl[i].item   = IMPORTITEM_MOBILENUM;
    pMe->m_importTbl[i++].func = CImportApp_ImportItemCB_MobileNumber;
    pMe->m_importTbl[i].item   = IMPORTITEM_RUIMNUM;
    pMe->m_importTbl[i++].func = CImportApp_ImportItemCB_RUIMNumber;
    pMe->m_importTbl[i].item   = IMPORTITEM_ADDRESS;
    pMe->m_importTbl[i++].func = CImportApp_ImportItemCB_Address;
    pMe->m_importTbl[i].item   = IMPORTITEM_URL;
    pMe->m_importTbl[i++].func = CImportApp_ImportItemCB_URL;
    pMe->m_importTbl[i].item   = IMPORTITEM_EMAIL;
    pMe->m_importTbl[i++].func = CImportApp_ImportItemCB_Email;
    pMe->m_importTbl[i].item   = IMPORTITEM_NEXTREC;
    pMe->m_importTbl[i++].func = CImportApp_ImportItemCB_NextRec;

    pMe->m_importTbl[i].item   = NULL;
    pMe->m_importTbl[i].func   = NULL;

    // If this is FALSE then we probably just corrupted some memory...
    ASSERT(i == IMPORTITEM_COUNT); //lint !e774

    return TRUE;
}


/*=============================================================================
FUNCTION: CImportApp_TrimStr

DESCRIPTION: Trims leading and trailing spaces from an ASCIIZ string

PARAMETERS:
   *src: Source string
   *dst: Destination buffer
   dstSize: Size of dst in bytes

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CImportApp_TrimStr(char *src, char *dst, uint16 dstSize)
{
    uint32 srcLen,
    srcPos,
    dstPos;

    if (NULL == src) 
    {
        return;
    }

    if (NULL == dst) 
    {
        return;
    }

    srcLen = STRLEN(src);

    srcPos = dstPos = 0;

    // trim leading spaces
    while (' ' == src[srcPos]) 
    {
        ++srcPos;

        if (srcPos >= srcLen) 
        {
            dst[0] = 0;
            return;
        }
    }

    // src is too large for dst
    if (srcLen - srcPos >= dstSize) 
    {
        dst[0] = 0;
        return;
    }

    // copy the rest to src to dst
    MEMCPY(dst,  &src[srcPos], (srcLen - srcPos) + 1);


    // trim trailing spaces
    for (dstPos = STRLEN(dst); (dstPos > 0) && (' ' == dst[dstPos-1]); dstPos--)
        ;

    dst[dstPos] = '\0';
}


/*=============================================================================
FUNCTION:  CImportApp_CheckNumber
 
DESCRIPTION: Ensures that the specified number contains valid digits
 
PARAMETERS:
   *num: number to check
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CImportApp_CheckNumber(AECHAR *num)
{
    while (*num) 
    {
        if ( ( (*num < (AECHAR)'0') && (*num > (AECHAR)'9') ) &&
            (*num != (AECHAR)'#') &&
            (*num != (AECHAR)'*') &&
            (*num != (AECHAR)'P') ) 
        {
            *num = '\0';
            return;
        }

        num++;
    }
}

/*=============================================================================
FUNCTION: CImportApp_ReadLine

DESCRIPTION: Read a single line of ASCII text from a file.

PARAMETERS:
   *pIFile: IFile interface pointer
   *szBuf: Buffer to use (assumed to be at least MAX_LINE_LENGTH long)

RETURN VALUE:
    char *: Read line, NULL if at EOF (or I/O error)

COMMENTS:

SIDE EFFECTS:
   Upon calling this function, the file pointer is assumed to be at the
   start of the line.  When the function returns, the file pointer will
   be at the start of the next line.

SEE ALSO:

=============================================================================*/
static char *CImportApp_ReadLine(char *szBuf, IFile *pIFile)
{
    uint32      pos;
    char        ch;

    pos = 0;
    while (1 == IFILE_Read(pIFile, &ch, 1)) 
    {
        // no NULLs allowed
        if (0 == ch) 
        {
            break;
        }

        // EOL
        if ('\n' == ch) 
        {
            break;
        }

        switch (ch) 
        {
            case '\r':  // ignore annoying ^M characters
                break;
                
            case '\t':
                    ch = ' ';
                    
            /*lint -fallthrough*/
            default:
                szBuf[pos++] = ch;
                break;
        }

        // line is too long
        if (pos >= MAX_LINE_LENGTH) 
        {
            return NULL;
        }
    }

    szBuf[pos] = 0;

    if (pos > 0) 
    {
        return szBuf;
    } 
    else 
    {
        return NULL;
    }
}


/*=============================================================================
FUNCTION:  CImportApp_ImportItem

DESCRIPTION: Import a single contact database item

PARAMETERS:
   *pMe: CImportApp object pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   Sets the CImportApp::m_opCompleted flag if there are no more
   items to import.

SEE ALSO:

=============================================================================*/
static void CImportApp_ImportItem(CImportApp *pMe)
{
    char          *szRawLine;
    char          *szRawData;
    int            i;
    boolean        eof;

    pMe->m_szItemData = pMe->m_szCookedData;
    pMe->m_wItemData = pMe->m_wCookedData;

    eof = FALSE;

    szRawLine = CImportApp_ReadLine(pMe->m_szBuf, pMe->m_pIFile);
    
    if (NULL == szRawLine) 
    {
        eof = TRUE;
        (void) SPRINTF(pMe->m_szMsgBuf,
                "%d contacts imported.  The source file, "
                CONTACTS_EFS_FILE ", has been deleted from EFS.",
                pMe->m_recordCount);
    } 
    else 
    {

        // Look for the item:data separator
        szRawData = STRCHR(szRawLine, ':');
        if (szRawData != NULL) 
        {
            *szRawData = '\0';
            ++szRawData;

            // trim spaces from the item data
            pMe->m_szCookedData[0] = 0;
            CImportApp_TrimStr(szRawData,
                    pMe->m_szCookedData,
                    sizeof(pMe->m_szCookedData));
        }

        // figure out what kind of item we've got
        for (i = 0; pMe->m_importTbl[i].func != NULL; i++) 
        {
            if (CImportApp_StrStr(szRawLine, pMe->m_importTbl[i].item)) 
            {
                STR_TO_WSTR(pMe->m_szCookedData,
                        pMe->m_wCookedData,
                        sizeof(pMe->m_wCookedData));

                // call the handler function for this item
                if (!pMe->m_importTbl[i].func(pMe)) 
                {
                    (void) SPRINTF(pMe->m_szMsgBuf,
                            "An error occured, %d contacts imported.",
                            pMe->m_recordCount);

                    eof = TRUE;
                }
                break;
            }
        }
    }

    if (eof) 
    {
        pMe->m_opCompleted = TRUE;

        IFILE_Release(pMe->m_pIFile);
        pMe->m_pIFile = NULL;

        (void) IFILEMGR_Remove(pMe->m_pIFileMgr,
                CONTACTS_EFS_FILE);
    }
}


/*=============================================================================
FUNCTION:  CImportApp_ExportRecord

DESCRIPTION: Exports a single database record from the contact database

PARAMETERS:
   *pMe: CImportApp object pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   Sets the CImportApp::m_opCompleted flag if there are no more
   items to export.

SEE ALSO:

=============================================================================*/
static void CImportApp_ExportRecord(CImportApp *pMe)
{
    AEEAddrField     fld;
    byte        *fldBuff;
    char        *fieldStr;
    int          nTotalFields=0;
    int          j=0;
    boolean      bNameFound = FALSE;
    uint16       wCategory = AEE_ADDR_CAT_NONE;

    for ( ; ;) 
    {
        pMe->m_rec = IADDRBOOK_EnumNextRec(pMe->m_addrBook);

        if (NULL == pMe->m_rec) 
        {
            pMe->m_opCompleted = TRUE;

            (void) SPRINTF(pMe->m_szMsgBuf,
                    "%d contacts exported to the EFS file "
                    CONTACTS_EFS_FILE,
                    pMe->m_recordCount);
            return;
        }

#ifdef FEATURE_FDN
        // Don't export FDN entries...
        if (AEE_ADDR_CAT_RUIM_FDN == IADDRREC_GetCategory(pMe->m_rec)) 
        {
            IADDRREC_Release(pMe->m_rec);
            pMe->m_rec = NULL;
        } 
        else 
        {
            // Found an entry we are interested in, exit the for() loop
            break;
        }
#else
        break;
#endif /*FEATURE_FDN*/
    }

    nTotalFields = IADDRREC_GetFieldCount(pMe->m_rec);
    
    for (j=0;j<nTotalFields;j++) 
    {
        AEEAddrField *addrField = IADDRREC_GetField(pMe->m_rec,j);
        MEMCPY(&fld,addrField,sizeof(AEEAddrField));

        fldBuff= fld.pBuffer;

        if (NULL == fldBuff) 
        {
            continue;
        }

        if ( (fld.fType != AEEDB_FT_STRING) &&
            (fld.fType != AEEDB_FT_PHONE) ) 
        {
            // Don't know how to handle this field type so ignore it
            // and move to the next one
            continue;
        }

        WSTR_TO_STR((AECHAR*)(void *) fldBuff,
            pMe->m_szBuf96,
            sizeof(pMe->m_szBuf96));

        if (fld.fID == AEE_ADDRFIELD_NAME) 
        {
            if (bNameFound == FALSE) 
            {
                bNameFound = TRUE;
                SPRINTF(pMe->m_szBuf,
                        IMPORTITEM_NAME ": %s" NL,
                        pMe->m_szBuf96);
                (void) IFILE_Write(pMe->m_pIFile,
                        pMe->m_szBuf,
                        STRLEN(pMe->m_szBuf));
                wCategory = IADDRREC_GetCategory(pMe->m_rec);
                
#ifdef FEATURE_RUIM_PHONEBOOK
                if (AEE_ADDR_CAT_RUIM_ADN != wCategory)
#endif // FEATURE_RUIM_PHONEBOOK
                {
                    switch (wCategory) 
                    {
                        case AEE_ADDR_CAT_BUSINESS:
                            STRCPY(pMe->m_szBuf, IMPORTITEM_BIZCONT);
                            break;
                            
                        case AEE_ADDR_CAT_PERSONAL:
                            STRCPY(pMe->m_szBuf, IMPORTITEM_PERCONT);
                            break;
                            
                        default:
                            STRCPY(pMe->m_szBuf, IMPORTITEM_DEFCONT);
                            break;
                    }

                    STRCAT(pMe->m_szBuf, NL);

                    (void) IFILE_Write(pMe->m_pIFile,
                            pMe->m_szBuf,
                            STRLEN(pMe->m_szBuf));
                }

                ++pMe->m_recordCount;
                (void) SPRINTF(pMe->m_szMsgBuf,
                        "Exporting: %s",
                        pMe->m_szBuf96);
                CImportApp_DrawScreen(pMe);

                continue;  // found full name, first pass is finished
            } 
            else 
            {
                // Do nothing with full name on second pass
            }
        } 
        else 
        {
            if (bNameFound == FALSE) 
            {
                //Ignore this address field if Name field has not been
                //found yet
                continue;
            }
            switch (fld.fID) 
            {
                case AEE_ADDRFIELD_PHONE_WORK:
                    fieldStr = IMPORTITEM_WORKNUM;
                    break;
                    
                case AEE_ADDRFIELD_PHONE_CELL:
                    fieldStr = IMPORTITEM_MOBILENUM;
                    break;
                    
                case AEE_ADDRFIELD_PHONE_HOME:
                    fieldStr = IMPORTITEM_HOMENUM;
                    break;
                    
                case AEE_ADDRFIELD_PHONE_GENERIC:
#ifdef FEATURE_RUIM_PHONEBOOK
                    if (AEE_ADDR_CAT_RUIM_ADN == wCategory) 
                    {
                        fieldStr = IMPORTITEM_RUIMNUM;
                    } 
                    else
#endif /* FEATURE_RUIM_PHONEBOOK*/
                    {
                        fieldStr = IMPORTITEM_GENNUM;
                    }
                    break;
                    
                case AEE_ADDRFIELD_ADDRESS:
                    fieldStr = IMPORTITEM_ADDRESS;
                    break;
                    
                case AEE_ADDRFIELD_URL:
                    fieldStr = IMPORTITEM_URL;
                    break;
                    
                case AEE_ADDRFIELD_EMAIL:
                    fieldStr = IMPORTITEM_EMAIL;
                    break;

                default:
                    fieldStr = NULL;
                    break;
            }

            if (fieldStr != NULL) 
            {
                STRCPY(pMe->m_szBuf, fieldStr);
                STRCAT(pMe->m_szBuf, ": ");
                STRCAT(pMe->m_szBuf, pMe->m_szBuf96);
                STRCAT(pMe->m_szBuf, NL);

                (void) IFILE_Write(pMe->m_pIFile,
                        pMe->m_szBuf,
                        STRLEN(pMe->m_szBuf));
            }
        }
    }

    IADDRREC_Release(pMe->m_rec);
    pMe->m_rec = NULL;


    STRCPY(pMe->m_szBuf, IMPORTITEM_NEXTREC);
    STRCAT(pMe->m_szBuf, NL);
    (void) IFILE_Write(pMe->m_pIFile, pMe->m_szBuf, STRLEN(pMe->m_szBuf));
}


/*=============================================================================
FUNCTION: CImportApp_StrStr

DESCRIPTION:
   Simple strstr() function.  Returns true if needle was
   found in haystack

PARAMETERS:
   *haystack: string in which the needle string is searched
   *needle: string to be searched

RETURN VALUE:
   boolean: TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_StrStr(char *haystack,
        char *needle
)
{
    char *n;

    if (needle == NULL || haystack == NULL) 
    {
        return FALSE;
    }

    n = needle;

    for (;;) 
    {
        if ('\0' == *n) 
        {
            return TRUE;
        }

        if ('\0' == *haystack) 
        {
            return FALSE;
        }

        if (*haystack == *n) 
        {
            n++;
        } 
        else 
        {
            n = needle;
        }

        haystack++;
    }
}



/*===========================================================================

FUNCTION: CImportApp_DrawScreen

DESCRIPTION
  Draws the screen. Basically updates the screen continuosly as the contacts
  are imported.
  
PARAMETERS:
  pMe: pointer to CImportApp Object
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void CImportApp_DrawScreen(CImportApp *pMe)
{
    AECHAR szText[MSGBUF_SIZE];

    STR_TO_WSTR(pMe->m_szMsgBuf,
                szText,
                sizeof(szText));

    (void) ISTATIC_SetText(pMe->m_staticBox,
                    NULL,
                    szText,
                    AEE_FONT_NORMAL,
                    AEE_FONT_NORMAL);
    (void) ISTATIC_Redraw(pMe->m_staticBox);
    (void) IMENUCTL_Redraw(pMe->m_softKey);
}


/*=============================================================================
FUNCTION: CImportApp_AddSoftKeyItem

DESCRIPTION: Add an item to a softkey control

PARAMETERS:
   *pSoftKey: pointer to IMenuCtl object
   *text: Text of the menu key
   nItemI: ItemID to add in the menu control

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void  CImportApp_AddSoftKeyItem (IMenuCtl   *pSoftKey,
        char       *szText,
        uint16      nItemID
)
{
    uint32  len = STRLEN(szText) + 1;
    AECHAR *wText;

    wText = MALLOC(sizeof(AECHAR) * len);

    STR_TO_WSTR(szText,
            wText,
            (int)(sizeof(AECHAR) * len));

    (void) IMENUCTL_AddItem(pSoftKey,
                NULL,
                0,
                nItemID,
                wText,
                0);

    FREE(wText);
}


/*=============================================================================
FUNCTION:  CImportApp_ImportTimerCB

DESCRIPTION: Called when the next record should be imported

PARAMETERS:
   *pUser: CImportApp object pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CImportApp_ImportTimerCB(void *pUser)
{
    CImportApp *pMe = (CImportApp*)pUser;

    (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                AEECLSID_APP_CONTACT_IMPORTER,
                EVT_USER_IMPORT,
                0,
                0);
}


/*=============================================================================
FUNCTION:  CImportApp_ExportTimerCB

DESCRIPTION: Called when the next record should be exported

PARAMETERS:
   *pUser: CImportApp object pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CImportApp_ExportTimerCB(void *pUser)
{
    CImportApp *pMe = (CImportApp*)pUser;

    (void) ISHELL_PostEvent(pMe->a.m_pIShell,
            AEECLSID_APP_CONTACT_EXPORTER,
            EVT_USER_EXPORT,
            0,
            0);
}

/*=============================================================================
FUNCTION:  CImportApp_ImportItemCB_Name

DESCRIPTION: Called when importing contact name from the text file

PARAMETERS:
   *pUser: CImportApp object pointer

RETURN VALUE:
   TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_ImportItemCB_Name(CImportApp *pMe)
{
    (void) SPRINTF(pMe->m_szMsgBuf,
    "Importing: %s",
    pMe->m_szItemData);

    CImportApp_DrawScreen(pMe);

    if (pMe->m_rec != NULL) 
    {
        IADDRREC_Release(pMe->m_rec);
        pMe->m_rec = NULL;
    }

    if (pMe->m_name != NULL) 
    {
        FREE(pMe->m_name);
    }
    pMe->m_name = WSTRDUP(pMe->m_wItemData);

    return TRUE;
}

/*=============================================================================
FUNCTION:  CImportApp_ImportItemCB_BizContact

DESCRIPTION: Called when importing business contact from the text file

PARAMETERS:
   *pUser: CImportApp object pointer

RETURN VALUE:
   TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_ImportItemCB_BizContact(CImportApp *pMe)
{
    AEEAddrField field;

    if (pMe->m_rec != NULL) 
    {
        return FALSE;
    }

    if (NULL == pMe->m_name) 
    {
        return FALSE;
    }

    field.fType    = AEEDB_FT_STRING;
    field.fID      = AEE_ADDRFIELD_NAME;
    field.wDataLen = (uint16)WSTRSIZE(pMe->m_name);
    field.pBuffer  = pMe->m_name;

    pMe->m_rec = IADDRBOOK_CreateRec(pMe->m_addrBook,
            AEE_ADDR_CAT_BUSINESS,
            &field, 1);

    FREE(pMe->m_name);
    pMe->m_name = NULL;

    if (pMe->m_rec != NULL) 
    {
        ++pMe->m_recordCount;
        return TRUE;
    }

    return FALSE;
}


/*=============================================================================
FUNCTION:  CImportApp_ImportItemCB_PerContact

DESCRIPTION: Called when importing Personal contact from the text file

PARAMETERS:
   *pUser: CImportApp object pointer

RETURN VALUE:
   TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_ImportItemCB_PerContact(CImportApp *pMe)
{
    AEEAddrField field;

    if (pMe->m_rec != NULL) 
    {
        return FALSE;
    }

    if (NULL == pMe->m_name) 
    {
        return FALSE;
    }

    field.fType    = AEEDB_FT_STRING;
    field.fID      = AEE_ADDRFIELD_NAME;

    field.wDataLen = (uint16)WSTRSIZE(pMe->m_name);
    field.pBuffer  = pMe->m_name;

    pMe->m_rec = IADDRBOOK_CreateRec(pMe->m_addrBook,
            AEE_ADDR_CAT_PERSONAL,
            &field, 1);

    FREE(pMe->m_name);
    pMe->m_name = NULL;

    if (pMe->m_rec != NULL) 
    {
        ++pMe->m_recordCount;
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION:  CImportApp_ImportItemCB_DefContact

DESCRIPTION: Called when importing default(neither business nor 
     Personal) contact from the text file

PARAMETERS:
   *pUser: CImportApp object pointer

RETURN VALUE:
   TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_ImportItemCB_DefContact(CImportApp *pMe)
{
    AEEAddrField field;

    if (pMe->m_rec != NULL) 
    {
        return FALSE;
    }

    if (NULL == pMe->m_name) 
    {
        return FALSE;
    }

    field.fType    = AEEDB_FT_STRING;
    field.fID      = AEE_ADDRFIELD_NAME;

    field.wDataLen = (uint16)WSTRSIZE(pMe->m_name);
    field.pBuffer  = pMe->m_name;

    pMe->m_rec = IADDRBOOK_CreateRec(pMe->m_addrBook,
            AEE_ADDR_CAT_NONE,
            &field, 1);

    FREE(pMe->m_name);
    pMe->m_name = NULL;

    if (pMe->m_rec != NULL) 
    {
        ++pMe->m_recordCount;
        return TRUE;
    }

    return TRUE;
}


/*=============================================================================
FUNCTION:  CImportApp_ImportItemCB_CB_HomeNumber

DESCRIPTION: Called when importing home number from text file

PARAMETERS:
   *pUser: CImportApp object pointer

RETURN VALUE:
   TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_ImportItemCB_HomeNumber(CImportApp *pMe)
{
    AEEAddrField field;

    if (NULL == pMe->m_rec) 
    {
        return FALSE;
    }

    CImportApp_CheckNumber(pMe->m_wItemData);

    field.fType    = AEEDB_FT_PHONE;
    field.fID      = AEE_ADDRFIELD_PHONE_HOME;

    field.wDataLen = (uint16)WSTRSIZE(pMe->m_wItemData);
    field.pBuffer  = pMe->m_wItemData;

    if (0 == field.wDataLen) 
    {
        return TRUE;
    }

    if (SUCCESS == IADDRREC_AddField(pMe->m_rec, &field)) 
    {
        return TRUE;
    }

    return FALSE;
}


/*=============================================================================
FUNCTION:  CImportApp_ImportItemCB_WorkNumber

DESCRIPTION: Called when importing work number from text file

PARAMETERS:
   *pUser: CImportApp object pointer

RETURN VALUE:
   TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_ImportItemCB_WorkNumber(CImportApp *pMe)
{
    AEEAddrField field;

    if (NULL == pMe->m_rec) 
    {
        return FALSE;
    }

    CImportApp_CheckNumber(pMe->m_wItemData);

    field.fType    = AEEDB_FT_PHONE;
    field.fID      = AEE_ADDRFIELD_PHONE_WORK;

    field.wDataLen = (uint16)WSTRSIZE(pMe->m_wItemData);
    field.pBuffer  = pMe->m_wItemData;

    if (0 == field.wDataLen) 
    {
        return TRUE;
    }

    if (SUCCESS == IADDRREC_AddField(pMe->m_rec, &field)) 
    {
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION:  CImportApp_ImportItemCB_MobileNumber

DESCRIPTION: Called when importing mobile number from text file

PARAMETERS:
   *pUser: CImportApp object pointer

RETURN VALUE:
   TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_ImportItemCB_MobileNumber(CImportApp *pMe)
{
    AEEAddrField field;

    if (NULL == pMe->m_rec) 
    {
        return FALSE;
    }

    CImportApp_CheckNumber(pMe->m_wItemData);

    field.fType    = AEEDB_FT_PHONE;
    field.fID      = AEE_ADDRFIELD_PHONE_CELL;

    field.wDataLen = (uint16)WSTRSIZE(pMe->m_wItemData);
    field.pBuffer  = pMe->m_wItemData;

    if (0 == field.wDataLen) 
    {
        return TRUE;
    }

    if (SUCCESS == IADDRREC_AddField(pMe->m_rec, &field)) 
    {
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION:  CImportApp_ImportItemCB_RUIMNumber

DESCRIPTION: Called when importing a R-UIM contact from the text file

PARAMETERS:
   *pUser: CImportApp object pointer

RETURN VALUE:
   TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_ImportItemCB_RUIMNumber(CImportApp *pMe)
{
    AEEAddrField  field[2];
    IAddrRec     *rec;

    if (NULL == pMe->m_name) 
    {
        return FALSE;
    }

    CImportApp_CheckNumber(pMe->m_wItemData);

    field[0].fType    = AEEDB_FT_STRING;
    field[0].fID      = AEE_ADDRFIELD_NAME;
    field[0].wDataLen = (uint16)WSTRSIZE(pMe->m_name);
    field[0].pBuffer  = pMe->m_name;

    field[1].fType    = AEEDB_FT_PHONE;
    field[1].fID      = AEE_ADDRFIELD_PHONE_GENERIC;
    field[1].wDataLen = (uint16)WSTRSIZE(pMe->m_wItemData);
    field[1].pBuffer  = pMe->m_wItemData;

#ifdef FEATURE_RUIM_PHONEBOOK
    rec = IADDRBOOK_CreateRec(pMe->m_addrBook,
            AEE_ADDR_CAT_RUIM_ADN,
            field, 2);
#else
    rec = IADDRBOOK_CreateRec(pMe->m_addrBook,
            AEE_ADDR_CAT_NONE,
            field, 2);
#endif /* FEATURE_RUIM_PHONEBOOK*/

    FREE(pMe->m_name);
    pMe->m_name = NULL;

    if (rec != NULL) 
    {
        ++pMe->m_recordCount;
        IADDRREC_Release(rec);
        return TRUE;
    }

    return FALSE;
}



/*=============================================================================
FUNCTION:  CImportApp_ImportItemCB_Address

DESCRIPTION: Called when importing address from text file

PARAMETERS:
   *pUser: CImportApp object pointer

RETURN VALUE:
   TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_ImportItemCB_Address(CImportApp *pMe)
{
    AEEAddrField field;

    if (NULL == pMe->m_rec) 
    {
        return FALSE;
    }

    field.fType    = AEEDB_FT_STRING;
    field.fID      = AEE_ADDRFIELD_ADDRESS;

    field.wDataLen = (uint16)WSTRSIZE(pMe->m_wItemData);
    field.pBuffer  = pMe->m_wItemData;

    if (SUCCESS == IADDRREC_AddField(pMe->m_rec, &field)) 
    {
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION:  CImportApp_ImportItemCB_URL

DESCRIPTION: Called when importing URL from text file

PARAMETERS:
   *pUser: CImportApp object pointer

RETURN VALUE:
   TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_ImportItemCB_URL(CImportApp *pMe)
{
    AEEAddrField field;

    if (NULL == pMe->m_rec) 
    {
        return FALSE;
    }

    field.fType    = AEEDB_FT_STRING;
    field.fID      = AEE_ADDRFIELD_URL;

    field.wDataLen = (uint16)WSTRSIZE(pMe->m_wItemData);
    field.pBuffer  = pMe->m_wItemData;

    if (SUCCESS == IADDRREC_AddField(pMe->m_rec, &field)) 
    {
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION:  CImportApp_ImportItemCB_Email

DESCRIPTION: Called when importing Email from text file

PARAMETERS:
   *pUser: CImportApp object pointer

RETURN VALUE:
   TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_ImportItemCB_Email(CImportApp *pMe)
{
    AEEAddrField field;

    if (NULL == pMe->m_rec) 
    {
        return FALSE;
    }

    field.fType    = AEEDB_FT_STRING;
    field.fID      = AEE_ADDRFIELD_EMAIL;

    field.wDataLen = (uint16)WSTRSIZE(pMe->m_wItemData);
    field.pBuffer  = pMe->m_wItemData;

    if (SUCCESS == IADDRREC_AddField(pMe->m_rec, &field)) 
    {
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION:  CImportApp_ImportItemCB_Address

DESCRIPTION: Called when importing a record is complete.

PARAMETERS:
   *pUser: CImportApp object pointer

RETURN VALUE:
   TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CImportApp_ImportItemCB_NextRec(CImportApp *pMe)
{
    PARAM_NOT_REF(pMe)

    return TRUE;
}

#endif /* FEATURE_APP_CONTACT_IMPORTER */
