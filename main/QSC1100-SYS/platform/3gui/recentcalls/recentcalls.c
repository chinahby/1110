/*=============================================================================

FILE: recentcalls.c
 
SERVICES:  Recent Calls applet
 
GENERAL DESCRIPTION:
 
PUBLIC CLASSES AND STATIC FUNCTIONS:
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:
 
(c) COPYRIGHT 2003-2006 QUALCOMM Incorporated.
                         All Rights Reserved.
 
                         QUALCOMM Proprietary


=============================================================================*/

/*=============================================================================

  $Header: //depot/asic/msmshared/apps/RecentCalls/recentcalls.c#35 $
$DateTime: 2006/05/30 10:07:06 $
 
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
05/24/06     JD      Added call to SelectFonts to increase size on VGA
12/15/05     JAS     Fix string for saving to UIM card
07/19/05     AT      Added a new Menu to reset the call timers.
03/09/05     sun     Fixed Compile Errors
11/09/04     JAS     Use BREW extended number in place of QCT (BREW 3.1.2.7)
10/29/04     PA      Support for sending SMS
10/01/04     JAS     Don't give "Save to RUIM" option when MMGSDI not enabled
09/24/04     PA      Added support for saving to RUIM
07/01/04     PA      Release interface objects on exiting.
06/07/04     AT      Removed the use of the dwparam for IMenuCtls.  Brew 2.1.3
                     has changed the meaning of this parameter, so this parameter
                     can no longer be used.
05/06/04     ak      Added feature flag for 1X targets
04/06/04     AT      Added dynamic PIN selection.  Fix LINT errors.
03/15/04     PA      Added a main menu for clear call history and removed delete entry
                     menu from call options
02/17/04     PA      Using NUMBER_EXT field in call history instead of NUMBER
01/22/04     tml     Featurized ICARD API changes
12/22/03     tml     ICARD API Changes
12/16/03     BPW     Added Brew 3.0 and UK field changes
11/25/03     BPW     Added error checking for Redial related functions
11/10/03     BPW     End App more cleanly after redial. 
11/10/03     AT      Implemented code review comments.  Added softkeys to all
                     the AOC screens, parameter and error checking, increased
                     debugging capabilities.
11/10/03     AT      Removed ChargeInfo menu item from CDMA only builds.
11/09/03     AT      Fixed compiler warnings/Additional testing.
11/08/03     AT      Added Advice of Charge screens.
10/21/03     BPW     Added flag to exit the app when it is not needed anymore
08/18/03     ADM     Added initialisation of the variable which stores time values
08/18/03     ADM     Fixed bug when if no calls are made time values should be zero
08/11/03     ADM     Added code for retrieving call time info
05/29/03     PA      Changes to support PBM call history
05/13/03     kar     Fixed compile warnings
04/24/03     kar     Initial revision
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "comdef.h"
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimfeatures.h"  // Simulator Feature Definitions
#endif

#include "AEE.h"
#include "AEEShell.h"
#include "AEEMenu.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "recentcalls_res.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"

#include "AppComFunc.h"
#include "AEETime.h"
#include "AEEDate.h"

#include "OEMText.h"

#include "AEECallHistoryExt.h"
#include "AEEConfig.h"
#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "cm.h"
#include "uixcm.h"
#endif

#ifdef FEATURE_APP_CONTACT
#include "ContApp.h"
#endif

#include "recentcalls.bid"
#include "recentcalls.h"
/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
#ifndef DIALER_PAUSE_AECHAR
#define DIALER_PAUSE_AECHAR  ((AECHAR)'P')
#endif

#undef PARAM_NOT_REF
#define PARAM_NOT_REF(x) /*lint -esym(715,x) */
/*lint -e740*/ /*Removed for BREW */
/*lint -e611*/ /*Removed for BREW */

#define MAX_TITLE_LENGTH   32 //max length of title text. Used in number dialog

// Max number of characters allowed in the MsgBox text
#define MSGBOX_MAXTEXTLEN  256

// Time (in ms) before a YesNo Box will close automatically
#define YESNO_TIMEOUT      6000

// Time (in ms) before a MsgBox will close automatically
#define MSGBOX_TIMEOUT     3000

//Maximum static control text length. Used in Date dialgo
#define MAX_STATIC_TEXT_LEN 12
#define MAX_PIN_STATIC_TEXT_LEN 64

//AOC Format Size and resulting string.
#define AOC_FMT_SIZE        20
#define AOC_STR_SIZE        (AOC_FMT_SIZE + 12)

// User inactivity Timeout (in milliseconds) before the redial screen 
// is automatically closed
#define TIMEOUT_MS_REDIAL_SCREEN               3000

// Time (in milliseconds) before the no entries screen is automatically closed
#define TIMEOUT_MS_NOENTRIES_SCREEN            2000

// Max entries per call list
#define MAX_LIST_ENTRIES 20

// Num of fields in call history
#define NUM_OF_FIELDS 5

// The size of the format string (hhh:mm:ss) 
#define FORMAT_SIZE 10                          // in AECHAR i.e. 16 bit units
#define FORMAT_SIZE_IN_BYTES (FORMAT_SIZE * 2)  // in bytes 

// Events used by this program.
#define EVT_RECENTCALLS_PIN_COMPLETE EVT_USER
#define EVT_RECENTCALLS_PIN_DISABLED (EVT_USER+1)

// The following ENUM is used to keep track of where we are in the
// PUK sequence of screens.   When the PIN is blocked, we first request
// the PUK code, next we request a new PIN code, and third we ask for
// the same new PIN code.  If the two new PIN codes do not match, we
// ask for those again.  We then send the request to ICARD to unblock
// the PIN.  When the command returns, if it fails, we then start the
// process all over.
enum puk_states
{
   RECENTCALLS_NOT_REQUESTING,          /* Not requesting a PUK */
   RECENTCALLS_REQUESTING_PUK,          /* Requesting the PUK code. */
   RECENTCALLS_REQUESTING_PIN_FIRST,    /* Requesting the first PIN */
   RECENTCALLS_REQUESTING_PIN_SECOND    /* Requesting verification. */
};

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/
typedef struct cRecentCalls_s CRecentCalls;
typedef boolean process_fptr(CRecentCalls *pMe, boolean passed);

struct cRecentCalls_s {
   AEEApplet  a;
   
   AEERect    m_rect;

   uint16     m_savenew;
   uint16     m_addto;
   uint16     m_time;

   uint16     m_selectedItem; //index element in the selected category
   boolean    m_idleTimer;
   int        m_lineHeight;
   int        m_largeLineHeight;

   // Keeps track of the active dialog Id when the applet is
   // suspended so the dialog can be restarted upon resume
   uint16     m_activeDialogId; 

   //Used in editing the phone number
   AECHAR     m_phoneNumber[CALLHISTORY_MAXDIGITS];

   // The resource id that the YesNo dialog box uses for it's title
   // (this should be set before creating a YesNo dialog box)
   uint16     m_yesnoDialogTitle;
   
   // True if the yesno dialog should timeout after YESNO_TIMEOUT (currently
   // set to 6 seconds ) 
   boolean    m_yesnoTimeout;
   
   // String resource to use for the Msgbox text
   uint16 m_msgBox_textId;

   // Keeps track of the dialog to which to return, after showing the 
   //MsgBox dialog or yes no dialog
   uint16     m_msgboxReturnDialogId; 

   // Number of outgoing entries, used by the redial dialog to
   // draw the scroll bar.
   uint8       m_numRedialEntries;

   // Calls category
   uint8       m_callsCategory;

   // Callback used to redraw the redial dialog
   AEECallback m_drawRedialCB;

   // TO DO - remove the foll. when PBM support comes in
   // Incoming list
   CallHistoryEntry m_IncomingList[MAX_LIST_ENTRIES];

   // Outgoing list
   CallHistoryEntry m_OutgoingList[MAX_LIST_ENTRIES];

   // Missed list
   CallHistoryEntry m_MissedList[MAX_LIST_ENTRIES];

   // Counts for the above lists
   uint32           m_nIncomingRecs;
   uint32           m_nOutgoingRecs;
   uint32           m_nMissedRecs;

   // Interfaces used for Advice of Charge.
#ifdef FEATURE_ICM
   ICM             *m_pICM;
#endif /*FEATURE_ICM*/
#ifdef FEATURE_NEW_ICARD
   ICard           *m_pICard;
#else
   ICARD           *m_pICard;
#endif /*FEATURE_NEW_ICARD */
#ifdef FEATURE_ICM
   AEECMPhInfo      m_sPh_info;
#endif
   // The following varaibles are used for verifying the PIN/PUK.
   uint8            m_PinID;              /* Which slot are we using. */
   process_fptr    *m_fpPIN2_complete_fn; /* Function called after PIN. */
   AECHAR           m_PUKstring[9];       /* Saved PUK value. */
   AECHAR           m_PINstringA[9];      /* Saved PIN value. */
#ifdef FEATURE_NEW_ICARD
   AEECardPinCmdStatus     m_sPinActionStatus;      /* Response from ICard. */
   AEECallback      m_sCallback;          /* Callback to give to ICard. */
#else
   AEEPINCbData     m_sPin_rsp_data;      /* Response from ICARD. */
   AEECallback      m_sCallback;          /* Callback to give to ICARD. */
#endif /*FEATURE_NEW_ICARD */
   enum puk_states  m_ePukState;          /* Where are we in the PUK prompts */
};

/*===========================================================================

                       FUNCTION PROTOTYPES

===========================================================================*/
static boolean RecentCalls_HandleEvent(CRecentCalls  *pi, 
                                       AEEEvent       eCode, 
                                       uint16         wParam, 
                                       uint32         dwParam);

static void CRecentCalls_Free(CRecentCalls *pMe);

#if defined(BREW_STATIC_APP)

static int  RecentCallsMod_CreateInstance(AEECLSID  ClsId,
                                          IShell   *pIShell,
                                          IModule  *po,
                                          void    **ppObj);

/*static*/ int  RecentCalls_Load(IShell   *ps, 
                             void     *pHelpers, 
                             IModule **pMod);



PFNMODENTRY RecentCalls_GetModInfo(IShell      *ps, 
                                   AEECLSID   **ppClasses, 
                                   AEEAppInfo **pApps, 
                                   uint16      *pnApps,
                                   uint16      *pwMinPriv);

#endif  //BREW_STATIC_APP

// Helper functions
static boolean RecentCalls_SetupDialogHandler(CRecentCalls *pMenu, 
                                              IDialog      *pDialog);

static boolean RecentCalls_MoveToDialog(CRecentCalls *pme, 
                                        uint16        id);

static void RecentCalls_AddCallList(IMenuCtl            *pCtl,
                                    word                 id,
                                    CallHistoryEntry *ent);

static void RecentCalls_PopulateNumbers(CRecentCalls *pMe,
                                        IMenuCtl    *pCtl);

static void RecentCalls_DrawRedialDialog(CRecentCalls *pMe);

static void RecentCalls_DisplayNumber(CRecentCalls *pMenu);

static void RecentCalls_DisplayTime(CRecentCalls *pMe);

static void FormatTimeForDisplay(AECHAR* fmtTime, int timeSec);

static void RecentCalls_DialogTimeout_Callback(void *pUser);

static boolean RecentCalls_GetList (CRecentCalls *pMe);

static boolean RecentCalls_GetEntry (CRecentCalls *pMe, uint8  nCat, byte nElement,
                                     CallHistoryEntry *ent);

static boolean RecentCalls_DeleteAll (CRecentCalls *pMe);

//
// IDialog event handlers
// 
static boolean RecentCalls_MainCategoryDlgHandler(CRecentCalls *pMe,
                                                  AEEEvent      eCode,
                                                  uint16        wParam,
                                                  uint32        dwParam);

static boolean RecentCalls_PhoneNumbersDlgHandler(CRecentCalls *pMe,
                                                  AEEEvent      eCode,
                                                  uint16        wParam,
                                                  uint32        dwParam);

static boolean RecentCalls_RCallsMenuDlgHandler(CRecentCalls   *pMe,
                                                AEEEvent        eCode,
                                                uint16          wParam,
                                                uint32          dwParam);

static boolean RecentCalls_DateTimeDlgHandler(CRecentCalls     *pMe,
                                              AEEEvent          eCode,
                                              uint16            wParam,
                                              uint32            dwParam);

static boolean RecentCalls_ViewNumberDlgHandler(CRecentCalls       *pMe,
                                                AEEEvent            eCode,
                                                uint16              wParam,
                                                uint32              dwParam);

static boolean RecentCalls_EditNumberDlgHandler(CRecentCalls       *pMe,
                                                AEEEvent            eCode,
                                                uint16              wParam,
                                                uint32              dwParam);

static boolean RecentCalls_YesNoDlgHandler(CRecentCalls       *pMe,
                                           AEEEvent            eCode,
                                           uint16              wParam,
                                           uint32              dwParam);

static boolean RecentCalls_MsgBoxDlgHandler(CRecentCalls       *pMe,
                                            AEEEvent            eCode,
                                            uint16              wParam,
                                            uint32              dwParam);

static boolean RecentCalls_RedialDlgHandler(CRecentCalls       *pMe,
                                            AEEEvent            eCode,
                                            uint16              wParam,
                                            uint32              dwParam);

static boolean RecentCalls_NoEntriesDlgHandler(CRecentCalls       *pMe,
                                               AEEEvent            eCode,
                                               uint16              wParam,
                                               uint32              dwParam);

static boolean RecentCalls_CallTimeDlgHandler(CRecentCalls       *pMe,
                                              AEEEvent            eCode,
                                              uint16              wParam,
                                              uint32              dwParam);

static boolean RecentCalls_AllCallTimeDlgHandler(CRecentCalls       *pMe,
                                                 AEEEvent            eCode,
                                                 uint16              wParam,
                                                 uint32              dwParam);

static boolean RecentCalls_AocInfoDlgHandler  (CRecentCalls       *pMe,
                                               AEEEvent            eCode,
                                               uint16              wParam,
                                               uint32              dwParam);

static boolean RecentCalls_AocCCMDlgHandler   (CRecentCalls       *pMe,
                                               AEEEvent            eCode,
                                               uint16              wParam,
                                               uint32              dwParam);

static boolean RecentCalls_AocACMDlgHandler   (CRecentCalls       *pMe,
                                               AEEEvent            eCode,
                                               uint16              wParam,
                                               uint32              dwParam);

static boolean RecentCalls_AocACMMAXDlgHandler(CRecentCalls       *pMe,
                                               AEEEvent            eCode,
                                               uint16              wParam,
                                               uint32              dwParam);

static boolean RecentCalls_AocSetACMMAXDlgHandler(CRecentCalls       *pMe,
                                                AEEEvent            eCode,
                                                uint16              wParam,
                                                uint32              dwParam);

static boolean RecentCalls_PIN2_RequestDlgHandler
                                              (CRecentCalls       *pMe,
                                               AEEEvent            eCode,
                                               uint16              wParam,
                                               uint32              dwParam);

/*===========================================================================

                      STATIC/LOCAL DATA

===========================================================================*/
static const AEEAppInfo gaiRecentCall = {
   AEECLSID_RECENTCALLSAPP,
   RECENTCALLS_RES_FILE,
   IDS_RECENTCALLS_TITLE,
   0,0,0,0,
   AFLAG_TOOL | AFLAG_HIDDEN
};

////
// This is a list of dialog event handers that correspond to the 
// dialogs in the recentcalls.bar file, indexed by dialog resource Id
//
// IMPORTANT!  Dialog resource IDs are assumed to start at 1 and
//             must not have gaps between successive IDs (ie. 1,2,4,5).
// 
static const PFNAEEEVENT sDialogEventHandlers[] =
{
   (PFNAEEEVENT) RecentCalls_MainCategoryDlgHandler, // IDD_MAIN_CATEGORY_DLG
   (PFNAEEEVENT) RecentCalls_PhoneNumbersDlgHandler, // IDD_PHONENUMBERS_DLG
   (PFNAEEEVENT) RecentCalls_RCallsMenuDlgHandler,   // IDD_RCALLSMENU_DLG
   (PFNAEEEVENT) RecentCalls_DateTimeDlgHandler,     // IDD_DATETIME_DLG
   (PFNAEEEVENT) RecentCalls_ViewNumberDlgHandler,   // IDD_VIEW_NUMBER_DLG
   (PFNAEEEVENT) RecentCalls_EditNumberDlgHandler,   // IDD_EDIT_NUMBER_DLG
   (PFNAEEEVENT) RecentCalls_YesNoDlgHandler,        // IDD_DELETE_YES_NO_DLG
   (PFNAEEEVENT) RecentCalls_MsgBoxDlgHandler,       // IDD_MSGBOX_DLG
   (PFNAEEEVENT) RecentCalls_RedialDlgHandler,       // IDD_REDIAL_DLG
   (PFNAEEEVENT) RecentCalls_NoEntriesDlgHandler,    // IDD_NOENTRIES_DLG
   (PFNAEEEVENT) RecentCalls_CallTimeDlgHandler,     // IDD_CALL_TIME_DLG
   (PFNAEEEVENT) RecentCalls_AllCallTimeDlgHandler,  // IDD_LAST_CALL_TIME_DLG
   (PFNAEEEVENT) RecentCalls_AllCallTimeDlgHandler,  // IDD_TOTAL_HOME_TIME_DLG
   (PFNAEEEVENT) RecentCalls_AllCallTimeDlgHandler,  // IDD_TOTAL_ROAM_TIME_DLG
   (PFNAEEEVENT) RecentCalls_AllCallTimeDlgHandler,  // IDD_TOTAL_TIME_DLG
   (PFNAEEEVENT) RecentCalls_AocInfoDlgHandler,      // IDD_AOC_INFO_DLG
   (PFNAEEEVENT) RecentCalls_AocCCMDlgHandler,       // IDD_CCM_INFO_DLG
   (PFNAEEEVENT) RecentCalls_AocACMDlgHandler,       // IDD_ACM_INFO_DLG
   (PFNAEEEVENT) RecentCalls_AocACMMAXDlgHandler,    // IDD_ACMMAX_INFO_DLG
   (PFNAEEEVENT) RecentCalls_AocSetACMMAXDlgHandler, // IDD_ACMMAX_SET_DLG
   (PFNAEEEVENT) RecentCalls_PIN2_RequestDlgHandler, // IDD_PIN2_REQUEST_DLG
};

#if defined(BREW_STATIC_APP)
/*===========================================================================

FUNCTION RecentCalls_GetModInfo

DESCRIPTION
  Retrieves information about the static module

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
PFNMODENTRY RecentCalls_GetModInfo(IShell      *ps, 
                                   AEECLSID   **ppClasses, 
                                   AEEAppInfo **pApps, 
                                   uint16      *pnApps,
                                   uint16      *pwMinPriv)
{
   PARAM_NOT_REF(ps)
   PARAM_NOT_REF(ppClasses)
   PARAM_NOT_REF(pwMinPriv)

   *pApps = (AEEAppInfo *)&gaiRecentCall;
   *pnApps = 1;
   return (PFNMODENTRY)RecentCalls_Load;
}

/*===========================================================================

FUNCTION RecentCalls_Load

DESCRIPTION
  Static fixed .LIB entrypoint for applet

PARAMETERS:
   *ps: pointer to IShell
   *pHelpers : 
   **pointer : pointer to IModule

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
/*static*/ int RecentCalls_Load(IShell   *ps, 
                            void     *pHelpers, 
                            IModule **pMod)
{
   return AEEStaticMod_New(sizeof(AEEMod),
                           ps,
                           pHelpers,
                           pMod,
                           RecentCallsMod_CreateInstance,
                           NULL);
}

#endif  //BREW_STATIC_APP

/*===========================================================================
FUNCTION RecentCalls_InitAppData

DESCRIPTION
  Initializing the application data member

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static boolean CRecentCalls_InitAppData(IApplet* po)
{
  CRecentCalls *pMe = (CRecentCalls *)po;
  int err;
  
  if (pMe == NULL)
    return FALSE;

  pMe->m_nIncomingRecs = 0;
  pMe->m_nOutgoingRecs = 0;
  pMe->m_nMissedRecs = 0;
#ifdef FEATURE_ICM
  /* Create the interfaces necessary for Advice of Charge. */
  err = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CM,
                              (void **)&pMe->m_pICM);
  if (err)
    return FALSE;
#endif /*FEATURE_ICM*/
  err = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CARD,
                              (void **)&pMe->m_pICard);
  if (err)
    return FALSE;

  /* Increase the font size */
  AppComFunc_SelectFonts(pMe->a.m_pIDisplay);
  
  // Read the Call History
  if (RecentCalls_GetList(pMe))
    return TRUE;
  return FALSE;
}

/*===========================================================================

FUNCTION RecentCallsMod_CreateInstance

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
#if defined(BREW_STATIC_APP)
static int RecentCallsMod_CreateInstance(AEECLSID  ClsId,
                                         IShell   *pIShell,
                                         IModule  *po,
                                         void    **ppObj)
#else
int AEEClsCreateInstance(AEECLSID  ClsId,
                         IShell   *pIShell,
                         IModule  *po,
                         void    **ppObj)
#endif
{
   *ppObj = NULL;

   if (ClsId != AEECLSID_RECENTCALLSAPP) {
      return ECLASSNOTSUPPORT;
   }

   if (AEEApplet_New(sizeof(CRecentCalls),
                     ClsId,
                     pIShell,
                     po,
                     (IApplet**)ppObj,
                     (AEEHANDLER)RecentCalls_HandleEvent,
                     (PFNFREEAPPDATA)CRecentCalls_Free)) {
         
      if(CRecentCalls_InitAppData((IApplet*)*ppObj) != FALSE) {
        return(AEE_SUCCESS);
      }
      IAPPLET_Release(((IApplet *) *ppObj)); //lint !e522
      *ppObj = NULL;
   }
  
   return EFAILED;
}

/*===========================================================================

FUNCTION CRecentCalls_Free

DESCRIPTION
  Releases application data

PARAMETERS:
   pMe: pointer to the CRecentCalls object 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
========================================================================*/

static void CRecentCalls_Free(CRecentCalls *pMe)
{
  pMe->m_nIncomingRecs = 0;
  pMe->m_nOutgoingRecs = 0;
  pMe->m_nMissedRecs = 0;

#ifdef FEATURE_ICM
  if (pMe->m_pICM) {
    ICM_Release(pMe->m_pICM); //lint !e522
    pMe->m_pICM = NULL;
  }
#endif

  if (pMe->m_pICard) {
    ICARD_Release(pMe->m_pICard); //lint !e522
    pMe->m_pICard = NULL;
  }

}   

/*===========================================================================

FUNCTION ParseHistoryRecord

DESCRIPTION
   copy call history information into local buffer.  Only specific fields.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void ParseHistoryRecord(CRecentCalls *pMe, AEECallHistoryField *pItems, 
                               int nItemCount )
{
  int i;
  CallHistoryEntry *ent = NULL;

  if (nItemCount <= 0)
     return;

  ent = (CallHistoryEntry *) MALLOC(sizeof(CallHistoryEntry));
  if (ent == NULL) {
    MSG_ERROR ("Memory allocation failed", 0, 0, 0);
    return;
  }

  for (i=0; i<nItemCount; i++ )
  {
    if ((pItems[i].ClsId != AEECLSID_RECENTCALLSAPP) &&
        (pItems[i].ClsId != 0)) {
      continue;
    }
    switch (pItems[i].wID)
    {
        case AEECALLHISTORY_FIELD_CALL_DURATION:
          (void) MEMCPY(&ent->duration, pItems[i].pData, pItems[i].wDataLen);
          break;

        case AEECALLHISTORY_FIELD_DATE_TIME:
          (void) MEMCPY(&ent->time_stamp, pItems[i].pData, pItems[i].wDataLen);
          break;

        case AEECALLHISTORY_FIELD_NAME:
          (void) MEMCPY (ent->letters, pItems[i].pData, 
                  MIN(ARR_SIZE(ent->letters)*sizeof(AECHAR), pItems[i].wDataLen));
          break;

        case AEECALLHISTORY_FIELD_NUMBER_EX:
          (void) MEMCPY (ent->digits, pItems[i].pData, 
                  MIN(ARR_SIZE(ent->digits)*sizeof(AECHAR), pItems[i].wDataLen));
          break;

        case AEECALLHISTORY_FIELD_CALL_TYPE:
          ent->status = *((uint8*)pItems[i].pData);
          break;

        default:
          break;
    }
  }

  switch (ent->status) {
    case AEECALLHISTORY_CALL_TYPE_TO:
      if (pMe->m_nOutgoingRecs == MAX_LIST_ENTRIES) {
        pMe->m_nOutgoingRecs = 0;
      }
      (void) MEMCPY(&pMe->m_OutgoingList[pMe->m_nOutgoingRecs++],ent, 
            sizeof(CallHistoryEntry));
      break;
    case AEECALLHISTORY_CALL_TYPE_FROM:
      if (pMe->m_nIncomingRecs == MAX_LIST_ENTRIES) {
        pMe->m_nIncomingRecs = 0;
      }
      (void) MEMCPY(&pMe->m_IncomingList[pMe->m_nIncomingRecs++],ent,
            sizeof(CallHistoryEntry));
      break;
    case AEECALLHISTORY_CALL_TYPE_MISSED:
      if (pMe->m_nMissedRecs == MAX_LIST_ENTRIES) {
        pMe->m_nMissedRecs = 0;
      }
      (void) MEMCPY(&pMe->m_MissedList[pMe->m_nMissedRecs++],ent, 
              sizeof(CallHistoryEntry));
      break;
    default:
      MSG_ERROR ("Invalid status type", 0,0,0);
      break;
  }
  FREE (ent);
}


/*=========================================================================

FUNCTION RecentCalls_GetList


DESCRIPTION
  Reads the entire call history into cache

PARAMETERS:
   pMe [in] - Pointer to RecentCalls object.

DEPENDENCIES
  none

RETURN VALUE
  TRUE - if successful
  FALSE - otherwise

SIDE EFFECTS
  none
===========================================================================*/
static boolean RecentCalls_GetList (CRecentCalls *pMe)
{
  ICallHistory *pICallHist;
  int err,status;
  const AEECallHistoryEntry *pCallHistoryEntry = NULL;       
  AEECallHistoryField *pItems = NULL;
  int nItemCount = 0;

  err = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CALLHISTORY,(void **)&pICallHist);
  if((err != SUCCESS) || (pICallHist==NULL))
  {
    MSG_ERROR ("Failed to create instance of call hist : %d",err, 0, 0);
    return FALSE;
  }
  // Enumerate through each item
  (void) ICALLHISTORY_EnumInit(pICallHist);

  pItems = NULL;
  while (((pCallHistoryEntry = ICALLHISTORY_EnumNext(pICallHist, &status)) != 0) && 
         (status == SUCCESS))
  {
    pItems = pCallHistoryEntry->pFields;
    nItemCount = pCallHistoryEntry->wNumFields;
    if (pItems) 
    {
      // Save the record to the appropriate cache
      ParseHistoryRecord( pMe, pItems, nItemCount );
    }
  }
  ICALLHISTORY_Release (pICallHist); //lint !e522

  return TRUE;
}

/*=========================================================================

FUNCTION RecentCalls_AddEntry


DESCRIPTION
  Adds an entry to the call history

PARAMETERS:
   nCat    [in]: Category of the entry MISSED CALLS, INCOMING CALLS,
                    OUTGOING CALLS
   *ent [in]: Contents of the call history item to be added.



DEPENDENCIES
  none

RETURN VALUE
  TRUE - if successful
  FALSE - otherwise

SIDE EFFECTS
  none
===========================================================================*/
boolean RecentCalls_AddEntry (IShell *pShell, CallHistoryEntry *ent)
{
  ICallHistory *pICallHist;
  int err;
  AEECallHistoryField Items[NUM_OF_FIELDS];
  AEECallHistoryEntry Entry;

  if ((ent == NULL) || (pShell == NULL)) {
    // Nothing to do 
    return FALSE;
  }

  err = ISHELL_CreateInstance(pShell, AEECLSID_CALLHISTORY,(void **)&pICallHist);
  if((err != SUCCESS) || (pICallHist==NULL))
  {
    MSG_ERROR ("Failed to create instance of call hist : %d",err, 0, 0);
    return FALSE;
  }

  // Status
  Items[0].ClsId = 0;
  Items[0].wID = AEECALLHISTORY_FIELD_CALL_TYPE;
  Items[0].wDataLen = sizeof(ent->status);
  Items[0].pData = (void *) &(ent->status);

  // Name
  Items[1].ClsId = 0;
  Items[1].wID = AEECALLHISTORY_FIELD_NAME;
  Items[1].wDataLen = ARR_SIZE (ent->letters) * sizeof(AECHAR);
  Items[1].pData = ent->letters;

  // Number
  Items[2].ClsId = 0;
  Items[2].wID = AEECALLHISTORY_FIELD_NUMBER;
  Items[2].wDataLen = ARR_SIZE (ent->digits) * sizeof(AECHAR);
  Items[2].pData = ent->digits;

  // Duration
  Items[3].ClsId = 0;
  Items[3].wID = AEECALLHISTORY_FIELD_CALL_DURATION;
  Items[3].wDataLen = sizeof(ent->duration);
  Items[3].pData = (void  *) &(ent->duration);

  // Date & Time
  Items[4].ClsId = 0;
  Items[4].wID = AEECALLHISTORY_FIELD_DATE_TIME;
  Items[4].wDataLen = sizeof(ent->time_stamp);
  Items[4].pData = (void *) &(ent->time_stamp);

  Entry.pFields = (AEECallHistoryField *) Items;
  Entry.wNumFields = NUM_OF_FIELDS;

  // Add to Call History
  if ((err = ICALLHISTORY_AddEntry (pICallHist, &Entry)) != SUCCESS) {
    DBGPRINTF ("Could not add entry", err);
  }
  ICALLHISTORY_Release (pICallHist); //lint !e522
  if (err == SUCCESS)
    return TRUE;
  return FALSE;

}

/*=========================================================================

FUNCTION RecentCalls_GetEntry


DESCRIPTION
  Gets an entry from the call history

PARAMETERS:
   nCat    [in]: Category of the entry MISSED CALLS, INCOMING CALLS,
                    OUTGOING CALLS
   nElement[in] - index of the element in this category. 
   *ent [out]: Returns the contents of the next call history item.



DEPENDENCIES
  none

RETURN VALUE
  TRUE - if successful
  FALSE - otherwise

SIDE EFFECTS
  none
===========================================================================*/
static boolean RecentCalls_GetEntry (CRecentCalls *pMe, uint8  nCat, byte nElement,
                                     CallHistoryEntry *ent)
{

  switch (nCat) {
    case AEECALLHISTORY_CALL_TYPE_TO:
      if (nElement < pMe->m_nOutgoingRecs) {
        (void) MEMCPY (ent, &pMe->m_OutgoingList[nElement], sizeof (CallHistoryEntry));
        return TRUE;
      }
      return FALSE;
    case AEECALLHISTORY_CALL_TYPE_FROM:
      if (nElement < pMe->m_nIncomingRecs) {
        (void) MEMCPY (ent, &pMe->m_IncomingList[nElement], sizeof (CallHistoryEntry));
        return TRUE;
      }
      return FALSE;
    case AEECALLHISTORY_CALL_TYPE_MISSED:
      if (nElement < pMe->m_nMissedRecs) {
        (void) MEMCPY (ent, &pMe->m_MissedList[nElement], sizeof (CallHistoryEntry));
        return TRUE;
      }
      return FALSE;
    default:
      break;
  }
  return FALSE;
}

/*=========================================================================

FUNCTION RecentCalls_DeleteAll


DESCRIPTION
  Deletes all items from the call history

PARAMETERS:
   None

DEPENDENCIES
  none

RETURN VALUE
  TRUE - if successful
  FALSE - otherwise

SIDE EFFECTS
  none
===========================================================================*/
static boolean RecentCalls_DeleteAll (CRecentCalls *pMe)
{
  ICallHistory *pICallHist;
  int err;

  err = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CALLHISTORY,(void **)&pICallHist);
  if((err != SUCCESS) || (pICallHist==NULL))
  {
    MSG_ERROR ("Failed to create instance of call hist : %d",err, 0, 0);
    return FALSE;
  }

  err = ICALLHISTORY_Clear(pICallHist);
  ICALLHISTORY_Release (pICallHist); //lint !e522

  pMe->m_nIncomingRecs = 0;
  pMe->m_nOutgoingRecs = 0;
  pMe->m_nMissedRecs = 0;

  if (err == SUCCESS) {
    return TRUE;
  }
  return FALSE;

}


/*=========================================================================

FUNCTION CRecentCalls_CallNumber


DESCRIPTION
  Uses the Idle applet to initiate a call

PARAMETERS:
   pMe: pointer to the CRecentCalls object 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static boolean RecentCalls_CallNumber(CRecentCalls *pMe)
{
   CallHistoryEntry  ent;
   char number[CALLHISTORY_MAXDIGITS];

   if (pMe->m_selectedItem > 0xFF) {
     MSG_ERROR ("Invalid selected Item.", 0, 0, 0);
     return FALSE;
   }
   // Get the entry from NV MO entries
   if (!RecentCalls_GetEntry(pMe, pMe->m_callsCategory,
                             (byte) pMe->m_selectedItem, 
                             &ent)) {
      return FALSE;
   }

   // originate call
   memset(number, 0, sizeof(number));
   (void) WSTRTOSTR(ent.digits, number, CALLHISTORY_MAXDIGITS);
   (void) OriginateVoiceCall (number);
   return TRUE;

}

/*=========================================================================

FUNCTION CRecentCalls_SaveNumber


DESCRIPTION
   Uses the Contact applet to save a number

PARAMETERS:
   pMe:      pointer to the CRecentCalls object
   saveType: Save option

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static boolean RecentCalls_SaveNumber(CRecentCalls *pMe, 
                                      PhoneNumberSaveType  saveType)
{
#ifdef FEATURE_APP_CONTACT
   CallHistoryEntry  ent;
   IContactApp         *ca = NULL;

   if (!pMe) {
      MSG_ERROR("Unexpected NULL pointer", 0, 0, 0);
      return FALSE;
   }

   if (pMe->m_selectedItem > 0xFF) {
      // should never happen
      MSG_ERROR("Should never reach here", 0, 0, 0);
      return FALSE;
   }

   if (!RecentCalls_GetEntry(pMe, pMe->m_callsCategory,
                             (byte) pMe->m_selectedItem, 
                             &ent)) {
      MSG_ERROR("Unable to obtain selected item %d in category %d", 
                 pMe->m_selectedItem, pMe->m_callsCategory, 0);
      return FALSE; 
   }

   if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,
                                        AEECLSID_CONTACT_APP,
                                        (void**)&ca)) {

      (void) IContactApp_SaveNumber(ca,
                                   ent.digits,
                                   STARTMETHOD_SAVENUMBER_NUMBER_EDIT,
                                   saveType);
      IContactApp_Release(ca); //lint !e522
   } else {
      MSG_ERROR("Unable to create Contact App", 0, 0, 0);
      return FALSE;
   }

   return TRUE;

#else
   return FALSE;
#endif /* FEATURE_APP_CONTACT */
}

/*=============================================================================
FUNCTION:  RecentCalls_AddCallList
 
DESCRIPTION:
   Adds the entry to the menu control
   
PARAMETERS:
   *pCtl:  Menu control to which we will add the menu items
   id:     item id to which the menuitem will be added
   *ent:   CallHistoryEntry which contains the information 
           like name and number of the call history entry
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void RecentCalls_AddCallList(IMenuCtl            *pCtl,
                                    word                 id,
                                    CallHistoryEntry *ent)
{
   CtlAddItem  ai;
   AECHAR     *wPtr;
   int         j;

   if (ent == NULL)
   {
     MSG_ERROR("ent was set to NULL", 0, 0, 0);
     return;
   }

   // check name buffer.
   for (j = 0; ent->letters[j]; j++) {
      if (ent->letters[j] != ' ') {
         wPtr = ent->letters;
         break;
      }
   }

   //Initialize the CtlAddItem object
   ai.pszResText  = RECENTCALLS_RES_FILE;

   wPtr = FormatPhoneNumberForDisplay(ent->digits);
   if (wPtr != NULL) {
      ai.pText    = wPtr;
   } else {
      ai.pText    = ent->digits;
   }
   ai.pImage      = NULL;
   ai.wText       = 0;
   ai.wFont       = 0;

   // Replace the following two lines with the switch statement to display
   // per-call call types in Call History.
   ai.pszResImage = NULL;
   ai.wImage      = 0;

   ai.pszResImage = RECENTCALLS_RES_FILE;
   switch (ent->status) {
   case AEECALLHISTORY_CALL_TYPE_FROM:
      ai.wImage = IDB_INCOMING_TYPE;
      break;
   
   case AEECALLHISTORY_CALL_TYPE_TO:
      ai.wImage = IDB_OUTGOING_TYPE;
      break;
   
   case AEECALLHISTORY_CALL_TYPE_MISSED:
      ai.wImage = IDB_MISSED_TYPE;
      break;
   
   default:
      MSG_ERROR("ASSERT_NOT_REACHABLE", 0, 0, 0);
      break;
   }


   //Add the index id from the index stack. 
   ai.wItemID = id;
   ai.dwData  = ent->time_stamp;

   (void) IMENUCTL_AddItemEx(pCtl,
                             &ai);

   if (wPtr != NULL) {
      FREE(wPtr);
   }
}

/*=============================================================================
FUNCTION:  RecentCalls_PopulateNumbers
 
DESCRIPTION:
   populate the numbers in the Menucontrol
 
PARAMETERS:
  *pMe:  pointer to CRecentCalls applet
  *pCtl: pointer to Menu control
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void RecentCalls_PopulateNumbers(CRecentCalls *pMe,
                                        IMenuCtl     *pCtl)
{
   CallHistoryEntry  ent;
   IShell              *ps = pMe->a.m_pIShell;
   uint16               TitleID = IDS_MISSED_CALLS;
   byte                 nElement=0;
   //sanity check
   if(!pMe || !pCtl || !ps){
      return;
   }
   //Find the category and modify the start id and 
   switch(pMe->m_callsCategory){
      case AEECALLHISTORY_CALL_TYPE_MISSED:
         TitleID = IDS_MISSED_CALLS;
         break;

      case AEECALLHISTORY_CALL_TYPE_FROM:
         TitleID = IDS_INCOMING_CALLS;
         break;
      
      case AEECALLHISTORY_CALL_TYPE_TO:
         TitleID = IDS_OUTGOING_CALLS;
         break;

      default:
         MSG_ERROR("Unknown category", 0, 0, 0);
         TitleID = IDS_OUTGOING_CALLS;
         break;
   }
   (void) IMENUCTL_SetTitle(pCtl,
                            RECENTCALLS_RES_FILE,
                            TitleID,
                            NULL);

   (void) IMENUCTL_DeleteAll(pCtl);

   for (;;) {
      
      //If it could not fetch the entry, means all further
      //entries may not be useful in this category
      if(RecentCalls_GetEntry(pMe, pMe->m_callsCategory,
                              nElement, 
                              &ent)==FALSE){
         break;
      }
      RecentCalls_AddCallList(pCtl,
                              nElement,
                              &ent);
      nElement++;
   }


   SetDefaultMenuLook(ps,
                      pCtl);

   IMENUCTL_SetActive(pCtl,
                      TRUE);

   (void) IMENUCTL_Redraw(pCtl);
}


/*=============================================================================
FUNCTION: RecentCalls_DrawRedialDialog

DESCRIPTION:  Draws the redial dialog 

PARAMETERS:
  *pMe  [in]:  Recent Calls applet object pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void RecentCalls_DrawRedialDialog(CRecentCalls *pMe)
{
   IDialog             *pDialog;
   IStatic             *pName,
                       *pNumber;
   AEERect              rect;
   AECHAR               wFormatStr[32];
   AECHAR               wText[32];
   AECHAR              *redialString; 
   boolean              freeRedialString;
   int                  bOffset;
   CallHistoryEntry     ent;
   AEEDeviceInfo        di;

   if(pMe == NULL){
     MSG_ERROR("Null Pointer", 0, 0, 0);
     return;
   }

   pDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
   if(pDialog == NULL){
      MSG_ERROR("Null Pointer", 0, 0, 0);
      return;
   }

   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);

   ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &di);
   
   // Check for no redial entries
   if (0 == pMe->m_numRedialEntries) {
      wText[0] = 0;
      (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  RECENTCALLS_RES_FILE,
                                  IDS_NO_OUTGOING_CALLS,
                                  wText,
                                  sizeof(wText));

      (void) IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                               AEE_FONT_NORMAL,
                               wText,
                               -1,
                               0,
                               0,
                               NULL,
                               IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);

      // Reset the dialog timeout timer.
      (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                             TIMEOUT_MS_REDIAL_SCREEN,
                             RecentCalls_DialogTimeout_Callback,
                             pMe);

      IDISPLAY_Update(pMe->a.m_pIDisplay);
      return;
   }

   if (!RecentCalls_GetEntry(pMe, AEECALLHISTORY_CALL_TYPE_TO,
                                   (uint8) pMe->m_selectedItem, 
                                   &ent)) {
      MSG_ERROR("Unable to retrieve call history entry", 0, 0, 0);
      return;
   }

   redialString = FormatPhoneNumberForDisplay(ent.digits);
   if (redialString != NULL) {
      freeRedialString = TRUE;
   } else {
      freeRedialString = FALSE;
      redialString = ent.digits;
   }

   pName     = (IStatic *) IDIALOG_GetControl(pDialog,
                                              IDC_REDIAL_NAME);
   pNumber   = (IStatic *) IDIALOG_GetControl(pDialog,
                                              IDC_REDIAL_NUMBER);
   if((pName == NULL) || (pNumber == NULL)){
     MSG_ERROR("Null Pointer", 0, 0, 0);
     return;
   }

   // 
   // Position the banner on the first line:
   //
   SETAEERECT(&rect,
              0,
              0,
              pMe->m_rect.dx - (di.cxScrollBar+1),
              pMe->m_lineHeight);

   wFormatStr[0] = 0;
   (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                               RECENTCALLS_RES_FILE,
                               IDS_REDIAL_BANNER_FMT,
                               wFormatStr,
                               sizeof(wFormatStr));

   WSPRINTF(wText, sizeof(wText), wFormatStr, pMe->m_selectedItem + 1);
   
   (void) IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                            AEE_FONT_BOLD,
                            wText,
                            -1,
                            rect.x,
                            rect.y,
                            &rect,
                            0);


   // 
   // Position name on second line
   //
   SETAEERECT(&rect,
              0,
              pMe->m_lineHeight,
              pMe->m_rect.dx - (di.cxScrollBar+1),
              pMe->m_largeLineHeight);

   ISTATIC_SetRect(pName,
                   &rect);

   (void) ISTATIC_SetText(pName, 
                          NULL, 
                          ent.letters,
                          AEE_FONT_NORMAL, 
                          AEE_FONT_LARGE);

   (void) ISTATIC_Redraw(pName);


   // 
   // Position number on second last line, to the right
   //
   bOffset = IDISPLAY_MeasureText(pMe->a.m_pIDisplay,
                                  AEE_FONT_BOLD,
                                  redialString);

   if (bOffset > (pMe->m_rect.dx - (di.cxScrollBar+1))) {
      bOffset = pMe->m_rect.dx - (di.cxScrollBar+1);
   }

   SETAEERECT(&rect,
              (pMe->m_rect.dx - (di.cxScrollBar+1)) - bOffset,
              pMe->m_rect.dy - 2 * pMe->m_lineHeight,
              bOffset,
              pMe->m_lineHeight);

   ISTATIC_SetRect(pNumber, &rect);

   (void) ISTATIC_SetText(pNumber, 
                          NULL, 
                          redialString, 
                          AEE_FONT_NORMAL, 
                          AEE_FONT_BOLD);

   (void) ISTATIC_Redraw(pNumber);


   if (freeRedialString) {
      FREE(redialString);
   }
   redialString = NULL;

   // 
   // Position Date/Time on last line
   //
   SETAEERECT(&rect,
              0,
              pMe->m_rect.dy - pMe->m_lineHeight,
              pMe->m_rect.dx - (di.cxScrollBar+1),
              pMe->m_lineHeight);

   //If timestamp is 0, then dont show the date and time.
   if (ent.time_stamp != 0) {
      IDateCtl            *pDate = NULL;
      IDateCtl            *pDatePick = NULL;
      ITimeCtl            *pTimeCtl = NULL;
      IConfig             *pConfig = NULL;
      JulianType           jdate;
      uint32               lDate, lMin;
      int                  nChars;

      GETJULIANDATE(ent.time_stamp, &jdate);

      (void) ISHELL_CreateInstance(pMe->a.m_pIShell, 
                                   AEECLSID_DATECTL, 
                                   (void **)(&pDate));
      (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                   AEECLSID_DATEPICKCTL, 
                                   (void **)(&pDatePick));

      //
      // Draw the call date on the right
      //
      if (pDate && pDatePick) {
      
         (void) IDATECTL_SetDate(pDate,
                                 jdate.wYear,
                                 jdate.wMonth,
                                 jdate.wDay);
         
         lDate = (uint32) IDATECTL_GetJulianDay(pDate);
         (void) IDATECTL_SetJulianDay(pDatePick, (int32) lDate);

         // MM/DD removed a year from the display
         (void) IDATECTL_GetDateString(pDatePick,
                                       wText,
                                       sizeof(wText),
                                       &nChars,
                                       DFMT_MM_DD_YYYY);
         // and, display 09/23 only.
         wText[nChars-3] = wText[nChars-1];
         wText[nChars-4] = wText[nChars-2];

         // Draw Date String
         (void) IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                                  AEE_FONT_NORMAL,
                                  wText,
                                  nChars-5,
                                  rect.x,
                                  rect.y,
                                  &rect,
                                  IDF_ALIGN_RIGHT);
      }

      if (pDatePick) {
         IDATECTL_Release(pDatePick); //lint !e522
      }

      if (pDate) {
         IDATECTL_Release(pDate); //lint !e522
      }

      //
      // Draw the call time on the left
      //
      (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                   AEECLSID_CONFIG,
                                   (void **)&pConfig);
      (void) ISHELL_CreateInstance(pMe->a.m_pIShell, 
                                   AEECLSID_CLOCKCTL, 
                                   (void **)(&pTimeCtl));

      if (pConfig && pTimeCtl) {
   
         lMin = 1000 * ( (jdate.wHour * 3600) +
                         (jdate.wMinute * 60) +
                         jdate.wSecond 
                       );

         GetTimeString(pConfig, pTimeCtl, pMe->a.m_pIShell, lMin, wText);

         (void) IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                                  AEE_FONT_NORMAL,
                                  wText,
                                  -1,
                                  rect.x,
                                  rect.y,
                                  &rect,
                                  IDF_ALIGN_LEFT);
      }

      if (pTimeCtl) {
         ITIMECTL_Release(pTimeCtl); //lint !e522
      }

      if (pConfig) {
         ICONFIG_Release(pConfig); //lint !e522
      }
   }

   // 
   // Draw the scroll bar
   // 
   if (pMe->m_numRedialEntries > 1) {
      int16 wScrollBarHeight;

      SETAEERECT(&rect,
                 pMe->m_rect.dx - di.cxScrollBar,
                 pMe->m_rect.y,
                 di.cxScrollBar,
                 pMe->m_rect.dy);

      IDISPLAY_FillRect(pMe->a.m_pIDisplay,
                        &rect, 
                        CLR_SYS_SCROLLBAR);
      IDISPLAY_FrameRect(pMe->a.m_pIDisplay, &rect);

      wScrollBarHeight = rect.dy - 2;


      rect.x  += 1;
      rect.dx -= 2;

      rect.dy = wScrollBarHeight / pMe->m_numRedialEntries;

      if (rect.dy < 3) {
         rect.dy = 3; // Minimum size of black part
      }

      if (rect.dy > wScrollBarHeight) {
         rect.dy = wScrollBarHeight;
      }

      if (pMe->m_selectedItem >= pMe->m_numRedialEntries - 1) {
         rect.y = pMe->m_rect.dy - rect.dy;
      } else {
         rect.y = (int16) (pMe->m_selectedItem * wScrollBarHeight / 
                           pMe->m_numRedialEntries);
      }


      if (rect.y < 0) {
         rect.y = 0;
      }

      if (rect.y + rect.dy > pMe->m_rect.dy) {
         rect.dy = pMe->m_rect.dy - rect.y;
      }

      IDISPLAY_FillRect(pMe->a.m_pIDisplay,
                        &rect, 
                        CLR_SYS_SCROLLBAR_FILL);
   }


   // Reset the dialog timeout timer.
   (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                          TIMEOUT_MS_REDIAL_SCREEN,
                          RecentCalls_DialogTimeout_Callback,
                          pMe);

   IDISPLAY_Update(pMe->a.m_pIDisplay);
}


/*=============================================================================
FUNCTION:  RecentCalls_DisplayNumber
 
DESCRIPTION:
 
PARAMETERS:
   *pMenu: pointer to CRecentCalls applet
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void RecentCalls_DisplayNumber( CRecentCalls *pMe )
{
   CallHistoryEntry  ent;
   IStatic             *pIStatic;
   AECHAR               title[MAX_TITLE_LENGTH]; 
   AEERect              tRect;
   IDialog             *pd;
   IMenuCtl            *pSK;
   AECHAR              *fmtNum;

   if (pMe->m_selectedItem > 0xFF) {
      // should never happen
      MSG_ERROR("Should not happen", 0, 0, 0);
      return;
   }
   
   //Get the entry of the selected item
   if (!RecentCalls_GetEntry(pMe, pMe->m_callsCategory,
                                   (byte) pMe->m_selectedItem, &ent)) {
      return;
   }

   //
   // Set the Number display to occupy the entire screen not used
   // by the softkey
   // 
   pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
   if (!pd) {
      return;
   }
   //Get the pointer to the static control
   pIStatic =(IStatic*) IDIALOG_GetControl(pd,IDC_NUMBER_STATIC_TEXT);
   
   //Get the pointer to the Soft key
   pSK = (IMenuCtl*) IDIALOG_GetControl(pd, IDC_NUMBER_OK_SK);
   if (!pSK || !pIStatic) {
      return;
   }

   IMENUCTL_GetRect(pSK, &tRect);
   
   tRect.dy = tRect.y;
   tRect.y  = pMe->m_rect.y;

   // Load Title
   (void) ISHELL_LoadResString(pMe->a.m_pIShell, 
                               RECENTCALLS_RES_FILE, 
                               IDS_NUMBER_TITLE, 
                               title, 
                               sizeof(title));

   // Set the dimensions of the rectangle for the text control
   ISTATIC_SetRect( pIStatic, &tRect);

   // Set the values of the title and text strings for this control
   fmtNum = FormatPhoneNumberForDisplay(ent.digits);
   if (fmtNum != NULL) {
      (void) ISTATIC_SetText(pIStatic, 
                             title, 
                             fmtNum,
                             AEE_FONT_BOLD, 
                             AEE_FONT_LARGE);
      FREE(fmtNum);
   } else {
      (void) ISTATIC_SetText(pIStatic, 
                             title, 
                             ent.digits,
                             AEE_FONT_BOLD, 
                             AEE_FONT_LARGE);
   }

   (void) ISTATIC_Redraw(pIStatic);

   return;
} //RecentCalls_DisplayNumber


/*=============================================================================
FUNCTION:  RecentCalls_ResetCallTimers
 
DESCRIPTION: Resets all the call timers back to 0.
 
PARAMETERS:
   *pMe: pointer to CRecentCalls applet
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void RecentCalls_ResetCallTimers(CRecentCalls *pMe)
{
  int                  timeSec=0;               // time in seconds
  IConfig             *p_Config;                // config passed to OEM_GetConfig funtion

  // Create the config object
  (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                               AEECLSID_CONFIG,
                               (void **)&p_Config);
  
  //Check to see if object was created
  if(!p_Config) {
    return;
  }

  // Set the last call timer information
  if (ICONFIG_SetItem(p_Config,CFGI_LASTCALL_TIMER,&timeSec,sizeof(timeSec))!= SUCCESS)
    MSG_ERROR("Error retrieving call time",0,0,0);

  // Retrieve the total home call timer information
  if (ICONFIG_SetItem(p_Config,CFGI_AIR_TIMER,&timeSec,sizeof(timeSec))!= SUCCESS)
    MSG_ERROR("Error retrieving call time",0,0,0);
    
  // Retrieve the total roam call timer information      
  if (ICONFIG_SetItem(p_Config,CFGI_ROAM_TIMER,&timeSec,sizeof(timeSec))!= SUCCESS)
    MSG_ERROR("Error retrieving call time",0,0,0);

  if (ICONFIG_SetItem(p_Config,CFGI_LIFE_TIMER,&timeSec,sizeof(timeSec))!= SUCCESS)
    MSG_ERROR("Error retrieving call time",0,0,0);

  ICONFIG_Release(p_Config); //lint !e522

  return;
}

/*=============================================================================
FUNCTION:  RecentCalls_DisplayTime
 
DESCRIPTION: Takes the active dialog information and displays the corresponding time
             information on the screen
 
PARAMETERS:
   *pMe: pointer to CRecentCalls applet
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void RecentCalls_DisplayTime(CRecentCalls *pMe)
{
  IStatic             *pIStatic;
  AECHAR               title[MAX_TITLE_LENGTH]; 
  IDialog             *pd;
  AECHAR               fmtTime[FORMAT_SIZE];    // to store the time in the hhh:mm:ss format
  int                  timeSec=0;               // time in seconds
  IConfig             *p_Config;                // config passed to OEM_GetConfig funtion

   //
   // Set the Time display to occupy the entire screen 
   // 
  pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (!pd) {
    return;
  }

  // Create the config object
  (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                               AEECLSID_CONFIG,
                               (void **)&p_Config);
  
  //Check to see if object was created
  if(!p_Config) {
    return;
  }

  // Check to see which Dialog is making the call and load correspondnig title
  switch (IDIALOG_GetID(pd)) {
    case IDD_LAST_CALL_TIME_DLG :
    {
      //Get the pointer to the static control
      pIStatic =(IStatic*) IDIALOG_GetControl(pd,IDC_LAST_CALL_STATIC_TEXT);

      if (!pIStatic) {
        return;
      }

      (void) ISHELL_LoadResString(pMe->a.m_pIShell, 
                                  RECENTCALLS_RES_FILE, 
                                  IDS_LAST_CALL_TITLE, 
                                  title, 
                                  sizeof(title));

      // Retrieve the last call timer information
      if (ICONFIG_GetItem(p_Config,CFGI_LASTCALL_TIMER,&timeSec,sizeof(timeSec))!= SUCCESS)
        MSG_ERROR("Error retrieving call time",0,0,0);

      break;
    }

    case IDD_TOTAL_HOME_TIME_DLG :
    {
      //Get the pointer to the static control
      pIStatic =(IStatic*) IDIALOG_GetControl(pd,IDC_HOME_TIME_STATIC_TEXT);

      if (!pIStatic) {
        return;
      }

      (void) ISHELL_LoadResString(pMe->a.m_pIShell, 
                                  RECENTCALLS_RES_FILE, 
                                  IDS_HOME_TIME_TITLE, 
                                  title, 
                                  sizeof(title));
      
      // Retrieve the total home call timer information
      if (ICONFIG_GetItem(p_Config,CFGI_AIR_TIMER,&timeSec,sizeof(timeSec))!= SUCCESS)
        MSG_ERROR("Error retrieving call time",0,0,0);

      break;
    }
   
    case IDD_TOTAL_ROAM_TIME_DLG :
    {
      //Get the pointer to the static control
      pIStatic =(IStatic*) IDIALOG_GetControl(pd,IDC_ROAM_TIME_STATIC_TEXT);

      if (!pIStatic) {
        return;
      }

      (void) ISHELL_LoadResString(pMe->a.m_pIShell, 
                                  RECENTCALLS_RES_FILE, 
                                  IDS_ROAM_TIME_TITLE, 
                                  title, 
                                  sizeof(title));
      
      // Retrieve the total roam call timer information      
      if (ICONFIG_GetItem(p_Config,CFGI_ROAM_TIMER,&timeSec,sizeof(timeSec))!= SUCCESS)
        MSG_ERROR("Error retrieving call time",0,0,0);

      break;
    }

    case IDD_TOTAL_TIME_DLG :
    {
      //Get the pointer to the static control
      pIStatic =(IStatic*) IDIALOG_GetControl(pd,IDC_TOTAL_TIME_STATIC_TEXT);
  
      if (!pIStatic) {
        return;
      }

      (void) ISHELL_LoadResString(pMe->a.m_pIShell, 
                                  RECENTCALLS_RES_FILE, 
                                  IDS_TOTAL_TIME_TITLE, 
                                  title, 
                                  sizeof(title));
      // Retrieve the total call timer information
      if (ICONFIG_GetItem(p_Config,CFGI_LIFE_TIMER,&timeSec,sizeof(timeSec))!= SUCCESS)
        MSG_ERROR("Error retrieving call time",0,0,0);

      break;
    }
   
    default :
      MSG_ERROR("Error in loading title",0,0,0);
      return;
  }

  ICONFIG_Release(p_Config); //lint !e522

   //change the time retrieved (in seconds) to the hhh:mm:ss format
  FormatTimeForDisplay(fmtTime,timeSec);

   // Set the values of the title and text strings for this control
  (void) ISTATIC_SetText(pIStatic, 
                         title, 
                         fmtTime,
                         AEE_FONT_BOLD, 
                         AEE_FONT_LARGE);

  (void) ISTATIC_Redraw(pIStatic);

  return;
}

/*=============================================================================
FUNCTION:  FormatTimeForDisplay
 
DESCRIPTION:
        Takes in the time in seconds (integer) and writes it to fmtTime in the hhh:mm:ss format
 
PARAMETERS:
   timeSec: time in seconds
 
RETURN VALUE:
   none

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void FormatTimeForDisplay(AECHAR* fmtTime, int timeSec)
{
  AECHAR    sPrintfFmt[]={'%','d',':','%','2','.','2','d',':','%','2','.','2','d','\0'};
  int       seconds;
  int       minutes;
  int       hours;
  
  if (timeSec > 0)
  {
    seconds = timeSec % 60;
    timeSec/=60;
    minutes = timeSec % 60;
    timeSec/=60;
    hours = timeSec;
  }
  else
  {
    seconds = 0;
    minutes = 0;
    hours = 0;
  }
  WSPRINTF(fmtTime,FORMAT_SIZE_IN_BYTES,sPrintfFmt,hours,minutes,seconds);
}

/*=============================================================================
FUNCTION: RecentCalls_SetupDialogHandler

DESCRIPTION:  Configures the dialog handler for a newly created dialog

PARAMETERS:
     *pMenu: CRecentCalls object pointer
   *pDialog: IDialog interface pointer

RETURN VALUE:
   boolean:  TRUE if the dialog event handler was setup successfully

COMMENTS:
   This function should be called in response to the EVT_DIALOG_INIT event.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_SetupDialogHandler(CRecentCalls *pMenu, 
                                               IDialog     *pDialog)
{
   uint16 id = IDIALOG_GetID(pDialog);
   //if the dialog id was null then bail out
   if (0 == id) {
      MSG_ERROR("Unknown dialog id", 0, 0, 0);
      return FALSE;
   }

   //Dialog ids are 1 based but array of event handler are 0 based.
   //So get the right handler and see if it does not fall out of valid range
   id--;
   if (id >= ARR_SIZE(sDialogEventHandlers)) {
      MSG_ERROR("Unknown dialog id %d", id+1, 0, 0);
      return FALSE;
   }
   
   //Set the event handler to the active dialog
   IDIALOG_SetEventHandler(pDialog, 
                           (PFNAEEEVENT) sDialogEventHandlers[id],
                           (void *) pMenu);

   //set the properties, so that the event handler for the dialog
   //handles all the events
   IDIALOG_SetProperties(pDialog, 
                         IDIALOG_GetProperties(pDialog) | 
                         DLG_HANDLE_ALL_EVENTS);

   return TRUE;
}

/*=============================================================================
FUNCTION: RecentCalls_DialogTimeout_Callback

DESCRIPTION:
   Callback function passed to the BREW layer so that whenever the timer that
   we have set in the dialog handler expires, we can post a timeout event to 
   the dialog.


PARAMETERS:
   *pUser: pUser is assumed to be a CContactApp pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void RecentCalls_DialogTimeout_Callback(void *pUser)
{
   CRecentCalls *pMe = (CRecentCalls*)pUser;

   // Post a message to the dialog telling it to close
   (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                           0,
                           EVT_COMMAND,
                           0,
                           0);
}

/*=============================================================================
FUNCTION: RecentCalls_MoveToDialog

DESCRIPTION:  Creates a new dialog, replacing the current dialog

PARAMETERS:
   *pme: CRecentCalls object pointer
   id: Dialog resource Id

RETURN VALUE:
   boolean: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_MoveToDialog(CRecentCalls *pme, uint16 id) 
{

   // Close the current dialog (if there is one)
   (void) ISHELL_EndDialog(pme->a.m_pIShell);

   // There shall only be a single dialog open at a time
   if (NULL != ISHELL_GetActiveDialog(pme->a.m_pIShell))
   {
     MSG_ERROR("Should only be a single dialog open", 0, 0, 0);
     return FALSE;
   }


   if (ISHELL_CreateDialog(pme->a.m_pIShell,
                           RECENTCALLS_RES_FILE,
                           id,
                           NULL) != AEE_SUCCESS) {
      MSG_ERROR ("Dialog creation failed id = %x", id,0,0);
      return FALSE;
   }
   return TRUE;
}

/*===========================================================================

FUNCTION RecentCalls_HandleEvent

DESCRIPTION
   Applet event handler.

PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/
static boolean RecentCalls_HandleEvent(CRecentCalls *pMe, 
                                       AEEEvent      eCode, 
                                       uint16        wParam, 
                                       uint32        dwParam)
{
   PARAM_NOT_REF(wParam)
      
   switch (eCode) {
   case EVT_APP_START:
   case EVT_APP_RESUME:
      {   
         // BREW will pass the applet its rectangle as a part of the AEEAppStart 
         // structure in the dwParam of the EVT_APP_START and EVT_APP_RESUME events.
         // However when an applet is running in a 'tab' on the Tabbed UI, this 
         // rectangle will be incorrect.  The rectangle will be for the entire display
         // area including the tab.  Therefore, we need to use ISHELL_GetDeviceInfo() 
         // to determine the correct rectangle.
         AEEDeviceInfo di;

         ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &di);
      
         SETAEERECT(&pMe->m_rect, 0, 0, di.cxScreen, di.cyScreen);
      

         pMe->m_lineHeight = IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay,
                                                     AEE_FONT_NORMAL,
                                                     NULL,
                                                     NULL);
         pMe->m_largeLineHeight = IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay,
                                                          AEE_FONT_LARGE,
                                                          NULL,
                                                          NULL);
         
         pMe->m_savenew = 0x0;
         pMe->m_addto   = 0x0;
         pMe->m_time    = 0x0;
         pMe->m_idleTimer = TRUE;
   
         //If we are starting the applications
         if (eCode == EVT_APP_START){
            const char *screen = ((AEEAppStart *) dwParam)->pszArgs;

            if (screen && (STRCMP(screen, RECENTCALLS_MISSED_CALL_SCREEN_STR) == 0)) {    
               //start with the missed call screen
               pMe->m_activeDialogId = IDD_PHONENUMBERS_DLG;
               pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_MISSED;

            } else if (screen && (STRCMP(screen, RECENTCALLS_REDIAL_SCREEN_STR) == 0)) {    
               //start with the redial screen
               pMe->m_activeDialogId = IDD_REDIAL_DLG;
               pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_TO;

            } else {
               //otherwise, start with the main menu
               pMe->m_activeDialogId = IDD_MAIN_CATEGORY_DLG;
            }
         } else{
            //If we are resuming and the suspend state was any other dialog
            //than main or phone number ie menu dialog, or other options dialog
            //go back to phone dialog, because the data may be stale due to 
            //incoming call or missed calls
            if ((pMe->m_activeDialogId != IDD_MAIN_CATEGORY_DLG) &&
                (pMe->m_activeDialogId != IDD_PHONENUMBERS_DLG)){
               pMe->m_activeDialogId = IDD_PHONENUMBERS_DLG;
            }
         }
   
         //Initialise this to TRUE
         pMe->m_yesnoTimeout = TRUE;
         
         //Move to the active dialog
         return RecentCalls_MoveToDialog(pMe, pMe->m_activeDialogId);
      }

   case EVT_APP_SUSPEND:
      {
         //If the applet is suspened store the dialog id, will be useful
         //when the applet is resumed
         IDialog *pDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
         if (NULL == pDialog) {
            MSG_ERROR("Unable to suspend because no dialog exists!", 0, 0, 0);
            return FALSE;
         }
         pMe->m_activeDialogId = IDIALOG_GetID(pDialog);
         return TRUE;
      }

   case EVT_APP_STOP:
      return TRUE;

   case EVT_DIALOG_INIT:
      //set the event handler based on the dialog which is getting initialized
      return RecentCalls_SetupDialogHandler(pMe, (IDialog *) dwParam);

   case EVT_KEY_PRESS:
   //case EVT_KEY_HELD:
      //(void) ResetMenuTimer(pMe->a.m_pIShell, TRUE);
      return TRUE;
   
   /* User events are sent to this dialog handler, however, we want to handle
    * them in the dialog handler for the current dialog.
    */
   case EVT_RECENTCALLS_PIN_COMPLETE:
   case EVT_RECENTCALLS_PIN_DISABLED:
   {
     /* Get our current dialog and ID. */
     IDialog *pDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
     uint16   id;
     if (NULL == pDialog) {
        MSG_ERROR("Unable to find current dialog!", 0, 0, 0);
        return FALSE;
     }

     id = IDIALOG_GetID(pDialog) - 1; /* sDialogEventHandlers is 0 based. */
     if (id >= ARR_SIZE(sDialogEventHandlers)) {
       MSG_ERROR("Unknown dialog id %d", id+1, 0, 0);
       return FALSE;
     }

     /* Call the dialog handler. */
     return (*sDialogEventHandlers[id])(pMe, eCode, wParam, dwParam);
   }

   default:
      break;
   }

   return FALSE;
}

/*=============================================================================

                    DIALOG EVENT HANDLERS
 
=============================================================================*/


/*=============================================================================
FUNCTION: RecentCalls_MainCategoryDlgHandler

DESCRIPTION: Main Menu Dialog Event Handler. You can select the category you 
   are looking for.

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_MainCategoryDlgHandler(CRecentCalls *pMe,
                                                  AEEEvent      eCode,
                                                  uint16        wParam,
                                                  uint32        dwParam)
{
  uint8 pin_id = 0xFF;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  AEECardApp CardApp;
#endif
#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (eCode) {
  case EVT_DIALOG_START:
  {
     IDialog  *pDialog;
     IMenuCtl *ctl;
     
     pDialog = (IDialog *) dwParam;
     ctl     = (IMenuCtl*) IDIALOG_GetControl(pDialog, 
                                              IDC_MAIN_CATEGORY_MENU);

     //set the title of the menu control
     (void) IMENUCTL_SetTitle(ctl,
                              RECENTCALLS_RES_FILE,
                              IDS_RECENTCALLS_TITLE,
                              NULL);
     
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
     /* Initialize the pin status and then ask ICARD to get us the
      * latest.
      */
     if (ICARD_GetCardApps(pMe->m_pICard, &CardApp) == SUCCESS)
     {
       if (CardApp.operational_slot & (AEECARD_GSM_SLOT1_OPERATIONAL |
                                       AEECARD_UMTS_SLOT1_OPERATIONAL))
         pin_id = AEECARD_PIN2;
       else if (CardApp.operational_slot & (AEECARD_GSM_SLOT2_OPERATIONAL |
                                            AEECARD_UMTS_SLOT2_OPERATIONAL))
         pin_id = AEECARD_PIN2_SLOT2;
     }
#endif

//lint -save -e774 "Always true"
     if (pin_id == 0xFF)
//lint +e774
     {
       if (!IMENUCTL_DeleteItem(ctl, IDL_MAIN_AOC_INFO))
       {
         MSG_ERROR("Could not delete menu control!", 0, 0, 0);
       }
     }

     //Set the default look of the menu
     SetDefaultMenuLook(pMe->a.m_pIShell, ctl);
     return TRUE;
  }

  case EVT_DIALOG_END:
     return TRUE;
      
  case EVT_KEY:
     switch ((AVKType) wParam) {
       case AVK_CLR:
         //close the applet and go to the calling applet 
         (void) ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
         break;
      
       default:
         break;
     }
     return TRUE;

   case EVT_COMMAND:
     //Check which category is selected and go the phone numbers dialog
     //and populate the number in that category
     switch (wParam) {
     case IDL_MAIN_MISSED_CALLS:
       pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_MISSED;
       break;
         
     case IDL_MAIN_INCOMING_CALLS:
       pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_FROM;
       break;

     case IDL_MAIN_OUTGOING_CALLS:
       pMe->m_callsCategory = AEECALLHISTORY_CALL_TYPE_TO;
       break;

     case IDL_MAIN_CALL_TIME:
       return RecentCalls_MoveToDialog(pMe, IDD_CALL_TIME_DLG);

     case IDL_MAIN_AOC_INFO:
       return RecentCalls_MoveToDialog(pMe, IDD_AOC_INFO_DLG);

     case IDL_MAIN_DELETEALL:
       //Set the message ID to show in YesNo dialog 
       pMe->m_yesnoDialogTitle = IDS_CONFIRM_CLEAR_HISTORY;
 
       //set the dialog to return to after showing the message box
       pMe->m_msgboxReturnDialogId = IDD_MAIN_CATEGORY_DLG;

       return RecentCalls_MoveToDialog(pMe, IDD_DELETE_YES_NO_DLG);

     default:
       MSG_ERROR("Unhandled EVT_COMMAND", 0, 0, 0);
       return FALSE;
     }

     {
       CallHistoryEntry  ent;

       // Check if the specified category is empty by attempting
       // to read the first entry:
       if(RecentCalls_GetEntry(pMe, pMe->m_callsCategory,
                               0, 
                               &ent)==FALSE){

          return RecentCalls_MoveToDialog(pMe, IDD_NOENTRIES_DLG);
       }

       return RecentCalls_MoveToDialog(pMe, IDD_PHONENUMBERS_DLG);
     }

   default:
      break;
   }
   return FALSE;
}

/*=============================================================================
FUNCTION: RecentCalls_PhoneNumbersDlgHandler

DESCRIPTION: Phone Number dialog handler. Shows the phone numbers in each 
 category.

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static boolean RecentCalls_PhoneNumbersDlgHandler(CRecentCalls *pMe,
                                                  AEEEvent      eCode,
                                                  uint16        wParam,
                                                  uint32        dwParam)
{
   
   PARAM_NOT_REF(dwParam)

   IDialog  *pDialog;
   IMenuCtl *ctl;

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

   pDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
   if (pDialog == NULL){
      return FALSE;
   }
   ctl = (IMenuCtl*) IDIALOG_GetControl(pDialog, 
                                        IDC_NUMBERS_MENU);
   if(ctl == NULL){
      return FALSE;
   }
   switch (eCode) {
   case EVT_DIALOG_START:
      {
         //Set the generic look of the menu
         SetDefaultMenuLook(pMe->a.m_pIShell, ctl);
         //populate the numbers based on the category
         RecentCalls_PopulateNumbers(pMe,ctl);
         return TRUE;
      }

   case EVT_DIALOG_END:
      return TRUE;

   case EVT_KEY:
      switch ((AVKType) wParam) {
      case AVK_CLR:
         // Return to main menu
         return RecentCalls_MoveToDialog(pMe, IDD_MAIN_CATEGORY_DLG);

      case AVK_SEND:
         //Call the number in this entry
         pMe->m_selectedItem = IMENUCTL_GetSel(ctl);
         return RecentCalls_CallNumber(pMe);

      default:
         break;
      }
      return TRUE;

   case EVT_COMMAND:
      pMe->m_selectedItem = IMENUCTL_GetSel(ctl);
      return RecentCalls_MoveToDialog(pMe, IDD_RCALLSMENU_DLG);

   default:
      break;
   }
   return FALSE;
}
/*=============================================================================
FUNCTION: RecentCalls_RCallsMenuDlgHandler

DESCRIPTION: Recent calls Menu handler 

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_RCallsMenuDlgHandler(CRecentCalls   *pMe,
                                                AEEEvent        eCode,
                                                uint16          wParam,
                                                uint32          dwParam)
{
   PARAM_NOT_REF(dwParam)
   
   IDialog  *pDialog;
   IMenuCtl *ctl;
   IShell   *ps;

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

   ps      = pMe->a.m_pIShell;
   pDialog = ISHELL_GetActiveDialog(ps);
   if (pDialog == NULL){
      return FALSE;
   }
   ctl     = (IMenuCtl*) IDIALOG_GetControl(pDialog, 
                                            IDC_RCALLS_MENU);
   if(ctl == NULL){
      return FALSE;
   }
   switch (eCode) {
   case EVT_DIALOG_START:
   {
      CallHistoryEntry  ent;

      if (pMe->m_selectedItem > 0xFF) {
         // should never happen
         MSG_ERROR("Invalid selected item %d", pMe->m_selectedItem, 0, 0);
         return FALSE;
      }

      if (!RecentCalls_GetEntry(pMe, pMe->m_callsCategory,
                                   (byte) pMe->m_selectedItem, &ent)) {
         return FALSE;
      }

      SetDefaultMenuLook(ps, ctl);

      // Remove the 'call number' and 'number' menu items if the
      // number is empty
      if (WSTRLEN(ent.digits) == 0) {
         (void) IMENUCTL_DeleteItem(ctl,
                                    IDL_CALLNUMBER_I);
         (void) IMENUCTL_DeleteItem(ctl,
                                    IDL_SAVE_NEW_I);
         (void) IMENUCTL_DeleteItem(ctl,
                                    IDL_SAVE_TO_EXISTING_I);
         (void) IMENUCTL_DeleteItem(ctl,
                                    IDL_SAVE_TO_UIM_I);
         (void) IMENUCTL_DeleteItem(ctl,
                                    IDL_NUMBER_I);
         (void) IMENUCTL_DeleteItem(ctl,
                                    IDL_SEND_SMS_I);
      }

#ifndef FEATURE_WMS_APP
      (void) IMENUCTL_DeleteItem(ctl,
                                 IDL_SEND_SMS_I);
#endif

#ifndef FEATURE_MMGSDI
      // If we don't support RUIM, don't give the menu option
      (void)IMENUCTL_DeleteItem(ctl, IDL_SAVE_TO_UIM_I);
#endif
      return TRUE;
   }

   case EVT_DIALOG_END:
      return TRUE;

   case EVT_KEY:
      switch ((AVKType) wParam) {
      case AVK_CLR:
         // Return to main menu
         return RecentCalls_MoveToDialog(pMe, IDD_PHONENUMBERS_DLG);
      
      //Call the number in the entry
      case AVK_SEND:
         return RecentCalls_CallNumber(pMe);

      default:
         break;
      }
      return TRUE;

   case EVT_COMMAND:
      switch (wParam) {
      case IDL_CALLNUMBER_I:
         return RecentCalls_CallNumber(pMe);
      
      // Submenu
      case IDL_SAVE_NEW_I:
         return RecentCalls_SaveNumber(pMe, SAVE_NEW);

      case IDL_SAVE_TO_EXISTING_I:
         return RecentCalls_SaveNumber(pMe, SAVE_TO_EXISTING);
         
      case IDL_SAVE_TO_UIM_I:
         return RecentCalls_SaveNumber(pMe, SAVE_TO_RUIM);

      //Go to the date time dialog
      case IDL_DATETIME_I:
         return RecentCalls_MoveToDialog(pMe, IDD_DATETIME_DLG);
      
      //Go the view number dialog
      case IDL_NUMBER_I:
         return RecentCalls_MoveToDialog(pMe, IDD_VIEW_NUMBER_DLG);

      //Send SMS message
#ifdef FEATURE_WMS_APP
      case IDL_SEND_SMS_I:
      {
         CallHistoryEntry  ent;
         char pszNumber[CALLHISTORY_MAXDIGITS];

         if (!RecentCalls_GetEntry(pMe, pMe->m_callsCategory,
                                      (byte) pMe->m_selectedItem, &ent)) {
            MSG_ERROR("Unable to obtain selected item %d", 
                       pMe->m_selectedItem, 0, 0);
            return FALSE; 
         }

         (void) WSTRTOSTR(ent.digits, pszNumber, sizeof(pszNumber));

         if (CheckAEEReturnStatus(OriginateMessage(FALSE, pszNumber))) {
           MSG_ERROR("Unable to send SMS message", 0, 0, 0);
           return FALSE;
         }

         return TRUE;
      }
#endif //FEATURE_WMS_APP

      default:
         break;
      }/*switch (wParam) */
      break;
      
      
   default:
      break;
   }/*switch (eCode)*/
   return FALSE;

}
/*=============================================================================
FUNCTION: RecentCalls_DateTimeDlgHandler

DESCRIPTION: Date Time dialog handler

PARAMETERS:
PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_DateTimeDlgHandler(CRecentCalls     *pMe,
                                              AEEEvent          eCode,
                                              uint16            wParam,
                                              uint32            dwParam)
{
#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

   switch (eCode) {
   case EVT_DIALOG_START:
      (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                              0,
                              EVT_COMMAND,
                              0,
                              dwParam);
      return TRUE;

   case EVT_COMMAND:
      if (IDL_DATETIME_OK_I == wParam){
         return RecentCalls_MoveToDialog(pMe, IDD_RCALLSMENU_DLG);
      } else {
         ITimeCtl            *pTimeCtl;
         IDateCtl            *pDate;
         IDateCtl            *pDatePick;
         CallHistoryEntry     ent;
         IConfig             *p_config;
         AEERect              rect;
         JulianType           jdate;
         AECHAR               buf[MAX_STATIC_TEXT_LEN],*tempBuf;
         uint32               lDate,lMin;
         int                  nChars;

         IShell              *ps= pMe->a.m_pIShell;

         if (pMe->m_selectedItem > 0xFF) {
            // should never happen
           MSG_ERROR("Invalid selected item %d", pMe->m_selectedItem, 0, 0);
           return FALSE;
         }

         if (!RecentCalls_GetEntry(pMe, pMe->m_callsCategory,
                                      (byte) pMe->m_selectedItem, &ent)) {
            return FALSE;
         }

         //create the controls requried to show different data in this dialog
         if (ISHELL_CreateInstance(ps, 
                                   AEECLSID_DATECTL, 
                                   (void **)(&pDate)) != SUCCESS     ||
             ISHELL_CreateInstance(ps, 
                                   AEECLSID_CLOCKCTL, 
                                   (void **)(&pTimeCtl)) != SUCCESS  ||
             ISHELL_CreateInstance(ps, 
                                   AEECLSID_DATEPICKCTL, 
                                   (void **)(&pDatePick))               ) {
            return FALSE;
         }

         if (!pDate || !pDatePick || !pTimeCtl) {
            return FALSE;
         }

         // display title
         (void) ISHELL_LoadResString(ps,
                                     RECENTCALLS_RES_FILE,
                                     IDS_DATETIME_TITLE,
                                     buf,
                                     sizeof(buf));
         SETAEERECT(&rect,
                    pMe->m_rect.x,
                    pMe->m_rect.y,
                    pMe->m_rect.dx,
                    pMe->m_lineHeight);

         (void) IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                                  AEE_FONT_BOLD,
                                  buf,
                                  -1,
                                  0,
                                  0,
                                  &rect,
                                  IDF_ALIGN_LEFT | IDF_ALIGN_TOP);

         SETAEERECT(&rect,
                    pMe->m_rect.x,
                    pMe->m_rect.y+pMe->m_lineHeight+1,
                    pMe->m_rect.dx,
                    pMe->m_lineHeight);
         
         //If timestamp is 0, then dont show the date and time.
         if (ent.time_stamp!=0) {
            
            GETJULIANDATE(ent.time_stamp, &jdate);
      
            (void) IDATECTL_SetDate(pDate,
                                    jdate.wYear,
                                    jdate.wMonth,
                                    jdate.wDay);
            
            lDate = (uint32) IDATECTL_GetJulianDay(pDate);
            (void) IDATECTL_SetJulianDay(pDatePick, (int32) lDate);
      
            // MM/DD removed a year from the display
            (void) IDATECTL_GetDateString(pDatePick,
                                          buf,
                                          sizeof(buf),
                                          &nChars,
                                          DFMT_MM_DD_YYYY);
            // and, display 09/23 only.
            buf[nChars-3] = buf[nChars-1];
            buf[nChars-4] = buf[nChars-2];
            tempBuf = &buf[0];
      
            // Draw Date String
            (void) IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                                     AEE_FONT_NORMAL,
                                     tempBuf,
                                     nChars-5,
                                     0,
                                     0,
                                     &rect,
                                     IDF_ALIGN_MIDDLE | IDF_ALIGN_RIGHT);
      
      
            // Get Time control to display the saved time information
            lMin =  1000 * ( (jdate.wHour * 3600) +
                             (jdate.wMinute * 60) +
                             jdate.wSecond 
                           );
      
            // first find out the time format
            (void) ISHELL_CreateInstance(ps,
                                         AEECLSID_CONFIG,
                                         (void **)&p_config);
      
            if (p_config) {
               GetTimeString(p_config, pTimeCtl, ps, lMin, buf);         
               ICONFIG_Release(p_config); //lint !e522
            }
      
            (void) IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                                     AEE_FONT_NORMAL,
                                     buf,
                                     -1,
                                     0,
                                     0,
                                     &rect,
                                     IDF_ALIGN_MIDDLE | IDF_ALIGN_LEFT);
         }
         if ((AEECALLHISTORY_CALL_TYPE_TO == ent.status) ||
             (AEECALLHISTORY_CALL_TYPE_FROM == ent.status))
         {

            AECHAR  dest_str[MAX_STATIC_TEXT_LEN]; // to store the duration string
            int     temp_pos = 11;  // fill it from the end
            dword   duration;
            AECHAR  min_str[2] = {0, 0};
            AECHAR  sec_str[2] = {0, 0};

            (void) ISHELL_LoadResString(ps,
                                        RECENTCALLS_RES_FILE,
                                        IDS_MIN,
                                        min_str,
                                        sizeof(AECHAR[2])
                                        );
            (void) ISHELL_LoadResString(ps,
                                        RECENTCALLS_RES_FILE,
                                        IDS_SEC,
                                        sec_str,
                                        sizeof(AECHAR[2])
                                        );

            // The duration string is prepared by calulating the seconds 
            // first from the ms, and then the digits of the minutes,
            // inserting the string for min and sec when necessary
            duration = ent.duration;
            dest_str[temp_pos--] = (AECHAR) '\0';
            dest_str[temp_pos--] = sec_str[0];
            dest_str[temp_pos--] = (AECHAR) ((duration%60) % 10 + '0');
            dest_str[temp_pos--] = (AECHAR) ((duration%60) / 10 + '0');
            dest_str[temp_pos--] = min_str[0];
            duration /= 60;
            dest_str[temp_pos--] = (AECHAR) (duration % 10 + '0');
            duration /= 10;
            while (temp_pos >= 0) {
               if (duration == 0) {
                  // do not fill in the leading positions by spaces
                  // that would make the string unnecessarily long
                  // dest_str[temp_pos--] = (AECHAR) ' ';
                  break;
               } else {
                  dest_str[temp_pos--] = (AECHAR) (duration % 10 + '0');
               }
               duration /= 10;
            }
            rect.y += (int16) pMe->m_lineHeight;
            (void) IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                                     AEE_FONT_NORMAL,
                                     dest_str + temp_pos + 1,
                                    -1,
                                    0,
                                    0,
                                    &rect,
                                    IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
         }

         {
            IMenuCtl    *pMenuCtl;
            IDialog     *pActiveDialog = ISHELL_GetActiveDialog( pMe->a.m_pIShell );


         // Get softkey control to change the looks of sk
            if( NULL != (pMenuCtl = (IMenuCtl *) IDIALOG_GetControl( pActiveDialog, IDC_DATETIME_OK_SK )) )
            {
               IMENUCTL_SetActive( pMenuCtl, TRUE );
            }
         }

         IDISPLAY_Update(pMe->a.m_pIDisplay);

         if (pDate) {
            IDATECTL_Release(pDate); //lint !e522
         }

         if (pDatePick) {
            IDATECTL_Release(pDatePick); //lint !e522
         }

         if (pTimeCtl) {
            ITIMECTL_Release(pTimeCtl); //lint !e522
         }
         return TRUE;
      }
   
   case EVT_DIALOG_END:
      return TRUE;

   case EVT_KEY:
      switch ((AVKType) wParam) {
      case AVK_CLR:
         //return to the Recent calls menu dialog
         return RecentCalls_MoveToDialog(pMe, IDD_RCALLSMENU_DLG);

      default:
         break;
      }
      return TRUE;

   default:
      break;
   }
   return FALSE;
}

/*=============================================================================
FUNCTION: RecentCalls_ViewNumberDlgHandler

DESCRIPTION: View Number dialog handler

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_ViewNumberDlgHandler(CRecentCalls       *pMe,
                                                AEEEvent            eCode,
                                                uint16              wParam,
                                                uint32              dwParam)
{
#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

   switch (eCode) {
   case EVT_DIALOG_START:
      {
         // Get softkey control to change the looks of sk
         IMenuCtl *ctl = (IMenuCtl*) IDIALOG_GetControl((IDialog *)dwParam, 
                                                         IDC_NUMBER_OK_SK);
         if (ctl) {
            SetDefaultSoftkeyLook(pMe->a.m_pIShell, ctl);
            IMENUCTL_SetActive(ctl, TRUE);
         }
         RecentCalls_DisplayNumber(pMe);
         return TRUE;
      }

   case EVT_DIALOG_END:
      return TRUE;

   case EVT_KEY:
      switch ((AVKType) wParam) {
      case AVK_CLR:
         // Return to main menu
         return RecentCalls_MoveToDialog(pMe, IDD_RCALLSMENU_DLG);

      default:
         break;
      }
      return TRUE;

   case EVT_COMMAND:
      if (IDL_NUMBER_OK_I == wParam){
         return RecentCalls_MoveToDialog(pMe, IDD_RCALLSMENU_DLG);
      }
      break;

   default:
      break;
   }
   return FALSE;
}

/*=============================================================================
FUNCTION: RecentCalls_EditNumberDlgHandler

DESCRIPTION: Edit Number dialog handler

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_EditNumberDlgHandler(CRecentCalls       *pMe,
                                                AEEEvent            eCode,
                                                uint16              wParam,
                                                uint32              dwParam)
{
   ITextCtl *pCtrl;
   IDialog  *pDialog;
   IMenuCtl *pSoftKey;
   pDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
   if (pDialog == NULL){
      return FALSE;
   }

 #ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

   //get the pointer to text control and soft key menu
   pCtrl = (ITextCtl*)IDIALOG_GetControl(pDialog,
                                        IDC_EDIT_NUMBER_TEXT);
   // Get softkey control to change the looks of sk
   pSoftKey = (IMenuCtl*) IDIALOG_GetControl(pDialog, 
                                             IDC_EDITNUMBER_SK);
   if ((pCtrl == NULL)||(pSoftKey== NULL)){
      return FALSE;
   }

   switch (eCode) {
   case EVT_DIALOG_START:
      {
         AEERect textRect, menuRect;
         CtlAddItem ai;

         // Remove the multitap softkey as soon as possible
         (void) IMENUCTL_DeleteAll(pSoftKey);
         (void) IMENUCTL_Redraw(pSoftKey);

                     // Build the softkey menu manually.
         ai.pText = NULL;
         ai.pImage = NULL;
         ai.pszResText = ai.pszResImage = RECENTCALLS_RES_FILE;
         ai.wFont = 0;
         ai.dwData = 0;

         //Add the Save Key
         ai.wText = IDS_SAVE_TITLE;
         ai.wImage = IDB_SAVE;
         ai.wItemID = IDL_SAVE_NUMBER;
         (void) IMENUCTL_AddItemEx(pSoftKey,
                                   &ai);

         //Add the Cancel Key
         (void)IMENUCTL_AddItem(pSoftKey,RECENTCALLS_RES_FILE,
                                IDS_CANCEL, IDL_CANCEL,
                                NULL,0);
         //Add the Pause Key
         ai.wText = IDS_PAUSE;
         ai.wImage = IDB_PAUSE;
         ai.wItemID = IDL_PAUSE;
         (void) IMENUCTL_AddItemEx(pSoftKey,
                                   &ai);

         SetDefaultSoftkeyLook(pMe->a.m_pIShell, pSoftKey);
         IMENUCTL_SetActive(pSoftKey, TRUE);

         IMENUCTL_GetRect(pSoftKey,&menuRect);
         
         //Set the max size dictated by the nv item
         ITEXTCTL_SetMaxSize(pCtrl,CALLHISTORY_MAXDIGITS);
         
         // Use the large font when editing numbers
//         ITEXTCTL_SetProperties(pCtrl, 
//                                ITEXTCTL_GetProperties(pCtrl) |
//                                TP_LARGE_FONT);

         //Set the original text in the control
         (void) ITEXTCTL_SetText(pCtrl,
                                 pMe->m_phoneNumber,
                                 -1);

         //Set the input mode to numbers only mode
         (void) ITEXTCTL_SetInputMode(pCtrl, AEE_TM_NUMBERS);

         //set the rect so that it does not clip, the menu control.
         ITEXTCTL_GetRect(pCtrl, &textRect);
         textRect.dy = pMe->m_rect.dy - menuRect.dy;
         ITEXTCTL_SetRect(pCtrl, &textRect);
         
         return TRUE;
      }

   case EVT_DIALOG_END:
      return TRUE;

   case EVT_KEY:
      switch ((AVKType) wParam) {
      case AVK_CLR:
         // Return to main menu
         return RecentCalls_MoveToDialog(pMe, IDD_RCALLSMENU_DLG);

      default:
         break;
      }
      return TRUE;

   case EVT_COMMAND:
      switch(wParam){
      case IDL_PAUSE:
         {
            int     len;
            int32   cursorPos;
            AECHAR *newStr;
            AECHAR *str;

            str = ITEXTCTL_GetTextPtr(pCtrl);
            len = WSTRLEN(str);

            cursorPos = ITEXTCTL_GetCursorPos(pCtrl);
            if (cursorPos < 0) {
               cursorPos = 0;
            }

            if (cursorPos > len) {
               cursorPos = len;
            }

            len += 2;  // make room for another character and the NULL
            newStr = MALLOC((uint32)len * sizeof(AECHAR));
            if (NULL == newStr) {
               return FALSE;
            }

            // Copy the original string up to the current cursor 
            // position into the new string
            (void) MEMCPY(newStr, str, (uint32) cursorPos * sizeof(AECHAR));

            // Add the pause character
            newStr[cursorPos] = DIALER_PAUSE_AECHAR;

            // Copy what's left of the original string into the 
            // new string after the pause character.
            (void) WSTRCPY(&newStr[cursorPos+1], &str[cursorPos]);

            (void) ITEXTCTL_SetText(pCtrl,
                                    newStr,
                                    -1);

            // Move the cursor to the character immediately after
            // the newly inserted pause character
            ITEXTCTL_SetCursorPos(pCtrl, cursorPos+1);

            FREE(newStr);

            // Set the focus back to the text control
            (void) IDIALOG_SetFocus(pDialog,
                                    IDC_EDIT_NUMBER_TEXT);
            return TRUE;
         }

      case IDL_SAVE_NUMBER:
         {
            (void) WSTRCPY(pMe->m_phoneNumber,ITEXTCTL_GetTextPtr(pCtrl));
            //if number is empty then show the message 
            if(WSTRLEN(pMe->m_phoneNumber) <=0){
               pMe->m_msgboxReturnDialogId = IDD_EDIT_NUMBER_DLG;
               pMe->m_msgBox_textId   = IDS_NOTHING_ENTERED;
            } else {
               //Save the edited number
#ifdef FEATURE_OTASP
               // Prevent the user from saving OTASP numbers!
               //if (AEENUMBERTYPE_OTASP == GetNumberType(pMe->m_phoneNumber)) {
               //   pMe->m_msgBox_textId = IDS_ENTRY_NO_SAVE;
               //} 
               //else
#endif /* FEATURE_OTASP */
               {
                  // TODO - Allow number edit ?
                  //if(RecentCalls_EditEntry(pMe->m_callsCategory,
                  //                            (byte)pMe->m_selectedItem,
                  //                            pMe->m_phoneNumber)==TRUE) {
                  //   pMe->m_msgBox_textId = IDS_ENTRY_SAVED;
                  //} else {
                     pMe->m_msgBox_textId= IDS_ENTRY_SAVE_ERROR;
                  //}
               }
               pMe->m_msgboxReturnDialogId = IDD_RCALLSMENU_DLG;
            }
            return RecentCalls_MoveToDialog(pMe, IDD_MSGBOX_DLG);
         }
      
      case IDL_CANCEL:
         return RecentCalls_MoveToDialog(pMe, IDD_RCALLSMENU_DLG);

      default:
         break;
      }
      break;

   default:
      break;
   }
   return FALSE;
}

/*=============================================================================
FUNCTION: RecentCalls_YesNoDlgHandler

DESCRIPTION: Message Box dialog handler

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_YesNoDlgHandler(CRecentCalls       *pMe,
                                           AEEEvent            eCode,
                                           uint16              wParam,
                                           uint32              dwParam)
{
   IMenuCtl *pMenu;
   IDialog  *pDialog;
   IStatic  *pCtl;
   pDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
   if (pDialog == NULL){
      return FALSE;
   }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

   pMenu = (IMenuCtl*)IDIALOG_GetControl(pDialog,
                                         YESNO_MENU);
   pCtl     = (IStatic *) IDIALOG_GetControl(pDialog, IDC_YESNO_TEXT);
   if((pCtl == NULL)||(pMenu==NULL)) {
      return FALSE;
   }
   
   switch (eCode) {
      case EVT_DIALOG_START:
         {
            AEERect rc;
            AECHAR  szText[MSGBOX_MAXTEXTLEN];
            AEEDeviceInfo  deviceInfo;     // Device Information
            deviceInfo.wStructSize = sizeof(deviceInfo);
            ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &deviceInfo);

            pDialog = (IDialog *) dwParam;

            IMENUCTL_GetRect(pMenu,&rc);

            rc.dy = rc.y;
            rc.dx = (int16)deviceInfo.cxScreen;
            rc.y = 0;
            rc.x = 0;
            
            (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                        RECENTCALLS_RES_FILE,
                                        pMe->m_yesnoDialogTitle,
                                        szText,
                                        sizeof(szText));
            
            ISTATIC_SetRect(pCtl, &rc);

            ISTATIC_SetProperties(pCtl,
                                  ST_CENTERTEXT | ST_MIDDLETEXT);

            (void) ISTATIC_SetText(pCtl,
                                   NULL,
                                   szText,
                                   AEE_FONT_NORMAL,
                                   AEE_FONT_NORMAL);

            if (pMe->m_yesnoTimeout) {
               IMENUCTL_SetSel(pMenu, IDL_MENUITEM_NO);
               (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                                      YESNO_TIMEOUT,
                                      RecentCalls_DialogTimeout_Callback,
                                      pMe);
            }
            return TRUE;

         }
      //if the dialog ends
      case EVT_DIALOG_END:
         //Cancel the timeout timer
         (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                                   RecentCalls_DialogTimeout_Callback,
                                   pMe);
         return TRUE;


      case EVT_KEY:
         switch (wParam) {
            case AVK_CLR:
               return RecentCalls_MoveToDialog(pMe, 
                                               pMe->m_msgboxReturnDialogId);

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         switch (wParam) {
            case IDL_MENUITEM_YES:
               if(RecentCalls_DeleteAll(pMe))
                  pMe->m_msgBox_textId = IDS_HISTORY_CLEARED;
               else
                  pMe->m_msgBox_textId = IDS_HISTORY_CLEAR_ERROR;
               return RecentCalls_MoveToDialog(pMe, IDD_MSGBOX_DLG);

            case IDL_MENUITEM_NO:
            default:
               return RecentCalls_MoveToDialog(pMe, 
                                               pMe->m_msgboxReturnDialogId);

         }

      default:
         break;
   }
   return FALSE;
}

/*=============================================================================
FUNCTION: RecentCalls_MsgBoxDlgHandler

DESCRIPTION: Message Box dialog handler

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_MsgBoxDlgHandler(CRecentCalls       *pMe,
                                            AEEEvent            eCode,
                                            uint16              wParam,
                                            uint32              dwParam)
{
#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

   switch (eCode) {
      case EVT_DIALOG_START:
      {
         IDialog  *pDialog;
         IStatic  *pCtl;
         AEERect rc;
         AECHAR  szText[MSGBOX_MAXTEXTLEN];
         AEEDeviceInfo  deviceInfo;     // Device Information
         deviceInfo.wStructSize = sizeof(deviceInfo);
         ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &deviceInfo);

         pDialog = (IDialog *) dwParam;
         pCtl     = (IStatic *) IDIALOG_GetControl(pDialog, IDC_MSGBOX_TEXT);
         if(pCtl == NULL){
            return FALSE;
         }

         SETAEERECT(&rc,
                    0,
                    0,
                    deviceInfo.cxScreen,
                    deviceInfo.cyScreen);

         
         (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                     RECENTCALLS_RES_FILE,
                                     pMe->m_msgBox_textId,
                                     szText,
                                     sizeof(szText));


         ISTATIC_SetRect(pCtl, &rc);

         ISTATIC_SetProperties(pCtl,
                               ST_CENTERTEXT | ST_MIDDLETEXT);

         //Set the text which you want to show in the message box
         (void) ISTATIC_SetText(pCtl,
                                NULL,
                                szText,
                                AEE_FONT_NORMAL,
                                AEE_FONT_NORMAL);

         //Set the timer, When this timer expires the dialog will
         //be closed
         (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                                MSGBOX_TIMEOUT,
                                RecentCalls_DialogTimeout_Callback,
                                pMe);
         return TRUE;
      }

      case EVT_DIALOG_END:
      {
         //when the dialog ends, close the timer
         (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                                   RecentCalls_DialogTimeout_Callback,
                                   pMe);

         return TRUE;
      }

      case EVT_KEY:
      case EVT_COMMAND:
         //when the dialog ends, close the timer
         (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                                   RecentCalls_DialogTimeout_Callback,
                                   pMe);
         return RecentCalls_MoveToDialog(pMe, pMe->m_msgboxReturnDialogId);

      default:
         break;
   }
   return FALSE;
} /*RecentCalls_MsgBoxDlgHandler */



/*=============================================================================
FUNCTION: RecentCalls_RedialDlgHandler

DESCRIPTION: Redial dialog handler

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_RedialDlgHandler(CRecentCalls       *pMe,
                                            AEEEvent            eCode,
                                            uint16              wParam,
                                            uint32              dwParam)
{
#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

   if(pMe == NULL){
     MSG_ERROR("Null Pointer", 0, 0, 0);
     return FALSE;
   }

   switch (eCode) {
   case EVT_DIALOG_START:
      {
         CallHistoryEntry  ent;

         // This is ugly.  The current call history API won't tell us
         // how many entries are currently in the call history, but we need
         // this information to display the scroll bar.  This means we 
         // need to manually load every entry here and count them
         // (but fortunately this only results in a bunch of mem copies
         // as the call history information is cached by the OEM layer.
         // 
         pMe->m_numRedialEntries = 0;
         for (;;) {
            if (!RecentCalls_GetEntry(pMe, AEECALLHISTORY_CALL_TYPE_TO,
                                     pMe->m_numRedialEntries,
                                     &ent)) {
               // Failed to read the current entry, we must be at the
               // end of the list.
               break;
            }
            pMe->m_numRedialEntries++;
         }

         pMe->m_selectedItem = 0;

         CALLBACK_Init(&pMe->m_drawRedialCB,
                       RecentCalls_DrawRedialDialog,
                       pMe);
         ISHELL_Resume(pMe->a.m_pIShell, &pMe->m_drawRedialCB);
         return TRUE;
      }

   case EVT_DIALOG_END:
      //when the dialog ends, close the timer and cancel callback
      CALLBACK_Cancel(&pMe->m_drawRedialCB); 
      (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                                RecentCalls_DialogTimeout_Callback,
                                pMe);
      return TRUE;

   case EVT_KEY:
      // Don't process keys if the call history is empty...
      if (pMe->m_numRedialEntries > 0) {
         switch ((AVKType) wParam) {
         case AVK_UP:
            if (0 == pMe->m_selectedItem) {
               pMe->m_selectedItem = pMe->m_numRedialEntries;
            }
            pMe->m_selectedItem--;

            RecentCalls_DrawRedialDialog(pMe);
            return TRUE;

         case AVK_DOWN:
            pMe->m_selectedItem++;
            if (pMe->m_selectedItem >= pMe->m_numRedialEntries) {
               pMe->m_selectedItem = 0;
            }

            RecentCalls_DrawRedialDialog(pMe);
            return TRUE;

         case AVK_SEND:
            if(RecentCalls_CallNumber(pMe) == TRUE)
            {
              // we are done with recent calls app so close it
              (void) ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
              return TRUE;
            }
            return FALSE;
            
         default:
            break;
         }
      }
      break;

   case EVT_COMMAND:
      (void) ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
      return TRUE;

   default:
      break;
   }

   return FALSE;

} /*RecentCalls_RedialDlgHandler*/


/*=============================================================================
FUNCTION: RecentCalls_NoEntriesDlgHandler

DESCRIPTION: NoEntries dialog handler

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_NoEntriesDlgHandler(CRecentCalls       *pMe,
                                            AEEEvent            eCode,
                                            uint16              wParam,
                                            uint32              dwParam)
{
#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

   switch (eCode) {
   case EVT_DIALOG_START:
      {
         IDialog *pDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell); 
         IStatic *pStatic;
         uint16   textId;
         AECHAR   wText[32];

         pStatic = (IStatic *) IDIALOG_GetControl(pDialog,
                                                  IDC_NOENTRIES_STATIC);
         switch(pMe->m_callsCategory){
         case AEECALLHISTORY_CALL_TYPE_MISSED:
            textId = IDS_NO_MISSED_CALLS;
            break;

         case AEECALLHISTORY_CALL_TYPE_FROM:
            textId = IDS_NO_INCOMING_CALLS;
            break;
         
         case AEECALLHISTORY_CALL_TYPE_TO:
            textId = IDS_NO_OUTGOING_CALLS;
            break;

         default:
            MSG_ERROR("Unknown category", 0, 0, 0);
            textId = IDS_NO_OUTGOING_CALLS;
            break;
         }

         wText[0] = 0;
         (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                     RECENTCALLS_RES_FILE,
                                     textId,
                                     wText,
                                     sizeof(wText));

         ISTATIC_SetProperties(pStatic, ST_CENTERTEXT | ST_MIDDLETEXT);

         (void) ISTATIC_SetText(pStatic,
                                NULL,
                                wText,
                                AEE_FONT_NORMAL,
                                AEE_FONT_NORMAL);

         (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                                TIMEOUT_MS_NOENTRIES_SCREEN,
                                RecentCalls_DialogTimeout_Callback,
                                pMe);
         return TRUE;
      }

   case EVT_DIALOG_END:
      //when the dialog ends, close the timer
      (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                                RecentCalls_DialogTimeout_Callback,
                                pMe);
      return TRUE;

   case EVT_KEY:
   case EVT_COMMAND:
      return RecentCalls_MoveToDialog(pMe, IDD_MAIN_CATEGORY_DLG);

   default:
      break;
   }

   return FALSE;

} /*RecentCalls_NoEntriesDlgHandler*/


/*=============================================================================
FUNCTION: RecentCalls_CallTimeDlgHandler

DESCRIPTION: Call Time Info dialog handler, handles all events occuring for the 
             main call times infor dialog

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_CallTimeDlgHandler(CRecentCalls      *pMe,
                                              AEEEvent           eCode,
                                              uint16             wParam,
                                              uint32             dwParam)
{
#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (eCode) {
    case EVT_DIALOG_START:
    {
      IDialog  *pDialog;
      IMenuCtl *ctl;
         
      pDialog = (IDialog *) dwParam;
      ctl     = (IMenuCtl*) IDIALOG_GetControl(pDialog, 
                                               IDC_CALL_TIME_MENU);

      //set the title of the menu control
      (void) IMENUCTL_SetTitle(ctl,
                               RECENTCALLS_RES_FILE,
                               IDS_CALL_TIMES,
                               NULL);
         
      //Set the default look of the menu
      SetDefaultMenuLook(pMe->a.m_pIShell, ctl);
      return TRUE;
    }

   case EVT_DIALOG_END:
     return TRUE;

   case EVT_KEY:
     { 
       switch ((AVKType) wParam) {
         case AVK_CLR:
           //return to the Recent calls menu dialog
           return RecentCalls_MoveToDialog(pMe, IDD_MAIN_CATEGORY_DLG);

         default:
           break;
       }

       return TRUE;
     }

   case EVT_COMMAND:
     //Check which category is selected, get that information (in secs),
     //convert to hours, minutes and seconds and display that value.
     switch (wParam) {
       case IDL_LAST_CALL_TIME:
         return RecentCalls_MoveToDialog(pMe, IDD_LAST_CALL_TIME_DLG);
         
       case IDL_TOTAL_HOME_TIME:
         return RecentCalls_MoveToDialog(pMe, IDD_TOTAL_HOME_TIME_DLG);

       case IDL_TOTAL_ROAM_TIME:
         return RecentCalls_MoveToDialog(pMe, IDD_TOTAL_ROAM_TIME_DLG);

       case IDL_TOTAL_TIME:
         return RecentCalls_MoveToDialog(pMe, IDD_TOTAL_TIME_DLG);

       case IDL_RESET_TIMES:
         RecentCalls_ResetCallTimers(pMe);
         return RecentCalls_MoveToDialog(pMe, IDD_TOTAL_TIME_DLG);

       default:
         MSG_ERROR("Unhandled EVT_COMMAND %d", wParam, 0, 0);
         return FALSE;
      }
      
  default:
    break;
  }

  return FALSE;

} /*RecentCalls_CallTimeDlgHandler*/

/*=============================================================================
FUNCTION: RecentCalls_AllCallTimeDlgHandler

DESCRIPTION: All Call Time Info dialog handler, handler for all 
             the call info types

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_AllCallTimeDlgHandler(CRecentCalls       *pMe,
                                                  AEEEvent            eCode,
                                                  uint16              wParam,
                                                  uint32              dwParam)
{
#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (eCode) {
    case EVT_DIALOG_START:
    {
      //displays the time of the last call
      RecentCalls_DisplayTime(pMe);
      return TRUE;
    }

    case EVT_DIALOG_END:
      return TRUE;

    case EVT_KEY:
    { 
      switch ((AVKType) wParam) {
        case AVK_CLR:
          //return to the Recent calls menu dialog
          return RecentCalls_MoveToDialog(pMe, IDD_CALL_TIME_DLG);

        default:
          break;
      }

      return TRUE;
    }

    case EVT_COMMAND:
      return TRUE;

    default:
      break;
  }

  return FALSE;

} /*RecentCalls_AllCallTimeDlgHandler*/

/*=============================================================================
FUNCTION: RecentCalls_AocInfoDlgHandler

DESCRIPTION: Advice of Charge dialog handler, handles all events occuring for
             the main call charge information dialog

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_AocInfoDlgHandler (CRecentCalls      *pMe,
                                              AEEEvent           eCode,
                                              uint16             wParam,
                                              uint32             dwParam)
{
#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (eCode) {
    case EVT_DIALOG_START:
    {
      IDialog  *pDialog;
      IMenuCtl *ctl;
         
      pDialog = (IDialog *) dwParam;
      ctl     = (IMenuCtl*) IDIALOG_GetControl(pDialog, 
                                               IDC_AOC_MENU);

      //set the title of the menu control
      (void) IMENUCTL_SetTitle(ctl,
                               RECENTCALLS_RES_FILE,
                               IDS_AOC_CHARGES,
                               NULL);
         
      //Set the default look of the menu
      SetDefaultMenuLook(pMe->a.m_pIShell, ctl);
      return TRUE;
    }

   case EVT_DIALOG_END:
     return TRUE;

   case EVT_KEY:
     { 
       switch ((AVKType) wParam) {
         case AVK_CLR:
           //return to the Recent calls menu dialog
           return RecentCalls_MoveToDialog(pMe, IDD_MAIN_CATEGORY_DLG);

         default:
           break;
       }

       return FALSE;
     }

   case EVT_COMMAND:
     //Check which category is selected, get that information (in secs),
     //convert to hours, minutes and seconds and display that value.
     switch (wParam) {
       case IDL_CCM:
         return RecentCalls_MoveToDialog(pMe, IDD_CCM_INFO_DLG);
         
       case IDL_ACM:
         return RecentCalls_MoveToDialog(pMe, IDD_ACM_INFO_DLG);

       case IDL_ACMMAX:
         return RecentCalls_MoveToDialog(pMe, IDD_ACMMAX_INFO_DLG);

       default:
         MSG_ERROR("Unhandled EVT_COMMAND %d", wParam, 0, 0);
         return FALSE;
      }
      
  default:
    break;
  }

  return FALSE;

} /*RecentCalls_AocInfoDlgHandler*/

/*=============================================================================
FUNCTION:  RecentCalls_DisplayCCM
 
DESCRIPTION:
   This function will get the current value for the CCM from ICM and display
   the number on the current dialog.
 
PARAMETERS:
   *pMe: pointer to CRecentCalls applet object.
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_DisplayCCM( CRecentCalls *pMe )
{
   IStatic             *pIStatic;
   IDialog             *pd;
   IMenuCtl            *pSK;

   AEERect              tRect;
   AECHAR               title[MAX_TITLE_LENGTH]; 
   AECHAR               fmtNum[AOC_FMT_SIZE];
   AECHAR               display_num[AOC_STR_SIZE];
   uint32               ccm_value = 0;

   if (pMe == NULL || pMe->a.m_pIShell == NULL) 
   {
     MSG_ERROR("Invalid pMe %x received %x %x", pMe, 0, 0);
     return FALSE;
   }
   //
   // Set the Number display to occupy the entire screen not used
   // by the softkey
   // 
   pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
   if (!pd) {
     MSG_ERROR("Could not get active dialog", 0, 0, 0);
     return FALSE;
   }
   if (IDIALOG_GetID(pd) != IDD_CCM_INFO_DLG)
   {
     MSG_ERROR("Unexpected dialog ID received %d wanted %d",
               IDIALOG_GetID(pd), IDD_ACMMAX_INFO_DLG, 0);
     return FALSE;
   }

#ifdef FEATURE_ICM
   if (pMe->m_pICM == NULL)
   {
     MSG_ERROR("m_pICM is set to NULL", 0, 0, 0);
     return FALSE;
   }

   (void) ICM_GetPhoneInfo(pMe->m_pICM, &pMe->m_sPh_info, sizeof(pMe->m_sPh_info));
   ccm_value = pMe->m_sPh_info.aoc_info.ccm;
#else
   ccm_value = cm_ph_cmd_aoc_get_ccm();
#endif

   //Get the pointer to the Soft key
   pSK = (IMenuCtl*) IDIALOG_GetControl(pd, IDC_CCM_INFO_SK);

   //Get the pointer to the static control
   pIStatic =(IStatic*) IDIALOG_GetControl(pd,IDC_CCM_INFO_STATIC_TEXT);
   if (!pIStatic || !pSK) {
     MSG_ERROR("Could not get static control", 0, 0, 0);
     return FALSE;
   }

   IMENUCTL_SetActive(pSK, TRUE);

   // Load Title
   (void) ISHELL_LoadResString(pMe->a.m_pIShell, 
                               RECENTCALLS_RES_FILE, 
                               IDS_CCM, 
                               title, 
                               sizeof(title));

   (void) ISHELL_LoadResString(pMe->a.m_pIShell, 
                               RECENTCALLS_RES_FILE, 
                               IDS_CCM_FMT, 
                               fmtNum, 
                               sizeof(fmtNum));

   /* Size the static portion of the window to be above the softkey.
    * The STATIC text portion starts at the top of the screen, and
    * goes to the top of the softkey.
    * */
   IMENUCTL_GetRect(pSK, &tRect);
   
   tRect.dy = tRect.y - pMe->m_rect.y;
   tRect.y  = pMe->m_rect.y;

   tRect.x  = pMe->m_rect.x;
   tRect.dx = pMe->m_rect.dx;

   // Set the dimensions of the rectangle for the text control
   ISTATIC_SetRect( pIStatic, &tRect);

   // Set the values of the title and text strings for this control
   WSPRINTF(display_num, sizeof(display_num), fmtNum, 
                  ccm_value / 1000, ccm_value % 1000);
   (void) ISTATIC_SetText(pIStatic, 
                          title, 
                          display_num,
                          AEE_FONT_BOLD, 
                          AEE_FONT_LARGE);
   
   (void) ISTATIC_Redraw(pIStatic);

   return TRUE;
} //RecentCalls_DisplayCCM

/*=============================================================================
FUNCTION:  RecentCalls_DisplayACM
 
DESCRIPTION:
   This function gets and displays the ACM value.
 
PARAMETERS:
   *pMe: pointer to CRecentCalls applet object
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_DisplayACM( CRecentCalls *pMe )
{
   IStatic             *pIStatic;
   IDialog             *pd;
   IMenuCtl            *pSK;

   AEERect              tRect;
   AECHAR               title[MAX_TITLE_LENGTH]; 
   AECHAR               fmtNum[AOC_FMT_SIZE];
   AECHAR               num_to_display[AOC_STR_SIZE];
   uint32               acm_value = 0;

   /* Check our input parameters. */
   if (pMe == NULL || pMe->a.m_pIShell == NULL)
   {
     MSG_ERROR("Invalid pMe %x received", pMe, 0, 0);
     return FALSE;
   }
   //
   // Set the Number display to occupy the entire screen not used
   // by the softkey
   // 
   pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
   if (!pd) {
     MSG_ERROR("Could not get dialog", 0, 0, 0);
     return FALSE;
   }

   if (IDIALOG_GetID(pd) != IDD_ACM_INFO_DLG)
   {
     MSG_ERROR("Unexpected dialog ID received %d wanted %d",
               IDIALOG_GetID(pd), IDD_ACM_INFO_DLG, 0);
     return FALSE;
   }
   
   /* Get the ACM value from ICM. */
#ifdef FEATURE_ICM
   if (pMe->m_pICM == NULL)
   {
     MSG_ERROR("m_pICM is set to NULL", 0, 0, 0);
     return FALSE;
   }

   (void) ICM_GetPhoneInfo(pMe->m_pICM, &pMe->m_sPh_info, sizeof(pMe->m_sPh_info));
   acm_value = pMe->m_sPh_info.aoc_info.acm;
#else
   acm_value = cm_ph_cmd_aoc_get_acm();
#endif

   //Get the pointer to the static control
   pIStatic =(IStatic*) IDIALOG_GetControl(pd,IDC_ACM_INFO_STATIC_TEXT);
   
   //Get the pointer to the Soft key
   pSK = (IMenuCtl*) IDIALOG_GetControl(pd, IDC_RESET_ACM_SK);

   //Check that we actually got the controls.
   if (!pSK || !pIStatic) {
     MSG_ERROR("Could not get control %x %x", pSK, pIStatic, 0);
     return FALSE;
   }

   IMENUCTL_SetActive(pSK, TRUE);

   // Load Title
   (void) ISHELL_LoadResString(pMe->a.m_pIShell, 
                               RECENTCALLS_RES_FILE, 
                               IDS_ACM, 
                               title, 
                               sizeof(title));

   (void) ISHELL_LoadResString(pMe->a.m_pIShell, 
                               RECENTCALLS_RES_FILE, 
                               IDS_ACM_FMT, 
                               fmtNum, 
                               sizeof(fmtNum));

   
   /* Size the static portion of the window to be above the softkey.
    * The STATIC text portion starts at the top of the screen, and
    * goes to the top of the softkey.
    * */
   IMENUCTL_GetRect(pSK, &tRect);
   
   tRect.dy = tRect.y - pMe->m_rect.y;
   tRect.y  = pMe->m_rect.y;

   tRect.x  = pMe->m_rect.x;
   tRect.dx = pMe->m_rect.dx;

   // Set the dimensions of the rectangle for the text control
   ISTATIC_SetRect( pIStatic, &tRect);

   // Set the values of the title and text strings for this control
   WSPRINTF(num_to_display, sizeof(num_to_display), fmtNum, acm_value);
   (void) ISTATIC_SetText(pIStatic, 
                          title, 
                          num_to_display,
                          AEE_FONT_BOLD, 
                          AEE_FONT_LARGE);
   
   (void) ISTATIC_Redraw(pIStatic);

   return TRUE;
} //RecentCalls_DisplayACM

/*=============================================================================
FUNCTION:  RecentCalls_DisplayACMMAX
 
DESCRIPTION:
   This function gets and displays the current ACMMAX value.
 
PARAMETERS:
   *pMe: pointer to CRecentCalls applet
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_DisplayACMMAX( CRecentCalls *pMe )
{
   IStatic             *pIStatic;
   IDialog             *pd;
   IMenuCtl            *pSK;
   AEERect              tRect;
   AECHAR               title[MAX_TITLE_LENGTH]; 
   AECHAR               num_to_display[AOC_STR_SIZE];
   AECHAR               fmtNum[AOC_FMT_SIZE];
   uint32               acm_value = 0;

   /* Check out input parameters. */
   if (pMe == NULL || pMe->a.m_pIShell == NULL)
   {
     MSG_ERROR("Invalid pMe %x received", pMe, 0, 0);
     return FALSE;
   }

   //
   // Set the Number display to occupy the entire screen not used
   // by the softkey
   // 
   pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
   if (!pd) {
     MSG_ERROR("Could not allocate dialog", 0, 0, 0);
     return FALSE;
   }
   if (IDIALOG_GetID(pd) != IDD_ACMMAX_INFO_DLG)
   {
     MSG_ERROR("Unexpected dialog ID received %d wanted %d",
               IDIALOG_GetID(pd), IDD_ACMMAX_INFO_DLG, 0);
     return FALSE;
   }

   
#ifdef FEATURE_ICM
   if (pMe->m_pICM == NULL)
   {
     MSG_ERROR("m_pICM is set to NULL", 0, 0, 0);
     return FALSE;
   }

   (void) ICM_GetPhoneInfo(pMe->m_pICM, &pMe->m_sPh_info, sizeof(pMe->m_sPh_info));
   acm_value = pMe->m_sPh_info.aoc_info.acm_max;
#else
   acm_value = cm_ph_cmd_aoc_get_acmmax();
#endif

   //Get the pointer to the static control
   pIStatic =(IStatic*) IDIALOG_GetControl(pd,IDC_ACMMAX_INFO_STATIC_TEXT);
   
   //Get the pointer to the Soft key
   pSK = (IMenuCtl*) IDIALOG_GetControl(pd, IDC_SET_ACMMAX_SK);
   if (!pSK || !pIStatic) {
     MSG_ERROR("Could not allocate control %x %x", pSK, pIStatic, 0);
     return FALSE;
   }

   IMENUCTL_SetActive(pSK, TRUE);

   // Load Title
   (void) ISHELL_LoadResString(pMe->a.m_pIShell, 
                               RECENTCALLS_RES_FILE, 
                               IDS_ACMMAX, 
                               title, 
                               sizeof(title));

   (void) ISHELL_LoadResString(pMe->a.m_pIShell, 
                               RECENTCALLS_RES_FILE, 
                               IDS_ACM_FMT, 
                               fmtNum, 
                               sizeof(fmtNum));

   /* Size the static portion of the window to be above the softkey.
    * The STATIC text portion starts at the top of the screen, and
    * goes to the top of the softkey.
    */
   IMENUCTL_GetRect(pSK, &tRect);
   
   tRect.dy = tRect.y - pMe->m_rect.y;
   tRect.y  = pMe->m_rect.y;

   tRect.dx = pMe->m_rect.dx;
   tRect.x  = pMe->m_rect.x;

   // Set the dimensions of the rectangle for the text control
   ISTATIC_SetRect( pIStatic, &tRect);

   // Set the values of the title and text strings for this control
   WSPRINTF(num_to_display, sizeof(num_to_display), fmtNum, acm_value);
   (void) ISTATIC_SetText(pIStatic, 
                          title, 
                          num_to_display,
                          AEE_FONT_BOLD, 
                          AEE_FONT_LARGE);
   
   (void) ISTATIC_Redraw(pIStatic);

   return TRUE;
} //RecentCalls_DisplayACM

/*=============================================================================
FUNCTION: RecentCalls_AocCCMDlgHandler

DESCRIPTION: CCM dialog handler.  All they can do from here is return to the
       previous screen.

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_AocCCMDlgHandler    (CRecentCalls       *pMe,
                                                AEEEvent            eCode,
                                                uint16              wParam,
                                                uint32              dwParam)
{
#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

   switch (eCode)
   {
     case EVT_DIALOG_START:
     {
         IMenuCtl *ctl;
         // Get softkey control to change the looks of sk
         ctl = (IMenuCtl*) IDIALOG_GetControl((IDialog *)dwParam, 
                                                         IDC_CCM_INFO_SK);
         if (ctl) {
           SetDefaultSoftkeyLook(pMe->a.m_pIShell, ctl);
           IMENUCTL_SetActive(ctl, TRUE);
         }

         return RecentCalls_DisplayCCM(pMe);
     }

     case EVT_DIALOG_END:
       return TRUE;

     case EVT_KEY:
       switch ((AVKType) wParam)
       {
         case AVK_CLR:
           // Return to main AOC menu.
           return RecentCalls_MoveToDialog(pMe, IDD_AOC_INFO_DLG);

         default:
           break;
       }
       return TRUE;

     case EVT_COMMAND:
      if (IDL_CCM_INFO_OK_SK == wParam)
      {
         return RecentCalls_MoveToDialog(pMe, IDD_AOC_INFO_DLG);
      } 
      return FALSE;

     default:
       break;
   }
   return FALSE;
}


/*=============================================================================
FUNCTION: RecentCalls_GetACMPinID

DESCRIPTION: 
  This function will get the PIN ID associated with the ACM file in use.

PARAMETERS:
  *pMe: CRecentCalls object pointer 

RETURN VALUE:
   uint8: The PIN ID to pass to ICARD.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint8 RecentCall_GetACMPinID(CRecentCalls *pMe)
{
  AEECardApp CardApp;
  if (ICARD_GetCardApps(pMe->m_pICard, &CardApp) == SUCCESS)
  {
    if ((CardApp.operational_slot &  AEECARD_GSM_SLOT1_OPERATIONAL) ||
        (CardApp.operational_slot & AEECARD_UMTS_SLOT1_OPERATIONAL))
      return AEECARD_PIN2;

    if ((CardApp.operational_slot &  AEECARD_GSM_SLOT2_OPERATIONAL) ||
        (CardApp.operational_slot & AEECARD_UMTS_SLOT2_OPERATIONAL))
      return AEECARD_PIN2_SLOT2;

    MSG_ERROR("No operational slot %x!", CardApp.operational_slot, 0, 0);
  }

  return AEECARD_PIN2;
}

/*=============================================================================
FUNCTION: RecentCalls_SetACMMAX

DESCRIPTION: This function is called once the PIN is verified.  If the PIN
  was successful, it displays the dialog to set the ACMMAX.  If the PIN was
  not successful, then it just shows the ACM MAX value.

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    passed: Did the PIN checks pass?

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean RecentCalls_SetACMMAX(CRecentCalls *pMe, boolean passed)
{
  if (passed)
    return RecentCalls_MoveToDialog(pMe, IDD_ACMMAX_SET_DLG);
  else
    return RecentCalls_MoveToDialog(pMe, IDD_ACMMAX_INFO_DLG);
} 


/*=============================================================================
FUNCTION: RecentCalls_ResetACM

DESCRIPTION: This function is called once the PIN is verified.  If the PIN
  was successful, it resets the ACM back to 0 before returning to the ACM
  dialog.  In either case, the ACM_INFO dialog is shown again.

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    passed: Did the PIN checks pass?

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean RecentCalls_ResetACM(CRecentCalls *pMe, boolean passed)
{
  if (passed)
  {
#ifdef FEATURE_ICM
    (void) ICM_ResetACM(pMe->m_pICM);
#else

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) 
    ui_cm_ph_cmd_reset_acm();
#endif /*  defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
	
#endif
  }

  return RecentCalls_MoveToDialog(pMe, IDD_ACM_INFO_DLG);
} 

/*=============================================================================
FUNCTION: RecentCalls_AocACMDlgHandler

DESCRIPTION: View the ACM dialog handler

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_AocACMDlgHandler    (CRecentCalls       *pMe,
                                                AEEEvent            eCode,
                                                uint16              wParam,
                                                uint32              dwParam)
{
   IMenuCtl *ctl;

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

   switch (eCode) {
   case EVT_DIALOG_START:
      {
         // Get softkey control to change the looks of sk
         ctl = (IMenuCtl*) IDIALOG_GetControl((IDialog *)dwParam, 
                                                         IDC_RESET_ACM_SK);
         if (ctl) {
           SetDefaultSoftkeyLook(pMe->a.m_pIShell, ctl);
           IMENUCTL_SetActive(ctl, TRUE);
         }

         return RecentCalls_DisplayACM(pMe);
      }

   case EVT_DIALOG_END:
      return TRUE;

   case EVT_KEY:
      switch ((AVKType) wParam) {
      case AVK_CLR:
         // Return to main AOC menu
         return RecentCalls_MoveToDialog(pMe, IDD_AOC_INFO_DLG);

      default:
         break;
      }
      return TRUE;

   case EVT_COMMAND:
      /* They have requested the the ACM be reset.  Set the function to
       * call at the completion and then start the PIN screens.
       */
      if (IDL_RESET_SK == wParam){
        pMe->m_fpPIN2_complete_fn = RecentCalls_ResetACM;
        pMe->m_PinID = RecentCall_GetACMPinID(pMe);
        return RecentCalls_MoveToDialog(pMe, IDD_PIN2_REQUEST_DLG);
      }
      break;

   default:
      break;
   }
   return FALSE;
}

/*=============================================================================
FUNCTION: RecentCalls_AocACMMAXDlgHandler

DESCRIPTION: ACMMAX value dialog handler

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_AocACMMAXDlgHandler    (CRecentCalls       *pMe,
                                                AEEEvent            eCode,
                                                uint16              wParam,
                                                uint32              dwParam)
{
   IMenuCtl *ctl;

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

   switch (eCode) {
   case EVT_DIALOG_START:
      {
         // Get softkey control to change the looks of sk
         ctl = (IMenuCtl*) IDIALOG_GetControl((IDialog *)dwParam, 
                                                         IDC_SET_ACMMAX_SK);
         if (ctl) {
           SetDefaultSoftkeyLook(pMe->a.m_pIShell, ctl);
           IMENUCTL_SetActive(ctl, TRUE);
         }
         return RecentCalls_DisplayACMMAX(pMe);
      }

   case EVT_DIALOG_END:
      return TRUE;

   case EVT_KEY:
      switch ((AVKType) wParam) {
      case AVK_CLR:
         // Return to main menu
         return RecentCalls_MoveToDialog(pMe, IDD_AOC_INFO_DLG);

      default:
         break;
      }
      return TRUE;

   case EVT_COMMAND:
      if (IDL_SET_ACMMAX_SK == wParam){
        pMe->m_fpPIN2_complete_fn = RecentCalls_SetACMMAX;
        pMe->m_PinID = RecentCall_GetACMPinID(pMe);
        return RecentCalls_MoveToDialog(pMe, IDD_PIN2_REQUEST_DLG);
      }
      break;

   default:
      break;
   }
   return FALSE;
}


/*=============================================================================
FUNCTION: RecentCalls_AocSetACMMAXDlgHandler

DESCRIPTION: Change the ACMMAX Value.

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_AocSetACMMAXDlgHandler(CRecentCalls       *pMe,
                                                AEEEvent            eCode,
                                                uint16              wParam,
                                                uint32              dwParam)
{
   ITextCtl *text_ctl;
   IMenuCtl *menu_ctl;
   AEERect tRect;
   AECHAR new_acmmax[9];
   IDialog *pd;

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

   switch (eCode) {
   case EVT_DIALOG_START:
     // Get softkey control to change the looks of sk
     text_ctl = (ITextCtl *) IDIALOG_GetControl((IDialog *)dwParam, 
                                                     IDT_ACMMAX_VALUE);

     menu_ctl = (IMenuCtl *) IDIALOG_GetControl((IDialog *)dwParam,
                                                     IDC_EDIT_ACMMAX_SK);
     if (!text_ctl || !menu_ctl)
     {
       MSG_ERROR("Could not allocate control %x %x", text_ctl, menu_ctl, 0);
       return FALSE;
     }

     (void) IMENUCTL_DeleteAll(menu_ctl);
     (void) IMENUCTL_AddItem(menu_ctl, RECENTCALLS_RES_FILE, IDS_OK,
         IDL_EDIT_ACMMAX_OK, NULL, 0);
     (void) IMENUCTL_AddItem(menu_ctl, RECENTCALLS_RES_FILE, IDS_CANCEL,
         IDL_EDIT_ACMMAX_CANCEL, NULL, 0);
     SetDefaultSoftkeyLook(pMe->a.m_pIShell, menu_ctl);

     tRect.x  = pMe->m_rect.x;
     tRect.dx = pMe->m_rect.dx;
     tRect.y  = pMe->m_rect.y;
     tRect.dy = pMe->m_rect.dy / 4 - 10;
     ITEXTCTL_SetRect(text_ctl, &tRect);

     ITEXTCTL_SetProperties(text_ctl, TP_FRAME);
     ITEXTCTL_SetMaxSize(text_ctl, 8);
     (void) ITEXTCTL_SetInputMode(text_ctl, AEE_TM_NUMBERS);
     ITEXTCTL_SetActive(text_ctl, TRUE);
     return TRUE;

   case EVT_DIALOG_END:
     return TRUE;

    case EVT_KEY_PRESS:
      if (((AVKType) wParam) == (AVKType) AVK_SELECT)
      {
        pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        if (!pd) {
          MSG_ERROR("Could not get dialog!", 0, 0, 0);
          return FALSE;
        }
        text_ctl = (ITextCtl *) IDIALOG_GetControl(pd, IDT_ACMMAX_VALUE);
        menu_ctl = (IMenuCtl *) IDIALOG_GetControl(pd, IDC_EDIT_ACMMAX_SK);
        if (!text_ctl || !menu_ctl) {
          MSG_ERROR("Could not get control %x %x!", text_ctl, menu_ctl, 0);
          return FALSE;
        }
        if (!ITEXTCTL_IsActive(text_ctl))
          return FALSE;
        else
        {
          ITEXTCTL_SetActive(text_ctl, FALSE);
          IMENUCTL_SetActive(menu_ctl, TRUE);
          return TRUE;
        }
      } //      if (((AVKType) wParam) =... 

      return FALSE;

   case EVT_KEY:
     switch ((AVKType) wParam) {
       case AVK_CLR:
         return RecentCalls_MoveToDialog(pMe, IDD_ACMMAX_INFO_DLG);

       default:
         return FALSE;
      }

   case EVT_COMMAND:
      switch (wParam)
      {
        case IDL_EDIT_ACMMAX_OK:
         pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
         if (!pd) {
           MSG_ERROR("Could not get dialog!", 0, 0, 0);
           return FALSE;
         }
         text_ctl = (ITextCtl *) IDIALOG_GetControl(pd, IDT_ACMMAX_VALUE);
         if (!text_ctl) {
           MSG_ERROR("Could not get text control!", 0, 0, 0);
           return FALSE;
         }

         if (ITEXTCTL_GetText(text_ctl, new_acmmax, sizeof(new_acmmax)))
         {
           char acmmax_string[9];
           (void) WSTRTOSTR(new_acmmax, acmmax_string, sizeof(acmmax_string));
#ifdef FEATURE_ICM
           (void) ICM_SetACMMax(pMe->m_pICM, (uint32) ATOI(acmmax_string));
#else
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
           ui_cm_ph_cmd_set_acmmax((uint32) ATOI(acmmax_string));
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
#endif
         } 
//lint -fallthrough
        case IDL_EDIT_ACMMAX_CANCEL:
          return RecentCalls_MoveToDialog(pMe, IDD_ACMMAX_INFO_DLG);
        default:
          return FALSE;
      }

   default:
      break;
   }
   return FALSE;
}

/*=============================================================================
FUNCTION: RecentCalls_DrawPIN

DESCRIPTION: Utility function to redraw the PIN prompts.  This function 
   will load the resource string and set the title and prompt for a static
   control.

PARAMETERS:
   *pMe: CRecentCalls object pointer 
   *static_ctl: The static control to apply the changes to.
   title_string: The ID of the string for the title.
   prompt_string: The ID of the string for the prompt.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void RecentCalls_DrawPIN(CRecentCalls       *pMe,
                        IStatic            *static_ctl,
                        uint16 title_string,
                        uint16 prompt_string)
{
   AECHAR *title;
   AECHAR *prompt;

   if (pMe == NULL || static_ctl == NULL)
   {
     MSG_ERROR("Invalid parameter received %x %x", pMe, static_ctl, 0);
     return;
   }

   title = MALLOC(MAX_TITLE_LENGTH * sizeof(AECHAR));
   prompt = MALLOC(MAX_PIN_STATIC_TEXT_LEN * sizeof(AECHAR));
   if (!title || !prompt)
   {
     MSG_ERROR("Malloc failed %x %x", title, prompt, 0);
     FREEIF(title);
     FREEIF(prompt);
     return;
   }

   *title = '\0';
   *prompt = '\0';

   (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                               RECENTCALLS_RES_FILE,
                               title_string,
                               title,
                               MAX_TITLE_LENGTH * sizeof(AECHAR));

   (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                               RECENTCALLS_RES_FILE,
                               prompt_string,
                               prompt,
                               MAX_PIN_STATIC_TEXT_LEN * sizeof(AECHAR));

   (void) ISTATIC_SetText(static_ctl, title, prompt, AEE_FONT_BOLD,
                          AEE_FONT_NORMAL);

   (void) ISTATIC_Redraw(static_ctl);
   FREE(title);
   FREE(prompt);
}

#ifdef FEATURE_NEW_ICARD
/*=============================================================================
FUNCTION: RecentCalls_VerifyPinCb

DESCRIPTION: 
   This function will be called by ICard when the PIN/PUK has been verified.
   The results are already saved, so now we just have to post the event back
   to the APP.   The PIN Dialog handler will get the event and display the
   next prompt/dialog.

PARAMETERS:
   *pUser:  The AEEPINCbData from the command..

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void Recentcalls_VerifyPinCb(void *pNotifyData)
{  
  CRecentCalls * pMe = (CRecentCalls *) pNotifyData;
  AEECardPinStatus sPinStatus;

  if (pMe == NULL)
  {
    MSG_ERROR("Invalid parameter received %x",
        pNotifyData, 0, 0);
    return;
  }

  (void) ICARD_GetPinStatus(pMe->m_pICard, (uint8) pMe->m_sPinActionStatus.nPinID, &sPinStatus); 
  if (!ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_RECENTCALLSAPP,
                        EVT_RECENTCALLS_PIN_COMPLETE,
                        sPinStatus.lock_state, (uint32) pMe))
  {
    MSG_ERROR ("Could not post the event to Recent Calls", 0, 0, 0);
  }
}
#else
/*=============================================================================
FUNCTION: RecentCalls_VerifyPinCb

DESCRIPTION: 
   This function will be called by ICARD when the PIN/PUK has been verified.
   The results are already saved, so now we just have to post the event back
   to the APP.   The PIN Dialog handler will get the event and display the
   next prompt/dialog.

PARAMETERS:
   *pUser:  The AEEPINCbData from the command..

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void Recentcalls_VerifyPinCb(void *pUser)
{
  AEEPINCbData *ICARDdata = pUser;
  CRecentCalls       *pMe = NULL;

  if (pUser == NULL || ICARDdata->user_data == NULL)
  {
    MSG_ERROR("Invalid parameter received %x %x",
        pUser, ICARDdata->user_data, 0);
    return;
  }

  pMe = ICARDdata->user_data;
  if (!ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_RECENTCALLSAPP,
                        EVT_RECENTCALLS_PIN_COMPLETE,
                        ICARDdata->pin_status.lock_state, (uint32) pMe))
  {
    MSG_ERROR ("Could not post the event to Recent Calls", 0, 0, 0);
  }
}
#endif/*FEATURE_NEW_ICARD*/
/*=============================================================================
FUNCTION: RecentCalls_PIN2_RequestDlgHandler

DESCRIPTION: Dialog handler which Verifies the PIN2 value.

PARAMETERS:
   *pMe: CRecentCalls object pointer 
    eCode: Event code
   wParam: 16bit event parameter
  dwParam: 32bit event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean RecentCalls_PIN2_RequestDlgHandler(CRecentCalls       *pMe,
                                                AEEEvent            eCode,
                                                uint16              wParam,
                                                uint32              dwParam)
{
   IDialog *pd;                 // Pointer to our dialog.
   IStatic  *static_ctl;        // Pointer to our static control.
   ITextCtl *text_ctl;          // Pointer to our user input area.
   AEERect tRect;               // Used to size things.
   AECHAR pin_buffer[9];        // Buffer as it is entered.
#ifdef FEATURE_NEW_ICARD
   AEECardPinCode pin_code_buf;     // Data sent to ICARD for the PIN
   AEECardPinCode puk_code_buf;     // Data sent to ICARD for the PUK
   AEECardPinStatus pin_status;     // Returned value of PIN status.
#else
   AEEPINCode pin_code_buf;     // Data sent to ICARD for the PIN
   AEEPINCode puk_code_buf;     // Data sent to ICARD for the PUK
   AEEPinStatus pin_status;     // Returned value of PIN status.
#endif/*FEATURE_NEW_ICARD */
   uint16 prompt_string;         // Which string to use as a prompt.
   uint16 title_string;          // Which string to use as a title.
   int character_height;        // Height of the characters. 
   int string_width;            // Width of the PIN string. "********"
   AECHAR string_to_measure[] = { '*', '*', '*', '*', '*', '*', '*', '*', '\0' };
#define SPACING 6

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif
  if (pMe == NULL)
    return FALSE;

  switch (eCode) {
   case EVT_DIALOG_START:
     // Get softkey control to change the looks of sk
     pMe->m_ePukState = RECENTCALLS_NOT_REQUESTING;
     static_ctl = (IStatic *) IDIALOG_GetControl((IDialog *)dwParam, 
                                                     IDC_PIN2_MSG_STATIC_TEXT);
     text_ctl = (ITextCtl *) IDIALOG_GetControl((IDialog *)dwParam, 
                                                     IDT_PIN2_REQUEST);

     
     character_height = IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay,
                                    AEE_FONT_NORMAL, NULL, NULL);

     string_width = IDISPLAY_MeasureText(pMe->a.m_pIDisplay,
                                    AEE_FONT_NORMAL,
                                    string_to_measure);
     if (!static_ctl || !text_ctl || string_width == 0)
     {
       MSG_ERROR("Bad data return from BREW %x %x %d",
           static_ctl, text_ctl, string_width);
       return FALSE;
     }

     /* Initialize the pin status and then ask ICARD to get us the
      * latest.
      */
     pin_status.lock_state = AEECARD_PIN_UNKNOWN;
     (void) ICARD_GetPinStatus(pMe->m_pICard, pMe->m_PinID, &pin_status);

     tRect.x  = pMe->m_rect.x;
     tRect.dx = pMe->m_rect.dx;
     tRect.y  = pMe->m_rect.y;
     tRect.dy = (int16) ((4 * character_height) + SPACING);
     ISTATIC_SetRect(static_ctl, &tRect);
     ISTATIC_SetProperties(static_ctl, ST_CENTERTEXT | ST_CENTERTITLE | ST_MIDDLETEXT);

     tRect.dx = (int16) (string_width + SPACING);
     tRect.x = pMe->m_rect.x +
               ((pMe->m_rect.dx - pMe->m_rect.x)/2) -
               (tRect.dx / 2);
     tRect.y += tRect.dy + SPACING;
     tRect.dy = (int16) (character_height + SPACING);
     ITEXTCTL_SetRect(text_ctl, &tRect);
     ITEXTCTL_SetProperties(text_ctl, TP_PASSWORD | TP_FRAME);
     ITEXTCTL_SetMaxSize(text_ctl, 8);
     (void) ITEXTCTL_SetInputMode(text_ctl, AEE_TM_NUMBERS);

     switch (pin_status.lock_state)
     {
       case AEECARD_PIN_DISABLED:
       case AEECARD_PIN_VERIFIED:
         // No need for a PIN, just say we are done.
         return (*pMe->m_fpPIN2_complete_fn)(pMe, TRUE);

       case AEECARD_PIN_ENABLED:
         // Need to request the PIN.
         title_string = IDS_PIN2_TITLE;
         prompt_string = IDS_PIN2_REQUEST;
         ITEXTCTL_SetActive(text_ctl, TRUE);
         break;

       case AEECARD_PIN_BLOCKED:
         // Need to request the PUK.
         pMe->m_ePukState = RECENTCALLS_REQUESTING_PUK;
         title_string = IDS_PIN2_IS_BLOCKED;
         prompt_string = IDS_PUK2_REQUEST;
         ITEXTCTL_SetActive(text_ctl, TRUE);
         break;
         
       default:
       case AEECARD_PIN_PERM_DISABLED:
       case AEECARD_PIN_UNKNOWN:
       case AEECARD_PIN_NOT_VALID:
         // Can't get anything, just show the error.
         title_string = IDS_PIN2_INVALID;
         prompt_string = IDS_SERVICE_PROVIDER;
         ITEXTCTL_SetActive(text_ctl, FALSE);
         (void) IDIALOG_SetFocus((IDialog *)dwParam, IDC_PIN2_MSG_STATIC_TEXT);
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_RECENTCALLSAPP,
                           EVT_RECENTCALLS_PIN_DISABLED, 0, (uint32) pMe);
         break;
     } 

     RecentCalls_DrawPIN(pMe, static_ctl, title_string, prompt_string);
     return TRUE;

   case EVT_RECENTCALLS_PIN_DISABLED:
     pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
     if (!pd) {
      return FALSE;
     }
     text_ctl = (ITextCtl *) IDIALOG_GetControl(pd, IDT_PIN2_REQUEST);
     ITEXTCTL_SetActive(text_ctl, FALSE);
     return TRUE;

   case EVT_RECENTCALLS_PIN_COMPLETE:
     // We sent a command and are getting the result.   If it was
     // successful, we can leave by calling the function.
#ifdef FEATURE_NEW_ICARD
     if (pMe->m_sPinActionStatus.nCmdStatus == SUCCESS)
#else
     if (pMe->m_sPin_rsp_data.pin_cmd_status == AEE_SUCCESS)
#endif /*FEATURE_NEW_ICARD */
     {
       return (*pMe->m_fpPIN2_complete_fn)(pMe, TRUE);
     }

     /* Not successful, we need to display a new prompt.  Get our dialog,
      * static and text controls.
      */
     pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
     if (!pd) {
      return FALSE;
     }

     static_ctl = (IStatic *) IDIALOG_GetControl(pd, IDC_PIN2_MSG_STATIC_TEXT);
     text_ctl = (ITextCtl *) IDIALOG_GetControl(pd, IDT_PIN2_REQUEST);
     if (!static_ctl || !text_ctl)
     {
       MSG_ERROR("Could not allocate control %x %x", static_ctl, text_ctl, 0);
       return FALSE;
     }

     /* We will prompt for information by default. */
     ITEXTCTL_SetActive(text_ctl, TRUE);
#ifdef FEATURE_NEW_ICARD
     (void) ICARD_GetPinStatus(pMe->m_pICard, (uint8) pMe->m_sPinActionStatus.nPinID, &pin_status);
     switch (pin_status.lock_state)
#else
     switch (pMe->m_sPin_rsp_data.pin_status.lock_state)
#endif /*FEATURE_NEW_ICARD */
     {
       case AEECARD_PIN_DISABLED:
       case AEECARD_PIN_VERIFIED:
         /* Pin became disabled?  Shouldn't have happened, but if
          * it does, we can call ourselves done.
          */
         return (*pMe->m_fpPIN2_complete_fn)(pMe, TRUE);

       case AEECARD_PIN_ENABLED:
         /* Need to ask for the PIN. */
         prompt_string = IDS_PIN2_REQUEST;
         title_string = IDS_PIN2_INCORRECT;
         break;

       case AEECARD_PIN_BLOCKED:
         /* Need to ask for the PUK. */
         title_string = IDS_PIN2_INCORRECT;
         prompt_string = IDS_PUK2_REQUEST;
         pMe->m_ePukState = RECENTCALLS_REQUESTING_PUK;
         break;

       default:
       case AEECARD_PIN_PERM_DISABLED:
       case AEECARD_PIN_UNKNOWN:
       case AEECARD_PIN_NOT_VALID:
         /* Pin became invalid.  Display the error message. */
         title_string = IDS_PIN2_INVALID;
         prompt_string = IDS_SERVICE_PROVIDER;
         ITEXTCTL_SetActive(text_ctl, FALSE);
         (void) IDIALOG_SetFocus(pd, IDC_PIN2_MSG_STATIC_TEXT);
         break;
     } 

     RecentCalls_DrawPIN(pMe, static_ctl, title_string, prompt_string);
     return TRUE;

   case EVT_DIALOG_END:
     return TRUE;

   case EVT_KEY:
     switch ((AVKType) wParam) {
       case AVK_CLR:
         /* Clearing out, we will leave, but tell our function that
          * the PIN was not verified.
          */
         return (*pMe->m_fpPIN2_complete_fn)(pMe, FALSE);

       case AVK_SELECT:
         /* They entered a NEW value, so get that value and process it. */
         pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
         if (!pd) {
          return FALSE;
         }
         static_ctl = (IStatic *) IDIALOG_GetControl(pd,
                                                     IDC_PIN2_MSG_STATIC_TEXT);
         text_ctl = (ITextCtl *) IDIALOG_GetControl(pd, IDT_PIN2_REQUEST);

         if (!static_ctl || !text_ctl)
         {
           return FALSE;
         }

         if (ITEXTCTL_GetText(text_ctl, pin_buffer, sizeof(pin_buffer)))
         {
           /* Based on the PUK state, our processing changes.  If we were
            * not asking for a PUK, then this is a PIN, Verify it.   If it
            * is a PUK, then ask for the PUK, New Pin, and Verify New Pin.
            * At the completion of everything, request the PIN be unblocked.
            */
           switch (pMe->m_ePukState)
           {
             default:
             case  RECENTCALLS_NOT_REQUESTING:
               /* Process the string as a PIN. */
#ifdef FEATURE_NEW_ICARD
               pMe->m_sCallback.pfnCancel = NULL;
               pMe->m_sCallback.pfnNotify = Recentcalls_VerifyPinCb;
               pMe->m_sCallback.pNotifyData = pMe;
#else
               pMe->m_sPin_rsp_data.user_data = (void*) pMe;
               CALLBACK_Init(&pMe->m_sCallback, Recentcalls_VerifyPinCb,
                   (void *)&(pMe->m_sPin_rsp_data));
#endif /*FEATURE_NEW_ICARD */

               pin_code_buf.code_len = (uint8) WSTRLEN(pin_buffer);
               pin_code_buf.code = pin_buffer;
#ifdef FEATURE_NEW_ICARD
               (void) ICARD_VerifyPin(pMe->m_pICard, pMe->m_PinID, &pin_code_buf,
                                      &pMe->m_sPinActionStatus, &pMe->m_sCallback);
#else
               (void) ICARD_VerifyPin(pMe->m_pICard, pMe->m_PinID, &pin_code_buf,
                                      &pMe->m_sCallback);
#endif /*FEATURE_NEW_ICARD */
               RecentCalls_DrawPIN(pMe, static_ctl,
                   IDS_PIN2_VERIFYING, IDS_PLEASE_WAIT);
               (void) ITEXTCTL_SetText(text_ctl, NULL, 0);
               ITEXTCTL_SetActive(text_ctl, FALSE);
               break;

             case RECENTCALLS_REQUESTING_PUK:
               /* We got the PUK, now request a NEW PIN. */
               (void) WSTRCPY(pMe->m_PUKstring, pin_buffer);
               RecentCalls_DrawPIN(pMe, static_ctl,
                   IDS_PIN2_TITLE, IDS_ENTER_NEW_PIN2);
               (void) ITEXTCTL_SetText(text_ctl, NULL, 0);
               ITEXTCTL_SetActive(text_ctl, TRUE);
               pMe->m_ePukState = RECENTCALLS_REQUESTING_PIN_FIRST;
               break;

             case RECENTCALLS_REQUESTING_PIN_FIRST:
               /* We got the new PIN, now request the PIN again. */
               (void) WSTRCPY(pMe->m_PINstringA, pin_buffer);
               RecentCalls_DrawPIN(pMe, static_ctl,
                   IDS_PIN2_TITLE, IDS_REENTER_NEW_PIN2);
               (void) ITEXTCTL_SetText(text_ctl, NULL, 0);
               ITEXTCTL_SetActive(text_ctl, TRUE);
               pMe->m_ePukState = RECENTCALLS_REQUESTING_PIN_SECOND;
               break;

             case RECENTCALLS_REQUESTING_PIN_SECOND:
               /* We got the PUK, and 2 new PINs, make sure they match. */
               if (WSTRNCMP(pMe->m_PINstringA, pin_buffer,sizeof(pMe->m_PINstringA)))
               {
                 /* PINs did not match, just reprompt for the new PIN. */
                 RecentCalls_DrawPIN(pMe, static_ctl,
                     IDS_NEW_PINS_MUST_MATCH, IDS_ENTER_NEW_PIN2);
                 (void) ITEXTCTL_SetText(text_ctl, NULL, 0);
                 ITEXTCTL_SetActive(text_ctl, TRUE);
                 pMe->m_ePukState = RECENTCALLS_REQUESTING_PIN_FIRST;
               }
               else
               {
                 /* PINs did match, ask ICARD to unblock the PIN. tell the
                  * user to wait. */
#ifdef FEATURE_NEW_ICARD
                 pMe->m_sCallback.pfnCancel = NULL;
                 pMe->m_sCallback.pfnNotify = Recentcalls_VerifyPinCb;
                 pMe->m_sCallback.pNotifyData = pMe;
#else
                 pMe->m_sPin_rsp_data.user_data = (void*) pMe;
                 CALLBACK_Init(&pMe->m_sCallback, Recentcalls_VerifyPinCb,
                     (void *)&(pMe->m_sPin_rsp_data));
#endif/*FEATURE_NEW_ICARD */
                 pin_code_buf.code_len = (uint8) WSTRLEN(pin_buffer);
                 pin_code_buf.code = pin_buffer;
                 puk_code_buf.code_len = (uint8) WSTRLEN(pMe->m_PUKstring);
                 puk_code_buf.code = pMe->m_PUKstring;
#ifdef FEATURE_NEW_ICARD
                 (void) ICARD_UnblockPin(pMe->m_pICard, pMe->m_PinID,
                                         &puk_code_buf, &pin_code_buf,
                                         &pMe->m_sPinActionStatus, &pMe->m_sCallback);
#else
                 (void) ICARD_UnblockPin(pMe->m_pICard, pMe->m_PinID,
                                         &puk_code_buf, &pin_code_buf,
                                         &pMe->m_sCallback);
#endif /*FEATURE_NEW_ICARD */
                 RecentCalls_DrawPIN(pMe, static_ctl,
                     IDS_PIN2_VERIFYING, IDS_PLEASE_WAIT);
                 (void) ITEXTCTL_SetText(text_ctl, NULL, 0);
                 ITEXTCTL_SetActive(text_ctl, FALSE);
                 pMe->m_ePukState = RECENTCALLS_NOT_REQUESTING;
               }
               break;
           }
         } 
         break;

       default:
         break;
      }
      return TRUE;

   default:
      break;
  }
  return FALSE;
}

