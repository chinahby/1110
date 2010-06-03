/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D A T A   S E R V I C E S   T A S K

GENERAL DESCRIPTION
  This software unit contains the task entry point and the top-level dispatch
  routines for the Data Services (DS) Task. Functions to manipulate the Data
  Services Task's command queue are also included here.

EXTERNALIZED FUNCTIONS
  ds_task()
    Data Services Task entry point

  ds_get_cmd_buf()
    Returns a command buffer

  ds_put_cmd()
    Puts a command on the Data Services Task command queue

EXTERNALIZED FUNCTIONS INTERNAL TO DS SUBSYSTEM

  dsi_get_nv_item()
    Get an item from the nonvolatile memory.  

  dsi_put_nv_item()
    Put an item into the nonvolatile memory.  


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The DS Task should have been created before any calls to ds_get_cmd_buf()
  or ds_put_cmd().


   Copyright (c) 2001 - 2009 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dstask.c_v   1.18   28 Feb 2003 18:55:50   rchar  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/src/dstask.c#14 $ $DateTime: 2009/05/13 04:39:11 $ $Author: bhaviks $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/05/09   bs      Added support for mmgsdi interface.
01/06/09   sn      Added support to read required NV items when ATCOP is 
                   disabled.
01/23/09   sn      Fixed featurization issue.
01/21/09   nc      Added support for SMS Commands.
01/02/09   bs      Added support for 1X AT Phonebook commands.
07/09/08   bs      Added support for NAARefresh.
07/11/08   sn      Changed FEATURE_DS_SIP_MULTIPLE_PROFILE to 
                   FEATURE_DS_MULTIPLE_PROFILES.
06/24/08   sn      Modified to use updated data session function name.
03/20/08   psng    Updated feature macro for multiple sip profile
02/25/08   sn      Fixed lint error.
02/11/08   psng    Added NV Support for multiple sip profiles.
01/28/08   sn      Featurized WCDMA/GSM code.
10/15/07 sq/mga/yz Merged and made changes to support FEATURE_UIM_SUPPORT_3GPD.
11/01/06   an      Featurize ATCOP, RMSM, Async Fax
05/23/06   squ     Featurized async data
11/04/04   gr      Added PZID_HYS_SDB_DATA_CMD to notify the hysteresis 
                   engine whenever SDB packet data is exchanged
10/12/04   vr      Added PZID_HYS_DATA_READY_CMD to notify the hysteresis 
                   engine whenever PPP data is exchanged
05/12/04   sy      Added new command for pzid hysteresis timer expired.
01/12/04   snn     Cleared the DOG_TIMER_SIG in the set_signals variable
                   after processing the watch dog. This was done in dsi_wait
10/01/03   ak      Added PZID delay timer processing commands.
08/25/03   ak      Added new commands for 1x pkt IFACE indications.
05/21/03   vas     Added support for HDR. Processig of HDR auth commands.
05/15/03   rsl     Moved rex_clr_sigs to one common point just after dsi_wait.
05/05/03   usb     Added comment.
04/23/03   dgy     Merged GSM and WCDMA CS Data mode-specific handlers.
04/18/03   sb      Reduce DSI_CMD_BUF_COUNT to 12 to conserve storage. 
04/18/03   vsk     updated to use umts_rmsm instead of gen_rmsm 
04/11/03   ak      Updated to use new 707 RMSM CMD names.
03/27/03   ar      Reduce DSI_CMD_BUF_COUNT to 17 to conserve storage.
03/14/03   ar      Added DS_AT_CM_PH_CMD to command processing.
02/26/03   rc      Renamed DS_CM_SS_SYS_MODE_CHG_CMD as DS_CM_SS_SRV_CHG_CMD.
02/25/03   sb      Increased DSI_CMD_BUF_COUNT to 20. WMS is sending us 3 more
                   events. Hence the increase.
01/16/03   vsk     Initialize PDP registry during startup for wcdma / gprs
11/14/02   jd      Removed DS_RMSM_UM_PPP_UP_CMD (not used)
11/13/02   rc      Added Multimode support for GPRS.
11/12/02   ar      Add support for supplementary services events
11/11/02   sb      Call Related CM events now apply in all modes
10/14/02   ak      Removed JCDMA command processing - no longer needed.
10/14/02   sb      Merge from ETSI. PBM event declarations and state machine.
10/12/02   ak      Re-did FEATUREs around DS_GEN... commands
10/12/02   ak      Updated for JCDMA 1X.
10/04/02   usb     Added Generic RMSM specific command handling
10/03/02   rc      Added UMTS specific Commands to support multimode.
08/28/02   usb     Added/renamed RMSM cmd names
08/23/02   ak      Added OFFLINE & STOP sigs to requested sig mask.
08/14/02   atp     Added DS_707_ASYNC_ATZ_CMD.
08/06/02   ak      Updated for new task interface.
08/01/02   rc      Removed FEATURE_DS_SOCKETS wrap around socket commands. 
                   Renamed dsumtsps_process_socket_cmd() as 
                   dsumtsps_process_ps_cmds().
                   Fixed bug in dsi_wait() where the check for which signal was
                   received was incorrect.                  
07/27/02   vsk     Removed FEATURE_DATA_WCDMA_PS_PDP_IP and adhere to new 
                   wcdma/gprs mode specific handler interface and file/fn naming
07/24/02   atp     Added DS_707_ASYNC_PHYS_LINK_TEAR_DOWN_CMD.
07/22/02   ak      Added DS_707_PKT_IFACE_DOWN_CBACK_CMD.
07/13/02   atp     Added commands for 707 async.
07/13/02   atp     Added init of 707 async mgr when DS task comes up.
07/03/02   ak      In nv_get/put, check to make sure in DS task.
06/18/02   tmr     Added DS_COMPLETE_LL_DISCONNECT_CMD, DS_GCSD_RX_DCD_ON_CMD and
                   DS_GCSD_TX_DATA_FLUSHED_CMD
05/21/01   smp     Added DS_CM_SS_SYS_MODE_CHG_CMD to dsi_process_cmds().
05/20/02   usb     Added command support for RMSM (IS707 pkt call Rm handler)
05/10/02   ak      Various changes to support 707 and new PS structure.
05/07/02    wx     Remove FEATURE_DSAT_TIMER
04/19/02   vsk     Added PDP context preservation support
04/18/02   rc      Commented out tmc_task_ack() and added line 
                   (void) rex_set_sigs(&tmc_tcb, TMC_ACK_SIG) instead.
04/17/02   rc      Added DS_INITIATE_CALL_CMD, DS_ENTER_SOCKET_MODE_CMD, 
                   DS_EXIT_SOCKET_MODE_CMD, DS_ORIG_SOCKET_NET_CMD and 
                   DS_RELEASE_SOCKET_NET_CMD to to dsi_process_cmds() for 
                   processing commands related to socket calls.
04/04/02   tmr     Added handling of DS_TASK_STOP_SIG and DS_TASK_OFFLINE_SIG 
                   signals
02/26/02   tmr     Added DS_COMPLETE_LL_CONNECT_CMD, DS_RELEASE_CALL_CMD to
                   DSMGR processed commands, and DS_GCSD_XID_NEG_COMPL_CMD to
                   GCSD processed commands.
02/05/02   rlp     Added online-command mode support.
01/11/02   smp     Merged from packet data branch. Added WCDMA PS
                   mode-specific handler sub-task.
11/21/01   tmr     Added support for GSM CS data (GCSD) sub-task.
10/31/01   smp     Added support for watchdog kicking. Invoke WCDMA CS signal
                   handler function.
10/01/01   ar      Added dsi_get_nv_item() and dsi_put_nv_item()
08/29/01   smp     Renamed FEATURE_WCDMA_CS_DATA to FEATURE_DATA_WCDMA_CS.
08/28/01   smp     Added WCDMA CS mode-specific handler sub-task, and moved
                   all sub-task initialization out of dsi_task_init() into
                   ds_task().
08/17/01   smp     Renamed ATCOP files/functions/data types.
05/24/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "msg.h"
#include "err.h"
#include "nv.h"
#include "rex.h"
#include "task.h"
#include "queue.h"

#include "task.h"

#include "dstask.h"
#include "dstaski.h"
#include "ds3gtimer.h"
#include "ds3gmgr.h"

#ifndef FEATURE_DATA_STRIP_ATCOP
#include "ds3gsiolib.h"
#include "dsat.h"
  #ifdef FEATURE_UIM_SUPPORT_3GPD
    #include "dsatme.h"
  #endif /* FEATURE_UIM_SUPPORT_3GPD */
#else
#include "dsatprofile.h"
#endif /* FEATURE_DATA_STRIP_ATCOP */

#include "dog.h"

#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_CS)) || \
     (defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD)))
#error code not present
#endif

#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_PS)) || \
     (defined(FEATURE_GSM  ) && defined(FEATURE_GSM_GPRS)))
#error code not present
#endif

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif

#ifdef FEATURE_DATA_IS707

  #ifndef FEATURE_ASYNC_DATA_NOOP
    #include "ds707_async_mgr.h"
  #endif /* FEATURE_ASYNC_DATA_NOOP */

  #include "ds707_pkt_mgr.h"
  #ifndef FEATURE_DATA_STRIP_ATCOP
  #include "ds707_rmsm.h"
  #endif
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FEATURE_DS_MULTIPLE_PROFILES
#include "ds707_data_session_profile.h"
#endif /* FEATURE_DS_MULTIPLE_PROFILES */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Data services task command queue definitions.
---------------------------------------------------------------------------*/ 
#define DSI_CMD_BUF_COUNT    12 /* Maximum number of commands on the queue */

LOCAL ds_cmd_type dsi_cmd_buf[DSI_CMD_BUF_COUNT];

LOCAL q_type dsi_cmd_q;                           /* DS Task command queue */
LOCAL q_type dsi_cmd_free_q;     /* Queue for storing free command buffers */

/*---------------------------------------------------------------------------
  Data services task NV command item.
---------------------------------------------------------------------------*/ 
LOCAL nv_cmd_type  nv_command;                                 /* NV items */

/*---------------------------------------------------------------------------
  Timer for watchdog reporting.
---------------------------------------------------------------------------*/
LOCAL rex_timer_type ds_dog_rpt_timer;


/*===========================================================================

              EXTERNAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
nv_esn_type     ds_esn;    /* Mobile's ESN value.  Used by an AT cmd       */


/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSI_TASK_INIT

DESCRIPTION
  This function performs all the power-up initialization for the Data
  Services Task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  dsi_task_init( void )
{
  uint8    i;                                                /* Loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Initialize the command queue and the free command queue, and link the
    command items onto the free command queue.
  -------------------------------------------------------------------------*/  
  (void)q_init( &dsi_cmd_q );
  (void)q_init( &dsi_cmd_free_q );

  for( i = 0; i < DSI_CMD_BUF_COUNT; i++ )
  {
    (void)q_link( &dsi_cmd_buf[i], &dsi_cmd_buf[i].hdr.link );
    q_put( &dsi_cmd_free_q, &dsi_cmd_buf[i].hdr.link );
  }

  /*-------------------------------------------------------------------------
    Define the watchdog timer, and start the timer.
  -------------------------------------------------------------------------*/
  rex_def_timer( &ds_dog_rpt_timer, &ds_tcb, DS_DOG_RPT_TIMER_SIG );
  (void)rex_set_timer( &ds_dog_rpt_timer, DOG_DS_RPT_TIME );

} /* dsi_task_init() */


/*===========================================================================
FUNCTION DSI_NV_INIT

DESCRIPTION
  This function gets all the NV items for the Data task and stores them in
  the appropriate variables.

DEPENDENCIES
  The NV task must be started and running.

RETURN VALUE
  None

SIDE EFFECTS
 Each call to 'dsi_nv_get_item' results in a Wait and watchdog kicking
===========================================================================*/
void dsi_nv_init(void)
{
  nv_stat_enum_type   nv_status;                   /* status from NV call  */
  nv_item_type        loc_nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  nv_status = dsi_get_nv_item( NV_ESN_I, &loc_nv_item);
  if( nv_status == NV_DONE_S)
  {
    ds_esn.esn = loc_nv_item.esn.esn;
  }
  else
  {
#ifdef FEATURE_UIM_RUIM
    ERR( "Bad NV read status %d", nv_status, 0, 0 );
#else
    ERR_FATAL( "Bad NV read status %d", nv_status, 0, 0 );
#endif /* FEATURE_UIM_RUIM */
  }
} /* dsi_nv_init */


/*===========================================================================

FUNCTION DSI_WAIT

DESCRIPTION
  This function suspends execution of the Data Services task until one (or
  more) signals from the requested signal mask is set.

  Watchdog kicking is performed in this function.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask returned by rex_wait().

SIDE EFFECTS
  None

===========================================================================*/

rex_sigs_type  dsi_wait
(
   rex_sigs_type  requested_signal_mask       /* Signal mask to suspend on */
)
{
  rex_sigs_type  set_signals;     /* Signal mask upon return from rex wait */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Wait for a signal to be set.
  -------------------------------------------------------------------------*/
  do
  {
    set_signals = rex_wait( requested_signal_mask | DS_DOG_RPT_TIMER_SIG );

    if( (set_signals & DS_DOG_RPT_TIMER_SIG) != 0 )
    {
      /*---------------------------------------------------------------------
    The watchdog needs a kick. Report back to the watchdog, and then
    restart the timer.
      ---------------------------------------------------------------------*/
      dog_report( DOG_DS_RPT );
      (void)rex_set_timer( &ds_dog_rpt_timer, DOG_DS_RPT_TIME );
      
	  /*---------------------------------------------------------------------
        Processed the DOG_RPT_TIMER_SIG so clear it off in set_signals. 
        Note that DS_TCB is already cleared off this bit with rex_set_timer() 
        call done above.
      ---------------------------------------------------------------------*/
      set_signals &= (~DS_DOG_RPT_TIMER_SIG);
    }
  } while( (set_signals & requested_signal_mask) == 0 );

  return( set_signals );

} /* dsi_wait() */


/*===========================================================================

FUNCTION DSI_PROCESS_CMDS

DESCRIPTION
  This function de-queues commands from the Data Services Task's command
  queue, and dispataches commands to the appropriate entity for further
  processing, if necessary. Commands are de-queued until the command queue is
  empty.

DEPENDENCIES
  This function should be called when the DS_CMD_Q_SIG is set.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsi_process_cmds( void )
{
  ds_cmd_type  *cmd_ptr;                             /* Pointer to command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Get commands from the command queue until the queue is empty. For each
    command received, dispatch the command to the appropriate sub-task.
  -------------------------------------------------------------------------*/
  while( (cmd_ptr = (ds_cmd_type *)q_get( &dsi_cmd_q )) != NULL )
  {

    switch( cmd_ptr->hdr.cmd_id )
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      3G Dsmgr Commands
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case DS_CM_CALL_END_CMD:
      case DS_CM_CALL_INCOM_CMD:
      case DS_CM_CALL_CONNECTED_CMD:
      case DS_CM_CALL_SETUP_CMD:
      case DS_CM_CALL_CONF_CMD:
#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */
      case DS_TIMER_EXPIRED_CMD:
      case DS_COMPLETE_LL_CONNECT_CMD:
      case DS_COMPLETE_LL_DISCONNECT_CMD:
      case DS_INITIATE_CALL_CMD:
      case DS_RELEASE_CALL_CMD:
      case DS_CM_SS_SRV_CHG_CMD:
        ds3g_process_cmds( cmd_ptr );
        break;
      #ifndef FEATURE_DATA_STRIP_ATCOP
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        3G SIOLIB Commands
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case DS_RDM_OPEN_CMD:
      case DS_RDM_CLOSE_CMD:
      case DS_ENTER_ONLINE_CMD_TX_FLUSH_CMD:
      case DS_ENTER_ONLINE_CMD_NO_TX_FLUSH_CMD:
      case DS_ENTER_ONLINE_DATA_TX_FLUSH_CMD:
      case DS_ENTER_ONLINE_DATA_NO_TX_FLUSH_CMD:
      case DS_COMPLETE_ONLINE_CMD_SWITCH_CMD:
      case DS_COMPLETE_ONLINE_DATA_SWITCH_CMD:
        ds3g_siolib_process_cmds( cmd_ptr );
        break;
      #endif

#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_CS)) || \
     (defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD)))
#error code not present
#endif

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        ATCoP Commands
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifndef FEATURE_DATA_STRIP_ATCOP
      case DS_AT_CM_CALL_CMD:
      case DS_AT_CM_CALL_INFO_CMD:
      case DS_AT_TIMER_EXPIRED_CMD:
      case DS_AT_STATUS_CMD:
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
      case DS_AT_PBM_CB_CMD:
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
      case DS_AT_SMS_ERR_CMD:
      case DS_AT_SMS_MSG_CMD:
      case DS_AT_SMS_CFG_CMD:
      case DS_AT_SMS_ABT_CMD:
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

#ifdef FEATURE_DATA_GCSD_FAX
#error code not present
#endif  /* FEATURE_DATA_GCSD_FAX */

#ifdef FEATURE_DATA_ETSI_SUPSERV 
      case DS_AT_CM_SUPS_CMD:
      case DS_AT_CM_SUPS_INFO_CMD:
#endif /* FEATURE_DATA_ETSI_SUPSERV */
#ifdef FEATURE_MMGSDI
      case DS_AT_MMGSDI_INFO_CMD:
#endif /* FEATURE_MMGSDI */
        dsat_process_async_cmd( cmd_ptr );
        break;
#endif /* FEATURE_DATA_STRIP_ATCOP */
#ifdef FEATURE_UIM_SUPPORT_3GPD
      case DS_AT_GSDI_INFO_CMD:
#ifdef FEATURE_DATA_STRIP_ATCOP
        (void)dsatprofile_gsdi_event_handler(DSAT_CMD,cmd_ptr);
#else
        (void)dsatme_gsdi_event_handler(DSAT_CMD,cmd_ptr);
#endif
        break;
#endif /*FEATURE_UIM_SUPPORT_3GPD*/


#ifdef FEATURE_DATA_IS707
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        IS707-PKT Commands
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case DS_707_PKT_PZID_CHANGE_CMD:
      case DS_707_PKT_SID_CHANGE_CMD:
      case DS_707_PKT_NID_CHANGE_CMD:
      case DS_707_PKT_CTA_TIMER_EXPIRED_CMD:
      case DS_707_PKT_HOLDDOWN_TIMER_EXPIRED_CMD:
      case DS_707_PKT_PZID_DELAY_TIMER_EXPIRED_CMD:
      case DS_707_PKT_PZID_HYSTERESIS_TIMER_EXPIRED_CMD:
      case DS_707_PKT_PHYS_LINK_UP_CMD:
      case DS_707_PKT_PHYS_LINK_DOWN_CMD:
      case DS_707_PKT_IFACE_UP_CMD:
      case DS_707_PKT_IFACE_DOWN_CMD:
      case DS_707_PKT_IFACE_DOWN_IND_CBACK_CMD:
      case DS_707_PKT_IFACE_UP_IND_CBACK_CMD:
      case DS_707_PKT_IFACE_ROUTEABLE_IND_CBACK_CMD:
      case DS_707_TOGGLE_QNC_ENABLE_CMD:
      case DS_707_PKT_PZID_HYS_DATA_READY_CMD:
      case DS_707_PKT_PZID_HYS_SDB_DATA_CMD:
        ds707_pkt_process_cmd(cmd_ptr);
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        RMSM IS707-PKT Commands
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
      #ifndef FEATURE_DATA_STRIP_ATCOP
      case DS_707_RMSM_RM_WANTS_PKT_CALL_CMD:
      case DS_707_RMSM_RM_IFACE_DOWN_CMD:
      case DS_707_RMSM_UM_IFACE_DOWN_CMD:
      case DS_707_RMSM_UM_PHYS_LINK_UP_CMD:
      case DS_707_RMSM_UM_PHYS_LINK_DOWN_CMD:
      case DS_707_RMSM_UM_PPP_DOWN_CMD:

#ifdef FEATURE_DS_MOBILE_IP
      case DS_707_RMSM_RM_PPP_UP_CMD:
      case DS_707_RMSM_UM_MIP_UP_CMD:
      case DS_707_RMSM_UM_MIP_DOWN_CMD:
#endif /* FEATURE_DS_MOBILE_IP */ 
 
        ds707_rmsm_process_cmd(cmd_ptr);
        break;
      #endif

#ifndef FEATURE_ASYNC_DATA_NOOP
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        IS707-Async Commands
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case DS_707_ASYNC_IFACE_BRING_UP_CMD:
      case DS_707_ASYNC_IFACE_TEAR_DOWN_CMD:
      case DS_707_ASYNC_PHYS_LINK_TEAR_DOWN_CMD:
      case DS_707_ASYNC_PTCL_OPENING_TIMER_EXPIRED_CMD:
      case DS_707_ASYNC_PTCL_OPENED_CMD:
      case DS_707_ASYNC_PTCL_CLOSED_CMD:
      case DS_707_ASYNC_ATZ_CMD:
        ds707_async_process_cmd(cmd_ptr);
        break;
#endif  /*FEATURE_ASYNC_DATA_NOOP*/

#endif /* FEATURE_DATA_IS707 */
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

      default:
        ERR_FATAL( "Invalid DS task command: %d", cmd_ptr->hdr.cmd_id, 0,0);

    } /* switch */

    /*-----------------------------------------------------------------------
      Return the command buffer to the free command queue.
    -----------------------------------------------------------------------*/
    q_put( &dsi_cmd_free_q, &cmd_ptr->hdr.link );

  } /* while */

} /* dsi_process_cmds() */


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION DS_TASK

DESCRIPTION
  This is the entry point for the Data Services Task. This function contains
  the main processing loop that waits for events (signals or commands) and
  dispatches each event to the appropriate entity for further processing.

DEPENDENCIES
  None

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None

===========================================================================*/

void  ds_task
(
  dword ignored
    /* lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  rex_sigs_type   requested_sigs;   /* Signal mask to suspend on           */
  rex_sigs_type   set_sigs;         /* Signals set upon return from wait   */
  #ifndef FEATURE_DATA_STRIP_ATCOP
  rex_sigs_type   siolib_sigs = 0;  /* SIOLIB signals to suspend on        */
  rex_sigs_type   atcop_sigs  = 0;  /* ATCoP signals to suspend on         */
  #endif
  rex_sigs_type   ucsd_sigs   = 0;  /* UMTS CS Hdlr signals to suspend on */
  rex_sigs_type   wpsd_sigs   = 0;  /* WCDMA PS Hdlr signals to suspend on */
  rex_sigs_type   cdma_sigs   = 0;  /* CDMA sub-task signals to suspend on */
  #ifndef FEATURE_ASYNC_DATA_NOOP
  rex_sigs_type   async707_sigs= 0; /* 707 async signals to suspend on     */
  #endif /* FEATURE_ASYNC_DATA_NOOP */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Do task initialization. The init function performs all the task-level
    initialization.
  -------------------------------------------------------------------------*/
  dsi_task_init();

  /*-------------------------------------------------------------------------
    Initialize timers
  -------------------------------------------------------------------------*/
  ds3gi_timer_init();
  
  #ifndef FEATURE_DATA_STRIP_ATCOP
  /*-------------------------------------------------------------------------
    Initialize SIOLIB
  -------------------------------------------------------------------------*/
  siolib_sigs = ds3g_siolib_init();
  #endif
  
  /*-------------------------------------------------------------------------
    Wait for the task start signal from task controller.
  -------------------------------------------------------------------------*/
  task_start(DS_DOG_RPT_TIMER_SIG, DOG_DS_RPT, &ds_dog_rpt_timer);

  /*-------------------------------------------------------------------------
    Perform sub-task initialization. 
  -------------------------------------------------------------------------*/
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif /*(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */

  /*-------------------------------------------------------------------------
    Each sub-task returns a signal mask containing the signals it wishes 
    to suspend on. 
    Note that ATCoP initialization should always be performed first, 
    since other sub-tasks may use AT parameter values during initialization.
  -------------------------------------------------------------------------*/ 
#ifndef FEATURE_DATA_STRIP_ATCOP
  atcop_sigs = dsat_init();
#else
#ifdef FEATURE_UIM_SUPPORT_3GPD
  dsatprofile_init_me();
#endif /* FEATURE_UIM_SUPPORT_3GPD */
  dsatprofile_nv_sync();
#endif /* FEATURE_DATA_STRIP_ATCOP */

  ds3g_init();

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_CS)) || \
     (defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD)))
#error code not present
#endif

#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_PS)) || \
     (defined(FEATURE_GSM  ) && defined(FEATURE_GSM_GPRS)))
#error code not present
#endif

#if defined(FEATURE_DATA_IS707)
  /*-------------------------------------------------------------------------
    Make sure that Pkt iface is always initialized before Async iface. This
    is because ps_iface assigns instance numbers to iface in sequential 
    order. We want pkt iface to get instance 0...max_pkt_ifaces since Apps 
    call ioctls on pkt iface using those instance numbers.
  -------------------------------------------------------------------------*/
  cdma_sigs = ds707_pkt_mgr_init();
#ifndef FEATURE_ASYNC_DATA_NOOP
  async707_sigs= ds707_async_mgr_powerup_init();
#endif /*FEATURE_ASYNC_DATA_NOOP*/
#endif

  /*-------------------------------------------------------------------------
    Get DS NV Items.
  -------------------------------------------------------------------------*/
  dsi_nv_init();


  /*-------------------------------------------------------------------------
    Signal mask to suspend on is the combination of all the signals requested
    by each of the sub-tasks.
  -------------------------------------------------------------------------*/
  requested_sigs = DS_CMD_Q_SIG        | 
                   DS_TASK_STOP_SIG    | 
                   DS_TASK_OFFLINE_SIG |
                   #ifndef FEATURE_DATA_STRIP_ATCOP
                   siolib_sigs  | 
                   atcop_sigs   |
                   #endif                   
                   ucsd_sigs    |
                   wpsd_sigs    |
                   #ifndef FEATURE_ASYNC_DATA_NOOP
                   async707_sigs |
                   #endif /* FEATURE_ASYNC_DATA_NOOP */
                   cdma_sigs;                                     
                   

  /*-------------------------------------------------------------------------
    Main task loop, never exits.
  -------------------------------------------------------------------------*/
  for( ;; )
  {
    /*----------------------------------------------------------------------
      Wait for one of the specified signals to be set. Note that watchdog
      kicking is performed in the wait.
    -----------------------------------------------------------------------*/
    set_sigs = dsi_wait( requested_sigs );
	
	/*----------------------------------------------------------------------
	We used to individually clear the wrong set of signals and some signals
	were getting lost.  Here, we clear ds_tcb with set_sigs.  set_sigs is
	not altered.
	----------------------------------------------------------------------*/
    (void)rex_clr_sigs( &ds_tcb, set_sigs );

    /*----------------------------------------------------------------------
      If any of the task signals were received, invoke the function to ACK
      task conroller.
    -----------------------------------------------------------------------*/
    if( (set_sigs & DS_TASK_STOP_SIG) != 0 )
    {
      task_stop();
    }

    if( (set_sigs & DS_TASK_OFFLINE_SIG) != 0 )
    {
      task_offline();
    }
    /*----------------------------------------------------------------------
      If the command queue signal was set, clear the signal and invoke the
      function that dispatches commands to the appropriate sub-task.
    -----------------------------------------------------------------------*/
    if( (set_sigs & DS_CMD_Q_SIG) != 0 )
    {
      dsi_process_cmds();
    }

    /*----------------------------------------------------------------------
      If any of the 3G SIOLIB signals were set, clear the signals and invoke
      a function to process the signals.
    -----------------------------------------------------------------------*/
    #ifndef FEATURE_DATA_STRIP_ATCOP
    if( (set_sigs & siolib_sigs) != 0 )
    {
      ds3g_siolib_process_signals( set_sigs );
    }

    /*----------------------------------------------------------------------
      If any of the ATCoP signals were set, clear the signals and invoke a
      function to process the signals.
    -----------------------------------------------------------------------*/
    if( (set_sigs & atcop_sigs) != 0 )
    {
      dsat_process_async_signal( set_sigs );
    }
    #endif

#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_CS)) || \
     (defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD)))
#error code not present
#endif

#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_PS)) ||\
     (defined(FEATURE_GSM  ) && defined(FEATURE_GSM_GPRS)))
#error code not present
#endif

#if defined(FEATURE_DATA_IS707)
    /*----------------------------------------------------------------------
      If any of the WCDMA CS Hdlr signals were set, clear the signals and
      invoke a function to process the signals.
    -----------------------------------------------------------------------*/
    if( (set_sigs & cdma_sigs) != 0 )
    {
      ds707_pkt_process_signals( set_sigs );
    }

    /*----------------------------------------------------------------------
      If any of the WCDMA CS Hdlr signals were set, clear the signals and
      invoke a function to process the signals.
    -----------------------------------------------------------------------*/
#ifndef FEATURE_ASYNC_DATA_NOOP
    if( (set_sigs & async707_sigs) != 0 )
    {
      ds707_async_process_signals( set_sigs );
    }
#endif /* FEATURE_ASYNC_DATA_NOOP */
#endif

  } /* forever */
} /* ds_task() */


/*===========================================================================

FUNCTION DS_GET_CMD_BUF

DESCRIPTION
  This function allocates a command buffer from the Data Services Task. The
  The caller of this function must check for a NULL return value, and either
  perform some error recovery or ERR_FATAL, as appropriate.

DEPENDENCIES
  None

RETURN VALUE
  Returns a pointer to a command buffer, if available, or NULL if no command
  buffers are available.

SIDE EFFECTS
  None

===========================================================================*/

ds_cmd_type  *ds_get_cmd_buf( void )
{
  ds_cmd_type    *cmd_ptr;                           /* Pointer to command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Get a command buffer from the free command queue.
  -------------------------------------------------------------------------*/
  if( (cmd_ptr = q_get( &dsi_cmd_free_q )) == NULL )
  {
    /*-----------------------------------------------------------------------
      No free command buffers available, log an error.
    -----------------------------------------------------------------------*/
    ERR( "No items on DS Task free cmd q", 0, 0, 0 );
  }

  /*-------------------------------------------------------------------------
    Note that the cmd_ptr may be NULL if there were no free command buffers
    available. The calling task must either handle the NULL return value
    or ERR_FATAL.
  -------------------------------------------------------------------------*/
  return( cmd_ptr );

} /* ds_get_cmd_buf() */


/*===========================================================================

FUNCTION DS_PUT_CMD

DESCRIPTION
  This function puts a command on the Data Services Task's command queue, and
  sets the DS_CMD_Q_SIG.

DEPENDENCIES
  The caller of this function should have previously allocated a command
  buffer by calling ds_get_cmd_buf().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds_put_cmd
(
  ds_cmd_type    *cmd_ptr                 /* Pointer to command to be sent */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Put the command on the command queue, and set the command queue signal.
  -------------------------------------------------------------------------*/
  q_put( &dsi_cmd_q, &cmd_ptr->hdr.link );
  (void)rex_set_sigs( &ds_tcb, DS_CMD_Q_SIG );

} /* ds_put_cmd() */



/*===========================================================================
FUNCTION DSI_GET_NV_ITEM 

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.

DEPENDENCIES
  NV task is expected to be available.  Must run in the DS task context.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsi_get_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
)
{
  ASSERT(rex_self() == &ds_tcb);
  nv_command.tcb_ptr = rex_self();    /* notify this task when done */
  nv_command.sigs = DS_NV_CMD_SIG;
  nv_command.done_q_ptr = NULL;       /* command goes on no queue when done */

  nv_command.item = item;             /* item to read */
  nv_command.cmd = NV_READ_F;

  /* Set up NV so that it will read the data into the correct location */
  nv_command.data_ptr = data_ptr;


  /* Clear the return signal, call NV, and wait for a response */
  (void) rex_clr_sigs( rex_self(), DS_NV_CMD_SIG );
  nv_cmd( &nv_command );
  (void) dsi_wait( DS_NV_CMD_SIG );

  /*-------------------------------------------------------
  Report error on return code.
  ---------------------------------------------------------*/
  if(( nv_command.status != NV_DONE_S ) &&
     ( nv_command.status != NV_NOTACTIVE_S))                                     
  {
    ERR( "NV Read Failed Item %d Code %d", 
         nv_command.item, nv_command.status, 0 );
  }

  return( nv_command.status );
}



/*===========================================================================
FUNCTION DSI_PUT_NV_ITEM 

DESCRIPTION
  Put an item into the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.  Must run in the DS task context.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsi_put_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
)
{
  ASSERT(rex_self() == &ds_tcb);

  nv_command.tcb_ptr = rex_self();    /* notify this task when done */
  nv_command.sigs = DS_NV_CMD_SIG;
  nv_command.done_q_ptr = NULL;       /* command goes on no queue when done */

  nv_command.item = item;             /* item to write */
  nv_command.cmd = NV_WRITE_F;

  /* Set up NV so that it will write the data into the correct location */
  nv_command.data_ptr = data_ptr;


  /* Clear the return signal, call NV, and wait for a response */
  (void) rex_clr_sigs( rex_self(), DS_NV_CMD_SIG );
  nv_cmd( &nv_command );
  (void) dsi_wait( DS_NV_CMD_SIG );

  /*-------------------------------------------------------
  Report error on return code.
  ---------------------------------------------------------*/
  if( nv_command.status != NV_DONE_S ) 
  {
    ERR( "NV Write Failed Item %d Code %d", 
         nv_command.item, nv_command.status, 0 );
  }

  return( nv_command.status );
}

#endif /* FEATURE_DATA */
