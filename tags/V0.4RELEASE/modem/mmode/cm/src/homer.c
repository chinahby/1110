/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
                      H A N D O F F   M A N A G E R

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This module is the main implementation for the Handoff Manager. 
  Homer is responsible for facilitating the following
  1. Traffic handover from UMTS -> 1x
  2. Traffic handover from VoIP on DO -> 1x


EXTERNALIZED FUNCTIONS

REGIONAL FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/homer.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
12/07/07   ra      Added support for IMS VCC in HOMER files 
06/22/07   sk      Added support for ATOM.
06/11/07   pk      VoIP -> 1x hho enahncements / CR fixes
06/05/07   pk      Added support to read mob_cai_rev from NV
05/09/07   pk      Fixed page fault caused due to ip_app pointer being NULL
02/02/07   pk      Send the HDR get message config response to HDR
01/25/07   pk      Changing Tunnled Message type to uint8
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
12/01/06   ic      Added homer_dummy() function to get rid of compiler 
                   Warning: C3040E: no external declaration in translation 
                   unit   
11/16/06   pk      Lint Fixes
10/11/06   pk      Initial release.
===========================================================================*/

/*
                    HOMER VoIP -> 1X HHO STATE MACHINE
                    ==================================

                              +-----------+ Valid CSNA, SRDM Received, VoIP in Progress
                              |           |------------------------------+
                              |   NONE    |                              |
                              |   (0)     | VoIP Connect, Valid CSNA     |
                              |           |----------------------+       |
                              +-----------+                      |       |
                                   /|\                           |       |
                                    |                            |       |
                                    |                            |       |
                                    |                            |       |
                                    |                           \|/     \|/
 +----------+   1X Call Connect     |                          +-----------+
 | WAIT_1X_ |---------------------->|<-------------------------| WAIT_SRDM |
 | CALL_    |   1X Call Fail        |         +--------------->|    (1)    |
 | CONNECT  |---------------------->|         |                +-----------+
 |    (6)   |---------------------->|         |                   |      |
 +----------+   Call End            |         |           VoIP    |      |
     /|\                            |         |         Connected |      |
      |                             |         |           SRDM    |      | SRDM 
      | VoIP Call                   |         |          Already  |      | Received
      | End                         |         |          Received |      |
      |                             |         |                  \|/    \|/
 +------------+    1X Call Fail     |         |   1X Call Fail  +----------+
 | WAIT_VOIP_ |-------------------->|         |<----------------| WAIT_MC_ |
 | CALL_END_  |                     |         |                 | ORIG     |
 | CNF        |-------------------->|<------- | ----------------|   (2)    |
 |    (5)     |     Call End        |         |   Call End      +----------+
 +------------+                     |         |                      |
      /|\                           |         |                      |
       | VoIp call                  |         |                      |  ORM 
       | end request                |         |                      | Received
       |                            |         |                      |
       |                            |         |                     \|/
 +-----------+    1X Call Fail      |         |  1X Call Fail  +-----------+
 | WAIT_VOIP |--------------------- | ------->+<---------------| WAIT_UHDM |
 | CALL_END  |                      |<-------------------------|    (3)    |
 |    (4)    |----------------------+        Call End          +-----------+
 +-----------+    Call End                                           |
      /|\                                                            |
       |                                                             |
       +-------------------------------------------------------------+
                               UHDM Received

----------------------------------------------------------------------------      
*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "customer.h"  /* Customer configuration file */

#if defined( FEATURE_UMTS_1X_HANDOVER) || \
    defined(FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X) || \
    defined(FEATURE_IMS_VCC)
#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif

#include "homeri.h"

#ifdef FEATURE_UMTS_1X_HANDOVER
#error code not present
#endif

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif

#ifdef FEATURE_HDR_CSNA
#error code not present
#endif

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif

/*===========================================================================

FUNCTION homer_ptr

DESCRIPTION
  Return a pointer to the handoff manager object  
DEPENDENCIES
 

RETURN VALUE
  None
 
SIDE EFFECTS
  None
  
===========================================================================*/
static homer_s_type* homer_ptr()
{

  static homer_s_type homer_state;
  return &homer_state;

}

#ifndef FEATURE_IMS_VCC
/*===========================================================================

FUNCTION homer_update_state

DESCRIPTION
  Return a pointer to the handoff manager object  
DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void homer_update_state( 

  homer_state_e_type new_state
    /* The new state for the handoff
    */
)
{
  homer_s_type* handoff_ptr = homer_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( handoff_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( handoff_ptr->state == new_state )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH("Handoff state: New %d old %d",new_state, handoff_ptr->state, 0);

  handoff_ptr->state = new_state;
  return;

}
#endif /*FEATURE_IMS_VCC*/

/*===========================================================================

FUNCTION homer_get_state

DESCRIPTION
  Retruns the current homer state
DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
homer_state_e_type          homer_get_state( void )
{

  homer_s_type* handoff_ptr = homer_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( handoff_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (handoff_ptr->state );
}

/*===========================================================================

FUNCTION homer_init

DESCRIPTION
  Retruns the current homer state
DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void                        homer_init( void )
{

  homer_s_type* handoff_ptr = homer_ptr();
  #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( handoff_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  handoff_ptr->state        = HOMER_STATE_NONE;
  handoff_ptr->call_id      = CM_CALL_ID_UNASSIGNED;
  handoff_ptr->ho_req_rcvd  = FALSE;
  handoff_ptr->end_cnf_rvcd = FALSE;

  #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
  #endif

  #ifdef FEATURE_IMS_VCC
  /** Initialize the parameters specific to IMS HOM
  */
  homer_update_handoff_info(CM_CALL_ID_INVALID, CM_CALL_ID_INVALID);
  #endif

  return;

}

#ifdef FEATURE_IMS_VCC
/*===========================================================================

@FUNCTION homer_update_handoff_info

@DESCRIPTION
  Updates or initializes the structure containing IMS HOM handoff call info.

  
@DEPENDENCIES
  none

@RETURN VALUE
  none

@SIDE EFFECTS
  none

@x
===========================================================================*/


void homer_update_handoff_info(

    cm_call_id_type    switch_out_call_id,
    /**< Switch out call id
    */

    cm_call_id_type    switch_in_call_id
    /**< Switch in call id
    */
    )
{

   homer_s_type        *handoff_call_info_ptr = homer_ptr();

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   CM_ASSERT( handoff_call_info_ptr != NULL );

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   handoff_call_info_ptr->call_id = switch_out_call_id;
   handoff_call_info_ptr->switch_in_call_id = switch_in_call_id;

}


/*===========================================================================

@FUNCTION homer_chk_orig_abort_in_switch_in_domain

@DESCRIPTION
  If the origination for switch in call failed and if switch out 
  domain call conditions improve, then abort the origination process

  
@DEPENDENCIES
  none

@RETURN VALUE
  -TRUE if the switch in call origination should be aborted
  -FALSE if the switch in call origination should not be aborted

@SIDE EFFECTS
  none

@x
===========================================================================*/
boolean homer_chk_orig_abort_in_switch_in_domain(


    cmcall_s_type    *call_ptr
    /**< pointer to a call object. */
    )
/*lint -esym(715, call_ptr) */
/*lint -esym(818, call_ptr) */

{

   homer_s_type        *handoff_call_info_ptr = homer_ptr();
   cmss_s_type          *ss_ptr = cmss_ptr();

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   CM_ASSERT( handoff_call_info_ptr != NULL );
   CM_ASSERT( ss_ptr != NULL );
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if( call_ptr == NULL )
   {
       return FALSE;
   }
     

   /** Abort Switch in call origination in the following two conditions:
   ** a) Switch in call is originated in WLAN domain, i.e a VOIP call 
   **    and WLAN conditions become POOR
   ** b) Switch in call is originated in CS domain, i.e a GSM/WCDMA call
   **    and WLAN conditions become GOOD
   */

   if( (call_ptr->call_id == handoff_call_info_ptr->switch_in_call_id) 
                            &&
       ( (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP &&
          ss_ptr->info.wlan_stats.wlan_link_quality == SYS_WLAN_LINK_QUAL_POOR)
                                ||
          (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS &&
           ss_ptr->info.wlan_stats.wlan_link_quality == SYS_WLAN_LINK_QUAL_GOOD) ) )
   {
       return TRUE;
   }

   return FALSE;
}
/*lint +esym(715, call_ptr) */
/*lint +esym(818, call_ptr) */

#endif /*FEATURE_IMS_VCC*/

#ifdef FEATURE_HDR_CSNA
#error code not present
#endif /* FEATURE_HDR_CSNA */


#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

#ifdef FEATURE_UMTS_1X_HANDOVER
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER */

#ifdef FEATURE_IMS_VCC
/*===========================================================================

@FUNCTION homer_umts_wlan_handover_proc

@DESCRIPTION
  Processes call events related to handoff calls. Checks 
  - if the call events need to be suppressed
  - if handover is complete and call objects need to be swapped

@DEPENDENCIES
  None.

@RETURN VALUE
 -TRUE if other client should be notified about the call event
 -FALSE if the other clients should not be notified

@SIDE EFFECTS
  None.

@x
===========================================================================*/
boolean homer_umts_wlan_handover_proc(
            
    cmcall_s_type    *call_ptr,
        /* pointer to a call object. */

    cm_call_event_e_type    call_event
        /* notify client list of this call event */
    
    )
/*lint -esym(715, call_ptr) */
/*lint -esym(818, call_ptr) */

{

   homer_s_type                  *handoff_call_info_ptr = homer_ptr();
   cmcall_s_type                 *switch_out_call_ptr = NULL;
   boolean                       notify_client = TRUE;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   CM_ASSERT( handoff_call_info_ptr != NULL );

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /** Get call pointers for switch-in and switch-out call ids, if
    **  handoff is initiated
   */

   if ( (handoff_call_info_ptr->call_id == CM_CALL_ID_INVALID) ||
      (handoff_call_info_ptr->switch_in_call_id == CM_CALL_ID_INVALID) )
   {
       return TRUE;
   }

   switch_out_call_ptr = cmcall_ptr(handoff_call_info_ptr->call_id);
  

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
   CM_ASSERT( call_ptr != NULL );
   CM_ASSERT( BETWEEN(call_event, CM_CALL_EVENT_NONE, CM_CALL_EVENT_MAX) );

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /** Support suppression of call events (after handoff is initiated) for
   *   switch-in & switch-out calls in the following scenarios.
   ** New corresponding call event should be generated whenever required
   **
   ** a) Supress all call events related to switch-in call, 
   *     except the CM_CALL_EVENT_END event when the switch-out call is
   *     already released
   ** b) CM_CALL_EVENT_END event for switch out call
   *  c) CM_CALL_EVENT_END_REQ event for both switch in and switch out calls
   *     should not be delivered to any client
   **
   */

   /*-----------------------------------------------------------------------*/
   if ( call_ptr->call_id == handoff_call_info_ptr->switch_in_call_id )
   {
       notify_client = FALSE;

       if( call_event == CM_CALL_EVENT_END )
       {
          if( switch_out_call_ptr == NULL )
          {
              /**< The call has ended, before handover could be completed 
              */
              notify_client = TRUE;
          }
         
          /** Clear the handoff call information stored in CM
          */
          homer_update_handoff_info(CM_CALL_ID_INVALID, CM_CALL_ID_INVALID);
       }

   }

   /*-----------------------------------------------------------------------*/
   if( (call_ptr->call_id == handoff_call_info_ptr->call_id)&&
     (call_event == CM_CALL_EVENT_END ) )
   {
       notify_client = FALSE;
   }

   /*------------------------------------------------------------------------*/

   if( (call_event == CM_CALL_EVENT_END_REQ) && 
      (call_ptr->call_id == handoff_call_info_ptr->call_id) )
   {
       notify_client = FALSE;
   }

   /** check if handoff is completed after siwtch-in call connect
   */
   if( call_event == CM_CALL_EVENT_CONNECT )
   {
       homer_chk_umts_wlan_handoff_complete(call_ptr->call_id,CM_CALL_EVENT_CONNECT);
   }
   
   return notify_client;
}
/*lint +esym(715, call_ptr) */
/*lint +esym(818, call_ptr) */


/*===========================================================================

@FUNCTION homer_chk_umts_wlan_handoff_complete

@DESCRIPTION
  To check if the wlan umts handover is complete.
  When complete, switch call objects and reset handoff info

@DEPENDENCIES
  None.

@RETURN VALUE
 None.

@SIDE EFFECTS
  None.

@x
===========================================================================*/
void homer_chk_umts_wlan_handoff_complete(

    cm_call_id_type    call_id,
    /**< call id */

    cm_call_event_e_type    call_event
    /**< notify client list of this call event */)
{

   homer_s_type                  *handoff_call_info_ptr = homer_ptr();
   cmcall_s_type                 *switch_out_call_ptr = NULL;
   cmcall_s_type                 *switch_in_call_ptr = NULL;
   boolean                        ret = FALSE;
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   CM_ASSERT( handoff_call_info_ptr != NULL );

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
   /** Get call pointers for switch-in and switch-out call ids, if handoff
    *  is initiated
   */

   if ( (handoff_call_info_ptr->call_id == CM_CALL_ID_INVALID) ||
      (handoff_call_info_ptr->switch_in_call_id == CM_CALL_ID_INVALID) )
   {
       return;
   }

   switch_out_call_ptr = cmcall_ptr(handoff_call_info_ptr->call_id);
   switch_in_call_ptr = cmcall_ptr(handoff_call_info_ptr->switch_in_call_id);
  
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   CM_ASSERT( BETWEEN(call_event, CM_CALL_EVENT_NONE, CM_CALL_EVENT_MAX) );

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /** Check if handover is complete. When completed, switch call
   *  ids and clear handoff call info stored in CM.
   ** Handover is considered complete in the following scenarios
   *  - Call event END received for switch out call and the switch in call
   *  is already connected
   *  - Call event CONNECT received for switch in call and switch out is
   *  already ended
   * */

   if(   (  (switch_in_call_ptr != NULL)&&
            (call_id == handoff_call_info_ptr->call_id)&&
            (call_event == CM_CALL_EVENT_END ) &&         
            (switch_in_call_ptr->call_state == CM_CALL_STATE_CONV)  )
                                    ||
        (   (call_id == handoff_call_info_ptr->switch_in_call_id)&&
            (call_event == CM_CALL_EVENT_CONNECT ) &&         
            (switch_out_call_ptr == NULL)  )    )
   {

       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

       /** Inform the VOIP application that call id has been swapped */
       if( switch_in_call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP )
       {

           cmipapp_cb_s_type  *app_cb_ptr      = NULL;

           /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

           /* Call object has the app_id that delivered the call
           */

           app_cb_ptr = cmipapp_get_cb_ptrs(switch_in_call_ptr->ipapp_id);

           if( app_cb_ptr == NULL )
           {
               CM_ERR ("CS->WLAN, App Call back ptr is NULL", 0, 0, 0);
           }

           /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

           CM_ASSERT( app_cb_ptr != NULL );

           /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

           /*lint -save -e774 Boolean within 'if' always evaluates to False */
           /* Send a SIP notify to the sip server
           */
           if ( app_cb_ptr != NULL &&
                app_cb_ptr->cmipapp_cmd_notify_ho_func_ptr != NULL &&
                !app_cb_ptr->cmipapp_cmd_notify_ho_func_ptr(
                           CMIPAPP_HO_TYPE_CS_WLAN,
                           handoff_call_info_ptr->switch_in_call_id,
                           handoff_call_info_ptr->call_id,
                           switch_in_call_ptr->ipapp_data_block))
               /*lint -restore */ 
              {
               CM_ERR ("HDR->1X, app ho ind. failed", 0, 0, 0);
              }

       }

       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       
       ret = cmcall_switch_call_id(handoff_call_info_ptr->call_id , 
                                handoff_call_info_ptr->switch_in_call_id);

       if( ret )
       {
           homer_update_handoff_info(CM_CALL_ID_INVALID, CM_CALL_ID_INVALID);
       }
           

   }

}

/*===========================================================================

@FUNCTION homer_end_switch_in_call

@DESCRIPTION
  End the switch-in call

@DEPENDENCIES
  None.

@RETURN VALUE
 None.

@SIDE EFFECTS
  None.

@x
===========================================================================*/

void homer_end_switch_in_call(

    cmcall_s_type *call_ptr
    /**< Pointer to the call object*/

    )
/*lint -esym(818, call_ptr) */

{
   homer_s_type                  *handoff_ptr = homer_ptr();
   cmcall_s_type                 *switch_in_call_ptr = NULL;
    
   CM_ASSERT( handoff_ptr != NULL );

   if( (call_ptr == NULL) || (call_ptr->call_id != handoff_ptr->call_id) )
   {
       return;
   }
    

   if( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_NONE )
   {
       call_ptr->end_status = CM_CALL_END_CLIENT_END;
   }

   if( call_ptr->end_status == CM_CALL_END_CLIENT_END )
   {
       /** call is ended by the user, so end the
       *  switch-in call as well */

       if( handoff_ptr->switch_in_call_id != CM_CALL_ID_INVALID )
       {
           switch_in_call_ptr = cmcall_ptr (handoff_ptr->switch_in_call_id);

           if( switch_in_call_ptr != NULL )
           {
               switch_in_call_ptr->end_status = CM_CALL_END_CLIENT_END;
               cmcall_end( switch_in_call_ptr );          
           }
       }

   }
}

/*lint +esym(818, call_ptr) */

    
/*===========================================================================

@FUNCTION homer_copy_call_info

@DESCRIPTION
  Copy the call info from switch out call pointer to switch in call pointer 
  if the switch out call is ending.

@DEPENDENCIES
  None.

@RETURN VALUE
 None.

@SIDE EFFECTS
  None.

@x
===========================================================================*/
void homer_copy_call_info(

    cmcall_s_type *call_ptr
    /**< pointer to a call object. */

    )
/*lint -esym(715, call_ptr) */
/*lint -esym(818, call_ptr) */

{
   homer_s_type                  *handoff_ptr = homer_ptr();
   cmcall_s_type                 *switch_in_call_ptr = NULL;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   CM_ASSERT( handoff_ptr != NULL );

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /** Copy call infor for switch_out call only  */
   if( (call_ptr == NULL) || (call_ptr->call_id != handoff_ptr->call_id) )
   {
       return;
   }
     
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   switch_in_call_ptr = cmcall_ptr(handoff_ptr->switch_in_call_id);

   /** Copy call_info   */

   if( switch_in_call_ptr!=NULL )
   {
       switch_in_call_ptr->send_call_event_end     = call_ptr->send_call_event_end;
       switch_in_call_ptr->is_user_originated_call = call_ptr->is_user_originated_call;
       switch_in_call_ptr->call_connect_time       = call_ptr->call_connect_time;
       switch_in_call_ptr->bytes_received          = 
            switch_in_call_ptr->bytes_received + call_ptr->bytes_received;
       switch_in_call_ptr->bytes_transmitted       = 
            switch_in_call_ptr->bytes_transmitted + call_ptr->bytes_transmitted;
       switch_in_call_ptr->direction               = call_ptr->direction;
   }

}
/*lint +esym(715, call_ptr) */
/*lint +esym(818, call_ptr) */


/*===========================================================================

@FUNCTION homer_get_connection_id

@DESCRIPTION
  Get connection id from call id.

@DEPENDENCIES
  None.

@RETURN VALUE
 connection id

@SIDE EFFECTS
  None.

@x
===========================================================================*/
byte homer_get_connection_id(

    homer_handoff_call_id_e_type    call_id
    /**< call id */

    )
/*lint -esym(818, call_ptr) */

{
   cmcall_s_type  *call_ptr = NULL;
   homer_s_type                    *handoff_ptr = homer_ptr();
 
   if( call_id == HOMER_SWITCH_OUT_CALL_ID )
   {
       call_ptr = cmcall_ptr(handoff_ptr->call_id );
   }
   else if ( call_id == HOMER_SWITCH_IN_CALL_ID )
   {
       call_ptr = cmcall_ptr(handoff_ptr->switch_in_call_id );
   }
   
   /** Return the connection id corresponding to the call_ptr
   */
    
   if ( call_ptr != NULL )
   {
       return call_ptr->gw_connection_id;
   }
   else
   {
       return CM_UNKNOWN_CONN_ID;
   }
     
}
/*lint +esym(818, call_ptr) */

/*===========================================================================

@FUNCTION homer_chk_handover

@DESCRIPTION
  Checks if the handover is in progress or not.
  Used for UT only.
@DEPENDENCIES
  None.

@RETURN VALUE
 -TRUE if handover is going on
 -FALSE if handover is not going on

@SIDE EFFECTS
  None.

@x
===========================================================================*/
boolean homer_chk_handover(void)
{

   homer_s_type                    *handoff_ptr = homer_ptr();

   if( (handoff_ptr->call_id == CM_CALL_ID_INVALID) && 
        (handoff_ptr->switch_in_call_id == CM_CALL_ID_INVALID) )
   {
       return FALSE;
   }
   else
   {
       return TRUE;
   }

}

#endif /*FEATURE_IMS_VCC */
#else /* #if defined( FEATURE_UMTS_1X_HANDOVER) || defined(FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X) || defined(FEATURE_IMS_VCC) */
/*===========================================================================
FUNCTION homer_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void homer_dummy( void )
/*lint -esym(714,homer_dummy) */
/*lint -esym(528,homer_dummy) */
/*lint -esym(765,homer_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}

#endif /* #if defined( FEATURE_UMTS_1X_HANDOVER) || defined(FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X) || defined(FEATURE_IMS_VCC)*/
