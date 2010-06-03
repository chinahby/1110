/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   I P   Q  V I D E O  P H O N E

                               I N T E R F A C E   M O D U L E

GENERAL DESCRIPTION
  This module registers qvp as an IP application to Call Manager.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmipqvp.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
10/08/05   ic      Added Header: field 
04/06/05   ka      Adding system mode to call origination
03/28/05   ka      Fixed Lint errors
03/14/05   ka      Changing signature of invite response
02/02/05   dk/ka   Initial release
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*lint -esym(766,customer.h) */
#include "customer.h"  /* Customer configuration file */

#if defined(FEATURE_IP_CALL)

#include "cmipapp.h"   /* CM IP interface */
#include "cmdbg.h"     /* Interface to CM debug services */
/* #include "cmipqvp.h" */ /* Empty at this point, commented to satisfy lint */


/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                              REG INTERFACE FUNCTIONS
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION cmipqvp_call_cmd_orig

DESCRIPTION
  Calls qvp function to make a call

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmipqvp_call_cmd_orig(

  const cm_num_s_type    called_num,
    /* Number dialed - phone that is being called
    */

  const cm_call_id_type   call_id,
    /* call ID of the call
    */

  cmipapp_call_type_e_type call_type,
    /* call type for the call
    */

  cmipapp_sys_mode_e_type  sys_mode,
    /* mode call needs to be originated on
    */

  uint32                  data_block
    /* Token created and used by CM.
    ** data_block is passed by CM, when calling app registered
    ** callbacks. Client needs to pass the most recent value
    ** given by CM, all the time when cmipapp_rpt_ind () is called.
    */
)
/*lint -esym(715, called_num)*/
/*lint -esym(715, call_id)*/
/*lint -esym(715, call_type)*/
/*lint -esym(715, data_block)*/
/*lint -esym(715, sys_mode)*/
{
  /* Call qvp function to originate a call */
  return TRUE;
} /* cmipqvp_call_cmd_orig */
/*lint +esym(715, called_num)*/
/*lint +esym(715, call_id)*/
/*lint +esym(715, call_type)*/
/*lint +esym(715, data_block)*/
/*lint +esym(715, sys_mode)*/



/*===========================================================================

FUNCTION cmipqvp_call_cmd_answer

DESCRIPTION
  Calls qvp function to answer a call

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmipqvp_call_cmd_answer(

  const cm_call_id_type            call_id,
  /* call id of incoming call
  */

  boolean                          accept,
  /* Flag indicating whether the call is accepted or not
  */

  cmipapp_incom_rej_cause_e_type   answer_cause,
  /* Valid when accept is set to FALSE
  */

  uint32                           data_block
  /* Token created and used by CM.
  ** data_block is passed by CM, when calling app registered
  ** callbacks. Client needs to pass the most recent value
  ** given by CM, all the time when cmipapp_rpt_ind () is called.
  */

)
/*lint -esym(715, accept)*/
/*lint -esym(715, call_id)*/
/*lint -esym(715, answer_cause)*/
/*lint -esym(715, data_block)*/
{
  /* Call qvp function to answer a call */
  return TRUE;
} /* cmipqvp_call_cmd_answer */
/*lint +esym(715, accept)*/
/*lint +esym(715, call_id)*/
/*lint +esym(715, answer_cause)*/
/*lint +esym(715, data_block)*/



/*===========================================================================

FUNCTION cmipqvp_call_cmd_invite_res

DESCRIPTION
  Call object must have already been initialized with
  cmcall_init().

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmipqvp_call_cmd_invite_res(

  cm_call_id_type                call_id,
    /* call id
    */

  boolean                        accept,
    /* Flag indicating whether the call is accepted or not
    */

  cmipapp_incom_rej_cause_e_type answer_cause,
    /* Valid when accept is set to FALSE
    */


  uint32                         data_block
    /* Token created and used by CM.
    ** data_block is passed by CM, when calling app registered
    ** callbacks. Client needs to pass the most recent value
    ** given by CM, all the time when cmipapp_rpt_ind () is called.
    */

)
/*lint -esym(715, accept)*/
/*lint -esym(715, call_id)*/
/*lint -esym(715, answer_cause)*/
/*lint -esym(715, data_block)*/
{
  /* Call qvp function to respond to an incoming call */
  return TRUE;
} /* cmipqvp_call_cmd_invite_res */
/*lint +esym(715, accept)*/
/*lint +esym(715, call_id)*/
/*lint +esym(715, answer_cause)*/
/*lint +esym(715, data_block)*/



/*===========================================================================

FUNCTION cmipqvp_call_cmd_end

DESCRIPTION
  Calls qvp function to end a call

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmipqvp_call_cmd_end(

  cm_call_id_type call_id,
    /* call id
    */

  uint32          data_block
    /* Token created and used by CM.
    ** data_block is passed by CM, when calling app registered
    ** callbacks. Client needs to pass the most recent value
    ** given by CM, all the time when cmipapp_rpt_ind () is called.
    */

)
/*lint -esym(715, call_id)*/
/*lint -esym(715, data_block)*/
{
  /* Call qvp function to end a call */
  return TRUE;
} /* cmipqvp_call_cmd_end */
/*lint +esym(715, call_id)*/
/*lint +esym(715, data_block)*/



/*===========================================================================

FUNCTION cmipqvp_inband_cmd_start_cont_dtmf

DESCRIPTION
  Calls qvp function to start continuous dtmf

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmipqvp_inband_cmd_start_cont_dtmf(

  cm_call_id_type call_id,
    /* call id
    */


  uint8           digit,
    /* DTMF digit
    */

  uint32          data_block
    /* Token created and used by CM.
    ** data_block is passed by CM, when calling app registered
    ** callbacks. Client needs to pass the most recent value
    ** given by CM, all the time when cmipapp_rpt_ind () is called.
    */

)
/*lint -esym(715, call_id)*/
/*lint -esym(715, digit)*/
/*lint -esym(715, data_block)*/
{
  /* Call qvp function to start continuous DTMF */
  return TRUE;
} /* cmipqvp_inband_cmd_start_cont_dtmf */
/*lint +esym(715, call_id)*/
/*lint +esym(715, digit)*/
/*lint +esym(715, data_block)*/



/*===========================================================================

FUNCTION cmipqvp_inband_cmd_stop_cont_dtmf

DESCRIPTION
  Calls qvp function to stop continuous dtmf

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmipqvp_inband_cmd_stop_cont_dtmf(

  cm_call_id_type call_id,
    /* call id of the call
    */

  uint16          duration,
    /* DTMF duration
    */

  uint32          data_block
    /* Token created and used by CM.
    ** data_block is passed by CM, when calling app registered
    ** callbacks. Client needs to pass the most recent value
    ** given by CM, all the time when cmipapp_rpt_ind () is called.
    */

)
/*lint -esym(715, call_id)*/
/*lint -esym(715, duration)*/
/*lint -esym(715, data_block)*/
{
  /* Call qvp function to stop continuous DTMF */
  return TRUE;
} /* cmipqvp_inband_cmd_stop_cont_dtmf */
/*lint +esym(715, call_id)*/
/*lint +esym(715, duration)*/
/*lint +esym(715, data_block)*/



/*===========================================================================

FUNCTION cmipqvp_call_cmd_hold

DESCRIPTION
  Calls qvp function to put call on hold

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmipqvp_call_cmd_hold(

  cm_call_id_type call_id,
    /* call id
    */

  uint32          data_block
    /* Token created and used by CM.
    ** data_block is passed by CM, when calling app registered
    ** callbacks. Client needs to pass the most recent value
    ** given by CM, all the time when cmipapp_rpt_ind () is called.
    */

)
/*lint -esym(715, call_id)*/
/*lint -esym(715, data_block)*/
{
  /* Call qvp function to put call on hold */
  return TRUE;
} /* cmipqvp_call_cmd_hold */
/*lint +esym(715, call_id)*/
/*lint +esym(715, data_block)*/



/*===========================================================================

FUNCTION cmipqvp_call_cmd_active

DESCRIPTION
  Calls qvp function to make a held call active

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmipqvp_call_cmd_active(

  cm_call_id_type call_id,
    /* call id
    */

  uint32          data_block
    /* Token created and used by CM.
    ** data_block is passed by CM, when calling app registered
    ** callbacks. Client needs to pass the most recent value
    ** given by CM, all the time when cmipapp_rpt_ind () is called.
    */
)
/*lint -esym(715, call_id)*/
/*lint -esym(715, data_block)*/
{
  /* Call qvp function to make held call active */
  return TRUE;
} /* cmipqvp_call_cmd_active */
/*lint +esym(715, call_id)*/
/*lint +esym(715, data_block)*/



/*===========================================================================

FUNCTION cmipqvp_call_cmd_modify

DESCRIPTION
  Calls qvp function to modify call type

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmipqvp_call_cmd_modify(

  cm_call_id_type              call_id,
    /* call id
    */

  cmipapp_call_type_e_type     cmipapp_type,
    /* Call type to modify the call into
    */

  uint32                          data_block
    /* Token created and used by CM.
    ** data_block is passed by CM, when calling app registered
    ** callbacks. Client needs to pass the most recent value
    ** given by CM, all the time when cmipapp_rpt_ind () is called.
    */
)
/*lint -esym(715, call_id)*/
/*lint -esym(715, cmipapp_type)*/
/*lint -esym(715, data_block)*/
{
  /* Call qvp function to modify call type for the call  */
  return TRUE;
} /* cmipqvp_call_cmd_modify () */
/*lint +esym(715, call_id)*/
/*lint +esym(715, cmipapp_type)*/
/*lint +esym(715, data_block)*/



/*===========================================================================

FUNCTION cmipqvp_init

DESCRIPTION
  Attaches qvp to CM IP module as an app.

DEPENDENCIES
  none


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmipqvp_init( void )
/*lint -esym(528, cmipqvp_init) */
/*lint -esym(765, cmipqvp_init) */
/*lint -esym(714, cmipqvp_init)
** Function written as an example, not currently used, needs to stay (karthika)
** Can't be static as then we get "..\..\services\cm\cmipqvp.c", line 608: 
** Warning: C2871W: static 'cmipqvp_init' declared but not used
*/
{
  cmipapp_id_type              qvp_id        = CMIPAPP_ID_INVALID;

  cmipapp_call_type_e_type     qvp_call_type = CMIPAPP_CALL_TYPE_VOICE;

  cmipapp_sys_mode_e_type      qvp_sys_mode  = CMIPAPP_SYS_MODE_GSM;

  cmipapp_cb_s_type            qvp_cb;

  /* First get an app id
  */
  if (cmipapp_id_allocate (qvp_call_type, qvp_sys_mode, &qvp_id) !=
                                                    CMIPAPP_STATUS_OK)
  {
    CM_ERR ("qvp app id allocation Failed", 0, 0, 0);

    return;
  }

  /* Initialize callback ptrs required by CM from app
  */
  cmipapp_cb_init (&qvp_cb);

  /* Assign qvp functions to callback function pointers
  */

  /*  call origination function */
  qvp_cb.cmipapp_cmd_orig_func_ptr              =
                                               cmipqvp_call_cmd_orig;

  /*  call answering function */
  qvp_cb.cmipapp_cmd_answer_func_ptr            =
                                               cmipqvp_call_cmd_answer;

  /*  incoming call response function */
  qvp_cb.cmipapp_cmd_invite_res_func_ptr        =
                                            cmipqvp_call_cmd_invite_res;

  /*  call end function */
  qvp_cb.cmipapp_cmd_end_func_ptr               =
                                               cmipqvp_call_cmd_end;

  /*  start continuous dtmf function */
  qvp_cb.cmipapp_inband_cmd_start_cont_dtmf_func_ptr =
                                   cmipqvp_inband_cmd_start_cont_dtmf;

  /*  stop continuous dtmf function */
  qvp_cb.cmipapp_inband_cmd_stop_cont_dtmf_func_ptr  =
                                   cmipqvp_inband_cmd_stop_cont_dtmf;

  /* To put call on hold */
  qvp_cb.cmipapp_cmd_hold_func_ptr              =
                                               cmipqvp_call_cmd_hold;

  /* To resume a held call */
  qvp_cb.cmipapp_cmd_active_func_ptr            =
                                               cmipqvp_call_cmd_active;

  /* To modify call type */
  qvp_cb.cmipapp_cmd_modify_func_ptr            =
                                               cmipqvp_call_cmd_modify;

  /* Register call back functions with CM
  */
  if (cmipapp_reg (&qvp_cb, qvp_id) != CMIPAPP_STATUS_OK)
  {
    CM_ERR ("QVP Callback reg failed", 0, 0, 0);
    return;
  }

  /* Inform active app status to CM
  */
  if (cmipapp_inform_status (qvp_id, CMIPAPP_RUN_STATUS_ACTIVE) !=
                                                        CMIPAPP_STATUS_OK)
  {
    CM_ERR ("QVP inform status failed", 0, 0, 0);
    return;
  }

} /* cmipqvp_init () */

#else /* FEATURE_IP_CALL */

/*===========================================================================
FUNCTION cmipqvp_dummy

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
void cmipqvp_dummy( void )
/*lint -esym(714,cmipqvp_dummy) */
/*lint -esym(765,cmipqvp_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}


#endif /* FEATURE_IP_CALL */

