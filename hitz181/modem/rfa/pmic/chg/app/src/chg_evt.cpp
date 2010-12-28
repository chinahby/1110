/*! \file 
 *  \n
 *  \brief  chg_evt.cpp ---- CHARGING EVENT RELATED CLASS DEFINITION
 *  \details This header file contains class definitions of the ChargingEvent
 *  class, the ChargingEventGenerator class and the ChargingEventListener 
 *  class.
 *  \n
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_evt.cpp#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/09   hs      Added support for unit tests.
05/14/08   hs      Removed the usage of std.
                   Added FEATURE_CHG_TASK.
07/12/07   hs      Created.

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "customer.h"                  /* FEATURE definitions    */

#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)

#include "chg_evt.h"                  /* For class definitions             */
#ifdef T_WINNT
#error code not present
#else
#include "assert.h"                   /* For ASSERT()                      */
#endif
/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */
                  /*********************************/
                  /*     class ChargingEvent       */
                  /*********************************/

                  /****************************************/
                  /*     class ChargingEventGenerator     */
                  /****************************************/
ChargingEventGenerator::ChargingEventGenerator()
{
    /* Initialize the listener queues */
    (void) q_init( &mListenerQueue );      /* Initialize listener queue */
    (void) q_init( &mListenerFreeQueue );  /* Initialize listener free queue */

    /* fill listener free queue with listener buffers */
    for (int i = 0; i < CHG_EVT_LISTENER_BUF_CNT; i++ ) {
      q_put( &mListenerFreeQueue, 
             q_link( &mListenerBuffer[i], &mListenerBuffer[i].mLink) );
    }

}
ChargingEventGenerator::~ChargingEventGenerator()
{

}

ChargingEventGenerator::EventListenerLinkInfo* ChargingEventGenerator::getListenerBuffer(void)
{
    ChargingEventGenerator::EventListenerLinkInfo    *buff_ptr;   /* Pointer to buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*-------------------------------------------------------------------------
      Get a listener buffer from the free listener queue.
      Currently it will already have the handle. If this changes in the future,
      assign a handle to the cmd.
    -------------------------------------------------------------------------*/
    if( (buff_ptr = (ChargingEventGenerator::EventListenerLinkInfo*)q_get( &mListenerFreeQueue )) == NULL )
    {
      /*-----------------------------------------------------------------------
        No free listener buffers available, log an error.
      -----------------------------------------------------------------------*/
      ASSERT(false);
    }

    /*-------------------------------------------------------------------------
      Note that the cmd_ptr may be NULL if there were no free listener buffers
      available. The calling task must either handle the NULL return value
      or ERR_FATAL.
    -------------------------------------------------------------------------*/
    return( buff_ptr );
}

int ChargingEventGenerator::listenerInformationCompare(void* item, void* compare_val)
{
    ASSERT( NULL != item);
    ASSERT( NULL != compare_val);

    EventListenerLinkInfo *item_ptr    = (EventListenerLinkInfo *)item;
    ChargingEventListener *compare_ptr = (ChargingEventListener *)compare_val;

    ASSERT( NULL != item_ptr);
    ASSERT( NULL != compare_ptr);

    if(item_ptr->mpListener == compare_ptr)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void ChargingEventGenerator::AddListener(ChargingEventListener *listener)
{
    ASSERT(listener);

    EventListenerLinkInfo *listenerinformation = getListenerBuffer();

    ASSERT(listenerinformation != NULL);

    listenerinformation->mpListener = listener;

    /* Initialize link field */
    (void) q_link(listenerinformation, &listenerinformation->mLink );    
    /* Put on listener queue  */
    q_put( &mListenerQueue, &listenerinformation->mLink );  

} /* end ChargingEventGenerator::AddListener()*/

void ChargingEventGenerator::RemoveListener(ChargingEventListener *listener)
{
    ASSERT(listener);

    if(q_cnt(&mListenerQueue) != 0) /* Listener queue is not empty */
    {
        EventListenerLinkInfo* item_ptr = (EventListenerLinkInfo*)q_linear_search( 
                                                                    &mListenerQueue, 
                                                                      listenerInformationCompare,
                                                                       listener );
        if( item_ptr != NULL )  
        {
          q_delete( &mListenerQueue, (q_link_type*)item_ptr );

          /* put back in free queue */
          q_put( &mListenerFreeQueue, &item_ptr->mLink );
        }
    }

} /* end ChargingEventGenerator::RemoveListener()*/

void ChargingEventGenerator::FireEvent(ChargingEvent *event)
{
    ASSERT( event != NULL );

    ChargingEventListener* listenerCopy[CHG_EVT_LISTENER_BUF_CNT];
    int index = 0;

    EventListenerLinkInfo* item_ptr = (EventListenerLinkInfo*)q_check(&mListenerQueue);

   /* Iterate the queue and make a copy */
   while(item_ptr != NULL)
   {
       listenerCopy[index++] = item_ptr->mpListener;
       item_ptr = (EventListenerLinkInfo*)q_next(&mListenerQueue, &item_ptr->mLink);
   }

   listenerCopy[ index ] = NULL;

   /* Iterate through the listenerCopy */
   index = 0;

   while(listenerCopy[ index ] != NULL )
   {
       /* Fire event */
       listenerCopy[ index++ ]->EventFired(event);
   }
} /* end ChargingEventGenerator::FireEvent()*/

                  /***************************************/
                  /*     class ChargingEventListener     */
                  /***************************************/
ChargingEventListener::ChargingEventListener()
{

}
ChargingEventListener::~ChargingEventListener()
{

}
void ChargingEventListener::EventFired(ChargingEvent* event)
{
    ASSERT( NULL != event);
}

#endif /* FEATURE_CHG_TASK || T_WINNT */
