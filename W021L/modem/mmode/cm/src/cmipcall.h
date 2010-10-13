#ifndef CMIPCALL_H
#define CMIPCALL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   I P  C A L L

                      M O D U L E

GENERAL DESCRIPTION

  This module implements the functionality of handling calls through
  IP. IP call reports are received and call object is updated. Call
  Origination, end functions are implemented to call registered
  applications that work using IP.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmipcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2005 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmipcall.h#2 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/18/09   mp      Added support to remove the last party added for VOIP calls
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
10/03/06   ka      Adding support for IP call on WCDMA.
07/14/06   ka      Fixing the issue where call cmd cb for call end api was
                   being called more than once.
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
05/07/06   ka      Correcting signature of stop cont dtmf.
01/06/06   ic      Lint cleanup  
11/23/05   ka      Modification to allow hold/active for ip calls.
11/08/05   sk      Separated CM reports and commands.
10/08/05   ic      Added Header: field 
03/14/05   ka      cleaning up IP rpt and adding handling call cmd for IP
                   in forward_ll, reply_ll and cmd_check functions.
02/02/05   dk/ka   Initial release
===========================================================================*/

#if defined(FEATURE_IP_CALL)

/*===========================================================================

FUNCTION cmipcall_client_cmd_proc

DESCRIPTION
  Process clients' call commands

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmipcall_client_cmd_proc(

    cm_cmd_s_type  *cmd_ptr
        /* pointer to a CM command */
);


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
extern void cmipcall_send_orig
(
  cmcall_s_type           *call_ptr
    /* Call object for the originating call
    */
);


/*===========================================================================

FUNCTION cmipcall_invite_res

DESCRIPTION
  Sends client response to invite request originating from App.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipcall_invite_res
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
);


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
extern void cmipcall_answer
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
);


/*===========================================================================

FUNCTION cmipcall_simple_flash

DESCRIPTION
  Sends user response to an call waiting indication or request to remove
  the last party added to the conference call

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_simple_flash
(
  const cmcall_s_type                   *call_ptr
    /* call object
    */

);

/*===========================================================================

FUNCTION cmipcall_send_end

DESCRIPTION
  Sends a call end request to the respective app

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipcall_send_end
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
);


/*===========================================================================

FUNCTION cmipcall_end

DESCRIPTION
  Call object is made free and phone is informed if phone not in use

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Call object is deallocated.

===========================================================================*/
extern void cmipcall_end
(
  cmcall_s_type           *call_ptr
    /* call object
    */
);


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
extern void cmipcall_hold
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
);


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
extern void cmipcall_active
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
);

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
extern void cmipcall_start_cont_dtmf
(
  const cmcall_s_type           *call_ptr,
    /* call object
    */

  uint8                    digit
    /* DTMF digit
    */
);

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
extern void cmipcall_stop_cont_dtmf
(
  const cmcall_s_type     *call_ptr,
    /* call object
    */

  uint16                    duration
    /* DTMF digit duration
    */
);


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
);

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
);

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
extern cm_call_cmd_err_e_type  cmipcall_client_cmd_check(

    cm_cmd_s_type          *cmd_ptr
        /* Pointer to a CM command */
);

#endif /* defined(FEATURE_IP_CALL) */

#endif // CMIPCALL_H
