#ifndef __SS_OPERATIONS_MN_H__
#define __SS_OPERATIONS_MN_H__

/*=============================================================================
FILE: SSOperations_MN.h

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
#include "SSOperations_CommonEventDefinitions.h"
#include "SSCallDatabase.h"
#include "SSUtil.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

struct _CSSOperationsMN;
struct _CSSOperationsFSM;
struct _CSSOperationsToggle;

// state event handler function pointer type
typedef boolean (*MN_STATE_EVENT_HANDLER)(
        struct _CSSOperationsMN       *pMe,
        AEEEvent                      eCode,
        uint16                        wParam,
        uint32                        dwParam
    );

typedef struct _CSSOperationsMN
{
    IShell                      *m_pIShell;       // Shell interface
 
    MN_STATE_EVENT_HANDLER       m_pCurrentStateHandler;  // Current state event handler
    ISSUtilCls                  *m_pSSUtilCls;
    struct _CSSOperationsFSM    *m_pFSM;
    struct _CSSOperationsToggle *m_pToggle;
    SSCallDatabase              *m_pCallDatabase;

    IPhone                      *m_pIPhone;      // Phone interface

    // data members for specific operations
    // Waiting call in the "Answer and Hold/Release" operations
    SSCall_Index_type            m_ciCallToAnswer;
    // Active call in the "Answer and Hold" and "Toggle" operations
    SSCall_Index_type            m_ciCallToHold;
    // Held call in the "Toggle" operation
    SSCall_Index_type            m_ciCallToRetrieve;
    // Active call in the "Answer and Release" operation
    SSCall_Index_type            m_ciCallToRelease;
    // CC instance that handles Build MPTY request
    SSCall_Index_type            m_ciBuildMPTYHandler;
    // placeholder for error cause when a ERR_IND is to be sent.
    SSEventErrorCauseType        m_errorCause;
    // placeholders for the results of the Toggle state machine
    SSEventToggleResultType      m_firstToggleResult, m_secondToggleResult;
} CSSOperationsMN;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

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
    IShell *pIShell
    );

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
    CSSOperationsMN          *pMe,
    struct _CSSOperationsFSM *parent,
    SSCallDatabase           *pCallDatabase
    );

/*=============================================================================
FUNCTION: SSOperationsMN_Release

DESCRIPTION: Destroy an MN object.

PARAMETERS:
    pMe [in]: Pointer to the MN object to be destroyed.

RETURN VALUE:
    None

COMMENTS:
    The MN does not maintain a reference counter. Calling Release always
    destroy the given object.
=============================================================================*/
void SSOperationsMN_Release( CSSOperationsMN *pMe );

/*=============================================================================
FUNCTION: SSOperationsMN_HandleEvent

DESCRIPTION: Entry point for events to the MN.

PARAMETERS:
   pMe [in]   : Pointer to the MN object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.
=============================================================================*/
boolean SSOperationsMN_HandleEvent(
   CSSOperationsMN *pMe,
   AEEEvent         eCode,
   uint16           wParam,
   uint32           dwParam
);

/*=============================================================================
FUNCTION: SSOperationsMN_QueueEvent

DESCRIPTION: queues an event onto the MN-CC queue. may be called from the
             Toggle SM.

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
);

#endif // end __SS_OPERATIONS_MN_H__

