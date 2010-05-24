/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


               G S D I   U I M   S E R V E R  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains functions that interface directly to the UIM
  Server that controls access to the serial drivers that communicate directly
  with the smart card.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdiuim.c#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/09   rm      Fix to avoid a null pointer access
05/22/09   js      Fixed path copying in gsdi_uim_server_cdma_select
05/05/09   js      Fixed warnings
04/29/09   js      Included "gsdi_chnl_mgt.h", defined prototypes for
                   static functions, fixed compiler warning
03/25/09   sun     Added support for Virtuim
03/11/09   mib     Fixed PUK1 number of retries
10/06/08   jk      Allow GSM SIM Access in 1X Only
08/21/08   sun     Fixed Klockwork errors
08/18/08   sp      Edited debug messages
07/22/08   js      Fixed UIM_MAX_PATH_ELEMENTS issue. Updated path length to 4
                   for all UIM commands. MAX path length in MMGSDI is set to 5.
06/20/08   sp      Add data offset messages for read
04/11/08   kk      Fixed compilation warning
03/26/08   kk      Buffer allocation for select response made dynamic
12/03/07   kk      Handled invalid length (length greater than
                   GSDI_DEFAULT_FILE_HEADER_LEN) in response of
                   get_file_attributes
11/15/07   sun     Added support for NAA refresh
11/08/07   vs/nk   Added EUIMID feature support
05/31/07   sp      Fixed compile error
05/31/07   sun     Set the P2 param to 0 in Disable CHV so that there is
                   no replacement performed.
05/30/07   sp      Add function to build status command for app termination
05/22/07   nk      Updated gsdi_uim_server_store_esn
05/21/06   jk      Do not allow reentry to mmgsdi_recover_after_reset
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
03/26/07   sun     Check if additional files are being read during refresh
                   fcn
03/02/07   sun     Fixed Lint Errors
02/21/07   sun     Remove GSDI Global PIN
11/08/06   tml     Fix Fake IMSI Reset with TP issue
08/23/06   sun     Fixed Lint Errors
07/07/06   sun     Removed Reference to mc.h
07/10/06   pv      Change message error to print correct error.
06/21/06   pv      Do not send commands to UIM when UIM has already notified
                   an error.
06/12/06   pv      Call mmgsdi_recover_after_reset when
                   GSDI_UIM_RESET_COMPLETE_SIG is received.
06/02/06   jar     Fix to ensure PERM Blocked status comes back when attempting
                   to unblock a Perm Blocked SIM
03/21/06   tml     lint
03/09/06   tml     Clear UIM Get Response cache when refresh
02/14/06   nk      Merged for ARR caching and by path changes
02/06/06   tml     Fixed event notification for cdma unblock chv and 0x62
                   status word return value overwrite issue for umts change
01/19/06   jar     Fix for AT+CRSM wrong implicit status words for EF under
                   MF Accesses.
12/23/05   sun     Copy only the first byte from the search result
11/17/05   tml     Set channel to 0 for Send APDU
11/14/05   tml     Fixed USIM unblock issue
11/09/05   sun     Fixed Lint Errors
11/07/05   tml     Fixed channel for reset
11/04/05   tml     Fixed linking error in 1X
11/03/05   tml     Fixed status word 0x63 for write and increase
11/03/05   tml     Pin num retries fix
09/20/05   tml     Fixed status word imcompatibility for pin related USIM cmds
09/20/05   jar     Fix to avoid Error Fatal invalid Channel Ids from the UIM
                   interface.
09/15/05   tml     Fixed status word issue for 0x63
09/01/05   tml     Fixed return status mapping for USIM
08/17/05   pv      Added messages to print the security context, request
                   to the card and the response from the CARD for usim
                   authenticate command.
07/18/05   sun     Increased the Access UIM Path Array
07/12/05   tml     Channel ID support
07/11/05   sun     Fixed Memory Leaks
06/16/05   sk      Fixed potential problems with gsdi_uim_server_send_channel_data
02/24/05   tml     Added 256+ support and lint fixes
05/11/05   sst     Lint fixes
03/09/05    jk     Fixed reset_due_to_fake_imsi to properly populate necessary
                   fields of the gsdi_link_est_message
02/15/05    pv     added additional parameter to gsdi_uim_server_sim_reset
                   to indicate the protocol for uim to start with
02/04/05   jar     Avoid causing a UIM Data Abort by not exiting from
                   gsdi_send_cmd_to_uim_server() when TASK_STOP_SIG is
                   detected.
01/28/05   jk      Added functionality to pass card status words back to application
                   for JAVA
01/26/05   jar     Fixed incorrect Message Format for Send Channel Data Status
01/21/05   jk      Added gsdi_cmd_ext which checks GSDI CMD Q size to replace gsdi_cmd.
11/15/04   jar     Merged in Dual Slot Support
11/01/04  pv       Set the sanity timer to the computed value instead of
                   a fixed value
10/18/04  sk       Added defintion for function gsdi_uim_server_cdma_seek
09/23/04 jar/tml   Added send channel data returned data support
09/09/2004 sk      Added and modified a few fields in pointer data_p in function
                   gsdi_uim_server_send_channel_data
                   Changed *returned_data_len_p += 3; to *returned_data_len_p
                   += 5; in gsdi_uim_server_send_channel_data
08/31/04   jar     Added support for FEATURE_MMGSDI_ATCSIM
07/14/04  jar/tml  Merged in UIM hanging sanity timer issue
07/12/04   tml     Return INCOMPAT_PIN_STATUS when trying to disable an already
                   disabled pin or enable an already enabled pin
05/20/04   tml     Correctly populate the channel id for close channel
04/28/04   jar     Added Channel Management Support
03/31/04   jar     Removed usage of warning_to_mmi in CHV related functions
                   Removed variables not used (compile warnings).
03/12/04   tml     Error Fatal Fix for QLINK & Virtual SIM Builds.
03/01/04   tml     Added rec num accessed support for write
02/21/04   jar     Removed variables not used in the gsdi_uim_server_send_apdu
                   function.  Removes compile warning.
12/20/03   tml     Added invalidate support for RUIM
12/03/03   tml     lint
11/10/03   jar     Fixed Compile Issue
11/05/03   jar     Added GSDI - UIM Interface to complete the processing of
                   GSDI_SWITCH_SLOT_PREF_REQ to a UIM_PREF_SLOT_CHANGE_F.
10/20/03   jar     Added SEND APDU Support under FEATURE_MMGSDI_TEST_
                   AUTOMATION.
09/09/03  jar/AR   Fixed PIN2 Verification Event Issue
09/04/03   jar     Removed ERR_FATALs & Dual Slot Code Cleanup
08/29/03   jar     FEATURE_MMGSDI_DUAL_SLOT Compile Warning Fixes
08/25/03   jar     Added FEATURE_MMGSDI_DUAL_SLOT
06/04/03   jar     Fixed Compile Error
05/20/03   tml     linted
04/28/03   tml     Added image support
04/21/03   tml     Properly featurized UTK support
04/10/03   jar     Removed unused declaration of sw1_sw2
03/19/03   jar     Modified FEATURE for FEATURE_UIM_TOOLKIT to
                   FEATURE_UIM_TOOLKIT_UTK for MSM6300 UTK Builds.
03/18/03   jar     Added code to set pin_status to GSDI_PIN_BLOCKED and
                   GSDI_PIN_PERM_BLOCKED
02/18/03   jar     Added GSDI/UIM Server Interface for a WARM REST
                   function:  gsdi_uim_server_sim_reset();
02/03/03   tml     changed GSDI_ERROR to GSDI_SIM_TECHNICAL_PROBLEMS when
                   memory problem is received from UIM server.
01/07/02   jar     Populate AID for PIN2 Functions when a USIM is used.
                   Modified final Else gsdi_status = GSDI_ERROR to
                   GSDI_ERROR_NO_EVENT_NEEDED.
11/26/02   jar     Fixed Compile issue for GSDI_CODE_PERM_BLOCKED.  Removed
                   MSG_HIGH in gsdi_uim_server_terminal_profile().
10/16/02   tml     Send GSDI_PIN1_PERM_BLOCKED / GSDI_PIN2_PERM_BLOCKED
                   event to registered client when the puk1 /puk2 is blocked
10/14/02   tml     1x, gsm merge
09/25/02   AP      Set TERMINAL_PROFILE bytes to enable UTK functionality.
09/03/02   jar     Modified gsdi_uim_server_terminal_profile() to reset
                   gsdi_status upon leaving this function to ensure
                   init procedures complete.
08/27/02   tml     Fixed comments.
08/21/02   jar     Added gsdi_uim_server_store_esn()
08/16/02   tml     Modified return status to correctly correspond with GSM
                   11.11
08/09/02   tml     Added status word response for error case handling
07/24/02   tml     Added Select api
05/27/02   jar     Made USIM Error Handling More Robust.
05/21/02   jar     Added more robust error handling for SIM requests
                   that fail.  Added gsdi_uim_server_usim_status.  Modified
                   gsdi_uim_server_usim_search to better handle an
                   Enhanced Search.
05/19/02   jar     Cleaned up CHV and PIN Functions to use only one Event type
                   to send events regardless of whether FEATURE_GSDI_
                   MULTICLIENT_SUPPORT is defined.
05/02/02   jar     Modified gsdi_uim_server_gsm_select to interpret the status
                   return of 0x94 0x04 as GSDI_NOT_SUPPORTED instead of
                   GSDI_ERROR.
04/17/02   jar     Modified gsdi_uim_server_gsm_increase and gsdi_uim_server_
                   usim_increase to properly support AOC.
03/23/02   jar     Added a check for Technical Problems (which are application
                   specific) for USIM Authentication
03/17/02   jar     Fixed Checking for PIN1 Twice and not checking for PIN2 in
                   all PIN Functions for SIM/USIM/GSDI Events.  Now check for
                   PIN1 and PIN2.
03/11/02   jar     Added event detecting and event generating code in all CHV
                   related functions below for both GSM and USIM.  All changes
                   are wrapped with a FEATURE_GSDI_MULTICLIENT_SUPPORT feature
03/09/02   jar     Changes were made in gsdi_uim_server_usim_write to do the
                   same mapping of GSDI_INDEX_PREVIOUS to UIM_PREVIOUS as in
                   gsdi_uim_server_gsm_write.  Updated comments in code and history.
03/08/02   jar     Changes were made in gsdi_uim_server_gsm_write to map index
                   mode GSDI_INDEX_PREVIOUS (for GSDI) to UIM_PREVIOUS (for UIM).
                   This corrected the problems of failing write attempts to GSM
                   SIM Cyclical EFs because GSDI_INDEX_ABSOLUTE (which is not
                   allowed for cyclical EFs) was hardcoded in the Messages
                   to the UIM.
02/03/02   jar     Added Phonebook Support functionality which is featurized with
                   FEATURE_GSDI_PHONEBOOK_SUPPORT.
01/31/02   jar     Added checks for the USIM Status Words Returned from the
                   UIM Server or Vitual UIM Server in gsdi_uim_server_usim_read
                   and gsdi_uim_server_usim_select
11/20/01   jar     Added a function for the UIM to use as a method of
                   determining what SIM/USIM is used in the way of a protocol
10/23/01   jc      Add FEATURE_GSDI_UIM_SERVER feature for virtuim_cmd of
                   Virtual UIM Server.
09/13/01   jc      Mods for network authentication for both GSM and UMTS.


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "intconv.h"
#include "customer.h"
#include "rex.h"
#include "memheap.h"
#include "queue.h"
#include "msg.h"
#include "task.h"
#include "dsm.h"
#include "err.h"

#include "string.h"

#include "gsdi_exp.h"
#include "gsdi.h"
#include "gsdi_convert.h"

#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif

#if defined (FEATURE_GSTK_DUAL_SLOTS) && defined (FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY)
#error code not present
#endif /* FEATURE_GSTK_DUAL_SLOTS && FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */

#ifdef FEATURE_UIM
#include "uim.h"
#endif

#ifdef FEATURE_GSDI_UIM_SERVER
#include "virtuim.h"
#endif

#include "mmgsdi.h"
#include "mmgsdiutil.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_recover.h"
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
#include "gsdi_chnl_mgt.h"
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

static uim_rpt_type uim_rsp_report;



typedef struct
{
  int32             req_id;
  gsdi_pin_ids_T    pin_id;
  gsdi_slot_id_type slot;
  gsdi_sim_events_T evt;
}gsdi_pin_evt_table_type;

#define GSDI_PIN_SUCCESS_NUM_EVT 20

gsdi_pin_evt_table_type gsdi_pin_success_evt_table[GSDI_PIN_SUCCESS_NUM_EVT] =
{
  {GSDI_VERIFY_PIN_REQ, GSDI_PIN1, GSDI_SLOT_1, GSDI_PIN1_VERIFIED},
  {GSDI_VERIFY_PIN_REQ, GSDI_PIN1, GSDI_SLOT_2, GSDI_PIN1_VERIFIED_2},
  {GSDI_VERIFY_PIN_REQ, GSDI_PIN2, GSDI_SLOT_1, GSDI_PIN2_VERIFIED},
  {GSDI_VERIFY_PIN_REQ, GSDI_PIN2, GSDI_SLOT_2, GSDI_PIN2_VERIFIED_2},

  {GSDI_CHANGE_PIN_REQ, GSDI_PIN1, GSDI_SLOT_1, GSDI_PIN1_CHANGED},
  {GSDI_CHANGE_PIN_REQ, GSDI_PIN1, GSDI_SLOT_2, GSDI_PIN1_CHANGED_2},
  {GSDI_CHANGE_PIN_REQ, GSDI_PIN2, GSDI_SLOT_1, GSDI_PIN2_CHANGED},
  {GSDI_CHANGE_PIN_REQ, GSDI_PIN2, GSDI_SLOT_2, GSDI_PIN2_CHANGED_2},

  {GSDI_UNBLOCK_PIN_REQ, GSDI_PIN1, GSDI_SLOT_1, GSDI_PIN1_UNBLOCKED},
  {GSDI_UNBLOCK_PIN_REQ, GSDI_PIN1, GSDI_SLOT_2, GSDI_PIN1_UNBLOCKED_2},
  {GSDI_UNBLOCK_PIN_REQ, GSDI_PIN2, GSDI_SLOT_1, GSDI_PIN2_UNBLOCKED},
  {GSDI_UNBLOCK_PIN_REQ, GSDI_PIN2, GSDI_SLOT_2, GSDI_PIN2_UNBLOCKED_2},

  {GSDI_DISABLE_PIN_REQ, GSDI_PIN1, GSDI_SLOT_1, GSDI_PIN1_DISABLED},
  {GSDI_DISABLE_PIN_REQ, GSDI_PIN1, GSDI_SLOT_2, GSDI_PIN1_DISABLED_2},
  {GSDI_DISABLE_PIN_REQ, GSDI_PIN2, GSDI_SLOT_1, GSDI_PIN2_DISABLED},
  {GSDI_DISABLE_PIN_REQ, GSDI_PIN2, GSDI_SLOT_2, GSDI_PIN2_DISABLED_2},

  {GSDI_ENABLE_PIN_REQ, GSDI_PIN1, GSDI_SLOT_1, GSDI_PIN1_ENABLED},
  {GSDI_ENABLE_PIN_REQ, GSDI_PIN1, GSDI_SLOT_2, GSDI_PIN1_ENABLED_2},
  {GSDI_ENABLE_PIN_REQ, GSDI_PIN2, GSDI_SLOT_1, GSDI_PIN2_ENABLED},
  {GSDI_ENABLE_PIN_REQ, GSDI_PIN2, GSDI_SLOT_2, GSDI_PIN2_ENABLED_2}
};

/*===========================================================================
         S T A T I C    F U N C T I O N   P R O T O T Y P E S
============================================================================*/

static void gsdi_uim_report (
    uim_rpt_type *report
);

static gsdi_returns_T gsdi_uim_server_parse_response(
  uim_report_code_type    rpt_code,
  uim_rpt_status          rpt_status,
  uint32                  sw1,
  uint32                  sw2
);

static gsdi_returns_T gsdi_populate_uim_slot(
  uim_slot_type *     uim_cmd_slot,
  gsdi_slot_id_type   slot
);

static void gsdi_uim_server_preset_num_retries (
  gsdi_slot_id_type            slot,
  gsdi_pin_ids_T               pin_id
);

static gsdi_returns_T gsdi_uim_server_find_pin_success_evt (
  gsdi_slot_id_type            slot,
  gsdi_pin_ids_T               pin_id,
  int32                        req_id,
  gsdi_sim_events_T           *event_ptr
);

#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_CDMA)
static gsdi_returns_T gsdi_uim_server_gsm_cdma_get_max_num_retries (
  gsdi_slot_id_type            slot,
  gsdi_pin_ids_T               pin_id,
  int32                        req_id,
  gsdi_sim_events_T           *event_ptr
);
#endif /* (FEATURE_MMGSDI_GSM) || (FEATURE_MMGSDI_CDMA) */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_UIM_REPORT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
static void gsdi_uim_report (
    uim_rpt_type *report
)
{
  if(NULL == report)
  {
    MSG_ERROR("gsdi_uim_report, report is NULL",0,0,0);
    return;
  }
  memcpy(&uim_rsp_report,report,sizeof(uim_rpt_type));
  (void) rex_set_sigs(&gsdi_task_tcb, GSDI_UIM_REPORT_SIG);

} /* gsdi_uim_report */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_SEND_CMD_TO_UIM_SERVER

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
void gsdi_send_cmd_to_uim_server (
    uim_cmd_type *cmd_ptr
)
{
    rex_sigs_type sigs = 0x00;

    if(NULL == cmd_ptr)
    {
      MSG_ERROR("gsdi_send_cmd_to_uim_server, cmd_ptr is NULL",0,0,0);
      return;
    }

    cmd_ptr->hdr.cmd_hdr.task_ptr   = NULL_PTR;
    cmd_ptr->hdr.cmd_hdr.sigs       = 0;
    cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL_PTR;
    (void)q_link(cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link);

    cmd_ptr->hdr.options      = UIM_OPTION_ALWAYS_RPT;
    cmd_ptr->hdr.rpt_function = gsdi_uim_report;

    (void)rex_clr_sigs(&gsdi_task_tcb, GSDI_UIM_REPORT_SIG);
    (void)rex_clr_sigs(&gsdi_task_tcb, GSDI_UIM_SANITY_TIMER_EXPIRE_SIG);

    MSG_HIGH("SEND CMD TO UIM",0,0,0);

    if ( gsdi_data.task_stopped )
    {
      ERR("GSDI TASK STOPPED...CAN'T PROCESS COMMAND ",0,0,0);
      uim_rsp_report.rpt_status = UIM_FAIL;
      uim_rsp_report.rpt_type   = UIM_NO_SUCH_REPORT_R;
      uim_rsp_report.sw1        = SW1_NO_STATUS;
      uim_rsp_report.sw2        = SW2_NORMAL_END;
      uim_rsp_report.user_data  = cmd_ptr->hdr.user_data;
      uim_rsp_report.cmd_transacted = FALSE;
      return;
    }
    else if ( (rex_get_sigs(&gsdi_task_tcb) & GSDI_UIM_ERROR_SIG) )
    {
      ERR("UIM already notified error", 0, 0, 0);
      uim_rsp_report.rpt_status = UIM_FAIL;
      uim_rsp_report.rpt_type   = UIM_NO_SUCH_REPORT_R;
      uim_rsp_report.sw1        = SW1_NO_STATUS;
      uim_rsp_report.sw2        = SW2_NORMAL_END;
      uim_rsp_report.user_data  = cmd_ptr->hdr.user_data;
      uim_rsp_report.cmd_transacted = FALSE;
      return;
    }

    /* Set the 2 Minute Sanity Timer for each SIM Access */
    (void)rex_set_timer(&gsdi_uim_sanity_timer,
                        gsdi_data.gsdi_uim_sanity_timer_value);

    MSG_HIGH("SEND CMD TO UIM",0,0,0);
#ifdef FEATURE_GSDI_UIM_SERVER
    virtuim_cmd(cmd_ptr);
#else
    uim_cmd(cmd_ptr);
#endif /* FEATURE_GSDI_UIM_SERVER */

    do
    {
      sigs = gsdi_wait(GSDI_UIM_REPORT_SIG
                       |GSDI_UIM_RESET_COMPLETE_SIG
                       |GSDI_UIM_ERROR_SIG
                       |GSDI_UIM_SANITY_TIMER_EXPIRE_SIG);

      MSG_HIGH("OUT OF WAIT FOR UIM: SIGS 0x%x",sigs,0,0);

      if (sigs & GSDI_UIM_RESET_COMPLETE_SIG)
      {
        /* Do not re-enter recover after reset if we are already recovering */
        if(!gsdi_data.recovering_after_reset)
        {
          if ( mmgsdi_recover_after_reset(MMGSDI_SLOT_1) != MMGSDI_SUCCESS )
          {
            MSG_ERROR("Recovery failed", 0, 0, 0);
          }
        }
      }

    } while ( (sigs &
               (GSDI_UIM_REPORT_SIG |
                GSDI_UIM_ERROR_SIG |
                GSDI_UIM_SANITY_TIMER_EXPIRE_SIG)) == 0 );
    /* If the Task Stop Sig is detected, stop sending
    ** the UIM Task commands.
    */
    if ( sigs & TASK_STOP_SIG )
    {
      gsdi_data.task_stopped = TRUE;
      mmgsdi_generic_data.task_stopped = TRUE;
    }

    /* Continue with normal processing of the Signals
    */
    if ( sigs & GSDI_UIM_REPORT_SIG )
    {
      (void)rex_clr_sigs(&gsdi_task_tcb, GSDI_UIM_REPORT_SIG);
      (void)rex_clr_timer(&gsdi_uim_sanity_timer);
    }
    else if ( sigs & GSDI_UIM_ERROR_SIG )
    {
      /* Do not clear the SIG let the GSDI Task Main Wait Function
      ** handle it.
      */
      ERR("UIM ERROR SIG DURING COMMAND",0,0,0);
      (void)rex_clr_timer(&gsdi_uim_sanity_timer);
      uim_rsp_report.rpt_status = UIM_FAIL;
      uim_rsp_report.rpt_type   = UIM_NO_SUCH_REPORT_R;
      uim_rsp_report.sw1        = SW1_NO_STATUS;
      uim_rsp_report.sw2        = SW2_NORMAL_END;
    }
    else if ( sigs & GSDI_UIM_SANITY_TIMER_EXPIRE_SIG )
    {
      /* Do no clear the SIG let the GSDI Task Main Wait Function
      ** handle it.
      */
      ERR("GSDI - UIM SANITY TIMER EXPIRED",0,0,0);
      uim_rsp_report.rpt_status = UIM_FAIL;
      uim_rsp_report.rpt_type   = UIM_NO_SUCH_REPORT_R;
      uim_rsp_report.sw1        = SW1_NO_STATUS;
      uim_rsp_report.sw2        = SW2_NORMAL_END;

    }

} /* gsdi_send_cmd_to_uim_server */


/*===========================================================================
FUNCTION GSDI_UIM_SERVER_PARSE_RESPONSE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
static gsdi_returns_T gsdi_uim_server_parse_response(
    uim_report_code_type    rpt_code,
    uim_rpt_status          rpt_status,
    uint32                  sw1,
    uint32                  sw2
)
{
    gsdi_returns_T gsdi_status = GSDI_SUCCESS;

    /* Was the request successful */
    if (rpt_status == UIM_PASS)
    {
        switch ( sw1 )
        {
        case GSDI_SIM_SW1_NORM_END: /* 0x90 */
            gsdi_status = GSDI_SUCCESS;
            break;
        default: /* other sw1 when rpt_status == PASS */
            gsdi_status = GSDI_ERROR;
            break;
        }
    }
    else
    {
        /* UIM_FAIL */
        switch ( sw1 )
        {
        case GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO: /* 0x91 XX*/
            MSG_HIGH("GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO", 0, 0, 0);

            switch (rpt_code)
            {
            case UIM_SELECT_R:
            case UIM_CACHED_SEL_R:
            case UIM_SEEK_R:
            case UIM_RUN_GSM_ALGO_R:
                /* also Increase.... */
                MSG_ERROR("Invalid report code 0x%x", rpt_code, 0, 0);
                gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                break;

            default:
                /* Unhandled Error Condition */
                gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                break;
            }
            break; /* GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO  0x91 XX*/

        case GSDI_SW1_MEMORY_MANAGEMENT: /* 0x92 */
            switch ( sw2 )
            {
            case GSDI_SW2_MEMORY_PROBLEM: /* 0x40 */
                MSG_HIGH("GSDI_SW2_MEMORY_PROBLEM", 0, 0, 0);
                /* Memory Management Error          */
                /* SIM Memory Problem           */
                gsdi_status = GSDI_SIM_TECHNICAL_PROBLEMS;
                break;

            default:
                MSG_HIGH("Success After Card Internal Update", 0, 0, 0);
                if ((sw2 & 0xF0) == 0)  /* 0x0X */
                {
                    switch (rpt_code)
                    {
                    case UIM_SELECT_R:
                    case UIM_CACHED_SEL_R:
                    case UIM_STATUS_R:
                    case UIM_SEEK_R:
                    case UIM_RUN_GSM_ALGO_R:
                        /* read binary, read record is not valid */
                        MSG_ERROR("Invalid Report Code 0x%x", rpt_code, 0, 0);
                        gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                        break;
                    default:
                        /* command success only after X times internal retries */
                        /* May need to set to GSDI SUCCESS because action did  */
                        /* eventually succeed                                  */
                        gsdi_status = GSDI_SUCCESS_AFTER_CARD_INTERNAL_UPDATE;
                        break;
                    }
                }
                else
                {
                    /* Can't determine error but    */
                    /* Error doesn't need to create */
                    /* an event to the UI           */
                    gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                }
                break;
            }
            break; /* GSDI_SW1_MEMORY_MANAAGEMENT 0x92 */

        case GSDI_SW1_SIM_APP_TOOLKIT_BUSY: /* 0x93 */
            MSG_HIGH("GSDI_SW1_SIM_APP_TOOLKIT_BUSY", 0, 0, 0);
            switch (rpt_code)
            {
            case UIM_ENVELOPE_R:
                /* Sim is busy */
                gsdi_status = GSDI_SIM_BUSY;
                break;
            default:
                MSG_ERROR("Invalid report code 0x%x", rpt_code, 0, 0);
                gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                break;
            }
            break; /* GSDI_SW1_SIM_APP_TOOLKIT_BUSY 0x93 */

        case GSDI_SW1_REFERENCE_MANAGEMENT:  /* 0x94 */
            /* Reference Management Error, Now  */
            /* Determine the specific Error     */
            switch ( sw2 )
            {
            case GSDI_SW2_NO_EF_SELECTED: /* 0x00 */
                MSG_HIGH("GSDI_SW2_NO_EF_SELECTED", 0, 0, 0);
                switch (rpt_code)
                {
                case UIM_ACCESS_R:
                case UIM_SEEK_R:
                case UIM_INVALIDATE_R:
                case UIM_REHABILITATE_R:
                    /* No EF Selected               */
                    gsdi_status = GSDI_NO_EF_SELECTED;
                    break;

                default:
                    MSG_ERROR("Invalid report code 0x%x", rpt_code, 0, 0);
                    gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                    break;
                }
                break;

            case GSDI_SW2_OUT_OF_RANGE: /* 0x02 */
                MSG_HIGH("GSDI_SW2_OUT_OF_RANGE", 0, 0, 0);
                switch (rpt_code)
                {
                case UIM_ACCESS_R: /* only update record and read record */
                    /* Out of Range or invalid      */
                    /* address                      */
                    gsdi_status = GSDI_INCORRECT_PARAMS;
                    break;
                default:
                    MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
                    gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                    break;
                }
                break;

            case GSDI_SW2_FILE_OR_PATTER_ID_NOT_FOUND: /* 0x04 */
                MSG_HIGH("GSDI_SW2_FILE_OR_PATTER_ID_NOT_FOUND",0,0,0);
                switch (rpt_code)
                {
                case UIM_ACCESS_R:
                case UIM_SELECT_R:
                case UIM_CACHED_SEL_R:
                case UIM_SEEK_R:
                    /* File ID not found or pattern */
                    /* not found                    */
                    gsdi_status = GSDI_NOT_FOUND;
                    break;
                default:
                    MSG_ERROR("Invalid report code 0x%x", rpt_code, 0, 0);
                    gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                    break;
                }
                break;

            case GSDI_SW2_FILE_INCONSISTENT_WITH_CMD: /* 0x08 */
                MSG_HIGH("GSDI_SW2_FILE_INCONSISTENT_WITH_CMD", 0, 0, 0);
                switch (rpt_code)
                {
                case UIM_ACCESS_R:
                case UIM_SEEK_R:
                case UIM_RUN_GSM_ALGO_R:
                    /* File is inconsistend with    */
                    /* command                      */
                    gsdi_status = GSDI_EF_INCONSISTENT;
                    break;
                default:
                    MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
                    gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                    break;
                }
                break;

            default:
                /* Can't determine error but    */
                /* Error doesn't need to create */
                /* an event to the UI           */
                gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                break;
            }
            break; /* GSDI_SW1_REFERENCE_MANAGEMENT 0x94 */

        case GSDI_SW1_SECURITY_MANAGEMENT: /* 0x98 */
            /* Security Management Errors       */
            switch ( sw2 )
            {
            case GSDI_SW2_NO_CHV_INITIALISED: /* 0x02 */
                MSG_HIGH("GSDI_SW2_NO_CHV_INITIALISED", 0, 0, 0);
                switch (rpt_code)
                {
                case UIM_VERIFY_CHV_R:
                case UIM_CHANGE_CHV_R:
                case UIM_DISABLE_CHV_R:
                case UIM_ENABLE_CHV_R:
                case UIM_UNBLOCK_CHV_R:
                    /* No CHV Initialized           */
                    gsdi_status = GSDI_PIN_NOT_INITIALIZED;
                    break;
                default:
                    MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
                    gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                    break;
                }
                break;

            case GSDI_SW2_VERIF_UNBLOCK_AUTH_FAIL_W_1: /* 0x04 */
                MSG_HIGH("GSDI_SW2_VERIF_UNBLOCK_AUTH_FAIL_W_1",0,0,0);
                switch (rpt_code)
                {
                case UIM_VERIFY_CHV_R:
                case UIM_CHANGE_CHV_R:
                case UIM_DISABLE_CHV_R:
                case UIM_ENABLE_CHV_R:
                case UIM_UNBLOCK_CHV_R:
                    MSG_HIGH("GSDI_INCORRECT_CODE", 0, 0, 0);
                    gsdi_status = GSDI_INCORRECT_CODE;
                    break;

                case UIM_ACCESS_R:
                case UIM_SEEK_R:
                case UIM_INVALIDATE_R:
                case UIM_REHABILITATE_R:
                    MSG_HIGH("GSDI_ACCESS_DENIED", 0, 0, 0);
                    /* Access conditon not fullfilled  */
                    /* or CHV Verification not success */
                    /* or CHV Unblock not success      */
                    /* or authentication failed        */
                    gsdi_status = GSDI_ACCESS_DENIED;
                    break;

                case UIM_RUN_GSM_ALGO_R:
                    MSG_HIGH("GSDI_AUTH_FAIL", 0, 0, 0);
                    gsdi_status = GSDI_AUTH_FAIL;
                    break;

                default:
                    MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
                    gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                    break;
                }
                break;

            case GSDI_SW2_CNTRCTN_W_CHV_STATUS: /* 0x08 */
                MSG_HIGH("GSDI_SW2_CNTRCTN_W_CHV_STATUS", 0, 0, 0);
                switch (rpt_code)
                {
                case UIM_VERIFY_CHV_R:
                case UIM_CHANGE_CHV_R:
                case UIM_DISABLE_CHV_R:
                case UIM_ENABLE_CHV_R:
                case UIM_UNBLOCK_CHV_R:
                    /* Access conditon not fullfilled  */
                    /* or CHV Verification not success */
                    /* or CHV Unblock not success      */
                    /* or authentication failed        */
                    gsdi_status = GSDI_INCOMPAT_PIN_STATUS;
                    break;
                default:
                    MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
                    gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                    break;
                }
                break;

            case GSDI_SW2_CNTRCTN_W_INVALIDATION_STATUS: /* 0x10 */
                MSG_HIGH("GSDI_SW2_CNTRCTN_W_INVALIDATION_STATUS", 0, 0, 0);
                switch (rpt_code)
                {
                case UIM_ACCESS_R:
                case UIM_SEEK_R:
                case UIM_INVALIDATE_R:
                case UIM_REHABILITATE_R:
                    /* Access conditon not fullfilled  */
                    /* or CHV Verification not success */
                    /* or CHV Unblock not success      */
                    /* or authentication failed        */
                    gsdi_status = GSDI_INVALIDATION_CONTRADICTION_STATUS;
                    break;
                default:
                    MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
                    gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                    break;
                }
                break;

            case GSDI_SW2_CHV_BLOCK_UNSUCCESSFUL_UNBLOCK: /* or GSDI_SW2_CHV_UNBLOCKED 0x40 */
                switch (rpt_code)
                {
                case UIM_VERIFY_CHV_R:
                case UIM_CHANGE_CHV_R:
                case UIM_DISABLE_CHV_R:
                case UIM_ENABLE_CHV_R:
                    MSG_HIGH("GSDI_CODE_BLOCKED", 0, 0, 0);
                    gsdi_status = GSDI_CODE_BLOCKED;
                    break;

                case UIM_UNBLOCK_CHV_R:
                    /* Access conditon not fullfilled  */
                    /* or CHV Verification not success */
                    /* or CHV Unblock not success      */
                    /* or authentication failed        */
                    MSG_HIGH("GSDI_CODE_PERM_BLOCKED", 0, 0, 0);
                    gsdi_status = GSDI_CODE_PERM_BLOCKED;
                    break;

                default:
                    MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
                    gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                    break;
                }
                break;

            case GSDI_SW2_INCREASED_FAIL_MAX_REACHED: /* 0x50 */
                MSG_HIGH("GSDI_SW2_INCREASED_FAIL_MAX_REACHED", 0, 0, 0);
                switch (rpt_code)
                {
                case UIM_ACCESS_R: /* Increase only */
                    /* Access conditon not fullfilled  */
                    /* or CHV Verification not success */
                    /* or CHV Unblock not success      */
                    /* or authentication failed        */
                    gsdi_status = GSDI_INCREASE_MAX_REACHED;
                    break;
                default:
                    MSG_ERROR("Invalid report code %d", rpt_code, 0, 0);
                    gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                    break;
                }
                break;

            default:
                gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                break;
            }
            break; /* GSDI_SW1_SECURITY_MANAGEMENT 0x98 */

        case GSDI_SW1_WRONG_LENGTH: /* 0x67 */
            MSG_HIGH("GSDI_SW1_WRONG_LENGTH", 0, 0, 0);
            /* or GSDI_SW1_INCORRECT_P3 - doesn't have specific sw2 */
            gsdi_status = GSDI_INCORRECT_PARAMS;
            break; /* GSDI_SW1_WRONG_LENGTH 0x67 */

        case GSDI_SW1_INCORRECT_P1_OR_P2: /* 0x6B */
            MSG_HIGH("GSDI_SW1_INCORRECT_P1_OR_P2", 0, 0, 0);
            /* Application Independent Errors   */
            /* Incorrect Parameters P3          */
            /* or Incorrect Parameters P1 & P2  */
            gsdi_status = GSDI_INCORRECT_PARAMS;
            break; /* GSDI_SW1_INCORRECT_P1_OR_P2 0x6B */

        case GSDI_SW1_UNKNOWN_INSTRUCTION_CODE: /* 0x6D */
        case GSDI_SW1_WRONG_INSTRUCTION_CLASS:  /* 0x6E */
            MSG_HIGH("GSDI_UNKNOWN_INST_CLASS", 0, 0, 0);
            /* Unknown or Wrong Instruction class  */
            gsdi_status = GSDI_UNKNOWN_INST_CLASS;
            break; /* GSDI_SW1_WRONG_INSTRUCTION_CLASS 0x6E */

        case GSDI_SW1_TECH_PROB_NO_DIAG_GIVEN: /* 0x6F */
            MSG_HIGH("GSDI_SW1_TECH_PROB_NO_DIAG_GIVEN", 0, 0, 0);
            /* Application Independent Erros    */
            gsdi_status = GSDI_SIM_TECHNICAL_PROBLEMS;
            break; /* GSDI_SW1_TECH_PROB_NO_DIAG_GIVEN 0x6F */

        case GSDI_SIM_SW1_NORM_END: /* 0x90 */
            MSG_HIGH("GSDI_SIM_SW1_NORM_END : UIM CMD TIME OUT", 0, 0, 0);
            gsdi_status = GSDI_UIM_CMD_TIMEOUT;
            break; /* GSDI_SIM_SW1_NORM_END 0x90 */

        case GSDI_SW1_UIM_TIMEOUT: /* 0x00 */
            MSG_HIGH("GSDI_SW1_UIM_TIMEOUT", 0, 0, 0);
            switch ( sw2 ) {
            case GSDI_SW2_UIM_TIMEOUT: /* 0x00 */
                gsdi_status = GSDI_UIM_CMD_TIMEOUT;
                break;
            default:
                gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
                break;
            }
            break; /* GSDI_SW1_UIM_TIMEOUT 0x00 */

        default:
            /* Unhandled Error Condition */
            gsdi_status = GSDI_ERROR_NO_EVENT_NEEDED;
            break;
        }
    } /* UIM Fail */

    return gsdi_status;
}


/*===========================================================================
FUNCTION GSDI_POPULATE_UIM_SLOT

DESCRIPTION
  Helper/Utility Function used to determine and populate the UIM SLOT in the
  Command Header.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
static gsdi_returns_T gsdi_populate_uim_slot(
    uim_slot_type *     uim_cmd_slot,
    gsdi_slot_id_type   slot)
{
    if (uim_cmd_slot == NULL)
    {
        MSG_ERROR("NULL UIM Slot Pointer", 0, 0, 0);
        return GSDI_ERROR;
    }
#ifndef FEATURE_MMGSDI_DUAL_SLOT
    *uim_cmd_slot = UIM_SLOT_1;
#else
    switch (slot)
    {
      case GSDI_SLOT_NONE:
      case GSDI_SLOT_1:
          *uim_cmd_slot = UIM_SLOT_1;
          break;

      case GSDI_SLOT_2:
          *uim_cmd_slot = UIM_SLOT_2;
          break;

      default:
          MSG_ERROR("BAD SLOT ID 0x%x ",slot,0,0);
          return GSDI_ERROR;
    }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    return GSDI_SUCCESS;
} /*lint !e715 */

/*===========================================================================
FUNCTION GSDI_UIM_SERVER_PRESET_NUM_RETRIES

DESCRIPTION
  Preset the number of retries to 3 and 10

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
static void gsdi_uim_server_preset_num_retries (
  gsdi_slot_id_type            slot,
  gsdi_pin_ids_T               pin_id
)
{
  switch (slot)
  {
  case GSDI_SLOT_1:
    mmgsdi_util_preset_retries(MMGSDI_SLOT_1);
    return;
  case GSDI_SLOT_2:
    break;
  default:
    MSG_ERROR("Invalid Slot 0x%x", slot, 0, 0);
    return;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* Get number of retry left for PINs and PUKs */
  switch (pin_id)
  {
  case GSDI_PIN1:
    gsdi_data.pin_status_info_2.pin1_num_retries_allowed = 3;
    gsdi_data.pin_status_info_2.puk1_num_retries_allowed = 10;
    break;
  case GSDI_PIN2:
    gsdi_data.pin_status_info_2.pin2_num_retries_allowed = 3;
    gsdi_data.pin_status_info_2.puk2_num_retries_allowed = 10;
    break;
  default:
    MSG_ERROR("Invalid pin value 0x%x", pin_id, 0, 0);
    return;
  }
  return;
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/
} /* gsdi_uim_server_preset_num_retries */


/*===========================================================================
FUNCTION GSDI_UIM_SERVER_FIND_PIN_SUCCESS_EVT

DESCRIPTION
  Find Success PIN Action event

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_Tn.
===========================================================================*/
static gsdi_returns_T gsdi_uim_server_find_pin_success_evt (
  gsdi_slot_id_type            slot,
  gsdi_pin_ids_T               pin_id,
  int32                        req_id,
  gsdi_sim_events_T           *event_ptr
)
{
  int i = 0;

  if (event_ptr == NULL)
    return GSDI_ERROR;

  for (i = 0; i < GSDI_PIN_SUCCESS_NUM_EVT; i++)
  {
    if ((gsdi_pin_success_evt_table[i].slot == slot) &&
        (gsdi_pin_success_evt_table[i].pin_id == pin_id) &&
        (gsdi_pin_success_evt_table[i].req_id == req_id))
    {
      *event_ptr = gsdi_pin_success_evt_table[i].evt;
      return GSDI_SUCCESS;
    }
  }
  return GSDI_ERROR;
}

#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_CDMA)
/*===========================================================================
FUNCTION GSDI_UIM_SERVER_GSM_CDMA_GET_MAX_NUM_RETRIES

DESCRIPTION
  Does a select to find out the number of retries for PIN and PUK

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_Tn.
===========================================================================*/
static gsdi_returns_T gsdi_uim_server_gsm_cdma_get_max_num_retries (
    gsdi_slot_id_type            slot,
    gsdi_pin_ids_T               pin_id,
    int32                        req_id,
    gsdi_sim_events_T           *event_ptr
)
{
  gsdi_returns_T        gsdi_status = GSDI_ERROR;
  byte                  dir_info_len      = 0;
  byte                  dir_info[GSDI_DEFAULT_FILE_HEADER_LEN] = {0};
  pin_status_info_T    *pin_status_info = NULL;
  pin_status_info_T     slot1_pin_status_info;

  MSG_MED("GSDI uim server gsm get max num retries",0,0,0);

  if (event_ptr == NULL)
    return GSDI_ERROR;

  switch (slot)
  {
  case GSDI_SLOT_1:
    pin_status_info = &slot1_pin_status_info;
    break;
  case GSDI_SLOT_2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    pin_status_info = &gsdi_data.pin_status_info_2;
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  default:
    MSG_ERROR("Invalid Slot 0x%x", slot, 0, 0);
    return GSDI_ERROR;
  }


  /* Select GSM DF for header info */
  if ((gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK) == GSDI_GSM_SIM_MASK)
  {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */
  }
  else
  {
#ifdef FEATURE_MMGSDI_CDMA
    gsdi_status = gsdi_uim_server_cdma_select( GSDI_GET_FILE_ATTRIBUTES_REQ,
                                               slot,
                                              (uim_items_enum_type)0,
                                              UIM_DF,
                                              GSDI_RUIM_DIR,
                                              NULL,
                                              &dir_info_len,
                                              dir_info );
#endif /* FEATURE_MMGSDI_CDMA */
  }


  /* Extract the pin status from the header info */
  if ( gsdi_status == GSDI_SUCCESS )
  {
    /* Get number of retry left for PINs and PUKs */
    switch (pin_id)
    {
    case GSDI_PIN1:
      pin_status_info->pin1_num_retries_allowed = dir_info[18] & 0x0F;
      pin_status_info->puk1_num_retries_allowed = dir_info[19] & 0x0F;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      if(slot == GSDI_SLOT_1)
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/
     {
        (void)(mmgsdi_util_convert_to_mmgsdi_pin_info(pin_id,
                                                      pin_status_info->pin1_num_retries_allowed,
                                                      GSDI_VERIFY_PIN_REQ));
        (void)(mmgsdi_util_convert_to_mmgsdi_pin_info(pin_id,
                                                      pin_status_info->puk1_num_retries_allowed,
                                                      GSDI_UNBLOCK_PIN_REQ));
      }
      break;
    case GSDI_PIN2:
      pin_status_info->pin2_num_retries_allowed = dir_info[20] & 0x0F;
      pin_status_info->puk2_num_retries_allowed = dir_info[21] & 0x0F;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      if(slot == GSDI_SLOT_1)
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/
      {
        (void)(mmgsdi_util_convert_to_mmgsdi_pin_info(pin_id,
                                                      pin_status_info->pin2_num_retries_allowed,
                                                      GSDI_VERIFY_PIN_REQ));
        (void)(mmgsdi_util_convert_to_mmgsdi_pin_info(pin_id,
                                                      pin_status_info->puk2_num_retries_allowed,
                                                      GSDI_UNBLOCK_PIN_REQ));
      }
      break;
    default:
      MSG_ERROR("Invalid pin value 0x%x", pin_id, 0, 0);
      return GSDI_ERROR;
    }
  }
  else
  {
    MSG_ERROR("Unable to get pin num retries 0x%x", gsdi_status, 0, 0);
    return gsdi_status;
  }

  gsdi_status = gsdi_uim_server_find_pin_success_evt(slot, pin_id, req_id, event_ptr);

  MSG_MED("GSDI uim server gsm get max num retries %d",gsdi_status,0,0);
  return gsdi_status;
} /* gsdi_uim_server_gsm_cdma_get_max_num_retries */
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_CDMA */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

/* <EJECT> */
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS*/


#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */


#ifdef FEATURE_MMGSDI_CDMA
/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_CHANGE_CHV

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_Tn.
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_change_chv (
    gsdi_slot_id_type  slot,
    gsdi_pin_ids_T    pin_id,
    byte *            old_pin,
    byte *            new_pin,
    byte *            data_p
)
{

    uim_cmd_type *            uim_cmd_ptr;
    gsdi_returns_T            gsdi_status = GSDI_SUCCESS;
    gsdi_sim_events_T         event_to_mmi= GSDI_NO_SIM_EVENT;
    mmgsdi_int_app_info_type *sel_app_ptr = mmgsdi_util_get_sel_app();

    MSG_LOW("Start uim_server_cdma_change_chv",0,0,0);
    MMGSDI_RETURN_IF_NULL(sel_app_ptr);

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_CHANGE_CHV_F;
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Deallocate the cmd from heap */
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }

        switch ( pin_id )
        {
        case GSDI_PIN1:
            uim_cmd_ptr->change_chv.chv = UIM_CHV1;
            break;

        case GSDI_PIN2:
            uim_cmd_ptr->change_chv.chv = UIM_CHV2;
            break;

        default:
            MSG_ERROR("BAD PIN ID: 0x%x",pin_id,0,0);
            uim_cmd_ptr->change_chv.chv = UIM_CHV_NONE;
            break;
        }

        (void) memcpy( uim_cmd_ptr->change_chv.chv_digits,
                       old_pin,
                       GSDI_MAX_PIN_LEN );

        (void) memcpy( uim_cmd_ptr->change_chv.new_chv_digits,
                       new_pin,
                       GSDI_MAX_PIN_LEN );

        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

        /* Safety check for proper report  */
        if (uim_rsp_report.rpt_type == UIM_CHANGE_CHV_R)
        {
            gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                          uim_rsp_report.rpt_status,
                                                          (uint32)uim_rsp_report.sw1,
                                                          (uint32)uim_rsp_report.sw2 );
            if(data_p != NULL)
            {
              /* Copy the status words into data pointer to pass back to calling app */
              data_p[0] = (byte)uim_rsp_report.sw1;
              data_p[1] = (byte)uim_rsp_report.sw2;
            }
            else
            {
              MSG_HIGH("Unable to return status words from GSDI, Null data ptr",0,0,0);
            }

            if (gsdi_status == GSDI_SUCCESS)
            {
                gsdi_status = gsdi_uim_server_gsm_cdma_get_max_num_retries(
                  slot, pin_id, GSDI_CHANGE_PIN_REQ, &event_to_mmi);
                if (gsdi_status != GSDI_SUCCESS)
                {
                  MSG_ERROR("Unable to get max num retries 0x%x", gsdi_status, 0, 0);
                  /* preset the pin num retries to 3 and puk retries to 10 so that the
                     phone will have minimal valid num retries data when the num retries were not able
                     to be updated correctly from the card */
                  gsdi_uim_server_preset_num_retries(slot, pin_id);
                  /* reset the status */
                  gsdi_status = GSDI_SUCCESS;
                }
            }
            else
            {  /* UIM_FAIL */
                MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                             uim_rsp_report.sw2,
                                                             gsdi_status);
                switch (gsdi_status)
                {
                case GSDI_INCORRECT_CODE:
                    switch ( slot )
                    {
                    case GSDI_SLOT_1:
                      if ( pin_id == GSDI_PIN1 )
                      {
                        MMGSDI_RETURN_IF_NULL(sel_app_ptr->pin1);
                        if (sel_app_ptr->pin1->num_retries != 0)
                        {
                          --sel_app_ptr->pin1->num_retries;
                        }
                      }
                      else if ( pin_id == GSDI_PIN2 )
                      {
                        if (sel_app_ptr->pin2.num_retries != 0)
                        {
                          --sel_app_ptr->pin2.num_retries;
                        }
                      }
                      break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
                    case GSDI_SLOT_2:
                        if ( pin_id == GSDI_PIN1 )
                        {
                          if (gsdi_data.pin_status_info_2.pin1_num_retries_allowed != 0)
                            gsdi_data.pin_status_info_2.pin1_num_retries_allowed--;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                          if (gsdi_data.pin_status_info_2.pin2_num_retries_allowed != 0)
                            gsdi_data.pin_status_info_2.pin2_num_retries_allowed--;
                        }
                        break;

#endif /* FEATURE_MMGSDI_DUAL_SLOT */
                    default:
                        MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
                        break;
                    } /* switch slot */
                    break;

                case GSDI_CODE_BLOCKED:

                    switch ( slot )
                    {
                      case GSDI_SLOT_1:
                        if(mmgsdi_util_convert_to_mmgsdi_pin_status(pin_id,
                                                                    GSDI_PIN_BLOCKED)
                          != GSDI_SUCCESS)
                        {
                           MSG_ERROR("Could not convert to MMGSDI PIN Status",0,0,0);
                        }
                        if ( pin_id == GSDI_PIN1 )
                        {
                          event_to_mmi = GSDI_PIN1_BLOCKED;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                          event_to_mmi = GSDI_PIN2_BLOCKED;
                        }
                        break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
                    case GSDI_SLOT_2:
                        if ( pin_id == GSDI_PIN1 )
                        {
                          gsdi_data.pin_status_info_2.pin1_num_retries_allowed = 0;
                          event_to_mmi = GSDI_PIN1_BLOCKED_2;
                          gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_BLOCKED;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                          gsdi_data.pin_status_info_2.pin2_num_retries_allowed = 0;
                          event_to_mmi = GSDI_PIN2_BLOCKED_2;
                          gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_BLOCKED;
                        }
                        break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

                    default:
                        MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
                        break;
                    } /* switch slot */
                    break;

                default:
                    MSG_HIGH("Other gsdi_status %d", gsdi_status, 0, 0);
                    break;
                } /* gsdi _status */
            }
        } /* rpt_type = UIM_DISABLE_CHV_R */
    } /* uim_cmd_ptr != NULL */
    else
    {
        /* Null ptr for buffer */
        gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_ERROR )
    {
        /* Has an error indication been set? */
        if ( event_to_mmi == GSDI_NO_SIM_EVENT )
            event_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( event_to_mmi != GSDI_NO_SIM_EVENT )
    {
        if(gsdi_util_send_mmi_sim_ind( event_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", event_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",event_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;

} /* gsdi_uim_server_cdma_change_chv */


/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_DISABLE_CHV

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_disable_chv (
    gsdi_slot_id_type  slot,
    gsdi_pin_ids_T  pin_id,
    byte *          pin,
    byte *          data_p
)
{
    uim_cmd_type *            uim_cmd_ptr;
    gsdi_returns_T            gsdi_status  = GSDI_SUCCESS;
    gsdi_sim_events_T         event_to_mmi = GSDI_NO_SIM_EVENT;
    mmgsdi_int_app_info_type *sel_app_ptr  = mmgsdi_util_get_sel_app();

    MSG_LOW("Start uim_server_cdma_disable_chv",0,0,0);

    MMGSDI_RETURN_IF_NULL(sel_app_ptr);
    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_DISABLE_CHV_F;
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Deallocate the cmd from heap */
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }

        switch ( pin_id )
        {
        case GSDI_PIN1:
            uim_cmd_ptr->disable_chv.chv = UIM_CHV1;
            break;

        case GSDI_PIN2:
            uim_cmd_ptr->disable_chv.chv = UIM_CHV2;
            break;

        default:
            MSG_ERROR("BAD PIN ID: 0x%x",pin_id,0,0);
            break;
        }

        (void) memcpy( uim_cmd_ptr->disable_chv.chv_digits,
                       pin,
                       GSDI_MAX_PIN_LEN );

        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

        /* Safety check for proper report  */
        if (uim_rsp_report.rpt_type == UIM_DISABLE_CHV_R)
        {
            gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                          uim_rsp_report.rpt_status,
                                                          (uint32)uim_rsp_report.sw1,
                                                          (uint32)uim_rsp_report.sw2);
            if(data_p != NULL)
            {
              /* Copy the status words into data pointer to pass back to calling app */
              data_p[0] = (byte)uim_rsp_report.sw1;
              data_p[1] = (byte)uim_rsp_report.sw2;
            }
            else
            {
              MSG_HIGH("Unable to return status words from GSDI, Null data ptr",0,0,0);
            }

            if (gsdi_status == GSDI_SUCCESS)
            {
                gsdi_status = gsdi_uim_server_gsm_cdma_get_max_num_retries(
                    slot, pin_id, GSDI_DISABLE_PIN_REQ, &event_to_mmi);
                if (gsdi_status != GSDI_SUCCESS)
                {
                    MSG_ERROR("Unable to get max num retries 0x%x", gsdi_status, 0, 0);
                    /* preset the pin num retries to 3 and puk retries to 10 so that the
                     phone will have minimal valid num retries data when the num retries were not able
                     to be updated correctly from the card */
                    gsdi_uim_server_preset_num_retries(slot, pin_id);
                    /* reset the status */
                    gsdi_status = GSDI_SUCCESS;
                }
            }
            else
            {
                /* UIM_FAIL */
                MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                             uim_rsp_report.sw2,
                                                             gsdi_status);
                switch (gsdi_status)
                {
                case GSDI_INCORRECT_CODE:
                    switch ( slot )
                    {
                      case GSDI_SLOT_1:
                        if ( pin_id == GSDI_PIN1 )
                        {
                          MMGSDI_RETURN_IF_NULL(sel_app_ptr->pin1);
                          if (sel_app_ptr->pin1->num_retries != 0)
                          {
                            --sel_app_ptr->pin1->num_retries;
                          }
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                          if (sel_app_ptr->pin2.num_retries != 0)
                          {
                            --sel_app_ptr->pin2.num_retries;
                          }
                        }
                        break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                    case GSDI_SLOT_2:
                        if ( pin_id == GSDI_PIN1 )
                        {
                            if (gsdi_data.pin_status_info_2.pin1_num_retries_allowed != 0)
                                gsdi_data.pin_status_info_2.pin1_num_retries_allowed--;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                            if (gsdi_data.pin_status_info_2.pin2_num_retries_allowed != 0)
                                gsdi_data.pin_status_info_2.pin1_num_retries_allowed--;
                        }
                        break;
#endif
                    default:
                        MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
                        break;
                    }
                    break;

                case GSDI_CODE_BLOCKED:
                    switch ( slot )
                    {
                      case GSDI_SLOT_1:
                        if(mmgsdi_util_convert_to_mmgsdi_pin_status(pin_id,
                                                                    GSDI_PIN_BLOCKED)
                           != GSDI_SUCCESS)
                        {
                          MSG_ERROR("Could not convert to MMGSDI PIN Status",0,0,0);
                        }
                        if ( pin_id == GSDI_PIN1 )
                        {
                          event_to_mmi = GSDI_PIN1_BLOCKED;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                          event_to_mmi = GSDI_PIN2_BLOCKED;
                        }
                        break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                    case GSDI_SLOT_2:
                        if ( pin_id == GSDI_PIN1 )
                        {
                            gsdi_data.pin_status_info_2.pin1_num_retries_allowed = 0;
                            event_to_mmi = GSDI_PIN1_BLOCKED_2;
                            gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_BLOCKED;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                            gsdi_data.pin_status_info_2.pin2_num_retries_allowed = 0;
                            event_to_mmi = GSDI_PIN2_BLOCKED_2;
                            gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_BLOCKED;
                        }
                        break;
#endif
                    default:
                        MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
                        break;
                    }
                    break;

                default:
                    MSG_HIGH("Other gsdi_status %d", gsdi_status, 0, 0);
                    break;
                } /* gsdi_status */
            }
        } /* rpt_type = UIM_DISABLE_CHV_R */
    } /* uim_cmd_ptr != NULL */
    else
    {
        /* Null ptr for buffer */
        gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_ERROR )
    {
        /* Has an error indication been set? */
        if ( event_to_mmi == GSDI_NO_SIM_EVENT )
            event_to_mmi = GSDI_CARD_ERROR;
    }
    /* Let MMI know if there were any error */
    if ( event_to_mmi != GSDI_NO_SIM_EVENT )
    {
        if(gsdi_util_send_mmi_sim_ind( event_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", event_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",event_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;

} /* gsdi_uim_server_cdma_disable_chv */

/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_ENABLE_CHV

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_enable_chv (
    gsdi_slot_id_type slot,
    gsdi_pin_ids_T    pin_id,
    byte *            pin,
    byte *            data_p
)
{
    uim_cmd_type *            uim_cmd_ptr;
    gsdi_returns_T            gsdi_status    = GSDI_SUCCESS;
    gsdi_sim_events_T         event_to_mmi   = GSDI_NO_SIM_EVENT;
    mmgsdi_int_app_info_type *sel_app_ptr    = mmgsdi_util_get_sel_app();

    MSG_LOW("Start uim_server_cdma_enable_pin",0,0,0);

    MMGSDI_RETURN_IF_NULL(sel_app_ptr);
    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_ENABLE_CHV_F;
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Deallocate the cmd from heap */
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }

        switch ( pin_id )
        {
        case GSDI_PIN1:
            uim_cmd_ptr->enable_chv.chv = UIM_CHV1;
            break;

        case GSDI_PIN2:
            uim_cmd_ptr->enable_chv.chv = UIM_CHV2;
            break;

        default:
            MSG_ERROR("BAD PIN ID: 0x%x",pin_id,0,0);
            uim_cmd_ptr->enable_chv.chv = UIM_CHV_NONE;
            break;
        }

        (void) memcpy( uim_cmd_ptr->enable_chv.chv_digits,
                       pin,
                       GSDI_MAX_PIN_LEN );

        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

        MSG_MED("Resp uim_server_gsm_enable_chv",0,0,0);

        /* Safety check for proper report  */
        if (uim_rsp_report.rpt_type == UIM_ENABLE_CHV_R)
        {
            gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                          uim_rsp_report.rpt_status,
                                                          (uint32)uim_rsp_report.sw1,
                                                          (uint32)uim_rsp_report.sw2);
            if(data_p != NULL)
            {
              /* Copy the status words into data pointer to pass back to calling app */
              data_p[0] = (byte)uim_rsp_report.sw1;
              data_p[1] = (byte)uim_rsp_report.sw2;
            }
            else
            {
              MSG_HIGH("Unable to return status words from GSDI, Null data ptr",0,0,0);
            }

            if (gsdi_status == GSDI_SUCCESS)
            {
                gsdi_status = gsdi_uim_server_gsm_cdma_get_max_num_retries(
                  slot, pin_id, GSDI_ENABLE_PIN_REQ, &event_to_mmi);
                if (gsdi_status != GSDI_SUCCESS)
                {
                  MSG_ERROR("Unable to get max num retries 0x%x", gsdi_status, 0, 0);
                  /* preset the pin num retries to 3 and puk retries to 10 so that the
                     phone will have minimal valid num retries data when the num retries were not able
                     to be updated correctly from the card */
                  gsdi_uim_server_preset_num_retries(slot, pin_id);
                  /* reset the status */
                  gsdi_status = GSDI_SUCCESS;
                }
            }
            else
            {
                /* UIM_FAIL */
                MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                          uim_rsp_report.sw2,
                                                          gsdi_status);
                switch (gsdi_status)
                {
                case GSDI_INCORRECT_CODE:

                    switch ( slot )
                    {
                      case GSDI_SLOT_1:
                        if ( pin_id == GSDI_PIN1 )
                        {
                          MMGSDI_RETURN_IF_NULL(sel_app_ptr->pin1);
                          if (sel_app_ptr->pin1->num_retries != 0)
                          {
                            --sel_app_ptr->pin1->num_retries;
                          }
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                          if (sel_app_ptr->pin2.num_retries != 0)
                          {
                            --sel_app_ptr->pin2.num_retries;
                          }
                        }
                        break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
                    case GSDI_SLOT_2:
                        if ( pin_id == GSDI_PIN1 )
                        {
                            if (gsdi_data.pin_status_info_2.pin1_num_retries_allowed != 0)
                                gsdi_data.pin_status_info_2.pin1_num_retries_allowed--;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                            if (gsdi_data.pin_status_info_2.pin2_num_retries_allowed != 0)
                                gsdi_data.pin_status_info_2.pin2_num_retries_allowed--;
                        }
                        break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

                    default:
                      MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
                      break;
                    }
                    break;

                case GSDI_CODE_BLOCKED:
                    switch ( slot )
                    {
                      case GSDI_SLOT_1:
                        if(mmgsdi_util_convert_to_mmgsdi_pin_status(pin_id,
                                                                    GSDI_PIN_BLOCKED)
                           != GSDI_SUCCESS)
                        {
                          MSG_ERROR("Could not convert to MMGSDI PIN Status",0,0,0);
                        }
                        if ( pin_id == GSDI_PIN1 )
                        {
                          event_to_mmi = GSDI_PIN1_BLOCKED;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                          event_to_mmi = GSDI_PIN2_BLOCKED;
                        }
                        break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
                    case GSDI_SLOT_2:
                        if ( pin_id == GSDI_PIN1 )
                        {
                            gsdi_data.pin_status_info_2.pin1_num_retries_allowed = 0;
                            event_to_mmi = GSDI_PIN1_BLOCKED_2;
                            gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_BLOCKED;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                            gsdi_data.pin_status_info_2.pin2_num_retries_allowed = 0;
                            event_to_mmi = GSDI_PIN2_BLOCKED_2;
                            gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_BLOCKED;
                        }
                        break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

                    default:
                        MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
                        break;
                    }
                    break;

                default:
                    MSG_HIGH("Other gsdi_status %d", gsdi_status, 0, 0);
                    break;
                } /* gsdi status */
            } /* UIM_FAIL */
        } /* ENABLE_CHV_R */
    } /* uim_cmd_ptr != NULL */
    else
    {
      gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_ERROR )
    {
      /* Has an error indication been set? */
      if ( event_to_mmi == GSDI_NO_SIM_EVENT )
        event_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( event_to_mmi != GSDI_NO_SIM_EVENT )
    {
        if(gsdi_util_send_mmi_sim_ind( event_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", event_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",event_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;

} /* gsdi_uim_server_usim_enable_pin */


/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_READ

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_read (
    gsdi_sim_read_req_T *  req_p,
    byte *                 rec_num_accessed_p,
    byte *                 num_records_p,
    byte *                 returned_data_len_p,
    byte *                 data_p
)
{
    uim_cmd_type *    uim_cmd_ptr    = NULL_PTR;
    gsdi_returns_T    gsdi_status    = GSDI_SUCCESS;
    gsdi_sim_events_T warning_to_mmi = GSDI_NO_SIM_EVENT;

    MMGSDI_RETURN_IF_NULL(req_p);

    MSG_HIGH("Start uim_server_cdma_read for EF 0x%x with offset %d",
             req_p->sim_filename, req_p->data_offset,0);

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_ACCESS_F;
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              req_p->message_header.slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Deallocate the cmd from heap */
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }

        uim_cmd_ptr->access_uim.access   = UIM_READ_F;
        uim_cmd_ptr->access_uim.rec_mode = UIM_ABSOLUTE;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              req_p->message_header.slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
          MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
          return gsdi_status;
        }

        /* Is a path required for this EF? */
        uim_cmd_ptr->access_uim.item = (uim_items_enum_type)req_p->sim_filename;

        if (req_p->rec_num != 0)
          uim_cmd_ptr->access_uim.offset = (word)req_p->rec_num;
        else
          uim_cmd_ptr->access_uim.offset = (word)req_p->data_offset;

        uim_cmd_ptr->access_uim.num_bytes = GSDI_MAX_DATA_BLOCK_LEN;
        uim_cmd_ptr->access_uim.data_ptr  = data_p;

        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

        /* Safety check for proper report  */
        if (uim_rsp_report.rpt_type == UIM_ACCESS_R)
        {
            gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                          uim_rsp_report.rpt_status,
                                                          (uint32)uim_rsp_report.sw1,
                                                          (uint32)uim_rsp_report.sw2);

            if (gsdi_status == GSDI_SUCCESS)
            {
                *returned_data_len_p = (byte)uim_cmd_ptr->access_uim.num_bytes_rsp;
                *rec_num_accessed_p  = req_p->rec_num;
                *num_records_p       = (byte)uim_cmd_ptr->access_uim.num_records_rsp;
            }
            else
            {
                /* UIM_FAIL */
                MSG_HIGH("UIM_FAIL For EF 0x%x",(uim_items_enum_type)req_p->sim_filename,0,0);
                MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                              uim_rsp_report.sw2,
                                                              gsdi_status);
            } /* UIM Fail */
        }
        else
        {
            /* Invalid report returned */
            gsdi_status = GSDI_ERROR;
        }
    }
    else
    {
        /* Null ptr for buffer */
        gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_ERROR )
    {
        /* Has an error indication been set? */
        warning_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( warning_to_mmi != GSDI_NO_SIM_EVENT )
    {
        if(gsdi_util_send_mmi_sim_ind( warning_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", warning_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",warning_to_mmi,0,0);
    }
    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;

} /* gsdi_uim_server_cdma_read */



/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_RUIM_GET_IMAGE

DESCRIPTION
  This function populates the UIM command buffer correctly with an
  ACCESS request and wait synchronously for UIM responses.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_ruim_get_image(
    gsdi_get_image_req_T   * req_p,
    uint8 *                 rec_num_accessed_p,
    uint8 *                 num_records_p,
    int32 *                 returned_data_length_p,
    uint8 *                 data_p
)
{
    uim_cmd_type *    uim_cmd_ptr    = NULL_PTR;
    gsdi_returns_T    gsdi_status    = GSDI_SUCCESS;
    gsdi_sim_events_T warning_to_mmi = GSDI_NO_SIM_EVENT;

    MSG_LOW("Start gsdi_uim_server_cdma_ruim_get_image",0,0,0);

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_ACCESS_F;
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              req_p->message_header.slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Deallocate the cmd from heap */
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }

        uim_cmd_ptr->access_uim.access   = UIM_READ_F;
        uim_cmd_ptr->access_uim.rec_mode = UIM_ABSOLUTE;

        /* check for rec number, if the record number to be accessed
           is set to 0, then fill in the data offset instead */
        if (req_p->rec_num != 0)
            uim_cmd_ptr->access_uim.offset = (word)req_p->rec_num;
        else
            uim_cmd_ptr->access_uim.offset = (word)req_p->file_data_offset;

        /* Check for read len, since GSM and CDMA sim access doesn't support
           0x00 data len, if the user enters GSDI_STANDARD_READ_LEN,
           translate that to maximum data block len */
        if(req_p->required_data_len == GSDI_STANDARD_READ_LEN)
            uim_cmd_ptr->access_uim.num_bytes = GSDI_MAX_DATA_BLOCK_LEN;
        else
            uim_cmd_ptr->access_uim.num_bytes = req_p->required_data_len;  /* return the max */

        uim_cmd_ptr->access_uim.data_ptr  = data_p;

        /* Is a path required for this EF? */
        uim_cmd_ptr->access_uim.item  = (uim_items_enum_type)req_p->sim_filename;

        if( uim_cmd_ptr->access_uim.item == UIM_EF_BY_PATH )
        {
            /* initialize path array */
            memset(uim_cmd_ptr->access_uim.path,
                   0xFF,
                   sizeof(uim_cmd_ptr->access_uim.path));

            /* Check for validity */
            if ( ( req_p->path_len > 0 ) &&
                 ( req_p->path_len <= UIM_MAX_PATH_ELEMENTS ) &&
                 ( req_p->path != NULL ) )
            {
                memcpy( uim_cmd_ptr->access_uim.path,
                        req_p->path,
                        sizeof(word)*req_p->path_len);
            }
            else
            {
                MSG_ERROR("wrong path len or null path ptr %d", req_p->path_len, 0, 0);
                MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
                gsdi_status = GSDI_ERROR;
            }
        }

        if (gsdi_status == GSDI_SUCCESS)
        {
            gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

            /* Safety check for proper report  */
            if (uim_rsp_report.rpt_type == UIM_ACCESS_R)
            {
                gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                              uim_rsp_report.rpt_status,
                                                              (uint32)uim_rsp_report.sw1,
                                                              (uint32)uim_rsp_report.sw2);

                if (gsdi_status == GSDI_SUCCESS)
                {
                    *returned_data_length_p = (byte)uim_cmd_ptr->access_uim.num_bytes_rsp;
                    *rec_num_accessed_p     = req_p->rec_num;
                    *num_records_p          = (byte)uim_cmd_ptr->access_uim.num_records_rsp;

                }
                else
                {
                    /* UIM_FAIL */
                    MSG_HIGH("UIM_FAIL For EF 0x%x",(uim_items_enum_type)req_p->sim_filename,0,0);
                    MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                                  uim_rsp_report.sw2,
                                                                  gsdi_status);
                } /* UIM Fail */
            }
            else
            { /* not UIM_ACCESS_R */
                /* Invalid report returned */
                gsdi_status = GSDI_ERROR;
            }
        } /* gsdi_status == GSDI_SUCCESS => path len and path ptr are valid when
            the filename in request is EF_BY_PATH */
    }
    else
    {
        /* Null ptr for buffer */
        gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_ERROR )
    {
        /* Has an error indication been set? */
        warning_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( warning_to_mmi != GSDI_NO_SIM_EVENT )
    {
        if(gsdi_util_send_mmi_sim_ind( warning_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", warning_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",warning_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;

} /* gsdi_uim_server_cdma_ruim_get_image */


/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_RUIM_ACCESS

DESCRIPTION
  This function populates the UIM command buffer correctly with an
  ACCESS request and wait synchronously for UIM responses.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_ruim_access(
    gsdi_sim_access_req_T * req_p,
    uint8 *                 rec_num_accessed_p,
    uint8 *                 num_records_p,
    int32  *                 returned_data_length_p,
    uint8 *                 data_p
)
{
    uim_cmd_type *    uim_cmd_ptr    = NULL_PTR;
    gsdi_returns_T    gsdi_status    = GSDI_SUCCESS;
    gsdi_sim_events_T warning_to_mmi = GSDI_NO_SIM_EVENT;

    MSG_LOW("Start gsdi_uim_server_cdma_ruim_access",0,0,0);

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_ACCESS_F;
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              req_p->message_header.slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Deallocate the cmd from heap */
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }

        uim_cmd_ptr->access_uim.access   = UIM_READ_F;
        uim_cmd_ptr->access_uim.rec_mode = UIM_ABSOLUTE;

        /* check for rec number, if the record number to be accessed
           is set to 0, then fill in the data offset instead */
        if (req_p->rec_num != 0)
            uim_cmd_ptr->access_uim.offset = (word)req_p->rec_num;
        else
            uim_cmd_ptr->access_uim.offset = (word)req_p->file_data_offset;

        /* Check for read len, since GSM and CDMA sim access doesn't support
           0x00 data len, if the user enters GSDI_STANDARD_READ_LEN,
           translate that to maximum data block len */
        if(req_p->required_data_len == GSDI_STANDARD_READ_LEN)
            uim_cmd_ptr->access_uim.num_bytes = GSDI_MAX_DATA_BLOCK_LEN;
        else
            uim_cmd_ptr->access_uim.num_bytes = req_p->required_data_len;  /* return the max */

        uim_cmd_ptr->access_uim.data_ptr  = data_p;

        /* Is a path required for this EF? */
        uim_cmd_ptr->access_uim.item  = (uim_items_enum_type)req_p->sim_filename;

        if( uim_cmd_ptr->access_uim.item == UIM_EF_BY_PATH )
        {
            /* initialize path array */
            memset(uim_cmd_ptr->access_uim.path,
                   0xFF,
                   sizeof(uim_cmd_ptr->access_uim.path));

            /* Check for validity */
            if ( ( req_p->path_len > 0 ) &&
                 ( req_p->path_len <= UIM_MAX_PATH_ELEMENTS ) &&
                 ( req_p->path != NULL ) )
            {
                memcpy( uim_cmd_ptr->access_uim.path,
                        req_p->path,
                        sizeof(word)*req_p->path_len);
            }
            else
            {
                MSG_ERROR("wrong path len or null path ptr %d", req_p->path_len, 0, 0);
                gsdi_status = GSDI_ERROR;
            }
        }

        if (gsdi_status == GSDI_SUCCESS)
        {
            gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

            /* Safety check for proper report  */
            if (uim_rsp_report.rpt_type == UIM_ACCESS_R)
            {
                gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                              uim_rsp_report.rpt_status,
                                                              (uint32)uim_rsp_report.sw1,
                                                              (uint32)uim_rsp_report.sw2);

                if (gsdi_status == GSDI_SUCCESS)
                {
                    *returned_data_length_p = (int32)uim_cmd_ptr->access_uim.num_bytes_rsp;
                    *rec_num_accessed_p     = req_p->rec_num;
                    *num_records_p          = (byte)uim_cmd_ptr->access_uim.num_records_rsp;

                }
                else
                {
                    /* UIM_FAIL */
                    MSG_HIGH("UIM_FAIL For EF 0x%x",(uim_items_enum_type)req_p->sim_filename,0,0);
                    MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                                  uim_rsp_report.sw2,
                                                                  gsdi_status);
                } /* UIM Fail */
            }
            else
            { /* not UIM_ACCESS_R */
                /* Invalid report returned */
                gsdi_status = GSDI_ERROR;
            }
        } /* gsdi_status == GSDI_SUCCESS => path len and path ptr are valid when
            the filename in request is EF_BY_PATH */
    }
    else
    {
        /* Null ptr for buffer */
        gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_ERROR )
    {
        /* Has an error indication been set? */
        warning_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( warning_to_mmi != GSDI_NO_SIM_EVENT )
    {
        if(gsdi_util_send_mmi_sim_ind( warning_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", warning_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",warning_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;

} /* gsdi_uim_server_cdma_ruim_access */

/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_GET_IMAGE

DESCRIPTION
  This function populates the UIM command buffer correctly with an
  ACCESS request and wait synchronously for UIM responses

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_get_image(
   gsdi_get_image_req_T   *req_p,
   uint8 *                 rec_num_accessed_p,
   uint8 *                 num_records_p,
   int32                  *returned_data_length_p,
   uint8                  *data_p )
{
    uim_cmd_type *    uim_cmd_ptr    = NULL_PTR;
    gsdi_returns_T    gsdi_status    = GSDI_SUCCESS;
    gsdi_sim_events_T warning_to_mmi = GSDI_NO_SIM_EVENT;

    MSG_LOW("Start gsdi_uim_server_cdma_ruim_access",0,0,0);

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_ACCESS_F;
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              req_p->message_header.slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Deallocate the cmd from heap */
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }

        uim_cmd_ptr->access_uim.access   = UIM_READ_F;
        uim_cmd_ptr->access_uim.rec_mode = UIM_ABSOLUTE;

        /* check for rec number, if the record number to be accessed
           is set to 0, then fill in the data offset instead */
        if (req_p->rec_num != 0)
            uim_cmd_ptr->access_uim.offset = (word)req_p->rec_num;
        else
            uim_cmd_ptr->access_uim.offset = (word)req_p->file_data_offset;

        /* Check for read len, since GSM and CDMA sim access doesn't support
           0x00 data len, if the user enters GSDI_STANDARD_READ_LEN,
           translate that to maximum data block len */
        if(req_p->required_data_len == GSDI_STANDARD_READ_LEN)
            uim_cmd_ptr->access_uim.num_bytes = GSDI_MAX_DATA_BLOCK_LEN;
        else
            uim_cmd_ptr->access_uim.num_bytes = req_p->required_data_len;  /* return the max */

        uim_cmd_ptr->access_uim.data_ptr  = data_p;

        /* Is a path required for this EF? */
        uim_cmd_ptr->access_uim.item  = (uim_items_enum_type)req_p->sim_filename;

        if( uim_cmd_ptr->access_uim.item == UIM_EF_BY_PATH )
        {
            /* initialize path array */
            memset(uim_cmd_ptr->access_uim.path,
                   0xFF,
                   sizeof(uim_cmd_ptr->access_uim.path));

            /* Check for validity */
            if ( ( req_p->path_len > 0 ) &&
                 ( req_p->path_len <= UIM_MAX_PATH_ELEMENTS ) &&
                 ( req_p->path != NULL ) )
            {
                memcpy( uim_cmd_ptr->access_uim.path,
                        req_p->path,
                        sizeof(word)*req_p->path_len);
            }
            else
            {
                MSG_ERROR("wrong path len or null path ptr %d", req_p->path_len, 0, 0);
                gsdi_status = GSDI_ERROR;
            }
        }

        if (gsdi_status == GSDI_SUCCESS)
        {
            gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

            /* Safety check for proper report  */
            if (uim_rsp_report.rpt_type == UIM_ACCESS_R)
            {
                gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                              uim_rsp_report.rpt_status,
                                                              (uint32)uim_rsp_report.sw1,
                                                              (uint32)uim_rsp_report.sw2);

                if (gsdi_status == GSDI_SUCCESS)
                {
                    *returned_data_length_p = (int32)uim_cmd_ptr->access_uim.num_bytes_rsp;
                    *rec_num_accessed_p     = req_p->rec_num;
                    *num_records_p          = (byte)uim_cmd_ptr->access_uim.num_records_rsp;

                }
                else
                {
                    /* UIM_FAIL */
                    MSG_HIGH("UIM_FAIL For EF 0x%x",(uim_items_enum_type)req_p->sim_filename,0,0);
                    MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                                  uim_rsp_report.sw2,
                                                                  gsdi_status);
                } /* UIM Fail */
            }
            else
            { /* not UIM_ACCESS_R */
                /* Invalid report returned */
                gsdi_status = GSDI_ERROR;
            }
        } /* gsdi_status == GSDI_SUCCESS => path len and path ptr are valid when
            the filename in request is EF_BY_PATH */
    }
    else
    {
        /* Null ptr for buffer */
        gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_ERROR )
    {
        /* Has an error indication been set? */
        warning_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( warning_to_mmi != GSDI_NO_SIM_EVENT )
    {
        if(gsdi_util_send_mmi_sim_ind( warning_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", warning_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",warning_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;

}
/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_SELECT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_select (
    byte                message_id,
    gsdi_slot_id_type   slot,
    uim_items_enum_type sim_filename,
    uim_file_type       type_of_file,
    uim_dir_type        directory,
    const word *        path,
    byte *              length,
    byte *              response_data_p
)
{
    uim_cmd_type *    uim_cmd_ptr    = NULL_PTR;
    gsdi_returns_T    gsdi_status    = GSDI_SUCCESS;
    gsdi_sim_events_T warning_to_mmi = GSDI_NO_SIM_EVENT;

    MSG_MED("Start uim_server_cdma_select",0,0,0);

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        if (type_of_file == UIM_ADF)
        {
            ERR("Invalid file_type %d",0,0,0);
            gsdi_status = GSDI_ERROR;
        }

        if (gsdi_status == GSDI_SUCCESS)
        {
            if (message_id == GSDI_GET_FILE_ATTRIBUTES_REQ)
            {
              uim_cmd_ptr->hdr.command  = UIM_CACHED_SEL_F;
            }
            else
            {
              uim_cmd_ptr->hdr.command  = UIM_SELECT_F;
            }
            uim_cmd_ptr->hdr.protocol = UIM_CDMA;
            uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

            /* populate slot information */
            gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                                  slot);
            if (gsdi_status != GSDI_SUCCESS)
            {
                /* Deallocate the cmd from heap */
                MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
                return gsdi_status;
            }

            uim_cmd_ptr->select.file_type = type_of_file;
            uim_cmd_ptr->select.item      = sim_filename;
            uim_cmd_ptr->select.dir       = directory;

            /* Initialize the path array to 0xFF */
            memset((void *)uim_cmd_ptr->select.path,
                   0xFF,
                   UIM_MAX_PATH_ELEMENTS * sizeof(word));

            /* copy file path array */
            if( NULL != (void *)path )
            {
              memcpy((void *)uim_cmd_ptr->select.path,
                     (void *)path,
                     UIM_MAX_PATH_ELEMENTS * sizeof(word));
            }

            gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

            /* Safety check for proper report  */
            if ((uim_rsp_report.rpt_type == UIM_CACHED_SEL_R) ||
                (uim_rsp_report.rpt_type == UIM_SELECT_R))
            {
                gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                              uim_rsp_report.rpt_status,
                                                              (uint32)uim_rsp_report.sw1,
                                                              (uint32)uim_rsp_report.sw2);

                if (gsdi_status == GSDI_SUCCESS)
                {
                    *length = (uint8)uim_rsp_report.rpt.select.num_data_bytes;
                    (void) memcpy( &response_data_p[0],
                                   &uim_rsp_report.rpt.select.data[0],
                                   uim_rsp_report.rpt.select.num_data_bytes);
                }
                else
                {
                    /* UIM_FAIL */
                    MSG_HIGH("UIM_FAIL For EF 0x%x",sim_filename,0,0);
                    MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                                 uim_rsp_report.sw2,
                                                                 gsdi_status);
                } /* UIM Fail */
            }
            else
            {  /* Invalid report returned */
                gsdi_status = GSDI_ERROR;
            }
        }
    }
    else
    {
        /* Null ptr for buffer */
        gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_ERROR )
    {
        /* Has an error indication been set? */
        warning_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( warning_to_mmi != GSDI_NO_SIM_EVENT )
    {
        if(gsdi_util_send_mmi_sim_ind( warning_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", warning_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",warning_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;

} /* gsdi_uim_server_cdma_select */


/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_WRITE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_write (
    gsdi_sim_write_req_T *  req_p,
    byte *                  rec_num_accessed_p
)
{
    uim_cmd_type *    uim_cmd_ptr    = NULL_PTR;
    gsdi_returns_T    gsdi_status    = GSDI_SUCCESS;
    gsdi_sim_events_T warning_to_mmi = GSDI_NO_SIM_EVENT;

    MSG_LOW("Start uim_server_cdma_write",0,0,0);

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_ACCESS_F;
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              req_p->message_header.slot );

        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Deallocate the cmd from heap */
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }

        uim_cmd_ptr->access_uim.access = UIM_WRITE_F;
        uim_cmd_ptr->access_uim.item   = (uim_items_enum_type)req_p->sim_filename;

        if ( req_p->index_mode == (byte)GSDI_INDEX_PREVIOUS )
        {
            /* Warning...should only be used for cyclical files */
            uim_cmd_ptr->access_uim.rec_mode = UIM_PREVIOUS;
        }
        else
        {
            /* Use default UIM_ABSOLUTE */
            uim_cmd_ptr->access_uim.rec_mode = UIM_ABSOLUTE;
        }

        if (req_p->rec_num != 0)
        {
            uim_cmd_ptr->access_uim.offset = (word)req_p->rec_num;
        }
        else
        {
            uim_cmd_ptr->access_uim.offset = (word)req_p->data_offset;
        }

        uim_cmd_ptr->access_uim.num_bytes = (word)req_p->data_len;
        uim_cmd_ptr->access_uim.data_ptr  = (byte*)(req_p->data_buffer);

        (void) memcpy(uim_cmd_ptr->access_uim.data_ptr,
                      req_p->data_buffer,
                      uim_cmd_ptr->access_uim.num_bytes);

        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

        /* Safety check for proper report  */
        if (uim_rsp_report.rpt_type == UIM_ACCESS_R)
        {
            gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                          uim_rsp_report.rpt_status,
                                                          (uint32)uim_rsp_report.sw1,
                                                          (uint32)uim_rsp_report.sw2 );

            if (gsdi_status == GSDI_SUCCESS)
            {
                *rec_num_accessed_p  = req_p->rec_num;
            }
            else
            {
                /* UIM_FAIL */
                MSG_HIGH("UIM_FAIL For EF 0x%x",(uim_items_enum_type)req_p->sim_filename,0,0);
                MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                             uim_rsp_report.sw2,
                                                             gsdi_status);
            } /* UIM Fail */
        }
        else
        {
            /* Invalid report returned */
            gsdi_status = GSDI_ERROR;
        }
    }
    else
    {
        /* Null ptr for buffer */
        gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_ERROR )
    {
        /* Has an error indication been set? */
        warning_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( warning_to_mmi != GSDI_NO_SIM_EVENT )
    {
        if(gsdi_util_send_mmi_sim_ind( warning_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", warning_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",warning_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;

} /* gsdi_uim_server_cdma_write */



/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_UNBLOCK_CHV

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_unblock_chv (
    gsdi_slot_id_type slot,
    gsdi_pin_ids_T    pin_id,
    byte *            puk,
    byte *            new_pin,
    byte*             data_p
)
{
    uim_cmd_type *            uim_cmd_ptr    = NULL_PTR;
    gsdi_returns_T            gsdi_status    = GSDI_SUCCESS;
    gsdi_sim_events_T         event_to_mmi   = GSDI_NO_SIM_EVENT;
    mmgsdi_int_app_info_type *sel_app_ptr    = mmgsdi_util_get_sel_app();

    MSG_LOW("Start uim_server_cdma_unblock_chv",0,0,0);
    MMGSDI_RETURN_IF_NULL(sel_app_ptr);

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_UNBLOCK_CHV_F;
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Deallocate the cmd from heap */
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }

        switch ( pin_id )
        {
        case GSDI_PIN1:
            uim_cmd_ptr->unblk_chv.chv = UIM_CHV1;
            break;

        case GSDI_PIN2:
            uim_cmd_ptr->unblk_chv.chv = UIM_CHV2;
            break;

        default:
            MSG_ERROR("BAD PIN ID: 0x%x",pin_id,0,0);
            uim_cmd_ptr->unblk_chv.chv = UIM_CHV_NONE;
            break;

        }

        (void) memcpy( uim_cmd_ptr->unblk_chv.chv_digits,
                       puk,
                       GSDI_MAX_PUK_LEN );

        (void) memcpy( uim_cmd_ptr->unblk_chv.new_chv_digits,
                       new_pin,
                       GSDI_MAX_PIN_LEN );

        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

        /* Safety check for proper report  */
        if (uim_rsp_report.rpt_type == UIM_UNBLOCK_CHV_R)
        {
            gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                          uim_rsp_report.rpt_status,
                                                          (uint32)uim_rsp_report.sw1,
                                                          (uint32)uim_rsp_report.sw2 );
            if(data_p != NULL)
            {
              /* Copy the status words into data pointer to pass back to calling app */
              data_p[0] = (byte)uim_rsp_report.sw1;
              data_p[1] = (byte)uim_rsp_report.sw2;
            }
            else
            {
              MSG_HIGH("Unable to return status words from GSDI, Null data ptr",0,0,0);
            }

            if (gsdi_status == GSDI_SUCCESS)
            {
                gsdi_status = gsdi_uim_server_gsm_cdma_get_max_num_retries(
                  slot, pin_id, GSDI_UNBLOCK_PIN_REQ, &event_to_mmi);
                if (gsdi_status != GSDI_SUCCESS)
                {
                  MSG_ERROR("Unable to get max num retries 0x%x", gsdi_status, 0, 0);
                  /* preset the pin num retries to 3 and puk retries to 10 so that the
                     phone will have minimal valid num retries data when the num retries were not able
                     to be updated correctly from the card */
                  gsdi_uim_server_preset_num_retries(slot, pin_id);
                  /* reset the status */
                  gsdi_status = GSDI_SUCCESS;
                }
            }
            else
            { /* UIM_FAIL */
                MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                             uim_rsp_report.sw2,
                                                             gsdi_status);
                switch (gsdi_status)
                {
                case GSDI_INCORRECT_CODE:
                    switch ( slot )
                    {
                      case GSDI_SLOT_1:
                        if ( pin_id == GSDI_PIN1 )
                        {
                          MMGSDI_RETURN_IF_NULL(sel_app_ptr->pin1);
                          if (sel_app_ptr->pin1->num_unblock_retries != 0)
                          {
                            --sel_app_ptr->pin1->num_unblock_retries;
                          }
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                          if (sel_app_ptr->pin2.num_unblock_retries != 0)
                          {
                            --sel_app_ptr->pin2.num_unblock_retries;
                          }
                        }
                        break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
                    case GSDI_SLOT_2:
                        if ( pin_id == GSDI_PIN1 )
                        {
                            if (gsdi_data.pin_status_info_2.puk1_num_retries_allowed != 0)
                                gsdi_data.pin_status_info_2.puk1_num_retries_allowed--;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                            if (gsdi_data.pin_status_info_2.puk2_num_retries_allowed != 0)
                                gsdi_data.pin_status_info_2.puk2_num_retries_allowed--;
                        }
                        break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

                    default:
                      MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
                      break;
                    }
                    break;

                case GSDI_CODE_PERM_BLOCKED:
                    switch ( slot )
                    {
                      case GSDI_SLOT_1:
                        if(mmgsdi_util_convert_to_mmgsdi_pin_status(pin_id,
                                                                    GSDI_PIN_BLOCKED)
                           != GSDI_SUCCESS)
                        {
                          MSG_ERROR("Could not convert to MMGSDI PIN Status",0,0,0);
                        }
                        if ( pin_id == GSDI_PIN1 )
                        {
                          event_to_mmi = GSDI_PIN1_PERM_BLOCKED;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                          event_to_mmi = GSDI_PIN2_PERM_BLOCKED;
                        }
                        break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                    case GSDI_SLOT_2:
                        if ( pin_id == GSDI_PIN1 )
                        {
                            gsdi_data.pin_status_info_2.puk1_num_retries_allowed = 0;
                            event_to_mmi = GSDI_PIN1_PERM_BLOCKED_2;
                            gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_PERM_BLOCKED;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                            gsdi_data.pin_status_info_2.puk2_num_retries_allowed = 0;
                            event_to_mmi = GSDI_PIN2_PERM_BLOCKED_2;
                            gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_PERM_BLOCKED;
                        }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

                    default:
                        MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
                        break;
                    }
                    break;

                default:
                    MSG_HIGH("Other gsdi status %d", gsdi_status, 0, 0);
                    break;
                } /* gsdi _ status */
            }
        } /* rpt_type = UNBLOCK_CHV_R */
        else
        {
            /* Invalid report returned */
            gsdi_status = GSDI_ERROR;
        }
    }
    else
    {
        /* Null ptr for buffer */
        gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_ERROR )
    {
        /* Has an error indication been set? */
        if ( event_to_mmi == GSDI_NO_SIM_EVENT )
            event_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( event_to_mmi != GSDI_NO_SIM_EVENT )
    {
        if(gsdi_util_send_mmi_sim_ind( event_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", event_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",event_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;

} /* gsdi_uim_server_cdma_unblock_chv */


/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_VERIFY_CHV

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_verify_chv (
    gsdi_slot_id_type   slot,
    gsdi_pin_ids_T      pin_id,
    byte *              pin,
    byte *              data_p
)
{
    uim_cmd_type             *uim_cmd_ptr    = NULL_PTR;
    gsdi_returns_T            gsdi_status    = GSDI_SUCCESS;
    gsdi_sim_events_T         event_to_mmi   = GSDI_NO_SIM_EVENT;
    mmgsdi_int_app_info_type *sel_app_ptr    = mmgsdi_util_get_sel_app();

    MSG_LOW("Start uim_server_cdma_verify_pin",0,0,0);
    MMGSDI_RETURN_IF_NULL(sel_app_ptr);

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_VERIFY_CHV_F;
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              slot );
        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Deallocate the cmd from heap */
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }

        switch ( pin_id )
        {
        case GSDI_PIN1:
            uim_cmd_ptr->verify_chv.chv = UIM_CHV1;
            break;

        case GSDI_PIN2:
            uim_cmd_ptr->verify_chv.chv = UIM_CHV2;
            break;

        default:
            MSG_ERROR("BAD PIN ID: 0x%x",pin_id,0,0);
            uim_cmd_ptr->verify_chv.chv = UIM_CHV_NONE;
            break;
        }

        (void) memcpy( &uim_cmd_ptr->verify_chv.chv_digits[0],
                       pin,
                       GSDI_MAX_PIN_LEN );

        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

        /* Safety check for proper report  */
        if (uim_rsp_report.rpt_type == UIM_VERIFY_CHV_R)
        {
            gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                          uim_rsp_report.rpt_status,
                                                          (uint32)uim_rsp_report.sw1,
                                                          (uint32)uim_rsp_report.sw2 );
            if(data_p != NULL)
            {
              /* Copy the status words into data pointer to pass back to calling app */
              data_p[0] = (byte)uim_rsp_report.sw1;
              data_p[1] = (byte)uim_rsp_report.sw2;
            }
            else
            {
              MSG_HIGH("Unable to return status words from GSDI, Null data ptr",0,0,0);
            }

            if (gsdi_status == GSDI_SUCCESS)
            {
                gsdi_status = gsdi_uim_server_gsm_cdma_get_max_num_retries(
                  slot, pin_id, GSDI_VERIFY_PIN_REQ, &event_to_mmi);
                if (gsdi_status != GSDI_SUCCESS)
                {
                  MSG_ERROR("Unable to get max num retries 0x%x", gsdi_status, 0, 0);
                  /* preset the pin num retries to 3 and puk retries to 10 so that the
                     phone will have minimal valid num retries data when the num retries were not able
                     to be updated correctly from the card */
                  gsdi_uim_server_preset_num_retries(slot, pin_id);
                  /* reset the status */
                  gsdi_status = GSDI_SUCCESS;
                }
            } /* rpt_status = UIM_PASS */
            else
            {
                /* UIM_FAIL */
                MSG_HIGH("UIM_FAIL CHV VERIFICATION",0,0,0);
                MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                             uim_rsp_report.sw2,
                                                             gsdi_status);

                switch (gsdi_status)
                {
                case GSDI_INCORRECT_CODE:
                    switch ( slot )
                    {
                      case GSDI_SLOT_1:
                        if ( pin_id == GSDI_PIN1 )
                        {
                          MMGSDI_RETURN_IF_NULL(sel_app_ptr->pin1);
                          if (sel_app_ptr->pin1->num_retries != 0)
                          {
                            --sel_app_ptr->pin1->num_retries;
                          }
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                          if (sel_app_ptr->pin2.num_retries != 0)
                          {
                            --sel_app_ptr->pin2.num_retries;
                          }
                        }
                        break;

  #ifdef FEATURE_MMGSDI_DUAL_SLOT
                    case GSDI_SLOT_2:
                        if ( pin_id == GSDI_PIN1 )
                        {
                            if ( gsdi_data.pin_status_info_2.pin1_num_retries_allowed != 0 )
                                gsdi_data.pin_status_info_2.pin1_num_retries_allowed--;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                            if ( gsdi_data.pin_status_info_2.pin2_num_retries_allowed != 0 )
                                gsdi_data.pin_status_info_2.pin2_num_retries_allowed--;
                        }
                        break;
  #endif /* FEATURE_MMGSDI_DUAL_SLOT */

                    default:
                        MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
                        break;
                    }
                    break;

                case GSDI_CODE_BLOCKED:
                    switch ( slot )
                    {
                      case GSDI_SLOT_1:
                        if(mmgsdi_util_convert_to_mmgsdi_pin_status(pin_id,
                                                                    GSDI_PIN_BLOCKED)
                           != GSDI_SUCCESS)
                        {
                          MSG_ERROR("Could not convert to MMGSDI PIN Status",0,0,0);
                        }
                        if ( pin_id == GSDI_PIN1 )
                        {
                          event_to_mmi = GSDI_PIN1_BLOCKED;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                          event_to_mmi = GSDI_PIN2_BLOCKED;
                        }
                        break;

  #ifdef FEATURE_MMGSDI_DUAL_SLOT
                    case GSDI_SLOT_2:
                        if ( pin_id == GSDI_PIN1 )
                        {
                            gsdi_data.pin_status_info_2.pin1_num_retries_allowed = 0;
                            event_to_mmi = GSDI_PIN1_BLOCKED_2;
                            gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_BLOCKED;
                        }
                        else if ( pin_id == GSDI_PIN2 )
                        {
                            gsdi_data.pin_status_info_2.pin2_num_retries_allowed = 0;
                            event_to_mmi = GSDI_PIN2_BLOCKED_2;
                            gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_BLOCKED;
                        }
                        break;
  #endif /* FEATURE_MMGSDI_DUAL_SLOT */

                    default:
                        MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
                        break;
                    }
                    break;

                default:
                    MSG_MED("Other GSDI Status, %d", gsdi_status, 0, 0);
                    break;
                } /* gsdi_status */
            } /* UIM_FAIL */
        } /*rpt_type = UIM_VERIFY_CHV */
    }
    else
    {
      /* Null ptr for buffer */
      gsdi_status = GSDI_ERROR;
    }
    if ( gsdi_status == GSDI_ERROR )
    {
        /* Has an error indication been set? */
      if ( event_to_mmi == GSDI_NO_SIM_EVENT )
        event_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( event_to_mmi != GSDI_NO_SIM_EVENT )
    {
        if(gsdi_util_send_mmi_sim_ind( event_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", event_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",event_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;
} /* gsdi_uim_server_cdma_verify_chv */


/*===========================================================================
FUNCTION gsdi_uim_server_cdma_rehabilitate

DESCRIPTION
  make uim function call to perform rehabilitation on the EF

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_rehabilitate(
    gsdi_slot_id_type   slot,
    uim_items_enum_type filename )
{
    uim_cmd_type *    uim_cmd_ptr    = NULL_PTR;
    gsdi_returns_T    gsdi_status    = GSDI_SUCCESS;
    gsdi_sim_events_T warning_to_mmi = GSDI_NO_SIM_EVENT;

    MSG_LOW("Start uim_server_cdma_rehabilitate",0,0,0);

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_REHABILITATE_F;
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Deallocate the cmd from heap */
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }

        uim_cmd_ptr->rehab.item = filename;

        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

        /* Safety check for proper report  */
        if (uim_rsp_report.rpt_type == UIM_REHABILITATE_R)
        {
            gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                          uim_rsp_report.rpt_status,
                                                          (uint32)uim_rsp_report.sw1,
                                                          (uint32)uim_rsp_report.sw2);

            if ( ( gsdi_status != GSDI_SUCCESS ) &&
                 ( gsdi_status != GSDI_INVALIDATION_CONTRADICTION_STATUS ) )
            {
                /* UIM_FAIL */
                MSG_HIGH("UIM_FAIL For EF 0x%x",filename,0,0);
                MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                             uim_rsp_report.sw2,
                                                             gsdi_status);
            }
        }
        else
        {  /* Invalid report returned */
            gsdi_status = GSDI_ERROR;
        }
    }
    else
    {  /* Null ptr for buffer */
        gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_ERROR ) {
        /* Has an error indication been set? */
        warning_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( warning_to_mmi != GSDI_NO_SIM_EVENT ) {
        if(gsdi_util_send_mmi_sim_ind( warning_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", warning_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",warning_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;
} /*gsdi_uim_server_cdma_rehabilitate */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_INVALIDATE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_invalidate (
  gsdi_slot_id_type   slot,
  uim_items_enum_type filename
)
{
    uim_cmd_type*     uim_cmd_ptr    = NULL_PTR;
    gsdi_returns_T    gsdi_status    = GSDI_SUCCESS;
    gsdi_sim_events_T warning_to_mmi = GSDI_NO_SIM_EVENT;

    MSG_LOW("Start uim_server_cdma_invalidate",0,0,0);

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_INVALIDATE_F;
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }

        uim_cmd_ptr->invalidate.item   = filename;

        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

        /* Safety check for proper report  */
        if (uim_rsp_report.rpt_type == UIM_INVALIDATE_R)
        {

            gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                          uim_rsp_report.rpt_status,
                                                          (uint32)uim_rsp_report.sw1,
                                                          (uint32)uim_rsp_report.sw2);

            if ( ( gsdi_status != GSDI_SUCCESS) &&
                 ( gsdi_status != GSDI_INVALIDATION_CONTRADICTION_STATUS ) )
            {
                /* UIM_FAIL */
                MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                             uim_rsp_report.sw2,
                                                             gsdi_status);
            }
        }
        else
        {  /* Invalid report returned */
            gsdi_status = GSDI_ERROR;
        }
    }
    else
    {  /* Null ptr for buffer */
        gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_ERROR )
    {
        /* Has an error indication been set? */
        warning_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( warning_to_mmi != GSDI_NO_SIM_EVENT ) {
        if(gsdi_util_send_mmi_sim_ind( warning_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", warning_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",warning_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;


} /* gsdi_uim_server_cdma_invalidate */


/*===========================================================================
FUNCTION GSDI_UIM_SERVER_STORE_ESN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_uim_server_store_esn (
    gsdi_store_esn_req_T *  req_p
)
{
    uim_cmd_type *    uim_cmd_ptr    = NULL_PTR;
    gsdi_returns_T    gsdi_status    = GSDI_SUCCESS;
    gsdi_sim_events_T warning_to_mmi = GSDI_NO_SIM_EVENT;

    MSG_LOW("Start uim_server_gsm_write",0,0,0);

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_STORE_ESN_ME_F;
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              req_p->message_header.slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Deallocate the cmd from heap */
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }

#ifdef FEATURE_UIM_EUIMID
        if (gsdi_is_sf_euimid_activated(req_p->message_header.slot))
        {
          uim_cmd_ptr->hdr.command  = UIM_STORE_ESN_MEID_ME_F;
          uim_cmd_ptr->store_meid.meid_available = FALSE;
        }
        /* if meid was populated then send cmd with meid and not esn */
        if (gsdi_is_meid_svc_activated(req_p->message_header.slot) &&
            (req_p->meid_available))
        {
          uim_cmd_ptr->hdr.command  = UIM_STORE_ESN_MEID_ME_F;
          uim_cmd_ptr->store_meid.len_and_usage =
            req_p->esn_length | req_p->esn_usage;
          qw_equ(uim_cmd_ptr->store_meid.meid, req_p->meid);
          uim_cmd_ptr->store_meid.meid_available = TRUE;
        }
        if (!req_p->meid_available)
#endif /* FEATURE_UIM_EUIMID */
        {
          uim_cmd_ptr->store_esn.len_and_usage =
            req_p->esn_length | req_p->esn_usage;
          uim_cmd_ptr->store_esn.esn = req_p->esn;
        }
        uim_cmd_ptr->hdr.options   = UIM_OPTION_NONE;

        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

        /* Safety check for proper report  */
        if ( (uim_rsp_report.rpt_type == UIM_STORE_ESN_MEID_ME_R) ||
             (uim_rsp_report.rpt_type == UIM_STORE_ESN_ME_R)
           )
        {

            gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                          uim_rsp_report.rpt_status,
                                                          (uint32)uim_rsp_report.sw1,
                                                          (uint32)uim_rsp_report.sw2);

            if (gsdi_status != GSDI_SUCCESS)
            {
                /* UIM_FAIL */
                MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                             uim_rsp_report.sw2,
                                                             gsdi_status);
            } /* UIM FAIL */
#ifdef FEATURE_UIM_RUIM
            else
            {
                /* Store the ESN ME Change Flag, LSB indicates if ESN
                   has changed*/
                if (uim_rsp_report.rpt.store_esn.usage_ind_conf &
                    MMGSDI_JCDMA_ESN_ME_BIT)
                {
                    if ( req_p->message_header.slot == GSDI_SLOT_1 )
                    {
                        mmgsdi_data_slot1.esn_me_change_flag = MMGSDI_ESN_ME_CHANGE;
                    }
                    else
                    {
                        MSG_HIGH("ESN ME Change Flag not set for slot 0x%x",
                                 req_p->message_header.slot, 0, 0);
                    }

                }
                else
                {
                   mmgsdi_data_slot1.esn_me_change_flag = MMGSDI_ESN_ME_NO_CHANGE;
                }
                MSG_HIGH("ESN ME Change flag set to : %d",
                          mmgsdi_data_slot1.esn_me_change_flag,0,0);
            }
#endif /* FEATURE_UIM_RUIM */
        }
        else
        {
            /* Invalid report returned */
            gsdi_status = GSDI_ERROR;
        }
    }
    else
    {
        /* Null ptr for buffer */
        gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_ERROR )
    {
        /* Has an error indication been set? */
        warning_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( warning_to_mmi != GSDI_NO_SIM_EVENT )
    {
        if(gsdi_util_send_mmi_sim_ind( warning_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", warning_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",warning_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;
} /* gsdi_uim_server_store_esn */

#if defined (FEATURE_UIM_TOOLKIT_UTK) && !defined (FEATURE_GSTK)
/*===========================================================================
FUNCTION GSDI_UIM_SERVER_TERMINAL_PROFILE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_uim_server_terminal_profile (
    gsdi_slot_id_type slot)
{
    uim_cmd_type *    uim_cmd_ptr    = NULL_PTR;
    gsdi_returns_T    gsdi_status    = GSDI_SUCCESS;
    gsdi_sim_events_T warning_to_mmi = GSDI_NO_SIM_EVENT;

    MSG_LOW("Start uim_server_gsm_write",0,0,0);

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_TERMINAL_PROFILE_F;
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Deallocate the cmd from heap */
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }
        /* clear out the Terminal Profile Buffer */
        memset(uim_cmd_ptr->terminal_profile.data,
               0,
               UIM_TK_TERMINAL_PROFILE_DATA_LENGTH);

        /* Set UIM Terminal Profile Command members */
        uim_cmd_ptr->terminal_profile.num_bytes =
          UIM_TK_TERMINAL_PROFILE_DATA_LENGTH;
        uim_cmd_ptr->terminal_profile.data[0]  |=
          (UIM_TK_B1_PROFILE_DOWNLOAD | UIM_TK_B1_CDMA_SMS_PP_DOWNLOAD);
        uim_cmd_ptr->terminal_profile.data[2]  |=
          UIM_TK_B3_DISPLAY_TEXT;

#ifdef FEATURE_UIM_DEBUG_TOOLKIT_SEND_SMS
#error code not present
#endif /* FEATURE_UIM_DEBUG_TOOLKIT_SEND_SMS */
        uim_cmd_ptr->terminal_profile.data[19] |=
              ( UIM_TK_B20_CDMA_SEND_SMS | UIM_TK_B20_CDMA_SMS_PP_DOWNLOAD );

        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

        /* Safety check for proper report  */
        if (uim_rsp_report.rpt_type == UIM_TERMINAL_PROFILE_R)
        {
            gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                          uim_rsp_report.rpt_status,
                                                          (uint32)uim_rsp_report.sw1,
                                                          (uint32)uim_rsp_report.sw2);

            if (gsdi_status != GSDI_SUCCESS)
            {
                /* UIM_FAIL */
                MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                            uim_rsp_report.sw2,
                                                            gsdi_status);
            } /* UIM FAIL */

        }
        else
        {
            /* Invalid report returned */
            gsdi_status = GSDI_ERROR;
        }
    }
    else
    {
        /* Null ptr for buffer */
        gsdi_status = GSDI_ERROR;
    }

    /* At this point...and this may be temporary */
    /* if the terminal profile command fails in  */
    /* any way...it is ok.  We can reset the     */
    /* the status to allow init to complete for  */
    /* usage of the the mobile.  Reset           */
    /* gsdi_status                               */
    gsdi_status = GSDI_SUCCESS;

    if ( gsdi_status == GSDI_ERROR )
    {
        /* Has an error indication been set? */
        warning_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( warning_to_mmi != GSDI_NO_SIM_EVENT )
    {
        if(gsdi_util_send_mmi_sim_ind( warning_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", warning_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",warning_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;

} /* gsdi_uim_server_gsm_write */
#endif /* FEATURE_UIM_TOOLKIT */
#endif /* FEATURE_MMGSDI_CDMA */

/*===========================================================================
FUNCTION GSDI_GET_UIM_PROTOCOL

DESCRIPTION
   This function is used by the UIM Server.  It will notify what protocol is
   currently used by the GSDI and is dependent on the SIM/USIM Type.

DEPENDENCIES


RETURN VALUE
  uim_protocol_type
===========================================================================*/
uim_protocol_type gsdi_get_uim_protocol()
{
    gsdi_card_mode_capability_T  capability_of_card;
    capability_of_card = gsdi_data.current_card_mode_capability;


    if ( capability_of_card == GSDI_CARD_USIM)
    {
        return UIM_WCDMA;
    }
    else if (capability_of_card == GSDI_CARD_GSM)
    {
        return UIM_GSM;
    }
    else
    {
        return UIM_NO_SUCH_MODE;
    }
} /* gsdi_get_uim_protocol */


#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */


/*===========================================================================
FUNCTION gsdi_uim_server_sim_reset

DESCRIPTION
  This function will request that the UIM Perform a Warm Reset on the
  SIM/USIM/RUIM based on ISO/IEC 7816-3 Specifications

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  1.  Resets the SIM/USIM/RUIM.  All current state information in the
      SIM/USIM/RUIM is reset (PIN Verifications, etc).
  2.  A GSDI_LINK_ESTABLISHED_MSG message is generated which kicks off the
      initialization procedures.
===========================================================================*/
gsdi_returns_T gsdi_uim_server_sim_reset(
  gsdi_slot_id_type slot,
  uim_protocol_type protocol)
{
    uim_cmd_type     *uim_cmd_ptr;
    gsdi_returns_T    gsdi_status = GSDI_SUCCESS;

    MSG_LOW("Start uim_server_sim_reset",0,0,0);

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command  = UIM_RESET_F;
        uim_cmd_ptr->hdr.protocol = protocol;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Deallocate the cmd from heap */
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }
        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);
    }

    if (uim_rsp_report.rpt_type == UIM_RESET_R)
        gsdi_status = GSDI_SUCCESS;
    else
        gsdi_status = GSDI_ERROR;

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;
}

#ifdef FEATURE_MMGSDI_TEST_AUTOMATION
/*===========================================================================
FUNCTION GSDI_UIM_SERVER_SEND_APDU

DESCRIPTION
  Function is called by gsdi_proc_send_apdu().  This function will serve
  as the GSDI - UIM Interface Function to process the SEND_APDU Request.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_send_apdu(
  gsdi_slot_id_type slot,
  uint32            apdu_len,
  uint8           * apdu_data,
  uint8           * returned_data_len_p,
  uint8           * data_p
)
{
    uim_cmd_type     * uim_cmd_ptr;
    gsdi_returns_T     gsdi_status = GSDI_SUCCESS;

    MSG_LOW("Start uim_server_send_apdu",0,0,0);

    MMGSDI_RETURN_IF_NULL(data_p);
    /*lint -esym(613,returned_data_len_p)*/
    /* ---------------------------------------------------------------------
       Lets get a buffer for the request
       --------------------------------------------------------------------- */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        /* Populate Header Content
        */
        uim_cmd_ptr->hdr.command  = UIM_STREAM_APDU_F;
        uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;

        /* Populate Header Slot information
        */
        gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                              slot);
        if ( gsdi_status != GSDI_SUCCESS )
        {
            MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
        }

        /* Start parsing the data Format is as follows:
        **    BYTE 0         BYTE 1       BYTE2  BYTE3  BYTE4    DATA
        ** --------------------------------------------------------------
        ** [ INST CLASS ] [ INSTRUCTION ] [ P1 ] [ P2 ] [ P3 ] [ DATA 1 ]
        */
        MMGSDI_RETURN_IF_NULL(apdu_data);

        uim_cmd_ptr->stream_apdu.uim_class  = apdu_data[GSDI_INST_CLASS_OFFSET];
        uim_cmd_ptr->stream_apdu.instrn     = apdu_data[GSDI_INST_TYPE_OFFSET];
        uim_cmd_ptr->stream_apdu.p1         = apdu_data[GSDI_P1_PARAM_OFFSET];
        uim_cmd_ptr->stream_apdu.p2         = apdu_data[GSDI_P2_PARAM_OFFSET];
        uim_cmd_ptr->stream_apdu.p3         = apdu_data[GSDI_P3_PARAM_OFFSET];
        memcpy(uim_cmd_ptr->stream_apdu.data,
               &apdu_data[GSDI_APDU_DATA_OFFSET],
               uim_cmd_ptr->stream_apdu.p3);

        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

        if (uim_rsp_report.rpt_type == UIM_STREAM_APDU_R)
        {
            gsdi_status = gsdi_uim_server_parse_response( uim_rsp_report.rpt_type,
                                                          uim_rsp_report.rpt_status,
                                                          (uint32)uim_rsp_report.sw1,
                                                          (uint32)uim_rsp_report.sw2);
        }
        /* Got a response report from UIM
        */
        *returned_data_len_p = 2;
        data_p[GSDI_SW1_OFFSET] = (byte)uim_rsp_report.sw1;
        data_p[GSDI_SW1_OFFSET] = (byte)uim_rsp_report.sw2;
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;
    /*lint +esym(613,returned_data_len_p)*/
} /*gsdi_uim_server_send_apdu*/ /*lint !e715 */

#endif /* FEATURE_MMGSDI_TEST_AUTOMATION */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION GSDI_UIM_SERVER_PREF_SLOT_CHANGE

DESCRIPTION
  This function will serve as the interface to the UIM.  It will be used
  to notify the UIM Task of a Preferred Slot Change.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_pref_slot_change(
  gsdi_slot_id_type  slot,
  gsdi_app_enum_type app
)
{
    gsdi_returns_T     gsdi_status    = GSDI_SUCCESS;
    uim_cmd_type     * uim_cmd_ptr    = NULL_PTR;
    uim_slot_type      uim_slot       = UIM_SLOT_NONE;
    uim_protocol_type  uim_protocol   = UIM_NO_SUCH_MODE;
    uim_app_type       uim_app        = UIM_GSM_APP;

    /* ---------------------------------------------------------------------
       Lets get a buffer for the request
       --------------------------------------------------------------------- */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        /* Set the UIM Slot */
        switch (slot)
        {
        case GSDI_SLOT_NONE:
            uim_slot = UIM_SLOT_NONE;
            break;

        case GSDI_SLOT_1:
            uim_slot = UIM_SLOT_1;
            break;

        case GSDI_SLOT_2:
            uim_slot = UIM_SLOT_2;
            break;

        default:
            MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return GSDI_ERROR;
        }

        /* Set the UIM Protocol and UIM App*/
        switch (app)
        {
        case GSDI_GSM_APP:
            uim_protocol = UIM_GSM;
            uim_app      = UIM_GSM_APP;
            break;

        case GSDI_CDMA_APP:
            uim_protocol = UIM_CDMA;
            uim_app      = UIM_CDMA_APP;
            break;

        default:
           MSG_ERROR("BAD CARD APP: 0x%x", app,0,0);
           MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
           return GSDI_ERROR;
        }

        /* Populate Header Content
        */
        uim_cmd_ptr->hdr.command  = UIM_PREF_SLOT_CHANGE_F;
        uim_cmd_ptr->hdr.slot     = uim_slot;
        uim_cmd_ptr->hdr.protocol = uim_protocol;

        /* Populate the Message Request Contents
        */
        uim_cmd_ptr->slot_chg.app  = uim_app;
        uim_cmd_ptr->slot_chg.slot = uim_slot;

        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

        /* Check the response information */
        if ( uim_rsp_report.rpt_type == UIM_PREF_SLOT_CHANGE_R )
        {
            if ( uim_rsp_report.rpt_status == UIM_PASS )
            {
                gsdi_status = GSDI_SUCCESS;
            }
            else
            {
                gsdi_status = GSDI_ERROR;
            }
        }
    }
    else
    {
        /* Could not get UIM Buffer */
        return GSDI_ERROR;
    }

    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;
}
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT

/*===========================================================================
FUNCTION GSDI_UIM_SERVER_OPEN_CHANNEL

DESCRIPTION
  This function is called from the Channel Management Open Channel
  function to build a UIM Manage Channel Request to open a new
  communication channel with the Card presently inserted.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_open_channel(
  gsdi_open_channel_req_T * req_p,
  int32 *                   returned_data_len_p,
  uint8 *                   data_p
)
{
    gsdi_returns_T     gsdi_status   = GSDI_SUCCESS;
    uim_cmd_type     * uim_cmd_ptr   = NULL_PTR;
    sw1_sw2_T          sw1_sw2 = {0};

    MMGSDI_RETURN_IF_NULL(req_p);
    MMGSDI_RETURN_IF_NULL(data_p);
    /*lint -esym(613,returned_data_len_p)*/
    /* ------------------------------------------------------------------------
       1.  Get UIM Command Space
       2.  Set UIM Command Header Slot information
       ----------------------------------------------------------------------*/
    uim_cmd_ptr = gsdi_get_uim_buf();
    if ( uim_cmd_ptr == NULL )
    {
      ERR_FATAL("Could not allocate UIM CMD Buffer Space",0,0,0);
    }

    gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                          req_p->message_header.slot);
    if ( gsdi_status != GSDI_SUCCESS )
    {
      MSG_ERROR("Could not set UIM CMD HDR SLOT - SLOT: 0x%x",
        req_p->message_header.slot,0,0);
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return GSDI_ERROR;
    }
    uim_cmd_ptr->hdr.command               = UIM_MANAGE_CHANNEL_F;
    uim_cmd_ptr->hdr.protocol              = gsdi_get_uim_protocol();

    /* ------------------------------------------------------------------------
       Set UIM Manage Channel Command Options
       ----------------------------------------------------------------------*/
    uim_cmd_ptr->manage_channel.mc_action        = UIM_MC_OPEN;
    uim_cmd_ptr->manage_channel.channel          = (byte)req_p->channel_id;
    uim_cmd_ptr->manage_channel.op_close_channel = (byte)req_p->requested_channel_id;
    uim_cmd_ptr->hdr.channel                     = (uim_channel_type)req_p->channel_id;

    /* ------------------------------------------------------------------------
       Send to UIM or Virtual SIM
       ----------------------------------------------------------------------*/
    gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

    /* ------------------------------------------------------------------------
       Verify the Report Received
       ----------------------------------------------------------------------*/
    if (uim_rsp_report.rpt_type == UIM_MANAGE_CHANNEL_R)
    {
        sw1_sw2.sw1 = (byte)uim_rsp_report.sw1;
        sw1_sw2.sw2 = (byte)uim_rsp_report.sw2;

        /* Was the request successful */
        if (uim_rsp_report.rpt_status == UIM_PASS)
        {
            if ( sw1_sw2.sw1 == 0x90 )
            {
                gsdi_status = GSDI_SUCCESS;
                /* Check the Channel ID requested.
                ** If the channel requested is not the same as
                ** the default channel, then a channel id is
                ** not returned.  Otherwise, use the channel
                ** id given in the report
                */
                if ( req_p->requested_channel_id == 0x00 )
                {
                    MSG_HIGH("OPEN CHNL SUCCESS WITH ASSIGNED CHNL ID: 0x%x",
                        uim_rsp_report.rpt.channel.channel_num,0,0);
                    *returned_data_len_p = 0x03;
                    data_p[0]           = uim_rsp_report.rpt.channel.channel_num;
                    data_p[1]           = sw1_sw2.sw1;
                    data_p[2]           = sw1_sw2.sw2;
                }
                else
                {
                    MSG_HIGH("OPEN CHNL SUCCESS WITH REQUESTED CHNL ID: 0x%x",
                        (byte)req_p->channel_id,0,0);
                    *returned_data_len_p = 0x02;
                    data_p[0]            = sw1_sw2.sw1;
                    data_p[1]            = sw1_sw2.sw2;

                    /* Data Len is not incremented to reflect
                    ** appending the channel ID but is still used
                    ** in gsdi_util_send_gsdi_cnf ()
                    */
                    data_p[2]            = int32touint8(req_p->requested_channel_id);
                }
            }
        }
        else if ( uim_rsp_report.rpt_status     == UIM_FAIL &&
                  uim_rsp_report.cmd_transacted == TRUE )
        {
            /* Commad was processed and a Transaction with
            ** the Card Occurred.  The fact that the card did
            ** not like the TPDU most likely indicates a problem with
            ** the applications APDU.  For GSDI, this is a SUCCESS,
            ** and the corresponding Status Words will be returned
            */
            MSG_HIGH("OPEN CHNL PROCESS PASS WITH CARD ERROR",0,0,0);
            gsdi_status           = GSDI_SUCCESS;
            *returned_data_len_p  = 0x02;
            data_p[0]             = sw1_sw2.sw1;
            data_p[1]             = sw1_sw2.sw2;
            data_p[2]             = 0x00;
        }
        else
        {
            /* The UIM Could not process the command due to
            ** a Software Error */
            MSG_ERROR("OPEN CHNL UIM DRIVER SOFTWARE ERROR",0,0,0);
            gsdi_status           = GSDI_ERROR;
            *returned_data_len_p  = 0x01;
            data_p[0]             = int32touint8(req_p->requested_channel_id);
        }
    }
    else
    {
        /* Invalid report returned */
        gsdi_status = GSDI_ERROR;
    }

    MSG_HIGH("OPEN CHANNEL STATUS: 0x%x SW1 0x%x SW2 0x%x ",
      gsdi_status,sw1_sw2.sw1,sw1_sw2.sw2);

    /* ------------------------------------------------------------------------
       Free Up the memory allocated for this request
       ----------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);

    return gsdi_status;
    /*lint +esym(613,returned_data_len_p)*/
} /* gsdi_uim_server_open_channel */


/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CLOSE_CHANNEL

DESCRIPTION
  This function is called from the Channel Management Close Channel
  function to build a UIM Manage Channel Request to close the
  communication channel with the Card.  The Channel to be closed will be the
  channel provided in the request.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_close_channel(
  gsdi_close_channel_req_T * req_p,
  int32 *                   returned_data_len_p,
  byte *                    data_p
)
{
    gsdi_returns_T     gsdi_status   = GSDI_SUCCESS;
    uim_cmd_type     * uim_cmd_ptr   = NULL_PTR;
    sw1_sw2_T          sw1_sw2 = {0};

    MMGSDI_RETURN_IF_NULL(req_p);
    MMGSDI_RETURN_IF_NULL(data_p);
    /*lint -esym(613,returned_data_len_p)*/
    /* ------------------------------------------------------------------------
       1.  Get UIM Command Space
       2.  Set UIM Command Header Slot information
       ----------------------------------------------------------------------*/
    uim_cmd_ptr = gsdi_get_uim_buf();
    if ( uim_cmd_ptr == NULL )
    {
      ERR_FATAL("Could not allocate UIM CMD Buffer Space",0,0,0);
    }

    gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                          req_p->message_header.slot);
    if ( gsdi_status != GSDI_SUCCESS )
    {
      MSG_ERROR("Could not set UIM CMD HDR SLOT - SLOT: 0x%x",
        req_p->message_header.slot,0,0);
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return GSDI_ERROR;
    }
    uim_cmd_ptr->hdr.command               = UIM_MANAGE_CHANNEL_F;
    uim_cmd_ptr->hdr.protocol              = gsdi_get_uim_protocol();

    /* ------------------------------------------------------------------------
       Set UIM Manage Channel Command Options
       ----------------------------------------------------------------------*/
    uim_cmd_ptr->manage_channel.mc_action        = UIM_MC_CLOSE;
    uim_cmd_ptr->manage_channel.op_close_channel = (byte)req_p->requested_channel_id;
    uim_cmd_ptr->manage_channel.channel          = (byte)req_p->channel_id;
    uim_cmd_ptr->hdr.channel                     = (uim_channel_type)req_p->channel_id;

    /* ------------------------------------------------------------------------
       Send to UIM or Virtual SIM
       ----------------------------------------------------------------------*/
    gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

    /* ------------------------------------------------------------------------
       Verify the Report Received
       ----------------------------------------------------------------------*/
    if (uim_rsp_report.rpt_type == UIM_MANAGE_CHANNEL_R)
    {
        sw1_sw2.sw1 = (byte)uim_rsp_report.sw1;
        sw1_sw2.sw2 = (byte)uim_rsp_report.sw2;

        /* Was the request successful */
        if (uim_rsp_report.rpt_status == UIM_PASS)
        {
            if ( sw1_sw2.sw1 == 0x90 )
            {
                MSG_HIGH("CLOSE CHANNEL SUCCESS",0,0,0);
                gsdi_status = GSDI_SUCCESS;
                *returned_data_len_p  = 0x03;
                data_p[0]             = sw1_sw2.sw1;
                data_p[1]             = sw1_sw2.sw2;
                data_p[2]             = int32touint8(req_p->requested_channel_id);
            }
        }
        else if ( uim_rsp_report.rpt_status     == UIM_FAIL &&
                  uim_rsp_report.cmd_transacted == TRUE )
        {
            /* Commad was processed and a Transaction with
            ** the Card Occurred.  The fact that the card did
            ** not like the TPDU most likely indicates a problem with
            ** the Channel ID provided.  For GSDI, this is a SUCCESS,
            ** and the corresponding Status Words will be returned
            */
            MSG_HIGH("CLOSE CHNL PROCESS SUCCESS WITH CARD ERROR",0,0,0);
            gsdi_status           = GSDI_SUCCESS;
            *returned_data_len_p  = 0x03;
            data_p[0]             = sw1_sw2.sw1;
            data_p[1]             = sw1_sw2.sw2;
            data_p[2]             = int32touint8(req_p->requested_channel_id);
        }
        else
        {
            /* The UIM Could not process the command due to
            ** a Software Error */
            MSG_ERROR("CLOSE CHNL UIM DRIVER SOFTWARE ERROR",0,0,0);
            gsdi_status           = GSDI_ERROR;
            *returned_data_len_p  = 0x01;
            data_p[0]             = int32touint8(req_p->requested_channel_id);
        }
    }

    MSG_HIGH("CLOSE CHANNEL STATUS: 0x%x CHNL 0x%x SW1 0x%x SW2 0x%x ",
      gsdi_status,sw1_sw2.sw1,sw1_sw2.sw2);

    /* ------------------------------------------------------------------------
       Free Up the memory allocated for this request
       ----------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);

    return gsdi_status;
    /*lint +esym(613,returned_data_len_p)*/
} /* gsdi_uim_server_close_channel */
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

#if defined (FEATURE_MMGSDI_CHANNEL_MANAGEMENT) || \
    defined (FEATURE_MMGSDI_ATCSIM)
/*===========================================================================
FUNCTION GSDI_UIM_SERVER_SEND_CHANNEL_DATA

DESCRIPTION
  This function is called from the Channel Management Send Channel
  Data function to build a UIM Request to send an APDU on the
  communication channel with the Card.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_send_channel_data(
  gsdi_send_channel_data_req_T * req_p,
  int32 *                        returned_data_len_p,
  byte *                         data_p
)
{
    gsdi_returns_T     gsdi_status   = GSDI_SUCCESS;
    uim_cmd_type     * uim_cmd_ptr   = NULL_PTR;
    sw1_sw2_T          sw1_sw2 = {0};

    MMGSDI_RETURN_IF_NULL(req_p);
    MMGSDI_RETURN_IF_NULL(data_p);
    /*lint -esym(613,returned_data_len_p)*/
   /* ------------------------------------------------------------------------
       1.  Get UIM Command Space
       2.  Set UIM Command Header Slot information
       ----------------------------------------------------------------------*/
    uim_cmd_ptr = gsdi_get_uim_buf();
    MMGSDI_RETURN_IF_NULL(uim_cmd_ptr);

    /* -----------------------------------------------------------------------
       Clear out the contents of the global response buffer to
       be used for storing the UIM Data in the callback.
       ---------------------------------------------------------------------*/
    memset(&uim_rsp_report,0x00,sizeof(uim_rpt_type));

    gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                          req_p->message_header.slot);
    if ( gsdi_status != GSDI_SUCCESS )
    {
      MSG_ERROR("Could not set UIM CMD HDR SLOT - SLOT: 0x%x",
        req_p->message_header.slot,0,0);
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return GSDI_ERROR;
    }

    uim_cmd_ptr->hdr.command               = UIM_STREAM_ISO7816_APDU_F;
    uim_cmd_ptr->hdr.protocol              = gsdi_get_uim_protocol();

    /* ------------------------------------------------------------------------
       Set UIM Stream APDU IS07816 Channel Command Options
       ----------------------------------------------------------------------*/
    uim_cmd_ptr->stream_iso7816_apdu.num_cmd_bytes = int32touint16(req_p->apdu_len);

    if(req_p->apdu_len < GSDI_APDU_DATA_MIN_LEN)
    {
        MSG_ERROR("APDU CLASS INSTR P1 P2: Insufficient ",0,0,0);
        MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
        return GSDI_ERROR;
    }

    if ( req_p->apdu_len > GSDI_MAX_CHANNEL_DATA_CMD_LEN )
    {
      req_p->apdu_len = GSDI_MAX_CHANNEL_DATA_CMD_LEN;
    }
    memcpy(uim_cmd_ptr->stream_iso7816_apdu.cmd_data,
           req_p->data_buffer,
           int32touint32(req_p->apdu_len));

    /* ------------------------------------------------------------------------
       Lastly, update the Instruction Class to contain the
       channel id.
       -----------------------------------------------------------------------*/
    uim_cmd_ptr->stream_iso7816_apdu.cmd_data[0] |= (byte)req_p->channel_id;
    uim_cmd_ptr->hdr.channel                      = (uim_channel_type)req_p->channel_id;

    /* ------------------------------------------------------------------------
       Send to UIM or Virtual SIM
       ----------------------------------------------------------------------*/
    gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

    /* ------------------------------------------------------------------------
       Verify the Report Received
       ----------------------------------------------------------------------*/
    if (uim_rsp_report.rpt_type == UIM_STREAM_ISO7816_APDU_R)
    {
        sw1_sw2.sw1 = (byte)uim_rsp_report.sw1;
        sw1_sw2.sw2 = (byte)uim_rsp_report.sw2;

        /* Was the request successful */
        if (uim_rsp_report.cmd_transacted == TRUE)
        {
            MSG_HIGH("SEND CHANNEL CMD COMPLETED",0,0,0);
            gsdi_status = GSDI_SUCCESS;
            /* with the addition of FEATURE_MMGSDI_ATCSIM Support, there is now a need to
            ** set two additional members of the confirmation.  Because of the nature of this
            ** function, the data is appended appropriately to the data_p to be stripped out
            ** before sending it back in the gsdi_cnf
            */
            *returned_data_len_p  = uim_rsp_report.rpt.stream_iso7816_apdu.num_data_bytes;
            if ( *returned_data_len_p > (GSDI_MAX_CHANNEL_DATA_RSP_LEN-5) )
            {
              *returned_data_len_p = GSDI_MAX_CHANNEL_DATA_RSP_LEN-5;
            }
            memcpy(data_p,
                   uim_rsp_report.rpt.stream_iso7816_apdu.data,
                   int32touint32(*returned_data_len_p));

            /* Now append to the data_p accordingly */
            data_p[*returned_data_len_p]       = sw1_sw2.sw1;
            data_p[*returned_data_len_p + 1]   = sw1_sw2.sw2;
            data_p[*returned_data_len_p + 2]   = int32touint8(req_p->channel_id);

            /* Do not allow an implicit status word go to the client for any of the
            ** of the Read/Update Operations.  Will be removed when UIM
            ** Fix for ME/EF issue is taken cared of.
            */
            if (uim_cmd_ptr->stream_iso7816_apdu.cmd_data[1] == 0xB0 ||
                uim_cmd_ptr->stream_iso7816_apdu.cmd_data[1] == 0xB2 ||
                uim_cmd_ptr->stream_iso7816_apdu.cmd_data[1] == 0xD6 ||
                uim_cmd_ptr->stream_iso7816_apdu.cmd_data[1] == 0xDC  )
            {
              data_p[*returned_data_len_p + 3]   = (byte)0x00;
              data_p[*returned_data_len_p + 4]   = (byte)0x00;
            }
            else
            {
              data_p[*returned_data_len_p + 3]   = (byte)uim_rsp_report.rpt.stream_iso7816_apdu.get_resp_sw1;
              data_p[*returned_data_len_p + 4]   = (byte)uim_rsp_report.rpt.stream_iso7816_apdu.get_resp_sw2;
            }
            *returned_data_len_p += 5;
        }
        else
        {
            /* The UIM Could not process the command due to
            ** a Software Error */
            MSG_ERROR("SEND CHANNEL REJECTED BY UIM",0,0,0);
            gsdi_status           = GSDI_ERROR;
            *returned_data_len_p  = 0x01;
            data_p[0]             = int32touint8(req_p->channel_id);
        }
    }

    MSG_HIGH("SEND CHANNEL STATUS: CHNL 0x%x SW1 0x%x SW2 0x%x ",
      req_p->channel_id,sw1_sw2.sw1,sw1_sw2.sw2);

    /* ------------------------------------------------------------------------
       Free Up the memory allocated for this request
       ----------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);

    return gsdi_status;
    /*lint +esym(613,returned_data_len_p)*/
} /* gsdi_uim_server_send_channel_data */
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT || FEATURE_MMGSDI_ATCSIM */

/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_SEEK

DESCRIPTION
Function that populates the uim cmd pointer and sends it to
  the uim queue to be processed. The funtion also parses the
  response received from the uim server.

INPUT
  gsdi_slot_id_type   slot,
  uim_items_enum_type file_to_search,
  gsdi_index_modes_T  index_mode,
  byte *              search_string,
  byte                search_string_len,
  byte *              rec_num_len_p,
  byte *              rec_num_accessed_p

OUTPUT
  Populates the uim cmd ptr and outs it in the queue.

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_seek (
    gsdi_slot_id_type   slot,
    uim_items_enum_type file_to_search,
    gsdi_index_modes_T  index_mode,
    byte *              search_string,
    byte                search_string_len,
    byte *              rec_num_len_p,
    byte *              rec_num_accessed_p
)
{
    uim_cmd_type *    uim_cmd_ptr    = NULL_PTR;
    gsdi_returns_T    gsdi_status    = GSDI_SUCCESS;
    gsdi_sim_events_T warning_to_mmi = GSDI_NO_SIM_EVENT;

    MSG_MED("Start uim_server_gsm_seek",0,0,0);

    MMGSDI_RETURN_IF_NULL(search_string);
    /*lint -esym(613,rec_num_len_p)*/
    /*lint -esym(613,rec_num_accessed_p)*/

    /* Lets get a buffer for the request */
    if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
    {
        uim_cmd_ptr->hdr.command   = UIM_SEEK_F;
        uim_cmd_ptr->hdr.protocol  = UIM_CDMA;
        uim_cmd_ptr->hdr.channel   = UIM_CHANNEL0;

        /* populate slot information */
        gsdi_status = gsdi_populate_uim_slot(&uim_cmd_ptr->hdr.slot, slot);
        if (gsdi_status != GSDI_SUCCESS) {
            MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
            return gsdi_status;
        }

        uim_cmd_ptr->seek.item = file_to_search;

        if ( gsdi_data.sim_phase > 1 )
        {
            uim_cmd_ptr->seek.seek_type = UIM_SEEK_TYPE_2;
        }
        else
        {
            uim_cmd_ptr->seek.seek_type = UIM_SEEK_TYPE_1;
        }

        switch ( index_mode )
        {
            case GSDI_SEARCH_FWD_FROM_START:
                uim_cmd_ptr->seek.mode = UIM_SEEK_BEGINNING_FORWARDS;
                break;

            case GSDI_SEARCH_BACK_FROM_END:
                uim_cmd_ptr->seek.mode = UIM_SEEK_END_BACKWARDS;
                break;

            case GSDI_SEARCH_FWD_FROM_NEXT:
                uim_cmd_ptr->seek.mode = UIM_SEEK_NEXT_LOCN_FORWARDS;
                break;

            case GSDI_SEARCH_BACK_FROM_PREV:
                uim_cmd_ptr->seek.mode = UIM_SEEK_PREV_LOCN_BACKWARDS;
                break;

            default:
                uim_cmd_ptr->seek.mode = UIM_SEEK_BEGINNING_FORWARDS;
                break;
        }

        uim_cmd_ptr->seek.num_bytes = search_string_len;
        (void) memcpy(&uim_cmd_ptr->seek.srch_pattern[0],
                      search_string,
                      search_string_len);

        /* nil-out remaining fields */
        uim_cmd_ptr->seek.srch_mode          = UIM_NO_SUCH_SRCH_MODE;
        uim_cmd_ptr->seek.rec_number         = 0;
        uim_cmd_ptr->seek.srch_dir           = UIM_SRCH_FORWARD;
        uim_cmd_ptr->seek.offset_type        = UIM_SRCH_ABSOLUTE;
        uim_cmd_ptr->seek.offset             = 0;
        uim_cmd_ptr->seek.srch_rec_mode_type = UIM_SRCH_P1_FORWARD;

        gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

        /* Safety check for proper report  */
        if (uim_rsp_report.rpt_type == UIM_SEEK_R)
        {
            gsdi_status = gsdi_uim_server_parse_response(uim_rsp_report.rpt_type,
                                                         uim_rsp_report.rpt_status,
                                                         (uint32)uim_rsp_report.sw1,
                                                         (uint32)uim_rsp_report.sw2);

            if (gsdi_status == GSDI_SUCCESS)
            {
                *rec_num_len_p = (uint8)uim_rsp_report.rpt.seek.num_records;
                /*Copy only the first record*/
                if(uim_rsp_report.rpt.seek.num_records > 0)
                {
                  *rec_num_accessed_p = uim_rsp_report.rpt.seek.data[0];
                }
            }
            else
            {
                /* UIM_FAIL */
                MSG_HIGH("UIM_FAIL For EF 0x%x",file_to_search,0,0);
                MSG_HIGH("UIM_FAIL sw1 0x%x sw2 0x%x status %d", uim_rsp_report.sw1,
                                                            uim_rsp_report.sw2,
                                                            gsdi_status);
            }
        }
        else
        {
            /* Invalid report returned */
            gsdi_status = GSDI_ERROR;
        }
    }
    else
    {
        /* Null ptr for buffer */
        gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_ERROR )
    {
        /* Has an error indication been set? */
        warning_to_mmi = GSDI_CARD_ERROR;
    }

    /* Let MMI know if there were any error */
    if ( warning_to_mmi != GSDI_NO_SIM_EVENT )
    {
        if(gsdi_util_send_mmi_sim_ind( warning_to_mmi ) != GSDI_SUCCESS)
            MSG_HIGH("Not able to send warning %d", warning_to_mmi, 0, 0);
        else
            MSG_HIGH("Warning indication %d",warning_to_mmi,0,0);
    }

    /* Deallocate the cmd from heap */
    MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

    return gsdi_status;

    /*lint +esym(613,rec_num_len_p)*/
    /*lint +esym(613,rec_num_accessed_p)*/

} /* gsdi_uim_server_cdma_seek */


/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CLEAR_UIM_CACHE

DESCRIPTION
  Function that will clear the UIM Cache Selectively or All

INPUT

OUTPUT
  Populates the uim cmd ptr and outs it in the queue.

RETURN VALUE
  void
===========================================================================*/
void gsdi_uim_server_clear_uim_cache(  gsdi_slot_id_type slot,
                                       int32 num_file,
                                       uim_items_enum_type* file_list)
{
  uim_cmd_type *    uim_cmd_ptr    = NULL;
  gsdi_returns_T    gsdi_status    = GSDI_ERROR;
  MSG_LOW("Start util_clear_uim_cache",0,0,0);

  if (num_file > 0 && file_list == NULL)
  {
    MSG_ERROR("Numfile 0x%x but file_list is null", num_file, 0, 0);
    return;
  }
  if (num_file == 0 && file_list != NULL)
  {
    MSG_ERROR("Numfile 0 but file_list is not null", 0, 0, 0);
    return;
  }

  if ((uim_cmd_ptr = gsdi_get_uim_buf()) != NULL)
  {
    uim_cmd_ptr->hdr.command          = UIM_CLEAR_CACHE_F;
    uim_cmd_ptr->hdr.protocol         = UIM_NO_SUCH_MODE;
    uim_cmd_ptr->hdr.channel          = UIM_CHANNEL0;
    if ((num_file == 0) && (file_list == NULL))
    {
      uim_cmd_ptr->clear_cache.clr_cache_option    = UIM_OPTION_CLEAR_CACHE_ALL;
    }
    else
    {
      uim_cmd_ptr->clear_cache.clr_cache_option    = UIM_OPTION_CLEAR_CACHE_SELECTIVE;
    }
    uim_cmd_ptr->clear_cache.num_items = int32touint16(num_file);
    uim_cmd_ptr->clear_cache.item_list = file_list;

    /* populate slot information */
    gsdi_status = gsdi_populate_uim_slot( &uim_cmd_ptr->hdr.slot,
                                           slot);

    if (gsdi_status != GSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);
      MSG_ERROR(" Could not populate slot info Failing 0x%x", 0, 0, 0);
      return;
    }

    gsdi_send_cmd_to_uim_server(uim_cmd_ptr);

    /* Safety check for proper report  */
    if (uim_rsp_report.rpt_type == UIM_CLEAR_CACHE_R)
    {
      if (uim_rsp_report.rpt_status != UIM_PASS)
      {
        MSG_ERROR(" Clear Cache fail 0x%x", uim_rsp_report.rpt_status, 0, 0);
      }
    }
  }
  /* Deallocate the cmd from heap */
  MMGSDIUTIL_TMC_MEM_FREE( uim_cmd_ptr);

}

/*lint -esym(714,gsdi_uim_server_close_channel)*/
/*lint -esym(714,gsdi_uim_server_open_channel)*/
/*lint -esym(714,gsdi_uim_server_send_channel_data)*/

