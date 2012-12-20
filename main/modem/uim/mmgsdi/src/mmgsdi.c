/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   D R I V E R   I N T E R F A C E

                            M A I N     T A S K


GENERAL DESCRIPTION

  This source file contains the main GSDI task along with supporting functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001 - 2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3.65/uim/mmgsdi/src/mmgsdi.c#2 $$ $DateTime: 2011/03/25 06:51:16 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/11   ssr     Caching the MEID data
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
04/29/09   js      Added prototype for static function, fixed compile warnings
03/25/09   sun     Added support for Virtuim
03/19/09   js      Support for Enhanced NAA Refresh by file path
03/06/09   tml     Clean up refresh pointer when we receive a card error
02/24/09   kk      Updated Sec rnd timer name
02/12/09   kk      Add files to refresh file list for re-reading during FCN
02/06/09   jar     Fixed Declaration type of secrnd_timer
02/06/09   jar     Fixed compilation issue on defining secrnd_timer.
02/05/09   jar     Define the SECRND Timer
02/03/09   rm      Fix to avoid a possible dangling command pointer access by
                   the UIM task in erroneous situations (SIM interface going bad
                   due to transmission errors leading to multiple recoveries)
01/22/09   sun     Removal of old GSDI Refresh timer
01/21/09   kk      Featurized conditional file access
01/15/09   ssr     Fixed Lint errors for RTRE changes
01/12/09   ssr     MMGSDI RTRE Configuration initialization
01/09/09   tml     ensure MMGSDI_GENERATE_KEY_VPM_REQ response will be sent
                   back to client when the protocol is UICC
01/08/09   tml     Report back to client regardless of whether a card/
                   protocol is known or not
12/01/08   ssr     Handled error condition for some MMGSDI Functions
11/19/08   js      Fixed KW error for potential null ptr derferencing
11/18/08   js      Updated featurization for Send APDU related code
11/17/08   nk      Removed const from input in mmgsdi_add_client_id_entry to
                   fix compiler warning
11/15/08   js      Removed MFLO featurization for Send APDU related code
10/29/08   tml     Modem restart client id dereg clean up
10/06/08   jk      Allow GSM SIM access in 1X Only
09/29/08   ssr     Return MMGSDI_ACCESS_DENIED on unfulfilled access condition
09/22/08   js      Fixed Lint Errors
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/26/08   jk      Fixed Featurization for 1X Auth for LTK compilation
08/22/08   js      Klockwork fixes
08/20/08   jk      Support for Generate Key API
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
06/25/08   sun     Fixed Compilation
06/23/08   tml     Added featurization for BT in WM
06/23/08   sp      Update mmgsdi_util_reset_app_data()
                   with new function parameter in mmgsdi_init.
06/04/08   tml     Added status word to response header.  Messages cleanup
                   and return more appropriate error code when
                   mmgsdi_process_response is unable to process the responses.
                   return NOT SUPPORTED appropriately due to featurization
04/08/08   nk      Added CDMA Refresh Support
03/24/08   sun     Added support for OMA Bcast
03/19/08   nk      Updated call to mmgsdi_icc_rsp_get_file_attr and
                   mmgsdi_icc_rsp_rehabitate
03/14/08   sk      Reverted the change for the usage of signals during
                   sim initialization
02/22/08   tml     Queue get all pin status via library to avoid
                   simultaneous handling of 2 get all pin status at the
                   same time when there is > 1 usim app available
02/21/08   sun     Added CPHS files for FCN Registration
02/14/08   kk      Removed the Usage of signals during sim initialization
01/20/08   sun     If same command is in progress then do not set the task
                   command signal
01/11/08   jar     Don't send a generic error response for a flush queue attempt
                   when the current command has a response pending.
                   For Card Power Up, Set Refresh Orig to MMGSDI_CARD_PUP_REQ.
12/17/07   vs      Featurized function call mmgsdi_icc_run_cave()
12/10/07   sun     Verify PIN Internally when SAP is Disconnected
11/19/07   js      Radio Access Technology Balancing support
11/30/07   sun     Added support for NAA refresh
11/16/07   jk      Reorganized signal processing to fix crash
11/15/07   tml     Use regular rex function for critical sections for
                   mmgsdi_task_rsp_q to prevent gsdi task stuck when synchronous
                   commands were issued
09/23/07   sk      Put a lock around mmgsdi_task_rsp_q
09/10/07   tml     Added MFLO Supports.
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
07/12/07   sk      Fixed boolean return value and more EFs to
                   mmgsdi_present_in_preinit_file_access_list()
06/26/07   sk      Fixed compilation problem.
06/25/07   sk      Prevent certain access to card at particular gsdi states
05/21/07   jk      Do not allow reentry to mmgsdi_recover_after_reset
05/17/07   wli     Added support for JCDMA RUIM
05/13/07   sp      Fixed warning.
05/07/07   sp      Added new mmgsdi malloc/free functions.
05/02/07   sun     Added support for Service Available and USIM Auth
04/24/07   sun     Fixed Warning
04/17/07   sp      Featurize for LTK
04/17/07   sun     Set up a timer if external module other than UIM does not
                   respond
04/03/07   sun     Let the entire GET_PIN_STATUS complete before processing
                   new requests
03/19/07   sun     Fixed Lint Error
02/27/07   jk      Added featurization for Memory Reduction
02/09/07   tml     Ensure confirmation is built and sent to client
01/17/07   sp      Fix thinUI build issues.
01/17/07   pv      Use gsdi_perso_engine_notify_clients_of_perso_event instead
                   of gsdi_perso_notify_clients_of_perso_event  based on
                   the feature FEATURE_MMGSDI_PERSONALIZATION_ENGINE
12/27/06   sp      Back out pin event changes.
12/11/06   tml     Ensure proper event notification for late registered clients
11/14/06   sp      Fixed lint error
11/13/06   sun     Synch MMGSDI any call present flag with CM Info
11/06/06   sp      Proceed with post pin functions only after pin event
                   is sent
10/25/06   tml     Trigger post pin1 from SELECT_AID event only if there is
                   replacement criteria is satisfied.
10/13/06   sun     When the card is powered off, let the card pup
                   go to the card regardless of the slot.
10/02/06   sp      Function to read fdn, bdn and acl from nv
08/20/06   jar     Process the Error Signal before the response.
08/23/06   sun     Fixed Lint Errors
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/22/06   tml     Compilation fix for 1x
08/16/06   tml     Added ACL support
08/16/06   sun     Fixed Lint
08/14/06   sun     Fixed the request type for static cnfs
07/26/06   tml     Fixed compilation warning
07/26/06   sun     Added support for multi USIM App and MMGSDI PIN
07/07/06   sun     Removed Reference to mc.h
06/12/06   pv      Call mmgsdi_recover_after_reset when
                   GSDI_UIM_RESET_COMPLETE_SIG is received
06/13/06   tml     Fixed compilation issue
06/08/06   tml     Fixed featurization issue
05/23/06   tml     Lint, BDN support and GSM Invalidate and Rehabilitate support
05/19/06   pv      Do not wait for GSDI_TASK_CMD_Q_SIG while a mmgsdi command
                   is in progress.
05/22/06   tml     Fixed lint issue
05/19/06   tml     Fixed compilation warnings
05/15/06   tml     Flush pending mmgsdi command upon receiving UIM error
                   signal
05/03/06   pv      Do now allow processing a GSDI command while a MMGSDI
                   command is in progress.
                   Do not wait for MMGSDI_TASK_CMD_Q_SIG signal while mmgsdi
                   is only waiting for a response.
                   Clear the signals when no more items to look at.
04/11/06   jar     Added ONCHIP SIM Support
03/21/06   tml     lint
03/14/06   sun     Register with CM for PH_IN_USE event.
02/15/06   tml     lint fix
02/16/06   sp      Support for EF_EHPLMN
12/12/05   nk      Merge for ARR caching and by path changes
01/30/05   pv      Do not notify any other clients other than BT when a
                   failure is reported by UIM in passive mode.
01/29/06   tml     Moved mmgsdi_sim_refresh to mmgsdi.c
12/20/05   sun     Added missing commands to mmgsdi_cmd
12/14/05   tml     Moved Confirmations to mmgsdi_cnf.c
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/09/05   sun     Fixed Lint Errors
11/08/05   pv      change mmgsdi_check_card_status - mmgsdi_3G_ready_status_ind
                   and change the parameters to mmgsdi_send_card_status so as
                   to send a 3G session ready status to USIM.
11/03/05   sun     Added support for Refresh
10/28/05   sun     Fixed SAP Disconnect
10/26/05   pv      Do not print an error message when a Refresh is in progress
10/20/05   sun     Added support for BT Deregisteration
10/18/05   tml     Added in_synch info to the cache
09/29/05   tml     Client Deregistration support
08/30/05   jar     Added support to register as a client and
                   to handle status commands to / from the card
08/26/05   pv      Consider MMGSDI_REFRESH_IN_PROGRESS as success
08/26/05   sun     Added support for BT SAP
08/25/05   pv      Added support for powering down and powerering up SIM/USIM
07/26/05   sst     Added MMGSDIBT support
07/18/05   jk      Fixed Compile Issues for 1x Targets
07/14/05   tml     Added original request type
07/12/05   tml     Session Management and ISIM support
06/16/05   tml     Prototype change for transparent and record access
05/05/05   tml     initial versoin


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rex.h"
#include "task.h"
#ifdef FEATURE_DOG
   #include "dog.h"
#endif /* FEATURE_DOG */
#include <stdlib.h>
#include "msg.h"
#include "err.h"
#include "queue.h"
#include "dsm.h"
#include "memheap.h"
#include "customer.h"
#include "string.h"
#ifdef FEATURE_UIM
   #include "uim.h"
#endif /* FEATURE_UIM */

#include "gsdi_exp.h"
#include "gsdi.h"

#include "intconv.h"
#include "mmgsdiutil.h"

#include "mmgsdi.h"
#include "mmgsdi_icc.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_icc_rsp.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi_session.h"
#include "mmgsdi_gen.h"
#include "mmgsdi_gen_rsp.h"
#include "mmgsdi_recover.h"
#include "tmc.h"


#if defined ( FEATURE_MMGSDI_GSM ) || defined ( FEATURE_MMGSDI_UMTS )
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
  #include "target.h"
  #include "queue.h"
  #include "nvruimi.h"
#endif /* FEATURE_MMGSDI_CDMA */

#ifdef FEATURE_VIRTUAL_SIM
  #include "virtuim.h"
#endif  /* FEATURE_VIRTUAL_SIM */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#include "mmgsdi_cnf.h"

#ifdef FEATURE_MMGSDI_NV_ME_CONFIG
  #include "gsdi_nv.h"
#endif

#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
  #include "mmgsdi_ruim_jcdma.h"
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */

#include "mmgsdi_refresh.h"
#include "mmgsdi_refresh_rsp.h"

#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
mmgsdi_client_id_reg_table_type mmgsdi_client_id_reg_table[MMGSDI_MAX_CLIENT_INFO];

mmgsdi_uim_client_req_user_data_type mmgsdi_client_req_table_info[MMGSDI_MAX_CLIENT_INFO];

static mmgsdi_return_enum_type mmgsdi_process_client_reg_req(
  mmgsdi_client_id_reg_req_type * req_ptr
);

static mmgsdi_return_enum_type mmgsdi_process_client_dereg_req(
  const mmgsdi_client_id_dereg_req_type * req_ptr
);

static mmgsdi_return_enum_type mmgsdi_process_client_reg_cnf(
  const mmgsdi_client_id_reg_rsp_type   * client_rsp_ptr
);

static mmgsdi_return_enum_type mmgsdi_process_client_dereg_cnf(
  const mmgsdi_client_id_reg_rsp_type   * client_rsp_ptr
);

static void mmgsdi_gsdi_evt_cb (
  const mmgsdi_event_data_type *event_ptr
);

/* A static confirmation, used when the process_response was not
   able to dynamically allocate or if there is any other build confirmation error */
static mmgsdi_task_cmd_type mmgsdi_static_task_buf;

static void mmgsdi_init (void);

#ifdef FEATURE_MMGSDI_INIT_RTRE_CONFIG
void mmgsdi_init_rtre_config(void);
#endif /* FEATURE_MMGSDI_INIT_RTRE_CONFIG */

mmgsdi_record_cache_node_list_type mmgsdi_record_cache_list_node;

static boolean mmgsdi_cmd_in_progress        = FALSE;
static uint8   mmgsdi_queue_service_counter  = 1;
static mmgsdi_meid_data_type  mmgsdi_data_meid = {{0,0}, FALSE};

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */

/* Queue to hold commands for mmgsdi */
q_type mmgsdi_task_cmd_q;

/* Queue to hold responses for mmgsdi */
q_type mmgsdi_task_rsp_q;

/* Global structure to general info for gsdi task */
mmgsdi_generic_data_type mmgsdi_generic_data;
mmgsdi_slot_data_type    mmgsdi_data_slot1;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
mmgsdi_slot_data_type    mmgsdi_data_slot2;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

mmgsdi_refresh_info_type *mmgsdi_refresh_info_ptr;

rex_crit_sect_type gsdi_crit_sect; /* gsdi critical section */

rex_crit_sect_type mmgsdi_crit_sect;

/* Timer definitions */
#ifdef FEATURE_DOG
rex_timer_type gsdi_rpt_timer;
#endif /* FEATURE_DOG */

#if defined(FEATURE_SEC) && defined(FEATURE_MMGSDI_PERSO_SEC_RAND_CB_API)
rex_timer_type     gsdi_perso_secrnd_timer;
#endif /* FEATURE_SEC && FEATURE_MMGSDI_PERSO_SEC_RAND_CB_API*/


#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS*/


#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
/*
** RUIM CACHE ELEMENTS
*/
sim_cache_element_T gsdi_ruim_cache[GSDI_NUM_FILES_IN_RUIM_CACHE] =
{
    { UIM_CDMA_IMSI_M,        TRUE,   FALSE, GSDI_CACHE_CDMA_IMSI_M_LEN,      { 0x00 }, 0, NULL, FALSE},
    { UIM_CDMA_IMSI_T,        TRUE,   FALSE, GSDI_CACHE_CDMA_IMSI_T_LEN,      { 0x00 }, 0, NULL, FALSE},
    { UIM_CDMA_CDMA_SVC_TABLE,TRUE,   FALSE, GSDI_CACHE_CDMA_CST_LEN,         { 0x00 }, 0, NULL, FALSE},
    { UIM_CDMA_PREF_LANG,     TRUE,   FALSE, GSDI_CACHE_CDMA_PREF_LANG_LEN,   { 0x00 }, 0, NULL, FALSE},
    { UIM_CDMA_ECC,           TRUE,   FALSE, GSDI_CACHE_CDMA_ECC_LEN,         { 0x00 }, 0, NULL, FALSE}
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    ,
    { UIM_CDMA_CDMA_SVC_TABLE,FALSE,  FALSE, GSDI_CACHE_CDMA_CST_LEN,         { 0x00 }, 0, NULL, FALSE},
    { UIM_CDMA_ECC,           FALSE,  FALSE, GSDI_CACHE_CDMA_ECC_LEN,         { 0x00 }, 0, NULL, FALSE}
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
};
#endif /* FEATURE_MMGSDI_CDMA */

sim_cache_element_T gsdi_common_cache[GSDI_NUM_FILES_IN_COMMON_CACHE] =
{
    { UIM_ELP,  TRUE,   FALSE, GSDI_LP_LEN, { 0x00 }, 0, NULL, FALSE},
};

mmgsdi_file_enum_type mmgsdi_refresh_reg_file_list[MMGSDI_REFRESH_REG_FILES] =
{
  MMGSDI_GSM_AD,        MMGSDI_GSM_AAEM,       MMGSDI_GSM_ACC,        MMGSDI_GSM_BCCH,
  MMGSDI_GSM_EMLPP,     MMGSDI_GSM_FPLMN,      MMGSDI_GSM_HPLMN,      MMGSDI_GSM_HPLMNACT,
  MMGSDI_GSM_IMSI,      MMGSDI_GSM_KC,         MMGSDI_GSM_KCGPRS,     MMGSDI_GSM_LOCI,
  MMGSDI_GSM_LOCIGPRS,  MMGSDI_GSM_LP,         MMGSDI_GSM_OPLMNWACT,  MMGSDI_GSM_PHASE,
  MMGSDI_GSM_PLMN,      MMGSDI_GSM_PLMNWACT,   MMGSDI_GSM_SPN,        MMGSDI_GSM_SST,
  MMGSDI_GSM_ACM,       MMGSDI_GSM_ACM_MAX,    MMGSDI_GSM_PUCT,       MMGSDI_GSM_CBMID,
  MMGSDI_GSM_ECC,       MMGSDI_USIM_ACM_MAX,   MMGSDI_USIM_ACM,       MMGSDI_USIM_PUCT,
  MMGSDI_USIM_ACC,      MMGSDI_USIM_LOCI,      MMGSDI_USIM_PSLOCI,    MMGSDI_USIM_LI,
  MMGSDI_USIM_KCGPRS,   MMGSDI_USIM_AD,        MMGSDI_USIM_CBMID,     MMGSDI_USIM_EHPLMN,
  MMGSDI_USIM_AAEM,     MMGSDI_USIM_EMLPP,     MMGSDI_USIM_EST,       MMGSDI_USIM_FPLMN,
  MMGSDI_USIM_HPLMN,    MMGSDI_USIM_HPLMNWACT, MMGSDI_USIM_IMSI,      MMGSDI_USIM_KC,
  MMGSDI_USIM_KEYS,     MMGSDI_USIM_KEYSPS,    MMGSDI_USIM_PLMNWACT,  MMGSDI_USIM_OPLMNWACT,
  MMGSDI_USIM_RPLMNACT, MMGSDI_USIM_SPN,       MMGSDI_USIM_START_HFN, MMGSDI_USIM_THRESHOLD,
  MMGSDI_USIM_UST,      MMGSDI_USIM_ARR,       MMGSDI_TELECOM_ARR,    MMGSDI_ARR,
  MMGSDI_GSM_CPHSI,     MMGSDI_USIM_CPHSI,     MMGSDI_GSM_CSP,        MMGSDI_USIM_CSP,
  MMGSDI_USIM_7F66_PROP1_RAT
#ifdef FEATURE_RUIM_CDMA_REFRESH
  ,MMGSDI_CDMA_ACCOLC,                          MMGSDI_CDMA_IMSI_M,
  MMGSDI_CDMA_IMSI_T,                          MMGSDI_CDMA_TMSI,
  MMGSDI_CDMA_NAM_LOCK,                        MMGSDI_CDMA_MSISDN,
  MMGSDI_CDMA_CC,                              MMGSDI_CDMA_SF_EUIM_ID,
  MMGSDI_CDMA_ECC,                             MMGSDI_CDMA_PREF_LANG,
  MMGSDI_CDMA_PRL,                             MMGSDI_CDMA_SERVICE_PREF,
  MMGSDI_CDMA_RUIM_ID,                         MMGSDI_CDMA_SCI,
  MMGSDI_CDMA_SVC_TABLE,                       MMGSDI_CDMA_OTAPA_SPC_ENABLE,
  MMGSDI_CDMA_ANALOG_HOME_SID,                 MMGSDI_CDMA_HOME_SID_NID,
  MMGSDI_CDMA_ZONE_BASED_REGN_IND,             MMGSDI_CDMA_SYS_REGN_IND,
  MMGSDI_CDMA_DIST_BASED_REGN_IND,             MMGSDI_CDMA_SPC_STATUS,
  MMGSDI_CDMA_CALL_TERM_MODE_PREF,             MMGSDI_CDMA_OTASP_OTAPA_FEATURES,
  MMGSDI_CDMA_UIM_ID_USAGE_IND,                MMGSDI_CDMA_3GPD_SIPPAPSS,
  MMGSDI_CDMA_3GPD_SIPUPP,                     MMGSDI_CDMA_3GPD_MIPUPP,
  MMGSDI_CDMA_3GPD_MIPFLAGS,                   MMGSDI_CDMA_3GPD_TCPCONFIG,
  MMGSDI_CDMA_ANALOG_LOCN_AND_REGN_IND
#endif /* FEATURE_RUIM_CDMA_REFRESH */
};

mmgsdi_app_pin_info_type mmgsdi_app_pin1_table[MMGSDI_MAX_APP_INFO];
mmgsdi_app_pin_info_type mmgsdi_app_univ_pin_table[1];
mmgsdi_int_app_info_type mmgsdi_app_pin_table[MMGSDI_MAX_APP_INFO];
rex_timer_type mmgsdi_ext_api_timer;
rex_timer_type mmgsdi_refresh_timer;
rex_timer_type mmgsdi_refresh_auto_trigger_timer;

/*===========================================================================
FUNCTION MMGSDI_3G_READY_STATUS_IND

DESCRIPTION
  Function used to Queue Back a Command to send a status indicating that the
  ME is ready for a 3G session if it is a UICC card.  The MMGSDI Task will
  process the command at a later time.  This would be translated to a regular
  status command for a non UICC card

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_3G_ready_status_ind(
  mmgsdi_slot_id_enum_type slot
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  /* --------------------------------------------------------------------------
     Validate the Slot Provided
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(slot,FALSE);

  /* --------------------------------------------------------------------------
     Now use the existing MMGSDI Library to Queue a Status Command to the
     SIM or USIM Card
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_send_card_status(mmgsdi_generic_data.client_id,
                                          slot,
                                          MMGSDI_STATUS_APP_INITIALIZED,
                                          MMGSDI_STATUS_TLV_DATA,
                                          mmgsdi_util_internal_cb,
                                          0x00);
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR("Could not send Card Status Command",0,0,0);
  }
  else
  {
    MSG_HIGH("CARD STATUS REQ Sent to MMGSDI Task",0,0,0);
  }
  return mmgsdi_status;
} /* mmgsdi_3G_ready_status_ind */


/*===========================================================================
FUNCTION MMGSDI_IS_CMD_SLOT_SPECIFIC

DESCRIPTION
  This function determines whether the request is slot specific or non slot
  related

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  NONE
===========================================================================*/
static boolean mmgsdi_is_cmd_slot_specific(
  mmgsdi_cmd_enum_type       cmd)
{
  switch (cmd)
  {
    case MMGSDI_CLIENT_ID_REG_REQ:
    case MMGSDI_CLIENT_EVT_REG_REQ:
    case MMGSDI_CLIENT_ID_DEREG_REQ:
    case MMGSDI_CLIENT_EVT_DEREG_REQ:
    case MMGSDI_SAP_INIT_REQ:
    case MMGSDI_SAP_PIN_VERIFY_REQ:
    case MMGSDI_SAP_CONNECT_REQ:
    case MMGSDI_SAP_DISCONNECT_REQ:
    case MMGSDI_SAP_POWER_ON_REQ:
    case MMGSDI_SAP_POWER_OFF_REQ:
    case MMGSDI_SAP_CARD_READER_STATUS_REQ:
    case MMGSDI_SAP_RESET_REQ:
    case MMGSDI_CARD_PUP_REQ:
    case MMGSDI_CARD_PDOWN_REQ:
    case MMGSDI_CARD_STATUS_REQ:
    case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
    /*Refresh Request needs to be slot unspecific for cases such as when
      refresh is sent as a result of card power up*/
    case MMGSDI_REFRESH_REQ:
    case MMGSDI_CARD_RESET_REQ:
    case MMGSDI_UIM_REPORT_RSP:
    case MMGSDI_CLIENT_ID_REG_RSP:
    case MMGSDI_CNF:
    case MMGSDI_MAX_CMD_ENUM:
      return FALSE;

    case MMGSDI_CLIENT_INIT_COMP_REQ:
    case MMGSDI_READ_REQ:
    case MMGSDI_WRITE_REQ:
    case MMGSDI_GET_FILE_ATTR_REQ:
    case MMGSDI_SESSION_OPEN_REQ:
    case MMGSDI_SESSION_CLOSE_REQ:
    case MMGSDI_SESSION_DEACTIVATE_REQ:
    case MMGSDI_SESSION_SELECT_AID_REQ:
    case MMGSDI_ISIM_AUTH_REQ:
    case MMGSDI_SEND_APDU_REQ:
    case MMGSDI_SAP_GET_ATR_REQ:
    case MMGSDI_SAP_SEND_APDU_REQ:
    case MMGSDI_SAP_DEREG_REQ:
    case MMGSDI_SAP_REPORT_RSP:
    case MMGSDI_DISABLE_BDN_REQ:
    case MMGSDI_ENABLE_BDN_REQ:
    case MMGSDI_ACL_OPERATION_REQ:
    case MMGSDI_REHABILITATE_REQ:
    case MMGSDI_INVALIDATE_REQ:
    case MMGSDI_PIN_OPERATION_REQ:
    case MMGSDI_SELECT_AID_REQ:
    case MMGSDI_GET_ALL_PIN_STATUS_REQ:
    case MMGSDI_SRV_AVAILABLE_REQ:
    case MMGSDI_USIM_AUTH_REQ:
    case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
    case MMGSDI_COMPUTE_IP_AUTH_REQ:
    case MMGSDI_RUN_CAVE_REQ:
    case MMGSDI_GENERATE_KEY_VPM_REQ:
    case MMGSDI_GET_ATR_REQ:
    case MMGSDI_CARD_READER_STATUS_REQ:
    case MMGSDI_MFLO_INIT_ACT_PARAMS_REQ:
    case MMGSDI_MFLO_GET_SUBSCRIBER_ID_REQ:
    case MMGSDI_MFLO_GET_PUBLIC_KEY_REQ:
    case MMGSDI_MFLO_SIGN_UCAST_REQ:
    case MMGSDI_MFLO_VERIFY_UCAST_REQ:
    case MMGSDI_BCAST_REQ:
    case MMGSDI_SEARCH_REQ:
    case MMGSDI_CREATE_PKCS15_TABLE_REQ:
      return TRUE;
  }
  MSG_ERROR("Unknown command 0x%x",cmd, 0, 0);
  return FALSE;
}


/*===========================================================================
FUNCTION MMGSDI_IS_CMD_IN_PROGRESS

DESCRIPTION
  This function checks to see if a command is in progress

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  Limited to Get PIN Status only
===========================================================================*/
static boolean mmgsdi_is_cmd_in_progress(
  const mmgsdi_task_cmd_type *    mmgsdi_task_ptr
)
{
  mmgsdi_cmd_enum_type cmd_req_enum  = MMGSDI_MAX_CMD_ENUM;
  int32                          i   = 0;

  if(mmgsdi_task_ptr == NULL)
  {
    MSG_ERROR("TASK POINTER IS NULL",0,0,0);
    return FALSE;
  }

  for (i=0; i<MMGSDI_MAX_CLIENT_INFO; i++)
  {
    cmd_req_enum = mmgsdi_client_req_table_info[i].orig_request_type;

    switch (cmd_req_enum)
    {
      case MMGSDI_GET_ALL_PIN_STATUS_REQ:
        /*Get PIN Status internally sends request for verfiy pin
          and unblock pin to UIM but the commands for those are not
          queued so it will not get here*/
        if(mmgsdi_task_ptr->cmd.cmd_enum == cmd_req_enum)
        {
          return TRUE;
        }
        break;

      default:
        break;
    }
  }
  return FALSE;
}


/*===========================================================================
FUNCTION MMGSDI_CAN_PROCEED_CMD_WITH_SLOT

DESCRIPTION
  This function determines whether the request can proceed to the UIM
  module or not.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  NONE
===========================================================================*/
static boolean mmgsdi_can_proceed_cmd_with_slot(
  mmgsdi_cmd_enum_type       cmd,
  mmgsdi_slot_id_enum_type   slot,
  boolean                   *conditional_flag_ptr)
{
  mmgsdi_slot_data_type   *slot_data_ptr     = NULL;
  boolean                  proceed_to_card   = TRUE;

  /* conditional_flag_ptr is used to indicate if certain types of access to
     EFs on the card are allowed when 'SIM init completed' is still pending.
  */

  if(conditional_flag_ptr == NULL)
  {
    MSG_ERROR("conditional_flag_ptr NULL", 0, 0, 0);
    return FALSE;
  }

  switch (slot)
  {
  case MMGSDI_SLOT_1:
    slot_data_ptr = &mmgsdi_data_slot1;
    break;
  case MMGSDI_SLOT_2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    slot_data_ptr = &mmgsdi_data_slot2;
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  default:
    MSG_ERROR("Slot Specific cmd with Unknown Slot 0x%x", slot, 0, 0);
    proceed_to_card = FALSE;
    break;
  }
  if (slot_data_ptr != NULL)
  {
    MSG_HIGH("mmgsdi state 0x%x", slot_data_ptr->mmgsdi_state, 0, 0);
    switch (slot_data_ptr->mmgsdi_state)
    {
      case MMGSDI_STATE_NOT_INIT:
      case MMGSDI_STATE_NO_CARD:
      case MMGSDI_STATE_ENUM_MAX:
        proceed_to_card = FALSE;
        break;
      case MMGSDI_STATE_CARD_INSERTED:
      case MMGSDI_STATE_APP_DETECTED:
      case MMGSDI_STATE_PIN_EVT_SENT:
        {
          switch(cmd)
          {
            case MMGSDI_READ_REQ:
            case MMGSDI_WRITE_REQ:
            case MMGSDI_GET_FILE_ATTR_REQ:
            case MMGSDI_SEARCH_REQ:
              *conditional_flag_ptr = TRUE;
              proceed_to_card = TRUE;
              break;
            default:
              proceed_to_card = TRUE;
              break;
          }
        }
        break;
      case MMGSDI_STATE_CARD_INIT_COMPLETED:
        break;
    }
  }
  return proceed_to_card;
}


/*===========================================================================
   FUNCTION:      MMGSDI_BUILD_AND_QUEUE_CLIENT_REG_RSP

   DESCRIPTION:
     This function will build and queue the client ID, Event registration
     and deregistration response

   DEPENDENCIES:

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_build_and_queue_client_reg_rsp(
  int32                               index,
  mmgsdi_return_enum_type             client_reg_status,
  const mmgsdi_request_header_type  * req_header_ptr
)
{
  mmgsdi_task_cmd_type       *task_cmd_ptr = NULL;
  mmgsdi_request_header_type rsp_header;
  int32                      mem_size;
  uint32                     task_cmd_len  =0;

  mem_size = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_RETURN_IF_NULL(req_header_ptr);

  task_cmd_len = sizeof(q_link_type) + int32touint32(mem_size);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /*---------------------------------------------------------------------------
    POPULATE THE RESPONSE MESSAGE PAYLOAD
    1. client_id         - Client ID Obtained
    2. request_type      - Request Command Type
    3. orig_request_type - original request type
    3. request_len       - Length of message + header
    4. payload_len       - Length of the message payload
    5. request_id_ptr    - Request ID (task_tcb_ptr)
    6. slot_id           - Slot that the request is to be performed on
    7. client_data       - Pointer to Client Data
    8. response_cb       - Response Callback Pointer
  ---------------------------------------------------------------------------*/
  rsp_header = task_cmd_ptr->cmd.cmd.client_id_reg_rsp.request_header;

  rsp_header.client_id                  = req_header_ptr->client_id;
  rsp_header.request_type               = req_header_ptr->request_type;
  rsp_header.orig_request_type          = req_header_ptr->orig_request_type;
  rsp_header.request_id_ptr             = req_header_ptr->request_id_ptr;
  rsp_header.slot_id                    = req_header_ptr->slot_id;
  rsp_header.client_data                = req_header_ptr->client_data;
  rsp_header.response_cb                = req_header_ptr->response_cb;
  rsp_header.request_len                = uint32toint32(sizeof(mmgsdi_task_cmd_type));
  rsp_header.payload_len                = rsp_header.request_len -
                                            uint32toint32(sizeof(mmgsdi_request_header_type));

  task_cmd_ptr->cmd.cmd_enum = MMGSDI_CLIENT_ID_REG_RSP;
  task_cmd_ptr->cmd.cmd.client_id_reg_rsp.rsp_data_index = index;
  task_cmd_ptr->cmd.cmd.client_id_reg_rsp.status = client_reg_status;
  rex_enter_crit_sect(&mmgsdi_crit_sect);
  (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);
  /* Put the message on the queue */
  q_put(&mmgsdi_task_rsp_q, &task_cmd_ptr->link);

  /* Set the command queue signal for */
  /* receive task                     */
  (void) rex_set_sigs(&gsdi_task_tcb, MMGSDI_TASK_RSP_Q_SIG);
  rex_leave_crit_sect(&mmgsdi_crit_sect);
  return MMGSDI_SUCCESS;
}


/*===========================================================================
FUNCTION MMGSDI_BUILD_AND_QUEUE_GENERIC_ERR_RSP

DESCRIPTION
  This function build the generic error response in case a command is not
  able to be sent to MMGSDI/UIM for further processing.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_build_and_queue_generic_err_rsp(
  mmgsdi_slot_id_enum_type slot,
  int32                    index,
  mmgsdi_cmd_enum_type     cmd,
  mmgsdi_return_enum_type  mmgsdi_error_status)
{
  mmgsdi_request_header_type request_header;
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_SUCCESS;

  if ((index < 0) || (index >= MMGSDI_MAX_CLIENT_INFO))
    return MMGSDI_ERROR;

  switch (cmd)
  {
  case MMGSDI_CLIENT_ID_REG_REQ:
  case MMGSDI_CLIENT_EVT_REG_REQ:
  case MMGSDI_CLIENT_ID_DEREG_REQ:
  case MMGSDI_CLIENT_EVT_DEREG_REQ:
  case MMGSDI_CLIENT_INIT_COMP_REQ:
    /* flush client ID rsp */
    request_header.client_id         = mmgsdi_client_req_table_info[index].client_id;
    request_header.request_type      = mmgsdi_client_req_table_info[index].request_type;
    request_header.orig_request_type = mmgsdi_client_req_table_info[index].orig_request_type;
    request_header.request_id_ptr    = mmgsdi_client_req_table_info[index].request_tcb_ptr;
    request_header.slot_id           = slot;
    request_header.client_data       = mmgsdi_client_req_table_info[index].client_data;
    request_header.response_cb       = mmgsdi_client_req_table_info[index].response_cb;

    mmgsdi_status = mmgsdi_build_and_queue_client_reg_rsp(index, MMGSDI_ERROR, &request_header);
    break;

  case MMGSDI_SAP_INIT_REQ:
  case MMGSDI_SAP_PIN_VERIFY_REQ:
  case MMGSDI_SAP_DEREG_REQ:
  case MMGSDI_SAP_DISCONNECT_REQ:
    mmgsdi_status = mmgsdi_util_queue_mmgsdi_sap_report_rsp(
                    index, cmd, MMGSDI_ERROR);
    break;
  case MMGSDI_UIM_REPORT_RSP:
  case MMGSDI_CLIENT_ID_REG_RSP:
  case MMGSDI_CNF:
  case MMGSDI_MAX_CMD_ENUM:
    return MMGSDI_ERROR;

  default:
    /* clear the pending command first */
    mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
      index, cmd, mmgsdi_error_status);
    break;
  }
  return mmgsdi_status;
} /* mmgsdi_build_and_queue_generic_err_rsp */


/*===========================================================================
FUNCTION MMGSDI_QUEUE_GENERIC_ERR_RSP_AND_INDEX

DESCRIPTION
  This function get a free index from the client reg table and
  based on the command type build and queue a generic error response
  message to MMGSDI.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_queue_generic_err_rsp_and_index(
  const mmgsdi_task_cmd_data_type      * task_cmd_data_ptr,
  mmgsdi_return_enum_type                mmgsdi_error_status)
{
  int32                               index           = 0;
  mmgsdi_return_enum_type             mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_client_req_extra_info_type * extra_param_ptr = NULL;

  if (task_cmd_data_ptr == NULL)
    return mmgsdi_status;

  /*-------------------------------------------------------------------------
  build response message and queue onto mmgsdi response q
  mmgsdi_process_response is expecting to have a client request table
  index to free
  -------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Unable to find valid index, 0x%x", mmgsdi_status, 0, 0);
  }
  else
  {
    /*-------------------------------------------------------------------------
    allocate and build extra param ptr
    -------------------------------------------------------------------------*/
    /* Evaluate if this need to be done or not */
    mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                      task_cmd_data_ptr->cmd_enum,
                      &task_cmd_data_ptr->cmd,
                      &extra_param_ptr);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Unable to populate extra param 0x%x", mmgsdi_status, 0, 0);
      return mmgsdi_status;
    }

    mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                      index,
                      &task_cmd_data_ptr->cmd.client_id_reg_req.request_header,
                      NULL,
                      extra_param_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Unable to populate client request 0x%x", mmgsdi_status, 0, 0);
      /* Release the client table */
      mmgsdi_util_free_client_request_table_index(index);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    }
    else
    {
      mmgsdi_status = mmgsdi_build_and_queue_generic_err_rsp(
           task_cmd_data_ptr->cmd.client_id_reg_req.request_header.slot_id,
           index,
           task_cmd_data_ptr->cmd_enum,
           mmgsdi_error_status);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Queue to MMGSDI UIM Report Rsp Unsuccessful",mmgsdi_status,0,0);
        /* Release the client table */
        mmgsdi_util_free_client_request_table_index(index);
      }
    }
  }
  return mmgsdi_status;
} /*mmgsdi_queue_generic_err_rsp_and_index*/


/*===========================================================================
FUNCTION MMGSDI_PROCESS_ACL_OPERATION_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate function for different
  ACL operations.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_acl_operation_cmd (
  const mmgsdi_acl_operation_req_type *acl_op_ptr
)
{
#ifndef FEATURE_MMGSDI_UMTS
  /*lint -esym(715,acl_op_ptr) */
  return MMGSDI_NOT_SUPPORTED;
#else
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
}


/*===========================================================================
FUNCTION MMGSDI_PROCESS_DISABLE_BDN_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate Disable BDN library
  functions.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_disable_bdn_cmd (
  const mmgsdi_bdn_req_type *disable_bdn_req_ptr
)
{
  mmgsdi_protocol_enum_type  protocol           = MMGSDI_NO_PROTOCOL;
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;


  MMGSDIUTIL_RETURN_IF_NULL(disable_bdn_req_ptr);

  /* Check if BDN Is already disabled, if so, fail the command */
  if (gsdi_data.sim_capability.bdn_enabled == FALSE)
  {
    MSG_ERROR("BDN Already disabled, Fail the request", 0, 0, 0);
    return MMGSDI_ERROR;
  }
  /*----------------------------------------------------------------------------
  Get protocol
  NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
  -----------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Problem determining protocol in mmgsdi_process_disable_bdn_cmd",0,0,0);
    return mmgsdi_status;
  } /* mmgsdi_status is not SUCCESS */
  if(protocol == MMGSDI_UICC)
  {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#else
    mmgsdi_status = MMGSDI_ERROR;
#endif /* FEATURE_MMGSDI_UMTS */
  } /* protocol is MMGSDI_UICC */
  else
  {
    mmgsdi_status = mmgsdi_icc_process_bdn(disable_bdn_req_ptr);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMGSDI_PROCESS_ENABLE_BDN_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate Enable BDN library
  functions.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_enable_bdn_cmd (
  const mmgsdi_bdn_req_type *enable_bdn_req_ptr
)
{
  mmgsdi_protocol_enum_type  protocol           = MMGSDI_NO_PROTOCOL;
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(enable_bdn_req_ptr);

  /* Check if BDN Is already enabled, if so, fail the command */
  if (gsdi_data.sim_capability.bdn_enabled == TRUE)
  {
    MSG_ERROR("BDN Already enabled, Fail the request", 0, 0, 0);
    return MMGSDI_ERROR;
  }
  /*----------------------------------------------------------------------------
  Get protocol
  NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
  -----------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Problem determining protocol in mmgsdi_process_enable_bdn_cmd",0,0,0);
    return mmgsdi_status;
  } /* mmgsdi_status is not SUCCESS */
  if(protocol == MMGSDI_UICC)
  {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#else
    mmgsdi_status = MMGSDI_ERROR;
#endif /* FEATURE_MMGSDI_UMTS */
  } /* protocol is MMGSDI_UICC */
  else
  {
    mmgsdi_status = mmgsdi_icc_process_bdn(enable_bdn_req_ptr);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMGSDI_PROCESS_REHABILITATE_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate rehabilitate library
  functions.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_rehabilitate_cmd (
  const mmgsdi_rehabilitate_req_type *rehabilitate_req_ptr
)
{
  mmgsdi_protocol_enum_type  protocol           = MMGSDI_NO_PROTOCOL;
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(rehabilitate_req_ptr);
  /*----------------------------------------------------------------------------
  Get protocol
  NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
  -----------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Problem determining protocol in mmgsdi_process_rehabilitate_cmd",0,0,0);
    return mmgsdi_status;
  } /* mmgsdi_status is not SUCCESS */
  if(protocol == MMGSDI_UICC)
  {
    MSG_ERROR("Rehabilitate not supported for UICC", 0, 0, 0);
    mmgsdi_status = MMGSDI_ERROR;
  } /* protocol is MMGSDI_UICC */
  else
  {
    mmgsdi_status = mmgsdi_icc_rehabilitate(rehabilitate_req_ptr);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMGSDI_PROCESS_PIN_OP_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate PIN Op functions.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_pin_op_cmd (
  const mmgsdi_pin_operation_req_type *pin_op_req_ptr
)
{
  mmgsdi_protocol_enum_type  protocol           = MMGSDI_NO_PROTOCOL;
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;


  MMGSDIUTIL_RETURN_IF_NULL(pin_op_req_ptr);

  /*----------------------------------------------------------------------------
  Get protocol
  NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
  -----------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Problem determining protocol in mmgsdi_process_pin_op_cmd",0,0,0);
    return mmgsdi_status;
  } /* mmgsdi_status is not SUCCESS */
  if(protocol == MMGSDI_UICC)
  {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#else
    mmgsdi_status = MMGSDI_ERROR;
#endif /* FEATURE_MMGSDI_UMTS */
  } /* protocol is MMGSDI_UICC */
  else
  {
    mmgsdi_status = mmgsdi_icc_pin_operation(pin_op_req_ptr);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
}

/*===========================================================================
FUNCTION MMGSDI_PROCESS_GET_ALL_PIN_STATUS_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate Get All Pin Status
  functions.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_get_all_pin_status_cmd (
  const mmgsdi_get_all_pin_status_req_type *get_all_pin_status_req_ptr
)
{
  mmgsdi_protocol_enum_type  protocol           = MMGSDI_NO_PROTOCOL;
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;


  MMGSDIUTIL_RETURN_IF_NULL(get_all_pin_status_req_ptr);

  /*----------------------------------------------------------------------------
  Get protocol
  NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
  -----------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Problem determining protocol in mmgsdi_process_get_all_pin_status_cmd",0,0,0);
    return mmgsdi_status;
  } /* mmgsdi_status is not SUCCESS */
  if(protocol == MMGSDI_UICC)
  {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#else
    mmgsdi_status = MMGSDI_ERROR;
#endif /* FEATURE_MMGSDI_UMTS */
  } /* protocol is MMGSDI_UICC */
  else
  {
    mmgsdi_status = mmgsdi_icc_pin_status(get_all_pin_status_req_ptr);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMGSDI_PROCESS_ACL_OPERATION_RSP

DESCRIPTION
  This function, called in mgmsdi_process_response.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate ACL library
  functions.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_acl_operation_rsp (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr
)
{
  mmgsdi_protocol_enum_type  protocol           = MMGSDI_NO_PROTOCOL;
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_ERROR;

  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  /*----------------------------------------------------------------------------
  Get protocol
  NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
  -----------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Problem determining protocol in mmgsdi_process_acl_operation_rsp",0,0,0);
    return mmgsdi_status;
  } /* mmgsdi_status is not SUCCESS */
  if(protocol == MMGSDI_ICC)
  {
    mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  } /* protocol is MMGSDI_ICC */
  else
  {
#ifndef FEATURE_MMGSDI_GEN_OPTIMIZATION
    mmgsdi_status = mmgsdi_gen_rsp(uim_rsp_ptr);
#endif /* FEATURE_MMGSDI_GEN_OPTIMIZATION*/
  } /* protocol is MMGSDI_UICC */
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMGSDI_PROCESS_REHABILITATE_RSP

DESCRIPTION
  This function, called in mgmsdi_process_response.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate rehabilitate library
  functions.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_rehabilitate_rsp (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr
)
{
  mmgsdi_protocol_enum_type  protocol           = MMGSDI_NO_PROTOCOL;
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  /*----------------------------------------------------------------------------
  Get protocol
  NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
  -----------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Problem determining protocol in mmgsdi_process_rehabilitate_rsp",0,0,0);
    return mmgsdi_status;
  } /* mmgsdi_status is not SUCCESS */
  if(protocol == MMGSDI_UICC)
  {
    MSG_ERROR("Rehabilitate not supported for UICC", 0, 0, 0);
    mmgsdi_status = MMGSDI_ERROR;
  } /* protocol is MMGSDI_UICC */
  else
  {
    mmgsdi_status = mmgsdi_icc_rsp_rehabitate(uim_rsp_ptr, NULL, FALSE);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMGSDI_PROCESS_INVALIDATE_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate invalidate library
  functions.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_invalidate_cmd (
  const mmgsdi_invalidate_req_type *invalidate_req_ptr
)
{
  mmgsdi_protocol_enum_type  protocol           = MMGSDI_NO_PROTOCOL;
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;


  MMGSDIUTIL_RETURN_IF_NULL(invalidate_req_ptr);
  /*----------------------------------------------------------------------------
  Get protocol
  NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
  -----------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Problem determining protocol in mmgsdi_process_invalidate_cmd",0,0,0);
    return mmgsdi_status;
  } /* mmgsdi_status is not SUCCESS */
  if(protocol == MMGSDI_UICC)
  {
    MSG_ERROR("Invalidate not supported for UICC", 0, 0, 0);
    mmgsdi_status = MMGSDI_ERROR;
  } /* protocol is MMGSDI_UICC */
  else
  {
      mmgsdi_status = mmgsdi_icc_invalidate(invalidate_req_ptr);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMGSDI_PROCESS_INVALIDATE_RSP

DESCRIPTION
  This function, called in mgmsdi_process_response.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate invalidate library
  functions.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_invalidate_rsp (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr
)
{
  mmgsdi_protocol_enum_type  protocol           = MMGSDI_NO_PROTOCOL;
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  /*----------------------------------------------------------------------------
  Get protocol
  NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
  -----------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Problem determining protocol in mmgsdi_process_invalidate_rsp",0,0,0);
    return mmgsdi_status;
  } /* mmgsdi_status is not SUCCESS */
  if(protocol == MMGSDI_UICC)
  {
    MSG_ERROR("Rehabilitate not supported for UICC", 0, 0, 0);
    mmgsdi_status = MMGSDI_ERROR;
  } /* protocol is MMGSDI_UICC */
  else
  {
    mmgsdi_status = mmgsdi_icc_rsp_invalidate(uim_rsp_ptr);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMGSDI_PROCESS_PIN_OP_RSP

DESCRIPTION
  This function, called in mmgsdi_process_response.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate pin operation
  functions.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_pin_op_rsp (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr
)
{
  mmgsdi_protocol_enum_type  protocol           = MMGSDI_NO_PROTOCOL;
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  /*----------------------------------------------------------------------------
  Get protocol
  NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
  -----------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Problem determining protocol in mmgsdi_process_pin_op_rsp",0,0,0);
    return mmgsdi_status;
  } /* mmgsdi_status is not SUCCESS */
  if(protocol == MMGSDI_UICC)
  {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_UIM_UICC */
  } /* protocol is MMGSDI_UICC */
  else
  {
      mmgsdi_status = mmgsdi_icc_rsp_pin_operation(uim_rsp_ptr);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMGSDI_PROCESS_GET_ALL_PIN_STATUS_RSP

DESCRIPTION
  This function, called in mmgsdi_process_response.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate get all pin status
  functions.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_get_all_pin_status_rsp (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr
)
{
  mmgsdi_protocol_enum_type  protocol           = MMGSDI_NO_PROTOCOL;
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  /*----------------------------------------------------------------------------
  Get protocol
  NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
  -----------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Problem determining protocol in mmgsdi_process_get_all_pin_status_rsp",0,0,0);
    return mmgsdi_status;
  } /* mmgsdi_status is not SUCCESS */
  if(protocol == MMGSDI_UICC)
  {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_UIM_UICC */
  } /* protocol is MMGSDI_UICC */
  else
  {
      mmgsdi_status = mmgsdi_icc_rsp_pin_status(uim_rsp_ptr);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION mmgsdi_present_in_preinit_file_access_list

DESCRIPTION
  This function, is called from mmgsdi_process_command and will be invoked
  during an attempt to access EFs during SIM initialization.
  This function is important in limiting access to only certain files during
  the initialization process. At present the files include EF DIR, EF ELP
  EF GSM LP and EF CDMA LP.

DEPENDENCIES
  None.

LIMITATIONS
  None.

RETURN VALUE
  boolean
  TRUE          - If file is found,
  FALSE         - If file is not found

SIDE EFFECTS
  NONE
===========================================================================*/
static boolean mmgsdi_present_in_preinit_file_access_list(
    const mmgsdi_task_cmd_data_type *cmd_ptr)
{
  mmgsdi_file_enum_type mmgsdi_filename = MMGSDI_MAX_FILE_ENUM;

  MSG_HIGH("In mmgsdi_present_in_preinit_file_access_list() ",0,0,0);

  if(cmd_ptr == NULL)
  {
    MSG_ERROR("cmd_ptr NULL ",0,0,0);
    return FALSE;
  }

  switch(cmd_ptr->cmd_enum)
  {
    case MMGSDI_READ_REQ:
      switch (cmd_ptr->cmd.read_req.access.access_method)
      {
        case MMGSDI_EF_ENUM_ACCESS:
          mmgsdi_filename = cmd_ptr->cmd.read_req.access.file.file_enum;
          break;
        case MMGSDI_DF_ENUM_ACCESS:
        case MMGSDI_BY_PATH_ACCESS:
        case MMGSDI_BY_APP_ID_ACCESS:
        case MMGSDI_MAX_ACCESS_METHOD_ENUM:
          MSG_HIGH("access_method currently not supported 0x%x",
                    cmd_ptr->cmd.read_req.access.access_method,0,0);
          break;
        default:
          break;
      }/* End of switch (cmd_ptr->cmd.read_req.access.access_method) */
      break;

    case MMGSDI_WRITE_REQ:
      switch (cmd_ptr->cmd.write_req.access.access_method)
      {
        case MMGSDI_EF_ENUM_ACCESS:
          mmgsdi_filename = cmd_ptr->cmd.write_req.access.file.file_enum;
          break;
        case MMGSDI_DF_ENUM_ACCESS:
        case MMGSDI_BY_PATH_ACCESS:
        case MMGSDI_BY_APP_ID_ACCESS:
        case MMGSDI_MAX_ACCESS_METHOD_ENUM:
          MSG_HIGH("access_method currently not supported 0x%x",
                    cmd_ptr->cmd.write_req.access.access_method,0,0);
          break;
        default:
          break;
      }/* switch (cmd_ptr->cmd.write_req.access.access_method) */
      break;

     case MMGSDI_GET_FILE_ATTR_REQ:
      switch (cmd_ptr->cmd.get_file_attr_req.access.access_method)
      {
        case MMGSDI_EF_ENUM_ACCESS:
          mmgsdi_filename = cmd_ptr->cmd.get_file_attr_req.access.file.file_enum;
          break;
        case MMGSDI_DF_ENUM_ACCESS:
        case MMGSDI_BY_PATH_ACCESS:
        case MMGSDI_BY_APP_ID_ACCESS:
        case MMGSDI_MAX_ACCESS_METHOD_ENUM:
          MSG_HIGH("access_method currently not supported 0x%x",
                    cmd_ptr->cmd.get_file_attr_req.access.access_method,0,0);
          break;
        default:
          break;
      }/* End of switch (cmd_ptr->cmd.get_file_attr_req.access.access_method) */
      break;

    case MMGSDI_SEARCH_REQ:
      switch (cmd_ptr->cmd.search_req.access.access_method)
      {
        case MMGSDI_EF_ENUM_ACCESS:
          mmgsdi_filename = cmd_ptr->cmd.search_req.access.file.file_enum;
          break;
        case MMGSDI_DF_ENUM_ACCESS:
        case MMGSDI_BY_PATH_ACCESS:
        case MMGSDI_BY_APP_ID_ACCESS:
        case MMGSDI_MAX_ACCESS_METHOD_ENUM:
          MSG_HIGH("access_method currently not supported 0x%x",
                   cmd_ptr->cmd.search_req.access.access_method,0,0);
          break;

        default:
          break;
      }/* End of switch (cmd_ptr->cmd.read_req.access.access_method) */
      break;

    default:
      break;
  }/* End of switch(cmd_ptr->cmd.cmd_enum) */

  switch(mmgsdi_filename)
  {
    case MMGSDI_ELP:
    case MMGSDI_GSM_ECC:
    case MMGSDI_USIM_ECC:
    case MMGSDI_CDMA_ECC:
    case MMGSDI_DIR:
    case MMGSDI_GSM_LP:
    case MMGSDI_ICCID:
    case MMGSDI_CDMA_PREF_LANG:
      MSG_HIGH("Return TRUE for 0x%x", mmgsdi_filename,0,0);
      return TRUE;
    default:
      break;
  }
  return FALSE;
}/* mmgsdi_present_in_preinit_file_access_list */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_COMMAND

DESCRIPTION
  This function, called in gsdimain, is called to process an mmgsdi command.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate library function.

DEPENDENCIES
  gsdimain must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_command(
  mmgsdi_task_cmd_type           * task_cmd_ptr
)
{
  /*-----------------------------------------------------------------------------
  Initialize variables
  -----------------------------------------------------------------------------*/
  mmgsdi_return_enum_type             mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_protocol_enum_type           protocol           = MMGSDI_NO_PROTOCOL;
  boolean                             proceed_processing = TRUE;
  boolean                             cond_processing    = FALSE;

  /*----------------------------------------------------------------------------
  Validate the parameters provided
  ----------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(task_cmd_ptr);

  MSG_HIGH("MMGSDI_PROCESS_COMMAND",0,0,0);

  if (mmgsdi_is_cmd_slot_specific(task_cmd_ptr->cmd.cmd_enum))
  {
    if (mmgsdi_can_proceed_cmd_with_slot(
        task_cmd_ptr->cmd.cmd_enum,
        task_cmd_ptr->cmd.cmd.client_id_reg_req.request_header.slot_id,
        &cond_processing))
    {
      proceed_processing = TRUE;
      if(cond_processing == TRUE)
      {
        if(mmgsdi_present_in_preinit_file_access_list(
           &task_cmd_ptr->cmd) == FALSE)
        {
#ifdef FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
          MSG_HIGH("proceed_processing FALSE", 0, 0, 0);
          proceed_processing = FALSE;
#else
          MSG_HIGH("Access rule check for EF access is skipped", 0, 0, 0);
#endif /* FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES */
        }
      }/* if(cond_processing == TRUE) */
    }
    else
    {
      proceed_processing = FALSE;
    }
  }
  else
  {
    proceed_processing = TRUE;
  }

  if (!proceed_processing)
  {

    MSG_ERROR("Unable to proceed with command 0x%x", task_cmd_ptr->cmd.cmd_enum, 0, 0);
    MSG_ERROR("Use static cnf buffer to build cnf notification", 0, 0, 0);

    mmgsdi_status = mmgsdi_queue_generic_err_rsp_and_index(
                      &task_cmd_ptr->cmd,
                      MMGSDI_ACCESS_DENIED);

    if (mmgsdi_status != MMGSDI_SUCCESS)
      MSG_ERROR("Impossible to return cnf back to client", 0, 0, 0);
    return mmgsdi_status;
  }

  /*----------------------------------------------------------------------------
  Dispatch commmand to proper technology/command handling function.
  -----------------------------------------------------------------------------*/
  switch(task_cmd_ptr->cmd.cmd_enum)
  {
    case MMGSDI_READ_REQ:
      MSG_HIGH("Received MMGSDI_READ_REQ",0,0,0);
      /*----------------------------------------------------------------------------
      Get protocol
      NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
      -----------------------------------------------------------------------------*/
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */
      if(protocol == MMGSDI_UICC)
      {
        /* can this be featurized */
        #ifdef FEATURE_MMGSDI_UMTS
#error code not present
        #else
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
        #endif /* FEATURE_MMGSDI_UMTS */
      } /* protocol is MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_read(&task_cmd_ptr->cmd.cmd.read_req);
      } /* protocol is MMGSDI_ICC */
      break; /* end case - MMGSDI_READ_REQ */

    case MMGSDI_WRITE_REQ:
      MSG_HIGH("Received MMGSDI_WRITE_REQ",0,0,0);
      /*----------------------------------------------------------------------------
      Get protocol
      NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
      -----------------------------------------------------------------------------*/
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */
      if(protocol == MMGSDI_UICC)
      {
        #ifdef FEATURE_MMGSDI_UMTS
#error code not present
        #else
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
        #endif /* FEATURE_MMGSDI_UMTS */
      } /* protocol is MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_write(&task_cmd_ptr->cmd.cmd.write_req);
      } /* protocol is MMGSDI_ICC */
      break; /* end case - MMGSDI_WRITE_REQ */

    case MMGSDI_GET_FILE_ATTR_REQ:
      MSG_HIGH("Received MMGSDI_GET_FILE_ATTR_REQ",0,0,0);
      /*----------------------------------------------------------------------------
      Get protocol
      NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
      -----------------------------------------------------------------------------*/
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */
      if(protocol == MMGSDI_UICC)
      {
        #ifdef FEATURE_MMGSDI_UMTS
#error code not present
        #else
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
        #endif /* FEATURE_MMGSDI_UMTS */
      } /* protocol is MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_get_file_attr(&task_cmd_ptr->cmd.cmd.get_file_attr_req);
      } /* protocol is MMGSDI_ICC */
      break; /* end case - MMGSDI_GET_FILE_ATTRIBUTES */

    case MMGSDI_CLIENT_ID_REG_REQ:
    case MMGSDI_CLIENT_EVT_REG_REQ:
    case MMGSDI_CLIENT_INIT_COMP_REQ:
      MSG_HIGH("Received MMGSDI_CLIENT_ID/EVT_REG_REQ or MMGSDI_CLIENT_INIT_COMP_REQ 0x%x",
        task_cmd_ptr->cmd.cmd_enum,0,0);
      mmgsdi_status = mmgsdi_process_client_reg_req(&task_cmd_ptr->cmd.cmd.client_id_reg_req);
      break; /* end case - MMGSDI_CLIENT_ID_REG_REQ,  MMGSDI_CLIENT_EVT_REG_REQ*/

    case MMGSDI_CLIENT_ID_DEREG_REQ:
    case MMGSDI_CLIENT_EVT_DEREG_REQ:
      MSG_HIGH("Received MMGSDI_CLIENT_ID/EVT_DEREG_REQ 0x%x",task_cmd_ptr->cmd.cmd_enum,0,0);
      mmgsdi_status = mmgsdi_process_client_dereg_req(&task_cmd_ptr->cmd.cmd.client_id_dereg_req);
      break; /* end case - MMGSDI_CLIENT_ID_DEREG_REQ,  MMGSDI_CLIENT_EVT_DEREG_REQ*/

    case MMGSDI_SESSION_OPEN_REQ:
      MSG_HIGH("Received MMGSDI_SESSION_OPEN_REQ",0,0,0);
#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
      mmgsdi_status = mmgsdi_session_main(&task_cmd_ptr->cmd);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_SESSION_MANAGEMENT */
      break; /* end case - MMGSDI_SESSION_OPEN_REQ */

    case MMGSDI_SESSION_DEACTIVATE_REQ:
      MSG_HIGH("Received MMGSDI_SESSION_DEACTIVATE_REQ",0,0,0);
#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
      mmgsdi_status = mmgsdi_session_main(&task_cmd_ptr->cmd);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_SESSION_MANAGEMENT */
      break; /* end case - MMGSDI_SESSION_DEACTIVATE_REQ */

    case MMGSDI_SESSION_SELECT_AID_REQ:
      MSG_HIGH("Received MMGSDI_SESSION_SELECT_AID_REQ",0,0,0);
#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
       mmgsdi_status = mmgsdi_session_main(&task_cmd_ptr->cmd);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_SESSION_MANAGEMENT */
      break; /* end case - MMGSDI_SESSION_SELECT_AID_REQ */

    case MMGSDI_SESSION_CLOSE_REQ:
      MSG_HIGH("Received MMGSDI_SESSION_CLOSE_REQ",0,0,0);
#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
      mmgsdi_status = mmgsdi_session_main(&task_cmd_ptr->cmd);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_SESSION_MANAGEMENT */
      break; /* end case - MMGSDI_SESSION_CLOSE_REQ */

    case MMGSDI_ISIM_AUTH_REQ:
      MSG_HIGH("Received MMGSDI_ISIM_AUTH_REQ",0,0,0);
#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
      mmgsdi_status = mmgsdi_session_main(&task_cmd_ptr->cmd);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_SESSION_MANAGEMENT */
      break; /* end case - MMGSDI_ISIM_AUTH_REQ */


    case MMGSDI_CARD_PUP_REQ:
      MSG_HIGH("Received MMGSDI_CARD_PUP_REQ",0,0,0);
#ifndef  FEATURE_MMGSDI_GEN_OPTIMIZATION
      mmgsdi_status = mmgsdi_gen_process_card_pup(&task_cmd_ptr->cmd.cmd.card_pup_req);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /*  FEATURE_MMGSDI_GEN_OPTIMIZATION */
      break; /* end case - MMGSDI_CARD_PUP_REQ */

    case MMGSDI_CARD_PDOWN_REQ:
      MSG_HIGH("Received MMGSDI_CARD_PDOWN_REQ",0,0,0);
#ifndef  FEATURE_MMGSDI_GEN_OPTIMIZATION
      mmgsdi_status = mmgsdi_gen_process_card_pdown(&task_cmd_ptr->cmd.cmd.card_pdown_req);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /*  FEATURE_MMGSDI_GEN_OPTIMIZATION */
      break; /* end case - MMGSDI_CARD_PDOWN_REQ */

    case MMGSDI_CARD_RESET_REQ:
      MSG_HIGH("Received MMGSDI_CARD_RESET_REQ",0,0,0);
#ifndef  FEATURE_MMGSDI_GEN_OPTIMIZATION
      mmgsdi_status = mmgsdi_gen_process_card_reset(&task_cmd_ptr->cmd.cmd.card_reset_req);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /*  FEATURE_MMGSDI_GEN_OPTIMIZATION */
      break; /* end case - MMGSDI_CARD_RESET_REQ */

    case MMGSDI_SAP_CONNECT_REQ:
      MSG_HIGH("Received MMGSDI_SAP_CONNECT_REQ",0,0,0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
    break;

    case MMGSDI_SAP_DISCONNECT_REQ:
      MSG_HIGH("Received MMGSDI_SAP_DISCONNECT_REQ",0,0,0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
    break;

    case MMGSDI_SAP_GET_ATR_REQ:
      MSG_HIGH("Received MMGSDI_SAP_GET_ATR_REQ",0,0,0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
      break;  /* end case - MMGSDI_SAP_GET_ATR_REQ */

    case MMGSDI_SAP_SEND_APDU_REQ:
      MSG_HIGH("Received MMGSDI_SAP_SEND_APDU_REQ",0,0,0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
      break; /* end case - MMGSDI_SAP_SEND_APDU_REQ */

    case MMGSDI_SAP_POWER_ON_REQ:
      MSG_HIGH("Received MMGSDI_SAP_POWER_ON_REQ",0,0,0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
      break; /* end case - MMGSDI_SAP_POWER_ON_REQ */

    case MMGSDI_SAP_POWER_OFF_REQ:
      MSG_HIGH("Received MMGSDI_SAP_POWER_OFF_REQ",0,0,0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
      break; /* end case - MMGSDI_SAP_POWER_OFF_REQ */

    case MMGSDI_SAP_CARD_READER_STATUS_REQ:
      MSG_HIGH("Received MMGSDI_SAP_CARD_READER_STATUS_REQ",0,0,0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
      break; /* end case - MMGSDI_SAP_CARD_READER_STATUS_REQ */

    case MMGSDI_SAP_RESET_REQ:
      MSG_HIGH("Received MMGSDI_SAP_RESET_REQ",0,0,0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
      break; /* end case - MMGSDI_SAP_RESET_REQ */

    case MMGSDI_SAP_INIT_REQ:
      MSG_HIGH("Received MMGSDI_SAP_INIT_REQ",0,0,0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
      break; /* end case - MMGSDI_SAP_INIT_REQ */

    case MMGSDI_SAP_DEREG_REQ:
      MSG_HIGH("Received MMGSDI_SAP_DEREG_REQ",0,0,0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
      break; /* end case - MMGSDI_SAP_DEREG_REQ */

    case MMGSDI_SAP_PIN_VERIFY_REQ:
      MSG_HIGH("Received MMGSDI_SAP_PIN_VERIFY_REQ",0,0,0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP*/
      break; /* end case - MMGSDI_SAP_PIN_VERIFY_REQ */

    case MMGSDI_CARD_STATUS_REQ:
      MSG_HIGH("Received MMGSDI_CARD_STATUS_REQ",0,0,0);
      /*----------------------------------------------------------------------------
      Get protocol
      NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
      -----------------------------------------------------------------------------*/
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */
      if(protocol == MMGSDI_UICC)
      {
        #ifdef FEATURE_MMGSDI_UMTS
#error code not present
        #else
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
        #endif /* FEATURE_MMGSDI_UMTS */
      } /* protocol is MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_status(&task_cmd_ptr->cmd.cmd.status_req);
      } /* protocol is MMGSDI_ICC */
      break; /* end case - MMGSDI_CARD_STATUS_REQ */

    case MMGSDI_REFRESH_REQ:
      MSG_HIGH("Received MMGSDI_REFRESH_REQ command cmd =0x%x",task_cmd_ptr->cmd.cmd_enum,0,0);
      mmgsdi_status = mmgsdi_refresh_main(task_cmd_ptr);
      break; /* end case - MMGSDI_REFRESH_REQ */

    case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */
      break;

    case MMGSDI_DISABLE_BDN_REQ:
      MSG_HIGH("Received MMGSDI_DISABLE_BDN_REQ",0,0,0);
      mmgsdi_status = mmgsdi_process_disable_bdn_cmd(&task_cmd_ptr->cmd.cmd.disable_bdn_req);
      break; /* end case - MMGSDI_DISABLE_BDN_REQ */

    case MMGSDI_ENABLE_BDN_REQ:
      MSG_HIGH("Received MMGSDI_ENABLE_BDN_REQ",0,0,0);
      mmgsdi_status = mmgsdi_process_enable_bdn_cmd(&task_cmd_ptr->cmd.cmd.enable_bdn_req);
      break; /* end case - MMGSDI_ENABLE_BDN_REQ */

    case MMGSDI_ACL_OPERATION_REQ:
      MSG_HIGH("Received MMGSDI_ACL_OPERATION_REQ",0,0,0);
      mmgsdi_status = mmgsdi_process_acl_operation_cmd(
                        &task_cmd_ptr->cmd.cmd.acl_operation_req);
      break; /* end case - MMGSDI_ACL_OPERATION_REQ */

    case MMGSDI_REHABILITATE_REQ:
      MSG_HIGH("Received MMGSDI_REHABILITATE_REQ",0,0,0);
      mmgsdi_status = mmgsdi_process_rehabilitate_cmd(&task_cmd_ptr->cmd.cmd.rehab_req);
      break; /* end case - MMGSDI_REHABILITATE_REQ */

    case MMGSDI_INVALIDATE_REQ:
      MSG_HIGH("Received MMGSDI_INVALIDATE_REQ",0,0,0);
      mmgsdi_status = mmgsdi_process_invalidate_cmd(&task_cmd_ptr->cmd.cmd.invalidate_req);
      break; /* end case - MMGSDI_INVALIDATE_REQ */

    case MMGSDI_SEND_APDU_REQ:
      MSG_HIGH("Received MMGSDI_SEND_APDU_REQ",0,0,0);
#if (defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)) && !defined(FEATURE_MMGSDI_GEN_OPTIMIZATION)
      mmgsdi_status = mmgsdi_gen_process_send_apdu(&task_cmd_ptr->cmd.cmd.send_apdu_req);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)) && !FEATURE_MMGSDI_GEN_OPTIMIZATION */
      break;

    case MMGSDI_PIN_OPERATION_REQ:
      MSG_HIGH("Received MMGSDI_PIN_OPERATION_REQ",0,0,0);
      mmgsdi_status = mmgsdi_process_pin_op_cmd(&task_cmd_ptr->cmd.cmd.pin_operation_req);
      break; /* end case - MMGSDI_PIN_OPERATION_REQ */

    case MMGSDI_SELECT_AID_REQ:
      MSG_HIGH("Received MMGSDI_SELECT_AID_REQ",0,0,0);
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /*FEATURE_MMGSDI_UMTS*/
      break;

    case MMGSDI_GET_ALL_PIN_STATUS_REQ:
      MSG_HIGH("Received MMGSDI_GET_ALL_PIN_STATUS_REQ",0,0,0);
      mmgsdi_status = mmgsdi_process_get_all_pin_status_cmd(&task_cmd_ptr->cmd.cmd.pin_status_req);
      break; /* end case - MMGSDI_PIN_OPERATION_REQ */

    case MMGSDI_SRV_AVAILABLE_REQ:
      MSG_HIGH("Received MMGSDI_SRV_AVAILABLE_REQ",0,0,0);
#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /*FEATURE_MMGSDI_MBMS*/
      break; /* end case - MMGSDI_SRV_AVAILABLE_REQ */

    case MMGSDI_USIM_AUTH_REQ:
      MSG_HIGH("Received MMGSDI_USIM_AUTH_REQ",0,0,0);
#if defined(FEATURE_MMGSDI_MBMS) && defined (FEATURE_MMGSDI_UMTS)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /*defined(FEATURE_MMGSDI_MBMS) && defined (FEATURE_MMGSDI_UMTS)*/
      break; /* end case - MMGSDI_USIM_AUTH_REQ */

    case MMGSDI_RUN_CAVE_REQ:
      MSG_HIGH("Received MMGSDI_RUN_CAVE_REQ",0,0,0);
#ifdef FEATURE_MMGSDI_CDMA
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */
      if(protocol == MMGSDI_ICC)
      {
        #ifdef FEATURE_AUTH
        mmgsdi_status = mmgsdi_icc_run_cave(&task_cmd_ptr->cmd.cmd.run_cave_req);
        #else
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
        #endif /* FEATURE_AUTH */
      }
      else
      {
        MSG_ERROR("Incorrect Protocol for RUN CAVE",0,0,0);
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      }
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_CDMA */
      break; /* end case - MMGSDI_RUN_CAVE_REQ */

    case MMGSDI_GENERATE_KEY_VPM_REQ:
      MSG_HIGH("Received MMGSDI_GENERATE_KEY_VPM_REQ",0,0,0);
#ifdef FEATURE_MMGSDI_CDMA
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */
      if(protocol == MMGSDI_ICC)
      {
        #ifdef FEATURE_AUTH
        mmgsdi_status = mmgsdi_icc_generate_key_vpm(&task_cmd_ptr->cmd.cmd.generate_key_req);
        #else
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
        #endif /* FEATURE_AUTH */
      }
      else
      {
        MSG_ERROR("Incorrect Protocol for GENERATE KEY VPM, should be ICC",0,0,0);
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      }
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_CDMA */
      break; /* end case - MMGSDI_GENERATE_KEY_VPM_REQ */

    case MMGSDI_COMPUTE_IP_AUTH_REQ:
      MSG_HIGH("Received MMGSDI_COMPUTE_IP_AUTH_REQ",0,0,0);
#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined (FEATURE_UIM_SUPPORT_HRPD_AN)
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */
      if(protocol == MMGSDI_ICC)
      {
        mmgsdi_status = mmgsdi_icc_compute_ip_auth(&task_cmd_ptr->cmd.cmd.compute_ip_req);
      }
      else
      {
        MSG_ERROR("Incorrect Protocol for COMPUTE IP AUTH",0,0,0);
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      }
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */
      break; /* end case - MMGSDI_COMPUTE_IP_AUTH_REQ */

    case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
      MSG_HIGH("Received MMGSDI_JCDMA_GET_CARD_INFO_REQ", 0,0,0);
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT

      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */

      if(protocol == MMGSDI_ICC)
      {
        mmgsdi_status = mmgsdi_process_jcdma_card_get_info_cmd(&task_cmd_ptr->cmd.cmd.jcdma_card_info_req);
      }
      else
      {
        MSG_ERROR("JCDMA Incorrect Protocol",0,0,0);
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      }
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */
      break; /* end case - MMGSDI_JCDMA_GET_CARD_INFO_REQ */

    case MMGSDI_GET_ATR_REQ:
      MSG_HIGH("Received MMGSDI_GET_ATR_REQ ",0,0,0);
      mmgsdi_status = mmgsdi_gen_process_get_atr(MMGSDI_GET_ATR_REQ, &task_cmd_ptr->cmd.cmd.get_atr_req);
      break; /* end case - MMGSDI_GET_ATR_REQ */

    case MMGSDI_CARD_READER_STATUS_REQ:
      MSG_HIGH("Received MMGSDI_CARD_READER_STATUS_REQ ",0,0,0);
      mmgsdi_status = mmgsdi_gen_process_card_reader_status(MMGSDI_CARD_READER_STATUS_REQ,
                                                            &task_cmd_ptr->cmd.cmd.card_reader_status_req);
      break; /* end case - MMGSDI_CARD_READER_STATUS_REQ */

    case MMGSDI_MFLO_INIT_ACT_PARAMS_REQ:
    case MMGSDI_MFLO_GET_SUBSCRIBER_ID_REQ:
    case MMGSDI_MFLO_GET_PUBLIC_KEY_REQ:
    case MMGSDI_MFLO_SIGN_UCAST_REQ:
    case MMGSDI_MFLO_VERIFY_UCAST_REQ:
      MSG_HIGH("Received MMGSDI_MFLO REQ 0x%x ",task_cmd_ptr->cmd.cmd_enum,0,0);
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#else
      MSG_HIGH(" FEATURE_MMGSDI_MFLO not defined ",0,0,0);
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_MFLO */
      break;

    case MMGSDI_BCAST_REQ:
      MSG_HIGH("Received MMGSDI_BCAST_REQ",0,0,0);
#if defined(FEATURE_MMGSDI_MBMS) && defined (FEATURE_MMGSDI_UMTS)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /*defined(FEATURE_MMGSDI_MBMS) && defined (FEATURE_MMGSDI_UMTS)*/
      break; /* end case - MMGSDI_BCAST_REQ */

    case MMGSDI_SEARCH_REQ:
      MSG_HIGH("Received SEARCH command ",0,0,0);
      /*----------------------------------------------------------------------------
      Get protocol
      NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
      -----------------------------------------------------------------------------*/
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */

      if(protocol == MMGSDI_UICC)
      {
        /* can this be featurized */
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#else
        mmgsdi_status =  MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_UMTS */
      } /* protocol is MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_search(&task_cmd_ptr->cmd.cmd.search_req);
      } /* protocol is MMGSDI_ICC */
      break;

    case MMGSDI_CREATE_PKCS15_TABLE_REQ:
      MSG_HIGH("Received Create PKCS15 lookup table command from client ",0,0,0);
#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* defined FEATURE_MMGSDI_PKCS15 */
      break; /* end case - MMGSDI_CREATE_PKCS15_TABLE_REQ */
    case MMGSDI_SAP_REPORT_RSP:
    case MMGSDI_UIM_REPORT_RSP:
    case MMGSDI_CLIENT_ID_REG_RSP:
    case MMGSDI_CNF:
    case MMGSDI_MAX_CMD_ENUM:
      /* Cmd not to be handled by process_command */
      MSG_ERROR("Command not be processed by mmgsdi_process_command", 0, 0, 0);
      return MMGSDI_ERROR;

  } /* end switch - command type */

  if( (mmgsdi_status != MMGSDI_SUCCESS) &&
      (mmgsdi_status != MMGSDI_REFRESH_IN_PROGRESS) )
  {
    MSG_ERROR("Error in mmgsdi_process_command, Start internally building rsp",
      0,0,0);
     mmgsdi_status = mmgsdi_queue_generic_err_rsp_and_index(
                       &task_cmd_ptr->cmd,
                       mmgsdi_status);

    if (mmgsdi_status != MMGSDI_SUCCESS)
      MSG_ERROR("Impossible to return cnf back to client", 0, 0, 0);

  } /* end if - mmgsdi_status is not SUCCESS */
  return mmgsdi_status;
} /* mmgsdi_process_command */


/*===========================================================================
FUNCTION MMGSDI_BUILD_QUEUE_STATIC_CNF

DESCRIPTION
  This function, builds a default confirmation response in case of an error
  function.

DEPENDENCIES


LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_build_and_queue_static_cnf(
  int32                    index,
  mmgsdi_return_enum_type  status)
{
  mmgsdi_return_enum_type                mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_request_header_type             req_header;
  uim_cmd_type                         * uim_cmd_ptr          = NULL;
  mmgsdi_client_req_extra_info_type    * extra_param_ptr      = NULL;

  MSG_ERROR("Building Cnf from Static Task Cmd Buf 0x%x", status, 0, 0);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Error in getting info fr client req table info",
                             0, 0, 0);
    return mmgsdi_status;
  }
  memset(&mmgsdi_static_task_buf.cmd.cmd.mmgsdi_cnf, 0x00, sizeof(mmgsdi_task_cnf_type));

  mmgsdi_static_task_buf.cmd.cmd_enum = MMGSDI_CNF;

  mmgsdi_cnf_map_req_to_cnf_enum_type(
    req_header.orig_request_type,
    &mmgsdi_static_task_buf.cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.response_header.response_type);

  mmgsdi_static_task_buf.cmd.cmd.mmgsdi_cnf.client_req_cb                            = req_header.response_cb;
  mmgsdi_static_task_buf.cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.response_header.client_data   = req_header.client_data;
  mmgsdi_static_task_buf.cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.response_header.client_id     = req_header.client_id;
  mmgsdi_static_task_buf.cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.response_header.slot_id       = req_header.slot_id;
  mmgsdi_static_task_buf.cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.response_header.mmgsdi_status = status;

  mmgsdi_static_task_buf.cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.response_header.status_word.present = FALSE;
  mmgsdi_static_task_buf.cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.response_header.status_word.sw1     = 0x00;
  mmgsdi_static_task_buf.cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.response_header.status_word.sw2     = 0x00;

  mmgsdi_static_task_buf.cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.write_cnf.response_header.response_len =
                     uint32toint32(sizeof(mmgsdi_response_header_type));

  mmgsdi_static_task_buf.cmd.cmd.mmgsdi_cnf.mmgsdi_cnf.response_header.payload_len = 0;
  rex_enter_crit_sect(&mmgsdi_crit_sect);
  (void)q_link(&mmgsdi_static_task_buf, &mmgsdi_static_task_buf.link);
  /* Put the message on the queue */
  q_put(&mmgsdi_task_rsp_q, &mmgsdi_static_task_buf.link);

  /* Set the command queue signal for */
  /* receive task                     */
  (void) rex_set_sigs(&gsdi_task_tcb, MMGSDI_TASK_RSP_Q_SIG);
  rex_leave_crit_sect(&mmgsdi_crit_sect);
  return MMGSDI_SUCCESS;
} /* mmgsdi_build_and_queue_static_cnf */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_RESPONSE

DESCRIPTION
  This function, called in gsdimain, is called to process an mmgsdi response.
  It will determine which card technology the command requires (UICC or ICC),
  determine the response type, and call the appropriate response processing
  function.

DEPENDENCIES
  gsdimain must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_response(
  const mmgsdi_task_cmd_type *  task_cmd_ptr /* GSDI response to be processed */
)
{

/*-----------------------------------------------------------------------------
  Initialize variables
-----------------------------------------------------------------------------*/
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_protocol_enum_type protocol      = MMGSDI_NO_PROTOCOL;
  mmgsdi_cmd_enum_type      cmd_req_enum  = MMGSDI_MAX_CMD_ENUM;
  int32                     index         = 0;
  boolean                   free_index    = TRUE;

  MSG_HIGH ("MMGSDI_PROCESS_RESPONSE", 0, 0, 0);

  MMGSDIUTIL_RETURN_IF_NULL(task_cmd_ptr);

  /* Obtain the index and check the request type */
  switch(task_cmd_ptr->cmd.cmd_enum)
  {
    case MMGSDI_CLIENT_ID_REG_RSP:
      index = task_cmd_ptr->cmd.cmd.client_id_reg_rsp.rsp_data_index;
      break;

    case MMGSDI_UIM_REPORT_RSP:
      index = task_cmd_ptr->cmd.cmd.uim_report_rsp.rsp_data_index;
      break;

    default:
      MSG_ERROR("Incorrect cmd enum type in process response 0x%x", task_cmd_ptr->cmd.cmd_enum, 0, 0);
      return MMGSDI_ERROR;
  }

  if( ( index < 0 ) ||
      ( index >= MMGSDI_MAX_CLIENT_INFO ))
  {
    MSG_ERROR("Unable to index 0x%x", index, 0, 0);
    return MMGSDI_ERROR;
  }

  cmd_req_enum = mmgsdi_client_req_table_info[index].request_type;

  /*----------------------------------------------------------------------------
    Dispatch to proper technology/command response handling function
  -----------------------------------------------------------------------------*/
  switch(cmd_req_enum)
  {
    case MMGSDI_READ_REQ:
      MSG_HIGH("Received READ response",0,0,0);
      /*----------------------------------------------------------------------------
      Get protocol
      NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
      -----------------------------------------------------------------------------*/
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */
      if(protocol == MMGSDI_UICC)
      {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#else
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_UMTS */

      } /* end if - MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_rsp_read(&task_cmd_ptr->cmd.cmd.uim_report_rsp,
                                            NULL, FALSE);
      } /* end else - MMGSDI_ICC */
      break; /* end case - MMGSDI_READ_REQ*/

    case MMGSDI_WRITE_REQ:
      MSG_HIGH("Received WRITE response",0,0,0);
      /*----------------------------------------------------------------------------
      Get protocol
      NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
      -----------------------------------------------------------------------------*/
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */
      if(protocol == MMGSDI_UICC)
      {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#else
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_UMTS */
      } /* end if - MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_rsp_write(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      } /* end else - MMGSDI_ICC */
      break; /* end case - MMGSDI_WRITE_REQ*/

    case MMGSDI_GET_FILE_ATTR_REQ:
      MSG_HIGH("Received GET FILE ATTR response",0,0,0);
      /*----------------------------------------------------------------------------
      Get protocol
      NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
      -----------------------------------------------------------------------------*/
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */
      if(protocol == MMGSDI_UICC)
      {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#else
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_UMTS */
      } /* end if - MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_rsp_get_file_attr(
                          &task_cmd_ptr->cmd.cmd.uim_report_rsp, NULL, FALSE);
      } /* end else - MMGSDI_ICC */
      break; /* end case - MMGSDI_GET_FILE_ATTRIBUTES */

    case MMGSDI_CLIENT_ID_REG_REQ:
    case MMGSDI_CLIENT_EVT_REG_REQ:
    case MMGSDI_CLIENT_INIT_COMP_REQ:
      MSG_HIGH("Received CLIENT ID/EVT REG / INIT_COMP 0x%x response",cmd_req_enum,0,0);
      /* cnf_enum if being set in the process function */
      mmgsdi_status = mmgsdi_process_client_reg_cnf(&task_cmd_ptr->cmd.cmd.client_id_reg_rsp);
      break; /* end case - MMGSDI_CLIENT_ID_REG_REQ, MMGSDI_CLIENT_EVT_REG_REQ,
                           MMGSDI_CLIENT_INIT_COMP_REQ */

    case MMGSDI_CLIENT_EVT_DEREG_REQ:
    case MMGSDI_CLIENT_ID_DEREG_REQ:
      MSG_HIGH("Received CLIENT ID/EVT DEREG response",0,0,0);
      /* cnf_enum if being set in the process function */
      mmgsdi_status = mmgsdi_process_client_dereg_cnf(&task_cmd_ptr->cmd.cmd.client_id_reg_rsp);
      break; /* end case - MMGSDI_CLIENT_ID_DEREG_REQ, MMGSDI_CLIENT_EVT_DEREG_REQ */

    case MMGSDI_SESSION_OPEN_REQ:
    case MMGSDI_SESSION_SELECT_AID_REQ:
    case MMGSDI_SESSION_DEACTIVATE_REQ:
    case MMGSDI_SESSION_CLOSE_REQ:
    case MMGSDI_ISIM_AUTH_REQ:
      MSG_HIGH("Received SESSION 0x%x response",cmd_req_enum,0,0);
#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
      mmgsdi_status = mmgsdi_session_main_response(cmd_req_enum, &task_cmd_ptr->cmd.cmd.uim_report_rsp);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_SESSION_MANAGEMENT */
      break; /* end case - MMGSDI_SESSION_XXX_REQ */

    case MMGSDI_SAP_CONNECT_REQ:
      MSG_HIGH("Received SAP_CONNECT response", 0, 0, 0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
      break; /* end case - MMGSDI_SAP_CONNECT_REQ*/

    case MMGSDI_SAP_GET_ATR_REQ:
      MSG_HIGH("Received SAP_GET_ATR response", 0, 0, 0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP) && !defined (FEATURE_MMGSDI_GEN_OPTIMIZATION)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP && !FEATURE_MMGSDI_GEN_OPTIMIZATION */
    break; /* end case - MMGSDI_SAP_GET_ATR_REQ*/

    case MMGSDI_SAP_SEND_APDU_REQ:
      MSG_HIGH("Received SAP_SEND_APDU response", 0, 0, 0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break; /* end case - MMGSDI_SAP_SEND_APDU_REQ*/

    case MMGSDI_SAP_POWER_ON_REQ:
      MSG_HIGH("Received SAP_POWER_ON response", 0, 0, 0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break; /* end case - MMGSDI_SAP_POWER_ON_REQ*/

    case MMGSDI_SAP_POWER_OFF_REQ:
      MSG_HIGH("Received SAP_POWER_OFF response", 0, 0, 0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
       break; /* end case - MMGSDI_SAP_POWER_OFF_REQ*/

    case MMGSDI_SAP_CARD_READER_STATUS_REQ:
      MSG_HIGH("Received SAP_CARD_READER_STATUS response ", 0, 0, 0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP) && !defined (FEATURE_MMGSDI_GEN_OPTIMIZATION)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP && !FEATURE_MMGSDI_GEN_OPTIMIZATION*/
      break; /* end case - MMGSDI_SAP_CARD_READER_STATUS_REQ*/

    case MMGSDI_SAP_RESET_REQ:
      MSG_HIGH("Received SAP_RESET response ", 0, 0, 0);
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
      break; /* end case - MMGSDI_SAP_RESET_REQ*/

    case MMGSDI_CARD_PUP_REQ:
      MSG_HIGH("Received CARD_PUP response ",0,0,0);
#ifndef  FEATURE_MMGSDI_GEN_OPTIMIZATION
      mmgsdi_status = mmgsdi_gen_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_GEN_OPTIMIZATION */
      break; /* end case - MMGSDI_CARD_PUP_REQ */

    case MMGSDI_CARD_PDOWN_REQ:
      MSG_HIGH("Received CARD_PDOWN response",0,0,0);
#ifndef  FEATURE_MMGSDI_GEN_OPTIMIZATION
      mmgsdi_status = mmgsdi_gen_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_GEN_OPTIMIZATION */
      break; /* end case - MMGSDI_CARD_PDOWN_REQ */

    case MMGSDI_CARD_STATUS_REQ:
      MSG_HIGH("Received CARD_STATUS response",0,0,0);
      /*----------------------------------------------------------------------------
      Get protocol
      NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
      -----------------------------------------------------------------------------*/
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */
      if(protocol == MMGSDI_UICC)
      {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#else
        mmgsdi_status = MMGSDI_ERROR;
#endif /* FEATURE_MMGSDI_UMTS */
      } /* end if - MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_rsp_status(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      } /* end else - MMGSDI_ICC */
      break; /* end case - MMGSDI_CARD_STATUS_REQ */

    case MMGSDI_CARD_RESET_REQ:
      MSG_HIGH("Received CARD_RESET response",0,0,0);
#ifndef  FEATURE_MMGSDI_GEN_OPTIMIZATION
      mmgsdi_status = mmgsdi_gen_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_GEN_OPTIMIZATION */
      break; /* end case - MMGSDI_CARD_RESET_REQ */

    case MMGSDI_REFRESH_REQ:
      MSG_HIGH("Received REFRESH response",0,0,0);
      mmgsdi_status = mmgsdi_refresh_main_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break; /* end case - MMGSDI_REFRESH_REQ */

    case MMGSDI_DISABLE_BDN_REQ:
    case MMGSDI_ENABLE_BDN_REQ:
      MSG_HIGH("Received ENABLE/DISABLE_BDN 0x%x response",cmd_req_enum,0,0);
#ifndef  FEATURE_MMGSDI_GEN_OPTIMIZATION
      /* Send the response handling to generic file because if we get a
         response in the form of BDN_REQ, it means that it was not able
         build the corresponding ICC or UICC function or it was unable to
         determine the protocol; since no protocol can be identified, the
         response to client should be built as a generic response */
      mmgsdi_status = mmgsdi_gen_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_GEN_OPTIMIZATION */
      break;

    case MMGSDI_ACL_OPERATION_REQ:
      MSG_HIGH("Received ACL_OPERATION response",0,0,0);
      mmgsdi_status = mmgsdi_process_acl_operation_rsp(
                        &task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break; /* end case - MMGSDI_ACL_OPERATION_REQ */

    case MMGSDI_REHABILITATE_REQ:
      MSG_HIGH("Received REHABILITATE response",0,0,0);
      mmgsdi_status = mmgsdi_process_rehabilitate_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_INVALIDATE_REQ:
      MSG_HIGH("Received INVALIDATE response",0,0,0);
      mmgsdi_status = mmgsdi_process_invalidate_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_PIN_OPERATION_REQ:
      MSG_HIGH("Received PIN_OPERATION response",0,0,0);
      mmgsdi_status = mmgsdi_process_pin_op_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break; /* end case - MMGSDI_PIN_OPERATION_REQ */

    case MMGSDI_GET_ALL_PIN_STATUS_REQ:
      MSG_HIGH("Received GET_ALL_PIN_STATUS response",0,0,0);
      mmgsdi_status = mmgsdi_process_get_all_pin_status_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break; /* end case - MMGSDI_GET_ALL_PIN_STATUS_REQ */

    case MMGSDI_SELECT_AID_REQ:
      MSG_HIGH("Received SELECT_AID response",0,0,0);
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /*FEATURE_UIM_UICC*/
      break;

    case MMGSDI_SRV_AVAILABLE_REQ:
      MSG_HIGH("Received SRV_AVAILABLE response",0,0,0);
 #ifdef FEATURE_MMGSDI_MBMS
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /*FEATURE_MMGSDI_MBMS */
      break; /* end case - MMGSDI_SRV_AVAILABLE_REQ */

    case MMGSDI_USIM_AUTH_REQ:
      MSG_HIGH("Received USIM_AUTH response",0,0,0);
#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /*FEATURE_MMGSDI_MBMS */
      break; /* end case - MMGSDI_USIM_AUTH_REQ */

    case MMGSDI_COMPUTE_IP_AUTH_REQ:
      MSG_HIGH("Received COMPUTE_IP_AUTH response",0,0,0);
#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
      mmgsdi_status = mmgsdi_icc_rsp_compute_ip_auth(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /*FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */
      break; /* end case - MMGSDI_COMPUTE_IP_AUTH_REQ */

    case MMGSDI_RUN_CAVE_REQ:
      MSG_HIGH("Received RUN CAVE response",0,0,0);
    #if defined(FEATURE_MMGSDI_CDMA) && defined(FEATURE_AUTH)
      mmgsdi_status = mmgsdi_icc_rsp_run_cave(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
    #else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
    #endif /* FEATURE_MMGSDI_CDMA && FEATURE_AUTH */
      break; /* end case - MMGSDI_RUN_CAVE_REQ */

    case MMGSDI_GENERATE_KEY_VPM_REQ:
      MSG_HIGH("Received GENERATE KEY response",0,0,0);
    #if defined(FEATURE_MMGSDI_CDMA) && defined(FEATURE_AUTH)
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */
      if (protocol == MMGSDI_ICC)
      {
        mmgsdi_status = mmgsdi_icc_rsp_generate_key_vpm(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      }
      else
      {
        MSG_ERROR("Cannot process Generate Key, protocol is: 0x%x",protocol,0,0);
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      }
    #else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
    #endif /* FEATURE_MMGSDI_CDMA && FEATURE_AUTH */
      break; /* end case - MMGSDI_GENERATE_KEY_VPM_REQ */

    case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
      MSG_HIGH("Received JCDMA_GET_CARD_INFO response", 0, 0, 0);
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */

      if(protocol == MMGSDI_ICC)
      {
        mmgsdi_status = mmgsdi_jcdma_rsp_get_card_info(
                          &task_cmd_ptr->cmd.cmd.uim_report_rsp);
      }
      else
      {
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      }
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */
      break;

    case MMGSDI_SEND_APDU_REQ:
      MSG_HIGH("Received SEND_APDU response", 0, 0, 0);
#if (defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)) && !defined(FEATURE_MMGSDI_GEN_OPTIMIZATION)
      mmgsdi_status = mmgsdi_gen_rsp_send_apdu_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* (defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)) && !FEATURE_MMGSDI_GEN_OPTIMIZATION */
      break; /* end case - MMGSDI_SEND_APDU_REQ*/

    case MMGSDI_GET_ATR_REQ:
      MSG_HIGH("Received GET_ATR response ", 0, 0, 0);
#ifndef FEATURE_MMGSDI_GEN_OPTIMIZATION
      mmgsdi_status = mmgsdi_gen_rsp_get_atr(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_GEN_OPTIMIZATION */
      break;

    case MMGSDI_CARD_READER_STATUS_REQ:
      MSG_HIGH("Received CARD_READER_STATUS response ", 0, 0, 0);
#ifndef FEATURE_MMGSDI_GEN_OPTIMIZATION
      mmgsdi_status = mmgsdi_gen_rsp_card_reader_status(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_GEN_OPTIMIZATION */
      break;

    case MMGSDI_MFLO_INIT_ACT_PARAMS_REQ:
    case MMGSDI_MFLO_GET_SUBSCRIBER_ID_REQ:
    case MMGSDI_MFLO_GET_PUBLIC_KEY_REQ:
    case MMGSDI_MFLO_SIGN_UCAST_REQ:
    case MMGSDI_MFLO_VERIFY_UCAST_REQ:
      MSG_HIGH("Received MFLO 0x%x response ", cmd_req_enum, 0, 0);
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_MFLO */
      break;

   case MMGSDI_BCAST_REQ:
      MSG_HIGH("Received BCAST response",0,0,0);
#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /*FEATURE_MMGSDI_MBMS */
      break; /* end case - MMGSDI_BCAST_REQ */

    case MMGSDI_SEARCH_REQ:
      MSG_HIGH("Received SEARCH response from UIM ",0,0,0);
      /*----------------------------------------------------------------------------
      Get protocol
      NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
      -----------------------------------------------------------------------------*/
      mmgsdi_status = mmgsdi_util_determine_protocol(&protocol);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Problem determining protocol in mmgsdi_process_command",0,0,0);
        break;
      } /* mmgsdi_status is not SUCCESS */
      if(protocol == MMGSDI_UICC)
      {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#else
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_UMTS */
      } /* end if - MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_rsp_search(
                          &task_cmd_ptr->cmd.cmd.uim_report_rsp);
      } /* end else - MMGSDI_ICC */
      break; /* end case - MMGSDI_SEARCH_REQ*/

    case MMGSDI_CREATE_PKCS15_TABLE_REQ:
      MSG_HIGH("Received CREATE PKCS15 TABLE RSP - 0x%x response ", cmd_req_enum, 0, 0);
#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_PKCS15 */
      break;

    case MMGSDI_MAX_CMD_ENUM:
      MSG_ERROR("Unknown mmgsdi command in mmgsdi_process_response 0x%x",cmd_req_enum,0,0);
      mmgsdi_status = mmgsdi_util_is_client_request_table_index_free(index, &free_index);
      if ((mmgsdi_status == MMGSDI_SUCCESS) && (free_index == TRUE))
      {
        MSG_ERROR("Extra Rsp Message was built, drop the message", 0, 0, 0);
        return mmgsdi_status;
      }
      mmgsdi_status = MMGSDI_ERROR;
      break;

    default:
      MSG_ERROR("Unknown mmgsdi command in mmgsdi_process_response",0,0,0);
      mmgsdi_status = MMGSDI_ERROR;
      break; /* end default case */
  } /* end switch - req type */

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Req 0x%x, Unable to build proper Cnf in mmgsdi_process_response, 0x%x",
               cmd_req_enum,mmgsdi_status,0);
    MSG_ERROR("Use static cnf buffer to build cnf notification", 0, 0, 0);

    mmgsdi_status = mmgsdi_build_and_queue_static_cnf(index, mmgsdi_status);

    if (mmgsdi_status != MMGSDI_SUCCESS)
      MSG_ERROR("Impossible to return cnf back to client", 0, 0, 0);

  } /* end if - mmgsdi_status is not SUCCESS */


  /*----------------------------------------------------------------------------
    Free buffers: Elements in mmgsdi client req table
  -----------------------------------------------------------------------------*/
  mmgsdi_util_free_client_request_table_index(index);

  return mmgsdi_status;
} /* mmgsdi_process_response */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_SAP_RESPONSE

DESCRIPTION
  This function, called in gsdimain, is called to process an mmgsdi sap rsp.

DEPENDENCIES
  gsdimain must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_sap_response(
  const mmgsdi_generic_rsp_type *  response_ptr /* GSDI response to be processed */
)
{
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif

  MMGSDIUTIL_RETURN_IF_NULL(response_ptr);

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif

  return mmgsdi_status;
} /* mmgsdi_process_sap_response */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_CNF

DESCRIPTION
  This function, called in gsdimain, is called to process an mmgsdi
  confirmation.
  It will send out the confirmation to the user as necessary

DEPENDENCIES
  gsdimain must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_cnf(
  mmgsdi_task_cnf_type *  cnf_ptr /* GSDI response to be processed */
)
{

/*-----------------------------------------------------------------------------
  Initialize variables
-----------------------------------------------------------------------------*/
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_SUCCESS;

  MSG_HIGH ("MMGSDI_PROCESS_CNF", 0, 0, 0);

  if (cnf_ptr == NULL)
  {
    ERR_FATAL("Not Able to Allocate Confirmation Ptr",0, 0, 0);
  }
  if (cnf_ptr->client_req_cb == NULL)
  {
    ERR_FATAL("Client request response CB is a Null",0, 0, 0);
  }
  (cnf_ptr->client_req_cb)(
    cnf_ptr->mmgsdi_cnf.response_header.mmgsdi_status,
    cnf_ptr->mmgsdi_cnf.response_header.response_type,
    &cnf_ptr->mmgsdi_cnf);

  /* Check if event needs to be sent out or not */
  mmgsdi_cnf_generate_evt(
    cnf_ptr->mmgsdi_cnf.response_header.mmgsdi_status,
    cnf_ptr->mmgsdi_cnf.response_header.response_type,
    &cnf_ptr->mmgsdi_cnf);

  /*----------------------------------------------------------------------------
    Free buffers: Elements in Confirmation ptr
  -----------------------------------------------------------------------------*/
  mmgsdi_util_free_response_ptr(&cnf_ptr->mmgsdi_cnf);

  return mmgsdi_status;
} /* mmgsdi_process_cnf */


/*===========================================================================
FUNCTION MMGSDI_HANDLE_OFFLINE_SIG

DESCRIPTION
  This function, called in mmgsdi_task, is called to process an offline signal.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void mmgsdi_handle_offline_sig(void)
{
  MSG_HIGH("MMGSDI: GOT TASK_OFFLINE_SIG",0,0,0);

  /* Stop Task Processing */
  task_offline();

  /* Clear signal */
  (void) rex_clr_sigs(&gsdi_task_tcb, TASK_OFFLINE_SIG);

  MSG_HIGH("MMGSDI: Sent OFFLINE ACK to TMC",0,0,0);
}


/*===========================================================================
FUNCTION MMGSDI_HANDLE_STOP_SIG

DESCRIPTION
  This function, called in mmgsdi_task, is called to process an stop signal.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void mmgsdi_handle_stop_sig(void)
{
  MSG_HIGH("MMGSDI: GOT TASK_STOP_SIG",0,0,0);

   /* Clear signal */
  (void) rex_clr_sigs(&gsdi_task_tcb, TASK_STOP_SIG);

  /* Don't process items in Queue */
  /* Sort of flushes the Queue    */
  gsdi_data.sim_present_and_ready = FALSE;
  gsdi_data.task_stopped = TRUE;
  mmgsdi_generic_data.task_stopped = TRUE;

#ifndef FEATURE_VIRTUAL_SIM
  /*-----------------------------------------------------------------------------
    Vote off the UIM
    1 Stop Polling the SIM/USIM in the case we are still in a Traffic Channel
  -----------------------------------------------------------------------------*/
  (void)uim_power_control(UIM_TC,FALSE);

  /*-----------------------------------------------------------------------------
    Vote off the UIM
    2 Ensure the USIM Session is still not voted on.
  -----------------------------------------------------------------------------*/
  (void)uim_power_control(UIM_USIM_SESSION,FALSE);
#endif /* FEATURE_VIRTUAL_SIM */

  /* Stop Task Processing */
  task_stop();

  MSG_HIGH("MMGSDI: Sent STOP ACK to TMC",0,0,0);
}


/*===========================================================================
FUNCTION MMGSDI_HANDLE_SANITY_TIMER_EXPIRE_SIG

DESCRIPTION
  This function, called in mmgsdi_task, is called to process sanity timer
  expiration signal.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void mmgsdi_handle_sanity_timer_expire_sig(void)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  int                     index = 0;
  boolean                 more = FALSE;
  MSG_HIGH("MMGSDI: Got GSDI_UIM_SANITY_TIMER_EXPIRE_SIG",0,0,0);

  /* Clear the signal */
  (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_UIM_SANITY_TIMER_EXPIRE_SIG);

  /* Obtain the command index that the timer expiration is associated with */
  mmgsdi_status = mmgsdi_util_get_current_await_cmd(&index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("No MMGSDI Cmd Pending for UIM response", 0, 0, 0);
  }
  else
  {
    /* build command to report error to the current command that has the timer expired */
    mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(index,
      mmgsdi_client_req_table_info[index].request_type,
      MMGSDI_ERROR);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Unable to queue internal MMGSDI uim report 0x%x",
        mmgsdi_status, 0, 0);
      MSG_ERROR("No Response will be sent back to Client", 0,0,0);
      /* Release the client table */
      mmgsdi_util_free_client_request_table_index(index);
    }
  }
  /* Check if there is any more pending command */
  mmgsdi_status = mmgsdi_util_more_pending_command(&more);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("MMGSDI Cmd Pending for UIM response", 0, 0, 0);
  }
  else
  {
    /* Reset sanity timer */
    (void)rex_set_timer(&gsdi_uim_sanity_timer,
                        gsdi_data.gsdi_uim_sanity_timer_value);
  }
}


/*===========================================================================
FUNCTION MMGSDI_HANDLE_QUEUE_SIG

DESCRIPTION
  This function, called in mmgsdi_task, is called to process a signals of
  command and response queue.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  If a command is being processed by mmgsdi, the task would no longer
  wait for MMGSDI_TASK_CMD_Q_SIG.
===========================================================================*/
static void mmgsdi_handle_queue_sig(rex_sigs_type sigs_rcvd,
                                    rex_sigs_type *waiting_for_sigs)
{
  mmgsdi_task_cmd_type *    mmgsdi_task_ptr  = NULL;
  mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_SUCCESS;

  if(sigs_rcvd & MMGSDI_TASK_RSP_Q_SIG)
  {
    /* Process Responses first, before processing a new MMGSDI command */
    if ((mmgsdi_task_ptr =
        ((mmgsdi_task_cmd_type*)(q_get(&mmgsdi_task_rsp_q)))) != NULL)
    {
      switch (mmgsdi_task_ptr->cmd.cmd_enum)
      {
        case MMGSDI_UIM_REPORT_RSP:
        case MMGSDI_CLIENT_ID_REG_RSP:
          mmgsdi_status = mmgsdi_process_response(mmgsdi_task_ptr);

          if(mmgsdi_status != MMGSDI_SUCCESS)
          {
           MSG_HIGH("Error processing mmgsdi response in mmgsdi_task",0,0,0);
          }/* end if !MMGSDI_SUCCESS */
          break; /* end case MMGSDI_UIM_REPORT_RSP, MMGSDI_CLIENT_ID_REG_RSP */

        case MMGSDI_SAP_REPORT_RSP:
          mmgsdi_status = mmgsdi_process_sap_response(&mmgsdi_task_ptr->cmd.cmd.sap_report_rsp);

          if(mmgsdi_status != MMGSDI_SUCCESS)
          {
            MSG_HIGH("Error processing mmgsdi sap response in mmgsdi_task",0,0,0);
          }/* end if !MMGSDI_SUCCESS */
          break;/* end case MMGSDI_SAP_REPORT_RSP */

        default:
          mmgsdi_status = mmgsdi_process_cnf(&(mmgsdi_task_ptr->cmd.cmd.mmgsdi_cnf));

          if(mmgsdi_status != MMGSDI_SUCCESS)
          {
            MSG_HIGH("Error processing mmgsdi response in mmgsdi_task",0,0,0);
          }/* end if !MMGSDI_SUCCESS */
          mmgsdi_queue_service_counter--;
          break; /* end case default */
      }

      if (mmgsdi_task_ptr != &mmgsdi_static_task_buf)
      {
        /* Only free task pointer if we are not using the static task buf */
        MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_task_ptr);
      }
      mmgsdi_cmd_in_progress = FALSE;

      /* We are finished with the current command in progress, we are good to
       * look at further commands.  Add the CMD_Q signal back to the signals
       * that we would wait for.
       */
      *waiting_for_sigs |= ( MMGSDI_TASK_CMD_Q_SIG | GSDI_TASK_CMD_Q_SIG ) ;

    } /* end if mmgsdi_task_rsp_q is not empty */

    rex_enter_crit_sect(&mmgsdi_crit_sect);
    /* Clear the signal if no more responses to look at */
    if ( q_cnt(&mmgsdi_task_rsp_q) == 0 )
    {
      /*  No items on the queue. Clear signal */
      (void) rex_clr_sigs(&gsdi_task_tcb, MMGSDI_TASK_RSP_Q_SIG);
    } /* end else - mmgsdi_task_rsp_q is empty */
    rex_leave_crit_sect(&mmgsdi_crit_sect);
  } /* end if MMGSDI_TASK_RSP_Q_SIG */
  else if((sigs_rcvd & MMGSDI_TASK_CMD_Q_SIG) && !mmgsdi_cmd_in_progress)
  {
    /* Process MMGSDI Command */
    if ((mmgsdi_task_ptr = ((mmgsdi_task_cmd_type*)(q_get(&mmgsdi_task_cmd_q)))) != NULL)
    {
      /* We do not want to look at any further commands until we are
       * finished with the current command.  Remove the CMD_Q signal from the
       * signals that we would wait for.
       */
      *waiting_for_sigs &= ~( MMGSDI_TASK_CMD_Q_SIG | GSDI_TASK_CMD_Q_SIG );

      if(mmgsdi_is_cmd_in_progress(mmgsdi_task_ptr))
      {
        /*The command cannot be handled right now, queue it back*/
        (void)q_link(mmgsdi_task_ptr, &mmgsdi_task_ptr->link);
        q_put(&mmgsdi_task_cmd_q, &mmgsdi_task_ptr->link);
      }
      else
      {
        MSG_MED("MMGSDI_TASK_CMD_Q_SIG received",0,0,0);
        /* Processing the response, so clear the command in progress flag */
        mmgsdi_cmd_in_progress = TRUE;

        mmgsdi_status = mmgsdi_process_command(mmgsdi_task_ptr);

        if( (mmgsdi_status != MMGSDI_SUCCESS) &&
            (mmgsdi_status != MMGSDI_REFRESH_IN_PROGRESS) )
        {
          MSG_HIGH("Error processing mmgsdi command in mmgsdi_task",0,0,0);
          MSG_ERROR("No Response will be sent back to Client",
                    mmgsdi_status,0,0);
        }/* end if !MMGSDI_SUCCESS */
        MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_task_ptr);
      }
    } /* end if mmgsdi_task_cmd_q is not empty */

    /* Clear the signal if no more commands to look at */
    if ( q_cnt(&mmgsdi_task_cmd_q) == 0 )
    {
      /*  No items on the queue. Clear signal */
      (void) rex_clr_sigs(&gsdi_task_tcb, MMGSDI_TASK_CMD_Q_SIG);
    } /* end else - mmgsdi_task_cmd_q is empty */
  } /* end if MMGSDI_TASK_CMD_Q_SIG and ! mmgsdi_cmd_in_progress */
  else
  {
    /* It was MMGSDI's turn (odd counter), but neither MMGSDI CMD nor
      MMGSDI RSP signals were set, or we already have an mmgsdi command
      in progress.  Let gsdimain loop again */

  } /* end else, it was mmgsdi q's turn, but no commands */

  /* Increment the mmgsdi_queue_service_counter
     Only increment if there is not currently an mmgsdi cmd in progress.
     If there, is, we do not want to process anything but an MMGSDI RSP */
  if(!mmgsdi_cmd_in_progress)
  {
    mmgsdi_queue_service_counter++;
  } /* end if !mmgsdi_cmd_in_progress */
}


/*===========================================================================
FUNCTION GSDI_CHECK_PERSO_FS_PROCS

DESCRIPTION
  This function will perform the Personalization Initialization procedures
  for SIM Lock.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gsdi_check_perso_fs_procs(
  int32 *           len_p,
  uint8  *          data_buf_p)
{
#if defined(FEATURE_MMGSDI_PERSONALIZATION) && \
   !defined(FEATURE_MMGSDI_PERSONALIZATION_ENGINE)

  gsdi_returns_T gsdi_status = GSDI_ERROR;

  gsdi_status =  gsdi_perso_init(len_p,
                                  data_buf_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MSG_ERROR("FAILURE ACCESSING / VERIFYING FILE SYSTEM: 0x%x",gsdi_status,0,0);

    /* Now generate a notification for any task that
    ** may have already registered.  In doing so, this
    ** registration is saved, and every client, will receive
    ** a late notification
    */

    gsdi_perso_notify_clients_of_perso_event( GSDI_PERSO_SANITY_ERROR,
                                              FALSE,
                                              0,
                                              NULL );
  }
#else
#if !defined(FEATURE_MMGSDI_PERSONALIZATION_NON_QC) && \
     defined(FEATURE_MMGSDI_PERSONALIZATION_ENGINE)

  gsdi_returns_T gsdi_status = GSDI_ERROR;

  gsdi_status =  gsdi_perso_engine_init(len_p,
                          data_buf_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MSG_ERROR("FAILURE ACCESSING / VERIFYING FILE SYSTEM: 0x%x",gsdi_status,0,0);

    /* Now generate a notification for any task that
    ** may have already registered.  In doing so, this
    ** registration is saved, and every client, will receive
    ** a late notification
    */
    gsdi_perso_engine_notify_clients_of_perso_event( GSDI_PERSO_SANITY_ERROR,
                                              FALSE,
                                              0,
                                              NULL );
  }
#endif /* !(FEATURE_MMGSDI_PERSONALIZATION_NON_QC) && FEATURE_MMGSDI_PERSONALIZATION_ENGINE*/
#endif /* FEATURE_MMGSDI_PERSONALIZATION && !FEATURE_MMGSDI_PERSONALIZATION_ENGINE*/
}


/*===========================================================================
FUNCTION MMGSDI_FLUSH_COMMAND_IN_PROGRESS

DESCRIPTION
  This function will clean up the commands that are in progress so that
  MMGSDI can continue with its normal processing.  This command should be
  called when an error has been detected by UIM and the card is declared
  as No Card

DEPENDENCIES


LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_flush_command_in_progress(
  mmgsdi_slot_id_enum_type    slot)
{
  int32                      index            = 0;
  int32                      index_from_rsp_q = MMGSDI_MAX_CLIENT_INFO;
  int32                      client_index     = 0;
  int32                      session_index    = 0;
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  boolean                    need_flushing    = FALSE;
  int32                      pending_rsp_cnt  = 0;
  mmgsdi_task_cmd_type *     mmgsdi_item_ptr  = NULL;

  for (index = 0; index < MMGSDI_MAX_CLIENT_INFO; index++)
  {
    need_flushing = FALSE;
    mmgsdi_status = MMGSDI_SUCCESS;
    if ( mmgsdi_client_req_table_info[index].client_id != 0 )
    {
      mmgsdi_status = mmgsdi_util_get_client_id_index(
        mmgsdi_client_req_table_info[index].client_id ,
        &client_index, &session_index);
      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        if (session_index == MMGSDI_MAX_CHANNEL_INFO)  /* channel ID default to 0 */
        {
          /* Assume SLOT_1 now until default has a slot associated with it */
          if (slot == MMGSDI_SLOT_1)
          {
            need_flushing = TRUE;
          }
        }
        else
        {
          if (mmgsdi_client_id_reg_table[client_index].session_info[session_index].slot == slot)
          {
            need_flushing = TRUE;
          }
        }
        if (need_flushing)
        {
          /* Do not queue a response for a command that already has a
          ** a command pending in the response.
          */
          pending_rsp_cnt = q_cnt(&mmgsdi_task_rsp_q);

          if (pending_rsp_cnt)
          {
            /* Next need to verify we don't generate a generic errror
            ** response for a command that already has a response pending.
            ** 1. Retrieve the Index from the pending response.
            ** 2. Does the index Match for the client index for which we have
            **    a pending command that needs to be flushed.
            **    if so, there is no need to generate a generic error response.
            **    otherwise, flush the command and provide a generic error
            **    response.
            */
            mmgsdi_item_ptr = q_check(&mmgsdi_task_rsp_q);

            while ( pending_rsp_cnt > 0 && mmgsdi_item_ptr != NULL )
            {
              switch ( mmgsdi_item_ptr->cmd.cmd_enum )
              {
                case MMGSDI_SAP_REPORT_RSP:
                  index_from_rsp_q = mmgsdi_item_ptr->cmd.cmd.sap_report_rsp.rsp_data_index;
                  break;
                case MMGSDI_UIM_REPORT_RSP:
                  index_from_rsp_q = mmgsdi_item_ptr->cmd.cmd.uim_report_rsp.rsp_data_index;
                  break;
                case MMGSDI_CLIENT_ID_REG_RSP:
                  index_from_rsp_q = mmgsdi_item_ptr->cmd.cmd.client_id_reg_rsp.rsp_data_index;
                  break;
                default:
                  index_from_rsp_q = MMGSDI_MAX_CLIENT_INFO;
                  break;
              } /* switch ( mmgsdi_item_ptr->cmd.cmd_enum )*/
              if ( index == index_from_rsp_q )
              {
                need_flushing = FALSE;
                break;
              }

              pending_rsp_cnt--;
              if ( pending_rsp_cnt > 0 )
              {
                mmgsdi_item_ptr = q_next(&mmgsdi_task_rsp_q, &(mmgsdi_item_ptr->link));
              }
            } /* while (pending_rsp_cnt > 0 && mmgsdi_item_ptr != NULL) */
          } /* if (pending_rsp_cnt) */
        } /* if (need_flushing) */

        if ( need_flushing )
        {
          mmgsdi_status = mmgsdi_build_and_queue_generic_err_rsp(
            slot,
            index,
            mmgsdi_client_req_table_info[index].request_type,
            MMGSDI_ERROR);

          if (mmgsdi_status != MMGSDI_SUCCESS)
          {
            MSG_ERROR("Unable to queue internal MMGSDI uim report 0x%x",
              mmgsdi_status, 0, 0);
            MSG_ERROR("No Response will be sent back to Client", 0,0,0);
            /* Release the client table */
            mmgsdi_util_free_client_request_table_index(index);
          }
        } /* if ( need_flushing ) */
      } /* if (mmgsdi_status == MMGSDI_SUCCESS) */
    } /* if if ( mmgsdi_client_req_table_info[index].client_id != 0 ) */
  } /* for (index = 0; index < MMGSDI_MAX_CLIENT_INFO; index++) */
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION GSDIMAIN

DESCRIPTION
  Entry point for the MMGSDI task.  This function performs task initialization,
  then sits in an infinite loop, waiting on an input queue, and responding
  to messages received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gsdimain (
  dword dummy
  /* Parameter required for REX.  Tell lint to ignore it. */
  /*lint -esym(715,dummy) */
)
{
  mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_SUCCESS;
#ifndef TEST_FRAMEWORK
  byte                      data_ptr[GSDI_MAX_DATA_BLOCK_LEN];
  int32                     returned_data_len = 0;
#endif /*!TEST_FRAMEWORK*/
  rex_sigs_type             sigs =  GSDI_HIGH_PRI_CMD_Q_SIG  |
                                    GSDI_TASK_CMD_Q_SIG      |
                                    MMGSDI_TASK_CMD_Q_SIG    |
                                    MMGSDI_TASK_RSP_Q_SIG    |
                                    GSDI_UIM_ERROR_SIG       |
                                    GSDI_UIM_RESET_COMPLETE_SIG |
                                    TASK_STOP_SIG
#ifndef FEATURE_GSTK
                                    | TASK_OFFLINE_SIG;
#else
                                    | TASK_OFFLINE_SIG
                                    | GSDI_UIM_SANITY_TIMER_EXPIRE_SIG;
#endif /* FEATURE_GSTK */
  rex_sigs_type                       sigs_rcvd = 0x00;

  /* ------------------------------------------------------------------------
  ** Initialize support structures for main task
  ** ----------------------------------------------------------------------*/
  mmgsdi_init();

  /* ------------------------------------------------------------------------
  ** Wait for start signal from the Main Control
  ** task
  ** ----------------------------------------------------------------------*/

#if defined( FEATURE_MMGSDI_UMTS ) || defined ( FEATURE_MMGSDI_GSM )
#error code not present
#else
  task_start(GSDI_RPT_TIMER_SIG,DOG_GSDI_RPT,&gsdi_rpt_timer);
#endif /* FEATURE_MMGSDI_UMTS || FEATURE_MMGSDI_GSM */

#ifdef FEATURE_DOG
  /* ------------------------------------------------------------------------
  ** Initially kick watchdog and set timer for
  ** watchdog report interval
  ** ----------------------------------------------------------------------*/
  dog_report(DOG_GSDI_RPT);
  (void) rex_set_timer(&gsdi_rpt_timer, DOG_GSDI_RPT_TIME);
#endif /* FEATURE_DOG */

  /* ------------------------------------------------------------------------
  ** Initialize the Critical Section for GSDI
  ** Task
  ** ----------------------------------------------------------------------*/
  rex_init_crit_sect(&gsdi_crit_sect);
  rex_init_crit_sect(&mmgsdi_crit_sect);

#ifdef FEATURE_MMGSDI_TEST_AUTOMATION
  /* ------------------------------------------------------------------------
  ** Initialize GSDI - DIAG Interface
  ** ----------------------------------------------------------------------*/
  gsdidiag_init ();
#endif /* FEATURE_MMGSDI_TEST_AUTOMATION*/

#ifndef TEST_FRAMEWORK
  /* ------------------------------------------------------------------------
  ** Wait for all Tasks to Register Callback with
  ** GSDI
  ** ----------------------------------------------------------------------*/
  gsdi_wait_for_tasks_to_register ( );

  /* ------------------------------------------------------------------------

    ** Validate the File System for Legacy Personalization Solution.  If
    ** the personalization engine is used, we just return out of this function
    ** ----------------------------------------------------------------------*/
  gsdi_check_perso_fs_procs(&returned_data_len,
                            (uint8 *)data_ptr);
#endif /*!TEST_FRAMEWORK*/

  /* ------------------------------------------------------------------------
  ** Main Task Loop
  ** ----------------------------------------------------------------------*/
  for (;;)
  {
    /*-----------------------------------------------------------------------------
                            Wait on signal
    -----------------------------------------------------------------------------*/
    sigs_rcvd = gsdi_wait( sigs );

    /*-----------------------------------------------------------------------------
                          Process Signal - TASK_OFFLINE_SIG
    -----------------------------------------------------------------------------*/

    if (sigs_rcvd & TASK_OFFLINE_SIG)
    {
      mmgsdi_handle_offline_sig();
    } /* end if TASK_OFFLINE_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - TASK_STOP_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & TASK_STOP_SIG)
    {
      mmgsdi_handle_stop_sig();
    } /* end if TASK_STOP_SIG */
    /*-----------------------------------------------------------------------------
                          Process Signal - GSDI_UIM_RESET_COMPLETE_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & GSDI_UIM_RESET_COMPLETE_SIG)
    {
      if(!gsdi_data.recovering_after_reset)
      {
        if ( mmgsdi_recover_after_reset(MMGSDI_SLOT_1) == MMGSDI_ERROR )
        {
          MSG_ERROR("Failed to recover UIM after reset", 0, 0, 0);
        }
      }
      else
      {
        MSG_ERROR("Tried to call mmgsdi_recover_after_reset while recovering!",0,0,0);
      }
    }

    /*-----------------------------------------------------------------------------
                          Process Signal - GSDI_UIM_SANITY_TIMER_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & GSDI_UIM_SANITY_TIMER_EXPIRE_SIG)
    {
      mmgsdi_handle_sanity_timer_expire_sig();
    } /* end if GSDI_UIM_SANITY_TIMER_EXPIRE_SIG */
    /*-----------------------------------------------------------------------------
                          Process Signal - GSDI_UIM_ERROR_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & GSDI_UIM_ERROR_SIG)
    {
      mmgsdi_event_data_type *card_err_evt_ptr = NULL;
      MSG_HIGH("GSDI_UIM_ERROR_SIG received",0,0,0);

      /* Clear signal */
      (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_UIM_ERROR_SIG);

      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        card_err_evt_ptr,
        sizeof(mmgsdi_event_data_type),
        mmgsdi_status);

      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = mmgsdi_util_build_card_error_evt(MMGSDI_SLOT_1, card_err_evt_ptr);

        if (mmgsdi_status != MMGSDI_SUCCESS)
          MSG_ERROR("Unable to build Card Error Event", 0, 0, 0);
        else
        {
          mmgsdi_status = mmgsdi_flush_command_in_progress(MMGSDI_SLOT_1);
          if (mmgsdi_status != MMGSDI_SUCCESS)
          {
            MSG_ERROR("Unable to flush command", 0, 0, 0);
          }
          /* Do not notify the UI about card absense while in passive mode */
          if ( gsdi_data.uim_error_condition !=
               UIM_INT_CMD_ERR_IN_PASSIVE_MODE_S &&
               gsdi_data.uim_error_condition !=
               UIM_CMD_TIMED_OUT_IN_PASSIVE_MODE_S &&
               gsdi_data.uim_error_condition !=
               UIM_MAX_PARITY_IN_PASSIVE_S &&
               gsdi_data.uim_error_condition !=
               UIM_MAX_RXBRK_IN_PASSIVE_S &&
               gsdi_data.uim_error_condition !=
               UIM_MAX_OVERRUN_IN_PASSIVE_S
             )
          {
            mmgsdi_status = mmgsdi_util_notify_client_evt(card_err_evt_ptr);
          }

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif
        }
        MMGSDIUTIL_TMC_MEM_FREE(card_err_evt_ptr);
      }
    } /* end if - GSDI_UIM_ERROR_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - GSDI_HIGH_PRI_CMD_Q_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & GSDI_HIGH_PRI_CMD_Q_SIG)
    {
      gsdimain_legacy(sigs_rcvd, sigs);
    } /* end if GSDI_HIGH_PRI_CMD_Q_SIG */

    /*-----------------------------------------------------------------------------
      Process Signal -
      GSDI_TASK_CMD_Q_SIG or MMGSDI_TASK_RSP_Q_SIG or MMGSDI_TASK_CMD_Q_SIG
      If servicing the new queues, always process an mmgsdi response before a
      mmgsdi cmd.
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd &
          (GSDI_TASK_CMD_Q_SIG | MMGSDI_TASK_RSP_Q_SIG | MMGSDI_TASK_CMD_Q_SIG))
    {
      if(mmgsdi_queue_service_counter % 2 == 0)
      {
        /* mmgsdi_queue_service_counter is even, so it's gsdi q's turn
          Increment the mmgsdi_queue_service_counter */
        mmgsdi_queue_service_counter++;
        if(sigs_rcvd & GSDI_TASK_CMD_Q_SIG)
        {
          gsdimain_legacy(sigs_rcvd, sigs);
        } /* end if GSDI_TASK_CMD_Q_SIG */
      } /*  end if mmgsdi_queue_service_counter is even */
      else
      {
        mmgsdi_handle_queue_sig(sigs_rcvd, &sigs);
      } /* end else mmgsdi_queue_service_counter is odd  */
    } /* end GSDI_TASK_CMD_Q_SIG || MMGSDI_TASK_RSP_Q_SIG || MMGSDI_TASK_CMD_Q_SIG */
    else
    {
      ERR("MMGSDI TASK INVALID SIGS: %x",sigs_rcvd,0,0);

      // TODOOOOOOOOOOOOOOOOO
      /* See if the old gsdi module handles it */
      gsdimain_legacy(sigs_rcvd, sigs);
      /* Reset the Sigs by Masking out Good Sigs and
      ** Clearing out Bad SIGS
      */
      (void)rex_clr_sigs(&gsdi_task_tcb, (sigs_rcvd & ~sigs) );

      /* Preserve any Signals currently Set and wipe */
      /* out the bad Signals                         */
      sigs_rcvd &= sigs;

      mmgsdi_status = MMGSDI_ERROR;
    } /* end else - Invalid signal in GSDI task */
  } /* end for(;;) */
} /* mmgsdi_task */


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && defined (FEATURE_BT_EXTPF_SAP)*/


/*===========================================================================
FUNCTION MMGSDI_CM_INIT

DESCRIPTION
  Function called to register with CM

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
===========================================================================*/
static void mmgsdi_cm_init (void)
{
    mmgsdi_generic_data.mmgsdi_cm_id = 0;

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && defined (FEATURE_BT_EXTPF_SAP)*/
}


/*===========================================================================
FUNCTION MMGSDI_ADD_CLIENT_ID_ENTRY

DESCRIPTION
  Function to find a valid free spot in the client ID registration table
  and create a client ID in the table.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_add_client_id_entry(
  const mmgsdi_rex_tcb_type   *client_tcb_ptr,
  mmgsdi_client_id_type       *client_id_out_ptr,
  boolean                      notify_init,
  mmgsdi_callback_type         cmd_rsp_cb_ptr)
{
  int    i         = 0;
  int    j         = 0;
  uint64 client_id = 0;

  MMGSDIUTIL_RETURN_IF_NULL(client_tcb_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(client_id_out_ptr);

  for (i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if (mmgsdi_client_id_reg_table[i].client_tcb_ptr == NULL)
    {
      mmgsdi_client_id_reg_table[i].client_tcb_ptr = (mmgsdi_rex_tcb_type*)client_tcb_ptr;
      client_id = (uint64)(int32touint32(rand()));
      client_id = client_id << MMGSDI_CLIENT_ID_SHIFT;
      while(j < MMGSDI_MAX_CLIENT_INFO)
      {
        if (mmgsdi_util_is_valid_client_id(client_id) == MMGSDI_SUCCESS)
        {
          /* Search for next one */
          client_id = client_id >> MMGSDI_CLIENT_ID_SHIFT;
          client_id++;
          client_id = client_id << MMGSDI_CLIENT_ID_SHIFT;
        }
        else
        {
          break;
        }
        j++;
      }
      mmgsdi_client_id_reg_table[i].client_id = client_id;
      *client_id_out_ptr = mmgsdi_client_id_reg_table[i].client_id;
      mmgsdi_client_id_reg_table[i].notify_init = notify_init;
      mmgsdi_client_id_reg_table[i].cmd_rsp_cb_ptr = cmd_rsp_cb_ptr;
      return MMGSDI_SUCCESS;
    }
  }
  return MMGSDI_ERROR;
}


/*===========================================================================
FUNCTION MMGSDI_BUILD_POST_PIN1_CMD

DESCRIPTION
  Function to build the GSDI Internal Request and put into the GSDI
  command queue so that GSDI can be kicked into the post pin1 initialization
  process if needed

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
void mmgsdi_build_post_pin1_cmd(mmgsdi_slot_id_enum_type slot)
{
  gsdi_internal_req_T msg;

  MSG_HIGH("Entering mmgsdi_build_post_pin1_cmd",0,0,0);

  /* Build post pin command only if the pin event is sent out and
     and init_complete is false */
  if ( ( ( slot                           == MMGSDI_SLOT_1) &&
         ( mmgsdi_data_slot1.mmgsdi_state == MMGSDI_STATE_PIN_EVT_SENT ))
#ifdef FEATURE_MMGSDI_DUAL_SLOT
       ||
       ( ( slot                           == MMGSDI_SLOT_2) &&
         ( mmgsdi_data_slot2.mmgsdi_state == MMGSDI_STATE_PIN_EVT_SENT))
#endif
       )
  {

    MSG_HIGH("Building the post_pin1_cmd for slot: %d", slot, 0,0);
    memset(&msg, 0x00, sizeof(gsdi_internal_req_T));
    /* Message Header */
    msg.message_header.message_set = MS_GSDI;
    msg.message_header.message_id  = GSDI_INTERNAL_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_internal_req_T ), &msg.message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg.message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */

    switch(slot)
    {
    case MMGSDI_SLOT_1:
      msg.message_header.slot = GSDI_SLOT_1;
      break;
    case MMGSDI_SLOT_2:
      msg.message_header.slot = GSDI_SLOT_2;
      break;
    case MMGSDI_SLOT_AUTOMATIC:
    case MMGSDI_MAX_SLOT_ID_ENUM:
      ERR_FATAL("Bad Slot",0,0,0);
/*lint -esym(527,return) */
      return;
/*lint +esym(527,return) */
    }

    msg.message_header.gsdi_async_callback = gsdi_discard_cnf;
    if(gsdi_cmd_ext(&msg) != GSDI_SUCCESS)
    {
      MSG_ERROR("Unable to build GSDI Internal cmd", 0, 0, 0);
    }
  }
}


/*===========================================================================
FUNCTION MMGSDI_STATE_PIN1_UPDATE

DESCRIPTION
  Function to update the state or global info as needed when PIN1 event
  is received

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void mmgsdi_state_pin1_update(const mmgsdi_pin_evt_info_type* pin_evt_ptr)
{
  mmgsdi_state_enum_type    *state_ptr     = NULL;
  boolean                    can_proceed   = FALSE;

  if(pin_evt_ptr == NULL)
    return;

  switch(pin_evt_ptr->slot)
  {
  case MMGSDI_SLOT_1:
    state_ptr = &mmgsdi_data_slot1.mmgsdi_state;
    break;
  case MMGSDI_SLOT_2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    state_ptr = &mmgsdi_data_slot2.mmgsdi_state;
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  case MMGSDI_SLOT_AUTOMATIC:
  case MMGSDI_MAX_SLOT_ID_ENUM:
  default:
    MSG_ERROR("Bad Slot in PIN1_Update",0,0,0);
    return;
  }

  /* Only if we are in the PIN_EVT_SENT state should we need to continue
     with building a request to GSDI to trigger the post pin1 init */
  if (*state_ptr == MMGSDI_STATE_PIN_EVT_SENT)
  {
    can_proceed = mmgsdi_util_can_proceed_to_post_pin1(MMGSDI_SLOT_1);
    if(can_proceed)
    {
      MSG_HIGH("Sending to build post_pin1 cmd",0,0,0);
      mmgsdi_build_post_pin1_cmd(MMGSDI_SLOT_1);
    }
  }
}


/*===========================================================================
FUNCTION MMGSDI_EVT_CB

DESCRIPTION
  Internal Event Callback function registered during mmgsdi_init so that
  MMGSDI can receive event notification

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  State variables may be updated as needed
===========================================================================*/
void mmgsdi_evt_cb (const mmgsdi_event_data_type *event_ptr)
{
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif

  if (event_ptr == NULL)
    return;

  mmgsdi_util_update_state_variable(event_ptr);

  MSG_HIGH("Received event: 0x%x in mmgsdi_evt_cb",event_ptr->evt,0,0);

  switch(event_ptr->evt)
  {
  case MMGSDI_PIN1_EVT:
    switch (event_ptr->data.pin.slot)
    {
      case MMGSDI_SLOT_1:
        if(gsdi_data.init_completed == FALSE )
        {
          mmgsdi_data_slot1.mmgsdi_state = MMGSDI_STATE_PIN_EVT_SENT;
        }
        break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      case MMGSDI_SLOT_2:
        if(gsdi_data.init_completed_2 == FALSE )
        {
          mmgsdi_data_slot2.mmgsdi_state = MMGSDI_STATE_PIN_EVT_SENT;
        }
        break;
#endif
      default:
        MSG_ERROR("Wrong slot %d:", event_ptr->data.pin.slot,0,0);
        break;
    }
    mmgsdi_state_pin1_update(&event_ptr->data.pin);
    break;
  case MMGSDI_UNIVERSAL_PIN_EVT:
    mmgsdi_state_pin1_update(&event_ptr->data.pin);
    break;
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*FEATURE_MMGSDI_UMTS*/

  case MMGSDI_REFRESH_EVT:
    mmgsdi_refresh_process_evt(event_ptr);
    break;
    
  case MMGSDI_CARD_INSERTED_EVT:
#ifdef FEATURE_MMGSDI_INIT_RTRE_CONFIG
    mmgsdi_init_rtre_config();
#endif /* FEATURE_MMGSDI_INIT_RTRE_CONFIG */
    break;

  case MMGSDI_CARD_ERROR_EVT:
  case MMGSDI_CARD_REMOVED_EVT:
#ifdef FEATURE_MMGSDI_INIT_RTRE_CONFIG
    mmgsdi_init_rtre_config();
#endif /* FEATURE_MMGSDI_INIT_RTRE_CONFIG */
    /* clean up the refresh pointer to avoid memory leak as well as the 
       inability to do hot swap due to the refresh state */
    mmgsdi_refresh_rsp_clean_up();
    break;    

  default:
    break;
  }
}/* mmgsdi_evt_cb*/


/*===========================================================================
FUNCTION MMGSDI_GSDI_EVT_CB

DESCRIPTION
  Internal Event Callback function registered during mmgsdi_init so that
  GSDI can perform refresh procedures whenever required

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  State variables may be updated as needed
===========================================================================*/
static void mmgsdi_gsdi_evt_cb (const mmgsdi_event_data_type *event_ptr)
{
  uim_items_enum_type    *sim_filenames_ptr = NULL;
  boolean                 ok_to_refresh     = TRUE;
  mmgsdi_return_enum_type mmgsdi_status     = MMGSDI_ERROR;
  uint32                  num_files         = 0;
  uint8                   u8_num_files      = 0;
  mmgsdi_option_type      option            = {0};
  uint8                   i                 = 0;

  if(event_ptr == NULL)
  {
    MSG_ERROR("Event ptr is NULL",0,0,0);
    return;
  }

  switch(event_ptr->evt)
  {
    case MMGSDI_REFRESH_EVT:
      switch(event_ptr->data.refresh.stage)
      {
        /*Make a Dummy File List, so that Clients do not reject command*/
        uim_items_enum_type dummy_file_list;

        case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT:
          gsdi_util_notify_clients_to_vote(
                                  &dummy_file_list,
                                  0,
                                  TRUE); /*request_to_take_down_callstack*/

          gsdi_util_can_gsdi_take_down_stack(
                              refresh_vote_list_node.head_p,
                              &ok_to_refresh);

          /*------------------------------------------------------------
          If the command was triggered by MMGSDI to itself, and we need
          to do a refresh and there is a call, suspend refresh until the
          call is brought down.
          --------------------------------------------------------------*/
          if(ok_to_refresh == FALSE &&
            event_ptr->data.refresh.mode == MMGSDI_REFRESH_RESET_AUTO &&
            mmgsdi_generic_data.mmgsdi_cm_id != 0)
          {
            /*----------------------------------------------------------------------
               Check to see if any call is present
              ------------------------------------------------------------------------*/
             if(mmgsdi_generic_data.any_call_present)
             {
               /*Wait for Call End*/
               mmgsdi_refresh_info_ptr->refresh_status = MMGSDI_REFRESH_LATER;
             }
             else
             {
               (void)mmgsdi_ok_to_refresh(gsdi_data.client_id,
                                          event_ptr->data.refresh.slot,
                                          ok_to_refresh,
                                          option,
                                          mmgsdi_util_internal_cb,
                                          0);
             }
          }
          else
          {
            (void)mmgsdi_ok_to_refresh(gsdi_data.client_id,
                                       event_ptr->data.refresh.slot,
                                       ok_to_refresh,
                                       option,
                                       mmgsdi_util_internal_cb,
                                       0);
          }
          break;

        case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN:
          num_files = event_ptr->data.refresh.refresh_files.num_files;

          if(num_files == 0 ||
             event_ptr->data.refresh.refresh_files.file_list_ptr == NULL)
          {
            MSG_ERROR("File List or Num files is empty",0,0,0);
            return;
          }

          MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(sim_filenames_ptr,
             num_files * sizeof(uim_items_enum_type),
             mmgsdi_status);

          if(mmgsdi_status == MMGSDI_ERROR)
          {
            return;
          }

          for(i=0;i<num_files;i++)
          {
            (void)mmgsdi_util_convert_to_uim_items_enum(
                    event_ptr->data.refresh.refresh_files.file_list_ptr[i],
                    &sim_filenames_ptr[i]);
          }
          u8_num_files = (uint8) num_files;
          gsdi_util_notify_clients_to_vote(
                                  sim_filenames_ptr,
                                  u8_num_files,
                                  FALSE); /*request_to_take_down_callstack*/

          gsdi_util_can_gsdi_do_file_notify(
                              &refresh_vote_list_node,
                              &ok_to_refresh);

          (void)mmgsdi_ok_to_refresh(gsdi_data.client_id,
                                     event_ptr->data.refresh.slot,
                                     ok_to_refresh,
                                     option,
                                     mmgsdi_util_internal_cb,
                                     0);
          MMGSDIUTIL_TMC_MEM_FREE(sim_filenames_ptr);
          break;

        case MMGSDI_REFRESH_STAGE_START:
          if(event_ptr->data.refresh.mode == MMGSDI_REFRESH_NAA_FCN)
          {
            num_files = event_ptr->data.refresh.refresh_files.num_files;

            if(num_files == 0 ||
               event_ptr->data.refresh.refresh_files.file_list_ptr == NULL)
            {
              MSG_ERROR("File List or Num files is empty",0,0,0);
              return;
            }
            MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(sim_filenames_ptr,
               num_files * sizeof(uim_items_enum_type),
               mmgsdi_status);
            if(mmgsdi_status == MMGSDI_ERROR)
            {
              return;
            }

            for(i=0;i<num_files;i++)
            {
              (void)mmgsdi_util_convert_to_uim_items_enum(
                      event_ptr->data.refresh.refresh_files.file_list_ptr[i],
                      &sim_filenames_ptr[i]);
            }
            u8_num_files = (uint8)num_files;
            gsdi_util_notify_clients_fcn(sim_filenames_ptr,&u8_num_files);
            MMGSDIUTIL_TMC_MEM_FREE(sim_filenames_ptr);
          }
          break;

        default:
          break;
      }
      break;
    default:
      /* All other events are handled by MMGSDI in mmgsdi_refresh_process_evt*/
      break;
  }
}/*mmgsdi_gsdi_evt_cb*/


#ifdef FEATURE_MMGSDI_INIT_RTRE_CONFIG
/*===========================================================================
FUNCTION   mmgsdi_convert_nv_to_cm_rtre

DESCRIPTION
  Convert the RTRE define used by NV to the one used by CM

DEPENDENCIES
  None

RETURNS
  CM RTRE Config

SIDE EFFECTS
  None
===========================================================================*/
cm_rtre_config_e_type mmgsdi_convert_nv_to_cm_rtre(uint32 nv_rtre)
{
  cm_rtre_config_e_type cm_rtre;

  switch (nv_rtre)
  {
#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
#error code not present
#endif

    case NV_RTRE_CONFIG_RUIM_ONLY:
      cm_rtre = CM_RTRE_CONFIG_RUIM_ONLY;
      break;

    case NV_RTRE_CONFIG_NV_ONLY:
      cm_rtre = CM_RTRE_CONFIG_NV_ONLY;
      break;

    case NV_RTRE_CONFIG_RUIM_OR_DROP_BACK:
    default:
      cm_rtre = CM_RTRE_CONFIG_RUIM_OR_DROP_BACK;
      break;
  }
  return cm_rtre;
}/* mmgsdi_convert_nv_to_cm_rtre */


/*===========================================================================
FUNCTION   MMGSDI_CM_RTRE_CB

DESCRIPTION
  Callback for RTRE notification to the CM.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_cm_rtre_cb(void *data_block, cm_ph_cmd_e_type ph_cmd,
                cm_ph_cmd_err_e_type ph_cmd_err)
{
  (void) data_block;
  if (ph_cmd_err != CM_PH_CMD_ERR_NOERR)
  {
    MSG_ERROR("RTRE Command 0x%x is completed with Error 0x%x ", ph_cmd_err, ph_cmd, 0);
  }
  else
  {
    MSG_HIGH("RTRE Command 0x%x is completed successfully",ph_cmd,0,0);
  }
} /* mmgsdi_rtre_cb */


/*===========================================================================
FUNCTION   mmgsdi_init_rtre_config

DESCRIPTION
  Initializes the RTRE config with CM

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  This function is passsing the RTRE Mode / RTRE Configuration value to the CM and
  later CM will notify this RTRE configuration value to its clients.
===========================================================================*/
void mmgsdi_init_rtre_config()
{
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  nv_item_type *    nv_rtre_config;
  nv_stat_enum_type result;

  nv_rtre_config = mmgsdi_malloc(sizeof(nv_item_type));
  if (NULL == nv_rtre_config)
  {
    MSG_ERROR("MMGSDI RTRE Configuration is failed, No Heap Space",0,0,0);
    return;
  }
  memset((unsigned char*)(nv_rtre_config), 0x00, sizeof(nv_item_type));
  /* Get RTRE configuration */
  result = gsdi_get_nv(NV_RTRE_CONFIG_I, nv_rtre_config);

  if(result != NV_DONE_S)
  {
#ifdef FEATURE_RTRE_DEFAULT_IS_NV
    nv_rtre_config->rtre_config = NV_RTRE_CONFIG_NV_ONLY;
#else
    nv_rtre_config->rtre_config = NV_RTRE_CONFIG_RUIM_ONLY;
#endif /* FEATURE_RTRE_DEFAULT_IS_NV */
    (void) gsdi_put_nv(NV_RTRE_CONFIG_I, nv_rtre_config);
  }
  MSG_HIGH("Notifying CM for 0x%x RTRE configuration ",nv_rtre_config->rtre_config,0,0);
  if ( FALSE == cm_ph_cmd_change_rtre_config(mmgsdi_cm_rtre_cb, NULL, mmgsdi_generic_data.mmgsdi_cm_id,
                                             mmgsdi_convert_nv_to_cm_rtre(nv_rtre_config->rtre_config)))
  {
    MSG_ERROR("cm_ph_cmd_change_rtre_config is failed ",0,0,0);
  }
  mmgsdi_free((unsigned char*) nv_rtre_config);
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
} /* mmgsdi_init_rtre_config */
#endif /* FEATURE_MMGSDI_INIT_RTRE_CONFIG */


/*===========================================================================
FUNCTION MMGSDI_INIT

DESCRIPTION
  Function called to define Necessary Timers and initialize the gsdi_data
  global variable.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
===========================================================================*/
static void mmgsdi_init (
 void
)
{
  int32                   i             = 0;
  int                     j             = 0;
  int32                   dummy         = 0;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  /* ---------------------------------------------------------
      DEFINE NECESSARY TIMERS
    ---------------------------------------------------------*/

  /* Sanity Timer used for the GSDI - UIM Interface */
  rex_def_timer (&gsdi_uim_sanity_timer,&gsdi_task_tcb,GSDI_UIM_SANITY_TIMER_EXPIRE_SIG);

#ifdef FEATURE_DOG
  /* Initialize Dog Reporting timer */
  rex_def_timer( &gsdi_rpt_timer, &gsdi_task_tcb, GSDI_RPT_TIMER_SIG );
#endif /* FEATURE_DOG */

#if defined(FEATURE_SEC) && defined(FEATURE_MMGSDI_PERSO_SEC_RAND_CB_API)
  rex_def_timer(&gsdi_perso_secrnd_timer,&gsdi_task_tcb, GSDI_PERSO_SECRND_READY_SIG);
#endif /* FEATURE_SEC && FEATURE_MMGSDI_PERSO_SEC_RAND_CB_API*/

  /* ---------------------------------------------------------
      INITIALIZE ALL QUEUES
  --------------------------------------------------------- */
  (void) q_init(&mmgsdi_task_cmd_q);
  (void) q_init(&mmgsdi_task_rsp_q);

  /* --------------------------------------------------------
      INITIALIZE GLOBALS
  -------------------------------------------------------- */

  memset(&mmgsdi_generic_data, 0x00, sizeof(mmgsdi_generic_data_type));

  memset(&mmgsdi_data_slot1, 0x00, sizeof(mmgsdi_slot_data_type));
  mmgsdi_data_slot1.app_info_ptr = mmgsdi_app_pin_table;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  memset(&mmgsdi_data_slot2, 0x00, sizeof(mmgsdi_slot_data_type));

  mmgsdi_data_slot2.app_info_ptr = mmgsdi_app_pin_table;
#endif

  mmgsdi_util_reset_app_data(FALSE);

  for (i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    memset(&mmgsdi_client_req_table_info[i], 0x00, sizeof(mmgsdi_uim_client_req_user_data_type));
    mmgsdi_client_req_table_info[i].request_type = MMGSDI_MAX_CMD_ENUM;
    mmgsdi_client_req_table_info[i].orig_request_type = MMGSDI_MAX_CMD_ENUM;
  }

  memset(&mmgsdi_record_cache_list_node, 0x00, sizeof(mmgsdi_record_cache_node_list_type));
  memset(mmgsdi_client_id_reg_table, 0x00, sizeof(mmgsdi_client_id_reg_table_type));
  for (i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    for (j = 0; j < MMGSDI_MAX_CHANNEL_INFO; j++)
    {
      mmgsdi_client_id_reg_table[i].session_info[j].slot = MMGSDI_MAX_SLOT_ID_ENUM;
    }
  }

  /* -------------------------------------------------------
  CLEAR ALL SIGNALS
  ------------------------------------------------------- */
  /* Note:  All Clients of GSDI must register after this
            function has completed.  A Client registering
            before the signals are cleared, will result in
            not getting registered. */
  (void) rex_clr_sigs(&gsdi_task_tcb, (rex_sigs_type)~0);

  gsdi_main_task_init();

  /* -------------------------------------------------------
  Register the client ID and evt within the MMGSDI client
  registration table as the first entry.  This allows the
  client ID to be used as well as any event notification
  to be sent to MMGSDI itself at the earliest possible time
  ------------------------------------------------------- */
  if (mmgsdi_add_client_id_entry(&gsdi_task_tcb,
                                 &mmgsdi_generic_data.client_id,
                                 FALSE,
                                 NULL) != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Client ID Reg Failed", 0, 0, 0);
  }
  else
  {
    mmgsdi_status = mmgsdi_util_get_client_id_index(mmgsdi_generic_data.client_id,
                                                    &i, &dummy);
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      mmgsdi_client_id_reg_table[i].evt_cb_ptr = mmgsdi_evt_cb;
      /*Register for Refresh*/
      if ( MMGSDI_SUCCESS  != mmgsdi_refresh_register(mmgsdi_generic_data.client_id))
      {
        MSG_ERROR("mmgsdi_register_for_refresh is failed for REG files",0, 0, 0);
      }
    }
  }

 /* -------------------------------------------------------
  Register gsdi also within the MMGSDI client
  registration table as the next entry.  This will be used
  for refresh
  ------------------------------------------------------- */
  if (mmgsdi_add_client_id_entry(&gsdi_task_tcb,
                                 &gsdi_data.client_id,
                                 FALSE,
                                 NULL) != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Client ID Reg Failed", 0, 0, 0);
  }
  else
  {
    mmgsdi_status = mmgsdi_util_get_client_id_index(gsdi_data.client_id,
                                                    &i, &dummy);
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      mmgsdi_client_id_reg_table[i].evt_cb_ptr = mmgsdi_gsdi_evt_cb;
      if ( MMGSDI_SUCCESS == mmgsdi_refresh_register(gsdi_data.client_id))
      {
        MSG_ERROR("mmgsdi_register_for_refresh is failed for REG files",0, 0, 0);
      }
    }
  }

  mmgsdi_cm_init();
  mmgsdi_refresh_info_ptr = NULL;
}  /* mmgsdi_init */


#ifdef FEATURE_HTORPC_METACOMMENTS
/* ============================================================================
   FUNCTION:      MMGSDI_INTERNAL_CLIENT_ID_FORCE_CLEANUP_CB

   DESCRIPTION:
     This command callback that is used internally in MMGSDI for general
     command cnf processing

   PARAMETERS:
     status  : Input  status of the cnf
     cnf     : Input  confirmation type
     cnf_ptr : Input  detail of the confirmation

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
  ========================================================================== */
static void mmgsdi_internal_client_id_force_cleanup_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr)
{
  int32                          client_index       = MMGSDI_MAX_CLIENT_INFO;
  int32                          session_index      = MMGSDI_MAX_CHANNEL_INFO;
  mmgsdi_client_id_type          client_id          = 0;
  mmgsdi_return_enum_type        mmgsdi_status   = MMGSDI_ERROR;

  MSG_HIGH("Internal cmd cb cnf 0x%x, status 0x%x", cnf, status, 0);

  if(cnf_ptr == NULL)
  {
    MSG_ERROR("cnf_ptr is NULL", 0, 0, 0);
    return;
  }

  if(cnf == MMGSDI_SESSION_CLOSE_CNF)
  {
    if (status == MMGSDI_SUCCESS)
    {
      client_id =
        cnf_ptr->session_close_cnf.response_header.client_id & 0xFFFFFFFF00000000uLL;

      /* get the client id index from client_id*/
      mmgsdi_status = mmgsdi_util_get_client_id_index(client_id,
                                                      &client_index,
                                                      &session_index);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Invalid client id 0x%x, status 0x%x",
                   client_id, mmgsdi_status, 0);
        return;
      }

      if ((client_index >= MMGSDI_MAX_CLIENT_INFO) || (client_index < 0))
      {
        MSG_ERROR("Invalid client index 0x%x", client_index, 0, 0);
        return;
      }

      if ((mmgsdi_client_id_reg_table[client_index].cmd_rsp_cb_ptr != NULL) &&
          (mmgsdi_client_id_reg_table[client_index].cmd_rsp_cb_ptr ==
           (mmgsdi_callback_type)cnf_ptr->session_close_cnf.response_header.client_data))
      {
        if (mmgsdi_client_id_reg_table[client_index].num_session == 0)
        {

          mmgsdi_status = mmgsdi_client_id_dereg(
            client_id,
            mmgsdi_internal_client_id_force_cleanup_cb,
            0);
          if (mmgsdi_status != MMGSDI_SUCCESS)
          {
            MSG_ERROR("Dereg failed for client 0x%x, status 0x%x",
              mmgsdi_client_id_reg_table[client_index].client_id, mmgsdi_status, 0);
          }
        } /* if num_session == 0 */
      }
    }
    else
    {
      MSG_HIGH("Close Channel for session/client 0x%x failed, unable to proceed dereg",
        cnf_ptr->response_header.client_id, 0, 0);
    }
  }
} /* mmgsdi_internal_client_id_force_cleanup_cb */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/* ============================================================================
   FUNCTION:      MMGSDI_CLIENT_ID_FORCE_CLEANUP

   DESCRIPTION:
     This function performs cleanup for client id when RPC has indicated that
     the application processor client has been terminated.

   PARAMETERS:
     response_cb_ptr: Input  Callback that maps to the client id which enables
                             MMGSDI to perform corresponding client id
                             deregistration on the client's behalf.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     Will result in the client id(s) that were linked to the corresponding
     response_cb_ptr during the mmgsdi_client_id_reg to be deregistered.
  ========================================================================== */
void mmgsdi_client_id_force_cleanup(mmgsdi_callback_type response_cb_ptr)
{
#ifdef FEATURE_HTORPC_METACOMMENTS
  int32                      i               = 0;
  int32                      j               = 0;
  int32                      k               = 0;
  mmgsdi_return_enum_type    mmgsdi_status   = MMGSDI_ERROR;

  for (i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if ((mmgsdi_client_id_reg_table[i].cmd_rsp_cb_ptr != NULL) &&
        (mmgsdi_client_id_reg_table[i].cmd_rsp_cb_ptr == response_cb_ptr))
    {
      if (mmgsdi_client_id_reg_table[i].num_session != 0)
      {
        k = 0;
        for ( j = 0;
              (k < mmgsdi_client_id_reg_table[i].num_session &&
               j < MMGSDI_MAX_CHANNEL_INFO);
              j++)
        {
          /* check if the session id is valid or not */
          if ((mmgsdi_client_id_reg_table[i].session_info[j].app_info_ptr == NULL) &&
            (mmgsdi_client_id_reg_table[i].session_info[j].channel_id == 0))
          {
            /* move to the next iteration */
            continue;
          }
          k++;

          mmgsdi_status = mmgsdi_session_management_close (
            (mmgsdi_client_id_reg_table[i].client_id |
             mmgsdi_client_id_reg_table[i].session_info[j].session_id),
            mmgsdi_client_id_reg_table[i].session_info[j].slot,
            mmgsdi_internal_client_id_force_cleanup_cb,
            (mmgsdi_client_data_type)response_cb_ptr);

          if (mmgsdi_status != MMGSDI_SUCCESS)
          {
            MSG_ERROR("Close channel failed for client 0x%x, session 0x%x, status 0x%x",
              mmgsdi_client_id_reg_table[i].client_id,
              mmgsdi_client_id_reg_table[i].session_info[j].session_id,
              mmgsdi_status);
          }
        } /* end of for loop for num session */
      } /* if num_session != 0 */
      else
      {
        mmgsdi_status = mmgsdi_client_id_dereg(
          mmgsdi_client_id_reg_table[i].client_id,
          mmgsdi_internal_client_id_force_cleanup_cb,
          0);
        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          MSG_ERROR("Dereg failed for client 0x%x, status 0x%x",
            mmgsdi_client_id_reg_table[i].client_id, mmgsdi_status, 0);
        }
      } /* if num_session == 0 */
    } /* if the response_cb_ptr matches */
  } /* end of for loop for the client id reg table */
#else
  (void)response_cb_ptr;
#endif /* FEATURE_HTORPC_METACOMMENTS */
} /* mmgsdi_client_id_force_cleanup */


/*===========================================================================
   FUNCTION:      MMGSDI_PROCESS_CLIENT_REG_REQ

   DESCRIPTION:
     This function will process the client ID registration request

   DEPENDENCIES:

   LIMITATIONS:
     This function currently does not support event notification

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_client_reg_req(
  mmgsdi_client_id_reg_req_type * req_ptr
)
{
  mmgsdi_return_enum_type               build_status    = MMGSDI_ERROR;
  mmgsdi_return_enum_type               mmgsdi_status   = MMGSDI_ERROR;
  int32                                 i               = 0;
  int32                                 dummy           = 0;
  int32                                 index           = 0;
  int32                                 session_index   = 0;
  mmgsdi_option_type                    option          = {0};

  MSG_MED("MMGSDI PROCESS CLIENT ID REG REQ",0,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  switch (req_ptr->request_header.request_type)
  {
    case MMGSDI_CLIENT_EVT_REG_REQ:
      mmgsdi_status = mmgsdi_util_get_client_id_index(req_ptr->request_header.client_id,
                                                      &i, &dummy);
      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_client_id_reg_table[i].evt_cb_ptr = req_ptr->evt_cb_ptr;
        if (mmgsdi_client_id_reg_table[i].client_tcb_ptr != req_ptr->request_header.request_id_ptr)
        {
          // TODO???????????????  what if client tcb is different?????
          MSG_HIGH("Client TCB is different orig: 0x%x, new: 0x%x!",
                    mmgsdi_client_id_reg_table[i].client_tcb_ptr,
          req_ptr->request_header.request_id_ptr, 0);
        }
        mmgsdi_client_id_reg_table[i].client_tcb_ptr = req_ptr->request_header.request_id_ptr;
        /* If client has not been notified yet, send out the event */
        if (mmgsdi_data_slot1.mmgsdi_state != MMGSDI_STATE_NOT_INIT)
        {
          (void)mmgsdi_util_build_and_notify_evt_from_state(
            MMGSDI_SLOT_1, i);
        }
      }
      break;
    case MMGSDI_CLIENT_ID_REG_REQ:
      mmgsdi_status = mmgsdi_add_client_id_entry(
                        req_ptr->request_header.request_id_ptr,
                        &req_ptr->request_header.client_id,
                        req_ptr->notify_init,
                        req_ptr->request_header.response_cb);
      break;
    case MMGSDI_CLIENT_INIT_COMP_REQ:
      mmgsdi_status = mmgsdi_util_get_client_id_index(req_ptr->request_header.client_id,
                                                     &i,
                                                     &session_index);
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_client_id_reg_table[i].init_complete = TRUE;
        if(mmgsdi_refresh_info_ptr != NULL)
        {
          mmgsdi_status = mmgsdi_refresh_complete(
                                                  req_ptr->request_header.client_id,
                                                  req_ptr->request_header.slot_id,
                                                  TRUE,
                                                  option,
                                                  mmgsdi_util_internal_cb,
                                                  0);
          if (MMGSDI_SUCCESS != mmgsdi_status)
          {
            MSG_ERROR("Unable to send Refresh Complete 0x%x", mmgsdi_status, 0, 0);
          }
        } /* if(mmgsdi_refresh_info_ptr != NULL) */
      }
      break;
    default:
      MSG_ERROR("Invalid Client Reg", 0, 0, 0);
      break;
    }

  build_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (build_status == MMGSDI_SUCCESS)
  {
    build_status = mmgsdi_util_populate_client_request_table_info(index,
                                                                  &req_ptr->request_header,
                                                                  NULL, NULL);
    if (build_status == MMGSDI_SUCCESS)
    {
      build_status = mmgsdi_build_and_queue_client_reg_rsp(index, mmgsdi_status,
                                                            &req_ptr->request_header);
    }
  }

  return build_status;
} /* mmgsdi_process_client_reg_req */


/*===========================================================================
   FUNCTION:      MMGSDI_PROCESS_CLIENT_REG_CNF

   DESCRIPTION:
     This function will process the client registration confirmation

   DEPENDENCIES:

   LIMITATIONS:
     This function currently does not support event notification

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_client_reg_cnf(
  const mmgsdi_client_id_reg_rsp_type   * client_rsp_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_request_header_type         req_header;
  int32                              index            = 0;
  uim_cmd_type                      *uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  mmgsdi_sw_status_type              status_word;

  MSG_HIGH("MMGSDI PROCESS CLIENT ID REG CNF",0,0,0);

  /* status word is not applicable in client reg processing */
  status_word.sw1 = 0x00;
  status_word.sw2 = 0x00;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(client_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = client_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Error in getting info fr client req table info",
                             0, 0, 0);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Build confirmation message based on Client Registration or Client Event
    Callback Registration
    in case of MMGSDI_CLIENT_REG_MAX_ENUM, a client_reg_cnf_ptr will be built
    with mmgsdi_status sets to MMGSDI_ERROR
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cnf_build_and_queue(client_rsp_ptr->status,
                                             &req_header,
                                             extra_param_ptr,
                                             NULL,
                                             FALSE, /* status word present
                                                        flag */
                                             status_word);
  MSG_HIGH("CLIENT REG RSP   status 0x%x", client_rsp_ptr->status,  0, 0);
  return mmgsdi_status;
} /* mmgsdi_process_client_reg_cnf */


/*===========================================================================
   FUNCTION:      MMGSDI_PROCESS_CLIENT_DEREG_REQ

   DESCRIPTION:
     This function will process the client ID deregistration request

   DEPENDENCIES:

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_client_dereg_req(
  const mmgsdi_client_id_dereg_req_type * req_ptr
)
{
  mmgsdi_return_enum_type               build_status    = MMGSDI_ERROR;
  mmgsdi_return_enum_type               mmgsdi_status   = MMGSDI_ERROR;
  int32                                 i               = 0;
  int32                                 j               = 0;
  int32                                 session_dummy   = 0;
  int32                                 index           = 0;


  MSG_MED("MMGSDI PROCESS CLIENT ID DEREG REQ",0,0,0);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  switch (req_ptr->request_header.request_type)
  {
    case MMGSDI_CLIENT_EVT_DEREG_REQ:
      mmgsdi_status = mmgsdi_util_get_client_id_index(req_ptr->request_header.client_id,
                                                      &i, &session_dummy);
      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        if (mmgsdi_client_id_reg_table[i].evt_cb_ptr == req_ptr->evt_cb_ptr)
        {
          mmgsdi_client_id_reg_table[i].evt_cb_ptr = NULL;
        }
        else
        {
          MSG_ERROR("Evt Callback to be de-register not the same", 0, 0, 0);
          mmgsdi_status = MMGSDI_ERROR;
        }
      }
      break;

    case MMGSDI_CLIENT_ID_DEREG_REQ:
      mmgsdi_status = mmgsdi_util_get_client_id_index(req_ptr->request_header.client_id,
                                                              &i, &session_dummy);
      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        /* Check if any session is opened */
        if (mmgsdi_client_id_reg_table[i].num_session != 0)
        {
          MSG_ERROR("Still have session opened, need to close first", 0, 0, 0);
          mmgsdi_status = MMGSDI_ERROR;
        }
        else
        {
          mmgsdi_client_id_reg_table[i].client_tcb_ptr = NULL;
          mmgsdi_client_id_reg_table[i].client_id      = 0;
          mmgsdi_client_id_reg_table[i].evt_cb_ptr     = NULL;
          mmgsdi_client_id_reg_table[i].num_session    = 0;
          mmgsdi_client_id_reg_table[i].cmd_rsp_cb_ptr = NULL;
          for (j = 0; j < MMGSDI_MAX_CHANNEL_INFO; j++)
          {
            mmgsdi_client_id_reg_table[i].session_info[j].session_id = 0;
            mmgsdi_client_id_reg_table[i].session_info[j].channel_id = 0;
            mmgsdi_client_id_reg_table[i].session_info[j].app_info_ptr = NULL;
            mmgsdi_client_id_reg_table[i].session_info[j].slot = MMGSDI_MAX_SLOT_ID_ENUM;
          }
        }
      }
      break;
    default:
      MSG_ERROR("Invalid Client Dereg", 0, 0, 0);
      break;
    }

  build_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (build_status == MMGSDI_SUCCESS)
  {
    build_status = mmgsdi_util_populate_client_request_table_info(index,
        &req_ptr->request_header,
      NULL, NULL);
    if (build_status == MMGSDI_SUCCESS)
    {
      build_status = mmgsdi_build_and_queue_client_reg_rsp(index, mmgsdi_status,
          &req_ptr->request_header);
    }
  }

  return build_status;
} /* mmgsdi_process_client_dereg_req */


/*===========================================================================
   FUNCTION:      MMGSDI_PROCESS_CLIENT_DEREG_CNF

   DESCRIPTION:
     This function will process the client ID deregistration confirmation

   DEPENDENCIES:

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_client_dereg_cnf(
  const mmgsdi_client_id_reg_rsp_type   * client_rsp_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_request_header_type         req_header;
  int32                              index            = 0;
  uim_cmd_type                      *uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  mmgsdi_sw_status_type              status_word;

  MSG_HIGH("MMGSDI PROCESS CLIENT ID DEREG CNF",0,0,0);

  /* status word is not applicable in client dereg processing */
  status_word.sw1 = 0x00;
  status_word.sw2 = 0x00;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(client_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = client_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Error in getting info fr client req table info",
                             0, 0, 0);
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    Build confirmation message based on Client Registration or Client Event
    Callback Registration
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cnf_build_and_queue(client_rsp_ptr->status,
                                             &req_header,
                                             extra_param_ptr,
                                             NULL,
                                             FALSE, /* status word present
                                                       flag */
                                             status_word);

  MSG_HIGH("CLIENT DEREG RSP   status 0x%x", client_rsp_ptr->status,  0, 0);

  return mmgsdi_status;
} /* mmgsdi_process_client_dereg_cnf */


/*===========================================================================
   FUNCTION:      MMGSDI_CMD

   DESCRIPTION:
     This function allows client to put command into the MMGSDI command queue.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type  mmgsdi_cmd ( mmgsdi_task_cmd_type * task_cmd_ptr  )
{
  MMGSDIUTIL_RETURN_IF_NULL(task_cmd_ptr);

  switch (task_cmd_ptr->cmd.cmd_enum)
  {
    case MMGSDI_UIM_REPORT_RSP:
    case MMGSDI_SAP_REPORT_RSP:
    case MMGSDI_CLIENT_ID_REG_RSP:
    case MMGSDI_CNF:
      rex_enter_crit_sect(&mmgsdi_crit_sect);
      (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);
        /* Put the message on the queue */
      q_put(&mmgsdi_task_rsp_q, &task_cmd_ptr->link);

        /* Set the command queue signal for */
        /* receive task                     */
      (void) rex_set_sigs(&gsdi_task_tcb, MMGSDI_TASK_RSP_Q_SIG);
      rex_leave_crit_sect(&mmgsdi_crit_sect);
      return MMGSDI_SUCCESS;

    case MMGSDI_READ_REQ:
    case MMGSDI_WRITE_REQ:
    case MMGSDI_GET_FILE_ATTR_REQ:
    case MMGSDI_CLIENT_ID_REG_REQ:
    case MMGSDI_CLIENT_EVT_REG_REQ:
    case MMGSDI_CLIENT_ID_DEREG_REQ:
    case MMGSDI_CLIENT_EVT_DEREG_REQ:
    case MMGSDI_CLIENT_INIT_COMP_REQ:

    case MMGSDI_SESSION_DEACTIVATE_REQ:
    case MMGSDI_SESSION_OPEN_REQ:
    case MMGSDI_SESSION_SELECT_AID_REQ:
    case MMGSDI_SESSION_CLOSE_REQ:
    case MMGSDI_ISIM_AUTH_REQ:

    case MMGSDI_SEND_APDU_REQ:
    case MMGSDI_SAP_INIT_REQ:
    case MMGSDI_SAP_PIN_VERIFY_REQ:
    case MMGSDI_SAP_CONNECT_REQ:
    case MMGSDI_SAP_DISCONNECT_REQ:
    case MMGSDI_SAP_GET_ATR_REQ:
    case MMGSDI_SAP_SEND_APDU_REQ:
    case MMGSDI_SAP_POWER_ON_REQ:
    case MMGSDI_SAP_POWER_OFF_REQ:
    case MMGSDI_SAP_CARD_READER_STATUS_REQ:
    case MMGSDI_SAP_DEREG_REQ:
    case MMGSDI_SAP_RESET_REQ:
    case MMGSDI_CARD_PUP_REQ:
    case MMGSDI_CARD_PDOWN_REQ:
    case MMGSDI_CARD_STATUS_REQ:
    case MMGSDI_REFRESH_REQ:
    case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
    case MMGSDI_DISABLE_BDN_REQ:
    case MMGSDI_ENABLE_BDN_REQ:
    case MMGSDI_ACL_OPERATION_REQ:
    case MMGSDI_REHABILITATE_REQ:
    case MMGSDI_INVALIDATE_REQ:
    case MMGSDI_PIN_OPERATION_REQ:
    case MMGSDI_GET_ALL_PIN_STATUS_REQ:
    case MMGSDI_SELECT_AID_REQ:
    case MMGSDI_SRV_AVAILABLE_REQ:
    case MMGSDI_USIM_AUTH_REQ:
    case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
    case MMGSDI_COMPUTE_IP_AUTH_REQ:
    case MMGSDI_RUN_CAVE_REQ:
    case MMGSDI_GENERATE_KEY_VPM_REQ:
    case MMGSDI_CARD_RESET_REQ:
    case MMGSDI_GET_ATR_REQ:
    case MMGSDI_CARD_READER_STATUS_REQ:

    case MMGSDI_MFLO_INIT_ACT_PARAMS_REQ:
    case MMGSDI_MFLO_GET_SUBSCRIBER_ID_REQ:
    case MMGSDI_MFLO_GET_PUBLIC_KEY_REQ:
    case MMGSDI_MFLO_SIGN_UCAST_REQ:
    case MMGSDI_MFLO_VERIFY_UCAST_REQ:
    case MMGSDI_BCAST_REQ:
    case MMGSDI_SEARCH_REQ:
    case MMGSDI_CREATE_PKCS15_TABLE_REQ:
      if (mmgsdi_task_cmd_q.cnt >= MMGSDI_CMD_QUEUE_MAX_SIZE)
      {
        MSG_ERROR("Maximum number of command has been put onto cmd_q", 0, 0, 0);
        return MMGSDI_CMD_QUEUE_FULL;
      }
      (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

      /* Put the message on the queue */
      q_put(&mmgsdi_task_cmd_q, &task_cmd_ptr->link);

      /* Set the command queue signal for */
      /* receive task                     */
      (void) rex_set_sigs(&gsdi_task_tcb, MMGSDI_TASK_CMD_Q_SIG);
      return MMGSDI_SUCCESS;

    default:
      return MMGSDI_ERROR;
  }
} /* mmgsdi_cmd */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
FUNCTION MMGSDI_BUILD_REFRESH_REQ

DESCRIPTION
  This API is used by the Generic SIM Toolkit Task to request that a REFRESH
  be performed on the Mobile.  There are 5 GSM SIM Refresh Modes and 7 USIM
  Refresh Modes (as defined by the enum gsdi_refresh_modes_enum_type).

  MMGSDI_REFRESH_NAA_INIT_FULL_FCN      (SIM/USIM)
  MMGSDI_REFRESH_FCN                    (SIM/USIM)
  MMGSDI_REFRESH_NAA_INIT_FCN           (SIM/USIM)
  MMGSDI_REFRESH_NAA_INIT               (SIM/USIM)
  MMGSDI_REFRESH_RESET                  (SIM/USIM)
  MMGSDI_REFRESH_NAA_APP_RESET          (USIM)
  MMGSDI_REFRESH_3G_SESSION_RESET       (USIM)

DEPENDENCIES
 None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  Depending on the Mode requested, various events are generated as a result
  of the REFRESH Peformed.
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_build_refresh_req(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  const mmgsdi_refresh_req_type      *req_ptr,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type     * task_cmd_ptr    = NULL;
  mmgsdi_return_enum_type    mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_refresh_req_type *  msg_ptr         = NULL;
  int32                      total_mem_len   = 0;
  uint32                     var_len[MMGSDIUTIL_VAR_LEN_MAX_NUM];
  int32                      temp_mem        = 0;
  uint32                     task_cmd_len    = 0;
  mmgsdi_cmd_enum_type       mmgsdi_orig_req = MMGSDI_REFRESH_REQ;
  uint32                     i               = 0;
  uint32                     path_len        = 0;

  memset(var_len, 0x00, MMGSDIUTIL_VAR_LEN_MAX_NUM);

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response Callback Pointer Check
    2) Card Slot ID check
    3) Verify the Params provided are within the valid range
    4) Client ID Check
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("INVALID Client ID %x",client_id,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  switch(req_ptr->refresh_req_type)
  {
    case MMGSDI_REFRESH_ORIG_REFRESH_REQ:
      var_len[0] = int32touint32(req_ptr->refresh.card_req.refresh_files.data_len);
      break;

    case MMGSDI_REFRESH_ORIG_REG_REQ:
    case MMGSDI_REFRESH_ORIG_DEREG_REQ:
      /* This MACRO checks if client sends both file enums/
         and file paths with reg/dereg request */
      MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(
        req_ptr->refresh.reg_req.refresh_files.file_list_ptr,
        req_ptr->refresh.reg_req.refresh_files.file_path_ptr,
        mmgsdi_status);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }
      if(req_ptr->refresh.reg_req.refresh_files.file_list_ptr != NULL)
      {
        var_len[0] = req_ptr->refresh.reg_req.refresh_files.num_files *
                     sizeof(mmgsdi_file_enum_type);
      }
      if(req_ptr->refresh.reg_req.refresh_files.file_path_ptr != NULL)
      {
        var_len[0] = req_ptr->refresh.reg_req.refresh_files.num_file_paths *
                     sizeof(mmgsdi_path_type);
      }
      break;

    case MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ:
    case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
    case MMGSDI_REFRESH_ORIG_COMPLETE_REQ:
      var_len[0] = 0;
      break;

    case MMGSDI_REFRESH_ORIG_PUP_REQ:
      var_len[0] = 0;
      mmgsdi_orig_req = MMGSDI_CARD_PUP_REQ;
      break;

    case MMGSDI_REFRESH_ORIG_ENUM_MAX:
      MSG_ERROR("Invalid Orig Refresh Enum 0x%x", req_ptr->refresh_req_type, 0, 0);
      return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
   Allocate the memory for this request
   ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  mmgsdi_util_cal_align_size(uint32toint32(var_len[0]),
                              &temp_mem);

  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(var_len[0],temp_mem);

  total_mem_len += temp_mem;

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
    task_cmd_len);

  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type = req_ptr->refresh_req_type;
  if (var_len[0] > 0)
    mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.refresh_req, MMGSDI_REFRESH_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.refresh_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_REFRESH_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_REFRESH_REQ;
  msg_ptr->request_header.orig_request_type = mmgsdi_orig_req;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Populate the command details with the information provided by the client
    ------------------------------------------------------------------------*/
  if ( req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_REFRESH_REQ||
       req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ ||
       req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_PUP_REQ)
  {
    if(var_len[0])
    {
      memcpy(msg_ptr->refresh.card_req.refresh_files.data_ptr,
             req_ptr->refresh.card_req.refresh_files.data_ptr,
             var_len[0]);
    }

    memcpy(&msg_ptr->refresh.card_req.aid,
           &req_ptr->refresh.card_req.aid,
           sizeof(mmgsdi_aid_type));
    msg_ptr->refresh.card_req.num_files =
      req_ptr->refresh.card_req.num_files;
    msg_ptr->refresh.card_req.refresh_files.data_len =
      req_ptr->refresh.card_req.refresh_files.data_len;
    msg_ptr->refresh.card_req.refresh_mode =
      req_ptr->refresh.card_req.refresh_mode;

  }
  else if(req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_REG_REQ ||
          req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_DEREG_REQ )
  {
    if(req_ptr->refresh.reg_req.refresh_files.file_list_ptr != NULL)
    {
      memcpy(msg_ptr->refresh.reg_req.refresh_files.file_list_ptr,
             req_ptr->refresh.reg_req.refresh_files.file_list_ptr,
             var_len[0] );
      msg_ptr->refresh.reg_req.refresh_files.file_path_ptr = NULL;
    }
    if(req_ptr->refresh.reg_req.refresh_files.file_path_ptr != NULL)
    {
      /* No need to malloc for file path buff here */
      for(i = 0; i < req_ptr->refresh.reg_req.refresh_files.num_file_paths; i++)
      {
        if(req_ptr->refresh.reg_req.refresh_files.file_path_ptr[i].path_len == 0)
        {
          MSG_ERROR("Refresh:Zero path_len at file_path_ptr[0x%x].path_len",
                     i,0,0);
          /* Free the task_cmd_ptr */
          MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
          return MMGSDI_ERROR;
        }
        path_len =
          int32touint32(req_ptr->refresh.reg_req.refresh_files.file_path_ptr[i].path_len);
        memcpy(msg_ptr->refresh.reg_req.refresh_files.file_path_ptr[i].path_buf,
          req_ptr->refresh.reg_req.refresh_files.file_path_ptr[i].path_buf,
          MIN(path_len, MMGSDI_MAX_PATH_LEN)* sizeof(uint16));
        msg_ptr->refresh.reg_req.refresh_files.file_path_ptr[i].path_len =
          req_ptr->refresh.reg_req.refresh_files.file_path_ptr[i].path_len;
      }
      msg_ptr->refresh.reg_req.refresh_files.file_list_ptr = NULL;
    }
    msg_ptr->refresh.reg_req.refresh_files.num_files =
      req_ptr->refresh.reg_req.refresh_files.num_files;
   
    msg_ptr->refresh.reg_req.refresh_files.num_file_paths = 
      req_ptr->refresh.reg_req.refresh_files.num_file_paths;
    msg_ptr->refresh.reg_req.vote_for_init =
      req_ptr->refresh.reg_req.vote_for_init;
  }
  else if(req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ)
  {
    msg_ptr->refresh.ok_req = req_ptr->refresh.ok_req;
  }
  else if(req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_COMPLETE_REQ)
  {
    msg_ptr->refresh.pass_fail = req_ptr->refresh.pass_fail;
  }
  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS) {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
  }

  MSG_HIGH("Successful queue of REFRESH command %d for Refresh Request 0x%x",
            mmgsdi_status,
            req_ptr->refresh_req_type,
            0);

  return mmgsdi_status;
}/*mmgsdi_build_refresh_req*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


#ifdef FEATURE_MMGSDI_NV_ME_CONFIG
/*===========================================================================
FUNCTION MMGSDI_GET_ME_CONFIG_PARAM_FROM_NV

DESCRIPTION
This function gets the me capabilities for fdn, bdn and acl.
Reads the nv_item and sets the global data. If the nv_item is not enabled, then
the function writes the defaul value to the nv. If the nv_item could not be
read, a failure is return.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 If nv operation is not succesful, this function returns error and can put
 the me to no card.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_get_me_config_params_from_nv(void)
{
  nv_stat_enum_type        nv_status;
  nv_item_type           * nv_item_ptr = NULL;
  uint32                   config_value;

  MSG_LOW("Entered mmgsdi_nv_config_values",0,0,0);

  nv_item_ptr = mmgsdi_malloc(sizeof(nv_item_type));

  if(nv_item_ptr == NULL)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("TMC Heap Space Not available", 0, 0, 0);
    return MMGSDI_ERROR;
  }
  memset((unsigned char*)(nv_item_ptr), 0x00, sizeof(nv_item_type));
#ifndef TEST_FRAMEWORK
  nv_status = gsdi_get_nv(NV_MMGSDI_ME_CONFIG_PARAM_I,
                                         nv_item_ptr);

#else
  nv_status = NV_DONE_S;
#endif /*!TEST_FRAMEWORK*/
  if ( nv_status == NV_NOTACTIVE_S  )
  {

    MSG_HIGH("NV Item is not initialised, so write it to a default value.",
              0,0,0);
    nv_item_ptr->mmgsdi_me_config_param = int32touint32(MMGSDI_DEFAULT_ME_CONFIG_VALUE);
#ifndef TEST_FRAMEWORK
    nv_status = gsdi_put_nv(NV_MMGSDI_ME_CONFIG_PARAM_I,
                            nv_item_ptr);
#endif/* !TEST_FRAMEWORK*/
  }

  if(nv_status != NV_DONE_S)
  {

    MSG_ERROR("Couldn't get the nv parameter, this should not happen!!! %d",
                nv_status,0,0);
    mmgsdi_free( (unsigned char*) nv_item_ptr );
    return MMGSDI_ERROR;
  }

#if defined( PLATFORM_LTK ) || defined(TEST_FRAMEWORK)
  nv_item_ptr->mmgsdi_me_config_param = MMGSDI_DEFAULT_ME_CONFIG_VALUE;
#endif/*PLATFORM_LTK || TEST_FRAMEWORK*/

  /* nv read succesful */
  config_value = nv_item_ptr->mmgsdi_me_config_param;

  if(config_value & MMGSDI_FDN_SUPPORTED_MASK)
  {
    mmgsdi_generic_data.me_capabilities.me_supports_fdn = TRUE;
  }
  else
  {
    mmgsdi_generic_data.me_capabilities.me_supports_fdn = FALSE;
    MSG_HIGH("ME doesn't support FDN",0,0,0);
  }

  if(config_value & MMGSDI_BDN_SUPPORTED_MASK)
  {
    mmgsdi_generic_data.me_capabilities.me_supports_bdn = TRUE;

    MSG_HIGH("ME supports BDN",0,0,0);
  }
  else
  {
    mmgsdi_generic_data.me_capabilities.me_supports_bdn = FALSE;
    MSG_HIGH("ME doesn't support BDN ",0,0,0);
  }

  if(config_value & MMGSDI_ACL_SUPPORTED_MASK)
  {
    mmgsdi_generic_data.me_capabilities.me_supports_acl = TRUE;
    MSG_HIGH("ME supports ACL",0, 0, 0);
  }
  else
  {
    mmgsdi_generic_data.me_capabilities.me_supports_acl = FALSE;
    MSG_HIGH("ME doesn't support ACL",0, 0, 0);
  }
  mmgsdi_free((unsigned char*) nv_item_ptr );
  return MMGSDI_SUCCESS;
}
#endif /* FEATURE_MMGSDI_NV_ME_CONFIG*/


/*===========================================================================
FUNCTION MMGSDI_TIMER_EXPIRY

DESCRIPTION
This function builds a response if an external module that MMGSDI is
dependent on does not respond

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 Builds an Error Response to MMGSDI Client

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_timer_expiry (unsigned long req)
{
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && defined (FEATURE_BT_EXTPF_SAP)*/
  MSG_MED("Timer for cmd %x expired", req,0,0);
}/* mmgsdi_timer_expiry */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================

FUNCTION MMGSDI_MALLOC

DESCRIPTION
  mmgsdi_malloc calls mem_malloc to request memory from the tmc_heap or tmc_small_heap
  depending on whether or not the request greater than 64KB if
  FEATURE_MMGSDI_USE_SMALL_HEAP is defined and FEATURE_LOW_MEMORY_USAGE is not.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  a pointer is allocated.
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void * mmgsdi_malloc(dword size)
{
#if !defined(FEATURE_LOW_MEMORY_USAGE) && defined(FEATURE_MMGSDI_USE_SMALL_HEAP)
  if (size <= tmc_get_mem_heap_size(TMC_MEM_HEAP_TYPE_THRD))
  {
    return mem_malloc(&tmc_heap_small, size);
  }
  else
  {
    return mem_malloc(&tmc_heap, size);
  }
#else
  return mem_malloc(&tmc_heap, size);
#endif /* !FEATURE_LOW_MEMORY_USAGE && FEATURE_MMGSDI_USE_SMALL_HEAP */
}/* mmgsdi_malloc */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================

FUNCTION MMGSDI_FREE

DESCRIPTION
  The mmgsdi_free frees the pointer from the regular tmc_heap or the tmc_heap_small
  based on the pointer address, but only if FEATURE_MMGSDI_USE_SMALL_HEAP is defined
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
void mmgsdi_free(void * ptr)
{
  if (ptr == NULL)
  {
    MSG_ERROR("Attempt to free null ptr, return", 0, 0, 0);
    return;
  }

  /* If the pointer happens to be a command pointer and if found in the UIM
     command queue, delete that entry.

     If the pointer happens to be an internal MMGSDI pointer, it wouldn't be
     found in the UIM command queue and, hence, will not get deleted.
  */

  uim_remove_command_from_uim_cmd_q(ptr);

#if !defined(FEATURE_LOW_MEMORY_USAGE) && defined(FEATURE_MMGSDI_USE_SMALL_HEAP)
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
#endif /* !FEATURE_LOW_MEMORY_USAGE && FEATURE_MMGSDI_USE_SMALL_HEAP */
}/* mmgsdi_free */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================

FUNCTION MMGSDI_CACHE_MEID_DATA

DESCRIPTION
  Caching MEID data from NV memory.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mmgsdi_cache_meid_data(void)
{
#ifndef TEST_FRAMEWORK
  nv_item_type meid_nv_data;
  memset((byte*)(&meid_nv_data), 0x00, sizeof(nv_item_type));
  if (NV_DONE_S == gsdi_get_nv(NV_MEID_ME_I, (nv_item_type *) &meid_nv_data))
  {
    MSG_HIGH("MEID is enabled", 0,0,0);
    mmgsdi_data_meid.meid_available = TRUE;
    qw_equ(mmgsdi_data_meid.meid, meid_nv_data.meid);
  }
  else
  {
    MSG_HIGH("MEID is not enabled", 0,0,0);
    mmgsdi_data_meid.meid_available = FALSE;    
  }
#else
  mmgsdi_data_meid.meid_available = FALSE;
#endif /* TEST_FRAMEWORK */
} /* mmgsdi_cache_meid_data */


/*===========================================================================
FUNCTION MMGSDI_GET_MEID

DESCRIPTION
  It returns cached MEID data.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_meid_data_type

SIDE EFFECTS
  None.
===========================================================================*/

mmgsdi_meid_data_type mmgsdi_get_meid(void)
{
  return mmgsdi_data_meid;
} /* mmgsdi_get_meid */

