/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                   G S T K   T E R M I N A L   P R O F I L E


GENERAL DESCRIPTION
  This source file contains functions for handling terminal profile download



FUNCTIONS
  gstk_uim_terminal_profile_report
    This is the callback function for terminal profile response.

  gstk_cache_tp_info
    This function caches the terminal profile data received from GSTK clients

  gstk_pack_tp_cmd_to_uim
    This function puts the gstk_terminal_profile_cache info into uim format
    and put onto UIM's command queue
                  
  gstk_toolkit_mode_change
    Inform clients that there is a mode change
  
  gstk_process_mode_change
    This function will perform UIM reset and toolkit change based on new CM
    mode if required
                       
INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_term_profile.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/09   xz      Fix the issue of TP DL failing if GSTK fails to get app_id 
                   when parse client TP and simplify TP download procedure
11/24/08   sk      Determine card type as one of the first steps before 
                   processing client terminal profile updates.
10/22/08   sk      Handle tp dl from multiple clients
                   Removed gstk_terminal_profile_data_from_client
09/14/08   sk      Fixed lint errors
08/06/08   xz      Add debug info of if default or updated term profile is sent
02/06/08   sk      Lint fixes
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
10/23/06   sk      Fixed lint errors.
05/17/06   sk      Lint fixes
11/14/05   sk      Fixed lint errors
10/18/05   tml     Added card apps
05/11/05   sst     Lint fixes
01/07/05   jk      Fixed Compiler warnings
10/01/04   tml     Change function prototype for terminal profile caching
08/30/04   tml     Added gstk_profile_received
06/29/04   tml     Added fake imsi support for reset the new slot after mode
                   change
06/28/04   tml     Added fake imsi support
06/10/04   tml     Added terminal profile version support
05/26/04   tml     Set the terminal profile bits for Get Inkey and Setup Call
03/05/04   tml     Allowed single slot to operate individually in Dual Slot 
                   solution
09/04/03   tml     dual slot support
07/07/03   tml     Removed unnecessary messages
05/16/03   tml     linted
04/21/03   tml     Added UTK support
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "gstk_term_profile.h"
#include "gstkutil.h"
#include "err.h"
#include "string.h" 

/*===========================================================================
FUNCTION gstk_uim_terminal_profile_report

DESCRIPTION

  This is the callback function for terminal profile response.  UIM will call
  this function when it receives terminal profile response from the Card
  Based on the return status of the terminal profile response, this function
  will build the gstk_term_profile_rsp_type and set GSTK_CMD_Q_SIG so that
  this command can be processed in the gstk state machine

PARAMETERS
  report: [Input] Pointer to the UIM terminal profile report

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
void gstk_uim_terminal_profile_report (uim_rpt_type *report)
{
    gstk_task_cmd_type   * task_cmd;

    if(report == NULL)
    {
      MSG_ERROR("report ptr ERR:NULL",0,0,0);
      return;
    }

    MSG_HIGH("GSTK recv terminal profile uim report, 0x%x, 0x%x", 
              report->sw1, report->sw2, 0);

    if (report->rpt_type != UIM_TERMINAL_PROFILE_R) {
        // build error message to GSTK?????????????
        ERR("TP expected in CB", 0, 0, 0);
        return;
    }


    /* get command buf */
    task_cmd = gstk_task_get_cmd_buf();
    if ( task_cmd != NULL ) { /* successfully obtained command buffer */
        /* build the terminal profile response */
        task_cmd->cmd.terminal_profile_rsp.message_header.command_group = GSTK_TERMINAL_PROFILE_RSP;
        task_cmd->cmd.terminal_profile_rsp.message_header.command_id = (int)GSTK_PROFILE_DL_IND_RSP;
        task_cmd->cmd.terminal_profile_rsp.message_header.user_data = report->user_data;
        if (report->rpt_status == UIM_PASS) {
            task_cmd->cmd.terminal_profile_rsp.success = TRUE;
        }
        else {
            task_cmd->cmd.terminal_profile_rsp.success = FALSE;
        }
        /* set GSTK_CMD_Q_SIG and put on GSTK queue */
        gstk_task_put_cmd_buf(task_cmd);
    }
    else {
        // build error message to GSTK?????????????
        ERR("No more gstk buf", 0, 0, 0);
    }
}/*lint !e818 *report suppression for externalized function */
/* gstk_uim_terminal_profile_report */

#ifdef FEATURE_GSTK_DUAL_SLOTS
/*===========================================================================
FUNCTION gstk_uim_terminal_profile_reset_report

DESCRIPTION

  This is the callback function for terminal profile with reset response.  
  UIM will upon finishing the terminal profile with reset

PARAMETERS
  report: [Input] Pointer to the UIM terminal profile report

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
void gstk_uim_terminal_profile_reset_report (uim_rpt_type *report)
{

    MSG_HIGH(" GSTK received terminal profile uim report", 0 , 0, 0);

    if (report->rpt_type != UIM_RESET_WITH_TP_R) {
        // build error message to GSTK?????????????
        ERR("RESET with TP expected in CB", 0, 0, 0);
        return;
    }
    /* Do nothing */
}/* gstk_uim_terminal_profile_reset_report */

#endif /* FEATURE_GSTK_DUAL_SLOTS */

/*===========================================================================
FUNCTION gstk_cache_tp_info

DESCRIPTION
  This function caches the terminal profile data received from GSTK clients

PARAMETERS
  term_profile_ptr: [Input] Mobile Terminal Profile info from GSTK client

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
gstk_status_enum_type gstk_cache_tp_info(
    gstk_cmd_type  *term_profile_ptr )
{
    uint8   idx   = gstk_num_of_tp_dl_received - 1;
    uint32  size  = sizeof(gstk_terminal_profile_cmd_type);

    if(term_profile_ptr == NULL)
    {
      MSG_ERROR("term_profile_ptr is NULL",0,0,0);
      return GSTK_ERROR;
    }
    if(term_profile_ptr->terminal_profile_cmd.length > GSTK_MAX_TP_DATA_SIZE)
    {
      MSG_HIGH("term profile len 0x%x > GSTK_MAX_TP_DATA_SIZE",
               term_profile_ptr->terminal_profile_cmd.length,0,0);
      term_profile_ptr->terminal_profile_cmd.length = GSTK_MAX_TP_DATA_SIZE;
    }

    if (idx >= GSTK_EXPECTED_NUM_OF_TP_DL)
    {
      MSG_ERROR("Too many TPs are received! (0x%x > 0x%x)",
                idx, GSTK_EXPECTED_NUM_OF_TP_DL, 0);
      return GSTK_ERROR;
    }
    
    gstk_priv_data.client_tp_ptr[idx] = gstk_malloc(size);
    if (!gstk_priv_data.client_tp_ptr[idx])
    {
      MSG_ERROR("failed to allocate mem for TP cache!", 0, 0, 0);
      return GSTK_MEMORY_ERROR;
    }

    gstk_memcpy(gstk_priv_data.client_tp_ptr[idx],
                term_profile_ptr,
                size,
                size,
                size);

    return GSTK_SUCCESS;
}/* gstk_cache_tp_info */

/*===========================================================================
FUNCTION gstk_update_tp_dl_cache

DESCRIPTION
  This function updates the terminal profile cache if the client ui
  client differentiates between sat and usat tp dl

PARAMETERS
  term_profile: [Input] Mobile Terminal Profile info from GSTK client or from 
                GSTK itself (once the card type has been determined)

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
gstk_status_enum_type gstk_update_tp_dl_cache(
    gstk_terminal_profile_cmd_type  *term_profile_ptr )
{
    int                   i             = 0;
    uint32                byte_offset   = 0;
    uint8                 bit_offset    = 0;
    uint8                 bit_shift     = 0;
    uint32                location_mask = 0;
    gstk_status_enum_type gstk_status   = GSTK_SUCCESS;
    gstk_app_enum_type    app_type      = GSTK_APP_NONE;

    if(term_profile_ptr == NULL)
    {
        MSG_ERROR("term_profile_ptr is NULL",0,0,0);
        return GSTK_ERROR;
    }

    /* update the gstk_profile_received to TRUE */
    gstk_profile_received = TRUE;
    gstk_get_and_store_curr_app_type();

    while (i < (int)term_profile_ptr->length) 
    {
        app_type      = (gstk_app_enum_type)term_profile_ptr->data[i++];
        byte_offset   = term_profile_ptr->data[i++];
        bit_offset    = term_profile_ptr->data[i++];
        location_mask = ((byte_offset << 0x0008) | bit_offset);
        byte_offset--;

        /* Find the number of bits needed to be shifted in the data before bitwise and with
        ** the gstk_Terminal_profile_cache */
        bit_shift = 0;
        while (bit_offset > 0 ) 
        {
            bit_offset = bit_offset >> 0x01;
            bit_shift++;
        }
        bit_shift--;

        if(app_type == GSTK_APP_NONE)
        {
            switch(location_mask) {
            case GSTK_SET_UP_CALL_SUPPORT:
                term_profile_ptr->data[i] &= 0x01;
                gstk_terminal_profile_cache[byte_offset] |= 
                  (uint8)(term_profile_ptr->data[i] << bit_shift);
                gstk_terminal_profile_cache[7] |= 
                  uint16touint8(term_profile_ptr->data[i] << 6);
                break;
            case GSTK_GET_INKEY_SUPPORT:
                term_profile_ptr->data[i] &= 0x01;
                gstk_terminal_profile_cache[byte_offset] |= 
                  (uint8)(term_profile_ptr->data[i] << bit_shift);
                gstk_terminal_profile_cache[7] |= 
                  uint16touint8(term_profile_ptr->data[i] << 3);
                break;
            case GSTK_DISPLAY_TEXT_SUPPORT:
                term_profile_ptr->data[i] &= 0x01;
                gstk_terminal_profile_cache[byte_offset] |= 
                  (uint8)(term_profile_ptr->data[i] << bit_shift);
                gstk_terminal_profile_cache[8] |= 
                  term_profile_ptr->data[i];
                break;
            case GSTK_NUM_CHAR_SUPPORT_DOWN_ME: /* 5 bits of info */
                term_profile_ptr->data[i] &= 0x1F;
                bit_shift -= 0x04;
                gstk_terminal_profile_cache[byte_offset] |= 
                  (uint8)(term_profile_ptr->data[i] << bit_shift);
                break;
            case GSTK_NUM_CHAR_SUPPORT_ACROSS_ME:    /* 7 bits of info */
                term_profile_ptr->data[i] &= 0x7F;
                bit_shift -= 0x06;
                gstk_terminal_profile_cache[byte_offset] |= 
                  (uint8)(term_profile_ptr->data[i] << bit_shift);
                break;
            case GSTK_WIDTH_REDUCE_IN_MENU_SUPPORT: /* 3 bits of info */
                term_profile_ptr->data[i] &= 0x07;
                bit_shift -= 0x02;
                gstk_terminal_profile_cache[byte_offset] |= 
                  (uint8) (term_profile_ptr->data[i] << bit_shift);
                break;
            case GSTK_NUM_CH_SUPPORT: /* 3 bits of info */
                term_profile_ptr->data[i] &= 0x07;
                bit_shift -= 0x02;
                gstk_terminal_profile_cache[byte_offset] |= 
                  (uint8) (term_profile_ptr->data[i] << bit_shift);
                break;
            case GSTK_MAX_SK_SUPPORT: /* 8 bits of info */
                term_profile_ptr->data[i] &= 0xFF;
                bit_shift -= 0x07;
                gstk_terminal_profile_cache[byte_offset] |= 
                  (uint8) (term_profile_ptr->data[i] << bit_shift);
                break;
            case GSTK_PROTOCOL_VERSION: /* 4 bits of info */
                term_profile_ptr->data[i] &= 0x0F;
                bit_shift -= 0x03;
                gstk_terminal_profile_cache[byte_offset] |= 
                  (uint8) (term_profile_ptr->data[i] << bit_shift);
                break;
            default: /* 1, 0 values */
                term_profile_ptr->data[i] &= 0x01;
                gstk_terminal_profile_cache[byte_offset] |= 
                  (uint8) (term_profile_ptr->data[i] << bit_shift);
                break;
            }
        }
        if((app_type == gstk_curr_app_type) && (gstk_curr_app_type ==  GSTK_APP_USIM))
        {
            switch(location_mask) 
            {
            /* USAT */
            case GSTK_USAT_SMS_PP_DL_SUPPORT_BYTE1_BIT_5:
            case GSTK_PLI_NMR_UTRAN:
            case GSTK_USAT_CC_SUPPORT_BYTE_1_BIT_7:
            case GSTK_USAT_CC_SUPPORT_BYTE_2_BIT_5:
            case GSTK_USAT_CC_SUPPORT_BYTE_1_BIT_8:
            case GSTK_USAT_DISPLAY_SUPPORT_BYTE_2_BIT_8:
            case GSTK_USAT_DISP_TEXT_SUPPORT_BYTE_9_BIT_1:
            case GSTK_USAT_SETUP_CALL_SUPPORT_BYTE_8_BIT_7:
            case GSTK_USAT_CC_SUPPORT_BYTE_8_BIT_8:
                term_profile_ptr->data[i] &= 0x01;
                gstk_terminal_profile_cache[byte_offset] |= 
                  (uint8) (term_profile_ptr->data[i] << bit_shift);
                break;
            default:
                break;
            }
        }
        else if((app_type == gstk_curr_app_type) && (gstk_curr_app_type ==  GSTK_APP_SIM))
        {
            switch(location_mask) 
            {
            case GSTK_9EXX_RSP_CODE_SUPPORT:
            case GSTK_SUPPORT_FOR_ALPHA_IN_CC:
            case GSTK_USSD_DO_IN_CC_SUPPORT:
            case GSTK_CC_SUPPORT:
            case GSTK_SECOND_ALPHA_IN_SETUP_CALL_SUPPORT:
            case GSTK_PLI_NMR_BCCH:
            case GSTK_CC_ALWAYS_SENT_DURING_REDIAL:
            case GSTK_DISPLAY_OF_EXTN_TEXT:
            case GSTK_SUSTAINED_DISP_TEXT_SUPPORT:
            case GSTK_SECOND_CAP_CONFIG_PARAM_SUPPORT:
                term_profile_ptr->data[i] &= 0x01;
                gstk_terminal_profile_cache[byte_offset] |= 
                  (uint8) (term_profile_ptr->data[i] << bit_shift);
                break;
            default:
                break;
            }
        }
        i++;
    }
    return gstk_status;
} /* gstk_update_tp_dl_cache */


/*===========================================================================
FUNCTION gstk_pack_tp_cmd_to_uim

DESCRIPTION

  This function puts the gstk_terminal_profile_cache info into uim format
  and put onto UIM's command queue

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
gstk_status_enum_type gstk_pack_tp_cmd_to_uim (void)
{
    gstk_status_enum_type           gstk_status      = GSTK_SUCCESS;
    uim_cmd_type                   *uim_cmd_ptr      = NULL;
    uint8                           i                = 0;

    MSG_HIGH("Sending card specific terminal profile",0,0,0);

    /* update all cached TPs from clients to gstk_terminal_profile_cache*/
    for (i = 0; i < GSTK_EXPECTED_NUM_OF_TP_DL; ++ i)
    {
      if (gstk_priv_data.client_tp_ptr[i])
      {
        gstk_update_tp_dl_cache(gstk_priv_data.client_tp_ptr[i]);
        gstk_free(gstk_priv_data.client_tp_ptr[i]);
        gstk_priv_data.client_tp_ptr[i] = NULL;
      }
    }
    
    /* dump TP contents*/
    gstk_util_dump_byte_array("gstk_terminal_profile_cache",
                              gstk_terminal_profile_cache,
                              GSTK_TERMINAL_PROFILE_SIZE);
    
    /* Send terminal response to STK application on SIM */
    /* Send Message to UIM */
    /* get a buffer for the request */
    uim_cmd_ptr = gstk_get_uim_buf();
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_PROFILE_F, &uim_cmd_ptr);
    if (uim_cmd_ptr != NULL) {
     if (gstk_status == GSTK_SUCCESS) {
           /* set user_data */
           uim_cmd_ptr->hdr.user_data               = 0;
           /* populate the terminal_response info */
           uim_cmd_ptr->terminal_profile.num_bytes = UIM_TERMINAL_PROFILE_DATA_LENGTH;
           memcpy(uim_cmd_ptr->terminal_profile.data, 
                  gstk_terminal_profile_cache, 
                  UIM_TERMINAL_PROFILE_DATA_LENGTH);   
#ifdef FEATURE_GSTK_DUAL_SLOTS
           if (gstk_preferred_tk_slot == GSTK_SLOT_1) {
               uim_cmd_ptr->hdr.slot = UIM_SLOT_1;
           }
           else if (gstk_preferred_tk_slot == GSTK_SLOT_2) {
               uim_cmd_ptr->hdr.slot = UIM_SLOT_2;    
           }  
#endif

           gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
     else {
       MSG_MED("Never here", 0, 0, 0);
     }
    }
    return gstk_status;

} /*gstk_pack_tp_cmd_to_uim*/

#ifdef FEATURE_GSTK_DUAL_SLOTS

/*===========================================================================
FUNCTION gstk_toolkit_mode_change

DESCRIPTION
  This function notifies all gstk clients regarding a mode change.
  
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
gstk_status_enum_type gstk_toolkit_mode_change(void)
{
    gstk_status_enum_type               gstk_status = GSTK_SUCCESS;
    gstk_cmd_from_card_type             request;

    MSG_HIGH("IN toolkit mode change ",0,0,0);

    /* Message Header */
    request.hdr_cmd.command_id = GSTK_TOOLKIT_MODE_CHANGE_REQ;
    request.hdr_cmd.cmd_detail_reference = 0;

    /* Send command to all clients */
    gstk_status = gstk_util_send_message_to_clients(&request);

    if (gstk_status != GSTK_SUCCESS) {
      MSG_HIGH("Can't SEND Toolkit mode change to clients", 0, 0, 0);
    }
    return gstk_status;

} /* gstk_toolkit_mode_change */


/*===========================================================================
FUNCTION gstk_process_mode_change

DESCRIPTION
  This function processes a mode change and sends reset commands to UIM 
  if needed.

PARAMETERS
  new_slot [Input] - slot to change the toolkit sessino to

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
gstk_status_enum_type gstk_process_mode_change( gstk_slot_id_enum_type  new_slot )
{
    gstk_additional_info_type           additional_info;
    gstk_terminal_rsp_extra_param_type  dummy_extra_param;
    uim_cmd_type*                       uim_cmd_ptr;
    gstk_status_enum_type               gstk_status = GSTK_SUCCESS;
    int                                 i;


    /* Check if new slot is the same as curr slot */
    if (gstk_curr_tk_slot == new_slot) {
      /* check if the slot is tk capable, if so, nothing need
         to be done, if not, need to end the proactive session for
         the current slot, scenario that causes this stage is e.g.
         preferred slot was 1, curr was 1, 1 turns bad, preferred slot
         becomes 2, mode change command comes and want to switch to 1, 
         the bad slot */
      if (gstk_util_check_proactive_card(gstk_curr_tk_slot)) {
        return gstk_status;
      }
    }
    /* Tell clients to terminate commands */
    gstk_status = gstk_toolkit_mode_change();
     
    if (gstk_status != GSTK_SUCCESS) {
        return gstk_status;
    }

    /* 
    ** If the current slot has an active toolkit session, 
    ** end pending proactive commands and reset without
    ** terminal profile download.
    */
    if ( gstk_util_check_proactive_card(gstk_curr_tk_slot) ) {
        additional_info.length = 0;
        dummy_extra_param.present = FALSE;

        /* check for proactive commands */
        for (i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; i++) {
            if (!command_details_ref_table[i].free_slot) {
                /* send terminal response to UIM */
                gstk_status = gstk_send_terminal_response(
                    GSTK_CLIENT_ID, /* client ID */
                    0,              /* user data */
                    i,              /* command_details_ref */
                    command_details_ref_table[i].command_details.command_number, /* cmd num */
                    command_details_ref_table[i].expected_cmd_rsp_type, /* command response */
                    GSTK_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER,      /* general result */
                    &additional_info,                                   /* additional result */
                    &dummy_extra_param);                                /* extra param */
                if (gstk_status != GSTK_SUCCESS) {
                    ERR("Not able to send Terminal Rsp during mode switch!", 0, 0, 0);
                    return gstk_status;
                }
            }
        }


        /* get command buffer from UIM */
        uim_cmd_ptr = gstk_get_uim_buf();
        gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_RESET_WITH_TP_F, &uim_cmd_ptr);
        if (uim_cmd_ptr != NULL ) {
            uim_cmd_ptr->reset_with_tp.terminal_profile = FALSE;
#if defined( FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY ) 
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY  */
            
            gstk_send_cmd_to_uim_server(uim_cmd_ptr);

            /* update toolkit slot */
            gstk_curr_tk_slot = GSTK_SLOT_NONE;
        } /* uim_cmd_ptr != NULL */
        else {
            ERR ("no UIM cmd buf", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
        }
    } /* check current slot */
    else {
        /* if curr doesn't have toolkit capability */
        gstk_curr_tk_slot = GSTK_SLOT_NONE;
    }

    /* 
    ** If the new slot has a proactive card, reset with  
    ** terminal profile download.
    */
    if ( gstk_util_check_proactive_card(new_slot) ) {
        if (new_slot != gstk_preferred_tk_slot) {
            /* flag an error since new slot is proactive but the preferred tk is not set to this card */
            MSG_ERROR("Preferred %x and new slot %x mismatch", gstk_preferred_tk_slot, new_slot, 0);
        }
        /* get command buffer from UIM */
        uim_cmd_ptr = gstk_get_uim_buf();
        gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_RESET_WITH_TP_F, &uim_cmd_ptr);
        if (uim_cmd_ptr != NULL ) {
            /* over write the slot with new slot */
            uim_cmd_ptr->hdr.slot = new_slot;
            uim_cmd_ptr->reset_with_tp.terminal_profile = TRUE;

#if defined( FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY ) 
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY  */
 
            gstk_send_cmd_to_uim_server(uim_cmd_ptr);

            /* update toolkit slots */
            gstk_curr_tk_slot = gstk_preferred_tk_slot;
            
            if (gstk_profile_received)
            {
              MSG_HIGH("updated terminal profile is sent to card.", 0, 0, 0);
            }
            else
            {
              MSG_HIGH("default terminal profile is sent to card.", 0, 0, 0);
            }
            
        } /* uim_cmd_ptr != NULL */
        else {
            ERR ("no UIM cmd buf", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
        }
    } /* check proactive new_slot */

    return gstk_status;
} /* gstk_process_mode_change */                  
#endif /* FEATURE_GSTK_DUAL_SLOTS */

