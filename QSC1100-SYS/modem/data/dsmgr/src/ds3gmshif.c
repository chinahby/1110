/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               3 G   D A T A   S E R V I C E S   M A N A G E R

                   M O D E - S P E C I F I C   S U B - T A S K  
       
                   I N T E R F A C E   F U N C T I O N S

GENERAL DESCRIPTION
  This software unit contains functions for interfacing to Mode-Specific sub-tasks.

EXTERNALIZED FUNCTIONS
  ds3g_msh_complete_ll_connect()
    Process command to complete call connections

  ds3g_msh_complete_ll_disconnect()
    Process command to complete call disconnection
    
  ds3g_msh_release_call()
    Processes indications from lower level software to release calls
    
  ds3g_msh_initiate_call()
    Originates a data call

  ds3g_msh_answer_call()
    Answers an incoming data call

  ds3g_msh_hangup_call()
    Ends the currently active data call
    
  ds3g_msh_call_rab_reestab_req()
    Processes request for RAB re-establishment.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None


  Copyright (c) 2002-2008 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gmshif.c_v   1.15   28 Feb 2003 18:55:04   rchar  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/src/ds3gmshif.c#4 $ $DateTime: 2008/04/11 05:12:17 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/08   sn      Fixed klockwork errors.
02/01/08   sn      Fixed lint errors.
01/28/08   sn      Featurized WCDMA/GSM code.
11/01/06   an      Featurize ATCOP, RMSM, Async Fax
08/21/03    sy     Mainlined the code where CM is always informed about the
                   DS call state(earlier it was under jcdma feature).
04/09/03   rsl     Changes relating to CM ret. val. check.
03/10/03   ak      When orig'ing a call, check CM return value to see if
                   call failed.
02/26/03   rc      Added function ds3g_msh_ps_data_available_ind() to inform
                   CM that new data is available on the uplink.
02/12/03   wx      Accommodate cm_mm_call_cmd_end api change and 
                   cm_end_params_s_type struct change
12/10/02   tmr     Added ds3g_msh_is_call_connected() function
12/04/02   tmr     Changed ds3g_msh_hangup_call() so that repeated attempts
                   to hangup the same call are ignored.
12/02/02   ak      Removed old featurization in rab_reestab() for 1x builds.
11/04/02   tmr     Return FALSE if we already have an active call when
                   ds3g_msh_initiate_call is called.  Also added setting
                   of call_was_connected field in call info structure.
10/22/02   tmr     Removed dsat_send_result_code( DSAT_CONNECT ) from 
                   ds3g_msh_complete_ll_connect()
10/14/02   vsk     Store the system mode & call type  during origination 
                   to be used after the call has ended to get stats
10/13/02   ak      Added function to set pkt state.  only for single-call
                   systems/JCDMA
10/10/02   tmr     Removed cmd_ptr parameter from ds3g_msh_complete_ll_connect()
10/03/02   rc      Changed prototype for ds3g_msh_call_rab_reestab_req() 
08/23/02   atp     Removed dsat_send_result_code() from answer_call().
08/22/02   atp     Moved stop_alerting() from answer_call() to mode-sp hdlr.
08/19/02   ak      Updated for multiple calls.
08/05/02   ak      Moved ds3g_initiate_call(), ds3g_answer_call(), and 
                   ds3g_hangup_call to this file.
08/01/02   rc      Modified ds3gi_process_msh_complete_ll_disconnect_cmd() to 
                   to send the result code only if 
                   stack_type = DS_CALL_STACK_EXTERNAL.
06/18/02   tmr     Modified ds3g_hangup_call() to delay the disconnect if the
                   user_end_handler() indicates so.
06/18/02   tmr     Added ds3gi_process_msh_complete_ll_disconnect_cmd().
04/19/02   vsk     Added PDP context preservation support
04/17/02   rc      Added function ds3gi_process_initiate_call_cmd().
02/26/02   tmr     Initial revision
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "assert.h"
#include "msg.h"
#include "err.h"
#include "cm.h"
#include "dstask.h"
#include "dstaski.h"
#include "ds3gmgr.h"
#include "ds3gmgrint.h"
#include "ds3gcmif.h"
#include "ds3gmshif.h"


/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION DS3G_MSH_IS_CALL_CONNECTED

DESCRIPTION
  This function returns whether or not the call identified by the input 
  call ID is in the connected state or not.


DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the call_id is valid and the call is in the connected state,
  FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean  
ds3g_msh_is_call_connected
(
  cm_call_id_type call_id
)
{
   uint32  call_inst;
   boolean rc = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_inst = ds3gi_find_call_instance(call_id);
  
  if (call_inst >= DS3GI_MAX_NUM_CALLS)   /* Klockwork Fix */
  {
    ERR("Bad call id for is call active %d", call_id, 0, 0);
  }
  else if (ds3gi_call_info[call_inst].call_state == DS3G_CALL_ACTIVE_CALL_STATE)
  {
    rc = TRUE;
  }
  
  return rc;
}


/*===========================================================================

FUNCTION DS3G_MSH_COMPLETE_LL_CONNECT

DESCRIPTION
  This function processes the DS_COMPLETE_LL_CONNECT_CMD from lower
  layers when a connection has been delayed.  This is currently used by
  the GCSD protocol stack to support the +DR command which reports what
  type of compression is used in an NT data call. Compression is 
  negotiated at the RLP layer, and the +DR data must be reported by ATCOP prior
  to the CONNECT message, therefore the CONNECT is delayed while the RLP
  negotiation occurs.

  This function invokes the mode-specific complete_ll_connect_handler, and changes
  the serial mode to Rawdata or Packet, if directed to do so by the mode-
  specific handler. Note that the serial mode may have been changed earlier,
  either before the origination or when the call was answered.

DEPENDENCIES
  The DS_COMPLETE_LL_CONNECT_CMD command is valid only in the 
  DS3G_WAIT_FOR_LL_CONNECT_STATE call states, and is ignored in all other
  call states.

RETURN VALUE
  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS
  Updates the call_state. May also update the serial_state, and change the
  serial mode to Rawdata or Packet.

===========================================================================*/

boolean  ds3g_msh_complete_ll_connect
(
  cm_call_id_type         call_id
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;    /* Ptr to mode-sp. hdlr table */
  boolean                   hdlr_result;
  /* The following local variables are used to store return values from the
     mode specific handler (passed by reference to the handler function)   */
  cm_end_params_s_type      end_params;
  boolean                   ret_val = DS3G_FAILURE;
  uint32                    call_inst;
  boolean                   cm_ret_val = FALSE; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH( "Rx'ed DS_COMPLETE_LL_CONNECT_CMD", 0, 0, 0 );

  call_inst = ds3gi_find_call_instance(call_id);

  if (call_inst >= DS3GI_MAX_NUM_CALLS)   /* Klockwork Fix */
  {
    ERR("Bad call id for ll connect %d", call_id, 0, 0);
    return(ret_val);
  }

  if (ds3gi_mode == SYS_SYS_MODE_NONE)
  {
    ERR("LL Connect failed - System Mode NONE; Returning.", 0, 0, 0);
    return(ret_val);
  }

  if (ds3gi_call_info[call_inst].call_state == DS3G_WAIT_FOR_LL_CONNECT_STATE) 
  {
    /*---------------------------------------------------------------------  
      Point to the appropriate handler function table based on the current
      mode and call type.
    ---------------------------------------------------------------------*/
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode]
                                 [ds3gi_call_info[call_inst].call_type];

    /*---------------------------------------------------------------------
     Ensure that a handler function table is registered, with a non-null
     completed_call_connected_handler.
    ---------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->complete_ll_connect_handler != NULL )
      {
        /*-----------------------------------------------------------------
          Invoke the mode-specific handler function.
        -----------------------------------------------------------------*/
        hdlr_result = hdlr_tbl_ptr->complete_ll_connect_handler(
                          ds3gi_call_info[call_inst].call_id,
                          &end_params);

        /*-----------------------------------------------------------------
           Proceed only if the mode-specific handler was successful in
           completing the connection.
        -----------------------------------------------------------------*/
        if( hdlr_result == DS3G_SUCCESS )
        {
          /*---------------------------------------------------------------
          Send the CONNECT result code to the TE and set the call state
          to CALL_ACTIVE.
          ---------------------------------------------------------------*/
          ds3gi_call_info[call_inst].call_state = DS3G_CALL_ACTIVE_CALL_STATE;
          ds3gi_call_info[call_inst].call_was_connected = TRUE;

          MSG_HIGH( "Delayed connect call now connected", 0, 0, 0 );
          ret_val = DS3G_SUCCESS;

        } /* if hdlr_result == DS3G_SUCCESS */
        else
        {
          /*---------------------------------------------------------------
           The mode-specific handler failed to complete the connection,
           so end the call.
          ---------------------------------------------------------------*/
          MSG_HIGH( "Handler failed to connect, ending call", 0, 0, 0 );

          ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;

          cm_ret_val = cm_mm_call_cmd_end(ds3gi_cm_call_cmd_cb,
                                          NULL,
                                          ds3gi_cm_client_id,
                                          1,
                                          &end_params 
                                          );
          if (cm_ret_val == FALSE)
          {
            ERR_FATAL("CM end call failed", 0, 0, 0);
          }
        } 
      } /* complete_ll_connect_handler != NULL */
      else
      {
        ERR( "Complete call connected handler not registered", 0, 0, 0 );
      }
    } /* if hdlr_tbl_ptr != NULL */
  } /* if call_state == DS3G_WAIT_FOR_LL_CONNECT_STATE */
  else
  {
    ERR( "Unexpected DS_COMPLETE_LL_CONNECT_CMD rx'ed", 0, 0, 0 );
  }

  return(ret_val);

} /* ds3g_msh_complete_ll_connect() */


/*===========================================================================

FUNCTION DS3G_MSH_COMPLETE_LL_DISCONNECT

DESCRIPTION
  This function processes the DS_COMPLETE_LL_DISCONNECT_CMD from lower
  layers when a disconnect has been delayed.  This is currently used by
  the GCSD protocol stack for FAX calls so that any queued data can be 
  sent prior to a disconnect occuring.  This is necessary for FAX protocols

  This function invokes the mode-specific complete_ll_disconnect_handler, reports
  the disconnect to CM and outputs the AT response

DEPENDENCIES
  The DS_COMPLETE_LL_DISCONNECT_CMD command is valid only in the
  DS3G_WAIT_FOR_LL_DISCONNECT_STATE call state, and is ignored in all other
  call states.

RETURN VALUE
  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS
  Updates the call_state.

===========================================================================*/

boolean  ds3g_msh_complete_ll_disconnect
(
  cm_call_id_type   call_id
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;
  cm_end_params_s_type      end_params;
  uint32                    call_inst;
  boolean                   ret_val = DS3G_FAILURE;
  boolean                   cm_ret_val = FALSE; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH( "Rx'ed DS_COMPLETE_LL_DISCONNECT_CMD", 0, 0, 0 );

  call_inst = ds3gi_find_call_instance(call_id);

  if (call_inst >= DS3GI_MAX_NUM_CALLS)   /* Klockwork Fix */
  {
    ERR("Bad call id for ll disconnect %d", call_id, 0, 0);
    return(ret_val);
  }

  if (ds3gi_mode == SYS_SYS_MODE_NONE)
  {
    ERR("LL Disconnect failed - System Mode NONE", 0, 0, 0);
    return(ret_val);
  }

  if (ds3gi_call_info[call_inst].call_state == DS3G_WAIT_FOR_LL_DISCONNECT_STATE)
  {
    /*---------------------------------------------------------------------
      Point to the appropriate handler function table based on the current
      mode and call type.
    ---------------------------------------------------------------------*/
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode]
                                 [ds3gi_call_info[call_inst].call_type];

    /*---------------------------------------------------------------------
     Ensure that a handler function table is registered, with a non-null
     complete_ll_disconnect_handler.
    ---------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->complete_ll_disconnect_handler != NULL )
      {
        /*-----------------------------------------------------------------
          Invoke the mode-specific handler function.
        -----------------------------------------------------------------*/
        hdlr_tbl_ptr->complete_ll_disconnect_handler(
                          ds3gi_call_info[call_inst].call_id,
                          &end_params);

        /*-----------------------------------------------------------------
          Set the call state to indicate that we are waiting for the call to
          be ended.
        -----------------------------------------------------------------*/
        ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;

        /*------------------------------------------------------------------
          Now send a command to Call Manager to end the call.
        ------------------------------------------------------------------*/
        MSG_HIGH( "Sending End Call command to CM", 0, 0, 0 );
        cm_ret_val = cm_mm_call_cmd_end( ds3gi_cm_call_cmd_cb,
                                         NULL,
                                         ds3gi_cm_client_id,
                                         1,
                                         &end_params );
        if (cm_ret_val == FALSE)
        {
          ERR_FATAL("CM end call failed", 0, 0, 0);
        }
        else
        {
          ret_val = DS3G_SUCCESS;
        }
      } /* complete_ll_disconnect_handler != NULL */
      else
      {
        ERR( "Complete call disc hdlr not registered", 0, 0, 0 );
      }
    } /* if hdlr_tbl_ptr != NULL */
  } /* if call_state == DS3G_WAIT_FOR_LL_DISCONNECT_STATE */
  else
  {
    ERR( "Unexpected DS_COMPLETE_LL_DISCONNECT_CMD", 0, 0, 0 );
  }

  return(ret_val);
} /* ds3g_msh_complete_ll_disconnect() */

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */


/*===========================================================================
FUNCTION      DS3G_MSH_ANSWER_CALL

DESCRIPTION
  This function answers an incoming data call. It calls the mode-specific
  user_answer_handler, changes the serial mode (if requested to do so by the
  mode-specific handler), and then sends a command to Call Manager to answer
  the call.
  
  This function should be invoked by ATCoP when the answer command "ATA" is
  received.

DEPENDENCIES
  The call will not be answered if any of these conditions is true:
  (a) The call state is not RINGING
  (b) No mode-specific handler table is registered for the current mode and
      call type
  (c) No user_answer_handler is registered for the current mode and call
      type (since this handler must specify the ans_params to be sent to
      Call Mgr)

RETURN VALUE
  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS
  Updates 3G Dsmgr state variables and may also change the serial mode, if
  the call is answered.

===========================================================================*/
boolean ds3g_msh_answer_call
(
  cm_call_id_type      cm_call_id
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;
  cm_ans_params_s_type      ans_params;
  uint32                    call_inst;
  boolean                   ret_val = DS3G_FAILURE;
  boolean                   cm_ret_val = FALSE; 

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(IS_IN_DS_TASK());

  call_inst = ds3gi_find_call_instance(cm_call_id);

  if (call_inst >= DS3GI_MAX_NUM_CALLS)   /* Klockwork Fix */
  {
    ERR("Bad call id for answer call", cm_call_id, 0, 0);
    return(ret_val);
  }

  if (ds3gi_mode == SYS_SYS_MODE_NONE)
  {
    ERR("Answer call failed - System Mode NONE", 0, 0, 0);
    return(ret_val);
  }

  if( ds3gi_call_info[call_inst].call_state == DS3G_RINGING_CALL_STATE )
  {
    /*-----------------------------------------------------------------------
      Point to the appropriate handler function table based on the current
      mode and call type.
    -----------------------------------------------------------------------*/
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode]
                                 [ds3gi_call_info[call_inst].call_type];

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      user_answer_handler.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->user_answer_handler != NULL )
      {
        /*-------------------------------------------------------------------
          Invoke the mode-specific handler function.
        -------------------------------------------------------------------*/
        hdlr_tbl_ptr->user_answer_handler
                                      ( 
                                        ds3gi_call_info[call_inst].call_id,
                                        &ans_params
                                      );

        /*-------------------------------------------------------------------
          Set the call state to indicate that we are waiting for the call to
          be connected.
        -------------------------------------------------------------------*/
        ds3gi_call_info[call_inst].call_state = DS3G_WAIT_FOR_CONN_CALL_STATE;

        /*-------------------------------------------------------------------
          Now send a command to Call Manager to answer the call.
        -------------------------------------------------------------------*/
        MSG_HIGH( "Data call answered, call id: %d",
                   ds3gi_call_info[call_inst].call_id, 0, 0 );

        cm_ret_val = cm_mm_call_cmd_answer(ds3gi_cm_call_cmd_cb,
                                           NULL,
                                           ds3gi_cm_client_id,
                                           ds3gi_call_info[call_inst].call_id,
                                           &ans_params 
                                           );
        if (cm_ret_val == FALSE)
        {
          ERR("CM call cmd answer failed", 0, 0, 0);
          ds3gi_process_cm_ret_val ( ds3gi_call_info[call_inst].call_id );
        }
        else
        {
          ret_val = DS3G_SUCCESS;
        }
      } /* if user_answer_handler != NULL */
      else
      {
        ERR( "User answer handler not registered", 0, 0, 0 );
      }
    } /* if hdlr_tbl_ptr != NULL */
    else
    {
      ERR( "Handler function table not registered", 0, 0, 0 );
    }
  } /* if call_state == RINGING */
  else
  {
    ERR( "ATA rx'ed, but phone is not RINGING", 0, 0, 0 );
  }

  return(ret_val);
} /* ds3g_msh_answer_call() */


/*===========================================================================
FUNCTION      DS3G_MSH_INITIATE_CALL

DESCRIPTION   This function originates a data call of the specified call 
              type. It gets a call id from Call Manager, calls the mode-
              specific originate_call_handler, changes the serial mode (if
              requested to do so by the mode-specific handler), and then
              sends a command to Call Manager to originate a call.

              This function should be invoked by ATCoP when the dial command
              "ATD" (or equivalent) is received.

DEPENDENCIES  The call will not be originated if any of these conditions is
              true:
                (a) The call state is not IDLE
                (b) No mode-specific handler table is registered for the
                    current mode and call type
                (c) No originate_call_handler is registered for the current
                    mode and call type (since this handler must specify the
                    orig_params to be sent to Call Mgr)
                (d) A valid call id could not be obtained from Call Manager
                    (this could occur if the phone is in use, offline etc.)

RETURN VALUE  TRUE - call was originated.  FALSE - call was not originated.

SIDE EFFECTS  Updates 3G Dsmgr state variables and may also change the serial
              mode, if the origination is successful.
===========================================================================*/
boolean ds3g_msh_initiate_call
(
  ds_call_type_e_type  call_type,     /* Call type - Circuit or Packet     */
  sys_sys_mode_e_type  sys_mode,      /* WCDMA, GSM, CDMA, or other?       */
  const byte          *dial_string,   /* Null-terminated dial string       */
  boolean              digit_mode,    /* 1 = dial str has non-digit chars  */
                                      /* 0 = dial str has only digits      */
  void                *user_info_ptr  /* points to user supplied info      */
)
{
  ds3g_hdlr_func_tbl_type      *hdlr_tbl_ptr;
  boolean                       hdlr_result;
  const byte                   *s_ptr;
  uint8                         dial_string_len = 0;
  cm_digit_mode_e_type          cm_digit_mode;
  cm_num_s_type                 cm_dial_num;
  cm_num_s_type                 calling_num;
  cm_orig_alpha_s_type          alpha_buf;

  cm_cdma_orig_params_s_type    cdma_orig_params;
  cm_gw_cs_orig_params_s_type   gw_cs_orig_params;
  cm_gw_ps_orig_params_s_type   gw_ps_orig_params;

  boolean                       cdma_params_changed  = FALSE;
  boolean                       gw_cs_params_changed = FALSE;
  boolean                       gw_ps_params_changed = FALSE;

  cm_cdma_orig_params_s_type   *cdma_orig_params_ptr  = NULL;
  cm_gw_cs_orig_params_s_type  *gw_cs_orig_params_ptr = NULL;
  cm_gw_ps_orig_params_s_type  *gw_ps_orig_params_ptr = NULL;

  cm_srv_type_e_type            cm_srv_type;
  uint32                        call_inst = DS3GI_DEFAULT_CALL_INSTANCE;

  ds_call_type_e_type           orig_call_type;
  ds3gi_call_state_e_type       orig_call_state;

  boolean                       ret_val = DS3G_FAILURE;  /* did call orig? */
  boolean                       cm_ret_val; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(IS_IN_DS_TASK());

  if (sys_mode == SYS_SYS_MODE_NONE)
  {
    ERR("Can't initiate call - System mode NONE", 0, 0, 0);
    return ret_val;
  }

  if( call_type == DS_NUM_CALL_TYPES )
  {
    ERR("Can't initiate call - Invalid Call Type", 0,0,0);
    return ret_val;
  }

  if (ds3gi_call_info[call_inst].call_state != DS3G_IDLE_CALL_STATE)
  {
     ERR("Can't orig, incoming call in progress",0,0,0);
     return ret_val;
  }
  
  call_inst = ds3gi_find_empty_call_state();

  if (call_inst >= DS3GI_MAX_NUM_CALLS)   /* Klockwork fix */
  {
    ERR("cant initiate - Too many calls",0,0,0);
    return(ret_val);
  }

  /*-------------------------------------------------------------------------
    Originate a call only if the call state is IDLE.
  -------------------------------------------------------------------------*/
  if (sys_mode == ds3gi_mode)
  {
    /*-----------------------------------------------------------------------
      Point to the appropriate handler function table based on the current
      mode and call type.
    -----------------------------------------------------------------------*/
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[sys_mode][call_type];

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      originate_call_handler.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->originate_call_handler != NULL )
      {
        /*-------------------------------------------------------------------
          Count the number of characters/digits in the dial string.
        -------------------------------------------------------------------*/
        s_ptr = dial_string;
        
        while( *s_ptr++ != NULL )
        {
          dial_string_len++;
        };
      
        /*-------------------------------------------------------------------
          Invoke the mode-specific handler function, which will return all
          the necessary origination parameters.
        -------------------------------------------------------------------*/
        hdlr_result = hdlr_tbl_ptr->originate_call_handler(
                         dial_string,
                         dial_string_len,
                         &cdma_orig_params,         /* cdma orig params    */
                         &cdma_params_changed,
                         &gw_cs_orig_params,        /* GW CS orig parms    */
                         &gw_cs_params_changed,
                         &gw_ps_orig_params,        /* GW PS orig parms    */
                         &gw_ps_params_changed,
                         &cm_srv_type,
                         user_info_ptr);

        /*-------------------------------------------------------------------
          Originate a call only if the mode-specific handler says its ok
          to originate.
        -------------------------------------------------------------------*/
        if( hdlr_result == DS3G_SUCCESS )
        {

          /*-----------------------------------------------------------------
            Determine the digit mode.
          -----------------------------------------------------------------*/
          cm_digit_mode =
           (digit_mode ? CM_DIGIT_MODE_8BIT_ASCII : CM_DIGIT_MODE_4BIT_DTMF);

          /*-----------------------------------------------------------------
            Convert the dial string to the format required by Call Manager.
          -----------------------------------------------------------------*/
          cm_num_fill( &cm_dial_num, 
                       dial_string, 
                       dial_string_len,
                       cm_digit_mode 
                     );

          /*-----------------------------------------------------------------
            We do not specify any calling number or alpha tag, so set the
            lengths to zero.
          -----------------------------------------------------------------*/
          calling_num.len = 0;
          alpha_buf.len = 0;

          /*-----------------------------------------------------------------
           Set the call state to indicate that a data call origination is
           in progress, and remember the call type.  Save the orig call
           state in case orig fails synchronously.
          -----------------------------------------------------------------*/
          orig_call_type  = ds3gi_call_info[call_inst].call_type;
          orig_call_state = ds3gi_call_info[call_inst].call_state;

          ds3gi_call_info[call_inst].call_state = DS3G_ORIG_CALL_STATE;
          ds3gi_call_info[call_inst].call_type  = call_type;

          /*-----------------------------------------------------------------
            Set orig params ptr appropriately.
          -----------------------------------------------------------------*/
          if (cdma_params_changed == TRUE)
          {
            cdma_orig_params_ptr = &cdma_orig_params;
          }

          if (gw_cs_params_changed == TRUE)
          {
            gw_cs_orig_params_ptr = &gw_cs_orig_params;
          }

          if (gw_ps_params_changed == TRUE)
          {
            gw_ps_orig_params_ptr = &gw_ps_orig_params;
          }

          /*-----------------------------------------------------------------
             Now send a command to Call Manager to originate the call.  Note
             that the call_id field is filled in below via the cm cmd.
          -----------------------------------------------------------------*/
          cm_ret_val = cm_mm_call_cmd_orig
                             ( ds3gi_cm_call_cmd_cb,           /* callback */
                               NULL,                           /* data blk */
                               ds3gi_cm_client_id,             /* client id*/
                               DS3G_GET_CM_CALL_TYPE(call_type),
                               cm_srv_type,                    /* srv type */
   
                               &calling_num,          /* calling number    */
                               &cm_dial_num,          /* # being called    */
                               &alpha_buf,            /* alpha buff/tag    */

                               cdma_orig_params_ptr,   /* cdma orig params */
                               gw_cs_orig_params_ptr,  /* GW CS orig parms */
                               gw_ps_orig_params_ptr,  /* GW PS orig parms */
                               &ds3gi_call_info[call_inst].call_id
                             );

          if ((cm_ret_val == FALSE) ||
              (ds3gi_call_info[call_inst].call_id == CM_CALL_ID_INVALID))
          {
            ds3gi_call_info[call_inst].call_type = orig_call_type;
            ds3gi_call_info[call_inst].call_state = orig_call_state;

            MSG_HIGH("CM failed DS orig",0,0,0);
          }
          else
          {
            /*---------------------------------------------------------------
               CM accepted the origination and gave us a call id.
            ---------------------------------------------------------------*/
            MSG_HIGH( "Originating Data call, id: %d, type: %d",
                       ds3gi_call_info[call_inst].call_id, call_type, 0 );


            /*---------------------------------------------------------------
             Pass the call id allocated by Call Manager to the mode-specific
             handler.
            ---------------------------------------------------------------*/
            if( hdlr_tbl_ptr->call_id_handler != NULL )
            {
              hdlr_tbl_ptr->call_id_handler
                                        ( ds3gi_call_info[call_inst].call_id,
                                          user_info_ptr 
                                        );
            }

            /*----------------------------------------------------------------
              Keep a note of the call type and the mode in which call has been
              originated. this will be used to find the mode handler when the 
              call has ended to retrieve statistics
            ----------------------------------------------------------------*/
            ds3gi_last_call_mode = ds3gi_mode;
            ds3gi_last_call_type = call_type;

            ret_val = TRUE;
          }
        } /* if hdlr_result == DS3G_SUCCESS */
        else
        {
          ERR( "Can't orig, Mode-sp. Hdlr error", 0, 0, 0 );
        }
      } /* if originate_call_handler != NULL */
      else
      {
        ERR( "Can't orig, orig_handler not registered", 0, 0, 0 );
      }
    } /* if hdlr_tbl_ptr != NULL */
    else
    {
      ERR( "Can't orig, handler table not registered", 0, 0, 0 );
    }
  } /* if call_state == IDLE */
  else
  {
    ERR("cant orig - wrong mode %d %d",ds3gi_mode,sys_mode,0);
  }

  return(ret_val);
} /* ds3g_msh_initiate_call() */


/*===========================================================================
FUNCTION      DS3G_MSH_HANGUP_CALL

DESCRIPTION   This function ends the currently active data call (if any).
              It calls the mode-specific user_end_handler, to allow any
              protocols to be shut down if necessary, and then sends a
              command to Call Manager to end the call.

DEPENDENCIES  No action is taken if no call is active i.e. the call state
              is IDLE.

RETURN VALUE  DS3G_SUCCESS if CM is told to end call.  
              DS3G_FAILURE if could not end call.

SIDE EFFECTS  Updates 3G Dsmgr state variables if a call was active.
===========================================================================*/
boolean ds3g_msh_hangup_call
(
  cm_call_id_type      call_id,       /* call id of call to end            */
  void                *user_info_ptr  /* points to user supplied info      */
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;
  cm_end_params_s_type      end_params;
  boolean                   delay_disconnect;
  uint32                    call_inst;
  boolean                   ret_val = DS3G_FAILURE;
  boolean                   cm_ret_val = FALSE; 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(IS_IN_DS_TASK());

  call_inst = ds3gi_find_call_instance(call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)            /* Klockwork Fix */
  {
    ERR("Trying to end non-existent call id %d",call_id,0,0);
    return(ret_val);
  }

  if (ds3gi_mode == SYS_SYS_MODE_NONE)
  {
    ERR("Hangup call failed - System Mode NONE", 0, 0, 0);
    return(ret_val);
  }

  if( ( ds3gi_call_info[call_inst].call_state != DS3G_IDLE_CALL_STATE ) &&
      ( ds3gi_call_info[call_inst].call_state != DS3G_WAIT_FOR_LL_DISCONNECT_STATE ) &&
      ( ds3gi_call_info[call_inst].call_state != DS3G_END_CALL_STATE )
    )
  {
    /*-----------------------------------------------------------------------
      Point to the appropriate handler function table based on the current
      mode and call type.
    -----------------------------------------------------------------------*/
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[ds3gi_mode]
                                 [ds3gi_call_info[call_inst].call_type];

    /*-----------------------------------------------------------------------
      Check if a handler function table is registered, with a non-null
      user_end_handler.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->user_end_handler != NULL )
      {
        /*-------------------------------------------------------------------
          Invoke the mode-specific handler function.
        -------------------------------------------------------------------*/
        hdlr_tbl_ptr->user_end_handler( ds3gi_call_info[call_inst].call_id,
                                        &end_params,
                                        &delay_disconnect,
                                        user_info_ptr
                                      );

        if (delay_disconnect == TRUE)
        {
          ds3gi_call_info[call_inst].call_state = 
                                          DS3G_WAIT_FOR_LL_DISCONNECT_STATE;
          MSG_HIGH ( "Delaying user disconnect", 0, 0, 0 );
        }
        else
        {
          /*-----------------------------------------------------------------
            Set the call state to indicate that we are waiting for the call
            to be ended.
          -----------------------------------------------------------------*/
          ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;

         /*------------------------------------------------------------------
            Now send a command to Call Manager to end the call.
          -----------------------------------------------------------------*/
          MSG_HIGH( "Sending End Call command to CM", 0, 0, 0 );

          cm_ret_val = cm_mm_call_cmd_end(ds3gi_cm_call_cmd_cb,/* cm callback*/
                                          NULL,                /* data block */                      
                                          ds3gi_cm_client_id,  /* client id  */  
                                          1,                   /* # call id's*/
                                          &end_params          /* end parms  */ 
                                          );
          if (cm_ret_val == FALSE)
          {
            ERR_FATAL("CM call cmd end failed", 0, 0, 0);
          }
        }
        ret_val = DS3G_SUCCESS;
      } /* if user_end_handler != NULL */
      else
      {
        ERR( "User end handler not registered", 0, 0, 0 );
      }
    } /* if hdlr_tbl_ptr != NULL */
    else
    {
      ERR( "Can't hangup call - Hdlr NULL ", 0, 0, 0 );
    }
  } /* if call_state != IDLE */
  else
  {
    ERR( "Can't hangup in Idle or terminating call state", 0, 0, 0 );
  }

  return(ret_val);
} /* ds3g_msh_hangup_call() */


#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */


/*===========================================================================
FUNCTION      DS3G_MSH_SET_PKT_STATE

DESCRIPTION   This function is appropriate for one-call systems only.  It 
              allows the mode-specific handler to tell CM the state of its
              packet connection.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds3g_msh_set_pkt_state
(
  cm_packet_state_e_type    pkt_state
)
{
  boolean cm_ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cm_ret_val = cm_ph_cmd_packet_state(NULL,                         /* cb func          */
                                      NULL,                         /* data block ptr   */
                                      ds3gi_cm_client_id, 
                                      pkt_state);
  if (cm_ret_val == FALSE)
  {
    ERR("CM cmd_pkt_state failed", 0, 0, 0);
  }
} /* ds3g_msh_set_pkt_state() */

#endif /* FEATURE_DATA */
