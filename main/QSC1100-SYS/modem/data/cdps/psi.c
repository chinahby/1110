/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                           P S I . C

DESCRIPTION

  Protocol Task local header file. All definitions, structures, and functions
  needed internal to protocol task services.
  
EXTERNALIZED FUNCTIONS  
  
  psi_process_cmd()
    Procsses a PS task cmd and calls appropriate handler function

  Copyright (c) 1993-2008 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header:   O:/src/asw/COMMON/vcs/psi.c_v   1.19   01 Mar 2002 20:25:16   
vsali  $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/16/08    am     Added MMGSDI event handling in gsdi_event_cback().
02/22/08    pp     Klocwork fixes.
10/01/07    scb    Added initialization of callback function to handle
                   GSDI events and the callback function handler definition
05/12/03    usb    Fixed bad while() condition in psi_wait()
08/05/02    usb    Added new function psi_wait()
03/14/02    usb    Created Initial Version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_DATA_PS
#include "rex.h"
#include "queue.h"
#include "msg.h"
#include "assert.h"
#include "err.h"
#include "dog.h"
#include "psi.h"
#include "ps_svc.h"
#include "ps.h"
#include "ps_utils.h"
#include "pstimer.h"
#include "dss_ps.h"

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#include "mmgsdilib.h"
#endif /* FEATURE_MMGSDI */

/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
psi_sig_handler_info_type psi_sig_handler[PS_MAX_SIGNALS];
ps_cmd_handler_type psi_cmd_handler[PS_MAX_DEFINED_CMD_TYPES];

/*--------------------------------------------------------------------------
  Protocol Stack Task Command links and queues
---------------------------------------------------------------------------*/

psi_cmd_type psi_cmd_buf[ PSI_CMD_BUF_CNT];

q_type psi_cmd_q;
q_type psi_cmd_free_q;

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
rex_sigs_type psi_enabled_sig_mask;

/*---------------------------------------------------------------------------
  Function proto-type for the registered timer tick ISR
---------------------------------------------------------------------------*/
void psi_tick_timer_isr( int4 );


/*===========================================================================

                           FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION PSI_TIMER_TICK_START_CB()

DESCRIPTION
    This is a callback function called by the pstimer module to start a tick
  timer. This function calls the clk_reg, to register the call back structure
  and also reset the ticks_elapsed_cntr to 0 here. Reseting the counter here
  ensures that every time we register the clock callback, the counter 
  restarts.
  
    This function has been made a callback function because, this ensures
  that pstimer module is decoupled from ps task. (one reason is if we
  decide to move the tick_timer to any other task, the changes should
  not effect the pstimer module).
  

DEPENDENCIES
  ps_tick_timer_struct should have been initialized before calling this 
  function. The initialization should be done by calling clk_def().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void psi_timer_tick_start_cb(int4 ps_tick_interval)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*------------------------------------------------------------------------
    Register the clock services. While registring we do the following:
      pointer to call back struct: ps_tick_timer_struct
      To sets signal to PS task whenever the timer expires we have the 
      function ps_tick_timer_isr. 
      Since the timer should go off every 50ms until we deregister the 
      callback , we make repeated as TRUE.
  ------------------------------------------------------------------------*/    
  clk_reg( &ps_tick_timer_struct,       /* pointer to call back structure */
           psi_tick_timer_isr,           /* address of func to call back   */
           ps_tick_interval,            /* delay till call back           */
           ps_tick_interval,            /* delay between repeat call backs*/
           TRUE                         /* causes repeated call backs     */
         );
  /*------------------------------------------------------------------------
    Since we are starting the timer service now, clear the counter. This
    counter will hereafter be increased when timer expires.
  ------------------------------------------------------------------------*/    
  ps_ms_elapsed_cntr = 0;

} /* psi_timer_tick_start_cb() */ 


/*===========================================================================

FUNCTION PSI_TIMER_TICK_ISR()

DESCRIPTION
  This is the callback function that will be called by the clock services.
  The function does the following tasks:
    1. Set the signal to the PS task. This informs PS task that the 50ms 
       timer has expired.
    2. Increment the ticks_elapsed_cntr. This counter will inform the PS
       task how many 50 msecs ticks have expired before the PS task services
       the TICK_TIMER_SIG signal. 
       
  The parameter passed, is not used in the code, so we name it appropriately.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Modifies the static variable ps_ticks_elapsed_cntr.

===========================================================================*/
void psi_tick_timer_isr(  int4 milliseconds )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Set the signal to the PS task. This informs PS task that the 50ms timer 
    has expired.
  ------------------------------------------------------------------------*/    
  PS_SET_SIGNAL( PS_TIMER_TICK_SIGNAL );

  /*------------------------------------------------------------------------
       Increment the ticks_elapsed_cntr. This counter will inform the PS 
    task how many 50 msecs ticks have expired before the PS task services 
    the TICK_TIMER_SIG signal.                                              
       
       After the PS task gets the signal it should clear this counter value.
       
       Note that we donot check if this signal has overflowed because, if 
    the overflow occurs on this 16bit value, then 65k * 50msecs is way 
    greater than the DOG Kick time. So, we find no use in checking for 
    overflow.
  ------------------------------------------------------------------------*/    
  ps_ms_elapsed_cntr += milliseconds;
} /* psi_tick_timer_isr() */


/*===========================================================================

FUNCTION PSI_TIMER_TICK_STOP_CB()

DESCRIPTION
  
   This is a callback function called by the pstimer module to stop a tick
  timer. This function calls clk_dereg to deregister the call back. 
    
    This function has been made a callback function because, this ensures
  that pstimer module is decoupled from ps task. (one reason is if we
  decide to move the tick_timer to any other task, the changes should
  not effect the pstimer module).
  
DEPENDENCIES
  Tick Timer should have been initialized before calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void psi_timer_tick_stop_cb( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*------------------------------------------------------------------------
    Deregister the call clock routines.
  ------------------------------------------------------------------------*/    
  clk_dereg( &ps_tick_timer_struct );     

} /* psi_timer_tick_stop_cb() */ 


/*===========================================================================

FUNCTION PSI_PROCESS_CMD()

DESCRIPTION
  This function is called in response to PS_CMD_Q_SIGNAL.  It retrieves the
  next command from the PS command queue and processes the command by calling 
  the registered cmd handler.
  
DEPENDENCIES
    An item must be on the PS command queue or an ERR_FATAL will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function 
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean psi_process_cmd
( 
  void
)
{
  psi_cmd_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the command item from the head of the command queue and call the 
    correponding handler with appropriate parameters.
    ERR_FATAL if no command is available.
  -------------------------------------------------------------------------*/
  if( ( cmd_ptr = ( psi_cmd_type * ) q_get( &psi_cmd_q ) ) == NULL )
  {
    ERR_FATAL( "No PS command to process", 0, 0, 0 );
    return TRUE;
  }

  if (cmd_ptr->cmd >= PS_MAX_DEFINED_CMD_TYPES)
  {
    MSG_ERROR("Invalid cmd 0x%x in Q", cmd_ptr->cmd, 0, 0);
    q_put( &psi_cmd_free_q, &cmd_ptr->link );
    return TRUE;
  }
  else
  {
    psi_cmd_handler[cmd_ptr->cmd]( cmd_ptr->cmd, cmd_ptr->user_data_ptr );
    q_put( &psi_cmd_free_q, &cmd_ptr->link );
  }

  return ( q_cnt( &psi_cmd_q ) ? FALSE : TRUE );
} /* psi_process_cmd() */


/*===========================================================================

FUNCTION PSI_WAIT()

DESCRIPTION
  This function performs the PS Task waiting function. This function will 
  not return until at least one signal from the requested mask is set. 
  The watchdog is also kicked at least once.
  
DEPENDENCIES
  None

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated 
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared

===========================================================================*/
rex_sigs_type psi_wait
( 
  rex_sigs_type requested_mask              /* mask of signals to wait for */
)
{
  rex_sigs_type set_sigs;                         /* Currently set signals */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    First check to see if one of the signals in the mask is already set.  If 
    yes, then clear those signals and return. Otherwise enter into the wait
    only if no requested signal events have occurred
  -------------------------------------------------------------------------*/
  set_sigs = rex_get_sigs( &ps_tcb );

  while ( (set_sigs & requested_mask) == 0 )
  {
    set_sigs = rex_wait( requested_mask | 
                         ( (rex_sigs_type) 1 << PS_DOG_RPT_TIMER_SIGNAL ) ); 
  
    /*-----------------------------------------------------------------------
      If watchdog timer expired, kick the pooch and set timer for next. 
    -----------------------------------------------------------------------*/
    if( set_sigs & ( (rex_sigs_type) 1 << PS_DOG_RPT_TIMER_SIGNAL ) )
    {
      rex_clr_sigs( &ps_tcb, 
                    ( (rex_sigs_type) 1 << PS_DOG_RPT_TIMER_SIGNAL ) );
      dog_report( DOG_PS_RPT );
      ps_start_timer( &ps_dog_rpt_timer, DOG_PS_RPT_TIME );
    }  
  }

  rex_clr_sigs( &ps_tcb, ( set_sigs & requested_mask ) );
  return ( set_sigs & requested_mask );
} /* psi_wait() */


/*===========================================================================

FUNCTION DEFAULT_SIG_HANDLER()

DESCRIPTION
  This function is registered as the default signal handler for all the 
  signals which do not have a registered handler. It merely prints a warning 
  message.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean default_sig_handler
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */ 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ERR( "Signal %d, no handler registered", sig, 0, 0);

  /*-------------------------------------------------------------------------
    Return TRUE since we don't want this signal to be enqueued again.
  -------------------------------------------------------------------------*/
  return TRUE;
} /* default_sig_handler() */


/*===========================================================================

FUNCTION DEFAULT_CMD_HANDLER()

DESCRIPTION
  This function is registered as the default command handler for all the 
  commands which do not have a registered handler. It merely prints a warning 
  message.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void default_cmd_handler
(
  ps_cmd_enum_type cmd,          /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ERR( "Command %d, no handler registered", cmd, 0, 0 );
} /* default_cmd_handler() */

/*===========================================================================

FUNCTION PSI_GSDI_EVENT_HANDLER_CBACK()

DESCRIPTION
  Callback function which handles GSDI related events.

DEPENDENCIES
  PS task should have been started. GSDI event must have
  been triggered.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void psi_gsdi_event_handler_cback
( 
  ps_cmd_enum_type             cmd_name,
  void                         *user_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(PS_GSDI_EVENT_HANDLER_CMD == cmd_name);

  if(NULL != user_info_ptr)
  {
#ifdef FEATURE_MMGSDI
    ASSERT(
            (GSDI_SIM_INIT_COMPLETED == 
              *((gsdi_sim_events_T *)user_info_ptr)) ||
            (MMGSDI_CARD_INIT_COMPLETED_EVT ==
              *((mmgsdi_events_enum_type *)user_info_ptr))
          );
#endif
    /*-----------------------------------------------------------------------
      Calling the appropriate NV item initialization functions for various
      sub-modules in PS.
    -----------------------------------------------------------------------*/
    dssps_nv_init();
  }
} /* psi_sim_init_completed_event_cback */



/*===========================================================================

FUNCTION PSI_GSDI_INIT()

DESCRIPTION
  Initialization function for the initialization of GSDI module for PS.

DEPENDENCIES
  PS task should have been started.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void psi_gsdi_init
( 
  void 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Set the command handler for GSDI Event Hander related commands
  -------------------------------------------------------------------------*/

  ps_set_cmd_handler(PS_GSDI_EVENT_HANDLER_CMD, 
                     psi_gsdi_event_handler_cback);
  
} /* psi_gsdi_init() */

#endif /* FEATURE_DATA_PS */
