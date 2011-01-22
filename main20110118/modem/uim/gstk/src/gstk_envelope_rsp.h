#ifndef gstk_envelope_rsp_h
#define gstk_envelope_rsp_h
/*===========================================================================


            G S T K   E N V E L O P E   R S P

GENERAL DESCRIPTION
  This header file contains functions to handle envelope response from the
  card


FUNCTIONS
  gstk_uim_envelope_report
    This is the callback function for envelope response.  

  gstk_process_envelope_rsp
    This function performs a lookup on the command id with the 
    envelope_cmd_ref_table and based on the envelope rsp type, it calls the 
    corresponding envelope rsp function so that GSTK can build the corrresponding
    envelope response and sends it to the client


INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_envelope_rsp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/08   xz      Add support for raw envelop response
05/06/08   sk      Changed prototype for gstk_build_error_envelope_report() 
02/23/08   yb      Added support for parsing unexpected and unknown tlvs for
                   envelope call control response
07/25/06   sk      Added support to handle envelope cmds in non idle state
09/15/05   tml     Fixed link error: move gstk_build_error_envelope_report
                   to gstk_envelope_rsp.h
05/16/03   tml     Linted
02/07/03   tml     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk.h"
#include "uim.h"

#define GSTK_TAG_LEVEL_1 0x01
#define GSTK_TAG_LEVEL_2 0x02
#define GSTK_TAG_LEVEL_3 0x03
#define GSTK_TAG_LEVEL_4 0x04
#define GSTK_TAG_LEVEL_5 0x05
#define GSTK_TAG_LEVEL_6 0x06

/*===========================================================================
FUNCTION gstk_uim_envelope_report

DESCRIPTION

  This is the callback function for envelope response.  UIM will call
  this function when it receives envelope response from the Card
  Based on the return status of the envelope response, this function
  will build the corresponding envelope resp type and put it to the
  gstk command queue

PARAMETERS
  report: [Input] Pointer to the UIM envelope report

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_uim_envelope_report (uim_rpt_type *report);

/*===========================================================================
FUNCTION gstk_process_envelope_rsp

DESCRIPTION

  This function performs a lookup on the command id with the 
  envelope_cmd_ref_table and based on the envelope rsp type, it calls the 
  corresponding envelope rsp function so that GSTK can build the corrresponding
  envelope response and sends it to the client

PARAMETERS
  cmd: [Input] Pointer to the envelope response types to be processed

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
extern gstk_status_enum_type gstk_process_envelope_rsp(gstk_cmd_type* cmd);

/*===========================================================================
FUNCTION gstk_build_error_envelope_report

DESCRIPTION

  This function builds the error envelope response to be sent back 
  to the client.

PARAMETERS
  report: [Input] Pointer to the UIM envelope report

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_ERROR  : if command could not be built successfully
    GSTK_SUCCESS: if command was built successfully

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_build_error_envelope_report (
  uint32                               env_ref_id, 
  uint32                               command_id,
  gstk_general_envelope_rsp_enum_type  rsp_type
);

/*===========================================================================
FUNCTION gstk_non_idle_st_mo_sms_ctrl_err_rsp

DESCRIPTION
  This function builds the sms control download error rsp in non idle state
  and sends it to client.  GSTK will set the status to SUCCESS and no MOD
  in the data so that the mobile can proceed with the SMS as the Toolkit
  is no longer valid

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
extern gstk_status_enum_type gstk_non_idle_st_mo_sms_ctrl_err_rsp(
  const gstk_envelope_mo_sms_ctrl_ind_type * mo_sms_ctrl_cmd
);

/*===========================================================================
FUNCTION gstk_non_idle_st_cc_err_rsp

DESCRIPTION

  This function builds the call control rsp error message in non idle state
  and sends it to client.  GSTK will set the status to SUCCESS and no MOD
  in the data so that the mobile can proceed with the call as the Toolkit
  is no longer valid

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
extern gstk_status_enum_type gstk_non_idle_st_cc_err_rsp(
  const gstk_envelope_cc_ind_type * cc_cmd
);

/*===========================================================================
FUNCTION gstk_non_idle_st_sms_pp_err_rsp

DESCRIPTION

  This function builds the envelope SMS PP error rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
extern gstk_status_enum_type gstk_non_idle_st_sms_pp_err_rsp(
  const gstk_envelope_sms_pp_download_ind_type * sms_pp_cmd
);

/*===========================================================================
FUNCTION gstk_non_idle_st_evt_dl_err_rsp

DESCRIPTION

  This function builds the event download rsp error in non idle state
  and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
extern  gstk_status_enum_type gstk_non_idle_st_evt_dl_err_rsp(
  const gstk_cmd_type * cmd
);

/*===========================================================================
FUNCTION gstk_non_idle_st_menu_selection_err_rsp

DESCRIPTION

  This function builds the menu selection error rsp in non idle state
  and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
extern gstk_status_enum_type gstk_non_idle_st_menu_selection_err_rsp(
  const gstk_envelope_menu_selection_ind_type * menu_sel_cmd
);

/*===========================================================================
FUNCTION gstk_non_idle_st_timer_expire_err_rsp

DESCRIPTION

  This function builds the timer expiry non idle state error rsp and sends 
  it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
extern gstk_status_enum_type gstk_non_idle_st_timer_expire_err_rsp(
  const gstk_envelope_timer_expire_ind_type * timer_expire_cmd);

#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */
#endif /*gstk_envelope_rsp_h*/

