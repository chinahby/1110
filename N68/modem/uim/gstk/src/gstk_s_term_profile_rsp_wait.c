#ifndef GSTK_S_TERM_PROFILE_RSP_WAIT_C
#define GSTK_S_TERM_PROFILE_RSP_WAIT_C


/*===========================================================================


   G S T K   T E R M I N A L   P R O F I L E   R E S P O N S E   W A I T


GENERAL DESCRIPTION
  This source file contains function to handle event for
  GSTK_TERM_PROFILE_RSP_WAIT_S state.


FUNCTIONS
  gstk_state_tp_rsp_wait
    This function handles events/commands/signal for
    GSTK_TERM_PROFILE_RSP_WAIT_S


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


$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_s_term_profile_rsp_wait.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/08   xz      Fixed wrong debug message
10/22/08   sk      Handle client id reg and proactive command reg
09/14/08   sk      Fixed lint errors
09/08/08   sk      Fixed indentation issues
09/02/08   sk      Fixed error message
05/06/08   sk      Added support for event download envelope retry 
06/11/07   jk      Revert changes from previous fix as they are not necessary,
                   and introduce possiblity of FATAL ERROR if TP-DL delayed
02/13/07   pv      If GSDI_INTERNAL_SIM_RESET is received even when waiting
                   for a terminal response, do a TP-DL again.
08/25/06   sk      Featurized uim recovery
07/25/06   sk      Added support to handle envelope cmds in non idle state
07/12/06   sk      During UIM recovery, if TP dl was not done prior to 
                   receiving this cmd, return TP dl done to GSDI  
06/14/06   sk      Removed warnings
06/13/06   sk      UIM recovery support
06/09/06   sk      Lint fixes
04/04/06   sk      Added support for 1X related to location information
11/09/05   sk      Fixed Lint Errors
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID 
                   changed during normal service
07/29/05   sk      Fixed compilation errors.
04/02/04   tml     linted
03/05/04   tml     Allowed single slot to operate individually in Dual Slot 
                   solution
02/19/04   tml     compilation fix
02/04/04 jar/tml   Turn off Proactive UIM Bit for failed terminal profile dl.
09/04/03   tml     Dual slot support
07/07/03   tml     Changed message to capture more meaningful info
05/16/03   tml     linted
03/21/03   tml     Reflect API change in gsdi
02/07/03   tml     Initial Version



===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk_state.h"
#include "gstkutil.h"
#include "gstk_term_profile.h"
#include "gstk_envelope_cmd.h"
#include "gstk.h"

#include "gsdi_exp.h"
#include "err.h"

#ifdef FEATURE_GSTK_DUAL_SLOTS
/*===========================================================================
FUNCTION gstk_process_gsdi_evt_in_term_profile_rsp_state

DESCRIPTION

  This function checks the toolkit capability of the card, set the gstk_state
  appropriately

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
void gstk_process_gsdi_evt_in_term_profile_rsp_state(void) 
{
  gstk_toolkit_cap_enum_type    toolkit_cap = GSTK_UNKNOWN_TK_CAPABLE;

  gstk_state_determine_action_after_gsdi_evt(&toolkit_cap);

  switch (toolkit_cap) {
  case GSTK_UNKNOWN_TK_CAPABLE:
    /* the determine_action_after_gsdi_evt decides that it should be SIG_WAIT */
    /* reset to START_S */
    /* from start state, the only way to go to gsdi sig wait state is if
      * received TP from user */
    MSG_ERROR("The only TK cap becomes bad while waiting for TP rsp!", 0, 0, 0);
    /* reset the state to TERM_PROFILE_RSP_WAIT, will handle this case when the
      * response is received */
    gstk_state = GSTK_TERM_PROFILE_RSP_WAIT_S;
    break;
  case GSTK_NO_TK_CAPABLE:
    gstk_curr_tk_slot = GSTK_SLOT_NONE;
    gstk_state = GSTK_NO_SIM_S;
    break;
  case GSTK_BOTH_TK_CAPABLE:
  case GSTK_ONE_TK_CAPABLE:
    gstk_state = GSTK_TERM_PROFILE_RSP_WAIT_S;
    break;
  }
} /* gstk_process_gsdi_evt_in_term_profile_rsp_state */
#endif /*FEATURE_GSTK_DUAL_SLOTS */


/*===========================================================================
FUNCTION gstk_state_tp_rsp_wait

DESCRIPTION

  This function handles events/commands/signal for GSTK_TERM_PROFILE_RSP_WAIT_S
  GSTK_TERM_PROFILE_RSP_WAIT_S may receive any of the following events:
  - sim errors from GSDI -> transition to GSTK_NO_SIM_S
  - terminal profile response from uim -> transition to GSTK_IDLE_S/GSTK_NO_SIM_S
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
gstk_status_enum_type gstk_state_tp_rsp_wait (
  const gstk_cmd_type            * cmd_ptr )
{
  gstk_status_enum_type   gstk_status           = GSTK_SUCCESS;
  gstk_cmd_from_card_type terminal_profile_rsp;

#ifdef FEATURE_GSTK_DUAL_SLOTS
  gstk_toolkit_cap_enum_type    toolkit_cap = GSTK_UNKNOWN_TK_CAPABLE;
#endif

  MSG_HIGH("GSTK_TP_RSP_WAIT", 0, 0, 0);

  if(cmd_ptr == NULL)
  {
    MSG_ERROR("cmd_ptr ptr:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  memset(&terminal_profile_rsp, 0x00, sizeof(gstk_cmd_from_card_type));

  switch( cmd_ptr->general_cmd.message_header.command_group ) {
  case GSTK_TERMINAL_PROFILE_RSP: /* received rsp from UIM */
    terminal_profile_rsp.hdr_cmd.command_id = GSTK_PROFILE_DL_IND_RSP;
    /* command_number, cmd_ref_id are ignore */
    terminal_profile_rsp.hdr_cmd.cmd_detail_reference = 0;
    terminal_profile_rsp.hdr_cmd.command_number = 0;

    /* Signal GSDI regardless of whether TP success or fails so that
    ** GSDI can continue with initialization */
    gsdi_util_gstk_tp_dl_complete ();

    if(cmd_ptr->terminal_profile_rsp.success) 
    {
      MSG_HIGH("Received UIM TP Rsp", 0, 0, 0);
      terminal_profile_rsp.cmd.terminal_profile_rsp = GSTK_PROFILE_RSP_SUCCESS;
      gstk_state = GSTK_IDLE_S;

#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
      gstk_status = gstk_get_nv(
        NV_TOOLKIT_ENV_RETRY_FLAG_I,
        (nv_item_type *)&(gstk_curr_envelope_info.retry_info.retry_flag));/*lint !e826 area too small */
      if(gstk_status != GSTK_SUCCESS)
      {
        MSG_ERROR("NV_TOOLKIT_ENV_RETRY_I ERROR",0,0,0);
        gstk_curr_envelope_info.retry_info.retry_flag = FALSE;
      }
#else
      gstk_curr_envelope_info.retry_info.retry_flag = TRUE;
#endif  /* FEATURE_GSTK_FOR_NON_LTK_BUILD */
      if(gstk_curr_envelope_info.retry_info.retry_flag == TRUE)
      {
        MSG_HIGH("Env retry flag is set to TRUE",0,0,0);
        /* clear timer for envelope retry */
        (void)rex_clr_timer(&gstk_curr_envelope_info.gstk_env_retry_timer);

        /* define timer for envelope retry */
        rex_def_timer(&(gstk_curr_envelope_info.gstk_env_retry_timer), 
                      &gstk_tcb,
                      GSTK_TIMER_FOR_ENV_RETRY_SIG);
      }  
#ifndef FEATURE_VIRTUAL_SIM
#ifndef FEATURE_GSTK_DUAL_SLOTS
      uim_set_proactive_uim(TRUE);
#else
      switch (gstk_curr_tk_slot) {
      case GSTK_SLOT_1:
        uim_set_proactive_uim(TRUE);
        break;
      case GSTK_SLOT_2:
        uim_set_proactive_uim_2(TRUE);
        break;
      case GSTK_SLOT_NONE:
        MSG_ERROR("None curr_tk_slot in TP rsp", 0, 0, 0);
        break;
      }
#endif /*FEATURE_GSTK_DUALS_SLOTS */
#endif /* FEATURE_VIRTUAL_SIM */
    }
    else 
    {
      terminal_profile_rsp.cmd.terminal_profile_rsp = GSTK_PROFILE_RSP_FAIL;
#ifndef FEATURE_GSTK_DUAL_SLOTS
      gstk_state = GSTK_NO_SIM_S;
#else           
      
      if ( gstk_curr_tk_slot == GSTK_SLOT_1)
      {
        /* take out the toolkit capability mask for slot 1 */
        gstk_toolkit_info &= ~GSTK_SLOT1_TK_CAPABLE_MASK;
      }
      else if ( gstk_curr_tk_slot == GSTK_SLOT_2)
      {
        /* take out the toolkit capability mask for slot 1 */
        gstk_toolkit_info &= ~GSTK_SLOT2_TK_CAPABLE_MASK;
      }

      gstk_state_determine_action_after_gsdi_evt(&toolkit_cap);
      MSG_HIGH("TP Failed toolkit cap, %x", toolkit_cap, 0, 0);
        
      switch (toolkit_cap) {
      case GSTK_UNKNOWN_TK_CAPABLE:
        /* TP fails because the original one is no longer responding but 
           the other slot is not initialized */
        /* go back to GSDI WAIT state for the other TP */
        /* reset the curr tk slot to NONE */
        gstk_curr_tk_slot = GSTK_SLOT_NONE;
        gstk_state = GSTK_GSDI_SIG_WAIT_S;
        break;
      case GSTK_NO_TK_CAPABLE: /* 1 TK and No TK will both become No TK
                                  since the bad one has been set to not cap */
        /* TP fail because there is no more good slot */
        /* reset preferred and curr tk slot to NONE as there is no more good slot */
        gstk_preferred_tk_slot = GSTK_SLOT_NONE;
        gstk_curr_tk_slot = GSTK_SLOT_NONE;
        gstk_state = GSTK_NO_SIM_S;
        break;
      case GSTK_ONE_TK_CAPABLE: /* Both TK will becomes 1 TK since the 
                                   failure one has been set to not cap */
        /* TP fail when there is still another good slot for tk */
        if (gstk_preferred_tk_slot == gstk_curr_tk_slot) {
          switch (gstk_curr_tk_slot) {
          case GSTK_SLOT_1:
            gstk_preferred_tk_slot = GSTK_SLOT_2;
            break;
          case GSTK_SLOT_2:
            gstk_preferred_tk_slot = GSTK_SLOT_1;
            break;
          case GSTK_SLOT_NONE:
            gstk_preferred_tk_slot = GSTK_SLOT_NONE;
            gstk_state = GSTK_NO_SIM_S;
            break;
          }
        }

        /* try the new preferred slot if different from the curr one */
        if ((gstk_preferred_tk_slot != GSTK_SLOT_NONE) &&
            (gstk_curr_tk_slot != GSTK_SLOT_NONE) &&
            (gstk_curr_tk_slot != gstk_preferred_tk_slot)) 
        {
          /* reset the curr_tk_slot */
          gstk_curr_tk_slot = GSTK_SLOT_NONE;
          gstk_status = gstk_start_toolkit_session();
          if (gstk_status != GSTK_SUCCESS) {
              MSG_MED("Send TP error", 0, 0, 0);
          }
        }
        else 
        {
          MSG_ERROR("In TP Wait state but preferred tk slot is not set!", 0, 0, 0);
        }
        break;
      }
#endif /*FEATURE_GSTK_DUAL_SLOTS */
    }

    /* send response to client if there is client registered for this */
    gstk_status = gstk_util_send_message_to_clients(&terminal_profile_rsp);
    if(gstk_status != GSTK_SUCCESS)
    {
      MSG_MED("No client reg for TP Rsp, %d", gstk_status, 0, 0);
      /* reset the gstk status for terminal profile response failure */
      gstk_status = GSTK_SUCCESS;
    }
    break;

  case GSTK_GSDI_CMD:
    MSG_HIGH("Received GSDI, %x", cmd_ptr->gsdi_evt.command_id, 0, 0);
    switch( cmd_ptr->gsdi_evt.command_id ) {
    case GSDI_SIM_REMOVED:
    case GSDI_NO_SIM:
    case GSDI_DRIVER_ERROR:
    case GSDI_CARD_ERROR:
#ifndef FEATURE_GSTK_DUAL_SLOTS
      gstk_state = GSTK_NO_SIM_S;
#else
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT1_CARD_ERROR_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_INITIALIZED_MASK;

      gstk_process_gsdi_evt_in_term_profile_rsp_state();

#endif /* FEATURE_GSTK_DUAL_SLOTS */
      break; /* end - case GSTK_CARD_ERROR, GSDI_DRIVER_ERROR, GSDI_NO_SIM,
             ** GSDI_SIM_REMOVED */

#ifdef FEATURE_GSTK_DUAL_SLOTS
    case GSDI_CARD_ERROR_2:
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT2_CARD_ERROR_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_INITIALIZED_MASK;

      gstk_process_gsdi_evt_in_term_profile_rsp_state();

      break; /* end - case GSDI_CARD_ERROR_2 */

    case GSDI_OK_FOR_TERMINAL_PROFILE_DL_2:
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT2_INITIALIZED_MASK; 
      gstk_toolkit_info |= GSTK_SLOT2_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_CARD_ERROR_MASK;

      gstk_process_gsdi_evt_in_term_profile_rsp_state();

      break; /* end - case GSDI_OK_FOR_TERMINAL_PROFILE_DL_2 */
  
  case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL_2:
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT2_INITIALIZED_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_CARD_ERROR_MASK;

      gstk_process_gsdi_evt_in_term_profile_rsp_state();
      /* notify GSDI received TP DL */
      gsdi_util_gstk_tp_dl_complete ();

      break; /* end - case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL_2 */

    /* for single slot, should never get this event in this state */
    case GSDI_OK_FOR_TERMINAL_PROFILE_DL:
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT1_INITIALIZED_MASK;
      gstk_toolkit_info |= GSTK_SLOT1_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_CARD_ERROR_MASK;

      gstk_process_gsdi_evt_in_term_profile_rsp_state();

      break; /* end - case GSDI_OK_FOR_TERMINAL_PROFILE_DL */

    /* for single slot, should never get this event in this state */
    case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL:
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT1_INITIALIZED_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_CARD_ERROR_MASK;

      gstk_process_gsdi_evt_in_term_profile_rsp_state();
      /* notify GSDI received TP DL */
      gsdi_util_gstk_tp_dl_complete ();

      break; /* end - case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL */

#endif /* FEATURE_GSTK_DUAL_SLOTS */

#ifndef FEATURE_NO_UIM_RECOVERY_SUPPORT
    case GSDI_INTERNAL_SIM_RESET:
      /* Notify GSDI that TP dl is complete */
      gsdi_util_gstk_tp_dl_complete ();
      break;
#endif /* FEATURE_NO_UIM_RECOVERY_SUPPORT */

    default:
        break; /* ignore all other gsdi_sim_event_T */
    }
    break;

  case GSTK_MODE_CHANGE_IND:

#ifdef FEATURE_GSTK_DUAL_SLOTS
    gstk_util_handle_mode_change(cmd_ptr->mode_change_ind.cmd.new_mode);
#endif /* FEATURE_GSTK_DUAL_SLOTS */       

#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
    if(cmd_ptr->mode_change_ind.cmd.access_technology == GSTK_ACCESS_TECH_CDMA)
    {
      gstk_cache_1x_location_info(cmd_ptr->mode_change_ind.cmd.cdma_loc_info);
    }
#else  /*#ifdef FEATURE_CCAT */
    MSG_HIGH("GSTK_MODE_CHANGE_IND : FEATURE_CCAT not supported",0,0,0);
#endif /*#ifdef FEATURE_CCAT */
#else  /* #ifdef FEATURE_CAT_REL6 */
    MSG_HIGH("GSTK_MODE_CHANGE_IND : FEATURE_CAT_REL6 not supported",0,0,0);
#endif /* #ifdef FEATURE_CAT_REL6 */
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

  case GSTK_ENVELOPE_CMD:
    MSG_HIGH("Received Envelope cmd", 0, 0, 0);
    gstk_status = gstk_process_non_idle_state_envelope_cmd(cmd_ptr);
    if (gstk_status != GSTK_SUCCESS) 
    {
      MSG_ERROR("Envelope Cmd ERR, %x", gstk_status, 0, 0);
    }
    break;

#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

  default:
    MSG_MED("Cmd 0x%x Not handled in T_P_RSP_WAIT_S", 
            cmd_ptr->general_cmd.message_header.command_group , 0, 0);
    break;
  }

  return gstk_status;
} /*gstk_state_tp_rsp_wait*/


#endif  /* GSTK_S_TERM_PROFILE_RSP_WAIT_C */
/*lint -e750 local macro gstk_c not being used */


