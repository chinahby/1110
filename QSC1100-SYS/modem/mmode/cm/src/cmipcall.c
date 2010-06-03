/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   I P  C A L L

                      M O D U L E

GENERAL DESCRIPTION

  This module implements the functionality of handling calls through
  IP. IP call reports are received and call object is updated. Call
  Origination/end functions are implemented to call, registered
  applications that work using IP.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmipcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2005 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmipcall.c#3 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/09   aj      Prevent referencing deallocated memory
12/03/08   cl      Added support for CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL
10/30/08   jd      Added support for IMS registration 
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
08/20/08   st      Klockwork Fixes
06/12/08   sv      Resolve Klocwork defects
01/25/08   vs      Added 0-send call reject support to VOIP call.
12/07/07   ra      Added support for IMS VCC in HOMER files 
08/07/07   ic      Lint clean up
06/22/07   sk      Added support for ATOM.
06/11/07   pk      VoIP -> 1x hho enahncements / CR fixes
03/15/07   ka      Adding INFO_IP to CM_CALL_EVENT_MNG_CALLS_CONF.
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
01/09/07   ka      Replacing call to cmipcall_end with cmcall_end to make 
                   sure GW connection id is unreserved.  
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   ka      Removing the check in Hold Call for call id passed to be 
                   in CONV state.
10/03/06   ka      Adding support for IP call on WCDMA
09/18/06   ka      Moving data_block copy to within switch case block. 
                   Primarily done for MT_INVITE but changed others to make
                   it uniform.
08/08/06   sk      Mapped all VoIP errors to CM_CALL_END_IP_FAIL.
                   Removed unnecessary call end event when processing IP call
                   end reports.
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
                   VoIP.
07/14/06   ka      Fixing the issue where call cmd cb for call end api was
                   being called more than once.
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
05/10/06   ka      mode_spec_end used to bunch calls of particular info_type 
                   and then send them to wcall, ipcall or xcall. Modifying
                   wcall, ipcall and xcall to end calls of their typeonly.
05/07/06   ka      Adding support for activating/deactivating answer hold on 
                   VoIP calls.
02/17/06   ka      Adding support for videoshare
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
01/05/06   ka      Adding support for multi-call hold/activate.         
12/02/05   ka      Fixing compilation warnings.
11/23/05   ka      Modification to allow hold/active for ip calls.
11/16/05   ka      User used to be alerted at the receipt of 
                   CM_CALL_EVENT_SETUP_IND, this behavior is changed to wait 
                   for seperate ringing indication from network to alert user.
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
10/21/05   sk      Removed all unnecessary checks on digit_mode.
10/08/05   ic      Added Header: field 
08/16/05   pk      Added support for priority Queues for origination 
                   parameters
08/16/05   ka      Added mapping for SYS_SYS_MODE_HDR to CMIPAPP enum.
08/02/05   ic      Print value that caused error in default switch case
04/11/05   ka      Changing return type for function
                   cmipcall_map_sys_mode_to_ip_sys_mode ()
04/06/05   ka      Passing system mode to call origination.
03/28/05   ka      Fixed Lint errors.
03/15/05   ka      Fixing RVCT compiler warning, conversion from CM call type
                   to IP call type
03/14/05   ka      cleaning up IP rpt and adding handling call cmd for IP
                   in forward_ll, reply_ll and cmd_check functions.
02/09/05   ic      Lint cleanup
02/03/05   ka      Lint fixes
02/02/05   dk/ka   Initial release
===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
/*lint -esym(766,customer.h) */
#include "customer.h"  /* Customer configuration file */

#if defined(FEATURE_IP_CALL)

#include "cmipappi.h"   /* Includes definitions for VoIP app */
#include "cm.h"        /* External interface to cm.c */
#include "cmi.h"       /* Internal interface to cm.c */
#include "cmdbg.h"     /* CM debug macros */
#include "cmcall.h"    /* Interface to CM call object */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmipcall.h"
#include "cmph.h"      /* Includes phone object */
#include "cmss.h"      /* Includes SS object */

#if defined(FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X) || \
    defined(FEATURE_IMS_VCC)
#include "homeri.h"
#endif

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
                         TUNABLE CALL PARAMETERS
---------------------------------------------------------------------------*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/


/**--------------------------------------------------------------------------
** Functions - internal:
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmipcall_end_cause_to_cmcall_end_status

DESCRIPTION
  Map app end status to CM call end enum.

DEPENDENCIES
  None

RETURN VALUE
  CM call end status that corresponds to App end status

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_end_e_type cmipcall_end_cause_to_cmcall_end_status
(
       cmipapp_end_cause_e_type  end_cause
        /* ipcall cause that needs to be mapped to CM end cause */

)
{
  switch (end_cause)
  {
    case CMIPAPP_END_CAUSE_GENERAL_FAILURE:  return CM_CALL_END_IP_FAIL;

    case CMIPAPP_END_CAUSE_NORMAL:           return CM_CALL_END_REL_NORMAL;

    default:
       CM_MSG_HIGH ("Mapping to default end cause",0,0,0 );
       return CM_CALL_END_IP_FAIL;
  }

} /* cmipcall_end_cause_to_cmcall_end_status () */



/*===========================================================================

FUNCTION cmipcall_map_sysmode_to_cm_sysmode

DESCRIPTION
  Process app reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  None.

RETURN VALUE
  CM system mode that corresponds to App system mode

SIDE EFFECTS
  none

===========================================================================*/
static sys_sys_mode_e_type cmipcall_map_sysmode_to_cm_sysmode(

    cmipapp_sys_mode_e_type      sys_mode
        /* sys mode */
)
{
  switch (sys_mode)
  {
    case CMIPAPP_SYS_MODE_NONE:   return SYS_SYS_MODE_NONE;

    case CMIPAPP_SYS_MODE_CDMA:   return SYS_SYS_MODE_CDMA;

    case CMIPAPP_SYS_MODE_HDR:    return SYS_SYS_MODE_HDR;

    case CMIPAPP_SYS_MODE_GSM:    return SYS_SYS_MODE_GSM;

    case CMIPAPP_SYS_MODE_WCDMA:  return SYS_SYS_MODE_WCDMA;

    case CMIPAPP_SYS_MODE_WLAN:   return SYS_SYS_MODE_WLAN;

    default:
       CM_ERR( "CMIPAPP sys mode %d not mapped to CM sys mode!!!",sys_mode,0,0 );
       CM_ASSERT(FALSE);
       return SYS_SYS_MODE_MAX;

  }

} /* cmipcall_map_sysmode_to_cm_sysmode () */



/*===========================================================================

FUNCTION cmipcall_map_calltype_to_cm_calltype

DESCRIPTION
  Process app reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  None.

RETURN VALUE
  CM call type that corresponds to app call type

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_type_e_type cmipcall_map_calltype_to_cm_calltype(

    cmipapp_call_type_e_type      call_type
        /* call type */
)
{
  switch (call_type)
  {
    case CMIPAPP_CALL_TYPE_VOICE:   return CM_CALL_TYPE_VOICE;

    case CMIPAPP_CALL_TYPE_VT:      return CM_CALL_TYPE_VT;

    case CMIPAPP_CALL_TYPE_VS:      return CM_CALL_TYPE_VS;

    case CMIPAPP_CALL_TYPE_SMS:     return CM_CALL_TYPE_SMS;

    default:
       CM_ERR( "CMIPAPP call type %d not mapped to CM call type !!!",call_type,0,0 );
       CM_ASSERT(FALSE);
       return CM_CALL_TYPE_MAX;
  }

} /* cmipcall_map_calltype_to_cm_calltype () */


/*===========================================================================

FUNCTION cmipcall_map_cm_calltype_to_ip_calltype

DESCRIPTION
  Maps CM call type to CM app call type

DEPENDENCIES
  None.

RETURN VALUE
  app call type that corresponds to CM call type

SIDE EFFECTS
  none

===========================================================================*/
static cmipapp_call_type_e_type cmipcall_map_cm_calltype_to_ip_calltype(

    cm_call_type_e_type      call_type
        /* call type */
)
{
  switch (call_type)
  {
    case CM_CALL_TYPE_VOICE:   return CMIPAPP_CALL_TYPE_VOICE;

    case CM_CALL_TYPE_VT:      return CMIPAPP_CALL_TYPE_VT;

    case CM_CALL_TYPE_VS:      return CMIPAPP_CALL_TYPE_VS;

    case CM_CALL_TYPE_SMS:     return CMIPAPP_CALL_TYPE_SMS;

    default:
       CM_ERR( "CM call type %d not mapped to CMIPAPP call type !!!", call_type, 0,0 );
       CM_ASSERT(FALSE);
       return CMIPAPP_CALL_TYPE_MAX; /*lint !e641 */   
  }

} /* cmipcall_map_cm_calltype_to_ip_calltype () */


/*===========================================================================

FUNCTION cmipcall_map_sys_mode_to_ip_sys_mode

DESCRIPTION
  Maps system mode to IP system mode

DEPENDENCIES
  None.

RETURN VALUE
  sys mode that corresponds to ip system mode

SIDE EFFECTS
  none

===========================================================================*/
static cmipapp_sys_mode_e_type cmipcall_map_sys_mode_to_ip_sys_mode(

    sys_sys_mode_e_type  sys_mode
        /* call type */
)
{
  CM_ASSERT (BETWEEN(sys_mode, SYS_SYS_MODE_NONE, SYS_SYS_MODE_MAX));

  switch (sys_mode)
  {
    case SYS_SYS_MODE_NO_SRV:  return CMIPAPP_SYS_MODE_NO_SRV;

    case SYS_SYS_MODE_CDMA:    return CMIPAPP_SYS_MODE_CDMA;

    case SYS_SYS_MODE_GSM:     return CMIPAPP_SYS_MODE_GSM;

    case SYS_SYS_MODE_WCDMA:   return CMIPAPP_SYS_MODE_WCDMA;

    case SYS_SYS_MODE_WLAN:    return CMIPAPP_SYS_MODE_WLAN;

    case SYS_SYS_MODE_HDR:     return CMIPAPP_SYS_MODE_HDR;

    default:
       CM_ERR( "sys mode %d not supported in IP call !!!", sys_mode, 0,0 );
       CM_ASSERT(FALSE);
       return CMIPAPP_SYS_MODE_MAX;
  }

} /* cmipcall_map_sys_mode_to_ip_sys_mode () */

/*===========================================================================

FUNCTION cmipcall_sups_cmd_check

DESCRIPTION
  Check the supplementary service command.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no errors found or an error as appropriate

SIDE EFFECTS
  None

==========================================================================*/
static cm_call_cmd_err_e_type cmipcall_sups_cmd_check(

     cm_call_cmd_s_type *call_cmd_ptr
       /* SUPS command to be checked */
)
{

  cmss_s_type                *ss_ptr              = cmss_ptr();
    /* Pointer to the serving system object */

  cmcall_s_type              *call_ptr            = NULL;
    /* Pointer to call object */  

  cm_call_cmd_err_e_type      cmd_err             = CM_CALL_CMD_ERR_NOERR;
    /* Call command error to return */

  cm_call_cmd_info_s_type    *cmd_info_ptr        = CMD_INFO_PTR( call_cmd_ptr );
      /* Pointer to call cmd info */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ss_ptr       != NULL );
  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( cmd_info_ptr != NULL );

 
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* We have to have a call up for all SUPS commands  
  **
  ** - This also checks for Phone being online, since call obj
  **   does not get alloc without phone being online.     
  **
  */
  call_ptr = cmcall_ptr( cmd_info_ptr->call_id );

  if(call_ptr == NULL)
  {
    return CM_CALL_CMD_ERR_CALL_ID_P;
  }  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  /* Phone has to have service on the ss pointed by call
  */
  if( !( ( (call_ptr->ss == CM_SS_MAIN) && 
                        sys_srv_status_is_srv (ss_ptr->main_srv_status))
         ||
         ( (call_ptr->ss == CM_SS_HDR) && 
                        sys_srv_status_is_srv (ss_ptr->hybr_1_srv_status))
         ||
         ( (call_ptr->ss == CM_SS_WLAN) && 
                        sys_srv_status_is_srv (ss_ptr->info.wlan_srv_status))
       )
    )
  {
    CM_MSG_HIGH ("No service on ss %d", call_ptr->ss, 0, 0);

    return (CM_CALL_CMD_ERR_SRV_STATE_S );
  }   

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(cmd_info_ptr->sups_cmd_type)
  {    

    /* Hold call is used either to 
    ** - Hold the current active call
    ** - Hold current active call and accept incoming call.    
    **  call id passed for HOLD_CALL can be in INCOM or CONV state.
    */
    case CM_CALL_SUPS_TYPE_HOLD_CALL:
      break;

    /* Operation of multiparty requires both calls
    ** to be connected. State of either call will
    ** be in conversation.
    */    
    case CM_CALL_SUPS_TYPE_MULTIPARTY_CALL:
      if( call_ptr->call_state != CM_CALL_STATE_CONV )
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      break;
    
    /* ECT can be performed only when a call is 
    ** in INCOM/CONV state.
    */
    case CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER:
      if( !( (call_ptr->call_state == CM_CALL_STATE_CONV) ||
             (call_ptr->call_state == CM_CALL_STATE_INCOM)
           )
        )
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      break;


    /* Allow Act Answer hold only when call is in incoming state.
    */
    case CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD:
      if ( ! ( ( call_ptr->call_state == CM_CALL_STATE_INCOM) &&
               (call_ptr->call_subst.incom == 
                                      CM_CALL_INCOM_SUBST_ALERTING)
             )
         )
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      break;


    /* Allow De act Answer hold only when call is in CONV state.
    */
    case CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD:
      if( call_ptr->call_state != CM_CALL_STATE_CONV )
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      break;

  /* Allow UDUB (0-Send) only when call is in INCOM state */
  case CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB:
      if( call_ptr->call_state != CM_CALL_STATE_INCOM )
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      break;

  /* Allow only flash to go through if IP Call is in conversation */
  case CM_CALL_SUPS_TYPE_SIMPLE_FLASH:
      if (call_ptr->call_state != CM_CALL_STATE_CONV)
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      break; 

  /* Allow active Call Release only when Call is in conversation state */
  case CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL:
      if (call_ptr->call_state != CM_CALL_STATE_CONV)
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      break;

    default:
      cmd_err = CM_CALL_CMD_ERR_CALL_SUPS_P;
      break;
  }

  return cmd_err;

} /* cmipcall_sups_cmd_check () */


/*===========================================================================

FUNCTION cmipcall_process_hold_call

DESCRIPTION
  Sends request to answer a waiting call or to put a conencted call 
  on hold/active sub-state.

  Sequence to be followed
  -----------------------
  1> First put all active calls on hold.
  2> Make held calls active or accept an incoming call

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
static void cmipcall_process_hold_call
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  cmipapp_cb_s_type      *app_cb_ptr = NULL;
    /* Structure pointing to app call back functions
    */

  cm_iterator_type            call_itr;
    /* iterates through call call objects in CM 
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call object has the app_id that delivered the call
  */
  app_cb_ptr = cmipapp_get_cb_ptrs(call_ptr->ipapp_id);


  if (app_cb_ptr == NULL)
  {
    CM_ERR ("App Call back ptr is NULL", 0, 0, 0);
    CM_ASSERT( app_cb_ptr != NULL );

    return;
  }
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);  

  /* Active calls should be put on hold first
  */
  while(call_ptr != NULL)
  {
    
    if ((call_ptr->call_state      == CM_CALL_STATE_CONV) &&
        (call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ACTIVE)
       )
    {           

      CM_MSG_HIGH ("Requesting call %d to be put on hold", call_ptr->call_id,
                0, 0);

      if (app_cb_ptr->cmipapp_cmd_hold_func_ptr != NULL && 
          !app_cb_ptr->cmipapp_cmd_hold_func_ptr 
                        (call_ptr->call_id, call_ptr->ipapp_data_block))
      {
        CM_ERR ("Request to put call on hold failed", 0, 0, 0);
        return;
      }
         
    } /* if ((call_ptr->call_state == CM_CALL_STATE_CONV) &&
        (call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ACTIVE))
      */

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while(call_ptr != NULL) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  /* Make held calls active or accept an incoming call
  */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);  
  

  while(call_ptr != NULL)
  {
    switch (call_ptr->call_state)
    {

      case CM_CALL_STATE_INCOM:

        CM_MSG_HIGH ("Answering call %d", call_ptr->call_id,
                     0, 0);

        /* Accept incoming call with TRUE 
        */
        cmipcall_answer (call_ptr, TRUE, CMIPAPP_INCOM_REJ_CAUSE_NONE);

        break;


      case CM_CALL_STATE_CONV:
        
        /* If Call is currently on hold, invoke active callback
        */
        if (call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ON_HOLD)
        {       
                
          CM_MSG_HIGH ("Requesting call %d to be made active", 
                         call_ptr->call_id, 0, 0);

          if (app_cb_ptr->cmipapp_cmd_active_func_ptr != NULL && 
              !app_cb_ptr->cmipapp_cmd_active_func_ptr 
                            (call_ptr->call_id, call_ptr->ipapp_data_block))
          {
            CM_ERR ("Request to make call active failed", 0, 0, 0);
            return;
          }            
        }
        break;

      case CM_CALL_STATE_NONE:
      case CM_CALL_STATE_MAX:
        CM_ERR ("Invalid call state %d for call id %d", 
                 call_ptr->call_state, 
                 call_ptr->call_id, 
                 0);
        return;

      case CM_CALL_STATE_IDLE:
      case CM_CALL_STATE_ORIG:
      case CM_CALL_STATE_CC_IN_PROGRESS:
      case CM_CALL_STATE_RECALL_RSP_PEND:
      default:
        break;

    } /* switch (call_ptr->call_state) */

    call_ptr = cmcall_obj_get_next(&call_itr);
   
  } /* while(call_ptr != NULL) */

} /* cmipcall_process_hold_call () */


/*===========================================================================

FUNCTION cmipcall_process_act_answer_hold

DESCRIPTION
  Sends a request for activating answer hold.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
static void cmipcall_process_act_answer_hold
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  cmipapp_cb_s_type      *app_cb_ptr = NULL;
    /* Structure pointing to app call back functions
    */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   

  /* Call object has the app_id that delivered the call
  */
  app_cb_ptr = cmipapp_get_cb_ptrs(call_ptr->ipapp_id);

  if (app_cb_ptr == NULL)
  {
    CM_ERR ("App Call back ptr is NULL", 0, 0, 0);
    CM_ASSERT( app_cb_ptr != NULL );

    return;
  }

  CM_MSG_HIGH ("Activ answer-hold on call %d", call_ptr->call_id, 0, 0);

  if (app_cb_ptr->cmipapp_cmd_act_answer_hold_func_ptr != NULL && 
      !app_cb_ptr->cmipapp_cmd_act_answer_hold_func_ptr 
                     (call_ptr->call_id, call_ptr->ipapp_data_block))
  {
    CM_ERR ("Request to put call on answer-hold failed", 0, 0, 0);
    return;
  }
          

} /* cmipcall_process_act_answer_hold () */



/*===========================================================================

FUNCTION cmipcall_process_deact_answer_hold

DESCRIPTION
  Sends a request for de-activating answer hold.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
static void cmipcall_process_deact_answer_hold
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  cmipapp_cb_s_type      *app_cb_ptr = NULL;
    /* Structure pointing to app call back functions
    */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  /* Call object has the app_id that delivered the call
  */
  app_cb_ptr = cmipapp_get_cb_ptrs(call_ptr->ipapp_id);  

  if (app_cb_ptr == NULL)
  {
    CM_ERR ("App Call back ptr is NULL", 0, 0, 0);
    CM_ASSERT( app_cb_ptr != NULL );

    return;
  }

  CM_MSG_HIGH ("Deact answer-hold on call %d", call_ptr->call_id, 0, 0);

  if (app_cb_ptr->cmipapp_cmd_deact_answer_hold_func_ptr != NULL &&
      !app_cb_ptr->cmipapp_cmd_deact_answer_hold_func_ptr 
                     (call_ptr->call_id, call_ptr->ipapp_data_block))
  {
    CM_ERR ("Request to deact answer-hold failed", 0, 0, 0);
    return;
  }
          

} /* cmipcall_process_act_answer_hold () */



/*===========================================================================

FUNCTION cmipcall_process_ect

DESCRIPTION
  Sends a request to perform explicit call transfer on the calls.  

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
static void cmipcall_process_ect 
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  cmipapp_cb_s_type      *app_cb_ptr = NULL;
    /* Structure pointing to app call back functions
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call object has the app_id that delivered the call
  */
  app_cb_ptr = cmipapp_get_cb_ptrs(call_ptr->ipapp_id);


  if (app_cb_ptr == NULL)
  {
    CM_ERR ("ECT Call back ptr of app %d is NULL", call_ptr->ipapp_id, 0, 0);
    CM_ASSERT( app_cb_ptr != NULL );

    return;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH ("ECT callback of app %d being called", 
                                          call_ptr->ipapp_id, 0, 0);

  if (app_cb_ptr->cmipapp_cmd_ect_func_ptr != NULL && 
      !app_cb_ptr->cmipapp_cmd_ect_func_ptr ())
  {
    CM_ERR ("Request to perform ECT failed ", 0, 0, 0);
    return;
  }
          
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   


} /* cmipcall_process_ect () */


/*===========================================================================

FUNCTION cmipcall_process_mpty

DESCRIPTION
  Sends a request to perform multiparty on given calls.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
static void cmipcall_process_mpty
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  cmipapp_cb_s_type      *app_cb_ptr = NULL;
    /* Structure pointing to app call back functions
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call object has the app_id that delivered the call
  */
  app_cb_ptr = cmipapp_get_cb_ptrs(call_ptr->ipapp_id);


  if (app_cb_ptr == NULL)
  {
    CM_ERR ("MPTY Call back of app %d is NULL", call_ptr->ipapp_id, 0, 0);
    CM_ASSERT( app_cb_ptr != NULL );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   

  CM_MSG_HIGH ("MPTY callback of app %d being called", 
                                          call_ptr->ipapp_id, 0, 0);

  if (app_cb_ptr->cmipapp_cmd_mpty_func_ptr != NULL && 
      !app_cb_ptr->cmipapp_cmd_mpty_func_ptr ())
  {
    CM_ERR ("Request to perform MPTY failed ", 0, 0, 0);
    return;
  }
          
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   


} /* cmipcall_process_mpty () */

/*===========================================================================

FUNCTION cmipcall_proc_sups_cmd_type

DESCRIPTION
  Branches sups request to hold, activ answer-hold or deact answer-hold
  functions. 

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
static void cmipcall_proc_sups_cmd_type
(
  const cm_call_cmd_info_s_type    *cmd_info_ptr,
    /* Pointer to cmd
    */

  const cmcall_s_type              *call_ptr
    /* Pointer to a call object
    */
)
{
  switch (cmd_info_ptr->sups_cmd_type)
  {
    case CM_CALL_SUPS_TYPE_HOLD_CALL:                 

      /* Call is put into hold or active depending on its current
      ** active status.
      **
      ** SUPS_TYPE_HOLD_CALL toggles a call between held and active
      ** states.
      */
      cmipcall_process_hold_call (call_ptr);

      break;

    case CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD:

      /* Call is being put on answer hold.
      */
      cmipcall_process_act_answer_hold (call_ptr);

      break;


    case CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD:

      /* Call is being pulled out of answer hold.
      */
      cmipcall_process_deact_answer_hold (call_ptr);

      break;

    case CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER:                 

      /* Explicit call transfer request being processed
      */
      cmipcall_process_ect (call_ptr);

      break;


    case CM_CALL_SUPS_TYPE_MULTIPARTY_CALL:                 

      /* Request being sent to put call in MPTY
      */
      cmipcall_process_mpty (call_ptr);

      break;

    case CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB:

      /* Answer the call and send reject.
      */
      cmipcall_answer (call_ptr, FALSE, CMIPAPP_INCOM_REJ_CAUSE_BUSY);

      break;

    case CM_CALL_SUPS_TYPE_SIMPLE_FLASH:
      /* Answer the Call Waiting Indication 
      */
      cmipcall_answer_call_waiting(call_ptr);
      break;

    case CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL: 
      /* End the call
      */
      cmipcall_send_end (call_ptr);
      break;

    default:
      CM_ERR ("Unknown command type received = %d", 
                  cmd_info_ptr->sups_cmd_type, 0, 0);
      break;
  }   

} /* cmipcall_proc_sups_cmd_type () */

/**--------------------------------------------------------------------------
** Functions - external:
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmipcall_reset

DESCRIPTION
  Reset IP call fields to default values reflecting
  a IP call in idle state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmipcall_reset(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL )
  CM_FUNC_START( "cmipcall_reset()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  call_ptr->ipapp_id          = CMIPAPP_ID_INVALID;

  call_ptr->ipapp_data_block  = 0;

}



/*===========================================================================

FUNCTION cmipcall_rpt_proc

DESCRIPTION
  Process app reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmipcall_rpt_proc(

    const cm_hdr_type   *rpt_ptr
        /* Pointer to a IP app report */
)
{

  const cmipapp_rpt_s_type  *cmipapp_rpt_ptr = (cmipapp_rpt_s_type *) rpt_ptr;
    /* Pointer to a IP app report */

  cmcall_s_type             *call_ptr = NULL;
    /* Point at call object */

  cm_mm_call_info_s_type    *call_info_ptr = NULL;
    /* Structure to send call event */

  ip_cmcall_type            *ip_ptr = NULL;
    /* type to point to ipcall specific info */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmipapp_rpt_ptr != NULL );

  CM_FUNC_START( "cmipcall_rpt_proc, cmd=%d",
               cmipapp_rpt_ptr->hdr.cmd, 0, 0);

  /* Do command specific processing as follows:
  ** 1. Update appropriate call object fields.
  ** 2. Modify the call state if necessary.
  ** 3. Notifying clients of event if necessary.
  */
  switch( cmipapp_rpt_ptr->hdr.cmd )
  {
    case CM_IP_CALL_IND_MT_INVITE:

      CM_MSG_HIGH("RXD: MT_INVITE",0,0,0);
      /*
      ** Call id should be requested by IP app before sending any reports
      ** to CM.
      */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.mt_invite.id );

      if (call_ptr == NULL)
      {
        CM_ASSERT( call_ptr != NULL );
        CM_ERR ( "ERROR: Unknown call ID in MT invite", 0, 0, 0);

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

     /*------------------------------------------------------------------*/


      /* Record app that is placing the call
      */
      call_ptr->ipapp_id     = cmipapp_rpt_ptr->ip_ind.mt_invite.app_id;


      /* Copy system mode for the call
      */
      call_ptr->sys_mode   = cmipcall_map_sysmode_to_cm_sysmode (
                                cmipapp_rpt_ptr->ip_ind.mt_invite.sys_mode);

      /*------------------------------------------------------------------*/

      /* Determine ss for this call
      */
      call_ptr->ss         = cmcall_det_ss_from_sys_mode (call_ptr->sys_mode);

      CM_ASSERT (call_ptr->ss != CM_SS_NONE);

      if (call_ptr->ss == CM_SS_NONE)
      {
        CM_ERR (" ss for incom call id %d returned NONE", call_ptr->call_id,
                                                          0, 0 );
      }

      /*------------------------------------------------------------------*/

      /* Call type is carried by invite indication
      */
      call_ptr->call_type  = cmipcall_map_calltype_to_cm_calltype (
                               cmipapp_rpt_ptr->ip_ind.mt_invite.call_type);

      /* Initialize number buffer in call object
      */
      memset((byte *)&call_ptr->num, 0, sizeof(call_ptr->num));

      if ( cmipapp_rpt_ptr->ip_ind.mt_invite.num.len != 0 )
      {
          memcpy( call_ptr->num.buf,
                  cmipapp_rpt_ptr->ip_ind.mt_invite.num.buf,
                  cmipapp_rpt_ptr->ip_ind.mt_invite.num.len ) ;

          call_ptr->num.len =
                         (byte) cmipapp_rpt_ptr->ip_ind.mt_invite.num.len;
      }

      /*------------------------------------------------------------------*/

      /* Set up ind is not presented to the user and is only used by UI
      ** code to check if an incoming call can be allowed. User is alerted
      ** after CM_IP_CALL_IND_MT_RING_CNF is received. 
      */      
      call_ptr->call_state                     = CM_CALL_STATE_INCOM;

      call_ptr->call_subst.incom               = CM_CALL_INCOM_SUBST_SETUP;

      /* Set call info type to IP
      */
      call_ptr->cmcall_mode_info.info_type     = CM_CALL_MODE_INFO_IP;

      cmcall_set_call_direction (call_ptr, CM_CALL_DIRECTION_MT);     


      /*------------------------------------------------------------------*/      
      
      /* Set up indication to UI followed by setup response from UI was 
      ** designed to mirror GW way of handling incoming calls. With 2 clients
      ** ICM and ITelephone being able to handle setup indications it can
      ** happen that ITel sends setup response with FALSE even before ICM
      ** can respond with TRUE. Final Verdict is to remove sending INVITE
      ** to CM from ip app. Intermediate fix is to make CM send setup 
      ** response.
      */

      /* cmcall_event( call_ptr, CM_CALL_EVENT_SETUP_IND ); */

      /* Call the IP call funtion for set up response
      */
      cmipcall_invite_res (call_ptr, TRUE, CMIPAPP_INCOM_REJ_CAUSE_NONE);

      /* Change incom substate to setup processed.
      */
      call_ptr->call_subst.incom =  CM_CALL_INCOM_SUBST_SETUP_PROCESSED;      

      break;


    case CM_IP_CALL_IND_MT_RING_CNF:

      /*
      ** Call id should be requested by IP app before sending any reports
      ** to CM.
      */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.mt_ring_cnf.id );

      if (call_ptr == NULL)
      {
        CM_ASSERT( call_ptr != NULL );
        CM_ERR ( "ERROR: Unknown call ID in MT_RING_CNF", 0, 0, 0);

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;
     
      /*------------------------------------------------------------------*/

      /* Call state should be incoming and subst should be setup_processed
      ** for alerting to be processed.
      */
      if( (call_ptr->call_state       == CM_CALL_STATE_INCOM) &&
         (call_ptr->call_subst.incom == CM_CALL_INCOM_SUBST_SETUP_PROCESSED)
       )
      {

        call_ptr->call_subst.incom = CM_CALL_INCOM_SUBST_ALERTING;

        call_ptr->sys_mode   = cmipcall_map_sysmode_to_cm_sysmode (
                              cmipapp_rpt_ptr->ip_ind.mt_ring_cnf.sys_mode);

        cmph_call_start( call_ptr );

      }
      else
      {      

        CM_ERR( "Rcvd CM_IP_CALL_IND_MT_RING_CNF out of seq.",0,0,0);
        call_ptr->end_status = CM_CALL_END_INCOM_CALL;
        cmcall_end( call_ptr );
        return;

      }           

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      /*------------------------------------------------------------------*/

      cmcall_event( call_ptr, CM_CALL_EVENT_INCOM );
      break;


    case CM_IP_CALL_IND_MO_PROGR_INFO:

      CM_MSG_HIGH("RXD: MO_PROGR_INFO",0,0,0);
      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr (cmipapp_rpt_ptr->ip_ind.mo_progr_info.id);

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR    ("ERROR: Unknown call ID in progr info", 0, 0, 0);

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      /*------------------------------------------------------------------*/

      /* Copy the number, call is being made from
      */
      if ( cmipapp_rpt_ptr->ip_ind.mo_progr_info.num.buf[0] != 0 )
      {
          memcpy( call_ptr->num.buf,
                  cmipapp_rpt_ptr->ip_ind.mo_progr_info.num.buf,
                  cmipapp_rpt_ptr->ip_ind.mo_progr_info.num.len ) ;

          call_ptr->num.len =
                       (byte) cmipapp_rpt_ptr->ip_ind.mo_progr_info.num.len;
      }

      /* Check for the call state to be in origination
      */
      CM_ASSERT (call_ptr->call_state == CM_CALL_STATE_ORIG);

      if (call_ptr->call_state != CM_CALL_STATE_ORIG)
      {
        CM_ERR ("Progress info for call not in CM_CALL_STATE_ORIG", 0, 0, 0);
      }
      

      /*------------------------------------------------------------------*/

      switch (cmipapp_rpt_ptr->ip_ind.mo_progr_info.progr)
      {
        case CMIPAPP_PROG_FORWARD:
          cmcall_event( call_ptr, CM_CALL_EVENT_CALL_FORWARDED );
          break;

        case CMIPAPP_PROG_REDIRECT:
          cmcall_event( call_ptr, CM_CALL_EVENT_REDIRECTING_NUMBER );
          break;

       case CMIPAPP_PROG_MO_RINGBACK:
          ip_ptr                 = &call_ptr->cmcall_mode_info.info.ip_info;
          ip_ptr->call_prog_info = CM_CALL_PROG_INFO_REMOTE_ALERT;

          cmcall_event( call_ptr, CM_CALL_EVENT_PROGRESS_INFO_IND );
          break;

        default:
          CM_MSG_HIGH ("mo_prog_info default case %d",
                        cmipapp_rpt_ptr->ip_ind.mo_progr_info.progr, 0, 0);
          break;

      } /* switch */
      break;


    case CM_IP_CALL_IND_CONNECTED:

      CM_MSG_HIGH("RXD: CONNECTED",0,0,0);
      /* Get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.connected.id );

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR    ("ERROR: Unknown call ID in connected", 0, 0, 0);

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      /*------------------------------------------------------------------*/

      /* Get the system call is connected through
      */
      call_ptr->sys_mode =  cmipcall_map_sysmode_to_cm_sysmode (
                              cmipapp_rpt_ptr->ip_ind.connected.sys_mode);

      CM_ASSERT( call_ptr != NULL );

      call_ptr->call_state        = CM_CALL_STATE_CONV;

      call_ptr->call_subst.conv   = CM_CALL_CONV_CALL_ACTIVE;

      call_ptr->call_connect_time = clk_uptime();      

      /*------------------------------------------------------------------*/

      cmcall_event( call_ptr, CM_CALL_EVENT_CONNECT );

      #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
      #endif

      break;


    case CM_IP_CALL_IND_CALL_END:

      CM_MSG_HIGH("RXD: CALL_END",0,0,0);
      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr (cmipapp_rpt_ptr->ip_ind.call_end.id);

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR    ("ERROR: Unknown call ID in call_end", 0, 0, 0);

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      /*------------------------------------------------------------------*/

      switch( call_ptr->call_state )
      {

        case CM_CALL_STATE_IDLE:
          /* Client released already */
          if ( call_ptr->call_subst.idle == CM_CALL_IDLE_SUBST_ENDWAIT )
          {
            cmcall_end(call_ptr);
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_STATE_ORIG:

          /* If we are trying to originate, indicate a RELEASE
          ** end status and call the origination function to decide
          ** whether to continue or abort the origination process.
          */
          #ifdef FEATURE_IMS_VCC
          /** If the origination for switch in call failed and 
          ** if switch out domain call conditions improve, then
          ** abort origination process
          */
          if( homer_chk_orig_abort_in_switch_in_domain(call_ptr) )
          {
            call_ptr->end_status = CM_CALL_END_REL_NORMAL;
          }
          else
          #endif /*FEATURE_IMS_VCC */
          {
            call_ptr->end_status = cmipcall_end_cause_to_cmcall_end_status
                                (cmipapp_rpt_ptr->ip_ind.call_end.end_cause);
          }
           
          cmcall_orig_proc(call_ptr);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_STATE_INCOM:
          /* Call was never established -
          ** simply end the call.
          */
          call_ptr->end_status = CM_CALL_END_REL_NORMAL;
          cmcall_end( call_ptr );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_STATE_CONV:

          #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
          #endif
          {
            call_ptr->end_status = CM_CALL_END_REL_NORMAL;
            cmcall_end(call_ptr);

            #ifdef FEATURE_IMS_VCC
            /** Check if VCC handoff is complete */
            homer_chk_umts_wlan_handoff_complete(
                    cmipapp_rpt_ptr->ip_ind.call_end.id, 
                    CM_CALL_EVENT_END);
            #endif

          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        default:
          CM_SWITCH_ERR( "Default switch in CALL_EVENT_END , call_state=%d",
                         call_ptr->call_state, 0, 0 );
          break;

      }      

      /*------------------------------------------------------------------*/

      break;

    case CM_IP_CALL_IND_CALL_HOLD:

      CM_MSG_HIGH("RXD: CALL_HOLD",0,0,0);
      /* Get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.hold.id );

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR    ("ERROR: Unknown call ID in call hold", 0, 0, 0);

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      /*------------------------------------------------------------------*/

      /* Call has to satisfy following conditions to be put on hold
      ** Call type is not checked since multiple call types can use
      ** cmipcall
      ** 
      ** - Call state has to be in conversation.      
      ** - Call substate has to be Active.
      */
      if ((call_ptr->call_state      != CM_CALL_STATE_CONV) ||          
          (call_ptr->call_subst.conv != CM_CALL_CONV_CALL_ACTIVE)
         )
      {
        CM_ERR ("Call %d not in right state for hold", call_ptr->call_id, 0, 
                0);
        break;
      }          

      /*------------------------------------------------------------------*/


      if (cmipapp_rpt_ptr->ip_ind.hold.cause == CM_IPAPP_CAUSE_NORMAL)
      {
        CM_MSG_HIGH ("Call hold succeeded for call %d", call_ptr->call_id, 0, 0);
        call_ptr->call_subst.conv = CM_CALL_CONV_CALL_ON_HOLD; 
      }          
      else
      {
        CM_MSG_HIGH ("Call hold failed for call %d with reason %d", 
                     call_ptr->call_id, cmipapp_rpt_ptr->ip_ind.hold.cause,
                     0);
      }

      /* Copy the cause to call object.       
      */
      call_ptr->end_params.end_params.ip_cause.cause = 
                   cmipapp_rpt_ptr->ip_ind.hold.cause; /*lint !e641 !e732 */

      /*------------------------------------------------------------------*/

      /*
      ** 10/31/05 - According to UI MANAGE_CALLS_CONF way of supplying
      ** active call list in a table is a legacy implementation. UI is
      ** more comfortable handling call hold/retrieve events on a per 
      ** call basis.
      */
      cmcall_event (call_ptr, CM_CALL_EVENT_CALL_ON_HOLD);

      break;


    case CM_IP_CALL_IND_CALL_RETRIEVE:

      CM_MSG_HIGH("RXD: CALL_RETRIEVE",0,0,0);
      /* Get the call pointer from the call ID
      */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.retrieve.id );

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR    ("ERROR: Unknown call ID in call hold", 0, 0, 0);

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      /*------------------------------------------------------------------*/

      /* Call has to satisfy following conditions to be put on hold
      ** Call type is not checked since multiple call types can use
      ** cmipcall.
      ** 
      ** - Call state has to be in conversation.      
      ** - Call substate has to be on hold.
      */
      if ((call_ptr->call_state      != CM_CALL_STATE_CONV) ||          
          (call_ptr->call_subst.conv != CM_CALL_CONV_CALL_ON_HOLD)
         )
      {
        CM_ERR ("Call %d not in right state for retrieve", call_ptr->call_id,
                0, 0);
        break;
      }      

      /*------------------------------------------------------------------*/


      if (cmipapp_rpt_ptr->ip_ind.hold.cause == CM_IPAPP_CAUSE_NORMAL)
      {
        CM_MSG_HIGH ("Call hold succeeded for call %d", call_ptr->call_id, 0, 0);
        call_ptr->call_subst.conv = CM_CALL_CONV_CALL_ACTIVE; 
      }          
      else
      {
        CM_MSG_HIGH ("Call retrieve failed for call %d with reason %d", 
                     call_ptr->call_id, cmipapp_rpt_ptr->ip_ind.retrieve.cause,
                     0);
      }

      /* Copy the cause to call object.       
      */
      call_ptr->end_params.end_params.ip_cause.cause = 
                   cmipapp_rpt_ptr->ip_ind.hold.cause; /*lint !e641 !e732 */

      /*------------------------------------------------------------------*/

      /* 
      ** 10/31/05 - According to UI MANAGE_CALLS_CONF way of supplying
      ** active call list in a table is a legacy implementation. UI is
      ** more comfortable handling call hold/retrieve events on a per 
      ** call basis.
      */
      cmcall_event (call_ptr, CM_CALL_EVENT_CALL_RETRIEVED);

      break;            

    case CM_IP_CALL_IND_VS_STATUS:

      /* Get the call pointer from the call ID
      */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.vs_status.id );

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR    ("ERROR: Unknown call ID in call hold", 0, 0, 0);

        return;
      }      

      /* Voice call needs to be in conversation for videoshare to be possible
      */
      if( !((call_ptr->call_state == CM_CALL_STATE_CONV) &&                
            (call_ptr->call_type == CM_CALL_TYPE_VOICE)
           )
        )
      {
        CM_ERR  ("Call %d in incorrect state for videoshare", 
                  call_ptr->call_id, 0, 0);
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      /*------------------------------------------------------------------*/

      /* Event sent to clients informing videoshare availability
      */
      if (cmipapp_rpt_ptr->ip_ind.vs_status.is_vs_poss == TRUE)
      {     

        CM_MSG_HIGH ("Vidoeshare call possible on call %d", 
                      call_ptr->call_id, 0, 0);
        cmcall_event (call_ptr, CM_CALL_EVENT_VS_AVAIL);
      }
      else
      {
        CM_MSG_HIGH ("Vidoeshare call NOT possible on call %d", 
                      call_ptr->call_id, 0, 0);
        cmcall_event (call_ptr, CM_CALL_EVENT_VS_NOT_AVAIL);
      }

      break;

    case CM_IP_CALL_IND_SUPS_CONF:
      
      
      /* Call dependant supp service works on more
      ** than one call. Conf is not sent with respect 
      ** to a particular call id.
      */      

      /*------------------------------------------------------------------*/

      /* Allocate a call info buffer */
      call_info_ptr = cmcall_info_alloc();

      /* Success of supp service req */
      call_info_ptr->mode_info.info.ip_call.call_ss_success =
                    cmipapp_rpt_ptr->ip_ind.sups_conf.call_success;      
         
      /* Sups type that is generating ss_success */
      call_info_ptr->mode_info.info.ip_call.sups_type =
                    cmipapp_rpt_ptr->ip_ind.sups_conf.sups_type;      

      /* List of active calls is not sent as in GW because IP call
      ** sends individual hold/retrieve for each call.
      */      

      /* Set info_type to IP
      */
      call_info_ptr->mode_info.info_type = CM_CALL_MODE_INFO_IP;

      /* Notify clients list of call event. */
      cmclient_list_call_event_ntfy ( CM_CALL_EVENT_MNG_CALLS_CONF, 
                                      call_info_ptr );
      break;


    case CM_IP_CALL_IND_TRANSFER:
      
      /* Get the call pointer from the call ID
      */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.call_transfer.id );

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR    ("ERROR: Unknown call ID in call hold", 0, 0, 0);

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      /*------------------------------------------------------------------*/

      cmcall_event (call_ptr, CM_CALL_EVENT_TRANSFERRED_CALL);
      
      break;


    case CM_IP_CALL_IND_CONNECTED_NUM:
      
      /* Get the call pointer from the call ID
      */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.connected_num.id );

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR    ("ERROR: Unknown call ID in call hold", 0, 0, 0);

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      /*------------------------------------------------------------------*/

      /* Copy the new connected number from ip app */
      call_ptr->num  = cmipapp_rpt_ptr->ip_ind.connected_num.num;
      
      /*------------------------------------------------------------------*/

      cmcall_event (call_ptr, CM_CALL_EVENT_CONNECTED_NUM);

      break;

    case CM_IP_CALL_IND_CALL_WAIT:
      #ifdef FEATURE_IP_CALL
      /* Get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.call_waiting.call_id );

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR    ("ERROR: Unknown call ID in call hold", 0, 0, 0);
        return;
      }

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      /* Copy the new waiting num from ip */
      call_ptr->waiting_num = cmipapp_rpt_ptr->ip_ind.call_waiting.num;

      /* Notify clients list of call event CM_CALL_EVENT_CALL_IS_WAITING. */
      cmcall_event ( call_ptr, CM_CALL_EVENT_CALL_IS_WAITING );
      break;
      #else
      CM_MSG_HIGH("FEATURE_IP_CALL is not defined",0,0,0);
      /* fall through */
      #endif
      

    default:
      CM_ERR ("Unkwn report from IP app %d", cmipapp_rpt_ptr->hdr.cmd, 0, 0);
      break;

  } /* switch (cmipapp_rpt_ptr->hdr.cmd) */
  

} /* cmipcall_rpt_proc () */


/*===========================================================================

FUNCTION  cmipcall_client_cmd_check

DESCRIPTION
  Check for call command parameter errors and whether a specified call command
  is allowed in the current state of the call/phone.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if command is allowed in the current state
  of the call/phone, specific cm_call_cmd_err_e_type otherwise.

SIDE EFFECTS
  None

===========================================================================*/
cm_call_cmd_err_e_type  cmipcall_client_cmd_check(

    cm_cmd_s_type          *cmd_ptr
        /* Pointer to a CM command */
)
{
  cm_call_cmd_s_type          *call_cmd_ptr  = NULL;
      /* Point at call command component */

  cmcall_s_type               *call_ptr      = NULL;
    /* Pointer to the call object */

  cm_call_cmd_err_e_type       cmd_err       = CM_CALL_CMD_ERR_NOERR;
      /* Initialize command error to NO-ERROR */

  int                          i;
    /* For loop counter */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_CALL );

  CM_MSG_HIGH( "cmd check %d",CMD_TYPE(cmd_ptr), 0, 0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_cmd_ptr  = CALL_CMD_PTR( cmd_ptr );


  /* check cmd is already in error or not
  */
  if ( call_cmd_ptr->info.call_type != CM_CALL_TYPE_PS_DATA )
  {
    if (call_cmd_ptr->info.cmd_err != CM_CALL_CMD_ERR_NOERR)
    {
      return call_cmd_ptr->info.cmd_err;
    }

  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  /* Check whether command is possible.
  */
  switch( call_cmd_ptr->cmd )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /*---------------------------*/
                       /* Response to a setup message */
                       /*---------------------------*/

    case CM_CALL_CMD_SETUP_RES:


      call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id );

      if(call_ptr == NULL)
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
      }
      else if ( call_ptr->call_state != CM_CALL_STATE_INCOM )
      {
        /* Makes sense only in incoming state
        */
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      /*lint -restore */
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_ANSWER:

      /* check command parameter errors */
      if((cmd_err = cmcall_call_cmd_answer_para_check(call_cmd_ptr)) != CM_CALL_CMD_ERR_NOERR)
      {
          /* stop further processing if any errors found in the command */
          break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id );
      if(call_ptr==NULL)
      {
          CM_MSG_HIGH( "call_ptr_NULL",0,0,0);
          break; 
      }

      if (call_ptr->call_state != CM_CALL_STATE_INCOM)
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }

      /*lint -restore */
      break; /* CM_CALL_CMD_ANSWER */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_END:

      /* CALL_CMD_END can carry calls in multiple technologies. Earlier it
      ** was either xcall_end, wcall_end or ipcall_end being called. Currently
      ** all three get called one after the other and overall call state can
      ** go to IDLE in any of these calls. So not returning error any more.            
      */
      if( (cmcall_overall_call_state() == CM_OVERALL_CALL_STATE_IDLE ) )
      {                    
        CM_MSG_MED ("overall call state in idle", 0, 0, 0);         
      }


      /* check command parameter errors */
      /* none */
      for ( i = 0; i < call_cmd_ptr->info.num_end_call_ids; i++ )
      {

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* cmipcall should end calls of info_type IP only
        */
        if (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
               != CM_CALL_MODE_INFO_IP)         
        {
          continue;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        call_ptr = cmcall_ptr ( call_cmd_ptr->info.end_params[i].call_id );

        if (call_ptr == NULL )
        {
          cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
          break;
        }

        if ( call_ptr->call_state == CM_CALL_STATE_IDLE )
        {
          /* In idle this command makes no sense.
          */
          cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
          break;
        }

        /* Need not check for call info type, since CM internally
        ** determines from call object
        */
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 


    case CM_CALL_CMD_SUPS:
      
      /* Check cmd parameters for call related sups
      */
      cmd_err = cmipcall_sups_cmd_check (call_cmd_ptr);

    break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      cmd_err = CM_CALL_CMD_ERR_OTHER;
      CM_SWITCH_ERR( "Bad call command %d", call_cmd_ptr->cmd, 0, 0);
      break;

  }

  return cmd_err;
} /* cmipcall_client_cmd_check */



/*===========================================================================

FUNCTION cmipcall_client_cmd_forward_ll

DESCRIPTION
  Forward a client call commands to Lower Layer.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if CM needs to wait for a reply from IP App before continuing
  processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmipcall_client_cmd_forward_ll(

    const cm_cmd_s_type    *cmd_ptr
        /* Pointer to a CM command */
)
{
  const cm_call_cmd_s_type         *call_cmd_ptr     = NULL;
      /* Point at call command component */

  const cm_call_cmd_info_s_type    *cmd_info_ptr     = NULL;
      /* Pointer to call cmd info */

  cmipapp_incom_rej_cause_e_type    incom_rej        = CMIPAPP_INCOM_REJ_CAUSE_NONE;
      /* Reject reason for call set up and call answer */

  const cm_call_cmd_info_ip_s_type  *ip_cmd_info_ptr = NULL;


  cmcall_s_type                     *call_ptr        = NULL;
      /* Pointer to a call object */

  int                                i;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_CALL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_cmd_ptr    = CALL_CMD_PTR( cmd_ptr );
  ip_cmd_info_ptr = IPCALL_CMD_INFO_PTR(call_cmd_ptr);
  cmd_info_ptr    = CMD_INFO_PTR(call_cmd_ptr);


  switch( call_cmd_ptr->cmd )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*---------------------------*/
                     /* Response to a setup message*/
                     /*---------------------------*/

    case CM_CALL_CMD_SETUP_RES:

      call_ptr = cmcall_ptr (call_cmd_ptr->info.call_id);

      if (ip_cmd_info_ptr->accept)
      {
        incom_rej = CMIPAPP_INCOM_REJ_CAUSE_NONE;
      }
      else
      {
        incom_rej = CMIPAPP_INCOM_REJ_CAUSE_BUSY;
      }

      if(call_ptr == NULL)
      {
        CM_ERR( "Invalid callid: %d", call_cmd_ptr->info.call_id,0,0);
      }
      else
      {
        /* Call the IP call funtion for set up response
        */
        cmipcall_invite_res (call_ptr, ip_cmd_info_ptr->accept, incom_rej);
      }

      /* CM_CALL_EVENT_INCOM gets sent at the receipt of a seperate 
      ** indication.
      */      

      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*-------------------------*/
                     /* Answer an incoming call */
                     /*-------------------------*/

    case CM_CALL_CMD_ANSWER:

      call_ptr = cmcall_ptr (call_cmd_ptr->info.call_id);

      if(call_ptr == NULL)
      {
        CM_MSG_HIGH( "call_ptr_NULL",0,0,0);
        break;
      }

      if (ip_cmd_info_ptr->accept) {

        incom_rej = CMIPAPP_INCOM_REJ_CAUSE_NONE;

      }
      else
      {
        incom_rej = CMIPAPP_INCOM_REJ_CAUSE_BUSY;
      }

      cmipcall_answer (call_ptr, ip_cmd_info_ptr->accept, incom_rej);

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /*---------------------------*/
                     /* Ending a call              */
                     /*--------------------------- */

    case CM_CALL_CMD_END:
      for ( i = 0; i < call_cmd_ptr->info.num_end_call_ids; i++ )
      {

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* cmipcall should end calls of info_type IP only
        */
        if (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
             != CM_CALL_MODE_INFO_IP)        
        {
          continue;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        call_ptr = cmcall_ptr( call_cmd_ptr->info.end_params[i].call_id);

        if (call_ptr == NULL)
        {
          CM_ERR("Invalid call_ptr from call_id %d",
          call_cmd_ptr->info.end_params[i].call_id, 0, 0);

          continue;
        }

        /* End the call
        */
        cmipcall_send_end (call_ptr);
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                      /*---------------------------*/
                      /*   Call related SUPS CMD   */
                      /*---------------------------*/

    case CM_CALL_CMD_SUPS:

      call_ptr = cmcall_ptr (cmd_info_ptr->call_id);          

      /* ASSERT ON call_ptr not required because cmipcall_sups_cmd_check()
      ** already does the checking !!!!!
      */

      cmipcall_proc_sups_cmd_type (cmd_info_ptr, call_ptr);            

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      CM_SWITCH_ERR( "Bad call command %d", call_cmd_ptr->cmd, 0, 0);

      break;

  }

  return FALSE;
} /* cmipcall_client_cmd_forward_ll () */



/*===========================================================================

FUNCTION cmipcall_client_cmd_ll_reply

DESCRIPTION
  Forward a client call commands to Lower Layer.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmipcall_client_cmd_ll_reply(

    const cm_cmd_s_type    *cmd_ptr
        /* Pointer to a CM command */
)
{
  const cm_call_cmd_s_type             *call_cmd_ptr  = NULL;
      /* Point at call command component */

  cmcall_s_type                        *call_ptr      = NULL;
      /* Pointer to a call object */

  const cm_call_cmd_info_s_type    *cmd_info_ptr     = NULL;
      /* Pointer to call cmd info */

  int                                   i             = 0;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_CALL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_cmd_ptr  = CALL_CMD_PTR( cmd_ptr );
  cmd_info_ptr  = CMD_INFO_PTR(call_cmd_ptr);

  if (call_cmd_ptr->cmd != CM_CALL_CMD_END)
  {     
    if ((call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id)) == NULL)
    {
       CM_ERR_FATAL("Invalid call_ptr from call id %d", call_cmd_ptr->info.call_id, 0, 0);
       /*
       ** 527: Unreachable
       ** This is because for some targets _ERR_FATAL may evaluate with sth
       ** with exit(1) in it but on others error fatals may be turned off
       */
       /*lint -save -e527 */
       return;
       /*lint -restore */
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( call_cmd_ptr->cmd )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*---------------------------*/
                     /* Response to a setup message*/
                     /*---------------------------*/

    case CM_CALL_CMD_SETUP_RES:
      /* No fields to copy into call object as of now
      */

      /* if setup was rejected, send end to clients else no event is sent
      */
      if (call_cmd_ptr->info.end_status == CM_CALL_END_SETUP_REJ){

        /* Send call end request for the call
        */
        cmipcall_send_end (call_ptr);
      }

      /* Change incom substate to setup processed.
      */
      call_ptr->call_subst.incom =  CM_CALL_INCOM_SUBST_SETUP_PROCESSED;      

      cmcall_event (call_ptr, CM_CALL_EVENT_SETUP_RES);
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*-------------------------*/
                     /* Answer an incoming call */
                     /*-------------------------*/

    case CM_CALL_CMD_ANSWER:

      /* Change incom substate to answer processed      
      */
      call_ptr->call_subst.incom =  CM_CALL_INCOM_SUBST_ANSWERED;


      /* No fields to copy into call object as of now
      */

      if (call_cmd_ptr->info.end_status == CM_CALL_END_INCOM_REJ){

        /* Send call end request for the call
        */
        cmipcall_send_end (call_ptr);

      }
      else
      {
        cmcall_event( call_ptr, CM_CALL_EVENT_ANSWER );
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*-------------------------*/
                     /* End the call            */
                     /*-------------------------*/

    case CM_CALL_CMD_END:

      for ( i=0; i < call_cmd_ptr->info.num_end_call_ids; i++ )
      {
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* cmipcall should end calls of info_type IP only
        */
        if (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
             != CM_CALL_MODE_INFO_IP)         
        {
          continue;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        
        call_ptr = cmcall_ptr(call_cmd_ptr->info.end_params[i].call_id);

        if (call_ptr == NULL)
        {
          CM_MSG_HIGH( "call_ptr_NULL",0,0,0);
        }
        else
        {

          call_ptr->end_status = CM_CALL_END_CLIENT_END;

          memcpy( &call_ptr->end_params,
                  &call_cmd_ptr->info.end_params[i],
                  sizeof(cm_end_params_s_type));


          cmcall_event( call_ptr, CM_CALL_EVENT_END );
          cmcall_end (call_ptr);
        }

      }      

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/         

    case CM_CALL_CMD_SUPS:

      call_ptr = cmcall_ptr (call_cmd_ptr->info.call_id); 
      if(call_ptr==NULL)
      {
        CM_MSG_HIGH( "call_ptr_NULL",0,0,0);
        break;
      }

      switch (cmd_info_ptr->sups_cmd_type)
      {         
           
        case CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD:
          
          /* Change incom substate to answered
          */
          call_ptr->call_subst.incom =  CM_CALL_INCOM_SUBST_ANSWERED;

          cmcall_event( call_ptr, CM_CALL_EVENT_ANSWER );

          break;


        default:
          CM_MSG_MED("Unknown command type received = %d", 
                     cmd_info_ptr->sups_cmd_type, 0, 0);
          break;
      }

      /* Update call fields by copying relevant
      ** command fields into call struct.
      */
      cmcall_cmd_copy_fields( call_ptr, call_cmd_ptr );

      /* Send CM_CALL_EVENT_SUPS to clients
      ** IP call specific fields have not been added to 
      ** call object. sups_type and sups_cmd_params
      ** for ip call are not sent to clients in 
      ** CALL_EVENT_SUPS.
      */
      cmcall_event( call_ptr, CM_CALL_EVENT_SUPS );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/         

    default:

      CM_SWITCH_ERR( "Bad call command %d", call_cmd_ptr->cmd, 0, 0);

      break;


  } /* switch () */
} /* cmipcall_client_cmd_ll_reply () */


/*===========================================================================

FUNCTION cmipcall_client_cmd_proc

DESCRIPTION
 Handles call commands for CM IP call interface

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmipcall_client_cmd_proc
(
  cm_cmd_s_type  *cmd_ptr
    /* pointer to a CM command */

)
{
  cm_call_cmd_err_e_type    call_cmd_err;

  boolean                   wait_for_reply;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmipcall_client_cmd_proc()",0,0,0 );

  CM_ASSERT( cmd_ptr != NULL );

  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_CALL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Start by checking whether this is a valid command
  ** relative to the current state of the call/phone.
  */
  call_cmd_err = cmipcall_client_cmd_check( cmd_ptr );

  /*
  ** Notify clients of command processing status.
  ** If error found, return.
  */
  cmcall_client_cmd_err( cmd_ptr, call_cmd_err );

  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If we got here, no errors were found,
  ** so forward the client command to Lower Layer.
  */
  wait_for_reply = cmipcall_client_cmd_forward_ll( cmd_ptr );


  /* If NO need to wait for reply from IP App,
  ** call the reply function right a way.
  */
  if( ! wait_for_reply )
  {
    /* Comes here only when cmd_err is NO_ERR, so need not send
    ** cmd_err as an argument
    */
    cmipcall_client_cmd_ll_reply(cmd_ptr);

  }

} /* cmipcall_client_cmd_proc () */

/*===========================================================================

FUNCTION cmipcall_send_orig

DESCRIPTION
  Directs a call origination to a VoIP app after matching an app with the
  right system mode and capability,

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_send_orig
(
  cmcall_s_type           *call_ptr
    /* Call object for the originating call
    */
)
{
  cmipapp_id_type             app_id = CMIPAPP_ID_INVALID;
    /* IP app id
    */

  cmipapp_cb_s_type           *app_cb_ptr;
    /* Structure pointing to app call back functions
    */
  
  sys_sys_mode_e_type         sys_mode = SYS_SYS_MODE_NONE;
    /* Mode on which origination is sent */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copying mode on which origination will get sent */
  sys_mode = call_ptr->sr.orig_sent_on_mode;

  /* Need to get an active app id with matching sys_mode, call_type
  */
  app_id = cmipapp_find_active_app (sys_mode, call_ptr->call_type);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (app_id == CMIPAPP_ID_INVALID)
  {
    CM_ASSERT (app_id != CMIPAPP_ID_INVALID);
    CM_ERR ("No App found", 0, 0, 0);
    return;
  }

  app_cb_ptr = cmipapp_get_cb_ptrs(app_id);


  if (app_cb_ptr == NULL)
  {
    CM_ERR ("App Call back ptr is NULL", 0, 0, 0);
    CM_ASSERT( app_cb_ptr != NULL );

    return;
  } 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Give the origination to an active app
  */
  if ( app_cb_ptr->cmipapp_cmd_orig_func_ptr != NULL && 
    !app_cb_ptr->cmipapp_cmd_orig_func_ptr(call_ptr->num,
                                            call_ptr->call_id,
    cmipcall_map_cm_calltype_to_ip_calltype (call_ptr->call_type),
    cmipcall_map_sys_mode_to_ip_sys_mode    (sys_mode),
                                            call_ptr->ipapp_data_block))
   {
    CM_ERR ("Sending origination to app failed", 0, 0, 0);
    return;
   }

  /* Store the app id that has been selected for call origination
  */
  call_ptr->ipapp_id   =  app_id;


  CM_MSG_HIGH ("Sending IP orig, sys_mode %d, app_id %d",
                                                       sys_mode, app_id, 0);

} /* cmipcall_send_orig () */


/*===========================================================================

FUNCTION cmipcall_invite_res

DESCRIPTION
  Sends client response to invite received from network.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_invite_res
(
  const cmcall_s_type            *call_ptr,
    /* call object
    */

  boolean                         accept,
    /* TRUE or FALSE depending on if call is accepted or rejected
    */

  cmipapp_incom_rej_cause_e_type  cause
    /* causes for rejecting an incoming call. Can be set to
    ** CMIPAPP_INCOM_REJ_CAUSE_NONE for accept = TRUE
    */
)
{
  cmipapp_cb_s_type      *app_cb_ptr = NULL;
    /* Structure pointing to app call back functions
    */
  

  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call object has the app_id that delivered the call
  */
  app_cb_ptr = cmipapp_get_cb_ptrs(call_ptr->ipapp_id);


  if (app_cb_ptr == NULL)
  {
    CM_ERR ("App Call back ptr is NULL", 0, 0, 0);
    CM_ASSERT( app_cb_ptr != NULL );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the invite response using approp app's invite_res callback
  */
  if (app_cb_ptr->cmipapp_cmd_invite_res_func_ptr != NULL &&
      !app_cb_ptr->cmipapp_cmd_invite_res_func_ptr(call_ptr->call_id, accept,
                                                  cause,
                                                 call_ptr->ipapp_data_block))
  {
    CM_ERR ("Sending invite response to app failed", 0, 0, 0);
    return;
  }

} /* cmipcall_invite_res () */


/*===========================================================================

FUNCTION cmipcall_answer

DESCRIPTION
  Sends user response to an incoming call

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_answer
(
  const cmcall_s_type                   *call_ptr,
    /* call object
    */

  boolean                         accept,
    /* TRUE or FALSE depending on if call is accepted or rejected
    */

  cmipapp_incom_rej_cause_e_type  cause
    /* causes for rejecting an incoming call. Can be set to
    ** CMIPAPP_INCOM_REJ_CAUSE_NONE for accept = TRUE
    */
)
{
  cmipapp_cb_s_type              *app_cb_ptr = NULL;
    /* Structure pointing to app call back functions
    */  

  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call object has the app_id that delivered the call
  */

  app_cb_ptr = cmipapp_get_cb_ptrs(call_ptr->ipapp_id);


  if (app_cb_ptr == NULL)
  {
    CM_ERR ("App Call back ptr is NULL", 0, 0, 0);
    CM_ASSERT( app_cb_ptr != NULL );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the user response about answering the call
  */
  if (app_cb_ptr->cmipapp_cmd_answer_func_ptr != NULL &&
      !app_cb_ptr->cmipapp_cmd_answer_func_ptr (call_ptr->call_id, accept,
                                                cause, 
                                                call_ptr->ipapp_data_block))
  {
    CM_ERR ("Sending call answer to app failed", 0, 0, 0);
    return;
  }


} /* cmipcall_answer () */


/*===========================================================================

FUNCTION cmipcall_answer_call_waiting

DESCRIPTION
  Sends user response to an call waiting indication

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_answer_call_waiting
(
  const cmcall_s_type                   *call_ptr
    /* call object
    */
)
{
  cmipapp_cb_s_type              *app_cb_ptr = NULL;
    /* Structure pointing to app call back functions
    */  

  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call object has the app_id that delivered the call
  */

  app_cb_ptr = cmipapp_get_cb_ptrs(call_ptr->ipapp_id);


  if (app_cb_ptr == NULL)
  {
    CM_ERR ("App Call back ptr is NULL", 0, 0, 0);
    CM_ASSERT( app_cb_ptr != NULL );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the user response about answering the call waiting
  */
  if (!app_cb_ptr->cmipapp_cmd_answer_call_waiting_func_ptr (
                                                call_ptr->call_id, 
                                                call_ptr->ipapp_data_block))
  {
    CM_ERR ("Sending call wait answer to app failed", 0, 0, 0);
    return;
  }


} /* cmipcall_answer_call_waiting () */


/*===========================================================================

FUNCTION cmipcall_send_end

DESCRIPTION
  Ends a call

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_send_end
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  cmipapp_cb_s_type      *app_cb_ptr = NULL;
    /* Structure pointing to app call back functions
    */  

  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call object has the app_id that delivered the call
  */
  app_cb_ptr = cmipapp_get_cb_ptrs(call_ptr->ipapp_id);


  if (app_cb_ptr == NULL)
  {
    CM_ERR ("App Call back ptr is NULL", 0, 0, 0);
    CM_ASSERT( app_cb_ptr != NULL );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Request to end the call
  */
  if (app_cb_ptr->cmipapp_cmd_end_func_ptr != NULL &&
      !app_cb_ptr->cmipapp_cmd_end_func_ptr (call_ptr->call_id, 
                                             call_ptr->ipapp_data_block))
  {
    CM_ERR ("Call end request failed", 0, 0, 0);
    return;
  }


} /* cmipcall_send_end () */


/*===========================================================================

FUNCTION cmipcall_end

DESCRIPTION
  Deallocates call object and informs phone of a call end

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Call object gets deallocated.

===========================================================================*/
void cmipcall_end
(
  cmcall_s_type           *call_ptr
    /* call object
    */
)
{

  CM_ASSERT (call_ptr != NULL);

  /* Change the call state to idle.
  */
  call_ptr->call_state       = CM_CALL_STATE_IDLE;
  call_ptr->call_subst.idle  = CM_CALL_IDLE_SUBST_NONE;

  /* Tell phone that call is over.
     Number of allocated calls equal to 1 means that this is the
     last call that is being ended, so we need to notify the phone now.
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the queues for this activity
  ** serach the Queues and if the activity is found, update its uptime a
  ** and the update reason
  */


  (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id, 
                                 CM_SS_MAIN, 
                                 CM_ACT_UPDATE_REAS_ACT_END );

  (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id, 
                                 CM_SS_HDR, 
                                 CM_ACT_UPDATE_REAS_ACT_END );

  (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id, 
                                 CM_SS_WLAN, 
                                 CM_ACT_UPDATE_REAS_ACT_END );



  if(cm_number_of_allocated_calls() == 1) {
    cmph_call_end( call_ptr );
  }

  /* Notify clients of call end event.
  */
  cmcall_update_call_end_info( call_ptr );
  cmcall_event( call_ptr, CM_CALL_EVENT_END );

  #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
  #endif
  
  cm_call_id_deallocate( cmcall_call_id_get( call_ptr ) );

  /* update DDTM status */
  cmph_update_ddtm_status(cmph_ptr());

} /* cmipcall_end */

/*===========================================================================

FUNCTION cmipcall_hold

DESCRIPTION
  Request to put call on hold

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_hold
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  cmipapp_cb_s_type      *app_cb_ptr = NULL;
    /* Structure pointing to app call back functions
    */  

  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call object has the app_id that delivered the call
  */

  app_cb_ptr = cmipapp_get_cb_ptrs(call_ptr->ipapp_id);


  if (app_cb_ptr == NULL)
  {
    CM_ERR ("App Call back ptr is NULL", 0, 0, 0);
    CM_ASSERT( app_cb_ptr != NULL );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Request to put the call on hold
  */
  if (app_cb_ptr->cmipapp_cmd_hold_func_ptr != NULL &&
      !app_cb_ptr->cmipapp_cmd_hold_func_ptr (call_ptr->call_id, 
                                             call_ptr->ipapp_data_block))
  {
    CM_ERR ("Call hold request failed", 0, 0, 0);
    return;
  }

} /* cmipcall_hold () */


/*===========================================================================

FUNCTION cmipcall_active

DESCRIPTION
  Request to put the call active

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_active
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  cmipapp_cb_s_type      *app_cb_ptr = NULL;
    /* Structure pointing to app call back functions
    */  

  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call object has the app_id that delivered the call
  */

  app_cb_ptr = cmipapp_get_cb_ptrs(call_ptr->ipapp_id);


  if (app_cb_ptr == NULL)
  {
    CM_ERR ("App Call back ptr is NULL", 0, 0, 0);
    CM_ASSERT( app_cb_ptr != NULL );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Request to make the call active
  */
  if (app_cb_ptr->cmipapp_cmd_active_func_ptr != NULL &&
      !app_cb_ptr->cmipapp_cmd_active_func_ptr(call_ptr->call_id, 
                                              call_ptr->ipapp_data_block))
  {
    CM_ERR ("Call active request failed", 0, 0, 0);
  }

  return;

} /* cmipcall_active () */

/*===========================================================================

FUNCTION cmipcall_start_cont_dtmf

DESCRIPTION
  Sends the dtmf for the requested digit

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_start_cont_dtmf
(
  const cmcall_s_type           *call_ptr,
    /* call object
    */

  uint8                    digit
    /* DTMF digit
    */
)
{
  cmipapp_cb_s_type      *app_cb_ptr = NULL;
    /* Structure pointing to app call back functions
    */

  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call object has the app_id that delivered the call
  */

  app_cb_ptr = cmipapp_get_cb_ptrs(call_ptr->ipapp_id);

  if (app_cb_ptr == NULL)
  {
    CM_ERR ("App Call back ptr is NULL", 0, 0, 0);
    CM_ASSERT( app_cb_ptr != NULL );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Request to make the call active
  */
  if (app_cb_ptr->cmipapp_inband_cmd_start_cont_dtmf_func_ptr != NULL &&
      !app_cb_ptr->cmipapp_inband_cmd_start_cont_dtmf_func_ptr
                  (call_ptr->call_id, digit, call_ptr->ipapp_data_block))
  {
    CM_ERR ("start dtmf request failed", 0, 0, 0);
  }


  return;

} /* cmipcall_start_cont_dtmf () */

/*===========================================================================

FUNCTION cmipcall_stop_cont_dtmf

DESCRIPTION
  Sends request to stop dtmf operation for the digit

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_stop_cont_dtmf
(
  const cmcall_s_type           *call_ptr,
    /* call object
    */

  uint16                         duration
    /* DTMF digit duration
    */
)
{
  cmipapp_cb_s_type      *app_cb_ptr = NULL;
    /* Structure pointing to app call back functions
    */
  

  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call object has the app_id that delivered the call
  */

  app_cb_ptr = cmipapp_get_cb_ptrs(call_ptr->ipapp_id);


  if (app_cb_ptr == NULL)
  {
    CM_ERR ("App Call back ptr is NULL", 0, 0, 0);
    CM_ASSERT( app_cb_ptr != NULL );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Request to stop dtmf.
  */
  if (app_cb_ptr->cmipapp_inband_cmd_stop_cont_dtmf_func_ptr != NULL &&
      !app_cb_ptr->cmipapp_inband_cmd_stop_cont_dtmf_func_ptr
                  (call_ptr->call_id, duration, call_ptr->ipapp_data_block))
  {
    CM_ERR ("stop dtmf request failed", 0, 0, 0);
  }

  return;

} /* cmipcall_stop_cont_dtmf () */

#else /* FEATURE_IP_CALL */

/*===========================================================================
FUNCTION cmipcall_dummy

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
void cmipcall_dummy( void )
/*lint -esym(714,cmipcall_dummy) */
/*lint -esym(765,cmipcall_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}


#endif /* FEATURE_IP_CALL */

