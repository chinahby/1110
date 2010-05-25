/*=============================================================================
FILE: SSOperations_Hold.c

SERVICES: Provides the Hold sub state machine, which is part of the CC layer of
          the GSM1x SS Operations State Machine

GENERAL DESCRIPTION:
    The Hold sub SM is part of the CC layer. It handles the Hold and Retrieve
    procedures, which the user can execute for an active call.
    Hold sub SM manages Hold instances. Every Hold instance is linked to a CC
    instance.
    Events received at the Hold sub SM are routed to the appropriate Hold
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

#include "SSOperations_Hold.h"
#include "SSOperations_ClassId.h"
#include "SSOperations_UserEvents.h"
#include "SSCallDatabase.h"
#include "SSOperations_CommonEventDefinitions.h"
#include "SSOperations_CommonMessageDefinitions.h"
#include "SS_CommonMacros.h"

#include "SSUtil.h"                         // SSUtil Class header
#include "SSMsg.h"                          // SS layer 3 messages

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// This macro is used to set a new state handler
#define MOVE_TO_STATE(newStateHandler) \
{ \
    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:MOVE_TO_STATE: " #newStateHandler, 0, 0, 0); \
    SSOperationsHold_MoveToState(pMe, newStateHandler); \
}


/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

// Event handlers for every state in the Hold state machine

// Idle state
static boolean SSOperationsHold_IdleState_Handler(
   CSSOperationsHold *pMe,
   AEEEvent           eCode,
   uint16             wParam,
   uint32             dwParam
);

static boolean SSOperationsHold_HoldRequestState_Handler(
   CSSOperationsHold *pMe,
   AEEEvent           eCode,
   uint16             wParam,
   uint32             dwParam
);

static boolean SSOperationsHold_CallHeldState_Handler(
   CSSOperationsHold *pMe,
   AEEEvent           eCode,
   uint16             wParam,
   uint32             dwParam
);

static boolean SSOperationsHold_RetrieveRequestState_Handler(
   CSSOperationsHold *pMe,
   AEEEvent           eCode,
   uint16             wParam,
   uint32             dwParam
);

static void SSOperationsHold_CancelTimer(CSSOperationsHold *pMe, SSCall_TInfo_Index_type tInfoIndex);

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*=============================================================================
FUNCTION: SSOperationsHold_New

DESCRIPTION: Create the Hold object

PARAMETERS:
    pIShell [in]      : Pointer to the IShell instance 
    pCC [in]          : Pointer to the CC Manager object.
    pCallDatabase [in]: Pointer to a valid CallDatabase instance

RETURN VALUE:
    Pointer to the created SSOperationsCC or NULL on failure.
=============================================================================*/
CSSOperationsHold* SSOperationsHold_New(
    IShell                   *pIShell,
    struct _CSSOperationsCC  *pCC,
    SSCallDatabase           *pCallDatabase
    )
{
    CSSOperationsHold* obj;

    obj = MALLOC(sizeof(CSSOperationsHold));
    if (obj == NULL)
        return NULL;

    obj->m_pIShell = pIShell;
    obj->m_pCC = pCC;
    obj->m_pCallDatabase = pCallDatabase;

    ISHELL_AddRef(pIShell);

    return obj;
}

/*=============================================================================
FUNCTION: SSOperationsHold_Initialize

DESCRIPTION: Initialize an Hold object

PARAMETERS:
    pMe [in]: Pointer to the Hold object to be initialized.

RETURN VALUE:
    TRUE on success or FALSE on failure.
=============================================================================*/
boolean SSOperationsHold_Initialize( CSSOperationsHold *pMe )
{
    // create the SS utilities class
    pMe->m_pSSUtilCls = NULL;
    if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_SSUTIL, (void **)&pMe->m_pSSUtilCls) != SUCCESS)
        return FALSE;

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: SSUtil created by Hold", 0, 0, 0);

    pMe->m_currentlyProcessedTInfoIndex = 0xff; // no call is being processed

    return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsHold_Release

DESCRIPTION: Destroy an Hold object.

PARAMETERS:
     pMe [in]: Pointer to the Hold object to be destroyed.

RETURN VALUE:
     None
=============================================================================*/
void SSOperationsHold_Release( CSSOperationsHold *pMe )
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
FUNCTION: SSOperationsHold_InitCall

DESCRIPTION: 
    Called from the CC to initialize a new Hold instance.
    The Call Database should already contain an entry for this new call.

PARAMETERS:
   pMe [in]        : Pointer to the Hold object. 
   tInfoIndex [in] : transaction index of the new call.

RETURN VALUE:
    None.
=============================================================================*/
void SSOperationsHold_InitCall(
    CSSOperationsHold* pMe,
    SSCall_TInfo_Index_type tInfoIndex)
{
    boolean success;
    HoldTInfo *pHoldTInfo;
        
    // init a new Hold call info
    pHoldTInfo = &(pMe->m_Transactions[tInfoIndex]);
    
    success = SSCallDatabase_GetTransactionInfo(
        pMe->m_pCallDatabase, 
        tInfoIndex,
        &(pHoldTInfo->m_pCurrentTInfo));
    
    // the CC (MT) or the UI (MO) just added this call database entry so it 
    // must exist.
    assert(success == TRUE);
    
//#ifndef AEE_SIMULATOR
    // update the Hold part in Call Database
    pHoldTInfo->m_pCurrentTInfo->callState.holdState = SSCallHoldState_IDLE;
    // init the new call to the Idle state.
    pHoldTInfo->m_pCurrentStateHandler = SSOperationsHold_IdleState_Handler;
/*
#else
    // debug code that synchronize the current state as per the hold state
    // that is already stored in the Call Database.
    pHoldTInfo->m_pCurrentStateHandler = 
        ((pHoldTInfo->m_pCurrentTInfo->callState.holdState == SSCallHoldState_IDLE) ? 
            SSOperationsHold_IdleState_Handler : SSOperationsHold_CallHeldState_Handler);
#endif
*/    
    // init timer data
    pHoldTInfo->m_timerData.pMe = pMe;
    pHoldTInfo->m_timerData.tInfoIndex = tInfoIndex;
    pHoldTInfo->m_timerData.timerId = SSO_TIMER_NONE;
}

/*=============================================================================
FUNCTION: SSOperationsHold_ReleaseTransaction

DESCRIPTION:
    Called from the CC upon call clearing, to release a Hold instance.

PARAMETERS:
    pMe [in]        : Pointer to the Hold object.
    tInfoIndex [in] : transaction index of the cleared call.

RETURN VALUE:
    None.

SEE ALSO:
    SSOperationsCC_ReleaseTransaction
=============================================================================*/
void SSOperationsHold_ReleaseTransaction(
    CSSOperationsHold *pMe,
    SSCall_TInfo_Index_type tInfoIndex
    )
{
    HoldTInfo *pHoldTInfo = &(pMe->m_Transactions[tInfoIndex]);
        
    // reset the Hold call info
    pHoldTInfo->m_pCurrentTInfo = NULL;
    pHoldTInfo->m_pCurrentStateHandler = NULL;
    // cancel possible pending timer
    SSOperationsHold_CancelTimer(pMe, tInfoIndex);
    pHoldTInfo->m_timerData.pMe = NULL;
}

/*=============================================================================
FUNCTION: SSOperationsHold_SetState

DESCRIPTION:
    Set a given Hold instance to a specific state.
    Used by the CC as part of a Build MPTY procedure.

PARAMETERS:
    pMe [in]        : Pointer to the Hold object.
    tInfoIndex [in] : transaction index identifying the Hold instance to 
                      change its state.
    newState [in]   : new state for the Hold instance.

RETURN VALUE:
    None.

SEE ALSO:
    SSOperationsCC_MergeCalls
    
COMMENTS:
    Currently, the only allowed newState is SSCallHoldState_IDLE
=============================================================================*/
void SSOperationsHold_SetState(
    CSSOperationsHold *pMe,
    SSCall_TInfo_Index_type tInfoIndex, 
    SSCall_Hold_State_type newState
    )
{
    HoldTInfo *pHoldTInfo = &(pMe->m_Transactions[tInfoIndex]);

    assert(pHoldTInfo->m_pCurrentStateHandler != NULL);

    switch(newState)
    {
    case SSCallHoldState_IDLE:
        pHoldTInfo->m_pCurrentStateHandler = SSOperationsHold_IdleState_Handler;
        pHoldTInfo->m_pCurrentTInfo->callState.holdState = newState;
        break;

    default:
        ASSERT_NOT_REACHABLE;
    }
}

/*=============================================================================
FUNCTION: SSOperationsHold_HandleEvent

DESCRIPTION: Entry point for events to the Hold object.

PARAMETERS:
   pMe [in]            : Pointer to the Hold object. 
   tInfoIndex [in]     : transaction index.
   eCode [in]          : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled, FALSE otherwise.

COMMENTS:
    Routing of the event to the appropriate CC instance was already done in 
    the CC. This information is conveyed to the Hold by the tInfoIndex param.
=============================================================================*/
boolean SSOperationsHold_HandleEvent(
    CSSOperationsHold *pMe,
    SSCall_TInfo_Index_type tInfoIndex,
    AEEEvent           eCode,    
    uint16             wParam,      
    uint32             dwParam      
 )
{
    pMe->m_currentlyProcessedTInfoIndex = tInfoIndex;
    return (pMe->m_Transactions[tInfoIndex].m_pCurrentStateHandler(pMe, eCode, wParam, dwParam));
}

///////////////////////////////////////////////////////////////////////////////
//   Local Function Definitions
///////////////////////////////////////////////////////////////////////////////

/*=============================================================================
FUNCTION: SSOperationsHold_MoveToState

DESCRIPTION:
    Changes the current state for the currently processed Hold instance.
    This function provides a partial OnEnter/OnExit mechanism:
    1. an ONEXIT event is sent to the current state.
    2. current state is changed to the new state.
    3. an ONENTER event is sent to the current (new) state.
    This mechanism is used mainly for setting and canceling timers.
   
PARAMETERS:
    pMe [in]             : Pointer to an Hold object.
    newStateHandler [in] : new state, identified by its state handler.

RETURN VALUE:
    None.
=============================================================================*/
void SSOperationsHold_MoveToState(
    CSSOperationsHold        *pMe,
    HOLD_STATE_EVENT_HANDLER  newStateHandler
    )
{
    HoldTInfo *pHoldTInfo = &pMe->m_Transactions[pMe->m_currentlyProcessedTInfoIndex];

    // OnExit actions
    pHoldTInfo->m_pCurrentStateHandler(pMe, CC_ONEXIT, 0, 0);

    // change state handler
    pHoldTInfo->m_pCurrentStateHandler = newStateHandler;

    // OnEnter actions
    pHoldTInfo->m_pCurrentStateHandler(pMe, CC_ONENTER, 0, 0);
}

/*=============================================================================
FUNCTION: HoldTimeoutCallback

DESCRIPTION:
    Callback function passed to the BREW layer for Hold timers expirations.
    This function forwards the timeout indication as a PostEvent of CC_TIMEOUT.
    This event will be processed in the next HandleEvent iteration of the SSO.
   
PARAMETERS:
    pUser [in] : user specific data. it is assumed to be pointing to an
                 SSOperationsHold_TimerDataType. this timer data is specific to
                 the Hold instance that the expired timer belongs to.

RETURN VALUE:
    None
=============================================================================*/
static void HoldTimeoutCallback(void *pUser)
{
    SSOperationsHold_TimerDataType* timerData = (SSOperationsHold_TimerDataType*) pUser;

    GSM1X_LOG_HIGH(timerData->pMe->m_pSSUtilCls, 
        "SSO:HoldTimeoutCallback: tInfoIndex=%d timer id=%d", timerData->tInfoIndex, timerData->timerId, 0);
    if (timerData->timerId == SSO_TIMER_NONE)
    {
        GSM1X_LOG_HIGH(timerData->pMe->m_pSSUtilCls, "SSO:Warning: invalid Hold timer id. event ignored",0,0,0);
        return;
    }

    // Post a message to the FSM
    SS_PostEvent(timerData->pMe->m_pIShell, AEECLSID_SS_OPERATIONS,
                 CC_TIMEOUT, (uint16)(timerData->tInfoIndex), (uint32)(timerData->timerId));

    timerData->timerId = SSO_TIMER_NONE;
}

/*=============================================================================
FUNCTION: SSOperationsHold_SetTimer

DESCRIPTION:
    Set a specific timer, using BREW timers, for a specific Hold instance.
   
PARAMETERS:
   pMe [in]           : Pointer to the Hold object.
   tInfoIndex [in]    : identifies the requesting Hold instance.
   timerId [in]       : identifies the timer to be set.
   timerDuration [in] : timer duration.

RETURN VALUE:
    None

COMMENTS:
    Each Hold instance, at any given time, can only use a single timer.
=============================================================================*/
static void SSOperationsHold_SetTimer(
    CSSOperationsHold    *pMe,
    SSCall_TInfo_Index_type     tInfoIndex,
    SSO_TimerIdType       timerId, 
    SSO_TimerDurationType timerDuration
    )
{
    HoldTInfo *pHoldTInfo = &pMe->m_Transactions[tInfoIndex];

    pHoldTInfo->m_timerData.pMe = pMe;
    pHoldTInfo->m_timerData.timerId = timerId;

    CALLBACK_Init(
       &pHoldTInfo->m_TimerCB, 
       HoldTimeoutCallback, 
       &pHoldTInfo->m_timerData
       );

    (void) ISHELL_SetTimerEx(
              pMe->m_pIShell,
              timerDuration,
              &pHoldTInfo->m_TimerCB
              );
}

/*=============================================================================
FUNCTION: SSOperationsHold_CancelTimer

DESCRIPTION:
    Cancel a timer used by a specific Hold instance.
   
PARAMETERS:
   pMe [in]        : Pointer to the Hold object.
   tInfoIndex [in] : identifies the requesting Hold instance.

RETURN VALUE:
    None

COMMENTS:
    Each Hold instance, at any given time, can only use a single timer.
=============================================================================*/
static void SSOperationsHold_CancelTimer(
    CSSOperationsHold *pMe,
    SSCall_TInfo_Index_type  tInfoIndex
    )
{
    HoldTInfo *pHoldTInfo = &pMe->m_Transactions[tInfoIndex];

    CALLBACK_Cancel(&pHoldTInfo->m_TimerCB);
    pHoldTInfo->m_timerData.timerId = SSO_TIMER_NONE;
}

///////////////////////////////////////////////////////////////////////////////
//   State Handlers Definitions
///////////////////////////////////////////////////////////////////////////////

/*=============================================================================
FUNCTION: SSOperationsHold_IdleState_Handler

DESCRIPTION:
   This is the Idle state event handler
   
PARAMETERS:
   pMe [in]   : Pointer to the Hold object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsHold_IdleState_Handler(
    CSSOperationsHold *pMe,
    AEEEvent           eCode,
    uint16             wParam,
    uint32             dwParam
)
{
    SSCall_TInfo_type* pTInfo;

    switch (eCode)
    {
    case MNCC_HOLD_REQ:
        {
            SSEventCallwiseMsgData *pEventData = (SSEventCallwiseMsgData*)dwParam;

            // The primitive should not hold a message
            assert (pEventData->msg == NULL);
            
            // send HOLD
            SSOperationsCC_DataRequest(
                pMe->m_pCC, 
                pMe->m_currentlyProcessedTInfoIndex,
                SSMSG_MSG_TYPE_HOLD,
                NULL);

            SSOEvent_FreeEventData((SSEventData*)pEventData);

            // update Call Database and switch states for the processed
            // Hold instance
            pTInfo = pMe->m_Transactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            pTInfo->callState.holdState = SSCallHoldState_HOLD_REQUEST;
            MOVE_TO_STATE( SSOperationsHold_HoldRequestState_Handler );

            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsHold_HoldRequestState_Handler

DESCRIPTION:
   This is the HoldRequest state event handler
   
PARAMETERS:
   pMe [in]   : Pointer to the Hold object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsHold_HoldRequestState_Handler(
    CSSOperationsHold *pMe,
    AEEEvent           eCode,
    uint16             wParam,
    uint32             dwParam
 )
{
    SSEventDataIndData* pDataIndEventData;
    SSCall_TInfo_type* pTInfo;
    SSCall_Info_type* pCallInfo;
    SSCall_Index_type callIndex;
    
    switch (eCode)
    {
    case CC_ONENTER:
        // handle timers
        SSOperationsHold_SetTimer(pMe, pMe->m_currentlyProcessedTInfoIndex, 
            SSO_TIMER_Thold, SSO_TIMER_Thold_DURATION);
        return TRUE;

    case CC_ONEXIT:
        // stop timer
        SSOperationsHold_CancelTimer(pMe, pMe->m_currentlyProcessedTInfoIndex);
        return TRUE;

    case MNCC_DATA_INDICATION:
        pDataIndEventData = (SSEventDataIndData*)dwParam;
        
        switch(pDataIndEventData->msg->gen_hdr.msg_type)
        {
        case SSMSG_MSG_TYPE_HOLD_ACKNOWLEDGE:
            // send a hold cnf to the MN
            SSCallDatabase_GetCallIndexByTInfoIndex(
                pMe->m_pCallDatabase, 
                pMe->m_currentlyProcessedTInfoIndex,
                &callIndex);
            if (SSOperationsCC_SendCallwiseMsgEvent(
                pMe->m_pCC, 
                MNCC_HOLD_CNF,
                callIndex,
                0,
                NULL) == FALSE)
            {
                return FALSE;
            }
            
            // update Call Database and switch states for the processed
            // Hold instance
            pTInfo = pMe->m_Transactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            SSCallDatabase_GetCallInfo(pMe->m_pCallDatabase, callIndex, &pCallInfo);
            pCallInfo->isHeld = TRUE;
            pTInfo->callState.holdState = SSCallHoldState_CALL_HELD;
            
            MOVE_TO_STATE( SSOperationsHold_CallHeldState_Handler );
            
            SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);
            return TRUE;

        case SSMSG_MSG_TYPE_HOLD_REJECT:
            // send a hold reject ind to the MN
            SSCallDatabase_GetCallIndexByTInfoIndex(
                pMe->m_pCallDatabase, 
                pMe->m_currentlyProcessedTInfoIndex,
                &callIndex);
            if (SSOperationsCC_SendCallwiseMsgEvent(
                pMe->m_pCC, 
                MNCC_HOLD_REJ_IND,
                callIndex,
                0,
                pDataIndEventData->msg) == FALSE)
            {
                return FALSE;
            }
            
            // update Call Database and switch states for the processed
            // Hold instance
            pTInfo = pMe->m_Transactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            pTInfo->callStatus = SS_CALL_STATUS_ACTIVE;
            pTInfo->callState.holdState = SSCallHoldState_IDLE;
            
            MOVE_TO_STATE( SSOperationsHold_IdleState_Handler );
            
            SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);
            return TRUE;

        default:
            return FALSE;
        }

    case CC_TIMEOUT:
        // dwParam holds the timer id
        if ((SSO_TimerIdType)dwParam != SSO_TIMER_Thold)
            return FALSE;

        // send a timeout ind to the CC
        SSOperationsCC_QueueEvent(pMe->m_pCC, CC_HOLD_TIMEOUT_IND, pMe->m_currentlyProcessedTInfoIndex, 0);

        // update Call Database and switch states for the processed
        // Hold instance
        pTInfo = pMe->m_Transactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
        pTInfo->callStatus = SS_CALL_STATUS_ACTIVE;
        pTInfo->callState.holdState = SSCallHoldState_IDLE;
        
        MOVE_TO_STATE( SSOperationsHold_IdleState_Handler );

        return TRUE;
    }
    
    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsHold_CallHeldState_Handler

DESCRIPTION:
   This is the CallHeld state event handler
   
PARAMETERS:
   pMe [in]   : Pointer to the Hold object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsHold_CallHeldState_Handler(
    CSSOperationsHold *pMe,
    AEEEvent           eCode,
    uint16             wParam,
    uint32             dwParam
 )
{
    SSEventCallwiseMsgData *pMsgEventData;
    SSCall_TInfo_type* pTInfo;

    switch (eCode)
    {
    case MNCC_RETRIEVE_REQ:
        pMsgEventData = (SSEventCallwiseMsgData*)dwParam;
                
        // The primitive should not hold a message
        assert (pMsgEventData->msg == NULL);
        
        // send RETRIEVE
        SSOperationsCC_DataRequest(
            pMe->m_pCC, 
            pMe->m_currentlyProcessedTInfoIndex,
            SSMSG_MSG_TYPE_RETRIEVE,
            NULL);
        
        SSOEvent_FreeEventData((SSEventData*)pMsgEventData);
        
        // update Call Database and switch states for the processed
        // Hold instance
        pTInfo = pMe->m_Transactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
        pTInfo->callState.holdState = SSCallHoldState_RETRIEVE_REQUEST;
        MOVE_TO_STATE( SSOperationsHold_RetrieveRequestState_Handler );
        
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsHold_RetrieveRequestState_Handler

DESCRIPTION:
   This is the RetrieveRequest state event handler
   
PARAMETERS:
   pMe [in]   : Pointer to the Hold object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsHold_RetrieveRequestState_Handler(
    CSSOperationsHold *pMe,
    AEEEvent           eCode,
    uint16             wParam,
    uint32             dwParam
 )
{
    SSEventDataIndData* pDataIndEventData;
    SSCall_TInfo_type* pTInfo;
    SSCall_Info_type* pCallInfo;
    SSCall_Index_type callIndex;

    switch (eCode)
    {
    case CC_ONENTER:
        // handle timers
        SSOperationsHold_SetTimer(pMe, pMe->m_currentlyProcessedTInfoIndex, 
            SSO_TIMER_Tretrieve, SSO_TIMER_Tretrieve_DURATION);
        return TRUE;

    case CC_ONEXIT:
        // stop timer
        SSOperationsHold_CancelTimer(pMe, pMe->m_currentlyProcessedTInfoIndex);
        return TRUE;

    case MNCC_DATA_INDICATION:
        pDataIndEventData = (SSEventDataIndData*)dwParam;
            
        switch(pDataIndEventData->msg->gen_hdr.msg_type)
        {
        case SSMSG_MSG_TYPE_RETRIEVE_ACKNOWLEDGE:
            // send a retrieve cnf to the MN
            SSCallDatabase_GetCallIndexByTInfoIndex(
                pMe->m_pCallDatabase, 
                pMe->m_currentlyProcessedTInfoIndex,
                &callIndex);
            if (SSOperationsCC_SendCallwiseMsgEvent(
                pMe->m_pCC, 
                MNCC_RETRIEVE_CNF,
                callIndex,
                0,
                NULL) == FALSE)
            {
                return FALSE;
            }
            
            // update Call Database and switch states for the processed
            // Hold instance
            pTInfo = pMe->m_Transactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            SSCallDatabase_GetCallInfo(pMe->m_pCallDatabase, callIndex, &pCallInfo);
            pCallInfo->isHeld = FALSE;
            pTInfo->callStatus = SS_CALL_STATUS_ACTIVE;
            pTInfo->callState.holdState = SSCallHoldState_IDLE;
            
            MOVE_TO_STATE( SSOperationsHold_IdleState_Handler );
            
            SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);
            return TRUE;

        case SSMSG_MSG_TYPE_RETRIEVE_REJECT:
            // send a retrieve reject ind to the MN
            SSCallDatabase_GetCallIndexByTInfoIndex(
                pMe->m_pCallDatabase, 
                pMe->m_currentlyProcessedTInfoIndex,
                &callIndex);
            if (SSOperationsCC_SendCallwiseMsgEvent(
                pMe->m_pCC, 
                MNCC_RETRIEVE_REJ_IND,
                callIndex,
                0,
                pDataIndEventData->msg) == FALSE)
            {
                return FALSE;
            }
            
            // update Call Database and switch states for the processed
            // Hold instance
            pTInfo = pMe->m_Transactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            SSCallDatabase_GetCallInfo(pMe->m_pCallDatabase, callIndex, &pCallInfo);
            pCallInfo->isHeld = TRUE;
            pTInfo->callState.holdState = SSCallHoldState_CALL_HELD;
            
            MOVE_TO_STATE( SSOperationsHold_CallHeldState_Handler );
            
            SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);
            return TRUE;

        default:
            return FALSE;
        }
    case CC_TIMEOUT:
        // dwParam holds the timer id
        if ((SSO_TimerIdType)dwParam != SSO_TIMER_Tretrieve)
            return FALSE;

        // send a timeout ind to the CC
        SSOperationsCC_QueueEvent(pMe->m_pCC, CC_HOLD_TIMEOUT_IND, pMe->m_currentlyProcessedTInfoIndex, 0);

        // update Call Database and switch states for the processed
        // Hold instance
        pTInfo = pMe->m_Transactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
        pTInfo->callStatus = SS_CALL_STATUS_ACTIVE;
        pTInfo->callState.holdState = SSCallHoldState_IDLE;
        
        MOVE_TO_STATE( SSOperationsHold_IdleState_Handler );

        return TRUE;
    }
    
    return FALSE;
}

