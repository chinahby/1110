/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               3 G   D A T A   S E R V I C E S   M A N A G E R

       C A L L   M G R   I N T E R F A C E   F U N C T I O N S

GENERAL DESCRIPTION
  This software unit contains functions for interfacing to Call Manager.

EXTERNALIZED FUNCTIONS
  ds3gi_cm_init()
    Performs Call Manager client registration

  ds3gi_cm_call_cmd_cb()
    Callback function for Call Manager call commands

  ds3gi_process_cm_call_end_cmd()
    Processes the DS_CM_CALL_END_CMD

  ds3gi_process_cm_call_incom_cmd()
    Processes the DS_CM_CALL_INCOM_CMD

  ds3gi_process_cm_call_connected_cmd()
    Processes the DS_CM_CALL_CONNECTED_CMD

  ds3gi_process_cm_call_setup_cmd()
    Processes the DS_CM_CALL_SETUP_CMD

  ds3gi_process_cm_call_conf_cmd()
    Processes the DS_CM_CALL_CONF_CMD

  ds3gi_process_cm_call_rab_rel_cmd()
    Processes the DS_CM_RAB_REL_CMD

  ds3gi_process_cm_call_rab_reestab_ind_cmd()
    Processes the DS_CM_RAB_REESTAB_IND_CMD

  ds3gi_process_cm_call_rab_reestab_rej_cmd()
    Processes the DS_CM_RAB_REESTAB_REJ_CMD

  ds3gi_process_cm_call_rab_reestab_fail_cmd()
    Processes the DS_CM_RAB_REESTAB_FAIL_CMD    

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds3gi_cm_init() must be called once at startup, to register as a Call
  Manager client.


  Copyright (c) 2001-2008 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gcmif.c_v   1.11   28 Feb 2003 18:52:50   rchar  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/src/ds3gcmif.c#4 $ $DateTime: 2008/06/23 04:38:29 $ $Author: parmjeet $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/08   psng    Fixed compiler warnings
04/08/08   sn      Fixed klockwork errors.
01/28/08   sn      Featurized WCDMA code.
11/01/06   an      Featurize ATCOP, RMSM, Async Fax
05/21/03   vas     Added case from processing a return value of Ignore from
                   an incoming call handler (needed for HDR)
05/21/03   atp     Added fix to protect DS from CP trying to set up a 2nd
                   data call when one data call already active in SVD.
04/18/03   ak      Better msgs on incoming call.  Added param to switch_calls
                   interface
04/09/03   rsl     Changes relating to CM ret. val check.
03/18/03   ak      On incoming call, fixed bug where call_type was not pulled
                   from cmd_ptr, but instead referenced from un-inited array.
02/26/03   rc      Added code to process the ps_data_suspend flag received in 
                   the CM_SS_EVENT_SRV_CHANGED Event from Call Manager. 
02/12/03   wx      Accommodate cm_mm_call_cmd_end api change and 
                   cm_end_params_s_type struct change
02/10/02   ak      Handle all potential sys_mode changes correctly.
02/07/02   ak      Quick hack to handle transitions to AMPS mode.
11/04/02   tmr     Added call_was_connected paramter to call_ended_handler.  
                   Also added setting/clearing of call_was_connected field
                   of call info structure.
10/14/02   ak      Added client_id to stored information on CM event cback.
10/14/02   vsk     Store the system mode & call type for an incoming call 
                   to be used after the call has ended to get stats
08/19/02   ak      Updated for multiple calls.
07/24/02   atp     Made ALERT_USER a valid return value from mode-specific
                   handler in ds3gi_process_cm_call_incom_cmd().
07/22/02   ak      on incoming call, DO_NOTHING bumps up the call state to
                   WAITING_FOR_CONNECT
07/16/02   ak      Incoming call handler also takes cmd_ptr.
07/01/02   ak      When originating a call, need mode-spec hdlr to fill in
                   the cm_srv_type.
06/05/02   ar      Removed online-command mode SIO watermark preservation 
                   variables.
06/05/02   smp     Update ATCOP's service mode whenever the system mode changes.
05/21/02   smp     Added functions to register for and process the system
                   mode change event from CM.
05/16/02   smp     Updates for Multimode CM.
05/10/02   ak      Added ds3g_initiate_call and ds3g_hangup_call.
05/03/02   smp     When a call is ended, CD winking is done only if the call
                   state was ACTIVE, and no result code is returned if the
                   call state was RINGING. Also fixed a merge issue.
04/19/02   vsk     Added PDP context preservation support
04/17/02   rc      Fixed bug in ds3gi_cm_call_event_cb() where the cmd buffer
                   was not freed in case of invalid call events being recvd.
                   Added checks for call stack type.                   
03/15/02   smp     Reset the end_result state variable to NO CARRIER in
                   ds3gi_process_cm_call_end_cmd().
03/14/02   smp     The result code returned to the TE when a Data call is
                   ended is now based on a state variable (was previously
                   hard-coded to NO CARRIER).
03/05/02   smp     Added support for CD winking (&C2).
02/26/02   tmr     Added code to handle delay_connect and fixed bug of checking
                   wrong handler function pointer in 
                   ds3gi_process_cm_call_connected_cmd()
02/22/02   smp     Save mode-sp. handler sio_rx_func_ptr (to allow for
                   switching between online cmd/data).
02/19/02   rlp     Code review changes.
02/05/02   rlp     Added online-command mode support.
01/11/02   smp     Merged from packet data branch. CM API updates.
12/18/01   smp     Send NO CARRIER result code to the TE when a Data call is
                   ended.
12/14/01   smp     Fix to stop alerting the user when an incoming call is
                   released. Renamed ds3g_ring_serial_port().
11/06/01   smp     Tx watermark was not being registered when switching back
                   to Autodetect upon ending a call.
08/31/01   smp     Updates due to CM name changes.
08/17/01   smp     Renamed ATCOP files/functions/data types.
06/05/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "msg.h"
#include "err.h"
#include "cm.h"
#include "sio.h"
#include "dsm.h"
#include "dstask.h"
#include "ds3gmgr.h"
#include "ds3gmgrint.h"
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "ds3gsiolib.h"
#include "dsat.h"
#endif
#ifdef FEATURE_DATA_IS707
#include "ds707_pkt_mgr.h"
#include "ds707_p_rev.h"
#endif
#include "ds3gcmif.h"



/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION      DS3GI_CM_CALL_EVENT_CB

DESCRIPTION  Callback function invoked by Call Manager to notify 3G Dsmgr
             whenever a call event occurs that 3G Dsmgr registered for.

             This function sends a command to the DS task, depending on the
             type of call event that occurred.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_cm_call_event_cb
(
  cm_call_event_e_type         call_event,  /* The call event that occurred*/
  const cm_mm_call_info_s_type *call_info_ptr          /* Call information */
)
{
  ds_cmd_type  *cmd_ptr;                      /* Pointer to command buffer */
  ds_cmd_enum_type cmd_id;                    /* Cmd to send to DS task    */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    We only pay attention to call events related to Data calls, and ignore
    all others.
  -------------------------------------------------------------------------*/
  if( (call_info_ptr->call_type == CM_CALL_TYPE_CS_DATA) ||
      (call_info_ptr->call_type == CM_CALL_TYPE_PS_DATA) )
  {
    /*---------------------------------------------------------------------
      Send the appropriate command to the DS Task based on the type of
      event received.
    ---------------------------------------------------------------------*/
    switch( call_event )
    {
      case CM_CALL_EVENT_END:
        cmd_id = DS_CM_CALL_END_CMD;
        break;

      case CM_CALL_EVENT_INCOM:
        cmd_id = DS_CM_CALL_INCOM_CMD;
        break;

      case CM_CALL_EVENT_CONNECT:
        cmd_id = DS_CM_CALL_CONNECTED_CMD;
        break;

      case CM_CALL_EVENT_SETUP_IND:
        cmd_id = DS_CM_CALL_SETUP_CMD;
        break;

      case CM_CALL_EVENT_CALL_CONF:
        cmd_id = DS_CM_CALL_CONF_CMD;
        break;

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

      default:
        MSG_HIGH( "Rx'ed unknown call event: %d", call_event, 0, 0 );
        return;
    }

    if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
    {
      ERR_FATAL( "CM can't get cmd buf from DS task", 0, 0, 0 );
    }
    else
    {
      /*---------------------------------------------------------------------
        Fill in the command parameters and send the command.
      ---------------------------------------------------------------------*/
      cmd_ptr->hdr.cmd_id = cmd_id;
      cmd_ptr->cmd.call_info.call_id    = call_info_ptr->call_id;
      cmd_ptr->cmd.call_info.client_id  = call_info_ptr->call_client_id;
      cmd_ptr->cmd.call_info.call_type  = call_info_ptr->call_type;
      cmd_ptr->cmd.call_info.mode_info  = call_info_ptr->mode_info;
      cmd_ptr->cmd.call_info.end_status = call_info_ptr->end_status;

      ds_put_cmd( cmd_ptr );
    }
  }

} /* ds3gi_cm_call_event_cb() */

/*===========================================================================
FUNCTION     DS3GI_CM_SS_EVENT_CB

DESCRIPTION  Callback function invoked by Call Manager to notify 3G Dsmgr
             whenever a serving system related event occurs that 3G Dsmgr
             registered for.

             This function sends a command to the DS task, depending on the
             type of serving system event that occurred.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void  ds3gi_cm_ss_event_cb
(
  cm_ss_event_e_type           ss_event,        /* The event that occurred */
  const cm_mm_ss_info_s_type  *ss_info_ptr   /* Serving system information */
)
{
  ds_cmd_type  *cmd_ptr;                      /* Pointer to command buffer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    We only pay attention if the system mode was changed or the suspend
    state was changed, and ignore all other serving system related 
    notifications.
  -------------------------------------------------------------------------*/
  if( (ss_event == CM_SS_EVENT_SRV_CHANGED) &&
      (((ss_info_ptr->changed_fields & CM_SS_SYS_MODE_MASK) != 0) ||
      ((ss_info_ptr->changed_fields & CM_SS_PS_DATA_SUSPEND_MASK) != 0)))
  {
    /*-----------------------------------------------------------------------
      Get a command buffer from the DS Task.
    -----------------------------------------------------------------------*/
    if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
    {
      ERR_FATAL( "CM can't get cmd buf from DS task", 0, 0, 0 );
    }
    else
    {
      /*---------------------------------------------------------------------
        Fill in the command parameters and send the command to indicate that
        the system mode has changed and/or the suspend state has changed.
      ---------------------------------------------------------------------*/
      cmd_ptr->hdr.cmd_id = DS_CM_SS_SRV_CHG_CMD;
      cmd_ptr->cmd.ss_info.sys_mode = ss_info_ptr->sys_mode;
      cmd_ptr->cmd.ss_info.ps_data_suspend = ss_info_ptr->ps_data_suspend;

      ds_put_cmd( cmd_ptr );
    }
  }
#ifdef FEATURE_DATA_IS707
  else
  {
    ds707_p_rev_change_handler(ss_event, ss_info_ptr);
  }
#endif /*FEATURE_DATA_IS707 */

} /* ds3gi_cm_ss_event_cb() */


/*===========================================================================

FUNCTION DS3GI_FIND_EMPTY_CALL_STATE

DESCRIPTION
  Returns index to the first empty call state element.

DEPENDENCIES
  None
  
RETURN VALUE
  If found, returns the call instance index.  If not found, then returns 
  DS3GI_INVALID_CALL INSTANCE

SIDE EFFECTS
  None

===========================================================================*/
uint32 ds3gi_find_empty_call_state(void)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; i < DS3GI_MAX_NUM_CALLS; i++)
  {
    if (ds3gi_call_info[i].call_id == CM_CALL_ID_INVALID)
    {
      ASSERT(ds3gi_call_info[i].call_state == DS3G_IDLE_CALL_STATE);
      return(i);
    }
  }

  return(DS3GI_INVALID_CALL_INSTANCE);
}


/*===========================================================================

FUNCTION DS3GI_FIND_CALL_INSTANCE

DESCRIPTION
  Given a call_id, will return the index into the call_state array.  If 
  no instance is found, then returns DS3GI_INVALID_CALL_INSTANCE

DEPENDENCIES
  None
  
RETURN VALUE
  If found, returns the call instance.  If not found, then returns 
  DS3GI_INVALID_CALL INSTANCE

SIDE EFFECTS
  None

===========================================================================*/
uint32 ds3gi_find_call_instance
(
  cm_call_id_type     cm_call_id
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i = 0; i < DS3GI_MAX_NUM_CALLS; i++)
  {
    if (ds3gi_call_info[i].call_id == cm_call_id)
    {
      return(i);
    }
  }

  return(DS3GI_INVALID_CALL_INSTANCE);
}


/*===========================================================================
FUNCTION      DS3GI_END_CALL

DESCRIPTION   This function does all the work necessary to end a call. It
              calls the mode-specific call_ended_handler, cleans up the
              Autodetect RX data watermark prior to changing the serial mod
              to Autodetect and sets the state variables to the appropriate
              values.

DEPENDENCIES  This function can be called in any call state, except IDLE.

RETURN VALUE  None

SIDE EFFECTS  Updates the call_state, serial_state and at_state, and
              changes the serial mode to Autodetect.
===========================================================================*/
void ds3gi_end_call
(
  ds_cmd_type    *cmd_ptr
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;    /* Ptr to mode-sp. hdlr table */

  uint32 call_inst;
  
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  call_inst = ds3gi_find_call_instance(cmd_ptr->cmd.call_info.call_id);

  if (call_inst >= DS3GI_MAX_NUM_CALLS)   /* Klockwork fix */
  {
    ASSERT(0);
    return;
  }

  if (ds3gi_mode == SYS_SYS_MODE_NONE)
  {
    ERR("End call failed - System Mode NONE", 0, 0, 0);
    return;
  }

  /*-------------------------------------------------------------------------
    Point to the appropriate handler function table based on the current mode
    and call type.
  -------------------------------------------------------------------------*/
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode]
                               [ds3gi_call_info[call_inst].call_type];

  /*-------------------------------------------------------------------------
    Ensure that a handler function table is registered, with a non-null
    call_ended_handler.
  -------------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if( hdlr_tbl_ptr->call_ended_handler != NULL )
    {
      /*---------------------------------------------------------------------
        Invoke the mode-specific handler function.
      ---------------------------------------------------------------------*/
      hdlr_tbl_ptr->call_ended_handler( cmd_ptr,
                           ds3gi_call_info[call_inst].call_was_connected);
    
    } /* if call_ended_handler != NULL */
    else
    {
      MSG_HIGH( "Call ended handler not registered", 0, 0, 0 );
    }
  } /* if hdlr_tbl_ptr != NULL */

  /*-------------------------------------------------------------------------
    Set the call state, serial state and AT state.
  -------------------------------------------------------------------------*/
  ds3gi_call_info[call_inst].call_state = DS3G_IDLE_CALL_STATE;
  ds3gi_call_info[call_inst].call_id    = CM_CALL_ID_INVALID;
  ds3gi_call_info[call_inst].call_was_connected = FALSE;

  MSG_HIGH( "Data call ended", 0, 0, 0 );
} /* ds3gi_end_call() */


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION DS3GI_PROCESS_CM_RET_VAL

DESCRIPTION
  This function processes the return value from Call Manager functions.
  
  If an cm_mm_call_cmd_answer fails, calls force_call_down_hdlr which fills
  up the end_params and then calls cm_mm_call_cmd_end.
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  ds3gi_process_cm_ret_val
(
  cm_call_id_type            call_id    /* call id    */
)
{
  cm_end_params_s_type       end_params; /* end params */
  ds3g_hdlr_func_tbl_type    *hdlr_tbl_ptr;    /* Ptr to mode-sp. hdlr table */
  uint32                     call_inst; 
  boolean                    cm_ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  call_inst = ds3gi_find_call_instance(call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)   /* Klockwork fix */
  {
    ERR( "Bad call id %d; Call instance %d", call_id, call_inst, 0 );
    return;
  }

  if (ds3gi_mode == SYS_SYS_MODE_NONE)
  {
    ERR("System Mode NONE", 0, 0, 0);
    return;
  }

  ds3gi_call_info[call_inst].call_state = DS3G_IDLE_CALL_STATE;
  /*-------------------------------------------------------------------------
    Point to the appropriate handler function table based on the current mode
    and call type.
  -------------------------------------------------------------------------*/
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode]
                               [ds3gi_call_info[call_inst].call_type];
  if ((hdlr_tbl_ptr != NULL) &&
      (hdlr_tbl_ptr->force_call_down_handler != NULL))
  {
    hdlr_tbl_ptr->force_call_down_handler(call_id,
                                          &end_params);

    cm_ret_val = cm_mm_call_cmd_end(
      ds3gi_cm_call_cmd_cb,
      NULL,
      ds3gi_cm_client_id,
      1,
      &end_params
      );

    if (cm_ret_val == FALSE)
    {
      ERR_FATAL("CM call cmd end failed", 0, 0, 0);
      return;
    }
  }
} /* ds3gi_process_cm_ret_val() */
/*===========================================================================
FUNCTION      DS3GI_CM_INIT

DESCRIPTION   This function initializes the Data Services Call Manager
              client, and registers for call events of interest, namely:
                CM_CALL_EVENT_END
                CM_CALL_EVENT_INCOM
                CM_CALL_EVENT_CONNECT
                
              The following are only applicable to GSM/WCDMA:  
                CM_CALL EVENT_SETUP_IND
                CM_CALL_EVENT_CALL_CONF
                CM_CALL_EVENT_RAB_REL_IND
                CM_CALL_EVENT_RAB_ESTAB_IND
                CM_CALL_EVENT_RAB_REESTAB_REJ
                
              It then calls the individual mode handlers to allow them to
              register for mode-specific events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_init( void )
{
  cm_client_status_e_type  status;      /* Status returned by Call Manager */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Register as the Data Services client with Call Manager.
  -------------------------------------------------------------------------*/
  status = cm_client_init( CM_CLIENT_TYPE_DS, &ds3gi_cm_client_id );
  if( status != CM_CLIENT_OK )
  {
    ERR_FATAL( "DS unable to register as CM client:%d", status, 0, 0 );
  }
  else
  {
    /*-----------------------------------------------------------------------
      Register for call related events of interest. Multiple registrations
      are needed since only one range of events may be registered at a time.
    -----------------------------------------------------------------------*/
    cm_mm_client_call_reg( ds3gi_cm_client_id,
                           ds3gi_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG,
                           CM_CALL_EVENT_END,
                           CM_CALL_EVENT_END,
                           NULL 
                         );

    cm_mm_client_call_reg( ds3gi_cm_client_id,
                           ds3gi_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG,
                           CM_CALL_EVENT_INCOM,
                           CM_CALL_EVENT_CONNECT,  
                           NULL 
                         );

    cm_mm_client_call_reg( ds3gi_cm_client_id,
                           ds3gi_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG,
                           CM_CALL_EVENT_SETUP_IND,
                           CM_CALL_EVENT_SETUP_IND,
                           NULL 
                        );

    cm_mm_client_call_reg( ds3gi_cm_client_id,
                           ds3gi_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG,
                           CM_CALL_EVENT_CALL_CONF,
                           CM_CALL_EVENT_CALL_CONF,
                           NULL 
                        );

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

    /*-----------------------------------------------------------------------
      Register to be notified whenever the serving system changes.
    -----------------------------------------------------------------------*/
    cm_mm_client_ss_reg( ds3gi_cm_client_id,
                         ds3gi_cm_ss_event_cb,
                         CM_CLIENT_EVENT_REG,
                         CM_SS_EVENT_SRV_CHANGED,
                         CM_SS_EVENT_SRV_CHANGED,
                         NULL );

#ifdef FEATURE_DATA_IS707
    ds707_pkt_cm_init(ds3gi_cm_client_id);
#endif /* FEATURE_DATA_IS707 */

    /*-----------------------------------------------------------------------
      Activate the registered callback functions.
    -----------------------------------------------------------------------*/
    cm_client_act( ds3gi_cm_client_id );
  }
} /* ds3gi_cm_init() */


/*===========================================================================
FUNCTION      DS3GI_CM_CALL_CMD_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr
              of the completion status of the call command previously issued
              by 3G Dsmgr.
  
              This callback function may be invoked in relation to the
              following call commands: cm_call_cmd_answer(), 
              cm_call_cmd_orig(), cm_call_cmd_end(), cm_call_cmd_setup_res().

              In each case, if the command was successful, no action is
              taken. If the cm_call_cmd_orig() was unsuccessful, a
              DS_CM_CALL_ENDED_CMD is sent to the DS task. Errors in all
              other call commands do not require any action.

DEPENDENCIES  Assumes a one-call system.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_cm_call_cmd_cb
(
  void                   *data_block_ptr, /* Data blk provided in call cmd */
  cm_call_cmd_e_type      call_cmd,       /* Call command id               */
  cm_call_cmd_err_e_type  call_cmd_err    /* Call command error code       */
)
{
  ds_cmd_type  *cmd_ptr;                      /* Pointer to command buffer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    /*-----------------------------------------------------------------------
      Some type of error occurred, take action based on the the type of
      command.
    -----------------------------------------------------------------------*/
    switch( call_cmd )
    {
      case CM_CALL_CMD_ORIG:
        /*-------------------------------------------------------------------
          Send a DS_CM_CALL_END_CMD to the DS task.
        -------------------------------------------------------------------*/
        if (ds3gi_call_info[DS3GI_DEFAULT_CALL_INSTANCE].call_id ==
                                                          CM_CALL_ID_INVALID)
        {
          MSG_HIGH( "orig_cmd error, but no orig call", 0,0,0);
        }
        else
        {
          if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
          {
            ERR_FATAL( "CM can't get cmd buf from DS task", 0, 0, 0 );
          }
          else
          {
            MSG_HIGH( "cm_mm_call_cmd_orig() error: %d", call_cmd_err, 0, 0 );

            cmd_ptr->hdr.cmd_id = DS_CM_CALL_END_CMD;
            cmd_ptr->cmd.call_info.call_id = 
                          ds3gi_call_info[DS3GI_DEFAULT_CALL_INSTANCE].call_id;

            ds_put_cmd( cmd_ptr );
          }
        }
        break;

      case CM_CALL_CMD_ANSWER:
      case CM_CALL_CMD_END:
      case CM_CALL_CMD_SETUP_RES:
      default:
        MSG_HIGH( "CM call cmd error occurred: %d", call_cmd_err, 0, 0 );
        /* No action taken */
        break;

    } /* switch( call_cmd ) */
  } /* if cmd error */
} /* ds3gi_cm_call_cmd_cb() */


/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_END_CMD

DESCRIPTION   This function processes the DS_CM_CALL_END_CMD from Call
              Manager. This command is either a confirmation of a release
              initiated by 3G Dsmgr, or a notification of call release by the
              network. Note that if a data call is released from the UI, this
              will appear as a network call release.

DEPENDENCIES  The DS_CM_CALL_END_CMD command is valid in any call state,
              except IDLE. It is ignored if the call state is IDLE, or if
              the specified call id is not recognized.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_process_cm_call_end_cmd
(
  ds_cmd_type  *cmd_ptr                          /* Pointer to the command */
)
{
  uint32 call_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH( "Rx'ed CM_CALL_END_CMD", 0, 0, 0 );

  call_inst = ds3gi_find_call_instance(cmd_ptr->cmd.call_info.call_id);

  if (call_inst >= DS3GI_MAX_NUM_CALLS)   /* Klockwork fix */
  {
    MSG_HIGH("CM_CALL_END_CMD with bad call_id %d",
        cmd_ptr->cmd.call_info.call_id, 0, 0);
  }
  else if ( ds3gi_call_info[call_inst].call_state == DS3G_IDLE_CALL_STATE )
  {
    MSG_HIGH( "CM_CALL_END_CMD rx'ed in Idle call state", 0, 0, 0 );
    ds3gi_call_info[call_inst].call_id = CM_CALL_ID_INVALID;
  }
  else
  {
    /*---------------------------------------------------------------------
      Invoke a function to perform the actual work to end the call.
    ---------------------------------------------------------------------*/
    ds3gi_end_call(cmd_ptr);
  }
} /* ds3gi_process_cm_call_end_cmd() */


/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_INCOM_CMD

DESCRIPTION  This function processes the DS_CM_CALL_INCOM_CMD from Call
             Manager. It calls the mode-specific incoming_call_handler, and
             then either answers the incoming call, rings the serial port or
             rejects the incoming call (i.e. does nothing), depending on the
             return value from the handler function.

             If an incoming call is received in the Orig call state, the
             outgoing call is released, and the incoming call is given
             precedence.

DEPENDENCIES  The DS_CM_CALL_INCOM_CMD command is valid only in the IDLE
              or ORIG call states, and is ignored in all other call states.
              
              Currently, this code is written assuming only one CM data call
              at a time.  If multiple data calls are allowed, then this will
              have to change.
              
              Also assumes that simultaneous orig + inc call are always same
              technology.

RETURN VALUE  None

SIDE EFFECTS  Overwrites the call id and call type (in Idle or Orig call
              state only).  Updates the call_state, serial_state, at_state
              and may change the serial mode to Rawdata or Packet.
===========================================================================*/
void ds3gi_process_cm_call_incom_cmd
(
  ds_cmd_type  *cmd_ptr                          /* Pointer to the command */
)
{
  cm_call_id_type            inc_call_id;    /* incoming call's call id    */
  cm_call_id_type            exist_call_id;  /* existing call's call id    */
  ds3g_hdlr_func_tbl_type   *hdlr_tbl_ptr;   /* Ptr to mode-sp. hdlr table */
  ds3g_incom_e_type          action;         /* Action to take upon incoming 
                                                call                       */
  cm_ans_params_s_type       ans_params;     /* Answer params sent to Call 
                                                Mgr                        */

  /* End params for the current orginating calls.
     The param is an array.  Each element contains the parameter of individual
     originating calls to be ended.
     We only support one call for now 
  */
  cm_end_params_s_type       end_params_orig[1];

  cm_end_params_s_type       end_params_inc; /* end params if call rejected*/
  uint32                     call_inst = DS3GI_DEFAULT_CALL_INSTANCE; 
  boolean                    cm_ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  inc_call_id   = cmd_ptr->cmd.call_info.call_id;
  exist_call_id = ds3gi_call_info[call_inst].call_id;

  action = DS3G_DO_NOTHING;

  MSG_HIGH( "Rx'ed CM_CALL_INCOM_CMD, call id: %d",
             inc_call_id, 0, 0 );

  if (ds3gi_mode == SYS_SYS_MODE_NONE)
  {
    ERR("Could not process incoming call cmd. System Mode NONE", 0,0,0);
    return;
  }

  /*-------------------------------------------------------------------------
    If an incoming call is rx'ed after an origination, release the outgoing
    call, since the incoming call takes precedence. Note that this will set
    the call state back to Idle.
    
    OUR POLICY - 
    
               - this assumes 1 data call at a time (for any system)
               - have to remember could be outgoing async call and incoming
                 pkt data call (or vice versa).  End that call.
  -------------------------------------------------------------------------*/
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode]
                               [ds3gi_call_info[call_inst].call_type];

  if ((ds3gi_call_info[call_inst].call_state == DS3G_ORIG_CALL_STATE) &&
      (ds3gi_call_info[call_inst].call_type ==
                     DS3G_GET_DS_CALL_TYPE(cmd_ptr->cmd.call_info.call_type))
     )
  {
    /*-----------------------------------------------------------------------
      Same call type is originating as is incoming.  Tell mode-spec hdlr
      to end old call and accept new call.
    -----------------------------------------------------------------------*/
    if ((hdlr_tbl_ptr != NULL) &&
        (hdlr_tbl_ptr->switch_call_handler != NULL))
    {
      MSG_HIGH("Switch calls hdlr",0,0,0);
      MSG_HIGH("Inc call id %d exist call id %d",inc_call_id,exist_call_id,0);
      action = hdlr_tbl_ptr->switch_call_handler(cmd_ptr,
                                                 inc_call_id,
                                                 exist_call_id,
                                                 &ans_params,
                                                 &end_params_orig[0],
                                                 &end_params_inc);

      /*---------------------------------------------------------------------
         End the existing call with CM.  Incoming call is accepted/rejected
         further down.
      ---------------------------------------------------------------------*/
      cm_ret_val = cm_mm_call_cmd_end(
                                       ds3gi_cm_call_cmd_cb,
                                       NULL,
                                       ds3gi_cm_client_id,
                                       1,
                                       end_params_orig
                                     );
      if (cm_ret_val == FALSE)
      {
        ERR_FATAL("CM call cmd end failed", 0, 0, 0); 
        return;
      }
    }
  } /* if there is simultaneous orig + incoming call of same call type */
  else
  {
    if (ds3gi_call_info[call_inst].call_state == DS3G_ORIG_CALL_STATE)
    {
      /*-----------------------------------------------------------------------
        Different call types.  Policy is for first call to end and second
        call to be sent to mode-spec handler.  To end the first call, need
        the end_params.  Tell first call it must go down, and then notify
        mode-spec handler of incoming call.
      -----------------------------------------------------------------------*/
      if ((hdlr_tbl_ptr != NULL) &&
          (hdlr_tbl_ptr->force_call_down_handler != NULL))
      {
        MSG_HIGH("Force call down handler",0,0,0);
        if (hdlr_tbl_ptr->force_call_down_handler(exist_call_id, &end_params_orig[0])
                                                                     == TRUE)
        {
          cm_ret_val = cm_mm_call_cmd_end(
                                           ds3gi_cm_call_cmd_cb,
                                           NULL,
                                           ds3gi_cm_client_id,
                                           1,
                                           end_params_orig
                                         );

          if (cm_ret_val == FALSE)
          {
            ERR_FATAL("CM call cmd end failed", 0, 0, 0);
            return;
          }
          else
          {
            /*-------------------------------------------------------------------
              Existing call is ended.  Set up call state info so that rest of
              code can handle incoming call.
            -------------------------------------------------------------------*/
            ds3gi_call_info[call_inst].call_state = DS3G_IDLE_CALL_STATE;
            ds3gi_call_info[call_inst].call_id    = CM_CALL_ID_INVALID;
            ds3gi_call_info[call_inst].call_was_connected = FALSE;
          }
        }
      } 
    } /* if origination with different type call */

    /*-----------------------------------------------------------------------
      Incoming call is valid only in the Idle call state.
    -----------------------------------------------------------------------*/
    if (ds3gi_call_info[call_inst].call_state == DS3G_IDLE_CALL_STATE)
    {
      /*---------------------------------------------------------------------
        Point to the appropriate handler function table based on the current
        mode and call type.
      ---------------------------------------------------------------------*/
      hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode]
                  [DS3G_GET_DS_CALL_TYPE( cmd_ptr->cmd.call_info.call_type)];

      /*---------------------------------------------------------------------
        Ensure that a handler function table is registered, with a non-null
        incoming_call_handler.
      ---------------------------------------------------------------------*/
      if( hdlr_tbl_ptr != NULL )
      {
        if( hdlr_tbl_ptr->incoming_call_handler != NULL )
        {
          /*-----------------------------------------------------------------
            Invoke the mode-specific handler function.
          -----------------------------------------------------------------*/
          action = hdlr_tbl_ptr->incoming_call_handler(cmd_ptr,
                                                       inc_call_id,
                                                       &ans_params,
                                                       &end_params_inc 
                                                      );
        }
        else
        {
          MSG_HIGH( "Incoming call handler not registered", 0, 0, 0 );
        }
      } /* if hdlr_tbl_ptr != NULL */
    } /* if idle state */
    else
    {
      MSG_ERROR ( "DS already in data call, reject 2nd call", 0, 0, 0 );
      action = DS3G_REJECT_CALL;
      end_params_inc.call_id = cmd_ptr->cmd.call_info.call_id;
      end_params_inc.info_type = cmd_ptr->cmd.call_info.mode_info.info_type;
      if ( end_params_inc.info_type == CM_CALL_MODE_INFO_CDMA )
      {
        end_params_inc.end_params.cdma_end.end_reason_included = FALSE;
      }
    }
  }

  /*-------------------------------------------------------------------------
    At this point, the incoming call has been processed by the mode-
    specific handler.  Do next action based on results of mode-specific
    handler return value.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    If the mode specific handler returns ignore, then completely ignore the
    incoming call. do not save any information regarding teh call either.
  -------------------------------------------------------------------------*/
  if( action == DS3G_IGNORE )
  {
    MSG_HIGH("Mode_spec hdlr sez ignore",0,0,0);
    return;
  }

  if ( action != DS3G_REJECT_CALL )
  {
    ds3gi_call_info[call_inst].call_id = cmd_ptr->cmd.call_info.call_id;
    ds3gi_call_info[call_inst].call_type =
                   DS3G_GET_DS_CALL_TYPE( cmd_ptr->cmd.call_info.call_type );

  /*-------------------------------------------------------------------------
    Keep a note of the call type and the mode for the incoming call
    this will be used to find the mode handler when the call has ended
    to retrieve statistics
  -------------------------------------------------------------------------*/
    ds3gi_last_call_mode = ds3gi_mode;
    ds3gi_last_call_type = ds3gi_call_info[call_inst].call_type;
  }

  if( action == DS3G_ANSWER_CALL )
  {
    /*-----------------------------------------------------------------------
      Set the call state to indicate that we are waiting for the call
      to be connected.
    -----------------------------------------------------------------------*/
    ds3gi_call_info[call_inst].call_state = DS3G_WAIT_FOR_CONN_CALL_STATE;

    /*-----------------------------------------------------------------------
      Now send a command to Call Manager to answer the call.
    -----------------------------------------------------------------------*/
    MSG_HIGH( "Mode-Sp. Hdlr answers call", 0, 0, 0 );
    cm_ret_val = cm_mm_call_cmd_answer(
                                        ds3gi_cm_call_cmd_cb,
                                        NULL,
                                        ds3gi_cm_client_id,
                                        ds3gi_call_info[call_inst].call_id,
                                        &ans_params 
                                      );
    if (cm_ret_val == FALSE)
    {
      ERR("CM call cmd answer failed", 0, 0, 0);
      ds3gi_process_cm_ret_val ( ds3gi_call_info[call_inst].call_id );
      return;
    }
  }
  else if( action == DS3G_ALERT_USER )
  {
    /*-----------------------------------------------------------------------
      Set the call state to indicate that we are alerting the user, 
      and call a function to perform the alerting.
    -----------------------------------------------------------------------*/
    MSG_HIGH( "Mode-Sp. Hdlr says alert user", 0, 0, 0 );
    ds3gi_call_info[call_inst].call_state = DS3G_RINGING_CALL_STATE;
  }
  else if( action == DS3G_REJECT_CALL )
  {
    /*-----------------------------------------------------------------------
      For some reason, the mode-specific handler rejected the incoming
      call. Do nothing i.e. stay in the IDLE call state.
    -----------------------------------------------------------------------*/
    MSG_HIGH( "Mode-Sp. Hdlr rejected incoming call", 0, 0, 0 );
    cm_ret_val = cm_mm_call_cmd_end(
                                     ds3gi_cm_call_cmd_cb,
                                     NULL,
                                     ds3gi_cm_client_id,
                                     1,
                                     &end_params_inc 
                                   );

    if (cm_ret_val == FALSE)
    {
      ERR_FATAL("CM call cmd end failed", 0, 0, 0);
      return;
    }
  }
  else if( action == DS3G_DO_NOTHING)
  {
    /*-----------------------------------------------------------------
      Set the call state to indicate that we are waiting for the call
      to be connected, as we could receive a CONNECT still (example
      is bypass_alert).
    -----------------------------------------------------------------*/
    ds3gi_call_info[call_inst].call_state = DS3G_WAIT_FOR_CONN_CALL_STATE;
    MSG_HIGH("Mode_spec hdlr sez do nothing",0,0,0);
  }
} /* ds3gi_process_cm_call_incom_cmd() */


/*===========================================================================
FUNCTION     DS3GI_PROCESS_CM_CALL_CONNECTED_CMD

DESCRIPTION  This function processes the DS_CM_CALL_CONNECTED_CMD from Call
             Manager.  This command signifies that either an outgoing or an
             incoming call is connected, and end-to-end data transfer may
             proceed.

             This function invokes the mode-specific call_connected_handler,
             and changes the serial mode to Rawdata or Packet, if directed to
             do so by the mode-specific handler. Note that the serial mode
             may have been changed earlier, either before the origination or
             when the call was answered.

DEPENDENCIES  The DS_CM_CALL_CONNECTED_CMD command is valid only in the ORIG
              and WAIT_FOR_CONNECT call states, and is ignored in all other
              call states. This command is also ignored if the call id is not
              recognized.

RETURN VALUE  None

SIDE EFFECTS  Updates the call_state. May also update the serial_state, and
              change the serial mode to Rawdata or Packet.
===========================================================================*/
void ds3gi_process_cm_call_connected_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;    /* Ptr to mode-sp. hdlr table */
  boolean                   hdlr_result;
  cm_end_params_s_type      end_params;      
  boolean                   delay_connect;   /* Should connect be delayed? */
  uint32                    call_inst;
  boolean                   cm_ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_HIGH( "Rx'ed CM_CALL_CONNECTED_CMD", 0, 0, 0 );

  call_inst = ds3gi_find_call_instance(cmd_ptr->cmd.call_info.call_id);

  if (call_inst >= DS3GI_MAX_NUM_CALLS)   /* Klockwork fix */
  {
    MSG_HIGH( "CM_CALL_CONNECTED_CMD bad call id %d", 
               cmd_ptr->cmd.call_info.call_id, 0, 0 );
    return;
  }

  if (ds3gi_mode == SYS_SYS_MODE_NONE)
  {
    ERR("Call failed - System Mode NONE", 0, 0, 0);
    return;
  }

  if ((ds3gi_call_info[call_inst].call_state == DS3G_ORIG_CALL_STATE) ||
      (ds3gi_call_info[call_inst].call_state == DS3G_WAIT_FOR_CONN_CALL_STATE)
     )
  {
    /*---------------------------------------------------------------------
      Point to the appropriate handler function table based on the current
      mode and call type.
    ---------------------------------------------------------------------*/
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode]
                                 [ds3gi_call_info[call_inst].call_type];

    /*---------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      call_connected_handler.
    ---------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->call_connected_handler != NULL )
      {
        /*-----------------------------------------------------------------
          Invoke the mode-specific handler function.
        -----------------------------------------------------------------*/
        hdlr_result = hdlr_tbl_ptr->call_connected_handler
                                      (
                                         cmd_ptr->cmd.call_info.call_id,
                                         &cmd_ptr->cmd.call_info.mode_info,
                                         &delay_connect,
                                         &end_params
                                      );

        /*-------------------------------------------------------------------
           Proceed only if the mode-specific handler was successful in
           connecting the call.
        -------------------------------------------------------------------*/
        if( hdlr_result == DS3G_SUCCESS )
        {
          if (delay_connect == TRUE)
          {
            ds3gi_call_info[call_inst].call_state = 
                                              DS3G_WAIT_FOR_LL_CONNECT_STATE;
            MSG_HIGH( "Data call connect delayed", 0, 0, 0 );
          }
          else /* delay_connect is FALSE */
          {
            /*---------------------------------------------------------------
              Set Call state to CALL_ACTIVE.
            ---------------------------------------------------------------*/
            ds3gi_call_info[call_inst].call_state = 
                                                 DS3G_CALL_ACTIVE_CALL_STATE;
            ds3gi_call_info[call_inst].call_was_connected = TRUE;

            MSG_HIGH( "Data call connected", 0, 0, 0 );
          } /* delay_connect == FALSE */
        } /* if hdlr_result == DS3G_SUCCESS */
        else
        {
          /*---------------------------------------------------------------
           The mode-specific handler failed to connect the lower layers,
           so end the call.
          ---------------------------------------------------------------*/
          MSG_HIGH( "Handler failed to connect, ending call", 0, 0, 0 );

           ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;

           cm_ret_val = cm_mm_call_cmd_end( 
             ds3gi_cm_call_cmd_cb,
             NULL,
             ds3gi_cm_client_id,
             1,
             &end_params
             );
           if (cm_ret_val == FALSE)
           {
             ERR_FATAL("CM call cmd end failed", 0, 0, 0);
             return;
           }
        }
      } /* if call_connected_handler != NULL */
      else
      {
        ERR( "Call connected handler not registered", 0, 0, 0 );
      }
    } /* if hdlr_tbl_ptr != NULL */
  } /* if call_state == ORIG_CALL or WAIT_FOR_CONNECT */
  else
  {
    ERR( "Unexpected CM_CALL_CONNECTED_CMD rx'ed", 0, 0, 0 );
  }
} /* ds3gi_process_cm_call_connected_cmd() */


/*===========================================================================

FUNCTION DS3GI_PROCESS_CM_CALL_SETUP_CMD

DESCRIPTION
  This function processes the DS_CM_CALL_SETUP_CMD from Call Manager. It
  simply calls the mode-specific setup_handler, which validates the setup
  parameters, and either accepts or rejects the call.

  If the call is accepted, the mode-specific handler may specify new setup
  parameters which are sent to Call Manager in the setup response.

  No state change is involved for 3G Dsmgr.

DEPENDENCIES
  The DS_CM_CALL_SETUP_CMD is valid only in the IDLE or ORIG call states, and
  is ignored in all other call states.
  
  Parts of this function assume that there is only one active call at a time.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3gi_process_cm_call_setup_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_hdlr_func_tbl_type    *hdlr_tbl_ptr;  /* Ptr to mode-sp. hdlr table */
  boolean                     hdlr_result; /* Return val from mode-sp hdlr */
  cm_setup_res_params_s_type  setup_params;   /* Setup response parameters */
  uint32                      call_inst;
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))  /* UNFINISHED */
#error code not present
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MSG_HIGH( "Rx'ed CM_CALL_SETUP_CMD, call id: %d",
             cmd_ptr->cmd.call_info.call_id, 0, 0 );

  if (ds3gi_mode == SYS_SYS_MODE_NONE)
  {
    ERR("Call setup failed - System Mode NONE", 0, 0, 0);
    return;
  }

  call_inst = DS3GI_DEFAULT_CALL_INSTANCE;
  /*-------------------------------------------------------------------------
    If a call setup is rx'ed after an origination, release the outgoing
    call, since the incoming call takes precedence. Note that this will set
    the call state back to IDLE.
  -------------------------------------------------------------------------*/
  if (ds3gi_call_info[call_inst].call_state == DS3G_ORIG_CALL_STATE)
  {
    ds3gi_end_call(cmd_ptr);
  }

  /*-------------------------------------------------------------------------
    Call setup is valid only in the IDLE call state (its also valid in the
    ORIG call state, but this is taken care of by setting the call state to
    IDLE above).
  -------------------------------------------------------------------------*/

  if ((ds3gi_call_info[call_inst].call_state == DS3G_IDLE_CALL_STATE) &&
      (ds3gi_call_info[call_inst].call_id == CM_CALL_ID_INVALID))
  {
    /*-----------------------------------------------------------------------
      Point to the appropriate handler function table based on the current
      mode and call type.
    -----------------------------------------------------------------------*/
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode]
                 [DS3G_GET_DS_CALL_TYPE( cmd_ptr->cmd.call_info.call_type )];

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      setup_handler.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->setup_handler != NULL )
      {
        /*-------------------------------------------------------------------
          Invoke the mode-specific handler function.
        -------------------------------------------------------------------*/
        hdlr_result = hdlr_tbl_ptr->setup_handler(
                       cmd_ptr->cmd.call_info.call_id,
                       &cmd_ptr->cmd.call_info.mode_info,
                       &setup_params );

        if( hdlr_result == DS3G_FAILURE )
        {
          MSG_HIGH( "Mode-Sp. Hdlr rejected the setup", 0, 0, 0 );
        }

        /*-------------------------------------------------------------------
          Now send a setup response command to Call Manager. The mode-
          specific handler must specify whether the setup was accepted or
          rejected in the setup_params.
        -------------------------------------------------------------------*/
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))  /* UNFINISHED */
#error code not present
#endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */
        } /* if setup_handler != NULL */
      else
      {
        MSG_HIGH( "Setup call handler not registered", 0, 0, 0 );
      }
    } /* if hdlr_tbl_ptr != NULL */
  } /* if call_state == IDLE */
  else
  {
    ERR( "CM_SETUP_IND_CMD in Non-Idle call state", 0, 0, 0 );
  }
} /* ds3gi_process_cm_call_setup_cmd() */


/*===========================================================================
FUNCTION DS3GI_PROCESS_CM_CALL_CONF_CMD

DESCRIPTION
  This function processes the DS_CM_CALL_CONF_CMD received from Call Manager.
  It simply calls the mode-specific call_conf_handler.

  No state change is involved for 3G Dsmgr.

DEPENDENCIES
  The DS_CM_CALL_CONF_CMD is valid only in the ORIG call state, and is
  ignored in all other call states. Its is also ignored if the call id is not
  recognized.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3gi_process_cm_call_conf_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;    /* Ptr to mode-sp. hdlr table */
  boolean                   hdlr_result;/* Return value from mode-sp. hdlr */
  cm_end_params_s_type      end_params;
  uint32                    call_inst;
  boolean                   cm_ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH( "Rx'ed CM_CALL_CONF_CMD", 0, 0, 0 );

  call_inst = ds3gi_find_call_instance(cmd_ptr->cmd.call_info.call_id);

  /*-------------------------------------------------------------------------
    Call confirm is valid only in the Orig call state.
  -------------------------------------------------------------------------*/
  if ((call_inst < DS3GI_MAX_NUM_CALLS) &&
      (ds3gi_call_info[call_inst].call_state == DS3G_ORIG_CALL_STATE)
     )
  {
    if( cmd_ptr->cmd.call_info.call_id != ds3gi_call_info[call_inst].call_id )
    {
      MSG_HIGH( "CM_CALL_CONF_CMD has unknown call id", 0, 0, 0 );
    }
    else  /* Call id is valid */
    {
      if (ds3gi_mode == SYS_SYS_MODE_NONE)
      {
        ERR("Call conf failed - System Mode NONE", 0, 0, 0);
        return;
      }

      /*-----------------------------------------------------------------------
        Point to the appropriate handler function table based on the current
        mode and call type.
      -----------------------------------------------------------------------*/
      hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode]
                                   [ds3gi_call_info[call_inst].call_type];

      /*---------------------------------------------------------------------
        Ensure that a handler function table is registered, with a non-null
        call_conf_handler.
      ---------------------------------------------------------------------*/
      if( hdlr_tbl_ptr != NULL )
      {
        if( hdlr_tbl_ptr->call_conf_handler != NULL )
        {
          /*-----------------------------------------------------------------
            Invoke the mode-specific handler function.
          -----------------------------------------------------------------*/
          hdlr_result = hdlr_tbl_ptr->call_conf_handler( 
                       ds3gi_call_info[call_inst].call_id,
                       &cmd_ptr->cmd.call_info.mode_info,
                       &end_params );

          if( hdlr_result == DS3G_FAILURE )
          {
            /*---------------------------------------------------------------
              The parameters proposed by the network in the Call Conf were
              not acceptable to the mode-specific handler, so end the call.
            ---------------------------------------------------------------*/
            MSG_HIGH( "Handler rejected CALL_CONF, ending call", 0, 0, 0 );
            ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;

            cm_ret_val = cm_mm_call_cmd_end(ds3gi_cm_call_cmd_cb,
                                            NULL,
                                            ds3gi_cm_client_id,
                                            1,
                                            &end_params 
                                            );
            if (cm_ret_val == FALSE)
            {
              ERR_FATAL("CM call cmd end failed", 0, 0, 0);
              return;
            }
          }

        } /* if call_conf_handler != NULL */
        else
        {
          ERR( "Call confirm handler not registered", 0, 0, 0 );
        }
      } /* if hdlr_tbl_ptr != NULL */
    } /* Call id is valid */
  } /* if call_state == ORIG_CALL */
  else
  {
    ERR( "CM_CALL_CONF_CMD rx'ed in wrong call state", 0, 0, 0 );
  }

} /* ds3gi_process_cm_call_conf_cmd() */


#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */


/*===========================================================================
FUNCTION DS3GI_PROCESS_DATA_SUSPEND

DESCRIPTION
  If the suspend flag is set to TRUE, the mode specific Suspend Ind handler
  is called.
  This function does not check if the current suspend state is different 
  than the previous suspend state. It is the responsiblility of the mode 
  specific handler to check the suspend state and act accordingly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void ds3gi_process_data_suspend(void)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;    /* Ptr to mode-sp. hdlr table */
  uint8                    call_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds3gi_mode == SYS_SYS_MODE_NONE)
  {
    ERR("Data Suspend - System Mode NONE; Returning.", 0,0,0);
    return;
  }

  /*-------------------------------------------------------------------------
    Loop through all the registered handler function tables for the 
    current mode.
  -------------------------------------------------------------------------*/
  for (call_type = DS_CALL_TYPE_CKT; call_type < DS_NUM_CALL_TYPES;
                                                            call_type++) 
  {
    /*-----------------------------------------------------------------------
      Point to the appropriate handler function table based on the current
      mode and call type.
    -----------------------------------------------------------------------*/
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode][call_type];

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      rab_re_estab_fail_handler.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->data_suspend_ind_handler != NULL )
      {
        MSG_HIGH("Inter-RAT chg: Suspend Data Flow ",0,0,0);
        /*-------------------------------------------------------------------
          Invoke the mode-specific handler function.
        -------------------------------------------------------------------*/
        hdlr_tbl_ptr->data_suspend_ind_handler();
      }
    }
  }
} /* ds3gi_process_data_suspend() */


/*===========================================================================
FUNCTION DS3GI_PROCESS_DATA_RESUME

DESCRIPTION
  If the suspend flag is set to FALSE, the mode specific Resume handler
  is called.
  This function does not check if the current suspend state is different 
  than the previous suspend state. It is the responsiblility of the mode 
  specific handler to check the suspend state and act accordingly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void ds3gi_process_data_resume(void)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;    /* Ptr to mode-sp. hdlr table */
  uint8                    call_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ds3gi_mode == SYS_SYS_MODE_NONE)
  {
    ERR("Data Resume - System Mode NONE; Returning.", 0,0,0);
    return;
  }

  /*-------------------------------------------------------------------------
    Loop through all the registered handler function tables for the 
    current mode.
  -------------------------------------------------------------------------*/
  for (call_type = DS_CALL_TYPE_CKT; call_type < DS_NUM_CALL_TYPES;
                                                            call_type++) 
  {
    /*-----------------------------------------------------------------------
      Point to the appropriate handler function table based on the current
      mode and call type.
    -----------------------------------------------------------------------*/
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode][call_type];

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->data_resume_ind_handler != NULL )
      {
        /*-------------------------------------------------------------------
          Invoke the mode-specific handler function.
        -------------------------------------------------------------------*/
        hdlr_tbl_ptr->data_resume_ind_handler();
      }
    }
  }
} /* ds3gi_process_data_resume() */

/*===========================================================================

FUNCTION DS3GI_PROCESS_SYS_MODE_CHANGE

DESCRIPTION
  If the new mode is 'No Service' or SYS_SYS_MODE_GPS or SYS_SYS_MODE_AMPS, 
  then the mode change is ignored i.e.
  3G Dsmgr will behave as if the system mode is still whatever it was
  previously.
  If the new mode is the same as the current mode, no action is taken.

  For any other mode, the new mode is stored. For SYS_SYS_MODE_HDR, the
  mode that is stored is SYS_SYS_MODE_CDMA.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void ds3gi_process_sys_mode_change
(
  sys_sys_mode_e_type new_mode
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("New SYS Mode = %d, Current SYS Mode %d", new_mode,ds3gi_mode,0);

  if (new_mode == ds3gi_mode) 
  {
    MSG_HIGH("The SYS Mode has not changed ",0,0,0);
    return;
  }

  switch(new_mode)
  {
    case SYS_SYS_MODE_NO_SRV:
      MSG_HIGH( "Sys Mode changed to NO_SRV, ignored", 0, 0, 0 );
      break;

    case SYS_SYS_MODE_GPS:
    case SYS_SYS_MODE_AMPS:
      MSG_HIGH( "Ignoring unknown SYS MODE %d", new_mode, 0, 0);
      break;

    case SYS_SYS_MODE_HDR:               /* treated like CDMA */
    case SYS_SYS_MODE_CDMA:
      ds3gi_mode = SYS_SYS_MODE_CDMA;
      #ifndef FEATURE_DATA_STRIP_ATCOP
      dsat_init_service_mode( ds3gi_mode );
      #endif
      break;

    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
      ds3gi_mode = new_mode;
      #ifndef FEATURE_DATA_STRIP_ATCOP
      dsat_init_service_mode( ds3gi_mode );
      #endif
      break;

    default:
      ERR("Unknown mode to 3G DSMGR",0,0,0);
      ASSERT(0);
      break;
  } /* switch */
} /* ds3gi_process_sys_mode_change() */


/*===========================================================================

FUNCTION DS3GI_PROCESS_CM_SS_SRV_CHG_CMD

DESCRIPTION
  This function processes the DS_CM_SS_SRV_CHG_CMD from Call Manager.
  
  This function checks the suspend state (ps_data_suspend)from CM and does 
  the following:
  TRUE  : Calls the mode specific data suspend handler.
  FALSE : Calls the mode specific data suspend handler.

  This function also calls a function to set the new mode.  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  ds3gi_process_cm_ss_srv_chg_cmd
(
  ds_cmd_type  *cmd_ptr                          /* Pointer to the command */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Call Function to process the ps_data_suspend flag from Call Manager.
    If ps_data_suspend = TRUE, the mode specific suspend handler is called 
    before the switch to the new mode.
    If ps_data_suspend = FALSE, the mode specific resume handler is called 
    after the switch to the new mode.
  -------------------------------------------------------------------------*/
  if (cmd_ptr->cmd.ss_info.ps_data_suspend == TRUE) 
  {
    ds3gi_process_data_suspend();
    ds3gi_process_sys_mode_change(cmd_ptr->cmd.ss_info.sys_mode);
  }
  else
  {
    ds3gi_process_sys_mode_change(cmd_ptr->cmd.ss_info.sys_mode);
    ds3gi_process_data_resume();
  }
} /* ds3gi_process_cm_ss_srv_chg_cmd() */


#endif /* FEATURE_DATA */
