#ifndef __SS_OPERATIONS_COMMON_EVENT_DEFINITIONS_H__
#define __SS_OPERATIONS_COMMON_EVENT_DEFINITIONS_H__

/*=========================================================================
FILE: SSOperations_CommonEventDefinitions.h

SERVICES: SS Operations Common Event Definitions

GENERAL DESCRIPTION:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

#include "SSMsg.h"
#include "SSCallCommonDefinitions.h"

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

// SS Event Error
// --------------
#define SS_ERROR_ANSWER_AND_HOLD 0 
#define SS_ERROR_TOGGLE 1
#define SS_ERROR_MPTY 2
// UI request was rejected. e.g. due to bad call indices in the request.
#define SS_ERROR_UI_REQ_REJECTED 3
typedef int SSEventErrorType; /* like enum */



// SS Event Error Cause
// --------------------

// --- SS_ERROR_GENERAL causes:
#define SS_ERROR_GENERAL_BASE 0x0

#define SS_ERROR_CAUSE_UNSPECIFIED                  (SS_ERROR_GENERAL_BASE + 1)


// --- SS_ERROR_ANSWER_AND_HOLD causes:
#define SS_ERROR_ANSWER_AND_HOLD_BASE 0x10

// hold reject ind received for the active call while in the
// AH_WaitForHoldCnf1 state
#define SS_ERROR_CAUSE_HOLD_REJECT                  (SS_ERROR_ANSWER_AND_HOLD_BASE + 1)

// disc ind or rel ind received for the waiting call while in the
// AH_WaitForSetupCompl1 state
#define SS_ERROR_CAUSE_WAITING_CALL_DISCONNECTED    (SS_ERROR_ANSWER_AND_HOLD_BASE + 2)

// disc ind or rel ind received for the active call while in the
// AH_WaitForHoldCnf1 state
#define SS_ERROR_CAUSE_ACTIVE_CALL_DISCONNECTED     (SS_ERROR_ANSWER_AND_HOLD_BASE + 3)


// --- SS_ERROR_TOGGLE causes:
#define SS_ERROR_TOGGLE_BASE 0x20

// one of the calls has disconnected
#define SS_ERROR_CALL_DISCONNECTED                  (SS_ERROR_TOGGLE_BASE + 1) 


// --- SS_ERROR_MPTY causes:
#define SS_ERROR_MPTY_BASE 0x30

// TbuildMPTY expired
#define SS_ERROR_BUILD_MPTY_TIMEOUT                 (SS_ERROR_MPTY_BASE + 1)

// The network responded to Build MPTY request not with return result
#define SS_ERROR_BUILD_MPTY_ERROR                   (SS_ERROR_MPTY_BASE + 2)

// The CC instance of the Build MPTY request was disconnected
#define SS_ERROR_BUILD_MPTY_DISCONNECT              (SS_ERROR_MPTY_BASE + 3) 


// --- SS_ERROR_UI_REQ_REJECTED causes:
#define SS_ERROR_UI_REQ_REJECTED_BASE 0x40

// UI SETUP_REQ rejected. CC cannot setup a new call
#define SS_ERROR_CAUSE_CANNOT_SETUP_NEW_CALL        (SS_ERROR_UI_REQ_REJECTED_BASE + 1)

// UI request rejected. callIndex1 refers to a call with unacceptable status.
#define SS_ERROR_CAUSE_CALLINDEX1_REJECTED          (SS_ERROR_UI_REQ_REJECTED_BASE + 2)

// UI request rejected. callIndex2 refers to a call with unacceptable status.
#define SS_ERROR_CAUSE_CALLINDEX2_REJECTED          (SS_ERROR_UI_REQ_REJECTED_BASE + 3)

// UI request rejected. call indices refers to calls with unacceptable status.
#define SS_ERROR_CAUSE_CALLINDICES_REJECTED         (SS_ERROR_UI_REQ_REJECTED_BASE + 4)

typedef int SSEventErrorCauseType; /* like enum */



// SS Event Toggle Result
// ----------------------
#define SS_TOGGLE_RESULT_CNF 0  /* hold/retrieve ack */
#define SS_TOGGLE_RESULT_REJ 1  /* hold/retrieve reject */
#define SS_TOGGLE_RESULT_REL 2  /* call disconnected */

typedef int SSEventToggleResultType;  /* like enum */



// SS Event Data 
// -------------
#define SS_EVENT_DATA_CALLWISE_MSG          0 /* event data with call indices */
#define SS_EVENT_DATA_TRANSACTIONWISE_MSG   1 /* event data with transaction indices */
#define SS_EVENT_DATA_DATA_IND              2 /* used by the MNCC_DATA_INDICATION primitive */
#define SS_EVENT_DATA_ERROR                 3 /* used by the ERR_IND primitives */
 
typedef int SSEventDataType;  /* like enum */


// Event Common data
typedef struct _SSEventCommonData
{
    SSEventDataType  type;
    
} SSEventCommonData;

// Call Wise Msg Event specific data
typedef struct _SSEventCallwiseMsgData
{
    SSEventCommonData   commonData; // must be first!
    SSCall_Index_type   callIndex1;
    SSCall_Index_type   callIndex2;
    SSMsg_int_msg_type *msg;

} SSEventCallwiseMsgData;

// Transaction Wise Msg Event specific data
typedef struct _SSEventTransactionwiseMsgData
{
    SSEventCommonData       commonData; // must be first!
    SSCall_Index_type       callIndex;
    SSCall_TInfo_Index_type tInfoIndex;
    SSMsg_int_msg_type      *msg;
} SSEventTransactionwiseMsgData;

// Error Event specific data
typedef struct _SSEventErrorData
{
    SSEventCommonData      commonData; // must be first!
    SSCall_Index_type      callIndex1;
    SSCall_Index_type      callIndex2;
    boolean                isCallIndex2Valid;
    SSEventErrorType       errorType;
    SSEventErrorCauseType  cause;
} SSEventErrorData;

// Data Indication Event specific data
typedef struct _SSEventDataIndData
{
    SSEventCommonData      commonData; // must be first!
    SSMsg_int_msg_type    *msg;
} SSEventDataIndData;


// SS Operations Event data
typedef union _SSEventData
{
    SSEventCommonData               commonData;
    SSEventCallwiseMsgData          callwiseMsgData;
    SSEventTransactionwiseMsgData   transactionwiseMsgData;
    SSEventErrorData                errorData;
    SSEventDataIndData              dataIndData;
} SSEventData;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: SS_PostEvent

DESCRIPTION: post an event through BREW

PARAMETERS:
   pShell [in]: Pointer to the IShell object
   clsApp [in]: Class of the applet for the event.
   eCode  [in]: AEE Event code
   wParam [in]: Event-specific 16-bit value
   dwParam[in]: Event-specific 32-bit value

RETURN VALUE:
   None

COMMENTS:
   None

=============================================================================*/
void SS_PostEvent(
     IShell   *pShell,
     AEECLSID  clsApp,
     AEEEvent  eCode,
     uint16    wParam,
     uint32    dwParam
     );

/*=============================================================================
FUNCTION: SSOEvent_AllocateEventData

DESCRIPTION: allocate the memory for an event data.

PARAMETERS:
   None

RETURN VALUE:
   Pointer to a new SSEventData structure. Will be NULL if allocation failed.

COMMENTS:
   When structure is no longer used, call SSOEvent_FreeEventData to release it.

=============================================================================*/
SSEventData* SSOEvent_AllocateEventData(SSEventDataType eventDataType);

/*=============================================================================
FUNCTION: SSOEvent_FreeEventData

DESCRIPTION: free the memory allocated to an event data structure.

PARAMETERS:
   pEventData [in]: pointer to an event data structure.

RETURN VALUE:
   None

COMMENTS:
  if eventData is NULL, nothing is done.

=============================================================================*/
void SSOEvent_FreeEventData(
    SSEventData* pEventData
    );

/*=============================================================================
FUNCTION: SSOEvent_PrepareCallWiseMsgEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

=============================================================================*/
SSEventData* SSOEvent_PrepareCallwiseMsgEvent(
    SSCall_Index_type   callIndex1,
    SSCall_Index_type   callIndex2,
    SSMsg_int_msg_type *msg
    );

/*=============================================================================
FUNCTION: SSOEvent_PrepareTransactionWiseMsgEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

=============================================================================*/
SSEventData* SSOEvent_PrepareTransactionwiseMsgEvent(
    SSCall_Index_type        callIndex,
    SSCall_TInfo_Index_type  tInfoIndex,
    SSMsg_int_msg_type      *msg
    );

#endif  //__SS_OPERATIONS_COMMON_EVENT_DEFINITIONS_H__


