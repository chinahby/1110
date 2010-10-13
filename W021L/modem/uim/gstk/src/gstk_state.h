#ifndef GSTK_STATE_H
#define GSTK_STATE_H


/*===========================================================================


                    G S T K    S T A T E    H E A D E R 


GENERAL DESCRIPTION
  This header file contains functions that are needed by the gstk_state.c.


FUNCTIONS
  gstk_state_start
    This function handles events/commands/signal for GSTK_START_S

  gstk_state_gsdi_sig_wait
    This function handles events/commands/signal for GSTK_GSDI_SIG_WAIT_S

  gstk_state_tp_wait
    This function handles events/commands/signals for 
    GSTK_TERM_PROFILE_WAIT_S

  gstk_state_tp_rsp_wait
    This function handles events/commands/signal for 
    GSTK_TERM_PROFILE_RSP_WAIT_S

  gstk_state_idle
    This function handles events/commands/signals for GSTK_IDLE_S

  gstk_state_no_sim
    This function handles events/commands/signals for GSTK_NO_SIM_S

  gstk_start_toolkit_session
    This function starts the toolkit session in the GSDI sign wait state or
    terminal profile wait state

  gstk_state_determine_action_after_gsdi_evt
    This function determine the state and the preferred toolkit slot based
    on the current toolkit_info mask

INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2006 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_state.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/23/06   sk      Fixed lint errors.
06/09/06   sk      Lint fixes
03/05/04   tml     Allowed single slot to operate individually in Dual Slot 
                   solution
09/04/03   tml     Dual slot support
05/16/03   tml     Removed pwer down function definition
02/07/03   tml     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk.h"

/* hold the current GSTK gstk */
extern gstk_state_enum_type gstk_state;




/*===========================================================================
FUNCTION gstk_state_start

DESCRIPTION

  This function handles events/commands/signal for GSTK_START_S
  GSTK_START_S may receive any of the following events:
  - SIM_INSERTED from GSDI -> do nothing
  - sim errors from GSDI -> transition to GSTK_NO_SIM_S
  - GSDI_OK_TP_DL from GSDI -> transition to GSTK_TERM_PROFILE_WAIT_S
  - GSDI_NOT_OK_TP_DL from GSDI -> transition to GSTK_NO_SIM_S
  - terminal profile download from client -> transition to GSTK_GSDI_SIG_WAIT_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed in this state

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
extern gstk_status_enum_type gstk_state_start (
        gstk_cmd_type            * cmd_p );



/*===========================================================================
FUNCTION gstk_state_gsdi_sig_wait

DESCRIPTION

  This function handles events/commands/signal for GSTK_GSDI_SIG_WAIT_S
  GSTK_GSDI_SIG_WAIT_S may receive any of the following events:
  - SIM_INSERTED from GSDI -> do nothing
  - sim errors from GSDI -> transition to GSTK_NO_SIM_S
  - GSDI_OK_TP_DL from GSDI -> transition to GSTK_TERM_PROFILE_RSP_WAIT_S
  - GSDI_NOT_OK_TP_DL from GSDI -> transition to GSTK_NO_SIM_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed in this state

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
extern gstk_status_enum_type gstk_state_gsdi_sig_wait (
  const gstk_cmd_type            * cmd_p );


/*===========================================================================
FUNCTION gstk_state_tp_wait

DESCRIPTION

  This function handles events/commands/signal for GSTK_TERM_PROFILE_WAIT_S
  GSTK_TERM_PROFILE_WAIT_S may receive any of the following events:
  - sim errors from GSDI -> transition to GSTK_NO_SIM_S
  - terminal profile info from client -> transition to GSTK_TERM_PROFILE_RSP_WAIT_S
  - timer expiration from GSTK -> transition to GSTK_NO_SIM_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed in this state

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
extern gstk_status_enum_type gstk_state_tp_wait (
        gstk_cmd_type            * cmd_p );


/*===========================================================================
FUNCTION gstk_state_tp_rsp_wait

DESCRIPTION

  This function handles events/commands/signal for GSTK_TERM_PROFILE_RSP_WAIT_S
  GSTK_TERM_PROFILE_RSP_WAIT_S may receive any of the following events:
  - sim errors from GSDI -> transition to GSTK_NO_SIM_S
  - terminal profile response from uim -> transition to GSTK_IDLE_S/GSTK_NO_SIM_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed in this state

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
extern gstk_status_enum_type gstk_state_tp_rsp_wait (
  const gstk_cmd_type            * cmd_p );


/*===========================================================================
FUNCTION gstk_state_idle

DESCRIPTION

  This function handles events/commands/signal for GSTK_IDLE_S
  GSTK_IDLE_S may receive any of the following events:
  - sim errors from GSDI -> transition to GSTK_NO_SIM_S
  - terminal response from client -> stay in GSTK_IDLE_S
  - proactive command from uim -> stay in GSTK_IDLE_S
  - envelope command from client -> stay in GSTK_IDLE_S
  - envelope response from uim -> stay in GSTK_IDLE_S
  - timer expiration from GSTK -> stay in GSTK_IDLE_S
  - end proactive session from uim -> stay in GSTK_IDLE_S
  - GSDI_REFRESH_SIM_INIT from GSDI -> GSTK_NO_SIM_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed in this state

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
extern gstk_status_enum_type gstk_state_idle (
        gstk_cmd_type            * cmd_p );

/*===========================================================================
FUNCTION gstk_state_no_sim

DESCRIPTION

  This function handles events/commands/signal for GSTK_NO_SIM_S
  GSTK_NO_SIM_S may receive any of the following events:
  - sim valid from GSDI -> transition to GSTK_GSDI_SIG_WAIT_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed in this state

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
extern gstk_status_enum_type gstk_state_no_sim (
    const gstk_cmd_type            * cmd_p );
                         
#ifdef FEATURE_GSTK_DUAL_SLOTS     
/*===========================================================================
FUNCTION gstk_start_toolkit_session

DESCRIPTION
  This function starts a toolkit session in the specified slot after 
  checking for pending mode changes.

PARAMETERS
  None

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
extern gstk_status_enum_type gstk_start_toolkit_session ( void );

/*===========================================================================
FUNCTION gstk_state_determine_action_after_gsdi_evt

DESCRIPTION
  This function determine the state and the preferred toolkit slot based
  on the current toolkit_info mask

PARAMETERS
  toolkit_cap [Input/Output] - Pointer to toolkit capability of the cards

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
extern void gstk_state_determine_action_after_gsdi_evt(gstk_toolkit_cap_enum_type *toolkit_cap); 

#endif/* FEATURE_GSTK_DUAL_SLOTS */
#endif /* GSTK_STATE_H */

