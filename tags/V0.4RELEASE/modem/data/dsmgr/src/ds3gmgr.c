/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             3 G   D A T A   S E R V I C E S   M A N A G E R

GENERAL DESCRIPTION
  This software unit contains routines to initialize the 3G Dsmgr sub-task,
  and to process received signals and commands. Also contains other misc.
  functions for setup/cleanup of Autodetect watermarks and mode-specific
  handler registration.

EXTERNALIZED FUNCTIONS
  ds3g_init()
    Initializes the 3G Dsmgr sub-task

  ds3g_process_cmds()
    Processes 3G Dsmgr commands

  ds3g_register_mode_sp_handler()
    Called by mode-specific handlers to register a handler function table
    for a specified mode

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds3g_rdm_init() and ds3g_init() must be called before any other 3G Dsmgr
  functions are invoked.  ds3g_rdm_init() must be called before calling
  the TMC function to start the data services task.


  Copyright (c) 2001-2008 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gmgr.c_v   1.7   28 Feb 2003 18:54:08   rchar  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/src/ds3gmgr.c#3 $ $DateTime: 2008/04/11 05:12:17 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/08   sn      Fixed klockwork errors.
01/28/08   sn      Featurized WCDMA code.
11/01/06   an      Featurize ATCOP, RMSM, Async Fax
06/02/03   snb     Downgrade "Invalid call Id" msg ERR to MSG_MED in 
                   ds_get_current_session_data_counters. Called repeatedly 
                   during +CGACT orig'd PS data call.
02/26/03   rc      Changed DS_CM_SS_SYS_MODE_CHG_CMD to DS_CM_SS_SRV_CHG_CMD.
                   Changed ds3gi_process_cm_ss_sys_mode_chg_cmd() to 
                   ds3gi_process_cm_ss_srv_chg_cmd(). 
11/04/02   tmr     Added clearing of call_was_connected field in call info
                   structure.
10/14/02   vsk     Added support to get statistics for the call which has ended
08/19/02   ak      Updated for multiple calls.
08/13/02   dgy     Added return value to ds_get_call_information() and
                   ds_get_current_session_data_counters().
08/02/02   dgy     Added support for UI data call screen.
07/19/02   smp     Fix to ensure that the call is ended if DTR drops during
                   Data call setup.
06/18/02   tmr     Added ds3gi_process_msh_complete_ll_disconnect_cmd() handler.
05/10/02   ak      Added 707 signal processing.  Also removed some old
                   SIO & AT stuff.
04/19/02   vsk     Added PDP context preservation support
04/12/02   rc      Added DS3G_INITIATE_CALL_CMD event in ds3g_process_cmds().                 
03/15/02   smp     In ds3g_init(), initialize end_result state variable to
                   DSAT_NO_CARRIER.
02/26/02   tmr     Added handlers ds3gi_process_msh_complete_ll_connect_cmd() 
                   and ds3gi_process_msh_release_call_cmd().                   
02/22/02   smp     Added variable to store mode-sp. handler sio_rx_func_ptr
                   (to allow for switching between online cmd/data). Added
                   handling for aborting AT commands.
02/19/02   rlp     Code review changes.
02/07/02   vsk     Added ds3gi_get_current_network_mode() function 
02/05/02   rlp     Added online-command mode support.
01/02/02   tmr     Added conditional DS3G_GSM_MODE hard coding for GSM builds 
12/14/01   smp     Initialize the ring_counter state variable in ds3g_init().
10/31/01   smp     Added error handling for 1ST_SIO_RX_SIG being set in an
                   invalid state.
08/17/01   smp     Renamed ATCOP files/functions/data types.
06/05/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "assert.h"
#include "cm.h"
#include "ds3gcmif.h"
#include "ds3gmgr.h"
#include "ds3gmshif.h"
#include "ds3gtimer.h"
#include "ds3gmgrint.h"
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "ds3grdmif.h"
#include "dsat.h"
#endif
#include "dsm.h"
#include "dstask.h"
#include "err.h"
#include "msg.h"
#include "queue.h"
#include "rex.h"
#include "sys.h"
#include "task.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Table of function ptrs based on Mode (GSM/WCDMA/CDMA) and call type (pkt
  or circuit-switched)
---------------------------------------------------------------------------*/
ds3g_hdlr_func_tbl_type    *ds3gi_hdlr_tbl[SYS_SYS_MODE_MAX]
                                          [DS_NUM_CALL_TYPES];

/*---------------------------------------------------------------------------
  Call state info.  
---------------------------------------------------------------------------*/
ds3gi_call_info_type   ds3gi_call_info[DS3GI_MAX_NUM_CALLS];

/*---------------------------------------------------------------------------
  The current operating mode.
---------------------------------------------------------------------------*/
sys_sys_mode_e_type    ds3gi_mode;

/*---------------------------------------------------------------------------
  Client id assigned by Call Manager.
---------------------------------------------------------------------------*/ 
cm_client_id_type  ds3gi_cm_client_id;

/*---------------------------------------------------------------------------
  The operating mode the for the previous call 
---------------------------------------------------------------------------*/ 
sys_sys_mode_e_type     ds3gi_last_call_mode; 

/*---------------------------------------------------------------------------
  The call type for the previous call 
---------------------------------------------------------------------------*/ 
ds_call_type_e_type     ds3gi_last_call_type;  


/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION DS3G_INIT

DESCRIPTION
  This function performs power-up initialization of 3G Dsmgr. This includes
  initialization of some state variables and registration with Call Manager
  for handling of incoming/outgoing data calls.

  This function must be called once at data services task startup.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void  ds3g_init( void )
{
  uint8          i, j;                             /* Loop index variables */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Mode is hard-coded to GSM or WCDMA, for now.
  -------------------------------------------------------------------------*/
#if defined(FEATURE_GSM)
#error code not present
#elif defined(FEATURE_WCDMA)
#error code not present
#elif defined(FEATURE_DATA_IS707)
  ds3gi_mode = SYS_SYS_MODE_CDMA;
#elif
  #error Need one mode feature.
#endif
  #ifndef FEATURE_DATA_STRIP_ATCOP 
  dsat_init_service_mode(ds3gi_mode);
  #endif
  /*-------------------------------------------------------------------------
    Initialize the handler function table pointers to NULL.
  -------------------------------------------------------------------------*/
  for( i = 0; i < SYS_SYS_MODE_MAX; i++ )
  {
    for( j = 0; j < DS_NUM_CALL_TYPES; j++ )
    {
      ds3gi_hdlr_tbl[i][j] = NULL;
    }
  }

  /*-------------------------------------------------------------------------
    Explicitly initialize the 
  -------------------------------------------------------------------------*/
  for (i =0; i < DS3GI_MAX_NUM_CALLS; i++)
  {
    ds3gi_call_info[i].call_id    = CM_CALL_ID_INVALID;
    ds3gi_call_info[i].call_state = DS3G_IDLE_CALL_STATE;
    ds3gi_call_info[i].call_was_connected = FALSE;
  }

  /*-------------------------------------------------------------------------
    Register as a Call Manager client, and register for call events.
  -------------------------------------------------------------------------*/
  ds3gi_cm_init();

  /*-------------------------------------------------------------------------
    Initialize the last call's mode and type. Type is set to Packet. 
    This will be updated at Run time
  -------------------------------------------------------------------------*/
  ds3gi_last_call_mode = ds3gi_mode;
  ds3gi_last_call_type = DS_CALL_TYPE_PKT;

} /* ds3g_init() */

/*===========================================================================

FUNCTION DS3G_PROCESS_CMDS

DESCRIPTION
  This function processes 3G Dsmgr commands. A command processing function is
  called based on the type of command received.

  This function should be called by the top-level task dispatcher whenever
  any 3G Dsmgr command is received.

DEPENDENCIES
  3G Dsmgr should have been initialized prior to calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_process_cmds
(
  ds_cmd_type  *cmd_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Invoke the appropriate function to process the command, based on the
    command id.
  -------------------------------------------------------------------------*/
  switch( cmd_ptr->hdr.cmd_id )
  {
    case DS_CM_CALL_END_CMD:
      ds3gi_process_cm_call_end_cmd( cmd_ptr );
      break;

    case DS_CM_CALL_INCOM_CMD:
      ds3gi_process_cm_call_incom_cmd( cmd_ptr );
      break;

    case DS_CM_CALL_CONNECTED_CMD:
      ds3gi_process_cm_call_connected_cmd( cmd_ptr );
      break;

    case DS_CM_CALL_SETUP_CMD:
      ds3gi_process_cm_call_setup_cmd( cmd_ptr );
      break;

    case DS_CM_CALL_CONF_CMD:
      ds3gi_process_cm_call_conf_cmd( cmd_ptr );
      break;

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

    case DS_TIMER_EXPIRED_CMD:
      ds3gi_process_timer_expired_cmd( cmd_ptr );
      break;
      
    case DS_CM_SS_SRV_CHG_CMD:
      ds3gi_process_cm_ss_srv_chg_cmd( cmd_ptr );
      break;

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */
    case DS_COMPLETE_LL_CONNECT_CMD:
    case DS_COMPLETE_LL_DISCONNECT_CMD:
    case DS_RELEASE_CALL_CMD:
    case DS_INITIATE_CALL_CMD:
    default:
      ERR( "3G Dsmgr rx'ed unrecognized cmd: %d", cmd_ptr->hdr.cmd_id, 0, 0 );
      break;

  } /* switch( cmd_id ) */
  
} /* ds3g_process_cmds() */


/*===========================================================================

FUNCTION DS3G_REGISTER_MODE_SP_HANDLER

DESCRIPTION
  This function registers a mode-specific handler function table, for the
  specified mode and call type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If a mode-specific handler function table was previously registered for the
  specified mode and call type, it will be over-written.

===========================================================================*/

void  ds3g_register_mode_sp_handler
(
  sys_sys_mode_e_type             mode,        /* Network mode             */
  ds_call_type_e_type             call_type,   /* Call type-Circuit or pkt */
  ds3g_hdlr_func_tbl_type        *hdlr_tbl_ptr /* Handler function table   */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( mode == SYS_SYS_MODE_NONE )
  {
    ERR("Couldn't register mode specific hdlr - System Mode NONE", 0,0,0);
    return;
  }

  ds3gi_hdlr_tbl[mode][call_type] = hdlr_tbl_ptr;

} /* ds3g_register_mode_sp_handler() */

/*===========================================================================

FUNCTION DS3G_GET_CURRENT_NETWORK_MODE

DESCRIPTION
  This function returns the current system that we are camped on
  
DEPENDENCIES
  None

RETURN VALUE
  Returns the 3G Dsmgr network mode type

SIDE EFFECTS
  None

===========================================================================*/

sys_sys_mode_e_type  ds3g_get_current_network_mode( void )
{
  return ds3gi_mode;
} /* ds3g_get_current_network_mode() */


/*===========================================================================

FUNCTION DS3G_GET_CALL_STATE

DESCRIPTION
  This function returns the current call state.
  
DEPENDENCIES
  None

RETURN VALUE
  See above.

SIDE EFFECTS
  None

===========================================================================*/

ds3gi_call_state_e_type  ds3g_get_call_state
(
  cm_call_id_type    cm_call_id
)
{
  uint32 call_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_inst = ds3gi_find_call_instance(cm_call_id);

  if (call_inst >= DS3GI_MAX_NUM_CALLS)   /* Klockwork fix */
  {
    MSG_HIGH("Invalid call id for call state",0,0,0);
    return (DS3G_INVALID_CALL_STATE);
  }
  else
  {
    return(ds3gi_call_info[call_inst].call_state);
  }
} /* ds3g_get_call_state() */


/*===========================================================================

FUNCTION DS_GET_CALL_INFORMATION

DESCRIPTION
  This function should be invoked by UI for displaying data call information.
  The function returns by reference the following information:
  
  *system_information - A NULL-terminated string indicating the system or
                        service option, e.g., "WCDMA", for the call.
                        
  This function checks the size of the buffer for *system_information before
  it populates the buffer.
  
  Currently, UI retrieves the TX/RX maximum bit rates of the call from the
  QoS IE that comes with the CM CALL_CONNECTED event.

DEPENDENCIES
  None

RETURN VALUE
  This function returns the data statistics type, indicating if the information
  passed by reference, i.e., *system_information, is valid.

SIDE EFFECTS
  None

===========================================================================*/

ds_stats_e_type  ds_get_call_information
(
  cm_call_id_type     call_id,                                  /* Call ID */
  byte               *system_information,            /* System information */
  byte                max_system_information_length  /* Maximum length of
                                                        system information */
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;    /* Ptr to mode-sp. hdlr table */
  uint32                    call_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW( "Get call connection information",0,0,0);

  /*-------------------------------------------------------------------------
    Check if the call ID passed in matches the call ID stored for the current
    call.
  -------------------------------------------------------------------------*/
  call_inst = ds3gi_find_call_instance(call_id);

  if( (call_inst >= DS3GI_MAX_NUM_CALLS) || (ds3gi_mode == SYS_SYS_MODE_NONE) )
  {
    /*-----------------------------------------------------------------------
      Call ID is invalid.
    -----------------------------------------------------------------------*/
    ERR( "Invalid call id: %d or System Mode NONE", call_id, 0, 0 );

    return ( DS_STATS_INVALID_CALL_ID );
  }

  /*-------------------------------------------------------------------------
    Point to the appropriate handler function table based on the current
    mode and call type.
  -------------------------------------------------------------------------*/
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode]
                               [ds3gi_call_info[call_inst].call_type];

  /*-------------------------------------------------------------------------
    Ensure that a handler function table is registered, with a non-null
    get_call_connection_info_handler.
  -------------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if( hdlr_tbl_ptr->get_call_connection_info_handler != NULL )
    {
      if ( hdlr_tbl_ptr->get_call_connection_info_handler( 
                            call_id,
                            system_information,
                            max_system_information_length ) == DS3G_SUCCESS )
      {
        /*-------------------------------------------------------------------
          Call connection information is valid.
        -------------------------------------------------------------------*/
        return ( DS_STATS_VALID );
      }
      else
      {
        /*-------------------------------------------------------------------
          Call connection information is not available.
        -------------------------------------------------------------------*/
        return ( DS_STATS_UNAVAILABLE );
      }
    }
    else
    {
      ERR( "Call conn info handler not registered", 0,0,0 );

      return ( DS_STATS_UNAVAILABLE );
    }
  } /* if hdlr_tbl_ptr != NULL */
  else
  {
    ERR( "Handler function table not registered", 0, 0, 0 );

    return ( DS_STATS_UNAVAILABLE );
  }

} /* ds_get_call_information() */


/*===========================================================================

FUNCTION DS_GET_CURRENT_SESSION_DATA_COUNTERS

DESCRIPTION
  This function should be invoked by UI for displaying data call statistics.
  It returns by reference the total count of bytes transmitted and received 
  at RLC for the current call:
  
  *rx_from_network_byte_cnt - Total count of bytes received for the call
  *tx_to_network_byte_cnt   - Total count of bytes transmitted for the call.
  
  The total byte count is accumulative for the call. If RAB is 
  re-established after PDP context is preserved during the call, the total
  byte count includes bytes transmitted and received after RAB is 
  re-established.

DEPENDENCIES
  None

RETURN VALUE
  This function returns the data statistics type, indicating if the information
  passed by reference, e.g., *rx_from_network_byte_cnt, is valid.

SIDE EFFECTS
  None

===========================================================================*/

ds_stats_e_type  ds_get_current_session_data_counters 
(
  cm_call_id_type     call_id,                                  /* Call ID */
  uint32             *rx_from_network_byte_cnt,       /* RX byte count for 
                                                                  the call */
  uint32             *tx_to_network_byte_cnt          /* TX byte count for
                                                                  the call */
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;    /* Ptr to mode-sp. hdlr table */
  uint32                    call_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW( "Get current session data counters",0,0,0);

  /*-------------------------------------------------------------------------
    Check if the call ID passed in matches the call ID stored for the current
    call.
  -------------------------------------------------------------------------*/
  call_inst = ds3gi_find_call_instance(call_id);

  if( (call_inst >= DS3GI_MAX_NUM_CALLS) || (ds3gi_mode == SYS_SYS_MODE_NONE) )
  {
    /*-----------------------------------------------------------------------
      Call ID is invalid.
    -----------------------------------------------------------------------*/
    MSG_MED( "Invalid call id: %d or System Mode NONE", call_id, 0, 0 );

    return ( DS_STATS_INVALID_CALL_ID );
  }

  /*-------------------------------------------------------------------------
    Point to the appropriate handler function table based on the current
    mode and call type.
  -------------------------------------------------------------------------*/
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode]
                               [ds3gi_call_info[call_inst].call_type];

  /*-------------------------------------------------------------------------
    Ensure that a handler function table is registered, with a non-null
    get_data_counters_handler.
  -------------------------------------------------------------------------*/
  if( ( hdlr_tbl_ptr != NULL ) && 
      ( hdlr_tbl_ptr->get_data_counters_handler != NULL ) )
  {
    if ( hdlr_tbl_ptr->get_data_counters_handler( 
                                   call_id,
                                   rx_from_network_byte_cnt,
                                   tx_to_network_byte_cnt ) == DS3G_SUCCESS )
    {
      /*---------------------------------------------------------------------
        Data count is valid.
      ---------------------------------------------------------------------*/
      return ( DS_STATS_VALID );
    }
    else
    {
      /*---------------------------------------------------------------------
        Data count is not available.
      ---------------------------------------------------------------------*/
      return ( DS_STATS_UNAVAILABLE );
    }
  }
  else
  {
    return ( DS_STATS_UNAVAILABLE );
  }

} /* ds_get_current_session_data_counters() */


/*===========================================================================

FUNCTION DS_GET_LAST_SESSION_DATA_COUNTERS
DESCRIPTION
  This function should be invoked by UI for retrieving the data call
  statistics at the end of the call. It returns the total count of bytes 
  transmitted and received at link layer for the call which terminated:
  
  *rx_from_network_byte_cnt - Total count of bytes received for the call
  *tx_to_network_byte_cnt   - Total count of bytes transmitted for the call.
  
  The total byte count is accumulative for the call. If RAB is 
  re-established after PDP context is preserved during the call, the total
  byte count includes bytes transmitted and received after RAB is 
  re-established.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - A valid set of counters are returned
  FALSE - An error occured and the counters returned are not valid

SIDE EFFECTS
  None

===========================================================================*/

boolean  ds_get_last_session_data_counters 
(
  uint32             *rx_from_network_byte_cnt,       /* RX byte count for 
                                                                  the call */
  uint32             *tx_to_network_byte_cnt          /* TX byte count for
                                                                  the call */
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;    /* Ptr to mode-sp. hdlr table */
  boolean result=FALSE;
  uint32  call_inst = DS3GI_DEFAULT_CALL_INSTANCE;   
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW( "Get last session data counters",0,0,0);

  /*-------------------------------------------------------------------------
    DS just returns the counter values for the last call which terminated 
    or the current call (which could be in the process of being released)
  -------------------------------------------------------------------------*/
  
  /*-------------------------------------------------------------------------
    If at this point, we are in a call ( any call state ),then call the 
    call's regular statistics retrieval handler.  
    If we are not in a call, then use the last_call_mode and last_call_type 
    and call the last_session_handler for that mode & call type
  -------------------------------------------------------------------------*/
  switch ( ds3gi_call_info[call_inst].call_state )
  {
  case DS3G_IDLE_CALL_STATE:
    /*-------------------------------------------------------------------------
     Not in a call: Call the last_session_data_counters handler for the last 
     known mode & call type
   -------------------------------------------------------------------------*/
    if( ds3gi_last_call_mode == SYS_SYS_MODE_NONE )
    {
      ERR("Last session data counters - System mode NONE", 0,0,0);
      return FALSE;
    }

    hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_last_call_mode]
                                 [ds3gi_last_call_type];
    if( ( hdlr_tbl_ptr != NULL ) && 
        ( hdlr_tbl_ptr->get_last_session_data_counters_handler != NULL ) )
    { 
      result= hdlr_tbl_ptr->get_last_session_data_counters_handler( 
                                   rx_from_network_byte_cnt,
                                   tx_to_network_byte_cnt );
    }
    break;
  default:
   /*-------------------------------------------------------------------------
     If this function is called in a state other than IDLE, call the 
     handler function to return the current session counters. 
   -------------------------------------------------------------------------*/
   if( ds3gi_mode == SYS_SYS_MODE_NONE )
   {
     ERR("Last session data counters - System mode NONE", 0,0,0);
     return FALSE;
   }

   hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode]
                               [ds3gi_call_info[call_inst].call_type];

   if( ( hdlr_tbl_ptr != NULL ) && 
       ( hdlr_tbl_ptr->get_data_counters_handler != NULL ) )
    {
      result= hdlr_tbl_ptr->get_data_counters_handler( 
                                   ds3gi_call_info[call_inst].call_id,
                                   rx_from_network_byte_cnt,
                                   tx_to_network_byte_cnt );
    }
  }
  
  return result;  
} /* ds_get_last_session_data_counters() */

#endif /* FEATURE_DATA */
