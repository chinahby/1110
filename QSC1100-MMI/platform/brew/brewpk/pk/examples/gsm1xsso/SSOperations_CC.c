/*=============================================================================
FILE: SSOperations_CC.c

SERVICES: Provides the CC layer of the GSM1x SS Operations State Machine

GENERAL DESCRIPTION:
    The CC layer is placed under the MN layer of the State Machine.
    CC layer is composed of a CC Manager which manages CC instances. Every CC
    instance handles one transaction of a call, through the setup stage, active
    stage and call clearing phase.
    A regular call contains only one CC instance whereas a MPTY call can
    contain up to 5 CC instances, one for each possible remote party.
    Every CC instance is attached to a Hold SM instance and a MPTY SM instance.
    Each CC instance has a current state, which is composed of the three 
    current states of these state machines - CC state (Main state), Hold state
    and MPTY state.
    Events received at the CC can be handled by the CC Manager or routed to the
    appropriate CC instance.
    Events in the MN-CC interface are delivered through the EventQueue which is
    maintained by the FSM.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "SSOperations_CC.h"
#include "SSOperations_ClassId.h"
#include "SSOperations_UserEvents.h"
#include "SSCallDatabase.h"
#include "SSOperations_CommonEventDefinitions.h"
#include "SSOperations_CommonMessageDefinitions.h"
#include "SSOperations_FSM.h"
#include "SSOperations_Hold.h"
#include "SSOperations_MPTY.h"
#include "SS_CommonMacros.h"

#include "SSUtil.h"                         // SSUtil Class header
#include "OEMClassIDs.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

#define MOVE_TO_STATE(newStateHandler) \
    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:MOVE_TO_STATE: " #newStateHandler, 0, 0, 0); \
    SSOperationsCC_MoveToState(pMe, newStateHandler);

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

// state groups in the CC state machine
#define CC_STATEGROUP_ANYSTATE                              0
#define CC_STATEGROUP_ANYEXTERNALSTATE                      1
#define CC_STATEGROUP_ANYSTATEEXCEPTNULLANDRELEASEREQUEST   2
#define CC_STATEGROUP_ANYSTATEEXCEPTNULLANDMPTYREQUEST      3

typedef int SSOperationsCC_StateGroup;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

// Event handler for events that are common to all CC states
boolean SSOperationsCC_CommonEventHandler
(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

// Event handlers for every state in the CC state machine

// Null state
static boolean SSOperationsCCNullState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsCCMTCallConfState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsCCCallRecievedState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsCCConnectRequestState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsCCActiveState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsCCDisconnectRequestState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsCCReleaseRequestState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsCCCallInitState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

static boolean SSOperationsCCMOCallProceedingState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

void SSOperationsCC_InitializeCCCalls( CSSOperationsCC *pMe );

boolean IsStateInGroup(
    SSCall_State_type state, 
    SSOperationsCC_StateGroup stateGroup
    );

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*=============================================================================
FUNCTION: SSOperationsCC_New

DESCRIPTION: Create the CC Manager object

PARAMETERS:
    pIShell [in]      : Pointer to the IShell instance 
    parent [in]       : Pointer to the FSM object.
    pCallDatabase [in]: Pointer to a valid CallDatabase instance

RETURN VALUE:
    Pointer to the created SSOperationsCC or NULL on failure.
=============================================================================*/
CSSOperationsCC* SSOperationsCC_New(
    IShell                   *pIShell,
    struct _CSSOperationsFSM *parent,
    SSCallDatabase           *pCallDatabase
    )
{
    CSSOperationsCC* obj;

    obj = MALLOC(sizeof(CSSOperationsCC));
    if (obj == NULL)
        return NULL;

    obj->m_pIShell = pIShell;
    obj->m_pCallDatabase = pCallDatabase;
    obj->m_pFSM = parent;

    ISHELL_AddRef(pIShell);

    return obj;
}

/*=============================================================================
FUNCTION: SSOperationsCC_Initialize

DESCRIPTION: Initialize an CC object

PARAMETERS:
    pMe [in]: Pointer to the CC object to be initialized.

RETURN VALUE:
    TRUE on success or FALSE on failure.
=============================================================================*/
boolean SSOperationsCC_Initialize( CSSOperationsCC *pMe )
{
    // create the SS utilities class
    pMe->m_pSSUtilCls = NULL;
    if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_SSUTIL, (void **)&pMe->m_pSSUtilCls) != SUCCESS)
        return FALSE;

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: SSUtil created by CC", 0, 0, 0);

    pMe->m_pHold = NULL;
    if ((pMe->m_pHold = SSOperationsHold_New(pMe->m_pIShell, pMe, pMe->m_pCallDatabase)) == NULL)
        return FALSE;
    if (SSOperationsHold_Initialize(pMe->m_pHold) == FALSE)
        return FALSE;

    pMe->m_pMPTY = NULL;
    if ((pMe->m_pMPTY = SSOperationsMPTY_New(pMe->m_pIShell, pMe, pMe->m_pCallDatabase)) == NULL)
        return FALSE;
    if (SSOperationsMPTY_Initialize(pMe->m_pMPTY) == FALSE)
        return FALSE;

    // get a IPhone interface
    pMe->m_pIPhone = NULL;
    if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_PHONE, (void **)&pMe->m_pIPhone) != SUCCESS)
        return FALSE;
    
    pMe->m_currentlyProcessedTInfoIndex = 0xff; // no call is being processed

    // initialize CC instances according to the current state of the phone.
    SSOperationsCC_InitializeCCCalls(pMe);
    
    return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsCC_Release

DESCRIPTION: Destroy an CC object.

PARAMETERS:
     pMe [in]: Pointer to the CC object to be destroyed.

RETURN VALUE:
     None
=============================================================================*/
void SSOperationsCC_Release( CSSOperationsCC *pMe )
{
    // release the SS utilities class
    if (pMe->m_pSSUtilCls != NULL)
    {
        ISSUTILCLS_Release(pMe->m_pSSUtilCls);
        pMe->m_pSSUtilCls = NULL;
    }

    if (pMe->m_pHold != NULL)
    {
        SSOperationsHold_Release(pMe->m_pHold);
        pMe->m_pHold = NULL;
    }

    if (pMe->m_pMPTY != NULL)
    {
        SSOperationsMPTY_Release(pMe->m_pMPTY);
        pMe->m_pMPTY = NULL;
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
FUNCTION: SSOperationsCC_MoveToState

DESCRIPTION:
    Changes the current state for the currently processed CC instance.
    This function provides a partial OnEnter/OnExit mechanism:
    1. an ONEXIT event is sent to the current state.
    2. current state is changed to the new state.
    3. an ONENTER event is sent to the current (new) state.
    This mechanism is used mainly for setting and canceling timers.
   
PARAMETERS:
    pMe [in]             : Pointer to an CC object.
    newStateHandler [in] : new state, identified by its state handler.

RETURN VALUE:
    None.
=============================================================================*/
void SSOperationsCC_MoveToState(
    CSSOperationsCC    *pMe,
    CC_STATE_EVENT_HANDLER newStateHandler
    )
{
    CCTInfo *pCCTInfo = &pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex];

    // OnExit actions
    pCCTInfo->m_pCurrentStateHandler(pMe, CC_ONEXIT, 0, 0);

    // change state handler
    pCCTInfo->m_pCurrentStateHandler = newStateHandler;

    // OnEnter actions
    pCCTInfo->m_pCurrentStateHandler(pMe, CC_ONENTER, 0, 0);
}

/*=============================================================================
FUNCTION: SSOperationsCC_DropTch

DESCRIPTION:
    Drops the traffic channel using IPhone and inform the upper layer.
    Dropping the traffic channel will eventually result in closing the SSO.

PARAMETERS:
    pMe [in] : Pointer to an CC object.

RETURN VALUE:
    None.
=============================================================================*/
void SSOperationsCC_DropTch(CSSOperationsCC* pMe)
{
    IPHONE_EndCall(pMe->m_pIPhone);

    // send a drop tch ind to the MN
    SSOperationsCC_SendCallwiseMsgEvent(pMe, MNCC_DROP_TCH_IND, 0, 0, NULL);
}

/*=============================================================================
FUNCTION: IsStateInGroup

DESCRIPTION:
    Check if a given CC state belongs to a specific group of states.

PARAMETERS:
    state [in]      : the CC state to check
    stateGroup [in] : group of CC states

RETURN VALUE:
    TRUE if state belongs to stateGroup. FALSE otherwise.
=============================================================================*/
boolean IsStateInGroup(
    SSCall_State_type         state, 
    SSOperationsCC_StateGroup stateGroup  
    ) 
{
    switch(stateGroup) 
    {  
    case CC_STATEGROUP_ANYSTATE: 
        return TRUE;
        break;
    case CC_STATEGROUP_ANYEXTERNALSTATE:
        if (state.mainState != SSCallState_NULL &&
            state.mainState != SSCallState_RELEASE_REQUEST &&
            state.mainState != SSCallState_DISCONNECT_REQUEST)
            return TRUE;
        break;
    case CC_STATEGROUP_ANYSTATEEXCEPTNULLANDRELEASEREQUEST:
        if (state.mainState != SSCallState_NULL &&
            state.mainState != SSCallState_RELEASE_REQUEST)
            return TRUE;
        break;
    case CC_STATEGROUP_ANYSTATEEXCEPTNULLANDMPTYREQUEST:
        if (state.mainState != SSCallState_NULL &&
            state.mptyState != SSCallMPTYState_MPTY_REQUEST)
            return TRUE;
        break;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsCC_QueueEvent

DESCRIPTION: queues an event onto the MN-CC EventQueue. may be called from the
             Hold or from the MPTY state machines.

PARAMETERS:
   pMe   [in] : Pointer to the CC object.
   eCode [in] : Specifies the Event to queue.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None
=============================================================================*/
void SSOperationsCC_QueueEvent(
   CSSOperationsCC* pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
   )
{
    // forward it to the parent's QueueEvent
    SSOperationsFSM_QueueEvent(pMe->m_pFSM, eCode, wParam, dwParam);
}

/*=============================================================================
FUNCTION: CCTimeoutCallback

DESCRIPTION:
    Callback function passed to the BREW layer for CC timers expirations.
    This function forwards the timeout indication as a PostEvent of CC_TIMEOUT.
    This event will be processed in the next HandleEvent iteration of the SSO.
   
PARAMETERS:
    pUser [in] : user specific data. it is assumed to be pointing to an
                 SSOperationsCC_TimerDataType. this timer data is specific to
                 the CC instance that the expired timer belongs to.

RETURN VALUE:
    None
=============================================================================*/
static void CCTimeoutCallback(void *pUser)
{
    SSOperationsCC_TimerDataType* timerData = (SSOperationsCC_TimerDataType*) pUser;

    GSM1X_LOG_HIGH(timerData->pMe->m_pSSUtilCls, 
        "SSO:CCTimeoutCallback: tInfoIndex=%d timer id=%d", timerData->tInfoIndex, timerData->timerId, 0);

    // Post a message to the FSM
    (void) SS_PostEvent(timerData->pMe->m_pIShell, AEECLSID_SS_OPERATIONS,
                        CC_TIMEOUT, (uint16)(timerData->tInfoIndex), (uint32)(timerData->timerId));

    timerData->timerId = SSO_TIMER_NONE;
}

/*=============================================================================
FUNCTION: SSOperationsCC_SetTimer

DESCRIPTION:
    Set a specific timer, using BREW timers, for a specific CC instance.
   
PARAMETERS:
   pMe [in]           : Pointer to the CC object.
   tInfoIndex [in]    : identifies the requesting CC instance.
   timerId [in]       : identifies the timer to be set.
   timerDuration [in] : timer duration.

RETURN VALUE:
    None

COMMENTS:
    Each CC instance, at any given time, can only use a single timer.
=============================================================================*/
static void SSOperationsCC_SetTimer(
    CSSOperationsCC  *pMe,
    SSCall_TInfo_Index_type tInfoIndex,
    SSO_TimerIdType   timerId, 
    SSO_TimerDurationType timerDuration
    )
{
    CCTInfo *pCCTInfo = &pMe->m_ccTransactions[tInfoIndex];

    pCCTInfo->m_timerData.pMe = pMe;
    pCCTInfo->m_timerData.timerId = timerId;

    CALLBACK_Init(
       &pCCTInfo->m_TimerCB, 
       CCTimeoutCallback, 
       &pCCTInfo->m_timerData
       );

    (void) ISHELL_SetTimerEx(
              pMe->m_pIShell,
              timerDuration,
              &pCCTInfo->m_TimerCB
              );
}

/*=============================================================================
FUNCTION: SSOperationsCC_CancelTimer

DESCRIPTION:
    Cancel a timer used by a specific CC instance.
   
PARAMETERS:
   pMe [in]        : Pointer to the CC object.
   tInfoIndex [in] : identifies the requesting CC instance.

RETURN VALUE:
    None

COMMENTS:
    Each CC instance, at any given time, can only use a single timer.
=============================================================================*/
static void SSOperationsCC_CancelTimer(
    CSSOperationsCC  *pMe,
    SSCall_TInfo_Index_type tInfoIndex
    )
{
    CCTInfo *pCCTInfo = &pMe->m_ccTransactions[tInfoIndex];

    CALLBACK_Cancel(&pCCTInfo->m_TimerCB);
    pCCTInfo->m_timerData.timerId = SSO_TIMER_NONE;
}

///////////////////////////////////////////////////////////////////////////////
//   Data Request functions
///////////////////////////////////////////////////////////////////////////////

/*=============================================================================
FUNCTION: PrepareCallConfMessage

DESCRIPTION:
    Set default values for a CALL CONFIRM message.
   
PARAMETERS: 
    callConfMsg [in]: call confirmed message where values should be updated.
                      memory was allocated by calling function. message
                      type was set by calling function.

RETURN VALUE:
    None.
=============================================================================*/
void PrepareCallConfMessage(SSMsg_msg_call_confirmed_type* callConfMsg)
{
    callConfMsg->cause.hdr.present = TRUE;
    callConfMsg->cause.coding_standard = SSMSG_CODING_STANDARD_GSM;
    callConfMsg->cause.location = SSMSG_LOCATION_USER;
    callConfMsg->cause.diagnostic_len = 0;
    callConfMsg->cause.cause_value = SSMSG_CAUSE_VALUE_USER_BUSY;
}

/*=============================================================================
FUNCTION: PrepareDisconnectMessage

DESCRIPTION:
    Set default values for a Disconnect message.
   
PARAMETERS: 
    disconnectMsg [in]: disconnect message where values should be updated.
                        memory was allocated by calling function. message
                        type was set by calling function.

RETURN VALUE:
    None.
=============================================================================*/
void PrepareDisconnectMessage(SSMsg_msg_disconnect_type* disconnectMsg)
{
    disconnectMsg->cause.hdr.present = TRUE;
    disconnectMsg->cause.coding_standard = SSMSG_CODING_STANDARD_GSM;
    disconnectMsg->cause.location = SSMSG_LOCATION_USER;
    disconnectMsg->cause.diagnostic_len = 0;
    disconnectMsg->cause.cause_value = SSMSG_CAUSE_VALUE_NORMAL_CALL_CLEARING;
}

/*=============================================================================
FUNCTION: SSOperationsCC_DataRequest

DESCRIPTION:
   Prepares a GSM1x SS message to be sent to the remote party. Actual sending
   of the message is done by the FSM.
   This function sets the message header with the appropriate PD and TI values.
   
PARAMETERS:
   pMe [in]       : Pointer to the CC object.
   tInfoIndex [in]: transaction index identifying the sending CC instance.
   msgType [in]   : GSM1x SS message type
   msg [in]       : the message to send. if msg is NULL, a message of type 
                    msgType will be sent with default values.

RETURN VALUE:
    None.

SEE ALSO:
    SSOperationsFSM_SendSSMessage
=============================================================================*/
void SSOperationsCC_DataRequest(
    CSSOperationsCC    *pMe,
    SSCall_TInfo_Index_type   tInfoIndex,
    SSMsg_msg_type_type msgType,
    SSMsg_int_msg_type *msg
    )
{
    uint8 TI;

    // get a TI for the outgoing message
    SSCallDatabase_GetOutgoingTIByTInfoIndex(
        pMe->m_pCallDatabase,
        tInfoIndex,
        &TI);

    // send message
    if (msg == NULL)
    {
        msg = SSOMessage_AllocateMsg(msgType);
        assert(msg != NULL);

        SSOMessage_FillHeader(msg, TI);
        // for specific message types, set default values for message 
        // parameters
        switch (msgType)
        {
        case SSMSG_MSG_TYPE_CALL_CONFIRMED:
            PrepareCallConfMessage(&msg->call_confirmed);
            break;
        case SSMSG_MSG_TYPE_DISCONNECT:
            PrepareDisconnectMessage(&msg->disconnect);
            break;
        }

        SSOperationsFSM_SendSSMessage(pMe->m_pFSM, msg);
        SSOMessage_FreeMsg(msg);
    }
    else
    {
        SSOMessage_FillHeader(msg, TI);
        SSOperationsFSM_SendSSMessage(pMe->m_pFSM, msg);
    }

    // store the cause contained in the DISCONNECT message
    if (msgType == SSMSG_MSG_TYPE_DISCONNECT)
        pMe->m_ccTransactions[tInfoIndex].m_disconnectCauseValue = msg->disconnect.cause.cause_value;

}

/*=============================================================================
FUNCTION: SSOperationsCC_SendCallwiseMsgEvent

DESCRIPTION:
    Insert an event to the MN-CC EventQueue.
    This function allocate a new Event Data of type SS_EVENT_DATA_CALLWISE_MSG
    and initialize its members. This Event Data is the dwParam for the event
    inserted into the EventQueue.
   
PARAMETERS:
    pMe   [in] : Pointer to the CC object.
    eCode [in] : Specifies the event to queue.
    callIndex1, callIndex2, msg [in] : values for the event data.

RETURN VALUE:
    TRUE if the event was successfully inserted into the EventQueue or FALSE
    otherwise.

COMMENTS:
    This function doesn't store a reference to msg. Responsibility for msg 
    stays with the caller.
=============================================================================*/
boolean SSOperationsCC_SendCallwiseMsgEvent(
    CSSOperationsCC    *pMe,
    AEEEvent            eCode,
    SSCall_Index_type   callIndex1,
    SSCall_Index_type   callIndex2,
    SSMsg_int_msg_type *msg
    )
{
    SSEventData* pEventData = SSOEvent_PrepareCallwiseMsgEvent(callIndex1, callIndex2, msg);
    if (pEventData != NULL)
    {
        // queue the event
        SSOperationsFSM_QueueEvent(
            pMe->m_pFSM, 
            eCode, 
            0, 
            (uint32)pEventData
            );

        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsCC_SendTransactionwiseMsgEvent

DESCRIPTION:
    Insert an event to the MN-CC EventQueue.
    This function allocate a new Event Data of type 
    SS_EVENT_DATA_TRANSACTIONWISE_MSG and initialize its members. This Event
    Data is the dwParam for the event inserted into the EventQueue.
   
PARAMETERS:
    pMe   [in] : Pointer to the CC object.
    eCode [in] : Specifies the event to queue.
    callIndex1, callIndex2, msg [in] : values for the event data.

RETURN VALUE:
    TRUE if the event was successfully inserted into the EventQueue or FALSE
    otherwise.

COMMENTS:
    This function doesn't store a reference to msg. Responsibility for msg 
    stays with the caller.
=============================================================================*/
boolean SSOperationsCC_SendTransactionwiseMsgEvent(
    CSSOperationsCC   *pMe,
    AEEEvent          eCode,
    SSCall_TInfo_Index_type tInfoIndex,
    SSMsg_int_msg_type *msg
    )
{
    SSCall_Index_type callIndex;
    SSEventData* pEventData;
    boolean result;

    result = SSCallDatabase_GetCallIndexByTInfoIndex(
        pMe->m_pCallDatabase, 
        tInfoIndex,
        &callIndex);
    assert(result == TRUE);
    pEventData = SSOEvent_PrepareTransactionwiseMsgEvent(callIndex, tInfoIndex, msg);

    if (pEventData != NULL)
    {
        // queue the event
        SSOperationsFSM_QueueEvent(
            pMe->m_pFSM, 
            eCode, 
            0, 
            (uint32)pEventData
            );

        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsCC_InitMTCall

DESCRIPTION:
    Initialize data for a new Mobile Terminated call.
    New Call Database entry is created with the new call information.
    CC instance is allocated for handling this call and is initialized to the 
    Null state.
    Initialization is forwarded to the Hold and MPTY sub SM.

PARAMETERS:
    pMe [in]             : Pointer to the CC object.
    eventData [in]       : The Event Data from the event that indicated this
                           new call. It includes a SETUP message which holds
                           information about the new call.
    pNewTInfoIndex [out] : returns the new allocated transaction index for this
                           call.

RETURN VALUE:
    TRUE on success. FALSE if the new call could not be added to the Call
    Database.
=============================================================================*/
boolean SSOperationsCC_InitMTCall(
    CSSOperationsCC    *pMe,
    SSEventDataIndData *eventData,
    SSCall_TInfo_Index_type  *pNewTInfoIndex
    )
{
    SSCall_TInfo_type callTInfo;
    SSMsg_msg_setup_type *pSetupMsg = (SSMsg_msg_setup_type*)(eventData->msg);
    boolean success;
    char copyBuffer[MAX_SIZE_DIALER_TEXT];    
    SSCall_TInfo_Index_type newTInfoIndex;
    CCTInfo *pCCTInfo;

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:E:CC_InitMTCall", 0, 0, 0);

    // prepare call information to add to the Call Database
    callTInfo.callState.mainState = SSCallState_NULL;
    callTInfo.callState.holdState = SSCallHoldState_IDLE;
    callTInfo.callState.mptyState = SSCallMPTYState_IDLE;
    callTInfo.callStatus = SS_CALL_STATUS_SETUP;    
    callTInfo.callStartTime = ISHELL_GetUpTimeMS(pMe->m_pIShell);
    callTInfo.callOrigType = SSCall_MT;
    callTInfo.TIValue = eventData->msg->gen_hdr.tivalue;
    
    if (pSetupMsg->calling_party_bcd_number.hdr.present)
    {
        switch (pSetupMsg->calling_party_bcd_number.pi)
        {
        case SSMSG_PI_PRESENTATION_ALLOWED:
            callTInfo.callStringPI = SS_PI_ALLOWED;
            break;
            
        case SSMSG_PI_PRESENTATION_RESTRICTED:
            callTInfo.callStringPI = SS_PI_RESTRICTED;
            break;
            
        case SSMSG_PI_NUMBER_NOT_AVAILABLE_DUE_TO_INTERWORKING:
            callTInfo.callStringPI = SS_PI_UNAVAILABLE;
            break;
            
        default:
            // other pi values will not pass the translation of the message.
            assert(!"unexpected pi value");
            return FALSE;
        }
    }
    else
        // omitted calling_party_bcd_number is valid
        callTInfo.callStringPI = SS_PI_UNAVAILABLE;
    
    if ((pSetupMsg->calling_party_bcd_number.hdr.present == FALSE) ||
        (pSetupMsg->calling_party_bcd_number.pi != SSMSG_PI_PRESENTATION_ALLOWED))
        // copy an empty number into the Call Database
        *copyBuffer = '\0';
    else
        STRLCPY(copyBuffer, (char*)pSetupMsg->calling_party_bcd_number.number, sizeof(copyBuffer));
    
    // copy the bcd number into the call info structure,
    // turning it into a wide character version of the original string.
    STRTOWSTR(copyBuffer, callTInfo.callString, sizeof(callTInfo.callString));

    // create new entry in Call Database for the new call.
    if (SSCallDatabase_AddCall(
            pMe->m_pCallDatabase, 
            &callTInfo,
            NULL,
            &newTInfoIndex) == FALSE) 
    {
        // failed to add a call to the call database
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:CC_InitMTCall: SSCallDatabase_AddCall failed", 0, 0, 0);
        return FALSE;
    }

    // new transaction index was allocated for the new call and is indicated by
    // newTInfoIndex

    // init a new CC call info corresponding to the newTInfoIndex
    pCCTInfo = &(pMe->m_ccTransactions[newTInfoIndex]);
    
    success = SSCallDatabase_GetTransactionInfo(
        pMe->m_pCallDatabase, 
        newTInfoIndex,
        &(pCCTInfo->m_pCurrentTInfo));
    
    // we have just added it to the database so it must exist.
    assert(success == TRUE);
    
    // init the new call to the Null state.
    pCCTInfo->m_pCurrentStateHandler = SSOperationsCCNullState_Handler;
    // init timer data
    pCCTInfo->m_timerData.pMe = pMe;
    pCCTInfo->m_timerData.tInfoIndex = newTInfoIndex;
    pCCTInfo->m_timerData.timerId = SSO_TIMER_NONE;
    // inform Hold and MPTY state machines
    SSOperationsHold_InitCall(pMe->m_pHold, newTInfoIndex);
    SSOperationsMPTY_InitCall(pMe->m_pMPTY, newTInfoIndex);

    *pNewTInfoIndex = newTInfoIndex;

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:X:CC_InitMTCall", 0, 0, 0);

    return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsCC_InitMOCall

DESCRIPTION:
    Initialize data for a new Mobile Originated call.
    Call Database entry is assumed to already exist for the new call.
    CC instance is allocated for handling this call and is initialized to the 
    Null state.
    Initialization is forwarded to the Hold and MPTY sub SM.

PARAMETERS:
    pMe [in]           : Pointer to the CC object.
    newTInfoIndex [in] : transaction index of the new call.

RETURN VALUE:
    None.
=============================================================================*/
void SSOperationsCC_InitMOCall(
    CSSOperationsCC  *pMe,
    SSCall_TInfo_Index_type newTInfoIndex
    )
{
    boolean success;
    CCTInfo *pCCTInfo;
    
    // init a new CC call info
    pCCTInfo = &(pMe->m_ccTransactions[newTInfoIndex]);
    
    success = SSCallDatabase_GetTransactionInfo(
        pMe->m_pCallDatabase, 
        newTInfoIndex,
        &(pCCTInfo->m_pCurrentTInfo)
        );
    
    // this call database entry was added by the UI. it must exist.
    assert(success == TRUE);

    // init call state
    pCCTInfo->m_pCurrentTInfo->callState.mainState = SSCallState_NULL;
    pCCTInfo->m_pCurrentTInfo->callState.holdState = SSCallHoldState_IDLE;
    pCCTInfo->m_pCurrentTInfo->callState.mptyState = SSCallMPTYState_IDLE;

    // init the new call to the Null state.
    pCCTInfo->m_pCurrentStateHandler = SSOperationsCCNullState_Handler;
    // init timer data
    pCCTInfo->m_timerData.pMe = pMe;
    pCCTInfo->m_timerData.tInfoIndex = newTInfoIndex;
    pCCTInfo->m_timerData.timerId = SSO_TIMER_NONE;
    // inform Hold and MPTY state machines
    SSOperationsHold_InitCall(pMe->m_pHold, newTInfoIndex);
    SSOperationsMPTY_InitCall(pMe->m_pMPTY, newTInfoIndex);
}

/*=============================================================================
FUNCTION: SSOperationsCC_ReleaseTransaction

DESCRIPTION:
    Called upon call clearing, to release the corresponding transaction. It can
    be a regular call or one leg of a MPTY call.
    Release indication is forwarded to the Hold and MPTY sub SM.
    The transaction entry is removed from the Call Database.
    CC instance is released.

PARAMETERS:
    pMe [in]        : Pointer to the CC object.
    tInfoIndex [in] : transaction index of the cleared call.

RETURN VALUE:
    None.
=============================================================================*/
void SSOperationsCC_ReleaseTransaction(
    CSSOperationsCC  *pMe,
    SSCall_TInfo_Index_type tInfoIndex
    )
{
    CCTInfo *pCCTInfo = &(pMe->m_ccTransactions[tInfoIndex]);
        
    // inform Hold and MPTY state machines
    SSOperationsHold_ReleaseTransaction(pMe->m_pHold, tInfoIndex);
    SSOperationsMPTY_ReleaseTransaction(pMe->m_pMPTY, tInfoIndex);

    // remove entry from the Call Database
    if (pCCTInfo->m_pCurrentTInfo->callOrigType == SSCall_MO)
        SSCallDatabase_DeallocateMoTi(pMe->m_pCallDatabase, pCCTInfo->m_pCurrentTInfo->TIValue);

    SSCallDatabase_RemoveCallTransaction(pMe->m_pCallDatabase, tInfoIndex);

    // reset the CC call info
    pCCTInfo->m_pCurrentTInfo = NULL;
    pCCTInfo->m_pCurrentStateHandler = NULL;
}

/*=============================================================================
FUNCTION: SSOperationsCC_MergeCalls

DESCRIPTION:
    Called as part of a Build MPTY procedure.
    Merge two calls into one call that contains all the transactions of the
    two original calls. Transactions (CC instances) are moved from
    fromCallIndex to toCallIndex.
    The state of all CC instances is changed to Active, Idle, MPTY.

PARAMETERS:
    pMe [in]           : Pointer to the CC object.
    toCallIndex [in]   : call index of the destination call.
    fromCallIndex [in] : call index of the source call.

RETURN VALUE:
    None.

COMMENTS:
    the fromCallIndex call is assumed to refer to an active call and it should
    only contain one transaction.
    the toCallIndex may refer to an already released call. 
=============================================================================*/
void SSOperationsCC_MergeCalls(
    CSSOperationsCC  *pMe,
    SSCall_Index_type toCallIndex,
    SSCall_Index_type fromCallIndex
    )
{
    SSCall_TInfo_Index_type fromTInfoIndex, tInfoIndex;
    boolean result;
    SSCall_Info_type *fromCallInfo;

    result = SSCallDatabase_GetTInfoIndexByCallIndex(
        pMe->m_pCallDatabase, fromCallIndex, &fromTInfoIndex);
    assert(result == TRUE);
    result = SSCallDatabase_GetCallInfo(pMe->m_pCallDatabase, fromCallIndex, &fromCallInfo);
    assert(result == TRUE);

    result = SSCallDatabase_IsCallPresent(pMe->m_pCallDatabase, toCallIndex);

    if (result == TRUE)
    {
        // two active calls. do a merge.
        result = SSCallDatabase_MergeCalls(
            pMe->m_pCallDatabase, fromTInfoIndex, toCallIndex);
        assert(result == TRUE);
        
        // set all CC instances to active,idle,MPTY state
        for (tInfoIndex=0; tInfoIndex<SSO_CC_MAX_TRANSACTIONS_NUMBER; ++tInfoIndex)
        {
            CCTInfo* pCCTInfo = &(pMe->m_ccTransactions[tInfoIndex]);
            // handle used CC instances
            if (pCCTInfo->m_pCurrentStateHandler != NULL)
            {
                assert(pCCTInfo->m_pCurrentStateHandler == SSOperationsCCActiveState_Handler);
                assert(pCCTInfo->m_pCurrentTInfo->callState.mainState == SSCallState_ACTIVE);
                SSOperationsHold_SetState(pMe->m_pHold, tInfoIndex, SSCallHoldState_IDLE);
                SSOperationsMPTY_SetState(pMe->m_pMPTY, tInfoIndex, SSCallMPTYState_CALL_IN_MPTY);
            }
        }
    }
    else
    {
        // only one call exists.
        // update call database
        if (fromCallInfo->isMPTY == FALSE)
        {
            fromCallInfo->isMPTY = TRUE;
            fromCallInfo->MPTYStartTime = ISHELL_GetUpTimeMS(pMe->m_pShell);
        }
        fromCallInfo->isHeld = FALSE;
        
        // update CC instance
        SSOperationsHold_SetState(pMe->m_pHold, fromTInfoIndex, SSCallHoldState_IDLE);
        SSOperationsMPTY_SetState(pMe->m_pMPTY, fromTInfoIndex, SSCallMPTYState_CALL_IN_MPTY);
    }
}

/*=============================================================================
FUNCTION: SSOperationsCC_HandleSetupReq

DESCRIPTION: 
    Handles a setup request received from the UI.
    Call Database entry is assumed to already exist for the new call.
    Ensures that a new MO call can be set-up.

PARAMETERS:
    pMe [in]           : Pointer to the CC object.
    newTInfoIndex [in] : transaction index of the new call.

RETURN VALUE:
    TRUE  - MO Call is allowed to be established.
    FALSE - MO Call can't be established.

COMMENTS:
    A new MO call can not be set-up if another call already exist and:
    - the other call is active (not held), or
    - the other call is a MPTY call.
=============================================================================*/
boolean SSOperationsCC_HandleSetupReq(
    CSSOperationsCC *pMe,
    SSCall_TInfo_Index_type newTInfoIndex
)
{
    SSCall_Info_type *callInfo;
    boolean result;
    SSCall_Index_type callIndex;
    
    result = SSCallDatabase_GetCallIndexByTInfoIndex(
        pMe->m_pCallDatabase, 
        newTInfoIndex, 
        &callIndex);
    assert(result == TRUE);
    // new MO call
    SSOperationsCC_InitMOCall(pMe, newTInfoIndex);
    
    // we need to ensure that there is no other active or MPTY call
    result = SSCallDatabase_GetCallInfo(
                pMe->m_pCallDatabase, 
                GET_OTHER_CALL_INDEX(callIndex),
                &callInfo);
    if (result == FALSE)
        // no other call exists
        return TRUE;

    // check the other call
    if ((callInfo->isHeld == FALSE) || (callInfo->isMPTY == TRUE))
    {
        // other call is active or MPTY
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, 
            "SSO:Warning: unexpected SETUP request. other call state: held=%d,MPTY=%d",
            callInfo->isHeld, 
            callInfo->isMPTY, 0);
        SSOperationsCC_ReleaseTransaction(pMe, newTInfoIndex);
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsCC_HandleSetupInd

DESCRIPTION: 
    Handles a setup indication received from the network.
    Checks if a new MT call is allowed to be set-up. If not, a RELEASE COMPLETE
    message is sent to the network for rejecting the new call.

PARAMETERS:
    pMe [in]           : Pointer to the CC object.
    pEventData [in]    : The Event Data from the event that indicated this
                         new call. It includes a SETUP message which holds
                         information about the new call.
    newTInfoIndex [out]: returns the new allocated transaction index for this
                         call.

RETURN VALUE:
    TRUE  - MT Call is allowed to be established.
    FALSE - MT Call can't be established.

=============================================================================*/
boolean SSOperationsCC_HandleSetupInd(
    CSSOperationsCC *pMe,
    SSEventDataIndData* pEventData,
    SSCall_TInfo_Index_type* newTInfoIndex
)
{
    SSCall_Index_type callIndex;
    SSCall_Info_type* pCallInfo;
    boolean result;

    // ensure that there is no other MPTY call
    for (callIndex = 0; callIndex<MAX_DATABASE_CALL_ENTRIES; ++callIndex)
    {
        if ((SSCallDatabase_GetCallInfo(
            pMe->m_pCallDatabase, 
            callIndex,
            &pCallInfo) == TRUE) &&
            (pCallInfo->isMPTY == TRUE))
        {
            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, 
                "SSO:Warning: unexpected SETUP message while MPTY exists.", 0, 0, 0);
            // release the transaction
            SSOperationsFSM_SendReleaseComplete(pMe->m_pFSM, 
                COMPOSE_TI(1, pEventData->msg->gen_hdr.tivalue),
                // user busy cause was selected arbitrarily
                SSMSG_CAUSE_VALUE_USER_BUSY);
            
            return FALSE;
        }
    }
    // MT call is allowed to proceed
    result = SSOperationsCC_InitMTCall(pMe, pEventData, newTInfoIndex);
    
    return result;
}

/*=============================================================================
FUNCTION: SSOperationsCC_HandleEvent

DESCRIPTION: 
    Entry point for events to the CC Manager.
    The received events are handled by the CC Manager or routed to the 
    appropriate CC instance.

PARAMETERS:
   pMe [in]   : Pointer to the CC object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled, FALSE otherwise.
=============================================================================*/
boolean SSOperationsCC_HandleEvent(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,      
   uint32           dwParam      
   )
{
    SSCall_Index_type callIndex;         // call index carried by the event
    SSCall_TInfo_Index_type tInfoIndex;  // transaction index carried by the event
    SSEventData *pEvent;
    boolean result;
    
    switch (eCode)
    {
    case MNCC_SETUP_REQ:
    case MNCC_SETUP_RSP:
    case MNCC_ALERT_REQ:
    // MNCC_HOLD_REQ and MNCC_RETRIEVE_REQ should have special handling when 
    // HoldMPTY will be supported
    case MNCC_HOLD_REQ:     
    case MNCC_RETRIEVE_REQ: 
        pEvent = (SSEventData*)dwParam;

        // all MNCC primitive must have an Event Data.
        assert (pEvent != NULL);
        // these primitives must have an Event Data of type 
        // SS_EVENT_DATA_CALLWISE_MSG
        assert (pEvent->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);
        
        // for these primitives, routing to the CC instance is based on 
        // callIndex1
        callIndex = pEvent->callwiseMsgData.callIndex1;
        result = SSCallDatabase_GetTInfoIndexByCallIndex(
                    pMe->m_pCallDatabase, 
                    callIndex, 
                    &tInfoIndex);
        assert(result == TRUE);

        // handle a request for new MO call
        if (eCode == MNCC_SETUP_REQ)
        {
            if (SSOperationsCC_HandleSetupReq(pMe, tInfoIndex) == FALSE)
            {
                // MO Call cannot be established. 
                // inform the UI. reuse the pEvent memory.
				FREEIF(pEvent->callwiseMsgData.msg);
				pEvent->commonData.type = SS_EVENT_DATA_ERROR;
				pEvent->errorData.callIndex1 = callIndex; // unchanged.
				pEvent->errorData.isCallIndex2Valid = FALSE;
				pEvent->errorData.errorType = SS_ERROR_UI_REQ_REJECTED; 
				pEvent->errorData.cause = SS_ERROR_CAUSE_CANNOT_SETUP_NEW_CALL;

				SS_PostEvent(pMe->m_pIShell, AEECLSID_SS_OPERATIONS, 
					UIMN_ERR_IND, 0, (uint32)pEvent);
				return TRUE;
            }
        }

        break;
        
    case MNCC_DISC_REQ:
        pEvent = (SSEventData*)dwParam;

        // MNCC primitive must have an Event Data.
        assert (pEvent != NULL);
        // these primitives must have an Event Data of type SS_EVENT_DATA_TRANSACTIONWISE_MSG
        assert (pEvent->commonData.type == SS_EVENT_DATA_TRANSACTIONWISE_MSG);
        
        // for this primitive, routing to the CC instance is based on 
        // tInfoIndex
        tInfoIndex = pEvent->transactionwiseMsgData.tInfoIndex;
        break;

    case MNCC_BUILD_MPTY_REQ:
        pEvent = (SSEventData*)dwParam;

        // MNCC primitive must have an Event Data.
        assert (pEvent != NULL);
        // these primitives must have an Event Data of type SS_EVENT_DATA_CALLWISE_MSG
        assert (pEvent->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);
        
        // for this primitive, routing to the CC instance is based on 
        // callIndex2 (fromCallIndex)
        callIndex = pEvent->callwiseMsgData.callIndex2;
        result = SSCallDatabase_GetTInfoIndexByCallIndex(
                    pMe->m_pCallDatabase, 
                    callIndex, 
                    &tInfoIndex);
        assert(result == TRUE);
        break;

    case MNCC_MERGE_CALLS_REQ:
        {
            SSCall_Index_type toCallIndex, fromCallIndex;

            pEvent = (SSEventData*)dwParam;
            toCallIndex = pEvent->callwiseMsgData.callIndex1;
            fromCallIndex = pEvent->callwiseMsgData.callIndex2;
            
            SSOperationsCC_MergeCalls(pMe, toCallIndex, fromCallIndex);

            // send the confirmation to the MN
            SSOperationsCC_SendCallwiseMsgEvent(
                pMe, 
                MNCC_MERGE_CALLS_CNF,
                toCallIndex, 
                fromCallIndex, 
                pEvent->callwiseMsgData.msg);

            SSOEvent_FreeEventData(pEvent);

            // handled by the CC Manager (not routed further to any CC 
            // instance)
            return TRUE;
        }
        break;


    case MNCC_DATA_INDICATION:
        pEvent = (SSEventData*)dwParam;

        // MNCC primitive must have an Event Data.
        assert (pEvent != NULL);
        // this primitive must have an Event Data of type SS_EVENT_DATA_DATA_IND
        assert (pEvent->commonData.type == SS_EVENT_DATA_DATA_IND);
        // this primitive must have a GSM1x message
        assert (pEvent->dataIndData.msg != NULL);
        
        if (SSOperationsFSM_CheckIncomingTI(
                pMe->m_pFSM, 
                COMPOSE_TI(pEvent->dataIndData.msg->gen_hdr.tiflag,pEvent->dataIndData.msg->gen_hdr.tivalue),
                pEvent->dataIndData.msg->gen_hdr.msg_type) == FALSE)
        {
            // message contains an invalid TI
            GSM1X_LOG_HIGH(
                pMe->m_pSSUtilCls, 
                "SSO: DI with invalid TI=%d. msg type=%d", 
                COMPOSE_TI(pEvent->dataIndData.msg->gen_hdr.tiflag,pEvent->dataIndData.msg->gen_hdr.tivalue), 
                pEvent->dataIndData.msg->gen_hdr.msg_type, 
                0);
            SSOEvent_FreeEventData(pEvent);
            return TRUE;
        }

        // for this primitive, routing to the CC instance is based on TI
        // check for SETUP message
        if (pEvent->dataIndData.msg->gen_hdr.msg_type == SSMSG_MSG_TYPE_SETUP)
        {
            if (SSOperationsCC_HandleSetupInd(pMe, &pEvent->dataIndData, &tInfoIndex) == FALSE)
            {
                // MT call is not allowed to be set-up
                SSOEvent_FreeEventData(pEvent);
                return TRUE;
            }
        }
        else
        {
            uint8 TI = COMPOSE_TI(
                        pEvent->dataIndData.msg->gen_hdr.tiflag, 
                        pEvent->dataIndData.msg->gen_hdr.tivalue);

            result = SSCallDatabase_GetTInfoIndexByIncomingTI(
                        pMe->m_pCallDatabase, TI, &tInfoIndex);
            // we already checked the incoming TI and if the message was not 
            // SETUP then there must be an appropriate record in CallDatabase.
            assert (result == TRUE);

            // check that the CC instance is initialized
            if (pMe->m_ccTransactions[tInfoIndex].m_pCurrentStateHandler == NULL)
            {
                // one case this can happen: UI already added a record in Call 
                // Database with a new allocated TI. Before the FSM handles the 
                // SETUP_REQ, the FSM gets DI with the same TI.
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,
                    "SSO: CC got DI with valid TI=%d for un-init instance=%d", TI, tInfoIndex, 0);
                
                // act as if the TI is unallocated
                SSOperationsFSM_SendReleaseComplete(pMe->m_pFSM, 
                    COMPOSE_TI(!GET_TI_FLAG(TI), GET_TI_VALUE(TI)),
                    SSMSG_CAUSE_VALUE_INVALID_TRANSACTION_IDENTIFIER_VALUE);
                
                SSOEvent_FreeEventData(pEvent);
                return TRUE;
            }
        }

        break;

    case CC_TIMEOUT:
        // timeout from one of the CC timers.
        // wParam holds the transaction index
        tInfoIndex = (SSCall_TInfo_Index_type)wParam;
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,
            "SSO: Received CC_TIMEOUT tInfoIndex=%d", tInfoIndex, 0, 0);
        break;

    case CC_HOLD_TIMEOUT_IND:
        // timeout from the Hold sub SM.
        // wParam holds the transaction index
        tInfoIndex = (SSCall_TInfo_Index_type)wParam;
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,
            "SSO: Received CC_HOLD_TIMEOUT_IND tInfoIndex=%d", tInfoIndex, 0, 0);
        break;

    default:
        // other events are not handled by the CC.
        return FALSE;
    }
        
	// check that tInfoIndex is valid
	if (!SSCallDatabase_GetCallIndexByTInfoIndex(pMe->m_pCallDatabase, tInfoIndex, &callIndex))
	{
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,
            "SSO: CC got invalid tInfoIndex=%d", tInfoIndex, 0, 0);
		return FALSE;
	}

    pMe->m_currentlyProcessedTInfoIndex = tInfoIndex;

    // let the appropriate CC instance handle the event
    if (FALSE == pMe->m_ccTransactions[tInfoIndex].m_pCurrentStateHandler(pMe, eCode, wParam, dwParam))
    {
        // try the common handler
        return SSOperationsCC_CommonEventHandler(pMe, eCode, wParam, dwParam);
    }
    else
        return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsCC_IsAnotherActiveTransactionExists

DESCRIPTION: 
    Utility function to check if there exist another call which is not in the
    Call Clearing Phase.

PARAMETERS:
   pMe [in]        : Pointer to the CC object. 
   tInfoIndex [in] : transaction index of the asking CC instance.

RETURN VALUE:
    TRUE if a matching call was found, FALSE otherwise.
=============================================================================*/
boolean SSOperationsCC_IsAnotherActiveTransactionExists(
    CSSOperationsCC *pMe,
    SSCall_TInfo_Index_type tInfoIndex
)
{
    SSCall_TInfo_Index_type curTInfoIndex;
    SSCall_TInfo_type* pTInfo;

    for (curTInfoIndex=0; curTInfoIndex<MAX_CALL_TRANSACTION_INFO_NUMBER; ++curTInfoIndex)
    {
        // ignore the given transaction index
        if (curTInfoIndex == tInfoIndex)
            continue;
        // check for active transaction
        if (SSCallDatabase_GetTransactionInfo(
                pMe->m_pCallDatabase, 
                curTInfoIndex, 
                &pTInfo) &&
            (pTInfo->callStatus != SS_CALL_STATUS_DISCONNECTED))
        {
            return TRUE;
        }
    }

    return FALSE;
}
/*=============================================================================
FUNCTION: SSOperationsCC_CommonEventHandler

DESCRIPTION: Handle events that are common to all CC states

PARAMETERS:
   pMe [in]   : Pointer to the CC object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled, FALSE otherwise.
=============================================================================*/
boolean SSOperationsCC_CommonEventHandler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
   )
{
    SSCall_TInfo_type *tInfo;

    tInfo = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;

    switch (eCode)
    {
    case MNCC_DISC_REQ:
        if (IsStateInGroup(
                tInfo->callState, 
                CC_STATEGROUP_ANYEXTERNALSTATE))
        {
            SSEventTransactionwiseMsgData *pEventData = 
                (SSEventTransactionwiseMsgData*)dwParam;

            // do some checking
            // transaction index must match
            assert (pEventData->tInfoIndex == pMe->m_currentlyProcessedTInfoIndex);
            
            // If the primitive holds a message, it must be DISCONNECT message
            assert ((pEventData->msg == NULL) || 
                    (pEventData->msg->gen_hdr.msg_type == SSMSG_MSG_TYPE_DISCONNECT));

            // drop traffic channel if there is no other active transaction
            if (!SSOperationsCC_IsAnotherActiveTransactionExists(pMe, pMe->m_currentlyProcessedTInfoIndex))
            {
                SSOperationsCC_DropTch(pMe);
                
                SSOEvent_FreeEventData((SSEventData*)pEventData);
                
                return TRUE;
            }

            // send DISCONNECT
            SSOperationsCC_DataRequest(
                pMe, 
                pMe->m_currentlyProcessedTInfoIndex,
                SSMSG_MSG_TYPE_DISCONNECT,
                pEventData->msg);

            SSOEvent_FreeEventData((SSEventData*)pEventData);

            // update Call Database and switch states for the processed
            // CC instance
            tInfo->callStatus = SS_CALL_STATUS_DISCONNECTED;
            tInfo->callState.mainState = SSCallState_DISCONNECT_REQUEST;
            MOVE_TO_STATE( SSOperationsCCDisconnectRequestState_Handler );

            return TRUE;
        }
        break;

    case MNCC_DATA_INDICATION:
        {
            SSEventDataIndData  *pDataIndEventData = (SSEventDataIndData*)dwParam;
            
            switch (pDataIndEventData->msg->gen_hdr.msg_type)
            {
            case SSMSG_MSG_TYPE_RELEASE:
                if (!IsStateInGroup(
                        tInfo->callState, 
                        CC_STATEGROUP_ANYSTATEEXCEPTNULLANDRELEASEREQUEST))
                    return FALSE;
                // ignore clearing message if only one transaction exist.
                if (SSCallDatabase_GetCallTInfoCount(pMe->m_pCallDatabase) < 2)
                    return FALSE;
                // drop traffic channel if there is no other active transaction
                else if (!SSOperationsCC_IsAnotherActiveTransactionExists(pMe, pMe->m_currentlyProcessedTInfoIndex))
                {
                    SSOperationsCC_DropTch(pMe);
                    
                    SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);
                    
                    return TRUE;
                }

                // send a rel ind to the MN
                if (SSOperationsCC_SendTransactionwiseMsgEvent(
                        pMe, 
                        MNCC_REL_IND,
                        pMe->m_currentlyProcessedTInfoIndex,
                        pDataIndEventData->msg) == FALSE)
                {
                    return FALSE;
                }

                // send RELEASE COMPLETE
                SSOperationsCC_DataRequest(
                    pMe, 
                    pMe->m_currentlyProcessedTInfoIndex,
                    SSMSG_MSG_TYPE_RELEASE_COMPLETE,
                    NULL);

                // update Call Database and switch states for the processed
                // CC instance
                tInfo->callState.mainState = SSCallState_NULL;
                MOVE_TO_STATE( SSOperationsCCNullState_Handler );
                SSOperationsCC_ReleaseTransaction(pMe, pMe->m_currentlyProcessedTInfoIndex);
                
                SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);

                return TRUE;
                break;

            case SSMSG_MSG_TYPE_DISCONNECT:
                if (!IsStateInGroup(
                        tInfo->callState, 
                        CC_STATEGROUP_ANYSTATEEXCEPTNULLANDRELEASEREQUEST))
                    return FALSE;
                // ignore clearing message if only one transaction exist.
                if (SSCallDatabase_GetCallTInfoCount(pMe->m_pCallDatabase) < 2)
                    return FALSE;
                // drop traffic channel if there is no other active transaction
                else if (!SSOperationsCC_IsAnotherActiveTransactionExists(pMe, pMe->m_currentlyProcessedTInfoIndex))
                {
                    SSOperationsCC_DropTch(pMe);
                    
                    SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);
                    
                    return TRUE;
                }

                // send a disc ind to the MN
                if (SSOperationsCC_SendTransactionwiseMsgEvent(
                        pMe, 
                        MNCC_DISC_IND,
                        pMe->m_currentlyProcessedTInfoIndex,
                        pDataIndEventData->msg) == FALSE)
                {
                    return FALSE;
                }

                // send RELEASE
                SSOperationsCC_DataRequest(
                    pMe, 
                    pMe->m_currentlyProcessedTInfoIndex,
                    SSMSG_MSG_TYPE_RELEASE,
                    NULL);

                // update Call Database and switch states for the processed
                // CC instance
                pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_T308Count = 0;
                tInfo->callStatus = SS_CALL_STATUS_DISCONNECTED;
                tInfo->callState.mainState = SSCallState_RELEASE_REQUEST;
                MOVE_TO_STATE( SSOperationsCCReleaseRequestState_Handler );
                
                SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);

                return TRUE;
                break;

            case SSMSG_MSG_TYPE_RELEASE_COMPLETE:
                if (!IsStateInGroup(
                        tInfo->callState, 
                        CC_STATEGROUP_ANYSTATE))
                    return FALSE;
                // ignore clearing message if only one transaction exist.
                if (SSCallDatabase_GetCallTInfoCount(pMe->m_pCallDatabase) < 2)
                    return FALSE;
                // drop traffic channel if there is no other active transaction
                else if (!SSOperationsCC_IsAnotherActiveTransactionExists(pMe, pMe->m_currentlyProcessedTInfoIndex))
                {
                    SSOperationsCC_DropTch(pMe);
                    
                    SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);
                    
                    return TRUE;
                }

                // send a rel cnf to the MN
                if (SSOperationsCC_SendTransactionwiseMsgEvent(
                        pMe, 
                        MNCC_REL_CNF,
                        pMe->m_currentlyProcessedTInfoIndex,
                        pDataIndEventData->msg) == FALSE)
                {
                    return FALSE;
                }

                // update Call Database and switch states for the processed
                // CC instance
                tInfo->callState.mainState = SSCallState_NULL;
                MOVE_TO_STATE( SSOperationsCCNullState_Handler );
                SSOperationsCC_ReleaseTransaction(pMe, pMe->m_currentlyProcessedTInfoIndex);

                SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);

                return TRUE;
                break;

            case SSMSG_MSG_TYPE_FACILITY:
                if (!IsStateInGroup(
                        tInfo->callState, 
                        CC_STATEGROUP_ANYSTATEEXCEPTNULLANDMPTYREQUEST))
                    return FALSE;

                // send a facility ind to the MN
                if (SSOperationsCC_SendTransactionwiseMsgEvent(
                        pMe, 
                        MNCC_FACILITY_IND,
                        pMe->m_currentlyProcessedTInfoIndex,
                        pDataIndEventData->msg) == FALSE)
                {
                    return FALSE;
                }

                SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);
                return TRUE;

            default:
                return FALSE;
            }
        } // case MNCC_DATA_INDICATION
    } // switch (eCode)

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsCC_InitializeCCCalls

DESCRIPTION: 
    initialize CC instances according to the current state of the phone.
    The call database should be initialized before calling this function.

PARAMETERS:
    pMe [in]: Pointer to the CC object.

RETURN VALUE:
    None.
=============================================================================*/
void SSOperationsCC_InitializeCCCalls( CSSOperationsCC *pMe )
{
    SSCall_TInfo_type *pTInfo;
    SSCall_TInfo_Index_type tInfoIndex;
    boolean callExists, callFound = FALSE;

    // iterate through existing transactions
    for (tInfoIndex = 0; tInfoIndex < SSO_CC_MAX_TRANSACTIONS_NUMBER; ++tInfoIndex)
    {
        callExists = SSCallDatabase_GetTransactionInfo(pMe->m_pCallDatabase, tInfoIndex, &pTInfo);

        if (callExists)
        {
            //we assume that there is at most one call and it must be active
            assert (!callFound);
            callFound = TRUE;
            assert((pTInfo->callState.mainState == SSCallState_ACTIVE) &&
                   (pTInfo->callState.holdState == SSCallHoldState_IDLE) &&
                   (pTInfo->callState.mptyState == SSCallMPTYState_IDLE));

            pMe->m_ccTransactions[tInfoIndex].m_pCurrentTInfo = pTInfo;
            pMe->m_ccTransactions[tInfoIndex].m_pCurrentStateHandler = SSOperationsCCActiveState_Handler;
            pMe->m_ccTransactions[tInfoIndex].m_timerData.pMe = pMe;
            pMe->m_ccTransactions[tInfoIndex].m_timerData.tInfoIndex = tInfoIndex;
            pMe->m_ccTransactions[tInfoIndex].m_timerData.timerId = SSO_TIMER_NONE;
            // inform Hold and MPTY state machines
            SSOperationsHold_InitCall(pMe->m_pHold, tInfoIndex);
            SSOperationsMPTY_InitCall(pMe->m_pMPTY, tInfoIndex);
        }
        else
        {
            pMe->m_ccTransactions[tInfoIndex].m_pCurrentTInfo = NULL;
            pMe->m_ccTransactions[tInfoIndex].m_pCurrentStateHandler = NULL;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//   State Handlers Definitions
///////////////////////////////////////////////////////////////////////////////

/*=============================================================================
FUNCTION: SSOperationsCCNullState_Handler

DESCRIPTION:
   This is the Null state event handler.
   
PARAMETERS:
   pMe [in]   : Pointer to the CC object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsCCNullState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
   )
{
    SSCall_TInfo_type* pTInfo;
    SSCall_Index_type callIndex;

    switch (eCode)
    {
    case MNCC_DATA_INDICATION:
        {
            SSEventDataIndData* pDataIndEventData = (SSEventDataIndData*)dwParam;

            // check for SETUP message
            if (pDataIndEventData->msg->gen_hdr.msg_type != SSMSG_MSG_TYPE_SETUP)
                return FALSE;
            
            // send a setup ind to the MN
            SSCallDatabase_GetCallIndexByTInfoIndex(
                pMe->m_pCallDatabase, 
                pMe->m_currentlyProcessedTInfoIndex,
                &callIndex);
            if (SSOperationsCC_SendCallwiseMsgEvent(
                    pMe, 
                    MNCC_SETUP_IND, 
                    callIndex,
                    0,
                    pDataIndEventData->msg) == FALSE)
            {
                return FALSE;
            }
                   
            // send CALL CONFIRMED
            SSOperationsCC_DataRequest(
                pMe, 
                pMe->m_currentlyProcessedTInfoIndex, 
                SSMSG_MSG_TYPE_CALL_CONFIRMED,
                NULL);

            // update Call Database and switch states for the processed
            // CC instance
            pTInfo = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            pTInfo->callState.mainState = SSCallState_MT_CALL_CONF;
            MOVE_TO_STATE( SSOperationsCCMTCallConfState_Handler );
            
            SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);
            return TRUE;
        }

    case MNCC_SETUP_REQ:
        {
            SSEventCallwiseMsgData *pEventData = (SSEventCallwiseMsgData*)dwParam;
            
            // the primitive must hold a SETUP message
            assert ((pEventData->msg != NULL) &&
                    (pEventData->msg->gen_hdr.msg_type == SSMSG_MSG_TYPE_SETUP));

            // send SETUP
            SSOperationsCC_DataRequest(
                pMe, 
                pMe->m_currentlyProcessedTInfoIndex, 
                SSMSG_MSG_TYPE_SETUP,
                pEventData->msg);

            SSOEvent_FreeEventData((SSEventData*)pEventData);
            
            // update Call Database and switch states for the processed
            // CC instance
            pTInfo = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            pTInfo->callStatus = SS_CALL_STATUS_SETUP;
            pTInfo->callState.mainState = SSCallState_CALL_INIT;
            MOVE_TO_STATE( SSOperationsCCCallInitState_Handler );

            return TRUE;
        }
    }
    
    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsCCMTCallConfState_Handler

DESCRIPTION:
   This is the MTCallConf state event handler.
   
PARAMETERS:
   pMe [in]   : Pointer to the CC object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsCCMTCallConfState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
   )
{
    SSCall_TInfo_type* pTInfo;

    switch (eCode)
    {
    case MNCC_ALERT_REQ:
        {
            SSEventCallwiseMsgData *pEventData = (SSEventCallwiseMsgData*)dwParam;

            // If the primitive holds a message, it must be ALERTING message
            assert ((pEventData->msg == NULL) || 
                    (pEventData->msg->gen_hdr.msg_type == SSMSG_MSG_TYPE_ALERTING));

            // send ALERTING
            SSOperationsCC_DataRequest(
                pMe, 
                pMe->m_currentlyProcessedTInfoIndex, 
                SSMSG_MSG_TYPE_ALERTING,
                pEventData->msg);

            SSOEvent_FreeEventData((SSEventData*)pEventData);
            
            // update Call Database and switch states for the processed
            // CC instance
            pTInfo = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            pTInfo->callState.mainState = SSCallState_CALL_RECEIVED;
            MOVE_TO_STATE( SSOperationsCCCallRecievedState_Handler );

            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsCCCallRecievedState_Handler

DESCRIPTION:
   This is the CallRecieved state event handler.
   
PARAMETERS:
   pMe [in]   : Pointer to the CC object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsCCCallRecievedState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
   )
{
    SSCall_TInfo_type* pTInfo;

    switch (eCode)
    {
    case MNCC_SETUP_RSP:
        {
            SSEventCallwiseMsgData *pMsgEventData = (SSEventCallwiseMsgData*)dwParam;

            // This primitive should not hold a message
            assert (pMsgEventData->msg == NULL);
            
            // send CONNECT
            SSOperationsCC_DataRequest(
                pMe, 
                pMe->m_currentlyProcessedTInfoIndex, 
                SSMSG_MSG_TYPE_CONNECT,
                NULL);
            
            SSOEvent_FreeEventData((SSEventData*)pMsgEventData);

            // update Call Database and switch states for the processed
            // CC instance
            pTInfo = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            pTInfo->callStatus = SS_CALL_STATUS_CONNECTED;
            pTInfo->callState.mainState = SSCallState_CONNECT_REQUEST;

            MOVE_TO_STATE( SSOperationsCCConnectRequestState_Handler );

            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsCCConnectRequestState_Handler

DESCRIPTION:
   This is the ConnectRequest state event handler.
   
PARAMETERS:
   pMe [in]   : Pointer to the CC object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsCCConnectRequestState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
   )
{
    SSCall_TInfo_type* pTInfo;

    switch (eCode)
    {
    case CC_ONENTER:
        // handle timers
        SSOperationsCC_SetTimer(pMe, pMe->m_currentlyProcessedTInfoIndex, 
            SSO_TIMER_T313, SSO_TIMER_T313_DURATION);
        return TRUE;

    case CC_ONEXIT:
        // stop timer
        SSOperationsCC_CancelTimer(pMe, pMe->m_currentlyProcessedTInfoIndex);
        return TRUE;

    case MNCC_DATA_INDICATION:
        {
            SSCall_Index_type callIndex;
            SSEventDataIndData  *pDataIndEventData = (SSEventDataIndData*)dwParam;
            
            // check for CONNECT ACK message
            if (pDataIndEventData->msg->gen_hdr.msg_type != SSMSG_MSG_TYPE_CONNECT_ACKNOWLEDGE)
                return FALSE;

            // send a setup compl ind to the MN
            SSCallDatabase_GetCallIndexByTInfoIndex(
                pMe->m_pCallDatabase, 
                pMe->m_currentlyProcessedTInfoIndex,
                &callIndex);
            if (SSOperationsCC_SendCallwiseMsgEvent(
                    pMe, 
                    MNCC_SETUP_COMPLETE_IND,
                    callIndex,
                    0,
                    NULL) == FALSE)
            {
                return FALSE;
            }
            
            // update Call Database and switch states for the processed
            // CC instance
            pTInfo = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            pTInfo->callStatus = SS_CALL_STATUS_ACTIVE;
            pTInfo->callState.mainState = SSCallState_ACTIVE;
            
            MOVE_TO_STATE( SSOperationsCCActiveState_Handler );
            
            SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);
            return TRUE;
        }
    case CC_TIMEOUT:
        {
            // dwParam holds the timer id
            if ((SSO_TimerIdType)dwParam != SSO_TIMER_T313)
                return FALSE;

            // drop traffic channel if there is no other active transaction
            if (!SSOperationsCC_IsAnotherActiveTransactionExists(pMe, pMe->m_currentlyProcessedTInfoIndex))
            {
                SSOperationsCC_DropTch(pMe);
                return TRUE;
            }
            
            // send DISCONNECT
            // maybe use a different cause in the DISCONNECT message
            SSOperationsCC_DataRequest(
                pMe, 
                pMe->m_currentlyProcessedTInfoIndex,
                SSMSG_MSG_TYPE_DISCONNECT,
                NULL);
            
            // update Call Database and switch states for the processed
            // CC instance
            pTInfo = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            pTInfo->callStatus = SS_CALL_STATUS_DISCONNECTED;
            pTInfo->callState.mainState = SSCallState_DISCONNECT_REQUEST;
            MOVE_TO_STATE( SSOperationsCCDisconnectRequestState_Handler );
            
            return TRUE;
        }
    }
    
    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsCCActiveState_Handler

DESCRIPTION:
   This is the Active state event handler.
   The Hold and MPTY sub SM are running inside the Active state of the CC.
   Events that are received in the Active state may be handled by this state
   or forwarded to the Hold and MPTY sub SM.
   
PARAMETERS:
   pMe [in]   : Pointer to the CC object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsCCActiveState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
   )
{
    SSCall_TInfo_type* pTInfo;

    switch(eCode)
    {
    case CC_HOLD_TIMEOUT_IND:
        {
            // drop traffic channel if there is no other active transaction
            if (!SSOperationsCC_IsAnotherActiveTransactionExists(pMe, pMe->m_currentlyProcessedTInfoIndex))
            {
                SSOperationsCC_DropTch(pMe);
                return TRUE;
            }
            
            // send DISCONNECT
            // maybe use a different cause in the DISCONNECT message
            SSOperationsCC_DataRequest(
                pMe, 
                pMe->m_currentlyProcessedTInfoIndex,
                SSMSG_MSG_TYPE_DISCONNECT,
                NULL);
            
            // update Call Database and switch states for the processed
            // CC instance
            pTInfo = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            pTInfo->callStatus = SS_CALL_STATUS_DISCONNECTED;
            pTInfo->callState.mainState = SSCallState_DISCONNECT_REQUEST;
            MOVE_TO_STATE( SSOperationsCCDisconnectRequestState_Handler );
            
            return TRUE;
        }
    }
    
    // forward the event to the Hold and MPTY state machines.
    if (SSOperationsHold_HandleEvent(pMe->m_pHold, pMe->m_currentlyProcessedTInfoIndex, eCode, wParam, dwParam))
        return TRUE;
    else
        return SSOperationsMPTY_HandleEvent(pMe->m_pMPTY, pMe->m_currentlyProcessedTInfoIndex, eCode, wParam, dwParam);
}

/*=============================================================================
FUNCTION: SSOperationsCCDisconnectRequestState_Handler

DESCRIPTION:
   This is the DisconnectRequest state event handler. It is a part of the 
   Call Clearing Phase.
   
PARAMETERS:
   pMe [in]   : Pointer to the CC object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsCCDisconnectRequestState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
   )
{
    SSCall_TInfo_type* pTInfo;
    SSMsg_int_msg_type* msg;

    switch (eCode)
    {
    case CC_ONENTER:
        // handle timers
        SSOperationsCC_SetTimer(pMe, pMe->m_currentlyProcessedTInfoIndex, 
            SSO_TIMER_T305, SSO_TIMER_T305_DURATION);
        return TRUE;
        
    case CC_ONEXIT:
        // stop timer
        SSOperationsCC_CancelTimer(pMe, pMe->m_currentlyProcessedTInfoIndex);
        return TRUE;

    case CC_TIMEOUT:
        // dwParam holds the timer id
        if ((SSO_TimerIdType)dwParam != SSO_TIMER_T305)
            return FALSE;

        // construct a RELEASE message
        msg = SSOMessage_AllocateMsg(SSMSG_MSG_TYPE_RELEASE);
        assert(msg != NULL);
        msg->release.cause.hdr.present = TRUE;
        msg->release.cause.coding_standard = SSMSG_CODING_STANDARD_GSM;
        msg->release.cause.location = SSMSG_LOCATION_USER;
        msg->release.cause.diagnostic_len = 0;
        // get the cause originally contained in the DISCONNECT message
        msg->release.cause.cause_value = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_disconnectCauseValue;
        msg->release.second_cause.hdr.present = TRUE;
        msg->release.second_cause.coding_standard = SSMSG_CODING_STANDARD_GSM;
        msg->release.second_cause.location = SSMSG_LOCATION_USER;
        msg->release.second_cause.diagnostic_len = 0;
        msg->release.second_cause.cause_value = SSMSG_CAUSE_VALUE_RECOVERY_ON_TIMER_EXPIRY;
        
        // send the message
        SSOperationsCC_DataRequest(
            pMe, 
            pMe->m_currentlyProcessedTInfoIndex,
            SSMSG_MSG_TYPE_RELEASE,
            msg);

        SSOMessage_FreeMsg(msg);
        
        // update Call Database and switch states for the processed
        // CC instance
        pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_T308Count = 0;
        pTInfo = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
        pTInfo->callState.mainState = SSCallState_RELEASE_REQUEST;
        MOVE_TO_STATE( SSOperationsCCReleaseRequestState_Handler );

        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsCCReleaseRequestState_Handler

DESCRIPTION:
   This is the ReleaseRequest state event handler. It is a part of the 
   Call Clearing Phase.
   
PARAMETERS:
   pMe [in]   : Pointer to the CC object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsCCReleaseRequestState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
   )
{
    SSCall_TInfo_type* pTInfo;
    SSEventDataIndData* pDataIndEventData;

    switch (eCode)
    {
    case CC_ONENTER:
        // handle timers
        SSOperationsCC_SetTimer(pMe, pMe->m_currentlyProcessedTInfoIndex, 
            SSO_TIMER_T308, SSO_TIMER_T308_DURATION);
        return TRUE;

    case CC_ONEXIT:
        // stop timer
        SSOperationsCC_CancelTimer(pMe, pMe->m_currentlyProcessedTInfoIndex);
        return TRUE;

    case MNCC_DATA_INDICATION:
        pDataIndEventData = (SSEventDataIndData*)dwParam;
            
        // check for RELEASE message
        if (pDataIndEventData->msg->gen_hdr.msg_type != SSMSG_MSG_TYPE_RELEASE)
            return FALSE;
        // ignore clearing message if only one call exist.
        if (SSCallDatabase_GetCallCount(pMe->m_pCallDatabase) < 2)
            return FALSE;
        
        // no need to check if other call is disconnecting because we are
        // already deep inside the Call Clearing procedure.

        // send a rel ind to the MN
        if (SSOperationsCC_SendTransactionwiseMsgEvent(
            pMe, 
            MNCC_REL_IND, 
            pMe->m_currentlyProcessedTInfoIndex,
            pDataIndEventData->msg) == FALSE)
        {
            return FALSE;
        }
        
        // update Call Database and switch states for the processed
        // CC instance
        pTInfo = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
        pTInfo->callState.mainState = SSCallState_NULL;
        MOVE_TO_STATE( SSOperationsCCNullState_Handler );
        SSOperationsCC_ReleaseTransaction(pMe, pMe->m_currentlyProcessedTInfoIndex);
        
        SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);
        return TRUE;

    case CC_TIMEOUT:
        {
            SSMsg_int_msg_type* msg;
            
            // dwParam holds the timer id
            if ((SSO_TimerIdType)dwParam != SSO_TIMER_T308)
                return FALSE;
            
            if (pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_T308Count == 0)
            {
                // first expiry of T308
                pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_T308Count++;
                // construct a RELEASE message
                msg = SSOMessage_AllocateMsg(SSMSG_MSG_TYPE_RELEASE);
                assert(msg != NULL);
                msg->release.cause.hdr.present = TRUE;
                msg->release.cause.coding_standard = SSMSG_CODING_STANDARD_GSM;
                msg->release.cause.location = SSMSG_LOCATION_USER;
                msg->release.cause.diagnostic_len = 0;
                // get the cause originally contained in the DISCONNECT message
                msg->release.cause.cause_value = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_disconnectCauseValue;
                msg->release.second_cause.hdr.present = TRUE;
                msg->release.second_cause.coding_standard = SSMSG_CODING_STANDARD_GSM;
                msg->release.second_cause.location = SSMSG_LOCATION_USER;
                msg->release.second_cause.diagnostic_len = 0;
                msg->release.second_cause.cause_value = SSMSG_CAUSE_VALUE_RECOVERY_ON_TIMER_EXPIRY;
                
                // send the message
                SSOperationsCC_DataRequest(
                    pMe, 
                    pMe->m_currentlyProcessedTInfoIndex,
                    SSMSG_MSG_TYPE_RELEASE,
                    msg);
                
                SSOMessage_FreeMsg(msg);

                // start the timer again
                SSOperationsCC_SetTimer(pMe, pMe->m_currentlyProcessedTInfoIndex, 
                    SSO_TIMER_T308, SSO_TIMER_T308_DURATION);
            }
            else
            {
                // second expiry of T308
                // send a rel cnf to the MN
                if (SSOperationsCC_SendTransactionwiseMsgEvent(
                        pMe, 
                        MNCC_REL_CNF,
                        pMe->m_currentlyProcessedTInfoIndex,
                        NULL) == FALSE)
                {
                    return FALSE;
                }

                // update Call Database and switch states for the processed
                // CC instance
                pTInfo = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
                pTInfo->callState.mainState = SSCallState_NULL;
                MOVE_TO_STATE( SSOperationsCCNullState_Handler );
                SSOperationsCC_ReleaseTransaction(pMe, pMe->m_currentlyProcessedTInfoIndex);
            }
            
            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsCCCallInitState_Handler

DESCRIPTION:
   This is the CallInit state event handler.
   
PARAMETERS:
   pMe [in]   : Pointer to the CC object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsCCCallInitState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
   )
{
    SSCall_TInfo_type* pTInfo;

    switch (eCode)
    {
    case CC_ONENTER:
        // handle timers
        SSOperationsCC_SetTimer(pMe, pMe->m_currentlyProcessedTInfoIndex, 
            SSO_TIMER_T303, SSO_TIMER_T303_DURATION);
        return TRUE;

    case CC_ONEXIT:
        // stop timer
        SSOperationsCC_CancelTimer(pMe, pMe->m_currentlyProcessedTInfoIndex);
        return TRUE;

    case MNCC_DATA_INDICATION:
        {
            SSEventDataIndData* pDataIndEventData = (SSEventDataIndData*)dwParam;
            
            // check for CALL PROCEEDING message
            if (pDataIndEventData->msg->gen_hdr.msg_type != SSMSG_MSG_TYPE_CALL_PROCEEDING)
                return FALSE;
            
            // update Call Database and switch states for the processed
            // CC instance
            pTInfo = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            pTInfo->callState.mainState = SSCallState_MO_CALL_PROCEEDING;
            MOVE_TO_STATE( SSOperationsCCMOCallProceedingState_Handler );
            
            SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);
            return TRUE;
        }

    case CC_TIMEOUT:
        {
            // dwParam holds the timer id
            if ((SSO_TimerIdType)dwParam != SSO_TIMER_T303)
                return FALSE;

            // drop traffic channel if there is no other active transaction
            if (!SSOperationsCC_IsAnotherActiveTransactionExists(pMe, pMe->m_currentlyProcessedTInfoIndex))
            {
                SSOperationsCC_DropTch(pMe);
                return TRUE;
            }
            
            // send DISCONNECT
            // maybe use a different cause in the DISCONNECT message
            SSOperationsCC_DataRequest(
                pMe, 
                pMe->m_currentlyProcessedTInfoIndex,
                SSMSG_MSG_TYPE_DISCONNECT,
                NULL);
            
            // update Call Database and switch states for the processed
            // CC instance
            pTInfo = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            pTInfo->callStatus = SS_CALL_STATUS_DISCONNECTED;
            pTInfo->callState.mainState = SSCallState_DISCONNECT_REQUEST;
            MOVE_TO_STATE( SSOperationsCCDisconnectRequestState_Handler );
            
            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsCCMOCallProceedingState_Handler

DESCRIPTION:
   This is the MOCallProceeding state event handler.
   
PARAMETERS:
   pMe [in]   : Pointer to the CC object.           
   eCode [in] : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled by this state, FALSE otherwise.
=============================================================================*/
static boolean SSOperationsCCMOCallProceedingState_Handler(
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
   )
{
    SSCall_TInfo_type* pTInfo;

    switch (eCode)
    {
    case MNCC_DATA_INDICATION:
        {
            SSCall_Index_type callIndex;
            SSEventDataIndData* pDataIndEventData = (SSEventDataIndData*)dwParam;
            
            // check for CONNECT message
            if (pDataIndEventData->msg->gen_hdr.msg_type != SSMSG_MSG_TYPE_CONNECT)
                return FALSE;
            
            // send a setup cnf to the MN
            SSCallDatabase_GetCallIndexByTInfoIndex(
                pMe->m_pCallDatabase, 
                pMe->m_currentlyProcessedTInfoIndex,
                &callIndex);
            if (SSOperationsCC_SendCallwiseMsgEvent(
                    pMe, 
                    MNCC_SETUP_CNF, 
                    callIndex,
                    0,
                    NULL) == FALSE)
            {
                return FALSE;
            }
                   
            // send CONNECT ACK
            SSOperationsCC_DataRequest(
                pMe, 
                pMe->m_currentlyProcessedTInfoIndex, 
                SSMSG_MSG_TYPE_CONNECT_ACKNOWLEDGE,
                NULL);
            
            // update Call Database and switch states for the processed
            // CC instance
            pTInfo = pMe->m_ccTransactions[pMe->m_currentlyProcessedTInfoIndex].m_pCurrentTInfo;
            pTInfo->callStatus = SS_CALL_STATUS_ACTIVE;
            pTInfo->callState.mainState = SSCallState_ACTIVE;
            MOVE_TO_STATE( SSOperationsCCActiveState_Handler );
            
            SSOEvent_FreeEventData((SSEventData*)pDataIndEventData);
            return TRUE;
        }
    }

    return FALSE;
}
