/*! \file 
 *  \n
 *  \brief  chg_task.cpp ---- CHARGER TASK RELATED DEFINITION
 *  \n
 *  \n This header file contains class declarations of the Battery class
 *  and the BatteryDebug class.
 *  \n
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_task.cpp#14 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/09   hs      Treated the USB valid/invalid events as system events.
07/30/09   hs      Changed the global variable chg_is_task_initialized to local.
06/60/09   hs      Integrated the vbatt task interface for the battery alarm
                   status.
06/03/09   hs      Replaced chg_enable_debugger with CHG_ENABLE_DEBUGGER
04/28/09   hs      Added a new API that returns the status of the initialization
                   of the charger task.
04/03/09   hs      Added support for unit tests.
03/31/09   hs      Removed CHG_CMD_BUF_CNT for the size charger task command buffer.
03/26/09   hs      Removed support for software controlled charging.
03/24/09   hs      Modified chg_usb_i_is_available().
12/03/08   hs      Added support for "USB detection through VCHG".
11/20/08   hs      Added INTLOCK/INTFREE pairs to guard the code.
                   Replaced !FEATURE_HS_USB with FEATUER_HS_USB_PMIC_PHY.
11/11/08   hs      Modified the way to create the charging event
                   in interrupt context.
10/07/08   vk      Added support for USB suspend state
09/05/08   vk      Featurized FS-USB specific code
09/02/08   vk      Enabled USB charging based on NV
06/25/08   hs      Added support for autonomous charging.
06/12/08   hs      Added FEATURE_CHG_TASK.
04/18/08   hs      Initialize SwapStates in chg_task_init()
                   Included "chg_config.h"
07/12/07   hs      Created.

========================================================================== */

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"         /* FEATURE definitions                       */

#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)

extern "C"
{
#ifdef T_WINNT
#error code not present
#endif

#include "tmc.h"
#include "dog.h"
#include "nv.h"               /* For NV related definitions                */
#include "charger.h"          /* For chg_tx_wait_vote_info_type            */
} /* extern "C" */

#include "chg_task.h"
#include "chg_factory.h"      /* For class ChargingFactory                 */
#include "chg_timer.h"        /* class ChargerTimerEvent                   */
#include "chg_isr.h"          /* class ChargerIrqEvent                     */
#include "chg_system.h"       /* class ChargerSystemEvent                  */
#include "chg_config.h"       /* For various type definitions              */
#include "chg_autocharging.h" /* For AutoChargingManager                   */
#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */
#include "msg.h"              /* For MSG_ERROR                             */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*===========================================================================

                               MACROS

===========================================================================*/
/* Macro to kick the watchdog timer and set a timer to wake it again.      */

#define CHG_KICK_WATCHDOG() \
   dog_report(DOG_CHG_RPT); \
   (void) rex_set_timer(&chg_rpt_timer, DOG_CHG_RPT_TIME);


/*===========================================================================

                         TYPE AND CONSTANT DEFINITIONS

===========================================================================*/
/*===========================================================================

                            GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                         LOCAL FUNCTION PROTOTYPES

===========================================================================*/
static void chg_task_init(void);
static void chg_irq_init(void);
static chg_task_cmd_type* chg_get_command_buffer(void);
static void chg_put_command(chg_task_cmd_type *cmd_ptr);
static void chg_process_command(void);
static void chg_update_usb_OnOff_nv_item_val();
static void chg_wall_valid_isr(void* data);
static void chg_wall_invalid_isr(void* data);
#ifndef T_WINNT
static void chg_usb_convert_and_set_usb_imaxsel(uint32 isetting);
#endif
static void chg_wall_irq_switch(chg_cmd_type cmd);
/*===========================================================================

                            LOCAL VARIABLES

===========================================================================*/
/*! Timers used for watchdog reporting and generic purposes. */
static rex_timer_type          chg_rpt_timer;      /* Timer for the test task       */
/*! timer used for ftm_rex_wait */
static rex_timer_type          chg_wait_timer;
/*! Command buffer. */
static chg_task_cmd_type       chg_command_buffer[ CHG_CMD_BUF_CNT ];
/*! Queue for commands from other tasks/modules */
static q_type                  chg_command_queue; 
/*! Queue for storing free command buffers */
static q_type                  chg_command_free_queue;
/*! NV command. */
static nv_cmd_type             chg_nv_cmd;
/*! NV data buffer. */
static nv_item_type            chg_nv_data_buf; 
/*!  Used to save the status of the charger task initialization.  */
/* When it's TRUE, the charger task has been initialized.         */
/* When it's FALSE, this charger task has not been initialized.   */
static boolean                 chg_is_task_initialized  = FALSE;
/*===========================================================================

                           FUNCTION DEFINITIONS

===========================================================================*/

void  chg_task
(
  dword                         /* from REX - ignored                     */
)
{
    rex_sigs_type      sigs;             /* REX signals word              */

/*-------------------------------------------------------------------------*/
    /* Initialize chg task */
    chg_task_init();

#ifdef T_WINNT
#error code not present
#endif
    /* Check if the system TX was turned on */
    if(chg_system_tx_informaton.tx_on == TRUE)
    {
        INTLOCK();
        /* Construct a charging event */
        chg_task_evt_type                chg_task_event;
        chg_task_event.event_id          = CHG_EVT__SYSTEM;
        chg_task_event.system_event_id   = CHG_SYSTEM__TX_ON;
        chg_task_event.tx_voter_id       = chg_system_tx_informaton.voter_id;

        /* Fire Event, let charger task know */
        chg_push_event_on_command_queue(chg_task_event);
        INTFREE();
    }
#ifndef T_WINNT
    /* ------------------------------------------------------------------
    ** Initially kick watchdog and set timer for watchdog report interval
    ** ------------------------------------------------------------------ */
    CHG_KICK_WATCHDOG(); 
#endif
    /* Loop forever handling commands */
    for (;;)
    {
        sigs = rex_wait( CHG_RPT_TIMER_SIG    |     /* Watchdog report timer */
                         TASK_STOP_SIG        |     /* Command to shutdown   */
                         CHG_CMD_Q_SIG        |     /* Command Queue signal  */
                         TMC_NV_CMD_SIG             /* NV signal    */
                       );

        if ( sigs & CHG_RPT_TIMER_SIG )
        {
            #ifndef T_WINNT
            /* -----------------------------
            ** Kick watchdog and reset timer
            ** ----------------------------- */
            CHG_KICK_WATCHDOG();
            #endif
        }

        /* -------------------------------------------------------
        ** Check if powerdown command signal was set.  If set then
        ** clear signal, and process task stop procedure.
        ** ------------------------------------------------------- */
        if ( sigs & TASK_STOP_SIG )
        {
            (void) rex_set_sigs(&tmc_tcb, TMC_ACK_SIG); /* Acknowledge were done */
            (void) rex_clr_sigs(&chg_tcb, TASK_STOP_SIG);

            /* Release all the memory that is used in Charger application */
            ChargingFactory::DestroyAll();

            /* Update charger task status */
            chg_is_task_initialized = FALSE;
        }

        #ifndef T_WINNT

        /* -------------------------------------------------------
        ** Check if NV command signal was set.  If set then
        ** clear signal, and process reading NV item procedure.
        ** ------------------------------------------------------- */
        if( sigs & TMC_NV_CMD_SIG )
        {
            (void)rex_clr_sigs(&chg_tcb, TMC_NV_CMD_SIG); 

            /* Update USB On Off NV item value */
            chg_update_usb_OnOff_nv_item_val();

            /* Initialize/Enable wall and/or USB charger interrupts. */
            chg_irq_init();

            /* Update the charger task status */
            chg_is_task_initialized = TRUE;

        } /* end  if( sigs & TMC_NV_CMD_SIG ) */

        #endif /* T_WINNT */

        /* -----------------------------------------------------
        ** Check if any command signal was set.  If set then
        ** clear signal, and process the corresponding procedure.
        ** ----------------------------------------------------- */
        if ( sigs & CHG_CMD_Q_SIG )
        {
            /* Clear the signal prior to emptying the queue, to avoid the
            ** race condition where an item is added just as we are leaving.
            */

            (void) rex_clr_sigs( &chg_tcb, CHG_CMD_Q_SIG);

            chg_process_command();
        #ifdef T_WINNT
#error code not present
        #endif

        }/* end if ( sigs & CHG_CMD_Q_SIG) */

    } /* for (;;) */
} /* chg_task */

void  chg_task_init( void )
{
    /* Initialize timers */
    rex_def_timer( &chg_wait_timer, rex_self(), CHG_CLK_WAIT_SIG ); 

    /*
    ** Wait for the start signal from the Task Main Controller
    */
    tmc_task_start();

    (void) rex_clr_sigs( rex_self(), TASK_START_SIG );

    /* Initialize timers - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    rex_def_timer(&chg_rpt_timer, &chg_tcb, CHG_RPT_TIMER_SIG); /* Dog Report */

    /* Initialize queues - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    (void) q_init( &chg_command_queue );      /* Initialize command queue */
    (void) q_init( &chg_command_free_queue ); /* Initialize command free queue */
    /*------------------------------------------------------------------------
       fill command free queue with command buffers 
    ------------------------------------------------------------------------*/
    for (int i = 0; i < CHG_CMD_BUF_CNT; i++ ) {
      q_put( &chg_command_free_queue, 
             q_link( &chg_command_buffer[i], &chg_command_buffer[i].hdr.link ) );
    }

     /* Initialize NV command and post this commnand to NV task */
    chg_nv_cmd.cmd        = NV_READ_F;       /* NV operation.                */
    chg_nv_cmd.data_ptr   = &chg_nv_data_buf;/* buffer to store read data.   */
    chg_nv_cmd.done_q_ptr = NULL;            /* Return cmd to NO Q when done.*/
                                             /* NV ITEM. */
    chg_nv_cmd.item       = NV_CHG_USB_NVDISABLE_VAL_I;  
    chg_nv_cmd.sigs       = TMC_NV_CMD_SIG;   /* Signal with this sig when 
                                                done. */
    chg_nv_cmd.tcb_ptr    = &chg_tcb;         /* Notify this task when done   */
    /* Command the NV task to process the command */
    nv_cmd( &chg_nv_cmd );

    /* If VBATT task is used in the system, register for battery threashold 
       notification for CV (Constant Voltage) or pulse charging topoff */
    #ifdef FEATURE_VBATT_TASK
      #if defined(FEATURE_CHG_TASK_USE_PULSE_TOPOFF) || defined(FEATURE_CHG_TASK_USE_CV_TOPOFF)
      {
          vbatt_error_type     vbatt_err = VBATT_REGISTRATION_SUCCESSFUL;

          chg_handle_lwr =  pm_vbatt_register (PM_VBATT_COMPARATOR_MIN , 
                                                VBATT_VOLTAGE_UP ,  
                                                 chg_vbatt_lwr_thr_cb , 
                                                  NULL ,  
                                                   &vbatt_err ) ; 

          chg_handle_upr =  pm_vbatt_register (PM_VBATT_COMPARATOR_MAX , 
                                                VBATT_VOLTAGE_DOWN , 
                                                 chg_vbatt_upr_thr_cb , 
                                                  NULL , 
                                                   &vbatt_err ) ; 

          ASSERT(VBATT_REGISTRATION_SUCCESSFUL == vbatt_err );
      }

      #endif /* FEATURE_CHG_TASK_USE_CV_TOPOFF || FEATURE_CHG_TASK_USE_PULSE_TOPOFF */
    #endif /* FEATURE_VBATT_TASK */

     /* Initialize auto charging manager */
    (void)ChargingFactory::GetAutoChargingManager();

    /* Initialize Charging Debugger */
    #if CHG_ENABLE_DEBUGGER
        (void)ChargingFactory::GetChargingDebugger();
    #endif /* CHG_ENABLE_DEBUGGER */
} /* End chg_task_init() */

chg_task_cmd_type* chg_get_command_buffer(void)
{
    chg_task_cmd_type    *cmd_ptr;                     /* Pointer to command */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*-------------------------------------------------------------------------
      Get a command buffer from the free command queue.
      Currently it will already have the handle. If this changes in the future,
      assign a handle to the cmd.
    -------------------------------------------------------------------------*/
    if( (cmd_ptr = (chg_task_cmd_type*)q_get( &chg_command_free_queue )) == NULL )
    {
      /*-----------------------------------------------------------------------
        No free command buffers available, log an error.
      -----------------------------------------------------------------------*/
      ASSERT(false);
    }

    /*-------------------------------------------------------------------------
      Note that the cmd_ptr may be NULL if there were no free command buffers
      available. The calling task must either handle the NULL return value
      or ERR_FATAL.
    -------------------------------------------------------------------------*/
    return( cmd_ptr );

} /* End chg_get_command_buffer() */

void chg_put_command
(
   chg_task_cmd_type *cmd_ptr   
   /* pointer to a handset command.  When the command has finished, the
   ** command structure is put on the "done queue".  If there is no "done
   ** queue", or this operation is not desirable, set the "done queue"
   ** pointer to NULL.  Also when the command has completed, the specified
   ** task is signaled with the sig parameter.  If no signaling is required,
   ** the task pointer parameter can be set to NULL.
   */
)
{
    ASSERT( cmd_ptr != NULL );

    (void) q_link(cmd_ptr, &cmd_ptr->hdr.link );    /* Initialize link field */

    q_put( &chg_command_queue, &cmd_ptr->hdr.link );        /* Put on command queue  */

    (void) rex_set_sigs( &chg_tcb, CHG_CMD_Q_SIG ); /* Signal a queue event  */
} /* End chg_put_command()*/

void chg_push_event_on_command_queue( chg_task_evt_type  event )
{
    INTLOCK();

    chg_task_cmd_type *chg_task_command = chg_get_command_buffer(); 

    switch( event.event_id )
    {
        case CHG_EVT__TIMER :
        {
            /* Update command ID */
            chg_task_command->cmd_id = CHG_CMD__TIMER_EXPIRED;
            chg_task_command->timer_token = event.timer_token;

            /* Send a command to charger task command queue */
            chg_put_command( chg_task_command );
        }
        break;
        case CHG_EVT__IRQ :
        {
            /* Update command ID */
            chg_task_command->cmd_id = CHG_CMD__IRQ_TRIGGERED;
            chg_task_command->irq_id = event.irq_id;

            /* Send a command to charger task command queue */
            chg_put_command( chg_task_command );
        }
        break;
        case CHG_EVT__SYSTEM:
        {
            /* Update command ID */
            chg_task_command->cmd_id = CHG_CMD__SYSTEM_STATUS_CHANGED;
            chg_task_command->system_event_id = event.system_event_id;
            chg_task_command->tx_voter_id = event.tx_voter_id;

            /* Send a command to charger task command queue */
            chg_put_command( chg_task_command );
        }
        break;
        default :
            chg_task_command->cmd_id = CHG_CMD__INVALID;
    } /* end switch( event->GetEventId() ) */

    INTFREE();
} /* end chg_push_event_on_command_queue() */

void chg_process_command(void)
{
    chg_task_cmd_type *cmd_ptr; /* Pointer to the received command */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*-----------------------------------------------------------------------
    Get commands from the command queue until the queue is empty. For each
    command received, dispatch the command to the appropriate sub-task.
    ------------------------------------------------------------------------*/
    while( (cmd_ptr = (chg_task_cmd_type *)q_get( &chg_command_queue )) != NULL )
    {

        switch( cmd_ptr->cmd_id )
        {
            case CHG_CMD__TIMER_EXPIRED:
            {
                /* Fire timer event */
                TimerEvent timerEvent(cmd_ptr->timer_token);
                TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
                timerEventGenerator->FireEvent( &timerEvent );
            }
            break;
            case CHG_CMD__IRQ_TRIGGERED:
            {
                /* Fire IRQ event */
                IrqEvent  irqEvent(cmd_ptr->irq_id);
                IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
                irqEventGenerator->FireEvent( &irqEvent );
            }
            break;
            case CHG_CMD__SYSTEM_STATUS_CHANGED:
            {
                /* Fire system event */
                SystemEvent systemEvent(cmd_ptr->system_event_id, cmd_ptr->tx_voter_id);
                SystemEventGenerator *systemEventGenerator = ChargingFactory::GetSystemEventGenerator();
                systemEventGenerator->FireEvent( &systemEvent );
            }
            break;
            default:
                /*lint -save -e506, -e774 */
                ASSERT(0);
                /*lint -restore */
                MSG_ERROR("ChargerTask: Ignoring unknown command %d handle",
                          cmd_ptr->cmd_id, 0, 0);
            break;
        } /* end switch(cmd_ptr->cmd_id)*/

        /*-----------------------------------------------------------------------
          Free the command and place the commnd back to command free queue.

        -----------------------------------------------------------------------*/
        /* return buffer to free queue */
        q_put( &chg_command_free_queue, &cmd_ptr->hdr.link );
    } /* end while(...)*/
} /* end chg_process_command() */


void chg_irq_init(void)
{
    if (chg_usb_OnOff_nv_item_val == CHG_CMD_ENABLE)
    {
        /* Enable USB charger IRQ. */
        chg_usb_irq_switch(CHG_CMD_ENABLE);

        /* Disable Wall charger IRQ */
        chg_wall_irq_switch(CHG_CMD_DISABLE);
    }
    else
    {
        /* Enable Wall charger IRQ */
        chg_wall_irq_switch(CHG_CMD_ENABLE);

        /* Disable USB charger IRQ. */
        chg_usb_irq_switch(CHG_CMD_DISABLE);

    }

} /* end chg_irq_init() */

void chg_wall_irq_switch(chg_cmd_type cmd)
{
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

    /* Reset/Clear the Wall Charger valid IRQ. */
    err |=  pm_clear_irq(PM_VALID_CHG_IRQ_HDL);

    /* Disable the Wall Charger valid IRQ. */
    err |= pm_clear_irq_multi_handle(PM_VALID_CHG_IRQ_HDL, chg_wall_valid_isr);      

    if( cmd == CHG_CMD_ENABLE )
    {
        /* Enable the Wall Charger valid IRQ. */
        err |= pm_set_irq_multi_handle(PM_VALID_CHG_IRQ_HDL, 
                                       chg_wall_valid_isr, 
                                       NULL);  
    }

    /* Find out if there was an PMIC API error. */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
    }
} /* end chg_wall_irq_switch() */


void chg_usb_irq_switch(chg_cmd_type cmd)
{   
    switch(cmd)
    {
    case CHG_CMD_DISABLE:
            #ifdef FEATURE_HS_USB_PMIC_PHY
            /* Disable the ability for the PMIC USB Transceiver driver 
               to tells us if we are allowed to charger and if so, how much
               current are we allow to draw. */
            pm_app_otg_configure_usb_power_consumer(NULL,NULL,NULL,NULL,NULL);

            pm_app_otg_set_usb_power_consumption_requirement(FALSE);
            #endif
            /* Make sure that we inform the state machine that USB
               charging is not valid. */
            chg_usb_i_is_not_available();
            break;

        case CHG_CMD_ENABLE:
            #ifdef FEATURE_HS_USB_PMIC_PHY
            /* Have the PMIC USB Transceiver driver tells us if
               we are allowed to charger and if so, how much
               current are we allow to draw. */
            pm_app_otg_configure_usb_power_consumer(            
                                          chg_usb_charger_connected,   
                                          chg_usb_charger_disconnected,
                                          chg_usb_i_is_available,      
                                          chg_usb_i_is_not_available,
                                          chg_usb_suspended );

            pm_app_otg_set_usb_power_consumption_requirement(TRUE);
            #endif
            break;
        
        default:
            MSG_ERROR("CHARGER ERROR: BAD PARAMETER = %d",cmd,0,0);
    } /* end switch() */
 } /* end chg_usb_irq_switch() */

void chg_usb_i_is_available(uint32 i_ma)
{   
    if(chg_usb_OnOff_nv_item_val == CHG_CMD_ENABLE)
    { 
        /* PMIC doesn't support IMAXSEL setting lower than 50mA; 
           so if i_ma is lower, we need to treat the USB device as suspend */
        if(i_ma >= AUTO_CHARGING_IMAX_MINIMUM)
        {
            /* Convert (PMIC API format), save and set (writes to the 
               PMIC) the maximum amount of current that we are allowed to
               draw from the USB device.*/
            chg_usb_convert_and_set_usb_imaxsel(i_ma); 

            /* Construct a charging event */
            INTLOCK();
            chg_task_evt_type      chg_task_event;
            chg_task_event.event_id = CHG_EVT__SYSTEM;
            chg_task_event.system_event_id   = CHG_SYSTEM__USB_VALID;

            chg_push_event_on_command_queue( chg_task_event );
            INTFREE();
        }
        else
        {
            /* Construct a charging event */
            INTLOCK();
            chg_task_evt_type      chg_task_event;
            chg_task_event.event_id = CHG_EVT__SYSTEM;
            chg_task_event.system_event_id  = CHG_SYSTEM__USB_SUSPEND;
            chg_push_event_on_command_queue( chg_task_event );
            INTFREE();

        }
    } /* end if(chg_usb_OnOff_nv_item_val == CHG_CMD_ENABLE) */
}/* chg_usb_i_is_available */

void chg_usb_i_is_not_available(void)
{
    if(chg_usb_OnOff_nv_item_val == CHG_CMD_ENABLE)
    {
        /* Convert (PMIC API format), save and set (writes to the 
           PMIC) the maximum amount of current that we are allowed to
           draw from the USB device.*/
        chg_usb_convert_and_set_usb_imaxsel(0);

        /* Construct a charging event */
        INTLOCK();
        chg_task_evt_type      chg_task_event;
        chg_task_event.event_id = CHG_EVT__SYSTEM;
        chg_task_event.system_event_id   = CHG_SYSTEM__USB_INVALID;

        chg_push_event_on_command_queue( chg_task_event );
        INTFREE();

    } /* end if(chg_usb_OnOff_nv_item_val == CHG_CMD_ENABLE) */
} /* chg_usb_i_is_not_available */

void chg_usb_charger_connected(pm_app_otg_a_dev_type otg_dev)
{
    if(chg_usb_OnOff_nv_item_val == CHG_CMD_ENABLE)
    {
        chg_usb_otg_dev = otg_dev;
    }
} /* chg_usb_charger_connected */

void chg_usb_charger_disconnected(void)
{
    if(chg_usb_OnOff_nv_item_val == CHG_CMD_ENABLE)
    {
        chg_usb_otg_dev = PM_APP_OTG_A_DEV_TYPE__INVALID;
    }
} /* chg_usb_charger_disconnected */


void chg_usb_suspended(boolean usbSuspended)
{
   if(chg_usb_OnOff_nv_item_val == CHG_CMD_ENABLE)
   {

      if (usbSuspended == TRUE)
      {
          /* Construct a charging event */
          INTLOCK();
          chg_task_evt_type      chg_task_event;
          chg_task_event.event_id = CHG_EVT__SYSTEM;
          chg_task_event.system_event_id  = CHG_SYSTEM__USB_SUSPEND;
          chg_push_event_on_command_queue( chg_task_event );
          INTFREE();
      }
      else
      {
          MSG_ERROR("Invalid input parameter.",0,0,0);
      }
      
   }
} 


void chg_usb_convert_and_set_usb_imaxsel(uint32 isetting)
{
  pm_err_flag_type  err            = PM_ERR_FLAG__SUCCESS;
  uint32            i_available    = 0;   /* mA */
 
  const uint8       i_steps        = AUTO_CHARGING_IMAX_MINIMUM;

  switch(chg_usb_otg_dev)
  {
    case PM_APP_OTG_A_DEV_TYPE__USB_HOST:
        /* For a HOST device, we are not allow to draw more current than the 
           device can supply. The PMIC draws IMAXSEL setting + approximatly 
           30 mA (USB charger HW), Subtract 100 mA to the user setting to take 
           into account the extra (USB charger HW) current into account. 
           Note: The PMIC minimum setting is 100 mA.*/  
        if (isetting < i_steps) 
        {
            i_available = 0;
        }
        else if (isetting == i_steps) 
        {   
            i_available = i_steps;
        }
        else 
        {   
            i_available = isetting - i_steps;
        }

        break;

    case PM_APP_OTG_A_DEV_TYPE__USB_CHARGER:
    case PM_APP_OTG_A_DEV_TYPE__USB_CARKIT:

        if (isetting == 0) 
        {
            i_available = 0;

            #ifdef FEATURE_HS_USB_PMIC_PHY
            (void)pm_app_otg_process_usb_power_line_control_request(FALSE);
            #endif
        }
        else
        {
            #ifdef FEATURE_HS_USB_PMIC_PHY
            /* If we are not a USB HOST device and the PMIC transceiver 
               allows us to collapse the USB devise voltage then configure
               imaxsel higher than what the USB devise can provide so that 
               the voltage will collapse. */
            if (pm_app_otg_process_usb_power_line_control_request(TRUE))
            {

                i_available = isetting + i_steps;
            }
            else
            {   /* The PMIC transceiver does not allow for us to collapse
                   the USB device voltage. */
                i_available = CHG_USB_CARKIT_IMAX_MAXIMUM;
            }
            #else
                i_available = isetting - i_steps;
            #endif
        }

        break;    

    default:
      MSG_ERROR("INVALID USB CHARGER DEVICE DETECTED (%d).!!", chg_usb_otg_dev,0,0);
      /* Disable USB charging. */
      i_available = 0;
  }

 INTLOCK(); 

  /* disable suspend mode if current limit is greator than isteps */
  if (isetting > i_steps) 
  {
     err |= pm_chg_usb_suspend(false);
  }     

  /* Save the USB IMAXSEL select setting. */
  chg_usb_imaxsel.mA  = i_available;
  err |= pm_chg_imaxsel_set(chg_usb_imaxsel.mA);

  /* Find out if there was an PMIC API error */
  if (err != PM_ERR_FLAG__SUCCESS) 
  {
      MSG_ERROR(" PMIC API ERROR(0x%x).",err,0,0);
  }  

  INTFREE();
} /* chg_usb_convert_and_set_usb_imaxsel() */

void chg_wall_valid_isr(void*)
{
   pm_err_flag_type    err       = PM_ERR_FLAG__SUCCESS;
   boolean             rt_status;
   
   /* First debounce the IRQ. */
   err |= pm_get_rt_status(PM_VALID_CHG_RT_ST, &rt_status);

   if (rt_status) 
   {
       /* Disable the valid charger IRQ through masking */
       err =  pm_clear_irq_multi_handle(PM_VALID_CHG_IRQ_HDL, chg_wall_valid_isr);     

       /*Clear the valid charger IRQ */
       err |= pm_clear_irq(PM_VALID_CHG_IRQ_HDL);

       /* Reset the invalid wall charger IRQ */
       err |= pm_clear_irq(PM_INVALID_CHG_IRQ_HDL);
       /* Enable the wall charger invalid IRQ through unmasking */
       err |= pm_set_irq_multi_handle(PM_INVALID_CHG_IRQ_HDL, 
                                      chg_wall_invalid_isr, 
                                      NULL);

   #ifndef FEATURE_PMIC_USB_DETECTION_USING_VCHG

       /* Construct a charging event */
       INTLOCK();
       chg_task_evt_type      chg_task_event;
       chg_task_event.event_id = CHG_EVT__IRQ;
       chg_task_event.irq_id   = PM_VALID_CHG_IRQ_HDL;

       chg_push_event_on_command_queue( chg_task_event );
       INTFREE();

   #endif /* FEATURE_PMIC_USB_DETECTION_USING_VCHG */
   }
   else
   {
       /* Reset/clear the valid charger IRQ */
       err |= pm_clear_irq(PM_VALID_CHG_IRQ_HDL); 
   }
   
   /* Find out if there was an PMIC API error */
   if (err != PM_ERR_FLAG__SUCCESS) 
   {
       MSG_ERROR(" PMIC API ERROR(0x%x).",err,0,0);
   }
} /* end chg_wall_valid_isr() */

void chg_wall_invalid_isr(void*)
{
   pm_err_flag_type    err = PM_ERR_FLAG__SUCCESS;

   /* Disable the invalid charger IRQ through masking */
   err = pm_clear_irq_multi_handle(PM_INVALID_CHG_IRQ_HDL, chg_wall_invalid_isr);
      
   /* Clear the invalid charger IRQ */
   err |= pm_clear_irq(PM_INVALID_CHG_IRQ_HDL);

   /* Reset/clear the valid charger IRQ */
   err |= pm_clear_irq(PM_VALID_CHG_IRQ_HDL); 
   /* Enable the valid charger IRQ */
   err |= pm_set_irq_multi_handle(PM_VALID_CHG_IRQ_HDL, 
                                  chg_wall_valid_isr, 
                                  NULL);

   #ifndef FEATURE_PMIC_USB_DETECTION_USING_VCHG

   /* Construct a charging event */
   INTLOCK();
   chg_task_evt_type      chg_task_event;
   chg_task_event.event_id = CHG_EVT__IRQ;
   chg_task_event.irq_id   = PM_INVALID_CHG_IRQ_HDL;

   chg_push_event_on_command_queue( chg_task_event );
   INTFREE();

   #endif /* FEATURE_PMIC_USB_DETECTION_USING_VCHG */
   
   /* Find out if there was an PMIC API error */
   if (err != PM_ERR_FLAG__SUCCESS) 
   {
       MSG_ERROR(" PMIC API ERROR(0x%x).",err,0,0);
   }
} /* end chg_wall_invalid_isr() */

void chg_update_usb_OnOff_nv_item_val()
{
    if (chg_nv_cmd.status == NV_DONE_S) 
    {
        /* Do we enable or disable USB charging by default. */
        switch (chg_nv_data_buf.chg_usb_nvdisable_val)               
        {
          case NV_CHG_USB_NVSWITCH_DIS:
            /* Disable USB charging. */
            chg_usb_OnOff_nv_item_val = CHG_CMD_DISABLE;      

            break;

          case NV_CHG_USB_NVSWITCH_ENA:
            /* Enable USB charging. */
            chg_usb_OnOff_nv_item_val = CHG_CMD_ENABLE;      
            break;

          default:
            MSG_ERROR("NV_CHG_USB_NVDISABLE_VAL_I is invalid.",0,0,0);
        }      
    }
    else
    {
        /* The NV item is invalid or most likely, it has never been set.
           Lets assume that the NV item has never been set. In this case
           we assume that we are in the factory. The factory requires
           that USB charging is disable by default. */
        chg_usb_OnOff_nv_item_val = CHG_CMD_DISABLE;      
    }
}

boolean chg_is_application_initialized(void)
{
    return chg_is_task_initialized;
}

#endif /* FEATURE_CHG_TASK || T_WINNT */
