/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       O V E R   T H E   A I R   S E R V I C E   P R O G R A M M I N G

GENERAL DESCRIPTION
  This module contains routines to process OTASP messages received on
  the Traffic Channel in a Data Burst Message.

EXTERNALIZED FUNCTIONS

  otasp_init          - Initialize OTASP
  otasp_process_msg   - Processes an OTASP message
  otasp_report_status - Report status of OTASP call to UI

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1996-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/otasp.c_v   1.11   02 Oct 2002 22:52:34   louiel  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/otasp.c#7 $ $DateTime: 2009/11/02 09:35:38 $ $Author: jyothij $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/27/09   jj      Fix to include esn filed while sending OTAPA req, 
                   in case FEATURE_CSIM is enabled.
10/19/09   ag      Added support for eHRPD IMSI OTASP operation.
09/14/09   ag      Fix to prevent a crash because of extra mcc_wait() call.
08/14/09   jj      Added support for CSIM feature.
07/29/09   ag      Fix to receive UIM reports irrespective of command status
06/01/09   ag      Featurized otasp session status under FEATURE_OTASP_STATUS
                   Added CM_CALL_REL_OTASP_SPC_ERR for backward compatibility
05/29/09   ag      Added support for sending OTASP session status to CM.
                   Removed CM_CALL_REL_OTASP_SPC_ERR from CP-CM interface.
05/11/09   jj      Fixed the issue of handling NAM blk download only for PCS
                   bands.
03/23/09   ag      Fixed handling of max SPC/SPASM validation failures
02/27/09   ag      Resetting SPC/SPASM failure count if new programming 
                   session starts and max attempts have not happened
02/27/09   ag      Added log messages
02/04/09   ag      Updated max SPC/SPASM validation attempts to 6 as per 
                   OTASP standard requirement (section 3.4 of C.S0016)
01/28/09   ag      Fixed handling of maximum invalid SPC validation attempts.
                   CP will not initiate the powerdown. UI may power cycle the phone.
10/16/08   ag      Added a feature FEATURE_MORE_OTASP_MAX_ULK_ATT for increased max unlock attempts.
                   Added powerdown support for incorrect SPC attempts for thin UI targets
06/30/08   pk      Mainlined feature FEATURE_EXTENDED_PRL  
21/03/08   bb      Fixed klocwork errors
08/16/07   pg      Re-initialized nam value for each PRL download.
03/19/07   pg      Lint Cleanup.
03/08/07   an      Fixed Critical Lint Errors
10/12/06   rp      Added support for BC15
06/22/06   jqi     Eliminate a copy of PRL in CP/OTASP module to reduce the 
                   memory by the size of MAX PRL size and use SD PRL for 
                   uploading.
06/20/06   jqi     Removed the error fatal and responsed with the max allowed
                   features upon the capability request message.
06/14/06   an      Fixed RUIM Build Lint Errors
05/30/06   fc      Fixed lint errors.
05/01/06   an      Fixed Lint Errors
04/07/06   an      Changed START_STOP to otapa_req_ptr->start_stop
12/20/05   sb      Replaced cm_mc_rpt_free_q with cm_mc_rpt_get_buf_else_err_fatal().
10/19/05   sb      Added support for Band Class 14, Band Subclass I, J, K.
09/29/05   sb      Removed DB_VOICE_PRIVACY.
07/04/05   ht      Added OTASP MEID support.
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   an      Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated 
                   FEATURE_CDMA_800 and FEATURE_CDMA_1900
11/30/04   jqi     Added FEATURE_IS683C_OTASP to complete OTASP support for 
                   IS683C PRL.                   
10/22/04   wli     Deleted features around function call otasp_get_prl_stats().
03/05/04   bkm     Lint cleanup.
09/12/03   wli     Fixed SPC and SPASM failure counters in RUIM mode
08/11/03   bkm     Lint cleanup.
07/08/03   jqi     Added Extended PRL support
                   -Removed FEATURE_IS683C/A_PRL
                   -Updated SD API prl_validate() with SSPR P REV parameter.                   
                   -Clean up prl_fixed_roaming_list_type for standeard OTASP.
07/01/03   RC/aaj  Added change to accept block ID 0 as well 1 in dload rsp msg
06/20/03   wli     Merged 683C feature from PVCS mainline.
02/03/03   sb      Pass nam and pref_mode into cmph_otasp_update_mode.
10/02/02   yll     Linted the code.
09/30/02   sb      Mainlined FEATURE_SD20.
08/30/02   ts      Added support for Run Time R-UIM Enable
06/21/02   ph      changed cmmc.h to cmxll.h
11/01/01   RC      Fixed incorrect /8 of PRL size in otasp_get_prl_stats()
                   (SD 2.0 only).
10/23/01   kk      Merge from main archive.
                   10/02/01   jrp     Merged from MSM_CP.05.00.96
09/24/01   fc      Fixed compiler warning caused by UIM code.
                   08/20/01   ck      Fixed the bug where an invalid PRL was committed when it was
                                      downloaded after a valid PRL.
08/16/01   kk      Merge from common archive.
                   08/13/01   wli     Commented out cdma.spc if RUIM feature is defined.
06/26/01   lh      Merged following from the common archive.
                   05/25/01   ts      Converted the otasp calls to the UIM from auth calls to the
                   new UIM server calls.
06/14/01   va      Changed references to CDMA_CONV to CDMA_TRAFFIC.
02/20/01   fc      Cleanup debug messages.
02/16/01   fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
01/28/01   cah     Fixed problem with Pref Mode not resetting after PRL dl.
12/22/00   ck      Fixed the problem where Re-Authentication Response message
                   was not being sent when VP or message encryption was on
                   during key exchange.
12/01/00   ck      Moved FEATURE_AUTH to include Re-Auth response messages
11/29/00   ck      Added support for R-UIM implementation of IS-683A.
10/02/00   yll     Fixed the handling of otasp_otapa_failed_spc_val_attempts
                   and otasp_otapa_failed_spasm_val_attempts flags for OTAPA
                   session.
05/30/00   ck      Added a null function to remove compiler warning
03/19/00   ry      Updated process_otapa_req() to process an OTAPA Request
                   Message even if FEATURE_AUTH is undefined by removing the
                   check for !defined(FEATURE_AUTH); instead, made changes to
                   reject the message if nam_locked_state is TRUE
           ry      Fixed a compiler warning in process_otapa_req() if
                   FEATURE_AUTH is defined but FEATURE_OTASP_OTAPA is not
03/14/00   cah     Fixed compile problem in process_validation_req() if
                   OTAPA not defined.
03/14/00   ry      Added OTAPA support
03/10/00   kmp     Modified process_config_req() to always use the imsi_m
                   parameters instead of the operation imsi parameters
                   for the CDMA NAM Block and the CDMA/Analog NAM Block.
                   Modified otasp_reload_nam_params() to load the operational
                   imsi when the min based imsi is changed if that is the
                   operational imsi type. Did the same if the true imsi is the
                   current operational imsi type.
03/31/00   ry      Removed otasp_report_status()
03/06/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
           fc      Changed process_dload_req to clear old SID NID pairs from
                   otasp commit buffer.
           ram     Merged in the following change from the main trunk revision
                   of otasp.c into the trimode branch -
12/08/99   kmp     Changed otasp_reload_nam_params() to also update the min
                   based imsi parameters not just the True IMSI and operational
                   imsi parameters.
10/08/99   ck      Item NV_PRL_ENABLED_I is not written to NV for a Trimode
                   target as a RL is must.
                   The preferred mode is reset to AUTOMATIC by writing to NV
                   if the RL is downloaded in that session.
07/28/99   lh      CM and two-way SMS support added.
06/30/99   kmp     When committing acp data, place a #ifdef FEATURE_ACP around
                   this code in otasp_reload_nam_params().
06/17/99   ck      Updated the acp structure with the newly downloaded acp data
                   upon commit.
                   Added checks to ensure sid nid list does not exceed the
                   OTASPI_SID_NID_MAX when uploading NAM blocks.
05/10/99   kmp     Merged in IS-95B changes. Changed cdma.min1 and cdma.min2
                   to cdma.imsi_s1 and cdma.imsi_s2 respectively in
                   otasp_reload_nam_params(). Changed cdma.min1 and cdma.min2
                   in process_reauth_req() and otasp_process_reauth_report()
                   to function calls mcc_get_auth_imsi_s1() and
                   mcc_get_auth_imsi_s2() respectively. These changes
                   correspond to changes made to the cdma_ms_type data structure
                   in mccdma.h for IS-95B True IMSI support.
04/28/99   ck      Featurised the Key Exchange capability in FEATURE_DH_EXP in
                   the protocol capability response.
04/23/99   ck      Fixed the problem where the CDMA/Analog NAM Block was being
                   accepted in Non-std OTASP.
04/16/99   ck      Added capability to upload and download more than one
                   sid-nid pair. Changed T_AUTH to FEATURE_AUTH.
                   Initialized the roaming list download buffer to zeroes.
04/08/99   ck      Fixed the problem where the key response msg was sent in
                   place of key gen rsp msg when AUTH is not defined.
03/31/99   abh     Removed the use of imsi_s.
03/18/99   ck      Removed the feature and code under OTASP_800_HACK
03/18/99   ck      Merged in the changes from SPSW tip.
02/25/99   ck      Fixed the problem where the MS was sending the prev sent data
                   burst msg again when both NAM params and A-Key are committed
                   and Commit request is recd from the BS.
                   Fixed the problem where the RL upload failed when the size of
                   the RL was greater than the requested segment size.
02/23/99   ck      Changed the commit response to send the result code based
                   on whether Non Std OTASP is defined or not.
02/12/99   ck      Used mcc_roaming_list to upload the roaming list in the
                   SSPR Config Req msg and removed FEATURE_CLASS_1_IMSI.
01/20/99   ck      Added support for AKey exchange and Re-Authentication and
                   also support for IS-683A.
08/04/98   ych     Merged SSPR, E911 and Acquisition improvements.
07/20/98   pms     Added otasp_aho_handoff() and otasp_set_aho_handoff()
                   to handle OTASP call handoff to AMPS.
01/28/98   wjn     Call mcsys_get_prl_version() to set a new NV PRL field -
                   prl_version - after mcsys_validate_roaming_list() but
                   before writting the PRL structure to NV.
07/29/97   dna     Call NV function to get RL max size instead of constant.
04/18/97   dna     Made FEATURE_PRL_DOWNLOAD separate from FEATURE_PREFERRED_
                   ROAMING for OTASP_800_HACK.  Support Cellular Indicator
                   Data.  Support OTASP_800_HACK.  Support Dual Band OTASP.
02/11/97   fkm     (per dna) followed change of prl_fixed_roaming_list_type
                   as per prl.h
11/13/96   dna     Test and mark validity of roaming list before commit.
09/13/96   dna     Set roaming List Dimensions correctly.  Keep track and
                   report if something is committed.
08/26/96   dna     Fixed rl dload block retransmission, set dimensions.
08/08/96   dna     OTASP phase 2; subsidy lock and pr list download.
07/02/96   dna     Set IMSI_ADDR_NUM to zero for class 0 IMSI.
06/26/96   dna     Added special processing for download during commit.
06/25/96   dna     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"

#ifdef FEATURE_OTASP

#include "memory.h"
#include "bit.h"
#include "comdef.h"
#include "err.h"
#include "ran.h"
#include "mccdma.h"
#include "mccap.h"
#include "mcctcsup.h"
#include "nv.h"
#include "otaspe.h"
#include "otaspi.h"
#include "otaspx.h"
#include "queue.h"
#include "qw.h"
#include "cmxll.h"
#include "cm.h"
#include "cmph.h"
#include "prl.h"
#include "crc.h"
#ifdef FEATURE_DH
#include "dh.h"
#endif /* FEATURE_DH */
#ifdef FEATURE_AUTH
#include "auth.h"
#endif
#ifdef FEATURE_UIM_RUIM
#include "uim.h"
#include "mci.h"
#endif /* FEATURE_UIM_RUIM */
#include "diag.h"
#include "mccccim.h"

/*lint -e818 */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.


===========================================================================*/

#ifdef FEATURE_MORE_OTASP_MAX_ULK_ATT
#define OTASP_MAX_UNLOCK_ATTEMPTS  15
#else
#define OTASP_MAX_UNLOCK_ATTEMPTS  6
#endif

/* ------------------------------------------------------------------
** Two nv_otasp_commit_type buffers are needed in case a new download
** request is received before the commit operation completes.
** ------------------------------------------------------------------ */
LOCAL nv_otasp_commit_type commit;   /* Buffer to store info to commit */
LOCAL nv_otasp_commit_type nv_commit;/* Buffer for NV commit command */

#ifdef FEATURE_EHRPD
LOCAL boolean commit_ehrpd_imsi;
LOCAL nv_ehrpd_imsi_type otasp_ehrpd_imsi;
#endif /* FEATURE_EHRPD */

LOCAL otaspi_rtc_msg_type otasp_int; /* Buffer to store response message */

#ifdef FEATURE_UIM_RUIM
otaspi_rtc_msg_type ruim_otasp_int;  /* Buffer to store response message */
#endif /* FEATURE_UIM_RUIM */

/* Flag if NV is processing commit cmd */
LOCAL boolean otasp_commit_in_progress;

/* Flag if anything is committed to NV */
LOCAL boolean otasp_successful_commit;

LOCAL nv_cmd_type otasp_nv_buf;      /* Buffer for command to NV task */
LOCAL nv_item_type otasp_nv_item;  /* Item to hold LIST_ENABLED */
#ifdef FEATURE_DH_EXP
LOCAL otasp_dh_data otasp_dh;        /* Buffer to store otasp_dh data  */
#endif /* FEATURE_DH_EXP */

LOCAL otaspi_roam_siz_type otasp_roam_size; /* Dimensions of roaming list */
extern nv_roaming_list_type nv_pr_list; /* The Preferred Roaming List */

#ifdef FEATURE_MEM_REDUCE_PRL
/* Indicate if the nv_pr_list is used for SSPR cfg rsp for uploading the PRL. 
**
** At power up, it initializes to FALSE
** At SSPR download req/Download req, it update to FALSE.
** At commit, it update as TRUE.
*/
LOCAL boolean otasp_is_nv_pr_list_valid; 
#endif /* FEATURE_MEM_REDUCE_PRL */

#ifdef FEATURE_NSOTASP
LOCAL boolean otasp_commit_allowed;  /* Flag if the phone is locked */
#endif

LOCAL byte otasp_failed_unlock_attempts = 0;   /* Number of times bad SPC rxed */
#ifdef FEATURE_NSOTASP
LOCAL byte otasp_rl_seq;   /* Next expected Roaming List block seqence num */
#endif /* FEATURE_NSOTASP */

LOCAL boolean otasp_call_ho_from_cdma;
/* Flag if OTASP has been handed off to AMPS */

#ifdef FEATURE_AUTH
boolean  otasp_reauth_in_progress; /* flag if reauthentication is in progress */

LOCAL dword reauth_req_rand;
    /*Stores the rand from the reauthentication req message */
#endif /*FEATURE_AUTH*/

boolean otasp_akey_commit_result;
 /* flag to indicate the result of the a-key committ operation */

LOCAL boolean sp_locked_state;
    /* boolean flag to indicate if the MS is locked */

LOCAL boolean nam_locked_state;
    /* boolean flag to indicate if SPASM protects access to NAM parameters */

LOCAL boolean otasp_committed_both_flag = FALSE;
  /* Flag to indicate if NAM parameters and AKey have been committed
     before sending the response    */

LOCAL boolean otasp_commit_akey_pending;
  /* Flag to indicate if AKey is to be committed */

#ifndef FEATURE_NSOTASP
LOCAL boolean new_roaming_list = TRUE;
    /* To indicate if a new roaming list is to be downloaded and this is
    ** used to initialize the roaming list */
#endif

#ifdef FEATURE_OTASP_OTAPA
LOCAL byte otasp_otapa_failed_spc_val_attempts = 0;
   /* Number of times bad SPC validation rxed */
LOCAL byte otasp_otapa_failed_spasm_val_attempts = 0;
   /* Number of times bad SPASM validation rxed */
LOCAL boolean otasp_otapa_session_occurred;
    /* Flag if an OTAPA session has occurred */

#ifdef FEATURE_AUTH
LOCAL boolean  otasp_auth_otapa_in_progress;
   /* flag if auth_otapa calculation is in progress */
LOCAL dword otasp_auth_otapa;
    /* Store the auth_otapa from auth_signature */
LOCAL boolean otasp_auth_otapa_ready;
    /* If auth_otapa is not ready, then need to calculate it */
#endif /* FEATURE_AUTH */

#if (defined(FEATURE_AUTH) || defined(FEATURE_UIM_RUIM))
LOCAL dword rand_otapa;
    /* Stores the rand from the processing otapa request message */
#endif /* FEATURE_AUTH || FEATURE_UIM_RUIM */
#endif /* FEATURE_OTASP_OTAPA */

#ifdef FEATURE_UIM_RUIM
/* Table for maintaining info on the request messages */
otasp_ruim_req_rsp_table_type otasp_ruim_table;

/* Flag to indicate if we are sending the last segment to the card */
/* This helps in figuring out if the PRL has been committed in the card */
boolean otasp_ruim_last_segment;
/* The byte position of SSPR Request Response block */
#define PRL_NUM_REC_INDEX  6
#endif /* FEATURE_UIM_RUIM */

/* Used when validate PRL.
** updated when processing SSPR download request.
** and cleared after processing commit message.
*/
LOCAL prl_sspr_p_rev_e_type sspr_p_rev = PRL_SSPR_P_REV_INVALID;

/*===========================================================================

FUNCTION otasp_get_prl_stats

DESCRIPTION
  Get information about the PRL for OTASP configuration request processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void otasp_get_prl_stats
(
  otaspi_roam_siz_type *rl_size,   /* Structure to store Roaming List stats*/
  
  const prl_hdr_s_type *prl_hdr_ptr /* Point at NV PRL information structure*/
)
{
  rl_size->max_pr_list_size = nv_max_size_of_roaming_list() ;
  /* The function in NV returns the number of bytes  and
     the number of octets is sent over the air */

  rl_size->cur_pr_list_size = prl_hdr_ptr->pr_list_size;
  /* The size of the roaming list in bytes as the number of octets are
     sent over the air */

  rl_size->num_acq_recs     = prl_hdr_ptr->num_acq_recs;

  rl_size->num_sys_recs     = prl_hdr_ptr->num_sys_recs;

  rl_size->pr_list_id       = prl_hdr_ptr->pr_list_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("Size=%d, ACQ=%d, SYS=%d", rl_size->cur_pr_list_size,
    rl_size->num_acq_recs, rl_size->num_sys_recs);

  rl_size->cur_sspr_p_rev = prl_hdr_ptr->sspr_p_rev; /*lint !e641 */
  /* Protocol revision of current preferred roaming list. */

  if( prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3 )
  {
    rl_size->num_comm_subnet_recs = prl_hdr_ptr->num_subnet_recs;
    /* Number of records in the common subnet table.*/

    MSG_MED("Num comm subnet=%d", rl_size->num_comm_subnet_recs, 0, 0);
  }

} /* otasp_get_prl_stats */

/*===========================================================================

FUNCTION OTASP_GET_RL_STATS

DESCRIPTION
  This function returns information about the Preferred Roaming List for
  OTASP configuration request processing.

DEPENDENCIES
  None.

RETURN VALUE
  Fills in structure with MAX_PR_LIST_SIZE, CUR_PR_LIST_SIZE, NUM_ACQ_RECS,
  and NUM_SYS_RECS.

SIDE EFFECTS
  None.

===========================================================================*/

void otasp_get_rl_stats
(
  otaspi_roam_siz_type *rl_siz
    /* Structure to store Roaming List stats */
)
{
  byte           nv_sspr_p_rev = PRL_SSPR_P_REV_INVALID;
  prl_hdr_s_type prl_hdr, * prl_hdr_ptr = &prl_hdr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get sspr p rev from NV. Based on sspr p rev info, return requested data.
  */
  otasp_nv_buf.cmd = NV_READ_F;
  otasp_nv_buf.data_ptr   = (nv_item_type*) &nv_sspr_p_rev; //lint !e789 !e826
    // lint 789 - assigning auto value to static value.  Lint is flagging
    // that when this function ends, nv_sspr_p_rev goes out of scope and
    // otasp_nv_buf.data_ptr is left pointing to who-knows-what. Since
    // otasp_nv_buf.data_ptr is also set right before it is used, this is
    // not an issue. 
  otasp_nv_buf.item       = NV_SSPR_P_REV_I;
  (void) mcc_nv_cmd( &otasp_nv_buf);
  
  /* Extract header of PRL*/
  
  prl_hdr_extract( prl_hdr_ptr, 
                   (byte *)nv_pr_list.roaming_list, 
                   (prl_sspr_p_rev_e_type)nv_sspr_p_rev );

  /* Populate the PRL dimension information. */
  otasp_get_prl_stats( rl_siz, prl_hdr_ptr );

  #ifdef FEATURE_MEM_REDUCE_PRL
  /* Use this buffer for SSPR cfg request for uploading PRL. */
  otasp_is_nv_pr_list_valid = TRUE;
  MSG_HIGH(" COMMIT: otasp_is_nv_pr_list_valid=%d",
           otasp_is_nv_pr_list_valid, 0, 0);
  #endif /* FEATURE_MEM_REDUCE_PRL */

} /* otasp_get_rl_stats */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION SEND_CFG_RESP_MSG

DESCRIPTION
   This function sends the Configuration Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_cfg_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the Configuration Response Message */

  ruim_otasp_int.config_rsp.msg_type = OTASP_CONFIG_RSP_MSG;
  status = otaspx_int_to_ext( &len, &ruim_otasp_int, chari );
  if (status == OTASPX_DONE_S) /*lint !e641 */
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_cfg_resp_msg */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION PROCESS_CONFIG_REQ

DESCRIPTION
  This function processes an OTASP Configuration Request Message,
  formats the appropriate Configuration Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_config_req
(
  otaspi_config_req_msg_type* config_req_ptr
    /* Pointer to Configuration Request Message */
)
{
  word x;
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE)
  byte i;
#ifdef OTASP_CDMA_NAM_RULE
  otaspi_rtc_pcs_data_type *pcs; /* Shorthand for PCS data */
#endif
#if (!defined( FEATURE_NSOTASP ) || defined( FEATURE_ACP ))
  otaspi_rtc_cell_data_type *cell; /* Shorthand for Cellular data */
#endif
#ifndef FEATURE_NSOTASP
  otaspi_imsi_t_data_type *imsi;      /* Shorthand for IMSI_T data */
#endif /* FEATURE_NSOTASP */
#ifdef FEATURE_EHRPD
  otaspi_ehrpd_imsi_data_type *ehrpd_imsi; /* Shorthand for eHRPD_IMSI data */
#endif /* FEATURE_EHRPD */
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
#if defined( FEATURE_UIM_RUIM )
  byte rej_block_index = 0;    /* index for tracking rejected blocks */
  uim_cmd_type *uim_cmd_ptr;   /* Command */
#endif /* FEATURE_UIM_RUIM */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* Initialize the number of blocks req to and recd from the card */
    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_card = 0;

    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_rsp_card = 0;

    /* Check to see if a message buffer is available */
    if ((otasp_ruim_table.req_index + 1) % OTASP_RUIM_MAX_MSG_BUFS !=
                                   otasp_ruim_table.rsp_index)
    {
      /* Store the message type */
      otasp_ruim_table.msg[otasp_ruim_table.req_index].msg_type =
                          OTASP_CONFIG_REQ_MSG;

      /* Store the number of blocks requested in the message */
      otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_msg =
                config_req_ptr->num_blocks;

      /* send a command for each block to the card */
      for (x = 0; x < config_req_ptr->num_blocks; x++)
      {
        if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
        {
           uim_cmd_ptr->hdr.command = UIM_CONFIG_REQ_F;

           uim_cmd_ptr->configuration.block_id = config_req_ptr->block_id[x];

           uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

           mcc_uim_cmd( uim_cmd_ptr );

           if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
           {
               ERR( "Bad status from UIM %d", uim_cmd_ptr->hdr.status,0,0);

               /* Store the id of the block that is not processed by the card */
               otasp_ruim_table.msg[otasp_ruim_table.req_index].
                   rej_block_id[rej_block_index++] = config_req_ptr->block_id[x];
           }
           else
           {
               /* Increment the number of blocks sent to the card */
               ++(otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card);
           }
        }
        else
        {
           ERR( "No free buffers on uim_free_q", 0, 0, 0 );

           /* Store the id of the block that is not sent to the card */
           otasp_ruim_table.msg[otasp_ruim_table.req_index].
               rej_block_id[rej_block_index++] = config_req_ptr->block_id[x];
        }
      }   /* end of for loop */
    } /* check for message buffer availability */

    /* If any block has been sent to the card successfully */
    if (otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card > 0)
    {
        /* consider this message queued and increment the request index */
        otasp_ruim_table.req_index = (otasp_ruim_table.req_index + 1) %
                                    OTASP_RUIM_MAX_MSG_BUFS;
    }
    else
    {
        /* none of the blocks are sent to the card */
        ruim_otasp_int.config_rsp.num_blocks = config_req_ptr->num_blocks;

        /* send a reject response for all the blocks in this message */
        for (x = 0; x < config_req_ptr->num_blocks; x++)
        {
            ruim_otasp_int.config_rsp.block[x].block_id =
                                       config_req_ptr->block_id[x];

            ruim_otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_UNKNOWN;
            ruim_otasp_int.config_rsp.block[x].block_len = 0;
        }

        /* send the configuration response message */
        send_cfg_resp_msg();
    }
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    otasp_int.config_rsp.msg_type = OTASP_CONFIG_RSP_MSG;
    otasp_int.config_rsp.num_blocks = config_req_ptr->num_blocks;

    for (x = 0; x < config_req_ptr->num_blocks; x++)
    {
      otasp_int.config_rsp.block[x].block_id = config_req_ptr->block_id[x];

      /* --------------------------------------------------------------
      ** The block_len for each block is filled in later by the packing
      ** routines, and is set to zero if the block is not supported.

         If SP_LOCKED_STATE or NAM_LOCKED_STATE is TRUE, then we
         explicitly set the block len to zero to make sure that
         the blocks are not processed during translation.
      ** -------------------------------------------------------------- */
  #ifndef FEATURE_NSOTASP
      if ((sp_locked_state) || (nam_locked_state))
      {
          otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_MS_LOCKED;
          otasp_int.config_rsp.block[x].block_len = 0;
      }
      else if ((!sp_locked_state) && (!nam_locked_state))
      {
  #endif /*FEATURE_NSOTASP */

       switch (config_req_ptr->block_id[x])
       {

        case OTASP_MOB_DIR_NUM_BLK: /* Mobile Directory Number */

          MSG_MED("Reporting mob_dir",0,0,0);
          otasp_int.config_rsp.block[x].blk.mob_dir.n_digits =
            cdma.mob_dir.n_digits;
          (void) memcpy( otasp_int.config_rsp.block[x].blk.mob_dir.digitn,
                         cdma.mob_dir.digitn, cdma.mob_dir.n_digits );
  #ifndef FEATURE_NSOTASP
          /* this is not the actual length, but is set to 1 to make
          sure that this block gets processed during translation */
          otasp_int.config_rsp.block[x].block_len = 1;
          otasp_int.config_rsp.result_code[x] = OTASP_BLK_ACCEPT;
  #endif /* FEATURE_NSOTASP */

        break;

        case OTASP_CDMA_ANALOG_NAM_BLK: /* CDMA/Analog NAM Download Data */
  #if (!defined( FEATURE_NSOTASP) || defined( FEATURE_ACP ))
          /* -------------------------------------------------------------
          ** CDMA NAM information is still reported even if the phone does
          ** not support CDMA at 800 MHz e.g. a dual band phone.  The PCS
          ** band CDMA NAM information is reported instead.
          ** ------------------------------------------------------------- */
          MSG_MED("Reporting CDMA Analog NAM data",0,0,0);
          cell = &otasp_int.config_rsp.block[x].blk.rtc_cell;

          /* Fill in home_sid, firstchp, and ex */
  #ifdef FEATURE_ACP
#error code not present
  #else
          /* This phone doesn't support analog. Set analog fields to zero */
          cell->home_sid = 0;
          cell->firstchp = 0;
          cell->ex = 0;
  #endif /* FEATURE_ACP */

          cell->scm = cdma.scm;
          cell->mob_p_rev = cdma.mob_cai_rev;
          cell->imsi_class =
             (cdma.imsi_m_addr_num == IMSI_CLASS_0_ADDR_NUM)? 0 : 1;
          cell->imsi_addr_num = (cdma.imsi_m_addr_num == IMSI_CLASS_0_ADDR_NUM)?
                               0 : cdma.imsi_m_addr_num;
          cell->mcc = cdma.imsi_m_mcc;
          cell->imsi_11_12 = cdma.imsi_m_11_12;
          qw_equ( cell->imsi_s, cdma.imsi_m_s );
          cell->accolc = cdma.accolc;
          cell->local_control = 0;  /* We don't support any local control opt */
          cell->mob_term_home = cdma.mob_term_home;
          cell->mob_term_for_sid = cdma.mob_term_for_sid;
          cell->mob_term_for_nid = cdma.mob_term_for_nid;
          cell->max_sid_nid = OTASPI_SID_NID_MAX;

          /* Initialize the counter */
          i = 0;

          /* Upload the sid nid list until you encounter a sid = 0 */
          while ((i < OTASPI_SID_NID_MAX) && (cdma.pair[i].sid != 0))
          {
            cell->var[i].sid = cdma.pair[i].sid;
            cell->var[i].nid = cdma.pair[i].nid;
            i++;
          }
          cell->stored_sid_nid = i;  /*lint !e661 */

  #ifndef FEATURE_NSOTASP
          otasp_int.config_rsp.result_code[x] = OTASP_BLK_ACCEPT;
          otasp_int.config_rsp.block[x].block_len = 1;
  #endif /* FEATURE_NSOTASP */

  #else
          /* Block ID Not Supported */
  #ifndef FEATURE_NSOTASP
          otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_BLOCK_ID;
          otasp_int.config_rsp.block[x].block_len = 0;
  #endif /* FEATURE_NSOTASP  */

  #endif /* !FEATURE_NSOTASP OR FEATURE_ACP */
        break;

        case OTASP_CDMA_NAM_BLK: /* CDMA NAM Download Data */
  #ifdef OTASP_CDMA_NAM_RULE
          MSG_MED("Reporting CDMA NAM data",0,0,0);
          pcs = &otasp_int.config_rsp.block[x].blk.rtc_pcs;

          pcs->slotted_mode = TRUE;
          pcs->mob_p_rev = cdma.mob_cai_rev;
          pcs->imsi_class = (cdma.imsi_m_addr_num == IMSI_CLASS_0_ADDR_NUM)?0:1;
          pcs->imsi_addr_num = (cdma.imsi_m_addr_num == IMSI_CLASS_0_ADDR_NUM)?
                                0 : cdma.imsi_m_addr_num;
          pcs->mcc = cdma.imsi_m_mcc;
          pcs->imsi_11_12 = cdma.imsi_m_11_12;
          qw_equ( pcs->imsi_s, cdma.imsi_m_s );
          pcs->accolc = cdma.accolc;
          pcs->local_control = 0;  /* We don't support any local control opt */
          pcs->mob_term_home = cdma.mob_term_home;
          pcs->mob_term_for_sid = cdma.mob_term_for_sid;
          pcs->mob_term_for_nid = cdma.mob_term_for_nid;
          pcs->max_sid_nid = OTASPI_SID_NID_MAX;

          i = 0;

          /* Upload the sid nid list until you encounter a sid = 0 */
          while ((i < OTASPI_SID_NID_MAX) && (cdma.pair[i].sid != 0))
          {
            pcs->var[i].sid = cdma.pair[i].sid;
            pcs->var[i].nid = cdma.pair[i].nid;
            i++;
          }
          pcs->stored_sid_nid = i; /* Number of sid nid entries */ /*lint !e661 */
    #ifndef FEATURE_NSOTASP
          otasp_int.config_rsp.result_code[x] = OTASP_BLK_ACCEPT;
          otasp_int.config_rsp.block[x].block_len = 1;
    #endif /* FEATURE_NSOTASP */
          
  #else
          /* Block ID Not Supported */
          /* result code is reject only if it is standard OTASP */
  #ifndef FEATURE_NSOTASP
          otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_BLOCK_ID;
          otasp_int.config_rsp.block[x].block_len = 0;
  #endif /* FEATURE_NSOTASP  */

  #endif /* OTASP_CDMA_NAM_RULE */
        break;

        case OTASP_PREF_ROAM_LIST_SIZE_BLK: /* Roaming List Dimensions */
  #ifndef FEATURE_NSOTASP
          otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_BLOCK_ID;
          otasp_int.config_rsp.block[x].block_len = 0;
  #else
          /* Report dimensions of max and current Roaming List */
          otasp_int.config_rsp.block[x].blk.roam_siz = otasp_roam_size;
  #endif /* FEATURE_NSOTASP  */

        break;

        case OTASP_PREF_ROAM_LIST_BLK: /* Roaming List */
        /* Currently uploading the Roaming List is not supported */
  #ifndef FEATURE_NSOTASP
           otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_BLOCK_ID;
           otasp_int.config_rsp.block[x].block_len = 0;
  #else
  //        otasp_int.config_rsp.block[x].blk.rtc_roam =
  //          mcsys_get_roam_list();
  #endif /* FEATURE_NSOTASP  */
        break;

  #ifndef FEATURE_NSOTASP
        case OTASP_IMSI_T_BLK:
           otasp_int.config_rsp.block[x].block_len = 1;

           MSG_MED("Reporting IMSI data",0,0,0);
           imsi = &otasp_int.config_rsp.block[x].blk.imsi_t;
           imsi->imsi_t_class =
                 (cdma.imsi_t_addr_num == IMSI_CLASS_0_ADDR_NUM)? 0 : 1;
           imsi->imsi_t_addr_num =
                 (cdma.imsi_t_addr_num == IMSI_CLASS_0_ADDR_NUM)?
                               0 : cdma.imsi_t_addr_num;
           imsi->imsi_t_mcc =  cdma.imsi_t_mcc;
           imsi->imsi_t_11_12 =  cdma.imsi_t_11_12;
           qw_equ( imsi->imsi_t_s, cdma.imsi_t_s );
           otasp_int.config_rsp.result_code[x] = OTASP_BLK_ACCEPT;
        break;
  #endif /* FEATURE_NSOTASP */

#ifdef FEATURE_EHRPD
        case OTASP_EHRPD_IMSI_BLK:
        {
          int imsi_index = 0;

          MSG_MED("Reporting eHRPD IMSI data",0,0,0);
          ehrpd_imsi = &otasp_int.config_rsp.block[x].blk.ehrpd_imsi;
          /* Read from NV */
          otasp_nv_buf.cmd = NV_READ_F;
          otasp_nv_buf.data_ptr = &otasp_nv_item;
          otasp_nv_buf.item = NV_EHRPD_IMSI_I;
          if (mcc_nv_cmd( &otasp_nv_buf ) == NV_DONE_S)
          {
            if (otasp_nv_item.ehrpd_imsi.num_of_mnc_digits == 2)
            {
              ehrpd_imsi->ehrpd_mnc_num_digits = 0;
            }
            else if (otasp_nv_item.ehrpd_imsi.num_of_mnc_digits == 3)
            {
              ehrpd_imsi->ehrpd_mnc_num_digits = 1;
            }
            else
            {
              MSG_MED("Invalid number of mnc digits %d", 
                otasp_nv_item.ehrpd_imsi.num_of_mnc_digits, 0, 0);
              otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_UNKNOWN;
              otasp_int.config_rsp.block[x].block_len = 0;
              break;
            }

            for(i = 0; i < otasp_nv_item.ehrpd_imsi.num_of_mcc_digits; i++)
            {
              ehrpd_imsi->ehrpd_imsi_digits[imsi_index] = otasp_nv_item.ehrpd_imsi.ehrpd_mcc[i];
              imsi_index++;
            }

            for(i = 0; i < otasp_nv_item.ehrpd_imsi.num_of_mnc_digits; i++)
            {
              ehrpd_imsi->ehrpd_imsi_digits[imsi_index] = otasp_nv_item.ehrpd_imsi.ehrpd_mnc[i];
              imsi_index++;
            }

            for(i = 0; i < otasp_nv_item.ehrpd_imsi.num_of_msin_digits; i++)
            {
              ehrpd_imsi->ehrpd_imsi_digits[imsi_index] = otasp_nv_item.ehrpd_imsi.ehrpd_msin[i];
              imsi_index++;
            }

            ehrpd_imsi->ehrpd_imsi_num_digits = imsi_index;

            otasp_int.config_rsp.result_code[x] = OTASP_BLK_ACCEPT;
            otasp_int.config_rsp.block[x].block_len = 1;
          }
          else
          {
            MSG_MED("Failed to read item eHRPD IMSI from NV",0,0,0);
            otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_UNKNOWN;
            otasp_int.config_rsp.block[x].block_len = 0;
          }
        }
        break;
#endif /* FEATURE_EHRPD */

        default:
          /* Block ID Not Supported */
  #ifndef FEATURE_NSOTASP
          otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_BLOCK_ID;
          otasp_int.config_rsp.block[x].block_len = 0;
  #endif /* FEATURE_NSOTASP  */

        break;

       } /* end switch */
  #ifndef FEATURE_NSOTASP
      }
  #endif /* FEATURE_NSOTASP */
    } /* end for */

    /* Translate and send the Configuration Response Message */

    status = otaspx_int_to_ext( &len, &otasp_int, chari );

    if (status == OTASPX_DONE_S)  /*lint !e641 */
    {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
    }
    else
    {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

} /* process_config_req */

#ifdef FEATURE_NSOTASP
/*===========================================================================

FUNCTION APPEND_RL

DESCRIPTION
  This function takes a received part of a roaming list and appends
  it to the roaming list being accumulated in memory.

DEPENDENCIES
  The first of the received roaming list bits must be byte alligned.

RETURN VALUE
  TRUE if the append was successful, FALSE if it did not fit into memory.

SIDE EFFECTS
  None.

===========================================================================*/

boolean append_rl
(
  byte *bits_ptr,
    /* Index into the caii_rx_msg_type where the Roaming List piece begins */
  word num_bits,
    /* How many bits are in this piece */
  boolean reset,
    /* Whether or not this is the first piece of a roaming list */
  boolean overwrite
    /* Whether or not we are to overwrite the last block received */
)
{
  word num_whole_bytes;   /* Number of full bytes of roaming list received */
  word proposed_size;     /* Projected size of list if piece is added */
  static byte *index_ptr; /* Pointer to last byte received so far */
  static byte offset;     /* Offset of last bit in byte (0-7) */
  static byte *last_index_ptr = NULL; /* Index before last append */
  static byte last_offset = 0;        /* Offset before last append */
  static word last_size = 0;          /* Number of bits in last append */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if we will exceed max size by appending this piece */

  proposed_size = (reset) ? num_bits : (nv_pr_list.size + num_bits);
  if (proposed_size > (8 * nv_max_size_of_roaming_list()))
  {
    /* Adding this piece will exceed our max size, so don't do it */
    return (FALSE);
  }

  /* It is OK to add this piece */

  num_whole_bytes = num_bits / 8;

  /* ------------------------------------------------------------
  ** Overwrite means we overwrite the last piece.  Reset means we
  ** overwrite the entire list.  So reset overrides overwrite.
  ** ------------------------------------------------------------ */
  if (reset)
  {
    /* Initialize nv_pr_list and static variables */
    nv_pr_list.nam = cdma.curr_nam;
    nv_pr_list.size = 0;
    index_ptr = (byte *) nv_pr_list.roaming_list;
    offset = 0;
  }
  else if (overwrite)
  {
    /* -------------------------------------------------
    ** Undo changes from last append, so we can redo it.
    ** Check for NULL just to be safe.
    ** ------------------------------------------------- */
    if (last_index_ptr != NULL)
    {
      index_ptr = last_index_ptr;
      offset = last_offset;
      nv_pr_list.size = last_size;
    }
  }

  /* Save list state information in case we need to overwrite this piece */
  last_index_ptr = index_ptr;
  last_offset = offset;
  last_size = nv_pr_list.size;

  /* Increase size to include the new bits */
  nv_pr_list.size += num_bits;

  /* Check for null index_ptr just to be safe. */
  if (index_ptr != NULL)
  {
    if (offset == 0)
    {
       /* Everything is byte alligned, so we can do a simple memcpy */
       (void) memcpy( index_ptr, bits_ptr, num_whole_bytes );
       index_ptr += num_whole_bytes;

       /* --------------------------------------------------------------------
       ** If num_bits isn't a multiple of 8, we need bits from next byte, too.
       ** It is OK to copy the whole byte since the overall length tells how
       ** many VALID bits there are.
       ** --------------------------------------------------------------------*/
       if ((offset = num_bits % 8) != 0)
       {
          *index_ptr = *(bits_ptr + num_whole_bytes);
       }
    }
    else
    {
       /* We need to shift byte alligned source to unalligned destination */
       b_copy( bits_ptr, 0, index_ptr, offset, num_bits );
       index_ptr += num_whole_bytes;

       if ((offset + (num_bits % 8)) >= 8)
       {
          /* We have overflowed into the next byte, so move index one more */
          index_ptr++;
       }
       offset = (num_bits + offset) % 8;
    }
  }
  return (TRUE);

} /* append_rl() */
#endif /* FEATURE_NSOTASP  */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION SEND_DLOAD_RESP_MSG

DESCRIPTION
   This function sends the Download Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_dload_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the Download Response Message */
  ruim_otasp_int.dload_rsp.msg_type = OTASP_DOWNLOAD_RSP_MSG;
  status = otaspx_int_to_ext( &len, &ruim_otasp_int, chari );
  if (status == OTASPX_DONE_S)  /*lint !e641 */
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_dload_resp_msg */

#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION PROCESS_DLOAD_REQ

DESCRIPTION
  This function processes an OTASP Download Request Message,
  formats the appropriate Download Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  Next state to be processed.  CDMA_CONV or CDMA_EXIT for powerdown.

SIDE EFFECTS
  None.

===========================================================================*/

word process_dload_req
(
  otaspi_download_req_msg_type* dload_req_ptr
    /* Pointer to Download Request Message */
)
{
  word x;    /* loop counters */
  word next_state; /* Next state to be processed */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  word y;   /* loop counter */
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
#ifdef OTASP_CDMA_NAM_RULE
  otaspi_ftc_pcs_data_type *pcs; /* Shorthand for PCS NAM information */
#endif
#if (!defined( FEATURE_NSOTASP) || defined( FEATURE_ACP ))
  otaspi_ftc_cell_data_type *cell; /* Shorthand for cellular NAM information */
#endif
#ifdef FEATURE_NSOTASP
  otaspi_ftc_roam_lst_type *roam; /* Pointer to ftc_roam items */
  boolean reset; /* Flag if we sould reset roaming list */
#endif /* FEATURE_NSOTASP */
#ifndef FEATURE_NSOTASP
  otaspi_imsi_t_data_type *imsi_t; /* Pointer to imsi_t items */
#endif
#ifdef FEATURE_EHRPD
  otaspi_ehrpd_imsi_data_type *ehrpd_imsi; /* Shorthand for eHRPD_IMSI data */
#endif /* FEATURE_EHRPD */
#endif /* |FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

#if defined( FEATURE_UIM_RUIM )
  byte rej_block_index = 0;    /* index for tracking rejected blocks */
  uim_cmd_type *uim_cmd_ptr; /* Command */
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_NSOTASP  
  mccccim_event_type mccccim_evt;
    /* Event rpt struct for sending primitives to CCIM */
#endif /* FEATURE_NSOTASP */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ---------------------------------------------------------
  ** OTASP messages are only processed in CDMA_CONV substate.
  ** Currently next_state is only changed if too many attempts
  ** are made to unlock the phone, in which case we powerdown.
  ** --------------------------------------------------------- */
  next_state = CDMA_TRAFFIC; /*lint !e641 */

  /* The next state is not changed as the attempts are made to unlock
     the phone using this message only for Non-std OTASP and R-UIm does
     not support NS OTASP */

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* Initialize the number of blocks req to and recd from the card */
    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_card = 0;

    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_rsp_card = 0;

    /* Check to see if a message buffer is available */
    if ((otasp_ruim_table.req_index + 1) % OTASP_RUIM_MAX_MSG_BUFS !=
                                   otasp_ruim_table.rsp_index)
    {
      /* Store the message type */
      otasp_ruim_table.msg[otasp_ruim_table.req_index].msg_type =
                          OTASP_DOWNLOAD_REQ_MSG;

      /* Store the number of blocks requested in the message */
      otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_msg =
                dload_req_ptr->num_blocks;

      /* send a command for each block to the card */
      for (x = 0; x < dload_req_ptr->num_blocks; x++)
      {
         if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
         {
            uim_cmd_ptr->hdr.command = UIM_DOWNLOAD_REQ_F;

            uim_cmd_ptr->download.block_id = dload_req_ptr->block[x].block_id;

            uim_cmd_ptr->download.block_len = dload_req_ptr->block[x].block_len;

            /* Copy the data that is downloaded via OTASP to the command buffer */
            memcpy(uim_cmd_ptr->download.data,dload_req_ptr->block[x].blk.ruim_data,
                   dload_req_ptr->block[x].block_len);

            uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

            mcc_uim_cmd( uim_cmd_ptr );

            if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
            {
               ERR( "Bad status from UIM %d", uim_cmd_ptr->hdr.status,0,0);

               /* Store the id of the block that is not processed by the card */
               otasp_ruim_table.msg[otasp_ruim_table.req_index].
                   rej_block_id[rej_block_index++] =
                                         dload_req_ptr->block[x].block_id;
            }
            else
            {
               /* Increment the number of blocks sent to the card */
               ++(otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card);
            }
         }
         else
         {
            ERR( "No free buffers on uim_free_q", 0, 0, 0 );

            /* Store the id of the block that is not sent to the card */
            otasp_ruim_table.msg[otasp_ruim_table.req_index].
               rej_block_id[rej_block_index++] = dload_req_ptr->block[x].block_id;
         }
      } /* end of for loop */
    }/* message buffer availability */

    /* If any block has been sent to the card successfully */
    if (otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card > 0)
    {
        /* consider this message queued and increment the request index */
        otasp_ruim_table.req_index = (otasp_ruim_table.req_index + 1) %
                                    OTASP_RUIM_MAX_MSG_BUFS;
    }
    else
    {
        /* none of the blocks are sent to the card */
        ruim_otasp_int.dload_rsp.num_blocks = dload_req_ptr->num_blocks;

        /* send a reject response for all the blocks in this message */
        for (x = 0; x < dload_req_ptr->num_blocks; x++)
        {
            ruim_otasp_int.dload_rsp.block[x].block_id =
                                       dload_req_ptr->block[x].block_id;

            ruim_otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_UNKNOWN;
        }

        /* send the download response message */
        send_dload_resp_msg();
    }
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    otasp_int.dload_rsp.msg_type = OTASP_DOWNLOAD_RSP_MSG;
    otasp_int.dload_rsp.num_blocks = dload_req_ptr->num_blocks;

    for (x = 0; x < dload_req_ptr->num_blocks; x++)
    {
      otasp_int.dload_rsp.block[x].block_id =
                                   dload_req_ptr->block[x].block_id;

      switch (dload_req_ptr->block[x].block_id)
      {
        case OTASP_CDMA_ANALOG_NAM_BLK: /* Cellular Indicator Download Data */
        {

        /* ---------------------------------------------------------------
        ** The cellular data is processed for the dual band phone due to
        ** the analog information it contains.  The CDMA NAM info is used
        ** as the PCS NAM.
        ** --------------------------------------------------------------- */
#if (!defined( FEATURE_NSOTASP) || defined( FEATURE_ACP ))

          /* If this block is bad it deletes any previously rxed block */

          /* Block Length is set to zero if parsing fails */
          if (dload_req_ptr->block[x].block_len == 0)
          {
            /* Data Size Mismatch */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_DATA_SIZE;
            commit.commit_ftc_cdma = FALSE;
            commit.commit_ftc_amps = FALSE;
          }
          else if (dload_req_ptr->block[x].blk.ftc_cell.n_sid_nid == 0)
          {
            /* Invalid parameter */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_INVALID;
            commit.commit_ftc_cdma = FALSE;
            commit.commit_ftc_amps = FALSE;
          }
          else if (dload_req_ptr->block[x].blk.ftc_cell.n_sid_nid >
                   OTASPI_SID_NID_MAX)
          {
            /* SID/NID length mismatch */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_SID_NID;
            commit.commit_ftc_cdma = FALSE;
            commit.commit_ftc_amps = FALSE;
          }
          else
          {
            cell = &dload_req_ptr->block[x].blk.ftc_cell;
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

            /* Store off CDMA NAM data and flag as OK to commit */
            commit.ftc_cdma.imsi_addr_num = (cell->imsi_class == 0)?
                                           IMSI_CLASS_0_ADDR_NUM :
                                           cell->imsi_addr_num;
            commit.ftc_cdma.mcc = cell->mcc;
            commit.ftc_cdma.imsi_11_12 = cell->imsi_11_12;
            commit.ftc_cdma.imsi_s1 = (qw_lo(cell->imsi_s) & 0x00FFFFFFL);
            commit.ftc_cdma.imsi_s2 = ((qw_lo(cell->imsi_s) >> 24) |
                                  ((qw_hi(cell->imsi_s) & 0x3L) << 8));

            commit.ftc_cdma.accolc = cell->accolc;
            commit.ftc_cdma.mob_term_home = cell->mob_term_home;
            commit.ftc_cdma.mob_term_for_sid = cell->mob_term_for_sid;
            commit.ftc_cdma.mob_term_for_nid = cell->mob_term_for_nid;

            for (y=0; y < OTASPI_SID_NID_MAX ; y++)
            {
              /* Copy SID NID pairs from OTASP download request message */
              if (y < cell->n_sid_nid)
              {
                commit.ftc_cdma.pair[y].sid = cell->var[y].sid;
                commit.ftc_cdma.pair[y].nid = cell->var[y].nid;
              }
              /* Clear old SID NID pairs */
              else if (commit.ftc_cdma.pair[y].sid != 0)
              {
                commit.ftc_cdma.pair[y].sid = 0;
                commit.ftc_cdma.pair[y].nid = 0;
              }
              /* No more old SID NID pairs need to be cleared */
              else
              {
                break;
              }
            } /* end for */

            commit.commit_ftc_cdma = TRUE;
            MSG_MED("OK to commit CDMA NAM data from CID",0,0,0);

            /* Set CDMA_PREF_SERV = current system??? */

            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_ACCEPT;
          }
#else
          /* Block ID Not Supported by targets which don't support 800MHz band */
          otasp_int.dload_rsp.block[x].result_code =
            OTASP_BLK_REJ_BLOCK_ID;
#endif /* !FEATURE_NSOTASP OR FEATURE_ACP */
          break;
        }

        case OTASP_MOB_DIR_NUM_BLK: /* Mobile Directory Number */
        {
          boolean valid_block = TRUE; /* Flag if we are to commit this block */

          /* Block Length is set to zero if parsing fails */
          if (dload_req_ptr->block[x].block_len == 0)
          {
            /* Data Size Mismatch */
            otasp_int.dload_rsp.block[x].result_code =
              OTASP_BLK_REJ_DATA_SIZE;

            /* If a good block was rxed earlier, this bad one deletes it */
            commit.commit_mob_dir = FALSE;
          }
          else
          {

            /* Check all digits to make sure they are valid */
            for (y = 0; y < dload_req_ptr->block[x].blk.mob_dir.n_digits; y++)
            {
              /* Valid digits are 1-9, 0 (10) * (11) and # (12) */
              if ((dload_req_ptr->block[x].blk.mob_dir.digitn[y] == 0) ||
                  (dload_req_ptr->block[x].blk.mob_dir.digitn[y] > 12))
              {
                /* Invalid parameter */
                otasp_int.dload_rsp.block[x].result_code =
                  OTASP_BLK_REJ_INVALID;
                valid_block = FALSE;
                /* If a good block was rxed earlier, this bad one deletes it */
                commit.commit_mob_dir = FALSE;
                break;
              }
            }

            if (valid_block)
            {
              /* store off directory number and flag as OK to commit */
              commit.mob_dir.nam = cdma.curr_nam;
              commit.mob_dir.n_digits =
                dload_req_ptr->block[x].blk.mob_dir.n_digits;
              (void) memcpy( (byte*) commit.mob_dir.digitn,
                            dload_req_ptr->block[x].blk.mob_dir.digitn,
                            dload_req_ptr->block[x].blk.mob_dir.n_digits );
              commit.commit_mob_dir = TRUE;
              MSG_MED("OK to commit mob_dir",0,0,0);

              /* Set CDMA_PREF_SERV = current system??? */

              otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_ACCEPT;

              #ifdef FEATURE_OTASP_STATUS
              /* Send OTASP status to CM */
              otasp_report_status_to_cm(CM_OTASP_STATUS_MDN_DOWNLOADED);
              #endif /* FEATURE_OTASP_STATUS */
            }
          }
          break;
        }

        case OTASP_CDMA_NAM_BLK: /* PCS Indicator Download Data */
        {
#ifdef OTASP_CDMA_NAM_RULE
        
          /* If this block is bad it deletes any previously rxed block */

          /* Block Length is set to zero if parsing fails */
          if (dload_req_ptr->block[x].block_len == 0)
          {
            /* Data Size Mismatch */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_DATA_SIZE;
            commit.commit_ftc_cdma = FALSE;
          }
          else if (dload_req_ptr->block[x].blk.ftc_pcs.n_sid_nid == 0)
          {
            /* Invalid parameter */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_INVALID;
            commit.commit_ftc_cdma = FALSE;
          }
          else if (dload_req_ptr->block[x].blk.ftc_pcs.n_sid_nid >
                   OTASPI_SID_NID_MAX)
          {
            /* SID/NID length mismatch */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_SID_NID;
            commit.commit_ftc_cdma = FALSE;
          }
          else
          {
            pcs = &dload_req_ptr->block[x].blk.ftc_pcs;

            /* Store off CDMA NAM data and flag as OK to commit */
            commit.ftc_cdma.imsi_addr_num = (pcs->imsi_class == 0)?
                                             IMSI_CLASS_0_ADDR_NUM :
                                             pcs->imsi_addr_num;
            commit.ftc_cdma.mcc = pcs->mcc;
            commit.ftc_cdma.imsi_11_12 = pcs->imsi_11_12;
            commit.ftc_cdma.imsi_s1 = (qw_lo(pcs->imsi_s) & 0x00FFFFFFL);
            commit.ftc_cdma.imsi_s2 = ((qw_lo(pcs->imsi_s) >> 24) |
                                  ((qw_hi(pcs->imsi_s) & 0x3L) << 8));

            commit.ftc_cdma.accolc = pcs->accolc;
            commit.ftc_cdma.mob_term_home = pcs->mob_term_home;
            commit.ftc_cdma.mob_term_for_sid = pcs->mob_term_for_sid;
            commit.ftc_cdma.mob_term_for_nid = pcs->mob_term_for_nid;

            for (y=0; y < OTASPI_SID_NID_MAX ; y++)
            {
              /* Copy SID NID pairs from OTASP download request message */
              if (y < pcs->n_sid_nid)
              {
                commit.ftc_cdma.pair[y].sid = pcs->var[y].sid;
                commit.ftc_cdma.pair[y].nid = pcs->var[y].nid;
              }
              /* Clear old SID NID pairs */
              else if (commit.ftc_cdma.pair[y].sid != 0)
              {
                commit.ftc_cdma.pair[y].sid = 0;
                commit.ftc_cdma.pair[y].nid = 0;
              }
              /* No more old SID NID pairs need to be cleared */
              else
              {
                break;
              }
            } /* end for */

            commit.commit_ftc_cdma = TRUE;
            MSG_MED("OK to commit CDMA NAM data from PID",0,0,0);

            /* Set CDMA_PREF_SERV = current system??? */

            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_ACCEPT;
   
            #ifdef FEATURE_OTASP_STATUS
            /* Send OTASP status to CM */
            otasp_report_status_to_cm(CM_OTASP_STATUS_NAM_DOWNLOADED);
            #endif /* FEATURE_OTASP_STATUS */
          }
#else
          /* Block ID Not Supported for phones which don't support 1900MHz band */
          otasp_int.dload_rsp.block[x].result_code =
            OTASP_BLK_REJ_BLOCK_ID;
#endif /* OTASP_CDMA_NAM_RULE */
          break;
        }

        case OTASP_PREF_ROAM_LIST_BLK: /* Roaming List */
        {
#ifndef FEATURE_NSOTASP
        /* this is a proprietary block and if we are doing 683-A, then we
           reject the message  */
           otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_BLOCK_ID;
#else
          #ifdef FEATURE_MEM_REDUCE_PRL
          otasp_is_nv_pr_list_valid = FALSE;

          MSG_HIGH( "DLOAD REQ: otasp_is_nv_pr_list_valid=%d",
                    otasp_is_nv_pr_list_valid, 0, 0);
          #endif /* FEATURE_MEM_REDUCE_PRL */

          /* Block Length is set to zero if parsing fails */
          if (dload_req_ptr->block[x].block_len == 0)
          {
            /* Data Size Mismatch */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_DATA_SIZE;
            /* ---------------------------------------------------------------
            ** For other blocks we delete any received versions when an
            ** invalid block is received.  Since the Roaming List comes in
            ** multiple blocks, however, it isn't appropriate to do that here.
            ** -------------------------------------------------------------- */
          }
          else if (otasp_commit_in_progress && nv_commit.commit_roam_list)
          {
            /* ------------------------------------------------------------
            ** We are in the process of writing a Roaming List to NV, and
            ** rather than creating another huge buffer to handle this
            ** pathological case, we simply reject the block.
            ** ------------------------------------------------------------ */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_MODE;
          }
          else
          {
            roam = &dload_req_ptr->block[x].blk.ftc_roam;
            if ((roam->seq_num == 0) && (otasp_rl_seq != 1))
            {
              /* We are beginning a new roaming list download */
              if (append_rl ( roam->bits_ptr, roam->num_bits, TRUE, FALSE ))
              {
                otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_ACCEPT;
                otasp_rl_seq = 1; /* We expect to see 1 next */
                /* ----------------------------------------------------------
                ** As long as we have received one block of the list, we will
                ** commit it to NV.  The BS is responsible for making sure
                ** we have the whole list before telling us to commit.
                ** ---------------------------------------------------------- */
                commit.commit_roam_list = TRUE;
                commit.roam_list_ptr = &nv_pr_list;
              }
              else
              {
                otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_NO_MEM;
                /* If we can't store the whole list, don't store any of it. */
                commit.commit_roam_list = FALSE;
              }
            }
            else if (roam->seq_num == (otasp_rl_seq - 1))
            {
              /* --------------------------------------------------------
              ** This is a retransmission we already have, so overwrite
              ** the old one in case this contains different information.
              ** -------------------------------------------------------- */

              /* Reset if the seq_num is zero */
              reset = (roam->seq_num == 0);
              if (append_rl ( roam->bits_ptr, roam->num_bits, reset, TRUE ))
              {
                otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_ACCEPT;
                commit.commit_roam_list = TRUE;
              }
              else
              {
                otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_NO_MEM;
                /* If we can't store the whole list, don't store any of it. */
                commit.commit_roam_list = FALSE;
              }
            }
            else if (roam->seq_num == otasp_rl_seq)
            {
              if (append_rl ( roam->bits_ptr, roam->num_bits, FALSE, FALSE ))
              {
                /* Received next expected piece, now wait for the next */
                otasp_rl_seq++;
                otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_ACCEPT;
                commit.commit_roam_list = TRUE;
              }
              else
              {
                otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_NO_MEM;
                /* If we can't store the whole list, don't store any of it. */
                commit.commit_roam_list = FALSE;
              }
            }
            else
            {
              /* This is not a valid sequence number */
              otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_BAD_SEQ;
            }
          }
#endif /*FEATURE_NSOTASP */
          break;
        }

        case OTASP_VERIFY_SPC_BLK: /* Programming Lock Code Verify */
        {
#ifndef FEATURE_NSOTASP
           /* If we receive this block when we are doing 683-A OTASP, we
              reject this block as not supported */
           otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_BLOCK_ID;
#else
          boolean valid_block = TRUE; /* Flag if given SPC has valid digits */

          if (dload_req_ptr->block[x].block_len < ((NV_SEC_CODE_SIZE + 1) / 2))
          {
            /* SPC Parameter Block does not have enough digits! */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_DATA_SIZE;
          }
          else
          {
            /* Check all digits to make sure they are valid */
            for (y = 0; y < NV_SEC_CODE_SIZE; y++)
            {
              /* Valid BCD digits are 0-9, but also count 10 as zero */
              if (dload_req_ptr->block[x].blk.ftc_spc[y] > 10)
              {
                /* Invalid parameter */
                otasp_int.dload_rsp.block[x].result_code =
                  OTASP_BLK_REJ_INVALID;
                valid_block = FALSE;
                break;
              }
              else
              {
                /* Convert BCD to ASCII for comparison with stored SPC */
                if (dload_req_ptr->block[x].blk.ftc_spc[y] == 10)
                {
                  /* If we got a 10, assume it is a zero */
                  dload_req_ptr->block[x].blk.ftc_spc[y] = 0;
                }
                dload_req_ptr->block[x].blk.ftc_spc[y] += '0';
              }
            }

            if (valid_block)
            {
              if (memcmp( dload_req_ptr->block[x].blk.ftc_spc, cdma.spc,
                           NV_SEC_CODE_SIZE ) != 0)
              {
                /* Service Programming Code sent doesn't match */
                otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_BAD_SPC;
                if (++otasp_failed_unlock_attempts >= OTASP_MAX_UNLOCK_ATTEMPTS)
                  {
                    MSG_HIGH("%d SPC validation failures, terminate OTASP",
                                           otasp_failed_unlock_attempts,0,0);
                    #ifdef FEATURE_OTASP_STATUS
                    /* Send OTASP status to CM */
                    otasp_report_status_to_cm(CM_OTASP_STATUS_SPC_RETRIES_EXCEEDED);
                    #endif /* FEATURE_OTASP_STATUS */
    
                    /* Send Notification to MCCCCIM to release all CC instances */
                    mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
                    /* All CC instances should already be in release substate */
                    mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_REL_OTASP_ERR;
                    mccccim_process_event( &mccccim_evt );
                    /* No need to check return status in this case */

                    next_state = CDMA_RELEASE;
                    cdma.rel_orig = FALSE;
                    cdma.otasp_call = NO_SRV_PROV;

                    /* Events report: Release */
                    /* Treat it as BS originated because upper layers are not aware of it */
                    mclog_report_release(MCLOG_RELEASE_BS_ORDER);
                  }
              }
              else
              {
                /* We got a good SPC, so unlock the phone */
                otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_ACCEPT;
                otasp_commit_allowed = TRUE;
              }
            } /* end if (valid_block) */
          } /* end if (enough digits) */
#endif /*FEATURE_NSOTASP */
          break;
        }

        case OTASP_IMSI_T_BLK:
        {
#ifndef FEATURE_NSOTASP
           imsi_t = &dload_req_ptr->block[x].blk.imsi_t;
           commit.imsi_t.addr_num = (imsi_t->imsi_t_class == 0)?
                                           IMSI_CLASS_0_ADDR_NUM :
                                           imsi_t->imsi_t_addr_num;
           commit.imsi_t.mcc = imsi_t->imsi_t_mcc;
           commit.imsi_t.imsi_t_11_12 = imsi_t->imsi_t_11_12;
           commit.imsi_t.s1 =  (qw_lo(imsi_t->imsi_t_s) & 0x00FFFFFFL);
           commit.imsi_t.s2 = ((qw_lo(imsi_t->imsi_t_s) >> 24) |
                                  ((qw_hi(imsi_t->imsi_t_s) & 0x3L) << 8));

           commit.commit_imsi_t = TRUE;
           otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_ACCEPT;

           #ifdef FEATURE_OTASP_STATUS
           /* Send OTASP status to CM */
           otasp_report_status_to_cm(CM_OTASP_STATUS_IMSI_DOWNLOADED);
           #endif /* FEATURE_OTASP_STATUS */
#else
           otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_BLOCK_ID;
#endif /* FEATURE_NSOTASP */
           break;
        }

#ifdef FEATURE_EHRPD
        case OTASP_EHRPD_IMSI_BLK:
        {
          int imsi_index = 0;
          ehrpd_imsi = &dload_req_ptr->block[x].blk.ehrpd_imsi;
          /* Copy the MCC */
          otasp_ehrpd_imsi.num_of_mcc_digits = OTASPI_EHRPD_IMSI_MCC_DIGITS;
          for (y = 0; y < otasp_ehrpd_imsi.num_of_mcc_digits; y++)
          {
            otasp_ehrpd_imsi.ehrpd_mcc[y] = ehrpd_imsi->ehrpd_imsi_digits[imsi_index];
            imsi_index++;
          }

          /* Copy the MNC */
          otasp_ehrpd_imsi.num_of_mnc_digits = ((ehrpd_imsi->ehrpd_mnc_num_digits == 0)? 2 : 3);
          for (y = 0; y < otasp_ehrpd_imsi.num_of_mnc_digits; y++)
          {
            otasp_ehrpd_imsi.ehrpd_mnc[y] = ehrpd_imsi->ehrpd_imsi_digits[imsi_index];
            imsi_index++;
          }

          /* Copy the MSIN */
          otasp_ehrpd_imsi.num_of_msin_digits = 
                 ehrpd_imsi->ehrpd_imsi_num_digits - imsi_index;
          for (y = 0; y < otasp_ehrpd_imsi.num_of_msin_digits; y++)
          {
            otasp_ehrpd_imsi.ehrpd_msin[y] = ehrpd_imsi->ehrpd_imsi_digits[imsi_index];
            imsi_index++;
          }

          commit_ehrpd_imsi = TRUE;
          otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_ACCEPT;
          break;
        }
#endif /* FEATURE_EHRPD */

        default:
          /* Block ID Not Supported */
          otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_BLOCK_ID;
        break;
      } /* end switch */
    } /* end for */

    /* Translate and send the Download Response Message */

    status = otaspx_int_to_ext( &len, &otasp_int, chari );

    if (status == OTASPX_DONE_S) /*lint !e641 */
    {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
    }
    else
    {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
  return (next_state);

} /* end process_dload_req */

/*===========================================================================

FUNCTION PROCESS_MS_KEY_REQ

DESCRIPTION
  This function processes an OTASP MS Key Request Message,
  formats the appropriate MS Key Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void process_ms_key_req
(
  otaspi_ms_key_req_msg_type* ms_key_req_ptr
    /* Pointer to MS Key Request Message */
)
{
#if defined (FEATURE_UIM_RUIM) || defined (FEATURE_DH_EXP)
   byte i;
#endif

#ifdef FEATURE_UIM_RUIM
   uim_cmd_type *uim_cmd_ptr;
   word pos;
   dword temp_rand;
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_DH_EXP
   dh_cmd_type *dh_cmd_ptr;
    /* Pointer to dh command */
   byte * random_number;
   byte j, k, offset;  /* to calculate the offset into the array */
   dword temp;  /* temporary variable to store a dword */
#endif /* FEATURE_DH_EXP */
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
   /* Determine if the R-UIM is available for this NAM */
   if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
   {
      /* The R-UIM card would perform the key exchange operation */
      if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
      {
         uim_cmd_ptr->hdr.command = UIM_MS_KEY_REQ_F;

         /* This random number that is passed to the card is only a seed
            and the quality restrictions on the random number specified by
            IS-683A is now upon the card */

         /* Below loop makes 5 calls to ran_next() to get a 20 byte random seed
            as each call to ran_next returns only a dword */

         pos = 0;
         for (i = 0; i < 5; i++)
         {
            temp_rand = ran_next();
            b_packd(temp_rand,uim_cmd_ptr->ms_key_req.randseed,pos,sizeof(dword)*8);
            pos += (sizeof(dword) * 8);
         }

         uim_cmd_ptr->ms_key_req.a_key_p_rev =  ms_key_req_ptr->a_key_p_rev;
         uim_cmd_ptr->ms_key_req.param_g_len =  ms_key_req_ptr->param_g_len;

         /* The contents of param p and g are in dword format and we would
            like to send data to the card in OTA format resulting in
            unpacking the dword */

         pos = 0;
         for (i = 0; i < (ms_key_req_ptr->param_g_len/sizeof(dword)); i++)
         {
           b_packd(ms_key_req_ptr->param_g[i],
                   uim_cmd_ptr->ms_key_req.param_g, pos, sizeof(dword)*8);
           pos += (sizeof(dword) * 8);
         }

         uim_cmd_ptr->ms_key_req.param_p_len =  ms_key_req_ptr->param_p_len;

         pos = 0;
         for (i = 0; i < (ms_key_req_ptr->param_p_len/sizeof(dword)); i++)
         {
           b_packd(ms_key_req_ptr->param_p[i],
                   uim_cmd_ptr->ms_key_req.param_p, pos, sizeof(dword)*8);
           pos += (sizeof(dword) * 8);
         }

         uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

         mcc_uim_cmd( uim_cmd_ptr );

         if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
         {
             ERR( "Bad status from UIM %d", uim_cmd_ptr->hdr.status,0,0);
             otasp_int.ms_key_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
             send_ms_key_resp_msg();
         }
      }
      else
      {
         ERR( "No free buffers on uim_free_q", 0, 0, 0 );
         otasp_int.ms_key_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
         send_ms_key_resp_msg();
      }
   }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
   else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if defined( FEATURE_DH_EXP )
   {
      /* copy the content of PARAM_G into the otasp structure  */
      memcpy(otasp_dh.param_g, ms_key_req_ptr->param_g, OTASPI_PARAM_G_MAX);

      /* swapping the byte ordering(to Intel) */
      j =  (OTASPI_PARAM_G_MAX/4)  ;
      k = j - 1;
      for (i = 0; i < j/2 ; i++)
      {
         temp = otasp_dh.param_g[k];
         otasp_dh.param_g[k--] = otasp_dh.param_g[i];
         otasp_dh.param_g[i] = temp;
      }

      memcpy(otasp_dh.param_p, ms_key_req_ptr->param_p, OTASPI_PARAM_P);
        /* copy the content of PARAM_P into the otasp structure  */

      /* swapping the byte ordering(to Intel) */
      j =  (OTASPI_PARAM_P/4) ;
      k = j - 1;
      for (i = 0; i < j/2 ; i++)
      {
         temp = otasp_dh.param_p[k];
         otasp_dh.param_p[k--] = otasp_dh.param_p[i];
         otasp_dh.param_p[i] = temp;
      }

      if (OTASP_KEY_EXCHANGE_P_REV < ms_key_req_ptr->a_key_p_rev)
      {
         /* --------------------------------------------------------
         ** If the protocol revision level supported by the mobile
         ** statiom is less than the minimum protocol revision level
         ** supported by the base station, the mobile station shall
         ** indicate the error condition by rejecting the message
         ** with a protocol version mismatch indication.
         ** -------------------------------------------------------- */
         otasp_int.ms_key_rsp.result_code = OTASP_BLK_REJ_PROTOCOL;
         MSG_HIGH( "Min A-Key P_rev > mob_a_key_p_rev", 0, 0, 0 );
         send_ms_key_resp_msg();
      }
      else
      {
        if ((dh_cmd_ptr = (dh_cmd_type*) q_get( &dh_cmd_free_q )) != NULL)
        {
          dh_cmd_ptr->hdr.command = DH_EXP_F;

          /* function call to get a random number from DH */
          random_number = get_rand_from_dh() ;
          offset = 0;

          /* copy into the otasp structure in dword format with lease
          ** significant dword in 0th element*/

          for (i = DH_RAND/4 ; i > 0; i--)
          {
             otasp_dh.private_key[i-1] = b_unpackd(random_number, offset, 32);
             offset+=32;
          }

          /* all the parameters need to be in the reverse byte ordering for
          ** the apwrx routine in DH  */

          /* copy the dword array of prime into the command */
          memcpy(dh_cmd_ptr->exp.dh_prime, otasp_dh.param_p, OTASPI_PARAM_P);

          /* Zero the 64 bytes of base in the command */
          memset(dh_cmd_ptr->exp.dh_base, 0, DH_BASE);

          memcpy(dh_cmd_ptr->exp.dh_base, otasp_dh.param_g, OTASPI_PARAM_G_MAX);

          /* Zero the 64 bytes of exponent in the command */
          memset(dh_cmd_ptr->exp.dh_exponent, 0, DH_EXPONENT);

          /* copy the content of private key in otasp_dh struct into the cmd */
          memcpy(dh_cmd_ptr->exp.dh_exponent, otasp_dh.private_key , DH_RAND);

          /* if we are waiting for a report from DH, which means that the prev
          ** exponentiation is not complete, we have to abort the current one
          ** and then send the new command to DH  */

          if (otasp_dh.state == OTASP_WAIT_FOR_MS_RESULT)
          {
             (void) rex_set_sigs( &dh_tcb, DH_ABORT_EXP_SIG);
          }

          dh_cmd_ptr->exp.rpt_function = mcc_queue_dh_rpt;

          dh_cmd_ptr->exp.prime_bits = (OTASPI_PARAM_P * 8);

          mcc_dh_cmd( dh_cmd_ptr );

          /* change the state of the dh to waiting for MS_RESULT */
          otasp_dh.state = OTASP_WAIT_FOR_MS_RESULT ;
        }
        else
        {
          ERR( "No free buffers on dh_free_q", 0, 0, 0 );
          otasp_int.ms_key_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
          send_ms_key_resp_msg();
        }
      }
   }
#else
   {
     ERR(" Phone is not Diffie-Helman capable",0,0,0);
     otasp_int.ms_key_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
     send_ms_key_resp_msg();
   }
#endif /* FEATURE_DH_EXP */

}  /* end ms_key_req_message */ //lint !e715 ms_key_req_ptr not referenced

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
#if defined(FEATURE_DH) && defined(FEATURE_DH_EXP)
/*===========================================================================
FUNCTION OTASP_PROCESS_DH_REPORT

DESCRIPTION
   This function processes the report received from DH task and send the
   response message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void otasp_process_dh_report
(
   dh_rpt_type*  otasp_dh_rpt
   /* Pointer to Dh report type */
)
{
#ifdef FEATURE_AUTH
   auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
#endif /* FEATURE_AUTH */
   byte i;
    /* index into the array */
   byte j;

   switch(otasp_dh_rpt->rpt_type) {

      case DH_EXP_R:

         MSG_MED( " Received a DH Exponentiation report ",0,0,0);
         /*check for the state and send the appropriate response message */
         if  (otasp_dh.state == OTASP_WAIT_FOR_MS_RESULT)
         {
            MSG_MED( " Received a DH Exp report for MS_RESULT",0,0,0);

            otasp_dh.state = OTASP_WAIT_FOR_KEY_GEN_REQ;
            /* change the state to indicate that you are waiting for
            ** Key Generation Request Message */

            /* The result returned is always the size of the prime and
            ** the MS_RESULT is stored in the otasp data structure in
               the reverse order as this data is sent over the air */

            j = (OTASPI_PARAM_P/sizeof(dword)) - 1;
            for (i = 0; i < OTASPI_PARAM_P/sizeof(dword) ; i++)
            {
                otasp_dh.ms_result[i] = otasp_dh_rpt->rpt.exp_result[j--];
            }

            otasp_int.ms_key_rsp.result_code = OTASP_BLK_ACCEPT ;
            send_ms_key_resp_msg();
         }

         else if (otasp_dh.state == OTASP_WAIT_FOR_A_KEY)
         {
            MSG_MED( " Received a DH Exp report for A_KEY",0,0,0);
            otasp_dh.state = OTASP_WAIT_FOR_DH;
            /* change the state to indicate that you are back to init state */

            qw_set(otasp_dh.a_key, otasp_dh_rpt->rpt.exp_result[1],
                        otasp_dh_rpt->rpt.exp_result[0]);

#ifdef FEATURE_AUTH
            /* send a command to auth to update the a_key_temp */
            /* do not wait for a report */
            if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
            {
              auth_cmd_ptr->hdr.command = AUTH_UPDATE_A_KEY_TEMP_F;

              qw_equ(auth_cmd_ptr->temp.a_key, otasp_dh.a_key);

              /* We do not expect a report back from AUTH Task  */
              auth_cmd_ptr->temp.rpt_function = NULL;

              mcc_auth_cmd( auth_cmd_ptr );

              if ( auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S )
              {
                 /* Auth has received the command and so
                 ** fill in the necessary fields and send the response */
                 otasp_int.key_gen_rsp.result_code = OTASP_BLK_ACCEPT ;
                 otasp_int.key_gen_rsp.ms_result_len =  OTASPI_PARAM_P;
                 memcpy(otasp_int.key_gen_rsp.ms_result, otasp_dh.ms_result,
                                                            OTASPI_PARAM_P);
                 send_key_gen_resp_msg();

                 #ifdef FEATURE_OTASP_STATUS
                 /* Send OTASP status to CM */
                 otasp_report_status_to_cm(CM_OTASP_STATUS_AKEY_EXCHANGED);
                 #endif /* FEATURE_OTASP_STATUS */
              }
              else /* Status returned from command was an error condition */
              {
                 /* The protocol would fail if this operation fails, so
                 ** reject the message */
                 ERR( "Bad status from AUTH %d",auth_cmd_ptr->hdr.status, 0,0);
                 otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
                 send_key_gen_resp_msg();
              }
            }
            else
            {
              ERR_FATAL( "No free buffers on auth_free_q", 0, 0, 0 );
            }
#endif /*FEATURE_AUTH*/
         }
         else
         {
            /* we got the report in the wrong state, so ignore it*/
            ERR( "Report %d received in wrong state from DH!",
                  otasp_dh_rpt->rpt_type, 0, 0 );
         }
         break;

      default :
         /* we got the wrong report */
         ERR( "Wrong report %d from DH!", otasp_dh_rpt->rpt_type, 0, 0 );
         break;
   }

} /* otasp_process_dh_report */
#endif /* FEATURE_DH && FEATURE_DH_EXP */
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

/*===========================================================================

FUNCTION SEND_MS_KEY_RESP_MSG

DESCRIPTION
   This function send the MS Key Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_ms_key_resp_msg(void)

{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the MS Key Response Message */

  otasp_int.ms_key_rsp.msg_type = OTASP_MS_KEY_RSP_MSG;

  status = otaspx_int_to_ext( &len, &otasp_int, chari );
  if (status == OTASPX_DONE_S) /*lint !e641 */
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
} /* send_ms_key_resp_msg */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION SEND_COMMIT_RESP_MSG

DESCRIPTION
   This function send the Commit Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_commit_resp_msg(void)

{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the Commit Response Message */

  otasp_int.commit_rsp.msg_type = OTASP_COMMIT_RSP_MSG;

  status = otaspx_int_to_ext( &len, &otasp_int, chari );
  if (status == OTASPX_DONE_S) /*lint !e641 */
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
} /* send_commit_resp_msg */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION PROCESS_KEY_GEN_REQ

DESCRIPTION
  This function processes an OTASP Key Generation Request Message,
  formats the appropriate Key Generation Response Message, then
  calls the routine to send it. The MS Key Generation Request
  message should have been received first in order to do the
  normal processing for this message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void process_key_gen_req
(
  otaspi_key_gen_req_msg_type* key_gen_req_ptr
    /* Pointer to MS Key Generation Request Message */
)
{
#if defined (FEATURE_UIM_RUIM) || defined (FEATURE_DH_EXP)
  byte i;
#endif

#ifdef FEATURE_UIM_RUIM
  uim_cmd_type *uim_cmd_ptr;
  word pos;
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_DH_EXP
   dh_cmd_type *dh_cmd_ptr;
    /* Pointer to dh command */
   byte j,k; /* index into the array */
   dword temp; /* temporary storage for dword */
#endif /* FEATURE_DH_EXP */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
   /* Determine if the R-UIM is available for this NAM */
   if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
   {
      /* The R-UIM card would perform the key exchange operation */
      if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
      {
         uim_cmd_ptr->hdr.command = UIM_KEY_GEN_REQ_F;

         uim_cmd_ptr->key_gen.bs_result_len = key_gen_req_ptr->bs_result_len;

         /* The contents of bs result is in dword format and we would
            like to send data to the card in OTA format resulting in
            unpacking of the dword */

         pos = 0;
         for (i = 0; i < (key_gen_req_ptr->bs_result_len/sizeof(dword)); i++)
         {
           b_packd(key_gen_req_ptr->bs_result[i],
                    uim_cmd_ptr->key_gen.bs_result, pos, sizeof(dword)*8);
           pos += (sizeof(dword) * 8);
         }

         uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

         uim_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
         uim_cmd_ptr->hdr.cmd_hdr.sigs = NULL;
         uim_cmd_ptr->hdr.options = UIM_OPTION_ALWAYS_RPT;

         mcc_uim_cmd( uim_cmd_ptr );

         if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
         {
             ERR( "Bad status from UIM %d", uim_cmd_ptr->hdr.status,0,0);
             otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
             send_key_gen_resp_msg();
         }
      }
      else
      {
         ERR( "No free buffers on uim_free_q", 0, 0, 0 );
         otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
         send_key_gen_resp_msg();
      }
   }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
   else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if defined (FEATURE_DH_EXP)
   {
      /* this means that the ms key request was not successfully processed
         or that ms key request was not received */

      if (otasp_dh.state != OTASP_WAIT_FOR_KEY_GEN_REQ)
      {
         /* Message not expected in this mode  */
         ERR( "Message not expected in this mode", 0, 0, 0 );
         otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_MODE ;
         send_key_gen_resp_msg();
      }
      else
      {
         if ((dh_cmd_ptr = (dh_cmd_type*) q_get( &dh_cmd_free_q )) != NULL)
         {
            dh_cmd_ptr->hdr.command = DH_EXP_F;

            memcpy(dh_cmd_ptr->exp.dh_prime, otasp_dh.param_p, OTASPI_PARAM_P);

            /* assumed that the result is 512 bits
            ** send the BS_RESULT as the Parameter G */
            memcpy(dh_cmd_ptr->exp.dh_base, key_gen_req_ptr->bs_result, DH_BASE);
            /* swapping the byte ordering(to Intel) */

            j =  (DH_BASE/4)  ;
            k = j - 1;
            for (i = 0; i < j/2 ; i++)
            {
               temp = dh_cmd_ptr->exp.dh_base[k];
               dh_cmd_ptr->exp.dh_base[k--] = dh_cmd_ptr->exp.dh_base[i];
               dh_cmd_ptr->exp.dh_base[i] = temp;
            }

            /* Zero the 64 bytes of exponent in the command */
            memset(dh_cmd_ptr->exp.dh_exponent, 0, DH_EXPONENT);

            memcpy(dh_cmd_ptr->exp.dh_exponent, otasp_dh.private_key, DH_RAND);

            dh_cmd_ptr->exp.rpt_function = mcc_queue_dh_rpt;

            dh_cmd_ptr->exp.prime_bits = (OTASPI_PARAM_P * 8);

            /* if we are waiting for a report from DH, which means that the prev
            ** exponentiation is not complete, we have to abort the current one
            ** and then send the new command to DH  */

            if (otasp_dh.state == OTASP_WAIT_FOR_A_KEY)
            {
               (void) rex_set_sigs( &dh_tcb, DH_ABORT_EXP_SIG);
            }
            mcc_dh_cmd( dh_cmd_ptr );

            /* change the state of the dh to waiting for MS_RESULT */
            otasp_dh.state = OTASP_WAIT_FOR_A_KEY ;

         }
         else
         {
            ERR( "No free buffers on dh_cmd_free_q", 0, 0, 0 );
            otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
            send_key_gen_resp_msg();
         }
      }
   }
#else
   {
      ERR(" Phone is not Diffie-Helman capable",0,0,0);
      otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
      send_key_gen_resp_msg();
   }
#endif /* FEATURE_DH_EXP */

}  /* process_key_gen_req  */  //lint !e715 ms_key_req_ptr not referenced

/*===========================================================================

FUNCTION SEND_KEY_GEN_RESP_MSG

DESCRIPTION
   This function sends the Key Genration Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_key_gen_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the Key Generation Response Message */

  otasp_int.key_gen_rsp.msg_type = OTASP_KEY_GEN_RSP_MSG;

  status = otaspx_int_to_ext( &len, &otasp_int, chari );
  if (status == OTASPX_DONE_S) /*lint !e641 */
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_key_gen_resp_msg */

/*===========================================================================

FUNCTION PROCESS_REAUTH_REQ

DESCRIPTION
  This function processes an OTASP Re-Authentication Request Message,
  formats the appropriate Re-authentication Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void process_reauth_req
(
  otaspi_reauth_req_msg_type* reauth_req_ptr
    /* Pointer to Re-Authentication Request Message */
)
{
#ifndef FEATURE_AUTH
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
#endif
#ifdef FEATURE_AUTH
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
#ifdef FEATURE_UIM_RUIM
  byte i;
    /* Index variable */
#endif /* FEATURE_UIM_RUIM */
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  otasp_reauth_in_progress = TRUE;

  reauth_req_rand = reauth_req_ptr->rand;

  if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
  {
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available for this NAM */
    if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
      auth_cmd_ptr->hdr.command = AUTH_SIGNATURE_F;

      auth_cmd_ptr->sig.rand_type = RUIM_GLOBAL_RAND_CHAL;

      auth_cmd_ptr->sig.rand_chal = reauth_req_ptr->rand;

#ifndef FEATURE_AUTH_DIGITS

      auth_cmd_ptr->sig.dig_len = 0;

      for (i = 0; i < RUIM_RUN_CAVE_DIGITS ; i++)
      {
         auth_cmd_ptr->sig.digits[i] = 0;
      }

#else
      if (cdma.call_orig
#ifdef FEATURE_OTASP_OTAPA
          && (cdma.otasp_call != NETWORK_INIT_SRV_PROV)
#endif /* FEATURE_OTASP_OTAPA */
         )
      {
          /* user initiated call, send the dialed digits */
          /* number of bits in the digits */
          auth_cmd_ptr->sig.dig_len =
              (( cdma.dial.num_digits < (RUIM_RUN_CAVE_DIGITS * 2)) ?
                  cdma.dial.num_digits : (RUIM_RUN_CAVE_DIGITS * 2)) * 4 ;   /*lint !e734*/

          /* The digits in the cdma array is ASCII */

          auth_calculate_digits( cdma.dial.digits,
                         cdma.dial.num_digits, cdma.digit_mode,
                         auth_cmd_ptr->sig.digits);
      }
      else
      {
         /* network initiated call, send dialled digits as zero */
         auth_cmd_ptr->sig.dig_len = 0;
         for (i = 0; i < RUIM_RUN_CAVE_DIGITS ; i++)
         {
            auth_cmd_ptr->sig.digits[i] = 0;
         }
      }
#endif /* FEATURE_AUTH_DIGITS */

      auth_cmd_ptr->sig.process_control = RUIM_DEFLT_PROC_CNTRL |
                                        RUIM_COMMIT_NEW_SSD | RUIM_SAVE_REGS_ON;
      auth_cmd_ptr->sig.esn = cdma.esn;
      auth_cmd_ptr->sig.rpt_function = mcc_queue_auth_rpt;

    }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    {
      auth_cmd_ptr->hdr.command = AUTH_SIGNATURE_F;
      auth_cmd_ptr->sig.rand_chal = reauth_req_ptr->rand;

      /* for a mobile originated call use the saved auth_data in cdma structure
      ** else use the IMSI_S as the input for AUTHENTICATION signature */

      /* If during a mobile originated call, network initiates a OTAPA session
      ** then reauthentication will be treated as the mobile terminated call
      */
      if (cdma.call_orig
#ifdef FEATURE_OTASP_OTAPA
          && (cdma.otasp_call != NETWORK_INIT_SRV_PROV)
#endif /* FEATURE_OTASP_OTAPA */
         )
      {
        auth_cmd_ptr->sig.auth_data = cdma.auth_data;
      }
      else
      {
        auth_cmd_ptr->sig.auth_data = mcc_get_auth_imsi_s1();
      }
      auth_cmd_ptr->sig.save_reg = TRUE;
      auth_cmd_ptr->sig.rpt_function = mcc_queue_auth_rpt;
      MSG_MED("REAUTH: rand %lx auth_data:%lx, save_reg:%d",
              auth_cmd_ptr->sig.rand_chal,
              auth_cmd_ptr->sig.auth_data,
              auth_cmd_ptr->sig.save_reg);

    }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

    mcc_auth_cmd( auth_cmd_ptr );

    if (auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S)
    {
      /* -----------------------------------------------
      ** We have a serious problem and can not calculate
      ** authr, so error fatal.
      ** ----------------------------------------------- */
      ERR_FATAL( "Bad status from AUTH %d", auth_cmd_ptr->hdr.status,0,0);
    }
  }
  else
  {
    ERR_FATAL( "No free buffers on auth_free_q", 0, 0, 0 );
  }
#else
  ERR("The phone does not have AUTH feature enabled",0,0,0);
  rej_parms.rej_msg_type = CAI_TC_FWD_BURST_MSG;
  rej_parms.rej_code = CAI_REJ_CAP;
  send_rej_order( &rej_parms);
#endif /*FEATURE_AUTH*/

}  /* end reauth_req_message */  //lint !e715 reauth_req_ptr not referenced.

#ifdef FEATURE_AUTH
/*===========================================================================
FUNCTION OTASP_PROCESS_AUTH_REPORT

DESCRIPTION
   This function processes the report received from Auth task while
   reauthentication is in progress or while auth_otapa is in progress

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void otasp_process_auth_report
(
   auth_rpt_type*  otasp_auth_rpt
   /* Pointer to auth report type */
)
{
   auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */

#ifdef FEATURE_UIM_RUIM
   boolean send_reauth_rsp = FALSE;
#endif /* FEATURE_UIM_RUIM */

   switch(otasp_auth_rpt->rpt_type)
   {
      case AUTH_SIGNATURE_R:

         /* recalculate the message encryption parameters and private long
         ** code mask during reauthentication operation, only if they were
         ** not active before the start of the reauthentication operation */

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
         {
           cdma.authr = 0;
           send_reauth_rsp = TRUE;

           if (otasp_auth_rpt->rpt_status == RUIM_PASS)
           {
               cdma.authr = otasp_auth_rpt->rpt.auth;

               if (!((cdma.encrypt_mode) || cdma.long_code.private_lcm_active))
               {
                 if ((auth_cmd_ptr = (auth_cmd_type*) q_get(&auth_free_q))!= NULL)
                 {
                   MSG_MED( "Start key generation", 0, 0, 0);
                   auth_cmd_ptr->hdr.command = AUTH_GENERATE_KEY_F;
                   auth_cmd_ptr->key.vpm_first_octet = RUIM_FIRST_OCTET_OF_VPM;
                   auth_cmd_ptr->key.vpm_last_octet =  RUIM_LAST_OCTET_OF_VPM;
                   auth_cmd_ptr->key.rpt_function = mcc_queue_auth_rpt;
                   mcc_auth_cmd( auth_cmd_ptr );

                   if (auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S)
                   {
                      send_reauth_rsp = FALSE;
                   }
                   else
                   {
                      ERR("Bad status from AUTH %d",auth_cmd_ptr->hdr.status,0,0);
                   }
                 }
                 else
                 {
                   ERR( "No free buffers on auth_free_q", 0, 0, 0 );
                 }
               }
           }

           if (send_reauth_rsp)
           {
               /* We may be sending the response now bcos RUIM returned a bad
                  status */
               /* We do not have to generate the keys and we are done with the
                  process of reauthentication */

               otasp_reauth_in_progress = FALSE;

               /* Build the Re-Authentication Response Message */
               build_reauth_resp_msg();

               /* send the reauthentication response message */
               send_reauth_resp_msg();
           }
         }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
         {
           cdma.authr =  otasp_auth_rpt->rpt.auth;

           if (!((cdma.encrypt_mode) || cdma.long_code.private_lcm_active))
           {
              if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
              {
                 auth_cmd_ptr->hdr.command = AUTH_GENERATE_KEY_F;
                 /* We would always like to have voice privacy */
                 auth_cmd_ptr->key.do_vpm = TRUE;

                 auth_cmd_ptr->key.rpt_function = mcc_queue_auth_rpt;

                 mcc_auth_cmd( auth_cmd_ptr );

                 if (auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S)
                 {
                    ERR( "Bad status from AUTH %d", auth_cmd_ptr->hdr.status,0,0);
                 }
              }
              else
              {
                 ERR( "No free buffers on auth_free_q", 0, 0, 0 );
              }
           }
           else
           {
              /* we do not have to generate the keys and we are done with the
              ** process of reauthentication */
              otasp_reauth_in_progress = FALSE;

              /* Build the Re-Authentication Response Message */
              build_reauth_resp_msg();

              /* send the reauthentication response message */
              send_reauth_resp_msg();

           }
         }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

         break;

      case AUTH_GENERATE_KEY_R:

         if (otasp_reauth_in_progress)
         {
            /* We do not check to see the status because, Whenever a transition is
               requested to PLC, we would reject it, since the plcm_available would
               have been set to FALSE in auth when R-UIM failed. */

            /* Build the Re-Authentication Response Message */
            build_reauth_resp_msg();

            /* new keys have been generated and reauthentication is  */
            /* complete, send the reauth response message          */
            send_reauth_resp_msg();

            otasp_reauth_in_progress = FALSE;
         }

         break;

#ifdef FEATURE_OTASP_OTAPA
      case AUTH_SIGNATURE_SPASM_R:
         if (otasp_auth_otapa_in_progress)
         {
           otasp_auth_otapa_in_progress = FALSE;
           otasp_auth_otapa = otasp_auth_rpt->rpt.auth;
           otasp_auth_otapa_ready = TRUE;
           MSG_MED("Get auth_otapa: %lx", otasp_auth_otapa, 0,0);
         }

         break;
#endif /* FEATURE_OTASP_OTAPA */

      default:
         /* we got the wrong report */
         ERR( "Wrong report %d from AUTH!", otasp_auth_rpt->rpt_type, 0, 0 );
         break;
   } //lint !e788

} /* otasp_process_auth_report */

/*===========================================================================

FUNCTION BUILD_REAUTH_RESP_MSG

DESCRIPTION
   This function builds the Re-Authentication Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void build_reauth_resp_msg (void )
{

  otasp_int.reauth_rsp.authr = cdma.authr;
  otasp_int.reauth_rsp.randc =
               (byte) ((reauth_req_rand & 0xFF000000L) >> 24); /*lint !e737 */

  otasp_int.reauth_rsp.count =  auth_get_count();

  /* for a mobile originated call use the saved auth_data in cdma
  ** structure else use the IMSI_S as the input for AUTHENTICATION
  ** signature */

  /* If during a mobile originated call, network initiates a OTAPA session
  ** then reauthentication will be treated as the mobile terminated call
  */
  if ((cdma.call_orig)
#ifdef FEATURE_OTASP_OTAPA
       && (cdma.otasp_call != NETWORK_INIT_SRV_PROV)
#endif /* FEATURE_OTASP_OTAPA */
     )
  {
     otasp_int.reauth_rsp.auth_data = cdma.auth_data;
  }
  else
  {
     otasp_int.reauth_rsp.auth_data = mcc_get_auth_imsi_s1();
  }
  MSG_MED("REAUTH_R: rand:%lx,auth_sig:%lx,auth_data:%lx",
          otasp_int.reauth_rsp.randc,
          otasp_int.reauth_rsp.authr,
          otasp_int.reauth_rsp.auth_data);
} /* build_reauth_resp_msg */

/*===========================================================================

FUNCTION SEND_REAUTH_RESP_MSG

DESCRIPTION
   This function sends the Re-Authentication Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_reauth_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 *//* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the Re-Authentication Response Message */

  otasp_int.reauth_rsp.msg_type = OTASP_REAUTH_RSP_MSG;

  status = otaspx_int_to_ext( &len, &otasp_int, chari );
  if (status == OTASPX_DONE_S) /*lint !e641 */
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_reauth_resp_msg */
#endif /*FEATURE_AUTH*/

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
/*===========================================================================

FUNCTION OTASP_PROCESS_COMMIT

DESCRIPTION
  This function processes an OTASP Commit, after NV has finished processing
  the NV_OTASP_COMMIT_F command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void otasp_process_commit (void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  nv_stat_enum_type     nam_commit_status;     /* Status of request */
#if (defined (FEATURE_AUTH) && defined(FEATURE_DH))
   auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
#endif
  boolean send_response = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If NV signals commit complete when no commit is in progress, ignore it */
  if (!otasp_commit_in_progress)
  {
    return;
  }

  /* we might need to commit the A-Key also. Save the status of NV buffer
     as a result of commiting the NAM parameters if NAM parameters have
     been committed  */
  nam_commit_status = otasp_nv_buf.status;

  if (otasp_commit_akey_pending)
  {
#if (defined (FEATURE_DH)  && defined (FEATURE_AUTH ))

     if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
     {

        /* Fill in necessary fields of command block */

        auth_cmd_ptr->hdr.command = AUTH_COMMIT_KEYS_F;
        auth_cmd_ptr->hdr.cmd_hdr.task_ptr = &mc_tcb;
        auth_cmd_ptr->hdr.cmd_hdr.sigs = MCC_OTASP_COMMIT_SIG;

        auth_cmd_ptr->commit_keys.nam_index = cdma.curr_nam;
        auth_cmd_ptr->commit_keys.rpt_function = mcc_queue_auth_rpt;
        otasp_commit_in_progress = TRUE;
        otasp_committed_both_flag = TRUE;

        auth_cmd( auth_cmd_ptr );
        if (auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S)
        {
            ERR_FATAL( "Bad status from AUTH %d", auth_cmd_ptr->hdr.status,0,0);
        }
     }
     else
     {
         ERR_FATAL( "No free buffers on auth_free_q", 0, 0, 0 );
     }
#endif /* FEATURE_DH */
     otasp_commit_akey_pending = FALSE;
       /* as we have sent the command to commit */
  }
  else
  {
     otasp_int.commit_rsp.msg_type = OTASP_COMMIT_RSP_MSG;
     otasp_commit_in_progress = FALSE;
     send_response = TRUE;

     if (otasp_committed_both_flag)
     {
        if ((nam_commit_status == NV_DONE_S)
#ifdef FEATURE_AUTH
           && (otasp_akey_commit_result)
#endif
           )
        {
          otasp_reload_nam_params();
          otasp_int.commit_rsp.result_code = OTASP_BLK_ACCEPT;

          MSG_MED("A-KEY committed",0,0,0);
#ifdef FEATURE_AUTH
          otasp_akey_commit_result = FALSE;
          a_key_temp_valid = FALSE;
          /* reset the flags */
          new_ssd_valid = FALSE;
#endif /* FEATURE_AUTH */
        }
        else
        {
          otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
        }
        otasp_committed_both_flag = FALSE; /* reset the flag */
     }
     else
     {
        if (nv_commit.commit_mob_dir || nv_commit.commit_ftc_cdma ||
               nv_commit.commit_ftc_amps || nv_commit.commit_roam_list
#ifndef FEATURE_NSOTASP
            || nv_commit.commit_spc || nv_commit.commit_imsi_t
#ifdef FEATURE_OTASP_OTAPA
            || nv_commit.commit_nam_lock
#endif /* FEATURE_OTASP_OTAPA */
#endif
#ifdef FEATURE_EHRPD
            || commit_ehrpd_imsi
#endif /* FEATURE_EHRPD */
          )
        {
           if (nam_commit_status == NV_DONE_S)
           {
               otasp_reload_nam_params();
               #ifdef FEATURE_EHRPD
               commit_ehrpd_imsi = FALSE;
               #endif /* FEATURE_EHRPD */
               otasp_int.commit_rsp.result_code = OTASP_BLK_ACCEPT;
           }
           else
           {

              otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
           }
        }
#ifdef FEATURE_AUTH
        else if (otasp_akey_commit_result)
        {
           MSG_MED("A-KEY committed",0,0,0);
           otasp_successful_commit  = TRUE;
           otasp_akey_commit_result = FALSE;
           a_key_temp_valid = FALSE;
           otasp_int.commit_rsp.result_code = OTASP_BLK_ACCEPT;
           /* reset the flags */
           new_ssd_valid = FALSE;
        }
#endif /* FEATURE_AUTH */
        else
        {
           otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
        }
     }
     MSG_MED("OTASP commit complete",0,0,0);
  }
    
  #ifdef FEATURE_OTASP_STATUS
  /* Send OTASP status to CM */
  if(otasp_int.commit_rsp.result_code == OTASP_BLK_ACCEPT)
  {
    otasp_report_status_to_cm(CM_OTASP_STATUS_COMMITTED);
  }
  #endif /* FEATURE_OTASP_STATUS */

  if (send_response)
  {
      /* Translate and send the Commit Response Message */
      status = otaspx_int_to_ext( &len, &otasp_int, chari );

      if (status == OTASPX_DONE_S) /*lint !e641 */
      {
        send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );

        /* Notify CM that otasp commit has finished */
        otasp_report_commit_status_to_cm(FALSE);
      }
      else
      {
        ERR_FATAL( "Failed OTASP message translation",0,0,0);
      }
  }
} /* otasp_process_commit() */
#endif /* !FEATURE_UIM_RUIM FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
/*===========================================================================

FUNCTION CAP_RSP_RESET_FEATURE

DESCRIPTION
  This function reset the num feature field in the capability response 
  message.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void  cap_rsp_reset_feature( 

  otaspi_prot_cap_rsp_msg_type *cap_rsp_ptr
      /* Pointer to the cap rsp message
      */
)
{
  if( cap_rsp_ptr == NULL )
  {
    MSG_ERROR("CAP RSP MSG: NULL ptr", 0, 0, 0 );

    return;
  }

  cap_rsp_ptr->num_features = 0;

  return;
}/* cap_rsp_reset_feature */

/*===========================================================================

FUNCTION CAP_RSP_ADD_FEATURE

DESCRIPTION
  This function adds the feature_id in the capability response message.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void  cap_rsp_add_feature( 

  otaspi_prot_cap_rsp_msg_type *cap_rsp_ptr,
      /* Pointer to the cap rsp message for update
      */

  byte feature_id, 
      /* Feature ID to be added
      */

  byte feature_p_rev
      /* Feature P Revision to be added.
      */
)
{

  /* Return FALSE if the pointer to the capability response message is NULL
  */
  if( cap_rsp_ptr == NULL )
  {
    MSG_ERROR(" CAP RSP MSG: NULL ptr", 0, 0, 0 );

    return;
  }

  /* Return FALSE if the num features in the capability response message 
  ** reaches the max number of features supported
  */
  if( cap_rsp_ptr->num_features >= OTASPI_FEATURE_MAX )
  {

    MSG_ERROR(" CAP RSP MSG: num_features=%d, max=%d", 
              cap_rsp_ptr->num_features,
              OTASPI_FEATURE_MAX,
              0 );

    cap_rsp_ptr->num_features = OTASPI_FEATURE_MAX;

    return;

  }


  /* Store the feature_id and feature_p_rev in capability response message
  */
  cap_rsp_ptr->var[cap_rsp_ptr->num_features].feature_id = feature_id;
  cap_rsp_ptr->var[cap_rsp_ptr->num_features].feature_p_rev = feature_p_rev;

  /* Increase the num of features in the capability response message
  */
  cap_rsp_ptr->num_features++;

  MSG_HIGH(" CAP RSP MSG: feature_id =%d, p_rev=%d, num_features=%d",
           feature_id,
           feature_p_rev,
           cap_rsp_ptr->num_features);

  return;

} /* cap_rsp_add_feature */
#endif /* !FEATURE_UIM_RUIM  && FEATURE_UIM_RUN_TIME_ENABLE */

/*===========================================================================

FUNCTION PROCESS_PROT_CAP_REQ

DESCRIPTION
  This function processes an OTASP Protocol Capability Request Message,
  formats the appropriate Protocol Capability Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

/*lint -e831*/
void process_prot_cap_req 
(
  otaspi_prot_cap_req_msg_type* prot_cap_req_ptr   
    /* Pointer to Protocol Capability Request Message */
) 
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
#ifndef FEATURE_NSOTASP
  byte offset;  /* offset for additional bytes */
#endif
  byte num;  /* number of features  */

#ifdef FEATURE_MEID_SUPPORT
  int  j;   /* loop variable */
  byte recnum;     /* record number */
  boolean   duprec;   /* duplicated rec flag */
#endif /* FEATURE_MEID_SUPPORT */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the msg type to OTASP_PROT_CAP_RSP_MSG */
  otasp_int.cap_rsp.msg_type = OTASP_PROT_CAP_RSP_MSG;

#ifdef FEATURE_MEID_SUPPORT
  if (prot_cap_req_ptr->otasp_ext_cap_rsp_required) 
  {
    /* This is a new format prot_cap_req message
     * We need to format a Extended Protocol Capability Response Message
     */
    otasp_int.cap_rsp.otasp_mob_p_rev = OTASP_P_REV_3; /*lint !e641 */  
          /* Support IS-683C */

    /* Set the msg type to OTASP_EXT_PROT_CAP_RSP_MSG */
    otasp_int.cap_rsp.msg_type = OTASP_EXT_PROT_CAP_RSP_MSG;
  }
#endif /* FEATURE_MEID_SUPPORT */

  otasp_int.cap_rsp.mob_firm_rev = cdma.mob_firm_rev;
  otasp_int.cap_rsp.mob_model = cdma.mob_model;

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* Number of features supported by the card */
    otasp_int.cap_rsp.num_features = cdma.ruim_ota_features.num_features;

    if( otasp_int.cap_rsp.num_features > OTASPI_FEATURE_MAX ) 
    {

      MSG_ERROR( " CAP RSP MSG: num_features=%d, max=%d", 
               otasp_int.cap_rsp.num_features,
               OTASPI_FEATURE_MAX,
               0 );

      otasp_int.cap_rsp.num_features = OTASPI_FEATURE_MAX;
    }

    /* Fill the Feature-ID and Feature P_REV data */
    for (num = 0; num < otasp_int.cap_rsp.num_features ; num++)
    {
        otasp_int.cap_rsp.var[num].feature_id =
                          cdma.ruim_ota_features.feature_id[num];
        otasp_int.cap_rsp.var[num].feature_p_rev =
                          cdma.ruim_ota_features.feature_p_rev[num];
    }
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    cap_rsp_reset_feature( &otasp_int.cap_rsp );

    cap_rsp_add_feature( &otasp_int.cap_rsp,
                         OTASP_NAM_DOWNLOAD,
                         OTASP_NAM_DOWNLOAD_P_REV );

    #ifdef FEATURE_EHRPD
    cap_rsp_add_feature( &otasp_int.cap_rsp,
                         OTASP_NAM_DOWNLOAD,
                         OTASP_NAM_DOWNLOAD_EHRPD_IMSI_P_REV );
    #endif /* FEATURE_EHRPD */

    cap_rsp_add_feature( &otasp_int.cap_rsp,
                        OTASP_PROG_LOCK,
                        OTASP_SPL_P_REV );

    #ifdef FEATURE_IS683C_OTASP
    cap_rsp_add_feature( &otasp_int.cap_rsp,
                         OTASP_PREF_ROAM,
                         (byte) PRL_SSPR_P_REV_1 );
    #endif /* FEATURE_IS683C_OTASP */

    cap_rsp_add_feature( &otasp_int.cap_rsp,
                         OTASP_PREF_ROAM,
                         OTASP_PREF_ROAM_P_REV );

    #ifdef FEATURE_DH_EXP
    cap_rsp_add_feature( &otasp_int.cap_rsp,
                         OTASP_KEY_EXCHANGE,
                         OTASP_KEY_EXCHANGE_P_REV );
    #endif /* FEATURE_DH_EXP */
    
    #ifdef FEATURE_OTASP_OTAPA
    cap_rsp_add_feature( &otasp_int.cap_rsp,
                         OTASP_OTAPA,
                         OTASP_OTAPA_P_REV );
    #endif /* FEATURE_OTASP_OTAPA */
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

#ifdef FEATURE_MEID_SUPPORT
  if (otasp_int.cap_rsp.msg_type == OTASP_EXT_PROT_CAP_RSP_MSG) 
  {
    /* initialize recnum */
    recnum = 0;

    for (num = 0; num < prot_cap_req_ptr->num_cap_records; num++) 
    {
      /* initialize the duplicated rec flag */
      duprec = FALSE;

      /* check for duplicates */
      for (j = 0; j < num; j++)
      {
        if (prot_cap_req_ptr->cap_record_type[j] ==
              prot_cap_req_ptr->cap_record_type[num]) 
        {
          /* already processed this type, ignore */
          duprec = TRUE;
          MSG_ERROR("duprec num %d recnum %d", num, recnum, 0);
          break;
        }
      }

      if (!duprec) 
      {
        MSG_MED("proc rec num %d recnum %d", num, recnum, 0);

        switch (prot_cap_req_ptr->cap_record_type[num])
        {
        case OTASPI_CAP_INFO_OPER_MODE: 
          /* copy the cap rec type from the request */
          otasp_int.cap_rsp.cap_rec[recnum].cap_record_type =
              prot_cap_req_ptr->cap_record_type[num]; /*lint !e641 */

          /* Fill in OPER MODE */
          /* The length should be 1 byte */
          otasp_int.cap_rsp.cap_rec[recnum].cap_record_length = 1;

          otasp_int.cap_rsp.cap_rec[recnum].info.op_mode = 0;
          /* additional field */
          #ifdef FEATURE_ACP
#error code not present
          #endif

          /* CDMA is supported, otherwise we won't be here */
          otasp_int.cap_rsp.cap_rec[recnum].info.op_mode |= OTASP_OP_MODE1;

          #ifdef FEATURE_HDR
#error code not present
          #endif

          /* increment the recnum */
          recnum++;

          break;

        case OTASPI_CAP_INFO_CDMA_BAND_CLASS:
          {
            /* allocate a temp buffer to get the band class info */
            caii_band_rec_type  band_class_rec;

            /* copy the cap rec type from the request */
            otasp_int.cap_rsp.cap_rec[recnum].cap_record_type =
                prot_cap_req_ptr->cap_record_type[num]; /*lint !e641 */

            /* The length should be 2 byte */
            otasp_int.cap_rsp.cap_rec[recnum].cap_record_length = 2;

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class = 0;

            /* get the band class info */
            mcc_info_band_class(&band_class_rec);

            /* set the appropriate bit in the cap_rec for each band class */
            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[0] ? OTASP_BAND_CLASS_0 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[1] ? OTASP_BAND_CLASS_1 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[2] ? OTASP_BAND_CLASS_2 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[3] ? OTASP_BAND_CLASS_3 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[4] ? OTASP_BAND_CLASS_4 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[5] ? OTASP_BAND_CLASS_5 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[6] ? OTASP_BAND_CLASS_6 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[7] ? OTASP_BAND_CLASS_7 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[8] ? OTASP_BAND_CLASS_8 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[9] ? OTASP_BAND_CLASS_9 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[10] ? OTASP_BAND_CLASS_10 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[11] ? OTASP_BAND_CLASS_11 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[12] ? OTASP_BAND_CLASS_12 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[13] ? OTASP_BAND_CLASS_13 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[14] ? OTASP_BAND_CLASS_14 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
               ( band_class_rec.band[15] ? OTASP_BAND_CLASS_15 : 0);
            /* increment the recnum */
            recnum++;
          }

          break;

        case OTASPI_CAP_INFO_MEID: 
          /* We will process MEID Info record
           */
          if (mccap_is_meid_supported()) 
          {
            /* copy the cap rec type from the request */
            otasp_int.cap_rsp.cap_rec[recnum].cap_record_type =
                prot_cap_req_ptr->cap_record_type[num]; /*lint !e641 */

            /* we have the MEID
             * The length should be 7 bytes
             */
            otasp_int.cap_rsp.cap_rec[recnum].cap_record_length = 7;
            qw_equ(otasp_int.cap_rsp.cap_rec[num].info.meid, cdma.meid);

            /* increment the recnum */
            recnum++;
          }
          else
          {
            /* print an ERROR message and return */
            MSG_ERROR("No VALID MEID for EXT CAP RSP MSG",0,0,0);
          }
          break;

        default:
          /* Unsupported */
          break;

        } /* switch (prot_cap_req_ptr->cap_record_type[num]) */
      } /* if (!duprec) */
    } /* for (num = 0; num < prot_cap_req_ptr->num_cap_records */

    /*  fill in the number of records */
    otasp_int.cap_rsp.num_cap_records = recnum;

  } /* if (otasp_int.cap_rsp.msg_type == OTASP_EXT_PROT_CAP_RSP_MSG) */
  else
#endif /* FEATURE_MEID_SUPPORT */

  {
    #ifndef FEATURE_NSOTASP
    offset = 0;
    otasp_int.cap_rsp.addnl_fields[offset] = 0;
    /* additional field */
    #ifdef FEATURE_ACP
#error code not present
    #endif
    if (mcc_cellular_band_is_supported()) 
    {
      otasp_int.cap_rsp.addnl_fields[offset] |= 0x40;
    }
    if (mcc_pcs_band_is_supported()) 
    {
      otasp_int.cap_rsp.addnl_fields[offset] |= 0x20;
    }

    ++offset; /* offset plus one is the number of octets in addnl field  */
    otasp_int.cap_rsp.add_length = offset;
    /* number of octets in the additional fields */
    #endif /* FEATURE_NSOTASP */
  }

  /* Translate and send the Protocol Capability Response Message */

  status = otaspx_int_to_ext( &len, &otasp_int, chari );

  if (status == OTASPX_DONE_S) /*lint !e641 */
  {
    send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
    ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }

} /* process_prot_cap_req */  //lint !e715 Unreferenced pointer prot_cap_req_ptr
/*lint +e831*/

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION SEND_SSPR_CFG_RESP_MSG

DESCRIPTION
   This function sends the SSPR Configuration Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_sspr_cfg_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the SSPR Configuration Response Message */

  ruim_otasp_int.sspr_cfg_rsp.msg_type = OTASP_SSPR_CONFIG_RSP_MSG;
  status = otaspx_int_to_ext( &len, &ruim_otasp_int, chari );

  if (status == OTASPX_DONE_S) /*lint !e641*/
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_sspr_cfg_resp_msg */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION SSPR_CFG_RSP_POPULATE_PRL_BLK

DESCRIPTION
  This function populate the PRL block upon OTASP SSPR Config Request Message,
  with block ID=1.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void sspr_cfg_rsp_populate_prl_blk
( 
  otaspi_sspr_cfg_rsp_msg_type *sspr_cfg_rsp_ptr,
      /* Pointer to the sspr cfg rsp message */

  otaspi_sspr_cfg_req_msg_type *sspr_cfg_req_ptr
      /* Pointer to the sspr cfg req message */
)
{

  #ifdef FEATURE_NSOTASP
  sspr_cfg_rsp_ptr->block_len = 0;
  sspr_cfg_rsp_ptr->result_code = OTASP_BLK_REJ_BLOCK_ID;
  #else
  nv_roaming_list_type *roaming_list_ptr;

  #ifdef FEATURE_MEM_REDUCE_PRL
  const prl_hdr_s_type  *prl_hdr_ptr = prl_hdr_nv_get_ptr();

  roaming_list_ptr = (nv_roaming_list_type *)prl_get_ptr(); 

  MSG_HIGH(" SSPR CONF REQ: otasp_id=%d, sd_id=%d, is_nv_valid=%d",
            otasp_roam_size.pr_list_id,
            prl_hdr_ptr->pr_list_id,
            otasp_is_nv_pr_list_valid );


  /* The the PRL id does not match and nv is invalid, reject the request. */
  if( (otasp_roam_size.pr_list_id != prl_hdr_ptr->pr_list_id ) &&
      ! otasp_is_nv_pr_list_valid )
  {
    sspr_cfg_rsp_ptr->block_len = 0;

    sspr_cfg_rsp_ptr->result_code = OTASP_BLK_REJ_BLOCK_ID;

    return;
  }


  /* The the PRL id does not match but nv is valid, use nv PRL.
  */
  if( ( otasp_roam_size.pr_list_id != prl_hdr_ptr->pr_list_id ) &&
      otasp_is_nv_pr_list_valid )
  {
    MSG_HIGH(" SSPR CONF REQ: use nv PRL", 0, 0, 0 );
	
    roaming_list_ptr = &nv_pr_list;
  }
  #else
  roaming_list_ptr = &mcc_roaming_list;
  #endif /* FEATURE_MEM_REDUCE_PRL */


  MSG_MED("Reporting IS-683 PRL ",0,0,0);

  /* this is not the actual block len, the correct one will
     be computed before translation */
  sspr_cfg_rsp_ptr->block_len = 1;

  if (sspr_cfg_req_ptr->req_offset >= (roaming_list_ptr->size / 8))
  {
    sspr_cfg_rsp_ptr->result_code = OTASP_BLK_REJ_DATA_SIZE;
    sspr_cfg_rsp_ptr->block.blk.sspr_prl.last_segment = 0;
    sspr_cfg_rsp_ptr->block.blk.sspr_prl.segment_offset =
                         sspr_cfg_req_ptr->req_offset;
    sspr_cfg_rsp_ptr->block.blk.sspr_prl.segment_size  = 0;
    sspr_cfg_rsp_ptr->block.blk.sspr_prl.bits_ptr  = NULL;
  }
  else
  {
    sspr_cfg_rsp_ptr->result_code = OTASP_BLK_ACCEPT;

    sspr_cfg_rsp_ptr->block.blk.sspr_prl.bits_ptr =
         (byte*)roaming_list_ptr->roaming_list+sspr_cfg_req_ptr->req_offset;

    if ((sspr_cfg_req_ptr->req_offset + sspr_cfg_req_ptr->req_max_size)
                                               >= (roaming_list_ptr->size/8))
    {
       sspr_cfg_rsp_ptr->block.blk.sspr_prl.segment_size =
           (byte)((roaming_list_ptr->size/8)-sspr_cfg_req_ptr->req_offset);
    }
    else
    {
       sspr_cfg_rsp_ptr->block.blk.sspr_prl.segment_size =
            sspr_cfg_req_ptr->req_max_size;
    }

    sspr_cfg_rsp_ptr->block.blk.sspr_prl.segment_offset =
         sspr_cfg_req_ptr->req_offset;

    if ((sspr_cfg_req_ptr->req_offset +
                      sspr_cfg_rsp_ptr->block.blk.sspr_prl.segment_size)
                      == (roaming_list_ptr->size /8))
    {
      sspr_cfg_rsp_ptr->block.blk.sspr_prl.last_segment = 1;
    }
    else
    {
      sspr_cfg_rsp_ptr->block.blk.sspr_prl.last_segment = 0;
    }
  }
  #endif /* FEATURE_NSOTASP */

} /* sspr_cfg_rsp_populate_prl_blk */

/*===========================================================================

FUNCTION PROCESS_SSPR_CONFIG_REQ

DESCRIPTION
  This function processes an OTASP SSPR Config Request Message,
  formats the appropriate Protocol Configuration Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_sspr_config_req
(
  otaspi_sspr_cfg_req_msg_type* sspr_cfg_req_ptr
    /* Pointer to SSPR Configuration Request Message */
)
{
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
#if defined( FEATURE_UIM_RUIM )
  uim_cmd_type *uim_cmd_ptr;   /* UIM command pointer */
#endif /* FEATURE_UIM_RUIM */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* Initialize the number of blocks req to and recd from the card */
    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_card = 0;

    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_rsp_card = 0;

    /* Check to see if a message buffer is available */
    if ((otasp_ruim_table.req_index + 1) % OTASP_RUIM_MAX_MSG_BUFS !=
                                   otasp_ruim_table.rsp_index)
    {
       /* Store the message type */
       otasp_ruim_table.msg[otasp_ruim_table.req_index].msg_type =
                OTASP_SSPR_CONFIG_REQ_MSG;

       /* Store the number of blocks requested in the message as only one */
       otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_msg = 1;

       if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
       {
          uim_cmd_ptr->hdr.command = UIM_SSPR_CONFIG_REQ_F;

          uim_cmd_ptr->sspr_config.block_id = sspr_cfg_req_ptr->block_id;

          /* If the Block is a PRL block, then set the offset and size */

          if (sspr_cfg_req_ptr->block_id == OTASP_CFG_PREF_ROAM_LIST_BLK)
          {
             uim_cmd_ptr->sspr_config.req_offset = sspr_cfg_req_ptr->req_offset;

             uim_cmd_ptr->sspr_config.req_max_size= sspr_cfg_req_ptr->req_max_size;
          }
          else
          {
             uim_cmd_ptr->sspr_config.req_offset = 0;

             uim_cmd_ptr->sspr_config.req_max_size= 0;
          }

          uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

          mcc_uim_cmd( uim_cmd_ptr );

          if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
          {
             ERR( "Bad status from UIM %d", uim_cmd_ptr->hdr.status,0,0);
          }
          else
          {
             /* Increment the number of blocks sent to the card */
             ++(otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                    num_blocks_req_card);
          }
       }
       else
       {
          ERR( "No free buffers on uim_free_q", 0, 0, 0 );

          /* We do not store the id of the rejected block since there is only
             one block in this message */
       }
    } /* check for message buffer availability */

    /* If any block has been sent to the card successfully */
    if (otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card > 0)
    {
        /* consider this message queued and increment the request index */
        otasp_ruim_table.req_index = (otasp_ruim_table.req_index + 1) %
                                    OTASP_RUIM_MAX_MSG_BUFS;
    }
    else
    {
        ruim_otasp_int.sspr_cfg_rsp.block_id = sspr_cfg_req_ptr->block_id;

        ruim_otasp_int.sspr_cfg_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;

        ruim_otasp_int.sspr_cfg_rsp.block_len = 0;

        /* send the SSPR configuration response message */
        send_sspr_cfg_resp_msg();
    }
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    otasp_int.sspr_cfg_rsp.msg_type = OTASP_SSPR_CONFIG_RSP_MSG;

    otasp_int.sspr_cfg_rsp.block_id = sspr_cfg_req_ptr->block_id;

     /* --------------------------------------------------------------
     ** The block_len is filled in later by the packing routines, and
     ** is set to zero if the block is not supported.

        If SP_LOCKED_STATE or NAM_LOCKED_STATE is TRUE, then we
        explicitly set the block len to zero to make sure that
        the blocks are not processed during translation.
     ** -------------------------------------------------------------- */
    if ((sp_locked_state) || (nam_locked_state))
    {
      otasp_int.sspr_cfg_rsp.result_code = OTASP_BLK_REJ_MS_LOCKED;
      otasp_int.sspr_cfg_rsp.block_len = 0;
    }
    else if ((!sp_locked_state) && (!nam_locked_state))
    {
      switch (sspr_cfg_req_ptr->block_id)
      {
        case OTASP_CFG_PRL_DIMENSIONS_BLK :
          MSG_MED("Reporting the dimensions ",0,0,0);
#ifdef FEATURE_NSOTASP
          otasp_int.sspr_cfg_rsp.block_len = 0;
          otasp_int.sspr_cfg_rsp.result_code = OTASP_BLK_REJ_BLOCK_ID;
#else
          #ifdef FEATURE_IS683C_OTASP
          otasp_int.sspr_cfg_rsp.block_len = 0;
          otasp_int.sspr_cfg_rsp.result_code = OTASP_BLK_REJ_PRL_MISMATCH;
          MSG_HIGH("PRL format mismatch",0,0,0);
          #else
          /* this is not the actual block len, the correct one will
          be computed before translation */
          otasp_int.sspr_cfg_rsp.block_len = 1;

          /* Preferred Roaming List Dimensions */
          MSG_MED("PRL Dimensions for IS-683A",0,0,0);
          otasp_int.sspr_cfg_rsp.result_code = OTASP_BLK_ACCEPT;
          otasp_int.sspr_cfg_rsp.block.blk.prl_size = otasp_roam_size;
          #endif
#endif /* FEATURE_NSOTASP */
        break;

        case OTASP_CFG_PREF_ROAM_LIST_BLK:
          sspr_cfg_rsp_populate_prl_blk( &otasp_int.sspr_cfg_rsp,
                                         sspr_cfg_req_ptr );
        break;

        #ifdef FEATURE_IS683C_OTASP
        case OTASP_CFG_EXT_PRL_DIMENSIONS_BLK :

          MSG_MED("Reporting the extended PRL dimensions ",0,0,0);
          /* this is not the actual block len, the correct one will
          be computed before translation */
          otasp_int.sspr_cfg_rsp.block_len = 1;

          /* Preferred Roaming List Dimensions */
          otasp_int.sspr_cfg_rsp.result_code = OTASP_BLK_ACCEPT;
          otasp_int.sspr_cfg_rsp.block.blk.prl_size = otasp_roam_size;
        break;
        #endif /* FEATURE_IS683C_OTASP */
        default:
          /* invalid SSPR Parameter block */
          otasp_int.sspr_cfg_rsp.result_code = OTASP_BLK_REJ_BLOCK_ID;
          otasp_int.sspr_cfg_rsp.block_len = 0;
        break;
      }
    }

    /* Translate and send the Configuration Response Message */

    status = otaspx_int_to_ext( &len, &otasp_int, chari );

    if (status == OTASPX_DONE_S) /*lint !e641 */
    {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
    }
    else
    {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

} /* process_sspr_config_req  */

#ifndef FEATURE_NSOTASP
/*===========================================================================

FUNCTION SSPR_APPEND_RL

DESCRIPTION
  This function takes a received part of a roaming list and puts
  it into the roaming list being accumulated in memory.

DEPENDENCIES
  The first of the received roaming list bits must be byte alligned.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void sspr_append_rl
(
  byte *bits_ptr,
    /* Index into the caii_rx_msg_type where the Roaming List piece begins */
  word segment_offset,
    /* Offset of the segment */
  word segment_size
    /* size of the segment  */
)
{
  static byte *index_ptr; /* Pointer to starting of the roaming list */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (new_roaming_list)
  {
    /* Initialize nv_pr_list and static variables */
    nv_pr_list.nam = cdma.curr_nam;
    nv_pr_list.size = 0;
    index_ptr = (byte*) nv_pr_list.roaming_list;
    (void) memset( index_ptr, 0, NV_ROAMING_LIST_MAX_SIZE );
    new_roaming_list = FALSE;
  }

  /* The offset is always byte aligned, we can do a simple memcpy */
  /*lint -e668*/
  (void) memcpy( (index_ptr + segment_offset), bits_ptr, segment_size );
  /*lint +e668*/

} /* sspr_append_rl()  */
#endif /* FEATURE_NSOTASP */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION SEND_SSPR_DLOAD_RESP_MSG

DESCRIPTION
   This function sends the SSPR Download Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_sspr_dload_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the SSPR Download Response Message */
  ruim_otasp_int.sspr_dload_rsp.msg_type = OTASP_SSPR_DOWNLOAD_RSP_MSG;

  status = otaspx_int_to_ext( &len, &ruim_otasp_int, chari );

  if (status == OTASPX_DONE_S) /*lint !e641*/
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_sspr_dload_resp_msg */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION PROCESS_SSPR_DLOAD_REQ

DESCRIPTION
  This function processes an OTASP SSPR Download Request Message,
  formats the appropriate Protocol Download Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_sspr_dload_req
(
  otaspi_sspr_dload_req_msg_type* sspr_dload_req_ptr
    /* Pointer to SSPR Download Request Message */
)
{
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
#ifndef FEATURE_NSOTASP
  otaspi_ftc_sspr_prl_type *sspr_roam;
  word offset;  /* offset of segment to be downloaded */
  byte size;    /* size of the segment */
  prl_validation_enum_type validn_result;
#endif
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
#if defined( FEATURE_UIM_RUIM )
  uim_cmd_type *uim_cmd_ptr; /* UIM command pointer */
  byte pos = 0;
#endif /* FEATURE_UIM_RUIM */

  boolean is_full_validation;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* Initialize the number of blocks req to and recd from the card */
    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_card = 0;

    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_rsp_card = 0;

    /* Check to see if a message buffer is available */
    if ((otasp_ruim_table.req_index + 1) % OTASP_RUIM_MAX_MSG_BUFS !=
                                   otasp_ruim_table.rsp_index)
    {
       /* Store the message type */
       otasp_ruim_table.msg[otasp_ruim_table.req_index].msg_type =
                OTASP_SSPR_DOWNLOAD_REQ_MSG;

       /* Store the number of blocks requested in the message as only one */
       otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_msg = 1;

       if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
       {
          uim_cmd_ptr->hdr.command = UIM_SSPR_DOWNLOAD_REQ_F;

          /* Fill in the block id and block length in the command */
          uim_cmd_ptr->sspr_download.block_id = sspr_dload_req_ptr->block_id;

          uim_cmd_ptr->sspr_download.block_len = sspr_dload_req_ptr->block_len;

          /* Copy the contents of the PRL data buffer */
          memcpy(uim_cmd_ptr->sspr_download.data,
                 sspr_dload_req_ptr->block.blk.prl_data,
                 sspr_dload_req_ptr->block_len);

          /* move past the reserved bits */
          pos += FSIZ(otaspe_sspr_prl_fix_type, reserved);

          /* unpack the last segment indicator */
          otasp_ruim_last_segment =
              b_unpackb(sspr_dload_req_ptr->block.blk.prl_data, pos,
                        FSIZ(otaspe_sspr_prl_fix_type, last_segment));

          uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

          mcc_uim_cmd( uim_cmd_ptr );

          if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
          {
             ERR( "Bad status from UIM %d", uim_cmd_ptr->hdr.status,0,0);
          }
          else
          {
             /* Increment the number of blocks sent to the card */
             ++(otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                    num_blocks_req_card);
          }
       }
       else
       {
          ERR( "No free buffers on uim_free_q", 0, 0, 0 );

          /* We do not store the id of the rejected block since there is only
             one block in this message */
       }
    } /* check for message buffer availability */

    /* If any block has been sent to the card successfully */
    if (otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card > 0)
    {
        /* consider this message queued and increment the request index */
        otasp_ruim_table.req_index = (otasp_ruim_table.req_index + 1) %
                                    OTASP_RUIM_MAX_MSG_BUFS;
    }
    else
    {
        ruim_otasp_int.sspr_dload_rsp.block_id = sspr_dload_req_ptr->block_id;

        ruim_otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;

        /* send the SSPR Download response message */
        send_sspr_dload_resp_msg();
    }
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    otasp_int.sspr_dload_rsp.msg_type = OTASP_SSPR_DOWNLOAD_RSP_MSG;

    otasp_int.sspr_dload_rsp.block_id = sspr_dload_req_ptr->block_id;

    switch (sspr_dload_req_ptr->block_id) {
      case OTASP_DLOAD_PREF_ROAM_LIST:
    case OTASP_DLOAD_EXT_PREF_ROAM_LIST:
    /*  Accept IS-683A (Block ID=0 as well as IS-683C (Block ID=1) blocks
     ** for PRL download response msg
     */

  #ifdef FEATURE_NSOTASP
         otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_REJ_BLOCK_ID;
         otasp_int.sspr_dload_rsp.segment_offset = 0;
         otasp_int.sspr_dload_rsp.segment_size = 0;
  #else
         MSG_MED("Downloading IS-683A PRL ",0,0,0);

         #ifdef FEATURE_MEM_REDUCE_PRL
         otasp_is_nv_pr_list_valid = FALSE;

         MSG_HIGH( " SSPR DLOAD REQ: otasp_is_nv_pr_list_valid=%d",
                   otasp_is_nv_pr_list_valid, 0, 0);
         #endif /* FEATURE_MEM_REDUCE_PRL */

         /* Initialize this flag to false */
         /* Set this flag to TRUE only if the CRC passes, which also prevents an
            invalid PRL from being committed when an invalid PRL is downloaded
            after a valid PRL is downloaded and later committed */
         commit.commit_roam_list = FALSE;

         /* Block Length is set to zero if parsing fails */
         if (sspr_dload_req_ptr->block_len == 0)
         {
            /* Data Size Mismatch */
            otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_REJ_DATA_SIZE;
            /* ---------------------------------------------------------------
            ** For other blocks we delete any received versions when an
            ** invalid block is received.  Since the Roaming List comes in
            ** multiple blocks, however, it isn't appropriate to do that here.
            ** -------------------------------------------------------------- */

            /* Fill the remaining fields of the response message */
            otasp_int.sspr_dload_rsp.segment_offset =
                        sspr_dload_req_ptr->block.blk.sspr_prl.segment_offset;
            otasp_int.sspr_dload_rsp.segment_size =
                        sspr_dload_req_ptr->block.blk.sspr_prl.segment_size;
         }
         else if (otasp_commit_in_progress && nv_commit.commit_roam_list)
         {
            /* ------------------------------------------------------------
            ** We are in the process of writing a Roaming List to NV, and
            ** rather than creating another huge buffer to handle this
            ** pathological case, we simply reject the block.
            ** ------------------------------------------------------------ */
            otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_REJ_MODE;
         }
         else
         {
            offset = sspr_dload_req_ptr->block.blk.sspr_prl.segment_offset;
            size = sspr_dload_req_ptr->block.blk.sspr_prl.segment_size;
            otasp_int.sspr_dload_rsp.segment_offset = offset;
            otasp_int.sspr_dload_rsp.segment_size = size;

            if ((offset + size) > nv_max_size_of_roaming_list())
            {
               /* segment offset in conjunction with the segment size are
               ** inconsistent with the PRL storage capabilities of the MS,
               ** so reject the request with PRL length mismatch result code */
               otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_REJ_PRL_LEN;
            }
            else
            {
               sspr_roam =  &sspr_dload_req_ptr->block.blk.sspr_prl;

               /* call the function to append the roaming list */
               sspr_append_rl(sspr_roam->bits_ptr, offset, size);

               if (sspr_dload_req_ptr->block.blk.sspr_prl.last_segment)
               {
                  /* this is the last segment of the roaming list */
                  /* this is the size of roaming list in octets */
                  nv_pr_list.size = b_unpackw( (byte*) nv_pr_list.roaming_list,
                                                0,
                                                PR_LIST_SIZE_LENGTH);
                  /* Convert the size in bytes to bits before storing in NV */
                  nv_pr_list.size = (word) 8 * nv_pr_list.size; /*lint !e734 */

                  /* Validate prl based on SSPR P REV value.
                  ** Based on IS 683 Table 4.5.3-1 SSPR Para block type
                  **
                  ** Parameter Block Type     Block ID
                  **
                  **         PRL                 0
                  **     Extended PRL            1
                  **
                  ** Validating senario:
                  **   If block ID = 0, there is a case there is extended
                  **   PRL is downloaded. Do a full validation.
                  **   If block ID = 1, validate ext PRL only.
                  **
                  **   Update sspr_p_rev nv item.
                  */
                  if( sspr_dload_req_ptr->block_id == 0 )
                  {
                    sspr_p_rev = PRL_SSPR_P_REV_1;
                    is_full_validation = TRUE;
                  }
                  else
                  {
                    sspr_p_rev = PRL_SSPR_P_REV_3;
                    is_full_validation = FALSE;
                  }

                  validn_result =
                       prl_validate( (byte*) nv_pr_list.roaming_list, NULL, 
                                      &sspr_p_rev, is_full_validation);

                  if (validn_result == PRL_INVALID_CRC)
                  {
                     ERR("Invalid CRC", 0, 0, 0);
                     otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_REJ_CRC;
                     /* how to discard the previous and current segments */
                     nv_pr_list.size = 0;
                  }
                  else if (validn_result == PRL_VALID)
                  {
                    /* Update sspr_p_rev
                    */
                    otasp_nv_buf.item = NV_SSPR_P_REV_I;
                    otasp_nv_buf.cmd = NV_WRITE_F;
                    otasp_nv_buf.data_ptr =(nv_item_type*)&sspr_p_rev; /*lint !e826 */

                    (void) mcc_nv_cmd( &otasp_nv_buf);
                                        
                     otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_ACCEPT;
                     nv_pr_list.valid = TRUE;
                     /* Set the flag to TRUE */
                     commit.commit_roam_list = TRUE;
                     commit.roam_list_ptr = &nv_pr_list;

                     #ifdef FEATURE_OTASP_STATUS
                     /* Send OTASP status to CM */
                     otasp_report_status_to_cm(CM_OTASP_STATUS_PRL_DOWNLOADED);
                     #endif /* FEATURE_OTASP_STATUS */
                  }
                  else
                  {
                     otasp_int.sspr_dload_rsp.result_code =
                                        OTASP_BLK_REJ_UNKNOWN;
                     /* how to discard the previous and current segments */
                     nv_pr_list.size = 0;
                     nv_pr_list.nam = commit.nam;
                  }
                  new_roaming_list = TRUE;
               }
               else
               {
                  /* accept the segment of the roaming list */
                  otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_ACCEPT;
               }
            }
         }
  #endif /* FEATURE_NSOTASP */
      break;

      default:
          /* invalid SSPR Parameter block */
          otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_REJ_BLOCK_ID;
          otasp_int.sspr_dload_rsp.segment_offset = 0;
          otasp_int.sspr_dload_rsp.segment_size = 0;

      break;
    }

    /* Translate and send the SSPR Download Response Message */

    status = otaspx_int_to_ext( &len, &otasp_int, chari );

    if (status == OTASPX_DONE_S) /*lint !e641 */
    {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
    }
    else
    {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

} /* process_sspr_dload_req  */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION SEND_VALIDN_RESP_MSG

DESCRIPTION
   This function sends the Validation Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_validn_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the Validation Response Message */
  ruim_otasp_int.validn_rsp.msg_type = OTASP_VALIDATION_RSP_MSG;
  status = otaspx_int_to_ext( &len, &ruim_otasp_int, chari );

  if (status == OTASPX_DONE_S) /*lint !e641*/
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_validn_resp_msg */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION PROCESS_VALIDATION_REQ

DESCRIPTION
  This function processes an OTASP Validation Request Message,
  formats the appropriate Protocol Validation Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  Next state to be processed.  CDMA_CONV or CDMA_EXIT for powerdown.

SIDE EFFECTS
  None.

===========================================================================*/

word process_validation_req
(
  otaspi_validation_req_msg_type* validn_req_ptr
    /* Pointer to Validation Request Message */
)
{
  word x;
  word pos1,pos2;
  otaspi_validation_req_msg_type validn_temp;
  word next_state; /* Next state to be processed */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
#ifndef FEATURE_NSOTASP
  byte zeros[NV_SEC_CODE_SIZE];
  /* String to check against the SPC to see if it is zero */
  word y;
#endif
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
#if defined( FEATURE_UIM_RUIM )
  /* Pointer to UIM command */
  uim_cmd_type  *uim_cmd_ptr;
  /* index for tracking rejected blocks */
  byte rej_block_index = 0;
#endif /* FEATURE_UIM_RUIM */
  mccccim_event_type mccccim_evt;
    /* Event rpt struct for sending primitives to CCIM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  next_state = CDMA_TRAFFIC; /*lint !e641 */

  /* The following lines of code help us to find out if CHANGE_SPC_BLOCK
  ** comes before the VERIFY_SPC_BLOCK. If both the blocks are present,
  ** then irrespective of the order, VERIFY_SPC_BLOCK has to be processed
  ** before CHANGE_SPC_BLOCK */
  pos1 = 0;
  pos2 = OTASPI_VALIDN_BLOCK_MAX - 1 ;
  for (x = 0; x < validn_req_ptr->num_blocks; x++) {
     if (validn_req_ptr->block[x].block_id == OTASP_VERIFY_SPC_BLOCK)
     {
        pos1 = x;
     }
     else if ( validn_req_ptr->block[x].block_id == OTASP_CHANGE_SPC_BLOCK)
     {
        pos2 = x;
     }
  }

  /* if CHANGE_SPC_BLOCK precedes VERIFY_SPC_BLOCK */
  if (pos2 < pos1)
  {
     /* We swap their order within the message */
     validn_temp.block[0] = validn_req_ptr->block[pos2];
     validn_req_ptr->block[pos2] = validn_req_ptr->block[pos1]; /*lint !e661 */
     validn_req_ptr->block[pos1] = validn_temp.block[0]; /*lint !e661 */
  }

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* Initialize the number of blocks req to and recd from the card */
    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_card = 0;

    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_rsp_card = 0;

    /* Check to see if a message buffer is available */
    if ((otasp_ruim_table.req_index + 1) % OTASP_RUIM_MAX_MSG_BUFS !=
                                   otasp_ruim_table.rsp_index)
    {
      /* Store the message type */
      otasp_ruim_table.msg[otasp_ruim_table.req_index].msg_type =
                          OTASP_VALIDATION_REQ_MSG;

      /* Store the number of blocks requested in the message */
      otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_msg =
                validn_req_ptr->num_blocks;

      /* send a command for each block to the card */
      for (x = 0; x < validn_req_ptr->num_blocks; x++)
      {
        if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
        {
           uim_cmd_ptr->hdr.command = UIM_VALIDATE_F;

           uim_cmd_ptr->validate.block_id = validn_req_ptr->block[x].block_id;

           uim_cmd_ptr->validate.block_length = validn_req_ptr->block[x].block_len;

           memcpy(uim_cmd_ptr->validate.data,
                  validn_req_ptr->block[x].blk.validn_data,
                  validn_req_ptr->block[x].block_len);

           uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

           mcc_uim_cmd( uim_cmd_ptr );

           if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
           {
               ERR( "Bad status from UIM %d", uim_cmd_ptr->hdr.status,0,0);

               /* Store the id of the block that is not processed by the card */
               otasp_ruim_table.msg[otasp_ruim_table.req_index].
                   rej_block_id[rej_block_index++] =
                            validn_req_ptr->block[x].block_id;
           }
           else
           {
               /* Increment the number of blocks sent to the card */
               ++(otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card);
           }
        }
        else
        {
           ERR( "No free buffers on uim_free_q", 0, 0, 0 );

           /* Store the id of the block that is not sent to the card */
           otasp_ruim_table.msg[otasp_ruim_table.req_index].
               rej_block_id[rej_block_index++] =
                             validn_req_ptr->block[x].block_id;
        }
      }   /* end of for loop */
    } /* check for message buffer availability */

    /* If any block has been sent to the card successfully */
    if (otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card > 0)
    {
        /* consider this message queued and increment the request index */
        otasp_ruim_table.req_index = (otasp_ruim_table.req_index + 1) %
                                    OTASP_RUIM_MAX_MSG_BUFS;
    }
    else
    {
        /* none of the blocks are sent to the card */
        ruim_otasp_int.validn_rsp.num_blocks = validn_req_ptr->num_blocks;

        /* send a reject response for all the blocks in this message */
        for (x = 0; x < validn_req_ptr->num_blocks; x++)
        {
            ruim_otasp_int.validn_rsp.block[x].block_id =
                                       validn_req_ptr->block[x].block_id;

            ruim_otasp_int.validn_rsp.block[x].result_code= OTASP_BLK_REJ_UNKNOWN;
        }

        /* send the validation response message */
        send_validn_resp_msg();
    }
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    /* do the normal processing */
    otasp_int.validn_rsp.msg_type = OTASP_VALIDATION_RSP_MSG;
    otasp_int.validn_rsp.num_blocks = validn_req_ptr->num_blocks;

    for (x = 0; x < validn_req_ptr->num_blocks; x++)
    {
      otasp_int.validn_rsp.block[x].block_id =
                  validn_req_ptr->block[x].block_id;

      switch (validn_req_ptr->block[x].block_id) {

        case OTASP_VERIFY_SPC_BLOCK:
        {
#ifdef FEATURE_NSOTASP
         otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_REJ_BLOCK_ID;
#else
          boolean valid_block = TRUE; /* Flag if given SPC has valid digits */

          /* Block Length is set to zero if parsing fails */
          if (validn_req_ptr->block[x].block_len == 0)
          {
            /* Data Size Mismatch */
            otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_REJ_DATA_SIZE;
          }
          else
          {
            if (validn_req_ptr->block[x].block_len <
                                 ((NV_SEC_CODE_SIZE + 1) / 2))
            {
               /* SPC Parameter Block does not have enough digits! */
               otasp_int.validn_rsp.block[x].result_code =
                      OTASP_BLK_REJ_DATA_SIZE;
            }
            else
            {
               /* Check all digits to make sure they are valid */
               for (y = 0; y < NV_SEC_CODE_SIZE; y++)
               {
                 /* Valid BCD digits are 0-9, but also count 10 as zero */
                 if (validn_req_ptr->block[x].blk.verify_spc.spc[y] > 10)
                 {
                   /* Invalid parameter */
                   otasp_int.validn_rsp.block[x].result_code =
                         OTASP_BLK_REJ_INVALID;
                   valid_block = FALSE;
                   break;
                 }
                 else
                 {
                   /* Convert BCD to ASCII for comparison with stored SPC */
                   if (validn_req_ptr->block[x].blk.verify_spc.spc[y] == 10)
                   {
                     /* If we got a 10, assume it is a zero */
                     validn_req_ptr->block[x].blk.verify_spc.spc[y] = 0;
                   }
                   validn_req_ptr->block[x].blk.verify_spc.spc[y] += '0';
                 }
               }

               if (valid_block)
               {
                  if (memcmp( validn_req_ptr->block[x].blk.verify_spc.spc,
                          cdma.spc, NV_SEC_CODE_SIZE ) != 0)
                  {
                     /* Service Programming Code sent doesn't match */
                     otasp_int.validn_rsp.block[x].result_code =
                                        OTASP_BLK_REJ_SPC;

                     if (cdma.otasp_call == USER_INIT_SRV_PROV)
                     {
                        if (++otasp_failed_unlock_attempts ==
                                                  OTASP_MAX_UNLOCK_ATTEMPTS)
                        {
                          MSG_HIGH("%d SPC validation failures, terminate OTASP",
                                           otasp_failed_unlock_attempts,0,0);
                          #ifdef FEATURE_OTASP_STATUS
                          /* Send OTASP status to CM */
                          otasp_report_status_to_cm(CM_OTASP_STATUS_SPC_RETRIES_EXCEEDED);
                          #endif /* FEATURE_OTASP_STATUS */

                          /* Send Notification to MCCCCIM to release all CC instances */
                          mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
                          /* All CC instances should already be in release substate */
                          mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_REL_OTASP_ERR;
                          mccccim_process_event( &mccccim_evt );
                          /* No need to check return status in this case */

                          next_state = CDMA_RELEASE;
                          cdma.rel_orig = FALSE;
                          cdma.otasp_call = NO_SRV_PROV;

                          /* Events report: Release */
                          /* Treat it as BS originated because upper layers are not aware of it */
                          mclog_report_release(MCLOG_RELEASE_BS_ORDER);
                        }
                     } /* USER_INIT_SRV_PROV */
#ifdef FEATURE_OTASP_OTAPA
                     else
                     {
                        /* OTAPA call, don't power down */
                        if (++otasp_otapa_failed_spc_val_attempts ==
                                                  OTASP_MAX_UNLOCK_ATTEMPTS)
                        {
                           /* We don't want to powerdown if OTAPA session occurs
                           ** in the middle of a conversation. */
                           MSG_HIGH("%d SPC validation failures, terminate OTAPA",
                                   otasp_otapa_failed_spc_val_attempts,0,0);

                           cdma.otasp_call = NO_SRV_PROV;
                        }
                     } /* OTAPA call */
#endif /* FEATURE_OTASP_OTAPA */
                  }
                  else
                  {
                      /* We got a good SPC, so unlock the phone */
                      otasp_int.validn_rsp.block[x].result_code =
                                         OTASP_BLK_ACCEPT;
                      sp_locked_state = FALSE;

                      #ifdef FEATURE_OTASP_STATUS
                      /* Send OTASP status to CM */
                      otasp_report_status_to_cm(CM_OTASP_STATUS_SPL_UNLOCKED);
                      #endif /* FEATURE_OTASP_STATUS */
                  }
               } /* end if (valid_block) */
            } /* end if (enough digits) */
          }
#endif /* FEATURE_NSOTASP */
          break;
        }

        case OTASP_CHANGE_SPC_BLOCK:
        {
#ifdef FEATURE_NSOTASP
          otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_REJ_BLOCK_ID;
#else
          boolean valid_block = TRUE; /* Flag if given SPC has valid digits */

          /* Block Length is set to zero if parsing fails */
          if (validn_req_ptr->block[x].block_len == 0)
          {
            /* Data Size Mismatch */
            otasp_int.validn_rsp.block[x].result_code =
                       OTASP_BLK_REJ_DATA_SIZE;
          }
          else if (validn_req_ptr->block[x].block_len <
                                 ((NV_SEC_CODE_SIZE + 1) / 2))
          {
             /* SPC Parameter Block does not have enough digits! */
             otasp_int.validn_rsp.block[x].result_code =
                             OTASP_BLK_REJ_DATA_SIZE;
          }
          else
          {
            /* Check all digits to make sure they are valid */
            for (y = 0; y < NV_SEC_CODE_SIZE; y++)
            {
               /* Valid BCD digits are 0-9, but also count 10 as zero */
               if (validn_req_ptr->block[x].blk.change_spc.spc[y] > 10)
               {
                  /* Invalid parameter */
                  otasp_int.validn_rsp.block[x].result_code =
                       OTASP_BLK_REJ_INVALID;
                  valid_block = FALSE;
                  break;
               }
               else
               {
                 /* Convert BCD to ASCII for storing the SPC */
                 if (validn_req_ptr->block[x].blk.change_spc.spc[y] == 10)
                 {
                    /* If we got a 10, assume it is a zero */
                    validn_req_ptr->block[x].blk.change_spc.spc[y] = 0;
                 }
                 validn_req_ptr->block[x].blk.change_spc.spc[y] += '0';
               }
            }
            if (valid_block)
            {
               if (sp_locked_state)
               {
                  otasp_int.validn_rsp.block[x].result_code =
                       OTASP_BLK_REJ_MS_LOCKED;
               }
               else /* spc lock state is zero */
               {
                if (memcmp( cdma.spc,
                   memset( zeros, '0', NV_SEC_CODE_SIZE ),
                   NV_SEC_CODE_SIZE ) == 0)
                {
                    if (cdma.spc_change_enabled)
                    {
                       otasp_int.validn_rsp.block[x].result_code =
                            OTASP_BLK_ACCEPT;

                       /* copy the SPC into local memory */
                       memcpy( (byte*) commit.spc.digits,
                               validn_req_ptr->block[x].blk.change_spc.spc,
                               NV_SEC_CODE_SIZE );
                       commit.commit_spc = TRUE;
                    }
                    else
                    {
                       /* the user has disabled the spc change  */
                       otasp_int.validn_rsp.block[x].result_code =
                        OTASP_BLK_REJ_SPC_USER;
                    }
                 }
                 else
                 {
                    otasp_int.validn_rsp.block[x].result_code =
                            OTASP_BLK_ACCEPT;

                    /* copy the SPC into local memory */
                    memcpy( (byte*) commit.spc.digits,
                            validn_req_ptr->block[x].blk.change_spc.spc,
                            NV_SEC_CODE_SIZE );
                    commit.commit_spc = TRUE;
                 }
               } /* sp lock state  */
            }  /* valid block */
          } /* else condition */

#endif /* FEATURE_NSOTASP */
          break;
        }

        case VALIDATE_SPASM:
        {
#if defined (FEATURE_OTASP_OTAPA) && defined (FEATURE_AUTH)

          /* Not in a OTAPA call */
          if (cdma.otasp_call != NETWORK_INIT_SRV_PROV)
          {
            MSG_MED("Rej: not an OTAPA call",0,0,0);
            otasp_int.validn_rsp.block[x].result_code =
                                             OTASP_BLK_REJ_MODE_BLOCK_ID;
          }
          /* NAM is unlocked, save nam_lock and accept */
          else if (!nam_locked_state)
          {
            MSG_MED("Accept: NAM is unlocked",0,0,0);
            otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_ACCEPT;

            /* We commit nam_lock only if nam_lock in NV is different from what
            ** base station sent to the mobile.
            */
            if (cdma.nam_lock != validn_req_ptr->block[x].blk.
                                                     validate_spasm.nam_lock)
            {
              commit.nam_lock.enabled = validn_req_ptr->block[x].blk.
                                                     validate_spasm.nam_lock;
              commit.commit_nam_lock = TRUE;
            }
          }
          /* auth_otapa is not included in the validation message but NAM is
             locked */
          else if (!validn_req_ptr->block[x].blk.validate_spasm.auth_otapa_incl)
          {
            MSG_MED("Rej:auth_otapa not included",0,0,0);
            otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_REJ_SPASM;
          }
          /* auth_otapa has already calculated and ready - e.g. multiple SPASM
          ** validation attempts to unlock the phone.
          */
          else if (otasp_auth_otapa_ready)
          {
            MSG_MED("auth_otapa %lx", otasp_auth_otapa, 0, 0);
            MSG_MED("auth_otapa_r %lx",
                    validn_req_ptr->block[x].blk.validate_spasm.auth_otapa, 0, 0);

            if (otasp_auth_otapa == validn_req_ptr->block[x].blk.validate_spasm.
                                                                     auth_otapa)
            {
              MSG_MED("SPASM passed, unlock the mobile",0,0,0);
              /* auth_otapa match, SPASM validation passed. */
              otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_ACCEPT;
              nam_locked_state = FALSE;

              /* We commit nam_lock only if nam_lock in NV is different from what
              ** base station sent to the mobile.
              */
              if (cdma.nam_lock !=
                          validn_req_ptr->block[x].blk.validate_spasm.nam_lock)
              {
                commit.nam_lock.enabled =
                          validn_req_ptr->block[x].blk.validate_spasm.nam_lock;
                commit.commit_nam_lock = TRUE;
              }
            }
            else
            {
              MSG_HIGH("Rej: SPASM failed",0,0,0);
              /* Failed SPASM validation */
              otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_REJ_SPASM;

              if (++otasp_otapa_failed_spasm_val_attempts ==
                                                      OTASP_MAX_UNLOCK_ATTEMPTS)
              {
                 MSG_HIGH("%d SPASM validation failures, terminate OTAPA",
                         otasp_otapa_failed_spasm_val_attempts,0,0);
                 cdma.otasp_call = NO_SRV_PROV;
              }
            }
          }
          /* auth_otapa is not ready, send reject message. */
          else
          {
            MSG_MED("Rej: auth_otapa not ready", 0,0,0);
            otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_REJ_UNKNOWN;
          }

#else /* FEATURE_OTASP_OTAPA && FEATURE_AUTH */
          ERR("OTAPA or AUTH not defined",0,0,0);
          otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_REJ_UNKNOWN;
#endif /* FEATURE_OTASP_OTAPA && FEATURE_AUTH */
          break;
        }

        default:
          /* invalid Validation Parameter block */
          otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_REJ_BLOCK_ID;

        break;

      }
    }

    /* Translate and send the Validation Response Message */

    status = otaspx_int_to_ext( &len, &otasp_int, chari );

    if (status == OTASPX_DONE_S)  /*lint !e641 */
    {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
    }
    else
    {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
  return (next_state);

} /* process_validation_req  */

/*===========================================================================

FUNCTION OTASP_PROCESS_MSG

DESCRIPTION
  This function processes an OTASP message.

DEPENDENCIES
  None.

RETURN VALUE
  Next state to be processed.  CDMA_CONV or CDMA_EXIT for powerdown.

SIDE EFFECTS
  None.

===========================================================================*/

word otasp_process_msg
(
  otaspi_ftc_msg_type *otasp_msg_ptr
    /* OTASP message to be processed */
)
{
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
  word next_state; /* Next state to be processed */

#if (defined (FEATURE_AUTH) && defined(FEATURE_DH))
   auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
#endif

#ifdef FEATURE_UIM_RUIM
   uim_cmd_type *uim_cmd_ptr;
    /* Pointer to UIM command */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
   word prl_version;
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ---------------------------------------------------------
  ** OTASP messages are only processed in CDMA_CONV substate.
  ** Currently next_state is only changed if too many attempts
  ** are made to unlock the phone, in which case we powerdown.
  ** --------------------------------------------------------- */
  next_state = CDMA_TRAFFIC; /*lint !e641 */

  MSG_MED("Received OTASP message type %d", otasp_msg_ptr->gen.msg_type,0,0);

  switch (otasp_msg_ptr->gen.msg_type)
  {
    case OTASP_CONFIG_REQ_MSG:
      /* IS-683 OTASP Configuration Request Message */
      /* If a commit is in progress, we give the old configuration info */
      process_config_req (&otasp_msg_ptr->config_req);
    break;

    case OTASP_DOWNLOAD_REQ_MSG:
      /* IS-683 OTASP Download Request Message */
      next_state = process_dload_req (&otasp_msg_ptr->dload_req);
    break;

    case OTASP_MS_KEY_REQ_MSG:
      /* IS-683 OTASP MS Key Request Message */
      process_ms_key_req(&otasp_msg_ptr->ms_key_req);
    break;

    case OTASP_KEY_GEN_REQ_MSG:
      /* IS-683 OTASP Key Generation Request Message */
      process_key_gen_req(&otasp_msg_ptr->key_gen_req);
    break;

    case OTASP_REAUTH_REQ_MSG:
      /* IS-683 OTASP Re-authentication Request Message */
      process_reauth_req(&otasp_msg_ptr->reauth_req);
    break;

    case OTASP_COMMIT_REQ_MSG:
    {
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
      /* Determine if the R-UIM is available for this NAM */
      if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
      {
        if (otasp_commit_in_progress)
        {
          /* If we are already committing, then drop the second commit */
        }
        else
        {
            if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
            {
                uim_cmd_ptr->hdr.command = UIM_COMMIT_F;
                uim_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
                uim_cmd_ptr->hdr.cmd_hdr.sigs = NULL;
                uim_cmd_ptr->hdr.options = UIM_OPTION_ALWAYS_RPT;

                uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;
                uim_cmd( uim_cmd_ptr );

                /* Notify CM that otasp commit starts. */
                otasp_report_commit_status_to_cm(TRUE);

                otasp_commit_in_progress = TRUE;
            }
            else
            {
                ERR( "No free buffers on uim_free_q", 0, 0, 0 );
                otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
                send_commit_resp_msg();
            }
        }
      }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
      else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
      {
        boolean send_response = FALSE;
        otasp_committed_both_flag = FALSE;

        if (otasp_commit_in_progress)
        {
          /* If we are already committing, then drop the second commit */
        }
#ifdef FEATURE_NSOTASP
        else if (!otasp_commit_allowed)
#else
        else if ((sp_locked_state) || (nam_locked_state))
#endif /* FEATURE_NSOTASP */
        {
          /* ----------------------------------------------------------
          ** The phone is locked and is not allowed to commit anything.
          ** We must send the Commit Response with result code saying
          ** the SPC is required first.
          ** For IS-683A we send the result code as MS locked.
          ** ---------------------------------------------------------- */
          send_response = TRUE;
#ifdef FEATURE_NSOTASP
          otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_SPC_REQ;
#else
          otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_MS_LOCKED;
#endif /* FEATURE_NSOTASP */
        }
        else if (commit.commit_mob_dir || commit.commit_ftc_cdma ||
                 commit.commit_ftc_amps || commit.commit_roam_list
#ifdef FEATURE_AUTH
                 || a_key_temp_valid
#endif /* FEATURE_AUTH */
#ifndef FEATURE_NSOTASP
                 || commit.commit_spc || commit.commit_imsi_t
#ifdef FEATURE_OTASP_OTAPA
                 || commit.commit_nam_lock
#endif /* FEATURE_OTASP_OTAPA */
#endif /* FEATURE_NSOTASP */
#ifdef FEATURE_EHRPD
                 || commit_ehrpd_imsi
#endif /* FEATURE_EHRPD */
          )
        {
          if (commit.commit_mob_dir || commit.commit_ftc_cdma ||
                 commit.commit_ftc_amps || commit.commit_roam_list
#ifndef FEATURE_NSOTASP
                 || commit.commit_spc || commit.commit_imsi_t
#ifdef FEATURE_OTASP_OTAPA
                 || commit.commit_nam_lock
#endif /* FEATURE_OTASP_OTAPA */
#endif /* FEATURE_NSOTASP */
#ifdef FEATURE_EHRPD
                 || commit_ehrpd_imsi
#endif /* FEATURE_EHRPD */
             )
          {

            /* -----------------------------------------------------------
            ** If we are committing NAM info then commit.commit_ftc_cdma
            ** should be TRUE, since there is no case where we only commit
            ** analog NAM info.
            ** ----------------------------------------------------------- */
            if (!commit.commit_ftc_cdma && commit.commit_ftc_amps)
            {
              ERR( "Committing only AMPS NAM info!",0,0,0);
            }

            if (commit.commit_roam_list)
            {
              MSG_HIGH ("Commiting PRL",0,0,0);

              /* We need to mark the list as valid or invalid before committing */
              nv_pr_list.valid = FALSE;

              prl_version = PRL_DEFAULT_VER;
              if (prl_validate((byte*) nv_pr_list.roaming_list, &prl_version,
                               &sspr_p_rev, FALSE) ==PRL_VALID)
              {
                sspr_p_rev = PRL_SSPR_P_REV_INVALID;
                nv_pr_list.valid = TRUE;
              }


#ifdef FEATURE_SSPR_800
              if (!mcc_pcs_band_is_supported()) {
              /* For 800 MHz CDMA phones, the list can be enabled or disabled  */
                otasp_nv_item.prl_enabled.nam = cdma.curr_nam;
                otasp_nv_item.prl_enabled.enabled = nv_pr_list.valid;

              /* Write to NV that the list should be enabled or disabled */
                otasp_nv_buf.item = NV_PRL_ENABLED_I;
                otasp_nv_buf.cmd = NV_WRITE_F;
                otasp_nv_buf.data_ptr = &otasp_nv_item;

                (void) mcc_nv_cmd( &otasp_nv_buf);
              }
#endif /* FEATURE_SSPR_800 */

              if (mcc_cellular_band_is_supported() &&
                  mcc_pcs_band_is_supported()) 
              {
                MSG_HIGH ("Resetting PREF MODE for Trimode",0,0,0);
                otasp_nv_item.pref_mode.nam = cdma.curr_nam;
                otasp_nv_item.pref_mode.mode = NV_MODE_AUTOMATIC;

              /* Write to NV that the mode should be reset to Automatic */
                otasp_nv_buf.item = NV_PREF_MODE_I;
                otasp_nv_buf.cmd = NV_WRITE_F;
                otasp_nv_buf.data_ptr = &otasp_nv_item;

                (void) mcc_nv_cmd( &otasp_nv_buf);

              /* Now notify CM of the Pref Mode change */
                cmph_otasp_update_mode((cm_nam_e_type) cdma.curr_nam, NV_MODE_AUTOMATIC);
              }

              /* Set the PRL version before committing */
              nv_pr_list.prl_version = prl_version;

            }

#ifdef FEATURE_EHRPD
            if (commit_ehrpd_imsi)
            {
              MSG_HIGH ("Commiting eHRPD IMSI",0,0,0);

              otasp_nv_item.ehrpd_imsi = otasp_ehrpd_imsi;

              /* Write to NV that the list should be enabled or disabled */
              otasp_nv_buf.item = NV_EHRPD_IMSI_I;
              otasp_nv_buf.cmd = NV_WRITE_F;
              otasp_nv_buf.data_ptr = &otasp_nv_item;

              (void) mcc_nv_cmd( &otasp_nv_buf);
            }
#endif /* FEATURE_EHRPD */

            /* Copy information into buffer for NV commit command */
            nv_commit = commit;

          /* Clear the commit buffer for future download requests */
            commit.commit_mob_dir   = FALSE;
            commit.commit_ftc_cdma  = FALSE;
            commit.commit_ftc_amps  = FALSE;
            commit.commit_roam_list = FALSE;
#ifndef FEATURE_NSOTASP
            commit.commit_spc     = FALSE;
            commit.commit_imsi_t  = FALSE;
#ifdef FEATURE_OTASP_OTAPA
            commit.commit_nam_lock = FALSE;
#endif /* FEATURE_OTASP_OTAPA */
#endif

            /* Commit OTASP information to NVRAM */
            otasp_nv_buf.cmd = NV_OTASP_COMMIT_F;
            otasp_nv_item.otasp_commit = &nv_commit;
            otasp_nv_buf.data_ptr = &otasp_nv_item;

            /* Fill in necessary fields of command block */
            otasp_nv_buf.done_q_ptr = NULL;
            otasp_nv_buf.tcb_ptr = &mc_tcb;
            otasp_nv_buf.sigs = MCC_OTASP_COMMIT_SIG;
            nv_cmd( &otasp_nv_buf );

            otasp_commit_in_progress = TRUE;
#ifdef FEATURE_AUTH
            if (a_key_temp_valid)
            {
               otasp_commit_akey_pending = TRUE;
            }
            else
            {
               otasp_commit_akey_pending = FALSE;
            }
#endif /* FEATURE_AUTH */
            MSG_MED("OTASP commit in progress",0,0,0);
          }
          else
          {
#if (defined (FEATURE_DH)  && defined (FEATURE_AUTH ))

            if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
            {

               /* Fill in necessary fields of command block */

               /* Copy information into buffer for NV commit command */
               nv_commit = commit;

               auth_cmd_ptr->hdr.command = AUTH_COMMIT_KEYS_F;
               auth_cmd_ptr->hdr.cmd_hdr.task_ptr = &mc_tcb;
               auth_cmd_ptr->hdr.cmd_hdr.sigs = MCC_OTASP_COMMIT_SIG;

               auth_cmd_ptr->commit_keys.nam_index = cdma.curr_nam;
               auth_cmd_ptr->commit_keys.rpt_function = mcc_queue_auth_rpt;
               otasp_commit_in_progress = TRUE;

               otasp_commit_akey_pending = FALSE;

               auth_cmd( auth_cmd_ptr );
               if (auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S)
               {
                   ERR_FATAL( "Bad status from AUTH %d",
                                                auth_cmd_ptr->hdr.status,0,0);
               }
             }
             else
             {
                ERR_FATAL( "No free buffers on auth_free_q", 0, 0, 0 );
             }
#endif /* FEATURE_DH */
          /* We will send the response later, after the commit completes */
          }

          /* Notify CM that otasp commit starts. */
          otasp_report_commit_status_to_cm(TRUE);
        }
        else
        {
          /* --------------------------------------------------------------
          ** We have nothing to commit, but if our commit response got lost
          ** somewhere, a second commit may have been sent, so to be safe
          ** we immediately respond with result code ACCEPT.
          ** -------------------------------------------------------------- */
          send_response = TRUE;
          otasp_int.commit_rsp.result_code = OTASP_BLK_ACCEPT;
        }

        if (send_response)
        {
          otasp_int.commit_rsp.msg_type = OTASP_COMMIT_RSP_MSG;
          status = otaspx_int_to_ext( &len, &otasp_int, chari );

          if (status == OTASPX_DONE_S) /*lint !e641 */
          {
            send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
          }
          else
          {
            ERR_FATAL( "Failed OTASP message translation",0,0,0);
          }
        }
      }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
      break;
    }

    case OTASP_PROT_CAP_REQ_MSG:
      /* IS-683 OTASP Protocol Capability Request Message */
      process_prot_cap_req(&otasp_msg_ptr->prot_cap_req);
    break;

    case OTASP_SSPR_CONFIG_REQ_MSG:
      /* IS-683A OTASP SSPR Configuration Request Message  */
      process_sspr_config_req(&otasp_msg_ptr->sspr_cfg_req);
    break;

    case OTASP_SSPR_DOWNLOAD_REQ_MSG:
      /* IS-683A OTASP SSPR Download Request Message  */
      process_sspr_dload_req(&otasp_msg_ptr->sspr_dload_req);
    break;

    case OTASP_VALIDATION_REQ_MSG:
      /* IS-683A OTASP Validation Request Message  */
      next_state = process_validation_req(&otasp_msg_ptr->validn_req);
    break;

    case OTASP_OTAPA_REQ_MSG:
      /* IS-683A OTAPA Request Message */
      next_state = process_otapa_req(&otasp_msg_ptr->otapa_req);
    break;

    default:
    break;
  }

  return (next_state);

} /* otasp_process_msg */

/*===========================================================================

FUNCTION OTASP_COMMON_INIT

DESCRIPTION
  This function initializes the OTASP/OTAPA related common variables and
  flags. Should be called in otasp_init and in otasp_otapa_init only.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void otasp_common_init (void)
{
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  byte zeros[NV_SEC_CODE_SIZE];
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
   /* String to check against the SPC to see if it is zero */
  byte i;
   /* index variable */

/* Point at NV PRL information structure.
*/
const prl_hdr_s_type  *prl_hdr_ptr = prl_hdr_nv_get_ptr();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("Initializing OTASP/OTAPA",0,0,0);
  otasp_commit_in_progress = FALSE;
  (void) rex_clr_sigs( &mc_tcb, MCC_OTASP_COMMIT_SIG );

  /* Initilalize the sp_lock_state to TRUE */
  sp_locked_state = TRUE;

#ifdef FEATURE_AUTH
  /* Initialize the Key exchange related flags  */
  a_key_temp_valid = FALSE;
  new_ssd_valid = FALSE;
#endif /*  FEATURE_AUTH */

  /* Initialize the commit structure */
  commit.commit_mob_dir = FALSE;
  commit.commit_ftc_cdma = FALSE;
  commit.commit_ftc_amps = FALSE;
  commit.commit_roam_list = FALSE;
#ifndef FEATURE_NSOTASP
  commit.commit_spc = FALSE;
  commit.commit_imsi_t = FALSE;
#ifdef FEATURE_OTASP_OTAPA
  commit.commit_nam_lock = FALSE;
#endif /* FEATURE_OTASP_OTAPA */
#endif /* !FEATURE_NSOTASP */
#ifdef FEATURE_EHRPD
  commit_ehrpd_imsi = FALSE;
#endif /* FEATURE_EHRPD */

  /* Initialize the SID NID pairs */
  for (i = 0; i < OTASPI_SID_NID_MAX ; i++)
  {
    commit.ftc_cdma.pair[i].sid = 0;
    commit.ftc_cdma.pair[i].nid = 0;
  }
  
  #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (!nv_rtre_use_ruim_for_nam(cdma.curr_nam))
  #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* Get the initial roaming list stats from System Determination */
    otasp_get_prl_stats( &otasp_roam_size, prl_hdr_ptr );

    #ifdef FEATURE_MEM_REDUCE_PRL
    /* Do not use this buffer for SSPR cfg request for uploading PRL. */
    otasp_is_nv_pr_list_valid = FALSE;
    #endif /* EATURE_MEM_REDUCE_PRL */
  }

#ifdef FEATURE_DH_EXP
  otasp_dh.state = OTASP_WAIT_FOR_DH;
#endif /* FEATURE_DH_EXP */
#ifdef FEATURE_AUTH
  otasp_reauth_in_progress = FALSE;
#endif /* FEATURE_AUTH */

#ifdef FEATURE_NSOTASP
  /* First Roaming List block we expect is zero */
  otasp_rl_seq = 0;
  /* Initially the phone is locked so no commit operation is allowed */
  otasp_commit_allowed = FALSE;
#endif /* FEATURE_NSOTASP */

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (!nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* If SPC is all zeros, that means we can unlock the phone immediately */
    if (memcmp( cdma.spc,
                memset( zeros, '0', NV_SEC_CODE_SIZE ),
                NV_SEC_CODE_SIZE ) == 0)
    {
#ifdef FEATURE_NSOTASP
      otasp_commit_allowed = TRUE;
#endif /* FEATURE_NSOTASP  */
      sp_locked_state = FALSE;

      #ifdef FEATURE_OTASP_STATUS
      /* Send OTASP status to CM */
      otasp_report_status_to_cm(CM_OTASP_STATUS_SPL_UNLOCKED);
      #endif /* FEATURE_OTASP_STATUS */
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

  /* Maximum 6 attempts are allowed in same programming session to validate 
   * SPC/SPASM to unlock the phone. Resetting count to 0 if new
   * programming session starts. This will also prevent resetting of 
   * validation failure attempts by new programming session if max attempts 
   * have already happened and MS did not power cycle. */
  if(otasp_failed_unlock_attempts < OTASP_MAX_UNLOCK_ATTEMPTS)
  {
    otasp_failed_unlock_attempts = 0;
  }

  /* Tell NV which NAM to commit this to */
  commit.nam = cdma.curr_nam;

  /* Initialize AMPS HO Flag */
  otasp_call_ho_from_cdma = FALSE;

#ifdef FEATURE_OTASP_OTAPA
#ifdef FEATURE_AUTH
  /* Flag indicates whether mobile is calculating auth_otapa */
  otasp_auth_otapa_in_progress = FALSE;

  /* Flag indiates whether auth_otapa is ready */
  otasp_auth_otapa_ready = FALSE;
#endif

#endif /* FEATURE_OTASP_OTAPA */

#ifdef FEATURE_UIM_RUIM

  /* Initialize the indices of the table */
  otasp_ruim_table.req_index = 0;
  otasp_ruim_table.rsp_index = 0;

  otasp_ruim_last_segment = FALSE;

#endif /* FEATURE_UIM_RUIM */

} /* otasp_common_init() */

/*===========================================================================

FUNCTION OTASP_INIT

DESCRIPTION
  This function initializes the OTASP function. Should be called when
  entering CDMA_CONV substate.

  It initializes the following flags as otasp_otapa_init does not:
    otasp_successful_commit
    otasp_otapa_session_occurred

  It sets nam_locked_state to be FALSE.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void otasp_init (void)
{
  /* otasp_common_init initializes OTASP/OTAPA common variables and flags */
  otasp_common_init();

  otasp_successful_commit  = FALSE;

#ifdef FEATURE_OTASP_OTAPA
  /* Flag indicates whether an OTAPA session has occurred */
  otasp_otapa_session_occurred = FALSE;

  /* NAM lock status for the active NAM */
  /* If not in OTAPA call, it shall be set FALSE */
  nam_locked_state = FALSE;

#endif /* FEATURE_OTASP_OTAPA */

} /* otasp_init() */

#ifdef FEATURE_OTASP_OTAPA

/*===========================================================================

FUNCTION OTASP_OTAPA_INIT

DESCRIPTION
  This function initializes the Network-initiated OTAPA function.
  Should be called when an OTAPA session starts.

  It does not reset the following flags as otasp_init does:
    otasp_successful_commit
    otasp_otapa_session_occurred

  It sets nam_locked_state to be cdma.nam_lock

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void otasp_otapa_init (void)
{
  /* otasp_common_init initializes OTASP/OTAPA common variables and flags */
  otasp_common_init();

  MSG_MED("otapa init",0,0,0);
  /* NAM lock status for the active NAM. */
  nam_locked_state = cdma.nam_lock;

  /* Maximum 6 attempts are allowed in same programming session to validate 
   * SPC/SPASM to unlock the phone. Resetting counters to 0 if new
   * programming session starts. This will also prevent resetting of 
   * validation failure attempts by new programming session if max attempts 
   * have already happened. This is a defensive check. This function 
   * is really not called if max attempts have happened and MS did 
   * not power cycle. */
  if(otasp_otapa_failed_spc_val_attempts < OTASP_MAX_UNLOCK_ATTEMPTS)
  {
    otasp_otapa_failed_spc_val_attempts = 0;
  }

  if(otasp_otapa_failed_spasm_val_attempts < OTASP_MAX_UNLOCK_ATTEMPTS)
  {
    otasp_otapa_failed_spasm_val_attempts = 0;
  }

} /* otasp_otapa_init() */
#endif /* FEATURE_OTASP_OTAPA */

/*===========================================================================

FUNCTION OTASP_AHO_HANDOFF

DESCRIPTION
  This function returns whether or not this OTASP call has been handed off
  to an AMPS system.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if current OTASP call has been handed off to an AMPS system,
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean otasp_aho_handoff (void)
{
  return otasp_call_ho_from_cdma;
}

/*===========================================================================

FUNCTION OTASP_SET_AHO_HANDOFF

DESCRIPTION
  This function allows write access to the OTASP AMPS handoff flag.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if current OTASP call has been handed off to an AMPS system,
  FALSE otherwise.

SIDE EFFECTS
  Sets the value of otasp_call_ho_from_cdma.

===========================================================================*/

void otasp_set_aho_handoff (boolean value)
{
  otasp_call_ho_from_cdma = value;
}

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
/*===========================================================================

FUNCTION OTASP_RELOAD_NAM_PARAMS

DESCRIPTION
   This function updates the cdma structure with newly committed info

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void otasp_reload_nam_params (void)
{
    byte i; /* loop counter */

    otasp_successful_commit  = TRUE;

   /* Update the cdma structure to reflect the NV changes */
    if (nv_commit.commit_mob_dir)
    {
      cdma.mob_dir.n_digits = nv_commit.mob_dir.n_digits;
      (void) memcpy( cdma.mob_dir.digitn, (byte*) nv_commit.mob_dir.digitn,
                     nv_commit.mob_dir.n_digits );

      nv_commit.commit_mob_dir = FALSE;
      MSG_MED("mob_dir committed",0,0,0);
    }

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

    if (nv_commit.commit_ftc_cdma)
    {
      /* Save MIN Based IMSI parameters in the cdma_ms structure */
      cdma.imsi_m_addr_num = nv_commit.ftc_cdma.imsi_addr_num;
      cdma.imsi_m_mcc = nv_commit.ftc_cdma.mcc;
      cdma.imsi_m_11_12 = nv_commit.ftc_cdma.imsi_11_12;
      qw_set( cdma.imsi_m_s,(dword)((nv_commit.ftc_cdma.imsi_s2 & 0x0300) >> 8),
            ((nv_commit.ftc_cdma.imsi_s1 & 0x00FFFFFFL) |
             (((dword) nv_commit.ftc_cdma.imsi_s2 & 0x000000FFL) << 24)) );
      cdma.imsi_m_s1 = nv_commit.ftc_cdma.imsi_s1;
      cdma.imsi_m_s2 = nv_commit.ftc_cdma.imsi_s2;

      /* Update the operational IMSI params in the cdma structure */
      /* if the MIN based IMSI is currently the operational IMSI */
      if (cdma.imsi_type == CDMA_IMSI_M)
      {
        MSG_MED("New IMSI_M is now the operational IMSI",0,0,0);
        cdma.imsi_s1 = cdma.imsi_m_s1;
        cdma.imsi_s2 = cdma.imsi_m_s2;
        qw_equ( cdma.imsi_s, cdma.imsi_m_s);
        cdma.imsi_11_12 = cdma.imsi_m_11_12;
        cdma.mcc = cdma.imsi_m_mcc;
        cdma.imsi_addr_num = cdma.imsi_m_addr_num;
      }

      /* Save other related info */
      cdma.accolc = nv_commit.ftc_cdma.accolc;
      cdma.mob_term_home = nv_commit.ftc_cdma.mob_term_home;
      cdma.mob_term_for_sid = nv_commit.ftc_cdma.mob_term_for_sid;
      cdma.mob_term_for_nid = nv_commit.ftc_cdma.mob_term_for_nid;

      for (i = 0; i < OTASPI_SID_NID_MAX ; i++)
      {
         cdma.pair[i].sid = nv_commit.ftc_cdma.pair[i].sid;
         cdma.pair[i].nid = nv_commit.ftc_cdma.pair[i].nid;
      }

      nv_commit.commit_ftc_cdma = FALSE;
      MSG_MED("CDMA NAM info committed",0,0,0);
    }

    if (nv_commit.commit_roam_list)
    {
      nv_commit.commit_roam_list = FALSE;
      MSG_MED("Roaming List committed",0,0,0);

      #if defined(FEATURE_OTASP) && !defined(FEATURE_NSOTASP) && !defined(FEATURE_MEM_REDUCE_PRL)
      mcc_roaming_list = nv_pr_list;
      /* update the contents of the buffer */
      #endif /* FEATURE_OTASP && !FEATURE_NSOTASP && !FEATURE_MEM_REDUCE_PRL */

      /* Remember dimensions of roaming list in case they are requested */
      otasp_get_rl_stats( &otasp_roam_size );
    }

    if (nv_commit.commit_spc)
    {
      nv_commit.commit_spc = FALSE;
      MSG_MED("SPC committed",0,0,0);

      /* Load the SPC into CDMA structure  */
      (void) memcpy (cdma.spc, (byte*) nv_commit.spc.digits, NV_SEC_CODE_SIZE);
    }

    if (nv_commit.commit_imsi_t)
    {
      nv_commit.commit_imsi_t = FALSE;
      MSG_MED("IMSI_T info committed",0,0,0);

      /* Save True IMSI parameters in the cdma_ms structure */
      cdma.imsi_t_s1 =  nv_commit.imsi_t.s1;
      cdma.imsi_t_s2 =  nv_commit.imsi_t.s2;
      qw_set(cdma.imsi_t_s, (dword)((cdma.imsi_t_s2 >> 8) & 0x03),
                  (((cdma.imsi_t_s2 & 0x00ff) << 24) | cdma.imsi_t_s1));

      cdma.imsi_t_11_12 =  nv_commit.imsi_t.imsi_t_11_12;
      cdma.imsi_t_mcc = nv_commit.imsi_t.mcc;
      cdma.imsi_t_addr_num = nv_commit.imsi_t.addr_num;

      /* Update the operational IMSI params in the cdma structure */
      /* if the True IMSI is currently the operational IMSI */
      if (cdma.imsi_type == CDMA_IMSI_T)
      {
        MSG_MED("New IMSI_T is now the operational IMSI",0,0,0);
        cdma.imsi_s1 = cdma.imsi_t_s1;
        cdma.imsi_s2 = cdma.imsi_t_s2;
        qw_equ(cdma.imsi_s, cdma.imsi_t_s);
        cdma.imsi_11_12 = cdma.imsi_t_11_12;
        cdma.mcc = cdma.imsi_t_mcc;
        cdma.imsi_addr_num = cdma.imsi_t_addr_num;
      }
    } /* end imsi_t commit check */

#ifdef FEATURE_OTASP_OTAPA
    if (nv_commit.commit_nam_lock)
    {
       nv_commit.commit_nam_lock = FALSE;
       MSG_MED("NAM_LOCK committed", 0,0,0);
       /* Load the NAM_LOCK into CDMA structure */
       cdma.nam_lock = nv_commit.nam_lock.enabled;
    }
#endif /* FEATURE_OTASP_OTAPA */

} /* otasp_reload_nam_params */
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================
FUNCTION OTASP_PROCESS_RUIM_REPORT

DESCRIPTION
   This function processes the report received from UIM task for OTASP
   related messages.

DEPENDENCIES
  None.

RETURN VALUE
  The next CDMA sub state.

SIDE EFFECTS
  None.

===========================================================================*/
word otasp_process_ruim_report
(
   uim_rpt_type* otasp_ruim_rpt,
   /* Pointer to auth report type */
   word next_state
)
{
   byte i,x,y,z;
   byte rsp_index;
   word pos = 0;
   mccccim_event_type mccccim_evt;
    /* Event rpt struct for sending primitives to CCIM */

   /* Index of the message whose response is being currently processed */
   rsp_index = otasp_ruim_table.rsp_index;

   /* Number of blocks responded by the card */
   x = otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card;

   /* Number of blocks requested from the card */
   y = otasp_ruim_table.msg[rsp_index].num_blocks_req_card;

   /* Number of blocks requested in OTA message */
   z = otasp_ruim_table.msg[rsp_index].num_blocks_req_msg;

   switch(otasp_ruim_rpt->rpt_type)
   {
      case UIM_COMMIT_R:
      {

         MSG_MED(" Received UIM_COMMIT_R", 0,0,0);

         /* If we were not committing anything and we got this bcos
            of an error, we ignore it */

         if (!otasp_commit_in_progress)
         {
           return(next_state);
         }

         /* Check for the status from the UIM */
         if (otasp_ruim_rpt->rpt_status == UIM_PASS)
         {
             otasp_int.commit_rsp.result_code =
                               otasp_ruim_rpt->rpt.commit.result;

             if (otasp_int.commit_rsp.result_code == OTASP_BLK_ACCEPT)
             {
                /* Set flag to indicate that something has been committed */
                otasp_successful_commit = TRUE;

                #ifdef FEATURE_OTASP_STATUS
                otasp_report_status_to_cm(CM_OTASP_STATUS_COMMITTED);
                #endif /* FEATURE_OTASP_STATUS */
             }

             /* If the roaming list was committed successfully in the card */
             if ((commit.commit_roam_list) &&
                   (otasp_ruim_rpt->rpt.commit.result) == OTASP_BLK_ACCEPT)
             {
                if (!mcc_pcs_band_is_supported()) 
                {
                 /* For 800 MHz CDMA phones, we enable the roaming list and if
                    validation fails, it would be set back to FALSE, while
                    reading the PRL */
                  otasp_nv_item.prl_enabled.nam = cdma.curr_nam;
                  otasp_nv_item.prl_enabled.enabled = TRUE;

                  /* Write to NV that the list should be enabled */
                  otasp_nv_buf.item = NV_PRL_ENABLED_I;
                  otasp_nv_buf.cmd = NV_WRITE_F;
                  otasp_nv_buf.data_ptr = &otasp_nv_item;

                  (void) mcc_nv_cmd( &otasp_nv_buf);
                }

                if (mcc_cellular_band_is_supported() &&
                    mcc_pcs_band_is_supported()) 
                {
                  otasp_nv_item.pref_mode.nam = cdma.curr_nam;
                  otasp_nv_item.pref_mode.mode = NV_MODE_AUTOMATIC;

                  /* Write to NV that the mode should be reset to Automatic */
                  otasp_nv_buf.item = NV_PREF_MODE_I;
                  otasp_nv_buf.cmd = NV_WRITE_F;
                  otasp_nv_buf.data_ptr = &otasp_nv_item;

                  (void) mcc_nv_cmd( &otasp_nv_buf);

                  commit.commit_roam_list = FALSE;
                }
                /* Read the PRL into the buffer */
                otasp_nv_buf.cmd = NV_READ_F;
                otasp_nv_buf.data_ptr   = (nv_item_type*) &nv_pr_list;
                otasp_nv_buf.item       = NV_ROAMING_LIST_683_I;
                (void) mcc_nv_cmd( &otasp_nv_buf);

                /* Update the PRL dimensions structure */
                otasp_get_rl_stats( &otasp_roam_size );
             }
         }
         else
         {
             /* Reject the Commit Request since the R-UIM failed */
             otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
         }

         /* Send Commit Response Message */
         send_commit_resp_msg();

         /* Notify CM that otasp commit has finished */
         otasp_report_commit_status_to_cm(FALSE);

         /* Reset the Commit flag */
         otasp_commit_in_progress = FALSE;

         break;
      }

      case UIM_MS_KEY_REQ_R:
      {

         MSG_MED(" received UIM_MS_KEY_REQ_R", 0,0,0);

         if (otasp_ruim_rpt->rpt_status == UIM_PASS)
         {
             otasp_int.ms_key_rsp.result_code =
                                otasp_ruim_rpt->rpt.ms_key.result;
         }
         else
         {
             otasp_int.ms_key_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
         }

         /* Send MS KEY Response Message */
         send_ms_key_resp_msg();

         break;
      }

      case UIM_KEY_GEN_REQ_R:
      {

         MSG_MED(" received UIM_KEY_GEN_REQ_R", 0,0,0);

         if (otasp_ruim_rpt->rpt_status == UIM_PASS)
         {
             otasp_int.key_gen_rsp.result_code =
                    otasp_ruim_rpt->rpt.key_gen.result;
             otasp_int.key_gen_rsp.ms_result_len =
                    otasp_ruim_rpt->rpt.key_gen.ms_result_len ;

             pos = 0;

#ifdef FEATURE_AUTH
             /* Flag that the R-UIM has generated a new A-Key */
             a_key_temp_valid = TRUE;
#endif
             for (i = 0 ; i < (otasp_ruim_rpt->rpt.key_gen.ms_result_len
                                                 /sizeof(dword)); i++)
             {
                otasp_int.key_gen_rsp.ms_result[i] =
                     b_unpackd( otasp_ruim_rpt->rpt.key_gen.ms_result, pos,
                                         8* sizeof(dword));
                pos += (8 * sizeof(dword)) ;
             }

             #ifdef FEATURE_OTASP_STATUS
             /* Send OTASP status to CM */
             if(otasp_ruim_rpt->rpt.key_gen.result == OTASP_BLK_ACCEPT)
             {
               otasp_report_status_to_cm(CM_OTASP_STATUS_AKEY_EXCHANGED);
             }
             #endif /* FEATURE_OTASP_STATUS */
         }
         else
         {
             otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
             otasp_int.key_gen_rsp.ms_result_len = 0;
         }

         /* Send the Key Generation Response Message */
         send_key_gen_resp_msg();

         break;
      }

      case UIM_OTAPA_REQ_R:
      {

         MSG_MED(" received UIM_OTAPA_REQ_R", 0,0,0);

         if (otasp_ruim_rpt->rpt_status == UIM_PASS)
         {
             otasp_int.otapa_rsp.result_code = otasp_ruim_rpt->rpt.otapa.result;

             #ifdef FEATURE_OTASP_STATUS
             /* Send OTASP status to CM */
             if(cdma.otasp_call == NO_SRV_PROV)
             {
               /* Indicates that OTAPA request message had start_stop value 0 */
               otasp_report_status_to_cm(CM_OTASP_STATUS_OTAPA_STOPPED);
             }
             #endif /* FEATURE_OTASP_STATUS */

             /* Since this variable was set when we began processing the req,
                we change this back if the result was not successful
                Also this would not have been set to Network initiated if this
                was an OTASP Call and hence there is no fear of resetting this
                variable */

             if (otasp_int.otapa_rsp.result_code != OTASP_BLK_ACCEPT)
             {
                if (cdma.otasp_call == NETWORK_INIT_SRV_PROV)
                {
                   cdma.otasp_call = NO_SRV_PROV;
                }

                /* Flag indicates of an OTAPA session has not occurred */
                otasp_otapa_session_occurred = FALSE;
             }

             #ifdef FEATURE_OTASP_STATUS
             /* Send OTASP status to CM */
             if(cdma.otasp_call == NETWORK_INIT_SRV_PROV)
             {
               otasp_report_status_to_cm(CM_OTASP_STATUS_OTAPA_STARTED);
             }
             #endif /* FEATURE_OTASP_STATUS */

             otasp_int.otapa_rsp.nam_lock_ind =
                       otasp_ruim_rpt->rpt.otapa.nam_lock_ind;

             if (otasp_int.otapa_rsp.nam_lock_ind)
             {
                otasp_int.otapa_rsp.rand_otapa =
                                  otasp_ruim_rpt->rpt.otapa.rand_otapa;
             }
         }
         else
         {
             otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
             otasp_int.otapa_rsp.nam_lock_ind = FALSE;
             if (cdma.otasp_call == NETWORK_INIT_SRV_PROV)
             {
                 cdma.otasp_call = NO_SRV_PROV;
             }

             /* Flag indicates of an OTAPA session has not occurred */
             otasp_otapa_session_occurred = FALSE;
         }
         /* Send OTAPA response message */
         send_otapa_resp_msg();

         break;
      }

      case UIM_VALIDATE_R:
      {

         MSG_MED("Received UIM_VALIDATE_R", 0,0,0);

         if (otasp_ruim_table.msg[rsp_index].msg_type ==
                                                  OTASP_VALIDATION_REQ_MSG)
         {
            /* fill data from the response into the otaspi structure */
            if (otasp_ruim_rpt->rpt_status == UIM_PASS)
            {
               /* Fill in the ID of the block */
               ruim_otasp_int.validn_rsp.block[x].block_id =
                      otasp_ruim_rpt->rpt.validate.block_id;

               /* Fill in the result of the block */
               ruim_otasp_int.validn_rsp.block[x].result_code =
                      otasp_ruim_rpt->rpt.validate.result;

               if (((otasp_ruim_rpt->rpt.validate.block_id==OTASP_VERIFY_SPC_BLOCK) 
                    && (otasp_ruim_rpt->rpt.validate.result==OTASP_BLK_REJ_SPC)))
               {
                 if (cdma.otasp_call == USER_INIT_SRV_PROV)
                 {
                   /* OTASP call, power down if too many failed attempts */
                   if (++otasp_failed_unlock_attempts ==
                                               OTASP_MAX_UNLOCK_ATTEMPTS)
                   {
                     ERR("RUIM: %d SPC validation failures, terminate OTASP",
                               otasp_failed_unlock_attempts,0,0);

                     #ifdef FEATURE_OTASP_STATUS
                     /* Send OTASP status to CM */
                     otasp_report_status_to_cm(CM_OTASP_STATUS_SPC_RETRIES_EXCEEDED);
                     #endif /* FEATURE_OTASP_STATUS */

                     /* Send Notification to MCCCCIM to release all CC instances */
                     mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
                     /* All CC instances should already be in release substate */
                     mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_REL_OTASP_ERR;
                     mccccim_process_event( &mccccim_evt );
                     /* No need to check return status in this case */

                     next_state = CDMA_RELEASE;  /*lint !e641 */
                     cdma.rel_orig = FALSE;
                     cdma.otasp_call = NO_SRV_PROV;

                     /* Events report: Release */
                     /* Treat it as BS originated because upper layers are not aware of it */
                     mclog_report_release(MCLOG_RELEASE_BS_ORDER);
                   }
                 } /* USER_INIT_SRV_PROV */
#ifdef FEATURE_OTASP_OTAPA
                 else
                 {
                   /* OTAPA call, don't power down */
                   if (++otasp_otapa_failed_spc_val_attempts ==
                                              OTASP_MAX_UNLOCK_ATTEMPTS)
                   {
                     /* We don't want to powerdown if OTAPA session occurs
                      ** in the middle of a conversation. */
                     cdma.otasp_call = NO_SRV_PROV;
                   }
                 } /* OTAPA call */
#endif /* FEATURE_OTASP_OTAPA */ 
               }

#ifdef FEATURE_OTASP_OTAPA
               if (((otasp_ruim_rpt->rpt.validate.block_id==VALIDATE_SPASM)
                    && (otasp_ruim_rpt->rpt.validate.result==OTASP_BLK_REJ_SPASM)))
               {
                 if (cdma.otasp_call == NETWORK_INIT_SRV_PROV)
                 {
                   /* OTAPA call, don't power down */
                   if (++otasp_otapa_failed_spasm_val_attempts ==
                                              OTASP_MAX_UNLOCK_ATTEMPTS)
                   {
                     /* We don't want to powerdown if OTAPA session occurs
                      ** in the middle of a conversation. */
                     ERR("RUIM: %d SPASM validation failures, terminate OTAPA",
                               otasp_otapa_failed_spasm_val_attempts,0,0);
                     cdma.otasp_call = NO_SRV_PROV;
                   }
                 } /* OTAPA call */
               }
#endif /* FEATURE_OTASP_OTAPA */ 
            } /* otasp_ruim_rpt->rpt_status == UIM_PASS */
            else
            {
               ruim_otasp_int.validn_rsp.block[x].block_id =
                        otasp_ruim_rpt->rpt.validate.block_id;

               ruim_otasp_int.validn_rsp.block[x].result_code =
                        OTASP_BLK_REJ_UNKNOWN;
            }

            /* Increment the number of blocks responded */
            ++ (otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card);

            /* Reassign it to the same variable */
            x = otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card;

            /* The below check is to see if we have recd responses to all
              the blocks sent to the card for that message */
            if (y == x )
            {
               /* Check to see if there are any blocks that were not sent
                  to the card */
               if ( y != z)
               {
                  /* We did not send (z-y) the blocks to the card */
                  for (i = 0; i < (z-y); i++)
                  {
                     /* Get the block id from the rejected block id corr
                       to the message */
                     ruim_otasp_int.validn_rsp.block[x+i].block_id =
                         otasp_ruim_table.msg[rsp_index].rej_block_id[i];

                     /* Reject the block */
                     ruim_otasp_int.validn_rsp.block[x+i].result_code =
                                OTASP_BLK_REJ_UNKNOWN;
                  }
               }

               /* Set the number of blocks in the response message */
               ruim_otasp_int.validn_rsp.num_blocks = otasp_ruim_table.
                               msg[rsp_index].num_blocks_req_msg;

               /* send the validation response message */
               send_validn_resp_msg();

               /* Clean up the buffer */
               for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
               {
                  otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
               }

               otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

               otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

               otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

               otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

               /* consider this message as done and increment the response
                 index */
               otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                                OTASP_RUIM_MAX_MSG_BUFS;
            }
         }
         else
         {
             /* This is a pathological case here and wish this did not happen */
             ERR("OTASP RUIM req-rsp in incorrect order",0,0,0);

             /*  !!!!!!! CODE THIS CASE LATER !!!!!!! */
         }

         break;
      }

      case UIM_CONFIG_REQ_R:
      {
         MSG_MED(" received UIM_CONFIG_REQ_R", 0,0,0);

         if (otasp_ruim_table.msg[rsp_index].msg_type == OTASP_CONFIG_REQ_MSG)
         {
             /* fill data from the response into the otaspi structure */
             if (otasp_ruim_rpt->rpt_status == UIM_PASS)
             {
                /* Fill in the ID of the block */
                ruim_otasp_int.config_rsp.block[x].block_id =
                      otasp_ruim_rpt->rpt.configuration.block_id;

                /* Fill in the result of the block */
                ruim_otasp_int.config_rsp.result_code[x] =
                      otasp_ruim_rpt->rpt.configuration.result;

                /* Fill in the length of the block */
                ruim_otasp_int.config_rsp.block[x].block_len =
                      otasp_ruim_rpt->rpt.configuration.block_length;

                /* Fill in the data from the response */
                memcpy(ruim_otasp_int.config_rsp.block[x].blk.ruim_rsp_data,
                       otasp_ruim_rpt->rpt.configuration.data,
                       otasp_ruim_rpt->rpt.configuration.block_length);

                /* There are a few ME specific info that needs to be filled in
                   the Configuration Response Message */
                switch (otasp_ruim_rpt->rpt.configuration.block_id)
                {
                    case OTASP_CDMA_ANALOG_NAM_BLK:

                        /* Pack SCM */
                        b_packb( cdma.scm,ruim_otasp_int.config_rsp.block[x].
                                 blk.ruim_rsp_data, 27,
                                 FSIZ( otaspe_rtc_cell_data_fix_type, scm ));

                        /* Pack mobile station protocol revision number */
                        b_packb( cdma.mob_cai_rev,ruim_otasp_int.config_rsp.
                                 block[x].blk.ruim_rsp_data, 35 ,
                                 FSIZ(otaspe_rtc_cell_data_fix_type,mob_p_rev));

                        /* Pack the local control as 0 since we don't support*/
                        b_packb( 0 ,ruim_otasp_int.config_rsp.
                                 block[x].blk.ruim_rsp_data, 102,
                                 FSIZ( otaspe_rtc_cell_data_fix_type,
                                       local_control ));
                    break;

                    case OTASP_CDMA_NAM_BLK:

                        /* Pack Slotted mode as TRUE */
                        b_packb( 1,ruim_otasp_int.config_rsp.
                                 block[x].blk.ruim_rsp_data, 2 ,
                                 FSIZ(otaspe_rtc_pcs_data_fix_type,
                                      slotted_mode));

                        /* Pack mobile station protocol revision number */
                        b_packb( cdma.mob_cai_rev,ruim_otasp_int.config_rsp.
                                 block[x].blk.ruim_rsp_data, 8 ,
                                 FSIZ(otaspe_rtc_pcs_data_fix_type,mob_p_rev));

                        /* Pack the local control as 0 since we don't support*/
                        b_packb( 0 ,ruim_otasp_int.config_rsp.
                                 block[x].blk.ruim_rsp_data, 75,
                                 FSIZ( otaspe_rtc_pcs_data_fix_type,
                                       local_control ));
                    break;
                } /* end of switch */  /*lint !e744*/
             }
             else
             {
                ruim_otasp_int.config_rsp.block[x].block_id =
                      otasp_ruim_rpt->rpt.configuration.block_id;

                ruim_otasp_int.config_rsp.result_code[x]= OTASP_BLK_REJ_UNKNOWN;

                ruim_otasp_int.config_rsp.block[x].block_len = 0;
             }

             /* Increment the number of blocks responded */
             ++ (otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card);

             /* Reassign it to the same variable */
             x = otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card;

             /* The below check is to see if we have recd responses to all
                the blocks sent to the card for that message */
             if (y == x )
             {
                /* Check to see if there are any blocks that were not sent
                    to the card */
                if ( y != z)
                {
                    /* We did not send (z-y) the blocks to the card */
                    for (i = 0; i < (z-y); i++)
                    {
                        /* Get the block id from the rejected block id corr
                           to the message */
                        ruim_otasp_int.config_rsp.block[x+i].block_id =
                             otasp_ruim_table.msg[rsp_index].rej_block_id[i];

                        /* Reject the block */
                        ruim_otasp_int.config_rsp.result_code[x+i] =
                                    OTASP_BLK_REJ_UNKNOWN;

                        /* Set the block length to zero */
                        ruim_otasp_int.config_rsp.block[x+i].block_len = 0;
                    }
                }

                /* Set the number of blocks in the response message */
                ruim_otasp_int.config_rsp.num_blocks = otasp_ruim_table.
                                   msg[rsp_index].num_blocks_req_msg;

                /* send the Configuration Response message */
                send_cfg_resp_msg();

                /* Clean up the buffer */
                for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
                {
                    otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
                }

                otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

                otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

                otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

                otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

                /* consider this message as done and increment the response
                   index */
                otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                                  OTASP_RUIM_MAX_MSG_BUFS;
             }
         }
         else
         {
             /* This is a pathological case here and wish this did not happen */
             ERR("OTASP RUIM req-rsp in incorrect order",0,0,0);

             /*  !!!!!!! CODE THIS CASE LATER */
         }

         break;
      }

      case UIM_DOWNLOAD_REQ_R:
      {

         MSG_MED(" received UIM_DOWNLOAD_REQ_R", 0,0,0);

         if (otasp_ruim_table.msg[rsp_index].msg_type == OTASP_DOWNLOAD_REQ_MSG)
         {
             /* fill data from the response into the otaspi structure */
             if (otasp_ruim_rpt->rpt_status == UIM_PASS)
             {
                if ((otasp_ruim_rpt->rpt.download.block_id == OTASP_VERIFY_SPC_BLK) &&
                    (otasp_ruim_rpt->rpt.download.result == OTASP_BLK_REJ_BAD_SPC))
                {
                  if (++otasp_failed_unlock_attempts >= OTASP_MAX_UNLOCK_ATTEMPTS)
                  {
                      #ifdef FEATURE_OTASP_STATUS
                      /* Send OTASP status to CM */
                      otasp_report_status_to_cm(CM_OTASP_STATUS_SPC_RETRIES_EXCEEDED);
                      #endif /* FEATURE_OTASP_STATUS */

                      /* Send Notification to MCCCCIM to release all CC instances */
                      mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
                      /* All CC instances should already be in release substate */
                      mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_REL_OTASP_ERR;
                      mccccim_process_event( &mccccim_evt );
                      /* No need to check return status in this case */

                      next_state = CDMA_RELEASE;  /*lint !e641 */
                      cdma.rel_orig = FALSE;
                      cdma.otasp_call = NO_SRV_PROV;
  
                      /* Events report: Release */
                      /* Treat it as BS originated because upper layers are not aware of it */
                      mclog_report_release(MCLOG_RELEASE_BS_ORDER);
                  }
                }
                /* Fill in the ID of the block */
                ruim_otasp_int.dload_rsp.block[x].block_id =
                      otasp_ruim_rpt->rpt.download.block_id;

                /* Fill in the result of the block */
                ruim_otasp_int.dload_rsp.block[x].result_code =
                      otasp_ruim_rpt->rpt.download.result;

               #ifdef FEATURE_OTASP_STATUS
               /* Send OTASP status to CM */
               if(otasp_ruim_rpt->rpt.download.result == OTASP_BLK_ACCEPT)
               {
                 switch(otasp_ruim_rpt->rpt.download.block_id)
                 {
                   case OTASP_MOB_DIR_NUM_BLK: /* Mobile Directory Number */
                     otasp_report_status_to_cm(CM_OTASP_STATUS_MDN_DOWNLOADED);
                   break;

                   case OTASP_CDMA_NAM_BLK: /* PCS Indicator Download Data */
                     otasp_report_status_to_cm(CM_OTASP_STATUS_NAM_DOWNLOADED);
                   break;

                   case OTASP_IMSI_T_BLK: /* IMSI_T */
                     otasp_report_status_to_cm(CM_OTASP_STATUS_IMSI_DOWNLOADED);
                   break;      
                 }
               } /* end if */
               #endif /* FEATURE_OTASP_STATUS */

             } /* if (otasp_ruim_rpt->rpt_status == UIM_PASS) */
             else
             {
                ruim_otasp_int.dload_rsp.block[x].block_id =
                      otasp_ruim_rpt->rpt.download.block_id;

                ruim_otasp_int.dload_rsp.block[x].result_code=
                      OTASP_BLK_REJ_UNKNOWN;
             }

             /* Increment the number of blocks responded */
             ++ (otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card);

             /* Reassign it to the same variable */
             x = otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card;

             /* The below check is to see if we have recd responses to all
                the blocks sent to the card for that message */
             if (y == x )
             {
                /* Check to see if there are any blocks that were not sent
                    to the card */
                if ( y != z)
                {
                    /* We did not send (z-y) the blocks to the card */
                    for (i = 0; i < (z-y); i++)
                    {
                        /* Get the block id from the rejected block id corr
                           to the message */
                        ruim_otasp_int.dload_rsp.block[x+i].block_id =
                             otasp_ruim_table.msg[rsp_index].rej_block_id[i];

                        /* Reject the block */
                        ruim_otasp_int.dload_rsp.block[x+i].result_code =
                             OTASP_BLK_REJ_UNKNOWN;
                    }
                }

                /* Set the number of blocks in the response message */
                ruim_otasp_int.dload_rsp.num_blocks = otasp_ruim_table.
                                   msg[rsp_index].num_blocks_req_msg;

                /* send the Download Response message */
                send_dload_resp_msg();

                /* Clean up the buffer */
                for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
                {
                    otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
                }

                otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

                otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

                otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

                otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

                /* consider this message as done and increment the response
                   index */
                otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                                  OTASP_RUIM_MAX_MSG_BUFS;
             }
         }
         else
         {
             /* This is a pathological case here and wish this did not happen */
             ERR("OTASP RUIM req-rsp in incorrect order",0,0,0);

             /*  !!!!!!! CODE THIS CASE LATER */
         }

         break;
      }

      case UIM_SSPR_CONFIG_REQ_R:
      {
         MSG_MED(" received UIM_SSPR_CONFIG_REQ_R", 0,0,0);

         if (otasp_ruim_table.msg[rsp_index].msg_type ==
                                              OTASP_SSPR_CONFIG_REQ_MSG)
         {
             /* fill data from the response into the otaspi structure */
             if (otasp_ruim_rpt->rpt_status == UIM_PASS)
             {
                /* Fill in the ID of the block */
                ruim_otasp_int.sspr_cfg_rsp.block_id =
                      otasp_ruim_rpt->rpt.sspr_config.block_id;

                /* Fill in the result of the block */
                ruim_otasp_int.sspr_cfg_rsp.result_code =
                      otasp_ruim_rpt->rpt.sspr_config.result;

                /* Fill in the length of the block */
                ruim_otasp_int.sspr_cfg_rsp.block_len =
                      otasp_ruim_rpt->rpt.sspr_config.block_length;

                /* Re-pack NUM_ACQ_RECS and NUM_SYS_RECS fields because card
                 doesn't know 683C PRL and did not fill them in correctly. */
                if ( otasp_ruim_rpt->rpt.sspr_config.block_id ==
                        OTASP_CFG_PRL_DIMENSIONS_BLK )
                {
                  /* Fill NUM_ACQ_RECS and NUM_SYS_RECS fields. See IS-683A
                      3.5.3.1 */

                  /* Initialize the three bytes to zero */
                  otasp_ruim_rpt->rpt.sspr_config.data[PRL_NUM_REC_INDEX] =
                    otasp_ruim_rpt->rpt.sspr_config.data[PRL_NUM_REC_INDEX+1] =
                    otasp_ruim_rpt->rpt.sspr_config.data[PRL_NUM_REC_INDEX+2] =
                    0;

                  /* 7 MSB bits of NUM_ACQ_RECS */
                  otasp_ruim_rpt->rpt.sspr_config.data[PRL_NUM_REC_INDEX] |=
                          ( otasp_roam_size.num_acq_recs >> 2);                /*lint !e734*/
                  /* 2 LSB bits of NUM_ACQ_RECS */
                  otasp_ruim_rpt->rpt.sspr_config.data[PRL_NUM_REC_INDEX+1] |=
                          ( otasp_roam_size.num_acq_recs << 6);                /*lint !e734*/
                  /* 6 MSB bits of NUM_SYS_RECS */
                  otasp_ruim_rpt->rpt.sspr_config.data[PRL_NUM_REC_INDEX+1] |=
                          ( otasp_roam_size.num_sys_recs >> 8);
                  /* 8 LSB bits of NUM_SYS_RECS */
                  otasp_ruim_rpt->rpt.sspr_config.data[PRL_NUM_REC_INDEX+2] =
                          otasp_roam_size.num_sys_recs;                        /*lint !e734*/
                }

                /* Fill in the data from the response */
                memcpy(ruim_otasp_int.sspr_cfg_rsp.block.blk.ruim_rsp_data,
                       otasp_ruim_rpt->rpt.sspr_config.data,
                       otasp_ruim_rpt->rpt.sspr_config.block_length);
             }
             else
             {
                ruim_otasp_int.sspr_cfg_rsp.block_id =
                      otasp_ruim_rpt->rpt.sspr_config.block_id;

                ruim_otasp_int.sspr_cfg_rsp.result_code= OTASP_BLK_REJ_UNKNOWN;

                ruim_otasp_int.sspr_cfg_rsp.block_len = 0;
             }

             /* send the SSPR Configuration Response message */
             send_sspr_cfg_resp_msg();

             /* Clean up the buffer */
             for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
             {
                otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
             }

             otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

             otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

             otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

             otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

             /* consider this message as done and increment the response
                index */
             otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                               OTASP_RUIM_MAX_MSG_BUFS;
         }
         else
         {
             /* This is a pathological case here and wish this did not happen */
             ERR("OTASP RUIM req-rsp in incorrect order",0,0,0);

             /*  !!!!!!! CODE THIS CASE LATER */
         }

         break;
      }

      case UIM_SSPR_DOWNLOAD_REQ_R:
      {
         MSG_MED(" received UIM_SSPR_DOWNLOAD_REQ_R", 0,0,0);

         if (otasp_ruim_table.msg[rsp_index].msg_type ==
                                         OTASP_SSPR_DOWNLOAD_REQ_MSG)
         {
             /* fill data from the response into the otaspi structure */
             if (otasp_ruim_rpt->rpt_status == UIM_PASS)
             {
                /* Fill in the ID of the block */
                ruim_otasp_int.sspr_dload_rsp.block_id =
                      otasp_ruim_rpt->rpt.sspr_download.block_id;

                /* Fill in the result of the block */
                ruim_otasp_int.sspr_dload_rsp.result_code =
                      otasp_ruim_rpt->rpt.sspr_download.result;

                /* Fill in the segment offset in the response  */
                ruim_otasp_int.sspr_dload_rsp.segment_offset =
                      otasp_ruim_rpt->rpt.sspr_download.segment_offset;

                /* Fill in the length of the block */
                ruim_otasp_int.sspr_dload_rsp.segment_size =
                      otasp_ruim_rpt->rpt.sspr_download.segment_size;

                /* If the response to the last segment was accepted, then
                   we would be committing the PRL when a commit is requested */
                if ((otasp_ruim_last_segment) &&
                    (otasp_ruim_rpt->rpt.sspr_download.result == OTASP_BLK_ACCEPT))
                {
                    commit.commit_roam_list = TRUE;

                    #ifdef FEATURE_OTASP_STATUS
                    /* Send OTASP status to CM */
                    otasp_report_status_to_cm(CM_OTASP_STATUS_PRL_DOWNLOADED);
                    #endif /* FEATURE_OTASP_STATUS */
                }
             }
             else
             {
                ruim_otasp_int.sspr_dload_rsp.block_id =
                      otasp_ruim_rpt->rpt.sspr_download.block_id;

                ruim_otasp_int.sspr_dload_rsp.result_code=OTASP_BLK_REJ_UNKNOWN;

                ruim_otasp_int.sspr_dload_rsp.segment_offset = 0;

                ruim_otasp_int.sspr_dload_rsp.segment_size = 0;

                commit.commit_roam_list = FALSE;
             }

             /* send the SSPR Download Response message */
             send_sspr_dload_resp_msg();

             /* Clean up the buffer */
             for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
             {
                otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
             }

             otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

             otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

             otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

             otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

             /* consider this message as done and increment the response
                index */
             otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                               OTASP_RUIM_MAX_MSG_BUFS;
         }
         else
         {
             /* This is a pathological case here and wish this did not happen */
             ERR("OTASP RUIM req-rsp in incorrect order",0,0,0);

             /*  !!!!!!! CODE THIS CASE LATER */
         }

         break;
      }

      default:
      {
         /* we got the wrong report */
         ERR( "Wrong UIM report %d!", otasp_ruim_rpt->rpt_type, 0, 0 );

         break;
      }
   } /* end of switch */

   /* Place the report buffer back on the free queue. */
   cmd_done( &otasp_ruim_rpt->rpt_hdr );

   return(next_state);

} /* otasp_process_ruim_report */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION SEND_OTAPA_RESP_MSG

DESCRIPTION
   This function sends the OTAPA Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_otapa_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 *//* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the OTAPA Response Message */

  otasp_int.otapa_rsp.msg_type = OTASP_OTAPA_RSP_MSG;

  status = otaspx_int_to_ext( &len, &otasp_int, chari );
  if (status == OTASPX_DONE_S) /*lint !e641 */
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_otapa_resp_msg */

/*===========================================================================

FUNCTION PROCESS_OTAPA_REQ

DESCRIPTION
  This function processes an OTAPA Request Message, formats the
  appropriate OTAPA Response Message, then calls the
  routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  Next state to return. CDMA_CONV or CDMA_EXIT for power down.

SIDE EFFECTS
  None.

===========================================================================*/

word process_otapa_req
(
  otaspi_otapa_req_msg_type* otapa_req_ptr
    /* Pointer to OTAPA Request Message */
)
{
  word next_state = CDMA_TRAFFIC; /*lint !e641 */
#ifdef FEATURE_UIM_RUIM
  /* UIM command pointer */
  uim_cmd_type *uim_cmd_ptr;
  /* Indicates if the OTAPA command is sent to the R-UIM */
  boolean sent_otapa_command = TRUE;
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */ 
#if defined(FEATURE_AUTH) && !defined(FEATURE_NSOTASP) && \
    defined(FEATURE_OTASP_OTAPA)
  auth_cmd_type *auth_cmd_ptr;
#endif/* (FEATURE_AUTH) && !(FEATURE_NSOTASP) &&
         (FEATURE_OTASP_OTAPA) */
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    MSG_MED("process otapa_req_msg",0,0,0);

    if (!otasp_is_otasp_allowed())
    {
      otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
      otasp_int.otapa_rsp.nam_lock_ind = FALSE;
      /* Send OTAPA response message as Rejected */
      send_otapa_resp_msg();
      /* Indicates the the OTAPA command was not sent to the R-UIM */
      sent_otapa_command = FALSE;
      return (next_state);
    }
    else
    {
      /* Initialize OTASP OTAPA parameters */
      otasp_otapa_init();
    }

    if ((cdma.otasp_call != USER_INIT_SRV_PROV) &&
        (otapa_req_ptr->start_stop ))
    {
        /* Set the NAM lock state for an OTAPA Call */
        otasp_set_ruim_nam_lock_state( NETWORK_INIT_SRV_PROV );
    }

    /* The R-UIM card would perform the key exchange operation */
    if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
    {
       /* memset the uim_cmd_ptr to 0's set the values of all fields to 0 */
       memset(uim_cmd_ptr, 0, sizeof(uim_cmd_type));
       uim_cmd_ptr->hdr.command = UIM_OTAPA_REQ_F;

       if (otapa_req_ptr->start_stop)
       {
          uim_cmd_ptr->otapa_req.start_stop = 0x80;
       }
       else
       {
          uim_cmd_ptr->otapa_req.start_stop = 0;
       }

       uim_cmd_ptr->otapa_req.randseed = ran_next();
       #ifdef FEATURE_CSIM
#error code not present
       #endif /* FEATURE_CSIM */
       uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

       mcc_uim_cmd( uim_cmd_ptr );

       if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
       {
           ERR( "Bad status from UIM %d", uim_cmd_ptr->hdr.status,0,0);
           otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
           otasp_int.otapa_rsp.nam_lock_ind = FALSE;
           /* Send OTAPA response message as Rejected */
           send_otapa_resp_msg();
           /* Indicates the the OTAPA command was not sent to the R-UIM */
           sent_otapa_command = FALSE;
       }
    }
    else
    {
       ERR( "No free buffers on uim_free_q", 0, 0, 0 );
       otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
       otasp_int.otapa_rsp.nam_lock_ind = FALSE;

       /* Send OTAPA response message as Rejected */
       send_otapa_resp_msg();

       /* Indicates the the OTAPA command was not sent to the R-UIM */
       sent_otapa_command = FALSE;
    }

    /* If this is an user originated call, then this request would be rejected by
       the card, so we do not want to set the cdma OTASP state variable.
       Since we do not know if the card is going to reject this request, we
       will again change the state if required, upon receiving the report
       from R-UIM */

    /* If the OTAPA command was sent to the R-UIM */
    if (sent_otapa_command)
    {
      if (cdma.otasp_call != USER_INIT_SRV_PROV)
      {
        /* if the start stop bit is set, then we are starting an OTAPA session */
        if (otapa_req_ptr->start_stop)
        {
           cdma.otasp_call = NETWORK_INIT_SRV_PROV;

           /* Flag indicates of an OTAPA session has occurred */
           otasp_otapa_session_occurred = TRUE;
        }
        else if (!otapa_req_ptr->start_stop)
        {
           cdma.otasp_call = NO_SRV_PROV;
        }
      }
    }
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    MSG_MED("process otapa_req_msg",0,0,0);
    otasp_int.otapa_rsp.msg_type = OTASP_OTAPA_RSP_MSG;

#if defined (FEATURE_NSOTASP) || !defined(FEATURE_OTASP_OTAPA)
  /* mobile station does not support OTAPA */
    ERR("OTAPA is not supported",0,0,0);
    otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_MODE;
    otasp_int.otapa_rsp.nam_lock_ind = FALSE;
#else /* !FEATURE_NSOTASP && FEATURE_OTASP_OTAPA */
  /* mobile station supports OTAPA */

    if ((cdma.otasp_call == USER_INIT_SRV_PROV)
        || !cdma.otapa_enabled) /* OTAPA is disabled for this NAM */
    {
      ERR("otapa not supported in this mode",0,0,0);
      otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_MODE;
      otasp_int.otapa_rsp.nam_lock_ind = FALSE;
    }
    /* If too many SPC/SPASM validation failure, reject. */
    else if (!otasp_is_otasp_allowed())
    {
      otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
      otasp_int.otapa_rsp.nam_lock_ind = FALSE;
    }
    else //it's OK
    {
      if (otapa_req_ptr->start_stop == FALSE) /* stop OTAPA session */
      {
        MSG_HIGH("Stop OTAPA session", 0,0,0);
        otasp_int.otapa_rsp.result_code = OTASP_BLK_ACCEPT;
        otasp_int.otapa_rsp.nam_lock_ind = FALSE;
        cdma.otasp_call = NO_SRV_PROV;

        #ifdef FEATURE_OTASP_STATUS
        /* Send OTASP status to CM */
        otasp_report_status_to_cm(CM_OTASP_STATUS_OTAPA_STOPPED);
        #endif /* FEATURE_OTASP_STATUS */
      }
      else if (otapa_req_ptr->start_stop == TRUE) /* start OTAPA session */
      {
        MSG_HIGH("Start OTAPA session",0,0,0);
        cdma.otasp_call = NETWORK_INIT_SRV_PROV;
        otasp_otapa_init();

        #ifdef FEATURE_OTASP_STATUS
        /* Send OTASP status to CM */
        otasp_report_status_to_cm(CM_OTASP_STATUS_OTAPA_STARTED);
        #endif /* FEATURE_OTASP_STATUS */

        /* Flag indicates of an OTAPA session has occurred */
        otasp_otapa_session_occurred = TRUE;

        if (nam_locked_state)
        {
#ifdef FEATURE_AUTH
          otasp_int.otapa_rsp.result_code = OTASP_BLK_ACCEPT;
          otasp_int.otapa_rsp.nam_lock_ind = TRUE;
          rand_otapa = ran_next();
          otasp_int.otapa_rsp.rand_otapa = rand_otapa;
          MSG_MED("Include rand_otapa: %lx", rand_otapa,0,0);

          if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
          {
            /* Send command to AUTH task to calculate auth_otapa */
            otasp_auth_otapa_in_progress = TRUE;
            auth_cmd_ptr->hdr.command = AUTH_SIGNATURE_SPASM_F;
            auth_cmd_ptr->sig.rand_chal = rand_otapa;

            /* Always use IMSI_S1 as the input. see IS-683A 3.3.7 */
            MSG_MED("Use IMSI_S1 as input",0,0,0);
            auth_cmd_ptr->sig.auth_data = mcc_get_auth_imsi_s1();

            auth_cmd_ptr->sig.save_reg = FALSE;
            auth_cmd_ptr->sig.rpt_function = mcc_queue_auth_rpt;

            mcc_auth_cmd( auth_cmd_ptr );

            if (auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S)
            {
              /* -----------------------------------------------
              ** We have a serious problem and can not calculate
              ** auth_otapa, so error fatal.
              ** ----------------------------------------------- */
              ERR_FATAL( "Bad status from AUTH %d", auth_cmd_ptr->hdr.status,0,0);
            }
          }
          else
          {
            ERR_FATAL( "No free buffers on auth_free_q", 0, 0, 0 );
          }
#else
          /* FEATURE_AUTH is not defined. Reject if nam_locked_stated is true */
          otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_MODE;
          otasp_int.otapa_rsp.nam_lock_ind = FALSE;
#endif /* FEATURE_AUTH */
        }
        else /* nam_locked_state == FALSE */
        {
          otasp_int.otapa_rsp.result_code = OTASP_BLK_ACCEPT;
          otasp_int.otapa_rsp.nam_lock_ind = FALSE;
        }
      }
      else /* invalid value for START_STOP, translation error */
      {
        ERR_FATAL( "invalid stop/start value: %d", otapa_req_ptr->start_stop,0,0);
      }
    }
#endif /* defined (FEATURE_NSOTASP) || !defined(FEATURE_OTASP_OTAPA) */

    /* Translate and send the OTAPA Response Message */

    status = otaspx_int_to_ext( &len, &otasp_int, chari );

    if (status == OTASPX_DONE_S) /*lint !e641 */
    {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
    }
    else
    {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
  return (next_state);

} /* process_otapa_req */

#ifdef FEATURE_OTASP_OTAPA
/*===========================================================================

FUNCTION OTASP_GET_OTAPA_SESSION

DESCRIPTION
  This function returns whether or not an OTAPA session has taken place

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an OTAPA session has taken place
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean otasp_get_otapa_session (void)
{
  MSG_MED("otapa_session_occurred: %d", otasp_otapa_session_occurred,0,0);
  return otasp_otapa_session_occurred;
}

#endif /* FEATURE_OTASP_OTAPA */

/*===========================================================================

FUNCTION OTASP_REPORT_COMMIT_STATUS_TO_CM

DESCRIPTION
  The function sends a commit status command to CM

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void otasp_report_commit_status_to_cm
(
  boolean commit_in_progress
    /* Flag if an OTAPA session is in progress */
)
{
  cm_mc_rpt_type *cm_ptr;
    /* pointer to command to send to CM */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("send commit in prog %d to CM", commit_in_progress,0,0);
  if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)  {
    cm_ptr->otasp_commit_status.hdr.cmd = CM_OTASP_COMMIT_STATUS_F;
    cm_ptr->otasp_commit_status.commit_in_progress = commit_in_progress;
    mcc_cm_mc_rpt( cm_ptr );
  }
  else
  {
    ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
  }
} /* otasp_report_commit_status_to_cm() */

#ifdef FEATURE_OTASP_STATUS
/*===========================================================================

FUNCTION OTASP_REPORT_STATUS_TO_CM

DESCRIPTION
  The function sends the otasp status command to CM

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void otasp_report_status_to_cm
(
  cm_otasp_status_e_type otasp_status
    /* OTASP state */
)
{
  cm_mc_rpt_type *cm_ptr;
    /* pointer to command to send to CM */
  boolean send_status = FALSE;
    /* flag to decide when to report status */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(otasp_status)
  {
    /* Status values to be sent only in case of user initiated session */
    case CM_OTASP_STATUS_SPL_UNLOCKED:
    case CM_OTASP_STATUS_SPC_RETRIES_EXCEEDED:
    case CM_OTASP_STATUS_AKEY_EXCHANGED:
    case CM_OTASP_STATUS_NAM_DOWNLOADED:
    case CM_OTASP_STATUS_MDN_DOWNLOADED:
    case CM_OTASP_STATUS_IMSI_DOWNLOADED:
    case CM_OTASP_STATUS_PRL_DOWNLOADED:
    case CM_OTASP_STATUS_COMMITTED:
    {
      if(cdma.otasp_call == USER_INIT_SRV_PROV)
      {
         send_status = TRUE;
      }
      break;
    }
    /* Status values to be sent only in case of network initiated session */
    case CM_OTASP_STATUS_OTAPA_STARTED:
    {
      if(cdma.otasp_call == NETWORK_INIT_SRV_PROV)
      {
         send_status = TRUE;
      }
      break;
    }
    /* Status values to be sent only in case of network initiated session.
       For STOP and ABORT cases, cdma.otasp_call will be NO_SRV_PROV, so this
       variable can not be used. It is assumed that the status will not be set
       to any of the following for a user initiated otasp session. */
    case CM_OTASP_STATUS_OTAPA_STOPPED:
    /* case CM_OTASP_STATUS_OTAPA_ABORTED:
       This status will be sent from CM itself to upper layers */
    {
      send_status = TRUE;
      break;
    }
    /* status values to be sent for both user and network initiated session */
    case CM_OTASP_STATUS_SSD_UPDATED:
    {
      send_status = TRUE;
      break;
    }
    default:
    {
      MSG_ERROR("Invalid otasp status %d", otasp_status, 0, 0);
      break;
    }
  } /* end switch */

  if(send_status)
  {
    MSG_MED("sent OTASP status %d to CM", otasp_status, 0, 0);
    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cm_ptr->otasp_status.hdr.cmd = CM_OTASP_STATUS_F;
      cm_ptr->otasp_status.otasp_code = otasp_status;
      mcc_cm_mc_rpt( cm_ptr );
    }
    else
    {
      ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
    }
  } /* end if */

} /* otasp_report_status_to_cm() */
#endif /* FEATURE_OTASP_STATUS */

/*===========================================================================

FUNCTION OTASP_IS_SUCCESSFUL_COMMIT

DESCRIPTION
  The function determines if an OTASP commit is successful or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if an OTASP commit is successful
  FALSE, else.

SIDE EFFECTS
  None.

===========================================================================*/
boolean otasp_is_successful_commit (void)
{
  MSG_MED("otasp_is_successful_commit: %d", otasp_successful_commit, 0,0);
  return (otasp_successful_commit);
}

/*===========================================================================

FUNCTION OTASP_IS_OTASP_ALLOWED

DESCRIPTION
  The function determines if OTASP is allowed or not

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if an OTASP is allowed
  FALSE, else.

SIDE EFFECTS
  None.

===========================================================================*/
boolean otasp_is_otasp_allowed (void)
{
 boolean otasp_allowed = TRUE;

 if( (otasp_failed_unlock_attempts == OTASP_MAX_UNLOCK_ATTEMPTS) 
   || (otasp_otapa_failed_spc_val_attempts == OTASP_MAX_UNLOCK_ATTEMPTS)
   || (otasp_otapa_failed_spasm_val_attempts == OTASP_MAX_UNLOCK_ATTEMPTS) )
 {
  ERR("Max invalid SPC/SPASM validation attempts. So OTASP not allowed.",
           0,0,0);
  otasp_allowed = FALSE;
 }

 return (otasp_allowed);
}

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION OTASP_SET_RUIM_NAM_LOCK_STATE

DESCRIPTION
  This function indicates the nature of the otasp call to the R-UIM and
  accordingly sets the nam lock state in the R-UIM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void otasp_set_ruim_nam_lock_state
(
   otasp_call_type ota_call_type
)
{
  /* Pointer to command for UIM task */
  uim_cmd_type *uim_cmd_ptr;
  /* Pointer to report from UIM task */
  uim_rpt_type *uim_rpt_ptr;
  /* This is the value that we would use as default if the read FAILS
     indicating that the NAM is locked */
  byte temp_nam_lock = UIM_OTA_NAM_LOCK;

  /* Read the NAM_LOCK EF from the card */

  if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
  {
      uim_cmd_ptr->access_uim.access = UIM_READ_F;
      uim_cmd_ptr->access_uim.item = UIM_CDMA_NAM_LOCK;
      #if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )
#error code not present
      #endif /* #if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) ) */

      uim_cmd_ptr->access_uim.num_bytes = 1;
      uim_cmd_ptr->access_uim.data_ptr = uim_access_buffer;
      uim_cmd_ptr->access_uim.offset = 0;
      uim_cmd_ptr->hdr.command = UIM_ACCESS_F;
      uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

      mcc_uim_cmd( uim_cmd_ptr );

      /* ----------------------------------
      ** Wait for MCC_UIM_Q_SIG to be set.
      ** ---------------------------------- */
      MSG_MED( "Waiting for UIM report", 0, 0, 0 );

      (void) mcc_wait( MCC_UIM_Q_SIG );

      uim_rpt_ptr = (uim_rpt_type*) q_get( &mc_uim_q );

      if ( uim_cmd_ptr->hdr.status == UIM_RECEIVED_S )
      {
        if ((uim_rpt_ptr->rpt_type == UIM_ACCESS_R) &&
            (uim_rpt_ptr->rpt_status == UIM_PASS))
        {
          temp_nam_lock = uim_access_buffer[0];
        }
        else
        {
          MSG_MED( "Read Failed for OTA Features ", 0, 0, 0 );
        }

      }
      else
      {
         ERR( "Bad status from UIM %d", uim_cmd_ptr->hdr.status, 0,0);
      }

      /* Put the buffer back in the queue */
      if (uim_rpt_ptr != NULL)
      {
        cmd_done( &uim_rpt_ptr->rpt_hdr );
      }
  }
  else
  {
      ERR( "No free buffers on uim_free_q", 0, 0, 0 );
  }

  /* Only the least 3 bits of the EF contain info about the nam lock */
  temp_nam_lock &= UIM_NAM_LOCK_BITS;

  if (ota_call_type == USER_INIT_SRV_PROV)
  {
      /* Clear the OTA bit */
      temp_nam_lock = (temp_nam_lock & ~UIM_NAM_LOCK_OTA_MASK);

      /* Set the OTA bit as OTASP Call */
      temp_nam_lock |= UIM_OTA_OTASP;

      /* Clear the nam lock state bit */
      temp_nam_lock = (temp_nam_lock & ~UIM_NAM_LOCK_STATE_MASK);

      /* Set the nam lock state as unlocked */
      temp_nam_lock |= UIM_NL_STATE_UNLOCK;
  }
#ifdef FEATURE_OTASP_OTAPA
  else if (ota_call_type == NETWORK_INIT_SRV_PROV)
  {
      /* Clear the OTA bit */
      temp_nam_lock = (temp_nam_lock & ~UIM_NAM_LOCK_OTA_MASK);

      /* Set the OTA bit as OTASP Call */
      temp_nam_lock |= UIM_OTA_OTAPA;

      /* Clear the nam lock state bit */
      temp_nam_lock = (temp_nam_lock & ~UIM_NAM_LOCK_STATE_MASK);

      /* Set the nam locked state to reflect the nam lock */
      if (cdma.nam_lock)
      {
         temp_nam_lock |= UIM_NL_STATE_LOCK;
      }
      else
      {
         temp_nam_lock |= UIM_NL_STATE_UNLOCK;
      }
  }
#endif /* FEATURE_OTASP_OTAPA */

  uim_access_buffer[0] = temp_nam_lock;

  /* Write the data into the NAM LOCK EF */
  if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
  {
      uim_cmd_ptr->access_uim.access = UIM_WRITE_F;
      uim_cmd_ptr->access_uim.item = UIM_CDMA_NAM_LOCK;
      #if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )
#error code not present
      #endif /* #if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) ) */
      uim_cmd_ptr->access_uim.num_bytes = 1;
      uim_cmd_ptr->access_uim.data_ptr = uim_access_buffer;
      uim_cmd_ptr->access_uim.offset = 0;
      uim_cmd_ptr->hdr.command = UIM_ACCESS_F;
      uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

      mcc_uim_cmd( uim_cmd_ptr );

      /* ----------------------------------
      ** Wait for MCC_UIM_Q_SIG to be set.
      ** ---------------------------------- */
      MSG_MED( "Waiting for UIM report", 0, 0, 0 );

      (void) mcc_wait( MCC_UIM_Q_SIG );

      uim_rpt_ptr = (uim_rpt_type*) q_get( &mc_uim_q );

      if ( uim_cmd_ptr->hdr.status == UIM_RECEIVED_S )
      {
         if ((uim_rpt_ptr->rpt_type == UIM_ACCESS_R) &&
            (uim_rpt_ptr->rpt_status != UIM_PASS))
         {
            MSG_MED( "Write Failed for OTA Features ", 0, 0, 0 );
            ERR( " NAM_LOCK not written correctly", 0, 0, 0);
         }

      }
      else
      {
         ERR( "Bad status from UIM %d", uim_cmd_ptr->hdr.status, 0,0);
         MSG_HIGH( " NAM_LOCK not written correctly", 0, 0, 0);
      }

      /* Put the buffer back in the queue */
      if (uim_rpt_ptr != NULL)
      {
        cmd_done( &uim_rpt_ptr->rpt_hdr );
      }
  }
  else
  {
      ERR( "No free buffers on uim_free_q", 0, 0, 0 );
      MSG_HIGH( " NAM_LOCK not written correctly", 0, 0, 0);
  }
} /* otasp_set_ruim_nam_lock_state */
#endif /* FEATURE_UIM_RUIM */
#else

/* Null function to remove compiler warning
*/
int null_otasp_function(void) { return 1; }

#endif /* FEATURE_OTASP */

/*lint +e818 */
