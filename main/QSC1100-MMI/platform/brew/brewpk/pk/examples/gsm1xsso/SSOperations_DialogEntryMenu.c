/*=========================================================================
FILE: SSOperations_DialogEntryMenu.c

SERVICES: SS Operations Entry Menu

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

#include "AutomationLogging.h"

#include "SSOperations_DialogCommon.h"
#include "SSOperations_DialogHandler.h"
#include "SSOperations_DialogPopupHandler.h"

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
FUNCTION: SSOperationsDialog_EntryMenuHandleEvent

DESCRIPTION: Entry Menu dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_EntryMenuHandleEvent(
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
            AUTOMATION_LOG("%s","Entry Menu");
            return TRUE;
        }

        case EVT_DIALOG_START:
        {
            // Temp fix. 
            // The ID_SETUP_NEW_CALL softkey option is not really supported yet,
            // so just manually build the menu without it.

            IMenuCtl *pSkMenu;
            pSkMenu    = (IMenuCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                                       IDDC_ENTRY_MENU);

            (void) IMENUCTL_DeleteAll(pSkMenu);

            (void) IMENUCTL_AddItem(pSkMenu,
                                    SSOPERATIONS_RES_FILE,
                                    IDS_ENTRY_MENU_PUT_CALL_ON_HOLD,
                                    ID_PUT_CALL_ON_HOLD,
                                    (AECHAR*)NULL,
                                    (uint32)NULL);

            (void) IMENUCTL_AddItem(pSkMenu,
                                    SSOPERATIONS_RES_FILE,
                                    IDS_ABOUT,
                                    ID_ABOUT,
                                    (AECHAR*)NULL,
                                    (uint32)NULL);
            return TRUE;
        }

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_COMMAND:
        {
            switch (wParam) 
            {
            case ID_PUT_CALL_ON_HOLD:
                SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupCallDetailsDialog );

                (void) SS_PostEvent(
                          pSelf->m_pShell,
                          AEECLSID_SS_OPERATIONS,
                          EVT_ENTRY_MENU_HOLD_REQUESTED,
                          0,
                          0
                          );
                return TRUE;

            case ID_SETUP_NEW_CALL:
                SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupDialerDialog );
                return TRUE;

            case ID_ABOUT:
                SET_ABOUT_PREV_DIALOG_SETUP_TO_CURRENT;
                SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupAboutDialog );
                return TRUE;
            }
        }
        break;

        case EVT_KEY:
        {
            switch ((AVKType)wParam) 
            {
                case AVK_CLR:
                {
                   // close applet
                   return FALSE;
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
FUNCTION: SSOperationsDialog_EntryMenuHandleNonDisplayEvent

DESCRIPTION: Entry Menu dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_EntryMenuHandleNonDisplayEvent(
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
            // Entry Menu
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

FUNCTION: SSOperationsDialog_SetupEntryMenuDialog()

DESCRIPTION:
    This function creates an Entry Menu dialog.

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_SetupEntryMenuDialog(
    CSSOperationsDialogHandler *pSelf,
    boolean                     createNewDialog
    )
{
    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER( 
            SSOperationsDialog_EntryMenuHandleEvent 
            );
        SHOW_DIALOG( IDD_ENTRY_MENU );
    }

    SET_NON_DISPLAY_EVENT_HANDLER( 
        SSOperationsDialog_EntryMenuHandleNonDisplayEvent 
        );
}
