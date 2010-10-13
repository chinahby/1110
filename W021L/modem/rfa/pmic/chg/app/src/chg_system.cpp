/*! \file 
 *  \n
 *  \brief  chg_system.cpp ---- SYSTEM RELATED CLASS DEFINITION
 *  \details This header file contains class definition of the SystemEvent 
 *  class.
 *  \n
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_system.cpp#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/11/09   hs      Added support for USB valid/invalid.
04/03/09   hs      Added support for unit tests.
03/26/09   hs      Code cleanup.
11/20/08   hs      Added INTLOCK/INTFREE pairs to guard the code.
10/04/08   vk      Add/Remove listener for usb suspend case
09/26/08   vk      Resolved compiler warnings
05/12/08   hs      Removed the usage of std.
                   Added FEATURE_CHG_TASK.
09/05/07   hs      Created.

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "customer.h"            /* FEATURE definitions                    */

#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)

#ifdef T_WINNT
#error code not present
#else 
#include "assert.h"                   /* For ASSERT()                    */
#endif /* T_WINNT */
#include "chg_system.h"          /**/
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
                  /*     class ChargerSystemEvent    */
                  /***********************************/
SystemEvent::SystemEvent(chg_system_event_id_type systemeventid,
                         chg_tx_wait_voter_type    voterid):
    ChargingEvent(CHG_CMD__SYSTEM_STATUS_CHANGED),
    mSystemEventId(systemeventid),
    mVoterId(voterid)
{

}

SystemEvent::~SystemEvent()
{

}

                  /**************************************/
                  /*     class SystemEventGenerator     */
                  /**************************************/
SystemEventGenerator::SystemEventGenerator()
{
    /* Initialize the listener queues */
    (void) q_init( &mListenerQueue );      /* Initialize listener queue */
    (void) q_init( &mListenerFreeQueue );  /* Initialize listener free queue */

    /* fill listener free queue with listener buffers */
    for (int i = 0; i < CHG_SYS_EVT_LISTENER_BUF_CNT; i++ ) {
      q_put( &mListenerFreeQueue, 
             q_link( &mListenerBuffer[i], &mListenerBuffer[i].mLink) );
    }
}

SystemEventGenerator::~SystemEventGenerator()
{

}

SystemEventGenerator::SystemEventListenerLinkInfo* SystemEventGenerator::getListenerBuffer(void)
{
    SystemEventGenerator::SystemEventListenerLinkInfo    *buff_ptr;   /* Pointer to buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*-------------------------------------------------------------------------
      Get a listener buffer from the free listener queue.
      Currently it will already have the handle. If this changes in the future,
      assign a handle to the cmd.
    -------------------------------------------------------------------------*/
    if( (buff_ptr = (SystemEventGenerator::SystemEventListenerLinkInfo*)q_get( &mListenerFreeQueue )) == NULL )
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

int SystemEventGenerator::listenerInformationCompare(void* item, void* compare_val)
{
    ASSERT( NULL != item);
    ASSERT( NULL != compare_val);

    SystemEventListenerLinkInfo *item_ptr = (SystemEventListenerLinkInfo *)item;
    SystemEventListenerInfo *compare_ptr = (SystemEventListenerInfo *)compare_val;

    ASSERT( NULL != item_ptr);
    ASSERT( NULL != compare_ptr);

    if(item_ptr->mSystemEventId == compare_ptr->mSystemEventId &&
       item_ptr->mpListener == compare_ptr->mpListener)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void SystemEventGenerator::FireEvent( ChargingEvent* event )
{
    SystemEvent* systemevent = dynamic_cast< SystemEvent* >( event );
    ASSERT( systemevent != NULL );

    chg_system_event_id_type systemid = systemevent->GetSystemEventId();

    ChargingEventListener*       listenerCopy[CHG_SYS_EVT_LISTENER_BUF_CNT];
    chg_system_event_id_type     idCopy[CHG_SYS_EVT_LISTENER_BUF_CNT];
    int index = 0;

    SystemEventListenerLinkInfo* item_ptr = (SystemEventListenerLinkInfo*)q_check(&mListenerQueue);

   /* Iterate the queue and make a copy */
   while(item_ptr != NULL)
   {
       listenerCopy[index] = item_ptr->mpListener;
       idCopy[index++] = item_ptr->mSystemEventId;

       item_ptr = (SystemEventListenerLinkInfo*)q_next(&mListenerQueue, &item_ptr->mLink);
   }

   listenerCopy[ index ] = NULL;

   /* Iterate through the listenerCopy */
   index = 0;

   while(listenerCopy[ index ] != NULL )
   {
       if( idCopy[ index ] == systemid )
       {
           /* Fire event */
           listenerCopy[ index ]->EventFired(systemevent);
       }
       index++;
   }
} /* end SystemEventGenerator::FireEvent  */

void SystemEventGenerator::addListener(
                                ChargingEventListener          *listener,
                                chg_system_event_id_type       systemeventid)
{
    ASSERT(listener != NULL);

    SystemEventListenerLinkInfo *listenerinformation = getListenerBuffer();

    ASSERT(listenerinformation != NULL);

    listenerinformation->mpListener = listener;
    listenerinformation->mSystemEventId = systemeventid;

    /* Initialize link field */
    (void) q_link(listenerinformation, &listenerinformation->mLink );    
    /* Put on listener queue  */
    q_put( &mListenerQueue, &listenerinformation->mLink );  

} /* end SystemEventGenerator::addListener() */

void SystemEventGenerator::removeListener(
                                 ChargingEventListener          *listener,
                                 chg_system_event_id_type       systemeventid)
{
    SystemEventListenerInfo     compare_value;

    compare_value.mSystemEventId = systemeventid;
    compare_value.mpListener = listener;

    SystemEventListenerLinkInfo* item_ptr = (SystemEventListenerLinkInfo*)q_linear_search( 
                                                                           &mListenerQueue, 
                                                                             listenerInformationCompare,
                                                                               &compare_value );
    if( item_ptr != NULL )
    {
      q_delete( &mListenerQueue, (q_link_type*)item_ptr );

      /* put back in free queue */
      q_put( &mListenerFreeQueue, &item_ptr->mLink );
    }

} /* end SystemEventGenerator::removeListener() */

void SystemEventGenerator::AddListenerForTxWaitVote(ChargingEventListener *listener)
{
    /* Listen for TX WAIT ON */
    addListener(listener, CHG_SYSTEM__TX_ON);

    /* Listen to TX WAIT OFF */
    addListener(listener, CHG_SYSTEM__TX_OFF);
}

void SystemEventGenerator::RemoveListenerForTxWaitVote(ChargingEventListener *listener)
{
    /* TX WAIT ON */
    removeListener(listener, CHG_SYSTEM__TX_ON);

    /* TX WAIT OFF */
    removeListener(listener, CHG_SYSTEM__TX_OFF);
}

void SystemEventGenerator::AddListenerForFtmChargingSwitch(ChargingEventListener *listener)
{
    /* Listen for FTM CHARGING ON */
    addListener(listener, CHG_SYSTEM__FTM_SWITCH_ON_CHRGING);

    /* Listen to FTM CHARGING OFF */
    addListener(listener, CHG_SYSTEM__FTM_SWITCH_OFF_CHRGING);
}

void SystemEventGenerator::RemoveListenerForFtmChargingSwitch(ChargingEventListener *listener)
{
    /* FTM CHARGING ON */
    removeListener(listener, CHG_SYSTEM__FTM_SWITCH_ON_CHRGING);

    /* FTM CHARGING OFF */
    removeListener(listener, CHG_SYSTEM__FTM_SWITCH_OFF_CHRGING);
}

void SystemEventGenerator::AddListenerForAutoChargingSwitch(ChargingEventListener *listener)
{
    /* Listen for auto charging on */
    addListener(listener, CHG_SYSTEM__SWITCH_ON_AUTO_CHRGING);

    /* Listen to auto charging off */
    addListener(listener, CHG_SYSTEM__SWITCH_OFF_AUTO_CHRGING);
}

void SystemEventGenerator::RemoveListenerForAutoChargingSwitch(ChargingEventListener *listener)
{
    /* auto charging on */
    removeListener(listener, CHG_SYSTEM__SWITCH_ON_AUTO_CHRGING);

    /* auto charging off */
    removeListener(listener, CHG_SYSTEM__SWITCH_OFF_AUTO_CHRGING);
}

void SystemEventGenerator::AddListenerForUsbValid(ChargingEventListener *listener)
{
    /* Listen for trickle recovery on */
    addListener(listener, CHG_SYSTEM__USB_VALID);
}

void SystemEventGenerator::RemoveListenerForUsbValid(ChargingEventListener *listener)
{
    /* trickle recovery on*/
    removeListener(listener, CHG_SYSTEM__USB_VALID);
}

void SystemEventGenerator::AddListenerForUsbInvalid(ChargingEventListener *listener)
{
    /* Listen for trickle recovery on */
    addListener(listener, CHG_SYSTEM__USB_INVALID);
}

void SystemEventGenerator::RemoveListenerForUsbInvalid(ChargingEventListener *listener)
{
    /* trickle recovery on*/
    removeListener(listener, CHG_SYSTEM__USB_INVALID);
}

void SystemEventGenerator::AddListenerForUsbSuspend(ChargingEventListener *listener)
{
    /* Listen for trickle recovery on */
    addListener(listener, CHG_SYSTEM__USB_SUSPEND);
}

void SystemEventGenerator::RemoveListenerForUsbSuspend(ChargingEventListener *listener)
{
    /* trickle recovery on*/
    removeListener(listener, CHG_SYSTEM__USB_SUSPEND);
}

void SystemEventGenerator::TxWaitVote(chg_cmd_type cmd, void* data)
{
    /* Get voter ID */
    
    int intdata = reinterpret_cast<int>(data);
    chg_tx_wait_voter_type    voter = static_cast<chg_tx_wait_voter_type>(intdata);

    if(voter < CHG_TX_VOTE__INVALID)
    {
        switch(cmd)
        {
            case CHG_CMD_ENABLE:
            {
                chg_system_tx_informaton.tx_on = TRUE;

                /* Construct a charging event */
                INTLOCK();
                chg_task_evt_type                chg_task_event;
                chg_task_event.event_id          = CHG_EVT__SYSTEM;
                chg_task_event.system_event_id   = CHG_SYSTEM__TX_ON;
                chg_task_event.tx_voter_id       = voter;

                chg_push_event_on_command_queue(chg_task_event);
                INTFREE();
            }
            break;

            case CHG_CMD_DISABLE:
            {
                chg_system_tx_informaton.tx_on = FALSE;

                /* Construct a charging event */
                INTLOCK();
                chg_task_evt_type                chg_task_event;
                chg_task_event.event_id          = CHG_EVT__SYSTEM;
                chg_task_event.system_event_id   = CHG_SYSTEM__TX_OFF;
                chg_task_event.tx_voter_id       = voter;

                chg_push_event_on_command_queue(chg_task_event);
                INTFREE();
            }
            break;

            default:
            break;
        }/* switch() */
    } /* if(voter < CHG_TX_VOTE__INVALID) */
}

void SystemEventGenerator::FtmChargingSwitch(chg_cmd_type cmd)
{
    switch(cmd)
    {
        case CHG_CMD_ENABLE:
        {
            /* Construct a charging event */
            INTLOCK();
            chg_task_evt_type                chg_task_event;
            chg_task_event.event_id          = CHG_EVT__SYSTEM;
            chg_task_event.system_event_id   = CHG_SYSTEM__FTM_SWITCH_ON_CHRGING;
            chg_task_event.tx_voter_id       = CHG_TX_VOTE__INVALID;

            chg_push_event_on_command_queue(chg_task_event);
            INTFREE();
        }
        break;
        case CHG_CMD_DISABLE:
        {
            /* Construct a charging event */
            INTLOCK();
            chg_task_evt_type                chg_task_event;
            chg_task_event.event_id          = CHG_EVT__SYSTEM;
            chg_task_event.system_event_id   = CHG_SYSTEM__FTM_SWITCH_OFF_CHRGING;
            chg_task_event.tx_voter_id       = CHG_TX_VOTE__INVALID;

            chg_push_event_on_command_queue(chg_task_event);
            INTFREE();
        }
        break;
        default:
        //MSG_ERROR("CHG DEBUG: INVALID PARM:%d",cmd,0,0);
        break;
    } /* switch() */
}

void SystemEventGenerator::AutoChargingSwitch(chg_cmd_type cmd)
{
    switch(cmd)
    {
        case CHG_CMD_ENABLE:
        {
            /* Construct a charging event */
            INTLOCK();
            chg_task_evt_type                chg_task_event;
            chg_task_event.event_id          = CHG_EVT__SYSTEM;
            chg_task_event.system_event_id   = CHG_SYSTEM__SWITCH_ON_AUTO_CHRGING;
            chg_task_event.tx_voter_id       = CHG_TX_VOTE__INVALID;

            chg_push_event_on_command_queue(chg_task_event);
            INTFREE();
        }
        break;
        case CHG_CMD_DISABLE:
        {
            /* Construct a charging event */
            INTLOCK();
            chg_task_evt_type                chg_task_event;
            chg_task_event.event_id          = CHG_EVT__SYSTEM;
            chg_task_event.system_event_id   = CHG_SYSTEM__SWITCH_OFF_AUTO_CHRGING;
            chg_task_event.tx_voter_id       = CHG_TX_VOTE__INVALID;

            chg_push_event_on_command_queue(chg_task_event);
            INTFREE();
        }
        break;
        default:
        //MSG_ERROR("CHG DEBUG: INVALID PARM:%d",cmd,0,0);
        break;
    } /* switch() */
}

#endif /* FEATURE_CHG_TASK || T_WINNT */

