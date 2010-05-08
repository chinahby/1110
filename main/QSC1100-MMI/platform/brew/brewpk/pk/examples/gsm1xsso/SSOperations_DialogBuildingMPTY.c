/*===========================================================================
FILE: SSOperations_DialogBuildingMPTY.c

SERVICES: SS Operations dialog displayed while building MPTY call.

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
#include "SSOperations_CommonMessageDefinitions.h"
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
FUNCTION: SSOperationsDialog_BuildingMPTYHandleEvent

DESCRIPTION: Building MPTY dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_BuildingMPTYHandleEvent(
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
                    IDDC_BUILDING_MPTY_TEXT
                    );
            ISTATIC_SetProperties(pStatic, ST_CENTERTEXT | ST_MIDDLETEXT);
            ISTATIC_SetFont(pStatic, AEE_FONT_BOLD, AEE_FONT_NORMAL);

            //AUTOMATION_LOG("%s","Building MPTY");
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
                { 
                    SSMsg_int_msg_type* pMsg = 
                        SSOMessage_AllocateMsg(SSMSG_MSG_TYPE_FACILITY);
                    pMsg->facility.facility.hdr.present = TRUE;
                    pMsg->facility.facility.component.gen_hdr.present = TRUE;
                    pMsg->facility.facility.component.gen_hdr.component_type 
                        = SSMSG_COMPONENT_TYPE_RETURN_RESULT;

                    SSOperations_TriggerCCEventWithMsg(
                        pSelf, pMsg, FALSE, 
                        pSelf->m_BuildingMPTYCallDialogCallIndex
                        );
                }
                break;
                case AVK_1:
                { 
                    SSMsg_int_msg_type* pMsg = 
                        SSOMessage_AllocateMsg(SSMSG_MSG_TYPE_FACILITY);
                    pMsg->facility.facility.hdr.present = TRUE;
                    pMsg->facility.facility.component.gen_hdr.present = TRUE;
                    pMsg->facility.facility.component.gen_hdr.component_type 
                        = SSMSG_COMPONENT_TYPE_RETURN_ERROR;

                    SSOperations_TriggerCCEventWithMsg(
                        pSelf, pMsg, FALSE, 
                        pSelf->m_BuildingMPTYCallDialogCallIndex
                        );
                }
                break;
            }
#endif //U_TEST && AEE_SIMULATOR
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_BuildingMPTYHandleNonDisplayEvent

DESCRIPTION: Building MPTY dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_BuildingMPTYHandleNonDisplayEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    switch (eCode)
    {
        case UIMN_BUILD_MPTY_CNF:
        {
            SSEventData *pEventData = (SSEventData*)dwParam;
            assert(pEventData != NULL);
            assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

            // only handle event if it is for the building MPTY call
            if (pEventData->callwiseMsgData.callIndex1 != 
                pSelf->m_BuildingMPTYCallDialogCallIndex) return FALSE;

            SSOperationsDialogHandler_HandleBuildingMPTYPopupEvent(
                pSelf, 
                eCode, 
                wParam, 
                dwParam
                );

            SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupCallDetailsDialog );
            return TRUE;
        }
        break;

        case UIMN_ERR_IND:            
        {
            SSEventData *pEventData = (SSEventData*)dwParam;
            assert(pEventData != NULL);
            assert(pEventData->commonData.type == SS_EVENT_DATA_ERROR);

            // only handle a single call error event
            if (pEventData->errorData.isCallIndex2Valid == TRUE) return FALSE;

            // only handle error event if it is for the building MPTY call
            if (pEventData->errorData.callIndex1 != 
                pSelf->m_BuildingMPTYCallDialogCallIndex) return FALSE;

            SSOperationsDialogHandler_HandleBuildingMPTYPopupEvent(
                pSelf, 
                eCode, 
                wParam, 
                dwParam
                );

            SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupCallDetailsDialog );
            return TRUE;
        }
        break;
    }

    return FALSE;
}

/*===========================================================================

FUNCTION: SSOperationsDialog_SetupBuildingMPTYDialog()

DESCRIPTION:
    This function creates an Ending Call dialog.

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

RETURN VALUE:
   None

===========================================================================*/
void SSOperationsDialog_SetupBuildingMPTYDialog(
    CSSOperationsDialogHandler *pSelf,    
    boolean                     createNewDialog
    )
{
    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER( 
            SSOperationsDialog_BuildingMPTYHandleEvent 
            );
        SHOW_DIALOG( IDD_BUILDING_MPTY );
    }

    SET_NON_DISPLAY_EVENT_HANDLER( 
        SSOperationsDialog_BuildingMPTYHandleNonDisplayEvent 
        );
}
