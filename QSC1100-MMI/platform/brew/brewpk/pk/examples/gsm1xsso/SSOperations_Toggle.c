/*=============================================================================
FILE: SSOperations_Toggle.c

SERVICES: Provides the Toggle sub state machine, which is part of the MN layer
          of the GSM1x SS Operations State Machine

GENERAL DESCRIPTION:
    The Toggle sub SM is part of the MN layer. It handles the Toggle (swap)
    between active and held calls procedure, which the user can execute.
    Toggle sub SM manages exactly 2 Toggle instances. Every Toggle instance 
    handles one call.
    Events received at the Toggle sub SM are routed to the appropriate Toggle
    instance.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "SSOperations_Toggle.h"
#include "SSOperations_UserEvents.h"
#include "SSCallDatabase.h"
#include "SSUtil.h"                         // SSUtil Class header
#include "SS_CommonMacros.h"
#include "SSOperations_CommonEventDefinitions.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// This macro is used to set a new state handler
#define MOVE_TO_STATE(newStateHandler) \
{ \
    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:MOVE_TO_STATE: " #newStateHandler, 0, 0, 0); \
    SSOperationsToggle_MoveToState(pMe, newStateHandler); \
}

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

// Event handlers for every state in the Hold state machine. There is no "Idle"
// state for this sub SM.

static boolean SSOperationsToggle_WaitForCnfState_Handler(
   CSSOperationsToggle *pMe,
   AEEEvent             eCode,
   uint16               wParam,
   uint32               dwParam
);

static boolean SSOperationsToggle_DoneState_Handler(
   CSSOperationsToggle *pMe,
   AEEEvent             eCode,
   uint16               wParam,
   uint32               dwParam
);

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*=============================================================================
FUNCTION: SSOperationsToggle_New

DESCRIPTION: Create an Toggle object.

PARAMETERS:
     pIShell [in]      : Pointer to the IShell instance
     pMN [in]          : Pointer to the MN object.
     pCallDatabase [in]: Pointer to a valid CallDatabase instance

RETURN VALUE:
    Pointer to the created SSOperationsToggle or NULL on failure.

COMMENTS:
    in order to free SSOperationsToggle object, use SSOperationsToggle_Release.
    This function should be called only once in a life time of a client.
=============================================================================*/
CSSOperationsToggle* SSOperationsToggle_New(
    IShell                  *pIShell,
    struct _CSSOperationsMN *pMN,
    SSCallDatabase          *pCallDatabase
    )
{
    CSSOperationsToggle* obj;

    obj = MALLOC(sizeof(CSSOperationsToggle));
    if (obj == NULL)
        return NULL;

    obj->m_pIShell = pIShell;
    obj->m_pMN = pMN;
    obj->m_pCallDatabase = pCallDatabase;

    ISHELL_AddRef(pIShell);

    return obj;
}

/*=============================================================================
FUNCTION: SSOperationsToggle_InitOneInstance

DESCRIPTION:
    Initialize one Toggle instance to handle one call.

PARAMETERS:
    pMe [in]       : Pointer to the Toggle object to be initialized.
    callIndex [in] : call index of the call that this instance will handle.

RETURN VALUE:
    None.
=============================================================================*/
void SSOperationsToggle_InitOneInstance(
    CSSOperationsToggle *pMe,
    SSCall_Index_type    callIndex
    )
{
    boolean success;
    ToggleCallInfo *pToggleCallInfo;
        
    // init a new Toggle call info
    pToggleCallInfo = &(pMe->m_Calls[callIndex]);
    
    success = SSCallDatabase_GetCallInfo(
        pMe->m_pCallDatabase, 
        callIndex,
        &(pToggleCallInfo->m_pCurrentCallInfo)
        );
    
    assert(success == TRUE);

    // init the Toggle instance to the WaitForCnf state.
    pToggleCallInfo->m_pCurrentStateHandler = SSOperationsToggle_WaitForCnfState_Handler;
}

/*=============================================================================
FUNCTION: SSOperationsToggle_Initialize

DESCRIPTION: Initialize an Toggle object. Initialize the 2 Toggle instances.

PARAMETERS:
     pMe [in] : Pointer to the Toggle object to be initialized.

RETURN VALUE:
    TRUE on success or FALSE on failure.
=============================================================================*/
boolean SSOperationsToggle_Initialize( CSSOperationsToggle *pMe )
{
    // create the SS utilities class
    pMe->m_pSSUtilCls = NULL;
    if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_SSUTIL, (void **)&pMe->m_pSSUtilCls) != SUCCESS)
        return FALSE;

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: SSUtil created by Toggle", 0, 0, 0);

    pMe->m_currentlyProcessedCallIndex = 0xff; // no call is being processed

    // init the two instances
    SSOperationsToggle_InitOneInstance(pMe, 0);
    SSOperationsToggle_InitOneInstance(pMe, 1);

    return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsToggle_Release

DESCRIPTION: Destroy an Toggle object.

PARAMETERS:
    pMe [in]: Pointer to the Toggle object to be destroyed.

RETURN VALUE:
    None
=============================================================================*/
void SSOperationsToggle_Release( CSSOperationsToggle *pMe )
{
    // release the SS utilities class
    if (pMe->m_pSSUtilCls != NULL)
    {
        ISSUTILCLS_Release(pMe->m_pSSUtilCls);
        pMe->m_pSSUtilCls = NULL;
    }

    ISHELL_Release(pMe->m_pIShell);

    FREE(pMe);
}

/*=============================================================================
FUNCTION: SSOperationsToggle_HandleEvent

DESCRIPTION: Entry point for events to the Toggle object.

PARAMETERS:
   pMe [in]            : Pointer to the Toggle object. 
   eCode [in]          : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled, FALSE otherwise.
=============================================================================*/
boolean SSOperationsToggle_HandleEvent(
    CSSOperationsToggle *pMe,
    AEEEvent             eCode,    
    uint16               wParam,      
    uint32               dwParam      
    )
{
    SSCall_Index_type callIndex;  //The call index of the call
    SSEventData *pEvent;
    
    switch (eCode)
    {
    case MNCC_HOLD_CNF:
    case MNCC_RETRIEVE_CNF:
    case MNCC_HOLD_REJ_IND:
    case MNCC_RETRIEVE_REJ_IND:
        pEvent = (SSEventData*)dwParam;

        // MNCC primitive must have an Event Data.
        assert (pEvent != NULL);
        // these primitives must have an Event Data of type SS_EVENT_DATA_CALLWISE_MSG
        assert (pEvent->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);
        
        // for these primitives, routing to the Toggle instance is based on 
        // callIndex1
        callIndex = pEvent->callwiseMsgData.callIndex1;
        break;
        
    case MNCC_REL_IND:
    case MNCC_REL_CNF:
    case MNCC_DISC_IND:
        pEvent = (SSEventData*)dwParam;

        // MNCC primitive must have an Event Data.
        assert (pEvent != NULL);
        // these primitives must have an Event Data of type SS_EVENT_DATA_TRANSACTIONWISE_MSG
        assert (pEvent->commonData.type == SS_EVENT_DATA_TRANSACTIONWISE_MSG);
        
        // for these primitives, routing to the Toggle instance is based on 
        // callIndex
        callIndex = pEvent->transactionwiseMsgData.callIndex;
        break;

    default:
        return FALSE;
    }
        
    assert(callIndex < 2);
    pMe->m_currentlyProcessedCallIndex = callIndex;
    // let the appropriate Toggle instance handle the event
    return (pMe->m_Calls[callIndex].m_pCurrentStateHandler(pMe, eCode, wParam, dwParam));
}

///////////////////////////////////////////////////////////////////////////////
//   Local Function Definitions
///////////////////////////////////////////////////////////////////////////////

/*=============================================================================
FUNCTION: SSOperationsToggle_MoveToState

DESCRIPTION:
    Changes the current state for the currently processed Toggle instance.
    This function provides a partial OnEnter/OnExit mechanism:
    1. an ONEXIT event is sent to the current state.
    2. current state is changed to the new state.
    3. an ONENTER event is sent to the current (new) state.
    This mechanism is used mainly for setting and canceling timers.
   
PARAMETERS:
    pMe [in]             : Pointer to an Toggle object.
    newStateHandler [in] : new state, identified by its state handler.

RETURN VALUE:
    None.
=============================================================================*/
void SSOperationsToggle_MoveToState(
    CSSOperationsToggle       *pMe,
    TOGGLE_STATE_EVENT_HANDLER newStateHandler
    )
{
    ToggleCallInfo *pToggleCallInfo = &pMe->m_Calls[pMe->m_currentlyProcessedCallIndex];

    // OnExit actions
    pToggleCallInfo->m_pCurrentStateHandler(pMe, CC_ONEXIT, 0, 0);

    // change state handler
    pToggleCallInfo->m_pCurrentStateHandler = newStateHandler;

    // OnEnter actions
    pToggleCallInfo->m_pCurrentStateHandler(pMe, CC_ONENTER, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////
//   State Handlers Definitions
///////////////////////////////////////////////////////////////////////////////

/*=============================================================================
FUNCTION: SSOperationsToggle_WaitForCnfState_Handler

DESCRIPTION:
   This is the WaitForCnf state event handler
   
PARAMETERS:
   pMe [in]   : Pointer to the Toggle object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsToggle_WaitForCnfState_Handler(
    CSSOperationsToggle *pMe,
    AEEEvent             eCode,
    uint16               wParam,
    uint32               dwParam
)
{
    SSEventData* incomingEventData;
    
    switch (eCode)
    {
    case MNCC_HOLD_CNF:
    case MNCC_RETRIEVE_CNF:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        
        SSOperationsMN_QueueEvent(
            pMe->m_pMN,
            MN_TOGGLE_FSM_DONE,
            incomingEventData->callwiseMsgData.callIndex1,
            (uint32)SS_TOGGLE_RESULT_CNF);
        
        SSOEvent_FreeEventData(incomingEventData);
        
        MOVE_TO_STATE( SSOperationsToggle_DoneState_Handler );
        
        return TRUE;

    case MNCC_HOLD_REJ_IND:
    case MNCC_RETRIEVE_REJ_IND:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        
        SSOperationsMN_QueueEvent(
            pMe->m_pMN,
            MN_TOGGLE_FSM_DONE,
            incomingEventData->callwiseMsgData.callIndex1,
            (uint32)SS_TOGGLE_RESULT_REJ);
        
        SSOEvent_FreeEventData(incomingEventData);
        
        MOVE_TO_STATE( SSOperationsToggle_DoneState_Handler );
        
        return TRUE;

    case MNCC_REL_IND:
    case MNCC_REL_CNF:
    case MNCC_DISC_IND:
        // dwParam holds an SSEventData
        incomingEventData = (SSEventData*)dwParam;
        
        // forward the incoming event to the UI
        SSOperationsMN_QueueEvent(
            pMe->m_pMN,
            eCode,
            0,
            (uint32)incomingEventData);

        // send the toggle result to the MN
        SSOperationsMN_QueueEvent(
            pMe->m_pMN,
            MN_TOGGLE_FSM_DONE,
            incomingEventData->transactionwiseMsgData.callIndex,
            (uint32)SS_TOGGLE_RESULT_REL);
        
        MOVE_TO_STATE( SSOperationsToggle_DoneState_Handler );
        
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsToggle_DoneState_Handler

DESCRIPTION:
   This is the Done state event handler. This is a final state of the Toggle
   SM. After reaching it, the SM is expected to be destroyed.
   
PARAMETERS:
   pMe [in]   : Pointer to the Toggle object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsToggle_DoneState_Handler(
    CSSOperationsToggle *pMe,
    AEEEvent             eCode,
    uint16               wParam,
    uint32               dwParam
    )
{
    return FALSE;
}
