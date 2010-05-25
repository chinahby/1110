#ifndef __SS_OPERATIONS_MPTY_H__
#define __SS_OPERATIONS_MPTY_H__

/*=============================================================================
FILE: SSOperations_MPTY.h

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

struct _CSSOperationsMPTY;

// state event handler function pointer type
typedef boolean (*MPTY_STATE_EVENT_HANDLER)(
        struct _CSSOperationsMPTY *pMe,
        AEEEvent                   eCode,
        uint16                     wParam,
        uint32                     dwParam
    );

// timer data for each MPTY instance
typedef struct
{
    struct _CSSOperationsMPTY *pMe;
    SSCall_TInfo_Index_type    tInfoIndex;
    SSO_TimerIdType            timerId;
} SSOperationsMPTY_TimerDataType;

// MPTY transaction information. a MPTY instance is actually defined by this 
// structure
typedef struct _MPTYTInfo
{  
    SSCall_TInfo_type   *m_pCurrentTInfo;
    // Current state event handler
    MPTY_STATE_EVENT_HANDLER   m_pCurrentStateHandler;
    // Timer callback
    AEECallback                m_TimerCB;
    SSOperationsMPTY_TimerDataType m_timerData;
} MPTYTInfo;

// the MPTY object
typedef struct _CSSOperationsMPTY
{
    IShell                    *m_pIShell;       // Shell interface
    SSCallDatabase            *m_pCallDatabase; // Call database
    ISSUtilCls                *m_pSSUtilCls;    // SS utilities class
    struct _CSSOperationsCC   *m_pCC;           // parent CC Manager

     // array of transactions (MPTY instances)
    MPTYTInfo                  m_Transactions[SSO_CC_MAX_TRANSACTIONS_NUMBER];
    // while processing an event, this member identify the processing MPTY
    // instance
    SSCall_TInfo_Index_type    m_currentlyProcessedTInfoIndex;

} CSSOperationsMPTY;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

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
    IShell                  *pIShell,
    struct _CSSOperationsCC *pCC,
    SSCallDatabase          *pCallDatabase
    );

/*=============================================================================
FUNCTION: SSOperationsMPTY_Initialize

DESCRIPTION: Initialize an MPTY object

PARAMETERS:
     pMe [in] : Pointer to the MPTY object to be initialized.

RETURN VALUE:
    TRUE on success or FALSE on failure.
=============================================================================*/
boolean SSOperationsMPTY_Initialize( CSSOperationsMPTY *pMe );

/*=============================================================================
FUNCTION: SSOperationsMPTY_Release

DESCRIPTION: Destroy an MPTY object.

PARAMETERS:
    pMe [in]: Pointer to the MPTY object to be destroyed.

RETURN VALUE:
    None
=============================================================================*/
void SSOperationsMPTY_Release( CSSOperationsMPTY *pMe );

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
);

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
    CSSOperationsMPTY *pMe,
    SSCall_TInfo_Index_type  tInfoIndex
    );

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
    SSCall_TInfo_Index_type  tInfoIndex
    );

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
    );

#endif // end __SS_OPERATIONS_MPTY_H__

