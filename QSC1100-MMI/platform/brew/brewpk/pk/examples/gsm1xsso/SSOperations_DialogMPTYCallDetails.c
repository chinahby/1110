/*===========================================================================
FILE: SSOperations_DialogMPTYCallDetails.c

SERVICES: SS Operations dialog that displays MPTY Call Details.

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
#include "AEEMenu.h"

#include "AutomationLogging.h"

#include "SSOperations_DialogCommon.h"
#include "SSOperations_DialogHandler.h"
#include "SSOperations_DialogPopupHandler.h"
#include "SSOperations_CommonMessageDefinitions.h"
#include "SSOperations_UserEvents.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/
#define FORMAT_TEXT_MAX_LENGTH 20
#define LIST_ENTRY_TEXT_MAX_LENGTH (MAX_SIZE_NAME_TEXT+FORMAT_TEXT_MAX_LENGTH+10)

/*===========================================================================
                              MACRO DEFINITIONS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/
static void SSOperationsDialog_SetMPTYDetailsListMenu(
    CSSOperationsDialogHandler *pSelf,
    IMenuCtl                   *pListMenu
    );

static void SSOperationsDialog_SetMPTYDetailsSoftkeyMenu(
    CSSOperationsDialogHandler *pSelf,
    IMenuCtl                   *pSkMenu
    );

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

/*=============================================================================
FUNCTION: SSOperationsDialog_MPTYCallDetailsHandleEvent

DESCRIPTION: MPTY Call Details dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_MPTYCallDetailsHandleEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
        
        case EVT_DIALOG_START:
        {
            IMenuCtl *pListMenu;
            IMenuCtl *pSkMenu;

            pListMenu = (IMenuCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                                     IDDC_MPTY_DETAILS_LEGS);

            pSkMenu = (IMenuCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                                    IDDC_MPTY_DETAILS_SK);

            //SSOperations_SetDefaultSoftkeyLook(pSelf->m_pShell, pListMenu);

            SSOperations_SetDefaultSoftkeyLook(pSelf->m_pShell, pSkMenu);

            // populate list menu
            SSOperationsDialog_SetMPTYDetailsListMenu(pSelf, pListMenu);

            // populate softkey
            SSOperationsDialog_SetMPTYDetailsSoftkeyMenu(pSelf, pSkMenu);

            return TRUE;
        }

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_COMMAND:
        {
            switch (wParam) 
            {
                case IDS_DIALOG_MPTY_DETAILS_OKAY:
                {
                    SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupCallDetailsDialog );
                    return TRUE;
                }
                break;

                case IDS_DIALOG_MPTY_DETAILS_DISCONNECT:
                {
                    IMenuCtl    *pListMenu;
                    uint16       selectedEntryId;
                    SSEventData *pEventData;

                    pListMenu = (IMenuCtl*)IDIALOG_GetControl(
                                              pSelf->m_pDialog,
                                              IDDC_MPTY_DETAILS_LEGS
                                              );
                    selectedEntryId = IMENUCTL_GetSel(pListMenu);

                    pEventData = 
                        SSOEvent_AllocateEventData(SS_EVENT_DATA_TRANSACTIONWISE_MSG);

                    if (pEventData == NULL) return TRUE;

                    pEventData->transactionwiseMsgData.callIndex = 
                        pSelf->m_MPTYCallDetailsDialogCallIndex;

                    // The list menu is build in such a way that the 
                    // entry's id is the same as its transaction info index
                    pEventData->transactionwiseMsgData.tInfoIndex = 
                        (SSCall_TInfo_Index_type) selectedEntryId;

                    (void) SS_PostEvent(
                              pSelf->m_pShell,
                              AEECLSID_SS_OPERATIONS,
                              UIMN_DISC_REQ,
                              0,
                              (uint32) pEventData
                              );

                    pSelf->m_EndingCallDialogCallIndex =
                        pSelf->m_MPTYCallDetailsDialogCallIndex;

                    SET_ENDING_CALL_PREV_DIALOG_SETUP_TO_CURRENT;
                    SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupEndingCallDialog );    
                    return TRUE;
                }
                break;
            }
        }
        break;

        case EVT_KEY:

#if (defined U_TEST && defined AEE_SIMULATOR)
            // for unit testing
            // ----------------
            switch ((AVKType)wParam) 
            {
                case AVK_0:
                {
                    SSMsg_int_msg_type* pMsg = 
                      SSOMessage_AllocateMsg(SSMSG_MSG_TYPE_SETUP);

                    pMsg->setup.hdr.pd = SSMSG_PD_CC;
                    pMsg->setup.hdr.tiflag = 0;
                    pMsg->setup.hdr.tivalue = 1;
                    pMsg->setup.hdr.msg_type = SSMSG_MSG_TYPE_SETUP;
                    pMsg->setup.signal.hdr.present = TRUE;
                    pMsg->setup.signal.signal_value = 
                        SSMSG_SIGNAL_VALUE_CALL_WAITING_TONE_ON;

                    pMsg->setup.calling_party_bcd_number.hdr.present = TRUE;
                    pMsg->setup.calling_party_bcd_number.pi =
                        SSMSG_PI_NUMBER_NOT_AVAILABLE_DUE_TO_INTERWORKING;

                    MEMCPY(pMsg->setup.calling_party_bcd_number.number, "18007171", 8);
                    pMsg->setup.calling_party_bcd_number.number_len = 8;

                    SSOperations_TriggerCCEventWithMsg(
                       pSelf, pMsg, FALSE, 3
                       );
                }
                break;            
            }
#endif //U_TEST && AEE_SIMULATOR
            switch ((AVKType)wParam) 
            {
                case AVK_SELECT:
                {
                    IMenuCtl *pListMenu;
                    IMenuCtl *pSkMenu;

                    pListMenu = (IMenuCtl*)IDIALOG_GetControl(
                                            pSelf->m_pDialog,
                                            IDDC_MPTY_DETAILS_LEGS
                                            );

                    pSkMenu = (IMenuCtl*)IDIALOG_GetControl(
                                            pSelf->m_pDialog,
                                            IDDC_MPTY_DETAILS_SK
                                            );

                    IMENUCTL_SetActive(pListMenu, FALSE);
                    IMENUCTL_SetActive(pSkMenu, TRUE);

                    return TRUE;
                }
                break;

                case AVK_CLR:
                {
                    SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupCallDetailsDialog );
                    return TRUE;                    
                }
                break;
            }


        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_MPTYCallDetailsHandleNonDisplayEvent

DESCRIPTION: MPTY Call Details dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_MPTYCallDetailsHandleNonDisplayEvent(
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
            // change the current dialog setup handler to be call details
            // so that the incoming dialog will return to it and not to the  
            // current dialog
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

FUNCTION: SSOperationsDialog_SetupMPTYCallDetailsDialog()

DESCRIPTION:
    This function creates a MPTY Call Details dialog.

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_SetupMPTYCallDetailsDialog(
    CSSOperationsDialogHandler *pSelf,
    boolean                     createNewDialog
    )
{
    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER( 
            SSOperationsDialog_MPTYCallDetailsHandleEvent 
            );
        SHOW_DIALOG( IDD_MPTY_DETAILS );
    }

    SET_NON_DISPLAY_EVENT_HANDLER( 
        SSOperationsDialog_MPTYCallDetailsHandleNonDisplayEvent 
        );
}

/*=============================================================================
FUNCTION: SSOperationsDialog_SetMPTYDetailsListMenu

DESCRIPTION: Used by the MPTY Call Details dialog to set the list menu data

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
   *pListMenu [in+out]: IMenuCtl interface pointer
   
RETURN VALUE:
    None

=============================================================================*/
static void SSOperationsDialog_SetMPTYDetailsListMenu(
    CSSOperationsDialogHandler *pSelf,
    IMenuCtl                   *pListMenu
    )
{
    SSCall_Info_type  *pCallInfo;
    SSCall_TInfo_type *pCallTInfo;
    boolean            success;
    uint8              index;

    success = SSCallDatabase_GetCallInfo(
                 pSelf->m_pCallDatabase,
                 pSelf->m_MPTYCallDetailsDialogCallIndex,
                 &pCallInfo
                 );
    if (!success)
    {
        // MPTY call does not exist anymore, so close the dialog and return 
        // to the call details dialog
        SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupCallDetailsDialog );
        return;
    }

    for (index=0; index < pCallInfo->numTransactions; ++index)
    {
        success = SSCallDatabase_GetTransactionInfo(
                     pSelf->m_pCallDatabase, 
                     pCallInfo->transactionIndices[index],
                     &pCallTInfo
                     );
        assert(success == TRUE);

        if (pCallTInfo->callStatus == SS_CALL_STATUS_ACTIVE)
        {
            AECHAR     szTextBuffer[LIST_ENTRY_TEXT_MAX_LENGTH+1];
            AECHAR     szFormatText[FORMAT_TEXT_MAX_LENGTH+1];
            AECHAR    *pShownCallText;

            CtlAddItem menuCtlAddItem;

            MEMSET(&menuCtlAddItem, 0, sizeof(menuCtlAddItem));

            (void) ISHELL_LoadResString(
                      pSelf->m_pShell,
                      SSOPERATIONS_RES_FILE,
                      IDS_DIALOG_MPTY_DETAILS_LIST_ENTRY_FORMAT,
                      szFormatText,
                      sizeof(szFormatText)
                      );

            if (pCallTInfo->callName[0] != (AECHAR)'\0')
            {
                pShownCallText = pCallTInfo->callName;
            }
            else
            {
                pShownCallText = pCallTInfo->callString;
            }

            WSPRINTF(
                szTextBuffer,
                sizeof(szTextBuffer),
                szFormatText, 
                pCallInfo->transactionIndices[index],
                pShownCallText
                );

            menuCtlAddItem.pText = szTextBuffer;
            menuCtlAddItem.wItemID = pCallInfo->transactionIndices[index];
            menuCtlAddItem.dwData = pCallTInfo->callStartTime;

            success = IMENUCTL_AddItemEx(pListMenu, &menuCtlAddItem);
            assert(success == TRUE);

            AUTOMATION_LOG_WSTR("M:%s",szTextBuffer);
        }
    }

    // sort text entries according to their call time (the data value)
    IMENUCTL_Sort(pListMenu, MCS_PARAM_ASCENDING);
    
    // disable command selection from list menu
    IMENUCTL_EnableCommand(pListMenu, FALSE);
}

/*=============================================================================
FUNCTION: SSOperationsDialog_SetMPTYDetailsSoftkeyMenu

DESCRIPTION: Used by the MPTY Call Details dialog to set the softkey menu options

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
   *pSkMenu [in+out]: IMenuCtl interface pointer
   
RETURN VALUE:
    None

=============================================================================*/
static void SSOperationsDialog_SetMPTYDetailsSoftkeyMenu(
    CSSOperationsDialogHandler *pSelf,
    IMenuCtl                   *pSkMenu
    )
{
    (void) IMENUCTL_DeleteAll(pSkMenu);

    (void) IMENUCTL_AddItem(pSkMenu,
                            SSOPERATIONS_RES_FILE,
                            IDS_DIALOG_MPTY_DETAILS_OKAY,
                            IDS_DIALOG_MPTY_DETAILS_OKAY,
                            (AECHAR*)NULL,
                            (uint32)NULL);

    (void) IMENUCTL_AddItem(pSkMenu,
                            SSOPERATIONS_RES_FILE,
                            IDS_DIALOG_MPTY_DETAILS_DISCONNECT,
                            IDS_DIALOG_MPTY_DETAILS_DISCONNECT,
                            (AECHAR*)NULL,
                            (uint32)NULL);
}
