/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S Y S T E M   D E T E R M I N A T I O N

             S Y S T E M   S E L E C T I O N   S C R I P T

GENERAL DESCRIPTION
  This file makes up the System Selection Script (SS-Script) component of SD.

  An SS-Script is a sequence of SS-Script Constructs. Each SS-Script
  construct is a sequence of the construct name followed by predetermined
  number of parameters.

  An SS-Script shall only be parsed using an SS-Script-parser. The script
  parser provides an interface for parsing scripts and hides the details that
  are associated with the physical layout of the script.

  SS-Scripts are grouped into the following tables:

    User Events Script Table - This table enables associating unique scripts
                               with user events.

    CDMA Acquisition Events
    Script Table             - This table enables associating unique scripts
                               with CDMA acquisition events.

    CDMA Operation Events
    Script Table             - This table enables associating unique scripts
                               with CDMA operation events.


    AMPS Acquisition Events
    Script Table             - This table enables associating unique scripts
                               with AMPS acquisition events.

    AMPS Operation Events
    Script Table             - This table enables associating unique scripts
                               with AMPS operation events.

    HDR User Events
    Script Table             - This table enables associating unique scripts
                               with HDR user events.

    HDR Acquisition Events
    Script Table             - This table enables associating unique scripts
                               with HDR acquisition events.

    HDR Operation Events
    Script Table             - This table enables associating unique scripts
                               with HDR operation events.

    GW Acquisition Events
    Script Table             - This table enables associating unique scripts
                               with GSM/WCDMA acquisition events.

    GW Operation Events
    Script Table             - This table enables associating unique scripts
                               with GSM/WCDMA operation events.

    WLAN User Events
    Script Table             - This table enables associating unique scripts
                               with WLAN user events.

    WLAN Acquisition Events
    Script Table             - This table enables associating unique scripts
                               with WLAN acquisition events.

    WLAN Operation Events
    Script Table             - This table enables associating unique scripts
                               with WLAN operation events.

    Miscellaneous Events
    Script Table             - This table enables associating unique scripts
                               with miscellaneous events.

    Internal Events Script
    Table                    - This table enables associating unique scripts
                               with internal events.


    Called scripts table    - This table enables script to call for the
                              activation of other scripts.

  When the SS-Core receives an SS-Event it searches the appropriate script
  table to check whether a script is associated with SS-Event it received.
  The order of searching is from top of table to bottom. Searching stops once
  a match is found or when reaching the end of the script table. In the case
  where a match is found, the matched script becomes the active SS-Script.

EXTERNALIZED FUNCTIONS

  sdssscr_except_get_ref
  sdssscr_dflt_get_ref

  sdssscr_parser_init
  sdssscr_parser_next
  sdssscr_parser_is_contain

INITIALIZATION AND SEQUENCING REQUIREMENTS

  None.

Copyright (c) 2000 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/sd/rel/08H1/src/sdssscr.c#13 $

when       who     what, where, why
--------   --     ----------------------------------------------------------
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
04/22/09   mh      Change service status after NAM change
04/03/09   ak      Disable Sprint BSR for HDR activity.
02/13/09   sn      For user pwr save exit event, in pwr save, continue with 
                          earlier  4 + 1 GW  band group scan sequence.
01/21/09   rk      Changed the script ssscr_user_ss_pref_opr_gw_orig_cs_no_conf
                   to check for only GW band pref change
01/09/09   rm      Reverting the fix - After get_netowrks srv_req with SYNC_UP 
                   should not be sent to NAS. Also make ss_state to OPR 
                   after get_netowrks.
12/25/08   rk      Fixing data resume issue.
11/28/08   rm      In UOOS report Limited service to NAS if sim state is
                   CS_PS_Invalid. Reverting part of the fix for CR-124247 
11/13/08   rm      After get_netowrks srv_req with SYNC_UP should not be sent 
                   to NAS.
11/13/08   ak      UOOS correction to avoid restart scanning on call
                   preferences unforced if no preference conflict.
11/13/08   ak      Corrected the timer wipeout issue which was disrupting the 
                   UOOS timeline.
11/05/08   sv      Moved sdssscr_flag_e_type to sdss_flag_e_type
11/03/08   rk      Try for full service after ending emergency call on FPLMN
10/23/08   ak      Fixing the no firmware scenario
10/09/08   sn/rk   changing the aquistion logic to start from MRU
                   when SRV LOST in 1x_GW mode with GW MRU. 
09/29/08   aj      Fix user pwr up script to not schedule avoided systems
09/29/08   rk      Doing normal service lost silient processing when 
                   domain pref is changed and srv status is FULL srv.
09/26/08   cl      Fix the system to use TMR_HDR_REDIR under DO Redirection
09/25/08   sv      Memory Reductoin changes, featurizing GW scripts
09/15/08   pk      Added support for WLAN UOoS
09/16/08   ak      Revert to OOS in emergency call.
08/29/08   rm      Searching for GW systems in Automatic mode when SIM 
                   is not avilable
08/19/08   ak      Implemented the Sprint EVDO powersave algorithm.
08/12/08   fj      Fixed the lint error.
08/18/08   jq      Fixed the compilation errors due to CL 724596 
                   Rename TMR_ALT_CDMA_BSR_X to SD_ALT_CDMA_BSR_X
08/16/08   np      Added NV timer for deep sleep power save
08/12/08   ak      Implemented the new Sprint BSR algorithm
07/30/08   sv      Added scripts to handle stack sync-up
06/20/08   sn      BSR 2.0 changes
06/25/08   sn      Added new script ssscr_gw_opr_srv_info_lmtd_invalid
06/12/08   aj      Klocwork fixes
06/10/08   aj      When 1x is in acquisition and there is a collocated
                   call origination,ensure that hdr waits
05/06/08   pk      Added support for PWU_UP enh for UMTS UOOS for Multimode 
                   builds
04/28/08   ak      Added sd_ss_ind_cdma_opr_prot_mis & ssscr_hdr_opr_prot_mis
04/28/08   pk/rn   Reset the acq_scan_status if user changes the preferences
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
04/02/08   pk/aj   Added support for Time based hybrid HDR UOOS
03/31/08   ak      Updated the BSR timer as per CDG document
03/19/08   cl      Fixing UOOS Time to support Auto Mode Builds.
03/14/08   rm      CM-NAS interface for new service req type
03/05/08   ak      Added scripts to ensure that SS_PREF(CALL_END) does not 
                   reset reselection timer.
02/20/08   ak      Updated GSM behaviour to avoid unnecessary system 
                   reselection upon call origination.
02/14/08   cl      Hold Reselection Timer if IS_HDR_ACTIVITY is active.
02/13/08   vs      Added scripts to handle call end during hdr power save
                   ssscr_hdr_user_ss_pref_colloc_pwr_save_wakeup_acq_call_end
                   and ssscr_hdr_user_ss_pref_colloc_pwr_save_call_end
01/22/08   rm      Reverted the changes done for- support for SIM Invalid 
                   and UOOS
11/16/07   bh      modified redirection script to handle ndss and srdm
                   as well as incall redirection
11/13/07   rn      Deleted non-complying sys from SRCH_LST in
                   ssscr_called_resel_full_srv_manual
10/11/07   pk/sv   UOOS OTASP fix, 4 seconds after the otasp call ends, 
                   revert the preferences
09/27/07   dm      Fixed CR 124498
                   Fixed ssscr_cdma_opr_sys_lost_orig so that it searches
                   for SRV_SYS first
09/18/07   pk      Added support for non-Hybrid UOOS
08/25/07   jqi/cl  Adjusted Comments.
07/19/07   jqi     Scan the idle GEO system once when there is MAP during 
                   call orig if JCDMA is defined.
06/15/07   rp      Support for handing no service in connected mode
06/06/07   pk      UOOS enhancements cont...
06/06/07   pk      UOOS enhancements...
05/24/07   pk      Added support for SIM Invalid and UOOS cont...
05/23/07   pk      Added support for SIM Invalid and UOOS
05/14/07   pk      Enabling UMTS UOOS ENH and P2 features simultaneously
05/02/07   pk      Reset the gw_acq_scan_status at power up
05/02/07   jqi     DO not turn off the redirection afer IHO if the expected 
                   SID/NID of the redirected system is WILDCARD.
                   Expanded 1x system reselection timer value from 16-bit to
                   32-bit.
04/24/07   jqi     Added reselection state support for JCDMA 1X system
                   reselection.
04/23/07   jqi     JCDMA system reselection modification: Reselection srch 
                   list ordered by PRL not by MRU.
04/23/07   jqi     Added Filtering support for stopping HDR activity.
                   Added support for sd_misc_is_orig_mode.  
04/14/07   rp      Copy srch_lst to temp_lst
03/28/07   jqi     1x 100% PSIS avoidance requirement.
03/01/07   pk      Added support for FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2
02/23/07   ic      Corrected copy/paste error in IS_SAME macro - replaced 
                   SDSR_CONTAIN_##VAL with SDSR_EQUAL_##VAL 
02/12/07   jqi     Kept srv off until OH after failed EGSRDM.
02/08/07   sk      Added measurement during reselection after a redirection
                   for JCDMA builds.
                   Added const to script tables to reduce RAM usage.
01/08/07   sk      Added UOOS support for JCDMA.
01/04/07   pk      Added support for VoIP -> 1X Handoffs
12/01/06   pk      Added support for Automatic mode for CDMA + GSM UOOS 
                   builds
11/29/06   ka      Limited service resel timer expiry results in sending 
                   MEAS_BACK action instead of full service req. Satisfies
                   GCF 6.1.2.6.
11/15/06   pk      Added support for HYBR_GW
11/07/06   sk      Memory reduction effort.
                   - Featurized large scripts.
11/03/06   pk      Added support to default UOOS timers if the NV timers
                   are invalid
10/06/06   jqi     Featureize the change for DO call end reselection timer
                   default value.
10/11/06   jqi     Use preferred acquisition for overreach procedure when
                   FEATURE_JCDMA_2 is defined.
10/02/06   sk      When doing redial, cancel all avoids due to max access
                   probes.
09/22/06   rp      Added BC15/16 support
09/21/06   jqi     Increased default Do call end reselection timer to 190.
09/15/06   ka      Adding ssscr_user_ss_pref_opr_gw_orig_start_cs
09/08/06   sk      Added measurement of system pilot strength to the WLAN
                   power save behavior.
08/29/06   sk      Added WLAN Adhoc support.
08/24/06   pk      Added support for IF construct and GCF test fixes
08/23/06   jqi     Added support for system reselection during BCMCS flow.
08/15/06   pk      Added support for GW UOOS
08/07/06   pk      Added support for 1X UOOS
07/13/06   jqi     Added the system lost reason for HDR system lost event.
07/07/06   sk      Fixed comment.
06/09/06   sk      Fixed handling of power save exit request in GW only
                   targets to make sure the powersave timeline is not reset.
05/23/06   sk      Fixed Max access probe handling to make sure all
                   applicable systems are attempted acquisition before
                   attempting acquisition on systems which experienced a Max
                   access probe.
05/22/06   jqi     Made use of the chan list for HDR oh info.
05/12/06   sk      When searching for alternate systems during silent redial,
                   order the silent redial list according to MRU when.
05/08/06   sk      Do not allow redirection to negative systems for 1x.
                   Turn off redirection indication in HDR when the MS moves
                   to a different system.
04/28/06   sk      Changes all power save scripts to use PWR_SAVE_LST.
                   Removed DEL_CNT from script ssscr_called_hdr_acq_rotate.
04/20/06   jqi     Only start the HDR call end resel timer upon ss pref 
                   change with pref reas = orig end.
04/18/06   sk      Disabled reselection INIT phase by relaxing conditions in
                   the script ssscr_int_resel_init_to_norm.
04/12/06   sk      Added script ssscr_cdma_opr_oh_chan to handle idle handoff
                   (with only channel change) to a less pref system when no
                   reselection is active.
04/09/06   jqi     Increased Register Failure Counter to 6 in over reach.
                   Made "1x Reselection Threshold" as a configurable NV item.
                   Mainline FEATURE_SILENT_REDIAL_20.
                   Connection Deny reason(network busy), avoid the channel
                   60s.
                   No TCA and NoRTC ACK, 3 failures avoid 60s or encountering 
                   ACQ_FAIL, whichever come first.
                   Scan all channels during overreach wake up.
                   Do not cancel avoidance when entering power save mode.
                   Added null pointer check in sdsr_list_is_fade.
                   Mainline FEATURE_SILENT_REDIAL_20
03/27/06   dk      Added wlan power save mode pref script.
03/20/06   sk      Upon optimized silent redial, all same or better preferred
                   systems and all home systems (irrespective of their
                   preference) in the idle geo are attempted acquisition.
                   Upon successful acquisition, the call is allowed to
                   originate.
03/9/06    jqi     DO system reselection optimization(Sys resl at DO MO call
                   end.)
                   Multichannel avoidance.
                   Reset the DO failure counter when Subnet ID is changed.
                   Increased Register Failure Counter to 6 in over reach.
                   Increase 1x Reselection Threshold to -7db.
03/07/06   sk      Upon system lost, order idle geo list according to MRU
                   when doing acquisition attempts.
02/24/06   pk      Modified ssscr_hdr_acq_failed_sync, in hybrid mode, this
                   script will be called. The lost system will be
                   re-attempted twice
01/30/06   sk      Removed special treatment of AMPS systems during emergency
                   calls/modes.
01/16/06   sk      Upon call release and redirection on, attempt acquisition
                   of serving system (traffic channel during call) if it's
                   different from idle system.
                   Fixed call release script for redirection off case to
                   prevent attempting acquisition of the serving system if
                   it's the same as the idle system.
01/09/06   sk      Attempt acquisition of SRV_SYS upon call release.
01/09/06   sk      Removed FEATURE_T53 from script
                   ssscr_cdma_opr_redir_invalid_gsrm_rif
01/09/06   sk      Fixed ssscr_cdma_opr_oh_sid_pref_new to use
                   MORE_PREF_THAN_SRV_SYS instead of MORE_PREF.
12/22/05   jqi     Merged background PLMN search support.
12/21/05   jqi     Added additional 1 minutes avoidance upon session nego
                   timeout.
                   Reduced JCDMA reselection hold timer to 125s.
12/12/05   jqi     Removed the 1x system measurement during power up
                   Deleted unsupported band/chan priory to the 1x system
                   measurement.
                   Unify behavior for 100% persistent failure on 1x.
12/02/05   jqi     Lint 8 fixes.
11/23/05   jqi     Mainline FEATURE_HICPS20 and FEATURE_HICPS30 under
                   FEATURE_HDR.
11/07/05   jqi     Do colloc best pref acquisition for DO wake up acquisition.
11/02/05   sk      Fixed all Lint and compiler warnings.
10/19/05   jqi     Avoid the acq system for HDR opr system lost.
                   Restart the release timer for HDR release event.
10/18/05   jqi     Enable alter scan when there is a system lost for JCDMA.
10/14/05   jqi     Added BC14 support.
10/05/05   sk      Added support for configuring SD values from NV.
09/29/05   pk      Corrected scripts numbering
09/10/05   pk      Changed SD behavior for Data Call Originations on HDR.
                   Added DEL_CNT( LIST, START_POS, CNT ),
                   IS_TIME_GR(INSTANCE, SRV_VAL, TIME) constructs and
                   SDSSSCR_HDR_ACQ_ROTATE, SDSSSCR_HDR_ACQ_ROTATE_SRV_LOST
                   called scripts, added
                   ssscr_hdr_user_ss_pref_colloc_pwr_save_rotate
                   ssscr_hdr_user_ss_pref_colloc_pwr_save_wakeup_acq_rotate
09/06/05   jqi     Only process HDR opr acc_timeout script if the orig mode
                   is collocated.
09/01/05   jqi     DO system reselection optimization.
08/16/05   sk      KDDI 1x reselection and GSRDM requirements.
08/15/05   jqi     More Best preferred acquisition for DO.
08/10/05   jqi     Removed redudant service indication during HDR colloc
                   wake up/pwr save call origination scripts.
08/05/05   jqi     Added support for Best preferred acquisition for DO.
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
                   Changed to TIMEOUT_HDR_SHORT to 130 seconds.
07/11/05   jqi     Added avoidance timer for access failures and fades.
07/08/05   pk      Modified the
                   ssscr_hdr_user_ss_pref_colloc_pwr_save_wakeup_acq and
                   ssscr_hdr_user_ss_pref_colloc_pwr_save scripts to do a
                   full acquisition on HDR after reporting ACQ_FAIL. This is
                   done to optimize the HDR redial mechanism
07/08/05   sk      Defined IS_EVENT_CNT( EVENT, CNT, TIME ).
                   Added telescopic avoidance for session negotiation
                   timeouts during DO.
06/30/05   jqi     Filtered out the back to back call orig when HDR is in
                   power save state.
06/23/05   sk      Removing previous changes. Requirements changed.
06/17/05   sk      Added telescopic avoidance for session abort during DO.
06/06/05   jqi     Minimize 1x miss pages when DO is reverse link limited.
                   Avoidance: 2m(after 1 failure), 4m( after 2), 8m(after 3),
                   16m(after 6).
05/25/05   dk      Fixed WLAN User script for USER_RESEL and USER pref reas.
05/13/05   ka      Added scripts for avoiding WLAN system.
05/16/05   pk      Enhanced CHK_SRV_STATUS_AND_SET script to process
                   Bit masks of the SRV status. The new construct supports
                   sdssscr_srv_status_val_e_type parameters.
05/16/05   pk      Added CHK_SRV_STATUS_AND_SET( CHK_VAL, SRV_VAL ) to
                   MISCELLANEOUS SS-SCRIPT-CONSTRUCTS, the construct checks
                   if the current service indicator is set to CHK_VAL, if
                   it is set to CHK_VAL, sets the service indicator to
                   SRV_VAL
05/16/05   pk      Added SDSSSCR_MAX_LOOP_BACK_CNT, change the number of
                   times the loop is executed.
05/04/05   jqi     Added support to process HDR bad system event while in
                   acquisition state.
03/23/05   jqi     Do not turn off service upon get_network_list request.
01/30/05   sj      Added support for WLAN.
01/27/05   ka      Added support for Band class 10, 11, 12
12/14/06   sj      Do avoid_cancel only on non hdr systems.
12/04/04   jqi     Power save mode optimization for GSM/UMTS targets.
10/26/04   jqi     Allowed system reselection and tried on limited srv before
                   entering power save mode in a dormant ps call.
10/21/04   RC      Added support for FEATURE_ADD_HDR_IF_IS683A_PRL.
09/23/04   jqi     Clear GW limited system in internal service lost scripts.
09/21/04   jqi     Service status optimization in idle handle off.
09/08/04   jqi     Added ps dormant optimization.
08/31/04   sj      Added condition to check for dual mode targets.
08/16/04   sj      Enter power save in GW targets, if no service is acquired
                   while in normal or emerg. call origination.
08/03/04   sj      Do not turn off service for _pwr_up event.
07/29/04   jqi     Quadband optimizatioin - eliminate scheduling.
07/15/04   sj      Do attach ondemand for ps_service if not acquired
                   ps_service.
07/14/04   sj      Added support for construct to check for current registered
                   srv_domain.
                   Added support for contruct to check if the srv_domain_pref
                   is CS+PS.
                   Added scripts to initiate looking for full service when
                   making normal calls in limited service.
07/08/04   dk      Added script to handle Domain Change preference for CR
                   44112.
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
06/23/04   jqi     Only clear the GW last full system in power up.
06/21/04   RC      Improve association (collocation) determination algorithm
                   for HDR systems while in hybrid operation - CR 44638.
06/21/04   jqi     Use sd nam selection indication when there is a
                   subscription change.
06/07/04   jqi     More quad-band enhancement for automatic mode.
06/02/04   sj      On powerup check if in emergency mode.
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
05/19/04   SJ      Added changes to not request limited service for emergency
                   calls when there is service already.
04/27/04   RC      Added ssscr_cdma_opr_acc_term_traffic[] to support CDMA
                   traffic to idle transition (without calling release ind.).
03/08/04   SJ      Added 420 sec avoid time, if access is denied on colloc
                   hdr system.
01/23/04   JQ      Passed down most recent acquired band group to GW stack
                   during early power scan.
01/22/04   JQ      Changed SDSR_GW_LTD_SYS and SDSR_USER_SYS to lists with
                   appropriate grouping of GW bands.
01/22/04   SJ      While in GW opr, do acquisition only if preferences have
                   changed.
01/15/04   RC      Added support for FEATURE_SILENT_REDIAL_20
01/13/04   SJ      Updated to do wakeup colloc acquisitions only if temp_sys
                   is not empty.
12/16/03   RC/AAJ  Support for FEATURE_HDR_HICPS20
12/08/03   SJ      Added support for multistage full srv reselection.
11/17/03   SJ      Updates for HDR usability and new hdr power save.
11/10/03   RC      Added support for SDSSSCR_CON_IS_NET_SEL script construct.
11/05/03   RC      Fixed a problem where SRCH_LST was empty in JCDMA
                   over-reach.
11/03/03   RC      Added JCDMA support in ssscr_cdma_opr_acc_term_acc_den[]
                   and ssscr_cdma_opr_rescan[].
10/31/03   RC      Added GSM/WCDMA support for fall-back to limited service.
09/24/03   RC      Delay reselection by 1s after SS_PREF, so that SS-Client
                   can indicate whether reselection can be performed.
09/23/03   RC      Most recent system is now attempted acquisition
                   periodically during emergency origination mode - CR33792.
09/23/03   RC      Deleted ssscr_user_ss_pref_idl_not_mru() - CR34907.
08/06/03   SJ      Added srv_off before early power scan request.
08/05/03   SJ      Added defines for power save enter & wakeup.
07/02/03   RC      In ssscr_user_ss_pref_acq_orig_emerg clear SCHED_LST and
                   schedule acquisition conditionally.
06/23/03   RC      If CAND_SYS is empty, schedule IDL_SYS for reacquisitions
                   in ssscr_user_ss_pref_emerg_list_empty.
06/23/03   RC      Enhanced HDR wildcard subnet-ID behavior.
06/19/03   SJ      Perform service lost processing if pref_reason = USER_RESEL
                   while handling pref. sys. changed.
06/10/03   RC      Added counter to track CDMA available indications.
06/06/03   RC      Changed ssscr_xxx_opr_sys_lost to scan for more preferred
                   systems when reacquiring lost system.
06/05/03   RC      Enhanced CAM to CDMA paging channel behavior - modified
                   ssscr_cdma_opr_cam and ssscr_cdma_opr_oh_sid_handoff.
05/28/03   RC      Upon idle handoff leave forbidden HDR system immediately.
05/28/03   RC      Optimized handling of origination during acquisition.
05/16/03   RC      Added ssscr_user_ss_pref_vrfy_orig.
04/23/03   SJ      Added early power scan support for GSM/WCDMA.
04/04/03   SJ      Removed check for expected PLMN for manual PLMN selection.
03/28/03   RC      Modified scripts to move away from AMPS system if access
                   terminates unsuccessfully in emergency origination.
03/18/03   RC      Avoid HDR sys for 60s instead of 180s on max acc probes.
03/21/03   SJ      Added pref sys change script when we have service other
                   CDMA/AMPS/HDR.
03/18/03   SJ      Updated manual PLMN selection to not check for preferences
                   and negative systems.
03/12/03   SJ      Wait for 120 seconds before starting reacquisition after
                   get networks operation.
03/07/03   SJ      Changed MS level for few debug messages.
03/04/03   SJ      Added support for limited automatic mode.
02/05/03   SJ      Added power_save script for manual mode.
01/23/03   SJ      Modified ssscr_user__pwr_up script to do additional
                    cleanup.
01/20/03   jqi     Added support for band class 6.
01/16/03   SJ      Featurized CDMA scripts and corrected manual system
                   selection to check for preferences.
12/09/02   RC      Added support for FEATURE_JCDMA.
12/03/02   SJ      Merged in power save changes for HDR.
11/07/02   RC      Added support for new hybrid-HDR power-save algorithm.
10/25/02   RC      Added ssscr_cdma_opr_oh_sid_conf_mode script to enable
                   changing the mode preference to HDR at end of CDMA call.
08/28/02   RC      Fixed a typo in ssscr_hdr_acq_failed_sync (CDMA -> HDR).
07/31/02   RC      Optimized GPS SS-Preference related changes.
07/10/02   RC      Optimized power up acquisition with PRL containing 1 chan.
06/10/02   RC      Added ssscr_hdr_user_ss_pref_opr_colloc[] script.
05/16/02   RC      Deleted the ssscr_int_srv_lost_colloc_empty[] script.
05/07/02   RC      Avoid ACQ_SYS for 180s upon max access (orig_mode=COLLOC).
05/02/02   RC      Changed some SD_MSG_HIGH to SD_MSG_LOW.
04/02/02   RC      Clear SRV_COLLOC_LST upon 1x service lost or power save.
03/19/02   RC      Avoid HDR system for 600s when getting session abort.
03/12/02   RC      Use SRV_SYS instead of IDL_SYS in periodic reselection.
03/07/02   RC      Now reacquiring redirection system after call is released
                   even if this system conflicts with the SS-Preferences.
02/23/02   RC      Added support for Rel. 3.0 CDMA-HDR hybrid operation.
01/18/02   az      Modified/Added scripts for Silent redial, Max Access
                   probes and called scripts.
01/09/01   RC      Now performing reselection immediately upon idle-handoff
                   to a new SID/NID.
12/12/01   RC      Added support for CDMA-HDR hybrid operation.
12/07/01   jqi     Added support for band class 5.
10/30/01   RC      Added support for IS-683C collocated systems.
09/07/01   RC      Added support for T53 and JCDMA features.
07/24/01   RC      Aligned HDR periodic reselection with CDMA reselection.
05/23/01   RC      Eliminated compiler warning.
04/26/01   RC      Modified reacquisition schedules.
04/24/01   RC      More changes to support tri-mode and HDR targets.
03/27/01   RC      Changes to support tri-mode targets.
03/07/01   RC      Changes per 11 week of I&T.
02/26/01   RC      Changes to support dual-mode targets.
02/07/01   RC      Changes per 9th week of I&T.
02/01/01   RC      Changes per 8th week of I&T.
01/12/01   RC      Changes per 7th week of I&T.
01/12/01   RC      Changes per 6th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
12/14/00   RC      Changes per 3rd week of I&T.
12/04/00   RC      Changes per 2nd week of I&T.
11/27/00   RC      Changes per 1st week of I&T.
11/17/00   RC      Initial release.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "sdssscr.h"  /* External interface to sdssscr.c (SS-Script) */
#include "sdss.h"     /* External interface to sdss.c (System Selection) */
#include "sdsr.h"     /* External interface to sdsr.c (System Record) */
#include "sdprl.h"    /* External interface to sdprl.c */
#include "sd.h"       /* External interface to sd.c */
#include "sdi.h"      /* Internal interface to sd.c */
#include "sddbg.h"    /* SD debug macros */
#include "sdnv.h"     /* External interface to sd.c */

#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */

/*lint --e{641,750} */ /* Converting enum to int */ /* macro not used */
/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Set power save enter and wakeup times if not already defined.
*/
#ifndef SD_PWR_SAVE_ENTER_TIME
#define SD_PWR_SAVE_ENTER_TIME      900
#endif

#ifndef SD_PWR_SAVE_ENTER_TIME_DURING_CALL
#define SD_PWR_SAVE_ENTER_TIME_DURING_CALL     30
#endif

#ifndef SD_PWR_SAVE_WAKEUP_TIME
#define SD_PWR_SAVE_WAKEUP_TIME     180
#endif

#define RESEL_TIMEOUT_SS_PREF        1
#if defined( FEATURE_SD_ALT_CDMA_BSR_SCAN_1 )
#error code not present
#endif

/* Default values for the configurable SD values stored in NV.
*/
static const nv_sd_cfg_items_s_type sd_cfg_items_default = {

  /* Version. Modify this ONLY if you are deleting any enum from
  ** sdssscr_nv_item_e_type. Never decrement the version number.
  */

  1,

  /* Total number of entries used in the NV array.
  */
  SDSSSCR_NV_ITEM_MAX,

  /* Default values for all the items stored in the NV array. Default values
  ** are used (and written back to NV) ONLY under the following conditions:
  **
  ** 1. The entire structure is not present in the NV.
  ** 2. The version number in the NV is different from the version number in
  **    this structure (a few lines above).
  ** 3. When the entry count in the NV is less than the entry count in this
  **    structure. In which case, the default values are used ONLY for the
  **    new entries. The old entries retain the value stored in the NV.
  **
  ** Also, changes to default values take effect ONLY if the conditions
  ** mentioned above are met.
  */
  {
    /* 0 */
    #ifdef SD_TARGET_DUALMODE
    120,      /* TMR_AMPS_RESEL */
    #else
    60,       /* TMR_AMPS_RESEL */
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifndef FEATURE_JCDMA
    180,      /* TMR_CDMA_RESEL */

    5,       /* TMR_CDMA_RESEL_HOLD_CALL_END */

    1,        /* TMR_CDMA_RESEL_HOLD_IDLE_HOFF */

    /* 4 */
    180,      /* TMR_CDMA_REDIR */

    #else
    3600,     /* TMR_CDMA_RESEL */

    125,      /* TMR_CDMA_RESEL_HOLD_CALL_END */

    125,      /* TMR_CDMA_RESEL_HOLD_IDLE_HOFF */

    /* 4 */
    86340,    /* TMR_CDMA_REDIR */
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    3600,     /* TMR_HDR_RESEL*/

    130,      /* TMR_HDR_RESEL_HOLD */

    3600,     /* TMR_HDR_REDIR */

    #ifdef FEATURE_JCDMA_2

    /* 8 */
    190,      /* TMR_HDR_RESEL_HOLD_CALL_END */
    #else
    /* 8 */
    130,      /* TMR_HDR_RESEL_HOLD_CALL_END */
    #endif

    14,       /* CDMA_PI_THR */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_UNIFORM_SCAN_OOS
    36,       /* TMR_UOOS_SLEEP */

    4,        /* TMR_UOOS_SCAN  */
    #else
    0,        /* TMR_RESERVED_1 */

    0,        /* TMR_RESERVED_2 */
    #endif /* FEATURE_UNIFORM_SCAN_OOS */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
    #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
    #else

    /* 12 */
    0,        /* TMR_RESERVED_3 */

    0,        /* TMR_RESERVED_4 */

    0,        /* TMR_RESERVED_5 */

    0,        /* TMR_RESERVED_6 */
    #endif /* #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* 16 */
    180,      /* TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY */

    253,      /* WLAN_PI_THR */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING_PWR_UP_ENH
#error code not present
    #else
    0,        /* TMR_RESERVED_7 */
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_UNIFORM_SCAN_OOS_HDR_ENH
    4,        /* TMR_UOOS_HDR_SCAN */  
    #else
    0,        /* TMR_RESERVED_8 */
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* 20 */
    300,     /* TMR_PWR_SAVE_COLLOC */  

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_WLAN_UNIFORM_SCAN_OOS
#error code not present
    #else
    0,        /* TMR_RESERVED_9 */
    #endif

    0,0,0
  }
};

static sdssscr_sd_cfg_items_s_type sd_cfg_items;


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== SYSTEM SELECTION SCRIPT CONSTRUCTS =====================
=============================================================================
=============================================================================
===========================================================================*/

/* A System Selection Script (SS-Script) is built out of System Selection
** Script Constructs (SS-Script-Constructs). Each such construct takes a
** predetermined number of parameters and serves a specific purpose.
**
** The SS-Script constructs are specified using C macros, as follows:
*/


/*===========================================================================
=============================================================================
==================== LIST MANIPULATOR SS-SCRIPT-CONSTRUCTS ==================
=============================================================================
===========================================================================*/


/*===========================================================================

CLR( LIST )

DEFINITION
  Clear the system record list/item  LIST.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the
  actual system records that are stored in the system record table).

CONTROL
  Script execution continues immediately with next script construct
  (i.e., does not return control to the SS-Client).

APPLICABLE
  All record lists/items, but the PRL_LST, MRU_LST, AMPS_XXX_SYS.

EXAMPLE
  CLR( AVAIL_LST ) - Clear the SDSR_AVAIL_LST; that is, after, this script
  construct is executed the number of elements in the SDSR_AVAIL_LST is 0.

===========================================================================*/
#define CLR( LIST )                   SDSSSCR_CON_CLR, \
                                        SDSR_##LIST,


/* <EJECT> */
/*===========================================================================

CPY( LIST_TO, LIST_FROM )

DEFINITION
  Clear the system record list LIST_TO and copy all the elements from the
  system record list LIST_FROM into the system record list LIST_TO.

SCOPE
  Only affects the system record list LIST_TO (i.e., does not affect the
  actual system records that are stored in the system record table or the
  system record list LIST_FROM).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST only as LIST_TO.  All record lists/items as LIST_FROM.

EXAMPLE
  CPY( SRCH_LST, MRU_LST ) - Clears the SDSR_SRCH_LST list and copy all the
  elements from the SDSR_MRU_LST list into the SDSR_SRCH_LST list.

===========================================================================*/
#define CPY( LIST_TO, LIST_FROM )     SDSSSCR_CON_CPY, \
                                        SDSR_##LIST_TO, \
                                        SDSR_##LIST_FROM,


/* <EJECT> */
/*===========================================================================

DEL( LIST, LIST_REF )

DEFINITION
  Delete the system records that are referenced by the system record list
  LIST_REF from the system record list LIST.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the
  actual system records that are stored in the system record table or the
  system record list LIST_REF).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST only as LIST.  All record lists/items as LIST_REF.

EXAMPLE
  DEL( SRCH_LST, SRV_SYS ) - Delete the system record that is referenced by
  SDSR_SRV_SYS from the system record list SDSR_SRCH_LST.

===========================================================================*/
#define DEL( LIST, LIST_REF )         SDSSSCR_CON_DEL, \
                                        SDSR_##LIST, \
                                        SDSR_##LIST_REF,



/* <EJECT> */
/*===========================================================================

DEL_CNT( LIST, START_POS, CNT )

DEFINITION
  Delete CNT entries beginning from START_POS from LIST

SCOPE


CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  ALL lists

EXAMPLE
  DEL_CNT( SRCH_LST, 0, 2) - Deletes the first two entries from SRCH_LST

===========================================================================*/
#define DEL_CNT( LIST, START_POS, CNT ) SDSSSCR_CON_DEL_CNT, \
                                        SDSR_##LIST, \
                                        START_POS, \
                                        CNT,


/* <EJECT> */
/*===========================================================================

DEL_SYS( LIST, DEL_CAT )

DEFINITION
  Delete system records from LIST according to DEL_CAT.

  DEL_CAT can take on one of the following values:

    NON_AMPS    - Delete non AMPS systems.
    NON_CDMA    - Delete non CDMA systems.
    NON_HDR     - Delete non HDR systems.
    GW_SYS      - Delete GSM/WCDMA systems.
    NON_CELL    - Delete non cellular systems.
    NON_COMPLY  - Delete the systems that do not comply with target
                  capabilities and current mode and band preference.
    HDR         - Delete HDR systems.
    NON_WLAN

SCOPE
  Only affects the system record list LIST (i.e., does not affect the
  actual system records that are stored in the system record table or the
  system record list LIST_REF).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST only as LIST.  All record lists/items as LIST_REF.

EXAMPLE
  DEL_SYS( SRCH_LST, NON_CDMA ) - Delete all non-CDMA system records from the
  system record list SDSR_SRCH_LST.

===========================================================================*/
#define DEL_SYS( LIST, DEL_CAT )      SDSSSCR_CON_DEL_SYS, \
                                        SDSR_##LIST, \
                                        SDSR_CAT_##DEL_CAT,


/* <EJECT> */
/*===========================================================================

DEL_BAND( LIST, SYS_REF )

DEFINITION
  Delete the system records from LIST, which has differnt band as in
  reference system.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the
  actual system records that are stored in the system record table or the
  system record list LIST_REF).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST only as LIST.  All record lists/items as LIST_REF.

EXAMPLE
  DEL( SRCH_LST, ACQ_SYS ) - Delete the system record in SDSR_SRCH_LIST
  whose band is different from that in SDSR_ACQ_SYS

===========================================================================*/
#define DEL_BAND( LIST, SYS_REF )    SDSSSCR_CON_DEL_BAND, \
                                        SDSR_##LIST, \
                                        SDSR_##SYS_REF,


/* <EJECT> */
/*===========================================================================

APP( LIST_TO, LIST_FROM )

DEFINITION
  Append the elements from the system record list/item LIST_FROM to the end
  of system record list LIST_TO.  Duplicate elements are not appended.

SCOPE
  Only affects the system record list LIST_TO (i.e., does not affect the
  actual system records that are stored in the system record table or the
  system record list LIST_FROM).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST only as LIST_TO.  All record lists/items as LIST_FROM.

EXAMPLE
  APP( SRCH_LST, MRU_LST ) - Append all the elements from the SDSR_MRU_LST
  list to the end of SDSR_SRCH_LST list.

===========================================================================*/
#define APP( LIST_TO, LIST_FROM )     SDSSSCR_CON_APP, \
                                        SDSR_##LIST_TO, \
                                        SDSR_##LIST_FROM,


/* <EJECT> */
/*===========================================================================

ORDER( LIST, LIST_REF )

DEFINITION
  Order the elements of the system record list LIST according to their order
  in the reference list LIST_REF.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the actual
  system records that are stored in the system record table).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST.

EXAMPLE
  ORDER( AVAIL_LST, PRL_LST ) - Order the elements of the SDSR_AVAIL_LST list
  according to their order in the SDSR_PRL_LST list.

===========================================================================*/
#define ORDER( LIST, LIST_REF )       SDSSSCR_CON_ORDER, \
                                        SDSR_##LIST, \
                                        SDSR_##LIST_REF,


/* <EJECT> */
/*===========================================================================

ORDER2( LIST, LIST_REF, ORDER_CAT )

DEFINITION
  Order the elements of the system record list LIST according to the
  reference list LIST_REF based on the order categories.

  ORDER_CAT can take on one of the following values:

    BAND   - system bands of the system record.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the actual
  system records that are stored in the system record table).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST.

EXAMPLE
  ORDER2( PRL_LST, IDL_SYS, BAND  ) - Order the elements of the
  SDSR_PRL_LST list according to the band preference order in the PRL_LST.

===========================================================================*/
#define ORDER2( LIST, LIST_REF, ORDER_CAT)       SDSSSCR_CON_ORDER2, \
                                                  SDSR_##LIST, \
                                                  SDSR_##LIST_REF, \
                                                  SDSR_ORDER_##ORDER_CAT,
/* <EJECT> */
/*===========================================================================

SORT( LIST, COMP_CAT )

DEFINITION
  Sort the system record list LIST according to COMP_CAT.

  COMP_CAT can take on one of the following values:

    RXPWR     - according to Rx-power measurements.
    PILOT     - according to Pilot strength measurements.
    AMPS      - AMPS systems before other systems.
    CDMA      - CDMA systems before other systems.
    HDR       - HDR systems before other systems.
    DIGITAL   - Digital systems before other systems.
    BC0       - Band Class 0 systems before other systems.
    BC1       - Band Class 1 systems before other systems.
    ROAM      - Favorable roaming indication systems before other systems.
    CELL_A    - Cellular A systems before other system.
    CELL_B    - Cellular B systems before other system.
    CELL_HOME - Cellular home-side systems before other system.
    GW        - GSM/WCDMA systems before other system.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the actual
  system records that are stored in the system record table).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST

EXAMPLE
  SORT( SRCH_LST, RXPWR ) - Sort all the systems in the SDSR_SRCH_LST list
  in descending Rx-power order.

===========================================================================*/
#define SORT( LIST, COMP_CAT )        SDSSSCR_CON_SORT, \
                                        SDSR_##LIST, \
                                        SDSR_COMP_##COMP_CAT,


/* <EJECT> */
/*===========================================================================

GEO_EXT( LIST, LIST_REF, EXT_CAT )

DEFINITION
  Extract systems from the GEO of LIST_REF[0] (i.e., 1st system on LIST_REF)
  into LIST according to the EXT_CAT category.

  EXT_CAT can take on one of the following values:

  ROAM_COMPLY   - Indicate that only GEO systems that comply with the current
                  roaming preferences should be copied into LIST.

  SAME_BETTER   - Indicate that only GEO systems that are of same or better
                  preference (than LIST_REF[0]) should be added to LIST.

  ALL           - Indicate that all GEO systems should be copied into LIST.


SCOPE
  Only affects the system record list LIST (i.e., does not affect the actual
  system records that are stored in the system record table).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST

EXAMPLE
  GEO_EXT( SRCH_LST, IDL_SYS, ROAM_COMPLY ) - Copy into SDSR_SRCH_LST systems
  form the GEO of the system record that is referenced by SDSR_IDL_SYS and
  comply with the current roaming preference.

===========================================================================*/
#define GEO_EXT( LIST, LIST_REF, EXT_CAT )  SDSSSCR_CON_GEO_EXT, \
                                              SDSR_##LIST, \
                                              SDSR_##LIST_REF, \
                                              SDPRL_EXTRACT_##EXT_CAT,


/* <EJECT> */
/*===========================================================================

CREATE_GW_LTD( LIST )

DEFINITION
  Create a limited system couterpart from the system record that is
  referenced by LIST. SET LIST to reference the newly created record(s).

SCOPE
  Only affects LIST.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items. It is only recommended to use this constract on the
  TEMP_SYS or SRCH_LST.

EXAMPLE
  CREATE_GW_LTD( TEMP_SYS ) - Create a counterpart limited system from
  TEMP_SYS and set TEMP_SYS to reference the new system record.

===========================================================================*/
#define CREATE_GW_LTD( LIST )         SDSSSCR_CON_CREATE_GW_LTD, \
                                        SDSR_##LIST,


/* <EJECT> */
/*===========================================================================

FILTER( LIST )

DEFINITION
  Filter out the system that does not compile with the targer capability and
  user preference from the list.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the actual
  system records that are stored in the system record table).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST

EXAMPLE
  FILTER( SRCH_LST ) - Filter out the systems that does not compile with the
  target capability and user preference from the list.

===========================================================================*/
#define FILTER( LIST )                SDSSSCR_CON_FILTER, \
                                        SDSR_##LIST,


/* <EJECT> */
/*===========================================================================
=============================================================================
=================== LIST OPERATOR SS-SCRIPT-CONSTRUCTS ======================
=============================================================================
===========================================================================*/


/*===========================================================================

MEAS( LIST, MEAS_MODE, MEAS_TYPE )

DEFINITION
  Get a channel strength measurement of mode MEAS_MODE and type MEAS_TYPE for
  all the systems that are referenced by the system record list LIST.

  MEAS_MODE can take on one of the following values:

    DED   - Dedicated mode measurement. The Searcher abort any protocol
            related activity and delicately work on completing the channel
            strength measurement report.

    BACK  - Background mode measurement. In this mode the Searcher
            multiplexes channel strength measurement with protocol activity
            in a way that is not intrusive to the protocol activity (similar
            to neighbor list searching). The background mode is 
            available during CDMA/HDR idle and on GSM/UMTS when in Limited
            service.

  MEAS_TYPE can take on one of the following values:

    RXPWR  - Rx-power measurement.
    PILOT  - Pilot-strength measurement.
    NETLIST - Measurement based on network list.

SCOPE
  Only affects the channel strength measurement field of the actual system
  records that are stored in the system record table and referenced by LIST.
  (does not alter the LIST itself).

CONTROL
  Return control to the SS-Client, which then needs to submit the measurement
  request to Search. When Search comes back with the measurement report,
  the SS-Client should call on the SD_SS_IND_MISC_MEAS_RPRT SS-Indication
  function, at which time script execution continues with next script
  construct.

APPLICABLE
  All record lists.

EXAMPLE
  MEAS( IDL_PREF_LST, DED, RXPWR ) - Request dedicated mode Rx-power
  measurement report for all the systems that are referenced by the
  SDSR_IDL_PREF_LST. When the report is completed by Search, the SS-Client
  calls on the SD_SS_IND_MISC_MEAS_RPRT SS-Indication function.

===========================================================================*/
#define MEAS( LIST, MEAS_MODE, MEAS_TYPE )  SDSSSCR_CON_MEAS, \
                                              SDSR_##LIST, \
                                              SDSS_MEAS_MODE_##MEAS_MODE, \
                                              SD_SS_MEAS_TYPE_##MEAS_TYPE,


/*===========================================================================

ACQ_MODE( MODE )

DEFINITION
  Set the CDMA acquisition mode to MODE.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  ACQ_MODE( MINI ) - Set the CDMA acquisition mode to mini.

===========================================================================*/
#define ACQ_MODE( MODE )              SDSSSCR_CON_ACQ_MODE, \
                                        SDSS_ACQ_MODE_##MODE,


/* <EJECT> */
/*===========================================================================

ACQ( LIST, ACQ_TYPE )

DEFINITION
  Step through the system record list LIST (first to last) and try to acquire
  systems that are referenced by LIST and that were not already attempted
  acquisition since the last time the NEW_ACQ construct was encountered (and
  in accordance with ACQ_TYPE and the current SS-Preference).

  ACQ_TYPE can take on one of the following values:

    ANY: Try to fully acquire any system that complies with the current
    SS-Preference (without regard to PRL designation).

    ALLOWED: Try to fully acquire any system that is allowed by the PRL and
    complies with the current SS-Preference.

    PREFERRED: Try to fully acquire only a preferred PRL system that complies
    with the current SS-Preference.

    BEST_PREF: Try to fully acquire only a preferred PRL system that complies
    with the current SS-Preference in a two step process, as follows:

      1. Step through the system record list LIST (first to last) and
         try to acquire systems that are referenced by LIST.  Whenever a
         system from the PRL is acquired, continue with step 2.

      2. Try to acquire more preferred systems from the GEO of the system
         that was acquired in step 1 (most to last).

    MORE_PREF: Try to fully acquire only a system that is more preferred than
    the serving system and complies with the current SS-Preference.

    NOT_LESS_PREF: Try to fully acquire only a system that is not less
    preferred than the serving system and complies with the current
    SS-Preference.

    SPECIFIC: Try to fully acquire a specific system for which the SID/NID is
    known (for example systems that were included in a redirection message).

    SPECIFIC_PRL: Try to fully acquire a specific system for which the
    SID/NID is known (for example systems that were included in channel
    assignment message) and is allowed by the PRL.

    SPECIFIC_PREF: Try to fully acquire a specific system for which the
    SID/NID is known (for example a system that is included in a redirection)
    and complies with the current SS-Preference.

    COLLOC_PREF: Try to fully acquire a system that is associated
    (collocated) with the SDSR_IDL_SYS system and complies with the current
    SS-Preference

    COLLOC_MORE_PREF: Try to fully acquire a system that is associated
    (collocated) with the SDSR_IDL_SYS system and complies with the current
    SS-Preference and is more preferred than the serving system.

    COLLOC_BEST_PREF: Try to fully acquire only a preferred PRL system that
    is associated(collocated) with SDSR_IDL_SYS systme and complies with the
    current SS-Preference in a two step process, as follows:

      1. Step through the system record list LIST (first to last) and
         try to acquire systems that are referenced by LIST.  Whenever a
         system from the PRL is acquired, continue with step 2.

      2. Try to acquire more preferred systems from the GEO of the system
         that was acquired in step 1 (most to last).

    SAME_AS_LAST: Same as last acquisition attempt.

SCOPE
  Only affects the SS-Indication field of the actual system records that are
  stored in the system record table and referenced by LIST.  Does
  not alter the LIST itself.

CONTROL
  For each system that is referenced by LIST, control is returned to the
  SS-Client, which needs to try acquiring the selected system and then to
  come back with one of the following indications:

    CDMA: SS_IND_CDMA_ACQ_FAILED_PILOT, SS_IND_CDMA_ACQ_FAILED_SYNC or
    SS_IND_CDMA_ACQ_SCHM.

    AMPS: SS_IND_AMPS_ACQ_FAILED_CCH or SS_IND_AMPS_ACQ_CCH_SID.

  If no system in LIST is successfully fully acquired, script execution
  continues with the next script construct after the acquisition of the last
  system in LIST fails.

  If a system in LIST is fully acquired, script execution never goes past
  this construct - when receiving the SS_IND_XXX_OPR_OH_SID SS-Indication a
  reselection script shall take over control.

APPLICABLE
  All record lists/items.

EXAMPLE
  ACQ( AVAIL_LST, ALLOWED ) - Step through the SDSR_AVAIL_LST (first to last)
  and try to acquire a PRL allowed system.

===========================================================================*/
#define ACQ( LIST, ACQ_TYPE )         SDSSSCR_CON_ACQ, \
                                        SDSR_##LIST, \
                                        SDSSSCR_ACQ_##ACQ_TYPE,

/*===========================================================================

ACQ_TRUE_MODE( LIST, ACQ_TYPE )

DEFINITION
 This will be exactly same as  ACQ( LIST, ACQ_TYPE ) except for:
 In this construct, ss_ptr's true_mode_prefrence will be compared and not 
 ss_ptr's mode_pref.

===========================================================================*/
 
#define ACQ_TRUE_MODE( LIST, ACQ_TYPE )    SDSSSCR_CON_ACQ_TRUE_MODE, \
                                             SDSR_##LIST, \
                                             SDSSSCR_ACQ_##ACQ_TYPE,
/* <EJECT> */
/*===========================================================================

NEW_ACQ

DEFINITION
  Start a new acquisition by incrementing the new-acquisition counter.

  Note that the ACQ() acquisition constructs only attempts to acquire system
  that were not already attempted acquisition since the last time the NEW_ACQ
  construct was encountered.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  1.
  NEW_ACQ
  ACQ( MRU_LST, ALLOWED )
  ACQ( PRL_LST, ALLOWED )  <-- Systems that are referenced by the MRU_LST are
                               not attempted acquisition here.
  2.
  NEW_ACQ
  ACQ( MRU_LST, ALLOWED )
  NEW_ACQ
  ACQ( PRL_LST, ALLOWED )  <-- Systems that are referenced by the MRU_LST are
                               attempted acquisition here as well.

===========================================================================*/
#define NEW_ACQ                       SDSSSCR_CON_NEW_ACQ,


/* <EJECT> */
/*===========================================================================

START_TIME

DEFINITION
  Start the acquisition timer.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  START_ACQ_TIME - Starts the acquisition timer.

===========================================================================*/
#define START_TIME                    SDSSSCR_CON_START_TIME,


/* <EJECT> */
/*===========================================================================

ACQ_ALL( LIST, ACQ_TYPE )

DEFINITION
  Equivalent to the ACQ construct except that systems are not excluded from
  acquisition attempt based on whether they were attempted acquisition
  since the last time the NEW_ACQ construct was encountered.

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_ALL( AVAIL_LST, ALLOWED ) - Step through the SDSR_AVAIL_LST (first to
  last) and try to acquire all systems that complies with the current mode
  and band preference (regardless of whether the these systems were already
  attempted acquisition since the last time NEW_ACQ was called).

===========================================================================*/
#define ACQ_ALL( LIST, ACQ_TYPE )     SDSSSCR_CON_ACQ_ALL, \
                                        SDSR_##LIST, \
                                        SDSSSCR_ACQ_##ACQ_TYPE,


/* <EJECT> */
/*===========================================================================

ACQ_CNT( LIST, ACQ_TYPE, START_POS, CNT )

DEFINITION
  Equivalent to the ACQ construct except for the following:

    1. Instead of first to last, the list is traversed START_POS to
       START_POS-1 (traversal continue with the first element after the last
       list element is traversed).

       Note that the following magic values can be used in conjunction with
       the START_POS field:

         SDSSSCR_FULL_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_FULL_SYS.

         SDSSSCR_MINI_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MINI_SYS.

         SDSSSCR_MICRO_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MICRO_SYS.

         SDSSSCR_FULL2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_FULL2_SYS.

         SDSSSCR_MINI2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MINI2_SYS.

         SDSSSCR_DEEP_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_DEEP_SYS.

         SDSSSCR_DEEP2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_DEEP2_SYS.

         SDSSSCR_SHALLOW_SYS- Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_SHALLOW_SYS.

         SDSSSCR_SHALLOW2_SYS- Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_SHALLOW2_SYS.

         SDSSSCR_EMERG_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_EMERG_SYS.

    2. Acquisition is attempted over at most CNT elements. (i.e., if CNT
       elements are traversed before reaching back the START_POS, the
       acquisition attempt is cut short and the remaining list elements are
       skipped).

       Note that the following magic value can be used in conjunction with
       the CNT field:

         CNT_ALL    - Attempt acquisition over all systems.

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_CNT( IDL_PREF_LST, MORE_PREF, 2, 10 ) - Starting with the 3rd element,
  step through the SDSR_IDL_PREF_LST and try to acquire a system that is more
  preferred than the serving system (i.e., SDSR_SRV_SYS). If 10 elements are
  traversed before all the IDL_PREF_LST elements are traversed, the
  acquisition attempt is cut short and script execution continues with the
  next script construct.

===========================================================================*/
#define ACQ_CNT( LIST, ACQ_TYPE, START_POS, CNT ) \
                                        SDSSSCR_CON_ACQ_CNT, \
                                        SDSR_##LIST, \
                                        SDSSSCR_ACQ_##ACQ_TYPE, \
                                        START_POS, \
                                        CNT,

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Definition of a magic value to be used in conjunction with the CNT
** parameter of the ACQ_CNT() acquisition constructs.
*/
#define CNT_ALL  ((byte) (-1))           /* Attempt acquisition over all
                                         ** the systems that are on the
                                         ** list */


/* <EJECT> */
/*===========================================================================

ACQ_TIME( LIST, ACQ_TYPE, START_POS, TIME )

DEFINITION
  Equivalent to the ACQ construct except for the following:

    1. Instead of first to last, the list is traversed START_POS to
       START_POS-1 (traversal continue with the first element after the last
       list element is traversed).

       Note that the following magic values can be used in conjunction with
       the START_POS field:

         SDSSSCR_FULL_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_FULL_SYS.

         SDSSSCR_MINI_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MINI_SYS.

         SDSSSCR_MICRO_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MICRO_SYS.

         SDSSSCR_FULL2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_FULL2_SYS.

         SDSSSCR_MINI2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MINI2_SYS.

         SDSSSCR_DEEP_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_DEEP_SYS.

         SDSSSCR_DEEP2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_DEEP2_SYS.

         SDSSSCR_SHALLOW_SYS- Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_SHALLOW_SYS.

         SDSSSCR_SHALLOW2_SYS- Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_SHALLOW2_SYS.

         SDSSSCR_EMERG_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_EMERG_SYS.

    2. Acquisition is attempted for at most TIME seconds since the START_TIME
       script construct was last encountered (i.e., if TIME seconds expires
       before reaching back the START_POS, the acquisition attempt
       is cut short and the remaining list elements are skipped).

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_TIME( IDL_PREF_LST, MORE_PREF, 2, 10 ) - Starting with the 3rd element,
  step through the SDSR_IDL_PREF_LST and try to acquire a system that is more
  preferred than the serving system (i.e., SDSR_SRV_SYS). If 10s (since that
  last time START_TIME was encountered) expires before all the IDL_PREF_LST
  elements are traversed, the acquisition attempt is cut short and script
  execution continues with the next script construct.

===========================================================================*/
#define ACQ_TIME( LIST, ACQ_TYPE, START_POS, TIME ) \
                                        SDSSSCR_CON_ACQ_TIME, \
                                        SDSR_##LIST, \
                                        SDSSSCR_ACQ_##ACQ_TYPE, \
                                        START_POS, \
                                        TIME,


/*===========================================================================

ACQ_TIME2( LIST, ACQ_TYPE, START_POS, NV_ITEM )

DEFINITION
  Equivalent to the ACQ_TIME construct 
SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_TIME2( IDL_PREF_LST, MORE_PREF, 2, TMR_RESERVED_1) - Starting with the 3rd element,
  step through the SDSR_IDL_PREF_LST and try to acquire a system that is more
  preferred than the serving system (i.e., SDSR_SRV_SYS). If 
  SDSSSCR_NV_ITEM_TMR_RESERVED_1 seconds(since that
  last time START_TIME was encountered) expires before all the IDL_PREF_LST
  elements are traversed, the acquisition attempt is cut short and script
  execution continues with the next script construct.

===========================================================================*/
#define ACQ_TIME2( LIST, ACQ_TYPE, START_POS, NV_ITEM ) \
                                        SDSSSCR_CON_ACQ_TIME2, \
                                        SDSR_##LIST, \
                                        SDSSSCR_ACQ_##ACQ_TYPE, \
                                        START_POS, \
                                        SDSSSCR_NV_ITEM_##NV_ITEM,

/*===========================================================================

ACQ_TIME3( LIST, ACQ_TYPE, START_POS, NV_ITEM )

DEFINITION
  Equivalent to the ACQ_TIME construct 
SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_TIME3( IDL_PREF_LST, MORE_PREF, 2, TMR_RESERVED_1) - Starting with the 3rd element,
  step through the SDSR_IDL_PREF_LST and try to acquire a system that is more
  preferred than the serving system (i.e., SDSR_SRV_SYS). If 
  SDSSSCR_NV_ITEM_TMR_RESERVED_1 seconds(since that
  last time START_TIME was encountered) expires before all the IDL_PREF_LST
  elements are traversed, the acquisition attempt is cut short and script
  execution continues with the next script construct.

  Same as ACQ_TIME2, the NV item is in milliseconds, instead of seconds

===========================================================================*/
#define ACQ_TIME3( LIST, ACQ_TYPE, START_POS, NV_ITEM ) \
                                        SDSSSCR_CON_ACQ_TIME3, \
                                        SDSR_##LIST, \
                                        SDSSSCR_ACQ_##ACQ_TYPE, \
                                        START_POS, \
                                        SDSSSCR_NV_ITEM_##NV_ITEM,


/*===========================================================================

ACQ_CNT_OR_TIME( LIST, ACQ_TYPE, START_POS, CNT, TIME )

DEFINITION
  Equivalent to the ACQ construct except for the following:

    1. Instead of first to last, the list is traversed START_POS to
       START_POS-1 (traversal continue with the first element after the last
       list element is traversed).

       Note that the following magic values can be used in conjunction with
       the START_POS field:

         SDSSSCR_FULL_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_FULL_SYS.

         SDSSSCR_MINI_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MINI_SYS.

         SDSSSCR_MICRO_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MICRO_SYS.

         SDSSSCR_FULL2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_FULL2_SYS.

         SDSSSCR_MINI2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MINI2_SYS.

         SDSSSCR_DEEP_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_DEEP_SYS.

         SDSSSCR_DEEP2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_DEEP2_SYS.

         SDSSSCR_SHALLOW_SYS- Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_SHALLOW_SYS.

         SDSSSCR_SHALLOW2_SYS- Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_SHALLOW2_SYS.

         SDSSSCR_EMERG_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_EMERG_SYS.

    2. Acquisition is attempted over at most CNT or TIME seconds elements. 
      (i.e., if CNT elements are traversed or time expires before reaching 
       back the START_POS, the acquisition attempt is cut short and the 
       remaining list elements are skipped).

       Note that the following magic value can be used in conjunction with
       the CNT field:

         CNT_ALL    - Attempt acquisition over all systems.

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_CNT_OR_TIME( IDL_PREF_LST, MORE_PREF, 2, 6, TMR_HALF_UOOS_SCAN ) - 
  Starting with the 3rd element,
  step through the SDSR_IDL_PREF_LST and try to acquire a system that is more
  preferred than the serving system (i.e., SDSR_SRV_SYS). If 6 elements are
  traversed before all the IDL_PREF_LST elements are traversed or TMR_UOOS 
  seconds have elapsed, the acquisition attempt is cut short and script 
  execution continues with the next script construct.

===========================================================================*/
#define ACQ_CNT_OR_TIME( LIST, ACQ_TYPE, START_POS, CNT, TIME ) \
                                        SDSSSCR_CON_ACQ_CNT_OR_TIME, \
                                        SDSR_##LIST, \
                                        SDSSSCR_ACQ_##ACQ_TYPE, \
                                        START_POS, \
                                        CNT, \
                                        SDSSSCR_NV_ITEM_##TIME,
                                                                
 /* <EJECT> */
/*===========================================================================

ACQ_RX( LIST, ACQ_TYPE, RX )

DEFINITION
  Equivalent to the ACQ construct except that only systems with Rx-power
  measurement value >= -RX dB  are being attempted acquisition.

  Note that RX is specified in units of -1dB. For example RX of 100 implies
  -100dB

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_RX( IDL_PREF_LST, MORE_PREF, 99 ) - Step through the SDSR_IDL_PREF_LST
  (first to last) and try to only acquire systems for which the Rx-power
  measurement field has a value that is >= -99dB.  Only fully acquire a
  system that is more preferred than the current serving system.

===========================================================================*/
#define ACQ_RX( LIST, ACQ_TYPE, RX )  SDSSSCR_CON_ACQ_RX, \
                                        SDSR_##LIST, \
                                        SDSSSCR_ACQ_##ACQ_TYPE, \
                                        RX,



/* <EJECT> */
/*===========================================================================

ACQ_NETLIST( LIST, ACQ_TYPE, NETLIST )

DEFINITION
  Equivalent to the ACQ  construct except that acquistion is 
  issued only when background service search yields >=1 PLMN.

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_NETLIST ( IDL_PREF_LST, MORE_PREF, 2 ) - Step through the 
  SDSR_IDL_PREF_LST (first to last) and try to acquire only when 
  number of PLMNS found is >= 2.  

===========================================================================*/
#define ACQ_NETLIST( LIST, ACQ_TYPE, NETLIST )  SDSSSCR_CON_ACQ_NETLIST, \
                                        SDSR_##LIST, \
                                        SDSSSCR_ACQ_##ACQ_TYPE, \
                                        NETLIST,

/* <EJECT> */
/*===========================================================================

ACQ_PI( LIST, ACQ_TYPE, NV_ITEM )

DEFINITION
  Equivalent to the ACQ construct except that only systems with Pilot-
  strength measurement value >= -PI*0.5dB are being attempted acquisition.

  Note that PI is specified in units of -0.5dB. For example, PI of 100
  implies -50dB.

  Pilotstrength measurement value is the value stored in the configurable 
  item table at index SDSSSCR_NV_ITEM_"NV_ITEM".


SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_PI( IDL_PREF_LST, ALLOWED, CDMA_PI_THR )-Step through the SDSR_IDL_PREF_LST
  (first to last) and try to only acquire systems for which the Pilot-
  strength measurement field has a value that is >= CDMA_PI_THR. Only fully 
  acquire system that is allowed by the PRL.

===========================================================================*/
#define ACQ_PI( LIST, ACQ_TYPE, NV_ITEM )  SDSSSCR_CON_ACQ_PI, \
                                        SDSR_##LIST, \
                                        SDSSSCR_ACQ_##ACQ_TYPE, \
                                        SDSSSCR_NV_ITEM_##NV_ITEM,

/* <EJECT> */
/*===========================================================================

ACQ_PI_X( LIST, ACQ_TYPE, NV_ITEM, X )

DEFINITION
  Equivalent to the ACQ construct except that only systems of access tech X with 
  Pilot-strength measurement value >= -PI*0.5dB are being attempted acquisition.

  Note that PI is specified in units of -0.5dB. For example, PI of 100
  implies -50dB.

  Pilotstrength measurement value is the value stored in the configurable 
  item table at index SDSSSCR_NV_ITEM_"NV_ITEM".


SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_PI_X( IDL_PREF_LST, ALLOWED, CDMA_PI_THR, CDMA)-Step through the 
  SDSR_IDL_PREF_LST (first to last) and try to only acquire CDMA systems for which 
  the Pilot-strength measurement field has a value that is >= CDMA_PI_THR and try acquire 
  all other access technology systems (eg. GW) in the list. Only fully acquire system that is 
  allowed by the PRL.
  NOTE:Category NON_CELL and NON_COMPLY are not available for this construct

===========================================================================*/
#define ACQ_PI_X( LIST, ACQ_TYPE, NV_ITEM, X)  SDSSSCR_CON_ACQ_PI_X, \
                                        SDSR_##LIST, \
                                        SDSSSCR_ACQ_##ACQ_TYPE, \
                                        SDSSSCR_NV_ITEM_##NV_ITEM, \
                                        SDSR_CAT_##X,
/* <EJECT> */
/*===========================================================================

SCHED( LIST, SCHED, ACQ_TYPE )

DEFINITION
  Schedule the system records that are referenced by the system record
  list/item LIST to be attempted acquisition of type ACQ_TYPE at time points
  that are specified by SCHED. The acquisition mode is FULL.

SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST.  Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  SCHED( SRV_SYS, REACQ_0_1_2_6_14_22_34, ACQ_TYPE ) - Schedule SDSR_SRV_SYS
  to be attempted re-acquisition of type SDSSSCR_ACQ_ALLOWED according to the
  SDSSSCR_SCHED_REACQ_0_1_2_6_14_22 schedule (i.e., at 0, 1, 2, 6, 14, 22 and
  34 seconds from the current time).

===========================================================================*/
#define SCHED( LIST, SCHED, ACQ_TYPE )  SDSSSCR_CON_SCHED_COND2, \
                                          SDSR_##LIST, \
                                          SDSSSCR_SCHED_##SCHED, \
                                          SDSSSCR_ACQ_##ACQ_TYPE, \
                                          SDSSSR_SCHED_COND_UNCONDITIONAL,


/* <EJECT> */
/*===========================================================================

SCHED_COND( LIST, SCHED, ACQ_TYPE )

DEFINITION
  Identical to SCHED(), except that only schedule systems that are not
  already on equal or higher priority reacquisition schedule.

SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST.  Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  SCHED_COND( SRV_SYS, REACQ_0_1_2_6_14_22_34, ACQ_TYPE ) - Schedule
  SDSR_SRV_SYS to be attempted re-acquisition of type SDSSSCR_ACQ_ALLOWED
  according to the SDSSSCR_SCHED_REACQ_0_1_2_6_14_22 schedule if SRV_SYS
  is not already on equal or higher priority reacquisition schedule.

===========================================================================*/
#define SCHED_COND( LIST, SCHED, ACQ_TYPE ) \
                                        SDSSSCR_CON_SCHED_COND2, \
                                          SDSR_##LIST, \
                                          SDSSSCR_SCHED_##SCHED, \
                                          SDSSSCR_ACQ_##ACQ_TYPE, \
                                          SDSSSR_SCHED_COND_NOT_ON_HIGH_SCHED,

/* <EJECT> */
/*===========================================================================

SCHED_COND2( LIST, SCHED, ACQ_TYPE, SCHED_COND )

DEFINITION
  Identical to SCHED(), except that only schedule systems that are not
  already on equal or higher priority reacquisition schedule than CMP_SCHED.

SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST.  Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  SCHED_COND2( SRV_SYS, REACQ_0_1_2_6_14_22_34, ACQ_TYPE, NOT_ON_SAME_SCHED )
  - Schedule SDSR_SRV_SYS to be attempted re-acquisition of type 
    SDSSSCR_ACQ_ALLOWED according to the SDSSSCR_SCHED_REACQ_0_1_2_6_14_22 
    schedule if SRV_SYS is not already on SDSSSCR_SCHED_REACQ_0_1_2_6_14_22
    schedule 

===========================================================================*/
#define SCHED_COND2( LIST, SCHED, ACQ_TYPE, SCHED_COND ) \
                                        SDSSSCR_CON_SCHED_COND2, \
                                          SDSR_##LIST, \
                                          SDSSSCR_SCHED_##SCHED, \
                                          SDSSSCR_ACQ_##ACQ_TYPE, \
                                          SDSSSR_SCHED_COND_##SCHED_COND,


/* <EJECT> */
/*===========================================================================

SCHED_2( LIST, SCHED, ACQ_TYPE, ACQ_MODE )

DEFINITION
  Same as SCHED(), but also specifies the acquisition mode (FULL, MINI, etc.)
  to be for the reacquisition attempts.

SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST.  Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  SCHED_2( SRV_SYS, REACQ_0_1_2_6_14_22_34, ACQ_TYPE, ACQ_MODE ) - Schedule
  SDSR_SRV_SYS to be attempted re-acquisition of type SDSSSCR_ACQ_ALLOWED and
  mode ACQ_MODE according to the SDSSSCR_SCHED_REACQ_0_1_2_6_14_22 schedule
  (i.e., at 0, 1, 2, 6, 14, 22 and 34 seconds from the current time).

===========================================================================*/
#define SCHED_2( LIST, SCHED, ACQ_TYPE, ACQ_MODE ) \
                                          SDSSSCR_CON_SCHED_2, \
                                          SDSR_##LIST, \
                                          SDSSSCR_SCHED_##SCHED, \
                                          SDSSSCR_ACQ_##ACQ_TYPE, \
                                          SDSS_ACQ_MODE_##ACQ_MODE,


/* <EJECT> */
/*===========================================================================

AVOID( LIST, TIME )

DEFINITION:
  Mark the system records that are referenced by the system record list/item
  LIST to be avoided from any acquisition attempts for TIME seconds.

  The magic value UNTIL_NEW_ACQ indicates that the system is to be avoided
  from acquisition attempts until the NEW_ACQ construct is encountered.

SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST. Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  AVOID( ACQ_SYS, 20 ) - Mark SDSR_ACQ_SYS to be avoided from any acquisition
  attempts for the next 20 seconds.

===========================================================================*/
#define AVOID( LIST, TIME )           SDSSSCR_CON_AVOID, \
                                        SDSR_##LIST, \
                                        WORD_HI( TIME ), \
                                        WORD_LO( TIME ),

#define UNTIL_NEW_ACQ                 SDSSSCR_AVOID_UNTIL_NEW_ACQ


/* <EJECT> */
/*===========================================================================

AVOID_COND( LIST, TIME, COND )

DEFINITION:
  Mark the system records that are referenced by the system record list/item
  LIST to be avoided from any acquisition attempts for TIME seconds or until
  the condition is met whichever comes first.

  COND can take one of the following values;

    UNTIL_ACQ_FAIL The avoidance will be cancelled if ACQ_FAIL is 
                   encountered.

    UNTIL_ACQ_FAIL_DUR_CALL_ORIG
                   The avoidance will be cancelled if ACQ_FAIL is 
                   encountered during call origination.


SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST. Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  AVOID_COND( ACQ_SYS, 60, UNTIL_ACQ_FAIL ) - Mark SDSR_ACQ_SYS to be avoided from 
  any acquisition attempts for the next 60 seconds or ACQ_FAIL is 
  encountered.

===========================================================================*/
#define AVOID_COND( LIST, TIME, COND )  SDSSSCR_CON_AVOID_COND, \
                                        SDSR_##LIST, \
                                        WORD_HI( TIME ), \
                                        WORD_LO( TIME ), \
                                        SDSSSCR_SCHED_AVOID_##COND,


/* <EJECT> */
/*===========================================================================

AVOID_CANCEL_COND( LIST, COND )

DEFINITION:
  Remove the avoidance placed on systems that are referenced by LIST and have
  been avoided till condition COND is met.

  COND can take one of the following values;

    UNTIL_ACQ_FAIL The avoidance will be cancelled if ACQ_FAIL is 
                   encountered.

    UNTIL_ACQ_FAIL_DUR_CALL_ORIG
                   The avoidance will be cancelled if ACQ_FAIL is 
                   encountered during call origination.

    CANCEL_ALL     The avoidance will be cancelled for all the systems.

SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST. Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  AVOID_CANCEL_COND( ACQ_SYS, UNTIL_ACQ_FAIL ) - Remove avoidance of ACQ_SYS
  if it had been avoided until acq fail.

===========================================================================*/
#define AVOID_CANCEL_COND( LIST, COND ) SDSSSCR_CON_AVOID_CANCEL_COND, \
                                        SDSR_##LIST, \
                                        SDSSSCR_SCHED_AVOID_##COND,

/* <EJECT> */
/*===========================================================================

AVOID_CANCEL( LIST )

DEFINITION:
  Un-mark the system records that are referenced by the system record
  list/item LIST to be avoided from acquisition attempts.

SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST. Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  AVOID_CANCEL( ACQ_SYS ) - Un-mark SDSR_ACQ_SYS to be avoided from
  acquisition attempts.

===========================================================================*/
#define AVOID_CANCEL( LIST )          AVOID_CANCEL_COND( LIST, CANCEL_ALL )



/* <EJECT> */
/*===========================================================================
=============================================================================
======================= SS-SCRIPT CONTROL CONSTRUCTS ========================
=============================================================================
===========================================================================*/


/*===========================================================================

LOOP_TIME( TIME )

DEFINITION
  Repeat executing the script between this construct and the matching LOOP
  construct for TIME seconds since this construct was first encountered.
  When falling out of the loop, continue with the script construct following
  the matching LOOP construct.

CONTROL
  If < TIME seconds has passed since this construct was first encountered
  script execution continues with the script construct that follows this
  construct. Else, script execution continues with the script construct that
  follows the matching LOOP construct.

EXAMPLE
  LOOP_TIME( 60 )
  ACQ( SRCH_LST, ALLOWED )
  LOOP

  Try to acquire allowed systems that are referenced by the SRCH_LIST for a
  minimum of 60 seconds (note that if and when acquisition is successful the
  reselection script will take over control).

===========================================================================*/
#define LOOP_TIME( TIME )             SDSSSCR_CON_LOOP_TIME, \
                                        WORD_HI( TIME ), \
                                        WORD_LO( TIME ),


/* <EJECT> */
/*===========================================================================

LOOP_NUM( NUM )

DEFINITION
  Repeat executing the script between this construct and the matching LOOP
  construct for NUM times. When falling out of the loop, continue with the
  script construct following the matching LOOP construct.

CONTROL
  If repeated the loop between this construct and the matching LOOP construct
  < NUM times, script execution continues with the script construct that
  follows this construct.  Else, script execution continues with the script
  construct that follows the matching LOOP construct.

EXAMPLE
  LOOP_NUM( 4 )
  ACQ( SRCH_LST, ALLOWED )
  LOOP

  Try to acquire allowed systems from the SRCH_LST for 4 times (note that if
  and when acquisition is successful the reselection script will take over
  control).

===========================================================================*/
#define LOOP_NUM( NUM )               SDSSSCR_CON_LOOP_NUM, \
                                        NUM,


/* <EJECT> */
/*===========================================================================

LOOP_LIST( LIST )

DEFINITION
  Repeat executing the script between this construct and the matching LOOP
  construct for cont(LIST) times, where count(LIST) is the number of elements
  that are currently on LIST. When falling out of the loop, continue with
  the script construct following the matching LOOP construct.

CONTROL
  If repeated the loop between this construct and the matching LOOP construct
  < count(LIST) times, script execution continues with the script construct
  that follows this construct.  Else, script execution continues with the
  script construct that follows the matching LOOP construct.

EXAMPLE
  LOOP_LIST( SRCH_LST )
  ACQ( SRCH_LST, ALLOWED )
  LOOP

  Try to acquire allowed systems from the SRCH_LST for count(SRCH_LIST) times
  (note that if and when acquisition is successful the reselection script
  will take over control).

===========================================================================*/
#define LOOP_LIST( LIST )             SDSSSCR_CON_LOOP_LIST, \
                                        SDSR_##LIST,


/* <EJECT> */
/*===========================================================================

LOOP_FOREVER

DEFINITION
  Repeat executing the script between this construct and the matching LOOP
  construct forever.

CONTROL
  Script execution continues immediately with the script construct that
  follows this construct.

EXAMPLE
  LOOP_FOREVER
  ACQ( SRCH_LST, ALLOWED )
  LOOP

  Try to acquire allowed systems that are referenced by SRCH_LST over and
  over (note that if and when acquisition is successful the reselection
  script will take over control).

===========================================================================*/
#define LOOP_FOREVER                  SDSSSCR_CON_LOOP_FOREVER,


/*===========================================================================

LOOP_COND( COND )

DEFINITION
  Repeat executing the script between this construct and the matching LOOP
  construct until COND is true.

CONTROL
  Script execution continues immediately with the script construct that
  follows this construct.

EXAMPLE
  LOOP_COND( GW_SCAN_COMPLETE )
  ACQ( SRCH_LST, ALLOWED )
  LOOP

  Try to acquire allowed systems that are referenced by SRCH_LST until the 
  GW SCAN is complete
===========================================================================*/
#define LOOP_COND( COND )             SDSSSCR_CON_LOOP_COND, \
                                          SDSS_LOOP_COND_##COND,

/* <EJECT> */
/*===========================================================================

LOOP

DEFINITION
  Loop-back to the matching LOOP_XXX construct.

CONTROL
  Script execution continues immediately with the matching LOOP_XXX
  construct.

===========================================================================*/
#define LOOP                          SDSSSCR_CON_LOOP,



/* <EJECT> */
/*===========================================================================

PREV_SCR

DEFINITION
  Return to perviously active SS-Script and continue with script execution.

CONTROL
  Script execution continues immediately with the previous script and
  construct.

===========================================================================*/
#define PREV_SCR                      SDSSSCR_CON_PREV_SCR,



/*===========================================================================

IF( COND, PARA1, PARA2, PARA3 )

DEFINITION
  Conditional execution. If the COND is true, script execution continues
  with the IF section. 

  Otherwise the script execution continues with the ELSE section, if present
  If else section is absent, the script execution continues after the ENDIF
  construct

CONTROL
  Script execution continues immediately with the script construct that
  follows this construct.

EXAMPLE
  IF( IS_GCF, EQUAL, TRUE )

  If GCF flag is set execute the IF section
  
===========================================================================*/
#define IF( COND, CRITERIA, PARA1 ) SDSSSCR_CON_IF, \
                                          SDSS_IF_COND_##COND, \
                                          SDSS_CMP_CRITERIA_##CRITERIA, \
                                          WORD_HI(PARA1), \
                                          WORD_LO(PARA1),

/*===========================================================================

ELSE

DEFINITION
  Start of the ELSE block

CONTROL
  Script execution continues with the ELSE section if the matching IF 
  condition is FALSE

===========================================================================*/
#define ELSE                          SDSSSCR_CON_ELSE,


/*===========================================================================

ENDIF

DEFINITION
  End of the IF block

CONTROL
  Script execution continues immediately with the next script construct.

===========================================================================*/
#define ENDIF                         SDSSSCR_CON_ENDIF,


/* <EJECT> */
/*===========================================================================

PROC_EVT( SS_EVENT )

DEFINITION
  Process the SS_EVENT SS-Event.

CONTROL
  Script execution continues immediately with the first construct of the
  script that becomes active as a result of calling on the SS-Core to process
  the SS_EVENT.

EXAMPLE
  PROC_EVT( INT_SRV_LOST ) - The SS-Core breaks the current script
  execution and start processing the SDSS_EVT_INT_SRV_LOST SS-Event (as a
  result a service lost script becomes the active SS-Script).

===========================================================================*/
#define PROC_EVT( SS_EVENT )          SDSSSCR_CON_PROC_EVT, \
                                        WORD_HI( SDSS_EVT_##SS_EVENT ), \
                                        WORD_LO( SDSS_EVT_##SS_EVENT ),


/* <EJECT> */
/*===========================================================================

ACT_SCR( SCRIPT )

DEFINITION
  Make SCRIPT the active SS-Script.

  SCRIPT can take on one of the following values:

  SRV_LOST_NORM     - Normal service lost script.
  SRV_LOST_SKIP     - Skip-over service lost script.
  RESEL_DM          - Dual-mode reselection script.
  RESEL_INIT        - Initial reselection script.
  RESEL_NORM        - Normal reselection script.
  RESEL_RX_GT_99,   - Rx-power > -99dB during reselection script.
  PWR_SAVE          - Power-save script.
  PWR_SAVE_RX_GT_99 - PRL_LST Rx-power > -99dB during power-save mode.
  HDR_ACQ_ROTATE    - HDR rotating acquisitons

CONTROL
  Script execution continues immediately with the first construct of SCRIPT.

EXAMPLE
 ACT_SCR( SRV_LOST_NORM ) - Activate the service lost script that is
 referenced by SDSSSCR_SRV_LOST_NORM.

===========================================================================*/
#define ACT_SCR( SCRIPT )             SDSSSCR_CON_ACT_SCR, \
                                        SDSSSCR_##SCRIPT,


/* <EJECT> */
/*===========================================================================

ACT_SCR_RX( SCRIPT, LIST, RX )

DEFINITION
  Make SCRIPT the active System Selection Script only if at least one of the
  systems that are referenced by system record list LIST has Rx-power
  measurement value >= -RX dB.

  Note that RX is specified in units of -1dB. For example RX of 100 implies
  -100dB

CONTROL
  If at least one of the systems that are referenced by system record list
  LIST has Rx-power measurement value >= -RX dB, script execution continues
  with the first construct of SCRIPT. Else, script execution continues with
  the construct that follows this construct.

EXAMPLE
  ACT_SCR_RX( PWR_SAVE_RX_GT_99, PRL_LST, 99 ) - Activate the power-save
  acquisition script that is referenced by SDSSSCR_PWR_SAVE_RX_GT_99 iff at
  least one of the systems that are referenced by SDSR_PRL_LST has Rx-power
  measurement value >= -99dB.

===========================================================================*/
#define ACT_SCR_RX( SCRIPT, LIST, RX )  SDSSSCR_CON_ACT_SCR_RX, \
                                          SDSSSCR_##SCRIPT, \
                                          SDSR_##LIST, \
                                          RX,


/* <EJECT> */
/*===========================================================================

ACT_SCR_PI( SCRIPT, LIST, PI )

DEFINITION
  Make SCRIPT the active System Selection Script only if at least one of the
  systems that are referenced by system record list LIST has a pilot strength
  measurement value >= -PI*0.5dB.

  Note that PI is specified in units of -0.5dB. For example, PI of 100
  implies -50dB.

CONTROL
  If at least one of the systems that are referenced by system record list
  LIST has a pilot strength measurement value >= -PI*0.5dB, script execution
  continues with the first construct of SCRIPT. Else, script execution
  continues with the construct that follows this construct.

EXAMPLE
  ACT_SCR_PI( PWR_SAVE_RX_GT_99, PRL_LST, 20 ) - Activate the power-save
  acquisition script that is referenced by SDSSSCR_PWR_SAVE_RX_GT_99 iff at
  least one of the systems that are referenced by SDSR_PRL_LST has Pilot
  strength measurement value >= -10dB.

===========================================================================*/
#define ACT_SCR_PI( SCRIPT, LIST, PI )  SDSSSCR_CON_ACT_PI, \
                                          SDSSSCR_##SCRIPT, \
                                          SDSR_##LIST, \
                                          PI,


/* <EJECT> */
/*===========================================================================

BEGIN

DEFINITION
  Mark the beginning of an SS-Script.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  BEGIN - Mark the beginning of a script.

===========================================================================*/
#define BEGIN


/* <EJECT> */
/*===========================================================================

END

DEFINITION
  Mark the end of an SS-Script. This script construct should never be
  reached.

CONTROL
  Abort normal operation and do error handling - probably error fatal.

EXAMPLE
  END - Mark the end of a script.

===========================================================================*/
#define END                           SDSSSCR_CON_END,




/* <EJECT> */
/*===========================================================================
=============================================================================
===================== MISCELLANEOUS SS-SCRIPT-CONSTRUCTS ====================
=============================================================================
===========================================================================*/


/*===========================================================================

CONT( TIME )

DEFINITION
  Instructs the SS-Client to continue with its current activity while
  setting-up a TIME timer. When this timer expires the SS-Client shall call
  on the SD_SS_IND_MISC_TIMER SS-Indication function.

  Note that the following magic values can be used in conjunction with
  the TIME field:

    0                         - Does not set a timer. Script execution never
                                continues passed the CONT() construct.

    SDSSSCR_UNTIL_NEXT_EVENT  - Does not set a timer. Script execution
                                continues passed the CONT() upon processing
                                the next SS-Event.

CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT( 60 ) - Break the script execution and return control to the SS-Client
  for 60 seconds.  When 60 seconds expire the SS-Client shall call on the
  SD_SS_IND_MISC_TIMER SS-Indication function.

===========================================================================*/
#define CONT( TIME )                  SDSSSCR_CON_CONT, \
                                        SDSSSCR_CONT_TYPE_NONE, \
                                        WORD_HI( TIME ), \
                                        WORD_LO( TIME ),

/*===========================================================================

CONT_GW_UOOS_REM_SLEEP(  )

DEFINITION
  Instructs the SS-Client to continue with its current activity while
  setting-up a TIME timer. When this timer expires the SS-Client shall call
  on the SD_SS_IND_MISC_TIMER SS-Indication function.

  Note that the following magic values can be used in conjunction with
  the TIME field:

    0                         - Does not set a timer. Script execution never
                                continues passed the CONT() construct.

    SDSSSCR_UNTIL_NEXT_EVENT  - Does not set a timer. Script execution
                                continues passed the CONT() upon processing
                                the next SS-Event.

CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT( 60 ) - Break the script execution and return control to the SS-Client
  for 60 seconds.  When 60 seconds expire the SS-Client shall call on the
  SD_SS_IND_MISC_TIMER SS-Indication function.

===========================================================================*/
#define CONT_GW_UOOS_REM_SLEEP    SDSSSCR_CON_CONT_GW_UOOS_REM_SLEEP,
                                   


/*===========================================================================

CONT_REDIR( TIME )

DEFINITION
  Conditionally stops script execution and instructs the SS-Client to
  continue until TIME seconds passed since the most recent redirection
  indication was received. If instructs SS-Client to continue, sets a timer
  for the appropriate duration.

CONTROL
  If the time since the most recent redirection indication was received >=
  TIME script execution continues with next script construct.

  Else, return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT_REDIR( 180 ) - Break the script execution and return control to
  the SS-Client until 180s passed since the most recent redirection
  indication was received, at which time the SS-Client calls on the
  SD_SS_IND_MISC_TIMER SS-Indication function.

===========================================================================*/
#define CONT_REDIR( TIME )            SDSSSCR_CON_CONT_REDIR, \
                                        SDSSSCR_CONT_TYPE_NONE, \
                                        WORD_HI( TIME ), \
                                        WORD_LO( TIME ),


/*===========================================================================

CONT_WAIT( TYPE, TIME )

DEFINITION
  Instructs the SS-Client to continue with its current activity if a TYPE
  specific timer does not expires. When this timer expires the SS-Client
  shall call on the SD_SS_IND_MISC_TIMER SS-Indication function.

  Note that the following magic values can be used in conjunction with
  the TIME field:

    0                         - Does not set a timer. Script execution never
                                continues passed the CONT() construct.

    SDSSSCR_UNTIL_NEXT_EVENT  - Does not set a timer. Script execution
                                continues passed the CONT() upon processing
                                the next SS-Event.

  TYPE can take on one of the following values:

    LESS_PREF                 - Acquired less preferred channel.

    RELEASE                   - Exit traffic channel

CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT_WAIT( LESS PREF, RESEL_TIMEOUT_HDR_LONG ) - Break the script execution
  and return control to the SS-Client until RESEL_TIMEOUT_HDR_LONG seconds
  passed since the start time is set, at which time the SS-Client shall call
  on the SD_SS_IND_MISC_TIMER SS-Indication function.

===========================================================================*/
#define CONT_WAIT( TYPE, TIME )       SDSSSCR_CON_CONT_WAIT, \
                                        SDSSSCR_CONT_TYPE_##TYPE, \
                                        WORD_HI( TIME ), \
                                        WORD_LO( TIME ),


/*===========================================================================

CONT2( NV_ITEM )

DEFINITION
  Instructs the SS-Client to continue with its current activity while
  setting-up a TIME second timer. When this timer expires the SS-Client shall
  call on the SD_SS_IND_MISC_TIMER SS-Indication function. Where TIME is the
  value stored in the configurable items table at index
  SDSSSCR_NV_ITEM_"NV_ITEM".

 The size of timer is 32-bit in length.
 
CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT2( TIMER_HDR_LONG ) - Break the script execution and return control to
  the SS-Client for TIME seconds.  When TIME seconds expire the SS-Client
  shall call on the SD_SS_IND_MISC_TIMER SS-Indication function.  Where TIME
  is the value stored in the configurable items table at index
  SDSS_NV_ITEM_TIMER_HDR_LONG.

===========================================================================*/
#define CONT2( NV_ITEM )              SDSSSCR_CON_CONT2, \
                                        SDSSSCR_CONT_TYPE_NONE, \
                                        SDSSSCR_NV_ITEM_##NV_ITEM, \
                                        SDSSSCR_RESERVED,


/*===========================================================================

CONT3( NV_ITEM1, NV_ITEM2, NV_ITEM3 )

DEFINITION
  Instructs the SS-Client to continue with its current activity while
  setting-up a TIME second timer. When this timer expires the SS-Client shall
  call on the SD_SS_IND_MISC_TIMER SS-Indication function. Where TIME is the
  value stored in the configurable items table at index
  SDSSSCR_NV_ITEM_"NV_ITEM".

CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT2( TIMER_HDR_LONG ) - Break the script execution and return control to
  the SS-Client for TIME seconds.  When TIME seconds expire the SS-Client
  shall call on the SD_SS_IND_MISC_TIMER SS-Indication function.  Where TIME
  is the value stored in the configurable items table at index
  SDSS_NV_ITEM_TIMER_HDR_LONG.

===========================================================================*/
#define CONT3( NV_ITEM1, NV_ITEM2, NV_ITEM3 )  SDSSSCR_CON_CONT3, \
                                        SDSSSCR_CONT_TYPE_NONE, \
                                        SDSSSCR_NV_ITEM_##NV_ITEM1, \
                                        SDSSSCR_NV_ITEM_##NV_ITEM2, \
                                        SDSSSCR_NV_ITEM_##NV_ITEM3,

/*===========================================================================

CONT2_REDIR( NV_ITEM )

DEFINITION
  Conditionally stops script execution and instructs the SS-Client to
  continue until TIME seconds passed since the most recent redirection
  indication was received. If instructs SS-Client to continue, sets a timer
  for the appropriate duration. Where TIME is the value stored in the
  configurable items table at index SDSSSCR_NV_ITEM_"NV_ITEM".

 The size of timer is 32-bit in length.
 
CONTROL
  If the time since the most recent redirection indication was received >=
  TIME script execution continues with next script construct.

  Else, return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT2_REDIR( TIMER_CDMA ) - Break the script execution and returns control
  to the SS-Client until TIME seconds passed since the most recent
  redirection indication was received, at which time the SS-Client calls on
  the SD_SS_IND_MISC_TIMER SS-Indication function. Where TIME is the value
  stored in the configurable items table at index SDSS_NV_ITEM_TIMER_CDMA.

===========================================================================*/
#define CONT2_REDIR( NV_ITEM )        SDSSSCR_CON_CONT2_REDIR, \
                                        SDSSSCR_CONT_TYPE_NONE, \
                                        SDSSSCR_NV_ITEM_##NV_ITEM, \
                                        SDSSSCR_RESERVED,


/*===========================================================================

CONT2_WAIT( TIMER, NV_ITEM)

DEFINITION
  Conditional stops the script execution and instructs the SS-Client continue
  with its current activity until TIME seconds passed since TIMER is started.
  If instructs SS-Client to continue, sets TIMER for the appropriate duration.
  Where TIME is the value stored in the configurable items table at index
  SDSSSCR_NV_ITEM_"NV_ITEM".


  TIMER can take on one of the following values:

    LESS_PREF                 - Time is started when it acquires less
                                preferred channel.

    HOLD_RESEL                - Timer is started when HDR exits traffic
                                channel.

    CALL_END                  - Timer is started when MO call is ended.


 The size of timer is 32-bit in length.
 
CONTROL
  If the time since the TIMER was started >= TIME, script execution continues
  with next script construct.

  else return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT2_WAIT( HOLD_RESEL, TMR_HDR_RESEL_HOLD ) - Break the script
  execution and return control to the SS-Client until TMR_HDR_RESEL_HOLD
  seconds passed since LESS_PREF is set, at which time the SS-Client shall
  call on the SD_SS_IND_MISC_TIMER SS-Indication function.

===========================================================================*/
#define CONT2_WAIT( TIMER, NV_ITEM)    SDSSSCR_CON_CONT2_WAIT, \
                                        SDSSSCR_CONT_TYPE_##TIMER, \
                                        SDSSSCR_NV_ITEM_##NV_ITEM, \
                                        SDSSSCR_RESERVED,


/*===========================================================================

CONT3_WAIT( TIMER1, NV_ITEM1, TIMER2, NV_ITEM2)

DEFINITION
  Conditional stops the script execution and instructs the SS-Client to
  continue with its current activity until TIME1 seconds passed since TIMER1
  is started or TIME2 seconds passed since TIMER2 is started, whichever
  comess first. Where TIME1 and TIME2 are the value stored in the
  configurable items table at index SDSSSCR_NV_ITEM_"NV_ITEM".


  TIMER1 and TIMER2 can take on two of the following timer types:

  TIMER can take on one of the following values:

    LESS_PREF                 - Time is started when it acquires less
                                preferred channel.

    HOLD_RESEL                - Timer is started when HDR exits traffic
                                channel.

    CALL_END                  - Timer is started when MO call is ended.

 The size of timer is 16-bit in length.
 
CONTROL
  If either time1 or time2 since TIMER1 or TIMER2 was started >=
  TIME1 or TIME2, script execution continues with next script construct.

  Else return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when either TIMER1 or TIMER2
  whichever expires first, at which time script execution continues with next
  script construct.

EXAMPLE
  CONT_WAIT3( LESS PREF, TMR_HDR_RESEL, CALL_END, TMR_HDR_RESEL_HOLD_CALL_END)
  - Break the script execution and return control to the SS-Client until either
  TMR_HDR_RESEL seconds passed since LESS_PREF is started or
  TMR_HDR_RESEL_SS_PREF seconds passed since the SS_PREF is started, at which
  time the SS-Client shall call on the SD_SS_IND_MISC_TIMER SS-Indication
  function.

===========================================================================*/
#define CONT3_WAIT( TIMER1, NV_ITEM1, TIMER2, NV_ITEM2 )             \
                                        SDSSSCR_CON_CONT3_WAIT,    \
                                        SDSSSCR_CONT_TYPE_##TIMER1, \
                                        SDSSSCR_NV_ITEM_##NV_ITEM1,\
                                        SDSSSCR_CONT_TYPE_##TIMER2, \
                                        SDSSSCR_NV_ITEM_##NV_ITEM2,



/*===========================================================================

CONT_SET( TYPE, VALUE )

DEFINITION

  Set the start time of the pecified TYPE of timer with the VALUE.

  Note that the following values can be used in the VALUE field:

    ZERO                      - Set the value of the start time to 0.

    UPTIME_IF_ZERO            - Set the value of the start timer to uptime
                                only if it is zero.

    UPTIME                    - Set the value of the start timer to uptime.

    INC_BY_BCMCS_LEN          - Increment the timer by BCMCS flow length.

  TYPE can take on one of the following values:

    LESS_PREF                 - Acquired less preferred channel.

    HOLD_RESEL                - Exit traffic channel

    CALL_END             - After user ends the call.

CONTROL

EXAMPLE
  CONT_SET( LESS_PREF, UPTIME_IF_ZERO ) - Set the start time to UPTIME if it
  is zero.

===========================================================================*/
#define CONT_SET( TYPE, VALUE )       SDSSSCR_CON_CONT_SET, \
                                        SDSSSCR_CONT_TYPE_##TYPE, \
                                        SDSSSCR_CONT_VAL_##VALUE,

/*===========================================================================

SET_TIMER( NV_ITEM, TMR_ADJ )

DEFINITION
  Sets the timer with the timer maintainence task (MMoC)

  The size of timer is 32-bit in length.
 
CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE

  SET_TIMER( UOOS_TMR, CONN_UOOS_SCAN )

  Set a UOOS_TMR timer, adjusted by CONN_UOOS_SCAN, that is the actual timer 
  is UOOS_TMR - CONN_UOOS_SCAN seconds

===========================================================================*/
#define SET_TIMER( NV_ITEM, TMR_ADJ ) \
                                      SDSSSCR_CON_SET_TIMER, \
                                        SDSSSCR_NV_ITEM_##NV_ITEM, \
                                        TMR_ADJ,



/*===========================================================================

MAINTAIN_TIMER

DEFINITION
  Maintain the timer

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  MAINTAIN_TIMER - Maintain the timer

===========================================================================*/
#define MAINTAIN_TIMER                SDSSSCR_CON_MAINTAIN_TIMER,




/*===========================================================================

MAINTAIN_PREV_SCR_TIMER

DEFINITION
  Maintain the previous script timer

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  MAINTAIN_PREV_SCR_TIMER - Maintain the previous scripttimer

===========================================================================*/
#define MAINTAIN_PREV_SCR_TIMER       SDSSSCR_CON_MAINTAIN_PREV_SCR_TIMER,


/*===========================================================================

CONT_BSR( TIME )

DEFINITION
  Instructs the SS-Client to continue with its current activity while
  setting-up a BSR timer. If earlier BSR timer is yet to expire and UE is on the same
  system then earlier timer shall continue otherwise TIME timer will start. It will also
  update BSR system record for which this timer is started.
  
  When this timer expires the SS-Client shall call
  on the SD_SS_IND_MISC_TIMER SS-Indication function.

  Note that the following magic values can be used in conjunction with
  the TIME field:

    0                         - Does not set a timer. Script execution never
                                continues passed the CONT() construct.

    SDSSSCR_UNTIL_NEXT_EVENT  - Does not set a timer. Script execution
                                continues passed the CONT() upon processing
                                the next SS-Event.

CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT_BSR( 60 ) - Break the script execution and return control to the SS-Client
  for 60 seconds.  When 60 seconds expire the SS-Client shall call on the
  SD_SS_IND_MISC_TIMER SS-Indication function.

===========================================================================*/
#define CONT_BSR(TIME)                  SDSSSCR_CON_CONT_BSR, \
                                        SDSSSCR_CONT_TYPE_NONE, \
                                        WORD_HI( TIME ), \
                                        WORD_LO( TIME ),
                                        
/*===========================================================================

CONT2_BSR( NV_ITEM )

DEFINITION
  Instructs the SS-Client to continue with its current activity while
  setting-up a BSR timer. If earlier BSR timer is yet to expire and UE is on the same
  system then earlier timer shall continue otherwise TIME timer will start. It will also
  update BSR system record for which this timer is started. Where TIME is the
  value stored in the configurable items table at index
  SDSSSCR_NV_ITEM_"NV_ITEM".

 The size of timer is 32-bit in length.
 
CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT2_BSR( TIMER_HDR_LONG ) - Break the script execution and return control to
  the SS-Client for TIME seconds.  When TIME seconds expire the SS-Client
  shall call on the SD_SS_IND_MISC_TIMER SS-Indication function.  Where TIME
  is the value stored in the configurable items table at index
  SDSS_NV_ITEM_TIMER_HDR_LONG.

===========================================================================*/                                       
#define CONT2_BSR( NV_ITEM )              SDSSSCR_CON_CONT2_BSR, \
                                        SDSSSCR_CONT_TYPE_NONE, \
                                        SDSSSCR_NV_ITEM_##NV_ITEM, \
                                        SDSSSCR_RESERVED,
                                        
/*===========================================================================

STACK_SYNC_UP

DEFINITION
  Sync-up the UOoS scans across stacks

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  STACK_SYNC_UP - Sync-up the UOoS scans.

===========================================================================*/
#define STACK_SYNC_UP                 SDSSSCR_CON_STACK_SYNC_UP,



/*===========================================================================

ACCEPT

DEFINITION
  Accept BS proposed mode, band and/or CDMA channel/AMPS system.

  NOTE! This SS-Action is only valid for Redirection, Channel Assignment or
  Handoff Direction.

CONTROL
  Return control to the SS-Client, which then should call on the
  sd_ss_ind_cdma_opr_acpt_rej() SS-Indication function upon completing the
  processing that is associated with the accept, at which time script
  execution continues with next script construct.


EXAMPLE
  ACCEPT - SS-Client should send an acknowledgement for a directed
  redirection and than call on the sd_ss_ind_cdma_opr_acpt_rej()
  SS-Indication function.

===========================================================================*/
#define ACCEPT                        SDSSSCR_CON_ACCEPT,




/* <EJECT> */
/*===========================================================================

REJECT

DEFINITION
  Reject BS proposed mode, band and/or CDMA channel/AMPS system.

  NOTE! This SS-Action is only valid for Redirection, Channel Assignment or
  Handoff Direction.

CONTROL
  Return control to the SS-Client, which then should call on the
  sd_ss_ind_cdma_opr_acpt_rej() SS-Indication function upon completing the
  processing that is associated with the reject, at which time script
  execution continues with next script construct.

EXAMPLE
  REJECT - SS-Client should send an reject for a directed redirection
  and than call on the sd_ss_ind_cdma_opr_acpt_rej() SS-Indication function.

===========================================================================*/
#define REJECT                        SDSSSCR_CON_REJECT,




/* <EJECT> */
/*===========================================================================

PWR_SAVE

DEFINITION
  Instruct the SS-Client to enter Power Save mode of operation.  When the
  SS-Client enters the Power Save mode of operation the SS-Client should call
  on the SD_SS_IND_MISC_PWR_SAVE_ENTER SS-Indication function.

CONTROL
  Return control to the SS-Client, which then should enter power-save mode
  of operation. Upon entering power-save mode, the SS-Client should call on
  the SD_SS_IND_MISC_PWR_SAVE_ENTER SS-Indication function, at which time
  script execution continues with next script construct.

EXAMPLE
  PWR_SAVE - SS-Client should enter power save mode of operation and then
  call on the SD_SS_IND_MISC_PWR_SAVE_ENTER SS-Indication function.

===========================================================================*/
#define PWR_SAVE                      SDSSSCR_CON_PWR_SAVE,


/*===========================================================================

GET_NET

DEFINITION
  Instruct the SS-Client to scan for the available systems over GSM/WCDMA.

CONTROL
  Return control to the SS-Client, which then needs to submit the get networks
  request to the protocol.

EXAMPLE
  GET_NET
===========================================================================*/
#define GET_NET( LIST, REF_SYS )      SDSSSCR_CON_GET_NET,\
                                      SDSR_##LIST, \
                                      SDSR_##REF_SYS,



/*===========================================================================

SURVEY_WLAN

DEFINITION
  Instruct the SS-Client to scan for the available systems over WLAN.

CONTROL
  Return control to the SS-Client, which then needs to submit the get networks
  request to the protocol. When protocol comes back with the networks list,
  the SS-Client should call on the sd_ss_ind_wlan_misc_survey_rpt SS-Indication
  function, at which time script execution continues with next script
  construct.

EXAMPLE
  SURVEY_WLAN
===========================================================================*/
#define SURVEY_WLAN                SDSSSCR_CON_SURVEY_WLAN,

/*===========================================================================

PWR_SCAN_GW

DEFINITION
  Instruct the SS-Client to do early power scan over GSM/WCDMA. When the
  SS-Client enters the early power scan the SS-Client should call
  on the SD_SS_IND_MISC_CNF SS-Indication function.

CONTROL
  Return control to the SS-Client, which then needs to submit the power scan
  request to the protocol. The protocol need not come back with the results,
  which case the SD would be waiting for user SS-Indication to be invoked.

EXAMPLE
  PWR_SCAN_GW
===========================================================================*/
#define PWR_SCAN_GW( LIST )              SDSSSCR_CON_PWR_SCAN_GW, \
                                        SDSR_##LIST,


/* <EJECT> */
/*===========================================================================

REDIR_OFF

DEFINITION
  Set the redirection indication to disabled.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  REDIR_OFF - Set the redirection indication to off.

===========================================================================*/
#define REDIR_OFF                     SDSSSCR_CON_REDIR_OFF,


/* <EJECT> */
/*===========================================================================

HO_OFF

DEFINITION
  Set the handoff indication to disabled.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  OH_OFF - Set the handoff indication to off.

===========================================================================*/
#define HO_OFF                        SDSSSCR_CON_HO_OFF,

/*===========================================================================

RESET_GW_ACQ_SCAN_STATUS

DEFINITION
  Reset the gw acq scan status

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  RESET_GW_ACQ_SCAN_STATUS - Reset the gw acq scan status

===========================================================================*/
#define RESET_GW_ACQ_SCAN_STATUS      SDSSSCR_CON_RESET_GW_ACQ_SCAN_STATUS,


/*===========================================================================

RESET_GW_UOOS_ACQ_PARA

DEFINITION
  Resets the GW acquisition parameters.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  RESET_GW_UOOS_ACQ_PARA

===========================================================================*/
#define RESET_GW_UOOS_ACQ_PARA        SDSSSCR_CON_RESET_GW_UOOS_ACQ_PARA, 

/* <EJECT> */
/*===========================================================================

ACQ_FAIL

DEFINITION
  Indicate that acquisition over all systems has failed.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  ACQ_FAIL - Indicate that acquisition over all systems has failed.

===========================================================================*/
#define ACQ_FAIL                      SDSSSCR_CON_ACQ_FAIL,

/*===========================================================================

BSR_TIMER_RESET

DEFINITION
  Indicate that acquisition over all systems has failed.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  ACQ_FAIL - Indicate that acquisition over all systems has failed.

===========================================================================*/
#define BSR_TIMER_RESET                 SDSSSCR_CON_BSR_TIMER_RESET,

/* <EJECT> */
/*===========================================================================

ACC_RESET()

DEFINITION
  Reset the registration access failure count and start time.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  ACC_RESET() - Reset the registration access failure count and start time.

===========================================================================*/
#define ACC_RESET                     SDSSSCR_CON_ACC_RESET,


/* <EJECT> */
/*===========================================================================

SRV_STATUS( VAL )

DEFINITION
  Set the Service-Indication to VAL given.
  Possible values
  NO_SRV - Set service status to no service.
  PWR_SAVE - Set service status to PWR_SAVE.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  SRV_STATUS( NO_SRV ) - Set the service indicator to OFF.
  SRV_STATUS( PWR_SAVE ) - Set the service indicator to PWR_SAVE.

===========================================================================*/
#define SRV_STATUS( VAL )               SDSSSCR_CON_SRV_STATUS, \
                                            SYS_SRV_STATUS_##VAL,


/* <EJECT> */
/*===========================================================================

CHK_SRV_STATUS_AND_SET( CHK_VAL, SRV_VAL )

DEFINITION
  Check if the Service-Indication is CHK_VAL
  If the service indication is CHK_VAL, set the service indication to
  SRV_VAL.

 Possible values for SRV_VAL
  NO_SRV - Set service status to no service.
  PWR_SAVE - Set service status to PWR_SAVE.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  CHK_SRV_STATUS_AND_SET( NO_SRV_ANY, NO_SRV )
    - If the current service indicator is Power save or no service set the
      service indicator to OFF.

===========================================================================*/


#define CHK_SRV_STATUS_AND_SET( CHK_VAL, SRV_VAL ) SDSSSCR_CON_CHK_SET_SRV_STATUS, \
                                                   SDSSSCR_SRV_STATUS_VAL_##CHK_VAL,\
                                                   SYS_SRV_STATUS_##SRV_VAL,


/* <EJECT> */
/*===========================================================================

SRV_OFF_UNTIL( CRIT )

DEFINITION
  Set the Service-Indication to OFF. Do not update the service indicators
  before the specified criteria is met.

  CRIT can take on one of the following values:

    ACQ   - Acquiring a system.
    OH    - Receiving the overhead information.
    REG   - Registering with the system.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  SRV_OFF_UNTIL( REG ) - Set the service indicator to OFF. Do not update the
  service indicators until MS successfully registered on a system.


===========================================================================*/
#define SRV_OFF_UNTIL( CRIT )         SDSSSCR_CON_SRV_OFF_UNTIL, \
                                        SD_SI_UPDATE_##CRIT,


/* <EJECT> */
/*===========================================================================

SRV_OFF_IF_0( LIST )

DEFINITION
  Set the Service-Indication to OFF iff LIST contains 0 elements.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  SRV_OFF_IF_0( AVAIL_LST ) - Set the service indicator to OFF iff
  SDSR_AVAIL_LST contains 0 elements.

===========================================================================*/
#define SRV_OFF_IF_0( LIST )          SDSSSCR_CON_SRV_OFF_IF_0, \
                                        SDSR_##LIST,


/* <EJECT> */
/*===========================================================================

RESEL_STT( STATE )

DEFINITION
  Set the Reselection State to STATE.

  STATE can take on one of the following values:

    NONE      - Not doing reselection.

    WAIT      - Waiting for next reselection period.
    SRCH      - Searching for more preferred system.
    REACQ     - Reacquiring the serving system.

    SS_PREF   - Reselection after SS-Pref change.
    RELEASE   - Reselection after call is released.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  RESEL_STT( SRCH ) - Set the reselection state to SDSS_RESEL_STT_SRCH.

===========================================================================*/
#define RESEL_STT( STATE )            SDSSSCR_CON_RESEL_STT, \
                                        SDSS_RESEL_STT_##STATE,


/* <EJECT> */
/*===========================================================================

RESEL_MODE( MODE )

DEFINITION
  Set the Reselection Mode to MODE.

  MODE can take on one of the following values:

    INIT      - Initial reselection.
    NORM      - Normal reselection.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  RESEL_MODE( INIT ) - Set the reselection mode to SDSS_RESEL_MODE_INIT.

===========================================================================*/
#define RESEL_MODE( MODE )            SDSSSCR_CON_RESEL_MODE, \
                                        SDSS_RESEL_MODE_##MODE,


/*===========================================================================

SET_FLAG( FLAG, SET_VAL )

DEFINITION
  Set the FLAG to SET_VAL.

  FLAG can take on one of the following values:

  TRUE_EVENT_PROCESSED

  SET_VAL can take 
    TRUE
    FALSE

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  SET_FLAG( TRUE_EVENT_PROCESSED, TRUE ) - Set the TRUE_EVENT_PROCESSED flag
  to TRUE

===========================================================================*/
#define SET_FLAG( FLAG, SET_VAL )     SDSSSCR_CON_SET_FLAG, \
                                        SDSS_FLAG_##FLAG, \
                                        SET_VAL,

/*===========================================================================

SRV_REQ_TYPE( VAL )

DEFINITION
  Set the service request to VAL

  VAL is of type sd_ss_srv_req_type_e_type

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  SRV_REQ_TYPE( SYNC_UP ) - Set the srv_req type 
                            to SD_SS_SRV_REQ_TYPE_SYNC_UP
  
===========================================================================*/

#define SRV_REQ_TYPE( VAL )           SDSSSCR_CON_SRV_REQ,\
                                        SD_SS_SRV_REQ_TYPE_##VAL,

/*===========================================================================
=============================================================================
======================== CONDITION SCRIPT CONSTRUCTS ========================
=============================================================================
===========================================================================*/

/* Each SS-Condition is formulated as a condition script that is built of
** condition script constructs. A condition script evaluates to TRUE if and
** only if all of its condition constructs evaluate to TRUE. When searching
** the exception scripts table a match is declared if a particular condition
** script evaluates to TRUE, in which case the associated script becomes the
** active SS-Script.
*/


/* <EJECT> */
/*===========================================================================

NOT

DEFINITION
  Negate the value of the following condition script construct.

EXAMPLE
  NOT IS( REDIR_RIF ) - Evaluates to TRUE iff the current/proposed
  redirection's RETURN_IF_FAILED != 1.

===========================================================================*/
#define NOT                           SDSSSCR_CON_NOT,


/* <EJECT> */
/*===========================================================================

IS( VAL )

DEFINITION
  Evaluates to TRUE iff VAL is equal to a TRUE boolean condition.

  VAL can take on one of the following values:

    REDIR_VALID           - Redirection is valid.
    REDIR_RIF             - Redirection RETURN_IF_FAILED = 1.
    REDIR_WILDCARD_SID    - Redirection SID = wildcard-SID.
    REDIR_WILDCARD_NID    - Redirection NID = wildcard-NID.
    REDIR_IGNORE_CDMA     - Redirection IGNORE_CDMA = 1.

    CAP_AMPS              - MS is capable of AMPS.
    CAP_GSM               - MS is capable of GSM.
    CAP_CDMA_CELL         - MS is capable of CDMA-Cellular.
    CAP_CDMA_PCS          - MS is capable of CDMA-PCS.
    CAP_CDMA_CELL_NO_PCS  - MS is capable of CDMA-Cellular but not CDMA PCS.
    CAP_CDMA_PCS_NO_CELL  - MS is capable of CDMA-PCS but not CDMA cellular.
    CAP_GW_PREF_GW_ONLY          - MS is capable of GW but not 1x.
    CAP_GW                - MS is capable of GW.

    CDMA_LOCKED           - MS is CDMA locked until power cycle.
    SRDA                  - Silent redial with AMPS is enabled.

    ORIG_MODE_NORM_CNG    - Normal origination mode change.
    ORIG_MODE_END         - Ending call origination mode.
    TRUE_MODE_PREF_GSM    - True mode_pref has GSM in it.
    SRV_DOMAIN_PREF_CS_PS - Service domain pref is CS_PS
    CAP_GW_PREF_GW_ONLY   - MS is GW capable and Effective mode preference
                            is GSM or WCDMA or GSM_WCDMA
    SIM_DOMAIN_VALID      - SIM is valid for some/all of selected domain (CS+/PS)

EXAMPLE
  IS( REDIR_RIF ) - Evaluates to TRUE iff the current/proposed redirection's
  RETURN_IF_FAILED = 1.

===========================================================================*/
#define IS( VAL )                     SDSSSCR_CON_IS, \
                                        SDSSSCR_COND_##VAL,

/* <EJECT> */
/*===========================================================================

IS_NOT( VAL )

DEFINITION
  Evaluates to TRUE iff VAL is equal to a FALSE boolean condition.

  VAL can take on one of the following values:

    REDIR_VALID           - Redirection is valid.
    REDIR_RIF             - Redirection RETURN_IF_FAILED = 1.
    REDIR_WILDCARD_SID    - Redirection SID = wildcard-SID.
    REDIR_WILDCARD_NID    - Redirection NID = wildcard-NID.
    REDIR_IGNORE_CDMA     - Redirection IGNORE_CDMA = 1.

    CAP_AMPS              - MS is capable of AMPS.
    CAP_CDMA_CELL         - MS is capable of CDMA-Cellular.
    CAP_CDMA_PCS          - MS is capable of CDMA-PCS.
    CAP_CDMA_CELL_NO_PCS  - MS is capable of CDMA-Cellular but not CDMA-PCS.
    CAP_CDMA_PCS_NO_CELL  - MS is capable of CDMA-PCS but not CDMA-Cellular.

    CDMA_LOCKED           - MS is CDMA locked until power cycle.
    SRDA,                 - Silent redial with AMPS is enabled.

    ORIG_MODE_NORM_CNG    - Normal origination mode change.

    FEATURE_T53,          - FEATURE_T53 is on.
    FEATURE_JCDMA,        - FEATURE_JCDMA is on.

EXAMPLE
  IS_NOT( REDIR_RIF ) - Evaluates to TRUE iff the current/proposed
  redirection's RETURN_IF_FAILED = 0.

===========================================================================*/
#define IS_NOT( VAL )                 SDSSSCR_CON_IS_NOT, \
                                        SDSSSCR_COND_##VAL,


/*===========================================================================

IS_EVENT( VAL )

DEFINITION
  Evaluates to TRUE iff VAL is equal the current SS-Event.

  VAL can take on any one of the SS_EVT_XXX SS-Events.

EXAMPLE
  IS_EVENT( CDMA_OPR_SYS_LOST ) - Evaluates to TRUE iff the current SS-Event
  is system lost (i.e., SDSS_EVT_CDMA_OPR_SYS_LOST).

===========================================================================*/
#define IS_EVENT( VAL )               SDSSSCR_CON_IS_EVENT, \
                                        WORD_HI( SDSS_EVT_##VAL ), \
                                        WORD_LO( SDSS_EVT_##VAL ),

/*===========================================================================

IS_SLEEP_TIME_SET( VAL )

DEFINITION
  Evaluates to TRUE iff VAL is equal the current SS-Event.

  VAL can take on any one of the SS_EVT_XXX SS-Events.

EXAMPLE
  IS_EVENT( CDMA_OPR_SYS_LOST ) - Evaluates to TRUE iff the current SS-Event
  is system lost (i.e., SDSS_EVT_CDMA_OPR_SYS_LOST).

===========================================================================*/
#define IS_GW_SLEEP_TIME_SET        SDSSSCR_CON_IS_GW_SLEEP_TIME_SET, 


/*===========================================================================

IS_AWAKE_TIME_SET( VAL )

DEFINITION
  Evaluates to TRUE iff VAL is equal the current SS-Event.

  VAL can take on any one of the SS_EVT_XXX SS-Events.

EXAMPLE
  IS_EVENT( CDMA_OPR_SYS_LOST ) - Evaluates to TRUE iff the current SS-Event
  is system lost (i.e., SDSS_EVT_CDMA_OPR_SYS_LOST).

===========================================================================*/
#define IS_GW_AWAKE_TIME_SET     SDSSSCR_CON_IS_GW_AWAKE_TIME_SET, 
                              


/* <EJECT> */
/*===========================================================================

IS_OPR_MODE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the current operating mode.

  VAL can take on one of the following values:

    PWR_UP        - Powering-up.
    ONLINE        - Online (CDMA/AMPS/HDR).
    OFFLINE_CDMA  - Offline CDMA.
    OFFLINE_AMPS  - Offline AMPS.
    OFFLINE_ANY   - Offline, CDMA or AMPS.
    LPM           - Low Power Mode.

EXAMPLE
  IS_OPR_MODE( OFFLINE_CDMA ) - Evaluates to TRUE iff the current operating
  mode offline CDMA (i.e., SDSS_OPR_MODE_OFFLINE_CDMA).

===========================================================================*/
#define IS_OPR_MODE( VAL )            SDSSSCR_CON_IS_OPR_MODE, \
                                        SDSSSCR_OPR_MODE_VAL_##VAL,


/* <EJECT> */
/*===========================================================================

IS_PREF_REAS( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current SS-Preference reason.

  VAL can take on one of the following values:

    USER            - User changed the SS-Preference.
    USER_RESEL      - User updated the SS-Preference to initiate reselection.
    ORIG_START      - CM changed the SS-Preference to originate a cs or ps
                      call.
    REDIAL_NORM     - CM changed the SS-Preference as part of normal silent
                      redial.
    REDIAL_OPTI     - CM changed the SS-Preference as part of optimized
                      silent redial.
    ORIG_END        - CM changed the SS-Preference to end a call origination.
    CALL_ORIG       - Any of ORIG_START, REDIAL_NORM or REDIAL_OPTI.
    ORIG_START_CS   - CM changed the SS-Preference to originate a cs call.
    ORIG_START_PS   - CM changed the SS-Preference to originate a ps call.
    ORIG_RESUME     - ps call out of dormant.

EXAMPLE
  IS_PREF_REAS( ORIG_START ) - Evaluates to TRUE iff the reason for
  changing the SS-Preference is starting a call origination.

===========================================================================*/
#define IS_PREF_REAS( VAL )       SDSSSCR_CON_IS_PREF_REAS, \
                                    WORD_HI( SDSSSCR_PREF_REAS_VAL_##VAL ), \
                                    WORD_LO( SDSSSCR_PREF_REAS_VAL_##VAL ),

/*===========================================================================

IS_FLAG_SET( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current SS-Preference reason.

  VAL can take on one of the following values:

    SDSSSCR_FLAG_SYNC_UP_IN_PROGRESS   - is stack sync-up in process
    SDSSSCR_FLAG_TRUE_EVENT_PROCESSED  - is true event being processed

EXAMPLE
  IS_FLAG_SET( SDSSSCR_FLAG_SYNC_UP_IN_PROGRESS ) - Evaluates to TRUE iff 
  the sync-up flag is TRUE.

===========================================================================*/
#define IS_FLAG_SET( FLAG )       SDSSSCR_CON_IS_FLAG_SET, \
                                    SDSS_FLAG_##FLAG,

/* <EJECT> */
/*===========================================================================

IS_ORIG_MODE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the current origination mode.

  VAL can take on one of the following values:

    NONE            - Not in call origination.
    NORMAL          - Normal call origination (i.e. call other than OTASP or
                      emergency).
    COLLOC          - Collocated system origination (i.e., try originating on
                      a collocated system.
    OTASP           - OTASP call origination.
    EMERG_ORIG      - Emergency call origination.
    EMERG_CB        - Emergency callback mode.
    EMERG           - Emergency - origination or callback mode.
    OTASP_OR_EMERG  - OTASP or emergency (orig or callback)
    ANY             - Any call origination.
    MANUAL          - User selected a network to acquire.
    MAN_OR_NORM     - User selected a network or normal call origination.
    MAN_OR_NONE     - User selected a network or no call origination.

EXAMPLE
  IS_ORIG_MODE( OTASP ) - Evaluates to TRUE iff the current origination
  mode is OTASP (i.e., SD_SS_ORIG_MODE_OTASP).

===========================================================================*/
#define IS_ORIG_MODE( VAL )           SDSSSCR_CON_IS_ORIG_MODE, \
                                        SD_SS_ORIG_MODE_VAL_##VAL,


/* <EJECT> */
/*===========================================================================

IS_STATE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equal the current SS-State.

  VAL can take on one of the following values:

    ACQ           - Acquisition state.
    VRFY          - Verify state.
    OPR           - Operation state.

EXAMPLE
  IS_STATE( ACQ ) - Evaluates to TRUE iff the current SS-State is
  acquisition.

===========================================================================*/
#define IS_STATE( VAL )               SDSSSCR_CON_IS_STATE, \
                                        SDSS_STATE_##VAL,

/* <EJECT> */
/*===========================================================================

IS_ACQ_STATE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the current acquisition state.

  VAL can take on one of the following values:

    START         - Start acquisition state.
    MORE_PREF     - More preferred acquisition state.
    REACQ         - Reacquire acquisition state.
    DONE          - Done acquisition state.

EXAMPLE
  IS_ACQ_STATE( REACQ ) - Evaluates to TRUE iff the current acquisition state
  is SDSS_ACQ_STT_REACQ.

===========================================================================*/
#define IS_ACQ_STATE( VAL )           SDSSSCR_CON_IS_ACQ_STATE, \
                                        SDSS_ACQ_STT_##VAL,


/* <EJECT> */
/*===========================================================================

IS_ACQ_STATE2( SS, VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the current acquisition state for SS.

  VAL can take on one of the following values:

    START         - Start acquisition state.
    MORE_PREF     - More preferred acquisition state.
    REACQ         - Reacquire acquisition state.
    DONE          - Done acquisition state.

EXAMPLE
  IS_ACQ_STATE2( MAIN, REACQ ) - Evaluates to TRUE iff the current acquisition state
  for MAIN is SDSS_ACQ_STT_REACQ.

===========================================================================*/
#define IS_ACQ_STATE2( SS, VAL )      SDSSSCR_CON_IS_ACQ_STATE2, \
                                        SD_SS_##SS, \
                                        SDSS_ACQ_STT_##VAL,
/* <EJECT> */
/*===========================================================================

IS_ACQ_TYPE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equal the type of the most recent acquisition
  attempt.

  VAL can take on one of the acquisition type values that are specified by
  the sdssscr_acq_e_type enum.

EXAMPLE
  IS_ACQ_TYPE( BEST_PREF ) - Evaluates to TRUE iff the type of the most
  recent acquisition attempt is SDSSSCR_ACQ_BEST_PREF.

===========================================================================*/
#define IS_ACQ_TYPE( VAL )           SDSSSCR_CON_IS_ACQ_TYPE, \
                                        SDSSSCR_ACQ_##VAL,


/*===========================================================================

IS_GW_SCAN_STATUS( VAL )

DEFINITION
  Evaluates to TRUE iff the GW scan status is set to VAL.

  VAL can take on one of the following values:

    NONE         - Scan status not set
    COMPLETE     - Scan Completed
    ABORTED      - Scan aborted due to timer expiry
    

EXAMPLE
  IS_GW_SCAN_STATUS( COMPLETE ) - Evaluates to TRUE iff the GW scan completed

===========================================================================*/
#define IS_GW_SCAN_STATUS( VAL )     SDSSSCR_CON_IS_GW_SCAN_STATUS, \
                                        SD_SS_GW_SCAN_STATUS_##VAL,


/* <EJECT> */
/*===========================================================================

IS_REDIR( VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the current/proposed redirection
  type.

  VAL can take on one of the following values:

    NONE            - Not in redirection.

    CDMA_SRDM       - CDMA SRDM redirection.
    CDMA_GSRDM      - CDMA GSRM redirection.
    CDMA_NDSS       - CDMA NDSS redirection.
    CDMA_GSRDM_NDSS - CDMA GSRDM or NDSS redirection.
    CDMA_ANY        - Any CDMA redirection

    HDR_QC          - HDR QC redirection
    HDR_DIRECTED    - HDR directed redirection
    HDR_ANY         - Any HDR redirection

    ANY             - ANY redirection (CDMA or HDR)

EXAMPLE
  IS_REDIR( CDMA_SRDM ) - Evaluates to TRUE iff the current/proposed
  redirection type is SRDM (i.e., SD_SS_REDIR_CDMA_SRDM).

===========================================================================*/
#define IS_REDIR( VAL )               SDSSSCR_CON_IS_REDIR, \
                                        SDSS_REDIR_VAL_##VAL,


/* <EJECT> */
/*===========================================================================

IS_HANDOFF( VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the current/proposed handoff.

  VAL can take on one of the following values:

    NONE - Not in handoff.

    CDMA to CDMA
    ------------
    CDMA_CAM_TO_CDMA_PAG   - CDMA CAM to CDMA paging channel.
    CDMA_CAM_TO_CDMA_TRF   - CDMA CAM to CDMA traffic channel.
    CDMA_CAM_TO_CDMA_ANY   - Any CDMA CAM to CDMA.

    CDMA_HDM_TO_CDMA_TRF   - CDMA HDM to CDMA traffic channel.
    CDMA_ANY_TO_CDMA_ANY   - Any CDMA to CDMA handoff.

    CDMA to AMPS
    ------------
    CDMA_CAM_TO_AMPS_SYS   - CDMA CAM to AMPS system (A/B).
    CDMA_CAM_TO_AMPS_VOICE - CDMA CAM to AMPS voice channel.
    CDMA_CAM_TO_AMPS_ANY   - Any CDMA CAM to AMPS handoff.

    CDMA_HDM_TO_AMPS_VOICE - CDMA HDM to AMPS voice channel.

    CDMA_ANY_TO_AMPS_VOICE - Any CDMA to AMPS voice handoff
    CDMA_ANY_TO_AMPS_ANY   - Any CDMA to AMPS handoff.

    Combinations
    ------------
    CDMA_CAM_TO_ANY        - Any CDMA CAM
    CDMA_HDM_TO_ANY        - Any CDMA HDM.
    CDMA_ANY_TO_ANY        - Any CDMA handoff.

EXAMPLE
  IS_HANDOFF( CDMA_CAM_TO_AMPS_SYS ) - Evaluates to TRUE iff the
  current/proposed handoff is CDMA-CAM to AMPS system.
  (i.e., SDSS_HO_VAL_CDMA_CAM_TO_AMPS_SYS).

===========================================================================*/
#define IS_HANDOFF( VAL )        SDSSSCR_CON_IS_HANDOFF, \
                                    WORD_HI( SDSS_HO_VAL_##VAL ), \
                                    WORD_LO( SDSS_HO_VAL_##VAL ),

/* <EJECT> */
/*===========================================================================

IS_RESEL_STT( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current reselection state.

  VAL can take on one of the following values:

    NONE      - Not doing reselection.

    WAIT      - Waiting for next reselection period.
    SRCH      - Searching for more preferred system.
    REACQ     - Reacquiring the serving system.

    SS_PREF   - Reselection after SS-Pref change.
    RELEASE   - Reselection after call is released.

EXAMPLE
  IS_RESEL_STT( SRCH ) - Evaluates to TRUE iff the current reselection state
  is SDSS_RESEL_STT_SRCH.

===========================================================================*/
#define IS_RESEL_STT( VAL )           SDSSSCR_CON_IS_RESEL_STT, \
                                        SDSSSCR_RESEL_STT_##VAL,


/* <EJECT> */
/*===========================================================================

IS_RESEL_MODE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current reselection mode OR
  if VAL == RESEL_MODE_FULL_SRV, then to TRUE if current reselection mode
  is between SDSS_RESEL_MODE_FULL_SRV & SDSS_RESEL_MODE_FULL_SRV_MAX.

  VAL can take on one of the following values:

    INIT      - Initial reselection.
    NORM      - Normal reselection.
    FULL_SRV  - *Any* stage of the full service reselection.
    FULL_SRV_1 - First stage of the full service reselection.
    FULL_SRV_2 - Second stage of the full service reselection.
    FULL_SRV_3 - Third stage of the full service reselection.
    FULL_SRV_4 - Fourth stage of the full service reselection.
    FULL_SRV_5 - Fifth stage of the full service reselection.

EXAMPLE
  IS_RESEL_MODE( INIT ) - Evaluates to TRUE iff the current reselection mode
  is SDSS_RESEL_MODE_INIT.
  IS_RESEL_MODE( FULL_SRV ) - Evaluates to TRUE iff the current reselection mode
  is between SDSS_RESEL_MODE_FULL_SRV and (SDSS_RESEL_MODE_FULL_SRV_MAX - 1).

===========================================================================*/
#define IS_RESEL_MODE( VAL )          SDSSSCR_CON_IS_RESEL_MODE, \
                                        SDSS_RESEL_MODE_##VAL,


/* <EJECT> */
/*===========================================================================

IS_CDMA_AVAIL( CNT )

DEFINITION
  Evaluates to TRUE iff CDMA-available counter is < CNT.

EXAMPLE
  IS_CDMA_AVAIL( 6 ) - Evaluates to TRUE iff CDMA-available counter < 6.

===========================================================================*/
#define IS_CDMA_AVAIL( CNT )          SDSSSCR_CON_IS_CDMA_AVAIL, \
                                        CNT,


/* <EJECT> */
/*===========================================================================

IS_SRV_STATUS( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current srv status

  VAL can take on one of the following values:

    NO_SRV            - No service.
    SRV               - Full service.
    LIMITED           - Limited service.
    LIMITED_REG       - Limited service - regional.
    LIMITED_ANY       - Limited or limited regional.
    SRV_ANY           - Full or Limited or limited regional service.

EXAMPLE
   IS_SRV_STATUS( LIMITED ) - Evaluates to TRUE iff srv_status = LIMITED.
===========================================================================*/
#define IS_SRV_STATUS( VAL )        SDSSSCR_CON_IS_SRV_STATUS, \
                                        SDSSSCR_SRV_STATUS_VAL_##VAL,


/*===========================================================================

IS_SRV_STATUS2( SS, VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current srv status of SS

  VAL can take on one of the following values:

    NO_SRV            - No service.
    SRV               - Full service.
    LIMITED           - Limited service.
    LIMITED_REG       - Limited service - regional.
    LIMITED_ANY       - Limited or limited regional.
    SRV_ANY           - Full or Limited or limited regional service.

EXAMPLE
   IS_SRV_STATUS2( MAIN, LIMITED ) - Evaluates to TRUE iff srv_status = LIMITED.
   for MAIN
===========================================================================*/
#define IS_SRV_STATUS2( SS, VAL )   SDSSSCR_CON_IS_SRV_STATUS2, \
                                        SD_SS_##SS, \
                                        SDSSSCR_SRV_STATUS_VAL_##VAL,


/* <EJECT> */
/*===========================================================================

IS_SRV_DOMAIN( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current srv domain

  VAL can take on one of the following values:

    NO_SRV            - No service.
    CS                - CS service or CS_PS service.
    PS                - PS service or CS_PS service.
    CS_OR_PS          - CS or PS or CS_PS service.
    CS_AND_PS         - only CS_PS service.

EXAMPLE
    IS_SRV_DOMAIN( PS ) - Evaluates to TRUE iff srv_domain = PS or CS_PS.
===========================================================================*/
#define IS_SRV_DOMAIN( VAL )        SDSSSCR_CON_IS_SRV_DOMAIN, \
                                        SDSSSCR_SRV_DOMAIN_VAL_##VAL,

/* <EJECT> */
/*===========================================================================

IS_ACC_REAS( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the latest access attempt reason.

  VAL can take on one of the following values:

    REG         - Registration.
    ORIG        - Origination.
    PAGE        - Page response.
    DATA        - Data burst.
    TRAFFIC     - Traffic channel (to idle transition)
    OTHER       - Reason other than the above.

EXAMPLE
  IS_ACC_REAS( REG ) - Evaluates to TRUE iff the latest access attempt
  reason is registration (i.e., SD_SS_ACC_REAS_REG).

===========================================================================*/
#define IS_ACC_REAS( VAL )            SDSSSCR_CON_IS_ACC_REAS, \
                                        SD_SS_ACC_REAS_##VAL,


/* <EJECT> */
/*===========================================================================

IS_ACC_TERM( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the latest access attempt termination
  reason.

  VAL can take on one of the following values:

    ACC_DEN     - Access denied.
    TIMEOUT     - Access state timeout.
    MAX_ACC     - Max access probes being transmitted.
    REORDER     - Receives a reorder order.
    INTERCEPT   - Receives an intercept order.
    BS_REL      - Receives a release order.
    MS_REL      - User releases call.
    CON_DEN     - HDR connection deny with reason set to general.
    NET_BUSY    - HDR connection deny with reason set to network busy.
    AUTH_FAIL   - HDR connection deny with reason set to authentication.
    ACC_FAIL    - Access failure for reason other than one of the above.
    NORMAL      - Normal access completion.

EXAMPLE
  IS_ACC_TERM( MAX_ACC ) - Evaluates to TRUE iff the latest access attempt
  termination reason is Max Access Probes
  (i.e., SD_SS_ACC_TERM_MAX_ACC).

===========================================================================*/
#define IS_ACC_TERM( VAL )            SDSSSCR_CON_IS_ACC_TERM, \
                                        SD_SS_ACC_TERM_##VAL,


/* <EJECT> */
/*===========================================================================

IS_ACC_FAIL( CNT, TIME )

DEFINITION
  Evaluates to TRUE iff registration access has failed at least CNT times and
  TIME minutes.

EXAMPLE
  IS_ACC_FAIL( 5, 15 ) - Evaluates to TRUE iff registration access has failed
  at least 5 times and 15 minutes.

===========================================================================*/
#define IS_ACC_FAIL( CNT, TIME )    SDSSSCR_CON_IS_ACC_FAIL, \
                                        CNT, \
                                        TIME,


/* <EJECT> */
/*===========================================================================

IS_SYS_LOST( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the latest system lost reason.

  VAL can take on one of the following values:
  
    LOST_ACQ_AFTER_SLEEP  - Could not reacquire after sleep 

    RUP_PILOT_SUPERVISION - [6.6.5.5.2]  The strongest pilot is too weak for 
                                         too long

    SYNC_MSG_TIMEOUT      - Too long in sync state waiting for sync message

    NO_RESOURCES          - Lock in RF Resources was absent for too long

    MSTR_SLAM_TIMEOUT     - MSTR slam could not be completed for too long due 
                            to unlocked fingers

    RESELECT              - HDR signal is weak - may be moving out of 
                            coverage

    ADVISE_UNLOCK_RF      - HDR was told to release the RF lock if it held it

    SYNC_WEAK_HDR_COVERAGE- HDR coverage is weak at sync state

    DSP_NOT_READY         - DSP application is not ready 

    SUP_FAILURE           - Supervision failure

    PROT_Q_FULL           - Protocol Queue Full - force reacquisition

    UNKNOWN               - Unknown reason


EXAMPLE
  IS_SYS_LOST( ADVISE_UNLOCK_RF ) - Evaluates to TRUE iff the latest system
  lost reason is to release the RF lock.
  (i.e., SD_SS_SYS_LOST_ADVISE_UNLOCK_RF).

===========================================================================*/
#define IS_SYS_LOST( VAL )            SDSSSCR_CON_IS_SYS_LOST, \
                                        SD_SS_SYS_LOST_##VAL,


/* <EJECT> */
/*===========================================================================

IS_FADE( SYS, CNT )

DEFINITION
  Evaluates to TRUE iff there are at least CNT times fades on reference
  system.
EXAMPLE
  IS_FADE( HDR_LOST_SYS, 3 ) - Evaluates to TRUE iff there are at least 3
  fades on the lost channel.
===========================================================================*/
#define IS_FADE( SYS, CNT )              SDSSSCR_CON_IS_FADE, \
                                         SDSR_##SYS, \
                                         CNT,
/* <EJECT> */
/*===========================================================================

IS_EVENT_CNT ( EVENT, CNT, TIME )

DEFINITION
  Evaluates to TRUE iff EVENT has occurred CNT times within TIME minutes.

EXAMPLE
  IS_EVENT_CNT ( SES_NEGO_TIMEOUT, 5, 15 ) - Evaluates to TRUE iff session
  negotiation timeout has occurred at least 5 times in the last 15 minutes.

===========================================================================*/
#define IS_EVENT_CNT( EVENT, CNT, TIME )  SDSSSCR_CON_IS_EVENT_CNT, \
                                           SDSS_CNTED_EVT_##EVENT, \
                                           CNT, \
                                           TIME,

/* <EJECT> */
/*===========================================================================

IS_SID( LST, VAL )

DEFINITION
  Evaluates to TRUE iff VAL describes the SID/NID to expected SID/NID
  relation of at least one of the systems that are referenced by the system
  record list LST.

  VAL can take on one of the following values:

    EXPECTED      - The SID/NID match the expected SID/NID.
    NOT_EXPECTED  - The SID/NID does not match the expected SID/NID.
    COLLOC_IDL_SYS  - The sys is collocated (associated) with SDSR_IDL_SYS.

EXAMPLE
  IS_SID( SRV_SYS, EXPECTED ) - Evaluates to TRUE iff the SID/NID field of
  the SDSR_SRV_SYS match the expected SID/NID field of the serving system.

===========================================================================*/
#define IS_SID( LST, VAL )            SDSSSCR_CON_IS_SID, \
                                        SDSR_##LST, \
                                        SDSSSCR_SID_COND_##VAL,


/* <EJECT> */
/*===========================================================================

IS_MODE( LST, VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the mode designation of at least one of
  the systems that are referenced by the system record list LST.

  VAL can take on one of the following values:

    CDMA - CDMA mode.
    AMPS - AMPS mode.
    HDR -  HDR mode.

EXAMPLE
  IS_MODE( REDIR_LST, AMPS ) - Evaluates to TRUE iff MS is being redirected
  to AMPS system (i.e., the mode designation of the systems that are
  referenced by SDSR_REDIR_LST is AMPS mode).

===========================================================================*/
#define IS_MODE( LST, VAL )           SDSSSCR_CON_IS_MODE, \
                                        SDSR_##LST, \
                                        SD_MODE_##VAL,

/*===========================================================================

IS_MODE_PREF( LST, VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the mode designation of at least one of
  the systems that are referenced by the system record list LST.

  VAL can take on one of the following values:

    CDMA - CDMA mode.
    AMPS - AMPS mode.
    HDR  - HDR mode.
    GW   - GSM or WCDMA

EXAMPLE
  IS_MODE_PREF( SRV_SYS, GW ) - Evaluates to TRUE iff MS is camped on either
  GSM or WCDMA system

===========================================================================*/
#define IS_MODE_PREF( LST, VAL )     SDSSSCR_CON_IS_MODE_PREF, \
                                        SDSR_##LST, \
                                        SD_SS_MODE_PREF_##VAL,

/* <EJECT> */
/*===========================================================================

IS_BAND( LST, VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the band designation of at least one of
  the systems that are referenced by the system record list LST.

  VAL can take on one of the following values:

    BC0 - Band class 0.
    BC1 - Band class 1.
    BC3 - Band class 3.
    BC4 - Band class 4.
    BC5 - Band class 5.
    BC6 - Band class 6.
    BC10 - Band class 10.
    BC11 - Band class 11.
    BC12 - Band class 12.
    BC14 - Band class 14.
    BC15 - Band class 15.   
    BC16 - Band class 16.


EXAMPLE
  IS_BAND( REDIR_LST, BC1 ) - Evaluates to TRUE iff MS is being redirected to
  band class 1 systems (i.e., the band designation of the systems that are
  referenced by the SDSR_REDIR_LST is band class 0).

===========================================================================*/
#define IS_BAND( LST, VAL )           SDSSSCR_CON_IS_BAND, \
                                        SDSR_##LST, \
                                        SD_BAND_##VAL,


/* <EJECT> */
/*===========================================================================

IS_PRL( LST, VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the PRL designation of at least one of
  the systems that are referenced by the system record list LST.

  VAL can take on one of the following values:

    NEG           - Negative system.
    UNAVAIL       - Unavailable system since PRL forbids available systems.
    FORBID        - NEG or UNAVAIL system.
    AVAIL         - Available system.

    PREF          - Preferred, but not the most in its GEO system.
    NOT_MOST_PREF - NEG, UNAVAIL, AVAIL or PREF system.
    MOST_PREF     - Most preferred in its GEO system.
    ANY_PREF      - PREF or MOST_PREF system.

    LISTED        - NEG, PREF or MOST_PREF system.
    ALLOWED       - AVAIL, PREF, or MOST_PREF system.

EXAMPLE
  IS_PRL( SRV_SYS, PREF ) - Evaluates to TRUE iff the serving system is a
  preferred PRL system, but not the most preferred in its GEO.

===========================================================================*/
#define IS_PRL( LST, VAL )            SDSSSCR_CON_IS_PRL, \
                                        SDSR_##LST, \
                                        SDSS_PRL_VAL_##VAL,


/* <EJECT> */
/*===========================================================================

IS_NET_SEL( LST, VAL )

DEFINITION

  Evaluates to TRUE iff VAL equals the network selection mode of at least one
  of the systems that are referenced by the system record list LST.

  VAL can take on one of the following values:

    LIMITED     - Limited service
    AUTO        - Automatic PLMN selection
    HPLMN       - Home PLMN
    PRL_PLMN    - PRL specified PLMN
    USER_PLMN   - User specified PLMN

EXAMPLE
  IS_NET_SEL( SRV_SYS, LIMITED ) - Evaluates to TRUE iff the network
  selection mode of the serving system is SD_NET_SELECT_MODE_LIMITED.

===========================================================================*/
#define IS_NET_SEL( LST, VAL )        SDSSSCR_CON_IS_NET_SEL, \
                                        SDSR_##LST, \
                                        SD_NET_SELECT_MODE_##VAL,


/* <EJECT> */
/*===========================================================================

IS_SCHED( LST, VAL )

DEFINITION
  Evaluates to TRUE iff VAL equal the reacquisition schedule of at least one
  of the systems that are referenced by the system record list LST.

EXAMPLE
  IS_SCHED( SRV_SYS, REACQ_PWR_UP ) - Evaluates to TRUE iff the serving
  system is on a SDSSSCR_SCHED_REACQ_PWR_UP reacquisition schedule.

===========================================================================*/
#define IS_SCHED( LST, VAL )          SDSSSCR_CON_IS_SCHED, \
                                        SDSR_##LST, \
                                        SDSSSCR_SCHED_##VAL,


/* <EJECT> */
/*===========================================================================

IS_PREF_CONF( LST, VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the SS-Preference conflict of at least
  one of the systems that are referenced by the system record list LST.

  VAL can take on one of the following values:

    NONE - Not conflicting with mode, band and roam preferences.
    ANY - Conflicts with mode, band or roam preference.
    MODE - Conflicts with mode preference.
    MODE_OR_BAND - Conflicts with mode or band preference.
    MODE_OR_ROAM - Conflicts with mode or roam preference.
    BAND - Conflicts with band preference.
    BAND_OR_ROAM - Conflicts with band or roam preference.
    ROAM - Conflicts with roam preference.

EXAMPLE
  IS_PREF_CONF( SRV_SYS, CONFLICT_ANY ) - Evaluates to TRUE iff the serving
  system is conflicts any of the SS-Preferences - mode, band or roam.
  (i.e., the SS-Preference designation of the system that is referenced by
  the SDSR_SRV_SYS is CONFLICT_ANY).

===========================================================================*/
#define IS_PREF_CONF( LST, VAL )      SDSSSCR_CON_IS_PREF_CONF, \
                                        SDSR_##LST, \
                                        SDSS_PREF_CONF_##VAL,


/* <EJECT> */
/*===========================================================================

IS_PREF_CHGD( VAL )

DEFINITION
  Evaluates to TRUE iff specified SS preference given by VAL was changed with
  the last preferred system changed.

  VAL can take on one of the following values:

    NONE - No preference changed with last preferred system changed
    MODE - mode preference changed with last preferred system changed
    ROAM - roam preference changed with last preferred system changed
    BAND - band preference changed with last preferred system changed
    HYBR - Hybrid preference changed with last preferred system changed
    DOMAIN - Domain preference changed with last preferred system changed
    ACQ_ORDER - Acqusition orider preference changed with last preferred
                system changed
    ANY - Any preference changed with last preferred system changed

EXAMPLE
  IS_PREF_CHGD( ROAM ) - Evaluates to TRUE iff the roam preference
  was changed with the last preferred system change.

===========================================================================*/
#define IS_PREF_CHGD( VAL )          SDSSSCR_CON_IS_PREF_CHGD, \
                                        SDSS_PREF_CHGD_##VAL,

/*===========================================================================

IS_BAND_PREF_CHGD( VAL )

DEFINITION
  Evaluates to TRUE iff specified band preference for a given  mode was changed with
  the last preferred system changed.

  VAL can take on one of the following values:

    CDMA  - CDMA mode.
    AMPS  - AMPS mode.
    HDR    - HDR mode.
    GW     - GSM or WCDMA
    ANY    - Any mode

EXAMPLE
  IS_BAND_PREF_CHGD( GW ) - Evaluates to TRUE iff the GW band preference
  was changed with the last preferred system change.

===========================================================================*/
#define IS_BAND_PREF_CHGD( VAL )      SDSSSCR_CON_IS_BAND_PREF_CHGD, \
                                          SD_SS_MODE_PREF_##VAL,

/*===========================================================================

IS_PREF_UPDATE( VAL )

DEFINITION
  Evaluates to TRUE iff specified SS preference update equals VAL
  
  VAL can take on one of the following values:

    EMERG_ORIG_END - Emergnecy call ended

EXAMPLE
  IS_PREF_UPDATE( EMERG_ORIG_END ) - Evaluates to TRUE iff the roam preference
  was changed with the last preferred system change.

===========================================================================*/
#define IS_PREF_UPDATE( VAL )        SDSSSCR_CON_IS_PREF_UPDATE, \
                                        SDSS_PREF_UPDATED_##VAL,
                                                                     
/* <EJECT> */
/*===========================================================================

IS_SAME( LST1, LST2, VAL )

DEFINITION
  Evaluates to TRUE iff LST1 and LST2 are the same with respect to the VAL
  equality category.

  VAL can take on one of the following values:

  SID_NID   - LST1 and LST2 reference the same set of SID/NIDs.
  MODE      - LST1 and LST2 reference the same set of modes.
  BAND      - LST1 and LST2 reference the same set of bands
  CHAN      - LST1 and LST2 reference the same set of channels.

EXAMPLE
  IS_SAME( SRV_SYS, ACQ_SYS, SID ) - Evaluates to TRUE iff the SID/NID of
  SDSR_SRV_SYS is the same as the SID/NID of SDSR_ACQ_SYS.

===========================================================================*/
#define IS_SAME( LST1, LST2, VAL )    SDSSSCR_CON_IS_SAME, \
                                        SDSR_##LST1, \
                                        SDSR_##LST2, \
                                        SDSR_EQUAL_##VAL,


/* <EJECT> */
/*===========================================================================

IS_DIFF( LST1, LST2, VAL )

DEFINITION
  Evaluates to TRUE iff LST1 and LST2 are the different with respect to the
  VAL equality category.

  VAL can take on one of the following values:

  SID_NID   - LST1 and LST2 reference different set of SID/NIDs.
  MODE      - LST1 and LST2 reference different set of modes.
  BAND      - LST1 and LST2 reference different set of bands
  CHAN      - LST1 and LST2 reference different set of channels.

EXAMPLE
  IS_SAME( SRV_SYS, ACQ_SYS, SID ) - Evaluates to TRUE iff the SID/NID of
  SDSR_SRV_SYS is the same as the SID/NID of SDSR_ACQ_SYS.

===========================================================================*/
#define IS_DIFF( LST1, LST2, VAL )    SDSSSCR_CON_IS_DIFF, \
                                        SDSR_##LST1, \
                                        SDSR_##LST2, \
                                        SDSR_EQUAL_##VAL,


/* <EJECT> */
/*===========================================================================

IS_CONTAIN( LST1, VAL, LST2 )

DEFINITION
  Evaluates to TRUE iff LST1 contains VAL elements of LST2.

  VAL can take on one of the following values:

  NONE  - LST1 contain no elements of LST2.
  SOME  - LST1 contain some elements of LST2.
  ALL   - LST1 contain all elements of LST2.
  SAME  - LST1 contain the same elements as LST2.

EXAMPLE
  IS_CONTAIN( SCHED_LST, NONE, ACQ_SYS ) - Evaluates to TRUE iff
  SDSR_SCHED_LST contains no elements of SDSR_ACQ_SYS.

===========================================================================*/
#define IS_CONTAIN( LST1, VAL, LST2 ) SDSSSCR_CON_IS_CONTAIN, \
                                        SDSR_##LST1, \
                                        SDSR_CONTAIN_##VAL, \
                                        SDSR_##LST2,


/* <EJECT> */
/*===========================================================================

IS_CNT( LST, CNT, CNT_COND, CNT_INC )

DEFINITION

  Evaluates to TRUE iff LST contains CNT or more/less/exactly elements that
  comply with the CNT_INC count inclusion criteria.

  CNT can take on any integer from 0 to 255.

  CNT_COND can take on one of the following values:

    OR_MORE   - List contains CNT or more elements.
    OR_LESS   - List contains CNT or less elements.
    EXACTLY   - List contains exactly CNT elements .

  CNT_INC can take on one of the following values:

    ALL       - Count all systems.
    COMPLY    - Count only systems that comply with the target
                capabilities and current mode and band preference.
    AVAIL     - Count only systems that are available for acquisition
                attempts.
    NEW,      - Count only systems that are available for acquisition
                attempts and comply with the new acquisition counter
                criteria.

EXAMPLE
  IS_CNT( IDL_PREF_LST, 1, OR_MORE, AVAIL ) - Evaluates to TRUE iff
  SDSR_IDL_PREF_LST contains one or more elements that are available for
  acquisition attempts.

===========================================================================*/
#define IS_CNT( LST, CNT, CNT_COND, CNT_INC ) \
                                        SDSSSCR_CON_IS_CNT, \
                                        SDSR_##LST, \
                                        CNT, \
                                        SDSSSCR_CNT_COND_##CNT_COND, \
                                        SDSSSCR_CNT_INC_##CNT_INC,


/* <EJECT> */
/*===========================================================================

IS_TIME_GR( INSTANCE, SRV criteria, TIME )

DEFINITION

  Evaluates to TRUE iff the instance is in SRV state for atleast TIME seconds

  e.g:
  IS_TIME_GR(MAIN, SRV, 10) evaluates to TRUE if the MAIN instance has
  service for more than 10 seconds

LIMITATIONS:

  Currently SRV_VAL can only be SRV_ANY or NO_SRV_ANY. 
  No other SRV_VAL's are supported

===========================================================================*/

#define IS_TIME_GR(INSTANCE, SRV_VAL, TIME) \
                                       SDSSSCR_CON_IS_TIME, \
                                       SD_SS_##INSTANCE, \
                                       SDSSSCR_SRV_STATUS_VAL_##SRV_VAL, \
                                       TIME,


/*===========================================================================

IS_SIM_STATE( SIM_SATE )

DEFINITION

  Evaluates to TRUE iff the phone's SIM state is equal to given
  sim_status

  e.g:
  IS_SIM_STATE( SIM_SATE ) evaluates to TRUE if the phone's sim status
  and the given sim_state are equal.

LIMITATIONS:


===========================================================================*/

#define IS_SIM_STATE( SIM_SATE ) \
                      SDSSSCR_CON_IS_SIM_STATE, \
                      SDSSSCR_SIM_STATE_##SIM_SATE,

/*===========================================================================

GW_PWR_SAVE_ACQ_STAGE_SET( STAGE )

DEFINITION
  Instructs the SS-Client to set current GW pwr save acquisition stage. 

  This is needed for GW only configuration, where UE should scan band groups
  in 4 + 1 combination (4 for MRU band group and 1 for other).

CONTROL
  Control moves to next construct.

EXAMPLE
  GW_PWR_SAVE_ACQ_STAGE_SET(0)

===========================================================================*/
#define GW_PWR_SAVE_ACQ_STAGE_SET( VAL ) SDSSSCR_CON_SET_GW_PWR_SAVE_ACQ_STAGE, \
                                        WORD_HI( VAL ), \
                                        WORD_LO( VAL ),
                      

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
========================== SYSTEM SELECTION SCRIPTS =========================
=============================================================================
=============================================================================
===========================================================================*/

/* Type for an SS-Script Element.
**
** Each SS-Script is stored as an array of SS-Script Elements. When reading
** the SS-Script, each script element is resolved to either an SS-Script
** Construct, or an SS-Script Constructs Parameter.
*/
typedef union {

  /* SS-Script construct.
  */
  byte                  con;


  /* SS-Script construct parameter.
  */
  byte                  prm;

} sdssscr_elem_u_type;



/* Definition for the NULL SS-Script (i.e. an SS-Script that is resolved to
** no script at all).
*/
#define   SDSSSCR_NULL    ( (sdssscr_elem_u_type*) NULL )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* JCDMA constants.
*/
#define MAX_ACQ_FAILS           5
#define DEEP_SLEEP_J1           10
#define DEEP_SLEEP_JSUB1        10
#define DEEP_SLEEP_J2           20
#define DEEP_SLEEP_JSUB2        20
#define OVERREACH_SLEEP_J1      60
#define OVERREACH_SLEEP_J2      120


/* <EJECT> */
/*===========================================================================
=============================================================================
========================== SCRIPTS FOR USER EVENTS ==========================
=============================================================================
===========================================================================*/

/*lint -e708 */ /* Info 708 union initialization */


/*lint -esym(528, ssscr_reserved_script) */ /* symbol not referenced */

static const sdssscr_elem_u_type  ssscr_reserved_script[] = {

  /* Condition */
  IS( FALSE )  

  /* Script */
  PREV_SCR
  END
};

/*---------------------------------------------------------------------------
                            SDSS_EVT_USER_PWR_UP
---------------------------------------------------------------------------*/


/* Script for when the user commands phone to power-up in emergency mode.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_up_emerg[] = {

  /* Condition */
  IS_EVENT( USER_PWR_UP )         /* User commands MS to power-up */
  IS_ORIG_MODE( EMERG )

  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/* Script for when the user commands phone to power-up (i.e., the SS-Core
** receives SDSS_EVT_USER_PWR_UP).
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_up[] = {

  /* Condition */
  IS_EVENT( USER_PWR_UP )         /* User commands MS to power-up */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */

  CLR( ACQ_GEO_LST )              /* Clear the acquired system's GEO list */
  CLR( IDL_GEO_LST )              /* Clear the idle system's GEO list */
  CLR( SRV_GEO_LST )              /* Clear the serving system's GEO list */

  CLR( ACQ_PREF_LST )             /* Clear the acq sys's preferred list */
  CLR( IDL_PREF_LST )             /* Clear the idle sys's preferred list */
  CLR( SRV_PREF_LST )             /* Clear the srv sys's preferred list */

  RESEL_STT( NONE )               /* Set reselection state to NONE */
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */

  //#ifdef FEATURE_JCDMA

  //CPY( SRCH_LST, PRL_LST )        /* Copy PRL_LST into SRCH_SYS */
  //DEL_SYS( SRCH_LST, NON_CDMA )   /* Delete non-CDMA systems from SRCH list */
  //MEAS( SRCH_LST, DED, PILOT )    /* Get pilot strength measurement*/
  //SORT( SRCH_LST, PILOT )         /* Sort list according to pilot strength */

  //NEW_ACQ                         /* Start a new acquisition */
  //ACQ_MODE( FULL )                /* Do full acquisitions */
  //ACQ( SRCH_LST, BEST_PREF )      /* Try acquiring systems */

  //#else /* FEATURE_JCDMA */

  //SRV_STATUS( NO_SRV )
  CPY( TEMP_SYS, MRU_LST )          /* Copy MRU_LST[0] to TEMP_SYS */

  SCHED_COND2( TEMP_SYS, REACQ_PWR_UP, BEST_PREF, \
               NOT_ON_EQUAL_OR_HIGHER_PRIORITY_SCHED_OR_AVOID) 
                                   /* Schedule the MRU for reacq conditionally */ 

                                     /* Schedule GW last full system reacq */
  DEL_SYS( SCHED_LST, GW_SYS)
                                      /* Do not schedule GW sys in MRU LST.*/
  #ifdef FEATURE_UNIFORM_SCAN_OOS
  DEL_SYS( SCHED_LST, NON_CDMA ) 
  DEL_SYS( SCHED_LST, HDR )      /* Do not schedule HDR sys in MRU list */
  #endif

  //#endif /* FEATURE_JCDMA */

  ACT_SCR( SRV_LOST_NORM_SLNT )       /* Do normal service lost processing */
  END
};


/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_OFFLINE_CDMA
---------------------------------------------------------------------------*/

/* Script for when the user commands phone to enter offline CDMA
** mode of operation.
*/
static const  sdssscr_elem_u_type   ssscr_user_offline_cdma[] = {

  /* Condition */
  IS_EVENT( USER_OFFLINE_CDMA )   /* User commands MS to enter offline CDMA*/

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  CONT( 0 )                       /* Continue with CDMA offline operation */
  END
};

/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_OFFLINE_AMPS
---------------------------------------------------------------------------*/

/* Script for when the user commands phone to enter offline AMPS
** mode of operation.
*/
static const  sdssscr_elem_u_type   ssscr_user_offline_amps[] = {

  /* Condition */
  IS_EVENT( USER_OFFLINE_AMPS )   /* User commands MS to enter offline AMPS*/

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  CONT( 0 )                       /* Continue with AMPS offline operation */
  END
};

/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_PWR_SAVE_EXIT
---------------------------------------------------------------------------*/

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)

/* Script for when the user commands phone to exit power-save mode
** of operation and the target is GW only and SD is in manual mode.
** The phone is currently doing powersave wake up acquisitions. 
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_save_exit_gw_manual_wakeup_acq[] = {

  /* Condition */
  IS_EVENT( USER_PWR_SAVE_EXIT )  /* User commands MS to exit power-save */
  IS( CAP_GW_PREF_GW_ONLY )       /* Capable for GSM/WCDMA but not 1x */
  IS_ORIG_MODE( MANUAL )          /* SD in manual mode. */
  NOT IS_ACQ_STATE( NONE )        /* Doing wakeup acquisitions. */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
  ORDER( MANUAL_LST, GW_LAST_USER_SYS )/* Order by last user system */

  CLR( GW_LAST_LTD_SYS )          /* Clear the last limited system */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_STATUS( NO_SRV )            /* Turn off service status */
  ACT_SCR( SRV_LOST_GW )          /* Do ervice lost scr  */
  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  SRV_STATUS( PWR_SAVE )          /* Change the status to Power Save */
  PREV_SCR                        /* Continue with previous script */
  #endif
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user commands phone to exit power-save mode
** of operation and the target is GW only and SD is in manual mode.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_save_exit_gw_manual[] = {

  /* Condition */
  IS_EVENT( USER_PWR_SAVE_EXIT )  /* User commands MS to exit power-save */
  IS( CAP_GW_PREF_GW_ONLY )       /* Capable for GSM/WCDMA but not 1x */
  IS_ORIG_MODE( MANUAL )          /* SD in manual mode. */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
  ORDER( MANUAL_LST, GW_LAST_USER_SYS )/* Order by last user system */

  CLR( GW_LAST_LTD_SYS )          /* Clear the last limited system */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_STATUS( NO_SRV )            /* Turn off service status */
  ACT_SCR( SRV_LOST_GW )          /* Do ervice lost scr  */
  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  PWR_SAVE                        /* Enter power save again. */
  SRV_STATUS( PWR_SAVE )          /* Change the status to Power Save */
  PREV_SCR                        /* Continue with previous script */
  #endif
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user commands phone to exit power-save mode
** of operation and the target is GW only and no system has been acquired
** since power up. The phone is currently doing powersave wake up
** acquisitions.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_save_exit_gw_pwrup_wakeup_acq[] = {

  /* Condition */
  IS_EVENT( USER_PWR_SAVE_EXIT )  /* User commands MS to exit power-save */
  IS( CAP_GW_PREF_GW_ONLY )       /* Capable for GSM/WCDMA but not 1x */
  IS_CNT( IDL_SYS, 0, EXACTLY, ALL)/* IDL_SYS is empty */
  NOT IS_ACQ_STATE( NONE )        /* Doing wakeup acquisitions. */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST,GW_LST )
  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH_LST according to MRU LST*/
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_STATUS( NO_SRV )            /* Turn off service status */
  ACT_SCR( SRV_LOST_GW_PWRUP )    /* Do service lost scr for powe rup */
  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  SRV_STATUS( PWR_SAVE )          /* Change the status to Power Save */
  PREV_SCR                        /* Continue with previous script */
  #endif
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user commands phone to exit power-save mode
** of operation and the target is GW only and no system has been acquired
** since power up.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_save_exit_gw_pwrup[] = {

  /* Condition */
  IS_EVENT( USER_PWR_SAVE_EXIT )  /* User commands MS to exit power-save */
  IS( CAP_GW_PREF_GW_ONLY )       /* Capable for GSM/WCDMA but not 1x */
  IS_CNT( IDL_SYS, 0, EXACTLY, ALL)/* IDL_SYS is empty */


  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST,GW_LST )
  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH_LST according to MRU LST*/
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_STATUS( NO_SRV )            /* Turn off service status */
  ACT_SCR( SRV_LOST_GW_PWRUP )    /* Do service lost scr for powe rup */
  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  PWR_SAVE                        /* Enter power save again. */
  SRV_STATUS( PWR_SAVE )          /* Change the status to Power Save */
  PREV_SCR                        /* Continue with previous script */
  #endif
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user commands phone to exit power-save mode
** of operation and the target is GW only. The phone is currently doing
** powersave wake up acquisitions.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_save_exit_gw_wakeup_acq[] = {

  /* Condition */
  IS_EVENT( USER_PWR_SAVE_EXIT )  /* User commands MS to exit power-save */
  IS( CAP_GW_PREF_GW_ONLY )       /* Capable for GSM/WCDMA but not 1x */
  NOT IS_ACQ_STATE( NONE )        /* Doing wakeup acquisitions. */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GW_LST )         /* Append GW systems to SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/
  ORDER2( SRCH_LST, IDL_SYS, BAND )
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_STATUS( NO_SRV )            /* Turn off service status */
  ACT_SCR( SRV_LOST_GW )          /* Do service lost scr */
  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  SRV_STATUS( PWR_SAVE )          /* Change the status to Power Save */
  PREV_SCR                        /* Continue with previous script */
  #endif
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user commands phone to exit power-save mode
** of operation and the target is GW only.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_save_exit_gw[] = {

  /* Condition */
  IS_EVENT( USER_PWR_SAVE_EXIT )  /* User commands MS to exit power-save */
  IS( CAP_GW_PREF_GW_ONLY )       /* Capable for GSM/WCDMA but not 1x */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GW_LST )         /* Append GW systems to SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/
  ORDER2( SRCH_LST, IDL_SYS, BAND )
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_STATUS( NO_SRV )            /* Turn off service status */
  ACT_SCR( SRV_LOST_GW )          /* Do service lost scr */
  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  PWR_SAVE                        /* Enter power save again. */
  SRV_STATUS( PWR_SAVE )          /* Change the status to Power Save */
  PREV_SCR                        /* Continue with previous script */
  #endif
  END
};

#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user commands phone to exit power-save mode
** of operation.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_save_exit[] = {

  /* Condition */
  IS_EVENT( USER_PWR_SAVE_EXIT )  /* User commands MS to exit power-save */

  /* Script */
  //RESEL_STT( NONE )               /* Set reselection state to NONE */
  PROC_EVT( USER_PWR_UP )         /* Do power-up event processing */
  END
};

/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_SS_PREF
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                              GPS Scripts
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user wants to end GPS mode and:
**
** 1. Origination mode is NONE.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_gps_end[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_MODE( ACQ_SYS, GPS )         /* Acq sys is the GPS sys */
  IS_PREF_CONF( ACQ_SYS, MODE )   /* Acq sys conflicts with mode pref */
  IS_ORIG_MODE( NONE )            /* Not trying to originate a call */

  /* Script */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Acq failed - do sys lost event proc */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user wants to avoid the serving system.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_avoid_sys[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( AVOID_SYS )       /* SS-Pref is changed to avoid system */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The call needs to be handed off from HDR to 1X
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_hdr_cdma_ho[] = {

  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( HDR_CDMA_HO )     /* Call is being handed off to 1X */

  /* Script */
  ACT_SCR( SRV_LOST_HDR_CDMA_HO )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when user changes the SS-Preference and:
**
** 1. The origination mode change is not normal (i.e. other than none to
**    normal or normal to none).
** 2. Origination mode is not emergency (origination or callback).
** 3. The SCHED_LST is not empty (i.e. one or more systems are currently
**    scheduled for reacquisition attempts).
** 4. GW only target should skip this scripts.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_not_norm[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_NOT( ORIG_MODE_NORM_CNG )    /* Not a normal origination mode change */
  NOT IS_ORIG_MODE( EMERG )       /* Origination mode is not emergency */
  IS_CNT( SCHED_LST, 1, OR_MORE, ALL ) /* 1 or more systems sched for reacq*/
  NOT IS( CAP_GW_PREF_GW_ONLY )          /* Not GW only targets */

  /* Script */
  CLR( SCHED_LST )                /* Cancel all reacquisition schedules */
  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UNIFORM_SCAN_OOS

/* Script for when user changes the SS-Preference and:
**
** 1. The origination mode change is not normal (i.e. other than none to
**    normal or normal to none).
** 2. Origination mode is not emergency (origination or callback).
** 3. The SCHED_LST is not empty (i.e. one or more systems are currently
**    scheduled for reacquisition attempts).
** 4. GW only target should skip this scripts.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_uoos_not_norm[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_NOT( ORIG_MODE_NORM_CNG )    /* Not a normal origination mode change */
  NOT IS_STATE( OPR )             /* State is Not operation */
  NOT IS_ORIG_MODE( EMERG )       /* Origination mode is not emergency */
  NOT IS( CAP_GW_PREF_GW_ONLY )   /* Not GW only targets */
  IS( FEATURE_UOOS )              /* Is this a UOOS build */
  
  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
///* Script for when user changes the SS-Preference and:
//**
//** 1. User trying to originate a call except for manual mode.
//** 2. GW only target.
//** 3. The SCHED_LST contains one or more GW limited systems. ( not for power up)
//** 4. Delete the GW limited systems from schedule list.
//** 5. Reschedule the GW full system.
//*/
//static const sdssscr_elem_u_type  ssscr_user_ss_pref_gw_resched_orig[] = {
//
//  /* Condition */
//  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
//  NOT IS_ORIG_MODE( MAN_OR_NONE )/* User trying to originate a call
//                                  ** except for orig mode manual or none.*/
//  IS_CONTAIN( SCHED_LST, SOME, GW_LTD_LST )
//                                  /* The schedule list contains GW ltd list*/
//  IS( CAP_GW_PREF_GW_ONLY )              /*  GW only target.*/
//
//
//  /* Script */
//  ACT_SCR( GW_RESCHED_ORIG )      /* Do reschedule when there is call
//                                  ** origination. */
//  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
//  PREV_SCR                        /* Continue with previous script */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when user changes the SS-Preference and:
//**
//** 1. When call is end.
//** 2. GW only target.
//** 3. Phone is not in operation.
//** 4. The SCHED_LST is not empty, ( not in power up state )
//** 4. The SCHED_LST does not contains any GW limited systems.
//** 4. Re-schedule both GW systems.
//*/
//static const sdssscr_elem_u_type  ssscr_user_ss_pref_gw_resched[] = {

//  /* Condition */
//  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
//  IS_ORIG_MODE( NONE )             /* Not originating a call */
//  IS_CNT( SCHED_LST, 1, OR_MORE, COMPLY )
//                                  /* The schedule list has one or  more
//                                  ** complied system. */
//  NOT IS_CONTAIN( SCHED_LST, SOME, GW_LTD_LST )
//                                  /* The schedule list does not contains
//                                  ** GW ltd list*/
//  IS( CAP_GW_PREF_GW_ONLY )              /*  GW only target.*/
//
//
//  /* Script */
//  ACT_SCR( GW_RESCHED )     /* DO Reschedule */
//  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
//  PREV_SCR                        /* Continue with previous script */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The origination mode is not emergency.
** 2. The emergency acquisition list is not empty.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_emerg_list_full[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  NOT IS_ORIG_MODE( EMERG )       /* Not originating an emergency call */
  IS_CNT( EMERG_LST, 1, OR_MORE, ALL ) /* Emergency list is not empty */

  /* Script */
  CLR( EMERG_LST )                /* Clear emergency acquisition list */
  CLR( EMERG_SYS )                /* Clear last acquired emergency system */
  //CPY( IDL_SYS, MRU_LST )         /* Copy MRU[0] into IDL_SYS */
  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The origination mode is emergency (i.e., user try to originate an
**    emergency call).
** 2. Emergency acquisition list is empty.
** 3. GW only target should skip this script.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_emerg_list_empty[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )             /* User changes the SS-Preference */
  IS_ORIG_MODE( EMERG_ORIG )           /* Trying to originate an emergency call */
  IS_CNT( EMERG_LST, 0, EXACTLY, ALL ) /* Emergency list is empty */
  NOT IS( CAP_GW_PREF_GW_ONLY )        /*  1x only target.*/


  /* Script */
                                  /* Build emergency acquisition list */
  CPY( EMERG_LST, ACQED_LST )     /* Copy ACQED list into EMERG list */
  APP( EMERG_LST, MRU_LST )       /* Append MRU list to EMERG list */
  APP( EMERG_LST, PRL_LST )       /* Append PRL list to EMERG list */
  APP( EMERG_LST, ALL_PREF_LST )  /* Append ALL_PREF list to EMERG list */
  //APP( EMERG_LST, GW_LST )        /* Append ALL_PREF list to EMERG list */
  APP( EMERG_LST, GW_LTD_LST )    /* Append the LTD_GW system to EMERG list*/
  APP( EMERG_LST, AMPS_A_SYS )    /* Append AMPS_A system to EMERG list */
  APP( EMERG_LST, AMPS_B_SYS )    /* Append AMPS_B system to EMERG list */

                                  /* Schedule candidate or idle sys reacqs */
  //SCHED( CAND_SYS, REACQ_EMERG_LOST, ANY ) /* Sched candidate sys reacq */
  CPY( SRCH_LST, CAND_SYS )        /* Copy CAND_SYS into the search list */
  APP( SRCH_LST, IDL_SYS )         /* Append IDL_SYS to search list */
  CPY( TEMP_SYS, SRCH_LST )        /* Copy SRCH_SYS into TEMP_SYS */
  #ifdef FEATURE_UNIFORM_SCAN_OOS
  SCHED_COND2( TEMP_SYS, REACQ_EMERG_LOST, ANY, NOT_ON_SAME_SCHED )
                                   /* Schedule candidate sys reacquisitions,
                                   ** if not already on EMERG_LOST 
                                   ** schedule */
  #else
  SCHED_COND( TEMP_SYS, REACQ_EMERG_LOST, ANY )
                                   /* Schedule candidate sys reacquisitions*/
  #endif

  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( MINI )                /* Do mini acquisitions */

  //CPY( EMERG_SYS, SCHED_LST )     /* Copy SCHED_LST[0] into EMERG_SYS */

  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                     Optimized Silent Redial Scripts
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is starting call origination.
** 2. The silent redial list is not empty.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_orig_start[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( ORIG_START )      /* SS-Pref is changed to start an orig */
  IS_CNT( REDIAL_LST, 1, OR_MORE, ALL )  /* Redial list is not empty */

  /* Script */
  CLR( REDIAL_LST )               /* Clear the redial list */
  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preferenceis is optimized slinet redial.
** 2. The silent redial list is empty (i.e., optimized silent redial did
**    not start yet).
** 3. The idle system is a PRL listed system.
** 4. Origination mode is not none (i.e. user trying to originate a call).
** 5. SRDA is enabled
*/
static const  sdssscr_elem_u_type
                               ssscr_user_ss_pref_redial_start_prl_srda[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )      /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )   /* User requests optimized silent redial */
  IS_CNT( REDIAL_LST, 0, EXACTLY, ALL )  /* Redial list is empty */
  IS_PRL( IDL_SYS, LISTED )     /* Idle system is a PRL listed system */
  IS_ORIG_MODE( ANY )           /* User trying to originate a call */
  IS( SRDA )                     /* Is SRDA enabled */

  /* Script */
  RESEL_STT( NONE )             /* Set reselection state to NONE */
  REDIR_OFF                     /* Set redirection indication to off */
  HO_OFF                        /* Set handoff indication to off */

  GEO_EXT( REDIAL_LST, IDL_SYS, SAME_BETTER )
                                /* Extract same/better sys into redial list*/
  GEO_EXT( TEMP_LST, IDL_SYS, HOME )
                                /* Extract home systems into temp list*/
  APP( REDIAL_LST, TEMP_LST )   /* Append home systems to redial list */
  ORDER( REDIAL_LST, MRU_LST )  /* Order the systems according to MRU list */
  SORT( REDIAL_LST, AMPS )      /* Bubble AMPS system to the top */
  DEL( REDIAL_LST, IDL_SYS )    /* Delete the idle sys from redial list */
  APP( REDIAL_LST, IDL_SYS )    /* Append the idle sys to redial list */
  NEW_ACQ                       /* Start a new acquisition */
  CLR( ACQ_FULL2_SYS )          /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
  PROC_EVT( USER_SS_PREF )      /* Do SS-Pref event processing */
  PREV_SCR                      /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial.
** 2. The silent redial list is empty (i.e., optimized silent redial did
**    not start yet).
** 3. The idle system is a PRL listed system.
** 4. Origination mode is not none (i.e. user trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_redial_start_prl[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )      /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )   /* User requests optimized silent redial */
  IS_CNT( REDIAL_LST, 0, EXACTLY, ALL )  /* Redial list is empty */
  IS_PRL( IDL_SYS, LISTED )     /* Idle system is a PRL listed system */
  IS_ORIG_MODE( ANY )           /* User trying to originate a call */

  /* Script */
  RESEL_STT( NONE )             /* Set reselection state to NONE */
  REDIR_OFF                     /* Set redirection indication to off */
  HO_OFF                        /* Set handoff indication to off */

  GEO_EXT( REDIAL_LST, IDL_SYS, SAME_BETTER )
                                /* Extract same/better sys into redial list*/
  GEO_EXT( TEMP_LST, IDL_SYS, HOME )
                                /* Extract home systems into temp list*/
  APP( REDIAL_LST, TEMP_LST )   /* Append home systems to redial list */
  ORDER( REDIAL_LST, MRU_LST )  /* Order the systems according to MRU list */
  DEL( REDIAL_LST, IDL_SYS )    /* Delete the idle sys from redial list */
  APP( REDIAL_LST, IDL_SYS )    /* Append the idle sys to redial list */
  NEW_ACQ                       /* Start a new acquisition */
  CLR( ACQ_FULL2_SYS )          /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
  PROC_EVT( USER_SS_PREF )      /* Do SS-Pref event processing */
  PREV_SCR                      /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preferenceis is optimized slinet redial.
** 2. The silent redial list is empty (i.e., optimized silent redial did
**    not start yet).
** 3. Origination mode is not none (i.e. user trying to originate a call).
** 4. SRDA is enabled
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_redial_start_srda[]={

  /* Condition */
  IS_EVENT( USER_SS_PREF )      /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )   /* User requests optimized silnet redial */
  IS_CNT( REDIAL_LST, 0, EXACTLY, ALL )  /* Redial list is empty */
  IS_ORIG_MODE( ANY )           /* User trying to originate a call */
  IS( SRDA )                     /* Is SRDA enabled */

  /* Script */
  RESEL_STT( NONE )             /* Set reselection state to NONE */
  REDIR_OFF                     /* Set redirection indication to off */
  HO_OFF                        /* Set handoff indication to off */

  CPY( REDIAL_LST, PRL_LST )    /* Copy PRL list into redial list */
  ORDER(REDIAL_LST, MRU_LST)    /* Order the systems according to MRU list */
  SORT(REDIAL_LST, AMPS)        /* Bubble AMPS systems to the top */
  DEL( REDIAL_LST, IDL_SYS )    /* Delete the idle sys from redial list */
  APP( REDIAL_LST, IDL_SYS )    /* Append the idle sys to redial list */
  NEW_ACQ                       /* Start a new acquisition */
  CLR( ACQ_FULL2_SYS )          /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
  PROC_EVT( USER_SS_PREF )      /* Do SS-Pref event processing */
  PREV_SCR                      /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial.
** 2. The silent redial list is empty (i.e., optimized silent redial did
**    not start yet).
** 3. Origination mode is not none (i.e. user trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_redial_start[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )      /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )   /* User requests optimized silent redial */
  IS_CNT( REDIAL_LST, 0, EXACTLY, ALL )  /* Redial list is empty */
  IS_ORIG_MODE( ANY )           /* User trying to originate a call */

  /* Script */
  RESEL_STT( NONE )             /* Set reselection state to NONE */
  REDIR_OFF                     /* Set redirection indication to off */
  HO_OFF                        /* Set handoff indication to off */

  CPY( REDIAL_LST, PRL_LST )    /* Copy PRL list into redial list */
  ORDER(REDIAL_LST, MRU_LST)    /* Order the systems according to MRU list */
  DEL( REDIAL_LST, IDL_SYS )    /* Delete the idle sys from redial list */
  APP( REDIAL_LST, IDL_SYS )    /* Append the idle sys to redial list */
  NEW_ACQ                       /* Start a new acquisition */
  CLR( ACQ_FULL2_SYS )          /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
  PROC_EVT( USER_SS_PREF )      /* Do SS-Pref event processing */
  PREV_SCR                      /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial.
** 2. The silent redial list is not empty.
** 3. The idle system is a PRL listed system.
** 4. Origination mode is not none (i.e. user trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_redial_prl[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )     /* User requests optimized silent redial */
  IS_CNT( REDIAL_LST, 1, OR_MORE, ALL )  /* Redial list is not empty */
  IS_PRL( IDL_SYS, LISTED )       /* Idle system is a PRL listed system */
  IS_ORIG_MODE( ANY )             /* User trying to originate a call */

  /* Script */
  LOOP_FOREVER                    /* Loop for ever and: */
    ACT_SCR( CLR_ACQ_SCHED_LSTS ) /*   Clear acq process and sched lists */
    ACQ_MODE( FULL2 )             /*   Do full acquisitions */

    ACQ_CNT( REDIAL_LST, NOT_LESS_PREF_OR_HOME, SDSSSCR_FULL2_SYS, CNT_ALL )
                                  /*   Try all applicable systems.      */
    CPY( TEMP_LST, TBL_LST )      
    DEL_SYS( TEMP_LST, NON_CDMA ) /* Remove NON_CDMA from the TBL_LST */

    AVOID_CANCEL_COND( TEMP_LST, UNTIL_ACQ_FAIL )
                                  /*   Remove avoids due to MAP */
    #ifdef FEATURE_JCDMA
    AVOID_CANCEL_COND( TEMP_LST, UNTIL_ACQ_FAIL_DUR_CALL_ORIG )
                                  /*   Remove avoids due to 100% PSIS */
    #endif

    ACQ( REDIAL_LST, NOT_LESS_PREF_OR_HOME )
                                  /* Try sys that were avoided due to MAP */
    ACQ_ALL( AVAIL_LST, ALLOWED ) /*   Try reacquiring available systems */
    NEW_ACQ                       /*   Start a new acquisition */
    CLR( ACQ_FULL2_SYS )          /*   Clear last FULL2 acquisition system */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
  LOOP                            /* Loop back */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial.
** 2. The silent redial list is not empty.
** 3. Origination mode is not none (i.e. user trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_redial[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )     /* User requests optimized silent redial */
  IS_CNT( REDIAL_LST, 1, OR_MORE, ALL )  /* Redial list is not empty */
  IS_ORIG_MODE( ANY )             /* User trying to originate a call */

  /* Script */
  LOOP_FOREVER                    /* Loop for ever and: */
    CLR( SCHED_LST )              /*   Clear all existing reacq schedules */
    ACQ_MODE( FULL2 )             /*   Do full acquisitions */

    ACQ_CNT( REDIAL_LST, ALLOWED, SDSSSCR_FULL2_SYS, CNT_ALL )

    CPY( TEMP_LST, TBL_LST )      
    DEL_SYS( TEMP_LST, NON_CDMA ) /* Remove NON_CDMA from the TBL_LST */

    AVOID_CANCEL_COND( TEMP_LST, UNTIL_ACQ_FAIL )
                                  /*   Remove avoids due to MAP */
    #ifdef FEATURE_JCDMA
    AVOID_CANCEL_COND( TEMP_LST, UNTIL_ACQ_FAIL_DUR_CALL_ORIG )
                                  /*   Remove avoids due to 100% PSIS */
    #endif

    ACQ( REDIAL_LST, NOT_LESS_PREF_OR_HOME )
                                  /* Try sys that were avoided due to MAP */
    NEW_ACQ                       /*   Start a new acquisition */
    CLR( ACQ_FULL2_SYS )          /*   Clear last FULL2 acquisition system */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
  LOOP                            /* Loop back */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when the user changes the SS-Preference and:
//**
//** 1. The idle system is not empty.
//** 2. The idle system is not on the MRU (i.e., we do not want to stay on the
//**    idle system).
//** 3. The origination mode is not emergency.
//*/
//static const  sdssscr_elem_u_type   ssscr_user_ss_pref_idl_not_mru[] = {
//
//  /* Condition */
//  IS_EVENT( USER_SS_PREF )            /* User changes the SS-Preference */
//  IS_CNT( IDL_SYS, 1, OR_MORE, ALL )  /* Idle system is not empty */
//  IS_CONTAIN( MRU_LST, NONE, IDL_SYS )/* Idle system is not on MRU list */
//  IS_ORIG_MODE( NON_EMERG )           /* Not originating an emergency call */
//
//  /* Script */
//  CLR( IDL_SYS )                      /* Clear the idle system */
//  //PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
//  ACT_SCR( SRV_LOST_NORM )            /* Do normal service lost processing */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The user wants to originate an OTASP call.
** 2. The state is operation.
** 3  Idle system is not empty.
** 4. The idle system is one of the systems in the OTASP activation block.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_otasp_sys_ok[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_ORIG_MODE( OTASP )           /* Trying to originate an OTASP call */
  IS_STATE( OPR )                 /* State is operation */
  IS_CNT( IDL_SYS, 1, OR_MORE, ALL )    /* Idle system is not empty */
  IS_CONTAIN( OTASP_LST, ALL, IDL_SYS ) /* Idle sys is on OTASP list */

  /* Script */
  CONT( 0 )                       /* Continue with CDMA operation */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The user wants to originate an OTASP call.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_otasp[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_ORIG_MODE( OTASP )           /* Trying to originate an OTASP call */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  //CLR( SCHED_LST )                /* Cancel all reacquisition schedules */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The user wants to originate a call on a collocated system.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_colloc[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */

                                  /* Try full acq over collocated list */
  ACQ_MODE( FULL )                /*   Set acquisition mode to full */
  NEW_ACQ                         /*   Start a new acquisition */
  ACQ( SRV_COLLOC_LST, COLLOC_PREF )
                                  /*   Try acquiring a collocated sys */

                                  /* Acquisition failed */
  ACQ_FAIL                        /*   Indicate that acquisition failed */
  SRV_OFF_UNTIL( OH )             /*   Set srv OFF until OH info received */
  ACT_SCR( SRV_LOST_NORM )        /*   Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system conflicts with the new SS-Preference.
** 3. The SS-Preference is not changed to start a call origination mode.
** 4. The SS-Preference is not changed to end a call origination mode.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_conf[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_PREF_CONF( SRV_SYS, ANY )    /* Srv sys conflicts with new SS-Pref */

  NOT IS_PREF_REAS( ORIG_START )  /* SS-Pref is not changed to start orig */
  NOT IS_PREF_REAS( ORIG_END )    /* SS-Pref is not changed to end orig */

  //IS_ORIG_MODE( NONE )            /* Not starting a call origination mode*/
  //IS_NOT( ORIG_MODE_END )         /* Not ending a call origination mode */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  //CLR( EMERG_SYS )                /* Clear the EMERG_SYS */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system is a CDMA system.
** 3. MS is CDMA locked until power cycle.
** 4. MS is AMPS capable.
** 5. Origination mode is not none (i.e. user trying to originate a call).
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_cdma_lock[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE( SRV_SYS, CDMA )        /* Srv sys is a CDMA system */
  IS( CDMA_LOCKED )               /* MS is CDMA locked */
  IS( CAP_AMPS )                  /* MS is AMPS capable */
  IS_ORIG_MODE( ANY )             /* User trying to originate a call */

  /* Script */
  PROC_EVT( CDMA_OPR_LOCK )       /* Do CDMA lock event processing */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system is a CDMA system.
** 3. The pref reason is orig end.
** 4. Origination mode is NONE.
** 5. Reselecttion state is WAIT 
**
** This script makes sure that user_ss_pref with reason ORIG_END doesnt affect 
** any reselection timers that were already set.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_cdma_orig_end[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )            /* User changes the SS-Preference */
  IS_STATE( OPR )                     /* State is operation */
  IS_MODE( SRV_SYS, CDMA )            /* Srv sys is a CDMA system */
  IS_PREF_REAS( ORIG_END )            /* Pref reason is orig end. */
  IS_ORIG_MODE( NONE )                /* Not trying to originate a call */
  NOT IS_PREF_CONF( SRV_SYS, MODE )   /* Acquired system conflicts mode pref */
  IS_RESEL_STT( WAIT )                /* MS is doing reselection */

  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system is a CDMA system.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_cdma[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE( SRV_SYS, CDMA )        /* Srv sys is a CDMA system */
  //IS_NOT( CDMA_LOCKED )           /* MS is not CDMA locked */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  PROC_EVT( CDMA_OPR_OH_SID )     /* Do new CDMA SID/NID event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system is an AMPS system.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_amps[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE( SRV_SYS, AMPS )        /* Srv sys is an AMPS system */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  PROC_EVT( AMPS_OPR_PCH_SID )    /* Do AMPS PCH SID event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system is an HDR system
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_hdr[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE( SRV_SYS, HDR  )        /* Srv sys is an HDR system */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  PROC_EVT( HDR_OPR_OH_SID )      /* Do new HDR SID event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};


#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GW.
** 3. Starting or ending emergency call.
** 4. Domain or acq order preference didn't change.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_emerg[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, GW  )    /* Srv sys is a GW system */
  IS_ORIG_MODE( EMERG_ORIG_OR_NONE_OR_MAN_IDLE ) 
                                  /* Emergency orig or end of any call */
  IS_PREF_REAS( ORIG_START_OR_END ) 
                                  /* Orig or end of any call */

  NOT IS_PREF_CHGD( DOMAIN_ACQ_ORDER )

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  PROC_EVT( GW_OPR_SRV_INFO )     /* Do GW srv info event processing */
  PROC_EVT( GW_OPR_SYS_LOST )     /* Do system lost processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GW in limited service.
** 3. Starting a normal call.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_norm_call_orig_ltd[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, GW  )    /* Srv sys is a GW system */
  IS_ORIG_MODE( NORMAL )          /* Normal call */
  IS_SRV_STATUS( LIMITED )        /* Currently, have limited service */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  //ACQ_ALL(GW_LAST_FULL_SYS, ALLOWED)
  //ACQ_FAIL
  //PROC_EVT( GW_OPR_SYS_LOST )   /* Do system lost processing */
  ACT_SCR( SRV_LOST_NORM_SLNT )   /* Do service lost normal silent */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GW in limited service.
** 3. Starting a normal call in manual mode.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_man_call_orig_ltd[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, GW  )    /* Srv sys is a GW system */
  IS_ORIG_MODE( MANUAL_ORIG)      /* In manual mode */
  IS_SRV_STATUS( LIMITED )        /* In limited service */
  NOT IS( FEATURE_UMTS_UNIFORM_OOS_HANDLING )

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  ACQ_ALL(GW_LAST_USER_SYS, ALLOWED)
  ACQ_FAIL
  PROC_EVT( GW_OPR_SYS_LOST )     /* Do system lost processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GW in full service.
** 3. Starting a ps call.
** 4. Current domain_pref is CS_PS but we don't have both services.
** 5. Domain or acq order preference didn't change.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_start_csps[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, GW  )    /* Srv sys is a GW system */
  IS_PREF_REAS( ORIG_START_PS )   /* Originating  a ps call */
  IS ( SRV_DOMAIN_PREF_CS_PS )    /* Current pref. is cs+ps */
  IS_SRV_STATUS( SRV )            /* Having full service. */
  NOT IS_SRV_DOMAIN( PS )         /* We don't have PS services */
  IS_PREF_CHGD( NONE )            /* Preferences hasn't changed */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  ACQ_ALL(IDL_SYS, ALLOWED)       /* Kick stack to acquire both service */
  PROC_EVT( GW_OPR_SYS_LOST )     /* Do system lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GW in full service with non CS domain.
** 3. Start a cs call.
** 4. No preferences changed.
** 5. Perform system lost operation
*/

static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_start_cs[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, GW  )    /* Srv sys is a GW system */
  IS_PREF_REAS( ORIG_START_CS )   /* Originating  a cs call */
  IS_SRV_STATUS( SRV )            /* Having full service. */
  NOT IS_SRV_DOMAIN( CS )         /* We don't have CS services */
  IS_PREF_CHGD( NONE )            /* Preferences hasn't changed */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  ACQ_ALL(IDL_SYS, ALLOWED)       /* Kick stack to acquire both service */
  PROC_EVT( GW_OPR_SYS_LOST )     /* Do system lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GW.
** 3. Pref reason is voice call origination.
** 4. Acquired system has no mode conflict.
** 5. The current service domain is compliant
** 6. The GW Band preferences have not changed
**
** This script makes sure that user_ss_pref with reason ORIG_START_CS 
** doesn't initiate GW system scan when current system matches the call 
** mode requirements.
**
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_cs_no_conf[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, GW  )    /* Srv sys is a GW system */
  IS_PREF_REAS( ORIG_START_CS )   /* Originating  a cs call */
  NOT IS_PREF_CONF( SRV_SYS, MODE )   /* Acquired system conflicts mode pref */
  IS_SRV_DOMAIN( CS )             /* We have CS services */
  NOT IS_BAND_PREF_CHGD( GW )     /* GW band preference didn't change */
  /* Script */
  PREV_SCR
  END  
};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GW.
** 3. Pref reason is data call origination.
** 4. Acquired system has no mode conflict.
** 5. The current service domain is compliant
** 6. The Band preferences have not changed
**
** This script makes sure that user_ss_pref with reason ORIG_START_PS 
** doesn't initiate GW system scan when current system matches the call 
** mode requirements.
**
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_ps_no_conf[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, GW  )    /* Srv sys is a GW system */
  IS_PREF_REAS( ORIG_START_PS )   /* Originating  a ps call */
  NOT IS_PREF_CONF( SRV_SYS, MODE )   /* Acquired system conflicts mode pref */
  IS_SRV_DOMAIN( PS )             /* We have PS services */
  NOT IS_BAND_PREF_CHGD( GW )     /* GW band preference didn't change */

  /* Script */
  PREV_SCR
  END  
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the resuming dormant PS call and:
** 1. The state is operation.
** 2. Current system is GW.
** 3. Trying to resume dormant PS call.
** 4. We have PS service (full).
** 5. GW Band preference is not changed.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_ps_resume_full_srv [] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, GW  )    /* Srv sys is a GW system */
  IS_PREF_REAS( ORIG_RESUME )     /* PS call reorignation in dormant */
  IS_SRV_STATUS( SRV)             /* Having service. */
  IS_SRV_DOMAIN( PS )             /* We have PS services */
  NOT IS_BAND_PREF_CHGD( GW )     /* GW band preference didn't change */

  /* Script */
  PREV_SCR
  END  
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the resuming dormant PS call and:
** 1. The state is operation.
** 2. Current system is GW.
** 3. We don't have PS service.
** 4. Trying to resume dormant PS call.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_ps_resume_no_ps_srv [] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )          /* User changes the SS-Preference */
  IS_STATE( OPR )                   /* State is operation */
  IS_MODE_PREF( SRV_SYS, GW )       /* Srv sys is a GW system */
  IS_SRV_STATUS( SRV)               /* Having service. */
  NOT IS_SRV_DOMAIN( PS )           /* We don't have PS services */
  IS_PREF_REAS( ORIG_RESUME )       /* PS call reorignation in dormant*/

  /* Script */
  ACT_SCR( SRV_LOST_NORM_SLNT )     /* Do normal service lost silent processing */
  END
};

/* Script for when the resuming dormant PS call and:
** 1. The state is operation.
** 2. Current system is GW in limited service.
** 3. Trying to resume dormant PS call.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_ps_resume_ltd_srv [] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )                /* User changes the SS-Preference */
  IS_STATE( OPR )                         /* State is operation */
  IS_MODE_PREF( SRV_SYS, GW )             /* Srv sys is a GW system */
  IS_SRV_STATUS( LIMITED_ANY )            /* Currently, have limited service */
  IS_PREF_REAS( ORIG_RESUME )             /* PS call reorignation in dormant*/

  /* Script */
  ACT_SCR( SRV_LOST_NORM_SLNT )     /* Do normal service lost silent processing */
  END
};

/* Script for when the resuming dormant PS call and:
** 1. UE is in no service or power save mode.
** 2. Current system is GW.
** 3. Service status is NO_SRV or PWR_SAVE.
** 4. Trying to resume dormant PS call.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_ps_resume_no_srv_any [] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )          /* User changes the SS-Preference */
  IS_MODE_PREF( SRV_SYS, GW )       /* Srv sys is a GW system */
  IS_SRV_STATUS( NO_SRV_ANY )       /* Service status is NO_SRV or PWR_SAVE */
  IS_PREF_REAS( ORIG_RESUME )       /* PS call reorignation in dormant*/

  /* Script */
  ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Pref reason is not user wants to resel.
** 3. Current system is GW.
** 3. preference hasn't changed.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, GW  )    /* Srv sys is a GW system */
  NOT IS_PREF_REAS( USER_RESEL )
  NOT IS_PREF_REAS( USER )        /* User did not change network selection
                                  ** mode. */
  IS_PREF_CHGD( NONE )            /* preference didn't change */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  PROC_EVT( GW_OPR_SRV_INFO )     /* Do GW srv info event processing */
  PROC_EVT( GW_OPR_SYS_LOST )     /* Do system lost processing */
  END
};

#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. System is GW only system.
** 3. Schedule list is empty.
** 4. User has changed Domain Pref. and the phone is not in MANUAL mode.
*/
//static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_domain_auto[] = {

//  /* Condition */
//  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
//  IS_STATE( OPR )                 /* State is operation */
//  IS_CNT( SCHED_LST, 0, EXACTLY, ALL ) /* Scheduled list is empty */
//  IS( CAP_GW_PREF_GW_ONLY )              /* Capable for GSM/WCDMA but not 1x */
//  IS_PREF_CHGD(DOMAIN)
//  NOT IS_ORIG_MODE(MANUAL)

  /* Script */
//  NEW_ACQ
//ACQ(GW_LAST_FULL_SYS, ALLOWED)
//ACT_SCR( GW_SCHED )             /* Do scheduling for gw system */
//PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */

//  PREV_SCR                        /* Continue with previous script */
//  END
//};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. System is GW only system.
** 3. Schedule list is empty.
** 4. User has changed Domain Pref. and the phone is in MANUAL mode.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_domain[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_SRV_STATUS( SRV )            /* Having full service. */
  //IS_CNT( SCHED_LST, 0, EXACTLY, ALL ) /* Scheduled list is empty */
  IS_MODE_PREF( SRV_SYS, GW  )    /* Srv sys is a GW system */
  IS_PREF_CHGD(DOMAIN)
  //IS_ORIG_MODE(MANUAL)

  /* Script */
  // NEW_ACQ
  // ACQ(GW_LAST_USER_SYS, ALLOWED)
  ACT_SCR( SRV_LOST_NORM_SLNT )    /* Do normal service lost silient processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when the user changes the SS-Preference and:
//** 1. The state is operation
//** 2. System is GW only system.
//** 3. Schedule list is empty.
//*/
//static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_sched[] = {

//  /* Condition */
//  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
//  IS_STATE( OPR )                 /* State is operation */
//  IS_CNT( SCHED_LST, 0, EXACTLY, ALL ) /* Scheduled list is empty */
//  IS( CAP_GW_PREF_GW_ONLY )              /* Capable for GSM/WCDMA but not 1x */

  /* Script */
//  ACT_SCR( GW_SCHED )             /* Do scheduling for gw system */
//  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
//  PREV_SCR                        /* Continue with previous script */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 2. Pseudo online mode is requested
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_pseudo_online[] = {

  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS ( PSEUDO_ONLINE )  /* Pseudo online */

  /* Script */
  ACT_SCR( SRV_LOST_NORM ) /* Activate the service lost script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 2. Redirection indication is ON (i.e., SD works on a redirection).
** 3. Origination mode is emergency origination.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_redir_emerg[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_REDIR( ANY )                 /* Redirection indication is ON */
  IS_ORIG_MODE( EMERG_ORIG )      /* Trying to originate an emergency call */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */

                                  /* Prepend CDMA redir sys to emerg list */
  CPY( SRCH_LST, REDIR_LST )      /* Copy redirection list into SRCH list */
  APP( SRCH_LST, EMERG_LST )      /* Append emergency list to SRCH_LST */
  CPY( EMERG_LST, SRCH_LST )      /* Copy SRCH list into emergency list */

  ORDER( SCHED_LST, REDIR_LST )   /* Order SCHED_LST according to REDIR */
  RESET_GW_UOOS_ACQ_PARA          /* Reset the GW acquisition parameters */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 2. Redirection indication is ON (i.e., SD works on a redirection).
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_redir[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_REDIR( ANY )                 /* Redirection indication is ON */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  ACQ_MODE( FULL )                /* Do FULL acquisitions */
  PREV_SCR                        /* Continue with redirection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 2. MS is currently doing reselection.
** 3. The acquired system is GW limited system.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_acq_resel_lmtd[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_RESEL_STT( SRCH )            /* MS is doing reselection */
  IS_CONTAIN( GW_LTD_LST, SOME, IDL_SYS )
                                  /* The idle system is not limited system. */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  PROC_EVT( GW_OPR_SYS_LOST )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 3. MS is currently doing reselection.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_acq_resel[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_RESEL_STT( SRCH )            /* MS is doing reselection */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  HO_OFF                          /* Set handoff indication to OFF */
  CPY( CAND_SYS, IDL_SYS )        /* Set candidate sys per the IDL_SYS */
  SCHED_COND( IDL_SYS, REACQ_ACQ_LOST, ANY ) /* Schedule reacq for idle sys*/
  ACQ_ALL( IDL_SYS, ANY )         /* Try reacquiring the IDL_SYS */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The system selection state is verification.
** 2. User changes SS-preference in order to originate a call.
** 3. User trying yo originate a call other than OTASP.
** 4. Acquired system does not conflict with SS-Pref.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_vrfy_orig[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( VRFY )                /* The SS-State is VERIFY */
  IS_ORIG_MODE( ANY )             /* Trying to originate a call */
  NOT IS_ORIG_MODE( OTASP )       /* Orig a call other than OTASP */
  IS_PREF_CONF( ACQ_SYS, NONE )   /* ACQ sys does not conflict with SS-Pref*/

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  HO_OFF                          /* Set handoff indication to OFF */
  CONT( SDSSSCR_UNTIL_NEXT_EVENT )/* Wait until next event */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 2. User changes SS-preference in order to originate a normal call.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_acq_orig_normal[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_ORIG_MODE( NORMAL )          /* Trying to originate a normal call */
  NOT IS( CAP_GW_PREF_GW_ONLY )          /* Not GW only 1x */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  HO_OFF                          /* Set handoff indication to OFF */
  ACQ_MODE( FULL )                /* Do full acquisitions */

  #ifdef FEATURE_UNIFORM_SCAN_OOS
  NEW_ACQ                         /* If not doing Slient redials, need to 
                                  ** start a NEW ACW */ 
  PROC_EVT( INT_SRV_LOST )        /* Process the internal service lost event */
  #else

  ACQ_ALL( CAND_SYS, ALLOWED )    /* Try reacquiring the candidate system */
  ACQ_ALL( SI_SYS, ALLOWED )      /* Try reacquiring the SI system */

  ORDER( AVAIL_LST, PRL_LST )     /* Order avail systems according to PRL */
  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Try reacquiring available systems */
  ACQ( START_PREF_LST, ALLOWED )  /* Try acq from GEO we just worked on */
  ACQ_MODE( MINI )                /* Do mini acquisitions */
  ACQ_ALL( MRU_LST, ALLOWED )     /* Try reacquiring MRU systems */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */

  #endif
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 2. User changes SS-preference in order to originate an emergency call.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_acq_orig_emerg[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_ORIG_MODE( EMERG_ORIG )      /* Trying to originate emergency call */
  NOT IS( CAP_GW_PREF_GW_ONLY )   /* Not GW only 1x */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  HO_OFF                          /* Set handoff indication to OFF */

  CPY( TEMP_SYS, MRU_LST )                     /* Copy MRU_LST[0] to TEMP_SYS*/
  SCHED_COND( CAND_SYS, REACQ_EMERG_LOST, ANY )/* Sched CNAD_SYS for reacq */
  SCHED_COND( TEMP_SYS, REACQ_1_TIME, ANY )    /* Schedule MRU[0] for reacq */
  SCHED_COND( IDL_SYS, REACQ_1_TIME, ANY )     /* Schedule IDL_SYS for reacq */
  SCHED_COND( SI_SYS, REACQ_1_TIME, ANY )      /* Schedule SI_SYS for reacq */
  RESET_GW_UOOS_ACQ_PARA           /* Reset the GW acquisition parameters */
  ACT_SCR( SRV_LOST_NORM )         /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 #if ( defined (FEATURE_UMTS_UNIFORM_OOS_HANDLING) || \
       defined (FEATURE_UNIFORM_SCAN_OOS) )

static const  sdssscr_elem_u_type   ssscr_user_ss_pref_orig_end_no_pref_change_acq[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( ORIG_END )        /* Call origination is ended      */
  IS_SRV_STATUS( NO_SRV_ANY )     /* Service status is no service or pwr_save */
  NOT IS_PREF_UPDATE( EMERG_ORIG_END ) /* Emerg call is not ended */
  IS( FEATURE_UOOS )              /* Not doing silent re-dials */
  IS_STATE( ACQ )                 /* In UOOS scan for service */
  IS_PREF_CHGD( NONE )            /* preference didn't change */

  /* Script */
  PREV_SCR                        /* Continue with the previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

static const  sdssscr_elem_u_type   ssscr_user_ss_pref_orig_end_no_pref_change_pwr_save[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( ORIG_END )        /* Call origination is ended      */
  IS_SRV_STATUS( NO_SRV_ANY )     /* Service status is no service or pwr_save */
  NOT IS_PREF_UPDATE( EMERG_ORIG_END ) /* Emerg call is not ended */
  IS( FEATURE_UOOS )              /* Not doing silent re-dials */
  IS_STATE( PWR_SAVE)             /* In Deep sleep */
  IS_PREF_CHGD( NONE )            /* preference didn't change */

  /* Script */
  PREV_SCR                        /* Continue with the previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. SS-pref changes
** 2. The reason is call orgination end
** 3. Service status is no service or power save
** 4. Not doing silent redials
** 5. Emergnecy call origination is not ended
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_orig_end_no_srv[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( ORIG_END )        /* Call origination is ended      */
  IS_SRV_STATUS( NO_SRV_ANY )     /* Service status is no service or pwr_save */
  NOT IS_PREF_UPDATE( EMERG_ORIG_END ) /* Emerg call is not ended */
  IS( FEATURE_UOOS ) /* Not doing silent re-dials */
  IS_STATE( PWR_SAVE)

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#endif

/* Script for when user changes the SS-Preference and:
**
** 1. SS-pref changes
** 2. The reason is call orgination end
** 3. Doing silent redials
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_orig_end[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( ORIG_END )        /* Call origination is ended      */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_HICPS_STACK_SYNC_UP
#error code not present
#endif /* FEATURE_HICPS_STACK_SYNC_UP */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is acquisition (no need to check).
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_NAM_SEL
---------------------------------------------------------------------------*/

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
/* Script for when subscription is changed, phone is in operation and the
** orig_mode is none.
*/
static const  sdssscr_elem_u_type   ssscr_user_nam_sel_gw_opr[] = {

  /* Condition */
  IS_EVENT( USER_NAM_SEL )        /* User changes the NAM selection */
  IS( CAP_GW_PREF_GW_ONLY )              /* GW only capable targets no 1x. */
  IS_STATE( OPR )
  IS_ORIG_MODE( NONE )            /* not in origination */

  RESEL_STT( NONE )               /* Set reselection state to NONE */
  //ACT_SCR( GW_SCHED )             /* Do gw scheduling */
  ACT_SCR( SRV_LOST_NORM_SLNT)     /*Do normal service lost silent */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when subscription is changed.
*/
static const  sdssscr_elem_u_type   ssscr_user_nam_sel_gw[] = {

  /* Condition */
  IS_EVENT( USER_NAM_SEL )        /* User changes the NAM selection */
  IS( CAP_GW_PREF_GW_ONLY )              /* GW only capable targets no 1x.
                                  ** It need to do the scheduling.*/

  RESEL_STT( NONE )               /* Set reselection state to NONE */
  ACT_SCR( SRV_LOST_NORM_SLNT)     /*Do normal service lost silent */
  END
};

#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the NAM selection and the orig_mode
** is set to MANUAL.
*/
static const  sdssscr_elem_u_type   ssscr_user_nam_sel_manual[] = {

  /* Condition */
  IS_EVENT( USER_NAM_SEL )        /* User changes the NAM selection */
  IS_ORIG_MODE( MANUAL )          /* Orig mode is Manual? */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  //SRV_STATUS( NO_SRV )
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */

  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( ACQ_GEO_LST )              /* Clear the acquired system's GEO list */
  CLR( IDL_GEO_LST )              /* Clear the idle system's GEO list */
  CLR( SRV_GEO_LST )              /* Clear the serving system's GEO list */
  CLR( ACQ_PREF_LST )             /* Clear the acq sys's preferred list */
  CLR( IDL_PREF_LST )             /* Clear the idle sys's preferred list */
  CLR( SRV_PREF_LST )             /* Clear the srv sys's preferred list */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */

  ACT_SCR( SRV_LOST_SKIP_SLNT )   /* do srv lost skip slient proc */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the NAM selection.
*/
static const  sdssscr_elem_u_type   ssscr_user_nam_sel[] = {

  /* Condition */
  IS_EVENT( USER_NAM_SEL )        /* User changes the NAM selection */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  SRV_STATUS( NO_SRV )
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */

  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( ACQ_GEO_LST )              /* Clear the acquired system's GEO list */
  CLR( IDL_GEO_LST )              /* Clear the idle system's GEO list */
  CLR( SRV_GEO_LST )              /* Clear the serving system's GEO list */
  CLR( ACQ_PREF_LST )             /* Clear the acq sys's preferred list */
  CLR( IDL_PREF_LST )             /* Clear the idle sys's preferred list */
  CLR( SRV_PREF_LST )             /* Clear the srv sys's preferred list */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */

  CPY( SRCH_LST, MRU_LST )        /* Copy MRU systems into SRCH list */
  APP( SRCH_LST, PRL_LST )        /* Append PRL systems to SRCH list */
  //CPY( TEMP_SYS, ACQED_LST )      /* Copy ACQED_LST[0] to TEMP_SYS */
  //ORDER( SRCH_LST, TEMP_SYS )     /* Bring to top most recently acq system */
  APP( SRCH_LST, GW_LST )         /* Append PRL systems to SRCH list */
  ORDER( SRCH_LST, ACQ_SYS )      /* Bring to top most recently acq system */

  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_CNT( SRCH_LST, BEST_PREF, 0, 1 )
                                  /* Try acq most recently acquired sys */
  SRV_STATUS( NO_SRV )
  //PROC_EVT( USER_PWR_UP )         /* Acq-failed, do power-up event proc */
  ACT_SCR( SRV_LOST_SKIP )        /* Acq failed, do skip-over srv lost proc*/
  END
};

/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_PWR_DOWN
---------------------------------------------------------------------------*/

/* Script for when the user commands phone to power-off, perform a
** reset or enter LPM.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_down[] = {

  /* Condition */
  IS_EVENT( USER_PWR_DOWN )       /* User commands MS to power-down */

  /* Script */
  CONT( 0 )                       /* Continue with power down operation */
  END
};


/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_GET_NET_GW
---------------------------------------------------------------------------*/

/* Script for when the user commands to get the available networks from
** GSM and/or WCDMA technologies
** if condition
** 1. in operation mode
** 2. GW only targets
** 3. Shedule list is empty
*/
//static const  sdssscr_elem_u_type   ssscr_user_get_net_sched[] = {

//  /* Condition */
//  IS_EVENT( USER_GET_NET_GW )     /* User commands to get available networks*/
//  IS_STATE( OPR )                 /* State is operation */
//  IS( CAP_GW_PREF_GW_ONLY )              /* Capable for GSM/WCDMA but not 1x */
//  IS_CNT( SCHED_LST, 0, EXACTLY, ALL )
                                  /* To make sure only schedule once. */

//  /* Script */
//  ACT_SCR( GW_SCHED )             /* Do scheduling */

//  CPY( SRCH_LST, MANUAL_LST )       /* Copy srch list from user list.*/
//  ORDER( SRCH_LST, GW_LAST_USER_SYS )/* Order the list by last user system */
//  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
//  NEW_ACQ                         /* In order to step through the list */
//  GET_NET ( SRCH_LST )            /* Send the request to get the available
//                                  ** networks.*/
//  CONT( 10 )                      /*  Wait for user to make his selection
//                                  ** within 10 seconds. */
//  ACT_SCR( SRV_LOST_NORM )        /* Do service lost processing once response
//                                  ** is received. */
//  END
//};

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)

/* Script for when the user commands to get the available networks from
** GSM and/or WCDMA technologies
*/
static const  sdssscr_elem_u_type   ssscr_user_get_net[] = {

  /* Condition */
  IS_EVENT( USER_GET_NET_GW )     /* User commands to get available networks*/

  /* Script */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy srch list from user list.*/
  ORDER( SRCH_LST, GW_LAST_USER_SYS )/* Order the list by last user system */
  //SRV_STATUS( NO_SRV )          /* Set service status to NO_SRV  */
  NEW_ACQ                         /* In order to step through the list */
  RESET_GW_ACQ_SCAN_STATUS        /* Reset the GW acq scan status */
  GET_NET ( SRCH_LST,GW_LAST_USER_SYS )
                                  /* Send GW network list request.Update
                                  ** GW_LAST_USER_SYS with the system of
                                  ** which the PLMN list report contains
                                  ** at lease one PLMN list.
                                  */
  //CONT( 10 )                      /*  Wait for user to make his selection
  //                                  ** within 10 seconds. */
  SRV_REQ_TYPE( SYNC_UP )         /* After get_networks, SD sends SYNC_UP
                                     request to NAS */  
  ACT_SCR( SRV_LOST_NORM_SLNT )   /* Do service lost processing once response
                                  ** is received. */

  END
};


/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_TERM_GET_NET_GW
---------------------------------------------------------------------------*/

/* Script for when the user commands to terminate the get available networks
** command from GSM and/or WCDMA technologies
*/
static const  sdssscr_elem_u_type   ssscr_user_term_get_net[] = {

  /* Condition */
  IS_EVENT( USER_TERM_GET_NET_GW )/* User commands to terminate the get
                                  ** available networks command.
                                  */

  ACT_SCR( SRV_LOST_NORM_SLNT )  /* Do service lost slient processing
                                  */

  END
};
#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */


/*---------------------------------------------------------------------------
                         SDSS_EVT_HYBR_1_USER_HYBR_START
---------------------------------------------------------------------------*/

/* Script for when user starts CDMA+HDR hybrid mode of operation.
*/
static const sdssscr_elem_u_type  ssscr_hybr_1_user_hybr_start[] = {

  /* Condition */
  IS_EVENT( HYBR_1_USER_HYBR_START )   /* User ended hybrid mode of operation */

  /* Script */
  RESEL_STT( NONE )                 /* Reset reselection state */
  ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
  END
};


/*---------------------------------------------------------------------------
                         SDSS_EVT_HDR_USER_HYBR_END
---------------------------------------------------------------------------*/

/* Script for when user ends CDMA+HDR hybrid mode of operation.
*/
static const sdssscr_elem_u_type  ssscr_hybr_1_user_hybr_end[] = {

  /* Condition */
  IS_EVENT( HYBR_1_USER_HYBR_END )     /* User ended hybrid mode of operation */

  /* Script */
  RESEL_STT( NONE )                 /* Reset reselection state */
  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE     */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  CONT( 0 )                         /* Continue with power-save operation */
  END
};


/*---------------------------------------------------------------------------
                           User Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with user
** events.
*/
static const sdssscr_elem_u_type * const dsssscr_user_event_tbl[] = {

                                      /* 0 */
  ssscr_user_pwr_up_emerg,            /* User commands phone to power-up,
                                      ** in emergency mode.
                                      ** be done.
                                      */

  ssscr_user_pwr_up,                  /* User commands phone to power-up */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_user_offline_cdma,            /* User commands phone to enter offline
                                      ** CDMA mode of operation */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_user_offline_amps,            /* User commands phone to enter offline
                                      ** AMPS mode of operation */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
                                      /* 4 */
  ssscr_user_pwr_save_exit_gw_manual_wakeup_acq,
                                      /* User commands phone to exit power-
                                      ** save mode and to start searching for
                                      ** systems in GW only target and the
                                      ** orig mode is manual mode. The phone
                                      ** is currently doing powersave wake up
                                      ** acquisitions.
                                      */

  ssscr_user_pwr_save_exit_gw_manual, /* User commands phone to exit power-
                                      ** save mode and to start searching for
                                      ** systems in GW only target and the
                                      ** orig mode is manual mode.
                                      */

  ssscr_user_pwr_save_exit_gw_pwrup_wakeup_acq,
                                      /* User commands phone to exit power-
                                      ** save mode and to start searching for
                                      ** systems in GW only target and no
                                      ** system has been acquired since power
                                      ** up. The phone is currently doing
                                      ** powersave wake up acquisitions.
                                      */

  ssscr_user_pwr_save_exit_gw_pwrup,  /* User commands phone to exit power-
                                      ** save mode and to start searching for
                                      ** systems in GW only target and no
                                      ** system has been acquired since power
                                      ** up. */

                                      /* 8 */
  ssscr_user_pwr_save_exit_gw_wakeup_acq,
                                      /* User commands phone to exit power-
                                      ** save mode and to start searching for
                                      ** systems in GW only target. The phone
                                      ** is currently doing powersave wake up
                                      ** acquisitions.
                                      */

  ssscr_user_pwr_save_exit_gw,        /* User commands phone to exit power-
                                      ** save mode and to start searching for
                                      ** systems in GW only target. */

  #else

                                      /* 4 */
  ssscr_reserved_script,

  ssscr_reserved_script,

  ssscr_reserved_script,

  ssscr_reserved_script,

                                      /* 8 */
  ssscr_reserved_script,

  ssscr_reserved_script,

  #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */
  ssscr_user_pwr_save_exit,           /* User commands phone to exit power-
                                      ** save mode and to start searching for
                                      ** systems */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_user_ss_pref_gps_end,         /* User changes SS-Preference to end
                                      ** GPS mode */

                                      /* 12 */
  ssscr_user_ss_pref_avoid_sys,       /* User commands phone to avoid the
                                      ** system
                                      */

  ssscr_user_ss_pref_hdr_cdma_ho,     /* User changes the SS-Preference for 
                                      ** handing off the call to 1X */

  ssscr_user_ss_pref_not_norm,        /* User changes the SS-Preference and
                                      ** origination mode change is not
                                      ** normal (i.e. other than none to
                                      ** normal or normal to none) */

  ssscr_user_ss_pref_emerg_list_full, /* User changes the SS-Preference and
                                      ** origination mode is not emergency and
                                      ** emergency list is full */

                                      /* 16 */
  ssscr_user_ss_pref_emerg_list_empty,/* User changes the SS-Preference in
                                      ** order to originate an emergency
                                      ** call and emergency acquisition list
                                      ** is empty */
  #ifdef FEATURE_UNIFORM_SCAN_OOS
                                      
  ssscr_user_ss_pref_uoos_not_norm,   /* User changes the SS-Preference and
                                      ** origination mode change is not
                                      ** normal (i.e. other than none to
                                      ** normal or normal to none) */
  #else

  ssscr_reserved_script,
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Optimized silent redial Scripts.
  */

  ssscr_user_ss_pref_orig_start,      /* User changes the SS-Preference in
                                      ** order to start an origination and
                                      ** redial list is empty */

  ssscr_user_ss_pref_redial_start_prl_srda,
                                      /* User changes the SS-Preference in
                                      ** order to start optimized silent
                                      ** redial and idle system is from
                                      ** PRL, with SRDA enabled */

                                      /* 20 */
  ssscr_user_ss_pref_redial_start_prl,/* User changes the SS-Preference in
                                      ** order to start optimized silent
                                      ** redial and idle system is from
                                      ** PRL */

  ssscr_user_ss_pref_redial_start_srda,
                                      /* User changes the SS-Preference in
                                      ** order to start optimized silent
                                      ** redial and idle system is not from
                                      ** PRL , with SRDA enabled*/

  ssscr_user_ss_pref_redial_start,    /* User changes the SS-Preference in
                                      ** order to start optimized silent
                                      ** redial and idle system is not from
                                      ** PRL */

  ssscr_user_ss_pref_redial_prl,      /* User changes the SS-Preference in
                                      ** order to do optimized silent redial
                                      ** and idle system is from PRL */

                                      /* 24 */
  ssscr_user_ss_pref_redial,          /* User changes the SS-Preference in
                                      ** order to do optimized silent redial
                                      ** and idle system is not from PRL */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_user_ss_pref_otasp_sys_ok,    /* User changes the SS-Preference to
                                      ** originate an OTASP call and idle
                                      ** system is from OTASP activation
                                      ** block */

  ssscr_user_ss_pref_otasp,           /* User changes the SS-Preference to
                                      ** originate an OTASP call */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_user_ss_pref_colloc,          /* User changes the SS-Preference to
                                      ** originate on a collocated system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                      /* 28 */
  ssscr_user_ss_pref_opr_conf,        /* User changes the SS-Preference while
                                      ** in operation state and serving
                                      ** system conflicts with the new
                                      ** SS-Preference */

  ssscr_user_ss_pref_opr_cdma_lock,   /* User changes the SS-Preference while
                                      ** in operation state and MS is CDMA
                                      ** locked until power-cycle */

  ssscr_user_ss_pref_opr_cdma_orig_end,  
                                      /* User changes the SS-Preference while
                                      ** in operation state and the serving
                                      ** system is a CDMA system that does
                                      ** not conflict with the new
                                      ** SS-Preference */

  ssscr_user_ss_pref_opr_cdma,        /* User changes the SS-Preference while
                                      ** in operation state and the serving
                                      ** system is a CDMA system that does
                                      ** not conflict with the new
                                      ** SS-Preference */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                      /* 32 */
  ssscr_user_ss_pref_opr_amps,        /* User changes the SS-Preference while
                                      ** in operation state and the serving
                                      ** system is an AMPS system that does
                                      ** not conflict with the new
                                      ** SS-Preference */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_user_ss_pref_opr_hdr,         /* User changes the SS-Preference while
                                      ** in operation state and the serving
                                      ** system is an HDR system that does
                                      ** not conflict with the new
                                      ** SS-Preference */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
  ssscr_user_ss_pref_opr_gw_emerg,    /* User changes the SS-Preference while
                                      ** in operation in GW to make emergency
                                      ** call.
                                      */

  ssscr_user_ss_pref_opr_gw_norm_call_orig_ltd,
                                      /* User starts call origination while in
                                      ** GW limited service in auto mode.
                                      */
                                      /* 36 */
  ssscr_user_ss_pref_opr_gw_man_call_orig_ltd,
                                      /* User starts call origination while in
                                      ** GW limited service in auto mode.
                                      */

  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  ssscr_reserved_script,              /* Reserved */

  ssscr_reserved_script,              /* Reserved */
  #endif

  ssscr_user_ss_pref_opr_gw_orig_start_csps,
                                      /* User starts ps_call when SD doesn't have
                                      ** both services.
                                      */
                                      /* 40 */
  ssscr_user_ss_pref_opr_gw_orig_start_cs,
                                        /* User starts CS call when SD has GW 
                                        ** service but not CS domain. SD issues
                                        ** acquistion request for CS service
                                        */

  ssscr_user_ss_pref_opr_gw_orig_cs_no_conf, 
                                        /* User starts CS call when SD has GW 
                                        ** service and no mode or domain
                                        ** conflicts. SD must continue 
                                        ** operation and not issue
                                        ** acquistion request for CS service
                                        */

  ssscr_user_ss_pref_opr_gw_orig_ps_no_conf, 
                                        /* User starts PS call when SD has GW 
                                        ** service and no mode or domain
                                        ** conflicts. SD must continue 
                                        ** operation and not issue
                                        ** acquistion request for PS service
                                        */

  ssscr_user_ss_pref_opr_gw_orig_ps_resume_full_srv,   
                                        /* User trying to resume data for
                                        ** dormant PS call when SD has PS
                                        ** service and SD is in operation state.
                                        */

                                        /* 44 */
  ssscr_user_ss_pref_opr_gw_orig_ps_resume_no_ps_srv,   
                                        /* User trying to resume data for
                                        ** dormant PS call when SD is not
                                        ** having PS service and SD is in 
                                        ** operation state.
                                        */
                                         
                                        
  ssscr_user_ss_pref_opr_gw_orig_ps_resume_ltd_srv,
                                        /* User trying to resume data for
                                        ** dormant PS call when SD is having
                                        ** limited service and SD is in 
                                        ** operation state
                                        */

  ssscr_user_ss_pref_opr_gw_orig_ps_resume_no_srv_any,   
                                        /* User trying to resume data for
                                        ** dormant PS call when SD is in
                                        ** no service or power save.
                                        */

  ssscr_user_ss_pref_opr_gw,            /* User changes the SS-Preference while
                                        ** in operation state and the serving
                                        ** system is GW system.
                                        */

  #else

  ssscr_reserved_script,

  ssscr_reserved_script,

                                      /* 36 */
  ssscr_reserved_script,

  ssscr_reserved_script,

  ssscr_reserved_script,

  ssscr_reserved_script,

                                      /* 40 */
  ssscr_reserved_script,

  ssscr_reserved_script,

  ssscr_reserved_script,

  ssscr_reserved_script,
  
                                      /* 44 */
  ssscr_reserved_script,

  ssscr_reserved_script,

  ssscr_reserved_script,

  ssscr_reserved_script,

  #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                      /* 48 */
  ssscr_user_ss_pref_opr_domain,      /* User changes the Domain Preference while
                                      ** in operation state and the serving
                                      ** system is GW system in Manual Mode..
                                      */

  ssscr_user_ss_pref_opr,             /* User changes the SS-Preference while
                                      ** in operation state.
                                      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_user_ss_pref_pseudo_online, /* User changes the SS-Preference to put 
                                    ** the phone to pseudo online state
                                    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                      
  ssscr_user_ss_pref_redir_emerg,     /* User changes the SS-Preference to
                                      ** originate an emergency call while
                                      ** working on a redirection */

                                      /* 52 */
  ssscr_user_ss_pref_redir,           /* User changes the SS-Preference while
                                      ** working on a redirection */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_user_ss_pref_acq_resel_lmtd,  /* User changes the SS-Preference while
                                      ** MS is doing reselection after
                                      ** acquired the limited system */

  ssscr_user_ss_pref_acq_resel,       /* User changes the SS-Preference while
                                      ** MS is doing reselection */

                                      
  ssscr_user_ss_pref_vrfy_orig,       /* User changes the SS-Preference while
                                      ** in verification state in order to
                                      ** originate call other than OTASP */

                                      /* 56 */
  ssscr_user_ss_pref_acq_orig_normal, /* User changes the SS-Preference while
                                      ** in acquisition state in order to
                                      ** originate a normal call */

  ssscr_user_ss_pref_acq_orig_emerg,  /* User changes the SS-Preference while
                                      ** in acquisition state in order to
                                      ** originate an emergency call */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if ( defined (FEATURE_UMTS_UNIFORM_OOS_HANDLING) || \
       defined (FEATURE_UNIFORM_SCAN_OOS) )

  ssscr_user_ss_pref_orig_end_no_pref_change_acq,
                                     /* Call end, no pref change,
                                     ** scanning for service
                                     */
  ssscr_user_ss_pref_orig_end_no_pref_change_pwr_save,
                                     /* Call end, no pref change
                                     ** in deep sleep
                                     */

                                      /* 60 */                                      
  ssscr_user_ss_pref_orig_end_no_srv, /* Call origination ended and service
                                      ** status is no service or pwr_save */
  #else
  ssscr_reserved_script,              /* Reserved script */

  ssscr_reserved_script,              /* Reserved script */

  ssscr_reserved_script,              /* Reserved script */
  #endif
                                 
  ssscr_user_ss_pref_orig_end,        /* Call origination ended */

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
#error code not present
  #else
  ssscr_reserved_script,              /* Reserved script */
  ssscr_reserved_script,              /* Reserved script */
  ssscr_reserved_script,              /* Reserved script */
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                    
  ssscr_user_ss_pref,                 /* User changes the SS-Preference */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
  ssscr_user_nam_sel_gw_opr,          /* User changes the NAM selection in
                                      ** operation mode, and the target
                                      ** capability is GW only */

  ssscr_user_nam_sel_gw,              /* User changes the NAM selection and
                                      ** the target capability is GW only */

  #else
  ssscr_reserved_script,

  ssscr_reserved_script,

  #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */
                                      /* 68 */
  ssscr_user_nam_sel_manual,          /* User changes the NAM selection, and
                                      ** orig_mode is manual. */
                 
  ssscr_user_nam_sel,                 /* User changes the NAM selection */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_user_pwr_down,                /* User commands phone to phone to
                                      ** power-off, perform a reset or enter
                                      ** LPM */

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_user_get_net,                 /* User commands to get the available
                                      ** networks
                                      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                      /* 72 */
  ssscr_user_term_get_net,            /* User commands to terminate the get
                                      ** available networks command.
                                      */
  #else
  ssscr_reserved_script,

  ssscr_reserved_script,

  #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */

  ssscr_hybr_1_user_hybr_start,

  ssscr_hybr_1_user_hybr_end

};

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
/* <EJECT> */
/*===========================================================================
=============================================================================
===================== SCRIPTS FOR CDMA ACQUISITION EVENTS ===================
=============================================================================
===========================================================================*/


/*---------------------------------------------------------------------------
                       SDSS_EVT_CDMA_ACQ_FAILED_PILOT
---------------------------------------------------------------------------*/

/* Script for when acquisition fails during the pilot channel
** acquisition sub-state and:
**
** 1. Attempting to reacquire a system.
** 2. MS is not doing reselection.
** 3. MS is not a dual-mode target.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_acq_failed_pilot_reacq[] = {

  /* Condition */
  IS_EVENT( CDMA_ACQ_FAILED_PILOT )     /* Failed pilot acq */
  IS_ACQ_STATE( REACQ )                 /* Trying to reacquire the system */
  //IS_CONTAIN( SCHED_LST, NONE, ACQ_SYS )/* System is not on a reacq sched */
  NOT IS_RESEL_STT( SRCH )              /* MS is not doing reselection */
  IS_NOT( CAP_CDMA_CELL_NO_PCS )        /* MS is not a dual-mode target */

  /* Script */
  //SRV_OFF                             /* Set the srv indication to OFF */
  CPY( CAND_SYS, ACQ_SYS )              /* Set candidate sys per ACQ_SYS */
  SCHED_COND( ACQ_SYS, REACQ_ACQ_LOST, ALLOWED ) /* Schedule reacq attempts*/
  PREV_SCR                              /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                        SDSS_EVT_CDMA_ACQ_FAILED_SYNC
---------------------------------------------------------------------------*/

/* Script for when acquisition fails during the sync channel
** acquisition sub-state and:
**
** 1. Attempting to reacquire a system.
** 2. MS is not doing reselection.
** 3. MS is not a dual-mode target.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_acq_failed_sync_reacq[] = {

  /* Condition */
  IS_EVENT( CDMA_ACQ_FAILED_SYNC )      /* Failed receiving sync message */
  IS_ACQ_STATE( REACQ )                 /* Trying to reacquire the system */

  //IS_CONTAIN( SCHED_LST, NONE, ACQ_SYS ) /* System is not on a reacq sched */
  //NOT IS_SCHED( ACQ_SYS, REACQ_ACQ_LOST )/* Sys is not on acq lost sched */
  //NOT IS_SCHED( ACQ_SYS, REACQ_SYS_LOST )/* Sys is not on sys lost sched */

  NOT IS_RESEL_STT( SRCH )              /* MS is not doing reselection */
  IS_NOT( CAP_CDMA_CELL_NO_PCS )        /* MS is not a dual-mode target */

  /* Script */
  CPY( CAND_SYS, ACQ_SYS )              /* Set candidate sys per ACQ_SYS */
  #ifdef FEATURE_UNIFORM_SCAN_OOS
  SCHED_COND2( ACQ_SYS, REACQ_ACQ_LOST, ALLOWED, NOT_ON_SAME_SCHED )
                                        /* Schedule reacq attempts if not 
                                        ** already on a acq_lost or higer 
                                        ** schedule */
  #else
  SCHED_COND( ACQ_SYS, REACQ_ACQ_LOST, ALLOWED )/* Schedule reacq attempts */
  #endif

  PREV_SCR                              /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when acquisition fails during the sync channel
** acquisition sub-state and:
**
** 1  The acquisitions state is START.
** 2. MS is not doing reselection.
** 3. MS is not a dual-mode target.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_acq_failed_sync[] = {

  /* Condition */
  IS_EVENT( CDMA_ACQ_FAILED_SYNC )      /* Failed receiving sync message */
  IS_ACQ_STATE( START )                 /* The acquisitions state is START*/

  //IS_CONTAIN( SCHED_LST, NONE, ACQ_SYS ) /* System is not on a reacq sched */
  //NOT IS_SCHED( ACQ_SYS, REACQ_ACQ_LOST )/* Sys is not on acq lost sched */
  //NOT IS_SCHED( ACQ_SYS, REACQ_SYS_LOST )/* Sys is not on sys lost sched */

  NOT IS_RESEL_STT( SRCH )              /* MS is not doing reselection */
  IS_NOT( CAP_CDMA_CELL_NO_PCS )        /* MS is not a dual-mode target */

  /* Script */
  #ifdef FEATURE_UNIFORM_SCAN_OOS
  SCHED_COND2( ACQ_SYS, REACQ_ACQ_LOST, SAME_AS_LAST, NOT_ON_SAME_SCHED ) 
                                        /* Schedule reacq attempts if not 
                                        ** already on a acq_lost or higer 
                                        ** schedule */
  #else
  SCHED_COND( ACQ_SYS, REACQ_ACQ_LOST, SAME_AS_LAST )/* Schedule reacq attempts */
  #endif
  PREV_SCR                              /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                        SDSS_EVT_CDMA_ACQ_SCHM
---------------------------------------------------------------------------*/

static const  sdssscr_elem_u_type   ssscr_cdma_acq_schm_forbid_sys[] = {

  IS_EVENT( CDMA_ACQ_SCHM )             /* Received sync message */
  IS_PRL( ACQ_SYS, FORBID )             /* Acquired system is forbidden */
  NOT IS_ORIG_MODE( OTASP_OR_EMERG )    /* Not in OTASP or EMERG call */
  IS_HANDOFF( NONE )                    /* Not doing handoff */
  IS_REDIR( NONE )                      /* Redirection indication is OFF */
  NOT IS_RESEL_STT( SRCH )              /* MS is not doing reselection */
  NOT IS_ACQ_TYPE( ANY )                /* Not doing ANY acquisitions */
  IS( FEATURE_UOOS )                    /* UOOS is defined */

  /* Script */
  AVOID_COND( ACQ_SYS, 600, UNTIL_ACQ_FAIL ) 
                              /* Avoid the forbidden system            */
  DEL( SCHED_LST, ACQ_SYS )   /* Remove the acq_sys from the schedule */
  DEL( MRU_LST, ACQ_SYS )     /* Remove the acq_sys from the MRU     */
  CPY( FORBID_GEO_LST, ACQ_PREF_LST )
                              /* Construct the forbidded GEO list */
  IF( ACQ_SYS_PRL_DESIG, EQUAL_TO, SDSS_PRL_VAL_NEG )
                              /* If the system is a negative system */
      CLR( ACQ_FULL2_SYS )    /* Clear the rotating FULL scan position */ 
      CLR( ACQ_MICRO_SYS )    /* Clear the rotating MICRO scan position */ 
      CLR( ACQ_MICRO2_SYS )   /* Clear the rotating MICRO scan position */ 
  ENDIF

  NEW_ACQ                     /* Start a new acq */
  PROC_EVT( INT_SRV_LOST )    /* Process the internal service lost event*/
  END
};


/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_ACQ_PROT_MIS
---------------------------------------------------------------------------*/

/* Script for when acquisition fails due to protocol mismatch;
** that is MOB_P_REV < MIN_P_REV or MS does not support the Paging Channel
** data rate.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_acq_prot_mis[] = {

  /* Condition */
  IS_EVENT( CDMA_ACQ_PROT_MIS )   /* CDMA protocol mismatch */

  /* Script */
  AVOID( ACQ_SYS, 60 )            /* Avoid the acquired sys for 60s */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_ACQ_HO_FAIL
---------------------------------------------------------------------------*/

/* Script for when acquisition fails due to protocol mismatch;
** that is MOB_P_REV < MIN_P_REV or MS does not support the Paging Channel
** data rate.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_acq_ho_fail[] = {

  /* Condition */
  IS_EVENT( CDMA_ACQ_HO_FAIL )   /* CDMA protocol mismatch */

  /* Script */
  SRV_STATUS(PWR_SAVE)
  PWR_SAVE
  CONT(0)
  END
};

/*---------------------------------------------------------------------------
                    CDMA Acquisition Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with CDMA
** acquisition events.
*/
static const sdssscr_elem_u_type * const dsssscr_cdma_acq_event_tbl[] = {

                                      /* 0 */
  ssscr_cdma_acq_failed_pilot_reacq,  /* Exception script for when
                                      ** acquisition fails during the pilot
                                      ** channel acquisition sub-state while
                                      ** attempting to reacquire a system and
                                      ** this system is not already on a
                                      ** reacquisition schedule */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_acq_failed_sync_reacq,   /* Exception SS-Script for when
                                      ** acquisition fails during the sync
                                      ** channel acquisition sub-state while
                                      ** attempting to reacquire a system and
                                      ** this system is not already on a
                                      ** reacquisition schedule */

  ssscr_cdma_acq_failed_sync,         /* Exception SS-Script for when
                                      ** acquisition fails during the sync
                                      ** channel acquisition sub-state while
                                      ** attempting to acquire a system and
                                      ** this system is not already on a
                                      ** reacquisition schedule */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_acq_schm_forbid_sys,     /* Acquired SCHM on a forbidden system
                                      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                      /* 4 */
  ssscr_cdma_acq_prot_mis,            /* CDMA acquisition fails due to a
                                      ** protocol revision mismatch (i.e.,
                                      ** MOB_P_REV < MIN_P_REV or MS does not
                                      ** support the Paging Channel data
                                      ** rate */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_acq_ho_fail              /* CDMA handoff fails
                                      */

};
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */


#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
/* <EJECT> */
/*===========================================================================
=============================================================================
===================== SCRIPTS FOR CDMA OPERATION EVENTS =====================
=============================================================================
===========================================================================*/

/*---------------------------------------------------------------------------
                        SDSS_EVT_CDMA_OPR_OH_SID
---------------------------------------------------------------------------*/


/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The SID/NID of the new serving system is different from the previous
**    SID/NID (i.e., not expected).
** 2. Redirection indication is ON.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_new_redir[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_SID( SRV_SYS, NOT_EXPECTED ) /* SID/NID is diff. from previous one */
  IS_REDIR( ANY )                 /* Redirection indication is ON */

  /* Script */
  REDIR_OFF                       /* Turn redirection indication OFF */
  PROC_EVT( CDMA_OPR_OH_SID )     /* Reprocess the event */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system conflicts with the current SS-Preference:
** 2. The system selection state is verification (as opposed to operation).
** 3. Redirection indication is OFF.
** 4. Handoff indication is OFF.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_conf_vrfy[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_PREF_CONF( SRV_SYS, ANY )    /* Acquired system conflicts with SS-Pref*/
  IS_STATE( VRFY )                /* The SS-State is verification */
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */

  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system is a forbidden PRL system (i.e., negative or
**    unavailable).
** 2. The system selection state is verification (as opposed to operation).
** 3. Redirection indication is OFF.
** 4. Handoff indication is OFF.
** 5. Origination mode is not OTASP or emergency.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_frbd_vrfy[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_PRL( SRV_SYS, FORBID )       /* Acquired system is a forbidden PRL sys*/
  IS_STATE( VRFY )                /* The SS-State is verification */
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  NOT IS_ORIG_MODE( OTASP_OR_EMERG )/* Origination mode is not OTASP/EMERG */

  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system is a negative PRL system.
** 2. The system selection state is verification (as opposed to operation).
** 3. Redirection indication is ON.
** 4. Handoff indication is OFF.
** 5. Origination mode is not OTASP or emergency.
*/
static const  sdssscr_elem_u_type  ssscr_cdma_opr_oh_sid_neg_vrfy_redir[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_PRL( SRV_SYS, NEG )          /* Acquired system is a negative PRL sys*/
  IS_STATE( VRFY )                /* The SS-State is verification */
  IS_REDIR( ANY )                 /* Redirection indication is ON */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  NOT IS_ORIG_MODE( OTASP_OR_EMERG )/* Origination mode is not OTASP/EMERG */

  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system conflicts with the current mode preference.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_conf_mode[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_PREF_CONF( SRV_SYS, MODE )   /* Acquired system conflicts mode pref */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system conflicts with the current SS-Preference:
** 2. Redirection indication is OFF.
** 3. Handoff indication is OFF.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_conf[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_PREF_CONF( SRV_SYS, ANY )    /* Acquired system conflicts with SS-Pref*/
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */

  /* Script */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system is a forbidden PRL system (i.e., negative or
**    unavailable).
** 2. Redirection indication is OFF.
** 3. Handoff indication is OFF.
** 4. Origination mode is not otasp or emergency.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_frbd[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_PRL( SRV_SYS, FORBID )       /* Acquired system is a forbidden PRL sys*/
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  NOT IS_ORIG_MODE( OTASP_OR_EMERG )/* Origination mode is not OTASP/EMERG */

  /* Script */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. There are more preferred systems (according to the PRL) than the
**    acquired system.
** 2. The SID/NID of the new serving system is different from the previous
**    SID/NID (i.e., not expected).
** 3. The SS-State is operation.
** 4. Redirection indication is OFF.
** 5. Handoff indication is OFF.
** 6. Origination mode is none (i.e., not trying to originate a call).
** 7. MS is not CDMA locked until power cycle.
** 8. FEATURE_JCDMA is off.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_pref_new[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/
  IS_SID( SRV_SYS, NOT_EXPECTED ) /* SID/NID is diff. from previous one */
  IS_STATE( OPR )                 /* The SS-State is OPR */
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  IS_ORIG_MODE( NONE )            /* Not trying to originate a call */
  IS_NOT( CDMA_LOCKED )           /* Not CDMA locked until power cycle */

  /* Script */
  NEW_ACQ                         /* Start a new acquisition */
  #ifdef FEATURE_JCDMA
  RESEL_STT( WAIT )               /* Set reselection state to NONE */
  CONT2( TMR_CDMA_RESEL_HOLD_IDLE_HOFF)
                                  /* Wait for a short time */
  ACT_SCR( RESEL_JCDMA )          /* Do JCDMA reselection */
  #else
  LOOP_COND( IS_HDR_ACTIVITY )    /* Hold off Reselection if HDR_ACTIVITY is going */
    CONT2( TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY )
  LOOP
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  CONT2( TMR_CDMA_RESEL_HOLD_IDLE_HOFF)
                                  /* Wait for a short time */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( SRV_PREF_LST, MORE_PREF_THAN_SRV_SYS )
                                  /* Try acquiring more preferred systems */
  CPY( CAND_SYS, SRV_SYS )        /* Set candidate sys per the SRV_SYS */
  SCHED_COND( SRV_SYS, REACQ_SYS_LOST, ALLOWED ) /* Schedule reacq attempts*/
  ACQ_ALL( SRV_SYS, ALLOWED )     /* Try reacquiring serving system */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  #endif
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. There are more preferred systems (according to the PRL) than the
**    acquired system.
** 2. Redirection indication is OFF.
** 3. Handoff indication is OFF.
** 4. Origination mode is none (i.e., not trying to originate a call).
** 5. MS is not CDMA locked until power cycle.
** 6. FEATURE_JCDMA is off.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_pref[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  IS_ORIG_MODE( NONE )            /* Not trying to originate a call */
  IS_NOT( CDMA_LOCKED )           /* Not CDMA locked until power cycle */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. Origination mode is emergency (origination or callback).
** 2. Redirection indication is ON.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_emerg_redir[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_ORIG_MODE( EMERG )           /* Origination mode is emergency */
  IS_REDIR( ANY )                 /* Redirection indication is ON */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  ACT_SCR( CLR_ACQ_LSTS )         /* Clear acq process related lists */
  CPY( SRCH_LST, REDIR_LST )      /* Copy redirection list into SRCH list */
  APP( SRCH_LST, EMERG_LST )      /* Append emergency list to SRCH_LST */
  CPY( EMERG_LST, SRCH_LST )      /* Copy SRCH list into emergency list */
  CONT( 0 )                       /* Continue with CDMA operation */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. Origination mode is emergency (origination or callback).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_emerg[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_ORIG_MODE( EMERG )           /* Origination mode is emergency */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  ACT_SCR( CLR_ACQ_LSTS )         /* Clear acq process related lists */
  CONT( 0 )                       /* Continue with CDMA operation */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. Handoff indication is ON.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_handoff[]= {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_HANDOFF( CDMA_ANY_TO_ANY )   /* Handoff indication is ON */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  CONT( 2 )                       /* Wait 2s */
  HO_OFF                          /* set handoff indication to OFF */
  CONT( 18 )                      /* Wait 18s - see if we get CAM to TC */

  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( CDMA_OPR_OH_SID )     /* No CAM to TC - do overhead event proc */

  //CPY( IDL_SYS, SRV_SYS )         /* Set idle sys per the SRV_SYS */
  //RESEL_STT( NONE )               /* Set reselection state to NONE */
  //NEW_ACQ                         /* Start a new acquisition */
  //ACQ_MODE( FULL )                /* Do full acquisitions */
  //ACQ( SRV_PREF_LST, MORE_PREF )  /* Try acquiring more preferred systems */
  //CPY( CAND_SYS, SRV_SYS )        /* Set candidate sys per the SRV_SYS */
  //SCHED_COND( SRV_SYS, REACQ_ACQ_LOST, ALLOWED ) /* Schedule reacq attempts*/
  //ACQ( SRV_SYS, ALLOWED )         /* Try reacquiring serving system */

  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. Redirection indication is GSRM, SRDM, or NDSS.
** 2. There are more preferred systems (according to the PRL) than the
**    acquired system.
** 3. Origination mode is none (i.e., not trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_gsrdm_srdm_ndss[]= {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )         /* New CDMA system is fully acquired */
  IS_REDIR( CDMA_ANY )              /* Redirection indication is GSRM, SRDM, NDSS */
  //IS_PRL( REDIR_SYS, NOT_MOST_PREF )  /* Redirecting sys not most preferred*/
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  RESEL_STT( NONE )               /* Set reselection state to NONE */

  CONT2_REDIR( TMR_CDMA_REDIR )   /* Wait appropriate no. of seconds since
                                  ** redirection received */
  LOOP_COND( IS_HDR_ACTIVITY )
    CONT2( TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY )
  LOOP


  REDIR_OFF                       /* Turn redirection indication OFF */
  HO_OFF                          /* Turn handoff indication OFF */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  CPY( SRCH_LST, REDIR_SYS )      /* Copy redirecting system to SRCH_LST */
  APP( SRCH_LST, IDL_SYS )        /* Append redirected system to SRCH_LST */
  APP( SRCH_LST, MRU_LST )        /* Append MRU systems to SRCH_LST */
  APP( SRCH_LST, PRL_LST )        /* Append PRL systems to SRCH_LST */

  #ifdef FEATURE_JCDMA
  DEL_SYS( SRCH_LST, NON_CDMA )   /* Delete Non CDMA systems from SRCH_LST */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete Non compliant systems from
                                  ** SRCH_LST */
  MEAS( SRCH_LST, DED, PILOT )    /* Get pilot strength measurement */
  ACQ_PI( SRCH_LST, BEST_PREF, CDMA_PI_THR )
                                  /* Try to acq best pref sys with PI > 
                                  ** CDMA_PI_THR
                                  */
  #else
  ACQ( SRCH_LST, BEST_PREF )     /* Try acquiring the SRCH_LST systems */
  #endif

  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Try to reacquire available systems */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Acq failed - do sys lost event proc */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. There are more preferred systems (according to the PRL) than the
**    acquired system.
** 2. Redirection indication is OFF.
** 3. Handoff indication is OFF.
** 4. In Release State
** 5. Origination mode is not none
** 6. SS_PREF is for call origination state
**
** This script triggers call end reselection procedure when a MO call ends.
*/

static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_pref_resel[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  IS_RESEL_STT( RELEASE )         /* Rselection is in release state. */
  IS_ORIG_MODE( ANY )             /* User trying to originate a call */
  IS_PREF_REAS( CALL_ORIG )       /* SS-Pref is not for origination NOT_ORIG*/

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  CONT( 0 )                       /* Continue with CDMA operation */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_OPR_OH_CHAN
---------------------------------------------------------------------------*/

/* Script for when a new CDMA system with a channel different from the
** previous serving system is fully acquired and:
** 1. No reselection is being done.
** 2. There are more preferred systems than the current system
*/

static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_chan[] = {
  IS_EVENT( CDMA_OPR_OH_CHAN )     /* New CDMA system with different channel
                                   ** is fully acquired. */
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/
  IS_RESEL_STT( NONE )             /* Not doing reselection currently. */

  PROC_EVT( CDMA_OPR_OH_SID )      /* Reprocess the event as OH SID change */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_OPR_RESCAN
---------------------------------------------------------------------------*/

/* Script for when MS receives a SPM with the RESCAN field set
** to 1 and:
**
** 1. Origination mode is not emergency callback.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_rescan[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_RESCAN )     /* Receives SPM with RESCAN field = 1 */
  NOT IS_ORIG_MODE( EMERG_CB )    /* Origination mode is not emerg-callback*/

  /* Script */
  #ifdef FEATURE_JCDMA
  ACQ_ALL( ACQ_SYS, ALLOWED )     /* Try to reacquire the ACQ_SYS */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  #else
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  #endif /* FEATURE_JCDMA */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_OPR_REG_REJ
---------------------------------------------------------------------------*/

/* Script for when registration is rejected and origination mode
** is emergency (origination or callback).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_reg_rej_emerg[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REG_REJ )    /* CDMA registration rejected */
  IS_ORIG_MODE( EMERG )           /* Origination mode is emergency */

  /* Script */
  REDIR_OFF                       /* Turn redirection indication OFF */
  HO_OFF                          /* Turn handoff indication OFF */
  CONT( 0 )                       /* Ignore registration rejected */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when registration is rejected and:
**
** 1. Redirection indication is ON.
** 2. RETURN_IF_FAILED is 1
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_reg_rej_redir_rif[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REG_REJ )    /* CDMA registration rejected */
  IS_REDIR( ANY )                 /* Redirection indication is ON */
  IS( REDIR_RIF )                 /* RETURN_IF_FAILED = 1 */

  /* Script */
  REDIR_OFF                       /* Turn redirection indication OFF */
  HO_OFF                          /* Turn handoff indication OFF */
  //AVOID( ACQ_SYS, 60 )            /* Avoid the acquired sys for 60s */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  CPY( CAND_SYS, REDIR_SYS )      /* Set candidate sys per the REDIR_SYS */
  ACQ_ALL( REDIR_SYS, ALLOWED )   /* Try to reacquire the redirecting sys */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  //ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when registration is rejected and:
**
** 1. Redirection indication is ON.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_reg_rej_redir[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REG_REJ )    /* CDMA registration rejected */
  IS_REDIR( ANY )                 /* Redirection indication is ON */

  /* Script */
  //AVOID( ACQ_SYS, 60 )            /* Avoid the acquired sys for 60s */
  AVOID( REDIR_SYS, 30 )          /* Avoid redirecting system for 30s */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when registration is rejected.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_reg_rej[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REG_REJ )    /* CDMA registration rejected */

  /* Script */
  AVOID( ACQ_SYS, 30 )            /* Avoid the acquired sys for 30s */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_OPR_SYS_LOST
---------------------------------------------------------------------------*/

/* Script for when a CDMA serving system is lost and:
**
** 1. The SS-State is verification (as opposed to operation).
** 2. MS is not doing reselection.
** 3. MS is not a dual-mode target.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_vrfy_sched[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS_STATE( VRFY )                /* The SS-State is VERIFY */
  NOT IS_RESEL_STT( SRCH )        /* MS is not doing reselection */
  IS_NOT( CAP_CDMA_CELL_NO_PCS )  /* MS is not a dual-mode target */

  /* Script */
  SRV_STATUS( NO_SRV )            /* Set service indication to OFF */
  #ifdef FEATURE_UNIFORM_SCAN_OOS
  SCHED_COND2( ACQ_SYS, REACQ_ACQ_LOST, ANY, NOT_ON_SAME_SCHED )
                                  /* Schedule the lost system, if not already
                                  ** on a acq_lost or higher schedule */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  #else
  SCHED_COND( ACQ_SYS, REACQ_ACQ_LOST, ANY ) /* Schedule reacq attempts */
  PREV_SCR                        /* Continue with previous script */ 
  #endif
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. The SS-State is verification (as opposed to operation).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_vrfy[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS_STATE( VRFY )                /* The SS-State is VERIFY */

  /* Script */

  #ifdef FEATURE_UNIFORM_SCAN_OOS
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  #else
  PREV_SCR                        /* Continue with previous script */ 
  #endif
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. The SS-State is operation (no need to check).
** 2. Origination mode is OTASP or emergency.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_otasp_emerg[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS_ORIG_MODE( OTASP_OR_EMERG )  /* Origination mode is OTASP/emergency */

  /* Script */
  SCHED_COND( IDL_SYS, REACQ_EMERG_LOST, ANY ) /* Schedule reacq attempts */
  DEL( OTASP_LST, IDL_SYS )       /* Delete idle sys from OTASP list */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. FEATURE_JCDMA is on.
** 2. The access reason is Registration.
** 2. MS is in over-reach (i.e. MS can not reach the BS).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_over_reach[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS( FEATURE_JCDMA )             /* FEATURE_JCDMA is on */
  IS_ACC_REAS( REG )              /* Access reason is registration */
  IS_ACC_FAIL( 5, 0 )             /* Access failed at least 5 times */

  /* Script */
  PROC_EVT( CDMA_OPR_ACC_TERM )   /* Do access termination processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. MS is a dual-mode target.
** 2. The lost system is a preferred PRL system.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_dm_prl[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS ( CAP_AMPS )                 /* Capable of AMPS */
  IS( CAP_CDMA_CELL_NO_PCS )      /* MS is a dual-mode target */
  IS_PRL( IDL_SYS, ANY_PREF )     /* Idle system is a preferred PRL sys */

  /* Script */
  SCHED( AMPS_BSIDL_SYS, REACQ_1_TIME, PREFERRED )/*Schedule reacq attempts*/
  SCHED( IDL_SYS, REACQ_1_TIME, PREFERRED )       /*Schedule reacq attempts*/
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. MS is a dual-mode target.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_dm[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS ( CAP_AMPS )                 /* Capable of AMPS */
  IS( CAP_CDMA_CELL_NO_PCS )      /* MS is a dual-mode target */

  /* Script */
  SCHED( AMPS_BSIDL_SYS, REACQ_1_TIME, ALLOWED )/* Schedule reacq attempts */
  SCHED( IDL_SYS, REACQ_1_TIME, ALLOWED )       /* Schedule reacq attempts */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. Origination mode is ANY (i.e. user is trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_orig[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS_ORIG_MODE( ANY )             /* User trying to originate a call */

  /* Script */
  #if (defined(FEATURE_UNIFORM_SCAN_OOS))
  SCHED_COND( IDL_SYS, REACQ_EVERY_1S, ALLOWED ) /* Schedule reacq attempts*/
  #elif (defined (FEATURE_JCDMA))
  SCHED_COND( IDL_SYS, REACQ_1_TIME, ALLOWED )   /* Schedule reacq attempts */
  #else
  SCHED_COND( IDL_SYS, REACQ_SYS_LOST, ALLOWED ) /* Schedule reacq attempts*/
  #endif

  CPY( TEMP_LST, SRV_SYS )         /* Set TEMP_LST to SRV_SYS */
  DEL( TEMP_LST, IDL_SYS )         /* Empty TEMP_LST if SRV_SYS == IDL_SYS */
  SCHED_COND( TEMP_LST, REACQ_1_TIME, ANY )
                                  /* Try to acquire the traffic channel */

  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */

  /* Script */
  #ifdef FEATURE_UNIFORM_SCAN_OOS
  SCHED_COND( IDL_SYS, REACQ_EVERY_1S, BEST_PREF ) /* Schedule reacq attempts*/
  #elif defined FEATURE_JCDMA
  SCHED_COND( IDL_SYS, REACQ_1_TIME, BEST_PREF )   /* Schedule reacq attempts */
  #else
  SCHED_COND( IDL_SYS, REACQ_SYS_LOST, BEST_PREF ) /* Schedule reacq attempts*/
  #endif

  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_OPR_REDIR
---------------------------------------------------------------------------*/

/* Script for when MS receives invalid redirection of type GSRM and:
**
** 1. FEATURE_T53 is on.
** 2. RETURN_IF_FAIL = 1.
*/
static const  sdssscr_elem_u_type ssscr_cdma_opr_redir_invalid_gsrm_rif[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */
  IS_NOT( REDIR_VALID )           /* Redirection is invalid for this target*/
  IS_REDIR( CDMA_GSRDM )          /* Redirection type is GSRM */
  IS( REDIR_RIF )                 /* RETURN_IF_FAILED = 1 */

  /* Script */
  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  REJECT                          /* Reject the redirection */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives invalid redirection of type GSRM.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_redir_invalid_gsrm[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */
  IS_NOT( REDIR_VALID )           /* Redirection is invalid for this target*/
  IS_REDIR( CDMA_GSRDM )          /* Redirection type is GSRM */

  /* Script */
  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  REJECT                          /* Reject the redirection */
  AVOID( REDIR_SYS, 30 )          /* Avoid redirecting system for 30s */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP_SRV_OFF_UNTIL_OH )        
                                  /* Do skip-over service lost processing 
                                  ** Keep service off until receiving OH
                                  */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives invalid redirection.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_redir_invalid[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */
  IS_NOT( REDIR_VALID )           /* Redirection is invalid for this target*/

  /* Script */
  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  REJECT                          /* Reject the redirection */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives redirection message and:
**
** 1. Origination mode is emergency origination.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_redir_emerg[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */
  IS_ORIG_MODE( EMERG_ORIG )      /* Orig mode is emergency origination */

  /* Script */
  HO_OFF                          /* Set handoff indication to OFF */
  ACCEPT                          /* Accept redirection */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_ALL( REDIR_LST, ANY )       /* Acquire systems regardless of exp SID */
  REDIR_OFF                       /* Redirection failed - turn it off */
  //ACQ_ALL( REDIR_SYS, ANY )       /* Acquire back redirecting system */
  ACT_SCR( SRV_LOST_SKIP_SRV_OFF_UNTIL_OH )          
                                  /* Do skip-over service lost processing 
                                  ** Keep service off until receiving OH
                                  */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives redirection message and:
**
** RETURN_IF_FAILED = 1 (i.e., MS should return to the redirecting system
** if redirection fails).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_redir_rif[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */
  IS( REDIR_RIF )                 /* RETURN_IF_FAILED = 1 */

  /* Script */
  HO_OFF                          /* Set handoff indication to OFF */
  ACCEPT                          /* Accept redirection */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_ALL( REDIR_LST, SPECIFIC )  /* Try acquiring redirection's systems */
  REDIR_OFF                       /* Redirection failed - turn it off */
  #ifdef FEATURE_JCDMA
  SCHED_COND( IDL_SYS, REACQ_1_TIME, BEST_PREF )
                                  /* Try reacquiring redirecting system */
  #else
  SCHED_COND( IDL_SYS, REACQ_SYS_LOST, BEST_PREF )
                                  /* Try reacquiring redirecting system */
  #endif
  ACT_SCR( SRV_LOST_NORM_SRV_OFF_UNTIL_OH )        
                                  /* Do normal service lost processing 
                                  ** Keep service off until receiving OH
                                  */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives redirection message.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_redir[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */

  /* Script */
  HO_OFF                          /* Set handoff indication to OFF */
  ACCEPT                          /* Accept redirection */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_ALL( REDIR_LST, SPECIFIC )  /* Try acquiring redirection's systems */
  REDIR_OFF                       /* Redirection failed - turn it off */
  AVOID( REDIR_SYS, 30 )          /* Avoid redirecting system for 30s */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP_SRV_OFF_UNTIL_OH )        
                                  /* Do skip-over service lost processing 
                                  ** Do not update the service status until
                                  ** OH
                                  */
  END
};

/*---------------------------------------------------------------------------
                        SDSS_EVT_CDMA_OPR_ACPT_REJ
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                        SDSS_EVT_CDMA_OPR_NDSS_OFF
---------------------------------------------------------------------------*/

/* Script for when MS receives NDSS OFF indication.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_ndss_off[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_NDSS_OFF )   /* Receives NDSS OFF indication */
  IS_REDIR( ANY )                 /* Redirection is ON */

  /* Script */
  REDIR_OFF                       /* Turn redirection indication OFF */
  HO_OFF                          /* Set handoff indication to OFF */
  CLR( CAND_SYS )                 /* Clear the candidate system */
  CLR( AVAIL_LST )                /* Clear the available list */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( MRU_LST, BEST_PREF )       /* Try acquiring MRU systems */
  ACQ( PRL_LST, BEST_PREF )       /* Try acquiring PRL systems */
  //ORDER( AVAIL_LST, PRL_LST )     /* Order avail systems according to PRL */
  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Try to reacquire available systems */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Acq failed - do sys lost event proc */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END

  //ACQ_MODE( MINI )                    /* Do mini acquisitions */
  //ACQ_ALL( IDL_PREF_LST, MORE_PREF )  /* Try acquiring more preferred sys */
  //PROC_EVT( CDMA_OPR_SYS_LOST )       /* Try reacquiring the idle system */
  //ACT_SCR( SRV_LOST_NORM )            /* Do normal service lost processing */
  //END
};

/*---------------------------------------------------------------------------
                        SDSS_EVT_CDMA_OPR_NEW_SYS
---------------------------------------------------------------------------*/

/* Script for when MS idle-handoff to a BS with unknown
** configuration.
** Might need to have two scripts for emergency and normal cases. Look what
** current code is doing.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_new_sys[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_NEW_SYS )    /* Handoff to a BS with unknown config */

  /* Script */
  REDIR_OFF                       /* Turn redirection off */
  HO_OFF                          /* Set handoff indication to OFF */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  CPY( CAND_SYS, NEW_SYS )        /* Set candidate sys per the NEW_SYS */
  ACQ_ALL( NEW_SYS, ANY )         /* Attempt acquiring the new system */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*---------------------------------------------------------------------------
                        SDSS_EVT_CDMA_OPR_SYS_RESEL
---------------------------------------------------------------------------*/

/* Script for when MS detects that pilot power is below a BS specified
** threshold and:
**
** 1. Origination mode is emergency callback.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_resel_emerg_cb[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_RESEL )    /* Pilot power is below threshold */
  IS_ORIG_MODE( EMERG_CB )          /* Orig mode is emergency-callback */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS detects that pilot power is below a BS specified
** threshold and:
**
** 1. Idle system is a preferred PRL system.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_resel_prl[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_RESEL )    /* Pilot power is below threshold */
  IS_PRL( IDL_SYS, ANY_PREF )       /* Idle system is a preferred PRL sys */

  /* Script */
                                    /* Delete non-PRL sys from SYS_RESEL */
  CPY( SRCH_LST, SYS_RESEL_LST )    /* Copy SYS_RESEL list into SRCH list */
  DEL( SRCH_LST, PRL_LST )          /* Delete PRL list from SRCH list */
  DEL( SYS_RESEL_LST, SRCH_LST )    /* Delete SRCH list from SYS_RESEL list*/

  REDIR_OFF                         /* Turn redirection indication OFF */
  HO_OFF                            /* Turn handoff indication OFF */
  NEW_ACQ                           /* Start a new acquisition */
  ACQ( SYS_RESEL_LST, PREFERRED )   /* Try acquiring from reselection list */
  SRV_STATUS( NO_SRV )            /* Set service indication to OFF */
  AVOID( IDL_SYS, UNTIL_NEW_ACQ )   /* Avoid idle sys until NEW_ACQ */
  ACQ_MODE( FULL )                  /* Do full acquisitions */
  ACQ( IDL_GEO_LST, PREFERRED )     /* Try acquiring from idle GEO list */
  //CPY( SRCH_LST, SYS_RESEL_LST )    /* Copy SYS_RESEL list into SRCH list */
  //APP( SRCH_LST, IDL_GEO_LST )      /* Append to SRCH list idle GEO list */
  //AVOID( ACQ_SYS, 30 )              /* Acq failed - avoid acq sys for 30s */
  //ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
  //ACT_SCR( SRV_LOST_SKIP_ACQ )       /* Do skip-over acq-sys srv lost proc*/
  ACT_SCR( SRV_LOST_SKIP )           /* Do skip-over service lost proc */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS detects that pilot power is below a BS specified
** threshold and:
**
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_resel[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_RESEL )    /* Pilot power is below threshold */

  /* Script */
                                    /* Delete non-PRL sys from SYS_RESEL */
  CPY( SRCH_LST, SYS_RESEL_LST )    /* Copy SYS_RESEL list into SRCH list */
  DEL( SRCH_LST, PRL_LST )          /* Delete PRL list from SRCH list */
  DEL( SYS_RESEL_LST, SRCH_LST )    /* Delete SRCH list from SYS_RESEL list*/

  REDIR_OFF                         /* Turn redirection indication OFF */
  HO_OFF                            /* Turn handoff indication OFF */
  ACQ_MODE( FULL )                  /* Do full acquisitions */
  ACQ_ALL( SYS_RESEL_LST, ANY )     /* Try acquiring from reselection list */
  //AVOID( ACQ_SYS, 30 )              /* Acq failed - avoid acq sys for 30s */
  //ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP_ACQ )       /* Do skip-over acq-sys srv lost proc*/
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_CDMA_OPR_LOCK
---------------------------------------------------------------------------*/

/* Script for when MS receives a lock order and:;
**
** 1. Origination mode is not OTASP or emergency.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_lock[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_LOCK )       /* Receives a lock order */
  NOT IS_ORIG_MODE( OTASP )       /* Origination mode is not OTASP */

  /* Script */
  REDIR_OFF                       /* Set redirection indication to OFF */
  HO_OFF                          /* Set handoff indication to OFF */
  CLR( SRCH_LST )                 /* Clear the search list */
  APP( SRCH_LST, AMPS_1ST_SYS )   /* Append 1st AMPS sys the search list */
  APP( SRCH_LST, AMPS_2ND_SYS )   /* Append 2nd AMPS sys the search list */
  ORDER( SRCH_LST, IDL_GEO_LST )  /* Order search list according to GEO */
  ACQ_ALL( SRCH_LST, ALLOWED )    /* Attempt to acquire AMPS systems */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Acq failed, reacquire the locked sys */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  //ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_CDMA_OPR_UNLOCK
---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_OPR_ACC_TERM
---------------------------------------------------------------------------*/

/* Script for when access attempt terminates and (MS is in over-reach
** condition for long time):
**
** 1. The access reason is traffic to idle transition.
*/
static const  sdssscr_elem_u_type ssscr_cdma_opr_acc_term_traffic[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_REAS( TRAFFIC )          /* Access reason is TC to idle transition*/

  /* Script */
  RESEL_STT( RELEASE )            /* Set reselection state to RELEASE */
  HO_OFF                          /* Set handoff indication to OFF */
  PROC_EVT( CDMA_OPR_OH_SID )     /* Do OH SID event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef FEATURE_UNIFORM_SCAN_OOS

/* Script for when access attempt terminates and (MS is in over-reach
** condition for long time):
**
** 1. FEATURE_JCDMA is on.
** 2. The access reason is Registration.
** 3. Access as failed for 6 or more times and 11 or more minutes.
*/
static const  sdssscr_elem_u_type ssscr_cdma_opr_acc_term_overreach_l[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS( FEATURE_JCDMA )             /* FEATURE_JCDMA is on */
  IS_ACC_REAS( REG )              /* Access reason is registration */
  IS_ACC_FAIL( 6, 11 )            /* Access failed at least 6 times and 11m*/

  /* Script */
  ACT_SCR( PWR_SAVE_OVERREACH_2_3 )/* Activate over-reach proc. 2-3 script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and (MS is in over-reach
** condition):
**
** 1. FEATURE_JCDMA is on.
** 2. The access reason is Registration.
** 3. Access as failed for 6 or more times.
*/
static const  sdssscr_elem_u_type ssscr_cdma_opr_acc_term_overreach[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS( FEATURE_JCDMA )             /* FEATURE_JCDMA is on */
  IS_ACC_REAS( REG )              /* Access reason is registration */
  IS_ACC_FAIL( 6, 0 )             /* Access failed at least 6 times */

  /* Script */
  ACT_SCR( PWR_SAVE_OVERREACH_2_2 )/* Activate over-reach proc. 2-2 script */
  END
};
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when access attempt terminates and:
//** 1. FEATURE_JCDMA is on.
//** 2. The access reason is Registration.
//** 3. Access as failed for 6 or more times.
//*/
//static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_reg_fail_6[] = {
//
//  /* Condition */
//  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
//  IS( FEATURE_JCDMA )             /* FEATURE_JCDMA is on */
//  IS_ACC_REAS( REG )              /* Access reason is registration */
//  IS_ACC_FAIL( 6, 0 )             /* Access failed 6 times or more */
//
//  /* Script */
//  SRV_OFF_UNTIL( REG )            /* Set service OFF until successful reg */
//  ACT_SCR( SRV_LOST_SKIP_ACQ )    /* Do skip-over acq-sys srv lost proc*/
//  //PREV_SCR                        /* Continue with previous script */
//  END
//};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and emergency callback
** mode is in effect.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_emerg_cb[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ORIG_MODE( EMERG_CB )        /* In emergency callback mode */

  /* Script */
  CONT( 0 )                       /* Continue with CDMA operation */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is access denied.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_acc_den[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( ACC_DEN )          /* Due to access denied */

  /* Script */
  //#ifdef FEATURE_JCDMA
  //ACQ_ALL( ACQ_SYS, ALLOWED )     /* Try to reacquire the ACQ_SYS */
  //#else
  #ifdef FEATURE_JCDMA
  AVOID_COND( ACQ_SYS, 60, UNTIL_ACQ_FAIL_DUR_CALL_ORIG ) 
                                  /* Avoid the acquired sys for 60s or
                                  ** all the channel are exhaused during call
                                  ** origination, which ever comes first.
                                  */
  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list      */
  APP( SRCH_LST, MRU_LST )        /* Order SRCH list by MRU list          */
  APP( SRCH_LST, PRL_LST )

  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list by MRU list          */

  ACQ( SRCH_LST, PREFERRED )      /* Acquire SRCH list                    */
  
  AVOID_CANCEL_COND( SRCH_LST, UNTIL_ACQ_FAIL_DUR_CALL_ORIG )
                                /*   Remove avoids due to 100% PSIS */
  #else
  AVOID( ACQ_SYS, 60 )            /* Avoid the acquired sys for 60s */
  #endif
  DEL( OTASP_LST, IDL_SYS )       /* Delete idle sys from OTASP list */
  //#endif /* FEATURE_JCDMA */

  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  //ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when access attempt terminates due to access state
//** timeout
//*/
//static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_timeout[] = {
//
//  /* Condition */
//  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
//  IS_ACC_TERM( TIMEOUT )          /* Due to access state timeout */
//
//  /* Script */
//  AVOID( ACQ_SYS, 20 )            /* Avoid the acquired sys for 20s */
//  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is access state timeout.
** 2. The origination mode is OTASP or emergency.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_timeout_emerg[]={

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( TIMEOUT )          /* Due to access state timeout */
  IS_ORIG_MODE( OTASP_OR_EMERG )  /* Origination mode is OTASP or emergency*/

  /* Script */
  //AVOID( ACQ_SYS, 10 )            /* Avoid the acquired sys for 10s */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  DEL( OTASP_LST, IDL_SYS )       /* Delete idle sys from OTASP list */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is max access probes.
** 2. The origination mode is emergency.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map_emerg[]= {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_ORIG_MODE( EMERG )           /* Origination mode is emergency */

  /* Script */
  SCHED_COND( IDL_SYS, REACQ_EMERG_LOST, ANY ) /* Schedule reacq attempts */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is max access probes.
** 2. The origination mode is OTASP.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map_otasp[]= {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_ORIG_MODE( OTASP )           /* Origination mode is OTASP */

  /* Script */
  DEL( OTASP_LST, IDL_SYS )       /* Delete idle sys from OTASP list */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when access attempt terminates and:
//**
//** 1. The access termination reason is max access probes.
//** 2. Origination mode is ANY (i.e. user is trying to originate a call).
//** 3. The MS is CDMA Cellular and CDMA PCS capable (i.e. a trimode target).
//** 4. Silent redial with AMPS feature is turned OFF.
//*/
//static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map_trimode[]= {
//
//  /* Condition */
//  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
//  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
//  IS_ORIG_MODE( ANY )             /* User trying to originate a call */
//  IS( CAP_CDMA_CELL )             /* MS is CDMA Cellular capable */
//  IS( CAP_CDMA_PCS )              /* MS is CDMA Cellular capable */
//  IS_NOT( SRDA )                  /* Silent redial with AMPS is disabled */
//
//  /* Script */
//  PREV_SCR                        /* Continue with previous script */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is max access probes.
** 2. The idle system is a PRL listed system.
** 3. SRDA is enabled
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map_prl_srda[]={

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_PRL( IDL_SYS, LISTED )       /* Idle system is a PRL listed system */
  IS( SRDA )

  /* Script */
  SCHED( AMPS_BSIDL_SYS, REACQ_1_TIME, PREFERRED )/*Schedule reacq attempts*/
  AVOID_COND( ACQ_SYS, 600, UNTIL_ACQ_FAIL ) 
                                       /* Avoid the acq system till all
                                       ** systems in the idle geo have been
                                       ** attempted. */
  ACT_SCR( SYS_ACC_FAIL_PRL_AMPS )     /* Do PRL system access failure proc */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is max access probes.
** 2. The idle system is a PRL listed system.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map_prl[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_PRL( IDL_SYS, LISTED )       /* Idle system is a PRL listed system */

  /* Script */
  SCHED( AMPS_BSIDL_SYS, REACQ_1_TIME, PREFERRED )/*Schedule reacq attempts*/
  AVOID_COND( ACQ_SYS, 600, UNTIL_ACQ_FAIL ) 
                                       /* Avoid the acq system till all
                                       ** systems in the idle geo have been
                                       ** attempted. */
  ACT_SCR( SYS_ACC_FAIL_PRL )     /* Do PRL system access failure proc */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is max access probes.
** 2. SRDA is enabled
*/

static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map_srda[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS( SRDA )                        /* SRDA is ENABLED */

  /* Script */
  SCHED( AMPS_BSIDL_SYS, REACQ_1_TIME, ALLOWED )/* Schedule reacq attempts */
  AVOID_COND( ACQ_SYS, 600, UNTIL_ACQ_FAIL ) 
                                       /* Avoid the acq system till all
                                       ** systems in the idle geo have been
                                       ** attempted. */
  ACT_SCR( SYS_ACC_FAIL_AVAIL_AMPS )   /* Do avail system access failure proc */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is max access probes.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */

  /* Script */
  SCHED( AMPS_BSIDL_SYS, REACQ_1_TIME, ALLOWED )/* Schedule reacq attempts */
  AVOID_COND( ACQ_SYS, 600, UNTIL_ACQ_FAIL ) 
                                       /* Avoid the acq system till all
                                       ** systems in the idle geo have been
                                       ** attempted. */
  ACT_SCR( SYS_ACC_FAIL_AVAIL )   /* Do avail system access failure proc */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_CDMA_OPR_CAM_ERR
---------------------------------------------------------------------------*/

/* Script for when MS receives a channel assignment that is not
** supported by the MS while in emergency callback mode??.
*/
static const sdssscr_elem_u_type  ssscr_cdma_opr_cam_err_emerg_cb[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_CAM_ERR )    /* Cancel assignment not supported */
  IS_ORIG_MODE( EMERG_CB )        /* In emergency callback mode */

  /* Script */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives a channel assignment that is not
** supported by the MS.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_cam_err[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_CAM_ERR )    /* Cancel assignment not supported */

  /* Script */
  AVOID( ACQ_SYS, 60 )            /* Avoid the acquired system for 60s */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_CDMA_OPR_CAM
---------------------------------------------------------------------------*/

/* Script for when MS receives a channel assignment that is
** supported by the MS and:
**
** 1. MS is assigned to an AMPS system.
** 2. Origination mode is emergency origination.
*/
static const sdssscr_elem_u_type  ssscr_cdma_opr_cam_amps_sys_emerg[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_CAM )          /* Receive channel assignment */
  IS_HANDOFF( CDMA_CAM_TO_AMPS_SYS )/* Assigned to an AMPS system */
  IS_ORIG_MODE( EMERG_ORIG )        /* Orig mode is emergency origination */

  /* Script */
  ACQ_ALL( HO_SYS, ANY )            /* Attempt to acquire the handoff sys */
  ACT_SCR( SRV_LOST_SKIP )          /* Do skip-over service lost processing*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives a channel assignment that is
** supported by the MS and:
**
** 1. MS is assigned to an AMPS system.
*/
static const sdssscr_elem_u_type  ssscr_cdma_opr_cam_amps_sys[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_CAM )          /* Receive channel assignment */
  IS_HANDOFF( CDMA_CAM_TO_AMPS_SYS )/* Assigned to an AMPS system */

  /* Script */
  //CLR( SCHED_LST )                  /* Clear all existing reacq schedules */
  CPY( CAND_SYS, HO_SYS )           /* Set candidate sys per the HO_SYS */
  ACQ_ALL( HO_SYS, SPECIFIC_PRL )   /* Attempt to acquire the handoff sys */
  PROC_EVT( CDMA_OPR_SYS_LOST )     /* Handoff failed - reacq the idle sys */
  ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
  //ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives a channel assignment that is
** supported by the MS and:
**
** 1. MS is assigned to an AMPS voice channel.
*/
static const sdssscr_elem_u_type  ssscr_cdma_opr_cam_amps_voice[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_CAM )            /* Receive channel assignment */
  IS_HANDOFF( CDMA_CAM_TO_AMPS_VOICE )/* Assigned to an AMPS voice channel */

  /* Script */
  CLR( SCHED_LST )                /* Clear all existing reacq schedules */
  CONT( 30 )                      /* Give max 30s for handoff to complete */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Handoff failed - try to reacq idle sys*/
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives a channel assignment that is
** supported by the MS.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_cam[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_CAM )        /* Receive channel assignment */

  /* Script */
  CONT( 30 )                      /* Hold off reselection for 30s */
  HO_OFF                          /* Set handoff indication to OFF */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_CDMA_OPR_HDM
---------------------------------------------------------------------------*/

/* Script for when MS receives CDMA handoff direction.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_hdm[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_HDM )        /* Receive handoff direction */

  /* Script */
  //ACCEPT                          /* Accept handoff direction */
  CONT( 0 )                       /* Continue with handoff direction */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_CDMA_OPR_ITSPM
---------------------------------------------------------------------------*/

/* Script for when MS receives in traffic system parameters message (ITSPM).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_itspm[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ITSPM )      /* Receives in traffic system parameters */

  /* Script */
  CONT( 0 )                       /* Continue with CDMA traffic operation*/
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_OPR_RELEASE
---------------------------------------------------------------------------*/

/* Script for when Call is released and:
**
** 1. Redirection indication is ON.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_release_redir[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_RELEASE )    /* CDMA call is released */
  IS_REDIR( ANY )                 /* Redirection indication is ON */

  /* Script */
  RESEL_STT( RELEASE )            /* Set reselection state to RELEASE */
  HO_OFF                          /* Set handoff indication to off */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  CPY( CAND_SYS, IDL_SYS )        /* Set candidate sys per the IDL_SYS */
  CPY( TEMP_LST, SRV_SYS )         /* Set TEMP_LST to SRV_SYS */
  DEL( TEMP_LST, IDL_SYS )         /* Empty TEMP_LST if SRV_SYS == IDL_SYS */
  SCHED_COND( TEMP_LST, REACQ_1_TIME, ANY )
                                  /* Try to acquire the traffic channel */
  ACQ_ALL( IDL_SYS, SPECIFIC )    /* Try acquiring redirection's systems */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Acq -failed do system lost event proc */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when Call is released.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_release[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_RELEASE )    /* CDMA call is released */

  /* Script */
  RESEL_STT( RELEASE )            /* Set reselection state to RELEASE */
  HO_OFF                          /* Set handoff indication to off */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  CPY( CAND_SYS, IDL_SYS )        /* Set candidate sys per the IDL_SYS */
  CPY( TEMP_LST, SRV_SYS )         /* Set TEMP_LST to SRV_SYS */
  DEL( TEMP_LST, IDL_SYS )         /* Empty TEMP_LST if SRV_SYS == IDL_SYS */
  SCHED_COND( TEMP_LST, REACQ_1_TIME, ANY )
                                  /* Try to acquire the traffic channel */
  ACQ_ALL( IDL_SYS, ANY )         /* Try to reacquire the idle system */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Acq -failed do system lost event proc */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/* 
** Script for when ESPM received with MOB_P_REV < MIN_P_REV 
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_prot_mis[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_PROT_MIS )   /* CDMA protocol mismatch */

  /* Script */
  AVOID( SRV_SYS, 60 )            /* Avoid SRV_SYS for 60s */
  AVOID_COND( IDL_SYS, 60, UNTIL_ACQ_FAIL )
                                  /* Avoid IDL_SYS for 60s or ACQ_FAIL */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};
/*---------------------------------------------------------------------------
                     CDMA Operation Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with CDMA
** operation events.
*/
static const sdssscr_elem_u_type * const dsssscr_cdma_opr_event_tbl[] = {

                                      /* 0 */
  ssscr_cdma_opr_oh_sid_new_redir,    /* A new CDMA system is fully acquired
                                      ** and the SID/NID is new and
                                      ** redirection indication is on */

  ssscr_cdma_opr_oh_sid_conf_vrfy,    /* A new CDMA system is fully acquired
                                      ** and this system conflicts with the
                                      ** current SS-Preference and the
                                      ** SS-State is verify */


  ssscr_cdma_opr_oh_sid_frbd_vrfy,    /* A new CDMA system is fully acquired
                                      ** and this system is forbidden and
                                      ** origination mode is other than
                                      ** emergency and the SS-State is
                                      ** verify */

  ssscr_cdma_opr_oh_sid_neg_vrfy_redir,
                                      /* A new CDMA system is fully acquired
                                      ** and this system is negative,
                                      ** redirection is ON, origination mode
                                      ** is other than emergency and the
                                      ** SS-State is verify */

                                      /* 4 */
  ssscr_cdma_opr_oh_sid_conf_mode,    /* A new CDMA system is fully acquired
                                      ** and this system conflicts with the
                                      ** current mode preference */

  ssscr_cdma_opr_oh_sid_conf,         /* A new CDMA system is fully acquired
                                      ** and this system conflicts with the
                                      ** current SS-Preference */

  ssscr_cdma_opr_oh_sid_frbd,         /* A new CDMA system is fully acquired
                                      ** and this system is forbidden and
                                      ** origination mode is other than
                                      ** emergency */

  ssscr_cdma_opr_oh_sid_pref_new,     /* A new CDMA serving system is fully
                                      ** acquired, but there are more
                                      ** preferred systems in the serving
                                      ** system's GEO. The SID/NID of the new
                                      ** serving system is different from the
                                      ** previous SID/NID */

                                      /* 8 */
  ssscr_cdma_opr_oh_sid_pref,         /* A new CDMA serving system is fully
                                      ** acquired, but there are more
                                      ** preferred systems in the serving
                                      ** system's GEO */

  ssscr_cdma_opr_oh_sid_emerg_redir,  /* A new CDMA serving system is fully
                                      ** acquired and origination mode is
                                      ** emergency and redirection is ON */

  ssscr_cdma_opr_oh_sid_emerg,        /* A new CDMA serving system is fully
                                      ** acquired and origination mode is
                                      ** emergency */

  ssscr_cdma_opr_oh_sid_handoff,      /* A new CDMA serving system is fully
                                      ** acquired and handoff indication is
                                      ** ON */

                                      /* 12 */
  ssscr_cdma_opr_oh_sid_gsrdm_srdm_ndss,   /* A new CDMA serving system is fully
                                      ** acquired, but MS got to acquired
                                      ** system through a CDMA GSRM received
                                      ** on not most preferred system */

  ssscr_cdma_opr_oh_sid_pref_resel,   /* A new CDMA serving system is fully
                                      ** acquired, but there are more
                                      ** preferred systems in the serving
                                      ** system's GEO */

  ssscr_cdma_opr_oh_sid,              /* A CDMA system with a new SID/NID is
                                      ** fully acquired */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_opr_oh_chan,             /* A CDMA system with a new channel has
                                      ** been fully acquired. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                      /* 16 */
  ssscr_cdma_opr_rescan,              /* MS receives a SPM with the RESCAN
                                      ** field set to 1 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_opr_reg_rej_emerg,       /* CDMA registration is rejected while
                                      ** in emergency (origination or
                                      ** callback) mode */
                                      
  ssscr_cdma_opr_reg_rej_redir_rif,   /* CDMA registration is rejected while
                                      ** redirection indication is ON and
                                      ** RETURN_IF_FAILED = 1 */

  ssscr_cdma_opr_reg_rej_redir,       /* CDMA registration is rejected while
                                      ** redirection indication is ON */

                                      /* 20 */
  ssscr_cdma_opr_reg_rej,             /* Registration is rejected */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_opr_sys_lost_vrfy_sched, /* CDMA Serving system is lost during
                                      ** the VERIFY SS-State and acquired
                                      ** system is to be scheduled for
                                      ** reacquisition attempts */

  ssscr_cdma_opr_sys_lost_vrfy,       /* CDMA Serving system is lost during
                                      ** the VERIFY SS-State */

  ssscr_cdma_opr_sys_lost_otasp_emerg,/* CDMA Serving system is lost while in
                                      ** origination mode is OTASP or
                                      ** emergency */

                                      /* 24 */
  ssscr_cdma_opr_sys_lost_over_reach, /* CDMA Serving system is lost and MS
                                      ** is in over-reach (can not reach
                                      ** the BS) */

  ssscr_cdma_opr_sys_lost_dm_prl,     /* CDMA Serving system is lost and MS
                                      ** is a dual-mode target and lost
                                      ** system is from PRL */

                                      
  ssscr_cdma_opr_sys_lost_dm,         /* CDMA Serving system is lost and MS
                                      ** is a dual-mode target */

  ssscr_cdma_opr_sys_lost_orig,       /* CDMA Serving system is lost while
                                      ** user trying or originate a call */

                                      /* 28 */
  ssscr_cdma_opr_sys_lost,            /* CDMA Serving system is lost */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_opr_redir_invalid_gsrm_rif,/* MS receives invalid GSRM and
                                      ** RETURN_IF_FAILED = 1 */

  ssscr_cdma_opr_redir_invalid_gsrm,  /* MS receives invalid GSRM */

  ssscr_cdma_opr_redir_invalid,       /* MS receives invalid redirection
                                      ** message */

                                      /* 32 */
  ssscr_cdma_opr_redir_emerg,         /* MS redirected while in emergency
                                      ** origination mode */

  ssscr_cdma_opr_redir_rif,           /* MS receives redirection message and
                                      ** RETURN_IF_FAILED = 1 */

  ssscr_cdma_opr_redir,               /* MS receives redirection message */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_opr_ndss_off,            /* MS receives NDSS off indication */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                      /* 36 */
  ssscr_cdma_opr_new_sys,             /* MS idle-handoff to a BS with unknown
                                      ** configuration */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_opr_sys_resel_emerg_cb,  /* MS detects that pilot power is below
                                      ** a BS specified threshold and
                                      ** origination mode is emergency
                                      ** callback */

  ssscr_cdma_opr_sys_resel_prl,       /* MS detects that pilot power is below
                                      ** a BS specified threshold and idle
                                      ** system is a preferred PRL system */

  ssscr_cdma_opr_sys_resel,           /* MS detects that pilot power is below
                                      ** a BS specified threshold */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                      /* 40 */
  ssscr_cdma_opr_lock,                /* MS receives a lock order */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_opr_acc_term_traffic,    /* MS transitions from TC to idle
                                      ** without calling on release
                                      ** indication */

  #ifdef FEATURE_UNIFORM_SCAN_OOS

  ssscr_reserved_script,              /* Reserved script */

  ssscr_reserved_script,              /* Reserved script */

  #else

  ssscr_cdma_opr_acc_term_overreach_l,/* MS is in over-reach condition (i.e.
                                      ** can not reach the BS) for long
                                      ** time */

  ssscr_cdma_opr_acc_term_overreach,  /* MS is in over-reach condition (i.e.
                                      ** can not reach the BS) */
  #endif

                                      /* 44 */
  ssscr_cdma_opr_acc_term_emerg_cb,   /* Access attempt terminates and
                                      ** emergency callback mode is in
                                      ** effect */

  ssscr_cdma_opr_acc_term_acc_den,    /* Access attempt terminates due to
                                      ** access denied */

  ssscr_cdma_opr_acc_term_timeout_emerg, /* Access attempt terminates due to
                                      ** timeout and origination mode is
                                      ** emergency */

  ssscr_cdma_opr_acc_term_map_emerg,  /* Access attempt terminates due to
                                      ** max access probes and origination
                                      ** mode is emergency */

                                      /* 48 */
  ssscr_cdma_opr_acc_term_map_otasp,  /* Access attempt terminates due to
                                      ** max access probes and origination
                                      ** mode is OTASP */

  ssscr_cdma_opr_acc_term_map_prl_srda,
                                     /* Access attempt terminates due to
                                     ** max access probes and idle system is
                                     ** a listed PRL system,
                                     ** with SRDA ENABLED */

  ssscr_cdma_opr_acc_term_map_prl,    /* Access attempt terminates due to
                                      ** max access probes and idle system is
                                      ** a listed PRL system */

  ssscr_cdma_opr_acc_term_map_srda,   /* Access attempt terminates due to
                                      ** max access probes, and SRDA
                                      ** is ENABLED */

                                      /* 52 */
  ssscr_cdma_opr_acc_term_map,        /* Access attempt terminates due to
                                      ** max access probes */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_opr_cam_err_emerg_cb,    /* MS receives a channel assignment
                                      ** that is not supported by the MS
                                      ** while in emergency callback mode */

  ssscr_cdma_opr_cam_err,             /* MS receives a channel assignment
                                      ** that is not supported by the MS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_opr_cam_amps_sys_emerg,  /* MS receives a CDMA channel
                                      ** assignment to an AMPS system during
                                      ** emergency origination/callback */

                                      /* 56 */
  ssscr_cdma_opr_cam_amps_sys,        /* MS receives a CDMA channel
                                      ** assignment to an AMPS system */

  ssscr_cdma_opr_cam_amps_voice,      /* MS receives a CDMA channel
                                      ** assignment to an AMPS voice
                                      ** channel */

  ssscr_cdma_opr_cam,                 /* MS receives a channel assignment
                                      ** that is supported by the MS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_opr_hdm,                 /* MS receives a handoff direction
                                      ** message */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                      /* 60 */
  ssscr_cdma_opr_itspm,               /* MS receives an in traffic system
                                      ** parameters message with new SID/NID
                                      ** information */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_opr_release_redir,       /* Call is released and redirection is
                                      ** on */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_opr_release,             /* Call is released */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_cdma_opr_prot_mis             /* CDMA protocol mismatch */
};
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */



#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */




#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */


#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */


/* <EJECT> */
/*===========================================================================
=============================================================================
====================== SCRIPTS FOR MISCELLANEOUS EVENTS =====================
=============================================================================
===========================================================================*/


/*---------------------------------------------------------------------------
                         SDSS_EVT_MISC_TIMER_RESEL_OK
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                         SDSS_EVT_MISC_TIMER_RESEL_NO
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                           SDSS_EVT_MISC_MEAS_RPRT
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                          SDSS_EVT_MISC_ORIG_SUCCESS
---------------------------------------------------------------------------*/


/* Script for when SS-Client indicates that origination is successful.
*/
static const  sdssscr_elem_u_type   ssscr_misc_orig_success[] = {

  /* Condition */
  IS_EVENT( MISC_ORIG_SUCCESS )   /* Origination success indication */

  /* Script */
  CLR( SCHED_LST )                /* Cancel all reacquisition schedules */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_MISC_PWR_SAVE_ENTER
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                           SDSS_EVT_MISC_FILLER
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                          SDSS_EVT_MISC_GW_NET_LIST
---------------------------------------------------------------------------*/

/* Script for when SS-Client indicates that there is a GW_NET_LIST report.
*/

static const sdssscr_elem_u_type    ssscr_misc_gw_net_list[] = {

  /* Condition */
  IS_EVENT( MISC_GW_NET_LIST )   /* GET_NET_LIST report condition */

  /* Scripts */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy srch list from user list.*/
  ORDER( SRCH_LST, GW_LAST_USER_SYS )
                                  /* Order the list by last user system
                                  */
  GET_NET(SRCH_LST, GW_LAST_USER_SYS )
                                  /* Continue GW network list request until
                                  ** the report contains at least one PLMN
                                  ** list or SRCH_LST is exhaused. Update
                                  ** GW_LAST_USER_SYS with the system for
                                  ** which the PLMN list report contains
                                  ** at lease one PLMN list.
                                  */

  SRV_REQ_TYPE( SYNC_UP )         /* After get_networks, SD sends SYNC_UP */
  ACT_SCR( SRV_LOST_NORM_SLNT )   /* Do service lost processing once response
                                  ** is received.
                                  */
  END
};


/*---------------------------------------------------------------------------
                      Miscellaneous Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with
** miscellaneous events.
*/
static const sdssscr_elem_u_type * const dsssscr_misc_event_tbl[] = {

                                      /* 0 */
  ssscr_misc_orig_success,            /* Origination is successful */

  ssscr_misc_gw_net_list,             /* GW protocol reports Get_NET_LIST */

};


/* <EJECT> */
/*===========================================================================
=============================================================================
======================== SCRIPTS FOR INTERNAL EVENTS ========================
=============================================================================
===========================================================================*/


/*---------------------------------------------------------------------------
                          SDSS_EVT_INT_SRV_LOST
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script to handle case when service is lost in GW connected mode (DCH)
** and rem_uoos_sleep_time is non-zero
*/
static const  sdssscr_elem_u_type ssscr_int_srv_lost_gw_conn_mode_sleep_manual[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( MANUAL )          /* SD in manual mode. */
  IS_GW_SLEEP_TIME_SET            /* rem_uoos_sleep_time is greater than zero*/

  /* Script */
  SRV_STATUS( PWR_SAVE )          /* Update srv status */
  PWR_SAVE                        /* Goto PWR_SAVE*/
  CONT_GW_UOOS_REM_SLEEP          /* Sleep for uoos_rem_sleep_time */  

  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
  ORDER( MANUAL_LST, GW_LAST_USER_SYS )/* Order by last user system */
  CLR( GW_LAST_LTD_SYS )          /* Clear the last limited system */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  /* UE goes to Deep sleep, all other srv req should be USER_SELECTION.
  ** Since this is connected mode and the UE was already in deep sleep
  ** set the mode to USER_SELECTION
  */
  SRV_REQ_TYPE( USER_SELECTION )

  STACK_SYNC_UP /* Sync up the stacks */
  ACT_SCR( SRV_LOST_GW )          /* Do service lost scr  */
  ACT_SCR( PWR_SAVE_GW )
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when there is call origination while there is a service lost
** manual mode.
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_manual_call_orig[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( MANUAL_ORIG )     /* SD in manual orig mode. */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
  ORDER( MANUAL_LST, GW_LAST_USER_SYS )/* Order by last user system */
                          /* Copy the first user system to GW_LAST_USER_SYS*/
  CLR( GW_LAST_LTD_SYS )          /* Clear the last limited system */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_PWR_UP ) /* Is it power up? */
    CLR(PWR_SCAN_LST)                /* Clear the power scan list */  
    RESET_GW_ACQ_SCAN_STATUS
  ENDIF

  /* Manual mode power on and OOS for first time, 
  ** set the mode to NORMAL
  */
  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_GW_OPR_SYS_LOST )
    SRV_REQ_TYPE( NORMAL )
  ENDIF

  ACT_SCR( SRV_LOST_GW_ORIG )     /* Do service lost scr for call orig */
  /* UE goes to Deep sleep, all other 
  ** srv req should be USER_SELECTION  
  */
  SRV_REQ_TYPE( USER_SELECTION )
  ACT_SCR( PWR_SAVE_GW )          /* Nothing to acq - activate pwr-save scr*/

  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script to handle case when service is lost in GW connected mode (DCH)
** and rem_uoos_sleep_time is non-zero
*/
static const  sdssscr_elem_u_type ssscr_int_srv_lost_gw_conn_mode_sleep[] = {
  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_GW_SLEEP_TIME_SET            /* rem_uoos_sleep_time is greater than zero*/

  SRV_STATUS( PWR_SAVE )          /* Update srv status */
  PWR_SAVE                        /* Goto PWR_SAVE*/
  CONT_GW_UOOS_REM_SLEEP          /* Sleep for uoos_rem_sleep_time */  

  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GW_LST )
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  ACT_SCR( SRV_LOST_GW )          /* Do service lost scr  */
  ACT_SCR( PWR_SAVE_GW )
  END
};


/* Script to handle case when service is lost in GW connected mode (DCH)
** and rem_uoos_awake_time is non-zero
*/
static const  sdssscr_elem_u_type ssscr_int_srv_lost_gw_conn_mode_awake[] = {
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_GW_AWAKE_TIME_SET            /* rem_uoos_awake_time is greater than zero*/
  /* Replace this with IS UOOS timer set
  */

  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GW_LST )
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  /* Add a If check, if UOOS scan is set
  */
  ACT_SCR( SRV_LOST_GW )          /* Do service lost scr  */
  /*endif*/

  ACT_SCR( PWR_SAVE_GW )
  END
};

/* Script to handle case when service is lost in GW connected mode (DCH)
** and rem_uoos_awake_time is non-zero
*/
static const  sdssscr_elem_u_type ssscr_int_srv_lost_gw_conn_mode_awake_manual[] = {
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( MANUAL )          /* SD in manual mode. */
  IS_GW_AWAKE_TIME_SET            /* rem_uoos_awake_time is greater than zero*/

  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
  ORDER( MANUAL_LST, GW_LAST_USER_SYS )/* Order by last user system */
  CLR( GW_LAST_LTD_SYS )          /* Clear the last limited system */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_REQ_TYPE( NORMAL )         /* Set the serivce_req_type to NORMAL */ 
  ACT_SCR( SRV_LOST_GW )          /* Do service lost scr  */
  SRV_REQ_TYPE( USER_SELECTION )
  ACT_SCR( PWR_SAVE_GW )
  END
};

/* Script for when service is declared lost while in manual mode.
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_manual[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( MANUAL )          /* SD in manual mode. */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
  ORDER( MANUAL_LST, GW_LAST_USER_SYS )/* Order by last user system */

  CLR( GW_LAST_LTD_SYS )          /* Clear the last limited system */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_PWR_UP ) /* Is it power up? */
    CLR(PWR_SCAN_LST)                /* Clear the power scan list */  
    RESET_GW_ACQ_SCAN_STATUS
    SRV_REQ_TYPE( NORMAL )
  ENDIF

  /* Manual mode power on and OOS */
  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_GW_OPR_SYS_LOST )
    SRV_REQ_TYPE( NORMAL )
  ENDIF

  IF( SS_PREF_CHGD, EQUAL_TO, TRUE ) /* If ss preferences have changed, clear
                                     ** the ACQ_SCAN_STATUS */
    RESET_GW_ACQ_SCAN_STATUS         /* Clear the ACQ_SCAN_STATUS */
  ENDIF

  IF( STACK_SYNC_UP, EQUAL_TO, TRUE ) /* If required, sync up stacks */
      STACK_SYNC_UP
  ENDIF

  ACT_SCR( SRV_LOST_GW )          /* Do ervice lost scr  */
  /* UE goes to deep sleep, 
  ** all other srv req should be USER_RESELECTION
  */ 
  SRV_REQ_TYPE( USER_SELECTION )
  ACT_SCR( PWR_SAVE_GW )          /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when service is declared lost while trying to
** 1. In originate state
** 2. GW only target
** 3. IDL SYS is empty
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_emerg_call_orig_gw_pwrup[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG )             /* Orig mode is ANY */
  IS_PREF_REAS( CALL_ORIG )
  IS( CAP_GW_PREF_GW_ONLY )              /* Capable for GSM/WCDMA but not 1x */
  IS_CNT( IDL_SYS, 0, EXACTLY, ALL) /* IDL_SYS is empty */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GW_LST )
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_PWR_UP ) /* Is it power up? */
    #if ( defined ( FEATURE_UMTS_UNIFORM_OOS_HANDLING_PWR_UP_ENH ) )
#error code not present
    #endif
    RESET_GW_ACQ_SCAN_STATUS
  ENDIF

  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  ACT_SCR( SRV_LOST_GW_ORIG_PWRUP )
  #endif
  ACT_SCR( PWR_SAVE_GW_PWRUP )    /* Nothing to acq - activate pwr-save scr*/

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** 1. In originate state
** 2. GW only target
** 3. IDL SYS is empty
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_emerg_orig_gw_pwrup[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG )             /* Orig mode is ANY */
  IS( CAP_GW_PREF_GW_ONLY )              /* Capable for GSM/WCDMA but not 1x */
  IS_CNT( IDL_SYS, 0, EXACTLY, ALL) /* IDL_SYS is empty */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GW_LST )
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_PWR_UP ) /* Is it power up? */
    #if ( defined ( FEATURE_UMTS_UNIFORM_OOS_HANDLING_PWR_UP_ENH ) )
#error code not present
    #endif
    RESET_GW_ACQ_SCAN_STATUS
  ENDIF

  IF( SS_PREF_CHGD, EQUAL_TO, TRUE ) /* If ss preferences have changed, clear
                                     ** the ACQ_SCAN_STATUS */
      RESET_GW_ACQ_SCAN_STATUS         /* Clear the ACQ_SCAN_STATUS */
  ENDIF

  IF( STACK_SYNC_UP, EQUAL_TO, TRUE )  /* If required, sync up stacks */
      STACK_SYNC_UP
  ENDIF

  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  ACT_SCR( SRV_LOST_GW_ORIG_PWRUP )
  #endif

  ACT_SCR( PWR_SAVE_GW_PWRUP )    /* Nothing to acq - activate pwr-save scr*/

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** 1. In originate state
** 2. GW only target
** 3. IDL SYS is empty
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_orig_gw_pwrup[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( ANY )             /* Orig mode is ANY */
  IS( CAP_GW_PREF_GW_ONLY )              /* Capable for GSM/WCDMA but not 1x */
  IS_CNT( IDL_SYS, 0, EXACTLY, ALL) /* IDL_SYS is empty */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GW_LST )
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/

  ACT_SCR( SRV_LOST_GW_ORIG_PWRUP )
                          /* Do service lost scr for call orig in power up */
  ACT_SCR( PWR_SAVE_GW_PWRUP )    /* Nothing to acq - activate pwr-save scr*/

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** 1. In originate state
** 2. GW only target
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_emerg_call_orig_gw[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG )             /* Orig mode is ANY */
  IS_PREF_REAS( CALL_ORIG )
  IS( CAP_GW_PREF_GW_ONLY )              /* Capable for GSM/WCDMA but not 1x */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GW_LST )
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/
  ORDER2( SRCH_LST, IDL_SYS, BAND )/* Ordering by IDL_SYS according to BAND*/

  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  ACT_SCR( SRV_LOST_GW_ORIG )
  #endif
  ACT_SCR( PWR_SAVE_GW )          /* Nothing to acq - activate pwr-save scr*/

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** 1. In originate state
** 2. GW only target
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_emerg_orig_gw[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG )             /* Orig mode is ANY */
  IS( CAP_GW_PREF_GW_ONLY )              /* Capable for GSM/WCDMA but not 1x */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GW_LST )
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/
  ORDER2( SRCH_LST, IDL_SYS, BAND )/* Ordering by IDL_SYS according to BAND*/

  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  ACT_SCR( SRV_LOST_GW_ORIG )
  #endif
  
  ACT_SCR( PWR_SAVE_GW )          /* Nothing to acq - activate pwr-save scr*/

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** 1. In originate state
** 2. GW only target
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_orig_gw[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( ANY )             /* Orig mode is ANY */
  IS( CAP_GW_PREF_GW_ONLY )              /* Capable for GSM/WCDMA but not 1x */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  
  ACT_SCR( CHK_AND_CLR_ACQ_SYS )  /* Clear the acquisition systems, 
                                  ** if required                          */
                                  
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GW_LST )
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/
  ORDER2( SRCH_LST, IDL_SYS, BAND )/* Ordering by IDL_SYS according to BAND*/

  ACT_SCR( SRV_LOST_GW_ORIG )      /* Do service lost scr for call orig */
  ACT_SCR( PWR_SAVE_GW )          /* Nothing to acq - activate pwr-save scr*/

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** originate a normal call (i.e., call other than emergency or OTASP).
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_hdr_cdma_ho[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_HANDOFF( HDR_ANY_TO_CDMA_ANY ) /* Handoff from HDR to CDMA */

  /* Script */
  CLR( CAND_SYS )                 /* Clear the candidate system           */
  CLR( AVAIL_LST )                /* Clear the available list             */
  CLR( START_PREF_LST )           /* Clear the best pref acq list         */
  CLR( ACQ_FULL_SYS )             /* Clear last FULL acquisition system   */
  CLR( ACQ_FULL2_SYS )            /* Clear last FULL2 acquisition system  */
  CLR( ACQ_MICRO_SYS )            /* Clear last MICRO acquisition system  */
  CLR( ACQ_MICRO2_SYS )            /* Clear last MICRO acquisition system  */

  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /* Acq failed - clear COLLOC list       */
  #endif /* FEATURE_HDR */

  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list      */
  APP( SRCH_LST, MRU_LST )        /* Order SRCH list by MRU list          */
  APP( SRCH_LST, PRL_LST )

  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list by MRU list          */

  #ifdef FEATURE_AVAIL_HOME_SIDE_PREF
  SORT( AVAIL_LST, CDMA )         /* Sort AVAIL_LST according to CDMA 1st */
  SORT( AVAIL_LST, CELL_HOME )    /* Sort AVAIL_LST according to Home 1st */
  #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

  CPY( TEMP_SYS, MRU_LST )
  SCHED_COND2( TEMP_SYS, REACQ_EVERY_1S, ALLOWED, \
               NOT_ON_EQUAL_OR_HIGHER_PRIORITY_SCHED_OR_AVOID_OR_FORBID )

  ACQ_MODE( HO )                /* Do full acquisitions                 */
  ACQ_CNT( SRCH_LST, ANY, 0, 1 )
  ACQ_FAIL

  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** originate a normal call (i.e., call other than emergency or OTASP).
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_normal_orig[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( NORMAL )          /* Trying to originate a normal call */
  NOT IS( FEATURE_UOOS ) /* Doing Silent re-dials */

  /* Script */
  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /*   4. Acq failed - clear COLLOC list */
  #endif /* FEATURE_HDR */

  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU list */ 
  #ifndef FEATURE_UNIFORM_SCAN_OOS
  ACQ_MODE( MINI )                /* Try mini acq over GEO, MRU and PRL */
  ACQ( SRCH_LST, ALLOWED )        /*   1. Try acquiring allowed GEO sys */
  ACQ( MRU_LST, ALLOWED )         /*   2. Try acquiring allowed MRU sys */
  ACQ( PRL_LST, ALLOWED )         /*   4. Try acquiring allowed PRL sys*/
  ACQ( GW_LST, ALLOWED )          /*   5. Try acquiring allowed PRL sys */
  #endif

  LOOP_FOREVER                    /* Try full acq over GEO, MRU and PRL */
    SRV_STATUS( NO_SRV )          /* Set service status to NO_SRV  */
    ACQ_MODE( FULL )              /*   2. Do full acquisitions */
    NEW_ACQ                       /*   3. Start a new acquisition */
    ACQ( SRCH_LST, ALLOWED )      /*   4. Try acquiring allowed GEO sys */
    ACQ( MRU_LST, ALLOWED )       /*   5. Try acquiring allowed MRU sys */
    ACQ( PRL_LST, ALLOWED )       /*   6. Try acquiring allowed PRL sys */
    ACQ( GW_LST, ALLOWED )        /*   7. Try acquiring allowed PRL sys */
    ACQ_FAIL                      /*   8. Indicate that acquisition failed */
  LOOP                            /* Loop back */

  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#if (defined(FEATURE_UNIFORM_SCAN_OOS) && defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING))
#error code not present
#endif

#ifdef FEATURE_UNIFORM_SCAN_OOS

static const  sdssscr_elem_u_type   ssscr_int_srv_lost_normal_orig_uoos_no_sr_hdr_only[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( NORMAL )          /* Trying to originate a normal call */
  IS( FEATURE_UOOS )              /* UOOS turned on                       */
  IS( CAP_HDR_PREF_HDR_ONLY )     /* HDR only                             */

  /* Script */
  CLR( CAND_SYS )                 /* Clear the candidate system           */
  CLR( AVAIL_LST )                /* Clear the available list             */
  CLR( START_PREF_LST )           /* Clear the best pref acq list         */
  ACT_SCR( CHK_AND_CLR_ACQ_SYS )  /* Clear the acquisition systems, 
                                  ** if required                          */

  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list      */
  APP( SRCH_LST, MRU_LST )        /* Order SRCH list by MRU list          */
  APP( SRCH_LST, PRL_LST )

  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list by MRU list          */
  
  #ifdef FEATURE_AVAIL_HOME_SIDE_PREF
  SORT( AVAIL_LST, CDMA )         /* Sort AVAIL_LST according to CDMA 1st */
  SORT( AVAIL_LST, CELL_HOME )    /* Sort AVAIL_LST according to Home 1st */
  #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

  START_TIME

  ACQ_MODE( FULL )                /* Do full acquisitions                 */
  ACQ_CNT( SRCH_LST, ALLOWED, 0, 2 )

  ACQ_MODE( DEEP2 )               /* Do full2 acquisitions                */
  ACQ_CNT( SRCH_LST, ALLOWED, SDSSSCR_DEEP2_SYS, 2 )
                                  /* Try acq FULL2+2 systems              */

  ACQ_MODE( MICRO )               /* Do micro acquisitions                */

  IF( USER_HDR_GW_MODE_PREF_AND_CURR_NOT_HDR_GW, EQUAL_TO, TRUE )
    ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_MICRO_SYS, TMR_AUTO_UOOS_SCAN )
  ELSE
    ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_MICRO_SYS, TMR_UOOS_SCAN )
  ENDIF


  LOOP_FOREVER                    /* Loop forever                         */
    NEW_ACQ
    ACQ_MODE( DEEP2 )             /* Do time based acquisition            */

    IF( USER_HDR_GW_MODE_PREF_AND_CURR_NOT_HDR_GW, EQUAL_TO, TRUE )
      ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_DEEP2_SYS, TMR_AUTO_UOOS_SCAN )
    ELSE
      ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_DEEP2_SYS, TMR_UOOS_SCAN )
    ENDIF

  LOOP

  ACQ_FAIL                        /* Report acq_fail                      */
  ACT_SCR( PWR_SAVE_HDR )         /* Nothing to acq -activate pwr-save scr*/
  END
};


/* Script for when service is declared lost.
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_normal_orig_uoos_no_sr_non_hybr_digital_mode[] = {

  IS_EVENT( INT_SRV_LOST )        /* Internal system lost event           */
  IS_ORIG_MODE( NORMAL )          /* Trying to originate a normal call    */
  IS( NO_HYBR_OPR )               /* Not in hybrid operation              */
  IS( CAP_CDMA_HDR_PREF_CDMA_HDR_INCL ) 
                                  /* Mode has both CDMA and HDR           */
  IS( FEATURE_UOOS )              /* UOOS is defined                      */

  /* Script */
  CLR( CAND_SYS )                 /* Clear the candidate system           */
  CLR( AVAIL_LST )                /* Clear the available list             */
  CLR( START_PREF_LST )           /* Clear the best pref acq list         */
  ACT_SCR( CHK_AND_CLR_ACQ_SYS )  /* Clear the acquisition systems, 
                                  ** if required                          */
  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /* Acq failed - clear COLLOC list       */
  #endif /* FEATURE_HDR */

  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list      */
  APP (SRCH_LST, FORBID_GEO_LST ) /* Copy forbidden GEO list to SRCH list */
  APP( SRCH_LST, MRU_LST )        /* Append the MRU list to SRCH list     */
  APP( SRCH_LST, PRL_LST )        /* Append the PRL list to SRCH list     */

  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list by MRU list          */

  #ifdef FEATURE_AVAIL_HOME_SIDE_PREF
  SORT( AVAIL_LST, CDMA )         /* Sort AVAIL_LST according to CDMA 1st */
  SORT( AVAIL_LST, CELL_HOME )    /* Sort AVAIL_LST according to Home 1st */
  #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

  CPY( TEMP_LST, SRCH_LST)        /* Schedule a non-HDR MRU channed       */
  DEL_SYS( TEMP_LST, NON_CDMA )/* Remove HDR from the temp list        */
  CPY( TEMP_SYS, TEMP_LST )

  SCHED_COND2( TEMP_SYS, REACQ_EVERY_1S, BEST_PREF, \
               NOT_ON_EQUAL_OR_HIGHER_PRIORITY_SCHED_OR_AVOID_OR_FORBID )

  START_TIME                      /* Start time for the time based acq    */

  ACQ_MODE( FULL )                /* Do full acquisitions                 */
  ACQ_CNT_OR_TIME( SRCH_LST, ALLOWED, 0, 4, TMR_HALF_UOOS_SCAN )
                                  /* Acquire till time or count           */

  ACQ_MODE( FULL2 )               /* Do full2 acquisitions                */

  ACQ_CNT_OR_TIME( SRCH_LST, ALLOWED, SDSSSCR_FULL2_SYS, 2, TMR_HALF_UOOS_SCAN ) 
                                  /* Acquire till time or count           */

  ACQ_MODE( MICRO2 )              /* Do micro acquisitions                */
  ACQ_TIME2( SRCH_LST, ALLOWED, SDSSSCR_MICRO2_SYS, TMR_UOOS_SCAN )

  LOOP_FOREVER                    /* Continue with time based acquisitions*/
    NEW_ACQ
    ACQ_MODE( FULL2 )
    ACQ_TIME2( SRCH_LST, ALLOWED, SDSSSCR_FULL2_SYS, TMR_UOOS_SCAN )
  LOOP

  ACQ( GW_LST, ALLOWED )          /* Try acq GSM/WCDMA systems            */
  ACQ( GW_LTD_LST, ANY )          /* Try acquiring limited GW service     */

  ACQ_FAIL                        /* Report ACQ_FAIL                      */
  ACT_SCR( PWR_SAVE_NON_HYBR_DIGITAL_MODE )
                                   /* Nothing to acq -activate pwr-save scr*/
  END
};


/* Script for when service is declared lost while trying to
** originate a normal call (i.e., call other than emergency or OTASP) and
** not doing silent re-dials
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_normal_orig_uoos_no_sr[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( NORMAL )          /* Trying to originate a normal call */
  IS( FEATURE_UOOS )

  /* Script */
  CLR( CAND_SYS )                 /* Clear the candidate system           */
  CLR( AVAIL_LST )                /* Clear the available list             */
  CLR( START_PREF_LST )           /* Clear the best pref acq list         */
  ACT_SCR( CHK_AND_CLR_ACQ_SYS )  /* Clear the acquisition systems, 
                                  ** if required                          */

  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /* Acq failed - clear COLLOC list       */
  #endif /* FEATURE_HDR */

  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list      */
  APP( SRCH_LST, FORBID_GEO_LST ) /* Copy forbidden GEO list to SRCH list */
  APP( SRCH_LST, MRU_LST )        /* Order SRCH list by MRU list          */
  APP( SRCH_LST, PRL_LST )

  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list by MRU list          */
  
  #ifdef FEATURE_AVAIL_HOME_SIDE_PREF
  SORT( AVAIL_LST, CDMA )         /* Sort AVAIL_LST according to CDMA 1st */
  SORT( AVAIL_LST, CELL_HOME )    /* Sort AVAIL_LST according to Home 1st */
  #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

  CPY( TEMP_LST, SRCH_LST)
  DEL_SYS( TEMP_LST, NON_CDMA )/* Remove HDR from the temp list        */
  CPY( TEMP_SYS, TEMP_LST )

  SCHED_COND2( TEMP_SYS, REACQ_EVERY_1S, ALLOWED, \
               NOT_ON_EQUAL_OR_HIGHER_PRIORITY_SCHED_OR_AVOID_OR_FORBID )
  START_TIME

  ACQ_MODE( FULL )                /* Do full acquisitions                 */
  ACQ_CNT( SRCH_LST, ALLOWED, 0, 6 )

  ACQ_MODE( FULL2 )               /* Do full2 acquisitions                */
  ACQ_CNT( SRCH_LST, ALLOWED, SDSSSCR_FULL2_SYS, 2 )
                                  /* Try acq FULL2+2 systems              */

  ACQ_MODE( MICRO2 )              /* Do micro acquisitions                */

  IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
                                  /* If the build supports GW and the 
                                  ** user has set the mode to 1X + GSM    */
    ACQ_TIME2( SRCH_LST, ALLOWED, SDSSSCR_MICRO2_SYS, TMR_AUTO_UOOS_SCAN )
  ELSE
    ACQ_TIME2( SRCH_LST, ALLOWED, SDSSSCR_MICRO2_SYS, TMR_UOOS_SCAN )
  ENDIF

  LOOP_FOREVER                    /* Loop forever                         */
    NEW_ACQ
    ACQ_MODE( FULL2 )             /* Do time based acquisition            */
    IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
      ACQ_TIME2( SRCH_LST, ALLOWED, SDSSSCR_FULL2_SYS, TMR_AUTO_UOOS_SCAN )
    ELSE
      ACQ_TIME2( SRCH_LST, ALLOWED, SDSSSCR_FULL2_SYS, TMR_UOOS_SCAN )
    ENDIF

  LOOP

  ACQ( GW_LST, ALLOWED )          /* Try acq GSM/WCDMA systems            */
  ACQ( GW_LTD_LST, ANY )          /* Try acquiring limited GW service     */

  ACQ_FAIL                        /* Report acq_fail                      */
  ACT_SCR( PWR_SAVE )             /* Nothing to acq -activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost and:
**
** 1. Origination mode is collocated system.
** 2. there is a call origination.
** 3. Main has acquired service
*/

static const  sdssscr_elem_u_type   ssscr_int_srv_lost_colloc_call_orig_no_1x_srv[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost                      */
  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */
  IS_PREF_REAS( CALL_ORIG )       /* During the call origination          */
  IS_SRV_STATUS2( MAIN, NO_SRV_ANY ) 
                                  /* No service on MAIN                   */
  IS( FEATURE_UNIFORM_SCAN_OOS )  /* UOOS defined                         */

  /* Script */
  
  ACT_SCR( UNIFORM_COLLOC_CALL_ORIG_NO_1X_SRV )
                                   /* Activate the call orig script       */
  END
};

#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when service is declared lost and:
//**
//** 1. Origination mode is collocated system.
//** 2. No system on the collocated list complies with the user preferences.
//*/
//static const sdssscr_elem_u_type  ssscr_int_srv_lost_colloc_empty[] = {
//
//  /* Condition */
//  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
//  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */
//  IS_CNT( SRV_COLLOC_LST, 0, EXACTLY, COMPLY )
//                                  /* No sys on colloc list comply with pref*/
//  /* Script */
//  SRV_OFF                           /* Set service indication to OFF */
//  PWR_SAVE                          /* Tell SS-Client to enter power-save */
//  CONT( 0 )                         /* Continue with power-save operation */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when service is declared lost and:
//**
//** 1. Origination mode is collocated system.
//** 2. Collocated list contain 1 or less HDR systems.
//*/
//static const  sdssscr_elem_u_type   ssscr_int_srv_lost_colloc_orig_1sys[] = {

//  /* Condition */
//  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
//  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */
//  IS_CNT( SRV_COLLOC_LST, 1, OR_LESS, COMPLY ) /* Colloc list cnt <= 1 */
//
//  LOOP_NUM( 1 )                   /* Loop 1 times and: */
//    ACQ_MODE( FULL )              /*   1. Set acquisition mode to full */
//    ACQ( SRV_COLLOC_LST, COLLOC_PREF )
//                                  /*   2. Try acq allowed collocated sys */
//    NEW_ACQ                       /*   5. Start a new acquisition */
//    SRV_STATUS( NO_SRV )          /*   6. Set service status to NO_SRV  */
//    ACQ_FAIL                      /*   7. Indicate that acquisition failed */
//  LOOP                            /* Loop back */
//
//  ACT_SCR( PWR_SAVE_COLLOC_1SYS ) /* Nothing to acq - activate pwr-save scr*/
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost and:
**
** 1. Origination mode is collocated system.
** 2. there is a call origination.
** 3. Main doesnot have service (No need to check)
*/

static const  sdssscr_elem_u_type   ssscr_int_srv_lost_colloc_call_orig[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */
  IS_PREF_REAS( CALL_ORIG )       /* During the call origination */

  /* Script */
  ACT_SCR( HDR_ACQ_ROTATE_SRV_LOST ) /* Activate the HDR rotation script */
  ACT_SCR( PWR_SAVE_COLLOC )         /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef FEATURE_UNIFORM_SCAN_OOS


/* Script for when service is declared lost and:
**
** 1. Origination mode is collocated system.
** 2. Main is doing acquisitions
** 3. UOOS is defined
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_colloc_orig_main_in_acq[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */
  NOT IS_ACQ_STATE2( MAIN, NONE ) /* DO Doing wakeup acquisitions. */
  IS_SRV_STATUS2( MAIN, NO_SRV_ANY ) /* No service on Main */
  IS( FEATURE_UNIFORM_SCAN_OOS )  /* UOOS is defined */

  /* Script */
  SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
  PWR_SAVE                        /* Tell SS-Client to enter power-save */
  CONT( 0 )                       /* Continue */

  END
};


/* Script for when service is declared lost and:
**
** 1. Origination mode is collocated system.
** 2. Main has acquired service
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_colloc_orig_uoos_scan[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */
  IS_SRV_STATUS2( MAIN, PWR_SAVE ) /* Is service on main */
  IS( FEATURE_UNIFORM_SCAN_OOS )

  /* Script */

  ACQ_MODE( DEEP )              /*   1. Set acquisition mode to full */
  CPY( SRCH_LST, SRV_COLLOC_LST)/*   2. Copy COLLOC list into SRCH list */
  ORDER( SRCH_LST, MRU_LST )    /*   3. Order SRCH list according to MRU */

  START_TIME                    /* Start the time for time based scans */

  ACQ_CNT( SRCH_LST, COLLOC_BEST_PREF, 0, 1 )
                                /*   4. Try acq best pref collocated sys */
  ACQ_MODE( DEEP2 )
  ACQ_CNT( SRCH_LST, COLLOC_BEST_PREF, SDSSSCR_DEEP2_SYS, 1 )

  #ifdef FEATURE_UNIFORM_SCAN_OOS_HDR_ENH

  ACQ_MODE( MICRO2 )
  ACQ_TIME2( SRCH_LST, COLLOC_BEST_PREF, SDSSSCR_MICRO2_SYS, TMR_UOOS_HDR_SCAN )

  LOOP_FOREVER                    /* Continue with time based acquisitions*/
  NEW_ACQ
  ACQ_MODE( DEEP2 )
  ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_DEEP2_SYS, TMR_UOOS_HDR_SCAN )
  LOOP

  #endif

  ACQ_FAIL
  NEW_ACQ                       /*   5. Start a new acquisition */

  REDIR_OFF                       /* Set redirection indication to off */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acquisition related lists */

  SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
  PWR_SAVE                        /* Tell SS-Client to enter power-save */

  CONT( 0 )
  END
};



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when service is declared lost and:
**
** 1. Origination mode is collocated system.
** 2. Main service status is no service
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_colloc_orig_no_1x_srv[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */
  IS_SRV_STATUS2( MAIN, NO_SRV )  /* Main SRV status is NO_SRV, NOT NO_SRV_ANY */
  IS( FEATURE_UNIFORM_SCAN_OOS )  /* UOOS is defined */

  /* Script */
  SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
  PWR_SAVE                        /* Tell SS-Client to enter power-save */

  CONT( 0 )                       /* Continue */

  END
};

#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost and:
**
** 1. Origination mode is collocated system.
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_colloc_orig[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */

  /* Script */
  LOOP_NUM( 1 )                   /* Loop 1 times and: */
    ACQ_MODE( FULL )              /*   1. Set acquisition mode to full */
    CPY( SRCH_LST, SRV_COLLOC_LST)/*   2. Copy COLLOC list into SRCH list */
    ORDER( SRCH_LST, MRU_LST )    /*   3. Order SRCH list according to MRU */
    ACQ( SRCH_LST, COLLOC_BEST_PREF )
                                  /*   4. Try acq best pref collocated sys */
    NEW_ACQ                       /*   5. Start a new acquisition */
    SRV_STATUS( NO_SRV )          /*   6. Set service status to NO_SRV  */
    ACQ_FAIL                      /*   7. Indicate that acquisition failed */
  LOOP                            /* Loop back */

      ACT_SCR( PWR_SAVE_COLLOC )      /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** originate an OTASP call.
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_otasp_orig[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( OTASP )           /* Trying to originate an OTASP call */

  /* Script */
  ORDER( OTASP_LST, IDL_GEO_LST ) /* Order OTASP systems according to GEO */
  ORDER( OTASP_LST, MRU_LST )     /* Order OTASP systems according to MRU */

  #ifndef FEATURE_UNIFORM_SCAN_OOS

  ACQ_MODE( MINI )                /* First try mini acquisitions */
  ACQ( OTASP_LST, ANY )           /*   1. Try acquiring any OTASP systems */

  LOOP_FOREVER                    /* Mini acq failed - loop forever and: */
    SRV_STATUS( NO_SRV )          /*   1. Set service status to NO_SRV  */
    ACQ_MODE( FULL )              /*   2. Do full acquisitions */
    NEW_ACQ                       /*   3. Start a new acquisition */
    ACQ( OTASP_LST, ANY )         /*   4. Try acquiring any OTASP systems */
    ACQ_FAIL                      /*   5. Indicate that acquisition failed */
  LOOP                            /* Loop back */

  #else

  START_TIME

  ACQ_MODE( FULL )                /* Do full acquisitions                 */
  ACQ_CNT( OTASP_LST, ANY, 0, 8 )

  ACQ_MODE( MICRO )               /* Do micro acquisitions                */

  IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
    ACQ_TIME2( OTASP_LST, ANY, 0, TMR_AUTO_UOOS_SCAN )
  ELSE
    ACQ_TIME2( OTASP_LST, ANY, 0, TMR_UOOS_SCAN )
  ENDIF

  LOOP_FOREVER
    NEW_ACQ
    ACQ_MODE( FULL )              /* Continue with TIME based acquisitions */
    IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
      ACQ_TIME2( OTASP_LST, ANY, 0, TMR_AUTO_UOOS_SCAN )
    ELSE
      ACQ_TIME2( OTASP_LST, ANY, 0, TMR_UOOS_SCAN )
    ENDIF
  LOOP

  ACQ_FAIL
  #endif
  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to originate an
** emergency call and:
**
** 1. MS is CDMA locked until power-cycle.
** 2. MS is AMPS capable.
*/
static const sdssscr_elem_u_type  ssscr_int_srv_lost_emerg_orig_lock[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG_ORIG )      /* Trying to originate an emergency call */
  IS( CDMA_LOCKED )               /* MS is CDMA locked until power cycle */
  IS( CAP_AMPS )                  /* MS is AMPS capable */

  /* Script */
  CLR( SRCH_LST )                 /* Clear the search list */
  APP( SRCH_LST, AMPS_A_SYS )     /* Append AMPS A to the search list */
  APP( SRCH_LST, AMPS_B_SYS )     /* Append AMPS B to the search list */
  APP( SRCH_LST, GW_LST )         /* Append GW to the search list */
  ORDER( SRCH_LST, PRL_LST )      /* Order AMPS systems according to PRL */
  ORDER( SRCH_LST, IDL_GEO_LST )  /* Order AMPS systems according to GEO */

  LOOP_FOREVER                    /* Loop forever and: */
    SRV_STATUS( NO_SRV )          /*   Set service status to NO_SRV  */
    CLR( SCHED_LST )              /*   Cancel all reacq schedules */
    DEL_SYS( SRCH_LST, HDR )      /*   Delete HDR systems. */
    AVOID_CANCEL( SRCH_LST )      /*   Cancel acq avoid for AMPS systems*/
    ACQ_ALL( SRCH_LST, ANY )      /*   Attempt to acquire AMPS systems */
    ACQ_FAIL                      /*   Indicate that acquisition failed */
  LOOP                            /* Loop back */

  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** originate an emergency call.
*/
static const sdssscr_elem_u_type  ssscr_int_srv_lost_emerg_orig[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG_ORIG )      /* Trying to originate an emergency call */

  /* Script */
  LOOP_FOREVER                    /* Mini acq failed - loop forever and: */
    SRV_STATUS( NO_SRV )          /*   1. Set service status to NO_SRV  */
    CPY( TEMP_LST, TBL_LST )
    DEL_SYS( TEMP_LST, HDR )
    AVOID_CANCEL( TEMP_LST )       /*   2. Cancel acq avoid for all systems */
    ACQ_CNT( EMERG_LST, ANY, SDSSSCR_EMERG_SYS, CNT_ALL )
                                  /*   3. Try acq any system */
    NEW_ACQ                       /*   4. Start a new acquisition */
    ACQ_MODE( FULL )              /*   5. Do full acquisitions */
    ACQ_FAIL                      /*   6. Indicate that acquisition failed */
  LOOP                            /* Loop back */

  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while in emergency
** callback mode and:
**
** Serving system is AMPS system.
*/
static const sdssscr_elem_u_type  ssscr_int_srv_lost_emerg_cb_amps[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG_CB )        /* In emergency callback mode */
  IS_MODE( SRV_SYS, AMPS )        /* The serving sys is an AMPS system */

  /* Script */
  NEW_ACQ                         /* Start a new acquisition */
  CLR( EMERG_LST )                /* Clear the emergency list */

  LOOP_FOREVER                    /* Loop forever and: */
    SRV_STATUS( NO_SRV )          /*   1. Set service status to NO_SRV  */
    CLR( SCHED_LST )              /*   2. Cancel all reacq schedules */
    AVOID_CANCEL( SRV_SYS )       /*   3. Cancel acq avoid of srv system */
    ACQ_MODE( FULL )              /*   4. Do full acquisitions */
    ACQ_ALL( SRV_SYS, ANY )       /*   5. Try reacquiring the srv system */
  LOOP                            /* Loop back */

  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while in emergency
** callback mode.
*/
static const sdssscr_elem_u_type  ssscr_int_srv_lost_emerg_cb[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG_CB )        /* In emergency callback mode */
  IS_CNT( IDL_SYS, 1, OR_MORE, ALL ) /* Idle system is not empty */

  /* Script */
  NEW_ACQ                         /* Start a new acquisition */
  CLR( EMERG_LST )                /* Clear the emergency list */

  LOOP_FOREVER                    /* Loop forever and: */
    SRV_STATUS( NO_SRV )          /*   1. Set service status to NO_SRV  */
    CLR( SCHED_LST )              /*   2. Cancel all reacq schedules */
    AVOID_CANCEL( IDL_SYS )       /*   3. Cancel acq avoid of idle system */
    ACQ_MODE( FULL )              /*   4. Do full acquisitions */
    ACQ_ALL( IDL_SYS, ANY )       /*   5. Try reacquiring the idle system */
  LOOP                            /* Loop back */

  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost.
** 1. For GW only target.
** 2. IDL_SYS is empty
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_gw_pwrup[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )         /* Service is lost */
  IS( CAP_GW_PREF_GW_ONLY )               /* Capable for GSM/WCDMA but not 1x */
  IS_CNT( IDL_SYS, 0, EXACTLY, ALL)/* IDL_SYS is empty */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )    /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )         /* Copy PRL list to srch list */
  APP( SRCH_LST,GW_LST )
  ORDER( SRCH_LST, MRU_LST )       /* Order SRCH_LST according to MRU LST*/
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_PWR_UP ) /* Is it power up? */
    #if ( defined ( FEATURE_UMTS_UNIFORM_OOS_HANDLING_PWR_UP_ENH ) )
#error code not present
    #endif
    RESET_GW_ACQ_SCAN_STATUS
  ENDIF

  IF( SS_PREF_CHGD, EQUAL_TO, TRUE ) /* If ss preferences have changed, clear
                                     ** the ACQ_SCAN_STATUS */
      RESET_GW_ACQ_SCAN_STATUS         /* Clear the ACQ_SCAN_STATUS */
  ENDIF

  IF( STACK_SYNC_UP, EQUAL_TO, TRUE ) /* If required, sync up stacks */
      STACK_SYNC_UP
  ENDIF

  ACT_SCR( SRV_LOST_GW_PWRUP )     /* Do service lost scr for powe rup */
  ACT_SCR( PWR_SAVE_GW_PWRUP )          /* Nothing to acq - activate pwr-save scr*/

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost.
** 1. For GW only target.
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_gw[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )         /* Service is lost */
  IS( CAP_GW_PREF_GW_ONLY )               /* Capable for GSM/WCDMA but not 1x */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GW_LST )         /* Append GW systems to SRCH list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/
  ORDER2( SRCH_LST, IDL_SYS, BAND )
 
  IF( SS_PREF_CHGD, EQUAL_TO, TRUE ) /* If ss preferences have changed, clear
                                     ** the ACQ_SCAN_STATUS */
      RESET_GW_ACQ_SCAN_STATUS         /* Clear the ACQ_SCAN_STATUS */
  ENDIF
 
  IF( STACK_SYNC_UP, EQUAL_TO, TRUE ) /* If required, sync up stacks */
      STACK_SYNC_UP
  ENDIF

  ACT_SCR( SRV_LOST_GW )          /* Do service lost scr */
  ACT_SCR( PWR_SAVE_GW )          /* Nothing to acq - activate pwr-save scr*/

  END

};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#if (defined(FEATURE_UNIFORM_SCAN_OOS) && defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING))
#error code not present
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UNIFORM_SCAN_OOS
/* Script for when service is declared lost.
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_non_hybr_digital_mode[] = {

  IS_EVENT( INT_SRV_LOST )     /* Internal system lost event */
  IS( NO_HYBR_OPR )             /* Not in hybrid operation */
  IS( CAP_CDMA_HDR_PREF_CDMA_HDR_INCL ) /* Mode has both CDMA and HDR */
  IS( FEATURE_UOOS )             /* UOOS is defined */

  /* Script */
  CLR( CAND_SYS )                 /* Clear the candidate system           */
  CLR( AVAIL_LST )                /* Clear the available list             */
  CLR( START_PREF_LST )           /* Clear the best pref acq list         */
  ACT_SCR( CHK_AND_CLR_ACQ_SYS )
  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /* Acq failed - clear COLLOC list       */
  #endif /* FEATURE_HDR */

  #ifdef FEATURE_JCDMA
  SRV_STATUS( NO_SRV )
  #endif

  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list      */
  APP (SRCH_LST, FORBID_GEO_LST ) /* Copy forbidden GEO list to SRCH list */
  APP( SRCH_LST, MRU_LST )        /* Append the MRU list to SRCH list     */
  APP( SRCH_LST, PRL_LST )        /* Append the PRL list to SRCH list     */

  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list by MRU list          */

  #ifdef FEATURE_AVAIL_HOME_SIDE_PREF
  SORT( AVAIL_LST, CDMA )         /* Sort AVAIL_LST according to CDMA 1st */
  SORT( AVAIL_LST, CELL_HOME )    /* Sort AVAIL_LST according to Home 1st */
  #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

  CPY( TEMP_LST, SRCH_LST)
  DEL_SYS( TEMP_LST, NON_CDMA )
  CPY( TEMP_SYS, TEMP_LST )

  SCHED_COND2( TEMP_SYS, REACQ_EVERY_1S, BEST_PREF, \
               NOT_ON_EQUAL_OR_HIGHER_PRIORITY_SCHED_OR_AVOID_OR_FORBID )

  START_TIME                      /* Start time for the time based acq    */

  ACQ_MODE( FULL )                /* Do full acquisitions                 */

  IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
    ACQ_CNT_OR_TIME( SRCH_LST, BEST_PREF, 0, 4, TMR_HALF_AUTO_UOOS_SCAN ) 
  ELSE
    ACQ_CNT_OR_TIME( SRCH_LST, BEST_PREF, 0, 4, TMR_HALF_UOOS_SCAN ) 
  ENDIF

  ACQ_MODE( FULL2 )               /* Do full2 acquisitions                */

  IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
    ACQ_CNT_OR_TIME( SRCH_LST, BEST_PREF, SDSSSCR_FULL2_SYS, 2, TMR_HALF_AUTO_UOOS_SCAN ) 
  ELSE
    ACQ_CNT_OR_TIME( SRCH_LST, BEST_PREF, SDSSSCR_FULL2_SYS, 2, TMR_HALF_UOOS_SCAN ) 
  ENDIF

  ACQ_MODE( MICRO2 )               /* Do micro acquisitions                */

    IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
      ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_MICRO2_SYS, TMR_AUTO_UOOS_SCAN ) 
    ELSE
      ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_MICRO2_SYS, TMR_UOOS_SCAN ) 
    ENDIF

  LOOP_FOREVER                    /* Continue with time based acquisitions*/
    NEW_ACQ
    ACQ_MODE( FULL2 )
    IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
      ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_FULL2_SYS, TMR_AUTO_UOOS_SCAN )
    ELSE
      ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_FULL2_SYS, TMR_UOOS_SCAN )
    ENDIF
  LOOP

  ACQ_MODE( FULL )                /* Do full acquisitions                 */
  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Try acq available systems            */

  ACQ( GW_LST, BEST_PREF )        /* Try acq GSM/WCDMA systems            */
  ACQ( GW_LTD_LST, ANY )          /* Try acquiring limited GW service     */

  ACQ_FAIL                        /* Report ACQ_FAIL                      */
  ACT_SCR( PWR_SAVE_NON_HYBR_DIGITAL_MODE ) /* Nothing to acq -activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when service is declared lost for UOOS builds
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_hdr_only[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost                      */
  IS( FEATURE_UOOS )              /* Is UOOS turned on                    */
  IS( CAP_HDR_PREF_HDR_ONLY )     /* HDR only                             */

  /* Script */
  CLR( CAND_SYS )                 /* Clear the candidate system           */
  CLR( AVAIL_LST )                /* Clear the available list             */
  CLR( START_PREF_LST )           /* Clear the best pref acq list         */

  ACT_SCR( CHK_AND_CLR_ACQ_SYS )

  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list      */
  APP( SRCH_LST, MRU_LST )        /* Append the MRU list to SRCH list     */
  APP( SRCH_LST, PRL_LST )        /* Append the PRL list to SRCH list     */

  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list by MRU list          */
  
  #ifdef FEATURE_AVAIL_HOME_SIDE_PREF
  SORT( AVAIL_LST, CDMA )         /* Sort AVAIL_LST according to CDMA 1st */
  SORT( AVAIL_LST, CELL_HOME )    /* Sort AVAIL_LST according to Home 1st */
  #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

  #ifdef FEATURE_JCDMA
  SRV_STATUS( NO_SRV )
  #endif

  START_TIME                      /* Start time for the time based acq    */

  ACQ_MODE( FULL )                /* Do full acquisitions                 */
  ACQ_CNT( SRCH_LST, BEST_PREF, 0, 2 )

  ACQ_MODE( DEEP2 )               /* Do full2 acquisitions                */
  ACQ_CNT( SRCH_LST, BEST_PREF, SDSSSCR_DEEP2_SYS, 2 )
                                  /* Try acq DEEP2+2 systems              */

  ACQ_MODE( MICRO2 )               /* Do micro acquisitions                */
  IF( USER_HDR_GW_MODE_PREF_AND_CURR_NOT_HDR_GW, EQUAL_TO, TRUE )
    ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_MICRO2_SYS, TMR_AUTO_UOOS_SCAN )
  ELSE
    ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_MICRO2_SYS, TMR_UOOS_SCAN )
  ENDIF

  LOOP_FOREVER                    /* Continue with time based acquisitions*/
    NEW_ACQ
    ACQ_MODE( DEEP2 )
    IF( USER_HDR_GW_MODE_PREF_AND_CURR_NOT_HDR_GW, EQUAL_TO, TRUE )
      ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_DEEP2_SYS, TMR_AUTO_UOOS_SCAN )
    ELSE
      ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_DEEP2_SYS, TMR_UOOS_SCAN )
    ENDIF
  LOOP

  ACQ_MODE( FULL )                /* Do full acquisitions                 */
  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Try acq available systems            */

  ACQ_FAIL                        /* Report ACQ_FAIL                      */
  ACT_SCR( PWR_SAVE_HDR )         /* Nothing to acq -activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when service is declared lost for UOOS builds
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost                      */

  /* Script */
  CLR( CAND_SYS )                 /* Clear the candidate system           */
  CLR( AVAIL_LST )                /* Clear the available list             */
  CLR( START_PREF_LST )           /* Clear the best pref acq list         */
  ACT_SCR( CHK_AND_CLR_ACQ_SYS )
  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /* Acq failed - clear COLLOC list       */
  #endif /* FEATURE_HDR */

  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list      */
  APP (SRCH_LST, FORBID_GEO_LST ) /* Copy forbidden GEO list to SRCH list */
  APP( SRCH_LST, MRU_LST )        /* Append the MRU list to SRCH list     */
  APP( SRCH_LST, PRL_LST )        /* Append the PRL list to SRCH list     */

  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list by MRU list          */

  #ifdef FEATURE_AVAIL_HOME_SIDE_PREF
  SORT( AVAIL_LST, CDMA )         /* Sort AVAIL_LST according to CDMA 1st */
  SORT( AVAIL_LST, CELL_HOME )    /* Sort AVAIL_LST according to Home 1st */
  #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */
  CPY( TEMP_LST, SRCH_LST ) /* Prepare the search list              */
  DEL_SYS( TEMP_LST, NON_CDMA )
  CPY( TEMP_SYS, TEMP_LST )
  SCHED_COND2( TEMP_SYS, REACQ_EVERY_1S, BEST_PREF, \
               NOT_ON_EQUAL_OR_HIGHER_PRIORITY_SCHED_OR_AVOID_OR_FORBID )

  #ifdef FEATURE_JCDMA
  SRV_STATUS( NO_SRV )
  #endif

  START_TIME                      /* Start time for the time based acq    */

  ACQ_MODE( FULL )                /* Do full acquisitions                 */
  ACQ_CNT( SRCH_LST, BEST_PREF, 0, 6 )

  ACQ_MODE( FULL2 )               /* Do full2 acquisitions                */
  ACQ_CNT( SRCH_LST, BEST_PREF, SDSSSCR_FULL2_SYS, 2 )
                                  /* Try acq FULL2+2 systems              */
  ACQ_MODE( MICRO2 )               /* Do micro acquisitions                */
  IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
    ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_MICRO2_SYS, TMR_AUTO_UOOS_SCAN )
  ELSE
    ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_MICRO2_SYS, TMR_UOOS_SCAN )
  ENDIF

  LOOP_FOREVER                    /* Continue with time based acquisitions*/
    NEW_ACQ
    ACQ_MODE( FULL2 )
      IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
        ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_FULL2_SYS, TMR_AUTO_UOOS_SCAN )
      ELSE
        ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_FULL2_SYS, TMR_UOOS_SCAN )
      ENDIF
  LOOP

  ACQ_MODE( FULL )                /* Do full acquisitions                 */
  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Try acq available systems            */

  ACQ( GW_LST, BEST_PREF )        /* Try acq GSM/WCDMA systems            */
  ACQ_TRUE_MODE( GW_LTD_LST, ANY )          /* Try acquiring limited GW service     */

  ACQ_FAIL                        /* Report ACQ_FAIL                      */
  ACT_SCR( PWR_SAVE )             /* Nothing to acq -activate pwr-save scr*/
  END
};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#else
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when service is declared lost.
*/

static const  sdssscr_elem_u_type   ssscr_int_srv_lost[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */

  /* Script */
  CLR( CAND_SYS )                 /* Clear the candidate system */
  CLR( AVAIL_LST )                /* Clear the available list */
  CLR( START_PREF_LST )           /* Clear the best pref acq list */

  //ACQ_MODE( FULL )                /* Try full acq over collocated systems */
  //CPY( SRCH_LST, SRV_COLLOC_LST ) /*   Copy COLLOC list into SRCH list */
  //ORDER( SRCH_LST, MRU_LST )      /*   Order SRCH list according to MRU */
  //
  //DEL( SRCH_LST, HDR_DFLT_LST )      /* Delete HDR_DFLT_LST from search list */
  //
  //ACQ( SRCH_LST, BEST_PREF )      /*   Try acq best collocated sys */
  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /*   4. Acq failed - clear COLLOC list */
  #endif /* FEATURE_HDR */
  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU list */
  ACQ_MODE( MINI )                /* Try mini acq over GEO, MRU and PRL */
  ACQ( SRCH_LST, BEST_PREF )      /*   Try acquiring best GEO sys */
  ACQ( MRU_LST, BEST_PREF )       /*   Try acquiring best MRU sys */
  ACQ( PRL_LST, BEST_PREF )       /*   Try acquiring best ACQ table sys */
  ACQ_MODE( FULL )                /*   Do full acq on avail systems */
  ORDER( AVAIL_LST, PRL_LST )     /*   Order AVAIL_LST according to PRL */

  #ifdef FEATURE_AVAIL_HOME_SIDE_PREF
  SORT( AVAIL_LST, CDMA )         /*   Sort AVAIL_LST according to CDMA 1st*/
  SORT( AVAIL_LST, CELL_HOME )    /*   Sort AVAIL_LST according to Home 1st*/
  #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

  ACQ_ALL( AVAIL_LST, ALLOWED )   /*   Try reacquiring available systems*/

  ACQ( GW_LST, BEST_PREF )        /*   Try acquiring best GW_LST */
  ACQ_TRUE_MODE( GW_LTD_LST, ANY )          /*   Try acquiring limited GW service */

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
#error code not present
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  #ifdef FEATURE_JCDMA
  LOOP_NUM( MAX_ACQ_FAILS )       /* Mini acq failed - loop MAX_ACQ_FAILS */
  #else
  LOOP_TIME( SD_PWR_SAVE_ENTER_TIME )
                                  /* Mini acq failed - loop for 900s and: */
    SRV_STATUS( NO_SRV )          /*   Set service status to NO_SRV  */
  #endif
    ACQ_MODE( FULL )              /*   Do full acquisitions */
    NEW_ACQ                       /*   Start a new acquisition */
    CLR( CAND_SYS )               /*   Clear the candidate system */
    CLR( AVAIL_LST )              /*   Clear the available list */
    ACQ( SRCH_LST, BEST_PREF )    /*   Try acquiring best GEO sys */
    ACQ( MRU_LST, BEST_PREF )     /*   Try acquiring best MRU sys */
    ACQ( PRL_LST, BEST_PREF )     /*   Try acquiring best ACQ table sys */
    ORDER( AVAIL_LST, PRL_LST )   /*   Order AVAIL_LST according to PRL */

    #ifdef FEATURE_AVAIL_HOME_SIDE_PREF
    SORT( AVAIL_LST, CDMA )       /*   Sort AVAIL_LST according to CDMA 1st*/
    SORT( AVAIL_LST, CELL_HOME )  /*   Sort AVAIL_LST according to Home 1st*/
    #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

    ACQ_ALL( AVAIL_LST, ALLOWED ) /*   Try acquiring an available sys */
    ACQ( GW_LST, BEST_PREF )      /*   Try acquiring best GW_LST */
    ACQ_TRUE_MODE( GW_LTD_LST, ANY )        /*   Try acquiring limited GW service */

    #ifdef FEATURE_HICPS_STACK_SYNC_UP
#error code not present
    #endif /* FEATURE_HICPS_STACK_SYNC_UP */
    ACQ_FAIL                      /*   Indicate that acquisition failed */
  LOOP                            /* Loop back */

  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};

#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*---------------------------------------------------------------------------
                             SDSS_EVT_WLAN_INT_SRV_LOST
---------------------------------------------------------------------------*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when wlan service is declared lost while
** WLAN mode is not there.
*/
static const  sdssscr_elem_u_type   ssscr_wlan_int_srv_lost_mode_pref[] = {

  /* Condition */
  IS_EVENT( WLAN_INT_SRV_LOST )        /* Service is lost */
  NOT IS( MODE_PREF_WLAN)              /* WLAN mode is not enabled. */

  ACT_SCR( WLAN_PWR_SAVE_MODE_PREF)

  END

};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* 
** Script for when wlan service is declared lost with the reason
** being lack of firmware.
*/
static const  sdssscr_elem_u_type   ssscr_wlan_int_srv_lost_fail_reas[] = {

  /* Condition */
  IS_EVENT( WLAN_INT_SRV_LOST )        /* Service is lost */
  IS( WLAN_NOT_USABLE )                /* WLAN mode is not enabled. */

  ACT_SCR( WLAN_PWR_SAVE_MODE_PREF)    /* Go to deep sleep */

  END
};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_WLAN_UNIFORM_SCAN_OOS
#error code not present
#else

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when wlan service is declared lost while in
** manual mode.
*/
static const  sdssscr_elem_u_type   ssscr_wlan_int_srv_lost_manual[] = {

  /* Condition */
  IS_EVENT( WLAN_INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( MANUAL )               /* SD in manual mode. */

  #ifdef FEATURE_WLAN
#error code not present
  #endif
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when wlan service is declared lost while in
** automatic mode.
*/
static const  sdssscr_elem_u_type   ssscr_wlan_int_srv_lost[] = {


  /* Condition */
  IS_EVENT( WLAN_INT_SRV_LOST )    /* Service is lost */

  #ifdef FEATURE_WLAN
#error code not present
  #endif
};

#endif

/*---------------------------------------------------------------------------
                             SDSS_EVT_INT_RESEL
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for starting the reselection on a collocated system after receiving
//** the redirection message.
//*/
//static const  sdssscr_elem_u_type   ssscr_int_resel_colloc_redir[] = {
//
//  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
//  IS_ORIG_MODE( COLLOC )          /* Orig mode is colloc origination */
//  IS_REDIR( HDR_ANY )             /* Any HDR redirection */
//
//  /* Script */
//  CONT_REDIR( RESEL_HDR_RESEL )
//                                  /* Wait for RESEL_HDR_RESEL */
//  ACT_SCR( RESEL_HYBR )           /* Do hybrid system reselection */
//  END
//};
//
//
///*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
///* Script for starting the reselection on a collocated system after receiving
//** the release message.
//*/
//static const  sdssscr_elem_u_type   ssscr_int_resel_colloc_release[] = {
//
//  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
//  IS_ORIG_MODE( COLLOC )          /* Orig mode is colloc origination */
//  IS_RESEL_STT( RELEASE )         /* Rselection is in release state. */
//
//  CONT( TMR_HDR_RESEL_HOLD )      /* Wait 2 min after call end */
//  ACT_SCR( RESEL_HYBR )           /* Do hybrd system reselecton */
//  END
//};
//
//
///*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
///* Script for starting the reselection on a collocated system when the subnet
//** ID of the new serving system is different from the previous subnet ID. */
//
//static const  sdssscr_elem_u_type   ssscr_int_resel_colloc_new_sid[] = {
//
//  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
//  IS_ORIG_MODE( COLLOC )          /* Orig mode is colloc origination */
//  IS_SID( SRV_SYS, NOT_EXPECTED ) /* SID/NID is diff. from previous one */
//  IS_STATE( OPR )                 /* The SS-State is OPR */
//  IS_REDIR( NONE )                /* Redirection indication is OFF */
//
//  CONT( TMR_HDR_RESEL_HOLD )      /* Wait TMR_HDR_RESEL_HOLD */
//  ACT_SCR( RESEL_HYBR )           /* Do hybrd system reselecton */
//  END
//};
//
//
///*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
///* Script for starting the reselection on a collocated system when mobile
//** terminated call is ended
//*/
//
//static const  sdssscr_elem_u_type   ssscr_int_resel_colloc_ss_pref[] = {
//
//  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
//  IS_ORIG_MODE( COLLOC )          /* Orig mode is colloc origination */
//  NOT IS_PREF_REAS( CALL_ORIG )   /* Not in the call origination */
//  IS_RESEL_STT( SS_PREF )         /* reselection after ss_pref is changed.
//                                  */
//  /* Script */
//  CONT2_WAIT( HOLD_RESEL, TMR_HDR_RESEL_HOLD )
//                                  /* Wait for TMR_HDR_RESEL_HOLD since
//                                  ** SS_PREF timer is set.
//                                  */
//  ACT_SCR( RESEL_HYBR )           /* Do hybrid system reselection. */
//
//  END
//};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for starting the reselection on a collocated system
*/

static const  sdssscr_elem_u_type   ssscr_int_resel_colloc[] = {

  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_ORIG_MODE( COLLOC )          /* Orig mode is colloc origination */

  /* Script */
  ACT_SCR( RESEL_HYBR )           /* Do hybrid system reselection. */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for starting the full service reselection process.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_none[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( NONE )            /* Reselection not started yet */
  IS_SRV_STATUS( LIMITED_ANY )    /* We currently have lmtd service.
                                  */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  RESEL_MODE( FULL_SRV_1 )        /* Set reselection mode to full srv
                                  ** reselection stage 1.
                                  */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for doing the full service reselection when in manual plmn
** selection.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_srch_manual[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SETUP_SRCH )      /* Srch setup state */
  IS_RESEL_MODE( FULL_SRV )       /* Doing full srv reselection */
  IS_ORIG_MODE( MANUAL )          /* Currently in manual mode */

  /* Script */
  ACT_SCR( RESEL_FULL_SRV_MANUAL )/* Activate full srv reselection script
                                  ** for manual */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for doing the full service reselection when
** 1. not in manual plmn selection
** 2.GW only and no 1x target.
**
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_srch_gw_no_1x[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SETUP_SRCH )      /* Srch setup state */
  IS_RESEL_MODE( FULL_SRV )       /* Doing full srv reselection */
  IS( CAP_GW_PREF_GW_ONLY )              /* GW only target no 1x */

  /* Script */
  ACT_SCR( RESEL_GW_FULL_SRV ) /* Acquire gw last full system */
  END

};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for doing the full service reselection when not in manual plmn
** selection.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_srch[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SETUP_SRCH )      /* Srch setup state */
  IS_RESEL_MODE( FULL_SRV )       /* Doing full srv reselection */

  /* Script */
  ACT_SCR( RESEL_FULL_SRV )       /* Activate full srv reselection script*/
  END

};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for doing reselection just after releasing the GSM/WCDMA call.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_release[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( RELEASE )         /* In release substate */
  IS_RESEL_MODE( FULL_SRV )       /* Doing full srv reselection */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT ( 6 )                      /* Wait for 6 seconds after release */
  RESEL_STT( SETUP_SRCH )         /* Search for full srv substate */
  PROC_EVT( INT_RESEL )
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for doing reselection just after ss-pref change
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_ss_pref[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SS_PREF )         /* Preference change */
  IS_RESEL_MODE( FULL_SRV )       /* Doing full srv reselection */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT ( 2 )                      /* Wait for 2 seconds after release */
  RESEL_STT( SETUP_SRCH )         /* Search for full srv substate */
  PROC_EVT( INT_RESEL )
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for reacquiring the limited service after reselection
** for full service has failed.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_reacq[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing   */
  IS_RESEL_STT( REACQ )           /* Reacquire idle system                */
  IS_RESEL_MODE( FULL_SRV )       /* Doing full srv reselection           */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT        */
  NEW_ACQ
  /* SD is attempting to re-acquire back the limited service */
  SRV_REQ_TYPE( REQ_PERIODIC )
  ACQ( IDL_SYS, ANY )             /*  Try acquiring limited GW service    */

  #ifdef FEATURE_WLAN_UNIFORM_SCAN_OOS
#error code not present
  #endif

  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #endif

  /* The limited service re-acq failed, so we do a system lost.
  ** Set the srv req type to NORMAL or USER_RESEL based on AUTO 
  ** or Manual mode
  */
  IF( ORIG_MODE, EQUAL_TO, SD_SS_ORIG_MODE_MANUAL_ORIG )
    SRV_REQ_TYPE ( USER_SELECTION )
  ELSE
    SRV_REQ_TYPE ( NORMAL )
  ENDIF

  PROC_EVT( GW_OPR_SYS_LOST )     /* Do GW system lost event processing   */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing    */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for sleep interval during the full service reselection process in
** stage 1.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_wait_stage_1[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( WAIT )            /* Reselection in wait state */
  IS_RESEL_MODE( FULL_SRV_1 )     /* Full srv reselection in first stage */

  /* Script */
  //  PWR_SAVE                        /* Move phone to power save  */
  CONT_BSR( 20 )                      /* Wait for 20 seconds */
  RESEL_MODE( FULL_SRV_2 )        /* Set 2nd stage reselection */
  RESEL_STT( SETUP_SRCH )         /* Move to setup srch substate */
  PROC_EVT( INT_RESEL )
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for sleep interval during the full service reselection process in
** stage 2
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_wait_stage_2[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( WAIT )            /* Reselection in wait state */
  IS_RESEL_MODE( FULL_SRV_2 )     /* Full srv reselection in second stage */

  /* Script */
    //  PWR_SAVE                        /* Move phone to power save  */
  CONT_BSR( 30 )                      /* Wait for 30 seconds */
  RESEL_MODE( FULL_SRV_3 )        /* Set 3rd stage reselection */
  RESEL_STT( SETUP_SRCH )         /* Move to setup srch substate */
  PROC_EVT( INT_RESEL )
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for sleep interval during the full service reselection process in
** stage 3.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_wait_stage_3[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( WAIT )            /* Reselection in wait state */
  IS_RESEL_MODE( FULL_SRV_3 )     /* Full srv reselection in third stage */

  /* Script */
  // PWR_SAVE                        /* Move phone to power save  */
  CONT_BSR( 40 )                      /* Wait for 30 seconds */
  RESEL_MODE( FULL_SRV_4 )        /* Set 4th stage reselection */
  RESEL_STT( SETUP_SRCH )         /* Move to setup srch substate */
  PROC_EVT( INT_RESEL )
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for sleep interval during the full service reselection process in
** stage 4.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_wait_stage_4[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( WAIT )            /* Reselection in wait state */
  IS_RESEL_MODE( FULL_SRV_4 )     /* Full srv reselection in fourth stage */

  /* Script */
  // PWR_SAVE                        /* Move phone to power save  */
  CONT_BSR( 50 )                      /* Wait for 50 seconds */
  RESEL_MODE( FULL_SRV_5 )        /* Set 5th stage reselection */
  RESEL_STT( SETUP_SRCH )         /* Move to setup srch substate */
  PROC_EVT( INT_RESEL )
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for sleep interval during the full service reselection process in
** stage 5.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_wait_stage_5[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( WAIT )            /* Reselection in wait state */
  IS_RESEL_MODE( FULL_SRV )       /* Full srv reselection in last stage */

  /* Script */
  // PWR_SAVE                        /* Move phone to power save  */
  CONT_BSR( 60 )                      /* Wait for 60 seconds */
  RESEL_STT( SETUP_SRCH )         /* Move to setup srch substate */
  PROC_EVT( INT_RESEL )
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for a JCDMA target reselection for idle case
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_jcdma_idle[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS( FEATURE_JCDMA )             /* Is a JCDMA target */
  NOT IS_RESEL_STT( SS_PREF )     /* Reselection state is not SS_PREF */
  NOT IS_RESEL_STT( RELEASE )     /* Reselection state is not RELEASE */

  /* Script */
  RESEL_STT( WAIT )
  RESEL_MODE( INIT )              /* Set reselection mode to init */
  CONT2_BSR( TMR_CDMA_RESEL )         /* Wait for a long time, then do
                                  ** reselection */
  ACT_SCR( RESEL_JCDMA )          /* Activate dual-mode reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for a JCDMA target reselection after SS_PREF or RELEASE
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_jcdma_call_end[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS( FEATURE_JCDMA )             /* Is a JCDMA target */

  /* Script */
  RESEL_STT( WAIT )
  CONT2_BSR( TMR_CDMA_RESEL_HOLD_CALL_END )
                                  /* Wait for a short time, then do
                                  ** reselection */
  ACT_SCR( RESEL_JCDMA )          /* Activate dual-mode reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for starting the reselection process.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_none[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( NONE )            /* Reselection not started yet */

  /* Script */
  NEW_ACQ                         /* Start a new acquisition */
  CLR( ACQ_FULL2_SYS )            /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO2_SYS )           /* Clear last MICRO2 acquisition system */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  RESEL_MODE( INIT )              /* Set reselection mode to initial */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for reselection after new system is acquired.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_srch[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SRCH )            /* A new system is acquired */

  /* Script */
  NEW_ACQ                         /* Start a new acquisition */
  CLR( ACQ_FULL2_SYS )            /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO2_SYS )           /* Clear last MICRO2 acquisition system */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  RESEL_MODE( INIT )              /* Set reselection mode to initial */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for reacquiring the serving system after reselection
** period is over (serving system is an AMPS system).
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_reacq_amps[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( REACQ )           /* Reacquire idle system */
  IS_MODE( SRV_SYS, AMPS )        /* The serving sys is an AMPS system */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  ACQ_ALL( SRV_SYS, ALLOWED )     /* Try to reacquire the srv system */
  RESEL_MODE( INIT )              /* Reacq failed - set resel mode to init */
  PROC_EVT( AMPS_OPR_SYS_LOST )   /* Do AMPS system lost event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for reacquiring the serving system after reselection
** period is over.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_reacq[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( REACQ )           /* Reacquire idle system */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  ACQ_MODE( FULL )                /* Do full acquisition */
  ACQ_ALL( SRV_SYS, ALLOWED )     /* Try to reacquire the srv system */
  RESEL_MODE( INIT )              /* Reacq failed - set resel mode to init */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Do CDMA system lost event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for detecting that initial reselection mode is over and
** switching to normal reselection mode.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_init_to_norm[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_MODE( INIT )           /* Reselection mode is initial */

  /* Script */
  NEW_ACQ                         /* Start a new acquisition */
  RESEL_MODE( NORM )              /* Set reselection mode to normal */
  CLR( ACQ_MINI_SYS )             /* Clear last MINI acquisition system */
  CLR( ACQ_MINI2_SYS )            /* Clear last MINI2 acquisition system */
  CLR( ACQ_MICRO_SYS )            /* Clear last MICRO acquisition system */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for a dual-mode target reselection after SS-Preference change.
** (< 6 more preferred systems).
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_dm_ss_pref[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SS_PREF )         /* Reselection state is SS_PREF */
  IS ( CAP_AMPS )                 /* Capable of AMPS */
  IS( CAP_CDMA_CELL_NO_PCS )      /* MS is a dual-mode target */
  IS_CNT( SRV_PREF_LST, 5, OR_LESS, COMPLY ) /* < 6 more pref systems */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT( RESEL_TIMEOUT_SS_PREF )   /* Wait 1s then do reselection */
  ACT_SCR( RESEL_DM )             /* Activate dual-mode reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for dual-mode target reselection after call release.
** (< 6 more preferred systems).
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_dm_release[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( RELEASE )         /* Reselection state is RELEASE */
  IS ( CAP_AMPS )                 /* Capable of AMPS */
  IS( CAP_CDMA_CELL_NO_PCS )      /* MS is a dual-mode target */
  IS_CNT( SRV_PREF_LST, 5, OR_LESS, COMPLY ) /* < 6 more pref systems */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( TMR_CDMA_RESEL_HOLD_CALL_END )
                                  /* Wait for sometime then do reselection */
  ACT_SCR( RESEL_DM )             /* Activate dual-mode reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for dual-mode target reselection when serving system is an AMPS
** system.
** (< 6 more preferred systems).
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_dm_amps[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_MODE( SRV_SYS, AMPS )        /* Serving system is an AMPS system */
  IS ( CAP_AMPS )                 /* Capable of AMPS */
  IS( CAP_CDMA_CELL_NO_PCS )      /* MS is a dual-mode target */
  IS_CNT( SRV_PREF_LST, 5, OR_LESS, COMPLY ) /* < 6 more pref systems */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( TMR_AMPS_RESEL )         /* Wait then do reselection */
  ACT_SCR( RESEL_DM )             /* Activate dual-mode reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for dual-mode target reselection.
** (< 6 more preferred systems).
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_dm[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS( CAP_AMPS )
  IS( CAP_CDMA_CELL_NO_PCS )      /* MS is a dual-mode target */
  IS_CNT( SRV_PREF_LST, 5, OR_LESS, COMPLY ) /* < 6 more pref systems */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( TMR_CDMA_RESEL )         /* Wait for sometime then do reselection */
  ACT_SCR( RESEL_DM )             /* Activate dual-mode reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for initial reselection after SS-Preference change.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_init_ss_pref[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_MODE( INIT )           /* Reselection mode is initial */
  IS_RESEL_STT( SS_PREF )         /* Reselection state is SS_PREF */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT( RESEL_TIMEOUT_SS_PREF )   /* Wait for sometime then do reselection */
  ACT_SCR( RESEL_INIT )           /* Activate initial reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for initial reselection after call release.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_init_release[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_MODE( INIT )           /* Reselection mode is initial */
  IS_RESEL_STT( RELEASE )         /* Reselection state is RELEASE */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( TMR_CDMA_RESEL_HOLD_CALL_END )
                                  /* Wait a few seconds, then do reselection */
  ACT_SCR( RESEL_INIT )           /* Activate initial reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for initial reselection (serving system is an AMPS
** system).
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_init_amps[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_MODE( INIT )           /* Reselection mode is initial */
  IS_MODE( SRV_SYS, AMPS )        /* Serving system is an AMPS system */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( TMR_AMPS_RESEL )         /* Wait then do reselection */
  ACT_SCR( RESEL_INIT )           /* Activate initial reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for initial reselection.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_init[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_MODE( INIT )           /* Reselection mode is initial */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2_BSR( TMR_CDMA_RESEL )         /* Wait then do reselection */
  ACT_SCR( RESEL_INIT )           /* Activate initial reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for normal reselection after SS-Preference change.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_norm_ss_pref[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SS_PREF )         /* Reselection state is SS_PREF */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  #if defined( FEATURE_SD_ALT_CDMA_BSR_SCAN_1 )
#error code not present
  #else
  CONT( RESEL_TIMEOUT_SS_PREF )                       /* Wait 1s then do reselection */
  #endif
  ACT_SCR( RESEL_NORM )           /* Activate normal reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for normal reselection after call release.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_norm_release[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( RELEASE )         /* Reselection state is RELEASE */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  #if defined( FEATURE_SD_ALT_CDMA_BSR_SCAN_1 )
#error code not present
  #else
  CONT2( TMR_CDMA_RESEL_HOLD_CALL_END )
                                  /* Wait for sometime, then do reselection */
  #endif
  ACT_SCR( RESEL_NORM )           /* Activate normal reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for normal reselection (serving system is an AMPS
** system).
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_norm_amps[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_MODE( SRV_SYS, AMPS )        /* Serving system is an AMPS system */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  #if defined( FEATURE_SD_ALT_CDMA_BSR_SCAN_1 )
#error code not present
  #else
  CONT2( TMR_AMPS_RESEL )         /* Wait then do reselection */
  #endif  
  ACT_SCR( RESEL_NORM )           /* Activate normal reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#if defined( FEATURE_SD_ALT_CDMA_BSR_SCAN_1 )
#error code not present
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for normal reselection.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_norm[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  #if defined( FEATURE_SD_ALT_CDMA_BSR_SCAN_1 )
#error code not present
  #else
  CONT2_BSR( TMR_CDMA_RESEL )         /* Wait then do reselection */
  #endif
  ACT_SCR( RESEL_NORM )           /* Activate normal reselection script */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
#endif

/*---------------------------------------------------------------------------
                       Internal Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with
** internally generated events.
*/
static const sdssscr_elem_u_type * const dsssscr_int_event_tbl[] = {

                                      /* 0 */
  ssscr_int_srv_lost_gw_conn_mode_sleep_manual,
                                         /*
                                         ** Service is lost in GW conected mode
                                         ** and rem_uoos_sleep_time > 0
                                         */

  ssscr_int_srv_lost_gw_conn_mode_awake_manual,
                                       /*
                                       ** Service is lost in manual GW conected 
                                       ** mode and rem_uoos_awake_time > 0
                                       */

  ssscr_int_srv_lost_gw_conn_mode_awake,
                                       /*
                                       ** Service is lost in GW conected mode
                                       ** and rem_uoos_awake_time > 0
                                       */

  ssscr_int_srv_lost_gw_conn_mode_sleep,
                                       /*
                                       ** Service is lost in GW conected mode
                                       ** and rem_uoos_sleep_time > 0
                                       */

                                       /* 4 */
  ssscr_int_srv_lost_manual_call_orig, /* service is lost and orig mode is
                                      ** manual and during the call.
                                      */


  ssscr_int_srv_lost_manual,          /* Service is declared lost and orig
                                      ** mode is manual
                                      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_int_srv_lost_emerg_call_orig_gw_pwrup, 
                                      /* Service is declared lost and orig
                                      ** mode is emerg and AT is in a call orig
                                      */

  ssscr_int_srv_lost_emerg_orig_gw_pwrup, 
                                      /* Service is declared lost and orig
                                      ** mode is emerg 
                                      */

                                      /* 8 */
  ssscr_int_srv_lost_orig_gw_pwrup,   /* Service is declared lost while
                                      ** trying to originate a call and idle
                                      ** system is empty.
                                      ** And the target capability is GW only
                                      ** system. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_int_srv_lost_emerg_call_orig_gw,  
                                      /* Service is declared lost while
                                      ** trying to originate a E911 call and idle
                                      ** system is empty.
                                      ** And the target capability is GW only
                                      ** system. */


  ssscr_int_srv_lost_emerg_orig_gw,   /* Service is declared lost while
                                      ** mode is emerg and idle
                                      ** system is empty.
                                      ** And the target capability is GW only
                                      ** system. */


  ssscr_int_srv_lost_orig_gw,         /* Service is declared lost while
                                      ** trying to originate a call and idle
                                      ** system is not empty.
                                      ** And the target capability is GW only
                                      ** system. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                      /* 12 */
  ssscr_int_srv_lost_hdr_cdma_ho,     /* Service is declared lost while
                                      ** handing off the call to 1x */

  ssscr_int_srv_lost_normal_orig,     /* Service is declared lost while
                                      ** trying to originate a normal call */
  #if (defined(FEATURE_UNIFORM_SCAN_OOS) && defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING))
#error code not present
  #else
  ssscr_reserved_script,              /* Reserved script */
  ssscr_reserved_script,              /* Reserved script */
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_UNIFORM_SCAN_OOS
                                      /* 16 */
  ssscr_int_srv_lost_normal_orig_uoos_no_sr_hdr_only, 
                                      /* Service is declared lost while
                                      ** origination a normal call and not 
                                      ** doing silent re-dials, mode HDR 
                                     ** only */

  ssscr_int_srv_lost_normal_orig_uoos_no_sr_non_hybr_digital_mode, 
                                      /* Service is declared lost while
                                      ** origination a normal call and not 
                                      ** doing silent re-dials, mode HDR 
                                      ** only */

  ssscr_int_srv_lost_normal_orig_uoos_no_sr, /* Service is declared lost while
                                      ** origination a normal call and not 
                                      ** doing silent re-dials */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ssscr_int_srv_lost_colloc_orig_main_in_acq, 
                                      /* Service is declared lost while
                                      ** origination mode is collocated
                                      ** system acquisition and main is in
                                      ** acquisition */


                                      /* 20 */
  ssscr_int_srv_lost_colloc_orig_no_1x_srv, 
                                      /* Service is declared lost while
                                      ** origination mode is collocated
                                      ** system acquisition and main does not 
                                      ** have service */


  ssscr_int_srv_lost_colloc_call_orig_no_1x_srv, /* Service is declared lost while
                                      ** origination mode is collocated
                                      ** system and during a call and 1x 
                                      ** service is available */
  #else
  ssscr_reserved_script,              /* Reserved script */
  ssscr_reserved_script,              /* Reserved script */
  ssscr_reserved_script,              /* Reserved script */
  ssscr_reserved_script,              /* Reserved script */
  ssscr_reserved_script,              /* Reserved script */
  ssscr_reserved_script,              /* Reserved script */
  #endif


  ssscr_int_srv_lost_colloc_call_orig, 
                                      /* Service is declared lost while
                                      ** origination mode is collocated
                                      ** system and during a call */

  #ifdef FEATURE_UNIFORM_SCAN_OOS


  ssscr_int_srv_lost_colloc_orig_uoos_scan, 
                                      /* Service is declared lost while
                                      ** origination mode is collocated
                                      ** system acquisition and main has 
                                      ** service */
  #else
  ssscr_reserved_script,              /* Reserved script */
  #endif

                                      /* 24 */ 
  ssscr_int_srv_lost_colloc_orig,     /* Service is declared lost while
                                      ** origination mode is collocated
                                      ** system acquisition */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_int_srv_lost_otasp_orig,      /* Service is declared lost while
                                      ** trying to originate an OTASP call */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_int_srv_lost_emerg_orig_lock, /* Service is declared lost while
                                      ** trying to originate an emergency
                                      ** call and MS is CDMA locked until
                                      ** power-cycle */

  ssscr_int_srv_lost_emerg_orig,      /* Service is declared lost while
                                      ** trying to originate an emergency
                                      ** call */

                                      /* 28 */
  ssscr_int_srv_lost_emerg_cb_amps,   /* Service is declared lost while in
                                      ** emergency callback mode and serving
                                      ** system is an AMPS system */

  ssscr_int_srv_lost_emerg_cb,        /* Service is declared lost while in
                                      ** emergency callback mode */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_int_srv_lost_gw_pwrup,        /* Internally generated SS-Event,
                                      ** indicating that GW service is
                                      ** declared lost and idle system is
                                      ** empty and this is GW only target.*/

  ssscr_int_srv_lost_gw,              /* Internally generated SS-Event,
                                      ** indicating that GW service is
                                      ** declared lost and idle system is not
                                      ** empty and this is GW only target.*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined(FEATURE_UNIFORM_SCAN_OOS) && defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING))
#error code not present
  #else
  ssscr_reserved_script,              /* Reserved script */
  ssscr_reserved_script,              /* Reserved script */
  #endif
  
  #ifdef FEATURE_UNIFORM_SCAN_OOS
  ssscr_int_srv_lost_non_hybr_digital_mode, 
                                      /* Internally generated SS-Event,
                                      ** indicating that service is lost for
                                      ** non hybrid mode with CDMA and HDR 
                                      ** as the mode preferences */

  ssscr_int_srv_lost_hdr_only,        /* Internally generated SS-Event,
                                      ** indicating that service is lost for
                                      ** non hybrid mode with HDR 
                                      ** as the mode preference */

  #else
  ssscr_reserved_script,
  ssscr_reserved_script,
  #endif
                                      /* 36 */
  ssscr_int_srv_lost,                 /* Internally generated SS-Event,
                                      ** indicating that service is declared
                                      ** lost */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_wlan_int_srv_lost_mode_pref,  /* WLAN Service is declared lost while
                                      ** no WLAN mode. */

  ssscr_wlan_int_srv_lost_fail_reas,  /* WLAN service lost with no firmware */

  #ifdef FEATURE_WLAN_UNIFORM_SCAN_OOS
#error code not present
  #else
  ssscr_reserved_script,
  #endif

  ssscr_wlan_int_srv_lost_manual,
                                      /* WLAN Service is declared lost while
                                      ** in manual mode. */
  #ifdef FEATURE_WLAN_UNIFORM_SCAN_OOS
#error code not present
  #else
  ssscr_reserved_script,
  #endif
                                      
  ssscr_wlan_int_srv_lost,            /* Internally generated SS-Event,
                                      ** indicating that wlan service is declared
                                      ** lost */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  ssscr_int_resel_colloc_redir,       /* Script for starting the reselection
//                                      ** on a collocated system after
//                                      ** receiving the redirection message.*/
//
//  ssscr_int_resel_colloc_release,     /* Script for starting the reselection
//                                      ** on a collocated system after
//                                      ** receiving the release message. */
//
//  ssscr_int_resel_colloc_new_sid,     /* Script for starting the reselection
//                                      ** on a collocated system when the
//                                      ** subnet ID of the new serving
//                                      ** system is different from the
//                                      ** previous subnet ID. */
//
//
//  ssscr_int_resel_colloc_ss_pref,     /* Script for starting the reselection
//                                      ** on a collocated system when there is
//                                      ** a SS PREF change. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_int_resel_colloc,             /* Script for starting the reselection
                                      ** on a collocated system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_int_resel_lmtd_none,          /* Script for starting the full service
                                      ** reselection process.
                                      */

                                      /* 44 */
  ssscr_int_resel_lmtd_srch_manual,   /* Script for doing the full service
                                      ** reselection when in manual plmn
                                      ** selection.
                                      */

  ssscr_int_resel_lmtd_srch_gw_no_1x, /* GW only target.
                                      ** Script for doing the full service
                                      ** reselection when not in manual plmn
                                      ** selection.
                                      */

  ssscr_int_resel_lmtd_srch,          /* Script for doing the full service
                                      ** reselection when not in manual plmn
                                      ** selection.
                                      */

  ssscr_int_resel_lmtd_release,       /* Script for doing reselection just
                                      ** after releasing the GSM/WCDMA call.
                                      */

                                      /* 48 */
  ssscr_int_resel_lmtd_ss_pref,       /* Script for doing reselection just after
                                      ** ss-pref change.
                                      */

  ssscr_int_resel_lmtd_reacq,         /* Script for reacquiring the limited
                                      ** service after reselection for full
                                      ** service has failed.
                                      */

  ssscr_int_resel_lmtd_wait_stage_1,  /* Script for sleep interval during the
                                      ** full service reselection process in
                                      ** stage 1.
                                      */

  ssscr_int_resel_lmtd_wait_stage_2,  /* Script for sleep interval during the
                                      ** full service reselection process in
                                      ** stage 2.
                                      */

                                      /* 52 */
  ssscr_int_resel_lmtd_wait_stage_3,  /* Script for sleep interval during the
                                      ** full service reselection process in
                                      ** stage 3.
                                      */

  ssscr_int_resel_lmtd_wait_stage_4,  /* Script for sleep interval during the
                                      ** full service reselection process in
                                      ** stage 3.
                                      */

  ssscr_int_resel_lmtd_wait_stage_5,  /* Script for sleep interval during the
                                      ** full service reselection process in
                                      ** last stage.
                                      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_int_resel_jcdma_idle,         /* JCDMA reselection when idle */

                                      /* 56 */   
  ssscr_int_resel_jcdma_call_end,     /* JCDMA reselection after
                                      ** SS-Preference change or call release
                                      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_int_resel_none,               /* Starting reselection */
                                   
  ssscr_int_resel_srch,               /* Reselection after a new system is
                                      ** acquired */

  ssscr_int_resel_reacq_amps,         /* Reacquiring the serving system after
                                      ** reselection period is over (idle
                                      ** system is an AMPS system) */

                                      /* 60 */  
  ssscr_int_resel_reacq,              /* Reacquiring the serving system after
                                      ** reselection period is over */

  ssscr_int_resel_init_to_norm,       /* Detecting that initial reselection
                                      ** mode is over and switching to normal
                                      ** reselection mode */

  ssscr_int_resel_dm_ss_pref,         /* Dual-mode reselection after
                                      ** SS-Preference change */

  ssscr_int_resel_dm_release,         /* Dual-mode reselection after call
                                      ** release */
                                    
                                      /* 64 */
  ssscr_int_resel_dm_amps,            /* Dual-mode reselection (serving
                                      ** system is an AMPS system */

  ssscr_int_resel_dm,                 /* Dual-mode reselection */

  ssscr_int_resel_init_ss_pref,       /* Initial reselection after
                                      ** SS-Preference change */

  ssscr_int_resel_init_release,       /* Initial reselection after call
                                      ** release */

                                      /* 68 */ 
  ssscr_int_resel_init_amps,          /* Initial reselection (serving system
                                      ** is an AMPS system */

  ssscr_int_resel_init,               /* Initial reselection */

  ssscr_int_resel_norm_ss_pref,       /* Normal reselection after
                                      ** SS-Preference change */

  ssscr_int_resel_norm_release,       /* Normal reselection after call
                                      ** release */

                                      /* 72 */   
  ssscr_int_resel_norm_amps,          /* Normal reselection (serving system
                                      ** is an AMPS system */

  #if defined( FEATURE_SD_ALT_CDMA_BSR_SCAN_1 )
#error code not present
  #else
  ssscr_reserved_script,              /* Reserved script */
  ssscr_reserved_script,              /* Reserved script */
  ssscr_reserved_script,              /* Reserved script */

                                      /* 76 */   
  ssscr_reserved_script,              /* Reserved script */
  #endif

  ssscr_int_resel_norm,               /* Normal reselection start */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  ssscr_reserved_script,              /* Reserved script */
  ssscr_reserved_script,              /* Reserved script */

                                      /* 80 */ 
  ssscr_reserved_script,              /* Reserved script */
  #endif

};

/* <EJECT> */
/*===========================================================================
=============================================================================
============================== CALLED SCRIPTS ===============================
=============================================================================
===========================================================================*/

/*---------------------------------------------------------------------------
                            SDSSSCR_CLR_ACQ_LSTS
---------------------------------------------------------------------------*/

/* Clears acquisition process related lists.
*/
static const sdssscr_elem_u_type  ssscr_called_clr_acq_lsts[] = {

  CLR( CAND_SYS )                 /* Clear the candidate system */
  CLR( AVAIL_LST )                /* Clear the available system list */
  CLR( START_PREF_LST )           /* Clear the best pref acquisition list */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_CLR_ACQ_SCHED_LSTS
---------------------------------------------------------------------------*/

/* Clears acquisition process and reacquisition schedule related lists.
*/
static const sdssscr_elem_u_type  ssscr_called_clr_acq_sched_lsts[] = {

  CLR( CAND_SYS )                 /* Clear the candidate system */
  CLR( AVAIL_LST )                /* Clear the available system list */
  CLR( START_PREF_LST )           /* Clear the best pref acquisition list */
  CLR( SCHED_LST )                /* Cancel all reacquisition schedules */
  PREV_SCR                        /* Continue with previous script */
  END
};


/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_NORM
---------------------------------------------------------------------------*/

/* Normal service lost script. This script is used when service is declared
** lost and acquisition is to be started from scratch.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_norm[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */

  #if (defined FEATURE_JCDMA)
  CHK_SRV_STATUS_AND_SET( PWR_SAVE, NO_SRV )  /* Check if we are in pwr save,
                                              ** if we are then set service
                                              ** status to NO_SRV */
  #else
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  #endif

  REDIR_OFF                       /* Set redirection indication to OFF */
  HO_OFF                          /* Set handoff indication to OFF */
  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};

/*---------------------------------------------------------------------------
                          SDSSSCR_SRV_LOST_NORM_SLNT
---------------------------------------------------------------------------*/

/* Same as SDSSSCR_SRV_LOST_NORM, except that is does not set the service
** indication to OFF.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_norm_slnt[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */
  REDIR_OFF                       /* Set redirection indication to OFF */
  HO_OFF                          /* Set handoff indication to OFF */
  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};


/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_NORM_SRV_OFF_UNTIL_OH
---------------------------------------------------------------------------*/

/* Normal service lost script. This script is used when service is declared
** lost and acquisition is to be started from scratch.
** Keep service off until receiving OH
*/
static const sdssscr_elem_u_type  
                            ssscr_called_srv_lost_norm_srv_off_until_oh[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */

  SRV_OFF_UNTIL( OH )            /* Set service status to NO_SRV until OH */
  REDIR_OFF                       /* Set redirection indication to OFF */
  HO_OFF                          /* Set handoff indication to OFF */
  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_WLAN_SRV_LOST_NORM
---------------------------------------------------------------------------*/

/* Normal service lost script. This script is used when service is declared
** lost and acquisition is to be started from scratch.
*/
static const sdssscr_elem_u_type  ssscr_called_wlan_srv_lost_norm[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */

  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */

  REDIR_OFF                       /* Set redirection indication to OFF */
  HO_OFF                          /* Set handoff indication to OFF */
  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( WLAN_INT_SRV_LOST )   /* Do service lost processing */
  END
};

/*---------------------------------------------------------------------------
                          SDSSSCR_WLAN_SRV_LOST_NORM_SLNT
---------------------------------------------------------------------------*/

/* Same as SDSSSCR_SRV_LOST_NORM, except that is does not set the service
** indication to OFF.
*/
static const sdssscr_elem_u_type  ssscr_called_wlan_srv_lost_norm_slnt[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */
  REDIR_OFF                       /* Set redirection indication to OFF */
  HO_OFF                          /* Set handoff indication to OFF */
  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( WLAN_INT_SRV_LOST )        /* Do service lost processing */
  END
};



/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_SKIP
---------------------------------------------------------------------------*/

/* Optimized service lost script. This script is used when service is
** declared lost and acquisition is optimized to initially skip over systems
** that were attempted acquisition since the last time the NEW_ACQ script
** construct was encountered.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_skip[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */

  #if (defined FEATURE_JCDMA)
  CHK_SRV_STATUS_AND_SET( PWR_SAVE, NO_SRV )  /* Check if we are in pwr save,
                                              ** if we are then set service
                                              ** status to NO_SRV */
  #else
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  #endif

  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */
  AVOID( ACQ_SYS, UNTIL_NEW_ACQ ) /* Avoid ACQ_SYS until NEW_ACQ */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Same as SDSSSCR_SRV_LOST_SKIP, except that is does not set the service
** indication to OFF.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_skip_silent[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */

  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */
  AVOID( ACQ_SYS, UNTIL_NEW_ACQ ) /* Avoid ACQ_SYS until NEW_ACQ */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};


/*---------------------------------------------------------------------------
                         SDSSSCR_SRV_LOST_SKIP_ACQ
---------------------------------------------------------------------------*/

/* Optimized service lost script. This script is used when service is
** declared lost and acquisition is optimized to initially skip over the ACQ
** system.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_skip_acq[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */

  #if (defined FEATURE_JCDMA)
  CHK_SRV_STATUS_AND_SET( PWR_SAVE, NO_SRV )  /* Check if we are in pwr save,
                                              ** if we are then set service
                                              ** status to NO_SRV */
  #else
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  #endif

  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */
  NEW_ACQ                         /* Start a new acquisition */
  AVOID( ACQ_SYS, UNTIL_NEW_ACQ ) /* Avoid IDL_SYS until NEW_ACQ */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};

/*---------------------------------------------------------------------------
                         SDSSSCR_SRV_LOST_SKIP_IDL
---------------------------------------------------------------------------*/

/* Optimized service lost script. This script is used when service is
** declared lost and acquisition is optimized to initially skip over the idle
** system.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_skip_idl[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */

  #if (defined FEATURE_JCDMA)
  CHK_SRV_STATUS_AND_SET( PWR_SAVE, NO_SRV )  /* Check if we are in pwr save,
                                              ** if we are then set service
                                              ** status to NO_SRV */
  #else
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  #endif

  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */
  NEW_ACQ                         /* Start a new acquisition */
  AVOID( IDL_SYS, UNTIL_NEW_ACQ ) /* Avoid IDL_SYS until NEW_ACQ */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_SKIP_SRV_OFF_UNTIL_HO
---------------------------------------------------------------------------*/

/* Optimized service lost script. This script is used when service is
** declared lost and acquisition is optimized to initially skip over systems
** that were attempted acquisition since the last time the NEW_ACQ script
** construct was encountered.
** Keep service off until receiving OH
*/
static const sdssscr_elem_u_type  
                            ssscr_called_srv_lost_skip_srv_off_until_oh[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */

  SRV_OFF_UNTIL( OH )            /* Set service status to NO_SRV until OH */
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */
  AVOID( ACQ_SYS, UNTIL_NEW_ACQ ) /* Avoid ACQ_SYS until NEW_ACQ */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};


/*---------------------------------------------------------------------------
                          SDSSSCR_SYS_ACC_FAIL_PRL
---------------------------------------------------------------------------*/

/* Script for when system access attempt fails over a system that is listed
** in the PRL table, and SRDA is disabled.
*/

static const  sdssscr_elem_u_type   ssscr_called_sys_acc_fail_prl[] = {

  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy GEO list into SRCH list */

  DEL( SRCH_LST, IDL_SYS )        /* Delete IDLE_SYS from search list */
  DEL( SRCH_LST, ACQ_SYS )        /* Delete ACQ_SYS from search list */
  APP( SRCH_LST, IDL_SYS )        /* Append IDLE_SYS to end of search list */
  APP( SRCH_LST, ACQ_SYS )        /* Append ACQ_SYS to end of search list */

  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  CLR( CAND_SYS )                 /* Clear the candidate system */
  CLR( AVAIL_LST )                /* Clear the available list */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( SRCH_LST, PREFERRED )      /* Attempt to acquire preferred systems */
  ORDER( AVAIL_LST, PRL_LST )     /* Order avail systems according to PRL */
  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Attempt to acquire available systems */
  AVOID_CANCEL_COND( TBL_LST, UNTIL_ACQ_FAIL )
                                  /* Cancel acq_fail avoid */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};


/*---------------------------------------------------------------------------
                          SDSSSCR_SYS_ACC_FAIL_PRL_AMPS
---------------------------------------------------------------------------*/

/* Script for when system access attempt fails over a system that is listed
** in the PRL table, and SRDA is ENABLED.
*/

static const  sdssscr_elem_u_type   ssscr_called_sys_acc_fail_prl_amps[] = {

  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy GEO list into SRCH list */
  SORT( SRCH_LST, AMPS )          /* Bubble AMPS to the top */

  DEL( SRCH_LST, IDL_SYS )        /* Delete IDLE_SYS from search list */
  DEL( SRCH_LST, ACQ_SYS )        /* Delete ACQ_SYS from search list */
  APP( SRCH_LST, IDL_SYS )        /* Append IDLE_SYS to end of search list */
  APP( SRCH_LST, ACQ_SYS )        /* Append ACQ_SYS to end of search list */

  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  CLR( CAND_SYS )                 /* Clear the candidate system */
  CLR( AVAIL_LST )                /* Clear the available list */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( SRCH_LST, PREFERRED )      /* Attempt to acquire preferred systems */
  ORDER( AVAIL_LST, PRL_LST )     /* Order avail systems according to PRL */
  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Attempt to acquire available systems */
  AVOID_CANCEL_COND( TBL_LST, UNTIL_ACQ_FAIL )
                                  /* Cancel acq_fail avoid */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};


/*---------------------------------------------------------------------------
                         SDSSSCR_SYS_ACC_FAIL_AVAIL
---------------------------------------------------------------------------*/

/* Script for when system access attempt fails over a system that is not
** listed in the PRL acquisition table (i.e., available system) and srda is
** disabled.
*/
static const  sdssscr_elem_u_type   ssscr_called_sys_acc_fail_avail[] = {

  CPY( SRCH_LST, PRL_LST )        /* Copy acquisition table into SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Order the SRCH_LST with respect to MRU table */

  DEL( SRCH_LST, IDL_SYS )        /* Delete IDLE_SYS from search list */
  DEL( SRCH_LST, ACQ_SYS )        /* Delete ACQ_SYS from search list */
  APP( SRCH_LST, IDL_SYS )        /* Append IDLE_SYS to end of search list */
  APP( SRCH_LST, ACQ_SYS )        /* Append ACQ_SYS to end of search list */

  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( SRCH_LST, ALLOWED )        /* Attempt to acquire systems */
  AVOID_CANCEL_COND( TBL_LST, UNTIL_ACQ_FAIL )
                                  /* Cancel acq_fail avoid */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};

/*---------------------------------------------------------------------------
                         SDSSSCR_SYS_ACC_FAIL_AVAIL_AMPS
---------------------------------------------------------------------------*/

/* Script for when system access attempt fails over a system that is not
** listed in the PRL acquisition table (i.e., available system) ans srda
** is enabled.
*/
static const  sdssscr_elem_u_type   ssscr_called_sys_acc_fail_avail_amps[] = {

  CPY( SRCH_LST, PRL_LST )        /* Copy acquisition table into SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Order the SRCH_LST with respect to MRU table */
  SORT( SRCH_LST, AMPS )          /* Bubble AMPS to the top */

  DEL( SRCH_LST, IDL_SYS )        /* Delete IDLE_SYS from search list */
  DEL( SRCH_LST, ACQ_SYS )        /* Delete ACQ_SYS from search list */
  APP( SRCH_LST, IDL_SYS )        /* Append IDLE_SYS to end of search list */
  APP( SRCH_LST, ACQ_SYS )        /* Append ACQ_SYS to end of search list */

  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( SRCH_LST, ALLOWED )        /* Attempt to acquire systems */
  AVOID_CANCEL_COND( TBL_LST, UNTIL_ACQ_FAIL )
                                  /* Cancel acq_fail avoid */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};

/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_JCDMA
---------------------------------------------------------------------------*/

/* Reselection script for JCDMA targets.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_jcdma[] = {

  //RESEL_STT( SRCH )
  LOOP_COND( IS_HDR_ACTIVITY )    /* Loop till HDR activity stops */

    CONT2( TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY )
                                  /* Wait appropriate no. of seconds  */  
  LOOP

  CPY( CAND_SYS, SRV_SYS )        /* Set candidate sys per the serving sys */
  CPY( SRCH_LST, SRV_PREF_LST )   /* Copy PREF list into SRCH list */
  //ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU */
  DEL_SYS( SRCH_LST, NON_CDMA )   /* Delete non-CDMA systems from SRCH list */
  DEL_SYS( SRCH_LST, NON_COMPLY)  /* Delete non-COMPLY systems form SRCH list
                                  */
  RESEL_STT( SRCH )               /* Set reselection state to NONE */

  MEAS( SRCH_LST, DED, PILOT )    /* Get pilot strength measurement*/

  CPY(TEMP_LST, SRV_PREF_LST)     /* Get copy of PREF list  */
  DEL_SYS(TEMP_LST, NON_GW)       /* Get all preferrred GW systems in temp list */
  APP(SRCH_LST, TEMP_LST)         /* Append preferred GW systems to SRCH list   */
  ORDER(SRCH_LST, SRV_PREF_LST)   /* Order srch list as per pref. list.         */
  
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  #ifdef FEATURE_BSR2
  SRV_REQ_TYPE ( BSR )            /* Set service request type to Better System Reselection */
  #endif
  ACQ_PI_X( SRCH_LST, MORE_PREF, CDMA_PI_THR, NON_GW)
                                  /* Try to acq more pref sys with PI > 
                                  ** CDMA_PI_THR for non-GW systems
                                  */
  #ifdef FEATURE_BSR2
  SRV_REQ_TYPE ( NORMAL )            /* Reset service request type to Normal */
  #endif
  RESEL_STT( REACQ )
  ACQ_ALL( SRV_SYS, ALLOWED )     /* Try to reacquire the srv system */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Do CDMA system lost event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};


/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_DM
---------------------------------------------------------------------------*/

/* Reselection script for dual-mode targets with < 6 more preferred systems.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_dm[] = {

  CPY( CAND_SYS, SRV_SYS )        /* Set candidate sys per the serving sys */

  CPY( SRCH_LST, SRV_PREF_LST )   /* Copy PREF list into SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU */
  RESEL_STT( SRCH )               /* Set reselection state to SRCH */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL2 )               /* Do full2 acquisitions */
  ACQ_CNT( SRCH_LST, MORE_PREF, SDSSSCR_FULL2_SYS, 2 )
                                  /* Try acq 2 (rotating) more pref sys */

  RESEL_STT( REACQ )              /* Set reselection state to REACQ */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};


/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_INIT
---------------------------------------------------------------------------*/

/* Reselection script for initial mode reselection.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_init[] = {

  CPY( CAND_SYS, SRV_SYS )        /* Set candidate sys per the serving sys */

  CPY( SRCH_LST, SRV_PREF_LST )   /* Copy PREF list into SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU */
  RESEL_STT( SRCH )               /* Set reselection state to SRCH */
  ACQ_MODE( MICRO )               /* Do micro acquisitions */
  #ifdef FEATURE_BSR2
  SRV_REQ_TYPE ( BSR )            /* Set service request type to Better System Reselection */
  #endif
  START_TIME                      /* Start the acquisition timer */
  ACQ_TIME( SRCH_LST, MORE_PREF, 0, 10 )
                                  /* Try acq more pref sys for up to 10s */
  #ifdef FEATURE_BSR2
  SRV_REQ_TYPE ( NORMAL )         /* Set service request type to Normal */
  #endif
  RESEL_STT( REACQ )              /* Set reselection state to REACQ */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};


/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_NORM
---------------------------------------------------------------------------*/

/* Reselection script for normal mode reselection.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_norm[] = {

  CPY( CAND_SYS, SRV_SYS )        /* Set candidate sys per the serving sys */

  CPY( SRCH_LST, SRV_PREF_LST )   /* Copy PREF list into SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU */
  #if defined( FEATURE_SD_ALT_CDMA_BSR_SCAN_1 )
#error code not present
  #else  
  LOOP_COND( IS_HDR_ACTIVITY )    /* Hold off Reselection if HDR_ACTIVITY is going */
    CONT2( TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY )
  LOOP

  RESEL_STT( SRCH )               /* Set reselection state to SRCH */
  NEW_ACQ                         /* Start a new acquisition */
  START_TIME                      /* Start the acquisition timer */
  #ifdef FEATURE_BSR2
  SRV_REQ_TYPE ( BSR )    /* Set service request type to Better System Reselection */
  #endif
  ACQ_MODE( MINI )                /* Do mini acquisitions */
  ACQ_CNT( SRCH_LST, MORE_PREF, 0, 2 )
                                  /* Try acq 1st and 2nd more pref systems */
  #ifdef FEATURE_BSR2
  SRV_REQ_TYPE ( BSR )    /* Set service request type to Better System Reselection */
  #endif
  ACQ_MODE( MINI2 )               /* Do mini2 acquisitions */
  ACQ_CNT( SRCH_LST, MORE_PREF, SDSSSCR_MINI2_SYS, 1 )
                                  /* Try acq 1 (rotating) more pref sys */
  #ifdef FEATURE_BSR2
  SRV_REQ_TYPE ( BSR )    /* Set service request type to Better System Reselection */
  #endif
  ACQ_MODE( MICRO )               /* Do micro acquisitions */
  ACQ_TIME( SRCH_LST, MORE_PREF, SDSSSCR_MICRO_SYS, 10 )
                                  /* Try acq more pref sys for up to 10s */
  #endif

  #ifdef FEATURE_BSR2
  SRV_REQ_TYPE ( NORMAL )         /* Set service request type to Normal */
  #endif
  RESEL_STT( REACQ )              /* Set reselection state to REACQ */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*---------------------------------------------------------------------------
                          SDSSSCR_RESEL_RX_GT_99
---------------------------------------------------------------------------*/

///* Called SS-Script for when MS detects Rx-power >= -99dB during reselection.
//*/
//static const sdssscr_elem_u_type  ssscr_called_resel_rx_gt_99[] = {
//
//  CLR( AVAIL_LST )                  /* Clear the available list */
//  NEW_ACQ                           /* Start a new acquisition */
//  ACQ_MODE( FULL )                  /* Do full acquisitions */
//  ACQ_RX( SRCH_LST, BEST_PREF, 99 ) /* Try acquiring sys w/ Rx-pwr >= -99dB*/
//  ACQ_ALL( AVAIL_LST, ALLOWED )     /* Try acquiring an available sys */
//
//  PROC_EVT( CDMA_OPR_SYS_LOST )     /* Resel failed - try to reacq idle sys*/
//  ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
//
//  //SCHED( IDL_SYS, REACQ_SYS_LOST, ALLOWED ) /* Schedule reacq attempts */
//  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
//
//  END
//};



/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_FULL_SRV
---------------------------------------------------------------------------*/

/* Reselection script for full service reselection.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_full_srv[] = {

  RESEL_STT( SRCH )
  NEW_ACQ
  CLR( AVAIL_LST )
  ACQ( PRL_LST, BEST_PREF )       /* Attempt from PRL lst */
  ACQ( GW_LST, BEST_PREF )        /* Try acq GSM/WCDMA systems */
  ACQ_ALL( AVAIL_LST, ALLOWED )
  RESEL_STT( REACQ )              /* Set reselection state to REACQ */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_HYBR
---------------------------------------------------------------------------*/

/* Reselection script for full service reselection.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_hybr[] = {

  RESEL_STT( WAIT )

  IF(IS_HDR_REDIR, EQUAL_TO, TRUE)
  CONT3_WAIT( LESS_PREF, TMR_HDR_REDIR, CALL_END, TMR_HDR_RESEL_HOLD_CALL_END )
                                  /* Wait for either LESS_PREF or CALL_END
                                  ** expires since LESS_PREF or CALL_END is
                                  ** started.
                                  */

  ELSE 

  CONT3_WAIT( LESS_PREF, TMR_HDR_RESEL, CALL_END, TMR_HDR_RESEL_HOLD_CALL_END )
                                  /* Wait for either LESS_PREF or CALL_END
                                  ** expires since LESS_PREF or CALL_END is
                                  ** started.
                                  */
  ENDIF 

  CONT2_WAIT( HOLD_RESEL, TMR_HDR_RESEL_HOLD )
                                  /* Wait for TMR_HDR_RESEL_HOLD since
                                  ** HOLD timer is set.
                                  */
  /* Reset the timer to zero */
  CONT_SET( LESS_PREF, ZERO )     /* Set LESS_PREF timer to zero */
  CONT_SET( HOLD_RESEL, ZERO )    /* Set HOLD_RESEL timer to zero */
  CONT_SET( CALL_END, ZERO )      /* Set CALL_END timer to zero */

  /* Scripts for reselection */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  RESEL_STT( SRCH )               /* Reset the reselection state */
  REDIR_OFF                       /* Turn redirection indication OFF */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( SRV_PREF_LST, COLLOC_MORE_PREF )
                                  /* Acquire more preferred services */
  RESEL_STT( REACQ )
  ACQ( SRV_SYS, COLLOC_PREF )     /* Reacquire the last acquired system */
  RESEL_STT( NONE )
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_GW_FULL_SRV
---------------------------------------------------------------------------*/

/* Reselection script for full service reselection.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_gw_full_srv[] = {

  RESEL_STT( SRCH )
  CPY( SRCH_LST, PRL_LST )
  APP( SRCH_LST, GW_LST )
  ORDER2( SRCH_LST, IDL_SYS, BAND )
  DEL_SYS( SRCH_LST, NON_COMPLY )    /* Delete the non-complying sytems from
                                     ** the list
                                     */
  CPY( TEMP_SYS, SRCH_LST )
  NEW_ACQ

  #if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH)
  MEAS( TEMP_SYS, BACK, NETLIST )  /* Back search request on GW system */
  /* SD is attempting acquisition on PLMN's found.
     Set srv req type to REQ_PERIODIC */
  SRV_REQ_TYPE( REQ_PERIODIC )
  ACQ_NETLIST( TEMP_SYS, ALLOWED, 1 ) /* Acq only if PLMNs ret are >= 1 */                                
  #else
  ACQ( TEMP_SYS, ALLOWED )/* Attempt last full system */
  #endif

  RESEL_STT( REACQ )      /* Set reselection state to REACQ */

  PROC_EVT( INT_RESEL )    /* Do reselection processing */
  END
};


/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_FULL_SRV_MANUAL
---------------------------------------------------------------------------*/

/* Reselection script for full service reselection when in manual PLMN
** selection mode.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_full_srv_manual[] = {

  RESEL_STT( SRCH )
  CLR( AVAIL_LST )
  CPY( SRCH_LST, MANUAL_LST )       /* Copy srch list from user list.*/
  ORDER( SRCH_LST, GW_LAST_USER_SYS )/* Order the list by last user system */
  DEL_SYS( SRCH_LST, NON_COMPLY )    /* Delete the non-complying sytems from
                                     ** the list
                                     */
  NEW_ACQ

  #if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH)
  CPY( TEMP_SYS, SRCH_LST )        /* Copy temp_sys from srch_lst*/
  MEAS( TEMP_SYS, BACK, NETLIST )  /* Back search request on GW system */
  ACQ_NETLIST( TEMP_SYS, ANY, 1 )  /* Acq only if PLMNs ret are >= 1 */
  #else
  ACQ_CNT( SRCH_LST, ANY, 0, 1 )  /* Try acquiring first user systems */
  #endif

  RESEL_STT( REACQ )  

  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END

};
/*---------------------------------------------------------------------------
                            SDSSSCR_PWR_SAVE
---------------------------------------------------------------------------*/

/* Power-save mode of operation script (MS scans 15 minutes with no success
** before this script is activated).
*/

#ifdef FEATURE_UNIFORM_SCAN_OOS

static const sdssscr_elem_u_type  ssscr_called_pwr_save[] = {

  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /* Acq failed - clear COLLOC list       */
  #endif /* FEATURE_HDR */
  RESEL_STT( NONE )               /* Set reselection state to NONE        */
  REDIR_OFF                       /* Set redirection indication to off    */
  HO_OFF                          /* Set handoff indication to off        */

  LOOP_FOREVER                    /* Loop-forever and:                    */

    CPY( PWR_SAVE_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list      */
    APP (PWR_SAVE_LST, FORBID_GEO_LST ) /* Copy forbidden GEO list to SRCH list */
    APP( PWR_SAVE_LST, MRU_LST )        /* Append the MRU list to SRCH list     */
    APP( PWR_SAVE_LST, PRL_LST )        /* Append the PRL list to SRCH list     */

    ORDER( PWR_SAVE_LST, MRU_LST )      /* Order SRCH list by MRU list          */

    ACT_SCR( CLR_ACQ_SCHED_LSTS ) /* Clear acquisition related lists      */
    CPY( TEMP_LST, TBL_LST )
    DEL_SYS( TEMP_LST, HDR )
    AVOID_CANCEL( TEMP_LST )      /* Cancel all acquisition avoids        */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE       */
    PWR_SAVE                      /* Tell SS-Client to enter power-save   */

    IF( USER_1X_HDR_GW_MODE_PREF_AND_CURR_NOT_1X_HDR_GW, EQUAL_TO, TRUE )
      CONT3( TMR_UOOS_SLEEP, TMR_UOOS_SCAN, TMR_UOOS_GW_TOTAL_SCAN )       
    ELSE
      CONT2( TMR_UOOS_SLEEP )       /* Wait UOOS Sleep duration             */
    ENDIF

    CPY( TEMP_LST, PWR_SAVE_LST ) /* Prepare the search list              */
    DEL_SYS( TEMP_LST, NON_CDMA )
    CPY( TEMP_SYS, TEMP_LST )
    SCHED_COND2( TEMP_SYS, REACQ_EVERY_1S, BEST_PREF, \
               NOT_ON_EQUAL_OR_HIGHER_PRIORITY_SCHED_OR_AVOID_OR_FORBID )
                                  /* Scehdule the MRU                     */

    NEW_ACQ                       /* Start a new acquisition              */
    START_TIME

    ACQ_MODE( FULL )              /* Do full acquisitions                 */
    ACQ_CNT( PWR_SAVE_LST, BEST_PREF, 0, 6 )
                                  /* Try acq most recently used system    */
    
    ACQ_MODE( FULL2 )             /* Do full2 acquisitions                */
    ACQ_CNT( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL2_SYS, 2 )
                                  /* Try acq FULL2+6 systems              */
    ACQ_MODE( MICRO2 )             /* Do micro acquisitions                */

    IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
      ACQ_TIME2( PWR_SAVE_LST, BEST_PREF, SDSSSCR_MICRO2_SYS, TMR_AUTO_UOOS_SCAN )
    ELSE
      ACQ_TIME2( PWR_SAVE_LST, BEST_PREF, SDSSSCR_MICRO2_SYS, TMR_UOOS_SCAN )
    ENDIF

    ACT_SCR( UNIFORM_SCAN_CONT_PWR_SAVE_LST )

    ACQ_MODE( FULL )              /* Do full acquisitions */
    ACQ_ALL( AVAIL_LST, ALLOWED ) /* Try acq available systems            */
    ACQ( GW_LST, BEST_PREF )      /* Try acq GSM/WCDMA systems            */
    ACQ( GW_LTD_LST, ANY )        /* Try acquiring limited GW service     */

    ACQ_FAIL                      /* Send ACQ_FAIL                        */
  LOOP

  END
};


/*---------------------------------------------------------------------------
                      SDSSSCR_PWR_SAVE_NON_HYBR_DIGITAL_MODE
---------------------------------------------------------------------------*/

static const sdssscr_elem_u_type  ssscr_called_pwr_save_non_hybr_digital_mode[] = {

  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /* Acq failed - clear COLLOC list       */
  #endif /* FEATURE_HDR */
  RESEL_STT( NONE )               /* Set reselection state to NONE        */
  REDIR_OFF                       /* Set redirection indication to off    */
  HO_OFF                          /* Set handoff indication to off        */

  LOOP_FOREVER                    /* Loop-forever and:                    */

    CPY( PWR_SAVE_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list      */
    APP( PWR_SAVE_LST, FORBID_GEO_LST ) /* Copy forbidden GEO list to SRCH list */
    APP( PWR_SAVE_LST, MRU_LST )        /* Append the MRU list to SRCH list     */
    APP( PWR_SAVE_LST, PRL_LST )        /* Append the PRL list to SRCH list     */

    ORDER( PWR_SAVE_LST, MRU_LST )      /* Order SRCH list by MRU list          */

    ACT_SCR( CLR_ACQ_SCHED_LSTS ) /* Clear acquisition related lists      */
    CPY( TEMP_LST, TBL_LST )
    DEL_SYS( TEMP_LST, HDR )
    AVOID_CANCEL( TEMP_LST )      /* Cancel all acquisition avoids        */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE       */
    PWR_SAVE                      /* Tell SS-Client to enter power-save   */

    IF( USER_1X_HDR_GW_MODE_PREF_AND_CURR_NOT_1X_HDR_GW, EQUAL_TO, TRUE )
      CONT3( TMR_UOOS_SLEEP, TMR_UOOS_SCAN, TMR_UOOS_GW_TOTAL_SCAN )       
    ELSE
      CONT2( TMR_UOOS_SLEEP )       /* Wait UOOS Sleep duration             */
    ENDIF

    CPY( TEMP_LST, PWR_SAVE_LST)
    DEL_SYS( TEMP_LST, NON_CDMA )
    CPY( TEMP_SYS, TEMP_LST )

    SCHED_COND2( TEMP_SYS, REACQ_EVERY_1S, BEST_PREF, \
               NOT_ON_EQUAL_OR_HIGHER_PRIORITY_SCHED_OR_AVOID_OR_FORBID )

    NEW_ACQ                       /* Start a new acquisition              */
    START_TIME                      /* Start time for the time based acq    */

    ACQ_MODE( FULL )                /* Do full acquisitions                 */

    IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
      ACQ_CNT_OR_TIME( PWR_SAVE_LST, BEST_PREF, 0, 4, TMR_HALF_AUTO_UOOS_SCAN ) 
    ELSE
      ACQ_CNT_OR_TIME( PWR_SAVE_LST, BEST_PREF, 0, 4, TMR_HALF_UOOS_SCAN ) 
    ENDIF
                                  /* Acquire till time or count */

    ACQ_MODE( FULL2 )               /* Do full2 acquisitions                */

    IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
      ACQ_CNT_OR_TIME( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL2_SYS, 2, TMR_HALF_AUTO_UOOS_SCAN ) 
    ELSE
      ACQ_CNT_OR_TIME( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL2_SYS, 2, TMR_HALF_UOOS_SCAN ) 
    ENDIF
                                  /* Acquire till time or count */

    ACQ_MODE( MICRO2 )               /* Do micro acquisitions                */

    IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
      ACQ_TIME2( PWR_SAVE_LST, BEST_PREF, SDSSSCR_MICRO2_SYS, TMR_AUTO_UOOS_SCAN ) 
    ELSE
      ACQ_TIME2( PWR_SAVE_LST, BEST_PREF, SDSSSCR_MICRO2_SYS, TMR_UOOS_SCAN ) 
    ENDIF

    ACT_SCR( UNIFORM_SCAN_CONT_SRCH_LST )

    ACQ_MODE( FULL )              /* Do full acquisitions */
    ACQ_ALL( AVAIL_LST, ALLOWED ) /* Try acq available systems            */

    ACQ( GW_LST, BEST_PREF )      /* Try acq GSM/WCDMA systems            */
    ACQ( GW_LTD_LST, ANY )        /* Try acquiring limited GW service     */

    ACQ_FAIL                      /* Send ACQ_FAIL                        */
  LOOP

  END
};

/*---------------------------------------------------------------------------
                      SDSSSCR_PWR_SAVE_HDR
---------------------------------------------------------------------------*/

static const sdssscr_elem_u_type  ssscr_called_pwr_save_hdr[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE        */
  REDIR_OFF                       /* Set redirection indication to off    */
  HO_OFF                          /* Set handoff indication to off        */

  LOOP_FOREVER                    /* Loop-forever and:                    */

    CPY( PWR_SAVE_LST, MRU_LST )  /* Copy MRU into PWR_SAVE list          */
    APP( SRCH_LST, PRL_LST )  /* Append PRL to PWR_SAVE list          */

    ACT_SCR( CLR_ACQ_SCHED_LSTS ) /* Clear acquisition related lists      */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE       */
    PWR_SAVE                      /* Tell SS-Client to enter power-save   */

    IF( USER_1X_HDR_GW_MODE_PREF_AND_CURR_NOT_1X_HDR_GW, EQUAL_TO, TRUE )
      CONT3( TMR_UOOS_SLEEP, TMR_UOOS_SCAN, TMR_UOOS_GW_TOTAL_SCAN )       
    ELSE
      CONT2( TMR_UOOS_SLEEP )       /* Wait UOOS Sleep duration             */
    ENDIF

    NEW_ACQ                       /* Start a new acquisition              */
    START_TIME

    ACQ_MODE( FULL )              /* Do full acquisitions                 */
    ACQ_CNT( SRCH_LST, BEST_PREF, 0, 2 )
                                  /* Try acq most recently used system    */

    ACQ_MODE( DEEP2 )             /* Do full2 acquisitions                */
    ACQ_CNT( SRCH_LST, BEST_PREF, SDSSSCR_DEEP2_SYS, 2 )
                                  /* Try acq FULL2+2 systems              */

    ACQ_MODE( MICRO2 )             /* Do micro acquisitions                */

    IF( USER_HDR_GW_MODE_PREF_AND_CURR_NOT_HDR_GW, EQUAL_TO, TRUE )
      ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_MICRO2_SYS, TMR_AUTO_UOOS_SCAN )
    ELSE
      ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_MICRO2_SYS, TMR_UOOS_SCAN )
    ENDIF

    ACT_SCR( UNIFORM_SCAN_CONT_HDR )

    ACQ_MODE( FULL )              /* Do full acquisitions */
    ACQ_ALL( AVAIL_LST, ALLOWED ) /* Try acq available systems            */

    ACQ_FAIL                      /* Send ACQ_FAIL                        */
  LOOP

  END
};

#elif (defined FEATURE_JCDMA) /* and !FEATURE_UNIFORM_SCAN_OOS */

static const sdssscr_elem_u_type  ssscr_called_pwr_save[] = {


  CLR( ACQ_FULL_SYS )             /* Clear last FULL acquisition system */
  CLR( ACQ_FULL2_SYS )            /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO_SYS )            /* Clear last MICRO acquisition system */
  CLR( ACQ_MICRO2_SYS )
  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /* Clear COLLOC list */
  #endif
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acquisition related lists */
  //CPY( TEMP_LST, TBL_LST )
  //DEL_SYS( TEMP_LST, HDR )
  //AVOID_CANCEL( TEMP_LST )         /* Cancel all acquisition avoids */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Procedure 1-2
  */
  LOOP_TIME( 120 )                /* Loop for 120s and: */

    CPY( PWR_SAVE_LST, MRU_LST )  /* Copy MRU into PWR_SAVE list */
    APP( PWR_SAVE_LST, PRL_LST )  /* Append PRL to PWR_SAVE list */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Procedure 1-2, A
    */
    ACQ_MODE( FULL )               /* Do full acquisitions */
    ACQ_ALL( PWR_SAVE_LST, BEST_PREF ) /* Try acquiring systems */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Procedure 1-2, B x 4
    */
    ACT_SCR( PWR_SAVE_JCDMA_1_2_B )/* Do JCDMA procedure 1-2, B */

    ACT_SCR( PWR_SAVE_JCDMA_1_2_B )/* Do JCDMA procedure 1-2, B */

    ACT_SCR( PWR_SAVE_JCDMA_1_2_B )/* Do JCDMA procedure 1-2, B */

    ACT_SCR( PWR_SAVE_JCDMA_1_2_B )/* Do JCDMA procedure 1-2, B */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Procedure 1-2, A
    */
    SRV_STATUS( PWR_SAVE )         /* Set service status to PWR_SAVE  */
    PWR_SAVE                       /* Tell SS-Client to enter power-save */
    CONT( DEEP_SLEEP_J1 )          /* Wait DEEP_SLEEP_J1 seconds */

    ACQ_MODE( FULL )               /* Do full acquisitions */
    ACQ_ALL( PWR_SAVE_LST, BEST_PREF ) /* Try acquiring systems */

  LOOP                            /* Loop back */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Procedure 1-3
  */
  LOOP_FOREVER                    /* Loop forever and: */

    CPY( PWR_SAVE_LST, MRU_LST )   /* Copy MRU into PWR_SAVE list */
    APP( PWR_SAVE_LST, PRL_LST )   /* Append PRL to PWR_SAVE list */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Procedure 1-3, A
    */
    ACQ_MODE( FULL )               /* Do full acquisitions */
    ACQ_ALL( PWR_SAVE_LST, BEST_PREF ) /* Try acquiring systems */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Procedure 1-3, B x 4
    */
    ACT_SCR( PWR_SAVE_JCDMA_1_3_B )/* Do JCDMA procedure 1-3, B */

    ACT_SCR( PWR_SAVE_JCDMA_1_3_B )/* Do JCDMA procedure 1-3, B */

    ACT_SCR( PWR_SAVE_JCDMA_1_3_B )/* Do JCDMA procedure 1-3, B */

    ACT_SCR( PWR_SAVE_JCDMA_1_3_B )/* Do JCDMA procedure 1-3, B */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Procedure 1-3, A
    */
    SRV_STATUS( PWR_SAVE )         /* Set service status to PWR_SAVE  */
    PWR_SAVE                       /* Tell SS-Client to enter power-save */
    CONT( DEEP_SLEEP_J2 )          /* Wait DEEP_SLEEP_J2 seconds */

    ACQ_MODE( FULL )               /* Do full acquisitions */
    ACQ_ALL( PWR_SAVE_LST, BEST_PREF ) /* Try acquiring systems */

  LOOP                            /* Loop back */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  END
};

#else /* !FEATURE_UNIFORM_SCAN_OOS and !FEATURE_JCDMA */

static const sdssscr_elem_u_type  ssscr_called_pwr_save[] = {

  CLR( ACQ_FULL_SYS )             /* Clear last FULL acquisition system */
  CLR( ACQ_FULL2_SYS )            /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO_SYS )            /* Clear last MICRO acquisition system */
  CLR( ACQ_MICRO2_SYS )
  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /*   4. Acq failed - clear COLLOC list */
  #endif /* FEATURE_HDR */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */

  LOOP_FOREVER                    /* Loop-forever and: */

    CPY( PWR_SAVE_LST, MRU_LST )  /* Copy MRU into PWR_SAVE list */
    APP( PWR_SAVE_LST, PRL_LST )  /* Append PRL to PWR_SAVE list */

    ACT_SCR( CLR_ACQ_SCHED_LSTS ) /*   Clear acquisition related lists */
    CPY( TEMP_LST, TBL_LST )
    DEL_SYS( TEMP_LST, HDR )
    AVOID_CANCEL( TEMP_LST )      /*   Cancel all acquisition avoids */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( SD_PWR_SAVE_WAKEUP_TIME )/*   Wait 180s */

    NEW_ACQ                       /*   Start a new acquisition */

    ACQ_MODE( FULL )              /*   Do full acquisitions */
    ACQ_CNT( PWR_SAVE_LST, BEST_PREF, 0, 1 )
                                  /*   Try acq most recently used system */

    ACQ_MODE( FULL2 )             /*   Do full2 acquisitions */
    ACQ_CNT( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL2_SYS, 1 )
                                  /*   Try acq FULL2+1 system */

    ACQ_MODE( MICRO )             /*   Do micro acquisitions */
    ACQ( PWR_SAVE_LST, BEST_PREF ) /*   Try acq remaining systems */

    ACQ_MODE( FULL )              /*   Do full acquisitions */
    //ORDER( AVAIL_LST, PRL_LST )   /*   Order avail systems according to PRL*/
    ACQ_ALL( AVAIL_LST, ALLOWED ) /*   Try acq available systems */

    ACQ( GW_LST, BEST_PREF )      /*  Try acq GSM/WCDMA systems */
    ACQ( GW_LTD_LST, ANY )        /*  Try acquiring limited GW service */
  LOOP                            /* Loop back */

  END
};

#endif /* FEATURE_UNIFORM_SCAN_OOS */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if (defined( FEATURE_UNIFORM_SCAN_OOS ) && defined( FEATURE_UMTS_UNIFORM_OOS_HANDLING ))
#error code not present
#endif

#if ( !defined (FEATURE_UNIFORM_SCAN_OOS) || !defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING ) || !defined(FEATURE_WLAN_UNIFORM_SCAN_OOS))
static const sdssscr_elem_u_type  ssscr_called_reserved[] = {

  PREV_SCR
  END
};
#endif

#ifdef FEATURE_UNIFORM_SCAN_OOS
/*---------------------------------------------------------------------------
                          SDSSSCR_UNIFORM_SCAN_CONT_PWR_SAVE_LST
---------------------------------------------------------------------------*/

/* UOOS scan continue
*/
static const sdssscr_elem_u_type  ssscr_called_uniform_scan_cont_pwr_save_lst[] = {

  LOOP_FOREVER                    /* Continue with time based scans */
    NEW_ACQ
    ACQ_MODE( FULL2 )
    IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
      ACQ_TIME2( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL2_SYS, TMR_AUTO_UOOS_SCAN )
    ELSE
      ACQ_TIME2( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL2_SYS, TMR_UOOS_SCAN )
    ENDIF
  LOOP

  PREV_SCR
  END
};

/*---------------------------------------------------------------------------
                          SDSSSCR_UNIFORM_SCAN_CONT_SRCH_LST
---------------------------------------------------------------------------*/
/* UOOS scan continue
*/
static const sdssscr_elem_u_type  ssscr_called_uniform_scan_cont_srch_lst[] = {

  LOOP_FOREVER                    /* Continue with time based scans */
    NEW_ACQ
    ACQ_MODE( FULL2 )
    IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
      ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_FULL2_SYS, TMR_AUTO_UOOS_SCAN )
    ELSE
      ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_FULL2_SYS, TMR_UOOS_SCAN )
    ENDIF
  LOOP

  PREV_SCR
  END
};


/*---------------------------------------------------------------------------
                          SDSSSCR_UNIFORM_SCAN_CONT_HDR
---------------------------------------------------------------------------*/
/* UOOS scan continue
*/
static const sdssscr_elem_u_type  ssscr_called_uniform_scan_cont_hdr[] = {

  LOOP_FOREVER                    /* Continue with time based scans */
    NEW_ACQ
    ACQ_MODE( DEEP2 )
    IF( USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW, EQUAL_TO, TRUE )
      ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_DEEP2_SYS, TMR_AUTO_UOOS_SCAN )
    ELSE
      ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_DEEP2_SYS, TMR_UOOS_SCAN )
    ENDIF
  LOOP

  PREV_SCR
  END
};
#endif


/*---------------------------------------------------------------------------
                          SDSSSCR_CHK_AND_CLR_ACQ_SYS
---------------------------------------------------------------------------*/

/* Process the acq return
*/
static const sdssscr_elem_u_type  ssscr_called_chk_and_clr_acq_sys[] = {

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_PWR_SAVE_EXIT )
      PREV_SCR
  ENDIF

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_SS_PREF )
      PREV_SCR
  ENDIF

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_CDMA_ACQ_SCHM )
      PREV_SCR
  ENDIF

  CLR( ACQ_FULL_SYS )             /* Clear last FULL acquisition system   */
  CLR( ACQ_FULL2_SYS )            /* Clear last FULL2 acquisition system  */
  CLR( ACQ_MICRO_SYS )            /* Clear last MICRO acquisition system  */
  CLR( ACQ_MICRO2_SYS )           /* Clear last MICRO acquisition system  */
  CLR( GW_LAST_LTD_SYS )          /* Clear GW_LAST_LTD_SYS */
  CLR( ACQ_UNIFORM_FULL_SYS )
  CLR( ACQ_DEEP_SYS )
  CLR( ACQ_DEEP2_SYS )

  PREV_SCR
  END
};

/*---------------------------------------------------------------------------
                          SDSSSCR_PWR_SAVE_COLLOC_1SYS
---------------------------------------------------------------------------*/

/* Power-save script for collocated origination mode.
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_colloc_1sys[] = {

  /* Enter power save.
  */
  REDIR_OFF                       /* Set redirection indication to off */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acquisition related lists */

  CPY( PWR_SAVE_LST, SRV_COLLOC_LST ) 
                                  /* Copy COLLOC list into PWR_SAVE list */
  DEL_SYS( PWR_SAVE_LST, NON_HDR )/* Delete non-HDR systems from PWR_SAVE
                                  ** list */
  CPY( TEMP_SYS, PWR_SAVE_LST )   /* Copy PWR_SAVE_LST[0] to TEMP_SYS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* First stage power save:
  ** 1. Wait 2s
  ** 2. Try acquiring collocated system.
  */
  SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
  PWR_SAVE                        /* Tell SS-Client to enter power-save */
  CONT( 2 )                       /* Wait 2s */

  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_ALL( TEMP_SYS, COLLOC_BEST_PREF )
                                 /* Try acquiring colloc sys */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Second stage power save:
  ** 1. Wait 6s
  ** 2. Try acquiring collocated system.
  */
  SRV_STATUS( PWR_SAVE )         /* Set service status to PWR_SAVE  */
  PWR_SAVE                       /*   Tell SS-Client to enter power-save */
  CONT( 6 )                      /*   Wait 6s */

  ACQ_MODE( FULL )               /* Do full acquisitions */
  ACQ_ALL( TEMP_SYS, COLLOC_BEST_PREF )
                                 /* Try acquiring colloc sys */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Third stage power save:
  ** 1. Wait 30s
  ** 2. Try acquiring collocated system.
  ** 3. Wait 30s
  */
  SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
  PWR_SAVE                        /* Tell SS-Client to enter power-save */
  CONT( 30 )                      /* Wait 30s */

  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_ALL( TEMP_SYS, COLLOC_BEST_PREF )
                                  /* Try acquiring colloc sys */

  SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
  PWR_SAVE                        /* Tell SS-Client to enter power-save */
  CONT( 30 )                      /* Wait 30s */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fourth stage power save:
  ** 1. Try acquiring collocated system.
  ** 2. Wait 420s
  ** 3. Go back to 1.
  */
  LOOP_FOREVER                    /* Loop for ever and: */

    ACQ_MODE( DEEP )              /* Do deep acquisitions */
    ACQ_ALL( TEMP_SYS, COLLOC_BEST_PREF )
                                  /* Try acquiring colloc sys */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /* Tell SS-Client to enter power-save */
    CONT( 420 )                   /* Wait 420s */

    ACQ_MODE( SHALLOW )           /* Do shallow acquisitions */
    ACQ_ALL( TEMP_SYS, COLLOC_BEST_PREF )
                                  /* Try acquiring colloc sys */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /* Tell SS-Client to enter power-save */
    CONT( 420 )                   /* Wait 420s */

  LOOP                            /* Loop back */

  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_PWR_SAVE_COLLOC
---------------------------------------------------------------------------*/

/* Power-save script for collocated origination mode.
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_colloc[] = {

  #if defined( FEATURE_SD_ALT_EVDO_PWRUP_ALG_1 )
#error code not present
  #else /* FEATURE_SD_ALT_EVDO_PWRUP_ALG_1 */

  /* Enter power save.
  */
  REDIR_OFF                       /* Set redirection indication to off */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acquisition related lists */

  CPY( PWR_SAVE_LST, SRV_COLLOC_LST ) 
                                  /* Copy COLLOC list into PWR_SAVE list */
  DEL_SYS( PWR_SAVE_LST, NON_HDR )/* Delete non-HDR systems from PWR_SAVE
                                  ** list */
  ORDER( PWR_SAVE_LST, MRU_LST )  /* Order PWR_SAVE list according to MRU */
  CPY( TEMP_SYS, PWR_SAVE_LST )   /* Copy PWR_SAVE_LST[0] to TEMP_SYS */
  DEL( PWR_SAVE_LST, TEMP_SYS )   /* Delete most recent colloc sys from
                                  ** PWR_SAVE_LST */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* First stage power save:
  ** 1. Wait 2s
  ** 2. Try acquiring most recent collocated system.
  */
  SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
  PWR_SAVE                        /* Tell SS-Client to enter power-save */
  CONT( 2 )                       /* Wait 2s */

  NEW_ACQ
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( TEMP_SYS, COLLOC_BEST_PREF )
                                  /* Try acquiring most recent colloc sys */
  ACQ_FAIL                        /* Indicate that acquisition failed */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Second stage power save:
  ** 1. Wait 4s
  ** 2. Try acquiring most recent collocated system.
  */
  SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
  PWR_SAVE                        /* Tell SS-Client to enter power-save */
  CONT( 4 )                       /* Wait 4s */

  NEW_ACQ
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( TEMP_SYS, COLLOC_BEST_PREF )
                                  /* Try acquiring most recent colloc sys */
  ACQ_FAIL                        /* Indicate that acquisition failed */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Third stage power save:
  **  1. Wait 10s
  **  2. Try full acquisition on most recent collocated system.
  **  3. Try full acquisition on all channels from collocated list.
  **  4. Go back to 1 if third stage is run for less than 4 times.
  */
  LOOP_NUM ( 4 )

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */
    CONT( 10 )                    /*   Wait 10s */

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL )              /*   Do full acquisitions */
    ACQ( TEMP_SYS, COLLOC_BEST_PREF )
                                  /* Try acquiring most recent colloc sys */
    ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )  
                                  /* Try acquiring other colloc systems */
    ACQ_FAIL                      /* Indicate that acquisition failed */
  LOOP


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fourth stage power save:
  **  1. Wait 30s
  **  2. Try full acquisition on most recent collocated system.
  **  3. Try full acquisition on all channels from collocated list.
  **  4. Go back to 1 if third stage is run for less than 4 times.
  */
  LOOP_NUM ( 4 )

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */
    CONT( 30 )                    /*   Wait 30s */

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL )              /*   Do full acquisitions */
    ACQ( TEMP_SYS, COLLOC_BEST_PREF )
                                  /* Try acquiring most recent colloc sys */
    ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )  
                                  /* Try acquiring other colloc systems */
    ACQ_FAIL                      /* Indicate that acquisition failed */
  LOOP


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fifth stage power save:
  **  1. Wait 60s
  **  2. Try full acquisition on most recent collocated system.
  **  3. Try full acquisition on all channels from collocated list.
  **  4. Go back to 1 if third stage is run for less than 4 times.
  */
  LOOP_NUM ( 4 )

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */
    CONT( 60 )                    /*   Wait 30s */

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL )              /*   Do full acquisitions */
    ACQ( TEMP_SYS, COLLOC_BEST_PREF )
                                  /* Try acquiring most recent colloc sys */
    ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )  
                                  /* Try acquiring other colloc systems */
    ACQ_FAIL                      /* Indicate that acquisition failed */
  LOOP


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Last stage power save:
  ** 1. Wait 300s
  ** 2. Try deep acquisition on most recent collocated system.
  ** 3. Try micro acquisition on all channels from collocated list.
  ** 4. Wait 300s
  ** 5. Try deep acquisition on 1 system from collocated list in a rotating fashion.
  ** 6. Try micro acquisition on most recent collocated system.
  ** 7. Try micro acquisition on all channels from collocated list.
  ** 8. Go back to 4, if all channels are not exhausted.
  ** 9. Wait 300s
  ** 10. Try shallow acquisition on most recent collocated system.
  ** 11. Try micro acquisition on all channels from collocated list.
  ** 12. Wait 300s
  ** 13. Try shallow acquisition on 1 system from collocated list in a rotating fashion.
  ** 14. Try micro acquisition on most recent collocated system.
  ** 15. Try micro acquisition on all channels from collocated list.
  ** 16. Go back to 12, if all channels are not exhausted.
  ** 17. Go back to 1.
  */
  CLR( ACQ_DEEP2_SYS )            /* Clear the DEEP_SYS */
  LOOP_FOREVER                    /* Loop for ever and: */

    ACT_SCR( PWR_SAVE_COLLOC_DEEP ) /* Do deep acquisitions */
    ACT_SCR( PWR_SAVE_COLLOC_SHALLOW )/* Do shallow acquisitions */

  LOOP                            /* Loop back */

  #endif /* FEATURE_SD_ALT_EVDO_PWRUP_ALG_1 */

  END

};

#ifdef FEATURE_UNIFORM_SCAN_OOS

/*---------------------------------------------------------------------------
            SDSSSCR_CALLED_UNIFORM_COLLOC_CALL_ORIG_NO_1X_SRV
---------------------------------------------------------------------------*/

/* Colloc Call origination for UOOS when no 1X srv
*/
static const sdssscr_elem_u_type  ssscr_called_uniform_colloc_call_orig_no_1x_srv[] = {

  ACQ_MODE( DEEP )                /*   1. Set acquisition mode to full */
  CPY( SRCH_LST, SRV_COLLOC_LST)  /*   2. Copy COLLOC list into SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /*   3. Order SRCH list according to MRU */

  START_TIME

  ACQ_CNT( SRCH_LST, COLLOC_BEST_PREF, 0, 1 )
                                  /*   4. Try acq best pref collocated sys */
  ACQ_FAIL

  ACQ_MODE( DEEP2 )
  ACQ_CNT( SRCH_LST, COLLOC_BEST_PREF, SDSSSCR_DEEP2_SYS, 1 )
                                  /*   Try acq DEEP2+1 system */

  #ifdef FEATURE_UNIFORM_SCAN_OOS_HDR_ENH
  ACQ_MODE( MICRO2 )
  ACQ_TIME2( SRCH_LST, COLLOC_BEST_PREF, SDSSSCR_MICRO2_SYS, TMR_UOOS_HDR_SCAN )

  LOOP_FOREVER                    /* Continue with time based acquisitions*/
  NEW_ACQ
  ACQ_MODE( DEEP2 )
  ACQ_TIME2( SRCH_LST, BEST_PREF, SDSSSCR_DEEP2_SYS, TMR_UOOS_HDR_SCAN )
  LOOP
  #endif

  ACQ_FAIL
  NEW_ACQ                         /* Start a new acquisition */

  /* Enter power save.
  */
  REDIR_OFF                       /* Set redirection indication to off */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acquisition related lists */
  SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
  PWR_SAVE                        /* Tell SS-Client to enter power-save */

  CONT( 0 )
  END
};

#endif

/*---------------------------------------------------------------------------
                       SDSSSCR_PWR_SAVE_COLLOC_DEEP
---------------------------------------------------------------------------*/

/* Power-save script for collocated origination mode -- deep acquisitions.
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_colloc_deep[] = {



  SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
  PWR_SAVE                      /* Tell SS-Client to enter power-save */
  CONT2(TMR_PWR_SAVE_COLLOC)     /* Wait in NV defined, default is 300s */
  NEW_ACQ                       /* Start a new acquisition */
  ACQ_MODE( DEEP )              /* Do deep acquisitions */
  ACQ( TEMP_SYS, COLLOC_BEST_PREF )
                                /* Try acquiring most recent colloc sys */
  ACQ_MODE( MICRO )             /* Do full acquisitions */
  ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )  
                                /* Try acquiring other colloc systems */
  ACQ_FAIL                      /* Indicate that acquisition failed */


  LOOP_LIST( PWR_SAVE_LST )     /* Loop HDR_PWR_SAVE_LST count and: */

    SRV_STATUS( PWR_SAVE )      /* Set service status to PWR_SAVE  */
    PWR_SAVE                    /* Tell SS-Client to enter power-save */
    CONT2(TMR_PWR_SAVE_COLLOC)     /*Wait in NV defined, default is 300s */
    NEW_ACQ                     /* Start a new acquisition */
    ACQ_MODE( DEEP2 )           /* Do deep acquisitions */
    ACQ_CNT( PWR_SAVE_LST, COLLOC_MORE_PREF, SDSSSCR_DEEP2_SYS, 1 )
    ACQ_MODE( MICRO )           /* Do micro acquisitions */
    ACQ( TEMP_SYS, COLLOC_BEST_PREF )
    ACQ_MODE( MICRO )           /* Do micro acquisitions */
    ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )
    ACQ_FAIL                    /* Indicate that acquisition failed */

  LOOP                          /* Loop back */

  PREV_SCR                      /* Continue with previous script */
  END

};

/*---------------------------------------------------------------------------
                       SDSSSCR_PWR_SAVE_COLLOC_SHALLOW
---------------------------------------------------------------------------*/

/* Power-save script for collocated origination mode - shallow acquisitions.
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_colloc_shallow[] = {


  SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
  PWR_SAVE                      /* Tell SS-Client to enter power-save */
  CONT2(TMR_PWR_SAVE_COLLOC)     /* Wait in NV defined, default is 300s */
  NEW_ACQ                       /* Start a new acquisition */
  ACQ_MODE( SHALLOW )           /* Do shallow acquisitions */
  ACQ( TEMP_SYS, COLLOC_BEST_PREF )
                                /* Try acquiring most recent colloc sys */
  ACQ_MODE( MICRO )             /* Do full acquisitions */
  ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )  
                                /* Try acquiring other colloc systems */
  ACQ_FAIL                      /* Indicate that acquisition failed */


  LOOP_LIST( PWR_SAVE_LST )     /* Loop HDR_PWR_SAVE_LST count and: */

    SRV_STATUS( PWR_SAVE )      /* Set service status to PWR_SAVE  */
    PWR_SAVE                    /* Tell SS-Client to enter power-save */
    CONT2(TMR_PWR_SAVE_COLLOC)     /* Wait in NV defined, default is 300s */
    NEW_ACQ                     /* Start a new acquisition */
    ACQ_MODE( SHALLOW2 )        /* Do shallow acquisitions */
    ACQ_CNT( PWR_SAVE_LST, COLLOC_BEST_PREF, SDSSSCR_SHALLOW2_SYS, 1 )
    ACQ_MODE( MICRO )           /* Do micro acquisitions */
    ACQ( TEMP_SYS, COLLOC_BEST_PREF )
    ACQ_MODE( MICRO )           /* Do micro acquisitions */
    ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )
    ACQ_FAIL                    /* Indicate that acquisition failed */

  LOOP                          /* Loop back */

  PREV_SCR                      /* Continue with previous script */
  END

};

//static const sdssscr_elem_u_type  ssscr_called_pwr_save[] = {
//
//  RESEL_STT( NONE )                             /* Set resel state to NONE */
//  SRV_OFF                                       /* Turn srv indicator OFF */
//  REDIR_OFF                                     /* Turn redirection OFF */
//  HO_OFF                                        /* Turn handoff OFF */
//  ACT_SCR( CLR_ACQ_SCHED_LST )                      /* Clear acq related lists */
//  AVOID_CANCEL( TBL_LST )                       /* Cancel all acq avoids */
//
//  CPY( PWR_SAVE_LST, MRU_LST )                      /* Copy MRU into PWR_SAVE list */
//  APP( PWR_SAVE_LST, PRL_LST )                      /* Append PRL to PWR_SAVE list */
//
//  PWR_SAVE                                      /* Enter power-save mode */
//  CONT( 200 )                                   /* Wait 200s */
//  MEAS( PWR_SAVE_LST, DED, RXPWR )                  /* Get Rx-power measurement*/
//  ACT_SCR_RX( PWR_SAVE_RX_GT_99, PWR_SAVE_LST, 99 ) /* Act acq scr if Rx > 99 */
//
//  PWR_SAVE                                      /* Enter power-save mode */
//  CONT( 400 )                                   /* Wait 400s */
//  MEAS( PWR_SAVE_LST, DED, RXPWR )                  /* Get Rx-power measurement*/
//  ACT_SCR_RX( PWR_SAVE_RX_GT_99, PWR_SAVE_LST, 99 ) /* Act acq scr if Rx > 99 */
//
//  LOOP_FOREVER                                  /* Loop forever */
//    PWR_SAVE                                    /* Enter power-save mode */
//    CONT( 800 )                                 /* Wait 800s */
//    MEAS( PWR_SAVE_LST, DED, RXPWR )                /* Get Rx-power measurement*/
//    ACT_SCR_RX( PWR_SAVE_RX_GT_99, PWR_SAVE_LST, 99)/* Act acq scr if Rx > 99 */
//  LOOP                                          /* Loop back */
//  END
//};

/*---------------------------------------------------------------------------
                          SDSSSCR_PWR_SAVE_RX_GT_99
---------------------------------------------------------------------------*/

/* Called SS-Script for when MS detects Rx-power >= -99dB during power-save
** mode of operation.
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_rx_gt_99[] = {

  CLR( AVAIL_LST )                  /* Clear the available list */
  NEW_ACQ                           /* Start a new acquisition */
  ACQ_MODE( FULL )                  /* Do full acquisitions */
  ACQ_RX( PWR_SAVE_LST, BEST_PREF, 99 ) 
                                    /* Try acquiring sys w/ Rx-pwr >= -99dB*/
  ORDER( AVAIL_LST, PRL_LST )       /* Order avail systems according to PRL*/
  ACQ_ALL( AVAIL_LST, ALLOWED )     /* Try acquiring an available sys */
  ACT_SCR( PWR_SAVE )               /* Acq failed - Activate pwr-save scr */
  END
};


/*---------------------------------------------------------------------------
                          SDSSSCR_PWR_SAVE_JCDMA_1_2_B
---------------------------------------------------------------------------*/

static const sdssscr_elem_u_type  ssscr_called_pwr_save_jcdma_1_2_b[] = {

  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE  */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  CONT( DEEP_SLEEP_J1 )             /* Wait DEEP_SLEEP_J1 seconds */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ACQ_MODE( MINI )                  /* Do mini acquisitions */
  ACQ_ALL( PWR_SAVE_LST, BEST_PREF )/* Try acquiring systems */

  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE  */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  CONT( DEEP_SLEEP_JSUB1 )          /* Wait DEEP_SLEEP_JSUB1 seconds */

  ACQ_MODE( MICRO )                 /* Do micro acquisitions */
  ACQ_ALL( PWR_SAVE_LST, BEST_PREF )/* Try acquiring systems */

  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE  */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  CONT( DEEP_SLEEP_JSUB1 )          /* Wait DEEP_SLEEP_JSUB1 seconds */

  ACQ_MODE( MINI )                  /* Do mini acquisitions */
  ACQ_ALL( PWR_SAVE_LST, BEST_PREF )/* Try acquiring systems */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PREV_SCR                          /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                          SDSSSCR_PWR_SAVE_JCDMA_1_3_B
---------------------------------------------------------------------------*/

static const sdssscr_elem_u_type  ssscr_called_pwr_save_jcdma_1_3_b[] = {

  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE  */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  CONT( DEEP_SLEEP_J2 )             /* Wait DEEP_SLEEP_J2 seconds */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ACQ_MODE( MINI )                  /* Do mini acquisitions */
  ACQ_ALL( PWR_SAVE_LST, BEST_PREF )/* Try acquiring systems */

  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE  */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  CONT( DEEP_SLEEP_JSUB2 )          /* Wait DEEP_SLEEP_JSUB2 seconds */

  ACQ_MODE( MICRO )                 /* Do micro acquisitions */
  ACQ_ALL( PWR_SAVE_LST, BEST_PREF )/* Try acquiring systems */

  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE  */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  CONT( DEEP_SLEEP_JSUB2 )          /* Wait DEEP_SLEEP_JSUB2 seconds */

  ACQ_MODE( MINI )                  /* Do mini acquisitions */
  ACQ_ALL( PWR_SAVE_LST, BEST_PREF )/* Try acquiring systems */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PREV_SCR                          /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                       SDSSSCR_PWR_SAVE_OVERREACH_2_2
---------------------------------------------------------------------------*/

/* Power-save mode of operation script for JCDMA over-reach condition
** procedure 2-2.
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_overreach_2_2[] = {


  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /* Clear COLLOC list */
  #endif
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acquisition related lists */
  CPY( PWR_SAVE_LST, SRCH_LST )   /* Copy SRCH_LST to PWR_SAVE_LST */
  APP( PWR_SAVE_LST, PRL_LST )    /* Append PRL_LST to PWR_SAVE_LST */
  CPY( TEMP_LST, TBL_LST )
  DEL_SYS( TEMP_LST, NON_CDMA )
  AVOID_CANCEL( TEMP_LST )        /* Cancel all acquisition avoids */
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */

  SRV_OFF_UNTIL( REG )            /* Set service OFF until successful reg */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  //RESEL_STT( SRCH )               /* Indicate that over-reach is active */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Procedure 2-2
  */

  SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
  PWR_SAVE                      /* Tell SS-Client to enter power-save */
  CONT( OVERREACH_SLEEP_J1 )    /* Wait OVERREACH_SLEEP_J1 seconds */

  NEW_ACQ                       /* Start a new acquisition */
  ACQ_MODE( FULL )              /* Do full acquisitions */
  //ACQ_CNT( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL_SYS, 1 )
  #ifdef FEATURE_JCDMA_2
  ACQ_CNT( PWR_SAVE_LST, PREFERRED, SDSSSCR_FULL_SYS, CNT_ALL )
  #else
  ACQ_CNT( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL_SYS, CNT_ALL )
  #endif
                                /* Attemp all the channels in the
                                ** PWR_SAVE_LST */

  PROC_EVT( CDMA_OPR_ACC_TERM ) /* Do access termination processing */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  END
};


/*---------------------------------------------------------------------------
                       SDSSSCR_PWR_SAVE_OVERREACH_2_3
---------------------------------------------------------------------------*/

/* Power-save mode of operation script for JCDMA over-reach condition
** procedure 2-3.
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_overreach_2_3[] = {


  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /* Clear COLLOC list */
  #endif
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acquisition related lists */
  CPY( PWR_SAVE_LST, SRCH_LST )   /* Copy SRCH_LST to PWR_SAVE_LST */
  APP( PWR_SAVE_LST, PRL_LST )    /* Append PRL_LST to PWR_SAVE_LST */
  CPY( TEMP_LST, TBL_LST )
  DEL_SYS( TEMP_LST, NON_CDMA )
  AVOID_CANCEL( TEMP_LST )         /* Cancel all acquisition avoids */
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */

  SRV_OFF_UNTIL( REG )            /* Set service OFF until successful reg */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  //RESEL_STT( SRCH )               /* Indicate that over-reach is active */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Procedure 2-2
  */
  LOOP_FOREVER                    /* Loop forever and: */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /* Tell SS-Client to enter power-save */
    CONT( OVERREACH_SLEEP_J2 )    /* Wait OVERREACH_SLEEP_J1 seconds */

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL )              /* Do full acquisitions */
    //ACQ_CNT( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL_SYS, 1 )
    #ifdef FEATURE_JCDMA_2
    ACQ_CNT( PWR_SAVE_LST, PREFERRED, SDSSSCR_FULL_SYS, CNT_ALL )
    #else
    ACQ_CNT( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL_SYS, CNT_ALL )
    #endif
                                  /* Attemp all the channels in the 
                                  ** PWR_SAVE_LST 
                                  */

  LOOP                            /* Loop back */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  END
};


/*---------------------------------------------------------------------------
                            SDSSSCR_PWR_SAVE_GW_PWRUP
---------------------------------------------------------------------------*/
/* Power-save mode of operation script for GSM/UMTS target when UE powers up
** and directly enters in power save mode.
** It looks for full followed by limited service on the alternative band in
** each wake up cycles.
** The deep sleep cycles are 30 seconds for the first 10 cycles, then 45
** seconds for the second 10 cycles, then 60 seconds for each cycle and for
** ever.
*/

static const sdssscr_elem_u_type  ssscr_called_pwr_save_gw_pwrup[] = {


  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #else
  PREV_SCR
  #endif /* FEATURE_GSM || FEATURE_WCDMA */

  END
};


/*---------------------------------------------------------------------------
                            SDSSSCR_PWR_SAVE_GW
---------------------------------------------------------------------------*/
/* Power-save mode of operation script for GSM/UMTS target when UE powers up
** and directly enters in power save mode.
** It looks for full followed by limited service on the last active band for
** 4 wake up cycles then alternative band in the every 5th wake up cycles.
** The deep sleep cycles are the same as that in sdssscr_pwr_save_gw_pwrup,
** 30 seconds for the first 10 cycles, then 45 seconds for the second 10
** cycles, then 60 seconds for each cycle and for ever.
**
** The limitation of the implementation is that
** If there are three systems in the srch list, the script will never try the
** third one in the list.
*/

static const sdssscr_elem_u_type  ssscr_called_pwr_save_gw[] = {

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #else
  PREV_SCR
  #endif /* FEATURE_GSM || FEATURE_WCDMA */

  END

};


/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_GW_ORIG_PWRUP
---------------------------------------------------------------------------*/

/* Service lost script when UE has never acquired any system since power up
** and makes a E911 call for GSM/UMTS target.
** Attempt for full service on alternative bands during call origination
** period of time. If failed, attempt on limited services on all the bands
** before return to the previous script.
*/

static const sdssscr_elem_u_type  ssscr_called_srv_lost_gw_emerg_call_orig_pwrup[] = {

  CLR( ACQ_FULL2_SYS )            /* Clear FULL2 to start from beginning*/
  CLR( ACQ_MICRO2_SYS )           /* Clear last MICRO acquisition system  */
  ORDER2( GW_LTD_LST, SRCH_LST, BAND ) 
                                  /* Order by SRCH_LST */
  CPY( PWR_SAVE_LST, SRCH_LST )   /* Copy SRCH_LST to PWR_SAVE_LST */
  RESET_GW_UOOS_ACQ_PARA

  LOOP_FOREVER
    NEW_ACQ                       /* Start a new acquisition */
    CLR( GW_LAST_LTD_SYS )        /* Clear GW LAST LTD system */
    ACQ_MODE( FULL2 )
    ACQ_CNT( SRCH_LST, ANY, SDSSSCR_FULL2_SYS, 1)/* Acquire full services  */
  LOOP

  ACT_SCR( PWR_SAVE_GW_PWRUP )
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Service lost script when UE has never acquired any system since power up
** and makes a call for GSM/UMTS target.
** Attempt for full service on alternative bands during call origination
** period of time. If failed, attempt on limited services on all the bands
** before return to the previous script.
*/

#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
#else
static const sdssscr_elem_u_type  ssscr_called_srv_lost_gw_orig_pwrup[] = {

  CLR( ACQ_FULL2_SYS )            /* Clear FULL2 to start from beginning*/
  CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
  ORDER2( GW_LTD_LST, SRCH_LST, BAND ) 
                                  /* Order by SRCH_LST */

  LOOP_TIME(SD_PWR_SAVE_ENTER_TIME_DURING_CALL) 
                                  /* Loop until timer expire.*/

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL2 )
    ACQ_CNT( SRCH_LST, ANY, SDSSSCR_FULL2_SYS, 1 )
                           /* Acquire full services with alternated systems*/
    SRV_STATUS( NO_SRV )   /* Turn off service status */
    ACQ_FAIL               /* Indicate acquisition fail */

  LOOP                         /* Loop back */
  ACQ(  GW_LTD_LST, ANY )   /* Try acquiring limited GW service */
  PREV_SCR
  END
};
#endif

/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_GW_EMERG_ORIG
---------------------------------------------------------------------------*/
/* Service lost script after UE lost the service and makes a call for
** GSM/UMTS target.
** Attempt for full service on the last active bands during call origination
** period of time. If failed, one attempt for limited service on the last
** active band before returning to the previous script.
*/

static const sdssscr_elem_u_type  ssscr_called_srv_lost_gw_emerg_call_orig[] = {

  CLR( GW_LAST_LTD_SYS )       /* Clear GW_LAST_LTD_SYS */

  RESET_GW_UOOS_ACQ_PARA
  LOOP_FOREVER /* Loop until timer expire.*/
    CLR( ACQ_FULL2_SYS )          /* Clear FULL2 and start from beginning */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
    ACQ_MODE( FULL )
    NEW_ACQ                    /* Start a new acquisition */
    ACQ_CNT( SRCH_LST, ANY, 0, 1 )
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( SRCH_LST, ANY, 0, 1 )/* Acquire the last active system */
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( SRCH_LST, ANY, 0, 1 )/* Acquire the last active system */
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL2 )             /* Set acq mode to FULL2 */
    ACQ_CNT( SRCH_LST, ANY, SDSSSCR_FULL2_SYS, 1 )
                                /* Acquire the last active system */
    NEW_ACQ
    ACQ_CNT( SRCH_LST, ANY, SDSSSCR_FULL2_SYS, 1 )
  LOOP

  ACT_SCR( PWR_SAVE_GW )
  END
};
 
/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_GW_ORIG
---------------------------------------------------------------------------*/
/* Service lost script after UE lost the service and makes a call for
** GSM/UMTS target.
** Attempt for full service on the last active bands during call origination
** period of time. If failed, one attempt for limited service on the last
** active band before returning to the previous script.
*/

static const sdssscr_elem_u_type  ssscr_called_srv_lost_gw_orig[] = {

  CLR( GW_LAST_LTD_SYS )       /* Clear GW_LAST_LTD_SYS */

  #ifdef FEATURE_WLAN_UNIFORM_SCAN_OOS
#error code not present
  #endif

  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  ACQ_MODE( FULL )
  LOOP_TIME(SD_PWR_SAVE_ENTER_TIME_DURING_CALL) /* Loop until timer expire.*/
  #endif

  NEW_ACQ                    /* Start a new acquisition */
  ACQ_CNT( SRCH_LST, ANY, 0, 1 )
                             /* Acquire full service on the first system */
  SRV_STATUS( NO_SRV )       /* Turn off service status */
  ACQ_FAIL                   /* Indicate that acquisition fail */

  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  LOOP                         /* Loop back */
  ACQ( GW_LAST_LTD_SYS, ANY )   /* Try acquiring limited GW service */
  #endif

  PREV_SCR
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_GW_PWRUP
---------------------------------------------------------------------------*/
/* Service lost script when UE has never acquire service since power up on
** GSM/UMTS target.
** Attempt for full service followed by the limited on all the bands before
** returning to the previous script.
*/

#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
#else

static const sdssscr_elem_u_type  ssscr_called_srv_lost_gw_pwrup[] = {

  CLR( ACQ_FULL2_SYS )          /* Clear FULL2 and start from the beginning*/
  CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
  NEW_ACQ

  LOOP_LIST( SRCH_LST )         /* Loop for count of SRCH LIST time. */

    CLR( GW_LAST_LTD_SYS )      /* Clear GW_LAST_LTD_SYS */
    ACQ_MODE( FULL2 )
    ACQ_CNT( SRCH_LST, ANY, SDSSSCR_FULL2_SYS, 1 )
                                /* Acquire full services on the alternated
                                ** systems */
    ACQ_MODE( MINI )            /* Switch to the mode other than FULL2 */
    ACQ( GW_LAST_LTD_SYS, ANY ) /* Try acquiring limited GW service */
    SRV_STATUS( NO_SRV )        /* Turn off service status */
    ACQ_FAIL                    /* Indicate the acquisition failure */

  LOOP                          /* Loop back */

  PREV_SCR
  END
};

#endif /* FEATURE_UMTS_UNIFORM_OOS_HANDLING */

/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_GW
---------------------------------------------------------------------------*/
/* Service lost script after service lost on GSM/UMTS target.
** Attempt for full service followed by the limited on the last acquired band
** before returning to the previous script.
*/

static const sdssscr_elem_u_type  ssscr_called_srv_lost_gw[] = {

  NEW_ACQ
  CLR( GW_LAST_LTD_SYS )        /* Clear GW_LAST_LTD_SYS */

  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  ACQ_MODE( FULL )
  #endif
  
  #ifdef FEATURE_WLAN_UNIFORM_SCAN_OOS
#error code not present
  #endif
  
  ACQ_CNT( SRCH_LST, ANY, 0, 1 )/* Acquire full services on the first system
                                */
  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  ACQ( GW_LAST_LTD_SYS, ANY ) /* Try acquiring limited GW service */
  #endif

  SRV_STATUS( NO_SRV )          /* Turn off service */
  ACQ_FAIL                      /* Indicate the acquisition failure */

  PREV_SCR
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_GW_SCHED
---------------------------------------------------------------------------*/
//
///* Schedule to acquire the last full and limited gsm/wcdma systems. */
//static const sdssscr_elem_u_type  ssscr_called_gw_sched[] = {
//  /* Script */
//  SCHED( GW_LAST_FULL_SYS, REACQ_GW_FULL_ACQ_LOST, ALLOWED )
//                                  /* Schedule GW last full system reacq */
//  SCHED( GW_LAST_LTD_SYS, REACQ_GW_LTD_ACQ_LOST, ANY )
//                                  /* Schedule GW last LTD system reacq */
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//  PREV_SCR                        /* Continue with previous script */
//  END
//};
//
//
/*---------------------------------------------------------------------------
                            SDSSSCR_GW_SCHED_ORIG
---------------------------------------------------------------------------*/
//
///* Schedule to acquire last full gsm/wcdma system in the origination. */
//static const sdssscr_elem_u_type  ssscr_called_gw_sched_orig[] = {
//
//  SCHED(GW_LAST_FULL_SYS, REACQ_GW_FULL_ORIG, ANY)
//                                  /* Schedule GW last full system reacq */
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//  PREV_SCR                        /* Continue with previous script */
//  END
//};
//
/*---------------------------------------------------------------------------
                            SDSSSCR_GW_RESCHED
---------------------------------------------------------------------------*/
//
///* Reschedule to acquire last full and limited gsm/wcdma system. */
//static const sdssscr_elem_u_type  ssscr_called_gw_resched[] = {
//
//  CPY( SRCH_LST, SCHED_LST )      /* Clear the srch list and copy it from
//                                  ** schedule list. */
//  SORT( SRCH_LST, GW  )           /* GW systems becomes the first systems in
//                                  ** the srch list. */
//  CPY( TEMP_SYS, SRCH_LST )       /* TEMP sys is the last acquired gw full
//                                  ** system. */
//  CLR( SCHED_LST )                /* Clear SCHED list */
//
//  SCHED( TEMP_SYS, REACQ_GW_FULL_ACQ_LOST, ALLOWED )
//                                  /* Schedule GW last full system reacq */
//  CREATE_GW_LTD( TEMP_SYS )       /* Create a counterpart of limited system*/
//
//  SCHED( TEMP_SYS, REACQ_GW_LTD_ACQ_LOST, ANY )
//                                  /* Schedule GW last LTD system reacq */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//  PREV_SCR                        /* Continue with previous script */
//  END
//
//};

/*---------------------------------------------------------------------------
                            SDSSSCR_GW_RESCHED
---------------------------------------------------------------------------*/
//
///* Reschedule to acquire last full gsm/wcdma system in the origination. */
//static const sdssscr_elem_u_type  ssscr_called_gw_resched_orig[] = {
//
//  DEL( SCHED_LST, GW_LTD_LST )    /* Delete GW limited list from schedule
//                                  ** list. */
//  CPY( SRCH_LST, SCHED_LST )      /* Clear the srch list and copy it from
//                                  ** schedule list. */
//  DEL_SYS( SCHED_LST, GW_SYS )    /* Delete GW system from schedule list */
//  SORT( SRCH_LST, GW  )           /* Sorting the srch list with GW system
//                                  ** first. */
//  CPY( TEMP_SYS, SRCH_LST )       /* Clear the temp sys and copy it with
//                                  ** the first system from schedule list. */
//  SCHED( TEMP_SYS, REACQ_GW_FULL_ORIG, ANY  )
//                                  /*Schedule the last full gsm/wcdma system*/
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  PREV_SCR                        /* Continue with previous script */
//  END
//
//};


/*---------------------------------------------------------------------------
                            SDSSSCR_WLAN_PWR_SAVE_MODE_PREF
---------------------------------------------------------------------------*/

/* Power-save script for WLAN system.
*/
static const sdssscr_elem_u_type  ssscr_called_wlan_pwr_save_mode_pref[] = {

  /* Enter power save.
  */
  REDIR_OFF                       /* Set redirection indication to off */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acquisition related lists */

  SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
  PWR_SAVE                        /* Tell SS-Client to enter power-save */

  CONT(0)                         /* Continue with Power-Save */

  END
};




/*---------------------------------------------------------------------------
                            SDSSSCR_WLAN_PWR_SAVE_MANUAL
---------------------------------------------------------------------------*/

#ifdef FEATURE_WLAN_UNIFORM_SCAN_OOS
#error code not present
#else

/* Power-save script for WLAN system.
*/
static const sdssscr_elem_u_type  ssscr_called_wlan_pwr_save_manual[] = {

  #ifdef FEATURE_WLAN
#error code not present
  #else
  PREV_SCR
  #endif /* FEATURE_WLAN */
  END

};

#endif

/*---------------------------------------------------------------------------
                            SDSSSCR_WLAN_PWR_SAVE
---------------------------------------------------------------------------*/


#ifdef FEATURE_WLAN_UNIFORM_SCAN_OOS
#error code not present
#else
/* Power-save script for WLAN system.
*/
static const sdssscr_elem_u_type  ssscr_called_wlan_pwr_save[] = {

  #ifdef FEATURE_WLAN
#error code not present
  #else
  PREV_SCR
  #endif /* FEATURE_WLAN */
  END

};

#endif

/*---------------------------------------------------------------------------
                            SDSSSCR_HDR_ACQ_ROTATE
---------------------------------------------------------------------------*/

/* Rotating HDR acquisitions
*/
static const sdssscr_elem_u_type  ssscr_called_hdr_acq_rotate[] = {

  /* FULL ACQ on MRU[0,1]
  ** FULL ACQ on 2 pseudo randomly selected channels
  ** Micro ACQ on the remaining channels
  */

  CPY( SRCH_LST, SRV_COLLOC_LST )         /* Copy COLLOC list into SRCH list       */
  DEL_SYS( SRCH_LST, NON_HDR )            /* Delete non-HDR systems from SRCH list */
  ORDER( SRCH_LST, MRU_LST )              /* Order SRCH list according to MRU      */
    NEW_ACQ                                 /* Start New ACQ                         */
  ACQ_MODE( FULL )                        /* Do FULL acquisitions                  */
  ACQ_CNT( SRCH_LST, COLLOC_PREF, 0 , 2 ) /* FULL acq on MUR[0,1]                  */

  ACQ_MODE( FULL2 )                       /* Do FULL2 acquisitions                  */
  ACQ_CNT( SRCH_LST, COLLOC_PREF, SDSSSCR_FULL2_SYS, 2 )

  ACQ_MODE( MICRO )                       /* Do micro acquisitions on remaining    */
  ACQ( SRCH_LST, COLLOC_PREF )

    ACQ_FAIL                      
    SRV_STATUS( PWR_SAVE )                /* Change the status to Power Save */

  PREV_SCR
  END

};

/*---------------------------------------------------------------------------
                            SDSSSCR_HDR_ACQ_ROTATE_SRV_LOST
---------------------------------------------------------------------------*/

/* Rotating HDR acquisitions
*/
static const sdssscr_elem_u_type  ssscr_called_hdr_acq_rotate_srv_lost[] = {

  /* FULL ACQ on MRU[0,1]
  ** FULL ACQ on 2 pseudo randomly selected channels
  ** Micro ACQ on the remaining channels
  */

  CPY( SRCH_LST, SRV_COLLOC_LST )         /* Copy COLLOC list into SRCH list       */
  DEL_SYS( SRCH_LST, NON_HDR )            /* Delete non-HDR systems from SRCH list */
  ORDER( SRCH_LST, MRU_LST )              /* Order SRCH list according to MRU      */
  ACQ_MODE( FULL )                        /* Do FULL acquisitions                  */
  ACQ_CNT( SRCH_LST, COLLOC_PREF, 0 , 2 ) /* FULL acq on MUR[0,1]                  */

  DEL_CNT( SRCH_LST, 0, 2 )               /* Delete top 2 sys from SRCH list   */

  ACQ_MODE( FULL2 )                       /* Do FULL2 acquisitions                  */
  ACQ_CNT( SRCH_LST, COLLOC_PREF, SDSSSCR_FULL2_SYS, 2 )

  ACQ_MODE( MICRO )                       /* Do micro acquisitions on remaining    */
  ACQ( SRCH_LST, COLLOC_PREF )

  NEW_ACQ
  SRV_STATUS( NO_SRV )          /*   6. Set service status to NO_SRV  */
  ACQ_FAIL                      /*   7. Indicate that acquisition failed */

  PREV_SCR
  END

};


/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_HDR_CDMA_HO
---------------------------------------------------------------------------*/

/* Normal service lost script. This script is used when service is declared
** lost and acquisition is to be started from scratch.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_hdr_cdma_ho[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */

  #if (defined FEATURE_JCDMA)
  CHK_SRV_STATUS_AND_SET( PWR_SAVE, NO_SRV )  /* Check if we are in pwr save,
                                              ** if we are then set service
                                              ** status to NO_SRV */
  #else
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  #endif

  REDIR_OFF                       /* Set redirection indication to OFF */
  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_GW_SET_PWR_SAVE_TIMER
---------------------------------------------------------------------------*/
#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING 
#error code not present
#endif

#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING 
#error code not present
#endif

/*lint +e708 */ /* Info 708 union initialization */


/* <EJECT> */
/*---------------------------------------------------------------------------
                            Called Scripts Table
---------------------------------------------------------------------------*/

/* This table enables SS-Script to activate other scripts.
**
** Note that this table is implemented as an array that is being indexed by
** sdssscr_e_type enum values. In other words, the
** dsssscr_called_tbl[ SDSSSCR_XXX ] entry reference the SS-Script that is
** associated with the SDSSSCR_XXX enum value.
*/

static const sdssscr_elem_u_type * const dsssscr_called_tbl[SDSSSCR_MAX] = {

                                      /* 0 */
  ssscr_called_clr_acq_lsts,          /* SDSSSCR_CLR_ACQ_LSTS */
  ssscr_called_clr_acq_sched_lsts,    /* SDSSSCR_CLR_ACQ_SCHED_LSTS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_called_srv_lost_norm,         /* SDSSSCR_SRV_LOST_NORM */
  ssscr_called_srv_lost_norm_slnt,    /* SDSSSCR_SRV_LOST_NORM_SLNT */

                                      /* 4 */
  ssscr_called_srv_lost_norm_srv_off_until_oh,
  ssscr_called_wlan_srv_lost_norm,    /* SDSSSCR_WLAN_SRV_LOST_NORM */
  ssscr_called_wlan_srv_lost_norm_slnt,
                                      /* SDSSSCR_WLAN_SRV_LOST_NORM_SLNT */
  ssscr_called_srv_lost_skip,         /* SDSSSCR_SRV_LOST_SKIP */

                                      /* 8 */
  ssscr_called_srv_lost_skip_silent,  /* SDSSSCR_SRV_LOST_SKIP_SLNT */
  ssscr_called_srv_lost_skip_acq,     /* SDSSSCR_SRV_LOST_SKIP_ACQ */
  ssscr_called_srv_lost_skip_idl,     /* SDSSSCR_SRV_LOST_SKIP_IDL */
  ssscr_called_srv_lost_skip_srv_off_until_oh,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                      /* 12 */
  ssscr_called_sys_acc_fail_prl,      /* SDSSSCR_SYS_ACC_FAIL_PRL */
  ssscr_called_sys_acc_fail_prl_amps, /* SDSSSCR_SYS_ACC_FAIL_PRL_AMPS */
  ssscr_called_sys_acc_fail_avail,    /* SDSSSCR_SYS_ACC_FAIL_AVAIL */
  ssscr_called_sys_acc_fail_avail_amps,/*SDSSSCR_SYS_ACC_FAIL_AVAIL_AMPS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                      /* 16 */
  ssscr_called_resel_jcdma,           /* SDSSSCR_RESEL_JCDMA */
  ssscr_called_resel_dm,              /* SDSSSCR_RESEL_DM */
  ssscr_called_resel_init,            /* SDSSSCR_RESEL_INIT */
  ssscr_called_resel_norm,            /* SDSSSCR_RESEL_NORM */

                                      /* 20 */
  ssscr_called_resel_full_srv,        /* SDSSSCR_RESEL_FULL_SRV */
  ssscr_called_resel_hybr,            /* SDSSSCR_RESEL_HYBR */
  ssscr_called_resel_gw_full_srv,     /* SDSSSCR_RESEL_GW_FULL_SRV */
  ssscr_called_resel_full_srv_manual, /* SDSSSCR_RESEL_FULL_SRV_MANUAL */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                      /* 24 */
  ssscr_called_pwr_save,              /* SDSSSCR_PWR_SAVE */
  #ifdef FEATURE_UNIFORM_SCAN_OOS
  ssscr_called_pwr_save_hdr,          /* SDSSSCR_PWR_SAVE_HDR */
  ssscr_called_pwr_save_non_hybr_digital_mode, 
  #else
  ssscr_called_reserved,              /* RESERVED */
  ssscr_called_reserved,              /* RESERVED */
  #endif  

  #if (defined(FEATURE_UNIFORM_SCAN_OOS) && defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING))
#error code not present
  #else
  ssscr_called_reserved,              /* RESERVED */
  ssscr_called_reserved,              /* RESERVED */
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_called_pwr_save_colloc_1sys,  /* SDSSSCR_PWR_SAVE_COLLOC_1SYS */
  ssscr_called_pwr_save_colloc,       /* SDSSSCR_PWR_SAVE_COLLOC */
  ssscr_called_pwr_save_colloc_deep,  /* SDSSSCR_PWR_SAVE_COLLOC_DEEP */

                                      /* 32 */
  ssscr_called_pwr_save_colloc_shallow,/*SDSSSCR_PWR_SAVE_COLLOC_SHALLOW */
  ssscr_called_pwr_save_rx_gt_99,     /* SDSSSCR_PWR_SAVE_RX_GT_99 */
  ssscr_called_pwr_save_jcdma_1_2_b,  /* SDSSSCR_PWR_SAVE_JCDMA_1_2_B */
  ssscr_called_pwr_save_jcdma_1_3_b,  /* SDSSSCR_PWR_SAVE_JCDMA_1_3_B */

                                      /* 36 */
  ssscr_called_pwr_save_overreach_2_2,/* SDSSSCR_PWR_SAVE_OVERREACH_2_2 */
  ssscr_called_pwr_save_overreach_2_3,/* SDSSSCR_PWR_SAVE_OVERREACH_2_3 */

  ssscr_called_pwr_save_gw_pwrup,     /* SDSSSCR_PWR_SAVE_GW_PWRUP */
  ssscr_called_pwr_save_gw,           /* SDSSSCR_PWR_SAVE_GW */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                      /* 40 */
  ssscr_called_srv_lost_gw_emerg_call_orig_pwrup, 
  ssscr_called_srv_lost_gw_orig_pwrup, /* SDSSSCR_SRV_LOST_GW_ORIG_PWRUP */
  ssscr_called_srv_lost_gw_emerg_call_orig, 
                                      /* SDSSSCR_SRV_LOST_EMERG_CALL_GW_ORIG */
  ssscr_called_srv_lost_gw_orig,      /* SDSSSCR_SRV_LOST_GW_ORIG */

                                      /* 44 */
  ssscr_called_srv_lost_gw_pwrup,     /* SDSSSCR_SRV_LOST_GW_PWRUP */
  ssscr_called_srv_lost_gw,           /* SDSSSCR_SRV_LOST_GW */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ssscr_called_wlan_pwr_save_mode_pref, 
                                      /* SDSSSCR_WLAN_PWR_SAVE_MODE_PREF */
  ssscr_called_wlan_pwr_save_manual,  /* SDSSSCR_WLAN_PWR_SAVE_MANUAL */

                                      /* 48 */
  ssscr_called_wlan_pwr_save,         /* SDSSSCR_WLAN_PWR_SAVE */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ssscr_called_hdr_acq_rotate,        /* SDSSSCR_HDR_ACQ_ROTATE */

  ssscr_called_hdr_acq_rotate_srv_lost,
                                      /* SDSSSCR_HDR_ACQ_ROTATE_SRV_LOST */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_UNIFORM_SCAN_OOS
  ssscr_called_uniform_colloc_call_orig_no_1x_srv, 
 
                                      /* SDSSSCR_UNIFORM_COLLOC_CALL_ORIG_NO_1X_SRV */

                                      /* 52 */
  ssscr_called_uniform_scan_cont_pwr_save_lst, 
                                      /* SDSSSCR_UNIFORM_SCAN_CONT_PWR_SAVE_LST */
  ssscr_called_uniform_scan_cont_srch_lst, 
                                      /* SDSSSCR_UNIFORM_SCAN_CONT_SRCH_LST */

  ssscr_called_uniform_scan_cont_hdr, /* SDSSSCR_UNIFORM_SCAN_CONT_HDR */
  #else
  ssscr_called_reserved,              /* RESERVED */
  ssscr_called_reserved,              /* RESERVED */
  ssscr_called_reserved,              /* RESERVED */
  ssscr_called_reserved,              /* RESERVED */
  #endif

  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  ssscr_called_reserved,              /* RESERVED */  
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                      /* 56 */
  ssscr_called_srv_lost_hdr_cdma_ho,  /* SDSSSCR_SRV_LOST_HDR_CDMA_HO */
  ssscr_called_chk_and_clr_acq_sys,   /* SDSSSCR_CHK_AND_CLR_ACQ_SYS */

  #ifdef FEATURE_WLAN_UNIFORM_SCAN_OOS
#error code not present
  #else
  ssscr_called_reserved,              /* RESERVED */  
  ssscr_called_reserved,              /* RESERVED */  
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #else
  ssscr_called_reserved,
  ssscr_called_reserved,              /* RESERVED */  
  ssscr_called_reserved              /* RESERVED */
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                      
  
};


/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_nv_items_init

DESCRIPTION
  Read the various configurable SD values that are stored in NV. Use defaults
  when items not available in NV.

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdssscr_nv_items_init( void )
{
  boolean is_write_back = FALSE;
  /*lint -save -e826 sd_cfg_items_nv Suspicious pointer-to-pointer conversion
  */
  nv_sd_cfg_items_s_type sd_cfg_items_nv = sd_cfg_items_default;
  unsigned int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure we are not trying to read more that what's available. */
  SD_ASSERT( SDSSSCR_NV_ITEM_MAX <= NV_SD_ITEMS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !sdnv_read(NV_SD_CFG_ITEMS_I, (nv_item_type*) &sd_cfg_items_nv) )
  {
    /* sdnv_read() prints error message in case of failure. So don't repeat
    ** it here. Just fallback to default values.
    */
    sd_cfg_items_nv = sd_cfg_items_default;

    is_write_back = TRUE;
  }

  /* If the version read from NV is not the expected one then fallback to
  ** default values.
  */
  if( sd_cfg_items_nv.version != sd_cfg_items_default.version )
  {
    SD_MSG_ERROR( "Cfg items version mismatch!",0,0,0 );

    /* Copy the default values to sd_cfg_items_nv.
    */
    sd_cfg_items_nv = sd_cfg_items_default;

    is_write_back = TRUE;
  }

  /* Values for newly added configurable items missing in NV. Use default for
  ** just the new items instead of resetting the whole table to the default
  ** values.
  */
  if( sd_cfg_items_nv.count < sd_cfg_items_default.count)
  {
    for(i = sd_cfg_items_nv.count; i < sd_cfg_items_default.count &&
                                   i < ARR_SIZE(sd_cfg_items_nv.items); i++)
    {
      sd_cfg_items_nv.items[i] = sd_cfg_items_default.items[i];
    }
    sd_cfg_items_nv.count = MIN(sd_cfg_items_default.count, ARR_SIZE(sd_cfg_items_nv.items));

    is_write_back = TRUE;
  }

  /* Copy final SD values into local structure (different data type).
  */
  sd_cfg_items.version = sd_cfg_items_nv.version;
  sd_cfg_items.count = sd_cfg_items_nv.count;
  for(i=0; i < sd_cfg_items_nv.count &&
           i < ARR_SIZE(sd_cfg_items.items); i++)
  {
    sd_cfg_items.items[i] = sd_cfg_items_nv.items[i];
  }

  #if (defined (FEATURE_UNIFORM_SCAN_OOS) && defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING))
#error code not present
  #elif defined( FEATURE_UMTS_UNIFORM_OOS_HANDLING )
#error code not present
  #elif defined ( FEATURE_UNIFORM_SCAN_OOS )
  /* 1X + HDR build
  */
  if( sd_cfg_items.items[SDSSSCR_NV_ITEM_TMR_UOOS_SLEEP] == 0 && 
      sd_cfg_items.items[SDSSSCR_NV_ITEM_TMR_UOOS_SCAN] == 0 )
  #endif
  {
    #if (defined (FEATURE_UNIFORM_SCAN_OOS) || defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING))
    SD_MSG_HIGH( "Invalid UOOS timers, Writing default to NV",0,0,0 );
    sd_cfg_items.items[ SDSSSCR_NV_ITEM_TMR_UOOS_UMTS_SCAN ] = 
    sd_cfg_items_nv.items[ SDSSSCR_NV_ITEM_TMR_UOOS_UMTS_SCAN ] = 
        sd_cfg_items_default.items[ SDSSSCR_NV_ITEM_TMR_UOOS_UMTS_SCAN ];

    sd_cfg_items.items[ SDSSSCR_NV_ITEM_TMR_UOOS_GSM_SCAN ] = 
    sd_cfg_items_nv.items[ SDSSSCR_NV_ITEM_TMR_UOOS_GSM_SCAN ] = 
        sd_cfg_items_default.items[ SDSSSCR_NV_ITEM_TMR_UOOS_GSM_SCAN ];

    sd_cfg_items.items[ SDSSSCR_NV_ITEM_TMR_UOOS_GW_TOTAL_SCAN ] = 
    sd_cfg_items_nv.items[ SDSSSCR_NV_ITEM_TMR_UOOS_GW_TOTAL_SCAN ] = 
        sd_cfg_items_default.items[ SDSSSCR_NV_ITEM_TMR_UOOS_GW_TOTAL_SCAN ];

    sd_cfg_items.items[ SDSSSCR_NV_ITEM_TMR_UOOS_GW_SLEEP ] = 
    sd_cfg_items_nv.items[ SDSSSCR_NV_ITEM_TMR_UOOS_GW_SLEEP ] = 
        sd_cfg_items_default.items[ SDSSSCR_NV_ITEM_TMR_UOOS_GW_SLEEP ];

    sd_cfg_items.items[ SDSSSCR_NV_ITEM_TMR_UOOS_SCAN ] = 
    sd_cfg_items_nv.items[ SDSSSCR_NV_ITEM_TMR_UOOS_SCAN ] = 
        sd_cfg_items_default.items[ SDSSSCR_NV_ITEM_TMR_UOOS_SCAN ];

    sd_cfg_items.items[ SDSSSCR_NV_ITEM_TMR_UOOS_SLEEP ] = 
    sd_cfg_items_nv.items[ SDSSSCR_NV_ITEM_TMR_UOOS_SLEEP ] = 
        sd_cfg_items_default.items[ SDSSSCR_NV_ITEM_TMR_UOOS_SLEEP ];

    is_write_back = TRUE;
    #endif
  }

  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING_PWR_UP_ENH
#error code not present
  #endif

  if( is_write_back )
  {
    SD_MSG_HIGH( "Writing default cfg items to NV",0,0,0 );
    (void)sdnv_write(NV_SD_CFG_ITEMS_I, (nv_item_type*) &sd_cfg_items_nv);
  }
}/* sdssscr_nv_items_init */
/*lint -restore */

/*===========================================================================

FUNCTION sdssscr_nv_item_get

DESCRIPTION
  Read the NV value for the specified item

DEPENDENCIES
  None.

RETURN VALUE
  The value of the NV item

SIDE EFFECTS
  None.

===========================================================================*/

MMEXTN  uint32                    sdssscr_nv_item_get(

        sdssscr_nv_item_e_type    nv_item
            /* The NV item to read
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( nv_item, SDSSSCR_NV_ITEM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( nv_item >= SDSSSCR_NV_ITEM_MAX )
  {
    return 0;
  }

  return (sd_cfg_items.items[nv_item]);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_get_ref

DESCRIPTION
  Get a reference to the script that is associated with a specified SS-Event
  and table entry number.

DEPENDENCIES
  None.

RETURN VALUE
  Reference to the script that is associated with the input event and table
  entry number. SDSSSCR_REF_NULL if no such script is found.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdssscr_ref_type          sdssscr_get_ref(

        sdss_evt_grp_e_type       event_group,
            /* Event group which to get the script reference.
            */

        int                       entry
            /* Entry number for which get the script reference.
            */
)
{ /*lint -save -e826 (sdssscr_ref_type*)Suspicious ptr-to-ptr converstion */
  SD_ASSERT_ENUM_IS_INRANGE( event_group, SDSS_EVT_GRP_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( event_group )
  {
    case SDSS_EVT_GRP_USER:
    case SDSS_EVT_GRP_HYBR_1_USER:
      if( entry < (int) ARR_SIZE(dsssscr_user_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_user_event_tbl[ entry ];
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_MISC:
      if( entry < (int) ARR_SIZE(dsssscr_misc_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_misc_event_tbl[ entry ];
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_INT:
      if( entry < (int) ARR_SIZE(dsssscr_int_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_int_event_tbl[ entry ];
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_CDMA_ACQ:
      #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
      if( entry < (int) ARR_SIZE(dsssscr_cdma_acq_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_cdma_acq_event_tbl[ entry ];
      }
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_CDMA_OPR:
      #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
      if( entry < (int) ARR_SIZE(dsssscr_cdma_opr_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_cdma_opr_event_tbl[ entry ];
      }
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_AMPS_ACQ:
      #ifdef FEATURE_ACP
#error code not present
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_AMPS_OPR:
      #ifdef FEATURE_ACP
#error code not present
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_HDR_USER:
      #ifdef FEATURE_HDR
#error code not present
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_HDR_ACQ:
      #ifdef FEATURE_HDR
#error code not present
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_HDR_OPR:
      #ifdef FEATURE_HDR
#error code not present
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_GW_ACQ:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_GW_OPR:
      #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
      #endif
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_WLAN_USER:
      #ifdef FEATURE_WLAN
#error code not present
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_WLAN_ACQ:
      #ifdef FEATURE_WLAN
#error code not present
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_WLAN_OPR:
      #ifdef FEATURE_WLAN
#error code not present
      #endif
      break;



    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_CALLED:
    case SDSS_EVT_GRP_MAX:
    default:
      SD_CASE_ERR( "event_group",0,0,0 );
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no script is associated with the input event group and
  ** table entry - return SDSSSCR_REF_NULL.
  */
  return SDSSSCR_REF_NULL;

} /* sdssscr_get_ref */
/*lint -restore */



/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_called_get_ref

DESCRIPTION
  Given a called SS-Script name, return a reference to the script that is
  referenced by this name. If no SS-Script is referenced by this name, return
  SDSSSCR_REF_NULL.

DEPENDENCIES
  None.

RETURN VALUE
  Reference to a called SS-Script if the input SS-Script name references an
  SS-Script. SDSSSCR_REF_NULL, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdssscr_ref_type          sdssscr_called_get_ref(

        sdssscr_e_type            scr_name
            /* Script name for which to return the referenced SS-Script.
            */
)
{
  /*lint -save -e826 (sdssscr_ref_type*)Suspicious ptr-to-ptr converstion */
  SD_ASSERT( scr_name < (int) ARR_SIZE(dsssscr_called_tbl) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( scr_name < (int) ARR_SIZE(dsssscr_called_tbl) )
  {
    return  (sdssscr_ref_type*)   dsssscr_called_tbl[ scr_name ];
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDSSSCR_REF_NULL;

} /* sdssscr_called_get_ref */
/*lint -restore */



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
====================== SYSTEM SELECTION SCRIPT PARSER =======================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdssscr_parser_init

DESCRIPTION
  Initialize a script-parser with a new SS-Script.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdssscr_parser_init(

        sdssscr_parser_s_type     *parser_ptr,
            /* Pointer to a script-parser item to be initialized with the
            ** SS-Script that is pointed to by the scr_ptr parameter.
            */

        sdssscr_ref_type          scr_ref,
            /* Reference to an SS-Script with which to initialize the
            ** script-parser that is pointed to by the parser_ptr parameter.
            */

        sdss_evt_grp_e_type       event_group,
        int                       scr_tbl_idx,
            /* The event group and script table index (for debugging).
            */

        sd_ss_e_type              ss
            /* System selection with which this parser is associated.
            */
)
{
  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( scr_ref != SDSSSCR_REF_NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event_group, SDSS_EVT_GRP_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //SD_FUNC_START( "sdssscr_parser_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  parser_ptr->scr_ref       = scr_ref;
  parser_ptr->scr_evt_grp   = event_group;
  parser_ptr->scr_tbl_idx   = scr_tbl_idx;

  parser_ptr->scr_idx       = 0;
  parser_ptr->scr_line      = 0;

  parser_ptr->loop_type     = SDSSSCR_CON_MAX;
  parser_ptr->loop_idx      = 0;
  parser_ptr->loop_line     = 0;
  parser_ptr->loop_val      = 0;

  parser_ptr->loop_back_cnt = 0;

  parser_ptr->if_idx        = 0;
  parser_ptr->if_line       = 0;
  parser_ptr->if_val        = FALSE;
  parser_ptr->else_idx      = 0;
  parser_ptr->else_line     = 0;
  parser_ptr->endif_idx     = 0;
  parser_ptr->endif_line    = 0;

  parser_ptr->ss            = ss;

} /* sdssscr_parser_init */




/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_parser_get

DESCRIPTION
  Extract the script-construct that is reference by the current script index,
  along with the construct's associated parameters.

DEPENDENCIES
  The script-parser that is pointed to by parser_ptr parameter must have
  already been associated with a script via a call to sdssscr_parser_init().

RETURN VALUE
  The index delta from the current script construct to the next script
  construct. 0 if current script index is referencing the last
  script-construct (i.e., SDSSSCR_CON_END).

SIDE EFFECTS
  None

===========================================================================*/
static  int                             sdssscr_parser_get(

        const sdssscr_parser_s_type     *parser_ptr,
            /* Pointer to the script-parser that is associated with the
            ** script from which to get the next script-construct.
            */

        sdssscr_con_e_type              *con_ptr,
            /* Pointer to an item where to store the returned script
            ** construct
            */

        sdssscr_prm_type                *prm1_ptr,
            /* Pointer to an item where to store the 1st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type                *prm2_ptr,
            /* Pointer to an item where to store the 2st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type                *prm3_ptr,
            /* Pointer to an item where to store the 3st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type                *prm4_ptr,
            /* Pointer to an item where to store the 4st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type                *prm5_ptr,
            /* Pointer to an item where to store the 5th parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type                *prm6_ptr
            /* Pointer to an item where to store the 6th parameter that is
            ** associated with the returned script-construct (if any).
            */
)
{
  sdssscr_elem_u_type   *scr_ptr;
  int                   idx_delta = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //SD_FUNC_START( "sdssscr_parser_get()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( con_ptr != NULL );
  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( prm1_ptr != NULL );
  SD_ASSERT( prm2_ptr != NULL );
  SD_ASSERT( prm3_ptr != NULL );
  SD_ASSERT( prm4_ptr != NULL );
  SD_ASSERT( parser_ptr->scr_ref != SDSSSCR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the script that is being parsed and extract the script
  ** construct that is being referenced by the current script index.
  */
  scr_ptr     = (sdssscr_elem_u_type*)  parser_ptr->scr_ref;
  *con_ptr    = (sdssscr_con_e_type)    scr_ptr[ parser_ptr->scr_idx ].con;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract potential construct's parameters.
  */
  *prm1_ptr   = scr_ptr[ parser_ptr->scr_idx+1 ].prm;
  *prm2_ptr   = scr_ptr[ parser_ptr->scr_idx+2 ].prm;
  *prm3_ptr   = scr_ptr[ parser_ptr->scr_idx+3 ].prm;
  *prm4_ptr   = scr_ptr[ parser_ptr->scr_idx+4 ].prm;
  *prm5_ptr   = scr_ptr[ parser_ptr->scr_idx+5 ].prm;
  *prm6_ptr   = scr_ptr[ parser_ptr->scr_idx+6 ].prm;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Based on the script construct that is being parsed, determine the
  ** index delta to the next construct.
  */
  switch( *con_ptr )
  {

    /* --------------------------------------
    ** List Manipulator SS-Script-Constructs.
    ** --------------------------------------
    */

    case SDSSSCR_CON_CLR:           /* CLR( LIST )                    */
    case SDSSSCR_CON_CREATE_GW_LTD: /* CREATE_GW_LTD( LIST )          */
      idx_delta = 2;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_CPY:           /* CPY( LIST_TO, LIST_FROM )         */
    case SDSSSCR_CON_DEL:           /* DEL( LIST, LIST_REF )             */
    case SDSSSCR_CON_DEL_SYS:       /* DEL_SYS( LIST, DEL_CAT )          */
    case SDSSSCR_CON_DEL_BAND:      /* DEL_SYS( LIST, SYS_REF )          */
    case SDSSSCR_CON_APP:           /* APP( LIST_TO, LIST_FROM )         */
    case SDSSSCR_CON_ORDER:         /* ORDER( LIST, LIST_REF )           */
    case SDSSSCR_CON_SORT:          /* SORT( LIST, COMP_CAT )            */
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_GEO_EXT:       /* GEO_EXT( LIST, LIST_REF, EXT_CAT )*/
    case SDSSSCR_CON_ORDER2:        /* ORDER2( LIST, LIST_REF, ORDER_CAT )*/
    case SDSSSCR_CON_DEL_CNT:
      idx_delta = 4;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* -----------------------------------
    ** List Operator SS-Script-Constructs.
    ** -----------------------------------
    */

    case SDSSSCR_CON_MEAS:          /* MEAS( LIST, MEAS_MODE, MEAS_TYPE ) */
      idx_delta = 4;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACQ:           /* ACQ( LIST, ACQ_TYPE )          */
    case SDSSSCR_CON_ACQ_TRUE_MODE: /* ACQ_TRUE_MODE ( LIST, ACQ_TYPE )    */      
    case SDSSSCR_CON_ACQ_ALL:       /* ACQ_ALL( LIST, ACQ_TYPE )      */
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACQ_CNT: /* ACQ_CNT( LIST, ACQ_TYPE, START_POS, CNT ) */
    case SDSSSCR_CON_ACQ_TIME:/* ACQ_TIME( LIST, ACQ_TYPE, START_POS, TIME )*/
      idx_delta = 5;
      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACQ_TIME2: /* ACQ_TIME2( LIST, ACQ_TYPE, START_POS, NV_ITEM ) */
    case SDSSSCR_CON_ACQ_TIME3:

      if( *prm4_ptr == SDSSSCR_NV_ITEM_TMR_AUTO_UOOS_SCAN )
      {
        #if ( defined (FEATURE_UMTS_UNIFORM_OOS_HANDLING) &&\
              defined(FEATURE_UNIFORM_SCAN_OOS))
#error code not present
        #elif (defined(FEATURE_UNIFORM_SCAN_OOS))
        *prm4_ptr = (sdssscr_prm_type) 
                       ( sd_cfg_items.items[ SDSSSCR_NV_ITEM_TMR_UOOS_SCAN ]);
        #elif (defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING))
#error code not present
        #else
        SD_MSG_ERROR(" Invalid construct",0,0,0);
        *prm4_ptr = 0;
        #endif
      }
      else if( *prm4_ptr == SDSSSCR_NV_ITEM_TMR_CONN_MODE_UOOS_SCAN )
      {
        dword temp_prm = 0;
        sdss_get_conn_mode_uoos_timers( parser_ptr->ss, &temp_prm, NULL );
        *prm4_ptr = (word) (temp_prm / 1000.0);
      }
      else
      {
        SD_ASSERT( ((word) *prm4_ptr) < ARR_SIZE(sd_cfg_items.items));
        *prm4_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm4_ptr ];
      }
      idx_delta = 5;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACQ_CNT_OR_TIME: /* ACQ_CNT_OR_TIME( LIST, ACQ_TYPE, START_POS, CNT, TIME)*/

      if( *prm5_ptr == SDSSSCR_NV_ITEM_TMR_HALF_UOOS_SCAN )
      {
        #if (defined(FEATURE_UNIFORM_SCAN_OOS))
        *prm5_ptr = (sdssscr_prm_type) 
                    ( sd_cfg_items.items[ SDSSSCR_NV_ITEM_TMR_UOOS_SCAN ] *
                      0.5 );
        #else
        SD_MSG_ERROR(" Invalid construct",0,0,0);
        *prm5_ptr = 0;
        #endif
      }
      else if( *prm5_ptr == SDSSSCR_NV_ITEM_TMR_HALF_AUTO_UOOS_SCAN )
      {
        
        #if ( defined (FEATURE_UMTS_UNIFORM_OOS_HANDLING) &&\
              defined(FEATURE_UNIFORM_SCAN_OOS))
#error code not present
        #elif (defined(FEATURE_UNIFORM_SCAN_OOS))
        *prm5_ptr = (sdssscr_prm_type) 
                       ( ( sd_cfg_items.items[ SDSSSCR_NV_ITEM_TMR_UOOS_SCAN ]) * 0.5 );
        #elif (defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING))
#error code not present
        #else
        SD_MSG_ERROR(" Invalid construct",0,0,0);
        *prm5_ptr = 0;
        #endif

        #if (defined(FEATURE_UNIFORM_SCAN_OOS))
        *prm5_ptr = (sdssscr_prm_type) 
                    ( sd_cfg_items.items[ SDSSSCR_NV_ITEM_TMR_UOOS_SCAN ] *
                      0.5 );
        #else
        SD_MSG_ERROR(" Invalid construct",0,0,0);
        *prm5_ptr = 0;
        #endif
      }
      else if( *prm5_ptr == SDSSSCR_NV_ITEM_TMR_CONN_MODE_UOOS_SCAN )
      {

        dword temp_prm = 0;
        sdss_get_conn_mode_uoos_timers( parser_ptr->ss, &temp_prm, NULL );
        *prm5_ptr = (word) (temp_prm / 1000.0);
      }
      else
      {
        SD_ASSERT( ((word) *prm5_ptr) < ARR_SIZE(sd_cfg_items.items));
        *prm5_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm5_ptr ];
      }
      idx_delta = 6;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACQ_MODE:      /* ACQ_MODE( MODE )               */
    case SDSSSCR_CON_PWR_SCAN_GW:
    case SDSSSCR_CON_SRV_REQ:
      idx_delta = 2;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_NEW_ACQ:       /* NEW_ACQ                        */
    case SDSSSCR_CON_START_TIME:    /* START_ACQ_TIME                 */
    case SDSSSCR_CON_SURVEY_WLAN:   /* SURVEY_WLAN                    */
    case SDSSSCR_CON_RESET_GW_UOOS_ACQ_PARA: /* RESET_GW_UOOS_ACQ_PARA*/
      idx_delta = 1;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACQ_PI:        /* ACQ_PI( LIST, ACQ_TYPE, NV_ITEM ) */
      SD_ASSERT( ((word) *prm3_ptr) < ARR_SIZE(sd_cfg_items.items));
      *prm3_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm3_ptr ];
      idx_delta = 4;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_PI_X:        /* ACQ_PI_X( LIST, ACQ_TYPE, NV_ITEM, X ) */
      SD_ASSERT( ((word) *prm3_ptr) < ARR_SIZE(sd_cfg_items.items));
      *prm3_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm3_ptr ];
      idx_delta = 5;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACQ_RX:        /* ACQ_RX( LIST, ACQ_TYPE, RX )   */
    case SDSSSCR_CON_SCHED:         /* SCHED( LIST, ACQ_TYPE, SCHED ) */
    case SDSSSCR_CON_SCHED_COND:    /* SCHED_COND( LIST, SCHED, ACQ_TYPE ) */
    case SDSSSCR_CON_ACQ_NETLIST:   /* ACQ_NETLIST(LIST, ACQ_TYPE, NETLIST)*/
      idx_delta = 4;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_SCHED_2:  /* SCHED_2( LIST, SCHED, ACQ_TYPE, ACQ_MODE ) */
    case SDSSSCR_CON_SCHED_COND2: /* SCHED_COND2( LIST, SCHED, ACQ_TYPE, SCHED_COND )*/
      idx_delta = 5;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_AVOID:         /* AVOID( LIST, TIME_HI, TIME_LO ) */
      *prm2_ptr = (sdssscr_prm_type) TO_WORD( *prm2_ptr, *prm3_ptr );
      *prm3_ptr = 0;
      idx_delta = 4;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_AVOID_CANCEL_COND: /* AVOID_CANCEL_COND( LIST, COND ) */
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_GET_NET:       /* GET_NET( LIST, REF_SYS )            */
      idx_delta = 3;
      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_AVOID_COND:   /* AVOID( LIST, TIME_HI, TIME_LO, COND )*/
      *prm2_ptr = (sdssscr_prm_type) TO_WORD( *prm2_ptr, *prm3_ptr );
      *prm3_ptr = 0;
      idx_delta = 5;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* -----------------------------
    ** SS-Script Control Constructs.
    ** -----------------------------
    */

    case SDSSSCR_CON_LOOP_TIME:     /* LOOP_TIME( TIME_HI, TIME_LO )  */
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_LOOP_NUM:      /* LOOP_NUM( NUM )                */
    case SDSSSCR_CON_LOOP_LIST:     /* LOOP_LIST( LIST )              */
    case SDSSSCR_CON_LOOP_COND:     /* LOOP_COND( COND )              */
      idx_delta = 2;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_LOOP_FOREVER:  /* LOOP_FOREVER()                 */
      idx_delta = 1;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_LOOP:          /* LOOP()                         */
      idx_delta = 1;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_PREV_SCR:      /* PREV_SCR()                     */
      idx_delta = 1;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_PROC_EVT:      /* PROC_EVT( EVENT_HI, EVENT_LO ) */
      *prm1_ptr = (sdssscr_prm_type) TO_WORD( *prm1_ptr, *prm2_ptr );
      *prm2_ptr = 0;
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACT_SCR:       /* ACT_SCR( SCRIPT )              */
      idx_delta = 2;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACT_SCR_RX:    /* ACT_SCR_RX( SCRIPT, LIST, RX ) */
    case SDSSSCR_CON_ACT_SCR_PI:    /* ACT_SCR_PI( SCRIPT, LIST, TC ) */
      idx_delta = 4;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IF:            /* IF(IF_COND, PARAM1, PARAM2, PARAM3 ) */
      idx_delta = 5;
      break;
    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDSSSCR_CON_CONT_GW_UOOS_REM_SLEEP:
    case SDSSSCR_CON_ELSE:          /* ELSE()                         */
    case SDSSSCR_CON_ENDIF:         /* ENDIF()                        */
      idx_delta = 1;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_END:           /* END                            */

      /* END marks the end of an SS-Script. Under normal situation this
      ** script construct should never be reached.
      **
      ** NOTE! parsing position shall never be incremented beyond this
      ** construct.
      */
      idx_delta = 0;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* -----------------------------------
    ** Miscellaneous SS-Script-Constructs.
    ** -----------------------------------
    */

    case SDSSSCR_CON_CONT:          /* CONT( TIME_HI, TIME_LO )       */
    case SDSSSCR_CON_CONT_REDIR:    /* CONT_REDIR( TIME_HI, TIME_LO ) */
    case SDSSSCR_CON_CONT_BSR:          /* CONT_BSR( NV_ITEM )            */
      //*prm1_ptr = (sdssscr_prm_type) TO_WORD( *prm1_ptr, *prm2_ptr );
      //*prm2_ptr = 0;
      //idx_delta = 3;
      //break;

    case SDSSSCR_CON_CONT_WAIT:       /* CONT_WAIT( TYPE, TIME_HI, TIME_LO )  */
      *prm2_ptr = (sdssscr_prm_type) TO_WORD( *prm2_ptr, *prm3_ptr );
      *prm3_ptr = 0;
      idx_delta = 4;
      break;

    case SDSSSCR_CON_CONT2:          /* CONT( NV_ITEM )            */
    case SDSSSCR_CON_CONT2_REDIR:    /* CONT_REDIR( NV_ITEM )      */
    case SDSSSCR_CON_CONT2_WAIT:     /* CONT_WAIT( TYPE, NV_ITEM ) */
    case SDSSSCR_CON_CONT2_BSR:          /* CONT_BSR( NV_ITEM )            */
      SD_ASSERT( ((word) *prm2_ptr) < ARR_SIZE(sd_cfg_items.items));

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Store the dword to prm2_ptr and prm3_ptr.
      */

      *prm3_ptr = (sdssscr_prm_type)(DWORD_LOW(sd_cfg_items.items[ *prm2_ptr ]));
      *prm2_ptr = (sdssscr_prm_type)(DWORD_HIGH(sd_cfg_items.items[ *prm2_ptr ]));
      idx_delta = 4;
      break;


    case SDSSSCR_CON_CONT_SET:      /* CONT_SET( TYPE, TIME )  */
      idx_delta = 3;
      break;

    case SDSSSCR_CON_CONT3_WAIT:     /* CONT3_WAIT( TYPE1, NV_ITEM1, TYPE2, NV_ITEM2 ) */
      SD_ASSERT( ((word) *prm2_ptr) < ARR_SIZE(sd_cfg_items.items));
      SD_ASSERT( ((word) *prm4_ptr) < ARR_SIZE(sd_cfg_items.items));
      *prm2_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm2_ptr ];
      *prm4_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm4_ptr ];
      idx_delta = 5;
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_CONT3:  /* CONT3( NV_ITEM1, NV_ITEM2, NV_ITEM3 ) */

      SD_ASSERT( ((word) *prm2_ptr) < ARR_SIZE(sd_cfg_items.items));
      SD_ASSERT( ((word) *prm3_ptr) < ARR_SIZE(sd_cfg_items.items));
      SD_ASSERT( ((word) *prm4_ptr) < ARR_SIZE(sd_cfg_items.items));
      *prm2_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm2_ptr ];
      *prm3_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm3_ptr ];
      *prm4_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm4_ptr ];
      idx_delta = 5;
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_SET_TIMER:

      SD_ASSERT( ((word) *prm1_ptr) < ARR_SIZE(sd_cfg_items.items));
      *prm1_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm1_ptr ];
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACCEPT:        /* ACCEPT()                       */
    case SDSSSCR_CON_REJECT:        /* REJECT()                       */
    case SDSSSCR_CON_PWR_SAVE:      /* PWR_SAVE()                     */
    case SDSSSCR_CON_REDIR_OFF:     /* REDIR_OFF()                    */
    case SDSSSCR_CON_HO_OFF:        /* HO_OFF()                       */
    case SDSSSCR_CON_ACQ_FAIL:      /* ACQ_FAIL()                     */
    case SDSSSCR_CON_ACC_RESET:     /* ACQ_RESET()                    */
    case SDSSSCR_CON_RESET_GW_ACQ_SCAN_STATUS: 
                                    /* RESET_GW_ACQ_SCAN_STATUS()     */
    case SDSSSCR_CON_MAINTAIN_TIMER:
    case SDSSSCR_CON_STACK_SYNC_UP:
    case SDSSSCR_CON_MAINTAIN_PREV_SCR_TIMER:
    case SDSSSCR_CON_BSR_TIMER_RESET: /* Reset BSR timer uptime */

      idx_delta = 1;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_SRV_STATUS:    /* SRV_STATUS( SRV_STATUS_VAL )   */
    case SDSSSCR_CON_SRV_OFF_UNTIL: /* SRV_OFF_IF_UNTIL( CRIT )       */
    case SDSSSCR_CON_SRV_OFF_IF_0:  /* SRV_OFF_IF_0( LIST )           */
    case SDSSSCR_CON_RESEL_STT:     /* RESEL_STT( STATE )             */
    case SDSSSCR_CON_RESEL_MODE:    /* RESEL_MODE( MODE )             */
      idx_delta = 2;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_SET_FLAG:
    case SDSSSCR_CON_CHK_SET_SRV_STATUS:
      idx_delta = 3;
      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* ----------------------------
    ** Condition Script Constructs.
    ** ----------------------------
    */

    case SDSSSCR_CON_IS_GW_SLEEP_TIME_SET:
    case SDSSSCR_CON_IS_GW_AWAKE_TIME_SET:
    case SDSSSCR_CON_NOT:           /* NOT                            */
      idx_delta = 1;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS:            /* IS( VAL )                      */
    case SDSSSCR_CON_IS_NOT:        /* IS_NOT( VAL )                  */
      idx_delta = 2;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS_EVENT:      /* IS_EVENT( VAL_HI, VAL_LO )     */
      *prm1_ptr = (sdssscr_prm_type) TO_WORD( *prm1_ptr, *prm2_ptr );
      *prm2_ptr = 0;
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS_PREF_REAS:  /* IS_PREF_REAS( VAL )            */
      *prm1_ptr = (sdssscr_prm_type) TO_WORD( *prm1_ptr, *prm2_ptr );
      *prm2_ptr = 0;
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS_HANDOFF:  /* IS_HANDOFF( VAL )            */
      *prm1_ptr = (sdssscr_prm_type) TO_WORD( *prm1_ptr, *prm2_ptr );
      *prm2_ptr = 0;
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDSSSCR_CON_SET_GW_PWR_SAVE_ACQ_STAGE: 
                                      /* GW_PWR_SAVE_ACQ_STAGE_SET (VAL) */  
      *prm1_ptr = (sdssscr_prm_type) TO_WORD( *prm1_ptr, *prm2_ptr );
      *prm2_ptr = 0;
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS_FLAG_SET:   /* IS_FLAG_SET( VAL )             */
    case SDSSSCR_CON_IS_OPR_MODE:   /* IS_OPR_MODE( VAL )             */
    case SDSSSCR_CON_IS_ORIG_MODE:  /* IS_ORIG_MODE( VAL )            */
    case SDSSSCR_CON_IS_STATE:      /* IS_STATE( VAL )                */
    case SDSSSCR_CON_IS_ACQ_STATE:  /* IS_ACQ_STATE( VAL )            */
    case SDSSSCR_CON_IS_ACQ_TYPE:   /* IS_ACQ_TYPE( VAL )             */
    case SDSSSCR_CON_IS_REDIR:      /* IS_REDIR( VAL )                */
    case SDSSSCR_CON_IS_RESEL_STT:  /* IS_RESEL_STT( VAL )            */
    case SDSSSCR_CON_IS_RESEL_MODE: /* IS_RESEL_MODE( VAL )           */
    case SDSSSCR_CON_IS_CDMA_AVAIL: /* IS_CDM_AVAIL( CNT )            */
    case SDSSSCR_CON_IS_SRV_STATUS: /* IS_SRV_STATUS( VAL )           */
    case SDSSSCR_CON_IS_SRV_DOMAIN: /* IS_SRV_DOMAIN( VAL )           */
    case SDSSSCR_CON_IS_ACC_REAS:   /* IS_ACC_REAS( VAL )             */
    case SDSSSCR_CON_IS_ACC_TERM:   /* IS_ACC_TERM( VAL )             */
    case SDSSSCR_CON_IS_PREF_CHGD:  /* IS_PREF_CHGD( VAL )            */
    case SDSSSCR_CON_IS_BAND_PREF_CHGD: /* IS_BAND_PREF_CHGD( VAL )   */
    case SDSSSCR_CON_IS_PREF_UPDATE:/* IS_PREF_UPDATE( VAL )          */
    case SDSSSCR_CON_IS_GW_SCAN_STATUS: /* IS_GW_SCAN_STATUS( VAL )   */
    case SDSSSCR_CON_IS_SYS_LOST:   /* IS_SYS_LOST( VAL )             */  
    case SDSSSCR_CON_IS_SIM_STATE: /*IS_NOT_SIM_STATE(SIM_STATUS) */
      idx_delta = 2;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS_ACC_FAIL:   /* IS_ACC_FAIL( CNT, TIME )       */
    case SDSSSCR_CON_IS_SID:        /* IS_SID( LST, VAL )             */
    case SDSSSCR_CON_IS_MODE:       /* IS_MODE( LST, VAL )            */
    case SDSSSCR_CON_IS_BAND:       /* IS_BAND( LST, VAL )            */
    case SDSSSCR_CON_IS_PRL:        /* IS_PRL( LST, VAL )             */
    case SDSSSCR_CON_IS_NET_SEL:    /* IS_NET_SEL( LST, VAL )         */
    case SDSSSCR_CON_IS_SCHED:      /* IS_SCHED( LST, VAL )           */
    case SDSSSCR_CON_IS_PREF_CONF:  /* IS_PREF_CONF( LST, VAL )       */
    case SDSSSCR_CON_IS_FADE:       /* IS_FADE( SYS, VAL )            */
    case SDSSSCR_CON_IS_ACQ_STATE2: /* IS_ACQ_STATE2( SS, VAL )       */
    case SDSSSCR_CON_IS_SRV_STATUS2:/* IS_SRV_STATUS2( SS, VAL )      */
    case SDSSSCR_CON_IS_MODE_PREF:  /* IS_MODE_PREF( LST, VAL )       */
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS_SAME:       /* IS_SAME( LST1, LST2, VAL )          */
    case SDSSSCR_CON_IS_DIFF:       /* IS_DIFF( LST1, LST2, VAL )          */
    case SDSSSCR_CON_IS_CONTAIN:    /* IS_CONTAIN( LST1, VAL, LST2 )       */
    case SDSSSCR_CON_IS_EVENT_CNT:  /* IS_EVENT_CNT ( EVENT, CNT, TIME )   */
    case SDSSSCR_CON_IS_TIME:       /* IS_TIME_GR(INSTANCE, SRV_VAL, TIME) */
      idx_delta = 4;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS_CNT:        /* IS_CNT( LST, CNT, CNT_COND, CNT_INC)*/
      idx_delta = 5;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_1ST_COND:
    case SDSSSCR_CON_MANIPULATOR:
    case SDSSSCR_CON_OPERATOR:
    case SDSSSCR_CON_CTRL:
    case SDSSSCR_CON_MISC:
    case SDSSSCR_CON_COND:
    case SDSSSCR_CON_MAX:
    default:
      SD_ERR_FATAL( "script cons=%d",(int)*con_ptr,0,0 );
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Based on the index delta zero out the unused construct's parameters.
  */
  if( idx_delta < 2 ) {
    *prm1_ptr = 0;
  }
  if( idx_delta < 3 ) {
    *prm2_ptr = 0;
  }
  if( idx_delta < 4 ) {
    *prm3_ptr = 0;
  }
  if( idx_delta < 5 ) {
    *prm4_ptr   = 0;
  }
  if( idx_delta < 6 ) {
    *prm5_ptr   = 0;
  }
  if( idx_delta < 7 ) {
    *prm6_ptr   = 0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the index delta from the current script construct to the next
  ** script construct
  */
  return idx_delta;

} /* sdssscr_parser_get */



/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_parser_next

DESCRIPTION
  Return the next script-construct of the script with which the script-parser
  is currently associated.

DEPENDENCIES
  The script-parser that is pointed to by parser_ptr parameter must have
  already been associated with a script via a call to sdssscr_parser_init().

RETURN VALUE
  The next script-construct of the script with which the script-parser
  is currently associated.

SIDE EFFECTS
  Increment the script index to point at the next script construct to be
  parsed.

===========================================================================*/
MMEXTN  sdssscr_con_e_type        sdssscr_parser_next(

        sdssscr_parser_s_type     *parser_ptr,
            /* Pointer to the script-parser that is associated with the
            ** script from which to get the next script-construct.
            */

        dword                     uptime,
            /* The current uptime.
            */

        int                       ctrl_cnt,
            /* Script control counter - being incremented every time control
            ** is returned to the SS-Client.
            */

        sdssscr_prm_type          *prm1_ptr,
            /* Pointer to an item where to store the 1st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm2_ptr,
            /* Pointer to an item where to store the 2st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm3_ptr,
            /* Pointer to an item where to store the 3st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm4_ptr,
            /* Pointer to an item where to store the 4st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm5_ptr,
            /* Pointer to an item where to store the 5th parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm6_ptr
            /* Pointer to an item where to store the 6th parameter that is
            ** associated with the returned script-construct (if any).
            */


)
{
  sdssscr_con_e_type    scr_con;
  int                   scr_idx;
  int                   scr_line;
  int                   idx_delta;
  boolean               is_loop_back;
  boolean               is_skip_loop = FALSE;
  int                   if_err_chk_idx;
  int                   if_err_chk_line;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //SD_FUNC_START( "sdssscr_parser_next()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( uptime != 0 );
  SD_ASSERT( prm1_ptr != NULL );
  SD_ASSERT( prm2_ptr != NULL );
  SD_ASSERT( prm3_ptr != NULL );
  SD_ASSERT( prm4_ptr != NULL );
  SD_ASSERT( prm5_ptr != NULL );
  SD_ASSERT( prm6_ptr != NULL );
  SD_ASSERT( parser_ptr->scr_ref != SDSSSCR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do script parsing (as long as encountering loop control constructs).
  */
  do
  {
    /* Get the next construct and its associated parameters.
    */
    idx_delta = sdssscr_parser_get( parser_ptr,
                                    &scr_con,
                                    prm1_ptr,
                                    prm2_ptr,
                                    prm3_ptr,
                                    prm4_ptr,
                                    prm5_ptr,
                                    prm6_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* For non-condition constructs, print the script's table, table-entry,
    ** line, construct and construct's associated parameters.
    */
    if( ! SDSSSCR_CON_IS_COND(scr_con) )
    {
      switch( parser_ptr->scr_evt_grp )
      {
        case SDSS_EVT_GRP_USER:
        case SDSS_EVT_GRP_HYBR_1_USER:
          SD_MSG_LOW( "User scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_MISC:
          SD_MSG_LOW( "Misc scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_INT:
          SD_MSG_LOW( "Int scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_CDMA_ACQ:
          SD_MSG_LOW( "CDMA acq scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_CDMA_OPR:
          SD_MSG_LOW( "CDMA opr scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_AMPS_ACQ:
          SD_MSG_LOW( "AMPS acq scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_AMPS_OPR:
          SD_MSG_LOW( "AMPS opr scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_HDR_USER:
          SD_MSG_LOW( "HDR user scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_HDR_ACQ:
          SD_MSG_LOW( "HDR acq scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_HDR_OPR:
          SD_MSG_LOW( "HDR opr scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_GW_ACQ:
          SD_MSG_LOW( "GW acq scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_GW_OPR:
          SD_MSG_LOW( "GW opr scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

                  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_WLAN_USER:
          SD_MSG_LOW( "WLAN user scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_WLAN_ACQ:
          SD_MSG_LOW( "WLAN acq scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_WLAN_OPR:
          SD_MSG_LOW( "WLAN opr scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;


        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_CALLED:
          SD_MSG_LOW( "Called scr=%d, line=%d, cons=%d",
                       parser_ptr->scr_tbl_idx,
                       parser_ptr->scr_line,
                       scr_con );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_EVT_GRP_MAX:
        default:
          SD_CASE_ERR( "event_group",0,0,0 );
          break;

      } /* switch */

    } /* if */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Remember the scr_idx and scr_line that reference the current script
    ** construct.
    */
    scr_idx   = parser_ptr->scr_idx;
    scr_line  = parser_ptr->scr_line;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Increment the scr_idx and scr_line to reference the next script
    ** construct.
    */
    parser_ptr->scr_idx += idx_delta;
    if( idx_delta > 0 )
    {
      parser_ptr->scr_line++;
    }

    /* Check for 0 val in loop_list & loop_num.
    */
    if ( is_skip_loop &&
         scr_con != SDSSSCR_CON_LOOP
       )
    {
      /* Skipping this line.
      */
      SD_MSG_HIGH("Skipping line %d", parser_ptr->scr_line, 0, 0 );
      continue;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Based on the script construct that is being parsed, increment the
    ** parsing position.
    */
    switch( scr_con ) /*lint -e788 */ /* sparse switch */
    {

      case SDSSSCR_CON_LOOP_TIME:     /* LOOP_TIME( TIME_HI, TIME_LO )     */

        /* If this is the first time this loop construct is encountered,
        ** initialize the loop parameters.
        */
        if( parser_ptr->loop_type == SDSSSCR_CON_MAX )
        {
          parser_ptr->loop_type = SDSSSCR_CON_LOOP_TIME;
          parser_ptr->loop_idx  = scr_idx;
          parser_ptr->loop_line = scr_line;
          parser_ptr->loop_val  = uptime + TO_WORD(*prm1_ptr, *prm2_ptr);

          SD_MSG_MED( "@@@@ LOOP_TIME, uptime=%d, loop_uptime=%d",
                        uptime, parser_ptr->loop_val, 0);
        }

        /* Record the value of the script execution control counter in order
        ** to detect a loop that does not return control to the SS-Client.
        */
        parser_ptr->loop_ctrl_cnt = ctrl_cnt;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_LOOP_NUM:      /* LOOP_NUM( NUM )                */

        /* If this is the first time this loop construct is encountered,
        ** initialize the loop parameters.
        */
        if( parser_ptr->loop_type == SDSSSCR_CON_MAX )
        {
          parser_ptr->loop_type = SDSSSCR_CON_LOOP_NUM;
          parser_ptr->loop_idx  = scr_idx;
          parser_ptr->loop_line = scr_line;
          parser_ptr->loop_val  = *prm1_ptr;

          SD_MSG_MED( "@@@@ LOOP_NUM, loop_num=%d",
                       parser_ptr->loop_val, 0, 0);
        }

        /* Record the value of the script execution control counter in order
        ** to detect a loop that does not return control to the SS-Client.
        */
        parser_ptr->loop_ctrl_cnt = ctrl_cnt;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_LOOP_LIST:      /* LOOP_LIST( LIST )              */

        /* If this is the first time this loop construct is encountered,
        ** initialize the loop parameters.
        */
        if( parser_ptr->loop_type == SDSSSCR_CON_MAX )
        {
          parser_ptr->loop_type = SDSSSCR_CON_LOOP_LIST;
          parser_ptr->loop_idx  = scr_idx;
          parser_ptr->loop_line = scr_line;
          parser_ptr->loop_val  =
              (dword) sdsr_list_cnt( sdss_sr_list_map2( parser_ptr->ss,
                                                  (sdsr_e_type) *prm1_ptr) );

          SD_MSG_MED( "@@@@ LOOP_LIST, list_cnt=%d",
                       parser_ptr->loop_val, 0, 0);
        }

        /* Record the value of the script execution control counter in order
        ** to detect a loop that does not return control to the SS-Client.
        */
        parser_ptr->loop_ctrl_cnt = ctrl_cnt;

        /* Check for loop_val = 0.
        */
        if ( parser_ptr->loop_val == 0 )
        {
          is_skip_loop = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      case SDSSSCR_CON_LOOP_COND:
        /* If this is the first time this loop construct is encountered,
        ** initialize the loop parameters.
        */
        if( parser_ptr->loop_type == SDSSSCR_CON_MAX )
        {
          parser_ptr->loop_type = SDSSSCR_CON_LOOP_COND;
          parser_ptr->loop_idx  = scr_idx;
          parser_ptr->loop_line = scr_line;
          parser_ptr->loop_cond = (sdss_loop_cond_e_type ) *prm1_ptr;
          parser_ptr->loop_val  = 
          (dword) sdss_is_loop_cond( parser_ptr->ss, parser_ptr->loop_cond );

          SD_MSG_MED( "@@@@ LOOP_COND, cond=%d val %d",
                            parser_ptr->loop_cond, parser_ptr->loop_val, 0 );
        }

        /* Record the value of the script execution control counter in order
        ** to detect a loop that does not return control to the SS-Client.
        */
        parser_ptr->loop_ctrl_cnt = ctrl_cnt;

        /* Check for loop_val = 0.
        */
        if ( parser_ptr->loop_val == 0 )
        {
          is_skip_loop = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_LOOP_FOREVER:  /* LOOP_FOREVER()                 */

        /* If this is the first time this loop construct is encountered,
        ** initialize the loop parameters.
        */
        if( parser_ptr->loop_type == SDSSSCR_CON_MAX )
        {
          parser_ptr->loop_type = SDSSSCR_CON_LOOP_FOREVER;
          parser_ptr->loop_idx  = scr_idx;
          parser_ptr->loop_line = scr_line;
          parser_ptr->loop_val  = 1;

          SD_MSG_MED( "LOOP_FOREVER",0,0,0 );
        }

        /* Record the value of the script execution control counter in order
        ** to detect a loop that does not return control to the SS-Client.
        */
        parser_ptr->loop_ctrl_cnt = ctrl_cnt;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_LOOP:          /* LOOP()                         */

        /* If the script execution control counter was not incremented since
        ** the matching LOOP_XXX() construct was encountered, the loop did
        ** not return control to the SS-Front-end. Record this fact by
        ** incrementing the loop_back_cnt. Else, reset the loop_back_cnt to
        ** 0.
        */
        is_skip_loop = FALSE;
        if( parser_ptr->loop_ctrl_cnt == ctrl_cnt )
        {
          parser_ptr->loop_back_cnt++;
        }
        else
        {
          parser_ptr->loop_back_cnt = 0;
        }

        /* If the loop was traversed 3 times without returning control to the
        ** SS-Front-end (i.e., loop_back_cnt is > 2), let script execution
        ** fall out of the loop regardless of whether the loop condition
        ** evaluates to TRUE or FALSE.
        */
        if( parser_ptr->loop_back_cnt > SDSSSCR_MAX_LOOP_BACK_CNT )
        {
          SD_MSG_MED( "!!!!EARLY LOOP TERMINATION!!!!",0,0,0 );
          parser_ptr->loop_type     = SDSSSCR_CON_MAX;
          parser_ptr->loop_val      = 0;
          parser_ptr->loop_back_cnt = 0;
          break;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Decide whether we should continue looping.
        */
        is_loop_back = FALSE;

        switch( parser_ptr->loop_type )
        {
          case SDSSSCR_CON_LOOP_TIME:

            if( uptime < parser_ptr->loop_val )
            {
              is_loop_back = TRUE;

              SD_MSG_MED( "<<LOOP BACK, until=%d, uptime=%d",
                            parser_ptr->loop_val, uptime, 0 );
            }
            else
            {
              SD_MSG_MED( ">>LOOP END, until=%d, uptime=%d",
                            parser_ptr->loop_val, uptime, 0 );
            }
            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case SDSSSCR_CON_LOOP_NUM:
          case SDSSSCR_CON_LOOP_LIST:

            if( parser_ptr->loop_val > 0 ) {
              parser_ptr->loop_val--;
            }

            if( parser_ptr->loop_val > 0 )
            {
              is_loop_back = TRUE;

              SD_MSG_MED( "<<LOOP BACK, loop_cnt=%d",
                            parser_ptr->loop_val, 0, 0 );
            }
            else
            {
              SD_MSG_MED( ">>LOOP END, loop_cnt=%d",
                            parser_ptr->loop_val, 0, 0 );
            }
            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case  SDSSSCR_CON_LOOP_COND:
            parser_ptr->loop_val  = 
            (dword) sdss_is_loop_cond( parser_ptr->ss, parser_ptr->loop_cond );

            if( parser_ptr->loop_val )
            {
              is_loop_back = TRUE;

              SD_MSG_MED( "<<LOOP BACK, loop_cond=%d loop_val=%d",
                            parser_ptr->loop_cond, parser_ptr->loop_val, 0 );
            }
            else
            {
              SD_MSG_MED( ">>LOOP END, loop_cond=%d loop_val=%d",
                            parser_ptr->loop_cond, parser_ptr->loop_val, 0 );
            }
            break;
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case SDSSSCR_CON_LOOP_FOREVER:

            SD_MSG_MED( "<<LOOP BACK, forever",0,0,0 );

            is_loop_back = TRUE;
            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          default:
            SD_CASE_ERR( "loop_type",0,0,0 );

        } /* switch */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If looping back, set the script index and line per the beginning
        ** of the loop. Else, reset the loop type, val and back count.
        */
        if( is_loop_back )
        {
          parser_ptr->scr_idx       = parser_ptr->loop_idx;
          parser_ptr->scr_line      = parser_ptr->loop_line;
        }
        else
        {
          parser_ptr->loop_type     = SDSSSCR_CON_MAX;
          parser_ptr->loop_val      = 0;
          parser_ptr->loop_back_cnt = 0;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IF:

        /* Initialize the if statements parsing parameters
        */

        parser_ptr->if_idx     = scr_idx;
        parser_ptr->if_line    = scr_line;
        parser_ptr->else_idx   = 0;
        parser_ptr->else_line  = 0;
        parser_ptr->endif_idx  = 0;
        parser_ptr->endif_line = 0;
        if_err_chk_idx         = 0;
        if_err_chk_line        = 0;


        /* Find the position of the next IF construct, if present
        */
        (void) sdssscr_parser_is_contain( parser_ptr, SDSSSCR_CON_IF, FALSE, 
                                         &if_err_chk_idx, &if_err_chk_line );

        /* Find the begining of the ELSE block if present
        */
        (void)sdssscr_parser_is_contain( parser_ptr, SDSSSCR_CON_ELSE, FALSE, 
                             &parser_ptr->else_idx, &parser_ptr->else_line );

        /* Find the end of the if block
        */
        (void)sdssscr_parser_is_contain( parser_ptr, SDSSSCR_CON_ENDIF, FALSE, 
                           &parser_ptr->endif_idx, &parser_ptr->endif_line );

        SD_ASSERT( parser_ptr->endif_idx  != 0 );
        SD_ASSERT( parser_ptr->endif_line != 0 );
        if( if_err_chk_idx != 0 )
        {
          SD_ASSERT( parser_ptr->endif_idx < if_err_chk_idx );
          SD_ASSERT( parser_ptr->endif_line < if_err_chk_line );
        }

        if( parser_ptr->endif_idx == 0 )
        {
          SD_ERR(" Missing ENDIF",0,0,0 );

          if( parser_ptr->else_idx != 0 )
          {
            parser_ptr->endif_idx  = parser_ptr->else_idx;
            parser_ptr->endif_line = parser_ptr->else_line;
          }
          else
          {
            parser_ptr->endif_idx  = parser_ptr->if_idx;
            parser_ptr->endif_line = parser_ptr->if_line;
          }
        }

        if( if_err_chk_idx != 0 && parser_ptr->endif_idx >= if_err_chk_idx )
        {
          SD_ERR( "Nested IF not supported",0,0,0 );
        }

        /* If the ELSE is not for this if block, mark the block as having 
        ** no else
        */

        if( parser_ptr->else_idx >= parser_ptr->endif_idx )
        {
          parser_ptr->else_idx  = 0;
          parser_ptr->else_line = 0;
        }

        /* Check if the IF condition is true
        */
        parser_ptr->if_val = sdss_is_if_cond( parser_ptr->ss, 
                                              (sdss_if_cond_e_type)*prm1_ptr, 
                                              (sdss_cmp_criteria_e_type)*prm2_ptr, 
                                              *prm3_ptr, 
                                              *prm4_ptr );

        SD_MSG_MED( "<<IF, if_cond=%d cmp_criteria=%d if_val=%d",
                            *prm1_ptr, *prm2_ptr, parser_ptr->if_val );

        /* If the condition is not true, pass control to the else block, if
        ** present or the next statement after the end
        */
        if( !parser_ptr->if_val )
        {
          if( parser_ptr->else_idx != 0 )
          {
            parser_ptr->scr_idx  = parser_ptr->else_idx;
            parser_ptr->scr_line = parser_ptr->else_line;
          }
          else
          {
            parser_ptr->scr_idx  = parser_ptr->endif_idx;
            parser_ptr->scr_line = parser_ptr->endif_line;
          }
        }

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ELSE:

        if( parser_ptr->if_val )
        {
          parser_ptr->scr_idx  = parser_ptr->endif_idx;
          parser_ptr->scr_line = parser_ptr->endif_line;
        }

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      default:
        break;

    } /* switch */ /*lint +e788 */ /* sparse switch */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  } while( scr_con == SDSSSCR_CON_LOOP_TIME     ||
           scr_con == SDSSSCR_CON_LOOP_NUM      ||
           scr_con == SDSSSCR_CON_LOOP_LIST     ||
           scr_con == SDSSSCR_CON_LOOP_COND     ||
           scr_con == SDSSSCR_CON_LOOP_FOREVER  ||
           scr_con == SDSSSCR_CON_LOOP          ||
           scr_con == SDSSSCR_CON_IF            ||
           scr_con == SDSSSCR_CON_ELSE          ||
           scr_con == SDSSSCR_CON_ENDIF         ||
           is_skip_loop );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the parsed script-construct.
  */
  return scr_con;

} /* sdssscr_parser_next */




/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_parser_is_contain

DESCRIPTION
  Check whether the script that is associated with the script parser that is
  pointed to by parser_ptr contains a specified script construct.

DEPENDENCIES
  The script-parser that is pointed to by parser_ptr parameter must have
  already been associated with a script via a call to sdssscr_parser_init().

RETURN VALUE
  TRUE if the script that is associated with the script parser that is
  pointed to by parser_ptr contains the specified script construct. FALSE
  otherwise.

   If rtrn_idx and rtrn_line are passed, the position of the construct is
  returned in those

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                       sdssscr_parser_is_contain(

        const sdssscr_parser_s_type   *parser_ptr,
            /* Pointer to the script-parser that is associated with the
            ** script to be searched.
            */

        sdssscr_con_e_type            scr_con,
            /* Script construct for which to search.
            */

        boolean                       from_beginning,
            /* Is the matching to be done from the beginning of the script
            */

        int                           *rtrn_idx,
            /* Record the index where the construct is found
            */

        int                           *rtrn_line
            /* Record the line where the construct is found
            */
)
{
  sdssscr_parser_s_type parser;

  sdssscr_con_e_type    extracted_scr_con;
  sdssscr_prm_type      prm1;
  sdssscr_prm_type      prm2;
  sdssscr_prm_type      prm3;
  sdssscr_prm_type      prm4;
  sdssscr_prm_type      prm5;
  sdssscr_prm_type      prm6;

  int                   idx_delta;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( scr_con, SDSSSCR_CON_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the script parser into a local variable and set the traversal
  ** index to 0.
  */
  parser = *parser_ptr;

  if( from_beginning )
  {
    parser.scr_idx  = 0;
    parser.scr_line = 0;
  }  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Parse the script while end of script is not reached. If script construct
  ** in question is encountered, return TRUE.
  */
  do
  {
    idx_delta = sdssscr_parser_get( &parser,
                                    &extracted_scr_con,
                                    &prm1,
                                    &prm2,
                                    &prm3,
                                    &prm4,
                                    &prm5,
                                    &prm6 );

    parser.scr_idx += idx_delta;

    if( idx_delta > 0 )
    {
      parser.scr_line++;
    }

    /* Construct found
    */

    if( extracted_scr_con == scr_con )
    {
      /* Return the index position of the construct
      */
      if( rtrn_idx != NULL )
      {
        *rtrn_idx = parser.scr_idx;
      }

      /* Return the line the construct was found
      */
      if( rtrn_line != NULL )
      {
        *rtrn_line = parser.scr_line;
      }

      return TRUE;
    }

  } while( idx_delta > 0 && extracted_scr_con != SDSSSCR_CON_END );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the script does not contain the script construct in
  ** question - return FALSE.
  */
  return FALSE;

} /* sdssscr_parser_is_contain */



#if 0
#if( TG==T_PC )
#error code not present
#endif
#endif
