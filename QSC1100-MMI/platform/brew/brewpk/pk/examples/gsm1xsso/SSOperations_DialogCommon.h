#ifndef __SS_OPERATIONS_DIALOG_COMMON_H__
#define __SS_OPERATIONS_DIALOG_COMMON_H__

/*=============================================================================
FILE: SSOperations_DialogCommon.h

SERVICES: GSM1x SS Operations Dialog Common Definitions

GENERAL DESCRIPTION:
      This file defines common definitions and declarations for the SS Operations
      dialog handler functionality

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "AEE.h"                    // Standard AEE Declarations
#include "AEEShell.h"
#include "AEEMenu.h"
#include "AEEAddrBook.h"

#include "SSOperations_ClassId.h"   // Applet class ID
#include "gsm1xsso_res.h"	    // Applet Resources
#define  SSOPERATIONS_RES_FILE GSM1XSSO_RES_FILE

#include "SSOperations_DialogHandler.h"
#include "SSOperations_CommonEventDefinitions.h"
#include "SS_CommonMacros.h"

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/

#define TIMEOUT_MS_ONE_SECOND                  1000

//#define DIALER_PAUSE_AECHAR  ((AECHAR)'P')

/*
// See IS-95A Table 6.7.2.3.2.7-1. Recommended DTMF Pulse Width
typedef enum {
  SS_DTMF_ON_95  = 0x00,    // 95 ms recommended pulse width
  SS_DTMF_ON_150 = 0x01,    // 150 ms recommended pulse width
  SS_DTMF_ON_200 = 0x02,    // 200 ms recommended pulse width
  SS_DTMF_ON_250 = 0x03,    // 250 ms recommended pulse width
  SS_DTMF_ON_300 = 0x04,    // 300 ms recommended pulse width
  SS_DTMF_ON_350 = 0x05     // 350 ms recommended pulse width
} SS_DTMF_ON_type;


// See IS-95A Table 6.7.2.3.2.7-2 Recommended Minimum Interdigit Interval
typedef enum {
  SS_DTMF_OFF_60  = 0x00,  // 60 ms recommended minimum interdigit interval
  SS_DTMF_OFF_100 = 0x01,  // 100 ms recommended minimum interdigit interval
  SS_DTMF_OFF_150 = 0x02,  // 150 ms recommended minimum interdigit interval
  SS_DTMF_OFF_200 = 0x03   // 200 ms recommended minimum interdigit interval
} SS_DTMF_OFF_type;
*/

/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/

// Macro used to show a dialog
#define SHOW_DIALOG(dialogId) { SSOperationsDialog_ShowDialog(pSelf, dialogId); }

// Macro used to close the active dialog
#define CLOSE_ACTIVE_DIALOG { (void) ISHELL_EndDialog(pSelf->m_pShell); }

// Macro used to setup a new dialog and close the old dialog
#define SETUP_NEW_UI_DIALOG(setupHandler)                                  \
    {                                                                      \
        SSOperationsDialog_SetupNewDialog(pSelf, setupHandler);            \
    }                                                                      \

// Macro used to test if the incoming call info can be inspected/is available
#define IsIncomingCallInfoAvailable()                                       \
    SSCallDatabase_IsCallPresent(pSelf->m_pCallDatabase, pSelf->m_IncomingDialogCallIndex) 

// Macro used to get the incoming dialog call info
#define GetIncomingCallInfo()                                              \
    SSOperationsDialogHandler_GetCallInfo(pSelf, pSelf->m_IncomingDialogCallIndex) 

// Macro used to get the incoming dialog call info
#define GetIncomingCallTransactionInfo()                                   \
    SSOperationsDialogHandler_GetCallTInfo(pSelf, pSelf->m_IncomingDialogCallIndex)

/*==================== SET DATA MEMBER MACROS =====================*/

// Macro used to set the displayed dialog event handler
#define SET_DISPLAY_DIALOG_HANDLER(eventHandler)                           \
   {                                                                       \
      pSelf->m_pDisplayDialogEventHandler = eventHandler;                  \
   }                                                                       \

// Macro used to set the non dialog event, event handler
#define SET_NON_DISPLAY_EVENT_HANDLER(eventHandler)                        \
   {                                                                       \
      pSelf->m_pNonDisplayEventHandler = eventHandler;                     \
   }                                                                       \

// Macro used to store the current dialog setup handler as the volume dialog's
// previous setup handler
#define SET_VOLUME_PREV_DIALOG_SETUP_TO_CURRENT                            \
   {                                                                       \
      pSelf->m_pVolumeDialogPrevSetupHandler = pSelf->m_pDialogSetupHandler;\
   }                                                                       \

// Macro used to store the current dialog setup handler as the About dialog's
// previous setup handler
#define SET_ABOUT_PREV_DIALOG_SETUP_TO_CURRENT                             \
   {                                                                       \
      pSelf->m_pAboutDialogPrevSetupHandler = pSelf->m_pDialogSetupHandler;\
   }                                                                       \

// Macro used to store the current dialog setup handler as the incoming
// dialog's previous setup handler
#define SET_INCOMING_PREV_DIALOG_SETUP_TO_CURRENT                          \
  {                                                                        \
      pSelf->m_pIncomingDialogPrevSetupHandler = pSelf->m_pDialogSetupHandler;\
  }                                                                        \

// Macro used to store the current dialog setup handler as the ending call
// dialog's previous setup handler
#define SET_ENDING_CALL_PREV_DIALOG_SETUP_TO_CURRENT                       \
  {                                                                        \
      pSelf->m_pEndingCallDialogPrevSetupHandler = pSelf->m_pDialogSetupHandler;\
  }                                                                        \

/*==================== GET DATA MEMBER MACROS =====================*/

// Macro used to retrieve the Volume dialog's previous dialog setup handler
#define GET_VOLUME_PREV_DIALOG_SETUP (pSelf->m_pVolumeDialogPrevSetupHandler)

// Macro used to retrieve the Volume dialog's previous dialog setup handler
#define GET_ABOUT_PREV_DIALOG_SETUP (pSelf->m_pAboutDialogPrevSetupHandler)

// Macro used to retrieve the Incoming dialog's previous dialog setup handler
#define GET_INCOMING_PREV_DIALOG_SETUP (pSelf->m_pIncomingDialogPrevSetupHandler)

// Macro used to retrieve the Ending Call dialog's previous dialog setup handler
#define GET_ENDING_CALL_PREV_DIALOG_SETUP (pSelf->m_pEndingCallDialogPrevSetupHandler)

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: SSOperationsDialog_ShowDialog

DESCRIPTION: display a new dialog

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer           
    dialogId [in]: dialog resource id

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialog_ShowDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    uint16                              dialogId
    );

/*=============================================================================
FUNCTION: SSOperationsDialog_SetupNewDialog

DESCRIPTION: display a new dialog

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer           
    setupHandler [in]: dialog setup handler function

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialog_SetupNewDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    DIALOG_SETUP_HANDLER                setupHandler
    );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_GetCallInfo

DESCRIPTION: Get a call's call info structure, using the call index to find 
             the call.

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
   callIndex [in]: call index

RETURN VALUE:
   pointer to a valid SSCall_Info_type.

=============================================================================*/
SSCall_Info_type* SSOperationsDialogHandler_GetCallInfo(
    struct _CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type                   callIndex
    );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_GetCallTInfo

DESCRIPTION: Get a call's transaction info, using the call index to find 
             the call.

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    callIndex [in]: call index

RETURN VALUE:
   pointer to a valid SSCall_TInfo_type.

COMMENTS:
   call index must be that of a regular call and not of a MPTY call.

=============================================================================*/
SSCall_TInfo_type* SSOperationsDialogHandler_GetCallTInfo(
    struct _CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type                   callIndex
    );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_GetCallTInfoIndex

DESCRIPTION: Get a call's transaction info index, using the call index to find
             the call.

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    callIndex [in]: call index

RETURN VALUE:
   call transaction info index.

COMMENTS:
   call index must be that of a regular call and not of a MPTY call.

=============================================================================*/
SSCall_TInfo_Index_type SSOperationsDialogHandler_GetCallTInfoIndex(
    struct _CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type                   callIndex
    );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_GetCallStartTime

DESCRIPTION: Get a call's start time, using the call index to find 
             the call.

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    callIndex [in]: call index

RETURN VALUE:
   start time value.

=============================================================================*/
uint32 SSOperationsDialogHandler_GetCallStartTime(
    struct _CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type                   callIndex
    );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_GetCallString

DESCRIPTION: Get a call's call string, using the call index to find 
             the call.

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    callIndex [in]: call index

RETURN VALUE:
   null terminated call string

=============================================================================*/
AECHAR* SSOperationsDialogHandler_GetCallString(
    struct _CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type                   callIndex
    );


/*===========================================================================
FUNCTION SSOperationsDialog_MoveToTheIncomingDialog

DESCRIPTION
    This function leaves the current dialog and starts 
    the Incoming (call waiting) dialog.

PARAMETERS:
    *pSelf:    Pointer to the dialog handler object
    *eventData: Pointer to the incoming call's event data. 
                The function is responsible to free the pointer.

RETURN VALUE:
    None

COMMENTS:
    Implemented in SSOperations_DialogIncoming.c

===========================================================================*/
void SSOperationsDialog_MoveToTheIncomingDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    SSEventData                        *eventData
    );

/*=============================================================================
FUNCTION: SSOperationsDialog_ProcessCallNumberAndName

DESCRIPTION:  sets the call transaction info entry's call name using the
              contacts database, and sets the call phone number to according
              to its PI (presentation indication) data.

PARAMETERS:
   *pSelf      [in]: Pointer to the dialog handler object
   *pCallTInfo [in]: pointer to call transaction info entry

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialog_ProcessCallNumberAndName(
    struct _CSSOperationsDialogHandler *pSelf,
    SSCall_TInfo_type                  *pCallTInfo
    );

/*=============================================================================
FUNCTION: SSOperationsDialog_DoNumberLookup

DESCRIPTION:  Searches the Contacts database for a name match for the
              specified phone number.

PARAMETERS:
   *pAddrBook[in]: IAddrBook interface pointer
   *number   [in]: Phone number to lookup
   *name:   [out]: Destination buffer for the name
    size:    [in]: Size of name in bytes

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialog_DoNumberLookup(
    IAddrBook *pAddrBook,
    AECHAR    *number, 
    AECHAR    *name, 
    uint32     size
    );

/*=============================================================================
FUNCTION:  SSOperationsDialog_GetFirstLineOfStr
 
DESCRIPTION: Checks if the specified string will fit in a single line.  If
             the string is too long, a new string will be returned that
             does fit on one line.  

             The original string will not be modified, but the original string
             may (but not necessarily) be passed back as the return value from 
             this function.
 
PARAMETERS:
   *pSelf: Pointer to the dialog handler object
      fnt: Which font to use
    *wStr: string to check
 
RETURN VALUE:
    AECHAR const *: the portion of the string that fits on one line.

COMMENTS:
 
SIDE EFFECTS:
 
SEE ALSO:
 
=============================================================================*/
AECHAR const* SSOperationsDialog_GetFirstLineOfStr(
    struct _CSSOperationsDialogHandler *pSelf,
    AEEFont                             fnt,
    AECHAR                             *wStr
    );

/*=============================================================================
FUNCTION: SSOperationsDialog_ChangeCallVolume

DESCRIPTION: Increase or decreases the current call volume

PARAMETERS:
   *pSelf: Pointer to the dialog handler object
    louder: TRUE to increase the call volume
            FALSE to decrease the call volume

RETURN VALUE:
    None

COMMENTS:
    Implemented in SSOperations_DialogVolume.c

=============================================================================*/
void SSOperationsDialog_ChangeCallVolume(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             louder
    );

/*===========================================================================

FUNCTION: SSOperations_SetDefaultSoftkeyLook

DESCRIPTION: Sets a default softkey menu look 

PARAMETERS:
    *pIShell      [in]: Pointer to an IShell object
    *pSoftKey [in/out]: Pointer to an softkey menu object

RETURN VALUE:
   None

===========================================================================*/
void SSOperations_SetDefaultSoftkeyLook(
    IShell   *pIShell, 
    IMenuCtl *pSoftKey
    );

/*===========================================================================
FUNCTION SSOperationsDialog_CreateTransactionWiseEventData

DESCRIPTION
   Creates a transaction wise event data and initialize it.

PARAMETERS:
    *pSelf [in]: Pointer to the dialog handler object
     callIndex [in]: call index
    *pMsg [in]: Pointer to a message structure. can be NULL.

RETURN VALUE:
    Pointer to a dynamically allocated event data or NULL if a failure occured.
    event data's memory is the responsibility of the caller.

COMMENTS:
    Implemented in SSOperations_DialogIncoming.c

===========================================================================*/
SSEventData* SSOperationsDialog_CreateTransactionWiseEventData(
    struct _CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type                   callIndex,
    SSMsg_int_msg_type                 *pMsg
    );

/*=============================================================================
FUNCTION: SSOperationsDialog_AVKType2ASCII

DESCRIPTION:
   Converts a key code (AVK_0..AVK_9,AVK_STAR,AVK_POUND) to the
   ASCII equivalent

PARAMETERS:
   key: BREW keycode

RETURN VALUE:
   char: ASCII value

COMMENTS:
   Returns the NULL character if the input value is out of range

=============================================================================*/
char SSOperationsDialog_AVKType2ASCII(AVKType key);

/*=============================================================================
FUNCTION: SSOperationsDialog_HandleDialogTimeout

DESCRIPTION: Generic timer used by several dialogs. 
             When this callback is invoked, it sends a EVT_USER_DIALOG_TIMEOUT 
             event to the applet.

PARAMETERS:
   *pUser: Pointer to the dialog handler object

RETURN VALUE:
    None

=============================================================================*/
void SSOperationsDialog_HandleDialogTimeout(void *pUser);

#ifdef AEE_SIMULATOR
/*=============================================================================
FUNCTION: SSOperations_TriggerCCEvent

DESCRIPTION: Testing Function used to send CC MNCC_DATA_INDICATION events 
             that simulate the reception of network messages

PARAMETERS:
   *pIShell [in]: Pointer to an IShell object
	msgType [in]: Message type to be created,
	tiFlag  [in]: TI flag (TRUE for 1 (MO), FALSE for 0 (MT))
	tiValue [in]: TI value (0-6)

RETURN VALUE:
    None

=============================================================================*/
void SSOperations_TriggerCCEvent(
    struct _CSSOperationsDialogHandler *pSelf,
	SSMsg_msg_type_type                 msgType,
	boolean                             tiFlag,
	uint8                               tiValue
	);
#endif

#ifdef AEE_SIMULATOR
/*=============================================================================
FUNCTION: SSOperations_TriggerCCEventWithMsg

DESCRIPTION: Testing Function used to send CC MNCC_DATA_INDICATION events 
             that simulate the reception of network messages

PARAMETERS:
   *pSelf   [in]: CSSOperationsDialogHandler object pointer
	pMsg    [in]: Pointer to a Message to be sent. takes ownership of the 
                  message's memory.                  
	tiFlag  [in]: TI flag (TRUE for 1 (MO), FALSE for 0 (MT))
	tiValue [in]: TI value (0-6)

RETURN VALUE:
    None

=============================================================================*/
void SSOperations_TriggerCCEventWithMsg(
    struct _CSSOperationsDialogHandler *pSelf,
	SSMsg_int_msg_type                 *pMsg,
	boolean                             tiFlag,
	uint8                               tiValue
	);
#endif

/*===========================================================================
                              Dialog Setup methods
===========================================================================*/

/*===========================================================================
FUNCTION SSOperationsDialog_SetupCallDetailsDialog

DESCRIPTION
    This function creates the CallDetails dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

COMMENTS:
    Implemented in SSOperations_DialogCallDetails.c

===========================================================================*/
void SSOperationsDialog_SetupCallDetailsDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
    );

/*===========================================================================
FUNCTION SSOperationsDialog_SetupCallVolumeDialog

DESCRIPTION
    This function creates the Call Volume dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

COMMENTS:
    Implemented in SSOperations_DialogVolume.c

===========================================================================*/
void SSOperationsDialog_SetupCallVolumeDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
    );

/*===========================================================================
FUNCTION SSOperationsDialog_SetupAboutDialog

DESCRIPTION
    This function creates the About dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

COMMENTS:
    Implemented in SSOperations_DialogAbout.c

===========================================================================*/
void SSOperationsDialog_SetupAboutDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
    );

/*===========================================================================
FUNCTION SSOperationsDialog_SetupAboutDialogWhenInvalidPhoneState

DESCRIPTION
    This function creates the About dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

COMMENTS:
    Implemented in SSOperations_DialogAbout.c

===========================================================================*/
void SSOperationsDialog_SetupAboutDialogWhenInvalidPhoneState(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
    );

/*===========================================================================
FUNCTION SSOperationsDialog_SetupDialerDialog

DESCRIPTION
    This function creates theDialer dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

COMMENTS:
    Implemented in SSOperations_DialogDialer.c

===========================================================================*/
void SSOperationsDialog_SetupDialerDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
    );

/*===========================================================================
FUNCTION SSOperationsDialog_SetupIncomingDialog

DESCRIPTION
    This function creates the Incoming (call waiting) dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

COMMENTS:
    Implemented in SSOperations_DialogIncoming.c

===========================================================================*/
void SSOperationsDialog_SetupIncomingDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
    );

/*===========================================================================
FUNCTION SSOperationsDialog_SetupPopupDialog

DESCRIPTION
    This function creates the Pop-up dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object

COMMENTS:
    Implemented in SSOperations_DialogPopup.c

===========================================================================*/
void SSOperationsDialog_SetupPopupDialog(
    struct _CSSOperationsDialogHandler *pSelf
    );

/*===========================================================================
FUNCTION SSOperationsDialog_SetupSwappingCallsDialog

DESCRIPTION
    This function creates the Swapping Calls dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

COMMENTS:
    Implemented in SSOperations_DialogSwappingCalls.c

===========================================================================*/
void SSOperationsDialog_SetupSwappingCallsDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
    );

/*===========================================================================
FUNCTION SSOperationsDialog_SetupBuildingMPTYDialog

DESCRIPTION
    This function creates the Building MPTY dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

COMMENTS:
    Implemented in SSOperations_DialogBuildingMPTY.c

===========================================================================*/
void SSOperationsDialog_SetupBuildingMPTYDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
    );

/*===========================================================================
FUNCTION SSOperationsDialog_SetupMPTYCallDetailsDialog

DESCRIPTION
    This function creates the MPTY Call Details dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

COMMENTS:
    Implemented in SSOperations_DialogMPTYCallDetails.c

===========================================================================*/
void SSOperationsDialog_SetupMPTYCallDetailsDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
    );

/*===========================================================================
FUNCTION SSOperationsDialog_SetupEndingCallDialog

DESCRIPTION
    This function creates the Ending Call dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

COMMENTS:
    Implemented in SSOperations_DialogEndingCall.c

===========================================================================*/
void SSOperationsDialog_SetupEndingCallDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
    );

/*===========================================================================
FUNCTION SSOperationsDialog_SetupConnectingIncomingCallDialog

DESCRIPTION
    This function creates the Connecting Incoming Call dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

COMMENTS:
    Implemented in SSOperations_DialogConnectingIncomingCall.c

===========================================================================*/
void SSOperationsDialog_SetupConnectingIncomingCallDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
    );

/*===========================================================================
FUNCTION SSOperationsDialog_SetupOriginatingSecondCallDialog

DESCRIPTION
    This function creates the Originating Second Call dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

COMMENTS:
    Implemented in SSOperations_DialogOriginatingSecondCall.c

===========================================================================*/
void SSOperationsDialog_SetupOriginatingSecondCallDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
    );

/*===========================================================================
FUNCTION SSOperationsDialog_SetupEntryMenuDialog

DESCRIPTION
    This function creates the Entry Menu dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

COMMENTS:
    Implemented in SSOperations_DialogEntryMenu.c

===========================================================================*/
void SSOperationsDialog_SetupEntryMenuDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
    );

/*===========================================================================
FUNCTION SSOperationsDialog_SetupInvalidPhoneStateDialog

DESCRIPTION
    This function creates the Invalid Phone State dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

COMMENTS:
    Implemented in SSOperations_DialogInvalidPhoneState.c

===========================================================================*/
void SSOperationsDialog_SetupInvalidPhoneStateDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
   );

/*===========================================================================

FUNCTION: SSOperationsDialog_SetupWaitForIncomingCallDialog()

DESCRIPTION:
    This function creates a Wait For Incoming Call dialog.

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_SetupWaitForIncomingCallDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    boolean                             createNewDialog
    );

#endif // end __SS_OPERATIONS_DIALOG_COMMON_H__