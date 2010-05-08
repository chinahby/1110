
/*===========================================================================

              C A L L   M A N A G E R   I P   A P P L I C A T I O N

                                M O D U L E

DESCRIPTION

   Contains implementation of functions that interface an application (VoIP)
   to Call Manager. Call Manager can interact with the application to place
   calls on whatever technology application may support. This way current
   Call Manager interfaces to its clients (UI) need not change and at the
   same time clients (UI) get to make use of new technology.

  Copyright (c) 2005 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmipapp.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/08   cl      Remove check for call waiting callback func as it's not 
                   mandatory for IP Call Apps to be activate.
10/30/08   jd      Adding support for IMS registration
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
03/29/08   rm      Adding UMTS interface support for cmipapp
06/22/07   sk      Added support for ATOM.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/26/07   ka      Changes to support Voip deregistration.
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
                   VoIP.
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
02/17/06   ka      Adding support for videoshare
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
01/05/06   ka      Adding debug messages
12/11/05   ic      Lint cleanup
10/13/05   ic      Replaced INTLOCK / INTFREE with rex_enter_crit_sect() /
                   rex_leave_crit_sect()  
05/01/05   ka      Correcting logic to compute hdr srv status before
                   reporting it to clients.
04/21/05   ka      Changed function name to cmipapp_cap_reg
04/20/05   ka      Informing apps of service status change in CM
04/06/05   ka      Header Description change
03/14/05   ka      Cleaning find active app, app id allocate and removing
                   feature flags
02/09/05   ic      Lint cleanup
02/02/05   dk/ka   Initial release
02/04/05   ka      Lint fixes
===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
/*lint -esym(766,customer.h) */
#include "customer.h"  /* Customer configuration file */

#if defined(FEATURE_IP_CALL)

#include "cmipapp.h"   /* Interface header file for applications */
#include "comdef.h"    /* For ARR_SIZE() */
#include "cmdbg.h"     /* Interface to CM debug services */

#include "cmtask.h"    /* Interface to command queue processing */
#include "cmss.h"      /* Serving system info structure */
#include "cmph.h"      /* Phone object interface */

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
#define CMIPAPP_POOL_SIZE  10
    /* Total number of apps that CM can support  */



/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* Stores records for registered apps
*/
typedef struct cmipapp_s {

  cmipapp_id_type                   app_id;
    /* App ID
    */

  cmipapp_call_type_e_type          call_type;
    /* App call type type
    */

  cmipapp_sys_mode_e_type           sys_mode;
    /* System modes app can support for calls
    */

  cmipapp_run_status_e_type         app_run_status;
   /* CMIPAPP_RUN_STATUS_ACTIVE  - App is active
   ** CMIPAPP_RUN_STATUS_INACTIVE  - App is inactive
   */


  cmipapp_cb_s_type                  app_cb;
   /* Structure holds function pointers that
   ** are registered by Apps. CM calls these
   ** functions for call related services
   */

  cmipapp_cap_reg_reason_s_type      reason;
    /* reason carries details of success or failure  
    ** with respect to app registeration with network.
    */  

  cmipapp_trans_id_type              active_trans_id;
    /* Trasaction id sent to apps along with
    ** oprt_mode. This id has to be returned
    ** when apps call cmipapp_oprt_mode_cnf.
    */

  sys_oprt_mode_e_type               oprt_mode_sent;
    /* Operating mode that was last sent
    ** to apps.
    */

} cmipapp_s_type;

/* Stores details of all apps
*/
static cmipapp_s_type cmipapp_arr[CMIPAPP_POOL_SIZE];

/**--------------------------------------------------------------------------
** Functions - Internal
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmipapp_send_reg_status

DESCRIPTION
  Sends CM ip app registration status

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void cmipapp_send_reg_status (

  cmipapp_id_type                app_id,
   /* App ID
   */

  cm_ipapp_reg_status_e_type     ipapp_reg_status
   /* ipapp's registration status
   */

)
{

 cm_sd_rpt_u_type      *rpt_cmd_ptr = NULL;

 /*----------------------------------------------------------------------*/

 /* Using sd report queue to pass on ipapp registration message. Since ipapp
 ** reports are common to technologies, need to use a way that is common to 
 ** all.
 */
 if ((rpt_cmd_ptr = cm_sd_rpt_get_buf_else_err_fatal()) != NULL)
 {
   /* Fill in command type */
   rpt_cmd_ptr->hdr.cmd                            = CM_IP_APP_REG_STATUS;

   /* App registration status */
   rpt_cmd_ptr->ip_app_reg_status.ipapp_reg_status = ipapp_reg_status;

   /* Call type's that app can service */

   /* Copying bit mask call type's app can support
   */
   rpt_cmd_ptr->ip_app_reg_status.call_type_mask   = 
                                      cmipapp_arr[app_id].call_type;

   /* cause for registration, de-registration */
   rpt_cmd_ptr->ip_app_reg_status.ipapp_cause      = 
                                      cmipapp_arr[app_id].reason.ipapp_cause;

   /* Warning info tied to registration */
   rpt_cmd_ptr->ip_app_reg_status.warn_info        = 
                                      cmipapp_arr[app_id].reason.warn_info;

   /* System modes app can support for calls */
   rpt_cmd_ptr->ip_app_reg_status.sys_mode         =
            (sys_sys_mode_mask_e_type)(cmipapp_arr[app_id].sys_mode);
   
   /* Put message in destination task queue */
   cm_sd_rpt (rpt_cmd_ptr);
 } 

} /* cmipapp_send_reg_status () */


/*===========================================================================

FUNCTION cmipapp_next_trans_id

DESCRIPTION
  Gives the next transaction identifier.

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint32 cmipapp_next_trans_id ( void )
{
  static uint32 trans_id = CMIPAPP_TRANS_ID_NONE;


  return ++trans_id;

}


/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmipapp_reset

DESCRIPTION
  Initializes the app entry pointed by app_id
  CAUTION:- Function gets called within a lock since it accesses app_id that
            can be used by multiple apps. Function should not wait on any
            event.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmipapp_reset(

  cmipapp_id_type                         app_id
    /* App id assigned by CM
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  /* Reset all details of the particular app
  */
  cmipapp_arr[app_id].app_id         = CMIPAPP_ID_INVALID;

  /* Call types app can service */
  cmipapp_arr[app_id].call_type      = CMIPAPP_CALL_TYPE_NONE;

  /* Technologies app can work on */
  cmipapp_arr[app_id].sys_mode       = CMIPAPP_SYS_MODE_NONE;

  /* APP status active/inactive */
  cmipapp_arr[app_id].app_run_status = CMIPAPP_RUN_STATUS_NONE;

  /* ip app registration/dereg/failure cause */
  cmipapp_arr[app_id].reason.ipapp_cause         = CM_IPAPP_CAUSE_NONE;

  /* ip app registration/dereg/failure warning */
  cmipapp_arr[app_id].reason.warn_info.present = FALSE;


  /* Initializes transaction id to NONE */
  cmipapp_arr[app_id].active_trans_id = CMIPAPP_TRANS_ID_NONE;

  /* Initializes oprt mode sent to NONE */
  cmipapp_arr[app_id].oprt_mode_sent  = SYS_OPRT_MODE_NONE;

  memset ((byte *)&(cmipapp_arr[app_id].app_cb), 0,
                              sizeof(cmipapp_arr[app_id].app_cb));

} /* cmipapp_reset() */



/*===========================================================================

FUNCTION cmipapp_reset_trans_id

DESCRIPTION
  Reset trans id being used for tracking app's oprt mode cnf

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  Clears trans_id in cmipapp_arr[]

===========================================================================*/
void cmipapp_reset_trans_id (void)
{

  /*----------------------------------------------------------------------*/

  unsigned int id_count = 0;
    /* Index
    */

  /*----------------------------------------------------------------------*/

  /* Obtain a lock
  */
  rex_enter_crit_sect(cmtask_crit_sect_ptr());

  /* Check is made if there is an app registered
  ** with required call type and system mode
  */
  for (id_count = 0; id_count < ARR_SIZE(cmipapp_arr); id_count++)
  {  
    cmipapp_arr[id_count].active_trans_id = CMIPAPP_TRANS_ID_NONE;

  }

  /* Free the lock
  */
  rex_leave_crit_sect(cmtask_crit_sect_ptr());

  /*----------------------------------------------------------------------*/

} /* cmipapp_reset_trans_id () */


/*===========================================================================

FUNCTION cmipapp_init

DESCRIPTION
  Initializes table to maintain app details. Reads phone identity

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(714, cmipapp_init) */  
void cmipapp_init(void)
{
  cmipapp_id_type app_id;

  /*----------------------------------------------------------------------*/

  /* Initialize cmipapp_arr[] with invalid app ids and the state of app
  ** to inactive
  */
  memset (cmipapp_arr, 0, sizeof(cmipapp_arr));

  for (app_id = 0; app_id < ARR_SIZE(cmipapp_arr); app_id++)
  {
    cmipapp_reset (app_id);
  }

  /* Read phone identity and cache it for read by cmipapp_get_ph_identity()
  */

} /* cmipapp_init() */




/*===========================================================================

FUNCTION cmipapp_cb_init

DESCRIPTION
  Initializes function pointers to NULL

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  Modifies argument with pointers to call related functions

===========================================================================*/
void cmipapp_cb_init(

  cmipapp_cb_s_type                *app_cb_ptr
    /* Structure to be initialized with callback functions
    */
)
{

  if (app_cb_ptr == NULL)
  {
    return;
  }

  /* Initialize structure fields with default call back values */
  /* Points to call origination function */
  app_cb_ptr->cmipapp_cmd_orig_func_ptr                   =   NULL;

  /* Points to call answering function */
  app_cb_ptr->cmipapp_cmd_answer_func_ptr                 =   NULL;

  /* Points to incoming call response function */
  app_cb_ptr->cmipapp_cmd_invite_res_func_ptr             =   NULL;

  /* Points to call end function */
  app_cb_ptr->cmipapp_cmd_end_func_ptr                    =   NULL;

  /* Points to start continuous dtmf function */
  app_cb_ptr->cmipapp_inband_cmd_start_cont_dtmf_func_ptr =   NULL;

  /* Points to stop continuous dtmf function */
  app_cb_ptr->cmipapp_inband_cmd_stop_cont_dtmf_func_ptr  =   NULL;

  /* To put call on hold */
  app_cb_ptr->cmipapp_cmd_hold_func_ptr                   =   NULL;

  /* To resume a call on hold */
  app_cb_ptr->cmipapp_cmd_active_func_ptr                 =   NULL;

  /* To modify call type */
  app_cb_ptr->cmipapp_cmd_modify_func_ptr                 =   NULL;

  /* To inform change in serving system parameters */
  app_cb_ptr->cmipapp_cmd_serv_sys_info_ptr               =   NULL;

  app_cb_ptr->cmipapp_cmd_act_answer_hold_func_ptr        =   NULL;


  app_cb_ptr->cmipapp_cmd_deact_answer_hold_func_ptr      =   NULL;

  app_cb_ptr->cmipapp_cmd_answer_call_waiting_func_ptr    =   NULL;

  app_cb_ptr->cmipapp_cmd_ect_func_ptr                    =   NULL;

  app_cb_ptr->cmipapp_cmd_mpty_func_ptr                   =   NULL;
  
  app_cb_ptr->cmipapp_inform_oprt_mode                    =   NULL;  

  app_cb_ptr->cmipapp_cmd_notify_ho_func_ptr              =   NULL;

  /* To send MO SMS over IP app */
  app_cb_ptr->cmipapp_sms_cmd_mo_sms_func_ptr             =   NULL;

} /* cmipapp_cb_init() */



/*===========================================================================

FUNCTION cmipapp_reg

DESCRIPTION
  Used by application to register callback functions with CM

DEPENDENCIES
  cmipapp_init

RETURN VALUE

  CMIPAPP_STATUS_OK         - Registration Successful

  CMIPAPP_STATUS_CB_REG_FAILED - Registration Failed

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg (

  const cmipapp_cb_s_type          *app_cb_ptr,
    /* Structure pointing to app call back functions
    */

  cmipapp_id_type                   app_id
   /* App id that is registering call backs
   */

)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ((app_cb_ptr == NULL) ||
      (!(app_id < ARR_SIZE(cmipapp_arr)))
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* Copy the call backs that app wants to register in cmipapp_arr
  */
  memcpy (&(cmipapp_arr[app_id].app_cb), app_cb_ptr,
                                           sizeof(cmipapp_cb_s_type));

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg () */



/*===========================================================================

FUNCTION cmipapp_id_deallocate

DESCRIPTION
  Application calls to deallocate app id

DEPENDENCIES
  cmipapp_init

RETURN VALUE

  CMIPAPP_STATUS_OK                - App id deallocation Successful
  CMIPAPP_STATUS_ID_DEALLOC_FAILED - App id deallocation Failed

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_id_deallocate (

  cmipapp_id_type                         app_id
    /* App id assigned by CM
    */
)
{

  /*----------------------------------------------------------------------*/

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if (!(app_id < ARR_SIZE(cmipapp_arr)))
  {
    return CMIPAPP_STATUS_ID_DEALLOC_FAILED;
  }

  /* Obtain a lock
  */
  rex_enter_crit_sect(cmtask_crit_sect_ptr());

  /* Reset all details of the particular app
  */
  cmipapp_reset (app_id);

  /* Free the lock
  */
  rex_leave_crit_sect(cmtask_crit_sect_ptr());

  /* Update the DDTM status.
  */
  cmph_update_ddtm_status( cmph_ptr() );

  CM_MSG_HIGH (" app_id %d deallocated", app_id, 0, 0);

  return CMIPAPP_STATUS_OK;

} /* cmipapp_id_deallocate () */



/*===========================================================================

FUNCTION cmipapp_find_active_app

DESCRIPTION
  Finds an active app with a particular sys_mode and call type

DEPENDENCIES
  None

RETURN VALUE

  App id

SIDE EFFECTS
  None

===========================================================================*/
/*lint -esym(714, cmipapp_find_active_app) */
cmipapp_id_type cmipapp_find_active_app (

  sys_sys_mode_e_type     sys_mode,
    /* The system mode
    */

  cm_call_type_e_type     call_type
    /* call type
    */

)
{

  cmipapp_id_type   app_id    = CMIPAPP_ID_INVALID;
    /* app id
    */

  unsigned int id_count = 0;
    /* Index
    */

  /*----------------------------------------------------------------------*/

  /* Sanity check on call_type and system mode
  */
  CM_ASSERT_ENUM_IS_INRANGE (call_type, CM_CALL_TYPE_MAX);   /*lint !e641 */
  CM_ASSERT_ENUM_IS_INRANGE (sys_mode,  SYS_SYS_MODE_MAX);   /*lint !e641 */

  /* Obtain a lock
  */
  rex_enter_crit_sect(cmtask_crit_sect_ptr());

  /* Check is made if there is an app registered
  ** with required call type and system mode
  */
  for (id_count = 0; id_count < ARR_SIZE(cmipapp_arr); id_count++)
  {
    /* CM only routes the service when the IP app is in the active state.
     */
    if((cmipapp_arr[id_count].app_id != CMIPAPP_ID_INVALID) 
       &&
       (cmipapp_arr[id_count].call_type & SYS_BM_64BIT(call_type)) /*lint !e641 */           
       &&   
      (cmipapp_arr[id_count].sys_mode & SYS_BM_64BIT(sys_mode)) /*lint !e641 */       
       &&
      (cmipapp_arr[id_count].app_run_status == CMIPAPP_RUN_STATUS_ACTIVE ) 
      )
    {
      app_id = cmipapp_arr[id_count].app_id;

      CM_MSG_HIGH (" Found active app %d for call type %d, sys mode %d", 
                     app_id, call_type, sys_mode);
      break;
    }

  }

  /* Free the lock
  */
  rex_leave_crit_sect(cmtask_crit_sect_ptr());

  return app_id;

} /* cmipapp_find_active_app () */


/*===========================================================================

FUNCTION cmipapp_get_cb_ptrs

DESCRIPTION
  Returns callback functions registered by an app

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  NULL - if operation to get call back ptrs FAILS
  valid ptr to call back ptrs

SIDE EFFECTS
  None
===========================================================================*/

cmipapp_cb_s_type *cmipapp_get_cb_ptrs (

  cmipapp_id_type             app_id
    /* App id
    */
)
{
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if (!(app_id < ARR_SIZE(cmipapp_arr)))
  {
    return NULL;
  }

  /* Return function ptr for the particular app
  */
  return &cmipapp_arr[app_id].app_cb;

}

/*===========================================================================

FUNCTION cmipapp_call_type_reg

DESCRIPTION
  Allows apps to register call handling capabilities

DEPENDENCIES
  cmipapp_init

RETURN VALUE

  CMIPAPP_STATUS_OK                   - Successfully registered
  CMIPAPP_STATUS_CALL_TYPE_REG_FAILED - Error in registering app capabilities

SIDE EFFECTS
  None
===========================================================================*/

cmipapp_status_e_type cmipapp_cap_reg (

  cmipapp_id_type                 app_id,
    /* App id
    */

  cmipapp_call_type_e_type        call_type,
    /* Bit Masks of call type it supports
    */

  cmipapp_sys_mode_e_type         sys_mode
    /* System modes app can support for calls
    */
)
{

  /* Check for invalid app id
  */
  if (app_id == CMIPAPP_ID_INVALID)
  {
    return CMIPAPP_STATUS_CALL_TYPE_REG_FAILED;
  }

  /* Sanity check on call_type and system mode
  */
  CM_ASSERT (BETWEEN (call_type, CMIPAPP_CALL_TYPE_NONE,
                                 CMIPAPP_CALL_TYPE_MAX)); /*lint !e641 */
  CM_ASSERT (BETWEEN (sys_mode, CMIPAPP_SYS_MODE_NONE, 
                                 CMIPAPP_SYS_MODE_MAX));   /*lint !e641 */

  /* Access the table to set call_type and system mode
  */
  cmipapp_arr[app_id].call_type      = call_type;
  cmipapp_arr[app_id].sys_mode       = sys_mode;

  CM_MSG_HIGH (" App %d registers for call type %d, sys mode %d",
                 app_id, call_type, sys_mode);

  return CMIPAPP_STATUS_OK;

} /* cmipapp_cap_reg () */


/*===========================================================================

FUNCTION cmipapp_cap_reg_with_reason

DESCRIPTION
  Allows clients to register call handling capabilities

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  TRUE - Successfully registered, FALSE - Error in registering

SIDE EFFECTS
  None
===========================================================================*/

cmipapp_status_e_type cmipapp_cap_reg_with_reason (

  cmipapp_id_type                 app_id,
    /* App ID
    */

  cmipapp_call_type_e_type        call_type,
    /* Bit Masks of call type it supports
    */

  cmipapp_sys_mode_e_type         sys_mode,
    /* System modes app can support for calls
    */

  cmipapp_cap_reg_reason_s_type   reason
    /* reason carries success or details of failure  
    ** with respect to app registeration with network.
    */  
)
{

  cmipapp_status_e_type  app_reg_status = CMIPAPP_STATUS_NONE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check for invalid app id
  */
  if (app_id == CMIPAPP_ID_INVALID)
  {
    return CMIPAPP_STATUS_CALL_TYPE_REG_FAILED;
  }

  /* Sanity check on call_type and system mode
  */
  CM_ASSERT (BETWEEN (call_type, CMIPAPP_CALL_TYPE_NONE, CMIPAPP_CALL_TYPE_MAX)); /*lint !e641 */
  CM_ASSERT (BETWEEN (sys_mode, CMIPAPP_SYS_MODE_NONE, CMIPAPP_SYS_MODE_MAX));   /*lint !e641 */

  /* Access the table to set call_type and system mode  
  */  
  app_reg_status = cmipapp_cap_reg (app_id, call_type, sys_mode);

  if (app_reg_status == CMIPAPP_STATUS_OK)
  { 
    cmipapp_arr[app_id].reason         = reason;  
  }

  return app_reg_status;

}

/*===========================================================================

FUNCTION cmipapp_inform_status

DESCRIPTION
  App informs CM if it is active or inactive

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  CMIPAPP_STATUS_OK             - Successfully
  CMIPAPP_STATUS_INFORM_FAILED  - Error in processing app inform cmd

SIDE EFFECTS
  None
===========================================================================*/
cmipapp_status_e_type cmipapp_inform_status (

  cmipapp_id_type                app_id,
   /* App ID
   */

  cmipapp_run_status_e_type      app_run_status
   /* CMIPAPP_RUN_STATUS_ACTIVE  - App is active
   ** CMIPAPP_RUN_STATUS_INACTIVE  - App is inactive
   */
)
{ 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  


  CM_MSG_HIGH (" App %d informs status %d",
                 app_id, app_run_status, 0);

  /* Check for invalid app id
  */
  if (app_id == CMIPAPP_ID_INVALID)
  {
    return CMIPAPP_STATUS_INFORM_FAILED;
  }

  /* Sanity check on app run status
  */
  CM_ASSERT_ENUM_IS_INRANGE (app_run_status, CMIPAPP_RUN_STATUS_MAX); /*lint !e641 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  /* Status transition from Inactive to Inactive
  */
  if ( cmipapp_arr[app_id].app_run_status == CMIPAPP_RUN_STATUS_INACTIVE 
       &&
       app_run_status ==  CMIPAPP_RUN_STATUS_INACTIVE
     )
  {
    cmipapp_send_reg_status (app_id, CM_IPAPP_REG_STATUS_FAILURE);

      /* Access the table to set the app status
    */
    cmipapp_arr[app_id].app_run_status = app_run_status;

    CM_MSG_HIGH (" App %d is INACTIVE", app_id, 0, 0);

    return CMIPAPP_STATUS_OK;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  /* Status transition from Active to Inactive
  */
  if ( cmipapp_arr[app_id].app_run_status == CMIPAPP_RUN_STATUS_ACTIVE 
       &&
       app_run_status ==  CMIPAPP_RUN_STATUS_INACTIVE
     )
  {
    cmipapp_send_reg_status (app_id, CM_IPAPP_REG_STATUS_DEREGISTERED);

      /* Access the table to set the app status
    */
    cmipapp_arr[app_id].app_run_status = app_run_status;

    CM_MSG_HIGH (" App %d is INACTIVE", app_id, 0, 0);

    /* Update the DDTM status.
    */
    cmph_update_ddtm_status( cmph_ptr() );

    return CMIPAPP_STATUS_OK;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Status transition to Active.
  ** does not depend on current state.
  */
  if (app_run_status ==  CMIPAPP_RUN_STATUS_ACTIVE)
  {

    /* for ipapp to be made Active, all callbacks should be available
    */
    if ( (cmipapp_arr[app_id].app_cb.cmipapp_cmd_orig_func_ptr == NULL)
       ||
      (cmipapp_arr[app_id].app_cb.cmipapp_cmd_answer_func_ptr == NULL)
       ||
       (cmipapp_arr[app_id].app_cb.
                                cmipapp_cmd_invite_res_func_ptr == NULL)
       ||
       (cmipapp_arr[app_id].app_cb.cmipapp_cmd_end_func_ptr == NULL)
       ||
       (cmipapp_arr[app_id].app_cb.
         cmipapp_inband_cmd_start_cont_dtmf_func_ptr == NULL)
       ||
       (cmipapp_arr[app_id].app_cb.
         cmipapp_inband_cmd_stop_cont_dtmf_func_ptr == NULL)
       ||
       (cmipapp_arr[app_id].app_cb.cmipapp_cmd_hold_func_ptr == NULL)
       ||
      (cmipapp_arr[app_id].app_cb.cmipapp_cmd_active_func_ptr == NULL)
       ||
      (cmipapp_arr[app_id].app_cb.
                             cmipapp_cmd_modify_func_ptr == NULL)
     )
    {
      return CMIPAPP_STATUS_INFORM_FAILED;
    }

    cmipapp_send_reg_status (app_id, CM_IPAPP_REG_STATUS_REGISTERED); 

    /* Access the table to set the app status
    */
    cmipapp_arr[app_id].app_run_status = app_run_status;

    CM_MSG_HIGH (" App %d is ACTIVE", app_id, 0, 0);

    /* Update the DDTM status.
    */
    cmph_update_ddtm_status( cmph_ptr() );

    return CMIPAPP_STATUS_OK;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   

  CM_ERR (" run_status is invalid", 0, 0, 0 ); 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return CMIPAPP_STATUS_INFORM_FAILED;

} /* cmipapp_inform_status () */



/*===========================================================================

FUNCTION cmipapp_inform_status_with_callid

DESCRIPTION
  App informs CM if videoshare call for a voice call is available

DEPENDENCIES
  cmipapp_init
  General status of the app should be active

RETURN VALUE
  CMIPAPP_STATUS_OK             - Successfully
  CMIPAPP_STATUS_INFORM_FAILED  - Error in processing app inform cmd

SIDE EFFECTS
  None
===========================================================================*/
cmipapp_status_e_type cmipapp_inform_status_with_callid (

  cmipapp_id_type                app_id,
   /* App ID
   */

  cmipapp_run_status_e_type      app_run_status,
   /* CMIPAPP_RUN_STATUS_ACTIVE  - App is active
   ** CMIPAPP_RUN_STATUS_INACTIVE  - App is inactive
   */

  cm_call_id_type                voice_call_id
    /* voice_call_id for which videoshare is available
    */
)
{ 
  
  cmipapp_ind_u_type      ip_ind;
    /* cm ipapp report */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check for invalid app id
  */
  if ((app_id == CMIPAPP_ID_INVALID) ||
      /*lint -e641*/
      (cmipapp_arr[app_id].app_run_status != CMIPAPP_RUN_STATUS_INACTIVE) 
      /*lint +e641*/
     )
  {
    return CMIPAPP_STATUS_INFORM_FAILED;
  }

  /* Sanity check on app run status
  */
  CM_ASSERT_ENUM_IS_INRANGE (app_run_status, CMIPAPP_RUN_STATUS_MAX); /*lint !e641 */


  /*  
  ** Report videoshare availability to CM
  */
  
  /* Queue a report informing CM that videoshare is available for 
  ** voice_call_id
  */
  ip_ind.vs_status.id         = voice_call_id;

  /* Active app maps to videoshare being available.
  ** Inactive app maps to videoshare not being available
  */
  if (app_run_status == CMIPAPP_RUN_STATUS_INACTIVE)
  {
    ip_ind.vs_status.is_vs_poss = FALSE;  
  }
  else if (app_run_status == CMIPAPP_RUN_STATUS_ACTIVE)
  {
    ip_ind.vs_status.is_vs_poss = TRUE;
  }
  
  /* data_block is useful during the course of a call.
  ** Since this api will be getting called to inform 
  ** a general status, data_block support to app is not
  ** needed.
  */
  cmipapp_rpt_ind (CM_IP_CALL_IND_VS_STATUS, ip_ind, 0);

  return CMIPAPP_STATUS_OK;

} /* cmipapp_inform_status_with_callid () */


/*===========================================================================

FUNCTION cmipapp_id_allocate

DESCRIPTION
  Returns a free index from cmipapp_arr[]

DEPENDENCIES
  cmipapp_init

RETURN VALUE

  CMIPAPP_STATUS_APPID_PTR_ERR - Supplied app id ptr is NULL

  CMIPAPP_STATUS_APPID_EXHAUST - No free app id available

  CMIPAPP_STATUS_OK            - App id successfully allocated

SIDE EFFECTS

  Modifies global cmipapp_arr[].
===========================================================================*/
cmipapp_status_e_type cmipapp_id_allocate (

    cmipapp_call_type_e_type     call_type,
      /* App type
      */

    cmipapp_sys_mode_e_type      sys_mode,
      /* System modes app can support for calls
      */

    cmipapp_id_type             *app_id_ptr
      /* App ID pointer
      */
)
{

  cmipapp_id_type             app_id = CMIPAPP_ID_INVALID;
    /* Stores the app id returned by cmipapp_id_allocate ()
    */

  boolean  is_duplicate_app_present  = FALSE;
    /* Checks for the presence of a duplicate
    ** app with same call_type and system mode
    */

  cmipapp_id_type         id_count   = 0;
    /* App id loop
    */

  /*----------------------------------------------------------------------*/


  /* Verify if the call type and system mode values are valid
  */
  CM_ASSERT (BETWEEN (call_type, CMIPAPP_CALL_TYPE_NONE, 
                                   CMIPAPP_CALL_TYPE_MAX)); /*lint !e641 */
  CM_ASSERT (BETWEEN (sys_mode, CMIPAPP_SYS_MODE_NONE, 
                                   CMIPAPP_SYS_MODE_MAX));   /*lint !e641 */
  

  if (app_id_ptr == NULL)
  {
    return CMIPAPP_STATUS_APPID_PTR_ERR;
  }

  /* Only one app can exist for a combination of
  ** call type and system mode. Travel through
  ** the list of apps to find a duplicate, if
  ** none get a app_id returned.
  */

  rex_enter_crit_sect(cmtask_crit_sect_ptr());

  /* Check is made if there is an alternate app registered
  ** with same call type and system mode
  */
  for (id_count = 0; id_count < ARR_SIZE(cmipapp_arr); id_count++)
  {

    if ((cmipapp_arr[id_count].app_id    != CMIPAPP_ID_INVALID)
         &&
        (cmipapp_arr[id_count].call_type == call_type)
         &&
        (cmipapp_arr[id_count].sys_mode  == sys_mode)
       )
    {
      is_duplicate_app_present = TRUE;
      break;
    }

  }

  /* If there is no duplicate app registered
  ** allocate a app id
  */
  if (!is_duplicate_app_present)
  {
    for (id_count = 0; id_count < ARR_SIZE(cmipapp_arr); id_count++)
    {
      if (cmipapp_arr[id_count].app_id == CMIPAPP_ID_INVALID)
      {
        app_id                      = id_count;

        cmipapp_reset (app_id);
        cmipapp_arr[app_id].app_id  = app_id;

        /* Access the table to set sys_mode and call type
        */
        cmipapp_arr[app_id].call_type      = call_type;
        cmipapp_arr[app_id].sys_mode       = sys_mode;

        break;
      }
    }
  }
  rex_leave_crit_sect(cmtask_crit_sect_ptr());

  if (app_id == CMIPAPP_ID_INVALID)
  {
    if (is_duplicate_app_present)
    {
      return CMIPAPP_STATUS_DUP_APP_EXISTS;
    }
    else
    {
      return CMIPAPP_STATUS_APPID_EXHAUST;
    }
  }

  CM_MSG_HIGH( "App id being allocated = %d", app_id, 0, 0 );


  /* Set app id ptr to the assigned app id
  */
  *app_id_ptr = app_id;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_id_allocate () */



/*===========================================================================

FUNCTION cmipapp_reg_orig_func

DESCRIPTION
  Registers function for the app to originate a call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_orig_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_orig_f_type                 *cmipapp_cmd_orig_func_ptr
    /* Ptr to call orig function
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_cmd_orig_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }
  
  /* Origination function assigment */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_orig_func_ptr = 
                                cmipapp_cmd_orig_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_orig_func () */


/*===========================================================================

FUNCTION cmipapp_reg_answer_func

DESCRIPTION
  Registers function for the app to answer a MT call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_answer_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_answer_f_type              *cmipapp_cmd_answer_func_ptr
    /* Ptr to MT call answer function
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_cmd_answer_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* Answer function assigment */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_answer_func_ptr = 
                                cmipapp_cmd_answer_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_answer_func () */


/*===========================================================================

FUNCTION cmipapp_reg_answer_call_waiting_func

DESCRIPTION
  Registers function for the app to answer an call waiting MT call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_answer_call_waiting_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_answer_call_waiting_f_type *answer_call_waiting_func_ptr
    /* Ptr to MT call answer function
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!answer_call_waiting_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* Answer function assigment */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_answer_call_waiting_func_ptr = 
                                answer_call_waiting_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_answer_call_waiting_func () */

/*===========================================================================

FUNCTION cmipapp_reg_invite_res_func

DESCRIPTION
  Registers function for the app to respond to a invite message.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_invite_res_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_invite_res_f_type          *cmipapp_cmd_invite_res_func_ptr
    /* Ptr to invite response function
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_cmd_invite_res_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* Invite response function assigment */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_invite_res_func_ptr = 
                                cmipapp_cmd_invite_res_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_invite_res_func () */


/*===========================================================================

FUNCTION cmipapp_reg_end_func

DESCRIPTION
  Registers function for the app to end a call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_end_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_end_f_type                 *cmipapp_cmd_end_func_ptr
    /* Ptr to call end function
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_cmd_end_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* End function assigment */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_end_func_ptr = 
                                  cmipapp_cmd_end_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_end_func () */


/*===========================================================================

FUNCTION cmipapp_reg_inband_start_cont_dtmf_func

DESCRIPTION
  Registers function for the app to start continuous inband dtmf.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_inband_start_cont_dtmf_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_inband_cmd_start_cont_dtmf_f_type

                              *cmipapp_inband_cmd_start_cont_dtmf_func_ptr
    /* Ptr to start inband dtmf function
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_inband_cmd_start_cont_dtmf_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to start cont dtmf */
  cmipapp_arr[app_id].app_cb.cmipapp_inband_cmd_start_cont_dtmf_func_ptr = 
                                cmipapp_inband_cmd_start_cont_dtmf_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_inband_start_cont_dtmf_func () */



/*===========================================================================

FUNCTION cmipapp_reg_inband_stop_cont_dtmf_func

DESCRIPTION
  Registers function for the app to stop inband continuous dtmf.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_inband_stop_cont_dtmf_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_inband_cmd_stop_cont_dtmf_f_type

                              *cmipapp_inband_cmd_stop_cont_dtmf_func_ptr
    /* Ptr to stop inband dtmf function
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_inband_cmd_stop_cont_dtmf_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to stop cont dtmf */
  cmipapp_arr[app_id].app_cb.cmipapp_inband_cmd_stop_cont_dtmf_func_ptr = 
                                cmipapp_inband_cmd_stop_cont_dtmf_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_inband_stop_cont_dtmf_func () */





/*===========================================================================

FUNCTION cmipapp_reg_hold_func

DESCRIPTION
  Registers function for the app to hold a call

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_hold_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_hold_f_type                *cmipapp_cmd_hold_func_ptr
    /* Ptr to call hold function 
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_cmd_hold_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to Hold call */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_hold_func_ptr = 
                                     cmipapp_cmd_hold_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_hold_func () */


/*===========================================================================

FUNCTION cmipapp_reg_active_func

DESCRIPTION
  Registers function for the app to make the call active.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_active_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_active_f_type              *cmipapp_cmd_active_func_ptr
    /* Ptr to call active function
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_cmd_active_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to put call as active */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_active_func_ptr = 
                                     cmipapp_cmd_active_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_active_func () */


/*===========================================================================

FUNCTION cmipapp_reg_modify_func

DESCRIPTION
  Registers function for the app to modify the call type (ex:- voice to VT).

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_modify_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_modify_f_type              *cmipapp_cmd_modify_func_ptr
    /* Ptr to call modify function
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_cmd_modify_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to modify call */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_modify_func_ptr = 
                                     cmipapp_cmd_modify_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_modify_func () */

/*===========================================================================

FUNCTION cmipapp_reg_serv_sys_info_func

DESCRIPTION
  Registers function for the app to inform app about current serving system.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_serv_sys_info_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_srv_sys_info_f_type        *cmipapp_cmd_srv_sys_info_func_ptr
    /* Ptr to serving system info
    */
)
{
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_cmd_srv_sys_info_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to inform about serving system change */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_serv_sys_info_ptr = 
                                    cmipapp_cmd_srv_sys_info_func_ptr;

  return CMIPAPP_STATUS_OK;



}


/*===========================================================================

FUNCTION cmipapp_reg_act_answer_hold_func

DESCRIPTION
  Registers function for the app to activate answer hold.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_act_answer_hold_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_act_answer_hold_f_type 
                                      *cmipapp_cmd_act_answer_hold_func_ptr
    /* Ptr to activate answer hold function
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_cmd_act_answer_hold_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to activate answer hold */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_act_answer_hold_func_ptr = 
                                     cmipapp_cmd_act_answer_hold_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_act_answer_hold_func () */


/*===========================================================================

FUNCTION cmipapp_reg_deact_answer_hold_func

DESCRIPTION
  Registers function for the app to de-activate answer hold.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_deact_answer_hold_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_deact_answer_hold_f_type 
                                      *cmipapp_cmd_deact_answer_hold_func_ptr
    /* Ptr to de-activate answer hold function
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_cmd_deact_answer_hold_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to deactivate answer hold */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_deact_answer_hold_func_ptr = 
                                     cmipapp_cmd_deact_answer_hold_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_deact_answer_hold_func () */


/*===========================================================================

FUNCTION cmipapp_reg_ect_func

DESCRIPTION
  Registers function for the app to provide Call transfer funcitonality.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_ect_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_ect_f_type 
                                      *cmipapp_cmd_ect_func_ptr
    /* Ptr to call transfer function.
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_cmd_ect_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment for ect */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_ect_func_ptr = 
                                     cmipapp_cmd_ect_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_ect_func () */


/*===========================================================================

FUNCTION cmipapp_reg_mpty_func

DESCRIPTION
  Registers function for the app to provide Multiparty functionality.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_mpty_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_mpty_f_type 
                                      *cmipapp_cmd_mpty_func_ptr
    /* Ptr to de-activate answer hold function
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_cmd_mpty_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment for mpty */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_mpty_func_ptr = 
                                     cmipapp_cmd_mpty_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_mpty_func () */



/*===========================================================================

FUNCTION cmipapp_reg_ph_oprt_mode_func

DESCRIPTION
  Registers function for the app to be informed of Phone's operating mode.  

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK             - Registration successfull.
  CMIPAPP_STATUS_CB_REG_FAILED  - Registration failed.

SIDE EFFECTS
  None
===========================================================================*/
cmipapp_status_e_type cmipapp_reg_oprt_mode_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_inform_oprt_mode_f_type 
                                      *cmipapp_inform_oprt_mode_func_ptr
    /* Ptr to record oprt mode function
    */
)
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_inform_oprt_mode_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  CM_MSG_HIGH ("Registering oprt_mode_func", 0, 0, 0);

  /* Registering cb for informing app phone's operating mode */
  cmipapp_arr[app_id].app_cb.cmipapp_inform_oprt_mode = 
                                     cmipapp_inform_oprt_mode_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_oprt_mode_func */


/*===========================================================================

FUNCTION cmipapp_reg_mo_sms_func

DESCRIPTION
  Registers function for the app to be informed of MO SMS message.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_mo_sms_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_sms_cmd_mo_sms_f_type 
                                       *cmipapp_sms_cmd_mo_sms_func_ptr
    /* Ptr to send MO SMS message function
    */
)
{
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( app_id >= ARR_SIZE(cmipapp_arr) ||     
       ( ! cmipapp_sms_cmd_mo_sms_func_ptr )
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment for mpty */
  cmipapp_arr[app_id].app_cb.cmipapp_sms_cmd_mo_sms_func_ptr= 
                                     cmipapp_sms_cmd_mo_sms_func_ptr;

  return CMIPAPP_STATUS_OK;

}/* cmipapp_reg_mo_sms_func */


/*===========================================================================

FUNCTION cmipapp_rpt_get_buf_else_err_fatal

DESCRIPTION
  Allocate a IP app rpt buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  Pointer to allocated IP app rpt buffer.

SIDE EFFECTS
  none

===========================================================================*/
static cmipapp_rpt_s_type *cmipapp_rpt_get_buf_else_err_fatal (

  void
)
{

  cmipapp_rpt_s_type         *cmipapp_rpt_ptr = NULL;
    /* CM report command buffer */

  /*----------------------------------------------------------------------*/

  /* Try allocating a CM report command buffer
  */
  cmipapp_rpt_ptr = (cmipapp_rpt_s_type *) cm_mem_malloc(sizeof(cmipapp_rpt_s_type));

  /* If allocation failed, error fatal.
  */
  if( cmipapp_rpt_ptr == NULL )
  {
    CM_ERR_FATAL( "Unable to allocate CM IPAPP Rpt",0,0,0 );
  }

  cmipapp_rpt_ptr->hdr.task_ptr    = NULL;

  return cmipapp_rpt_ptr;

}  /* cmipapp_rpt_get_buf_else_err_fatal */



/*===========================================================================

FUNCTION cmipapp_rpt_ind

DESCRIPTION
  Allows Voip Apps to queue up messages to the CM. Grab a buffer from the
  cmipapp_rpt_free_q, fill it in, and call cmipapp_rpt_ind with it.
  All this will do is queue it and set a signal for the CM task.  When the
  command is done, the buffer will be queued as directed in the command
  header, and a task you specify may be signaled.

DEPENDENCIES
  cm_ip_rpt_free_q already initialized, cmipapp_init

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipapp_rpt_ind (

  cm_name_type        ip_rpt_name,
   /* Name of report being queued to CM
   */

  cmipapp_ind_u_type ip_ind,
   /* Union of all rpt types getting queued to CM
   */

  uint32           data_block
   /* Token created and used by CM.
   ** data_block is passed by CM, when calling app registered
   ** callbacks. App needs to pass the most recent value
   ** given by CM, all the time when cmipapp_rpt_ind () is called.
   */

)
{
  cmipapp_rpt_s_type         *cmipapp_rpt_ptr = NULL;
    /* CM report command buffer */

  /*----------------------------------------------------------------------*/

  CM_MSG_HIGH("RPT name %d ", ip_rpt_name, 0, 0 );

  /* Allocate rpt buffer
  */
  cmipapp_rpt_ptr          = cmipapp_rpt_get_buf_else_err_fatal ();

  /* Name of report being queued
  */
  cmipapp_rpt_ptr->hdr.cmd = ip_rpt_name;

  /* Report structure pertaining to ip_rpt_name
  */
  cmipapp_rpt_ptr->ip_ind  = ip_ind;

  /* Copy the data_block value from app
  */
  cmipapp_rpt_ptr->data_block = data_block;

  (void) q_link( cmipapp_rpt_ptr, &cmipapp_rpt_ptr->hdr.link ); /* init link */
  q_put( &cm_rpt_q, &cmipapp_rpt_ptr->hdr.link );        /* and queue it */
  (void) rex_set_sigs( &cm_tcb, CM_RPT_Q_SIG );      /* signal the CM task */

} /* cmipapp_rpt_ind() */


/*===========================================================================

FUNCTION cmipapp_call_id_allocate

DESCRIPTION
  Returns a new CM call id

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CM_CALL_ID_INVALID - invalid call id
  valid call id

SIDE EFFECTS
  None
===========================================================================*/
cm_call_id_type cmipapp_call_id_allocate (

  void
)
{
  return cm_call_id_allocate ();
}


/*===========================================================================

FUNCTION cmipapp_call_id_deallocate

DESCRIPTION
  Deallocates a CM call id

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmipapp_call_id_deallocate (

   cm_call_id_type call_id
     /* Call id */
)
{
  cm_call_id_deallocate (call_id);
  return;
}

/*===========================================================================

FUNCTION cmipapp_inform_srv_system_change

DESCRIPTION
  Apps that have a registered function for being informed of service
  change get notified.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -esym(714, cmipapp_inform_srv_system_change) */  
void cmipapp_inform_srv_system_change (

   const cm_mm_ss_info_s_type *ss_info_ptr
     /* Serving system information */
)
{
   cmipapp_cmd_srv_sys_info_f_type  *sys_info_funcptr = NULL;
     /* Stores app provided call back for reporting service change
     */

   int                               list_loop        = 0;
     /* Loops through client list
     */

   cmipapp_srv_status_s_type         srv_status;
     /* Service status that gets reported to apps
     */

   CM_ASSERT (ss_info_ptr != NULL);
  /*----------------------------------------------------------------------*/

  /* Initialize service status
  */
  srv_status.evdo_srv_status = SYS_SRV_STATUS_NO_SRV;
  srv_status.wlan_srv_status = SYS_SRV_STATUS_NO_SRV;
  srv_status.umts_srv_status = SYS_SRV_STATUS_NO_SRV;


  /* Copy the current HDR service information
  */
  if (ss_info_ptr->hdr_hybrid)
  {
    /* Phone is in hybrid mode, 1x + HDR
    */
    srv_status.evdo_srv_status = ss_info_ptr->hdr_srv_status;
  }
  else
  {
    /* Non hybrid mode
    */
    if (ss_info_ptr->sys_mode == SYS_SYS_MODE_HDR)
    {
      #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
      srv_status.evdo_srv_status = ss_info_ptr->hdr_srv_status;
      #else
      srv_status.evdo_srv_status = ss_info_ptr->srv_status;
      #endif
    }
    
    if (ss_info_ptr->sys_mode == SYS_SYS_MODE_WCDMA)
    {
      srv_status.umts_srv_status = ss_info_ptr->srv_status;
    }
  }


  srv_status.wlan_srv_status = ss_info_ptr->wlan_srv_status;

  /* Loop through app store to call registered serving system info
  ** call backs.
  */

  /* A Lock is required to avoid a situation where an app is trying to
  ** write to cmipapp_arr[index0] and CM is trying to read the value.
  ** A Possible garbage here can lead us to get an invalid function ptr
  ** resulting in a crash.
  ** Lock does not solve sequencing issues where App has deregistered
  ** and CM calls a callback.
  */

  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {

    if (  (cmipapp_arr[list_loop].app_id != CMIPAPP_ID_INVALID)
          &&
          (cmipapp_arr[list_loop].app_cb.cmipapp_cmd_serv_sys_info_ptr)
       )
    {
      /* Get the function ptr
      */
      sys_info_funcptr =
                 cmipapp_arr[list_loop].app_cb.cmipapp_cmd_serv_sys_info_ptr;

      /* Call the app supplied function for service indication
      */
      if (!sys_info_funcptr(&srv_status))
      {
        CM_ERR ("Informing app id %d for srv chg failed",
                 cmipapp_arr[list_loop].app_id, 0, 0);

      }

    }
  }
} /* cmipapp_inform_srv_system_change () */

/*===========================================================================

FUNCTION cmipapp_send_oprt_mode

DESCRIPTION
  Send Phone's operating mode to IP app

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipapp_send_oprt_mode
(
  sys_oprt_mode_e_type  oprt_mode
    /* Operating mode
    */
)
{
  
  int list_loop = 0;
    /* Loop through list of apps 
    */

  cmipapp_inform_oprt_mode_f_type *oprt_mode_funcptr = NULL;
    /* oprt mode cb func ptr
    */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {

    if (  (cmipapp_arr[list_loop].app_id != CMIPAPP_ID_INVALID)
          &&
          (cmipapp_arr[list_loop].app_cb.cmipapp_inform_oprt_mode)
          &&
          (cmipapp_arr[list_loop].oprt_mode_sent != oprt_mode)
       )
    {
      /* Get the function ptr
      */
      oprt_mode_funcptr =
                 cmipapp_arr[list_loop].app_cb.cmipapp_inform_oprt_mode;

      /* Trans id for sending oprt mode
      */
      cmipapp_arr[list_loop].active_trans_id = cmipapp_next_trans_id();

      /* Oprt mode value being sent
      */
      cmipapp_arr[list_loop].oprt_mode_sent  = oprt_mode;

      /* Call the app supplied function for service indication
      */
      if (!oprt_mode_funcptr (oprt_mode, 
                              cmipapp_arr[list_loop].active_trans_id))
      {
        CM_ERR ("Informing app id %d for oprt_mode changed",
                 cmipapp_arr[list_loop].app_id, 0, 0);

      }      
      else
      {
        CM_MSG_HIGH ("Oprt_mode callback successfull for app %d", 
                      cmipapp_arr[list_loop].app_id , 0, 0);
        
      }

    }
  }  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* cmipapp_send_oprt_mode () */


/*===========================================================================

FUNCTION cmipapp_is_wait_on_oprt_mode_cnf

DESCRIPTION
  Checks if ipapp would need to deregister on this request and
  also if inform_oprt_mode callback is registered.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cmipapp_is_wait_on_oprt_mode_cnf (

  sys_oprt_mode_e_type  oprt_mode
    /* Operating mode
    */
)
{

  int list_loop = 0;
    /* Loop through list of apps 
    */  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* IP app perform deregistration only 
  ** on LPM and PWR_OFF
  */
  if ((oprt_mode != SYS_OPRT_MODE_PWROFF)  &&
      (oprt_mode != SYS_OPRT_MODE_LPM)
     )
  {  
    return FALSE;
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For PWR_OFF and LPM wait till apps send 
  ** CM back a confirmation for the oprt_mode 
  ** request sent
  */
  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {

    if (  (cmipapp_arr[list_loop].app_id != CMIPAPP_ID_INVALID)
          &&
          (cmipapp_arr[list_loop].app_cb.cmipapp_inform_oprt_mode)
          &&
          (cmipapp_arr[list_loop].oprt_mode_sent == oprt_mode)
          &&
          (cmipapp_arr[list_loop].active_trans_id != CMIPAPP_TRANS_ID_NONE)
       )
    {

      CM_MSG_HIGH ("App %d has cb registered for oprt_mode",
                   cmipapp_arr[list_loop].app_id, 0, 0 );
      
      return TRUE;

    }
  }  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  /* No apps registered for
  ** deregistration
  */
  return FALSE;

} /* cmipapp_is_wait_on_oprt_mode_cnf () */


/*===========================================================================

FUNCTION cmipapp_oprt_mode_cnf

DESCRIPTION
  Api called by app to confirm processing
  of operating mode request.

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void cmipapp_oprt_mode_cnf (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_trans_id_type                trans_id
    /* Transaction id
    */
)
{   

  int                               list_loop        = 0;
    /* Loops through client list
    */

  cm_sd_rpt_u_type                 *rpt_cmd_ptr = NULL;
    /* Report being sent to CM
    */

  sys_oprt_mode_e_type          oprt_mode   = SYS_OPRT_MODE_NONE;
    /* oprt_mode that was last sent to app
    */
  /*----------------------------------------------------------------------*/

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)))     
  {
    CM_ERR ("Incorrect app id %d", app_id, 0, 0);
    return;
  }

  /*----------------------------------------------------------------------*/

  CM_MSG_HIGH ("App %d confirming oprt mode", app_id, 0, 0);
  
  if (cmipapp_arr[app_id].active_trans_id != trans_id) 
  {    
    CM_ERR ("Not expecting oprt_mode_cnf from app %d", app_id, 0, 0);
    return;
  }

  /*----------------------------------------------------------------------*/

  /* Operating mode being confirmed by the apps
  */
  oprt_mode   = cmipapp_arr[app_id].oprt_mode_sent;

  cmipapp_arr[app_id].active_trans_id = CMIPAPP_TRANS_ID_NONE;
                                    
  /*----------------------------------------------------------------------*/

  /* 
  ** If there is an app registered with oprt_mode func cb, 
  ** return if it has not been informed of the 
  ** operating mode or if the transaction id is not NONE.
  **
  ** oprt_mode_sent is also checked to avoid ALLAPS_OPRT_CNF
  ** from being sent when just one app has been informed of
  ** oprt_mode and it immed returns back with conf (Depends
  ** on App's priority).
  */
  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {

    if (  (cmipapp_arr[list_loop].app_id != CMIPAPP_ID_INVALID)
          &&
          (cmipapp_arr[list_loop].app_cb.cmipapp_inform_oprt_mode)        
          &&
          (
            (cmipapp_arr[list_loop].oprt_mode_sent != oprt_mode)
            ||
            (cmipapp_arr[list_loop].active_trans_id != CMIPAPP_TRANS_ID_NONE)
          )
          
       )
    {
      return;
    }
    
  }  
  

  /*----------------------------------------------------------------------*/

  /* Send report when all ipapps
  ** have confirmed operating mode
  ** If all the apps have confirmed a particular operating
  ** mode then it is time to send CM_IP_APP_ALLAPPS_OPRT_CNF
  ** to CM.
  */ 
  if ((rpt_cmd_ptr = cm_sd_rpt_get_buf_else_err_fatal()) != NULL)
  {
    /* Fill in command type */
    rpt_cmd_ptr->hdr.cmd                       = CM_IP_APP_ALLAPPS_OPRT_CNF;


    CM_MSG_HIGH ("Sending CM_IP_APP_ALLAPPS_OPRT_CNF", 0, 0, 0);

    /* Put message in destination task queue */
    cm_sd_rpt (rpt_cmd_ptr);
  }  
  
} /* cmipapp_oprt_mode_cnf () */


#else /* FEATURE_IP_CALL */

/*===========================================================================
FUNCTION cmipapp_dummy

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
void cmipapp_dummy( void )
/*lint -esym(714,cmipapp_dummy) */
/*lint -esym(765,cmipapp_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}


#endif /* FEATURE_IP_CALL */

