/*=========================================================================
FILE: SSqueue.c

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

#include "SSqueue.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

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
SSQueue* SSQueue_New(void)
{
    SSQueue* pQueue = MALLOC(sizeof(SSQueue));
    
    return pQueue;
}

/*=============================================================================
FUNCTION: SSQueue_Release

DESCRIPTION: Release a SSQueue instance

PARAMETERS:
   pCallDB [in] : pointer to a valid SSQueue

RETURN VALUE:
    None
=============================================================================*/
void SSQueue_Release(SSQueue* pQueue)
{
    FREE(pQueue);
}

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
boolean SSQueue_Put(SSQueue* pQueue, SSQueueEvent * eventIn)
{    
    SSQueueEvent *pe = NULL;
    
    //Validate input parameters
    if((pQueue == NULL) || (eventIn == NULL))
    {
       return(FALSE); 
    }
    
    // See if there is room in the event queue. If not, return
    // false...
    if(pQueue->m_nEventIn == pQueue->m_nEventOut && pQueue->m_nEvents)
    {
       return(FALSE);    // Queue is full!
    }

    pe = &pQueue->m_eventQueue[pQueue->m_nEventIn];
    if(++pQueue->m_nEventIn >= SS_EVENT_QUEUE_SIZE)
    {
       pQueue->m_nEventIn = 0;
    }

    // copy event data into the free item
    pe->evt     = eventIn->evt;
    pe->dwParam = eventIn->dwParam;
    pe->wParam  = eventIn->wParam;
    
    pQueue->m_nEvents++;

    return TRUE;
}

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
boolean SSQueue_GetFirst(SSQueue* pQueue, SSQueueEvent * pOutEvent)
{
   /*  Validate input parameters  */
   if((pQueue == NULL) || (pOutEvent == NULL))
   {
       return FALSE; 
   }

   /* Check to see if there are any pending events that have been posted....  */
   if(pQueue->m_nEvents > 0)
   {

       /* Grab the event. Copy it so that we can use the spot in the list... */
       
       *pOutEvent = pQueue->m_eventQueue[pQueue->m_nEventOut];
       
       if(++pQueue->m_nEventOut >= SS_EVENT_QUEUE_SIZE)
       {
           pQueue->m_nEventOut = 0;
       }
       
       pQueue->m_nEvents--;   
   }
   else
   {
      /* return nothing from an empty queue  */
      return FALSE;
   }
   
   return TRUE;
}
