#ifndef DS3GMGR_H
#define DS3GMGR_H
/*===========================================================================

                3 G   D A T A   S E R V I C E S   M A N A G E R

                           H E A D E R   F I L E

DESCRIPTION
  This file contains functions and definitions exported by 3G Dsmgr to other
  Data Services software units, namely: the top-level task dispatcher, ATCoP
  and Mode-Specific Handlers.

  Copyright (c) 2001 by QUALCOMM Incorporated. All Rights Reserved.
  Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gmgr.h_v   1.5   28 Feb 2003 18:54:50   rchar  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/inc/ds3gmgr.h#1 $ $DateTime: 2008/04/11 12:46:53 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/06   an      Featurize ATCOP, RMSM, Async Fax
05/21/03   vas     Added DS3G_IGNORE enum to ds3g_incom_e_type.
04/18/03   ak      Added parameter to switch_calls interface, to support
                   bypass alert in 1x.
02/26/03   rc      Added data_suspend_ind_handler() and 
                   data_resume_ind_handler()
11/04/02   tmr     Added call_was_connected paramter to call_ended_handler
10/14/02   vsk     Added support to get statistics for the call which has ended
08/19/02   ak      Updated for multiple calls.
08/13/02   dgy     Added return value to get_call_connection_info_handler() 
                   and get_data_counters_handler().
08/02/02   dgy     Added support for UI data call screen.
07/24/02   atp     Added DS3G_ALERT_USER to ds3g_incom_e_type enum.
07/16/02   ak      Incoming call handler also takes cmd_ptr.
07/01/02   ak      add cm_srv_type to orig_hdlr prototype.
06/18/02   tmr     Added the delay_disconnect paramter to user_end_handler() and
                   added the complete_ll_disconnect_handler()
06/05/02   ar      Removed online-command support wrapper functions. 
                   Added at_return_to_online_data_complete_handler().
05/21/02   smp     Removed ds3g_mode_e_type definition. Use sys_sys_mode_e_type
                   instead.
05/16/02   smp     Updates for Multimode CM.
05/12/02   ak      Updated parameters for mode-specific handlers.
04/30/02   ak      Changed call_id_hdlr signature to take mode_spec info.
04/30/02   ak      Changed orig_hdlr signature.  Changed ds3g_initiate_call()
                   signature.
04/19/02   vsk     Added PDP context preservation support
04/12/02   rc      Removed enum DS3G_CALL_TYPE_E_TYPE. Added parameter
                   stack_type to originate_call_handler,
                   incoming_call_handler, and ds3g_initiate_call().                   
02/25/02   tmr     Added complete_ll_connect_handler() to mode-specific
                   handler function table
02/19/02   rlp     Code review changes.
02/13/02   vsk     removed call_id in call orig handler function and also
                   changed prototypes to use cm_call_id_type
02/07/02   vsk     Added ds3gi_get_current_networ_mode() function prototype
02/05/02   rlp     Added online-command mode support.
01/11/02   smp     Merged from packet data branch. Added end_params to call
                   confirm, call connected and user end handlers, since CM
                   now needs this.
12/14/01   smp     Added function prototype for ds3g_stop_alerting() and
                   renamed ds3g_ring_serial_port() to ds3g_alert_user().
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

#include "rex.h"
#include "cm.h"
#include "dsm.h"
#include "dstask.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Action to be taken when an incoming call is received. This is determined by
  the mode-specific handler.
---------------------------------------------------------------------------*/
typedef enum
{
  DS3G_ANSWER_CALL,                /* Answer the incoming call immediately */
  DS3G_REJECT_CALL,                /* Reject the incoming call             */
  DS3G_ALERT_USER,                 /* Ring laptop                          */
  DS3G_DO_NOTHING,                 /* do nothing (may be ringing TE...?)   */
  DS3G_IGNORE                      /* Ignore the incoming call             */
} ds3g_incom_e_type;

/*---------------------------------------------------------------------------
  Result returned by mode-specific handler functions.
---------------------------------------------------------------------------*/
#define DS3G_SUCCESS        1
#define DS3G_FAILURE        0

/*---------------------------------------------------------------------------
  Table of handler functions for circuit-switched data calls. Each circuit-
  switched mode-specific handler maintains such a table, and registers this
  table with 3G Dsmgr.
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
    This handler is invoked when the user initiates a data call. The mode-
    specific handler should specify the orig_params for the technology
    it will use.  The *_params_changed for the technology should also be set
    to TRUE.  The mode_spec_orig_info_ptr contains data that is passed to
    CM by the entity that wants to originate the call.  Current call models
    have the mode-specific handler (or some mode-specific entity) making 
    that call.  The information within the struct is defined by the mode-spec
    modules.
    
    User must also fill out the SRV type of his call.
  -------------------------------------------------------------------------*/
  boolean (*originate_call_handler)
                  (
                    const byte                     *dial_string,                    
                    uint8                           dial_string_len,
                    cm_cdma_orig_params_s_type     *cdma_orig_params_ptr,
                    boolean                        *cdma_params_changed,
                    cm_gw_cs_orig_params_s_type    *gw_cs_orig_params_ptr,
                    boolean                        *gw_cs_params_changed,
                    cm_gw_ps_orig_params_s_type    *gw_ps_orig_params_ptr,
                    boolean                        *gw_ps_params_changed,
                    cm_srv_type_e_type             *cm_srv_type,
                    void                           *mode_spec_info_ptr
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked after the origination, in order to pass the call
    id allocated by Call Mgr to the mode-specific handler.  The mode_spec
    info is passed into ds3g_initiate_call, and passed back to the mode_hdlr
    in this call.
  -------------------------------------------------------------------------*/
  void (*call_id_handler)( 
                           cm_call_id_type    call_id,
                           void               *mode_spec_info_ptr
                         );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_CALL_CONF event is rx'ed
    from Call Mgr. The mode_info_ptr points to mode-specific information
    provided by Call Mgr. The mode-specific handler should validate any
    parameters that may have been proposed by the network. A return value of
    SUCCESS indicates that the parameters are acceptable, FAILURE indicates
    that the parameters are not acceptable i.e. the call should be released.
  -------------------------------------------------------------------------*/
  boolean (*call_conf_handler)
                         (
                           cm_call_id_type            call_id,
                           cm_call_mode_info_s_type   *mode_info_ptr,
                           cm_end_params_s_type       *end_params
                         );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_CONNECT event is rx'ed
    from Call Mgr. The mode_info_ptr points to mode-specific information
    provided by Call Mgr. The mode-specific handler should set up the lower
    layers for data transfer, and optionally, set change_serial_mode to TRUE
    (and specify the additional serial parameters) if the serial mode needs
    to be changed. A return value of SUCCESS indicates that the data path is
    established, FAILURE indicates that the lower layers could not be set up
    i.e. the call should be released.  The delay_connect parameter should be
    set to FALSE if the call should be connected immediately, and TRUE if
    the connection should be delayed.  If connection is delayed, the 
    call_connected_complete_handler() will be called when connection is 
    completed. 
  -------------------------------------------------------------------------*/
  boolean (*call_connected_handler)
                          (
                            cm_call_id_type           call_id,
                            cm_call_mode_info_s_type  *mode_info_ptr,
                            boolean                   *delay_connect,
                            cm_end_params_s_type      *end_params
                          );
                
  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_RAB_REL_IND event is 
    rx'ed from Call Mgr. The mode-specific handler should make a note that
    the radio link  interface has been released and no data transfer 
    can take place on the radio link interface. 
    A return value of SUCCESS indicates that mode specific handler
    has de-coupled the protocol layers from the radio link interface .
    FAILURE indicates that the lower layers could not be decoupled.
    i.e. the call should be released.
  -------------------------------------------------------------------------*/
  boolean (*rab_rel_handler)(
                              cm_call_id_type          call_id,
                              cm_end_params_s_type     *end_params
                            );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_RAB_REESTAB_IND event is 
    rx'ed from Call Mgr. The mode-specific handler should make a note that
    the radio link interface has been re-establised and ready
    for data transfer. The mode specific handler should bind the appropriate
    protocol layers to the radio link interface 
    A return value of SUCCESS indicates that mode specific handler
    has coupled the protocol layers to  the radio link interface.
    FAILURE indicates that the lower layers could not be coupled.
    i.e. the call should be released.
  -------------------------------------------------------------------------*/
  boolean (*rab_re_estab_ind_handler)(
                                       cm_call_id_type           call_id,
                                       cm_call_mode_info_s_type  *mode_info_ptr,
                                       cm_end_params_s_type      *end_params
                                     );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_RAB_REESTAB_REJ event is 
    rx'ed from Call Mgr. 
    A return value of SUCCESS indicates that mode specific handler
    has either started a call release procedure or ignored the event
    FAILURE indicates that the call cannot proceed further and the call should
    be released.      
  -------------------------------------------------------------------------*/
  boolean (*rab_re_estab_rej_handler)(
                                       cm_call_id_type          call_id,
                                       cm_end_params_s_type     *end_params
                                     );
                
  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_RAB_REESTAB_FAIL event is 
    rx'ed from Call Mgr. 
    A return value of SUCCESS indicates that mode specific handler
    has either started a call release procedure or ignored the event
    FAILURE indicates that the call cannot proceed further and the call should
    be released.      
  -------------------------------------------------------------------------*/
  boolean (*rab_re_estab_fail_handler)(
                                        cm_call_id_type         call_id,
                                        cm_end_params_s_type    *end_params
                                      );

  /*-------------------------------------------------------------------------
    This handler is invoked when the DS_COMPLETE_LL_CONNECT_CMD event is 
    rx'ed from mode-specific handlers if delay_connect was set to TRUE when
    the call_connected_handler() was called. The mode-specific handler should 
    set up the lower layers for data transfer, and optionally, set 
    change_serial_mode to TRUE
    (and specify the additional serial parameters) if the serial mode needs
    to be changed. A return value of SUCCESS indicates that the data path is
    established, FAILURE indicates that the lower layers could not be set up
    i.e. the call should be released.  
  -------------------------------------------------------------------------*/
  boolean (*complete_ll_connect_handler)(
                                          cm_call_id_type        call_id,
                                          cm_end_params_s_type   *end_params
                                        );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_SETUP_IND event is rx'ed
    from Call Mgr. The mode_info_ptr points to mode-specific information
    provided by Call Mgr. The mode-specific handler should validate the setup
    parameters proposed by the network, and specify the setup_res_params. A 
    return value of SUCCESS indicates that the setup was accepted, FAILURE
    indicates that the setup was rejected.
  -------------------------------------------------------------------------*/
  boolean (*setup_handler)(
                            cm_call_id_type               call_id,
                            cm_call_mode_info_s_type      *mode_info_ptr,
                            cm_setup_res_params_s_type    *setup_res_params
                          );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_INCOM event is rx'ed from
    Call Mgr. The mode-specific handler should specify the ans_params and
    ring type, and optionally, set change_serial_mode to TRUE (and specify
    the additional serial parameters) if the serial mode needs to be changed.
    The return value indicates the action to be taken: answer, notify user or
    reject call. The serial mode is changed only if the return value
    indicates answer.
  -------------------------------------------------------------------------*/
  ds3g_incom_e_type (*incoming_call_handler)
                                        (
                                          ds_cmd_type            *cmd_ptr,
                                          cm_call_id_type         call_id,
                                          cm_ans_params_s_type   *ans_params,
                                          cm_end_params_s_type   *end_params
                                        );

  /*-------------------------------------------------------------------------
    This handler is invoked when the user or TE answers the call. The mode-
    specific handler should specify the ans_params, and optionally, set
    change_serial_mode to TRUE (and specify the additional serial parameters)
    if the serial mode needs to be changed.
  -------------------------------------------------------------------------*/
  void (*user_answer_handler)(
                               cm_call_id_type              call_id,
                               cm_ans_params_s_type         *ans_params
                             );
  /*-------------------------------------------------------------------------
    This handler is invoked when the user or TE ends the call. The mode-
    specific handler may attempt to shut down lower layer protocols.  If
    the handler would like to delay the disconnect, the delay_disconnect
    paramter should be set to TRUE.  In this case the end_params data will
    be ignored.  If delaying the disconnect is not desired, delay_disconnect
    must be set to FALSE, and the end_params data must be filled in.
  -------------------------------------------------------------------------*/
  void (*user_end_handler)( 
                            cm_call_id_type            call_id,
                            cm_end_params_s_type       *end_params,
                            boolean                    *delay_disconnect,
                            void                       *user_info_ptr
                          );

  /*-------------------------------------------------------------------------
    This handler is invoked when the DS_COMPLETE_LL_DISCONNECT_CMD event is 
    rx'ed from lower layers if delay_disconnect was set to TRUE when
    the user_end_handler() was called.  The handler must fill in the 
    end_params data prior to returning.
  -------------------------------------------------------------------------*/
  void (*complete_ll_disconnect_handler)
                                   (
                                     cm_call_id_type           call_id,
                                     cm_end_params_s_type     *end_params
                                   );
  
  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_END event is rx'ed from
    Call Mgr. The mode-specific handler should clean up all call related
    state and shut down protocol layers.  
    
    The cmd_ptr points to the command received by DS from CM.  Contains
    details about why the call ended.  The call_was_connected paramter
    indicates to the mode-specific handler whether or not the call was
    in the connected state when ended.
  -------------------------------------------------------------------------*/
  void (*call_ended_handler)( 
                              ds_cmd_type               *cmd_ptr,
                              boolean                   call_was_connected
                            );

  /*-------------------------------------------------------------------------
    This handler is used when a mode-specific handler has an origination 
    in progress when an incoming page of the same type appears.  Mode-spec
    handler must end originating call.  It can accept or reject or do 
    nothing with incoming call (normal incoming call processing).
    
    There are two end_params.  The first is for the originating call, which
    is always ended.  The second is for the incoming call.  That call may
    also be ended (but is not required to be ended).
  -------------------------------------------------------------------------*/
  ds3g_incom_e_type (*switch_call_handler)
                                (
                                  ds_cmd_type           *cmd_ptr,
                                  cm_call_id_type        inc_call_id,
                                  cm_call_id_type        exist_call_id,
                                  cm_ans_params_s_type  *ans_params,
                                  cm_end_params_s_type  *end_params_orig,
                                  cm_end_params_s_type  *end_params_inc
                                );
  /*-------------------------------------------------------------------------
    This handler is used when there is an incoming call which will override
    an originating call.  This tells the mode-speci handler that their
    call should be ended immediately.
    
    NOTE:  the mode-specific handler will not get a call_ended() after this.
    This is the only indication the mode-specific handler will get that
    their call has been torn down.
    
    A return of TRUE indicates that there was a call to be torn down.
    FALSE indicates that the mode-spec handler had no active/originating
    calls.
  -------------------------------------------------------------------------*/
  boolean (*force_call_down_handler)
                               (
                                 cm_call_id_type        exist_call_id,
                                 cm_end_params_s_type  *end_params
                               );

  /*-------------------------------------------------------------------------
    This handler is invoked from the function ds_get_call_information(),
    which is invoked by UI when a call is established. The mode-specific 
    handler should return by reference the call connection information, which
    is currently a NULL-terminated string indicating the system or service 
    option, e.g., "WCDMA", for the call. The handler checks the size of the 
    buffer for *system_information before it populates the buffer. The return
    value indicates if the call connection information is valid.
  -------------------------------------------------------------------------*/
  boolean (*get_call_connection_info_handler)
                         (
                            cm_call_id_type     call_id,
                            byte               *system_information, 
                            byte                max_system_information_length
                         );
  
  /*-------------------------------------------------------------------------
    This handler is invoked from the function 
    ds_get_current_session_data_counters(), which is periodically invoked by
    UI during a data call. The mode-specific handler should return by 
    reference the total count of bytes transmitted and received at RLC for 
    the current call. The total byte count is accumulative for the call. If 
    RAB is re-established after PDP context is preserved during the call, the
    total byte count includes bytes transmitted and received after RAB is 
    re-established. The return value indicates if the byte count is valid.
  -------------------------------------------------------------------------*/
  boolean (*get_data_counters_handler)
                              (
                                 cm_call_id_type    call_id,
                                 uint32            *rx_from_network_byte_cnt, 
                                 uint32            *tx_to_network_byte_cnt
                              );

  /*-------------------------------------------------------------------------
    This handler is invoked from the function 
    ds_get_last_session_data_counters(), which is invoked by
    UI at the end of a data call. The mode-specific handler should return by 
    reference the total count of bytes transmitted and received at Link Layer
    for the previous call. 
    DS just returns the counter values for the last call which terminated 
    or the current call (which could be in the process of being released)

    The total byte count is accumulative for the call. If 
    RAB is re-established after PDP context is preserved during the call, the
    total byte count includes bytes transmitted and received after RAB is 
    re-established. The return value indicates if the byte count is valid.
  -------------------------------------------------------------------------*/
  boolean (*get_last_session_data_counters_handler)
                              (
                                 uint32            *rx_from_network_byte_cnt, 
                                 uint32            *tx_to_network_byte_cnt
                              );

  /*-------------------------------------------------------------------------
    This handler is invoked when CM sends a service change event with
    suspend = TRUE. The mode-specific handler may attempt to shut down  
    connection to the old RAT. 
    The call id is not passed into this function.
    The mode-specific handler should do the required processing for all
    active calls.
  -------------------------------------------------------------------------*/
  void (*data_suspend_ind_handler)(void);

  /*-------------------------------------------------------------------------
    This handler is invoked when CM sends a service change event with
    suspend = FALSE. The mode-specific handler may attempt to resume
    connection to the new RAT.  
    The call id is not passed into this function.
    The mode-specific handler should do the required processing for all
    active calls.
  -------------------------------------------------------------------------*/
  void (*data_resume_ind_handler)(void);


}ds3g_hdlr_func_tbl_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

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

extern void  ds3g_init( void );

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

extern void  ds3g_process_cmds
(
  ds_cmd_type  *cmd_ptr                         /* Command to be processed */
);



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

extern void  ds3g_register_mode_sp_handler
(
  sys_sys_mode_e_type             mode,        /* Network mode             */
  ds_call_type_e_type             call_type,   /* Call type-Circuit or Pkt */
  ds3g_hdlr_func_tbl_type        *hdlr_tbl_ptr /* Handler function table   */
);


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

extern sys_sys_mode_e_type  ds3g_get_current_network_mode( void );




#endif /* FEATURE_DATA */

#endif /* DS3GMGR_H */
