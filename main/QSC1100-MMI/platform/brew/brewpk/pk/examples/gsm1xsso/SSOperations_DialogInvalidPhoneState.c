/*===========================================================================
FILE: SSOperations_DialogInvalidPhoneState.c

SERVICES: 
    This dialog is displayed if SS Operations Applet tries 
    to start while the phone is not ready.

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
#include "AEEText.h"          // AEE Text Services
#include "AEEStdLib.h"        // AEE StdLib Services

#include "AutomationLogging.h"

#include "SSOperations_DialogCommon.h"
#include "SSOperations_DialogHandler.h"
#include "SSOperations_UserEvents.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// Time before the Invalid Phone State Dialog is automatically closed.
#define INVALID_PHONE_STATE_TIMEOUT          (4*TIMEOUT_MS_ONE_SECOND)

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
FUNCTION: SSOperationsDialog_HandleInvalidPhoneStateTimeout

DESCRIPTION: When this callback is invoked, it closes the applet

PARAMETERS:
   *pUser: CSSOperationsDialogHandler object pointer

RETURN VALUE:
    None

=============================================================================*/
static void SSOperationsDialog_HandleInvalidPhoneStateTimeout(void *pUser)
{
    CSSOperationsDialogHandler *pSelf = (CSSOperationsDialogHandler *)pUser;
    assert(pSelf->m_pDialog != NULL);

    ISHELL_CloseApplet(pSelf->m_pShell, FALSE);
}

/*=============================================================================
FUNCTION: SSOperationsDialog_InvalidPhoneStateHandleEvent

DESCRIPTION: Invalid Phone State dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_InvalidPhoneStateHandleEvent(
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
            IMenuCtl *pSKMenu = (IMenuCtl*)IDIALOG_GetControl(
                                              pSelf->m_pDialog,
                                              IDDC_INVALID_PHONE_STATE_SK
                                              );

            SSOperations_SetDefaultSoftkeyLook(pSelf->m_pShell, pSKMenu);

            ISTATIC_SetProperties(pSelf->m_pNameCtl, ST_CENTERTEXT | ST_MIDDLETEXT);

            AUTOMATION_LOG("%s","Invalid Phone State!");  
            return TRUE;
        }

        case EVT_DIALOG_START:
        {
            AEERect   rc;
            IMenuCtl *pSKMenu = (IMenuCtl*)IDIALOG_GetControl(
                                              pSelf->m_pDialog,
                                              IDDC_INVALID_PHONE_STATE_SK
                                              );
            IMENUCTL_GetRect(pSKMenu, &rc);
            rc.dy = rc.y;
            rc.y = 0;
            ISTATIC_SetRect(pSelf->m_pNameCtl, &rc);

            pSelf->m_InvalidPhoneStateTextBuf[0] = '\0';
            (void) ISHELL_LoadResString(
                      pSelf->m_pShell,
                      SSOPERATIONS_RES_FILE,
                      IDS_INVALID_PHONE_STATE,
                      pSelf->m_InvalidPhoneStateTextBuf,
                      sizeof(pSelf->m_InvalidPhoneStateTextBuf)
                      );

            (void) ISTATIC_SetText(
                      pSelf->m_pNameCtl,
                      NULL,
                      pSelf->m_InvalidPhoneStateTextBuf,
                      AEE_FONT_NORMAL,
                      AEE_FONT_NORMAL
                      );

            // Always start a timer for the dialog
            CALLBACK_Init(
               &pSelf->m_UIDialogTimerCB, 
               SSOperationsDialog_HandleInvalidPhoneStateTimeout, 
               (void*)pSelf
               );

            (void) ISHELL_SetTimerEx(
                      pSelf->m_pShell,
                      INVALID_PHONE_STATE_TIMEOUT,
                      &pSelf->m_UIDialogTimerCB
                      );

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
            // KLUDGE!  Need to wait for the Dialog to draw itself before
            //          the ISTATIC_Redraw() command can be issued or the
            //          Dialog will overwrite the IStatic control
            ISTATIC_Redraw(pSelf->m_pNameCtl);
            return TRUE;
        }

        case EVT_DIALOG_END:
        {
            // if moving to a new dialog and not closing,
            // the callback must be cancelled!
            CALLBACK_Cancel(&pSelf->m_UIDialogTimerCB); // cancel timer callback

            // cleanup shared static control
            ISTATIC_Reset(pSelf->m_pNameCtl);
            return TRUE;
        }

        case EVT_COMMAND:
        {
            switch (wParam) 
            {
                case ID_ABOUT_WHILE_INVALID_PHONE_STATE:
                {
                    SET_ABOUT_PREV_DIALOG_SETUP_TO_CURRENT;
                    SETUP_NEW_UI_DIALOG( 
                        SSOperationsDialog_SetupAboutDialogWhenInvalidPhoneState
                        );
                    return TRUE;
                }
            }

            return FALSE;
        }

        case EVT_KEY:
            // close applet when AVK_CLR is pressed
            if ((AVKType)wParam != AVK_CLR)
            {
                return TRUE;
            }
    }

    return FALSE;
}

/*===========================================================================

FUNCTION: SSOperationsDialog_SetupInvalidPhoneStateDialog()

DESCRIPTION:
    This function creates an Invalid Phone State dialog.

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_SetupInvalidPhoneStateDialog(
    CSSOperationsDialogHandler *pSelf,
    boolean                     createNewDialog
    )
{
    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER( 
            SSOperationsDialog_InvalidPhoneStateHandleEvent 
            );    
        SHOW_DIALOG( IDD_INVALID_PHONE_STATE );
    }

    SET_NON_DISPLAY_EVENT_HANDLER( NULL );
}
