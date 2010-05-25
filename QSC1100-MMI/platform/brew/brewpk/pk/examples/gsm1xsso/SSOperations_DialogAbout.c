/*=========================================================================
FILE: SSOperations_DialogAbout.c

SERVICES: SS Operations About

GENERAL DESCRIPTION:
    Present and manage the menu that appears on when SSO starts.

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================== */
#include "AEEShell.h"         // AEE Shell Services
#include "AEEStdLib.h"        // AEE StdLib Services

#include "SSOperations_DialogCommon.h"
#include "SSOperations_DialogHandler.h"

#include "SSOperations_UserEvents.h"
#include "SSOperations_CommonMessageDefinitions.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/
// Time before the About Dialog is automatically closed.
#define ABOUT_TIMEOUT          (10*TIMEOUT_MS_ONE_SECOND)

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
FUNCTION: SSOperationsDialog_AboutHandleEvent

DESCRIPTION: About dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_AboutHandleEvent(
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
            AECHAR   endOfLineString[2] = {'\n', '\0'};
            IStatic *pStatic = (IStatic*)IDIALOG_GetControl(
                                            pSelf->m_pDialog,
                                            IDDC_ABOUT_TEXT
                                            );
            ISTATIC_SetProperties(pStatic, ST_MIDDLETEXT);
            
            pSelf->m_AboutTextBuf[0] = '\0';
            ISHELL_LoadResString(
                        pSelf->m_pShell,
                        SSOPERATIONS_RES_FILE,
                        IDS_MODULE_NAME,
                        pSelf->m_AboutTextBuf,
                        sizeof(pSelf->m_AboutTextBuf)
                        );

            (void) ISTATIC_SetText(
                      pStatic,
                      pSelf->m_AboutTextBuf,
                      NULL,
                      AEE_FONT_NORMAL,
                      AEE_FONT_NORMAL
                      );

            pSelf->m_AboutTextBuf[0] = '\0';
            ISHELL_LoadResString(
                        pSelf->m_pShell,
                        SSOPERATIONS_RES_FILE,
                        IDS_SSO_VERSION,
                        pSelf->m_AboutTextBuf,
                        sizeof(pSelf->m_AboutTextBuf)
                        );

            (void) ISTATIC_SetTextEx(pStatic, (void*)endOfLineString, NULL, TRUE /*Append*/);
            (void) ISTATIC_SetTextEx(pStatic, (void*)pSelf->m_AboutTextBuf, NULL, TRUE /*Append*/);

            pSelf->m_AboutTextBuf[0] = '\0';
            ISHELL_LoadResString(
                        pSelf->m_pShell,
                        SSOPERATIONS_RES_FILE,
                        IDS_AGENT_VERSION,
                        pSelf->m_AboutTextBuf,
                        sizeof(pSelf->m_AboutTextBuf)
                        );

            (void) ISTATIC_SetTextEx(pStatic, (void*)endOfLineString, NULL, TRUE /*Append*/);
            (void) ISTATIC_SetTextEx(pStatic, (void*)pSelf->m_AboutTextBuf, NULL, TRUE /*Append*/);
            
            pSelf->m_AboutTextBuf[0] = '\0';
            ISHELL_LoadResString(
                        pSelf->m_pShell,
                        SSOPERATIONS_RES_FILE,
                        IDS_PROMPT_VERSION,
                        pSelf->m_AboutTextBuf,
                        sizeof(pSelf->m_AboutTextBuf)
                        );

            (void) ISTATIC_SetTextEx(pStatic, (void*)endOfLineString, NULL, TRUE /*Append*/);
            (void) ISTATIC_SetTextEx(pStatic, (void*)pSelf->m_AboutTextBuf, NULL, TRUE /*Append*/);

            (void) ISTATIC_SetTextEx(pStatic, (void*)endOfLineString, NULL, TRUE /*Append*/);
            (void) ISTATIC_SetTextEx(
                      pStatic, 
                      (void*)ISSUTILCLS_GetVersionString(pSelf->m_pSSUtilCls), 
                      NULL, 
                      TRUE /*Append*/
                      );

            pSelf->m_AboutTextBuf[0] = '\0';
            ISHELL_LoadResString(
                        pSelf->m_pShell,
                        SSOPERATIONS_RES_FILE,
                        IDS_ABOUT_COPYRIGHT,
                        pSelf->m_AboutTextBuf,
                        sizeof(pSelf->m_AboutTextBuf)
                        );

            (void) ISTATIC_SetTextEx(pStatic, (void*)endOfLineString, NULL, TRUE /*Append*/);
            (void) ISTATIC_SetTextEx(pStatic, (void*)endOfLineString, NULL, TRUE /*Append*/);
            (void) ISTATIC_SetTextEx(pStatic, (void*)pSelf->m_AboutTextBuf, NULL, TRUE /*Append*/);

            return TRUE;
        }

        case EVT_DIALOG_START:
        {
             // Always start a timer for the dialog
            CALLBACK_Init(
               &pSelf->m_UIDialogTimerCB, 
               SSOperationsDialog_HandleDialogTimeout, 
               (void*)pSelf
               );

            (void) ISHELL_SetTimerEx(
                      pSelf->m_pShell,
                      ABOUT_TIMEOUT,
                      &pSelf->m_UIDialogTimerCB
                      );

            return TRUE;
        }

        case EVT_USER_DIALOG_TIMEOUT:
        {
            SETUP_NEW_UI_DIALOG( GET_ABOUT_PREV_DIALOG_SETUP );
            return TRUE;
        }

        case EVT_DIALOG_END:
        {
            CALLBACK_Cancel(&pSelf->m_UIDialogTimerCB); // cancel timer callback
            return TRUE;
        }

        case EVT_KEY:
        {
            switch ((AVKType)wParam) 
            {
                case AVK_CLR:
                {
                    SETUP_NEW_UI_DIALOG( GET_ABOUT_PREV_DIALOG_SETUP );
                    return TRUE;
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
                    pMsg->setup.calling_party_bcd_number.pi =
                        SSMSG_PI_NUMBER_NOT_AVAILABLE_DUE_TO_INTERWORKING;

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
FUNCTION: SSOperationsDialog_AboutHandleNonDisplayEvent

DESCRIPTION: About dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_AboutHandleNonDisplayEvent(
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
            // About
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

FUNCTION: SSOperationsDialog_SetupAboutDialog()

DESCRIPTION:
    This function creates an About dialog.

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_SetupAboutDialog(
    CSSOperationsDialogHandler *pSelf,
    boolean                     createNewDialog
    )
{
    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER(
            SSOperationsDialog_AboutHandleEvent
            );
        SHOW_DIALOG( IDD_ABOUT );
    }

    SET_NON_DISPLAY_EVENT_HANDLER(
        SSOperationsDialog_AboutHandleNonDisplayEvent
        );
}

/*===========================================================================

FUNCTION: SSOperationsDialog_SetupAboutDialogWhenInvalidPhoneState()

DESCRIPTION:
    This function creates an About dialog, when the 

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_SetupAboutDialogWhenInvalidPhoneState(
    CSSOperationsDialogHandler *pSelf,
    boolean                     createNewDialog
    )
{
    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER(
            SSOperationsDialog_AboutHandleEvent
            );
        SHOW_DIALOG( IDD_ABOUT );
    }

    SET_NON_DISPLAY_EVENT_HANDLER(NULL);
}