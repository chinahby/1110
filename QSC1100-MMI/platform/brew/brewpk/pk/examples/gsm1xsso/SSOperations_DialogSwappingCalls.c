/*===========================================================================
FILE: SSOperations_DialogSwappingCalls.c

SERVICES:  SS Operations dialog, displayed while swapping calls

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
FUNCTION: SSOperationsDialog_SwappingCallsHandleEvent

DESCRIPTION: Swapping Calls dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_SwappingCallsHandleEvent(
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
                    IDDC_SWAPPING_CALLS_TEXT
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
                       pSelf, SSMSG_MSG_TYPE_HOLD_ACKNOWLEDGE, FALSE,
                       pSelf->m_ForegroundCallIndex
                       );
                    break;

                case AVK_1:
                    SSOperations_TriggerCCEvent(
                       pSelf, SSMSG_MSG_TYPE_RETRIEVE_ACKNOWLEDGE, FALSE,
                       pSelf->m_BackgroundCallIndex
                       );
                    break;

                case AVK_2:
                    SSOperations_TriggerCCEvent(
                       pSelf, SSMSG_MSG_TYPE_HOLD_REJECT, FALSE, 0
                       );
                    break;

                case AVK_3:
                    SSOperations_TriggerCCEvent(
                       pSelf, SSMSG_MSG_TYPE_RETRIEVE_REJECT, FALSE, 1
                       );
                    break;

                case AVK_4:
                    SSOperations_TriggerCCEvent(
                       pSelf, SSMSG_MSG_TYPE_DISCONNECT, FALSE, 0
                       );
                    break;
 
                case AVK_5:
                    SSOperations_TriggerCCEvent(
                       pSelf, SSMSG_MSG_TYPE_RELEASE_COMPLETE, FALSE, 0
                       );
                    break;
            }
#endif //U_TEST && AEE_SIMULATOR
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_SwappingCallsHandleNonDisplayEvent

DESCRIPTION: Swapping Calls dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_SwappingCallsHandleNonDisplayEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    switch (eCode)
    {
        case UIMN_ERR_IND:            
        case UIMN_TOGGLE_CNF:
        case UIMN_TOGGLE_REJ_IND:
        {
            if (eCode == UIMN_ERR_IND)
            {
                SSEventData *pEventData = (SSEventData*)dwParam;
                assert(pEventData != NULL);
                assert(pEventData->commonData.type == SS_EVENT_DATA_ERROR);

                // only handle two call error event
                if (pEventData->errorData.isCallIndex2Valid == FALSE) return FALSE;                
            }

            SSOperationsDialogHandler_HandleSwappingCallsPopupEvent(
                pSelf, 
                eCode, 
                wParam, 
                dwParam
                );

            SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupCallDetailsDialog );
            return TRUE;
        }
    }

    return FALSE;
}

/*===========================================================================

FUNCTION: SSOperationsDialog_SetupSwappingCallsDialog()

DESCRIPTION:
    This function creates a Swapping Calls dialog.

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_SetupSwappingCallsDialog(
    CSSOperationsDialogHandler *pSelf,
    boolean                     createNewDialog
    )
{
    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER( 
            SSOperationsDialog_SwappingCallsHandleEvent 
            );

        SHOW_DIALOG( IDD_SWAPPING_CALLS );
    }

    SET_NON_DISPLAY_EVENT_HANDLER( 
        SSOperationsDialog_SwappingCallsHandleNonDisplayEvent 
        );
}