/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                   P S . C 

DESCRIPTION
  This is the header file for the Data protocol Task. Contained herein are
  the functions needed to initialize all the modules that execute in PS task 
  context and the main task processing loop.
  

EXTERNALIZED FUNCTIONS
  ps_task()
    Data Protocol Task entry point and main processing loop.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the PS services can be used unless MC has created the PS task 
  and ps_task() has been invoked.

Copyright (c) 2002-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps.c_v   1.13   12 Feb 2003 20:35:26   omichael  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps.c#2 $ $DateTime: 2007/12/10 23:14:44 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/01/07    scb    Added initialization of callback functions to handle
                   GSDI events
05/27/03    om     Changed powerup to explicitly order lo and ipsec ACL's.
05/21/03    vas    Added support for HDR. Processing HDR auth signal
04/15/03    ss     Added PS_LO_TX_DATA_Q_SIGNAL processing
02/12/03    om     Added Loopback Device initialization.
02/06/03    mvl    Initializing IP fragmentation at power up.
12/30/02    jd     Removed mip signals from internal enabled mask, done at
                   runtime in ps_mip_task
12/29/02    jd     Moved mip_perf_init() into mip_meta_sm_init()
12/29/02    jd     Add PS_MIP_DMU_RAND_SIGNAL to enabled mask
                   Handle MIP signals via registered callbacks
11/25/02    usb    Included header file for dsmip_perf
11/19/02    jd     Featurized reference to MIP signals
11/14/02    jd     Added commands for mobile ip subtask
11/13/02    rc     Added support for processing SNDCP signals.
11/01/02    ifk    Removed call to dss_init_dns_cache_tbl(), added 
                   dss_dns_init()
10/07/02    aku    Added support for processing PS_SOCKET_FLOW_ENABLED_SIG
10/03/02    rc     Added support for processing PDP signals.
09/03/02    usb    Wrapped dssnet specific calls in FEATURE_DATA_IS707
08/06/02    ak     Updated for new task interface.
08/05/02    usb    Externalized ps_dog_rpt_timer since its used in psi.c
07/31/02    usb    Renamed psmisc.h to ps_utils.h
07/25/02    mvl    Updates for PPP renaming.
07/17/02    aku    Added support for processing PS_SOCKET_RX_Q_SIGNAL
07/16/02    atp    Added processing for PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL.
07/15/02    aku    Removed the if 0 in psi_init() and added sockets 
                   initialization
06/22/02    aku    Added support for sockets calls
03/14/02    usb    Created Initial Version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_DATA_PS
#include "task.h"
#include "rex.h"
#include "queue.h"
#include "msg.h"
#include "err.h"
#include "dog.h"

#include "task.h"

#include "ps.h"
#include "ps_svc.h"
#include "psi.h"
#include "ps_utils.h"
#include "pstimer.h"
#include "ps_ppp_task.h"
#include "ps_ipfrg.h"
#include "ps_lo.h"

#ifdef FEATURE_SEC_IPSEC
#include "secipsiface.h"
#endif /* FEATURE_SEC_IPSEC */

#ifdef FEATURE_DATA_IS707
#include "ps707_dssnet.h"
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_DS_SOCKETS
#include "dsstcp.h"
#include "dss_ps.h"
#include "dssifcbacks.h"
#endif /* FEATURE_DS_SOCKETS */

#ifdef FEATURE_DS_MOBILE_IP
#include "ps_mip_task.h"
#include "dsmip_metasm.h"
#include "dsmip_io.h"
#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */
#endif /* FEATURE_DS_MOBILE_IP */


/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
  Timers needed for the Data Protocol Task.
---------------------------------------------------------------------------*/
rex_timer_type ps_dog_rpt_timer;                /* for watchdog reporting  */

/*--------------------------------------------------------------------------
  Local Function Prototypes
---------------------------------------------------------------------------*/

LOCAL void psi_powerup( void);
LOCAL void psi_init( void );

/* Needed to insert /dev/lo and /dev/ipsec as the first */
/* two ACL's explicitly at powerup.                     */
extern acl_type* routei_acl_list_ptr;


/*===========================================================================

                           FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/


/*===========================================================================

FUNCTION PSI_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the data Protocol task. This
  function will perform all first time thru functions for the Protocol task. 
  Note that initializations internal to PS should be performed before 
  initializing other components that run in PS task context.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
LOCAL void psi_powerup( void)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED( "Protocol Service Task Powerup", 0, 0, 0);

  /*-------------------------------------------------------------------------
    Initialize the command queues and link items onto the command free queue
  -------------------------------------------------------------------------*/
  q_init( &psi_cmd_q );
  q_init( &psi_cmd_free_q );

  for( i = 0; i < PSI_CMD_BUF_CNT; i++ )
  {
    q_put( &psi_cmd_free_q, q_link( &psi_cmd_buf[i], &psi_cmd_buf[i].link) );
  }

  /*------------------------------------------------------------------------
    Initialize all the signal and cmd handlers
  -------------------------------------------------------------------------*/
  for( i = 0; i < PS_MAX_SIGNALS; i++ )
  {
    psi_sig_handler[i].sig_handler = default_sig_handler;
    psi_sig_handler[i].user_data_ptr = NULL;
  }

  for( i = 0; i < PS_MAX_DEFINED_CMD_TYPES; i++ )
  {
    psi_cmd_handler[i] = default_cmd_handler;
  }

  /*------------------------------------------------------------------------
    Set signal mask for operation of all signals internal to PS Task
  -------------------------------------------------------------------------*/
  psi_enabled_sig_mask = ( (rex_sigs_type) 1 << PS_TASK_STOP_SIGNAL ) | 
                         ( (rex_sigs_type) 1 << PS_TASK_OFFLINE_SIGNAL ) |
                         ( (rex_sigs_type) 1 << PS_CMD_Q_SIGNAL ) |
                         ( (rex_sigs_type) 1 << PS_DOG_RPT_TIMER_SIGNAL ) |
                         ( (rex_sigs_type) 1 << PS_TIMER_TICK_SIGNAL ) |
                         ( (rex_sigs_type) 1 << PS_SIG_MASK_CHANGE_SIGNAL );


  /*------------------------------------------------------------------------
    Perform the power up initialization for all other modules that run in 
    PS context 
  -------------------------------------------------------------------------*/

  ps_timer_tick_init( ps_timer_handler,
                      psi_timer_tick_start_cb,
                      psi_timer_tick_stop_cb);

  ps_timer_init();

  ppp_task_powerup_init();

  ip_reasm_init();

#ifdef FEATURE_DS_MOBILE_IP
  /*-------------------------------------------------------------------------
    Mobile IP command handlers
  -------------------------------------------------------------------------*/
  mip_task_powerup_init();

#endif /* FEATURE_DS_MOBILE_IP */
  /*------------------------------------------------------------------------
    Initialise the PS hosted GPRS SNDCP module.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_GSM_GPRS
#error code not present
#endif

  /*------------------------------------------------------------------------
    Define and start the watchdog timer to wake up for the 1st petting.
  -------------------------------------------------------------------------*/
  rex_def_timer(&ps_dog_rpt_timer,
                &ps_tcb,
                (rex_sigs_type) 1 << PS_DOG_RPT_TIMER_SIGNAL);
  ps_start_timer( &ps_dog_rpt_timer, DOG_PS_RPT_TIME );

} /* psi_powerup() */


/*===========================================================================

FUNCTION PSI_INIT()

DESCRIPTION
  Initialization function for the data services task. This function
  performs the functions needed for the data task to exit disabled state.

DEPENDENCIES
  PS task should have been started.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void psi_init( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DS_SOCKETS
  dssifcb_init();
  dssps_init();
  dsstcp_init_tcb_array(); // should be replace by tcp_init();

  /*-------------------------------------------------------------------------
    Initialize DNS subsystem, acquires the DNS application id.
  -------------------------------------------------------------------------*/
  dss_dns_init();
#endif /*  FEATURE_DS_SOCKETS */

#ifdef FEATURE_DATA_IS707
  /*-------------------------------------------------------------------------
    DSSNET SM Initialization - Note dssnet is used even in non-sockets calls
    now.
  -------------------------------------------------------------------------*/
  dssnet_sm_init();
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_DS_MOBILE_IP
  /*-------------------------------------------------------------------------
    Mobile IP Initialization
  -------------------------------------------------------------------------*/
  mip_meta_sm_init();
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_SEC_IPSEC
  /* Create the IPSec interface - this has to be the second iface ! */
  secipsiface_init();
  secips_acl.next_ptr = routei_acl_list_ptr;
  routei_acl_list_ptr = &secips_acl;
#endif

  /* Create the loopback interface - this has to be the first iface ! */
  lo_init();
  lo_acl.next_ptr = routei_acl_list_ptr;
  routei_acl_list_ptr = &lo_acl;

  /*-------------------------------------------------------------------------
    RUIM related events callback initialization
  -------------------------------------------------------------------------*/
  psi_gsdi_init();
} /* psi_init() */


/*===========================================================================

FUNCTION PS_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the Data
  Protocol Service(s) task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ps_task
( 
  uint32 ignored
    /* Parameter received from Main Control task - ignored */
) 
{
  rex_sigs_type loop_sigs;   /* Signals to be processed within a task loop */
  rex_sigs_type set_sigs = 0;                     /* Currently set signals */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Do power-up initializtion then wait for start signal (from the MC task,
    which controls task startup sequencing)
  -------------------------------------------------------------------------*/
  psi_powerup();

  task_start( (rex_sigs_type) 1 << PS_DOG_RPT_TIMER_SIGNAL, 
                 DOG_PS_RPT, 
                 &ps_dog_rpt_timer 
               );
 
  psi_init();

  /*-------------------------------------------------------------------------
    Main task loop. Never exit!
  -------------------------------------------------------------------------*/
  for( ;;)
  {
    /*-----------------------------------------------------------------------
      Wait for work to do. No return from wait until one of the enabled
      signals is set. Even if we come out of wait, process only those 
      signals which are currently enabled.
    -----------------------------------------------------------------------*/

    set_sigs |= rex_get_sigs( &ps_tcb );
    set_sigs &= psi_enabled_sig_mask;

    /*-----------------------------------------------------------------------
      Enter into the Wait only if no requested signal events have occurred
    -----------------------------------------------------------------------*/
    if( set_sigs == 0 )
    {
      set_sigs = rex_wait( psi_enabled_sig_mask );
      set_sigs &= psi_enabled_sig_mask;
    }
    
    rex_clr_sigs( &ps_tcb, set_sigs );

    loop_sigs = set_sigs & ~( (rex_sigs_type) 1 << PS_SIG_MASK_CHANGE_SIGNAL );
    set_sigs = 0;

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority
    -----------------------------------------------------------------------*/
    while( loop_sigs )
    { 
      /*---------------------------------------------------------------------
        If watchdog timer expired, kick the pooch and set timer for next. 
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 << PS_DOG_RPT_TIMER_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 << PS_DOG_RPT_TIMER_SIGNAL );

        dog_report( DOG_PS_RPT );
        ps_start_timer( &ps_dog_rpt_timer, DOG_PS_RPT_TIME );
      }
      
      /*---------------------------------------------------------------------
        Incoming PPP data signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 << PS_PPP_UM_SN_RX_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 << PS_PPP_UM_SN_RX_SIGNAL );

        if ( psi_sig_handler[PS_PPP_UM_SN_RX_SIGNAL].sig_handler
             (
               PS_PPP_UM_SN_RX_SIGNAL,
               psi_sig_handler[PS_PPP_UM_SN_RX_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 << PS_PPP_UM_SN_RX_SIGNAL );
        }      
      }

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FEATURE_DATA_WCDMA_PS
#error code not present
#endif /* FEATURE_DATA_WCDMA_PS */

#ifdef FEATURE_GSM_GPRS
#error code not present
#endif/* FEATURE_GSM_GPRS */

      else if( loop_sigs & ( (rex_sigs_type) 1 << PS_PPP_RM_RX_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 << PS_PPP_RM_RX_SIGNAL );

        if ( psi_sig_handler[PS_PPP_RM_RX_SIGNAL].sig_handler
             (
               PS_PPP_RM_RX_SIGNAL,
               psi_sig_handler[PS_PPP_RM_RX_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 << PS_PPP_RM_RX_SIGNAL );
        }      
      }

      /*---------------------------------------------------------------------
        IS707 Async TX data available signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs &
               ( (rex_sigs_type) 1 << PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 
                             << PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL);

        if ( psi_sig_handler[PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL].sig_handler
           (
            PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL,
            psi_sig_handler[PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL].user_data_ptr
           ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 
                             << PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL);
        }
      }

#ifdef FEATURE_DS_SOCKETS
      /*---------------------------------------------------------------------
       Sockets Send signal 
      ---------------------------------------------------------------------*/
      else if( loop_sigs &
               ( (rex_sigs_type) 1 << PS_SOCKET_RX_Q_SIGNAL) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1
                             << PS_SOCKET_RX_Q_SIGNAL);

        if ( psi_sig_handler[PS_SOCKET_RX_Q_SIGNAL].sig_handler
           (
            PS_SOCKET_RX_Q_SIGNAL,
            psi_sig_handler[PS_SOCKET_RX_Q_SIGNAL].user_data_ptr
           ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1
                             << PS_SOCKET_RX_Q_SIGNAL);
        }
      }
      /*---------------------------------------------------------------------
       Sockets Flow enabled signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs &
               ( (rex_sigs_type) 1 << PS_SOCKET_FLOW_ENABLED_SIG) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1
                             << PS_SOCKET_FLOW_ENABLED_SIG);

        if ( psi_sig_handler[PS_SOCKET_FLOW_ENABLED_SIG].sig_handler
           (
            PS_SOCKET_FLOW_ENABLED_SIG,
            psi_sig_handler[PS_SOCKET_FLOW_ENABLED_SIG].user_data_ptr
           ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1
                             << PS_SOCKET_FLOW_ENABLED_SIG);
        }
      }

#endif /* FEATURE_DS_SOCKETS */
      /*---------------------------------------------------------------------
        PS Task command queue signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 << PS_CMD_Q_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 << PS_CMD_Q_SIGNAL );

        if ( psi_process_cmd() == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 << PS_CMD_Q_SIGNAL );
        }        
      }

#ifdef FEATURE_GSM_GPRS
#error code not present
#endif/* FEATURE_GSM_GPRS */

      /*---------------------------------------------------------------------
        PS Timer Tick Signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 << PS_TIMER_TICK_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 << PS_TIMER_TICK_SIGNAL );

        ps_timer_handler();
      }

#ifdef FEATURE_DS_MOBILE_IP
      /*---------------------------------------------------------------------
        Handle mobile IP socket events (messages coming in, etc.)

        This signal should be handled after the PS_TIMER_TICK_SIG in order 
        to be most efficient if the reg. holdoff timer and AAM arrive in 
        the same pass through psi_mgr() (else we would dump the RRQ)
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 << PS_MIP_SOCK_EVENT_SIGNAL ) )
      {
        MSG_MED("Received MIP socket event signal", 0,0,0);
        loop_sigs &= ~( (rex_sigs_type) 1 << PS_MIP_SOCK_EVENT_SIGNAL ); 
        if ( psi_sig_handler[PS_MIP_SOCK_EVENT_SIGNAL].sig_handler
           (
            PS_MIP_SOCK_EVENT_SIGNAL,
            psi_sig_handler[PS_MIP_SOCK_EVENT_SIGNAL].user_data_ptr
           ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 << PS_MIP_SOCK_EVENT_SIGNAL);
        }
      }

      /*---------------------------------------------------------------------
        Handle mobile IP network events (interfaces up/down)
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 << PS_MIP_NET_EVENT_SIGNAL ) )
      {
        MSG_MED("Received MIP network event signal", 0,0,0);
        loop_sigs &= ~( (rex_sigs_type) 1 << PS_MIP_NET_EVENT_SIGNAL ); 
        if ( psi_sig_handler[PS_MIP_NET_EVENT_SIGNAL].sig_handler
           (
            PS_MIP_NET_EVENT_SIGNAL,
            psi_sig_handler[PS_MIP_NET_EVENT_SIGNAL].user_data_ptr
           ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 << PS_MIP_NET_EVENT_SIGNAL);
        }
      }
#ifdef FEATURE_DS_MOBILE_IP_DMU
      /*---------------------------------------------------------------------
        Handle mobile IP DMU random # generation signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 << PS_MIP_DMU_RAND_SIGNAL ) )
      {
        MSG_MED("Received MIP DMU rand signal", 0,0,0);
        loop_sigs &= ~( (rex_sigs_type) 1 << PS_MIP_DMU_RAND_SIGNAL ); 
        if ( psi_sig_handler[PS_MIP_DMU_RAND_SIGNAL].sig_handler
           (
            PS_MIP_DMU_RAND_SIGNAL,
            psi_sig_handler[PS_MIP_DMU_RAND_SIGNAL].user_data_ptr
           ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 << PS_MIP_DMU_RAND_SIGNAL);
        }
      }
#endif /* FEATURE_DS_MOBILE_IP_DMU */

#endif /* FEATURE_DS_MOBILE_IP */

      /*---------------------------------------------------------------------
        Process loop-back tx data queue signal 
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 << PS_LO_TX_DATA_Q_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 << PS_LO_TX_DATA_Q_SIGNAL ); 
        if ( psi_sig_handler[PS_LO_TX_DATA_Q_SIGNAL].sig_handler
           (
            PS_LO_TX_DATA_Q_SIGNAL,
            psi_sig_handler[PS_LO_TX_DATA_Q_SIGNAL].user_data_ptr
           ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 << PS_LO_TX_DATA_Q_SIGNAL);
        }
      }

      /*---------------------------------------------------------------------
        Process other signals with registered handlers
      ---------------------------------------------------------------------*/

      /*---------------------------------------------------------------------
        Check if powerdown command signal was set.  If set then clear signal, 
        and process task stop procedures.
      ---------------------------------------------------------------------*/
      else if( loop_sigs & TASK_STOP_SIG ) 
      {
        MSG_MED( "TASK_STOP_SIG received", 0,0,0);
        
        loop_sigs &= ~TASK_STOP_SIG;

        task_stop();

      }
      /*---------------------------------------------------------------------
        Check if offline command signal was set.  If set then clear signal, 
        and process task offline procedures.
      ---------------------------------------------------------------------*/
      else if( loop_sigs & TASK_OFFLINE_SIG )
      {
        MSG_MED( "TASK_OFFLINE_SIG received", 0,0,0);

        loop_sigs &= ~TASK_OFFLINE_SIG;

        task_offline();

      }
    }
  } /* forever */
} /* ps_task() */

#endif /* FEATURE_DATA_PS */

