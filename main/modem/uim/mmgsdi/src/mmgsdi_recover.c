/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   R E C O V E R Y    F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the functions to support recovering UIM module
  after it has internally reset the card.

EXTERNALIZED FUNCTIONS
  mmgsdi_recover_after_reset


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_recover.c#8 $$ $DateTime: 2011/03/25 06:51:16 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/11   ssr     Fixed the MEID cache
12/24/09   rm      Fix to correct the logic for recovery attempts when 
                   recovery is triggered before SIM Initialization
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
04/21/09   kk      Clear RESET_COMPLETE_SIG before sending power down in case
                   recovery fails
04/07/09   xz      Resume GSTK task while in recovery
02/03/09   ssr     Read from NV_MEID_ME_I to ensure that MEID value,
                   should be come form NV memory, not from the card.
11/18/08   js      Updated featurization for Send APDU related code
11/15/08   js      Removed MFLO featurization for Send APDU related code
10/09/08   tml     Fixed compilation warnings
11/08/07   vs/nk   Added EUIMID feature support. Updated mmgsdi_store_esn_synch()
                   to check n9 independent of n8 and to return the ESN if
                   n9 is disabled
05/21/07   jk      Make extra checks for reset sig during recovery
02/01/07   pv      Wait for GSDI_UIM_RESET_COMPLETE_SIG as well while waiting
                   for GSTK to complete TP-DL.  Use pin_data.data_len instead
                   of valid_pin.
02/01/07   jk      Changes to prevent warnings when FEATURE_GSTK is off
10/26/06   tml     Get Key ref from client ID
10/02/06   sp      Changed me supports fdn from gsdi to mmgsdi global data
08/23/06   sun     Fixed Lint Errors
08/21/06   tml     Lint fixes
08/07/06   sun     Moved PIN functions from gsdi to mmgsdi
08/09/06   pv      Featurize the usage of members of nv_uim_config_params_type
                   under FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS for
                   targets not picking up the NV dependencies
07/28/06   tml     lint fix
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
07/13/06   jar     Modifications for PLATFORM_LTK Builds.  Fixed checkin
                   date for last checkin
07/12/06   pv      Change break to continue in mmgsdi_recover_after_reset so
                   that if UIM did reset during the time, the signal would
                   be cleared and re-tried.
06/15/06   tml     Fixed compilation warning
06/12/06   pv      Initial Revision.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmgsdi_recover.h"
#include "mmgsdiutil.h"
#include "intconv.h"
#include "uim.h"
#include "gsdi.h"
#include "mmgsdi_gen.h"

#ifdef FEATURE_MMGSDI_CDMA
#include "tmc.h"
#endif /* FEATURE_MMGSDI_CDMA */

#ifdef FEATURE_UIM_EUIMID
#include "gsdi_nv.h"
#endif /* FEATURE_UIM_EUIMID */

#ifdef FEATURE_GSTK
#include "gstk.h"
#endif /* FEATURE_GSTK */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Timer that would be used to wait for gstk to complete TP DL */
extern rex_timer_type gsdi_gstk_timer;

#if !defined(PLATFORM_LTK) && !defined(FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS)
/* Varible to hold configurable parameters stored in the NV */
extern nv_uim_config_params_type uim_config_params;
#endif /* !PLATFORM_LTK && !FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS*/

#ifdef FEATURE_GSTK
/* Declaration for external function which is registered with GSDI
 * for receiving gsti events.  This is needed to notify GSTK
 * about the internal reset (only to GSTK).
 */
extern void gstk_gsdi_sim_events_cb (
    gsdi_sim_events_T   gsdi_event
);
#endif /*FEATURE_GSTK*/

/* Number of times mmgsdi would try to recover for each reset */
#define MAX_RECOVERY_ATTEMPTS 3

#ifdef FEATURE_MMGSDI_CDMA

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_STORE_ESN_SYNCH

   DESCRIPTION:
     This function sends down a store esn command to the uim synchronously

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_store_esn_synch(
  mmgsdi_slot_id_enum_type slot
)
{
  uim_cmd_type *                        uim_cmd_ptr      = NULL;
  mmgsdi_return_enum_type               mmgsdi_status    = MMGSDI_SUCCESS;
  uim_slot_type                         uim_slot         = UIM_SLOT_NONE;
#ifdef FEATURE_UIM_EUIMID
  mmgsdi_meid_data_type                 meid_nv_data;
  gsdi_slot_id_type                     gsdi_slot;
  nv_stat_enum_type                     nv_return_staus;

  memset((byte*)(&meid_nv_data), 0x00, sizeof(mmgsdi_meid_data_type));
#endif /* FEATURE_UIM_EUIMID */

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(slot,
                                              &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    return mmgsdi_status;
  }


  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_STORE_ESN_ME_F
    2) protocol = CDMA
    3) slot     = supplied
    4) channel = default channel
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command             = UIM_STORE_ESN_ME_F;
  uim_cmd_ptr->hdr.protocol            = UIM_CDMA;
  uim_cmd_ptr->hdr.slot                = uim_slot;
  uim_cmd_ptr->hdr.channel             = UIM_CHANNEL0;
  
#ifdef FEATURE_UIM_EUIMID
   /* check phase */
  mmgsdi_status = mmgsdi_map_gsdi_mmgsdi_slot(TRUE,
                                              &gsdi_slot,
                                              &slot);
  if (mmgsdi_status != MMGSDI_SUCCESS) 
  {
    MSG_ERROR("MMGSDI Slot %d could not be converted to GSDI Slot", slot,0,0);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }
  if (gsdi_is_sf_euimid_activated(gsdi_slot)) 
  {
    uim_cmd_ptr->hdr.command  = UIM_STORE_ESN_MEID_ME_F;
  }

  /* Check if service 9 is supported and activated. If the NV read to MEID */
  /* returns an error code then we assume that the handset does not have a */
  /* valid MEID and we continue with sending the ESN as if n9 is not enabled.*/
  meid_nv_data = mmgsdi_get_meid();  
  if ((meid_nv_data.meid_available  == TRUE) && (gsdi_is_meid_svc_activated(gsdi_slot)))	  
  {  
    uim_cmd_ptr->hdr.command  = UIM_STORE_ESN_MEID_ME_F;
    uim_cmd_ptr->store_meid.len_and_usage = (GSDI_RUIM_STORE_MEID_LENGTH | 
                                             GSDI_RUIM_STORE_MEID_USE_EUIM_ID);
    
    qw_equ(uim_cmd_ptr->store_meid.meid,meid_nv_data.meid);
  }
  else
#endif /* FEATURE_UIM_EUIMID */
  {
    uim_cmd_ptr->store_esn.len_and_usage =
        GSDI_RUIM_STORE_ESN_LENGTH | GSDI_RUIM_STORE_ESN_USE_UIM_ID;
#ifndef FEATURE_VIRTUAL_SIM
    /* Need to get ESN Using TMC Interface */
    uim_cmd_ptr->store_esn.esn = tmc_get_stored_esn_me();
#else
    uim_cmd_ptr->store_esn.esn = 1111;
#endif /* FEATURE_VIRTUAL_SIM */
  }

  /* Send the command to UIM synchronously.  If recovery is in progress,
   * the appropriate uim api would be used
   */
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);

  /* Clear any memory that has been allocated */
  if (mmgsdi_internal_synch_uim_rpt_rsp != NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      mmgsdi_internal_synch_uim_rpt_rsp->rsp_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp);
  }

  /* Free the UIM command pointer that has been allocated */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);

  return mmgsdi_status;
} /* mmgsdi_store_esn_synch */
#endif /* FEATURE_MMGSDI_CDMA */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_RECOVERY_COMPLETE_IND_TO_UIM

   DESCRIPTION:
     This function sends down a recovery complete command to uim

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
   get's UIM module out of recovery mode to the previously existing mode

-------------------------------------------------------------------------------*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static mmgsdi_return_enum_type mmgsdi_recovery_complete_ind_to_uim(
  mmgsdi_slot_id_enum_type slot
)
{
  uim_cmd_type *                        uim_cmd_ptr      = NULL;
  mmgsdi_return_enum_type               mmgsdi_status    = MMGSDI_SUCCESS;
  uim_slot_type                         uim_slot         = UIM_SLOT_NONE;

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(slot,
                                              &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    ERR_FATAL("Could not allocate memory for recovery complete", 0, 0, 0);
  }


  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_RECOVER_COMPLETE_F
    2) protocol = not required
    3) slot     = supplied
    4) channel = default
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_RECOVERY_COMPLETE_F;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = UIM_CHANNEL0;

  /* Send the command to UIM synchronously.  If recovery is in progress,
   * the appropriate uim api would be used
   */
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);

  /* Clear any memory that has been allocated */
  if (mmgsdi_internal_synch_uim_rpt_rsp != NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      mmgsdi_internal_synch_uim_rpt_rsp->rsp_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp);
  }

  /* Free the UIM command pointer that has been allocated */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);

  return mmgsdi_status;
} /* mmgsdi_recovery_complete_ind_to_uim */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_POWER_DOWN_UIM_SYNCH

   DESCRIPTION:
     This function sends down a power down command to uim synchronously.

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
   get's UIM module out of recovery mode to the previously existing mode and
   since the UIM module is now powered down, all the commands that where
   pending in the UIM command queue would eventually be flushed out.  Also,
   UIM_ERROR_SIG would be set due to powering down the interface.

-------------------------------------------------------------------------------*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static mmgsdi_return_enum_type mmgsdi_power_down_uim_synch(
  mmgsdi_slot_id_enum_type slot
)
{
  uim_cmd_type *                        uim_cmd_ptr      = NULL;
  mmgsdi_return_enum_type               mmgsdi_status    = MMGSDI_SUCCESS;
  uim_slot_type                         uim_slot         = UIM_SLOT_NONE;

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(slot,
                                              &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uim_cmd_ptr, sizeof(uim_cmd_type),mmgsdi_status);
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    return mmgsdi_status;
  }


  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_POWER_DOWN_F
    2) protocol = not required
    3) slot = supplied
    4) channel = default
    5) pdown.pdown_option = NOTIFY GSDI ON RESET
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_POWER_DOWN_F;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = UIM_CHANNEL0;
  uim_cmd_ptr->pdown.pdown_option  = UIM_OPTION_NOTIFY_PDOWN;

  /* Send the command to UIM synchronously.  If recovery is in progress,
   * the appropriate uim api would be used
   */
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);

  /* Clear any memory that has been allocated */
  if (mmgsdi_internal_synch_uim_rpt_rsp != NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      mmgsdi_internal_synch_uim_rpt_rsp->rsp_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp);
  }

  /* Free the UIM command pointer that has been allocated */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);

  return mmgsdi_status;
} /* mmgsdi_power_down_uim_synch */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*FEATURE_MMGSDI_UMTS*/


#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS) */


#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /*FEATURE_MMGSDI_GSM*/


#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*FEATURE_MMGSDI_UMTS*/


#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /*FEATURE_MMGSDI_GSM*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_RECOVER_AFTER_RESET

   DESCRIPTION:
   This function is primarily responsible to try to get the CARD to a well
   defined state.  Based on the protocol, this function tries to perform the
   initialization procedures specified for the kind of card.

   If pin has been verified, it would be re-verified

   Terminal profile download will be done appropriately

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
   Card comes back to a known state or is powered down.

-------------------------------------------------------------------------------*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_recover_after_reset(mmgsdi_slot_id_enum_type slot)
{
  uint8 *                 card_apps_p    = NULL;
  uint8                   card_apps      = 0x00;
  mmgsdi_return_enum_type mmgsdi_status  = MMGSDI_SUCCESS;
  gsdi_slot_id_type       gsdi_slot      = GSDI_SLOT_NONE;
#ifdef FEATURE_GSTK
  rex_sigs_type           wait_sigs      = 0x00;
  rex_sigs_type           sigs           = 0x0;
#endif /* FEATURE_GSTK */

  MSG_HIGH("mmgsdi_recover_after_reset", 0, 0, 0);

  /* Reset the retry count only after SIM initialization. */
  if(gsdi_data.gsdi_state == GSDI_SIM_INIT_COMPLETED_S)
  {
    /* Initialize the variable indicating the number of times recovery is
     * re-tried
     */
    gsdi_data.recovery_retry_count = 0;
  }

  /* Convert mmgsdi slot to gsti slot */
  mmgsdi_util_convert_mmgsdi_to_gsdi_slot(slot, &gsdi_slot);
  if ((gsdi_slot != GSDI_SLOT_1) && (gsdi_slot != GSDI_SLOT_2))
  {
    ERR_FATAL("Invalid slot received", 0, 0, 0);
  }

  /* Get the currently enabled applications to a local pointer */
  card_apps_p = gsdi_util_get_apps_enabled(gsdi_slot);

  if(gsdi_data.recovering_after_reset)
  {
    MSG_HIGH("Entering mmgsdi_recover_after_reset while recovering!",0,0,0);
  }
  /* Indicate that recovery is in progress */
  gsdi_data.recovering_after_reset = TRUE;

  /* Clean up session table for non default channel and
    notify all the clients of Close Session */
  mmgsdi_util_close_all_sessions_cleanup_and_notify();

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */

  do
  {
    /* Clear the signal which was set to indicate that the card
     * was internally reset
     */
    (void)rex_clr_sigs(&gsdi_task_tcb, GSDI_UIM_RESET_COMPLETE_SIG);

    /* Increment the re-try counter */
    ++gsdi_data.recovery_retry_count;

    MSG_ERROR("Performing procedure %d time", gsdi_data.recovery_retry_count,
               0, 0);


#ifdef FEATURE_GSTK
    /* TP download would be done for all technologies for GSM and CDMA,
     * the PIN would have already been verified by the time we are here
     * for USIM, the pin would be verified after completing the TP dl
     */

    /* Send out the event that UIM was internally reset to gstk only */
#ifdef FEATURE_ENHANCED_NW_SELECTION
    /* let GSTK continue since we are sending cmd to it */
    gstk_gsdi_continue_task(TRUE);
#endif /* FEATURE_ENHANCED_NW_SELECTION */
    gstk_gsdi_sim_events_cb (GSDI_INTERNAL_SIM_RESET);

    wait_sigs = ( GSDI_REX_TIMER_EXPIRE_SIG          | /*GSTK took too long*/
                  GSDI_WAIT_GSTK_TP_DL_COMPLETED_SIG | /*GSTK DL Completed */
                  GSDI_UIM_RESET_COMPLETE_SIG          /* UIM internal reset*/
                 );

    /* Clear Out all Signals relevant to the
    ** Terminal Profile Download Event
    */
    (void) rex_clr_sigs(&gsdi_task_tcb, wait_sigs);

    /* Set Timer for wait on GSTK.  Give it little more time compared
    ** to while in non-recovery mode.
    */
    (void) rex_set_timer(&gsdi_gstk_timer, GSDI_WAIT_FOR_GSTK_TO_TP_DL * 5);

    /* Wait for indication from GSTK that
    ** the Terminal Profile Download
    ** has completed.  PASS or FAIL
    ** it doesn't matter to GSDI
    */
    sigs = gsdi_wait (wait_sigs);

    if ( sigs & GSDI_UIM_RESET_COMPLETE_SIG )
    {
      MSG_ERROR("UIM reset while waiting for GSTK to TP-DL", 0, 0, 0);
      /* Try to re-cover again */
      continue;
    }
    if ( sigs & GSDI_REX_TIMER_EXPIRE_SIG )
    {
      MSG_ERROR("GSTK could not do a TP DL while recovery", 0, 0, 0);
      (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_REX_TIMER_EXPIRE_SIG);
    }
    else
    {
      MSG_HIGH("MMGSDI RECOVERY completed TP DL", 0, 0, 0);
      (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_WAIT_GSTK_TP_DL_COMPLETED_SIG);
    }
#endif /*FEATURE_GSTK*/

#ifdef FEATURE_MMGSDI_PERSONALIZATION
    if ( !gsdi_is_perso_completed() )
    {
      /* If personalization is not complete, we need not recover */
      MSG_HIGH("Personalization not completed yet nothing to recover",
                0, 0, 0);
      continue;
    }
#endif

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
    card_apps = *card_apps_p & GSDI_RUIM_APP_MASK;
    if ( card_apps )
    {
      mmgsdi_util_convert_mmgsdi_to_gsdi_slot(slot, &gsdi_slot);

      /* Read CDMA service table */
      if ( mmgsdi_init_cache_binary(gsdi_slot, MMGSDI_CDMA_SVC_TABLE, NULL) ==
           MMGSDI_UIM_INTERNALLY_RESET )
      {
        continue;
      }

      /* Store ESN */
      mmgsdi_status = mmgsdi_store_esn_synch(slot);
      if ( mmgsdi_status == MMGSDI_UIM_INTERNALLY_RESET )
      {
        continue;
      }
      else if ( mmgsdi_status != MMGSDI_SUCCESS )
      {
        MSG_ERROR("Store ESN failed during recovery", 0, 0, 0);
      }
    } /* RUIM card apps */
#endif /* FEATURE_MMGSDI_CDMA */


#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif
  } while ( (gsdi_data.recovery_retry_count < MAX_RECOVERY_ATTEMPTS ) &&
            ( (mmgsdi_status == MMGSDI_UIM_INTERNALLY_RESET) ||
              rex_get_sigs(&gsdi_task_tcb) & GSDI_UIM_RESET_COMPLETE_SIG) );

  /* We are out because we have exceed the max attempts */
  if ( gsdi_data.recovery_retry_count >= MAX_RECOVERY_ATTEMPTS )
  {
    /* Clear the signal if it is still present since we do not want to process
     * this again after the completion
     */
    (void)rex_clr_sigs(&gsdi_task_tcb, GSDI_UIM_RESET_COMPLETE_SIG);

    /* send power down command to the card */
    MSG_ERROR("Powering down the interface since recovery failed", 0, 0, 0);
    if ( mmgsdi_power_down_uim_synch(slot) )
    {
      MSG_ERROR("UIM power down failed", 0, 0, 0);
    }

    mmgsdi_status = MMGSDI_ERROR;
  }
  else
  {
    if(rex_get_sigs(&gsdi_task_tcb) & GSDI_UIM_RESET_COMPLETE_SIG)
    {
      /* UIM Has just RESET AGAIN, don't send RECOVERY IND, let main task 
         loop call mmgsdi_recover_after_reset once more */
      MSG_HIGH("Recovery finished, but UIM Reset Again. Don't send Recovery Complete",0,0,0);
    }
    else
    {
      /* recovery complete command here */
      mmgsdi_status = mmgsdi_recovery_complete_ind_to_uim(slot);
      MSG_HIGH("Sent recovery complete ind to UIM - mmgsdi_status %d",
             mmgsdi_status, 0 , 0);
      if(rex_get_sigs(&gsdi_task_tcb) & GSDI_UIM_RESET_COMPLETE_SIG)
      {
        /* Reset has just occurred after we finished recovering */
        MSG_ERROR("UIM has RESET again after RECOVERY.  Abandoning Recovery efforts",0,0,0);

       /* Clear the signal if it is still present since we do not want to process
        * this again after the completion
        */
        (void)rex_clr_sigs(&gsdi_task_tcb, GSDI_UIM_RESET_COMPLETE_SIG);

        /* send power down command to the card */
        if ( mmgsdi_power_down_uim_synch(slot) )
        {
          MSG_ERROR("UIM power down failed", 0, 0, 0);
        }

        mmgsdi_status = MMGSDI_ERROR;
      }
    }
  }

  /* Note that we are done with recovery */
  gsdi_data.recovering_after_reset = FALSE;

  return mmgsdi_status;
} /* mmgsdi_recover_after_reset */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */

