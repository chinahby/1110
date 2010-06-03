/*! \file 
 *  \n
 *  \brief  chg_isr.cpp ---- PMIC IRQ EVENT RELATED CLASS DEFINITION
 *  \details This header file contains class definitions of the IrqEvent class,
 *  and the IrqEventGenerator class.
 *  \n
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_isr.cpp#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/05/09   hs      Added support for software controlled CV topoff.
04/03/09   hs      Added support for unit tests.
03/26/09   hs      Removed support for software controlled charging.
12/01/08   hs      Modified addListener():if the listener with the IRQ ID has 
                   already been in the listener list, ignore it; otherwise, 
                   add the listener to the list.
11/20/08   hs      Added INTLOCK/INTFREE pairs in ISRs to guard the code.
11/13/08   hs      Added support for QSC1100.
09/27/08   vk      Added support for Battery removal case
09/26/08   vk      Resolved compiler warnings
06/25/08   hs      Added support for autonomous charging.
05/07/08   hs      Removed the usage of std.
                   Added FEATURE_CHG_TASK.
04/16/08   hs      Name change for some member functions.
07/12/07   hs      Created.

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "customer.h"                  /* FEATURE definitions    */

#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)

#include "chg_isr.h"
extern "C"
{
#include "rex.h"                   /* For rex_tcb_type                     */
}
#ifdef T_WINNT
#error code not present
#else
#include "assert.h"                /* For ASSERT()                         */
#endif
#include "chg_factory.h"           /* For class ChargingFactory            */
#include "msg.h"                   /* For MSG_ERROR                        */
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
                  /*     class ChargingIrqEvent      */
                  /***********************************/
IrqEvent::IrqEvent(pm_irq_hdl_type irqid )
: ChargingEvent( CHG_CMD__IRQ_TRIGGERED ),
    mIrqId(irqid)
{
}

IrqEvent::~IrqEvent()
{

}

                  /****************************************/
                  /*     class ChargerIrqEventGenerator   */
                  /****************************************/

IrqEventGenerator::IrqEventGenerator():
    mBatteryAlarmStatus(CHG_VBAT_TH_NONE)
{
    /* Initialize the listener queues */
    (void) q_init( &mListenerQueue );      /* Initialize listener queue */
    (void) q_init( &mListenerFreeQueue );  /* Initialize listener free queue */

    /* fill listener free queue with listener buffers */
    for (int i = 0; i < CHG_IRQ_LISTENER_BUF_CNT; i++ ) {
      q_put( &mListenerFreeQueue, 
             q_link( &mListenerBuffer[i], &mListenerBuffer[i].mLink) );
    }
}

IrqEventGenerator::~IrqEventGenerator()
{

}

IrqEventGenerator::IrqListenerLinkInfo* IrqEventGenerator::getListenerBuffer(void)
{
    IrqEventGenerator::IrqListenerLinkInfo    *buff_ptr;               /* Pointer to buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*-------------------------------------------------------------------------
      Get a listener buffer from the free listener queue.
      Currently it will already have the handle. If this changes in the future,
      assign a handle to the cmd.
    -------------------------------------------------------------------------*/
    if( (buff_ptr = (IrqEventGenerator::IrqListenerLinkInfo*)q_get( &mListenerFreeQueue )) == NULL )
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

int IrqEventGenerator::listenerInformationCompare(void* item, void* compare_val)
{
    ASSERT( NULL != item);
    ASSERT( NULL != compare_val);

    IrqListenerLinkInfo *item_ptr = (IrqListenerLinkInfo *)item;
    IrqListenerInfo *compare_ptr = (IrqListenerInfo *)compare_val;

    ASSERT( NULL != item_ptr);
    ASSERT( NULL != compare_ptr);

    if(item_ptr->mIrq == compare_ptr->mIrq &&
       item_ptr->mpListener == compare_ptr->mpListener)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void IrqEventGenerator::addListener( ChargingEventListener *listener,
                                     pm_irq_hdl_type       irqid )
{
    ASSERT(listener != NULL);

    IrqListenerInfo     compare_value;

    compare_value.mIrq = irqid;
    compare_value.mpListener = listener;

    IrqListenerLinkInfo* item_ptr = (IrqListenerLinkInfo*)q_linear_search( 
                                                            &mListenerQueue, 
                                                              listenerInformationCompare,
                                                               &compare_value );

    /* If the listener with the IRQ ID is ready in the instener list, ignore; 
       otherwise, add it to the list. */
    if( item_ptr == NULL )
    {
        IrqListenerLinkInfo *listenerinformation = getListenerBuffer();

        ASSERT(listenerinformation != NULL);

        listenerinformation->mpListener = listener;
        listenerinformation->mIrq       = irqid;

        /* Initialize link field */
        (void) q_link(listenerinformation, &listenerinformation->mLink );    
        /* Put on listener queue  */
        q_put( &mListenerQueue, &listenerinformation->mLink );        
    }

} /* end ChargerIrqEventGenerator::AddListener() */

void IrqEventGenerator::removeListener(
                                 ChargingEventListener *listener,
                                 pm_irq_hdl_type       irqid )
{
    IrqListenerInfo     compare_value;

    compare_value.mIrq = irqid;
    compare_value.mpListener = listener;

    IrqListenerLinkInfo* item_ptr = (IrqListenerLinkInfo*)q_linear_search( 
                                                            &mListenerQueue, 
                                                              listenerInformationCompare,
                                                               &compare_value );
    if( item_ptr != NULL )
    {
      q_delete( &mListenerQueue, (q_link_type*)item_ptr );

      /* put back in free queue */
      q_put( &mListenerFreeQueue, &item_ptr->mLink );
    }
} /* end ChargerIrqEventGenerator::RemoveListener() */

void IrqEventGenerator::AddListenerForValidChargerIRQ(ChargingEventListener *listener)
{
    /* Listen to Valid Charger IRQ */
    addListener(listener, PM_VALID_CHG_IRQ_HDL);

    /* Listen to Valid USB Charger IRQ */
    addListener(listener, PM_VALID_USB_CHG_IRQ_HDL);
}

void IrqEventGenerator::RemoveListenerForValidChargerIRQ(ChargingEventListener *listener)
{
    /* Valid Charger IRQ */
    removeListener(listener, PM_VALID_CHG_IRQ_HDL);

    /* Valid USB Charger IRQ */
    removeListener(listener, PM_VALID_USB_CHG_IRQ_HDL);
}

void IrqEventGenerator::AddListenerForInvalidChargerIRQ(ChargingEventListener *listener)
{
    /* Listen to Invalid Charger IRQ */
    addListener(listener, PM_INVALID_CHG_IRQ_HDL);

    /* Listen to Invalid USB Charger IRQ */
    addListener(listener, PM_INVALID_USB_CHG_IRQ_HDL);
}

void IrqEventGenerator::RemoveListenerForInvalidChargerIRQ(ChargingEventListener *listener)
{
    /* Invalid Charger IRQ */
    removeListener(listener, PM_INVALID_CHG_IRQ_HDL);

    /* Invalid USB Charger IRQ */
    removeListener(listener, PM_INVALID_USB_CHG_IRQ_HDL);
}

void IrqEventGenerator::FireEvent( ChargingEvent* event )
{
    IrqEvent* irqevent = dynamic_cast< IrqEvent* >( event );
    ASSERT( irqevent != NULL );

    pm_irq_hdl_type irqId = irqevent->GetIrqId();

    ChargingEventListener* listenerCopy[CHG_IRQ_LISTENER_BUF_CNT];
    pm_irq_hdl_type        irqCopy[CHG_IRQ_LISTENER_BUF_CNT];
    int index = 0;

    IrqListenerLinkInfo* item_ptr = (IrqListenerLinkInfo*)q_check(&mListenerQueue);

   /* Iterate the queue and make a copy */
   while(item_ptr != NULL)
   {
       listenerCopy[index] = item_ptr->mpListener;
       irqCopy[index++] = item_ptr->mIrq;

       item_ptr = (IrqListenerLinkInfo*)q_next(&mListenerQueue, &item_ptr->mLink);
   }

   listenerCopy[ index ] = NULL;

   /* Iterate through the listenerCopy */
   index = 0;

   while(listenerCopy[ index ] != NULL )
   {
       if( irqCopy[ index ] == irqId )
       {
           /* Fire event */
           listenerCopy[ index ]->EventFired(irqevent);
       }
       index++;
   }
}

void IrqEventGenerator::isr(void* data)
{ 
    /* Get IRQ ID */
    int intdata = reinterpret_cast<int>(data);
    pm_irq_hdl_type    irqid = static_cast<pm_irq_hdl_type>(intdata);

    if(irqid < PM_NUM_OF_IRQ_HDL) 
    {
        pm_err_flag_type  err     = PM_ERR_FLAG__SUCCESS;

        /* Disable the IRQ */
        err |= pm_clear_irq_multi_handle(irqid, isr); 

        /* Clear the IRQ. */
        err |= pm_clear_irq(irqid);

        /* Find out if there was an PMIC API error */
        if (err != PM_ERR_FLAG__SUCCESS) 
        {
           MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
        }

        /* Construct a charging event */
        INTLOCK();
        chg_task_evt_type       chg_task_event;
        chg_task_event.event_id = CHG_EVT__IRQ;
        chg_task_event.irq_id   = irqid;

        chg_push_event_on_command_queue( chg_task_event );
        INTFREE();
    } /* end If(irqid < PM_NUM_OF_IRQ_HDL) */
} /* end ChargerIrqEventGenerator::Isr() */

#if defined(FEATURE_CHG_TASK_USE_PULSE_TOPOFF) || defined(FEATURE_CHG_TASK_USE_CV_TOPOFF) || defined(T_WINNT)

pm_err_flag_type IrqEventGenerator::AddListenerForBatteryAlarmIrq(
                                                     ChargingEventListener *listener,
                                                     unsigned short        vmax,
                                                     unsigned short        vmin)
{
    pm_err_flag_type  err     = PM_ERR_FLAG__SUCCESS;

    /* Reset the vbat threshold variable. */
    mBatteryAlarmStatus = CHG_VBAT_TH_NONE;

    /* Add listener to the list */
    addListener(listener, PM_BAT_STAT_CHANGED_IRQ_HDL);

    INTLOCK();
    /* Disable the battery alarm IRQ. */
    err |=  pm_clear_irq_multi_handle(PM_BAT_STAT_CHANGED_IRQ_HDL, batteryAlarmIsr);   

    /* Disable the battery alarms. */
    err |= pm_chg_bat_alarm_switch(PM_OFF_CMD);

    /* Configure the PMIC to trigger the battery alarm if the battery
       Voltage goes above vmax or bellow vmin for more than 976 usecs */
    err |= pm_chg_bat_alarm_config(vmax, 
                                   vmin,
                                   PM_CHG_BATT_ALARM_HYST__976us); 

    /* Clear the battery alarm IRQ. */
    err |= pm_clear_irq(PM_BAT_STAT_CHANGED_IRQ_HDL);

    /* Enable the battery alarms. */
    err |= pm_chg_bat_alarm_switch(PM_ON_CMD);

    /* Enable the battery alarm IRQ. */
    err |= pm_set_irq_multi_handle(PM_BAT_STAT_CHANGED_IRQ_HDL,
                                   batteryAlarmIsr, 
                                   NULL);
    INTFREE();

    /* Find out if there was an PMIC API error */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
    }

    return err;
} 

pm_err_flag_type IrqEventGenerator::RemoveListenerForBatteryAlarmIrq(ChargingEventListener *listener)
{
    pm_err_flag_type  err     = PM_ERR_FLAG__SUCCESS;

     /* Disable the battery alarms. */
     err |= pm_chg_bat_alarm_switch(PM_OFF_CMD);

    /* Remove listener */
    removeListener(listener, PM_BAT_STAT_CHANGED_IRQ_HDL);

    /* Find out if there was an PMIC API error */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
    }

    return err;
}
void IrqEventGenerator::batteryAlarmIsr(void*)
{
    pm_err_flag_type  err       = PM_ERR_FLAG__SUCCESS;
    boolean           upper_thr = FALSE;
    boolean           lower_thr = FALSE;

    /* Clear the battery alarm IRQ. */
    err |= pm_clear_irq(PM_BAT_STAT_CHANGED_IRQ_HDL);

    /* Disable the battery alarm IRQ through masking. */
    err |=  pm_clear_irq_multi_handle(PM_BAT_STAT_CHANGED_IRQ_HDL, batteryAlarmIsr);   
  
    /* Get the battery alarm status. */
    err |= pm_chg_bat_alarm_read_status(&upper_thr,&lower_thr);

    /* Disable the battery alarms. */
    err |= pm_chg_bat_alarm_switch(PM_OFF_CMD);

    IrqEventGenerator* instance = ChargingFactory::GetIrqEventGenerator();

    if (upper_thr == TRUE) 
    {
        /* Battery voltage above threshold. */
        instance->mBatteryAlarmStatus = CHG_VBAT_TH_UPPER;

        /* Construct a charging event */
        INTLOCK();
        chg_task_evt_type       chg_task_event;
        chg_task_event.event_id = CHG_EVT__IRQ;
        chg_task_event.irq_id   = PM_BAT_STAT_CHANGED_IRQ_HDL;

        chg_push_event_on_command_queue( chg_task_event );
        INTFREE();
    }
    else if  (lower_thr == TRUE) 
    {
        /* Battery voltage bellow threshold. */
        instance->mBatteryAlarmStatus = CHG_VBAT_TH_LOWER;

        /* Construct a charging event */
        INTLOCK();
        chg_task_evt_type       chg_task_event;
        chg_task_event.event_id = CHG_EVT__IRQ;
        chg_task_event.irq_id   = PM_BAT_STAT_CHANGED_IRQ_HDL;

        chg_push_event_on_command_queue( chg_task_event );
        INTFREE();
    } 
    else
    {
        /* Battery within thresholds, probably an invalid IRQ. */
        instance->mBatteryAlarmStatus = CHG_VBAT_TH_NONE;

        /* Re-enable the battery alarm IRQ through unmasking. */
        err |= pm_set_irq_multi_handle(PM_BAT_STAT_CHANGED_IRQ_HDL, 
                                       batteryAlarmIsr, 
                                       NULL);
        
        MSG_ERROR("CHG DEBUG: False Battery Threshold Alarm.",0,0,0);
    }

    /* Find out if there was an PMIC API error */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
    }
}
#endif /* FEATURE_CHG_TASK_USE_PULSE_TOPOFF || FEATURE_CHG_TASK_USE_CV_TOPOFF */

#if defined( FEATURE_CHG_TASK_USE_PULSE_TOPOFF) || defined(T_WINNT)

pm_err_flag_type IrqEventGenerator::AddListenerForPulseTDoneIrq(ChargingEventListener *listener)
{
    /* Listen to pulse charging TDone IRQ */
    addListener(listener, PM_PULSE_CHG_DONE_IRQ_HDL);

    pm_err_flag_type  err     = PM_ERR_FLAG__SUCCESS;

    INTLOCK();

    /* Disable the IRQ */
    err |=  pm_clear_irq_multi_handle(PM_PULSE_CHG_DONE_IRQ_HDL, isr);   

    /* Clear the IRQ. */
    err |= pm_clear_irq(PM_PULSE_CHG_DONE_IRQ_HDL);

    /* Enable the IRQ, pass the IRQ ID as data */
    err |= pm_set_irq_multi_handle(PM_PULSE_CHG_DONE_IRQ_HDL, 
                                   isr, 
                                   (void*)PM_PULSE_CHG_DONE_IRQ_HDL);

    /* Find out if there was an PMIC API error */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
    }

    INTFREE();

    return err;
}

void IrqEventGenerator::RemoveListenerForPulseTDoneIrq(ChargingEventListener *listener)
{
    /* Remove listener */
    removeListener(listener, PM_PULSE_CHG_DONE_IRQ_HDL);
}

#endif /* FEATURE_CHG_TASK_USE_PULSE_TOPOFF */

pm_err_flag_type IrqEventGenerator::AddListenerForEndOfChargingIrq(ChargingEventListener *listener)
{
    /* Listen to end-of-charging IRQ */
    addListener(listener, PM_CHGDONE_IRQ_HDL);

    pm_err_flag_type  err     = PM_ERR_FLAG__SUCCESS;

    INTLOCK();

    /* Disable the IRQ */
    err |=  pm_clear_irq_multi_handle(PM_CHGDONE_IRQ_HDL, isr);   

    /* Clear the IRQ. */
    err |= pm_clear_irq(PM_CHGDONE_IRQ_HDL);

    /* Enable the IRQ, pass the IRQ ID as data */
    err |= pm_set_irq_multi_handle(PM_CHGDONE_IRQ_HDL, 
                             isr, 
                             (void*)PM_CHGDONE_IRQ_HDL);

    /* Find out if there was an PMIC API error */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
    }

    INTFREE();

    return err;
}

void IrqEventGenerator::RemoveListenerForEndOfChargingIrq(ChargingEventListener *listener)
{
    /* Remove listener */
    removeListener(listener, PM_CHGDONE_IRQ_HDL);
}

pm_err_flag_type IrqEventGenerator::AddListenerForFailOfChargingIrq(ChargingEventListener *listener)
{
    /* Listen to fail-of-charging IRQ */
    addListener(listener, PM_CHGFAIL_IRQ_HDL);

    pm_err_flag_type  err     = PM_ERR_FLAG__SUCCESS;

    INTLOCK();

    /* Disable the IRQ */
    err |=  pm_clear_irq_multi_handle(PM_CHGFAIL_IRQ_HDL, isr);   

    /* Clear the IRQ. */
    err |= pm_clear_irq(PM_CHGFAIL_IRQ_HDL);

    /* Enable the IRQ, pass the IRQ ID as data */
    err |= pm_set_irq_multi_handle(PM_CHGFAIL_IRQ_HDL, 
                             isr, 
                             (void*)PM_CHGFAIL_IRQ_HDL);

    /* Find out if there was an PMIC API error */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
    }

    INTFREE();

    return err;
}

void IrqEventGenerator::RemoveListenerForFailOfChargingIrq(ChargingEventListener *listener)
{
    /* Remove listener */
    removeListener(listener, PM_CHGFAIL_IRQ_HDL);
}

pm_err_flag_type IrqEventGenerator::AddListenerForChangeOfStateIrq(ChargingEventListener *listener)
{
    /* Listen to change-of-state IRQ */
    addListener(listener, PM_CHGSTATE_IRQ_HDL);

    pm_err_flag_type  err     = PM_ERR_FLAG__SUCCESS;

    INTLOCK();

    /* Disable the IRQ */
    err |=  pm_clear_irq_multi_handle(PM_CHGSTATE_IRQ_HDL, isr);   

    /* Clear the IRQ. */
    err |= pm_clear_irq(PM_CHGSTATE_IRQ_HDL);

    /* Enable the IRQ, pass the IRQ ID as data */
    err |= pm_set_irq_multi_handle(PM_CHGSTATE_IRQ_HDL, 
                             isr, 
                             (void*)PM_CHGSTATE_IRQ_HDL);

    /* Find out if there was an PMIC API error */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
    }

    INTFREE();

    return err;
}

void IrqEventGenerator::RemoveListenerForChangeOfStateIrq(ChargingEventListener *listener)
{
    /* Remove listener */
    removeListener(listener, PM_CHGSTATE_IRQ_HDL);
}

pm_err_flag_type IrqEventGenerator::AddListenerForBattTempIrq(ChargingEventListener *listener)
{
    /* Listen to change-of-batt-temp IRQ */
    addListener(listener, PM_BATTTEMP_IRQ_HDL);

    pm_err_flag_type  err     = PM_ERR_FLAG__SUCCESS;

    INTLOCK();

    /* Disable the IRQ */
    err |=  pm_clear_irq_multi_handle(PM_BATTTEMP_IRQ_HDL, isr);   

    /* Clear the IRQ. */
    err |= pm_clear_irq(PM_BATTTEMP_IRQ_HDL);

    /* Enable the IRQ, pass the IRQ ID as data */
    err |= pm_set_irq_multi_handle(PM_BATTTEMP_IRQ_HDL, 
                             isr, 
                             (void*)PM_BATTTEMP_IRQ_HDL);

    /* Find out if there was an PMIC API error */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
    }

    INTFREE();

    return err;
}

void IrqEventGenerator::RemoveListenerForBattTempIrq(ChargingEventListener *listener)
{
    /* Remove listener */
    removeListener(listener, PM_BATTTEMP_IRQ_HDL);
}

pm_err_flag_type IrqEventGenerator::AddListenerForVbatdetIrq(ChargingEventListener *listener)
{
    /* Listen to VBATDET IRQ */
    addListener(listener, PM_VBAT_DET_IRQ_HDL);

    pm_err_flag_type  err     = PM_ERR_FLAG__SUCCESS;

    INTLOCK();

    /* Disable the IRQ */
    err |=  pm_clear_irq_multi_handle(PM_VBAT_DET_IRQ_HDL, isr);   

    /* Clear the IRQ. */
    err |= pm_clear_irq(PM_VBAT_DET_IRQ_HDL);

    /* Enable the IRQ, pass the IRQ ID as data */
    err |= pm_set_irq_multi_handle(PM_VBAT_DET_IRQ_HDL, 
                             isr, 
                             (void*)PM_VBAT_DET_IRQ_HDL);

    /* Find out if there was an PMIC API error */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
    }

    INTFREE();

    return err;
}

void IrqEventGenerator::RemoveListenerForVbatdetIrq(ChargingEventListener *listener)
{
    /* Remove listener */
    removeListener(listener, PM_VBAT_DET_IRQ_HDL);
}

pm_err_flag_type IrqEventGenerator::AddListenerForFastChargeIrq(ChargingEventListener *listener)
{
    /* Listen to in-fast-charging IRQ */
    addListener(listener, PM_FASTCHG_IRQ_HDL);

    pm_err_flag_type  err     = PM_ERR_FLAG__SUCCESS;

    INTLOCK();

    /* Disable the IRQ */
    err |=  pm_clear_irq_multi_handle(PM_FASTCHG_IRQ_HDL, isr);   

    /* Clear the IRQ. */
    err |= pm_clear_irq(PM_FASTCHG_IRQ_HDL);

    /* Enable the IRQ, pass the IRQ ID as data */
    err |= pm_set_irq_multi_handle(PM_FASTCHG_IRQ_HDL, 
                             isr, 
                             (void*)PM_FASTCHG_IRQ_HDL);

    /* Find out if there was an PMIC API error */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
    }

    INTFREE();

    return err;
}

void IrqEventGenerator::RemoveListenerForFastChargeIrq(ChargingEventListener *listener)
{
    /* Remove listener */
    removeListener(listener, PM_FASTCHG_IRQ_HDL);
}

pm_err_flag_type IrqEventGenerator::AddListenerForBattConnectIrq(ChargingEventListener *listener)
{
    /* Listen to Battery connect IRQ */
    addListener(listener, PM_BATTCONNECT_IRQ_HDL);

    pm_err_flag_type  err     = PM_ERR_FLAG__SUCCESS;

    INTLOCK();

    /* Disable the IRQ */
    err |=  pm_clear_irq_multi_handle(PM_BATTCONNECT_IRQ_HDL, isr);   

    /* Clear the IRQ. */
    err |= pm_clear_irq(PM_BATTCONNECT_IRQ_HDL);

    /* Enable the IRQ, pass the IRQ ID as data */
    err |= pm_set_irq_multi_handle(PM_BATTCONNECT_IRQ_HDL, 
                             isr, 
                             (void*)PM_BATTCONNECT_IRQ_HDL);

    /* Find out if there was an PMIC API error */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
    }

    INTFREE();

    return err;
}

void IrqEventGenerator::RemoveListenerForBattConnectIrq(ChargingEventListener *listener)
{
    /* Remove listener */
    removeListener(listener, PM_BATTCONNECT_IRQ_HDL);
}

#if defined(FEATURE_CHG_TASK_USE_CV_TOPOFF) || defined(T_WINNT)
pm_err_flag_type IrqEventGenerator::AddListenerForVcpIrq(ChargingEventListener *listener)
{
    /* Listen to Battery connect IRQ */
    addListener(listener, PM_VCP_IRQ_HDL);

    pm_err_flag_type  err     = PM_ERR_FLAG__SUCCESS;

    INTLOCK();

    /* Disable the IRQ */
    err |=  pm_clear_irq_multi_handle(PM_VCP_IRQ_HDL, isr);   

    /* Clear the IRQ. */
    err |= pm_clear_irq(PM_VCP_IRQ_HDL);

    /* Enable the IRQ, pass the IRQ ID as data */
    err |= pm_set_irq_multi_handle(PM_VCP_IRQ_HDL, 
                                   isr, 
                                   (void*)PM_VCP_IRQ_HDL);

    /* Find out if there was an PMIC API error */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
    }

    INTFREE();

    return err;
}

void IrqEventGenerator::RemoveListenerForVcpIrq(ChargingEventListener *listener)
{
    /* Remove listener */
    removeListener(listener, PM_VCP_IRQ_HDL);
}

pm_err_flag_type IrqEventGenerator::AddListenerForChgTLimitIrq(ChargingEventListener *listener)
{
    /* Listen to Battery connect IRQ */
    addListener(listener, PM_CHGTLIMIT_IRQ_HDL);

    pm_err_flag_type  err     = PM_ERR_FLAG__SUCCESS;

    INTLOCK();

    /* Disable the IRQ */
    err |=  pm_clear_irq_multi_handle(PM_CHGTLIMIT_IRQ_HDL, isr);   

    /* Clear the IRQ. */
    err |= pm_clear_irq(PM_CHGTLIMIT_IRQ_HDL);

    /* Enable the IRQ, pass the IRQ ID as data */
    err |= pm_set_irq_multi_handle(PM_CHGTLIMIT_IRQ_HDL, 
                                   isr, 
                                   (void*)PM_CHGTLIMIT_IRQ_HDL);

    /* Enable VCP (voltage collapse protection). */
    err |=  pm_chg_vcp_switch(PM_ON_CMD);

    /* Find out if there was an PMIC API error */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
    }

    INTFREE();

    return err;
}

void IrqEventGenerator::RemoveListenerForChgTLimitIrq(ChargingEventListener *listener)
{
    /* Remove listener */
    removeListener(listener, PM_CHGTLIMIT_IRQ_HDL);
}
#endif /* FEATURE_CHG_TASK_USE_CV_TOPOFF */

#endif /* FEATURE_CHG_TASK || T_WINNT */

