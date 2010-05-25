#ifndef gstk_envelope_cmd_h
#define gstk_envelope_cmd_h
/*===========================================================================


            G S T K   E N V E L O P E   C M D

GENERAL DESCRIPTION
  This header file contains functions to handle envelope commands 


FUNCTIONS
  gstk_process_envelope_cmd
    This function switches on the command id and call the corresponding
    envelope command ind function so that GSTK can build the corrresponding
    envelope command tlvs and send it to UIM Server

  gstk_cm_call_event_cb
    Call event callback function, called when a call event is happening.
    This function populate the various call related indication type and
    put the command to the GSTK command queue.

  gstk_mt_call_evt_ind
    This function builds the mt call command and sends it to UIM

  gstk_call_connected_evt_ind
    This function builds the call connected envelope command and sends it 
    to UIM

  gstk_user_act_evt_ind
   This function builds the user activity command and sends it to UIM

  gstk_location_status_evt_ind
    This function builds the location status event download envelope command 
    and sends it to UIM

  gstk_timer_expire_ind
    This function builds the timer expiration command and sends it to UIM

  gstk_lang_selection_evt_ind
    This function builds the language selection event download envelope command
    and sends it to UIM

  gstk_browser_term_evt_ind
    This function builds the browser termination event download envelope command
    and sends it to UIM

  gstk_build_postpone_envelope
    This function builds the envelope that has been postponed due to
    terminal response that is required to be sent down first


INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2006 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_envelope_cmd.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/07   sk      Fixed lint error
07/14/07   sk      Added support to requeue sms pp envelope if there is a 
                   pending send sms tr
07/25/06   sk      Added support to handle envelope cmds in non idle state
06/06/06   sk      Added support for network search mod
06/02/06   tml     lint
05/03/06   tml     lint
04/04/06   sk      Added new function to build location status envelope 
09/15/05   tml     Fixed link error: move gstk_build_error_envelope_report
                   to gstk_envelope_rsp.h
07/13/05   sk      Added support for Access Technology.
                   Added function gstk_build_error_envelope_report()
11/23/04  sk/tml   Added SAT/USAT data support
05/11/04   tml     Timer management support
05/16/03   tml     linted
02/07/03   tml     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk.h"

#include "cm.h"


/*===========================================================================
FUNCTION   gstk_cm_call_event_cb

DESCRIPTION
  Call event callback function, called when a call event is happening.
  This function populate the various call related indication type and
  put the command to the GSTK command queue.

PARAMETERS
  call_event: [Input] cm call event 
  call_info_ptr: [Input] Pointer to the call info struct

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_cm_call_event_cb (
  cm_call_event_e_type         call_event,
  const cm_mm_call_info_s_type *call_info_ptr
);



/*===========================================================================
FUNCTION gstk_process_envelope_cmd

DESCRIPTION

  This function switches on the command id and call the corresponding
  envelope command ind function so that GSTK can build the corrresponding
  envelope command tlvs and send it to UIM Server

PARAMETERS
  cmd: [Input] Pointer to the envelope command types to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_process_envelope_cmd(
    gstk_cmd_type* cmd );


/*===========================================================================
FUNCTION gstk_requeue_envelope_command

DESCRIPTION

  This function requeues the envelope command to the back of the gstk queue

PARAMETERS
  cmd_ptr: [Input] Pointer to the envelope command types to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_requeue_envelope_command(
  const gstk_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION gstk_mt_call_evt_ind

DESCRIPTION

  This function builds the mt call command and sends it to UIM

PARAMETERS
  mt_evt_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_mt_call_evt_ind(
        gstk_envelope_mt_evt_ind_type    * mt_evt_cmd);

/*===========================================================================
FUNCTION gstk_call_connected_evt_ind

DESCRIPTION

  This function builds the call connected envelope command and sends it to UIM

PARAMETERS
  call_connected_evt_cmd: [Input] Pointer to the envelope command to be 
                                  processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_call_connected_evt_ind(
        gstk_envelope_call_connected_evt_ind_type    * call_connected_evt_cmd);




/*===========================================================================
FUNCTION gstk_call_disconnected_evt_ind

DESCRIPTION

  This function builds the call disconnected envelope command and sends it 
  to UIM

PARAMETERS
  call_connected_evt_cmd: [Input] Pointer to the envelope command to be 
                                  processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_call_disconnected_evt_ind(
        gstk_envelope_call_disconnected_evt_ind_type    * call_disconnected_evt_cmd);

/*===========================================================================
FUNCTION gstk_idle_scrn_evt_ind

DESCRIPTION

  This function builds the idle screen command and sends it to UIM

PARAMETERS
  idle_scrn_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_idle_scrn_evt_ind(
  const gstk_envelope_idle_scrn_evt_ind_type * idle_scrn_cmd);

 
/*===========================================================================
FUNCTION gstk_user_act_evt_ind

DESCRIPTION

  This function builds the user activity command and sends it to UIM

PARAMETERS
  user_act_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_user_act_evt_ind(
  const gstk_envelope_user_act_evt_ind_type * user_act_cmd);




/*===========================================================================
FUNCTION gstk_location_status_evt_ind

DESCRIPTION

  This function builds the location status event download envelope command 
  and sends it to UIM

PARAMETERS
  locaiton_status_evt_cmd: [Input] Pointer to the envelope command to be 
                                  processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_location_status_evt_ind(
  const gstk_envelope_location_statue_ind_type    * location_status_evt_cmd);

/*===========================================================================
FUNCTION gstk_lang_selection_evt_ind

DESCRIPTION

  This function builds the language selection event download envelope command
  and sends it to UIM

PARAMETERS
  lang_select_evt_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_lang_selection_evt_ind(
  const gstk_envelope_lang_sel_evt_ind_type    * lang_select_evt_cmd);

/*===========================================================================
FUNCTION gstk_browser_term_evt_ind

DESCRIPTION

  This function builds the language selection event download envelope command
  and sends it to UIM

PARAMETERS
  lang_select_evt_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_browser_term_evt_ind(
  const gstk_envelope_browser_term_evt_ind_type    * browser_term_evt_cmd);

/*===========================================================================
FUNCTION gstk_data_avail_evt_ind 

DESCRIPTION

  This function builds the data available event download envelope command
  and sends it to UIM

PARAMETERS
  data_avail_evt_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_data_avail_evt_ind(
  const gstk_envelope_data_avail_evt_ind_type *data_avail_evt_cmd
);

/*===========================================================================
FUNCTION gstk_ch_status_evt_ind 

DESCRIPTION

  This function builds the channel status event download envelope command
  and sends it to UIM

PARAMETERS
  data_avail_evt_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_ch_status_evt_ind(
  const gstk_envelope_ch_status_evt_ind_type *ch_status_evt_cmd
);

/*===========================================================================
FUNCTION   gstk_build_postpone_envelope

DESCRIPTION
  This function builds the envelope that has been postponed due to
    terminal response that is required to be sent down first

PARAMETERS
  env_cmd_index: [Input] Index into the envelope command reference table

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_build_postpone_envelope ( uint8  env_cmd_index );


/*===========================================================================
FUNCTION gstk_access_tech_evt_ind 

DESCRIPTION

  This function builds the language selection event download envelope command
  and sends it to UIM

PARAMETERS
  lang_select_evt_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_access_tech_evt_ind(
  const gstk_envelope_access_tech_ind_type    *access_tech_evt_cmd);

/*===========================================================================
FUNCTION   gstk_build_access_technology_envelope

DESCRIPTION
  This function builds the envelope that has been postponed due to
    terminal response that is required to be sent down first

PARAMETERS
  env_cmd_index: [Input] Index into the envelope command reference table

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_build_access_technology_envelope(
	gstk_access_technology_type access_tech
);
#ifdef FEATURE_CAT_REL6
/*===========================================================================
FUNCTION   gstk_build_location_status_envelope

DESCRIPTION
  This function builds the envelope for location status event.

PARAMETERS
  access tech     :- Type of access technology used
  location status :- GSTK_NORMAL_SERVICE
                     GSTK_NO_SERVICE
                     GSTK_LIMITED_SERVICE

DEPENDENCIES
 None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_build_location_status_envelope(
    gstk_access_technology_type    access_tech,
    gstk_location_status_enum_type location_status
);
/*===========================================================================
FUNCTION   gstk_build_nw_search_mode_change_envelope

DESCRIPTION
  This function builds the envelope for network search mode event.

PARAMETERS
  ph_nw_sel_pref :- Preferred network selection mode used.

DEPENDENCIES
 None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_build_nw_search_mode_change_envelope(
    gstk_nw_search_mode_enum_type  ph_nw_sel_pref
);
#endif /* FEATURE_CAT_REL6 */

/*===========================================================================
FUNCTION gstk_process_non_idle_state_envelope_cmd

DESCRIPTION
  This function switches on the command id and call the corresponding
  envelope command ind function so that GSTK can build the corrresponding
  envelope response to the caller

PARAMETERS
  cmd: [Input] Pointer to the envelope command types to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_process_non_idle_state_envelope_cmd(
  const gstk_cmd_type* cmd );

gstk_status_enum_type gstk_check_envelope_offset(uim_cmd_type *cmd_ptr, 
                                                       int offset, uint32 length);

#endif /*gstk_envelope_cmd_h*/

