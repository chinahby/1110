/*=========================================================================
FILE: SSOperations_MN.c

SERVICES: Provides the MN layer of the GSM1x SS Operations State Machine

GENERAL DESCRIPTION:
    The MN layer is the upper layer of the State Machine which interacts     
    with the SSO UI. The MN also interact with the lower CC layer.
    The MN is responsible of handling operations that demand coordination      
    between two calls (e.g. toggle).
    SSOUI-FSM interface primitives that are not handled by the MN, are
    forwarded to the appropriate layer, either to the UI or to the CC.
    Events in the SSOUI-FSM are delivered using ISHELL_PostEvent. Events in the
    MN-CC interface are delivered through the EventQueue which is maintained by
    the FSM.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "SSOperations_MN.h"
#include "SSOperations_ClassId.h"
#include "SSOperations_UserEvents.h"
#include "SSCallDatabase.h"
#include "SS_CommonMacros.h"
#include "SSOperations_CommonEventDefinitions.h"
#include "SSOperations_FSM.h"
#include "SSOperations_Toggle.h"
#include "SSOperations_CommonMessageDefinitions.h"

#include "SSUtil.h"                         // SSUtil Class header

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// This macro is used to set a new state handler
#define MOVE_TO_STATE(newStateHandler) \
{ \
    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:MOVE_TO_STATE: " #newStateHandler, 0, 0, 0); \
    pMe->m_pCurrentStateHandler = newStateHandler; \
}

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

// Event handler for events that are common to all MN states
boolean SSOperationsMN_CommonEventHandler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

// Event handlers for every state in the MN state machine

// Ready state
static boolean SSOperationsMN_ReadyState_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

// Answer and Hold states
static boolean SSOperationsMN_AH_WaitForRelease2State_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsMN_AH_WaitForReleaseHoldState_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsMN_AH_WaitForHoldCnf1State_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsMN_AH_WaitForHoldCnf2State_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsMN_AH_WaitForSetupCompl1State_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsMN_AH_WaitForRelease1State_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsMN_AH_WaitForSetupCompl2State_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

// Answer and Release states
static boolean SSOperationsMN_AR_WaitForReleaseState_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsMN_AR_WaitForSetupComplState_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

// Toggle states
static boolean SSOperationsMN_Toggle_WaitFor2State_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsMN_Toggle_WaitFor1State_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

// MPTY states
static boolean SSOperationsMN_MPTY_WaitForMPTYCnfState_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsMN_MPTY_WaitForMergeCnfState_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*=============================================================================
FUNCTION: SSOperationsMN_New

DESCRIPTION: Create an MN object

PARAMETERS:
    pIShell [in]: Pointer to the IShell instance

RETURN VALUE:
    Pointer to the created SSOperationsMN or NULL on failure.

COMMENTS:
    in order to free SSOperationsMN object, use SSOperationsMN_Release.
    This function should be called only once in a life time of a client.
=============================================================================*/
CSSOperationsMN* SSOperationsMN_New(
    IShell* pIShell
    )
{
    CSSOperationsMN* obj;

    obj = MALLOC(sizeof(CSSOperationsMN));
    if (obj == NULL)
        return NULL;

    obj->m_pIShell = pIShell;

    ISHELL_AddRef(pIShell);

    return obj;
}

/*=============================================================================
FUNCTION: SSOperationsMN_Initialize

DESCRIPTION: Initialize an MN object

PARAMETERS:
    pMe [in]          : Pointer to the MN object to be initialized.
    parent [in]       : Pointer to the FSM object.
    pCallDatabase [in]: Pointer to a valid CallDatabase instance

RETURN VALUE:
    TRUE on success or FALSE on failure.
=============================================================================*/
boolean SSOperationsMN_Initialize(
    CSSOperationsMN  *pMe,
    CSSOperationsFSM *parent,
    SSCallDatabase   *pCallDatabase
    )
{
    pMe->m_pFSM = parent;
    pMe->m_pToggle = NULL;
    pMe->m_pCallDatabase = pCallDatabase;
    // create the SS utilities class
    pMe->m_pSSUtilCls = NULL;
    if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_SSUTIL, (void **)&pMe->m_pSSUtilCls) != SUCCESS)
        return FALSE;

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: SSUtil created by MN", 0, 0, 0);

    // get a IPhone interface
    pMe->m_pIPhone = NULL;
    if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_PHONE, (void **)&pMe->m_pIPhone) != SUCCESS)
        return FALSE;

    pMe->m_errorCause = SS_ERROR_CAUSE_UNSPECIFIED;
    
    // initialize the MN state machine to the Ready state
    MOVE_TO_STATE( SSOperationsMN_ReadyState_Handler );

    return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsFSM_Release

DESCRIPTION: Destroy an FSM object.

PARAMETERS:
     pMe [in]: Pointer to the FSM object to be destroyed.

RETURN VALUE:
    None

COMMENTS:
    The FSM does not maintain a reference counter. Calling Release always
    destroy the given object.
=============================================================================*/
void SSOperationsMN_Release( CSSOperationsMN *pMe )
{
    if (pMe->m_pToggle != NULL)
    {
        SSOperationsToggle_Release(pMe->m_pToggle);
        pMe->m_pToggle = NULL;
    }

    // release the SS utilities class
    if (pMe->m_pSSUtilCls != NULL)
    {
        ISSUTILCLS_Release(pMe->m_pSSUtilCls);
        pMe->m_pSSUtilCls = NULL;
    }

    // release the IPhone interface
    if (pMe->m_pIPhone != NULL)
    {
        IPHONE_Release(pMe->m_pIPhone);
        pMe->m_pIPhone = NULL;
    }

    ISHELL_Release(pMe->m_pIShell);

    FREE(pMe);
}

///////////////////////////////////////////////////////////////////////////////
//   Local Function Definitions
///////////////////////////////////////////////////////////////////////////////

/*=============================================================================
FUNCTION: SSOperationsMN_PostErrorEvent

DESCRIPTION:
    Set an SSEventErrorData with given values and send it as a UIMN_ERR_IND
    to the UI.
    eventData is allocated by the caller.
   
PARAMETERS:
    pMe   [in]     : Pointer to the MN object.
    eventData [in] : event data placeholder. allocated by the caller and filled
                     by this function.
    callIndex1, callIndex2, isCallIndex2Valid, errorType, errorCause [in] :
        values for the event data.

RETURN VALUE:
    None.
=============================================================================*/
void SSOperationsMN_PostErrorEvent(
    CSSOperationsMN      *pMe,
    SSEventData          *eventData,
    SSCall_Index_type     callIndex1,
    SSCall_Index_type     callIndex2,
    boolean               isCallIndex2Valid,
    SSEventErrorType      errorType,
    SSEventErrorCauseType errorCause
    )
{
    eventData->commonData.type = SS_EVENT_DATA_ERROR;
    eventData->errorData.callIndex1 = callIndex1;
    eventData->errorData.callIndex2 = callIndex2;
    eventData->errorData.isCallIndex2Valid = isCallIndex2Valid;
    eventData->errorData.errorType = errorType;
    eventData->errorData.cause = errorCause;
    
    SS_PostEvent(
        pMe->m_pIShell, 
        AEECLSID_SS_OPERATIONS, 
        UIMN_ERR_IND, 
        0, 
        (uint32)eventData
        );
}

/*=============================================================================
FUNCTION: SSOperationsMN_CheckEventData

DESCRIPTION:
    Check some parameters in a given event data.

PARAMETERS:
    pMe   [in]             : pointer to an MN object.
    eventData [in]         : the event data to check
    eventDataType [in]     : the expected type of the event data
    expectedCallIndex [in] : expected call index in the event data. applicable
                             only to SS_EVENT_DATA_CALLWISE_MSG and 
                             SS_EVENT_DATA_TRANSACTIONWISE_MSG types.

RETURN VALUE:
    TRUE if the event data was found valid or FALSE otherwise.

COMMENTS:
    Only SS_EVENT_DATA_CALLWISE_MSG and SS_EVENT_DATA_TRANSACTIONWISE_MSG types
    are currently expected. The reason being that no checking is needed for
    the parameters of other event data types.
=============================================================================*/
boolean SSOperationsMN_CheckEventData(
    CSSOperationsMN *pMe,
    SSEventData* eventData, 
    SSEventDataType eventDataType,
    SSCall_Index_type expectedCallIndex)
{

    assert(eventData->commonData.type == eventDataType);

    switch(eventDataType)
    {
    case SS_EVENT_DATA_CALLWISE_MSG:
        return (eventData->callwiseMsgData.callIndex1 == expectedCallIndex);

    case SS_EVENT_DATA_TRANSACTIONWISE_MSG:
        return (eventData->transactionwiseMsgData.callIndex == expectedCallIndex);

    case SS_EVENT_DATA_DATA_IND:
    case SS_EVENT_DATA_ERROR:
        // these event data types are not currently expected.
	    ASSERT_NOT_REACHABLE;
        return TRUE;
    }

    ASSERT_NOT_REACHABLE;
    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsMN_QueueEvent

DESCRIPTION: queues an event onto the MN-CC queue. may be called from the
             MN or from the Toggle SM.

PARAMETERS:
   pMe   [in] : Pointer to the MN object.
   eCode [in] : Specifies the Event to queue.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None
=============================================================================*/
void SSOperationsMN_QueueEvent(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
   )
{
    // forward it to the parent's QueueEvent
    SSOperationsFSM_QueueEvent(pMe->m_pFSM, eCode, wParam, dwParam);
}

/*=============================================================================
FUNCTION: SSOperationsMN_HandleEvent

DESCRIPTION: Entry point for events to the MN.

PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled, FALSE otherwise.
=============================================================================*/
boolean SSOperationsMN_HandleEvent(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
)
{
    assert (NULL != pMe->m_pCurrentStateHandler);
 
    // send the event to the current state handler. if it was not handled
    // there, send it to the common event handler.
    if (FALSE == pMe->m_pCurrentStateHandler(pMe, eCode, wParam, dwParam))
    {
        return SSOperationsMN_CommonEventHandler(pMe, eCode, wParam, dwParam);
    }
    else
        return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsMN_CommonEventHandler

DESCRIPTION: Handle events that are common to all MN states

PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled, FALSE otherwise.
=============================================================================*/
boolean SSOperationsMN_CommonEventHandler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
)
{
    AEEEvent newECode = INITIAL_USER_EVENT_ECODE_VALUE;

    switch(eCode)
    {
        // Events from the UI or from the CC that wasn't handled by the MN 
        // state machine are transparently forwarded to the CC or to the UI
        // respectively (while changing their prefix to indicate the correct 
        // interface).

        // Applicable events in the UI => CC direction are the intersection 
        // of UI => MN and MN => CC events
    case UIMN_SETUP_REQ:
        newECode = MNCC_SETUP_REQ;
        break;
    case UIMN_SETUP_RSP:
        newECode = MNCC_SETUP_RSP;
        break;
    case UIMN_ALERT_REQ:
        newECode = MNCC_ALERT_REQ;
        break;
    case UIMN_DISC_REQ:
        newECode = MNCC_DISC_REQ;
        break;
    case UIMN_BUILD_MPTY_REQ:
        newECode = MNCC_BUILD_MPTY_REQ;
        break;
    case UIMN_HOLD_REQ:
        newECode = MNCC_HOLD_REQ;
        break;
    case UIMN_RETRIEVE_REQ:
        newECode = MNCC_RETRIEVE_REQ;
        break;
        // Applicable events in the CC => UI direction are the intersection 
        // of CC => MN and MN => UI events
    case MNCC_SETUP_IND:
        newECode = UIMN_SETUP_IND;
        break;
    case MNCC_SETUP_CNF:
        newECode = UIMN_SETUP_CNF;
        break;
    case MNCC_SETUP_COMPLETE_IND:
        newECode = UIMN_SETUP_COMPLETE_IND;
        break;
    case MNCC_DISC_IND:
        newECode = UIMN_DISC_IND;
        break;
    case MNCC_REL_IND:
        newECode = UIMN_REL_IND;
        break;
    case MNCC_REL_CNF:
        newECode = UIMN_REL_CNF;
        break;
    case MNCC_BUILD_MPTY_CNF:
        newECode = UIMN_BUILD_MPTY_CNF;
        break;
    case MNCC_FACILITY_IND:
        newECode = UIMN_FACILITY_IND;
        break;
    case MNCC_HOLD_CNF:
        newECode = UIMN_HOLD_CNF;
        break;
    case MNCC_HOLD_REJ_IND:
        newECode = UIMN_HOLD_REJ_IND;
        break;
    case MNCC_RETRIEVE_CNF:
        newECode = UIMN_RETRIEVE_CNF;
        break;
    case MNCC_RETRIEVE_REJ_IND:
        newECode = UIMN_RETRIEVE_REJ_IND;
        break;
    case MNCC_ERR_IND:
        newECode = UIMN_ERR_IND;
        break;
    case MNCC_DROP_TCH_IND:
        newECode = UIMN_DROP_TCH_IND;
        break;
    }

    if (newECode != INITIAL_USER_EVENT_ECODE_VALUE)
    {
        assert ((newECode > EVT_UIMN_FIRST) && (newECode < EVT_MNCC_LAST));
        // send the translated event to the UI or to the CC
        if ((newECode > EVT_UIMN_FIRST) && (newECode < EVT_UIMN_LAST))
            SS_PostEvent(
                pMe->m_pIShell, 
                AEECLSID_SS_OPERATIONS, 
                newECode, 
                wParam, 
                dwParam
                );
        else
            SSOperationsFSM_QueueEvent(pMe->m_pFSM, newECode, wParam, dwParam);

        return TRUE;
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//   State Handlers Definitions
///////////////////////////////////////////////////////////////////////////////

/*=============================================================================
FUNCTION: SSOperationsMN_ReadyState_Handler

DESCRIPTION:
   This is the Ready state event handler. The Ready state is the "idle" state
   of the MN. Every MN procedure starts from this state and ends at it.
   
PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMN_ReadyState_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
)
{
    SSCall_TInfo_type* pTInfo;

    switch(eCode)
    {
    case UIMN_ANSWER_AND_HOLD_REQ:
        {
            SSEventData *answerAndHoldData, *holdData, *outgoingEventData;
            SSCall_Index_type activeCallIndex, waitingCallIndex;
            boolean                callExists;

            // dwParam holds an SSEventData
            answerAndHoldData = (SSEventData*)dwParam;
            assert (answerAndHoldData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);
            
            waitingCallIndex = answerAndHoldData->callwiseMsgData.callIndex1;
            activeCallIndex = answerAndHoldData->callwiseMsgData.callIndex2;
            // store the indexes in the MN struct
            pMe->m_ciCallToAnswer = waitingCallIndex;
            pMe->m_ciCallToHold = activeCallIndex;
            
            // insure non MPTY, active call and waiting call.
            callExists = SSCallDatabase_GetCallTInfoByCallIndex(pMe->m_pCallDatabase, waitingCallIndex, &pTInfo);
            if (!callExists || 
                (pTInfo->callState.mainState != SSCallState_CALL_RECEIVED))
            {
                // bad call indices
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:AandH: call %d is not waiting", waitingCallIndex, 0, 0);
                // inform UI. reuse answerAndHoldData memory
                outgoingEventData = answerAndHoldData;
                SSOperationsMN_PostErrorEvent(pMe, 
                    outgoingEventData,
                    waitingCallIndex,
                    activeCallIndex,
                    TRUE,
                    SS_ERROR_UI_REQ_REJECTED,
                    SS_ERROR_CAUSE_CALLINDEX1_REJECTED);
                
                return TRUE;
            }
            
            callExists = SSCallDatabase_GetCallTInfoByCallIndex(pMe->m_pCallDatabase, activeCallIndex, &pTInfo);
            if (!callExists || 
                (pTInfo->callState.mainState != SSCallState_ACTIVE) ||
                (pTInfo->callState.holdState != SSCallHoldState_IDLE))
            {
                // bad call indices
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:AandH: call %d is not active", activeCallIndex, 0, 0);
                // inform UI. reuse answerAndHoldData memory
                outgoingEventData = answerAndHoldData;
                SSOperationsMN_PostErrorEvent(pMe, 
                    outgoingEventData,
                    waitingCallIndex,
                    activeCallIndex,
                    TRUE,
                    SS_ERROR_UI_REQ_REJECTED,
                    SS_ERROR_CAUSE_CALLINDEX2_REJECTED);

                return TRUE;
            }

            // appropriate calls found. send command to the CC.
            // reuse answerAndHoldData memory
            holdData = answerAndHoldData;
            holdData->callwiseMsgData.callIndex1 = activeCallIndex;
            SSOperationsFSM_QueueEvent(pMe->m_pFSM, MNCC_HOLD_REQ, 0, (uint32)holdData);
            
            MOVE_TO_STATE( SSOperationsMN_AH_WaitForHoldCnf1State_Handler );
            
            return TRUE;
        }
    case UIMN_ANSWER_AND_RELEASE_REQ:
        {
            SSEventData *answerAndReleaseData, *disconnectData, *outgoingEventData;
            SSCall_Index_type existingCallIndex, waitingCallIndex;
            boolean callExists, result;

            // dwParam holds an SSEventData
            answerAndReleaseData = (SSEventData*)dwParam;
            assert (answerAndReleaseData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);
            
            waitingCallIndex = answerAndReleaseData->callwiseMsgData.callIndex1;
            existingCallIndex = answerAndReleaseData->callwiseMsgData.callIndex2;
            // store the indexes in the MN struct
            pMe->m_ciCallToAnswer = waitingCallIndex;
            pMe->m_ciCallToRelease = existingCallIndex;
            
            // insure non MPTY, active call and waiting call.
            callExists = SSCallDatabase_GetCallTInfoByCallIndex(pMe->m_pCallDatabase, waitingCallIndex, &pTInfo);
            if (!callExists || 
                (pTInfo->callState.mainState != SSCallState_CALL_RECEIVED))
            {
                // bad call indices
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:AandR: call %d is not waiting", waitingCallIndex, 0, 0);
                // inform UI. reuse answerAndReleaseData memory
                outgoingEventData = answerAndReleaseData;
                SSOperationsMN_PostErrorEvent(pMe, 
                    outgoingEventData,
                    waitingCallIndex,
                    existingCallIndex,
                    TRUE,
                    SS_ERROR_UI_REQ_REJECTED,
                    SS_ERROR_CAUSE_CALLINDEX1_REJECTED);

                return TRUE;
            }
            callExists = SSCallDatabase_GetCallTInfoByCallIndex(pMe->m_pCallDatabase, existingCallIndex, &pTInfo);
            if (!callExists)
            {
                // bad call indices
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:AandR: call %d does not exist", existingCallIndex, 0, 0);
                // inform UI. reuse answerAndReleaseData memory
                outgoingEventData = answerAndReleaseData;
                SSOperationsMN_PostErrorEvent(pMe, 
                    outgoingEventData,
                    waitingCallIndex,
                    existingCallIndex,
                    TRUE,
                    SS_ERROR_UI_REQ_REJECTED,
                    SS_ERROR_CAUSE_CALLINDEX2_REJECTED);

                return TRUE;
            }

            // appropriate calls found. send command to the CC.
            disconnectData = 
                SSOEvent_PrepareTransactionwiseMsgEvent(existingCallIndex, 0, NULL);
            result = SSCallDatabase_GetTInfoIndexByCallIndex(
                pMe->m_pCallDatabase, 
                existingCallIndex, 
                &disconnectData->transactionwiseMsgData.tInfoIndex);
            assert (result == TRUE);

            SSOperationsFSM_QueueEvent(pMe->m_pFSM, MNCC_DISC_REQ, 0, (uint32)disconnectData);

            SSOEvent_FreeEventData(answerAndReleaseData);
            
            MOVE_TO_STATE( SSOperationsMN_AR_WaitForReleaseState_Handler );
            
            return TRUE;
        }

    case UIMN_TOGGLE_REQ:
        {
            SSEventData *toggleData, *holdData, *retrieveData, *outgoingEventData;
            SSCall_Index_type activeCallIndex, heldCallIndex;
            boolean                callExists;

            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: MN Got Toggle request", 0, 0, 0);

            // dwParam holds an SSEventData
            toggleData = (SSEventData*)dwParam;
            assert (toggleData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);
            
            activeCallIndex = toggleData->callwiseMsgData.callIndex1;
            heldCallIndex = toggleData->callwiseMsgData.callIndex2;

            // store the indexes in the MN struct
            pMe->m_ciCallToHold = activeCallIndex;
            pMe->m_ciCallToRetrieve = heldCallIndex;

            // insure non MPTY, active call and held call.
            callExists = SSCallDatabase_GetCallTInfoByCallIndex(pMe->m_pCallDatabase, heldCallIndex, &pTInfo);
            if (!callExists || 
                (pTInfo->callState.mainState != SSCallState_ACTIVE) ||
                (pTInfo->callState.holdState != SSCallHoldState_CALL_HELD))
            {
                // bad call indices
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: Toggle: call %d is not held", heldCallIndex, 0, 0);
                // inform UI. reuse toggleData memory
                outgoingEventData = toggleData;
                SSOperationsMN_PostErrorEvent(pMe, 
                    outgoingEventData,
                    activeCallIndex,
                    heldCallIndex,
                    TRUE,
                    SS_ERROR_UI_REQ_REJECTED,
                    SS_ERROR_CAUSE_CALLINDEX2_REJECTED);
                
                return TRUE;
            }
            callExists = SSCallDatabase_GetCallTInfoByCallIndex(pMe->m_pCallDatabase, activeCallIndex, &pTInfo);
            if (!callExists || 
                (pTInfo->callState.mainState != SSCallState_ACTIVE) ||
                (pTInfo->callState.holdState != SSCallHoldState_IDLE))
            {
                // bad call indices
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:Toggle: call %d is not active", activeCallIndex, 0, 0);
                // inform UI. reuse toggleData memory
                outgoingEventData = toggleData;
                SSOperationsMN_PostErrorEvent(pMe, 
                    outgoingEventData,
                    activeCallIndex,
                    heldCallIndex,
                    TRUE,
                    SS_ERROR_UI_REQ_REJECTED,
                    SS_ERROR_CAUSE_CALLINDEX1_REJECTED);

                return TRUE;
            }

            assert(pMe->m_pToggle == NULL);

            // create the Toggle object
            if ((pMe->m_pToggle = SSOperationsToggle_New(pMe->m_pIShell, pMe, pMe->m_pCallDatabase)) == NULL)
            {
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:Failed to create Toggle object", 0, 0, 0);
                return TRUE;
            }
            if (SSOperationsToggle_Initialize(pMe->m_pToggle) == FALSE)
            {
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:Failed to initialize Toggle object", 0, 0, 0);
                return FALSE;
            }

            // appropriate calls found. send commands to the CC.
            holdData = SSOEvent_PrepareCallwiseMsgEvent(activeCallIndex, 0, NULL);
            if (holdData == NULL)
            {
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:Toggle: Failed to allocate Msg Event", 0, 0, 0);
                return TRUE;
            }

            retrieveData = SSOEvent_PrepareCallwiseMsgEvent(heldCallIndex, 0, NULL);
            if (retrieveData == NULL)
            {
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:Toggle: Failed to allocate Msg Event", 0, 0, 0);
                return TRUE;
            }
            // queue the events
            SSOperationsFSM_QueueEvent(pMe->m_pFSM, MNCC_HOLD_REQ, 0, (int32)holdData);
            SSOperationsFSM_QueueEvent(pMe->m_pFSM, MNCC_RETRIEVE_REQ, 0, (int32)retrieveData);

            SSOEvent_FreeEventData(toggleData);
            
            MOVE_TO_STATE( SSOperationsMN_Toggle_WaitFor2State_Handler );

            return TRUE;
        }

    case UIMN_BUILD_MPTY_REQ:
        {
            SSEventData *buildMPTYData, *outgoingEventData;
            SSCall_Index_type toCallIndex, fromCallIndex;
            SSCall_TInfo_type *pTInfo1, *pTInfo2;
            boolean           callExists;

            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: MN Got Build MPTY request", 0, 0, 0);

            // dwParam holds an SSEventData
            buildMPTYData = (SSEventData*)dwParam;
            assert (buildMPTYData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);
            
            toCallIndex = buildMPTYData->callwiseMsgData.callIndex1;
            fromCallIndex = buildMPTYData->callwiseMsgData.callIndex2;

            // store the index in the MN struct
            pMe->m_ciBuildMPTYHandler = fromCallIndex;

            // insure calls exist
            callExists = SSCallDatabase_GetCallTInfoByCallIndex(pMe->m_pCallDatabase, toCallIndex, &pTInfo1);
            if (!callExists)
            {
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:Build MPTY: call %d is invalid", toCallIndex, 0, 0);
                // inform UI. reuse buildMPTYData memory
                outgoingEventData = buildMPTYData;
                SSOperationsMN_PostErrorEvent(pMe, 
                    outgoingEventData,
                    toCallIndex,
                    fromCallIndex,
                    TRUE,
                    SS_ERROR_UI_REQ_REJECTED,
                    SS_ERROR_CAUSE_CALLINDEX1_REJECTED);

                return TRUE;
            }
            callExists = SSCallDatabase_GetCallTInfoByCallIndex(pMe->m_pCallDatabase, fromCallIndex, &pTInfo2);
            if (!callExists)
            {
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:Build MPTY: call %d is invalid", fromCallIndex, 0, 0);
                // inform UI. reuse buildMPTYData memory
                outgoingEventData = buildMPTYData;
                SSOperationsMN_PostErrorEvent(pMe, 
                    outgoingEventData,
                    toCallIndex,
                    fromCallIndex,
                    TRUE,
                    SS_ERROR_UI_REQ_REJECTED,
                    SS_ERROR_CAUSE_CALLINDEX2_REJECTED);

                return TRUE;
            }
            // insure calls are active and not in MPTY
            if ((pTInfo1->callState.mainState != SSCallState_ACTIVE) ||
                (pTInfo2->callState.mainState != SSCallState_ACTIVE) ||
                (pTInfo1->callState.mptyState != SSCallMPTYState_IDLE) ||
                (pTInfo2->callState.mptyState != SSCallMPTYState_IDLE))
            {
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:Build MPTY: calls %d,%d are not in valid state", toCallIndex, fromCallIndex, 0);
                // inform UI. reuse buildMPTYData memory
                outgoingEventData = buildMPTYData;
                SSOperationsMN_PostErrorEvent(pMe, 
                    outgoingEventData,
                    toCallIndex,
                    fromCallIndex,
                    TRUE,
                    SS_ERROR_UI_REQ_REJECTED,
                    SS_ERROR_CAUSE_CALLINDICES_REJECTED);

                return TRUE;
            }
            // insure one call is active and the other one is on hold
            if (!(((pTInfo1->callState.holdState == SSCallHoldState_CALL_HELD) &&
                (pTInfo2->callState.holdState == SSCallHoldState_IDLE)) ||
                ((pTInfo1->callState.holdState == SSCallHoldState_IDLE) &&
                (pTInfo2->callState.holdState == SSCallHoldState_CALL_HELD))))
            {
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:Build MPTY: calls %d,%d are not in valid state", toCallIndex, fromCallIndex, 0);
                // inform UI. reuse buildMPTYData memory
                outgoingEventData = buildMPTYData;
                SSOperationsMN_PostErrorEvent(pMe, 
                    outgoingEventData,
                    toCallIndex,
                    fromCallIndex,
                    TRUE,
                    SS_ERROR_UI_REQ_REJECTED,
                    SS_ERROR_CAUSE_CALLINDICES_REJECTED);

                return TRUE;
            }

            // appropriate calls found. send commands to the CC.
            outgoingEventData = buildMPTYData;

            // queue the event
            SSOperationsFSM_QueueEvent(pMe->m_pFSM, MNCC_BUILD_MPTY_REQ, 0, (int32)outgoingEventData);
            
            MOVE_TO_STATE( SSOperationsMN_MPTY_WaitForMPTYCnfState_Handler );

            return TRUE;
        }
    }

    return FALSE;
}

/******************************************************************************
//   Answer and Hold state handlers
******************************************************************************/

/*=============================================================================
FUNCTION: SSOperationsMN_AH_WaitForRelease2State_Handler

DESCRIPTION:
   This is the WaitForRelease2 state event handler, which is part of the 
   Answer and Hold procedure.
   
PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMN_AH_WaitForRelease2State_Handler(
    CSSOperationsMN *pMe,
    AEEEvent         eCode,
    uint16           wParam,
    uint32           dwParam
)
{
    SSEventData *incomingEventData, *outgoingEventData;

    switch(eCode)
    {
    case MNCC_REL_CNF:
    case MNCC_REL_IND:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        if (SSOperationsMN_CheckEventData(
                pMe, 
                incomingEventData, 
                SS_EVENT_DATA_TRANSACTIONWISE_MSG, 
                pMe->m_ciCallToAnswer) == FALSE)
        {
            return FALSE;
        }

        // reuse the incomingEventData memory
        FREEIF(incomingEventData->transactionwiseMsgData.msg);
        outgoingEventData = incomingEventData;
        outgoingEventData->commonData.type = SS_EVENT_DATA_ERROR;
        outgoingEventData->errorData.callIndex1 = pMe->m_ciCallToAnswer;
        outgoingEventData->errorData.callIndex2 = pMe->m_ciCallToHold;
        outgoingEventData->errorData.isCallIndex2Valid = TRUE;
        outgoingEventData->errorData.errorType = SS_ERROR_ANSWER_AND_HOLD;
        outgoingEventData->errorData.cause = SS_ERROR_CAUSE_WAITING_CALL_DISCONNECTED;

        SS_PostEvent(
            pMe->m_pIShell, 
            AEECLSID_SS_OPERATIONS, 
            UIMN_ERR_IND, 
            0, 
            (uint32)outgoingEventData
            );
        
        MOVE_TO_STATE( SSOperationsMN_ReadyState_Handler );
        return TRUE;
	}

	return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsMN_AH_WaitForReleaseHoldState_Handler

DESCRIPTION:
   This is the WaitForReleaseHold state event handler, which is part of the 
   Answer and Hold procedure.
   
PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMN_AH_WaitForReleaseHoldState_Handler(
    CSSOperationsMN *pMe,
    AEEEvent         eCode,
    uint16           wParam,
    uint32           dwParam
)
{
    SSEventData *incomingEventData;

    switch(eCode)
    {
    case MNCC_REL_IND:
    case MNCC_REL_CNF:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;

		if (SSOperationsMN_CheckEventData(
                pMe, 
                incomingEventData, 
                SS_EVENT_DATA_TRANSACTIONWISE_MSG, 
                pMe->m_ciCallToAnswer) == TRUE) // TI==CW
        {
	        SSOEvent_FreeEventData(incomingEventData); 
			MOVE_TO_STATE( SSOperationsMN_AH_WaitForHoldCnf2State_Handler );
			return TRUE;           
        }
		
		return FALSE;

    case MNCC_HOLD_CNF:
    case MNCC_HOLD_REJ_IND:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;

		if (SSOperationsMN_CheckEventData(
                pMe, 
                incomingEventData, 
                SS_EVENT_DATA_CALLWISE_MSG, 
                pMe->m_ciCallToHold) == TRUE) // TI==Active
        {
	        SSOEvent_FreeEventData(incomingEventData); 
			MOVE_TO_STATE( SSOperationsMN_AH_WaitForRelease2State_Handler );
			return TRUE;           
        }
		
		return FALSE;
	}
	return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsMN_AH_WaitForHoldCnf2State_Handler

DESCRIPTION:
   This is the WaitForHoldCnf2 state event handler, which is part of the 
   Answer and Hold procedure.
   
PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMN_AH_WaitForHoldCnf2State_Handler(
    CSSOperationsMN *pMe,
    AEEEvent         eCode,
    uint16           wParam,
    uint32           dwParam
)
{
   SSEventData *incomingEventData, *outgoingEventData;

    switch(eCode)
    {
    case MNCC_HOLD_CNF:
    case MNCC_HOLD_REJ_IND:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;

		if (SSOperationsMN_CheckEventData(
                pMe, 
                incomingEventData, 
                SS_EVENT_DATA_CALLWISE_MSG, 
                pMe->m_ciCallToHold) == FALSE) // TI==Active
        {
			return FALSE;
		}           

        // reuse the incomingEventData memory
        FREEIF(incomingEventData->transactionwiseMsgData.msg);
        outgoingEventData = incomingEventData;
        outgoingEventData->commonData.type = SS_EVENT_DATA_ERROR;
        outgoingEventData->errorData.callIndex1 = pMe->m_ciCallToAnswer;
        outgoingEventData->errorData.callIndex2 = pMe->m_ciCallToHold;
        outgoingEventData->errorData.isCallIndex2Valid = TRUE;
        outgoingEventData->errorData.errorType = SS_ERROR_ANSWER_AND_HOLD;
        outgoingEventData->errorData.cause = SS_ERROR_CAUSE_WAITING_CALL_DISCONNECTED;

        SS_PostEvent(
            pMe->m_pIShell, 
            AEECLSID_SS_OPERATIONS, 
            UIMN_ERR_IND, 
            0, 
            (uint32)outgoingEventData
            );
        
        MOVE_TO_STATE( SSOperationsMN_ReadyState_Handler );
		return TRUE;
	}
	return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsMN_AH_WaitForHoldCnf1State_Handler

DESCRIPTION:
   This is the WaitForHoldCnf1 state event handler, which is part of the 
   Answer and Hold procedure.
   
PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMN_AH_WaitForHoldCnf1State_Handler(
    CSSOperationsMN *pMe,
    AEEEvent         eCode,
    uint16           wParam,
    uint32           dwParam
)
{
    SSEventData *incomingEventData, *outgoingEventData;

    switch(eCode)
    {
    case MNCC_HOLD_CNF:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        assert (incomingEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

        // check that the event belongs to the appropriate call
        if (incomingEventData->callwiseMsgData.callIndex1 != pMe->m_ciCallToHold)
            return FALSE;

        // reuse the incomingEventData memory
        outgoingEventData = incomingEventData;
        outgoingEventData->callwiseMsgData.callIndex1 = 
            GET_OTHER_CALL_INDEX(incomingEventData->callwiseMsgData.callIndex1);
        // callIndex2 is not used.
        outgoingEventData->callwiseMsgData.msg = NULL;

        SSOperationsFSM_QueueEvent(pMe->m_pFSM, MNCC_SETUP_RSP, 0, (uint32)outgoingEventData);
        
        MOVE_TO_STATE( SSOperationsMN_AH_WaitForSetupCompl1State_Handler );

        return TRUE;

    case MNCC_HOLD_REJ_IND:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        assert (incomingEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

        // check that the event belongs to the appropriate call
        if (incomingEventData->callwiseMsgData.callIndex1 != pMe->m_ciCallToHold)
            return FALSE;

        // reuse the incomingEventData memory
        outgoingEventData = incomingEventData;

        // make sure to free the message 
        if (incomingEventData->callwiseMsgData.msg != NULL)
        {
            SSOMessage_FreeMsg(incomingEventData->callwiseMsgData.msg);
        }

        // change event data type
        outgoingEventData->commonData.type = SS_EVENT_DATA_TRANSACTIONWISE_MSG;

        outgoingEventData->transactionwiseMsgData.callIndex = 
            GET_OTHER_CALL_INDEX(incomingEventData->callwiseMsgData.callIndex1);

        {
            boolean result = 
                SSCallDatabase_GetTInfoIndexByCallIndex(
                    pMe->m_pCallDatabase, 
                    outgoingEventData->transactionwiseMsgData.callIndex, 
                    &(outgoingEventData->transactionwiseMsgData.tInfoIndex)
                    );
            assert (result == TRUE);
        }

        // use a default DISCONNECT message (cause = normal call clearing).
        // maybe we should use a different cause ?
        outgoingEventData->transactionwiseMsgData.msg = NULL;

        SSOperationsFSM_QueueEvent(pMe->m_pFSM, MNCC_DISC_REQ, 0, (uint32)outgoingEventData);

        // update error cause for the AH_WaitForRelease1 state
        pMe->m_errorCause = SS_ERROR_CAUSE_HOLD_REJECT;
        
        MOVE_TO_STATE( SSOperationsMN_AH_WaitForRelease1State_Handler );

        return TRUE;

    case MNCC_REL_IND:
    case MNCC_REL_CNF:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        assert (incomingEventData->commonData.type == SS_EVENT_DATA_TRANSACTIONWISE_MSG);
     
		if (incomingEventData->transactionwiseMsgData.callIndex == pMe->m_ciCallToHold) 
        {
            // forward the incoming event to the UI
            SSOperationsFSM_QueueEvent(pMe->m_pFSM, eCode, 0, (uint32)incomingEventData);
            // send a setup rsp to the CC
			outgoingEventData = SSOEvent_PrepareCallwiseMsgEvent(pMe->m_ciCallToAnswer, 0, NULL);
	        SSOperationsFSM_QueueEvent(pMe->m_pFSM, MNCC_SETUP_RSP, 0, (uint32)outgoingEventData);

			MOVE_TO_STATE( SSOperationsMN_AH_WaitForSetupCompl2State_Handler );

			return TRUE;
        }
        
		if (incomingEventData->transactionwiseMsgData.callIndex == pMe->m_ciCallToAnswer)
        {
	        SSOEvent_FreeEventData(incomingEventData); 
			MOVE_TO_STATE( SSOperationsMN_AH_WaitForHoldCnf2State_Handler );
			return TRUE;           
        }
		
        return FALSE;

    case MNCC_DISC_IND:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        assert (incomingEventData->commonData.type == SS_EVENT_DATA_TRANSACTIONWISE_MSG);
     
		if (incomingEventData->transactionwiseMsgData.callIndex == pMe->m_ciCallToHold) 
        {
            // forward the incoming event to the UI
            SSOperationsFSM_QueueEvent(pMe->m_pFSM, eCode, 0, (uint32)incomingEventData);
            // send a setup rsp to the CC
			outgoingEventData = SSOEvent_PrepareCallwiseMsgEvent(pMe->m_ciCallToAnswer, 0, NULL);
	        SSOperationsFSM_QueueEvent(pMe->m_pFSM, MNCC_SETUP_RSP, 0, (uint32)outgoingEventData);

			MOVE_TO_STATE( SSOperationsMN_AH_WaitForSetupCompl2State_Handler );

			return TRUE;
		} 

		if (incomingEventData->transactionwiseMsgData.callIndex == pMe->m_ciCallToAnswer)
		{
            MOVE_TO_STATE( SSOperationsMN_AH_WaitForReleaseHoldState_Handler );
			SSOEvent_FreeEventData(incomingEventData);
			return TRUE;
		}
		
		return FALSE;
    
	} // end of switch(eCode)

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsMN_AH_WaitForSetupCompl1State_Handler

DESCRIPTION:
   This is the WaitForSetupCompl1 state event handler, which is part of the 
   Answer and Hold procedure.
   
PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMN_AH_WaitForSetupCompl1State_Handler(
    CSSOperationsMN *pMe,
    AEEEvent         eCode,
    uint16           wParam,
    uint32           dwParam
)
{
    SSEventData *incomingEventData, *outgoingEventData;

    switch(eCode)
    {
    case MNCC_SETUP_COMPLETE_IND:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        assert (incomingEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

        // check that the event belongs to the appropriate call
        if (incomingEventData->callwiseMsgData.callIndex1 != pMe->m_ciCallToAnswer)
            return FALSE;

        // fill the primitive data and send it to the UI
        // reuse the incomingEventData memory
        outgoingEventData = incomingEventData;
        // callIndex1 is the same as in the incomingEventData
        outgoingEventData->callwiseMsgData.callIndex2 = 
            GET_OTHER_CALL_INDEX(incomingEventData->callwiseMsgData.callIndex1);
        outgoingEventData->callwiseMsgData.msg = NULL;

        SS_PostEvent(
            pMe->m_pIShell, 
            AEECLSID_SS_OPERATIONS, 
            UIMN_ANSWER_AND_HOLD_CNF, 
            0, 
            (uint32)outgoingEventData
            );
        
        MOVE_TO_STATE( SSOperationsMN_ReadyState_Handler );

        return TRUE;

    case MNCC_REL_CNF:
    case MNCC_REL_IND:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        if (SSOperationsMN_CheckEventData(
                pMe, 
                incomingEventData, 
                SS_EVENT_DATA_TRANSACTIONWISE_MSG, 
                pMe->m_ciCallToAnswer) == FALSE)
        {
            return FALSE;
        }

        // reuse the incomingEventData memory
        FREEIF(incomingEventData->transactionwiseMsgData.msg);
        outgoingEventData = incomingEventData;
        outgoingEventData->commonData.type = SS_EVENT_DATA_ERROR;
        outgoingEventData->errorData.callIndex1 = pMe->m_ciCallToAnswer;
        outgoingEventData->errorData.callIndex2 = pMe->m_ciCallToHold;
        outgoingEventData->errorData.isCallIndex2Valid = TRUE;
        outgoingEventData->errorData.errorType = SS_ERROR_ANSWER_AND_HOLD;
        outgoingEventData->errorData.cause = SS_ERROR_CAUSE_WAITING_CALL_DISCONNECTED;

        SS_PostEvent(
            pMe->m_pIShell, 
            AEECLSID_SS_OPERATIONS, 
            UIMN_ERR_IND, 
            0, 
            (uint32)outgoingEventData
            );
        
        MOVE_TO_STATE( SSOperationsMN_ReadyState_Handler );

        return TRUE;
    case MNCC_DISC_IND:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        if (SSOperationsMN_CheckEventData(
                pMe, 
                incomingEventData, 
                SS_EVENT_DATA_TRANSACTIONWISE_MSG, 
                pMe->m_ciCallToAnswer) == FALSE)
        {
            return FALSE;
        }

        // update error cause for the AH_WaitForRelease1 state
        pMe->m_errorCause = SS_ERROR_CAUSE_WAITING_CALL_DISCONNECTED;
        
        MOVE_TO_STATE( SSOperationsMN_AH_WaitForRelease1State_Handler );

        SSOEvent_FreeEventData(incomingEventData);
        return TRUE;
    }
    
    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsMN_AH_WaitForRelease1State_Handler

DESCRIPTION:
   This is the WaitForRelease1 state event handler, which is part of the 
   Answer and Hold procedure.
   
PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMN_AH_WaitForRelease1State_Handler(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
)
{
    SSEventData *incomingEventData, *outgoingEventData;

    switch(eCode)
    {
    case MNCC_REL_IND:
    case MNCC_REL_CNF:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        if (SSOperationsMN_CheckEventData(
                pMe, 
                incomingEventData, 
                SS_EVENT_DATA_TRANSACTIONWISE_MSG, 
                pMe->m_ciCallToAnswer) == FALSE)
        {
            return FALSE;
        }

        // reuse the incomingEventData memory
        FREEIF(incomingEventData->transactionwiseMsgData.msg);
        outgoingEventData = incomingEventData;
        outgoingEventData->commonData.type = SS_EVENT_DATA_ERROR;
        outgoingEventData->errorData.callIndex1 = pMe->m_ciCallToAnswer;
        outgoingEventData->errorData.callIndex2 = pMe->m_ciCallToHold;
        outgoingEventData->errorData.isCallIndex2Valid = TRUE;
        outgoingEventData->errorData.errorType = SS_ERROR_ANSWER_AND_HOLD;

        assert(pMe->m_errorCause != SS_ERROR_CAUSE_UNSPECIFIED);
        outgoingEventData->errorData.cause = pMe->m_errorCause;

        SS_PostEvent(
            pMe->m_pIShell, 
            AEECLSID_SS_OPERATIONS, 
            UIMN_ERR_IND, 
            0, 
            (uint32)outgoingEventData
            );
        
        MOVE_TO_STATE( SSOperationsMN_ReadyState_Handler );

        return TRUE;
    case MNCC_DISC_IND:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        if (SSOperationsMN_CheckEventData(
                pMe, 
                incomingEventData, 
                SS_EVENT_DATA_TRANSACTIONWISE_MSG, 
                pMe->m_ciCallToAnswer) == FALSE)
        {
            return FALSE;
        }

        SSOEvent_FreeEventData(incomingEventData);
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsMN_AH_WaitForSetupCompl2State_Handler

DESCRIPTION:
   This is the WaitForSetupCompl2 state event handler, which is part of the 
   Answer and Hold procedure.
   
PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMN_AH_WaitForSetupCompl2State_Handler(
    CSSOperationsMN *pMe,
    AEEEvent         eCode,
    uint16           wParam,
    uint32           dwParam
)
{
    SSEventData *incomingEventData, *outgoingEventData;

    switch(eCode)
    {
    case MNCC_SETUP_COMPLETE_IND:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        assert (incomingEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

        // check that the event belongs to the appropriate call
        if (incomingEventData->callwiseMsgData.callIndex1 != pMe->m_ciCallToAnswer)
            return FALSE;

        // fill the primitive data and send it to the UI
        // reuse the incomingEventData memory
        outgoingEventData = incomingEventData;
        // callIndex1 is the same as in the incomingEventData
        outgoingEventData->callwiseMsgData.callIndex2 = 
            GET_OTHER_CALL_INDEX(incomingEventData->callwiseMsgData.callIndex1);
        outgoingEventData->callwiseMsgData.msg = NULL;

        SS_PostEvent(
            pMe->m_pIShell, 
            AEECLSID_SS_OPERATIONS, 
            UIMN_ANSWER_AND_HOLD_CNF, 
            0, 
            (uint32)outgoingEventData
            );
        
        MOVE_TO_STATE( SSOperationsMN_ReadyState_Handler );

        return TRUE;
    }
    
    return FALSE;
}

/******************************************************************************
//   Answer and Release state handlers
******************************************************************************/

/*=============================================================================
FUNCTION: SSOperationsMN_AR_WaitForReleaseState_Handler

DESCRIPTION:
   This is the WaitForRelease state event handler, which is part of the 
   Answer and Release procedure.
   
PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMN_AR_WaitForReleaseState_Handler(
    CSSOperationsMN *pMe,
    AEEEvent         eCode,
    uint16           wParam,
    uint32           dwParam
)
{
    SSEventData *incomingEventData, *outgoingEventData;

    switch(eCode)
    {
    case MNCC_REL_IND:
    case MNCC_REL_CNF:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        if (SSOperationsMN_CheckEventData(
                pMe, 
                incomingEventData, 
                SS_EVENT_DATA_TRANSACTIONWISE_MSG, 
                pMe->m_ciCallToRelease) == FALSE)
        {
            return FALSE;
        }

        // forward the incoming event to the UI
        SSOperationsFSM_QueueEvent(pMe->m_pFSM, eCode, 0, (uint32)incomingEventData);
        // send a setup rsp to the CC
        outgoingEventData = SSOEvent_PrepareCallwiseMsgEvent(pMe->m_ciCallToAnswer, 0, NULL);

        SSOperationsFSM_QueueEvent(pMe->m_pFSM, MNCC_SETUP_RSP, 0, (uint32)outgoingEventData);
        
        MOVE_TO_STATE( SSOperationsMN_AR_WaitForSetupComplState_Handler );

        return TRUE;
    case MNCC_DISC_IND:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        if (SSOperationsMN_CheckEventData(
                pMe, 
                incomingEventData, 
                SS_EVENT_DATA_TRANSACTIONWISE_MSG, 
                pMe->m_ciCallToRelease) == FALSE)
        {
            return FALSE;
        }

        SSOEvent_FreeEventData(incomingEventData);
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsMN_AR_WaitForSetupComplState_Handler

DESCRIPTION:
   This is the WaitForSetupCompl state event handler, which is part of the 
   Answer and Release procedure.
   
PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMN_AR_WaitForSetupComplState_Handler(
    CSSOperationsMN *pMe,
    AEEEvent         eCode,
    uint16           wParam,
    uint32           dwParam
)
{
    SSEventData *incomingEventData, *outgoingEventData;

    switch(eCode)
    {
    case MNCC_SETUP_COMPLETE_IND:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        assert (incomingEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

        // check that the event belongs to the appropriate call
        if (incomingEventData->callwiseMsgData.callIndex1 != pMe->m_ciCallToAnswer)
            return FALSE;

        // fill the primitive data and send it to the UI
        // reuse the incomingEventData memory
        outgoingEventData = incomingEventData;
        // callIndex1 is the same as in the setupComplIndData
        outgoingEventData->callwiseMsgData.callIndex2 = 
            GET_OTHER_CALL_INDEX(incomingEventData->callwiseMsgData.callIndex1);
        outgoingEventData->callwiseMsgData.msg = NULL;

        SS_PostEvent(
            pMe->m_pIShell, 
            AEECLSID_SS_OPERATIONS, 
            UIMN_ANSWER_AND_RELEASE_CNF, 
            0, 
            (uint32)outgoingEventData
            );
        
        MOVE_TO_STATE( SSOperationsMN_ReadyState_Handler );

        return TRUE;
    }
    
    return FALSE;
}

/******************************************************************************
//   Toggle state handlers
******************************************************************************/

/*=============================================================================
FUNCTION: SSOperationsMN_Toggle_WaitFor2State_Handler

DESCRIPTION:
   This is the WaitFor2 state event handler, which is part of the 
   Toggle procedure.
   
PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMN_Toggle_WaitFor2State_Handler(
    CSSOperationsMN *pMe,
    AEEEvent         eCode,
    uint16           wParam,
    uint32           dwParam
)
{
    switch(eCode)
    {
    case MN_TOGGLE_FSM_DONE:
        pMe->m_firstToggleResult = (SSEventToggleResultType)dwParam;

        MOVE_TO_STATE( SSOperationsMN_Toggle_WaitFor1State_Handler );

        return TRUE;
    }

    return SSOperationsToggle_HandleEvent(pMe->m_pToggle, eCode, wParam, dwParam);
}

/*=============================================================================
FUNCTION: SSOperationsMN_Toggle_WaitFor1State_Handler

DESCRIPTION:
   This is the WaitFor1 state event handler, which is part of the 
   Toggle procedure.
   
PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMN_Toggle_WaitFor1State_Handler(
    CSSOperationsMN *pMe,
    AEEEvent         eCode,
    uint16           wParam,
    uint32           dwParam
)
{
    SSEventData* outgoingEventData;

    switch(eCode)
    {
    case MN_TOGGLE_FSM_DONE:
        pMe->m_secondToggleResult = (SSEventToggleResultType)dwParam;

        if ((pMe->m_firstToggleResult == SS_TOGGLE_RESULT_CNF) &&
            (pMe->m_secondToggleResult == SS_TOGGLE_RESULT_CNF))
        {
            // 2x cnf => toggle succeeded
            outgoingEventData = SSOEvent_PrepareCallwiseMsgEvent(
                                    pMe->m_ciCallToHold, 
                                    pMe->m_ciCallToRetrieve, 
                                    NULL);
            SS_PostEvent(
                pMe->m_pIShell, 
                AEECLSID_SS_OPERATIONS, 
                UIMN_TOGGLE_CNF, 
                0, 
                (uint32)outgoingEventData
                );
        }
        else if ((pMe->m_firstToggleResult == SS_TOGGLE_RESULT_REJ) &&
                 (pMe->m_secondToggleResult == SS_TOGGLE_RESULT_REJ))
        {
            // 2x rej => toggle rejected
            outgoingEventData = SSOEvent_PrepareCallwiseMsgEvent(
                                    pMe->m_ciCallToHold, 
                                    pMe->m_ciCallToRetrieve, 
                                    NULL);
            SS_PostEvent(
                pMe->m_pIShell, 
                AEECLSID_SS_OPERATIONS, 
                UIMN_TOGGLE_REJ_IND, 
                0, 
                (uint32)outgoingEventData
                );
        }
        else if (((pMe->m_firstToggleResult == SS_TOGGLE_RESULT_CNF) &&
                  (pMe->m_secondToggleResult == SS_TOGGLE_RESULT_REJ)) ||
                 ((pMe->m_firstToggleResult == SS_TOGGLE_RESULT_REJ) &&
                  (pMe->m_secondToggleResult == SS_TOGGLE_RESULT_CNF)))
        {
            // cnf+rej => protocol error
            IPHONE_EndCall(pMe->m_pIPhone);

            // send a drop tch ind to the UI
            outgoingEventData = SSOEvent_PrepareCallwiseMsgEvent(0, 0, NULL);
            SS_PostEvent(
                pMe->m_pIShell, 
                AEECLSID_SS_OPERATIONS, 
                MNCC_DROP_TCH_IND, 
                0, 
                (uint32)outgoingEventData
                );
        }
        else
        {
            // one call disconnected
            // assert rel+!rel
            assert (((pMe->m_firstToggleResult == SS_TOGGLE_RESULT_REL) &&
                  (pMe->m_secondToggleResult != SS_TOGGLE_RESULT_REL)) ||
                 ((pMe->m_firstToggleResult != SS_TOGGLE_RESULT_REL) &&
                  (pMe->m_secondToggleResult == SS_TOGGLE_RESULT_REL)));

            outgoingEventData = SSOEvent_AllocateEventData(SS_EVENT_DATA_ERROR);
            assert(outgoingEventData);
            outgoingEventData->errorData.callIndex1 = pMe->m_ciCallToHold;
            outgoingEventData->errorData.callIndex2 = pMe->m_ciCallToRetrieve;
            outgoingEventData->errorData.isCallIndex2Valid = TRUE;
            outgoingEventData->errorData.errorType = SS_ERROR_TOGGLE;
            outgoingEventData->errorData.cause = SS_ERROR_CALL_DISCONNECTED;

            SS_PostEvent(
                pMe->m_pIShell, 
                AEECLSID_SS_OPERATIONS, 
                UIMN_ERR_IND, 
                0, 
                (uint32)outgoingEventData
                );
        }

        SSOperationsToggle_Release(pMe->m_pToggle);
        pMe->m_pToggle = NULL; // set toggle to NULL so it isn't cleared when MN ends!

        MOVE_TO_STATE( SSOperationsMN_ReadyState_Handler );

        return TRUE;
    }

    return SSOperationsToggle_HandleEvent(pMe->m_pToggle, eCode, wParam, dwParam);
}

/******************************************************************************
//   Build MPTY state handlers
******************************************************************************/

/*=============================================================================
FUNCTION: SSOperationsMN_MPTY_WaitForMPTYCnfState_Handler

DESCRIPTION:
   This is the WaitForMPTYCnf state event handler, which is part of the 
   Build MPTY procedure.
   
PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMN_MPTY_WaitForMPTYCnfState_Handler(
    CSSOperationsMN *pMe,
    AEEEvent         eCode,
    uint16           wParam,
    uint32           dwParam
)
{
    SSEventData *pEventData, *outgoingEventData;

    switch(eCode)
    {
    case MNCC_BUILD_MPTY_CNF:
        // dwParam holds an SSEventData
        pEventData = (SSEventData*)dwParam;
        assert (pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

        // check that the event belongs to the appropriate call
        // callIndex1 is the handler of the BuildMPTY procedure. It should
        // match callIndex2 from the UIMN_BUILD_MPTY_REQ
        if (pEventData->callwiseMsgData.callIndex1 != pMe->m_ciBuildMPTYHandler)
            return FALSE;

        // send a merge req to the CC
        outgoingEventData = pEventData;
        // flip between the call indices. callIndex2 will be the handler of the
        // BuildMPTY procedure
        outgoingEventData->callwiseMsgData.callIndex2 = pEventData->callwiseMsgData.callIndex1;
        outgoingEventData->callwiseMsgData.callIndex1 = GET_OTHER_CALL_INDEX(pEventData->callwiseMsgData.callIndex1);
        SSOperationsFSM_QueueEvent(pMe->m_pFSM, MNCC_MERGE_CALLS_REQ, 0, (uint32)outgoingEventData);
        
        MOVE_TO_STATE( SSOperationsMN_MPTY_WaitForMergeCnfState_Handler );

        return TRUE;

    case MNCC_ERR_IND:
        // dwParam holds an SSEventData
        pEventData = (SSEventData*)dwParam;
        assert (pEventData->commonData.type == SS_EVENT_DATA_ERROR);

        // check that the event belongs to the appropriate call
        if (pEventData->errorData.callIndex1 != pMe->m_ciBuildMPTYHandler)
            return FALSE;

        assert(pEventData->errorData.errorType == SS_ERROR_MPTY);

        // reuse the incoming EventData memory
        SS_PostEvent(
            pMe->m_pIShell, 
            AEECLSID_SS_OPERATIONS, 
            UIMN_ERR_IND, 
            0, 
            (uint32)pEventData
            );
        
        MOVE_TO_STATE( SSOperationsMN_ReadyState_Handler );

        return TRUE;

    case MNCC_DISC_IND:
    case MNCC_REL_IND:
    case MNCC_REL_CNF:
        // dwParam holds an SSEventData
        pEventData = (SSEventData*)dwParam;
        if (SSOperationsMN_CheckEventData(
                pMe, 
                pEventData, 
                SS_EVENT_DATA_TRANSACTIONWISE_MSG, 
                pMe->m_ciBuildMPTYHandler) == FALSE)
        {
            return FALSE;
        }

        // send an err ind to the UI
        outgoingEventData = SSOEvent_AllocateEventData(SS_EVENT_DATA_ERROR);
        assert (outgoingEventData != NULL);
        outgoingEventData->errorData.callIndex1 = pEventData->transactionwiseMsgData.callIndex;
        outgoingEventData->errorData.isCallIndex2Valid = FALSE;
        outgoingEventData->errorData.errorType = SS_ERROR_MPTY;
        outgoingEventData->errorData.cause = SS_ERROR_BUILD_MPTY_DISCONNECT;
        
        SS_PostEvent(
            pMe->m_pIShell, 
            AEECLSID_SS_OPERATIONS, 
            UIMN_ERR_IND, 
            0, 
            (uint32)outgoingEventData
            );

        // forward the incoming event to the UI by queueing it. in the next 
        // handle event iteration, the MN will translate it to UIMN event and
        // will send it to the UI.
        SSOperationsFSM_QueueEvent(pMe->m_pFSM, eCode, 0, (uint32)pEventData);
        
        MOVE_TO_STATE( SSOperationsMN_ReadyState_Handler );

        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsMN_MPTY_WaitForMergeCnfState_Handler

DESCRIPTION:
   This is the WaitForMergeCnf state event handler, which is part of the 
   Build MPTY procedure.
   
PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMN_MPTY_WaitForMergeCnfState_Handler(
    CSSOperationsMN *pMe,
    AEEEvent         eCode,
    uint16           wParam,
    uint32           dwParam
)
{
    SSEventData *pEventData, *outgoingEventData;

    switch(eCode)
    {
    case MNCC_MERGE_CALLS_CNF:
        // dwParam holds an SSEventData
        pEventData = (SSEventData*)dwParam;
        assert (pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

        // send a build MPTY cnf to the UI
        // reuse the incoming EventData memory
        outgoingEventData = pEventData;
        // callIndex2 from MNCC_MERGE_CALLS_CNF refer to the CC instance that
        // handled the request.
        outgoingEventData->callwiseMsgData.callIndex1 = 
            pEventData->callwiseMsgData.callIndex2;
        // other call index is not used in UIMN_BUILD_MPTY_CNF
        outgoingEventData->callwiseMsgData.callIndex2 = 0;
        SS_PostEvent(
            pMe->m_pIShell, 
            AEECLSID_SS_OPERATIONS, 
            UIMN_BUILD_MPTY_CNF, 
            0, 
            (uint32)pEventData
            );
        
        MOVE_TO_STATE( SSOperationsMN_ReadyState_Handler );

        return TRUE;
    }

    return FALSE;
}

