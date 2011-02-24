/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S Y S T E M   D E T E R M I N A T I O N

              S Y S T E M   S E L E C T I O N   C O R E

GENERAL DESCRIPTION
  This file makes up the System Selection Core (SS-Core) component of SD.

  The SS-Core encapsulates all the functionality that is associated with
  processing the SS-Events that are being received from the SS-Front-end and
  for instructing the SS-Front-end of the next action to take in return to
  each SS-Event that is being processed.

  The SS-Core is built around a script engine, which is responsible for
  executing the currently active SS-Script. When the SS-Core receives an
  SS-Event from the SS-Front-end, the script engine consults the
  SS-Script component to whether an SS-Script is associated with the SS-Event
  it got. If an SS-Script is associated with the SS-Event, this script
  becomes the active SS-Script. Else, the current active script remains the
  active SS-Script.

  Note that normally a new SS-Script becomes active only when a User or
  Operation related SS-Event is being received from the SS-Front-end. In
  other words, the currently active SS-Script is responsible for handling
  the Acquisition related SS-Events that are being received during the
  acquisition process.

  The SS-Core also maintains an acquisition state machine that is being
  consulted during the acquisition and re-selection process.

  The following diagram shows a 2nd level decomposition of the SS-Core. The
  arrows in this diagram represent function calls.

                     2nd Level SS-Core Decomposition
                     -------------------------------

  +----------------------------------------------------------------------+
  |                           SS-Front-end                               |
  +----------------------------------------------------------------------+
              |
              |
  +===========|=========================================+     +-----------+
  |           |           SS-Core                       |     |           |
  |          \|/                                        |     |           |
  |  +-----------------------------------------------+  |     |           |
  |  |                  SS-Interface                 |  |     |           |
  |  |                   (sdss_xxx)                  |  |     |           |
  |  +-----------------------------------------------+  |     |           |
  |      |                  |                           |     |           |
  |      |                 \|/                          |     |           |
  |      |    +--------------------------------------+  |     |           |
  |      |    |                                      |  |     |           |
  |      |    |                                      |  |     |           |
  |      |    |               SS-Engine              |  |     |  Database |
  |      |    |            (sdss_eng_xxx)            |  |---->| (sdsr_xxx)|
  |      |    |                                      |  |     |(sdprl_xxx)|
  |      |    |                                      |  |     |           |
  |      |    +--------------------------------------+  |     |           |
  |      |          |                       |           |     |           |
  |     \|/        \|/                     \|/          |     |           |
  |  +-------------------+       +-------------------+  |     |           |
  |  |                   |       |                   |  |     |           |
  |  |                   |       |                   |  |     |           |
  |  |  SS-Acquisition   |       |     SS-Scripts    |  |     |           |
  |  |  (sdss_acq_xxx)   |       |    (sdssscr_xxx)  |  |     |           |
  |  |                   |       |                   |  |     |           |
  |  |                   |       |                   |  |     |           |
  |  +-------------------+       +-------------------+  |     |           |
  |                                                     |     |           |
  +=====================================================+     +-----------+

  SS-Interface.
    Define the interface that is visible to entities outside the SS-Core.

  SS-Engine:
    Encapsulates the functionality that is associated with executing
    SS-Scripts.

  SS-Script:
    SS-Script definitions.

  SS-Acquisition:
    Acquisition state machine - encapsulate the functionality that is
    associated with the acquisition process. The following figure shows
    each of the acquisition states and the transition between states:

                         Acquisition State Machine
                         -------------------------

                          +------------------------+
     Acquired a system    |                        |
 |<--and not interested---|   SDSS_ACQ_STT_START   |<-----------------------+
 |   in a more preferred  |                        |                        |
 |   one.                 +------------------------+                        |
 |                                    |                                     |
 |                                    |                                     |
 |                           Acquire a PRL system,                          |
 |                           but interested in a                            |
 |                           more preferred one                             |
 |                                    |                                     |
 |                                   \|/                                    |
 |                        +------------------------+                        |
 |   Acquired a more      |                        |  Didn't find a more    |
 |<--preferred system-----| SDSS_ACQ_STT_MORE_PREF |--preferred system----->|
 |                        |                        |  and original system   |
 |                        +------------------------+  is not usable         |
 |                                    |                                     |
 |                                    |                                     |
 |                           Didn't find a more                             |
 |                           preferred system and                           |
 |                           original system is usable                      |
 |                                    |                                     |
 |                                   \|/                                    |
 |                        +------------------------+                        |
 |                        |                        |  Failed to reacquire   |
 |                        |   SDSS_ACQ_STT_REACQ   |--the original system-->|
 |                        |                        |                        |
 |                        +------------------------+                        |
 |                                    |                                     |
 |                                    |                                     |
 |                           Successfully reacquired                        |
 |                           the original system                            |
 |                                    |                                     |
 |                                   \|/                                    |
 |                        +------------------------+                        |
 |                        |                        |                        |
 +----------------------->|    SDSS_ACQ_STT_DONE   |--Receives SS-Event---->|
                          |                        |
                          +------------------------+

EXTERNALIZED FUNCTIONS

  General:
    sdss_is_orig_mode
    sdss_is_hybr_pref
    sdss_is_handoff
    sdss_is_band_pref
    sdss_is_acq_specific_sid
    sdss_get_pref
    sdss_get_acc_prm
    sdss_get_sim_state
    sdss_update_sim_state
    sdss_update_srv_status

    sdss_opr_mode_update
    sdss_pref_update
    sdss_redir_update
    sdss_cdma_lock_update
    sdss_acc_term_update
    sdss_hdr_ses_close_update
    sdss_ho_update
    sdss_orig_success_update
    sdss_throttle_acq_on
    sdss_throttle_acq_off
    sdss_throttle_reset_cntrs
    sdss_sys_lost_update

    sdss_act_print
    sdss_iact_print

    sdss_eng_stack_get_state

  System Record Suport
    sdss_sr_is_mode_pref
    sdss_sr_is_band_pref
    sdss_sr_is_pref_conf
    sdss_sr_is_prl

  System Record List Suport
    sdss_sr_list_is_mode

  Major Routines
    sdss_component_init
    sdss_event_recovery
    sdss_event_proc


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The SS-Core initialization function sdss_init() must be called at power-up
  before any other sdss_xxx() function is called.

Copyright (c) 2000 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/sd/rel/08H1/src/sdss.c#18 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/09   rk      Handling new pref_reason SD_SS_PREF_REAS_MULTIMODE_SCAN
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
03/19/09   ak      Fixed compiler warnings.
03/06/09   rk      Changes to handle limited service aquisition.
02/13/09   sn      For user pwr save exit event, in pwr save, continue with 
                          earlier  4 + 1 GW  band group scan sequence.
02/10/09   fj      Filter band_capability based on mode_capability, using
                   sdss_get_supp_mode_band_pref()
01/09/09   rm      Reverting the fix - After get_netowrks srv_req with SYNC_UP 
                   should not be sent to NAS. Also make ss_state to OPR 
                   after get_netowrks.
12/05/08   sn      UOOS GW lmtd search for multimode phones when GW subs not 
                   available.
12/25/08   rk      Added new construct IS_BAND_PREF_CHGD.
12/11/08   sv      Excluding WLAN systems from checking Roam Preference
11/28/08   rm      In UOOS report Limited service to NAS if sim state is
                   CS_PS_Invalid. Reverting part of the fix for CR-124247 
11/13/08   rm      After get_netowrks srv_req with SYNC_UP should not be sent 
                   to NAS. Also make ss_state to OPR after get_netowrks.
11/05/08   sv      Moved sdssscr_flag_e_type to sdss_flag_e_type
                   Added sdss_update_flag to update stack_sync_up_flag
10/23/08   ak      Fixing the no firmware scenario
09/30/08   fj      Replace FEATURE_SD_DETECT_HDR_HW with FEATURE_MSM_SUPPORT.
10/02/08   aj      Bandclass support for BC17, 18, 19
09/29/08   rk      Change the ss state ACQ when SD sends srv req and not got cnf.
09/26/08   cl      Fix the system to use TMR_HDR_REDIR under DO Redirection
09/25/08   sv      Memory reduction changes
09/18/08   rm      Adding acq_order_pref in sd_ss_act_get_net_gw_s_type.
09/15/08   pk      Added support for WLAN UOoS
09/15/08   rm      Store true_mode_pref in HYBR mode.
08/29/08   rm      Searching for GW systems in Automatic mode when SIM 
                   is not avilable
08/25/08   ak      Fixed the general compiler warnings.
08/20/08   st      resolved Klockwork issues
08/12/08   ak      Implemented the new Sprint BSR algorithm
07/31/08   fj      HDR acquisition issue. Modify sdss_get_mode_capability.
07/30/08   sv      - Added is_stack_syncup_in_progress to indicate the sync-up status
                   - Filter the stack sync-up event when call orig on HDR
                   - If Stack sync-up has conflicting preferences, update acq list
07/10/08   rm      Assigning srv_req_type before checking for pref changes
06/20/08   sn      BSR 2.0 changes
06/25/08   sn      Added new IS script construct SDSSSCR_COND_SIM_DOMAIN_VALID 
                   in sdss_eng_scr_con_is
06/11/08   fj      Added mode_capability in sdss_s_type,
                   replaced is_hdr_not_supported from sdss_s_type with mode_capability
                   (SD_SS_MODE_PREF_HDR),
                   modified sdss_get_band_capability to EXTERN function,
                   added function sdss_get_mode_capability.                   
06/12/08   aj      Klocwork fixes
06/10/08   rm      Report ACQ_FAIL when phone moves to POWER_SAVE state while
                   in UOOS  
05/22/08   vk/sv   modified code to handle roam only preference
04/30/08   cl      Adding support for API sd_misc_sys_avoid_list, which list all  
                   the avoided systems. HDR/SCHR use this info to update their 
                   scan list when doing OFS.
04/28/08   rn      Reset the acq_scan_status if user changes the preferences
04/24/08   rm      Correcting comment style
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
03/19/08   cl      Fixing UOOS Time to support Auto Mode Builds.
03/20/08   rm      Lint Clean
03/17/08   vs      Added prev_sys_info to the sd state structure,
                   Added functions to get/set prev_sys_info,
                   Made changes to clear prev_sys_info when starting 
                   a GW acquisition.
03/14/08   rm      CM-NAS interface for new service req type
03/11/08   vk      Corrected code for GW case for SD_SS_MODE_PREF_GW
02/19/08   cl      Optimize the execution time of SD API sd_misc_is_fts_mode
02/20/08   vs      Made changes to clear SDSR_PREV_COLLOC_LST
                   when starting a GW acquisition
02/13/08   vs      Added SDSS_IF_COND_SS_STATE to check selection state
01/22/08   rm      Reverted the changes done for- support for SIM Invalid
                   and UOOS
12/11/07   rn      Allow GSM ONLY if the hardware is EDGE ONLY
11/28/07   jqi/ak  Prevented NULL pointer access in the schedule list.
10/03/07   bh      Moved to new API and supported client stub
09/18/07   pk      Added support for non-Hybrid UOOS
09/04/07   cl      Change the interface that detect HDR Support
08/29/07   sk      Fixed error in MOST_PREF classification of a system to
                   ignore systems that shall not be acquired due to user pref
                   or target capability.
06/19/07   cl      Featurize the fix 118758 with FEATURE_SD_DETECT_HDR_HW
06/19/07   cl      Adding support for 6175 to detect HDR hardware
06/15/07   rp      Adding rem_uoos_awake_time and rem_uoos_sleep_time
06/06/07   pk      UOOS enhancements...
05/29/07   pk      Added support for SIM Invalid and UOOS
05/02/07   jqi     DO not turn off the redirection afer IHO if the expected
                   SID/NID of the redirected system is WILDCARD.
                   Expanded 1x system reselection timer value from 16-bit to
                   32-bit.
05/01/07   rp      Correct sdss_pref_update to update orig_mode correctly
04/23/07   jqi     Added Filtering support for stopping HDR activity.
                   Added support for sd_misc_is_orig_mode.
03/28/07   jqi     Cancelled the avoidance until acq fail during redial.
03/28/07   sk      Added new GW UOOS wakeup time for GCF fix.
                   Added sdss_is_gprs_anite_gcf().
03/23/07   pk      Disabling 1X acquisition throttling for VoIP -> 1X hho
02/26/07   pk      Added sdss_get_ss_for_mode for all featurizations
01/04/07   pk      Added support for VoIP -> 1X Handoffs
12/01/06   pk      Added support for Automatic mode for CDMA + GSM UOOS
                   builds
12/03/06   ka      Adding sdss_map_mode_pref_to_mode()
11/29/06   ka      Limited service resel timer expiry results in sending
                   MEAS_BACK action instead of full service req. Satisfies
                   GCF 6.1.2.6.
11/20/06   pk      Added support for HYBR_GW cont...
11/16/06   pk      Using sys_simultaneous_mode_e_type for hybr_gw
11/15/06   pk      Added support for HYBR_GW
11/13/06   ic      Lint fixes
11/07/06   sk      Memory reduction effort.
                   - Hand optimized sdss_map_band_pref_to_band().
10/02/06   sk      Added support for AVOID_CANCEL_COND.
09/22/06   RP      Added BC15, BC16 support.
09/17/06   sk      Added all WLAN bands to WLAN band capability.
08/29/06   sk      Added WLAN Adhoc support.
08/24/06   pk      Added support for IF construct and GCF test fixes
08/24/06   pk      Added support for sdss_nv_init
08/23/06   jqi     Added support for system reselection during BCMCS flow.
08/15/06   pk      Added support for GW UOOS
08/07/06   pk      Added support for 1X UOOS
08/08/06   sk      Updated sdss_get_cdma_request_mode() to handle new SD mode
                   prefs.
07/13/06   jqi     Added the system lost reason for HDR system lost event.
07/07/06   sk      Added sdss_get_srv_status().
05/22/06   jqi     Made use of the chan list for HDR oh info.
05/17/06   jqi     Added ss information in sdsr table to differenciate the
                   avoidance over different SD instances.
05/09/06   jqi     Fixed compilation errors.
05/08/06   sk      Added support for expected SID for HDR.
05/04/06   sk      Renamed FEATURE_PREVENT_SCRIPT_MULTI_INST_EXEC to
                   FEATURE_SD_PREVENT_SCRIPT_MULTI_INST_EXEC.
05/04/06   jqi     Lint fixes.
04/28/06   sk      Added new list PWR_SAVE_LST.
                   Added protection against invalid list/position when
                   returning to a ACQ that was interrupted by a new script.
                   Added protection to prevent multiple instances of the same
                   script from running in the engine stack. Featurized under
                   FEATURE_PREVENT_SCRIPT_MULTI_INST_EXEC.
04/17/05   jqi     Added EGSRDM support.
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
03/20/06   sk      Added support for acquisition type NOT_LESS_PREF_OR_HOME.
03/9/06    jqi     DO system reselection optimization(Sys resl at DO MO call
                   end.)
                   Multichannel avoidance.
                   Reset the DO failure counter when Subnet ID is changed.
                   Do not change sd state if only srv domain is changed.
03/07/06   sk      Added support for SD_SS_SRV_DOMAIN_PREF_NONE.
03/05/06   jqi     Merged WCDMA BC3,4,8 and 9 support.
02/08/06   pk      Replaced SD_ERR with SD_MSG_ERROR
01/16/06   sk      Fixed bug in conditional scheduling of systems.
                   Fixed bug that resulted in the scheduled system's acq type
                   being used for the first system in the acquisition list.
01/09/06   sk      Made changes to treat custom home system as home system.
                   Added new acquisition type MORE_PREF_THAN_SRV_SYS
                   Changed acquisition type NOT_LESS_PREF to check only upto
                   roaming indicator.
12/22/05   jqi     Merged background PLMN search support.
12/12/05   jqi     Lint fixes.
12/02/05   jqi     Lint 8 fixes.
11/08/05   jqi     Added support for 1x acquisition throttling for BCMCS
                   activity.
11/07/05   jqi     Added F3 message.
                   Removed the featurization around the sdss_is_hybr_pref().
11/02/05   sk      Fixed all Lint and compiler warnings.
                   Added WLAN bands/band_prefs mapping for all 2400 and 5000
                   bands.
10/19/05   jqi     Added more debug message.
10/14/05   jqi     Added BC14 support.
10/08/05   jqi     Ignore the roaming preference for GSM/WCDMA operation.
10/05/05   sk      Added support for configuring SD values from NV.
09/12/05   jqi     Lint fixes.
09/10/05   pk      Added support for IS_TIME_GR(INSTANCE, SRV_VAL, TIME) and
                   DEL_CNT( LIST, START_POS, CNT )
09/07/05   jqi     Fixed compilation errors.
09/01/05   jqi     DO system reselection optimization.
                   Use different scripts based on the SS item during event
                   recovery process.
08/19/05   sk      Initialized event counters and times during power up.
08/05/05   jqi     Added support for Best preferred acquisition for DO.
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
07/11/05   jqi     Added avoidance timer for access failures and fades.
07/08/05   sk      Added generic event counters and timestamps to SS core
                   and added sdss_event_cnt_update() to update them.
                   Added IS_EVENT_CNT construct to the SD engine
                   Fixed Lint warnings.
06/30/05   jqi     Lint fixes.
06/26/05   ic      Lint and compiler warnings cleanup
06/06/05   jqi     Minimize 1x miss pages when DO is reverse link limited.
                   Avoidance: 2m(after 1 failure), 4m( after 2), 8m(after 3),
                   16m(after 6).
05/16/05   jqi     Skip the HDR record if FEATURE_HDR is not supported.
05/13/05   ka      Handling SDSS_EVT_WLAN_ACQ_BAD_SYS in acquisiton event
                   processing.
05/16/05   pk      Enhanced SDSSSCR_CON_CHK_SET_SRV_STATUS script to process
                   Bit masks of the SRV status. The new construct supports
                   sdssscr_srv_status_val_e_type parameters.
05/16/05   pk      Added support for SDSSSCR_CON_CHK_SET_SRV_STATUS script

05/04/05   jqi     Added support to process HDR bad system event while in
                   acquisition state.
                   Replace CELL/PCS with BC0/BC1 while checking the band
                   capability.
02/24/05   pk      Fixed lint errors.
01/30/05   sj      Added support for WLAN.
01/27/05   ka      Added support for Band class 10, 11, 12
01/17/05   jqi     Lint fix.
01/11/04   jqi     Lint fixes for newer version of Lint 7.50ab
01/06/04   jqi     Lint fixes.
01/06/05   sj      Fixed issue of ACQ event being circulated to different
                   engine which did not issue the corresponding ACQ action.
                   May happen because of PREV_SCR.
12/04/04   jqi     Power save mode optimization for GSM/UMTS targets.
10/26/04   jqi     Read subscription items only when the subscription is
                   available.
10/08/04   jqi     Fixed a typo.
08/08/04   sj      Added parameter to sdss_subsc_avail_update() to specify
                   SS-Main or SS-HDR. And adjust mode_pref according to
                   subscription status on SS-HDR.
08/26/04   jqi     Added WCDMA 1800 band support.
08/26/04   jqi     Lint fix.
08/20/04   sj      Added support for Auto A/B.
08/06/04   sj      Check for collocated association wih 1x srv_sys only if it
                   is populated.
08/03/04   sj      Added function to query the operating mode.
07/29/04   jqi     Quadband optimizatioin - eliminate scheduling.
07/15/04   sj      In throttle state _enter_pwr_save, ignore if throttle
                   is disabled or not and move to power_save state.
07/14/04   sj      Changed calls to update_srv_status to include srv_domain.
                   Changed issue with updating prev_*_pref in update_pref().
                   Added support for construct to check if srv domain pref is
                   CS+PS.
                   Added support for construct to check for current registered
                   srv_domain.
07/09/04   sj      For pref_update & is_pref_chgd, compare only the filtered
                   mode/band preferences.
06/30/04   RC      Now incrementing the schedule of a system only if the
                   scheduled acquisition attempt completes.
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
06/22/04   jqi     Capable to skip the system record in schedule list.
06/21/04   RC      Improve association (collocation) determination algorithm
                   for HDR systems while in hybrid operation - CR 44638.
06/21/04   jqi     Use sd nam selection indication when there is a
                   subscription change.
06/07/04   jqi     More quad-band enhancement for automatic mode.
06/02/04   sj      Place phone in emergency mode if GW is unavailable.
05/25/04   jqi     Fixed mode preference reset issue after power up.
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
05/07/04   jqi     Fixed the typo.
05/06/04   jqi     Renamed T32, KPCS, BC5,6 with band mapping features.
                   Added support for FEATURE_RF_GET_BAND_CAPABILITY.
05/03/04   jqi     Added UMTS 1700.850,800 band support.
04/19/04   jqi     Lint fix - for error level : error.
04/14/04   sj      Fixed issue with default acq. throttle table.
04/13/04   sj      Added support for acquisition throttling.
04/09/04   jqi     Print out band preference in sdss_act_print.
03/18/04   jqi     Kept band and mode consistent before passing to the GW
                   stack. Filtered out unqalified system record during GW PWR
                   SCAN.
02/27/04   jqi     Eliminate system scan whose bands does not comply with
                   target capability or system mode or mode preference.
02/06/04   SJ      Changed param type for get_acc_prm().
01/23/04   JQ      Passed down most recent acquired band group to GW stack
                   during early power scan.
01/22/04   SJ      Added support for new script construct is_pref_chgd().
12/12/03   RC      Added support for FEATURE_INT_ROAMING
12/08/03   SJ      Added support for multistage full srv reselection.
11/17/03   SJ      Updates for HDR usability and new hdr power save.
11/10/03   RC      Added support for SDSSSCR_CON_IS_NET_SEL script construct.
11/05/03   JQ      Do not error fatal while validating PRL.
                   More MRU3 updates.
10/31/03   RC      Added GSM/WCDMA support for fall-back to limited service.
10/24/03   RC      Changed SDSR_GW_LST to contain a single system of mode
                   SD_MODE_GW.
10/09/03   jqi     Adjusted band preference enum.
09/24/03   RC      In sdss_eng_scr_con_cont() set timer to 1s upon getting a
                   non-timer event if timer already expired.
08/26/03   SJ      Added ss parameter to sdss_get_pref().
08/18/03   RC      Fixed issues with IS-683D implementation.
07/28/03   RC      Restart OTASP acquisitions if orig_mode = ORIG_START.
06/10/03   RC      Added counter to track CDMA available indications.
05/23/03   RC      Initialize new_acq_cnt to 200 in sdss_init().
05/23/03   RC      Set scr_timer=0 when moving to next script construct.
05/21/03   RC      Fixed a problem with time based schedule initialization.
05/07/03   JQ      CR27630-MSG_LEVEL reference cleanup.
05/06/03   JQ      Added IS683B PRL support.
04/28/03   RC      Added support for event logging.
04/23/03   SJ      Added early power scan support for GSM/WCDMA.
03/07/03   SJ      Changed MSG level for few debug messages.
02/26/03   SJ      Added support for Band Class 8 & 9.
01/20/03   jqi     Added support for band class 6.
01/16/03   SJ      Featurized use of CAI defines.
12/17/02   SJ      Return TRUE in band_pref supported for GSM/WCDMA/GPS.
12/09/02   RC      Added support for FEATURE_JCDMA.
11/07/02   RC      Added support for new hybrid-HDR power-save algorithm.
07/17/02   RC      Added error recovery when the END script construct is
                   reached.
07/01/02   RC      Added error recovery when the SS-Client reports a system
                   selection event that is not allowed relative to the
                   current state of SD.
05/29/02   RC      Set the scr_timer to 0 when it is expired.
05/02/02   RC      Changed some SD_MSG_HIGH to SD_MSG_LOW.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
02/12/02   RC      Added sdss_eng_stack_get_state().
01/25/02   sj      Added support for Tiered Service - User Zone feature.
01/23/02   RC      Added support for Rel. 3.0 CDMA-HDR hybrid operation.
12/12/01   RC      Added support for CDMA-HDR hybrid operation.
12/07/01   jqi     Added support for band class 5.
10/30/01   RC      Added support for IS-683C collcated systems.
09/07/01   RC      Added support for T53 and JCDMA features.
07/20/01   RC      Added support for IS-683C.
05/23/01   RC      Eliminated compiler warnings.
04/26/01   RC      Modified reacquisition schedules.
03/27/01   RC      Changes to support tri-mode targets.
03/09/01   RC      Changes per 11th week of I&T.
02/26/01   RC      Changes to support dual-mode targets.
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


#include "sdss.h"     /* External interface to sdss.c */
#include "sdssscr.h"  /* External interface to sdssscr.c (SS-Script) */
#include "sdsr.h"     /* External interface to SDSR.C (System Record)*/
#include "sd.h"       /* External interface to sd.c */
#include "sdi.h"      /* Internal interface to sd.c */
#include "sdprl.h"    /* External interface to SDPRL.C */
#include "sdnv.h"     /* External interface to sdnv.c */
#include "sddbg.h"    /* SD debug macros */

#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */
#ifdef SD_USES_CAI
#include "cai.h"      /* Over the air messages definitions */
#endif
#include "clk.h"      /* Interface to clock services */
#include "sys.h"      /* Declarations for common types. */

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FEATURE_MSM_SUPPORT
#include "msm_support.h"    /* Declarations for efuse */
#endif

#include "rfm.h"


/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* <EJECT> */
/*---------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
-------------------------------- ACQUISITION THROTTLE  ----------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/*
                         Throttle acquisition State Machine
                         ---------------------------------

                          +------------------------+
                          |                        |          ( IACT_ACQ ||
                          |                        |          ( PWR_SAVE_ENTER ||
                          |                        |          ( TIMEOUT ) &
                          |         INACT          |<--------- THRTTL_OFF /-----
                          |                        |          => ACQUIRE
                          +------------------------+        ( From any state )
                                     |
                                     |
                                  IACT_ACQ &
                                  THRTTL_ON
                                     |          +--------------------------+
                                     |          ^                          |
                                     |          |                          |
                                     |          |                          |
                                    \|/         |                          |
                         +------------------------+                        |
                         |                        |       IACT_ACQ &       |
           +>----------->|          ACQ           |<------THRTTL_ON &------+
           |             |                        |       acq_count < MAX in stage
           |             +------------------------+
           |                         |
           |                         |
           |                       IACT_ACQ &
           |                       THRTTL_ON &
           |                       acq_count > MAX in stage
           |                         |
           |                        \|/
           |               +------------------------+
           |               |                        |
           |  IACT_ACQ &   |     ENTER_PWR_SAVE     |
           +<-THRTTL_ON/ --|                        |
           |  =>ACQUIRE    |                        |
           |               +------------------------+
           |                          |
           |                          |
           |                      PWR_SAVE_ENTER &
           |                       THRTTL_ON/THRTTL_OFF
           |                          |
           |                         \|/
           |  (IACT_ACQ ||  +------------------------+
           |  TIMEOUT )&    |                        |
           +<-THRTTL_ON/ ---|       PWR_SAVE         |
             =>ACQUIRE      |                        |
                            +------------------------+
*/

/* Enumeration that defines the state of the throttling.
*/
typedef enum {

  SDSS_THRTTL_STATE_INACT,              /* Throttling is not in progress.
                                        */

  SDSS_THRTTL_STATE_ACQ,                /* Throttling is enabled and acquisition
                                        ** is pending.
                                        */

  SDSS_THRTTL_STATE_ENTER_PWR_SAVE,     /* Throttling SM is waiting for power save
                                        ** enter event.
                                        */

  SDSS_THRTTL_STATE_PWR_SAVE,           /* Throttling is in power save.
                                        */

  SDSS_THRTTL_STATE_MAX                 /* Internal use.
                                        */

} sdss_thrttl_state_e_type;



/* Enumeration that defines whether the event was consumed or not.
*/
typedef enum {

  SDSS_THRTTL_EVT_STATUS_CONSUMED,      /* Event given to throttle SM is
                                        ** consumed.
                                        */

  SDSS_THRTTL_EVT_STATUS_NOT_CONSUMED,  /* Event given to throttle SM is
                                        ** not consumed, proceed with rest
                                        ** of processing.
                                        */

  SDSS_THRTTL_EVT_STATUS_MAX

} sdss_thrttl_evt_status_e_type;
/*lint -esym(749, SDSS_THRTTL_EVT_STATUS_MAX ) */ /* Sym not defined */


/* Enum for tri state return values
** TRUE
** FALSE
** Not applicable (INVALID)
*/

typedef enum {

  SDSS_RETURN_STATUS_NONE = -1,  /* Internal use. */

  SDSS_RETURN_STATUS_FALSE,      /* FALSE */

  SDSS_RETURN_STATUS_TRUE,       /* TRUE */

  SDSS_RETURN_STATUS_INVALID,    /* Invalid */

  SDSS_RETURN_STATUS_MAX         /* Internal use. */

} sdss_return_status_e_type;


/* Type that defines the acquisition throttling algorithm in terms of
** delay between stages and number of acquisitions in each stage.
*/
typedef struct {

  /* Delay in seconds from the previous stage.
  */
  dword                     delay;

  /* Number of acquisitions in this stage, if num_acq = 0,
  ** skip this stage.
  */
  unsigned int              num_acq;

} sdss_thrttl_stage_param_s_type;


/* Type for holding the state variables that are associated with the
** acquisition throttle mechanism.
*/
typedef struct {

  /* State of the throttling mechanism.
  */
  sdss_thrttl_state_e_type        state;

  /* Flag to indicate if throttling was enabled.
  */
  boolean                         is_thrttl_on;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Pointer to table which defines acquisition throttle algorithm.
  */
  sdss_thrttl_stage_param_s_type  *thrttl_tbl_ptr;

  /* Number of stages.
  */
  unsigned int                    num_stages;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Current stage in the throttle table.
  */
  unsigned int                    curr_stage;

  /* Field to keep track of number of acquisitions in each stage.
  */
  unsigned int                    acq_count;

  /* The last acqusition action which was throttled.
  */
  sdss_iact_s_type                last_acq_iact;

} sdss_acq_thrttl_s_type;


/* Default acquisition throttling table.
*/
static sdss_thrttl_stage_param_s_type dflt_thrttl_tbl[] =
{
  /* { delay(seconds), number of acquisitions }
  */
  #if defined(SD_DEBUG)
#error code not present
  #else
  {5, 1},
  {5, 1},
  {5, 1},
  {5, 1},
  {10,1}
  #endif
};

/* <EJECT> */
/*---------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
--------------------------------- GENERAL -----------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
---------------------------------------------------------------------------*/


/* Type for holding the state variables that are associated with the system
** selection core.
*/
typedef struct {

  /* Operating mode - represent the current operating mode indication.
  */
  sdss_opr_mode_e_type        opr_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Service status - represent the current service status - no-ervice,
  ** limited service, full service.
  ** Service domain - represent whether it is CS or PS or both.
  */
  sys_srv_status_e_type       srv_status;
  sys_srv_domain_e_type       srv_domain;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Origination mode - represent the current origination mode indication.
  */
  sd_ss_orig_mode_e_type      orig_mode;

  /* The previous origination mode.
  */
  sd_ss_orig_mode_e_type      orig_mode_prev;

  /* The most recent reason for updating the SS-Preference.
  */
  sd_ss_pref_reas_e_type      pref_reason;

  /* Mode, band, roam and hybrid preferences - represent the current system
  ** selection preferences.
  */
  sd_ss_mode_pref_e_type      mode_pref;

  /* Indication whether PRL supports HDR or not based on current BAND PREF.
  */
  boolean                     is_prl_support_hdr;

  sd_ss_mode_pref_e_type      true_mode_pref;
  sd_ss_mode_pref_e_type      user_mode_pref;
  sd_ss_mode_pref_e_type      user_true_mode_pref;
  sd_ss_band_pref_e_type      band_pref;
  sd_ss_band_pref_e_type      user_band_pref;
  sd_ss_prl_pref_e_type       prl_pref;
  sd_ss_roam_pref_e_type      roam_pref;
  sd_ss_hybr_pref_e_type      hybr_pref;

  /* Other preferences related to GSM/WCDMA.
  */
  sd_ss_srv_domain_pref_e_type domain_pref;
  sd_ss_acq_order_pref_e_type  acq_order_pref;

  sd_ss_tech_pref_e_type       tech_pref;
  sd_ss_wlan_fail_reas_e_type  wlan_fail_reason;
  sys_wlan_scan_pref_e_type    wlan_scan_pref;
  sys_wlan_bss_e_type          bss_type_pref;


  /* Previous Mode, band, roam, hybrid, domain and acq_order preferences.
  */
  sd_ss_mode_pref_e_type      prev_mode_pref;
  sd_ss_band_pref_e_type      prev_band_pref;
  sd_ss_prl_pref_e_type       prev_prl_pref;
  sd_ss_roam_pref_e_type      prev_roam_pref;
  sd_ss_hybr_pref_e_type      prev_hybr_pref;
  sd_ss_srv_domain_pref_e_type prev_domain_pref;
  sd_ss_acq_order_pref_e_type  prev_acq_order_pref;
  sd_ss_tech_pref_e_type       prev_tech_pref;
  sys_wlan_scan_pref_e_type    prev_wlan_scan_pref;
  sys_wlan_bss_e_type          prev_bss_type_pref;


  /* The band-class and PCS frequency block/Cellular system that are
  ** associated with OTASP origination mode. Note that these fields are
  ** ignored when orig_mode != OTASP.
  */
  sd_band_e_type              otasp_band;
  sd_blksys_e_type            otasp_blksys;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The event that triggered the stack processing
  */
  sdss_evt_e_type             true_event;

  boolean                     is_true_event_processed;

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
#error code not present
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* System Selection State - indicates whether the system selection is in
  ** acquisition state or operation state.
  **
  ** Note that being in acquisition state corresponds to the SS-Client being
  ** in initialization state (of CDMA, AMPS or HDR) whereas being in
  ** operation state corresponds to the SS-Client being in states other than
  ** initialization, like idle, access or traffic.
  */
  sdss_state_e_type           ss_state;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Acquisition type - indicate the last acquisition type being attempted.
  */
  sdssscr_acq_e_type          acq_type;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Acquisition start time - record the uptime as of the last time
  ** the START_TIME construct was executed.
  */
  dword                       acq_start_time;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* New-acquisition counter - incremented every time a NEW_ACQ script
  ** construct is encountered.
  **
  ** NOTE! This value should never be set to 0 (as 0 is reserved to indicate
  ** that the new-acquisition counter shall be ignored).
  */
  byte                        new_acq_cnt;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Script execution control counter. This counter is being incremented each
  ** time control is returned to the SS-Front-end. Its is being used for
  ** detecting SS-Script loops that does not return control to the
  ** SS-Front-end.
  */
  int                         scr_ctrl_cnt;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Redirection parameters - represent the current redirection,
  ** RETURN_IF_FAILED, IGNORE_CDMA, Max Redirect Delay and redirection
  ** uptime.
  **
  ** Redirection - represent the redirection type.
  **
  ** Is valid - indicate whether the redirection is valid for this target.
  **
  ** Expected SID - If the MS is being redirected to a specific system,
  ** indicate the SID of that system; otherwise, set to SD_WILDCARD_SID.
  **
  ** Expected NID - If the MS is being redirected to a specific network,
  ** indicate the NID of that network; otherwise, set to SD_WILDCARD_NID.
  **
  ** RETURN_IF_FAILED indicate whether the MS is required to return to the
  ** system from which it is redirected upon failure to obtain service using
  ** the redirection criteria.
  **
  ** IGNORE_CDMA indicate whether the MS is to ignore the CDMA Capability
  ** Message (i.e. CDMA available indication) on the AMPS system to which
  ** the MS is being redirected.
  **
  ** Maximum redirect delay. When the BS redirects the MS from a CDMA system
  ** to an AMPS system (using GSRDM), this field indicate the maximum
  ** redirect delay value (in units of 8 second increments) to be used in the
  ** AMPS registration randomization algorithm. Note that the
  ** max_redirect_delay is a redirection parameter that needs to be passed
  ** into AMPS CP - SD is merely a path-through. This variable is ignored
  ** when it is set to SD_SS_MAX_REDIR_DELAY_NONE.
  **
  ** Redirection uptime - record the uptime when the most recent redirection
  ** indication was received from the SS-front-end.
  */
  sd_ss_redir_e_type          redir;
  boolean                     redir_is_valid;
  word                        redir_expected_sid;
  word                        redir_expected_nid;
  boolean                     redir_is_rif;
  boolean                     redir_is_ignore_cdma;
  word                        redir_max_redirect_delay;
  dword                       redir_uptime;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Handoff - represent the current handoff indication.
  */
  sd_ss_ho_e_type             handoff;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Access reason and termination - represent the most recent access reason
  ** and access termination reason.
  */
  sd_ss_acc_reas_e_type       acc_reas;
  sd_ss_acc_term_e_type       acc_term;

  /* Indicate how many times registration access has failed continuously.
  */
  int                         acc_fail_cnt;

  /* Indicate the uptime where registration access have started failing.
  */
  dword                       acc_fail_start_time;

  /* CDMA-Available counter - indicates how many times MS tries to acquire
  ** CDMA as a result of CDMA-available indication. This counter is being
  ** reset upon the following conditions:
  ** 1. Successful CDMA acquisition.
  ** 2. CDMA/AMPS registration or implicit registration.
  */
  int                         cdma_avail_cnt;

  /* HDR session close - represent the most recent HDR session close reason.
  */
  sd_ss_ses_close_e_type      ses_close;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* CDMA locked - represent the CDMA locked until power cycle indication.
  */
  boolean                     is_cmda_locked;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SIM state, applicable for GSM/WCDMA networks.
  */
  sys_sim_state_e_type        sim_state;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SRDA - indicate whether the silent redial with AMPS feature is enabled.
  */
  boolean                     is_srda;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reselection state - represent the current reselection state.
  */
  sdss_resel_stt_e_type       resel_state;

  /* Reselection mode - represent the current reselection mode.
  */
  sdss_resel_mode_e_type      resel_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Flag to indicate if GW subscription is available.
  */
  boolean                     is_gw_subsc_avail;

  /* Flag to indicate if 1X subscription is available.
  */
  boolean                     is_1x_subsc_avail;

  /* Flag to indicate if WLAN subscription is available.
  */
  boolean                     is_wlan_subsc_avail;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Acquisition throttle state information.
  */
  sdss_acq_thrttl_s_type      acq_thrttl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* band capability.
  */
  sd_ss_band_pref_e_type      band_capability;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* mode pref for get network request
  */
  sd_ss_mode_pref_e_type      get_net_mode_pref;

  /* band pref for get network request.
  */
  sd_ss_band_pref_e_type      get_net_band_pref;


  /* The list, the starting list position of get network request, the list
  ** position of the last get network request.
  */
  sdsr_e_type                 get_net_list;

  int                         get_net_list_start_pos;

  int                         get_net_list_pos;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Acquisition list and position - indicates the list and position
  ** most recently attempted acquisition.
  */
  sdsr_e_type                 acq_list;
  int                         acq_pos;

  /* Assosiation tag of the Previous System - indicates the association tag
  ** , from prl, of the recent succesfully connected CDMA System.
  **
  ** Updates: Set to ass-tag of the CDMA system acquired.
  **          Clear the ass-tag when trying for GSM system.
  **
  ** Used to determine the realign_hdr flag, for CM to kick HDR
  */
  sdss_sys_addn_info_s_type  prev_sys_info;

  /* Last engine which issue the acquisition action.
  ** Made void else needs forward declaration.
  */
  void*                       last_acq_eng_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate how many times session negotiation or access has timed out
  ** continuously.
  */
  int                         event_cnt[SDSS_CNTED_EVT_MAX];

  /* Indicate the uptime where session negotiation or access started timing
  ** out.
  */
  dword                       event_uptime[SDSS_CNTED_EVT_MAX];

  /* Indicate the system SID associated with the event.
  **
  ** Initial to 0.
  **
  ** Updated when the system SID is changed.
  **
  */
  sd_sid2_u_type              event_sid;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate the uptime when a continue wait timer should be started
  **
  ** The uptime can be zero out during power up or clear it explicitly in
  ** the script.
  **
  */
  dword                       cont_uptime[SDSSSCR_CONT_TYPE_MAX];


  /* The very recent HDR Activity start/stop uptime -indicates the uptime in
  ** seconds when activity is started or stopped, which can used to get the
  ** the duration of the very recent HDR activity.
  */
  dword                       hdr_bcmcs_start_uptime;

  dword                       hdr_bcmcs_stop_uptime;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate user preference updated status upon user ss pref event.
  */
  sdss_pref_updated_e_type    ss_pref_updated_status;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate the system lost reason upon system lost event - represent the
  ** most recent system lost reason.
  **
  ** Initialize: None at Power up
  **
  ** Update: whereeven there is a HDR system lost operation event reported.
  */
  sd_ss_sys_lost_e_type       sys_lost;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Parameters for GW uniform scan
  */

  /* Flag to check GCF testing
  */
  boolean                     gprs_anite_gcf;

  /* GW scan status
  */
  sd_ss_gw_scan_status_e_type acq_gw_scan_status;


  /* Use timer, the timer values are set to valid values
  */
  boolean                     acq_gw_use_timer;

  /* Is this a new acquisition or the continuation of previous scan
  */
  boolean                     acq_gw_new_acq;

  /* Total time the scan should be completed
  */
  dword                       acq_gw_total_scan_time;

  /* Time for UMTS scan
  */
  dword                       acq_gw_wcdma_scan_time;

  /* Time for gsm scan
  */
  dword                       acq_gw_gsm_scan_time;

  /* The service request type to be sent to NAS */
  sd_ss_srv_req_type_e_type   srv_req_type;

  /* Populate the mode preference based on the featurization, HW capability,
  ** filter out the mode without any supported bands, and filter out the band
  ** without any supported mode.
  */
  sd_ss_mode_pref_e_type      mode_capability;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
  sd_ss_mode_pref_e_type      stack_mode_pref[SD_SS_MAX];
  #endif

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dword                       rem_uoos_awake_time;
  /* Time in milliseconds that determines how much more UE should search for service
  ** before going to sleep. This field is used when UE loses service in
  ** connected mode in GW (DCH)
  */

  dword                       rem_uoos_sleep_time;
  /* Time in milliseconds that determines how much more UE should sleep before
  ** searching for service. This field is used when UE loses service in
  ** connected mode in GW (DCH)
  */


  boolean                    is_conn_uoos_awake;
  /* Flag to track the connected mode UOoS awake condition
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  boolean                     is_eng_scr_timer_set;
  /* Flag to check if the script timer is set
  */

  dword                       eng_scr_timer;
  /* the script timer
  */

  dword                       bsr_timer;
  /* Uptime for BSR
  */

  sd_sid2_u_type          bsr_sid;
  /* SID/PLMN for which BSR timer is started 
  */

  sd_sys_s_type        bsr_sys;

  byte                 gw_pwr_save_acq_stage_val;
  /* GW pwr save acq stage value.
  */
#ifdef FEATURE_IS683A_450M_SUPPORT 
  boolean                is_IS683A_450M_support; 
#endif 
  /* Flat states whether we support 450M bandclass in PRL version 683A.
  */ 

} sdss_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* <EJECT> */
/*---------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
-------------------------------- SCHEDULING ---------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/* Array of reacquisition schedules.
**
** This array is indexed using sdssscr_sched_e_type enum values. The ith
** element in this array represent the time in seconds that corresponds to
** the ith sdssscr_sched_e_type enum value. SCHED_START, SCHED_END and
** SCHED_LOOP are magic values that represents the beginning and end of each
** reacquisition schedules.
*/
#define SCHED_START ((byte) (-1))
#define SCHED_END   ((byte) (-2))
#define SCHED_LOOP  ((byte) (-3))

static const byte   sdss_sched_arr[SDSSSCR_SCHED_MAX] = {

  /* ------------------------------------
  ** Time based schedules (low priority).
  ** ------------------------------------
  */
  SCHED_END,    /* SDSSSCR_SCHED_REACQ_TIME_LOW - for range checking */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Power-up.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_PWR_UP */
  0,            /* SDSSSCR_SCHED_REACQ_PWR_UP_1 */
  1,            /* SDSSSCR_SCHED_REACQ_PWR_UP_2 */
  SCHED_END,    /* SDSSSCR_SCHED_REACQ_PWR_UP_3 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------------
  ** Time based schedules (high priority).
  ** ------------------------------------
  */
  SCHED_END,    /* SDSSSCR_SCHED_REACQ_TIME_HIGH - for range checking */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire 1 time.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_1_TIME */
  0,            /* SDSSSCR_SCHED_REACQ_1_TIME_1 */
  SCHED_END,    /* SDSSSCR_SCHED_REACQ_1_TIME_2 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire 2 times.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_2_TIMES */
  0,            /* SDSSSCR_SCHED_REACQ_2_TIMES_1 */
  0,            /* SDSSSCR_SCHED_REACQ_2_TIMES_2 */
  SCHED_END,    /* SDSSSCR_SCHED_REACQ_2_TIMES_3 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire every 1s.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EVERY_1S */
  1,            /* SDSSSCR_SCHED_REACQ_EVERY_1S_1 */
  SCHED_LOOP,   /* SDSSSCR_SCHED_REACQ_EVERY_1S_2 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire every 2s.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EVERY_2S */
  2,            /* SDSSSCR_SCHED_REACQ_EVERY_2S_1 */
  SCHED_LOOP,   /* SDSSSCR_SCHED_REACQ_EVERY_2S_2 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire every 4s.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EVERY_4S */
  4,            /* SDSSSCR_SCHED_REACQ_EVERY_4S_1 */
  SCHED_LOOP,   /* SDSSSCR_SCHED_REACQ_EVERY_4S_2 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire every 2s.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EVERY_8S */
  8,            /* SDSSSCR_SCHED_REACQ_EVERY_8S_1 */
  SCHED_LOOP,   /* SDSSSCR_SCHED_REACQ_EVERY_8S_2 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* System lost during operation.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_SYS_LOST */
  0,            /* SDSSSCR_SCHED_REACQ_SYS_LOST_1 */
  0,            /* SDSSSCR_SCHED_REACQ_SYS_LOST_2 */
  0,            /* SDSSSCR_SCHED_REACQ_SYS_LOST_3 */
  1,            /* SDSSSCR_SCHED_REACQ_SYS_LOST_4 */
  1,            /* SDSSSCR_SCHED_REACQ_SYS_LOST_5 */
  2,            /* SDSSSCR_SCHED_REACQ_SYS_LOST_6 */
  2,            /* SDSSSCR_SCHED_REACQ_SYS_LOST_7 */
  4,            /* SDSSSCR_SCHED_REACQ_SYS_LOST_8 */
  4,            /* SDSSSCR_SCHED_REACQ_SYS_LOST_9 */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_SYS_LOST_10 */
  8,            /* SDSSSCR_SCHED_REACQ_SYS_LOST_11 */
  SCHED_LOOP,   /* SDSSSCR_SCHED_REACQ_SYS_LOST_12 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* System lost during acquisition.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_ACQ_LOST */
  0,            /* SDSSSCR_SCHED_REACQ_ACQ_LOST_1 */
  0,            /* SDSSSCR_SCHED_REACQ_ACQ_LOST_2 */
  1,            /* SDSSSCR_SCHED_REACQ_ACQ_LOST_3 */
  2,            /* SDSSSCR_SCHED_REACQ_ACQ_LOST_4 */
  2,            /* SDSSSCR_SCHED_REACQ_ACQ_LOST_5 */
  4,            /* SDSSSCR_SCHED_REACQ_ACQ_LOST_6 */
  4,            /* SDSSSCR_SCHED_REACQ_ACQ_LOST_7 */
  8,            /* SDSSSCR_SCHED_REACQ_ACQ_LOST_8 */
  8,            /* SDSSSCR_SCHED_REACQ_ACQ_LOST_9 */
  SCHED_END,    /* SDSSSCR_SCHED_REACQ_ACQ_LOST_10 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* AMPS system lost during operation.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST */
  0,            /* SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_1 */
  1,            /* SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_2 */
  1,            /* SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_3 */
  2,            /* SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_4 */
  2,            /* SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_5 */
  4,            /* SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_6 */
  4,            /* SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_7 */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_8 */
  8,            /* SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_9 */
  SCHED_LOOP,   /* SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_10 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* AMPS control channel acquisition failure.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST */
  0,            /* SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_1 */
  1,            /* SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_2 */
  1,            /* SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_3 */
  2,            /* SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_4 */
  2,            /* SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_5 */
  4,            /* SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_6 */
  4,            /* SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_7 */
  SCHED_END,    /* SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_8 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -------------------------------------
  ** Count based schedules (low priority).
  ** -------------------------------------
  */
  SCHED_END,    /* SDSSSCR_SCHED_REACQ_CNT_LOW - for range checking */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -------------------------------------
  ** Count based schedules (high priority).
  ** -------------------------------------
  */
  SCHED_END,    /* SDSSSCR_SCHED_REACQ_CNT_HIGH - for range checking */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire immidiately and alternate with 1 system there after.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EVERY_1 */
  0,            /* SDSSSCR_SCHED_REACQ_EVERY_1_1 */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EVERY_1_2 */
  1,            /* SDSSSCR_SCHED_REACQ_EVERY_1_3 */
  SCHED_LOOP,   /* SDSSSCR_SCHED_REACQ_EVERY_1_4 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire immidiately and alternate with 2 systems there after.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EVERY_2 */
  0,            /* SDSSSCR_SCHED_REACQ_EVERY_2_1 */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EVERY_2_2 */
  2,            /* SDSSSCR_SCHED_REACQ_EVERY_2_3 */
  SCHED_LOOP,   /* SDSSSCR_SCHED_REACQ_EVERY_2_4 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire immidiately and alternate with 3 systems there after.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EVERY_3 */
  0,            /* SDSSSCR_SCHED_REACQ_EVERY_3_1 */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EVERY_3_2 */
  3,            /* SDSSSCR_SCHED_REACQ_EVERY_3_3 */
  SCHED_LOOP,   /* SDSSSCR_SCHED_REACQ_EVERY_3_4 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire immidiately and alternate with 4 systems there after.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EVERY_4 */
  0,            /* SDSSSCR_SCHED_REACQ_EVERY_4_1 */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EVERY_4_2 */
  4,            /* SDSSSCR_SCHED_REACQ_EVERY_4_3 */
  SCHED_LOOP,   /* SDSSSCR_SCHED_REACQ_EVERY_4_4 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire immidiately and alternate with 8 systems there after.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EVERY_8 */
  0,            /* SDSSSCR_SCHED_REACQ_EVERY_8_1 */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EVERY_8_2 */
  8,            /* SDSSSCR_SCHED_REACQ_EVERY_8_3 */
  SCHED_LOOP,   /* SDSSSCR_SCHED_REACQ_EVERY_8_4 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Emergency system lost/access failure.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_EMERG_LOST */
  0,            /* SDSSSCR_SCHED_REACQ_EMERG_LOST_1 */
  3,            /* SDSSSCR_SCHED_REACQ_EMERG_LOST_2 */
  3,            /* SDSSSCR_SCHED_REACQ_EMERG_LOST_3 */
  3,            /* SDSSSCR_SCHED_REACQ_EMERG_LOST_4 */
  6,            /* SDSSSCR_SCHED_REACQ_EMERG_LOST_5 */
  6,            /* SDSSSCR_SCHED_REACQ_EMERG_LOST_6 */
  6,            /* SDSSSCR_SCHED_REACQ_EMERG_LOST_7 */
  SCHED_END,    /* SDSSSCR_SCHED_REACQ_EMERG_LOST_8 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* GW full system acquisition failure.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST */
  0,            /* SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST_1 */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST_2 */
  1,            /* SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST_3 */
  1,            /* SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST_4 */
  1,            /* SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST_5 */
  3,            /* SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST_6 */
  SCHED_LOOP,   /* SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST_7 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* GW ltd system acquisition failure.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST */
  1,            /* SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST_1 */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST_2 */
  1,            /* SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST_3 */
  1,            /* SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST_4 */
  1,            /* SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST_5 */
  3,            /* SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST_6 */
  SCHED_LOOP,   /* SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST_7 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* GW full system acquisition failure when orig is not manual or none.
  */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_GW_FULL_ORIG */
  0,            /* SDSSSCR_SCHED_REACQ_GW_FULL_ORIG_1 */
  SCHED_START,  /* SDSSSCR_SCHED_REACQ_GW_FULL_ORIG_2 */
  0,            /* SDSSSCR_SCHED_REACQ_GW_FULL_ORIG_3 */
  0,            /* SDSSSCR_SCHED_REACQ_GW_FULL_ORIG_4 */
  0,            /* SDSSSCR_SCHED_REACQ_GW_FULL_ORIG_5 */
  1,            /* SDSSSCR_SCHED_REACQ_GW_FULL_ORIG_6 */
  SCHED_LOOP,   /* SDSSSCR_SCHED_REACQ_GW_FULL_ORIG_7 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SCHED_END,    /* SDSSSCR_SCHED_REACQ_LAST */

  SCHED_END,    /* SDSSSCR_SCHED_AVOID */

  SCHED_END,    /* SDSSSCR_SCHED_AVOID_UNTIL_ACQ_FAIL */

  SCHED_END,    /* SDSSSCR_SCHED_AVOID_UNTIL_ACQ_FAIL_DUR_CALL_ORIG */

  SCHED_END,    /* SDSSSCR_SCHED_AVOID_CANCEL_ALL */

  SCHED_END,    /* SDSSSCR_SCHED_AVOID_LAST */
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of schedule actions.
*/
typedef enum {

  SDSS_SCHED_ACT_ACQ,   /* Schedule is calling for acquisition attempt */
  SDSS_SCHED_ACT_WAIT,  /* Schedule is not calling for acquisition attempt */
  SDSS_SCHED_ACT_END,   /* Schedule is exhausted */

  SDSS_SCHED_ACT_MAX
} sdss_sched_act_e_type;
/*lint -esym(749, SDSS_SCHED_ACT_MAX) */ /* Sym not defined */



/* <EJECT> */
/*---------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
------------------------- ACQUISITION STATE MACHINE -------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
---------------------------------------------------------------------------*/


/* Type for holding all the information that is associated with a acquisition
** state machine. The acquisition state machine carries the acquisition
** process on behalf of the ACQ script construct.
*/
typedef struct {

  /* System selection enum type.
  */
  sd_ss_e_type             ss;

  /* Pointer to associated System Selection information structure.
  */
  sdss_s_type             *ss_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The state of the acquisition state machine.
  */
  sdss_acq_stt_e_type     state;

  /* The previous state of the acquisition state machine.
  */
  sdss_acq_stt_e_type     prev_state;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Acquisition list (i.e.m system record list over which acquisition is
  ** attempted).
  */
  sdsr_e_type             acq_lst;

  /* Acquisition list starting position.
  */
  int                     acq_start_pos;

  /* Number of acquisitions left.
  */
  int                     acq_left;

  /* Max uptime for the acquisition attempts.
  */
  dword                   acq_max_uptime;

  /* Acquisition list traversal position.
  */
  int                     acq_pos;

  /* List end reached indicator.
  */
  boolean                 is_acq_lst_end_reached;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Acquisition script construct that is calling for the acquisition
  ** attempt.
  */
  sdssscr_con_e_type      acq_con;

  /* The acquisition type that is being attempted.
  */
  sdssscr_acq_e_type      acq_type;

  /* The acquisition mode that is being attempted.
  */
  sdss_acq_mode_e_type    acq_mode;

  /* Used to maintain acq_mode across scripts, for some GW scan, we need to
  ** use the same acq_mode for continuing the scan
  */
  sdss_acq_mode_e_type    prev_acq_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The current value of the new-acquisition counter. Only system with
  ** different new-acquisition count are being attempted acquisition.
  **
  ** The new-acquisition counter serves as a mechanism for avoiding
  ** acquisition attempts over systems that were already attempted
  ** acquisition since the last time the NEW_ACQ script constructs was
  ** encountered.
  **
  ** The new-acquisition counter shall be ignored when it is set to 0.
  */
  byte                    new_acq_cnt;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The measurement value with which systems have to comply in order to be
  ** attempted acquisition. That is, only systems with measurement value >=
  ** than this value are being attempted acquisition.
  */
  int2                    meas_val;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The measurement category with which systems have to comply in order to be
  ** attempted acquisition above meas_val threshold otherwise threshold will be ignored 
  ** for them. That is, meas_cat systems with measurement value >= than meas 
  ** value and all others are being attempted acquisition.
  */
  sdsr_cat_e_type         meas_cat;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate whether MS tries to acquire the 2nd strongest AMPS control
  ** channel.
  */
  boolean                 is_amps_2nd_cch;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The system acquired most recently in the SDSS_ACQ_STT_START state.
  */
  sdsr_e_type             start_sys;

  /* The list that contains all the systems that are more preferred than
  ** start_sys, along with traversal position.
  **
  ** This list is being traversed during the SDSS_ACQ_STT_MORE_PREF state.
  */
  sdsr_e_type             pref_lst;
  int                     pref_pos;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The system record list that keeps track of all the systems that are on a
  ** reacquisition schedule, as well as, traversal position.
  **
  ** Scheduled reacquisition attempts are given priority over the list that
  ** is being attempted acquisition. Each time before attempting to acquire a
  ** new system out of the acquisition list, this list is being consulted as
  ** to whether the reacquisition schedule of a particular system record is
  ** calling for a reacquisition attempt at the current time, at which case
  ** such a system is being attempted acquisition first.
  */
  sdsr_e_type             sched_lst;
  int                     sched_pos;

  /* Last rtrn action from the ACQ construct.
  */
  sdss_iact_s_type        rtrn_iact;

} sdss_acq_s_type;




/* <EJECT> */
/*---------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
----------------------------- SS-SCRIPT-ENGINE ------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                         Script Execution Control
---------------------------------------------------------------------------*/

/* Enumeration of SS-Script execution controls.
*/
typedef enum {

  SDSS_SCR_CTRL_RTRN_ACT,   /* Return SS-Action to SS-Client */

  SDSS_SCR_CTRL_NEXT_CONS,  /* Parse the next SS-Script construct */

  SDSS_SCR_CTRL_PREV_SCR,   /* Return to previously active SS-Script and
                            ** continue with that script execution */

  SDSS_SCR_CTRL_ACT_SCR,    /* Activate a new SS-Script (as specified by the
                            ** ACT_SCR's parameters) and continue with that
                            ** script execution */

  SDSS_SCR_CTRL_PROC_EVT,   /* Process an SS-Event (as specified by the
                            ** PROC_EVT's parameters */

  SDSS_SCR_CTRL_MAX
} sdss_scr_ctrl_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for parameters that are associated with the SDSS_SCR_CTRL_ACT_SCR
** script execution control.
*/
typedef struct {

  sdssscr_e_type            script;  /* SS-Script which to activate */

} sdss_scr_ctrl_act_scr_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for parameters that are associated with the SDSS_SCR_CTRL_PROC_EVT
** script execution control.
*/
typedef struct {

  sdss_evt_e_type           event;   /* SS-Event to process */

} sdss_scr_ctrl_proc_evt_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Types for holding an SS-Script-engine execution control information.
*/
typedef struct {

  /* Execution control.
  */
  sdss_scr_ctrl_e_type                        ctrl;

  /* Execution control associated parameters.
  */
  union {
    sdss_scr_ctrl_act_scr_s_type    act;
    sdss_scr_ctrl_proc_evt_s_type   proc;
  }                                           prm;

} sdss_scr_ctrl_s_type;


/*---------------------------------------------------------------------------
                         Script Engine Information
---------------------------------------------------------------------------*/

/* Type for holding the information that is associated with a single instance
** of a script engine.
*/
typedef struct {

  /* System selection enum type.
  */
  sd_ss_e_type             ss;

  /* Pointer to associated System Selection information structure.
  */
  sdss_s_type             *ss_ptr;

  /* Parser for keeping track of the SS-Scripts that is being parsed.
  */
  sdssscr_parser_s_type   parser;

  /* Keep track of script's event group and table index (for debugging).
  */
  sdss_evt_grp_e_type     scr_evt_grp;
  int                     scr_tbl_idx;

  /* Buffers to hold the SS-Script Construct that is being executed, as well
  ** as, the construct's parameters.
  */
  sdssscr_con_e_type      scr_con;
  sdssscr_prm_type        con_prm1;
  sdssscr_prm_type        con_prm2;
  sdssscr_prm_type        con_prm3;
  sdssscr_prm_type        con_prm4;
  sdssscr_prm_type        con_prm5;
  sdssscr_prm_type        con_prm6;


  /* Script control element to control the execution of the SS-Script.
  */
  sdss_scr_ctrl_s_type    scr_ctrl;


  /* Acquisition state machine to handle the acquisition processing.
  */
  sdss_acq_s_type         acq;


  /* The mode of the acquisition being attempted.
  */
  sdss_acq_mode_e_type    acq_mode;


  /* Script timer. This timer is set to the uptime when the script timer
  ** should expire if the timer is currently active. It is set to 0 when the
  ** script timer is disabled.
  */
  dword                   scr_timer;

  /* Event associated with the script
  */
  sdss_evt_e_type         event;

} sdss_eng_s_type;


/*---------------------------------------------------------------------------
                          Script Engine Stack
---------------------------------------------------------------------------*/


/* Type for a stack of script engines.
*/
typedef struct {

  /* Pointer to array of script engines.
  */
  sdss_eng_s_type   *arr_ptr;

  /* Size of the engines array (in elements).
  */
  unsigned int      size;

  /* Number of engines that are pushed on the stack.
  */
  unsigned int      cnt;

} sdss_eng_stack_s_type;


/* SS-Script-engine stack to process SS-Events. The stack has a depth of 12
** to support 12 levels of script nesting.
*/
static sdss_eng_s_type  sdss_eng_arr[12];
static sdss_eng_stack_s_type  sdss_eng_stack;

#if (defined (FEATURE_TECH_IND_SD_STACK_ARCH) || defined (FEATURE_HDR_HYBRID) )
static sdss_eng_s_type  sdss_hybr_1_eng_arr[12];
static sdss_eng_stack_s_type  sdss_hybr_1_eng_stack;
#endif

#ifdef FEATURE_WLAN
#error code not present
#endif

#ifdef FEATURE_SD_PREVENT_SCRIPT_MULTI_INST_EXEC
#define SDSS_STACK_POS_NULL ((unsigned int) (-1))
#endif




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
================================ GENERAL ====================================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdss_ptr

DESCRIPTION
  Return the pointer to SS component.


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdss_s_type*             sdss_ptr(

        sd_ss_e_type              ss
            /* System selection item.
            */

)
{

  /* System Selection.
  */
  static sdss_s_type    sdss;

  #if ( defined( FEATURE_TECH_IND_SD_STACK_ARCH) || defined(FEATURE_HDR_HYBRID))
  static sdss_s_type    sdss_hybr_1;
  #endif

  #ifdef FEATURE_WLAN
#error code not present
  #endif

  static sdss_s_type*  sdss_arr[SD_SS_MAX] =
  {
    &sdss,

    #if ( defined( FEATURE_TECH_IND_SD_STACK_ARCH) || defined(FEATURE_HDR_HYBRID))
    &sdss_hybr_1,
    #else
    &sdss,
    #endif

    #ifdef FEATURE_WLAN
#error code not present
    #else
    &sdss,
    #endif

  };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ss < SD_SS_MAX );

  return sdss_arr[ss];

} /* sdss_ptr() */


/*===========================================================================

FUNCTION sdss_map_event_to_group

DESCRIPTION
  Map SS-Event to SS-Event Group.

DEPENDENCIES
  None.

RETURN VALUE
  SS-Event group that is associated with the input SS-Event.
  SDSS_EVT_GRP_MAX if event is not associated with any event group.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdss_evt_grp_e_type           sdss_map_event_to_group(

        sdss_evt_e_type               event
            /* SS-Event for which to get event group.
            */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SDSS_EVT_IS_USER(event) ) {
    return SDSS_EVT_GRP_USER;
  }

  else if( SDSS_EVT_IS_MISC(event) ) {
    return SDSS_EVT_GRP_MISC;
  }

  else if( SDSS_EVT_IS_INT(event) ) {
    return SDSS_EVT_GRP_INT;
  }

  else if( SDSS_EVT_IS_CDMA_ACQ(event) ) {
    return SDSS_EVT_GRP_CDMA_ACQ;
  }

  else if( SDSS_EVT_IS_CDMA_OPR(event) ) {
    return SDSS_EVT_GRP_CDMA_OPR;
  }

  else if( SDSS_EVT_IS_AMPS_ACQ(event) ) {
    return SDSS_EVT_GRP_AMPS_ACQ;
  }

  else if( SDSS_EVT_IS_AMPS_OPR(event) ) {
    return SDSS_EVT_GRP_AMPS_OPR;
  }

  else if( SDSS_EVT_IS_HDR_USER(event) ) {
    return SDSS_EVT_GRP_HDR_USER;
  }

  else if( SDSS_EVT_IS_HDR_ACQ(event) ) {
    return SDSS_EVT_GRP_HDR_ACQ;
  }

  else if( SDSS_EVT_IS_HDR_OPR(event) ) {
    return SDSS_EVT_GRP_HDR_OPR;
  }

  else if( SDSS_EVT_IS_GW_ACQ(event) ) {
    return SDSS_EVT_GRP_GW_ACQ;
  }

  else if( SDSS_EVT_IS_GW_OPR(event) ) {
    return SDSS_EVT_GRP_GW_OPR;
  }

  else if( SDSS_EVT_IS_WLAN_USER(event) ) {
    return SDSS_EVT_GRP_WLAN_USER;
  }

  else if( SDSS_EVT_IS_WLAN_ACQ(event) ) {
    return SDSS_EVT_GRP_WLAN_ACQ;
  }

  else if( SDSS_EVT_IS_WLAN_OPR(event) ) {
    return SDSS_EVT_GRP_WLAN_OPR;
  }
  if( SDSS_EVT_IS_HYBR_1_USER(event) ) {
    return SDSS_EVT_GRP_HYBR_1_USER;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the input SS-Event is out of range - return
  ** SDSS_EVT_GRP_MAX.
  */
  SD_CASE_ERR( "event",0,0,0 );
  return SDSS_EVT_GRP_MAX;

} /* sdss_map_event_to_group */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_opr_mode

DESCRIPTION
  Check whether a specified operating mode value includes the current
  operating mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified operating mode value includes the current operating
  mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_is_opr_mode(

        sdssscr_opr_mode_val_e_type   opr_mode_val
            /* Operating mode value.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( opr_mode_val, SDSSSCR_OPR_MODE_VAL_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified operating mode value includes the current operating mode,
  ** return TRUE. Else, return FALSE.
  */
  if(SD_IS_BIT_CONTAIN( opr_mode_val, BM(ss_ptr->opr_mode)) )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_is_opr_mode */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_opr_mode

DESCRIPTION
  Returns the current operating mode for the specified ss.

DEPENDENCIES
  None.

RETURN VALUE
  sdss_opr_mode_e_type

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdss_opr_mode_e_type         sdss_get_opr_mode(

        sd_ss_e_type                  ss
            /* System selection - MAIN or HDR.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the operating mode.
  */
  return ss_ptr->opr_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* sdss_get_opr_mode */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_cdma_locked

DESCRIPTION
  Return the value of the CDMA locked indicator.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the CDMA locked indicator is set to TRUE. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                       sdss_is_cdma_locked( void )
{
  const sdss_s_type   *ss_ptr   = sdss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the value of the CDMA locked indicator.
  */
  return ss_ptr->is_cmda_locked;

} /* sdss_is_cdma_locked */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_pref_reason

DESCRIPTION
  Check whether a specified SS-Preference reason equals the current
  SS-Preference reason.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_is_pref_reason(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HDR.
            */

        sdssscr_pref_reas_val_e_type pref_reason
            /* SS-Preference reason combination type.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( pref_reason, SDSSSCR_PREF_REAS_VAL_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified SS-Preference reason equals the current SS-Preference
  ** reason, return TRUE. Else, return FALSE.
  */
  if( SD_IS_BIT_CONTAIN(pref_reason, BM(ss_ptr->pref_reason)) )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_is_pref_reason */

/*===========================================================================

FUNCTION sdss_is_flag_set

DESCRIPTION
  Check whether a specified SS-Preference reason equals the current
  SS-Preference reason.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_IS683A_450M_SUPPORT
MMEXTN  boolean                       sdss_is_flag_set(
#else
static  boolean                       sdss_is_flag_set(
#endif

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HDR.
            */

        sdss_flag_e_type flag
            /* SS-Preference reason combination type.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( flag, SDSS_FLAG_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( flag )
  {
    case SDSS_FLAG_TRUE_EVENT_PROCESSED:
      return ss_ptr->is_true_event_processed;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDSS_FLAG_SYNC_UP_IN_PROGRESS:
    #ifdef FEATURE_HICPS_STACK_SYNC_UP
#error code not present
    #endif /* FEATURE_HICPS_STACK_SYNC_UP */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_FLAG_ENG_SCR_TIMER:
      return ss_ptr->is_eng_scr_timer_set;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_FLAG_CONN_MODE_UOOS_AWAKE:
      return ss_ptr->is_conn_uoos_awake;
      
#ifdef FEATURE_IS683A_450M_SUPPORT
    case SDSS_FLAG_IS683A_450M_SUPPORT: 
      return ss_ptr->is_IS683A_450M_support; 
#endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_FLAG_NONE:
    case SDSS_FLAG_MAX:
    default:

      return FALSE;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  }

} /* sdss_is_flag_set */

/*===========================================================================

FUNCTION sdss_is_pref_reason

DESCRIPTION
  Check whether a specified SS-Preference reason equals the current
  SS-Preference reason.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                       sdss_is_pref_reason0(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HDR.
            */

        uint32                        pref_reason
            /* SS-Preference reason combination type.
            */
)
{
  return( sdss_is_pref_reason( ss, (sdssscr_pref_reas_val_e_type)pref_reason ));

} /* sdss_is_pref_reason */

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_orig_mode0

DESCRIPTION
  Check whether a specified origination mode value includes the specified
  origination mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the specified
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.


===========================================================================*/
MMEXTN  boolean                    sdss_is_orig_mode0(

        sd_ss_orig_mode_e_type     orig_mode,
        /* Specified Origination mode enumeration type.
        */

        sd_ss_orig_mode_val_e_type orig_mode_val
        /* Origination mode value.
        */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( orig_mode, SD_SS_ORIG_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( orig_mode_val, SD_SS_ORIG_MODE_VAL_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If specified origination mode value includes the current origination
  ** mode, return TRUE. Else, return FALSE.
  */
  if( SD_IS_BIT_CONTAIN(orig_mode_val, BM(orig_mode)) )

  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

}/* sd_is_orig_mode */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_orig_mode

DESCRIPTION
  Check whether a specified origination mode value includes the current
  origination mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                       sdss_is_orig_mode(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HDR.
            */

        sd_ss_orig_mode_val_e_type    orig_mode_val
            /* Origination mode value.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( orig_mode_val, SD_SS_ORIG_MODE_VAL_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( sdss_is_orig_mode0( ss_ptr->orig_mode, orig_mode_val ) );

} /* sdss_is_orig_mode */

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_hybr_pref

DESCRIPTION
  Check whether a specified hybrid preference equals the current hybrid
  preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified hybrid preference equals the current hybrid
  preference. FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                       sdss_is_hybr_pref(

        sd_ss_hybr_pref_e_type        hybr_pref
            /* Hybrid preference.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr( SD_SS_MAIN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( hybr_pref, SD_SS_HYBR_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified hybrid preference equals the current hybrid preference,
  ** return TRUE. Else, return FALSE.
  */
  if( ss_ptr->hybr_pref == hybr_pref )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_is_hybr_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_band_pref

DESCRIPTION
  Check whether a specified band preference equals the current band
  preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified band preference equals the current band preference
  type FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                       sdss_is_band_pref(

        sd_ss_band_pref_e_type        band_pref
            /* Band preference.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified band preference equals the current band preference, return
  ** TRUE. Else, return FALSE.
  */
  if( band_pref == ss_ptr->band_pref )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_is_band_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_acq_specific_sid

DESCRIPTION
  Indicate whether last acquisition is for a specific SID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if last acquisition is for a specific SID. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                     sdss_is_acq_specific_sid(

        sd_ss_e_type                ss
            /* System selection - MAIN or HDR.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If acquisition is of type specific, return TRUE.
  */
  if( ss_ptr->acq_type == SDSSSCR_ACQ_SPECIFIC ||
      ss_ptr->acq_type == SDSSSCR_ACQ_SPECIFIC_PRL ||
      ss_ptr->acq_type == SDSSSCR_ACQ_SPECIFIC_PREF )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else return FALSE.
  */
  return FALSE;

} /* sdss_is_acq_specific_sid */

/*===========================================================================

@FUNCTION sdss_is_gprs_anite_gcf

@DESCRIPTION
  Indicates if a SS in underdoing GCF tests.

@DEPENDENCIES
  None.

@RETURNS
  TRUE if this SS is underdoing GCF tests.
  FALSE otherwise.

@SIDE_EFFECT
  None.

@x
===========================================================================*/
MMEXTN boolean                        sdss_is_gprs_anite_gcf(

       sd_ss_e_type                   ss
       /**< System Selection to check.
       */

)
{
  sdss_s_type   *ss_ptr       = sdss_ptr(ss);

  SD_ASSERT(ss_ptr != NULL);

  return BOOLEAN(ss_ptr->gprs_anite_gcf);
}

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_ss_for_mode

DESCRIPTION
 Gets the SS for the Mode

DEPENDENCIES
  None.

RETURN VALUE
  The ss for the mode, if valid
  SD_SS_MAX otherwise

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_e_type                  sdss_get_ss_for_mode(

        sd_mode_e_type                mode_pref
            /* Hybrid preference.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr( SD_SS_MAIN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SYS_SYS_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH

  /* Get the ss from the stack mode pref
  */
  if( SD_MODE_CONTAIN( ss_ptr->stack_mode_pref[SD_SS_MAIN],
                       sdss_map_mode_to_mode_pref(mode_pref)) )
  {
    return SD_SS_MAIN;
  }

  if( SD_MODE_CONTAIN( ss_ptr->stack_mode_pref[SD_SS_HYBR_1],
                       sdss_map_mode_to_mode_pref(mode_pref)) )
  {
    return SD_SS_HYBR_1;
  }

  if( SD_MODE_CONTAIN( ss_ptr->stack_mode_pref[SD_SS_HYBR_WLAN],
                       sdss_map_mode_to_mode_pref(mode_pref)) )
  {
    return SD_SS_HYBR_WLAN;
  }
  
  if (( mode_pref == SD_MODE_GSM   || 
        mode_pref == SD_MODE_WCDMA || 
        mode_pref == SD_MODE_GW )  && 
      SD_MODE_CONTAIN(ss_ptr->true_mode_pref,sdss_map_mode_to_mode_pref(mode_pref)))
  {
    /* If we come here and if mode_pref is G or W or GW and true mode pref 
    ** also contains mode_pref means UE is trying for limited service on 
    ** MAIN stack so returning SD_SS_MAIN.
    */
    return SD_SS_MAIN;
  }
  return SD_SS_MAX;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #else

  switch( mode_pref )
  {
    case SD_MODE_GPS:
    case SD_MODE_AMPS:
    case SD_MODE_CDMA:
    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:

      if( SD_MODE_CONTAIN( ss_ptr->mode_pref,
                           sdss_map_mode_to_mode_pref(mode_pref)) )
      {
        return SD_SS_MAIN;
      }

      return SD_SS_MAX;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_HDR:

      if( SD_MODE_CONTAIN( ss_ptr->mode_pref,
                           sdss_map_mode_to_mode_pref(mode_pref))
          &&
          sdss_is_hybr_pref( SD_SS_HYBR_PREF_NONE ) )
      {
        return SD_SS_MAIN;
      }
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_HDR_HYBRID
#error code not present
      #endif

      return SD_SS_MAX;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_WLAN:

      #ifdef FEATURE_WLAN
#error code not present
      #endif
      return SD_SS_MAX;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_MAX:
    default:
      SD_CASE_ERR( "mode_pref %d",mode_pref,0,0 );
      return SD_SS_MAX;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  }
  #endif

} /* sdss_get_ss_for_mode */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_pref

DESCRIPTION
   Fetches the current preferences from SD System selection core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                       sdss_get_pref(

        sd_ss_e_type               ss,
            /* System selection - MAIN or HDR.
            */

        sdss_pref_s_type           *pref_info_ptr
            /* Pointer to preference struct which will be populated with
            ** current preferences.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( pref_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Populate the preferences
  *//*lint -e774 */ /* Supporess if state is always true */
  if ( pref_info_ptr != NULL )/*lint +e774 */
  {
    pref_info_ptr->acq_order_pref = ss_ptr->acq_order_pref;
    pref_info_ptr->band_pref      = ss_ptr->band_pref;
    pref_info_ptr->prl_pref       = ss_ptr->prl_pref;
    pref_info_ptr->get_net_band_pref = ss_ptr->get_net_band_pref;
    pref_info_ptr->domain_pref    = ss_ptr->domain_pref;
    pref_info_ptr->hybr_pref      = ss_ptr->hybr_pref;
    pref_info_ptr->mode_pref      = ss_ptr->mode_pref;
    pref_info_ptr->true_mode_pref = ss_ptr->true_mode_pref;
    pref_info_ptr->get_net_mode_pref = ss_ptr->get_net_mode_pref;
    pref_info_ptr->tech_pref      = ss_ptr->tech_pref;
    pref_info_ptr->wlan_scan_pref = ss_ptr->wlan_scan_pref;
    pref_info_ptr->bss_type_pref  = ss_ptr->bss_type_pref;
    pref_info_ptr->roam_pref      = ss_ptr->roam_pref;
    pref_info_ptr->pref_reason    = ss_ptr->pref_reason;
    pref_info_ptr->orig_mode      = ss_ptr->orig_mode;
    pref_info_ptr->srv_req_type   = ss_ptr->srv_req_type;

  } /* if ( pref_info_ptr != NULL ) */

} /* sdss_get_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_pref

DESCRIPTION
   Fetches the previous origination mode preferences from SD System selection
   core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_orig_mode_e_type     sdss_get_prev_orig_mode(

        sd_ss_e_type               ss
            /* System selection - MAIN or HDR.
            */

)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ( ss_ptr->orig_mode_prev );


} /* sdss_get_prev_orig_mod */


/*===========================================================================

FUNCTION sdss_get_prev_sys_info

DESCRIPTION
   Fetches the previous association tag from SD System selection core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdss_sys_addn_info_s_type*     sdss_get_prev_sys_info(

        sd_ss_e_type                  ss
            /* System selection - MAIN or HDR.
            */

)
{
  sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT(ss != SD_SS_MAX);
  SD_ASSERT(ss_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ( &(ss_ptr->prev_sys_info) );


} /* sdss_get_prev_sys_info */

/*===========================================================================

FUNCTION sdss_addn_sys_info_init

DESCRIPTION
   Initialize the aditional sys info structure..

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                          sdss_addn_sys_info_init(

        sdss_sys_addn_info_s_type*    sys_info
            /* aditional sys info pointer.
            */

)
{

  SD_ASSERT(sys_info != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_info->assn_tag   = SDSS_ASSN_TAG_NONE;
  sys_info->geo_offset = SDSS_GEO_OFFSET_NONE;
  sys_info->geo_count  = SDSS_GEO_COUNT_NONE;

} /* sdss_addn_sys_info_init */

/*===========================================================================

FUNCTION sdss_set_prev_sys_info

DESCRIPTION
   Sets the previous association tag from SD System selection core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                       sdss_set_prev_sys_info(

        sd_ss_e_type               ss,
            /* System selection - MAIN or HDR.
            */

        const byte                 assn_tag,
           /* The Association Tag value.
           */
        const int                  geo_offset,
            /* Pointer to system record's geo.
            */
        const int                  multi_geo_count
            /* Pointer to the counter, for counting geo's
            ** in a multi-geo scenario.
            */

)
{
  sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT(ss != SD_SS_MAX);
  SD_ASSERT(ss_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the Association tag in the SS State.
  */
  ss_ptr->prev_sys_info.assn_tag = assn_tag;
  ss_ptr->prev_sys_info.geo_offset = geo_offset;
  ss_ptr->prev_sys_info.geo_count = multi_geo_count;

} /* sdss_set_prev_sys_info */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_acc_prm

DESCRIPTION
   Fetch the current access related parameters from the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                        sdss_get_acc_prm(

        sd_ss_e_type                ss,
            /* System selection - MAIN or HDR.
            */
        byte                       *rtrn_acc_reas_ptr,
            /* Pointer to a buffer where to deposit the access reason.
            */

        byte                       *rtrn_acc_term_ptr
            /* Pointer to a buffer where to deposit the access termination.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT( rtrn_acc_reas_ptr != NULL );
  SD_ASSERT( rtrn_acc_term_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy over the access reason and termination.
  */
  *rtrn_acc_reas_ptr = (byte)ss_ptr->acc_reas;
  *rtrn_acc_term_ptr = (byte)ss_ptr->acc_term;

} /* sdss_get_acc_prm */




/*===========================================================================
FUNCTION sdss_set_srv_req_type

DESCRIPTION
  Set the service request type in ss_ptr as indicated.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
/*lint --e{528} suppress sdss_set_srv_req_type not referenced
** warning 
*/
static void sdss_set_srv_req_type (sdss_s_type                  *ss_ptr,
                                   /* Pointer to the ss object*/
                                   sd_ss_srv_req_type_e_type  req_type)
/*lint -esym(715,ss_ptr) */
/*lint -esym(818,ss_ptr) */
/*lint -esym(715,req_type) */
{
  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( req_type, SD_SS_SRV_REQ_TYPE_MAX ); 

  ss_ptr->srv_req_type = req_type;
  return;
}
/*lint +esym(715,ss_ptr) */
/*lint +esym(818,ss_ptr) */
/*lint +esym(715,req_type) */
/*lint +esym(528,sdss_set_srv_req_type) */
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */


/*===========================================================================

FUNCTION sdss_update_rem_uoos_time

DESCRIPTION
   Updates the current service status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void             sdss_update_rem_uoos_time(

        sd_ss_e_type     ss,
            /* System selection - MAIN or HDR.
            */

        dword   uoos_awake_time,
            /* New service status.
            */

        dword   uoos_sleep_time
            /* New srv domain.
            */
)
{
  sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ss_ptr->rem_uoos_sleep_time = uoos_sleep_time;
  ss_ptr->rem_uoos_awake_time = uoos_awake_time;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( uoos_awake_time > 0 )
  {
    ss_ptr->is_conn_uoos_awake = TRUE;
  }
  else
  {
    ss_ptr->is_conn_uoos_awake = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

} /* sdss_update_srv_status */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*===========================================================================

FUNCTION sdss_is_user_1x_gw_and_curr_not_1x_gw

DESCRIPTION
   Updates the GW scan status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean               sdss_is_user_1x_gw_and_curr_not_1x_gw(

        const sdss_s_type          *ss_ptr
           /* Pointer to the SS object
           */
)
{
  #ifdef FEATURE_UNIFORM_SCAN_OOS
  sd_ss_mode_pref_e_type user_mode_pref = SD_SS_MODE_PREF_NONE;
  sd_ss_mode_pref_e_type curr_mode_pref = SD_SS_MODE_PREF_NONE;
  boolean is_user_mode_1x_gw = FALSE;
  boolean is_curr_mode_1x_gw = FALSE;


  SD_ASSERT( ss_ptr != NULL );
  /*user_mode_pref =  sdss_get_supp_mode_pref(ss_ptr->user_mode_pref, ss_ptr->user_band_pref); */
  user_mode_pref =  sdss_get_supp_mode_pref(ss_ptr->user_true_mode_pref, ss_ptr->user_band_pref);
  /* In IDLE mode, check true mode preference for current mode pref as limited can be acquired */
  if(ss_ptr->orig_mode == SD_SS_ORIG_MODE_NONE)
  {
    curr_mode_pref =  sdss_get_supp_mode_pref(ss_ptr->true_mode_pref, ss_ptr->band_pref);
  }
  else
  {
    curr_mode_pref =  sdss_get_supp_mode_pref(ss_ptr->mode_pref, ss_ptr->band_pref);
  }

  if ( ( SD_MODE_CONTAIN( user_mode_pref, BM(SD_MODE_GW)) ||
         SD_MODE_CONTAIN( user_mode_pref, BM(SD_MODE_GSM))||
         SD_MODE_CONTAIN( user_mode_pref, BM(SD_MODE_WCDMA)) ) &&
       ( SD_MODE_CONTAIN( user_mode_pref, BM(SD_MODE_CDMA)) ) )
  {
    is_user_mode_1x_gw = TRUE;
  }

  if ( ( SD_MODE_CONTAIN( curr_mode_pref, BM(SD_MODE_GW)) ||
         SD_MODE_CONTAIN( curr_mode_pref, BM(SD_MODE_GSM))||
         SD_MODE_CONTAIN( curr_mode_pref, BM(SD_MODE_WCDMA)) ) &&
       ( SD_MODE_CONTAIN( curr_mode_pref, BM(SD_MODE_CDMA)) ) )
  {
    is_curr_mode_1x_gw = TRUE;
  }

  return ( BOOLEAN( is_user_mode_1x_gw && !is_curr_mode_1x_gw ));
  #else
  SYS_ARG_NOT_USED(ss_ptr);
  return FALSE;
  #endif /* FEATURE_UNIFORM_SCAN_OOS */
}

/*===========================================================================

FUNCTION sdss_is_user_hdr_gw_and_curr_not_hdr_gw

DESCRIPTION
   Updates the GW scan status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean               sdss_is_user_hdr_gw_and_curr_not_hdr_gw(

        const sdss_s_type          *ss_ptr
           /* Pointer to the SS object
           */
)
{
  #ifdef FEATURE_UNIFORM_SCAN_OOS
  sd_ss_mode_pref_e_type user_mode_pref = SD_SS_MODE_PREF_NONE;
  sd_ss_mode_pref_e_type curr_mode_pref = SD_SS_MODE_PREF_NONE;
  boolean is_user_mode_hdr_gw = FALSE;
  boolean is_curr_mode_hdr_gw = FALSE;


  SD_ASSERT( ss_ptr != NULL );
  user_mode_pref =  sdss_get_supp_mode_pref(ss_ptr->user_true_mode_pref, ss_ptr->user_band_pref);
  /* In IDLE mode, check true mode preference for current mode pref as limited can be acquired */
  if(ss_ptr->orig_mode == SD_SS_ORIG_MODE_NONE)
  {
    curr_mode_pref =  sdss_get_supp_mode_pref(ss_ptr->true_mode_pref, ss_ptr->band_pref);
  }
  else
  {
    curr_mode_pref =  sdss_get_supp_mode_pref(ss_ptr->mode_pref, ss_ptr->band_pref);
  }

  if ( ( SD_MODE_CONTAIN( user_mode_pref, BM(SD_MODE_GW)) ||
         SD_MODE_CONTAIN( user_mode_pref, BM(SD_MODE_GSM))||
         SD_MODE_CONTAIN( user_mode_pref, BM(SD_MODE_WCDMA)) ) &&
       ( SD_MODE_CONTAIN( user_mode_pref, BM(SD_MODE_HDR)) ) )
  {
    is_user_mode_hdr_gw = TRUE;
  }

  if ( ( SD_MODE_CONTAIN( curr_mode_pref, BM(SD_MODE_GW)) ||
         SD_MODE_CONTAIN( curr_mode_pref, BM(SD_MODE_GSM))||
         SD_MODE_CONTAIN( curr_mode_pref, BM(SD_MODE_WCDMA)) ) &&
       ( SD_MODE_CONTAIN( curr_mode_pref, BM(SD_MODE_HDR)) ) )
  {
    is_curr_mode_hdr_gw = TRUE;
  }

  return ( BOOLEAN( is_user_mode_hdr_gw && !is_curr_mode_hdr_gw ));
  #else
  SYS_ARG_NOT_USED(ss_ptr);
  return FALSE;
  #endif /* FEATURE_UNIFORM_SCAN_OOS */
}

/*===========================================================================

FUNCTION sdss_update_srv_status

DESCRIPTION
   Updates the current service status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void             sdss_update_srv_status(

        sd_ss_e_type     ss,
            /* System selection - MAIN or HDR.
            */

        sys_srv_status_e_type srv_status,
            /* New service status.
            */

        sys_srv_domain_e_type  srv_domain
            /* New srv domain.
            */
)
{
  sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->srv_status = srv_status;
  ss_ptr->srv_domain = srv_domain;

} /* sdss_update_srv_status */

#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_srv_status

DESCRIPTION
   Get the current service status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  Return the current service status of the SS-Core.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sys_srv_status_e_type  sdss_get_srv_status(

        sd_ss_e_type     ss
            /* System selection - MAIN or HDR.
            */
)
{
  sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return(ss_ptr->srv_status);

} /* sdss_get_srv_status */



/*===========================================================================

FUNCTION sdss_get_simultaneous_mode

DESCRIPTION
   Get the simultaneous mode of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  Return the simultaneous mode of the SS-Core.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sys_simultaneous_mode_e_type  sdss_get_simultaneous_mode(

        sd_ss_e_type     ss
            /* System selection - MAIN or HDR.
            */
)
/*lint -esym(715, ss) */ /* not referenced */
{
  #ifndef FEATURE_HYBR_GW
  return SYS_SIMULTANEOUS_MODE_NON_WX;
  #else
#error code not present
  #endif
}
/*lint +esym(715, ss) */ /* not referenced */

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_redir

DESCRIPTION
  Check whether a specified redirection value includes the current
  redirection type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified redirection value includes the current redirection
  type FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                       sdss_is_redir(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HDR.
            */

        sdss_redir_val_e_type         redir_val
            /* Redirection value.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( redir_val, SDSS_REDIR_VAL_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified redirection value includes the current redirection type,
  ** return TRUE. Else, return FALSE.
  */
  if( SD_IS_BIT_CONTAIN(redir_val, BM(ss_ptr->redir)) )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_is_redir */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_handoff

DESCRIPTION
  Check whether a specified handoff value includes the current handoff type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified handoff value includes the current handoff type FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                       sdss_is_handoff(

        sdss_ho_val_e_type            handoff_val
            /* Handoff value.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( handoff_val, SDSS_HO_VAL_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified handoff value includes the current handoff type, return
  ** TRUE. Else, return FALSE.
  */
  if( SD_IS_BIT_CONTAIN(handoff_val,BM(ss_ptr->handoff)) )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_is_handoff */

/*===========================================================================

FUNCTION sdss_misc_is_handoff

DESCRIPTION
  Check whether a specified handoff value includes the current handoff type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified handoff value includes the current handoff type FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                       sdss_misc_is_handoff(

        sd_ss_ho_e_type            handoff_val
            /* Handoff value.
            */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( handoff_val, SD_SS_HO_MAX );
  return ( sdss_is_handoff( (sdss_ho_val_e_type)BM(handoff_val) ));
}

/*===========================================================================

FUNCTION sdss_is_resel_state1

DESCRIPTION
  Check whether a specified reselection value equals the current reselection
  state.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified reselection value equal the current reselection
  state. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdss_is_resel_state1(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdssscr_resel_stt_val_e_type     resel_state
            /* Reselection state value.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( resel_state, SDSSSCR_RESEL_STT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_IS_BIT_CONTAIN(resel_state,BM(ss_ptr->resel_state)) )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;
} /* sdss_is_resel_state1 */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_resel_state

DESCRIPTION
  Check whether a specified reselection value equals the current reselection
  state.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified reselection value equal the current reselection
  state. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdss_is_resel_state(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdss_resel_stt_e_type     resel_state
            /* Reselection state value.
            */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( resel_state, SDSS_RESEL_STT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sdss_is_resel_state1(ss, (sdssscr_resel_stt_val_e_type)BM(resel_state));

} /* sdss_is_resel_state */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_acc_reas

DESCRIPTION
  Check whether a specified access reason value equals the most recent
  access reason.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified access reason value equals the most recent access
  termination reason. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_is_acc_reas(

        const sdss_s_type             *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        sd_ss_acc_reas_e_type         acc_reas
            /* Access reason.
            */
)
{
  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( acc_reas, SD_SS_ACC_REAS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified access reason equals the most recent access reason, return
  ** TRUE. Else, return FALSE.
  */
  if( acc_reas == ss_ptr->acc_reas )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_is_acc_reas */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_acc_term

DESCRIPTION
  Check whether a specified access termination reason equals the most recent
  access termination reason.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified access termination reason equals the most recent
  access termination reason. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_is_acc_term(

        const sdss_s_type             *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        sd_ss_acc_term_e_type         acc_term_val
            /* Access termination value.
            */
)
{
  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( acc_term_val, SD_SS_ACC_TERM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified access termination value includes the current access
  ** termination reason, return TRUE. Else, return FALSE.
  */
  if( acc_term_val == ss_ptr->acc_term )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_is_acc_term */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_acc_fail

DESCRIPTION
  Check whether registration access has failed at least cnt times and time
  minutes.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if registration access has failed at least cnt times and time minutes.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_is_acc_fail(

        const sdss_s_type             *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        int                           cnt,
            /* Number of times registration access has failed.
            */

        word                          time
            /* Time (in minutes) over which registration access has failed.
            */
)
{
  SD_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ss_ptr->acc_fail_cnt >= cnt &&
      clk_uptime() >= ss_ptr->acc_fail_start_time &&
      (word)(clk_uptime() - ss_ptr->acc_fail_start_time) >= time * 60 )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_is_acc_fail */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_event_cnt

DESCRIPTION
  Check whether the event has occurred CNT times within TIME minutes.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE   if event has occurred CNT times within TIME minutes.
  FALSE  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_is_event_cnt (

        const sdss_s_type             *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        sdss_cnted_evt_e_type         event,
            /* Event index in the counter/time array.
            */

        int                           cnt,
            /* Number of events.
            */

        word                          time
            /* Time (in minutes) over which event has occurred.
            */
)
{
  SD_ASSERT( ss_ptr != NULL );
  if(event <= SDSS_CNTED_EVT_NONE)
  {
    return FALSE;
  }   

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_CNTED_EVT_MAX );

  if( ss_ptr->event_cnt[event] >= cnt &&
      clk_uptime() >= ss_ptr->event_uptime[event] &&
      (word)(clk_uptime() - ss_ptr->event_uptime[event]) >= time*60 )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_is_event_cnt */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_prl_pref_chgd

DESCRIPTION
  Check whether the PRL preference was changed the last time.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if changed.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                       sdss_is_prl_pref_chgd(

        sd_ss_e_type                  ss
          /* System selection - MAIN or HDR.
          */
)
{

  sdss_s_type   *ss_ptr = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if it changed.
  */
  if( ss_ptr->prl_pref != ss_ptr->prev_prl_pref )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_is_prl_pref_chgd */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_time_gr

DESCRIPTION
  Check whether the instance has acquired service more than time seconds ago

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if passed.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

static boolean sdss_is_time_gr(
  sd_ss_e_type ss,
  /* The ss for which time is to be checked
  */

  sdssscr_srv_status_val_e_type srv_status,
  /* The srv_status to check for
  */

  byte time
  /* The time how long srv_status must have been achieved
  */
)
{
  const sd_si_info_s_type    *si_info_ptr = sd_si_info_ptr_get();
  boolean cond_is_true = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( si_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ss != SD_SS_MAIN )
  {
    SD_MSG_ERROR( " Invalid ss %d in sdss_is_time_gr",ss,0,0);
    SD_ASSERT(FALSE);
    /* Default return to FALSE
    */
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( srv_status )
  {
    case SDSSSCR_SRV_STATUS_VAL_SRV:
    case SDSSSCR_SRV_STATUS_VAL_SRV_ANY:

      /* Verify if we have acquired service
      */
      if( SD_IS_BIT_CONTAIN( BM(si_info_ptr->srv_status), srv_status ))
      {
        cond_is_true = TRUE;
      }

      /* If we are required to check the time, check it now
      */
      if( cond_is_true && time != 0 && si_info_ptr->srv_acq_time != 0 )
      {
        cond_is_true = ( (clk_uptime() > si_info_ptr->srv_acq_time + time) ?
                       TRUE : FALSE );
      }

      return cond_is_true;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDSSSCR_SRV_STATUS_VAL_NO_SRV:

      /* Verify if we have acquired service
      */
      if( SD_IS_BIT_CONTAIN( BM(si_info_ptr->srv_status), srv_status ))
      {
        cond_is_true = TRUE;
      }

      /* If we are required to check the time, check it now
      */
      if( cond_is_true && time != 0 && si_info_ptr->srv_lost_time != 0 )
      {
        cond_is_true = ( (clk_uptime() > si_info_ptr->srv_lost_time + time) ?
                       TRUE : FALSE );
      }

      return cond_is_true;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_SRV_STATUS_VAL_PWR_SAVE:
    case SDSSSCR_SRV_STATUS_VAL_NO_SRV_ANY:
    case SDSSSCR_SRV_STATUS_VAL_LIMITED:
    case SDSSSCR_SRV_STATUS_VAL_LIMITED_REG:
    case SDSSSCR_SRV_STATUS_VAL_LIMITED_ANY:
    case SDSSSCR_SRV_STATUS_VAL_MAX:
    default:
      return FALSE;
  } /* switch */

} /* sdss_is_time_gr */


/*===========================================================================

FUNCTION sdss_is_srv_status2

DESCRIPTION
  Check whether the srv status for ss instance is srv_status
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if passed.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

static  boolean                       sdss_is_srv_status2(

        sd_ss_e_type                  ss,
            /* The ss for which time is to be checked
            */

        sdssscr_srv_status_val_e_type srv_status
            /* The srv_status to check for
            */
)
{


  const sd_si_info_s_type    *si_info_ptr = sd_si_info_ptr_get();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( si_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( ss )
  {
    case SD_SS_MAIN:

      si_info_ptr = sd_si_info_ptr_get();

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      SD_ASSERT( si_info_ptr != NULL );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      return( SD_IS_BIT_CONTAIN( BM(si_info_ptr->srv_status), srv_status ) );

    case SD_SS_HYBR_HDR:
      #ifdef FEATURE_HDR
#error code not present
      #endif
      /* Delibrate fall down to the default is FEATURE_HDR is not defined
      */

    case SD_SS_HYBR_WLAN:

      #ifdef FEATURE_WLAN
#error code not present
      #endif

      /* Delibrate fall down to the default is FEATURE_WLAN is not defined
      */
    case SD_SS_MAX:
    default:
      SD_CASE_ERR(" ss %d", ss, 0, 0 );
      return FALSE;
  }
}


/*===========================================================================

FUNCTION sdss_is_sys_lost

DESCRIPTION
  Check whether a specified system lost reason equals the most recent system
  lost reason.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified system lost reason equals the most recent system lost
  reason. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_is_sys_lost(

        const sdss_s_type             *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        sd_ss_sys_lost_e_type         sys_lost
            /* System lost reason.
            */
)
{
  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( sys_lost, SD_SS_SYS_LOST_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified system lost reason equals the current system lost reason,
  ** return TRUE. Else, return FALSE.
  */
  if( sys_lost == ss_ptr->sys_lost )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_is_sys_lost */

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_opr_mode_update

DESCRIPTION
  Update the operating mode status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                          sdss_opr_mode_update(

        sdss_opr_mode_e_type          opr_mode
            /* New operating mode.
            */
)
{
  sdss_s_type   *ss_ptr   = sdss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( opr_mode, SDSS_OPR_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->opr_mode = opr_mode;

} /* sdss_opr_mode_update */


#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_network_pref_update

DESCRIPTION
  Update the mode and band preference for get network request of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                          sdss_get_network_pref_update(

        sd_ss_e_type                  ss,
            /* The ss for which to update
            */

        sd_ss_mode_pref_e_type        mode_pref,
            /* system mode preference for get network request.
            */
        sd_ss_band_pref_e_type        band_pref
            /* system band preference for get network request.
            */
)
{
  sdss_s_type   *ss_ptr   = sdss_ptr(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->get_net_mode_pref = mode_pref;
  ss_ptr->get_net_band_pref = band_pref;

} /* sdss_get_network_pref_update */
#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_subsc_avail_update

DESCRIPTION
  Update the Subscription availability status in the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                          sdss_subsc_avail_update(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HDR.
            */

        boolean                       is_gw_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                       is_1x_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */

        boolean                       is_wlan_subsc_avail
           /* Indicates if subscription is available for providing service on
           ** WLAN network.
           */
)
{
  sdss_s_type   *ss_ptr = sdss_ptr( ss );


  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the subscription availability information on SS-Main.
  */
  ss_ptr->is_gw_subsc_avail = is_gw_subsc_avail;
  ss_ptr->is_1x_subsc_avail = is_1x_subsc_avail;
  ss_ptr->is_wlan_subsc_avail = is_wlan_subsc_avail;

} /* sdss_subsc_avail_update */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_subsc_type

DESCRIPTION
  Get the subscription type from the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  Enumeration type of subscription

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdss_subsc_e_type             sdss_get_subsc_type(

        sd_ss_e_type                  ss
            /* System selection - MAIN or HDR.
            */
)
{
  sdss_s_type                         *ss_ptr = sdss_ptr( ss );
                                           /**/

  sdss_subsc_e_type                   subsc = SDSS_SUBSC_NONE;
                                          /* the subscription type */

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ss_ptr->is_1x_subsc_avail )
  {
    subsc = SDSS_SUBSC_ADD( subsc, SDSS_SUBSC_1X );
  }

  if( ss_ptr->is_gw_subsc_avail )
  {
    subsc = SDSS_SUBSC_ADD( subsc, SDSS_SUBSC_GW );
  }

  return subsc;

}




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_pref_update

DESCRIPTION
  Update the SS-Preference setting of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if any of the new preferences is different than the existing
  preferences. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdss_pref_updated_e_type      sdss_pref_update(

        sd_ss_pref_reas_e_type        pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type        orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type        mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency.
            */

        sd_ss_band_pref_e_type        band_pref,
            /* New band preference (BC0, BC1, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency..
            */

        sd_ss_prl_pref_e_type         prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type        roam_pref,
            /* New roam preference (any, home, affiliated, etc.) - ignored
            ** when orig_mode = OTASP or emergency..
            */

        sd_ss_hybr_pref_e_type        hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type              otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */


        sd_ss_srv_domain_pref_e_type   domain_pref,
            /* Service domain preference that is associated with the selected
            ** NAM.
            ** Applies only for GSM/WCDMA modes.
            */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
            /* Preference for the order of acquisition ( WCDMA before GSM,
            ** GSM before WCDMA etc).
            */

        sd_ss_tech_pref_e_type         tech_pref,
           /* Technology preference setting.
           */

        sys_wlan_scan_pref_e_type      wlan_scan_pref,
           /* WLAN scan preference, not compared.
           */

        sys_wlan_bss_e_type            bss_type_pref
           /* BSS type.
           */

)
{
  sdss_s_type   *ss_ptr           = sdss_ptr(SD_SS_MAIN);
  boolean       otasp_is_changed  = FALSE;
  sd_ss_mode_pref_e_type   new_supp_mode_pref;
  sd_ss_mode_pref_e_type   curr_supp_mode_pref;
  sd_ss_band_pref_e_type   new_supp_band_pref;
  sd_ss_band_pref_e_type   curr_supp_band_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( pref_reason, SD_SS_PREF_REAS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( orig_mode, SD_SS_ORIG_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl_pref,  SD_SS_PRL_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( roam_pref, SD_SS_ROAM_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( hybr_pref, SD_SS_HYBR_PREF_MAX );
  SD_ASSERT( INRANGE(domain_pref, SD_SS_SRV_DOMAIN_PREF_NONE,
                     (SD_SS_SRV_DOMAIN_PREF_MAX-1)) );
  SD_ASSERT_ENUM_IS_INRANGE( acq_order_pref, SD_SS_ACQ_ORDER_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( tech_pref, SYS_TECH_PREF_MASK_ANY );
  SD_ASSERT_ENUM_IS_INRANGE( wlan_scan_pref, SYS_WLAN_SCAN_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( bss_type_pref, SYS_WLAN_BSS_TYPE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( otasp_band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( otasp_blksys, SD_BLKSYS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the service domain is NONE, remove GW components from the mode_pref.
  ** Service domain doesn't affect non-GW systems.
  */
  if(domain_pref == SD_SS_SRV_DOMAIN_PREF_NONE)
  {
    mode_pref = SD_MODE_DELETE(mode_pref, SD_SS_MODE_PREF_GW);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initial ss_pref_updated_status to NONE.
  */
  ss_ptr->ss_pref_updated_status =  SDSS_PREF_UPDATED_NONE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return TRUE if the command was to avoid the system.
  */
  if (pref_reason == SD_SS_PREF_REAS_AVOID_SYS )
  {
    ss_ptr->pref_reason            = pref_reason;
    ss_ptr->ss_pref_updated_status = SDSS_PREF_UPDATED_PREF_REAS;

    return ss_ptr->ss_pref_updated_status;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If origination mode is OTASP and the OTASP band/blksys has changed or
  ** origination mode is ORIG_START indicate that OTASP parameters have
  ** changed.
  */
  if( orig_mode == SD_SS_ORIG_MODE_OTASP )
  {
    if( ss_ptr->otasp_band   != otasp_band ||
        ss_ptr->otasp_blksys != otasp_blksys ||
        pref_reason == SD_SS_PREF_REAS_ORIG_START_CS )
    {
      otasp_is_changed = TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the true mode pref unadjusted based on subscription.
  */
  ss_ptr->true_mode_pref = mode_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Adjust the mode_pref based on the subscription availability, only if
  ** it is not emergency origination.
  */
  if ( orig_mode != SD_SS_ORIG_MODE_EMERG_ORIG &&
       orig_mode != SD_SS_ORIG_MODE_EMERG_CB )
  {

    sd_ss_mode_pref_e_type supp_mode_pref =
                              sdss_get_supp_mode_pref( mode_pref, band_pref );

    /* Check if 1x subscription is available?
    */
    if ( !ss_ptr->is_1x_subsc_avail )
    {
       /* 1x subscription is not available, reset CDMA, AMPS & HDR modes.
       */
       mode_pref = SD_MODE_DELETE( mode_pref, SD_SS_MODE_PREF_CDMA_AMPS );
       mode_pref = SD_MODE_DELETE( mode_pref, SD_SS_MODE_PREF_HDR);
       ss_ptr->true_mode_pref = SD_MODE_DELETE( ss_ptr->true_mode_pref, SD_SS_MODE_PREF_CDMA_AMPS );
       ss_ptr->true_mode_pref = SD_MODE_DELETE( ss_ptr->true_mode_pref, SD_SS_MODE_PREF_HDR);

       supp_mode_pref = mode_pref;

    } /*  if ( !ss_ptr->is_1x_subsc_avail ) */

    /* Check if WLAN subscription is available?
    */
    if ( !ss_ptr->is_wlan_subsc_avail )
    {
       /* 1x subscription is not available, reset WLAN.
       */
       mode_pref = SD_MODE_DELETE( mode_pref, SD_SS_MODE_PREF_WLAN);
       ss_ptr->true_mode_pref = SD_MODE_DELETE( ss_ptr->true_mode_pref, SD_SS_MODE_PREF_WLAN);

       supp_mode_pref = mode_pref;

    } /*  if ( !ss_ptr->is_1x_subsc_avail ) */


    /* Deleting WLAN preferences so that if gw subscription is not available
    ** orig_mode gets assigned to SD_SS_ORIG_MODE_EMERG_ORIG
    */
    supp_mode_pref = SD_MODE_DELETE( mode_pref, SD_SS_MODE_PREF_WLAN);

    /* Check if GW subscription is available.
    */

    if ( !ss_ptr->is_gw_subsc_avail )
    {

      /* Check if it is GW only mode_pref.
      */
      if ( supp_mode_pref == SD_SS_MODE_PREF_GSM ||
           supp_mode_pref == SD_SS_MODE_PREF_WCDMA ||
           supp_mode_pref == SD_SS_MODE_PREF_GW
         )
      {
        /* GW subscription is not available and GW only mode_pref.
        */
        orig_mode = SD_SS_ORIG_MODE_EMERG_ORIG;
      }
      else
      {
       /* gw subscription is not available, reset GSM & WCDMA modes.
       */
       mode_pref = SD_MODE_DELETE( mode_pref, SD_SS_MODE_PREF_GW );

      }

    } /* if ( !ss_ptr->is_gw_subsc_avail ) */


  } /* if ( orig_mode != SD_SS_ORIG_MODE_EMERG_ORIG ... ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

  mode_pref = SD_MODE_DELETE( mode_pref, SD_SS_MODE_PREF_WLAN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
  /* If doing WCDMA/CDMA hybrid operation, elimnate GW and HDR
  ** mode preference from
  ** the main system selection.
  */

  switch( hybr_pref )
  {
    case SD_SS_HYBR_PREF_NONE:
      ss_ptr->stack_mode_pref[SD_SS_MAIN]   = mode_pref;
      ss_ptr->stack_mode_pref[SD_SS_HYBR_1] = SD_SS_MODE_PREF_NONE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_HYBR_PREF_CDMA__HDR:

      mode_pref = SD_MODE_DELETE( mode_pref, SD_SS_MODE_PREF_HDR);
      ss_ptr->true_mode_pref = SD_MODE_DELETE( ss_ptr->true_mode_pref, SD_SS_MODE_PREF_HDR);
      ss_ptr->stack_mode_pref[SD_SS_MAIN]   = mode_pref;
      ss_ptr->stack_mode_pref[SD_SS_HYBR_1] = SD_SS_MODE_PREF_HDR;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_HYBR_PREF_CDMA__WCDMA:

      mode_pref = SD_MODE_DELETE( mode_pref, SD_SS_MODE_PREF_GW );
      ss_ptr->true_mode_pref = SD_MODE_DELETE( ss_ptr->true_mode_pref, SD_SS_MODE_PREF_GW);
      ss_ptr->stack_mode_pref[SD_SS_MAIN]   = mode_pref;
      ss_ptr->stack_mode_pref[SD_SS_HYBR_1] = SD_SS_MODE_PREF_GW;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_HYBR_PREF_CDMA__HDR_WCDMA:

      mode_pref = SD_MODE_DELETE( mode_pref, SD_SS_MODE_PREF_GW );
      mode_pref = SD_MODE_DELETE( mode_pref, SD_SS_MODE_PREF_HDR );
      ss_ptr->true_mode_pref = SD_MODE_DELETE( ss_ptr->true_mode_pref, SD_SS_MODE_PREF_HDR);
      ss_ptr->true_mode_pref = SD_MODE_DELETE( ss_ptr->true_mode_pref, SD_SS_MODE_PREF_GW);
      ss_ptr->stack_mode_pref[SD_SS_MAIN]   = mode_pref;
      ss_ptr->stack_mode_pref[SD_SS_HYBR_1] = SD_MODE_ADD (SD_SS_MODE_PREF_GW,
                                                           SD_SS_MODE_PREF_HDR);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_HYBR_PREF_MAX:
    default:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  }
  #endif

  /* Record the current origination mode and update the reson for this
  ** SS-Preference update.
  */
  ss_ptr->orig_mode_prev = ss_ptr->orig_mode;
  if ( ss_ptr->pref_reason != pref_reason )
  {
    ss_ptr->pref_reason    = pref_reason;
    ss_ptr->ss_pref_updated_status = SDSS_PREF_UPDATED_PREF_REAS;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if ( defined (FEATURE_UNIFORM_SCAN_OOS) || \
       defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING) )
  if( ss_ptr->ss_pref_updated_status == SDSS_PREF_UPDATED_PREF_REAS &&
      pref_reason                    == SD_SS_PREF_REAS_ORIG_END &&
      ss_ptr->orig_mode_prev         == SD_SS_ORIG_MODE_EMERG_ORIG )
  {
     ss_ptr->ss_pref_updated_status = SDSS_PREF_UPDATED_EMERG_ORIG_END;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get current/new supported mode/band pref.
  */
  curr_supp_mode_pref  = ss_ptr->mode_pref;
  curr_supp_band_pref  = ss_ptr->band_pref;
  sd_misc_get_band_mode( &curr_supp_mode_pref, &curr_supp_band_pref,
                         SD_SS_MODE_PREF_ANY, SD_SS_BAND_PREF_ANY
                       );

  new_supp_mode_pref  = mode_pref;
  new_supp_band_pref  = band_pref;
  sd_misc_get_band_mode( &new_supp_mode_pref, &new_supp_band_pref,
                         SD_SS_MODE_PREF_ANY, SD_SS_BAND_PREF_ANY
                       );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the previous preferences fields.
  */
  ss_ptr->prev_mode_pref       = ss_ptr->mode_pref;
  ss_ptr->prev_band_pref       = ss_ptr->band_pref;
  ss_ptr->prev_prl_pref        = ss_ptr->prl_pref;
  ss_ptr->prev_roam_pref       = ss_ptr->roam_pref;
  ss_ptr->prev_hybr_pref       = ss_ptr->hybr_pref;
  ss_ptr->prev_domain_pref     = ss_ptr->domain_pref;
  ss_ptr->prev_acq_order_pref  = ss_ptr->acq_order_pref;
  ss_ptr->prev_tech_pref       = ss_ptr->tech_pref;
  ss_ptr->prev_wlan_scan_pref  = ss_ptr->wlan_scan_pref;
  ss_ptr->prev_bss_type_pref   = ss_ptr->bss_type_pref;

  /* update PRL Support HDR Indication */
  ss_ptr->is_prl_support_hdr =
     sdprl_is_supp_mode_band_pref(SD_SS_MODE_PREF_HDR, band_pref);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the user_mode_preference
  */
  if(ss_ptr->pref_reason == SD_SS_PREF_REAS_USER ||
     ss_ptr->pref_reason == SD_SS_PREF_REAS_MULTIMODE_SCAN ||
     ss_ptr->pref_reason == SD_SS_PREF_REAS_USER_RESEL )
  {
     /* If the mode has changed from AUTO <-> MANUAL,
     ** change the service_req_type as USER_SELECTION.
     ** Else there is some system parameter like band or mode pref 
     ** change. Assign srv_req_type to SYSTEM_CHANGE.
     */

    if( ss_ptr->pref_reason == SD_SS_PREF_REAS_USER_RESEL )
    {
      ss_ptr->srv_req_type = SD_SS_SRV_REQ_TYPE_USER_SELECTION;
    }
    else
    {
      ss_ptr->srv_req_type = SD_SS_SRV_REQ_TYPE_SYSTEM_CHANGE;
    }
  }

  /* If none of the new preferences is different than the existing
  ** preferences, return FALSE.
  */
  if( ss_ptr->orig_mode == orig_mode &&
      curr_supp_mode_pref == new_supp_mode_pref &&
      curr_supp_band_pref == new_supp_band_pref &&
      ss_ptr->prl_pref == prl_pref &&
      ss_ptr->roam_pref == roam_pref &&
      ss_ptr->hybr_pref == hybr_pref &&
      ss_ptr->acq_order_pref == acq_order_pref &&
      ss_ptr->domain_pref == domain_pref &&
      ss_ptr->tech_pref   == tech_pref &&
      ss_ptr->wlan_scan_pref == wlan_scan_pref &&
      ss_ptr->bss_type_pref == bss_type_pref &&
      ! otasp_is_changed )
  {
    return ss_ptr->ss_pref_updated_status;
  }

  
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the current preferences.
  */
  ss_ptr->orig_mode     = orig_mode;
  ss_ptr->mode_pref     = mode_pref;
  ss_ptr->band_pref     = band_pref;
  ss_ptr->prl_pref      = prl_pref;
  ss_ptr->roam_pref     = roam_pref;
  ss_ptr->hybr_pref     = hybr_pref;

  ss_ptr->otasp_band    = otasp_band;
  ss_ptr->otasp_blksys  = otasp_blksys;

  ss_ptr->acq_order_pref= acq_order_pref;
  ss_ptr->domain_pref   = domain_pref;
  ss_ptr->tech_pref     = tech_pref;
  ss_ptr->wlan_scan_pref= wlan_scan_pref;
  ss_ptr->bss_type_pref = bss_type_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the user_mode_preference
  */
  if(ss_ptr->pref_reason == SD_SS_PREF_REAS_USER ||
     ss_ptr->pref_reason == SD_SS_PREF_REAS_MULTIMODE_SCAN ||
     ss_ptr->pref_reason == SD_SS_PREF_REAS_USER_RESEL )
  {
    ss_ptr->user_mode_pref = ss_ptr->mode_pref;
    ss_ptr->user_true_mode_pref = ss_ptr->true_mode_pref;
    ss_ptr->user_band_pref = ss_ptr->band_pref;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the handoff state if the reason is for a handoff
  */
  if( pref_reason == SD_SS_PREF_REAS_HDR_CDMA_HO )
  {
    /* Update the SS-Core with the handoff type.
    */
    sdss_ho_update( SD_SS_HO_HDR_TO_CDMA );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Overwrite the ss_pref_updated_status.
  **
  ** Regardless if orig mode or pref reason is changed or not,
  ** if only service domain is changed and all other preferences remains same,
  ** overwrite with SDSS_PREF_UPDATED_SRV_DOMAIN.
  **
  ** Otherwise overwrite with SDSS_PREF_UPDATED_TRUE_PREF.
  */
  if( curr_supp_mode_pref         == new_supp_mode_pref     &&
      curr_supp_band_pref         == new_supp_band_pref     &&
      ss_ptr->prev_prl_pref       == ss_ptr->prl_pref       &&
      ss_ptr->prev_roam_pref      == ss_ptr->roam_pref      &&
      ss_ptr->prev_hybr_pref      == ss_ptr->hybr_pref      &&
      ss_ptr->prev_acq_order_pref == ss_ptr->acq_order_pref &&
      ss_ptr->prev_domain_pref    != ss_ptr->domain_pref    &&
      ! otasp_is_changed                                    &&
      ss_ptr->prev_tech_pref      == ss_ptr->tech_pref      &&
      ss_ptr->prev_wlan_scan_pref == ss_ptr->prev_wlan_scan_pref &&
      ss_ptr->bss_type_pref            == ss_ptr->bss_type_pref )
  {
    ss_ptr->ss_pref_updated_status = SDSS_PREF_UPDATED_SRV_DOMAIN;
  }
  else
  {

    #if ( defined (FEATURE_UNIFORM_SCAN_OOS) || \
          defined (FEATURE_UMTS_UNIFORM_OOS_HANDLING) )
    if( ss_ptr->ss_pref_updated_status != SDSS_PREF_UPDATED_EMERG_ORIG_END )
    {
      ss_ptr->ss_pref_updated_status = SDSS_PREF_UPDATED_TRUE_PREF;
    }
    #else
    ss_ptr->ss_pref_updated_status = SDSS_PREF_UPDATED_TRUE_PREF;
    #endif

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return pref updated status.
  */
  return(  ss_ptr->ss_pref_updated_status );

} /* sdss_pref_update */


#ifdef FEATURE_HDR_HYBRID
#error code not present
#endif /* FEATURE_HDR_HYBRID */


#ifdef FEATURE_TECH_IND_SD_STACK_ARCH
/* <EJECT> */
/*===========================================================================

FUNCTION sdss_hybr_1_pref_update

DESCRIPTION
  Update the Hybr_1 SS-Preference setting of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if any of the new preferences is different than the existing
  preferences. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdss_pref_updated_e_type      sdss_hybr_1_pref_update(

        sd_ss_pref_reas_e_type        pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type        orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type        mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency.
            */

        sd_ss_band_pref_e_type        band_pref,
            /* New band preference (BC0, BC1, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency..
            */

        sd_ss_prl_pref_e_type         prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type        roam_pref,
            /* New roam preference (any, home, affiliated, etc.) - ignored
            ** when orig_mode = OTASP or emergency..
            */

        sd_ss_hybr_pref_e_type        hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_ss_srv_domain_pref_e_type   domain_pref,
            /* Service domain preference that is associated with the selected
            ** NAM.
            ** Applies only for GSM/WCDMA modes.
            */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
            /* Preference for the order of acquisition ( WCDMA before GSM,
            ** GSM before WCDMA etc).
            */

        sd_ss_tech_pref_e_type         tech_pref,
           /* Technology preference setting.
           */

        sys_wlan_scan_pref_e_type      wlan_scan_pref,
           /* WLAN scan preference, not compared.
           */

        sys_wlan_bss_e_type            bss_type_pref
           /* BSS type.
           */
)
{
  sdss_s_type   *ss_ptr    = sdss_ptr(SD_SS_HYBR_1);

  sdss_s_type   *ss_main_ptr = sdss_ptr(SD_SS_MAIN);

  sd_ss_mode_pref_e_type   new_supp_mode_pref;
  sd_ss_mode_pref_e_type   curr_supp_mode_pref;
  sd_ss_band_pref_e_type   new_supp_band_pref;
  sd_ss_band_pref_e_type   curr_supp_band_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( pref_reason, SD_SS_PREF_REAS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( orig_mode, SD_SS_ORIG_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl_pref,  SD_SS_PRL_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( roam_pref, SD_SS_ROAM_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( hybr_pref, SD_SS_HYBR_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( domain_pref, SD_SS_SRV_DOMAIN_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acq_order_pref, SD_SS_ACQ_ORDER_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( tech_pref, SYS_TECH_PREF_MASK_ANY );
  SD_ASSERT_ENUM_IS_INRANGE( wlan_scan_pref, SYS_WLAN_SCAN_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( bss_type_pref, SYS_WLAN_BSS_TYPE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the service domain is NONE, remove GW components from the mode_pref.
  ** Service domain doesn't affect non-GW systems.
  */
  if(domain_pref == SD_SS_SRV_DOMAIN_PREF_NONE)
  {
    mode_pref = SD_MODE_DELETE(mode_pref, SD_SS_MODE_PREF_GW);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize to SDSS_PREF_UPDATED_NONE
  */
  ss_ptr->ss_pref_updated_status = SDSS_PREF_UPDATED_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return TRUE if the command was to avoid the system.
  */
  if (pref_reason == SD_SS_PREF_REAS_AVOID_SYS )
  {
    ss_ptr->pref_reason            = pref_reason;
    ss_ptr->ss_pref_updated_status = SDSS_PREF_UPDATED_PREF_REAS;

    return ss_ptr->ss_pref_updated_status;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Isolate the Hybr_1 mode pref.
  */
  mode_pref = SD_GET_COMMON_MODE( mode_pref, ss_main_ptr->stack_mode_pref[SD_SS_HYBR_1]);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the true mode pref unadjusted based on subscription.
  */
  ss_ptr->true_mode_pref = mode_pref;

  /* If not doing hybrid operation, set mode preference to NONE.
  */
  if( hybr_pref == SD_SS_HYBR_PREF_NONE )
  {
    mode_pref = SD_SS_MODE_PREF_NONE;
    ss_ptr->true_mode_pref = SD_SS_MODE_PREF_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Record the current origination mode and update the reson for this
  ** SS-Preference update.
  */
  ss_ptr->orig_mode_prev = ss_ptr->orig_mode;
  if ( ss_ptr->pref_reason != pref_reason )
  {
    ss_ptr->pref_reason    = pref_reason;
    ss_ptr->ss_pref_updated_status = SDSS_PREF_UPDATED_PREF_REAS;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Adjust the mode_pref based on the subscription availability, only if
  ** it is not emergency origination.
  */
  if ( orig_mode != SD_SS_ORIG_MODE_EMERG_ORIG &&
       orig_mode != SD_SS_ORIG_MODE_EMERG_CB )
  {

    sd_ss_mode_pref_e_type supp_mode_pref =
                              sdss_get_supp_mode_pref( mode_pref, band_pref );


    /* Check if 1x subscription is available?
    */
    if ( !ss_ptr->is_1x_subsc_avail )
    {
       /* 1x subscription is not available, reset CDMA, AMPS & HDR modes.
       */
       mode_pref = SD_MODE_DELETE( mode_pref, SD_SS_MODE_PREF_CDMA_AMPS );
       mode_pref = SD_MODE_DELETE( mode_pref, SD_SS_MODE_PREF_HDR);
       ss_ptr->true_mode_pref = SD_MODE_DELETE( ss_ptr->true_mode_pref, SD_SS_MODE_PREF_CDMA_AMPS );
       ss_ptr->true_mode_pref = SD_MODE_DELETE( ss_ptr->true_mode_pref, SD_SS_MODE_PREF_HDR);
       
       supp_mode_pref = mode_pref;

    } /*  if ( !ss_ptr->is_1x_subsc_avail ) */

    /* Check if GW subscription is available.
    */
    if ( !ss_ptr->is_gw_subsc_avail )
    {
      /* Check if it is GW only mode_pref.
      */
      if ( supp_mode_pref == SD_SS_MODE_PREF_GSM ||
           supp_mode_pref == SD_SS_MODE_PREF_WCDMA ||
           supp_mode_pref == SD_SS_MODE_PREF_GW
         )
      {
        /* GW subscription is not available and GW only mode_pref.
        */
        orig_mode = SD_SS_ORIG_MODE_EMERG_ORIG;
      }
      else
      {
       /* gw subscription is not available, reset GSM & WCDMA modes.
       */
       mode_pref = SD_MODE_DELETE( mode_pref, SD_SS_MODE_PREF_GW );

      }

    } /* if ( !ss_ptr->is_gw_subsc_avail ) */


  } /* if ( orig_mode != SD_SS_ORIG_MODE_EMERG_ORIG ... ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if ( defined (FEATURE_UNIFORM_SCAN_OOS) || \
       defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING) )
  if( ss_ptr->ss_pref_updated_status == SDSS_PREF_UPDATED_PREF_REAS &&
      pref_reason                    == SD_SS_PREF_REAS_ORIG_END &&
      ss_ptr->orig_mode_prev         == SD_SS_ORIG_MODE_EMERG_ORIG )
  {
     ss_ptr->ss_pref_updated_status = SDSS_PREF_UPDATED_EMERG_ORIG_END;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get current/new supported mode/band pref.
  */
  curr_supp_mode_pref  = ss_ptr->mode_pref;
  curr_supp_band_pref  = ss_ptr->band_pref;
  sd_misc_get_band_mode( &curr_supp_mode_pref, &curr_supp_band_pref,
                         SD_SS_MODE_PREF_ANY, SD_SS_BAND_PREF_ANY
                       );

  new_supp_mode_pref  = mode_pref;
  new_supp_band_pref  = band_pref;
  sd_misc_get_band_mode( &new_supp_mode_pref, &new_supp_band_pref,
                         SD_SS_MODE_PREF_ANY, SD_SS_BAND_PREF_ANY
                       );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the previous preferences fields.
  */
  ss_ptr->prev_mode_pref       = ss_ptr->mode_pref;
  ss_ptr->prev_band_pref       = ss_ptr->band_pref;
  ss_ptr->prev_prl_pref        = ss_ptr->prl_pref;
  ss_ptr->prev_roam_pref       = ss_ptr->roam_pref;
  ss_ptr->prev_hybr_pref       = ss_ptr->hybr_pref;
  ss_ptr->prev_domain_pref     = ss_ptr->domain_pref;
  ss_ptr->prev_acq_order_pref  = ss_ptr->acq_order_pref;
  ss_ptr->prev_tech_pref       = ss_ptr->tech_pref;
  ss_ptr->prev_wlan_scan_pref  = ss_ptr->wlan_scan_pref;
  ss_ptr->prev_bss_type_pref   = ss_ptr->bss_type_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If none of the new preferences is different than the existing
  ** preferences, return FALSE.
  */
  if( ss_ptr->orig_mode == orig_mode &&
      curr_supp_mode_pref == new_supp_mode_pref &&
      curr_supp_band_pref == new_supp_band_pref &&
      ss_ptr->roam_pref == roam_pref &&
      ss_ptr->prl_pref == prl_pref &&
      ss_ptr->hybr_pref == hybr_pref &&
      ss_ptr->acq_order_pref == acq_order_pref &&
      ss_ptr->domain_pref == domain_pref &&
      ss_ptr->tech_pref   == tech_pref &&
      ss_ptr->wlan_scan_pref == wlan_scan_pref &&
      ss_ptr->bss_type_pref == bss_type_pref
  )
  {
    return ss_ptr->ss_pref_updated_status;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the existing preferences.
  */
  ss_ptr->orig_mode     = orig_mode;
  ss_ptr->mode_pref     = mode_pref;
  ss_ptr->band_pref     = band_pref;
  ss_ptr->prl_pref      = prl_pref;
  ss_ptr->roam_pref     = roam_pref;
  ss_ptr->hybr_pref     = hybr_pref;
  ss_ptr->acq_order_pref= acq_order_pref;
  ss_ptr->domain_pref   = domain_pref;
  ss_ptr->tech_pref     = tech_pref;
  ss_ptr->wlan_scan_pref= wlan_scan_pref;
  ss_ptr->bss_type_pref = bss_type_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Overwrite the ss_pref_updated_status.
  **
  ** Regardless if orig mode or pref reason is changed or not,
  ** if only service domain is changed and all other preferences remains same,
  ** overwrite with SDSS_PREF_UPDATED_SRV_DOMAIN.
  **
  ** Otherwise overwrite with SDSS_PREF_UPDATED_TRUE_PREF.
  */
  if( curr_supp_mode_pref         == new_supp_mode_pref     &&
      curr_supp_band_pref         == new_supp_band_pref     &&
      ss_ptr->prev_prl_pref       == ss_ptr->prl_pref       &&
      ss_ptr->prev_roam_pref      == ss_ptr->roam_pref      &&
      ss_ptr->prev_hybr_pref      == ss_ptr->hybr_pref      &&
      ss_ptr->prev_acq_order_pref == ss_ptr->acq_order_pref &&
      ss_ptr->prev_domain_pref    != ss_ptr->domain_pref    &&
      ss_ptr->prev_tech_pref      == ss_ptr->tech_pref      &&
      ss_ptr->prev_wlan_scan_pref == ss_ptr->prev_wlan_scan_pref &&
      ss_ptr->bss_type_pref            == ss_ptr->bss_type_pref )
  {

    ss_ptr->ss_pref_updated_status = SDSS_PREF_UPDATED_SRV_DOMAIN;
  }
  else
  {

    #if ( defined (FEATURE_UNIFORM_SCAN_OOS) || \
          defined (FEATURE_UMTS_UNIFORM_OOS_HANDLING) )
    if( ss_ptr->ss_pref_updated_status != SDSS_PREF_UPDATED_EMERG_ORIG_END )
    {
      ss_ptr->ss_pref_updated_status = SDSS_PREF_UPDATED_TRUE_PREF;
    }
    #else
    ss_ptr->ss_pref_updated_status = SDSS_PREF_UPDATED_TRUE_PREF;
    #endif

  }

  return ss_ptr->ss_pref_updated_status;

} /* sdss_hdr_pref_update */
#endif /* FEATURE_HDR_HYBRID */

#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_redir_update0

DESCRIPTION
  Update the redirection status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                          sdss_redir_update0(

        sdss_s_type                   *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        sd_ss_redir_e_type            redir_type,
            /* Redirection type - GSRDM, SRDM, NDSS.
            */

        boolean                       is_valid,
            /* Indicate whether redirection is valid for this target.
            */

        word                          expected_sid,
            /* Expected SID. If the MS is being redirected to a specific
            ** system, indicate the SID of that system; otherwise, should be
            ** set to SD_WILDCARD_SID.
            */

        word                          expected_nid,
            /* Expected NID. If the MS is being redirected to a specific
            ** network, indicate the NID of that network; otherwise, should
            ** be set to SD_WILDCARD_NID.
            */

        boolean                       is_rif,
            /* Return if fail indicator.  Indicates whether the MS is
            ** required to return to the system from which it is being
            ** redirected upon failure to obtain service using the
            ** redirection criteria specified by this function call.
            */

        boolean                       is_ignore_cdma,
            /* IGNORE_CDMA indicator. Indicates whether the MS is to ignore
            ** the CDMA Capability Message on the analog system to which it
            ** is being redirected.
            */

        word                          max_redirect_delay
            /* Maximum delay upon redirection. Indicate the maximum delay
            ** time, in units of 8 second increments, to be used by MS in the
            ** event of a global redirection (GSRDM) to analog mode in order
            ** to avoid overloading an underlying analog cell's reverse
            ** control channel.
            **
            ** Note that a value of SD_SS_MAX_REDIR_DELAY_NONE indicate that
            ** this variable should not be used by AMPS CP to determine the
            ** registration time.
            */
)
{
  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( redir_type, SD_SS_REDIR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If redirection type is none, indicate that redirection is invalid.
  */
  if( redir_type == SD_SS_REDIR_NONE )
  {
    is_valid = FALSE;

    /* Reset delete_tmsi bit mask of the system in the redirection list.
    */
    sdsr_list_reset_status_mask(SDSR_REDIR_LST, SDSR_STATUS_MASK_DEL_TMSI);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->redir                     = redir_type;
  ss_ptr->redir_is_valid            = is_valid;
  ss_ptr->redir_expected_sid        = expected_sid;
  ss_ptr->redir_expected_nid        = expected_nid;
  ss_ptr->redir_is_rif              = is_rif;
  ss_ptr->redir_is_ignore_cdma      = is_ignore_cdma;
  ss_ptr->redir_max_redirect_delay  = max_redirect_delay;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If redirection is not none, update the redirection uptime per the
  ** current uptime.
  */
  if( ss_ptr->redir != SD_SS_REDIR_NONE )
  {
    ss_ptr->redir_uptime = clk_uptime();
  }

} /* sdss_redir_update0 */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_redir_update

DESCRIPTION
  Update the redirection status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                          sdss_redir_update(

        sd_ss_e_type                  ss,
          /* System selection - MAIN or HDR.
          */

        sd_ss_redir_e_type            redir_type,
            /* Redirection type - GSRDM, SRDM, NDSS.
            */

        boolean                       is_valid,
            /* Indicate whether redirection is valid for this target.
            */

        word                          expected_sid,
            /* Expected SID. If the MS is being redirected to a specific
            ** system, indicate the SID of that system; otherwise, should be
            ** set to SD_WILDCARD_SID.
            */

        word                          expected_nid,
            /* Expected NID. If the MS is being redirected to a specific
            ** network, indicate the NID of that network; otherwise, should
            ** be set to SD_WILDCARD_NID.
            */

        boolean                       is_rif,
            /* Return if fail indicator.  Indicates whether the MS is
            ** required to return to the system from which it is being
            ** redirected upon failure to obtain service using the
            ** redirection criteria specified by this function call.
            */

        boolean                       is_ignore_cdma,
            /* IGNORE_CDMA indicator. Indicates whether the MS is to ignore
            ** the CDMA Capability Message on the analog system to which it
            ** is being redirected.
            */

        word                          max_redirect_delay
            /* Maximum delay upon redirection. Indicate the maximum delay
            ** time, in units of 8 second increments, to be used by MS in the
            ** event of a global redirection (GSRDM) to analog mode in order
            ** to avoid overloading an underlying analog cell's reverse
            ** control channel.
            **
            ** Note that a value of SD_SS_MAX_REDIR_DELAY_NONE indicate that
            ** this variable should not be used by AMPS CP to determine the
            ** registration time.
            */
)
{
  sdss_s_type   *ss_ptr = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdss_redir_update0( ss_ptr,
                      redir_type,
                      is_valid,
                      expected_sid,
                      expected_nid,
                      is_rif,
                      is_ignore_cdma,
                      max_redirect_delay );
}




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_redir_update2

DESCRIPTION
  Update the redirection status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                          sdss_redir_update2(

        sd_ss_e_type                  ss,
          /* System selection - MAIN or HDR.
          */

        sd_ss_redir_e_type            redir_type,
            /* Redirection type - GSRDM, SRDM, NDSS.
            */

        boolean                       is_valid,
            /* Indicate whether redirection is valid for this target.
            */

        boolean                       is_rif,
            /* Return if fail indicator.  Indicates whether the MS is
            ** required to return to the system from which it is being
            ** redirected upon failure to obtain service using the
            ** redirection criteria specified by this function call.
            */

        const sd_redir_rec_s_type *rec_list_ptr
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

)
{
  word          expected_sid;
  word          expected_nid;
  boolean       is_ignore_cdma;
  word          max_redirect_delay;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set default value for expected_sid, expected_nid, is_ignore_cdma and
  ** max_redirect_delay.
  */
  expected_sid = SD_WILDCARD_SID;
  expected_nid = SD_WILDCARD_NID;
  is_ignore_cdma = FALSE;
  max_redirect_delay = SD_SS_MAX_REDIR_DELAY_NONE;

  /* Set the expected_sid, is_ignore_cdma and max_redirect_delay per first
  ** redirection record.
  */

  if( rec_list_ptr != NULL )
  {
    if( rec_list_ptr->record_type == SD_REDIR_REC_CDMA )
    {
      expected_sid = rec_list_ptr->rec.cdma_rec.expected_sid;
      expected_nid = rec_list_ptr->rec.cdma_rec.expected_nid;
    }


    if( rec_list_ptr->record_type == SD_REDIR_REC_AMPS )
    {
      expected_sid = rec_list_ptr->rec.amps_rec.expected_sid;
      is_ignore_cdma = rec_list_ptr->rec.amps_rec.is_ignore_cdma;
      max_redirect_delay = rec_list_ptr->rec.amps_rec.max_redirect_delay;
    }
  }

  /* Update the SS-Core with the redirection type.
  */
  sdss_redir_update( ss,                  /* system selection */
                     redir_type,          /* Redirection type */
                     is_valid,            /* Valid redirection indicator */
                     expected_sid,        /* Expected SID */
                     expected_nid,        /* Expected NID */
                     is_rif,              /* RETURN_IF_FAIL indicator */
                     is_ignore_cdma,      /* IGNORE_CDMA indicator */
                     max_redirect_delay); /* MAX_REDIRECT_DELAY value */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

}/* sdss_redir_update2 */


#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
#endif


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_cdma_lock_update

DESCRIPTION
  Update the CDMA lock status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN void                          sdss_cdma_lock_update(

        boolean                       is_cdma_locked
            /* Indicate whether MS is CDMA locked until power-cycle.
            */
)
{
  sdss_s_type   *ss_ptr   = sdss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->is_cmda_locked = is_cdma_locked;

} /* sdss_cdma_lock_update */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_cdma_avail_update

DESCRIPTION
  Update the CDMA-available related parameters of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                          sdss_cdma_avail_update(

        sd_ss_e_type                  ss,
          /* System selection - MAIN or HDR.
          */

        boolean                       is_reset
          /* Indicate wether to reset the CDMA-available counter.
          */
)
{
  sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If need to reset the CDMA-available counter, do so. Else increment the
  ** counter.
  */
  if( is_reset )
  {
    ss_ptr->cdma_avail_cnt = 0;
  }
  else
  {
    ss_ptr->cdma_avail_cnt++;
  }

} /* sdss_cdma_avail_update */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_acc_update

DESCRIPTION
  Update the access related parameters of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                          sdss_acc_update(

        sd_ss_e_type                  ss,
          /* System selection - MAIN or HDR.
          */

        sd_ss_acc_reas_e_type         acc_reas,
            /* Access termination reason.
            */

        sd_ss_acc_term_e_type         acc_term
            /* Access termination reason.
            */
)
{
  sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acc_reas, SD_SS_ACC_REAS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acc_term, SD_SS_ACC_TERM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->acc_reas = acc_reas;
  ss_ptr->acc_term = acc_term;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If access reason is successful, reset the registration access fail count
  ** and start time.
  */
  if( acc_term == SD_SS_ACC_TERM_NORMAL )
  {
    ss_ptr->acc_fail_cnt = 0;
    ss_ptr->acc_fail_start_time = ~0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (access is not successful) if access reason is registration,
  ** increment the registration access fail count and set the registration
  ** access fail start time, as needed.
  */
  else if( acc_reas == SD_SS_ACC_REAS_REG ||
           acc_reas == SD_SS_ACC_REAS_OTHER )
  {
    dword  uptime = clk_uptime();

    ss_ptr->acc_fail_cnt++;
    ss_ptr->acc_fail_start_time = MIN( ss_ptr->acc_fail_start_time, uptime );

    SD_MSG_LOW( "acc_fail_start_time=%d, uptime=%d",
                 ss_ptr->acc_fail_start_time, uptime, 0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset the CDMA-available counter.
  */
  sdss_cdma_avail_update( SD_SS_MAIN, TRUE );

} /* sdss_acc_update */


#if defined(FEATURE_HDR)
#error code not present
#endif /* (FEATURE_HDR) */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_acc_reset

DESCRIPTION
  Reset the registration access failure count and start time.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                          sdss_acc_reset(

        sdss_s_type                   *ss_ptr
            /* Pointer to a System Selection information structure.
            */
)
{
  SD_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->acc_fail_cnt = 0;
  ss_ptr->acc_fail_start_time = ~0;

} /* sdss_acc_reset */




#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_ho_update

DESCRIPTION
  Update the handoff status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Update the origination mode from emergency-orig to emergency-callback, as
  necessary.

===========================================================================*/
MMEXTN  void                          sdss_ho_update(

        sd_ss_ho_e_type               handoff
            /* Handoff type.
            */
)
{
  sdss_s_type   *ss_ptr   = sdss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( handoff, SD_SS_HO_MAX )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If handoff it other than AMPS->AMPS, update the handoff status of the
  ** SS-Core.
  */
  if( handoff != SD_SS_HO_AMPS_CAM_TO_AMPS_VOICE )
  {
    ss_ptr->handoff = handoff;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If handoff is other than none, reset the registration access fail count
  ** and start time (as we successfully accessed the system).
  */
  if( handoff != SD_SS_HO_NONE )
  {
    ss_ptr->acc_fail_cnt = 0;
    ss_ptr->acc_fail_start_time = ~0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If origination mode is emergency origination and handoff is a channel
  ** assignment to AMPS voice channel, change the origination mode to
  ** emergency callback and inform the registered client (typically CM) that
  ** SD entered emergency callback mode of operation.
  */
  if( ss_ptr->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG
                           &&
      (handoff == SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE ||
       handoff == SD_SS_HO_AMPS_CAM_TO_AMPS_VOICE) )
  {
    ss_ptr->orig_mode = SD_SS_ORIG_MODE_EMERG_CB;
    sd_si_emerg_cb_mode();
  }

} /* sdss_ho_update */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_orig_success_update

DESCRIPTION
  Update the orig-success status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                          sdss_orig_success_update( void )
{
  sdss_s_type   *ss_ptr   = sdss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If origination mode is emergency origination, change the origination
  ** mode to emergency callback and inform the registered client (typically
  ** CM) that SD entered emergency callback mode of operation.
  */
  if( ss_ptr->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG )
  {
    ss_ptr->orig_mode = SD_SS_ORIG_MODE_EMERG_CB;
    sd_si_emerg_cb_mode();
  }

} /* sdss_orig_success_update */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_acq_type_update

DESCRIPTION
  Update the acquisition type of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                          sdss_acq_type_update(

        sdss_s_type                   *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        sdssscr_acq_e_type            acq_type
            /* Acquisition type.
            */
)
{
  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( acq_type, SDSSSCR_ACQ_MAX );
  SD_ASSERT( acq_type != SDSSSCR_ACQ_SAME_AS_LAST );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->acq_type = acq_type;

} /* sdss_acq_type_update */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_update_avail_sys_status

DESCRIPTION
  Goes throught the entire system table and updates the AVAIL/UNAVAUL
  prl flag based on the prl_pref.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdss_update_avail_sys_status
(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list
            /* List for which to update the avail status.
            */
)
{

  sdsr_s_type   *sr_ptr   = NULL;
  int i = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse the entire system record table.
  */
  while ( (sr_ptr = sdsr_list_get_ptr( list, i )) != NULL )
  {

    /* Check if the system is marked as avail or unavail.
    */
    if ( sr_ptr->prl == (byte)SDSS_PRL_AVAIL ||
         sr_ptr->prl == (byte)SDSS_PRL_UNAVAIL
       )
    {
      sr_ptr->prl = (byte)SDSS_PRL_AVAIL;
      if ( sdss_sr_is_pref_conf( ss, sr_ptr, SDSS_PREF_CONF_PRL ))
      {
        sr_ptr->prl = (byte)SDSS_PRL_UNAVAIL;
      }
    }
    i++;
  } /* while () */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* sdss_update_avail_sys_status */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_pref_chgd

DESCRIPTION
  Check whether preference specified in the parameter changed during the last
  preferred system changed command.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if preference changed.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdss_is_pref_chgd(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdss_pref_chgd_e_type     pref_chgd
            /* SS-Preference changed to be evaluated.
            */
)
{
  sdss_s_type   *ss_ptr = sdss_ptr( ss );
  boolean             is_pref_chgd  = FALSE;
  sd_ss_mode_pref_e_type   prev_supp_mode_pref;
  sd_ss_mode_pref_e_type   curr_supp_mode_pref;
  sd_ss_band_pref_e_type   prev_supp_band_pref;
  sd_ss_band_pref_e_type   curr_supp_band_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( pref_chgd, SDSS_PREF_CHGD_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get supported mode and band pref.
  */
  curr_supp_mode_pref  = ss_ptr->mode_pref;
  curr_supp_band_pref  = ss_ptr->band_pref;
  sd_misc_get_band_mode( &curr_supp_mode_pref, &curr_supp_band_pref,
                         SD_SS_MODE_PREF_ANY, SD_SS_BAND_PREF_ANY
                       );

  prev_supp_mode_pref  = ss_ptr->prev_mode_pref;
  prev_supp_band_pref  = ss_ptr->prev_band_pref;
  sd_misc_get_band_mode( &prev_supp_mode_pref, &prev_supp_band_pref,
                         SD_SS_MODE_PREF_ANY, SD_SS_BAND_PREF_ANY
                       );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If requested, check for mode preference changes.
  */
  if( SDSS_PREF_CONTAIN( pref_chgd, SDSS_PREF_CHGD_MODE ))
  {
    if( curr_supp_mode_pref != prev_supp_mode_pref )
    {
      is_pref_chgd = TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If requested, check for band preference changes.
  */
  if( SDSS_PREF_CONTAIN( pref_chgd, SDSS_PREF_CHGD_BAND ) )
  {
    if( curr_supp_band_pref != prev_supp_band_pref )
    {
      is_pref_chgd = TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If requested, check for roam preference changes.
  */
  if( SDSS_PREF_CONTAIN( pref_chgd, SDSS_PREF_CHGD_ROAM ) )
  {
    if( ss_ptr->roam_pref != ss_ptr->prev_roam_pref )
    {
      is_pref_chgd = TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If requested, check for hybrid preference changes.
  */
  if( SDSS_PREF_CONTAIN( pref_chgd, SDSS_PREF_CHGD_HYBR ) )
  {
    if( ss_ptr->hybr_pref != ss_ptr->prev_hybr_pref )
    {
      is_pref_chgd = TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If requested, check for domain preference changes.
  */
  if( SDSS_PREF_CONTAIN( pref_chgd, SDSS_PREF_CHGD_DOMAIN ) )
  {
    if( ss_ptr->domain_pref != ss_ptr->prev_domain_pref )
    {
      is_pref_chgd = TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If requested, check for acq. order preference changes.
  */
  if( SDSS_PREF_CONTAIN( pref_chgd, SDSS_PREF_CHGD_ACQ_ORDER ) )
  {
    if( ss_ptr->acq_order_pref != ss_ptr->prev_acq_order_pref )
    {
      is_pref_chgd = TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the pref_conf category is CONF_NONE negate the return value;
  */
  if( pref_chgd == SDSS_PREF_CHGD_NONE )
  {
    is_pref_chgd = !is_pref_chgd;
  }

  return BOOLEAN( is_pref_chgd );

} /* sdss_is_pref_chgd */

/*===========================================================================

FUNCTION sdss_is_band_pref_chgd

DESCRIPTION
  Check whether preference specified in the parameter changed during the last
  preferred system changed command.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if preference changed.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdss_is_band_pref_chgd(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sd_ss_mode_pref_e_type    mode
            /* mode of bands to be evaluated.
            */
)
{
  sdss_s_type   *ss_ptr = sdss_ptr( ss );
  boolean             is_pref_chgd  = FALSE;
  sd_ss_mode_pref_e_type   prev_supp_mode_pref;
  sd_ss_mode_pref_e_type   curr_supp_mode_pref;
  sd_ss_band_pref_e_type   prev_supp_band_pref;
  sd_ss_band_pref_e_type   curr_supp_band_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  SD_ASSERT( ss_ptr != NULL );
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get supported mode and band pref.
  */
  curr_supp_mode_pref  = ss_ptr->mode_pref;
  curr_supp_band_pref  = ss_ptr->band_pref;
  sd_misc_get_band_mode( &curr_supp_mode_pref, &curr_supp_band_pref,
                         mode, SD_SS_BAND_PREF_ANY
                       );

  prev_supp_mode_pref  = ss_ptr->prev_mode_pref;
  prev_supp_band_pref  = ss_ptr->prev_band_pref;
  sd_misc_get_band_mode( &prev_supp_mode_pref, &prev_supp_band_pref,
                         mode, SD_SS_BAND_PREF_ANY
                       );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check for band preference changes.
  */

  if( curr_supp_band_pref != prev_supp_band_pref )
  {
     is_pref_chgd = TRUE;
  }
 

  return BOOLEAN( is_pref_chgd );

} /* sdss_is_band_pref_chgd */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_pref_updated

DESCRIPTION
  Check whether preference specified in the parameter changed during the last
  preferred system changed command.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if preference changed.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdss_is_pref_update(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdss_pref_updated_e_type     pref_update
            /* SS-Preference changed to be evaluated.
            */
)
{
  sdss_s_type   *ss_ptr = sdss_ptr( ss );

  SD_ASSERT( ss_ptr != NULL )
  SD_ASSERT_ENUM_IS_INRANGE( pref_update, SDSS_PREF_UPDATE_MAX );

  return ( BOOLEAN( ss_ptr->ss_pref_updated_status == pref_update ));

} /* sdss_is_pref_updated */

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_stay_current_state

DESCRIPTION
  Decide on if ss should stay on the current state.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  The next System Selection state.

SIDE EFFECTS
  None.

===========================================================================*/
//static boolean                  sdss_is_stay_current_state(

//       sd_ss_e_type             ss,
//         /* The ss for the event
//         */

//       sdss_evt_e_type          event,
//          /* SS-Event that was process.
//          */

//       sdss_pref_updated_e_type pref_updated_status,
//          /* Enumeration of preference updated type.
//          */
//
//       const sdss_iact_s_type   *rtrn_iact_ptr
//        /* Pointer to a buffer where returned internal action is placed.
//          */
//)
//{
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

//  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
//  SD_ASSERT_ENUM_IS_INRANGE( pref_updated_status, SDSS_PREF_UPDATE_MAX );
//  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If event is not user ss pref change, return now.
  */
//  if( event != SDSS_EVT_USER_SS_PREF )
//  {
//    return FALSE;
//  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the ss pref change is not SDSS_PREF_UPDATED_SRV_DOMAIN, return now.
  */
//  if(  pref_updated_status != SDSS_PREF_UPDATED_SRV_DOMAIN )
//  {
//    return FALSE;
//  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the action is not acquisition, return now.
  */
//  if( rtrn_iact_ptr->act != SDSS_IACT_ACQUIRE )
//  {
//    return FALSE;
//  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If  both mode of idle system and attempt system are GW.
  ** decide to stay on the current state.
  */

//  if( sdsr_is_sys( SD_MODE_GW, sdss_sr_list_map2( ss,SDSR_IDL_SYS), 0 ) &&
//     sdsr_is_sys( SD_MODE_GW, rtrn_iact_ptr->prm.acq.list, rtrn_iact_ptr->prm.acq.pos )
//    )
//  {
//    SD_MSG_HIGH("Stay on current state, event=%d", event, 0, 0);

//    return TRUE;
//  }

//  return FALSE;

// }/* sdss_is_stay_current_state */





/* <EJECT> */
/*===========================================================================

FUNCTION sdss_state_next

DESCRIPTION
  Decide on the next System Selection state.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  The next System Selection state.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdss_state_e_type     sdss_state_next(
        /*lint -esym(715, ss) */ /* not referenced */
        sd_ss_e_type          ss,
            /* System selection - MAIN or HDR.
            */

        sdss_state_e_type     ss_state,
            /* Current SS-State.
            */

        sdss_evt_e_type       event,
            /* SS-Event that was process.
            */

        sdss_iact_s_type      *rtrn_iact_ptr
            /* Pointer to a buffer where returned internal action is placed.
            */
)
{
  sdss_iact_e_type        act;
  boolean                 is_dedicated_meas_mode;
  sdss_state_e_type       prev_ss_state;
                      /*lint -esym(550, prev_ss_state) */ /* not accessed */         

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( ss_state, SDSS_STATE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get the action and dedicated measurement mode indication.
  */
  act                     = rtrn_iact_ptr->act;
  is_dedicated_meas_mode  = BOOLEAN( rtrn_iact_ptr->prm.meas.mode ==
                                     SDSS_MEAS_MODE_DED );
  prev_ss_state           = ss_state;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If action is acquire or perform measurement in dedicated mode, change
  ** the SS-State to acquisition.
  */
  if( act == SDSS_IACT_ACQUIRE  ||
      act == SDSS_IACT_PWR_SAVE ||
      act == SDSS_IACT_GET_NET_WLAN ||
      (act == SDSS_IACT_MEAS && is_dedicated_meas_mode) )
  {
    ss_state = SDSS_STATE_ACQ;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, if event is CDMA SCHM, AMPS control channel SID or HDR
  ** acquisition SID and action is continue, change the SS-State to
  ** verification.
  */
  else if( event == SDSS_EVT_CDMA_ACQ_SCHM ||
           event == SDSS_EVT_AMPS_ACQ_CCH_SID ||
           event == SDSS_EVT_HDR_ACQ_SID ||
           event == SDSS_EVT_GW_ACQ_PLMN_SUCCESS ||
           event == SDSS_EVT_WLAN_ACQ_ATTACH_OK
         )
  {
    if( act == SDSS_IACT_CONTINUE )
    {
      ss_state = SDSS_STATE_VRFY;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, if event is CDMA overhead info, AMPS paging channel SID or HDR
  ** overhead info and action is continue or measurement in background mode,
  ** change the SS-State to operation.
  */
  else if( event == SDSS_EVT_CDMA_OPR_OH_SID  ||
           event == SDSS_EVT_CDMA_OPR_OH_CHAN ||
           event == SDSS_EVT_AMPS_OPR_PCH_SID ||
           event == SDSS_EVT_HDR_OPR_OH_SID   ||
           event == SDSS_EVT_HDR_OPR_OH_CHAN  ||
           event == SDSS_EVT_GW_OPR_SRV_INFO  ||
           event == SDSS_EVT_WLAN_OPR_ATTACH_OK
         )
  {
    if( (act == SDSS_IACT_CONTINUE) ||
        (act == SDSS_IACT_MEAS && !is_dedicated_meas_mode) )
    {
      ss_state = SDSS_STATE_OPR;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else if event is power save, enter the acq state is PWR save
  */

  else if( event == SDSS_EVT_MISC_PWR_SAVE_ENTER &&
           act == SDSS_IACT_CONTINUE )
  {
    ss_state = SDSS_STATE_PWR_SAVE;
  }
  else if( event == SDSS_EVT_CDMA_ACQ_HO_FAIL )
  {
    ss_state = SDSS_STATE_VRFY;
  }

  /* Inform the SS-Front end of the SS-State.
  */
  SD_MSG_MED("ss state = %d->%d, event=%d",
              prev_ss_state, ss_state, event);

  rtrn_iact_ptr->ss_state = ss_state;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ss_state;

} /* sdss_state_next *//*lint +esym(550, prev_state) */ /* not accessed */
                       /*lint +esym(715, ss) */ /* not referenced */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_resel_state_update

DESCRIPTION
  Update the reselection state variable.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdss_resel_state_update(

        sdss_s_type               *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        sdss_resel_stt_e_type     resel_state
            /* New reselection state.
            */
)
{
  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( resel_state, SDSS_RESEL_STT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->resel_state = resel_state;

} /* sdss_resel_state_update */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_resel_mode_update

DESCRIPTION
  Update the reselection mode variable.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdss_resel_mode_update(

        sdss_s_type               *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        sdss_resel_mode_e_type    resel_mode
            /* New reselection mode.
            */
)
{
  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( resel_mode, SDSS_RESEL_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->resel_mode = resel_mode;

} /* sdss_resel_mode_update */


/*===========================================================================

FUNCTION sdss_set_flag

DESCRIPTION
  Set the flag to set_val.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdss_set_flag(

        sdss_s_type               *ss_ptr,
            /* Pointer to the ss object
            */

        sdss_flag_e_type       flag,
            /* The flag to set
            */

        boolean                   set_val
            /* The value the flag is supposed to be set to
            */
)
{

  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( flag, SDSS_FLAG_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( flag )
  {
    case SDSS_FLAG_TRUE_EVENT_PROCESSED:
      ss_ptr->is_true_event_processed = set_val;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDSS_FLAG_SYNC_UP_IN_PROGRESS:
      #ifdef FEATURE_HICPS_STACK_SYNC_UP
#error code not present
      #endif /* FEATURE_HICPS_STACK_SYNC_UP */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_FLAG_ENG_SCR_TIMER:
      ss_ptr->is_eng_scr_timer_set = set_val;
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_FLAG_CONN_MODE_UOOS_AWAKE:
      ss_ptr->is_conn_uoos_awake = set_val;
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
#ifdef FEATURE_IS683A_450M_SUPPORT
    case SDSS_FLAG_IS683A_450M_SUPPORT: 
      ss_ptr->is_IS683A_450M_support = set_val; 
      break; 
#endif
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDSS_FLAG_NONE:
    case SDSS_FLAG_MAX:
    default:

      SD_CASE_ERR(" Invalid flag %d", flag, 0,0 );
      break;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  }
  return;
} /* sdss_set_flag */

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_set_flag2

DESCRIPTION
  Check whether a specified SS-Preference reason equals the current
  SS-Preference reason.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                       sdss_set_flag2(

        sd_ss_e_type        ss,
            /* System selection - MAIN or HDR.
            */

        sdss_flag_e_type    flag,
            /* SS-Preference reason combination type.
            */

        boolean             val
            /* Set the Stack Sync Up Flag to this value
            */
)
{
  sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdss_set_flag(ss_ptr, flag, val);

} /* sdss_set_flag2 */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_cdma_request_mode

DESCRIPTION
  Get the CDMA request_mode to be used in CDMA origination and page-response
  messages.

  The CDMA request mode is determined according to the current mode and
  band preferences.

DEPENDENCIES
  None.

RETURN VALUE
  CDMA request_mode as defined in ANSI J-STD-008 Table 2.7.1.3.2.4-1 and
  cai.h.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  byte                  sdss_get_cdma_request_mode( void )
{

  /* If this target has AMPS support, do the following:
  */
  #ifdef FEATURE_ACP
#error code not present
  #elif defined(SD_USES_CAI)

  return CAI_CDMA_ONLY;

  #else

  return 0;

  #endif

} /* sdss_get_cdma_request_mode */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_band_capability

DESCRIPTION

  Get target band capability by using RF API or based on feature define.
  The AMPS capability is not considered here. It is done seperately in
  function sdss_get_supp_band_pref().

DEPENDENCIES
  NV task has started

RETURN VALUE
  sd_ss_band_pref_e_type

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_band_pref_e_type    sdss_get_band_capability( void )
{
  return ((sd_ss_band_pref_e_type) rfm_get_band_mask(RFCOM_TRANSCEIVER_0)
           #ifdef FEATURE_WLAN
#error code not present
           #endif
         );
}



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_amps_band_pref

DESCRIPTION
  Return a band preference mask that corresponds to all the supported AMPS
  bands out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band preference mask that corresponds to all the supported AMPS bands out
  of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_ss_band_pref_e_type    sdss_get_supp_amps_band_pref(

        sd_ss_band_pref_e_type    band_pref
            /* Band preference out of which to extract supported AMPS bands.
            */
)
{
  sd_ss_band_pref_e_type    band_supp = SD_SS_BAND_PREF_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   #if( defined(FEATURE_ACP) )
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (SD_GET_COMMON_BAND(band_pref, band_supp));

} /* sdss_get_supp_amps_band_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_cdma_band_pref

DESCRIPTION
  Return a band preference mask that corresponds to all the supported CDMA
  bands out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band preference mask that corresponds to all the supported CDMA bands out
  of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_ss_band_pref_e_type    sdss_get_supp_cdma_band_pref(

        sd_ss_band_pref_e_type    band_pref
            /* Band preference out of which to extract supported CDMA bands.
            */
)
{
  sdss_s_type *ss_ptr = sdss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e655 */ /* bit-wise operation uses enum */
  return ( band_pref &
           ss_ptr->band_capability 
       &
       #if (defined(FEATURE_CDMA ) || defined(FEATURE_HDR) || defined(FEATURE_JCDMA))
           ( SD_SS_BAND_PREF_BC0 |
             SD_SS_BAND_PREF_BC1  |
             SD_SS_BAND_PREF_BC3  |
             SD_SS_BAND_PREF_BC4  |
             SD_SS_BAND_PREF_BC5  |
             SD_SS_BAND_PREF_BC6  |
             SD_SS_BAND_PREF_BC7  |
             SD_SS_BAND_PREF_BC8  |
             SD_SS_BAND_PREF_BC9  |
             SD_SS_BAND_PREF_BC10 |
             SD_SS_BAND_PREF_BC11 |
             SD_SS_BAND_PREF_BC12 |
             SD_SS_BAND_PREF_BC14 |
             SD_SS_BAND_PREF_BC15 |
             SD_SS_BAND_PREF_BC16 |
             SD_SS_BAND_PREF_BC17 |
             SD_SS_BAND_PREF_BC18 |
             SD_SS_BAND_PREF_BC19 )
             #else
             SD_SS_BAND_PREF_NONE             
             #endif
           ); /*lint +e655 */

} /* sdss_get_supp_cdma_band_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_hdr_band_pref

DESCRIPTION
  Return a band preference mask that corresponds to all the supported HDR
  bands out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band preference mask that corresponds to all the supported HDR bands out
  of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_ss_band_pref_e_type    sdss_get_supp_hdr_band_pref(

        sd_ss_band_pref_e_type    band_pref
            /* Band preference out of which to extract supported HDR bands.
            *//*lint -esym(715, band_pref) */ /* not referenced */

)
{
  #ifdef FEATURE_HDR
#error code not present
  #else
  return SD_SS_BAND_PREF_NONE;
  #endif
} /* sdss_get_supp_hdr_band_pref *//*lint +esym(715, band_pref) */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_gsm_band_pref

DESCRIPTION
  Return a band preference mask that corresponds to all the supported GSM
  bands out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band preference mask that corresponds to all the supported GSM bands out
  of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_band_pref_e_type    sdss_get_supp_gsm_band_pref(

        sd_ss_band_pref_e_type    band_pref
            /* Band preference out of which to extract supported AMPS bands.
            */
)
{
  sdss_s_type *ss_ptr = sdss_ptr( SD_SS_MAIN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e655 */ /* bit-wise operation uses enum */
  return ( band_pref &
           ss_ptr->band_capability &
           ( SD_SS_BAND_PREF_GSM_450 |
             SD_SS_BAND_PREF_GSM_480 |
             SD_SS_BAND_PREF_GSM_750 |
             SD_SS_BAND_PREF_GSM_850 |
             SD_SS_BAND_PREF_GSM_EGSM_900 |
             SD_SS_BAND_PREF_GSM_PGSM_900 |
             SD_SS_BAND_PREF_GSM_RGSM_900 |
             SD_SS_BAND_PREF_GSM_DCS_1800 |
             SD_SS_BAND_PREF_GSM_PCS_1900)); /*lint +e655 */

} /* sdss_get_supp_gsm_band_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_wcdma_band_pref

DESCRIPTION
  Return a band preference mask that corresponds to all the supported WCDMA
  bands out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band preference mask that corresponds to all the supported WCDMA bands out
  of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_band_pref_e_type    sdss_get_supp_wcdma_band_pref(

        sd_ss_band_pref_e_type    band_pref
            /* Band preference out of which to extract supported AMPS bands.
            */
)
{
  sdss_s_type *ss_ptr = sdss_ptr( SD_SS_MAIN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e655 */ /* bit-wise operation uses enum */

  #ifdef HW_HAS_EDGE_ONLY_MODEM
#error code not present
  #endif
  {
    return ( band_pref &
             ss_ptr->band_capability &
             ( SD_SS_BAND_PREF_WCDMA_I_IMT_2000  |
               SD_SS_BAND_PREF_WCDMA_II_PCS_1900 |
               SD_SS_BAND_PREF_WCDMA_III_1700    |
               SD_SS_BAND_PREF_WCDMA_IV_1700     |
               SD_SS_BAND_PREF_WCDMA_V_850       |
               SD_SS_BAND_PREF_WCDMA_VI_800      |
               SD_SS_BAND_PREF_WCDMA_VII_2600    |
               SD_SS_BAND_PREF_WCDMA_IX_1700     |
               SD_SS_BAND_PREF_WCDMA_VIII_900 ));
  }

} /* sdss_get_supp_wcdma_band_pref */
/*lint -restore */

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_wlan_band_pref

DESCRIPTION
  Return a band preference mask that corresponds to all the supported WLAN
  bands out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band preference mask that corresponds to all the supported CDMA bands out
  of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_ss_band_pref_e_type    sdss_get_supp_wlan_band_pref(

        sd_ss_band_pref_e_type    band_pref
            /* Band preference out of which to extract supported WLAN bands.
            */
)
{
  sdss_s_type *ss_ptr = sdss_ptr( SD_SS_MAIN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e655 */ /* bit-wise operation uses enum */
  return ( band_pref &
           ss_ptr->band_capability &
           SD_SS_BAND_PREF_WLAN_ANY
         );
  /*lint +e655 */ /* bit-wise operation uses enum */


} /* sdss_get_supp_wlan_band_pref */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_mode_capability

DESCRIPTION
  Provide mode preference based on featuriztion or HW capability.

DEPENDENCIES
  None.

RETURN VALUE
  Return a mode preference supported by the target.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_mode_pref_e_type    sdss_get_mode_capability( void )
{
  sd_ss_mode_pref_e_type   mode_capability;

  /* Populate the mode capability based on the featurization.
  */
  mode_capability = SD_SS_MODE_PREF_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if( defined(FEATURE_ACP) )
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if( defined(FEATURE_WLAN) )
  mode_capability = SD_MODE_ADD( mode_capability, SD_SS_MODE_PREF_WLAN );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if( defined(FEATURE_HDR) )
  mode_capability = SD_MODE_ADD( mode_capability, SD_SS_MODE_PREF_HDR );
  #endif

  #if( defined(FEATURE_CDMA) || defined(FEATURE_JCDMA) )
  mode_capability = SD_MODE_ADD( mode_capability, SD_SS_MODE_PREF_CDMA );
  #endif

  #if( defined(FEATURE_GSM) )
#error code not present
  #endif

  #if( defined(FEATURE_WCDMA) )
  mode_capability = SD_MODE_ADD( mode_capability, SD_SS_MODE_PREF_WCDMA);
  #endif

  #if( defined(FEATURE_GPSONE_MSBASED) )
  mode_capability = SD_MODE_ADD( mode_capability, SD_SS_MODE_PREF_GPS);
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MSM_SUPPORT
  /* Read the fuse to determine if the product support any mode
  */
  if ( msm_support_is_available(MSM_SUPPORT_1XEVDO) == MSM_SUPPORT_DISABLED )
  {
    mode_capability = SD_MODE_DELETE( mode_capability, SD_SS_MODE_PREF_HDR );
  }
  if ( msm_support_is_available(MSM_SUPPORT_1X) == MSM_SUPPORT_DISABLED )
  {
    mode_capability = SD_MODE_DELETE( mode_capability, SD_SS_MODE_PREF_CDMA );
  }
  if ( msm_support_is_available(MSM_SUPPORT_GSM) == MSM_SUPPORT_DISABLED )
  {
    mode_capability = SD_MODE_DELETE( mode_capability, SD_SS_MODE_PREF_GSM);
  }
  if ( msm_support_is_available(MSM_SUPPORT_UMTS) == MSM_SUPPORT_DISABLED )
  {
    mode_capability = SD_MODE_DELETE( mode_capability, SD_SS_MODE_PREF_WCDMA);
  }
  if ( msm_support_is_available(MSM_SUPPORT_CGPS) == MSM_SUPPORT_DISABLED )
  {
    mode_capability = SD_MODE_DELETE( mode_capability, SD_SS_MODE_PREF_GPS);
  }

  #endif



  return ( mode_capability );
}
#ifdef  FEATURE_IS683A_450M_SUPPORT
MMEXTN  boolean         sdss_precondition_satisfied( void ) 
{ 
  const sdss_s_type   *ss_ptr   = sdss_ptr(SD_SS_MAIN); 
 
  if (((ss_ptr->band_capability & SD_SS_BAND_PREF_BC5) != 0) && 
     ((ss_ptr->band_capability & SD_SS_BAND_PREF_BC1) == 0 || (ss_ptr->band_capability & SD_SS_BAND_PREF_BC0) == 0))  
  { 
    return TRUE; 
  } 
  else 
  { 
    return FALSE; 
  } 
}
#endif
/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_mode_band_capability

DESCRIPTION
  Fill in the modes and bands supported by the target.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                   sdss_get_mode_band_capability(

        sd_ss_mode_pref_e_type    *mode_pref_ptr,
            /* Mode preferences to be filled in.
            */

        sd_ss_band_pref_e_type    *band_pref_ptr
            /* Band preferences to be filled in.
            */
)
{
  sdss_s_type   * ss_ptr = sdss_ptr(SD_SS_MAIN);

  *band_pref_ptr = ss_ptr->band_capability;
  *mode_pref_ptr = ss_ptr->mode_capability;

} /* sd_misc_get_mode_band_capability */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_mode_pref

DESCRIPTION
  Return a mode preference that corresponds to all the supported modes
  preferences out of the input mode preference.

DEPENDENCIES
  None.

RETURN VALUE
  Return a mode preference that corresponds to all the supported modes
  preferences out of the input mode preference.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_mode_pref_e_type    sdss_get_supp_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference from which to extract supported modes.
            */

        sd_ss_band_pref_e_type    band_pref
            /* band preference from which to extract supported modes.
            */
)
{
  sd_ss_mode_pref_e_type    mode_supp = SD_SS_MODE_PREF_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add AMPS mode preference if supported.
  */
  if( sdss_get_supp_amps_band_pref(band_pref) != SD_SS_BAND_PREF_NONE )
  {
    mode_supp = SD_MODE_ADD( mode_supp, SD_SS_MODE_PREF_AMPS );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add CDMA mode preference if supported.
  */
  if( sdss_get_supp_cdma_band_pref(band_pref) != SD_SS_BAND_PREF_NONE )
  {
    mode_supp = SD_MODE_ADD( mode_supp, SD_SS_MODE_PREF_CDMA );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add HDR mode preference if supported.
  */
  if( sdss_get_supp_hdr_band_pref(band_pref) != SD_SS_BAND_PREF_NONE )
  {
    mode_supp = SD_MODE_ADD( mode_supp, SD_SS_MODE_PREF_HDR);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_GPSONE_MSBASED
#error code not present
  #endif /* FEATURE_GPSONE_MSBASED */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add GSM mode preference if supported.
  */
  if( sdss_get_supp_gsm_band_pref(band_pref) != SD_SS_BAND_PREF_NONE )
  {
    mode_supp = SD_MODE_ADD( mode_supp, SD_SS_MODE_PREF_GSM );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add WCDMA mode preference if supported.
  */
  if( sdss_get_supp_wcdma_band_pref(band_pref) != SD_SS_BAND_PREF_NONE )
  {
    mode_supp = SD_MODE_ADD( mode_supp, SD_SS_MODE_PREF_WCDMA );
  }

  /*check if we have WCDMA and GSW and then add GW*/
  if(SD_MODE_CONTAIN(mode_supp,SD_SS_MODE_PREF_GSM) && SD_MODE_CONTAIN(mode_supp,SD_SS_MODE_PREF_WCDMA))
  {
    mode_supp = SD_MODE_ADD(mode_supp, SD_SS_MODE_PREF_GW);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add WLAN mode preference if supported.
  */
  if( sdss_get_supp_wlan_band_pref(band_pref) != SD_SS_BAND_PREF_NONE )
  {
    mode_supp = SD_MODE_ADD( mode_supp, SD_SS_MODE_PREF_WLAN );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return all supported modes out of input mode preference.
  */
  return ( SD_GET_COMMON_MODE( mode_pref, mode_supp) );

} /* sdss_get_supp_mode_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_band_pref

DESCRIPTION
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_band_pref_e_type    sdss_get_supp_band_pref(

        sd_ss_band_pref_e_type    band_pref
            /* Band preference which to check for support.
            */
)
{ /*lint -e655 */ /* bit-wise operation uses (compatible) enum's */
  return ( sdss_get_supp_amps_band_pref( band_pref )  |
           sdss_get_supp_cdma_band_pref( band_pref )  |
           sdss_get_supp_hdr_band_pref( band_pref )   |
           sdss_get_supp_gsm_band_pref( band_pref)    |
           sdss_get_supp_wcdma_band_pref( band_pref ) |
           sdss_get_supp_wlan_band_pref( band_pref )  ); /*lint +e655 */

} /* sdss_get_supp_band_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_mode_band_pref

DESCRIPTION
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference and the specific mode.

DEPENDENCIES
  None.

RETURN VALUE
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference and the specific mode.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_band_pref_e_type    sdss_get_supp_mode_band_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference which to check for support.
            */
)
{

  sd_ss_band_pref_e_type band_supp = SD_SS_BAND_PREF_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified mode preference includes AMPS, keep AMP related band
  ** preference out of the input band preference.
  */
  if( SD_MODE_CONTAIN( mode_pref, SD_SS_MODE_PREF_AMPS ) )
  {
    band_supp = sdss_get_supp_amps_band_pref(band_pref);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified mode preference includes CDMA, keep CDMA related band
  ** preference out of the input band preference.
  */
  if( SD_MODE_CONTAIN( mode_pref, SD_SS_MODE_PREF_CDMA ) )
  {
    band_supp = SD_BAND_ADD( band_supp,
                             sdss_get_supp_cdma_band_pref(band_pref) );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified mode preference includes HDR, keep HDR related band
  ** preference out of the input band preference.
  */
  if( SD_MODE_CONTAIN( mode_pref, SD_SS_MODE_PREF_HDR ) )
  {
    band_supp = SD_BAND_ADD( band_supp,
                             sdss_get_supp_hdr_band_pref(band_pref) );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_GSM
#error code not present
  #endif /* FEATURE_GSM */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WCDMA
#error code not present
  #endif /* FEATURE_WCDMA */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_GPSONE_MSBASED
#error code not present
  #endif /* FEATURE_GPSONE_MSBASED */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  return band_supp;

} /* sdss_get_supp_mode_band_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_supp_mode_band_pref

DESCRIPTION
  Check whether a specified combination of mode and band preference is
  supported by the current target.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdss_is_supp_mode_band_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference which to check for support.
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified mode preference includes AMPS and AMPS is supported over
  ** specified band preference, return TRUE.
  */
  if( SD_MODE_CONTAIN( mode_pref, SD_SS_MODE_PREF_AMPS ) )
  {
    if( sdss_get_supp_amps_band_pref(band_pref) != SD_SS_BAND_PREF_NONE )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified mode preference includes CDMA and CDMA is supported over
  ** specified band preference, return TRUE.
  */
  if( SD_MODE_CONTAIN( mode_pref, SD_SS_MODE_PREF_CDMA ) )
  {
    if( sdss_get_supp_cdma_band_pref(band_pref) != SD_SS_BAND_PREF_NONE )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified mode preference includes HDR and HDR is supported over
  ** specified band preference, return TRUE.
  */
  if( SD_MODE_CONTAIN( mode_pref, SD_SS_MODE_PREF_HDR ) )
  {
    if( sdss_get_supp_hdr_band_pref(band_pref) != SD_SS_BAND_PREF_NONE )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_GPSONE_MSBASED
#error code not present
  #endif /* FEATURE_GPSONE_MSBASED */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_GSM
#error code not present
  #endif /* FEATURE_GSM */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WCDMA
#error code not present
  #endif /* FEATURE_WCDMA */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the specified combination of mode and band preference is
  ** not supported by the current target - return FALSE.
  */
  return FALSE;

} /* sdss_is_supp_mode_band_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_supp_mode_band

DESCRIPTION
  Check whether a specified combination of mode and band is supported by the
  current target.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified combination of mode and band is supported by the
  current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdss_is_supp_mode_band(

        sd_mode_e_type            mode,
            /* Mode which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference which to check for support.
            */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return a boolean indicating whether the mapped band and mode preference
  ** combination is supported.
  */
  return sdss_is_supp_mode_band_pref( sdss_map_mode_to_mode_pref(mode),
                                      band_pref );

} /* sdss_is_supp_mode_band */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_map_mode_to_mode_pref

DESCRIPTION
  Map mode to mode preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band perference.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_mode_pref_e_type    sdss_map_mode_to_mode_pref(

        sd_mode_e_type            mode
          /* Mode mapped to mode pref.
          */
)
{
  sd_ss_mode_pref_e_type    mode_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Map the specified mode to its corresponding mode preferences.
  */
  switch( mode )
  {
    case SD_MODE_AMPS:
      mode_pref = SD_SS_MODE_PREF_AMPS;
      break;

    case SD_MODE_CDMA:
      mode_pref = SD_SS_MODE_PREF_CDMA;
      break;

    case SD_MODE_HDR:
      mode_pref = SD_SS_MODE_PREF_HDR;
      break;

    case SD_MODE_GPS:
      mode_pref = SD_SS_MODE_PREF_GPS;
      break;

    case SD_MODE_GSM:
      mode_pref = SD_SS_MODE_PREF_GSM;
      break;

    case SD_MODE_WCDMA:
      mode_pref = SD_SS_MODE_PREF_WCDMA;
      break;

    case SD_MODE_GW:
      mode_pref = SD_SS_MODE_PREF_GW;
      break;

    case SD_MODE_WLAN:
      mode_pref = SD_SS_MODE_PREF_WLAN;
      break;

    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_MAX:
    default:
      mode_pref = SD_SS_MODE_PREF_NONE;
      break;

  } /* switch */

  return mode_pref;
}


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_map_mode_pref_to_mode

DESCRIPTION
  Map mode preference to mode.

DEPENDENCIES
  None.

RETURN VALUE
  Band perference.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_mode_e_type    sdss_map_mode_pref_to_mode(

        sd_ss_mode_pref_e_type        mode_pref
          /* Mode pref mapped to mode.
          */
)
{

  SD_ASSERT (BETWEEN(mode_pref, SD_SS_MODE_PREF_NONE, SD_SS_MODE_PREF_MAX));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_MODE_CONTAIN( mode_pref, SD_SS_MODE_PREF_AMPS ) )
  {
    return SD_MODE_AMPS;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_MODE_CONTAIN( mode_pref, SD_SS_MODE_PREF_CDMA ) )
  {
    return SD_MODE_CDMA;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_MODE_CONTAIN( mode_pref, SD_SS_MODE_PREF_HDR ) )
  {
    return SD_MODE_HDR;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_MODE_CONTAIN( mode_pref, SD_SS_MODE_PREF_GPS ) )
  {
    return SD_MODE_GPS;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_MODE_CONTAIN( mode_pref, SD_SS_MODE_PREF_GSM ) )
  {
    return SD_MODE_GSM;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_MODE_CONTAIN( mode_pref, SD_SS_MODE_PREF_WCDMA ) )
  {
    return SD_MODE_WCDMA;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_MODE_CONTAIN( mode_pref, SD_SS_MODE_PREF_GW ) )
  {
    return SD_MODE_GW;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_MODE_CONTAIN( mode_pref, SD_SS_MODE_PREF_WLAN ) )
  {
    return SD_MODE_WLAN;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ERR ("mode_pref %d mapping to mode failed", mode_pref, 0, 0);

  return SD_MODE_MAX;

} /* sdss_map_mode_pref_to_mode */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_map_band_to_band_pref

DESCRIPTION
  Map band to band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band perference.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_band_pref_e_type    sdss_map_band_to_band_pref(

        sd_band_e_type            band
          /* Band mapped to band pref.
          */
)
{
  sd_ss_band_pref_e_type band_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Map the specified band to its corresponding band preferences.
  */

  switch( band ){

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- -*/

    case SD_BAND_BC0:
#ifdef  FEATURE_IS683A_450M_SUPPORT 
      if( sdss_is_flag_set(SD_SS_MAIN, SDSS_FLAG_IS683A_450M_SUPPORT ))  
      {
        band_pref = SD_SS_BAND_PREF_BC5; 
      }
      else 
#endif
      band_pref = SD_SS_BAND_PREF_BC0;
      break;

    case SD_BAND_BC1:
#ifdef  FEATURE_IS683A_450M_SUPPORT 
      if( sdss_is_flag_set(SD_SS_MAIN, SDSS_FLAG_IS683A_450M_SUPPORT ))  
      {
        band_pref = SD_SS_BAND_PREF_BC5; 
      }
      else
#endif
      band_pref = SD_SS_BAND_PREF_BC1;
      break;

    case SD_BAND_BC3:
      band_pref = SD_SS_BAND_PREF_BC3;
      break;

    case SD_BAND_BC4:
      band_pref = SD_SS_BAND_PREF_BC4;
      break;

    case SD_BAND_BC5:
      band_pref = SD_SS_BAND_PREF_BC5;
      break;

    case SD_BAND_BC6:
      band_pref = SD_SS_BAND_PREF_BC6;
      break;

    case SD_BAND_BC7:
      band_pref = SD_SS_BAND_PREF_BC7;
      break;

    case SD_BAND_BC8:
      band_pref = SD_SS_BAND_PREF_BC8;
      break;

    case SD_BAND_BC9:
      band_pref = SD_SS_BAND_PREF_BC9;
      break;

    case SD_BAND_BC10:
      band_pref = SD_SS_BAND_PREF_BC10;
      break;

    case SD_BAND_BC11:
      band_pref = SD_SS_BAND_PREF_BC11;
      break;

    case SD_BAND_BC12:
      band_pref = SD_SS_BAND_PREF_BC12;
      break;

    case SD_BAND_BC14:
      band_pref = SD_SS_BAND_PREF_BC14;
      break;

    case SD_BAND_BC15:
      band_pref = SD_SS_BAND_PREF_BC15;
      break;

    case SD_BAND_BC16:
      band_pref = SD_SS_BAND_PREF_BC16;
      break;

    case SD_BAND_BC17:
      band_pref = SD_SS_BAND_PREF_BC17;
      break;

    case SD_BAND_BC18:
      band_pref = SD_SS_BAND_PREF_BC18;
      break;

    case SD_BAND_BC19:
      band_pref = SD_SS_BAND_PREF_BC19;
      break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- -*/

    case SD_BAND_GSM_DCS_1800:
      band_pref = SD_SS_BAND_PREF_GSM_DCS_1800;
      break;

    case SD_BAND_GSM_EGSM_900:
      band_pref = SD_SS_BAND_PREF_GSM_EGSM_900;
      break;

    case SD_BAND_GSM_PGSM_900:
      band_pref = SD_SS_BAND_PREF_GSM_PGSM_900;
      break;

   case SD_BAND_GSM_450:
      band_pref = SD_SS_BAND_PREF_GSM_450;
      break;

    case SD_BAND_GSM_480:
      band_pref = SD_SS_BAND_PREF_GSM_480;
      break;

    case SD_BAND_GSM_RGSM_900:
      band_pref = SD_SS_BAND_PREF_GSM_RGSM_900;
      break;

    case SD_BAND_GSM_PCS_1900:
      band_pref = SD_SS_BAND_PREF_GSM_PCS_1900;
      break;

    case SD_BAND_GSM_850:
      band_pref = SD_SS_BAND_PREF_GSM_850;
      break;

    case SD_BAND_GSM_750:
      band_pref = SD_SS_BAND_PREF_GSM_750;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- -*/

    case SD_BAND_WCDMA_I_IMT_2000:
      band_pref = SD_SS_BAND_PREF_WCDMA_I_IMT_2000;
      break;

    case SD_BAND_WCDMA_II_PCS_1900:
      band_pref = SD_SS_BAND_PREF_WCDMA_II_PCS_1900;
      break;

    case SD_BAND_WCDMA_III_1700:
      band_pref = SD_SS_BAND_PREF_WCDMA_III_1700;
      break;

    case SD_BAND_WCDMA_IV_1700:
      band_pref = SD_SS_BAND_PREF_WCDMA_IV_1700;
      break;

    case SD_BAND_WCDMA_V_850:
      band_pref = SD_SS_BAND_PREF_WCDMA_V_850;
      break;

    case SD_BAND_WCDMA_VI_800:
      band_pref = SD_SS_BAND_PREF_WCDMA_VI_800;
      break;

    case SD_BAND_WCDMA_VII_2600:
      band_pref = SD_SS_BAND_PREF_WCDMA_VII_2600;
      break;

    case SD_BAND_WCDMA_VIII_900:
      band_pref = SD_SS_BAND_PREF_WCDMA_VIII_900;
      break;

    case SD_BAND_WCDMA_IX_1700:
      band_pref = SD_SS_BAND_PREF_WCDMA_IX_1700;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- -*/

    case SD_BAND_WLAN_US_2400:
      band_pref = SD_SS_BAND_PREF_WLAN_US_2400;
      break;

    case SD_BAND_WLAN_JAPAN_2400:
      band_pref = SD_SS_BAND_PREF_WLAN_JAPAN_2400;
      break;

    case SD_BAND_WLAN_EUROPE_2400:
      band_pref = SD_SS_BAND_PREF_WLAN_EUROPE_2400;
      break;

    case SD_BAND_WLAN_FRANCE_2400:
      band_pref = SD_SS_BAND_PREF_WLAN_FRANCE_2400;
      break;

    case SD_BAND_WLAN_SPAIN_2400:
      band_pref = SD_SS_BAND_PREF_WLAN_SPAIN_2400;
      break;

    case SD_BAND_WLAN_US_5000:
      band_pref = SD_SS_BAND_PREF_WLAN_US_5000;
      break;

    case SD_BAND_WLAN_JAPAN_5000:
      band_pref = SD_SS_BAND_PREF_WLAN_JAPAN_5000;
      break;

    case SD_BAND_WLAN_EUROPE_5000:
      band_pref = SD_SS_BAND_PREF_WLAN_EUROPE_5000;
      break;

    case SD_BAND_WLAN_FRANCE_5000:
      band_pref = SD_SS_BAND_PREF_WLAN_FRANCE_5000;
      break;

    case SD_BAND_WLAN_SPAIN_5000:
      band_pref = SD_SS_BAND_PREF_WLAN_SPAIN_5000;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- -*/

    case SD_BAND_NONE:
    case SD_BAND_MAX:
    default:
      band_pref = SD_SS_BAND_PREF_NONE;
      break;

  } /* switch */

  return band_pref;
} /* sdss_map_band_to_band_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_map_band_pref_to_band

DESCRIPTION
  Map band to band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_band_e_type            sdss_map_band_pref_to_band(

        sd_ss_band_pref_e_type    band_pref
          /* Band mapped to band pref.
          */
)
{
  static const sd_sband_e_type band_pref_bit_tbl[] =
  {
    SD_SBAND_BC1,
    SD_SBAND_BC3,
    SD_SBAND_BC4,
    SD_SBAND_BC5,
    SD_SBAND_BC6,
    SD_SBAND_BC7,
    SD_SBAND_BC10,
    SD_SBAND_BC11,
    SD_SBAND_BC12,
    SD_SBAND_BC14,
    SD_SBAND_BC15,
    SD_SBAND_BC16,
    SD_SBAND_BC17,
    SD_SBAND_BC18,
    SD_SBAND_BC19,
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    SD_SBAND_GSM_450,
    SD_SBAND_GSM_480,
    SD_SBAND_GSM_DCS_1800,
    SD_SBAND_GSM_EGSM_900,
    SD_SBAND_GSM_PGSM_900,
    SD_SBAND_GSM_RGSM_900,
    SD_SBAND_GSM_PCS_1900,
    SD_SBAND_GSM_850,
    SD_SBAND_GSM_750,

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    SD_SBAND_WCDMA_I_IMT_2000,
    SD_SBAND_WCDMA_II_PCS_1900,
    SD_SBAND_WCDMA_III_1700,
    SD_SBAND_WCDMA_IV_1700,
    SD_SBAND_WCDMA_V_850,
    SD_SBAND_WCDMA_VI_800,
    SD_SBAND_WCDMA_VII_2600,
    SD_SBAND_WCDMA_VIII_900,
    SD_SBAND_WCDMA_IX_1700,

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    SD_SBAND_WLAN_US_2400,
    SD_SBAND_WLAN_JAPAN_2400,
    SD_SBAND_WLAN_EUROPE_2400,
    SD_SBAND_WLAN_FRANCE_2400,
    SD_SBAND_WLAN_SPAIN_2400,

    SD_SBAND_WLAN_US_5000,
    SD_SBAND_WLAN_JAPAN_5000,
    SD_SBAND_WLAN_EUROPE_5000,
    SD_SBAND_WLAN_FRANCE_5000,
    SD_SBAND_WLAN_SPAIN_5000,
  };

  static const sd_band_e_type band_val[] =
  {
    SD_BAND_BC1,
    SD_BAND_BC3,
    SD_BAND_BC4,
    SD_BAND_BC5,
    SD_BAND_BC6,
    SD_BAND_BC7,
    SD_BAND_BC10,
    SD_BAND_BC11,
    SD_BAND_BC12,
    SD_BAND_BC14,
    SD_BAND_BC15,
    SD_BAND_BC16,
    SD_BAND_BC17,
    SD_BAND_BC18,
    SD_BAND_BC19,

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    SD_BAND_GSM_450,
    SD_BAND_GSM_480,
    SD_BAND_GSM_DCS_1800,
    SD_BAND_GSM_EGSM_900,
    SD_BAND_GSM_PGSM_900,
    SD_BAND_GSM_RGSM_900,
    SD_BAND_GSM_PCS_1900,
    SD_BAND_GSM_850,
    SD_BAND_GSM_750,

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    SD_BAND_WCDMA_I_IMT_2000,
    SD_BAND_WCDMA_II_PCS_1900,
    SD_BAND_WCDMA_III_1700,
    SD_BAND_WCDMA_IV_1700,
    SD_BAND_WCDMA_V_850,
    SD_BAND_WCDMA_VI_800,
    SD_BAND_WCDMA_VII_2600,
    SD_BAND_WCDMA_VIII_900,
    SD_BAND_WCDMA_IX_1700,

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    SD_BAND_WLAN_US_2400,
    SD_BAND_WLAN_JAPAN_2400,
    SD_BAND_WLAN_EUROPE_2400,
    SD_BAND_WLAN_FRANCE_2400,
    SD_BAND_WLAN_SPAIN_2400,

    SD_BAND_WLAN_US_5000,
    SD_BAND_WLAN_JAPAN_5000,
    SD_BAND_WLAN_EUROPE_5000,
    SD_BAND_WLAN_FRANCE_5000,
    SD_BAND_WLAN_SPAIN_5000,
  };

  word i;

  SD_ASSERT( ARR_SIZE(band_pref_bit_tbl) == ARR_SIZE(band_val) );

  /* Map the specified band preferences to its corresponding band.
  */
  if( SD_BAND_CONTAIN( band_pref, SD_SS_BAND_PREF_BC0 ) )
    return SD_BAND_BC0;

  for(i=0; i<ARR_SIZE(band_pref_bit_tbl); i++)
  {
    if( SD_BAND_CONTAIN( band_pref, SYS_BM_64BIT(band_pref_bit_tbl[i]) ) )
    {
      return band_val[i];
    }
  }

  return SD_BAND_MAX;

} /* sdss_map_band_to_band_pref */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_common_bss_type

DESCRIPTION
  Find the intersection of two BSS types.

DEPENDENCIES
  None.

RETURN VALUE
  BSS Type.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sys_wlan_bss_e_type       sdss_get_common_bss_type(

        sys_wlan_bss_e_type       bss_type1,
          /* BSS type 1
          */

        sys_wlan_bss_e_type       bss_type2
          /* BSS type 1
          */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( bss_type1, SYS_WLAN_BSS_TYPE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( bss_type2, SYS_WLAN_BSS_TYPE_MAX );

  switch( bss_type1 )
  {
    case SYS_WLAN_BSS_TYPE_ANY:
      return bss_type2;
    case SYS_WLAN_BSS_TYPE_ADHOC:
    case SYS_WLAN_BSS_TYPE_ADHOC_START:
      if( bss_type2 == SYS_WLAN_BSS_TYPE_ADHOC ||
          bss_type2 == SYS_WLAN_BSS_TYPE_ANY )
      {
        return bss_type1;
      }
      else
      {
        return SYS_WLAN_BSS_TYPE_NONE;
      }
    case SYS_WLAN_BSS_TYPE_INFRA:
      if( bss_type2 == SYS_WLAN_BSS_TYPE_INFRA ||
          bss_type2 == SYS_WLAN_BSS_TYPE_ANY )
      {
        return SYS_WLAN_BSS_TYPE_INFRA;
      }
      else
      {
        return SYS_WLAN_BSS_TYPE_NONE;
      }
    case SYS_WLAN_BSS_TYPE_NONE:
    case SYS_WLAN_BSS_TYPE_MAX:
    default:
      SD_ERR("Invalid BSS type %d, %d!", bss_type1, bss_type2, 0);
      return SYS_WLAN_BSS_TYPE_NONE;
  }

} /* sdss_get_common_bss_type() */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_supp_sys

DESCRIPTION
  Check whether a specified system (i.e., a combination of mode, band and
  CDMA channel/AMPS system is supported by the current target.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified system is supported by the current target. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdss_is_supp_sys(

        sd_mode_e_type            mode,
            /* Mode which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* Band which to check for support.
            */

        sd_chan_type              chan
            /* Channel which to check for support.
            */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the specified mode and band combination is NOT supported by the
  ** current target, return FALSE.
  */
  if( ! sdss_is_supp_mode_band(mode, band_pref) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the specified channel is invalid, return FALSE.
  */
  if( sdprl_map_chan_to_blksys(mode,
    sdss_map_band_pref_to_band(band_pref), chan) == SD_BLKSYS_MAX )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the specified mode, band and channel combination is
  ** supported - return TRUE.
  */
  return TRUE;

} /* sdss_is_supp_sys */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_group_print

DESCRIPTION
  Print an SS-Event group to which the input SS-Event belongs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                  sdss_event_group_print(

        sdss_evt_e_type       event
            /* SS-Event for which to print the event group.
            */
)
{
  sdss_evt_grp_e_type   event_group;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  event_group = sdss_map_event_to_group( event );
  switch( event_group )
  {
    case SDSS_EVT_GRP_USER:
      SD_MSG_HIGH( ">>>> Proc user event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_MISC:
      SD_MSG_HIGH( ">>>> Proc misc event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_CDMA_ACQ:
      SD_MSG_HIGH( ">>>> Proc CDMA acq event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_CDMA_OPR:
      SD_MSG_HIGH( ">>>> Proc CDMA opr event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_AMPS_ACQ:
      SD_MSG_HIGH( ">>>> Proc AMPS acq event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_AMPS_OPR:
      SD_MSG_HIGH( ">>>> Proc AMPS opr event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_HDR_USER:
      SD_MSG_HIGH( ">>>> Proc HDR user event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_HDR_ACQ:
      SD_MSG_HIGH( ">>>> Proc HDR acq event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_HDR_OPR:
      SD_MSG_HIGH( ">>>> Proc HDR opr event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_GW_ACQ:
      SD_MSG_HIGH( ">>>> Proc GW acq event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_GW_OPR:
      SD_MSG_HIGH( ">>>> Proc GW opr event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_INT:
      SD_MSG_HIGH( ">>>> Proc INT event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_WLAN_USER:
      SD_MSG_HIGH( ">>>> Proc WLAN user event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_WLAN_ACQ:
      SD_MSG_HIGH( ">>>> Proc WLAN acq event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_WLAN_OPR:
      SD_MSG_HIGH( ">>>> Proc WLAN opr event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_HYBR_1_USER:
      SD_MSG_HIGH( ">>>> Proc HYBR_1 user event=%d >>>>",event,0,0 );
      break;

    case SDSS_EVT_GRP_CALLED:
    case SDSS_EVT_GRP_MAX:
    default:
      SD_CASE_ERR( "event_group",0,0,0 );
      break;

  } /* switch */

} /* sdss_event_group_print */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_act_print

DESCRIPTION
  Print an SS-Action.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                        sdss_act_print(

        const sd_ss_act_s_type      *act_ptr
            /* Pointer to an SS-Action which to print.
            */
)
{
  SD_ASSERT( act_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( act_ptr->act )
  {
    case SD_SS_ACT_CONTINUE:
      SD_MSG_HIGH( "CONTINUE",0,0,0 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_CDMA:
      SD_MSG_HIGH( "ACQ_CDMA, band=%d, chan=%d, blksys=%d",
                   act_ptr->prm.acq_cdma.band,
                   act_ptr->prm.acq_cdma.chan,
                   act_ptr->prm.acq_cdma.blksys );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_AMPS:
      SD_MSG_HIGH( "ACQ_AMPS, band=%d, sys=%d, delay=%d",
                   act_ptr->prm.acq_amps.band,
                   act_ptr->prm.acq_amps.sys,
                   act_ptr->prm.acq_amps.max_redirect_delay );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_HDR:
      SD_MSG_HIGH( "ACQ_HDR, band=%d, chan=%d acq_mode=%d",
                   act_ptr->prm.acq_hdr.band,
                   act_ptr->prm.acq_hdr.chan,
                   act_ptr->prm.acq_hdr.acq_mode );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_GW:
    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
    #endif /* FEATURE_GSM || FEATURE_WCDMA */

      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_GET_NET_GW:
    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
    #endif /* FEATURE_GSM || FEATURE_WCDMA */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_PWR_SCAN_GW:
    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
    #endif /* FEATURE_GSM || FEATURE_WCDMA */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_GPS:
      SD_MSG_HIGH( "ACQ_GPS",0,0,0 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_MEAS_DED:
      SD_MSG_HIGH( "MEAS_DED, type=%d, ID=%d",
                   act_ptr->prm.meas.type,
                   act_ptr->prm.meas.id,
                   0 );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_MEAS_BACK:
      SD_MSG_HIGH( "MEAS_BACK, type=%d, ID=%d",
                   act_ptr->prm.meas.type,
                   act_ptr->prm.meas.id,
                   0 );
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_WLAN:
      SD_MSG_HIGH( "ACQ_WLAN, band=%d, chan=%d scan_type=%d",
                   act_ptr->prm.acq_wlan.band,
                   act_ptr->prm.acq_wlan.chan,
                   act_ptr->prm.acq_wlan.scan_type
                 );
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_SURVEY_WLAN:
      SD_MSG_HIGH( "SURVEY_WLAN, band=0x%llx, scan_type=%d",
                   act_ptr->prm.survey_wlan.band,
                   act_ptr->prm.survey_wlan.scan_type,
                   0
                 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_PWR_SAVE:
      SD_MSG_HIGH( "PWR_SAVE",0,0,0 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACCEPT:
      SD_MSG_HIGH( "ACCEPT",0,0,0 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_REJECT:
      SD_MSG_HIGH( "REJECT",0,0,0 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_RELEASE:
      SD_MSG_HIGH( "RELEASE",0,0,0 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
    case SD_SS_ACT_MAX:
      SD_CASE_ERR( "SS-Action",0,0,0 );
  }

} /* sdss_act_print */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_iact_print

DESCRIPTION
  Print an SS-Internal-action.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                        sdss_iact_print(

        const sdss_iact_s_type      *iact_ptr
            /* Pointer to an SS-Action which to print.
            */
)
{
  SD_ASSERT( iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( iact_ptr->act )
  {
    case SDSS_IACT_CONTINUE:
      SD_MSG_HIGH( "CONTINUE",0,0,0 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_ACQUIRE:
      SD_MSG_HIGH( "ACQUIRE, list=%d, pos=%d",
                   iact_ptr->prm.acq.list,
                   iact_ptr->prm.acq.pos,
                   iact_ptr->prm.acq.mode );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_MEAS:
      SD_MSG_HIGH( "MEAS, list=%d, mode=%d, type=%d",
                   iact_ptr->prm.meas.list,
                   iact_ptr->prm.meas.mode,
                   iact_ptr->prm.meas.type );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_PWR_SAVE:
      SD_MSG_HIGH( "PWR_SAVE",0,0,0 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_ACCEPT:
      SD_MSG_HIGH( "ACCEPT",0,0,0 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_REJECT:
      SD_MSG_HIGH( "REJECT",0,0,0 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_RELEASE:
      SD_MSG_HIGH( "RELEASE",0,0,0 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_GET_NET_GW:
      SD_MSG_HIGH( "GET NETWORKS GW ",0,0,0 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_PWR_SCAN_GW:
      SD_MSG_HIGH( "PWR SCAN GW ",0,0,0 );
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_GET_NET_WLAN:
      SD_MSG_HIGH( "GET_NET_WLAN ",0,0,0 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDSS_IACT_MAX:
    default:
      SD_CASE_ERR( "SS-Action",0,0,0 );
  }

} /* sdss_iact_print */


/*===========================================================================

FUNCTION sdss_is_mode_comply_meas_cat

DESCRIPTION

  Return the indication whether current sys mode is part of given 
  measurement category

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean sdss_is_mode_comply_meas_cat(

        const sd_mode_e_type         mode,
            /* Pointer to a system record which to check for mode preference
            ** compliance.
            */

        const sdsr_cat_e_type        meas_cat
            /* Measurement category.
            */
)
{
  boolean return_val = FALSE;

  switch (meas_cat) 
  {
    case SDSR_CAT_NON_AMPS:    /* Non AMPS systems */
      if(mode != SYS_SYS_MODE_AMPS)
      {
        return_val = TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_CAT_NON_CDMA:    /* Non CDMA systems */
      if(mode != SYS_SYS_MODE_CDMA)
      {
        return_val = TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_CAT_NON_HDR:     /* Non HDR systems */
      if(mode != SYS_SYS_MODE_HDR)
      {
        return_val = TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_CAT_NON_GW:      /* Non GW systems */
      if(mode != SYS_SYS_MODE_GSM && 
         mode != SYS_SYS_MODE_WCDMA &&
         mode != SYS_SYS_MODE_GW)
      {
        return_val = TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_CAT_GW_SYS:      /* GW system */
      if(mode == SYS_SYS_MODE_GSM || 
         mode == SYS_SYS_MODE_WCDMA ||
         mode == SYS_SYS_MODE_GW)
      {
        return_val = TRUE;
      }
      break; 

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_CAT_HDR:         /* HDR systems*/
      if(mode == SYS_SYS_MODE_HDR)
      {
        return_val = TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_CAT_NON_WLAN:
      if(mode != SYS_SYS_MODE_WLAN)
      {
        return_val = TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_CAT_ALL:
      return_val = TRUE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_CAT_NON_CELL:
    case SDSR_CAT_NON_COMPLY:
    case SDSR_CAT_MAX:
    default:
      SD_MSG_HIGH( "Meas_cat %d invalid for this function", meas_cat,0,0 );
      return_val = FALSE;
  }

  return return_val;
}

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
=========================== SYSTEM RECORD SUPPORT ===========================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdss_sr_is_sid_match_exp_sid

DESCRIPTION
  Check whether the SID/NID that was obtained during the acquisition matches
  the expected SID/NID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if SID/NID that was obtained during the acquisition matches the
  expected SID/NID. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdss_sr_is_sid_match_exp_sid(

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record for which to check the expected
            ** SID/NID.
            */
)
{

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For an HDR system check whether the system record matches its expected
  ** PRL's system table index.
  */
  if( sr_ptr->sys.mode == SD_MODE_HDR )
  {
    if( sdsr_is_same_is856_sid(sr_ptr, sr_ptr->sid_expected.prm.is856.sid,
                           sr_ptr->sid_expected.prm.is856.subnet_mask_len) )
    {
      return TRUE;
    }

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For GSM/WCDMA systems do the following.
  */
  if( sr_ptr->sys.mode == SD_MODE_GSM   ||
      sr_ptr->sys.mode == SD_MODE_WCDMA ||
      sr_ptr->sys.mode == SD_MODE_GW )
  {

    switch ( sr_ptr->sid_expected.net_select_mode )
    {
      case SD_NET_SELECT_MODE_LIMITED:
      case SD_NET_SELECT_MODE_AUTO:
      case SD_NET_SELECT_MODE_HPLMN:
        return TRUE;

      case SD_NET_SELECT_MODE_PRL_SYS:
      case SD_NET_SELECT_MODE_MANUAL_SYS:
      case SD_NET_SELECT_MODE_PRIV_SYS:
        if ( sys_plmn_id_is_undefined( sr_ptr->sid_expected.prm.plmn.plmn_id ))
        {
          /* Expected SID is wildcard.
          */
          return TRUE;
        }

        /* Expected SID is not wildcard, compare the PLMNs.
        */
        return sys_plmn_match( sr_ptr->sid.plmn.plmn_id,
                               sr_ptr->sid_expected.prm.plmn.plmn_id
                         );

      case SD_NET_SELECT_MODE_MANUAL_BSSID:
      case SD_NET_SELECT_MODE_MAX:
      default:
        return TRUE;

    } /* switch () */

  } /* if ( sr_ptr->sys.mode == SD_MODE_GSM .. */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For WLAN systems do the following.
    */
  if( sr_ptr->sys.mode == SD_MODE_WLAN )
  {

    switch ( sr_ptr->sid_expected.net_select_mode )
    {
      case SD_NET_SELECT_MODE_LIMITED:
      case SD_NET_SELECT_MODE_AUTO:
      case SD_NET_SELECT_MODE_HPLMN:
        return TRUE;

      case SD_NET_SELECT_MODE_PRL_SYS:
      case SD_NET_SELECT_MODE_MANUAL_SYS:
      case SD_NET_SELECT_MODE_PRIV_SYS:
        return sys_is_wlan_ssid_match( &sr_ptr->sid_expected.prm.wlan.ssid,
                                       &sr_ptr->sid_expected.prm.wlan.ssid
                                     );

      case SD_NET_SELECT_MODE_MANUAL_BSSID:
      case SD_NET_SELECT_MODE_MAX:
      default:
        return TRUE;

    } /* switch () */

  } /* if ( sr_ptr->sys.mode == SD_MODE_WLAN .. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If expected SID is equal to SD_WILDCARD_SID or to obtained SID, and
  ** expected NID is equal to SD_WILDCARD_NID or to obtained NID, return
  ** TRUE. Else, return FALSE.
  */
  if( ((sr_ptr->sid_expected.prm.is95.sid == SD_WILDCARD_SID) ||
       (sr_ptr->sid.is95.sid == sr_ptr->sid_expected.prm.is95.sid))
                        &&
      ((sr_ptr->sid_expected.prm.is95.nid == SD_WILDCARD_NID) ||
      (sr_ptr->sid.is95.nid  == sr_ptr->sid_expected.prm.is95.nid)) )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_sr_is_sid_match_exp_sid */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_avoid

DESCRIPTION
  Check whether a specified system is to be avoided from acquisition attempts
  at the current time.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified system is to be avoided from acquisition attempts
  at or before the current time. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdss_sr_is_avoid(

        sd_ss_e_type              ss,
            /* Current system selection item type.
            **
            */
        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record for which to check the avoid
            ** from acquisition attempts status.
            */
)
{

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If  the current ss is different from the ss of the avoid.
  ** return FALSE.
  */
  if( sr_ptr->sched_ss != ss )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system is marked avoid from acquisition attempts and the avoid
  ** period did not expired yet, return TRUE. Else, return FALSE.
  */
  if( SDSSSCR_SCHED_IS_AVOID( sr_ptr->sched ) &&
      clk_uptime() < sr_ptr->sched_prm.avoid_time )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_sr_is_avoid */

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_mode_pref

DESCRIPTION
  Check whether a specified system complies with a specified mode preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if system complies with the specified mode preference. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdss_sr_is_mode_pref(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record which to check for mode preference
            ** compliance.
            */

        sd_ss_mode_pref_e_type    mode_pref
            /* Mode preference against which to check the system.
            */
)
{
  sd_ss_mode_pref_e_type    mode_mask;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sr_ptr->sys.mode == SD_MODE_GW )
  {
    mode_mask = SD_SS_MODE_PREF_GW;
  }
  else
  {
    mode_mask = (sd_ss_mode_pref_e_type)BM( sr_ptr->sys.mode );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_GET_COMMON_MODE(mode_pref, mode_mask ) )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_sr_is_mode_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_band_pref

DESCRIPTION
  Check whether a specified system complies with a specified band preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if system complies with the specified band preference. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdss_sr_is_band_pref(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record which to check for mode preference
            ** compliance.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference against which to check the system.
            */
)
{

   SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Band not valid for GPS.
  */
  if ( sr_ptr->sys.mode  == SD_MODE_GPS )
  {
    /*
    */
    return TRUE;
  }

  if( SD_BAND_CONTAIN( band_pref, sr_ptr->sys.band ) )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_sr_is_band_pref */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_bss_type_pref

DESCRIPTION
  Check whether a specified system complies with a specified bss type
  preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if system complies with the specified bss type preference. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdss_sr_is_bss_type_pref(

        /*lint -esym(715, sr_ptr) */ /* not accessed */
        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record which to check for mode preference
            ** compliance.
            */

        /*lint -esym(715, bss_type_pref) */ /* not accessed */
        sys_wlan_bss_e_type       bss_type_pref
            /* Bss type preference against which to check the system.
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( bss_type_pref, SYS_WLAN_BSS_TYPE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN_ADHOC
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* sdss_sr_is_bss_type_pref */ /*lint +esym(715, bss_type_pref) */
/*lint +esym(715, sr_ptr) */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_pref_conf0

DESCRIPTION
  Check whether a specified system conflicts with the current system
  selection preference (i.e., mode, band and roam preference).

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if system conflicts with the current system selection preference (as
  per the pref_conf category parameter). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdss_sr_is_pref_conf0(

        const sdss_s_type         *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record for which to check SS-Preference
            ** conflict.
            */

        sdss_pref_conf_e_type     pref_conf
            /* SS-Preference conflict category to be evaluated.
            */
)
{
  boolean             is_pref_conflict  = FALSE;
  sd_ss_mode_pref_e_type      mode_pref = SD_SS_MODE_PREF_NONE;
             
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( pref_conf, SDSS_PREF_CONF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If requested, check for mode preference conflict.
  */
  if( SDSS_PREF_CONTAIN( pref_conf, SDSS_PREF_CONF_MODE ) )
  {

    if (sdsr_list_find( SDSR_GW_LTD_LST, sr_ptr ) == SDSR_POS_NULL )
    {
      mode_pref = ss_ptr->mode_pref;
    }
    else
    {
      /* System record is part of SDSR_GW_LTD_LST that means SIM status
      ** is not available .So take true_mode_pref into consideration.
      */
       mode_pref = ss_ptr->true_mode_pref;
    }
    
    if( ! sdss_sr_is_mode_pref( sr_ptr, mode_pref ) )
    {
      SD_MSG_HIGH("Sr conflicts mode pref=%d, mode=%d",
                  ss_ptr->mode_pref, sr_ptr->sys.mode, 0 );
      is_pref_conflict = TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If requested, check for band preference conflict.
  */
  if( SDSS_PREF_CONTAIN( pref_conf, SDSS_PREF_CONF_BAND ) )
  {
    if( ! sdss_sr_is_band_pref(sr_ptr, ss_ptr->band_pref) )
    {

      SD_MSG_HIGH("Sr conflicts band pref=0x%lx, band=%d",
                  ss_ptr->band_pref, sr_ptr->sys.band, 0 );  /*lint !e559 */
      is_pref_conflict = TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If requested, check for roam preference conflict.
  ** But if the mode of acquired system is GSM/WCDMA/WLAN, ignore the roam
  ** preference.
  */
  if( SDSS_PREF_CONTAIN( pref_conf, SDSS_PREF_CONF_ROAM ) && 
    ! SD_IS_BIT_CONTAIN(sdss_map_mode_to_mode_pref(sr_ptr->sys.mode),
                            SD_SS_MODE_PREF_GW) && 
    ! SD_IS_BIT_CONTAIN(sdss_map_mode_to_mode_pref(sr_ptr->sys.mode),
                            SD_SS_MODE_PREF_WLAN))
  {
    switch (ss_ptr->roam_pref)
    {
      case SD_SS_ROAM_PREF_ANY:
        break;

      case SD_SS_ROAM_PREF_ROAM_ONLY:
        if (sdprl_roam_ind_map_custom_home_to_home(sr_ptr->roam) ==
          SYS_ROAM_STATUS_OFF)
        {
          SD_MSG_HIGH("Sr conflicts roam pref=%d, roam=%d",
                  ss_ptr->roam_pref, sr_ptr->roam, 0 );
          is_pref_conflict = TRUE;
        }
        break;

      case SD_SS_ROAM_PREF_HOME:
      case SD_SS_ROAM_PREF_AFFIL:
        if (! SD_IS_BIT_CONTAIN( ss_ptr->roam_pref,
            BM(sdprl_roam_ind_map_custom_home_to_home(sr_ptr->roam))))
        {
          SD_MSG_HIGH("Sr conflicts roam pref=%d, roam=%d",
                ss_ptr->roam_pref, sr_ptr->roam, 0 );
          is_pref_conflict = TRUE;
        }
        break;

      case SD_SS_ROAM_PREF_MAX:
      default:
        SD_MSG_ERROR ("Invalid roam pref",0,0,0);
        break;  
    }
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If requested, check for prl preference conflict.
  */
  if( SDSS_PREF_CONTAIN( pref_conf, SDSS_PREF_CONF_PRL )  )
  {
    sd_blksys_e_type     blksys;
    blksys = sdprl_map_chan_to_blksys( sr_ptr->sys.mode,
                                       sdss_map_band_pref_to_band(sr_ptr->sys.band),
                                       sr_ptr->sys.chan
                                     );
    switch ( ss_ptr->prl_pref )
    {
      case SD_SS_PRL_PREF_AVAIL_BC0_A:
        if( blksys != SD_BLKSYS_CELL_A )
        {
          SD_MSG_HIGH("Sr conflicts prl pref=%d, blksys=%d",
                      ss_ptr->prl_pref, blksys, 0 );
          is_pref_conflict = TRUE;
        }
        break;

      case SD_SS_PRL_PREF_AVAIL_BC0_B:
        if( blksys != SD_BLKSYS_CELL_B )
        {
          SD_MSG_HIGH("Sr conflicts prl pref=%d, blksys=%d",
                      ss_ptr->prl_pref, blksys, 0 );
          is_pref_conflict = TRUE;
        }
        break;

      case SD_SS_PRL_PREF_NONE:
      case SD_SS_PRL_PREF_ANY:
      case SD_SS_PRL_PREF_MAX:
      default:
        break;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If requested, check for BSS type preference conflict.
  ** But if the mode of acquired system is not WLAN, ignore the BSS type
  ** preference.
  */
  #ifdef FEATURE_WLAN_ADHOC
#error code not present
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the pref_conf category is CONF_NONE negate the return value;
  */
  if( pref_conf == SDSS_PREF_CONF_NONE )
  {
    is_pref_conflict = !is_pref_conflict;
  }

  return BOOLEAN( is_pref_conflict );

} /* sdss_sr_is_pref_conf0 */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_pref_conf

DESCRIPTION
  Check whether a specified system conflicts with the current system
  selection preference (i.e., mode, band and roam preference).

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if system conflicts with the current system selection preference (as
  per the pref_conf category parameter). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdss_sr_is_pref_conf(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record for which to check SS-Preference
            ** conflict.
            */

        sdss_pref_conf_e_type     pref_conf
            /* SS-Preference conflict category to be evaluated.
            */
)
{
  sdss_s_type   *ss_ptr = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( pref_conf, SDSS_PREF_CONF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sdss_sr_is_pref_conf0( ss_ptr, sr_ptr, pref_conf );

} /* sdss_sr_is_pref_conf */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_prl

DESCRIPTION
  Check whether a system complies with a specific PRL designation value.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system complies with the PRL specified designation value.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdss_sr_is_prl(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record for which to check PRL designation.
            */

        sdss_prl_val_e_type       prl_desig_val
            /* PRL designation value against which to checked.
            */
)
{

  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( prl_desig_val, SDSS_PRL_VAL_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system complies with the PRL designation category, return TRUE.
  ** Else, return FALSE.
  */
  if( SD_IS_BIT_CONTAIN(prl_desig_val,BM(sr_ptr->prl)) )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH( "Sr conflicts prl desig=%d, prl=%d",
               prl_desig_val, sr_ptr->prl, 0 );

  return FALSE;

} /* sdss_sr_is_prl */




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
======================== SYSTEM RECORD LIST SUPPORT =========================
=============================================================================
=============================================================================
===========================================================================*/


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_map

DESCRIPTION
  Map list to the appropriate list according to the input system selection
  information structur.

DEPENDENCIES
  None.

RETURN VALUE
  Mapped system record list.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdsr_e_type             sdss_sr_list_map(

        const sdss_s_type       *ss_ptr,
          /* Pointer to system selection information structure.
          */

        sdsr_e_type             list
            /* List for which to check the mode designation.
            */
)
{
  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For MAIN system selection, there is no need to map list.
  */
  if( ss_ptr == sdss_ptr(SD_SS_MAIN) )
  {
    return list;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH

  if ( ss_ptr == sdss_ptr(SD_SS_HYBR_1) )
  {

    switch( list ) /*lint -e787 -e788 *//* enum not used */
    {
      case SDSR_AVAIL_LST:          return SDSR_HYBR_1_AVAIL_LST;
      case SDSR_ACQ_GEO_LST:        return SDSR_HYBR_1_ACQ_GEO_LST;
      case SDSR_IDL_GEO_LST:        return SDSR_HYBR_1_IDL_GEO_LST;
      case SDSR_SRV_GEO_LST:        return SDSR_HYBR_1_SRV_GEO_LST;
      case SDSR_ACQ_PREF_LST:       return SDSR_HYBR_1_ACQ_PREF_LST;
      case SDSR_IDL_PREF_LST:       return SDSR_HYBR_1_IDL_PREF_LST;
      case SDSR_SRV_PREF_LST:       return SDSR_HYBR_1_SRV_PREF_LST;
      case SDSR_START_PREF_LST:     return SDSR_HYBR_1_START_PREF_LST;
      case SDSR_REDIR_LST:          return SDSR_HYBR_1_REDIR_LST;
      case SDSR_SRCH_LST:           return SDSR_HYBR_1_SRCH_LST;
      case SDSR_SCHED_LST:          return SDSR_HYBR_1_SCHED_LST;
      case SDSR_MEAS_LST:           return SDSR_HYBR_1_MEAS_LST;
      //case SDSR_ACQ_COLLOC_LST:     return SDSR_HDR_ACQ_COLLOC_LST;
      //case SDSR_SRV_COLLOC_LST:     return SDSR_HDR_SRV_COLLOC_LST;
      case SDSR_MRU_LST:            return SDSR_HYBR_1_MRU_LST;
      case SDSR_PWR_SAVE_LST:       return SDSR_HYBR_1_PWR_SAVE_LST;
      case SDSR_BACKUP_LST:         return SDSR_HYBR_1_BACKUP_LST;
      case SDSR_ACQ_CHAN_LST:       return SDSR_HYBR_1_ACQ_CHAN_LST;
      case SDSR_OH_CHAN_LST:        return SDSR_HYBR_1_OH_CHAN_LST;

      case SDSR_ACQ_SYS:            return SDSR_HYBR_1_ACQ_SYS;
      case SDSR_ACQ_FULL_SYS:       return SDSR_HYBR_1_ACQ_FULL_SYS;
      case SDSR_ACQ_MINI_SYS:       return SDSR_HYBR_1_ACQ_MINI_SYS;
      case SDSR_ACQ_MICRO_SYS:      return SDSR_HYBR_1_ACQ_MICRO_SYS;
      case SDSR_ACQ_FULL2_SYS:      return SDSR_HYBR_1_ACQ_FULL2_SYS;
      case SDSR_ACQ_MINI2_SYS:      return SDSR_HYBR_1_ACQ_MINI2_SYS;
      case SDSR_ACQ_DEEP_SYS:       return SDSR_HYBR_1_ACQ_DEEP_SYS;
      case SDSR_ACQ_DEEP2_SYS:      return SDSR_HYBR_1_ACQ_DEEP2_SYS;
      case SDSR_ACQ_SHALLOW_SYS:    return SDSR_HYBR_1_ACQ_SHALLOW_SYS;
      case SDSR_ACQ_SHALLOW2_SYS:   return SDSR_HYBR_1_ACQ_SHALLOW2_SYS;
      case SDSR_START_SYS:          return SDSR_HYBR_1_START_SYS;
      case SDSR_IDL_SYS:            return SDSR_HYBR_1_IDL_SYS;
      case SDSR_SRV_SYS:            return SDSR_HYBR_1_SRV_SYS;
      case SDSR_SI_SYS:             return SDSR_HYBR_1_SI_SYS;
      case SDSR_REDIR_SYS:          return SDSR_HYBR_1_REDIR_SYS;
      case SDSR_LOST_SYS:           return SDSR_HYBR_1_LOST_SYS;
      case SDSR_CAND_SYS:           return SDSR_HYBR_1_CAND_SYS;
      case SDSR_TEMP_SYS:           return SDSR_HYBR_1_TEMP_SYS;
      case SDSR_CAND_IDL_SYS:       return SDSR_HYBR_1_CAND_IDL_SYS;

      case SDSR_ACQED_LST:          return SDSR_HYBR_1_ACQED_LST;
      case SDSR_GW_LST:             return SDSR_HYBR_1_GW_LST;
      case SDSR_MANUAL_LST:         return SDSR_HYBR_1_MANUAL_LST;
      case SDSR_GW_LTD_LST:         return SDSR_HYBR_1_GW_LTD_LST;

      default:
        break;

    } /* switch */ /*lint +e787 +e788 */
  }
  #elif defined FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return list;

} /* sdss_sr_list_map */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_map2

DESCRIPTION
  Map list to the appropriate list according to the input system selection.

DEPENDENCIES
  None.

RETURN VALUE
  Mapped system record list.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdsr_e_type             sdss_sr_list_map2(

        sd_ss_e_type            ss,
          /* System selection - MAIN or HDR.
          */

        sdsr_e_type             list
            /* List for which to check the mode designation.
            */
)
{
  return sdss_sr_list_map( sdss_ptr(ss), list );
}




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_is_mode

DESCRIPTION
  Check whether a specified mode value includes the mode designation of at
  least one of the systems that are referenced by a specified list.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified mode value includes the mode designation of at least
  one of the systems that are referenced by the specified list. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                       sdss_sr_list_is_mode(

        sdsr_e_type                   list,
            /* List for which to check the mode designation.
            */

        sd_mode_e_type                mode_val
            /* Mode value against which to compare.
            */
)
{
  int                 i;
  sdsr_s_type         *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_val, SD_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and return TRUE if the specified mode value
  ** includes the mode designation of any of the systems that are referenced
  ** by this list. Else, return FALSE.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( mode_val == sr_ptr->sys.mode )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_sr_list_is_mode */



/*===========================================================================

FUNCTION sdss_sr_list_is_mode_pref

DESCRIPTION
  Check whether a specified mode value includes the mode designation of at
  least one of the systems that are referenced by a specified list.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified mode value includes the mode designation of at least
  one of the systems that are referenced by the specified list. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_sr_list_is_mode_pref(

        sdsr_e_type                   list,
            /* List for which to check the mode designation.
            */

        sd_ss_mode_pref_e_type        mode_pref
            /* Mode value against which to compare.
            */
)
{
  int                 i;
  sdsr_s_type         *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and return TRUE if the specified mode value
  ** includes the mode designation of any of the systems that are referenced
  ** by this list. Else, return FALSE.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( sdss_sr_is_mode_pref(sr_ptr,mode_pref))
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_sr_list_is_mode */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_is_band

DESCRIPTION
  Check whether a specified band value includes the band designation of at
  least one of the systems that are referenced by a specified list.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified band value includes the band designation of at least
  one of the systems that are referenced by the  specified list. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_sr_list_is_band(

        sdsr_e_type                   list,
            /* List for which to check the band designation.
            */

        sd_band_e_type                band_val
            /* Band value against which to compare.
            */
)
{
  int                                 i;
  sdsr_s_type                         *sr_ptr;
  sd_ss_band_pref_e_type              band_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_val, SD_BAND_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and return TRUE if the specified band value
  ** includes the band designation of any of the systems that are referenced
  ** by this list. Else, return FALSE.
  */
  band_pref = sdss_map_band_to_band_pref(band_val);

  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( sr_ptr->sys.band == band_pref )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_sr_list_is_band */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_is_prl

DESCRIPTION
  Check whether a specified PRL value includes the PRL designation of at
  least one of the systems that are referenced by a specified list.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified PRL value includes the PRL designation of at least
  one of the systems that are referenced by the specified list. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_sr_list_is_prl(

        sdsr_e_type                   list,
            /* List for which to check the PRL designation.
            */

        sdss_prl_val_e_type           prl_val
            /* PRL value against which to compare.
            */
)
{
  int                 i;
  sdsr_s_type         *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl_val, SDSS_PRL_VAL_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and return TRUE if the specified PRL value
  ** includes the PRL designation of any of the systems that are referenced
  ** by this list. Else, return FALSE.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( sdss_sr_is_prl(sr_ptr, prl_val) )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_sr_list_is_prl */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_is_net_sel

DESCRIPTION
  Check whether a specified network selection mode value equals the network
  selection mode of at least one of the systems that are referenced by a
  specified list.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified network selection mode value equals the network
  selection mode of at least one of the systems that are referenced by the
  specified list. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_sr_list_is_net_sel(

        sdsr_e_type                   list,
            /* List for which to check the network selection mode.
            */

        sd_net_select_mode_e_type     net_sel
            /* Network selection mode against which to compare.
            */
)
{
  int                 i;
  sdsr_s_type         *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( net_sel, SD_NET_SELECT_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and return TRUE if the specified network selection
  ** mode equals the network selection mode of any of the systems that are
  ** referenced by this list. Else, return FALSE.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( sr_ptr->sid_expected.net_select_mode == net_sel )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_sr_list_is_net_sel */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_is_pref_conf

DESCRIPTION
  Check whether a any of the systems that are referenced by the specified
  list conflicts with the current system selection preference (i.e., mode,
  band and roam preference).

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if any of the systems that are referenced by the specified list
  conflicts (as per the pref_conf_val parameter) with the current system
  selection preference.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_sr_list_is_pref_conf(

        sd_ss_e_type                  ss,
          /* System selection - MAIN or HDR.
          */

        sdsr_e_type                   list,
            /* List for which to check the SS-Preference conflict
            */

        sdss_pref_conf_e_type         pref_conf_val
            /* SS-Prereference conflict value against which to compare.
            */
)
{
  int                 i;
  sdsr_s_type         *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( pref_conf_val, SDSS_PREF_CONF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the pref_conf category is CONF_NONE negate the return value;
  */
  if( pref_conf_val == SDSS_PREF_CONF_NONE )
  {
    /* Step through the list and return FALSE if the specified SS-Preference
    ** conflicts with any of the systems that are referenced by this list.
    ** Else, return TRUE.
    */
    for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
    {
      if( sdss_sr_is_pref_conf(ss, sr_ptr, SDSS_PREF_CONF_ANY) )
      {
        return FALSE;
      }
    }

    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and return TRUE if any of the systems that are
  ** referenced by this list conflicts with the SS-Preference as per the
  ** specified pref_conf_val. Else, return FALSE.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( sdss_sr_is_pref_conf(ss, sr_ptr, pref_conf_val) )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_sr_list_is_pref_conf */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_is_sid

DESCRIPTION
  Check whether at least one of the systems that are referenced by the
  specified system record match the input sid condition.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if at least one of the systems that are referenced by the specified
  system record list match the input sid condition. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_sr_list_is_sid(

        sdsr_e_type                   list,
            /* System record list for which to check sid condition.
            */

        sdssscr_sid_cond_e_type       sid_cond
            /* SID condition to be checked.
            */
)
{
  int                 i;
  sdsr_s_type         *sr_ptr;
  sdsr_s_type         *idl_sys_ptr = sdsr_list_get_ptr( SDSR_IDL_SYS, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( sid_cond, SDSSSCR_SID_COND_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and return TRUE if at least one of the systems
  ** match the input SID condition.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    switch( sid_cond )
    {
      case SDSSSCR_SID_COND_EXPECTED:
        if( sdss_sr_is_sid_match_exp_sid(sr_ptr) )
        {
          return TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_SID_COND_NOT_EXPECTED:
        if( !sdss_sr_is_sid_match_exp_sid(sr_ptr) )
        {
          return TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_SID_COND_COLLOC_IDL_SYS:

        if( idl_sys_ptr == NULL ||
            sdprl_is_associated(sr_ptr, idl_sys_ptr)
          )
        {
          return TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_SID_COND_MAX:
      default:
        SD_CASE_ERR( "sid_cond",0,0,0 );
        break;

    } /* switch */

  } /* for */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, SID condition did not match with any of the systems in
  ** the list - return FALSE.
  */
  return FALSE;

} /* sdss_sr_list_is_sid */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_is_cnt

DESCRIPTION
  Check whether a specified list contains CNT or more/less/exactly systems
  that comply with the cnt_inc count inclusion criteria.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified list contains CNT or more/less/exactly elements that
  are currently available for acquisition attempts. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_sr_list_is_cnt(

       sd_ss_e_type                   ss,
          /* System selection enum type.
          */

        sdsr_e_type                   list,
            /* List for which to check the CNT.
            */

        int                           cnt_val,
            /* CNT value against which to compare.
            */

        sdssscr_cnt_cond_e_type       cnt_cond,
            /* CNT condition to be used in the comparison.
            */

        sdssscr_cnt_inc_e_type        cnt_inc
            /* CNT inclusion to be used in the comparison.
            */
)
{
  int                 i;
  sdsr_s_type         *sr_ptr;
  int                 cnt           = 0;
  byte                new_acq_cnt;
  sdss_s_type         *ss_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cnt_cond, SDSSSCR_CNT_COND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cnt_inc, SDSSSCR_CNT_INC_MAX );

  ss_ptr = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  new_acq_cnt = ss_ptr->new_acq_cnt;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and count the number of systems that complies
  ** with the specified count inclusion criteria, cnt_inc.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    switch( cnt_inc )
    {
      case SDSSSCR_CNT_INC_ALL:
        cnt++;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CNT_INC_COMPLY:
        if( sdss_is_supp_mode_band(sr_ptr->sys.mode, sr_ptr->sys.band) &&
            !sdss_sr_is_pref_conf0(ss_ptr,
                                   sr_ptr,
                                   SDSS_PREF_CONF_MODE_OR_BAND) )
        {
          cnt++;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CNT_INC_AVAIL:
        if( sdss_is_supp_mode_band(sr_ptr->sys.mode, sr_ptr->sys.band) &&
            !sdss_sr_is_pref_conf0(ss_ptr,
                                   sr_ptr,
                                   SDSS_PREF_CONF_MODE_OR_BAND) &&
            !sdss_sr_is_avoid( ss, sr_ptr ))
        {
          cnt++;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CNT_INC_NEW:
        if( sdss_is_supp_mode_band(sr_ptr->sys.mode, sr_ptr->sys.band) &&
            !sdss_sr_is_pref_conf0(ss_ptr,
                                   sr_ptr,
                                   SDSS_PREF_CONF_MODE_OR_BAND) &&
            !sdss_sr_is_avoid(ss, sr_ptr) &&
            sr_ptr->new_acq_cnt != new_acq_cnt )
        {
          cnt++;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CNT_INC_MAX:
      default:
        SD_CASE_ERR( "cnt_inc",0,0,0 );
        break;

    } /* switch */

  } /* for */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If cnt is within the range that is specified by cnt_cond and cnt_val,
  ** return TRUE. Else, return FALSE.
  */
  if( (cnt_cond == SDSSSCR_CNT_COND_EXACTLY && cnt == cnt_val) ||
      (cnt_cond == SDSSSCR_CNT_COND_OR_MORE && cnt >= cnt_val) ||
      (cnt_cond == SDSSSCR_CNT_COND_OR_LESS && cnt <= cnt_val) )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_sr_list_is_cnt */


/*===========================================================================

FUNCTION sdss_sr_list_is_comply

DESCRIPTION
  Check whether a specified list contains 1 or more systems that are
  compliant with the SS preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified list contains 1 or more systems that
  are currently available for acquisition attempts.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                       sdss_sr_list_is_comply(

       sd_ss_e_type                   ss,
          /* System selection enum type.
          */

        sdsr_e_type                   list
            /* List for which to check.
            */

)
{

  return sdss_sr_list_is_cnt( ss, list, 1, SDSSSCR_CNT_COND_OR_MORE,
                              SDSSSCR_CNT_INC_COMPLY );

}




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_avoid

DESCRIPTION
  Mark the system records that are referenced by the list parameter to be
  avoided from any acquisition attempts for the number of seconds that is
  indicated by the time parameter.

  Note that marking a system to be avoided from acquisition attempts
  overwrites any reacquisition attempts that are scheduled for that system.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdss_sr_list_avoid(

        sd_ss_e_type              ss,
            /* system selection item.
            */

        sdsr_e_type               list,
            /* List to be avoided from acquisition attempts.
            */

        dword                     avoid_time,
            /* The time period in seconds during which acquisition attempts
            ** should be avoided.
            */

        sdssscr_sched_e_type      sched
            /* The avoidance schedule type.
            */

)
{
  int i;
  sdsr_s_type         *sr_ptr;
  dword               uptime      = clk_uptime();
  sdss_s_type         *ss_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( sched,SDSSSCR_SCHED_MAX );

  ss_ptr = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_MED( "Avoid list=%d, uptime=%d, avoid_time=%d",
               list, uptime, avoid_time );

  SD_MSG_MED( "Schedule=%d, ss=%d",sched, ss, 0 );

  /* Step through the list and mark systems to be avoided from acquisitions.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    /* If avoid time equal 0, reset the avoid_time which is set on the same
    ** system selection item based on the following conditions.
    **
    ** 1.If condition is SDSSSCR_SCHED_AVOID_CANCEL_ALL, cancel all avoidance
    ** of which sched is AVOID or UNTIL_ACQ_FAIL.
    **
    ** 2.Otherwise only cancel the avoidance of which sched field is specifed.
    */
    if( avoid_time == 0 )
    {
      if( sdss_sr_is_avoid( ss, sr_ptr ) &&
          ( sched == SDSSSCR_SCHED_AVOID_CANCEL_ALL ||
            sr_ptr->sched == sched ))
      {
        sr_ptr->sched_prm.avoid_time = 0;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else, if avoid time equal the magic value AVOID_UNTIL_NEW_ACQ, only
    ** set the new_acq_cnt field per the current new acquisition counter
    ** value.
    */
    else if( avoid_time == SDSSSCR_AVOID_UNTIL_NEW_ACQ )
    {
      sr_ptr->new_acq_cnt = ss_ptr->new_acq_cnt;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else, set the schedule field per the avoid time.
    */
    else
    {
      sr_ptr->sched                 = (word) sched;
      sr_ptr->sched_prm.avoid_time  = uptime + avoid_time;
      sr_ptr->sched_ss              = ss;
    }

    SD_MSG_MED( "Avoid =%d, ss=%d, mode=%d ",
                sr_ptr->sched,
                sr_ptr->sched_ss,
                sr_ptr->sys.mode);

    SD_MSG_MED( "      band= 0x%x 0x%x, chan=%d ",
                QWORD_HIGH(sr_ptr->sys.band),
                QWORD_LOW(sr_ptr->sys.band),
                sr_ptr->sys.chan);
  }

} /* sdss_sr_list_avoid */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_avoid2

DESCRIPTION
  Mark the system records that are referenced by the list parameter to be
  avoided from any acquisition attempts for the number of seconds that is
  indicated by the time parameter.

  Note that marking a system to be avoided from acquisition attempts
  overwrites any reacquisition attempts that are scheduled for that system.

  Takes enum sd_ss_e_type instead of sdss_s_type*

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdss_sr_list_avoid2(

        const sd_ss_e_type        ss,
           /* System selection - MAIN or HDR.
           */

        sdsr_e_type               list,
           /* List to be avoided from acquisition attempts.
           */

        dword                      avoid_time
           /* The time period in seconds during which acquisition attempts
           ** should be avoided.
           */
)
{
  sdss_sr_list_avoid(ss, list, avoid_time, SDSSSCR_SCHED_AVOID);
  return;

} /* sdss_sr_list_avoid2 */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_find_next

DESCRIPTION
  Given a list and list position, this function finds the next system
  (starting with the list position) that is available for acquisition
  attempt.

  A system that is available for acquisition attempt is a system that:

    1. Complies with the target capabilities.
    2. Complies with the specified mode preferences.
    3. Complies with the specified band preferences.
    4. Complies with the specified measurement value.
    5. Currently is not marked to be avoided from acquisition attempts.
    6. Has a new-acquisition count that is different than the specified
       new-acquisition count or the specified new-acquisition count is 0.

DEPENDENCIES
  None.

RETURN VALUE
  The position of the next system that is available for an acquisition
  attempt if such a system is found. SDSR_POS_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  int                       sdss_sr_list_find_next(

        sd_ss_e_type              ss,
            /* system selection item.
            */

        sdsr_e_type               list,
            /* List over which to find the next system.
            */

        int                       pos,
            /* List position from which to start searching for next system.
            */

        byte                      new_acq_cnt,
            /* The new-acquisition counter value with which system should
            ** comply (ignored if set to 0).
            */

        int2                      meas_val,
            /* Measurement value with which system should comply.
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference with which system should comply.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* Band preference with which system should comply.
            */

        /*lint -esym(715, bss_type_pref) */
        sys_wlan_bss_e_type       bss_type_pref,
            /* Bss type preference with which system should comply.
            */

        sdsr_cat_e_type              meas_cat
           /* Systems which should comply meas_val
            */
)
{

  int               i;
  sdsr_s_type       *sr_ptr, sr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( bss_type_pref, SYS_WLAN_BSS_TYPE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether acquisition needs to be attempted over AMPS 2nd strongest
  ** control channel.
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Starting from the specified list position, step through the list and
  ** return the position of the next system that is available for an
  ** acquisition attempt. If such a system is not found, return
  ** SDSR_POS_NULL.
  **
  ** A system that is available for acquisition attempt is a system that:
  **
  **   1. Complies with the target capabilities.
  **   2. Complies with the specified mode preferences.
  **   3. Filters band within the target capability, system mode, band pref
  **      and mode pref.
  **   4. Complies with the specified band preferences.
  **   5. Complies with the specified measurement value.
  **   6. Currently is not marked to be avoided from acquisition attempts.
  **   7. Has a new-acquisition count that is different than the specified
  **      new-acquisition count or the specified new-acquisition count is 0.
  */
  for( i=pos; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( ! sdss_is_supp_mode_band(sr_ptr->sys.mode, sr_ptr->sys.band) ) {
      SD_MSG_HIGH( "Skip list=%-3d, pos=%-3d - target-cap", list, i, 0 );
      continue;
    }

    if( ! sdss_sr_is_mode_pref(sr_ptr, mode_pref)  ) {
      SD_MSG_HIGH( "Skip list=%-3d, pos=%-3d - mode-pref", list, i, 0 );
      continue;
    }

    sr.sys = sr_ptr->sys;

    /* Filters the system band within the target capability.
    */
    sr.sys.band = sdss_get_supp_band_pref(sr.sys.band);

    /* Filters the system band within the system mode preference.
    */
    sr.sys.band = sdss_get_supp_mode_band_pref(
                    sdss_map_mode_to_mode_pref(sr.sys.mode),
                    sr.sys.band);

    /* Filters the system band within the mode preference.
    */
    sr.sys.band = sdss_get_supp_mode_band_pref(mode_pref,sr.sys.band);

    /* Filters the system band within the band preference.
    */
    if( ! sdss_sr_is_band_pref(&sr, band_pref)  ) {
      SD_MSG_HIGH( "Skip list=%-3d, pos=%-3d - band-pref", list, i, 0 );
    }

    #ifdef FEATURE_WLAN_ADHOC
#error code not present
    #endif

    else if( (sr_ptr->meas_val < meas_val) &&
             (sdss_is_mode_comply_meas_cat( sr_ptr->sys.mode, meas_cat)) ) {
      SD_MSG_HIGH( "Skip list=%-3d, pos=%-3d - meas_val", list, i, 0 );
    }

    else if( sdss_sr_is_avoid(ss, sr_ptr) ) {
      SD_MSG_HIGH( "Skip list=%-3d, pos=%-3d - avoid-time", list, i, 0 );
    }

    else if( new_acq_cnt != 0 && new_acq_cnt == sr_ptr->new_acq_cnt ) {
      SD_MSG_HIGH( "Skip list=%-3d, pos=%-3d - new_acq_cnt", list, i, 0 );
    }

    else
    {
      /* If we got here, the system is available for acquisition - return its
      ** list position.
      */
      return i;
    }

  } /* for */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no system that is available for acquisition is found -
  ** return SDSR_POS_NULL.
  */
  return SDSR_POS_NULL;

} /* sdss_sr_list_find_next */ /*lint +esym(715, bss_type_pref) */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_find_acq_start_pos

DESCRIPTION
  Decide on the acquisition starting point for the ACQ_CNT() and ACQ_TIME()
  acquisition constructs.

DEPENDENCIES
  None.

RETURN VALUE
  The acquisition start position to be used for the ACQ_CNT() and ACQ_TIME()
  acquisition constructs.

SIDE EFFECTS
  None.

===========================================================================*/
static  int                       sdss_sr_list_find_acq_start_pos(

        const sdss_s_type         *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        sdsr_e_type               acq_list,
            /* Acquisition list.
            */

        sdssscr_prm_type          start_pos
            /* Start position parameter from which to conclude the actual
            ** acquisition starting position.
            */
)
{
  int               pos;
  sdsr_ref_type     sr_ref  = SDSR_REF_NULL;
  sdsr_e_type       list    = SDSR_MAX;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( acq_list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If start position is one of the magic values, set sr_ref variable
  ** accordingly.
  */
  switch( start_pos )
  {
    case SDSSSCR_FULL_SYS:
      list = sdss_sr_list_map( ss_ptr, SDSR_ACQ_FULL_SYS );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_MINI_SYS:
      list = sdss_sr_list_map( ss_ptr, SDSR_ACQ_MINI_SYS );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_MICRO_SYS:
      list = sdss_sr_list_map( ss_ptr, SDSR_ACQ_MICRO_SYS );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_FULL2_SYS:
      list = sdss_sr_list_map( ss_ptr, SDSR_ACQ_FULL2_SYS );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_MINI2_SYS:
      list = sdss_sr_list_map( ss_ptr, SDSR_ACQ_MINI2_SYS );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_MICRO2_SYS:
      list = sdss_sr_list_map( ss_ptr, SDSR_ACQ_MICRO2_SYS );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_DEEP_SYS:
      list = sdss_sr_list_map( ss_ptr, SDSR_ACQ_DEEP_SYS );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_DEEP2_SYS:
      list = sdss_sr_list_map( ss_ptr, SDSR_ACQ_DEEP2_SYS );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_SHALLOW_SYS:
      list = sdss_sr_list_map( ss_ptr, SDSR_ACQ_SHALLOW_SYS );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_SHALLOW2_SYS:
      list = sdss_sr_list_map( ss_ptr, SDSR_ACQ_SHALLOW2_SYS );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_EMERG_SYS:
      list = sdss_sr_list_map( ss_ptr, SDSR_EMERG_SYS );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_UNIFORM_FULL_SYS:
      list = sdss_sr_list_map( ss_ptr, SDSR_ACQ_UNIFORM_FULL_SYS );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      break;

  } /* switch */

  if( list != SDSR_MAX ) {
    sr_ref = sdsr_list_get_ref( list, 0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If start position is one of the magic values, and the corresponding
  ** system record reference is found on the acquisition list, set the
  ** acquisition start position to 1 over the matching position.
  */
  if( sr_ref != SDSR_REF_NULL &&
      (pos = sdsr_list_find_ref(acq_list, sr_ref)) != SDSR_POS_NULL )
  {
    start_pos = (word) pos + 1;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return start_pos;

} /* sdss_sr_list_find_acq_start_pos */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_set_acq_end_pos

DESCRIPTION
  Set the acquisition end position to one before the acquisition start
  position ( on behalf of the ACQ_XXX() acquisition constructs).
  constructs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdss_sr_list_set_acq_end_pos(

        const sdss_s_type         *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        sdsr_e_type               acq_list,
            /* Acquisition list.
            */

        sdss_acq_mode_e_type      acq_mode,
            /* Acquisition mode.
            */

        int                       start_pos
            /* Acquisition start position.
            */
)
{
  sdsr_ref_type     sr_ref;
  int               end_pos;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( acq_list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acq_mode, SDSS_ACQ_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If list is empty, return now.
  */
  if( sdsr_list_cnt(acq_list) <= 0 )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the acquisition end position to one before the acquisition start
  ** position.
  */
  if( start_pos == 0 )
  {
    end_pos = sdsr_list_cnt( acq_list ) - 1;
  }
  else
  {
    end_pos = start_pos - 1;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a reference to the acquisition end position.
  */
  sr_ref = sdsr_list_get_ref( acq_list, end_pos );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Depending on the acquistion mode, set the appropriate SDSR_ACQ_xxx_SYS.
  */
  switch( acq_mode )
  {
    case SDSS_ACQ_MODE_HO:
    case SDSS_ACQ_MODE_FULL:
    case SDSS_ACQ_MODE_UNIFORM_FULL:
      sdsr_list_put_ref( sdss_sr_list_map( ss_ptr, SDSR_ACQ_FULL_SYS ), sr_ref );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_MINI:
      sdsr_list_put_ref(sdss_sr_list_map( ss_ptr, SDSR_ACQ_MINI_SYS ), sr_ref );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_MICRO:
      sdsr_list_put_ref( sdss_sr_list_map( ss_ptr, SDSR_ACQ_MICRO_SYS ), sr_ref );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_FULL2:
    case SDSS_ACQ_MODE_UNIFORM_FULL2:
    case SDSS_ACQ_MODE_UNIFORM_FULL_RTN2:
      sdsr_list_put_ref( sdss_sr_list_map( ss_ptr, SDSR_ACQ_FULL2_SYS ), sr_ref );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_MINI2:
      sdsr_list_put_ref( sdss_sr_list_map( ss_ptr, SDSR_ACQ_MINI2_SYS ), sr_ref );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_MICRO2:
      sdsr_list_put_ref( sdss_sr_list_map( ss_ptr, SDSR_ACQ_MICRO2_SYS ), sr_ref );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_DEEP:
      sdsr_list_put_ref( sdss_sr_list_map( ss_ptr, SDSR_ACQ_DEEP_SYS ), sr_ref );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_DEEP2:
      sdsr_list_put_ref( sdss_sr_list_map( ss_ptr, SDSR_ACQ_DEEP2_SYS ),sr_ref );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_SHALLOW:
      sdsr_list_put_ref( sdss_sr_list_map( ss_ptr, SDSR_ACQ_SHALLOW_SYS ), sr_ref );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_SHALLOW2:
      sdsr_list_put_ref( sdss_sr_list_map( ss_ptr, SDSR_ACQ_SHALLOW2_SYS ), sr_ref );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_UNIFORM_FULL_RTN:
      sdsr_list_put_ref( sdss_sr_list_map( ss_ptr, SDSR_ACQ_UNIFORM_FULL_SYS ), sr_ref );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_PREV:
    case SDSS_ACQ_MODE_MAX:
    default:
      SD_CASE_ERR( "acq_mode",0,0,0 );
      break;

  } /* switch */

} /* sdss_sr_list_set_acq_end_pos */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_create

DESCRIPTION
  Create a couterpart of limited system records based on the reference
  list/item. And update the reference list with the GW LTD list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdss_sr_list_create(

        sd_ss_e_type              ss,
            /* The ss for which the list is to be created
            */

        sdsr_e_type               list
            /* System list/item refered to create the couterpart of limited
            ** systems.
            */

)
{
  int i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_MED( "Create GW LTD system, ss=%d reference list=%d", ss, list, 0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Populate the limited system from input list in reverse order as it
  ** is inserted to the GW LTD LIST. */
  for( i = sdsr_list_cnt( list ) - 1; i>=0;  i-- )
  {
    sdsr_list_update_gw_sys( ss,
                             sdsr_list_get_ref(list, i),
                             TRUE,
                             FALSE,
                             FALSE );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the limited system to list.*/
  sdsr_list_cpy( list, SDSR_GW_LAST_LTD_SYS );

}




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
================================ SCHEDULING =================================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdss_sr_sched_inc

DESCRIPTION
  Increment the schedule field of the system record list.

  If we need to loop back to the beginning of the schedule, do so and also
  reset the schedule start time to the current uptime.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdss_sr_sched_inc(

        sdsr_s_type               *sr_ptr
            /* Pointer to a system record for which to increment the
            ** reacquisition schedule.
            */
)
{
  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT( INRANGE(sr_ptr->sched, 0, (ARR_SIZE(sdss_sched_arr)-2)) );
  SD_ASSERT( sdss_sched_arr[sr_ptr->sched] != SCHED_END );
  SD_ASSERT( sdss_sched_arr[sr_ptr->sched] != SCHED_LOOP );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Increment the schedule.
  */
  sr_ptr->sched++;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we need to loop back to the beginning of the schedule, do so.
  */
  if( sdss_sched_arr[sr_ptr->sched] == SCHED_LOOP )
  {
    while( sdss_sched_arr[sr_ptr->sched] != SCHED_START )
    {
      sr_ptr->sched--;
      SD_ASSERT( INRANGE(sr_ptr->sched, 0, (ARR_SIZE(sdss_sched_arr)-1)) );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are starting a schedule, increment the schedule counter to point
  ** at the first schedule element.
  */
  if( sdss_sched_arr[sr_ptr->sched] == SCHED_START )
  {
    sr_ptr->sched++;
  }

  SD_ASSERT( sdss_sched_arr[sr_ptr->sched] != SCHED_START );
  SD_ASSERT( sdss_sched_arr[sr_ptr->sched] != SCHED_LOOP );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we did not reach the end of the schedule, set the reacquisition
  ** time_count field according to the schedule.
  */
  if( sdss_sched_arr[sr_ptr->sched] != SCHED_END )
  {
    sr_ptr->sched_prm.acq.time_cnt = sdss_sched_arr[sr_ptr->sched];
  }

} /* sdss_sr_sched_inc */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_sched_get_act

DESCRIPTION
  Return the action that is called for by the reacquisition schedule of a
  specified system record.

DEPENDENCIES
  None.

RETURN VALUE
  Action called for by the reacquisition schedule, as defined by
  sdss_sched_act_e_type.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdss_sched_act_e_type     sdss_sr_sched_get_act(

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record for which to get the action called
            ** for by the reacquisition schedule.
            */
)
{
  byte              sched_time;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the schedule field is not set for reacquisitions, inform caller that
  ** schedule is ended.
  */
  if( sr_ptr->sched >= ARR_SIZE(sdss_sched_arr) )
  {
    return SDSS_SCHED_ACT_END;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the reacquisition schedule is exhausted, inform caller that schedule
  ** is ended.
  */
  sched_time = sdss_sched_arr[sr_ptr->sched];

  SD_ASSERT( sched_time != SCHED_START &&
             sched_time != SCHED_LOOP )

  if( sched_time == SCHED_END )
  {
    return SDSS_SCHED_ACT_END;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, we have an active reacquisition schedule. If schedule is
  ** time based, check the time_cnt field against the current uptime to see
  ** whether the schedule is calling for a reacquisition attempt at or before
  ** the current time, return SDSS_SCHED_ACT_ACQ. Else, return
  ** SDSS_SCHED_ACT_WAIT.
  */
  if( SDSSSCR_SCHED_IS_TIME(sr_ptr->sched) )
  {
    if( clk_uptime() >= sr_ptr->event_time + sr_ptr->sched_prm.acq.time_cnt )
    {
      return SDSS_SCHED_ACT_ACQ;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else if schedule is count based, check the time_cnt field against the
  ** current acquisition counter.
  */
  else if( SDSSSCR_SCHED_IS_CNT(sr_ptr->sched) )
  {
    if( sr_ptr->sched_prm.acq.time_cnt == 0 )
    {
      return SDSS_SCHED_ACT_ACQ;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, schedule is out of range - indicate that schedule is over.
  */
  else
  {
    return SDSS_SCHED_ACT_END;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDSS_SCHED_ACT_WAIT;

} /* sdss_sr_sched_get_act */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_sched

DESCRIPTION
  Check whether a system complies with a specific PRL designation value.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system complies with the PRL specified designation value.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdss_sr_is_sched(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record for which to check the schedule.
            */

        sdssscr_sched_e_type      sched
            /* Schedule against which to check.
            */
)
{
  int    i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( sched, SDSSSCR_SCHED_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step backward to the beginning of the schedule. If a match is found
  ** before stepping into the previous schedule, return TRUE. Else, return
  ** FALSE.
  */
  for( i = (int) sr_ptr->sched;

       INRANGE(i, 0, (SDSSSCR_SCHED_MAX-1)) &&
       sdss_sched_arr[i] != SCHED_LOOP &&
       sdss_sched_arr[i] != SCHED_END;

       i-- )
  {
    if( i == (int) sched )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_sr_is_sched */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_is_sched

DESCRIPTION
  Check whether a specified reacquisition schedule includes the reacquisition
  schedule of at least one of the systems that are referenced by a specified
  list.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified reacquisition schedule includes the reacquisition
  schedule of at least one of the systems that are referenced by a specified
  list. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_sr_list_is_sched(

        sdsr_e_type                   list,
            /* List for which to check the reacquisition schedule.
            */

        sdsr_e_type                   sched_list,
            /* Schedule list.
            */

        sdssscr_sched_e_type          sched
            /* Schedule against which to check.
            */
)
{
  int                 i;
  sdsr_s_type         *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( sched_list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( sched, SDSSSCR_SCHED_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and return TRUE if any of the systems that are
  ** referenced by this list is on the SCHED list and its reacquisition
  ** schedule is equals the specified reacquisition schedule value.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( sdsr_list_find(sched_list, sr_ptr) != SDSR_POS_NULL &&
        sdss_sr_is_sched(sr_ptr, sched) )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_sr_list_is_sched */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_sched

DESCRIPTION
  Set the schedule field of the system records that are referenced by the
  list parameter as per the reacquisition schedule that is indicated by the
  acq_type, acq_mode and sched parameters.

  Insert the system records that are referenced by the list parameter at the
  beginning of the SDSR_SCHED_LST.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdss_sr_list_sched(

        sdsr_e_type               list,
            /* System record list for which to set the schedule for
            ** reacquisition attempts.
            */

        sdsr_e_type               sched_list,
            /* Schedule list.
            */

        sdssscr_sched_e_type      sched,
            /* Reacquisition schedule as defined by sdssscr_sched_e_type.
            */

        sdssscr_acq_e_type        acq_type,
            /* Reacquisition type to be attempted as defined by
            */

        sdss_acq_mode_e_type      acq_mode,
            /* Mode of reacquisition attempt.
            */

        sdsssr_sched_cond_e_type  sched_cond
            /* Indicate whether systems should be conditionally schedule
            ** depending whether they are not already on equal or higher
            ** priority reacquisition schedule.
            */
)
{
  int i;
  int ins_pos = 0;
  sdsr_s_type       *sr_ptr;
  sdsr_ref_type     sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( sched_list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_MED( "Sched acq, list=%d, sched=%d time=%d",
               list, sched, clk_uptime() );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and set the schedule, its start time and its
  ** reacquisition type for each system. Also insert the system into the
  ** specified schedule list.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    /* If only systems that are not on equal or higher priority reacquisition
    ** schedule should be scheduled and the system is already on an equal or
    ** higher priority schedule, continue.
    */
    switch( sched_cond )
    {
      case SDSSSR_SCHED_COND_NOT_ON_HIGH_SCHED:

        if( sdsr_list_find(sched_list, sr_ptr) != SDSR_POS_NULL
            &&
            ( SDSSSCR_SCHED_IS_HIGH(sr_ptr->sched) ||
              SDSSSCR_SCHED_IS_LOW(sched) ))
        {
          continue;
        }
        break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSR_SCHED_COND_NOT_ON_SAME_SCHED:

        if( sdsr_list_find(sched_list, sr_ptr) != SDSR_POS_NULL &&
            sdss_sr_is_sched(sr_ptr, sched) )
        {
          continue;
        }
        break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSR_SCHED_COND_NOT_ON_EQUAL_OR_HIGHER_PRIORITY_SCHED_OR_AVOID:

        if( ( sdsr_list_find(sched_list, sr_ptr) != SDSR_POS_NULL &&
              sr_ptr->sched >= sched )
              ||
              SDSSSCR_SCHED_IS_AVOID( sr_ptr->sched ) )
        {
          continue;
        }
        break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSR_SCHED_COND_NOT_ON_EQUAL_OR_HIGHER_PRIORITY_SCHED_OR_AVOID_OR_FORBID:

        if( ( sdsr_list_find(sched_list, sr_ptr) != SDSR_POS_NULL &&
              sr_ptr->sched >= sched )
              ||
              SDSSSCR_SCHED_IS_AVOID( sr_ptr->sched )
              ||
              sdss_sr_is_prl(sr_ptr, SDSS_PRL_VAL_FORBID) )
        {
          continue;
        }
        break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSR_SCHED_COND_UNCONDITIONAL:
      case SDSSSR_SCHED_COND_NONE:
      case SDSSSR_SCHED_COND_MAX:
      default:
        break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    sr_ptr->sched               = (word)sched;
    sr_ptr->sched_prm.acq.type  = (byte)acq_type;
    sr_ptr->sched_prm.acq.mode  = (byte)acq_mode;
    sr_ptr->event_time          = clk_uptime();

    sdsr_print( sr_ptr, FALSE );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Kick-start the reacquisition schedule.
    */
    sdss_sr_sched_inc( sr_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Insert system into the specified schedule_list.
    */
    sr_ref = sdsr_list_get_ref( list, i );
    SD_ASSERT( sr_ref != SDSR_REF_NULL );
    sdsr_list_insert_ref( sched_list, ins_pos, sr_ref, TRUE );
    ins_pos++;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH( "  Sechule list count=%d",sdsr_list_cnt(sched_list),0,0);

} /* sdss_sr_list_sched */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_sched_proc

DESCRIPTION
  Given a list, do necessary scheduling related processing on this list.

DEPENDENCIES
  None.

RETURN VALUE
  The position of the first system record on the specified list that complies
  with the specified mode and band preference and for which the system
  record's schedule is calling for a reacquisition attempt at the current
  time if such a system record is found. SDSR_POS_NULL, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  int                       sdss_sr_list_sched_proc(

        sdsr_e_type               list,
            /* List over which to check the schedule.
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference with which system should comply.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* Band preference with which system should comply.
            */

        sys_wlan_bss_e_type       bss_type_pref
            /* Bss type preference with which system should comply.
            */
)
{
  int i;
  sdsr_s_type       *sr_ptr;
  sdsr_s_type       sr;
  sd_ss_mode_pref_e_type    acq_mode_pref;
  sd_ss_band_pref_e_type    acq_band_pref;
  /*lint -esym(550, acq_bss_type_pref) */ /* not accessed */
  sys_wlan_bss_e_type       acq_bss_type_pref = SYS_WLAN_BSS_TYPE_ANY;
  boolean           inc_schedule;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( bss_type_pref, SYS_WLAN_BSS_TYPE_MAX );

  SYS_ARG_NOT_USED( acq_bss_type_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and increment the schedules of systems for which
  ** their schedule is calling for acquisition attempt at the current time,
  ** but are not available for acquisition attempt. A system is not available
  ** for acquisition attempt if one of the following is true:
  **
  ** 1. The system's mode/band combination is not supported by this target.
  ** 2. The system does not comply with the specified mode preferences.
  ** 3. The system does not comply with the specified band preferences.
  ** 4. Filtered band within the target capability,system mode, mode pref and
  **    band pref.
  ** 5. The system does not comply with the specified bss type preferences.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( sdss_sr_sched_get_act(sr_ptr) == SDSS_SCHED_ACT_ACQ )
    {
      /* If the system under consideration has a SPECIFIC acquisition type,
      ** ignore the current mode and band preference.
      */
      if( sr_ptr->sched_prm.acq.type == SDSSSCR_ACQ_SPECIFIC ||
          sr_ptr->sched_prm.acq.type == SDSSSCR_ACQ_SPECIFIC_PRL )
      {
        acq_mode_pref     = SD_SS_MODE_PREF_ANY;
        acq_band_pref     = SD_SS_BAND_PREF_ANY;
        acq_bss_type_pref = SYS_WLAN_BSS_TYPE_ANY;
      }
      else
      {
        acq_mode_pref     = mode_pref;
        acq_band_pref     = band_pref;
        acq_bss_type_pref = bss_type_pref;
      }

      inc_schedule = FALSE;

      if( ! sdss_is_supp_mode_band(sr_ptr->sys.mode, sr_ptr->sys.band) ||
          ! sdss_sr_is_mode_pref(sr_ptr, acq_mode_pref) )
      {
        sr.sys = sr_ptr->sys;

        /* Filtered the system band within the target capability.
        */
        sr.sys.band = sdss_get_supp_band_pref(sr.sys.band);

        /* Filtered the system band within the system mode preference.
        */
        sr.sys.band = sdss_get_supp_mode_band_pref(
                        sdss_map_mode_to_mode_pref(sr.sys.mode),
                        sr.sys.band);

        /* Filtered the system band within the mode preference.
        */
        sr.sys.band = sdss_get_supp_mode_band_pref(acq_mode_pref,sr.sys.band);

        /* Filter the system band within the band preference.
        */
        if( ! sdss_sr_is_band_pref(&sr, acq_band_pref)  ) {
          inc_schedule = TRUE;
        }
      }
      #ifdef FEATURE_WLAN_ADHOC
#error code not present
      #endif
      if( inc_schedule )
      {
        sdss_sr_sched_inc( sr_ptr );
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and delete systems for which their schedules is
  ** exhausted.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( sdss_sr_sched_get_act(sr_ptr) == SDSS_SCHED_ACT_END )
    {
      sdsr_list_del_pos( list, i );
      i--;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and look for first system for which its schedule
  ** is calling for acquisition attempt at the current time and is available
  ** for acquisition attempt. A system is available for acquisition attempt
  ** if all of the following are true:
  **
  ** 1. The system's mode and band combination is supported by this target.
  ** 2. The system complies with the specified mode preferences.
  ** 3. The system complies with the specified band preferences.
  ** 4. Filtered band within the target capability,system mode, mode pref and
  **    band pref
  **
  ** If such a system is found, return its position. Else, return
  ** SDSR_POS_NULL.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    /* If the system under consideration has a SPECIFIC acquisition type,
    ** ignore the current mode and band preference.
    */
    if( sr_ptr->sched_prm.acq.type == SDSSSCR_ACQ_SPECIFIC ||
        sr_ptr->sched_prm.acq.type == SDSSSCR_ACQ_SPECIFIC_PRL )
    {
      acq_mode_pref = SD_SS_MODE_PREF_ANY;
      acq_band_pref = SD_SS_BAND_PREF_ANY;
      acq_bss_type_pref = SYS_WLAN_BSS_TYPE_ANY;
    }
    else
    {
      acq_mode_pref = mode_pref;
      acq_band_pref = band_pref;
      acq_bss_type_pref = bss_type_pref;
    }

    if( sdss_sr_sched_get_act(sr_ptr) == SDSS_SCHED_ACT_ACQ &&
        sdss_is_supp_mode_band(sr_ptr->sys.mode, sr_ptr->sys.band) &&
        sdss_sr_is_mode_pref(sr_ptr, acq_mode_pref)
        #ifdef FEATURE_WLAN_ADHOC
#error code not present
        #endif
      )
    {
      sr.sys = sr_ptr->sys;

      /* Filtered the system band within the target capability.
      */
      sr.sys.band = sdss_get_supp_band_pref(sr.sys.band);

      /* Filtered the system band within the system mode preference.
      */
      sr.sys.band = sdss_get_supp_mode_band_pref(
                      sdss_map_mode_to_mode_pref(sr.sys.mode),
                      sr.sys.band);

      /* Filtered the system band within the mode preference.
      */
      sr.sys.band = sdss_get_supp_mode_band_pref(acq_mode_pref,sr.sys.band);

      /* Filter the system band within the band preference.
      */
      if( sdss_sr_is_band_pref(&sr, acq_band_pref)  ) {
        return i;
      }

       else
       {
         SD_MSG_HIGH( "Skip list=%-3d, pos=%-3d - band-pref", list, i, 0 );
       }
     }
     else
     {
       SD_MSG_HIGH( "Skip list=%-3d, pos=%-3d - target-cap, mode or bss type",
                    list, i, 0 );
     }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDSR_POS_NULL;

} /* sdss_sr_list_sched_proc */ /*lint +esym(550, acq_bss_type_pref) */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_sched_srv

DESCRIPTION
  Given a list, do necessary scheduling related servicing on this list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdss_sr_list_sched_srv(

        sdsr_e_type               list,
            /* List over which to service the schedule.
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /* Service only systems that complies with this mode preference.
            */

        sdsr_e_type               acq_list,
        int                       acq_pos
            /* List and position of the system record that is attempted
            ** acquisition.
            */
)
{
  int i;
  sdsr_s_type       *sr_ptr;
  sdsr_s_type       *acq_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acq_list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at system record that is attempted acquisition.
  */
  acq_ptr = sdsr_list_get_ptr( acq_list, acq_pos );
  SD_ASSERT( acq_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If the last system attempted acquisition is from the list over which
  ** schedule is being serviced (SCHED_LST), increment the systems schedule.
  */
  /*lint -save -e774 *//* Boolean within 'right side of && within if'
                           always evaluates to True*/
  if( ( acq_list == list ) && (acq_ptr != NULL) )/*lint -restore */
  {
    sdss_sr_sched_inc( acq_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and service the schedule of systems that are
  ** on count based reacquisition schedules as follows:
  **
  ** 1. Decrement the time_cnt filed of a system record that is on a count
  **    based reacquisition schedule and is currently NOT attempted
  **    acquisition.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    /* If system does not complies with the designated mode preference,
    ** continue.
    */
    if( ! sdss_sr_is_mode_pref(sr_ptr, mode_pref) )
    {
      continue;
    }

    if( sr_ptr != acq_ptr &&
             SDSSSCR_SCHED_IS_CNT(sr_ptr->sched) &&
             sr_ptr->sched_prm.acq.time_cnt > 0 )
    {
      sr_ptr->sched_prm.acq.time_cnt--;
    }
  }

} /* sdss_sr_list_sched_srv */




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== ROUTINES FOR ACQUISITION THROTTLING ====================
=============================================================================
=============================================================================
===========================================================================*/


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_thrttl_update_state

DESCRIPTION
  Helper function which updates the state of throttling mechanism.


DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_thrttl_update_state(

        sdss_acq_thrttl_s_type  *acq_thrttl_ptr,
            /* Pointer to a acq. throttle information structure.
            */

        sdss_thrttl_state_e_type new_state
            /* New state of thrttl engine.
          */
)
{
  SD_MSG_HIGH("Update thrttl state %d %d",
            new_state, acq_thrttl_ptr->state, 0
       );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the state.
  */
  SD_ASSERT_ENUM_IS_INRANGE( new_state, SDSS_THRTTL_STATE_MAX );
  SD_ASSERT( acq_thrttl_ptr != NULL );

  acq_thrttl_ptr->state = new_state;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reinit if going back to INACT state.
  */
  if ( new_state == SDSS_THRTTL_STATE_INACT )
  {
    acq_thrttl_ptr->curr_stage = 0;
    acq_thrttl_ptr->acq_count  = 0;
  }

} /* sdss_thrttl_update_state() */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_thrttl_inc_acq_count

DESCRIPTION
  Helper function which increments the acquisition count in the state.


DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  TRUE - if all acquisitions are done in current stage.
  FALSE - not all acquisitions are done.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                    sdss_thrttl_inc_acq_count(

        sdss_acq_thrttl_s_type     *acq_thrttl_ptr
            /* Pointer to a acq. throttle information structure.
            */
)
{

  /* Local pointer to throttle backoff alogorithm table.
  */
  sdss_thrttl_stage_param_s_type *thrttl_tbl_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT(acq_thrttl_ptr != NULL );
  thrttl_tbl_ptr =    acq_thrttl_ptr->thrttl_tbl_ptr;
  SD_ASSERT( thrttl_tbl_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( acq_thrttl_ptr->state, SDSS_THRTTL_STATE_MAX );
  SD_ASSERT( acq_thrttl_ptr->curr_stage < acq_thrttl_ptr->num_stages );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH("Increment thrttl acq count %d %d %d",
              acq_thrttl_ptr->state,
              acq_thrttl_ptr->curr_stage,
              acq_thrttl_ptr->acq_count
             );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Increment the count and set it to MAX.
  */
  if ( acq_thrttl_ptr->acq_count <
         thrttl_tbl_ptr[acq_thrttl_ptr->curr_stage].num_acq
     )
  {
    /* All acquisitions in this stage are not completed.
    */
    ++acq_thrttl_ptr->acq_count;
    return FALSE;

  }

  /* Done all acquisitions in this stage.
  */
  return TRUE;

} /* sdss_thrttl_inc_acq_count() */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_thrttl_inc_curr_stage

DESCRIPTION
  Helper function which increments the current stage of throttling.


DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  TRUE - reached the final stage.
  FALSE - not reached the final stage.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                    sdss_thrttl_inc_curr_stage(

        sdss_acq_thrttl_s_type     *acq_thrttl_ptr
            /* Pointer to a acq. throttle information structure.
            */
)
{

  /* Local pointer to throttle backoff alogorithm table.
  */
  sdss_thrttl_stage_param_s_type *thrttl_tbl_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT(acq_thrttl_ptr != NULL );
  thrttl_tbl_ptr =    acq_thrttl_ptr->thrttl_tbl_ptr;
  SD_ASSERT( thrttl_tbl_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( acq_thrttl_ptr->state, SDSS_THRTTL_STATE_MAX );
  SD_ASSERT( acq_thrttl_ptr->curr_stage < acq_thrttl_ptr->num_stages );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(thrttl_tbl_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH("Increment thrttl curr stage %d %d %d",
              acq_thrttl_ptr->state,
              acq_thrttl_ptr->curr_stage,
              acq_thrttl_ptr->acq_count
             );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset the acq_count.
  */
  acq_thrttl_ptr->acq_count = 0;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Increment the count and set it to MAX.
  */
  if ( acq_thrttl_ptr->curr_stage < acq_thrttl_ptr->num_stages - 1)
  {
    /* Not reached the final stage.
    */
    ++acq_thrttl_ptr->curr_stage;
    return FALSE;

  }

  /* Reached the final stage.
  */
  return TRUE;

} /* sdss_thrttl_inc_curr_stage() */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_thrttl_acq_init

DESCRIPTION
  Initializes the acquisition throttle mechanism.


DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_thrttl_acq_init(

        sdss_acq_thrttl_s_type  *acq_thrttl_ptr
            /* Pointer to a acq. throttle information structure.
            */
)
{
  SD_ASSERT( acq_thrttl_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  acq_thrttl_ptr->state          = SDSS_THRTTL_STATE_INACT;
  acq_thrttl_ptr->thrttl_tbl_ptr = dflt_thrttl_tbl;
  acq_thrttl_ptr->num_stages     = ARR_SIZE(dflt_thrttl_tbl);
  sdss_thrttl_update_state ( acq_thrttl_ptr, SDSS_THRTTL_STATE_INACT );

} /* sdss_thrttl_acq_init() */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_thrttl_enter_pwr_save

DESCRIPTION
  This routine preprocesses the PWR_SAVE_ENTER event pertaining to
  acquisition throttling engine.


DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  sdss_thrttl_evt_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
static  sdss_thrttl_evt_status_e_type   sdss_thrttl_enter_pwr_save(

        sdss_acq_thrttl_s_type          *acq_thrttl_ptr,
            /* Pointer to a acq. throttle information structure.
            */

        dword                           *scr_timer_ptr,
          /* Pointer to the SS-script-engine's timer to be populated.
          */

        sdss_iact_s_type                *rtrn_iact_ptr
            /* Pointer to the return internal action.
            */

)
{

  /* Local pointer to throttle backoff alogorithm table.
  */
  sdss_thrttl_stage_param_s_type *thrttl_tbl_ptr;
  sdss_thrttl_evt_status_e_type  thrttl_evt_status =
                                         SDSS_THRTTL_EVT_STATUS_NOT_CONSUMED;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_thrttl_ptr != NULL );
  thrttl_tbl_ptr =    acq_thrttl_ptr->thrttl_tbl_ptr;
  SD_ASSERT( thrttl_tbl_ptr != NULL );
  SD_ASSERT( rtrn_iact_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( acq_thrttl_ptr->state, SDSS_THRTTL_STATE_MAX );
  SD_ASSERT( acq_thrttl_ptr->curr_stage < acq_thrttl_ptr->num_stages );
  SD_ASSERT( scr_timer_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process acquire action based on state.
  */
  switch ( acq_thrttl_ptr->state )
  {
    case SDSS_THRTTL_STATE_ENTER_PWR_SAVE:
      /* Ignore whether throttling is still enabled or not.
      */
      *scr_timer_ptr = clk_uptime() +
                          thrttl_tbl_ptr[acq_thrttl_ptr->curr_stage].delay;
      rtrn_iact_ptr->act = SDSS_IACT_CONTINUE;
      sdss_thrttl_update_state( acq_thrttl_ptr, SDSS_THRTTL_STATE_PWR_SAVE );
      thrttl_evt_status = SDSS_THRTTL_EVT_STATUS_CONSUMED;
      break;


    case SDSS_THRTTL_STATE_INACT:
    case SDSS_THRTTL_STATE_ACQ:
    case SDSS_THRTTL_STATE_PWR_SAVE:
      SD_MSG_HIGH("Ignore pwr_save enter thrttl state %d %d %d",
                  acq_thrttl_ptr->state,
                  acq_thrttl_ptr->curr_stage,
                  acq_thrttl_ptr->acq_count
                 );
      break;


    case SDSS_THRTTL_STATE_MAX:
    default:
      SD_ERR_FATAL("Incorrect state %d",acq_thrttl_ptr->state, 0, 0 );
      break;

  }/* switch ( acq_thrttl_ptr->state ) */

  return thrttl_evt_status;

} /*  sdss_thrttl_enter_pwr_save */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_thrttl_pwr_save_timeout

DESCRIPTION
  This routine preprocesses the power save timeout event pertaining to
  acquisition throttling engine.


DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  sdss_thrttl_evt_status_e_type

SIDE EFFECTS
  None.

===========================================================================*/
static  sdss_thrttl_evt_status_e_type  sdss_thrttl_pwr_save_timeout(

        sdss_acq_thrttl_s_type         *acq_thrttl_ptr,
            /* Pointer to a acq. throttle information structure.
            */

        dword                          *scr_timer_ptr,
          /* Pointer to the SS-script-engine's timer to be populated.
          */

        sdss_iact_s_type               *rtrn_iact_ptr
            /* Pointer to the return internal action.
            */

)
{

  /* Local pointer to throttle backoff alogorithm table.
  */
  sdss_thrttl_stage_param_s_type *thrttl_tbl_ptr;

  sdss_thrttl_evt_status_e_type  thrttl_evt_status = \
                                         SDSS_THRTTL_EVT_STATUS_NOT_CONSUMED;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_thrttl_ptr != NULL );
  thrttl_tbl_ptr =    acq_thrttl_ptr->thrttl_tbl_ptr;
  SD_ASSERT( thrttl_tbl_ptr != NULL );
  SD_ASSERT( rtrn_iact_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( acq_thrttl_ptr->state, SDSS_THRTTL_STATE_MAX );
  SD_ASSERT( acq_thrttl_ptr->curr_stage < acq_thrttl_ptr->num_stages );
  SD_ASSERT( scr_timer_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(thrttl_tbl_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process acquire action based on state.
  */
  switch ( acq_thrttl_ptr->state )
  {
    case SDSS_THRTTL_STATE_PWR_SAVE:

      *scr_timer_ptr = 0;
      *rtrn_iact_ptr = acq_thrttl_ptr->last_acq_iact;
      thrttl_evt_status = SDSS_THRTTL_EVT_STATUS_CONSUMED;

      /* Check if throttling was disabled in the middle.
      */
      if ( !acq_thrttl_ptr->is_thrttl_on )
      {
        /* Throttling is disabled, move to STATE_INACT.
        */
        sdss_thrttl_update_state( acq_thrttl_ptr, SDSS_THRTTL_STATE_INACT );
      }
      else
      {
        /* Throttling is enabled.
        */
        sdss_thrttl_update_state( acq_thrttl_ptr, SDSS_THRTTL_STATE_ACQ );
        (void)sdss_thrttl_inc_curr_stage( acq_thrttl_ptr );
        (void)sdss_thrttl_inc_acq_count( acq_thrttl_ptr );
      }
      break;


    case SDSS_THRTTL_STATE_INACT:
    case SDSS_THRTTL_STATE_ACQ:
    case SDSS_THRTTL_STATE_ENTER_PWR_SAVE:
      SD_MSG_HIGH("Ignore pwr_save timeout thrttl state %d %d %d",
                    acq_thrttl_ptr->state,
                    acq_thrttl_ptr->curr_stage,
                    acq_thrttl_ptr->acq_count
                   );
      break;


    case SDSS_THRTTL_STATE_MAX:
    default:
      SD_ERR_FATAL("Incorrect state %d",acq_thrttl_ptr->state, 0, 0 );
      break;

  }/* switch ( acq_thrttl_ptr->state ) */

  return thrttl_evt_status;

} /*  sdss_thrttl_pwr_save_timeout */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_thrttl_iact_acq

DESCRIPTION
  This routine preprocesses the internal acquisition action.


DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                           sdss_thrttl_iact_acq(

        sdss_acq_thrttl_s_type         *acq_thrttl_ptr,
            /* Pointer to a acq. throttle information structure.
            */

        sdss_iact_s_type               *rtrn_iact_ptr
            /* Pointer to the return internal action.
            */

)
{
  /* Local pointer to throttle backoff alogorithm table.
  */
  sdss_thrttl_stage_param_s_type *thrttl_tbl_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_thrttl_ptr != NULL );
  thrttl_tbl_ptr =    acq_thrttl_ptr->thrttl_tbl_ptr;
  SD_ASSERT( thrttl_tbl_ptr != NULL );
  SD_ASSERT( rtrn_iact_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( acq_thrttl_ptr->state, SDSS_THRTTL_STATE_MAX );
  SD_ASSERT( acq_thrttl_ptr->curr_stage < acq_thrttl_ptr->num_stages );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(thrttl_tbl_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the return action is acq.
  */
  if ( rtrn_iact_ptr->act != SDSS_IACT_ACQUIRE )
  {
    /* It is not acquire action, so just return.
    */
    SD_MSG_HIGH(" thrttl act not acquire %d", rtrn_iact_ptr->act, 0, 0 );
    return;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if throttling was disabled in the middle.
  */
  if ( !acq_thrttl_ptr->is_thrttl_on )
  {
    /* Throttling is disabled.
    */
    sdss_thrttl_update_state( acq_thrttl_ptr, SDSS_THRTTL_STATE_INACT );
    return;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is a handoff acquisition, don't throttle the acquisition
  */
  if( rtrn_iact_ptr->prm.acq.is_handoff_acq )
  {
    SD_MSG_HIGH("Handoff acq, ignore acq thrttl %d %d", rtrn_iact_ptr->act,
                                rtrn_iact_ptr->prm.acq.is_handoff_acq , 0 );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process acquire action based on state.
  */
  switch ( acq_thrttl_ptr->state )
  {
    case SDSS_THRTTL_STATE_INACT:
    case SDSS_THRTTL_STATE_ACQ:
      if ( sdss_thrttl_inc_acq_count( acq_thrttl_ptr ) )
      {
        /* All acquisitions in this stage are completed, do pwr save.
        */
        acq_thrttl_ptr->last_acq_iact = *rtrn_iact_ptr;
        rtrn_iact_ptr->act = SDSS_IACT_PWR_SAVE;
        sdss_thrttl_update_state( acq_thrttl_ptr, SDSS_THRTTL_STATE_ENTER_PWR_SAVE );
      }
      else
      {
        sdss_thrttl_update_state( acq_thrttl_ptr, SDSS_THRTTL_STATE_ACQ );
      }
      break;

    case SDSS_THRTTL_STATE_PWR_SAVE:
    case SDSS_THRTTL_STATE_ENTER_PWR_SAVE:
      sdss_thrttl_update_state( acq_thrttl_ptr, SDSS_THRTTL_STATE_ACQ );
      break;

    case SDSS_THRTTL_STATE_MAX:
    default:
      SD_ERR_FATAL("Incorrect state %d",acq_thrttl_ptr->state, 0, 0 );
      break;

  }/* switch ( acq_thrttl_ptr->state ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

} /* sdss_thrttl_iact_acq() */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_throttle_acq_on

DESCRIPTION
  This routine sets the throttle enable flag, so throttling can begin when
  acquisition begins.


DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                           sdss_throttle_acq_on(

        sd_ss_e_type                   ss
          /* System Selection instance.
          */

)
{

  sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if throttle is enabled.
  */
  if ( ss_ptr->acq_thrttl.is_thrttl_on )
  {
    /* Already enabled.
    */
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enable the throttling.
  */
  SD_MSG_HIGH("Enabling throttling for ss=%d state=%d",
              ss,
              ss_ptr->acq_thrttl.state,
              0
             );
  ss_ptr->acq_thrttl.is_thrttl_on = TRUE;

} /* sdss_throttle_acq_on() */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_throttle_acq_off

DESCRIPTION
  This routine resets the throttle enable flag, so throttling can be disabled,
  when there is event on this engine.


DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                           sdss_throttle_acq_off(

        sd_ss_e_type                   ss
          /* System Selection instance.
          */

)
{

  sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if throttle is disabled.
  */
  if ( !ss_ptr->acq_thrttl.is_thrttl_on )
  {
    /* Already disabled.
    */
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Disable the throttling.
  */
  SD_MSG_HIGH("Disabling throttling for ss=%d state=%d",
              ss,
              ss_ptr->acq_thrttl.state,
              0
             );
  ss_ptr->acq_thrttl.is_thrttl_on = FALSE;

} /* sdss_throttle_acq_off() */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_throttle_reset_cntrs

DESCRIPTION
  This function resets the throttling counters.Called when SD determines to
  idle on the acquired system.

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                           sdss_throttle_reset_cntrs(

        sd_ss_e_type                   ss
          /* System Selection instance.
          */

)
{

  sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if throttle is disabled.
  */
  if ( !ss_ptr->acq_thrttl.is_thrttl_on )
  {
    /* It is disabled.
    */
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset the throttle counters.
  */
  SD_MSG_HIGH("Reset thrttl cntrs for ss=%d state=%d",
              ss,
              ss_ptr->acq_thrttl.state,
              0
             );
  ss_ptr->acq_thrttl.acq_count = 0;
  ss_ptr->acq_thrttl.curr_stage = 0;

} /* sdss_throttle_reset_cntrs() */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_hdr_bcmcs_start_uptime_update

DESCRIPTION
  This function indicate the HDR BCMCS flow monitoring is started. It updates
  the start time with the uptime.

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                           sdss_hdr_bcmcs_start_uptime_update(

        sd_ss_e_type                   ss
          /* System Selection instance.
          */
)
{

  dword               uptime = clk_uptime();

  sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->hdr_bcmcs_start_uptime = uptime;

  return;

}/* sdss_hdr_bcmcs_start_uptime_update */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_hdr_bcmcs_stop_uptime_update

DESCRIPTION
  This function indicate the HDR BCMCS flow monitoring is stopped. It updates
  the stop time with the uptime.

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                           sdss_hdr_bcmcs_stop_uptime_update(

        sd_ss_e_type                   ss
          /* System Selection instance.
          */
)
{

  dword               uptime = clk_uptime();

  sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->hdr_bcmcs_stop_uptime = uptime;

  return;

}/* sdss_hdr_bcmcs_start_uptime_update */




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
========================= ACQUISITION STATE MACHINE =========================
=============================================================================
=============================================================================
===========================================================================*/


/* Macro to map a list into a system record list in the context of an
** acquisition state machine.
*/
#define ACQ_LIST( list )        sdss_sr_list_map( acq_ptr->ss_ptr, list )


/*===========================================================================

FUNCTION sdss_acq_init

DESCRIPTION
  Initialized an acquisition state machine with a specific system record list
  and acquisition type.

  NOTE! this function must be called before any other sdss_acq_xxx is called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_acq_init(

        sdss_acq_s_type         *acq_ptr,
            /* Pointer an acquisition state machine to be initialized.
            */

        sdss_s_type             *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        sdsr_e_type             list,
            /* System record list over which acquisition is to be attempted.
            */

        int                     start_pos,
            /* Position from which list traversal should start.
            */

        int                     max_num,
            /* Limit for the number of acquisitions to be attempted.
            */

        dword                   max_uptime,
            /* Limit for the duration over which to attempt acquisitions.
            */

        sdssscr_con_e_type      acq_con,
            /* Acquisition script construct that is calling for the
            ** acquisition attempt.
            */

        sdssscr_acq_e_type      acq_type,
            /* Acquisition type that is being attempted.
            */

        sdss_acq_mode_e_type    acq_mode,
            /* Mode of acquisition that is being attempted.
            */

        byte                    new_acq_cnt,
            /* The current value of the new-acquisition counter - only
            ** systems with different new-acquisition count should be
            ** attempted acquisition.
            */

        int2                    meas_val,
            /* The measurement value with which system should comply.
            */

       sdsr_cat_e_type          meas_cat
           /* The category for which measurement value should apply 
           */
)
{
  int   list_cnt;
  sd_ss_e_type      ss                = SD_SS_MAIN;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_ptr != NULL );
  SD_ASSERT( ss_ptr != NULL );

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT( start_pos >= 0 );

  SD_ASSERT( INRANGE(acq_con, SDSSSCR_CON_ACQ, SDSSSCR_CON_ACQ_NETLIST) );
  SD_ASSERT_ENUM_IS_INRANGE( acq_type, SDSSSCR_ACQ_MAX );
  SD_ASSERT( acq_type != SDSSSCR_ACQ_SAME_AS_LAST );
  SD_ASSERT_ENUM_IS_INRANGE( acq_mode, SDSS_ACQ_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if (defined (FEATURE_TECH_IND_SD_STACK_ARCH) || defined(FEATURE_HDR_HYBRID))
  if( ss_ptr == sdss_ptr(SD_SS_HYBR_1) )
  {
    ss = SD_SS_HYBR_1;
  }
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_MED( "Acq start, list=%d,type=%d,mode=%d",
                list, acq_type, acq_mode );

  sdsr_list_print( list, FALSE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the ss to point at the associated system selection enum type
  */
  acq_ptr->ss             = ss;


  /* Point at associated system selection information structure.
  */
  acq_ptr->ss_ptr         = ss_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the ACQ-State to start.
  */
  acq_ptr->state          = SDSS_ACQ_STT_START;
  acq_ptr->prev_state     = SDSS_ACQ_STT_START;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the system record list to be traversed during the SDSS_ACQ_STT_START
  ** state.
  */
  acq_ptr->acq_lst = list;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remember the list count.
  */
  list_cnt = sdsr_list_cnt( list );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the position from which list traversal should start.
  */
  if( start_pos >= list_cnt )
  {
    acq_ptr->acq_start_pos = 0;
  }
  else
  {
    acq_ptr->acq_start_pos = start_pos;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the number of list elements left to be traversed.
  */
  acq_ptr->acq_left       = max_num;

  /* Set the time limit for the acquisition attempts.
  */
  acq_ptr->acq_max_uptime = max_uptime;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the acquisition traversal position to none and indicate that list
  ** end was not reached.
  */
  acq_ptr->acq_pos                = SDSR_POS_NULL;
  acq_ptr->is_acq_lst_end_reached = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Record the acquisition script construct that is calling for this
  ** acquisition attempt.
  */
  acq_ptr->acq_con        = acq_con;

  /* Set the type and mode of the acquisition attempt.
  */
  acq_ptr->acq_type       = acq_type;
  acq_ptr->prev_acq_mode  = acq_ptr->acq_mode;
  acq_ptr->acq_mode       = acq_mode;

  /* Set the value of the new-acquisition counter.
  */
  acq_ptr->new_acq_cnt    = new_acq_cnt;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the measurement value with which system should comply.
  */
  acq_ptr->meas_val       = meas_val;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the measurement system category in which meas_val should comply.
  */
  acq_ptr->meas_cat       = meas_cat;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the AMPS 2nd strongest control channel indication to FALSE.
  */
  acq_ptr->is_amps_2nd_cch = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the system record list to be traversed during the
  ** SDSS_ACQ_STT_MORE_PREF state, set the list traversal position to
  ** NULL and clear the list.
  */
  acq_ptr->start_sys      = ACQ_LIST( SDSR_START_SYS );
  acq_ptr->pref_lst       = ACQ_LIST( SDSR_START_PREF_LST );
  acq_ptr->pref_pos       = SDSR_POS_NULL;
  sdsr_list_clr( acq_ptr->pref_lst );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the system record list to be attempted acquisition when a
  ** the reacquisition schedule of a particular system is calling for a
  ** reacquisition attempt.
  */
  acq_ptr->sched_lst      = ACQ_LIST( SDSR_SCHED_LST );
  acq_ptr->sched_pos      = SDSR_POS_NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the rtrn act to MAX.
  */
  acq_ptr->rtrn_iact.act  = SDSS_IACT_MAX;

} /* sdss_acq_init */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_acq_is_amps_2nd_cch

DESCRIPTION
  Check whether 2nd strongest AMPS control channel needs to be attempted
  acquisition.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if 2nd strongest AMPS control channel needs to be attempted
  acquisition. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                 sdss_acq_is_amps_2nd_cch(


        sdss_acq_s_type         *acq_ptr,
            /* Pointer an acquisition state machine in question.
            */

        sdsr_e_type             list,
            /* List for which to check AMPS 2nd strongest condition.
            */

        int                     pos,
            /* List position for which to check AMPS 2nd strongest condition.
            */

        sdssscr_acq_e_type      acq_type,
            /* Acquistion type that is attempted.
            */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{
  sdsr_s_type       *sr_ptr;
  boolean           is_sys_conf = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT( pos != SDSR_POS_NULL );
  SD_ASSERT_ENUM_IS_INRANGE( acq_type, SDSSSCR_ACQ_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the system in question.
  */
  sr_ptr = sdsr_list_get_ptr( list, pos );
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Assume 2nd strongest AMPS control channel does not need to be attempted
  ** acquisition.
  */
  rtrn_iact_ptr->prm.acq.is_amps_2nd_cch = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the mode of the system in question is other than AMPS, return FALSE.
  *//*lint -e774 *//* Suppress the if statement is always true */
  if( sr_ptr == NULL ||
      sr_ptr->sys.mode != SD_MODE_AMPS )/*lint +e774 */
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If 2nd strongest AMPS control channel was already attempted acquisition,
  ** return FALSE.
  */
  if( acq_ptr->is_amps_2nd_cch )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Decide whether the system conflicts with the specified acquisition type.
  */
  switch( acq_type )
  {
    case SDSSSCR_ACQ_SPECIFIC:

      /* If acquisition type is SPECIFIC, indicate whether the SID/NID that
      ** was obtained during acquisition conflicts the expected SID/NID.
      */
      if( ! sdss_sr_is_sid_match_exp_sid(sr_ptr) )
      {
        is_sys_conf = TRUE;
        SD_MSG_HIGH( "SID/PLMN mismatched for specific acq", 0,0,0 );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_ACQ_SPECIFIC_PRL:

      /* If acquisition type is SPECIFIC_PRL, indicate whether the SID/NID
      ** that was obtained during acquisition conflicts the expected SID/NID
      ** or is forbidden by the PRL.
      */
      if( ! sdss_sr_is_sid_match_exp_sid(sr_ptr) ||
          sdss_sr_is_prl(sr_ptr, SDSS_PRL_VAL_FORBID) )
      {
        is_sys_conf = TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_ACQ_SPECIFIC_PREF:

      /* If acquisition type is SPECIFIC_PREF, indicate whether the SID that
      ** was obtained during acquisition conflicts the expected SID,
      ** conflicts with the SS-Preference or is forbidden by the PRL.
      */
      if( ! sdss_sr_is_sid_match_exp_sid(sr_ptr)
                          ||
          sdss_sr_is_pref_conf0(acq_ptr->ss_ptr,
                                sr_ptr,
                                SDSS_PREF_CONF_ANY)
                          ||
          sdss_sr_is_prl(sr_ptr, SDSS_PRL_VAL_FORBID) )
      {
        is_sys_conf = TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_ACQ_COLLOC_PREF:
    case SDSSSCR_ACQ_COLLOC_MORE_PREF:
    case SDSSSCR_ACQ_COLLOC_BEST_PREF:
    case SDSSSCR_ACQ_ANY:
    case SDSSSCR_ACQ_ALLOWED:
    case SDSSSCR_ACQ_PREFERRED:
    case SDSSSCR_ACQ_BEST_PREF:
    case SDSSSCR_ACQ_MORE_PREF:
    case SDSSSCR_ACQ_MORE_PREF_THAN_SRV_SYS:
    case SDSSSCR_ACQ_NOT_LESS_PREF:
    case SDSSSCR_ACQ_NOT_LESS_PREF_OR_HOME:
    case SDSSSCR_ACQ_SAME_AS_LAST:
    case SDSSSCR_ACQ_MAX:
    default:

      /* For all other acquisition types indicate whether the acquired system
      ** conflicts with the SS-Preference or is forbidden by the PRL.
      */
      if( sdss_sr_is_pref_conf0(acq_ptr->ss_ptr,
                                sr_ptr,
                                SDSS_PREF_CONF_ANY)
                            ||
          sdss_sr_is_prl(sr_ptr, SDSS_PRL_VAL_FORBID) )
      {
        is_sys_conf = TRUE;
      }
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If acquired system does conflicts with the acquisition type, return
  ** FALSE.
  */
  if( ! is_sys_conf )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, instruct SS-Client to attempt 2nd strongest AMPS control
  ** channel.
  */
  acq_ptr->is_amps_2nd_cch                = TRUE;

  rtrn_iact_ptr->act                      = SDSS_IACT_ACQUIRE;
  rtrn_iact_ptr->prm.acq.list             = list;
  rtrn_iact_ptr->prm.acq.pos              = pos;
  rtrn_iact_ptr->prm.acq.is_amps_2nd_cch  = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* sdss_acq_is_amps_2nd_cch */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_acq_next

DESCRIPTION
  Select the next system to be attempted acquisition in the context of the
  SDSS_ACQ_STT_START state and instruct the caller of the next
  SS-Internal-Action to take.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the entire list that is currently being associated with the
  acquisition state machine is exhausted. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                 sdss_acq_next(

        sdss_acq_s_type         *acq_ptr,
            /* Pointer an acquisition state machine to process the SS-Event.
            */

        sd_ss_mode_pref_e_type  mode_pref,
            /* Current mode preference.
            */

        sd_ss_band_pref_e_type  band_pref,
            /* Current band preference.
            */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{
  boolean           list_is_exhausted = FALSE;
  byte              new_acq_cnt;
  sdsr_s_type       *sr_ptr           = NULL;
  int               next_pos;
  sys_wlan_bss_e_type  bss_type_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  new_acq_cnt = acq_ptr->new_acq_cnt;
  bss_type_pref = acq_ptr->ss_ptr->bss_type_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If acquisition time is expired, indicate that acquisition list traversal
  ** is over.
  */
   {
     qword time_now;
     dword uptime = clk_uptime();
     /* Read the current time
     */
     clk_uptime_ms( time_now );

     SD_MSG_HIGH(" Time: clk_uptime %d , ms: %d, acq_start_time: %d",
                    uptime,(QW_CVT_Q2N(time_now) % 1000 ),
                    acq_ptr->acq_max_uptime );

     /* Lint complains about suspicious cast, time_now is of type qword and
     ** we are casting it to uint64 and then taking the modulus of the
     ** resulting number
     */
     /*lint -e740 */
     if( (QW_CVT_Q2N(time_now) % 1000 ) >= 500 )
     /*lint +e740 */
     {
       uptime++;
     }

    if( uptime > acq_ptr->acq_max_uptime )
    {
      list_is_exhausted = TRUE;
      return list_is_exhausted;
    }
   }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether the schedule of any of the systems that are on a
  ** reacquisition schedule (i.e. on the sched list) is calling for
  ** reacquisition attempt at the current time. If so, attempt to acquire
  ** such a system.
  */
  acq_ptr->sched_pos = sdss_sr_list_sched_proc( acq_ptr->sched_lst,
                                                mode_pref,
                                                band_pref,
                                                bss_type_pref );

  /* If the count or time has expired, return list is exhausted
  */

  if( acq_ptr->sched_pos != SDSR_POS_NULL )
  {

    rtrn_iact_ptr->act           = SDSS_IACT_ACQUIRE;
    rtrn_iact_ptr->prm.acq.list  = acq_ptr->sched_lst;
    rtrn_iact_ptr->prm.acq.pos   = acq_ptr->sched_pos;

    /* Get the mode of the scheduled acquisition.
    */
    sr_ptr = sdsr_list_get_ptr( acq_ptr->sched_lst, acq_ptr->sched_pos );
    if( sr_ptr == NULL )
    {
      SD_MSG_ERROR("sr_ptr NULL",0,0,0);
      return list_is_exhausted;
    }
    rtrn_iact_ptr->prm.acq.mode  = (sdss_acq_mode_e_type)
                                                  sr_ptr->sched_prm.acq.mode;

    ///* Set the new acquisition counter of the system that is attempted
    //** acquisition per the current value of the acquisition counter.
    //*/
    //sr_ptr->new_acq_cnt = acq_ptr->new_acq_cnt;

    return list_is_exhausted;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no schedule is calling for reacquisition attempt at
  ** the current time, so check whether acquisition list traversal should
  ** continue.
  */

  /* Decrement the number of acquisitions left - if it is < 0, indicate that
  ** acquisition list traversal is over.
  */
  acq_ptr->acq_left--;
  if( acq_ptr->acq_left < 0 )
  {
    list_is_exhausted = TRUE;

    return list_is_exhausted;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, acquisition list traversal should continue.
  */

  /* If acquisition construct is ACQ_ALL, ignore the new_acq_count by
  ** setting it to 0.
  */
  if( acq_ptr->acq_con == SDSSSCR_CON_ACQ_ALL )
  {
    new_acq_cnt = 0;
  }

  /* If acquisition type is SPECIFIC, get next system regardless of the
  ** current mode and band preference.
  */
  if( acq_ptr->acq_type == SDSSSCR_ACQ_SPECIFIC ||
      acq_ptr->acq_type == SDSSSCR_ACQ_SPECIFIC_PRL )
  {
    mode_pref = SD_SS_MODE_PREF_ANY;
    band_pref = SD_SS_BAND_PREF_ANY;
    bss_type_pref = SYS_WLAN_BSS_TYPE_ANY;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find next system on the acquisition list that complies with the current
  ** mode and band preference.
  */
  next_pos = sdss_sr_list_find_next( acq_ptr->ss,
                                     acq_ptr->acq_lst,
                                     acq_ptr->acq_pos+1,
                                     new_acq_cnt,
                                     acq_ptr->meas_val,
                                     mode_pref,
                                     band_pref,
                                     bss_type_pref,
                                     acq_ptr->meas_cat );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If acquisition start position != 0 and list traversal reached the end of
  ** the list, loop back to start of list.
  */
  if( acq_ptr->acq_start_pos != 0 &&
      next_pos == SDSR_POS_NULL &&
      ! acq_ptr->is_acq_lst_end_reached )
  {
    acq_ptr->is_acq_lst_end_reached = TRUE;

    next_pos = sdss_sr_list_find_next( acq_ptr->ss,
                                       acq_ptr->acq_lst,
                                       0,
                                       new_acq_cnt,
                                       acq_ptr->meas_val,
                                       mode_pref,
                                       band_pref,
                                       bss_type_pref,
                                       acq_ptr->meas_cat );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If list is exhausted, indicate so.
  */
  if( next_pos == SDSR_POS_NULL
                     ||
      (acq_ptr->is_acq_lst_end_reached &&
       next_pos >= acq_ptr->acq_start_pos) )
  {
    list_is_exhausted = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If list is not exhausted, attempt to acquire the next system. Else set
  ** the acquisition end position to one before the acquisition start
  ** position.
  */
  if( ! list_is_exhausted )
  {
    acq_ptr->acq_pos              = next_pos;

    rtrn_iact_ptr->act            = SDSS_IACT_ACQUIRE;
    rtrn_iact_ptr->prm.acq.list   = acq_ptr->acq_lst;
    rtrn_iact_ptr->prm.acq.pos    = acq_ptr->acq_pos;

    if( acq_ptr->acq_mode == SDSS_ACQ_MODE_HO )
    {
      rtrn_iact_ptr->prm.acq.mode           = SDSS_ACQ_MODE_FULL;
      rtrn_iact_ptr->prm.acq.is_handoff_acq = TRUE;
    }
    else
    {
      rtrn_iact_ptr->prm.acq.mode           = acq_ptr->acq_mode;
      rtrn_iact_ptr->prm.acq.is_handoff_acq = FALSE;
    }

    ///* Set the new acquisition counter of the system that is attempted
    //** acquisition per the current value of the acquisition counter.
    //*/
    //sr_ptr = sdsr_list_get_ptr( acq_ptr->acq_lst, acq_ptr->acq_pos );
    //SD_ASSERT( sr_ptr != NULL );
    //sr_ptr->new_acq_cnt = acq_ptr->new_acq_cnt;
  }
  else
  {
    sdss_sr_list_set_acq_end_pos( acq_ptr->ss_ptr,
                                  acq_ptr->acq_lst,
                                  acq_ptr->acq_mode,
                                  acq_ptr->acq_start_pos );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Instruct the caller whether the entire acquisition list is exhausted.
  */
  return list_is_exhausted;

} /* sdss_acq_next */

/*===========================================================================

FUNCTION sdss_eng_scr_con_acq_fail

DESCRIPTION
  Indicate that acquisition over all systems has failed.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_scr_con_acq_fail(

        sd_ss_e_type              ss
            /* System selection item.
            */

)
{
  /* ACQ_FAIL - Indicate that acquisition over all systems has failed.
  */
  sd_si_acq_fail( ss );

  /* Cancel the avoidance of which shedule is set to 
  ** SDSSSCR_SCHED_AVOID_UNTIL_ACQ_FAIL.
  */
  sdss_sr_list_avoid( ss, 
                      SDSR_TBL_LST, 
                      0, 
                      SDSSSCR_SCHED_AVOID_UNTIL_ACQ_FAIL );

  if( sdss_is_orig_mode( ss, SD_SS_ORIG_MODE_VAL_ORIG ) )
  {
    /* Cancel the avoidance of which shedule is set to 
    ** SDSSSCR_SCHED_AVOID_UNTIL_ACQ_FAIL.
    */
    sdss_sr_list_avoid( ss, 
                        SDSR_TBL_LST, 
                        0, 
                        SDSSSCR_SCHED_AVOID_UNTIL_ACQ_FAIL_DUR_CALL_ORIG );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_acq_start_event_proc

DESCRIPTION
  Process SS-Event in the context of an SDSS_ACQ_STT_START state and instruct
  the caller of the next SS-Internal-Action to take.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the entire list that is currently being associated with the
  acquisition state machine is exhausted. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                 sdss_acq_start_event_proc(

        sdss_acq_s_type         *acq_ptr,
            /* Pointer an acquisition state machine to process the SS-Event.
            */

        dword                   *scr_timer_ptr,
            /* Script timer
            */

        sdss_evt_e_type         event,
            /* SS-Event to process.
            */

        sd_ss_mode_pref_e_type  mode_pref,
            /* Current mode preference.
            */

        sd_ss_band_pref_e_type  band_pref,
            /* Current band preference.
            */

        sd_ss_roam_pref_e_type  roam_pref,
            /* Current roam preference.
            */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
/*lint -esym(715, roam_pref, scr_timer_ptr) */ /* not referenced */
/*lint -esym(818, scr_timer_ptr) */ /* Could be declared as pointing to const
                                    */
{
  sdsr_s_type           *acq_sys_ptr;
  sdsr_s_type           *srv_sys_ptr;
  sdsr_s_type           *colloc_srv_sys_ptr;
  sdssscr_acq_e_type    acq_type;
  sys_wlan_bss_e_type   bss_type_pref;
  sd_ss_e_type          ss;

  boolean               is_no_pref_conf;
  boolean               list_is_exhausted       = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( roam_pref, SD_SS_ROAM_PREF_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );
  SD_ASSERT( acq_ptr->ss_ptr != NULL );

  bss_type_pref = acq_ptr->ss_ptr->bss_type_pref;
  ss = acq_ptr->ss;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* First try to get the information of the system that is being attempted
  ** acquisition in this state.
  */
  acq_sys_ptr = sdsr_list_get_ptr( ACQ_LIST(SDSR_ACQ_SYS), 0 );
  if( acq_sys_ptr == NULL )
  {
    SD_ERR_FATAL( "get_ptr SDSR_ACQ_SYS",0,0,0 ); /*lint -e527 */
    return FALSE;/*lint +e527 */ /* Unreachable */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If sched position is not SDSR_POS_NULL (i.e., we try acquiring a system
  ** from the sched_lst), set the acquisition type per the reacquisition
  ** schedule. Else, set the acquisition type per the list that is attempted
  ** acquisition.
  */
  if( acq_ptr->sched_pos != SDSR_POS_NULL )
  {
    acq_type = (sdssscr_acq_e_type)   acq_sys_ptr->sched_prm.acq.type;
  }
  else
  {
    acq_type = acq_ptr->acq_type;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do acquisition processing in accordance with the requested acquisition
  ** type and SS-Event.
  */
  switch( event )  /*lint -e788 */ /* sparse switch */
  {

    case SDSS_EVT_AMPS_ACQ_FAILED_CCH:

      /* AMPS acquisition attempt failed.
      */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If MS failed to acquire 2nd strongest control channel and
      ** acquisition type is BEST_PREF and the SID that was obtained from the
      ** strongest control channel is listed in the PRL, check whether there
      ** are more preferred systems in the same GEO.
      */
      if( acq_ptr->is_amps_2nd_cch &&
          acq_type == SDSSSCR_ACQ_BEST_PREF &&
          sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_LISTED) )
      {
        /* Search for a more preferred system that complies with the
        ** current mode and band preference.
        */
        acq_ptr->pref_pos = sdss_sr_list_find_next( acq_ptr->ss,
                                                    acq_ptr->pref_lst,
                                                    0,
                                                    0,
                                                    -10000,
                                                    mode_pref,
                                                    band_pref,
                                                    bss_type_pref,
                                                    acq_ptr->meas_cat );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If a more preferred system is found, go to the MORE_PREF
        ** state to try acquiring the more preferred system.
        */
        if( acq_ptr->pref_pos != SDSR_POS_NULL )
        {
          acq_ptr->state = SDSS_ACQ_STT_MORE_PREF;
        }
      }

      /* Acquisition attempt failed, try acquiring the next system that
      ** complies with the current mode and band preference from the list .
      */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_CDMA_ACQ_FAILED_PILOT:
    case SDSS_EVT_CDMA_ACQ_FAILED_SYNC:
    case SDSS_EVT_CDMA_ACQ_PROT_MIS:

    case SDSS_EVT_HDR_ACQ_FAILED_PILOT:
    case SDSS_EVT_HDR_ACQ_FAILED_SYNC:
    case SDSS_EVT_HDR_ACQ_PROT_MIS:
    case SDSS_EVT_HDR_ACQ_BAD_SYS:
    case SDSS_EVT_WLAN_ACQ_BAD_SYS:
    case SDSS_EVT_WLAN_ACQ_ATTACH_FAILED:

      /* Acquisition attempt failed.
      */

      /* Try acquiring the next system that complies with the current mode
      ** and band preference from the list .
      */

      break;

    case SDSS_EVT_WLAN_ACQ_SCAN_FAILED:


      #ifdef FEATURE_WLAN_UNIFORM_SCAN_OOS
#error code not present
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GW_ACQ_PLMN_FAILED:
    /*lint -esym(529, ss_ptr) */ /* not referenced */
      {
        sdss_s_type *ss_ptr  = sdss_ptr( SD_SS_MAIN );
        /* ACQ_PLMN failed. So set the default srv_req_type to NORMAL.
           This is to distinguish between any existing request with
           same parameters. 
        */
        ss_ptr->srv_req_type = SD_SS_SRV_REQ_TYPE_NORMAL;

      #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
       #endif
      }
      /*lint +esym(529, ss_ptr) */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_CDMA_ACQ_SCHM:
    case SDSS_EVT_AMPS_ACQ_CCH_SID:
    case SDSS_EVT_HDR_ACQ_SID:
    case SDSS_EVT_GW_ACQ_PLMN_SUCCESS:
    case SDSS_EVT_WLAN_ACQ_ATTACH_OK:
      /* Acquisition attempt succeeded.
      */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* ACQ_PLMN Success. So set the default srv_req_type back to NORMAL.
         This is to distinguish between any existing request with
         same parameters.
      */
      acq_ptr->ss_ptr->srv_req_type = SD_SS_SRV_REQ_TYPE_NORMAL;

      /* Copy the acquired system and its more preferred list into start_sys
      ** and pref_lst, respectively.
      */
      sdsr_list_cpy( acq_ptr->start_sys, ACQ_LIST(SDSR_ACQ_SYS) );
      sdsr_list_cpy( acq_ptr->pref_lst, ACQ_LIST(SDSR_ACQ_PREF_LST) );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Find out whether the acquired system conflicts with any of the
      ** system selection preferences.
      */
      is_no_pref_conf = sdss_sr_is_pref_conf0( acq_ptr->ss_ptr,
                                               acq_sys_ptr,
                                               SDSS_PREF_CONF_NONE );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Decide on the next state according to the requested acquisition
      ** type, PRL designation and SS-preference conflict.
      */
      switch( acq_type )
      {
        case SDSSSCR_ACQ_ANY:

          /* If acquired system does not conflict with the SS-Preference,
          ** stay on the acquired system.
          */
          if( is_no_pref_conf )
          {
            acq_ptr->state = SDSS_ACQ_STT_DONE;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSSSCR_ACQ_ALLOWED:

          /* If acquired system does not conflict with the current
          ** SS-Preferences and it satisfies the ALLOWED acquisition type,
          ** stay on the acquired system.
          */
          if( is_no_pref_conf &&
              sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ALLOWED) )
          {
            acq_ptr->state = SDSS_ACQ_STT_DONE;
          }
          #ifdef FEATURE_UNIFORM_SCAN_OOS
          /* If the system is forbidden, report acq done
          */
          else if( sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_FORBID) )
          {
            list_is_exhausted = TRUE;
            return list_is_exhausted;
          }
          #endif
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSSSCR_ACQ_PREFERRED:

          /* If acquired system does not conflict with the current
          ** SS-Preference and is listed as preferred system in the PRL,
          ** stay on the acquired system.
          */
          if( is_no_pref_conf &&
              sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ANY_PREF) )
          {
            acq_ptr->state = SDSS_ACQ_STT_DONE;
          }
          #ifdef FEATURE_UNIFORM_SCAN_OOS
          /* If the system is forbidden, report acq done
          */
          else if( sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_FORBID) )
          {
            list_is_exhausted = TRUE;
            return list_is_exhausted;
          }
          #endif
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSSSCR_ACQ_BEST_PREF:

          /* If acquired system does not conflict with the current
          ** SS-Preference and it is the most preferred in its GEO,
          ** stay on the acquired system.
          */
          if( is_no_pref_conf &&
              sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_MOST_PREF) )
          {
            acq_ptr->state = SDSS_ACQ_STT_DONE;
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Else if AMPS 2nd strongest control channel needs to be attempted
          ** acquisition, return now.
          */
          else if( sdss_acq_is_amps_2nd_cch(acq_ptr,
                                            ACQ_LIST(SDSR_ACQ_SYS),
                                            0,
                                            acq_type,
                                            rtrn_iact_ptr) )
          {
            return list_is_exhausted;
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          #ifdef FEATURE_UNIFORM_SCAN_OOS
          /* If the system is forbidden, report acq done
          */
          else if( sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_FORBID) )
          {
            list_is_exhausted = TRUE;
            return list_is_exhausted;
          }
          #endif
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Else, if the acquired system is a PRL listed system, check
          ** whether there are more preferred systems in the same GEO.
          */
          else if( sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_LISTED) )
          {
            /* Search for a more preferred system that complies with the
            ** current mode and band preference.
            */
            acq_ptr->pref_pos  =  sdss_sr_list_find_next(acq_ptr->ss,
                                                         acq_ptr->pref_lst,
                                                         0,
                                                         0,
                                                         -10000,
                                                         mode_pref,
                                                         band_pref,
                                                         bss_type_pref,
                                                         acq_ptr->meas_cat );

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            /* If a more preferred system is found, go to the MORE_PREF
            ** state to try acquiring the more preferred system.
            */
            if( acq_ptr->pref_pos != SDSR_POS_NULL )
            {
              acq_ptr->state = SDSS_ACQ_STT_MORE_PREF;
            }

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            /* Else (a more preferred system is not found), if the acquired
            ** acquired system does not conflict with the current
            ** SS-Preference and it is a preferred PRL system, stay on this
            ** system.
            */
            else if( is_no_pref_conf &&
                     sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ANY_PREF) )
            {
              acq_ptr->state = SDSS_ACQ_STT_DONE;
            }

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            /* Else, stay in the start state and attempt acquire the
            ** next system from the acquisition list.
            */
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Else, stay in the start state and attempt acquire the
          ** next system from the acquisition list.
          */
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSSSCR_ACQ_MORE_PREF:
        case SDSSSCR_ACQ_MORE_PREF_THAN_SRV_SYS:
        case SDSSSCR_ACQ_NOT_LESS_PREF:
        case SDSSSCR_ACQ_NOT_LESS_PREF_OR_HOME:

          /* If acquired system does not conflict with the current
          ** SS-Preference and it is more/not less preferred than the serving
          ** system, stay on the acquired system.
          */
          if( is_no_pref_conf )
          {
            if( acq_type == SDSSSCR_ACQ_MORE_PREF_THAN_SRV_SYS )
            {
              srv_sys_ptr = sdsr_list_get_ptr(ACQ_LIST(SDSR_SRV_SYS), 0);
            }
            else
            {
              srv_sys_ptr = sdsr_list_get_ptr(ACQ_LIST(SDSR_IDL_SYS), 0);
            }

            /* If we don't have a serving system, and the acquired system is
            ** allowed by the PRL, stay on the acquired system.
            */
            if( srv_sys_ptr == NULL )
            {
              if( sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ALLOWED) )
              {
                acq_ptr->state = SDSS_ACQ_STT_DONE;
              }
            }

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            /* Else (we have a serving system), if acq_type is MORE_PREF and
            ** the acquired system is more preferred than the serving system,
            ** or acq_type is NOT_LESS_PREF and the acquired system is not
            ** less preferred than the serving system, stay on the acquired
            ** system.
            */
            else if( ((acq_type == SDSSSCR_ACQ_MORE_PREF ||
                       acq_type == SDSSSCR_ACQ_MORE_PREF_THAN_SRV_SYS) &&
                      sdprl_is_more_pref(ss, acq_sys_ptr, srv_sys_ptr)
                     )
                                      ||
                     (acq_type == SDSSSCR_ACQ_NOT_LESS_PREF &&
                      !sdprl_is_more_pref_upto(ss, srv_sys_ptr, acq_sys_ptr,
                                               SDPRL_COMP_UPTO_ROAM)
                     )
                                      ||
                     (acq_type == SDSSSCR_ACQ_NOT_LESS_PREF_OR_HOME &&
                      (sdprl_roam_ind_map_custom_home_to_home(acq_sys_ptr->roam)
                                                 == SYS_ROAM_STATUS_OFF ||
                       !sdprl_is_more_pref_upto(ss, srv_sys_ptr, acq_sys_ptr,
                                               SDPRL_COMP_UPTO_ROAM)
                      )
                     )
                   )
            {
              acq_ptr->state = SDSS_ACQ_STT_DONE;
            }

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            /* Else, stay in the start state and attempt acquire the
            ** next system from the acquisition list.
            */
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Else, stay in the start state and attempt acquire the
          ** next system from the acquisition list.
          */
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSSSCR_ACQ_SPECIFIC:

          /* If the SID/NID that was obtained during the acquisition matches
          ** the expected SID/NID, stay on the acquired system.
          */
          if( sdss_sr_is_sid_match_exp_sid(acq_sys_ptr) )
          {
            acq_ptr->state = SDSS_ACQ_STT_DONE;
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Else, stay in the start state and attempt acquire the next
          ** system from the acquisition list.
          */

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSSSCR_ACQ_SPECIFIC_PRL:

          /* If the SID/NID that was obtained during the acquisition matches
          ** the expected SID/NID and is allowed by the PRL, stay on the
          ** acquired system.
          */
          if( sdss_sr_is_sid_match_exp_sid(acq_sys_ptr) &&
              sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ALLOWED) )
          {
            acq_ptr->state = SDSS_ACQ_STT_DONE;
          }

          /* Else, stay in the start state and attempt acquire the
          ** next system from the acquisition list.
          */
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSSSCR_ACQ_SPECIFIC_PREF:

          /* If the SID/NID that was obtained during the acquisition does not
          ** conflict with the current SS-Preference, matches the expected
          ** SID/NID and is allowed by the PRL, stay on the acquired system.
          */
          if( is_no_pref_conf &&
              sdss_sr_is_sid_match_exp_sid(acq_sys_ptr) &&
              sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ALLOWED) )
          {
            acq_ptr->state = SDSS_ACQ_STT_DONE;
          }

          /* Else, stay in the start state and attempt acquire the
          ** next system from the acquisition list.
          */
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSSSCR_ACQ_COLLOC_PREF:

          /* If acquired system does not conflict with the current
          ** SS-Preference, is collocated (associated) with the SDSR_IDL_SYS
          ** system and is allowed by the PRL, stay on the acquired
          ** system.
          */
          srv_sys_ptr = sdsr_list_get_ptr( SDSR_IDL_SYS, 0 );

          if( is_no_pref_conf
                      &&
              (srv_sys_ptr == NULL ||
               sdprl_is_associated(acq_sys_ptr, srv_sys_ptr))
                      &&
               sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ANY_PREF) )
          {
            acq_ptr->state = SDSS_ACQ_STT_DONE;
          }

          /* Else, stay in the start state and attempt acquire the
          ** next system from the acquisition list.
          */
          break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_ACQ_COLLOC_MORE_PREF:

        /* If acquired system does not conflict with the current
        ** SS-Preference, is collocated (associated) with the SDSR_IDL_SYS
        ** system and is allowed by the PRL, stay on the acquired
        ** system and and not less preferred than the idle system
        */
        srv_sys_ptr = sdsr_list_get_ptr( SDSR_IDL_SYS, 0 );
        colloc_srv_sys_ptr = sdsr_list_get_ptr(ACQ_LIST(SDSR_IDL_SYS), 0);


        if( is_no_pref_conf
                   &&
            (srv_sys_ptr == NULL ||
             sdprl_is_associated(acq_sys_ptr, srv_sys_ptr))
                   &&
             sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ALLOWED)
                   &&
            (colloc_srv_sys_ptr == NULL ||
             sdprl_is_more_pref(ss, acq_sys_ptr, colloc_srv_sys_ptr)) )
        {
          acq_ptr->state = SDSS_ACQ_STT_DONE;
        }

        /* Else, stay in the start state and attempt acquire the
        ** next system from the acquisition list.
        */
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_ACQ_COLLOC_BEST_PREF:

         /* If acquired system does not conflict with the current
         ** SS-Preference, is collocated (associated) with the SDSR_IDL_SYS
         ** system and is allowed by the PRL, stay on the acquired
         ** system.
         */
         srv_sys_ptr = sdsr_list_get_ptr( SDSR_IDL_SYS, 0 );

        /* If acquired system does not conflict with the current
        ** SS-Preference and it is the most preferred in its GEO,
        ** and it is associated (collocated ) with the SDSR_IDL_SYS system,
        ** stay on the acquired system.
        */
        if( is_no_pref_conf
                    &&
            (srv_sys_ptr == NULL ||
             sdprl_is_associated(acq_sys_ptr, srv_sys_ptr))
                    &&
            sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_MOST_PREF) )
        {
          acq_ptr->state = SDSS_ACQ_STT_DONE;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Else, if the acquired system is a PRL listed system, and is
        ** associated (collocated ) with SDSR_IDL_SYS system, check
        ** whether there are more preferred systems in the same GEO.
        */
        else if( sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_LISTED)
                          &&
                 (srv_sys_ptr == NULL ||
                  sdprl_is_associated(acq_sys_ptr, srv_sys_ptr)))
        {
          /* Search for a more preferred system that complies with the
          ** current mode and band preference.
          */
          acq_ptr->pref_pos  =  sdss_sr_list_find_next(acq_ptr->ss,
                                                       acq_ptr->pref_lst,
                                                       0,
                                                       0,
                                                       -10000,
                                                       mode_pref,
                                                       band_pref,
                                                       bss_type_pref,
                                                       acq_ptr->meas_cat );

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* If a more preferred system is found, go to the MORE_PREF
          ** state to try acquiring the more preferred system.
          */
          if( acq_ptr->pref_pos != SDSR_POS_NULL )
          {
            acq_ptr->state = SDSS_ACQ_STT_MORE_PREF;
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Else (a more preferred system is not found), if the acquired
          ** acquired system does not conflict with the current
          ** SS-Preference, it is a preferred PRL system and it is associated
          ** (collocated) with the SDSR_IDL_SYS system, stay on this
          ** system.
          */
          else if( is_no_pref_conf
                           &&
                   (srv_sys_ptr == NULL ||
                    sdprl_is_associated(acq_sys_ptr, srv_sys_ptr))
                           &&
                   sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ANY_PREF) )
          {
            acq_ptr->state = SDSS_ACQ_STT_DONE;
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Else, stay in the start state and attempt acquire the
          ** next system from the acquisition list.
          */
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Else, stay in the start state and attempt acquire the
        ** next system from the acquisition list.
        */
        break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSSSCR_ACQ_SAME_AS_LAST:
        case SDSSSCR_ACQ_MAX:
        default:
          SD_CASE_ERR( "acq_type",0,0,0 );
          break;

      } /* switch( acq_type ) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If state is START and AMPS 2nd strongest control channel needs to be
      ** attempted acquisition, return now.
      */
      if( acq_ptr->state == SDSS_ACQ_STT_START &&
          sdss_acq_is_amps_2nd_cch(acq_ptr,
                                   ACQ_LIST(SDSR_ACQ_SYS),
                                   0,
                                   acq_type,
                                   rtrn_iact_ptr) )
      {
        return list_is_exhausted;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If this is a scheduled reacquisition and acq_state != DONE (i.e., we
      ** have reacquired the system, but decided to continue attempting
      ** acquisition of other systems), cancel any pending reacquisition
      ** attempts of the acquired system by deleting it from the schedule
      ** list.
      */
      if( acq_ptr->sched_pos != SDSR_POS_NULL &&
          acq_ptr->state != SDSS_ACQ_STT_DONE )
      {
        (void) sdsr_list_del_ref( acq_ptr->sched_lst,
                                  sdsr_list_get_ref( ACQ_LIST(SDSR_ACQ_SYS),
                                                     0) );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      /* If we got here, this event has no impact on the acquisition
      ** processing, so just continue.
      */
      rtrn_iact_ptr->act = SDSS_IACT_CONTINUE;
      SD_MSG_MED( "Ignoring event=%d",event,0,0 );

      return list_is_exhausted;

  } /* switch( event ) */ /*lint +e788 */ /* sparse switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #endif
      /* Set the new acquisition counter of the system that was attempted
      ** acquisition per the current value of the acquisition counter.
      */
      acq_sys_ptr->new_acq_cnt = acq_ptr->new_acq_cnt;
      sdsr_list_del_list(SDSR_PWR_SCAN_LST, SDSR_ACQ_SYS );
  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #endif

  acq_sys_ptr->new_acq_cnt = acq_ptr->new_acq_cnt;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate that AMPS 2nd strongest control channel is not attempted
  ** acquisition.
  */
  acq_ptr->is_amps_2nd_cch                = FALSE;
  rtrn_iact_ptr->prm.acq.is_amps_2nd_cch  = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Based on the next state, adjust the acquisition position and build the
  ** return action.
  */
  switch( acq_ptr->state )
  {
    case SDSS_ACQ_STT_START:

      /* Get the next system to be attempted acquisition.
      */
      list_is_exhausted = sdss_acq_next( acq_ptr,
                                         mode_pref,
                                         band_pref,
                                         rtrn_iact_ptr );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_STT_MORE_PREF:

      /* Attempt to acquire a more preferred system.
      */
      rtrn_iact_ptr->act             = SDSS_IACT_ACQUIRE;
      rtrn_iact_ptr->prm.acq.list    = acq_ptr->pref_lst;
      rtrn_iact_ptr->prm.acq.pos     = acq_ptr->pref_pos;
      rtrn_iact_ptr->prm.acq.mode    = SDSS_ACQ_MODE_FULL;

      SD_MSG_HIGH( "Attempt acquiring preferred systems",0,0,0 );
      sdsr_list_print( acq_ptr->pref_lst, FALSE );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_STT_DONE:

      /* Stay on the acquired system.
      */
      rtrn_iact_ptr->act             = SDSS_IACT_CONTINUE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
    #else
    case SDSS_ACQ_STT_SCAN_CONT:
    case SDSS_ACQ_STT_PWR_SAVE_ENTER:
      SD_CASE_ERR( "state",0,0,0 );
      break;
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_STT_REACQ:
    case SDSS_ACQ_STT_PWR_SAVE:
    case SDSS_ACQ_STT_MAX:
    case SDSS_ACQ_STT_NONE:
    default:
      SD_CASE_ERR( "state",0,0,0 );
      break;
  }

  return list_is_exhausted;

} /* sdss_acq_start_event_proc */

/*lint +esym(715, roam_pref, scr_timer_ptr) */ /* not referenced */
/*lint +esym(818, scr_timer_ptr) */ /* Could be declared as pointing to const
                                    */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_acq_more_pref_event_proc

DESCRIPTION
  Process SS-Event in the context of an SDSS_ACQ_STT_MORE_PREF state and
  instruct the caller of the next SS-Internal-Action to take.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the entire list that is currently being associated with the
  acquisition state machine is exhausted. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                 sdss_acq_more_pref_event_proc(

        sdss_acq_s_type         *acq_ptr,
            /* Pointer an acquisition state machine to process the SS-Event.
            */

        sdss_evt_e_type         event,
            /* SS-Event to process.
            */

        sd_ss_mode_pref_e_type  mode_pref,
            /* Current mode preference.
            */

        sd_ss_band_pref_e_type  band_pref,
            /* Current band preference.
            */

        sd_ss_roam_pref_e_type  roam_pref,
            /* Current roam preference.
            *//*lint -esym(715, roam_pref) */ /* not referenced */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{

  sdsr_s_type                   *acq_sys_ptr;
  sdsr_s_type                   *start_sys_ptr;
  sdsr_s_type                   *srv_sys_ptr;
  boolean                       list_is_exhausted = FALSE;
  sys_wlan_bss_e_type   bss_type_pref;
  sd_ss_e_type          ss;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( roam_pref, SD_SS_ROAM_PREF_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );
  SD_ASSERT( acq_ptr->ss_ptr != NULL );

  bss_type_pref = acq_ptr->ss_ptr->bss_type_pref;
  ss = acq_ptr->ss;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* First get the information of the system being attempted acquisition in
  ** this state, as well as, the information of the last system being
  ** acquired in the SDSS_ACQ_STT_START state.
  */
  acq_sys_ptr = sdsr_list_get_ptr( ACQ_LIST(SDSR_ACQ_SYS), 0 );
  if( acq_sys_ptr == NULL )
  {
    SD_ERR_FATAL( "get_ptr SDSR_ACQ_SYS",0,0,0 );
  }

  start_sys_ptr = sdsr_list_get_ptr( acq_ptr->start_sys, 0 );
  if( start_sys_ptr == NULL )
  {
    SD_ERR_FATAL( "get_ptr START_SYS",0,0,0 );
  }

  srv_sys_ptr = sdsr_list_get_ptr( SDSR_IDL_SYS, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch( event ) /*lint -e788 */  /* sparse switch */
  {
    case SDSS_EVT_CDMA_ACQ_FAILED_PILOT:
    case SDSS_EVT_CDMA_ACQ_FAILED_SYNC:
    case SDSS_EVT_CDMA_ACQ_PROT_MIS:

    case SDSS_EVT_AMPS_ACQ_FAILED_CCH:

    case SDSS_EVT_HDR_ACQ_FAILED_PILOT:
    case SDSS_EVT_HDR_ACQ_FAILED_SYNC:
    case SDSS_EVT_HDR_ACQ_PROT_MIS:
    case SDSS_EVT_HDR_ACQ_BAD_SYS:
    case SDSS_EVT_GW_ACQ_PLMN_FAILED:
    case SDSS_EVT_WLAN_ACQ_SCAN_FAILED:
    case SDSS_EVT_WLAN_ACQ_ATTACH_FAILED:
      /* Acquisition attempt failed.
      */

      /* Try acquiring the next system that complies with the current mode
      ** and band preference from the more preferred list.
      */
      acq_ptr->pref_pos  =  sdss_sr_list_find_next( acq_ptr->ss,
                                                    acq_ptr->pref_lst,
                                                    acq_ptr->pref_pos+1,
                                                    0,
                                                    -10000,
                                                    mode_pref,
                                                    band_pref,
                                                    bss_type_pref,
                                                    acq_ptr->meas_cat );

      /* If the more preferred list is exhausted go back to the start state.
      */
      if( acq_ptr->pref_pos == SDSR_POS_NULL )
      {
        acq_ptr->state = SDSS_ACQ_STT_START;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_CDMA_ACQ_SCHM:
    case SDSS_EVT_AMPS_ACQ_CCH_SID:
    case SDSS_EVT_HDR_ACQ_SID:
    case SDSS_EVT_GW_ACQ_PLMN_SUCCESS:
    case SDSS_EVT_WLAN_ACQ_ATTACH_OK:
      /* Acquisition attempt succeeded.
      */

      /* If the acquired system is a preferred PRL system that complies with
      ** the current SS-Preference, is more preferred than the last system
      ** acquired in the start state and is collocated (associated) with
      ** SDSR_IDL_SYS system, stay and provide service on this
      ** system.
      */
      if( sdss_sr_is_prl(acq_sys_ptr,SDSS_PRL_VAL_ANY_PREF)
                          &&
          sdss_sr_is_pref_conf0(acq_ptr->ss_ptr,
                                acq_sys_ptr,
                                SDSS_PREF_CONF_NONE)
                          &&
          sdprl_is_more_pref(ss, acq_sys_ptr, start_sys_ptr)
                          &&
          (acq_ptr->acq_type != SDSSSCR_ACQ_COLLOC_BEST_PREF ||
           srv_sys_ptr == NULL                             ||
           sdprl_is_associated(acq_sys_ptr, srv_sys_ptr)) )
      {
        acq_ptr->state = SDSS_ACQ_STT_DONE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Else if AMPS 2nd strongest control channel needs to be attempted
      ** acquisition, return now.
      */
      else if( sdss_acq_is_amps_2nd_cch( acq_ptr,
                                         ACQ_LIST(SDSR_ACQ_SYS),
                                         0,
                                         SDSSSCR_ACQ_BEST_PREF,
                                         rtrn_iact_ptr) )
      {
          return list_is_exhausted;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Else, attempt acquiring the next system that complies with the
      ** current mode and band preference from the more_pref list.
      */
      else
      {
        acq_ptr->pref_pos  =  sdss_sr_list_find_next( acq_ptr->ss,
                                                      acq_ptr->pref_lst,
                                                      acq_ptr->pref_pos+1,
                                                      0,
                                                      -10000,
                                                      mode_pref,
                                                      band_pref,
                                                      bss_type_pref,
                                                      acq_ptr->meas_cat );

        /* If the more preferred list is exhausted go back to the start
        ** state.
        */
        if( acq_ptr->pref_pos == SDSR_POS_NULL )
        {
          acq_ptr->state = SDSS_ACQ_STT_START;
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      /* If we got here, this event has no impact on the acquisition
      ** processing, so just continue.
      */
      rtrn_iact_ptr->act = SDSS_IACT_CONTINUE;
      SD_MSG_MED( "Ignoring event=%d",event,0,0 );

      return list_is_exhausted;

  } /* switch( event ) */ /*lint +e788 */  /* sparse switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate that AMPS 2nd strongest control channel is not attempted
  ** acquisition.
  */
  acq_ptr->is_amps_2nd_cch                = FALSE;
  rtrn_iact_ptr->prm.acq.is_amps_2nd_cch  = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Based on the next state, adjust the acquisition position and build the
  ** return action.
  */
  switch( acq_ptr->state )
  {
    case SDSS_ACQ_STT_START:

      /* If the last system being acquired in the start state is a preferred
      ** system that complies with the current SS-Preference, and is
      ** associated (collocated) with SDSR_IDL_SYS system, go to the
      ** SDSS_ACQ_STT_REACQ state and attempt to reacquire that system.
      */
      if( sdss_sr_is_prl(start_sys_ptr, SDSS_PRL_VAL_ANY_PREF)
                           &&
          sdss_sr_is_pref_conf0(acq_ptr->ss_ptr,
                                start_sys_ptr,
                                SDSS_PREF_CONF_NONE)
                           &&
          (acq_ptr->acq_type != SDSSSCR_ACQ_COLLOC_BEST_PREF ||
           srv_sys_ptr == NULL                               ||
           sdprl_is_associated(start_sys_ptr, srv_sys_ptr)) )
      {
        acq_ptr->state               = SDSS_ACQ_STT_REACQ;

        rtrn_iact_ptr->act           = SDSS_IACT_ACQUIRE;
        rtrn_iact_ptr->prm.acq.list  = acq_ptr->start_sys;
        rtrn_iact_ptr->prm.acq.pos   = 0;
        rtrn_iact_ptr->prm.acq.mode  = SDSS_ACQ_MODE_FULL;

        SD_MSG_HIGH( "Attempt reacquire candidate system",0,0,0 );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Else, select the next system to be attempted acquisition in the
      ** start state.
      */
      else
      {
        list_is_exhausted = sdss_acq_next( acq_ptr,
                                           mode_pref,
                                           band_pref,
                                           rtrn_iact_ptr);
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_STT_MORE_PREF:

      /* Attempt to acquire the more preferred system.
      */
      rtrn_iact_ptr->act             = SDSS_IACT_ACQUIRE;
      rtrn_iact_ptr->prm.acq.list    = acq_ptr->pref_lst;
      rtrn_iact_ptr->prm.acq.pos     = acq_ptr->pref_pos;
      rtrn_iact_ptr->prm.acq.mode    = SDSS_ACQ_MODE_FULL;

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_STT_DONE:

      /* Stay on the acquired system.
      */
      rtrn_iact_ptr->act             = SDSS_IACT_CONTINUE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_STT_REACQ:
    case SDSS_ACQ_STT_PWR_SAVE_ENTER:
    case SDSS_ACQ_STT_PWR_SAVE:
    case SDSS_ACQ_STT_SCAN_CONT:
    case SDSS_ACQ_STT_MAX:
    case SDSS_ACQ_STT_NONE:
    default:
      SD_CASE_ERR( "state",0,0,0 );
      break;
  }

  return list_is_exhausted;

} /* sdss_acq_more_pref_event_proc */ /*lint +esym(715, roam_pref) */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_acq_reacq_event_proc

DESCRIPTION
  Process SS-Event in the context of an SDSS_ACQ_STT_REACQ state and instruct
  the caller of the next SS-Internal-Action to take.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the entire list that is currently being associated with the
  acquisition state machine is exhausted. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                 sdss_acq_reacq_event_proc(

        sdss_acq_s_type         *acq_ptr,
            /* Pointer an acquisition state machine to process the SS-Event.
            */

        sdss_evt_e_type         event,
            /* SS-Event to process.
            */

        sd_ss_mode_pref_e_type  mode_pref,
            /* Current mode preference.
            */

        sd_ss_band_pref_e_type  band_pref,
            /* Current band preference.
            */

        sd_ss_roam_pref_e_type  roam_pref,
            /* Current roam preference.
            *//*lint -esym(715, roam_pref) */ /* not referenced */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{

  sdsr_s_type       *acq_sys_ptr;
  sdsr_s_type       *srv_sys_ptr;
  boolean           list_is_exhausted = FALSE;
  sd_ss_e_type      ss                = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( roam_pref, SD_SS_ROAM_PREF_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined (FEATURE_TECH_IND_SD_STACK_ARCH) || defined(FEATURE_HDR_HYBRID))
  /* Work with HYBR_1 system selection, if specified so.
  */
  if( acq_ptr->ss_ptr == sdss_ptr(SD_SS_HYBR_1) )
  {
    ss = SD_SS_HYBR_1;
  }
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* First get the information of the system being attempted acquisition in
  ** this state.
  */
  acq_sys_ptr = sdsr_list_get_ptr( ACQ_LIST(SDSR_ACQ_SYS), 0 );

  if( acq_sys_ptr == NULL )
  {
    SD_ERR_FATAL( "get_ptr SDSR_ACQ_SYS",0,0,0 );
  }

  srv_sys_ptr = sdsr_list_get_ptr( SDSR_IDL_SYS, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( event ) /*lint -e788 */  /* sparse switch */
  {
    case SDSS_EVT_CDMA_ACQ_FAILED_PILOT:
    case SDSS_EVT_CDMA_ACQ_FAILED_SYNC:
    case SDSS_EVT_CDMA_ACQ_PROT_MIS:

    case SDSS_EVT_AMPS_ACQ_FAILED_CCH:

    case SDSS_EVT_HDR_ACQ_FAILED_PILOT:
    case SDSS_EVT_HDR_ACQ_FAILED_SYNC:
    case SDSS_EVT_HDR_ACQ_PROT_MIS:
    case SDSS_EVT_HDR_ACQ_BAD_SYS:
    case SDSS_EVT_GW_ACQ_PLMN_FAILED:
    case SDSS_EVT_WLAN_ACQ_SCAN_FAILED:
    case SDSS_EVT_WLAN_ACQ_ATTACH_FAILED:

      /* Reacquisition attempt failed, go back to the start state to continue
      ** traversing the list that is being attempted acquisition.
      */
      acq_ptr->state = SDSS_ACQ_STT_START;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_CDMA_ACQ_SCHM:
    case SDSS_EVT_AMPS_ACQ_CCH_SID:
    case SDSS_EVT_HDR_ACQ_SID:
    case SDSS_EVT_GW_ACQ_PLMN_SUCCESS:
    case SDSS_EVT_WLAN_ACQ_ATTACH_OK:

      /* Reacquisition attempt is successful. If this system is a preferred
      ** system that complies with the current SS-Preference, and is
      ** collocated with the SDSR_IDL_SYS system, stay and
      ** provide service on this system.
      */
      if( sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ANY_PREF)
                          &&
          sdss_sr_is_pref_conf0(acq_ptr->ss_ptr,
                                acq_sys_ptr,
                                SDSS_PREF_CONF_NONE)
                          &&
        (acq_ptr->acq_type != SDSSSCR_ACQ_COLLOC_BEST_PREF ||
         srv_sys_ptr == NULL                               ||
         sdprl_is_associated(acq_sys_ptr, srv_sys_ptr)) )
      {
        acq_ptr->state = SDSS_ACQ_STT_DONE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Else if AMPS 2nd strongest control channel needs to be attempted
      ** acquisition, return now.
      */
      else if( sdss_acq_is_amps_2nd_cch( acq_ptr,
                                         ACQ_LIST(SDSR_ACQ_SYS),
                                         0,
                                         SDSSSCR_ACQ_BEST_PREF,
                                         rtrn_iact_ptr) )
      {
          return list_is_exhausted;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Else, go back to the start state to continue traversing the list
      ** that is being attempted acquisition.
      */
      else
      {
        acq_ptr->state = SDSS_ACQ_STT_START;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      /* If we got here, this event has no impact on the acquisition
      ** processing, so just continue.
      */
      rtrn_iact_ptr->act = SDSS_IACT_CONTINUE;
      SD_MSG_MED( "Ignoring event=%d",event,0,0 );

      return list_is_exhausted;

  } /* switch( event ) */ /*lint +e788 */  /* sparse switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate that AMPS 2nd strongest control channel is not attempted
  ** acquisition.
  */
  acq_ptr->is_amps_2nd_cch                = FALSE;
  rtrn_iact_ptr->prm.acq.is_amps_2nd_cch  = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Based on the next state, adjust the acquisition position and build the
  ** return action.
  */
  switch( acq_ptr->state )
  {
    case SDSS_ACQ_STT_START:

      /* Reacquisition attempt failed - set the service-indication to OFF
      ** and select the next system to be attempted acquisition.
      */
      sd_si_srv_off( ss );
      list_is_exhausted = sdss_acq_next( acq_ptr,
                                         mode_pref,
                                         band_pref,
                                         rtrn_iact_ptr);

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_STT_DONE:

      /* Stay on the acquired system.
      */
      rtrn_iact_ptr->act             = SDSS_IACT_CONTINUE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_STT_MORE_PREF:
    case SDSS_ACQ_STT_REACQ:
    case SDSS_ACQ_STT_PWR_SAVE_ENTER:
    case SDSS_ACQ_STT_PWR_SAVE:
    case SDSS_ACQ_STT_SCAN_CONT:
    case SDSS_ACQ_STT_MAX:
    case SDSS_ACQ_STT_NONE:
    default:
      SD_CASE_ERR( "state",0,0,0 );
      break;
  }

  return list_is_exhausted;

} /* sdss_acq_reacq_event_proc *//*lint +esym(715, roam_pref) */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_acq_done_event_proc

DESCRIPTION
  Process SS-Event in the context of an SDSS_ACQ_STT_DONE state and instruct
  the caller of the next SS-Internal-Action to take.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the entire list that is currently being associated with the
  acquisition state machine is exhausted. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                 sdss_acq_done_event_proc(

        sdss_acq_s_type         *acq_ptr,
            /* Pointer an acquisition state machine to process the SS-Event.
            */

        sdss_evt_e_type         event,
            /* SS-Event to process.
            */

        sd_ss_mode_pref_e_type  mode_pref,
            /* Current mode preference.
            */

        sd_ss_band_pref_e_type  band_pref,
            /* Current band preference.
            */

        sd_ss_roam_pref_e_type  roam_pref,
            /* Current roam preference.
            *//*lint -esym(715, roam_pref) */ /* not referenced */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{

  boolean           list_is_exhausted = FALSE;
  sys_wlan_bss_e_type   bss_type_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( roam_pref, SD_SS_ROAM_PREF_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );
  SD_ASSERT( acq_ptr->ss_ptr != NULL );

  bss_type_pref = acq_ptr->ss_ptr->bss_type_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Event is a user or miscellaneous event, instruct the SS-Client to
  ** continue.
  **
  ** Note that user or miscellaneous events that require a change of policy
  ** are not expected to get here as they cause a new script to be activated.
  */
  if( SDSS_EVT_IS_USER(event) ||
      SDSS_EVT_IS_MISC(event) )
  {
    rtrn_iact_ptr->act = SDSS_IACT_CONTINUE;
    SD_MSG_MED( "Ignoring event=%d",event,0,0 );

    return list_is_exhausted;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the SS-Event is a protocol related event, so go back to
  ** the start/more-preferred state and select the next system to be
  ** attempted acquisition.
  **
  ** Note that protocol related events that require a change of policy are
  ** not expected to get here as they cause a new script to be activated.
  ** For some protocol events, however, like SDSS_EVT_AMPS_OPR_FAILED_PCH it
  ** might be desirable to continue with the current SS-Script.
  */
  if( acq_ptr->prev_state == SDSS_ACQ_STT_MORE_PREF )
  {
    /* Go back to more-preferred state.
    */
    acq_ptr->state = SDSS_ACQ_STT_MORE_PREF;

    /* Try acquiring the next system that complies with the current mode
    ** and band preference from the more preferred list.
    */
    acq_ptr->pref_pos  =  sdss_sr_list_find_next( acq_ptr->ss,
                                                  acq_ptr->pref_lst,
                                                  acq_ptr->pref_pos+1,
                                                  0,
                                                  -10000,
                                                  mode_pref,
                                                  band_pref,
                                                  bss_type_pref,
                                                  acq_ptr->meas_cat );

    /* If the more preferred list is exhausted, go back to the start state.
    */
    if( acq_ptr->pref_pos == SDSR_POS_NULL )
    {
      acq_ptr->state = SDSS_ACQ_STT_START;
    }
  }
  else
  {
    /* Go back to start state.
    */
    acq_ptr->state = SDSS_ACQ_STT_START;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate that AMPS 2nd strongest control channel is not attempted
  ** acquisition.
  */
  acq_ptr->is_amps_2nd_cch                = FALSE;
  rtrn_iact_ptr->prm.acq.is_amps_2nd_cch  = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Based on the next state, adjust the acquisition position and  build the
  ** return action.
  */
  switch( acq_ptr->state )
  {
    case SDSS_ACQ_STT_START:

      /* Set the service-indication to OFF and select the next system to be
      ** attempted acquisition.
      */
      SD_MSG_MED( "ACQ_STT DONE -> START, event=%d",event,0,0 );

      #ifndef FEATURE_JCDMA
      sd_si_srv_off( acq_ptr->ss );
      #endif

      list_is_exhausted = sdss_acq_next( acq_ptr,
                                         mode_pref,
                                         band_pref,
                                         rtrn_iact_ptr );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_STT_MORE_PREF:

      /* Attempt to acquire the more preferred system.
      */
      SD_MSG_MED( "ACQ_STT DONE -> MORE_PREF, event=%d",event,0,0 );

      rtrn_iact_ptr->act             = SDSS_IACT_ACQUIRE;
      rtrn_iact_ptr->prm.acq.list    = acq_ptr->pref_lst;
      rtrn_iact_ptr->prm.acq.pos     = acq_ptr->pref_pos;
      rtrn_iact_ptr->prm.acq.mode    = SDSS_ACQ_MODE_FULL;

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_STT_DONE:
    case SDSS_ACQ_STT_REACQ:
    case SDSS_ACQ_STT_PWR_SAVE_ENTER:
    case SDSS_ACQ_STT_PWR_SAVE:
    case SDSS_ACQ_STT_SCAN_CONT:
    case SDSS_ACQ_STT_MAX:
    case SDSS_ACQ_STT_NONE:
    default:
      SD_CASE_ERR( "state",0,0,0 );
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return list_is_exhausted;

} /* sdss_acq_done_event_proc *//*lint +esym(715, roam_pref) */
                                /*lint +esym(529, ss) */
                                /*lint +esym(550, ss) */

#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_acq_event_proc

DESCRIPTION
  Process SS-Event in the context of an acquisition state machine and
  instruct the caller of the next SS-Internal-Action to take.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the entire list that is currently being associated with the
  acquisition state machine is exhausted. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                 sdss_acq_event_proc(

        sdss_acq_s_type         *acq_ptr,
            /* Pointer an acquisition state machine to process the SS-Event.
            */

        dword                   *scr_timer_ptr,
            /* Script timer
            */

        sdss_evt_e_type         event,
            /* SS-Event to process.
            */

        sd_ss_mode_pref_e_type  mode_pref,
            /* Current mode preference.
            */

        sd_ss_band_pref_e_type  band_pref,
            /* Current band preference.
            */

        sd_ss_roam_pref_e_type  roam_pref,
            /* Current roam preference.
            */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{
  /*lint -esym(715, roam_pref, scr_timer_ptr) */ /* not referenced */
  /*lint -esym(818, scr_timer_ptr) */ /* Could be declared as pointing to const
                                      */

  sdss_acq_stt_e_type   acq_state;
  boolean               list_is_exhausted = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( roam_pref, SD_SS_ROAM_PREF_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );
  SD_ASSERT( scr_timer_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  acq_state = acq_ptr->state;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is the first time this function is being called for this
  ** acquisition construct, select the first system to be attempted
  ** acquisition.
  */
  if( acq_ptr->acq_pos  == SDSR_POS_NULL &&
      acq_ptr->sched_pos == SDSR_POS_NULL )
  {
    /* Set the acquisition starting position.
    */
    acq_ptr->acq_pos = acq_ptr->acq_start_pos - 1;

    /* Get the next system to be attempted acquisition. If acquisition
    ** type is SPECIFIC, get next system regardless of the current mode and
    ** band preference. Else, get the next system that complies with the
    ** current mode and band preference.
    */
    list_is_exhausted = sdss_acq_next( acq_ptr,
                                       mode_pref,
                                       band_pref,
                                       rtrn_iact_ptr );

    return list_is_exhausted;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, this is not the first time this function is being called
  ** for this acquisition construct, so do processing in accordance with the
  ** acquisition state, type and SS-Event.
  */
  switch( acq_ptr->state )
  {
    case SDSS_ACQ_STT_START:

      list_is_exhausted = sdss_acq_start_event_proc( acq_ptr,
                                                     scr_timer_ptr,
                                                     event,
                                                     mode_pref,
                                                     band_pref,
                                                     roam_pref,
                                                     rtrn_iact_ptr );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_STT_MORE_PREF:

      list_is_exhausted = sdss_acq_more_pref_event_proc( acq_ptr,
                                                         event,
                                                         mode_pref,
                                                         band_pref,
                                                         roam_pref,
                                                         rtrn_iact_ptr );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_STT_REACQ:

      list_is_exhausted = sdss_acq_reacq_event_proc( acq_ptr,
                                                     event,
                                                     mode_pref,
                                                     band_pref,
                                                     roam_pref,
                                                     rtrn_iact_ptr );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_STT_DONE:

      list_is_exhausted = sdss_acq_done_event_proc( acq_ptr,
                                                    event,
                                                    mode_pref,
                                                    band_pref,
                                                    roam_pref,
                                                    rtrn_iact_ptr );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
    #else
    case SDSS_ACQ_STT_PWR_SAVE:
    case SDSS_ACQ_STT_PWR_SAVE_ENTER:
      SD_CASE_ERR( "state",0,0,0 );
      break;
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_STT_SCAN_CONT:
    case SDSS_ACQ_STT_MAX:
    case SDSS_ACQ_STT_NONE:
    default:

      SD_CASE_ERR( "state",0,0,0 );
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If acquisition state was changed, remember the previous state.
  */
  if( acq_ptr->state != acq_state )
  {
    acq_ptr->prev_state = acq_state;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return list_is_exhausted;

} /* sdss_acq_event_proc */
 /*lint +esym(715, roam_pref, scr_timer_ptr) */
 /*lint +esym(818, scr_timer_ptr) */



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================ SS-SCRIPT-ENGINE ===============================
=============================================================================
=============================================================================
===========================================================================*/


/* Function prototypes.
*/
static  boolean                       sdss_eng_stack_is_state(
        const sdss_eng_stack_s_type   *stack_ptr,
        sdss_acq_stt_e_type           acq_stt
);

static  sdss_return_status_e_type    sdss_eng_stack_is_state2(
        sd_ss_e_type                  ss,
        sdss_acq_stt_e_type           acq_stt
);

static  boolean                       sdss_eng_stack_is_type(
        const sdss_eng_stack_s_type   *stack_ptr,
        sdssscr_acq_e_type            acq_type
);


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to map a script constract parameter into a system record list in the
** context of a script engine.
*/
#define ENG_LIST( list )        sdss_sr_list_map( eng_ptr->ss_ptr, \
                                                  (sdsr_e_type) list)



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_init

DESCRIPTION
  Initializes a script engine with a particular script.

  NOTE! this function must be called before any other sdss_eng_xxx is called.

DEPENDENCIES
  sdss_init() must have already been called to initialize the SS-Core.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_init(

        sdss_eng_s_type         *eng_ptr,
            /* Pointer to the script engine which to initialize with the
            ** SS-Script that is referenced by scr_ref.
            */

        sdss_s_type             *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        sdssscr_ref_type        scr_ref,
            /* Reference to an SS-Script with which to initialize the
            ** script-engine that is pointed to by eng_ptr.
            */

        sdss_evt_grp_e_type     event_group,
        int                     scr_tbl_idx
            /* The event group and script table index (for debugging).
            */
)
{
  sd_ss_e_type      ss                = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT( scr_ref != SDSSSCR_REF_NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event_group, SDSS_EVT_GRP_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if (defined (FEATURE_TECH_IND_SD_STACK_ARCH) || defined(FEATURE_HDR_HYBRID))
  /* Work with HYBR system selection, if specified so.
  */
  if( ss_ptr == sdss_ptr(SD_SS_HYBR_1) )
  {
    ss = SD_SS_HYBR_1;
  }
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the ss to point at the associated system selection enum type
  */
  eng_ptr->ss             = ss;


  /* Set the ss_ptr to point at the associated system selection information
  ** structure.
  */
  eng_ptr->ss_ptr         = ss_ptr;


  /* Initialize the script-parser with the script that is referenced by
  ** scr_ref.
  */
  sdssscr_parser_init( &eng_ptr->parser,
                       scr_ref,
                       event_group,
                       scr_tbl_idx,
                       ss );

  /* Initialize the script-control, so that first script construct is going
  ** to be parsed when the script-engine is instructed to process an event.
  */
  eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_NEXT_CONS;


  /* Initialize acquisition mode to MINI.
  */
  eng_ptr->acq_mode       = SDSS_ACQ_MODE_MINI;


  /* Initialize script timer to disabled.
  */
  eng_ptr->scr_timer      = 0;


  /* Remember the script's event group and table index (for debugging).
  */
  eng_ptr->scr_evt_grp    = event_group;
  eng_ptr->scr_tbl_idx    = scr_tbl_idx;

  /* Initialize the Acq state machine to NONE.
  */
  eng_ptr->acq.state      = SDSS_ACQ_STT_NONE;

  /* Initialize the Acq type to SDSSSCR_ACQ_MAX.
  */
  eng_ptr->acq.acq_type   = SDSSSCR_ACQ_MAX;

} /* sdss_eng_init */



/* <EJECT> */
/*===========================================================================
=============================================================================
============== LIST MANIPULATOR SS-SCRIPT CONSTRUCTS FUNCTIONS ==============
=============================================================================
===========================================================================*/


/* <EJECT> */
/*===========================================================================
=============================================================================
=============== LIST OPERATOR SS-SCRIPT CONSTRUCTS FUNCTIONS ================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdss_eng_scr_con_meas

DESCRIPTION
  Implement the MEAS( LIST, MEAS_TYPE ) SS-Script Constructs.

  Request a measurement of type meas_type from Search for all the systems
  that are referenced by the system record list/item list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_scr_con_meas(

        sdss_eng_s_type         *eng_ptr,
            /* Pointer to the SS-script-engine that is associated with the
            ** SS-Script that is being executed.
            */

        sdss_evt_e_type         event,
            /* SS-Event coming from the SS-Front-end.
            *//*lint -esym(715, event) */ /* not referenced */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{

  /* List for which to get the channel strength measurement.
  */
  sdsr_e_type             list;

  /* The mode of the measurement to perform.
  */
  sdss_meas_mode_e_type   meas_mode;

  /* The type of the measurement to perform.
  */
  sd_ss_meas_type_e_type  meas_type;

  sd_ss_e_type            ss        = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  list      = ENG_LIST( eng_ptr->con_prm1 );
  meas_mode = (sdss_meas_mode_e_type) eng_ptr->con_prm2;
  meas_type = (sd_ss_meas_type_e_type) eng_ptr->con_prm3;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( meas_mode, SDSS_MEAS_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( meas_type, SD_SS_MEAS_TYPE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined (FEATURE_TECH_IND_SD_STACK_ARCH) || defined(FEATURE_HDR_HYBRID))
  /* Work with HYBR_1 system selection, if specified so.
  */
  if( eng_ptr->ss_ptr == sdss_ptr(SD_SS_HYBR_1) )
  {
    ss = SD_SS_HYBR_1;
  }
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is the first time this function is being called for this script
  ** construct, submit the measurement request.
  */
  if( eng_ptr->scr_ctrl.ctrl == SDSS_SCR_CTRL_NEXT_CONS )
  {
    eng_ptr->scr_ctrl.ctrl = SDSS_SCR_CTRL_RTRN_ACT;

    rtrn_iact_ptr->act = SDSS_IACT_MEAS;
    rtrn_iact_ptr->prm.meas.mode = meas_mode;
    rtrn_iact_ptr->prm.meas.type = meas_type;
    rtrn_iact_ptr->prm.meas.list = list;

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, this is not the first time this function is being called
  ** for this script construct. If the get networks request is done, continue
  ** with the next script construct. Else, stay and wait for the get networks
  ** results.
  */
  if( sd_ss_meas_is_done(ss) )
  {
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_NEXT_CONS;
  }
  else
  {
    SD_MSG_MED( "Ignoring event=%d",event,0,0 );
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_RTRN_ACT;
    rtrn_iact_ptr->act      = SDSS_IACT_CONTINUE;
  }

} /* sdss_eng_scr_con_meas */ /*lint +esym(715, event) */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
/*===========================================================================

FUNCTION sdss_get_net

DESCRIPTION
  Request to get the available GSM/WCDMA networks according to the reference
  list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdss_scr_ctrl_e_type    sdss_get_net(

        sd_ss_e_type            ss,
            /* Indicate the system selection type.
            */
        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{
  sdsr_s_type                   *sr_ptr;
  int                           list_pos;
  sdss_s_type                   *ss_ptr = sdss_ptr( ss );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the first complied system record from the list.
  */
  list_pos = sdss_sr_list_find_next( ss,
                                     ss_ptr->get_net_list,
                                     ss_ptr->get_net_list_start_pos,
                                     0,
                                     -10000,
                                     ss_ptr->get_net_mode_pref,
                                     ss_ptr->get_net_band_pref,
                                     SYS_WLAN_BSS_TYPE_ANY,
                                     SDSR_CAT_ALL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no system on the specified list complies with current mode and
  ** band preference, indicate the GET_NET done and move to next
  ** construct.
  */
  if( list_pos == SDSR_POS_NULL )
  {
    SD_MSG_HIGH( "User list done!",0,0,0 );

    /* Indicate get network list request done.
    */
    sd_ss_update_get_net_done( ss, TRUE );

    /* Move to next construct.
    */
    return SDSS_SCR_CTRL_NEXT_CONS;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Populate the request with bands and modes filtered within target
  ** capability, system record, band and mode preference.
  */
  sr_ptr = sdsr_list_get_ptr( ss_ptr->get_net_list, list_pos );
  if(sr_ptr == NULL)
  {
    SD_ERR_FATAL("sr_ptr NULL",0,0,0);
  }
  rtrn_iact_ptr->prm.get_net.sdsr_ref = sdsr_list_get_ref(
                                                  ss_ptr->get_net_list,
                                                  list_pos );

  rtrn_iact_ptr->prm.get_net.mode_pref =
                            sdss_map_mode_to_mode_pref(sr_ptr->sys.mode);

  rtrn_iact_ptr->prm.get_net.band_pref = sr_ptr->sys.band;

  rtrn_iact_ptr->prm.get_net.acq_order_pref = ss_ptr->acq_order_pref;

  /* Indicate get network list request is pending.
  */
  sd_ss_update_get_net_done( ss, FALSE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the list position of current get network list request and
  ** point to next position.
  */
  ss_ptr->get_net_list_pos = list_pos;
  ss_ptr->get_net_list_start_pos = list_pos + 1;

  /* Return action: SDSS_IACT_GET_NET_GW
  */
  rtrn_iact_ptr->act = SDSS_IACT_GET_NET_GW;

  return SDSS_SCR_CTRL_RTRN_ACT;

}/* sdss_get_net */



/*===========================================================================

FUNCTION sdss_eng_scr_con_get_net

DESCRIPTION
  Implement the GET_NET( ) SS-Script Constructs.

  Request to get the available GSM/WCDMA networks.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_scr_con_get_net(

        sd_ss_e_type            ss,
            /* The ss for which to get_net
            */

        sdss_eng_s_type         *eng_ptr,
            /* Pointer to the SS-script-engine that is associated with the
            ** SS-Script that is being executed.
            */

        sdss_evt_e_type         event,
            /* SS-Event coming from the SS-Front-end.
            */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{
  sdsr_e_type        list      = ENG_LIST( eng_ptr->con_prm1 );
  sdsr_e_type        ref_sys   =  ENG_LIST( eng_ptr->con_prm2 );
  sdss_s_type        *ss_ptr    = eng_ptr->ss_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is the first time this function is being called for this script
  ** construct and event is SDSS_EVT_USER_GET_NET_GW,
  ** send get network list request according to the first system in the get
  ** network search list. Pointer list_pos to the next position of the system
  ** in the get network search list.
  */
  if( eng_ptr->scr_ctrl.ctrl == SDSS_SCR_CTRL_NEXT_CONS &&
      event == SDSS_EVT_USER_GET_NET_GW)
  {
    ss_ptr->get_net_list = list;
    ss_ptr->get_net_list_start_pos = 0;

    eng_ptr->scr_ctrl.ctrl = sdss_get_net(ss, rtrn_iact_ptr);

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If event is SDSS_EVT_MISC_GW_NET_LIST
  **
  ** 1.If get network list request is being serviced by Search, put the last
  ** system which recievied the gw network list report into the ref_sys and
  ** continue with the next script construct.
  **
  ** 2.Otherwise, request one more get network list until the search list is
  **   exhaused.
  **
  */
  if( event == SDSS_EVT_MISC_GW_NET_LIST )
  {
    if( sd_ss_is_get_net_done(ss) )
    {
      SD_MSG_HIGH(" GET_NET list_pos = %d",
                  ss_ptr->get_net_list_pos, 0 , 0);

      sdsr_list_put_ref( ref_sys,
                         sdsr_list_get_ref( ss_ptr->get_net_list,
                                            ss_ptr->get_net_list_pos));

      eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_NEXT_CONS;

      return;
    }

    eng_ptr->scr_ctrl.ctrl = sdss_get_net(ss, rtrn_iact_ptr);

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For all other events, continue with the next construct only if the get
  ** network list is complete.
  */

  if( sd_ss_is_get_net_done(ss) )
  {
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_NEXT_CONS;
  }
  else
  {
    SD_MSG_MED( "Ignoring event=%d",event,0,0 );
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_RTRN_ACT;
    rtrn_iact_ptr->act      = SDSS_IACT_CONTINUE;
  }
  return;

} /* sdss_eng_scr_con_get_net */
#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */

/*===========================================================================

FUNCTION sdss_eng_scr_con_survey_wlan

DESCRIPTION
  Implement the SURVEY_WLAN SS-Script Constructs.

  Request to get the available WLAN networks.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_scr_con_survey_wlan(
        /*lint -esym(715, event) */ /* not referenced */

        sdss_eng_s_type         *eng_ptr,
            /* Pointer to the SS-script-engine that is associated with the
            ** SS-Script that is being executed.
            */

        sdss_evt_e_type         event,
            /* SS-Event coming from the SS-Front-end.
            */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{
  sd_ss_e_type            ss        = SD_SS_HYBR_WLAN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is the first time this function is being called for this script
  ** construct, submit the measurement request.
  */
  if( eng_ptr->scr_ctrl.ctrl == SDSS_SCR_CTRL_NEXT_CONS )
  {
    eng_ptr->scr_ctrl.ctrl = SDSS_SCR_CTRL_RTRN_ACT;

    rtrn_iact_ptr->act = SDSS_IACT_GET_NET_WLAN;

    sd_ss_update_wlan_survey_done( ss, FALSE );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, this is not the first time this function is being called
  ** for this script construct.
  */

  /* If request is done being serviced by Search, continue with the next
  ** script construct.
  */
  if( sd_ss_is_wlan_survey_done(ss) )
  {
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_NEXT_CONS;
    return;
  }
  else
  {
    SD_MSG_MED( "Ignoring event=%d",event,0,0 );
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_RTRN_ACT;
    rtrn_iact_ptr->act      = SDSS_IACT_CONTINUE;
  }

} /* sdss_eng_scr_con_survey_wlan*/
/*lint +esym(715, event) */ /* not referenced */


#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
/*===========================================================================

FUNCTION sdss_eng_scr_con_pwr_scan_gw

DESCRIPTION
  Implement the PWR_SCAN_GW( LIST ) SS-Script Constructs.

  Request to do early power scan for GSM/WCDMA networks.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_scr_con_pwr_scan_gw(

        sdss_eng_s_type         *eng_ptr,
            /* Pointer to the SS-script-engine that is associated with the
            ** SS-Script that is being executed.
            */

        sdss_evt_e_type         event,
            /* SS-Event coming from the SS-Front-end.
            */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{
  /* System record list over which to attempt acquisition.
  */
  sdsr_e_type             list;
  sdsr_s_type             sr;
  sdsr_s_type             *sr_ptr   = &sr;
  int                     pos;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  list = ENG_LIST( eng_ptr->con_prm1 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is the first time this function is being called for this script
  ** construct, submit the early power scan request. Once issuing the
  ** pwr_scan request to the GSM/UMTS stack, SS-Client will call the
  ** sd_ss_ind_misc_cnf() which in turn will generate the SDSS_EVT_MISC_CNF
  ** SS-Event.
  */
  if( eng_ptr->scr_ctrl.ctrl == SDSS_SCR_CTRL_NEXT_CONS )
  {
    eng_ptr->scr_ctrl.ctrl = SDSS_SCR_CTRL_RTRN_ACT;

    rtrn_iact_ptr->act = SDSS_IACT_PWR_SCAN_GW;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Get the GW band from list which comply with system preference.
    */
    pos = sdss_sr_list_find_next( eng_ptr->ss,
                                  list,
                                  0,
                                  0,
                                  -10000,
                                  eng_ptr->ss_ptr->true_mode_pref,
                                  eng_ptr->ss_ptr->band_pref,
                                  SYS_WLAN_BSS_TYPE_ANY,
                                  SDSR_CAT_ALL
                                );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Update band preference if there is no available record use the default
    ** band.
    */
    if( pos != SDSR_POS_NULL)
    {
      sr_ptr = sdsr_list_get_ptr(list, pos);
      if(sr_ptr!= NULL)
      {
        rtrn_iact_ptr->prm.pwr_scan.band_pref = sr_ptr->sys.band;
      }  

    }
    else
    {
      rtrn_iact_ptr->prm.pwr_scan.band_pref = SD_BAND_GROUP1;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Update mode and acq order preference
    */
    rtrn_iact_ptr->prm.pwr_scan.acq_order_pref =
                                             eng_ptr->ss_ptr->acq_order_pref;
    rtrn_iact_ptr->prm.pwr_scan.mode_pref = eng_ptr->ss_ptr->true_mode_pref;


    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here this is not the first time this function is being called
  ** for this script construct. If event is SDSS_EVT_MISC_CNF,
  ** continue with next script construct. Else, stay and wait for SS-Client
  ** to report that it entered power-save mode of operation.
  */
  if( event == SDSS_EVT_MISC_CNF )
  {
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_NEXT_CONS;
  }
  else
  {
    SD_MSG_MED( "Ignoring event=%d",event,0,0 );
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_RTRN_ACT;
    rtrn_iact_ptr->act      = SDSS_IACT_CONTINUE;
  }

} /* sdss_eng_scr_con_pwr_scan_gw */
  #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_scr_con_acq

DESCRIPTION
  Implement the following acquisition SS-Script Constructs:

  ACQ( LIST, ACQ_TYPE )
    Step through the system record list/item LIST (first to last) and try to
    acquire a system that are referenced by LIST in accordance with ACQ_TYPE
    and the current SS-Preference.

  ACQ_ALL( LIST, ACQ_TYPE )
    Equivalent to the ACQ construct except that also systems that were
    already attempted acquisition since the last time the NEW_ACQ construct
    was encountered are being attempted acquisition.

  ACQ_CNT( LIST, ACQ_TYPE, START_POS, CNT )
    Equivalent to the ACQ construct except that LIST is traversed START_POS
    to START_POS-1 and acquisition is bounded to CNT elements.

  ACQ_TIME( LIST, ACQ_TYPE, START_POS, TIME )
    Equivalent to the ACQ construct except that LIST is traversed START_POS
    to START_POS-1 and acquisition is bounded to TIME seconds since the
    START_TIME script constructs was last encountered.

  ACQ_RX( LIST, ACQ_TYPE, RX )
    Equivalent to he ACQ construct except that only systems with Rx-power
    measurement value >= RX are being attempted acquisition.

  ACQ_PI( LIST, ACQ_TYPE, PI )
    Equivalent to the ACQ construct except that only systems with pilot
    measurement value >= PI are being attempted acquisition.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_scr_con_acq(

        sdss_eng_s_type         *eng_ptr,
            /* Pointer to the SS-script-engine that is associated with the
            ** SS-Script that is being executed.
            */

        sdss_evt_e_type         event,
            /* SS-Event coming back from the SS-Front-end.
            */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{

  /* System record list over which to attempt acquisition.
  */
  sdsr_e_type           list;

  /* Acquisition type to be attempted.
  */
  sdssscr_acq_e_type    acq_type;

  boolean               list_is_exhausted;
  sdss_s_type           *ss_ptr;
  byte                  new_acq_cnt;
  int2                  meas_val      = -1000;
  int                   start_pos     = 0;
  int                   max_num       = 1000;
  dword                 max_uptime    = (dword) (-1);
  sdsr_cat_e_type       meas_cat      = SDSR_CAT_ALL;
  sd_ss_mode_pref_e_type   mode_pref;
  const sdss_s_type       *temp_ss_ptr;
  sd_ss_mode_pref_e_type   temp_mode_pref;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT( rtrn_iact_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  list        = ENG_LIST( eng_ptr->con_prm1 );
  acq_type    = (sdssscr_acq_e_type) eng_ptr->con_prm2;
  ss_ptr      = eng_ptr->ss_ptr;
  new_acq_cnt = ss_ptr->new_acq_cnt;
  mode_pref   = ss_ptr->mode_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acq_type, SDSSSCR_ACQ_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If calling for same as last acquisition type, get the last acquisition
  ** type attempted from the SS-Core.
  */
  if( acq_type == SDSSSCR_ACQ_SAME_AS_LAST )
  {
    acq_type = ss_ptr->acq_type;
    SD_ASSERT_ENUM_IS_INRANGE( acq_type, SDSSSCR_ACQ_SAME_AS_LAST );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is the first time this function is being called for this
  ** acquisition construct, initialize the acquisition state machine.
  */
  if( eng_ptr->scr_ctrl.ctrl == SDSS_SCR_CTRL_NEXT_CONS )
  {
    /* Based on the acquisition type that is being attempted, set the
    ** parameters with which systems that are being attempted acquisition
    ** should comply.
    */
    switch( eng_ptr->scr_con ) /*lint -e788 */  /* sparse switch */
    {
      case SDSSSCR_CON_ACQ:
       break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case SDSSSCR_CON_ACQ_TRUE_MODE:
       /* Need to initialise the true_mode_pref based on the stack.
       ** Logic to initilaise true_mode_pref:
       ** If stack is MAIN, remove hybrid and WLAN mode preferences from true_mode_pref.
       ** If stack is HYBRID, remove MAIN and WLAN mode preferences from true_mode_pref.
       ** If stack is WLAN, remove hybrid and MAIN mode preferences from true_mode_pref.
       */
       mode_pref = ss_ptr->true_mode_pref;    

       if (eng_ptr->ss == SD_SS_MAIN)
       {
         #if ( defined( FEATURE_TECH_IND_SD_STACK_ARCH) || defined( FEATURE_HDR_HYBRID ))
         /* Get HYBR mode pref */
         temp_ss_ptr    = sdss_ptr( SD_SS_HYBR_1 );
         SD_ASSERT( temp_ss_ptr != NULL );
         temp_mode_pref = temp_ss_ptr->mode_pref; 
     
         /* Remove HYBR mode_pref from true mode pref */
         mode_pref = SD_MODE_DELETE( mode_pref, temp_mode_pref );
         #endif

         #ifdef FEATURE_WLAN 
#error code not present
         #endif
       }

       if (eng_ptr->ss == SD_SS_HYBR_1)
       {
         /* Get MAIN mode pref */
         temp_ss_ptr    = sdss_ptr( SD_SS_MAIN );
         SD_ASSERT( temp_ss_ptr != NULL );
         temp_mode_pref = temp_ss_ptr->mode_pref; 

         /* Remove MAIN mode_pref from true mode pref */
         mode_pref = SD_MODE_DELETE( mode_pref, temp_mode_pref );

         #ifdef FEATURE_WLAN
#error code not present
         #endif
       }

       if (eng_ptr->ss == SD_SS_HYBR_WLAN)
       {
         /* Get MAIN mode pref */
         temp_ss_ptr    = sdss_ptr( SD_SS_MAIN );
         SD_ASSERT( temp_ss_ptr != NULL );
         temp_mode_pref = temp_ss_ptr->mode_pref; 

         /* Remove MAIN mode_pref from true mode pref */
         mode_pref = SD_MODE_DELETE( mode_pref, temp_mode_pref );

        #if ( defined( FEATURE_TECH_IND_SD_STACK_ARCH) || defined( FEATURE_HDR_HYBRID ))
         /* Get HYBR mode pref */
         temp_ss_ptr    = sdss_ptr( SD_SS_HYBR_1 );
         SD_ASSERT( temp_ss_ptr != NULL );
         temp_mode_pref = temp_ss_ptr->mode_pref; 

         /* Remove HYBR mode_pref from true mode pref */
         mode_pref = SD_MODE_DELETE( mode_pref, temp_mode_pref );
         #endif
       }

       break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      
    case SDSSSCR_CON_ACQ_ALL:
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_CNT:
      start_pos   = sdss_sr_list_find_acq_start_pos( ss_ptr,
                                                       list,
                                                       eng_ptr->con_prm3 );
        max_num     = (int) (eng_ptr->con_prm4);
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_TIME:
        start_pos   = sdss_sr_list_find_acq_start_pos( ss_ptr,
                                                       list,
                                                       eng_ptr->con_prm3 );
        max_uptime  = ss_ptr->acq_start_time + eng_ptr->con_prm4;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_TIME2:
        start_pos   = sdss_sr_list_find_acq_start_pos( ss_ptr,
                                                       list,
                                                       eng_ptr->con_prm3 );
        max_uptime  = ss_ptr->acq_start_time + eng_ptr->con_prm4;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_TIME3:
        start_pos   = sdss_sr_list_find_acq_start_pos( ss_ptr,
                                                       list,
                                                       eng_ptr->con_prm3 );
        /* Convert the scan time from milli-seconds to seconds
        ** NOTE: Round off errors are not considered
        */
        max_uptime  = ss_ptr->acq_start_time + (dword)eng_ptr->con_prm4/1000;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_CNT_OR_TIME:

        start_pos   = sdss_sr_list_find_acq_start_pos( ss_ptr,
                                                       list,
                                                       eng_ptr->con_prm3 );
        max_num     = (int) ( eng_ptr->con_prm4 );
        max_uptime  = (dword)( ss_ptr->acq_start_time + eng_ptr->con_prm5 );
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_RX:
        meas_val    = - (int2) ((uint2) (eng_ptr->con_prm3));
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_PI:
        meas_val    = - (int2) ((uint2) (eng_ptr->con_prm3));
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case SDSSSCR_CON_ACQ_PI_X:
        meas_cat = (sdsr_cat_e_type)(eng_ptr->con_prm4);
        meas_val    = - (int2) ((uint2) (eng_ptr->con_prm3));
        break;
        
      case SDSSSCR_CON_ACQ_NETLIST:
        meas_val    = (int2) (eng_ptr->con_prm3);
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      default:
        SD_CASE_ERR( "scr_con",0,0,0 );
        break;

    } /* switch */ /*lint +e788 */ /* sparse switch */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    sdss_acq_init( &eng_ptr->acq,
                   eng_ptr->ss_ptr,
                   list,
                   start_pos,
                   max_num,
                   max_uptime,
                   eng_ptr->scr_con,
                   acq_type,
                   eng_ptr->acq_mode,
                   new_acq_cnt,
                   meas_val,
                   meas_cat);

    /* Update the acquisition type of the SS-Core per the acquisition type
    ** parameter.
    */
    sdss_acq_type_update( eng_ptr->ss_ptr, acq_type );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if pwr_save_enter pertains to throttle engine.
  */
  if ( event == SDSS_EVT_MISC_PWR_SAVE_ENTER &&
       ( sdss_thrttl_enter_pwr_save( &eng_ptr->ss_ptr->acq_thrttl,
                                    &eng_ptr->scr_timer,
                                    rtrn_iact_ptr
                                   ) == SDSS_THRTTL_EVT_STATUS_CONSUMED
       )

     )
  {
    /* Throttle in progress.
    */
    eng_ptr->scr_ctrl.ctrl = SDSS_SCR_CTRL_RTRN_ACT;
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if timeout pertains to throttle engine.
  */
  if ( (event == SDSS_EVT_MISC_TIMER_RESEL_OK ||
        event == SDSS_EVT_MISC_TIMER_RESEL_NO ) &&
        (sdss_thrttl_pwr_save_timeout( &eng_ptr->ss_ptr->acq_thrttl,
                                       &eng_ptr->scr_timer,
                                       rtrn_iact_ptr
                                     ) == SDSS_THRTTL_EVT_STATUS_CONSUMED
        )
     )
  {
    /* Throttle in progress.
    */
    eng_ptr->scr_ctrl.ctrl = SDSS_SCR_CTRL_RTRN_ACT;
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if it is the ACQ event is for acquisition action issued from
  ** the same engine. It is possible because of PREV_SCR, the acq event
  ** circulates to a different engine where it should not be processed.
  ** Note eng_ptr->acq.rtrn_iact.act == SDSS_IACT_ACQUIRE indicates
  ** new script engine was pushed when ACQ action was pending.
  */
  if ( SDSS_EVT_IS_XXX_ACQ(event) &&
       eng_ptr->acq.rtrn_iact.act == SDSS_IACT_ACQUIRE &&
       ss_ptr->last_acq_eng_ptr != eng_ptr
     )
  {
    /* Incorrect acq. event, re issue the acq action from current engine.
    */
    SD_MSG_HIGH("Re-issuing acquire action", 0, 0, 0 );
    *rtrn_iact_ptr = eng_ptr->acq.rtrn_iact;

    /* If the system used in the pending ACQ is not there anymore when we
     * get back to it, then assume all systems in the acq list failed
     * acquisition.
     *
     * Examples of when this could happen:
     *
     * 1. The scripts that were pushed on top of this script had modified the
     *    list used for acquisition.
     * 2. The scripts that were pushed on top of this script completed
     *    processing the SCHED_LST (due to another ACQ construct) that was
     *    being processed by this script when it was interrupted.
    */
    if( sdsr_list_get_ref( rtrn_iact_ptr->prm.acq.list,
                           rtrn_iact_ptr->prm.acq.pos ) == SDSR_REF_NULL )
    {
      /* If the pending ACQ was from the SCHED_LST, then see if we can
      ** proceed with the list used in the ACQ construct.
      */
      if( rtrn_iact_ptr->prm.acq.list == eng_ptr->acq.sched_lst )
      {
        eng_ptr->acq.sched_pos = SDSR_POS_NULL;
      }

      /* Check if there are more systems in the list. */
      list_is_exhausted  =  sdss_acq_event_proc( &eng_ptr->acq,
                                                 &eng_ptr->scr_timer,
                                                 event,
                                                 mode_pref,
                                                 ss_ptr->band_pref,
                                                 ss_ptr->roam_pref,
                                                 rtrn_iact_ptr );

      /* This check makes sure we don't consider an empty SCHED_LST as an
      ** error. The SCHED_LST was simply taken care of by an ACQ in a
      ** different script.
      */
      if( sdsr_list_get_ref( eng_ptr->acq.acq_lst,
                             eng_ptr->acq.acq_pos ) == SDSR_REF_NULL )
      {
        SD_ERR("ERROR! Multiple scripts accessing list %d !",
                                              eng_ptr->acq.acq_lst,0,0);
      }
    }
    else if ( sdss_sr_is_pref_conf0(ss_ptr, 
                            sdsr_list_get_ptr( rtrn_iact_ptr->prm.acq.list,
                            rtrn_iact_ptr->prm.acq.pos ), 
                                    SDSS_PREF_CONF_ANY))
    {
      /* Check if there are more systems in the list
      ** with updated preferences
      */
      list_is_exhausted  =  sdss_acq_event_proc( &eng_ptr->acq,
                                                   &eng_ptr->scr_timer,
                                                   event,
                                                   mode_pref,
                                                   ss_ptr->band_pref,
                                                   ss_ptr->roam_pref,
                                                   rtrn_iact_ptr );
    }
    else
    {
      list_is_exhausted = FALSE;
    }
  }
  else
  {
    /* Call on the acquisition state machine to do acquisition processing.
    */
    list_is_exhausted  =  sdss_acq_event_proc( &eng_ptr->acq,
                                               &eng_ptr->scr_timer,
                                               event,
                                               mode_pref,
                                               ss_ptr->band_pref,
                                               ss_ptr->roam_pref,
                                               rtrn_iact_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the acquisition list is exhausted, continue with next script
  ** construct. Else, return with the acquisition action to the SS-Client.
  */
  if( list_is_exhausted )
  {
    SD_MSG_MED( "List=%d is exhausted!",list,0,0 );
    eng_ptr->acq.state = SDSS_ACQ_STT_NONE;
    eng_ptr->scr_ctrl.ctrl = SDSS_SCR_CTRL_NEXT_CONS;
  }
  else
  {
    SD_MSG_HIGH("Acq type=%d, prev_state=%d, new_state=%d",
                eng_ptr->acq.acq_type,
                eng_ptr->acq.prev_state,
                eng_ptr->acq.state );
    /* Save the rtrn act in acq state machine and return.
    */
    if ( rtrn_iact_ptr->act == SDSS_IACT_ACQUIRE )
    {
      eng_ptr->acq.rtrn_iact = *rtrn_iact_ptr;
      ss_ptr->last_acq_eng_ptr = eng_ptr;
    }

    /* Turn the service indicator to power save
    */
    if( rtrn_iact_ptr->act == SDSS_IACT_PWR_SAVE )
    {
      sd_si_srv_pwr_save( eng_ptr->parser.ss );
    }

    eng_ptr->scr_ctrl.ctrl = SDSS_SCR_CTRL_RTRN_ACT;
    sdss_thrttl_iact_acq( &eng_ptr->ss_ptr->acq_thrttl, rtrn_iact_ptr );
  }

} /* sdss_eng_scr_con_acq */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_scr_con_sched

DESCRIPTION
  Implement the following SS-Script Constructs:
  SCHED( LIST, SCHED, ACQ_TYPE )
  SCHED_COND( LIST, SCHED, ACQ_TYPE )
  SCHED_2( LIST, SCHED, ACQ_TYPE, ACQ_MODE )

  Schedule the system records that are referenced by the system record list
  LIST to be attempted acquisition of type ACQ_TYPE at time points that are
  specified by the SCHED schedule.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_scr_con_sched(

        sdss_eng_s_type         *eng_ptr,
            /* Pointer to the SS-script-engine that is associated with the
            ** SS-Script that is being executed.
            */

        sdsssr_sched_cond_e_type sched_cond,
            /* Indicate whether systems should be conditionally schedule
            ** depending whether they are not already on equal or higher
            ** priority reacquisition schedule.
            */

        const sdss_iact_s_type  *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            *//*lint -esym(715, rtrn_iact_ptr) */ /* not referenced */
)
{
  /* System record list over which to attempt reacquisition.
  */
  sdsr_e_type           list;

  /* Schedule list.
  */
  sdsr_e_type           sched_list;

  /* Reacquisition schedule.
  */
  sdssscr_sched_e_type  sched;

  /* Acquisition type to be attempted.
  */
  sdssscr_acq_e_type    acq_type;

  sdss_acq_mode_e_type  acq_mode  = SDSS_ACQ_MODE_FULL;

  const sdss_s_type     *ss_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  list       = ENG_LIST( eng_ptr->con_prm1 );
  sched_list = ENG_LIST( SDSR_SCHED_LST );
  sched      = (sdssscr_sched_e_type) eng_ptr->con_prm2;
  acq_type   = (sdssscr_acq_e_type) eng_ptr->con_prm3;
  ss_ptr     = eng_ptr->ss_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acq_type, SDSSSCR_ACQ_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( sched, SDSSSCR_SCHED_REACQ_LAST );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If scheduling calling for same as last acquisition type, get the last
  ** acquisition type to be attempted from the SS-Core.
  */
  if( acq_type == SDSSSCR_ACQ_SAME_AS_LAST )
  {
    acq_type = ss_ptr->acq_type;
    SD_ASSERT_ENUM_IS_INRANGE( acq_type, SDSSSCR_ACQ_SAME_AS_LAST );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If script construct is SDSSSCR_CON_SCHED_2, the acquisition mode is
  ** specified by the 4th parameter.
  */
  if( eng_ptr->scr_con == SDSSSCR_CON_SCHED_2 )
  {
    acq_mode = (sdss_acq_mode_e_type) eng_ptr->con_prm4;
    SD_ASSERT_ENUM_IS_INRANGE( acq_mode, SDSS_ACQ_MODE_MAX );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Schedule the list for reacquisition attempts.
  */
  sdss_sr_list_sched( list,
                      sched_list,
                      sched,
                      acq_type,
                      acq_mode,
                      sched_cond );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Continue with next script construct.
  */
  eng_ptr->scr_ctrl.ctrl = SDSS_SCR_CTRL_NEXT_CONS;

} /* sdss_eng_scr_con_sched *//*lint +esym(715, rtrn_iact_ptr) */


/* <EJECT> */
/*===========================================================================
=============================================================================
================== CONTROL SS-SCRIPT CONSTRUCTS FUNCTIONS ===================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdss_eng_scr_con_proc_evt

DESCRIPTION
  Implement the PROC_EVT( SS_EVENT ) SS-Script Construct.

  Process the SS_EVENT SS-Event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_scr_con_proc_evt(

        sdss_eng_s_type         *eng_ptr,
            /* Pointer to the SS-script-engine that is associated with the
            ** SS-Script that is being executed.
            */

        const sdss_iact_s_type  *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            *//*lint -esym(715, rtrn_iact_ptr) */ /* not referenced */
)
{
  /* SS-Event to process.
  */
  sdss_evt_e_type        event;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  event = (sdss_evt_e_type) eng_ptr->con_prm1;

  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is the first time this function is being called for this
  ** acquisition construct, process the specified SS-Event. Else (the
  ** activated script has returned control), continue with next script
  ** construct.
  */
  if( eng_ptr->scr_ctrl.ctrl == SDSS_SCR_CTRL_NEXT_CONS )
  {
    eng_ptr->scr_ctrl.ctrl            = SDSS_SCR_CTRL_PROC_EVT;
    eng_ptr->scr_ctrl.prm.proc.event  = event;
  }
  else
  {
    eng_ptr->scr_ctrl.ctrl            = SDSS_SCR_CTRL_NEXT_CONS;
  }

} /* sdss_eng_scr_con_proc_evt *//*lint +esym(715, rtrn_iact_ptr) */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_scr_con_act_scr

DESCRIPTION
  Implement the ACT_SCR( SCRIPT ) SS-Script Construct.

  Make the script that is referenced SCRIPT the active SS-Script.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_scr_con_act_scr(

        sdss_eng_s_type         *eng_ptr,
            /* Pointer to the SS-script-engine that is associated with the
            ** SS-Script that is being executed.
            */

        const sdss_iact_s_type  *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            *//*lint -esym(715, rtrn_iact_ptr) */ /* not referenced */
)
{
  /* SS-Script which to activate.
  */
  sdssscr_e_type        script;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  script    = (sdssscr_e_type) eng_ptr->con_prm1;

  SD_ASSERT_ENUM_IS_INRANGE( script, SDSSSCR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is the first time this function is being called for this
  ** acquisition construct, activate the specified SS-Script. Else (the
  ** activated script has returned control), continue with next script
  ** construct.
  */
  if( eng_ptr->scr_ctrl.ctrl == SDSS_SCR_CTRL_NEXT_CONS )
  {
    eng_ptr->scr_ctrl.ctrl            = SDSS_SCR_CTRL_ACT_SCR;
    eng_ptr->scr_ctrl.prm.act.script  = script;
  }
  else
  {
    eng_ptr->scr_ctrl.ctrl            = SDSS_SCR_CTRL_NEXT_CONS;
  }

} /* sdss_eng_scr_con_act_scr *//*lint +esym(715, rtrn_iact_ptr) */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_scr_con_act_meas

DESCRIPTION
  Implement the ACT_SCR_RX( SCRIPT, LIST, RX ) and
  ACT_SCR_PI( SCRIPT, LIST, PI ) SS-Script Constructs.

  Make SCRIPT the active System Selection Script iff at least one of the
  systems that are referenced by the system record list, LIST, has a channel
  strength measurement value >= MEAS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_scr_con_act_meas(

        sdss_eng_s_type         *eng_ptr,
            /* Pointer to the SS-script-engine that is associated with the
            ** SS-Script that is being executed.
            */

        const sdss_iact_s_type  *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            *//*lint -esym(715, rtrn_iact_ptr) *//* not referenced */
)
{
  /* SS-Script which to activate.
  */
  sdssscr_e_type        script;

  /* System record list for which to check the channel strength measurements.
  */
  sdsr_e_type           list;

  const sdss_s_type     *ss_ptr;
  int2                  meas_val;
  int                   list_pos;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  script = (sdssscr_e_type) eng_ptr->con_prm1;
  list   = ENG_LIST( eng_ptr->con_prm2 );
  ss_ptr = eng_ptr->ss_ptr;

  SD_ASSERT_ENUM_IS_INRANGE( script, SDSSSCR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is NOT the first time this function is being called for this
  ** acquisition construct (i.e., the activated script has returned control),
  ** continue with next script construct.
  */
  if( eng_ptr->scr_ctrl.ctrl != SDSS_SCR_CTRL_NEXT_CONS )
  {
    eng_ptr->scr_ctrl.ctrl = SDSS_SCR_CTRL_NEXT_CONS;

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, this is the first time this function is being called for
  ** this acquisition construct.
  */

  /* Based on the activation type, set the measurement against which to
  ** compare.
  */
  switch( eng_ptr->scr_con ) /*lint -e788 */ /* sparse switch */
  {
    case SDSSSCR_CON_ACT_SCR_RX:
      meas_val = - (int2) ((uint2) (eng_ptr->con_prm3));
      break;

    case SDSSSCR_CON_ACT_SCR_PI:
      meas_val = - (int2) ((uint2) (eng_ptr->con_prm3));
      break;

    default:
      SD_CASE_ERR( "scr_con",0,0,0 );
      meas_val = -1000;
      break;

  } /* switch */ /*lint +e788 */ /* sparse switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether any system record on the specified list has a channel
  ** strength measurement value that is >= than the specified one.
  */
  list_pos = sdss_sr_list_find_next( eng_ptr->ss,
                                     list,
                                     0,
                                     0,
                                     meas_val,
                                     ss_ptr->mode_pref,
                                     ss_ptr->band_pref,
                                     ss_ptr->bss_type_pref,
                                     SDSR_CAT_ALL);

  /* If no such system found, continue with next script construct. Else,
  ** activate the specified SS-Script.
  */
  if( list_pos == SDSR_POS_NULL )
  {
    eng_ptr->scr_ctrl.ctrl            = SDSS_SCR_CTRL_NEXT_CONS;
  }
  else
  {
    eng_ptr->scr_ctrl.ctrl            = SDSS_SCR_CTRL_ACT_SCR;
    eng_ptr->scr_ctrl.prm.act.script  = script;
  }

} /* sdss_eng_scr_con_act_meas *//*lint +esym(715, rtrn_iact_ptr) */



/* <EJECT> */
/*===========================================================================
=============================================================================
=============== MISCELLANEOUS SS-SCRIPT-CONSTRUCTS FUNCTIONS ================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdss_eng_scr_con_cont

DESCRIPTION
  Implement the CONT( TIME ) SS-Script Constructs.

  Instructs the SS-Client to continue with its current activity while
  setting-up a TIME timer. When this timer expires the SS-Client shall call
  on the SD_SS_IND_MISC_TIMER SS-Indication function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_scr_con_cont(

        sdss_eng_s_type         *eng_ptr,
            /* Pointer to the SS-script-engine that is associated with the
            ** SS-Script that is being executed.
            */

        sdss_evt_e_type         event,
            /* SS-Event coming back from the SS-Front-end.
            */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{
  /* Time in seconds with which to set-up the timer.
  */
  word                  cont_type, cont_type2 = 0;
  dword                 timer, timer2;
  dword                 expired_time;
  dword                 uptime     = clk_uptime();
  sdss_s_type           *ss_ptr;
  #ifndef FEATURE_HYBR_GW
  sdsr_s_type           *srv_sys_ptr = NULL;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cont_type = eng_ptr->con_prm1;

  SD_ASSERT_ENUM_IS_INRANGE( cont_type, SDSSSCR_CONT_TYPE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e788 */ /* sparse switch */
  switch(eng_ptr->scr_con)
  {
    case SDSSSCR_CON_CONT2:
    case SDSSSCR_CON_CONT2_REDIR :
    case SDSSSCR_CON_CONT2_WAIT:
    case SDSSSCR_CON_CONT2_BSR:

      timer = WORD_TO_DWORD( eng_ptr->con_prm2, eng_ptr->con_prm3 );
      break;
    case  SDSSSCR_CON_CONT_GW_UOOS_REM_SLEEP :

      if (eng_ptr->ss_ptr->rem_uoos_sleep_time == 0)
      {
        timer = SDSSSCR_UNTIL_NEXT_EVENT;
      }
      else
      {
        timer = eng_ptr->ss_ptr->rem_uoos_sleep_time/1000;
      }

      /*  */
      if ( timer == 0 )
      {
        timer = 1;
      }
      break;

    default:

    timer     = eng_ptr->con_prm2;

    cont_type2 = eng_ptr->con_prm3;

    SD_ASSERT_ENUM_IS_INRANGE( cont_type2, SDSSSCR_CONT_TYPE_MAX );
  } /*lint +e788 */ /* sparse switch */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  timer2    = eng_ptr->con_prm4;
  ss_ptr    = eng_ptr->ss_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the script is calling for no timer, wait on the current construct
  ** forever.
  */
  if( timer == 0 )
  {
    /* If this is not the first time this function is being called for this
    ** script construct, indicate that this event is being ignored.
    */
    if( eng_ptr->scr_ctrl.ctrl != SDSS_SCR_CTRL_NEXT_CONS )
    {
      SD_MSG_MED( "Ignoring event=%d",event,0,0 );
    }

    eng_ptr->scr_timer      = 0;
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_RTRN_ACT;
    rtrn_iact_ptr->act      = SDSS_IACT_CONTINUE;

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the script is calling to continue until next event, wait on the
  ** current construct until next event.
  */
  if( timer == SDSSSCR_UNTIL_NEXT_EVENT )
  {
    /* If this is not the first time this function is being called for this
    ** script construct, continue with the next script construct. Else, wait
    ** for next event.
    */
    if( eng_ptr->scr_ctrl.ctrl != SDSS_SCR_CTRL_NEXT_CONS )
    {
      eng_ptr->scr_timer      = 0;
      eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_NEXT_CONS;
      return;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    eng_ptr->scr_timer      = 0;
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_RTRN_ACT;
    rtrn_iact_ptr->act      = SDSS_IACT_CONTINUE;

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( timer == SDSSSCR_UNTIL_NEXT_EVENT_WITH_CURR_TIMER )
  {
    /* If this is not the first time this function is being called for this
    ** script construct, continue with the next script construct. Else, wait
    ** for next event.
    */
    if( eng_ptr->scr_ctrl.ctrl != SDSS_SCR_CTRL_NEXT_CONS )
    {
      eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_NEXT_CONS;
      return;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_RTRN_ACT;
    rtrn_iact_ptr->act      = SDSS_IACT_CONTINUE;

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is the first time this function is being called for this script
  ** construct, setup the timer and return.
  */
  if( eng_ptr->scr_ctrl.ctrl == SDSS_SCR_CTRL_NEXT_CONS )
  {

    switch( eng_ptr->scr_con )  /*lint -e788 *//* enum not used */
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case SDSSSCR_CON_CONT_GW_UOOS_REM_SLEEP:
        eng_ptr->scr_timer    = uptime + timer;
        sdss_update_rem_uoos_time(eng_ptr->ss,0,0);

        break;

      case SDSSSCR_CON_CONT:
      case SDSSSCR_CON_CONT2:
      case SDSSSCR_CON_CONT3:

        eng_ptr->scr_timer    = uptime + timer;

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_CONT_REDIR:
      case SDSSSCR_CON_CONT2_REDIR:

        SD_ASSERT( uptime >= ss_ptr->redir_uptime )

        /* If the redirection timer is expired priory to the uptime and the
        ** last event is SDSS_EVT_MISC_TIMER_RESEL_OK, then ignore this timer.
        ** (We do not blindly ignore it since the reselection may be not
        ** allowed even the time is passed.
        */
        SD_MSG_HIGH("redir_uptime= %d, uptime= %d, timer= %d" ,
                    ss_ptr->redir_uptime, uptime, timer);
        if( uptime >= ss_ptr->redir_uptime + timer &&
            event == SDSS_EVT_MISC_TIMER_RESEL_OK )
        {
          return;
        }

        eng_ptr->scr_timer = ss_ptr->redir_uptime + timer;

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_CONT_WAIT:
      case SDSSSCR_CON_CONT2_WAIT:

        /* If the timer is expired priory to the uptime and the last event is
        ** SDSS_EVT_MISC_TIMER_RESEL_OK, then ignore this timer. (We do not
        ** blindly ignore it since the reselection may be not allowed even
        ** the time is passed.
        */
        if( uptime >= ss_ptr->cont_uptime[cont_type] + timer &&
            event == SDSS_EVT_MISC_TIMER_RESEL_OK )
        {
          return;
        }

        eng_ptr->scr_timer = ss_ptr->cont_uptime[cont_type] + timer;

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_CONT3_WAIT:

        /* If both timers have been started , set the expired_time per that
        ** of the earliest expiration timer.
        **
        ** Else if only one timer is active set the expiration_time per
        ** this timer.
        **
        ** Else (both timer expired), set timer for 1s.
        */

        if( ss_ptr->cont_uptime[cont_type]  > 0 &&
            ss_ptr->cont_uptime[cont_type2] > 0)
        {
          expired_time = MIN( ss_ptr->cont_uptime[cont_type] + timer,
                              ss_ptr->cont_uptime[cont_type2] + timer2 );
        }
        else
        {
          if( ss_ptr->cont_uptime[cont_type2] > 0 )
          {
            expired_time = ss_ptr->cont_uptime[cont_type2] + timer2;
          }
          else
          {
            expired_time = ss_ptr->cont_uptime[cont_type] + timer;
          }
        }

        /* If timer expires priory to the uptime and the last event is
        ** SDSS_EVT_MISC_TIMER_RESEL_OK, then ignore this timer.
        ** (We do not blindly ignore it since the reselection may be
        ** not allowed even the time is passed.)
        */

        if( uptime >= expired_time &&
            event == SDSS_EVT_MISC_TIMER_RESEL_OK )
        {
          return;
        }

        eng_ptr->scr_timer = expired_time;

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case SDSSSCR_CON_CONT_BSR:
      case SDSSSCR_CON_CONT2_BSR:

        #ifndef FEATURE_HYBR_GW
        /* If BSR timer uptime is greater than current uptime and current SRV_SYS is same 
        ** as system on which BSR timer was started, then continue with same BSR uptime.
        ** Also if BSR timer is greater than current timer then reduce the BSR timer to 
        ** current timer.
        ** Otherwise update BSR uptime with new uptime.
        */
        if(ss_ptr->bsr_timer <= uptime || 
           ((ss_ptr->bsr_timer - uptime) > timer && ss_ptr->bsr_timer > uptime))
        {       
           ss_ptr->bsr_timer= uptime + timer;
        }

        /* Set scr_timer as per bsr_timer */
        eng_ptr->scr_timer    = ss_ptr->bsr_timer;
        /* Update BSR sr ref */
        srv_sys_ptr = sdsr_list_get_ptr(SDSR_SRV_SYS,0);
        ss_ptr->bsr_sid = srv_sys_ptr->sid;
        ss_ptr->bsr_sys = srv_sys_ptr->sys;
        #else
#error code not present
        #endif
        
        break;
        
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
      default:
        SD_ERR_FATAL( "cons",0,0,0 ); /*lint -e527 */
        return; /*lint +e527 */ /* Unreachable */
    }/* switch */ /*lint +e788 *//* enum not used */

    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_RTRN_ACT;
    rtrn_iact_ptr->act      = SDSS_IACT_CONTINUE;

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, this is not the first time this function is being called
  ** for this script construct.
  */

  /* If the timer has expired and reselection is allowed, continue with the
  ** next script construct.
  */
  if( event == SDSS_EVT_MISC_TIMER_RESEL_OK )
  {
    eng_ptr->scr_timer      = 0;
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_NEXT_CONS;

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, if the timer has expired, but reselection is not allowed, indicate
  ** so by setting the timer to 0 and continue waiting.
  ** For GW, start a 30 sec timer
  */
  if( event == SDSS_EVT_MISC_TIMER_RESEL_NO )
  {
    if(sdss_sr_list_is_mode(SDSR_SRV_SYS, SYS_SYS_MODE_GW))
    {
      eng_ptr->scr_timer      = uptime + 30;
    }
    else
    {
      eng_ptr->scr_timer      = 0;
    }
    
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_RTRN_ACT;
    rtrn_iact_ptr->act      = SDSS_IACT_CONTINUE;

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, this is not the first time this function is being called
  ** and the current event is not a timer expiration event.
  */

  /* If expiration uptime <= current uptime (i.e. timer already expired), set
  ** the timer for 1s, so we would continue with the next script construct
  ** upon the expiration. Else, stay and wait for the timer to expire.
  */
  if( eng_ptr->scr_timer <= uptime )
  {
    /* If currently srv system is GW then set the timer value to 30sec */
    /* Else set it to 1sec as usual                                                     */
    if(sdss_sr_list_is_mode(SDSR_SRV_SYS, SYS_SYS_MODE_GW))
    {
      eng_ptr->scr_timer      = uptime + 30;
    }
    else
    {
      eng_ptr->scr_timer      = uptime + 1;
    }

    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_RTRN_ACT;
    rtrn_iact_ptr->act      = SDSS_IACT_CONTINUE;
  }
  else
  {
    SD_MSG_MED( "Ignoring event=%d",event,0,0 );
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_RTRN_ACT;
    rtrn_iact_ptr->act      = SDSS_IACT_CONTINUE;
  }

} /* sdss_eng_scr_con_cont */




/* <EJECT> */
/*===========================================================================
//
//FUNCTION sdss_eng_scr_con_cont_redir
//
//DESCRIPTION
//  Implement the CONT_REDIR( TIME ) SS-Script Constructs.
//
//  Conditionally stops script execution and instructs the SS-Client to
//  continue until TIME seconds passed since the most recent redirection
//  indication was received. If instructs SS-Client to continue, sets a timer
//  for the appropriate duration.
//
//DEPENDENCIES
//  None.
//
//RETURN VALUE
//  None.
//
//SIDE EFFECTS
//  None.
//
//===========================================================================*/
//static  void                    sdss_eng_scr_con_cont_redir(
//
//        sdss_eng_s_type         *eng_ptr,
//            /* Pointer to the SS-script-engine that is associated with the
//            ** SS-Script that is being executed.
//            */
//
//        sdss_evt_e_type         event,
//            /* SS-Event coming back from the SS-Front-end.
//            */
//
//        sdss_iact_s_type        *rtrn_iact_ptr
//            /* Pointer to a buffer where to place returned internal action.
//            */
//)
//{
//  /* Time in seconds with which to set-up the timer.
//  */
//  dword                 timer   = eng_ptr->con_prm1;
//  dword                 uptime  = clk_uptime();
//  const sdss_s_type     *ss_ptr = eng_ptr->ss_ptr;
//
//  dword                 time_since_redir;
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  SD_ASSERT( eng_ptr != NULL );
//  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
//  SD_ASSERT( rtrn_iact_ptr != NULL );
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  /* If this is the first time this function is being called for this script
//  ** construct, check whether script execution should continue. If script
//  ** execution should not continue, setup the appropriate timer.
//  */
//  if( eng_ptr->scr_ctrl.ctrl == SDSS_SCR_CTRL_NEXT_CONS )
//  {
//    SD_ASSERT( uptime >= ss_ptr->redir_uptime )
//    time_since_redir = uptime - ss_ptr->redir_uptime;
//
//    if( timer > time_since_redir )
//    {
//      eng_ptr->con_prm1 = (sdssscr_prm_type) (timer - time_since_redir);
//      sdss_eng_scr_con_cont( eng_ptr, event, rtrn_iact_ptr );
//    }
//
//    return;
//  }
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  /* If we got here, this is not the first time this function is being called
//  ** for this script construct, so call on the timer function to process
//  ** this event.
//  */
//  sdss_eng_scr_con_cont( eng_ptr, event, rtrn_iact_ptr );
//
//} /* sdss_eng_scr_con_cont_redir */


/*===========================================================================

FUNCTION sdss_eng_scr_con_cont_set

DESCRIPTION
  Implement the CONT_SET( TIME ) SS-Script Constructs.

  Set the start time of the conditional wait.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_scr_con_cont_set(

        const sdss_eng_s_type   *eng_ptr
      /* Pointer to the SS-script-engine that is associated with the
      ** SS-Script that is being executed.
      */
)
{
  /* Time in seconds with which to set-up the timer.
  */
  word                  cont_type;
  word                  timer_value;
  dword                 uptime       = clk_uptime();
  sdss_s_type           *ss_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cont_type   = eng_ptr->con_prm1;
  timer_value  = eng_ptr->con_prm2;
  ss_ptr      = eng_ptr->ss_ptr;

  SD_ASSERT_ENUM_IS_INRANGE( cont_type, SDSSSCR_CONT_TYPE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( timer_value, SDSSSCR_CONT_VAL_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( timer_value ) {

    case SDSSSCR_CONT_VAL_ZERO:
      /* Reset the timer to zero.
      */
      ss_ptr->cont_uptime[cont_type] = 0;
      break;

    case SDSSSCR_CONT_VAL_UPTIME_IF_ZERO:
      /* If the start time is zero, set to the uptime. Otherwise, do nothing.
      */
      if(ss_ptr->cont_uptime[cont_type] != 0 )
      {
        return;
      }
      ss_ptr->cont_uptime[cont_type] = uptime;
      break;

    case SDSSSCR_CONT_VAL_UPTIME:
      /* Set the start time to uptime.
      */
      ss_ptr->cont_uptime[cont_type] = uptime ;
      break;

    case SDSSSCR_CONT_VAL_INC_BY_BCMCS_LEN:
      /* Pause the timer until BCMCS is end.
      */

      if( ss_ptr->cont_uptime[cont_type] != 0 )
      {
        ss_ptr->cont_uptime[cont_type] +=
           (ss_ptr->hdr_bcmcs_stop_uptime - ss_ptr->hdr_bcmcs_start_uptime);
      }

      break;

    case SDSSSCR_CONT_VAL_MAX:
    default:
      SD_MSG_ERROR(" Invalid CONT VALUE",0,0,0);
      break;
  }

  SD_MSG_HIGH("Set Timer= %d, uptime= %d",
              cont_type, ss_ptr->cont_uptime[cont_type], 0);
  return;

} /* sdss_eng_scr_con_cont_set */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_scr_acpt_rej

DESCRIPTION
  Implement the ACCEPT and REJECT SS-Script Constructs.

  Instructs the SS-Client to accept/reject BS proposed mode, band and/or
  CDMA channel/AMPS system.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_scr_acpt_rej(

        sdss_eng_s_type         *eng_ptr,
            /* Pointer to the SS-script-engine that is associated with the
            ** SS-Script that is being executed.
            */

        sdss_evt_e_type         event,
            /* SS-Event coming back from the SS-Front-end.
            *//*lint -esym(715, event) */ /* not referenced */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{
  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is the first time this function is being called for this script
  ** construct, instruct the SS-Client of accept/reject.
  */
  if( eng_ptr->scr_ctrl.ctrl == SDSS_SCR_CTRL_NEXT_CONS )
  {
    if( eng_ptr->scr_con == SDSSSCR_CON_ACCEPT )
    {
      rtrn_iact_ptr->act    = SDSS_IACT_ACCEPT;
    }
    else
    {
      rtrn_iact_ptr->act    = SDSS_IACT_REJECT;
    }

    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_RTRN_ACT;
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, this is not the first time this function is being called
  ** for this script construct. so continue with the next script construct.
  */
  eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_NEXT_CONS;

} /* sdss_eng_scr_acpt_rej *//*lint +esym(715, event) */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_scr_con_pwr_save

DESCRIPTION
  Implement the PWR_SAVE SS-Script Constructs.

  Instruct the SS-Client to enter Power-Save mode of operation. Once entering
  Power-Save mode of operation, the SS-Client should call on the
  SD_SS_IND_MISC_PWR_SAVE_ENTER SS-Indication function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_scr_con_pwr_save(

        sdss_eng_s_type         *eng_ptr,
            /* Pointer to the SS-script-engine that is associated with the
            ** SS-Script that is being executed.
            */

        sdss_evt_e_type         event,
            /* SS-Event coming back from the SS-Front-end.
            */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{
  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is the first time this function is being called for this script
  ** construct, instruct the SS-Client to enter Power-Save mode of operation.
  ** Once entering power-save mode the SS-Client should call in the
  ** sd_ss_ind_misc_pwr_save_enter() SS-Indication function which in turn
  ** generate an SDSS_EVT_MISC_PWR_SAVE_ENTER SS-Event.
  */
  if( eng_ptr->scr_ctrl.ctrl == SDSS_SCR_CTRL_NEXT_CONS )
  {
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_RTRN_ACT;
    rtrn_iact_ptr->act      = SDSS_IACT_PWR_SAVE;

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here this is not the first time this function is being called
  ** for this script construct. If event is SDSS_EVT_MISC_PWR_SAVE_ENTER,
  ** continue with next script construct. Else, stay and wait for SS-Client
  ** to report that it entered power-save mode of operation.
  */
  if( event == SDSS_EVT_MISC_PWR_SAVE_ENTER )
  {
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_NEXT_CONS;
  }
  else
  {
    SD_MSG_MED( "Ignoring event=%d",event,0,0 );
    eng_ptr->scr_ctrl.ctrl  = SDSS_SCR_CTRL_RTRN_ACT;
    rtrn_iact_ptr->act      = SDSS_IACT_CONTINUE;
  }

} /* sdss_eng_scr_con_pwr_save */




/* <EJECT> */
/*===========================================================================
=============================================================================
================== CONDITION SS-SCRIPT-CONSTRUCTS FUNCTIONS =================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdss_eng_scr_con_is

DESCRIPTION
  Implement the IS( VAL ) condition SS-Script Constructs.

  Evaluates to TRUE iff VAL is equal to a TRUE boolean condition.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if VAL is equal to a TRUE boolean condition. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                 sdss_eng_scr_con_is(

        const sdss_eng_s_type   *eng_ptr
            /* Pointer to the SS-script-engine that is associated with the
            ** SS-Script that is being executed.
            */
)
{
  /* Condition to be checked.
  */
  sdssscr_cond_e_type   cond;

  const sdss_s_type      *ss_ptr;
  sd_ss_mode_pref_e_type  mode_pref;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cond   = (sdssscr_cond_e_type) eng_ptr->con_prm1;
  ss_ptr = eng_ptr->ss_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( cond )
  {
    case SDSSSCR_COND_REDIR_VALID:
      if( ss_ptr->redir_is_valid )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_REDIR_RIF:
      if( ss_ptr->redir_is_rif )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_REDIR_WILDCARD_SID:
      if( ss_ptr->redir_expected_sid == SD_WILDCARD_SID )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_REDIR_WILDCARD_NID:
      if( ss_ptr->redir_expected_nid == SD_WILDCARD_NID )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_REDIR_IGNORE_CDMA:
      if( ss_ptr->redir_is_ignore_cdma )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_CAP_AMPS:
      if( sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_AMPS,
                                      SD_SS_BAND_PREF_ANY) )
      {
        return TRUE;
      }
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_CAP_GSM:
      if( sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_GSM,
                                      SD_SS_BAND_PREF_ANY)
        )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_CAP_GW:
      if( sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_GW,
                                      SD_SS_BAND_PREF_ANY)
        )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_CAP_GW_NO_1X:
      if( sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_GW,
                                      SD_SS_BAND_PREF_ANY)
                      &&
          !sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_CDMA_AMPS_HDR,
                                       SD_SS_BAND_PREF_ANY)
        )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_CAP_CDMA_CELL:
      if( sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_CDMA,
                                      SD_SS_BAND_PREF_CELL) )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_CAP_CDMA_PCS:
      if( sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_CDMA,
                                      SD_SS_BAND_PREF_PCS) )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_CAP_CDMA_CELL_NO_PCS:
      if( sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_CDMA,
                                      SD_SS_BAND_PREF_CELL)
                                &&
          ! sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_CDMA,
                                        SD_SS_BAND_PREF_PCS) )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_CAP_CDMA_PCS_NO_CELL:
      if( sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_CDMA,
                                      SD_SS_BAND_PREF_PCS)
                                &&
          ! sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_CDMA,
                                        SD_SS_BAND_PREF_CELL) )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_CDMA_LOCKED:
      if( ss_ptr->is_cmda_locked )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_SRDA:
      if( ss_ptr->is_srda )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_ORIG_MODE_NORM_CNG:

      if( ss_ptr->orig_mode == ss_ptr->orig_mode_prev )
      {
        return TRUE;
      }

      if( (ss_ptr->orig_mode == SD_SS_ORIG_MODE_NONE   ||
           ss_ptr->orig_mode == SD_SS_ORIG_MODE_NORMAL ||
           ss_ptr->orig_mode == SD_SS_ORIG_MODE_COLLOC)
                               &&
          (ss_ptr->orig_mode_prev == SD_SS_ORIG_MODE_NONE   ||
           ss_ptr->orig_mode_prev == SD_SS_ORIG_MODE_NORMAL ||
           ss_ptr->orig_mode_prev == SD_SS_ORIG_MODE_COLLOC) )
      {
        return TRUE;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_FEATURE_JCDMA:
      #ifdef FEATURE_JCDMA
        return TRUE;
      #else
        break;
      #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_FEATURE_T53:
      #ifdef FEATURE_T53
        return TRUE;
      #else
        break;
      #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_TRUE_MODE_PREF_GSM:
      #if defined(FEATURE_GSM)
#error code not present
      #endif
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_SRV_DOMAIN_PREF_CS_PS:
      if ( ss_ptr->domain_pref == SD_SS_SRV_DOMAIN_PREF_CS_PS )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_CAP_GW_PREF_GW_ONLY:

      mode_pref =  sdss_get_supp_mode_pref(ss_ptr->mode_pref, ss_ptr->band_pref);

      if( sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_GW,
                                      SD_SS_BAND_PREF_ANY)
          &&
          ( mode_pref == SD_SS_MODE_PREF_GW   ||
            mode_pref == SD_SS_MODE_PREF_GSM  ||
            mode_pref == SD_SS_MODE_PREF_WCDMA)
          )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case SDSSSCR_COND_MODE_PREF_WLAN:

      ss_ptr = sdss_ptr(SD_SS_HYBR_WLAN);

      if ( SD_MODE_CONTAIN( ss_ptr->mode_pref, BM(SD_MODE_WLAN)) )
      {
        return TRUE;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_FEATURE_UNIFORM_SCAN_OOS:
      #ifdef FEATURE_UNIFORM_SCAN_OOS
      return TRUE;
      #else
      return FALSE;
      #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_FEATURE_UMTS_UNIFORM_OOS_HANDLING:
      #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
      #else
        return FALSE;
      #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_FEATURE_UOOS:
      #if (defined (FEATURE_UMTS_UNIFORM_OOS_HANDLING) || defined ( FEATURE_UNIFORM_SCAN_OOS ) )
        return TRUE;
      #else
        break;
      #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_FALSE:
      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_WLAN_NOT_USABLE :
      return (ss_ptr->wlan_fail_reason == SD_SS_WLAN_ACQ_FAIL_WLAN_NOT_USABLE);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_1X_GW_MODE_PREF:

      mode_pref =  sdss_get_supp_mode_pref(ss_ptr->mode_pref, ss_ptr->band_pref);

      if ( ( SD_MODE_CONTAIN( mode_pref, BM(SD_MODE_GW)) ||
             SD_MODE_CONTAIN( mode_pref, BM(SD_MODE_GSM))||
             SD_MODE_CONTAIN( mode_pref, BM(SD_MODE_WCDMA)) ) &&
           ( SD_MODE_CONTAIN( mode_pref, BM(SD_MODE_CDMA)) ) )
      {
        return TRUE;
      }
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDSSSCR_COND_CAP_HDR_PREF_HDR_ONLY:

      mode_pref =  sdss_get_supp_mode_pref(ss_ptr->mode_pref, ss_ptr->band_pref);

      if( sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_HDR,
                                      SD_SS_BAND_PREF_ANY)
          &&
          ( mode_pref == SD_SS_MODE_PREF_HDR )
        )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_NO_HYBR_OPR:

      #ifdef FEATURE_HDR_HYBRID
#error code not present
      #else
      /*lint -e527 */
      break;
      /*lint +e527 */
      #endif
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_CAP_CDMA_HDR_PREF_CDMA_HDR_INCL:

      mode_pref =  sdss_get_supp_mode_pref(ss_ptr->mode_pref, ss_ptr->band_pref);

      if( sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_CDMA_HDR,
                                      SD_SS_BAND_PREF_ANY)
          &&
          ( SD_MODE_CONTAIN( mode_pref, BM(SD_MODE_CDMA)) )
          &&
          ( SD_MODE_CONTAIN( mode_pref, BM(SD_MODE_HDR)) )
        )
      {
        return TRUE;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_COND_SIM_DOMAIN_VALID:
      #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)    
#error code not present
      #endif
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

    case SDSSSCR_COND_1X_GW_TRUE_MODE_PREF:

      mode_pref =  sdss_get_supp_mode_pref(ss_ptr->true_mode_pref, ss_ptr->band_pref);

      if ( ( SD_MODE_CONTAIN( mode_pref, BM(SD_MODE_GW)) ||
             SD_MODE_CONTAIN( mode_pref, BM(SD_MODE_GSM))||
             SD_MODE_CONTAIN( mode_pref, BM(SD_MODE_WCDMA)) ) &&
           ( SD_MODE_CONTAIN( mode_pref, BM(SD_MODE_CDMA)) ) )
      {
        return TRUE;
      }
      break;
      
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
    case SDSSSCR_COND_MAX:
    default:
      SD_CASE_ERR( "cond",0,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here condition is FALSE.
  */
  return FALSE;

} /* sdss_eng_scr_con_is */


/*===========================================================================

FUNCTION sdss_eng_scr_con_acq_mode

DESCRIPTION
  Set the acq mode

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean               sdss_eng_scr_con_acq_mode(

        sdss_eng_s_type         *eng_ptr,
            /* Pointer to the SS-script-engine that is associated with the
            ** SS-Script that is being executed.
            */

        sdss_s_type           *ss_ptr,
            /* Pointer to the SS object
            */

        sdss_acq_mode_e_type  acq_mode
            /* The acq_mode to set to
            */
)
/*lint -esym(818, ss_ptr) */
/* Pointer parameter 'ss_ptr' could be declared as pointing to const */
{

  boolean is_acq_mode_processed = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( acq_mode, SDSS_ACQ_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while( !is_acq_mode_processed )
  {
    switch ( acq_mode )
    {
      case SDSS_ACQ_MODE_PREV:

        if( ss_ptr->last_acq_eng_ptr != NULL )
        {
          eng_ptr->acq_mode =
                  ((sdss_eng_s_type * )ss_ptr->last_acq_eng_ptr)->acq_mode;
        }
        else
        {
          SD_ERR( "Invalid acq_mode %d",eng_ptr->con_prm1, 0, 0 );
          eng_ptr->acq_mode = SDSS_ACQ_MODE_FULL;
        }
        is_acq_mode_processed = FALSE;
        acq_mode = eng_ptr->acq_mode;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSS_ACQ_MODE_UNIFORM_FULL:
      case SDSS_ACQ_MODE_UNIFORM_FULL2:

        #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
        #else
        sdss_reset_gw_acq_para( eng_ptr->parser.ss );
        #endif

        if( acq_mode == SDSS_ACQ_MODE_UNIFORM_FULL )
        {
          eng_ptr->acq_mode               = SDSS_ACQ_MODE_FULL;
        }
        else
        {
          eng_ptr->acq_mode               = SDSS_ACQ_MODE_FULL2;
        }
        is_acq_mode_processed = TRUE;
        return TRUE;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSS_ACQ_MODE_UNIFORM_FULL_RTN:
      case SDSS_ACQ_MODE_UNIFORM_FULL_RTN2:

        #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
        #else
        sdss_reset_gw_acq_para( eng_ptr->parser.ss );
        #endif
        eng_ptr->acq_mode               = acq_mode;
        is_acq_mode_processed = TRUE;
        return TRUE;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSS_ACQ_MODE_FULL:
      case SDSS_ACQ_MODE_MINI:
      case SDSS_ACQ_MODE_MICRO:
      case SDSS_ACQ_MODE_FULL2:
      case SDSS_ACQ_MODE_MINI2:
      case SDSS_ACQ_MODE_MICRO2:
      case SDSS_ACQ_MODE_DEEP:
      case SDSS_ACQ_MODE_DEEP2:
      case SDSS_ACQ_MODE_SHALLOW:
      case SDSS_ACQ_MODE_SHALLOW2:

        eng_ptr->acq_mode               = acq_mode;
        is_acq_mode_processed = TRUE;
        return TRUE;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSS_ACQ_MODE_HO:
        eng_ptr->acq_mode               = acq_mode;
        is_acq_mode_processed = TRUE;
        return TRUE;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSS_ACQ_MODE_MAX:
      default:

        SD_CASE_ERR( "Invalid acq_mode %d", acq_mode, 0, 0 );
        is_acq_mode_processed = TRUE;
        return FALSE;
    }
  }
  return TRUE;
} /* sdss_eng_scr_con_acq_mode */
/*lint +esym(818, ss_ptr) */

/* <EJECT> */
/*===========================================================================
=============================================================================
====================== MAIN ENGINE PROCESSING ROUTINES ======================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdss_eng_event_proc

DESCRIPTION
  Process an SS-Event in the context of the SS-Script that is currently
  associated with the script engine.

DEPENDENCIES
  The SS-Script-engine that is pointed to by eng_ptr must have already been
  initialized with an SS-Script via a call to sdss_eng_init().

RETURN VALUE
  SS-Internal-Action to be taken by the SS-Front-end (through the
  rtrn_iact_ptr).

SIDE EFFECTS
  None.

===========================================================================*/
static  void                  sdss_eng_event_proc(

        sdss_eng_s_type       *eng_ptr,
            /* Pointer to the script engine that is to process the
            ** SS-Event */

        sdss_evt_e_type       event,
            /* SS-Event to process */


        sdss_iact_s_type      *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action */
)
{
  sdss_s_type     *ss_ptr;
  sd_ss_e_type     ss         = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr         = eng_ptr->ss_ptr;
  ss             = eng_ptr->ss;
  eng_ptr->event = event;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do script processing (as long as instructed to process the next script
  ** construct).
  */
  do
  {
    /* If necessary, parse the next script construct.
    */
    if( eng_ptr->scr_ctrl.ctrl == SDSS_SCR_CTRL_NEXT_CONS )
    {
      /* Skip over any condition script constructs that might be prepended
      ** to the script.
      */
      do
      {
        eng_ptr->scr_con = sdssscr_parser_next( &eng_ptr->parser,
                                                clk_uptime(),
                                                ss_ptr->scr_ctrl_cnt,
                                                &eng_ptr->con_prm1,
                                                &eng_ptr->con_prm2,
                                                &eng_ptr->con_prm3,
                                                &eng_ptr->con_prm4,
                                                &eng_ptr->con_prm5,
                                                &eng_ptr->con_prm6 );

      } while( SDSSSCR_CON_IS_COND(eng_ptr->scr_con) );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    switch( eng_ptr->scr_con )
    {

      /* --------------------------------------
      ** List Manipulator SS-Script-Constructs.
      ** --------------------------------------
      */

      case SDSSSCR_CON_CLR:

        /* CLR( LIST ) - Clears the system record list/item LIST.
        */
        sdsr_list_clr( ENG_LIST(eng_ptr->con_prm1) );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_CPY:

        /* CPY( LIST_TO, LIST_FROM ) - Clear the system record list LIST_TO
        ** and copy all the elements from the system record list LIST_FROM
        ** into the system record list LIST_TO
        */
        sdsr_list_cpy( ENG_LIST(eng_ptr->con_prm1),
                       ENG_LIST(eng_ptr->con_prm2) );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_DEL:

        /* DEL( LIST, LIST_REF ) - Delete the system records that are
        ** referenced by the system record list LIST_REF from the system
        ** record list LIST.
        */
        sdsr_list_del_list( ENG_LIST(eng_ptr->con_prm1),
                            ENG_LIST(eng_ptr->con_prm2) );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_DEL_CNT:

        /* DEL_CNT( LIST, START_POS, CNT ) - Delete 'CNT' entries from LIST
        ** starting from START_POS
        */
        sdsr_list_del_list_cnt( ENG_LIST(eng_ptr->con_prm1),
                                (int)eng_ptr->con_prm2,
                                (int)eng_ptr->con_prm3 );


        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_DEL_SYS:

        /* DEL_SYS( LIST, DEL_CAT ) - Delete system records from LIST
        ** according to DEL_CAT.
        */
        sdsr_list_del_sys( eng_ptr->ss,
                           ENG_LIST(eng_ptr->con_prm1),
                           (sdsr_cat_e_type) eng_ptr->con_prm2 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_DEL_BAND:

        /* DEL_BAND( LIST, SYS_REF ) - Delete system records from LIST
        ** which have not the same band from the reference sys
        */
        sdsr_list_del_band( ENG_LIST(eng_ptr->con_prm1),
                            ENG_LIST(eng_ptr->con_prm2) );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      case SDSSSCR_CON_APP:

        /* APP( LIST_TO, LIST_FROM ) - Append the elements from the system
        ** record list/item LIST_FROM to the end of system record list
        ** LIST_TO. Duplicate elements are not appended.
        */
        sdsr_list_app( ENG_LIST(eng_ptr->con_prm1),
                       ENG_LIST(eng_ptr->con_prm2) );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ORDER:

        /* ORDER( LIST, LIST_REF ) - Order the elements of the system record
        ** list LIST according to their order in the reference list LIST_REF.
        */
        sdsr_list_order( ENG_LIST(eng_ptr->con_prm1),
                         ENG_LIST(eng_ptr->con_prm2) );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ORDER2:

        /* ORDER2( LIST, LIST_REF, ORDER_CAT ) - Order the elements of the
        ** system record list LIST according to the ORDER_CAT field of the
        ** reference sytem in the list LIST_REF.
        */
        sdsr_list_order2( ENG_LIST(eng_ptr->con_prm1),
                          ENG_LIST(eng_ptr->con_prm2),
                          (sdsr_order_e_type) ENG_LIST(eng_ptr->con_prm3) );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_SORT:

        /* SORT( LIST, COMP_CAT ) - Sort the system record list LIST
        ** according to the COMP_CAT category.
        */
        sdsr_list_sort( ENG_LIST(eng_ptr->con_prm1),
                        (sdsr_comp_e_type) eng_ptr->con_prm2 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_GEO_EXT:

        /* GEO_EXT( LIST, LIST_REF, EXT_CAT ) - Extract systems from the GEO
        ** of LIST_REF[0] (i.e., 1st system on LIST_REF) into LIST according
        ** to the EXT_CAT category.
        */
        sdsr_list_clr( ENG_LIST(eng_ptr->con_prm1) );

        ( void)sdprl_get_info_base(ss,
                                   ENG_LIST(eng_ptr->con_prm2),
                                   0,
                                   ENG_LIST(eng_ptr->con_prm2),
                                   0,
                                   ENG_LIST(eng_ptr->con_prm1),
                                   SDSR_MAX,
                                   SDSR_MAX,
                                   (sdprl_extract_e_type) eng_ptr->con_prm3);

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_CREATE_GW_LTD:

        /* CREATE_GW_LTD( LIST ) - create a conterpart GW limited list from
        ** the system record list/item LIST and store in the system record
        ** database
        */
        sdss_sr_list_create( ss, ENG_LIST(eng_ptr->con_prm1) );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* -----------------------------------
      ** List Operator SS-Script-Constructs.
      ** -----------------------------------
      */

      case SDSSSCR_CON_MEAS:

        /* MEAS( LIST, MEAS_MODE, MEAS_TYPE ) - Request a channel strength
        ** measurement of mode MEAS_MODE and type MEAS_TYPE for all the
        ** systems that are referenced by the system record list LIST.
        */
        sdss_eng_scr_con_meas( eng_ptr, event, rtrn_iact_ptr );

        break;


      case SDSSSCR_CON_GET_NET:

        #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
        /* GET_NET - Request to get the available networks
        */
        sdss_eng_scr_con_get_net( ss, eng_ptr, event, rtrn_iact_ptr );
        #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */
        break;


      case SDSSSCR_CON_PWR_SCAN_GW:

        #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
        /*  PWR_SCAN_GW - Request to do early power scan in GSM/WCDMA.
        */
        sdss_eng_scr_con_pwr_scan_gw( eng_ptr, event, rtrn_iact_ptr );
        #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */
        break;


      case SDSSSCR_CON_SURVEY_WLAN:

        /* SURVEY_WLAN - Request to get the available networks
        */
        sdss_eng_scr_con_survey_wlan( eng_ptr, event, rtrn_iact_ptr );
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ:

        /* ACQ( LIST, ACQ_TYPE ) - Step through the system record list LIST
        ** (first to last) and try to acquire systems that are referenced by
        ** LIST and that were not already attempted acquisition since the
        ** last time the NEW_ACQ construct was encountered (and in accordance
        ** with ACQ_TYPE and the current SS-Preference.
        */
        sdss_eng_scr_con_acq( eng_ptr, event, rtrn_iact_ptr );

        break;

      case SDSSSCR_CON_ACQ_TRUE_MODE:

        /* ACQ_TRUE_MODE( LIST, ACQ_TYPE ) - Step through the system record list LIST
        ** (first to last) and try to acquire systems that are referenced by
        ** LIST and that were not already attempted acquisition since the
        ** last time the NEW_ACQ construct was encountered (and in accordance
        ** with ACQ_TYPE and the current SS-Preference.
        ** 
        ** The difference between SDSSSCR_CON_ACQ and SDSSSCR_CON_ACQ_TRUE_MODE is:
        ** in SDSSSCR_CON_ACQ_TRUE_MODE ss_ptr's true_mode_pref is used instead of
        ** ss_ptr's mode_pref.
        ** This is to enable world mode phones to search for Limited GW
        ** when there is no GW_service.
        */
        sdss_eng_scr_con_acq( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_MODE:

        /* ACQ_MODE( MODE ) - Set the CDMA acquisition mode to MODE.
        */
        SD_ASSERT_ENUM_IS_INRANGE( (sdss_acq_mode_e_type) eng_ptr->con_prm1,
                                   SDSS_ACQ_MODE_MAX );

        (void) sdss_eng_scr_con_acq_mode( eng_ptr, ss_ptr,
                                   (sdss_acq_mode_e_type) eng_ptr->con_prm1 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_RESET_GW_UOOS_ACQ_PARA:

        /* RESET_GW_UOOS_ACQ_PARA - Reset the GW acq parameters */
        sdss_reset_gw_acq_para( eng_ptr->ss );
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case SDSSSCR_CON_NEW_ACQ:

        /* NEW_ACQ - Start a new acquisition by incrementing the
        ** new-acquisition counter.
        **
        ** Note that the ACQ() acquisition constructs only attempts to
        ** acquire system that were not already attempted acquisition since
        ** the last time the NEW_ACQ construct was encountered.
        */
        ss_ptr->new_acq_cnt++;
        if( ss_ptr->new_acq_cnt == 0 )
        {
          ss_ptr->new_acq_cnt++;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_START_TIME:

        /* START_ACQ_TIME - Starts the acquisition timer.
        */
        {
          qword time_now;
          clk_uptime_ms( time_now );

          /* Lint complains about suspicious cast, time_now is of type qword and
          ** we are casting it to uint64 and then taking the modulus of the
          ** resulting number
          */
          /*lint -e740 */

          if( (QW_CVT_Q2N(time_now) % 1000 ) >= 500 )
          /*lint +e740 */
          {
            ss_ptr->acq_start_time = clk_uptime() + 1;
          }
          else
          {
            ss_ptr->acq_start_time = clk_uptime();
          }

          SD_MSG_HIGH(" Time: clk_uptime %d , ms: %d, acq_start_time: %d",
                      clk_uptime(),(QW_CVT_Q2N(time_now) % 1000 ),
                      ss_ptr->acq_start_time );
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_ALL:

        /* ACQ_ALL( LIST, ACQ_TYPE ) - Equivalent to the ACQ construct except
        ** that also systems that were already attempted acquisition since
        ** the last time the NEW_ACQ construct was encountered are being
        ** attempted acquisition.
        */
        sdss_eng_scr_con_acq( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_CNT:

        /* ACQ_CNT( LIST, ACQ_TYPE, START_POS, CNT ) - Equivalent to the ACQ
        ** construct except that LIST is traversed START_POS to START_POS-1
        ** and acquisition is bounded to CNT elements.
        */
        sdss_eng_scr_con_acq( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_TIME:

        /* ACQ_TIME( LIST, ACQ_TYPE, START_POS, TIME ) - Equivalent to the
        ** ACQ construct except that LIST is traversed START_POS to
        ** START_POS-1 and acquisition is bounded to TIME seconds since the
        ** START_TIME script construct was last encountered.
        */
        sdss_eng_scr_con_acq( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_TIME2:

        /* ACQ_TIME2( LIST, ACQ_TYPE, START_POS, NV_ITEM ) - Equivalent to the
        ** ACQ_TIME construct except the timer value is from NV
        */
        sdss_eng_scr_con_acq( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_TIME3:

        /* ACQ_TIME3( LIST, ACQ_TYPE, START_POS, NV_ITEM ) - Equivalent to the
        ** ACQ_TIME2 construct except the timer value from NV is in ms
        */
        sdss_eng_scr_con_acq( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_CNT_OR_TIME:

        /* ACQ_CNT_OR_TIME( LIST, ACQ_TYPE, START_POS, MAGIC_CONSTANT ) -
        ** Equivalent to the
        ** ACQ construct except that LIST is traversed START_POS to
        ** START_POS-1 and acquisition is bounded to TIME seconds or CNT
        ** since the START_TIME script construct was last encountered.
        */
        sdss_eng_scr_con_acq( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_RX:

        /* ACQ_RX( LIST, ACQ_TYPE, RX ) - Equivalent to he ACQ construct
        ** except that only systems with Rx-power measurement value >= RX
        ** are being attempted acquisition.
        */
        sdss_eng_scr_con_acq( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_PI:

        /* ACQ_PI( LIST, ACQ_TYPE, PI ) - Equivalent to the ACQ construct
        ** except that only systems with pilot measurement value >= PI are
        ** being attempted acquisition.
        */
        sdss_eng_scr_con_acq( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_PI_X:

        /* ACQ_PI_X( LIST, ACQ_TYPE, PI, X ) - Equivalent to the ACQ construct
        ** except that only X tech systems with pilot measurement value >= PI and
        ** all other system present in list are being attempted acquisition.
        */
        sdss_eng_scr_con_acq( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case SDSSSCR_CON_ACQ_NETLIST:

        /* ACQ_NETLIST( LIST, ACQ_TYPE, NETLIST ) - Equivalent to the ACQ
        ** construct except that acquistion is issued only when background
        ** service search yields >=1 PLMN.
        */
        sdss_eng_scr_con_acq( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_SCHED:

        /* SCHED( LIST, ACQ_TYPE, SCHED ) - Schedule the system records that
        ** are referenced by the system record list/item LIST to be attempted
        ** acquisition of type ACQ_TYPE at time points that are specified by
        ** SCHED.
        */
        sdss_eng_scr_con_sched( eng_ptr, SDSSSR_SCHED_COND_UNCONDITIONAL,
                                rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_SCHED_COND:

        /* SCHED_COND( LIST, SCHED, ACQ_TYPE ) - Identical to SCHED(),
        ** except that only schedule systems that are not already on equal or
        ** higher priority reacquisition schedule.
        */
        sdss_eng_scr_con_sched( eng_ptr,SDSSSR_SCHED_COND_NOT_ON_HIGH_SCHED,
                                rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_SCHED_COND2:

        /* SCHED_COND2( LIST, SCHED, ACQ_TYPE, SCHED_COND ) - Identical to SCHED(),
        ** except that only schedule systems that are not already on equal or
        ** higher priority reacquisition schedule than CMP_SCHED.
        */
        sdss_eng_scr_con_sched( eng_ptr,
                                ( sdsssr_sched_cond_e_type) eng_ptr->con_prm4,
                                rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_SCHED_2:

        /* SCHED_2( LIST, SCHED, ACQ_TYPE, ACQ_MODE ) - Same as SCHED(), but
        ** also specifies the acquisition mode (FULL, MINI, etc.) to be used
        ** for the reacquisition attempts.
        */
        sdss_eng_scr_con_sched( eng_ptr, SDSSSR_SCHED_COND_UNCONDITIONAL,
                                rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_AVOID:

        /* AVOID( LIST, TIME ) - Mark the system records that are referenced
        ** by the system record list/item LIST to be avoided from any
        ** acquisition attempts for TIME seconds.
        */
        sdss_sr_list_avoid( eng_ptr->ss,
                            ENG_LIST(eng_ptr->con_prm1),
                            eng_ptr->con_prm2,
                            SDSSSCR_SCHED_AVOID );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_AVOID_CANCEL_COND:

        /* AVOID_CANCEL_COND( LIST, COND ) - Remove the avoidance placed on
        ** systems that are referenced by LIST and have been avoided till
        ** condition COND is met.
        ** AVOID_CANCEL( LIST ) - Same as above, with COND equal to
        ** CANCEL_ALL.
        */
        sdss_sr_list_avoid( eng_ptr->ss,
                            ENG_LIST(eng_ptr->con_prm1),
                            0,
                            (sdssscr_sched_e_type) eng_ptr->con_prm2 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_AVOID_COND:

        /* AVOID_COND( LIST, TIME, COND ) - Mark the system records that are
        ** referenced by the system record list/item LIST to be avoided from
        ** any acquisition attempts for TIME seconds or until the condition
        ** is met whichever comes first.
        */
        sdss_sr_list_avoid( eng_ptr->ss,
                            ENG_LIST(eng_ptr->con_prm1),
                            eng_ptr->con_prm2,
                            (sdssscr_sched_e_type) eng_ptr->con_prm4 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* -----------------------------
      ** SS-Script Control Constructs.
      ** -----------------------------
      */

      case SDSSSCR_CON_LOOP_TIME:

        /* LOOP_TIME( TIME ) - Repeat executing the script between this
        ** construct and the matching LOOP construct for TIME seconds since
        ** this construct was first encountered.
        */
        SD_CASE_ERR( "SDSSSCR_CON_LOOP_TIME",0,0,0 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_LOOP_NUM:

        /* LOOP_NUM( NUM ) - Repeat executing the script between this
        ** construct and the matching LOOP construct for NUM times.
        */
        SD_CASE_ERR( "SDSSSCR_CON_LOOP_NUM",0,0,0 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_LOOP_LIST:

        /* LOOP_LIST( LIST ) - Repeat executing the script between this
        ** construct and the matching LOOP construct for count(LIST) times.
        */
        SD_CASE_ERR( "SDSSSCR_CON_LOOP_LIST",0,0,0 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_LOOP_COND:

        /* LOOP_COND - Repeat executing the script between this construct
        ** and the matching LOOP construct until the COND is TRUE.
        */
        SD_CASE_ERR( "SDSSSCR_CON_LOOP_COND",0,0,0 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      case SDSSSCR_CON_LOOP_FOREVER:

        /* LOOP_FOREVER - Repeat executing the script between this construct
        ** and the matching LOOP construct forever.
        */
        SD_CASE_ERR( "SDSSSCR_CON_LOOP_FOREVER",0,0,0 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_LOOP:

        /* LOOP - Loop-back to the matching LOOP_XXX construct.
        */
        SD_CASE_ERR( "SDSSSCR_CON_LOOP",0,0,0 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_PREV_SCR:

        /* PREV_SCR - Return to perviously active SS-Script and continue with
        ** that script execution.
        */
        eng_ptr->scr_ctrl.ctrl = SDSS_SCR_CTRL_PREV_SCR;

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_PROC_EVT:

        /* PROC_EVT( SS_EVENT ) - Process the SS_EVENT SS-Event.
        */
        sdss_eng_scr_con_proc_evt( eng_ptr, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACT_SCR:

        /* ACT_SCR( SCRIPT ) - Make the script that is referenced SCRIPT the
        ** active SS-Script.
        */
        sdss_eng_scr_con_act_scr( eng_ptr, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACT_SCR_RX:

        /* ACT_SCR_RX( SCRIPT, LIST, RX ) - Make SCRIPT the active System
        ** Selection Script only if at least one of the systems that are
        ** referenced by the system record list LIST has an Rx-power
        ** measurement value >= RX.
        */
        sdss_eng_scr_con_act_meas( eng_ptr, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACT_SCR_PI:

        /* ACT_SCR_PI( SCRIPT, LIST, PI ) - Make SCRIPT the active System
        ** Selection Script only if at least one of the systems that are
        ** referenced by system record list LIST has a pilot measurement
        ** value >= PI.
        */
        sdss_eng_scr_con_act_meas( eng_ptr, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IF:

        /* Beginning of IF block
        */
        SD_CASE_ERR( "SDSSSCR_CON_IF",0,0,0 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ELSE:

        /* Beginning of ELSE block
        */
        SD_CASE_ERR( "SDSSSCR_CON_ELSE",0,0,0 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ENDIF:

        /* End of IF block
        */
        SD_CASE_ERR( "SDSSSCR_CON_ENDIF",0,0,0 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_END:

        /* END - Mark the end of an SS-Script. Under normal situation this
        ** script construct should never be reached.
        */
        //SD_ERR_FATAL( "SCR_CON_END",0,0,0 );
        SD_ASSERT( FALSE );
        ERR( "=SD= CON_END, evt=%d, ss=%d",event,ss,0 );

        /* If no further error recovery should be performed, return the
        ** SS-Action set up by the error recovery function. Else, process the
        ** alternate SS-Event set by the error recovery function.
        */
        if( ! sdss_event_recovery(ss, &event, rtrn_iact_ptr) )
        {
          eng_ptr->scr_ctrl.ctrl = SDSS_SCR_CTRL_RTRN_ACT;
        }
        else
        {
          eng_ptr->scr_ctrl.ctrl = SDSS_SCR_CTRL_PROC_EVT;
          eng_ptr->scr_ctrl.prm.proc.event  = event;
        }

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      /* -----------------------------------
      ** Miscellaneous SS-Script-Constructs.
      ** -----------------------------------
      */
      case SDSSSCR_CON_CONT_GW_UOOS_REM_SLEEP :
      case SDSSSCR_CON_CONT:

        /* CONT( TIME_HI, TIME_LO ) - Instructs the SS-Client to continue
        ** with its current activity while setting-up a TIME timer. When this
        ** timer expires the SS-Client shall call on the SD_SS_IND_MISC_TIMER
        ** SS-Indication function.
        */
        sdss_eng_scr_con_cont( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_CONT_REDIR:

        /* CONT_REDIR( TIME ) - Conditionally stops script execution and
        ** instructs the SS-Client to continue until TIME seconds passed
        ** since the most recent redirection indication was received.
        */
        sdss_eng_scr_con_cont( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_CONT_WAIT:

        /* CONT_WAIT( TIME ) - Conditionally stops script execution and
        ** instructs the SS-Client to continue until TIME seconds passed
        ** since the start time was set.
        */
        sdss_eng_scr_con_cont( eng_ptr, event, rtrn_iact_ptr );

        break;

      case SDSSSCR_CON_CONT2:

        /* CONT2( NV_ITEM ) - Instructs the SS-Client to continue with its
        ** current activity while setting-up a TIME second timer. When this
        ** timer expires the SS-Client shall call on the SD_SS_IND_MISC_TIMER
        ** SS-Indication function. Where TIME is the value stored in the
        ** configurable items table at index SDSS_NV_ITEM_"NV_ITEM".
        */
        sdss_eng_scr_con_cont( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case SDSSSCR_CON_CONT3:

        /* CONT3( NV_ITEM1, NV_ITEM2, NV_ITEM3 ) - Instructs the SS-Client to
        ** continue with its current activity while setting-up a TIME timer.
        ** When this timer expires the SS-Client shall call on the
        ** SD_SS_IND_MISC_TIMER SS-Indication function.Where TIME is the
        ** value stored in the configurable items table
        ** SDSS_NV_ITEM_NV_ITEM1 + NV_ITEM3 *( NV_ITEM1 / NV_ITEM2 )
        */

        if( eng_ptr->con_prm3 == 0 )
        {
          eng_ptr->con_prm2 =
            (word)( (SDSS_DEFAULT_UOOS_DUTY_CYCLE * eng_ptr->con_prm4)/1000 ) +
            eng_ptr->con_prm2;
        }
        else
        {
          eng_ptr->con_prm2 = (word)
          ((eng_ptr->con_prm2/(eng_ptr->con_prm3+SD_UOOS_SCAN_TIME_OFFSET)) *
          eng_ptr->con_prm4)/1000 + eng_ptr->con_prm2;
        }
        eng_ptr->con_prm3 = (word) SDSSSCR_CONT_TYPE_NONE;
        sdss_eng_scr_con_cont( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      case SDSSSCR_CON_CONT2_REDIR:

        /* CONT2_REDIR( NV_ITEM) - Conditionally stops script execution and
        ** instructs the SS-Client to continue until TIME seconds passed
        ** since the most recent redirection indication was received. If
        ** instructs SS-Client to continue, sets a timer for the appropriate
        ** duration. Where TIME is the value stored in the configurable items
        ** table at index SDSS_NV_ITEM_"NV_ITEM".
        */
        sdss_eng_scr_con_cont( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_CONT2_WAIT:

        /* CONT2_WAIT( NV_ITEM ) - Instructs the SS-Client to continue with
        ** its current activity if a TYPE specific timer does not expires.
        ** When this timer expires the SS-Client shall call on the
        ** SD_SS_IND_MISC_TIMER SS-Indication function.
        */
        sdss_eng_scr_con_cont( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_CONT3_WAIT:

        /* CONT3_WAIT( TYPE1, NV_ITEM1, TYPE2, NV_ITEM2) - Instructs the
        ** SS-Client to continue with its current activity if either a TYPE1
        ** or a TYPE2 specific timer does not expires. When this timer expires
        ** the SS-Client shall call on the SD_SS_IND_MISC_TIMER SS-Indication
        ** function.
        */
        sdss_eng_scr_con_cont( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_CONT_SET:

        /* CONT_WAIT_SET( TIME ) - Set the start time.
        */
        sdss_eng_scr_con_cont_set( eng_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_MAINTAIN_TIMER:

        /* MAINTAIN_TIMER - Maintain the timer
        */
        eng_ptr->scr_timer = sd_ss_get_timer(eng_ptr->ss);
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_MAINTAIN_PREV_SCR_TIMER:

        /* MAINTAIN_PREV_SCR_TIMER - Maintain the previous script timer
        */
        eng_ptr->scr_timer = ss_ptr->eng_scr_timer;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_SET_TIMER:

        /* SET_TIMER( NV_ITEM, TMR_ADJ ) - Set the timer to prm 1 - prm 
        */
        if( eng_ptr->con_prm2 == SDSSSCR_NV_ITEM_TMR_CONN_MODE_UOOS_SCAN )
        {
          eng_ptr->scr_timer = clk_uptime() + eng_ptr->con_prm1 - 
                               ( ss_ptr->rem_uoos_awake_time / 1000 );
        }
        else
        {
          eng_ptr->scr_timer = clk_uptime() + eng_ptr->con_prm1 - 
                                                           eng_ptr->con_prm2;
        }
        ss_ptr->eng_scr_timer = eng_ptr->scr_timer;

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_CONT_BSR:

        /* CONT_BSR( TIME_HI, TIME_LO ) - Instructs the SS-Client to continue
        ** with its current activity while setting-up a TIME timer. When this
        ** timer expires the SS-Client shall call on the SD_SS_IND_MISC_TIMER
        ** SS-Indication function.
        ** If phone is on the last system on which this timer was started, timer
        ** shall continue, otherwise timer with value specified will be started.
        */
        sdss_eng_scr_con_cont( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      
      case SDSSSCR_CON_CONT2_BSR:

        /* CONT2_BSR( NV_ITEM ) - Instructs the SS-Client to continue with its
        ** current activity while setting-up a TIME second timer. When this
        ** timer expires the SS-Client shall call on the SD_SS_IND_MISC_TIMER
        ** SS-Indication function. Where TIME is the value stored in the
        ** configurable items table at index SDSS_NV_ITEM_"NV_ITEM".
         ** If phone is on the last system on which this timer was started, timer
        ** shall continue, otherwise timer with value specified will bestarted.
        */
        sdss_eng_scr_con_cont( eng_ptr, event, rtrn_iact_ptr );

        break;
        
      case SDSSSCR_CON_ACCEPT:

        /* ACCEPT- Accept BS proposed mode, band and/or CDMA channel/AMPS
        ** system. This SS-Action is only valid for Redirection, Channel
        ** Assignment or Handoff Direction.
        */
        sdss_eng_scr_acpt_rej( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_REJECT:

        /* REJECT- Reject BS proposed mode, band and/or CDMA channel/AMPS
        ** system. This SS-Action is only valid for Redirection, Channel
        ** Assignment or Handoff Direction.
        */
        sdss_eng_scr_acpt_rej( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_PWR_SAVE:

        /* PWR_SAVE - Instruct the SS-Client to enter Power-Save mode of
        ** operation. Once entering Power-Save mode of operation, the
        ** SS-Client should call on the SD_SS_IND_MISC_PWR_SAVE_ENTER
        ** SS-Indication function.
        */
        sdss_eng_scr_con_pwr_save( eng_ptr, event, rtrn_iact_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_REDIR_OFF:

        /* REDIR_OFF - Set the redirection indication to disabled.
        */
        sdss_redir_update(
            ss,
            SD_SS_REDIR_NONE,             /* Set redirection to none */
            FALSE,                        /* Set valid redirection to FALSE*/
            SD_WILDCARD_SID,              /* Set expected SID to wildcard */
            SD_WILDCARD_NID,              /* Set expected NID to wildcard */
            FALSE,                        /* Set RETURN_IF_FAIL to FALSE */
            FALSE,                        /* Set IGNORE_CDMA to FALSE */
            SD_SS_MAX_REDIR_DELAY_NONE ); /* Set max redirect delay to none*/

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_HO_OFF:

        /* HO_OFF - Set the handoff indication to disabled.
        */
        sdss_ho_update( SD_SS_HO_NONE );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_RESET_GW_ACQ_SCAN_STATUS:

        /* RESET_GW_ACQ_SCAN_STATUS - Set the power up acqusition to OFF.
        */
        ss_ptr->acq_gw_scan_status = SD_SS_GW_SCAN_STATUS_NONE;

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_FAIL:

        /* Indicate that acquisition over all systems has failed. */
        sdss_eng_scr_con_acq_fail (ss);
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_STACK_SYNC_UP:

        /* Indicate that service is lost. */
        sd_si_srv_lost_cb( ss );
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACC_RESET:

        /* ACC_RESET - Reset the registration access failure count and start
        ** time.
        */
        sdss_acc_reset( ss_ptr );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_SRV_STATUS:

        /* SRV_STATUS( VAL ) - Set the Service-Indication to VAL.
        */
        switch ( (sys_srv_status_e_type)eng_ptr->con_prm1 )
        {
          case SYS_SRV_STATUS_NO_SRV:
            sd_si_srv_off( ss );
            break;

          case SYS_SRV_STATUS_PWR_SAVE:
            sd_si_srv_pwr_save( ss );
            break;

          case SYS_SRV_STATUS_SRV:
          case SYS_SRV_STATUS_LIMITED:
          case SYS_SRV_STATUS_NONE:
          case SYS_SRV_STATUS_LIMITED_REGIONAL:
          case SYS_SRV_STATUS_MAX:
          default:
            SD_SWITCH_ERR("Incorrect VAL", 0, 0, 0 );
            break;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_CHK_SET_SRV_STATUS:

        if( SD_IS_BIT_CONTAIN( BM(ss_ptr->srv_status), eng_ptr->con_prm1 ) )
        {
          switch ( (sys_srv_status_e_type)eng_ptr->con_prm2 )
          {
            case SYS_SRV_STATUS_NO_SRV:
              sd_si_srv_off( ss );
              break;

            case SYS_SRV_STATUS_PWR_SAVE:
              sd_si_srv_pwr_save( ss );
              break;

            case SYS_SRV_STATUS_SRV:
            case SYS_SRV_STATUS_LIMITED:
            case SYS_SRV_STATUS_NONE:
            case SYS_SRV_STATUS_LIMITED_REGIONAL:
            case SYS_SRV_STATUS_MAX:
            default:
              SD_SWITCH_ERR("Incorrect VAL", 0, 0, 0 );
              break;

          }/* switch */
        }/* if( SD_IS_BIT_CONTAIN ... */

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_SRV_OFF_UNTIL:

        /* SRV_OFF_UNTIL( CRIT ) - Set the Service Indication to OFF. Do not
        ** update the service indicators before the specified criteria is
        ** met.
        */
        sd_si_srv_off_until( ss, (sd_si_update_e_type) eng_ptr->con_prm1 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_SRV_OFF_IF_0:

        /* SRV_OFF_IF_0( LIST ) - Set the Service Indication to OFF iff LIST
        ** contains 0 elements
        */
        if( sdsr_list_cnt(ENG_LIST(eng_ptr->con_prm1)) == 0 )
        {
          sd_si_srv_off( ss );
        }

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_RESEL_STT:

        /* RESEL_STT( STATE ) - Set the Reselection State to STATE.
        */
        SD_MSG_HIGH("Resel STT %d", eng_ptr->con_prm1, 0, 0 );
        sdss_resel_state_update( ss_ptr,
                                 (sdss_resel_stt_e_type) eng_ptr->con_prm1 );
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_RESEL_MODE:

        /* RESEL_MODE( MODE ) - Set the Reselection Mode to MODE.
        */
        sdss_resel_mode_update( ss_ptr,
                                (sdss_resel_mode_e_type) eng_ptr->con_prm1 );
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      case SDSSSCR_CON_SET_FLAG:

        /* SET_FLAG( FLAG, SET_VAL ) - Set the FLAG to SET_VAL.
        */
        sdss_set_flag( ss_ptr,
                       (sdss_flag_e_type) eng_ptr->con_prm1,
                       (boolean) eng_ptr->con_prm2 );
        break;

      case SDSSSCR_CON_SRV_REQ:
        /* SRV_REQ_TYPE( VAL ) - Set the srv_req_type parameter 
                                 in ss_ptr to VAL.
        */
        sdss_set_srv_req_type( ss_ptr,
                               (sd_ss_srv_req_type_e_type) eng_ptr->con_prm1);
        break;

      case SDSSSCR_CON_SET_GW_PWR_SAVE_ACQ_STAGE:
        /* SET_GW_PWR_SAVE_ACQ_STAGE(STAGE) Set stage value
        */
        sdss_set_gw_pwr_save_acq_stage( ss, (byte) eng_ptr->con_prm1);
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_BSR_TIMER_RESET:
      /* Reset BSR timer */
      ss_ptr->bsr_timer = 0;
      break;
      
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      case SDSSSCR_CON_IS_GW_SLEEP_TIME_SET:
      case SDSSSCR_CON_IS_GW_AWAKE_TIME_SET:
      case SDSSSCR_CON_1ST_COND:
      case SDSSSCR_CON_IS:
      case SDSSSCR_CON_IS_NOT:
      case SDSSSCR_CON_IS_EVENT:
      case SDSSSCR_CON_IS_OPR_MODE:
      case SDSSSCR_CON_IS_FLAG_SET:
      case SDSSSCR_CON_IS_PREF_REAS:
      case SDSSSCR_CON_IS_ORIG_MODE:
      case SDSSSCR_CON_IS_STATE:
      case SDSSSCR_CON_IS_ACQ_STATE:
      case SDSSSCR_CON_IS_ACQ_TYPE:
      case SDSSSCR_CON_IS_REDIR:
      case SDSSSCR_CON_IS_HANDOFF:
      case SDSSSCR_CON_IS_RESEL_STT:
      case SDSSSCR_CON_IS_RESEL_MODE:
      case SDSSSCR_CON_IS_CDMA_AVAIL:
      case SDSSSCR_CON_IS_SRV_STATUS:
      case SDSSSCR_CON_IS_SRV_DOMAIN:
      case SDSSSCR_CON_IS_ACC_REAS:
      case SDSSSCR_CON_IS_ACC_TERM:
      case SDSSSCR_CON_IS_ACC_FAIL:
      case SDSSSCR_CON_IS_EVENT_CNT:
      case SDSSSCR_CON_IS_SID:
      case SDSSSCR_CON_IS_MODE:
      case SDSSSCR_CON_IS_BAND:
      case SDSSSCR_CON_IS_PRL:
      case SDSSSCR_CON_IS_NET_SEL:
      case SDSSSCR_CON_IS_SCHED:
      case SDSSSCR_CON_IS_PREF_CONF:
      case SDSSSCR_CON_IS_PREF_CHGD:
      case SDSSSCR_CON_IS_BAND_PREF_CHGD:
      case SDSSSCR_CON_IS_SAME:
      case SDSSSCR_CON_IS_DIFF:
      case SDSSSCR_CON_IS_CONTAIN:
      case SDSSSCR_CON_IS_CNT:
      case SDSSSCR_CON_IS_FADE:
      case SDSSSCR_CON_MANIPULATOR:
      case SDSSSCR_CON_OPERATOR:
      case SDSSSCR_CON_CTRL:
      case SDSSSCR_CON_MISC:
      case SDSSSCR_CON_COND:
      case SDSSSCR_CON_NOT:
      case SDSSSCR_CON_IS_TIME:
      case SDSSSCR_CON_IS_SYS_LOST:
      case SDSSSCR_CON_IS_ACQ_STATE2:
      case SDSSSCR_CON_IS_SRV_STATUS2:
      case SDSSSCR_CON_IS_PREF_UPDATE:
      case SDSSSCR_CON_IS_GW_SCAN_STATUS:
      case SDSSSCR_CON_IS_MODE_PREF:
      case SDSSSCR_CON_IS_SIM_STATE:
      case SDSSSCR_CON_MAX:
      default:
        SD_ERR_FATAL( "cons",0,0,0 );
        break;

    } /* switch */

  } while( eng_ptr->scr_ctrl.ctrl == SDSS_SCR_CTRL_NEXT_CONS );

} /* sdss_eng_event_proc */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_scr_cond_is_true

DESCRIPTION
  Check whether the condition part of the SS-Script that is currently
  associated with the script engine evaluates to TRUE.

DEPENDENCIES
  The SS-Script-engine that is pointed to by eng_ptr must have already been
  initialized with an SS-Script via a call to sdss_eng_init().

RETURN VALUE
  TRUE if the condition part of the SS-Script that is currently associated
  with the script engine evaluates to TRUE. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean               sdss_eng_scr_cond_is_true(

        sdss_eng_s_type       *eng_ptr,
            /* Pointer to the script engine that is to process the
            ** SS-Event */

        sdss_evt_e_type       event
            /* SS-Event to process */
)
{

  sdss_s_type             *ss_ptr;
  boolean                 cond_is_true    = TRUE;
  boolean                 is_negate_cond  = FALSE;
  sd_ss_e_type            ss;
  sdss_eng_stack_s_type   *eng_stack_ptr  = &sdss_eng_stack;
  sdss_return_status_e_type return_status = SDSS_RETURN_STATUS_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr         = eng_ptr->ss_ptr;
  ss             = eng_ptr->ss;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined (FEATURE_TECH_IND_SD_STACK_ARCH) || defined(FEATURE_HDR_HYBRID))
  /* Work with HDR system selection, if specified so.
  */
  if( ss_ptr == sdss_ptr(SD_SS_HYBR_1) )
  {
    eng_stack_ptr   = &sdss_hybr_1_eng_stack;
  }
  #endif /* FEATURE_HDR_HYBRID */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* While condition constructs evaluate to TRUE, parse the next script
  ** construct.
  */
  while( cond_is_true )
  {
    /* Parse the next script construct .
    */
    eng_ptr->scr_con = sdssscr_parser_next( &eng_ptr->parser,
                                            clk_uptime(),
                                            ss_ptr->scr_ctrl_cnt,
                                            &eng_ptr->con_prm1,
                                            &eng_ptr->con_prm2,
                                            &eng_ptr->con_prm3,
                                            &eng_ptr->con_prm4,
                                            &eng_ptr->con_prm5,
                                            &eng_ptr->con_prm6 );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check whether the condition construct evaluates to TRUE.
    */
    cond_is_true   = FALSE;
    return_status = SDSS_RETURN_STATUS_NONE;

    switch( eng_ptr->scr_con ) /*lint -e788 */
    {

      case SDSSSCR_CON_NOT:
        cond_is_true    = TRUE;
        is_negate_cond  = TRUE;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS:

        /* IS( VAL ) - Evaluates to TRUE iff VAL is equal to a TRUE boolean
        ** condition.
        */
        if( sdss_eng_scr_con_is(eng_ptr) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_NOT:

        /* IS_NOT( VAL ) - Evaluates to TRUE iff VAL is equal to a FALSE
        ** boolean condition.
        */
        if( ! sdss_eng_scr_con_is(eng_ptr) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case SDSSSCR_CON_IS_GW_SLEEP_TIME_SET:

        if (ss_ptr->rem_uoos_sleep_time > 0)
        {
          cond_is_true = TRUE;
        }
        break;

      case SDSSSCR_CON_IS_GW_AWAKE_TIME_SET:

        if (ss_ptr->rem_uoos_awake_time > 0)
        {
         cond_is_true = TRUE;
        }
        break;

      case SDSSSCR_CON_IS_EVENT:

        /* IS_EVENT( VAL ) - Evaluates to TRUE iff VAL is equal the current
        ** SS-Event.
        */
        if( event == (sdss_evt_e_type) eng_ptr->con_prm1 )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_OPR_MODE:

        /* IS_OPR_MODE( VAL ) - Evaluates to TRUE iff VAL includes the
        ** current operating mode.
        */
        if( sdss_is_opr_mode( (sdssscr_opr_mode_val_e_type)
                              eng_ptr->con_prm1) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_PREF_REAS:

        /* IS_PREF_REAS( VAL ) - Evaluates to TRUE iff VAL equals the
        ** current SS-Preference reason.
        */
        if( sdss_is_pref_reason(
                            ss,
                            (sdssscr_pref_reas_val_e_type) eng_ptr->con_prm1) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS_FLAG_SET:

      /* IS_FLAG_SET( VAL ) - Evaluates to TRUE iff VAL equals the
      ** current SS-Preference reason.
      */
      if( sdss_is_flag_set(
                           ss,
                           (sdss_flag_e_type) eng_ptr->con_prm1) )
      {
        cond_is_true = TRUE;
      }
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_ORIG_MODE:

        /* IS_ORIG_MODE( VAL ) - Evaluates to TRUE iff VAL includes the
        ** current origination mode.
        */
        if( sdss_is_orig_mode(
                            ss,
                            (sd_ss_orig_mode_val_e_type) eng_ptr->con_prm1) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_STATE:

        /* IS_STATE( VAL ) - Evaluates to TRUE iff VAL equal the current
        ** SS-State.
        */
        if( ss_ptr->ss_state == (sdss_state_e_type) eng_ptr->con_prm1 )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_ACQ_STATE:

        /* IS_ACQ_STATE( VAL ) - Evaluates to TRUE iff VAL equal the current
        ** acquisition state.
        */
        if( sdss_eng_stack_is_state(eng_stack_ptr,
                                    (sdss_acq_stt_e_type) eng_ptr->con_prm1))
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_ACQ_STATE2:

        /* IS_ACQ_STATE( SS, VAL ) - Evaluates to TRUE iff VAL equal the current
        ** acquisition state.
        */
        return_status = sdss_eng_stack_is_state2(
                                   (sd_ss_e_type)eng_ptr->con_prm1,
                                   (sdss_acq_stt_e_type) eng_ptr->con_prm2 );

        switch( return_status )
        {
          case SDSS_RETURN_STATUS_TRUE:

            cond_is_true = TRUE;
            break;

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case SDSS_RETURN_STATUS_INVALID:
          case SDSS_RETURN_STATUS_FALSE:
          case SDSS_RETURN_STATUS_NONE:
          case SDSS_RETURN_STATUS_MAX:
          default:
            cond_is_true = FALSE;
            break;
            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_ACQ_TYPE:

        /* IS_ACQ_TYPE( VAL ) - Evaluates to TRUE iff VAL equal the type of
        ** the most recent acquisition attempt.
        */
        if( sdss_eng_stack_is_type(eng_stack_ptr,
                                   (sdssscr_acq_e_type) eng_ptr->con_prm1) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_GW_SCAN_STATUS:

        /* IS_GW_SCAN_STATUS( VAL ) - Evaluates to TRUE iff the GW scan
        ** status is set to VAL.
        */
        if( ss_ptr->acq_gw_scan_status ==
                            (sd_ss_gw_scan_status_e_type)eng_ptr->con_prm1 )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_REDIR:

        /* IS_REDIR( VAL ) - Evaluates to TRUE iff VAL encompasses the
        ** current/proposed redirection type.
        */
        if( sdss_is_redir( ss, (sdss_redir_val_e_type) eng_ptr->con_prm1) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_HANDOFF:

        /* IS_HANDOFF( VAL ) - Evaluates to TRUE iff VAL encompasses the
        ** current/proposed handoff.
        */
        if( sdss_is_handoff((sdss_ho_val_e_type) eng_ptr->con_prm1) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_RESEL_STT:

        /* IS_RESEL_STT( VAL ) - Evaluates to TRUE iff VAL equals the current
        ** reselection state.
        */
        if( sdss_is_resel_state1(ss,
                                (sdssscr_resel_stt_val_e_type)  eng_ptr->con_prm1) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_RESEL_MODE:

        /* IS_RESEL_MODE( VAL ) - Evaluates to TRUE iff VAL equals the
        ** current reselection mode OR
        ** if VAL == RESEL_MODE_FULL_SRV, then to TRUE if current reselection mode
        ** is between SDSS_RESEL_MODE_FULL_SRV & SDSS_RESEL_MODE_FULL_SRV_MAX-1.
        */
        if ( ss_ptr->resel_mode == (sdss_resel_mode_e_type)eng_ptr->con_prm1 ||
            ((sdss_resel_mode_e_type)eng_ptr->con_prm1 ==
                                             SDSS_RESEL_MODE_FULL_SRV &&
              ss_ptr->resel_mode >= SDSS_RESEL_MODE_FULL_SRV &&
              ss_ptr->resel_mode < SDSS_RESEL_MODE_FULL_SRV_MAX
             )
             #if defined( FEATURE_SD_ALT_CDMA_BSR_SCAN_1 )
#error code not present
             #endif
          )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_CDMA_AVAIL:

        /* IS_CDMA_AVAIL( CNT ) - Evaluates to TRUE iff CDMA-available
        ** counter < CNT.
        */
        if( ss_ptr->cdma_avail_cnt < (int) eng_ptr->con_prm1 )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_SRV_STATUS:

        /* IS_SRV_STATUS( VAL ) - Evaluates to TRUE iff VAL equals the
        ** current srv status.
        */
        if( SD_IS_BIT_CONTAIN( BM(ss_ptr->srv_status), eng_ptr->con_prm1 ))
        {
          cond_is_true = TRUE;
        }

        break;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_SRV_STATUS2:

        /* IS_SRV_STATUS2( SS, VAL ) - Evaluates to TRUE iff VAL equals the
        ** current srv status.
        */
        if( sdss_is_srv_status2( (sd_ss_e_type)eng_ptr->con_prm1,
                        (sdssscr_srv_status_val_e_type) eng_ptr->con_prm2 ) )
        {
          cond_is_true = TRUE;
        }
        break;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_SRV_DOMAIN:

        /* IS_SRV_DOMAIN( VAL ) - Evaluates to TRUE iff VAL equals the
        ** current srv domain.
        */
        if( SD_IS_BIT_CONTAIN( BM(ss_ptr->srv_domain), eng_ptr->con_prm1 ))
        {
          cond_is_true = TRUE;
        }

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_ACC_REAS:

        /* IS_ACC_REAS( VAL ) - Evaluates to TRUE iff VAL equals the latest
        ** access attempt reason.
        */
        if( sdss_is_acc_reas(ss_ptr,
                             (sd_ss_acc_reas_e_type) eng_ptr->con_prm1) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_ACC_TERM:

        /* IS_ACC_TERM( VAL ) - Evaluates to TRUE iff VAL encompasses the
        ** latest access attempt termination reason.
        */
        if( sdss_is_acc_term(ss_ptr,
                             (sd_ss_acc_term_e_type) eng_ptr->con_prm1) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_ACC_FAIL:

        /* IS_ACC_FAIL( CNT, TIME ) - Evaluates to TRUE iff registration
        ** access has failed at least CNT times and TIME minutes.
        */
        if( sdss_is_acc_fail(ss_ptr,
                             (int) eng_ptr->con_prm1,
                             (word) eng_ptr->con_prm2) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_EVENT_CNT:

        /* IS_EVENT_CNT ( EVENT, CNT, TIME ) - Evaluates to TRUE iff EVENT
        ** has occurred CNT times within TIME minutes.
        */
        if( sdss_is_event_cnt(ss_ptr,
                             (sdss_cnted_evt_e_type) eng_ptr->con_prm1,
                             (int)                   eng_ptr->con_prm2,
                             (word)                  eng_ptr->con_prm3) )
        {
          cond_is_true = TRUE;
        }
        break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_SID:

        /* IS_SID( LST, VAL ) - Evaluates to TRUE iff VAL describes the
        ** SID/NID to expected SID/NID relation of at least one of the
        ** systems that are referenced by the system record list LST.
        */
        if( sdss_sr_list_is_sid(
                              ENG_LIST(eng_ptr->con_prm1),
                              (sdssscr_sid_cond_e_type) eng_ptr->con_prm2) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_MODE:

        /* IS_MODE( LST, VAL ) - Evaluates to TRUE iff VAL includes the mode
        ** designation of at least one of the systems that are referenced by
        ** the system record list LST.
        */
        if( sdss_sr_list_is_mode(ENG_LIST(eng_ptr->con_prm1),
                                 (sd_mode_e_type) eng_ptr->con_prm2) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_MODE_PREF:

        /* IS_MODE( LST, VAL ) - Evaluates to TRUE iff VAL includes the mode
        ** designation of at least one of the systems that are referenced by
        ** the system record list LST.
        */
        if( sdss_sr_list_is_mode_pref(ENG_LIST(eng_ptr->con_prm1),
                                (sd_ss_mode_pref_e_type) eng_ptr->con_prm2) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      case SDSSSCR_CON_IS_BAND:

        /* IS_BAND( LST, VAL ) - Evaluates to TRUE iff VAL includes the band
        ** designation of at least one of the systems that are referenced by
        ** the system record list LST.
        */
        if( sdss_sr_list_is_band(ENG_LIST(eng_ptr->con_prm1),
                                 (sd_band_e_type) eng_ptr->con_prm2) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_PRL:

        /* IS_PRL( LST, VAL ) - Evaluates to TRUE iff VAL includes the PRL
        ** designation of at least one of the systems that are referenced by
        ** the system record list LST.
        */
        if( sdss_sr_list_is_prl(ENG_LIST(eng_ptr->con_prm1),
                                (sdss_prl_val_e_type) eng_ptr->con_prm2) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_NET_SEL:

        /* IS_NET_SEL( LST, VAL ) - Evaluates to TRUE iff VAL equals the
        ** network selection mode of at least one of the systems that are
        ** referenced by the system record list LST.
        */
        if( sdss_sr_list_is_net_sel( ENG_LIST(eng_ptr->con_prm1),
                             (sd_net_select_mode_e_type) eng_ptr->con_prm2) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_SCHED:

        /* IS_SCHED( LST, VAL ) - Evaluates to TRUE iff VAL equals the
        ** reacquisition schedule of at least one of the systems that are
        ** referenced by the system record list LST
        */
        if( sdss_sr_list_is_sched(ENG_LIST(SDSR_SCHED_LST),
                                  ENG_LIST(eng_ptr->con_prm1),
                                  (sdssscr_sched_e_type) eng_ptr->con_prm2) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_PREF_CONF:

        /* IS_PREF_CONF( LST, VAL ) - Evaluates to TRUE iff VAL includes the
        ** SS-Preference conflict of at least one of the systems that are
        ** referenced by the system record list LST.
        */
        if( sdss_sr_list_is_pref_conf(
                              ss,
                              ENG_LIST(eng_ptr->con_prm1),
                              (sdss_pref_conf_e_type) eng_ptr->con_prm2) )
        {
          cond_is_true = TRUE;
        }
        break;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_PREF_CHGD:

        /* IS_PREF_CHGD(  VAL ) - Evaluates to TRUE iff specified SS
        ** preference given by VAL was changed with the last preferred
        ** system changed.
        */
        if( sdss_is_pref_chgd( ss,
                               (sdss_pref_chgd_e_type) eng_ptr->con_prm1
                             ))
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_BAND_PREF_CHGD:

        /* IS_BAND_PREF_CHGD(  VAL ) - Evaluates to TRUE iff band
        ** for a given  VAL (mode)  was changed with the last preferred
        ** system changed.
        */
        if( sdss_is_band_pref_chgd( ss,
                               (sd_ss_mode_pref_e_type) eng_ptr->con_prm1
                             ))
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_PREF_UPDATE:

        /* IS_PREF_UPDATE(  VAL ) - Evaluates to TRUE iff specified SS
        ** preference given by VAL was the update status with the last preferred
        ** system changed.
        */
        if( sdss_is_pref_update( ss,
                               (sdss_pref_updated_e_type) eng_ptr->con_prm1
                             ))
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_SAME:

        /* IS_SAME( LST1, LST2, VAL ) - Evaluates to TRUE iff LST1 and LST2
        ** are the same with respect to the VAL equality category.
        */
        if( sdsr_list_is_same(ENG_LIST(eng_ptr->con_prm1),
                              ENG_LIST(eng_ptr->con_prm2),
                              (sdsr_equal_e_type) eng_ptr->con_prm3) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_DIFF:

        /* IS_DIFF( LST1, LST2, VAL ) - Evaluates to TRUE iff LST1 and LST2
        ** are different with respect to the VAL equality category
        */
        if( ! sdsr_list_is_same(ENG_LIST(eng_ptr->con_prm1),
                                ENG_LIST(eng_ptr->con_prm2),
                                (sdsr_equal_e_type) eng_ptr->con_prm3) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_CONTAIN:

        /* IS_CONTAIN( LST1, VAL, LST2 ) - Evaluates to TRUE iff LST1
        ** contains VAL elements of LST2.
        */
        if( sdsr_list_is_contain(ENG_LIST(eng_ptr->con_prm1),
                                 (sdsr_contain_e_type) eng_ptr->con_prm2,
                                 ENG_LIST(eng_ptr->con_prm3)) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_CNT:

        /* IS_CNT( LST, CNT, CNT_COND, CNT_INC ) - Evaluates to TRUE iff LST
        ** contains CNT or more/less/exactly elements that comply with the
        ** CNT_INC count inclusion criteria.
        */
        if( sdss_sr_list_is_cnt(eng_ptr->ss,
                                ENG_LIST(eng_ptr->con_prm1),
                                (int) eng_ptr->con_prm2,
                                (sdssscr_cnt_cond_e_type) eng_ptr->con_prm3,
                                (sdssscr_cnt_inc_e_type) eng_ptr->con_prm4) )
        {
          cond_is_true = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_FADE:

      /* IS_FADE( CNT ) - Evaluates to TRUE iff there is fades for
      ** at least CNT times.
      */
      if( sdsr_list_is_fade( ENG_LIST(eng_ptr->con_prm1),
                             (byte)eng_ptr->con_prm2 ))
      {
        cond_is_true = TRUE;
      }
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_TIME:

      /* IS_TIME_GR(INSTANCE, SRV_VAL, TIME) -
      ** Evaluates to TRUE iff INSTANCE has SRV_VAL for atleast TIME seconds
      */
      if( sdss_is_time_gr( (sd_ss_e_type) eng_ptr->con_prm1,
                           (sdssscr_srv_status_val_e_type)eng_ptr->con_prm2,
                           (byte)eng_ptr->con_prm3 ) )
      {
        cond_is_true = TRUE;
      }
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_SYS_LOST:

        /* IS_SYS_LOST( VAL )- Evaluates to TRUE iff
        ** VALUE equals the latest system lost reason. */
        if( sdss_is_sys_lost( ss_ptr,
                              (sd_ss_sys_lost_e_type) eng_ptr->con_prm1) )
        {
          cond_is_true = TRUE;
        }
        break;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IS_SIM_STATE:

        /* IS_SIM_STATE( SIM_STATUS ) - Evaluates to TRUE iff the
        ** phone's SIM state is equal to the given sim_status */

        if ( ss_ptr->sim_state == (sys_sim_state_e_type) eng_ptr->con_prm1)
        {
          cond_is_true = TRUE;  
        }
        else
        {
          cond_is_true = FALSE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_1ST_COND:
      case SDSSSCR_CON_MAX:
        SD_CASE_ERR( "construct",0,0,0 );
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      default:

        /* If this is a condition construct, it must be catched above.
        */
        if( SDSSSCR_CON_IS_COND(eng_ptr->scr_con) )
        {
          SD_CASE_ERR( "Cond construct not implemented",0,0,0 );
          break;
        }

        /* We reached a non-condition construct (i.e., the condition part of
        ** the script is evaluated to TRUE) - return TRUE.
        */
        return TRUE;

    } /* switch */ /*lint +e788 */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If condition needs to be negated, do so.
    */
    if( is_negate_cond &&
        eng_ptr->scr_con != SDSSSCR_CON_NOT )
    {
      is_negate_cond  = FALSE;
      cond_is_true    = BOOLEAN( !cond_is_true );
    }

    if( return_status == SDSS_RETURN_STATUS_INVALID )
    {
      cond_is_true   = FALSE;
      return_status = SDSS_RETURN_STATUS_INVALID;
    }

  } /* while( cond_is_true ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the condition is evaluated to FALSE - return FALSE.
  */
  return FALSE;

} /* sdss_eng_scr_cond_is_true */




/* <EJECT> */
/*===========================================================================
=============================================================================
========================= SS-SCRIPT-ENGINES STACK ===========================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdss_eng_stack_init

DESCRIPTION
  Initializes the SS-Script-engine stack.

  NOTE! this function must be called before any other sdss_eng_stack_xxx is
  called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_stack_init(

        sdss_eng_stack_s_type   *stack_ptr,
            /* Engine stack pointer.
            */

        sdss_eng_s_type         *arr_ptr,
            /* Pointer to array of engines.
            */

        unsigned int            arr_size
            /* Size of array (in elements).
            */
)
{
  SD_ASSERT( stack_ptr != NULL );
  SD_ASSERT( arr_ptr != NULL );
  SD_ASSERT( arr_size != 0 );
  SD_FUNC_START( "sdss_eng_stack_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

//  STACK_INIT( sdss_eng_stack );

  stack_ptr->arr_ptr  = arr_ptr;
  stack_ptr->size     = arr_size;
  stack_ptr->cnt      = 0;

} /* sdss_eng_stack_init */

#ifdef FEATURE_SD_PREVENT_SCRIPT_MULTI_INST_EXEC
/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_stack_contains_script

DESCRIPTION
  Checks if a script is present in the engine stack

DEPENDENCIES
  sdss_eng_stack_init() must have already been called to initialize the
  script engine stack.

RETURN VALUE
  Position of first occurance, if engine stack contains the script.
  SDSS_STACK_POS_NULL, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  unsigned int            sdss_eng_stack_contains_script(

        const sdss_eng_stack_s_type   *stack_ptr,
            /* Engine stack pointer.
            */
        const sdssscr_ref_type        *scr_ref
            /* Script pointer
            */
)
{
  unsigned int i;

  for( i=0; i < stack_ptr->cnt; i++ )
  {
    /* Check if the script is already in the engine stack. If yes, exit
    ** without pushing the script.
    */
    if( stack_ptr->arr_ptr[i].parser.scr_ref == scr_ref )
    {
      return i;
    }
  }

  return SDSS_STACK_POS_NULL;
} /* sdss_eng_stack_contains_script */
#endif /* FEATURE_SD_PREVENT_SCRIPT_MULTI_INST_EXEC */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_stack_push

DESCRIPTION
  Push a script engine on top of the engines stack.

  Note! If the script that is to be pushed on the engines stack contain no
  PREV_SCR construct, the stack is emptied prior to pushing the script
  engine.

DEPENDENCIES
  sdss_eng_stack_init() must have already been called to initialize the
  script engine stack.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_stack_push(

        sdss_eng_stack_s_type   *stack_ptr,
            /* Engine stack pointer.
            */
        const sdss_eng_s_type   *eng_ptr
            /* Pointer to the script engine that is to be pushed on to the
            ** engines stack.
            */
)
{
  SD_ASSERT( stack_ptr != NULL );
  SD_ASSERT( eng_ptr != NULL );
  SD_ASSERT( stack_ptr->cnt < stack_ptr->size );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the script that is to be pushed on the engines stack contain no
  ** PREV_SCR construct, empty the engine stack.
  */
  if( ! sdssscr_parser_is_contain( &eng_ptr->parser, SDSSSCR_CON_PREV_SCR,
                                   TRUE, NULL, NULL ))
  {
    stack_ptr->cnt = 0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Push engine on top of engines stack.
  */
  if( stack_ptr->cnt < stack_ptr->size )
  {
    stack_ptr->arr_ptr[stack_ptr->cnt] = *eng_ptr;
    stack_ptr->cnt++;
  }

} /* sdss_eng_stack_push */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_stack_pop

DESCRIPTION
  Pop a script engine from top of the engines stack.

DEPENDENCIES
  sdss_eng_stack_init() must have already been called to initialize the
  script engine stack.

RETURN VALUE
  TRUE if pop is successful. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                 sdss_eng_stack_pop(

        sdss_eng_stack_s_type   *stack_ptr
            /* Engine stack pointer.
            */
)
{
  SD_ASSERT( stack_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( stack_ptr->cnt > 0 )
  {
    stack_ptr->cnt--;
    return TRUE;
  }

  return FALSE;

} /* sdss_eng_stack_pop */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_stack_top

DESCRIPTION
  Return a pointer to script engine that is at the top of the engines stack.

DEPENDENCIES
  sdss_eng_stack_init() must have already been called to initialize the
  script engine stack.

RETURN VALUE
  Pinter to the script engine that is at the top of the engines stack if
  stack is not empty. NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdss_eng_s_type*              sdss_eng_stack_top(

        const sdss_eng_stack_s_type   *stack_ptr
            /* Engine stack pointer.
            */
)
{
  SD_ASSERT( stack_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( stack_ptr->cnt > 0 )
  {
    return &stack_ptr->arr_ptr[stack_ptr->cnt-1];
  }

  return NULL;

} /* sdss_eng_stack_top */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_stack_is_state

DESCRIPTION
  Check the acquisition state of script engine that is at the top of the
  stack.

DEPENDENCIES
  sdss_eng_stack_init() must have already been called to initialize the
  script engine stack.

RETURN VALUE
  TRUE if the acquisition state of the script engine that is at the top of
  the stack is equal to the acquisition state in question. FALSE otherwise.
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_eng_stack_is_state(

        const sdss_eng_stack_s_type   *stack_ptr,
            /* Engine stack pointer.
            */

        sdss_acq_stt_e_type           acq_stt
            /* Acquisition state in question.
            */
)
{
  sdss_eng_s_type   *eng_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( stack_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( acq_stt, SDSS_ACQ_STT_MAX );
  SD_ASSERT( stack_ptr->cnt > 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the script engine that is at the top of the stack. Return TRUE
  ** if the acquisition state of this engine is equal to the acquisition
  ** state in question. Else, return FALSE.
  */
  eng_ptr = sdss_eng_stack_top( stack_ptr );
  SD_ASSERT( eng_ptr != NULL ); //?? delete this assert.
  /*lint -e774 *//* Suppress the if statement is always true */
  if( eng_ptr == NULL ) /*lint +e774 */
  {
    SD_ERR( "Empty stack!",0,0,0 );
  }
  else if( eng_ptr->acq.state == acq_stt )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_eng_stack_is_state */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_stack_is_state2

DESCRIPTION
  Check the acquisition state of script engine that is at the top of the
  stack.

DEPENDENCIES
  sdss_eng_stack_init() must have already been called to initialize the
  script engine stack.

RETURN VALUE
  TRUE if the acquisition state of the script engine that is at the top of
  the stack is equal to the acquisition state in question. FALSE otherwise.
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdss_return_status_e_type    sdss_eng_stack_is_state2(

        sd_ss_e_type                  ss,
            /* The SS whose stack state it to be checked
            */

        sdss_acq_stt_e_type           acq_stt
            /* Acquisition state in question.
            */
)
{

  sdss_eng_stack_s_type   *stack_ptr;
  sdss_eng_s_type         *eng_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acq_stt, SDSS_ACQ_STT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( ss )
  {
    case SD_SS_MAIN:
      stack_ptr = &sdss_eng_stack;
      break;


    case SD_SS_HYBR_HDR:

      #if (defined (FEATURE_TECH_IND_SD_STACK_ARCH) || defined(FEATURE_HDR_HYBRID))
      stack_ptr = &sdss_hybr_1_eng_stack;
      break;
      #endif
      /* Delibrate fall down to the default is FEATURE_HDR is not defined
      */
      /*lint -fallthrough */

    case SD_SS_HYBR_WLAN:

      #ifdef FEATURE_WLAN
#error code not present
      #endif

      /* Delibrate fall down to the default is FEATURE_WLAN is not defined
      */
      /*lint -fallthrough */
    case SD_SS_MAX:
    default:
      SD_MSG_HIGH("Invalid ss %d", ss, 0, 0 );
      return SDSS_RETURN_STATUS_INVALID;
  }

  if( stack_ptr->cnt == 0 )
  {
      return SDSS_RETURN_STATUS_INVALID;
  }

  /* Point at the script engine that is at the top of the stack. Return TRUE
  ** if the acquisition state of this engine is equal to the acquisition
  ** state in question. Else, return FALSE.
  */
  eng_ptr = sdss_eng_stack_top( stack_ptr );
  SD_ASSERT( eng_ptr != NULL ); //?? delete this assert.
  /*lint -e774 *//* Suppress the if statement is always true */
  if( eng_ptr == NULL ) /*lint +e774 */
  {
    SD_ERR( "Empty stack!",0,0,0 );
  }
  else if( eng_ptr->acq.state == acq_stt )
  {
    return SDSS_RETURN_STATUS_TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDSS_RETURN_STATUS_FALSE;

} /* sdss_eng_stack_is_state */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_stack_is_type

DESCRIPTION
  Check the last acquisition type of the script engine that is at the top of
  the stack.

DEPENDENCIES
  sdss_eng_stack_init() must have already been called to initialize the
  script engine stack.

RETURN VALUE
  TRUE if the acquisition type of the script engine that is at the top of
  the stack is equal to the acquisition state in question. FALSE otherwise.
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                       sdss_eng_stack_is_type(

        const sdss_eng_stack_s_type   *stack_ptr,
            /* Engine stack pointer.
            */

        sdssscr_acq_e_type            acq_type
            /* Acquisition type in question.
            */
)
{
  sdss_eng_s_type   *eng_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( stack_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( acq_type, SDSSSCR_ACQ_MAX);
  SD_ASSERT( stack_ptr->cnt > 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the script engine that is at the top of the stack. Return TRUE
  ** if the last acquisition type of this engine is equal to the acquisition
  ** state in question. Else, return FALSE.
  */
  eng_ptr = sdss_eng_stack_top( stack_ptr );
  SD_ASSERT( eng_ptr != NULL ); //?? delete this assert.
  /*lint -e774 *//* Suppress the if statement is always true */
  if( eng_ptr == NULL )/*lint +e774*/
  {
    SD_ERR( "Empty stack!",0,0,0 );
  }
  else if( eng_ptr->ss_ptr->acq_type == acq_type )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdss_eng_stack_is_type */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_stack_get_state

DESCRIPTION
  Return the acquisition state of script engine that is at the top of the
  stack.

DEPENDENCIES
  sdss_eng_stack_init() must have already been called to initialize the
  script engine stack.

RETURN VALUE
  The acquisition state of script engine that is at the top of the stack.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdss_acq_stt_e_type     sdss_eng_stack_get_state(

        sd_ss_e_type            ss
            /* System selection - MAIN or HDR.
            *//*lint -esym( 715, ss ) */ /* Symbol is not referenced.*/
)
{
  sdss_eng_s_type         *eng_ptr;
  sdss_eng_stack_s_type   *stack_ptr  = &sdss_eng_stack;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined (FEATURE_TECH_IND_SD_STACK_ARCH) || defined(FEATURE_HDR_HYBRID))
  /* If the input system selection is HDR, point at HDR engine stack.
  */
  if( ss == SD_SS_HYBR_1 )
  {
    stack_ptr   = &sdss_hybr_1_eng_stack;
  }
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the script engine that is at the top of the stack. Return the
  ** acquisition state of this engine.
  */
  eng_ptr = sdss_eng_stack_top( stack_ptr );

  if( eng_ptr == NULL )
  {
    ERR( "Empty stack!, ss=%d",ss,0,0 );
    return SDSS_ACQ_STT_DONE;
  }

  return ( eng_ptr->acq.state );

} /* sdss_eng_stack_get_state */
/*lint +esym( 715, ss ) */ /* Symbol is not referenced.*/




/* ^L<EJECT> */
/*===========================================================================

FUNCTION sdss_eng_stack_scr_act

DESCRIPTION
  Activate a new script as necessary.

DEPENDENCIES
  sdss_eng_stack_init() must have already been called to initialize the
  script engine stack.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_stack_scr_act(

        sdss_eng_stack_s_type   *stack_ptr,
            /* Engine stack pointer.
            */

        sdss_s_type             *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        sdss_evt_e_type         event
            /* SS-Event to process.
            */
)
{
  int                   i;
  sdss_eng_s_type       eng;
  sdssscr_ref_type      scr_ref;
  sdss_evt_grp_e_type   event_group = sdss_map_event_to_group( event );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( stack_ptr != NULL );
  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the scripts table and check whether any script is
  ** associated with the current SS-Event and conditions. If such a script is
  ** found, activate it by pushing its associated script engine on top of the
  ** engines stack.
  **
  ** Note that a script is considered associated with the current SS-Event
  ** and condition iff the condition part of the script evaluates to TRUE.
  */
  for( i=0;
       (scr_ref=sdssscr_get_ref(event_group, i)) != SDSSSCR_REF_NULL;
       i++ )
  {
    sdss_eng_init( &eng, ss_ptr, scr_ref, event_group, i );
    if( sdss_eng_scr_cond_is_true(&eng, event) )
    {
      switch( event_group )
      {
        case SDSS_EVT_GRP_USER:
          SD_MSG_HIGH( "** Activate user script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_MISC:
          SD_MSG_HIGH( "** Activate misc script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_INT:
          SD_MSG_HIGH( "** Activate internal script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_CDMA_ACQ:
          SD_MSG_HIGH( "** Activate CDMA acq script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_CDMA_OPR:
          SD_MSG_HIGH( "** Activate CDMA opr script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_AMPS_ACQ:
          SD_MSG_HIGH( "** Activate AMPS acq script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_AMPS_OPR:
          SD_MSG_HIGH( "** Activate AMPS opr script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_HDR_USER:
          SD_MSG_HIGH( "** Activate HDR user script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_HDR_ACQ:
          SD_MSG_HIGH( "** Activate HDR acq script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_HDR_OPR:
          SD_MSG_HIGH( "** Activate HDR opr script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_GW_ACQ:
          SD_MSG_HIGH( "** Activate GW acq script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_GW_OPR:
          SD_MSG_HIGH( "** Activate GW opr script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_WLAN_USER:
          SD_MSG_HIGH( "** Activate WLAN user script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_WLAN_ACQ:
          SD_MSG_HIGH( "** Activate WLAN acq script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_WLAN_OPR:
          SD_MSG_HIGH( "** Activate WLAN opr script=%d **",i,0,0 );
          break;


        case SDSS_EVT_GRP_HYBR_1_USER:
          SD_MSG_HIGH( "** Activate HYBR_1 user script=%d **",i,0,0 );
          break;

        case SDSS_EVT_GRP_CALLED:
        case SDSS_EVT_GRP_MAX:
        default:
          SD_CASE_ERR( "event_group",0,0,0 );
          break;

      } /* switch */

      sdss_eng_init( &eng, ss_ptr, scr_ref, event_group, i );
      sdss_eng_stack_push( stack_ptr, &eng );

      #ifdef FEATURE_SD_PREVENT_SCRIPT_MULTI_INST_EXEC
      /* If the script is not an internal event script and the script is
      ** present more than once in the stack, then pop the script and exit.
      */
      if( event_group != SDSS_EVT_GRP_INT &&
          sdss_eng_stack_contains_script(stack_ptr, scr_ref) < (stack_ptr->cnt-1)
        )
      {
        /*lint -esym(550, stat, eng_ptr) */ /* para not accessed */
        boolean         stat;
        sdss_eng_s_type *eng_ptr;

        stat = sdss_eng_stack_pop( stack_ptr );
        SD_ASSERT( stat );

        eng_ptr = sdss_eng_stack_top( stack_ptr );
        SD_ASSERT( eng_ptr != NULL );

        /* If this is not a known corner case, print an error message.
        */
        if( event != SDSS_EVT_CDMA_OPR_CAM )
        {
          #if ( TG==T_PC)
#error code not present
          #else
          SD_ERR(
            "Evt %d tried to push grp=%d, scr=%d onto stack again!",
            event, event_group, i
          );
          #endif
        }
      }
      /*lint +esym(550, stat, eng_ptr) */ /* para not accessed */
      #endif /* FEATURE_SD_PREVENT_SCRIPT_MULTI_INST_EXEC */

      return;
    }
  }

} /* sdss_eng_stack_scr_act */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_stack_event_proc

DESCRIPTION
  Process an SS-Event called for by the SS-Front-end and instruct the
  SS-Front-end of the next SS-Internal-Action to take.

DEPENDENCIES
  sdss_eng_stack_init() must have already been called to initialize the
  script engine stack.

RETURN VALUE
  SS-Internal-Action to be taken by the SS-Front-end (through the
  rtrn_iact_ptr parameter).

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_eng_stack_proc(

        sdss_eng_stack_s_type   *stack_ptr,
            /* Engine stack pointer.
            */

        sdss_s_type             *ss_ptr,
            /* Pointer to a System Selection information structure.
            */

        sdss_evt_e_type         event,
            /* SS-Event to process.
            */

        sdss_iact_s_type        *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{
  boolean             stat      = FALSE;
  sdss_eng_s_type     eng;
  sdss_eng_s_type     *eng_ptr;
  sdssscr_ref_type    scr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( stack_ptr != NULL );
  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(stat);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Activate a new script as necessary.
  */
  sdss_eng_stack_scr_act( stack_ptr, ss_ptr, event );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do script processing as long as control is not to be returned to the
  ** SS-Front-end.
  */ /*lint -e716 */ /* While (1) */
  while( TRUE )
  {
    /* Call on the script-engine that is at the top of the engines stack to
    ** process the event.
    */
    eng_ptr = sdss_eng_stack_top( stack_ptr );
    if( eng_ptr == NULL )
    {
      SD_MSG_ERROR( "NULL eng ptr, CONTINUE", 0, 0, 0 );
      rtrn_iact_ptr->act = SDSS_IACT_CONTINUE;
      return;
    }

    sdss_eng_event_proc( eng_ptr, event, rtrn_iact_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Based on the control information that comes back from the
    ** script-engine take the next step.
    */
    switch( eng_ptr->scr_ctrl.ctrl )
    {
      case SDSS_SCR_CTRL_RTRN_ACT:

        /* Set the return timer as per the active script timer and return
        ** control to the SS-Front-end.
        */
        rtrn_iact_ptr->ss_timer = eng_ptr->scr_timer;
        return;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSS_SCR_CTRL_NEXT_CONS:

        /* Continue with next script construct.
        */
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSS_SCR_CTRL_PREV_SCR:

        /* Reactivate the previous script.
        */
        stat = sdss_eng_stack_pop( stack_ptr );
        SD_ASSERT( stat );

        eng_ptr = sdss_eng_stack_top( stack_ptr );
        if( eng_ptr == NULL )
        {
          SD_MSG_ERROR( "NULL eng ptr, CONTINUE", 0, 0, 0 );
          rtrn_iact_ptr->act = SDSS_IACT_CONTINUE;
          return;
        }


        SD_MSG_HIGH( "** Re-act scr, grp=%d, scr=%d **",
                    eng_ptr->scr_evt_grp, eng_ptr->scr_tbl_idx, 0 );

        /* If the script engine's acquisition state machine is active, update
        ** the acquisition type of the SS-Core per the acquisition type of
        ** the this script engine.
        */
        if( eng_ptr->acq.acq_type != SDSSSCR_ACQ_MAX ) {
          sdss_acq_type_update( eng_ptr->ss_ptr, eng_ptr->acq.acq_type );
        }

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSS_SCR_CTRL_ACT_SCR:

        /* Activate a new script by associating an engine with the script and
        ** pushing the associated engine on to the engines stack.
        */
        SD_MSG_HIGH( "** Activate called script=%d **",
                    eng_ptr->scr_ctrl.prm.act.script,0,0 );

        scr_ref = sdssscr_called_get_ref( eng_ptr->scr_ctrl.prm.act.script );
        SD_ASSERT( scr_ref != NULL );

        sdss_eng_init( &eng,
                       ss_ptr,
                       scr_ref,
                       SDSS_EVT_GRP_CALLED,
                       (int) eng_ptr->scr_ctrl.prm.act.script );

        sdss_eng_stack_push( stack_ptr, &eng );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSS_SCR_CTRL_PROC_EVT:

        /* Do new event processing by activating a new script as needed.
        */
        event = eng_ptr->scr_ctrl.prm.proc.event;
        SD_MSG_HIGH( "** Process event=%d **", event, 0, 0 );

        sdss_eng_stack_scr_act( stack_ptr, ss_ptr, event );
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSS_SCR_CTRL_MAX:
      default:
        SD_CASE_ERR( "scr_ctrl",0,0,0 );
        break;

    } /* switch */

  } /* while */ /*lint +e716 */

} /* sdss_eng_stack_proc */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== MAJOR ROUTINES ===============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdss_init

DESCRIPTION
  Initializes a System Selection information structure.

  NOTE! This function only returns after the system selection related items
  are read from NV.

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdss_init(

        sdss_s_type             *ss_ptr
            /* Pointer to a System Selection information structure.
            */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  int i;

  SD_ASSERT( ss_ptr != NULL );
  SD_FUNC_START( "sdss_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the SS-Core variables.
  */

  /* Initialize the operating mode to power-up.
  */
  ss_ptr->opr_mode        = SDSS_OPR_MODE_PWR_UP;

  /* Initialize the service status / srv domain to no-service.
  */
  ss_ptr->srv_status      = SYS_SRV_STATUS_NO_SRV;
  ss_ptr->srv_domain      = SYS_SRV_DOMAIN_NO_SRV;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the origination mode to none and indicate that last
  ** origination mode change was not done in order to end a call origination
  ** mode.
  */
  ss_ptr->orig_mode       = SD_SS_ORIG_MODE_NONE;
  ss_ptr->orig_mode_prev  = SD_SS_ORIG_MODE_NONE;
  //ss_ptr->orig_is_end     = FALSE;
  ss_ptr->pref_reason     = SD_SS_PREF_REAS_MAX;

  /* Initialize the Mode, band and roam preferences to ANY.
  */
  ss_ptr->mode_pref       = SD_SS_MODE_PREF_ANY;
  ss_ptr->band_pref       = SD_SS_BAND_PREF_ANY;
  ss_ptr->roam_pref       = SD_SS_ROAM_PREF_ANY;
  ss_ptr->hybr_pref       = SD_SS_HYBR_PREF_NONE;
  ss_ptr->domain_pref     = SD_SS_SRV_DOMAIN_PREF_CS_ONLY;
  ss_ptr->acq_order_pref  = SD_SS_ACQ_ORDER_PREF_AUTO;
  ss_ptr->user_mode_pref  = SD_SS_MODE_PREF_ANY;
  ss_ptr->user_true_mode_pref = SD_SS_MODE_PREF_ANY;
  ss_ptr->user_band_pref  = SD_SS_BAND_PREF_ANY;

  /* Initialize the Previous Mode, band and roam preferences
  */
  ss_ptr->prev_mode_pref       = ss_ptr->mode_pref;
  ss_ptr->prev_band_pref       = ss_ptr->band_pref;
  ss_ptr->prev_roam_pref       = ss_ptr->roam_pref;
  ss_ptr->prev_hybr_pref       = ss_ptr->hybr_pref;
  ss_ptr->prev_domain_pref     = ss_ptr->domain_pref;
  ss_ptr->prev_acq_order_pref  = ss_ptr->acq_order_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize PRL Support HDR Indication */
  ss_ptr->is_prl_support_hdr = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the SS-State to acquisition.
  */
  ss_ptr->ss_state        = SDSS_STATE_ACQ;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the acquisition type to allowed.
  */
  ss_ptr->acq_type        = SDSSSCR_ACQ_ALLOWED;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the acquisition start time to 0.
  */
  ss_ptr->acq_start_time  = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the new-acquisition counter to 200.
  */
  ss_ptr->new_acq_cnt     = 200;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize the event counters and their start times.
  */
  for( i=0; i < (int) SDSS_CNTED_EVT_MAX; i++ )
  {
    ss_ptr->event_cnt[i] = 0;
    /* If set to 0, will never get updated. So setting it to "infinity".
    */
    ss_ptr->event_uptime[i] = ~0;
  }

  /* Reset the system SID to 0.
  */
  memset( &ss_ptr->event_sid, 0, sizeof(ss_ptr->event_sid));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the script execution control counter to 0.
  */
  ss_ptr->scr_ctrl_cnt    = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the redirection to none.
  */
  ss_ptr->redir                     = SD_SS_REDIR_NONE;
  ss_ptr->redir_is_valid            = FALSE;
  ss_ptr->redir_expected_sid        = SD_WILDCARD_SID;
  ss_ptr->redir_expected_nid        = SD_WILDCARD_NID;
  ss_ptr->redir_is_rif              = FALSE;
  ss_ptr->redir_is_ignore_cdma      = FALSE;
  ss_ptr->redir_max_redirect_delay  = SD_SS_MAX_REDIR_DELAY_NONE;
  ss_ptr->redir_uptime              = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize handoff to none.
  */
  ss_ptr->handoff         = SD_SS_HO_NONE;

  /* Initialize the access reason and termination.
  */
  ss_ptr->acc_reas        = SD_SS_ACC_REAS_MAX;
  ss_ptr->acc_term        = SD_SS_ACC_TERM_NORMAL;

  /* Initialize the registration access count and start time.
  */
  ss_ptr->acc_fail_cnt        = 0;
  ss_ptr->acc_fail_start_time = ~0;

  /* Initialize the CDMA-available counter;
  */
  ss_ptr->cdma_avail_cnt = 0;

  /* Initialize the HDR session close reason to normal.
  */
  ss_ptr->ses_close       = SD_SS_SES_CLOSE_NORMAL;

  /* Initialize the CDMA locked until power cycle indication to FALSE.
  */
  ss_ptr->is_cmda_locked  = FALSE;

  /* Initialize the sim state to unavailable
  */
  ss_ptr->sim_state       = SYS_SIM_STATE_NOT_AVAILABLE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the AMPS backstop indication.
  */
  ss_ptr->is_srda = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the true event and the event processed fields
  */
  ss_ptr->true_event              = SDSS_EVT_USER;
  ss_ptr->is_true_event_processed = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the reselection state to none.
  */
  ss_ptr->resel_state     = SDSS_RESEL_STT_NONE;

  /* Initialize the reselection mode to wait.
  */
  ss_ptr->resel_mode      = SDSS_RESEL_MODE_INIT;

  /* Initialize the subscription availability status.
  */
  ss_ptr->is_gw_subsc_avail = FALSE;
  ss_ptr->is_1x_subsc_avail = FALSE;

  /* Initialize the throttle data structure.
  */
  sdss_thrttl_acq_init( &ss_ptr->acq_thrttl );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the band capability.
  */
  ss_ptr->band_capability = sdss_get_band_capability();

  /* Populate the mode capability.
  */
  ss_ptr->mode_capability = sdss_get_mode_capability();

  /* Filter out the modes without supported bands. */
  ss_ptr->mode_capability = sdss_get_supp_mode_pref(ss_ptr->mode_capability, ss_ptr->band_capability);

  /* Filter out the bands without the available modes. */
  ss_ptr->band_capability = sdss_get_supp_mode_band_pref(ss_ptr->mode_capability, ss_ptr->band_capability);
  
#ifdef FEATURE_IS683A_450M_SUPPORT 
    //check if the hardware capability is allow 450M and don't support 1900 PCS 
  if (((ss_ptr->band_capability & SD_SS_BAND_PREF_BC5) != 0) && 
   ((ss_ptr->band_capability & SD_SS_BAND_PREF_BC1) == 0 || (ss_ptr->band_capability & SD_SS_BAND_PREF_BC0) == 0))  
  { 
    //enable the flag 
    sdss_set_flag2(SD_SS_MAIN, SDSS_FLAG_IS683A_450M_SUPPORT, TRUE); 
  } 
  else 
  { 
    //disable the flag 
    sdss_set_flag2(SD_SS_MAIN, SDSS_FLAG_IS683A_450M_SUPPORT, FALSE); 
       
  } 
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the get network band/mode preference, list, start position
  ** and list position.
  */
  ss_ptr->get_net_band_pref       = SD_SS_BAND_PREF_ANY;
  ss_ptr->get_net_mode_pref       = SD_SS_MODE_PREF_ANY;
  ss_ptr->get_net_list            = SDSR_MAX_LST;
  ss_ptr->get_net_list_start_pos  = SDSR_POS_NULL;
  ss_ptr->get_net_list_pos        = SDSR_POS_NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the list and position most recently attempted acquisition to
  ** none.
  */
  ss_ptr->acq_list                  = SDSR_MAX;
  ss_ptr->acq_pos                   = SDSR_POS_NULL;
  ss_ptr->last_acq_eng_ptr          = NULL;

  /* Initialize the recently acquired association tag.
  */
  sdss_addn_sys_info_init(&(ss_ptr->prev_sys_info));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the start time of the conditional wait construct.
  */
  for( i=0; i < (int) SDSSSCR_CONT_TYPE_MAX; i++)
  {
    ss_ptr->cont_uptime[i] = 0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the pref changed status to NONE.
  */
  ss_ptr->ss_pref_updated_status = SDSS_PREF_UPDATED_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system lost reason to NONE.
  */
  ss_ptr->sys_lost = SD_SS_SYS_LOST_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the Uniform GW scan parameters
  */
  ss_ptr->acq_gw_scan_status      = SD_SS_GW_SCAN_STATUS_NONE;
  ss_ptr->acq_gw_use_timer        = FALSE;
  ss_ptr->acq_gw_new_acq          = TRUE;
  ss_ptr->acq_gw_total_scan_time  = ~0;
  ss_ptr->acq_gw_wcdma_scan_time  = ~0;
  ss_ptr->acq_gw_gsm_scan_time    = ~0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the service request type
  */
  ss_ptr->srv_req_type            = SD_SS_SRV_REQ_TYPE_NORMAL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the script timers
  */
  ss_ptr->is_eng_scr_timer_set   = FALSE;
  ss_ptr->eng_scr_timer          = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->bsr_timer                   = 0;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif

  /* Initialize wlan scan fail reason */
  ss_ptr->wlan_fail_reason = SD_SS_WLAN_ACQ_FAIL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* sdss_init */


/*===========================================================================

FUNCTION sdss_nv_init

DESCRIPTION
Initializes the NV parameters

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                    sdss_nv_init(

        sd_ss_e_type            ss
            /* The SS to initiaize
            */
)
{

  sdss_s_type *ss_ptr = sdss_ptr( ss );

  /*lint -save -e826 */ /*Suspicious pointer-to-pointer conversion*/
  sdnv_data_s_type          buf;
  nv_item_type              *item_ptr               = (nv_item_type*) &buf;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_FUNC_START( "sdss_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read gprs_anite_gcf from nv.
  */
  (void) sdnv_read(NV_GPRS_ANITE_GCF_I,item_ptr);

   ss_ptr->gprs_anite_gcf          = item_ptr->gprs_anite_gcf;
}


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_component_init

DESCRIPTION
  Initializes the SS-Core component.

  NOTE! This function only returns after the system selection related items
  are read from NV.

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                    sdss_component_init( void )
{


  /* Initializes the System Selection information structutre.
  */
  sdss_init( sdss_ptr(SD_SS_MAIN) );

  /* Initializes the SS-Script-engine stack.
  */
  sdss_eng_stack_init( &sdss_eng_stack,
                       sdss_eng_arr,
                       ARR_SIZE(sdss_eng_arr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined (FEATURE_TECH_IND_SD_STACK_ARCH) || defined(FEATURE_HDR_HYBRID))
  /* If the input system selection is HDR, point at HDR engine stack.
  */
   /* Initializes the HDR System Selection information structutre.
  */
  sdss_init( sdss_ptr(SD_SS_HYBR_1) );

  /* Initializes the HDR SS-Script-engine stack.
  */
  sdss_eng_stack_init( &sdss_hybr_1_eng_stack,
                       sdss_hybr_1_eng_arr,
                       ARR_SIZE(sdss_hybr_1_eng_arr) );
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_WLAN */

} /* sdss_component_init */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_srda_enabled_update

DESCRIPTION
  Update the srda field in SS-Core component.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                    sdss_srda_enabled_update( boolean enabled )
{
  sdss_s_type    *ss_ptr;


  ss_ptr = sdss_ptr( SD_SS_MAIN );
  ss_ptr->is_srda = enabled;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined (FEATURE_TECH_IND_SD_STACK_ARCH) || defined(FEATURE_HDR_HYBRID))
  ss_ptr = sdss_ptr( SD_SS_HYBR_1 );
  ss_ptr->is_srda = enabled;
  #endif

}/* sdss_srda_enabled_update */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_event_allowed

DESCRIPTION
  Check whether SS-Event is allowed in the current mode of operation.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  TRUE if SS-Event is allowed in the current operating mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean               sdss_is_event_allowed(

        sdss_evt_e_type       event,
            /* SS-Event received from SS-Front-end.
            */

        sdss_state_e_type     ss_state,
            /* Current SS-State.
            */

        sdss_opr_mode_e_type  opr_mode
            /* Current operating mode.
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( ss_state, SDSS_STATE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( opr_mode, SDSS_OPR_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Decide whether SS-Event is allowed in the current operating mode.
  */
  switch( opr_mode )
  {
    case SDSS_OPR_MODE_PWR_UP:

      /* Only the following events are allowed while powering-up.
      */
      if( SDSS_EVT_IS_USER(event) ||
          SDSS_EVT_IS_HDR_USER(event) ||
          SDSS_EVT_IS_WLAN_USER(event)||
          SDSS_EVT_IS_HYBR_1_USER(event))
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_OPR_MODE_ONLINE:

      /* Only the following events are allowed while in online.
      */
      if( SDSS_EVT_IS_USER(event) ||
          SDSS_EVT_IS_MISC(event) ||
          SDSS_EVT_IS_HDR_USER(event) ||
          SDSS_EVT_IS_WLAN_USER(event)||
          SDSS_EVT_IS_HYBR_1_USER(event) )
      {
        return TRUE;
      }

      switch( ss_state )
      {
        case SDSS_STATE_ACQ:
        case SDSS_STATE_PWR_SAVE:
          if( SDSS_EVT_IS_CDMA_ACQ(event) ||
              SDSS_EVT_IS_AMPS_ACQ(event) ||
              SDSS_EVT_IS_HDR_ACQ(event)  ||
              SDSS_EVT_IS_GW_ACQ(event)  ||
              SDSS_EVT_IS_WLAN_ACQ(event))
          {
            return TRUE;
          }
          break;

        case SDSS_STATE_VRFY:
        case SDSS_STATE_OPR:
          if( SDSS_EVT_IS_CDMA_OPR(event) ||
              SDSS_EVT_IS_AMPS_OPR(event) ||
              SDSS_EVT_IS_HDR_OPR(event)  ||
              SDSS_EVT_IS_GW_OPR(event)   ||
              SDSS_EVT_IS_WLAN_OPR(event) )
          {
            return TRUE;
          }
          break;

        case SDSS_STATE_MAX:
        default:
          SD_CASE_ERR( "state",0,0,0 );
          break;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_OPR_MODE_OFFLINE_CDMA:

      /* Only the following events are allowed while in offline CDMA mode.
      */
      if( SDSS_EVT_IS_USER(event)     ||
          SDSS_EVT_IS_HDR_USER(event) ||
          SDSS_EVT_IS_WLAN_USER(event) ||
          SDSS_EVT_IS_HYBR_1_USER(event)||
          SDSS_EVT_IS_CDMA(event)     ||
          SDSS_EVT_IS_MISC(event) )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_OPR_MODE_OFFLINE_AMPS:

      /* Only the following events are allowed while in offline AMPS mode.
      */
      if( SDSS_EVT_IS_USER(event)     ||
          SDSS_EVT_IS_HDR_USER(event) ||
          SDSS_EVT_IS_WLAN_USER(event) ||
          SDSS_EVT_IS_HYBR_1_USER(event) ||
          SDSS_EVT_IS_AMPS(event)     ||
          SDSS_EVT_IS_MISC(event) )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_OPR_MODE_PWR_DOWN:

      /* Only the following events are allowed while in Power-down, reset or
      ** or LPM mode.
      */
      if( SDSS_EVT_IS_USER(event)     ||
          SDSS_EVT_IS_HDR_USER(event) ||
          SDSS_EVT_IS_WLAN_USER(event) ||
          SDSS_EVT_IS_HYBR_1_USER(event) ||
          SDSS_EVT_IS_MISC(event) )
      {
        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_OPR_MODE_MAX:
    default:
      SD_CASE_ERR( "opr_mode",0,0,0 );

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the SS-Event is not allowed in the current operating
  ** mode - return FALSE.
  */
  return FALSE;

} /* sdss_is_event_allowed */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_recovery

DESCRIPTION
  Performs error recovery when the SS-Client reports a system selection event
  that is not allowed relative to the current state of SD or when the END
  script construct is reached.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  TRUE if event processing should continue, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean               sdss_event_recovery(

        sd_ss_e_type          ss,
            /* System selection item.
            */

        sdss_evt_e_type       *event_ptr,
            /* Pointer to SS-Event received from SS-Front-end.
            */

        sdss_iact_s_type      *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{
  sdss_evt_e_type   event;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( event_ptr != NULL )
  SD_ASSERT( rtrn_iact_ptr != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  event = *event_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Catch all the events that should not be further processed, set the an
  ** appropriate return SS-Action and return FALSE to indicate that no
  ** further processing should be done.
  */
  switch( event )  /*lint -e788 */ /* sparse switch */
  {
    /* User events.
    */
    case SDSS_EVT_USER_OFFLINE_CDMA:
    case SDSS_EVT_USER_OFFLINE_AMPS:
    case SDSS_EVT_USER_PWR_DOWN:
      rtrn_iact_ptr->act = SDSS_IACT_CONTINUE;
      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* CDMA events.
    */
    case SDSS_EVT_CDMA_OPR_REDIR:
      rtrn_iact_ptr->act = SDSS_IACT_REJECT;
      return FALSE;

    case SDSS_EVT_CDMA_OPR_CAM:
    case SDSS_EVT_CDMA_OPR_HDM:
    case SDSS_EVT_CDMA_OPR_ITSPM:
      rtrn_iact_ptr->act = SDSS_IACT_CONTINUE;
      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* AMPS events.
    */
    case SDSS_EVT_AMPS_OPR_ACC_SID:
      rtrn_iact_ptr->act = SDSS_IACT_REJECT;
      return FALSE;

    case SDSS_EVT_AMPS_OPR_CAM:
      rtrn_iact_ptr->act = SDSS_IACT_CONTINUE;
      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* HDR events.
    */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Misc.
    */
    case SDSS_EVT_MISC_ORIG_SUCCESS:
    case SDSS_EVT_MISC_PWR_SAVE_ENTER:
    case SDSS_EVT_MISC_CNF:
      rtrn_iact_ptr->act = SDSS_IACT_CONTINUE;
      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* All other events
    */
    default:
      break;

  } /* switch( event ) */ /*lint +e788 */ /* sparse switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, this event can be processed as power-up event, so set
  ** the returned event according to the SS item and return TRUE to indicate
  ** that further processing should be done.
  */
  switch ( ss )
  {

    case SD_SS_HYBR_1:
      *event_ptr = SDSS_EVT_HDR_USER_HYBR_START;
      break;

    case SD_SS_HYBR_WLAN:
      *event_ptr = SDSS_EVT_WLAN_USER_SS_PREF;
      break;

    case SD_SS_MAIN:
    case SD_SS_MAX:
    default:
      *event_ptr = SDSS_EVT_USER_PWR_UP;
      break;
  }

  return TRUE;

} /* sdss_event_recovery */



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_proc

DESCRIPTION
  Process an SS-Event called for by the SS-Front-end and instruct the
  SS-Front-end of the next SS-Internal-Action to take.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  None

  Note that SS-Internal-Action to be taken by the SS-Front-end is returned
  through the rtrn_iact_ptr parameter.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                  sdss_event_proc(

        sdss_evt_e_type       event,
            /* SS-Event to process.
            */

        sdss_iact_s_type      *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{
  sdss_s_type             *ss_ptr   = sdss_ptr(SD_SS_MAIN);
  sd_ss_e_type            ss        = SD_SS_MAIN;
  sd_ss_mode_pref_e_type  mode_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the returned SS-Internal-action to empty.
  */
  memset( rtrn_iact_ptr, 0, sizeof(*rtrn_iact_ptr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is an acquisition event, service the schedule.
  */
  if( SDSS_EVT_IS_XXX_ACQ(event) )
  {
    /* Service the systems that are on the schedule list. If hybrid
    ** preference is CDMA/HDR (i.e. MS is performing CDMA/HDR hybrid
    ** operation), only service non-HDR systems.
    */
    mode_pref = SD_SS_MODE_PREF_ANY;
    if( ss_ptr->hybr_pref == SD_SS_HYBR_PREF_CDMA_HDR )
    {
      mode_pref = SD_MODE_DELETE( mode_pref, SD_SS_MODE_PREF_HDR );
    }

    sdss_sr_list_sched_srv( SDSR_SCHED_LST,
                            mode_pref,
                            ss_ptr->acq_list,
                            ss_ptr->acq_pos );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* When starting a GW acquisition, clear the previous colloc list
    */
    if(SDSS_EVT_IS_GW_ACQ(event) && ss_ptr->prev_sys_info.assn_tag != SDSS_ASSN_TAG_NONE)
    {
      sdss_addn_sys_info_init(&(ss_ptr->prev_sys_info));
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Generate an error if SS-Event is not expected in the current operation
  ** mode.
  */
  if( ! sdss_is_event_allowed(event, ss_ptr->ss_state, ss_ptr->opr_mode) )
  {
    //SD_ERR_FATAL( "Event not allowed!",0,0,0 );
    SD_ERR( "Event=%d not allowed, state =%d, opr mode =%d!",
         event, ss_ptr->ss_state, ss_ptr->opr_mode);
    SD_ASSERT( FALSE );

    /* If no further recovery should be performed, return now.
    */
    if( ! sdss_event_recovery(ss, &event, rtrn_iact_ptr) )
    {
      return;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the TRUE event. Scripts can generate their own events, the
  ** TRUE event will get lost if we don't update the true event
  */
  ss_ptr->true_event              = event;
  ss_ptr->is_true_event_processed = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the engines to process this event.
  */
  sdss_eng_stack_proc( &sdss_eng_stack, ss_ptr, event, rtrn_iact_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->is_true_event_processed = TRUE;

  /* Update the SS-State and inform the SS-Front-end of the current SS-State.
  */
  ss_ptr->ss_state = sdss_state_next( ss,
                                      ss_ptr->ss_state,
                                      event,
                                      rtrn_iact_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is ACQUIRE do the following:
  */
  if( rtrn_iact_ptr->act == SDSS_IACT_ACQUIRE )
  {
    /* Update the most recent acquisition list and position per the list
    ** and position that are being attempted acquisition.
    */
    ss_ptr->acq_list  = rtrn_iact_ptr->prm.acq.list;
    ss_ptr->acq_pos   = rtrn_iact_ptr->prm.acq.pos;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the redirection indication is CDMA GSRM, set the returned
    ** max_redirect_delay per the max_redirect_delay value of the SS-Core.
    ** Else, set the returned max_redirect_delay to
    ** SD_SS_MAX_REDIR_DELAY_NONE.
    */
    if( ss_ptr->redir == SD_SS_REDIR_CDMA_GSRDM )
    {
      rtrn_iact_ptr->prm.acq.max_redirect_delay =
                                            ss_ptr->redir_max_redirect_delay;
    }
    else
    {
      rtrn_iact_ptr->prm.acq.max_redirect_delay = SD_SS_MAX_REDIR_DELAY_NONE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Increment the script execution control counter.
  */
  ss_ptr->scr_ctrl_cnt++;

} /* sdss_event_proc */


#ifdef FEATURE_TECH_IND_SD_STACK_ARCH
/* <EJECT> */
/*===========================================================================

FUNCTION sdss_hybr_1_event_proc

DESCRIPTION
  Process an HDR related SS-Event called for by the SS-Front-end and instruct
  the SS-Front-end of the next SS-Internal-Action to take.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  None

  Note that SS-Internal-Action to be taken by the SS-Front-end is returned
  through the rtrn_iact_ptr parameter.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                  sdss_hybr_1_event_proc(

        sdss_evt_e_type       event,
            /* SS-Event to process.
            */

        sdss_iact_s_type      *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
)
{
  sdss_s_type   *ss_ptr        = sdss_ptr(SD_SS_MAIN);
  sdss_s_type   *ss_hybr_1_ptr = sdss_ptr(SD_SS_HYBR_1);
  sd_ss_e_type  ss             = SD_SS_HYBR_1;
  sd_ss_mode_pref_e_type mode_pref = SD_SS_MODE_PREF_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( rtrn_iact_ptr != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the returned SS-Internal-action to empty.
  */
  memset( rtrn_iact_ptr, 0, sizeof(*rtrn_iact_ptr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is an acquisition event, service the schedule.
  */
  if( SDSS_EVT_IS_XXX_ACQ(event) )
  {
    /* Service the systems that are on the schedule list. If hybrid
    ** preference is CDMA/HDR (i.e. MS is performing CDMA/HDR hybrid
    ** operation), only service non-HDR systems.
    */
    mode_pref = ss_ptr->stack_mode_pref[SD_SS_HYBR_1];
    sdss_sr_list_sched_srv( SDSR_HYBR_1_SCHED_LST,
                            mode_pref,
                            ss_hybr_1_ptr->acq_list,
                            ss_hybr_1_ptr->acq_pos );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Generate an error if SS-Event is not expected in the current operation
  ** mode.
  */
  if( ! sdss_is_event_allowed(event,
                              ss_hybr_1_ptr->ss_state,
                              ss_ptr->opr_mode) )
  {
    //SD_ERR_FATAL( "Event not allowed!",0,0,0 );
    SD_ERR( "Event=%d not allowed, state =%d, opr mode =%d!",
         event, ss_hybr_1_ptr->ss_state, ss_ptr->opr_mode);
    SD_ASSERT( FALSE );

    /* If no further recovery should be performed, return now.
    */
    if( ! sdss_event_recovery(ss, &event, rtrn_iact_ptr) )
    {
      return;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the engines to process this event.
  */
  sdss_eng_stack_proc( &sdss_hybr_1_eng_stack,
                       ss_hybr_1_ptr,
                       event,
                       rtrn_iact_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-State and inform the SS-Front-end of the current SS-State.
  */
  ss_hybr_1_ptr->ss_state = sdss_state_next( ss,
                                             ss_hybr_1_ptr->ss_state,
                                             event,
                                             rtrn_iact_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is ACQUIRE do the following:
  */
  if( rtrn_iact_ptr->act == SDSS_IACT_ACQUIRE )
  {
    /* Update the most recent acquisition list and position per the list
    ** and position that are being attempted acquisition.
    */
    ss_hybr_1_ptr->acq_list  = rtrn_iact_ptr->prm.acq.list;
    ss_hybr_1_ptr->acq_pos   = rtrn_iact_ptr->prm.acq.pos;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the redirection indication is CDMA GSRM, set the returned
    ** max_redirect_delay per the max_redirect_delay value of the SS-Core.
    ** Else, set the returned max_redirect_delay to
    ** SD_SS_MAX_REDIR_DELAY_NONE.
    */
    if( ss_hybr_1_ptr->redir == SD_SS_REDIR_CDMA_GSRDM )
    {
      rtrn_iact_ptr->prm.acq.max_redirect_delay =
                                            ss_hybr_1_ptr->redir_max_redirect_delay;
    }
    else
    {
      rtrn_iact_ptr->prm.acq.max_redirect_delay = SD_SS_MAX_REDIR_DELAY_NONE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Increment the script execution control counter.
  */
  ss_hybr_1_ptr->scr_ctrl_cnt++;

} /* sdss_hybr_1_event_proc */

#elif defined FEATURE_HDR_HYBRID
#error code not present
#endif /* FEATURE_HDR_HYBRID */


#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_sys_resel_start_time

DESCRIPTION
  Return the start time for the specified system reselection timer.

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  dword                    sdss_get_sys_resel_uptime(

        sd_ss_e_type             ss,
            /* System selection item.
            */

        int                      timer
            /* Specify the system reselection timer type for the start time.
            */
)
{
  const sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( timer, SDSSSCR_CONT_TYPE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ss_ptr->cont_uptime[timer];

}/* sdss_get_sys_resel_uptime */


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_new_acq_cnt

DESCRIPTION
  Return the start time for the specified system reselection timer.

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  byte                      sdss_get_new_acq_cnt(

        sd_ss_e_type             ss
            /* System selection item.
            */
)
{

  const sdss_s_type   *ss_ptr   = sdss_ptr( ss );
    /* Pointer to associated System Selection information structure.
    */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ss_ptr->new_acq_cnt;

}


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sys_lost_update

DESCRIPTION
  Update the system lost reason of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                          sdss_sys_lost_update(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HDR.
            */

        sd_ss_sys_lost_e_type         sys_lost
            /* System lost reason.
            */

)
{
  sdss_s_type   *ss_ptr   = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( sys_lost, SD_SS_SYS_LOST_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->sys_lost = sys_lost;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

} /* sdss_sys_lost_update */


#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
#endif


/*===========================================================================

FUNCTION sdss_get_conn_mode_uoos_timers

DESCRIPTION
  Get the connected mode UOOS timers

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                  sdss_get_conn_mode_uoos_timers(

        sd_ss_e_type ss,
            /* The instance
            */

        dword    *conn_mode_awake_time,
            /* The total scan time
            */

        dword    *conn_mode_sleep_time
            /* UMTS scan time
            */
)
/*lint -esym(715,ss) */
{

  sdss_s_type   *ss_ptr       = sdss_ptr(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( conn_mode_awake_time  != NULL )
  {
    *conn_mode_awake_time   = ss_ptr->rem_uoos_awake_time;
  }

  if( conn_mode_sleep_time   != NULL )
  {
    *conn_mode_sleep_time    = ss_ptr->rem_uoos_sleep_time;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
}

/*===========================================================================

FUNCTION sdss_is_loop_cond

DESCRIPTION
  Returns TRUE is loop_cond is true

DEPENDENCIES
  None.

RETURN VALUE
 True : loop_cond is TRUE
 False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean               sdss_is_loop_cond(

        sd_ss_e_type          ss,
            /* The ss associated with the parser
            */

        sdss_loop_cond_e_type loop_cond
            /* The loop condition
            */
)
/*lint -esym(715,ss) */
{

  sdss_s_type   *ss_ptr       = sdss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ss == SD_SS_MAIN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( loop_cond )
  {
    case SDSS_LOOP_COND_GW_SCAN_ABORTED:
      return ( ss_ptr->acq_gw_scan_status == SD_SS_GW_SCAN_STATUS_ABORTED );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_LOOP_COND_IS_HDR_ACTIVITY:
      return( ss_ptr->acq_thrttl.is_thrttl_on );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_LOOP_COND_NONE:
    case SDSS_LOOP_COND_MAX:
    default:
      return FALSE;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  }

} /* sdss_is_loop_cond */
/*lint +esym(715,ss) */


/*===========================================================================

FUNCTION sdss_is_if_cond

DESCRIPTION
  Returns TRUE is if_cond is true

DEPENDENCIES
  None.

RETURN VALUE
 True : if_cond is TRUE
 False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean               sdss_is_if_cond(


        sd_ss_e_type          ss,
            /* The ss associated with the parser
            */

        sdss_if_cond_e_type   if_cond,
            /* The if condition
            */

        sdss_cmp_criteria_e_type cmp_criteria,
            /* The Comparision criteria
            */

        word                  para1,
            /* Depending on the condition, we may require additional parameters
            */

        word                  para2
            /* Depending on the condition, we may require additional parameters
            */
)
/*lint -esym(715,ss) */
/*lint -esym(715,cmp_criteria) */
/*lint -esym(715,para1) */
/*lint -esym(715,para2) */
{

  sdss_s_type   *ss_ptr = sdss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( if_cond, SDSS_IF_COND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmp_criteria, SDSS_CMP_CRITERIA_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( if_cond )
  {
    case SDSS_IF_COND_GCF_TESTING:

      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:
          return ( BOOLEAN( ss_ptr->gprs_anite_gcf == TO_WORD( para1, para2 )));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          return ( BOOLEAN( ss_ptr->gprs_anite_gcf != TO_WORD( para1, para2 ) ) );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IF_COND_TOP_SYS_MODE_GW:

      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:

          return ( sdsr_is_sys(
                              SYS_SYS_MODE_GW,
                              sdss_sr_list_map( ss_ptr, (sdsr_e_type) para2 ),
                              0 ) );
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          return ( !sdsr_is_sys(
                              SYS_SYS_MODE_GW,
                              sdss_sr_list_map( ss_ptr, (sdsr_e_type) para2 ),
                              0 ) );
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IF_COND_TRUE_EVENT:
    {
      sdss_evt_e_type          chk_event =
                                   (sdss_evt_e_type) TO_WORD( para1, para2 );
      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:
          return (BOOLEAN( chk_event == ss_ptr->true_event ));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          return (BOOLEAN( chk_event != ss_ptr->true_event ));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }
    } /* case IS_TRUE_EVENT*/

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IF_COND_NEW_TRUE_EVENT:
    {
      sdss_evt_e_type          chk_event =
                                   (sdss_evt_e_type) TO_WORD( para1, para2 );
      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:
          return (BOOLEAN( !ss_ptr->is_true_event_processed \
                           && ( chk_event == ss_ptr->true_event )));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          return (BOOLEAN( (!ss_ptr->is_true_event_processed \
                            && chk_event != ss_ptr->true_event )));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }
    } /* case IS_TRUE_EVENT*/

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IF_COND_CURR_EVENT:
    {
      sdss_evt_e_type          chk_event =
                                   (sdss_evt_e_type) TO_WORD( para1, para2 );
      sdss_eng_stack_s_type   *stack_ptr;
      sdss_eng_s_type         *eng_ptr;

      switch( ss )
      {
        case SD_SS_MAIN:
          stack_ptr = &sdss_eng_stack;
          break;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_SS_HYBR_1:
          #ifdef FEATURE_HDR
#error code not present
          #endif
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          /* Delibrate fall down to the default is FEATURE_HDR is not defined
          */

        case SD_SS_HYBR_WLAN:

          #ifdef FEATURE_WLAN
#error code not present
          #endif
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          /* Delibrate fall down to the default is FEATURE_WLAN is not defined
          */
        case SD_SS_MAX:
        default:
          SD_MSG_HIGH("Invalid ss %d", ss, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }/* switch ss */

      eng_ptr = sdss_eng_stack_top( stack_ptr );
      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:
          return ( BOOLEAN( chk_event == eng_ptr->event ));
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          return ( BOOLEAN( chk_event != eng_ptr->event ));
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_CASE_ERR("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }
    } /* case IS_CURR_EVENT*/

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDSS_IF_COND_USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW:
    {
      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:
          return (sdss_is_user_1x_gw_and_curr_not_1x_gw(ss_ptr) );
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          return (!sdss_is_user_1x_gw_and_curr_not_1x_gw(ss_ptr) );
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }
    }/* case IF_COND_USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IF_COND_USER_HDR_GW_MODE_PREF_AND_CURR_NOT_HDR_GW:
    {
      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:
          return (sdss_is_user_hdr_gw_and_curr_not_hdr_gw(ss_ptr) );
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          return (!sdss_is_user_hdr_gw_and_curr_not_hdr_gw(ss_ptr) );
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      } /* case IF_COND_USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW */
    }

    case SDSS_IF_COND_USER_1X_HDR_GW_MODE_PREF_AND_CURR_NOT_1X_HDR_GW:
    {
      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:
          return (sdss_is_user_hdr_gw_and_curr_not_hdr_gw(ss_ptr) ||
                  sdss_is_user_1x_gw_and_curr_not_1x_gw(ss_ptr) );
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          return (!( sdss_is_user_hdr_gw_and_curr_not_hdr_gw(ss_ptr) ||
                     sdss_is_user_1x_gw_and_curr_not_1x_gw(ss_ptr) ) );
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
      }
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IF_COND_SS_STATE:
    {
        switch( cmp_criteria )
        {
          case SDSS_CMP_CRITERIA_EQUAL_TO:
            return (ss_ptr->ss_state ==
                               (sdss_state_e_type) TO_WORD( para1, para2 ));

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
            return (ss_ptr->ss_state !=
                                (sdss_state_e_type) TO_WORD( para1, para2 ));

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case SDSS_CMP_CRITERIA_NONE:
          case SDSS_CMP_CRITERIA_MAX:
          default:
            SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
            return FALSE;
        }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IF_COND_ACQ_SYS_PRL_DESIG:
    {
      sdsr_s_type         *acq_sys_ptr  =
               sdsr_list_get_ptr( sdss_sr_list_map(ss_ptr,SDSR_ACQ_SYS), 0 );

      sdss_prl_val_e_type chk_prl_desig =
                               (sdss_prl_val_e_type) TO_WORD( para1, para2 );

      SD_ASSERT_ENUM_IS_INRANGE( chk_prl_desig, SDSS_PRL_VAL_MAX );

      if( acq_sys_ptr == NULL )
      {
        SD_ERR( "get_ptr SDSR_ACQ_SYS failed",0,0,0 );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:
          return ( ( acq_sys_ptr != NULL ?
                     sdss_sr_is_prl( acq_sys_ptr, chk_prl_desig ) : FALSE ) );
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          return ( ( acq_sys_ptr != NULL ?
                     !sdss_sr_is_prl( acq_sys_ptr, chk_prl_desig ) : TRUE ) );
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_CASE_ERR("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }
    } /* SDSS_IF_COND_ACQ_SYS_PRL_DESIG */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IF_COND_HYBR_PREF:
    {
      #ifdef FEATURE_HDR_HYBRID
#error code not present
      #else
      SD_CASE_ERR("Invalid featurization", 0, 0, 0 );
      return FALSE;
      #endif


    } /* case HYBR_PREF*/

    case SDSS_IF_COND_ORIG_MODE:
      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:
          return ( BOOLEAN( ss_ptr->orig_mode == (sd_ss_orig_mode_e_type)TO_WORD( para1, para2 )));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          return ( BOOLEAN( ss_ptr->orig_mode != (sd_ss_orig_mode_e_type)TO_WORD( para1, para2 )));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      } /* case SDSS_IF_COND_ORIG_MODE */
     

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IF_COND_SS_PREF_CHGD:
    {

      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:
          return ( BOOLEAN (( ss_ptr->true_event == SDSS_EVT_USER_SS_PREF ) &&
                   (ss_ptr->pref_reason == SD_SS_PREF_REAS_USER ||
                    ss_ptr->pref_reason == SD_SS_PREF_REAS_MULTIMODE_SCAN ||
                    ss_ptr->pref_reason == SD_SS_PREF_REAS_USER_RESEL ) &&
                   sdss_is_pref_update( ss, SDSS_PREF_UPDATED_TRUE_PREF )));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:

          return ( BOOLEAN (( ss_ptr->true_event != SDSS_EVT_USER_SS_PREF ) ||
                   (ss_ptr->pref_reason != SD_SS_PREF_REAS_USER &&
                    ss_ptr->pref_reason != SD_SS_PREF_REAS_MULTIMODE_SCAN &&
                    ss_ptr->pref_reason != SD_SS_PREF_REAS_USER_RESEL ) ||
                   ! sdss_is_pref_update( ss, SDSS_PREF_UPDATED_TRUE_PREF )));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }
    } /* case SDSS_IF_COND_SS_PREF_CHGD */

    case SDSS_IF_COND_SRV_PREF_LIST_SIZE:
    {

      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:
          return ( BOOLEAN (sdsr_list_cnt(SDSR_SRV_PREF_LST) == TO_WORD( para1, para2 )));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          return ( BOOLEAN (sdsr_list_cnt(SDSR_SRV_PREF_LST) != TO_WORD( para1, para2 )));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }
    } /* case SDSS_IF_COND_SRV_PREF_LIST_SIZE */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IF_COND_STACK_SYNC_UP:
    {
      boolean is_timer_evt = FALSE;
      boolean is_user_evt = FALSE;
      boolean is_pref_reas_sync_up = FALSE;
      boolean is_sync_up = FALSE;

      is_timer_evt = ( ss_ptr->true_event == SDSS_EVT_MISC_TIMER_RESEL_NO ||
                       ss_ptr->true_event == SDSS_EVT_MISC_TIMER_RESEL_OK );

      switch ( ss )
      {
        case  SD_SS_MAIN:
           is_user_evt = (ss_ptr->true_event == SDSS_EVT_USER_SS_PREF );
           break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_SS_HYBR_HDR:
          is_user_evt = (ss_ptr->true_event == SDSS_EVT_HDR_USER_SS_PREF );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_SS_HYBR_WLAN:
          is_user_evt = (ss_ptr->true_event == SDSS_EVT_WLAN_USER_SS_PREF );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_SS_MAX:
        default:
          SD_MSG_HIGH("Invalid ss %d", ss, 0, 0 );
          return FALSE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      is_pref_reas_sync_up = 
                      (ss_ptr->pref_reason == SD_SS_PREF_REAS_STACK_SYNC_UP);

      is_sync_up = ( !is_timer_evt                            && 
                     !( is_user_evt && is_pref_reas_sync_up ) &&
                    !ss_ptr->gprs_anite_gcf );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:

          return BOOLEAN( is_sync_up == TO_WORD( para1, para2 ));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:

          return BOOLEAN( is_sync_up != TO_WORD( para1, para2 ));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }
    } /* case SDSS_IF_COND_STACK_SYNC_UP */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IF_COND_WLAN_SCAN_FAIL_REAS:
    {
      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:

          return BOOLEAN( ss_ptr->wlan_fail_reason == (sd_ss_wlan_fail_reas_e_type)
                                                      TO_WORD( para1, para2 ));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:

          return BOOLEAN( ss_ptr->wlan_fail_reason != (sd_ss_wlan_fail_reas_e_type)
                                                      TO_WORD( para1, para2 ));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }
    }

    case SDSS_IF_COND_ENG_SCR_TIMER_FLAG:
    {
      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:

          return BOOLEAN( ss_ptr->is_eng_scr_timer_set == \
                          TO_WORD( para1, para2 ));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:

          return BOOLEAN( ss_ptr->is_eng_scr_timer_set != \
                          TO_WORD( para1, para2 ));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }
    } /* case SDSS_IF_COND_ENG_SCR_TIMER_FLAG */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IF_COND_TMR_CONN_MODE_UOOS_SCAN_SET:
    {
      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:

          /*lint -e731 */
          return BOOLEAN( ( ss_ptr->rem_uoos_awake_time > 0 ) == TO_WORD( para1, para2 ) );
          /*lint +e731 */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          /*lint -e731 */
          return BOOLEAN( ( ss_ptr->rem_uoos_awake_time > 0 ) != TO_WORD( para1, para2 ) );
          /*lint +e731 */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }
    } /* case SDSS_IF_COND_TMR_CONN_MODE_UOOS_SCAN_SET */

    case SDSS_IF_COND_IS_HDR_REDIR:
    {
      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:
        {
          /*lint -e731 */
          return BOOLEAN( sdss_is_redir(ss, SDSS_REDIR_VAL_HDR_ANY) == \
                            TO_WORD( para1, para2 ) );
                                
          /*lint +e731 */
        }
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          /*lint -e731 */
          return BOOLEAN( sdss_is_redir(ss, SDSS_REDIR_VAL_HDR_ANY) != \
                           TO_WORD( para1, para2 ) );
          /*lint +e731 */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IF_COND_TOP_SYS_MODE_GW_SUBS_AVAIL:
    {
      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:

          return ( ss_ptr->is_gw_subsc_avail && 
                        sdsr_is_sys(
                              SYS_SYS_MODE_GW,
                              sdss_sr_list_map( ss_ptr, (sdsr_e_type) para2 ),
                              0 ) );
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          return ( !(ss_ptr->is_gw_subsc_avail &&
                            sdsr_is_sys(
                              SYS_SYS_MODE_GW,
                              sdss_sr_list_map( ss_ptr, (sdsr_e_type) para2 ),
                              0 ) ));
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IF_COND_GW_SUBS_AVAIL:
    
      switch( cmp_criteria )
      {
        case SDSS_CMP_CRITERIA_EQUAL_TO:
          return ( BOOLEAN(ss_ptr->is_gw_subsc_avail == TO_WORD( para1, para2 )));
          
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
          return ( !BOOLEAN(ss_ptr->is_gw_subsc_avail == TO_WORD( para1, para2 )));
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SDSS_CMP_CRITERIA_NONE:
        case SDSS_CMP_CRITERIA_MAX:
        default:
          SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
          return FALSE;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }
      
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
    case SDSS_IF_COND_GW_PWR_SAVE_ACQ_STAGE:
    {
        switch( cmp_criteria )
        {
          case SDSS_CMP_CRITERIA_EQUAL_TO:
            return (ss_ptr->gw_pwr_save_acq_stage_val ==
                               (byte) TO_WORD( para1, para2));

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case SDSS_CMP_CRITERIA_NOT_EQUAL_TO:
            return (ss_ptr->gw_pwr_save_acq_stage_val !=
                                (byte) TO_WORD( para1, para2 ));

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case SDSS_CMP_CRITERIA_NONE:
          case SDSS_CMP_CRITERIA_MAX:
          default:
            SD_MSG_HIGH("Invalid cmp_criteria %d", cmp_criteria, 0, 0 );
            return FALSE;
        }
    }
    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
    case SDSS_IF_COND_NONE:
    case SDSS_IF_COND_MAX:
    default:

      SD_CASE_ERR("Invalid if_cond %d", if_cond, 0, 0 );

      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* if cond */
} /* sdss_is_if_cond */

/*lint +esym(715,ss) */
/*lint +esym(715,cmp_criteria) */
/*lint +esym(715,para1) */
/*lint +esym(715,para2) */

/*===========================================================================

FUNCTION sdss_reset_gw_acq_para

DESCRIPTION
  Reset the GW scan parameters

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                  sdss_reset_gw_acq_para(

        sd_ss_e_type          ss
            /* The ss associated with the parser
            */
)
/*lint -esym(715,ss) */
{

  sdss_s_type   *ss_ptr       = sdss_ptr(SD_SS_MAIN);

  if( ss != SD_SS_MAIN )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->acq_gw_scan_status      = SD_SS_GW_SCAN_STATUS_NONE;
  ss_ptr->acq_gw_use_timer        = FALSE;
  ss_ptr->acq_gw_new_acq          = TRUE;
  ss_ptr->acq_gw_total_scan_time  = ~0;
  ss_ptr->acq_gw_wcdma_scan_time  = ~0;
  ss_ptr->acq_gw_gsm_scan_time    = ~0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return;
}
/*lint +esym(715,ss) */

#ifdef FEATURE_HYBR_GW
#error code not present
#endif


/*===========================================================================

FUNCTION sdss_is_prl_support_hdr

DESCRIPTION

  Return the indication whether PRL Support HDR or not based on current
  BAND PREF

  MODE
DEPENDENCIES
  None

RETURN VALUE
  Return the filtered mode preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean    sdss_is_prl_support_hdr(void)
{

  sdss_s_type   *ss_ptr   = sdss_ptr(SD_SS_MAIN);

  return  ss_ptr->is_prl_support_hdr;

} /* sdss_misc_is_prl_support_hdr */

/*===========================================================================

FUNCTION sdss_clear_bsr_timer

DESCRIPTION

  Clears BSR timer uptime.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

MMEXTN void sdss_clear_bsr_timer(void)
{
  sdss_s_type   *ss_ptr   = sdss_ptr(SD_SS_MAIN);

  ss_ptr->bsr_timer = 0;
  return;
  
} /* sdss_bsr_sr_get */

/*===========================================================================

FUNCTION sdss_check_bsr_timer

DESCRIPTION

  Checks whether to clear BSR timer uptime and if needed clears BSR timer uptime.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN void sdss_check_bsr_timer(void)
{

  sdss_s_type   *ss_ptr   = sdss_ptr(SD_SS_MAIN);
  sdsr_s_type* srv_sys_ptr = NULL; 
  #ifdef FEATURE_HDR
#error code not present
  #endif /* FEATURE_HDR */
  boolean       match = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  srv_sys_ptr = sdsr_list_get_ptr(SDSR_SRV_SYS, 0);

  if(srv_sys_ptr == NULL)
  {
    return;
  }

  /* Compare bsr sys with SRV_SYS sys */
  if (ss_ptr->bsr_sys.mode!= srv_sys_ptr->sys.mode)
  {
    match = FALSE;
  }
  else
  {
    /* Compare the two SIDs based on the system's mode.
    */
    switch( srv_sys_ptr->sys.mode )
    {
      case SD_MODE_AMPS:
      case SD_MODE_CDMA:

        if( ss_ptr->bsr_sid.is95.sid != srv_sys_ptr->sid.is95.sid ||
            ss_ptr->bsr_sid.is95.nid != srv_sys_ptr->sid.is95.nid )
        {
          match = FALSE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
      case SD_MODE_HDR:
  
        #ifdef FEATURE_HDR
#error code not present
        #endif /* FEATURE_HDR */
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SD_MODE_GSM:
      case SD_MODE_WCDMA:
      case SD_MODE_GW:

        match = sys_plmn_match( ss_ptr->bsr_sid.plmn.plmn_id,
                               srv_sys_ptr->sid.plmn.plmn_id );
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SD_MODE_WLAN:
      case SD_MODE_NONE:
      case SD_MODE_INACT:
      case SD_MODE_GPS:
      case SD_MODE_MAX:
      default:
        SD_MSG_HIGH("CHECK_BSR_TIMER not valid for mode %d",srv_sys_ptr->sys.mode,0,0);
        return;
    }

  } /* else  */
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset BSR timer uptime if SRV_SYS does not matches with BSR system.
  */
  if(match == FALSE)
  {
    sdss_clear_bsr_timer();
  }
  
  return;
}

/*===========================================================================

FUNCTION sdss_set_gw_pwr_save_acq_stage

DESCRIPTION

  Set the GW power save acquisition stage.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN void sdss_set_gw_pwr_save_acq_stage(

            sd_ss_e_type                  ss,
            /* SS ptr */

            byte                             val
            /* STAGE value */
)
{
   sdss_s_type   *ss_ptr   = sdss_ptr( ss );
    
   ss_ptr->gw_pwr_save_acq_stage_val = val;

   SD_MSG_HIGH("GW PWR SAVE STAGE %d",ss_ptr->gw_pwr_save_acq_stage_val,0,0);
   return;
}

/**************************************************************************
FUNCTION sdss_gw_sim_domain_valid

DESCRIPTION

  Checks whether SIM is valid for all selected domains in current SS.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN boolean sdss_gw_sim_domain_valid(
           sd_ss_e_type          ss
            /* The ss associated with the parser
            */
)
{

  /*lint -esym(550, ss_ptr) */ /* not accessed */
  sdss_s_type   *ss_ptr   = sdss_ptr(ss);

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) 
#error code not present
  #endif
  ss_ptr = NULL;
  return FALSE;
}
  /*lint +esym(550, ss_ptr) */
