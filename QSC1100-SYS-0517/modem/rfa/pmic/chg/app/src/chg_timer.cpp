/*! \file 
 *  \n
 *  \brief  chg_timer.cpp ---- TIMER RELATED CLASS DEFINITION
 *  \details This header file contains class definitions of the TimerEvent 
 *  class and the TimerEventGenerator class.
 *  \n
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_timer.cpp#11 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/05/09   hs      Added support for software controlled CV topoff.
05/05/09   hs      Added support for the improved algorithm of end-of-charge detection.
04/23/09   hs      Added support for the resume-charging detection.
04/03/09   hs      Added support for unit tests.
03/26/09   hs      Removed support for software controlled charging.
03/10/09   hs      Added support for end of charge work around in the 
                   autonomous charging.
12/01/08   hs      Modified AddListener():if the listener with the timeouts  
                   has already been in the listener list, ignore it; otherwise, 
                   add the listener to the list.
11/20/08   hs      Added INTLOCK/INTFREE pairs in the timer callback function
                   to guard the code.
09/23/08   vk      Fixed charger code warnings
05/15/08   hs      Removed the usage of std.
                   Added FEATURE_CHG_TASK.
04/16/08   hs      Name changes for some member functions.
07/12/07   hs      Created.

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "customer.h"                  /* FEATURE definitions    */
#ifndef T_WINNT
#include "pmapp_ads_compat.h"          /* for compiler warnings*/
#endif

#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)
#ifdef T_WINNT
#error code not present
#else 
#include "assert.h"                   /* For ASSERT()                    */
#endif /* T_WINNT */

#include "chg_timer.h"     /*class TimerEvent and ChargerTimerEventGenerator*/
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
                  /***********************************/
                  /*     class ChargerTimerEvent     */
                  /***********************************/
TimerEvent::TimerEvent(unsigned int token)
: ChargingEvent( CHG_CMD__TIMER_EXPIRED ),
    mFirstDelay(0), 
    mRepeatDelay(0), 
    mToken(token)
{
    
}

                  /********************************************/
                  /*     class ChargerTimerEventGenerator     */
                  /********************************************/
TimerEventGenerator::TimerEventGenerator():mTimerToken( 0 )
{
    /* Initialize the listener queues */
    (void) q_init( &mListenerQueue );      /* Initialize listener queue */
    (void) q_init( &mListenerFreeQueue );  /* Initialize listener free queue */

    /* fill listener free queue with listener buffers */
    for (int i = 0; i < CHG_TIMER_LISTENER_BUF_CNT; i++ ) {
      q_put( &mListenerFreeQueue, 
             q_link( &mListenerBuffer[i], &mListenerBuffer[i].mLink) );
    }
}

TimerEventGenerator::~TimerEventGenerator()
{

}

TimerEventGenerator::TimerListenerLinkInfo* TimerEventGenerator::getListenerBuffer(void)
{
    TimerEventGenerator::TimerListenerLinkInfo    *buff_ptr;     /* Pointer to buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*-------------------------------------------------------------------------
      Get a listener buffer from the free listener queue.
      Currently it will already have the handle. If this changes in the future,
      assign a handle to the cmd.
    -------------------------------------------------------------------------*/
    if( (buff_ptr = (TimerEventGenerator::TimerListenerLinkInfo*)q_get( &mListenerFreeQueue )) == NULL )
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

int TimerEventGenerator::listenerInformationCompare(void* item, void* compare_val)
{
    ASSERT( NULL != item);
    ASSERT( NULL != compare_val);

    TimerListenerLinkInfo *item_ptr = (TimerListenerLinkInfo *)item;
    TimerListenerInfo *compare_ptr = (TimerListenerInfo *)compare_val;

    ASSERT( NULL != item_ptr);
    ASSERT( NULL != compare_ptr);

    if( item_ptr->mFirstTimeOut == compare_ptr->mFirstTimeOut &&
        item_ptr->mRepeatTimeOut == compare_ptr->mRepeatTimeOut &&
        item_ptr->mpListener == compare_ptr->mpListener )
    {
        return 1;
    }
    else
    {
        return 0;
    }
} /* end TimerEventGenerator::listenerInformationCompare() */

void TimerEventGenerator::AddListener(
     ChargingEventListener *listener,
     unsigned int firstdelay, 
     unsigned int repeatdelay,
     bool
    )
{
    ASSERT( listener != NULL );

    TimerListenerLinkInfo *listenerinformation = getListenerBuffer();

    ASSERT(listenerinformation != NULL);

    listenerinformation->mpListener     = listener;
    listenerinformation->mTimerToken    = mTimerToken;
    listenerinformation->mFirstTimeOut  = firstdelay;
    listenerinformation->mRepeatTimeOut = repeatdelay;

    /* Initialize link field */
    (void) q_link(listenerinformation, &listenerinformation->mLink );    
    /* Put on listener queue  */
    q_put( &mListenerQueue, &listenerinformation->mLink );        

} /* end TimerEventGenerator::AddListener() */

void TimerEventGenerator::RemoveListener(
     ChargingEventListener *listener,
     unsigned int firstdelay, 
     unsigned int repeatdelay
    )
{
    TimerListenerInfo     compare_value;

    compare_value.mFirstTimeOut  = firstdelay;
    compare_value.mRepeatTimeOut = repeatdelay;
    compare_value.mpListener     = listener;

    TimerListenerLinkInfo* item_ptr = (TimerListenerLinkInfo*)q_linear_search( 
                                                               &mListenerQueue, 
                                                                listenerInformationCompare,
                                                                 &compare_value );
    if( item_ptr != NULL )
    {
      q_delete( &mListenerQueue, (q_link_type*)item_ptr );

      /* put back in free queue */
      q_put( &mListenerFreeQueue, &item_ptr->mLink );
    }

} /* end TimerEventGenerator::RemoveListener() */

void TimerEventGenerator::AddListenerForChargingHeartBeat(ChargingEventListener*)
{
}

void TimerEventGenerator::RemoveListenerForChargingHeatBeat(ChargingEventListener*)
{
}

void TimerEventGenerator::AddListenerForEndOfChargingDetectionTimeout(ChargingEventListener*)
{

}
void TimerEventGenerator::RemoveListenerForEndOfChargingDetectionTimeout(ChargingEventListener*)
{

}
void TimerEventGenerator::AddListenerForResumeChargingDetectionTimeout(ChargingEventListener*)
{

}
void TimerEventGenerator::RemoveListenerForResumeChargingDetectionTimeout(ChargingEventListener*)
{

}
void TimerEventGenerator::AddListenerForEndOfFastChargeDetectionTimeout(ChargingEventListener*)
{

}
void TimerEventGenerator::RemoveListenerForEndOfFastChargeDetectionTimeout(ChargingEventListener*)
{

}

#if defined(FEATURE_CHG_TASK_USE_CV_TOPOFF) || defined(T_WINNT)
void TimerEventGenerator::AddListenerForEndOfCVTopoffDetection1Timeout(ChargingEventListener*)
{

}
void TimerEventGenerator::RemoveListenerForEndOfCVTopoffDetection1Timeout(ChargingEventListener*)
{

}
void TimerEventGenerator::AddListenerForEndOfCVTopoffDetection2Timeout(ChargingEventListener*)
{

}
void TimerEventGenerator::RemoveListenerForEndOfCVTopoffDetection2Timeout(ChargingEventListener*)
{

}
#endif /* FEATURE_CHG_TASK_USE_CV_TOPOFF */

void TimerEventGenerator::FireEvent( ChargingEvent* event )
{
    TimerEvent* timerevent = dynamic_cast< TimerEvent* >( event );
    ASSERT( timerevent != NULL );

    unsigned int  token = timerevent->GetToken();

    ChargingEventListener* listenerCopy[CHG_TIMER_LISTENER_BUF_CNT];
    unsigned int           tokenCopy[CHG_TIMER_LISTENER_BUF_CNT];
    unsigned int           firstTimeoutCopy[CHG_TIMER_LISTENER_BUF_CNT];
    unsigned int           repeatTimeoutCopy[CHG_TIMER_LISTENER_BUF_CNT];
    int index = 0;

    TimerListenerLinkInfo* item_ptr = (TimerListenerLinkInfo*)q_check(&mListenerQueue);

   /* Iterate the queue and make a copy */
   while(item_ptr != NULL)
   {
       listenerCopy[index]      = item_ptr->mpListener;
       firstTimeoutCopy[index]  = item_ptr->mFirstTimeOut;
       repeatTimeoutCopy[index] = item_ptr->mRepeatTimeOut;
       tokenCopy[index++]       = item_ptr->mTimerToken;

       item_ptr = (TimerListenerLinkInfo*)q_next(&mListenerQueue, &item_ptr->mLink);
   }

   listenerCopy[ index ] = NULL;

   /* Iterate through the listenerCopy */
   index = 0;

   while(listenerCopy[ index ] != NULL )
   {
       if( tokenCopy[ index ] == token )
       {
           timerevent->SetFirstDelay( firstTimeoutCopy[index]);
           timerevent->SetRepeatDelay( repeatTimeoutCopy[index]);
           /* Fire event */
           listenerCopy[ index ]->EventFired(timerevent);
       }
       index++;
   }

} /* end TimerEventGenerator::FireEvent() */

                  /*****************************************/
                  /*     class RexTimerEventGenerator      */
                  /*****************************************/

RexTimerEventGenerator::RexTimerEventGenerator()
{
    /* Initialize the timer queues */
    (void) q_init( &mTimerQueue );      /* Initialize timer queue */
    (void) q_init( &mTimerFreeQueue );  /* Initialize timer free queue */

    /* fill listener free queue with listener buffers */
    for (int i = 0; i < CHG_TIMER_CNT; i++ ) {
      q_put( &mTimerFreeQueue, 
             q_link( &mTimerBuffer[i], &mTimerBuffer[i].mLink) );
    }
}

RexTimerEventGenerator::~RexTimerEventGenerator()
{
    /* Release memory allocated to the timers */

    TimerListenerLinkInfo* item_ptr = (TimerListenerLinkInfo*)q_check(&mListenerQueue);

   /* Iterate the queue */
    while( item_ptr != NULL )
    {
        TimerLinkInfo* item_ptr2 = (TimerLinkInfo*)q_linear_search( 
                                                     &mTimerQueue, 
                                                       tokenInformationCompare,
                                                         (timer_type*)item_ptr->mTimerToken );

        if( item_ptr2 != NULL )
        {
            q_delete( &mTimerQueue, (q_link_type*)item_ptr2 );

            /* put back in free queue */
            q_put( &mTimerFreeQueue, &item_ptr2->mLink );

            #ifndef T_WINNT
            /* Stop the timer */
            timer_clr( (timer_type*)item_ptr->mTimerToken, T_NONE );
            #endif

            /* release memory */
            delete (timer_type*)(item_ptr->mTimerToken);
        } /* end if( item_ptr2 != NULL ) */

        /* Get next item */
        item_ptr = (TimerListenerLinkInfo*)q_next(&mListenerQueue, &item_ptr->mLink);

    } /* if( item_ptr != NULL ) */

} /* end RexTimerEventGenerator::~RexTimerEventGenerator() */

int RexTimerEventGenerator::timerInformationCompare(void* item, void* compare_val)
{
    ASSERT( NULL != item);
    ASSERT( NULL != compare_val);

    TimerListenerLinkInfo *item_ptr = (TimerListenerLinkInfo *)item;
    TimerListenerInfo *compare_ptr = (TimerListenerInfo *)compare_val;

    ASSERT( NULL != item_ptr);
    ASSERT( NULL != compare_ptr);

    if( item_ptr->mFirstTimeOut == compare_ptr->mFirstTimeOut &&
        item_ptr->mRepeatTimeOut == compare_ptr->mRepeatTimeOut )
    {
        return 1;
    }
    else
    {
        return 0;
    }
} /* end RexTimerEventGenerator::timerInformationCompare() */

int RexTimerEventGenerator::tokenInformationCompare(void* item, void* compare_val)
{
    ASSERT( NULL != item);
    ASSERT( NULL != compare_val);

    TimerLinkInfo *item_ptr = (TimerLinkInfo *)item;
    timer_type *compare_ptr = (timer_type *)compare_val;

    ASSERT( NULL != item_ptr);
    ASSERT( NULL != compare_ptr);

    if( item_ptr->mpTimer == compare_ptr)
    {
        return 1;
    }
    else
    {
        return 0;
    }
} /* end RexTimerEventGenerator::tokenInformationCompare() */

RexTimerEventGenerator::TimerLinkInfo* RexTimerEventGenerator::getTimerBuffer(void)
{
    RexTimerEventGenerator::TimerLinkInfo    *buff_ptr;     /* Pointer to buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*-------------------------------------------------------------------------
      Get a timer buffer from the free timer queue.
      Currently it will already have the handle. If this changes in the future,
      assign a handle to the cmd.
    -------------------------------------------------------------------------*/
    if( (buff_ptr = (RexTimerEventGenerator::TimerLinkInfo*)q_get( &mTimerFreeQueue )) == NULL )
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

void RexTimerEventGenerator::AddListener(
    ChargingEventListener *listener,
    unsigned int firstdelay, 
    unsigned int repeatdelay,
    bool shared
    )
{
    timer_type* pTimerType;

    ASSERT( listener != NULL );

    TimerListenerInfo    listener_compare_value;

    listener_compare_value.mpListener = listener;
    listener_compare_value.mFirstTimeOut = firstdelay;
    listener_compare_value.mRepeatTimeOut = repeatdelay;

    TimerListenerLinkInfo* listener_item_ptr = (TimerListenerLinkInfo*)q_linear_search( 
                                                                         &mListenerQueue, 
                                                                          listenerInformationCompare,
                                                                           &listener_compare_value );

    /* If the listener with the timout is already in the instener list, ignore; 
       otherwise, add it to the list. */
    if(listener_item_ptr == NULL)
    {
        if( shared == true )
        {
            // Set shared to false until we find a timer that is already created
            // If it is not already created we need to create it, so it will be 
            // as if shared was false
            shared = false;

            TimerListenerInfo     timer_compare_value;

            timer_compare_value.mFirstTimeOut  = firstdelay;
            timer_compare_value.mRepeatTimeOut = repeatdelay;
            timer_compare_value.mpListener     = listener;

            TimerListenerLinkInfo* item_ptr = (TimerListenerLinkInfo*)q_linear_search( 
                                                                       &mListenerQueue, 
                                                                        timerInformationCompare,
                                                                         &timer_compare_value );
            if( item_ptr != NULL ) /* Timer already defined */
            {
                mTimerToken = item_ptr->mTimerToken;
                shared = true;
            }
        } /* end if( shared == true ) */

        /* New timer */
        if( shared == false )
        {
            pTimerType = new timer_type;

            TimerLinkInfo *timerinformation = getTimerBuffer();

            ASSERT(timerinformation != NULL);

            timerinformation->mpTimer = pTimerType;

            /* Initialize link field */
            (void) q_link(timerinformation, &timerinformation->mLink );    
            /* Put on timer queue  */
            q_put( &mTimerQueue, &timerinformation->mLink );        

            #ifndef T_WINNT
            /* Create and start a REX timer */
            timer_def2(pTimerType, 0 ); 
            timer_reg(pTimerType,                      /*!< Timer to set */
                      timerCallBack,                   /*!< Function to call at timer expiry */
                      (timer_cb_data_type)pTimerType,  /*!< Arbitrary data for func(time_ms, data). */
                      firstdelay,                         /*!< Time (in ms) until first timer expiry */
                      repeatdelay); /*!< Period (in ms) between repeated expiries (0 = not periodic) */
            #endif

            mTimerToken = (unsigned int)pTimerType;
        } /* end if( shared == false ) */

        TimerEventGenerator::AddListener( listener, firstdelay, repeatdelay );

    } /* end new listener */

} /* end RexChargerTimerEventGenerator::AddListener() */

void RexTimerEventGenerator::RemoveListener(
    ChargingEventListener *listener,
    unsigned int firstdelay, 
    unsigned int repeatdelay
    )
{
   ASSERT( listener != NULL );

   TimerListenerInfo     compare_value;

   compare_value.mFirstTimeOut  = firstdelay;
   compare_value.mRepeatTimeOut = repeatdelay;
   compare_value.mpListener     = listener;

   TimerListenerLinkInfo* item_ptr = (TimerListenerLinkInfo*)q_linear_search( 
                                                              &mListenerQueue, 
                                                               listenerInformationCompare,
                                                                &compare_value );
   if( item_ptr != NULL )
   {
       TimerLinkInfo* item_ptr2 = (TimerLinkInfo*)q_linear_search( 
                                                    &mTimerQueue, 
                                                      tokenInformationCompare,
                                                        (timer_type*)item_ptr->mTimerToken );

       if( item_ptr2 != NULL )
       {
           q_delete( &mTimerQueue, (q_link_type*)item_ptr2 );

           /* put back in free queue */
           q_put( &mTimerFreeQueue, &item_ptr2->mLink );

           #ifndef T_WINNT
           /* Stop the timer */
           timer_clr( (timer_type*)item_ptr->mTimerToken, T_NONE );
           #endif

           /* release memory */
           delete (timer_type*)(item_ptr->mTimerToken);
       } /* end if( item_ptr2 != NULL ) */

       /* Remove the listener from the listener queue */
       q_delete( &mListenerQueue, (q_link_type*)item_ptr );

       /* put back in free queue */
       q_put( &mListenerFreeQueue, &item_ptr->mLink );

   } /* if( item_ptr != NULL ) */
} /* end RexChargerTimerEventGenerator::RemoveListener() */

void RexTimerEventGenerator::AddListenerForChargingHeartBeat(ChargingEventListener *listener)
{
    AddListener(listener, 
                CHG_HEART_BEAT_FIRST_TIME,
                CHG_HEART_BEAT,
                true);
}

void RexTimerEventGenerator::RemoveListenerForChargingHeatBeat(ChargingEventListener *listener)
{
    RemoveListener(listener,
                   CHG_HEART_BEAT_FIRST_TIME,
                   CHG_HEART_BEAT);
}


void RexTimerEventGenerator::AddListenerForEndOfChargingDetectionTimeout(ChargingEventListener *listener)
{
    AddListener(listener, 
                AUTO_CHARGING_END_CHARGE_DETECTION_T,
                0,
                false);

}
void RexTimerEventGenerator::RemoveListenerForEndOfChargingDetectionTimeout(ChargingEventListener *listener)
{
    RemoveListener(listener,
                   AUTO_CHARGING_END_CHARGE_DETECTION_T,
                   0);

}

void RexTimerEventGenerator::AddListenerForResumeChargingDetectionTimeout(ChargingEventListener *listener)
{
    AddListener(listener, 
                AUTO_CHARGING_RESUME_CHARGE_DETECTION_T,
                AUTO_CHARGING_RESUME_CHARGE_DETECTION_T,
                false);
}
void RexTimerEventGenerator::RemoveListenerForResumeChargingDetectionTimeout(ChargingEventListener *listener)
{
    RemoveListener(listener,
                   AUTO_CHARGING_RESUME_CHARGE_DETECTION_T,
                   AUTO_CHARGING_RESUME_CHARGE_DETECTION_T);
}

void RexTimerEventGenerator::AddListenerForEndOfFastChargeDetectionTimeout(ChargingEventListener *listener)
{
    AddListener(listener, 
                AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T,
                AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T,
                false);
}
void RexTimerEventGenerator::RemoveListenerForEndOfFastChargeDetectionTimeout(ChargingEventListener *listener)
{
    RemoveListener(listener,
                   AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T,
                   AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T);
}

#if defined(FEATURE_CHG_TASK_USE_CV_TOPOFF) || defined(T_WINNT)

void RexTimerEventGenerator::AddListenerForEndOfCVTopoffDetection1Timeout(ChargingEventListener *listener)
{
    AddListener(listener, 
                AUTO_CHARGING_END_CV_TOPOFF_DETECTION_1_T,
                AUTO_CHARGING_END_CV_TOPOFF_DETECTION_1_T,
                false);
}
void RexTimerEventGenerator::RemoveListenerForEndOfCVTopoffDetection1Timeout(ChargingEventListener *listener)
{
    RemoveListener(listener,
                   AUTO_CHARGING_END_CV_TOPOFF_DETECTION_1_T,
                   AUTO_CHARGING_END_CV_TOPOFF_DETECTION_1_T);
}

void RexTimerEventGenerator::AddListenerForEndOfCVTopoffDetection2Timeout(ChargingEventListener *listener)
{
    AddListener(listener, 
                AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T,
                AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T,
                false);
}
void RexTimerEventGenerator::RemoveListenerForEndOfCVTopoffDetection2Timeout(ChargingEventListener *listener)
{
    RemoveListener(listener,
                   AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T,
                   AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T);
}

#endif /* FEATURE_CHG_TASK_USE_CV_TOPOFF */

void RexTimerEventGenerator::timerCallBack(int32, timer_cb_data_type token)
{
    /* Construct a charging event */
    INTLOCK();
    chg_task_evt_type            chg_task_event;
    chg_task_event.event_id      = CHG_EVT__TIMER;
    chg_task_event.timer_token   = token;

    /* Post the event on charger task command queue */
    chg_push_event_on_command_queue( chg_task_event );
    INTFREE();
}

#endif /* FEATURE_CHG_TASK || T_WINNT */
