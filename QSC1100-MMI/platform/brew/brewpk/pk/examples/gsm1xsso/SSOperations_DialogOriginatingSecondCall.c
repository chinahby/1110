/*===========================================================================
FILE: SSOperations_DialogOriginatingSecondCall.c

SERVICES: SS Operations Dialog for Originating a SecondCall

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================== */

#include "AEEShell.h"         // AEE Shell Services
#include "AEEText.h"          // AEE Text Services
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

// Macro used to test if the originating second call's call info can
// be inspected/is available
#define IsOriginatingSecondCallInfoAvailable()                            \
    SSCallDatabase_IsCallPresent(                                         \
        pSelf->m_pCallDatabase,                                           \
        pSelf->m_OriginatingSecondCallDialogCallIndex                     \
        )                                                                 \

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
FUNCTION: SSOperationsDialog_OriginatingSecondCallHandleEvent

DESCRIPTION: Originating Second Call dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_OriginatingSecondCallHandleEvent(
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
            return TRUE;
        }

        case EVT_DIALOG_START:
        {
            ITextCtl *pBannerCtl;
            AEERect   rect;
            int       bOffset;

            pBannerCtl = (ITextCtl*)IDIALOG_GetControl(
                                       pSelf->m_pDialog,
                                       IDDC_CALLING_BANNER
                                       );
            //
            //  BANNER
            //

            // Position Banner on first line
            SETAEERECT(&rect,
                       0,
                       0,
                       pSelf->m_Rect.dx,
                       pSelf->m_LineHeight);

            ITEXTCTL_SetRect(pBannerCtl, &rect);

            //
            //  Position PHONE BOOK MATCH
            //

            // Position on second line
            SETAEERECT(&rect,
                       0,
                       pSelf->m_LineHeight,
                       pSelf->m_Rect.dx,
                       pSelf->m_LineHeight);

            ISTATIC_SetRect(pSelf->m_pNameCtl, &rect);

            //
            // Position PHONE NUMBER
            //

            // Position on last line, to the right
            bOffset = IDISPLAY_MeasureText(
                         pSelf->m_pDisplay,
                         AEE_FONT_NORMAL,
                         pSelf->m_DialString
                         );

            if (bOffset > pSelf->m_Rect.dx) 
            {
                bOffset = pSelf->m_Rect.dx;
            }

            SETAEERECT(&rect,
                       (pSelf->m_Rect.dx - bOffset),
                       (pSelf->m_Rect.dy - pSelf->m_LineHeight),
                        pSelf->m_Rect.dx,
                        pSelf->m_LineHeight);

            ISTATIC_SetRect(pSelf->m_pNumberCtl, &rect);

            (void) SS_PostEvent(
                      pSelf->m_pShell,
                      AEECLSID_SS_OPERATIONS,
                      EVT_USER_DRAW,
                      0,
                      0
                      );

            return TRUE;
        }

        case EVT_USER_DRAW:
        {
            SSCall_TInfo_type *pCallTInfo;
            ITextCtl          *pBannerCtl;

            pBannerCtl = (ITextCtl*) IDIALOG_GetControl(
                                        pSelf->m_pDialog,
                                        IDDC_CALLING_BANNER
                                        );

            ITEXTCTL_SetActive(pBannerCtl, FALSE);

            // if originating second call's call index is no longer valid,
            // (there is no longer a second call) then leave the dialog
            if (IsOriginatingSecondCallInfoAvailable() == FALSE)
            {
                SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupCallDetailsDialog );
                return TRUE;
            }

            pCallTInfo =
                SSOperationsDialogHandler_GetCallTInfo(
                   pSelf,
                   pSelf->m_OriginatingSecondCallDialogCallIndex
                   );

            //
            //  Set PHONE BOOK MATCH
            //
            (void) ISTATIC_SetText(
                      pSelf->m_pNameCtl,
                      NULL,
                      pCallTInfo->callName,
                      AEE_FONT_NORMAL,
                      AEE_FONT_NORMAL
                      );

            (void) ISTATIC_Redraw(pSelf->m_pNameCtl);

            //
            // Set PHONE NUMBER
            //
            (void) ISTATIC_SetText(
                      pSelf->m_pNumberCtl,
                      NULL,
                      pCallTInfo->callString,
                      AEE_FONT_NORMAL,
                      AEE_FONT_NORMAL
                      );

            (void) ISTATIC_Redraw(pSelf->m_pNumberCtl);

            return TRUE;
        }

        case EVT_DIALOG_END:
        {
            ISTATIC_Reset(pSelf->m_pNumberCtl);
            ISTATIC_Reset(pSelf->m_pNameCtl);
            return TRUE;
        }

        case EVT_KEY:

#if (defined U_TEST && defined AEE_SIMULATOR)
        // for unit testing
        // ----------------
        switch ((AVKType)wParam) 
        {
            case AVK_0:
                SSOperations_TriggerCCEvent(
                   pSelf, SSMSG_MSG_TYPE_CALL_PROCEEDING, TRUE, 0
                   );
                break;

            case AVK_1:
                SSOperations_TriggerCCEvent(
                   pSelf, SSMSG_MSG_TYPE_CONNECT, TRUE, 0
                   );
                break;

            case AVK_2:
                SSOperations_TriggerCCEvent(
                   pSelf, SSMSG_MSG_TYPE_RELEASE, TRUE, 0
                   );
                break;
        }
#endif //U_TEST && AEE_SIMULATOR
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_OriginatingSecondCallHandleNonDisplayEvent

DESCRIPTION: Originating Second Call dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_OriginatingSecondCallHandleNonDisplayEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    SSCall_Index_type eventCallIndex;
    boolean           checkEvent = FALSE;

    switch (eCode)
    {
        case UIMN_SETUP_CNF:
        {
            SSEventData *pEventData = (SSEventData*) dwParam;
            assert(pEventData != NULL);
            assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

            eventCallIndex = pEventData->callwiseMsgData.callIndex1;
            checkEvent = TRUE;
        }
        break;

        case UIMN_REL_CNF:
        case UIMN_REL_IND:
        {
            SSEventData *pEventData = (SSEventData*) dwParam;
            assert(pEventData != NULL);
            assert(pEventData->commonData.type == 
                   SS_EVENT_DATA_TRANSACTIONWISE_MSG);

            eventCallIndex = pEventData->transactionwiseMsgData.callIndex;
            checkEvent = TRUE;
        }
        break;
    }

    if (checkEvent == TRUE)
    {
        if (eventCallIndex == pSelf->m_OriginatingSecondCallDialogCallIndex)
        {
            SSOperationsDialogHandler_HandleOriginatingSecondCallPopupEvent(
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

FUNCTION: SSOperationsDialog_SetupOriginatingSecondCallDialog()

DESCRIPTION:
    This function creates a Originating Second Call dialog.

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_SetupOriginatingSecondCallDialog(
    CSSOperationsDialogHandler *pSelf,
    boolean                     createNewDialog
    )
{
    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER( 
            SSOperationsDialog_OriginatingSecondCallHandleEvent 
            );
        SHOW_DIALOG( IDD_CALLING );
    }

    SET_NON_DISPLAY_EVENT_HANDLER( 
        SSOperationsDialog_OriginatingSecondCallHandleNonDisplayEvent 
        );
}