/*===========================================================================
FILE: SSOperations_DialogConnectingIncomingCall.c

SERVICES: 
    SS Operations dialog, displayed while Connecting an Incoming Call.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                               INCLUDE FILES
=============================================================================== */
#include "AEEShell.h"         // AEE Shell Services
#include "AEEStdLib.h"        // AEE StdLib Services

#include "AutomationLogging.h"

#include "SSOperations_DialogCommon.h"
#include "SSOperations_DialogHandler.h"
#include "SSOperations_DialogPopupHandler.h"
#include "SSOperations_UserEvents.h"

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
FUNCTION: SSOperationsDialog_ConnectingIncomingCallHandleEvent

DESCRIPTION: Connecting Incoming Call dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_ConnectingIncomingCallHandleEvent(
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
                    IDDC_CONNECTING_INCOMING_CALL_TEXT
                    );
            ISTATIC_SetProperties(pStatic, ST_CENTERTEXT | ST_MIDDLETEXT);
            ISTATIC_SetFont(pStatic, AEE_FONT_BOLD, AEE_FONT_NORMAL);
            return TRUE;
        }

        case EVT_DIALOG_START:
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:

#if (defined U_TEST && defined AEE_SIMULATOR)
        // for unit testing
        // ----------------
        switch ((AVKType)wParam) 
        {
            case AVK_0:
                SSOperations_TriggerCCEvent(
                    pSelf, SSMSG_MSG_TYPE_HOLD_ACKNOWLEDGE, FALSE, 0
                    );
				break;

            case AVK_1:
                SSOperations_TriggerCCEvent(
                    pSelf, SSMSG_MSG_TYPE_CONNECT_ACKNOWLEDGE, FALSE, 1
                    );
				break;

            case AVK_2:
                SSOperations_TriggerCCEvent(
                    pSelf, SSMSG_MSG_TYPE_RELEASE, FALSE, 0
                    );
				break;

			case AVK_3:
				SSOperations_TriggerCCEvent(
                    pSelf, SSMSG_MSG_TYPE_DISCONNECT, FALSE, 1
                    );
				break;

			case AVK_4:
				SSOperations_TriggerCCEvent(
                    pSelf, SSMSG_MSG_TYPE_RELEASE, FALSE, 1
                    );
				break;

			case AVK_5:
				SSOperations_TriggerCCEvent(
                    pSelf, SSMSG_MSG_TYPE_RELEASE_COMPLETE, FALSE, 1
                    );
				break;

			case AVK_6:
				SSOperations_TriggerCCEvent(
                    pSelf, SSMSG_MSG_TYPE_HOLD_ACKNOWLEDGE, FALSE, 0
                    );
				break;

			case AVK_7:
				SSOperations_TriggerCCEvent(
                    pSelf, SSMSG_MSG_TYPE_HOLD_REJECT, FALSE, 0
                    );
				break;
        }
#endif //U_TEST && AEE_SIMULATOR
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_ConnectingIncomingCallHandleNonDisplayEvent

DESCRIPTION: Connecting Incoming Call dialog non display event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_ConnectingIncomingCallHandleNonDisplayEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    switch (eCode)
    {
        case UIMN_ANSWER_AND_HOLD_CNF:
        case UIMN_ANSWER_AND_RELEASE_CNF:
        {
            SSOperationsDialogHandler_HandleConnectingIncomingCallPopupEvent(
                   pSelf,
                   eCode,
                   wParam,
                   dwParam
                   );
            SETUP_NEW_UI_DIALOG( GET_INCOMING_PREV_DIALOG_SETUP );            
            return TRUE;
        }
        break;

        case UIMN_SETUP_COMPLETE_IND:
        {
            SSEventData *pEventData = (SSEventData*)dwParam;
            assert(pEventData != NULL);
            assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

            if (pEventData->callwiseMsgData.callIndex1 ==
                pSelf->m_IncomingDialogCallIndex)
            {
                SSOperationsDialogHandler_HandleConnectingIncomingCallPopupEvent(
                   pSelf,
                   eCode,
                   wParam,
                   dwParam
                   );
                SETUP_NEW_UI_DIALOG( GET_INCOMING_PREV_DIALOG_SETUP );
                return TRUE;
            }
        }
        break;

        case UIMN_ERR_IND:            
        {
            SSEventData *pEventData = (SSEventData*)dwParam;
            assert(pEventData != NULL);
            assert(pEventData->commonData.type == SS_EVENT_DATA_ERROR);

            // only handle two call error events
            if (pEventData->errorData.isCallIndex2Valid == FALSE) return FALSE;

            SSOperationsDialogHandler_HandleConnectingIncomingCallPopupEvent(
               pSelf,
               eCode,
               wParam,
               dwParam
               );

            pEventData = NULL; // released by the pop-up handler.

            if (IsIncomingCallInfoAvailable() == FALSE)
            {
                SETUP_NEW_UI_DIALOG( GET_INCOMING_PREV_DIALOG_SETUP );
            }
            else
            {
                // Incoming call still exist, and it can not be a MPTY call yet,
                // so we just get its transaction info entry and check the status
                SSCall_TInfo_type *pCallTInfo =
                    SSOperationsDialogHandler_GetCallTInfo(
                        pSelf,
                        pSelf->m_IncomingDialogCallIndex
                        );

                if (pCallTInfo->callStatus == SS_CALL_STATUS_ACTIVE)
                {
                    // incoming call is now active - nothing left to do in the 
                    // incoming dialog
                    SETUP_NEW_UI_DIALOG( GET_INCOMING_PREV_DIALOG_SETUP );
                }
                else
                {
                    // incoming call still exists and it is not active - 
                    // show again the incoming dialog so that the user can try again
                    // to answer it
                    SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupIncomingDialog );
                }
            }
            return TRUE;
        }
        break;

        case UIMN_REL_IND:
        case UIMN_REL_CNF:
        {
            SSEventData *pEventData = (SSEventData*)dwParam;
            assert(pEventData != NULL);
            assert(pEventData->commonData.type == SS_EVENT_DATA_TRANSACTIONWISE_MSG);

            if (pSelf->m_IncomingDialogCallIndex == 
                pEventData->transactionwiseMsgData.callIndex)
            {
                SSOperationsDialogHandler_HandleConnectingIncomingCallPopupEvent(
                   pSelf,
                   eCode,
                   wParam,
                   dwParam
                   );
                SETUP_NEW_UI_DIALOG( GET_INCOMING_PREV_DIALOG_SETUP );
                return TRUE;
            }
        }
        break;
    }

    return FALSE;
}

/*===========================================================================

FUNCTION: SSOperationsDialog_SetupConnectingIncomingCallDialog()

DESCRIPTION:
    This function creates a Connecting Incoming Call dialog.

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: boolean value that specifies whether to setup a new dialog display

RETURN VALUE:
   None

===========================================================================*/
void SSOperationsDialog_SetupConnectingIncomingCallDialog(
    CSSOperationsDialogHandler *pSelf,
    boolean                     createNewDialog
    )
{
    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER( 
            SSOperationsDialog_ConnectingIncomingCallHandleEvent 
            );
        SHOW_DIALOG( IDD_CONNECTING_INCOMING_CALL );
    }

    SET_NON_DISPLAY_EVENT_HANDLER( 
        SSOperationsDialog_ConnectingIncomingCallHandleNonDisplayEvent 
        );
}
