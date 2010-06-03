/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      G S T K     M A I N      T A S K


GENERAL DESCRIPTION

  This source file contains the main GSTK task along with supporting functions.

EXTERNALIZED FUNCTIONS
  gstk_task_put_cmd_buf
    This function put the command onto gstk task command queue

  gstk_cmd
    This function allows clients to put the gstk_cmd onto the GSTK command
    queue.

  gstk_power_down
    This function flushes GSTK queue, vote UIM off and Ack TMC

  gstk_cm_ss_event_cb
    This function listens to CM SS events and will put a GSTK mode change
    command when appropriate

  gstk_test_automation_restart
    Called by test automation script via DIAG to soft restart GSTK

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk.c#11 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
04/28/09   sg      Added as argument in gstk_update_default_app_specific_tp
                   void keyword to remove warning
04/15/09   xz      Support client id clean up of modem restart
04/07/09   xz      Fix dead lock issue in main task loop
03/26/09   xz      Fix the issue of TP DL failing if GSTK fails to get app_id
                   when parse client TP and simplify TP download procedure
02/26/09   gg      Introduced Class B event registration
02/10/09   yb      Defined terminal response wait timers before clearing them
                   in gstk_util_cmd_details_ref_table_free_slot()
02/03/09   yb      Added secondary support for cell id information when
                   rr_get_cell_id returns NULL in gstk_mo_sms_ctrl_ind()
01/23/09   xz      Add gstk_priv_data
11/11/08   xz      Fix issue of unable to dereg via DIAG
11/06/08   sk      Fixed error message
10/22/08   sk      Intiialize client tp dl cache
                   Return error to client for client id reg or proactive
                   command reg when not in the right gstk state
                   Removed gstk_terminal_profile_data_from_client
                   Fixed lint errors
10/13/08   yb      Removed Lint Errors
10/06/08   yb      Sending BIP envelope after TR while BIP is working
09/29/08   xz      Added support of ESTK for test automation
09/14/08   sk      Fixed lint errors
09/08/08   sk      Added support for FEATURE_ESTK
09/05/08   xz      Change param type of gstk_test_automation_restart() to int
09/02/08   sk      Removed Featurization FEATURE_GSTK_SINGLE_CLIENT_RAW_FMT
08/29/08   yb      Fixed warning
08/28/08   sk      Fixed enhanced registration with class c and e cmds
08/28/08   yb      Added support for longer text string in terminal response
08/22/08   xz      Added gstk_test_automation_restart() to support test automation
08/22/08   sk      Added support to send end session to only those clients
                   that are involved in the current proactive session
08/21/08   sk      Added support for enhanced GSTK registration that includes
                   support for multiple clients to perform terminal profile
                   download and support for clients to register for a subset of
                   proactive command in GSTK or RAW format.
08/08/08   xz      Added support for raw proactive command, raw terminal response
                   and raw envelope command.
                   Added support for processing proactive command timer expiration
05/15/06   sk      Fixed warning
05/06/08   sk      Added support for event download envelope retry
02/06/08   sk      Support for PLI UTRAN NMR
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
11/07/07   sk      Added support for NAA Refresh
09/25/07   sk      Changed tp dl bytes to allow alpha and display of extn text
                   support indication to come from the app
08/07/07   sk      Fixed compilation problem and warning
08/01/07   sk      Fixed lint error
07/14/07   sk      Added support to requeue sms pp envelope if there is a
                   pending send sms tr
07/03/07   st      Merged changes from UNDP-1, add interface to DCTSTK
06/27/07   sk      Fixed cell id len value
05/11/07   sp      Featurize for LTK
04/19/07   nk      Added GSTK memcpy function
04/12/07   jk      Added GSTK MALLOC and FREE functions
03/22/07   sk      Featurized header file specific to 1X
11/29/06   sk      Downgraded message from error to high
10/23/06   sk      Fixed lint errors.
08/28/06   jar     Convert a detailed GSDI Error into a generic error.
08/25/06   sk      Featurized uim recovery
08/17/06   sk      Updated Terminal Profile bits
07/31/06   tml     Terminal Profile bit for GPRS Call Control
06/24/06   sk      Lint fixes
06/17/06   jar     Added FEATURE_GSTK_FOR_NON_LTK_BUILD around RRC Includes
                   to support VSIM in LTK.
06/13/06   sk      UIM recovery support
06/06/06   sk      Added support for network search mode
05/11/06   tml     Lint fix
05/03/06   sk      Removed checks for 2G cell id
05/03/06   tml     lint
04/06/06   sk      Updated Terminal Profile
04/04/06   sk      Added gstk_get_nv() to read NV items.
                   Added support for 1X related location information.
11/21/05   sp      Fixed lint errors
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
10/18/05   tml     Added protocol info
10/10/05   tml     Added terminal response error support for Access Tech
10/08/05   jar     Add Get Cell ID info function
10/07/05   sk      PLI/LS fixes
10/04/05   sk      Added extra messages in gstk_rrc_cell_change_ind_cb().
                   Also added check for card app type to add appropriate
                   support to extended cell id.
09/30/05   sk      Changed prototype for gstk_rrc_cell_change_ind_cb()
                   Added two new tables gstk_loci_table and rrc_loci_table
                   for maintaining plmn/lac/cell_id info from mm and rrc
                   respectively.
09/26/05   tml     Updated RRC cell id received variable to ensure location
                   status for normal service is only sent when both MM and RRC
                   info are received
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID
                   changed during normal service
09/20/05   tml     Terminal profile support update
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added support for Access Technology.
                   Added two new functions gstk_cm_access_tech_cb()
                   and gstk_check_for_pending_pro_cmd()
06/09/05   sun     Check for flag before processing commands
05/11/05   sst     Lint fixes
04/06/05   tml     Removed unnecessary timers for terminal response
11/23/04  sk/tml   fixed compilation warning
11/23/04   jar     Added Features around code to fix compile errors in dual slot
09/02/03   PMD     Added SMS CB Data Download supported in Terminal Profile
08/30/04   tml     Added gstk_profile_received
08/10/04   tml     Added GSTK_SEND_POSTPONE_ENVELOPE signal handling
06/29/04   tml     include ROAM status check for ss cb
06/28/04   tml     added uim notification during cm ss cb
05/11/04   tml     additional SAT/USAT support (timer management, language
                   notification etc)
04/16/04   tml     linted
04/02/04   tml     linted
03/05/04   tml     Allowed single slot to operate individually in Dual Slot
                   solution
03/01/04   tml     Merged from branch, fixes include:
                   location status envelope command fix, simplify gstk_wait
                   and initialize terminal profile cache to certain default value
09/13/03   tml     Changed slot_id_type to slot_id_enum_type
09/04/03   tml     Dual Slot support
06/23/03   tml     fixed compiler wanring for 1x/gsm
06/23/03   tml     Used CM_CLIENT_TYPE_GSTK and added default support for
                   command_response
05/16/03   tml     lint
04/17/03   tml     lint and UTK support
03/06/03   tml     Updated enum names
02/22/03   tml     Changed gstk registration with CM to another client type
                   memset the task cmd type before returning to calling functs
02/13/03   tml     Added/Changed DM Messages
02/11/03   tml     Initialize GSTK_CLIENT_ID in the gstk_client_table after
                   other client table initialization
02/07/03 jar/tml   Initial Version


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* OS Headers */
#include "rex.h"
#include "customer.h"

/* Task Definition Headers */
#include "task.h"
#ifndef FEATURE_CDMA1X
#include "tmc.h"
#else
#include "mc.h"
#endif /* FEATURE_CDMA1X */


/* Command/Msg Headers/Memory */
#include "msg.h"
#include "err.h"
#include "queue.h"
#include "memheap.h"

/* Task Reporting Headers */
#ifdef FEATURE_DOG
#include "dog.h"
#endif

/* Other Headers */
#include "dsm.h"
#include "customer.h"
#include "string.h"
#include "cm.h"

/* GSTK Specific Headers */
#include "gstk.h"
#include "gstki.h"
#include "gstkutil.h"
#include "gstk_state.h"
#include "gstk_envelope_cmd.h"
#include "gstk_envelope_rsp.h"
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */
#endif /* !FEATURE_GSTK_FOR_NON_LTK_BUILD */
/* GSTK implementation headers */

/* GSTK - GSDI Interface */
#include "gsdi_exp.h"

#ifdef FEATURE_DCTSTK_SUPPORT
#error code not present
#endif

#ifdef FEATURE_ESTK
#error code not present
#endif /* FEATURE_ESTK */

/* For UNIT Testing Only */
#include "uim.h"
#ifdef FEATURE_GSDI_UIM_SERVER
#include "virtuim.h"
#endif

#ifdef FEATURE_GSTK_STATE_TESTER
#include "gstk_state_tester.h"
#endif /*FEATURE_GSTK_STATE_TESTER */

#include "err.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Real Feature for Unit Test Framework Development */

/* Queue to hold commands for gstk */
static q_type gstk_task_cmd_q;

/* Queue from which tasks get Command Space */
static q_type gstk_task_free_q;

/* Timer definitions */
static rex_timer_type gstk_rpt_timer;
rex_timer_type gstk_tp_wait_timer;  /* 2 seconds timer*/
/* rex_timer_type gstk_term_rsp_wait_timer;  */ /* 5 seconds timer*/

#ifdef FEATURE_GSTK_STATE_TESTER
rex_timer_type gstk_state_tester_timer; /* 5 seconds timer */
rex_timer_type gstk_state_tester_next_test_timer; /* 1 second timer */
#endif /*FEATURE_GSTK_STATE_TESTER */

/* GSTK Specific Globals */
static gstk_task_cmd_type gstk_cmd_pool[ GSTK_NUM_CMD_BUFS ];

/* GSTK private data */
gstk_priv_data_type gstk_priv_data;

/* command details reference table */
gstk_cmd_details_ref_table_type command_details_ref_table[GSTK_MAX_PROACTIVE_COMMAND];

/* array of timer that GSTK will manage for timer management command */
gstk_rex_timer_type gstk_timer_management[GSTK_MAX_TIMER_SUPPORTED];

/* client arrays */
gstk_client_table_type gstk_client_table[GSTK_MAX_CLIENT];

#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

/* envelope command request arrays */
gstk_env_cmd_ref_table_type envelope_cmd_ref_table[GSTK_MAX_PROACTIVE_COMMAND];

/* event list */
uint8 gstk_evt_list[GSTK_MAX_EVENT_LIST_LEN];

static gstk_client_type GSTK_GSTK_TYPE = (gstk_client_type)10;

/* client id from cm */
cm_client_id_type gstk_client_id_from_cm;

/* mmgsdi client id */
mmgsdi_client_id_type gstk_mmgsdi_client_id;

/* This variable is set to TRUE when gstk allows UIM to fetch proactive command
   from card. And it is set to FALSE when gstk sends Terminal Response to UIM.
   It is used to check if a proactive command is in progress from card and TR
   has not been sent to it */
boolean gstk_proactive_cmd_in_progress;

/* This variable is set to TRUE when gstk dequeues a BIP Envelope command from its
   command queue. And it is set to FALSE when receives an Envelope response for
   BIP Envelope command.
   It is used to check if a BIP Envelope is in progress from gstk and it has not
   yet received an Envelope response for that. */
boolean gstk_bip_env_in_progress;

/* This variable is set to TRUE when gstk declines UIM request to do fetch. And
   it is set to FALSE when gstk receives a Proactive command.
   It is used to check if a proactive command from card is pending or not.*/
boolean gstk_uim_fetch_pending;

/* gstk critical section */
rex_crit_sect_type gstk_crit_sect;

/* Mask of signals interested in */
rex_sigs_type  gstk_requested_mask;

/* location information */
uint8 gstk_curr_mcc_mnc[3];
uint8 gstk_curr_lac[2];
gstk_cell_id_type gstk_curr_cell_id;
gstk_location_status_enum_type gstk_curr_location_status;
uint8 gstk_next_curr_mcc_mnc[3];
uint8 gstk_next_curr_lac[2];
gstk_cell_id_type gstk_next_curr_cell_id;
gstk_location_status_enum_type gstk_next_curr_location_status;
gstk_access_technology_type gstk_curr_rat;
gstk_access_technology_type gstk_next_curr_rat;

/* Count of terminal profile downloads sent by various clients to GSTK */
uint8 gstk_num_of_tp_dl_received;

boolean gstk_mm_param_received;
boolean gstk_pending_location_evt_envelope;

boolean gstk_mm_state_is_idle;
boolean gstk_cm_call_is_connected;
#ifndef FEATURE_NO_UIM_RECOVERY_SUPPORT
boolean gstk_gsdi_internal_sim_reset;
#endif /* FEATURE_NO_UIM_RECOVERY_SUPPORT */

#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
/* CDMA location information cache */
gstk_cdma_loc_info_cache_type cdma_location_info_cache;
#endif /* FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

#ifdef FEATURE_ENHANCED_NW_SELECTION
boolean gstk_continue_task;
#endif /* FEATURE_ENHANCED_NW_SELECTION */

boolean gstk_profile_received;

/* Current Card App Type */
gstk_app_enum_type gstk_curr_app_type;

#ifdef FEATURE_GSTK_DUAL_SLOTS

/* Information about slot initialization (last 2 bits), toolkit capability
** (middle 2 bits), and card error (first 2 bits). See dual slot masks defined
** in gstk.h.
*/
uint8 gstk_toolkit_info;

/* Current toolkit slot */
gstk_slot_id_enum_type gstk_curr_tk_slot;

/* Preferred toolkit slot */
/* Based on GSDI and Mode Change command,
   1. GSDI OK_TP and OK_TP2 => default to slot 1
   2. only 1 OK_TP is received => default to the OK_TP slot
   3. Mode Change command to an invalid slot (w/o TP cap) => remain the same
   4. Mode Change command to a valid slot => set to the new slot
   5. Upon receiving OK_TPs, before TP from client, rec'd Mode Change => set to
      Mode Change indicated slot */
/* Note: 5 is the reason that this cannot be taken out until a smarter way is found */
gstk_slot_id_enum_type gstk_preferred_tk_slot;

#endif /* FEATURE_GSTK_DUAL_SLOTS */

/* Facilitate Event download Envelope Retry */
gstk_pending_env_info_type gstk_curr_envelope_info;

/* Flag to indicate if current proactive session involves UI */
boolean gstk_is_curr_session_ui_related;

/* card mode */
gsdi_card_apps_T gstk_gsdi_card_mode;

gsdi_protocol_enum_type gstk_gsdi_protocol_slot[GSTK_NUM_SLOT];

#ifdef FEATURE_GSTK_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_GSTK_TEST_AUTOMATION */

#ifndef FEATURE_CAT_REL6
/* Definition of what ME Support */
uint8 gstk_terminal_profile_cache[GSTK_TERMINAL_PROFILE_SIZE] =
{

  0x27, /* Profile Download, SMS-PP, CB-DD, Menu selection, DD-Error */
  0x0F, /* Command result & Handling of alpha field */
  0xEC, /* Disp Text, Get Inkey, Get Input, Play Tone, Poll Int, Poll Off, Refresh */
  0xC0, /* Sel Item, Send SM, SendSS, SU Call, SU Menu, Provide Local Info */
  0x1F,
  0x10,/* Lang Sel, Brow Term, DA, Chnll St, Access tech, Disp params, Local Conn, NW Srch Mode */
  0x00,
  0x07,
  0x94,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  GSTK_TP_EIGHTEENTH_BYTE, /* GPRS Call Control */
  0x00,
  0x00
};
#else
/* Definition of what ME Support */
uint8 gstk_terminal_profile_cache[GSTK_TERMINAL_PROFILE_SIZE] =
{

  0x27, /* Profile Download, SMS-PP, CB-DD, Menu selection, DD-Error */
  0x0F, /* Command result & Handling of alpha field */
  0xEC, /* Disp Text, Get Inkey, Get Input, Play Tone, Poll Int, Poll Off, Refresh */
  0xC0, /* Sel Item, Send SM, SendSS, SU Call, SU Menu, Provide Local Info */
  0x1F,
  0x90, /* Lang Sel, Brow Term, DA, Chnl St, Access tech, Disp params, Local Conn, NW Srch Mode */
  0x00,
  0x07,
  0x94,
  0x00,

  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  GSTK_TP_EIGHTEENTH_BYTE, /* ESN, GPRS Call Control, IMEISV, PLI-Search Mode */
  0x00,
  0x00,

  0x00,
  0x00,
  GSTK_TP_TWENTY_THIRD_BYTE, /* MEID */
  0x10,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};
#endif /* FEATURE_CAT_REL6 */

#if defined(FEATURE_WCDMA) && !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#error code not present
#endif /* FEATURE_WCDMA && !FEATURE_GSTK_FOR_NON_LTK_BUILD*/

/* Specifically used for Event download envelope retry */
static int gstk_compare_specific_cmds(void *item_ptr,void *param);
static void gstk_process_specific_cmds(void *item_ptr,void *param);
/* Specifically used for BIP Envelope */
static int gstk_compare_bip_env_cmd(void *item_ptr,void *param);

#if defined(FEATURE_GSTK_TEST_AUTOMATION) && defined(FEATURE_ESTK)
extern void estk_gstk_init_cb (
  gstk_status_enum_type             gstk_reg_status,
  gstk_client_id_type               client_id,
  gstk_client_ref_data_type         user_data
);
#endif /* FEATURE_GSTK_TEST_AUTOMATION && FEATURE_ESTK */

/*===========================================================================
FUNCTION gstk_update_default_app_specific_tp

DESCRIPTION
  This function sends app specific terminal profile that GSTK supports by
  default

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
static gstk_status_enum_type gstk_update_default_app_specific_tp(void)
{
  uint8 num_of_tps = 0;

  /* GSTK default app specific terminal profiles */
  gstk_profile_dl_support_ext_type default_tp[] =
  {
    {1, GSTK_9EXX_RSP_CODE_SUPPORT,              (uint8)GSTK_APP_SIM},
    {1, GSTK_CC_ALWAYS_SENT_DURING_REDIAL,       (uint8)GSTK_APP_SIM},
    {1, GSTK_USAT_SMS_PP_DL_SUPPORT_BYTE1_BIT_5, (uint8)GSTK_APP_USIM},
    {1, GSTK_USAT_CC_SUPPORT_BYTE_1_BIT_7,       (uint8)GSTK_APP_USIM},
    {1, GSTK_USAT_CC_SUPPORT_BYTE_1_BIT_8,       (uint8)GSTK_APP_USIM},
    {1, GSTK_PLI_NMR_UTRAN,                      (uint8)GSTK_APP_USIM},
    {1, GSTK_USAT_CC_SUPPORT_BYTE_8_BIT_8,       (uint8)GSTK_APP_USIM},
    {1, GSTK_USAT_CC_SUPPORT_BYTE_2_BIT_5,       (uint8)GSTK_APP_USIM}
  };

  num_of_tps = sizeof(default_tp) / sizeof(gstk_profile_dl_support_ext_type);
  return gstk_update_profile_download_value_ext(
           GSTK_CLIENT_ID,
           0,
           num_of_tps,
           default_tp);
} /* gstk_update_default_app_specific_tp */

/*===========================================================================
FUNCTION   gstk_cm_ss_event_cb

DESCRIPTION
  Serving system (SS) event callback function, called when an SS event is happening.
  This function populates the various call related indication types and
  puts the command in the GSTK command queue.

PARAMETERS
  ss_event: [Input] CM serving system event
  ss_info_ptr: [Input] Pointer to the ss info struct

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
void gstk_cm_ss_event_cb (
  cm_ss_event_e_type         ss_event,
  const cm_mm_ss_info_s_type *ss_info_ptr
)
{
  gstk_task_cmd_type *task_cmd = NULL;

  if (ss_info_ptr == NULL)
  {
    ERR("Null SS info ptr fr CM", 0, 0, 0);
    return;
  }

  MSG_HIGH("gstk_cm_ss_event_cb: event %d", ss_event, 0, 0);
  switch (ss_event) {
  case CM_SS_EVENT_SRV_CHANGED:
    /* determine if the system mode has changed */
    MSG_HIGH("CM_SS_EVENT_SRV_CHANGED evt received from CM..",0,0,0);
    if ( ss_info_ptr->changed_fields & (CM_SS_SYS_MODE_MASK | CM_SS_ROAM_STATUS_MASK
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
                                        |
                                        CM_SS_SID_MASK | CM_SS_NID_MASK |
                                        CM_SS_BASE_STATION_PARMS_CHGD_MASK |
                                        CM_SS_MCC_MASK | CM_SS_IMSI_11_12_MASK |
                                        CM_SS_SRV_STATUS_MASK
#endif /* ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */
                                        ))
    {
      MSG_MED( "CM mode changed, sys_mode=%d",
                ss_info_ptr->sys_mode, 0, 0 );

#ifdef FEATURE_GSTK_DUAL_SLOTS
      /* check for valid sys mode before getting command pointer */
      if ((ss_info_ptr->sys_mode != SYS_SYS_MODE_CDMA) &&
        (ss_info_ptr->sys_mode != SYS_SYS_MODE_GSM))
      {
        MSG_ERROR("Unknown System mode", 0, 0, 0);
        return;
      }
#endif /* FEATURE_GSTK_DUAL_SLOTS */

      /* get command from buffer */
      task_cmd = gstk_task_get_cmd_buf();
      if (task_cmd != NULL)
      {
        /* build the gstk_cmd */
        switch (ss_info_ptr->sys_mode)
        {
        case SYS_SYS_MODE_CDMA:
#ifdef FEATURE_GSTK_DUAL_SLOTS
          task_cmd->cmd.mode_change_ind.cmd.new_mode = GSTK_CDMA_MODE;
#endif /* FEATURE_GSTK_DUAL_SLOTS */
          task_cmd->cmd.mode_change_ind.cmd.access_technology=  GSTK_ACCESS_TECH_CDMA;
          if(ss_info_ptr->srv_status != SYS_SRV_STATUS_SRV)
          {
            MSG_ERROR(" Invalid Srvc status from CM",0,0,0);
            return;
          }
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
          if(gstk_copy_1x_info_from_cm(&(task_cmd->cmd.mode_change_ind.cmd.cdma_loc_info),
                                          ss_info_ptr) != GSTK_SUCCESS)
          {
            MSG_ERROR("ERR: Copy from ss_info_ptr failed",0,0,0);
            return;
          }
#endif /* #ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */
          break;
        case SYS_SYS_MODE_GSM:
#ifdef FEATURE_GSTK_DUAL_SLOTS
          task_cmd->cmd.mode_change_ind.cmd.new_mode = GSTK_GSM_MODE;
#endif /* FEATURE_GSTK_DUAL_SLOTS */
          if((ss_info_ptr->srv_status != SYS_SRV_STATUS_SRV) &&
            (ss_info_ptr->srv_status != SYS_SRV_STATUS_LIMITED))
          {
            MSG_ERROR(" Invalid Srvc status from CM",0,0,0);
            return;
          }
          task_cmd->cmd.mode_change_ind.cmd.access_technology =  GSTK_ACCESS_TECH_GSM;
          break;
        case SYS_SYS_MODE_WCDMA:
          if((ss_info_ptr->srv_status != SYS_SRV_STATUS_SRV) &&
            (ss_info_ptr->srv_status != SYS_SRV_STATUS_LIMITED))
          {
            MSG_ERROR(" Invalid Srvc status from CM",0,0,0);
            return;
          }
          task_cmd->cmd.mode_change_ind.cmd.access_technology = GSTK_ACCESS_TECH_UTRAN;
          break;
        case SYS_SYS_MODE_NO_SRV:
          task_cmd->cmd.mode_change_ind.cmd.access_technology = GSTK_ACCESS_NONE;
          break;
        default:  /* all other modes */
          ERR("Unknown Sys Mode, should never be here", 0, 0, 0);
          break;
        }

        switch(ss_info_ptr->srv_status)
        {
        case SYS_SRV_STATUS_NO_SRV:
        case SYS_SRV_STATUS_PWR_SAVE:
          task_cmd->cmd.mode_change_ind.cmd.location_status = GSTK_NO_SERVICE;
          break;
        case SYS_SRV_STATUS_SRV:
          task_cmd->cmd.mode_change_ind.cmd.location_status = GSTK_NORMAL_SERVICE;
          break;
        case SYS_SRV_STATUS_LIMITED:
          task_cmd->cmd.mode_change_ind.cmd.location_status = GSTK_LIMITED_SERVICE;
          break;
        default:
          MSG_ERROR(" Invalid Srvc status from CM",0,0,0);
          return;
        }
        MSG_HIGH("Access tech is %d",
        task_cmd->cmd.mode_change_ind.cmd.access_technology,0,0);
        task_cmd->cmd.mode_change_ind.message_header.command_group =
            GSTK_MODE_CHANGE_IND;
        task_cmd->cmd.mode_change_ind.message_header.command_id = 0; /* doesn't matter */
        task_cmd->cmd.mode_change_ind.message_header.user_data = 0;  /* doesn't matter */

        /* put command in command queue */
        gstk_task_put_cmd_buf(task_cmd);
        #ifdef FEATURE_UIM_INACTIVE_SLOT_POWERDOWN
        /* Notify UIM of the serving system */
        uim_notify_sys_mode( ss_info_ptr->sys_mode );
#endif /* FEATURE_UIM_INACTIVE_SLOT_POWERDOWN */
      }
      else
      {
        ERR("No more GSTK buffer", 0, 0, 0);
      }
    }
    break;

  case CM_SS_EVENT_INFO:
    MSG_HIGH("CM_SS_EVENT_INFO evt from CM..",0,0,0);
    break;

  default:
    MSG_LOW("No relevant SS events", 0, 0, 0);
    break;
  }
} /* gstk_cm_ss_event_cb */

/*===========================================================================
FUNCTION GSTK_WAIT

DESCRIPTION
  Main wait routine for GSTK task.

PARAMETERS
  requested_mask [Input]: input signal to be waited on

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals).  The calling unit can then check for the reason for
  exiting the wait.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
LOCAL rex_sigs_type gstk_wait
(
  rex_sigs_type requested_mask    /* Mask of signals to wait for */
)
{
  rex_sigs_type rex_signals_mask;  /* Mask of signals returned by REX     */
  boolean       gstk_is_specific_signal = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   MSG_LOW("GSTK_WAIT FOR requested_mask = 0x%x, gstk_requested_mask = 0x%x",requested_mask,gstk_requested_mask,0);

  /* --------------------------------------------------------------
  ** Check message queue:  If the request mask contains a check for
  ** GSTK_TASK_CMD_Q_SIG then we should check the queue, and set in
  ** the queue_mask an indication that something is waiting to be
  ** serviced on that queue.  The rex signal is also cleared
  ** regardless of the queue count.  This is to cover the case
  ** where there is a signal set but nothing on the queue.
  ** ------------------------------------------------------------- */
  if(requested_mask == gstk_requested_mask)
  {
    gstk_is_specific_signal = FALSE;
  }
  if (requested_mask & GSTK_TASK_CMD_Q_SIG)
  {
    (void) rex_clr_sigs( &gstk_tcb, GSTK_TASK_CMD_Q_SIG );

    if (q_cnt( &gstk_task_cmd_q ))
    {
      (void) rex_set_sigs(&gstk_tcb,GSTK_TASK_CMD_Q_SIG);
    }
  }


  /* --------------------------------------------------------------------
  ** Loop until the current signals from REX (including the queue signals
  ** faked due to an item on a queue, if any) match with one or more of
  ** the requested signals.
  ** -------------------------------------------------------------------- */
  do
  {
#ifdef FEATURE_DOG
    /* Kick the watchdog */
    dog_report( DOG_GSTK_RPT );
    (void) rex_set_timer( &gstk_rpt_timer, DOG_GSTK_RPT_TIME );
#endif /* FEATURE_DOG */
     if(gstk_is_specific_signal == FALSE)
     {
       requested_mask = gstk_requested_mask;
     }

    rex_signals_mask = rex_wait( requested_mask | GSTK_RPT_TIMER_SIG );


    /* ---------------------------------------------------------------
    ** Make sure that if we got a queue signal, that there is actually
    ** an item on the queue indicated
    ** --------------------------------------------------------------- */
    if (rex_signals_mask & GSTK_TASK_CMD_Q_SIG)
    {
      if (q_cnt(&gstk_task_cmd_q) == 0)   /* No elements in queue ? */
      {
        (void) rex_clr_sigs( &gstk_tcb, GSTK_TASK_CMD_Q_SIG );
        rex_signals_mask &= ~GSTK_TASK_CMD_Q_SIG;  /* Turn off bit */
      }
    }

  /* do while mask requested does not match current mask */
  } while ((rex_signals_mask & requested_mask) == 0);

  return (rex_signals_mask);

} /* end gstk_wait() */

/*===========================================================================
FUNCTION gstk_init

DESCRIPTION
  GSTK Task Intialization Procedures.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
LOCAL static void gstk_init( void )
{
  uint8                            i                       = 0;
  cm_client_status_e_type          cm_init_return_status   = CM_CLIENT_OK;
  cm_client_status_e_type          cm_reg_return_status    = CM_CLIENT_OK;
  mmgsdi_return_enum_type          mmgsdi_status           = MMGSDI_SUCCESS;

  MSG_HIGH(" GSTK_INIT",0,0,0);

  /* Clear All Signals */
  (void) rex_clr_sigs(&gstk_tcb, (rex_sigs_type)~0);

  /* Initialize timers */
  rex_def_timer( &gstk_rpt_timer, &gstk_tcb, GSTK_RPT_TIMER_SIG );

  /* terminal profile command wait timer */
  rex_def_timer( &gstk_tp_wait_timer, &gstk_tcb, GSTK_TP_TIMER_SIG );

  /* Initialize structure that stores current envelope info */
  memset(&gstk_curr_envelope_info,0x00,sizeof(gstk_curr_envelope_info));

  /* initialize GSTK private data */
  memset(&gstk_priv_data, 0x00, sizeof(gstk_priv_data));

#ifdef FEATURE_GSTK_STATE_TESTER
  /* Initialize timers */
  rex_def_timer( &gstk_state_tester_timer, &gstk_tcb, GSTK_STATE_TESTER_TIMER_SIG );
  rex_def_timer( &gstk_state_tester_next_test_timer, &gstk_tcb, GSTK_STATE_TESTER_NEXT_TEST_TIMER_SIG );
#endif /*FEATURE_GSTK_STATE_TESTER */

  /* define timer management timer */
  for (i = 0; i< GSTK_MAX_TIMER_SUPPORTED; i++) {
    rex_def_timer_ex( &(gstk_timer_management[i].timer),
                      gstk_rex_timer_expire_cb,
                      i );
    rex_def_timer_ex( &(gstk_timer_management[i].timer_elapsed),
                      gstk_rex_timer_elapsed_expire_cb,
                      i );
  }

  (void) q_init(&gstk_task_cmd_q);
  (void) q_init(&gstk_task_free_q);

  /* Fill gstk_task_free_q */
  for (i = 0; i < GSTK_NUM_CMD_BUFS; i++)
  {
      gstk_cmd_pool[i].cmd_hdr.done_q_ptr = &gstk_task_free_q;
      q_put( &gstk_task_free_q,
             q_link( &gstk_cmd_pool[i],
                    &gstk_cmd_pool[i].cmd_hdr.link ) );
  }

  /* Initialize the Critical Section for GSTK Task */
  rex_init_crit_sect(&gstk_crit_sect);

  /* client init with CM */
  cm_init_return_status = cm_client_init(CM_CLIENT_TYPE_GSTK, &gstk_client_id_from_cm);

  if ( cm_init_return_status != CM_CLIENT_OK ) {
    ERR_FATAL("cm_client_init() returned bad status %d", (int32)cm_init_return_status, 0, 0);
  }

#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)

  (void)cm_client_act(gstk_client_id_from_cm);

#endif /* FEATURE_GSTK_FOR_NON_LTK_BUILD */
  /* register for CM serving system mode change event */
  /* This will now also be used to provide support for CCAT-PLI (location info)
  */
  cm_reg_return_status = cm_mm_client_ss_reg(
      /* pointer to the client struct */
      gstk_client_id_from_cm,
      /* pointer to a cb function for ss events */
      gstk_cm_ss_event_cb,
      /* event registration */
      (cm_client_event_reg_e_type)CM_CLIENT_EVENT_REG,
      /* from this call event */
      (cm_ss_event_e_type)CM_SS_EVENT_SRV_CHANGED,
      /* to this call event */
      (cm_ss_event_e_type)CM_SS_EVENT_SRV_CHANGED,
      NULL );                     /* pointer to a cb function for errors */
  if (cm_reg_return_status != CM_CLIENT_OK) {
    MSG_ERROR("Failed to register for CM mode change", 0, 0, 0);
  }

  /* register for call connected event */
  cm_reg_return_status = cm_mm_client_call_reg(
                        gstk_client_id_from_cm,     /* pointer to the client struct */
                        gstk_cm_call_event_cb,      /* pointer to a cb function for call events */
                        CM_CLIENT_EVENT_REG,        /* event registration */
                        CM_CALL_EVENT_CONNECT,      /* from this call event */
                        CM_CALL_EVENT_CONNECT,      /* to this call event */
                        NULL );                     /* pointer to a cb function for errors */
  if (cm_reg_return_status != CM_CLIENT_OK) {
    MSG_ERROR("Failed to register for CM Call Connect event", 0, 0, 0);
  }

  /* register for call disconnected event */

  cm_reg_return_status = cm_mm_client_call_reg(
                    gstk_client_id_from_cm,     /* pointer to the client struct */
                    gstk_cm_call_event_cb,      /* pointer to a cb function for call events */
                    CM_CLIENT_EVENT_REG,        /* event registration */
                    CM_CALL_EVENT_END,          /* from this call event */
                    CM_CALL_EVENT_END,          /* to this call event */
                    NULL );                     /* pointer to a cb function for errors */
  if (cm_reg_return_status != CM_CLIENT_OK) {
    MSG_ERROR("Failed to register for CM Call DisConnect event", 0, 0, 0);
  }

  /* Get client id from mmgsdi */
  mmgsdi_status = mmgsdi_client_id_reg ((mmgsdi_callback_type)gstk_mmgsdi_cmd_cb,0);
  if(mmgsdi_status != MMGSDI_SUCCESS)
    MSG_ERROR("mmgsdi_client_id_reg() result 0x%x", mmgsdi_status, 0, 0);

  /* initialize the mm_param received to FALSE */
  /* this is set to true when received the first mm idle event in the idle state */
  gstk_mm_param_received = FALSE;

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

  /* initialize other slots in GSTK client table */
  for (i = 0; i < GSTK_MAX_CLIENT; i++) {
    gstk_client_table[i].evt_cb.client_cb = NULL;
    gstk_client_table[i].evt_cb_type      = GSTK_CLI_CB_NONE;
    gstk_client_table[i].client_type      = GSTK_NO_TYPE;
    /* NOTE:Evt reg bitmask in the gstk_client_table is not used
       if FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT is turned ON
    */
    gstk_client_table[i].evt_reg_bitmask  = 0;
    gstk_client_table[i].free             = TRUE;
    gstk_client_table[i].user_data        = 0;
    gstk_client_table[i].client_init_cmd_cb = NULL;
  }

#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

/* supress 778 => constant expression evaluates to 0 */
/*lint -e778 */
  /* initialize first slot GSTK client table, reserve for GSTK use.
     Some members have alreaby been initialized above in the for loop */
  gstk_client_table[GSTK_CLIENT_ID-1].client_type    = GSTK_GSTK_TYPE;
  gstk_client_table[GSTK_CLIENT_ID-1].free           = FALSE;
  gstk_client_table[GSTK_CLIENT_ID-1].client_init_cmd_cb = NULL;
/*lint +e778 */

  /* terminal response wait timer */
  /* assign timer signal to each timer in the table */
  rex_def_timer( &(command_details_ref_table[0].tp_rpt_time), &gstk_tcb, GSTK_TERM_RSP_TIMER_SIG1);
  rex_def_timer( &(command_details_ref_table[1].tp_rpt_time), &gstk_tcb, GSTK_TERM_RSP_TIMER_SIG2);
  rex_def_timer( &(command_details_ref_table[2].tp_rpt_time), &gstk_tcb, GSTK_TERM_RSP_TIMER_SIG3);
  rex_def_timer( &(command_details_ref_table[3].tp_rpt_time), &gstk_tcb, GSTK_TERM_RSP_TIMER_SIG4);
  rex_def_timer( &(command_details_ref_table[4].tp_rpt_time), &gstk_tcb, GSTK_TERM_RSP_TIMER_SIG5);
  rex_def_timer( &(command_details_ref_table[5].tp_rpt_time), &gstk_tcb, GSTK_TERM_RSP_TIMER_SIG6);
  rex_def_timer( &(command_details_ref_table[6].tp_rpt_time), &gstk_tcb, GSTK_TERM_RSP_TIMER_SIG7);
  rex_def_timer( &(command_details_ref_table[7].tp_rpt_time), &gstk_tcb, GSTK_TERM_RSP_TIMER_SIG8);

  /* initilaize GSTK command details table */
  for (i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; i++) {
    gstk_util_cmd_details_ref_table_free_slot(i);
  }

  /* initialize GSTK env command ref table */
  for (i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; i++) {
    gstk_util_env_cmd_ref_table_free_slot(i);
  }

  gstk_cm_call_is_connected = FALSE;
  gstk_mm_state_is_idle     = FALSE;
  gstk_pending_location_evt_envelope = FALSE;
  memset(gstk_curr_mcc_mnc, 0xFF, 3);
  memset(gstk_curr_lac, 0xFF, 2);
  memset(gstk_curr_cell_id.cell_id, 0xFF, GSTK_MAX_CELL_ID_LEN);
  gstk_curr_cell_id.cell_len = 0;
  gstk_curr_location_status = GSTK_NORMAL_SERVICE;
  gstk_curr_rat = GSTK_ACCESS_NONE;
  memset(gstk_next_curr_mcc_mnc, 0xFF, 3);
  memset(gstk_next_curr_lac, 0xFF, 2);
  memset(gstk_next_curr_cell_id.cell_id, 0xFF, GSTK_MAX_CELL_ID_LEN);
  gstk_next_curr_cell_id.cell_len = 0;
  gstk_next_curr_location_status = GSTK_NO_SERVICE;
  gstk_next_curr_rat = GSTK_ACCESS_NONE;

  gstk_uim_fetch_pending         = FALSE;
  gstk_bip_env_in_progress       = FALSE;
  gstk_proactive_cmd_in_progress = FALSE;

  gstk_profile_received = FALSE;

  gstk_is_curr_session_ui_related = FALSE;

  /* card mode initialization */
  gstk_gsdi_card_mode.apps_enabled = GSDI_NO_APP_MASK;

  gstk_gsdi_protocol_slot[0] = GSDI_PROTOCOL_NONE;

#ifndef FEATURE_NO_UIM_RECOVERY_SUPPORT
  /* Set the internal sim reset flag to FALSE */
  gstk_gsdi_internal_sim_reset = FALSE;
#endif /* FEATURE_NO_UIM_RECOVERY_SUPPORT */
#ifdef FEATURE_GSTK_DUAL_SLOTS

  gstk_gsdi_card_mode.apps2_enabled = GSDI_NO_APP_MASK;

  /* initialize toolkit-related info */
  gstk_toolkit_info               = 0;
  gstk_preferred_tk_slot          = GSTK_SLOT_NONE;
  gstk_curr_tk_slot               = GSTK_SLOT_NONE;
  gstk_gsdi_protocol_slot[1]      = GSDI_PROTOCOL_NONE;
  gstk_curr_app_type              = GSTK_APP_NONE;
  gstk_num_of_tp_dl_received      = 0;

#endif /* FEATURE_GSTK_DUAL_SLOTS */
#ifdef FEATURE_ENHANCED_NW_SELECTION
  gstk_continue_task = TRUE;
#endif /* FEATURE_ENHANCED_NW_SELECTION */

#ifdef FEATURE_GSTK_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_GSTK_TEST_AUTOMATION */

  if (gstk_update_default_app_specific_tp() != GSTK_SUCCESS)
  {
    MSG_ERROR("Failed to update GSTP default TP!", 0, 0, 0);

    /* increment counter even if it fails to queue TP cmd */
    ++ gstk_num_of_tp_dl_received;
  }

  MSG_HIGH(" == GSTK _ INIT _ COMPLETE ",0,0,0);
} /* gstk_init */

/*===========================================================================
FUNCTION GSTK_MMGSDI_CMD_CB

DESCRIPTION
  GSTK Callback for Client Id Reg and Refresh from MMGSDI

PARAMETER
  status  [Input]: status of command returned from MMGSDI
  cnf     [Input]: confirmation type
  cnf_ptr [Input]: confirmation pointer

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_mmgsdi_cmd_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;

  MSG_HIGH("In gstk_mmgsdi_cmd_cb()",0,0,0);

  if(cnf_ptr == NULL)
  {
    MSG_HIGH("cnf_ptr is NULL",0,0,0);
  }

  switch(cnf)
  {
  case MMGSDI_CLIENT_ID_REG_CNF:
    MSG_HIGH("MMGSDI_CLIENT_ID_REG_CNF result 0x%x",status,0,0);
    break;
  case MMGSDI_REFRESH_CNF:
    gstk_status = gstk_handle_mmgsdi_refresh_cnf(status, &(cnf_ptr->refresh_cnf));
    MSG_HIGH("gstk_handle_mmgsdi_refresh_cnf() result 0x%x",gstk_status,0,0);
    break;
  default:
    MSG_HIGH("Unhandled mmgsdi cnf 0x%x",cnf,0,0);
    break;
  }
}

/*===========================================================================
FUNCTION GSTK_GSDI_SIM_EVENTS_CB

DESCRIPTION
  GSTK Callback for SIM Events from GSDI

PARAMETER
  gstk_event [Input]: the GSDI event received

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void gstk_gsdi_sim_events_cb (
    gsdi_sim_events_T   gsdi_event
)
{
  gstk_task_cmd_type * task_cmd;

  MSG_HIGH("GSTK receives GSDI event in Callback = %x",gsdi_event,0,0);

  /* Convert the Detailed Error to a Generic Error since GSTK
  ** doesn't need to differentiate between the detailed Errors */
  gsdi_convert_detailed_err_evt_to_gen_err(gsdi_event,
                                           &gsdi_event);

  MSG_HIGH("GSTK receives GSDI event in Callback = %x",gsdi_event,0,0);

#if defined(FEATURE_WCDMA) && !defined(FEATURE_GSTK_FOR_NON_LTK_BUILD)
#error code not present
#endif /* FEATURE_WCDMA && !FEATURE_GSTK_FOR_NON_LTK_BUILD */

  /* get the command from buffer */
  task_cmd = gstk_task_get_cmd_buf();
  if (task_cmd != NULL) {
  /* Build the gstk_cmd */

    task_cmd->cmd.gsdi_evt.command_group = GSTK_GSDI_CMD;
    task_cmd->cmd.gsdi_evt.command_id = (int)gsdi_event;
    task_cmd->cmd.gsdi_evt.user_data = 0;

    /* put command back to command queue */
    gstk_task_put_cmd_buf(task_cmd);

#ifdef FEATURE_GSTK_STATE_TESTER
    if (gsdi_event == GSDI_SIM_INIT_COMPLETED_2) {
      /* start a timer that will do the different combination of state change verification */
      /* first clear the timer */
      (void) rex_clr_timer( &gstk_state_tester_timer);

      /* then start the timer */
      (void) rex_set_timer( &gstk_state_tester_timer,
                            5000 );
    }
#endif /*FEATURE_GSTK_STATE_TESTER */
  }
}/* gstk_gsdi_sim_events_cb */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */

/*===========================================================================
FUNCTION GSTK_FREE_CMD_Q_PTR

DESCRIPTION
  Free Callback Pointer so that it can be used again.

PARAMETER
  task_cmd [Input]:  Pointer to be freed

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
/* LOCAL void gstk_free_cmd_q_ptr(
  gstk_task_cmd_type * task_cmd)
{
  if(task_cmd != NULL)
  {
    q_put(&gstk_task_free_q,&task_cmd->cmd_hdr.link);
  }
}*/

/*===========================================================================
FUNCTION GSTK_TASK

DESCRIPTION
  GSTK Main Task

PARAMETER
  dummy [Input]:  no use!

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_task( dword dummy )
{
  gstk_status_enum_type           gstk_status               = GSTK_SUCCESS;
  gstk_task_cmd_type              *task_cmd_ptr             = NULL;
  boolean                         gstk_is_cmd_processed_qld = FALSE;
  mmgsdi_return_enum_type         mmgsdi_status             = MMGSDI_ERROR;
  /* Define sigs variable */
  rex_sigs_type                   gstk_sigs;

  MSG_LOW("STARTING gstk_task",0,0,0);

  /* Init gstk task */
  gstk_init();

  /* ------------------------------------------------
  ** Wait for start signal from the Main Control task
  ** ------------------------------------------------ */


#ifdef FEATURE_DOG
  /* ------------------------------------------------------------------
  ** Initially kick watchdog and set timer for watchdog report interval
  ** ------------------------------------------------------------------ */
  dog_report(DOG_GSTK_RPT);
  (void) rex_set_timer(&gstk_rpt_timer, DOG_GSTK_RPT_TIME);
#endif /* FEATURE_DOG */

  tmc_task_start();
  /* ------------------------------------------------------------------
  ** Register a Callback for SIM Events to GSDI
  ** ------------------------------------------------------------------ */
  (void) gsdi_reg_callback(gstk_gsdi_sim_events_cb);

#ifdef FEATURE_DCTSTK_SUPPORT
#error code not present
#endif /* FEATURE_DCTSTK_SUPPORT */
#ifdef FEATURE_ESTK
#error code not present
#endif /* FEATURE_ESTK */

    gstk_requested_mask = GSTK_TASK_CMD_Q_SIG |
                          GSTK_TP_TIMER_SIG   |
                          TASK_STOP_SIG       |
                          TASK_OFFLINE_SIG    |
                          GSTK_DO_TP_DL_SIG |
                          GSTK_TERM_RSP_TIMER_SIG1 |
                          GSTK_TERM_RSP_TIMER_SIG2 |
                          GSTK_TERM_RSP_TIMER_SIG3 |
                          GSTK_TERM_RSP_TIMER_SIG4 |
                          GSTK_TERM_RSP_TIMER_SIG5 |
                          GSTK_TERM_RSP_TIMER_SIG6 |
                          GSTK_TERM_RSP_TIMER_SIG7 |
                          GSTK_TERM_RSP_TIMER_SIG8 |
                          GSTK_RETRY_ENVELOPE_SIG |
                          GSTK_SEND_POSTPONE_ENVELOPE_SIG |
                          GSTK_TIMER_FOR_ENV_RETRY_SIG
#ifdef FEATURE_DCTSTK_SUPPORT
#error code not present
#endif /*FEATURE_DCTSTK_SUPPORT */
#ifdef FEATURE_GSTK_STATE_TESTER
                          | GSTK_STATE_TESTER_TIMER_SIG
                          | GSTK_STATE_TESTER_NEXT_TEST_TIMER_SIG
#endif /*FEATURE_GSTK_STATE_TESTER */
#ifdef FEATURE_ESTK
#error code not present
#endif /* FEATURE_ESTK */
;
    for ( ; ; )
    {

      /* Add Wait Code and Wait Function Here */
      gstk_sigs = gstk_wait(gstk_requested_mask);
      if ( gstk_sigs & TASK_STOP_SIG ) {
        MSG_HIGH("Got STOP Sig", 0, 0, 0);
        (void) rex_clr_sigs(&gstk_tcb, TASK_STOP_SIG);
        gstk_state = GSTK_POWER_DOWN_S;
        gstk_status = gstk_power_down();
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("Err in proc stop Sig", 0, 0, 0);
        }
        task_stop();
      }
      else if (gstk_sigs & TASK_OFFLINE_SIG ) {
        MSG_HIGH("Got Offline Sig", 0, 0, 0);
        (void) rex_clr_sigs(&gstk_tcb, TASK_OFFLINE_SIG);
        task_offline();
      }
      else if( gstk_sigs & GSTK_TP_TIMER_SIG ) {
        MSG_LOW("GOT GSTK_TP_TIMER_SIG",0,0,0);
            /* clear the signal */
        (void) rex_clr_sigs(&gstk_tcb, GSTK_TP_TIMER_SIG);
        gstk_status = gstk_build_timer_expired_cmd(GSTK_TP_TIMER,
                                                   GSTK_TP_TIMER_SIG);
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("Err in proc tp timer Sig", 0, 0, 0);
        }
      }
      else if ( gstk_sigs & GSTK_TIMER_FOR_ENV_RETRY_SIG )
      {
        MSG_HIGH("GOT GSTK_TIMER_FOR_ENV_RETRY_SIG",0,0,0);
        /* clear the signal */
        (void) rex_clr_sigs(&gstk_tcb, GSTK_TIMER_FOR_ENV_RETRY_SIG);
        /* retry the envelope */
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
        /* If the queue has pending commands, set the gstk signal */
        if(q_cnt( &gstk_task_cmd_q ) > 0)
        {
          (void) rex_set_sigs(&gstk_tcb, GSTK_TASK_CMD_Q_SIG);
        }
      }
      else if ( gstk_sigs & GSTK_TERM_RSP_TIMER_SIG1 ) {
        MSG_HIGH("GOT GSTK_TERM_RSP_TIMER_SIG1",0,0,0);
        /* clear the signal */
        (void) rex_clr_sigs(&gstk_tcb, GSTK_TERM_RSP_TIMER_SIG1);
        gstk_status = gstk_build_timer_expired_cmd(GSTK_TR_TIMER1,
                                                   GSTK_TERM_RSP_TIMER_SIG1);
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("Err in proc term rsp timer1 Sig", 0, 0, 0);
        }
      }
      else if ( gstk_sigs & GSTK_TERM_RSP_TIMER_SIG2 ) {
        MSG_LOW("GSTK_TERM_RSP_TIMER_SIG2",0,0,0);
        /* clear the signal */
        (void) rex_clr_sigs(&gstk_tcb, GSTK_TERM_RSP_TIMER_SIG2);
        gstk_status = gstk_build_timer_expired_cmd(GSTK_TR_TIMER2,
                                                   GSTK_TERM_RSP_TIMER_SIG2);
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("Err in proc term rsp timer2 Sig", 0, 0, 0);
        }
      }
      else if ( gstk_sigs & GSTK_TERM_RSP_TIMER_SIG3 ) {
        MSG_LOW("GOT GSTK_TERM_RSP_TIMER_SIG3",0,0,0);
        /* clear the signal */
        (void) rex_clr_sigs(&gstk_tcb, GSTK_TERM_RSP_TIMER_SIG3);
        gstk_status = gstk_build_timer_expired_cmd(GSTK_TR_TIMER3,
                                                   GSTK_TERM_RSP_TIMER_SIG3);
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("Err in proc term rsp timer3 Sig", 0, 0, 0);
        }
      }
      else if ( gstk_sigs & GSTK_TERM_RSP_TIMER_SIG4 ) {
        MSG_LOW("GOT GSTK_TERM_RSP_TIMER_SIG4",0,0,0);
        /* clear the signal */
        (void) rex_clr_sigs(&gstk_tcb, GSTK_TERM_RSP_TIMER_SIG4);
        gstk_status = gstk_build_timer_expired_cmd(GSTK_TR_TIMER4,
                                                   GSTK_TERM_RSP_TIMER_SIG4);
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("Err in proc term rsp timer4 Sig", 0, 0, 0);
        }
      }
      else if ( gstk_sigs & GSTK_TERM_RSP_TIMER_SIG5 ) {
        MSG_LOW("GOT GSTK_TERM_RSP_TIMER_SIG5",0,0,0);
        /* clear the signal */
        (void) rex_clr_sigs(&gstk_tcb, GSTK_TERM_RSP_TIMER_SIG5);
        gstk_status = gstk_build_timer_expired_cmd(GSTK_TR_TIMER5,
                                                   GSTK_TERM_RSP_TIMER_SIG5);
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("Err in proc term rsp timer5 Sig", 0, 0, 0);
        }
      }
      else if ( gstk_sigs & GSTK_TERM_RSP_TIMER_SIG6 ) {
        MSG_LOW("GOT GSTK_TERM_RSP_TIMER_SIG6",0,0,0);
        /* clear the signal */
        (void) rex_clr_sigs(&gstk_tcb, GSTK_TERM_RSP_TIMER_SIG6);
        gstk_status = gstk_build_timer_expired_cmd(GSTK_TR_TIMER6,
                                                   GSTK_TERM_RSP_TIMER_SIG6);
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("Err in proc term rsp timer6 Sig", 0, 0, 0);
        }
      }
      else if ( gstk_sigs & GSTK_TERM_RSP_TIMER_SIG7 ) {
        MSG_LOW("GOT GSTK_TERM_RSP_TIMER_SIG7",0,0,0);
        /* clear the signal */
        (void) rex_clr_sigs(&gstk_tcb, GSTK_TERM_RSP_TIMER_SIG7);
        gstk_status = gstk_build_timer_expired_cmd(GSTK_TR_TIMER7,
                                                   GSTK_TERM_RSP_TIMER_SIG7);
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("Err in proc term rsp timer7 Sig", 0, 0, 0);
        }
      }
      else if ( gstk_sigs & GSTK_TERM_RSP_TIMER_SIG8 ) {
        MSG_LOW("GOT GSTK_TERM_RSP_TIMER_SIG8",0,0,0);
        /* clear the signal */
        (void) rex_clr_sigs(&gstk_tcb, GSTK_TERM_RSP_TIMER_SIG8);
        gstk_status = gstk_build_timer_expired_cmd(GSTK_TR_TIMER8,
                                                   GSTK_TERM_RSP_TIMER_SIG8);
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("Err in proc term rsp timer8 Sig", 0, 0, 0);
        }
      }
      else if ( gstk_sigs & GSTK_RETRY_ENVELOPE_SIG ) {
        uint8 index;
        MSG_LOW("GOT GSTK_RETRY_ENVELOPE_SIG",0,0,0);
        /* clear the signal */
        (void) rex_clr_sigs(&gstk_tcb, GSTK_RETRY_ENVELOPE_SIG);
        index = gstk_util_find_first_timer_expire_command();
        if (index == GSTK_MAX_TIMER_SUPPORTED) {
          MSG_HIGH("Invalide Timer Table index", 0, 0, 0);
        }
        else
        {
          gstk_build_timer_expire_envelope(index);
          if(gstk_status != GSTK_SUCCESS) {
            MSG_ERROR("Err in proc retry envelope Sig", 0, 0, 0);
          }
        }
      }
      else if ( gstk_sigs & GSTK_SEND_POSTPONE_ENVELOPE_SIG ) {
        uint8 index;
        MSG_LOW("GOT GSTK_SEND_POSTPONE_ENVELOPE_SIG",0,0,0);
        /* clear the signal */
        (void) rex_clr_sigs(&gstk_tcb, GSTK_SEND_POSTPONE_ENVELOPE_SIG);
        index = gstk_util_find_first_non_timer_expire_postpone_env_command();
        if (index == GSTK_MAX_PROACTIVE_COMMAND) {
          MSG_HIGH("Invalide Envelope Index", 0, 0, 0);
        }
        else
        {
          gstk_status = gstk_build_postpone_envelope(index);
          if(gstk_status != GSTK_SUCCESS) {
            MSG_ERROR("Err in proc send postpone envelope Sig", 0, 0, 0);
          }
        }
      }
      else if ( gstk_sigs & GSTK_TASK_CMD_Q_SIG ) { /* other non timer sig */
        /* clear the signal */
        (void) rex_clr_sigs(&gstk_tcb, GSTK_TASK_CMD_Q_SIG);
        while(q_cnt( &gstk_task_cmd_q ) != 0) {
#ifdef FEATURE_ENHANCED_NW_SELECTION
          if(!gstk_continue_task)
          {
            /* the flag is set to FALSE, ignore all commands... */
            break;
          }
#endif /* FEATURE_ENHANCED_NW_SELECTION */
          /* This variable is used to check if there are commands other than BIP envelopes
              in gstk queue when q_linear_delete is called because BIP envelope cannot be processed */
          gstk_is_cmd_processed_qld = FALSE;

          MSG_HIGH("GSTK env state %d ", gstk_curr_envelope_info.gstk_env_state, 0, 0);
          /* Check if Event Download Envelope commands have to be skipped */
          if(gstk_curr_envelope_info.gstk_env_state != GSTK_SUB_ST_PENDING_EVT_DL_NONE)
          {
            MSG_HIGH(" Skip Event Download Envelope commands ",0,0,0);
            /* Since an evt dl is still being retried, process only proactive commands, TRs and
               non evt dl envelope commands. Pull out these selected items using q_linear_delete() */
            q_linear_delete(&gstk_task_cmd_q,
                            gstk_compare_specific_cmds,
                            &gstk_is_cmd_processed_qld,
                            gstk_process_specific_cmds);
            if(gstk_is_cmd_processed_qld == FALSE)
            {
              /* No cmd was dequeued possibly because all are event downloads */
              break; /* break out of the while loop */
            }
          }/* if(gstk_curr_envelope_info.gstk_env_state != GSTK_SUB_ST_PENDING_EVT_DL_NONE) */
          /* Check if BIP envelope commands have to be skipped */
          else if((gstk_is_first_cmd_bip_env()) && (gstk_proactive_cmd_in_progress == TRUE ||
                   gstk_bip_env_in_progress == TRUE))
          {
            MSG_HIGH("Skip BIP Envelope commands, gstk_proactive_cmd_in_progress = 0x%x, gstk_bip_env_in_progress = 0x%x",
                       gstk_proactive_cmd_in_progress,gstk_bip_env_in_progress,0);
            /* BIP Envelope command should not be sent to the card in either of following two cases:
               1> if gstk has allowed UIM to do fetch and a TR for that has not been sent yet
                  (i.e. gstk_proactive_cmd_in_progress is TRUE)
               2> or if gstk has already sent a BIP Envelope command and has not received a response
                  for it yet(i.e.gstk_bip_env_in_progress is TRUE)
               As UIM can not keep track of which toolkit command it is processing and what toolkit command
               it has processed, we are keeping this logic in gstk and thus preventing things from getting
               mixed up in UIM.
               We perform these checks only when gstk is going to process the BIP ENV cmd (i.e. when it is
               in the front of the queue and it is going to be removed from the queue). BIP Env cmd is of
               no harm in any of the above two cases if its just sitting in the queue - but if its going to
               be processed then we need to check the above two conditions and accordingly skip the bip env cmd. */
            q_linear_delete(&gstk_task_cmd_q,
                            gstk_compare_bip_env_cmd,
                            &gstk_is_cmd_processed_qld,
                            gstk_process_specific_cmds);
            if(gstk_is_cmd_processed_qld == FALSE)
            {
              MSG_HIGH(" All commands in queue are BIP Envelope Commands ",0,0,0);
              gstk_requested_mask &= ~GSTK_TASK_CMD_Q_SIG;
              break; /* break out of the while loop since all commands in queue are
                        unprocessable, with a pending TR */
            }
          }/*else if(...)*/
          /* else - process the command without skipping any other command */
          else
          {
            int cnt_cmd =0;
            MSG_HIGH(" Process command without skipping ",0,0,0);
            /* Entering gstk critical section */
            rex_enter_crit_sect(&gstk_crit_sect);
            /* get command from the gstk command queue */
            task_cmd_ptr = (gstk_task_cmd_type*) q_get(&gstk_task_cmd_q);
            /* update gstk_bip_env_in_progress if the command is BIP Envelope */
            if(task_cmd_ptr != NULL)
            {
              if(task_cmd_ptr->cmd.general_cmd.message_header.command_group == GSTK_ENVELOPE_CMD)
              {
                if(task_cmd_ptr->cmd.general_cmd.message_header.command_id == (uint32)GSTK_DATA_AVAIL_EVT_IND ||
                   task_cmd_ptr->cmd.general_cmd.message_header.command_id == (uint32)GSTK_CH_STATUS_EVT_IND)
                {
                  MSG_HIGH(" Processing BIP Envelope command ",0,0,0);
                  gstk_bip_env_in_progress = TRUE;
                }
              }
              rex_leave_crit_sect(&gstk_crit_sect);
              /* Leaving gstk critical section */
              gstk_status = gstk_process_state(&(task_cmd_ptr->cmd));
              if(gstk_status != GSTK_SUCCESS)
              {
                MSG_ERROR("Err in processing Task Cmd Sig", 0, 0, 0);
              }
              if(task_cmd_ptr->cmd_hdr.done_q_ptr)
              {
                (void)q_link(task_cmd_ptr, &task_cmd_ptr->cmd_hdr.link);
                /* put it back on free queue */
                q_put(task_cmd_ptr->cmd_hdr.done_q_ptr, &(task_cmd_ptr->cmd_hdr.link));
              }
            }
            else
            {
              rex_leave_crit_sect(&gstk_crit_sect);
              /* Leaving gstk critical section */
            }
            cnt_cmd = q_cnt( &gstk_task_cmd_q );
            if((cnt_cmd == 0) && (gstk_uim_fetch_pending == TRUE))
            {
              /* queue status command */
              MSG_HIGH(" Sending Status command as fetch is pending and no command left in gstk queue ",0,0,0);
              mmgsdi_status = mmgsdi_send_card_status(gstk_mmgsdi_client_id,
                                                      MMGSDI_SLOT_1,
                                                      MMGSDI_STATUS_APP_NO_INDICATION,
                                                      MMGSDI_STATUS_NO_DATA,
                                                      gstk_mmgsdi_cmd_cb,
                                                      0x00);
              if(mmgsdi_status != MMGSDI_SUCCESS)
              {
                MSG_ERROR("Error in sending status command, mmgsdi_status = 0x%x",mmgsdi_status,0,0);
              }
            }
          }/* else(process the command without skipping any other command) */
        }/* while(q_cnt( &gstk_task_cmd_q ) != 0) */
      }/*  else if ( gstk_sigs & GSTK_TASK_CMD_Q_SIG ) */
#ifdef FEATURE_DCTSTK_SUPPORT
#error code not present
#endif /*FEATURE_DCTSTK_SUPPORT */
#ifdef FEATURE_ESTK
#error code not present
#endif /*FEATURE_ESTK */
#ifdef FEATURE_GSTK_STATE_TESTER
    else if ( gstk_sigs & GSTK_STATE_TESTER_TIMER_SIG) {
      MSG_LOW("GOT GSTK_STATE_TESTER_TIMER_SIG", 0, 0, 0);
      /* clear signal */
      (void) rex_clr_sigs(&gstk_tcb, GSTK_STATE_TESTER_TIMER_SIG);
      gstk_start_state_test();
    }
    else if ( gstk_sigs & GSTK_STATE_TESTER_NEXT_TEST_TIMER_SIG) {
      MSG_LOW("GOT GSTK_STATE_TESTER_NEXT_TEST_TIMER_SIG", 0, 0, 0);
      /* clear signal */
      (void) rex_clr_sigs(&gstk_tcb, GSTK_STATE_TESTER_NEXT_TEST_TIMER_SIG);
      gstk_start_next_state_test();
    }
#endif /*FEATURE_GSTK_STATE_TESTER */
  }
} /* gstk_task */

/*lint -e715 "param_ptr not referenced " */
/*===========================================================================
FUNCTION GSTK_COMPARE_SPECIFIC_CMDS

DESCRIPTION
  GSTK Action function callback that is called by q_linear_delete when the
  compare callback function returns TRUE. This function will return TRUE only
  when the command id is that of a Terminal Response, Proactive Command, an
  Envelope response or Envelope for a non event download command.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  no use as of now. This will be populated as NULL
                      by q_linear_delete()

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if there is a match
  FALSE - if there is no match

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
int gstk_compare_specific_cmds(
   void *item_ptr,
   void *param_ptr
)
{
  gstk_task_cmd_type *task_cmd_ptr  = NULL;
  if(item_ptr == NULL)
  {
    MSG_HIGH("item_ptr is NULL", 0, 0, 0);
    return 0;
  }
  if(param_ptr == NULL)
  {
    /* Checking if param_ptr is NULL or not, though its not used here */
    MSG_HIGH("param_ptr is NULL 0x%x", param_ptr, 0, 0);
  }
  task_cmd_ptr = (gstk_task_cmd_type*)item_ptr;

  if(task_cmd_ptr->cmd.general_cmd.message_header.command_group == GSTK_ENVELOPE_CMD)
  {
    switch(task_cmd_ptr->cmd.general_cmd.message_header.command_id)
    {
    case GSTK_MT_CALL_EVT_IND:
    case GSTK_CALL_CONNECTED_EVT_IND:
    case GSTK_CALL_DISCONNECTED_EVT_IND:
    case GSTK_IDLE_SCRN_EVT_IND:
    case GSTK_LOCATION_STATUS_EVT_IND:
    case GSTK_USER_ACTIVITY_EVT_IND:
    case GSTK_LANG_SELECTION_EVT_IND:
    case GSTK_BROWSER_TERMINATION_EVT_IND:
    case GSTK_DATA_AVAIL_EVT_IND:
    case GSTK_CH_STATUS_EVT_IND:
    case GSTK_ACCESS_TECH_EVT_IND:
    case GSTK_NW_SEARCH_MODE_EVT_IND:
      return 0;
    default:
      return 1;
    }/* switch */
  }/*  if(cmd_p->general_cmd.message_header.command_group == GSTK_ENVELOPE_CMD) */
  return 1;
} /* gstk_compare_specific_cmds */
/*lint +e715 "param_ptr not referenced " */

/*===========================================================================
FUNCTION gstk_compare_bip_env_cmd

DESCRIPTION
  GSTK Action function callback that is called by q_linear_delete when the
  compare callback function returns TRUE. This function will return FALSE only
  for BIP Envelope commands.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  no use as of now. This will be populated as NULL
                      by q_linear_delete()

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if there is a match
  FALSE - if there is no match

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
int gstk_compare_bip_env_cmd(
   void *item_ptr,
   void *param_ptr
)
{
  gstk_task_cmd_type *task_cmd_ptr  = NULL;
  if(item_ptr == NULL)
  {
    MSG_HIGH("item_ptr is NULL", 0, 0, 0);
    return 0;
  }
  if(param_ptr == NULL)
  {
    /* Checking if param_ptr is NULL or not, though its not used here */
    MSG_HIGH("param_ptr is NULL 0x%x", param_ptr, 0, 0);
  }
  task_cmd_ptr = (gstk_task_cmd_type*)item_ptr;

  if(task_cmd_ptr->cmd.general_cmd.message_header.command_group == GSTK_ENVELOPE_CMD)
  {
    switch(task_cmd_ptr->cmd.general_cmd.message_header.command_id)
    {
      case GSTK_DATA_AVAIL_EVT_IND:
      case GSTK_CH_STATUS_EVT_IND:
        return 0;
      default:
        return 1;
    }/* switch */
  }/*  if(cmd_p->general_cmd.message_header.command_group == GSTK_ENVELOPE_CMD) */
  return 1;
} /* gstk_compare_bip_env_cmd */

/*lint -e715 "param_ptr not referenced " */
/*===========================================================================
FUNCTION GSTK_PROCESS_SPECIFIC_CMDS

DESCRIPTION
  GSTK Action function callback that is called by q_linear_delete when the
  compare callback function returns TRUE. This function copies the

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  no use as of now. This will be populated as NULL
                      by q_linear_delete()

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_specific_cmds(
   void *item_ptr,
   void *param_ptr
)
{
  gstk_status_enum_type  gstk_status = GSTK_SUCCESS;
  gstk_task_cmd_type *task_cmd_ptr   = NULL;
  if(item_ptr == NULL)
  {
    MSG_HIGH("item_ptr is NULL", 0, 0, 0);
    return;
  }
  if(param_ptr == NULL)
  {
    MSG_HIGH("param_ptr is NULL", 0, 0, 0);
  }
  else
  {
    *((byte*)param_ptr) = TRUE;
  }
  task_cmd_ptr   = (gstk_task_cmd_type*)item_ptr;

  gstk_status = gstk_process_state(&(task_cmd_ptr->cmd));
  if(gstk_status != GSTK_SUCCESS)
  {
    MSG_ERROR("Err in processing Task Cmd Sig", 0, 0, 0);
  }
  if (task_cmd_ptr->cmd_hdr.done_q_ptr)
  {
    (void)q_link(task_cmd_ptr, &task_cmd_ptr->cmd_hdr.link);
    /* put it back on free queue */
    q_put(task_cmd_ptr->cmd_hdr.done_q_ptr, &(task_cmd_ptr->cmd_hdr.link));
  }
} /* gstk_process_specific_cmds */
/*lint +e715 "param_ptr not referenced " */

/*===========================================================================
FUNCTION gstk_task_get_cmd_buf

DESCRIPTION
  This function get the gstk_task_cmd_type from the GSTK free queue.
  Calling client SHOULD NOT change information on the gstk_task_cmd_type's
  header.

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_task_cmd_type pointer

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
gstk_task_cmd_type *gstk_task_get_cmd_buf ( void  )
{
  gstk_task_cmd_type * cmd_p;
  /* Allocate command buffer */
  cmd_p = (gstk_task_cmd_type*) q_get( &gstk_task_free_q );
  if(cmd_p != NULL) {
    memset(&cmd_p->cmd, 0x00, sizeof(gstk_cmd_type));
  }
  return cmd_p;
} /* gstk_task_cmd_type */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */

/*===========================================================================
FUNCTION GSTK_TASK_PUT_CMD_BUF

DESCRIPTION
  This function put the command onto gstk task command queue

PARAMETER
  cmd_ptr [Input]: Command pointer to be put on the gstk queue

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
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void gstk_task_put_cmd_buf (
  gstk_task_cmd_type *cmd_ptr
)
{
  /* Check for Null pointer */
  if (cmd_ptr != NULL) {

    (void)q_link(cmd_ptr, &cmd_ptr->cmd_hdr.link);

    /* Put the message on the queue */
    q_put(&gstk_task_cmd_q, &cmd_ptr->cmd_hdr.link);

    /* Mask of signals interested in */
    gstk_requested_mask |= GSTK_TASK_CMD_Q_SIG;

    /* Set the command queue signal for */
    /* receive task                     */
    (void) rex_set_sigs(&gstk_tcb, GSTK_TASK_CMD_Q_SIG);

    MSG_LOW("set the signal for putting task cmd", 0, 0, 0);
  } else {
    ERR("put cmd buf NULL cmd_ptr",0,0,0);
  }
} /* gstk_task_put_cmd_buf */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */

/*===========================================================================
FUNCTION GSTK_TASK_PUT_CMD_BUF_TO_HEAD

DESCRIPTION
  This function puts the command at the front of the gstk task command queue

PARAMETER
  cmd_ptr [Input]: Command pointer to be put on the gstk queue

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
void gstk_task_put_cmd_buf_to_head (
  gstk_task_cmd_type *cmd_ptr
)
{
  /* Check for Null pointer */
  if (cmd_ptr == NULL)
  {
    MSG_ERROR("put cmd buf NULL cmd_ptr",0,0,0);
    return;
  }
  /* init link */
  ( void )q_link( cmd_ptr, &cmd_ptr->cmd_hdr.link );

  if ( q_cnt(&gstk_task_cmd_q) == 0 )
  {
    q_put( &gstk_task_cmd_q, & cmd_ptr->cmd_hdr.link );
  }
  else
  {
#ifdef FEATURE_Q_NO_SELF_QPTR
    q_insert(&gstk_task_cmd_q,
             &cmd_ptr->cmd_hdr.link,
             &((gstk_task_cmd_type*) q_check(&gstk_task_cmd_q))->cmd_hdr.link );
#else
    q_insert(&cmd_ptr->hdr.cmd_hdr.link,
             &((gstk_task_cmd_type*) q_check(&gstk_task_cmd_q))->cmd_hdr.link );
#endif /* FEATURE_Q_NO_SELF_QPTR */
  }

  /* Mask of signals interested in */
  gstk_requested_mask |= GSTK_TASK_CMD_Q_SIG;

  /* Set the command queue signal for */
  /* receive task                     */
  (void) rex_set_sigs(&gstk_tcb, GSTK_TASK_CMD_Q_SIG);
  MSG_LOW("set the signal for putting task cmd", 0, 0, 0);
} /* gstk_task_put_cmd_buf_to_front_of_q */

/*===========================================================================
FUNCTION GSTK_CMD

DESCRIPTION
  This function allows clients to put the gstk_cmd onto the GSTK command
  queue.

PARAMETER
  gstk_cmd [Input]:  Pointer of the task_cmd_type

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
void gstk_cmd (
  gstk_task_cmd_type *gstk_task_cmd
)
{
  if (gstk_task_cmd != NULL) {
    /* put command back to command queue */
    gstk_task_put_cmd_buf(gstk_task_cmd);
  }
  else {
    ERR("NULL cmd_ptr",0,0,0);
  }
} /* gstk_cmd */

/*===========================================================================
FUNCTION gstk_build_timer_expired_cmd

DESCRIPTION

  This function build the timer expiration command based on the passed in
  timer type and put onto the command queue

PARAMETERS
  timer_type - [Input] gstk_timer_enum_type, use to build the command
  timer_sig  - [Input] The rex signal corresponding to the timer

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
gstk_status_enum_type gstk_build_timer_expired_cmd(
  gstk_timer_enum_type  gstk_timer_type,
  rex_sigs_type         timer_sig
)
{
  gstk_status_enum_type gstk_status   = GSTK_SUCCESS;
  gstk_task_cmd_type    *task_cmd_ptr = NULL;
  uint32                ref_id        = 0;

  if (gstk_timer_type >= GSTK_TR_TIMER_INVALID)
  {
    MSG_ERROR("Invalid GSTK timer id 0x%x!", gstk_timer_type, 0, 0);
    return GSTK_BAD_PARAM;
  }

  /* get the command from buffer */
  task_cmd_ptr = gstk_task_get_cmd_buf();
  if (task_cmd_ptr != NULL) {
    /* find corresponding entry in command_details_ref_table */
    for (; ref_id < GSTK_MAX_PROACTIVE_COMMAND; ++ ref_id)
    {
      if (command_details_ref_table[ref_id].tp_rpt_time.sigs == timer_sig)
      {
        break;
      }
    }

    /* Build the gstk_cmd */
    task_cmd_ptr->cmd.timer_msg.message_header.command_group = GSTK_TIMER_EXP_CMD;
    task_cmd_ptr->cmd.timer_msg.message_header.command_id = (int)gstk_timer_type;
    task_cmd_ptr->cmd.timer_msg.message_header.user_data = ref_id;

    /* put command back to command queue */
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
  else {
    MSG_ERROR("Failed to allocate memory - gstk_build_timer_expired_cmd()!", 0, 0, 0);
    gstk_status = GSTK_MEMORY_ERROR;
  }

  return gstk_status;

} /* gstk_build_timer_expired_cmd */

/*===========================================================================
FUNCTION gstk_power_down

DESCRIPTION

  This function
  - flushes GSTK queue
  - vote UIM off
  - Ack TMC

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
gstk_status_enum_type gstk_power_down ()
{
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;

  MSG_MED("GSTK powering down",0,0,0);

  /* flush GSTK queue */
  /* more code */

  /* vote UIM off */
  (void) uim_power_control(UIM_USIM_SESSION, FALSE);

  return gstk_status;
} /* gstk_power_down */

/*===========================================================================
FUNCTION gstk_set_additional_info

DESCRIPTION
  Sets gstk_additional_info_type->additional_info to field.

PARAMETERS
  rec: The record to set the field in.
  out: The field used to set the record with.

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
void gstk_set_additional_info
(
  gstk_additional_info_type*          rec,
  gstk_additional_info_code_enum_type field
)
{
  if(rec != NULL)
  {
    rec->length = 1;
    rec->additional_info[0] = (uint8)field;
  }
} /* gstk_set_additional_info */

/*===========================================================================
FUNCTION gstk_set_command_id

DESCRIPTION
  Sets the command id.

PARAMETER
  header: The header to set the command_id for.
  command_id: The value to set the command_id to.

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
void gstk_set_command_id(gstk_hdr_type* header,
                         gstk_cmd_enum_type command_id)
{
  if(header != NULL)
  {
    header->command_id = (int)command_id;
  }
} /* gstk_set_command_id */

#ifdef FEATURE_ENHANCED_NW_SELECTION
/*===========================================================================
FUNCTION gstk_gsdi_continue_task

DESCRIPTION
  Sets the flags

PARAMETER
  val indeicates whether the task should continue processing commands

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  If the flag is set to FALSE, GSTK will not process any commands.

SEE ALSO
  None
===========================================================================*/
void gstk_gsdi_continue_task(boolean val)
{
  gstk_continue_task = val;
  if (gstk_continue_task && (q_cnt( &gstk_task_cmd_q ) != 0))
  {
    /* set signal to let GSTK start processing cmd if any in the queue */
    (void) rex_set_sigs(&gstk_tcb, GSTK_TASK_CMD_Q_SIG);
  }
} /* gstk_gsdi_continue_task */
#endif /* FEATURE_ENHANCED_NW_SELECTION */

#ifdef FEATURE_STOP_ENV_CMD_PROCESSING
/*===========================================================================
FUNCTION   gstk_check_for_pending_pro_cmd

DESCRIPTION
  This function builds the envelope that has been postponed due to
    terminal response that is required to be sent down first

PARAMETERS
  none

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
boolean gstk_check_for_pending_pro_cmd(void)
{
  int i = 0;
  for(i=0;i<GSTK_MAX_PROACTIVE_COMMAND;i++)
  {
    if (command_details_ref_table[i].free_slot == FALSE)
    {
      return TRUE;
    }
  }
  return FALSE;
} /* gstk_check_for_pending_pro_cmd */
#endif /* FEATURE_STOP_ENV_CMD_PROCESSING */


#ifdef FEATURE_STOP_SMS_PP_ON_PENDING_SMS_TR
/*===========================================================================
FUNCTION   gstk_check_for_pending_send_sms_term_resp

DESCRIPTION
  This function builds the envelope that has been postponed due to
    terminal response that is required to be sent down first

PARAMETERS
  none

DEPENDENCIES

RETURN VALUE
  boolean TRUE  - if a send sms terminal response is pending
          FALSE - if send sms terminal response is not pending

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_check_for_pending_send_sms_term_resp(void)
{
  int i = 0;

  MSG_HIGH("gstk_check_for_pending_send_sms_term_resp ()",0,0,0);

  for(i=0;i<GSTK_MAX_PROACTIVE_COMMAND;i++)
  {
    if (command_details_ref_table[i].free_slot == FALSE &&
        command_details_ref_table[i].expected_cmd_rsp_type == GSTK_SEND_SMS_CNF)
    {
      MSG_HIGH("found a match, returning TRUE",0,0,0);
      return TRUE;
    }
  }
  return FALSE;

} /* gstk_check_for_pending_send_sms_term_resp */
#endif /* FEATURE_STOP_SMS_PP_ON_PENDING_SMS_TR */

/*===========================================================================
FUNCTION   gstk_cm_access_tech_cb

DESCRIPTION
  Callback function called as immediate response when GSTK sends a request
  for Access Technology to CM as part of the Proactive Command Provide Local
  Info.

PARAMETERS
  void*                 data_block_ptr,
  cm_ss_cmd_e_type      cmd,
  cm_ss_cmd_err_e_type  cmd_err

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
void gstk_cm_access_tech_cb (
  const void*           data_block_ptr,
  cm_ss_cmd_e_type      cmd,
  cm_ss_cmd_err_e_type  cmd_err
)
{
  gstk_task_cmd_type *                  task_cmd;

  /* PARAMETER CHECKS */

  /* Data block pointer should contain the Provide Local Info information */

  MSG_HIGH(" IN gstk_cm_access_tech_cb ",0,0,0);

  if (data_block_ptr == NULL)
  {
    ERR("Null Data Block ptr fr CM", 0, 0, 0);
    return;
  }

  if(cmd != CM_SS_CMD_INFO_GET)
  {
    ERR("Invalid Cmd from CM", 0, 0, 0);
    return;
  }

  if(cmd_err == CM_SS_CMD_ERR_NOERR)
  {
    MSG_HIGH("Expecting event CM_SS_EVENT_INFO next..",0,0,0);
    return;
  }

  /* If cmd_err reports an error, it indicates that ME is unable to process cmd */

  /* build a terminal response and put it in the gstk
     queue to be sent down to the card
  */

  /* get the command from buffer */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd == NULL)
  {
    ERR("GSTK_MEMORY_ERROR", 0, 0, 0);
    return;
  }

  MSG_HIGH("Building error terminal response...",0,0,0);
  /* Build the gstk_cmd */
  task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.command_id    = (uint32) GSTK_PROVIDE_ACCESS_TECH_CNF;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type     = GSTK_ACCESS_TECHNOLOGY_INFO;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.user_data     = 0;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.cmd_details_ref_id           =
      *((uint32*)data_block_ptr);
  task_cmd->cmd.provide_local_info_term_rsp_cnf.command_number               =
    command_details_ref_table[*((uint32*)data_block_ptr)].command_details.command_number;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.command_result               =
       GSTK_ACCESS_TECH_UNABLE_TO_PROCESS_CMD;
  /* initialize additional info length to zero */
  task_cmd->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = 0;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type = GSTK_INVALID_LOC_INFO;

  /* put command back to command queue */
  gstk_task_put_cmd_buf(task_cmd);
} /* gstk_cm_access_tech_cb */

#if defined(FEATURE_WCDMA) && !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#error code not present
#endif /* FEATURE_WCDMA && !FEATURE_GSTK_FOR_NON_LTK_BUILD */

/*lint -e715 "cell_id_info_p not referenced when FEATURE_GSM is
  not defined" */
#ifndef FEATURE_GSTK_FOR_NON_LTK_BUILD
/*===========================================================================
FUNCTION gstk_get_rr_cell_id_info

DESCRIPTION
  Function used to query RR for the Cell Id.

PARAMETER
  Input:   const uint8       * mcc_mnc_p      : Current PLMN Information
  Input:   const uint8       *lac_p           : Current LAC  Information
  Input:   gstk_cell_id_type * cell_id_info_p : Structure to populated
  Output:  cell_id_info_p->cell_id            : cell id
  Output:  cell_id_info_p->cell_id_len        : lenght of cell id

DEPENDENCIES
  RR has to be able to provide a valid Cell ID:

RETURN VALUE
  boolean:  TRUE:  Cell ID information is set.  It may be default data
            FALSE: Cell ID information is not check.  An error occurred.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_get_rr_cell_id_info(
  const uint8             * mcc_mnc_p,
  const uint8             * lac_p,
  gstk_cell_id_type * cell_id_info_p
)
{
#ifdef FEATURE_GSM
#error code not present
#else
  MSG_ERROR(" FEATURE_GSM not defined ",0,0,0);
  return FALSE;
#endif /* FEATURE_GSM */

}/*lint !e818 "cell_id_info_p not const . This lint error is suppressed
  when FEATURE_GSM is not defined." */
/*lint +e715 "cell_id_info_p not referenced when FEATURE_GSM is not defined */
/* gstk_get_rr_cell_id_info */

/*lint -e715 "cell_id_info_p mcc_mnc_p lac_p not referenced when
  FEATURE_GSM is not defined" */
/*===========================================================================
FUNCTION gstk_get_rrc_cell_id_info

DESCRIPTION
  Function used to query RRC Table information populated by RRC.

PARAMETER
  Input:   uint8  * mcc_mnc_p                 : Current PLMN Information
  Input:   uint8  * lac_p                     : Current LAC  Information
  Input:   gstk_cell_id_type * cell_id_info_p : Structure to populated
  Output:  cell_id_info_p->cell_id            : cell id
  Output:  cell_id_info_p->cell_id_len        : lenght of cell id

DEPENDENCIES
  RRC would have populated the RRC Loci Table

RETURN VALUE
  boolean:  TRUE:  Cell ID information is set.  It may be default data
            FALSE: Cell ID information is not check.  An error occurred.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_get_rrc_cell_id_info (
  const uint8             * mcc_mnc_p,
  const uint8             * lac_p,
  gstk_cell_id_type       * cell_id_info_p
)
{
#ifdef FEATURE_WCDMA
#error code not present
#else
  return FALSE;
#endif /* FEATURE_WCDMA */

}/*lint !e818 "cell_id_info_p mcc_mnc_p lac_p not const . This lint error
  is suppressed when FEATURE_GSM is not defined." */
/*lint +e715 supress "cell_id_info_p mcc_mnc_p lac_p not referenced when
  FEATURE_GSM not defined" */
/* gstk_get_rrc_cell_id_info */

/*===========================================================================
FUNCTION gstk_get_curr_cell_id_info

DESCRIPTION
  Function called to retrieve the Cell ID information based on the RAT
  and current PLMN Information.

PARAMETER
  Input:   sys_radio_access_type_e_type radio_access_tech : Current RAT
  Input:   uint8  * mcc_mnc_p                 : Current PLMN Information
  Input:   uint8  * lac_p                     : Current LAC  Information
  Input:   gstk_cell_id_type * cell_id_info_p : Structure to populated
  Output:  cell_id_info_p->cell_id            : cell id
  Output:  cell_id_info_p->cell_id_len        : lenght of cell id

DEPENDENCIES
  TODO

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_get_curr_cell_id_info(
  sys_radio_access_tech_e_type radio_access_tech,
  const uint8       *          mcc_mnc,
  const uint8       *          lac,
  gstk_cell_id_type *          cell_id_info_p
)
{
  boolean status             = FALSE;

  /* ---------------------------------------------------------------------------
     Validate that Pointer
     -------------------------------------------------------------------------*/
  if ( cell_id_info_p == NULL ||
       mcc_mnc        == NULL ||
       lac            == NULL )
  {
    MSG_ERROR("NULL POINTER",0,0,0);
    return FALSE;
  }

  /* ---------------------------------------------------------------------------
     Initailize the cell id information
     -------------------------------------------------------------------------*/
  memset(cell_id_info_p->cell_id,0xFF,GSTK_MAX_CELL_ID_LEN);
  cell_id_info_p->cell_len = GSTK_MAX_CELL_ID_LEN;

  /* --------------------------------------------------------------------------
     Validate the Radio Access Technology Information.  Simply for
     checking the RAT.
     ------------------------------------------------------------------------*/
  switch ( radio_access_tech )
  {
    case SYS_RAT_GSM_RADIO_ACCESS:
    case SYS_RAT_UMTS_RADIO_ACCESS:
      break;

    case  SYS_RAT_NONE:
      MSG_ERROR("SYS_RAT_NONE:  Unhanlded RAT:  Return False",0,0,0);
      return FALSE;

    case SYS_RAT_MAX:
      MSG_ERROR("SYS_RAT_MAX:  Unhandled RAT: Return False",0,0,0);
      return FALSE;

    default:
      MSG_ERROR("0x%x: Unhandled RAT:  Return False",radio_access_tech,0,0);
      return FALSE;
  }

  /* --------------------------------------------------------------------------
     Now populate the Cell Information
     ------------------------------------------------------------------------*/
  if ( radio_access_tech == SYS_RAT_GSM_RADIO_ACCESS )
  {
    status = gstk_get_rr_cell_id_info(mcc_mnc, lac, cell_id_info_p);
  }
  else
  {
    status = gstk_get_rrc_cell_id_info(mcc_mnc,
                                       lac,
                                       cell_id_info_p);
  }
  return status;
} /* gstk_get_curr_cell_id_info */
#endif /* FEATURE_GSTK_FOR_NON_LTK_BUILD */

/*===========================================================================
FUNCTION GSTK_GET_NV

DESCRIPTION
  Get an item from the nonvolatile memory.

RETURN VALUE
  gstk_status_enum_type
    GSTK_SUCCESS : if NV_DONE
    GSTK_ERROR   : otherwise.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done.
===========================================================================*/
gstk_status_enum_type gstk_get_nv(
  nv_items_enum_type item,
  nv_item_type *data_ptr
)
{
  nv_cmd_type gstk_nv_cmd;           /* nv command buffer */
  MSG_HIGH(" -- In GSTK_GET_NV",0,0,0);

  if(data_ptr == NULL)
  {
    MSG_HIGH("ERR: data_ptr is NULL!!!",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /*
  ** notify this task when done
  */
  gstk_nv_cmd.tcb_ptr = rex_self();
  gstk_nv_cmd.sigs = GSTK_NV_SIG;  /* TODO - to be defined */

  /*
  ** command goes on no queue when done
  */
  gstk_nv_cmd.done_q_ptr = NULL;

  /*
  ** item to read
  */
  gstk_nv_cmd.item = item;
  gstk_nv_cmd.cmd = NV_READ_F;

  /*
  ** Set the Data Ptr for NV ITEM TO read
  */
   gstk_nv_cmd.data_ptr = data_ptr;

  /*
  ** Clear REX Signals
  */
  (void) rex_clr_sigs( rex_self(), GSTK_NV_SIG );

  /*
  ** Send Command to NV
  */
  nv_cmd( &gstk_nv_cmd );

  /*
  ** Wait for Signal from NV that it's complete
  */
  (void)gstk_wait(GSTK_NV_SIG);

  /*
  ** Clear Signal
  */
  (void) rex_clr_sigs( rex_self(), GSTK_NV_SIG );

  MSG_MED(" -- NV STATUS = %x",gstk_nv_cmd.status,0,0);

  if ( gstk_nv_cmd.status != NV_DONE_S )
  {
    MSG_ERROR(" NV GET FAILED ",0,0,0);
    return GSTK_ERROR;
  }
  return GSTK_SUCCESS;
} /* gstk_get_nv */

#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
/*===========================================================================
FUNCTION gstk_cache_1x_location_info

DESCRIPTION
  Caches 1x location information

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void gstk_cache_1x_location_info(gstk_cdma_location_info_type loc_info)
{
  /* Memset the cache and copy individual elements to
     avoid issues due to structure padding
  */
  memset(&cdma_location_info_cache, 0x00, sizeof(cdma_location_info_cache));

  cdma_location_info_cache.is_loc_info_valid = TRUE;

  /* SID info */
  memcpy(cdma_location_info_cache.loc_info.sid_info.sid,
         loc_info.sid_info.sid,loc_info.sid_info.id_len);
  cdma_location_info_cache.loc_info.sid_info.id_len
    = loc_info.sid_info.id_len;

  /* NID info */
  memcpy(cdma_location_info_cache.loc_info.nid_info.nid,
         loc_info.nid_info.nid,loc_info.nid_info.id_len);
  cdma_location_info_cache.loc_info.nid_info.id_len
    = loc_info.nid_info.id_len;

  /* MCC info */
  memcpy(cdma_location_info_cache.loc_info.mcc,
         loc_info.mcc, GSTK_MCC_LEN);

  /* IMSI_11_12 */
  cdma_location_info_cache.loc_info.imsi_11_12
    = loc_info.imsi_11_12;

  /* BASE_ID info */
  memcpy(cdma_location_info_cache.loc_info.base_id_info.base_id,
         loc_info.base_id_info.base_id,loc_info.base_id_info.id_len);
  cdma_location_info_cache.loc_info.base_id_info.id_len
    = loc_info.base_id_info.id_len;

  /* BASE Latitude */
  memcpy(cdma_location_info_cache.loc_info.base_lat,
         loc_info.base_lat, GSTK_BASE_LAT_LEN);

  /* BASE Longitude */
  memcpy(cdma_location_info_cache.loc_info.base_long,
         loc_info.base_lat, GSTK_BASE_LONG_LEN);
} /* gstk_cache_1x_location_info */

/*===========================================================================
FUNCTION gstk_copy_1x_info_from_cm

DESCRIPTION
  Copies 1x related system information from the cm ss info ptr to
  the gstk task cmd ptr.

RETURN VALUE
  SUCCESS
  FAIL

DEPENDENCIES
  None
===========================================================================*/
gstk_status_enum_type gstk_copy_1x_info_from_cm(
  gstk_cdma_location_info_type   *cdma_loc_info,
  const cm_mm_ss_info_s_type     *ss_info_ptr
)
{
  /* check for Null ptr */
  if(cdma_loc_info == NULL)
  {
    MSG_ERROR("ERR: cdma_loc_info is NULL ",0,0,0);
    return GSTK_BAD_PARAM;
  }
  if(ss_info_ptr == NULL)
  {
    MSG_ERROR("ERR: ss_info_ptr is NULL ",0,0,0);
    return GSTK_BAD_PARAM;
  }
  /* SID info */
  memcpy(cdma_loc_info->sid_info.sid,
         &(ss_info_ptr->sys_id.id.is95.sid),
         GSTK_MAX_SID_LEN);
  cdma_loc_info->sid_info.id_len = GSTK_MAX_SID_LEN;

  /* NID info */
  memcpy(cdma_loc_info->nid_info.nid,
         &(ss_info_ptr->sys_id.id.is95.nid),
         GSTK_MAX_NID_LEN);
  cdma_loc_info->nid_info.id_len = GSTK_MAX_NID_LEN;

  /* MCC info */
  memcpy(cdma_loc_info->mcc,
         &(ss_info_ptr->sys_id.id.is95.mcc),
         GSTK_MCC_LEN);

  /* IMSI_11_12 */
  cdma_loc_info->imsi_11_12  = ss_info_ptr->sys_id.id.is95.imsi_11_12;

  /* BASE_ID info */
  memcpy(cdma_loc_info->base_id_info.base_id,
         &(ss_info_ptr->mode_info.cdma_info.base_id),
         GSTK_MAX_BASE_ID_LEN);
  cdma_loc_info->base_id_info.id_len = GSTK_MAX_BASE_ID_LEN;

  /* BASE Latitude */
  memcpy(cdma_loc_info->base_lat,
         &(ss_info_ptr->mode_info.cdma_info.base_lat),
         GSTK_BASE_LAT_LEN);

  /* BASE Longitude */
  memcpy(cdma_loc_info->base_long,
         &(ss_info_ptr->mode_info.cdma_info.base_long),
         GSTK_BASE_LONG_LEN);

  return GSTK_SUCCESS;
} /* gstk_copy_1x_info_from_cm */
#endif /* #ifdef FEATURE_CCAT */

/*===========================================================================
FUNCTION   gstk_cm_network_search_mode_event_cb

DESCRIPTION
  Callback function called as immediate response when GSTK sends a request
  for Network Search Mode to CM as part of the Proactive Command Provide Local
  Info.

PARAMETERS
  void*                 data_block_ptr,
  cm_ss_cmd_e_type      cmd,
  cm_ss_cmd_err_e_type  cmd_err

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
void gstk_cm_network_search_mode_event_cb (
  void*                 data_block_ptr,
  cm_ph_cmd_e_type      cmd,
  cm_ph_cmd_err_e_type  cmd_err
)
{
  gstk_task_cmd_type * task_cmd = NULL;

  /* PARAMETER CHECKS */

  /* Data block pointer should contain the Provide Local Info information */

  MSG_HIGH(" IN gstk_cm_network_search_mode_event_cb ",0,0,0);

  if (data_block_ptr == NULL)
  {
    ERR("Null Data Block ptr fr CM", 0, 0, 0);
    return;
  }

  if(cmd != CM_PH_CMD_INFO_GET)
  {
    ERR("Invalid Cmd from CM", 0, 0, 0);
    return;
  }

  if(cmd_err == CM_PH_CMD_ERR_NOERR)
  {
    MSG_HIGH("Expecting event CM_PH_EVENT_INFO next..",0,0,0);
    return;
  }

  /* If cmd_err reports an error, it indicates that ME is unable to process cmd */

  /* build a terminal response and put it in the gstk
     queue to be sent down to the card
  */

  /* get the command from buffer */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd == NULL)
  {
    ERR("GSTK_MEMORY_ERROR", 0, 0, 0);
    return;
  }

  MSG_HIGH("Building error terminal response...",0,0,0);
  /* Build the gstk_cmd */
  task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.command_id    =
    int32touint32((int32)GSTK_PROVIDE_NW_SEARCH_MODE_CNF);
  task_cmd->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type     = GSTK_NETWORK_SEARCH_MODE_INFO;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.user_data     = 0;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.cmd_details_ref_id           =
    *((uint32*)data_block_ptr);
  task_cmd->cmd.provide_local_info_term_rsp_cnf.command_number               =
    command_details_ref_table[*((uint32*)data_block_ptr)].command_details.command_number;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.command_result               =
    GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
  /* initialize additional info length to zero */
  task_cmd->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = 0;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type = GSTK_INVALID_LOC_INFO;

  /* put command back to command queue */
  gstk_task_put_cmd_buf(task_cmd);

}/*lint !e818 "data_block_ptr not const . This lint error is suppressed as
cm_ph_cmd_get_ph_info which gstk calls with this callback does not support a const cb type ." */
/* gstk_cm_network_search_mode_event_cb */

/*===========================================================================
FUNCTION   gstk_cm_ph_event_cb

DESCRIPTION
  Phone (ph) event callback function, called when an ph event is happening.
  This function populates the preferred selection types and
  puts the command in the GSTK command queue.

PARAMETERS
  ph_event: [Input] CM phone event
  ph_info_ptr: [Input] Pointer to the ph info struct

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
void gstk_cm_ph_event_cb (
  cm_ph_event_e_type          ph_event,
  const cm_ph_info_s_type    *ph_info_ptr
)
{
  gstk_task_cmd_type * task_cmd = NULL;

  if (ph_info_ptr == NULL)
  {
    ERR("Null PH info ptr fr CM", 0, 0, 0);
    return;
  }

  MSG_HIGH("gstk_cm_ph_event_cb: event %d", ph_event, 0, 0);

  switch (ph_event) {
  case CM_PH_EVENT_SYS_SEL_PREF:
    task_cmd = gstk_task_get_cmd_buf();
    if(task_cmd == NULL)
    {
      ERR("GSTK_MEMORY_ERROR", 0, 0, 0);
      return;
    }
    task_cmd->cmd.ph_nw_sel_pref_ind.message_header.command_group =
      GSTK_PH_NW_SEL_PREF_CHANGE_IND;
    task_cmd->cmd.ph_nw_sel_pref_ind.message_header.command_id = 0; /* doesn't matter */
    task_cmd->cmd.ph_nw_sel_pref_ind.message_header.user_data = 0;  /* doesn't matter */
    switch(ph_info_ptr->network_sel_mode_pref)
    {
    case CM_NETWORK_SEL_MODE_PREF_AUTOMATIC:
      task_cmd->cmd.ph_nw_sel_pref_ind.ph_nw_sel_pref = GSTK_NW_SEARCH_MODE_AUTOMATIC;
      break;
    case CM_NETWORK_SEL_MODE_PREF_MANUAL:
      task_cmd->cmd.ph_nw_sel_pref_ind.ph_nw_sel_pref = GSTK_NW_SEARCH_MODE_MANUAL;
      break;
    default:
      MSG_HIGH("Unknown Network Selection Mode",0,0,0);
      (void)q_link(task_cmd, &task_cmd->cmd_hdr.link);
      q_put(task_cmd->cmd_hdr.done_q_ptr, &(task_cmd->cmd_hdr.link));
      return;
    }
    /* put command in command queue */
    gstk_task_put_cmd_buf(task_cmd);
    break;

  case CM_PH_EVENT_INFO:
    MSG_HIGH("CM_PH_EVENT_INFO evt from CM..",0,0,0);
    task_cmd = gstk_task_get_cmd_buf();
    if(task_cmd == NULL)
    {
      ERR("GSTK_MEMORY_ERROR", 0, 0, 0);
      return;
    }
    /* Build the gstk_cmd */
    task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
    task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.command_id    = (uint32)GSTK_PROVIDE_NW_SEARCH_MODE_CNF;
    task_cmd->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type     = GSTK_NETWORK_SEARCH_MODE_INFO;
    task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.user_data     = 0;
    task_cmd->cmd.provide_local_info_term_rsp_cnf.command_result               =
         GSTK_COMMAND_PERFORMED_SUCCESSFULLY;

    switch(ph_info_ptr->network_sel_mode_pref)
    {
    case CM_NETWORK_SEL_MODE_PREF_AUTOMATIC:
      task_cmd->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nw_search_mode =
        GSTK_NW_SEARCH_MODE_AUTOMATIC;
      break;

    case CM_NETWORK_SEL_MODE_PREF_MANUAL:
      task_cmd->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nw_search_mode =
        GSTK_NW_SEARCH_MODE_MANUAL;
      break;

    default:
      task_cmd->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nw_search_mode =
        GSTK_NW_SEARCH_MODE_MAX;
      task_cmd->cmd.provide_local_info_term_rsp_cnf.command_result               =
         GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      break;

    }
    /* initialize additional info length to zero */
    MSG_HIGH("Network Search Mode is %d",
              task_cmd->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nw_search_mode,
              0,0);
    task_cmd->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = 0;
    task_cmd->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

    /* put command back to command queue */
    gstk_task_put_cmd_buf(task_cmd);
    break;

  default:
    MSG_LOW("No relevant PH events", 0, 0, 0);
    break;
  }
} /* gstk_cm_ph_event_cb */
#endif /* #ifdef FEATURE_CAT_REL6 */

/*===========================================================================

FUNCTION GSTK_MALLOC

DESCRIPTION
  gstk_malloc calls mem_malloc to request memory from the tmc_heap or tmc_small_heap
  depending on whether or not the request greater than 64KB if
  FEATURE_GSTK_USE_SMALL_HEAP is defined and FEATURE_LOW_MEMORY_USAGE is not.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  a pointer is allocated.
===========================================================================*/
void * gstk_malloc(dword size)
{
  void *ptr = NULL;
#if !defined(FEATURE_LOW_MEMORY_USAGE) && defined(FEATURE_GSTK_USE_SMALL_HEAP)
  if (size <= tmc_get_mem_heap_size(TMC_MEM_HEAP_TYPE_THRD))
  {
    ptr = mem_malloc(&tmc_heap_small, size);
  }
  else
  {
    ptr = mem_malloc(&tmc_heap, size);
  }
#else
  ptr = mem_malloc(&tmc_heap, size);
#endif /* !FEATURE_LOW_MEMORY_USAGE && FEATURE_GSTK_USE_SMALL_HEAP */
  if (ptr != NULL)
  {
    (void)memset(ptr, 0x00, size);
  }
  return ptr;
} /* gstk_malloc */

/* <EJECT> */
/*===========================================================================

FUNCTION GSTK_FREE

DESCRIPTION
  The gstk_free frees the pointer from the regular tmc_heap or the tmc_heap_small
  based on the pointer address, but only if FEATURE_GSTK_USE_SMALL_HEAP is defined
  and FEATURE_LOW_MEMORY_USAGE IS NOT

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  the pointer is freed.
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void gstk_free(void * ptr)
{
  if (ptr == NULL)
  {
    MSG_HIGH("Attempt to free null ptr, return", 0, 0, 0);
    return;
  }
#if !defined(FEATURE_LOW_MEMORY_USAGE) && defined(FEATURE_GSTK_USE_SMALL_HEAP)
  if (((uint8 *)ptr >= (uint8 *) tmc_heap_mem_buffer) &&
      ((uint8 *)ptr < ((uint8 *)tmc_heap_mem_buffer +
      tmc_get_mem_heap_size(TMC_MEM_HEAP_TYPE_BUF))))
  {
    mem_free(&tmc_heap, ptr);
  }
  else
  {
    mem_free(&tmc_heap_small, ptr);
  }
#else
  mem_free(&tmc_heap, ptr);
#endif /* !FEATURE_LOW_MEMORY_USAGE && FEATURE_GSTK_USE_SMALL_HEAP */
  ptr = NULL;
}/* gstk_free */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */

/*===========================================================================

FUNCTION gstk_offset_memcpy

DESCRIPTION
  gstk_offset_memcpy first checks that in an invalid index is not be accessed
  in the destination buffer then calls gstk_offset

PARAMETERS
  dest_ptr:   Pointer to copy data to.
  src_ptr: Pointer to copy data from.
  dest_offset:        offset from where to start copying data to
  copy_size:     size of data to copy
  dest_max_size:      size of destination buffer
  src_max_size: size of source buffer, if 0 then no check on source size done

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  copy_size may be truncated if the offset plus the size to be copied exceeds
  destination buffer maximum size, in which case the dest_max_size will also
  be updated before sending to gstk_memcpy
===========================================================================*/
void  gstk_byte_offset_memcpy(
  byte *dest_ptr,
  const void *src_ptr,
  int dest_offset,
  size_t copy_size,
  size_t dest_max_size,
  size_t src_max_size)
{
 if (dest_ptr == NULL || src_ptr == NULL) {
   MSG_HIGH("gstk_offset_memcpy: NULL pointers passed in, cannot memcpy",0,0,0);
   return;
 }

 /* ensure not to access invalid index in destination buffer */
 if ( (((size_t)dest_offset) + copy_size) > dest_max_size) {
   MSG_ERROR("gstk_offset_memcpy: Trying to copy more than destination Buffer Can Handle",0,0,0);
   copy_size = dest_max_size - ((size_t)dest_offset);
   dest_max_size = copy_size;
 }
 gstk_memcpy(&(dest_ptr[dest_offset]), src_ptr, copy_size, dest_max_size,
             src_max_size);
} /* gstk_byte_offset_memcpy */

/*===========================================================================

FUNCTION gstk_memcpy

DESCRIPTION
  gstk_memcpy first checks if the size requested does not exceed the source
  size and that the size of the data to be copied does not exceed the
  max data size the destination buffer can accomodate before preceeding with
  the memcpy

PARAMETERS
  dest_ptr:   Pointer to copy data to.
  src_ptr: Pointer to copy data from.
  copy_size:     size of data to copy
  dest_max_size:      size of destination buffer
  src_max_size: size of source buffer, if 0 then no check on source size done

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  data is copied to the destination buffer.  the copy may be truncated if the
  size of data to be copied exceeds the size of the source buffer or if the
  size of data to be copied exceeds the size of the destination buffer.
===========================================================================*/
void  gstk_memcpy(
  void *dest_ptr,
  const void *src_ptr,
  size_t copy_size,
  size_t dest_max_size,
  size_t src_max_size)
{
 if (dest_ptr == NULL || src_ptr == NULL) {
   MSG_HIGH("gstk_offset_memcpy: NULL pointers passed in, cannot memcpy",0,0,0);
   return;
 }

 /* ensure not accessing invalid mem location from source buffer */
 if ( copy_size > src_max_size) {
   MSG_ERROR("gstk_offset_memcpy: Trying to access data greater than size of source buffer",
             0,0,0);
   copy_size = src_max_size;
 }

 /* ensure not to overshoot destination buffer */
 if (copy_size > dest_max_size) {
   MSG_ERROR("gstk_offset_memcpy: Trying to copy data greater than size of destination buffer",
            0,0,0);
   copy_size = dest_max_size;
 }
 memcpy(dest_ptr, src_ptr, copy_size);
} /* gstk_memcpy */

/*===========================================================================

FUNCTION gstk_get_and_store_curr_app_type

DESCRIPTION
  This function stores the current app type obtained from MMGSDI
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void gstk_get_and_store_curr_app_type(void)
{
  gsdi_protocol_enum_type protocol_type = GSDI_PROTOCOL_NONE;
  gsdi_returns_T          gsdi_status   = GSDI_SUCCESS;

  gsdi_status = gsdi_get_protocol(GSDI_SLOT_1,&protocol_type);
  if(gsdi_status != GSDI_SUCCESS)
  {
    MSG_ERROR("gsdi_get_protocol FAILED return status 0x%x",gsdi_status,0,0);
    return;
  }

  switch(protocol_type)
  {
  case GSDI_PROTOCOL_UICC:
    gstk_curr_app_type = (gstk_app_enum_type)GSTK_APP_USIM;
    break;
  case GSDI_PROTOCOL_GSM:
    gstk_curr_app_type = (gstk_app_enum_type)GSTK_APP_SIM;
    break;
  default:
    MSG_HIGH("Unhandled protocol type 0x%x",protocol_type,0,0);
    break;
  }
  MSG_MED("gstk_curr_app_type is 0x%x",gstk_curr_app_type,0,0);
}/* gstk_get_and_store_curr_app_type */

#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

#ifdef FEATURE_GSTK_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_GSTK_TEST_AUTOMATION */

/*===========================================================================
FUNCTION   gstk_fill_error_response

DESCRIPTION
  This function fills uim cmd ptr with error response when the length of
  terminal response exceeds the max chars limit for uim.

PARAMETERS
  uim_cmd_ptr          :  Pointer to UIM command
  STK_response_header  :  Terminal Response header type
  byte_offset          :  offset

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_fill_error_response(
  uim_cmd_type                        *uim_cmd_ptr,
  gstk_terminal_response_header_type  STK_response_header,
  int                                 byte_offset
)
{
  gstk_status_enum_type  gstk_status                = GSTK_ERROR;
  int                    gstk_length_of_error_rsp   = 0;
  if(uim_cmd_ptr != NULL)
  {
    gstk_length_of_error_rsp = STK_response_header.command_details.length +
                               GSTK_TAG_LENGTH_LEN +
                               STK_response_header.device_id.device_tag_length +
                               GSTK_TAG_LENGTH_LEN +
                               4;/* 4 is the length of result tlv*/
    if(gstk_length_of_error_rsp < UIM_MAX_CHARS)
    {
      /* copy command details */
      gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                              &STK_response_header.command_details,
                              byte_offset,
                              (STK_response_header.command_details.length +
                              GSTK_TAG_LENGTH_LEN ),
                              UIM_MAX_CHARS,
                              sizeof(gstk_command_details_tlv_type));
      byte_offset = (int)(byte_offset + STK_response_header.command_details.length +
                     GSTK_TAG_LENGTH_LEN );
      /* copy device */
      gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                              &STK_response_header.device_id,
                              byte_offset,
                              (STK_response_header.device_id.device_tag_length +
                              GSTK_TAG_LENGTH_LEN ),
                              UIM_MAX_CHARS,
                              sizeof(gstk_device_identities_tag_tlv_type));
      byte_offset = (int)(byte_offset + STK_response_header.device_id.device_tag_length +
                     GSTK_TAG_LENGTH_LEN );

      /* copy result */
      uim_cmd_ptr->terminal_response.data[byte_offset++] = STK_response_header.result.result_tag;
      uim_cmd_ptr->terminal_response.data[byte_offset++] = GSTK_ERROR_RESP_RESULT_LEN;
      uim_cmd_ptr->terminal_response.data[byte_offset++] = uint32touint8((uint32)GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
      uim_cmd_ptr->terminal_response.data[byte_offset++] = uint32touint8((uint32)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      /* copy number of bytes in error response */
      uim_cmd_ptr->terminal_response.num_bytes           = uint32touint8((uint32)byte_offset);
      gstk_status = GSTK_SUCCESS;
    }
    else
    {
      MSG_ERROR("Length of Terminal response exceeding Max Chars in UIM Buffer", 0, 0, 0);
      gstk_status = GSTK_INVALID_LENGTH;
    }
  }
  else
  {
    MSG_ERROR("no UIM cmd buf", 0, 0, 0);
    gstk_status = GSTK_MEMORY_ERROR;
  }
  return gstk_status;
}/* gstk_fill_error_response */

#ifdef FEATURE_ESTK
#error code not present
#endif /* FEATURE_ESTK */

/*===========================================================================

FUNCTION gstk_is_first_cmd_bip_env

DESCRIPTION
  This function checks if the first command in gstk command queue is BIP
  Evt Dl or not.

PARAMETERS
  None

DEPENDENCIES
  gstk command queue should be created in advance

RETURN VALUE
  TRUE : If first command is BIP Evt Dl
  FALSE: If first command is not BIP Evt Dl

SIDE EFFECTS
  This will determine if fetch can be handeled by UIM or not. Also it will
  be used to check if BIP envelope can be processed by gstk or not.

SEE ALSO
  None
===========================================================================*/
boolean gstk_is_first_cmd_bip_env(
)
{
  gstk_task_cmd_type *gstk_first_cmd_ptr = NULL;
  gstk_first_cmd_ptr = (gstk_task_cmd_type*) q_check(&gstk_task_cmd_q);
  if(gstk_first_cmd_ptr == NULL)
  {
    MSG_ERROR("NULL Pointer",0,0,0);
    return FALSE;
  }
  if(gstk_first_cmd_ptr->cmd.general_cmd.message_header.command_group == GSTK_ENVELOPE_CMD)
  {
    if(gstk_first_cmd_ptr->cmd.general_cmd.message_header.command_id == (uint32)GSTK_DATA_AVAIL_EVT_IND ||
       gstk_first_cmd_ptr->cmd.general_cmd.message_header.command_id == (uint32)GSTK_CH_STATUS_EVT_IND)
      return TRUE;
  }
  return FALSE;
}/* gstk_is_first_cmd_bip_env */
