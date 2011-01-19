/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         VBATT TASK IMPLEMENTATION.

GENERAL DESCRIPTION
  This file implements the vbatt_task and it's supplementary functions for the 
  new vbatt-driver, which uses Qualcomm Power Management ICs. The new vbatt 
  driver will not work without this file.

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/vbatt/main/latest/app/src/vbatt_task.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/09   APU     Added APIs to support the new VBATT info structure and 
                   also support the CHG callback functionality.
09/09/08   APU     Added Doxygen Comments to the code.
07/26/08   APU     Added vbatt_set_window(), a function to set new/next window. 
07/08/08   APU     Tested with 6260, 7201 and added comments
01/02/08   APU     Created.
===========================================================================*/



/*! \file 
 *  \n
 *  \brief  vbatt_task.c ---- VBATT TASK RELATED DEFINITION
 *  \n
 *  \n This header file contains class declarations of the new vbatt functions.
 *  and the BatteryDebug class.
 *  \n
 *  \n &copy; Copyright 2007 QUALCOMM Incorporated, All Rights Reserved
 */


/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tmc.h"        /*tmc_task_start and tmc signals*/
#include "dog.h"
#include "msg.h"        /* reporting of messages for errors and debugging */
#include "task.h"
#include "vbatt_task.h"
#include "vbatt_pmic_comparator.h"  /*!< For VBATT task related definitions */

/* For calling vbatt_remote_app_init( ) and vbatt_remotecb_app_init( ) */
#ifdef FEATURE_EXPORT_VBATT_REMOTE
#include "vbatt_remote_rpc.h"
#endif /* FEATURE_EXPORT_VBATT_REMOTE */


#ifdef FEATURE_VBATT_TASK /* FEATURE_VBATT_TASK */ 
/*===========================================================================

                               MACROS

===========================================================================*/
#define BOOT_TIME_IN_MS_TO_WAIT         1000
#define HYS_TIME_IN_MS_TO_WAIT          35
#define CHG_WAIT_TIME_IN_MS_TO_WAIT     3000 
/* Macro to kick the watchdog timer and set a timer to wake it again.      */

#define VBATT_KICK_WATCHDOG() \
   dog_report(DOG_VBATT_RPT); \
   (void) rex_set_timer(&vbatt_rpt_timer, DOG_VBATT_RPT_TIME);


/*===========================================================================

                            LOCAL VARIABLES

===========================================================================*/


/*! Timers used for watchdog reporting and generic purposes, if any. */
static rex_timer_type          vbatt_rpt_timer ;      /* For dog reporting  */
static rex_timer_type          vbatt_hys_timer ;      /* For disabling interrupts */
static boolean                 vbatt_chg_event_detected ;

/*===========================================================================

FUNCTION vbatt_set_window                                  EXTERNAL FUNCTION

DESCRIPTION
 After an interrupt, vbatt_task is signalled. vbatt_task then processes the
 clients and sets the new window using vbatt_set_window().                  .  

EXAMPLE:

INPUT PARAMETERS
 NONE.

RETURN VALUE

DEPENDENCIES
  The following functions must have been called:
  1) pm_init()
  2) vbatt_init()
  3) vbatt_task_init()

SIDE EFFECTS

===========================================================================*/

void 
vbatt_set_window ( void )
{
    /* 
       NOTE: Sequence SHOULD NOT be changed.
       Set window and enable the interrupt disabled during ISR. 
       1. Check for window change at the same time get new vbatt voltage.
       2. Set the comparator window and program.
    */
    pm_vbatt_comp_window_change_detector () ;
    pm_set_comparator_window () ;
}


/*===========================================================================

FUNCTION vbatt_service_vbatt_changed_sig                  EXTERNAL FUNCTION

DESCRIPTION
 After an interrupt, vbatt_task is signalled. vbatt_task then processes the
 signals. This function takes care of the signal and the clients.                   .  

EXAMPLE:

INPUT PARAMETERS
 NONE.

RETURN VALUE

DEPENDENCIES
  The following functions must have been called:
  1) pm_init()
  2) vbatt_init()
  3) vbatt_task_init()

SIDE EFFECTS

===========================================================================*/

void 
vbatt_service_vbatt_changed_sig ( void )
{
    /* Clear the signal prior to servicing the clients, to avoid the
    ** race condition where an item is added just as we are leaving.
    */
    (void) rex_clr_sigs ( &vbatt_tcb , VBATT_CHANGED_SIG ) ;


    /* Get the new voltage, prepare and program the next window */
    vbatt_set_window () ;

    if ( pm_vbatt_is_servicing_tables_enabled() ) 
    {   
        pm_service_vbatt_tables () ;
    }
    vbatt_reset_chg_event () ;

    /* Acknowledge that task has serviced the new vbatt values */
    ( void ) pm_vbatt_voltage_change_registered () ;

    ( void ) rex_clr_sigs ( &vbatt_tcb , VBATT_TIME_HYS_SIG ) ;

    /* Clear the CHG signal. */
    ( void ) rex_clr_sigs ( &vbatt_tcb , VBATT_CHG_EVT_SIG ) ;

    ( void ) rex_timed_wait ( VBATT_TIME_HYS_SIG , 
                                  &vbatt_hys_timer , 
                                  HYS_TIME_IN_MS_TO_WAIT ) ;

}




/*===========================================================================

FUNCTION vbatt_task_init                                    EXTERNAL FUNCTION

DESCRIPTION
 When the tmc(task-main-controller) spawns the tasks during bootup. vbatt_task
 is one of the task. When the CPU is given to vbatt_task for the very first
 time, this function is called. vbatt_task_init initializes the task and the
 related resources.

 EXAMPLE:

INPUT PARAMETERS
 NONE.

RETURN VALUE

DEPENDENCIES
  The following functions must have been called:
  1) pm_init()
  2) vbatt_init()

SIDE EFFECTS

===========================================================================*/
void 
vbatt_task_init ( void )
{
    /* Used an automatic variable to be able to change the 
       timer value during debug and in future to be able to
       set it through FTM
    */
    uint16 time_in_ms_to_wait = BOOT_TIME_IN_MS_TO_WAIT ;
    /*
     time = (max_hysteresis_time + system_response_time) X MAX_VBATT_STEPS 
          = (  ( (2^7) X 122uS) + 10 mS )    X 15 )
          = ( (128 X 122uS) + 10mS ) X 15
          = ( 15616uS + 10mS ) X 15
          = ( 15.616mS + 10mS ) X 15
          ~ 26mS X 15
          = 390 mS ~400mS
    */


    /* Initialize timers - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    rex_def_timer ( &vbatt_rpt_timer , &vbatt_tcb, VBATT_RPT_TIMER_SIG ) ; /* Dog Report */
    rex_def_timer ( &vbatt_hys_timer , &vbatt_tcb, VBATT_TIME_HYS_SIG  ) ;

    ( void ) rex_clr_sigs ( &vbatt_tcb , VBATT_CHANGED_SIG ) ;
    ( void ) rex_clr_sigs ( &vbatt_tcb , VBATT_TIME_HYS_SIG ) ;

    /* Wait for the start signal from the Task Main Controller */
    tmc_task_start() ;
    (void) rex_clr_sigs( rex_self(), TASK_START_SIG );

    /* 
       After the phone has booted, we need a starting point to set a PMIC comparator window.
       Now if we set the starting-window to 3.5 volts and if the actual voltage is say 4.2 V,
       with the interrupts enabled, in the vbatt ISR, the window will be be moved from 3.5 V
       to all the way to 4.2 V. This will happen with the window shifting from 3.5 to 3.6 and 
       then 3.6 to 3.7 and so on and so forth until 4.2 is reached.
       While this is happening and if our task is running, corresponding registered clients will
       get serviced. So all the clients registered for voltage going up between 3.5 to 4.2 will
       be serviced. However this is a transitory period and our window has not settled to a 
       stable value after boot and if we call the CB functions from various clients, we are 
       misleading them and we should therefore not service the clients until the comparator
       window has stablized to proper battery volatge.
       Therefore we give 400mS for us to settle after booting. During this time the window is
       moved until it stablizes near the true battery voltage and the clients are not serviced.
       To accomplish this we use one time rex timer and our task will not actually start 
       servicing the clients before 400 mS */

    /* Wait some time before the voltage will stabalize and we can start 
        servicing the clients */
    (void)rex_timed_wait ( VBATT_TIME_HYS_SIG , 
                           &vbatt_hys_timer , time_in_ms_to_wait ) ;

#ifdef FEATURE_EXPORT_VBATT_REMOTE
    vbatt_remote_app_init( ) ;
#endif /* FEATURE_EXPORT_VBATT_REMOTE */


    /* Indicate the start of the task */
    (void) pm_vbatt_indicate_task_start () ; 

    /* Restart interrupts since the supply has supposedly stabalized */ 
    pm_start_vbatt_interrupt () ;

    /* ------------------------------------------------------------------
     ** Initially kick watchdog and set timer for watchdog report interval
     ** ------------------------------------------------------------------ */
    VBATT_KICK_WATCHDOG(); 
}


/*===========================================================================

FUNCTION vbatt_task                                     EXTERNAL FUNCTION

DESCRIPTION
 The vbatt driver does all its dealings which include but are not limited to 
 1. Servicing clients
 2. Calling their callbacks.
 3. Setting PMIC-Comparator windows after boot-up.
 4. Servicing REX\L4 sgnals.
 in the vbatt_task.
 5. Initializing the PMIC hadware and the task during start startup.

EXAMPLE:
 NONE.

INPUT PARAMETERS
 dword - Not Used.
 
RETURN VALUE
 None.

DEPENDENCIES
  The following functions must have been called:
  1) pm_init()

SIDE EFFECTS

===========================================================================*/
void  
vbatt_task
(
  dword              parm       /* from REX - ignored                     */
)
{
    rex_sigs_type      sigs;             /* REX signals word              */

/*-------------------------------------------------------------------------*/
    /* If vbatt init does not pass the task never gets ready */
    if ( vbatt_init () == VBATT_INIT_PASSED )
    {
        /* Initialize vbatt task */
        vbatt_task_init () ;

        /* Permit the task to service the tables */
        (void) pm_vbatt_enable_table_servicing () ;

        /* Loop forever handling commands */
        for (;;)
        {
            sigs = rex_wait( 
                             VBATT_RPT_TIMER_SIG    |   /* Watchdog report timer */
                             TASK_STOP_SIG          |   /* Command to shutdown   */
                             VBATT_CHANGED_SIG          /* Command Queue signal  */
                           ) ;

            if ( sigs & VBATT_RPT_TIMER_SIG )
            {
                /* -----------------------------
                ** Kick watchdog and reset timer
                ** ----------------------------- */
                VBATT_KICK_WATCHDOG();

                /* In case someone has frozen interrupt */
                //pm_start_vbatt_interrupt () ;
            }

            /* -------------------------------------------------------
            ** Check if powerdown command signal was set.  If set then
            ** clear signal, and process task stop procedure.
            ** ------------------------------------------------------- */
            if ( sigs & TASK_STOP_SIG )
            {
                /* Acknowledge we are done */
                (void) rex_set_sigs ( &tmc_tcb , TMC_ACK_SIG ) ; 
                (void) rex_clr_sigs ( &vbatt_tcb , TASK_STOP_SIG ) ;

                dog_monitor_pause(DOG_VBATT_RPT) ; 
            }

            /* -----------------------------------------------------
            ** Check if any command signal was set.  If set then
            ** clear signal, and process the corresponding procedure.
            ** ----------------------------------------------------- */
            if ( sigs & ( VBATT_CHG_EVT_SIG | VBATT_CHANGED_SIG ) )
            {
                vbatt_service_vbatt_changed_sig () ;
                /* Restart the servicing of the interrupts */
                pm_start_vbatt_interrupt () ;
            }/* end if ( sigs & VBATT_CHANGED_SIG */
        } /* for (;;) */
    } /* If init succeeded and we are ready for the task */
    else 
    {
        /* If vbatt_init() has failed then stop the dog from monitoring 
        us and ask the tmc to move on. */
        tmc_task_start() ;
        dog_monitor_pause(DOG_VBATT_RPT) ; 
        pm_vbatt_deregister_irq ( ) ;
    }
} /* vbatt_task */






/*===========================================================================

FUNCTION vbatt_interrupt_handler_lean                    EXTERNAL FUNCTION

DESCRIPTION
 The vbatt_interrupt_handler_lean is one of the interrupt handlers that the 
 vbatt driver uses to service change in the vbatt voltage and therefore the 
 comparator signal. This is the main ISR. The other ISR used is only acive 
 during the bootup. This ISR:
 1. Clears IRQ.
 2. Sends a signal to the task.
 3. Disables the PMIC-comparator interrupt.

EXAMPLE:
 NONE.

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 None.

DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()
 3) vbatt_task_init()

SIDE EFFECTS
 NONE  
===========================================================================*/

void 
vbatt_interrupt_handler_lean ( void* vptr )
{
    /* Mask the int, let the task enable it after some msecs */
    pm_vbatt_deregister_irq () ;
    
    /* Signal a vbatt event  */
    ( void ) rex_set_sigs( &vbatt_tcb , VBATT_CHANGED_SIG ) ; 
}



/*===========================================================================

FUNCTION pm_vbatt_deregister_irq                          EXTERNAL FUNCTION

DESCRIPTION
 The pm_vbatt_deregister_irq is used for decoupling the PMIC comparator with 
 the ISR(vbatt_interrupt_handler_lean). This is desirable when the task is 
 processing clients based on one voltage and does not want to be disturbed. 
 Also if voltage is changing very quicky, it might be ripple because of charger
 or some other reason. In such case we want o decrease our response time by 
 off interrupts for some time. This function achives this.

EXAMPLE:
 NONE.

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 None.

DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()
 3) vbatt_task_init()

SIDE EFFECTS
 NONE  
===========================================================================*/
void 
pm_vbatt_deregister_irq ( void )
{
    /* Deregister for the interrupt because there is no one to service it now */
    pm_clear_irq ( PM_BAT_STAT_CHANGED_IRQ_HDL ) ;                             
    pm_clear_irq_multi_handle ( PM_BAT_STAT_CHANGED_IRQ_HDL , 
                                &vbatt_interrupt_handler_lean ) ; 
    /* Remove the pm_irq_call() if the call below is used. 
    pm_disable_comparator_alarm ( ) ;                             
    */
}




/*===========================================================================

FUNCTION pm_start_vbatt_interrupt                           EXTERNAL FUNCTION

DESCRIPTION
 The pm_start_vbatt_interrupt is called after pm_vbatt_deregister_irq. This 
 function couples the PMIC Comparator's interrupt to the driver-ISR.

EXAMPLE:
 NONE.

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 None.

DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()
 3) vbatt_task_init()

SIDE EFFECTS
 NONE  
===========================================================================*/
void 
pm_start_vbatt_interrupt ( void ) 
{
    ( void ) pm_set_irq_multi_handle ( PM_BAT_STAT_CHANGED_IRQ_HDL , 
                                       &vbatt_interrupt_handler_lean , 
                                       NULL ) ;
//  ( void ) pm_enable_comparator_alarm() ;
}

/*===========================================================================

FUNCTION vbatt_set_chg_event                           EXTERNAL FUNCTION

DESCRIPTION
 The vbatt_set_chg_event is called after a CHG event occurs. It sets a flag
 to record a charger event. 

EXAMPLE:
 NONE.

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 None.

DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()
 3) vbatt_task_init()

SIDE EFFECTS
 NONE  
===========================================================================*/
void
vbatt_set_chg_event ( void )
{
    INTLOCK() ;
    vbatt_chg_event_detected = TRUE ;
    INTFREE() ;
}

/*===========================================================================

FUNCTION vbatt_reset_chg_event                           EXTERNAL FUNCTION

DESCRIPTION
 The vbatt_reset_chg_event is called after pm_service_vbatt_tables() has 
 serviced all the clients. This makes the VBATT ready to receive new events. 

EXAMPLE:
 NONE.

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 None.

DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()
 3) vbatt_task_init()

SIDE EFFECTS
 NONE  
===========================================================================*/
void
vbatt_reset_chg_event ( void )
{
    INTLOCK() ;
    vbatt_chg_event_detected = FALSE ;
    INTFREE() ;
}


/*===========================================================================

FUNCTION vbatt_notify_chg_event                           EXTERNAL FUNCTION

DESCRIPTION
 The vbatt_notify_chg_event is called to notify the VBATT task of a CHG event. 

EXAMPLE:
 NONE.

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 None.

DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()
 3) vbatt_task_init()

SIDE EFFECTS
 NONE  
===========================================================================*/
void
vbatt_notify_chg_event ( void ) 
{
    ( void ) rex_set_sigs ( &vbatt_tcb , VBATT_CHG_EVT_SIG ) ;
    vbatt_set_chg_event ( ) ;
}


/*===========================================================================

FUNCTION vbatt_query_is_chg_event                           EXTERNAL FUNCTION

DESCRIPTION
 The vbatt_query_is_chg_event can be called to check if the last VBATT driver 
 event was a VBATT or a CHG event. 

EXAMPLE:
 NONE.

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 None.

DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()
 3) vbatt_task_init()

SIDE EFFECTS
 NONE  
===========================================================================*/
boolean
vbatt_query_is_chg_event ( void ) 
{
    return vbatt_chg_event_detected ;
}

#endif /* FEATURE_VBATT_TASK */

