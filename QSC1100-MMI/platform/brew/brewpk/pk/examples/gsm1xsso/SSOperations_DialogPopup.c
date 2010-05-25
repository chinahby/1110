/*===========================================================================
FILE: SSOperations_DialogPopup.c

SERVICES: SS Operations dialog that displays a pop-up message dialog.

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
#include "SSOperations_DialogPopupHandler.h"
#include "SSOperations_UserEvents.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// Time before the Pop-up Dialog is automatically closed.
#define POPUP_TIMEOUT          (3*TIMEOUT_MS_ONE_SECOND)

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
FUNCTION: SSOperationsDialog_HandlePopupTimeout

DESCRIPTION: When this callback is invoked, it sends a EVT_POPUP_DIALOG_TIMEOUT event
             to the applet (which will be routed to the pop-up dialog)

PARAMETERS:
   *pUser: CSSOperationsDialogHandler object pointer

RETURN VALUE:
    None

=============================================================================*/
static void SSOperationsDialog_HandlePopupTimeout(void *pUser)
{
    boolean eventhandled;
    CSSOperationsDialogHandler *pSelf = (CSSOperationsDialogHandler *)pUser;

    assert(pSelf->m_pDialog != NULL);

    eventhandled = ISHELL_SendEvent(pSelf->m_pShell,
                           AEECLSID_SS_OPERATIONS,
                           EVT_POPUP_DIALOG_TIMEOUT,
                           0,
                           0);
    assert(eventhandled == TRUE);
}

/*=============================================================================
FUNCTION: SSOperationsDialog_PopupHandleEvent

DESCRIPTION: Pop-up dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_PopupHandleEvent(
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
         if (ISHELL_CreateInstance(pSelf->m_pShell,
                     AEECLSID_STATIC,(void**)&pSelf->m_pPopupIStatic) != SUCCESS)
         {
            return FALSE;
         }

         ISTATIC_SetRect(pSelf->m_pPopupIStatic, &pSelf->m_Rect);

         ISTATIC_SetProperties(pSelf->m_pPopupIStatic,
                               ST_CENTERTEXT | ST_MIDDLETEXT);

         (void) ISTATIC_SetText(pSelf->m_pPopupIStatic,
                                NULL,
                                pSelf->m_pPopupText,
                                AEE_FONT_NORMAL,
                                AEE_FONT_BOLD);

         AUTOMATION_LOG_WSTR("%s",pSelf->m_pPopupText);

         // Always start a timer for the popup
         CALLBACK_Init(
            &pSelf->m_UIDialogTimerCB, 
            SSOperationsDialog_HandlePopupTimeout, 
            (void*)pSelf
            );

         (void) ISHELL_SetTimerEx(
                   pSelf->m_pShell,
                   POPUP_TIMEOUT,
                   &pSelf->m_UIDialogTimerCB
                   );
         return TRUE;
      }

      case EVT_DIALOG_START:
         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) SS_PostEvent(
                   pSelf->m_pShell,
                   AEECLSID_SS_OPERATIONS,
                   EVT_USER_POPUP_REDRAW,
                   0,
                   0
                   );
         return TRUE;

      case EVT_USER_POPUP_REDRAW:
         (void) ISTATIC_Redraw(pSelf->m_pPopupIStatic);
         return TRUE;

      case EVT_POPUP_DIALOG_TIMEOUT: 
         CLOSE_ACTIVE_DIALOG;
         return TRUE;

      case EVT_DIALOG_END:
         CALLBACK_Cancel(&pSelf->m_UIDialogTimerCB); // cancel timer callback

         if (NULL != pSelf->m_pPopupIStatic)
         {
            ISTATIC_Reset(pSelf->m_pPopupIStatic);
            (void) ISTATIC_Release(pSelf->m_pPopupIStatic);
            pSelf->m_pPopupIStatic = NULL;
         }

         SSOperationsDialogHandler_HandlePopupDialogEnd(pSelf);
         return TRUE;

      case EVT_KEY:
         CLOSE_ACTIVE_DIALOG;
         return TRUE;
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_PopupHandleNonDisplayEvent

DESCRIPTION: Pop-up dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_PopupHandleNonDisplayEvent(
   CSSOperationsDialogHandler *pSelf,
   AEEEvent                    eCode,
   uint16                      wParam,
   uint32                      dwParam
)
{
   return FALSE;
}

/*===========================================================================

FUNCTION: SSOperationsDialog_SetupPopupDialog()

DESCRIPTION:
    This function creates a Pop-up dialog.

PARAMETERS:
    pSelf: Pointer to the dialog handler object

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_SetupPopupDialog(CSSOperationsDialogHandler *pSelf)
{
   SET_DISPLAY_DIALOG_HANDLER( SSOperationsDialog_PopupHandleEvent );

   pSelf->m_pPopupNonDisplayEventHandler = 
       SSOperationsDialog_PopupHandleNonDisplayEvent;

   SHOW_DIALOG( IDD_POPUP );
}
