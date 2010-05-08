#ifndef __SSMANAGEMENT_APP_H__
#define __SSMANAGEMENT_APP_H__

/*=============================================================================
FILE: SSManagement_App.h

SERVICES: 
	GSM1x SS Management Applet

GENERAL DESCRIPTION:
	GSM1x SS Management Applet Definitions

PUBLIC CLASSES:
    CGSM1xSSManagementApp

INITIALIZATION AND SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/* 

Introduction to the GSM1x SS Management Applet
----------------------------------------------

The GSM1x SS Management Applet (for short, SSM) is a standalone applet, 
that lets the user manage Supplementary Services. For example, the user 
can activate or deactivate a "Call Waiting" service.

The SSM is made of two major components:
1. UI  - a User Interface, organized as a set of Dialog Objects.
2. FSM - has the actual Logic needed for managing SS.

As the SSM is an event driven applet, both the UI and FSM are implemented 
as State Machines. The internal state of the SSM consists, therefore, 
of its UI state and its FSM state. This can be a little confusing, 
because the FSM is sometimes called "The State Machine" (of SSM).

A state is defined by an Event Handler, which is
a function that accepts an event, does some operation and  
eventually moves the control to another state.


Logging and Simulation
----------------------

SSM code can be compiled to work with BREW simulator.
Within the simulator, some of the code is disabled.

SSM can emit logs with BREW's standard function, DBGPRINTF.
But SSM also has a special logging capability using the SSUtil module.
Please refer to SSUtil's documentation for more details.

*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "SSUtil.h"

#include "AEE.h"              // Standard AEE Declarations
#include "AEEAppGen.h"        // AEEApplet definitions
#include "AEEShell.h"         // AEE Shell Services
#include "AEEStdLib.h"        // AEE StdLib Services

#include "GSM1xPhone.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// Password Length (Number of password digits)
#define PASSWORD_LENGTH 4

// Maximum number of 'forward to number' digits
#define FORWARD_TO_NUMBER_MAX_LENGTH 25

// Maximum number of the first supplementary service (SIA) digits
// In the current usage it is = max (PASSWORD_LENGTH,FORWARD_TO_NUMBER_MAX_INPUT_LENGTH)
#define SUPPLEMENTARY_INFORMATION_A_MAX_LENGTH (FORWARD_TO_NUMBER_MAX_LENGTH)

// Service Code string maximum length
#define SERVICE_CODE_MAX_LENGTH 3

// Operation Prefix string maximum length
#define OPERATION_PREFIX_MAX_LENGTH 4

// Management command maximum length must suffice for: 
// operation_prefix + service_code + '*' + SIA + '#' 
//
// If we implement SIB and SIC, then SIA should be replaced with:
// SI = SIA + '*' + SIB + '*' SIC.
//
//
#define MANAGEMENT_COMMAND_MAX_LENGTH \
            (OPERATION_PREFIX_MAX_LENGTH \
            + SERVICE_CODE_MAX_LENGTH \
            + 1 /* '*' */ \
            + SUPPLEMENTARY_INFORMATION_A_MAX_LENGTH \
            + 1 /* '#' */)

// Max number of characters allowed in the MsgBox text
#define MSGBOX_MAX_TEXT_LEN  256

// Max number of characters allowed in the MsgBox title
#define MSGBOX_MAX_TITLE_LEN 30

// SS operation result constants
// -----------------------------
// like enum:
#define SS_OPER_RES_SUCCESS       0    /* the operation successfully ended */
#define SS_OPER_RES_CANT_INVOKE   1    /* the operation can't be invoked */
#define SS_OPER_RES_NO_RESPONSE   2    /* timer expired or traffic channel lost */
                                       /* while waiting for a response from the network */

// Management possible operations
// ------------------------------
#define MANAGEMENET_OPERATION_REGISTER 0
#define MANAGEMENET_OPERATION_ACTIVATE 1
#define MANAGEMENET_OPERATION_DEACTIVATE 2
#define MANAGEMENET_OPERATION_ERASE 3
#define MANAGEMENET_OPERATION_INTERROGATE 4
#define MANAGEMENET_OPERATION_CHANGE_PASSWORD 5
typedef int SSManagementOperationType; /* like enum */

// Screens Password Change
// -----------------------
#define CHANGE_PASSWORD_ENTER_OLD    0
#define CHANGE_PASSWORD_ENTER_NEW    1
#define CHANGE_PASSWORD_VERIFY_NEW   2
typedef int ChangePasswordScreenType; /* like enum */

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

// Event Handlers and Setup Handlers
// ---------------------------------
// The FSM and UI are implemented as state machines. 
//
// Each state has an event handler -
// STATE_EVENT_HANDLER in FSM,
// DIALOG_EVENT_HANDLER in UI.
//
// Each state may also have an setup function -
// STATE_SETUP_HANDLER in FSM,
// DIALOG_SETUP_HANDLER in UI.

// forward declarations
struct _CGSM1xSSManagementApp;
struct IGSM1xSig;

// dialog event handler (function pointer type)
typedef boolean (*DIALOG_EVENT_HANDLER)(
        struct _CGSM1xSSManagementApp *pMe,
        AEEEvent                      eCode,
        uint16                        wParam,
        uint32                        dwParam
    );

// state event handler (function pointer type)
typedef boolean (*STATE_EVENT_HANDLER)(
        struct _CGSM1xSSManagementApp *pMe,
        AEEEvent                      eCode,
        uint16                        wParam,
        uint32                        dwParam
    );

// dialog setup (function pointer type)
typedef void (*DIALOG_SETUP_HANDLER)(struct _CGSM1xSSManagementApp *pMe);

// state setup (function pointer type)
typedef void (*STATE_SETUP_HANDLER)(struct _CGSM1xSSManagementApp *pMe);


// CGSM1xSSManagementApp
// --------------------
// This structure contains applet data. Most of the data is contained 
// in its UI and FSM subunits.
// 
typedef struct _CGSM1xSSManagementApp
{
    AEEApplet a;                  // must have AEEApplet first. 
    AEERect   m_cMenuFrame;
    boolean   m_bSuspended; // applet received EVT_APP_SUSPEND
    boolean   m_AppStarted;       // applet received EVT_APP_START
    boolean   m_WasDialogHandlerInitCalled; // was the dialog handler init function called
    boolean   m_WasStateMachineInitCalled; // was the state machine init function calle

    // UI Data
    // -------
    uint16    m_cCurrentDialogID;   // Current dialog resource Id
    IDialog  *m_pCurrentIDialog;    // Current dialog

    DIALOG_SETUP_HANDLER  m_pCurrentDialogSetupHandler; // Current dialog setup handler
    DIALOG_EVENT_HANDLER  m_pCurrentDialogHandleEvent;  // Current dialog event handler

    AEECallback           m_UITimerCB; // Timer callback

    // Next dialog setup handler.
    DIALOG_SETUP_HANDLER  m_pNextDialogSetupHandler;

    // Dialog to pop up after ChooseOperationMenu finishes.
    DIALOG_SETUP_HANDLER  m_pChooseOperationPrevDialogSetupHandler;

	// Dialog to pop up after EnterPasswordMenu finishes.
    DIALOG_SETUP_HANDLER  m_pPasswordPrevDialogSetupHandler;
    
    IStatic  *m_pMsgBox_IStatic; // Pointer to the ISTATIC control used by the MsgBox dialog
    AECHAR    m_cMsgBoxTitle[ MSGBOX_MAX_TITLE_LEN+1 ]; // Msgbox's title string
    AECHAR    m_cMsgBoxText[ MSGBOX_MAX_TEXT_LEN+1 ]; // Msgbox's text string

    // next section makes the user request, for example, 
    // Do "Activate" of "Barring" for "All Incoming Calls".
    uint16    m_ServiceCategory; // Category of Service (e.g. Barring)
    uint16    m_ServiceSpecific; // Specific Service (e.g. All Incoming Calls)
    uint16    m_cSelectedOperation; // Operation to do with the Service (e.g. Activate)

    boolean   m_bDoEndCall; // tells some handlers to consider an IPHONE_EndCAll().
    boolean   m_bSIAValid;  // m_cSupplementaryInformationA buffer validity flag
    char      m_cSupplementaryInformationA[ SUPPLEMENTARY_INFORMATION_A_MAX_LENGTH+1 ];
    char      m_cServiceCode[ SERVICE_CODE_MAX_LENGTH+1 ];
    char      m_cManagementCommandString[ MANAGEMENT_COMMAND_MAX_LENGTH+1 ];
    uint16    m_cSScode;   // SS Code value, as defined in SSMsg.h 

    // Change Password Information:
    // indicator for which screen the password procedure should display
    ChangePasswordScreenType m_cChangePasswordScreen; 
    char                     m_cOldPassword[ PASSWORD_LENGTH+1 ];
    char                     m_cNewPassword[ PASSWORD_LENGTH+1 ];

    // SS Operation Result Information:
    boolean  m_bResultStatusValid;
    boolean  m_bResultStatusActive;
    boolean  m_bResultStatusQuiescent;
    uint16   m_cResultFailureCode;  // Error code value or Problem code value
    boolean  m_bResultForwardedToNumberValid;
    char     m_cResultForwardedToNumber[ FORWARD_TO_NUMBER_MAX_LENGTH+1 ];

    // State Machine specific data
	// ---------------------------
    STATE_EVENT_HANDLER   m_pCurrentStateHandler; // Current state event handler
    ISSUtilCls           *m_pSSUtilCls;           // SS utilities class

    AEECallback           m_StateMachineTimerCB;  // Timer callback

    IPhone*               m_pIPhone;      // Phone interface
    struct IGSM1xSig*     m_pIGSM1xSig;   // GSM1x Signalling interface
	
} CGSM1xSSManagementApp;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: CGSM1xSSManagementApp_New

DESCRIPTION:   
   This function is invoked when the app is being loaded. It creates a new 
   instance of the CGSM1xSSManagementApp structure for this applet.

PARAMETERS:
   pIShell [in]:    Contains a valid pointer to the AEE IShell interface
   pIModule [in]:   Contains a valid pointer to the Applet's module 
   ppObj [out] :    Address of a pointer. On return, *ppobj points to a 
                    valid AEEApplet structure, allocated by this function.
RETURN VALUE:
   SUCCESS: If successful
   EFAILED: If failed

=============================================================================*/
int CGSM1xSSManagementApp_New(
    IShell                *pIShell,
    IModule               *pIModule,
    CGSM1xSSManagementApp **ppObj
    );

#endif // end __SSMANAGEMENT_APP_H__
