/*=========================================================================
FILE: SSOperations_DialogWaitForIncomingCallCall.c

SERVICES: SS Operations Wait For Incoming Call

GENERAL DESCRIPTION:
    Present the dialog that appears on when SSO starts due to an incoming call.

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================== */
#include "AEEShell.h"         // AEE Shell Services
#include "AEEStdLib.h"        // AEE StdLib Services

#include "AutomationLogging.h"
#include "SSOperations_DialogCommon.h"

#include "SSOperations_UserEvents.h"
#include "SSOperations_CommonMessageDefinitions.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              MACRO DEFINITIONS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

/*=============================================================================
FUNCTION: SSOperationsDialog_WaitForIncomingCallHandleEvent

DESCRIPTION: Wait For Incoming Call dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_WaitForIncomingCallHandleEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
            IStatic *pStatic = (IStatic*)
                IDIALOG_GetControl(
                    pSelf->m_pDialog,
                    IDDC_WAIT_FOR_INCOMING_CALL_TEXT
                    );
            ISTATIC_SetProperties(pStatic, ST_CENTERTEXT | ST_MIDDLETEXT);
            ISTATIC_SetFont(pStatic, AEE_FONT_BOLD, AEE_FONT_NORMAL);

            AUTOMATION_LOG("%s","Wait For Incoming Call");
            return TRUE;
        }

        case EVT_DIALOG_START:
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
        {
            switch ((AVKType)wParam) 
            {
                case AVK_CLR:
                {
                   return TRUE; // ignore key
                }
   
#if (defined U_TEST && defined AEE_SIMULATOR)
                // for unit testing:
                case AVK_0:
                {
                    SSMsg_int_msg_type* pMsg = 
                      SSOMessage_AllocateMsg(SSMSG_MSG_TYPE_SETUP);
                    
                    pMsg->setup.signal.hdr.present = TRUE;
                    pMsg->setup.signal.signal_value = 
                        SSMSG_SIGNAL_VALUE_CALL_WAITING_TONE_ON;
                    pMsg->setup.calling_party_bcd_number.hdr.present = TRUE;
                    pMsg->setup.calling_party_bcd_number.pi = SSMSG_PI_PRESENTATION_ALLOWED;

                    MEMCPY(pMsg->setup.calling_party_bcd_number.number, "048506506", 9);                    
                    pMsg->setup.calling_party_bcd_number.number_len = 9;

                    SSOperations_TriggerCCEventWithMsg(
                        pSelf, pMsg, FALSE, 1
                        );
                }
                break;
#endif //U_TEST && AEE_SIMULATOR
            }
            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_WaitForIncomingCallHandleNonDisplayEvent

DESCRIPTION: Wait For Incoming Call dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_WaitForIncomingCallHandleNonDisplayEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    switch (eCode)
    {
        case UIMN_SETUP_IND:
        {
            // change the current dialog setup handler to be Call Details
            // so that the incoming dialog will return to it and not to the 
            // Wait For Incoming Call
            pSelf->m_pDialogSetupHandler = 
                SSOperationsDialog_SetupCallDetailsDialog;

            SSOperationsDialog_MoveToTheIncomingDialog(
                pSelf, 
                (SSEventData*)dwParam
                );
            return TRUE;
        }
    }

    return FALSE;
}

/*===========================================================================

FUNCTION: SSOperationsDialog_SetupWaitForIncomingCallDialog()

DESCRIPTION:
    This function creates a Wait For Incoming Call dialog.

PARAMETERS:
    *pSelf:Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_SetupWaitForIncomingCallDialog(
    CSSOperationsDialogHandler *pSelf,
    boolean                     createNewDialog
    )
{
    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER( 
            SSOperationsDialog_WaitForIncomingCallHandleEvent 
            );
        SHOW_DIALOG( IDD_WAIT_FOR_INCOMING_CALL );
    }

    SET_NON_DISPLAY_EVENT_HANDLER( 
        SSOperationsDialog_WaitForIncomingCallHandleNonDisplayEvent 
        );
}