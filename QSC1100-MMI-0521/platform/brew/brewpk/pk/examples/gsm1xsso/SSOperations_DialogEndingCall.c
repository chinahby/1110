/*===========================================================================
FILE: SSOperations_DialogEndingCall.c

SERVICES: SS Operations dialog, displayed while Ending a Call

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
FUNCTION: SSOperationsDialog_EndingCallHandleEvent

DESCRIPTION: Ending Call dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_EndingCallHandleEvent(
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
                    IDDC_ENDING_CALL_TEXT
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
                       pSelf, SSMSG_MSG_TYPE_RELEASE_COMPLETE, FALSE, 
                       pSelf->m_EndingCallDialogCallIndex
                       );
                    break;

                case AVK_1:
                    SSOperations_TriggerCCEvent(
                       pSelf, SSMSG_MSG_TYPE_RELEASE_COMPLETE, FALSE, 
                       (int8)(1 - pSelf->m_EndingCallDialogCallIndex)
                       );
                    break;
            }
#endif //U_TEST && AEE_SIMULATOR
            return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_EndingCallHandleNonDisplayEvent

DESCRIPTION: Ending Call dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_EndingCallHandleNonDisplayEvent(
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
            // This case should never happen until Held MPTY is supported!
            assert(!"SETUP_IND in EndingCall is not supported");

            // return FALSE because event data is not freed!
            // default end of event processing code for non handled event 
            // should free this event.
            return FALSE; 
        }

        case UIMN_REL_CNF:
        case UIMN_REL_IND:
        {
            SSEventData *pEventData = (SSEventData*) dwParam;
            assert(pEventData != NULL);
            assert(pEventData->commonData.type == 
                   SS_EVENT_DATA_TRANSACTIONWISE_MSG);

            if (pEventData->transactionwiseMsgData.callIndex ==
                pSelf->m_EndingCallDialogCallIndex)
            {        
                SSOperationsDialogHandler_HandleEndingCallPopupEvent(
                   pSelf,
                   eCode,
                   wParam,
                   dwParam
                   );

                SETUP_NEW_UI_DIALOG( GET_ENDING_CALL_PREV_DIALOG_SETUP );
                return TRUE;
            }
        }
        break;
    }

    return FALSE;
}

/*===========================================================================

FUNCTION: SSOperationsDialog_SetupEndingCallDialog()

DESCRIPTION:
    This function creates an Ending Call dialog.

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_SetupEndingCallDialog(
    CSSOperationsDialogHandler *pSelf,
    boolean                     createNewDialog
    )
{
    assert(GET_ENDING_CALL_PREV_DIALOG_SETUP != NULL);

    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER( 
            SSOperationsDialog_EndingCallHandleEvent 
            );    
        SHOW_DIALOG( IDD_ENDING_CALL );
    }

    SET_NON_DISPLAY_EVENT_HANDLER( 
        SSOperationsDialog_EndingCallHandleNonDisplayEvent 
        );
}