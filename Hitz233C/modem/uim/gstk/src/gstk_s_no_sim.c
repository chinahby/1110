/*===========================================================================

 
                    G S T K     N O     S I M


GENERAL DESCRIPTION
  This source file contains function to handle event for 
  GSTK_NO_SIM_S state.


FUNCTIONS
  gstk_state_no_sim
    This function handles events/commands/signals for GSTK_NO_SIM_S


INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2006,2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_s_no_sim.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/22/08   sk      Handle client id reg and proactive command reg
09/08/08   sk      Fixed indentation issues
09/02/08   sk      Fixed error message
08/25/08   xz      Start a timer and wait for all expected TP download
08/06/08   xz      Remove gstk_profile_received and send term profile as soon
                   as GSTK gets GSDI_OK_FOR_TERMINAL_PROFILE_DL
10/23/06   sk      Fixed lint errors.
08/25/06   sk      Featurized uim recovery
07/25/06   sk      Added support to handle envelope cmds in non idle state
07/12/06   sk      During UIM recovery, if TP dl was not done prior to 
                   receiving this cmd, return TP dl done to GSDI 
06/24/06   sk      Lint fixes
06/13/06   sk      UIM recovery support
06/09/06   sk      Lint fixes
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
10/18/05   tml     Added support for OK and NOT OK for TP
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID 
                   changed during normal service
05/11/05   sst     Lint fixes
11/23/04   jar     Changed GSTK_START_s to GSTK_START_S to fix Dual Slot 
                   compile problem.
08/30/04   tml     Go to Start State if gstk_profile_received is set to FALSE
06/08/04   tml     Added support for timer expiration, browser term and lang
                   selection envelope support
06/07/04   tml     Returned envelope response in NO SIM state
03/05/04   tml     Allowed single slot to operate individually in Dual Slot 
                   solution
09/04/03   tml     Dual slot support
05/26/03   tml     linted
02/07/03   tml     Initial Version


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "gstk_state.h"
#include "gstkutil.h"
#include "gstk_term_profile.h"
#include "gstk_envelope_cmd.h"

#include "gsdi_exp.h"
#include "err.h"


#ifdef FEATURE_GSTK_DUAL_SLOTS
/*===========================================================================
FUNCTION gstk_process_gsdi_evt_in_no_sim_state

DESCRIPTION

  This function checks the toolkit capability of the card, set the gstk_state
  appropriately and determine if a TP is ready to be sent to the card

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
void gstk_process_gsdi_evt_in_no_sim_state(void) 
{
  gstk_toolkit_cap_enum_type    toolkit_cap = GSTK_UNKNOWN_TK_CAPABLE;

  gstk_state_determine_action_after_gsdi_evt(&toolkit_cap);

  switch (toolkit_cap) {
  case GSTK_UNKNOWN_TK_CAPABLE:
    /* remain in GSTK_NO_SIM */
    break;
  case GSTK_NO_TK_CAPABLE:
    /* remain in GSTK_NO_SIM */
    break;
  case GSTK_BOTH_TK_CAPABLE:
  case GSTK_ONE_TK_CAPABLE:
    gstk_start_toolkit_session();
    break;
  }
} /* gstk_process_gsdi_evt_in_no_sim_state */
#endif /*FEATURE_GSTK_DUAL_SLOTS */

/*===========================================================================
FUNCTION gstk_state_no_sim

DESCRIPTION

  This function handles events/commands/signal for GSTK_NO_SIM_S
  GSTK_NO_SIM_S may receive any of the following events:
  - sim valid from GSDI -> transition to GSTK_GSDI_SIG_WAIT_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_ptr: [Input] Pointer to the command to be processed in this state

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
gstk_status_enum_type gstk_state_no_sim (
  const gstk_cmd_type  * cmd_ptr )
{
  gstk_status_enum_type  gstk_status = GSTK_SUCCESS;

  MSG_HIGH("GSTK_NO_SIM", 0, 0, 0);

  if(cmd_ptr == NULL)
  {
    MSG_ERROR("cmd_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  switch( cmd_ptr->general_cmd.message_header.command_group ) {
  case GSTK_GSDI_CMD:
    MSG_HIGH("Received GSDI, %x", cmd_ptr->gsdi_evt.command_id, 0, 0);
    switch( cmd_ptr->gsdi_evt.command_id ) {
    case GSDI_OK_FOR_TERMINAL_PROFILE_DL:
#ifndef FEATURE_GSTK_DUAL_SLOTS
      if (gstk_num_of_tp_dl_received == GSTK_EXPECTED_NUM_OF_TP_DL)
      {
        /* use the gstk_terminal_profile_cache info and send it to uim */
        gstk_status = gstk_pack_tp_cmd_to_uim();
        if (gstk_status == GSTK_SUCCESS) 
        {
          /* go to wait for TP response state */
          gstk_state = GSTK_TERM_PROFILE_RSP_WAIT_S;
        }
        else 
        {
          MSG_ERROR("Send TP error", 0, 0, 0);
        }
      }
      else
      {
        (void)rex_set_timer( &gstk_tp_wait_timer, GSTK_TP_CMD_WAIT_TIME );
        gstk_state = GSTK_TERM_PROFILE_WAIT_S;
      }
#else
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT1_INITIALIZED_MASK;
      gstk_toolkit_info |= GSTK_SLOT1_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_CARD_ERROR_MASK;

      gstk_process_gsdi_evt_in_no_sim_state();

#endif /* FEATURE_GSTK_DUAL_SLOTS */
      break;   /* end - case GSDI_OK_FOR_TERMINAL_PROFILE_DL */

#ifdef FEATURE_GSTK_DUAL_SLOTS
    case GSDI_OK_FOR_TERMINAL_PROFILE_DL_2:
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT2_INITIALIZED_MASK;
      gstk_toolkit_info |= GSTK_SLOT2_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_CARD_ERROR_MASK;

      gstk_process_gsdi_evt_in_no_sim_state();

      break; /* end - case GSDI_OK_FOR_TERMINAL_PROFILE_DL_2 */
#endif /* FEATURE_GSTK_DUAL_SLOTS */

    case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL: /* no TP is allowed/required */
      /* For single slot, remain in the NO_SIM state */
      /* For dual slot, set the mask and see if we need to do TP accordingly */
#ifdef FEATURE_GSTK_DUAL_SLOTS
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT1_INITIALIZED_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_CARD_ERROR_MASK;

      gstk_process_gsdi_evt_in_no_sim_state();
#endif /* FEATURE_GSTK_DUAL_SLOTS */

      /* notify GSDI received TP DL */
      gsdi_util_gstk_tp_dl_complete ();
      break; /* end - case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL */


#ifdef FEATURE_GSTK_DUAL_SLOTS
    case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL_2: /* no TP is allowed/required */
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT2_INITIALIZED_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_CARD_ERROR_MASK;

      gstk_process_gsdi_evt_in_no_sim_state();
      /* notify GSDI received TP DL */
      gsdi_util_gstk_tp_dl_complete ();

      break; /* end - case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL_2 */
#endif /* FEATURE_GSTK_DUAL_SLOTS */

    case GSDI_SIM_INSERTED: /* for refresh/reset command etc */
#ifdef FEATURE_GSTK_DUAL_SLOTS
      /* update the mask */
      gstk_toolkit_info &= ~GSTK_SLOT1_INITIALIZED_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_CARD_ERROR_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_TK_CAPABLE_MASK;
#endif /* FEATURE_GSTK_DUAL_SLOTS */
      if (gstk_profile_received) 
      {
        gstk_state = GSTK_GSDI_SIG_WAIT_S;
      }
      else 
      {
        gstk_state = GSTK_START_S;
      }
      break; /* end - case GSDI_SIM_INSERTED */

#ifdef FEATURE_GSTK_DUAL_SLOTS
    case GSDI_SIM_INSERTED_2: /* for refresh/reset command etc */
      /* update the mask */
      gstk_toolkit_info &= ~GSTK_SLOT2_INITIALIZED_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_CARD_ERROR_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_TK_CAPABLE_MASK;
      if (gstk_profile_received) 
      { 
        gstk_state = GSTK_GSDI_SIG_WAIT_S;
      }
      else 
      {
        gstk_state = GSTK_START_S;
      }
      break; /* end - case GSDI_SIM_INSERTED_2 */
#endif /* FEATURE_GSTK_DUAL_SLOTS */

#ifndef FEATURE_NO_UIM_RECOVERY_SUPPORT
    case GSDI_INTERNAL_SIM_RESET:
      /* Notify GSDI that TP dl is complete */
      gsdi_util_gstk_tp_dl_complete ();

      /* Remain in No SIM state */
      break;
#endif /* FEATURE_NO_UIM_RECOVERY_SUPPORT */

    default:
      /* GSDI_SIM_INIT_COMPLETED should be ignored by GSTK_NO_SIM_S
         since if we receive this event, we should have already passed
         any window of opportunities for supporting (U)SAT functions */
      break; /* ignore all other gsdi_sim_event_T */
    }
    break;

  case GSTK_ENVELOPE_CMD:
    MSG_HIGH("Received Envelope cmd", 0, 0, 0);
    gstk_status = gstk_process_non_idle_state_envelope_cmd(cmd_ptr);
    if (gstk_status != GSTK_SUCCESS) 
    {
      MSG_ERROR("Envelope Cmd ERR, %x", gstk_status, 0, 0);
    }
    break;

  case GSTK_STOP_CMD:
    MSG_HIGH("Received STOP Sig", 0, 0, 0);
    /* flush GSTK queue, clean up and ack tmc */
    gstk_status = gstk_power_down();
    if (gstk_status == GSTK_SUCCESS) 
    {
      gstk_state = GSTK_POWER_DOWN_S;
    }
    else 
    {
      MSG_MED("Power Down err", 0, 0, 0);
    }
    break;

  case GSTK_MM_IDLE_CMD:
    MSG_MED("Ignore MM IDLE CMD", 0, 0, 0);
    /* gstk_mm_state_is_idle flag is used  as an indication to start queuing RRC messages */ 
    gstk_mm_state_is_idle = TRUE;
    break;

#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

  default:
    MSG_MED("Cmd 0x%x Not handled in NO_SIM_S", 
             cmd_ptr->general_cmd.message_header.command_group, 0, 0);
    break;
  }

  return gstk_status;
} /*gstk_state_no_sim*/

