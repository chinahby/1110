/*===========================================================================


                            G S T K     I D L E


GENERAL DESCRIPTION
  This source file contains function to handle event for GSTK_IDLE_S state.


FUNCTIONS
  gstk_state_idle
    This function handles events/commands/signals for GSTK_IDLE_S


INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2005,2008-2009 QUALCOMM, Incorporated and its licensors.  All 
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_s_idle.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/19/09   yb      Added secondary support for cell id information when 
                   rr_get_cell_id returns NULL in gstk_mo_sms_ctrl_ind()
10/22/08   sk      Handle client id reg and proactive command reg
09/14/08   sk      Fixed lint errors
09/08/08   sk      Fixed indentation issues
09/02/08   sk      Fixed error message
08/22/08   sk      Added support to send end session to only those clients
                   that are involved in the current proactive session 
08/21/08   sk      Added support to send end session to only those clients
                   that are involved in the current proactive session
08/08/08   xz      Added support for proactive command/terminal response
                   timer expiration
05/27/08   sk      Fixed expression evaluation
05/06/08   sk      Added support for event download envelope retry
05/06/08   sk      Fixed access technology updates to the card after a
                   No service. 
                   Fixed duplicate access technology updates to the card.
04/04/08   sk      Fix access technology updates to the card
12/19/07   sk      Query for cell id during MM Idle event only for 3G RAT
08/25/06   sk      Featurized uim recovery
06/16/06   jar     Changes to support offtarget VSIM Support
06/13/06   sk      UIM recovery support
06/06/06   sk      Added support for network search mod
05/17/06   sk      Added support to send access technology envelope
05/03/06   sk      Removed cell id checks.
05/03/06   tml     lint
04/04/06   sk      Added support for 1X related to location information
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
10/30/05   sk      Added check for Access technology event bit
                   set in set up event list in GSTK_MODE_CHANGE
10/26/05   tml     compilation fix
10/10/05   tml     Added curr Rat and next curr Rat caching
10/08/05   tml     Used new MM info type for handling MM IDLE event
10/07/05   tml     Added plmn and lac in rrc cell change ind type
09/30/05   sk      Added support for two tables, one for RRC location information
                   and one for MM location information
09/26/05   tml     Updated RRC cell id received variable to ensure location 
                   status for normal service is only sent when both MM and RRC
                   info are received
09/22/05   tml     Fixed cell ID change envelope download notification issue
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID 
                   changed during normal service
08/25/05   sk      Fix for envelope location status for 3G
                   in gstk_state_idle() for case GSTK_MM_IDLE_CMD
07/29/05   sk/tml  Fixed compilation errors.
                   fixed compilation problem
07/22/05   tml     util function for rrc cell id suport
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added support for Access Technology.
05/11/05   sst     Lint fixes
06/18/04   tml     Fixed dual slot not signaling GSDI for OK for TP issue
05/12/04   tml     Clear timer management timer when enter NO SIM state
04/02/04   tml     linted
03/17/04   tml     added handling for Terminal profile download during 
                   refresh sim init
03/05/04   tml     Allowed single slot to operate individually in Dual Slot 
                   solution
03/01/04   tml     Fixes for location info env command
02/24/04   tml     Fixed compilation issue with CDMA only target
01/25/04   tml     Dual Slot refresh support
09/04/03   tml     Dual slot support
07/07/03   tml     Made messages more meaningful
06/23/03   tml     removed compiler error for 1x/gsm
05/26/03   tml     linted
03/21/03   tml     Update featurization for WCDMA single build
03/20/03   tml     Properly featurize local info for FEATURE_WCDMA and
                   FEATURE_GSM
03/07/03   tml     Added (FEATURE_GSTK_FOR_NON_LTK_BUILD)
02/28/03   jar     Do not go to GSTK_GSDI_SIG_WAIT when GSDI_REFRESH_SIM_INIT
02/19/03   jar     Modifed MSG_HIGH in gstk_state_idle() from GSDI to GSTK.
                   Added Handling of GSDI_REFRESH_SIM_RESET.
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk_state.h"
#include "gstk_proactive_cmd.h"
#include "gstk_terminal_rsp.h"
#include "gstk_envelope_cmd.h"
#include "gstk_envelope_rsp.h"
#include "gstkutil.h"
#include "gstk_term_profile.h"

#include "string.h"
#include "gsdi_exp.h"
#include "err.h"
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM
#error code not present
#endif /*FEATURE_GSM */
#endif
#if ((defined FEATURE_GSM) || (defined FEATURE_WCDMA))
#error code not present
#endif /* ((defined FEATURE_GSM) || (defined FEATURE_WCDMA)) */

#ifdef FEATURE_GSTK_DUAL_SLOTS
/*===========================================================================
FUNCTION gstk_process_gsdi_evt_in_idle_state

DESCRIPTION

  This function checks the toolkit capability of the card, set the gstk_state
  appropriately 

PARAMETERS
  None

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
void gstk_process_gsdi_evt_in_idle_state(void) 
{
  gstk_toolkit_cap_enum_type    toolkit_cap = GSTK_UNKNOWN_TK_CAPABLE;
  int i;

  gstk_state_determine_action_after_gsdi_evt(&toolkit_cap);

  switch (toolkit_cap) {
  case GSTK_UNKNOWN_TK_CAPABLE:
    /* goto GSDI_SIG_WAIT */
    gstk_state = GSTK_GSDI_SIG_WAIT_S;
    break;
  case GSTK_NO_TK_CAPABLE:
    gstk_state = GSTK_NO_SIM_S;
    /* clear the timers */
    for (i = 0; i< GSTK_MAX_TIMER_SUPPORTED; i++) 
    {
      rex_clr_timer( &(gstk_timer_management[i].timer) );
      rex_clr_timer( &(gstk_timer_management[i].timer_elapsed) );
      gstk_timer_management[i].time = 0;
      gstk_timer_management[i].timer_started = FALSE;
      gstk_timer_management[i].pending = FALSE;
      gstk_timer_management[i].waiting_rsp = FALSE;

    }
    break;
  case GSTK_BOTH_TK_CAPABLE:
  case GSTK_ONE_TK_CAPABLE:
    /* remain in idle */
    break;
  }
} /* gstk_process_gsdi_evt_in_idle_state */
#endif /*FEATURE_GSTK_DUAL_SLOTS */

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
  - GSDI_REFRESH_SIM_RESET from GSDI -> transition to GSTK_NO_SIM_S
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
gstk_status_enum_type gstk_state_idle (
  gstk_cmd_type            * cmd_ptr )
{
  gstk_status_enum_type           gstk_status     = GSTK_SUCCESS;
  gstk_cell_id_type               temp_cell_id;
  gstk_evt_dl_ind_type            evt_list;
  gstk_location_status_enum_type  location_status = GSTK_NO_SERVICE;  
  boolean                         ls_env          = TRUE;
  gstk_access_technology_type     rat             = GSTK_ACCESS_NONE;
  #ifdef FEATURE_GSTK_DUAL_SLOTS 
  gstk_slot_id_enum_type new_mode_chg_slot        = GSTK_SLOT_NONE;
  #else
  int i;
  #endif

  MSG_LOW("GSTK_IDLE", 0, 0, 0);

  if(cmd_ptr == NULL)
  {
    MSG_ERROR("cmd_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  memset(temp_cell_id.cell_id, 0xFF, (GSTK_MAX_CELL_ID_LEN * sizeof(uint8)));
  temp_cell_id.cell_len = 0;

  memset(&evt_list, 0x00, sizeof(gstk_evt_dl_ind_type));

  switch( cmd_ptr->general_cmd.message_header.command_group ) {
  case GSTK_PROACTIVE_CMD:
    MSG_HIGH("Received Proactive cmd", 0, 0, 0);
    gstk_status = gstk_process_proactive_command(&cmd_ptr->proactive_cmd);
    if (gstk_status != GSTK_SUCCESS) 
    {
      MSG_ERROR("Proactive Cmd ERR, %x", gstk_status, 0, 0);
    }
    if(gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_EVT_DL_IN_QUEUE)
    {
      if(gstk_util_retry_envelope_cmd() != GSTK_SUCCESS)
      {
        /* build envelope response back to the client */
        gstk_status = gstk_build_error_envelope_report(
          gstk_curr_envelope_info.env_ref_id,
          int32touint32((int32)envelope_cmd_ref_table[gstk_curr_envelope_info.env_ref_id].expected_cmd_rsp_type),
          GSTK_ENVELOPE_CMD_FAIL);
      }
    }
    break;

#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

  case GSTK_TERMINAL_RSP:
    MSG_HIGH("Received Term rsp", 0, 0, 0);
    gstk_status = gstk_process_terminal_response(cmd_ptr);
    if (gstk_status != GSTK_SUCCESS) 
    {
      MSG_ERROR("Terminal Rsp ERR, %x", gstk_status, 0, 0);
    }
    break;

  case GSTK_ENVELOPE_CMD:
    MSG_HIGH("Received Envelope cmd", 0, 0, 0);
    gstk_status = gstk_process_envelope_cmd(cmd_ptr);
    if (gstk_status != GSTK_SUCCESS) 
    {
      MSG_ERROR("Envelope Cmd ERR, %x", gstk_status, 0, 0);
    }
    break;

  case GSTK_ENVELOPE_RSP:
    MSG_HIGH("Received Envelope rsp", 0, 0, 0);
    gstk_status = gstk_process_envelope_rsp(cmd_ptr);
    if (gstk_status != GSTK_SUCCESS) 
    {
      MSG_ERROR("Envelope rsp ERR, %x", gstk_status, 0, 0);
    }
    if(!(gstk_util_is_envelope_rsp_evt_dl(cmd_ptr->envelope_response.message_header.command_id)) && 
       (cmd_ptr->envelope_response.rsp_status != GSTK_ENVELOPE_CMD_CARD_BUSY) && 
       (gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_EVT_DL_IN_QUEUE))
    {
      if(gstk_util_retry_envelope_cmd() != GSTK_SUCCESS)
      {
        /* build envelope response back to the client */
        gstk_status = gstk_build_error_envelope_report(
          gstk_curr_envelope_info.env_ref_id,
          int32touint32((int32)envelope_cmd_ref_table[gstk_curr_envelope_info.env_ref_id].expected_cmd_rsp_type),
          GSTK_ENVELOPE_CMD_FAIL);
      }
    }
    break;

  case GSTK_GSDI_CMD:
    MSG_HIGH("Received GSDI, %x", cmd_ptr->gsdi_evt.command_id, 0, 0);
    switch( cmd_ptr->gsdi_evt.command_id ) {
    case GSDI_SIM_REMOVED:
    case GSDI_NO_SIM:
    case GSDI_DRIVER_ERROR:
    case GSDI_REFRESH_SIM_RESET:
    case GSDI_CARD_ERROR:
#ifndef FEATURE_GSTK_DUAL_SLOTS
      gstk_state = GSTK_NO_SIM_S;
      gstk_is_curr_session_ui_related = FALSE;
      /* clear the timers */
      for (i = 0; i< GSTK_MAX_TIMER_SUPPORTED; i++) 
      {
        (void)rex_clr_timer( &(gstk_timer_management[i].timer) );
        (void)rex_clr_timer( &(gstk_timer_management[i].timer_elapsed) );
        gstk_timer_management[i].time = 0;
        gstk_timer_management[i].timer_started = FALSE;
        gstk_timer_management[i].pending = FALSE;
        gstk_timer_management[i].waiting_rsp = FALSE;
      }
#else
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT1_CARD_ERROR_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_INITIALIZED_MASK;

      gstk_process_gsdi_evt_in_idle_state();

#endif /* FEATURE_GSTK_DUAL_SLOTS */
      break; /* end - case GSDI_CARD_ERROR, GSDI_REFRESH_SIM_RESET, 
                 ** GSDI_DRIVER_ERROR, GSDI_NO_SIM, GSDI_SIM_REMOVED */

    case GSDI_OK_FOR_TERMINAL_PROFILE_DL:
#ifdef FEATURE_GSTK_DUAL_SLOTS
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT1_INITIALIZED_MASK; 
      gstk_toolkit_info |= GSTK_SLOT1_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_CARD_ERROR_MASK;

      gstk_process_gsdi_evt_in_idle_state();

      /* In refresh... */
      if (gstk_curr_tk_slot == GSTK_SLOT_1) 
#endif /*FEATURE_GSTK_DUAL_SLOTS */
      {
        /* use the gstk_terminal_profile_cache info and send it to uim */
        gstk_status = gstk_pack_tp_cmd_to_uim();
        if (gstk_status == GSTK_SUCCESS) {
          /* go to wait for TP response state */
          gstk_state = GSTK_TERM_PROFILE_RSP_WAIT_S;
        }
        else {
          MSG_ERROR("Send TP error", 0, 0, 0);
        }
      }
#ifdef FEATURE_GSTK_DUAL_SLOTS 
      else
      {
        /* curr tk slot != GSTK_SLOT_1 */
        /* signal GSDI */
        gsdi_util_gstk_tp_dl_complete ();
      }
#endif /*FEATURE_GSTK_DUAL_SLOTS */
      break;

#ifdef FEATURE_GSTK_DUAL_SLOTS
    case GSDI_REFRESH_SIM_RESET_2:
    case GSDI_CARD_ERROR_2:
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT2_CARD_ERROR_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_INITIALIZED_MASK;

      gstk_process_gsdi_evt_in_idle_state();
      break; /* end - case GSDI_CARD_ERROR_2 */

    case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL:
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT1_INITIALIZED_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT1_CARD_ERROR_MASK;

      gstk_process_gsdi_evt_in_idle_state();
      /* notify GSDI received TP DL */
      gsdi_util_gstk_tp_dl_complete ();
      break;
          
    case GSDI_OK_FOR_TERMINAL_PROFILE_DL_2:
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT2_INITIALIZED_MASK;
      gstk_toolkit_info |= GSTK_SLOT2_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_CARD_ERROR_MASK;
      gstk_process_gsdi_evt_in_idle_state();

      /* In refresh... */
      if (gstk_curr_tk_slot == GSTK_SLOT_2) 
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
        /* curr tk slot != GSTK_SLOT_2 */
        /* signal GSDI */
        gsdi_util_gstk_tp_dl_complete ();
      }
      break;

    case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL_2:
      /* update the mask */
      gstk_toolkit_info |= GSTK_SLOT2_INITIALIZED_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_TK_CAPABLE_MASK;
      gstk_toolkit_info &= ~GSTK_SLOT2_CARD_ERROR_MASK;

      gstk_process_gsdi_evt_in_idle_state();
      /* notify GSDI received TP DL */
      gsdi_util_gstk_tp_dl_complete ();
      break;
#endif /* FEATURE_GSTK_DUAL_SLOTS */
#ifndef FEATURE_NO_UIM_RECOVERY_SUPPORT
    case GSDI_INTERNAL_SIM_RESET:
      gstk_gsdi_internal_sim_reset    = TRUE;
      gstk_is_curr_session_ui_related = FALSE;

      /* clean up proactive cmd-TR tables */
      /* Keep the envelope cmd-rsp table to notify clients of the envelope 
         rsp from card after the internal sim reset is complete 
      */
      gstk_util_cmd_details_ref_table_free_all_slots();

      /* Notify all clients of the end of proactive session */
      gstk_status = gstk_util_build_proactive_end_session();
      if (gstk_status != GSTK_SUCCESS) 
      {
        MSG_ERROR("ERR in sending End Proactive session to clients", 0, 0, 0);
      }
      /* Send TP to UIM */
      gstk_status = gstk_pack_tp_cmd_to_uim();
      if (gstk_status == GSTK_SUCCESS) 
      {
        gstk_state = GSTK_TERM_PROFILE_RSP_WAIT_S;
      }
      else 
      {
        MSG_ERROR("ERR in sending Terminal Profile", 0, 0, 0);
      }
      /* Set this flag to FALSE to avoid sending cmd to the card through uim spl buffer
         during normal SIM intialization 
      */
      gstk_gsdi_internal_sim_reset = FALSE;
      break;
#endif /* FEATURE_NO_UIM_RECOVERY_SUPPORT */
    default:
      break; /* ignore all other gsdi_sim_event_T */
    }
#ifdef FEATURE_GSTK_DUAL_SLOTS 
    if (gstk_state != GSTK_IDLE_S) 
    {
      if (gstk_state != GSTK_TERM_PROFILE_RSP_WAIT_S) 
      {
        /* exiting to some other state 
        * set curr slot = NONE */
        gstk_curr_tk_slot = GSTK_SLOT_NONE;
      }
    }
#endif /*FEATURE_GSTK_DUAL_SLOTS */
    break;

  case GSTK_MM_IDLE_CMD:
    MSG_HIGH("Received MM Idle, %x", cmd_ptr->mm_idle_state.message_header.command_id, 0, 0);
      
    /* gstk_mm_state_is_idle flag is used  as an indication to start queuing RRC messages */ 
    gstk_mm_state_is_idle = TRUE;

    /* get the location status gstk enum */
    gstk_status = gstk_util_map_mm_to_gstk_location_status(&(cmd_ptr->mm_idle_state),
                                                             &location_status);
    MSG_HIGH("Location Status = 0x%x", location_status, 0, 0);

    if (gstk_status != GSTK_SUCCESS) 
    {
      MSG_ERROR("Bad location status", 0, 0, 0);
      break;
    }
    MSG_HIGH("MM_IDLE EVT............... ",0 ,0,0);
    MSG_HIGH("MM_IDLE Evt Location status is %d...... ",location_status ,0,0);
    MSG_HIGH("MM_IDLE Evt PLMN Id 0x%x 0x%x 0x%x",
              cmd_ptr->mm_idle_state.mcc_mnc[0],
              cmd_ptr->mm_idle_state.mcc_mnc[1],
              cmd_ptr->mm_idle_state.mcc_mnc[2]);

    MSG_HIGH("MM_IDLE Evt LAC Id : 0x%x 0x%x",
              cmd_ptr->mm_idle_state.lac[0],
              cmd_ptr->mm_idle_state.lac[1],
              0);

    if (cmd_ptr->mm_idle_state.cell_id.cell_len >= 2) {
      MSG_HIGH("MM_IDLE Evt Cell Id : 0x%x 0x%x",
               cmd_ptr->mm_idle_state.cell_id.cell_id[0],
               cmd_ptr->mm_idle_state.cell_id.cell_id[1],
               0);
    }

    if (cmd_ptr->mm_idle_state.cell_id.cell_len >= GSTK_MAX_CELL_ID_LEN) {
      MSG_HIGH("MM_IDLE Evt Cell Id : 0x%x 0x%x",
                cmd_ptr->mm_idle_state.cell_id.cell_id[2],
                cmd_ptr->mm_idle_state.cell_id.cell_id[3],
                0);
    }

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

    if (!gstk_util_is_valid_rat((sys_radio_access_tech_e_type)cmd_ptr->mm_idle_state.rat, &rat))
    {
      MSG_ERROR("Unknown Sys Rat 0x%x", cmd_ptr->mm_idle_state.rat, 0, 0);
      /* Indicate not to send any LS;  However, if we are in No Service,
         the switch statement on location status below will reset the
         boolean to TRUE, so that we will still send down an envelope
         for NO SERVICE regardless of RAT */
      ls_env = FALSE;
    }
    /* Store the current access technology used */
    gstk_next_curr_rat = rat;

#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
      /* If RAT is GSM or WCDMA Invalidate the CDMA location info cache and memset it */
    if(ls_env == TRUE)
    {
      cdma_location_info_cache.is_loc_info_valid = FALSE;
      memset(&cdma_location_info_cache.loc_info,
             0x00,
             sizeof(gstk_cdma_location_info_type));
    }
#endif /* #ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */

    /* Update MM-GSTK loci table when you receive new srvc notification with change in plmn/lac.
       So that the loci table can remain most updated for RRC cb checking purpose
    */
    switch(location_status)
    {
    case GSTK_NORMAL_SERVICE:
    case GSTK_LIMITED_SERVICE:
      /* Build access technology envelope if necessary */
      MSG_HIGH("GSTK_EVT_LIST[GSTK_ACCESS_TECH} is %d",gstk_evt_list[GSTK_ACCESS_TECH],0,0);

      if(((gstk_evt_list[GSTK_ACCESS_TECH] & 0x01) == 0x01) && 
         ((gstk_curr_rat != gstk_next_curr_rat) || 
         (gstk_next_curr_location_status != location_status)))
      { /* has registered for this event */
        gstk_curr_rat = gstk_next_curr_rat;
        gstk_status = gstk_build_access_technology_envelope(gstk_next_curr_rat);
        if(gstk_status != GSTK_SUCCESS) 
        {
          MSG_ERROR("Err in building Access Tech Envelope", 0, 0, 0);
        }
      }
      /* Initialized cell ID to what MM returned */
      memcpy(temp_cell_id.cell_id, cmd_ptr->mm_idle_state.cell_id.cell_id, GSTK_MAX_CELL_ID_LEN);
      temp_cell_id.cell_len = cmd_ptr->mm_idle_state.cell_id.cell_len;

      /* Attempting to get the latest cell id from RR and RRC. If no information 
         is available, cell id from MM will be used */
      /* Tried getting cell ID one more time */
#ifndef FEATURE_GSTK_FOR_NON_LTK_BUILD
      if((sys_radio_access_tech_e_type)cmd_ptr->mm_idle_state.rat == SYS_RAT_UMTS_RADIO_ACCESS)
      {
        if (gstk_get_curr_cell_id_info((sys_radio_access_tech_e_type)cmd_ptr->mm_idle_state.rat,
                                       cmd_ptr->mm_idle_state.mcc_mnc,
                                       cmd_ptr->mm_idle_state.lac,
                                       &temp_cell_id) == FALSE) 
        {
          /* Unknown Rat or null cell ID pointer, no need to update table or LS */
          MSG_ERROR("gstk_get_curr_cell_id_info error", 0, 0, 0);
          ls_env = FALSE;
          break;
        }
        else
        {
          /* Update the cmd_ptr cell ID data */
          memcpy(cmd_ptr->mm_idle_state.cell_id.cell_id, temp_cell_id.cell_id, GSTK_MAX_CELL_ID_LEN);
          cmd_ptr->mm_idle_state.cell_id.cell_len = temp_cell_id.cell_len;
        }
      }
#endif /*  FEATURE_GSTK_FOR_NON_LTK_BUILD */
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */
      break;

    case GSTK_NO_SERVICE:
       /* Reset to TRUE because if ls_env was set to FALSE at this point, it was
       because we have an unknown rat, but we will still want to send down a
       LS */
      ls_env = TRUE;
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
      break;
    }

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

    if (ls_env == FALSE) 
    {
      /* Cell ID Not ready yet in Normal or Limited Service, no need to send LS */
      MSG_HIGH("Cell ID Not ready, no LS Env", 0, 0, 0);
      break;
    }

    /* check for validity of the data */
    if (!gstk_util_is_valid_location_info(cmd_ptr->mm_idle_state.mcc_mnc,
                                          cmd_ptr->mm_idle_state.lac,
                                          cmd_ptr->mm_idle_state.cell_id.cell_id,
                                          location_status))
    {
      /* do not use gstk_status since that is used to indicate if need to 
         send down envelope command */
      MSG_ERROR("Invalid Location Info",0,0,0);
      break;
    }

    if (!gstk_mm_param_received)
    {
      /* set mm_param_received to TRUE since we have determined that the first
         set of data is good */
      MSG_HIGH("gstk_mm_param_received is set to TRUE",0,0,0);
      gstk_mm_param_received = TRUE;
      /* gstk_status == GSTK_SUCCESS to ensure we will send down the first
        location info if requested by the card */
    }
    else 
    {
      MSG_HIGH("gstk_mm_param_received is set to TRUE",0,0,0);

      /* already received the data, need to see if there is any difference */
      if (!gstk_util_check_mm_state_difference(cmd_ptr->mm_idle_state.mcc_mnc,
                                               cmd_ptr->mm_idle_state.lac,
                                               cmd_ptr->mm_idle_state.cell_id.cell_id,
                                               cmd_ptr->mm_idle_state.cell_id.cell_len,
                                               location_status)) 
      {
        /* No difference */
        MSG_HIGH("No difference in MM info", 0, 0, 0);
        break;
      }
    }
        
    /* valid data, copy over the data */
    memcpy(gstk_next_curr_cell_id.cell_id,
           cmd_ptr->mm_idle_state.cell_id.cell_id,
           (uint32)cmd_ptr->mm_idle_state.cell_id.cell_len);
    gstk_next_curr_cell_id.cell_len = cmd_ptr->mm_idle_state.cell_id.cell_len;
    memcpy(gstk_next_curr_mcc_mnc, cmd_ptr->mm_idle_state.mcc_mnc, GSTK_MAX_PLMN_LEN);
    memcpy(gstk_next_curr_lac, cmd_ptr->mm_idle_state.lac, GSTK_MAX_LAC_LEN);
    gstk_next_curr_location_status = location_status;

    MSG_HIGH("GSTK_EVT_LIST[LOC_STATUS} is %d",gstk_evt_list[GSTK_LOCATION_STATUS],0,0);

    if((gstk_evt_list[GSTK_LOCATION_STATUS] & 0x01) == 0x01) 
    { /* has registered for this event */
      if (!gstk_pending_location_evt_envelope) 
      {
        /* there are location info changes that cause the MM to enter idle */
        /* update the gstk static values */
        MSG_HIGH("No pending envelopes",0,0,0);
        memcpy(gstk_curr_mcc_mnc, gstk_next_curr_mcc_mnc, GSTK_MAX_PLMN_LEN);
        memcpy(gstk_curr_lac, gstk_next_curr_lac, GSTK_MAX_LAC_LEN);
        memcpy(&gstk_curr_cell_id, &gstk_next_curr_cell_id, sizeof(gstk_cell_id_type));
        gstk_curr_location_status = gstk_next_curr_location_status;
        gstk_curr_rat = gstk_next_curr_rat;

        MSG_HIGH("is_rrc_cell_id_received is set to FALSE",0,0,0);
#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */
        /* Send an envelope to the card */
        evt_list.evt_type = GSTK_LOCATION_STATUS_EVT_IND;

        MSG_HIGH("Location status envelope information sent to the card",0,0,0);

        MSG_HIGH(" GSTK_NORMAL_SRVC - 0, GSTK_LMTD_SRVC - 1, GSTK_NO_SRVC - 2",0,0,0);

        MSG_HIGH("  MM->GSTK->Card -- Location status is %d...... ",gstk_curr_location_status ,0,0);
        MSG_HIGH("  MM->GSTK->Card -- MNC_MCC is 0x%x 0x%x 0x%x",
                    gstk_curr_mcc_mnc[0],
                    gstk_curr_mcc_mnc[1],
                    gstk_curr_mcc_mnc[2]);

        MSG_HIGH("  MM->GSTK->Card -- LAC is 0x%x 0x%x",
                    gstk_curr_lac[0],
                    gstk_curr_lac[1],
                    0);

        MSG_HIGH("MM->GSTK->Card -- Cell Id [0][1]: 0x%x 0x%x",
                  gstk_curr_cell_id.cell_id[0],
                  gstk_curr_cell_id.cell_id[1],
                  0);

        MSG_HIGH("MM->GSTK->Card -- Cell Id [2][3]: 0x%x 0x%x",
                  gstk_curr_cell_id.cell_id[2],
                  gstk_curr_cell_id.cell_id[3],
                  0);

        gstk_status = gstk_send_envelope_evt_download_command(
                        GSTK_CLIENT_ID,
                        0,
                        &evt_list);
        if (gstk_status != GSTK_SUCCESS) 
        {
          MSG_ERROR("Have problem sending location status envelope", 0, 0, 0);
        }
        else 
        {
          gstk_pending_location_evt_envelope = TRUE;
        }
      }
    }
    break;

  case GSTK_TIMER_EXP_CMD:
    MSG_HIGH("Received Timer Exp, %x", cmd_ptr->timer_msg.message_header.command_id, 0, 0);

    /* user_data in cmd contains command details ref id */
    gstk_status = gstk_process_term_rsp_timer_expire(
                    cmd_ptr->timer_msg.message_header.user_data);
    if (gstk_status != GSTK_SUCCESS)
    {
      MSG_ERROR("Failed to process timer expiration cmd!", 0, 0, 0);
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
        MSG_ERROR("Power Down err", 0, 0, 0);
    }
    break;

  case GSTK_MODE_CHANGE_IND:
    MSG_HIGH("Received Mode Change ind", 0, 0, 0);
#ifdef FEATURE_GSTK_DUAL_SLOTS
    new_mode_chg_slot = gstk_util_determine_new_pref_from_mode_chg(cmd_ptr->mode_change_ind.cmd.new_mode);

    if (gstk_util_check_proactive_card(new_mode_chg_slot)) 
    {
      /* new slot has tk capability */
      gstk_preferred_tk_slot = new_mode_chg_slot;
    }
    if (new_mode_chg_slot != GSTK_SLOT_NONE) 
    {
      /* sent down mode change command even if the new mode does not have tk capability,
         this will allow us to "reset" curr back to NONE */
      gstk_status = gstk_process_mode_change(new_mode_chg_slot);
      if (gstk_status != GSTK_SUCCESS) 
      {
        MSG_MED("Mode Change ind ERR, %x", gstk_status, 0, 0);
      }           
    }
#endif /* FEATURE_GSTK_DUAL_SLOTS */   

    /* Access Technology Envelope */

    /* If only FEATURE_CCAT is turned on and no MM events are expected, gstk
       can update its current  RAT info based on CM including a case where 
       RAT == NONE
       else
    */
    if ((!gstk_mm_param_received) && 
        (cmd_ptr->mode_change_ind.cmd.access_technology == GSTK_ACCESS_NONE))
    {
      gstk_next_curr_rat = GSTK_ACCESS_NONE;
    }

    /* If FEATURE CCAT and FEATURE GSM/WCDMA are turned on, then gstk allows
       only MM events to update RAT info if RAT == none
       This is to prevent cm events (where RAT == none) from overwriting MM events
       due to timing and other issues
    */
    /* CM updates global gstk rat info only if Access Tech is CDMA */
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
    if(cmd_ptr->mode_change_ind.cmd.access_technology == GSTK_ACCESS_TECH_CDMA)
    {
      gstk_next_curr_rat = cmd_ptr->mode_change_ind.cmd.access_technology;
    }
    if(cmd_ptr->mode_change_ind.cmd.access_technology != GSTK_ACCESS_NONE)
    {
      if ((gstk_evt_list[GSTK_ACCESS_TECH] & 0x01) == 0x01)
      {
        /* Send Envelope Access Technlogy only if the current access technology
            is different from the previously reported access technology envelope
        */
        if(gstk_curr_rat != gstk_next_curr_rat)
        {
          /* Build an envelope to send down the Access Technology change details to the card */
          MSG_HIGH("Building Access Tech Envelope",0,0,0);
          gstk_curr_rat = gstk_next_curr_rat;
          gstk_status = gstk_build_access_technology_envelope(cmd_ptr->mode_change_ind.cmd.access_technology);
          if(gstk_status != GSTK_SUCCESS) 
          {
            MSG_ERROR("Err in building Access Tech Envelope", 0, 0, 0);
          }
        }
      }
    }
    /* Location Status Envelope */
    if((cmd_ptr->mode_change_ind.cmd.access_technology == GSTK_ACCESS_TECH_CDMA) ||
       (cmd_ptr->mode_change_ind.cmd.access_technology == GSTK_ACCESS_NONE))
    {
      if(gstk_util_check_cdma_loc_info_difference(
           cmd_ptr->mode_change_ind.cmd.cdma_loc_info,
           cmd_ptr->mode_change_ind.cmd.location_status) == FALSE)
      {
        break;
      }
      if(cmd_ptr->mode_change_ind.cmd.location_status == GSTK_NORMAL_SERVICE)
      {
        gstk_cache_1x_location_info(cmd_ptr->mode_change_ind.cmd.cdma_loc_info);
      }
      else if(cmd_ptr->mode_change_ind.cmd.location_status == GSTK_NO_SERVICE)
      {
        cdma_location_info_cache.is_loc_info_valid = FALSE;
        memset(&cdma_location_info_cache,0x00,sizeof(cdma_location_info_cache));
      }

      if ((gstk_evt_list[GSTK_LOCATION_STATUS] & 0x01) == 0x01)
      {
        if(!gstk_pending_location_evt_envelope)
        {
         /* Build an envelope to send down the Location change details to the card */
          MSG_HIGH("Building Location Status Envelope",0,0,0);
          /* check for validity of cdma loc status information */
          evt_list.evt_type = GSTK_LOCATION_STATUS_EVT_IND;
          gstk_curr_rat = gstk_next_curr_rat;
          gstk_status = gstk_build_location_status_envelope(
                          cmd_ptr->mode_change_ind.cmd.access_technology,
                          cmd_ptr->mode_change_ind.cmd.location_status);
          if (gstk_status != GSTK_SUCCESS) 
          {
            MSG_ERROR("Have problem sending location status envelope", 0, 0, 0);
          }
          else 
          {
            gstk_pending_location_evt_envelope = TRUE;
          }
        }
      }
    }
    else
    {
      memset(&cdma_location_info_cache, 0x00, sizeof(gstk_cdma_loc_info_cache_type));
    }
#else  /*#ifdef FEATURE_CCAT */
    MSG_HIGH("GSTK_MODE_CHANGE_IND : Feature CCAT not supported",0,0,0);
#endif /*#ifdef FEATURE_CCAT */
#else  /* #ifdef FEATURE_CAT_REL6 */
    MSG_HIGH("GSTK_MODE_CHANGE_IND : FEATURE_CAT_REL6 not supported",0,0,0);
#endif /* #ifdef FEATURE_CAT_REL6 */
    break;      

  case GSTK_RRC_CELL_CHANGE_IND:
#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */
    break;

  case GSTK_PH_NW_SEL_PREF_CHANGE_IND:
#ifdef FEATURE_CAT_REL6
    /* Build an envelope to send down the Access Technology change details to the card */ 
    MSG_HIGH("Building Network Search Mode Change Envelope",0,0,0);
    gstk_status = gstk_build_nw_search_mode_change_envelope(cmd_ptr->ph_nw_sel_pref_ind.ph_nw_sel_pref);
    if(gstk_status != GSTK_SUCCESS) 
    {
      MSG_ERROR("Err in building Network Search Mode Change Envelope", 0, 0, 0);
    }
#else
    MSG_HIGH("GSTK_PH_NW_SEL_PREF_CHANGE_IND: FEATURE_CAT_REL6 not supported",0,0,0);
#endif /* ifdef FEATURE_CAT_REL6 */
    break;  

  default:
    MSG_HIGH("Cmd 0x%x Not handled in GSTK_IDLE_S", 
              cmd_ptr->general_cmd.message_header.command_group, 0, 0);
    break;
  }
  return gstk_status;
} /*gstk_state_idle*/

/*lint -e750 local macro gstk_c not being used */

