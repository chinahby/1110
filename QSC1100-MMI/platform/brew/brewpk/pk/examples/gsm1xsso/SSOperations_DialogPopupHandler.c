/*===========================================================================
FILE: SSOperations_DialogPopupHandler.c

SERVICES:  SS Operations - Pop-up Dialog Handlers

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

#include "SSMsg.h"
#include "SSOperations_DialogHandler.h"
#include "SSOperations_DialogCommon.h"
#include "SSOperations_CommonEventDefinitions.h"
#include "SSOperations_UserEvents.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              MACRO DEFINITIONS
===========================================================================*/
#define MAX_CAUSE_STRING_LENGTH 10

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

static boolean SSOperationsDialog_HandleNonDisplayPopupEvents(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    );

static boolean SSOperationsDialog_HandleCallPopupEvents(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    );

static boolean SSOperationsDialog_HandleGeneralPopupEvents(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    );

static void SSOperationsDialog_CyclePopupDialog(
    CSSOperationsDialogHandler *pSelf
    );

static void SSOperationsDialogHandler_ReplacePopupDialog(
    CSSOperationsDialogHandler *pSelf
    );

static void SSOperationsDialog_PerformNewGeneralPopupUpdate(
    CSSOperationsDialogHandler *pSelf
    );

static void SSOperationsDialog_PerformNewCallPopupUpdate(
    CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type           callIndex,
    AECHAR                     *popupText
    );

static void SSOperationsDialog_SetNoCauseValueString(
    CSSOperationsDialogHandler *pSelf,
    AECHAR                     *pCauseValueString,
    uint8                       causeValueStringSize
    );

static void SSOperationsDialog_SetCauseValueString(
    CSSOperationsDialogHandler *pSelf,
    SSMsg_ie_cause_type        *pCauseIE,
    AECHAR                     *pCauseValueString,
    uint8                       causeValueStringSize
    );

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandlePopupEvents

DESCRIPTION: Handle a BREW event for the pop-up dialog

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
boolean SSOperationsDialogHandler_HandlePopupEvents(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    if (TRUE == SSOperationsDialog_HandleNonDisplayPopupEvents(
                   pSelf, 
                   eCode, 
                   wParam, 
                   dwParam)
       )
    {
        return TRUE;
    }

    if (TRUE == SSOperationsDialog_HandleCallPopupEvents(
                   pSelf, 
                   eCode, 
                   wParam, 
                   dwParam)
       )
    {
        return TRUE;
    }

    return SSOperationsDialog_HandleGeneralPopupEvents(
              pSelf, 
              eCode, 
              wParam, 
              dwParam
              );
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandlePopupDialogEnd

DESCRIPTION: handle the end of the current pop-up dialog

PARAMETERS:
   pSelf [in]: Pointer to the dialog handler structure

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialogHandler_HandlePopupDialogEnd(
    CSSOperationsDialogHandler *pSelf
    )
{
    assert(pSelf->m_PopupStatus != SS_NO_POPUP);

    if (pSelf->m_OverwritePopupDialog == FALSE)
    {
        SSOperationsDialogHandler_ReplacePopupDialog(pSelf);
    }
    else
    {
        // clear the overwrite flag as the pop-up dialog was not replaced
        // but redrawn again
        pSelf->m_OverwritePopupDialog = FALSE;
    }
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandleSwappingCallsPopupEvent

DESCRIPTION: Handle a pop-up event that was detected in the Swapping Calls dialog

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   eCode [in]: Specifies the Event sent.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialogHandler_HandleSwappingCallsPopupEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
   AECHAR       formatPopupText[ POPUP_FORMAT_MAX_TEXT_LEN+1 ];
   SSEventData *pEventData = (SSEventData*)dwParam;

   switch (eCode)
   {
   case UIMN_TOGGLE_CNF:
      assert(pEventData != NULL);
      assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

      (void) ISHELL_LoadResString(
                pSelf->m_pShell,
                SSOPERATIONS_RES_FILE,
                IDS_UIMN_TOGGLE_CNF,
                pSelf->m_PopupGeneralText,
                sizeof(pSelf->m_PopupGeneralText)
                );
      break;

   case UIMN_TOGGLE_REJ_IND:
      assert(pEventData != NULL);
      assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

      (void) ISHELL_LoadResString(
                pSelf->m_pShell,
                SSOPERATIONS_RES_FILE,
                IDS_UIMN_TOGGLE_REJ_IND,
                pSelf->m_PopupGeneralText,
                sizeof(pSelf->m_PopupGeneralText)
                );
      break;

   case UIMN_ERR_IND:
      assert(pEventData != NULL);
      assert(pEventData->commonData.type == SS_EVENT_DATA_ERROR);

      (void) ISHELL_LoadResString(
                pSelf->m_pShell,
                SSOPERATIONS_RES_FILE,
                IDS_UIMN_ERR_IND,
                formatPopupText,
                sizeof(formatPopupText)
                );

      // format text should contain '%d %d' if the command is to be shown!
      WSPRINTF(
         pSelf->m_PopupGeneralText,
         sizeof(pSelf->m_PopupGeneralText),
         formatPopupText,                         
         pEventData->errorData.errorType,
         pEventData->errorData.cause
         );
      break;

   default:
      assert(!"Unknown event code");
      return;
    }

    SSOperationsDialog_PerformNewGeneralPopupUpdate(pSelf);
    SSOEvent_FreeEventData((SSEventData*)dwParam);
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandleSwappingCallsPopupEvent

DESCRIPTION: Handle a pop-up event that was detected in the Building MPTY dialog

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   eCode [in]: Specifies the Event sent.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialogHandler_HandleBuildingMPTYPopupEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
   AECHAR       formatPopupText[ POPUP_FORMAT_MAX_TEXT_LEN+1 ];
   SSEventData *pEventData = (SSEventData*)dwParam;

   switch (eCode)
   {
   case UIMN_BUILD_MPTY_CNF:
      assert(pEventData != NULL);
      assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

      (void) ISHELL_LoadResString(
                pSelf->m_pShell,
                SSOPERATIONS_RES_FILE,
                IDS_UIMN_MPTY_CNF,
                pSelf->m_PopupGeneralText,
                sizeof(pSelf->m_PopupGeneralText)
                );
      break;

   case UIMN_ERR_IND:
      assert(pEventData != NULL);
      assert(pEventData->commonData.type == SS_EVENT_DATA_ERROR);

      (void) ISHELL_LoadResString(
                pSelf->m_pShell,
                SSOPERATIONS_RES_FILE,
                IDS_UIMN_ERR_IND,
                formatPopupText,
                sizeof(formatPopupText)
                );

      // format text should contain '%d %d' if the command is to be shown!
      WSPRINTF(
         pSelf->m_PopupGeneralText,
         sizeof(pSelf->m_PopupGeneralText),
         formatPopupText,                         
         pEventData->errorData.errorType,
         pEventData->errorData.cause
         );
      break;

   default:
      assert(!"Unknown event code");
      return;
    }

    SSOperationsDialog_PerformNewGeneralPopupUpdate(pSelf);
    SSOEvent_FreeEventData((SSEventData*)dwParam);
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandleEndingCallPopupEvent

DESCRIPTION: Handle a pop-up event that was detected in the Ending Call dialog

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   eCode [in]: Specifies the Event sent.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialogHandler_HandleEndingCallPopupEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    boolean eventHandled;

    // forward to the call specific event handler
    eventHandled = SSOperationsDialog_HandleCallPopupEvents(
                      pSelf,
                      eCode,
                      wParam,
                      dwParam
                      );

    assert(TRUE == eventHandled);
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandleOriginatingSecondCallPopupEvent

DESCRIPTION: Handle a pop-up event that was detected in the 
             Originating Second Call dialog

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   eCode [in]: Specifies the Event sent.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialogHandler_HandleOriginatingSecondCallPopupEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    if (eCode == UIMN_SETUP_CNF)
    {
        AECHAR       tempPopupText[ POPUP_MAX_TEXT_LEN+1 ];
        SSEventData *pEventData = (SSEventData*) dwParam;

        assert(pEventData != NULL);
        assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

        (void) ISHELL_LoadResString(
                  pSelf->m_pShell,
                  SSOPERATIONS_RES_FILE,
                  IDS_UIMN_SETUP_CNF,
                  tempPopupText,
                  sizeof(tempPopupText)
                  );

        SSOperationsDialog_PerformNewCallPopupUpdate(
           pSelf, 
           pEventData->callwiseMsgData.callIndex1,
           tempPopupText
           );

        SSOEvent_FreeEventData(pEventData);
    }
    else
    {
        boolean eventHandled;

        // forward to the call specific event handler
        eventHandled = SSOperationsDialog_HandleCallPopupEvents(
                          pSelf,
                          eCode,
                          wParam,
                          dwParam
                          );

        assert(TRUE == eventHandled);
    }
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandleConnectingIncomingCallPopupEvent

DESCRIPTION: Handle a pop-up event that was detected in the 
             Connecting Incoming Call dialog

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   eCode [in]: Specifies the Event sent.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialogHandler_HandleConnectingIncomingCallPopupEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    AECHAR       formatPopupText[ POPUP_FORMAT_MAX_TEXT_LEN+1 ];
    SSEventData *pEventData = (SSEventData*)dwParam;

    switch (eCode)
    {   
        case UIMN_ANSWER_AND_HOLD_CNF:
            assert(pEventData != NULL);
            assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

            (void) ISHELL_LoadResString(
                      pSelf->m_pShell,
                      SSOPERATIONS_RES_FILE,
                      IDS_UIMN_ANSWER_AND_HOLD_CNF,
                      pSelf->m_PopupGeneralText,
                      sizeof(pSelf->m_PopupGeneralText)
                      );
            break;

        case UIMN_ANSWER_AND_RELEASE_CNF:
            assert(pEventData != NULL);
            assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

            (void) ISHELL_LoadResString(
                      pSelf->m_pShell,
                      SSOPERATIONS_RES_FILE,
                      IDS_UIMN_ANSWER_AND_RELEASE_CNF,
                      pSelf->m_PopupGeneralText,
                      sizeof(pSelf->m_PopupGeneralText)
                      );
            break;

        case UIMN_SETUP_COMPLETE_IND:
        {
            AECHAR tempPopupText[ POPUP_MAX_TEXT_LEN+1 ];

            assert(pEventData != NULL);
            assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

            (void) ISHELL_LoadResString(
                      pSelf->m_pShell,
                      SSOPERATIONS_RES_FILE,
                      IDS_SETUP_COMPLETE_IND_FOR_ANSWER,
                      tempPopupText,
                      sizeof(tempPopupText)
                      );

            SSOperationsDialog_PerformNewCallPopupUpdate(
                pSelf, 
                pEventData->callwiseMsgData.callIndex1,
                tempPopupText);

            SSOEvent_FreeEventData(pEventData);
            return; // leave function
        }
        break;

        case UIMN_REL_IND:
        case UIMN_REL_CNF:
        {
            boolean eventHandled = 
               SSOperationsDialog_HandleCallPopupEvents(
                  pSelf,
                  eCode,
                  wParam,
                  dwParam
                  );
            assert(eventHandled == TRUE);
            return; // leave function
        }
        break;

        case UIMN_ERR_IND:
            assert(pEventData != NULL);
            assert(pEventData->commonData.type == SS_EVENT_DATA_ERROR);

            (void) ISHELL_LoadResString(
                       pSelf->m_pShell,
                       SSOPERATIONS_RES_FILE,
                       IDS_UIMN_ERR_IND,
                       formatPopupText,
                       sizeof(formatPopupText)
                       );

            // format text should contain '%d %d' if the command is to be shown!
            WSPRINTF(
                pSelf->m_PopupGeneralText,
                sizeof(pSelf->m_PopupGeneralText),
                formatPopupText,                         
                pEventData->errorData.errorType,
                pEventData->errorData.cause
                );
            break;

        default:
            assert(!"Unknown event code");
            return;
    }

    SSOperationsDialog_PerformNewGeneralPopupUpdate(pSelf);
    SSOEvent_FreeEventData((SSEventData*)dwParam);
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_ReplacePopupDialog

DESCRIPTION: close the active pop-up dialog and replace it with another pop-up

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           

RETURN VALUE:
   None

COMMENTS:
   should only be called by a pop-up dialog handler.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void SSOperationsDialogHandler_ReplacePopupDialog(
    CSSOperationsDialogHandler *pSelf
    )
{
    switch (pSelf->m_PopupStatus)
    {
    case SS_NO_POPUP:
        assert(!"invalid state");
        break;

    case SS_1ST_POPUP_ACTIVE:
        pSelf->m_Popup1DataValid = FALSE;
        break;

    case SS_2ND_POPUP_ACTIVE:
        pSelf->m_Popup2DataValid = FALSE;
        break;

    case SS_GENERAL_POPUP_ACTIVE:
        pSelf->m_PopupGeneralDataValid = FALSE;
        break;

    default:
        assert(!"unknown pop-up status value");
        return;
    }

    SSOperationsDialog_CyclePopupDialog(pSelf);
}

///////////////////////////////////////////////////////////////////////////////
//   Local Function Definitions
///////////////////////////////////////////////////////////////////////////////

/*=============================================================================
FUNCTION: SSOperationsDialog_CyclePopupDialog

DESCRIPTION: Replace the active pop-up dialog with a new one

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer

RETURN VALUE:
   None

=============================================================================*/
static void SSOperationsDialog_CyclePopupDialog(
    CSSOperationsDialogHandler *pSelf
    )
{
    switch (pSelf->m_PopupStatus)
    {
        case SS_NO_POPUP:
            assert(!"Invalid state");
            return;

        case SS_1ST_POPUP_ACTIVE:
            if (pSelf->m_Popup2DataValid) 
            {
                pSelf->m_PopupStatus = SS_2ND_POPUP_ACTIVE;
                return;
            }

            if (pSelf->m_PopupGeneralDataValid) 
            {
                pSelf->m_PopupStatus = SS_GENERAL_POPUP_ACTIVE;
                return;
            }

            if (pSelf->m_Popup1DataValid) 
            {
                pSelf->m_PopupStatus = SS_1ST_POPUP_ACTIVE;
                return;
            }
            break;

        case SS_2ND_POPUP_ACTIVE:
            if (pSelf->m_PopupGeneralDataValid) 
            {
                pSelf->m_PopupStatus = SS_GENERAL_POPUP_ACTIVE;
                return;
            }

            if (pSelf->m_Popup1DataValid) 
            {
                pSelf->m_PopupStatus = SS_1ST_POPUP_ACTIVE;
                return;
            }

            if (pSelf->m_Popup2DataValid) 
            {
                pSelf->m_PopupStatus = SS_2ND_POPUP_ACTIVE;
                return;
            }
            break;

        case SS_GENERAL_POPUP_ACTIVE:
            if (pSelf->m_Popup1DataValid) 
            {
                pSelf->m_PopupStatus = SS_1ST_POPUP_ACTIVE;
                return;
            }

            if (pSelf->m_Popup2DataValid) 
            {
                pSelf->m_PopupStatus = SS_2ND_POPUP_ACTIVE;
                return;
            }

            if (pSelf->m_PopupGeneralDataValid) 
            {
                pSelf->m_PopupStatus = SS_GENERAL_POPUP_ACTIVE;
                return;
            }
            break;

        default:
            assert(!"unknown pop-up status value");
    }

    // no new pop-up was found
    pSelf->m_PopupStatus = SS_NO_POPUP;
    pSelf->m_pPopupNonDisplayEventHandler = NULL;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_HandleNonDisplayPopupEvents

DESCRIPTION: Handle non display pop-up events

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    eCode [in]: BREW event code
    wParam [in]:
    dwParam [in]:

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_HandleNonDisplayPopupEvents(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
   if (NULL != pSelf->m_pPopupNonDisplayEventHandler)
   {
      return pSelf->m_pPopupNonDisplayEventHandler(pSelf, eCode, wParam, dwParam);
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_HandleCallPopupEvents

DESCRIPTION: Handle call pop-up events

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    eCode [in]: BREW event code
    wParam [in]:
    dwParam [in]:

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_HandleCallPopupEvents(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
   AECHAR             tempPopupText[ POPUP_MAX_TEXT_LEN+1 ];
   AECHAR             formatPopupText[ POPUP_FORMAT_MAX_TEXT_LEN+1 ];
   SSEventData       *pEventData = (SSEventData*) dwParam;
   SSCall_Index_type  callIndex1;

   switch (eCode)
   {
      // handle regular call pop-up events
      case UIMN_FACILITY_IND:
      {
         SSMsg_msg_facility_type *pMsg;
         assert(pEventData != NULL);
         assert(pEventData->commonData.type == SS_EVENT_DATA_TRANSACTIONWISE_MSG);

         callIndex1 = pEventData->transactionwiseMsgData.callIndex;
         pMsg = (SSMsg_msg_facility_type*)pEventData->transactionwiseMsgData.msg;

         assert(pMsg != NULL);
         assert(pMsg->hdr.msg_type == SSMSG_MSG_TYPE_FACILITY);
         assert(pMsg->facility.hdr.present == TRUE);
         assert(pMsg->facility.component.gen_hdr.present == TRUE);

         if (pMsg->facility.component.gen_hdr.component_type != 
             SSMSG_COMPONENT_TYPE_INVOKE)
         {
             SSOEvent_FreeEventData(pEventData);
             return TRUE;
         }

         if (pMsg->facility.component.invoke.operation_code.hdr.present == TRUE)
         {
            assert(pMsg->facility.component.invoke.operation_code.value == SSMSG_OPERATION_CODE_NOTIFYSS);
         }                

         if (pMsg->facility.component.invoke.call_on_hold_indicator.hdr.present)
         {
            switch (pMsg->facility.component.invoke.call_on_hold_indicator.value)
            {
               case SSMSG_HOLD_INDICATOR_CALL_ON_HOLD:
                  (void) ISHELL_LoadResString(
                           pSelf->m_pShell,
                           SSOPERATIONS_RES_FILE,
                           IDS_UIMN_FACILITY_IND_CALL_ON_HOLD,
                           tempPopupText,
                           sizeof(tempPopupText)
                           );
                   break;

               case SSMSG_HOLD_INDICATOR_CALL_RETRIEVED:
                  (void) ISHELL_LoadResString(
                            pSelf->m_pShell,
                            SSOPERATIONS_RES_FILE,
                            IDS_UIMN_FACILITY_IND_CALL_RETRIEVED,
                            tempPopupText,
                            sizeof(tempPopupText)
                            );
                  break;

               default:
                  assert(!"Unknown call on hold indicator value");
                       return FALSE;
            }
            break;
         }

         if (pMsg->facility.component.invoke.clir_suppression_rejected.hdr.present)
         {
            (void) ISHELL_LoadResString(
                      pSelf->m_pShell,
                      SSOPERATIONS_RES_FILE,
                      IDS_UIMN_FACILITY_IND_CLIR_SUPRESSION_REJECTED,
                      tempPopupText,
                      sizeof(tempPopupText)
                      );
            break;
         }

         if (pMsg->facility.component.invoke.call_is_waiting_indicator.hdr.present)
         {
            (void) ISHELL_LoadResString(
                      pSelf->m_pShell,
                      SSOPERATIONS_RES_FILE,
                      IDS_UIMN_FACILITY_IND_CALL_IS_WAITING,
                      tempPopupText,
                      sizeof(tempPopupText)
                      );
            break;
         }
      }
      return FALSE;

      case UIMN_ERR_IND:
      {
         assert(pEventData != NULL);
         assert(pEventData->commonData.type == SS_EVENT_DATA_ERROR);
         callIndex1 = pEventData->errorData.callIndex1;

         // only handle single call events
         if (pEventData->errorData.isCallIndex2Valid == TRUE) return FALSE;

         (void) ISHELL_LoadResString(
                   pSelf->m_pShell,
                   SSOPERATIONS_RES_FILE,
                   IDS_UIMN_ERR_IND,
                   formatPopupText,
                   sizeof(formatPopupText)
                   );

         // format text should contain '%d %d' if the command is to be shown!
         WSPRINTF(
            tempPopupText,
            sizeof(tempPopupText),
            formatPopupText,                         
            pEventData->errorData.errorType,
            pEventData->errorData.cause
            );
      }
      break;

      case UIMN_DISC_IND:
      {
         SSMsg_msg_disconnect_type *pMsg;
         AECHAR causeValueString[MAX_CAUSE_STRING_LENGTH];

         assert(pEventData != NULL);
         assert(pEventData->commonData.type == SS_EVENT_DATA_TRANSACTIONWISE_MSG);

         callIndex1 = pEventData->transactionwiseMsgData.callIndex;
         pMsg = (SSMsg_msg_disconnect_type*) pEventData->transactionwiseMsgData.msg;

         assert(pMsg != NULL);
         assert(pMsg->hdr.msg_type == SSMSG_MSG_TYPE_DISCONNECT);

         (void) ISHELL_LoadResString(
                   pSelf->m_pShell,
                   SSOPERATIONS_RES_FILE,
                   IDS_UIMN_DISC_IND,
                   formatPopupText,
                   sizeof(formatPopupText)
                   );

         SSOperationsDialog_SetCauseValueString(
             pSelf,
             &pMsg->cause, 
             causeValueString, 
             sizeof(causeValueString)
             );

         // format text should contain '%d' if the command is to be shown!
         WSPRINTF(
            tempPopupText,
            sizeof(tempPopupText),
            formatPopupText,                        
            causeValueString
            );
      }
      break;

      case UIMN_HOLD_CNF:
      {
          assert(pEventData != NULL);
          assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);
          callIndex1 = pEventData->callwiseMsgData.callIndex1;

          (void) ISHELL_LoadResString(
                    pSelf->m_pShell,
                    SSOPERATIONS_RES_FILE,
                    IDS_UIMN_HOLD_CNF,
                    tempPopupText,
                    sizeof(tempPopupText)
                    );
      }
      break;

      case UIMN_HOLD_REJ_IND:
      {
         SSMsg_msg_hold_reject_type *pMsg;
         AECHAR causeValueString[MAX_CAUSE_STRING_LENGTH];

         assert(pEventData != NULL);
         assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);
         callIndex1 = pEventData->callwiseMsgData.callIndex1;

         pMsg = (SSMsg_msg_hold_reject_type*) pEventData->callwiseMsgData.msg;

         assert(pMsg != NULL);
         assert(pMsg->hdr.msg_type == SSMSG_MSG_TYPE_HOLD_REJECT);

         (void) ISHELL_LoadResString(
                   pSelf->m_pShell,
                   SSOPERATIONS_RES_FILE,
                   IDS_UIMN_HOLD_REJ_IND,
                   formatPopupText,
                   sizeof(formatPopupText)
                   );

         SSOperationsDialog_SetCauseValueString(
            pSelf,
            &pMsg->cause, 
            causeValueString, 
            sizeof(causeValueString)
            );

         // format text should contain '%d' if the command is to be shown!
         WSPRINTF(
            tempPopupText,
            sizeof(tempPopupText),
            formatPopupText,                         
            causeValueString
            );
      }
      break;

      case UIMN_RETRIEVE_CNF:
      {
           assert(pEventData != NULL);
           assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);
           callIndex1 = pEventData->callwiseMsgData.callIndex1;

           (void) ISHELL_LoadResString(
                     pSelf->m_pShell,
                     SSOPERATIONS_RES_FILE,
                     IDS_UIMN_RETRIEVE_CNF,
                     tempPopupText,
                     sizeof(tempPopupText)
                     );
      }
      break;

      case UIMN_RETRIEVE_REJ_IND:
      {
         SSMsg_msg_retrieve_reject_type *pMsg;
         AECHAR causeValueString[MAX_CAUSE_STRING_LENGTH];

         assert(pEventData != NULL);
         assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);
         callIndex1 = pEventData->callwiseMsgData.callIndex1;
         pMsg = (SSMsg_msg_retrieve_reject_type*) pEventData->callwiseMsgData.msg;

         assert(pMsg != NULL);
         assert(pMsg->hdr.msg_type == SSMSG_MSG_TYPE_RETRIEVE_REJECT);

         (void) ISHELL_LoadResString(
                   pSelf->m_pShell,
                   SSOPERATIONS_RES_FILE,
                   IDS_UIMN_RETRIEVE_REJ_IND,
                   formatPopupText,
                   sizeof(formatPopupText)
                   );

         SSOperationsDialog_SetCauseValueString(
            pSelf,
            &pMsg->cause, 
            causeValueString, 
            sizeof(causeValueString)
            );

         // format text should contain '%d' if the command is to be shown!
         WSPRINTF(
            tempPopupText,
            sizeof(tempPopupText),
            formatPopupText,                         
            causeValueString
            );
      }
      break;

      // handle end call pop-up events
      case UIMN_REL_IND:
      {
         SSMsg_msg_release_type *pMsg; 
         AECHAR causeValueString1[MAX_CAUSE_STRING_LENGTH];
         AECHAR causeValueString2[MAX_CAUSE_STRING_LENGTH];

         assert(pEventData != NULL);
         assert(pEventData->commonData.type == SS_EVENT_DATA_TRANSACTIONWISE_MSG);
         callIndex1 = pEventData->transactionwiseMsgData.callIndex;
         pMsg = (SSMsg_msg_release_type*) pEventData->transactionwiseMsgData.msg;

         assert(pMsg != NULL);
         assert(pMsg->hdr.msg_type == SSMSG_MSG_TYPE_RELEASE);

         (void) ISHELL_LoadResString(
                   pSelf->m_pShell,
                   SSOPERATIONS_RES_FILE,
                   IDS_UIMN_RELEASE,
                   formatPopupText,
                   sizeof(formatPopupText)
                   );

         SSOperationsDialog_SetCauseValueString(
            pSelf,
            &pMsg->cause, 
            causeValueString1, 
            sizeof(causeValueString1)
            );

         SSOperationsDialog_SetCauseValueString(
            pSelf,
            &pMsg->second_cause, 
            causeValueString2, 
            sizeof(causeValueString2)
            );

         // format text should contain '%s %s' if the command is to be shown!
         WSPRINTF(
             tempPopupText,
             sizeof(tempPopupText),
             formatPopupText,                         
             causeValueString1,
             causeValueString2
            );
      }
      break;

      case UIMN_REL_CNF:
      {
         AECHAR causeValueString1[MAX_CAUSE_STRING_LENGTH];
         AECHAR causeValueString2[MAX_CAUSE_STRING_LENGTH];

         assert(pEventData != NULL);
         assert(pEventData->commonData.type == SS_EVENT_DATA_TRANSACTIONWISE_MSG);
         callIndex1 = pEventData->transactionwiseMsgData.callIndex;

         (void) ISHELL_LoadResString(
                     pSelf->m_pShell,
                     SSOPERATIONS_RES_FILE,
                     IDS_UIMN_RELEASE,
                     formatPopupText,
                     sizeof(formatPopupText)
                     );

         if (pEventData->transactionwiseMsgData.msg == NULL)
         {
            SSOperationsDialog_SetNoCauseValueString(
                pSelf,
                causeValueString1, 
                sizeof(causeValueString1)
                );

            SSOperationsDialog_SetNoCauseValueString(
                pSelf,
                causeValueString2, 
                sizeof(causeValueString2)
                );

            // format text should contain '%s %s' if the command is to be shown!
            WSPRINTF(
                tempPopupText,
                sizeof(tempPopupText),
                formatPopupText,                         
                causeValueString1,
                causeValueString2
                );

            break; // leave case ...
         }

         assert(pEventData->transactionwiseMsgData.msg != NULL);

         switch (pEventData->transactionwiseMsgData.msg->gen_hdr.msg_type)
         {
            case SSMSG_MSG_TYPE_RELEASE:
            {
               SSMsg_msg_release_type *pMsg = 
                   (SSMsg_msg_release_type*) pEventData->transactionwiseMsgData.msg;

               assert(pMsg->hdr.msg_type == SSMSG_MSG_TYPE_RELEASE);

               SSOperationsDialog_SetCauseValueString(
                  pSelf,
                  &pMsg->cause, 
                  causeValueString1, 
                  sizeof(causeValueString1)
                  );

               SSOperationsDialog_SetCauseValueString(
                  pSelf,
                  &pMsg->second_cause, 
                  causeValueString2, 
                  sizeof(causeValueString2)
                  );

               // format text should contain '%s %s' if the command is to be shown!
               WSPRINTF(
                  tempPopupText,
                  sizeof(tempPopupText),
                  formatPopupText,                         
                  causeValueString1,
                  causeValueString2
                  );
            }
            break;

            case SSMSG_MSG_TYPE_RELEASE_COMPLETE:
            {
               SSMsg_msg_release_complete_type *pMsg = 
               (SSMsg_msg_release_complete_type*) pEventData->transactionwiseMsgData.msg;

               assert(pMsg->hdr.msg_type == SSMSG_MSG_TYPE_RELEASE_COMPLETE);

               SSOperationsDialog_SetCauseValueString(
                  pSelf,
                  &pMsg->cause, 
                  causeValueString1, 
                  sizeof(causeValueString1)
                  );

               SSOperationsDialog_SetNoCauseValueString(
                  pSelf,
                  causeValueString2, 
                  sizeof(causeValueString2)
                  );

               // format text should contain '%s %s' if the command is to be shown!
               WSPRINTF(
                  tempPopupText,
                  sizeof(tempPopupText),
                  formatPopupText,                         
                  causeValueString1,
                  causeValueString2
                  );
            }
            break;

            default:
               assert(!"Bad message type value");
               return FALSE;
         }
      }
      break;

      default:
         return FALSE;
   }

   SSOperationsDialog_PerformNewCallPopupUpdate(
      pSelf, 
      callIndex1,
      tempPopupText
      );

   SSOEvent_FreeEventData(pEventData);
   return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_HandleGeneralPopupEvents

DESCRIPTION: Handle general pop-up events

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    eCode [in]: BREW event code
    wParam [in]:
    dwParam [in]:

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_HandleGeneralPopupEvents(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
   switch (eCode)
   {  
     case UIMN_DROP_TCH_IND:
         SSOEvent_FreeEventData((SSEventData*)dwParam);
         ISHELL_CloseApplet(pSelf->m_pShell, FALSE);
         return TRUE;

      default:
         return FALSE; // event not handled!
   }

   SSOperationsDialog_PerformNewGeneralPopupUpdate(pSelf);
   SSOEvent_FreeEventData((SSEventData*)dwParam);
   return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_PerformNewGeneralPopupUpdate

DESCRIPTION: handle the procedure to setup a general pop-up's new data

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer

RETURN VALUE:
   None

=============================================================================*/
static void SSOperationsDialog_PerformNewGeneralPopupUpdate(
   CSSOperationsDialogHandler *pSelf
   )
{
   GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls,"SSO:PerformNewGeneralPopupUpdate", 0, 0, 0);
   pSelf->m_PopupGeneralDataValid = TRUE;

   switch (pSelf->m_PopupStatus)
   {
      case SS_GENERAL_POPUP_ACTIVE:
         pSelf->m_OverwritePopupDialog = TRUE;
         CLOSE_ACTIVE_DIALOG;
         break;

      case SS_NO_POPUP:
         pSelf->m_PopupStatus = SS_GENERAL_POPUP_ACTIVE;
         CLOSE_ACTIVE_DIALOG;    
         break;
   }
}

/*=============================================================================
FUNCTION: SSOperationsDialog_PerformNewCallPopupUpdate

DESCRIPTION: handle the procedure to setup a general pop-up's new data

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    callIndex [in]: new pop-up's call index
   *popupText [in]: new pop-up's text buffer

RETURN VALUE:
   None

=============================================================================*/
static void SSOperationsDialog_PerformNewCallPopupUpdate(
    CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type           callIndex,
    AECHAR                     *popupText                        
    )
{
   assert(popupText != NULL);

   GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls,"SSO:PerformNewCallPopupUpdate %d", callIndex, 0, 0);

   if ( pSelf->m_Popup1DataValid && (callIndex == pSelf->m_Popup1CallIndex) )
   {
      WSTRLCPY(pSelf->m_Popup1Text, popupText, ARRAY_SIZE(pSelf->m_Popup1Text));

      switch (pSelf->m_PopupStatus)
      {
          case SS_1ST_POPUP_ACTIVE:
              pSelf->m_OverwritePopupDialog = TRUE;
              CLOSE_ACTIVE_DIALOG;
              break;

          case SS_NO_POPUP:
              pSelf->m_PopupStatus = SS_1ST_POPUP_ACTIVE;
              CLOSE_ACTIVE_DIALOG;    
              break;
      }
      return;
   }

   if ( pSelf->m_Popup2DataValid && (callIndex == pSelf->m_Popup2CallIndex) )
   {
      WSTRLCPY(pSelf->m_Popup2Text, popupText, ARRAY_SIZE(pSelf->m_Popup2Text));

      switch (pSelf->m_PopupStatus)
      {
          case SS_2ND_POPUP_ACTIVE:
              pSelf->m_OverwritePopupDialog = TRUE;
              CLOSE_ACTIVE_DIALOG;
              break;

          case SS_NO_POPUP:
              pSelf->m_PopupStatus = SS_2ND_POPUP_ACTIVE;
              CLOSE_ACTIVE_DIALOG;    
              break;
      }
      return;
   }

   if (!pSelf->m_Popup1DataValid)
   {
      WSTRLCPY(pSelf->m_Popup1Text, popupText, ARRAY_SIZE(pSelf->m_Popup1Text));

      pSelf->m_Popup1CallIndex = callIndex;
      pSelf->m_Popup1DataValid = TRUE;

      assert(pSelf->m_PopupStatus != SS_1ST_POPUP_ACTIVE);
      if (pSelf->m_PopupStatus == SS_NO_POPUP)
      {
          pSelf->m_PopupStatus = SS_1ST_POPUP_ACTIVE;
          CLOSE_ACTIVE_DIALOG;
      }
      return;
   }

   if (!pSelf->m_Popup2DataValid)
   {
      WSTRLCPY(pSelf->m_Popup2Text, popupText, ARRAY_SIZE(pSelf->m_Popup2Text));

      pSelf->m_Popup2CallIndex = callIndex;
      pSelf->m_Popup2DataValid = TRUE;

      assert(pSelf->m_PopupStatus != SS_2ND_POPUP_ACTIVE);
      if (pSelf->m_PopupStatus == SS_NO_POPUP)
      {
          pSelf->m_PopupStatus = SS_2ND_POPUP_ACTIVE;
          CLOSE_ACTIVE_DIALOG;
      }
      return;
   }

   // This should not be reached as long as there are only two possible calls
   assert(!"Unreachable Code");
}

/*=============================================================================
FUNCTION: SSOperationsDialog_SetNoCauseValueString

DESCRIPTION: Set a string buffer to contain a string that represents 
             the case where the cause Information element is NOT present.

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   pCauseValueString [out]: Pointer to string.
   causeValueStringSize [in]: size of the output string, in bytes. 

RETURN VALUE:
   None

=============================================================================*/
static void SSOperationsDialog_SetNoCauseValueString(
    CSSOperationsDialogHandler *pSelf,
    AECHAR                     *pCauseValueString,
    uint8                       causeValueStringSize
    )
{
   (void) ISHELL_LoadResString(
             pSelf->m_pShell,
             SSOPERATIONS_RES_FILE,
             IDS_NO_CAUSE_STRING,
             pCauseValueString,
             causeValueStringSize
             );
}

/*=============================================================================
FUNCTION: SSOperationsDialog_SetCauseValueString

DESCRIPTION: Set a string buffer to contain a cause information element value's
             string representation.

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   pCauseIE [in]: Pointer to the cause Information element.
   pCauseValueString [out]: Pointer to string.
   causeValueStringSize [in]: size of the output string, in bytes. 

RETURN VALUE:
   None

=============================================================================*/
static void SSOperationsDialog_SetCauseValueString(
    CSSOperationsDialogHandler *pSelf,
    SSMsg_ie_cause_type        *pCauseIE,
    AECHAR                     *pCauseValueString,
    uint8                       causeValueStringSize
    )
{
    AECHAR digitFormat[3] = { '%', 'd', '\0' };

    assert(pCauseIE != NULL);
    assert(pCauseValueString != NULL);

    if (pCauseIE->hdr.present)
    {
        WSPRINTF(
            pCauseValueString,
            causeValueStringSize,
            digitFormat,
            pCauseIE->cause_value
            );
    }
    else
    {
        SSOperationsDialog_SetNoCauseValueString(
            pSelf,
            pCauseValueString,
            causeValueStringSize
            );
    }
}
