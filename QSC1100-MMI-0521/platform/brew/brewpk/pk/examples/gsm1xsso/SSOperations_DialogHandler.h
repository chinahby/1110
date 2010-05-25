#ifndef __SS_OPERATIONS_DIALOG_HANDLER_H__
#define __SS_OPERATIONS_DIALOG_HANDLER_H__

/*=============================================================================
FILE: SSOperations_DialogHandler.h

SERVICES: GSM1x SS Operations Dialog Handlers

GENERAL DESCRIPTION:
      This file defines the SS Operations dialog component, which is responsible
      for handling the various User interface Dialog screen.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "AEE.h"                  // Standard AEE Declarations
#include "AEEShell.h"             // AEE Shell Services
#include "AEEAddrBook.h"          // AEE Address Book Services

#include "GSM1xPhone.h"

#include "SSCallCommonDefinitions.h"
#include "SSCallDatabase.h"

#include "SSUtil.h"
#include "IStaticScrollingWrapper.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/
#define DISPLAY_LINE_SIZE 33 /* a kludge */
#define ABOUT_TEXT_MAX_SIZE          40
#define INVALID_PHONE_STATE_TEXT_BUF 50
#define POPUP_FORMAT_MAX_TEXT_LEN    48
#define POPUP_MAX_TEXT_LEN           (POPUP_FORMAT_MAX_TEXT_LEN + 12)
#define MPTY_CALL_TEXT_SIZE           1 

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

// Pop-up dialogs status
// ---------------------
#define SS_NO_POPUP             0
#define SS_1ST_POPUP_ACTIVE     1
#define SS_2ND_POPUP_ACTIVE     2
#define SS_GENERAL_POPUP_ACTIVE 3

typedef int SS_PopupStatus; /* like enum */


// Call Volume levels
// ------------------
#define SS_1ST_VOLUME 0
#define SS_2ND_VOLUME 1
#define SS_3RD_VOLUME 2
#define SS_4TH_VOLUME 3
#define SS_5TH_VOLUME 4
#define SS_6TH_VOLUME 5
#define SS_MAX_VOLUME (SS_6TH_VOLUME+1)

typedef int SS_CallVolume; /* like enum */


// forward declaration
struct _CSSOperationsDialogHandler;

// Event Handlers and Setup Handlers
// ---------------------------------
// The UI is implemented as a state machine. 
//
// Each state has an event handler -
// DIALOG_EVENT_HANDLER in UI.
//
// Each state may also have an setup function -
// DIALOG_SETUP_HANDLER in UI.

// dialog event handler function pointer type
typedef boolean (*DIALOG_EVENT_HANDLER)(
        struct _CSSOperationsDialogHandler *pSelf,
        AEEEvent                            eCode,
        uint16                              wParam,
        uint32                              dwParam
        );

// dialog setup function pointer type
typedef void (*DIALOG_SETUP_HANDLER)(
        struct _CSSOperationsDialogHandler *pSelf,
        boolean                             createNewDialog
        );

typedef struct _CSSOperationsDialogHandler
{

    // Global Resources
    // ----------------
    IDisplay             *m_pDisplay;     // Display interface
    IShell               *m_pShell;       // Shell interface
    SSCallDatabase       *m_pCallDatabase;// Call database
    ISSUtilCls           *m_pSSUtilCls;   // SSUtil 

    ISound               *m_pSound;       // Sound interface

    IAddrBook            *m_pAddressBook; // Address Book interface

    IPhone               *m_pPhone;       // Phone interface

    boolean               m_bSuspended;   // Is the applet suspended
    AEERect               m_Rect;         // Display rectangle
    int                   m_LineHeight;   // Height of each text line for current font


    // Current UI Dialog data
    // ----------------------
    uint16                m_DialogID;     // Current dialog resource Id
    IDialog              *m_pDialog;      // Current dialog 

    AEECallback           m_UIDialogTimerCB; // Timer callback

    DIALOG_EVENT_HANDLER  m_pDisplayDialogEventHandler; // displayed dialog event handler
    DIALOG_EVENT_HANDLER  m_pNonDisplayEventHandler;    // non displayed dialog event handler
    DIALOG_SETUP_HANDLER  m_pDialogSetupHandler;        // current dialog setup handler


    // Pop-up Dialogs data
    // ------------------
    SS_PopupStatus        m_PopupStatus;
    boolean               m_OverwritePopupDialog; // if TRUE then the active dialog pop-up should
                                                  // overwritten with new pop-up data,
                                                  // if FALSE then the active dialog pop-up
                                                  // should be replaced by another pop-up.

    AEECallback           m_PopupDialogTimerCB; // Timer callback

    // Pop-up non displayed dialog event handler
    DIALOG_EVENT_HANDLER  m_pPopupNonDisplayEventHandler;
    boolean               m_PopupGeneralDataValid;
    AECHAR                m_PopupGeneralText[ POPUP_MAX_TEXT_LEN+1 ];

    boolean               m_Popup1DataValid;
    AECHAR                m_Popup1Text[ POPUP_MAX_TEXT_LEN+1 ];
    SSCall_Index_type     m_Popup1CallIndex;

    boolean               m_Popup2DataValid;
    AECHAR                m_Popup2Text[ POPUP_MAX_TEXT_LEN+1 ];
    SSCall_Index_type     m_Popup2CallIndex;
    
    // General Call data
    SS_CallVolume         m_CallVolume; // volume for all calls
    boolean               m_CallMuted;  // mute flag for all calls


    // Data for Specific Dialogs
    // -------------------------

    // Volume Dialog data
    // Dialog's previous dialog setup handler
    DIALOG_SETUP_HANDLER  m_pVolumeDialogPrevSetupHandler;

    // About Dialog data
    // Dialog's previous dialog setup handler
    DIALOG_SETUP_HANDLER  m_pAboutDialogPrevSetupHandler;
    AECHAR                m_AboutTextBuf[ABOUT_TEXT_MAX_SIZE];

    // Invalid Phone State data
    AECHAR                m_InvalidPhoneStateTextBuf[INVALID_PHONE_STATE_TEXT_BUF]; 

    // Incoming Dialog Data
    // Dialog's previous dialog setup handler
    DIALOG_SETUP_HANDLER  m_pIncomingDialogPrevSetupHandler;
    SSCall_Index_type     m_IncomingDialogCallIndex;

    // Originating Second Call Dialog Data
    SSCall_Index_type     m_OriginatingSecondCallDialogCallIndex;

    // Ending Call Dialog Data
    // Dialog's previous dialog setup handler
    DIALOG_SETUP_HANDLER  m_pEndingCallDialogPrevSetupHandler;
    SSCall_Index_type     m_EndingCallDialogCallIndex;

    // Building MPTY Call Dialog Data
    SSCall_Index_type     m_BuildingMPTYCallDialogCallIndex;

    // MPTY Call Detail Dialog Data
    SSCall_Index_type     m_MPTYCallDetailsDialogCallIndex;

    // Call Detail Dialog data
    uint8                 m_NumberOfCalls; // number of existing calls (1 or 2)
    SSCall_Status_type    m_ForegroundCallStatus;
    boolean               m_IsForegroundCallMPTY;
    boolean               m_IsForegroundCallHeld;
    SSCall_Index_type     m_ForegroundCallIndex;

    SSCall_Status_type    m_BackgroundCallStatus;
    boolean               m_IsBackgroundCallMPTY;
    boolean               m_IsBackgroundCallHeld;
    SSCall_Index_type     m_BackgroundCallIndex;

    // Current number entered in the dialer dialog
    AECHAR   m_DialString[MAX_SIZE_DIALER_TEXT];

    // Call string used for a MPTY call 
    AECHAR   m_MPTYCallString[MPTY_CALL_TEXT_SIZE];

    // Pop-up Dialog data
    AECHAR  *m_pPopupText;    // Pointer to pop-up's text string
    IStatic *m_pPopupIStatic; // Pointer to pop-up dialog's ISTATIC control


    // Common data
    // -----------
    IStatic *m_pNumberCtl; // Pointer to ISTATIC control used by various dialogs
                           // to hold the call number.
                           // As only one dialog is active at a time this
                           // pointer is used by several dialogs
    IStaticScroller *m_pNumberScroller;

    IStatic *m_pNameCtl;   // Pointer to ISTATIC control used by various dialogs
                           // to hold the call name.
                           // As only one dialog is active at a time this
                           // pointer is used by several dialogs
    IStaticScroller *m_pNameScroller;

    AECHAR   m_wDisplayLineBuf[DISPLAY_LINE_SIZE]; 

} CSSOperationsDialogHandler;


/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandleEvent

DESCRIPTION: handle a BREW event by the dialog handler

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean SSOperationsDialogHandler_HandleEvent(
   CSSOperationsDialogHandler *pSelf,
   AEEEvent                    eCode,
   uint16                      wParam, 
   uint32                      dwParam
   );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_New

DESCRIPTION: Create a dialog handler instance

PARAMETERS:
   pIShell [in]: Pointer to the IShell instance
   pCallDatabase [in]: Pointer to a valid CallDatabase instance
   pSSUtilCls [in] : Pointer to a valid ISSUtilCls instance


RETURN VALUE:
   Pointer to CSSOperationsDialogHandler.

COMMENTS:
    in order to free CSSOperationsDialogHandler use 
    SSOperationsDialogHandler_Release().

    should only be called once by a client.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
CSSOperationsDialogHandler* SSOperationsDialogHandler_New(
    IShell         *pIShell,
    SSCallDatabase *pCallDatabase,
    ISSUtilCls     *pSSUtilCls
    );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_Initialize

DESCRIPTION: Initialize the dialog handler instance

PARAMETERS:
   pSelf [in]: Pointer to the dialog handler structure

RETURN VALUE:
   TRUE if initialization succeeded, FALSE otherwise.

=============================================================================*/
boolean SSOperationsDialogHandler_Initialize(
    CSSOperationsDialogHandler *pSelf
    );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_Release

DESCRIPTION: Release a dialog handler instance

PARAMETERS:
    pSelf [in]: Pointer to a valid dialog data instance

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SSOperationsDialogHandler_Release(
    CSSOperationsDialogHandler* pSelf
    );

#endif // end __SS_OPERATIONS_DIALOG_HANDLER_H__

