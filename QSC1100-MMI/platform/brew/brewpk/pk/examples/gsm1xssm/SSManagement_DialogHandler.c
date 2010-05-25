/*=========================================================================
FILE: SSManagement_DialogHandler.c

SERVICES: GSM1x SS Management Dialog Handlers

GENERAL DESCRIPTION:
    SS Management dialog handlers for the UI (User interface).

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================== */
#include "AEEShell.h"               // AEE Shell Services
#include "AutomationLogging.h"

#include "SSManagement_App.h"
#include "SSManagement_DialogHandler.h"

#include "ssmanagement.bid"	        // Applet class ID
#include "gsm1xssm_res.h"	            // Applet Resources
#define  SSMANAGEMENT_RES_FILE GSM1XSSM_RES_FILE

#include "SSManagement_UserEvent.h"

#include "SSManagement_DialogMsgDecoding.h" // also includes logging definitions!!!

#include "AEEText.h"                // AEE Text Services
#include "AEEStdLib.h"              // AEE StdLib Services

#include "GSM1xPhone.h"

#include "SSMsg.h"
#include "SS_CommonMacros.h"

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/

// Time (in ms) before a MsgBox will close automatically
#define MSGBOX_TIMEOUT 3000

// Time (in ms) before the About dialog will close automatically
#define ABOUT_TIMEOUT 10000

// Max number of characters allowed in the status text string
#define STATUS_MAX_TEXT_LEN 30

// Max number of characters allowed in the format text string
#define FORMAT_MAX_TEXT_LEN 30

// Max number of characters allowed in the about text buffer
#define ABOUT_BUFFER_MAX_TEXT_LEN 30

/*===========================================================================
                               MACRO DEFINITIONS
===========================================================================*/

/*==================== GENERAL MACROS =====================*/

// Macro used to show a dialog
#define SHOW_DIALOG(dlgResId) { ShowDialog(pMe, dlgResId); }

// Macro used to setup a new dialog and close the old dialog
#define SETUP_NEW_DIALOG(setupHandler)                                      \
    {                                                                       \
        assert(setupHandler != NULL);                                       \
        pMe->m_pCurrentDialogSetupHandler = setupHandler;                   \
        (void) ISHELL_EndDialog(pMe->a.m_pIShell);                          \
    }                                                                       \

// Macro used to setup a msgbox's test using a resource string
#define SETUP_RESOURCE_STRING(resourceId)                                   \
    {                                                                       \
        (void) ISHELL_LoadResString(pMe->a.m_pIShell,                       \
                                    SSMANAGEMENT_RES_FILE,                  \
                                    resourceId,                             \
                                    pMe->m_cMsgBoxText,                     \
                                    sizeof(pMe->m_cMsgBoxText));            \
    }                                                                       \

// Macro used to setup a msgbox's title using a resource string 
#define SETUP_MSGBOX_TITLE(resourceId)                                      \
    {                                                                       \
        (void) ISHELL_LoadResString(pMe->a.m_pIShell,                       \
                                    SSMANAGEMENT_RES_FILE,                  \
                                    resourceId,                             \
                                    pMe->m_cMsgBoxTitle,                    \
                                    sizeof(pMe->m_cMsgBoxTitle));           \
    }                                                                       \

// Macro used to setup a msgbox's title to no title
#define SETUP_MSGBOX_NO_TITLE                                               \
    {                                                                       \
        pMe->m_cMsgBoxTitle[0] = '\0';                                      \
    }                                                                       \

// Macro for calculating the size in bytes of unused text left in
// the message box string
#define MSGBOX_APPENDED_TEXT_MAX_SIZE                                       \
    (                                                                       \
        sizeof(pMe->m_cMsgBoxText) -                                        \
        WSTRLEN(pMe->m_cMsgBoxText) * sizeof(AECHAR)                        \
    )                                                                       \

// Macro used to get pointer to end of message box string
#define MSGBOX_APPENDED_TEXT_PTR                                            \
    (                                                                       \
        pMe->m_cMsgBoxText + WSTRLEN(pMe->m_cMsgBoxText)                    \
    )                                                                       \
              
/*==================== SET DATA MEMBER MACROS =====================*/

// Macro used to set the current dialog event handler
#define SET_CURRENT_DIALOG_HANDLER(eventHandler)                            \
    {                                                                       \
        pMe->m_pCurrentDialogHandleEvent = eventHandler;                    \
    }                                                                       \

// Macro used to setup the next dialog setup handler
#define SET_NEXT_DIALOG_SETUP(setupHandler)                                 \
    {                                                                       \
        pMe->m_pNextDialogSetupHandler = setupHandler;                      \
    }                                                                       \

// Macro used to set the service-category menu choice value
#define SET_SERVICE_CATEGORY_CHOICE(choice) { pMe->m_ServiceCategory = choice; }

// Macro used to set the specific-service menu choice value
#define SET_SPECIFIC_SERVICE_CHOICE(choice) { pMe->m_ServiceSpecific = choice; }

// Macro used to set the operations menu choice value
#define SET_SELECTED_OPERATION(choice) { pMe->m_cSelectedOperation = choice; }

// Macro used to set the GSM SS code value
#define SET_SS_CODE(code) { pMe->m_cSScode = code; }

// Macro used to store the current setup handler as the 
// previous choose dialog's setup handler
#define SET_CHOOSE_OPERATION_PREV_DIALOG_SETUP_TO_CURRENT                   \
    {                                                                       \
        pMe->m_pChooseOperationPrevDialogSetupHandler =						\
			pMe->m_pCurrentDialogSetupHandler;								\
    }                                                                       \

// Macro used to setup the current setup handler as the 
// previous password dialog's setup handler
#define SET_PASSWORD_PREV_DIALOG_SETUP_TO_CURRENT			                \
    {                                                                       \
        pMe->m_pPasswordPrevDialogSetupHandler =			                \
    			pMe->m_pCurrentDialogSetupHandler;							\
    }                                                                       \

// Macro used to store the current setup handler as the next setup handler
#define SET_NEXT_DIALOG_SETUP_TO_CURRENT                                    \
    {                                                                       \
        pMe->m_pNextDialogSetupHandler = pMe->m_pCurrentDialogSetupHandler; \
    }                                                                       \

// Macro used to store the service code string
#define SET_SERVICE_CODE_STRING(str)                                        \
    {                                                                       \
        assert( STRLEN(str) < sizeof(pMe->m_cServiceCode) );                \
        STRLCPY(pMe->m_cServiceCode, str, sizeof(pMe->m_cServiceCode));     \
    }                                                                       \

/*==================== GET DATA MEMBER MACROS =====================*/

// Macro used to retrieve the next dialog setup handler
#define GET_NEXT_DIALOG_SETUP (pMe->m_pNextDialogSetupHandler)

// Macro used to retrieve the Choose Operation dialog's previous dialog setup handler
#define GET_CHOOSE_OPERATION_PREV_DIALOG_SETUP							\
	(pMe->m_pChooseOperationPrevDialogSetupHandler)

// Macro used to retrieve the Password dialog's previous dialog setup handler
#define GET_PASSWORD_PREV_DIALOG_SETUP (pMe->m_pPasswordPrevDialogSetupHandler)

// Macro used to retrieve the service-category menu choice value
#define GET_SERVICE_CATEGORY_CHOICE (pMe->m_ServiceCategory)

// Macro used to retrieve the specific-service menu choice value
#define GET_SPECIFIC_SERVICE_CHOICE (pMe->m_ServiceSpecific)


/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*** Forward declarations ***/
static boolean RouteDialogEvent(
    CGSM1xSSManagementApp *pMe,
    AEEEvent              eCode,
    uint16                wParam,
    uint32                dwParam
    );

static void ShowDialog(
    CGSM1xSSManagementApp *pMe,
    uint16                dlgResId
    );

static boolean IsPhoneStateValidForManagementApp(CGSM1xSSManagementApp* pMe);

static void DialogTimeoutCallback(void *pUser);
static void HandleManagementRequest(CGSM1xSSManagementApp *pMe);
static void PostManagementRequestEvent(CGSM1xSSManagementApp *pMe);
static void HandleShowOperationResult(CGSM1xSSManagementApp *pMe);
static void HandleShowResultMsgErrorCode(CGSM1xSSManagementApp *pMe);
static void HandleShowResultMsgProblemCode(CGSM1xSSManagementApp *pMe);
static void BuildPasswordDataIntoSIA(CGSM1xSSManagementApp *pMe);
static void SetupAndShowAboutDialog(CGSM1xSSManagementApp *pMe);

/*** Dialog setup methods ***/
static void SetupMainMenu(CGSM1xSSManagementApp *pMe);
static void SetupCallDivertingMenu(CGSM1xSSManagementApp* pMe);
static void SetupCallBarringMenu(CGSM1xSSManagementApp* pMe);
static void SetupLineIdentificationMenu(CGSM1xSSManagementApp* pMe);

static void SetupChooseOperationMenu(CGSM1xSSManagementApp* pMe);
static void SetupTimedMsgBox(CGSM1xSSManagementApp *pMe);
static void SetupEnterPasswordMenu(CGSM1xSSManagementApp* pMe);
static void SetupChangePasswordMenu(CGSM1xSSManagementApp* pMe);
static void SetupEnterForwardToNumberMenu(CGSM1xSSManagementApp* pMe);
static void SetupShowOperationResult(CGSM1xSSManagementApp* pMe);

// not really a setup method, this method just terminates the applet. 
static void SetupExitApplet(CGSM1xSSManagementApp* pMe);

/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

/*=============================================================================
FUNCTION: DialogHandler_HandleEvent

DESCRIPTION: handle a BREW event by the dialog handler

PARAMETERS:
   pMe [in]: Pointer to the Applet structure.           
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean DialogHandler_HandleEvent(
   struct _CGSM1xSSManagementApp *pMe,
   AEEEvent                      eCode,
   uint16                        wParam,
   uint32                        dwParam
)
{
  	switch (eCode) 
	{
    case EVT_DIALOG_INIT:
        pMe->m_cCurrentDialogID = wParam;
        pMe->m_pCurrentIDialog  = (IDialog*)dwParam;
        return RouteDialogEvent(pMe, eCode, wParam, dwParam);

    case EVT_DIALOG_START:
        return RouteDialogEvent(pMe, eCode, wParam, dwParam);

    case EVT_DIALOG_END:
        if (0 == wParam) return TRUE;

        assert(wParam == pMe->m_cCurrentDialogID);
        (void) RouteDialogEvent(pMe, eCode, wParam, dwParam);

        pMe->m_cCurrentDialogID = 0;
        pMe->m_pCurrentIDialog = NULL;

        // Note that pMe->m_pCurrentDialogHandleEvent must not be set to NULL!
        // This is because the current dialog event handle is needed in order
        // to process events from the state machine!
        //pMe->m_pCurrentDialogHandleEvent = NULL;
        
        // Don't show dialog if we are suspending (EVT_SUSPEND is
        // sent before EVT_DIALOG_END).  Wait until we resume...
        if ((FALSE == pMe->m_bSuspended) && (TRUE == pMe->m_AppStarted))
        {
            pMe->m_pCurrentDialogSetupHandler(pMe); // setup current dialog
        }
        return TRUE;

	case EVT_APP_START:
        // call show dialog handler
        assert(NULL != pMe->m_pCurrentDialogSetupHandler);
        pMe->m_pCurrentDialogSetupHandler(pMe);
		return TRUE;

	case EVT_APP_STOP:
		// active dialog will be closed 'auto-magically
		return TRUE;

	case EVT_APP_SUSPEND:
		return TRUE;

	case EVT_APP_RESUME:
        // call show dialog handler
        assert(NULL != pMe->m_pCurrentDialogSetupHandler);
        pMe->m_pCurrentDialogSetupHandler(pMe);
		return TRUE;
	}

    // Route the received event to the current active dialog handler.
    return RouteDialogEvent(pMe, eCode, wParam, dwParam);
}

/*=============================================================================
FUNCTION: DialogHandler_Initialize

DESCRIPTION: Initialize UI part.

PARAMETERS:
   pMe [in]: Pointer to the Applet structure.           

RETURN VALUE:
   None

=============================================================================*/
void DialogHandler_Initialize( struct _CGSM1xSSManagementApp *pMe )
{
    assert( sizeof(pMe->m_cSupplementaryInformationA) > PASSWORD_LENGTH);

    pMe->m_pCurrentDialogHandleEvent = NULL;
    pMe->m_pNextDialogSetupHandler = NULL;
    pMe->m_pChooseOperationPrevDialogSetupHandler = NULL;
	pMe->m_pPasswordPrevDialogSetupHandler = NULL;

    pMe->m_cCurrentDialogID = 0;
    pMe->m_pCurrentIDialog = NULL;
    pMe->m_pMsgBox_IStatic = NULL;
    pMe->m_cMsgBoxText[0] = (AECHAR)'\0';

    pMe->m_ServiceCategory = 0;
    pMe->m_ServiceSpecific = 0;
    pMe->m_cSelectedOperation = 0;
    
    pMe->m_pCurrentDialogSetupHandler = SetupMainMenu;

    pMe->m_bDoEndCall =FALSE;
    pMe->m_bSIAValid = FALSE;
    pMe->m_bResultStatusValid = FALSE;
    pMe->m_bResultForwardedToNumberValid = FALSE;

    if (!IsPhoneStateValidForManagementApp(pMe))
    {
        SETUP_MSGBOX_NO_TITLE;
        SETUP_RESOURCE_STRING( IDS_INVALID_PHONE_STATE_FOR_APP );
        pMe->m_pCurrentDialogSetupHandler = SetupTimedMsgBox;
        pMe->m_pNextDialogSetupHandler = SetupExitApplet;
    }
}

/*=============================================================================
FUNCTION: DialogHandler_Destroy

DESCRIPTION: destroy UI part

PARAMETERS:
   pMe [in]: Pointer to the Applet structure.           

RETURN VALUE:
   None

=============================================================================*/
void DialogHandler_Destroy( struct _CGSM1xSSManagementApp *pMe )
{
    // clear app data (just for consistency)
    pMe->m_pCurrentDialogSetupHandler = NULL;
   	pMe->m_pCurrentDialogHandleEvent = NULL;

    if (pMe->m_pMsgBox_IStatic)
    {
        (void) ISTATIC_Release(pMe->m_pMsgBox_IStatic);
        pMe->m_pMsgBox_IStatic = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
//   Local Function Definitions
///////////////////////////////////////////////////////////////////////////////

/*=============================================================================
FUNCTION: RouteDialogEvent

DESCRIPTION: Routes a BREW event to the active dialog's event handler

PARAMETERS:
   pMe [in]: Pointer to the Applet structure.           
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean RouteDialogEvent(
    CGSM1xSSManagementApp *pMe,
    AEEEvent              eCode,
    uint16                wParam,
    uint32                dwParam
    )
{
   if (NULL == pMe->m_pCurrentDialogHandleEvent) 
   {
      return FALSE;
   }

   return pMe->m_pCurrentDialogHandleEvent(pMe, eCode, wParam, dwParam);
}

/*=============================================================================
FUNCTION: IsPhoneStateValidForManagementApp

DESCRIPTION: checks the phone state and if the management app should run or not

PARAMETERS:
   pMe: CGSM1xSSManagementApp object pointer

RETURN VALUE:
   boolean: TRUE if the app should run, FALSE otherwise.

=============================================================================*/
static boolean IsPhoneStateValidForManagementApp(CGSM1xSSManagementApp* pMe)
{
	#ifdef FEATURE_MULTIMODE_ARCH
	    const cm_mm_call_info_s_type *ci = IPHONE_GetCallInfo(pMe->m_pIPhone);
	#else
		const cm_call_info_s_type* ci = IPHONE_GetCallInfo(pMe->m_pIPhone);
	#endif

    // no state information - probably idle state, assume management app should run
    if (ci == NULL) return TRUE;

    // if phone is in conversation don't run management applet
    if (ci->call_state == CM_CALL_STATE_CONV)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/*=============================================================================
FUNCTION: IsPasswordStringValid

DESCRIPTION: 
    checks if the password number string is valid.
    password must be exactly PASSWORD_LENGTH, and digits only.
   
PARAMETERS:
   passwordString: Null terminated string

RETURN VALUE:
    boolean: TRUE if the password string is valid, FALSE otherwise.

=============================================================================*/
static boolean IsPasswordStringValid( char* passwordString )
{
    uint8 index;

    assert(passwordString != NULL);

    if (STRLEN(passwordString) != PASSWORD_LENGTH) return FALSE;

    for (index=0; index < PASSWORD_LENGTH; ++index)
    {
        if ( (passwordString[index] < '0') || (passwordString[index] > '9') )
            return FALSE;
    }

    return TRUE;
}

/*=============================================================================
FUNCTION: IsForwardToStringValid

DESCRIPTION: 
    checks if the forward to number string is valid.
    number length can be up to FORWARD_TO_NUMBER_MAX_INPUT_LENGTH,
    and it can consist of digits and the characters '*', '#', '+'.
   
PARAMETERS:
   forwardingString: Null terminated string

RETURN VALUE:
    boolean: TRUE if the forwarding string is valid, FALSE otherwise.

=============================================================================*/
static boolean IsForwardToStringValid( char* forwardingString )
{
    uint8 index;
    uint8 maxLength;
    
    assert(forwardingString != NULL);

    maxLength = STRLEN(forwardingString);
    assert( maxLength <= FORWARD_TO_NUMBER_MAX_LENGTH);

    // check string characters:
    if (maxLength == 0) return FALSE;

    for (index=0; index < maxLength; ++index)
    {
        if ( (forwardingString[index] >= '0') && (forwardingString[index] <= '9') ) continue;

        // According to GSM 22.030 the characters 'a','b','c' should be supported,
        // this is out of scope of the trial and is not currently supported.
        switch (forwardingString[index])
        {
            case '*' :
            case '#' :
            case '+' :
            //case 'a' :
            //case 'b' :
            //case 'c' :
                continue;

            default:
                return FALSE;
        }
    }

    return TRUE;
}

/*=============================================================================
FUNCTION: DialogTimeoutCallback

DESCRIPTION:
   Callback function passed to the BREW function ISHELL_SetTimer. As a timer 
   set for a dialog expires, we post a timeout event to the state handler. 
   
PARAMETERS:
   pUserData: Assumed to be a pointer to the Applet Structure (CGSM1xSSManagementApp).
              It is dictated by BREW type PFNNOTIFY.

RETURN VALUE:
    None

=============================================================================*/
static void DialogTimeoutCallback(void *pUserData)
{
   CGSM1xSSManagementApp *pMe = (CGSM1xSSManagementApp*)pUserData;

   // Send event to the UI (active dialog)
   (void) ISHELL_SendEvent(
             pMe->a.m_pIShell, 
             AEECLSID_SS_MANAGEMENT,
             EVT_DIALOG_TIMEOUT,
             0,
             0
             );

   GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:EVT_DIALOG_TIMEOUT\n",0,0,0);
}

/*=============================================================================
FUNCTION: ShowDialog

DESCRIPTION: Displays a new dialog

PARAMETERS:
   pMe: CGSM1xSSManagementApp object pointer
   dlgResId: Dialog resource ID

RETURN VALUE:
    None

=============================================================================*/
static void ShowDialog(
    CGSM1xSSManagementApp *pMe,
    uint16                dlgResId
    )
{
   if (SUCCESS != ISHELL_CreateDialog(pMe->a.m_pIShell,
                                      SSMANAGEMENT_RES_FILE,
                                      dlgResId,
                                      NULL)) 
   {
      GSM1X_LOG_FATAL(pMe->m_pSSUtilCls, "SSM:Unable to open dialog %d!\n",dlgResId,0,0);
      (void) ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
   }
   else
   {
      GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:CreateDialog %d\n",dlgResId,0,0);
   }
}


///////////////////////////////////////////////////////////////////////////////
//   Dialog Function Definitions
///////////////////////////////////////////////////////////////////////////////

/*===========================================================================

FUNCTION SetupExitApplet()

DESCRIPTION
	This function closes the current applet.

PARAMETERS:
	pMe:	Pointer to the app object

===========================================================================*/
static void SetupExitApplet(CGSM1xSSManagementApp* pMe)
{
    (void) ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
}

/*=============================================================================
FUNCTION: TimedMsgBox_HandleEvent

DESCRIPTION:
   This dialog handler is to create a dialog with only a static control (no
   softkeys) in it.
   
   After the dialog is created, it starts a timer and when the timer expires
   it closes the dialog (in case if the dialog is not yet closed by the user)

PARAMETERS:
   pMe [in]: CGSM1xSSManagementApp object pointer.            
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

=============================================================================*/
static boolean TimedMsgBox_HandleEvent(
   CGSM1xSSManagementApp *pMe,
   AEEEvent              eCode,
   uint16                wParam,
   uint32                dwParam
)
{
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         {
            if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                                 AEECLSID_STATIC,
                                                 (void**)&pMe->m_pMsgBox_IStatic))
            {
                return FALSE;
            }

            ISTATIC_SetRect(pMe->m_pMsgBox_IStatic, &pMe->m_cMenuFrame);

            ISTATIC_SetProperties(pMe->m_pMsgBox_IStatic,
                                  ST_CENTERTEXT | ST_MIDDLETEXT);

            (void) ISTATIC_SetText(pMe->m_pMsgBox_IStatic,
                                   NULL, // currently do not use title!
                                   pMe->m_cMsgBoxText,
                                   AEE_FONT_NORMAL,
                                   AEE_FONT_NORMAL);

            CALLBACK_Init(&pMe->m_UITimerCB, DialogTimeoutCallback, pMe);

            (void) ISHELL_SetTimerEx(
                      pMe->a.m_pIShell,
                      MSGBOX_TIMEOUT,
                      &pMe->m_UITimerCB
                      );

            return TRUE;
         }

      case EVT_DIALOG_START:
         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_SS_MANAGEMENT,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_pMsgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         CALLBACK_Cancel(&pMe->m_UITimerCB); // cancel timer callback

         if (NULL != pMe->m_pMsgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_pMsgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_pMsgBox_IStatic);
            pMe->m_pMsgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_KEY:
		 if ((AVKType)wParam != AVK_CLR) 
			break;
		 // else fall through.
      case EVT_DIALOG_TIMEOUT: 
      {
		 if (pMe->m_bDoEndCall == TRUE) 
         {
			// clear call if no other voice calls exist.
            if (ISSUTILCLS_GetCallDependentCounter(pMe->m_pSSUtilCls) == 0)
            {
                IPHONE_EndCall(pMe->m_pIPhone);
            }

			// management operation has ended
			ISSUTILCLS_DecCallIndependentCounter(pMe->m_pSSUtilCls);
			pMe->m_bDoEndCall = FALSE;
         }
         SETUP_NEW_DIALOG( GET_NEXT_DIALOG_SETUP );
         return TRUE;
      }
   }

   return FALSE;
}

/*===========================================================================

FUNCTION SetupTimedMsgBox()

DESCRIPTION
	This function creates the TimedMsgBox dialog.

PARAMETERS:
	pMe:	Pointer to the app object

===========================================================================*/
static void SetupTimedMsgBox(CGSM1xSSManagementApp *pMe)
{
    assert(GET_NEXT_DIALOG_SETUP != NULL);

    SET_CURRENT_DIALOG_HANDLER( TimedMsgBox_HandleEvent );
    SHOW_DIALOG( IDD_MSG_BOX_DLG );
}

/*=============================================================================
FUNCTION: AboutDialog_HandleEvent

DESCRIPTION:
   This dialog handler is to create a dialog with only a static control (no
   softkeys) in it.
   
   After the dialog is created, it starts a timer and when the timer expires
   it closes the dialog (in case if the dialog is not yet closed by the user)

PARAMETERS:
   pMe [in]: CGSM1xSSManagementApp object pointer.            
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

=============================================================================*/
static boolean AboutDialog_HandleEvent(
   CGSM1xSSManagementApp *pMe,
   AEEEvent              eCode,
   uint16                wParam,
   uint32                dwParam
)
{
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         {
            if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                                 AEECLSID_STATIC,
                                                 (void**)&pMe->m_pMsgBox_IStatic))
            {
                return FALSE;
            }

            ISTATIC_SetRect(pMe->m_pMsgBox_IStatic, &pMe->m_cMenuFrame);

            ISTATIC_SetProperties(pMe->m_pMsgBox_IStatic, ST_MIDDLETEXT);

            (void) ISTATIC_SetText(pMe->m_pMsgBox_IStatic,
                                   pMe->m_cMsgBoxTitle,
                                   pMe->m_cMsgBoxText,
                                   AEE_FONT_NORMAL,
                                   AEE_FONT_NORMAL);

            CALLBACK_Init(&pMe->m_UITimerCB, DialogTimeoutCallback, pMe);

            (void) ISHELL_SetTimerEx(
                      pMe->a.m_pIShell,
                      ABOUT_TIMEOUT,
                      &pMe->m_UITimerCB
                      );

            return TRUE;
         }

      case EVT_DIALOG_START:
         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_SS_MANAGEMENT,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_pMsgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         CALLBACK_Cancel(&pMe->m_UITimerCB); // cancel timer callback

         if (NULL != pMe->m_pMsgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_pMsgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_pMsgBox_IStatic);
            pMe->m_pMsgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_KEY:
		 if ((AVKType)wParam != AVK_CLR) 
			break;
		 // else fall through.
      case EVT_DIALOG_TIMEOUT: 
      {
         SETUP_NEW_DIALOG( SetupMainMenu );
         return TRUE;
      }
   }

   return FALSE;
}

/*===========================================================================

FUNCTION SetupAboutDialog()

DESCRIPTION
	This function creates the About dialog.

PARAMETERS:
	pMe:	Pointer to the app object

===========================================================================*/
static void SetupAboutDialog(CGSM1xSSManagementApp *pMe)
{
    SET_CURRENT_DIALOG_HANDLER( AboutDialog_HandleEvent );
    SHOW_DIALOG( IDD_MSG_BOX_DLG );
}

/*===========================================================================

FUNCTION MainMenu_HandleEvent

DESCRIPTION
	This is the MainMenu dialog EventHandler.

PARAMETERS:
    pMe [in]: CGSM1xSSManagementApp object pointer.            
    eCode [in]: Specifies the Event sent to this applet
    wParam, dwParam [in]: Event specific data.

RETURN VALUE
    TRUE: If event was processed
    FALSE: If event was not processed

===========================================================================*/
static boolean MainMenu_HandleEvent(
    CGSM1xSSManagementApp *pMe,
    AEEEvent              eCode, 
    uint16                wParam,
    uint32                dwParam
    )
{
    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            SET_SERVICE_CODE_STRING( "" );
            SET_SS_CODE( 0xffff );
            return TRUE;
            
        case EVT_DIALOG_START:
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_COMMAND:
            switch (wParam) 
            {
                case ID_CALL_DIVERTING:
                    SET_SERVICE_CATEGORY_CHOICE( wParam );
                    // dialog setup should be the last action
                    SETUP_NEW_DIALOG( SetupCallDivertingMenu );                   
                    return TRUE;

                case ID_CALL_BARRING:
                    SET_SERVICE_CATEGORY_CHOICE( wParam );
                    // dialog setup should be the last action
                    SETUP_NEW_DIALOG( SetupCallBarringMenu );
                    return TRUE;

                case ID_CALL_WAITING:
                    SET_SERVICE_CODE_STRING( "43" );
                    SET_SS_CODE( SSMSG_SS_CODE_CW );
                    SET_CHOOSE_OPERATION_PREV_DIALOG_SETUP_TO_CURRENT;
                    SET_SERVICE_CATEGORY_CHOICE( wParam );
                    // dialog setup should be the last action
                    SETUP_NEW_DIALOG( SetupChooseOperationMenu );
                    return TRUE;

                case ID_LINE_IDENTIFICATION:
                    SET_SERVICE_CATEGORY_CHOICE( wParam );
                    // dialog setup should be the last action
                    SETUP_NEW_DIALOG( SetupLineIdentificationMenu );
                    return TRUE;

                case ID_ABOUT:
                    SetupAndShowAboutDialog(pMe);
                    return TRUE;
            }
            break;
    }

    return FALSE;
}

/*===========================================================================

FUNCTION SetupMainMenu()

DESCRIPTION
	This function creates the MainMenu dialog.

PARAMETERS:
	pMe:	Pointer to the app object

===========================================================================*/
static void SetupMainMenu(CGSM1xSSManagementApp *pMe)
{
    SET_CURRENT_DIALOG_HANDLER( MainMenu_HandleEvent );
    SHOW_DIALOG( IDD_MAIN_MENU_DLG );
}

/*===========================================================================

FUNCTION CallDiverting_HandleEvent

DESCRIPTION
	This is the CallDiverting dialog event handler.

PARAMETERS:
   pMe [in]: CGSM1xSSManagementApp object pointer.            
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE
  TRUE: If event was processed
  FALSE: If event was not processed

===========================================================================*/
static boolean CallDiverting_HandleEvent(
    CGSM1xSSManagementApp *pMe,
    AEEEvent              eCode, 
    uint16                wParam, 
    uint32                dwParam
    )
{
    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_COMMAND:
            switch (wParam) 
            {
                case ID_DIVERT_ALWAYS:

                    SET_SPECIFIC_SERVICE_CHOICE( wParam );
                    SET_SERVICE_CODE_STRING( "21" );
                    SET_SS_CODE( SSMSG_SS_CODE_CFU );
                    SET_CHOOSE_OPERATION_PREV_DIALOG_SETUP_TO_CURRENT;
                    SETUP_NEW_DIALOG( SetupChooseOperationMenu );
                    return TRUE;

                case ID_DIVERT_BUSY:

                    SET_SPECIFIC_SERVICE_CHOICE( wParam );
                    SET_SERVICE_CODE_STRING( "67" );
                    SET_SS_CODE( SSMSG_SS_CODE_CFB );
                    SET_CHOOSE_OPERATION_PREV_DIALOG_SETUP_TO_CURRENT;
                    SETUP_NEW_DIALOG( SetupChooseOperationMenu );
                    return TRUE;

                case ID_DIVERT_NO_REPLY:

                    SET_SPECIFIC_SERVICE_CHOICE( wParam );
                    SET_SERVICE_CODE_STRING( "61" );
                    SET_SS_CODE( SSMSG_SS_CODE_CFNRY );
                    SET_CHOOSE_OPERATION_PREV_DIALOG_SETUP_TO_CURRENT;
                    SETUP_NEW_DIALOG( SetupChooseOperationMenu );
                    return TRUE;

                case ID_DIVERT_UNREACHABLE:

                    SET_SPECIFIC_SERVICE_CHOICE( wParam );
                    SET_SERVICE_CODE_STRING( "62" );
                    SET_SS_CODE( SSMSG_SS_CODE_CFNRC );
                    SET_CHOOSE_OPERATION_PREV_DIALOG_SETUP_TO_CURRENT;
                    SETUP_NEW_DIALOG( SetupChooseOperationMenu );
                    return TRUE;

                case ID_DIVERT_CANCEL_ALL:
                                        
                    SET_SPECIFIC_SERVICE_CHOICE( wParam );
                    SET_SERVICE_CODE_STRING( "002" );
                    SET_SS_CODE( SSMSG_SS_CODE_ALLFORWARDINGSS );

                    // simulate an 'deactivate' selection from the operation menu
                    SET_SELECTED_OPERATION( MANAGEMENET_OPERATION_DEACTIVATE );

                    SET_NEXT_DIALOG_SETUP_TO_CURRENT;
                    HandleManagementRequest( pMe );
                    return TRUE;
            }
            break;

        case EVT_KEY:
            if ((AVKType)wParam == AVK_CLR)
            {
                SETUP_NEW_DIALOG( SetupMainMenu );
                return TRUE;
            }
    }
   
    return FALSE;
}

/*===========================================================================

FUNCTION SetupCallDivertingMenu()

DESCRIPTION

	This function creates the CallDiverting dialog.

PARAMETERS:
	pMe:	Pointer to the app object

===========================================================================*/
static void SetupCallDivertingMenu(CGSM1xSSManagementApp *pMe)
{
    SET_CURRENT_DIALOG_HANDLER( CallDiverting_HandleEvent );
	SHOW_DIALOG( IDD_CALL_DIVERTING_DLG );
}

/*===========================================================================

FUNCTION CallBarring_HandleEvent

DESCRIPTION
	This is the CallBarring dialog event handler.

PARAMETERS:
   pMe [in]: CGSM1xSSManagementApp object pointer.            
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE
  TRUE: If event was processed
  FALSE: If event was not processed

===========================================================================*/
static boolean CallBarring_HandleEvent(
    CGSM1xSSManagementApp *pMe,
    AEEEvent              eCode, 
    uint16                wParam, 
    uint32                dwParam
    )
{
    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            SET_SERVICE_CODE_STRING( "" );
            SET_SS_CODE( 0xffff );
            return TRUE;

        case EVT_DIALOG_START:
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_COMMAND:
            switch (wParam) 
            {
                case ID_CALL_BARRING_ALL_OUTGOING:

                    SET_SPECIFIC_SERVICE_CHOICE( wParam );
                    SET_SERVICE_CODE_STRING( "33" );
                    SET_SS_CODE( SSMSG_SS_CODE_BARRINGOFOUTGOINGCALLS );
                    SET_CHOOSE_OPERATION_PREV_DIALOG_SETUP_TO_CURRENT;
                    SETUP_NEW_DIALOG( SetupChooseOperationMenu );
                    return TRUE;

                case ID_CALL_BARRING_OUTGOING_INTERNATIONAL:

                    SET_SPECIFIC_SERVICE_CHOICE( wParam );
                    SET_SERVICE_CODE_STRING( "331" );
                    SET_SS_CODE( SSMSG_SS_CODE_BOIC );
                    SET_CHOOSE_OPERATION_PREV_DIALOG_SETUP_TO_CURRENT;
                    SETUP_NEW_DIALOG( SetupChooseOperationMenu );
                    return TRUE;

                case ID_CALL_BARRING_OUTGOING_INTERNATIONAL_EXCEPT_HOME:

                    SET_SPECIFIC_SERVICE_CHOICE( wParam );
                    SET_SERVICE_CODE_STRING( "332" );
                    SET_SS_CODE( SSMSG_SS_CODE_BOICEXHC );
                    SET_CHOOSE_OPERATION_PREV_DIALOG_SETUP_TO_CURRENT;
                    SETUP_NEW_DIALOG( SetupChooseOperationMenu );
                    return TRUE;

                case ID_CALL_BARRING_ALL_INCOMING:

                    SET_SPECIFIC_SERVICE_CHOICE( wParam );
                    SET_SERVICE_CODE_STRING( "35" );
                    SET_SS_CODE( SSMSG_SS_CODE_BAIC );
                    SET_CHOOSE_OPERATION_PREV_DIALOG_SETUP_TO_CURRENT;
                    SETUP_NEW_DIALOG( SetupChooseOperationMenu );                    
                    return TRUE;

                case ID_CALL_BARRING_INCOMING_WHILE_ABROAD:

                    SET_SPECIFIC_SERVICE_CHOICE( wParam );
                    SET_SERVICE_CODE_STRING( "351" );
                    SET_SS_CODE( SSMSG_SS_CODE_BICROAM );
                    SET_CHOOSE_OPERATION_PREV_DIALOG_SETUP_TO_CURRENT;
                    SETUP_NEW_DIALOG( SetupChooseOperationMenu );
                    return TRUE;

                case ID_CALL_BARRING_CANCEL_ALL:
                    
                    SET_SPECIFIC_SERVICE_CHOICE( wParam );
                    SET_SERVICE_CODE_STRING( "330" );
                    SET_SS_CODE( SSMSG_SS_CODE_ALLBARRINGSS );

                    // simulate an 'deactivate' selection from the operation menu
                    SET_SELECTED_OPERATION( MANAGEMENET_OPERATION_DEACTIVATE );

                    SET_PASSWORD_PREV_DIALOG_SETUP_TO_CURRENT;
                    SETUP_NEW_DIALOG( SetupEnterPasswordMenu );
                    return TRUE;

                case ID_CALL_BARRING_CHANGE_PASSWORD:
                    
                    SET_SPECIFIC_SERVICE_CHOICE( wParam );
                    SET_SERVICE_CODE_STRING( "330" );
                    // SET_SS_CODE(); // ss code not checked 
                                       
                    SET_SELECTED_OPERATION( MANAGEMENET_OPERATION_CHANGE_PASSWORD );
                    SET_NEXT_DIALOG_SETUP_TO_CURRENT;

                    // initialize change password to use the first screen
                    pMe->m_cChangePasswordScreen = CHANGE_PASSWORD_ENTER_OLD;
                    SETUP_NEW_DIALOG( SetupChangePasswordMenu );
                    return TRUE;
            }
            break;

        case EVT_KEY:
            if ((AVKType)wParam == AVK_CLR)
            {
                SETUP_NEW_DIALOG( SetupMainMenu );
                return TRUE;
            }
            break;
    }

    return FALSE;
}

/*===========================================================================

FUNCTION SetupCallBarringMenu()

DESCRIPTION
	This function creates the CallBarring dialog.

PARAMETERS:
	pMe:	Pointer to the app object

===========================================================================*/
static void SetupCallBarringMenu(CGSM1xSSManagementApp *pMe)
{
    SET_CURRENT_DIALOG_HANDLER( CallBarring_HandleEvent );
	SHOW_DIALOG( IDD_CALL_BARRING_DLG );
}

/*===========================================================================

FUNCTION LineIdentification_HandleEvent

DESCRIPTION
	This is the LineIdentification dialog event handler.

PARAMETERS:
   pMe [in]: CGSM1xSSManagementApp object pointer.            
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE
  TRUE: If event was processed
  FALSE: If event was not processed

===========================================================================*/
static boolean LineIdentification_HandleEvent(
    CGSM1xSSManagementApp *pMe,
    AEEEvent              eCode, 
    uint16                wParam, 
    uint32                dwParam
    )
{
    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            SET_SERVICE_CODE_STRING( "" );
            SET_SELECTED_OPERATION( 0xffff );
            SET_SS_CODE( 0xffff );
            return TRUE;

        case EVT_DIALOG_START:
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_COMMAND:
            switch (wParam) 
            {
                case ID_LINE_IDENTIFICATION_CLIP:

                    SET_SERVICE_CODE_STRING( "30" );
                    SET_SS_CODE( SSMSG_SS_CODE_CLIP );

                    // the actual operation is to check status, so we 
                    // simulate an 'interrogate' selection from the operation menu
                    SET_SELECTED_OPERATION( MANAGEMENET_OPERATION_INTERROGATE ); 

                    SET_NEXT_DIALOG_SETUP_TO_CURRENT;
                    HandleManagementRequest( pMe );
                    SET_SPECIFIC_SERVICE_CHOICE( wParam );
                    return TRUE;

                case ID_LINE_IDENTIFICATION_CLIR:

                    SET_SERVICE_CODE_STRING( "31" );
                    SET_SS_CODE( SSMSG_SS_CODE_CLIR );

                    // the actual operation is to check status, so we 
                    // simulate an 'interrogate' selection from the operation menu
                    SET_SELECTED_OPERATION( MANAGEMENET_OPERATION_INTERROGATE ); 

                    SET_NEXT_DIALOG_SETUP_TO_CURRENT;
                    HandleManagementRequest( pMe );
                    SET_SPECIFIC_SERVICE_CHOICE( wParam );
                    return TRUE;
            }
            break;

        case EVT_KEY:
            if ((AVKType)wParam == AVK_CLR)
            {
                SETUP_NEW_DIALOG( SetupMainMenu );
                return TRUE;
            }
            break;
    }

    return FALSE;
}

/*===========================================================================

FUNCTION SetupLineIdentificationMenu()

DESCRIPTION
	This function creates the LineIdentification dialog.

PARAMETERS:
	pMe:	Pointer to the app object

===========================================================================*/
static void SetupLineIdentificationMenu(CGSM1xSSManagementApp *pMe)
{
    SET_CURRENT_DIALOG_HANDLER( LineIdentification_HandleEvent );
	SHOW_DIALOG( IDD_LINE_IDENTIFICATION_DLG );
}

/*===========================================================================

FUNCTION ChooseOperation_HandleEvent

DESCRIPTION
	This is the ChooseOperation dialog event handler.
    notes:
    1. Operation REGISTER exists only for Call Diverting.
    2. Operation ERASE exists only for Call Diverting.
    3. Menus for specific services usually rely on this function to send
       the actual request, with HandleManagementRequest(). 
       Such menus are Call Diverting, Call Barring and Line Identification.
       Call Diverting and Call Barring also have a "Cancel All" entry, 
       that immediately executes, since it needs no further user input.

PARAMETERS:
   pMe [in]: CGSM1xSSManagementApp object pointer.            
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE
  TRUE: If event was processed
  FALSE: If event was not processed

===========================================================================*/
static boolean ChooseOperation_HandleEvent(
    CGSM1xSSManagementApp *pMe,
    AEEEvent              eCode, 
    uint16                wParam, 
    uint32                dwParam
    )
{
    IMenuCtl *pTextMenu = (IMenuCtl*)IDIALOG_GetControl(
                                           pMe->m_pCurrentIDialog,
                                           IDD_CHOOSE_OPERATION_DLG
                                           );
    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            SET_SELECTED_OPERATION( 0xffff );
            return TRUE;

        case EVT_DIALOG_START:
            (void) IMENUCTL_DeleteAll(pTextMenu);

            if (GET_SERVICE_CATEGORY_CHOICE == ID_CALL_DIVERTING)
            {
                (void) IMENUCTL_AddItem(pTextMenu,
                                        SSMANAGEMENT_RES_FILE,
                                        IDS_CHOOSE_OPERATION_REGISTER,
                                        ID_OPERATION_REGISTER,
                                        (AECHAR*)NULL,
                                        (uint32)NULL);
            }

            (void) IMENUCTL_AddItem(pTextMenu,
                                    SSMANAGEMENT_RES_FILE,
                                    IDS_CHOOSE_OPERATION_ACTIVATE,
                                    ID_OPERATION_ACTIVATE,
                                    (AECHAR*)NULL,
                                    (uint32)NULL);

            (void) IMENUCTL_AddItem(pTextMenu,
                                    SSMANAGEMENT_RES_FILE,
                                    IDS_CHOOSE_OPERATION_DEACTIVATE,
                                    ID_OPERATION_DEACTIVATE,
                                    (AECHAR*)NULL,
                                    (uint32)NULL);

            if (GET_SERVICE_CATEGORY_CHOICE == ID_CALL_DIVERTING)
            {
                (void) IMENUCTL_AddItem(pTextMenu,
                                        SSMANAGEMENT_RES_FILE,
                                        IDS_CHOOSE_OPERATION_ERASE,
                                        ID_OPERATION_ERASE,
                                        (AECHAR*)NULL,
                                        (uint32)NULL);
            }

            (void) IMENUCTL_AddItem(pTextMenu,
                                    SSMANAGEMENT_RES_FILE,
                                    IDS_CHOOSE_OPERATION_INTERROGATE,
                                    ID_OPERATION_INTERROGATE,
                                    (AECHAR*)NULL,
                                    (uint32)NULL);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_COMMAND:
            switch (wParam) 
            {
                case ID_OPERATION_REGISTER:
                    assert(GET_SERVICE_CATEGORY_CHOICE == ID_CALL_DIVERTING);

                    SET_SELECTED_OPERATION( MANAGEMENET_OPERATION_REGISTER );
                    SETUP_NEW_DIALOG( SetupEnterForwardToNumberMenu );
                    return TRUE;

                case ID_OPERATION_ACTIVATE:
                    SET_SELECTED_OPERATION( MANAGEMENET_OPERATION_ACTIVATE );

                    if (GET_SERVICE_CATEGORY_CHOICE == ID_CALL_BARRING)
                    {
					    SET_PASSWORD_PREV_DIALOG_SETUP_TO_CURRENT;
                        SETUP_NEW_DIALOG( SetupEnterPasswordMenu );
                    }
                    else
                    {
                        SET_NEXT_DIALOG_SETUP( SetupChooseOperationMenu );
                        HandleManagementRequest( pMe );
                    }
                    return TRUE;


                case ID_OPERATION_DEACTIVATE:

                    SET_SELECTED_OPERATION( MANAGEMENET_OPERATION_DEACTIVATE );

                    if (GET_SERVICE_CATEGORY_CHOICE == ID_CALL_BARRING)
                    {
						SET_PASSWORD_PREV_DIALOG_SETUP_TO_CURRENT;
                        SETUP_NEW_DIALOG( SetupEnterPasswordMenu );
                    }
                    else 
                    {
                        SET_NEXT_DIALOG_SETUP( SetupChooseOperationMenu );
                        HandleManagementRequest( pMe );
                    }
                    return TRUE;

                case ID_OPERATION_ERASE:

                    assert(GET_SERVICE_CATEGORY_CHOICE == ID_CALL_DIVERTING);

                    SET_SELECTED_OPERATION( MANAGEMENET_OPERATION_ERASE );
                    SET_NEXT_DIALOG_SETUP( SetupChooseOperationMenu );
                    HandleManagementRequest( pMe );
                    return TRUE;

                case ID_OPERATION_INTERROGATE:

                    SET_SELECTED_OPERATION( MANAGEMENET_OPERATION_INTERROGATE );
                    SET_NEXT_DIALOG_SETUP( SetupChooseOperationMenu );
                    HandleManagementRequest( pMe );
                    return TRUE;                    
            }
            break;

        case EVT_KEY:
            if ((AVKType)wParam == AVK_CLR)
            {
                SETUP_NEW_DIALOG( GET_CHOOSE_OPERATION_PREV_DIALOG_SETUP );
                return TRUE;
            }
            break;
    }

    return FALSE;
}

/*===========================================================================

FUNCTION SetupChooseOperationMenu()

DESCRIPTION
	This function creates the ChooseOperation dialog.

PARAMETERS:
	pMe:	Pointer to the app object

===========================================================================*/
static void SetupChooseOperationMenu(CGSM1xSSManagementApp *pMe)
{
    SET_CURRENT_DIALOG_HANDLER( ChooseOperation_HandleEvent );
	SHOW_DIALOG( IDD_CHOOSE_OPERATION_DLG );
}

/*===========================================================================

FUNCTION EnterPassword_HandleEvent

DESCRIPTION
	This is the EnterPassword dialog event handler.

PARAMETERS:
   pMe [in]: CGSM1xSSManagementApp object pointer.            
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE
  TRUE: If event was processed
  FALSE: If event was not processed

===========================================================================*/
static boolean EnterPassword_HandleEvent(
    CGSM1xSSManagementApp *pMe,
    AEEEvent              eCode, 
    uint16                wParam, 
    uint32                dwParam
    )
{
    ITextCtl* pTextCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_pCurrentIDialog,
                                                       IDC_ENTER_PASSWORD);
    assert(NULL != pTextCtl);

    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            ITEXTCTL_SetMaxSize(pTextCtl,
                                PASSWORD_LENGTH);

            ITEXTCTL_SetProperties(pTextCtl,
                                   TP_FRAME);

            // Set initial input mode
            (void) ITEXTCTL_SetInputMode(pTextCtl,
                                         AEE_TM_NUMBERS);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch ((AVKType)wParam) 
            {
                case AVK_SELECT:
                {
                    WSTRTOSTR(ITEXTCTL_GetTextPtr(pTextCtl), 
                                pMe->m_cSupplementaryInformationA, 
                                sizeof(pMe->m_cSupplementaryInformationA));

                    pMe->m_bSIAValid = 
                        IsPasswordStringValid(pMe->m_cSupplementaryInformationA);

                    if (pMe->m_bSIAValid)
                    {
                        SET_NEXT_DIALOG_SETUP( GET_PASSWORD_PREV_DIALOG_SETUP );
                        HandleManagementRequest( pMe );                        
                    }
                    else
                    {
                        SET_NEXT_DIALOG_SETUP_TO_CURRENT;
                        SETUP_MSGBOX_NO_TITLE;
                        SETUP_RESOURCE_STRING( IDS_INVALID_PASSWORD_STRING );
                        SETUP_NEW_DIALOG( SetupTimedMsgBox );
                    }
                    return TRUE;
                 }

                case AVK_CLR:
                {						
                    SETUP_NEW_DIALOG( GET_PASSWORD_PREV_DIALOG_SETUP );
                    return TRUE;
                }
            }
    }

    return FALSE;
}

/*===========================================================================

FUNCTION SetupEnterPasswordMenu()

DESCRIPTION
	This function creates the EnterPassword dialog.

PARAMETERS:
	pMe:	Pointer to the app object

===========================================================================*/
static void SetupEnterPasswordMenu(CGSM1xSSManagementApp *pMe)
{
    SET_CURRENT_DIALOG_HANDLER( EnterPassword_HandleEvent );
	SHOW_DIALOG( IDD_ENTER_PASSWORD_DLG );
}

/*===========================================================================

FUNCTION EnterForwardToNumberMenu_HandleEvent

DESCRIPTION
	This is the EnterForwardToNumberMenu dialog event handler.

PARAMETERS:
   pMe [in]: CGSM1xSSManagementApp object pointer.            
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.


RETURN VALUE
  TRUE: If event was processed
  FALSE: If event was not processed

===========================================================================*/
static boolean EnterForwardToNumberMenu_HandleEvent(
    CGSM1xSSManagementApp *pMe,
    AEEEvent              eCode, 
    uint16                wParam, 
    uint32                dwParam
    )
{
    ITextCtl* pTextCtl = 
        (ITextCtl*)IDIALOG_GetControl(pMe->m_pCurrentIDialog,
                                      IDC_ENTER_FORWARD_TO_NUMBER_TEXT);
    assert(NULL != pTextCtl);

    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            ITEXTCTL_SetMaxSize(pTextCtl,
                                FORWARD_TO_NUMBER_MAX_LENGTH);

            ITEXTCTL_SetProperties(pTextCtl,
                                   TP_FRAME);

            (void) ITEXTCTL_SetInputMode(pTextCtl,
                                         AEE_TM_NUMBERS);

            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch ((AVKType)wParam) 
            {
                case AVK_SELECT:
                {
                    WSTRTOSTR(ITEXTCTL_GetTextPtr(pTextCtl), 
                                pMe->m_cSupplementaryInformationA, 
                                sizeof(pMe->m_cSupplementaryInformationA));

                    pMe->m_bSIAValid = 
                        IsForwardToStringValid(pMe->m_cSupplementaryInformationA);

                    if (pMe->m_bSIAValid)
                    {
                        SET_NEXT_DIALOG_SETUP( SetupChooseOperationMenu );
                        HandleManagementRequest( pMe );
                    }
                    else
                    {
                        SET_NEXT_DIALOG_SETUP_TO_CURRENT;
                        SETUP_MSGBOX_NO_TITLE;
                        SETUP_RESOURCE_STRING( IDS_INVALID_FORWARD_TO_STRING );
                        SETUP_NEW_DIALOG( SetupTimedMsgBox );
                    }
                    return TRUE;
                }

                case AVK_CLR:
                {
                    SETUP_NEW_DIALOG( SetupChooseOperationMenu );
                    return TRUE;
                }
            }
    }

    return FALSE;
}

/*===========================================================================

FUNCTION SetupEnterForwardToNumberMenu()

DESCRIPTION
	This function creates the EnterForwardToNumberMenu dialog.

PARAMETERS:
	pMe:	Pointer to the app object

===========================================================================*/
static void SetupEnterForwardToNumberMenu(CGSM1xSSManagementApp *pMe)
{
    SET_CURRENT_DIALOG_HANDLER( EnterForwardToNumberMenu_HandleEvent );
	SHOW_DIALOG( IDD_ENTER_FORWARD_TO_NUMBER_DLG );
}

/*=============================================================================
FUNCTION: SendManagementRequest_HandleEvent

DESCRIPTION:
   This dialog handler is to create a dialog with only a static control (no
   softkeys) in it.
   
   After the dialog is created, it waits for a SUCCESS or FAILURE events
   and sets up the next dialog, according to the event received.

PARAMETERS:
   pMe [in]: CGSM1xSSManagementApp object pointer.      
   eCode [in]: Specifies the Event sent to this applet.  
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SendManagementRequest_HandleEvent(
   CGSM1xSSManagementApp *pMe,
   AEEEvent              eCode,
   uint16                wParam,
   uint32                dwParam
)
{
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         {
            if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                                 AEECLSID_STATIC,
                                                 (void**)&pMe->m_pMsgBox_IStatic))
            {
                return FALSE;
            }

            ISTATIC_SetRect(pMe->m_pMsgBox_IStatic, &pMe->m_cMenuFrame);

            ISTATIC_SetProperties(pMe->m_pMsgBox_IStatic,
                                  ST_CENTERTEXT | ST_MIDDLETEXT);

            (void) ISTATIC_SetText(pMe->m_pMsgBox_IStatic,
                                   NULL,
                                   pMe->m_cMsgBoxText,
                                   AEE_FONT_NORMAL,
                                   AEE_FONT_NORMAL);
            return TRUE;
         }

      case EVT_DIALOG_START:
         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_SS_MANAGEMENT,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_pMsgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         if (NULL != pMe->m_pMsgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_pMsgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_pMsgBox_IStatic);
            pMe->m_pMsgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_SS_OPER_END:
		
          // For SS_OPER_RES_NO_RESPONSE and SS_OPER_RES_SUCCESS,
		  // there might exist a management call. So we set a reminder
		  // to do IPHONE_EndCall() if there is a call.
		  if ((wParam == SS_OPER_RES_NO_RESPONSE) ||
		   	  (wParam == SS_OPER_RES_SUCCESS)) 
          {
		      pMe->m_bDoEndCall = TRUE;
          }

          switch (wParam)
          {
             case SS_OPER_RES_SUCCESS: /* success */
                {
                    SSmsgDecodedResultStatus status;

                    status = DialogMsgDecoding_HandleResultMsg(
                                pMe, 
                                (SSMsg_int_msg_type*) dwParam
                                );
                    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:Decode Status:%d\n",status,0,0);

                    FREE((SSMsg_int_msg_type*) dwParam);

                    switch (status)
                    {
                        case SS_DECODING_MSG_RESULT_SUCCESS:                         
                            HandleShowOperationResult(pMe);
                            return TRUE;

                        case SS_DECODING_MSG_INVALID:
                            SETUP_MSGBOX_NO_TITLE;
                            SETUP_RESOURCE_STRING( IDS_OPERATION_RESULT_INVALID_MESSAGE );
							AUTOMATION_LOG_WSTR("%s",pMe->m_cMsgBoxText);

                            SETUP_NEW_DIALOG( SetupTimedMsgBox );
                            return TRUE;

                        case SS_DECODING_MSG_RESULT_REJECTED:
                            HandleShowResultMsgProblemCode(pMe);
                            return TRUE;

                        case SS_DECODING_MSG_RESULT_ERROR_CODE:
                            HandleShowResultMsgErrorCode(pMe);							
                            return TRUE;

                        case SS_DECODING_MSG_RESULT_END_TRANSACTION:
                            SETUP_MSGBOX_NO_TITLE;
                            SETUP_RESOURCE_STRING( IDS_OPERATION_RESULT_INVALID_MESSAGE );
							AUTOMATION_LOG_WSTR("%s",pMe->m_cMsgBoxText);

                            SETUP_NEW_DIALOG( SetupTimedMsgBox );
                            return TRUE;

                        case SS_DECODING_APP_ERROR:
                            assert(!"Message Decoding Internal Error");
                            return FALSE;

                        default:
                            assert(!"Unknown Status Code");
                    }
                }
                break;

            case SS_OPER_RES_CANT_INVOKE: /* error */
            case SS_OPER_RES_NO_RESPONSE: /* error */

				AUTOMATION_LOG("Communication Error!","");

                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,"SSM:Communication Error!\n",0,0,0);
                SETUP_MSGBOX_NO_TITLE;
                SETUP_RESOURCE_STRING( IDS_OPERATION_COMMUNICATION_ERROR );
                SETUP_NEW_DIALOG( SetupTimedMsgBox );
                return TRUE;

            default:
                assert(!"UNKNOWN STATUS CODE");
         }
         break;

      case EVT_KEY:
        if ((AVKType)wParam == AVK_CLR)
        {
            // do nothing if clear is pressed!
            return TRUE;
        }
        break;
   }

   return FALSE;
}

/*===========================================================================

FUNCTION SetupSendManagementRequest()

DESCRIPTION
	This function creates the StatusMsgBox dialog.

PARAMETERS:
	pMe:	Pointer to the app object

===========================================================================*/
static void SetupSendManagementRequest(CGSM1xSSManagementApp *pMe)
{
    assert(GET_NEXT_DIALOG_SETUP != NULL);

    SET_CURRENT_DIALOG_HANDLER( SendManagementRequest_HandleEvent );
    SHOW_DIALOG( IDD_MSG_BOX_DLG );
}

/*=============================================================================
FUNCTION: BuildManagementRequest

DESCRIPTION: 
    build the management command string, from values kept in pMe.

PARAMETERS:
    pMe: CGSM1xSSManagementApp object pointer

RETURN VALUE:
   none

=============================================================================*/
static void BuildManagementRequest(CGSM1xSSManagementApp *pMe)
{
    char* operationPrefix = "";
    char* siaPrefix = "";
    char* siaString = "";

    if (pMe->m_bSIAValid)
    {
        siaPrefix = "*";
        siaString = pMe->m_cSupplementaryInformationA;
    }
   
    // must re-initialize all SI data:
    pMe->m_bSIAValid = FALSE;


    switch (pMe->m_cSelectedOperation)
    {
        case MANAGEMENET_OPERATION_REGISTER:
            operationPrefix = "*";
            break;

        case MANAGEMENET_OPERATION_ACTIVATE:
            operationPrefix = "*";
            break;

        case MANAGEMENET_OPERATION_DEACTIVATE:
            operationPrefix = "#";
            break;

        case MANAGEMENET_OPERATION_ERASE:
            operationPrefix = "##";
            break;

        case MANAGEMENET_OPERATION_INTERROGATE:
            operationPrefix = "*#";
            break;

        case MANAGEMENET_OPERATION_CHANGE_PASSWORD:
            operationPrefix = "*03*"; 
            break;

        default:
            assert(!"SSM: unknown operation id");
            return;  /* error */
    }

    assert(STRLEN(operationPrefix) <= OPERATION_PREFIX_MAX_LENGTH);

    SNPRINTF(pMe->m_cManagementCommandString,
        sizeof(pMe->m_cManagementCommandString),"%s%s%s%s%s",
        operationPrefix,pMe->m_cServiceCode,siaPrefix,siaString,"#");

    // If command string is too long, null termination will be missed.
    if (MEMCHR(pMe->m_cManagementCommandString, 0, 
            sizeof(pMe->m_cManagementCommandString)) == NULL)
    {
        pMe->m_cManagementCommandString[sizeof(pMe->m_cManagementCommandString)-1]=0;
        GSM1X_LOG_ERROR(pMe->m_pSSUtilCls,"SSM: Command String Too Long",0,0,0);
        assert(! "SSM: Command String Too Long");
    } 

    return;
}

/*=============================================================================
FUNCTION: BuildManagementRequestMsgBoxText

DESCRIPTION: build the management request message box text string 

PARAMETERS:
    *pMe: CGSM1xSSManagementApp object pointer

RETURN VALUE:
   None

=============================================================================*/
static void BuildManagementRequestMsgBoxText(CGSM1xSSManagementApp *pMe)
{
    AECHAR  szCommandText[ MANAGEMENT_COMMAND_MAX_LENGTH +1 ];
    AECHAR  szFormatText[ FORMAT_MAX_TEXT_LEN ];
    AECHAR  endOfLineString[2] = {'\n', '\0'};

    SETUP_MSGBOX_NO_TITLE;
    SETUP_RESOURCE_STRING( IDS_WAITING_FOR_RESPONSE );

    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                SSMANAGEMENT_RES_FILE,
                                IDS_OPERATION_REQUEST_FORMAT,
                                szFormatText,
                                sizeof(szFormatText));

    STRTOWSTR(pMe->m_cManagementCommandString, szCommandText, sizeof(szCommandText));

    // append to string
    // format text should contain '%s %s' if the command is to be shown!
    WSPRINTF(MSGBOX_APPENDED_TEXT_PTR,
             MSGBOX_APPENDED_TEXT_MAX_SIZE,
             szFormatText,
             endOfLineString,
             szCommandText
             );
}

/*=============================================================================
FUNCTION: PostManagementRequestEvent

DESCRIPTION: send the management request event

PARAMETERS:
    *pMe: CGSM1xSSManagementApp object pointer

RETURN VALUE:
   None

=============================================================================*/
static void PostManagementRequestEvent(CGSM1xSSManagementApp *pMe)
{
   /* post a request event to state machine ! */
   (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_SS_MANAGEMENT,
                           EVT_SS_REQUEST, 0, (uint32)pMe->m_cManagementCommandString);

   GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSM:Command:%s\n", (uint32)pMe->m_cManagementCommandString, 0, 0);
}

/*=============================================================================
FUNCTION: HandleManagementRequest

DESCRIPTION: 
    build the management request command string, 
    setup its dialog and post its event.

PARAMETERS:
    *pMe: CGSM1xSSManagementApp object pointer

RETURN VALUE:
   None

=============================================================================*/
static void HandleManagementRequest(CGSM1xSSManagementApp *pMe)
{
   BuildManagementRequest(pMe);
   BuildManagementRequestMsgBoxText(pMe);

   SETUP_NEW_DIALOG( SetupSendManagementRequest );
   PostManagementRequestEvent(pMe);
}

/*=============================================================================
FUNCTION: ShowOperationResult_HandleEvent

DESCRIPTION:
   This dialog handler is to create a dialog with only a static control (no
   softkeys) in it.
   
   After the dialog is created, it starts a timer and when the timer expires
   it closes the dialog (in case if the dialog is not yet closed by the user)

PARAMETERS:
   pMe [in]: CGSM1xSSManagementApp object pointer.            
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

=============================================================================*/
static boolean ShowOperationResult_HandleEvent(
   CGSM1xSSManagementApp *pMe,
   AEEEvent              eCode,
   uint16                wParam,
   uint32                dwParam
)
{
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         {
            if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                                 AEECLSID_STATIC,
                                                 (void**)&pMe->m_pMsgBox_IStatic))
            {
                return FALSE;
            }

            ISTATIC_SetRect(pMe->m_pMsgBox_IStatic, &pMe->m_cMenuFrame);

            ISTATIC_SetProperties(pMe->m_pMsgBox_IStatic,
                                  ST_CENTERTEXT | ST_MIDDLETEXT);

            (void) ISTATIC_SetText(pMe->m_pMsgBox_IStatic,
                                   NULL,
                                   pMe->m_cMsgBoxText,
                                   AEE_FONT_NORMAL,
                                   AEE_FONT_NORMAL);

            // Always start a timer for the message box. 
            CALLBACK_Init(&pMe->m_UITimerCB, DialogTimeoutCallback, pMe);

            (void) ISHELL_SetTimerEx(
                      pMe->a.m_pIShell,
                      MSGBOX_TIMEOUT,
                      &pMe->m_UITimerCB
                      );

            return TRUE;
         }

      case EVT_DIALOG_START:
         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_SS_MANAGEMENT,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_pMsgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         CALLBACK_Cancel(&pMe->m_UITimerCB); // cancel timer callback

         if (NULL != pMe->m_pMsgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_pMsgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_pMsgBox_IStatic);
            pMe->m_pMsgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_KEY:
		 if ((AVKType)wParam != AVK_CLR) 
			break;
		 // else fall through.
      case EVT_DIALOG_TIMEOUT: 
	  {
 		 if (pMe->m_bDoEndCall == TRUE) 
         {
			// clear call if no other voice calls exist.
            if (ISSUTILCLS_GetCallDependentCounter(pMe->m_pSSUtilCls) == 0)
            {
                IPHONE_EndCall(pMe->m_pIPhone);
            }

			// management operation has ended
			ISSUTILCLS_DecCallIndependentCounter(pMe->m_pSSUtilCls);
			pMe->m_bDoEndCall = FALSE;
         } 

         SETUP_NEW_DIALOG( GET_NEXT_DIALOG_SETUP );
         return TRUE;
      }
   }

   return FALSE;
}

/*===========================================================================

FUNCTION SetupShowOperationResult()

DESCRIPTION
	This function creates the ShowOperationResult dialog.

PARAMETERS:
	pMe:	Pointer to the app object

===========================================================================*/
static void SetupShowOperationResult(CGSM1xSSManagementApp *pMe)
{
    assert(GET_NEXT_DIALOG_SETUP != NULL);

    SET_CURRENT_DIALOG_HANDLER( ShowOperationResult_HandleEvent );
    SHOW_DIALOG( IDD_MSG_BOX_DLG );
}

/*=============================================================================
FUNCTION: AddOperationResultStatusToMsgBoxText

DESCRIPTION: add the operation result status string to the message box text

PARAMETERS:
    pMe: CGSM1xSSManagementApp object pointer

RETURN VALUE:
   None
=============================================================================*/
static void AddOperationResultStatusToMsgBoxText(CGSM1xSSManagementApp *pMe)
{
    AECHAR  szStatusText[STATUS_MAX_TEXT_LEN];
    AECHAR  szFormatText[FORMAT_MAX_TEXT_LEN];
    AECHAR  endOfLineString[2] = {'\n', '\0'};

    if (!pMe->m_bResultStatusValid) 
	{
		AUTOMATION_LOG("Success!","");
		return;
	}

    if (pMe->m_bResultStatusActive)
    {
        if (pMe->m_bResultStatusQuiescent)
        {
            (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                        SSMANAGEMENT_RES_FILE,
                                        IDS_RESULT_ACTIVE_AND_QUIESCENT,
                                        szStatusText,
                                        sizeof(szStatusText));  
        }
        else 
        {
            (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                        SSMANAGEMENT_RES_FILE,
                                        IDS_RESULT_ACTIVE,
                                        szStatusText,
                                        sizeof(szStatusText));  
        }
    }
    else
    {
        (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                    SSMANAGEMENT_RES_FILE,
                                    IDS_RESULT_NOT_ACTIVE,
                                    szStatusText,
                                    sizeof(szStatusText));  
    }

	AUTOMATION_LOG_WSTR("Success:%s",szStatusText);
    
    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                SSMANAGEMENT_RES_FILE,
                                IDS_RESULT_STATUS_FORMAT,
                                szFormatText,
                                sizeof(szFormatText));

    // append status string.
    // format text should contain '%s %s %s' !
    WSPRINTF(MSGBOX_APPENDED_TEXT_PTR, 
             MSGBOX_APPENDED_TEXT_MAX_SIZE,
             szFormatText,
             endOfLineString,
             szStatusText,
             endOfLineString );
}

/*=============================================================================
FUNCTION: AddOperationResultForwardToNumberToMsgBoxText

DESCRIPTION: add the operation result forward to number string to the message box text

PARAMETERS:
    *pMe: CGSM1xSSManagementApp object pointer

RETURN VALUE:
   None
=============================================================================*/
static void AddOperationResultForwardToNumberToMsgBoxText(
    CGSM1xSSManagementApp *pMe
    )
{
    // append FTN string
    if (!pMe->m_bResultForwardedToNumberValid) return;

    STRTOWSTR(pMe->m_cResultForwardedToNumber,
              MSGBOX_APPENDED_TEXT_PTR,
              MSGBOX_APPENDED_TEXT_MAX_SIZE
              );

	AUTOMATION_LOG("FTN:%s",pMe->m_cResultForwardedToNumber);

    // FTN string is not longer valid
    pMe->m_bResultForwardedToNumberValid = FALSE;
}

/*=============================================================================
FUNCTION: BuildOperationResultMsgBoxText

DESCRIPTION: build the operation result message box text string 

PARAMETERS:
    *pMe: CGSM1xSSManagementApp object pointer

RETURN VALUE:
   None

=============================================================================*/
static void BuildOperationResultMsgBoxText(CGSM1xSSManagementApp *pMe)
{
    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                SSMANAGEMENT_RES_FILE,
                                IDS_RESULT_HEADER,
                                pMe->m_cMsgBoxText,
                                sizeof(pMe->m_cMsgBoxText));

    AddOperationResultStatusToMsgBoxText(pMe);
    AddOperationResultForwardToNumberToMsgBoxText(pMe);
}

/*=============================================================================
FUNCTION: HandleShowOperationResult

DESCRIPTION: build the operation result message box string and setup its dialog

PARAMETERS:
    pMe: CGSM1xSSManagementApp object pointer

RETURN VALUE:
    None

=============================================================================*/
static void HandleShowOperationResult(CGSM1xSSManagementApp *pMe)
{
   BuildOperationResultMsgBoxText(pMe);

   SETUP_NEW_DIALOG( SetupShowOperationResult );
}

/*=============================================================================
FUNCTION: BuildResultMsgFailureCodeMsgBoxText

DESCRIPTION: build the result message failure code message box text string 

PARAMETERS:
    pMe: CGSM1xSSManagementApp object pointer
    failureFormatResourceId: String resource id to use as a format string
    failureHeaderResourceId: String resource id to use as a failure text string header

RETURN VALUE:
    None

=============================================================================*/
static void BuildResultMsgFailureCodeMsgBoxText(
    CGSM1xSSManagementApp *pMe,
    uint16                failureFormatResourceId,
    uint16                failureHeaderResourceId,
    uint16                failureStringResourceId
    )
{
    AECHAR  szFormatText[ FORMAT_MAX_TEXT_LEN ];
    AECHAR  endOfLineString[2] = {'\n', '\0'};
    
    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                SSMANAGEMENT_RES_FILE,
                                failureFormatResourceId,
                                szFormatText,
                                sizeof(szFormatText));

    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                SSMANAGEMENT_RES_FILE,
                                failureHeaderResourceId,
                                pMe->m_cMsgBoxText,
                                sizeof(pMe->m_cMsgBoxText));

    // assume that resource id of zero specifies no resource
    if (failureStringResourceId != 0)
    {
        (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                    SSMANAGEMENT_RES_FILE,
                                    failureStringResourceId,
                                    MSGBOX_APPENDED_TEXT_PTR,
                                    MSGBOX_APPENDED_TEXT_MAX_SIZE);
    }


    // append to the end of the message box text.
    // format text should contain '%s %d' if the failure code is to be shown!
    WSPRINTF(MSGBOX_APPENDED_TEXT_PTR,
             MSGBOX_APPENDED_TEXT_MAX_SIZE,
             szFormatText,
             endOfLineString,
             pMe->m_cResultFailureCode );
}

/*=============================================================================
FUNCTION: HandleShowResultMsgErrorCode

DESCRIPTION: build the operation result message box string for an error code result
             and setup its dialog

PARAMETERS:
    *pMe: CGSM1xSSManagementApp object pointer

RETURN VALUE:
   None

=============================================================================*/
static void HandleShowResultMsgErrorCode(CGSM1xSSManagementApp *pMe)
{
   uint16 errorCodeResourceId;

   switch (pMe->m_cResultFailureCode)
   {
    case SSMSG_ERROR_CODE_UNKNOWNSUBSCRIBER: 
         errorCodeResourceId = IDS_ERROR_UNKNOWN_SUBSCRIBER;
         break;
    case SSMSG_ERROR_CODE_ILLEGALSUBSCRIBER:
         errorCodeResourceId = IDS_ERROR_ILLEGAL_SUBSCRIBER;
         break;
    case SSMSG_ERROR_CODE_BEARERSERVICENOTPROVISIONED:
         errorCodeResourceId = IDS_ERROR_BEARER_SERVICE_NOT_PROVISIONED;
         break;
    case SSMSG_ERROR_CODE_TELESERVICENOTPROVISIONED:
         errorCodeResourceId = IDS_ERROR_TELESERVICE_NOT_PROVISIONED;
         break;
    case SSMSG_ERROR_CODE_ILLEGALEQUIPMENT:
         errorCodeResourceId = IDS_ERROR_ILLEGAL_EQUIPMENT;
         break;
    case SSMSG_ERROR_CODE_CALLBARRED:
         errorCodeResourceId = IDS_ERROR_CALL_BARRED;
         break;
    case SSMSG_ERROR_CODE_ILLEGALSS_OPERATION:
         errorCodeResourceId = IDS_ERROR_ILLEGAL_SS_OPERATION;
         break;
    case SSMSG_ERROR_CODE_SS_ERRORSTATUS:
         errorCodeResourceId = IDS_ERROR_SS_ERROR_STATUS;
         break;
    case SSMSG_ERROR_CODE_SS_NOTAVAILABLE:
         errorCodeResourceId = IDS_ERROR_SS_NOT_AVAILABLE;
         break;
    case SSMSG_ERROR_CODE_SS_SUBSCRIPTIONVIOLATION:
         errorCodeResourceId = IDS_ERROR_SS_SUBSCRIPTION_VIOLATION;
         break;
    case SSMSG_ERROR_CODE_SS_INCOMPATIBILITY:
         errorCodeResourceId = IDS_ERROR_SS_INCOMPATIBILITY;
         break;
    case SSMSG_ERROR_CODE_FACILITYNOTSUPPORTED:
         errorCodeResourceId = IDS_ERROR_FACILITY_NOT_SUPPORTED;
         break;
    case SSMSG_ERROR_CODE_ABSENTSUBSCRIBER:
         errorCodeResourceId = IDS_ERROR_ABSENT_SUBSCRIBER;
         break;
    case SSMSG_ERROR_CODE_SHORTTERMDENIAL:
         errorCodeResourceId = IDS_ERROR_SHORT_TERM_DENIAL;
         break;
    case SSMSG_ERROR_CODE_LONGTERMDENIAL:
         errorCodeResourceId = IDS_ERROR_LONG_TERM_DENIAL;
         break;
    case SSMSG_ERROR_CODE_SYSTEMFAILURE:
         errorCodeResourceId = IDS_ERROR_SYSTEM_FAILURE;
         break;
    case SSMSG_ERROR_CODE_DATAMISSING:
         errorCodeResourceId = IDS_ERROR_DATA_MISSING;
         break;
    case SSMSG_ERROR_CODE_UNEXPECTEDDATAVALUE:
         errorCodeResourceId = IDS_ERROR_UNEXPECTED_DATA_VALUE;
         break;
    case SSMSG_ERROR_CODE_PW_REGISTRATIONFAILURE:
         errorCodeResourceId = IDS_ERROR_PW_REGISTRATION_FAILURE;
         break;
    case SSMSG_ERROR_CODE_NEGATIVEPW_CHECK:
         errorCodeResourceId = IDS_ERROR_NEGATIVE_PW_CHECK;
         break;
    case SSMSG_ERROR_CODE_NUMBEROFPW_ATTEMPTSVIOLATION:
         errorCodeResourceId = IDS_ERROR_NUMBER_OF_PW_ATTEMPTS_VIOLATION;
         break;
    case SSMSG_ERROR_CODE_POSITIONMETHODFAILURE:
         errorCodeResourceId = IDS_ERROR_POSITION_METHOD_FAILURE;
         break;
    case SSMSG_ERROR_CODE_UNKNOWNALPHABET:
         errorCodeResourceId = IDS_ERROR_UNKNOWN_ALPHABET;
         break;
    case SSMSG_ERROR_CODE_USSD_BUSY:
         errorCodeResourceId = IDS_ERROR_USSD_BUSY;
         break;
    case SSMSG_ERROR_CODE_REJECTEDBYUSER:
         errorCodeResourceId = IDS_ERROR_REJECTED_BY_USER;
         break;
    case SSMSG_ERROR_CODE_REJECTEDBYNETWORK:
         errorCodeResourceId = IDS_ERROR_REJECTED_BY_NETWORK;
         break;
    case SSMSG_ERROR_CODE_DEFLECTIONTOSERVEDSUBSCRIBER:
         errorCodeResourceId = IDS_ERROR_DEFLECTION_TO_SERVED_SUBSCRIBER;
         break;
    case SSMSG_ERROR_CODE_SPECIALSERVICECODE:
         errorCodeResourceId = IDS_ERROR_SPECIAL_SERVICE_CODE;
         break;
    case SSMSG_ERROR_CODE_INVALIDDEFLECTEDTONUMBER:
         errorCodeResourceId = IDS_ERROR_INVALID_DEFLECTED_TO_NUMBER;
         break;
    case SSMSG_ERROR_CODE_MAXNUMBEROFMPTY_PARTICIPANTSEXCEEDED:
         errorCodeResourceId = IDS_ERROR_MAX_NUMBER_OF_MPTY_PARTICIPANTS_EXCEEDED;
         break;
    case SSMSG_ERROR_CODE_RESOURCESNOTAVAILABLE:
        errorCodeResourceId = IDS_ERROR_RESOURCES_NOT_AVAILABLE;
         break;

    default:
       errorCodeResourceId = IDS_ERROR_UNKNOWN;
   }

   BuildResultMsgFailureCodeMsgBoxText(pMe,
                                       IDS_RESULT_ERROR_CODE_FORMAT,
                                       IDS_OPERATION_RESULT_ERROR_CODE,
                                       errorCodeResourceId
                                       );

   AUTOMATION_LOG_WSTR("%s",pMe->m_cMsgBoxText);

   SETUP_NEW_DIALOG( SetupTimedMsgBox );
}

/*=============================================================================
FUNCTION: HandleShowResultMsgProblemCode

DESCRIPTION: build the operation result message box string for a problem code result
             and setup its dialog

PARAMETERS:
    *pMe: CGSM1xSSManagementApp object pointer

RETURN VALUE:
   None

=============================================================================*/
static void HandleShowResultMsgProblemCode(CGSM1xSSManagementApp *pMe)
{
   BuildResultMsgFailureCodeMsgBoxText(pMe,
                                       IDS_RESULT_REJECT_FORMAT,
                                       IDS_OPERATION_RESULT_REJECT,
                                       0
                                       );

   AUTOMATION_LOG_WSTR("%s",pMe->m_cMsgBoxText);

   SETUP_NEW_DIALOG( SetupTimedMsgBox );
}

/*===========================================================================

FUNCTION ChangePassword_HandleEvent

DESCRIPTION
	This is the ChangePassword dialog event handler.

PARAMETERS:
   pMe [in]: CGSM1xSSManagementApp object pointer.            
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE
  TRUE: If event was processed
  FALSE: If event was not processed

===========================================================================*/
static boolean ChangePassword_HandleEvent(
    CGSM1xSSManagementApp *pMe,
    AEEEvent              eCode, 
    uint16                wParam, 
    uint32                dwParam
    )
{
    ITextCtl* pTextCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_pCurrentIDialog,
                                                       IDC_CHANGE_PASSWORD);
    assert(NULL != pTextCtl);

    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            switch (pMe->m_cChangePasswordScreen)
            {
                case CHANGE_PASSWORD_ENTER_OLD:
                    ITEXTCTL_SetTitle(pTextCtl,
                                      SSMANAGEMENT_RES_FILE,
                                      IDS_CHANGE_OLD_PASSWORD_TITLE,
                                      NULL);
                    break;

                case CHANGE_PASSWORD_ENTER_NEW:
                    ITEXTCTL_SetTitle(pTextCtl,
                                      SSMANAGEMENT_RES_FILE,
                                      IDS_CHANGE_FIRST_NEW_PASSWORD_TITLE,
                                      NULL);
                    break;
                case CHANGE_PASSWORD_VERIFY_NEW:
                    ITEXTCTL_SetTitle(pTextCtl,
                                      SSMANAGEMENT_RES_FILE,
                                      IDS_CHANGE_SECOND_NEW_PASSWORD_TITLE,
                                      NULL);
                    break;

                default:
                    assert(!"unknown changePasswordScreen value");
            }
            return TRUE;

        case EVT_DIALOG_START:
            ITEXTCTL_SetMaxSize(pTextCtl, PASSWORD_LENGTH);
            ITEXTCTL_SetProperties(pTextCtl, TP_FRAME);
            (void) ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_NUMBERS);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch ((AVKType)wParam) 
            {
                 case AVK_SELECT:
                 {
                    WSTRTOSTR(ITEXTCTL_GetTextPtr(pTextCtl), 
                                pMe->m_cSupplementaryInformationA, 
                                sizeof(pMe->m_cSupplementaryInformationA));

                    pMe->m_bSIAValid = 
                        IsPasswordStringValid(pMe->m_cSupplementaryInformationA);

                    if (!pMe->m_bSIAValid)
                    {
                        SET_NEXT_DIALOG_SETUP_TO_CURRENT;
                        SETUP_MSGBOX_NO_TITLE;
                        SETUP_RESOURCE_STRING( IDS_INVALID_PASSWORD_STRING );
                        SETUP_NEW_DIALOG( SetupTimedMsgBox );
                        return TRUE;
                    }
                   
                    switch (pMe->m_cChangePasswordScreen)
                    {
                        case CHANGE_PASSWORD_ENTER_OLD:
                            STRLCPY(pMe->m_cOldPassword, pMe->m_cSupplementaryInformationA,
                                sizeof(pMe->m_cOldPassword));
                            pMe->m_cChangePasswordScreen = CHANGE_PASSWORD_ENTER_NEW;
                            SETUP_NEW_DIALOG( pMe->m_pCurrentDialogSetupHandler );
                            break;

                        case CHANGE_PASSWORD_ENTER_NEW:
                            STRLCPY(pMe->m_cNewPassword, pMe->m_cSupplementaryInformationA,
                                sizeof(pMe->m_cNewPassword));
                            pMe->m_cChangePasswordScreen = CHANGE_PASSWORD_VERIFY_NEW;
                            SETUP_NEW_DIALOG( pMe->m_pCurrentDialogSetupHandler );
                            break;

                        case CHANGE_PASSWORD_VERIFY_NEW:
                            if (STRCMP(pMe->m_cNewPassword, pMe->m_cSupplementaryInformationA) != 0)
                            {
                                SET_NEXT_DIALOG_SETUP_TO_CURRENT;
                                SETUP_MSGBOX_NO_TITLE;
                                SETUP_RESOURCE_STRING( IDS_SECOND_NEW_PASSWORD_DIFFERENT_THEN_FIRST );
                                SETUP_NEW_DIALOG( SetupTimedMsgBox );
                            }
                            else
                            {
                                SET_NEXT_DIALOG_SETUP( SetupCallBarringMenu );
                                BuildPasswordDataIntoSIA(pMe);
                                HandleManagementRequest(pMe);
                            }
                            break;
                    }
                    return TRUE;
                 }

                 case AVK_CLR:
                 {
                    switch (pMe->m_cChangePasswordScreen)
                    {
                        case CHANGE_PASSWORD_ENTER_OLD:
                            SETUP_NEW_DIALOG( SetupCallBarringMenu );
                            break;

                        case CHANGE_PASSWORD_ENTER_NEW:
                            pMe->m_cChangePasswordScreen = CHANGE_PASSWORD_ENTER_OLD;
                            SETUP_NEW_DIALOG( pMe->m_pCurrentDialogSetupHandler );
                            break;

                        case CHANGE_PASSWORD_VERIFY_NEW:
                            pMe->m_cChangePasswordScreen = CHANGE_PASSWORD_ENTER_NEW;
                            SETUP_NEW_DIALOG( pMe->m_pCurrentDialogSetupHandler );
                            break;
                    }
                    return TRUE;
                 }
            }
    }

    return FALSE;
}

/*===========================================================================

FUNCTION SetupChangePasswordMenu()

DESCRIPTION
	This function creates the ChangePassword dialog.

PARAMETERS:
	*pMe: CGSM1xSSManagementApp object pointer

RETURN VALUE:
   None

===========================================================================*/
static void SetupChangePasswordMenu(CGSM1xSSManagementApp *pMe)
{
    SET_CURRENT_DIALOG_HANDLER( ChangePassword_HandleEvent );
	SHOW_DIALOG( IDD_CHANGE_PASSWORD_DLG );
}

/*=============================================================================
FUNCTION: BuildPasswordDataIntoSIA

DESCRIPTION: build the password request data into the SSIA

PARAMETERS:
    *pMe: CGSM1xSSManagementApp object pointer

RETURN VALUE:
   None

=============================================================================*/
static void BuildPasswordDataIntoSIA(
    CGSM1xSSManagementApp *pMe
    )
{    
    SNPRINTF(pMe->m_cSupplementaryInformationA,
            sizeof(pMe->m_cSupplementaryInformationA),
            "%s*%s*%s",
            pMe->m_cOldPassword,
            pMe->m_cNewPassword,
            pMe->m_cNewPassword
            );           
}

/*=============================================================================
FUNCTION: SetupAndShowAboutDialog

DESCRIPTION: setup and show the about dialog

PARAMETERS:
    *pMe: CGSM1xSSManagementApp object pointer

RETURN VALUE:
   None

=============================================================================*/
static void SetupAndShowAboutDialog(CGSM1xSSManagementApp *pMe)
{
    AECHAR  szAboutBuffer[ ABOUT_BUFFER_MAX_TEXT_LEN ];
    AECHAR  endOfLineString[2] = {'\n', '\0'};

    SETUP_MSGBOX_TITLE( IDS_MAIN_MENU_TITLE );
    SETUP_RESOURCE_STRING( IDS_VERSION );

    // format string needs to contain %s%s%s%s
    szAboutBuffer[0] = '\0';
    (void) ISHELL_LoadResString(
              pMe->a.m_pIShell,
              SSMANAGEMENT_RES_FILE,
              IDS_ABOUT_FORMAT,
              szAboutBuffer,
              sizeof(szAboutBuffer)
              );

    // append to string                 
    WSPRINTF(MSGBOX_APPENDED_TEXT_PTR,
             MSGBOX_APPENDED_TEXT_MAX_SIZE,
             szAboutBuffer,
             endOfLineString,
             ISSUTILCLS_GetVersionString(pMe->m_pSSUtilCls),
             endOfLineString,
             endOfLineString
             );

    // load copyright text
    szAboutBuffer[0] = '\0';
    (void) ISHELL_LoadResString(
              pMe->a.m_pIShell,
              SSMANAGEMENT_RES_FILE,
              IDS_ABOUT_COPYRIGHT,
              szAboutBuffer,
              sizeof(szAboutBuffer)
              );

    // append to string                 
    WSTRLCAT(MSGBOX_APPENDED_TEXT_PTR,
             szAboutBuffer,
             MSGBOX_APPENDED_TEXT_MAX_SIZE
             );

    SETUP_NEW_DIALOG( SetupAboutDialog );
}