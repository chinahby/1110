#ifndef GSM1XSMSDIALOGHANDLER_H
#define GSM1XSMSDIALOGHANDLER_H

/*=============================================================================
FILE: SMSDialogHandler.h

SERVICES: SMS Dialog Handlers

GENERAL DESCRIPTION:
      This file defines all SMS dialog handlers which are used
      by the SMSApp to bring up various SMS screens to the user

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2002 QUALCOMM Incorporated. All Rights Reserved.
    QUALCOMM Proprietary
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES FOR MODULE
===========================================================================*/

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/
struct SMSApp;

// Enum to define the possible operations that we can perform on messages
// stored in the outbox.
typedef enum SMSOutboxOp
{
   SMSOUTBOXOP_DONT_SAVE, // Don't save the message. Just send it.
   SMSOUTBOXOP_SAVE       // Save the message in outbox
} SMSOutboxOp;

// Macro used at lot of places to close the dialog (after setting the
// dialog result).
#define CLOSE_DIALOG(dlgResult)  {                                           \
                                    pMe->m_dlgResult = dlgResult;            \
                                    (void) ISHELL_EndDialog(pMe->a.m_pIShell);  \
                                 }

/*===========================================================================
                              TYPE DECLARATIONs
===========================================================================*/

typedef boolean (*DIALOG_EVENTHANDLER)(struct  SMSApp *pMe,
                                       AEEEvent       eCode,
                                       uint16         wParam,
                                       uint32         dwParam);

// List of values that a dialog may return
typedef enum DLGRetValue
{
   // Initial value indicates that the dialog needs to be created.
   DLGRET_CREATE,         
   DLGRET_CANCELED,           // General dialog response
   DLGRET_OK,                 // General dialog response
   DLGRET_MSGBOX_OK,          // Message box response
   DLGRET_NO,                 // Returned by YESNO dialog
   DLGRET_YES,                // Returned by YESNO dialog

   // DLG_MAIN specific return values
   DLGRET_MAIN_VM,
   DLGRET_MAIN_INBOX,
   DLGRET_MAIN_CMSG,
   DLGRET_MAIN_SENDPREFORMAT,
   DLGRET_MAIN_OUTBOX,
   DLGRET_MAIN_SIM,
   DLGRET_MAIN_ERASE,
   DLGRET_MAIN_SETTINGS,
   DLGRET_MAIN_SEND,
   DLGRET_MAIN_TEST,
   DLGRET_MAIN_ABOUT,

   // DLG Inbox specific return values
   DLGRET_INBOX_SEND,
   DLGRET_CALL,

   DLGRET_SAVE_TO_SIM,
   
   DLGRET_REPLY,
   DLGRET_IN_NEXT,
   DLGRET_IN_ERASE,
   DLGRET_IN_FORWARD,
   DLGRET_IN_SENDER,
   DLGRET_IN_MOVESIM,
   DLGRET_IN_REPLY,
   DLGRET_END,

   // DLG Outbox specific return values
   DLGRET_OUT_SEND,
   DLGRET_OUT_FORWARD,
   DLGRET_OUT_ERASE,
   DLGRET_OUT_DONE,
   DLGRET_OUT_MOVESIM,

   // DLG Message Arrived specific return values
   DLGRET_MA_IGNORE,
   DLGRET_MA_VIEW,
   DLGRET_MA_SEND,
   DLGRET_MA_SAVE,

   // DLG Erase message specific return values
   DLGRET_EM_SIM,
   DLGRET_EM_OUTBOX,
   DLGRET_EM_INBOX,
   DLGRET_EM_NO,
   DLGRET_ERASE_OLD_MSGS,

   // DLG Message settings screen specific dialog return values.
   DLGRET_MS_ALERT,
   DLGRET_MS_VMNUM,
   DLGRET_MS_VMNUM_OK,
   DLGRET_MS_VMNUM_EDIT,
   DLGRET_MS_VMNUM_EDIT_OK,
   DLGRET_MS_EDIT_SC_ADDR,

   // DLG voice mail box screen specific dialog return values
   DLGRET_VOICEMAIL_SEND,
   DLGRET_VOICEMAIL_RESET,
   DLGRET_VOICEMAIL_CALL,

   // DLG SIM msg box screen specific dialog return values
   DLGRET_SIM_NEXT,
   DLGRET_SIM_ERASE,
   DLGRET_SIM_SENDER,
   DLGRET_SIM_MOVEIN,
   DLGRET_SIM_FORWARD,
   DLGRET_SIM_REPLY,

   // DLG Create message screen specific dialog return values
   DLGRET_CM_CONTINUE,
   DLGRET_CM_CONTACTS,
   DLGRET_CM_SEND,
   DLGRET_CM_SAVE,

   // DLG Edit SC Address screen specific dialog return values
   DLGRET_EDIT_SC_ADDR_CONTINUE,

   // TODO Remove these?
   // DLG Test screen specific dialog return values
   DLGRET_TEST_CLASS0,
   DLGRET_TEST_CLASS1,
   DLGRET_TEST_CLASS2,
   DLGRET_TEST_CLASS_NONE,
   DLGRET_TEST_VM_PID,
   DLGRET_TEST_VM_DCS_STORE,
   DLGRET_TEST_VM_DCS_DISCARD,
   DLGRET_TEST_VM_UDH
} DLGRetValue;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: GSM1xSMSDialogHandler_UpdateActiveDialogInfo

DESCRIPTION: Updates the key dialog parameters in the SMSApp object.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   wParam, dwParam [in]: Active dialog information

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.
   
COMMENTS:
   The updated SMS applet dialog attributes are later used to route the 
   dialog events to thier respective handlers.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSDialogHandler_UpdateActiveDialogInfo
(
   struct SMSApp *pMe,
   uint16        wParam,
   uint32        dwParam
);

/*=============================================================================
FUNCTION: GSM1xSMSDialogHandler_ShowDialog

DESCRIPTION:
   This function is used by various state handlers to bring up one of the many:)
   SMS dialogs. The calling function passes the dialog resource ID as a 
   parameter to this function and this function creates the dialog.

PARAMETERS:
   *pMe: Pointer to the SMS Applet structure.
   dlgResId: Dialog Id.

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSDialogHandler_ShowDialog
(
   struct SMSApp *pMe,
   uint16        dlgResId
);

/*=============================================================================
FUNCTION: GSM1xSMSDialogHandler_ShowMsgBox

DESCRIPTION:
   This function is used by various state handlers to bring up a dialog with 
   just a static control in it. The text (resource ID) to be put in the static
   control is also passed as a parameter.

PARAMETERS:
   *pMe: Pointer to the SMS Applet structure
   msgTextResId:  Message text resource ID to be shown in the static dialog.

RETURN VALUE:
    None

COMMENTS:                   

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSDialogHandler_ShowMsgBox
(
   struct SMSApp *pMe,
   uint16        msgTextResId
);

/*=============================================================================
FUNCTION: GSM1xSMSDialogHandler_ShowOKBox

DESCRIPTION:
   This function is used by various state handlers to bring up a dialog with 
   a static control and a OK softkey in it. The text (resource ID) to be put in 
   the static control is also passed as a parameter.

PARAMETERS:
   *pMe: Pointer to the SMS Applet structure
   msgTextResId:  Message text resource ID to be shown in the static dialog.

RETURN VALUE:
    None

COMMENTS:                   

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSDialogHandler_ShowOKBox
(
   struct SMSApp *pMe,
   uint16        msgTextResId
);

/*=============================================================================
FUNCTION: GSM1xSMSDialogHandler_RouteDialogEvent

DESCRIPTION: Route a BREW event to the active dialog handler

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
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
boolean GSM1xSMSDialogHandler_RouteDialogEvent
(
   struct SMSApp *pMe,
   AEEEvent      eCode,
   uint16        wParam,
   uint32        dwParam
);

#endif // end GSM1XSMSDIALOGHANDLER_H

