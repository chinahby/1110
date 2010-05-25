#ifndef __SS_OPERATIONS_HOLD_H__
#define __SS_OPERATIONS_HOLD_H__

/*=============================================================================
FILE: SSOperations_Hold.h

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

#include "SSOperations_CC.h"
#include "SSCallCommonDefinitions.h"
#include "SSCallDatabase.h"
#include "SSUtil.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

struct _CSSOperationsHold;

// state event handler function pointer type
typedef boolean (*HOLD_STATE_EVENT_HANDLER)(
        struct _CSSOperationsHold *pMe,
        AEEEvent                   eCode,
        uint16                     wParam,
        uint32                     dwParam
    );

// timer data for one Hold instance
typedef struct
{
    struct _CSSOperationsHold *pMe;
    SSCall_TInfo_Index_type tInfoIndex;
    SSO_TimerIdType timerId;
} SSOperationsHold_TimerDataType;

// Hold transaction information. a Hold instance is actually defined by this 
// structure
typedef struct _HoldTInfo
{  
    SSCall_TInfo_type         *m_pCurrentTInfo;
    // Current state event handler
    HOLD_STATE_EVENT_HANDLER   m_pCurrentStateHandler;
    // Timer callback
    AEECallback                m_TimerCB;
    SSOperationsHold_TimerDataType m_timerData;
} HoldTInfo;

// the Hold object
typedef struct _CSSOperationsHold
{
    IShell                    *m_pIShell;       // Shell interface
    SSCallDatabase            *m_pCallDatabase; // Call database
    ISSUtilCls                *m_pSSUtilCls;    // SS utilities class
    struct _CSSOperationsCC   *m_pCC;           // parent CC Manager

     // array of transactions (Hold instances)
    HoldTInfo                  m_Transactions[SSO_CC_MAX_TRANSACTIONS_NUMBER];
    // while processing an event, this member identify the processing Hold
    // instance
    SSCall_TInfo_Index_type    m_currentlyProcessedTInfoIndex;

} CSSOperationsHold;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: SSOperationsHold_New

DESCRIPTION: Create an Hold object.

PARAMETERS:
     pIShell [in]      : Pointer to the IShell instance
     pCC [in]          : Pointer to the CC Manager object.
     pCallDatabase [in]: Pointer to a valid CallDatabase instance

RETURN VALUE:
    Pointer to the created SSOperationsHold or NULL on failure.

COMMENTS:
    in order to free SSOperationsHold object, use SSOperationsHold_Release.
    This function should be called only once in a life time of a client.
=============================================================================*/
CSSOperationsHold* SSOperationsHold_New(
    IShell                  *pIShell,
    struct _CSSOperationsCC *pCC,
    SSCallDatabase          *pCallDatabase
    );

/*=============================================================================
FUNCTION: SSOperationsHold_Initialize

DESCRIPTION: Initialize an Hold object

PARAMETERS:
     pMe [in] : Pointer to the Hold object to be initialized.

RETURN VALUE:
    TRUE on success or FALSE on failure.
=============================================================================*/
boolean SSOperationsHold_Initialize( CSSOperationsHold *pMe );

/*=============================================================================
FUNCTION: SSOperationsHold_Release

DESCRIPTION: Destroy an Hold object.

PARAMETERS:
    pMe [in]: Pointer to the Hold object to be destroyed.

RETURN VALUE:
    None
=============================================================================*/
void SSOperationsHold_Release( CSSOperationsHold *pMe );

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
);

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
    CSSOperationsHold *pMe,
    SSCall_TInfo_Index_type tInfoIndex
    );

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
    );

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
    );

#endif // end __SS_OPERATIONS_HOLD_H__

