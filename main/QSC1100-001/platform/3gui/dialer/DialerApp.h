#ifndef DIALERAPP_H
#define DIALERAPP_H

/*=============================================================================

FILE: DialerApp.h

SERVICES: Dialer Applet

GENERAL DESCRIPTION: Main applet functions

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2002 - 2007 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerApp.h#80 $
$DateTime: 2007/08/24 13:42:18 $
$Author: carls $
$Change: 526599 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/22/07   cvs     Add framework for dialogs
08/10/07   cvs     Add number of call ID's to ICM
08/02/07   cvs     Featurize all secondary display support
02/12/07   mjv     Featurized parts of Dialer to work with HDK
06/27/06   cvs     Update max # simultaneous calls
05/31/05   RI      Added support for IMS VideoShare.
12/05/05   sun     Increased Display Buffer size for Alpha of Request Struct
06/22/05   AT      Setting the size of the input to 40 characters (plus NULL)
06/07/05   sun     Added support for CCBS
05/31/05   RI      Modified the USSD data storage.
05/19/05   SUN     Fixed SS Operation and SS Code Type
05/11/05   AT      Adding in the flag for the waiting for origination state.
03/11/05   cvs     Optimize Status screen
12/10/04   SUN     Increased the number of call Ids
10/08/04   SUN     Added function to Display Data from other Apps
009/28/04   JAS     Support for missed call notif on secondary display
9/27/04   SUN     Added Support for CPHS
09/22/04   SUN     Fixed Mute and Vocoder Settings
07/26/04   JAS     Added support for secondary display
07/08/04   PA      The call ended after emergency flash should not be displayed
06/24/04   SUN     Vocoder should not be turned on multiple times
04/14/04   SUN     Start Alerting only after power up
04/09/04   SUN     Fixed Lint Errors
04/05/04   BPW     Alert changes for failed calls
03/19/04   sun     Added changes for DTMF
02/27/04   BPW     Fixed runtime memory error
02/23/04   SUN     Use ICM's max string for MAX_DIAL_DIGITS
02/03/04   JAS     Added ptr to IDiag object to CDialerApp structure
                   Added ptr for Dialog data to CDialerApp structure
                   Added struct defs for Status and Detail data storage
01/30/04   BPW     Featurized SS request structure 
01/22/04   BPW     Optimized NV gets
01/21/04   BPW     Added ISuppSvc API changes
01/20/04   BPW     DTMF and other memory cleanup and lint errors
01/20/04   BPW     Added API for End Voice Call
01/07/04   BPW     Changed how dialogs startup after being suspended
12/19/03   BPW     USSD & MissedCall now uses own dialg info structure
12/18/03   BPW     Added more SS feedback for requesting and incomplete
12/18/03   BPW     Added base station signalling support
12/10/03   BPW     Moved tab controller out of the defines
12/05/03   BPW     Added variable for Numedit dialog
12/02/03   BPW     Code review changes, BREW 3.0 support, & minor changes
11/26/03   SUN     Added Support for IALERT
11/25/03   BPW     Modified AnswerVoiceCall to reject the incoming call as well
11/17/03   BPW     Increased Max dialed digits to 40 and made rect dynamic
11/14/03   BPW     Turn off ringer after call waiting sound is complete
11/13/03   BPW     Press and hold clear to clear contents of text control
11/13/03   BPW     Made auto answer timer based
10/15/03   RAM     Added NW initiated USS Indication String
10/06/03   SUN     Added new Event to Display USSD Info
09/04/03   SUN     Changes related to AEESUPPSVC
08/29/03   BW      Made Dialer App a background app
08/22/03   BW      Added TimePauseDlg and functionality
08/19/03   BW      Added messages, param checking, and more code clean up
08/14/03   BW      Integration with both ICM and ISuppSvc
07/25/03   BW      Added bid include and added incoming call event
06/19/03   BW      Created Dialer Header File

=============================================================================*/
/*=============================================================================

                       INCLUDES

=============================================================================*/
#include "AEE.h"
#include "AEEAppGen.h"
#include "AEECM.h"
#include "DialerApp.bid"
#include "AEECM.h"
#include "snd.h"    // TODO:remove later
#include "uixsnd.h" // TODO:remove later
#include "AEEVector.h"
#include "AEEAddrBook.h"
#include "AppComFunc.h"
#include "AEEAlert.h"

#ifdef FEATURE_APP_TEST_AUTOMATION
#include "AEEDiag.h"
#endif

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#include "AEESuppSvc.h"
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

/*=============================================================================

                       CONSTANTS

=============================================================================*/
#define DIALERAPP_APP_VERSION "1.0"
#define DIALERAPP_MAX_ENDED_DISPLAY_TIME      10000  // milliseconds to flash ended call
#define DIALERAPP_MAX_NUM_CALLS               AEECM_MAX_NUM_CALLS     // num of simultaneous calls
#define DIALERAPP_ONETOUCH_SPEEDDIAL_TIMEOUT  2500   // millisecond timeout for onetouch
#define DIALERAPP_AUTOANSWER_TIME             7500   // millisecond timer for autoanswer
#define DIALERAPP_CLEAR_TEXTCTL_TIMEOUT       1250   // milliseconds to hold clear for textctl
#define DIALERAPP_MAX_NOTIF_MSGS              32     // maximum messages for notif vector
#define DIALERAPP_MAX_DISPLAY_CALL_FAIL       30     // maximum seconds to display notif & play sound
#define DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY   2      // default minimum seconds to display a notification
#define DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY   10     // default maximum seconds to display a notification
#define DIALERAPP_SEC_STATUS_TIMEOUT          1000   // frequency of update to 2ndary display (in msec)

// string buffer sizes
#define DIALERAPP_MAX_STR_SIZE                65     // max string buffer size
#define DIALERAPP_MAX_SMALL_STR_SIZE          30     // small display string sz
#define DIALERAPP_MAX_NOTIF_BUFFER_SIZE       255    // buffer size for notif strings
#define DIALERAPP_NUMBER_BUFFER_SIZE          41     // Max digits that can be saved (PBM) + 1.


/*=============================================================================

                       DIALER APP USER EVENTS

=============================================================================*/
// Starting App Events
#define EVT_DIALER_STARTAPP_DIGIT               (EVT_USER + 1) // start app with numedit
#define EVT_DIALER_STARTAPP_ORIGINATEVOICECALL  (EVT_USER + 2) // originate voice call
#define EVT_DIALER_STARTAPP_ICON                (EVT_USER + 3) // start app from icon  
#define EVT_DIALER_STARTAPP_DISPLAY_STRING      (EVT_USER + 4) // display string on notif dialog
  
// Sound events
#define EVT_DIALER_MULTI_COMPLETE               (EVT_USER + 11) // burst dtmf is completed

/*
 * Event values for diagnostic events
 */
#ifdef FEATURE_APP_TEST_AUTOMATION
#define EVT_DIALER_START_DIAGS           (EVT_USER + 0x100)
// Get Call Status Diagnostic
#define EVT_DIALER_DIAG_CALL_STATUS      (EVT_DIALER_START_DIAGS + 1)
// Get Call Detail Diagnostic
#define EVT_DIALER_DIAG_CALL_DETAIL      (EVT_DIALER_START_DIAGS + 2)
#endif

/* NOTE: There are additional events defined in DialerVSEvents.h. These 
         are for IMS VideoShare. These new events are added in a separate
         file for ease of integrating IMS applications with minimal impact to
         Dialer.
         
         Any new events must be added to DialerApp.h
*/
/*===========================================================================

                       TYPE DECLARATIONs

===========================================================================*/
// call status data structure for a single call
typedef struct
{
    AEECMCallState   CallState;
    AEECMCallType    CallType;
    AEECMCallID      CallID;
    AECHAR           Privacy;
    char             MinSecTime[DIALERAPP_MAX_STR_SIZE];
    char             OtherParty[DIALERAPP_MAX_STR_SIZE];
} CCallStatus;

/* Call status structure for all calls on the call status screen
 * We display up to DIALERAPP_MAX_NUM_CALLS active calls and as
 * many ended calls.
 */
typedef struct
{
    uint8        yNumDisplayedCalls;
    CCallStatus  CallStatus[DIALERAPP_MAX_NUM_CALLS * 2];
} CAllCallStatus;

// call detail structure for call displayed on call detail screen
typedef struct{
    uint8             CallID;   // displayed ID, not internal ID
    char              Privacy[DIALERAPP_MAX_STR_SIZE];
    char              MinSecTime[DIALERAPP_MAX_STR_SIZE];
    char              OtherParty1[DIALERAPP_MAX_STR_SIZE];
    char              OtherParty2[DIALERAPP_MAX_STR_SIZE];
    char              CUGString[DIALERAPP_MAX_STR_SIZE];
    char              CallStatus[DIALERAPP_MAX_STR_SIZE];
} CCallDetail;

// event handler function definition
typedef boolean (*PFNDLGHANDLER) (void *pUser,
                                  AEEEvent evt,
                                  uint16 w,
                                  uint32 dw );

// refresh dialog function definition
typedef boolean (*PFNDLGREFRESH) (void *pUser);

// notif dialog's item for message queue
typedef struct _CMsgQueueItem
{
  AECHAR *pszStr;
  uint32 dwExpireTime;
  uint32 dwMaxDisplaySecs;
} CMsgQueueItem;

// struct to keep ended call info
typedef struct _CEndedCall
{
  AEECMCallInfo CallInfo;
  uint32        dwDisplayStart;
} CEndedCall;

// struct to keep in the dialog stack
typedef struct _CDlgItem
{
  uint16 wDlgID;
  void*  pDlgInfo;
  uint32 dwInfoSize;
} CDlgItem;

typedef struct _CMissedCallDlgInfo
{
  AEECMCallInfo MissedCallInfo;   
} CMissedCallDlgInfo;

typedef struct _CRecallDlgInfo
{
  uint32 callID;   
} CRecallDlgInfo;

/* CDialer_DispState is an enum of values that determine what will
 * be displayed on the secondary display.
 */
typedef enum
{
  DIALER_DISP_STATE_NONE = 0,                 // Nothing to display
  DIALER_DISP_STATE_STATUS,                   // General call status
  DIALER_DISP_STATE_INCOM,                    // Incoming call
  DIALER_DISP_STATE_ORIG,                     // Mobile originated call
  DIALER_DISP_STATE_MISSEDCALL                // Missed call notification
} CDialer_DispState;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
typedef struct _CSSRequestDlgInfo
{
  byte byReqID; 
  AEESuppSvcSSOperationType ssOper; 
  AEESuppSvcSSCodeType ssCode;
  AECHAR szAlpha[DIALERAPP_MAX_NOTIF_BUFFER_SIZE];
} CSSRequestDlgInfo;

typedef struct _CUSSDDlgInfo
{
  AECHAR szNWInitiatedData[AEESUPPSVC_MAX_USS_LENGTH + 1];
  AEESuppSvcSSResponse SSResponse;
  AEESuppSvcReleaseSSOp ssOper;
} CUSSDDlgInfo;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)


typedef struct _CDialerApp 
{
  // app specific members
  AEEApplet          a;                    // applet data
  AEERect            m_rc;                 // applet rectangle
  boolean            m_bSuspended;         // applet is suspended
  boolean            m_bOrigVoiceCall;     // applet is started and originating a voice call
  ICM               *m_pICM;               // instance of ICM
  IAddrBook         *m_pIADDRBOOK;         // instance of IADDRBOOK
  IALERT*            m_pIALERT;            // instance of IALERT
  IVector           *m_pDialogStack;       // dialer's own dialog stack
  IVector           *m_pNotifMsgQueue;     // notif's message queue

#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
  // Members for the secondary display
  IDisplay          *m_pIDisplay2;         // secondary display
  IStatic           *m_pSecText;           // secondary display text object
  uint32             m_dwStartTS;          // start timestamp of call session
  uint32             m_dwEndTS;            // end timestamp of call session
  CDialer_DispState  m_DispState;          // secondary display state
  boolean            m_bIsActiveCalls;     // whether we have active calls
  // End of secondary display members
#endif

#ifdef FEATURE_APP_TEST_AUTOMATION
  IDiag             *m_pDiag;              // ptr to IDiag object
  void              *m_pDlgData;           // ptr to dialog contents
  CCallStatus       *m_pCallStatus;        // call status ptr used in
                                           //    populating diag response msg
#endif
  AECHAR             m_LastKeyPressed;     // stores the last key pressed
  AEECMCallInfo      m_CallInfo;           // local copy of callinfo struct (too big for stack)
  uint16             m_wNumMissedCalls;    // stores the number of missed calls
  AECHAR             m_CurrDTMF;           // flag that stores dtmf started state
  boolean            m_bMuted;             // if phone is muted, user option. It is TRUE if there is no call
  boolean            m_bOneTouchFromCore;  // flag to identify one touch from core
  boolean            m_bIconStartedApp;    // used to determine if no calls active be displayed
  boolean            m_bSpeedDialing;      // flag used to determine if a speed dial is occuring
  boolean            m_bEmergencyFlashed;  // flag used to determine if a voice call was converted to emergency
  CEndedCall        *m_EndedCalls[DIALERAPP_MAX_NUM_CALLS]; // stores the info for ended calls
  boolean            m_bLastCallsToDisplay;//Whether there were calls to display last time through StatusRefresh
  boolean            m_bLastActiveCalls[DIALERAPP_MAX_NUM_CALLS]; //what the active calls were last time through Status refresh
  AECHAR*            m_szCallNames[DIALERAPP_MAX_NUM_CALLS]; //The names to display for calls
  boolean            m_bOKToRefreshStatusDlg; //TRUE if DiaplayInit was called for status
  uint16             m_wStartupDlgID;      // dialog to display on startup
  void              *m_pStartupDlgInfo;    // info for startup dialog
  uint32             m_dwStartupDlgInfoSize; // size of info for startup dialog
  boolean            m_bNVAutoHyphen;
  uint8              m_byNVDTMF;
  boolean            m_bNVOneTouch;
  AEEALERTType       m_CallFailedAlert;
  boolean            m_bStartAlerting;


  // ISuppSvc specific members
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  ISUPPSVC                  *m_pISUPPSVC;           // pointer to ISuppSvc interface   
  AEESuppSvcSSStart         *m_pSSStart;  
  boolean                    m_bWizardSKDisplayed;  // flag for numedit dialog
  AEESuppSvcActiveService    m_ActiveData;          // CPHS Info 
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

  // dialog specific members
  AECHAR            *m_pszDialString;       // dialed string
  AECHAR            *m_pszDisplayString;       // dialed string
  AECHAR            *m_pszTimePauseSent;    // sent dtmf string
  AECHAR            *m_pszTimePauseRemaining; // remaing dtmf string
  AECHAR            *m_pszCallWaitingNum;   // call waiting phone number
  uint8              m_byCurrIncomCall;     // current incoming call id
  uint8              m_byCurrRecallID;      // current recall ID
  uint8              m_byCurrOrigCall;      // current origination call id
  uint8              m_byCurrCallOptions;   // current call id for call options dlg
  uint8              m_byCurrDetailCall;    // current call id for detail dlg
  uint8              m_byCurrTimePauseCall; // current call id for timepause dlg
  uint8              m_byCurrNumEditCall;   // current call id for numedit dlg
  uint16             m_wTextCtlIDToClear;   // text control id to be cleared
  uint32             m_dwSpaceLength;       // font measurement of a space char
  uint32             m_dwTimeLength;        // font measurement of time string
  uint32             m_dwNotifMinTime;      // notif min display time milliseconds
  uint32             m_dwNotifStartTime;    // notif start time milliseconds
  AEERect            m_DialedDigitsRect;    // dialed digits rectangle 
  uint16             m_wNextNumEditDlg;     // Numedit dlg flag
  DlgTabCtl         *m_pTabController;      // tab controller object

  boolean            m_bVocoder;            // vocoder on or off, the vocoder 
                                            // is on when there is a call up,
                                            // regardless of whether it is muted or not
  boolean            m_bWaitForStatusResult;
  boolean            m_bAppBeingSuspended;
  boolean            m_bWaitingforOrigVoiceCall;   // applet is started and originating a voice call
  
#ifdef FEATURE_IMS
#ifdef FEATURE_IMS_VIDEOSHARE
  boolean            m_bSIPRegStatus;    // IMS registration status
#endif // FEATURE_IMS_VIDEOSHARE
#endif // FEATURE_IMS
} CDialerApp;


typedef boolean (*CGenericDlgHandlerHelper)(CDialerApp *pMe);
typedef boolean (*CGenericDlgHandlerKeyHelper)(CDialerApp *pMe, AEEEvent evt, uint16 key);
typedef boolean (*CGenericDlgHandlerCmdHelper)(CDialerApp *pMe, uint16 ctrl_id);

// event handler sanity check definition
typedef struct _CDlgHandler
{
  uint16                       wDlgID;
  PFNDLGHANDLER                DlgHandlerFunc;
  PFNDLGREFRESH                DlgRefreshFunc;
  CGenericDlgHandlerHelper     DlgInitFunc;
  CGenericDlgHandlerKeyHelper  DlgKeyFunc;
  CGenericDlgHandlerCmdHelper  DlgCmdFunc;
  CGenericDlgHandlerHelper     DlgEndFunc;
  CDialerApp                  *pDialerApp;
} CDlgHandler;



/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: CDialerApp_GetModInfo

DESCRIPTION:  Called by BREW to retrieve information about this module

PARAMETERS:
  *ps: 
  **ppClasses: 
  **pApps: 
  *pnApps: 
  *pwMinPriv: 

RETURN VALUE:
  PFNMODENTRY: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
PFNMODENTRY     CDialerApp_GetModInfo(IShell *ps, AEECLSID **ppClasses, 
                                      AEEAppInfo **pApps, uint16 *pnApps, 
                                      uint16 *pwMinPriv);

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
#ifndef FEATURE_UIONE_HDK
boolean CDialerApp_StartApp(IShell *pIShell);
#else
static __inline boolean CDialerApp_StartApp(IShell *pIShell)
{
   // Not supported in HDK
   return FALSE;
}
#endif /* FEATURE_UIONE_HDK */


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
#ifndef FEATURE_UIONE_HDK
boolean CDialerApp_StartApp_Digit(IShell *pIShell, AECHAR cDigit);
#else
static __inline boolean CDialerApp_StartApp_Digit(IShell *pIShell, AECHAR cDigit)
{
   // Not supported in HDK
   return FALSE;
}
#endif /* FEATURE_UIONE_HDK */


/*=============================================================================
FUNCTION: CDialerApp_StartApp_OriginateVoiceCall

DESCRIPTION:  Starts the Dialer App and originates voice call for passed in string

PARAMETERS:
  pIShell: instance of IShell
  pszDialString: string of number to dial

RETURN VALUE: 
  boolean: Returns TRUE if successfull

COMMENTS:
  The memory for pszDialString will be freed by the Dialer Application and 
  must not be freed before.

SIDE EFFECTS: Dialer App is started and voice call is originated

SEE ALSO:

=============================================================================*/
#ifndef FEATURE_UIONE_HDK
boolean CDialerApp_StartApp_OriginateVoiceCall(IShell *pIShell, const char *pszDialString);
#else
static __inline boolean CDialerApp_StartApp_OriginateVoiceCall(IShell *pIShell, const char *pszDialString)
{
   // Not supported in HDK
   return FALSE;
}
#endif /* FEATURE_UIONE_HDK */

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
#ifndef FEATURE_UIONE_HDK
boolean CDialerApp_AnswerVoiceCall(IShell *pIShell, boolean bAnswer);
#else
static __inline boolean CDialerApp_AnswerVoiceCall(IShell *pIShell, boolean bAnswer)
{
   // Not supported in HDK
   return FALSE;
}
#endif /* FEATURE_UIONE_HDK */

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
#ifndef FEATURE_UIONE_HDK
boolean CDialerApp_EndVoiceCall(IShell *pIShell);
#else
static __inline boolean CDialerApp_EndVoiceCall(IShell *pIShell)
{
   // Not supported in HDK
   return FALSE;
}
#endif /* FEATURE_UIONE_HDK */

/*=============================================================================
FUNCTION: CDialerApp_RefreshDlg

DESCRIPTION: Refresh the active dialog's display to reflect updated info

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_RefreshDlg(void *pUser);

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
boolean CDialerApp_OnAppSuspend(CDialerApp *pMe, uint16 wParam, uint32 dwParam);

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
boolean CDialerApp_OnAppStop(CDialerApp *pMe, uint16 wParam, uint32 dwParam);


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

SIDE EFFECTS: Dialer App is started and notif screen is displayed

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StartApp_DisplayMsg(IShell *pIShell, const AECHAR *pszBuf);

#endif // DIALERAPP_H
