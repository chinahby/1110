/*=============================================================================

FILE: DialerApp.c

SERVICES: Dialer Applet

GENERAL DESCRIPTION: Main applet functions

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerApp.c#108 $
$DateTime: 2008/06/24 19:00:37 $
$Author: sgavini $
$Change: 687208 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/24/08   sg      Fix parameter passed to CDialerApp_GenericEventHandler 
                   for EVT_COMMAND and EVT_USER
05/07/08   cvs     Fix off by 1 in string copy
03/19/08   sg      Modify all dialogs to use the new framework
08/22/07   cvs     Add framework for dialogs
08/02/07   cvs     Featurize all secondary display support
12/06/06   cvs     Fix lint issues
12/01/06   AYK     Clean up when CDialerApp_OnAppStart fails due to dialog
                   creation failure.
06/13/06   JAS     Modifying sizing of text input to workaround a BREW bug
06/06/06   RI      Added support for IMS VideoShare.
06/01/06   cvs     fix race condition when multiple EVT_STARTAPP_DIGIT events 
                   are received
05/24/06   JD      Added call to SelectFonts to increase size on VGA
01/31/06   AYK     Added NMASK_CM_OTHER_CALL to registration mask so that
                   dialerApp receives AEECM_EVENT_CALL_DISPLAY notification
                   for the sake of Feature Notification Messages.
12/07/05   ak      Do not release PS data calls when phone powers down
06/10/05   cvs     Support for Smart USB Carkit
06/07/05   sun     Added support for CCBS
05/10/05   AT      Added flag for waiting for origination.
03/11/05   cvs     Optimize Status screen
12/27/04   SUN     Do not display the calling dialog if any call is connected
12/10/04   SUN     Added AppBeinSuspended flag - Dialer misses events between 
                   the time it tries to close itself and when it actually closes
                   This flag helps to capture all those events 
11/23/04   sun     Added support for USB HFK
10/27/04   SUN     Freeup memory on Error
10/08/04   SUN     Added function to Display Data from other Apps
09/27/04   SUN     Added Support for CPHS
09/20/04   SUN     Fixed Mute and Vocoder settings
09/14/04   SUN     Release all resources in FreeAppData
07/27/04   SUN     Start with Calling Screen if there is any call in Orig State
07/27/04   SUN     Removed the Global pointer pszOriginalString
07/26/04   JAS     Added support for secondary display
07/18/04   SUN     Send DTMF for only one active call
07/12/04   PA      Initialize emergency flashed flag to FALSE
07/02/04   SUN     Set DTMFs in OnAppStart
06/24/04   SUN     Vocoder should not be turned on multiple times
04/29/04   SUN     Unmute the phone on app start if call is active
04/26/04   BPW     Set pointers to Null after release
04/14/04   SUN     Start Alerting only after App Resume
04/09/04   SUN     Fixed Lint Errors
04/05/03   BPW     Featurized Diag 
02/03/04   JAS     Added creation of IDiag object and diag registration
                   Added explicit routing of EVT_COMMAND to dialogs
                   Added handling of CALL_STATUS and CALL_DETAIL diag events
03/19/04   sun     Added changes for DTMF
01/22/04   BPW     Optimized NV gets
01/20/03   BPW     DTMF and other memory cleanup and lint errors
01/20/03   BPW     Added API for End Voice Call
01/08/04   BPW     Changed when redial may occur
01/07/03   BPW     Changed how dialogs startup after being suspended
12/19/03   BPW     USSD & MissedCall now uses own dialg info structure
12/18/03   BPW     Added global var for outside number to originate
12/18/03   BPW     Added more SS feedback for requesting and incomplete
12/18/03   BPW     Added base station signalling support
12/05/03   BPW     Added variable for Numedit dialog
12/02/03   BPW     Code review changes, BREW 3.0 support, & minor changes
11/26/03   SUN     Added Support for IALERT
11/25/03   BPW     Modified AnswerVoiceCall to reject the incoming call as well
11/25/03   BPW     Linted for the Saber 6250 build
11/20/03   BPW     Update sent DTMFs when timepause dlg is active
11/19/03   BPW     Added display refresh for Numedit
11/17/03   BPW     Increased Max dialed digits to 40 and made rect dynamic
11/17/03   BPW     Changed the way AVK_POWER is handled
11/14/03   BPW     Turn off ringer after call waiting sound is complete
11/13/03   BPW     Press and hold clear to clear contents of text control
11/13/03   BPW     Register for Test Call events
11/13/03   BPW     Made auto answer timer based.
10/15/03   RAM     Added NW initiated USS Indication String
10/15/03   BW      Made Dialer a true background app and fixed end key press prob
08/29/03   BW      Made Dialer App a background app
08/22/03   BW      Added TimePauseDlg and functionality
08/19/03   BW      Added messages, param checking, and more code clean up
08/14/03   BW      Integration with both ICM and ISuppSvc
07/25/03   BW      Handle incoming call event and close dialer if no ICM
06/19/03   BW      Created Dialer Source File 

=============================================================================*/
//lint -save -e611
//lint -save -e740

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"
#include "AEEAppGen.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AppComFunc.h"
#include "DialerApp.h"
#include "DialerUtils.h"
#include "dialerapp_res.h"
#include "DialerNumEdit.h"
#include "DialerNotif.h"
#include "DialerDlgManager.h"
#include "DialerCalling.h"
#include "DialerIncoming.h"
#include "DialerStatus.h"
#include "DialerDetail.h"
#include "DialerSups.h"
#include "DialerUSSD.h"

#ifdef FEATURE_IMS
#ifdef FEATURE_IMS_VIDEOSHARE
#include "DialerVSEvents.h"
#endif // FEATURE_IMS_VIDEOSHARE
#endif // FEATURE_IMS

#ifdef FEATURE_APP_TEST_AUTOMATION
#include "AEEDiag.h"
#include "DialerDiag.h"
#include "Diag.bid"
#endif

#ifdef FEATURE_APP_RECENTCALLS
#include "recentcalls.bid"
#include "recentcalls.h"
#endif //FEATURE_APP_RECENTCALLS

/*===========================================================================

LOCAL/STATIC DATA

===========================================================================*/
static boolean CDialerApp_Default_EndHandler(CDialerApp *pMe)
{
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
      return FALSE;
   }

   return TRUE;
}

// dialog event handler and refresh function table
static CDlgHandler DlgHandlers[] = {
  {0,                       NULL,                                        
    NULL, NULL, NULL, NULL, NULL, NULL}, // This is for the offset

  {IDD_NUMEDIT,             CDialerApp_GenericEventHandler,          
    CDialerApp_NumEditDlg_DisplayRefresh, CDialerApp_NumEditDlg_Init, 
    CDialerApp_NumEditDlg_KeyHandler, CDialerApp_NumEditDlg_CmdHandler, 
    CDialerApp_NumEditDlg_EndHandler, NULL},

  {IDD_CALLING,             CDialerApp_GenericEventHandler,          
    CDialerApp_CallingDlg_DisplayRefresh, CDialerApp_CallingDlg_Init,
    CDialerApp_CallingDlg_KeyHandler, CDialerApp_CallingDlg_CmdHandler, 
    CDialerApp_CallingDlg_EndHandler, NULL},

  {IDD_SERVICE_PROGRAMMING, CDialerApp_GenericEventHandler, 
    NULL, CDialerApp_SupsServiceProgramming_Init, 
    NULL, CDialerApp_SupsServiceProgramming_CmdHandler, 
    CDialerApp_Default_EndHandler, NULL},

  {IDD_CALL_FORWARDING,     CDialerApp_GenericEventHandler,
    NULL, CDialerApp_SupsCallForwarding_Init, 
    CDialerApp_SupsCallForwarding_KeyHandler, CDialerApp_SupsCallForwarding_CmdHandler,
    CDialerApp_Default_EndHandler, NULL},

  {IDD_SUPS_OPERATION,      CDialerApp_GenericEventHandler,
    NULL, CDialerApp_SupsSupsOperation_Init, 
    CDialerApp_SupsSupsOperation_KeyHandler, CDialerApp_SupsSupsOperation_CmdHandler,
    CDialerApp_Default_EndHandler, NULL},

  {IDD_CF_REGISTER,         CDialerApp_GenericEventHandler,
    NULL, CDialerApp_SupsCFRegister_Init, 
    CDialerApp_SupsCFRegister_KeyHandler, CDialerApp_SupsCFRegister_CmdHandler,
    CDialerApp_SupsCFRegister_EndHandler, NULL},

  {IDD_SERVICE_GROUP,       CDialerApp_GenericEventHandler,
    NULL, CDialerApp_SupsServiceGroup_Init, 
    CDialerApp_SupsServiceGroup_KeyHandler, CDialerApp_SupsServiceGroup_CmdHandler,
    CDialerApp_Default_EndHandler, NULL},

  {IDD_USSD_PASSWORD,       CDialerApp_GenericEventHandler,
    NULL, CDialerApp_USSDPasswordDlg_Init, 
    CDialerApp_USSDPasswordDlg_KeyHandler, CDialerApp_USSDPasswordDlg_CmdHandler,
    CDialerApp_USSDPasswordDlg_EndHandler, NULL},

  {IDD_CFNR_TIMER,          CDialerApp_SupsCFNRTimerDlgHandler,
    NULL, NULL, NULL, NULL, NULL, NULL},

  {IDD_USSD_INFO,           CDialerApp_GenericEventHandler,
   NULL, CDialerApp_USSDInfoDlg_Init, 
   CDialerApp_USSDInfoDlg_KeyHandler, CDialerApp_USSDInfoDlg_CmdHandler,
   CDialerApp_USSDInfoDlg_EndHandler, NULL},

  {IDD_CALL_BARRING,        CDialerApp_GenericEventHandler,
    NULL, CDialerApp_SupsCallBarring_Init, 
    CDialerApp_SupsCallBarring_KeyHandler, CDialerApp_SupsCallBarring_CmdHandler,
    CDialerApp_Default_EndHandler, NULL},

  {IDD_CB_PASSWORD,         CDialerApp_GenericEventHandler,
    NULL, CDialerApp_SupsCBPassword_Init, 
    CDialerApp_SupsCBPassword_KeyHandler, CDialerApp_SupsCBPassword_CmdHandler,
    CDialerApp_SupsCBPassword_EndHandler, NULL},

  {IDD_CALLER_IDENTITY,     CDialerApp_GenericEventHandler,
    NULL, CDialerApp_SupsCallerIdentity_Init, 
    CDialerApp_SupsCallerIdentity_KeyHandler, CDialerApp_SupsCallerIdentity_CmdHandler,
    CDialerApp_Default_EndHandler, NULL},

  {IDD_CHANGE_PASSWORD,     CDialerApp_GenericEventHandler,
    NULL, CDialerApp_SupsChangePassword_Init, 
    CDialerApp_SupsChangePassword_KeyHandler, CDialerApp_SupsChangePassword_CmdHandler,
    CDialerApp_SupsChangePassword_EndHandler, NULL},

  {IDD_WILDCARD,            CDialerApp_GenericEventHandler,
    NULL, CDialerApp_WildCardDlg_Init, 
    CDialerApp_WildCardDlg_KeyHandler, CDialerApp_WildCardDlg_CmdHandler,
    CDialerApp_WildCardDlg_EndHandler, NULL},

  {IDD_NOTIF,               CDialerApp_GenericEventHandler,
    CDialerApp_NotifDlg_DisplayRefresh, CDialerApp_NotifDlg_Init,
    CDialerApp_NotifDlg_KeyHandler, CDialerApp_NotifDlg_CmdHandler,
    CDialerApp_NotifDlg_EndHandler, NULL},

  {IDD_INCOMING,            CDialerApp_GenericEventHandler,
    CDialerApp_IncomingDlg_DisplayRefresh, CDialerApp_IncomingDlg_Init,
    CDialerApp_IncomingDlg_KeyHandler, CDialerApp_IncomingDlg_CmdHandler,
    CDialerApp_Default_EndHandler, NULL},

  {IDD_STATUS,              CDialerApp_GenericEventHandler,
    CDialerApp_StatusDlg_DisplayInit, CDialerApp_StatusDlg_Init,
    CDialerApp_StatusDlg_KeyHandler, CDialerApp_StatusDlg_CmdHandler,
    CDialerApp_StatusDlg_EndHandler, NULL},

  {IDD_INCOM_OPTIONS,       CDialerApp_GenericEventHandler,
    CDialerApp_IncomOptionsDlg_DisplayRefresh, CDialerApp_IncomOptionsDlg_Init,
    CDialerApp_IncomOptionsDlg_KeyHandler, CDialerApp_IncomOptionsDlg_CmdHandler,
    CDialerApp_Default_EndHandler, NULL},

  {IDD_CALL_OPTIONS,        CDialerApp_GenericEventHandler,
    CDialerApp_CallOptionsDlg_DisplayRefresh, CDialerApp_CallOptionsDlg_Init,
    NULL, CDialerApp_CallOptionsDlg_CmdHandler,
    CDialerApp_Default_EndHandler, NULL},

  {IDD_MISSEDCALL,          CDialerApp_GenericEventHandler,
    NULL, CDialerApp_MissedCallDlg_Init,
    CDialerApp_MissedCallDlg_KeyHandler, CDialerApp_MissedCallDlg_CmdHandler,
    CDialerApp_MissedCallDlg_EndHandler, NULL},

  {IDD_DETAIL,              CDialerApp_GenericEventHandler,
    CDialerApp_DetailDlg_DisplayRefresh, CDialerApp_DetailDlg_Init,
    NULL, CDialerApp_DetailDlg_CmdHandler,
    CDialerApp_DetailDlg_EndHandler, NULL},

  {IDD_USSD_PROMPT,         CDialerApp_GenericEventHandler,
    NULL, CDialerApp_USSDPromptDlg_Init, 
    CDialerApp_USSDPromptDlg_KeyHandler, CDialerApp_USSDPromptDlg_CmdHandler,
    CDialerApp_USSDPromptDlg_EndHandler, NULL},

  {IDD_TIMEPAUSE,           CDialerApp_GenericEventHandler,
    CDialerApp_TimePauseDlg_DisplayRefresh, CDialerApp_TimePauseDlg_Init,
    CDialerApp_TimePauseDlg_KeyHandler, CDialerApp_TimePauseDlg_CmdHandler,
    CDialerApp_TimePauseDlg_EndHandler, NULL},

  {IDD_FORWARDING,          CDialerApp_GenericEventHandler,
    CDialerApp_ForwardingDlg_DisplayRefresh, CDialerApp_ForwardingDlg_Init,
    CDialerApp_ForwardingDlg_KeyHandler, CDialerApp_ForwardingDlg_CmdHandler,
    CDialerApp_ForwardingDlg_EndHandler, NULL},

  {IDD_SSREQUEST,           CDialerApp_GenericEventHandler,
    NULL, CDialerApp_SSRequestDlg_Init,
    CDialerApp_SSRequestDlg_KeyHandler, CDialerApp_SSRequestDlg_CmdHandler,
    CDialerApp_Default_EndHandler, NULL},

  {IDD_CCBS_SUPS_OPERATION, CDialerApp_GenericEventHandler,
    NULL, CDialerApp_SupsCCBSDlg_Init,
    CDialerApp_SupsCCBSDlg_KeyHandler, CDialerApp_SupsCCBSDlg_CmdHandler,
    CDialerApp_Default_EndHandler, NULL},

  {IDD_RECALL,              CDialerApp_GenericEventHandler,
    NULL, CDialerApp_RecallDlg_Init,
    CDialerApp_RecallDlg_KeyHandler, CDialerApp_RecallDlg_CmdHandler,
    CDialerApp_Default_EndHandler, NULL},

  {IDD_SETUP_RECALL,        CDialerApp_GenericEventHandler,
    NULL, CDialerApp_SetupRecallDlg_Init,
    CDialerApp_SetupRecallDlg_KeyHandler, CDialerApp_SetupRecallDlg_CmdHandler,
    CDialerApp_Default_EndHandler, NULL},

  {IDD_CCBS_INDEX,          CDialerApp_GenericEventHandler,
    NULL, CDialerApp_CCBSIndexDlg_Init,
    CDialerApp_CCBSIndexDlg_KeyHandler, CDialerApp_CCBSIndexDlg_CmdHandler,
    CDialerApp_Default_EndHandler, NULL},

  {IDD_CCBS_PENDING_LIST,   CDialerApp_GenericEventHandler,
    NULL, CDialerApp_SupsCCBSListDlg_Init,
    CDialerApp_SupsCCBSListDlg_KeyHandler, NULL,
    CDialerApp_Default_EndHandler, NULL},
};


static const AEEAppInfo gaiDialerApp = 
{
  AEECLSID_DIALER,
  DIALERAPP_RES_FILE,
  0,
  0,
  0,
  0,
  0,
  AFLAG_STATIC | AFLAG_PHONE |AFLAG_HIDDEN
};

#ifdef FEATURE_APP_TEST_AUTOMATION
/* The following array defines the events that need to be registered
 * with the IDiag object, along with their attributes.
 */
static IDiagEvtReg CDialerApp_DiagEventReg[] =
{
    //EventId,                          UsesPayload,    UsesDelayedResponse
    {EVT_DIALER_DIAG_CALL_STATUS,       FALSE,          TRUE},
    {EVT_DIALER_DIAG_CALL_DETAIL,       FALSE,          TRUE}
};

// Calculate the number of events being registered
#define DIALERAPP_DIAG_EVENT_REG_SIZE (sizeof(CDialerApp_DiagEventReg) / \
                                       sizeof(IDiagEvtReg))
#endif

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
int             CDialerApp_Load(IShell *ps, void *pHelpers, IModule **pMod);
static int      CDialerApp_CreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj);
static boolean  CDialerApp_InitAppData(IApplet * pi);
static boolean  CDialerApp_EventHandler(CDialerApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static void     CDialerApp_FreeAppData(CDialerApp *pMe);
static boolean  CDialerApp_GetFontMeasurements(CDialerApp *pMe);
static boolean  CDialerApp_OnAppStart(CDialerApp *pMe, uint16 wParam, uint32 dwParam);
static CDlgHandler* CDialerApp_GetDlgTableEntry(uint16 wDlgID);

/*===========================================================================

                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*=============================================================================
FUNCTION: CDialerApp_GetModInfo

DESCRIPTION:  Called by BREW to retrieve information about this module

PARAMETERS:
  *ps: Pointer to IShell object
  **ppClasses: pointer to AEEClSID object
  **pApps: pointer to AEEAppInfo
  *pnApps: uint15
  *pwMinPriv: uint16

RETURN VALUE:
  PFNMODENTRY 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
PFNMODENTRY CDialerApp_GetModInfo(IShell      *ps,
                                  AEECLSID   **ppClasses,
                                  AEEAppInfo **pApps, 
                                  uint16      *pnApps,
                                  uint16      *pwMinPriv)
{ 
  //lint -save -esym(715, ps)
  //lint -save -esym(715, ppClasses)

  PARAM_NOT_REF(ps)
  PARAM_NOT_REF(ppClasses)
  PARAM_NOT_REF(pwMinPriv)

  PRINT_FUNCTION_NAME();
  *pApps = (AEEAppInfo *)&gaiDialerApp;
  *pnApps = 1; 
  *pwMinPriv = PL_SYSTEM;   // Dialer App has system priviliges

  return (PFNMODENTRY) CDialerApp_Load;
}

/*=============================================================================
FUNCTION: CDialerApp_Load

DESCRIPTION:  Load the module into memory

PARAMETERS:
  *ps: Pointer to IShell object
  *pHelpers: void pointer
  **pMod: Pointer to IModule object

RETURN VALUE:
  int 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CDialerApp_Load(IShell *ps, 
                           void     *pHelpers, 
                           IModule **pMod)
{
  PRINT_FUNCTION_NAME();
  return AEEStaticMod_New(sizeof(AEEMod),
                          ps,
                          pHelpers,
                          pMod,
                          (PFNMODCREATEINST) CDialerApp_CreateInstance,
                          (PFNFREEMODDATA) NULL);
}


/*=============================================================================
FUNCTION: CDialerApp_CreateInstance

DESCRIPTION:  Called by BREW to create an instance of an object contained
              in this module.

PARAMETERS:
  ClsId: Class ID for app
  *pIShell: Pointer to IShell object
  *po: Pointer to IModule object
  **ppObj: Void pointer

RETURN VALUE:
  int 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CDialerApp_CreateInstance(AEECLSID ClsId, 
                                     IShell *pIShell, 
                                     IModule *po, 
                                     void **ppObj)
{
  *ppObj = NULL;

  PRINT_FUNCTION_NAME();
  if(ClsId == AEECLSID_DIALER)
  {
    if (AEEApplet_New(sizeof(CDialerApp), ClsId, pIShell, po,
       (IApplet**)ppObj, (AEEHANDLER) CDialerApp_EventHandler,
      (PFNFREEAPPDATA) CDialerApp_FreeAppData) == TRUE) 
    {
      if(CDialerApp_InitAppData((IApplet*)*ppObj) == TRUE)
      {
        return (AEE_SUCCESS);
      }
      else
      {
        (void) IAPPLET_Release(((IApplet *) *ppObj));
        // FreeAppData will be called by BREW
        *ppObj = NULL;
        DIALER_ERR("InitAppData Failed", 0, 0, 0);
        return (EFAILED);
      }
    }
    else
    {
      return (ENOMEMORY);
    }
  }

  DIALER_ERR("Class ID Mismatch", 0, 0, 0);
  return (EFAILED);
}


/*=============================================================================
FUNCTION: CDialerApp_InitAppData

DESCRIPTION: Initializes the application data

PARAMETERS:
  *pi: pointer to IApplet object

RETURN VALUE:
  boolean

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_InitAppData(IApplet * pi)
{
  CDialerApp * pMe = (CDialerApp*) pi;
  AEEDeviceInfo pDeviceInfo;
  int i;
#ifdef FEATURE_APP_TEST_AUTOMATION
  AECHAR        AppVersion[MAX_APP_VERSION_SIZE];
  uint32        IDiagRegisterSuccess;
#endif
#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
  int           nReturnStatus;
#endif

  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  // check dialog handler list for errors
  for(i=1; i<((int)ARR_SIZE(DlgHandlers)); i++)
  {
    if((i+DIALERAPP_DIALOG_OFFSET) != DlgHandlers[i].wDlgID)
    { 
      // this is a fail safe check to make sure that the dlg handlers
      // match up with each dialog ID
      ERR_FATAL("Dialog Handler ID Mismatch", 0, 0, 0);
    }
    DlgHandlers[i].pDialerApp = pMe;
  }

  // init suspended var
  pMe->m_bSuspended = FALSE;
  pMe->m_bAppBeingSuspended = FALSE;

  //init m_EndedCalls
  for(i=0; i<DIALERAPP_MAX_NUM_CALLS; i++)
  {
    pMe->m_EndedCalls[i] = NULL;
  }

  pMe->m_bOKToRefreshStatusDlg = FALSE;

  //Whether there were calls to display last time through StatusRefresh
  pMe->m_bLastCallsToDisplay = FALSE;

  //what the active calls were last time through Status refresh
  for(i=0; i<DIALERAPP_MAX_NUM_CALLS; i++)
  {
    pMe->m_bLastActiveCalls[i] = FALSE;
  }

  for(i=0; i<DIALERAPP_MAX_NUM_CALLS; i++)
  {
    pMe->m_szCallNames[i] = NULL;
  }

  // init num missed calls
  pMe->m_wNumMissedCalls = 0;

  // init the sound
  uisnd_snd_init();

  // init one touch flag
  pMe->m_bOneTouchFromCore = FALSE;

  // init notif specific time vars
  pMe->m_dwNotifMinTime = 0;
  pMe->m_dwNotifStartTime = 0;

  // init call failed snd
  pMe->m_CallFailedAlert = AEEALERT_ALERT_NONE;

  // init the muted flag
  pMe->m_bMuted = TRUE;

  // init Numedit dlg flag
  pMe->m_wNextNumEditDlg = 0;

  // init flag for speed dial
  pMe->m_bSpeedDialing = FALSE;

  // init flag for emergency flashed
  pMe->m_bEmergencyFlashed = FALSE;
  
  // init start flag for icon select through core
  pMe->m_bIconStartedApp = FALSE;

  // init flag for orig voice call
  pMe->m_bOrigVoiceCall = FALSE;

  // set dtmf flag
  pMe->m_CurrDTMF = 0;

  // set global for text control clearing
  pMe->m_wTextCtlIDToClear = 0;

  // set the startup dialog info
  pMe->m_wStartupDlgID = 0;
  pMe->m_pStartupDlgInfo = NULL;
  pMe->m_dwStartupDlgInfoSize = 0;

  // get device info
  pDeviceInfo.wStructSize = sizeof(pDeviceInfo);
  ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &pDeviceInfo);

  // set screen size
  pMe->m_rc.x = 0;
  pMe->m_rc.y = 0;
  pMe->m_rc.dx = (int16) pDeviceInfo.cxScreen;
  pMe->m_rc.dy = (int16) pDeviceInfo.cyScreen;

  // get memory for strings
  pMe->m_pszDialString = (AECHAR*) MALLOC(DIALERAPP_NUMBER_BUFFER_SIZE*sizeof(AECHAR)); 
  pMe->m_pszTimePauseSent = (AECHAR *) MALLOC((DIALERAPP_NUMBER_BUFFER_SIZE*2)*sizeof(AECHAR));
  pMe->m_pszTimePauseRemaining = (AECHAR *) MALLOC(DIALERAPP_NUMBER_BUFFER_SIZE*sizeof(AECHAR));
  pMe->m_pszCallWaitingNum = (AECHAR *) MALLOC(DIALERAPP_NUMBER_BUFFER_SIZE*sizeof(AECHAR));
  pMe->m_pszDisplayString =  (AECHAR*) MALLOC(DIALERAPP_NUMBER_BUFFER_SIZE*sizeof(AECHAR)); 
  if((pMe->m_pszTimePauseRemaining == NULL) || (pMe->m_pszTimePauseSent == NULL) || 
    (pMe->m_pszDialString == NULL) || (pMe->m_pszCallWaitingNum  == NULL) ||
    (pMe->m_pszDisplayString == NULL))
  {
      DIALER_ERR("No Memory", 0, 0, 0);
      CDialerApp_FreeAppData(pMe);
      return FALSE;
  }
  

  // create vector for notif message queue
  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_VECTOR, 
    (void **) &pMe->m_pNotifMsgQueue) != SUCCESS)
  {
    CDialerApp_FreeAppData(pMe);
    DIALER_ERR("VECTOR Create Failed", 0, 0, 0);
    return FALSE;
  }
  if (pMe->m_pNotifMsgQueue == NULL) 
  {
    DIALER_ERR("Null pointer", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }

  // init the vector notif message queue with maximum of DIALERAPP_MAX_NOTIF_MSGS messages
  if(IVector_Init(pMe->m_pNotifMsgQueue, DIALERAPP_MAX_NOTIF_MSGS) != SUCCESS)
  {
    DIALER_ERR("VECTOR Init Failed", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }

  // create vector for dialog stack
  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_VECTOR, 
                        (void **) &pMe->m_pDialogStack) != SUCCESS)
  {
    DIALER_ERR("VECTOR Create Failed", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }
  if (pMe->m_pDialogStack == NULL) 
  {
    DIALER_ERR("Null pointer", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }

  // init the vector dialog stack with maximum of 32 dialogs
  if(IVector_Init(pMe->m_pDialogStack, 32) != SUCCESS)
  {
    DIALER_ERR("VECTOR Init Failed", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }
    
  // Create the ICallManager object.
  if(ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_CM, 
                        (void **) &pMe->m_pICM) != SUCCESS)
  {
    DIALER_ERR("ICM Create Failed", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }
  if (pMe->m_pICM == NULL) 
  {
    DIALER_ERR("Null pointer", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }

    // Register with ICallManager to recieve events
  // Added NMASK_CM_OTHER_CALL so that dialerApp receives AEECM_EVENT_CALL_DISPLAY notification
  // for the sake of Feature Notification Messages
  if(ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_DIALER,
      AEECLSID_CM_NOTIFIER, NMASK_CM_VOICE_CALL | NMASK_CM_TEST_CALL |
      NMASK_CM_PHONE | NMASK_CM_OTHER_CALL) != SUCCESS)
  {
    DIALER_ERR("ICM Register Notify Failed", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }

  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_ALERT, 
                        (void **) &pMe->m_pIALERT) != SUCCESS)
  {
    DIALER_ERR("IALERT Create Failed", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }
  if (pMe->m_pIALERT == NULL) 
  {
    DIALER_ERR("Null pointer", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }

  // Set the Alerting to False
  pMe->m_bStartAlerting = FALSE;

  if(ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_DIALER,
    AEECLSID_ALERT_NOTIFIER, NMASK_ALERT_ONOFF | NMASK_ALERT_MUTED) != SUCCESS)
  {
    DIALER_ERR("IALERT Register Notify Failed", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  // Create the Supplementary Serivce object.
  if(ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SUPPSVC, 
                        (void **) &pMe->m_pISUPPSVC) != SUCCESS)
  {
    DIALER_ERR("SUPPSVC Create Failed", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }
  if(pMe->m_pISUPPSVC == NULL) 
  {
    DIALER_ERR("Null pointer", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }

  // Register with SUPPSVC to recieve events
  if(ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_DIALER,
      AEECLSID_SUPPSVC_NOTIFIER, NMASK_SS_STARTED | NMASK_SS_END | 
                                 NMASK_SS_COMPLETE | NMASK_SS_NW_INITIATED | 
                                 NMASK_SS_NW_RESPONSE) != SUCCESS)
  {
    DIALER_ERR("ISUPPSVC Register Notify Failed", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }
  
  // init ss flag
  pMe->m_bWizardSKDisplayed = FALSE;

#endif //defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

  
  // create addrbook object
  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_ADDRBOOK,(void **) &pMe->m_pIADDRBOOK) != SUCCESS)
  {
    DIALER_ERR("ADDRBOOK Create Failed", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE;
  }
  if(pMe->m_pIADDRBOOK == NULL) 
  {
    DIALER_ERR("Null pointer", 0, 0, 0);
    CDialerApp_FreeAppData(pMe);
    return FALSE; //error
  } 
  
  // init notif dialog members
  pMe->m_dwNotifMinTime = 0;

  // set background colors
  (void) IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_SYS_WIN, MAKE_RGB(255, 255, 204));
  (void) IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_SYS_TITLE, MAKE_RGB(255, 255, 204));

  // increase font size on VGA displays
  AppComFunc_SelectFonts(pMe->a.m_pIDisplay);
  
  // init NV settings
  pMe->m_bNVAutoHyphen = CDialerApp_NVCheck_IsAutoHyphen();
  pMe->m_byNVDTMF  = CDialerApp_NV_GetDTMF();
  pMe->m_bNVOneTouch = CDialerApp_NVCheck_IsOneTouch();

#ifdef FEATURE_APP_TEST_AUTOMATION
  // create IDiag object
  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_DIAG,
                           (void **)&pMe->m_pDiag) != SUCCESS)
  {
    DIALER_ERR("IDiag Create Failed", 0, 0, 0);
  }
  else
  {

    STRTOWSTR(DIALERAPP_APP_VERSION, AppVersion, MAX_APP_VERSION_SIZE);

    /* Now register for diagnostics.  Don't fail init if we cannot
     * register, but go ahead and release the object if registration fails
     */
    IDiagRegisterSuccess = IDIAG_RegisterApp(pMe->m_pDiag, AEECLSID_DIALER,
                                             AppVersion,
                                             DIALERAPP_DIAG_EVENT_REG_SIZE,
                                             CDialerApp_DiagEventReg);
    if (IDiagRegisterSuccess != SUCCESS)
    {
      DIALER_ERR("IDiag Registration Failed", 0, 0, 0);
      IDIAG_Release(pMe->m_pDiag);
    }

  }

  pMe->m_pDlgData = NULL;
  pMe->m_pCallStatus = NULL;
#endif //FEATURE_APP_TEST_AUTOMATION

#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
  /* Create secondary display */
  pMe->m_pSecText = NULL;
  pMe->m_dwStartTS = 0;
  pMe->m_dwEndTS = 0;
  pMe->m_DispState = DIALER_DISP_STATE_NONE;
  pMe->m_bIsActiveCalls = FALSE;
  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_DISPLAY2,
                                        (void **)&pMe->m_pIDisplay2);
  if(nReturnStatus != SUCCESS)
  {
    pMe->m_pIDisplay2 = NULL;
  }
#endif //FEATURE_UI_SECONDARY_DISP_SUPPORT

  pMe->m_bVocoder = FALSE;

  //wait for Status Result only when CPHS is enabled 
  pMe->m_bWaitForStatusResult = FALSE;

  pMe->m_bWaitingforOrigVoiceCall = FALSE;

 // init font sizes
  if(CDialerApp_GetFontMeasurements(pMe) == FALSE)
  {
    CDialerApp_FreeAppData(pMe);
  }

  return TRUE;

}

/*=============================================================================
FUNCTION: CDialerApp_GetFontMeasurements

DESCRIPTION: Takes measurements and stores the values of specific text that is
             used.

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_GetFontMeasurements(CDialerApp *pMe)
{
  AECHAR szSpace[] = {' ','\0'};
  AECHAR szTime[] = {'9','9','9','9',':','0','0','\0'}; 
  AECHAR *pszMaxDigits = NULL;
  int nMaxNumLength;
  int nRows;
  int nFontAscent;
  int nFontDescent;
  int i;

  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  // get the space length
  pMe->m_dwSpaceLength = (uint32) IDISPLAY_MeasureText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szSpace); 
  if(pMe->m_dwSpaceLength == 0)
  {
    DIALER_ERR("Div by Zero", 0, 0, 0);
    return FALSE;
  }

  // get the time length
  pMe->m_dwTimeLength = (uint32) IDISPLAY_MeasureText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szTime); 
  if(pMe->m_dwTimeLength == 0)
  {
    DIALER_ERR("Div by Zero", 0, 0, 0);
    return FALSE;
  }

  // figure out size of dialed digits text control rectangle
  pszMaxDigits = (AECHAR*) MALLOC(sizeof(AECHAR) * DIALERAPP_NUMBER_BUFFER_SIZE);
  if(pszMaxDigits == NULL)
  {
    DIALER_ERR("No Memory", 0, 0, 0);
    return FALSE;
  }

  // measure text
  for(i=0; i<(DIALERAPP_NUMBER_BUFFER_SIZE-1); i++)
  {
    pszMaxDigits[i] = '0';
  } 
  nMaxNumLength = (int) IDISPLAY_MeasureText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, pszMaxDigits); 
  
  // sides are fixed
  pMe->m_DialedDigitsRect.x = 10; 
  pMe->m_DialedDigitsRect.dx = (int16) (pMe->m_rc.dx - 20);
 
  if(nMaxNumLength%(pMe->m_DialedDigitsRect.dx-6) == 0)
  {
    nRows = (nMaxNumLength/(pMe->m_DialedDigitsRect.dx-6));
  }
  else
  {
    nRows = (nMaxNumLength/(pMe->m_DialedDigitsRect.dx-6))+1;
  }

  // set the top
  pMe->m_DialedDigitsRect.y = 10; 

  // special check for only one row so that large font will still be displayed
  if(nRows == 1)
  {
    (void) IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay, AEE_FONT_LARGE, &nFontAscent, &nFontDescent);

    /* Workaround for a display problem in ITextCtl; this is a magic
     * formula that tells how big it must be until the real fix is in place.
     */
    pMe->m_DialedDigitsRect.dy = (int16)(nFontAscent+nFontDescent);
    pMe->m_DialedDigitsRect.dy = (pMe->m_DialedDigitsRect.dy * 2) + 4; 
  }
  else
  {
    (void) IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, &nFontAscent, &nFontDescent); 
    pMe->m_DialedDigitsRect.dy = (int16)((nRows*(nFontAscent+nFontDescent))+6+((nRows-1)*(10)));
  }

  // make sure we are not exceeding the length of screen
  if((pMe->m_DialedDigitsRect.y + pMe->m_DialedDigitsRect.dy) > (pMe->m_rc.dy-21))
  {
    pMe->m_DialedDigitsRect.dy =(int16)((pMe->m_rc.dy-21) - (2*pMe->m_DialedDigitsRect.y));
  }

  FREEIF(pszMaxDigits);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_EventHandler

DESCRIPTION:  Dialer Applet event handler

PARAMETERS:
  *pMe: CDialerApp object pointer
  eCode: Event code
  wParam: word event parameter
  dwParam: dword event parameter

RETURN VALUE:
  boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_EventHandler(CDialerApp *pMe, 
                                      AEEEvent    eCode, 
                                      uint16      wParam, 
                                      uint32      dwParam)
{
  const CDlgHandler *pDlgTableEntry = NULL;

  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (eCode) 
  {
  case EVT_APP_START:
    {   
      DIALER_MSG("EVT_APP_START", 0, 0, 0);   
      
      if(pMe->m_bSuspended == FALSE)
      {
        // starting for the first time on power up
        return CheckAEEReturnStatus(ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE));
      }
      else
      { 
        // reset the suspended flag
        pMe->m_bSuspended = FALSE;

        return CDialerApp_OnAppStart(pMe, wParam, dwParam);
      }
    }

  

  case EVT_APP_STOP:
    {
      boolean *pb = (boolean *)dwParam;
      DIALER_MSG("EVT_APP_STOP", 0, 0, 0);
      
      if(pb) // brew magic var
      {
        // Set the app to background app
        *pb = FALSE;   
      }

      // set the suspended flag
      pMe->m_bSuspended = TRUE;

      // perform cleanup
      return CDialerApp_OnAppStop(pMe, wParam, dwParam);
    }
    

  case EVT_APP_SUSPEND:
    {
      AEESuspendInfo * pi = (AEESuspendInfo *)dwParam;
      DIALER_MSG("EVT_APP_SUSPEND", 0, 0, 0);

      if(pi) // brew magic var
      {
        // do not kill dialog stack
        pi->bCloseDialogs = FALSE; 
      }

      // set the suspended flag
      pMe->m_bSuspended = TRUE;

      return CDialerApp_OnAppSuspend(pMe, wParam, dwParam); 
    }

  case EVT_APP_RESUME:
    {
      DIALER_MSG("EVT_APP_RESUME", 0, 0, 0);

      // reset the suspended flag
      pMe->m_bSuspended = FALSE;

      // decide what dialog to display
      return CDialerApp_OnAppStart(pMe, wParam, dwParam);
    }

  case EVT_KEY:
    switch(wParam)
    {

      case AVK_POWER:
        DIALER_MSG("AVK_POWER", 0, 0, 0);
        // check for active calls
        // Do not release for PS data calls CM does the needful.
        if(AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM) == TRUE)
          {
          (void)CDialerApp_EndVoiceCall(pMe->a.m_pIShell);
        }
        (void) CDialerApp_StopApp(pMe);
        // do not swallow this event
        return FALSE;

      case AVK_CLR:
        DIALER_MSG("AVK_CLR", 0, 0, 0);
        // end current dialog
        return CDialerApp_EndDlg(pMe);

      case AVK_END:
        DIALER_MSG("AVK_END", 0, 0, 0); 
        // handle the end key event
        return CDialerApp_HandleEndKey(pMe);

      case AVK_SEND:
        DIALER_MSG("AVK_SEND", 0, 0, 0); 
        // check to see if we are in a call
        if(AEECM_IS_VOICECALL_PRESENT(pMe->m_pICM) == TRUE)
        {
          AEECMCallID nCallID;

          // perform flash
          if(CheckAEEReturnStatus(ICM_OriginateVoiceCall(pMe->m_pICM, NULL, &nCallID)) == FALSE)
          {
            DIALER_ERR("ICM_OriginateVoiceCall FAILED", 0, 0, 0);  
            return FALSE;
          }
        }
#ifdef FEATURE_APP_RECENTCALLS
        else
        {
          uint16 wActiveDlg = CDialerApp_GetActiveDlgID(pMe);

          // bring up call history for redial if the user just pressed send
          if((wActiveDlg != IDD_NUMEDIT) && 
             (wActiveDlg != IDD_WILDCARD))
          {
            (void) ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_RECENTCALLSAPP,
              RECENTCALLS_REDIAL_SCREEN_STR);
          }
        }
#endif //FEATURE_APP_RECENTCALLS
        return TRUE;


      default:
      return FALSE;
    }

  case EVT_KEY_PRESS:
        {
      DIALER_MSG("EVT_KEY_PRESS: %d", wParam, 0, 0);
      
          if(wParam == (uint16)AVK_UNDEFINED)
          {
            // do nothing on these because they are not real keypresses
            return FALSE;
          }

          // store the last key pressed
          pMe->m_LastKeyPressed = (AECHAR) AVKType2ASCII((AVKType)wParam);

          switch(pMe->m_LastKeyPressed)
          {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            (void) CDialerApp_StartTimer_SpeedDial(pMe);
            // fall through
          case '*':
          case '#':
            // perform key press dtmf 
            return CDialerApp_StartKeyPressDTMF(pMe, pMe->m_LastKeyPressed);

          default:
            // event not handled
            return FALSE;
          }
        }

  case EVT_KEY_RELEASE:
    {
      DIALER_MSG("EVT_KEY_RELEASE: %d", wParam, 0, 0);
      switch(pMe->m_LastKeyPressed)
      {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        (void) CDialerApp_StopTimer_SpeedDial(pMe);
        // fall through
      case '*':
      case '#':
        // stop the DTMF
        (void) CDialerApp_StopKeyPressDTMF(pMe);

        // special case that occurs during speed dialing
        // we do not want to handle release event if a 
        // speed dial is being performed
        if(pMe->m_bSpeedDialing == TRUE)
        {
          pMe->m_bSpeedDialing = FALSE;
          // key has been handled so reset it
          pMe->m_LastKeyPressed = 0;
          return TRUE; 
        }

        switch(CDialerApp_GetActiveDlgID(pMe))
        {
          // need input on these dialogs so don't create numedit
        case IDD_TIMEPAUSE:
          {
            // add the dtmf to the end of sent string
            uint32 dwStrLen = (uint32) WSTRLEN(pMe->m_pszTimePauseSent);

            if(dwStrLen < ((DIALERAPP_NUMBER_BUFFER_SIZE*2)-1))
            {
              pMe->m_pszTimePauseSent[dwStrLen] = pMe->m_LastKeyPressed;
              pMe->m_pszTimePauseSent[dwStrLen+1] = 0;

              (void) CDialerApp_TimePauseDlg_DisplayRefresh(pMe);
            }
          }
          // fall through      
        case IDD_NUMEDIT:
        case IDD_WILDCARD: 
        case IDD_NOTIF:
        case IDD_USSD_INFO:                   
        case IDD_FORWARDING:
        case IDD_USSD_PASSWORD:
        case IDD_CF_REGISTER:           
        case IDD_CB_PASSWORD:           
        case IDD_CHANGE_PASSWORD:
        case IDD_CCBS_INDEX:
          // key has been handled so reset it
          pMe->m_LastKeyPressed = 0;
          return TRUE;   

          // start the Numedit dialog otherwise
        default:
          pMe->m_pszDialString[0] = pMe->m_LastKeyPressed; 
          pMe->m_pszDialString[1] = '\0';

          // create the numedit dialog
          CDialerApp_CreateDlg(pMe, IDD_NUMEDIT, NULL, 0);
          // First Key was not sent to IDD_NUMEDIT. Send Again
          (void) CDialerApp_StopKeyPressDTMF(pMe);

          // key has been handled so reset it
          pMe->m_LastKeyPressed = 0;
          return TRUE;

      }

      default:
        // event not handled
        return FALSE;
      }
    }

  case EVT_DIALOG_INIT: 
    {
      pDlgTableEntry = CDialerApp_GetDlgTableEntry(wParam);

      DIALER_MSG("EVT_DIALOG_INIT", 0, 0, 0);
      // set the event handler for the started dialog
      if(pDlgTableEntry != NULL) 
      { 
        // check for array bound 
        if (pDlgTableEntry->DlgHandlerFunc != NULL) 
        {
          IDIALOG_SetEventHandler((IDialog *) dwParam,
                                  pDlgTableEntry->DlgHandlerFunc, 
                                  (pDlgTableEntry->DlgHandlerFunc == CDialerApp_GenericEventHandler) ? (void*)pDlgTableEntry : (void*)pMe);
          IDIALOG_SetProperties((IDialog *) dwParam, DLG_HANDLE_ALL_EVENTS);
          return TRUE;
        }
      }
      return FALSE;
    }
  
  case EVT_NOTIFY:
    // Let Dialog Manager handle notify events
    DIALER_MSG("EVT_NOTIFY: %d", wParam, 0, 0);
    return CDialerApp_DlgManager(pMe, eCode, wParam, dwParam);

  case EVT_COMMAND:
    {
      DIALER_MSG("EVT_COMMAND: %d %d", wParam, dwParam, 0);

      // pass event to dialog handlers to see if it is theirs
      pDlgTableEntry = CDialerApp_GetDlgTableEntry(CDialerApp_GetActiveDlgID(pMe));

      if(pDlgTableEntry != NULL)
      {
        // find the appropriate event handler
        if (pDlgTableEntry->DlgHandlerFunc != NULL)
        {
          if (pDlgTableEntry->DlgHandlerFunc((void *)pDlgTableEntry, eCode, wParam, dwParam)
              == TRUE)
          {
            // event handled by dialog event handler
            return TRUE;
          }
          else
          {
            return FALSE;
          }
        }
      }

      DIALER_MSG("EVT_COMMAND: %d %d %d Not Handled", eCode, wParam, dwParam);
      return FALSE;

   }
  
  case EVT_USER:
    {
      uint16 wNumOrigCallIDs = 0;
      
      DIALER_MSG("EVT_USER: %d", wParam, 0, 0);
     
      // check for app specfic user events
      switch(wParam)
      {
      case EVT_DIALER_STARTAPP_ICON:
        // started dialer from the icon
        DIALER_MSG("EVT_DIALER_STARTAPP_ICON", 0, 0, 0);
        if(pMe->m_bSuspended == FALSE)
        {
          DIALER_MSG("Dialer App is already started", 0, 0, 0);
          return FALSE;
        }

        wNumOrigCallIDs = ICM_GetActiveCallIDs(pMe->m_pICM, (AEECMCallType)(AEECM_CALL_TYPE_NONE), 
                                               (AEECMCallState)AEECM_CALL_STATE_ORIG, NULL, 
                                               0);

        /*Display Calling ONLY if no other calls are up*/ 
        if(wNumOrigCallIDs >0 && !AEECM_IS_ANYCALL_CONNECTED(pMe->m_pICM))
        {
          pMe->m_wStartupDlgID = IDD_CALLING;
        }
        else
        {
          pMe->m_wStartupDlgID = IDD_STATUS;
        }

        pMe->m_bIconStartedApp = TRUE;
        return(CheckAEEReturnStatus(ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_DIALER)));

      case EVT_USB_GENERIC_BUTTON_PRESS:
        DIALER_MSG("EVT_USB_GENERIC_BUTTON_PRESS", 0, 0, 0);
        if(AEECM_IS_ANYCALL_CONNECTED(pMe->m_pICM))
        {
          CDialerApp_EndVoiceCall(pMe->a.m_pIShell);
        }
        else
          CDialerApp_AnswerVoiceCall(pMe->a.m_pIShell, TRUE);
        
        return TRUE;
        
      case EVT_DIALER_STARTAPP_DIGIT: 
        {
          AECHAR dial_string[2];

          // core app passed us a digit so start dialer with digit and numedit displayed
          DIALER_MSG("EVT_DIALER_STARTAPP_DIGIT", 0, 0, 0);
          if(pMe->m_bSuspended == FALSE)
          {
            DIALER_MSG("Dialer App is already started", 0, 0, 0);
            return FALSE;
          }

          dial_string[0] = (AECHAR) dwParam;
          dial_string[1] = '\0';

          WSTRLCAT(pMe->m_pszDialString, dial_string, DIALERAPP_NUMBER_BUFFER_SIZE);

          (void) CDialerApp_StartKeyPressDTMF(pMe, (AECHAR) dwParam);
          pMe->m_wStartupDlgID = IDD_NUMEDIT;
          pMe->m_LastKeyPressed  = (AECHAR) dwParam;
          pMe->m_bOneTouchFromCore = TRUE;
          pMe->m_bNVOneTouch = CDialerApp_NVCheck_IsOneTouch();
          (void) CDialerApp_StartTimer_SpeedDial(pMe);
          return(CheckAEEReturnStatus(ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_DIALER)));
        }

      case EVT_DIALER_STARTAPP_ORIGINATEVOICECALL:
        // originate voice call and free the memory passed to us
        DIALER_MSG("EVT_DIALER_STARTAPP_ORIGINATEVOICECALL", 0, 0, 0);
      
        // error check
        if((AECHAR*)dwParam == NULL)
        {
          DIALER_ERR("Null Pointer", 0, 0, 0);
          return FALSE;
        }

        (void) MEMSET(pMe->m_pszDialString, 0, (sizeof(AECHAR)*DIALERAPP_NUMBER_BUFFER_SIZE));  
        (void) MEMCPY(pMe->m_pszDialString, (AECHAR*)dwParam, (sizeof(AECHAR)*DIALERAPP_NUMBER_BUFFER_SIZE));   

        // check to see if we are already running
        if(pMe->m_bSuspended == FALSE)
        {
          // This will occur if the app who originated the voice call ends itself right after the
          // originate voice call funtion.  In this case the app start event will arrive before the
          // EVT_DIALER_STARTAPP_ORIGINATEVOICECALL event so we will already be running and we just have
          // to originate the voice call.

          // an event will create the next dialog
          if(CDialerApp_OriginateVoiceCall(pMe, pMe->m_pszDialString) == FALSE)
          {
            // do not want to start the app if we cannot originate
            DIALER_ERR("Unable to Originate Voice Call - Stopping Dialer", 0, 0, 0);
            (void) CDialerApp_StopApp(pMe);
          }
          return TRUE;
        }

        // dialer is not started so start it
        pMe->m_bOrigVoiceCall = TRUE;
        pMe->m_wStartupDlgID = 0;
        return(CheckAEEReturnStatus(ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_DIALER)));

      case EVT_DIALER_STARTAPP_DISPLAY_STRING:
        DIALER_MSG("EVT_DIALER_STARTAPP_DISPLAY_STRING", 0, 0, 0);
      
        // error check
        if((AECHAR*)dwParam == NULL)
        {
          DIALER_ERR("Null Pointer", 0, 0, 0);
          return FALSE;
        }

        (void) MEMSET(pMe->m_pszDisplayString, 0, (sizeof(AECHAR)*DIALERAPP_NUMBER_BUFFER_SIZE));  
        (void) MEMCPY(pMe->m_pszDisplayString, (AECHAR*)dwParam, (sizeof(AECHAR)*DIALERAPP_NUMBER_BUFFER_SIZE));   

         CDialerApp_NotifDlg_DisplayMsgEx(pMe,  pMe->m_pszDisplayString, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
         return TRUE;
      case EVT_DIALER_MULTI_COMPLETE:
        // stop the multi DTMF burst sound
        DIALER_MSG("EVT_DIALER_MULTI_COMPLETE", 0, 0, 0);
        uisnd_multi_stop();
        return TRUE;

      default:
        break;
      }

      // there is a bug in brew so we have to send evt_user to the dialogs directly
      // pass event to dialog handlers to see if it is theirs

      pDlgTableEntry = CDialerApp_GetDlgTableEntry(CDialerApp_GetActiveDlgID(pMe));

      if(pDlgTableEntry != NULL)
      {
        // find the appropriate event handler 
        if (pDlgTableEntry->DlgHandlerFunc != NULL) 
        {
          if(pDlgTableEntry->DlgHandlerFunc((void *)pDlgTableEntry, eCode, wParam, dwParam) == TRUE) 
          {
            // event handled by dialog event handler 
            return TRUE;
          }
        }
      }

      DIALER_MSG("EVT_USER: %d Not Handled", 0, 0, 0);
      return FALSE;
    }
    
#ifdef FEATURE_APP_TEST_AUTOMATION
    case EVT_DIALER_DIAG_CALL_STATUS:
      DIALER_MSG("EVT_DIALER_DIAG_CALL_STATUS: %d %d", wParam, dwParam, 0);
      CDialerApp_GetStatusData(pMe, eCode, wParam, dwParam);
      return TRUE;

    case EVT_DIALER_DIAG_CALL_DETAIL:
      DIALER_MSG("EVT_DIALER_DIAG_CALL_DETAIL: %d %d", wParam, dwParam, 0);
      CDialerApp_GetDetailData(pMe, eCode, wParam, dwParam);
      return TRUE;
#endif

#ifdef FEATURE_IMS
#ifdef FEATURE_IMS_VIDEOSHARE
    case EVT_QP_IMS_REGISTRATION:
      // received an event from the SIP registration manager
      DIALER_MSG("EVT_QP_IMS_REGISTRATION", 0, 0, 0);
        
      
      pMe->m_bSIPRegStatus = TRUE;   
                 
      if(pMe->m_bSuspended == FALSE) {          
        /* check to see if there are any voice calls connected,
           if so, add the VS SK to the status dialog
        */
        if(AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM) == TRUE) {
          return CDialerApp_DisplayStatusDlg(pMe);
        }
      }
      return TRUE;

    case EVT_QP_IMS_DEREGISTRATION:
      // received an event from the SIP registration manager
      DIALER_MSG("EVT_QP_IMS_DEREGISTRATION", 0, 0, 0);
        
      pMe->m_bSIPRegStatus = FALSE;   
                 
      if(pMe->m_bSuspended == FALSE) {          
        /* check to see if there are any voice calls connected,
           if so, update the status dialog
        */
        if(AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM) == TRUE) {
          return CDialerApp_DisplayStatusDlg(pMe);
        }
      }
      return TRUE;
#endif // FEATURE_IMS_VIDEOSHARE
#endif // FEATURE_IMS
  default:
    return FALSE;
  } 
}

/*=============================================================================
FUNCTION: CDialerApp_FreeAppData

DESCRIPTION:  Free applet resources

PARAMETERS:
  *pMe: CDialerApp object pointer 

RETURN VALUE:
  None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CDialerApp_FreeAppData(CDialerApp *pMe)
{
  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return;
  }

  // free allocated strings
  FREEIF(pMe->m_pszDialString);
  FREEIF(pMe->m_pszTimePauseSent);
  FREEIF(pMe->m_pszTimePauseRemaining);
  FREEIF(pMe->m_pszCallWaitingNum);
  FREEIF(pMe->m_pszDisplayString);

  // release the dialog stack
  if(pMe->m_pDialogStack != NULL)
  {
    if(CheckAEEReturnStatus(IVector_Release(pMe->m_pDialogStack)) == FALSE)
    {
      DIALER_ERR("IVector_Release FAILED", 0, 0, 0);  
    }

    pMe->m_pDialogStack = NULL;
  }

  // release the notif message queue
  if(pMe->m_pNotifMsgQueue != NULL)
  {
    if(CheckAEEReturnStatus(IVector_Release(pMe->m_pNotifMsgQueue)) == FALSE)
    {
      DIALER_ERR("IVector_Release FAILED", 0, 0, 0);  
    }

    pMe->m_pNotifMsgQueue = NULL;
  }

  // release ICM object
  if(pMe->m_pICM != NULL)
  {
    if(CheckAEEReturnStatus(ICM_Release(pMe->m_pICM)) == FALSE)
    {
      DIALER_ERR("ICM_Release FAILED", 0, 0, 0);  
    }

    pMe->m_pICM = NULL;
  }

  // unregister for ICM events
  (void) ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_DIALER, AEECLSID_CM_NOTIFIER, NMASK_CM_VOICE_CALL);

  // release IADDRBOOK object
  if(pMe->m_pIADDRBOOK != NULL)
  {
    if(CheckAEEReturnStatus(IADDRBOOK_Release(pMe->m_pIADDRBOOK)) == FALSE)
    {
      DIALER_ERR("IADDRBOOK_Release FAILED", 0, 0, 0);  
    }

    pMe->m_pIADDRBOOK = NULL;
  }

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  // release ISUPPSVC object
  if(pMe->m_pISUPPSVC != NULL)
  {
    if(CheckAEEReturnStatus(ISUPPSVC_Release(pMe->m_pISUPPSVC)) == FALSE)
    {
      DIALER_ERR("ISUPPSVC_Release FAILED", 0, 0, 0);  
    }

    pMe->m_pISUPPSVC = NULL;
  }

  // unregister for ISuppSvc events
  (void) ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_DIALER, AEECLSID_SUPPSVC_NOTIFIER, 0);

#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

#ifdef FEATURE_APP_TEST_AUTOMATION
  if(pMe->m_pDiag != NULL)
  {
    // Unregister with Diags and release the object
    IDIAG_UnregisterApp(pMe->m_pDiag, AEECLSID_DIALER);
    if(CheckAEEReturnStatus(IDIAG_Release(pMe->m_pDiag)) == FALSE)
    {
      DIALER_ERR("IDiag_Release FAILED", 0, 0, 0);  
    }
    pMe->m_pDiag = NULL;
  }
#endif
  if(pMe->m_pIALERT != NULL)
  {
     //Free IALERT
     if(CheckAEEReturnStatus(IALERT_Release(pMe->m_pIALERT)) == FALSE)
     {
        DIALER_ERR("IALERT_Release FAILED", 0, 0, 0);
     }
     pMe->m_pIALERT = NULL;
  }

#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
  if(pMe->m_pIDisplay2)
  {
     if(CheckAEEReturnStatus(IDISPLAY_Release(pMe->m_pIDisplay2)) == FALSE)
     {
        DIALER_ERR("IDISPLAY_Release FAILED", 0, 0, 0);
     }
     pMe->m_pIDisplay2 = NULL;
  }
#endif
}

/*=============================================================================
FUNCTION: CDialerApp_OnAppStart

DESCRIPTION:  Applet startup processing

PARAMETERS:
  *pMe: CDialerApp object pointer 
  wParam: word parameter
  dwParam: double word parameter

RETURN VALUE:
  boolean: returns TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_OnAppStart(CDialerApp *pMe, uint16 wParam, uint32 dwParam)
{
  //lint -save -esym(715, wParam)
  //lint -save -esym(715, dwParam)
  IDialog *pIDialog = NULL;
  AEECMCallID CallIDs[DIALERAPP_MAX_NUM_CALLS];
  uint16 wNumCallIDs = 0;
  int i;
  

  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  pIDialog = CDialerApp_GetActiveDlg(pMe);

  // check to see if we are starting after close, suspend, or just brew doing its thing
  if(pMe->m_bOrigVoiceCall == TRUE)
  {
    //special case for orginating voice calls
    pMe->m_bOrigVoiceCall = FALSE;

    // an event will create the next dialog
    if(CDialerApp_OriginateVoiceCall(pMe, pMe->m_pszDialString) == FALSE)
    {
      // do not want to start the app if we cannot originate
      DIALER_ERR("Unable to Originate Voice Call - Stopping Dialer", 0, 0, 0);
      (void) CDialerApp_StopApp(pMe);
      return TRUE;
    }
  }
  else if(pMe->m_wStartupDlgID != 0)
  { // create the dialog on startup to be shown
  
    // app was suspended at some arbitrary point so pop dialogs
    // we want a clean stack of dialogs on startup
    if((pIDialog != NULL))
    {
      // perform dialog stack cleanup
      // close all of the dialogs
      (void) CDialerApp_EndAllDlgs(pMe);
    }  

    // start the startup dialog
    if(CDialerApp_CreateDlg(pMe, pMe->m_wStartupDlgID, pMe->m_pStartupDlgInfo, pMe->m_dwStartupDlgInfoSize) == FALSE)
    {
      DIALER_ERR("Unable to create startup Dialog %d", pMe->m_wStartupDlgID, 0, 0);
      (void) CDialerApp_StopApp(pMe);
      
      //Need to return TRUE here because this return value is passed as the
      //return value of CDialerApp_EventHandler meaning the event was handled.
      return TRUE;
    }

    // reset the startup dialog info
    pMe->m_wStartupDlgID = 0;
    FREEIF(pMe->m_pStartupDlgInfo);
    pMe->m_pStartupDlgInfo = NULL;
    pMe->m_dwStartupDlgInfoSize = 0;

    if(pMe->m_bStartAlerting)
    {
      AEEALERTType AlertType;
  
      (void) IALERT_GetInfo(pMe->m_pIALERT, NULL, NULL, 0, &AlertType);

      wNumCallIDs = ICM_GetActiveCallIDs(pMe->m_pICM, (AEECMCallType)(AEECM_CALL_TYPE_VOICE|
                                                               AEECM_CALL_TYPE_EMERGENCY|
                                                               AEECM_CALL_TYPE_NON_STD_OTASP|
                                                               AEECM_CALL_TYPE_STD_OTASP), 
                                               (AEECMCallState)AEECM_CALL_STATE_INCOM, CallIDs, 
                                               DIALERAPP_MAX_NUM_CALLS*sizeof(AEECMCallID));


      if(wNumCallIDs > 0)
      {
        for(i=0; i<wNumCallIDs; i++)
        {
          AEECMCallID byCurrentCallID = CallIDs[i];

          if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, byCurrentCallID, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == TRUE)
          {
          // start the ringer
           if(AlertType == (AEEALERTType)AEEALERT_ALERT_NONE)
           {
             if(AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM) == TRUE)
             {
               // play call waiting ring
               (void) IALERT_StartAlerting(pMe->m_pIALERT, AEEALERT_CALLTYPE_VOICE, 
                      pMe->m_CallInfo.other_party_no, 
                    (AEEALERTType)AEEALERT_ALERT_CALLWAITING);
             }
             else
             {
                // play normal ring
               (void) IALERT_StartAlerting(pMe->m_pIALERT, AEEALERT_CALLTYPE_VOICE, 
                      pMe->m_CallInfo.other_party_no, 
                      AEEALERT_ALERT_NORMAL);
             }
           }
          }
        }
        if(CDialerApp_NVCheck_IsAutoAnswer() == TRUE)
        {
             (void) ISHELL_SetTimer(pMe->a.m_pIShell, DIALERAPP_AUTOANSWER_TIME, 
                   (PFNNOTIFY) CDialerApp_AutoAnswer, (void*) pMe);
        }
      }
      pMe->m_bStartAlerting = FALSE;
    }

  }
  else if(pIDialog != NULL)
  {
    // suspended by another app and then resumed
    // so refresh the current dialog
    (void) CDialerApp_RefreshDlg(pMe);

     wNumCallIDs = ICM_GetActiveCallIDs(pMe->m_pICM, (AEECMCallType)(AEECM_CALL_TYPE_VOICE|
                                                               AEECM_CALL_TYPE_EMERGENCY|
                                                               AEECM_CALL_TYPE_NON_STD_OTASP|
                                                               AEECM_CALL_TYPE_STD_OTASP), 
                                               (AEECMCallState)AEECM_CALL_STATE_CONV, CallIDs, 
                                               DIALERAPP_MAX_NUM_CALLS*sizeof(AEECMCallID));

    if(wNumCallIDs > 0)
    {
      for(i=0; i<wNumCallIDs; i++)
      {
        AEECMCallID byCurrentCallID = CallIDs[i];

        if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, byCurrentCallID, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == TRUE)
        {
          // start the ringer
           if(AEECM_IS_CALLTYPE_VOICE(pMe->m_CallInfo.call_type) == TRUE)
           {
             CDialerApp_SetVocoder(pMe,TRUE);
           }
        }
      }
    }
  }
  else 
  {
    // no start up dialog or current dialog exists so
    // must use function Dialer funtion to start app
    // this keeps dialer from getting displayed after end key press
    // set timer to close the dialer app
    DIALER_MSG("Dialer will close in 250 ms unless a dialog is created", 0,0,0);
    (void) ISHELL_SetTimer(pMe->a.m_pIShell, 250, (PFNNOTIFY) CDialerApp_StopApp, (void*) pMe);
    // we have to return true here or brew will create a new instance of dialer
    return TRUE;
  }

  // restart the status refresh timer
  (void) ISHELL_SetTimer(pMe->a.m_pIShell, 100, 
      (PFNNOTIFY) CDialerApp_StatusDlg_TimeRefresh, (void*) pMe);

  // init NV settings
  pMe->m_bNVAutoHyphen = CDialerApp_NVCheck_IsAutoHyphen();
  pMe->m_bNVOneTouch = CDialerApp_NVCheck_IsOneTouch();
  pMe->m_byNVDTMF = CDialerApp_NV_GetDTMF();

  return TRUE;
}


/*=============================================================================
FUNCTION: CDialerApp_OnAppSuspend

DESCRIPTION:  Applet suspend cleanup

PARAMETERS:
  *pMe: CDialerApp object pointer 
  wParam: word parameter
  dwParam: double word parameter

RETURN VALUE:
  boolean: returns TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_OnAppSuspend(CDialerApp *pMe, uint16 wParam, uint32 dwParam)
{
  //lint -save -esym(715, wParam)
  //lint -save -esym(715, dwParam)

  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  // reset the startup dialog info
  if(!pMe->m_bAppBeingSuspended)
    pMe->m_wStartupDlgID = 0;

  FREEIF(pMe->m_pStartupDlgInfo);
  pMe->m_pStartupDlgInfo = NULL;
  pMe->m_dwStartupDlgInfoSize = 0;

  // reset the orig flag
  pMe->m_bOrigVoiceCall = FALSE;

  // get rid off stored key press
  pMe->m_LastKeyPressed = 0;

  // get rid of text control to clear
  pMe->m_wTextCtlIDToClear = 0;

  // stop the status refresh timer
  (void) ISHELL_CancelTimer(pMe->a.m_pIShell, 
        (PFNNOTIFY) CDialerApp_StatusDlg_TimeRefresh, (void*) pMe);

  pMe->m_bSuspended = TRUE;
  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_OnAppStop

DESCRIPTION:  Applet stop cleanup

PARAMETERS:
  *pMe: CDialerApp object pointer 
  wParam: word parameter
  dwParam: double word parameter

RETURN VALUE:
  boolean: returns TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_OnAppStop(CDialerApp *pMe, uint16 wParam, uint32 dwParam)
{
  int i;

  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  // call suspend cleanup function
  if(CDialerApp_OnAppSuspend(pMe, wParam, dwParam) == FALSE)
  {
    DIALER_ERR("CDialerApp_OnAppSuspend FAILED", 0, 0, 0);
    return FALSE;
  }

  (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_StopApp, (void*) pMe);
  
  // delete contents in common strings
  (void) MEMSET(pMe->m_pszDialString, 0, (sizeof(AECHAR)*DIALERAPP_NUMBER_BUFFER_SIZE));
  (void) MEMSET(pMe->m_pszTimePauseSent, 0, (sizeof(AECHAR)*(DIALERAPP_NUMBER_BUFFER_SIZE*2)));
  (void) MEMSET(pMe->m_pszTimePauseRemaining, 0, (sizeof(AECHAR)*DIALERAPP_NUMBER_BUFFER_SIZE));

  // reset startup flag
  pMe->m_bIconStartedApp = FALSE;

  // init one touch flag
  pMe->m_bOneTouchFromCore = FALSE;

  // init Numedit dlg flag
  pMe->m_wNextNumEditDlg = 0;

  // clear ended calls
  for(i=0; i<DIALERAPP_MAX_NUM_CALLS; i++)
  {
    FREEIF(pMe->m_EndedCalls[i]);
    pMe->m_EndedCalls[i] = NULL;
  }

  // stop ringer
  (void) IALERT_StopAlerting(pMe->m_pIALERT);

  // close all of the dialogs
  (void) CDialerApp_EndAllDlgs(pMe); 
 
  pMe->m_bSuspended         = TRUE;
  pMe->m_bAppBeingSuspended = FALSE;

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_StartApp

DESCRIPTION:  Starts the Dialer App

PARAMETERS:
  pIShell: instance of IShell

RETURN VALUE: 
  boolean: Returns TRUE if successfull

COMMENTS:

SIDE EFFECTS: Dialer App is started

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StartApp(IShell *pIShell)
{
  PRINT_FUNCTION_NAME();
  if(pIShell == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  return ISHELL_PostEventEx(pIShell, EVTFLG_ASYNC, 
    AEECLSID_DIALER, EVT_USER, EVT_DIALER_STARTAPP_ICON, 0);  
}


/*=============================================================================
FUNCTION: CDialerApp_StartApp_Digit

DESCRIPTION:  Starts the Dialer App and adds the digit passed to the dial string

PARAMETERS:
  pIShell: instance of IShell
  CDigit: digit that was pressed

RETURN VALUE: 
  boolean: Returns TRUE if successfull

COMMENTS:

SIDE EFFECTS: Dialer App is started

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StartApp_Digit(IShell *pIShell, AECHAR cDigit)
{
  PRINT_FUNCTION_NAME();
  if(pIShell == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  return ISHELL_PostEventEx(pIShell, EVTFLG_ASYNC, 
    AEECLSID_DIALER, EVT_USER, EVT_DIALER_STARTAPP_DIGIT, (uint32)cDigit);
}

/*=============================================================================
FUNCTION: CDialerApp_StartApp_OriginateVoiceCall

DESCRIPTION:  Starts the Dialer App and originates voice call for passed in string

PARAMETERS:
  pIShell: instance of IShell
  pszDialString: string of number to dial

RETURN VALUE: 
  boolean: Returns TRUE if successfull

COMMENTS:
  The memory for pszBuf will be freed by the Dialer Application and 
  must not be freed before.

SIDE EFFECTS: Dialer App is started and voice call is originated

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StartApp_OriginateVoiceCall(IShell *pIShell, const char *pszDialString)
{
  AECHAR pszOrigDialString[DIALERAPP_NUMBER_BUFFER_SIZE];
  PRINT_FUNCTION_NAME();
  if(pIShell == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  if(pszDialString == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

 (void) STRTOWSTR(pszDialString, pszOrigDialString, (int)(sizeof(AECHAR)*DIALERAPP_NUMBER_BUFFER_SIZE));

  return ISHELL_SendEvent(pIShell, AEECLSID_DIALER, EVT_USER, EVT_DIALER_STARTAPP_ORIGINATEVOICECALL, (uint32)pszOrigDialString);
}

/*=============================================================================
FUNCTION: CDialerApp_StartApp_DisplayMsg

DESCRIPTION:  Starts the Dialer App and displays a string on the notif screen

PARAMETERS:
  pIShell: instance of IShell
  pszBuf: string to display

RETURN VALUE: 
  boolean: Returns TRUE if successfull

COMMENTS:
  None

SIDE EFFECTS: Dialer App is started and notif screen comes up

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StartApp_DisplayMsg(IShell *pIShell, const AECHAR *pszBuf)
{
  AECHAR pszString[DIALERAPP_NUMBER_BUFFER_SIZE];
  PRINT_FUNCTION_NAME();
  if(pIShell == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  if(pszBuf == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  (void) WSTRLCPY(pszString,pszBuf,DIALERAPP_NUMBER_BUFFER_SIZE);

  return ISHELL_SendEvent(pIShell, AEECLSID_DIALER, EVT_USER, EVT_DIALER_STARTAPP_DISPLAY_STRING, (uint32)pszString);
}

/*=============================================================================
FUNCTION: CDialerApp_AnswerVoiceCall

DESCRIPTION:  Answers the current incomming voice call if one exists.

PARAMETERS:
  pIShell: instance of IShell
  bAnswer: TRUE to answer call and FALSE to reject

RETURN VALUE: 
  boolean: Returns TRUE if successfull

COMMENTS:

SIDE EFFECTS: Dialer App will call ICM to answer the voice call

SEE ALSO:

=============================================================================*/
boolean CDialerApp_AnswerVoiceCall(IShell *pIShell, boolean bAnswer)
{
  ICM *pICM;
  AEECMCallID CallIDs[DIALERAPP_MAX_NUM_CALLS];
  uint16 wNumCallIDs = 0;
  int i;
  boolean bRetVal = TRUE;

  PRINT_FUNCTION_NAME();
  if(pIShell == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  // Create the ICallManager object.
  if(ISHELL_CreateInstance(pIShell,AEECLSID_CM, 
                        (void **) &pICM) != SUCCESS)
  {
    DIALER_ERR("ICM Create Failed", 0, 0, 0);
    return FALSE;
  }
  if(pICM == NULL) 
  {
    DIALER_ERR("Null pointer", 0, 0, 0);
    return FALSE;
  }

  wNumCallIDs = ICM_GetActiveCallIDs(pICM, (AEECMCallType)(AEECM_CALL_TYPE_VOICE|
                                                           AEECM_CALL_TYPE_EMERGENCY|
                                                           AEECM_CALL_TYPE_NON_STD_OTASP|
                                                           AEECM_CALL_TYPE_STD_OTASP), 
    (AEECMCallState)AEECM_CALL_STATE_INCOM, CallIDs, DIALERAPP_MAX_NUM_CALLS*sizeof(AEECMCallID));

  if(wNumCallIDs > 0)
  {
    if(bAnswer == TRUE)
    {
      for(i=0; i<wNumCallIDs; i++)
      {
        // want to answer incomming voice call
        if(CheckAEEReturnStatus(ICM_AnswerCall(pICM, CallIDs[i])) == FALSE)
        {
          DIALER_MSG("ANSWER VOICE CALL FAILED - CALLID: %d", CallIDs[i], 0, 0);
          bRetVal = FALSE;
        }
        else
        {
          DIALER_MSG("ANSWER VOICE CALL WAS SUCCESSFUL - CALLID: %d", CallIDs[i], 0, 0);
        }
      }
    }
    else
    {
      for(i=0; i<wNumCallIDs; i++)
      {
        // want to reject incomming voice call
        if(CheckAEEReturnStatus(ICM_EndCall(pICM, CallIDs[i])) == FALSE)
        {
          DIALER_MSG("REJECT VOICE CALL FAILED - CALLID: %d", CallIDs[i], 0, 0);
          bRetVal = FALSE;
        }
        else
        {
          DIALER_MSG("REJECT VOICE CALL WAS SUCCESSFUL - CALLID: %d", CallIDs[i], 0, 0);
        }
      }
    }  
  }

  (void) ICM_Release(pICM);

  return bRetVal;
}

/*=============================================================================
FUNCTION: CDialerApp_EndVoiceCall

DESCRIPTION:  Ends all voice calls.

PARAMETERS:
  pIShell: instance of IShell

RETURN VALUE: 
  boolean: Returns TRUE if successfull

COMMENTS:

SIDE EFFECTS: Dialer App will call ICM to end any voice calls

SEE ALSO:

=============================================================================*/
boolean CDialerApp_EndVoiceCall(IShell *pIShell)
{
  ICM *pICM;
  AEECMCallID CallIDs[DIALERAPP_MAX_NUM_CALLS];
  uint16 wNumCallIDs = 0;
  int i;
  boolean bRetVal = TRUE;

  PRINT_FUNCTION_NAME();
  if(pIShell == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  // Create the ICallManager object.
  if(ISHELL_CreateInstance(pIShell,AEECLSID_CM, 
                        (void **) &pICM) != SUCCESS)
  {
    DIALER_ERR("ICM Create Failed", 0, 0, 0);
    return FALSE;
  }
  if(pICM == NULL) 
  {
    DIALER_ERR("Null pointer", 0, 0, 0);
    return FALSE;
  }

  wNumCallIDs = ICM_GetActiveCallIDs(pICM, (AEECMCallType)(AEECM_CALL_TYPE_VOICE|
                                                           AEECM_CALL_TYPE_EMERGENCY|
                                                           AEECM_CALL_TYPE_NON_STD_OTASP|
                                                           AEECM_CALL_TYPE_STD_OTASP), 
    (AEECMCallState)AEECM_CALL_STATE_NONE, CallIDs, DIALERAPP_MAX_NUM_CALLS*sizeof(AEECMCallID));

  if(wNumCallIDs > 0)
  {
    for(i=0; i<wNumCallIDs; i++)
    {
      if(CheckAEEReturnStatus(ICM_EndCall(pICM, CallIDs[i])) == FALSE)
      {
        DIALER_MSG("END VOICE CALL FAILED - CALLID: %d", CallIDs[i], 0, 0);
        bRetVal = FALSE;
      }
      else
      {
        DIALER_MSG("END VOICE CALL WAS SUCCESSFUL - CALLID: %d", CallIDs[i], 0, 0);
      }
    }
  }

  (void) ICM_Release(pICM);

  return bRetVal;
}

/*=============================================================================
FUNCTION: CDialerApp_RefreshDlg

DESCRIPTION: Refresh the active dialog's display to reflect updated info

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS: This function is located in DialerApp.c rather than DialerUtils.c 
          because of the location of the DlgHandlers array.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_RefreshDlg(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  IDialog *pIActiveDialog;
  const CDlgHandler *pDlgTableEntry = NULL;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  if(pMe->m_bSuspended == TRUE)
  {	
    DIALER_MSG("Cannot refresh dialog because suspended", 0,0,0);
    return TRUE;
  }

  pIActiveDialog = CDialerApp_GetActiveDlg(pMe);

  if(pIActiveDialog == NULL) 
  { 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  pDlgTableEntry = CDialerApp_GetDlgTableEntry(CDialerApp_GetActiveDlgID(pMe));

  if(pDlgTableEntry == NULL) 
  {
    DIALER_ERR("Unable to get dialog table entry", 0, 0, 0);
    return FALSE;
  }

  if (pDlgTableEntry->DlgRefreshFunc != NULL) 
  {
    return pDlgTableEntry->DlgRefreshFunc(pMe);
  }
  else
  {
    // dialog does not have refresh funtion
    IDIALOG_Redraw(pIActiveDialog);
    return TRUE;
  }
}

/*=============================================================================
FUNCTION: CDialerApp_GetDlgTableEntry

DESCRIPTION: Retrieve the table entry corresponding to a given dialog

PARAMETERS:
  wDlgId: Dialog ID of the desired table entry

RETURN VALUE:
  (CDlgHandler*): returns pointer to table entry, or NULL if error.

COMMENTS: This function is located in DialerApp.c rather than DialerUtils.c 
          because of the location of the DlgHandlers array.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static CDlgHandler* CDialerApp_GetDlgTableEntry(uint16 wDlgID)
{
  int16 table_offset;

  table_offset = (int16)wDlgID - DIALERAPP_DIALOG_OFFSET;

  if((table_offset >= ARR_SIZE(DlgHandlers)) || (table_offset <= 0))
  {
    // error checking
    DIALER_ERR("Dlg ID %d out of Range", wDlgID, 0, 0);
    return NULL;
  }

  return &DlgHandlers[table_offset];
}

//lint -restore 
