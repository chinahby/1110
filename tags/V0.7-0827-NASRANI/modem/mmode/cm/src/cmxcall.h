#ifndef CMXCALL_H
#define CMXCALL_H
/*===========================================================================

    C A L L   M A N A G E R   C D M A   C A L L   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMXCALL.C


Copyright (c) 2002 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmxcall.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/08   sv      Removing unneccesary snd.h
06/11/07   pk      VoIP -> 1x hho enahncements / CR fixes
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
07/14/06   ka      Fixing the issue where call cmd cb for call end api was
                   being called more than once.
12/03/05   ic      Lint clean up 
11/08/05   sk      Separated CM reports and commands.
03/24/05   dk      Externalize cmxcall_send_end_cmd.
02/01/05   dk      Added WLAN Support.
06/25/04   dk      Moved certain generic call obj array walk through functions
                   from cmxcall.c.
06/14/04   ic      Renamed cmxcall_is_there_a_hdrdata_call() to
                   cmxcall_is_hdr_call()
                   Modified cmxcall_is_hybr_hdr_call() to also return call id
                   of the HDR call (if any).
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
10/01/03   ws      Merged from GW Targets.
07/29/03   ws      Added Automatic Silent ReDial.
01/21/03   ws      Updated copyright information for 2003
05/05/02   vt      Fixed compile problem with new mm cm.h
04/28/02   vt      Initial version.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "cm.h"                              /* External interface to cm.c */
#include "cmi.h"                             /* Internal interface to cm.c */

#include "cmcall.h"


/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

#ifdef FEATURE_HDR_HYBRID
#error code not present
#endif /* FEATURE_HDR_HYBRID */


#ifdef FEATURE_DDTM_CNTL
#endif /*  FEATURE_DDTM_CNTL */



/*===========================================================================

FUNCTION cmxcall_send_orig

DESCRIPTION
  Send an origination command to MC.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_send_orig(

    cmcall_s_type    *call_ptr
        /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmxcall_reset

DESCRIPTION
  Reset call fields to default values reflecting a call in idle state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_reset
(
  cmcall_s_type  *call_ptr
    /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmxcall_info_get

DESCRIPTION
  Copy the current call state information into a specified buffer.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmxcall_info_get
(
  const cmcall_s_type          *call_ptr,
    /* Point at call object */

  cm_mm_call_info_s_type    *call_info_ptr
    /* Copy call state info into this buffer */
);



/*===========================================================================

FUNCTION cmxcall_end

DESCRIPTION
  Does call end necessary activities.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_end
(
  cmcall_s_type  *call_ptr
    /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmxcall_timer_proc

DESCRIPTION
  Process timer events

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_timer_proc
(
  cmcall_s_type           *call_ptr
      /* Point at call object */
);



/*===========================================================================

FUNCTION CMXCALL_CLIENT_CMD_PROC

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
void cmxcall_client_cmd_proc
(
  cm_cmd_s_type  *cmd_ptr                       /* pointer to a CM command */
);


/*===========================================================================

FUNCTION CMXCALL_CLIENT_ORIG_PROC

DESCRIPTION
  Process clients' origination call command.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type cmxcall_client_orig_proc
(
  cmcall_s_type  *call_ptr

);


/*===========================================================================

FUNCTION cmxcall_mc_rpt_proc

DESCRIPTION
  Process MC reports


DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS^
  none

===========================================================================*/
extern void cmxcall_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);


#ifdef FEATURE_HDR_HANDOFF
#error code not present
#endif // FEATURE_HDR_HANDOFF

#ifdef FEATURE_HDR_HYBRID
#error code not present
#endif // FEATURE_HDR_HYBRID


#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

/*===========================================================================

FUNCTION  cmxcall_call_cmd_orig_para_check

DESCRIPTION
  Check parameter errors for call origination command

DEPENDENCIES
  none
RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type  cmxcall_call_cmd_orig_para_check
(
  const cm_call_cmd_s_type       *call_cmd_ptr
);

/*===========================================================================

FUNCTION cmxcall_cmd_copy_fields

DESCRIPTION
  Copy the active CDMA/HDR fields of a call command into a call object.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_cmd_copy_fields(

    cmcall_s_type               *call_ptr,
        /* Pointer to a call object */

    const cm_call_cmd_s_type    *cmd_ptr
        /* Pointer to a call command */

);

#ifdef FEATURE_OTASP_OTAPA
#ifdef FEATURE_OTASP_HOLD_CMD_IN_COMMIT

/*===========================================================================

FUNCTION cmxcall_is_conv_pc_otapa_commit

DESCRIPTION
  Finds if any paging channel otapa call is in the process of commit.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - call found; FALSE - Not.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmxcall_is_conv_pc_otapa_commit
(
  void
);
#endif
#endif

/*===========================================================================

FUNCTION cmxcall_end_call

DESCRIPTION
  Ends the specified call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_end_call
(
  cm_call_id_type      call_id,
  cm_call_end_e_type   end_status,
  byte                 end_ll_reason
);

/*===========================================================================

FUNCTION cmxcall_send_end_cmd

DESCRIPTION
  Send an end command to either MC or HDRMC task

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_send_end_cmd
(
  cmcall_s_type    *call_ptr,
     /* pointer to a call object */

  byte                   end_reason
     /* call end reason */
);
#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif

/*===========================================================================

FUNCTION  cmxcall_client_cmd_check

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
  If originating dial-string matches one of the special numbers that is
  stored in the phone-book, this function changes the call type to E911,
  OTASP, etc.

===========================================================================*/
extern cm_call_cmd_err_e_type  cmxcall_client_cmd_check(

    cm_cmd_s_type          *cmd_ptr
        /* Pointer to a CM command */
);

#endif /* CMXCALL_H */

