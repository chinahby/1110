#ifndef GSTK_STATE_C
#define GSTK_STATE_C


/*===========================================================================


                           G S T K       S T A T E


GENERAL DESCRIPTION
  This source file contains function to handle GSTK main state machine.


FUNCTIONS
  gstk_process_state
    This function dispatches the command to the current state of the GSTK state
    machine.
    
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

Copyright (c) 2003-2004 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_state.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/07/05   jk      Fixed Compiler warnings
03/05/04   tml     Allowed single slot to operate individually in Dual Slot 
                   solution
09/04/03   tml     Dual slot support
05/16/03   tml     removed power down state function call, linted
02/07/03   tml     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk_state.h"
#include "gstk_term_profile.h"
#include "msg.h"
#include "err.h"
#ifdef FEATURE_GSTK_DUAL_SLOTS 
#include "gstkutil.h"
#endif /* FEATURE_GSTK_DUAL_SLOTS */

/* hold the current GSTK gstk */
gstk_state_enum_type gstk_state;

/*===========================================================================
FUNCTION GSTK_PROCESS_STATE

DESCRIPTION

  This function dispatches the command to the current state of the GSTK state
  machine.

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed by various the
                 GSTK state machine

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
gstk_status_enum_type gstk_process_state (
        gstk_cmd_type            * cmd_p )
{
    gstk_status_enum_type  gstk_status = GSTK_SUCCESS;

    /* Send the command to the current state for processing */
    switch(gstk_state) {
    case GSTK_START_S: /* Start state */
        gstk_status = gstk_state_start(cmd_p);
        break;
    
    case GSTK_TERM_PROFILE_WAIT_S: /* waiting for term profile from client state */
        gstk_status = gstk_state_tp_wait(cmd_p);
        break;

    case GSTK_GSDI_SIG_WAIT_S: /* waiting for gsdi to signal term profile process state */
        gstk_status = gstk_state_gsdi_sig_wait(cmd_p);
        break;

    case GSTK_TERM_PROFILE_RSP_WAIT_S: /* waiting for response from uim in
                                          regards to term profile state */
        gstk_status = gstk_state_tp_rsp_wait(cmd_p);
        break;

    case GSTK_IDLE_S: /* idle state */
        gstk_status = gstk_state_idle(cmd_p);
        break;

    case GSTK_NO_SIM_S: /* no sim state */
#ifdef FEATURE_GSTK_DUAL_SLOTS
        gstk_curr_tk_slot = GSTK_SLOT_NONE;
#endif /* FEATURE_GSTK_DUAL_SLOTS */
        gstk_status = gstk_state_no_sim(cmd_p);
        break;

    case GSTK_POWER_DOWN_S: /* power down state */
        MSG_MED("Ignore cmd for pwr dn", 0, 0, 0);
        break;
    }
    return gstk_status;
}
           

#ifdef FEATURE_GSTK_DUAL_SLOTS     

/*===========================================================================
FUNCTION gstk_state_have_no_toolkit_supports

DESCRIPTION
  This function checks if we have toolkit supports from either of the card 

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_state_have_no_toolkit_supports()
{
  /* no slot 1 or slot 2 */
  if (((gstk_toolkit_info & GSTK_SLOT1_CARD_ERROR_MASK) 
         == GSTK_SLOT1_CARD_ERROR_MASK) &&
      ((gstk_toolkit_info & GSTK_SLOT2_CARD_ERROR_MASK) 
         == GSTK_SLOT2_CARD_ERROR_MASK)) {
    return TRUE;
  }
  /* no slot 1, slot 2 has no TP */
  if (((gstk_toolkit_info & GSTK_SLOT1_CARD_ERROR_MASK) 
         == GSTK_SLOT1_CARD_ERROR_MASK) &&
      ((gstk_toolkit_info & GSTK_SLOT2_INITIALIZED_MASK) 
         == GSTK_SLOT2_INITIALIZED_MASK) &&
      ((gstk_toolkit_info & GSTK_SLOT2_TK_CAPABLE_MASK) 
         != GSTK_SLOT2_TK_CAPABLE_MASK)) {
    return TRUE;
  }
  /* no slot 2, slot 1 has no TP */
  if (((gstk_toolkit_info & GSTK_SLOT2_CARD_ERROR_MASK) 
         == GSTK_SLOT2_CARD_ERROR_MASK) &&
      ((gstk_toolkit_info & GSTK_SLOT1_INITIALIZED_MASK) 
         == GSTK_SLOT1_INITIALIZED_MASK) &&
      ((gstk_toolkit_info & GSTK_SLOT1_TK_CAPABLE_MASK) 
         != GSTK_SLOT1_TK_CAPABLE_MASK)) {
    return TRUE;
  }
  /* slot 1, slot 2 have no TP */
  if (((gstk_toolkit_info & GSTK_SLOT1_INITIALIZED_MASK) 
        == GSTK_SLOT1_INITIALIZED_MASK) &&
      ((gstk_toolkit_info & GSTK_SLOT1_TK_CAPABLE_MASK) 
        != GSTK_SLOT1_TK_CAPABLE_MASK) &&
      ((gstk_toolkit_info & GSTK_SLOT2_INITIALIZED_MASK) 
        == GSTK_SLOT2_INITIALIZED_MASK) &&
      ((gstk_toolkit_info & GSTK_SLOT2_TK_CAPABLE_MASK) 
        != GSTK_SLOT2_TK_CAPABLE_MASK)) {
    return TRUE;
  }
  return FALSE;
} /* gstk_state_have_no_toolkit_supports */

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
gstk_status_enum_type gstk_start_toolkit_session ( void )

{
    gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
         
    if (gstk_preferred_tk_slot == GSTK_SLOT_NONE) {
        MSG_MED("Preferred tk slot == NULL", 0, 0, 0);
        return GSTK_ERROR;
    }

    if (gstk_curr_tk_slot != GSTK_SLOT_NONE) {
        ERR("first TP has curr_slot = %d (but != SLOT_NONE)!!!", gstk_curr_tk_slot,0 , 0);
        return GSTK_ERROR;
    }

    /* check for pending mode change */
    /* process mode change is successful and the new slot is not NONE, 
        therefore, we have to do a terminal profile download */
    /* use the gstk_terminal_profile_cache info and send it to uim */   
    gstk_curr_tk_slot = gstk_preferred_tk_slot;
    gstk_status = gstk_pack_tp_cmd_to_uim();

    if (gstk_status == GSTK_SUCCESS) {
        /* go to wait for TP response state */          
        gstk_state = GSTK_TERM_PROFILE_RSP_WAIT_S;
    }
    else {
        /* reset curr_tk_slot */
        gstk_curr_tk_slot = GSTK_SLOT_NONE;
        ERR("Send TP error", 0, 0, 0);
    }
    return gstk_status;
} /*gstk_start_toolkit_session*/

/*===========================================================================
FUNCTION gstk_state_is_slot_initialized

DESCRIPTION
  This function checks if the slot passed in from the parameter field is
  initialized or not

PARAMETERS
  slot: [Input] - Slot the check is to be performed on 

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_state_is_slot_initialized(gstk_slot_id_enum_type slot) {
  uint32 error_mask;
  uint32 init_mask;
  switch (slot) {
    case GSTK_SLOT_1:
      error_mask = GSTK_SLOT1_CARD_ERROR_MASK;
      init_mask = GSTK_SLOT1_INITIALIZED_MASK;
      break;
    case GSTK_SLOT_2:
      error_mask = GSTK_SLOT2_CARD_ERROR_MASK;
      init_mask = GSTK_SLOT2_INITIALIZED_MASK;
      break;
    default:
      MSG_ERROR("Unknown slot %x", slot, 0, 0);
      return FALSE;
  }
  /* the slot has not been initialized */
  if (((gstk_toolkit_info & error_mask) != error_mask) &&
       ((gstk_toolkit_info & init_mask) != init_mask)) 
  {
    return FALSE;
  }
  return TRUE;
} /*gstk_state_is_slot_initialized */

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
void gstk_state_determine_action_after_gsdi_evt(gstk_toolkit_cap_enum_type *toolkit_cap)
{

    if ((gstk_state_is_slot_initialized(GSTK_SLOT_1)) &&
        (gstk_state_is_slot_initialized(GSTK_SLOT_2))){
        /* both slot has been initialized */
        /* check for zero toolkit supports from both slot */
        /* If there is no toolkit supports available, 
        */           
        if(gstk_state_have_no_toolkit_supports()) {
          *toolkit_cap = GSTK_NO_TK_CAPABLE;
          gstk_preferred_tk_slot = GSTK_SLOT_NONE;
        }
        else {
          /* at least 1 toolkit slot or both slots have
           * toolkit support and have already initialized */
          if ((gstk_util_check_proactive_card(GSTK_SLOT_1)) &&
              (gstk_util_check_proactive_card(GSTK_SLOT_2))) {
            /* both slots are good */
            if (gstk_preferred_tk_slot == GSTK_SLOT_NONE) {
              /* if there hasn't any assignment in preferred slot
               * default to the first slot */
              gstk_preferred_tk_slot = GSTK_SLOT_1;
            }
            else {
              /* already has a preferred tk slot */
              /* need to turn the non curr one for uim proactive and
                 notify gsdi as well */
              gsdi_util_gstk_tp_dl_complete ();
#ifndef FEATURE_VIRTUAL_SIM
              switch (gstk_preferred_tk_slot) {
                case GSTK_SLOT_1:
                  /* set the other card (slot 2) to be proactive as well */
                  uim_set_proactive_uim_2(TRUE);
                  break;
                case GSTK_SLOT_2:
                  /* set the other card (slot 1) to be proactive as well */
                  uim_set_proactive_uim(TRUE);
                  break;
              }
#endif /*FEATURE_VIRTUAL_SIMT */
            }
            *toolkit_cap = GSTK_BOTH_TK_CAPABLE;
          }
          else {
            /* only 1 slot is good */
            if ((!gstk_util_check_proactive_card(GSTK_SLOT_1)) &&
                (gstk_util_check_proactive_card(GSTK_SLOT_2)))
            {
              /* good slot 2 */
              gstk_preferred_tk_slot = GSTK_SLOT_2;
            }
            else if ((!gstk_util_check_proactive_card(GSTK_SLOT_2)) &&
                     (gstk_util_check_proactive_card(GSTK_SLOT_1)))
            {
              /* good slot 1 */
              gstk_preferred_tk_slot = GSTK_SLOT_1;
            }
            *toolkit_cap = GSTK_ONE_TK_CAPABLE;
          }
        }
    }
    else if ((!gstk_state_is_slot_initialized(GSTK_SLOT_1)) &&
             (!gstk_state_is_slot_initialized(GSTK_SLOT_2))) {
        /* both slots are not initilized */
        /* go back to gsdi sig wait state and reset parameters */
        *toolkit_cap = GSTK_UNKNOWN_TK_CAPABLE;
        gstk_preferred_tk_slot = GSTK_SLOT_NONE;
    }
    else {
        /* at least 1 is initialized */

        /* check if the slot 1 is initialized or not */
        if (gstk_state_is_slot_initialized(GSTK_SLOT_1)) {
          if (gstk_util_check_proactive_card(GSTK_SLOT_1)) {
            gstk_preferred_tk_slot = GSTK_SLOT_1;
            *toolkit_cap = GSTK_ONE_TK_CAPABLE;
          }
          else { /* bad slot 1 and slot 2 not initialized */
            gstk_preferred_tk_slot = GSTK_SLOT_NONE;
            *toolkit_cap = GSTK_UNKNOWN_TK_CAPABLE;
          }
        }
        else { /* slot 2 is initialized */
          if (gstk_util_check_proactive_card(GSTK_SLOT_2)) {
            gstk_preferred_tk_slot = GSTK_SLOT_2;
            *toolkit_cap = GSTK_ONE_TK_CAPABLE;
          }
          else { /* bad slot 2 and slot 1 not initialized */
            gstk_preferred_tk_slot = GSTK_SLOT_NONE;
            *toolkit_cap = GSTK_UNKNOWN_TK_CAPABLE;
          }
        }
    }
} /* gstk_state_determine_action_after_gsdi_evt */
#endif /*FEATURE_GSTK_DUAL_SLOTS */

#endif  /* GSTK_STATE_C */
/*lint -e750 local macro gstk_c not being used */


