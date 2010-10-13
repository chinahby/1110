/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             C D M A   I N I T I A L I Z A T I O N   S T A T E

GENERAL DESCRIPTION
  This module contains the state machine for the CDMA sub-system,
  Initialization state. During this state, the mobile attempts to
  acquire the CDMA system.

EXTERNALIZED FUNCTIONS
  cdma_init
    This is the entrance procedure for the initialization state.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  This procedure assumes that resources such as queues and services are
  already initialized.

Copyright (c) 1990-2009 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccini.c_v   1.35   02 Oct 2002 13:42:16   azafer  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccini.c#6 $ $DateTime: 2010/01/29 04:32:10 $ $Author: sshahi $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/10   ssh     Corrected the ovhd_chan_info.ovhd_chan variable when TD 
                   mode specified by sync msg is not supported by the MS.
12/30/09   jj      Re-arranged DB writes of few SCHM time related params. 
12/27/09   jj      Added changes to refresh few NV items, during powerup.
06/11/09   jj      Re-arranged the NV write operation of few SCHM params.
06/01/09   ag      Featurized the use of new SD api under FEATURE_C2K_TIME_INFO.
                   Included sys.h.
05/29/09   ag      Used new SD APIs to pass CDMA time information to SD
06/06/08   pk      Update the previous changes to use mcc_nv_cmd instead of 
                   mc_put_nv_item, which is a local function in mc.c
05/15/08   bb      MVS API UPdate feature changes
03/21/08   pg      Correct some errors in the previous change.
03/13/08   pg      Read/write db_lp_sec, db_ltm_off, and db_daylt from/to NV
                   at the Sync Message processing time.
01/21/08   pg      Added support for '+' code dialing.
05/11/07   pg      More Lint Cleanup.
10/24/06   fc      Changes to support reverse link timing transfer 
                   enhancement for UMTS to 1x Handover.
09/25/06   fc      UMTS changes.
08/14/06   fc      Changed SD interface for unlock RF.
08/02/06   fc      Added the SD system exit reason reporting for sync 
                   acquisition failure, pilot acquisition failure and system
                   lost. 
07/06/06   fc      Renamed SRCH_LOST_RF_RESOURCE_R to 
                   SRCH_REQUEST_SYS_RESTART_R.
06/02/06   fc      Moved stuff to umts module.
06/01/06   an      Added the bypass registration indicator changes
05/18/06   fc      Added support for SRCH_LOST_RF_RESOURCE_RPT.
04/26/06   an      Fixed lint errors
02/07/06   fc      Removed duplicate function prototypes.
10/15/05   fc      Cleanup on NCC feature.
07/29/05   fc      Changes to inform Searcher the default page slot that MS
                   shall monitor upon enter idle from sync through 
                   SRCH_IDLE_F command.
07/11/05   fh      cleaned up header file including from MUX
                      changed rxc.h to rxcmc.h
                      changed txc.h to txcmc.h
05/31/05   fc      Merged the following from MSMSHARED:
05/31/05   va       Moved RF tuning to SRCH as part of SHDR changes
04/19/05   fc      Merged the following from MSMSHARED:
04/13/05   an       Mainlined FEATURE_SRCH_INACTIVE_STATE
03/23/03   fc      Merged the following from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated 
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/25/05   fc      Merged the following from MSMSHARED:
02/23/05   fc       Fixed the issue of acquisition failure because MS keep the 
                    stale Sync Channel Msg instead of storing the fresh one.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
02/03/05   fc      Fixed lint error.
01/07/05   an      Removed feature FEATURE_SMS and FEATURE_UASMS
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
10/19/04   fc      Obsolete SRCH_FADE_F interface,
10/19/04   fc      Featurized cdma_unjump under !FEATURE_SRCH_INACTIVE_STATE.
10/06/04   an      Reseting acq_seq and acq_typ everytime we acquire the sync channel
07/21/04   sj      Added support for umts to 1x handover.
06/30/04   dna     Changes for FEATURE_SRCH_INACTIVE_STATE.
                   This feature disables the t20m timer and gives TXC_CDMA_F 
                   just before entering idle, when MC has the RF lock, since
                   this command turns off the PA. 
03/05/04   bkm     Lint cleanup.
01/15/04   yll     Removed the sam_unlock from this file. It's done in func
                   cdma_exit().
01/05/04   pg      Mainlined FEATURE_MULTIMODE_ARCH.
12/16/03   aaj     Added sam_unlock when 1x acquisition fails
11/13/03   yll     Moved the resetting of cdma.call_orig outside the function
                   mcc_send_orig_failure().
11/04/03   yll     Changes with srch_tune_rfm() interface.
10/29/03   sst     T_M6500 in last checkin changed to proper T_MSM6500
10/29/03   sst     Surrounded SRCH_HAS_PILOT_GAIN_CNTL with T_M6500
10/24/03   sst     Changed RF tuning calls with srch_tune_rfm(), featurized 
                   as SRCH_HAS_PILOT_GAIN_CNTL
08/01/03   dyc     Added support for FEATURE_MULTI_RX_CHAIN
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
04/14/03   va      Avodied storing PCH entry in BS_info array if BS
                   supports BCCH/FCCCH.
04/07/03   bkm     Removed code for obsolete variable cdma_active_set.
12/02/02   yll     Added event logging for pref system reselection.
11/13/02   yll     Set the rxc_idle_f flag ebcsms to FALSE for Primary BCCH.
11/07/02   az      Removed the feature TMOBILE
10/31/02   va      Added support for dynamic p_rev change.
10/02/02   az      Mainlined FEATURE_IS2000
09/30/02   sb      Mainlined FEATURE_SD20.
09/06/02   fc      Added aset update.
08/28/02   fc      Cleaned lint errors.
08/13/02   HQ      Added FEATURE_CDSMS.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination.
06/07/02   sj      Added CP support for multimode architecture.
05/06/02   yll     Changed the Broadcast SMS function names.
04/30/02   sb      Added layer to payload of mcc_send_orig_failure.
04/12/02   lh      Added prevention of stuck in calling.
04/11/02   lh      Added catch all to prevent SMS failure.
04/09/02   yll     Changed handling of MC_BCSMS_ENABLE_F to a function call.
04/01/02   fc      Added walsh code channel as payload of RXC_IDLE_F.
03/08/02   hxw     Modified cdma_pil_acq() to handle UI cmd before SRCH resport.
02/22/02   fc      Added support for Release A common channels.
02/14/02   fc      Merged from 1.23 in MSM archive
 02/06/02  abh     Added code to go into GPS state when Phone is in Acquisition state.
 02/06/02  sh      Added hybird operation support
 01/25/02  sj      Added parameters in a call to sd_ss_user_ss_pref().
 01/19/02  az      Added a parameter in a call to mcsys_process_pref_sys_change()
 01/12/02  az      Assigned favored_roam_ind variable from cmd queue to the exit
                   reason data structure
01/17/01   va      Merged from 1.18 in MSM archive
  12/03/01 fc      Corrected timer event id to T21M in cdma_sync_msg.
01/08/02   lh      Remove FEATURE_IS2000_REL_A_SVD from MC-CM interface.
12/11/01   ph      merged from SVD branch
                   11/06/01   ph      Merged in a bug fix from main branch related to bs_info[]
                                      update upon sync chan msg processing.
                   08/28/01   lh      Added support for Concurrent Service.
12/06/01   lh      Merged from MSM_CP.05.00.108.
  11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                     mechanism.
12/03/01   lh      Removed some FEATURE_IS2000_REL_A.
10/24/01   ph/fc   In cdma_sync_msg(), fixed problem with initialization of
                   ovhd_chan flag when "pot_cdmach" is different from current cdmach.
                   Also, some other bug fixes.
10/24/01   kk      Merge from main archive, CP VU 99.
                   10/17/01   lcc     When paging channel frequency is different from that of sync
                   channel, when BS info is initialized, copy the sync channel
                   message over if the sync channel message has not been received
                   on the new frequency.
                   10/10/01   lcc     Added feature to delay update of P_REV_IN_USE until paging
                   channel.
10/02/01   yll     Merged from common archive.
                   -Copied the fwd_link_failure flag from the cmd queue to the
                   exit reason data structure
08/16/01   kk      Merge from common archive.
                   - Added update of bs_info if paging channel is on a different
                   frequency from sync channel.
07/24/01   fc      Removed the BCCH info processing of Sync Channel Msg to
                   fix QPCH issue.
07/16/01   lh      Deleted inclusion of mcsyspr.h to make it compile for sd20.
03/06/01   fc      Added support of events report: Call processing state change
                   and Protocol mismatch.
03/01/01   va      Merged from common archive.
02/26/01   sko     Removed JCDMA feature to poll CDMA_CH by CM.
02/16/01   fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
02/16/01   sh      added support for MC_END_F ack
01/24/01   ych     Merged T-53/JCDMA features for SK.
01/10/01   va      Added sync channel processing changes for Release A.
11/13/00   lcc     Added initial support for event reports.  Moved update of
                   BS_P_REV and p_rev_in_rev to mcc_update_bs_p_rev.
10/02/00   yll     Added support for Position Determination DBM.
09/20/00   jq      Cleaned all lint errors.
06/14/00   cah     FEATURE_ERRLOG_DEBUG support will now log Sync Channel
                   Acquisition failed.
06/14/00   kk      added missing FEATURE_E911 flag.
06/02/00   kk      Modified wrong system/network exit in case of an
                   emergency call.
04/05/00   va      Merged the following from PP (MSM3100_CP.03.00.17):
           jq      Added broadcast SMS support
           ry      Added broadcast OTAPA support
03/13/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
           ych     Added support for Trimode Silent Redial
           ht      Add call to mcsyspr_set_last_cdma after rf_tune_to_chan.
           ck      Added band class parameter to SRCH_FADE report
           ck      Added band class info in the SRCH_PC_F and SRCH_ACQ_F
                   commands.
11/15/99   va      IS2000 Voice Changes.
                   Support for enhanced sync message.
10/28/99   lcc     Moved PLT related code to plt_mc.c.
09/16/99   lcc     Merged in PLT support from PLT archive:
                   1. Supported the user-specified option whether or not the user
                      wants to wait for a Paging Channel message before proceeding.
                   2. Added code so that PLT_MCC waits for a Paging Channel message
                      before it returns PLT_SUCCESS status to RPC function.
                      Corrected the computation for Paging Channel long code mask.
                   3. Added support for PLT.
08/13/99   lh      CM and SMS phase 2 implementations.
07/28/99   lh      CM and two-way SMS support added.
05/04/99   kmp     Merged all of the IS-95B changes below plus changed T_ACP
                   to FEATURE_ACP
           kk      added NDSS support.
           lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
           ks      Added code to set the p_rev_in_use when the mobile receives
                   a Sync Channel message. This is for IS95B run-time p_rev.
04/28/99   kjn     Added #include of assert.h.
04/23/99   abh     Fixed problem in which UI showed call ended, but MC went
                   on traffic channel.
11/24/98   br      Added changes for storing new NAM.
08/06/98   ych     Fixed the Lint warnings
08/04/98   ych     Merged E911, SSPR-800, and Acq. Improvements
06/25/98   kis     Added code for ASCII digit mode in mcc_ui_ini function.
06/17/98   wjn     Added Feature E911 Support.
03/20/98   abh     Review comments for PRL Force Mode
02/02/98   abh     Merged changes for PRL Forced Mode
01/26/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
07/15/97   dna     Modified call to mcc_put_msg().
05/20/97   ck      Added support for Low Power Mode
12/13/96   jca     Modified processing for MC_ORIGINATION_F command.
10/17/96   jjw     Bug fix for AMPS originations from DS task
10/11/96   jjw     Added AMPS data task initiated call capability.
09/26/96   dna     New OTASP orig processing.
09/13/96   dna     Added processing for OTASP activation origination, etc.
04/09/96   day     Added TMSI addressing modifications.  Using T_TMSI ifdef.
03/28/96   jca     Acq timer now the same (T20m) for both PCS and cellular.
12/21/95   jca     Added DB_BS_P_REV for 13K/8K service negotiation "fix".
11/30/95   rdh     Added Search fade report to handle broken Sync channels.
08/22/95   dna     New Sync Channel Message field, only for p_rev >= 3
07/20/95   jca     Code updates to support subscriber access control.
06/28/95   rdh     Mods to support mini/micro acquisition modes.
06/09/95   jca     Mods to support new queue sigs interface to mcc_wait().
03/30/95   rdh     Eliminated slam bug catch fields. Fixed in Search task.
11/22/94   gb      Rhometer test functionality changed to standard TMOBILE.
11/14/94   dna     Added initialization of page_chan_s.
11/01/94   gb      Sector Lock test functionality added.
10/24/94   rdh     Additions for PN autoswitch/MSM error correction.
05/02/94   gb      Use DIPSW7 as stay in pilot/sync acq mode.
04/25/94   gb      Parameterised all IS95 timeouts in cai_tmo.
04/20/94   jca     Mods for IS-95 Phase 2 upgrade.
12/22/93   jca     Mods for IS-95 Phase 1 upgrade.
12/07/93   jah/gb  Removed duplicate cmd_done() in cdma_jump(),
                   and moved mccreg_idle_init() call to mccidl.c
07/24/92   jai     Removed time-out parameter from procedures which send
                   commands to tasks.  Added debug MSG calls.
06/30/92   jai     Altered module for DMSS
01/22/91   jai     Created module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <memory.h>
#include "customer.h"
#include "comdef.h"
#include "mobile.h"
#include "qw.h"
#include "rex.h"
#include "queue.h"
#include "task.h"
#include "srchmc.h"
#include "txcmc.h"
#include "rxcmc.h"
#include "err.h"
#include "mccdma.h"
#include "db.h"
#include "bio.h"
#include "ran.h"
#include "mc.h"
#include "mci.h"
#include "msg.h"
#include "parm.h"
#include "mccreg.h"
#include "mccrxtx.h"
#include "mccsrch.h"
#include "tmsi.h"
#include "sd.h"
#include "mccap.h"

#include "event.h"
#include "mclog.h"
#include "mccbcsms.h"
#include "caix.h"
#include "nv.h"

#ifdef FEATURE_IS2000_REL_A_CC
#include "cai.h"
#endif /* FEATURE_IS2000_REL_A_CC */

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

#include "mmoc.h"
#include "sys.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/* first subframe that Sync channel message was received in */
LOCAL byte sframe_num;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Buffers used to send commands to the tasks */

static db_items_value_type db_value;  /* Buffer to send data to DB */

/*===========================================================================

FUNCTION MCC_UI_INI

DESCRIPTION
  This function processes commands received from the user interface during
  the initialization state.

DEPENDENCIES
  None.

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mcc_ui_ini
(
  mc_msg_type *cmd_ptr
    /* pointer to command received from ui */
)
{
  word next_state;
    /* next state to be processed */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* initialize return state to current state */
  next_state = cdma.curr_state;

  switch( cmd_ptr->hdr.cmd )
  {

    case MC_ORIGINATION_F:
      cmd_ptr->hdr.status = MC_BADCMD_S;
      /* ----------------------------
      ** Inform CM origination failed
      ** ---------------------------- */
      mcc_send_orig_failure( CM_CALL_ORIG_ERR_NO_SRV,
                             cmd_ptr->origination.call_id,
                             CM_CALL_ORIG_ERR_LAYER_OTHER
                           );
      cdma.call_orig = FALSE;
      break;


    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
    #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

    case MC_END_F:
      ERR( "Invalid UI cmd %d", cmd_ptr->hdr.cmd, 0, 0 );
      cmd_ptr->hdr.status = MC_BADCMD_S;
      break;


    #if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
    case MC_MO_DBM_F:
      /* ----------------------------------------------------------
      ** It is not possible to receive another MO DBM while one is
      ** already in progress.
      ** ---------------------------------------------------------- */
      if( cdma.dbm_wait_for_l2_ack )
      {
        /* In stead of err_fatal, we just reject the current request.
         * CM is expected to only send one DBM origination at a time.
         * but in case it sends a SMS and then a PD, for example, we
         * should reject the PD request. */
        MSG_ERROR("DBM received while in progress %d %d",
                  cmd_ptr->mo_dbm.burst_type, cdma.dbm_wait_for_l2_ack, 0);
        cmd_ptr->hdr.status = MC_BADCMD_S;
        mcc_send_mo_dbm_status(cmd_ptr->mo_dbm.burst_type, CM_DBM_MO_HOLD_ORIG);
        break;
      }

      cmd_ptr->hdr.status = MC_BADCMD_S;
      mcc_send_mo_dbm_status(cmd_ptr->mo_dbm.burst_type, CM_DBM_MO_HOLD_ORIG);
      break;
    #endif /* FEATURE_CDSMS || FEATURE_DBM */


    #ifdef FEATURE_BROADCAST_SMS
    case MC_BCSMS_ENABLE_F:
      MSG_MED( "Rxed BCSMS_ENABLE = %d in INIT",
                cmd_ptr->bcsms_enable.enable,0,0 );
      mccbcsms_enable(cmd_ptr->bcsms_enable.enable);
      break;
    #endif /* FEATURE_BROADCAST_SMS */


    case MC_CDMA_PROT_DEACT_CMD_F:
      mcc_exit.exit = mc_map_deact_to_mcc_exit_reason
                      (
                        cmd_ptr->deact_prot.info.reason
                      );
      mcc_exit.mmoc_trans_id = cmd_ptr->deact_prot.info.trans_id;
      next_state = CDMA_EXIT;
      cmd_ptr->hdr.status = MC_DONE_S;
      break;

    case MC_CDMA_PROT_GEN_CMD_F:
      /* Process the MMoC's generic protocol command.
      */
      if (!mcc_continue_on_curr_sys(
            MEAS_BACK_ALLOWED,
            mmoc_proc_prot_gen_cmd( TRUE, TRUE, &cmd_ptr->gen_cmd.info ) )
         )
      {
         mcc_exit.exit = 
           mc_map_gen_cmd_to_mcc_exit_reason( &cmd_ptr->gen_cmd.info );
         next_state    = CDMA_EXIT;
      }
      cmd_ptr->hdr.status = MC_DONE_S;
      break;

    #ifdef FEATURE_MVS_MIGRATE
#error code not present
    #endif /* FEATURE_MVS_MIGRATE */

    default:
    {
      ERR( "Invalid UI cmd %d", cmd_ptr->hdr.cmd, 0, 0 );
      cmd_ptr->hdr.status = MC_BADCMD_S;
      break;
    }

  }/* switch */

  return( next_state );
  
} /* mcc_ui_ini */

/*===========================================================================

FUNCTION CDMA_JUMP

DESCRIPTION
  This function controls the "jump to hyperspace".

DEPENDENCIES
  None.

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_jump( void )
{
  mccsrch_rpt_type *srch_ptr;    /* Pointer to report from Searcher task */
  dword mask;                    /* Lower dword of Paging Channel mask */
  static mc_msg_type *cmd_ptr;   /* Command from the MC_CMD_Q queue */
  word next_state = CDMA_JUMP;   /* Next state to be processed */  /*lint !e641 */
  dword imsi_hash_key;           /* IMSI hash key */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Command Searcher task to Jump to slew */

  mcc_srch_buf.slew.hdr.cmd = SRCH_SLEW_F;
  mcc_srch_buf.slew.pilot_index = (int2) cur_bs_ptr->pilot_pn;
  mcc_srch_buf.slew.phase_80ms =  sframe_num;
  mcc_srch_buf.slew.cdma_freq   = cdma.cdmach;
  mcc_srch_buf.slew.band_class  = cdma.band_class;

  mcc_srch_cmd(&mcc_srch_buf);

  while (next_state == CDMA_JUMP)  /*lint !e641 */
  /* Wait for slew report from Searcher */
  {
    if ( (srch_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL )
    {
      if (srch_ptr->hdr.rpt == SRCH_SLEW_R)
      {
        /* begin telling tasks to go to Paging channel */
        next_state = CDMA_IDLE;  /* finish Jump to hyperspace and go to idle */
      }

      else if (srch_ptr->hdr.rpt ==  SRCH_SYS_MEAS_R)
      {
        /* Report system measurements to System Determination */
        mcc_sys_meas_rpt(srch_ptr);
        /* -----------------------------------------------------------
        ** Report system measurement to system determination. Based on
        ** SS-Action, determines the next state to be processed based
        ** on returned SS-Action.
        ** ----------------------------------------------------------- */
        if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
            sd_ss_ind_misc_meas_rprt(srch_ptr->sys_meas.meas_id, TRUE, NULL)))
        {
          mcc_exit.exit = MCC_RESELECTION;
          next_state = CDMA_EXIT;
          event_report(EVENT_PREF_SYS_RESEL);
        }
      }

      else if ( srch_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
      {
        /* ----------------------------------------------------------
        ** Searcher request system restart in the middle of jump to 
        ** hyperspace. Treat it as system lost.
        ** Inform system determination of pilot acquisition failure.
        ** Determine the next state to be processed based on returned
        ** SS-Action.
        ** ----------------------------------------------------------*/
        if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_ADVISE_UNLOCK_RF, NULL ) )
           )
        {
          MSG_HIGH ( "SRCH request sys restart", 0, 0, 0 );

          mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
          next_state = CDMA_EXIT;
        }
      }
      
      /* Place item back on queue and set signal if requested */
      cmd_done(&srch_ptr->hdr.rpt_hdr);
    }
    else
    {
      if( (cmd_ptr = (mc_msg_type*) q_get(&mc_cmd_q)) != NULL)
      {
        next_state = mcc_ui_ini(cmd_ptr);
        cmd_done(&cmd_ptr->hdr.cmd_hdr);
      }
      else
      {
        /* Wait for report */
        (void) mcc_wait(MCC_SRCH_Q_SIG | MCC_CMD_Q_SIG);
      }
    }
  } /* while */

  if (next_state == CDMA_IDLE)
  {
    int32 nTimezoneOffset = 0;
    uint8 ltm_off = 0;
    nv_item_type nv_db_item;
    nv_cmd_type mcc_nv_buf;

    /* Send request to Transmit task to go to CDMA mode */
    mcc_tx_buf.cdma.hdr.command = TXC_CDMA_F;
    mcc_txc_cmd( &mcc_tx_buf );

    /* -------------------------------------------------------------------
    ** MS initiates TXC now to start Access Channel operation for normal 
    ** idle activation.
    **
    ** MS initiate TXC  to start reverse link timing transfer at the
    ** beginning of UMTS to 1x Handover operation for pseudo-idle 
    ** activation.
    ** ------------------------------------------------------------------- */ 

    if ( cdma.entry_ptr->entry != MCC_PSEUDO_IDLE )
    {
      /* Continue with jump to Hyperspace */

      /* ----------------------------------------------------------------
      ** Per MUX request, MC send TXC_PC_F irrespectively of what type of
      ** forward common channel is in use.
      **
      ** Request that Transmit task begin Access Channel operations
      ** ---------------------------------------------------------------- */
      mcc_tx_buf.pc.hdr.command = TXC_PC_F;
      mcc_txc_cmd(&mcc_tx_buf);
    }

    /* ------------------------------------------------------------------------
    ** Tell lower layers to monitor Broadcast Control Channel or Paging Channel
    ** ------------------------------------------------------------------------*/

    /* Tell Receive task to go to idle state */
    mcc_rx_buf.idle.hdr.command = RXC_IDLE_F;

    #ifdef FEATURE_IS2000_REL_A_CC
    if (OVHD_CHAN_IS_BCCH)
    {
      mcc_rx_buf.idle.chan_info.chan_id = CAI_BCCH;
      mcc_rx_buf.idle.chan_info.config.bcch.code_chan = cdma.bcch;
      mcc_rx_buf.idle.chan_info.config.bcch.rate = cdma.brat;
      mcc_rx_buf.idle.chan_info.config.bcch.crate = cdma.bcch_code_rate;
    }
    else
    #endif /* FEATURE_IS2000_REL_A_CC */
    {
      mcc_rx_buf.idle.chan_info.chan_id = CAI_PCH;
      mcc_rx_buf.idle.chan_info.config.pch.rate = cdma.prat;
    }

    #ifdef FEATURE_IS2000_REL_A_CC
    mcc_rx_buf.idle.chan_info.ebcsms = FALSE;
    #endif /* FEATURE_IS2000_REL_A_CC */

    /* Multiply time received in message by 4 to get
       frame number for Receive task */
    qw_mul(mcc_rx_buf.idle.frame, cur_bs_ptr->sy.sys_time, 4L);

    /* seed random number generator */
    ran_seed((cdma.esn ^ qw_lo( mcc_rx_buf.idle.frame )) % 0x7FFFFFFF);
    mcc_rxc_cmd(&mcc_rx_buf);

    /* Time to tell Searcher to go to idle state */
    mcc_srch_buf.idle.hdr.cmd = SRCH_IDLE_F;
    mcc_srch_buf.idle.band_class = cdma.band_class;
    mcc_srch_buf.idle.cdma_freq = cdma.cdmach;

    /* Place long code state @ next 80ms into buffer */
    qw_equ(mcc_srch_buf.idle.pn_state, cur_bs_ptr->sy.lc_state); 
    /* Account for extra 80ms count in system time about to occur in TS */
    qw_equ(mcc_srch_buf.idle.sys_time, cur_bs_ptr->sy.sys_time);
    qw_dec(mcc_srch_buf.idle.sys_time, 1L);

    #ifdef FEATURE_IS2000_REL_A_CC
    /* calculate Broadcast Control or Paging Channel mask */
    if (OVHD_CHAN_IS_BCCH)
    {
      mask = 0xA4000000 | ((dword)(0 & 7) << 21); //lint !e737
    }
    else
    #endif /* FEATURE_IS2000_REL_A_CC */

    {
      mask = 0xA0000000 | ((dword)(cur_bs_ptr->ovhd_chan_info.ovhd_walsh_ch & 7) << 21); //lint !e737
    }
    mask |= cur_bs_ptr->pilot_pn &0x1FF ;
    qw_set(mcc_srch_buf.idle.pn_mask,  0x319L, mask);
 
    /* Default page slot that MS shall monitor upon enter idle. Real page slot 
       that MS shall monitor is notified through SRCH_PARM_F command while in
       idle state. This is important to make sure MER statistic is correct for
       scenarios such as IHO neighbor config 3, power up etc. */
    if ( ( cdma.scm & SCM_SLOTTED_CLASS_M ) == SCM_SLOTTED_V )
    {
      imsi_hash_key = cdma.imsi_s1 + (((dword) cdma.imsi_s2) << 24);
      mcc_srch_buf.idle.pgslot = 
        hash(imsi_hash_key, 2048, (word) (6*(imsi_hash_key & 0xfff)));
    }
    else
    {
      mcc_srch_buf.idle.pgslot = 0; 
    }

    mcc_srch_cmd(&mcc_srch_buf);

    /* Prepare command buffer to NV. */        
    mcc_nv_buf.cmd = NV_WRITE_F;
    mcc_nv_buf.data_ptr = &nv_db_item;
        
    if ( mcc_is_schm_time_param_chng == TRUE ) 
    {
      /* writing leap seconds into correspoding nv item */
      nv_db_item.db_lp_sec =  cur_bs_ptr->sy.lp_sec;

      /* write into NV*/
      mcc_nv_buf.item = NV_DB_LP_SEC_I;
      if (mcc_nv_cmd(&mcc_nv_buf ) != NV_DONE_S) 
      {
        ERR_FATAL("Failed write item to NV %d", mcc_nv_buf.item, 0, 0);
      }
   
      /* Convert local time offset value from sync channel to signed
       * integer. Sync channel contains time offset value in 30 min 
       * unit stamp */
      nTimezoneOffset = (int32)(cur_bs_ptr->sy.ltm_off & 0x3f);
      if (nTimezoneOffset >= 32)
      {
        nTimezoneOffset -= 64;
      }
      /* to set 15 min is the offset unit stamp */
      nTimezoneOffset *= 2;

      if (nTimezoneOffset > 0)
      {
        ltm_off = (uint8) nTimezoneOffset;
      }
      else
      {
        /* 8-bit two complement */
        ltm_off = (uint8) (-nTimezoneOffset);
        ltm_off = (~ltm_off) + 1;
      }
      /* Writing timezone offset value into corresponding nv item */
      nv_db_item.db_ltm_off = ltm_off;
              
      /* write into NV*/
      mcc_nv_buf.item = NV_DB_LTM_OFF_I;
      if (mcc_nv_cmd(&mcc_nv_buf ) != NV_DONE_S) 
      {
        ERR_FATAL("Failed write item to NV %d", mcc_nv_buf.item, 0, 0);
      }
      /* writing daylight saving flag into corresponding nv item */
      nv_db_item.db_daylt = (boolean)cur_bs_ptr->sy.daylt;
              
      /* write into NV*/
      mcc_nv_buf.item = NV_DB_DAYLT_I;
      if (mcc_nv_cmd(&mcc_nv_buf ) != NV_DONE_S) 
      {
        ERR_FATAL("Failed write item to NV %d", mcc_nv_buf.item, 0, 0);
      }
    }
    /* reset the SCHM time change param indicator flags to FALSE */
    
    mcc_is_schm_time_param_chng = FALSE;

    /* Initialize channel monitoring operation */
    mccidl_init_channel_monitoring();

    #ifdef FEATURE_IS2000_REL_A_CC
    if (OVHD_CHAN_IS_BCCH)
    {
      /* Tell Searcher TD info */
      mccidl_send_srch_idle_aset();
    }
    #endif /* FEATURE_IS2000_REL_A_CC */

    /* Update code channel */
    #ifdef FEATURE_IS2000_REL_A_CC
    if (OVHD_CHAN_IS_BCCH)
    {
      db_value.code_channel = cdma.bcch;
    }
    else
    #endif /* FEATURE_IS2000_REL_A_CC */
    {
      db_value.code_channel = cdma.pagech;
    }
    db_put(DB_CODE_CHANNEL, &db_value);

    /* Do idle registration initialization */
    mccreg_idle_init();

    if (!cur_bs_ptr->rx.sy_rx)
    {
      /* Should never be here since we just got the Sync Channel Msg */
      ERR("Sync Channel Msg not found", 0, 0, 0);
    }
    else
    {
      /* -------------------------------------------------------------------
       * Consult with system determination whether we should continue on the
       * current system - if not exit with MCC_NOT_PREFERRED.
       * ------------------------------------------------------------------- */
      if (!mcc_continue_on_curr_sys
           (MEAS_BACK_ALLOWED,
             #ifdef FEATURE_C2K_TIME_INFO
             sd_ss_ind_cdma_acq_schm4(cur_bs_ptr->sy.sid, cur_bs_ptr->sy.nid,
                                      cur_bs_ptr->sy.ltm_off,
                                      (sys_daylt_savings_e_type)cur_bs_ptr->sy.daylt,
                                      cur_bs_ptr->sy.lp_sec,
                                      NULL)
             #else
             sd_ss_ind_cdma_acq_schm3(cur_bs_ptr->sy.sid, cur_bs_ptr->sy.nid, 
                                      cur_bs_ptr->sy.ltm_off,
                                      (sd_daylt_savings_e_type)cur_bs_ptr->sy.daylt,
                                      NULL)
             #endif /* FEATURE_C2K_TIME_INFO */
           )
         )
      {
         mcc_exit.exit = MCC_NOT_PREFERRED;
         next_state = CDMA_EXIT;

         return (next_state);
      }
    }
  }

  /* If valid TMSI code has expired,     */
  /* then reset code and all TMSI timers */
  if (tmsi_expired ())
  {
    /* Reset TMSI code and all timers */
    tmsi_code_and_timer_reset();
  }
  /* If valid TMSI code has not expired, */
  /* set TMSI expiration timer           */
  else
  {
    /* Set TMSI expiration timer for valid TMSI code */
    tmsi_set_exp_timer();
  }

  return( next_state );

} /* cdma_jump */

/*===========================================================================

FUNCTION CDMA_SYNC_MSG

DESCRIPTION
  This function waits for a valid Sync channel message to be receieved
  from the Main Control task.  If a message is received, this function
  processes stores the message and checks for for CAI revision compatibility.

DEPENDENCIES
  None.

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_sync_msg( void )
{
  word next_state;            /* Next substate to be processed */
  mccrxtx_cmd_type *msg_ptr;  /* Pointer to msg received on the mcc_rxtx_q */
  mc_msg_type *cmd_ptr;       /* Pointer to command from UI */
  dword sigs;                 /* Return signals from mcc_wait() call */

  mccsrch_rpt_type *srch_ptr;    /* Pointer to report from Searcher task */

  boolean td_supported = FALSE;  /* Indicate if Transmit Diversity supported */
  word pot_cdmach = cdma.cdmach; /* CDMA frequency to be used */
  bs_info_id_type bs_info_id;    /* Fields unqiuely identify BS */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set Sync Channel acquisition time-out timer */
  (void) rex_set_timer( &mcc_state_timer, cai_tmo.t21m );

  next_state = CDMA_SYNC_MSG;  /*lint !e641 */

  while (next_state == CDMA_SYNC_MSG)  /*lint !e641 */
  {

    sigs = mcc_wait( MCC_RXTX_Q_SIG | MCC_STATE_TIMER_SIG | MCC_CMD_Q_SIG |
             MCC_SRCH_Q_SIG );
    if ((sigs & MCC_RXTX_Q_SIG) != 0)
    {
      if ((msg_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q) ) != NULL)
      {
        /* Received a message --> process it */

        if ((msg_ptr->hdr.command == MCCRXTX_MSG_F) &&
            (msg_ptr->msg.chn == RXC_CDMA_SC))  /*lint !e641 */
        {
          /* Message from Sync Channel */
          parm_store_sync_time( msg_ptr->msg.msg.sync.sys_time );

          if (cdma.mob_cai_rev < msg_ptr->msg.msg.sync.min_p_rev)
          {
            /* --------------------------------------------------------
            ** If the protocol revision level supported by the mobile
            ** statiom is less than the minimum protocol revision level
            ** supported by the base station, the mobile station shall
            ** enter the System Determination Substate with a protocol
            ** mismatch indication.
            ** -------------------------------------------------------- */
            MSG_HIGH( "Min CAI rev > mob_cai_rev", 0, 0, 0 );

            /* Event report: Protocol mismatch */
            event_report(EVENT_PROTOCOL_MISMATCH);

            /* ------------------------------------------------------------
            ** Inform system determination of Protocol Mismatch exit.
            ** Determine the next state to be processed based on returned
            ** SS-Action.
            ** ------------------------------------------------------------ */
            if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
              sd_ss_ind_cdma_acq_prot_mis(NULL)))
            {
              mcc_exit.exit = MCC_INCOMP_CAI_REV;
              next_state = CDMA_EXIT;
            }
          }
          else if (msg_ptr->msg.msg.sync.prat > CAI_PC_2_RATE)  /*lint !e641 */
          {
            /* ------------------------------------------------------------
            ** If the value of the PRATr field is designated as reserved by
            ** the protocol revision level supported by the mobile station
            ** the mobile station shall enter the System Determination
            ** Substate with a protocol mismatch indication.
            ** ------------------------------------------------------------ */
            MSG_HIGH( "Invalid Paging Channel Data Rate", 0, 0, 0 );

            /* Event report: Protocol mismatch */
            event_report(EVENT_PROTOCOL_MISMATCH);

            /* ------------------------------------------------------------
            ** Inform system determination of Protocol Mismatch exit.
            ** Determine the next state to be processed based ion returned
            ** SS-Action.
            ** ------------------------------------------------------------ */
            if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
              sd_ss_ind_cdma_acq_prot_mis(NULL)))
            {
              mcc_exit.exit = MCC_INCOMP_CAI_REV;
              next_state = CDMA_EXIT;
            }
          }

          #ifdef FEATURE_IS2000_REL_A_CC
          else if ((msg_ptr->msg.msg.sync.p_rev >= P_REV_IS2000_REL_A) &&
                  #ifdef FEATURE_DYNAMIC_P_REV
                  (cdma.mob_cai_rev >= P_REV_IS2000_REL_A) && /*DYNAMIC_P_REV */
                  #endif /*FEATURE_DYNAMIC_P_REV*/
                   ((msg_ptr->msg.msg.sync.sr1_bcch_non_td_incl &&
                     (msg_ptr->msg.msg.sync.sr1_brat_non_td >
                      CAI_BCCH_19200_RATE)) ||  /*lint !e641 */
                    (msg_ptr->msg.msg.sync.sr1_td_incl &&
                     (msg_ptr->msg.msg.sync.sr1_brat_td > CAI_BCCH_19200_RATE))))  /*lint !e641 */
          {
            /* ------------------------------------------------------------
            ** If the value of the BRATr field is designated as reserved by
            ** the protocol revision level supported by the mobile station
            ** the mobile station shall enter the System Determination
            ** Substate with a protocol mismatch indication.
            ** ------------------------------------------------------------ */
            MSG_HIGH( "Invalid BCCH Data Rate", 0, 0, 0 );

            /* Event report: Protocol mismatch */
            event_report(EVENT_PROTOCOL_MISMATCH);

            /* ------------------------------------------------------------
            ** Inform system determination of Protocol Mismatch exit.
            ** Determine the next state to be processed based ion returned
            ** SS-Action.
            ** ------------------------------------------------------------ */
            if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
              sd_ss_ind_cdma_acq_prot_mis(NULL)))
            {
              mcc_exit.exit = MCC_INCOMP_CAI_REV;
              next_state = CDMA_EXIT;
            }
          }
          #endif /* FEATURE_IS2000_REL_A_CC*/

          else
          {
            /* Read lp_sec from db item */
            db_get(DB_LP_SEC, &db_value);
            if (db_value.lp_sec !=  msg_ptr->msg.msg.sync.lp_sec)
            {
              /* put lp_sec into database */
              db_value.lp_sec = msg_ptr->msg.msg.sync.lp_sec;
              db_put(DB_LP_SEC, &db_value);
              mcc_is_schm_time_param_chng = TRUE;
            }
            /* Read ltm_off from db item */
            db_get(DB_LTM_OFF, &db_value);
            if (db_value.ltm_off != msg_ptr->msg.msg.sync.ltm_off)
            {
              /* put ltm_off into database */
              db_value.ltm_off = msg_ptr->msg.msg.sync.ltm_off;
              db_put(DB_LTM_OFF, &db_value);
              mcc_is_schm_time_param_chng = TRUE;
            }
            /* Read daylt from db item */
            db_get(DB_DAYLT, &db_value);
            if ( db_value.daylt != msg_ptr->msg.msg.sync.daylt)
            {
              /* put daylt into database */
              db_value.daylt = msg_ptr->msg.msg.sync.daylt;
              db_put(DB_DAYLT, &db_value);
              mcc_is_schm_time_param_chng = TRUE;
            }
            
            db_value.pilot_pn_off = msg_ptr->msg.msg.sync.pilot_pn;
            db_put(DB_PILOT_PN_OFF, &db_value);

            {
              /* Default ovhd chan to PC  */
              bs_info_id.ovhd_chan = CAIX_PC;
              /* Save this for idle handoff */
              cdma.prat = (cai_pch_rate_type) msg_ptr->msg.msg.sync.prat;

              #ifdef FEATURE_IS2000_REL_A_CC
              if ((msg_ptr->msg.msg.sync.p_rev >= P_REV_IS2000_REL_A) &&
                  #ifdef FEATURE_DYNAMIC_P_REV
                  (cdma.mob_cai_rev >= P_REV_IS2000_REL_A) && /*DYNAMIC_P_REV */
                  #endif /*FEATURE_DYNAMIC_P_REV*/
                  ((msg_ptr->msg.msg.sync.sr1_bcch_non_td_incl) ||
                   (msg_ptr->msg.msg.sync.sr1_td_incl)))
              {
                bs_info_id.ovhd_chan = CAIX_BCCH;

                /* Save data rate and code rate info for ECCLM hashing */
                if (msg_ptr->msg.msg.sync.sr1_bcch_non_td_incl)
                {
                  cdma.bcch_non_td =
                    msg_ptr->msg.msg.sync.sr1_bcch_code_chan_non_td;
                  cdma.brat_non_td = (cai_bcch_rate_type) msg_ptr->msg.msg.sync.sr1_brat_non_td;
                  cdma.bcch_code_rate_non_td =
                    (cai_code_rate_type) msg_ptr->msg.msg.sync.sr1_crat_non_td;
                  if (!msg_ptr->msg.msg.sync.sr1_td_incl)
                  {
                    /* ------------------------------------------------------
                     * If TD channel info is not provided, use non-TD channel
                     * info to initialize TD channel info.
                     * ------------------------------------------------------ */
                    cdma.bcch_td = cdma.bcch_non_td;
                    cdma.brat_td = cdma.brat_non_td;
                    cdma.bcch_code_rate_td = cdma.bcch_code_rate_non_td;
                  }
                }

                if (msg_ptr->msg.msg.sync.sr1_td_incl)
                {
                  cdma.bcch_td = msg_ptr->msg.msg.sync.sr1_bcch_code_chan_td;
                  cdma.brat_td = (cai_bcch_rate_type) msg_ptr->msg.msg.sync.sr1_brat_td;
                  cdma.bcch_code_rate_td = (cai_code_rate_type) msg_ptr->msg.msg.sync.sr1_crat_td;
                  if (!msg_ptr->msg.msg.sync.sr1_bcch_non_td_incl)
                  {
                    /* ------------------------------------------------------
                     * If non-TD channel info is not provided, use TD channel
                     * info to initialize non-TD channel info.
                     * ------------------------------------------------------ */
                    cdma.bcch_non_td = cdma.bcch_td;
                    cdma.brat_non_td = cdma.brat_td;
                    cdma.bcch_code_rate_non_td = cdma.bcch_code_rate_td;
                  }
                }

                if ((msg_ptr->msg.msg.sync.sr1_td_incl) &&
                    mccap_is_csch_td_mode_supported(msg_ptr->msg.msg.sync.sr1_td_mode))
                {
                  /* TD is supported */
                  td_supported = TRUE;
                  pot_cdmach = msg_ptr->msg.msg.sync.sr1_cdma_freq_td;
                  cdma.brat = cdma.brat_td;
                  cdma.bcch_code_rate = cdma.bcch_code_rate_td;
                  cdma.bcch = msg_ptr->msg.msg.sync.sr1_bcch_code_chan_td;
                  cdma.td_mode = msg_ptr->msg.msg.sync.sr1_td_mode;
                  cdma.td_power_level =
                    msg_ptr->msg.msg.sync.sr1_td_power_level;
                }
                else
                {
                  /* Non-TD is supported */
                  if (msg_ptr->msg.msg.sync.sr1_bcch_non_td_incl && msg_ptr->msg.msg.sync.sr1_non_td_freq_incl)
                  {
                    pot_cdmach = msg_ptr->msg.msg.sync.sr1_cdma_freq_non_td;
                  }
                  else
                  {
                    pot_cdmach = msg_ptr->msg.msg.sync.ext_cdma_freq;
                  }
                  cdma.brat = cdma.brat_non_td;
                  cdma.bcch_code_rate = cdma.bcch_code_rate_non_td;
                  cdma.bcch = cdma.bcch_non_td;
                }
                
                /* As per the requirement in section 2.6.1.4 (Timing Change
                ** Substate), changing the ovhd_chan to CAIX_PC */

                if (!((msg_ptr->msg.msg.sync.sr1_bcch_non_td_incl) || 
                       (msg_ptr->msg.msg.sync.sr1_td_incl) &&
                    mccap_is_csch_td_mode_supported(msg_ptr->msg.msg.sync.sr1_td_mode)))
                {
                  bs_info_id.ovhd_chan = CAIX_PC;
                }
              }
              else
              #endif /* FEATURE_IS2000_REL_A_CC*/

              if ((msg_ptr->msg.msg.sync.p_rev >= P_REV_IS2000) &&
                  #ifdef FEATURE_DYNAMIC_P_REV
                  (cdma.mob_cai_rev >= P_REV_IS2000) && /*DYNAMIC_P_REV */
                  #endif /*FEATURE_DYNAMIC_P_REV*/
                  mccap_is_enhanced_operation_supported())
              {
                pot_cdmach =  msg_ptr->msg.msg.sync.ext_cdma_freq;
              }
              else
              if (
                   (mcc_is_band_class_in_group(MCC_PCS_GROUP, cdma.band_class))
                   ||
                   (
                     (mcc_is_band_class_in_group(MCC_CELLULAR_GROUP, cdma.band_class))
                     &&
                     (msg_ptr->msg.msg.sync.p_rev >= 3)
                   )
                  )
              {
                /* The Base Station supports the Sync Channel Frequency */
                if (cdma.cdmach != msg_ptr->msg.msg.sync.cdma_freq)
                {
                   pot_cdmach = msg_ptr->msg.msg.sync.cdma_freq;
                }
              }

              /* ----------------------------------------------------------
              ** We now know the overhead channel or frequency to idle on,
              ** we need to start storing information about this base
              ** station. Initialize a new entry in bs_info array (or find
              ** the old one if it exists.
              ** --------------------------------------------------------- -*/
              bs_info_id.pilot_pn = msg_ptr->msg.msg.sync.pilot_pn;
              bs_info_id.cdma_ch = pot_cdmach;
              bs_info_id.band_class = cdma.band_class;
              bs_info_id.ovhd_walsh_ch =
                #ifdef FEATURE_IS2000_REL_A_CC
                (bs_info_id.ovhd_chan == CAIX_BCCH) ? cdma.bcch :
                #endif /* FEATURE_IS2000_REL_A_CC */
                PRI_PAGECH;

              mcc_entry_init(&bs_info_id, td_supported);

              /* It is possible that Common Channel may be on a different
                 frequency from Sync Channel. However, we count on system time
                 specified in Sync Channel Message to do acquisition. It is 
                 important to make sure the freshness of stored copy of Sync
                 Channel Message. So anyway, place the received Sync Channel 
                 msg into the entry for this BS */
              mcc_put_msg(MC_SYNC_MSG, &msg_ptr->msg.msg);    /*lint !e641 */
              /* Initialize the registration variables as per C.S0005-D v2.0 
               * section 2.6.5.5.1.1 */
              if (cdma.entry_ptr->gen.new_mode_or_blksys)
              {
                /* Initialize registration variables */
                mccreg_acq_init();
              }

              if (pot_cdmach != cdma.cdmach)
              {
                cdma.cdmach = pot_cdmach;
                MSG_HIGH("Sync freq %d", cdma.cdmach, 0, 0);
                /* Actual tune is done in SRCH as part of SLEW cmd */
              }

              if (!BIO_GET_SW( BIO_SW7_M ))
              {
                /* Go to jump to hyperspace (Timing Change Substate) */

                /* Set up variables for JUMP state */
                sframe_num = msg_ptr->msg.frm.sframe;

                #ifdef FEATURE_IS2000_REL_A_CC
                if (OVHD_CHAN_IS_BCCH)
                {
                  /* Initialize forward common control channel to Primary
                     forward common control channel */
                  cdma.fccch_id = PRI_FCCCH;
                  /* Initialize FCC_CHANs to 1 */
                  cdma.num_fccch_s = 1;
                }
                else
                #endif /* FEATURE_IS2000_REL_A_CC */

                {
                  /* Initialize paging channel to Primary Paging channel */
                  cdma.pagech = PRI_PAGECH;
                  /* Initialize PAGE_CHANs to 1 (IS-95 section 6.6.1.4) */
                  cdma.page_chan_s = 1;
                }
                /* Attempt jump to hyperspace */
                /* Reseting the acq_seq and acq_typ everytime we acquire the sync channel */
                mcc_layer2.ack_seq  = 7;  /* Binary '111' */
                mcc_layer2.ack_type = 0;
                next_state = CDMA_JUMP;  /*lint !e641 */
              }
            }
          }
        }
        /* Otherwise stay in Sync Channel message state */
        /* Place message from layer 2 back on queue if requested */
        cmd_done( &msg_ptr->hdr.cmd_hdr );

      } /* end if ((msg_ptr = q_get( &mcc_rxtx_q) ) != NULL) */
    } /* end if ((sigs & MCC_RXTX_Q_SIG) != 0) */

    else if ((sigs & MCC_CMD_Q_SIG) != 0)
    {
      if ((cmd_ptr = (mc_msg_type*) q_get( &mc_cmd_q )) != NULL)
      {
        next_state = mcc_ui_ini( cmd_ptr );
        cmd_done( &cmd_ptr->hdr.cmd_hdr );
      }
    } /* end else if ((sigs & MCC_CMD_Q_SIG) != 0) */

    else if ((sigs & MCC_SRCH_Q_SIG) != 0)
    {
      if ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL)
      {
        if (srch_ptr->hdr.rpt ==  SRCH_SYS_MEAS_R)
        {
          /* Report system measurements to System Determination */
          mcc_sys_meas_rpt(srch_ptr);
          /* -----------------------------------------------------------
          ** Report system measurement to system determination. Based on
          ** SS-Action, determines the next state to be processed based
          ** on returned SS-Action.
          ** ----------------------------------------------------------- */
          if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
            sd_ss_ind_misc_meas_rprt(srch_ptr->sys_meas.meas_id, TRUE, NULL)))
          {
            mcc_exit.exit = MCC_RESELECTION;
            next_state = CDMA_EXIT;
            event_report(EVENT_PREF_SYS_RESEL);
          }
        }

        else if ( srch_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
        {
          /* ----------------------------------------------------------
          ** Searcher requests system restart in the middle of waiting 
          ** for Sync Channel Message. Treat it as sync channel 
          ** acquisition failure.
          ** Inform system determination of pilot acquisition failure.
          ** Determine the next state to be processed based on returned
          ** SS-Action.
          ** ----------------------------------------------------------*/
          if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                  sd_ss_ind_cdma_acq_failed_unlock_rf( NULL ) )
             )
          {
            MSG_HIGH ( "SRCH request sys restart", 0, 0, 0 );

            mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
            next_state = CDMA_EXIT;
          }
        }

        else
        {
          MSG_MED("Unexpected SRCH rpt %d", srch_ptr->hdr.rpt, 0, 0);
        }

        /* Place item back on queue and set signal if requested */
        cmd_done( &srch_ptr->hdr.rpt_hdr );
      }
    } /* end else if ((sigs & MCC_SRCH_Q_SIG) != 0) */

    else if ((sigs & MCC_STATE_TIMER_SIG) != 0)
    {
      (void) rex_clr_sigs( &mc_tcb, MCC_STATE_TIMER_SIG );

      if ( !BIO_GET_SW( BIO_SW7_M ) )
      {
        /* -------------------------------------------------------------
        ** If the mobile station does not receive a valid Sync Channel
        ** Message within T21m seconds, the mobile station shall enter
        ** the System Determination Substate with an acquisition failure
        ** indication.
        ** ------------------------------------------------------------- */
        #ifndef FEATURE_ERRLOG_DEBUG
        MSG_HIGH( "Sync Channel acquisition failed", 0, 0, 0 );
        #else /* FEATURE_ERRLOG_DEBUG */
        ERR( "Sync Channel acquisition failed", 0, 0, 0 );
        #endif /* FEATURE_ERRLOG_DEBUG */

        /* -------------------------------------------------------------
        ** Inform system determination sync channel acquisition failure.
        ** Determine the next state to be processed based on SS-Action.
        ** ------------------------------------------------------------- */
        if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                sd_ss_ind_cdma_acq_failed_sync( NULL ) )
           )
        {
          mcc_exit.exit = MCC_ACQ_FAILED;
          next_state = CDMA_EXIT;
        }

      } /* end if not BIO_SW7 */

      /* Event report: Timer T21m expired */
      mclog_report_event_timer(CAI_TIMER_T21M);

    } /* end else if ((sigs & MCC_STATE_TIMER_SIG) != 0) */
  } /* end while (next_state == CDMA_SYNC_MSG) */

  return (next_state);

} /* cdma_sync_msg */

/*===========================================================================

FUNCTION CDMA_SYNC_ACQ

DESCRIPTION
  This function performs the Sync Channel Acquisition substate operations.
  During this substate the mobile station attempts to receive and process
  a Sync Channel message.

DEPENDENCIES
  None.

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_sync_acq( void )
{
  /* request that Receive task enter Sync mode */
  mcc_rx_buf.sc.hdr.command = RXC_SC_F;
  mcc_rxc_cmd( &mcc_rx_buf );   /* send command to receive task */

  /* request that Searcher task enter Sync mode */
  mcc_srch_buf.sc.hdr.cmd = SRCH_SC_F;
  mcc_srch_cmd( &mcc_srch_buf );

  /* --------------------------------------------------------------
  ** Update database code channel.  Note: The Sync Channel shall be
  ** assigned code channel number 32 (IS-95 section 7.1.3.1.8).
  ** -------------------------------------------------------------- */
  db_value.code_channel = 32;
  db_put( DB_CODE_CHANNEL, &db_value );


  return (CDMA_SYNC_MSG);  /*lint !e641 */

} /* cdma_sync_acq */

/*===========================================================================

FUNCTION CDMA_PIL_ACQ

DESCRIPTION
  This function performs the Pilot Channel Acquisition substate operations.
  During this substate the mobile station attempts to acquire and
  synchronize to a Pilot Channel.

DEPENDENCIES
  None.

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_pil_acq( void )
{
  mccsrch_rpt_type *srch_ptr;    /* Pointer to report from Searcher task */
  word next_state;               /* Next state to be processed */
  static mc_msg_type *cmd_ptr;   /* Command from the MC_CMD_Q queue */
  dword sigs;                    /* Return signals from mcc_wait() call */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Request that Searcher task enter Acquire mode */
  mcc_srch_buf.acq.hdr.cmd   = SRCH_ACQ_F;
  mcc_srch_buf.acq.cdma_freq = cdma.cdmach;
  mcc_srch_buf.acq.band_class = cdma.band_class;

  mcc_srch_cmd( &mcc_srch_buf );
  /* The timer cai_tmo.t20m is not a real requirmeent since there is no
   * requirement on when the acquisition actually starts.  Since 1x may 
   * now be denied the RF lock for long periods of time, it isn't 
   * appropriate for the protocol to time out on an acquisition attempt.
   * Once the searcher hardware actually starts an acquisition attempt
   * it is guaranteed to finish in much less than 15s.
   */
  next_state = CDMA_PIL_ACQ;  /*lint !e641 */

  while (next_state == CDMA_PIL_ACQ)  /*lint !e641 */
  {
    sigs = mcc_wait( MCC_SRCH_Q_SIG | MCC_STATE_TIMER_SIG |
                     MCC_CMD_Q_SIG );

    if ((sigs & MCC_CMD_Q_SIG) != 0)
    {
      if ((cmd_ptr = (mc_msg_type*) q_get( &mc_cmd_q )) != NULL)
      {
        next_state = mcc_ui_ini( cmd_ptr );
        cmd_done( &cmd_ptr->hdr.cmd_hdr );
      }
    } /* end else if ((sigs & MCC_CMD_Q_SIG) != 0) */

    else if ((sigs & MCC_SRCH_Q_SIG) != 0)
    {
      if ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL)
      {
        if (srch_ptr->hdr.rpt == SRCH_ACQ_R)
        {
          /* Searcher has acquired - go wait for a sync message */
          next_state = CDMA_SYNC_ACQ;  /*lint !e641 */
        }
        else if (srch_ptr->hdr.rpt == SRCH_ACQFAIL_R)
        {
          /* Searcher has failed acquisition */
          if ( !BIO_GET_SW( BIO_SW7_M ) )
          {
            MSG_HIGH ( "CDMA pilot acquisition failed", 0, 0, 0 );
            /* ----------------------------------------------------------
            ** Inform system determination of pilot acquisition failure.
            ** Determine the next state to be processed based on returned
            ** SS-Action.
            ** ----------------------------------------------------------*/
            if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                    sd_ss_ind_cdma_acq_failed_pilot( NULL ) )
               )
            {
              mcc_exit.exit = MCC_ACQ_FAILED;
              next_state = CDMA_EXIT;
            }
          }
        }

        #ifdef FEATURE_GPSONE_ACQ_MODE
#error code not present
        #endif /* FEATURE_GPSONE_ACQ_MODE */

        else if (srch_ptr->hdr.rpt ==  SRCH_SYS_MEAS_R)
        {
          /* Report system measurements to System Determination */
          mcc_sys_meas_rpt(srch_ptr);
          /* -----------------------------------------------------------
          ** Report system measurement to system determination. Based on
          ** SS-Action, determines the next state to be processed based
          ** on returned SS-Action.
          ** ----------------------------------------------------------- */
          if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
              sd_ss_ind_misc_meas_rprt(srch_ptr->sys_meas.meas_id, TRUE, NULL)))
          {
            mcc_exit.exit = MCC_RESELECTION;
            next_state = CDMA_EXIT;
            event_report(EVENT_PREF_SYS_RESEL);
          }
        }

        else if ( srch_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
        {
          /* ----------------------------------------------------------
          ** Searcher requests system restart in the middle of pilot
          ** acquisition. Treat it as pilot acquisition failure.
          ** Inform system determination of pilot acquisition failure.
          ** Determine the next state to be processed based on returned
          ** SS-Action.
          ** ----------------------------------------------------------*/
          if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                  sd_ss_ind_cdma_acq_failed_unlock_rf( NULL ) )
             )
          {
            MSG_HIGH ( "SRCH request sys restart", 0, 0, 0 );

            mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;

            next_state = CDMA_EXIT;
          }
        }

        /* Put item back on queue if Searcher task requests it */
        cmd_done( &srch_ptr->hdr.rpt_hdr );
      }

    } /* end if ((sigs & MCC_SRCH_Q_SIG) != 0) */

    else if ((sigs & MCC_STATE_TIMER_SIG) != 0)
    {
      (void) rex_clr_sigs( &mc_tcb, MCC_STATE_TIMER_SIG );

      if (!BIO_GET_SW( BIO_SW7_M ))
      {
        /* ---------------------------------------------------------
        ** If the mobile station does not acquire the Pilot Channel
        ** within T20m seconds, the mobile station shall enter the
        ** System Determination Substate with an acquisition failure
        ** indication.
        ** --------------------------------------------------------- */
        MSG_HIGH( "CDMA pilot acquisition failed", 0, 0, 0 );
        /* ----------------------------------------------------------
        ** Inform system determination of pilot acquisition failure.
        ** Determine the next state to be processed based on returned
        ** SS-Action.
        ** ---------------------------------------------------------- */
        if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                sd_ss_ind_cdma_acq_failed_pilot( NULL ) )
           )
        {
          mcc_exit.exit = MCC_ACQ_FAILED;
          next_state = CDMA_EXIT;
        }
      }
      /* Event report: Timer T20m expired */
      mclog_report_event_timer(CAI_TIMER_T20M);

    } /* end else if ((sigs & MCC_STATE_TIMER_SIG) != 0) */
  } /* end while (next_state == CDMA_PIL_ACQ) */

  return (next_state);

} /* cdma_pil_acq */

/*===========================================================================

FUNCTION CDMA_INIT

DESCRIPTION
  This procedure determines which substate of the initialization substate
  should be processed and vectors control to the appropriate procedure
  to process the substate.

DEPENDENCIES
  None.

RETURN VALUE
  Next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_init( void )
{
  while (MAIN_STATE( cdma.curr_state ) == CDMA_INIT)
  {
    /* Event report: Call processing state change */
    mclog_report_event_cp_state(cdma.curr_state);

    #ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
    #endif /* FEATURE_GPSONE_NEW_L2_ACK */

    switch (cdma.curr_state)
    {
      case CDMA_PIL_ACQ:
      {
        /* Pilot Channel Acquisition */
        cdma.curr_state = cdma_pil_acq();
        break;
      }

      case CDMA_SYNC_ACQ:
      {
        /* Sync Channel Acquisition */
        cdma.curr_state = cdma_sync_acq();
        break;
      }

      case CDMA_SYNC_MSG:
      {
        /* Wait for Sync Channel Message */
        cdma.curr_state = cdma_sync_msg();
        break;
      }

      case CDMA_JUMP:
      {
        /* Jump to Hyperspace */
        cdma.curr_state = cdma_jump();
        break;
      }

      default:
      {
        ERR_FATAL( "Bad init substate %d", cdma.curr_state, 0, 0 );
      }
    } /* end of switch */
  } /* end of while */


  return (cdma.curr_state);

} /* cdma_init */
