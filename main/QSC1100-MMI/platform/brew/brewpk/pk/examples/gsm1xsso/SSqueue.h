#ifndef __SS_QUEUE_H__
#define __SS_QUEUE_H__

/*=========================================================================
FILE: SSqueue.h

SERVICES: Event Queue with a predefined size.

GENERAL DESCRIPTION:
    Used by the SSO FSM to queue events that are sent between the various
    FSM sub state machines.

PUBLIC CLASSES AND STATIC FUNCTIONS:
    SSQueue

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "AEE.h"            // Standard AEE definitions
#include "AEEStdLib.h"        // BREW's standard library

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/
// the predefined queue size
#define SS_EVENT_QUEUE_SIZE   (20)

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

// Async event. This will be used as the queue items.
typedef struct _SSQueueEvent
{
   AEEEvent    evt;
   uint16      wParam;
   uint32      dwParam;
} SSQueueEvent;


// event queue type
typedef struct _SSQueue
{
    // queue items
    SSQueueEvent      m_eventQueue[SS_EVENT_QUEUE_SIZE];
    int               m_nEventIn;    // Input index (async)
    int               m_nEventOut;   // Output index (foreground)
    int               m_nEvents;     // number of pending events

} SSQueue;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: SSQueue_New

DESCRIPTION: Create a SSQueue instance

PARAMETERS:
    None

RETURN VALUE:
    pointer to the new SSQueue or NULL on MALLOC failure.

COMMENTS:
    in order to free SSQueue use SSQueue_Release().
=============================================================================*/
SSQueue* SSQueue_New(void);

/*=============================================================================
FUNCTION: SSQueue_Release

DESCRIPTION: Release a SSQueue instance

PARAMETERS:
   pCallDB [in] : pointer to a valid SSQueue

RETURN VALUE:
    None
=============================================================================*/
void SSQueue_Release(SSQueue *pCallDB);

/*=============================================================================
FUNCTION: SSQueue_Put

DESCRIPTION: Puts an event at the end of the FIFO queue

PARAMETERS:
    pQueue [in]  : the queue in which to place the event
    eventIn [in] : the event data to put in the queue. The event data is
                   copied into the queue. pQueue does not take Ownership
                   over eventIn.

RETURN VALUE:
    TRUE if the event was added to the queue or FALSE otherwise.
=============================================================================*/
boolean SSQueue_Put(SSQueue *pQueue, SSQueueEvent *eventIn);

/*=============================================================================
FUNCTION: SSQueue_GetFirst

DESCRIPTION: Takes out an event from the head of the FIFO queue

PARAMETERS:
    pQueue [in]     : the queue from which to take out the event.
    pOutEvent [out] : the event is returned inside this parameter. The caller
                      shall allocate the memory for this parameter.

RETURN VALUE:
    TRUE if an event was taken out of the queue or FALSE otherwise.
=============================================================================*/
boolean SSQueue_GetFirst(SSQueue *pQueue, SSQueueEvent *pOutEvent);

#endif  //__SS_QUEUE_H__
