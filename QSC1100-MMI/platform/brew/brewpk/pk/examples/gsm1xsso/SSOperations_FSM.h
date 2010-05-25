#ifndef __SS_OPERATIONS_FSM_H__
#define __SS_OPERATIONS_FSM_H__

/*=============================================================================
FILE: SSOperations_FSM.h

SERVICES: Provides the GSM1x SS Operations Finite State Machine entry point.
          Interact with the MN and the CC layers of the state machine

GENERAL DESCRIPTION:
    SSO state machine is composed of the following sub SM:
    - MN which handles procedures that involves more than one call (e.g.
      accept waiting call).
    - CC Manager and CC instances. Every CC instance handles a single call.
    - Toggle SM which handles the procedure of toggling between two calls. 
      The Toggle SM is part of the MN layer.
    - Hold SM which handles the HOLD auxiliary states for a single call. The 
      Hold SM is part of every CC instance.
    - Multi Party SM which handles the MPTY auxiliary states for a single call.
      The MPTY SM is part of every CC instance.

    The FSM is the entry point for external events to the SSO state machine.
    External events include standard BREW events along with the SSOUI-FSM
    interface primitives.
    The events are processed by a HandleEvent function which, if needed, 
    routes them either to the MN or to the CC Manager. They are further 
    processed by HandleEvent functions of these sub SM.

    Internal events (on the MN-CC interface or MN/CC internal events) are sent
    using an event queue maintained by the FSM.
    After handling each external event, the FSM iterates through this queue and
    handles all events in it until the queue is empty.
    Every event handling may result in additional events inserted to the event
    queue.

PUBLIC CLASSES AND STATIC FUNCTIONS:
    CSSOperationsFSM

INITIALIZATION AND SEQUENCING REQUIREMENTS:
    After creating an CSSOperationsFSM object, call SSOperationsFSM_Initialize
    to initialize it.

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "SSCallCommonDefinitions.h"
#include "SSCallDatabase.h"
#include "SSUtil.h"
#include "SSOperations_MN.h"
#include "SSOperations_CC.h"
#include "SSqueue.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

#define GET_OTHER_CALL_INDEX(i) ((uint8)((i) ? 0 : 1))

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

struct _CSSOperationsFSM;
struct IGSM1xSig;

// the SSO FSM structure
typedef struct _CSSOperationsFSM
{
    IShell           *m_pIShell;       // Shell interface

    SSCallDatabase   *m_pCallDatabase; // Call database
    ISSUtilCls       *m_pSSUtilCls;
    CSSOperationsMN  *m_pMN;
    CSSOperationsCC  *m_pCC;
    SSQueue          *m_pEventQueue;

    boolean           m_AppStarted;    // flag indicating if EVT_APP_START was
                                       // already received

    // placeholder for GSM1x signaling message notification while the applet
    // is not started.
    AEENotify        *m_pNotify;

    struct IGSM1xSig *m_pIGSM1xSig;    // IGSM1xSig interface

} CSSOperationsFSM;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: SSOperationsFSM_New

DESCRIPTION: Create an FSM object

PARAMETERS:
    pIShell [in]       : Pointer to the IShell interface
    pCallDatabase [in] : Pointer to a valid CallDatabase instance

RETURN VALUE:
   Pointer to the created SSOperationsFSM or NULL on failure.

COMMENTS:
    in order to free SSOperationsFSM object, use SSOperationsFSM_Release.
    This function should be called only once in a life time of a client.
=============================================================================*/
CSSOperationsFSM* SSOperationsFSM_New(
    IShell         *pIShell,
    SSCallDatabase *pCallDatabase
    );

/*=============================================================================
FUNCTION: SSOperationsFSM_Initialize

DESCRIPTION: Initialize an FSM object.

PARAMETERS:
     pMe [in]: Pointer to the FSM object to be initialized.

RETURN VALUE:
    TRUE on success or FALSE on failure.
=============================================================================*/
boolean SSOperationsFSM_Initialize( CSSOperationsFSM *pMe );

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
void SSOperationsFSM_Release( CSSOperationsFSM *pMe );

/*=============================================================================
FUNCTION: SSOperationsFSM_HandleEvent

DESCRIPTION: Entry point for external events to the FSM.

PARAMETERS:
   pMe [in]   : Pointer to the FSM object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.
=============================================================================*/
boolean SSOperationsFSM_HandleEvent(
   CSSOperationsFSM *pMe,
   AEEEvent          eCode,
   uint16            wParam,
   uint32            dwParam
);

/*=============================================================================
FUNCTION: SSOperationsFSM_QueueEvent

DESCRIPTION: Queues an event onto the MN-CC queue. may be called from the
             MN or from the CC.

PARAMETERS:
   pMe   [in] : Pointer to the FSM object.
   eCode [in] : Specifies the Event to queue.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None
=============================================================================*/
void SSOperationsFSM_QueueEvent(
   CSSOperationsFSM* pMe,
   AEEEvent          eCode,
   uint16            wParam,
   uint32            dwParam
);

/*=============================================================================
FUNCTION: SSOperationsFSM_SendSSMessage

DESCRIPTION:
    Send a GSM1x signaling message to the network using IGSM1xSig.
    Before sending it, the message is translated into external (packed) format.
   
PARAMETERS:
    pMe [in] : Pointer to the FSM object.
    msg [in] : GSM1x SS signaling message to send.

RETURN VALUE:
    None.
=============================================================================*/
void SSOperationsFSM_SendSSMessage(
    CSSOperationsFSM    *pMe,
    SSMsg_int_msg_type  *msg
);

/*=============================================================================
FUNCTION: SSOperationsFSM_SendReleaseComplete

DESCRIPTION:
    Send a RELEASE COMPLETE message to the network.
    Actual sending is done using SSOperationsFSM_SendSSMessage.
   
PARAMETERS:
    pMe [in]    : Pointer to the FSM object.
    TI  [in]    : Transaction identifier to use for the outgoing message.
    cause_value : Cause value to use for the outgoing message.

RETURN VALUE:
    None.

SEE ALSO:
    SSOperationsFSM_SendSSMessage
=============================================================================*/
void SSOperationsFSM_SendReleaseComplete(
    CSSOperationsFSM   *pMe,
    uint8 TI,
    SSMsg_cause_value_type cause_value
);

/*=============================================================================
FUNCTION: SSOperationsFSM_CheckIncomingTI

DESCRIPTION: 
    Check the validity of an incoming GSM1x signaling message. The validity
    check is based on the TI and the message type of the incoming message.
    A response, according to the relevant protocol specifications is sent to
    the network if needed.

PARAMETERS:
    pMe [in]      : Pointer to the FSM object.
    TI [in]       : the incoming TI to check.
    msg_type [in] : message type of the incoming message.

RETURN VALUE:
    TRUE  - the message was found valid, further processing is needed.
    FALSE - the message was found invalid and was handled by this function. no 
            further processing is needed.

COMMENTS:
    This function can be called either before or after the app was started.
=============================================================================*/
boolean SSOperationsFSM_CheckIncomingTI(
    CSSOperationsFSM    *pMe,
    uint8                TI,
    SSMsg_msg_type_type  msg_type
);

#endif // end __SS_OPERATIONS_FSM_H__

