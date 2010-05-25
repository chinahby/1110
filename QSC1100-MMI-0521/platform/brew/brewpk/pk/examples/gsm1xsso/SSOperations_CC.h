#ifndef __SS_OPERATIONS_CC_H__
#define __SS_OPERATIONS_CC_H__

/*=============================================================================
FILE: SSOperations_CC.h

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

#include "GSM1xPhone.h"

#include "SSCallCommonDefinitions.h"
#include "SSCallDatabase.h"
#include "SSUtil.h"
#include "SSMsg.h"                          // SS layer 3 messages

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// max number of CC instances. max is defined by one MPTY call with 5 remote
// parties and additional regular call.
#define SSO_CC_MAX_TRANSACTIONS_NUMBER 6

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

struct _CSSOperationsCC;
struct _CSSOperationsFSM;
struct _CSSOperationsHold;
struct _CSSOperationsMPTY;

// state event handler function pointer type
typedef boolean (*CC_STATE_EVENT_HANDLER)(
        struct _CSSOperationsCC *pMe,
        AEEEvent                 eCode,
        uint16                   wParam,
        uint32                   dwParam
    );

// timer data for one CC instance
typedef struct
{
    struct _CSSOperationsCC* pMe;
    SSCall_TInfo_Index_type tInfoIndex;
    SSO_TimerIdType timerId;
} SSOperationsCC_TimerDataType;

// CC transaction information. a CC instance is actually defined by this 
// structure
typedef struct _CCTInfo
{  
    SSCall_TInfo_type           *m_pCurrentTInfo;
    // Current state event handler
    CC_STATE_EVENT_HANDLER       m_pCurrentStateHandler;
    // Timer callback
    AEECallback                  m_TimerCB;
    SSOperationsCC_TimerDataType m_timerData;
    // cause sent in the DISCONNECT msg
    SSMsg_cause_value_type       m_disconnectCauseValue;
    // counter for T308 expirations
    uint8                        m_T308Count;
} CCTInfo;

// CC Manager structure
typedef struct _CSSOperationsCC
{
    IShell                      *m_pIShell;       // Shell interface
    struct _CSSOperationsFSM    *m_pFSM;          // FSM object  
    SSCallDatabase              *m_pCallDatabase; // Call database
    ISSUtilCls                  *m_pSSUtilCls;    // SS utilities class
    struct _CSSOperationsHold   *m_pHold;         // Hold state machine
    struct _CSSOperationsMPTY   *m_pMPTY;         // Multi Party state machine

    IPhone* m_pIPhone;      // Phone interface

     // array of transactions (CC instances)
    CCTInfo                 m_ccTransactions[SSO_CC_MAX_TRANSACTIONS_NUMBER];
    // while processing an event, this member identify the processing CC
    // instance
    SSCall_TInfo_Index_type m_currentlyProcessedTInfoIndex;

} CSSOperationsCC;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: SSOperationsCC_New

DESCRIPTION: Create an CC object.

PARAMETERS:
     pIShell [in]      : Pointer to the IShell instance
     parent [in]       : Pointer to the FSM object.
     pCallDatabase [in]: Pointer to a valid CallDatabase instance

RETURN VALUE:
    Pointer to the created SSOperationsCC or NULL on failure.

COMMENTS:
    in order to free SSOperationsCC object, use SSOperationsCC_Release.
    This function should be called only once in a life time of a client.
=============================================================================*/
CSSOperationsCC* SSOperationsCC_New(
    IShell                   *pIShell,
    struct _CSSOperationsFSM *parent,
    SSCallDatabase           *pCallDatabase
    );

/*=============================================================================
FUNCTION: SSOperationsCC_HandleEvent

DESCRIPTION: Entry point for events to the CC.

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
);

/*=============================================================================
FUNCTION: SSOperationsCC_Initialize

DESCRIPTION: Initialize an CC object

PARAMETERS:
     pMe [in] : Pointer to the CC object to be initialized.

RETURN VALUE:
    TRUE on success or FALSE on failure.
=============================================================================*/
boolean SSOperationsCC_Initialize( CSSOperationsCC *pMe );

/*=============================================================================
FUNCTION: SSOperationsCC_Release

DESCRIPTION: Destroy an CC object.

PARAMETERS:
     pMe [in]: Pointer to the CC object to be destroyed.

RETURN VALUE:
     None
=============================================================================*/
void SSOperationsCC_Release( CSSOperationsCC *pMe );

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
   CSSOperationsCC *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

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
    );

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
    CSSOperationsCC    *pMe,
    AEEEvent            eCode,
    SSCall_TInfo_Index_type   tInfoIndex,
    SSMsg_int_msg_type *msg
    );

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
    CSSOperationsCC     *pMe,
    SSCall_TInfo_Index_type tInfoIndex,
    SSMsg_msg_type_type  msgType,
    SSMsg_int_msg_type  *msg
    );

#endif // end __SS_OPERATIONS_CC_H__

