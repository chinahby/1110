
/*===========================================================================
FILE: SSOperations_DialogIncoming.c

SERVICES: SS Operations - Dialog for Incoming Call

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
#include "SSMsg.h"

#include "SSOperations_DialogCommon.h"
#include "SSOperations_DialogHandler.h"
#include "SSOperations_UserEvents.h"
#include "SSOperations_CommonMessageDefinitions.h"
#include "SSOperations_CommonEventDefinitions.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/
#define TIMEOUT_STATIC_TEXT_SCROLLING          (3*TIMEOUT_MS_ONE_SECOND)

/*===========================================================================
                              MACRO DEFINITIONS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/
static void SSOperationsDialog_SetIncomingCallName(
   CSSOperationsDialogHandler *pSelf,
   IStatic                    *pCallNameCtl
   );

static void SSOperationsDialog_SetIncomingSoftkeyMenu(
   CSSOperationsDialogHandler *pSelf,
   IMenuCtl                   *pSkMenu
   );

static const SSCall_Info_type* SSOperationsDialog_GetSecondCallData(
   CSSOperationsDialogHandler *pSelf,
   SSCall_Index_type           knownCallIndex,
   SSCall_Index_type          *newCallIndex
   );

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

/*=============================================================================
FUNCTION: SSOperationsDialog_IncomingCallHandleEvent

DESCRIPTION: Incoming call dialog event handler (IDD_INCOMINGCALL)

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_IncomingCallHandleEvent(
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
         IMenuCtl *pSKMenu;
         AEERect   rect;

         pBannerCtl     = (ITextCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                                        IDDC_INCOMINGCALL_BANNER);
         pSKMenu        = (IMenuCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                                        IDDC_INCOMINGCALL_SK);

         SSOperationsDialog_SetIncomingSoftkeyMenu(pSelf, pSKMenu);

         SSOperations_SetDefaultSoftkeyLook(pSelf->m_pShell, pSKMenu);

         //
         //  BANNER
         //
         ITEXTCTL_SetSoftKeyMenu(pBannerCtl, NULL);
         SETAEERECT(&rect,
                    0,
                    0,
                    pSelf->m_Rect.dx,
                    pSelf->m_LineHeight);

         ITEXTCTL_SetRect(pBannerCtl, &rect);

         //
         //  NAME
         //

         // Display on second line
         SETAEERECT(&rect,
                    0,
                    pSelf->m_LineHeight,
                    pSelf->m_Rect.dx,
                    pSelf->m_LineHeight);

         ISTATIC_SetRect(pSelf->m_pNameCtl, &rect);

         (void) IDIALOG_SetFocus(pSelf->m_pDialog, IDDC_INCOMINGCALL_SK);

         (void) SS_PostEvent(
                   pSelf->m_pShell,
                   AEECLSID_SS_OPERATIONS,
                   EVT_USER_DRAW,
                   0,
                   0
                   );
         return TRUE;
      }

      case EVT_DIALOG_END:
      {
         // Stop tone if any are playing
         ISOUND_StopTone (pSelf->m_pSound);

         IStaticScroller_SetIStatic(pSelf->m_pNumberScroller, NULL);
         IStaticScroller_SetIStatic(pSelf->m_pNameScroller, NULL);

         ISTATIC_Reset(pSelf->m_pNumberCtl);
         ISTATIC_Reset(pSelf->m_pNameCtl);

         return TRUE;
      }

      case EVT_USER_DRAW:
      {
         IMenuCtl *pSkMenu;
         AEERect   rect;
         int       offset;

         // if incoming call index is no longer valid, so there is no longer
         // an incoming call then leave the incoming call dialog
         if (IsIncomingCallInfoAvailable() == FALSE)
         {
            SETUP_NEW_UI_DIALOG( GET_INCOMING_PREV_DIALOG_SETUP );
            return TRUE;
         }

         pSkMenu    = (IMenuCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                                    IDDC_INCOMINGCALL_SK);

         // NAME

         SSOperationsDialog_SetIncomingCallName(pSelf, pSelf->m_pNameCtl);
         ISTATIC_Redraw(pSelf->m_pNameCtl);

         IStaticScroller_SetIStatic(pSelf->m_pNameScroller, pSelf->m_pNameCtl);
         IStaticScroller_StartScrolling(
             pSelf->m_pNameScroller, 
             TIMEOUT_STATIC_TEXT_SCROLLING
             );

         // NUMBER

         // Display on line 3, to the right
         offset = IDISPLAY_MeasureText(
                     pSelf->m_pDisplay,
                     AEE_FONT_NORMAL,                                       
                     GetIncomingCallTransactionInfo()->callString
                     );

         if (offset > pSelf->m_Rect.dx) 
         {
            offset = pSelf->m_Rect.dx;
         }

         SETAEERECT(&rect,
                    pSelf->m_Rect.dx - offset,
                    pSelf->m_LineHeight*2,
                    pSelf->m_Rect.dx,
                    pSelf->m_LineHeight);

         ISTATIC_SetRect(pSelf->m_pNumberCtl, &rect);

         (void) ISTATIC_SetText(
                   pSelf->m_pNumberCtl,
                   NULL,
                   GetIncomingCallTransactionInfo()->callString,
                   AEE_FONT_NORMAL,
                   AEE_FONT_NORMAL
                   );

         ISTATIC_Redraw(pSelf->m_pNumberCtl);

         IStaticScroller_SetIStatic(pSelf->m_pNumberScroller ,pSelf->m_pNumberCtl);
         IStaticScroller_StartScrolling(
             pSelf->m_pNumberScroller, 
             TIMEOUT_STATIC_TEXT_SCROLLING
             );

         AUTOMATION_LOG_WSTR(
            "Incoming:%s", 
            GetIncomingCallTransactionInfo()->callString
            );

         // KLUDGE:
         //    Redraw the softkey menu because the static control above it
         //    m_pNumberCtl has just overwritten the top 6 pixels of the
         //    softkey.
         (void) IMENUCTL_Redraw(pSkMenu);
         return TRUE;
      }

      case EVT_KEY_PRESS:
         switch ((AVKType)wParam) 
         {
            case AVK_UP:
            case AVK_DOWN:
            {
               IMenuCtl *pSkMenu = (IMenuCtl*)
                   IDIALOG_GetControl(
                      pSelf->m_pDialog,
                      IDDC_INCOMINGCALL_SK
                      );
               // Enable the no-redraw property 
               IMENUCTL_SetProperties(pSkMenu, 
                                      MP_NO_REDRAW | 
                                      IMENUCTL_GetProperties(pSkMenu));

               // Deactivate the menu control so it won't process the 
               // AVK_UP/AVK_DOWN on the EVT_KEY event.
               IMENUCTL_SetActive(pSkMenu, FALSE);
               break;
            }
         }
         break;

      case EVT_KEY:
         switch ((AVKType)wParam) 
         {
            case AVK_UP:
            case AVK_DOWN:
            {
               IMenuCtl *pSkMenu = (IMenuCtl*)
                   IDIALOG_GetControl(
                       pSelf->m_pDialog,
                       IDDC_INCOMINGCALL_SK
                       );
               // Remove the no-redraw property 
               IMENUCTL_SetProperties(pSkMenu, 
                                      ~MP_NO_REDRAW &
                                      IMENUCTL_GetProperties(pSkMenu));

               // Reactivate the menu control 
               IMENUCTL_SetActive(pSkMenu, TRUE);
            }
            return TRUE;

            case AVK_CLR:
                // do nothing!
                 return TRUE;
         }
         break;

      case EVT_COMMAND:
         switch (wParam) 
         {
            case IDDC_SK_LI_IGNORE: // Ignore the waiting call
            {
               SETUP_NEW_UI_DIALOG( GET_INCOMING_PREV_DIALOG_SETUP );
               return TRUE;
            }

            case IDDC_SK_LI_ANSWER:
            {
               const SSCall_Info_type *pSecondCallEntry;
               SSCall_Index_type       secondCallIndex = 0xff; // set to an invalid value
               boolean                 sendAnswerAndHold = FALSE; // must be init to FALSE
               SSEventData            *pEventData;
               
               pEventData = SSOEvent_AllocateEventData(SS_EVENT_DATA_CALLWISE_MSG);
               if (pEventData == NULL) return TRUE;

               pEventData->callwiseMsgData.callIndex1 = 
                   pSelf->m_IncomingDialogCallIndex;

               pSecondCallEntry = SSOperationsDialog_GetSecondCallData(
                                      pSelf, 
                                      pSelf->m_IncomingDialogCallIndex,
                                      &secondCallIndex
                                      );

               if ((pSecondCallEntry != NULL) && (!pSecondCallEntry->isHeld))
               {
                   if (pSecondCallEntry->isMPTY)
                   {
                       sendAnswerAndHold = TRUE;
                   }
                   else
                   {
                       SSCall_TInfo_type *pCallTInfo;
                       boolean            success;

                       success = SSCallDatabase_GetCallTInfoByCallIndex(
                                    pSelf->m_pCallDatabase, 
                                    secondCallIndex,
                                    &pCallTInfo
                                    );
                       assert(success == TRUE);

                       if (pCallTInfo->callStatus == SS_CALL_STATUS_ACTIVE)
                       {
                           sendAnswerAndHold = TRUE;
                       }
                   }
               }

               if (sendAnswerAndHold == TRUE)
               {
                   // there is a second Active or MPTY call
                   pEventData->callwiseMsgData.callIndex2 = secondCallIndex;

                   (void) SS_PostEvent(
                             pSelf->m_pShell,
                             AEECLSID_SS_OPERATIONS,
                             UIMN_ANSWER_AND_HOLD_REQ,
                             0,
                             (uint32) pEventData
                             );
               }
               else
               {
                   // there is no second call or it is not Active or MPTY
                   pEventData->callwiseMsgData.callIndex2 = 0;

                   (void) SS_PostEvent(
                             pSelf->m_pShell,
                             AEECLSID_SS_OPERATIONS,
                             UIMN_SETUP_RSP,
                             0,
                             (uint32) pEventData
                             );
               }

               SETUP_NEW_UI_DIALOG( 
                   SSOperationsDialog_SetupConnectingIncomingCallDialog 
                   );
               return TRUE;
            }

            case IDDC_SK_LI_REJECT:
            {
               SSMsg_msg_disconnect_type *pMsg;
               SSEventData               *pEventData;
               
               pMsg = (SSMsg_msg_disconnect_type*)
                   SSOMessage_AllocateMsg(SSMSG_MSG_TYPE_DISCONNECT);
               if (pMsg == NULL) return TRUE;

               pEventData = 
                    SSOperationsDialog_CreateTransactionWiseEventData(
                        pSelf,
                        pSelf->m_IncomingDialogCallIndex,
                        (SSMsg_int_msg_type*)pMsg
                        );

               if (pEventData == NULL) return TRUE;

               pMsg->progress_indicator.hdr.present = FALSE;
               pMsg->cause.hdr.present = TRUE;
               pMsg->cause.cause_value = SSMSG_CAUSE_VALUE_CALL_REJECTED;
               pMsg->cause.coding_standard = SSMSG_CODING_STANDARD_GSM;
               pMsg->cause.location = SSMSG_LOCATION_USER;
               pMsg->cause.diagnostic_len = 0;

               (void) SS_PostEvent(
                         pSelf->m_pShell,
                         AEECLSID_SS_OPERATIONS,
                         UIMN_DISC_REQ,
                         0,
                         (uint32) pEventData
                         );

               SETUP_NEW_UI_DIALOG( GET_INCOMING_PREV_DIALOG_SETUP );
               return TRUE;
            }

            case IDDC_SK_IL_REPLACE_CALLS:
            {
               const SSCall_Info_type *pSecondCallEntry;
               SSCall_Index_type       secondCallIndex = 0xff; // set to an invalid value
               SSEventData            *pEventData = 
                   SSOEvent_AllocateEventData(SS_EVENT_DATA_CALLWISE_MSG);
               if (pEventData == NULL) return TRUE;

               pEventData->callwiseMsgData.callIndex1 = 
                       pSelf->m_IncomingDialogCallIndex;

               pSecondCallEntry = SSOperationsDialog_GetSecondCallData(
                                     pSelf, 
                                     pSelf->m_IncomingDialogCallIndex,
                                     &secondCallIndex
                                     );

               if (pSecondCallEntry != NULL)
               {
                   pEventData->callwiseMsgData.callIndex2 = secondCallIndex;

                   (void) SS_PostEvent(
                             pSelf->m_pShell,
                             AEECLSID_SS_OPERATIONS,
                             UIMN_ANSWER_AND_RELEASE_REQ,
                             0,
                             (uint32)pEventData
                             );
               }
               else
               {
                   // no other call found - just answer the incoming call
                   pEventData->callwiseMsgData.callIndex2 = 0;

                   (void) SS_PostEvent(
                             pSelf->m_pShell,
                             AEECLSID_SS_OPERATIONS,
                             UIMN_SETUP_RSP,
                             0,
                             (uint32) pEventData
                             );
               }

               SETUP_NEW_UI_DIALOG( 
                   SSOperationsDialog_SetupConnectingIncomingCallDialog 
                   );
               return TRUE;
            }

         default:
            ASSERT_NOT_REACHABLE
            break;
      }
      break;
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_IncomingHandleNonDisplayEvent

DESCRIPTION: Incoming call dialog non dialog event handler (IDD_INCOMINGCALL)

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_IncomingHandleNonDisplayEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
   return FALSE;
}

/*===========================================================================
FUNCTION: SSOperationsDialog_SetupIncomingDialog

DESCRIPTION:
    This function creates the Incoming (call waiting) dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_SetupIncomingDialog(
    CSSOperationsDialogHandler *pSelf,
    boolean                     createNewDialog
    )
{
    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER( SSOperationsDialog_IncomingCallHandleEvent );
        SHOW_DIALOG( IDD_INCOMINGCALL );
    }

    SET_NON_DISPLAY_EVENT_HANDLER( 
        SSOperationsDialog_IncomingHandleNonDisplayEvent 
        );
}

/*=============================================================================
FUNCTION: SSOperationsDialog_SetIncomingCallName

DESCRIPTION: Used by the incoming dialog to set the call name string

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
   *pCallNameCtl [in+out]: IStatic interface pointer
   
RETURN VALUE:
    None

=============================================================================*/
static void SSOperationsDialog_SetIncomingCallName(
    CSSOperationsDialogHandler *pSelf,
    IStatic                    *pCallNameCtl
    )
{
   (void) ISTATIC_SetText(
             pCallNameCtl,
             NULL,
             GetIncomingCallTransactionInfo()->callName,
             AEE_FONT_NORMAL,
             AEE_FONT_NORMAL
             );
}

/*=============================================================================
FUNCTION: SSOperationsDialog_SetIncomingSoftkeyMenu

DESCRIPTION: Used by the incoming dialog to set the softkey options

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
   *pSkMenu [in+out]: ISoftkey interface pointer
   
RETURN VALUE:
    None

=============================================================================*/
static void SSOperationsDialog_SetIncomingSoftkeyMenu(
    CSSOperationsDialogHandler *pSelf,
    IMenuCtl                   *pSkMenu
    )
{
    const SSCall_Info_type *pSecondCallEntry = 
        SSOperationsDialog_GetSecondCallData(
            pSelf, 
            pSelf->m_IncomingDialogCallIndex,
            NULL
            );

    // Remove the "Multi-tap" softkey
    (void) IMENUCTL_DeleteAll(pSkMenu);

    (void) IMENUCTL_AddItem(pSkMenu,
                         SSOPERATIONS_RES_FILE,
                         IDS_ANSWER,
                         IDDC_SK_LI_ANSWER,
                         (AECHAR*)NULL,
                         (uint32)NULL);

    // do only if there is another valid call...
    if (pSecondCallEntry != NULL)
    {
        SSCall_TInfo_type* pCallTInfo;
        boolean addSoftkeyItems = FALSE; // must be init to FALSE
        
        if ((pSecondCallEntry->isMPTY) || (pSecondCallEntry->isHeld))
        {
            addSoftkeyItems = TRUE;
        }
        else
        {
            boolean success =
                SSCallDatabase_GetTransactionInfo(
                   pSelf->m_pCallDatabase,
                   pSecondCallEntry->transactionIndices[0],
                   &pCallTInfo
                   );
            assert(success == TRUE);

            if (pCallTInfo->callStatus == SS_CALL_STATUS_ACTIVE)
            {
                addSoftkeyItems = TRUE;
            }
        }

        if (addSoftkeyItems == TRUE)
        {
            (void) IMENUCTL_AddItem(pSkMenu,
                                    SSOPERATIONS_RES_FILE,
                                    IDS_REJECT,
                                    IDDC_SK_LI_REJECT,
                                    (AECHAR*)NULL,
                                    (uint32)NULL);

            (void) IMENUCTL_AddItem(pSkMenu,
                                    SSOPERATIONS_RES_FILE,
                                    IDS_REPLACE_CALLS,
                                    IDDC_SK_IL_REPLACE_CALLS,
                                    (AECHAR*)NULL,
                                    (uint32)NULL);
        }
    }
}

/*===========================================================================
FUNCTION SSOperationsDialog_GetSecondCallData

DESCRIPTION
    This finds the second call's call entry data, given the first 
    call's call index.

PARAMETERS:
    pSelf: Pointer to the app object
    knownCallIndex [in]: call index that is known
    newCallIndex [out]: pointer to the second call index, updated if second 
                        call is found and if it is not NULL!

RETURN VALUE:
    SSCall_Info_type: the second call's call entry, or NULL if a second call
    was not found. The pointer should NOT be freed or otherwise changed by
    the caller. 

===========================================================================*/
static const SSCall_Info_type* SSOperationsDialog_GetSecondCallData(
    CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type           knownCallIndex,
    SSCall_Index_type          *pNewCallIndex
    )
{
   SSCall_Index_type  callIndex;
   SSCall_Info_type  *pCallInfo;
   boolean            success;

   for (callIndex=0; callIndex < MAX_DATABASE_CALL_ENTRIES; ++callIndex)
   {
      if (callIndex == knownCallIndex) continue;

      success = SSCallDatabase_GetCallInfo(
                   pSelf->m_pCallDatabase, 
                   callIndex, 
                   &pCallInfo
                   );

      if (success)
      {
         if (pNewCallIndex != NULL) 
         {
            (*pNewCallIndex) = callIndex;
         }
         return pCallInfo;
      }
   }

   // no second call found
   return NULL;
}

/*===========================================================================
FUNCTION SSOperationsDialog_MoveToTheIncomingDialog

DESCRIPTION
    This function leaves the current dialog and starts the 
    Incoming (call waiting) dialog.

PARAMETERS:
    *pSelf: Pointer to the app object
    *pEventData: Pointer to the incoming call's SETUP event data. 
                 The function is responsible to free the pointer.

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_MoveToTheIncomingDialog(
    CSSOperationsDialogHandler *pSelf,
    SSEventData                *pEventData
    )
{
    AEESoundToneData   toneData;
    SSCall_TInfo_type *pCallTInfo;

    assert(pEventData != NULL);
    assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

    toneData.eTone = AEE_TONE_CALL_WT_TONE;
    toneData.wDuration = 500;  // TBD!

    ISOUND_PlayTone(pSelf->m_pSound, toneData);
    pSelf->m_IncomingDialogCallIndex = pEventData->callwiseMsgData.callIndex1;

    pCallTInfo = SSOperationsDialogHandler_GetCallTInfo(
                    pSelf, 
                    pSelf->m_IncomingDialogCallIndex
                    );
   
    SSOperationsDialog_ProcessCallNumberAndName(
       pSelf,
       pCallTInfo
       );

    // here the previous event data, containing the SETUP message, is reused
    // in order to avoid unnecessary deallocation and allocation of the event data
    // memory.

    // free the event data's old message 
    FREEIF(pEventData->callwiseMsgData.msg);
    pEventData->callwiseMsgData.msg = NULL;

    // passes the event data back to the SS operations
    (void) SS_PostEvent(
              pSelf->m_pShell,
              AEECLSID_SS_OPERATIONS,
              UIMN_ALERT_REQ,
              0,
              (uint32)pEventData
              );

    SET_INCOMING_PREV_DIALOG_SETUP_TO_CURRENT;
    SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupIncomingDialog );
}