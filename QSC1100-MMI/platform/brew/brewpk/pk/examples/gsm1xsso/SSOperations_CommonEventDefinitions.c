/*=========================================================================
FILE: SSOperations_CommonEventDefinitions.c

SERVICES: SS Operations Common Event Definitions

GENERAL DESCRIPTION:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                        INCLUDE FILES 
=============================================================================== */

#include "AEEStdLib.h"        // AEE StdLib Services
#include "AEEShell.h"

#include "SSOperations_CommonEventDefinitions.h"
#include "SSOperations_CommonMessageDefinitions.h"
#include "SS_CommonMacros.h"

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/
// ISHELL_PostEventEx is provided by BREW starting from version 2.1
#ifndef ISHELL_PostEventEx
	// for previous BREW version, we have a private define:
	// ISHELL_PostEventEx with flgs=0 is like ISHELL_PostEvent, except EVTFLG_UNIQUE.
	#define ISHELL_PostEventEx(p,flgs,cls,ec,wp,dw) GET_PVTBL(p,IShell)->SendEvent(p,((EVTFLG_ASYNC)|(flgs)),cls,ec,wp,dw)
#endif

/*===============================================================================
                    FUNCTION DEFINITIONS
================================================================================*/

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
	Uses ISHELL_PostEventEx to allow multiple event from the same type to be
	posted in BREW's queue.
	If ISHELL_PostEvent fails, the application is closed.
=============================================================================*/
void SS_PostEvent(
     IShell   *pShell,
     AEECLSID  clsApp,
     AEEEvent  eCode,
     uint16    wParam,
     uint32    dwParam
     )
{
    if (ISHELL_PostEventEx(pShell, 0, clsApp, eCode, wParam, dwParam) != TRUE)
    {
        DBGPRINTF("SS_PostEvent failed!");
        ISHELL_CloseApplet(pShell, FALSE); 
    }
}

/*=============================================================================
FUNCTION: SSOEvent_AllocateEventData

DESCRIPTION: allocate the memory for an event data.

PARAMETERS:
   eventDataType: type of event to allocate

RETURN VALUE:
   Pointer to a new SSEventData structure. Will be NULL if allocation failed.

COMMENTS:
   When structure is no longer used, call SSOEvent_FreeEventData to release it.

=============================================================================*/
SSEventData* SSOEvent_AllocateEventData(SSEventDataType eventDataType)
{
    SSEventData* result = MALLOC( sizeof(SSEventData) );
    if (result == NULL) return NULL;

    result->commonData.type = eventDataType;
    return result;
}

/*=============================================================================
FUNCTION: SSOEvent_FreeEventData

DESCRIPTION: free the memory allocated to an event data structure.

PARAMETERS:
   *pEventData [in]: pointer to an event data structure

RETURN VALUE:
   None

COMMENTS:
  if eventData == NULL, nothing is done.

=============================================================================*/
void SSOEvent_FreeEventData(
    SSEventData* pEventData
    )
{
    if (pEventData == NULL) return;

    switch (pEventData->commonData.type)
    {
        case SS_EVENT_DATA_CALLWISE_MSG:
            if (pEventData->callwiseMsgData.msg != NULL)
                SSOMessage_FreeMsg(pEventData->callwiseMsgData.msg);
            break;

        case SS_EVENT_DATA_TRANSACTIONWISE_MSG:
            if (pEventData->transactionwiseMsgData.msg != NULL)
                SSOMessage_FreeMsg(pEventData->transactionwiseMsgData.msg);
            break;

        case SS_EVENT_DATA_DATA_IND:
            if (pEventData->dataIndData.msg != NULL)
                SSOMessage_FreeMsg(pEventData->dataIndData.msg);
            break;

        case SS_EVENT_DATA_ERROR:
            break;

        default:
            assert(!"Unknown SS event type value");
    }

#ifdef TESTING
    // for Testing - make sure that the event data is cleaned!
    MEMSET(pEventData, 0, sizeof(*pEventData));
#endif

    FREEIF(pEventData);
}

/*=============================================================================
FUNCTION: SSOEvent_PrepareCallWiseMsgEvent

DESCRIPTION: 

PARAMETERS:
    callIndex1 [in]: Index of first call that participates in the event.
    callIndex2 [in]: Index of second call that participates in the event.
    msg [in]:  Signalling message to copy from.

RETURN VALUE:
    Pointer to SSEventData produced, or NULL on failure.

COMMENTS:

=============================================================================*/
SSEventData* SSOEvent_PrepareCallwiseMsgEvent(
    SSCall_Index_type   callIndex1,
    SSCall_Index_type   callIndex2,
    SSMsg_int_msg_type *msg)
{
    SSEventData* pEventData = SSOEvent_AllocateEventData(SS_EVENT_DATA_CALLWISE_MSG);
    if (pEventData != NULL)
    {
        pEventData->callwiseMsgData.callIndex1 = callIndex1;
        pEventData->callwiseMsgData.callIndex2 = callIndex2;
        if (msg != NULL)
        {
            pEventData->callwiseMsgData.msg = MALLOC(sizeof(SSMsg_int_msg_type));

            if (pEventData->callwiseMsgData.msg != NULL)
            {
                MEMCPY(pEventData->callwiseMsgData.msg, msg, sizeof(SSMsg_int_msg_type));
            }
            else
            {
                SSOEvent_FreeEventData(pEventData);
                pEventData = NULL;
            }
        }
        else
            pEventData->callwiseMsgData.msg = NULL;
    }

    return pEventData;
}

/*=============================================================================
FUNCTION: SSOEvent_PrepareTransactionWiseMsgEvent

DESCRIPTION: 

PARAMETERS:
    callIndex [in]: Index of call that participates in the event.
    tInfoIndex [in]: Index of transaction that participates in the event.
    msg [in]:  Signalling message to copy from.

RETURN VALUE:
    Pointer to SSEventData produced, or NULL on failure.

COMMENTS:

=============================================================================*/
SSEventData* SSOEvent_PrepareTransactionwiseMsgEvent(
    SSCall_Index_type        callIndex,
    SSCall_TInfo_Index_type  tInfoIndex,
    SSMsg_int_msg_type      *msg)
{
    SSEventData* pEventData = SSOEvent_AllocateEventData(SS_EVENT_DATA_TRANSACTIONWISE_MSG);
    if (pEventData != NULL)
    {
        pEventData->transactionwiseMsgData.callIndex = callIndex;
        pEventData->transactionwiseMsgData.tInfoIndex = tInfoIndex;
        if (msg != NULL)
        {
            pEventData->transactionwiseMsgData.msg = MALLOC(sizeof(SSMsg_int_msg_type));

            if (pEventData->transactionwiseMsgData.msg != NULL)
            {
                MEMCPY(pEventData->transactionwiseMsgData.msg, msg, sizeof(SSMsg_int_msg_type));
            }
             else
            {
                SSOEvent_FreeEventData(pEventData);
                pEventData = NULL;
            }

        }
        else
            pEventData->transactionwiseMsgData.msg = NULL;
    }

    return pEventData;
}
