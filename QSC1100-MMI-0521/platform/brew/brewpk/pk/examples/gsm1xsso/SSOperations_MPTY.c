/*=============================================================================
FILE: SSOperations_MPTY.c

SERVICES: Provides the MPTY sub state machine, which is part of the CC layer of
          the GSM1x SS Operations State Machine

GENERAL DESCRIPTION:
    The MPTY sub SM is part of the CC layer. It handles the MPTY procedures, 
    which the user can execute. Currently, only Build MPTY is implemented.
    MPTY sub SM manages MPTY instances. Every MPTY instance is linked to a CC
    instance.
    Events received at the MPTY sub SM are routed to the appropriate MPTY
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

#include "SSOperations_MPTY.h"
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
    SSOperationsMPTY_MoveToState(pMe, newStateHandler); \
}

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

// Event handlers for every state in the MPTY state machine

// Idle state
static boolean SSOperationsMPTY_IdleState_Handler(
   CSSOperationsMPTY *pMe,
   AEEEvent           eCode,
   uint16             wParam,
   uint32             dwParam
);

static boolean SSOperationsMPTY_MPTYRequestState_Handler(
   CSSOperationsMPTY *pMe,
   AEEEvent           eCode,
   uint16             wParam,
   uint32             dwParam
);

static boolean SSOperationsMPTY_CallInMPTYState_Handler(
   CSSOperationsMPTY *pMe,
   AEEEvent           eCode,
   uint16             wParam,
   uint32             dwParam
);

static void SSOperationsMPTY_CancelTimer(CSSOperationsMPTY *pMe, SSCall_TInfo_Index_type  tInfoIndex);

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*=============================================================================
FUNCTION: SSOperationsMPTY_New

DESCRIPTION: Create an MPTY object.

PARAMETERS:
     pIShell [in]      : Pointer to the IShell instance
     pCC [in]          : Pointer to the CC Manager object.
     pCallDatabase [in]: Pointer to a valid CallDatabase instance

RETURN VALUE:
    Pointer to the created SSOperationsMPTY or NULL on failure.

COMMENTS:
    in order to free SSOperationsMPTY object, use SSOperationsMPTY_Release.
    This function should be called only once in a life time of a client.
=============================================================================*/
CSSOperationsMPTY* SSOperationsMPTY_New(
    IShell                   *pIShell,
    struct _CSSOperationsCC  *pCC,
    SSCallDatabase           *pCallDatabase
    )
{
    CSSOperationsMPTY* obj;

    obj = MALLOC(sizeof(CSSOperationsMPTY));
    if (obj == NULL)
        return NULL;

    obj->m_pIShell = pIShell;
    obj->m_pCC = pCC;
    obj->m_pCallDatabase = pCallDatabase;

    ISHELL_AddRef(pIShell);

    return obj;
}

/*=============================================================================
FUNCTION: SSOperationsMPTY_Initialize

DESCRIPTION: Initialize an MPTY object

PARAMETERS:
     pMe [in] : Pointer to the MPTY object to be initialized.

RETURN VALUE:
    TRUE on success or FALSE on failure.
=============================================================================*/
boolean SSOperationsMPTY_Initialize( CSSOperationsMPTY *pMe )
{
    // create the SS utilities class
    pMe->m_pSSUtilCls = NULL;
    if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_SSUTIL, (void **)&pMe->m_pSSUtilCls) != SUCCESS)
        return FALSE;

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: SSUtil created by MPTY", 0, 0, 0);

    pMe->m_currentlyProcessedTInfoIndex = 0xff; // no call is being processed

    return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsMPTY_Release

DESCRIPTION: Destroy an MPTY object.

PARAMETERS:
    pMe [in]: Pointer to the MPTY object to be destroyed.

RETURN VALUE:
    None
=============================================================================*/
void SSOperationsMPTY_Release( CSSOperationsMPTY *pMe )
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
FUNCTION: SSOperationsMPTY_InitCall

DESCRIPTION: 
    Called from the CC to initialize a new MPTY instance.
    The Call Database should already contain an entry for this new call.

PARAMETERS:
   pMe [in]        : Pointer to the MPTY object. 
   tInfoIndex [in] : transaction index of the new call.

RETURN VALUE:
    None.
=============================================================================*/
void SSOperationsMPTY_InitCall(
    CSSOperationsMPTY* pMe,
    SSCall_TInfo_Index_type tInfoIndex)
{
    boolean success;
    MPTYTInfo *pMPTYTInfo;
        
    // init a new MPTY call info
    pMPTYTInfo = &(pMe->m_Transactions[tInfoIndex]);
    
    success = SSCallDatabase_GetTransactionInfo(
        pMe->m_pCallDatabase, 
        tInfoIndex,
        &(pMPTYTInfo->m_pCurrentTInfo));
    
    // the CC (MT) or the UI (MO) just added this call database entry so it 
    // must exist.
    assert(success == TRUE);
    
    // update the MPTY part in Call Database
    pMPTYTInfo->m_pCurrentTInfo->callState.mptyState = SSCallMPTYState_IDLE;
    // init the new call to the Idle state.
    pMPTYTInfo->m_pCurrentStateHandler = SSOperationsMPTY_IdleState_Handler;

    // init timer data
    pMPTYTInfo->m_timerData.pMe = pMe;
    pMPTYTInfo->m_timerData.tInfoIndex = tInfoIndex;
    pMPTYTInfo->m_timerData.timerId = SSO_TIMER_NONE;
}

/*=============================================================================
FUNCTION: SSOperationsMPTY_ReleaseTransaction

DESCRIPTION:
    Called from the CC upon call clearing, to release a MPTY instance.

PARAMETERS:
    pMe [in]        : Pointer to the MPTY object.
    tInfoIndex [in] : transaction index of the cleared call.

RETURN VALUE:
    None.

SEE ALSO:
    SSOperationsCC_ReleaseTransaction
=============================================================================*/
void SSOperationsMPTY_ReleaseTransaction(
    CSSOperationsMPTY *pMe,
    SSCall_TInfo_Index_type tInfoIndex)
{
    MPTYTInfo *pMPTYTInfo = &(pMe->m_Transactions[tInfoIndex]);
        
    // reset the MPTY call info
    pMPTYTInfo->m_pCurrentTInfo = NULL;
    pMPTYTInfo->m_pCurrentStateHandler = NULL;
    // cancel possible pending timer
    SSOperationsMPTY_CancelTimer(pMe, tInfoIndex);
    pMPTYTInfo->m_timerData.pMe = NULL;
}

/*=============================================================================
FUNCTION: SSOperationsMPTY_SetState

DESCRIPTION:
    Set a given MPTY instance to a specific state.
    Used by the CC as part of a Build MPTY procedure.

PARAMETERS:
    pMe [in]        : Pointer to the MPTY object.
    tInfoIndex [in] : transaction index identifying the MPTY instance to 
                      change its state.
    newState [in]   : new state for the MPTY instance.

RETURN VALUE:
    None.

SEE ALSO:
    SSOperationsCC_MergeCalls

COMMENTS:
    Currently, the only allowed newState is SSCallMPTYState_CALL_IN_MPTY
=============================================================================*/
void SSOperationsMPTY_SetState(
    CSSOperationsMPTY *pMe,
    SSCall_TInfo_Index_type tInfoIndex, 
    SSCall_MPTY_State_type newState
    )
{
    MPTYTInfo *pMPTYTInfo = &(pMe->m_Transactions[tInfoIndex]);

    assert(pMPTYTInfo->m_pCurrentStateHandler != NULL);

    switch(newState)
    {
    case SSCallMPTYState_CALL_IN_MPTY:
        pMPTYTInfo->m_pCurrentStateHandler = SSOperationsMPTY_CallInMPTYState_Handler;
        pMPTYTInfo->m_pCurrentTInfo->callState.mptyState = newState;
        break;

    default:
        ASSERT_NOT_REACHABLE;
    }
}

/*=============================================================================
FUNCTION: SSOperationsMPTY_HandleEvent

DESCRIPTION: Entry point for events to the MPTY object.

PARAMETERS:
   pMe [in]            : Pointer to the MPTY object. 
   tInfoIndex [in]     : transaction index.
   eCode [in]          : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled, FALSE otherwise.

COMMENTS:
    Routing of the event to the appropriate CC instance was already done in 
    the CC. This information is conveyed to the MPTY by the tInfoIndex param.
=============================================================================*/
boolean SSOperationsMPTY_HandleEvent(
    CSSOperationsMPTY *pMe,
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
FUNCTION: SSOperationsMPTY_MoveToState

DESCRIPTION:
    Changes the current state for the currently processed MPTY instance.
    This function provides a partial OnEnter/OnExit mechanism:
    1. an ONEXIT event is sent to the current state.
    2. current state is changed to the new state.
    3. an ONENTER event is sent to the current (new) state.
    This mechanism is used mainly for setting and canceling timers.
   
PARAMETERS:
    pMe [in]             : Pointer to an MPTY object.
    newStateHandler [in] : new state, identified by its state handler.

RETURN VALUE:
    None.
=============================================================================*/
void SSOperationsMPTY_MoveToState(
    CSSOperationsMPTY        *pMe,
    MPTY_STATE_EVENT_HANDLER  newStateHandler
    )
{
    MPTYTInfo *pMPTYTInfo = &pMe->m_Transactions[pMe->m_currentlyProcessedTInfoIndex];

    // OnExit actions
    pMPTYTInfo->m_pCurrentStateHandler(pMe, CC_ONEXIT, 0, 0);

    // change state handler
    pMPTYTInfo->m_pCurrentStateHandler = newStateHandler;

    // OnEnter actions
    pMPTYTInfo->m_pCurrentStateHandler(pMe, CC_ONENTER, 0, 0);
}

/*=============================================================================
FUNCTION: MPTYTimeoutCallback

DESCRIPTION:
    Callback function passed to the BREW layer for MPTY timers expirations.
    This function forwards the timeout indication as a PostEvent of CC_TIMEOUT.
    This event will be processed in the next HandleEvent iteration of the SSO.
   
PARAMETERS:
    pUser [in] : user specific data. it is assumed to be pointing to an
                 SSOperationsMPTY_TimerDataType. this timer data is specific to
                 the MPTY instance that the expired timer belongs to.

RETURN VALUE:
    None
=============================================================================*/
static void MPTYTimeoutCallback(void *pUser)
{
    SSOperationsMPTY_TimerDataType* timerData = (SSOperationsMPTY_TimerDataType*) pUser;

    GSM1X_LOG_HIGH(timerData->pMe->m_pSSUtilCls, 
        "SSO:MPTYTimeoutCallback: tInfoIndex=%d timer id=%d", timerData->tInfoIndex, timerData->timerId, 0);
    if (timerData->timerId == SSO_TIMER_NONE)
    {
        GSM1X_LOG_HIGH(timerData->pMe->m_pSSUtilCls, "SSO:Warning: invalid MPTY timer id. event ignored",0 ,0 , 0);
        return;
    }

    // Post a message to the FSM
    SS_PostEvent(timerData->pMe->m_pIShell, AEECLSID_SS_OPERATIONS,
                 CC_TIMEOUT, (uint16)(timerData->tInfoIndex), (uint32)(timerData->timerId));

    timerData->timerId = SSO_TIMER_NONE;
}

/*=============================================================================
FUNCTION: SSOperationsMPTY_SetTimer

DESCRIPTION:
    Set a specific timer, using BREW timers, for a specific MPTY instance.
   
PARAMETERS:
   pMe [in]           : Pointer to the MPTY object.
   tInfoIndex [in]    : identifies the requesting MPTY instance.
   timerId [in]       : identifies the timer to be set.
   timerDuration [in] : timer duration.

RETURN VALUE:
    None

COMMENTS:
    Each MPTY instance, at any given time, can only use a single timer.
=============================================================================*/
static void SSOperationsMPTY_SetTimer(
    CSSOperationsMPTY    *pMe,
    SSCall_TInfo_Index_type tInfoIndex,
    SSO_TimerIdType       timerId, 
    SSO_TimerDurationType timerDuration
    )
{
    MPTYTInfo *pMPTYTInfo = &pMe->m_Transactions[tInfoIndex];

    pMPTYTInfo->m_timerData.pMe = pMe;
    pMPTYTInfo->m_timerData.timerId = timerId;

    CALLBACK_Init(
       &pMPTYTInfo->m_TimerCB, 
       MPTYTimeoutCallback, 
       &pMPTYTInfo->m_timerData
       );

    (void) ISHELL_SetTimerEx(
              pMe->m_pIShell,
              timerDuration,
              &pMPTYTInfo->m_TimerCB
              );
}

/*=============================================================================
FUNCTION: SSOperationsMPTY_CancelTimer

DESCRIPTION:
    Cancel a timer used by a specific MPTY instance.
   
PARAMETERS:
   pMe [in]        : Pointer to the MPTY object.
   tInfoIndex [in] : identifies the requesting MPTY instance.

RETURN VALUE:
    None

COMMENTS:
    Each MPTY instance, at any given time, can only use a single timer.
=============================================================================*/
static void SSOperationsMPTY_CancelTimer(
    CSSOperationsMPTY *pMe,
    SSCall_TInfo_Index_type tInfoIndex
    )
{
    MPTYTInfo *pMPTYTInfo = &pMe->m_Transactions[tInfoIndex];

    CALLBACK_Cancel(&pMPTYTInfo->m_TimerCB);
    pMPTYTInfo->m_timerData.timerId = SSO_TIMER_NONE;
}

///////////////////////////////////////////////////////////////////////////////
//   State Handlers Definitions
///////////////////////////////////////////////////////////////////////////////

/*=============================================================================
FUNCTION: SSOperationsMPTY_IdleState_Handler

DESCRIPTION:
   This is the Idle state event handler
   
PARAMETERS:
   pMe [in]   : Pointer to the MPTY object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMPTY_IdleState_Handler(
    CSSOperationsMPTY *pMe,
    AEEEvent           eCode,
    uint16             wParam,
    uint32             dwParam
)
{
    SSCall_TInfo_type* pTInfo;

    switch (eCode)
    {
    case MNCC_BUILD_MPTY_REQ:
        {
            SSEventCallwiseMsgData *pEventData = (SSEventCallwiseMsgData*)dwParam;
            SSMsg_int_msg_type* msg;

            // If the primitive holds a message, it must be FACILITY message
            assert ((pEventData->msg == NULL) || 
                    (pEventData->msg->gen_hdr.msg_type == SSMSG_MSG_TYPE_FACILITY));

            if (pEventData->msg == NULL)
            {
                // prepare our own FACILITY message
                msg = SSOMessage_AllocateMsg(SSMSG_MSG_TYPE_FACILITY);
                assert(msg != NULL);
                msg->facility.facility.hdr.present = TRUE;
                msg->facility.facility.component.gen_hdr.present = TRUE;
                msg->facility.facility.component.gen_hdr.component_type = SSMSG_COMPONENT_TYPE_INVOKE;
                msg->facility.facility.component.invoke.operation_code.hdr.present = TRUE;
                msg->facility.facility.component.invoke.operation_code.value = SSMSG_OPERATION_CODE_BUILDMPTY;
                
                // send the message
                SSOperationsCC_DataRequest(
                    pMe->m_pCC, 
                    pMe->m_currentlyProcessedTInfoIndex,
                    msg->gen_hdr.msg_type,
                    msg);
                
                SSOMessage_FreeMsg(msg);
            }
            else
            {
                // send the message
                SSOperationsCC_DataRequest(
                    pMe->m_pCC, 
                    pMe->m_currentlyProcessedTInfoIndex,
                    pEventData->msg->gen_hdr.msg_type,
                    pEventData->msg);
            }

            SSOEvent_FreeEventData((SSEventData*)pEventData);

            // update Call Database and switch states for the processed
            // MPTY instance
            pTInfo = pMe->m_Transactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            pTInfo->callState.mptyState = SSCallMPTYState_MPTY_REQUEST;
            MOVE_TO_STATE( SSOperationsMPTY_MPTYRequestState_Handler );

            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsMPTY_MPTYRequestState_Handler

DESCRIPTION:
   This is the MPTYRequest state event handler
   
PARAMETERS:
   pMe [in]   : Pointer to the MPTY object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMPTY_MPTYRequestState_Handler(
    CSSOperationsMPTY *pMe,
    AEEEvent           eCode,
    uint16             wParam,
    uint32             dwParam
 )
{
    SSEventDataIndData* pDataIndEventData;
    SSEventData* pEventData;
    SSCall_TInfo_type* pTInfo;
    SSCall_Index_type callIndex;

    switch (eCode)
    {
    case CC_ONENTER:
        // handle timers
        SSOperationsMPTY_SetTimer(pMe, pMe->m_currentlyProcessedTInfoIndex, 
            SSO_TIMER_TbuildMPTY, SSO_TIMER_TbuildMPTY_DURATION);
        return TRUE;

    case CC_ONEXIT:
        // stop timer
        SSOperationsMPTY_CancelTimer(pMe, pMe->m_currentlyProcessedTInfoIndex);
        return TRUE;

    case MNCC_DATA_INDICATION:
        pDataIndEventData = (SSEventDataIndData*)dwParam;
        
        switch(pDataIndEventData->msg->gen_hdr.msg_type)
        {
        case SSMSG_MSG_TYPE_FACILITY:
            if ((pDataIndEventData->msg->facility.facility.hdr.present == TRUE) &&
                (pDataIndEventData->msg->facility.facility.component.gen_hdr.component_type == SSMSG_COMPONENT_TYPE_RETURN_RESULT))
            {
                // Build MPTY was successful. send a BuildMPTY cnf to the MN
                SSCallDatabase_GetCallIndexByTInfoIndex(
                    pMe->m_pCallDatabase, 
                    pMe->m_currentlyProcessedTInfoIndex,
                    &callIndex);
                if (SSOperationsCC_SendCallwiseMsgEvent(
                    pMe->m_pCC, 
                    MNCC_BUILD_MPTY_CNF,
                    callIndex,
                    0,
                    pDataIndEventData->msg) == FALSE)
                {
                    return FALSE;
                }
            
                // update Call Database and switch states for the processed
                // MPTY instance
                pTInfo = pMe->m_Transactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
                // update of isMPTY is done by the Call Database in MergeCalls
                pTInfo->callState.mptyState = SSCallMPTYState_CALL_IN_MPTY;
            
                MOVE_TO_STATE( SSOperationsMPTY_CallInMPTYState_Handler );
            }
            else
            {
                // Build MPTY failed. send an err ind to the MN
                pEventData = SSOEvent_AllocateEventData(SS_EVENT_DATA_ERROR);
                assert (pEventData != NULL);
                SSCallDatabase_GetCallIndexByTInfoIndex(
                    pMe->m_pCallDatabase, 
                    pMe->m_currentlyProcessedTInfoIndex,
                    &pEventData->errorData.callIndex1);
                pEventData->errorData.isCallIndex2Valid = FALSE;
                pEventData->errorData.errorType = SS_ERROR_MPTY;
                pEventData->errorData.cause = SS_ERROR_BUILD_MPTY_ERROR;
                
                // queue the event
                SSOperationsCC_QueueEvent(
                    pMe->m_pCC, 
                    MNCC_ERR_IND, 
                    0, 
                    (uint32)pEventData
                    );
                
                // update Call Database and switch states for the processed
                // MPTY instance
                pTInfo = pMe->m_Transactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
                pTInfo->callState.mptyState = SSCallMPTYState_IDLE;
            
                MOVE_TO_STATE( SSOperationsMPTY_IdleState_Handler );
            }
            
            SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);
            return TRUE;
        }

    case CC_TIMEOUT:
        // dwParam holds the timer id
        if ((SSO_TimerIdType)dwParam != SSO_TIMER_TbuildMPTY)
            return FALSE;

        // Build MPTY failed. send an err ind to the MN
        pEventData = SSOEvent_AllocateEventData(SS_EVENT_DATA_ERROR);
        assert (pEventData != NULL);
        SSCallDatabase_GetCallIndexByTInfoIndex(
            pMe->m_pCallDatabase, 
            pMe->m_currentlyProcessedTInfoIndex,
            &pEventData->errorData.callIndex1);
        pEventData->errorData.isCallIndex2Valid = FALSE;
        pEventData->errorData.errorType = SS_ERROR_MPTY;
        pEventData->errorData.cause = SS_ERROR_BUILD_MPTY_TIMEOUT;
        
        // queue the event
        SSOperationsCC_QueueEvent(
            pMe->m_pCC, 
            MNCC_ERR_IND, 
            0, 
            (uint32)pEventData
            );
        
        // update Call Database and switch states for the processed
        // MPTY instance
        pTInfo = pMe->m_Transactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
        pTInfo->callState.mptyState = SSCallMPTYState_IDLE;
        
        MOVE_TO_STATE( SSOperationsMPTY_IdleState_Handler );

        return TRUE;
    }
    
    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsMPTY_CallInMPTYState_Handler

DESCRIPTION:
   This is the CallInMPTY state event handler
   
PARAMETERS:
   pMe [in]   : Pointer to the MPTY object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsMPTY_CallInMPTYState_Handler(
    CSSOperationsMPTY *pMe,
    AEEEvent           eCode,
    uint16             wParam,
    uint32             dwParam
 )
{
    return FALSE;
}
