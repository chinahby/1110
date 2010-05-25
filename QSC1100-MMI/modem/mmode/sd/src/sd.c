/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S Y S T E M   D E T E R M I N A T I O N

                          F R O N T   E N D

GENERAL DESCRIPTION
  This file makes up the Front-end of System Determination.

  System Determination (SD) is a library that provides the following type of
  services to its clients:

    - System Selection

    - Service Indicators

    - Miscellaneous

  System Selection:
    SD provides the System Selection Client (SS-Client) with system selection
    service upon receipt of system selection indications (SS-Indications)
    that are being received from the SS-Client. The SS-Indications are
    received in the form of function calls. The return value from these
    function calls instruct the SS-Client of the next System Selection action
    (SS-Action) to be taken, as follows:

      - Continue:               Continue with current mode, band and/or CDMA
                                channel/AMPS system.

      - Acquire CDMA:           Attempt acquiring a CDMA system.

      - Acquire AMPS:           Attempt acquiring an AMPS system.

      - Acquire HDR:            Attempt acquiring an HDR system.

      - Dedicated Measurement:  Request channels strength measurement from
                                the searcher in dedicated mode.

      - Background Measurement: Request channels strength measurement from
                                the searcher in background mode.

      - Enter Power-save:       Enter Power-Save (deep sleep) mode of
                                operation.

      - Accept:                 Accept BS proposed mode, band and/or channel.

      - Reject:                 Reject BS proposed mode, band and/or channel.

      - Release:                Release current call.


  Service Indicators:

    SD provide an interface for a single client to register a callback
    function to be notified of the following service indicators status.

      - Service On/Off Indicator

      - Mode Indicator

      - Band Indicator

      - Roaming Indicator

    SD notifies the registered client (by calling on the client's provided
    callback function) of a new service indicators status whenever one or
    more of the service indicators change their value.


  Miscellaneous:

    SD provides an interface for clients to poll the following miscellaneous
    information:

      - PRL version:            Returns the version number of the PRL that is
                                associated with the currently selected NAM.

      - Preference validation:  Inform the caller as to whether a specified
                                system selection preference (like analog-
                                only mode) is supported by the PRL that is
                                associated with the currently selected NAM.
                                For example, analog-only mode preference is
                                only supported by a PRL that contains analog
                                acquisition records.

      - Capability validation:  Inform the caller as to whether the target
                                supports a specified combination of mode and
                                band.


  The following diagram shows a 2nd level decomposition of system
  determination. The arrows in this diagram represent function calls.


               2nd Level System Determination Decomposition
               --------------------------------------------

  +----------------------------------------------------------------------+
  |                              SD Clients                              |
  |  +--------------------+ +--------------------+ +-------------------+ |
  |  | Service Indicators | |  System Selection  | |   Misc. Service   | |
  |  | Client (SI-Client) | | Client (SS-Client) | | Client (MS-Client)| |
  |  +--------------------+ ---------------------+ +-------------------+ |
  +---------|--/|\-------------|--------------------------------|--------+
            |   |              |                                |
            |   |              |                                |
  +=========|===|==============|================================|========+
  |        \|/  |             \|/ SD Front-end                 \|/       |
  |   +-------------+  +-------------------------------+  +-----------+  |
  |   |             |  |                               |  |           |  |
  |   |   Service   |  |       System Selection        |  |   Misc.   |  |
  |   |  Indicators |  |           Front-end           |  |  Service  |  |
  |   |  (sd_si_xxx)|  |          (sd_ss_xxx)          |  |(sd_ms_xxx)|  |
  |   |             |  |                               |  |           |  |
  |   +-------------+  +-------------------------------+  +-----------+  |
  |           |            |                      |                      |
  +===========|============|======================|======================+
              |            |                      |
              |            |                      |
  +-----------|------------|----------------------|----------------------+
  |           |            |       SD Core        |                      |
  |          \|/          \|/                    \|/                     |
  |   +--------------------------+        +--------------------------+   |
  |   |                          |        |                          |   |
  |   |                          |        |                          |   |
  |   |                          |        |          Database        |   |
  |   |   System Selection Core  |------->|         (sdsr_xxx)       |   |
  |   |        (sdss_xxx)        |        |         (sdprl_xxx)      |   |
  |   |                          |        |                          |   |
  |   |                          |        |                          |   |
  |   |                          |        |                          |   |
  |   +--------------------------+        +--------------------------+   |
  |                                                                      |
  +----------------------------------------------------------------------+

  System Selection Front-end:
    Encapsulates the functionality that is associated with SS-Indications
    interfacing. The SS-Front-end is responsible for updating the System-
    Record with the information that is being received in the SS-Indications,
    as well as, mapping the SS-Indications into SS-Events that can be
    processed by the SS-Core.

  Database:
    Encapsulates all the knowledge that SD has of systems. This knowledge
    comes from two major sources; programmable knowledge (mostly in the form
    of a PRL and a SID/NID list that are programmed into NV) and accumulated
    knowledge that is collected during phone operation.

  System Selection Core:
    Encapsulates all the functionality that is associated with processing the
    SS-Events that are received from the SS-Front-end and for instructing the
    SS-Front-end of the next SS-Action to perform (e.g. acquire a CDMA
    system). The SS-Core operation is mostly controlled by SS-Scripts that
    are associated with the SS-Events that are being received from the
    SS-Front-end.

  Service Indicators:
    Encapsulate all the functionality that is associated with service
    indicators service.

  Miscellaneous Service:
    Encapsulate all the functionality that is associated with SD
    miscellaneous services.


  The following diagram shows the event flow between the SS-Client,
  SS-Front-end, SS-Core and the System Record that takes place when the
  SS-Client calls on an SS-Indication function.


                        SS-Indication Event Follow
                        --------------------------

  +-----------+       +------------+        +-----------+         +---------+
  |           |       |            |        |           |         |         |
  | SS-Client |       |SS-Front-end|        |  SS-Core  |         | Database|
  |           |       |            |        |           |         |         |
  +-----------+       +------------+        +-----------+         +---------+
       |                     |                    |                    |
       |     SS-Indication   |                    |                    |
      (1)-------------------->                    |                    |
       |   sd_ss_ind_xxx()   |                    |                    |
       |                     |             Update Database             |
       |                    (2)---------------------------------------->
       |                     |               sdsr_xxx()                |
       |                     |               sdprl_xxx()               |
       |                     |                    |                    |
       |                     |  Process SS-Event  |                    |
       |                    (3)------------------->                    |
       |                     |     sdss_xxx()     |                    |
       |                     |                    |                    |
       |                     |                   (4)                   |
       |                     |                    |                    |
       |                     |                    |Retrieve System Info|
       |                     |                   (5)------------------->
       |                     |                    |     sdsr_xxx()     |
       |                     |                    |     sdprl_xxx()    |
       |                     |                    |                    |
       |                     |                    < - - - - - - - - - -|
       |                     |                    |                    |
       |                     | Returned SS-Action |                    |
       |                     <- - - - - - - - - -(6)                   |
       |                     |    SDSS_IACT_XXX   |                    |
       | Returned SS-Action  |                    |                    |
       <- - - - - - - - - - (7)                   |                    |
       |   SD_SS_ACT_XXX     |                    |                    |
       |                     |                    |                    |


       Function Call:         ----------->
       Function Call Return:  <- - - - - -


  1. The SS-Client calls on an SS-Indication function.

  2. The SS-Front-end updates the Database with the information that comes
     with the SS-Indication, as necessary.

  3. The SS-Front-end maps the SD-Indication into an SS-Event and calls on
     the SS-Core to process the SS-Event.

  4. The SS-Core consults its script table as to whether an SS-Script is
     associated with the SS-Event it received from the SS-Front-end. If so,
     the associated script becomes the active SS-Script.

  5. The SS-Core processes the SS-Event in the context of the active
     SS-Script and the information that is stored in the Database.

  6. The SS-Core returns an SS-Action to the SS-Front-end, instructing it of
     the next action to take (i.e., continue, measurement-request, acquire,
     etc.).

  7. Based on the returned SS-Action from the SS-Core, the SS-Front-end
     instructs the SS-Client of the next SS-Action to take (i.e., continue,
     measurement-request, acquire, etc.).


EXTERNALIZED FUNCTIONS

  General:
    sd_init_before_app_task_start
    sd_init
    sd_term

  System Selection - General:
    sd_ss_act_get
    sd_ss_hdr_act_get
    sd_ss_log_last_action
    sd_ss_meas_list_get2
    sd_ss_meas_list_get
    sd_ss_meas_list_rprt

  System Selection Indications - User:
    sd_ss_ind_user_pwr_up
    sd_ss_ind_user_offline_cdma
    sd_ss_ind_user_offline_amps
    sd_ss_ind_user_pwr_save_exit
    sd_ss_ind_user_ss_pref
    sd_ss_ind_user_ss_pref2
    sd_ss_ind_user_nam_sel
    sd_ss_ind_user_pwr_down
    sd_ss_ind_user_get_net_gw
    sd_ss_ind_user_term_get_net_gw

  System Selection Indications - CDMA Acquisition:
    sd_ss_ind_cdma_acq_failed_pilot
    sd_ss_ind_cdma_acq_failed_sync
    sd_ss_ind_cdma_acq_failed_unlock_rf
    sd_ss_ind_cdma_acq_prot_mis
    sd_ss_ind_cdma_acq_schm

  System Selection Indications - CDMA Operation:
    sd_ss_ind_cdma_opr_oh_info
    sd_ss_ind_cdma_opr_oh_info2
    sd_ss_ind_cdma_opr_oh_info3
    sd_ss_ind_cdma_opr_rescan
    sd_ss_ind_cdma_opr_reg_rej
    sd_ss_ind_cdma_opr_sys_lost
    sd_ss_ind_cdma_opr_sys_lost2
    sd_ss_ind_cdma_opr_redir_cdma
    sd_ss_ind_cdma_opr_redir_amps
    sd_ss_ind_cdma_opr_redir
    sd_ss_ind_cdma_opr_acpt_rej
    sd_ss_ind_cdma_opr_ndss_off
    sd_ss_ind_cdma_opr_new_sys
    sd_ss_ind_cdma_opr_sys_resel
    sd_ss_ind_cdma_opr_lock
    sd_ss_ind_cdma_opr_unlock
    sd_ss_ind_cdma_opr_acc_term
    sd_ss_ind_cdma_opr_cam_err
    sd_ss_ind_cdma_opr_cam_cdma
    sd_ss_ind_cdma_opr_cam_amps
    sd_ss_ind_cdma_opr_hdm_cdma
    sd_ss_ind_cdma_opr_hdm_amps
    sd_ss_ind_cdma_opr_itspm
    sd_ss_ind_cdma_opr_release

  System Selection Indications - AMPS Acquisition:
    sd_ss_ind_amps_acq_failed_cch
    sd_ss_ind_amps_acq_cch_sid

  System Selection Indications - AMPS Operation:
    sd_ss_ind_amps_opr_failed_pch
    sd_ss_ind_amps_opr_pch_sid
    sd_ss_ind_amps_opr_acc_sid
    sd_ss_ind_amps_opr_rescan
    sd_ss_ind_amps_opr_sys_lost
    sd_ss_ind_amps_opr_cdma_avail
    sd_ss_ind_amps_opr_acc_term
    sd_ss_ind_amps_opr_cam
    sd_ss_ind_amps_opr_bs_in_ovld

  System Selection Indications - HDR User:
    sd_ss_ind_hdr_user_ss_pref
    sd_ss_ind_hdr_user_ss_pref2

  System Selection Indications - HDR Miscellaneous:
    sd_ss_ind_hdr_misc_timer
    sd_ss_ind_hdr_misc_meas_rprt
    sd_ss_ind_hdr_misc_pwr_save_enter

  System Selection Indications - HDR Acquisition:
    sd_ss_ind_hdr_acq_failed_pilot
    sd_ss_ind_hdr_acq_failed_sync
    sd_ss_ind_hdr_acq_prot_mis
    sd_ss_ind_hdr_acq_sid
    sd_ss_ind_hdr_acq_sid2

  System Selection Indications - HDR Operation:
    sd_ss_ind_hdr_opr_oh_info
    sd_ss_ind_hdr_opr_oh_info2
    sd_ss_ind_hdr_opr_oh_info3
    sd_ss_ind_hdr_opr_redir
    sd_ss_ind_hdr_opr_sys_lost
    sd_ss_ind_hdr_opr_sys_lost2
    sd_ss_ind_hdr_opr_resel_ok
    sd_ss_ind_hdr_opr_acc_term
    sd_ss_ind_hdr_opr_ses_abort
    sd_ss_ind_hdr_opr_ses_close
    sd_ss_ind_hdr_opr_ses_nego_timeout
    sd_ss_ind_hdr_opr_ses_opened
    sd_ss_ind_hdr_opr_conv
    sd_ss_ind_hdr_opr_release
    sd_ss_ind_hdr_opr_bad_sys
    sd_ss_ind_hdr_opr_start_activity
    sd_ss_ind_hdr_opr_stop_activity
    sd_ss_ind_hdr_opr_sys_usable

  System Selection Indications - GSM/WCDMA Acquisition:
    sd_ss_ind_gw_acq_plmn_failed
    sd_ss_ind_gw_acq_plmn_success

  System Selection Indications - GSM/WCDMA Operation:
    sd_ss_ind_gw_opr_srv_lost
    sd_ss_ind_gw_opr_srv_info
    sd_ss_ind_gw_opr_acc_done

  System Selection Indications - WLAN User:
    sd_ss_ind_wlan_user_ss_pref
    sd_ss_ind_wlan_user_prot_deactivate
    sd_ss_ind_wlan_user_get_networks
    sd_ss_ind_wlan_user_term_get_networks
    sd_ss_ind_wlan_user_pwr_save_exit

  System Selection Indications - WLAN Miscellaneous:
    sd_ss_ind_wlan_misc_timer
    sd_ss_ind_wlan_misc_survey_rpt
    sd_ss_ind_wlan_misc_pwr_save_enter

  System Selection Indications - WLAN Acquisition:
    sd_ss_ind_wlan_acq_scan_failed
    sd_ss_ind_wlan_acq_attach_failed
    sd_ss_ind_wlan_acq_attach_ok

  System Selection Indications - WLAN Operation:
    sd_ss_ind_wlan_opr_attach_ok
    sd_ss_ind_wlan_opr_ss_lost
    sd_ss_ind_wlan_opr_bss_dissociated

  System Selection Indications - Miscellaneous:
    sd_ss_ind_misc_timer
    sd_ss_ind_misc_meas_rprt
    sd_ss_ind_misc_orig_success
    sd_ss_ind_misc_pwr_save_enter
    sd_ss_ind_misc_cnf
    sd_ss_ind_misc_gw_net_list_rpt
    sd_ss_ind_misc_hdr_bcmcs_srv_status

  System Indicators:
    sd_si_reg
    sd_si_roam_update
    sd_si_info_ptr_get
    sd_si_uz_info_ptr_get

  Miscellaneous:
    sd_misc_prl_ver_get
    sd_misc_is_target_supp_mode_band
    sd_misc_is_target_supp_mode_band_chan
    sd_misc_is_supp_pref
    sd_misc_is_cdma_locked
    sd_misc_get_cdma_request_mode
    sd_misc_get_cdma_return_cause
    sd_misc_get_curr_acq_sys_mode
    sd_misc_is_mode_pref
    sd_misc_remove_mode_pref
    sd_misc_add_mode_pref
    sd_misc_is_hybr_opr
    sd_misc_get_supp_band_pref
    sd_misc_sys_id_print
    sd_misc_is_sys_same
    sd_misc_is_del_tmsi
    sd_misc_get_uoos_param
    sd_misc_is_orig_mode


REGIONAL FUNCTIONS

  System Selection:
    sd_ss_meas_is_done

  System Indicators:
    sd_si_srv_off
    sd_si_emerg_cb_mode
    sd_si_autonam_change


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Power-up:
    The initialization function sd_init_before_task_start must be called at
    power-up before any other SD function is called and before SD clients try
    to contact SD.

    After all tasks have started the SD initialization function sd_init() must
    be called to perform SD initialization that needs to be done after tasks
    have started.

    sd_init() returns TRUE if SD initialization is successful, in which case
    MS initialization should continue normally. sd_init() returns FALSE if SD
    initialization is not successful, in which case the MS is expected to go
    offline.

  Online:
    Upon receiving online command from CM, the SS-Client should call on
    sd_nam_sel() function. sd_nam_sel() returns TRUE if NAM selection is
    successful, in which case the SS-Client should call on
    sd_ss_ind_user_pwr_up() and follow the returned SS-Action. sd_nam_sel()
    returns FALSE if NAM selection is not successful, in which case the MS is
    expected to go offline.

  Offline:
    Upon a decision to go offline, the SS-Client should call on
    sd_ss_ind_user_offline_xxx().

  LPM:
    Upon entering LPM, the SS-Client should call
    on sd_ss_ind_user_pwr_down().

  Power-save:
    Upon entering Power-save (i.e., deep sleep), the SS-Client should call
    on sd_ss_ind_misc_pwr_save_enter().

  Power-off:
    Upon power-off, the SS-Client should call on sd_ss_ind_user_pwr_down()
    followed by sd_term(). After calling on sd_term(), no other public SD
    function shall be called prior to calling sd_init_before_task_start().

  Copyright (c) 2000 - 2009 by Qualcomm, Incorporated. All Rights Reserved.
  Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/sd/rel/08H1/src/sd.c#20 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
05/30/09   aj      Added FEATURE_C2K_TIME_INFO support.
05/27/09   aj      Adding support for updating clients with cdma local time
05/26/09   aj      Initialize is_wps_survey when issuing get networks to WLAN
05/15/09   mh      Update MCC/IMSI_11_12, regardless of is_mcc_sys_rec
04/12/09   sv      Reload HDR (GEO,PREF,COLLOC) lists before re-aligning HDR
04/09/09   aj      Added support for sd_misc_is_target_supp_mode_band_chan
03/31/09   mh      Added support for prl match indicator in ss info
03/26/09   mh      Update hdr_cand_idle_sys per hdr_cq_sys for hdr ovhd
03/09/09   rm      Adding wrapper function - sd_ss_get_supp_mode_pref
02/13/09   mh      Added support for default roaming indicator in ss info
02/13/09   sn      Constant GW UOOS scan time across band groups.
02/06/09   aj      WLAN subscription decoupling 
02/02/09   rk      Corrected the parameters order for sys_plmn_match call.
01/30/09   rk      Treating limited service ind on forbidden PLMN in 
                   manual mode as aquisition failure
11/28/08   rm      In UOOS report Limited service to NAS if sim state is
                   CS_PS_Invalid. Reverting part of the fix for CR-124247 
11/05/08   sv      Updated stack syncup to clear the stack sync-up progress flag
                   when the current pref_reason is not stack sync-up
10/23/08   ak      Fixing the no firmware scenario
09/29/08   rk      Featurizing band filtering logic. 
09/25/08   sv      Memory Reduction changes
09/18/08   rm      Adding acq_order_pref in sd_ss_act_get_net_gw_s_type.
09/15/08   pk      Added support for WLAN UOoS
09/15/08   rm      Get band and mode pref based on sdss_pref.true_mode_pref,
                   when GW service is unavailable.
09/04/08   sv      Added WPS API's for start and abort
                   Added Callback function for WPS result
08/20/08   st      Klockwork fixes
07/30/08   sv      Filter Stack Sync-up event when HDR call is being originated
07/24/08   sn      UOOS timer handling for both P2 and PWR_UP_ENH enabled.
07/13/08   sv      Optimise the use of List sorting.
06/11/08   fj      Added sd_misc_get_mode_band_capability
06/13/08   sn      Set UOOS WCDMA scan time as per ratio even for GSM only
                   mode and vice versa.
06/12/08   aj      Klocwork fixes
05/28/08   ak      Updated sd_ss_chan_list_update API interface to ensure
                   channel list update.
04/30/08   cl      Adding sd_misc_sys_avoid_list to list all the avoided 
                   systems. HDR/SCHR use this info to update their scan list
                   when doing OFS.
04/28/08   ak      Added sd_ss_ind_cdma_opr_prot_mis & ssscr_hdr_opr_prot_mis
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
04/07/08   cl      Update Function's Description sd_misc_is_fts_mode()
03/19/08   cl      Fixing UOOS Time to support Auto Mode Builds.
03/17/08   vs      Added is_assn_tag_changed, is_geo_count_changed and
                   is_geo_changed flag's to check if HDR needs to be 
                   re-aligned
03/14/08   rm      CM-NAS interface for new service req type
03/22/08   ic      Undo of "Replace custmmode.h with modem_mmode_cust.h and 
                   FEATURE_MMODE_CUSTMMODE_H with FEATURE_MODEM_MMODE_CUST_H"
03/18/08   ic      Replace custmmode.h with custmmode.h
                   Replace FEATURE_MMODE_CUSTMMODE_H with FEATURE_CUSTMMODE_H   
03/08/08   ic      Use FEATURE_MMODE_CUSTMMODE_H instead of FEATURE_MM_CUSTMM_H
02/19/08   cl      Optimize the execution time of SD API sd_misc_is_fts_mode
02/21/08   cl      Added SD API for sd_misc_is_fts_mode to determine if the
                   phone is in FULL_TIME_SHDR Mode or not.
02/20/08   vs      Made changes to clear SDSR_PREV_COLLOC_LST
                   and reset realign-hdr when camping on GW system
01/17/08   pk/vs   Added support for non-Hybrid UOOS

01/22/08   ak      Update MRU table in JCDMA config if GSM enabled.
01/22/08   rm      Reverted the changes done for- support for SIM Invalid
                   and UOOS
01/04/08   ka      New API for informing Daylight savings and LTM Offset as
                   part of schm.
09/17/07   pk      Added support for non-Hybrid UOOS
09/11/07   pk      Added support for FEATURE_DISABLE_DEEP_SLEEP
09/06/07   cl      KDDI request not to restart the TMR_HOLD_RESEL when we exit
                   session related traffic.
08/30/07   cl      Added CM critical section support. Also replaced INTLOCK /
                   INTFREE with rex_enter_crit_sect() / rex_leave_crit_sect()
08/29/07   sk      Fixed error in MOST_PREF classification of a system to
                   ignore systems that shall not be acquired due to user pref
                   or target capability.
08/21/07   ic      Added checks for custmm.h being used
07/24/07   cl      Mainline FEATURE_INT_ROAMING and FEATURE_2ND_PRL
06/15/07   rp      Adding support for rem_uoos_sleep_time and rem_uoos_awake_time
06/15/07   sk      Zeroing out unused fields.
06/11/07   sk      Adding support for passing netlist system record index
                   for WLAN acquisition and measurement.
06/07/07   sk/cl   Limited orig start filtering in MAIN instance to
                   non-powersave states if the prefs didn't change.
06/06/07   pk      UOOS enhancements...
05/25/07   pk      Added support for SIM Invalid and UOOS cont...
05/14/07   pk      Enabling UMTS UOOS ENH and P2 features simultaneously
05/03/07   jqi     DO not shorten the reselection timer after call release
                   if the expected SID/NID of redirected system is wildcard.
05/02/07   jqi     DO not turn off the redirection afer IHO if the expected
                   SID/NID of the redirected system is WILDCARD.
                   Expanded 1x system reselection timer value from 16-bit to
                   32-bit.
04/23/07   jqi     Added Filtering support for stopping HDR activity.
                   Added support for sd_misc_is_orig_mode.
04/17/07   rp      Adding check for NULL pointer
04/15/07   rp      Updated the  net_sel_mode field in srv_sys
03/28/07   sk      Added new GW UOOS wakeup time for GCF fix.
02/26/07   pk      Added sdss_get_ss_for_mode for all featurizations
02/15/07   jqi     Added support for sd_ss_ind_cdma_opr_oh_info3
01/04/07   pk      Added support for VoIP -> 1X Handoffs
12/01/06   pk      Added support for Automatic mode for CDMA + GSM UOOS
                   builds
12/03/06   ka      Correcting sd_ss_meas_list_get2() to return all GW
                   bands required for measurement.
11/29/06   ka      Limited service resel timer expiry results in sending
                   MEAS_BACK action instead of full service req. Satisfies
                   GCF 6.1.2.6.
11/17/06   jqi     Added support for HDR active protocol revision information.
11/15/06   pk      Added support for HYBR_GW
11/02/06   pk      Added support for SD_LOG_STATE_PWR_SAVE
09/17/06   sk      Store the newly passed band info for WLAN manual system.
09/11/06   rp      Added code to copy wlan_tech_pref from sr_ptr to act_ptr.
09/08/06   sk      Added new APIs to support WLAN measurement.
09/05/06   pk      FEATURE_UMTS_UNIFORM_OOS_HANDLING, added support to
                   query UOOS timers
08/29/06   sk      Added WLAN Adhoc support.
08/25/06   pk      Added support for sd_ss_ind_gw_acq_plmn_failed2
08/24/06   pk      Added support for IF construct and GCF test fixes
08/24/06   pk      Added support for sdss_nv_init
08/23/06   jqi     Added support for system reselection during BCMCS flow.
08/16/06   pk      Added support for GW UOOS
08/14/06   jqi     Added the system lost reason for 1x system exit events.
08/07/06   pk      Added support for 1X UOOS
07/14/06   pk      Modified the computation of is_colloc_changed flag when 1X
                   acquires service
07/13/06   jqi     Added the system lost reason for HDR system lost event.
07/07/06   sk      Added support for logging GW system into MRU when moving
                   from limited system to system with service.
06/14/06   jqi     Do not declare the redirection is invalid if there is zero
                   channel in the redirection recs.
06/09/06   jqi     Modified the comments.
05/23/06   jqi     Always forced HDR usable flag as usable for JCDMA build.
                   Fixed strip errors.
05/22/06   jqi     Made use of the chan list for HDR oh info.
05/12/06   sk      Fixed bug in WLAN API using/modifying non-WLAN lists.
05/09/06   jqi     Fixed the compilation errors caused by EJECT.
05/08/06   sk      Added support for expected SID for HDR.
05/04/06   jqi     Lint fixes.
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
04/06/06   sk      BCMCS can be turned off implicitly only if CDMA is
                   acquired. All other cases of implicit turning off of BCMCS
                   removed.
04/01/06   ka      Adding plmn list status to network list cnf.
03/31/05   pk      Modified sd_si_ok_to_orig() interface. Added data_suspend
                   flag as one more parameter
                   Modified sd_ss_ind_gw_opr_srv_info() and
                   sd_ss_ind_gw_acq_plmn_success to update the data
                   suspend flag as the last step of processing. The behavior
                   was changed to fix CR 59670.
03/27/06   jqi     Lint fixes.
03/14/06   jqi     Counting num of fades per HDR IDLE system.
03/9/06    jqi     DO system reselection optimization(Sys resl at DO MO call
                   end.)
                   Multichannel avoidance.
                   Reset the DO failure counter when Subnet ID is changed.
                   Do not change sd state if only srv domain is changed.
03/06/06   jqi     Added comments for bad system.
02/08/06   pk      Replaced SD_ERR with SD_MSG_ERROR
01/12/06   pk      Added sd_misc_is_sys_same
12/22/05   jqi     Merged background PLMN search support.
12/07/05   ka      Adding sd_ss_ind_gw_data_suspended() to inform CM about
                   data suspend.
12/02/05   jqi     Lint fixes for lint ver8.
11/23/05   jqi     Mainline FEATURE_HICPS20 and FEATURE_HICPS30 under
                   FEATURE_HDR.
11/08/05   jqi     Added support for 1x acquisition throttling for BCMCS
                   activity.
11/07/05   jqi     Used hybr pref from main instance.
11/02/05   sk      Fixed all Lint and compiler warnings.
10/18/05   pk      Added sd_misc_get_common_mode_pref
10/05/05   sk      Added support for configuring SD values from NV.
09/16/05   jqi     Fixed the errors for CDMA_WLAN configuration.
09/10/05   pk      Added srv_acq_time to sd_si_info_s_type
09/01/05   jqi     Fixed the race condition where SD return continue for the
                   system lost event.
                   Ignore the release event when SD is in acquisition state
                   and return continue.
8/15/05   pk       Modified the use of realign HDR flag while reporting
                   service to CM. If the collocated lists didn't change the
                   re align flag is not set.
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
07/18/05   dk      DeCouple WLAN dependency from HDR_HYBRID.
07/11/05   jqi     Mainlined FEATURE_BCMCS.
07/11/05   jqi     Added avoidance timer for access failures and fades.
07/08/05   sk      Added 2 new APIs:
                     - sd_ss_ind_hdr_opr_ses_nego_timeout
                     - sd_ss_ind_hdr_opr_ses_opened
                   Added SDSS_EVT_HDR_OPR_SES_NEGO_TIMEOUT to
                   sd_ss_return_cause_update().
                   Reset event counters on SID change.
06/26/05   ic      Lint and compiler warnings cleanup
06/17/05   sk      Removed previous changes.
06/17/05   sk      Updated sd_ss_ind_hdr_opr_ses_abort() to call
                   sdss_acc_update()
06/15/05   jqi     Return SD action continue for SD_SS_ACC_TERM_NORMAL for
                   HDR operation.
06/06/05   jqi     Added debug message for HDR access term report.
06/06/05   pk      Modified sd_ss_ind_hdr_user_ss_pref(),
                   modified the check for service status for Powersave, only
                   for redial normal.
                   Lint Clean up.
05/25/05   dk      Initialize WLAN SS state to ACQ during sd_nam_sel.
05/20/05   ka      Adding support for Limited Manual network sel type
                   to possibly camp with full service on Manually selected
                   PLMN.
05/13/05   ka      API to avoid a bad WLAN system
05/04/05   jqi     Fixed act and true act out of sync issue on GPS
                   activities.
                   Set redirect system with acq system during redirection
                   process
                   Provided the API to return supported band preference
                   according to target capability and user mode/band pref.
                   Added support to process HDR bad system event while in
                   acquisition state.
04/29/05   pk      Added sd_misc_sys_id_print() to print the sys_id
03/28/05   RC      Rebuilding COLLOC_LST upon ITSPM - CR59458
03/11/05   pk      Modified sd_ss_ind_hdr_user_ss_pref(),
                   if SS-Preferences are not changed, we return
                   SD_SS_ACT_CONTINUE for REDIAL_NORM.
02/24/05   pk      Fixed some lint errors
01/30/05   sj      Added support for WLAN.
01/10/05   dk      Updated sd_si_update() to report service change during
                   LAC changes when in GW mode.
01/07/05   sj      Fixed issue with setting reselect flag during auto->auto.
01/06/04   jqi     Lint fixes.
01/06/05   sj      Set srv_status to NO_SRV even if it is currently PWR_SAVE.
12/04/04   jqi     Power save mode optimization for GSM/UMTS targets.
11/24/04   sj      Added HDR acc_term in event report.
11/16/04   sj      Avoid system will place sid/nid lock.
10/21/04   RC      More enhancements related to FEATURE_INT_ROAMING
11/02/04   jqi     Added API to validate the mode preference and band class
                   against the target capability
10/26/04   jqi     Read subscription items only when the subscription is
                   available.
10/25/04   sj      Added bad_sys SD SS HDR indication.
10/21/04   RC      Update the SID of the serving system per the reported SID
                   in sd_ss_ind_hdr_opr_oh_info().
10/08/04   sj      Fixed issue with lists mapping in hybrid mode when hdr timer
                   expires after subscription available.
10/04/04   jqi     Valid band and mode againt both target and PRL for task
                   initialization and system pref change command.
09/15/04   sj      Fixed issue of SS-HDR returning ACQ_HDR if hybr_pref
                   is turned off before the instance is kicked.
09/08/04   jqi     Added ps dormant optimization.
09/08/04   sj      Update subscription status on SS-HDR also.
09/08/04   sj      Do not map hdr timer expiry to SS-Main.
08/26/04   jqi     Lint fix.
08/20/04   sj      Added support for Auto A/B.
08/16/04   sj      For user_ss_pref2(), if SS is in pwr_save, don't optimise
                   on pref_update().
08/09/04   sj      Updated call to sdprl_nam_sel().
08/06/04   sj      Changed sd_ss_*_act_get() to return the action other than
                   CONTINUE.
08/03/04   sj      Converts user_nam_sel to user_pwr_up if not already in
                   online.
08/02/04   sj      Backed out Featurize baseid/lat/long/srch_win_n in ss_info.
07/30/04   jqi     Featureize baseid/lat/long/srch_win_n in ss_info.
                   Qualband optimization - eliminate scheduling.
07/26/04   sj      Changes for camped_ind support.
07/23/04   sj      Added support for baseid/lat/long/srch_win_n in ss_info.
07/15/04   sj      Updated orig_start handling for orig_start_ps &
                   orig_start_cs.
07/14/04   sj      Changed calls to update_srv_status to include srv_domain.
07/09/04   sj      Do not filter the first pref_sys changed due to ORIG_START.
                   Simplified filtering supported mode/band preferences.
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
06/07/04   jqi     More quad-band enhancement for automatic mode.
05/26/04   jqi     Revised support for CM_CAMPED_IND received from REG that
                   happens when phone is camped without completing a location
                   updates.
05/25/04   jqi     Fixed mode preference reset issue after power up.
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
           ka      Added support for CM_CAMPED_IND received from REG that happens
                   when phone is camped without completing a Loctaion Update.
05/17/04   sj      Updated support for BCMCS.
04/21/04   jqi     Completed two SS-action comparison for ACT GW.
                   Added band and mode message for gw acq sucess and srv info.
04/19/04   jqi     Lint fix - for error level : error.
04/13/04   sj      Added new SD indications for HDR conversation and release.
                   Reset acquisition throttle counters upon getting overhead
                   on 1x.
04/09/04   jqi     Filter unsupported bands prior to filter overlapped bands.
03/18/04   jqi     Introduced the true act type in sd_ss_s_type.
                   Kept band and mode consistent before passing to the GW
                   stack.
                   lint fix.
03/12/04   sj      Added debug messages for user_ss_pref2().
           jqi     Use service domain form service indicator for call orig.
03/10/04   jqi     Propagated service domain for call origination.
02/18/04   JQ      Complied band pref with MS Radio Access capability before
                   passing down to the stack.
02/16/04   SJ      Removed casting in the call to get_acc_prm().
02/06/04   SJ      If origination mode is MANUAL, and the script is requesting
                   limited service, set net_sel_type = LIMITED, to avoid
                   the UMTS/GSM stack to acquire full service on non Manual
                   PLMNs.
02/04/04   SJ      Added flag in acq_gw to indicate if orig is pending.
02/02/03   RC      Added a call to sd_si_ok_to_orig( SD_SS_MAIN ) in
                   sd_ss_ind_misc_orig_success().
02/01/04   SJ      Fixed issue wherein hybrid lists are not switched
                   correctly after OTASP.
01/27/04   SJ      Fixed issue with current acq. system when there is no SIM.
01/23/04   JQ      Passed down most recent acquired band group to GW stack
                   during early power scan.
01/22/04   JQ      Changed SDSR_GW_LTD_SYS and SDSR_USER_SYS to lists with
                   appropriate grouping of GW bands.
12/12/03   RC      Added support for FEATURE_INT_ROAMING
12/08/03   SJ      Fixed issue with lists switch upon transition from
                   hybrid -> non-hybrid when 1x was not acquired in hybrid
                   mode.
11/17/03   SJ      Updates for HDR usability and new hdr power save.
11/14/03   JQ      Fixes for IS683D support.
11/12/03   JQ      Use manual sel mode specified with undef plmn for power up
                   manual orig mode.
11/05/03   JQ      Do not error fatal while validating PRL.
                   More MRU3 updates.
10/31/03   RC      Added GSM/WCDMA support for fall-back to limited service.
10/31/03   JQ      Added support for latest IS-683D format changes.
10/24/03   RC      Changed SDSR_GW_LST to contain a single system of mode
                   SD_MODE_GW.
10/14/03   SJ      Added function sd_misc_add_mode_pref.
10/09/03   jqi     Adjusted band preference enum.
09/28/03   SJ      Added functions to determine if we are in hybrid opr,
                   sd_misc_is_hybr_opr().
09/05/03   SJ      Added functions sd_misc_is_mode_pref &
                   sd_misc_remove_mode_pref.
08/26/03   SJ      Added mode to ok_to_orig cb.
                   Added pwr save reason in act_pwr_save SS Action.
08/20/03   SJ      Added new SS-Indication sd_ss_ind_misc_cnf.
08/18/03   RC      Fixed issues with IS-683D implementation.
07/18/03   JQ      Added Extended PRL support
                   - D PRL
                   - Auto detection.
07/17/03   SJ      Added SI call back to inform clients registration was
                   done succesfully.
07/14/03   SJ      In sd_misc_get_curr_acq_sys_mode() use the mode_pref
                   if SD is not attempting acqusitions.
06/24/03   RC      Call sdsr_component_init() in sd_init().
06/23/03   RC      Enhanced HDR wildcard subnet-ID behavior.
06/10/03   RC      Added counter to track CDMA available indications.
06/09/03   RC      Consult subnet_mask_len in sd_ss_ind_hdr_opr_oh_info()
06/05/03   RC      Modify sd_ss_idle_sys_update() to update the idle system
                   when doing channel assignment (CAM) to CDMA paging.
05/23/03   RC      Fixed ss_timer setup to not cause too frequent expiration.
05/22/03   RC      In sd_ss_ind_cdma_opr_oh_info() handling band change same
                   as a SID/NID change.
04/28/03   RC      Added support for event logging.
04/23/03   SJ      Added early power scan support for GSM/WCDMA
04/04/03   SJ      Acquire limited service if SIM was invalidated.
03/04/03   SJ      Added support for limited automatic mode using API
                   sd_misc_get_curr_acq_sys_mode.
02/26/03   SJ      Added support for Band Class 8 & 9.
02/24/03   SJ      Turn service indicators off when pwr_down indication is
                   called.
02/06/03   SJ      Adjust mode_pref in acq_gw action according to phone
                   capabilities.
02/05/03   SJ      Fixed the issues with manual WCDMA plmn selection.
01/22/03   SJ      Fixed the issue with GSM/WCDMA service indicator.
01/16/03   SJ      Featurized dependency on DB & CAI for non 1x targets.
                   Featurized SS-Indications for CDMA.
12/09/02   RC      Added support for FEATURE_JCDMA.
12/03/02   SJ      Fixed merge issue with hybrid operation.
11/07/02   RC      Added support for new hybrid-HDR power-save algorithm.
10/15/02   RC      Copy PRL association fields over in OH_INFO, CAM and HDM.
09/03/02   RC      Removed optimization in sd_ss_ind_hdr_user_ss_pref() to
                   return CONTINUE if no SS-Preference is changed.
07/31/02   RC      Optimized GPS SS-Preference related changes.
07/19/02   RC      In sd_ss_ind_hdr_opr_oh_info() call on
                   sd_ss_hdr_event_proc() if doing CDMA+HDR hybrid operation.
05/17/02   RC      Added support for IS-683C, PN and data association fields.
03/08/02   hxw     Added GPS mode support for MSBased gpsOne.
02/08/02   mpa     Added support for logging.
02/06/02   RC      Eliminated compiler warnings.
01/25/02   sj      Added support for Tiered Services - User Zone feature.
01/23/02   RC      Added support for Rel. 3.0 CDMA-HDR hybrid operation.
12/12/01   RC      Added support for CDMA-HDR hybrid operation.
12/11/01   sj      Added "sd_si_roam_update" to set the roam indication on
                   serving system.
12/10/01   RC      Ignore HDR directed redirection if it is reported before
                   SD decided to stay on the system.
10/30/01   RC      Added support for IS-683C collcated systems.
10/03/01   RC      Treat sd_ss_ind_hdr_opr_ses_abort() as sync acquisition
                   failure if it received during HDR acquisition.
09/07/01   RC      Added support for T53 and JCDMA features.
07/20/01   RC      Added support for IS-683C.
04/12/01   RC      Changes to support HDR subnet ID.
03/27/01   RC      Changes to support tri-mode targets.
03/09/01   RC      Changes per 11th week of I&T.
02/07/01   RC      Changes per 9th week of I&T.
02/01/01   RC      Changes per 8th week of I&T.
01/12/01   RC      Changes per 7th week of I&T.
01/12/01   RC      Changes per 6th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
12/14/00   RC      Changes per 3rd week of I&T.
12/04/00   RC      Changes per 2nd week of I&T.
11/27/00   RC      Added a channel strength measurement done indicator.
11/17/00   RC      Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h" /* Customer configuration file */
#ifndef FEATURE_CUSTMMODE_H
#error  Need to pull in custmmode.h!!!
#endif

#include "sd.h"       /* External interface to sd.c */
#include "sdi.h"      /* Internal interface to sd.c */
#include "sdss.h"     /* External interface to sdss.c */
#include "sdsr.h"     /* External interface to sdsr.c (System Record)*/
#include "sdprl.h"    /* External interface to sdprl.c */
#include "sdnv.h"     /* External interface to sdnv.c */
#include "sddbg.h"    /* SD debug macros */
#include "sdlog.h"    /* SD20 logging */
#include "prl.h"      /* External interface to prl.c */
#include "sdssscr.h"  /* External interface to sdssscr.c */

#include "comdef.h"   /* Definition for basic types and macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */
#include "event.h"    /* Event reporting services */

#include "clk.h"      /* Interface to clock services */
#include "nv.h"       /* Interface to NV services */
#ifdef SD_USES_CAI
#include "caii.h"     /* Internal IS-95 protocol data declaration */
#endif
#ifdef SD_USES_DB_SERVICES
#include "db.h"       /* Interface to database services */
#endif

/*lint -esym(528, sd_si_update_if_srv_is_off) */ /* func not referenced */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* This set of features must be defined for SD 2.0
*/

#if( defined(FEATURE_HDR_HYBRID) && !defined(FEATURE_HDR) )
#error code not present
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Dependencies for FEATURE_USERZONE
*/
#if defined(FEATURE_USERZONE) && ( (!defined(FEATURE_SD20))  || \
                                   (!defined(FEATURE_IS2000)) || \
                                   (!defined(FEATURE_EFS)))
#error  "User Zone requires SD20, IS2000  & EFS features to be defined"
#endif


/* <EJECT> */
/*---------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
---------------------------- SYSTEM SELECTION -------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/* Type for holding the state variables that are associated with the system
** selection front-end.
*/
typedef struct {

  /* SS-Action buffer - records the SS-Action that was most recently returned
  ** to the SS-Client.
  */
  sd_ss_act_s_type        act;
  sd_ss_act_s_type        prev_act;
  sd_ss_act_s_type        true_curr_act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-State indicator - indicates the current SS-State.
  */
  sdss_state_e_type       ss_state;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Acquisition list, position and mode - indicates the list and position
  ** most recently attempted acquisition, as well as, the acquisition mode.
  */
  sdsr_e_type             acq_list;
  int                     acq_pos;
  sdss_acq_mode_e_type    acq_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Measurement ID - indicate the ID of the most recent channel strength
  ** measurement request. This ID is being incremented every time a channel
  ** strength measurement is submitted to Search. It is being consulted upon
  ** receipt of the measurement report to determine the validity of the
  ** report.
  */
  word                    meas_id;

  /* Measurement is done - indicate whether the most recent measurement
  ** request (i.e., the one that is identified by meas_id) is done being
  ** serviced by Search.
  */
  boolean                 meas_is_done;

  /* Measurement list is different - indicate whether the most recent
  ** requested measurement list is different than the previous requested
  ** measurement list.
  */
  boolean                 meas_list_is_diff;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Indicates if the get networks request was serviced or not.
  */
  boolean                 is_get_net_done;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Indicates if the WLAN survey request was serviced or not.
  */
  boolean                 is_wlan_survey_done;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Pointer to an SS-Timer function.
  **
  ** SD calls on this function to set/disable the SS-Timer. Upon SS-Timer
  ** expiration the SS-Client should call on the sd_ss_ind_misc_timer()
  ** SS-Indication function.
  **
  ** The argument to this function is the time in ms with which to set the
  ** SS-Timer. An input of 0 should disable the SS-Timer.
  */
  void                    (*timer_func_ptr) (int4  ms);

  /* SS-Timer - indicates the uptime in seconds when the SS-Timer is set to
  ** expire, at which time the SS-Client is responsible for calling on the
  ** sd_ss_ind_misc_timer() SS-Indication function.
  */
  dword                   ss_timer;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ///* Bad point update uptime - indicates the uptime in seconds when the bad
  //** point count was last updated.
  //*/
  //dword                   bad_point_update_uptime;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return cause - indicate the RETURN_CAUSE value shown in IS-2000-5 Table
  ** 2.7.1.3.2.1-2 corresponding to the service redirection failure
  ** condition.
  */
  byte                    return_cause;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Optimized GPS SS-Preference change flag - idicate whether optimized GPS
  ** SS-preference change was performed.
  */
  boolean                 gps_is_opti_ss_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Critical section lock for SS component, to detect SS-Indications from
  ** multiple tasks being processed simultaneously.
  */
  boolean                 is_ss_locked;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Flag to start SD instance
  ** Reset: power up and after SD instance has been started.
  ** Set : when nam is changed or when hybrid preference is
  ** toggled from none->on.
  */
  boolean                 is_start_instance;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate the activity bit mask
  */
  sd_ss_hdr_act_mask_e_type   hdr_act_mask;

} sd_ss_s_type;





/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macros to lock the System selection component and unlock it.
*/
#define SD_SS_LOCK( ss_ptr )                                               \
                          sd_rex_enter_crit_sect();                        \
                          if ( ss_ptr->is_ss_locked)                       \
                          {                                                \
                            SD_ERR_FATAL("SS already locked", 0, 0, 0 );   \
                          }                                                \
                          else                                             \
                          {                                                \
                            ss_ptr->is_ss_locked = TRUE;                   \
                          }                                                \
                          sd_rex_leave_crit_sect();
#define SD_SS_FREE( ss_ptr )  ( ss_ptr->is_ss_locked = FALSE )             \

/* <EJECT> */
/*---------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
---------------------------- SERVICE INDICATOR ------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/* Type for holding the information that is associated with the SD Service
** Indicator component.
*/
typedef struct {

  /* Service Indicators Information buffer - Holds the information of the
  ** various service indicators.
  */
  sd_si_info_s_type             si_info;


  /* Service update criteria - Indicate the minimum criteria for updating
  ** the service indicators.
  */
  sd_si_update_e_type           si_update_criteria;


  /* Pointer to associated system selection item.
  */
  sd_ss_s_type                  *ss_ptr;


  /* Associated service indicator system.
  */
  sdsr_e_type                   si_sys;

  /* Flag to indicate the data suspend state reported from REG.
  */
  boolean                       data_suspend;

  /* Pointer to a Service Indicators callback function. SD calls on
  ** this function whenever one or more of the service indicators
  ** changes their value.
  **
  ** The argument to this function is a const pointer to a
  ** structure containing the current value of the service
  ** indicators.
  */
  sd_si_info_f_type             *si_info_cb_func;


  /* Pointer to an acq_fail callback function. SD calls on this function
  ** whenever acquisition fails (i.e., SD have attempted full acquisitions on
  ** all systems with no success),
  */
  sd_si_acq_fail_f_type         *acq_fail_cb_func;


  /* Pointer to an ok_to_orig callback function. SD calls on this
  ** function whenever ok_to_orig status changes from FALSE to
  ** TRUE while the origination mode is other than none.
  */
  sd_si_ok_to_orig_f_type       *ok_to_orig_cb_func;


  /* Pointer to a emergency callback mode function. SD calls on this function
  ** whenever the emergency callback mode status changes from FALSE to
  ** TRUE.
  */
  sd_si_emerg_cb_mode_f_type    *emerg_cb_mode_cb_func;


  /* Pointer to an auto-NAM change function. SD calls on this function
  ** whenever auto-NAM change is appropriate.
  */
  sd_si_autonam_change_f_type   *autonam_change_cb_func;

  /* Pointer to the GW network list call back function. SD calls on
  ** this function once it receives the results of the get network
  ** list on GSM/WCDMA systems.
  */
  sd_si_gw_net_list_f_type      *gw_net_list_cb_func;


  /* Pointer to the SIM state change call back function. SD calls this
  ** function when there is a change in SIM state.
  */
  sd_si_sim_state_f_type        *sim_state_cb_func;


  /* Pointer to the registration completed call back function. SD calls
  ** this when registration( including implicit) is done successfully.
  */
  sd_si_reg_done_f_type         *reg_done_cb_func;


  /* Pointer to the data suspend flag call back function. SD calls
  ** this when REG reports a change in data suspend flag.
  */
  sd_si_data_suspend_f_type     *data_suspend_cb_func;

  /* Pointer to the WLAN network list call back function. SD calls on
  ** this function once it receives the results of the get networkA
  ** list on WLAN systems.
  */
  sd_si_wlan_net_list_f_type    *wlan_net_list_cb_func;

  /* Pointer to the WLAN network list call back function. SD calls on
  ** this function once it receives the results of the get networkA
  ** list on WLAN systems.
  */
  sd_si_wlan_wps_result_f_type  *wlan_wps_result_cb_func;

  /**< Pointer to the service lost call function. SD calls on
  ** this function once the service lost status is changed
  */
  sd_si_srv_lost_f_type         *srv_lost_cb_func;


} sd_si_s_type;




/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================
=============================================================================
=============================================================================
=========================== FORWARD DECLARATIONS ============================
=============================================================================
=============================================================================
===========================================================================*/

static  sd_ss_act_e_type          sd_ss_event_proc(

        sd_ss_e_type              ss,
            /* The ss to use for the event
            */


        sdss_evt_e_type           event,
            /* SS-Event to process.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================ SERVICE INDICATORS =============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sd_si_ptr

DESCRIPTION
  Return the pointer to Service indicator component.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_si_s_type*             sd_si_ptr(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */

)
{
  /* Service Indicators item.
  */
  static sd_si_s_type  sd_si[SD_SS_MAX];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ss < SD_SS_MAX );

  return &(sd_si[ss]);

} /* sd_si_ptr() */

/* <EJECT> */
/*===========================================================================

FUNCTION sd_is_same_sid

DESCRIPTION
  Check whether the system IDs are equal.
  For WLAN, get the SSID from sid_expected.


DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system IDs are equal. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sd_is_same_sid(

        const sd_mode_e_type      mode,
            /* Mode of the sid_ptr.
            */

        const sd_sid_u_type       *sid_ptr,
            /* Pointer to a system Id.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr1_ptr.
            */
)
{
  #ifdef FEATURE_HDR
#error code not present
  #endif /* FEATURE_HDR */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sid_ptr != NULL );
  SD_ASSERT( sr2_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Compare the two SIDs base on the system's mode.
  */
  switch( sr2_ptr->sys.mode )
  {
    case SD_MODE_AMPS:
    case SD_MODE_CDMA:

      if ( mode != sr2_ptr->sys.mode )
      {
        return FALSE;
      }

      if( sid_ptr->is95.sid != sr2_ptr->sid.is95.sid ||
          sid_ptr->is95.nid != sr2_ptr->sid.is95.nid )
      {
        return FALSE;
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
      if ( mode != SD_MODE_GSM &&
           mode != SD_MODE_WCDMA &&
           mode != SD_MODE_GW
         )
      {
        return FALSE;
      }

      return sys_plmn_match( sid_ptr->plmn.plmn_id,
                             sr2_ptr->sid.plmn.plmn_id );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_WLAN:

      if ( mode != sr2_ptr->sys.mode )
      {
        return FALSE;
      }

      return sys_is_wlan_ssid_match( &sid_ptr->ssid,
                                     &sr2_ptr->sid_expected.prm.wlan.ssid
                                   );


   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_GPS:
    case SD_MODE_MAX:
    default:
      return FALSE;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the two SIDs match - return TRUE.
  */
  return TRUE;

} /* sd_is_same_sid */

#if defined(FEATURE_GW) || defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
/*===========================================================================

FUNCTION sd_is_same_gw_lac

DESCRIPTION
  Check whether the LAC of two GW system IDs of two system records are equal.


DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the LACs are equal. FALSE otherwise.
  This also return TRUE if we are comparing non-GW systems.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sd_is_same_gw_lac(


        const sd_mode_e_type      mode,
            /* Mode of the sid_ptr.
            */

        const sd_sid_u_type       *sid_ptr,
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr1_ptr.
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sid_ptr != 0 );
  SD_ASSERT( sr2_ptr != 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if both the modes is GW.
  */
  if( (sr2_ptr->sys.mode == SD_MODE_GSM   ||
      sr2_ptr->sys.mode == SD_MODE_WCDMA ||
      sr2_ptr->sys.mode == SD_MODE_GW ) &&
      ( mode == SD_MODE_GSM   ||
        mode == SD_MODE_WCDMA ||
        mode == SD_MODE_GW )
    )
  {
     if( sid_ptr->plmn.lac != sr2_ptr->sid.plmn.lac )
     {
       return FALSE;
     }
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

}
#endif /* FEATURE_GW || FEATURE_GSM || FEATURE_WCDMA */


/*===========================================================================

FUNCTION sd_si_init

DESCRIPTION
  Initialize SD Service Indicators component.

  NOTE! This function must be called before any other sd_si_xxx() function is
  called. This function shall not be called a second time prior to calling on
  sd_term().

DEPENDENCIES
  This function must be called before any other sd_si_xxx() function is ever
  called. This function shall not be called a second time prior to calling on
  sd_term().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_si_init(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sd_ss_s_type              *ss_ptr,
            /* Pointer to associated system selection item.
            */

        sdsr_e_type               si_sys
            /* Associated service indicator system.
            */
)
{
  sd_si_s_type          *si_ptr       = sd_si_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT( ss_ptr != NULL );

  #if (defined (FEATURE_TECH_IND_SD_STACK_ARCH) && defined(FEATURE_HDR_HYBRID) && defined(FEATURE_WLAN))
#error code not present
  #elif defined (FEATURE_TECH_IND_SD_STACK_ARCH)
  SD_ASSERT( si_sys == SDSR_SI_SYS || si_sys == SDSR_HYBR_1_SI_SYS);
  #elif (defined(FEATURE_HDR_HYBRID) && defined(FEATURE_WLAN))
#error code not present
  #elif defined(FEATURE_HDR_HYBRID)
#error code not present
  #elif defined(FEATURE_WLAN)
#error code not present
  #else
  SD_ASSERT( si_sys == SDSR_SI_SYS );
  #endif

  SD_FUNC_START( "sd_si_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system selection item pointer.
  */
  si_ptr->ss_ptr = ss_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the service indicators system.
  */
  si_ptr->si_sys = si_sys;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize data suspend flag to FALSE.
  */
  si_ptr->data_suspend = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize callback functions to NULL.
  */
  si_ptr->si_info_cb_func         = NULL;
  si_ptr->acq_fail_cb_func        = NULL;
  si_ptr->ok_to_orig_cb_func      = NULL;
  si_ptr->emerg_cb_mode_cb_func   = NULL;
  si_ptr->autonam_change_cb_func  = NULL;
  si_ptr->gw_net_list_cb_func     = NULL;
  si_ptr->sim_state_cb_func       = NULL;
  si_ptr->reg_done_cb_func        = NULL;
  si_ptr->data_suspend_cb_func    = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the mode to INACT on powerup.
  */
  si_ptr->si_info.mode = SD_MODE_INACT;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_USERZONE
  /* Initialize currently selected User zone to invalid
  */
  si_ptr->si_info.curr_uz_info.uz_id = SD_INVALID_UZID;
  #endif /* FEATURE_USERZONE */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the usable status to usable by default.
  */
  si_ptr->si_info.usable_status = SD_SYS_USABLE_OK;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the service uptime to default
  */
  si_ptr->si_info.srv_acq_time = 0;

  /* Initialize the service lost uptime to default
  */
  si_ptr->si_info.srv_lost_time = 0;

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif

  /* Initialize service to OFF. The service update condition is successful
  ** acquisition.
  */
  sd_si_srv_off_until( ss, SD_SI_UPDATE_ACQ );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the HDR active prot revision
  */
  si_ptr->si_info.hdr_active_prot = SYS_ACTIVE_PROT_NONE;

  /* Initialize the sys_mode in time info. if sys_mode has this value
  ** it means that no time info is available
  */
  si_ptr->si_info.time_info.sys_mode = SYS_SYS_MODE_NO_SRV;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the prl match flag
  */  
  si_ptr->si_info.is_sys_prl_match = FALSE;

} /* sd_si_init */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_reg

DESCRIPTION
  Register callback functions with the Service Indicators component.

  SD calls on the Service Indicators callback function whenever one or more
  of the service indicators changes their value.

  SD calls on the Ok To Orig callback function whenever the Ok To Orig status
  changes from FALSE to TRUE while the origination mode is other than none.

  SD calls on the Emergency Callback Mode callback function whenever the
  Emergency Callback Mode status changes from FALSE to TRUE.

  NOTE! A new call to this function overrides all previously registered
  callback functions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                          sd_si_reg(

        sd_si_info_f_type             *si_info_cb_func,
            /* Pointer to a Service Indicators callback function. SD calls on
            ** this function whenever one or more of the service indicators
            ** changes their value.
            **
            ** The argument to this function is a const pointer to a
            ** structure containing the current value of the service
            ** indicators */

        sd_si_acq_fail_f_type         *acq_fail_cb_func,
            /* Pointer to an Acquisition Fail callback function. SD calls on
            ** this function whenever acquisition fails (i.e., SD have
            ** attempted full acquisitions on all systems with no success).
            */

        sd_si_ok_to_orig_f_type       *ok_to_orig_cb_func,
            /* Pointer to an Ok To Orig callback function. SD calls on this
            ** function whenever the Ok To Orig status changes from FALSE to
            ** TRUE while the origination mode is other than none */

        sd_si_emerg_cb_mode_f_type    *emerg_cb_mode_cb_func,
            /* Pointer to an Emergency Callback Mode callback function. SD
            ** calls on this function whenever the Emergency Callback Mode
            ** status changes from FALSE to TRUE */

        sd_si_autonam_change_f_type   *autonam_change_cb_func,
            /* Pointer to an auto-NAM callback function. SD calls on this
            ** function whenever auto-NAM change is appropriate */


        sd_si_gw_net_list_f_type      *gw_net_list_cb_func,
            /* Pointer to the GW network list call function. SD calls on
            ** this function once it receives the results of the get network
            ** list on GSM/WCDMA systems.
            */

        sd_si_sim_state_f_type        *sim_state_cb_func,
           /* Pointer to the sim state call back function. Sd calls on this
           ** function if the SIM state was changed.
           */
        sd_si_reg_done_f_type         *reg_done_cb_func,
           /* Pointer to the reg. done call back function. SD calls this when
           ** a reg. is done successfully.
           */
        sd_si_data_suspend_f_type     *data_suspend_cb_func,
           /* Pointer to the data suspend call back function. SD calls this when
           ** a REG reports change in data suspend flag.
           */

        sd_si_wlan_net_list_f_type    *wlan_net_list_cb_func,
           /* Pointer to the WLAN network list call function. SD calls on
           ** this function once it receives the results of the get network
           ** list on WLAN systems.
           */

        sd_si_wlan_wps_result_f_type  *wlan_wps_result_cb_func,
           /* Pointer to the WLAN network list call function. SD calls on
           ** this function once it receives the results of the get network
           ** list on WLAN systems.
           */

        sd_si_srv_lost_f_type         *srv_lost_cb_func
           /**< Pointer to the service lost callback. Sd calls on this 
           ** function once the service is lost
           */
)
{
  sd_si_s_type        *si_ptr     = sd_si_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Register the callback functions.
  */
  si_ptr->si_info_cb_func         = si_info_cb_func;
  si_ptr->acq_fail_cb_func        = acq_fail_cb_func;
  si_ptr->ok_to_orig_cb_func      = ok_to_orig_cb_func;
  si_ptr->emerg_cb_mode_cb_func   = emerg_cb_mode_cb_func;
  si_ptr->autonam_change_cb_func  = autonam_change_cb_func;
  si_ptr->gw_net_list_cb_func     = gw_net_list_cb_func;
  si_ptr->sim_state_cb_func       = sim_state_cb_func;
  si_ptr->reg_done_cb_func        = reg_done_cb_func;
  si_ptr->data_suspend_cb_func    = data_suspend_cb_func;
  si_ptr->wlan_net_list_cb_func   = wlan_net_list_cb_func;
  si_ptr->wlan_wps_result_cb_func = wlan_wps_result_cb_func;
  si_ptr->srv_lost_cb_func        = srv_lost_cb_func;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  si_ptr = sd_si_ptr(SD_SS_HYBR_HDR);

  /* Register HDR hybrid the callback functions.
  */
  si_ptr->si_info_cb_func         = si_info_cb_func;
  si_ptr->acq_fail_cb_func        = acq_fail_cb_func;
  si_ptr->ok_to_orig_cb_func      = ok_to_orig_cb_func;
  si_ptr->emerg_cb_mode_cb_func   = emerg_cb_mode_cb_func;
  si_ptr->autonam_change_cb_func  = autonam_change_cb_func;
  si_ptr->gw_net_list_cb_func     = gw_net_list_cb_func;
  si_ptr->sim_state_cb_func       = sim_state_cb_func;
  si_ptr->reg_done_cb_func        = reg_done_cb_func;
  si_ptr->data_suspend_cb_func    = data_suspend_cb_func;
  si_ptr->wlan_net_list_cb_func   = wlan_net_list_cb_func;
  si_ptr->wlan_wps_result_cb_func = wlan_wps_result_cb_func;
  si_ptr->srv_lost_cb_func        = srv_lost_cb_func;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  si_ptr = sd_si_ptr(SD_SS_HYBR_WLAN);

  /* Register WLAN hybrid the callback functions.
  */
  si_ptr->si_info_cb_func         = si_info_cb_func;
  si_ptr->acq_fail_cb_func        = acq_fail_cb_func;
  si_ptr->ok_to_orig_cb_func      = ok_to_orig_cb_func;
  si_ptr->emerg_cb_mode_cb_func   = emerg_cb_mode_cb_func;
  si_ptr->autonam_change_cb_func  = autonam_change_cb_func;
  si_ptr->gw_net_list_cb_func     = gw_net_list_cb_func;
  si_ptr->sim_state_cb_func       = sim_state_cb_func;
  si_ptr->reg_done_cb_func        = reg_done_cb_func;
  si_ptr->data_suspend_cb_func    = data_suspend_cb_func;
  si_ptr->wlan_net_list_cb_func   = wlan_net_list_cb_func;
  si_ptr->wlan_wps_result_cb_func = wlan_wps_result_cb_func;
  si_ptr->srv_lost_cb_func        = srv_lost_cb_func;

} /* sd_si_reg */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_info_ptr_get

DESCRIPTION
  This function is used to fetch the current SI info.

DEPENDENCIES
  None.

RETURN VALUE
  sd_si_info_s_type* - pointer to current SI information.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN const sd_si_info_s_type*           sd_si_info_ptr_get(

        void
)
{
  return &(sd_si_ptr(SD_SS_MAIN)->si_info);
}


/*===========================================================================

FUNCTION sd_si_hybr_hdr_info_ptr_get

DESCRIPTION
  This function is used to fetch the current SI info.

DEPENDENCIES
  None.

RETURN VALUE
  sd_si_info_s_type* - pointer to current SI information.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN const sd_si_info_s_type*           sd_si_hybr_hdr_info_ptr_get(

        void
)
{

  return &(sd_si_ptr(SD_SS_HYBR_HDR)->si_info);
}


/*===========================================================================

FUNCTION sd_si_hybr_wlan_info_ptr_get

DESCRIPTION
  This function is used to fetch the current SI info.

DEPENDENCIES
  None.

RETURN VALUE
  sd_si_info_s_type* - pointer to current SI information.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN const sd_si_info_s_type*           sd_si_hybr_wlan_info_ptr_get(

        void
)
{

  return &(sd_si_ptr(SD_SS_HYBR_WLAN)->si_info);
}

#if 0
/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_rprt

DESCRIPTION
  Report SI changes.

DEPENDENCIES
  sd_si_init() must have already been called to initialize SI.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sd_si_rprt(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  const sd_si_s_type        *si_ptr         = sd_si_ptr( ss );
  const sd_si_info_s_type   *info_ptr       = &si_ptr->si_info;

  event_id_enum_type        event_id        = EVENT_SD_SRV_IND;

  sdlog_si_s_type           si_buf;
  sdlog_si_s_type           *si_buf_ptr     = &si_buf;
  int                       si_buf_size     = sizeof( sdlog_si_s_type );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If reporting for HDR hybrid system selection, set event_id to
  ** EVENT_SD_EVENT_ACTION_HYBR.
  */
  if( ss == SD_SS_HYBR_HDR ) {
    event_id = EVENT_SD_SRV_IND_HYBR;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy over the SI fields.
  */
  si_buf_ptr->srv_status  = info_ptr->srv_status;
  si_buf_ptr->mode        = info_ptr->mode;
  si_buf_ptr->band        = info_ptr->band;
  si_buf_ptr->chan        = info_ptr->chan;
  si_buf_ptr->roam        = info_ptr->roam;
  si_buf_ptr->prl_desig   = info_ptr->prl_desig;

  ???? continue

} /* sd_si_rprt */
#endif



#ifdef FEATURE_USERZONE
/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_uz_info_ptr_get

DESCRIPTION
  This function is used to fetch the current UZ selected info.

DEPENDENCIES
  None.

RETURN VALUE
  sd_si_uz_info_s_type* - pointer to current SI User zone selection information.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  const sd_si_uz_info_s_type*            sd_si_uz_info_ptr_get(

        void
)
{
  return &sd_si_ptr(SD_SS_MAIN)->si_info.curr_uz_info;
}
#endif /* FEATURE_USERZONE */




#ifdef FEATURE_REG_ACC_ROAM_IND
/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_si_roam_update

DESCRIPTION
  Sets the roam indicator to the value passed in( TSB58 value).
  Updates the roam flag in serving system record accordingly.

  If one or more of the service indicators has changed, inform the registered
  client (e.g., CM) of the new service indicators.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN void                      sd_si_roam_update(

        sd_roam_e_type            roam_ind
        /* Roam indication to be updated. */
)
{
  sd_si_s_type          *si_ptr       = sd_si_ptr(SD_SS_MAIN);
  sd_si_info_s_type     *info_ptr     = &si_ptr->si_info;
  #ifdef SD_USES_DB_SERVICES
  db_items_value_type   db_item;
  #endif

  /* Point at serving and idle systems.
  */
  sdsr_ref_type         sr_ref        = sdsr_list_get_ref(SDSR_SRV_SYS, 0);

  /* Point at system record per which to update the service indicators.
  */
  sdsr_s_type           *sr_ptr       = sdsr_tbl_get_ptr( sr_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert the TSB58 roam ind value to internal representation
  */
  roam_ind = prl_map_tsb58_roam_to_sd_roam( roam_ind );
  SD_ASSERT( roam_ind <= SD_ROAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the roam service indicators has not changed, just return.
  */
  if( info_ptr->roam == roam_ind )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  info_ptr->roam = roam_ind;

  /* Update the serving system record, so that
  ** roaming status doesn't get changed when  sd_si_update
  ** is called later.
  */
  sr_ptr->roam = roam_ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update DB roam item.
  */
  #ifdef SD_USES_DB_SERVICES
  db_item.roam = (db_roam_type) info_ptr->roam;
  db_put( DB_ROAM, &db_item );
  #endif

  /* Inform the registered client (e.g., CM) of the new service indicators.
  */
  if( si_ptr->si_info_cb_func != NULL )
  {
    SD_MSG_HIGH( "Roam ind change, mode=%d,band=%d,roam=%d",
                  info_ptr->mode, info_ptr->band, info_ptr->roam );

    si_ptr->si_info_cb_func( &si_ptr->si_info );
  }

} /* sd_si_roam_update */
#endif /* FEATURE_REG_ACC_ROAM_IND */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_update

DESCRIPTION
  Set the service indicator to TRUE and update the system indicators per
  the input system record.

  If one or more of the service indicators has changed, inform the registered
  client (e.g., CM) of the new service indicators.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                            sd_si_update(
             /*lint -esym(550, is_uz_check) */ /* para not accessed */
        sd_ss_e_type                    ss,
            /* System selection - MAIN or HDR.
            */

        sd_si_update_e_type             update_criteria,
            /* Update criteria.
            */

        sys_srv_status_e_type           srv_status,
            /* Current service status ( LIMITED, FULL service etc ).
            */

        sys_srv_domain_e_type           srv_domain,
            /* Registered service domain.
            */

        sdsr_ref_type                   sr_ref,
            /* System record reference according to which to update the
            ** system indicators.
            */

        boolean                         is_uz_check,
            /* Flag to indicate if User zone selected
            ** check needs to be done.
            */

        const sd_mode_e_type            *mode_ptr,
           /* Pointer to mode - ignored if NULL.
           */

        const sd_band_e_type            *band_ptr,
            /* Pointer to band - ignored if NULL.
            */

        const sd_chan_type              *chan_ptr,
             /* Pointer to channel - ignored if NULL.
             */

        const sd_si_mode_info_u_type    *mode_info_ptr,
            /* Pointer to mode info.
            */

        sdsr_e_type                     colloc_lst,
            /* The collocated list
            */

        const sys_active_prot_e_type    *hdr_active_prot_ptr,
            /* The HDR active protocol revision
            */

        const sdss_sys_addn_info_s_type  *curr_sys_info,
            /* Association tag of the currently acquired
            ** system, if any.
            */

        sys_srv_status_e_type           cs_srv_status
            /* Current CS service status
            */
)
{
  sd_si_s_type          *si_ptr       = sd_si_ptr( ss );
  sd_si_info_s_type     *info_ptr     = &si_ptr->si_info;

  /* Get the current SS-State.
  */
  sdss_state_e_type     ss_state      = si_ptr->ss_ptr->ss_state;

  /* Point at system record per which to update the service indicators.
  */
  sdsr_s_type           *sr_ptr       = sdsr_tbl_get_ptr( sr_ref );

  boolean               is_pref_srv_acq;
  boolean               is_redir_or_ho;

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

  #ifdef SD_USES_DB_SERVICES
  db_items_value_type   db_item;
  #endif

  sd_mode_e_type        mode;
  sd_band_e_type        band;
  sd_chan_type          chan;
  sd_sid_u_type         prev_sid;
  sys_active_prot_e_type  hdr_active_prot = info_ptr->hdr_active_prot;

  #ifdef FEATURE_USERZONE
  int4                  new_uzid      = SD_INVALID_UZID;
  int4                  uz_entry_index= PUZL_INVLD_UZ_ENTRY_INDEX;
  puzl_uz_record_s_type *uz_rec_ptr   = NULL;
  #else
  is_uz_check = FALSE;
  #endif /* FEATURE_USERZONE */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( update_criteria, SD_SI_UPDATE_MAX );

  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );
  SD_ASSERT_ENUM_IS_INRANGE( ss_state, SDSS_STATE_MAX );

  SD_ASSERT_ENUM_IS_INRANGE( sr_ptr->sys.mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( sr_ptr->sys.band, SD_SS_BAND_PREF_MAX );
  SD_ASSERT( sr_ptr->roam < SYS_ROAM_STATUS_MAX );
  SD_ASSERT( srv_status != SYS_SRV_STATUS_NO_SRV );
  SD_ASSERT( srv_status <= SYS_SRV_STATUS_MAX );
  SD_ASSERT( srv_domain <= SYS_SRV_DOMAIN_MAX );
  SD_ASSERT( cs_srv_status <= SYS_SRV_STATUS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(is_uz_check);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the minimum service update criteria is not met, return now.
  */
  if( update_criteria < si_ptr->si_update_criteria )
  {
    return;
  }

  /* If we got here, the minimum service update criteria is met, so reset
  ** the service update criteria to successful acquisition.
  */
  si_ptr->si_update_criteria = SD_SI_UPDATE_ACQ;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get acquired band and channel
  */
  mode = sr_ptr->sys.mode;
  band = sdss_map_band_pref_to_band(sr_ptr->sys.band);
  chan = sr_ptr->sys.chan;

  /* Overwrite system's mode and/or band if input mode and/or band parameters
  ** are provided.
  */
  if( mode_ptr != NULL )
  {
    mode = *mode_ptr;
  }

  if( band_ptr != NULL )
  {
    band = *band_ptr;
  }

  if ( chan_ptr != NULL )
  {
    chan = *chan_ptr;
  }
  else if ( mode == SD_MODE_WLAN )
  {
    /* If channel is not passed in for WLAN,
    ** set it to 0.
    */
    chan = 0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get HDR active protocol revision
  */
  if( hdr_active_prot_ptr != NULL )
  {

    hdr_active_prot = *hdr_active_prot_ptr;

    SD_ASSERT( hdr_active_prot == SYS_ACTIVE_PROT_NONE ||
               BETWEEN( hdr_active_prot,
                        SYS_ACTIVE_PROT_HDR_START,
                        SYS_ACTIVE_PROT_HDR_END ) );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform client if registration was done.
  */
  if( update_criteria == SD_SI_UPDATE_REG &&
      si_ptr->reg_done_cb_func != NULL )
  {
     SD_MSG_HIGH( "Registration done, mode=%d",
                  sr_ptr->sys.mode, 0, 0 );

     si_ptr->reg_done_cb_func( ss, mode );
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out whether system determination is still attempting to acquire
  ** a more preferred service.
  */
  if( ss_state == SDSS_STATE_OPR ||
      sdss_is_resel_state(ss, SDSS_RESEL_STT_SRCH) )
  {
    is_pref_srv_acq = FALSE;
  }
  else
  {
    is_pref_srv_acq = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out whether redirection or handoff indication is ON.
  */
  if( sdss_is_redir( ss, SDSS_REDIR_VAL_NONE) &&
      sdss_is_handoff(SDSS_HO_VAL_NONE) )
  {
    is_redir_or_ho = FALSE;
  }
  else
  {
    is_redir_or_ho = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set SI system for SID comparison.
  */
  prev_sid = info_ptr->sid;

  #ifdef FEATURE_USERZONE
  /* Check for user zone selected or not only if the following is true
  ** is_uz_check is true and
  ** MS is currently in CDMA mode
  */
  if ( is_uz_check == TRUE &&
       sr_ptr->sys.mode == SD_MODE_CDMA )
  {
     uz_entry_index = puzl_find_record_with_sidnid(sr_ptr->sid.is95.sid,
                                                   sr_ptr->sid.is95.nid);
     if ( uz_entry_index != PUZL_INVLD_UZ_ENTRY_INDEX )
     {
       uz_rec_ptr = puzl_extract_uz_record( uz_entry_index);
       if (uz_rec_ptr != NULL )
       {
         /* Found a user zone entry in PUZL with matching sid & nid and
         ** successfully extracted the matching uz record
         */
         new_uzid = uz_rec_ptr->uz_id;
       }
     }
  }
  #endif /* FEATURE_USERZONE */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If none of the service indicators has changed, just return.
  */
  if( info_ptr->srv_status        == srv_status &&
      info_ptr->cs_srv_status     == cs_srv_status &&
      info_ptr->is_pref_srv_acq   == is_pref_srv_acq &&
      info_ptr->is_redir_or_ho    == is_redir_or_ho &&
      info_ptr->mode              == mode &&
      info_ptr->band              == band &&
      info_ptr->chan              == chan &&
      info_ptr->prl_desig         == sr_ptr->prl &&
      info_ptr->hdr_active_prot   == hdr_active_prot &&
      info_ptr->is_sys_prl_match  == sdss_sr_is_prl( sr_ptr,
                                                     SDSS_PRL_VAL_ANY_PREF
                                                   ) &&
      info_ptr->is_sys_forbidden  == sdss_sr_is_prl( sr_ptr,
                                                     SDSS_PRL_VAL_FORBID
                                                   ) &&
      info_ptr->srv_domain        == srv_domain &&
      info_ptr->sys_srv_cap  == (sys_srv_capability_e_type)sr_ptr->srv_cap &&
      sd_is_same_sid( info_ptr->mode, &prev_sid, sr_ptr) &&
      #if defined(FEATURE_GW) || defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
      sd_is_same_gw_lac( info_ptr->mode, &prev_sid, sr_ptr ) &&
      #endif
      #ifdef FEATURE_USERZONE
      ( ! is_uz_check ||
        info_ptr->curr_uz_info.uz_id== new_uzid ) &&
      #endif /* FEATURE_USERZONE */
      info_ptr->roam              == (sd_roam_e_type)sr_ptr->roam
                              &&
      ( mode_info_ptr == NULL ||
        mode != SD_MODE_CDMA ||
        ( info_ptr->mode_info.cdma.base_id == mode_info_ptr->cdma.base_id &&
          info_ptr->mode_info.cdma.base_lat == mode_info_ptr->cdma.base_lat &&
          info_ptr->mode_info.cdma.base_long == mode_info_ptr->cdma.base_long
        )
      )
                              &&
      ( mode_info_ptr == NULL ||
        mode != SD_MODE_WLAN ||
        ( info_ptr->mode_info.wlan.bss_type == mode_info_ptr->wlan.bss_type &&
          info_ptr->mode_info.wlan.bssid  == mode_info_ptr->wlan.bssid &&
          info_ptr->mode_info.wlan.tech == mode_info_ptr->wlan.tech
        )
      )
                              &&
      info_ptr->def_roam_ind      == prl_hdr_get_ptr()->def_roam_ind
    )

  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, one or more of the service indicators has changed.
  */

  /* Update SI_SYS per the input sr_ref.
  */
  sdsr_list_put_ref( si_ptr->si_sys, sr_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the service indicators per the system record that is pointed by
  ** the sr_ptr parameter.
  */
  info_ptr->srv_status        = srv_status;
  info_ptr->is_pref_srv_acq   = is_pref_srv_acq;
  info_ptr->is_redir_or_ho    = is_redir_or_ho;
  //info_ptr->mode              = sr_ptr->sys.mode;

  /* Set the CS service status
  */
  info_ptr->cs_srv_status     = cs_srv_status;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If mode is changed to CDMA from any other or mode is not CDMA.;
  ** initialize the mode specific info.
  */
  if ( mode == SD_MODE_CDMA &&
       info_ptr->mode != SD_MODE_CDMA
     )
  {
    info_ptr->mode_info.cdma.base_id = 0;
    info_ptr->mode_info.cdma.base_lat = 0;
    info_ptr->mode_info.cdma.base_long = 0;
    info_ptr->mode_info.cdma.srch_win_n = 0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* When system acquire, it only acquire a specific band.
  */
  info_ptr->mode              = mode;
  info_ptr->band              = band;
  info_ptr->chan              = chan;

  /* if its WLAN mode, get it from sid_expected else get it from sid.
  */
  switch (info_ptr->mode)
  {
    case SYS_SYS_MODE_WLAN:
     info_ptr->sid.ssid = sr_ptr->sid_expected.prm.wlan.ssid;
     break;

    case SYS_SYS_MODE_CDMA:
    case SYS_SYS_MODE_AMPS:
      memcpy( &info_ptr->sid.is95,
              &sr_ptr->sid.is95,
              sizeof(info_ptr->sid.is95)
            );
      break;

    case SYS_SYS_MODE_HDR:
      memcpy( info_ptr->sid.is856,
              sr_ptr->sid.is856,
              sizeof( info_ptr->sid.is856)
           );
      break;

    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_GW:
      info_ptr->sid.plmn = sr_ptr->sid.plmn;
      break;

    case SYS_SYS_MODE_GPS:
    case SYS_SYS_MODE_NONE:
    case SYS_SYS_MODE_NO_SRV:
    case SYS_SYS_MODE_MAX:
      break;

  } /* switch (info_ptr->mode) */


  info_ptr->roam              = (sd_roam_e_type)sr_ptr->roam;
  info_ptr->prl_desig         = sr_ptr->prl;
  info_ptr->is_sys_prl_match  = sdss_sr_is_prl( sr_ptr,
                                                SDSS_PRL_VAL_ANY_PREF
                                              );
  info_ptr->is_sys_forbidden  = sdss_sr_is_prl( sr_ptr,
                                                SDSS_PRL_VAL_FORBID
                                              );
  info_ptr->def_roam_ind      = prl_hdr_get_ptr()->def_roam_ind;

  info_ptr->srv_domain        = srv_domain;
  info_ptr->sys_srv_cap       = (sys_srv_capability_e_type)sr_ptr->srv_cap;

  info_ptr->is_colloc     = SDSR_PRL_ASSN_GET( sr_ptr->prl_assn,
                                               SDSR_PRL_ASSN_TAG
                                             );

  info_ptr->is_pn_assn    = SDSR_PRL_ASSN_GET( sr_ptr->prl_assn,
                                               SDSR_PRL_ASSN_PN
                                             );

  info_ptr->is_data_assn  = SDSR_PRL_ASSN_GET( sr_ptr->prl_assn,
                                               SDSR_PRL_ASSN_DATA
                                             );

  if ( mode_info_ptr != NULL )
  {
    info_ptr->mode_info = *mode_info_ptr;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset BCMCS service status if mode is a protocol other than HDR.  We
  ** don't want to turn off BCMCS service just because we lost HDR serivce,
  ** because if we do so, then the DS flows will be torn down unnecessarily.
  ** Simply pass through the BCMCS serivce status to DS via CM and let DS
  ** make all the decisions.
  */
  if ( info_ptr->mode != SD_MODE_HDR && info_ptr->mode != SD_MODE_INACT )
  {
    info_ptr->bcmcs_srv_status = SYS_BCMCS_SRV_STATUS_NO_SRV;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_USERZONE
  /* Update the user zone info, if new_uzid is valid
  */
  if ( is_uz_check  &&
       new_uzid != info_ptr->curr_uz_info.uz_id )
  {
    info_ptr->curr_uz_info.uz_id       = new_uzid;
    if ( new_uzid != SD_INVALID_UZID &&
         uz_rec_ptr != NULL
       )
    {
      info_ptr->curr_uz_info.uz_act_flag = uz_rec_ptr->active_flag;
      info_ptr->curr_uz_info.uz_name     = uz_rec_ptr->uz_name;
    }
  } /* if(is_uz_check) */
  #endif /* FEATURE_USERZONE */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update SS-Core srv_status/srv_domain.
  */
  sdss_update_srv_status( ss, srv_status, srv_domain );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If main system selection, Update DB items.
  */
  if( ss == SD_SS_MAIN )
  {
    #ifdef SD_USES_DB_SERVICES
    db_item.no_svc    = FALSE;
    db_put( DB_NO_SVC, &db_item );

    db_item.digital   = (info_ptr->mode == SD_MODE_AMPS) ? FALSE : TRUE;
    db_put( DB_DIGITAL, &db_item );

    db_item.roam      = (db_roam_type) info_ptr->roam;
    db_put( DB_ROAM, &db_item );
    #endif

    if( info_ptr->mode == SD_MODE_HDR )
    {
      /* Printout HDR Sector ID.
      */
      sdsr_hdr_sid_print( info_ptr->sid.is856 );
    }
    else
    {
      #ifdef SD_USES_DB_SERVICES
      db_item.sid       = info_ptr->sid.is95.sid;
      db_put( DB_SID, &db_item );

      db_item.nid       = info_ptr->sid.is95.nid;
      db_put( DB_NID, &db_item );
      #endif
    }

    /* Update the srv uptime if applicable
    */

    if(!sys_srv_status_is_srv(srv_status))
    {
      si_ptr->si_info.srv_acq_time = 0;

      if( si_ptr->si_info.srv_lost_time == 0 )
      {
        si_ptr->si_info.srv_lost_time = clk_uptime();
      }
    }
    else
    {
      if( si_ptr->si_info.srv_acq_time == 0 )
      {
        si_ptr->si_info.srv_acq_time = clk_uptime();
      }
      si_ptr->si_info.srv_lost_time = 0;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the HDR active protocol revision
  */
  if( ( hdr_active_prot > SYS_ACTIVE_PROT_HDR_START &&
        hdr_active_prot < SYS_ACTIVE_PROT_HDR_END ) ||
        (hdr_active_prot == SYS_ACTIVE_PROT_NONE ) )
  {
    si_ptr->si_info.hdr_active_prot = hdr_active_prot;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif

  /* Inform the registered client (e.g., CM) of the new service indicators.
  */
  if( si_ptr->si_info_cb_func != NULL )
  {
    SD_MSG_HIGH( "Srv change, mode=%d,band=%d,roam=%d",
                  sr_ptr->sys.mode, band, sr_ptr->roam );

    SD_MSG_HIGH( "Srv change, realign_hdr=%d,is_colloc=%d, prot=%d",
                  info_ptr->realign_hdr, info_ptr->is_colloc,
                  info_ptr->hdr_active_prot );

    si_ptr->si_info_cb_func( ss, &si_ptr->si_info );
  }
  #ifndef FEATURE_HDR_HYBRID
  SYS_ARG_NOT_USED(colloc_lst);
  SYS_ARG_NOT_USED(curr_sys_info);
  #endif /* FEATURE_HDR_HYBRID */
} /* sd_si_update *//*lint +esym(550, is_uz_check) */ /* para not accessed */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_is_srv_on

DESCRIPTION
  Indicate whether the service indicator is currently ON.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the service indicator is currently ON. FALSE otherwise.

  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sd_si_is_srv_on(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  sd_si_s_type          *si_ptr     = sd_si_ptr( ss );
  sd_si_info_s_type     *info_ptr   = &si_ptr->si_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if service status indicates NO_SRV.
  */
  return sys_srv_status_is_srv( info_ptr->srv_status );

} /* sd_si_is_srv_on */

/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_update_if_srv_is_off

DESCRIPTION
  Set the service indicator to TRUE and update the system indicators per
  the input system record iff service is currently OFF.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_si_update_if_srv_is_off(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sd_si_update_e_type       update_criteria,
            /* Update criteria.
            */

        sys_srv_status_e_type     srv_status,
            /* Current service status ( LIMITED, FULL service etc ).
            */

        sys_srv_domain_e_type     srv_domain,
            /* Service domain.
            */


        sdsr_ref_type             sr_ref,
            /* System record reference according to which to update the
            ** system indicators.
            */

        const sd_mode_e_type      *mode_ptr,
           /* Pointer to mode - ignored if NULL.
           */

        const sd_band_e_type      *band_ptr,
            /* Pointer to band - ignored if NULL.
            */

        const sd_chan_type        *chan_ptr,
             /* Pointer to channel - ignored if NULL.
             */

        sdsr_e_type               colloc_list,
            /* collocated list.
            */

        const sys_active_prot_e_type *hdr_active_prot_ptr,
            /* HDR active protocol type.
            */

        const sdss_sys_addn_info_s_type *curr_sys_info,
            /* Association tag of the currently acquired
            ** system, if any.
            */

        sys_srv_status_e_type     cs_srv_status
            /* Current CS service status.
            */
)
{
  sd_si_s_type          *si_ptr     = sd_si_ptr( ss );
  sd_si_info_s_type     *info_ptr   = &si_ptr->si_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( update_criteria, SD_SI_UPDATE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If service is currently OFF, call on sd_si_update() to update the
  ** service indicators.
  */
  if( !sys_srv_status_is_srv( info_ptr->srv_status ) )
  {
    sd_si_update( ss,
                  update_criteria,
                  srv_status,
                  srv_domain,
                  sr_ref,
                  FALSE,
                  mode_ptr,
                  band_ptr,
                  chan_ptr,
                  NULL,
                  colloc_list,
                  hdr_active_prot_ptr,
                  curr_sys_info,
                  cs_srv_status
                );
  }

} /* sd_si_update_if_srv_is_off */





/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_srv_off

DESCRIPTION
  Inform the System Indication component that the service indication should
  be turned off.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sd_si_srv_off(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  sd_si_s_type          *si_ptr     = sd_si_ptr( ss );
  sd_si_info_s_type     *info_ptr   = &si_ptr->si_info;

  #ifdef SD_USES_DB_SERVICES
  db_items_value_type   db_item;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If main system selection, update DB items.
  */
  if( ss == SD_SS_MAIN )
  {
    #ifdef SD_USES_DB_SERVICES
    db_item.no_svc    = TRUE;
    db_put( DB_NO_SVC, &db_item );

    db_item.roam      = DB_ROAM_OFF;
    db_put( DB_ROAM, &db_item );
    #endif
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If service is already OFF, just return.
  */
  if( info_ptr->srv_status == SYS_SRV_STATUS_NO_SRV  )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, service is not already OFF
  */

  /* Clear the SI_SYS.
  */
  sdsr_list_clr( si_ptr->si_sys );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the service status to NO_SRV.
  */
  info_ptr->srv_status   = SYS_SRV_STATUS_NO_SRV;
  info_ptr->srv_acq_time = 0;

  info_ptr->is_colloc     = FALSE;
  info_ptr->is_pn_assn    = FALSE;
  info_ptr->is_data_assn  = FALSE;

  if( info_ptr->srv_lost_time == 0 )
  {
    info_ptr->srv_lost_time = clk_uptime();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update SS-Core srv_status.
  */
  sdss_update_srv_status( ss, SYS_SRV_STATUS_NO_SRV, SYS_SRV_DOMAIN_NO_SRV );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* clear CDMA / HDR time info
  */
  info_ptr->time_info.sys_mode    = SYS_SYS_MODE_NO_SRV;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the registered client (e.g., CM) of the new service indicators.
  */
  if( si_ptr->si_info_cb_func != NULL )
  {
    SD_MSG_HIGH( "Service is OFF, SS=%d",ss,0,0 );
    si_ptr->si_info_cb_func( ss, info_ptr );
  }

} /* sd_si_srv_off */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_srv_off_until

DESCRIPTION
  Inform the System Indication component that the service indication should
  be turned off until the specified service update criteria is met.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sd_si_srv_off_until(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sd_si_update_e_type       update_criteria
            /* Service update criteria - indicate the minimum criteria for
            ** updating the service indicators.
            */
)
{
  sd_si_s_type          *si_ptr     = sd_si_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the service update criteria.
  */
  si_ptr->si_update_criteria = update_criteria;

  /* Set the service indicator to OFF.
  */
  sd_si_srv_off( ss );

} /* sd_si_srv_off_until */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_srv_offline

DESCRIPTION
  Inform the System Indication component that the MS is going offline.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                         sd_si_srv_offline(
            /*lint -esym(715, opr_mode) */ /* param not referenced */

        sdss_opr_mode_e_type          opr_mode
            /* Offline mode (CDMA, AMPS, HDR, etc).
            */
)
{
  #ifdef SD_USES_DB_SERVICES
  db_items_value_type   db_item;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the DB_DIGITAL item according to the offline state.
  */
  #ifdef SD_USES_DB_SERVICES
  if( opr_mode == SDSS_OPR_MODE_OFFLINE_AMPS )
  {
    db_item.digital = FALSE;
  }
  else if ( opr_mode == SDSS_OPR_MODE_OFFLINE_CDMA )
  {
    db_item.digital = TRUE;
  }

  db_put( DB_DIGITAL, &db_item );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the service indicator to OFF.
  */
  sd_si_srv_off( SD_SS_MAIN );
  sd_si_srv_off( SD_SS_HYBR_HDR );

} /* sd_si_srv_offline */
  /*lint +esym(715, opr_mode) */ /* param not referenced */



/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_pwr_save

DESCRIPTION
  Inform the System Indication component that the service indication
  is pwr_save.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sd_si_srv_pwr_save(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  sd_si_s_type          *si_ptr     = sd_si_ptr( ss );
  sd_si_info_s_type     *info_ptr   = &si_ptr->si_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If service is already PWR_SAVE, just return.
  */
  if( info_ptr->srv_status == SYS_SRV_STATUS_PWR_SAVE )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, service is not already OFF
  */

  /* Clear the SI_SYS.
  */
  sdsr_list_clr( si_ptr->si_sys );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset the fields.
  */
  info_ptr->is_colloc     = FALSE;
  info_ptr->is_pn_assn    = FALSE;
  info_ptr->is_data_assn  = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the srv_status to pwr_save.
  */
  info_ptr->srv_status = SYS_SRV_STATUS_PWR_SAVE;
  sdss_update_srv_status( ss, SYS_SRV_STATUS_PWR_SAVE, SYS_SRV_DOMAIN_NO_SRV );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* clear time info flag */
  info_ptr->time_info.sys_mode     = SYS_SYS_MODE_NO_SRV;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the registered client (e.g., CM) of the new service indicators.
  */
  if( si_ptr->si_info_cb_func != NULL )
  {
    SD_MSG_HIGH( "Service is PWR_SAVE, SS=%d",ss,0,0 );
    si_ptr->si_info_cb_func( ss, info_ptr );
  }

} /* sd_si_srv_pwr_save*/



/* <EJECT> */
//*==========================================================================
//
//FUNCTION sd_si_srv_off_if_pwr_save
//
//DESCRIPTION
//  Inform the System Indication component that the service indication
//  is srv_off.
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
//static  void                      sd_si_srv_off_if_pwr_save(
//
//        sd_ss_e_type              ss
//            /* System selection - MAIN or HDR.
//            */
//)
//{
//  sd_si_s_type          *si_ptr     = sd_si_ptr( ss );
//  sd_si_info_s_type     *info_ptr   = &si_ptr->si_info;
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
//
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  /* If service is already OFF, just return.
//  */
//  if( info_ptr->srv_status != SYS_SRV_STATUS_PWR_SAVE )
//  {
//    return;
//  }
//
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  /* Update the srv_status to pwr_save.
//  */
//  info_ptr->srv_status = SYS_SRV_STATUS_NO_SRV;
//
// /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  /* Inform the registered client (e.g., CM) of the new service indicators.
//  */
//  if( si_ptr->si_info_cb_func != NULL )
//  {
//    SD_MSG_HIGH( "Service is NO_SRV, SS=%d",ss,0,0 );
//    si_ptr->si_info_cb_func( ss, info_ptr );
//  }
//
//} /* sd_si_pwr_save_if_srv_off */




#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_pref_srv_acq

DESCRIPTION
  Notify the registered client (i.e., CM) of preferred service acquisition
  indicator changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_si_pref_srv_acq(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  sd_si_s_type          *si_ptr     = sd_si_ptr( ss );
  sd_si_info_s_type     *info_ptr   = &si_ptr->si_info;

  /* Get the current SS-State.
  */
  sdss_state_e_type     ss_state    = si_ptr->ss_ptr->ss_state;

  boolean               is_pref_srv_acq;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( ss_state, SDSS_STATE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out whether system determination is still attempting to acquire
  ** a more preferred service.
  */
  if( ss_state == SDSS_STATE_OPR ||
      sdss_is_resel_state(ss, SDSS_RESEL_STT_SRCH) )
  {
    is_pref_srv_acq = FALSE;
  }
  else
  {
    is_pref_srv_acq = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* No need to inform the registered client of change if service is
  ** currently turned OFF or if preferred service acquisition indicator is
  ** not changed.
  */
  if( !sys_srv_status_is_srv( info_ptr->srv_status)  ||
      info_ptr->is_pref_srv_acq == is_pref_srv_acq )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the preferred service acquisition indicator.
  */
  info_ptr->is_pref_srv_acq = is_pref_srv_acq;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the registered client (e.g., CM) of the service indicators
  ** change.
  */
  if( si_ptr->si_info_cb_func != NULL )
  {
    SD_MSG_HIGH( "Pref srv acq change=%d", is_pref_srv_acq, 0, 0 );
    si_ptr->si_info_cb_func( ss, &si_ptr->si_info );
  }

} /* sd_si_pref_srv_acq */


#ifdef FEATURE_HDR
#error code not present
#endif


/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_acq_fail

DESCRIPTION
  Notify the registered client (i.e., CM) that acquisition over all systems
  has failed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sd_si_acq_fail(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  sd_si_s_type        *si_ptr     = sd_si_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( si_ptr->acq_fail_cb_func != NULL )
  {
    SD_MSG_HIGH( "ACQ failed ss=%d",ss,0,0 );
    si_ptr->acq_fail_cb_func( ss );
  }

} /* sd_si_acq_fail */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_ok_to_orig

DESCRIPTION
  Notify the registered client (i.e., CM) that it is OK to orig.

DEPENDENCIES
  Call after sd_si_update() and sd_si_update_data_suspend_state() have been
  called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_si_ok_to_orig(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        boolean                   data_suspend
            /* Data suspend status
            */
)
{
  sd_si_s_type                   *si_ptr     = sd_si_ptr( ss );
  sd_si_ok_to_orig_s_type        si_ok_to_orig;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If orig-mode is other than none, inform the registered client (i.e., CM)
  ** that it is OK to originate.
  */
  if( sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_ANY) )
  {
    if( si_ptr->ok_to_orig_cb_func != NULL )
    {
      SD_MSG_HIGH( "OK to orig ss=%d",ss,0,0 );

      si_ok_to_orig.mode            = si_ptr->si_info.mode;
      si_ok_to_orig.srv_domain      = si_ptr->si_info.srv_domain;
      si_ok_to_orig.srv_status      = si_ptr->si_info.srv_status;
      si_ok_to_orig.ps_data_suspend = data_suspend;

      si_ptr->ok_to_orig_cb_func( ss, &si_ok_to_orig );
    }
  }

} /* sd_si_ok_to_orig */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_emerg_cb_mode

DESCRIPTION
  Notify the registered client (e.g., CM) that SD entered emergency callback
  mode of operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sd_si_emerg_cb_mode( void )
{
  sd_si_s_type        *si_ptr     = sd_si_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the registered client (e.g., CM) that SD entered emergency
  ** callback mode of operation.
  */
  if( si_ptr->emerg_cb_mode_cb_func != NULL )
  {
    SD_MSG_HIGH( "Emergency callback mode",0,0,0 );
    si_ptr->emerg_cb_mode_cb_func();
  }

} /* sd_si_emerg_cb_mode */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_autonam_change

DESCRIPTION
  Notify the registered client (e.g., CM) that auto-NAM change is
  appropriate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sd_si_autonam_change(

        sd_nam_e_type             nam
        /* The NAM that is proposed by the auto-NAM functionality.
        */
)
{
   sd_si_s_type        *si_ptr     = sd_si_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the registered client (e.g., CM) that auto-NAM change is
  ** appropriate.
  */
  if( si_ptr->autonam_change_cb_func != NULL )
  {
    SD_MSG_HIGH( "Auto-NAM change nam=%d",nam,0,0 );
    si_ptr->autonam_change_cb_func( nam );
  }

} /* sd_si_autonam_change */


#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */


#if defined(FEATURE_WLAN)
#error code not present
#endif /* defined(FEATURE_WLAN) */


/*===========================================================================

FUNCTION sd_si_srv_lost_cb

DESCRIPTION
  Notify the registered client (i.e., CM) that service is lost

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sd_si_srv_lost_cb(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  sd_si_s_type        *si_ptr     = sd_si_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( si_ptr->srv_lost_cb_func != NULL )
  {
    SD_MSG_HIGH( "SRV Lost ss=%d",ss,0,0 );
    si_ptr->srv_lost_cb_func( ss );
  }

} /* sd_si_srv_lost_cb */

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== SYSTEM SELECTION =============================
=============================================================================
=============================================================================
===========================================================================*/

#ifdef FEATURE_WLAN
#error code not present
#endif /*FEATURE_WLAN */


/*===========================================================================

FUNCTION sd_ss_ptr

DESCRIPTION
  Return the pointer to SS component.


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_ss_s_type*             sd_ss_ptr(

        sd_ss_e_type              ss
            /* System selection item.
            */

)
{

  /* System Selection components.
  */
  static sd_ss_s_type   sd_ss;

  #if (defined (FEATURE_TECH_IND_SD_STACK_ARCH) || defined( FEATURE_HDR_HYBRID ))
  static sd_ss_s_type    sd_ss_hybr_1;
  #endif

  #ifdef FEATURE_WLAN
#error code not present
  #endif

  static sd_ss_s_type*  sd_ss_arr[SD_SS_MAX] =
  {
    &sd_ss,

    #if (defined (FEATURE_TECH_IND_SD_STACK_ARCH) || defined( FEATURE_HDR_HYBRID ))
    &sd_ss_hybr_1,
    #else
    &sd_ss,
    #endif

    #ifdef FEATURE_WLAN
#error code not present
    #else
    &sd_ss,
    #endif

  };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ss < SD_SS_MAX );

  return sd_ss_arr[ss];

} /* sd_ss_ptr() */

/*===========================================================================

FUNCTION sd_ss_init

DESCRIPTION
  Initialize the SD System Selection component.

  NOTE! This function must be called before any other sd_ss_xxx() or
  sdss_xxx() function is ever called. This function shall not be called a
  second time prior to calling on sd_term().

  This function also registers an SS-Timer callback function with SS. SS
  calls on the provided callback function to set/disable the SS-Timer. Upon
  SS-Timer expiration the SS-Client should call on the sd_ss_ind_misc_timer()
  SS-Indication function.

  This function set the System Selection State (SS-State) to acquisition.

DEPENDENCIES
  This function must be called before any other sd_ss_xxx() or sdss_xxx()
  function is ever called. This function shall not be called a second time
  prior to calling on sd_term().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_ss_init(

        sd_ss_s_type              *ss_ptr,
            /* Pointer to a system selection item.
            */

        void                      (*timer_func_ptr) (int4  ms)
            /* Pointer to an SS-Timer callback function.
            **
            ** SS calls on this function to set/disable the SS-Timer. Upon
            ** SS-Timer expiration the SS-Client should call on the
            ** sd_ss_ind_misc_timer() SS-Indication function.
            **
            ** The argument to this function is the time in ms with which
            ** to set the SS-Timer. An input of 0 should disable the
            ** SS-Timer */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ss_ptr != NULL );
  SD_ASSERT( timer_func_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_FUNC_START( "sd_ss_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system selection structure.
  */
  ss_ptr->act.act                   = SD_SS_ACT_MAX;
  ss_ptr->prev_act.act              = SD_SS_ACT_MAX;
  ss_ptr->true_curr_act.act         = SD_SS_ACT_MAX;
  ss_ptr->ss_state                  = SDSS_STATE_ACQ;

  ss_ptr->acq_list                  = SDSR_MAX;
  ss_ptr->acq_pos                   = SDSR_POS_NULL;
  ss_ptr->acq_mode                  = SDSS_ACQ_MODE_MAX;

  ss_ptr->meas_id                   = 0;
  ss_ptr->meas_is_done              = FALSE;
  ss_ptr->is_get_net_done           = TRUE;
  ss_ptr->meas_list_is_diff         = TRUE;
  ss_ptr->timer_func_ptr            = timer_func_ptr;
  ss_ptr->ss_timer                  = 0;
  //ss_ptr->bad_point_update_uptime   = 0;
  ss_ptr->return_cause              = CAI_NORMAL_ACCESS_RETURN;
  ss_ptr->gps_is_opti_ss_pref       = FALSE;
  ss_ptr->is_ss_locked              = FALSE;
  ss_ptr->is_start_instance         = FALSE;

  ss_ptr->hdr_act_mask              = SD_SS_HDR_ACT_MASK_NONE;


} /* sd_ss_init */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_meas_is_done

DESCRIPTION
  Indicate whether most recent measurement request is done being serviced by
  Search.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if most recent measurement request is done being serviced by Search.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean          sd_ss_meas_is_done(

        sd_ss_e_type     ss
          /* System selection - MAIN or HDR.
          */
)
{
  sd_ss_s_type           *ss_ptr     = sd_ss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ss_ptr->meas_is_done;

} /* sd_ss_meas_is_done */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_is_get_net_done

DESCRIPTION
  Indicate whether network list was received for the last get networks
  SS-Action.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if network list was received
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean          sd_ss_is_get_net_done(

        sd_ss_e_type     ss
          /* System selection - MAIN or HDR.
          */
)
{
  sd_ss_s_type           *ss_ptr     = sd_ss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ss_ptr->is_get_net_done;

} /* sd_ss_is_get_net_done */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_update_get_net_done

DESCRIPTION
  Update the get network done flag.
  SS-Action.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if network list was received
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void             sd_ss_update_get_net_done(

        sd_ss_e_type     ss,
          /* System selection - MAIN or HDR.
          */
        boolean          flag

)
{
  sd_ss_s_type           *ss_ptr     = sd_ss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->is_get_net_done = flag;

} /* sd_ss_update_get_net_done */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_is_wlan_survey_done

DESCRIPTION
  Indicate whether network list was received for the last WLAN get networks
  SS-Action.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if network list was received
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean          sd_ss_is_wlan_survey_done(

        sd_ss_e_type     ss
          /* System selection - MAIN or HDR or HYBR_WLAN.
          */
)
{
  sd_ss_s_type           *ss_ptr     = sd_ss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ss_ptr->is_wlan_survey_done;

} /* sd_ss_is_wlan_survey_done */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_update_wlan_survey_done

DESCRIPTION
  Update the get network done flag.
  SS-Action.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if network list was received
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void             sd_ss_update_wlan_survey_done (

        sd_ss_e_type     ss,
          /* System selection - MAIN or HDR or HYBR_WLAN
          */
        boolean          flag

)
{
  sd_ss_s_type           *ss_ptr     = sd_ss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->is_wlan_survey_done = flag;

} /* sd_ss_update_wlan_survey_done */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_act_is_equal

DESCRIPTION
  Compare two SS-Actions and return TRUE if they are equal to one another.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if the SS-Action that is pointed to by the act_a_ptr is equal to the
  SS-Action that is pointed to by the act_b_ptr. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean          sd_ss_act_is_equal(

        const sd_ss_act_s_type    *act_a_ptr,
            /* Pointer to an SS-Action to be compared.
            */

        const sd_ss_act_s_type    *act_b_ptr,
            /* Pointer to an SS-Action to be compared.
            */

        boolean                   meas_list_is_diff
            /* Indicate whether the most recent list of systems to be
            ** measured is different than the previous list of systems to be
            ** measured */
)
{
  SD_ASSERT( act_a_ptr != NULL );
  SD_ASSERT( act_b_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the two SS-Actions are not the same return FALSE.
  */
  if( act_a_ptr->act != act_b_ptr->act )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the two SS-Actions are identical, so compare the
  ** action's associated parameters (for actions that have parameters).
  */
  switch( act_a_ptr->act )
  {
    case SD_SS_ACT_ACQ_CDMA:
      if( act_a_ptr->prm.acq_cdma.band != act_b_ptr->prm.acq_cdma.band ||
          act_a_ptr->prm.acq_cdma.chan != act_b_ptr->prm.acq_cdma.chan )
      {
        return FALSE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_AMPS:
      if( act_a_ptr->prm.acq_amps.band != act_b_ptr->prm.acq_amps.band ||
          act_a_ptr->prm.acq_amps.sys != act_b_ptr->prm.acq_amps.sys )
      {
        return FALSE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_HDR:
      if( act_a_ptr->prm.acq_hdr.band != act_b_ptr->prm.acq_hdr.band ||
          act_a_ptr->prm.acq_hdr.chan != act_b_ptr->prm.acq_hdr.chan ||
          act_a_ptr->prm.acq_hdr.acq_mode != act_b_ptr->prm.acq_hdr.acq_mode)
      {
        return FALSE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_MEAS_DED:
    case SD_SS_ACT_MEAS_BACK:
      /* If measurement type is different or list of channels to measure is
      ** different than SDSR_MEAS_LIST, return FALSE.
      */
      if( act_a_ptr->prm.meas.type != act_b_ptr->prm.meas.type ||
          meas_list_is_diff )
      {
        return FALSE;
      }
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_GW:
      #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
      #endif /* FEATURE_GSM || FEATURE_WCDMA */
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_WLAN
#error code not present
      #endif /* FEATURE_WLAN */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_CONTINUE:
    case SD_SS_ACT_PWR_SAVE:
    case SD_SS_ACT_ACCEPT:
    case SD_SS_ACT_REJECT:
    case SD_SS_ACT_RELEASE:
    case SD_SS_ACT_ACQ_GPS:
    case SD_SS_ACT_GET_NET_GW:
    case SD_SS_ACT_PWR_SCAN_GW:
    case SD_SS_ACT_SURVEY_WLAN:
    case SD_SS_ACT_MAX:
    default:
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the SS-Actions in question are identical - return TRUE.
  */
  return TRUE;

} /* sd_ss_act_is_equal */


#ifdef SD_USES_CAI
/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_return_cause_update

DESCRIPTION
  Update the return_cause indicator per the input SS-Event, redirection
  status, last and current systems attempted acquisition.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                          sd_ss_return_cause_update(

        sd_ss_e_type                  ss,
            /* The ss for which the return cause is to be returned
            */

        sdss_evt_e_type               event,
            /* Event that is being processed.
            */

        boolean                       is_disabling_redir,
            /* Indicates whether redirection is being disabled.
            */

        const sdsr_s_type             *last_acq_sys_ptr
            /* Pointer to system record that was attempted acquisition last.
            */
)
{
  sd_ss_s_type           *ss_ptr     = sd_ss_ptr(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX )
  SD_ASSERT( last_acq_sys_ptr != NULL )
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX )
  SD_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the return_cause indicator per the input SS-Event, and
  ** redirection status.
  */
  switch( event ) /*lint -save -e788*/
  {   /* enum constant 'sdss_evt_e_type' not used within defaulted switch */
    /* SS-Events that are a result of User Indications.
    */

    case SDSS_EVT_USER_PWR_UP:
    case SDSS_EVT_USER_OFFLINE_CDMA:
    case SDSS_EVT_USER_OFFLINE_AMPS:
    case SDSS_EVT_USER_PWR_SAVE_EXIT:
    case SDSS_EVT_USER_NAM_SEL:
    case SDSS_EVT_USER_PWR_DOWN:
      ss_ptr->return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    case SDSS_EVT_USER_SS_PREF:
    case SDSS_EVT_INT_SRV_LOST:
    case SDSS_EVT_USER_GET_NET_GW:
    case SDSS_EVT_USER_TERM_GET_NET_GW:
    case SDSS_EVT_USER_PROT_DEACTIVATE:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of CDMA Acquisition Indications.
    */

    case SDSS_EVT_CDMA_ACQ_FAILED_PILOT:
    case SDSS_EVT_CDMA_ACQ_FAILED_SYNC:
      if( is_disabling_redir ) {
        ss_ptr->return_cause = CAI_SYSTEM_NOT_ACQ_RETURN;
      }
      break;

    case SDSS_EVT_CDMA_ACQ_PROT_MIS:
      if( is_disabling_redir ) {
        ss_ptr->return_cause = CAI_PROTOCOL_MISMATCH_RETURN;
      }
      break;

    case SDSS_EVT_CDMA_ACQ_SCHM:
      if( is_disabling_redir )
      {
        /* If expected NID of last acquired system is not equal to
        ** SD_WILDCARD_NID or to obtained NID, set return_cause to
        ** CAI_WRONG_NID_RETURN.
        */
        if( last_acq_sys_ptr->sid_expected.prm.is95.nid != SD_WILDCARD_NID &&
            last_acq_sys_ptr->sid.is95.nid != last_acq_sys_ptr->sid_expected.prm.is95.nid)
        {
          ss_ptr->return_cause = CAI_WRONG_NID_RETURN;
        }

        /* If expected SID of last acquired system is not equal to
        ** SD_WILDCARD_SID or to obtained SID, set return_cause to
        ** CAI_WRONG_SID_RETURN.
        */
        if( last_acq_sys_ptr->sid_expected.prm.is95.sid != SD_WILDCARD_SID &&
            last_acq_sys_ptr->sid.is95.sid != last_acq_sys_ptr->sid_expected.prm.is95.sid)
        {
          ss_ptr->return_cause = CAI_WRONG_SID_RETURN;
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_CDMA_ACQ_HO_FAIL:
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of CDMA Operation Indications.
    */

    case SDSS_EVT_CDMA_OPR_OH_SID:
    case SDSS_EVT_CDMA_OPR_OH_CHAN:
    case SDSS_EVT_CDMA_OPR_RESCAN:
      break;

    case SDSS_EVT_CDMA_OPR_REG_REJ:
      if( is_disabling_redir ) {
        ss_ptr->return_cause = CAI_REG_REJ_RETURN;
      }
      break;

    case SDSS_EVT_CDMA_OPR_SYS_LOST:
    case SDSS_EVT_CDMA_OPR_REDIR:
    case SDSS_EVT_CDMA_OPR_ACPT_REJ:
    case SDSS_EVT_CDMA_OPR_NDSS_OFF:
    case SDSS_EVT_CDMA_OPR_NEW_SYS:
    case SDSS_EVT_CDMA_OPR_SYS_RESEL:
    case SDSS_EVT_CDMA_OPR_LOCK:
      ss_ptr->return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    case SDSS_EVT_CDMA_OPR_UNLOCK:
    case SDSS_EVT_CDMA_OPR_ACC_TERM:
    case SDSS_EVT_CDMA_OPR_CAM_ERR:
    case SDSS_EVT_CDMA_OPR_CAM:
    case SDSS_EVT_CDMA_OPR_HDM:
    case SDSS_EVT_CDMA_OPR_ITSPM:
      break;

    case SDSS_EVT_CDMA_OPR_PROT_MIS:
      ss_ptr->return_cause = CAI_PROTOCOL_MISMATCH_RETURN;
      break;
      
    case SDSS_EVT_CDMA_OPR_RELEASE:
      ss_ptr->return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of AMPS Acquisition Indications.
    */

    case SDSS_EVT_AMPS_ACQ_FAILED_CCH:
      if( is_disabling_redir ) {
        ss_ptr->return_cause = CAI_SYSTEM_NOT_ACQ_RETURN;
      }
      break;

    case SDSS_EVT_AMPS_ACQ_CCH_SID:
      if( is_disabling_redir )
      {
        /* If expected SID of last acquired system is not equal to
        ** SD_WILDCARD_SID or to obtained SID, set return_cause to
        ** CAI_WRONG_SID_RETURN.
        */
        if( last_acq_sys_ptr->sid_expected.prm.is95.sid != SD_WILDCARD_SID &&
            last_acq_sys_ptr->sid.is95.sid != last_acq_sys_ptr->sid_expected.prm.is95.sid)
        {
          ss_ptr->return_cause = CAI_WRONG_SID_RETURN;
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of AMPS Operation Indications.
    */

    case SDSS_EVT_AMPS_OPR_FAILED_PCH:
      if( is_disabling_redir ) {
        ss_ptr->return_cause = CAI_SYSTEM_NOT_ACQ_RETURN;
      }
      break;

    case SDSS_EVT_AMPS_OPR_PCH_SID:
      if( is_disabling_redir )
      {
        /* If expected SID of last acquired system is not equal to
        ** SD_WILDCARD_SID or to obtained SID, set return_cause to
        ** CAI_WRONG_SID_RETURN.
        */
        if( last_acq_sys_ptr->sid_expected.prm.is95.sid != SD_WILDCARD_SID &&
            last_acq_sys_ptr->sid.is95.sid != last_acq_sys_ptr->sid_expected.prm.is95.sid)
        {
          ss_ptr->return_cause = CAI_WRONG_SID_RETURN;
        }
      }
      break;

    case SDSS_EVT_AMPS_OPR_ACC_SID:
      break;

    case SDSS_EVT_AMPS_OPR_RESCAN:
    case SDSS_EVT_AMPS_OPR_SYS_LOST:
    case SDSS_EVT_AMPS_OPR_CDMA_AVAIL:
      ss_ptr->return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    case SDSS_EVT_AMPS_OPR_ACC_TERM:
    case SDSS_EVT_AMPS_OPR_CAM:
      break;

    case SDSS_EVT_AMPS_OPR_OVERLOAD:
      ss_ptr->return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of HDR User Indications.
    */
    case SDSS_EVT_HDR_USER_SS_PREF:
    case SDSS_EVT_HDR_USER_HYBR_START:
    case SDSS_EVT_HDR_USER_HYBR_END:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of HDR Acquisition Indications.
    */
    case SDSS_EVT_HDR_ACQ_FAILED_PILOT:
    case SDSS_EVT_HDR_ACQ_FAILED_SYNC:
    case SDSS_EVT_HDR_ACQ_PROT_MIS:
    case SDSS_EVT_HDR_ACQ_BAD_SYS:
    case SDSS_EVT_HDR_ACQ_SID:
      ss_ptr->return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of HDR Operation Indications.
    */

    case SDSS_EVT_HDR_OPR_OH_SID:
    case SDSS_EVT_HDR_OPR_OH_CHAN:
    case SDSS_EVT_HDR_OPR_REDIR:
    case SDSS_EVT_HDR_OPR_SYS_LOST:
    case SDSS_EVT_HDR_OPR_RESEL_OK:
    case SDSS_EVT_HDR_OPR_ACC_TERM:
    case SDSS_EVT_HDR_OPR_SES_ABORT:
    case SDSS_EVT_HDR_OPR_SES_CLOSE:
    case SDSS_EVT_HDR_OPR_SES_NEGO_TIMEOUT:
    case SDSS_EVT_HDR_OPR_RELEASE:
    case SDSS_EVT_HDR_OPR_END_BCMCS:
      ss_ptr->return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    case SDSS_EVT_HDR_OPR_PROT_MIS:
      ss_ptr->return_cause = CAI_PROTOCOL_MISMATCH_RETURN;
      break;

    case SDSS_EVT_HDR_OPR_BAD_SYS:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
    case SDSS_EVT_HYBR_1_USER_SS_PREF:
    case SDSS_EVT_HYBR_1_USER_HYBR_START:
    case SDSS_EVT_HYBR_1_USER_HYBR_END:
      break;
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
    #endif /* FEATURE_GSM || FEATURE_WCDMA */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #if defined(FEATURE_WLAN)
#error code not present
    #endif /* FEATURE_WLAN */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of Miscellaneous Indications.
    */

    case SDSS_EVT_MISC_TIMER_RESEL_OK:
    case SDSS_EVT_MISC_TIMER_RESEL_NO:
    case SDSS_EVT_MISC_MEAS_RPRT:
    case SDSS_EVT_MISC_ORIG_SUCCESS:
      break;

    case SDSS_EVT_MISC_PWR_SAVE_ENTER:
      ss_ptr->return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    case SDSS_EVT_MISC_FILLER:
    case SDSS_EVT_MISC_GW_NET_LIST:
    case SDSS_EVT_MISC_WLAN_SURVEY_LIST:
    case SDSS_EVT_MISC_CNF:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are internally generated by the SS-Core.
    */
    case SDSS_EVT_INT_RESEL:
      SD_CASE_ERR( "ss_event",0,0,0 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_MAX:
    default:
      SD_CASE_ERR( "ss_event",0,0,0 );
      break;

  } /* switch */ /*lint -restore*/

} /* sd_ss_return_cause_update */
#endif  /* defined(SD_USES_CAI) */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_update_manual_sys

DESCRIPTION
  Update the return_cause indicator per the input SS-Event, redirection
  status, last and current systems attempted acquisition.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                                sd_update_manual_sys(

        sdsr_e_type                         lst,
            /* List to be updated
            */

        const sys_manual_sys_info_s_type    *man_sys_info_ptr
            /* Pointer to the manual system.
            */

)
{
  /* Point at manual system record.
  */
  sdsr_s_type            *man_sr_ptr;
  int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( lst, SDSR_MAX );
  SD_ASSERT( man_sys_info_ptr != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* User has changed the network selection preference and made it
  ** manual. Update the plmn information.
  */
  for( i=0; i < sdsr_list_cnt(lst); i++ )
  {
    man_sr_ptr = sdsr_list_get_ptr( lst, i );
    if (man_sr_ptr == NULL) 
    {
      SD_MSG_LOW("man_sr_ptr NULL",0,0,0);
      continue;
    }

    /* Check if it GW system.
    */
    switch ( man_sr_ptr->sys.mode )
    {
      #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
      case SD_MODE_GSM:
      case SD_MODE_WCDMA:
      case SD_MODE_GW:
        if ( man_sys_info_ptr->sys_id.id_type != SYS_SYS_ID_TYPE_UMTS )
        {
          break;
        }

        switch ( man_sys_info_ptr->prm.gw.sel_type )
        {
          case SYS_MANUAL_SEL_TYPE_HPLMN:
            man_sr_ptr->sid_expected.net_select_mode
                                      = SD_NET_SELECT_MODE_HPLMN;
            break;

          case SYS_MANUAL_SEL_TYPE_RPLMN:
            man_sr_ptr->sid_expected.net_select_mode
                                      = SD_NET_SELECT_MODE_MANUAL_SYS;
            sys_plmn_undefine_plmn_id
            (
              &man_sr_ptr->sid_expected.prm.plmn.plmn_id
            );
            break;

          case SYS_MANUAL_SEL_TYPE_USER_SPECIFIED:
            man_sr_ptr->sid_expected.net_select_mode
                                      = SD_NET_SELECT_MODE_MANUAL_SYS;
            man_sr_ptr->sid_expected.prm.plmn.plmn_id
                                      = man_sys_info_ptr->sys_id.id.plmn;
            break;

          case SYS_MANUAL_SEL_TYPE_NONE:
          case SYS_MANUAL_SEL_TYPE_MAX:
          default:
            SD_ERR_FATAL("Invalid net sel Type", 0, 0, 0 );

        } /* switch ( manual_sys_info_ptr->prm.gw.sel_type ) */
        man_sr_ptr->prl = (byte)SDSS_PRL_MOST_PREF;
        break;
        #endif /* FEATURE_GSM || FEATURE_WCDMA */

        #ifdef FEATURE_WLAN
#error code not present
        #endif /* FEATURE_WLAN */

      case SYS_SYS_MODE_NONE:
      case SYS_SYS_MODE_NO_SRV:
      case SYS_SYS_MODE_AMPS:
      case SYS_SYS_MODE_CDMA:
      case SYS_SYS_MODE_HDR:
      case SYS_SYS_MODE_GPS:
      case SYS_SYS_MODE_MAX:
      default:
        SD_MSG_HIGH("invalid manual mode %d", man_sr_ptr->sys.mode, 0, 0 );
        break;

    } /* switch () */

  } /* for ( ... */
#if !(defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW) \
    || defined(FEATURE_WLAN))
SYS_ARG_NOT_USED(man_sys_info_ptr);
#endif
} /* sd_update_manual_sys */


/* <EJECT> */
//*===========================================================================
//
//FUNCTION sd_ss_bad_point_update
//
//DESCRIPTION
//  Update the bad points count of all system records.
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
//static  void                          sd_ss_bad_point_update( void )
//{
//  int           i;
//  sdsr_s_type   *sr_ptr;
//  dword         time_delta;
//  sd_ss_s_type  *ss_ptr       = &sd_ss;
//  dword         uptime        = clk_uptime();
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  SD_ASSERT( uptime > ss_ptr->bad_point_update_uptime );
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  /* Calculate the time delta since last update. If this time delta is less
//  ** than 1 minute, just return.
//  */
//  if( uptime < ss_ptr->bad_point_update_uptime )
//  {
//    time_delta = 0;
//  }
//  else
//  {
//    time_delta = (uptime - ss_ptr->bad_point_update_uptime)/60;
//  }
//
//  //if( time_delta <= 0 )
//  //{
//  //  return;
//  //}
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  /* Set the bad point count to 7/(8*(1+time_delta)) of its current value.
//  */
//  for( i=0; (sr_ptr=sdsr_list_get_ptr(SDSR_TBL_LST, i)) != NULL; i++ )
//  {
//    dword   val = sr_ptr->bad_point_cnt;
//
//    sr_ptr->bad_point_cnt = (byte) ( (7*val)/(8*(1+time_delta)) );
//  }
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  ss_ptr->bad_point_update_uptime = uptime - uptime%60;
//}




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_idle_backstop_sys_update

DESCRIPTION
  Update the idle AMPS backstop system per the idle system.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                          sd_ss_idle_backstop_sys_update( void )
{
  sdsr_s_type       *idl_sys_ptr;
  sd_blksys_e_type  blksys;
  sdsr_e_type       amps_sys;
  sdsr_ref_type     amps_sys_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the idle AMPS backstop system.
  */
  sdsr_list_clr( SDSR_AMPS_BSIDL_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If AMPS backstop is turned OFF, return now.
  */
  if( ! sdprl_is_amps_backstop() )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here AMPS backstop is turned ON.
  */

  /* Point at idle system.
  */
  idl_sys_ptr = sdsr_list_get_ptr( SDSR_IDL_SYS, 0 );
  if(idl_sys_ptr == NULL)
  {
    SD_MSG_LOW("idl_sys_ptr NULL",0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the cellular side of the idle system. If idle system is not
  ** associated with a cellular side, return now.
  */
  blksys = sdprl_map_chan_to_blksys( idl_sys_ptr->sys.mode,
          sdss_map_band_pref_to_band(idl_sys_ptr->sys.band),
                                     idl_sys_ptr->sys.chan );

  if( blksys == SD_BLKSYS_CELL_A ) {
    amps_sys = SDSR_AMPS_A_SYS;
  }
  else if( blksys == SD_BLKSYS_CELL_B ) {
    amps_sys = SDSR_AMPS_B_SYS;
  }
  else {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, AMPS backstop is turned on and the idle system is
  ** associated with a cellular side.
  */

  /* If idle system is a PRL listed system, and the idle GEO contains an
  ** AMPS system of the same cellular side as the idle system, set the idle
  ** AMPS backstop system to this AMPS system.
  */
  if( sdss_sr_is_prl(idl_sys_ptr, SDSS_PRL_VAL_LISTED) )
  {
    if( sdsr_list_is_contain(SDSR_IDL_GEO_LST,
                             SDSR_CONTAIN_ALL,
                             amps_sys ))
    {
      sdsr_list_cpy( SDSR_AMPS_BSIDL_SYS, amps_sys );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (the idle system is an available system), if the PRL contains an
  ** AMPS system of the same cellular side as the idle system, set the idle
  ** AMPS backstop system to this AMPS system.
  */
  else
  {
    amps_sys_ref = sdsr_list_get_ref( amps_sys, 0 );

    if( amps_sys_ref == sdsr_list_get_ref(SDSR_AMPS_1ST_SYS, 0) ||
        amps_sys_ref == sdsr_list_get_ref(SDSR_AMPS_2ND_SYS, 0) )
    {
      sdsr_list_cpy( SDSR_AMPS_BSIDL_SYS, amps_sys );
    }
  }

} /* sd_ss_idle_backstop_sys_update */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_idle_sys_update

DESCRIPTION
  Update the idle system, its GEO list and PREF lists per the input lists
  as necessary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                          sd_ss_idle_sys_update(

        sd_ss_e_type                  ss,
            /* The ss for which the idle system is to be updated
            */

        sdsr_e_type                   sys,
            /* System per which to update the idle system.
            ** Ignored if set to SDSR_MAX.
            */

        sdsr_e_type                   geo_list,
            /* List per which to update the GEO list.
            */

        sdsr_e_type                   pref_list
            /* List per which to update the PREF list.
            */
)
{
  sdsr_ref_type     sr_ref;
  sdsr_s_type       *sys_ptr;
  boolean           is_update_idle_sys  = FALSE;
  boolean           is_gw_sys           = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( BETWEEN(sys, SDSR_1ST_SYS, SDSR_MAX) || sys == SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( geo_list, SDSR_1ST_SYS);
  SD_ASSERT_ENUM_IS_INRANGE( pref_list, SDSR_1ST_SYS);
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sys != SDSR_MAX )
  {
    sys_ptr = sdsr_list_get_ptr( sdss_sr_list_map2(ss, sys), 0 );
  }
  else
  {
    sys_ptr = NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Determine whether we need to update the idle system.
  */

  /* If handoff designation is none (i.e., MS is not engaged in a call or
  ** call setup activity), update the idle system.
  */
  if( sdss_is_handoff(SDSS_HO_VAL_NONE) )
  {
    is_update_idle_sys = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else if handoff is CAM to CDMA paging, the system is allowed by the PRL,
  ** complies with the current SS-Preference, and explicitly listed in the
  ** PRL's acquisition, update the idle system.
  */
  else if( sdss_is_handoff(SDSS_HO_VAL_CDMA_CAM_TO_CDMA_PAG) &&
           sys_ptr != NULL &&
           sdss_sr_is_prl(sys_ptr, SDSS_PRL_VAL_ALLOWED) &&
           sdss_sr_is_pref_conf(ss, sys_ptr, SDSS_PREF_CONF_NONE) &&
           sdsr_list_find(SDSR_PRL_LST, sys_ptr) != SDSR_POS_NULL )
  {
    is_update_idle_sys = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If need to update the idle system, update the following system record
  ** lists:
  **
  ** 1. The SDSR_IDL_SYS.
  ** 2. The SDSR_IDL_GEO_LST.
  ** 3. The SDSR_IDL_REF_LST.
  ** 4. The SDSR_MRU_LST.
  */
  if( is_update_idle_sys )
  {
    sdsr_list_cpy( sdss_sr_list_map2(ss, SDSR_IDL_GEO_LST ),
                   sdss_sr_list_map2(ss, geo_list) );

    sdsr_list_cpy( sdss_sr_list_map2(ss, SDSR_IDL_PREF_LST),
                   sdss_sr_list_map2(ss, pref_list) );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If sys is not to be ignored, do the following:
    */
    if( sys != SDSR_MAX )
    {
      /* Update the idle system per sys.
      */
      sdsr_list_cpy( sdss_sr_list_map2( ss, SDSR_IDL_SYS ),
                     sdss_sr_list_map2( ss, sys ) );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Insert system to beginning of MRU list (remove duplicates).
      */
      sr_ref = sdsr_list_get_ref( sdss_sr_list_map2( ss, sys ), 0 );
      SD_ASSERT( sr_ref != SDSR_REF_NULL )

      /* Update the last user system that contains the same bands as that in
      ** in the idle system.
      */
      sdsr_list_update_gw_sys( ss, sr_ref, TRUE, TRUE, TRUE );

      /* Change the system to be logged if the system is a GW system.
      */
      if( sys_ptr->sys.mode == SD_MODE_GSM   ||
          sys_ptr->sys.mode == SD_MODE_WCDMA ||
          sys_ptr->sys.mode == SD_MODE_GW       )
      {
        sr_ref = sdsr_list_get_ref( sdss_sr_list_map2(ss, SDSR_GW_LAST_FULL_SYS ), 0 );
        is_gw_sys = TRUE;
      }
      /* During emergency origination/callback, only insert system to
      ** beginning MRU if the system is found on the MRU, PRL or REDIR list.
      **
      ** Do not update MRU if origination mode is manul mode.
      ** Do not update MRU if acquired system is a GW system and doesn't
      ** provide full service.
      */
      if( (! sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_EMERG) ||
           sdsr_list_find_ref(sdss_sr_list_map2(ss, SDSR_MRU_LST ), sr_ref) != SDSR_POS_NULL ||
           sdsr_list_find_ref(SDSR_PRL_LST, sr_ref) != SDSR_POS_NULL ||
           sdsr_list_find_ref(sdss_sr_list_map2(ss, SDSR_REDIR_LST ), sr_ref) != SDSR_POS_NULL)
                           &&
           ! sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_MANUAL)
                           &&
          ( !is_gw_sys ||
            sdss_get_srv_status(ss) == SYS_SRV_STATUS_SRV )


        )
      {
        #ifdef FEATURE_JCDMA
        /* If JCDMA is defined, only insert to the MRU systems that are
        ** explicitly listed in the PRL.
        */
        if( sdsr_list_find_ref(SDSR_PRL_LST, sr_ref) != SDSR_POS_NULL ||
            sys_ptr->sys.mode == SD_MODE_GSM                          ||
            sys_ptr->sys.mode == SD_MODE_WCDMA                        ||
            sys_ptr->sys.mode == SD_MODE_GW )
        #endif
        {
          sdsr_list_insert_ref( sdss_sr_list_map2(ss, SDSR_MRU_LST ), 0, sr_ref, TRUE );
        }
      }

    } /* if( sys != SDSR_MAX ) */

  } /* if( not in handoff ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the idle AMPS backstop system.
  */
  sd_ss_idle_backstop_sys_update();

} /* sd_ss_idle_sys_update */


#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */


#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_srv_sys_update

DESCRIPTION
  Update the serving system, its GEO, and its more preferred lists per the
  input lists.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                          sd_ss_srv_sys_update(

        sd_ss_e_type                  ss,
            /* The ss for which the srv_sys is to be updated
            */

        sdsr_e_type                   sys,
            /* System per which to update the serving system.
            ** Ignored if set to SDSR_MAX.
            */

        sdsr_e_type                   geo_list,
            /* List per which to update the serving system's GEO.
            */

        sdsr_e_type                   pref_list,
            /* List per which to update the serving system's more preferred
            ** list.
            */

        sdsr_e_type                   colloc_list
            /* List per which to update the serving system's collocated list.
            */
)
{
  SD_ASSERT( BETWEEN(sys, SDSR_1ST_SYS, SDSR_MAX) || sys == SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( geo_list, SDSR_1ST_SYS);
  SD_ASSERT_ENUM_IS_INRANGE( pref_list, SDSR_1ST_SYS);
  SD_ASSERT_ENUM_IS_INRANGE_OR( colloc_list, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If needed, update the serving system.
  */
  if( sys != SDSR_MAX )
  {
    sdsr_list_cpy( sdss_sr_list_map2( ss, SDSR_SRV_SYS ),
                   sdss_sr_list_map2( ss, sys) );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the serving system's, GEO and more preferred lists.
  */
  sdsr_list_cpy( sdss_sr_list_map2( ss, SDSR_SRV_GEO_LST),
                 sdss_sr_list_map2( ss, geo_list ) );

  sdsr_list_cpy( sdss_sr_list_map2( ss, SDSR_SRV_PREF_LST),
                 sdss_sr_list_map2( ss, pref_list ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If needed, update the serving systems collocated list.
  */
  if( colloc_list != SDSR_MAX )
  {
    sdsr_list_cpy( sdss_sr_list_map2( ss, SDSR_SRV_COLLOC_LST),
                   sdss_sr_list_map2( ss, colloc_list) );
  }

} /* sd_ss_srv_sys_update */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */


#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_HDR */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_acq_update_pre

DESCRIPTION
  Does pre acquisition attempt updates.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                          sd_ss_acq_update_pre(

        sd_ss_e_type                  ss,
            /* The SS for which the acq is to be updated
            */

        sdsr_e_type                   list,
            /* List from which acquisition is attempted.
            */
        int                           pos,
            /* List position of system that is attempted acquisition.
            */

        sdss_acq_mode_e_type          acq_mode
            /* The acquisition mode that is attempted.
            */
)
{
  sd_ss_s_type          *ss_ptr               = sd_ss_ptr(ss);

  sdsr_ref_type         sr_ref;
  #ifdef SD_USES_DB_SERVICES
  db_items_value_type   db_item;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acq_mode, SDSS_ACQ_MODE_MAX );
  SD_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a reference to the system that is attempted acquisition.
  */
  sr_ref = sdsr_list_get_ref( list, pos );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update SDSR_ACQ_SYS per the system record that is attempted
  ** acquisition.
  */
  sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_SYS), sr_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the acquisition list, position and mode.
  */
  ss_ptr->acq_list  = list;
  ss_ptr->acq_pos   = pos;
  ss_ptr->acq_mode  = acq_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert SD acquisition mode to DB acquisition mode.
  */
  #ifdef SD_USES_DB_SERVICES
  if( ss == SD_SS_MAIN )
  {
    switch( acq_mode )
    {
      case SDSS_ACQ_MODE_FULL:
      case SDSS_ACQ_MODE_FULL2:
      case SDSS_ACQ_MODE_UNIFORM_FULL_RTN:
      case SDSS_ACQ_MODE_UNIFORM_FULL_RTN2:
        db_item.acq_mode = DB_ACQ_FULL;
        break;

      case SDSS_ACQ_MODE_MINI:
      case SDSS_ACQ_MODE_MINI2:
        db_item.acq_mode = DB_ACQ_MINI;
        break;

      case SDSS_ACQ_MODE_MICRO:
      case SDSS_ACQ_MODE_MICRO2:
        db_item.acq_mode = DB_ACQ_MICRO;
        break;

      case SDSS_ACQ_MODE_DEEP:
      case SDSS_ACQ_MODE_DEEP2:
      case SDSS_ACQ_MODE_SHALLOW:
      case SDSS_ACQ_MODE_SHALLOW2:
        /* For HDR_Only mode, we have Deep scans, map these to mini
        */
        db_item.acq_mode = DB_ACQ_MINI;
        break;

      case SDSS_ACQ_MODE_MAX:
      default:
        SD_CASE_ERR( "acq_mode",0,0,0 );
        db_item.acq_mode = DB_ACQ_FULL;
        break;

    } /* switch */
  } /* if */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Tell Search about the mode of the attempted acquisition by deposition
  ** the acquisition mode value in the DB.
  */
  db_put( DB_ACQ_MODE, &db_item );
  #endif

} /* sd_ss_acq_update_pre */

#if (defined (FEATURE_HDR_HYBRID) && !defined(FEATURE_TECH_IND_SD_STACK_ARCH ))
#error code not present
#endif /* FEATURE_HDR_HYBRID */


#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_acq_update_post

DESCRIPTION
  Does post acquisition attempt updates.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                          sd_ss_acq_update_post(

        sd_ss_e_type                  ss,
            /* The SS for which the acq is to be updated
            */

        sdsr_e_type                   list,
            /* List from which acquisition was attempted.
            */
        int                           pos,
            /* List position of system that was attempted acquisition.
            */

        sdss_acq_mode_e_type          acq_mode
            /* The acquisition mode that was attempted.
            */
)
{
  sdsr_ref_type         sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acq_mode, SDSS_ACQ_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a reference to the system that was attempted acquisition.
  */
  sr_ref = sdsr_list_get_ref( list, pos );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the last full, limited systems that contain the same
  ** bands as that in the acq system.
  */
  sdsr_list_update_gw_sys( ss, sr_ref, TRUE, FALSE, FALSE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If origination mode is emergency (origination or callback) and list is
  ** other than SCHED_LST (i.e., this acquisition attempt was not due to
  ** reacquisition schedule), update the EMERG_SYS per the system record
  ** that was attempted acquisition.
  */
  if( sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_EMERG) &&
      list != sdss_sr_list_map2(ss,SDSR_SCHED_LST) )
  {
    sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_EMERG_SYS), sr_ref );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* According to the acquisition mode that was attempted, update
  ** the appropriate SDSR_ACQ_xxx_SYS system.
  */
  switch( acq_mode )
  {
    case SDSS_ACQ_MODE_FULL:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_FULL_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_MINI:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_MINI_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_MICRO:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_MICRO_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_FULL2:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_FULL2_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_MINI2:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_MINI2_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_MICRO2:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_MICRO2_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_DEEP:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_DEEP_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_DEEP2:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_DEEP2_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_SHALLOW:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_SHALLOW_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_SHALLOW2:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_SHALLOW2_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_UNIFORM_FULL_RTN:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_UNIFORM_FULL_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_UNIFORM_FULL_RTN2:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_FULL2_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_MAX:
    default:
      SD_CASE_ERR( "acq_mode",0,0,0 );
      break;

  } /* switch */

} /* sd_ss_acq_update_post */


#if (defined (FEATURE_HDR_HYBRID) && !defined(FEATURE_TECH_IND_SD_STACK_ARCH ))
#error code not present
#endif /* FEATURE_HDR_HYBRID */


#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */


#ifdef FEATURE_HDR
#error code not present
#endif /* defined(FEATURE_HDR) */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_timer_rest

DESCRIPTION
  Reset the SD timer

EPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_ss_timer_reset(
        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  SD_ASSERT( ss < SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset the timer value and disable the SS-timer.
  */
  sd_ss_ptr(ss)->ss_timer = 0;

  sd_ss_ptr(ss)->timer_func_ptr(0);

}/* sd_ss_timer_reset */

/*===========================================================================

FUNCTION sd_ss_get_timer

DESCRIPTION
  Get the SD timer

EPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  dword                      sd_ss_get_timer(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  SD_ASSERT( ss < SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset the timer value and disable the SS-timer.
  */
  return (sd_ss_ptr(ss)->ss_timer); 

}/* sd_ss_get_timer */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_act_get

DESCRIPTION
  Return the SS-Action that was most recently returned to the SS-Client
  other than CONTINUE.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_s_type           *ss_ptr     = sd_ss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If caller supplied a buffer, copy the most recent SS-Action into
  ** into the supplied buffer.
  */
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = ss_ptr->true_curr_act;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return most recent SS-Action.
  */
  return ss_ptr->true_curr_act.act;

} /* sd_ss_act_get */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_act_get_true_curr_act

DESCRIPTION
  Return the true SS-Action that was most recently returned to the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{


  sd_ss_s_type           *ss_ptr     = sd_ss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If caller supplied a buffer, copy the most recent SS-Action into
  ** into the supplied buffer.
  */
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = ss_ptr->act;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return most recent SS-Action.
  */
  return ss_ptr->act.act;

} /* sd_ss_act_get */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_hdr_act_get

DESCRIPTION
  Return to the HDR client the true SS-Action that was most recently returned to
  the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_hdr_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  #ifndef FEATURE_HDR_HYBRID

  return sd_ss_act_get( rtrn_act_ptr );

  #else /* FEATURE_HDR_HYBRID */
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

} /* sd_ss_hdr_act_get */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_hdr_act_get_true_curr_act

DESCRIPTION
  Return to the HDR client the SS-Action that was most recently returned to
  the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_hdr_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  #ifndef FEATURE_HDR_HYBRID

  return sd_ss_act_get_true_curr_act( rtrn_act_ptr );

  #else /* FEATURE_HDR_HYBRID */
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

} /* sd_ss_hdr_act_get_true_curr_act */

/*===========================================================================

FUNCTION sd_ss_hybr_1_act_get

DESCRIPTION
  Return to the HDR client the true SS-Action that was most recently returned to
  the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_gw_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  #if !defined(FEATURE_TECH_IND_SD_STACK_ARCH)

  return sd_ss_act_get( rtrn_act_ptr );

  #else /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sd_ss_s_type           *ss_ptr     = sd_ss_ptr(SD_SS_HYBR_1);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If currently not doing CDMA+WCDMA hybrid operation, call on
  ** sd_ss_act_get().
  */
  if( sdss_get_ss_for_mode( SD_MODE_GW ) == SD_SS_MAIN )
  {
    return sd_ss_act_get( rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If caller supplied a buffer, copy the most recent SS-Action into
  ** into the supplied buffer.
  */
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = ss_ptr->true_curr_act;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return most recent SS-Action.
  */
  return ss_ptr->true_curr_act.act;

  #endif /* FEATURE_HDR_HYBRID */

} /* sd_ss_hdr_act_get */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_gw_act_get_true_curr_act

DESCRIPTION
  Return to the HYBR_1 client the SS-Action that was most recently returned to
  the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_gw_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  #ifndef FEATURE_TECH_IND_SD_STACK_ARCH

  return sd_ss_act_get_true_curr_act( rtrn_act_ptr );

  #else /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sd_ss_s_type           *ss_ptr     = sd_ss_ptr(SD_SS_HYBR_1);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If currently not doing CDMA+WCDMA hybrid operation, call on
  ** sd_ss_act_get().
  */
  if( sdss_get_ss_for_mode( SD_MODE_GW ) == SD_SS_MAIN )
  {
    return sd_ss_act_get_true_curr_act( rtrn_act_ptr );
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If caller supplied a buffer, copy the most recent SS-Action into
  ** into the supplied buffer.
  */
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = ss_ptr->act;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return most recent SS-Action.
  */
  return ss_ptr->act.act;

  #endif /* FEATURE_HDR_HYBRID */

} /* sd_ss_hybr_1_act_get_true_curr_act */



/*===========================================================================

FUNCTION sd_ss_hybr_1_act_get

DESCRIPTION
  Return to the HDR client the true SS-Action that was most recently returned to
  the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_hybr_1_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  #ifndef FEATURE_TECH_IND_SD_STACK_ARCH

  return sd_ss_hdr_act_get( rtrn_act_ptr );

  #else /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sd_ss_s_type           *ss_ptr     = sd_ss_ptr(SD_SS_HYBR_1);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If currently not doing CDMA+HDR hybrid operation, call on
  ** sd_ss_act_get().
  */
  if( sdss_is_hybr_pref( SD_SS_HYBR_PREF_NONE) )
  {
    SD_ERR_FATAL("Invalid sd_ss_hybr_1_act_get",0,0,0);
    /*
    **return (sd_ss_act_get( rtrn_act_ptr ));
    */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If caller supplied a buffer, copy the most recent SS-Action into
  ** into the supplied buffer.
  */
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = ss_ptr->true_curr_act;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return most recent SS-Action.
  */
  return ss_ptr->true_curr_act.act;

  #endif /* FEATURE_HDR_HYBRID */

} /* sd_ss_hdr_act_get */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_hybr_1_act_get_true_curr_act

DESCRIPTION
  Return to the HYBR_1 client the SS-Action that was most recently returned to
  the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_hybr_1_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  #ifndef FEATURE_TECH_IND_SD_STACK_ARCH

  return sd_ss_act_get_true_curr_act( rtrn_act_ptr );

  #else /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sd_ss_s_type           *ss_ptr     = sd_ss_ptr(SD_SS_HYBR_1);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If currently not doing CDMA+HDR hybrid operation, call on
  ** sd_ss_act_get().
  */
  if( sdss_is_hybr_pref( SD_SS_HYBR_PREF_NONE) )
  {
    SD_ERR_FATAL("Invalid sd_ss_hybr_1_act_get",0,0,0);
    /*
    **return sd_ss_act_get_true_curr_act( rtrn_act_ptr );
    */
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If caller supplied a buffer, copy the most recent SS-Action into
  ** into the supplied buffer.
  */
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = ss_ptr->act;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return most recent SS-Action.
  */
  return ss_ptr->act.act;

  #endif /* FEATURE_HDR_HYBRID */

} /* sd_ss_hybr_1_act_get_true_curr_act */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_wlan_act_get

DESCRIPTION
  Return to the HYBR_WLAN client the true SS-Action that was most recently returned to
  the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_wlan_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  #ifndef FEATURE_WLAN

  return sd_ss_act_get( rtrn_act_ptr );

  #else /* FEATURE_HDR_HYBRID */
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

} /* sd_ss_wlan_act_get */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_wlan_act_get_true_curr_act

DESCRIPTION
  Return to the HYBR_WLAN client the SS-Action that was most recently returned to
  the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_wlan_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  #ifndef FEATURE_WLAN

  return sd_ss_act_get_true_curr_act( rtrn_act_ptr );

  #else /* FEATURE_HDR_HYBRID */
#error code not present
  #endif /* FEATURE_WLAN */

} /* sd_ss_wlan_act_get_true_curr_act */




#ifdef FEATURE_SD20_LOGGING
/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_log_last_action

DESCRIPTION
  Logs information about System Determination state and variables

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sd_ss_log_last_action(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  sd_si_s_type          *si_ptr     = sd_si_ptr( ss );
  sd_ss_s_type          *ss_ptr     = sd_ss_ptr( ss );
  sd_ss_act_s_type      *act_ptr    = &ss_ptr->act;

  log_code_type         log_code    = LOG_SD20_LAST_ACTION_C;
    /* Code to be logged.
    */

  unsigned int          log_pkt_len = sizeof( LOG_SD20_LAST_ACTION_C_type );
    /* Length of packet to be logged.
    */

  LOG_SD20_LAST_ACTION_C_type *log_ptr  = NULL;
    /* log pointer to the log record type.
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If logging for HDR, use appropriate log code.
  */
  if( ss == SD_SS_HYBR_HDR || ss == SD_SS_HYBR_1 )
  {
    log_code = LOG_SD20_LAST_ACTION_HYBRID_C;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we don't log an HDR UATI, we can log a 12 byte smaller packet).
  */
  if( si_ptr->si_info.mode != SD_MODE_HDR )
  {
    log_pkt_len -= 12;
  }

  /* Allocate a log packet.
  */
  log_ptr = (LOG_SD20_LAST_ACTION_C_type *) log_alloc_ex( log_code,
                                                          log_pkt_len );
  /* If allocation failed, just return.
  */
  if ( log_ptr == NULL )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy SS-Action related information into the log packet.
  */
  log_ptr->act = act_ptr->act;

  switch( act_ptr->act )
  {
    case SD_SS_ACT_ACQ_CDMA:
      log_ptr->acq_band                 = act_ptr->prm.acq_cdma.band;
      log_ptr->acq_channel              = act_ptr->prm.acq_cdma.chan;
      log_ptr->act_info.acq_cdma.blksys = act_ptr->prm.acq_cdma.blksys;
      log_ptr->act_info.acq_cdma.is_new_mode_or_blksys =
        act_ptr->prm.acq_cdma.is_new_mode_or_blksys;
      break;

    case SD_SS_ACT_ACQ_AMPS:
      log_ptr->acq_band         = act_ptr->prm.acq_amps.band;
      log_ptr->acq_channel      = act_ptr->prm.acq_amps.sys;
      log_ptr->act_info.acq_amps.is_amps_2nd_cch =
        act_ptr->prm.acq_amps.is_amps_2nd_cch;
      log_ptr->act_info.acq_amps.max_redirect_delay =
        act_ptr->prm.acq_amps.max_redirect_delay;
      break;

    case SD_SS_ACT_ACQ_HDR:
      log_ptr->acq_band         = act_ptr->prm.acq_hdr.band;
      log_ptr->acq_channel      = act_ptr->prm.acq_hdr.chan;
      break;

    case SD_SS_ACT_MEAS_DED:
      log_ptr->act_info.ded_meas.meas_type = act_ptr->prm.meas.type;
      log_ptr->act_info.ded_meas.meas_id   = act_ptr->prm.meas.id;
      break;

    default:
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy SS-State related information into the log packet.
  */
  if( ss_ptr->ss_state == SDSS_STATE_ACQ )
  {
    switch (sdss_eng_stack_get_state( ss ))
    {
      case SDSS_ACQ_STT_START:
        log_ptr->sd_state = SD_LOG_STATE_ACQUISITION;
        break;

      case SDSS_ACQ_STT_MORE_PREF:
        log_ptr->sd_state = SD_LOG_STATE_ALT_ACQUISITION;
        break;

      case SDSS_ACQ_STT_REACQ:
        log_ptr->sd_state = SD_LOG_STATE_REACQUISITION;
        break;

      default:
        log_ptr->sd_state = SD_LOG_STATE_ACQUISITION;
        break;
    }
  }
  else if( ss_ptr->ss_state == SDSS_STATE_PWR_SAVE )
  {
    log_ptr->sd_state = SD_LOG_STATE_PWR_SAVE;
  }
  else
  {
    log_ptr->sd_state = SD_LOG_STATE_OPERATION;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy SI-State related information into the log packet.
  */
  log_ptr->srv_is_on = sys_srv_status_is_srv(si_ptr->si_info.srv_status );
  log_ptr->sd_state_flags.is_redir =
         !sdss_is_redir(ss,SDSS_REDIR_VAL_NONE);
  log_ptr->sd_state_flags.is_ho = !sdss_is_handoff(SDSS_HO_VAL_NONE);
  log_ptr->sd_state_flags.is_reselection = (ss_ptr->ss_timer != 0);
  log_ptr->mode = si_ptr->si_info.mode;
  log_ptr->band = si_ptr->si_info.band;
  log_ptr->roam = si_ptr->si_info.roam;
  log_ptr->channel = si_ptr->si_info.chan;

  if (si_ptr->si_info.mode == SD_MODE_HDR )
  {
    memcpy ((void*)(log_ptr->system_id.sector_id),
            (void*)(si_ptr->si_info.sid.is856), 16 );
  }
  else
  {
    log_ptr->system_id.sid_nid.sid = si_ptr->si_info.sid.is95.sid;
    log_ptr->system_id.sid_nid.nid = si_ptr->si_info.sid.is95.nid;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Submit the log to DIAG.
  */
  log_commit( (log_type *) log_ptr );

} /* sd_ss_log_last_action */
#endif /* FEATURE_SD20_LOGGING */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_log_map_ss_act

DESCRIPTION
  Map SD SS-Action to Logged SS-Action.

DEPENDENCIES
  None.

RETURN VALUE
  The logged SS-Action that is associated with the input SD SS-Action.
  SDLOG_SS_ACT_MAX if the input SD SS-Action is invalid.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdlog_ss_act_e_type       sd_log_map_ss_act(

        sd_ss_act_e_type          ss_act
            /* SD SS-Action to be mapped.
            */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( ss_act, SD_SS_ACT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( ss_act )  /*lint -save -e788*/
  {
    case SD_SS_ACT_CONTINUE:    return SDLOG_SS_ACT_CONTINUE;
    case SD_SS_ACT_ACQ_CDMA:    return SDLOG_SS_ACT_ACQ_CDMA;
    case SD_SS_ACT_ACQ_AMPS:    return SDLOG_SS_ACT_ACQ_AMPS;
    case SD_SS_ACT_ACQ_HDR:     return SDLOG_SS_ACT_ACQ_HDR;
    case SD_SS_ACT_MEAS_DED:    return SDLOG_SS_ACT_MEAS_DED;
    case SD_SS_ACT_MEAS_BACK:   return SDLOG_SS_ACT_MEAS_BACK;
    case SD_SS_ACT_PWR_SAVE:    return SDLOG_SS_ACT_PWR_SAVE;
    case SD_SS_ACT_ACCEPT:      return SDLOG_SS_ACT_ACCEPT;
    case SD_SS_ACT_REJECT:      return SDLOG_SS_ACT_REJECT;
    case SD_SS_ACT_RELEASE:     return SDLOG_SS_ACT_RELEASE;
    case SD_SS_ACT_ACQ_GPS:     return SDLOG_SS_ACT_ACQ_GPS;
    case SD_SS_ACT_ACQ_GW:      return SDLOG_SS_ACT_ACQ_GW;
    case SD_SS_ACT_GET_NET_GW:  return SDLOG_SS_ACT_GET_NET_GW;
    case SD_SS_ACT_PWR_SCAN_GW: return SDLOG_SS_ACT_PWR_SCAN_GW;

    case SD_SS_ACT_MAX:         return SDLOG_SS_ACT_MAX;
    default:                    return SDLOG_SS_ACT_MAX;

  } /* switch */ /*lint -restore */

} /* sd_log_map_ss_act */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_log_state_get

DESCRIPTION
  Map SD SS-Action to Logged SS-Action.

DEPENDENCIES
  None.

RETURN VALUE
  The logged SS-Action that is associated with the input SD SS-Action.
  SDLOG_SS_ACT_MAX if the input SD SS-Action is invalid.

SIDE EFFECTS
  None.

===========================================================================*/
static sd_log_state_e_type       sd_log_state_get(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  sd_ss_s_type              *ss_ptr     = sd_ss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-State is in PWR_SAVE
  */
  if( ss_ptr->ss_state == SDSS_STATE_PWR_SAVE )
  {
    return SD_LOG_STATE_PWR_SAVE;
  }

  /* If SS-State is not acquisition, SD is in operation state.
  */
  if( ss_ptr->ss_state != SDSS_STATE_ACQ )
  {
    return SD_LOG_STATE_OPERATION;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, SD is in acquisition state. Return the acquisition
  ** substate.
  */
  switch( sdss_eng_stack_get_state(ss) )
  {
    case SDSS_ACQ_STT_START:      return SD_LOG_STATE_ACQUISITION;
    case SDSS_ACQ_STT_MORE_PREF:  return SD_LOG_STATE_ALT_ACQUISITION;
    case SDSS_ACQ_STT_REACQ:      return SD_LOG_STATE_REACQUISITION;

    case SDSS_ACQ_STT_DONE:
    case SDSS_ACQ_STT_MAX:
    case SDSS_ACQ_STT_NONE:
    default:                      return SD_LOG_STATE_ACQUISITION;
  }

} /* sd_log_state_get */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_event_rprt

DESCRIPTION
  Report SD event and the resulting SD action.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_ss_event_rprt(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdss_evt_e_type           event,
            /* SS-Event to process.
            */

        const sd_ss_act_s_type    *act_ptr
            /* Pointer to a buffer with returned SS-Action.
            */
)
{
  sd_ss_s_type              *ss_ptr         = sd_ss_ptr( ss );

  event_id_enum_type        event_id        = EVENT_SD_EVENT_ACTION;

  sdlog_event_s_type        event_buf;
  sdlog_event_s_type        *event_ptr      = &event_buf;
  unsigned char             event_buf_size  = sizeof( sdlog_event_s_type );

  dword                     uptime          = clk_uptime();

  sdss_pref_s_type          pref_buf;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( act_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If reporting for HDR hybrid system selection, set event_id to
  ** EVENT_SD_EVENT_ACTION_HYBR.
  */
  if( ss == SD_SS_HYBR_1 ) {
    event_id = EVENT_SD_EVENT_ACTION_HYBR;
  }
  else if ( ss == SD_SS_HYBR_WLAN )
  {
    event_id = EVENT_SD_EVENT_ACTION_HYBR_WLAN;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the SS-event.
  */
  event_ptr->ss_event   = (word) event;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the SS-Action.
  */
  event_ptr->ss_act     = (byte) sd_log_map_ss_act( act_ptr->act );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the SD state.
  */
  event_ptr->sd_state   = (byte) sd_log_state_get( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy SS-Action's associated parameters.
  */
  switch( act_ptr->act )
  {
    case SD_SS_ACT_CONTINUE:
      if( ss_ptr->ss_timer > uptime ) {
        event_ptr->act_prm.cont.ss_timer = (word)(ss_ptr->ss_timer - uptime);
      }
      else {
        event_ptr->act_prm.cont.ss_timer = 0;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_CDMA:
      event_ptr->act_prm.acq_cdma.band    = (byte)act_ptr->prm.acq_cdma.band;
      event_ptr->act_prm.acq_cdma.blk_sys =
                                         (byte) act_ptr->prm.acq_cdma.blksys;
      event_ptr->act_prm.acq_cdma.chan    = act_ptr->prm.acq_cdma.chan;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_AMPS:
      event_ptr->act_prm.acq_amps.band  = (byte) act_ptr->prm.acq_amps.band;
      event_ptr->act_prm.acq_amps.sys   = (byte) act_ptr->prm.acq_amps.sys;
      event_ptr->act_prm.acq_amps.is_amps_2nd_cch =
                                       act_ptr->prm.acq_amps.is_amps_2nd_cch;
      event_ptr->act_prm.acq_amps.max_redirect_delay =
                             (byte) act_ptr->prm.acq_amps.max_redirect_delay;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_HDR:
      event_ptr->act_prm.acq_hdr.band     = (byte) act_ptr->prm.acq_hdr.band;
      event_ptr->act_prm.acq_hdr.acq_mode =
                                        (byte) act_ptr->prm.acq_hdr.acq_mode;
      event_ptr->act_prm.acq_hdr.chan     = act_ptr->prm.acq_hdr.chan;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_MEAS_DED:
    case SD_SS_ACT_MEAS_BACK:
    case SD_SS_ACT_PWR_SAVE:
    case SD_SS_ACT_ACCEPT:
    case SD_SS_ACT_REJECT:
    case SD_SS_ACT_RELEASE:
    case SD_SS_ACT_ACQ_GPS:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_GW:
    #if( defined(FEATURE_GSM) || defined(FEATURE_WCDMA) )
#error code not present
    #endif /* FEATURE_GSM || FEATURE_WCDMA */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_GET_NET_GW:
    #if( defined(FEATURE_GSM) || defined(FEATURE_WCDMA) )
#error code not present
    #endif /* FEATURE_GSM || FEATURE_WCDMA */
      break;

    case SD_SS_ACT_PWR_SCAN_GW:
    case SD_SS_ACT_ACQ_WLAN:
    case SD_SS_ACT_SURVEY_WLAN:
    case SD_SS_ACT_MAX:
    default:
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For events that carry parameters, copy the parameters.
  */
  switch( event )  /*lint -e788 */ /* sparse switch */
  {
    case SDSS_EVT_USER_SS_PREF:
      sdss_get_pref( ss, &pref_buf );
      event_ptr->event_prm.ss_pref.orig_mode = (byte) pref_buf.orig_mode;
      event_ptr->event_prm.ss_pref.mode_pref = (word) pref_buf.mode_pref;
      event_ptr->event_prm.ss_pref.band_pref = (dword) pref_buf.band_pref;
      event_ptr->event_prm.ss_pref.roam_pref = (byte) pref_buf.roam_pref;
      event_ptr->event_prm.ss_pref.hybr_pref = (byte) pref_buf.hybr_pref;
      break;

    case SDSS_EVT_CDMA_OPR_ACC_TERM:
      sdss_get_acc_prm(
            ss,
            &event_ptr->event_prm.acc_term.reason,
            &event_ptr->event_prm.acc_term.term );

      event_buf_size -= sizeof( event_ptr->event_prm.acc_term.reserved );
      break;

    case SDSS_EVT_HDR_OPR_ACC_TERM:
      sdss_get_acc_prm(
            ss,
            &event_ptr->event_prm.acc_term.reason,
            &event_ptr->event_prm.acc_term.term );

      event_buf_size -= sizeof( event_ptr->event_prm.acc_term.reserved );
      break;

    default:
      event_buf_size -= sizeof( event_ptr->event_prm );
      break;

  } /* switch( event ) */ /*lint +e788 */ /* sparse switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Report the event.
  */
  event_report_payload( event_id, event_buf_size, event_ptr );

} /* sd_ss_event_rprt */




#ifdef FEATURE_GPSONE_MSBASED
#error code not present
#endif /* FEATURE_GPSONE_MSBASED */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_meas_list_get2

DESCRIPTION
  Extract the mode, band pref and channel of each one of the systems that
  are on the measurement list (i.e., systems for which the channel strength
  measurement is requested).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if list position <= position of last element on measurement list.
  FALSE if list position is > position of last element on measurement list.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sd_ss_meas_list_get2(

        int                       pos,
            /* List position for which to return the mode band and channel.
            */

        sd_ss_mode_pref_e_type    *rtrn_mode_ptr,
            /* Pointer to a buffer where to copy the system's mode.
            */

        sd_ss_band_pref_e_type    *rtrn_band_ptr,
            /* Pointer to a buffer where to copy the system's band.
            */

        sd_chan_type              *rtrn_chan_ptr
            /* Pointer to a buffer where to copy the system's chan.
            */
)
{

  sdss_pref_s_type       sdss_pref;
    /*  SD storage of user preferences */

  sdsr_s_type            *sr_ptr = NULL;
    /* Pointer to sdsr_tbl entry */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fetch the preferences from SD_SS.
  */
  sdss_get_pref( SD_SS_MAIN, &sdss_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( INRANGE(pos, 0, sdsr_list_cnt(SDSR_MEAS_LST)) );
  SD_ASSERT( rtrn_mode_ptr != NULL );
  SD_ASSERT( rtrn_band_ptr != NULL );
  SD_ASSERT( rtrn_chan_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If measurement list position resolved to an existing system, copy the
  ** system's mode, band and channel to the returned buffer and return TRUE.
  ** Else, return FALSE.
  */
  sr_ptr = sdsr_list_get_ptr( SDSR_MEAS_LST, pos );

  if( sr_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy mode, band and channel
  */
  *rtrn_mode_ptr = sdss_map_mode_to_mode_pref (sr_ptr->sys.mode);
  *rtrn_band_ptr = sr_ptr->sys.band;
  *rtrn_chan_ptr = sr_ptr->sys.chan;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filtering mode and band for only GW systems
  */
  if ( (*rtrn_mode_ptr == SD_SS_MODE_PREF_GW) ||
       (*rtrn_mode_ptr == SD_SS_MODE_PREF_GSM) ||
       (*rtrn_mode_ptr == SD_SS_MODE_PREF_WCDMA)
     )
  {
    /* Filters out mode and band to comply with target and system preference
    */
    sd_misc_get_band_mode( rtrn_mode_ptr,
                           rtrn_band_ptr,
                           sdss_pref.mode_pref,
                           sdss_pref.band_pref);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* sd_ss_meas_list_get2 */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_meas_list_get

DESCRIPTION
  Extract the mode, band and channel of each one of the systems that are on
  the measurement list (i.e., systems for which the channel strength
  measurement is requested).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if list position <= position of last element on measurement list.
  FALSE if list position is > position of last element on measurement list.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sd_ss_meas_list_get(

        int                       pos,
            /* List position for which to return the mode band and channel.
            */

        sd_mode_e_type            *rtrn_mode_ptr,
            /* Pointer to a buffer where to copy the system's mode.
            */

        sd_band_e_type            *rtrn_band_ptr,
            /* Pointer to a buffer where to copy the system's band.
            */

        sd_chan_type              *rtrn_chan_ptr
            /* Pointer to a buffer where to copy the system's chan.
            */
)
{

  sd_ss_mode_pref_e_type mode_pref = SD_SS_MODE_PREF_NONE;
    /* mode_pref from sd_ss_meas_list_get2()
    */

  sd_ss_band_pref_e_type band_pref = SD_SS_BAND_PREF_NONE;
    /* band_pref from sd_ss_meas_list_get2()
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT( rtrn_mode_ptr != NULL );
  SD_ASSERT( rtrn_band_ptr != NULL );
  SD_ASSERT( rtrn_chan_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Calling new api for getting meas list
  */
  if (!sd_ss_meas_list_get2 ( pos,
                              &mode_pref,
                              &band_pref,
                              rtrn_chan_ptr
                            )
     )

  {
    SD_ERR ("sd_ss_meas_list_get2() FAILED pos %d", pos, 0, 0);

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert mode and band_pref */

  *rtrn_mode_ptr = sdss_map_mode_pref_to_mode (mode_pref);

  *rtrn_band_ptr = sdss_map_band_pref_to_band (band_pref);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* sd_ss_meas_list_get */


#ifdef FEATURE_WLAN
#error code not present
#endif


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_meas_list_rprt

DESCRIPTION
  Report channel strength measurement for each one of the systems that are
  on the measurement list (i.e., systems for which the channel strength
  measurement is requested).

  Note that reported measurement values are in units of -1dB for Rx-power and
  -0.5dB for Pilot-strength.

  Note that a reported value of 255 implies that no measurement was obtained.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if reported measurement ID matches the requested measurement ID and
  list position <= position of last system on the measurement list. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sd_ss_meas_list_rprt(

        int                       pos,
            /* List position for which measurement is reported.
            */

        int2                      meas_val
            /* Reported channel strength measurement value in units
            ** of -1dB for Rx-power and -0.5dB for Pilot strength. 255
            ** implies that no measurement was obtained.
            */
)
{
  sdsr_s_type            *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( INRANGE(pos, 0, sdsr_list_cnt(SDSR_MEAS_LST)) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If measurement list position resolved to an existing system record,
  ** copy the reported measurement value into the meas_val field of the
  ** system record and return TRUE. Else, return FALSE.
  */
  sr_ptr = sdsr_list_get_ptr( SDSR_MEAS_LST, pos );


  if( sr_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Measurement report for GW systems contains number of PLMNs found
  ** as part of background service search. This number can be either 0
  ** or positive and should not be made negative.
  ** Measurement value is negative only for CDMA.
  */
  if ( (sr_ptr->sys.mode == SD_MODE_CDMA)
       &&
       (meas_val > 0)
     )
  {
    meas_val = -meas_val;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH( "Meas report=%d, pos=%d, chan=%d",
                 meas_val, pos, sr_ptr->sys.chan );

  sr_ptr->meas_val = meas_val;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* sd_ss_meas_list_rprt */


#ifdef FEATURE_WLAN
#error code not present
#endif


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_map_acq_mode_to_hdr

DESCRIPTION
  Map SD acquisition mode to HDR specific acquisition mode.

DEPENDENCIES
  None.

RETURN VALUE
  The specific HDR acquisition that is associated with the input SD
  acquisition mode.

  SD_BLKSYS_MAX if the input mode, band and channel are bad or unsupported.

SIDE EFFECTS
  None.

===========================================================================*/
static   sd_hdr_acq_mode_e_type    sd_ss_map_acq_mode_to_hdr(

        sdss_acq_mode_e_type      acq_mode
            /* SD acquisition mode to be mapped.
            */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( acq_mode, SDSS_ACQ_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( acq_mode )
  {
    case SDSS_ACQ_MODE_HO:
    case SDSS_ACQ_MODE_FULL:
    case SDSS_ACQ_MODE_MINI:
    case SDSS_ACQ_MODE_FULL2:
    case SDSS_ACQ_MODE_MINI2:
    case SDSS_ACQ_MODE_UNIFORM_FULL:
    case SDSS_ACQ_MODE_UNIFORM_FULL2:
    case SDSS_ACQ_MODE_UNIFORM_FULL_RTN:
    case SDSS_ACQ_MODE_UNIFORM_FULL_RTN2:

      return SD_HDR_ACQ_MODE_FULL;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_MICRO:
    case SDSS_ACQ_MODE_MICRO2:
      return SD_HDR_ACQ_MODE_MICRO;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_DEEP:
    case SDSS_ACQ_MODE_DEEP2:
      return SD_HDR_ACQ_MODE_DEEP;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_SHALLOW:
    case SDSS_ACQ_MODE_SHALLOW2:
      return SD_HDR_ACQ_MODE_SHALLOW;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_PREV:
    case SDSS_ACQ_MODE_MAX:
    default:
      return SD_HDR_ACQ_MODE_FULL;

  } /* switch */

} /* sd_ss_map_acq_mode_to_hdr */



#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */



#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */



/* <EJECT> */
/*===========================================================================

FUNCTION sd_filter_out_overlap_band

DESCRIPTION
  Get band preference complies to MS radio access capability.Clear PGSM band
  pref if EGSM band pref is present. Clear both EGSM and PGSM band pref if
  RGSM band pref is present.

  SEMANTIC RULE : Among the three Access Type Technologies GSM 900-P,
  GSM 900-E and GSM 900-R only one shall be  present.

  Refer to 3GPP TS 05.05 Frequency in MHz
             Mobile Tx     Base Station Tx
  PGSM       890-915       935-960
  EGSM       880-915       925-960
  RGSM       876-915       921-960

DEPENDENCIES
  None

RETURN VALUE
  band preference enum type

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_ss_band_pref_e_type    sd_filter_out_overlap_band(

        sd_ss_band_pref_e_type    band_pref
       /* Enumeration of system selection band and sub-band preferences.*/
)
{
  sd_ss_band_pref_e_type          return_band_pref = band_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 #ifdef FEATURE_RGSM_CAPABLE
#error code not present
 #else  
    return_band_pref = SD_BAND_DELETE( return_band_pref,
                                       SD_SS_BAND_PREF_GSM_RGSM_900
                                     );
 #endif
  if( SD_BAND_CONTAIN( band_pref, SD_SS_BAND_PREF_GSM_EGSM_900) )
  {
    return_band_pref = SD_BAND_DELETE( return_band_pref,
                                       SD_SS_BAND_PREF_GSM_PGSM_900
                                     );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return return_band_pref;

} /* sd_filter_out_overlap_band */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_get_band_mode

DESCRIPTION
  Filter out the band and mode not supported by the target capability.
  Filter out the band and mode that are not in part of system preference.
  Filter out any overlapped bands.
  Bands that do not have corresponding modes.
  Modes that do not have corresponding bands.

  MODE
DEPENDENCIES
  None

RETURN VALUE
  Return the filtered mode and band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_get_band_mode(

        sd_ss_mode_pref_e_type    *mode_ptr,
            /* Pointer to the mode preference that needs to be filtered.
            */

        sd_ss_band_pref_e_type    *band_ptr,
            /* Pointer to the band preference that needs to be filtered.
            */

        sd_ss_mode_pref_e_type    sys_mode_pref,
            /* Enumeration of system mode perference.
            */

        sd_ss_band_pref_e_type    sys_band_pref
            /* Enumeration of system selection band and sub-band preferences.
            */
)
{
  sd_ss_mode_pref_e_type          mode_pref;
  sd_ss_band_pref_e_type          band_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( mode_ptr !=NULL );
  SD_ASSERT( band_ptr !=NULL );

  mode_pref = *mode_ptr;
  band_pref = *band_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out modes that are not part of the mode preference.
  */
  mode_pref = SD_GET_COMMON_MODE( mode_pref, sys_mode_pref );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out bands that are not in the band preference.
  */
  band_pref = SD_GET_COMMON_BAND( band_pref, sys_band_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out:
  ** 1. Bands that do not have corresponding modes.
  ** 2. Modes that do not have corresponding bands.
  */
  mode_pref = sdss_get_supp_mode_pref(mode_pref, band_pref);
  band_pref = sdss_get_supp_mode_band_pref( mode_pref, band_pref );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Filter out overlapping bands.
  */
  band_pref = sd_filter_out_overlap_band( band_pref );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy the filtered mode and band back into the input parameters.
  */
  *mode_ptr = mode_pref;
  *band_ptr = band_pref;

} /* sd_get_gw_band_mode */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_is_opti_ss_pref_change

DESCRIPTION
   Indicate a ss pref change an optimized one if it is not a true preference
   change and the cause of the change is not require polling the scripts.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE   An optimized ss pref change.
  FALSE  Not an optimized ss pref change.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean                    sd_is_opti_ss_pref_change(

       sd_ss_e_type               ss,
            /* System selection enum type
            */

       sdss_pref_updated_e_type   pref_change,
           /* Indicate the ss pref change status
           */
       sd_ss_pref_reas_e_type     pref_reason,
           /* Reason for changing the SS-Preference
           */

       sd_ss_orig_mode_e_type     orig_mode
           /* The Orig mode, used to differentiae between DO and UMTS on the
           ** hybrid stack
           */
)
{
  sd_si_s_type               *si_ptr     = sd_si_ptr(ss);
      /* System Indication.*/
  sys_srv_status_e_type      srv_status;
      /* Indicate the current Service status */
  sdss_state_e_type          ss_state = sd_ss_ptr(ss)->ss_state;
      /* The current SS state */
  #ifdef FEATURE_HICPS_STACK_SYNC_UP
#error code not present
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( pref_change, SDSS_PREF_UPDATE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( pref_reason, SD_SS_PREF_REAS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  srv_status = si_ptr->si_info.srv_status;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Preferences (main) is not changed and the reason for the
  ** SS-Preference indication is other than USER or REDIAL_OPTI (optimized
  ** silent redial), and if pref_reas was not changed for originating a call
  ** and not in pwr_save. just return CONTINUE.
  */
  if( (ss == SD_SS_MAIN ||
       orig_mode != SD_SS_ORIG_MODE_COLLOC )              &&
      (   pref_change != SDSS_PREF_UPDATED_TRUE_PREF      &&
          pref_change != SDSS_PREF_UPDATED_SRV_DOMAIN     &&
          pref_change != SDSS_PREF_UPDATED_EMERG_ORIG_END &&
          pref_reason != SD_SS_PREF_REAS_USER             &&
          pref_reason != SD_SS_PREF_REAS_STACK_SYNC_UP    &&
          pref_reason != SD_SS_PREF_REAS_USER_RESEL       &&
          pref_reason != SD_SS_PREF_REAS_REDIAL_OPTI      &&
          pref_reason != SD_SS_PREF_REAS_AVOID_SYS        &&
          pref_reason != SD_SS_PREF_REAS_ORIG_RESUME      &&
          !( ( pref_reason == SD_SS_PREF_REAS_ORIG_START_CS ||
               pref_reason == SD_SS_PREF_REAS_ORIG_START_PS )
                                &&
             ( pref_change == SDSS_PREF_UPDATED_PREF_REAS ||
               ss_state    == SDSS_STATE_PWR_SAVE )
           )
      )
      #ifndef FEATURE_UNIFORM_SCAN_OOS
       && ( srv_status != SYS_SRV_STATUS_PWR_SAVE )
      #endif
    )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
#error code not present
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Preferences(HDR) is not changed and the reason for the SS-Preference
  ** indication is ORIG_END or REDIAL_NORM, just return CONTINUE. If AT is in
  ** power save, don't continue if the user changed the preferences or call
  ** is originating
  **
  */
  if( ( ss == SD_SS_HYBR_1 )
       && ( orig_mode == SD_SS_ORIG_MODE_COLLOC ) )
    {
      if( pref_change != SDSS_PREF_UPDATED_TRUE_PREF
          &&
          pref_change != SDSS_PREF_UPDATED_SRV_DOMAIN  &&
          (( pref_reason == SD_SS_PREF_REAS_ORIG_END   &&
             srv_status != SYS_SRV_STATUS_SRV)         ||
          ( pref_reason == SD_SS_PREF_REAS_REDIAL_NORM &&
            srv_status != SYS_SRV_STATUS_PWR_SAVE  ) )
        )
      {
        return TRUE;
      }
    }

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
#error code not present
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Preferences(WLAN) is not changed, just return CONTINUE.
  */
  if( ss == SD_SS_HYBR_WLAN                        &&
      pref_change != SDSS_PREF_UPDATED_TRUE_PREF   &&
      pref_reason != SD_SS_PREF_REAS_USER          &&
      pref_reason != SD_SS_PREF_REAS_USER_RESEL    &&
      pref_reason != SD_SS_PREF_REAS_STACK_SYNC_UP
    )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

}/* sd_is_opti_ss_pref_change */


#ifdef FEATURE_HDR_HYBRID
#error code not present
#endif /* FEATURE_HDR_HYBRID */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_event_proc

DESCRIPTION
  Do SS-Event common processing. In other words, the processing that is
  common to all SS-Events is done in this function.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_ss_act_e_type          sd_ss_event_proc(

        sd_ss_e_type              ss,
            /* The ss to use for the event
            */


        sdss_evt_e_type           event,
            /* SS-Event to process.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sdsr_s_type           *sr_ptr;
  sdss_iact_s_type      iact;
  #ifdef SD_USES_CAI
  boolean               is_disabling_redir;
  #endif /* SD_USES_CAI */
  sdsr_s_type           *last_acq_sys_ptr;
  sd_mode_e_type        last_mode             = SD_MODE_MAX;
  sd_blksys_e_type      last_blksys           = SD_BLKSYS_MAX;

  sdss_iact_s_type      *iact_ptr             = &iact;
  sd_ss_s_type          *ss_ptr               = sd_ss_ptr(ss);
  sd_ss_act_s_type      *act_ptr              = &ss_ptr->act;
  dword                 uptime                = clk_uptime();
  sdss_pref_s_type      sdss_pref;

  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (!defined( FEATURE_TECH_IND_SD_STACK_ARCH ) && defined(FEATURE_HDR_HYBRID))
#error code not present
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Lock the MAIN ss component before processing it.
  */
  SD_SS_LOCK( ss_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print the SS-Event.
  */
  sdss_event_group_print( event );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the system that was attempted acquisition last and remember its
  ** mode and blksys.
  */
  last_acq_sys_ptr = sdsr_list_get_ptr( sdss_sr_list_map2(ss,SDSR_ACQ_SYS), 0 );

  if( last_acq_sys_ptr != NULL )
  {
    last_mode     = last_acq_sys_ptr->sys.mode;
    last_blksys   = sdprl_map_chan_to_blksys( last_acq_sys_ptr->sys.mode,
                     sdss_map_band_pref_to_band(last_acq_sys_ptr->sys.band),
                                              last_acq_sys_ptr->sys.chan );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef SD_USES_CAI
  /* Remember whether CDMA redirection is ON.
  */
  is_disabling_redir = sdss_is_redir( ss, SDSS_REDIR_VAL_CDMA_ANY );
  #endif /* SD_USES_CAI */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is an acquisition event, update the SDSR_ACQ_SYS as per this
  ** event.
  */
  if( SDSS_EVT_IS_XXX_ACQ(event) )
  {
    sdsr_s_type *list_sr_ptr = sdsr_list_get_ptr( ss_ptr->acq_list,
                                                  ss_ptr->acq_pos );
    SD_ASSERT( list_sr_ptr != NULL );

    sr_ptr = sdsr_list_get_ptr( sdss_sr_list_map2(ss,SDSR_ACQ_SYS), 0 );
    SD_ASSERT( sr_ptr != NULL );

    /*lint -e774 */
    if( sr_ptr != NULL )
    {
      sr_ptr->event       = (word) event;
      sr_ptr->event_time  = uptime;
    }

    #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
    #endif
    {
      /* Do post acquisition attempt updates.
      */
      if( list_sr_ptr != NULL )
      {
        /* Do post acquisition attempt updates.
        */
        sd_ss_acq_update_post( ss,
                               ss_ptr->acq_list,
                               ss_ptr->acq_pos,
                               ss_ptr->acq_mode );
      }
      else if( sr_ptr != NULL )
      {
        /* Do post acquisition attempt updates.
        */
        sd_ss_acq_update_post( ss,
                               sdss_sr_list_map2(ss, SDSR_ACQ_SYS),
                               0,
                               ss_ptr->acq_mode );
      }
      sdsr_list_del_list(SDSR_PWR_SCAN_LST, SDSR_ACQ_SYS );
    } /* if */
    #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
    #endif
    /*lint +e774 */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, if this is an operation event, update the SDSR_SRV_SYS and
  ** SDSR_IDL_SYS as per this event.
  */
  else if( SDSS_EVT_IS_XXX_OPR(event) )
  {
    sr_ptr = sdsr_list_get_ptr( sdss_sr_list_map2(ss,SDSR_SRV_SYS), 0 );
    SD_ASSERT( sr_ptr != NULL );

    /*lint -e774 */
    if( sr_ptr != NULL ) {
      sr_ptr->event       = (word) event;
      sr_ptr->event_time  = uptime;
    }
    /*lint +e774 */
    sr_ptr = sdsr_list_get_ptr( sdss_sr_list_map2(ss,SDSR_IDL_SYS), 0 );

    if( sr_ptr != NULL ) {
      sr_ptr->event       = (word) event;
      sr_ptr->event_time  = uptime;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remember the pervious SS-Action.
  */
  ss_ptr->prev_act = *act_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to process the SS-Event.
  */
  if( ss == SD_SS_MAIN )
  {
    sdss_event_proc( event, iact_ptr );
  }
  else
  {
    #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
    sdss_hybr_1_event_proc( event, iact_ptr );
    #else
    SD_ERR_FATAL(" Wrong stack",0,0,0);
    #endif
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------------
  ** Do SS-Action independent processing.
  ** ------------------------------------
  */

  /* Update the SS-State indicator and inform the registered client (i.e.,
  ** CM) of any preferred service acquisition indicator changes.
  */
  ss_ptr->ss_state = iact_ptr->ss_state;
  sd_si_pref_srv_acq( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If timer expiration time has changed, set the timer per the new
  ** expiration time.
  */
  if( iact_ptr->ss_timer != ss_ptr->ss_timer )
  {
    /* If new expiration time is 0. Disable the timer by setting it to 0.
    */
    if( iact_ptr->ss_timer == 0 )
    {
      SD_MSG_HIGH( "SS-Timer=0, ss=%d",ss,0,0 );
      ss_ptr->timer_func_ptr( 0 );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else (new expiration time != 0), set the timer according to the new
    ** expiration time.
    */
    else
    {
      dword timer;

      if( iact_ptr->ss_timer > uptime ) {
        timer = (iact_ptr->ss_timer - uptime) * 1000;
      }
      else {
        timer = SD_SS_TIMER_INVALID;
        SD_MSG_HIGH( "SS-Timer=%dms", timer, 0, 0 );
      }

      SD_MSG_HIGH( "SS-Timer=%ds, uptime=%ds, exp=%ds",
                   timer/1000, uptime, iact_ptr->ss_timer );

      ss_ptr->timer_func_ptr( (int4) timer );
    }

    ss_ptr->ss_timer = iact_ptr->ss_timer;

  } /* if */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fetch the preferences from SD_SS.
  */
  sdss_get_pref( ss, &sdss_pref );


  /* ----------------------------------
  ** Do SS-Action dependent processing.
  ** ----------------------------------
  */
  switch( iact_ptr->act )
  {
    case SDSS_IACT_CONTINUE:

      act_ptr->act = SD_SS_ACT_CONTINUE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_ACQUIRE:

      /* Do pre acquisition attempt updates.
      */
      sd_ss_acq_update_pre( ss,
                            iact_ptr->prm.acq.list,
                            iact_ptr->prm.acq.pos,
                            iact_ptr->prm.acq.mode );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Get the system to acquire and set the returned SS-Action as per
      ** the mode of this system.
      */
      sr_ptr = sdsr_list_get_ptr( iact_ptr->prm.acq.list,
                                  iact_ptr->prm.acq.pos );
      if(sr_ptr == NULL)
      {
        SD_ERR_FATAL("sr_ptr NULL",0,0,0);
      }
      if( ! sdss_is_supp_mode_band(sr_ptr->sys.mode, sr_ptr->sys.band) )
      {
        SD_ERR_FATAL( "Unsupported mode/band",0,0,0 );
      }

      switch( sr_ptr->sys.mode )
      {
        case SD_MODE_CDMA:
          act_ptr->act = SD_SS_ACT_ACQ_CDMA;
          act_ptr->prm.acq_cdma.band    = sdss_map_band_pref_to_band(
                                                         sr_ptr->sys.band);
          act_ptr->prm.acq_cdma.chan    = sr_ptr->sys.chan;
          act_ptr->prm.acq_cdma.blksys  = sdprl_map_chan_to_blksys(
                             sr_ptr->sys.mode,
                             sdss_map_band_pref_to_band(sr_ptr->sys.band),
                             sr_ptr->sys.chan );

          /* Indicate whether MS switched mode or blksys.
          */
          if( last_mode   != SD_MODE_CDMA ||
              last_blksys != act_ptr->prm.acq_cdma.blksys )
          {
            act_ptr->prm.acq_cdma.is_new_mode_or_blksys = TRUE;
          }
          else
          {
            act_ptr->prm.acq_cdma.is_new_mode_or_blksys = FALSE;
          }

          /* Update the handoff indicator
          */
          act_ptr->prm.acq_cdma.is_handoff_acq =
                                            iact_ptr->prm.acq.is_handoff_acq;
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_MODE_AMPS:
          act_ptr->act = SD_SS_ACT_ACQ_AMPS;
          act_ptr->prm.acq_amps.band = sdss_map_band_pref_to_band(
                                                           sr_ptr->sys.band);
          act_ptr->prm.acq_amps.sys = (sd_cell_sys_e_type) sr_ptr->sys.chan;
          SD_ASSERT_ENUM_IS_INRANGE( act_ptr->prm.acq_amps.sys,
                                     SD_CELL_SYS_MAX );

          act_ptr->prm.acq_amps.is_amps_2nd_cch =
                                        iact_ptr->prm.acq.is_amps_2nd_cch;

          act_ptr->prm.acq_amps.max_redirect_delay =
                                        iact_ptr->prm.acq.max_redirect_delay;
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_MODE_HDR:
          act_ptr->act = SD_SS_ACT_ACQ_HDR;
          act_ptr->prm.acq_hdr.band = sdss_map_band_pref_to_band(
                                                    sr_ptr->sys.band);
          act_ptr->prm.acq_hdr.chan = sr_ptr->sys.chan;
          act_ptr->prm.acq_hdr.acq_mode = sd_ss_map_acq_mode_to_hdr(
                                                    iact_ptr->prm.acq.mode );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_MODE_GPS:
          act_ptr->act = SD_SS_ACT_ACQ_GPS;
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_MODE_GSM:
        case SD_MODE_WCDMA:
        case SD_MODE_GW:

        #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
          #endif /* FEATURE_GSM || FEATURE_WCDMA */
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_MODE_NONE:
        case SD_MODE_WLAN:
        case SD_MODE_INACT:
        case SD_MODE_MAX:
        default:
          SD_CASE_ERR( "mode",0,0,0 );
          break;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_MEAS:

      /* First do the following:
      **
      ** 1. Increment the measurement ID.
      ** 2. Indicate that measurement request is not done being serviced.
      ** 3. Indicate whether the requested list of systems to measure is
      **    different than the previous requested list.
      ** 4. Copy the systems that are being measured into the SDSR_MEAS_LIST.
      */
      ss_ptr->meas_id++;
      ss_ptr->meas_is_done = FALSE;

      if( sdsr_list_is_contain(SDSR_MEAS_LST,
                               SDSR_CONTAIN_SAME,
                               iact_ptr->prm.meas.list) )
      {
        ss_ptr->meas_list_is_diff = FALSE;
      }
      else
      {
        ss_ptr->meas_list_is_diff = TRUE;
      }
      sdsr_list_cpy( SDSR_MEAS_LST, iact_ptr->prm.meas.list );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Set the returned SS-Action as per the measurement mode.
      */
      if( iact_ptr->prm.meas.mode == SDSS_MEAS_MODE_DED )
      {
        act_ptr->act = SD_SS_ACT_MEAS_DED;
      }
      else
      {
        act_ptr->act = SD_SS_ACT_MEAS_BACK;

      }
      act_ptr->prm.meas.type  = iact_ptr->prm.meas.type;
      act_ptr->prm.meas.id    = ss_ptr->meas_id;

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_PWR_SAVE:
      act_ptr->act = SD_SS_ACT_PWR_SAVE;

      /*  Populate the pwr save parameters, default is due to no_srv.
      **  check if there are modes available to acquire.
      */
      act_ptr->prm.pwr_save.reas =  SD_SS_PWR_SAVE_REAS_NO_SRV;
      if (!sdss_is_supp_mode_band_pref(sdss_pref.mode_pref, sdss_pref.band_pref))
      {
        /* No modes available to acquire.
        */
        act_ptr->prm.pwr_save.reas = SD_SS_PWR_SAVE_REAS_NO_SYS;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_ACCEPT:
      act_ptr->act = SD_SS_ACT_ACCEPT;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_REJECT:
      act_ptr->act = SD_SS_ACT_REJECT;
     break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_RELEASE:
      act_ptr->act = SD_SS_ACT_RELEASE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_GET_NET_GW:
    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
    #endif /* FEATURE_GSM || FEATURE_WCDMA */
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_PWR_SCAN_GW:
    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
    #endif /* FEATURE_GSM || FEATURE_WCDMA */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_MAX:
    case SDSS_IACT_GET_NET_WLAN:
    default:
      SD_CASE_ERR( "act",0,0,0 );
      break;

  } /* switch( act ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Client supplied a buffer for the return SS-Action, copy the
  ** returned SS-Action into this buffer.
  */
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = *act_ptr;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the return_cause indicator, as necessary.
  */
  #ifdef SD_USES_CAI
  /* If redirection is ON, set is_disabling_redir to FALSE.
  */
  if( sdss_is_redir(ss, SDSS_REDIR_VAL_CDMA_ANY) )
  {
    is_disabling_redir = FALSE;
  }
  #endif /* SD_USES_CAI */
  /* If last system attempted acquisition exists, update the
  ** return_cause indicator.
  */
  #ifdef SD_USES_CAI
  if( last_acq_sys_ptr  != NULL )
  {
    sd_ss_return_cause_update( ss,
                               event,
                               is_disabling_redir,
                               last_acq_sys_ptr );
  }
  #endif /* defined(SD_USES_CAI) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy contents of act field to true current act data structure if act
  ** enum type is not continue.
  */
  if( act_ptr->act != SD_SS_ACT_CONTINUE)
  {
    ss_ptr->true_curr_act = *act_ptr;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH( "<<<< Returned Action=%d <<<<",act_ptr->act,0,0 );
  sdss_act_print( act_ptr );
  sdss_iact_print( iact_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_SD20_LOGGING
    /* Send SD log packet.
    */
    sd_ss_log_last_action( ss );
  #endif /* FEATURE_SD20_LOGGING */

  /* Send SD event packet.
  */
  sd_ss_event_rprt( ss, event, act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Unlock the MAIN ss component.
  */
  SD_SS_FREE( ss_ptr );


  return act_ptr->act;

} /* sd_ss_event_proc */

#ifdef FEATURE_HDR
#error code not present
#endif


/* <EJECT> */
/*===========================================================================
=============================================================================
============================ USER SS-INDICATIONS ============================
=============================================================================
===========================================================================*/

/* SS-Indications that are a direct result of a user action, such as system
** preference change or NAM selection change. These indications are usually
** mode independent; or in other words, they can be received in more than one
** of the SS-Client operating modes (e.g. CDMA, AMPS or PWR_SAVE).
*/

/*===========================================================================

FUNCTION sd_ss_ind_user_pwr_up

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to power-up (from OFF or from LPM).

  Updates local SD data as necessary and call on the SS-Core to process the
  SS-Event that is associated with this SS-Indication.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

    SD_SS_ACT_PWR_SCAN_GW - Do early power scan over GSM/WCDMA.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_user_pwr_up(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_FUNC_START( "sd_ss_ind_user_pwr_up()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to update its operating mode.
  */
  sdss_opr_mode_update( SDSS_OPR_MODE_ONLINE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_JCDMA
  /* If MRU is not empty, update the service indicators per MRU[0]. Else
  ** update the service indicators per PRL[0]
  ** Only if 
  */
  if( sdsr_list_cnt(SDSR_MRU_LST) > 0 )
  {
    if(sdsr_list_find_ref(SDSR_GW_LST, sdsr_list_get_ref(SDSR_MRU_LST,0)) == SDSR_POS_NULL)
    {
      sd_si_update_if_srv_is_off( SD_SS_MAIN,
                                  SD_SI_UPDATE_ACQ,
                                  SYS_SRV_STATUS_SRV,
                                  SYS_SRV_DOMAIN_CS_PS,
                                  sdsr_list_get_ref(SDSR_MRU_LST, 0),
                                  NULL,
                                  NULL,
                                  NULL,
                                  SDSR_MAX,
                                  NULL,
                                  NULL,
                                  SYS_SRV_STATUS_SRV);
    }
  }
  else
  {
    if(sdsr_list_find_ref(SDSR_GW_LST, sdsr_list_get_ref(SDSR_PRL_LST,0)) == SDSR_POS_NULL)
    {
      sd_si_update_if_srv_is_off( SD_SS_MAIN,
                                  SD_SI_UPDATE_ACQ,
                                  SYS_SRV_STATUS_SRV,
                                  SYS_SRV_DOMAIN_CS_PS,
                                  sdsr_list_get_ref(SDSR_PRL_LST, 0),
                                  NULL,
                                  NULL,
                                  NULL,
                                  SDSR_MAX,
                                  NULL,
                                  NULL,
                                  SYS_SRV_STATUS_SRV);
    }
  }
  #endif /* FEATURE_JCDMA */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_PWR_UP, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE |
                    SD_SS_ACT_PWR_SCAN_GW) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_user_pwr_up */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_offline_cdma

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to enter offline CDMA mode of operation.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with CDMA offline operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_user_offline_cdma(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to update its operating mode.
  */
  sdss_opr_mode_update( SDSS_OPR_MODE_OFFLINE_CDMA );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the System Indication component that the MS is going offline-
  ** CDMA.
  */
  sd_si_srv_offline( SDSS_OPR_MODE_OFFLINE_CDMA );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save PRL-Component related information to NV.
  */
  sdprl_save();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_OFFLINE_CDMA, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act == SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_user_offline_cdma */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_offline_amps

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to enter offline AMPS mode of operation.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with AMPS offline operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_user_offline_amps(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to update its operating mode.
  */
  sdss_opr_mode_update( SDSS_OPR_MODE_OFFLINE_AMPS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the System Indication component that the MS is going offline-
  ** AMPS.
  */
  sd_si_srv_offline( SDSS_OPR_MODE_OFFLINE_AMPS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save PRL-Component related information to NV.
  */
  sdprl_save();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_OFFLINE_AMPS, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act == SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_user_offline_amps */



/*===========================================================================

FUNCTION sd_ss_ind_user_prot_deactivate

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user informs that
  protocol must be deactivated.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with CDMA offline operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_user_prot_deactivate(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the System Indication component that the SS-Main protocol is
  ** being deactivated.
  */
  sd_si_srv_off( SD_SS_MAIN );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_PROT_DEACTIVATE, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act == SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_user_prot_deactivate */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_pwr_save_exit

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to exit power save mode of operation.

  Note that the UI is the one to determine when the user wants the phone to
  exit power save mode of operation. Typically, however, the UI interprets
  any key press (while MS in power-save) as indication that the user wants
  the MS to exit power-save mode and to start searching for systems actively.

  Note that the actual SD behavior when getting this indication from the
  SS-Client is controlled by the SS-Script for this indication. This script,
  however, is expected to be fairly similar to the power-up script.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Go offline. SD instructs SS-Client to go right back
                          into power-save if the current SS-Preference
                          forbids any PRL system from being attempted
                          acquisition.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_user_pwr_save_exit(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_PWR_SAVE_EXIT, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_user_pwr_save_exit */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_ss_pref

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  System Selection Preference (SS-Preference).

  NOTE:: Function maintained for backwards compatability.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_user_ss_pref(
             /*lint -esym(715, avoid_type) */ /* param not referenced */
        sd_ss_pref_reas_e_type    pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type    orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type    band_pref,
            /* New band preference (BC0, BC1, any, etc.)
            */

        sd_ss_roam_pref_e_type    roam_pref,
            /* New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type    hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type            otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type          otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        sd_ss_avoid_sys_e_type    avoid_type,
            /* Type of the system to be avoided, valid only if
            ** pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        dword                     avoid_time,
            /* Time in seconds for which the system is to be avoided,
            ** valid only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call the combined user_ss_pref function.
  */
  return sd_ss_ind_user_ss_pref2( pref_reason,
                                  orig_mode,
                                  mode_pref,
                                  band_pref,
                                  SD_SS_PRL_PREF_ANY,
                                  roam_pref,
                                  hybr_pref,
                                  otasp_band,
                                  otasp_blksys,
                                  avoid_type,
                                  avoid_time,
                                  NULL,
                                  SD_SS_SRV_DOMAIN_PREF_CS_PS,
                                  SD_SS_ACQ_ORDER_PREF_AUTO,
                                  SYS_TECH_PREF_MASK_ANY,
                                  SYS_WLAN_SCAN_PREF_AUTO,
                                  rtrn_act_ptr
                                );

} /* sd_ss_ind_user_ss_pref */
  /*lint +esym(715, avoid_type) */ /* param not referenced */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_ss_pref2

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  System Selection Preference (SS-Preference).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type                    sd_ss_ind_user_ss_pref2(
            /*lint -esym(715,avoid_type) *//* param not referenced */
        sd_ss_pref_reas_e_type              pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type              orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type              mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type              band_pref,
            /* New band preference (BC0, BC1, any, etc.)
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /* New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                      otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type                    otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        sd_ss_avoid_sys_e_type              avoid_type,
            /* Type of the system to be avoided, valid only if
            ** pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        dword                               avoid_time,
            /* Time in seconds for which the system is to be avoided,
            ** valid only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type               tech_pref,
           /* Technology preference settings.
           */

        sys_wlan_scan_pref_e_type            wlan_scan_pref,
           /* WLAN scan preference.
           */

        sd_ss_act_s_type                    *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call the new user_ss_pref3 function.
  */
  return sd_ss_ind_user_ss_pref3( pref_reason,
                                  orig_mode,
                                  mode_pref,
                                  band_pref,
                                  prl_pref,
                                  roam_pref,
                                  hybr_pref,
                                  otasp_band,
                                  otasp_blksys,
                                  avoid_type,
                                  avoid_time,
                                  manual_sys_info_ptr,
                                  domain_pref,
                                  acq_order_pref,
                                  tech_pref,
                                  wlan_scan_pref,
                                  SYS_WLAN_BSS_TYPE_ANY,
                                  rtrn_act_ptr
                                );

}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_ss_pref3

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  System Selection Preference (SS-Preference).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type                    sd_ss_ind_user_ss_pref3(
            /*lint -esym(715,avoid_type) *//* param not referenced */
        sd_ss_pref_reas_e_type              pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type              orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type              mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type              band_pref,
            /* New band preference (BC0, BC1, any, etc.)
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /* New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                      otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type                    otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        sd_ss_avoid_sys_e_type              avoid_type,
            /* Type of the system to be avoided, valid only if
            ** pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        dword                               avoid_time,
            /* Time in seconds for which the system is to be avoided,
            ** valid only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type               tech_pref,
           /* Technology preference settings.
           */

        sys_wlan_scan_pref_e_type            wlan_scan_pref,
           /* WLAN scan preference.
           */

        sys_wlan_bss_e_type                  bss_type_pref,
           /* BSS type.
           */

        sd_ss_act_s_type                    *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr         = sd_ss_ptr(SD_SS_MAIN);
  sd_ss_s_type        *ss_hdr_ptr     = sd_ss_ptr(SD_SS_HYBR_HDR);
  sd_ss_act_s_type    prev_act        = ss_ptr->act;
  sdss_pref_updated_e_type is_pref_change;
  sd_si_s_type        *si_ptr     = sd_si_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( pref_reason, SD_SS_PREF_REAS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( orig_mode, SD_SS_ORIG_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl_pref,  SD_SS_PRL_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( roam_pref, SD_SS_ROAM_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( hybr_pref, SD_SS_HYBR_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( tech_pref, SYS_TECH_PREF_MASK_ANY );
  SD_ASSERT_ENUM_IS_INRANGE( wlan_scan_pref, SYS_WLAN_SCAN_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( bss_type_pref, SYS_WLAN_BSS_TYPE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( otasp_band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( otasp_blksys, SD_BLKSYS_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_MSG_HIGH( "pref_reason=%d, orig_mode=%d, mode_pref=0x%x",
                pref_reason, orig_mode, mode_pref );
  SD_MSG_HIGH( "band_pref=0x%llx, roam_pref=0x%x, hybr_pref=%d",
                band_pref, roam_pref, hybr_pref );
  SD_MSG_HIGH( "domain_pref=0x%x, acq_order_pref=0x%x  prl_pref 0x%x",
                domain_pref, acq_order_pref, prl_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_GPSONE_MSBASED
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the hybr preference is toggled from none to on, set the start hybr
  ** hdr flag to TRUE.
  */
  if(  ( sdss_is_hybr_pref(SD_SS_HYBR_PREF_NONE) &&
         ( hybr_pref == SD_SS_HYBR_PREF_CDMA__HDR ||
           hybr_pref == SD_SS_HYBR_PREF_CDMA__WCDMA ||
           hybr_pref == SD_SS_HYBR_PREF_CDMA__HDR_WCDMA ) )
    )
  {
    SD_MSG_HIGH("Toggling on hybr pref", 0, 0, 0);
    ss_hdr_ptr->is_start_instance = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to update its SS-Preference setting.
  */
  is_pref_change = sdss_pref_update( pref_reason,
                                     orig_mode,
                                     mode_pref,
                                     band_pref,
                                     prl_pref,
                                     roam_pref,
                                     hybr_pref,
                                     otasp_band,
                                     otasp_blksys,
                                     domain_pref,
                                     acq_order_pref,
                                     tech_pref,
                                     wlan_scan_pref,
                                     bss_type_pref
                                   );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if prl preference changed.
  */
  if ( (is_pref_change == SDSS_PREF_UPDATED_TRUE_PREF   ||
        is_pref_change == SDSS_PREF_UPDATED_SRV_DOMAIN) &&
       sdss_is_prl_pref_chgd( SD_SS_MAIN )
     )
  {
    /* Update the PRL_AVAIL status on the appropriate SYS.
    */
    if( ss_ptr->ss_state == SDSS_STATE_OPR )
    {
      sdss_update_avail_sys_status( SD_SS_MAIN, SDSR_SRV_SYS );
    }
    else
    {
      sdss_update_avail_sys_status( SD_SS_MAIN, SDSR_ACQ_SYS );
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Preferences is a optimized ss pref change. just return CONTINUE.
  */
  if( sd_is_opti_ss_pref_change( SD_SS_MAIN,
                                 is_pref_change,
                                 pref_reason,
                                 orig_mode ))
  {

    SD_MSG_HIGH( "SS-Pref didn't change - CONTINUE",0,0,0 );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SS-State is operation, inform the registered client (i.e., CM) that
    ** it is OK to originate as necessary.
    */
    if( ss_ptr->ss_state == SDSS_STATE_OPR )
    {
      sd_si_ok_to_orig( SD_SS_MAIN, si_ptr->data_suspend );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( rtrn_act_ptr != NULL )
    {
      rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, at least one of the SS-Preferences was changed or
  ** the SS-State is operation and origination mode is other than none.
  */

  /* If user wants to originate an OTASP call (i.e., orig_mode == OTASP),
  ** clear the OTASP system record list and append to it all the systems that
  ** are associated with the provisioning band-class and PCS frequency block/
  ** Cellular system.
  */
  if( pref_reason != SD_SS_PREF_REAS_AVOID_SYS &&
      orig_mode == SD_SS_ORIG_MODE_OTASP )
  {
    sdsr_list_clr( SDSR_OTASP_LST );
    (void) sdprl_blksys_app( SDSR_OTASP_LST, otasp_band, otasp_blksys );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else if( pref_reason != SD_SS_PREF_REAS_AVOID_SYS &&
           orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG &&
           sdsr_list_cnt(SDSR_ALL_PREF_LST) == 0 )
  {
    sdprl_all_pref_sys_app( SDSR_ALL_PREF_LST );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else if ( pref_reason == SD_SS_PREF_REAS_USER_RESEL &&
            sdss_is_orig_mode0( orig_mode, SD_SS_ORIG_MODE_VAL_MANUAL) )
  {

    sd_update_manual_sys( SDSR_MANUAL_LST, manual_sys_info_ptr );

  } /* else if ( pref_reas == ... ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else if ( pref_reason == SD_SS_PREF_REAS_AVOID_SYS)
  {
    #ifdef FEATURE_USERZONE
    if (avoid_type == SD_SS_AVOID_SYS_IDLE_IF_UZ)
    {
      /* Check if the MS has selected a user zone currently
      */
      if (si_ptr->si_info.curr_uz_info.uz_id == SD_INVALID_UZID)
       {
         /* Currently not in user zone, so return continue.
         */
         return SD_SS_ACT_CONTINUE;
       }
     }
     #endif /* FEATURE_USERZONE */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Avoid SID/WILDCARD_NID
    */
    if ( !sdprl_add_lock_sid_nid( sdsr_list_get_ptr(SDSR_IDL_SYS, 0 ), TRUE ))
    {
      SD_MSG_HIGH("SID/NID not locked, table full", 0, 0, 0 );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Avoid band/chan for 60 seconds.
    */
    avoid_time = (dword)60;
    sdss_sr_list_avoid2( SD_SS_MAIN, SDSR_IDL_SYS, avoid_time );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If orig mode is changed from manual mode, undefine the plmn in in user
  ** list.
  */
  //if( sdss_is_orig_mode( sdss_get_prev_orig_mode( SD_SS_MAIN ),
  //                       SD_SS_ORIG_MODE_VAL_MANUAL)
  //                    &&
  //    ! sdss_is_orig_mode( orig_mode, SD_SS_ORIG_MODE_VAL_MANUAL )
  //  )
  //{
  //  sdsr_list_undefine_plmn_id( SDSR_USER_LST );
  //}

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_SS_PREF, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE |
                    SD_SS_ACT_ACQ_GPS  ));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If action is equal to previous returned action, just continue - no need
  ** to instruct the SS-Client of the same action twice.
  */
  if( sd_ss_act_is_equal(&ss_ptr->act, &prev_act, ss_ptr->meas_list_is_diff))
  {
    SD_MSG_LOW( "Same action - CONTINUE",0,0,0 );

    /* If SS-Action is MEAS_XXX, set the measurement ID per the pervious
    ** SS-Action's measurement ID.
    */
    if( ss_ptr->act.act == SD_SS_ACT_MEAS_BACK ||
        ss_ptr->act.act == SD_SS_ACT_MEAS_DED )
    {
      ss_ptr->meas_id = prev_act.prm.meas.id;
    }

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( rtrn_act_ptr != NULL )
    {
      rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    act = SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-State is operation and SS-Action is CONTINUE or background
  ** measurement, inform the registered client (i.e., CM) that it is OK to
  ** originate as necessary.
  */
  if( (ss_ptr->ss_state == SDSS_STATE_OPR) &&
      (act == SD_SS_ACT_CONTINUE || act == SD_SS_ACT_MEAS_BACK) )
  {
    sd_si_ok_to_orig( SD_SS_MAIN, si_ptr->data_suspend );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return act;

} /* sd_ss_ind_user_ss_pref2 */
  /*lint +esym(715,avoid_type) */ /* param not referenced */





/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_nam_sel

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  NAM selection or information.

DEPENDENCIES
  sd_init() and must have already been called to initialize SD.

  sd_nam_sel() must have already been called to load the selected NAM's PRL
  from NV.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_user_nam_sel(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If operating mode is not online, call the power up indication.
  */
  if ( sdss_get_opr_mode( SD_SS_MAIN ) != SDSS_OPR_MODE_ONLINE )
  {
    SD_MSG_HIGH("Converting nam_sel to pwr_up SS-Ind", 0, 0, 0 );
    act = sd_ss_ind_user_pwr_up( rtrn_act_ptr );
    return act;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_NAM_SEL, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_ACQ_GPS  |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE |
                    SD_SS_ACT_PWR_SCAN_GW) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_user_nam_sel */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_pwr_down

DESCRIPTION
  Inform SD that the user commanded the MS power-off, perform a reset or
  enter LPM.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_user_pwr_down(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Deactivate SS-Timer??
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to update its operating mode.
  */
  sdss_opr_mode_update( SDSS_OPR_MODE_PWR_DOWN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the System Indication component that the MS is powering down.
  */
  sd_si_srv_offline( SDSS_OPR_MODE_PWR_DOWN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save PRL-Component related information to NV.
  */
  sdprl_save();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_PWR_DOWN, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act == SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_user_pwr_down */


#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */


#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
/* <EJECT> */
/*===========================================================================
=============================================================================
====================== CDMA SPECIFIC HELPER FUNCTIONS =======================
=============================================================================
===========================================================================*/
/**==========================================================================

@FUNCTION sd_si_update_cdma_time

@DESCRIPTION
  Updates CDMA -LTM offset and daylight savings info and leap seconds in 
  sd_si_info object

@DEPENDENCIES
  None

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
static void                      sd_si_update_cdma_time(

        sd_ss_e_type              ss,
        /**< System selection - MAIN */

        uint8                     ltm_offset,
        /**< Ltm offset on acquired CDMA system */

        sys_daylt_savings_e_type  daylt_savings,
        /**< Daylight savings on acquired CDMA system */

        byte                      leap_secs
        /**< Leap seconds */
)
{
  sd_si_s_type          *si_ptr       = sd_si_ptr( ss );
  sd_si_info_s_type     *info_ptr     = &si_ptr->si_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ltm_offset != SYS_LTM_OFFSET_INVALID)
  {
    info_ptr->time_info.sys_mode = SYS_SYS_MODE_CDMA;
  }
  else
  {
    /* If ltm offset is invalid ,then indicate that the time info is invalid
    ** by setting the sys mode to NO SRV
    */
    info_ptr->time_info.sys_mode = SYS_SYS_MODE_NO_SRV;
  }

  info_ptr->time_info.time.cdma_time.ltm_offset     = ltm_offset;

  info_ptr->time_info.time.cdma_time.daylt_savings  = daylt_savings;

  info_ptr->time_info.time.cdma_time.lp_sec         = leap_secs;

  SD_MSG_MED("mode %d ltm %d daylt %d", info_ptr->time_info.sys_mode,
               ltm_offset,daylt_savings);
  SD_MSG_MED("lp_sec %d", leap_secs, 0, 0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* sd_si_update_cdma_time */



/* <EJECT> */
/*===========================================================================
=============================================================================
====================== CDMA ACQUISITION SS-INDICATIONS ======================
=============================================================================
===========================================================================*/

/* Indications that are specific to CDMA pilot and sync channel acquisition,
** such as pilot acquisition failure or protocol mismatch. These indications
** can be received from the SS-Client during CDMA pilot and sync channel
** acquisition only; that is, before the SS-Client enters the CDMA idle
** state.
*/


/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_failed_pilot

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to failure to acquire the pilot channel.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_acq_failed_pilot(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_ACQ_FAILED_PILOT, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_acq_failed_pilot */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_failed_sync

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to failure to successfully receive the sync channel message.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_acq_failed_sync(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_ACQ_FAILED_SYNC, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_acq_failed_sync */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_failed_unlock_rf

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to unlock RF resources.

  This indication is called before CDMA protocol acquired the sync channel
  messages.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_acq_failed_unlock_rf(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH( "cdma acq fail unlock rf", 0, 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the last true action.
  */
  act = sd_ss_act_get( rtrn_act_ptr );

  SD_MSG_HIGH( "Get true action= %d ", act, 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_prot_mis

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to protocol mismatch (i.e., MOB_P_REV < MIN_P_REV or MS does not
  support the Paging Channel data rate).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_acq_prot_mis(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_ACQ_PROT_MIS, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_acq_prot_mis */


/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_prot_mis

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA idle handoff
  fails due to protocol mismatch (i.e., MOB_P_REV < MIN_P_REV or MS does not
  support the Paging Channel data rate).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_prot_mis(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_PROT_MIS, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_prot_mis */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_schm2

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when successfully receives
  the CDMA Sync Channel Message (SCHM) and the protocol revision is
  compatible.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  The search algorithm should find the strongest energy peak around the PN
  circle (rather than the first strong enough peak).

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_ss_act_e_type          sd_ss_ind_cdma_acq_schm2(

        word                      sid,
            /* SID from SCHM.
            */

        word                      nid,
            /* NID from SCHM.
            */

        boolean                   is_from_sync,
            /* Indicates whether this function is being called from sync or
            ** paging.
            */

        byte                      ltm_off,
            /* LTM Offset
            */

        sys_daylt_savings_e_type  daylt_savings,
            /* Daylight savings ind
            */

        byte                      leap_secs,
            /* Leap seconds
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type      act;

  /* Point at acquired system.
  */
  sdsr_ref_type       acq_sys_ref   = sdsr_list_get_ref( SDSR_ACQ_SYS, 0 );
  sdsr_s_type         *acq_sys_ptr  = sdsr_list_get_ptr( SDSR_ACQ_SYS, 0 );

  /* Point at candidate system.
  */
  sdsr_s_type         *cand_sys_ptr = sdsr_list_get_ptr( SDSR_CAND_SYS, 0 );

  /* Remember whether the service indicator is ON.
  */
  boolean             is_srv_on     = sd_si_is_srv_on( SD_SS_MAIN );

  sdss_sys_addn_info_s_type curr_sys_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_sys_ref != SDSR_REF_NULL );
  if(acq_sys_ptr == NULL)
  {
    SD_ERR_FATAL(" acq_sys_ptr NULL",0,0,0);
  }
  SD_ASSERT(BETWEEN(daylt_savings, SYS_DAYLT_SAVINGS_NONE, SYS_DAYLT_SAVINGS_MAX));

  SD_MSG_HIGH( "cdma_acq_schm, sid=%d, nid=%d", sid, nid, 0 );

  SD_MSG_HIGH( "cdma_acq_schm, ltm_off=%d, daylt_savings=%d", ltm_off, daylt_savings, 0 );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the additional sys info structure */
  sdss_addn_sys_info_init(&curr_sys_info);

  /* Update the SID/NID of the acquired system per the SCHM's SID/NID.
  */
  acq_sys_ptr->sid.is95.sid = sid;
  acq_sys_ptr->sid.is95.nid = nid;

  /* If this function is being called from sync, update the true SID/NID
  ** fields.
  */
  if( is_from_sync )
  {
    acq_sys_ptr->sid.is95.true_sid = sid;
    acq_sys_ptr->sid.is95.true_nid = nid;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the PRL designation, GEO list and more preferred list of the
  ** acquired system.
  */
  (void) sdprl_get_info(
                  SD_SS_MAIN,          /* MAIN system selection */
                  SDSR_ACQ_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_ACQ_GEO_LST,    /* Put GEO systems into this list */
                  SDSR_ACQ_PREF_LST,   /* Put PREF systems into this list */
                  SDSR_ACQ_COLLOC_LST, /* Put collocated sys in this list */
                  SDPRL_EXTRACT_ALL ); /* Get info regardless of PRL match */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Insert the acquired system at the beginning of the SDSR_ACQED_LST and
  ** remove any duplicates.
  */
  sdsr_list_insert_ref( SDSR_ACQED_LST, 0, acq_sys_ref, TRUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the following conditions are TRUE, return CONTINUE in order to get
  ** the MCC/IMSI_11_12 from the paging channel before deciding whether to
  ** stay on the acquired system:
  **
  ** 1. There is no explicit PRL match for the SID/NID,
  ** 2. This function is being called from sync.
  ** 3. The PRL contains MCC/IMSI_11_12 system records.
  ** 4. This acquisition is not for a specific SID.
  */
  if( acq_sys_ptr->match_lvl < (byte) SDPRL_MATCH_WILDCARD_NID &&
      is_from_sync &&
      prl_hdr_get_ptr()->is_mcc_sys_rec &&
      !sdss_is_acq_specific_sid(SD_SS_MAIN) )
  {
    if( acq_sys_ptr->match_lvl < (byte) SDPRL_MATCH_WILDCARD_SID ) {
      SD_MSG_LOW( "No SID match - trying MCC/IMSI_11_12",0,0,0 );
    }
    else {
      SD_MSG_LOW( "Wildcard SID match - trying MCC/IMSI_11_12",0,0,0 );
    }

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( rtrn_act_ptr != NULL ) {
      rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do auto-NAM processing.
  */
  sdprl_autonam_proc( acq_sys_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the acquired system does not conflict with the current SS-Preference
  ** and it is designated as an available in the PRL, append the acquired
  ** system to the AVAIL_LST list.
  */
  if( sdss_sr_is_pref_conf(SD_SS_MAIN, acq_sys_ptr, SDSS_PREF_CONF_NONE) &&
      sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_AVAIL) )
  {
    sdsr_list_app( SDSR_AVAIL_LST, SDSR_ACQ_SYS );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_ACQ_SCHM, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE (i.e., we have selected a new serving system),
  ** do the following updates.
  */
  if( act == SD_SS_ACT_CONTINUE )
  {

    /* Update cdma time in si_info */
    sd_si_update_cdma_time (SD_SS_MAIN, ltm_off, daylt_savings, leap_secs);

    #ifdef FEATURE_JCDMA

    /* Keep the expected SID/NID of acq system if it is  wildcard and
    ** redirection is on.
    **
    ** Acq system could be the redirected system or idle system ( call
    ** release would trigger an attempt on the idle system )
    **
    */
    if( ! ( sdss_is_redir( SD_SS_MAIN, SDSS_REDIR_VAL_CDMA_ANY )
            && acq_sys_ptr->sid_expected.prm.is95.sid == PRL_WILDCARD_SID ))

    #endif /* FEATURE_JCDMA */
    {

      /* Update the expected SID/NID of the acquired system per the acquired
      ** SID/NID.
      */
      acq_sys_ptr->sid_expected.prm.is95.sid = sid;
      acq_sys_ptr->sid_expected.prm.is95.nid = nid;
    }

    /* Get the additional system info of the acquired system.
    */
    sdprl_get_addn_sys_record_info(acq_sys_ptr, 
                                   &(curr_sys_info.assn_tag),
                                   &(curr_sys_info.geo_offset),
                                   &(curr_sys_info.geo_count));

    /* Update the serving system per the acquired system.
    */
    sd_ss_srv_sys_update( SD_SS_MAIN,
                          SDSR_ACQ_SYS,
                          SDSR_ACQ_GEO_LST,
                          SDSR_ACQ_PREF_LST,
                          SDSR_ACQ_COLLOC_LST );

    /* Update the system indicators per the acquired system.
    */
    sd_si_update_if_srv_is_off( SD_SS_MAIN,
                                SD_SI_UPDATE_ACQ,
                                SYS_SRV_STATUS_SRV,
                                SYS_SRV_DOMAIN_CS_PS,
                                acq_sys_ref,
                                NULL,
                                NULL,
                                NULL,
                                SDSR_ACQ_COLLOC_LST,
                                NULL,
                                &curr_sys_info,
                                SYS_SRV_STATUS_SRV
                               );


    if( !is_srv_on )
    {
      /* Update the additional system info in sd state with that of 
      ** the acquired system.
      */
      sdss_set_prev_sys_info(SD_SS_MAIN,  
                             curr_sys_info.assn_tag,
                             curr_sys_info.geo_offset,
                             curr_sys_info.geo_count);
    }


    /* Update the candidate system per the acquired system.
    */
    sdsr_list_put_ref( SDSR_CAND_SYS, acq_sys_ref );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (we have not yet selected a serving system), if the acquired system
  ** is allowed by the PRL and complies with the current SS-Preference do the
  ** following updates:
  */
  else if(sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ALLOWED) &&
          sdss_sr_is_pref_conf(SD_SS_MAIN, acq_sys_ptr, SDSS_PREF_CONF_NONE))
  {
    /* If the candidate system is NULL or the acquired system is more
    ** preferred than the candidate system, update the candidate system per
    ** the acquired system and reorder the available list according to the
    ** new candidate system.
    */
    if( cand_sys_ptr == NULL ||
        sdprl_is_more_pref(SD_SS_MAIN, acq_sys_ptr, cand_sys_ptr) )
    {
      sdsr_list_put_ref( SDSR_CAND_SYS, acq_sys_ref );
      sdsr_list_order( SDSR_AVAIL_LST, SDSR_CAND_SYS );
    }

    /* If before processing the SS-Event the service indicator was OFF,
    ** update the system indicators per the acquired system.
    */
    if( ! is_srv_on )
    {
      /* Get the additional system info of the acquired system.
      */
      sdprl_get_addn_sys_record_info(acq_sys_ptr, 
                                     &(curr_sys_info.assn_tag),
                                     &(curr_sys_info.geo_offset),
                                     &(curr_sys_info.geo_count));

      /* Update time info in si_info */
      sd_si_update_cdma_time (SD_SS_MAIN, ltm_off, daylt_savings, leap_secs);

      sd_si_update( SD_SS_MAIN,
                    SD_SI_UPDATE_ACQ,
                    SYS_SRV_STATUS_SRV,
                    SYS_SRV_DOMAIN_CS_PS,
                    acq_sys_ref,
                    FALSE,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    SDSR_ACQ_COLLOC_LST,
                    NULL,
                    &curr_sys_info,
                    SYS_SRV_STATUS_SRV
                  );

      sdss_set_prev_sys_info(SD_SS_MAIN,  
                             curr_sys_info.assn_tag,
                             curr_sys_info.geo_offset,
                             curr_sys_info.geo_count);
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_acq_schm2 */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_schm3

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when successfully receives
  the CDMA Sync Channel Message (SCHM) and the protocol revision is
  compatible.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_act_e_type          sd_ss_ind_cdma_acq_schm3 (

        word                      sid,
            /* SID from SCHM.
            */

        word                      nid,
            /* NID from SCHM.
            */

        byte                      ltm_off,
            /* LTM Offset
            */

        sd_daylt_savings_e_type  daylt_savings,
            /* Daylight savings ind
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  /* Not featurizing this code based on FEATURE_HS_BASED_PLUS_DIAL_DISPLAY since
  ** CM will not use the information on ltm, daylt if the feature is not turned
  ** on.
  */

  SD_MSG_HIGH ("===========sd_ss_ind_cdma_acq_schm3=======", 0, 0, 0);
  return sd_ss_ind_cdma_acq_schm2( sid,
                                   nid,
                                   TRUE,
                                   ltm_off,
                                   (sys_daylt_savings_e_type)daylt_savings,
                                   SYS_LEAP_SECONDS_NO_INFO,
                                   rtrn_act_ptr );
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_schm4

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when successfully receives
  the CDMA Sync Channel Message (SCHM) and the protocol revision is
  compatible. Leap seconds was added to the interface between SD and CP as
  compared to sd_ss_ind_cdma_acq_schm3

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_act_e_type          sd_ss_ind_cdma_acq_schm4 (

        word                      sid,
            /* SID from SCHM.
            */

        word                      nid,
            /* NID from SCHM.
            */

        byte                      ltm_off,
            /* LTM Offset
            */

        sys_daylt_savings_e_type  daylt_savings,
            /* Daylight savings ind
            */

        byte                      leap_secs,
            /* Leap seconds
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  SD_MSG_HIGH ("===========sd_ss_ind_cdma_acq_schm4=======", 0, 0, 0);
  return sd_ss_ind_cdma_acq_schm2( sid,
                                   nid,
                                   TRUE,
                                   ltm_off,
                                   daylt_savings,
                                   leap_secs,
                                   rtrn_act_ptr );
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_schm

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when successfully receives
  the CDMA Sync Channel Message (SCHM) and the protocol revision is
  compatible.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  The search algorithm should find the strongest energy peak around the PN
  circle (rather than the first strong enough peak).

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_acq_schm(

        word                      sid,
            /* SID from SCHM.
            */

        word                      nid,
            /* NID from SCHM.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_ind_cdma_acq_schm2( sid,
                                   nid,
                                   TRUE,
                                   SYS_LTM_OFFSET_INVALID,
                                   SYS_DAYLT_SAVINGS_NO_INFO,
                                   SYS_LEAP_SECONDS_NO_INFO,
                                   rtrn_act_ptr );
}
/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_ho_fail

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the handoff from
  HDR fails

DEPENDENCIES
  sd_init() must have already been called to initialize SD.


RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_acq_ho_fail(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_ACQ_HO_FAIL, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

}

#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */



#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
/* <EJECT> */
/*===========================================================================
=============================================================================
======================= CDMA OPERATION SS-INDICATIONS =======================
=============================================================================
===========================================================================*/

/* Indications that are specific to CDMA operation, such as CDMA Redirection
** or a CDMA new system. These indications can be received from the SS-Client
** during CDMA operation only; that is, after the SS-Client enters the CDMA
** idle state.
*/

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_oh_info3

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the CDMA overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff) and receives the last required overhead
       message on that channel.

    2. The SS-Client updates the overhead information of the acquired system.
       Note that this happen when the base station changes CONFIG_MSG_SEQ and
       the SS-Client receives the last required overhead message with the new
       CONFIG_MSG_SEQ.

  NOTE! This indication should not be called when the SS-Client is just
  passing through on the way to a re-hash.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Attempt acquiring a GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_oh_info3(

        sd_band_e_type            band,
            /* Serving system's band class.
            */

        sd_chan_type              chan,
            /* Serving system's CDMA channel.
            */

        word                      sid,
            /* Serving system's SID.
            */

        word                      nid,
            /* Serving system's NID.
            */

        boolean                   is_pending_gsrdm,
            /* Indicate whether a GSRDM is pending.
            */

        word                      mcc,
            /* Serving system's MCC.
            */
        byte                      imsi_11_12,
            /* Serving system's IMISI_11_12.
            */

        sys_srch_win_type         srch_win_n,
            /* Neighbor set window search size.
            */

        sys_base_lat_type         base_lat,
            /* Base station latitude.
            */

        sys_base_long_type        base_long,
            /* Base station latitude.
            */

        sys_base_id_type          base_id,
            /* Base station Id.
            */

        sd_ss_ovhd_status_e_type  ovhd_status,
           /* Indicate if the overhead message is complete.
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type      act;
  sd_ss_s_type          *ss_ptr           = sd_ss_ptr(SD_SS_MAIN);
  boolean               chan_is_changed   = FALSE;
  boolean               band_is_changed   = FALSE;
  boolean               sid_is_changed    = FALSE;
  boolean               is_prl_match      = FALSE;
  sdss_state_e_type     prev_ss_state;
  word                  sid_expected;
  word                  nid_expected;
  sdprl_extract_e_type  extract           = SDPRL_EXTRACT_ALL;
  sdsr_ref_type         srv_sys_ref;
  sdsr_s_type           *srv_sys_ptr;
  sdsr_s_type           *ho_sys_ptr;
  sd_si_mode_info_u_type  mode_info;
  sd_si_s_type          *si_ptr           = sd_si_ptr(SD_SS_MAIN);
  sdsr_s_type           *acq_sys_ptr  = sdsr_list_get_ptr( SDSR_ACQ_SYS, 0 );
  const prl_hdr_s_type  *prl_hdr_ptr  = prl_hdr_get_ptr();
  sdss_sys_addn_info_s_type curr_sys_info;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );
  SD_ASSERT( sdss_is_supp_sys(SD_MODE_CDMA, sdss_map_band_to_band_pref(band), chan) );
  SD_ASSERT_ENUM_IS_INRANGE( ovhd_status, SD_SS_OVHD_STATUS_MAX );
  SD_ASSERT( acq_sys_ptr != NULL );

  SD_MSG_HIGH( "CDMA oh, sid=%d, nid=%d, chan=%d", sid, nid, chan );
  SD_MSG_HIGH( "band=%d, mcc=%d, imsi_11_12=%d", band, mcc, imsi_11_12 );
  SD_MSG_HIGH( "ovhd_status %d", ovhd_status, 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the additional sys info structure */
  sdss_addn_sys_info_init(&curr_sys_info);

  /* If SS-State is acquisition (i.e. we were holding on processing sync in
  ** order to get the MCC/IMSI_11_12 from the paging channel), call on
  ** sd_ss_ind_cdma_acq_schm2 first.
  */
  if( ss_ptr->ss_state == SDSS_STATE_ACQ )
  {
    /* If MCC/IMSI_11_12 are provided, update the MCC and IMSI_11_12 fields
    ** of the ACQ_SYS, as well as encode the MCC/IMSI_11_12 into the SID/NID
    ** fields.
    */
    //if( mcc != SD_MCC_NONE )
    if( mcc < SD_MCC_MAX &&
        imsi_11_12 < SD_IMSI_11_12_MAX )
    {
      acq_sys_ptr->sid.is95.mcc         = mcc;
      acq_sys_ptr->sid.is95.imsi_11_12  = imsi_11_12;
      sdsr_sid_encode_mcc_imsi_11_12( acq_sys_ptr );
    }

    /* Process SCHM.
    */
    act = sd_ss_ind_cdma_acq_schm2( acq_sys_ptr->sid.is95.sid,
                                    acq_sys_ptr->sid.is95.nid,
                                    FALSE,
                                    SYS_LTM_OFFSET_INVALID,
                                    SYS_DAYLT_SAVINGS_NO_INFO,
                                    SYS_LEAP_SECONDS_NO_INFO,
                                    rtrn_act_ptr );

    /* If there is no explicit PRL match for the MCC/IMSI_11_12, restore the
    ** true SID/NID values.
    */
    if( acq_sys_ptr->match_lvl < (byte) SDPRL_MATCH_WILDCARD_NID )
    {
      SD_MSG_LOW( "No PRL match for MCC/IMSI_11_12",0,0,0 );
      sdsr_sid_restore_true( acq_sys_ptr );
    }

    /* If SS-Action is other than CONTINUE return now.
    */
    if( act != SD_SS_ACT_CONTINUE )
    {
      return act;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at serving and idle systems.
  */
  srv_sys_ref   = sdsr_list_get_ref( SDSR_SRV_SYS, 0 );
  srv_sys_ptr   = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );
  ho_sys_ptr    = sdsr_list_get_ptr( SDSR_HO_SYS, 0 );

  SD_ASSERT( srv_sys_ref != SDSR_REF_NULL );
  if(srv_sys_ptr == NULL)
  {
    SD_ERR_FATAL(" srv_sys_ref NULL",0,0,0);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out whether band/channel has changed.
  */
  if( srv_sys_ptr->sys.band !=  sdss_map_band_to_band_pref(band) )
  {
    band_is_changed = TRUE;
    chan_is_changed = TRUE;
  }
  else if( srv_sys_ptr->sys.chan != chan  )
  {
    chan_is_changed = TRUE;
  }

  /* Find out whether the SID/NID has changed.
  */
  if( srv_sys_ptr->sid.is95.true_sid   != sid ||
      srv_sys_ptr->sid.is95.true_nid   != nid  )
  {
    sid_is_changed = TRUE;
  }

  /* Find out whether the MCC/IMSI_11_12 has changed.
  */
  if( srv_sys_ptr->sid.is95.mcc        != mcc ||
      srv_sys_ptr->sid.is95.imsi_11_12 != imsi_11_12 )
  {
    if ( prl_hdr_ptr->is_mcc_sys_rec == TRUE )
    {
      sid_is_changed = TRUE;
    }

    /* Update MCC/IMSI_11_12, regardless of is_mcc_sys_rec */
    srv_sys_ptr->sid.is95.mcc        = mcc;
    srv_sys_ptr->sid.is95.imsi_11_12 = imsi_11_12;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If serving system's information has changed, do the following updates:
  */
  if( chan_is_changed ||
      sid_is_changed )
  {
    /* Remember the SID/NID of the previous serving system.
    */
    sid_expected = srv_sys_ptr->sid.is95.sid;
    nid_expected = srv_sys_ptr->sid.is95.nid;

    /* If band is changed but SID/NID is not changed, set sid_expected to
    ** SD_NEW_SID to indicate that this is a new system. We need to do this
    ** because the SID/NID is not unique across different bands.
    */
    if( band_is_changed &&
        ! sid_is_changed )
    {
      sid_expected = SD_NEW_SID;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Store the new information in the system record table and set the
    ** SRV_SYS to reference the stored record.
    */
    srv_sys_ref = sdsr_tbl_store5( SD_MODE_CDMA,
                                   band,
                                   chan,
                                   sid,
                                   nid,
                                   srv_sys_ptr->prl,
                                   srv_sys_ptr->roam,
                                   srv_sys_ptr->prl_assn );

    sdsr_list_put_ref( SDSR_SRV_SYS, srv_sys_ref );
    srv_sys_ptr = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );
    if(srv_sys_ptr == NULL)
    {
      SD_ERR_FATAL(" srv_sys_ptr NULL",0,0,0);
    }
    /* Update the MCC/IMSI_11_12.
    */
    srv_sys_ptr->sid.is95.mcc         = mcc;
    srv_sys_ptr->sid.is95.imsi_11_12  = imsi_11_12;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_JCDMA

    if( sdss_is_redir( SD_SS_MAIN, SDSS_REDIR_VAL_CDMA_ANY )
        && ( acq_sys_ptr->sid_expected.prm.is95.sid == PRL_WILDCARD_SID ))
    {
      /* Set the expected SID of the new serving system to wildcard if the
      ** expected SID of the acq system is wildcard and redirection is on
      */
      srv_sys_ptr->sid_expected.prm.is95.sid = PRL_WILDCARD_SID;
    }
    else

    #endif /* FEATURE_JCDMA */
    {
      /* Update the expected SID/NID of the new
      ** serving system per the SID/NID
      ** of the previous serving system.
      */
      srv_sys_ptr->sid_expected.prm.is95.sid = sid_expected;
      srv_sys_ptr->sid_expected.prm.is95.nid = nid_expected;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Get the PRL designation, GEO list and more preferred list of the
    ** new serving system.
    **
    ** Note that if SID/NID is not changed and band is not changes, we only
    ** update the PRL related information if the new serving system has an
    ** explicit PRL match - else we continue to use the current PRL
    ** information. This is done so because the PRL might not list systems
    ** (i.e., SID/NID, mode, band and channel combinations) to which MS can
    ** get through channel hashing or idle handoff.
    */
    if( ! sid_is_changed &&
        ! band_is_changed )
    {
      extract = SDPRL_EXTRACT_MATCH_ONLY;
    }

    is_prl_match =
              sdprl_get_info(
                  SD_SS_MAIN,          /* MAIN system selection */
                  SDSR_SRV_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_SRV_GEO_LST,    /* Put GEO systems into this list */
                  SDSR_SRV_PREF_LST,   /* Put PREF systems into this list */
                  SDSR_SRV_COLLOC_LST, /* Put collocated sys in this list */
                  extract );           /* Get info if diff SID or PRL match*/

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the following conditions are TRUE, try to match the MCC/IMSI_11_12:
    **
    ** 1. There is no explicit PRL match for the SID/NID,
    ** 2. The PRL contains MCC_IMSI_11_12 system records
    ** 3. MCC/IMSI_11_12 are provided
    ** 4. Acquisition is not for a specific SID.
    */
    //if( !is_prl_match &&
    if( (!is_prl_match || srv_sys_ptr->match_lvl < (byte) SDPRL_MATCH_WILDCARD_NID) &&
        prl_hdr_ptr->is_mcc_sys_rec &&
        mcc < SD_MCC_MAX &&
        imsi_11_12 < SD_IMSI_11_12_MAX &&
        !sdss_is_acq_specific_sid(SD_SS_MAIN) )
    {

      /* Encode the MCC/IMSI_11_12 into the SID/NID.
      */
      sdsr_sid_encode_mcc_imsi_11_12( srv_sys_ptr );

      /* Look for a PRL match.
      */
      is_prl_match =
              sdprl_get_info(
                  SD_SS_MAIN,          /* MAIN system selection */
                  SDSR_SRV_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_SRV_GEO_LST,    /* Put GEO systems into this list */
                  SDSR_SRV_PREF_LST,   /* Put PREF systems into this list */
                  SDSR_SRV_COLLOC_LST, /* Put collocated sys in this list */
                  extract );           /* Get info if diff SID or PRL match*/

      /* If there is no explicit PRL match for the MCC/IMSI_11_12, restore
      ** the true SID/NID values.
      */
      if( !is_prl_match ||
          srv_sys_ptr->match_lvl < (byte) SDPRL_MATCH_WILDCARD_NID )
      {
        sdsr_sid_restore_true( srv_sys_ptr );
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Insert the serving system at the beginning of the SDSR_ACQED_LST and
    ** remove any duplicates.
    */
    sdsr_list_insert_ref( SDSR_ACQED_LST, 0, srv_sys_ref, TRUE );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Do auto-NAM processing.
    */
    sdprl_autonam_proc( srv_sys_ptr );

  } /* if( Channel or SID is changed ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remember the current SS-State.
  */
  prev_ss_state = ss_ptr->ss_state;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if BSR timer uptime needs to be reset
  */
  sdss_check_bsr_timer();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication as
  ** follows:
  **
  **   1. If SS-State is not yet operation, SID/NID has changed or band has
  **      changed, process SDSS_EVT_CDMA_OPR_OH_SID.
  **   2. Else, if channel has changed, process SDSS_EVT_CDMA_OPR_OH_CHAN.
  **   3. Else, process SDSS_EVT_MISC_FILLER.
  */
  if( ss_ptr->ss_state != SDSS_STATE_OPR ||
      sid_is_changed ||
      band_is_changed )
  {
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_OH_SID, rtrn_act_ptr );
  }
  else if( chan_is_changed )
  {
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_OH_CHAN, rtrn_act_ptr );
  }
  else
  {
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_FILLER, rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is continue or background mode measurement, do the
  ** following updates.
  ** If the overhead is incomplete, CP will not be idle even though
  ** the action is CONTINUE.
  */
  if( ( act == SD_SS_ACT_CONTINUE &&
        ovhd_status == SD_SS_OVHD_STATUS_COMPLETE ) ||
      act == SD_SS_ACT_MEAS_BACK )
  {
    #ifdef FEATURE_JCDMA

    /* Set the expected SID of the new serving system to wildcard if the
    ** expected SID of the acq system is wildcard and redirection is on.
    */
    if( sdss_is_redir( SD_SS_MAIN, SDSS_REDIR_VAL_CDMA_ANY )
        && ( acq_sys_ptr->sid_expected.prm.is95.sid == PRL_WILDCARD_SID ))
    {
      srv_sys_ptr->sid_expected.prm.is95.sid = PRL_WILDCARD_SID;
    }
    else

    #endif /* FEATURE_JCDMA */
    {
      /* Update the expected SID/NID of the serving system per the Overhead
      ** Info's SID/NID.
      */
      srv_sys_ptr->sid_expected.prm.is95.sid = sid;
      srv_sys_ptr->sid_expected.prm.is95.nid = nid;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If previous SS-State was not operation, update the idle system, its
    ** GEO and its PREF lists per the initial acquisition system.
    */
    if( prev_ss_state != SDSS_STATE_OPR )
    {
      sd_ss_idle_sys_update( SD_SS_MAIN,
                             SDSR_ACQ_SYS,
                             SDSR_ACQ_GEO_LST,
                             SDSR_ACQ_PREF_LST );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else (previous SS-State is operation) if handoff designation is
    ** CDMA CAM to CDMA paging, update the idle system per the handoff
    ** system.
    */
    else if( sdss_is_handoff(SDSS_HO_VAL_CDMA_CAM_TO_CDMA_PAG) &&
             ho_sys_ptr != NULL )
    {
      ho_sys_ptr->prl   = srv_sys_ptr->prl;
      ho_sys_ptr->roam  = srv_sys_ptr->roam;

      sd_ss_idle_sys_update( SD_SS_MAIN,
                             SDSR_HO_SYS,
                             SDSR_IDL_GEO_LST,
                             SDSR_IDL_PREF_LST );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SID/NID is changed, band is changed or channel is changed and the
    ** new serving system is a PRL system that is explicitly listed in the
    ** PRL's acquisition table, update the IDL_SYS, IDL_GEO and IDL_PREF
    ** lists per the SRV_SYS, SRV_GEO and SRV_PREF lists.
    */
    if( sid_is_changed
              ||
        band_is_changed
              ||
        (chan_is_changed &&
         is_prl_match &&
         sdsr_list_find(SDSR_PRL_LST, srv_sys_ptr) != SDSR_POS_NULL) )
    {
      sd_ss_idle_sys_update( SD_SS_MAIN,
                             SDSR_SRV_SYS,
                             SDSR_SRV_GEO_LST,
                             SDSR_SRV_PREF_LST );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else if channel is changed, only update the IDL_GEO and IDL_PREF lists
    ** per the SRV_GEO and SRV_PREF lists.
    */
    else if( chan_is_changed )
    {
      sd_ss_idle_sys_update( SD_SS_MAIN,
                             SDSR_MAX,
                             SDSR_SRV_GEO_LST,
                             SDSR_SRV_PREF_LST );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If there is no pending GSRDM, update the service indicators per the
    ** new serving system now. Else leave the service indicators unchanged -
    ** they will be updated upon processing the GSRDM.
    */
    if( ! is_pending_gsrdm )
    {
      /* Populate the mode specific info.
      */
      mode_info.cdma.base_id = base_id;
      mode_info.cdma.base_lat = base_lat;
      mode_info.cdma.base_long = base_long;
      mode_info.cdma.srch_win_n = srch_win_n;

      /* Get the additional system info of the acquired system.
      */
      sdprl_get_addn_sys_record_info(acq_sys_ptr, 
                                     &(curr_sys_info.assn_tag),
                                     &(curr_sys_info.geo_offset),
                                     &(curr_sys_info.geo_count));

      sd_si_update( SD_SS_MAIN,
                    SD_SI_UPDATE_OH,
                    SYS_SRV_STATUS_SRV,
                    SYS_SRV_DOMAIN_CS_PS,
                    srv_sys_ref,
                    TRUE,
                    NULL,
                    NULL,
                    NULL,
                    &mode_info,
                    SDSR_SRV_COLLOC_LST,
                    NULL,
                    &curr_sys_info,
                    SYS_SRV_STATUS_SRV
                  );

      /* Update the additional system info in sd state with that of 
      ** the acquired system.
      */
      sdss_set_prev_sys_info(SD_SS_MAIN,  
                             curr_sys_info.assn_tag,
                             curr_sys_info.geo_offset,
                             curr_sys_info.geo_count);
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Inform the registered client (i.e., CM) that it is OK to originate as
    ** necessary.
    */
    sd_si_ok_to_orig( SD_SS_MAIN, si_ptr->data_suspend );


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Reset any throttle counters if running.
    ** necessary.
    */
    sdss_throttle_reset_cntrs( SD_SS_MAIN );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Reset the CDMA-available counter.
    */
    sdss_cdma_avail_update( SD_SS_MAIN, TRUE );

  } /* act == CONTINUE || MEAS_BACK */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_oh_info3 */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_oh_info2

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the CDMA overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff) and receives the last required overhead
       message on that channel.

    2. The SS-Client updates the overhead information of the acquired system.
       Note that this happen when the base station changes CONFIG_MSG_SEQ and
       the SS-Client receives the last required overhead message with the new
       CONFIG_MSG_SEQ.

  NOTE! This indication should not be called when the SS-Client is just
  passing through on the way to a re-hash.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Attempt acquiring a GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_oh_info2(

        sd_band_e_type            band,
            /* Serving system's band class.
            */

        sd_chan_type              chan,
            /* Serving system's CDMA channel.
            */

        word                      sid,
            /* Serving system's SID.
            */

        word                      nid,
            /* Serving system's NID.
            */

        boolean                   is_pending_gsrdm,
            /* Indicate whether a GSRDM is pending.
            */

        word                      mcc,
            /* Serving system's MCC.
            */
        byte                      imsi_11_12,
            /* Serving system's IMISI_11_12.
            */

        sys_srch_win_type         srch_win_n,
            /* Neighbor set window search size.
            */

        sys_base_lat_type         base_lat,
            /* Base station latitude.
            */

        sys_base_long_type        base_long,
            /* Base station latitude.
            */

        sys_base_id_type          base_id,
            /* Base station Id.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  return sd_ss_ind_cdma_opr_oh_info3( band,
                                      chan,
                                      sid,
                                      nid,
                                      is_pending_gsrdm,
                                      mcc,
                                      imsi_11_12,
                                      srch_win_n,
                                      base_lat,
                                      base_long,
                                      base_id,
                                      SD_SS_OVHD_STATUS_COMPLETE,
                                      rtrn_act_ptr );
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_oh_info

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the CDMA overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff) and receives the last required overhead
       message on that channel.

    2. The SS-Client updates the overhead information of the acquired system.
       Note that this happen when the base station changes CONFIG_MSG_SEQ and
       the SS-Client receives the last required overhead message with the new
       CONFIG_MSG_SEQ.

  NOTE! This indication should not be called when the SS-Client is just
  passing through on the way to a re-hash.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Attempt acquiring a GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_oh_info(

        sd_band_e_type            band,
            /* Serving system's band class.
            */

        sd_chan_type              chan,
            /* Serving system's CDMA channel.
            */

        word                      sid,
            /* Serving system's SID.
            */

        word                      nid,
            /* Serving system's NID.
            */

        boolean                   is_pending_gsrdm,
            /* Indicate whether a GSRDM is pending.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_ind_cdma_opr_oh_info2( band,
                                      chan,
                                      sid,
                                      nid,
                                      is_pending_gsrdm,
                                      SD_MCC_NONE,
                                      SD_IMSI_11_12_NONE,
                                      0, 0, 0, 0,
                                      rtrn_act_ptr );
}




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_rescan

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS gets a CDMA SPM
  with a RESCAN field that is set to 1.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GSM/WCDMA acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_rescan(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_RESCAN, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_rescan */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_reg_rej

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA registration is
  rejected.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_reg_rej(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_REG_REJ, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE  |
                    SD_SS_ACT_ACQ_CDMA  |
                    SD_SS_ACT_ACQ_AMPS  |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW    |
                    SD_SS_ACT_MEAS_DED  |
                    SD_SS_ACT_MEAS_BACK |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_reg_rej */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_sys_lost

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when an acquired CDMA
  system is lost.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_sys_lost(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return( sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_NONE,
                                        rtrn_act_ptr ) );
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_sys_lost2

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when an acquired CDMA
  system is lost and the system lost reason is provided.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_sys_lost2(

        sd_ss_sys_lost_e_type     sys_lost,
            /* System lost reason
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;
  sdss_state_e_type     ss_state          = sd_ss_ptr(SD_SS_MAIN)->ss_state;
  const prl_hdr_s_type  *prl_hdr_ptr      = prl_hdr_get_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ///* Update the bad points count.
  //*/
  //sd_ss_bad_point_update();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-State is acquisition and we have a PRL with MCC/IMSI_11_12 system
  ** records, process this as the following.
  **
  ** If the system lost is advise unlock rf, treat if as acq_failed_unlock_rf
  ** otherwise treat it as sync acquisition failure.
  */
  if( ss_state == SDSS_STATE_ACQ &&
      prl_hdr_ptr->is_mcc_sys_rec )
  {

    if( sys_lost == SD_SS_SYS_LOST_ADVISE_UNLOCK_RF )
    {
      SD_MSG_HIGH( "sys_lost => acq_failed_unlock_rf",0,0,0 );
      return sd_ss_ind_cdma_acq_failed_unlock_rf( rtrn_act_ptr );
    }


    SD_MSG_HIGH( "sys_lost => acq_failed",0,0,0 );
    return sd_ss_ind_cdma_acq_failed_sync( rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( sys_lost, SD_SS_SYS_LOST_MAX );

  SD_MSG_HIGH( "sys_lost=%d", sys_lost, 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the system lost reason.
  */
  sdss_sys_lost_update( SD_SS_MAIN, sys_lost );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SDSR_LOST_SYS as per the SDSR_SRV_SYS.
  */
  sdsr_list_cpy( SDSR_LOST_SYS, SDSR_SRV_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_SYS_LOST, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_sys_lost */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_redir_cdma

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the mobile receives a
  CDMA redirection message that redirects MS to CDMA systems.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACCEPT   -  Accept BS proposed redirection.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_redir_cdma(

        boolean                   is_rif,
            /* RETURN_IF_FAILED. Indicates whether the MS is required to
            ** return to the system from which it is being redirected upon
            ** failure to obtain service using the redirection criteria
            ** specified by this function call.
            */

        sd_ss_redir_e_type        redir_type,
            /* Redirection type - GSRDM, SRDM, NDSS.
            */

        sd_band_e_type            band_class,
            /* Band class. Indicate the CDMA band class, as specified in
            ** TSB58-A, for which the MS is being directed.
            */

        word                      expected_sid,
            /* Expected SID. If the MS is being redirected to a specific
            ** system, indicate the SID of that system; otherwise, should be
            ** set to SD_WILDCARD_SID.
            */

        word                      expected_nid,
            /* Expected NID. If the MS is being redirected to a specific
            ** network, indicate the NID of that network; otherwise, should
            ** be set to SD_WILDCARD_NID.
            */

        const word                *chans_ptr,
            /* Pointer to array of CDMA channels to which the MS is being
            ** redirected.
            */

        int                       num_chans,
            /* Number of CDMA channels in the array that is pointed to by
            ** chans_ptr.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_redir_rec_s_type       rec_list;
  int                       i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( redir_type == SD_SS_REDIR_NONE       ||
             redir_type == SD_SS_REDIR_CDMA_SRDM  ||
             redir_type == SD_SS_REDIR_CDMA_GSRDM ||
             redir_type == SD_SS_REDIR_CDMA_NDSS  );

  SD_ASSERT_ENUM_IS_INRANGE( band_class, SD_BAND_MAX );
  SD_ASSERT( INRANGE(num_chans, 0, SD_MAX_NUM_REDIR_CHAN) );
  SD_ASSERT( chans_ptr != NULL || num_chans == 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Need to confirm with ping
  ** Wrap the follwoing into a funtion.
  */
  rec_list.is_delete_tmsi = FALSE;

  rec_list.record_type = SD_REDIR_REC_CDMA;
  rec_list.rec.cdma_rec.band_class = band_class;
  rec_list.rec.cdma_rec.num_chans = num_chans;

  num_chans = MIN( num_chans, SD_MAX_NUM_REDIR_CHAN );

  for( i=0; i<num_chans; i++ )
  {
    SD_ASSERT( chans_ptr != NULL );
    rec_list.rec.cdma_rec.chan[i] = *chans_ptr;
    chans_ptr++;
  }

  rec_list.rec.cdma_rec.expected_sid = expected_sid;
  rec_list.rec.cdma_rec.expected_nid = expected_nid;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( sd_ss_ind_cdma_opr_redir( is_rif,
                                    redir_type,
                                    &rec_list,
                                    1,
                                    rtrn_act_ptr ));

} /* sd_ss_ind_cdma_opr_redir_cdma */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_redir_amps

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the mobile receives a
  CDMA redirection message that redirects MS to AMPS systems.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACCEPT   -  Accept BS proposed redirection.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_redir_amps(


        boolean                   is_rif,
            /* RETURN_IF_FAILED. Indicates whether the MS is required to
            ** return to the system from which it is being redirected upon
            ** failure to obtain service using the redirection criteria
            ** specified by this function call.
            */

        sd_ss_redir_e_type        redir_type,
            /* Redirection type - GSRDM, SRDM, NDSS.
            */

        word                      expected_sid,
            /* Expected SID. If the MS is being redirected to a specific
            ** system, indicate the SID of that system; otherwise, should be
            ** set to SD_WILDCARD_SID.
            */

        sd_ss_sys_ord_e_type      sys_ord,
            /* System ordering. Indicate the order in which the mobile
            ** station is to attempt to obtain service on an analog system.
            */

        word                      max_redirect_delay,
            /* Maximum delay upon redirection. Indicate the maximum delay
            ** time, in units of 8 second increments, to be used by MS in the
            ** event of a global redirection (GSRDM) to analog mode in order
            ** to avoid overloading an underlying analog cell's reverse
            ** control channel with MS registration.
            **
            ** A value of SD_SS_MAX_REDIR_DELAY_NONE indicates that this
            ** field should not be used to determine the AMPS registration
            ** time.
            */

        boolean                   is_ignore_cdma,
            /* IGNORE_CDMA indicator. Indicates whether the MS is to ignore
            ** the CDMA Capability Message on the analog system to which it
            ** is being redirected.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_redir_rec_s_type       rec_list;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( redir_type == SD_SS_REDIR_NONE       ||
             redir_type == SD_SS_REDIR_CDMA_SRDM  ||
             redir_type == SD_SS_REDIR_CDMA_GSRDM ||
             redir_type == SD_SS_REDIR_CDMA_NDSS  );

  SD_ASSERT_ENUM_IS_INRANGE( sys_ord, SD_SS_SYS_ORD_MAX );

  SD_ASSERT( max_redirect_delay == SD_SS_MAX_REDIR_DELAY_NONE ||
             max_redirect_delay <= 0xFF );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Need to confirm with ping
  ** Wrap the follwoing into a funtion.
  */
  rec_list.is_delete_tmsi = FALSE;

  rec_list.record_type = SD_REDIR_REC_AMPS;
  rec_list.rec.amps_rec.expected_sid = expected_sid;
  rec_list.rec.amps_rec.sys_ord = sys_ord;
  rec_list.rec.amps_rec.is_ignore_cdma = is_ignore_cdma;
  rec_list.rec.amps_rec.max_redirect_delay = max_redirect_delay;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( sd_ss_ind_cdma_opr_redir( is_rif,
                                    redir_type,
                                    &rec_list,
                                    1,
                                    rtrn_act_ptr ));

} /* sd_ss_ind_cdma_opr_redir_amps */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_get_redir_rec_amps_store

DESCRIPTION
  Step through the redirection message and store the redirection record that
  contains valide band/channl in the redirection list.
DEPENDENCIES

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_ss_redir_rec_amps_store(

        sdsr_e_type               sdsr_list,
            /* Pointed to the sdsr list to store the redirection record.
            */

        const sd_redir_rec_amps_s_type  *redir_rec_amps_ptr,
            /* Pointer to a analog redirection record to which the MS is
            ** being redirected.
            */

        boolean                   is_delete_tmsi
            /* Indicate if MS is required to delete the TMSI assigned.
            */

)
{
  sd_blksys_e_type    blksys;
  sdsr_ref_type       sr_ref;
  sd_cell_sys_e_type  first_sys       = SD_CELL_SYS_MAX;
  sd_cell_sys_e_type  second_sys      = SD_CELL_SYS_MAX;
  sd_cell_sys_e_type  redir_sys;
  sd_cell_sys_e_type  other_sys;
  /* Point at serving system.
  */
  sdsr_s_type         *srv_sys_ptr  = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );
  int                 avail_entries   = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( redir_rec_amps_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( sdsr_list, SDSR_MAX );
  if(srv_sys_ptr != NULL)
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Decide whether redirecting system is an A or B system.
    */
    blksys = sdprl_map_chan_to_blksys( srv_sys_ptr->sys.mode,
         sdss_map_band_pref_to_band(srv_sys_ptr->sys.band),
                                     srv_sys_ptr->sys.chan );

    if( blksys == SD_BLKSYS_CELL_B )
    {
      redir_sys = SD_CELL_SYS_B;
      other_sys = SD_CELL_SYS_A;
    }
    else
    {
      redir_sys = SD_CELL_SYS_A;
      other_sys = SD_CELL_SYS_B;
    }
  }
  else
  {
    SD_MSG_ERROR("srv_sys_ptr NULL",0,0,0);
    redir_sys = SD_CELL_SYS_A;
    other_sys = SD_CELL_SYS_B;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* According to the system ordering find out to which AMPS system(s) MS is
  ** being redirected to.
  */
  switch( redir_rec_amps_ptr->sys_ord )
  {
    case SD_SS_SYS_ORD_A_OR_B:

      /* Decide on the AMPS system according to the SID that is included with
      ** the redirection.
      */
      if( redir_rec_amps_ptr->expected_sid == SD_WILDCARD_SID )
      {
        if( sdss_is_band_pref(SD_SS_BAND_PREF_CELL_A) )
        {
          first_sys   = SD_CELL_SYS_A;
        }
        else if( sdss_is_band_pref(SD_SS_BAND_PREF_CELL_B) )
        {
          first_sys   = SD_CELL_SYS_B;
        }
        else
        {
          first_sys   = redir_sys;
          second_sys  = other_sys;
        }
      }
      else if( redir_rec_amps_ptr->expected_sid & 0x0001 )
      {
        first_sys   = SD_CELL_SYS_A;
      }
      else
      {
        first_sys   = SD_CELL_SYS_B;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_SYS_ORD_A_ONLY:
      first_sys   = SD_CELL_SYS_A;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_SYS_ORD_B_ONLY:
      first_sys   = SD_CELL_SYS_B;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_SYS_ORD_A_THEN_B:
      first_sys   = SD_CELL_SYS_A;
      second_sys  = SD_CELL_SYS_B;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_SYS_ORD_B_THEN_A:
      first_sys   = SD_CELL_SYS_B;
      second_sys  = SD_CELL_SYS_A;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_SYS_ORD_A_AND_B:

      /* Decide on the AMPS system according to the SID that is included with
      ** the redirection.
      */
      if( redir_rec_amps_ptr->expected_sid == SD_WILDCARD_SID )
      {
        if( sdss_is_band_pref(SD_SS_BAND_PREF_CELL_A) )
        {
          first_sys   = SD_CELL_SYS_A;
          second_sys  = SD_CELL_SYS_B;
        }
        else if( sdss_is_band_pref(SD_SS_BAND_PREF_CELL_B) )
        {
          first_sys   = SD_CELL_SYS_B;
          second_sys  = SD_CELL_SYS_A;
        }
        else
        {
          first_sys   = redir_sys;
          second_sys  = other_sys;
        }
      }
      else if( redir_rec_amps_ptr->expected_sid & 0x0001 )
      {
        first_sys   = SD_CELL_SYS_A;
        second_sys  = SD_CELL_SYS_B;
      }
      else
      {
        first_sys   = SD_CELL_SYS_B;
        second_sys  = SD_CELL_SYS_A;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_SYS_ORD_MAX:
    default:
      SD_MSG_ERROR( "sys_ord",0,0,0 );
      break;

  } /* switch */

  SD_ASSERT( first_sys != SD_CELL_SYS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out the number of system record table entries that are available
  ** for storing the redirection's channels.
  */
  avail_entries = sdsr_tbl_size() - sdsr_list_cnt(SDSR_PRL_LST);
  SD_ASSERT( avail_entries >= 2 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If MS supports AMPS mode, store the redirection's channels in the system
  ** record table and update SDSR_REDIR_LST to reference the stored records.
  ** Else, indicate that this redirection is invalid.
  */
  if( sdss_is_supp_mode_band(SD_MODE_AMPS,
                             sdss_map_band_to_band_pref(SD_BAND_CELL)) )
  {
    if( first_sys != SD_CELL_SYS_MAX &&
        avail_entries > 0 )
    {
      sr_ref = sdsr_tbl_store4( SD_MODE_AMPS,
                                SD_BAND_CELL,
                                (word) first_sys,
                                redir_rec_amps_ptr->expected_sid,
                                SD_WILDCARD_NID,
                                is_delete_tmsi );

      sdsr_list_insert_ref( SDSR_REDIR_LST, SDSR_POS_NULL, sr_ref, FALSE );
    }

    if( second_sys != SD_CELL_SYS_MAX &&
        avail_entries > 1 )
    {
      sr_ref = sdsr_tbl_store4( SD_MODE_AMPS,
                                SD_BAND_CELL,
                                (word) second_sys,
                                redir_rec_amps_ptr->expected_sid,
                                SD_WILDCARD_NID,
                                is_delete_tmsi );

      sdsr_list_insert_ref( sdsr_list, SDSR_POS_NULL, sr_ref, FALSE );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

}/* sd_ss_redir_rec_amps_store */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_get_redir_rec_cdma_store

DESCRIPTION
  Step through the redirection message and store the redirection record that
  contains valide band/channl in the redirection list.
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_ss_redir_rec_cdma_store(

        sdsr_e_type               sdsr_list,
            /* Pointed to the sdsr list to store the redirection record.
            */

        const sd_redir_rec_cdma_s_type  *redir_rec_cdma_ptr,
            /* Pointer to a analog redirection record to which the MS is
            ** being redirected.
            */
        boolean                   is_delete_tmsi
            /* Indicate if MS is required to delete the TMSI assigned.
            */

)
{
  int                            i;
  int                            avail_entries;
  int                            num_chans;
  sdsr_ref_type                  sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( redir_rec_cdma_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( sdsr_list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out the number of system record table entries that are available
  ** for storing the redirection's channels.
  */
  num_chans = redir_rec_cdma_ptr->num_chans;
  avail_entries = sdsr_tbl_size() - sdsr_list_cnt(SDSR_PRL_LST);
  SD_ASSERT( avail_entries >= num_chans );
  num_chans = MIN( num_chans, avail_entries );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store supported redirection's channels in the system record table and
  ** update SDSR_REDIR_LST to reference the stored records.
  */
  for( i=0; i < num_chans; i++ )
  {
    SD_ASSERT( &redir_rec_cdma_ptr->chan[i] != NULL );
    if( sdss_is_supp_sys(SD_MODE_CDMA,
                         sdss_map_band_to_band_pref(redir_rec_cdma_ptr->band_class),
                         redir_rec_cdma_ptr->chan[i]) )
    {
      sr_ref = sdsr_tbl_store4( SD_MODE_CDMA,
                                redir_rec_cdma_ptr->band_class,
                                redir_rec_cdma_ptr->chan[i],
                                redir_rec_cdma_ptr->expected_sid,
                                redir_rec_cdma_ptr->expected_nid,
                                is_delete_tmsi );

      sdsr_list_insert_ref( sdsr_list, SDSR_POS_NULL, sr_ref, FALSE );
    }
  }

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 return;

}/* sd_ss_redir_rec_cdma_store */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_get_redir_rec_store

DESCRIPTION
  Step through the redirection message and store the redirection record that
  contains valide band/channl in the redirection list.

DEPENDENCIES

RETURN VALUE
 None

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_ss_redir_rec_store(

        sdsr_e_type               sdsr_list,
            /* Pointed to the sdsr list to store the redirection record.
            */

        const sd_redir_rec_s_type *redir_rec_ptr,
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        int                       num_rec
            /* number of records in the redirection record list.
            */
)
{
  int                   i;
  sd_redir_rec_s_type   *rec_list_ptr = (sd_redir_rec_s_type *)redir_rec_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( redir_rec_ptr != NULL || num_rec == 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the redirection records and store them into the system
  ** record data base.
  */

  for( i=0; i<num_rec; i++, rec_list_ptr++ )
  {
    SD_ASSERT( rec_list_ptr != NULL );

    switch( rec_list_ptr->record_type )
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SD_REDIR_REC_AMPS:

         sd_ss_redir_rec_amps_store( sdsr_list,
                                     &rec_list_ptr->rec.amps_rec,
                                     rec_list_ptr->is_delete_tmsi);

      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SD_REDIR_REC_CDMA:


        sd_ss_redir_rec_cdma_store( sdsr_list,
                                    &rec_list_ptr->rec.cdma_rec,
                                    rec_list_ptr->is_delete_tmsi);

      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SD_REDIR_REC_NDSS_OFF:
      case SD_REDIR_REC_TACS:
      case SD_REDIR_REC_JTACS:
      case SD_REDIR_REC_DS41:
      case SD_REDIR_REC_MAX:
      default:
        SD_CASE_ERR("Bad red rec type %d",  rec_list_ptr->record_type, 0, 0);
        break;
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;


}/* sd_ss_redir_rec_store */



/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_get_redir_rec_get_num_chan

DESCRIPTION
  Calculate the sum of the channels in the redirection records

DEPENDENCIES

RETURN VALUE
 The sum of the channels.

SIDE EFFECTS
  None.

===========================================================================*/
static  int                       sd_ss_redir_rec_get_num_chan(

        const sd_redir_rec_s_type *redir_rec_ptr,
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        int                       num_rec
            /* number of records in the redirection record list.
            */
)
{
  int i;
  int num_chan = 0;
  sd_redir_rec_s_type   *rec_list_ptr = (sd_redir_rec_s_type *)redir_rec_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( redir_rec_ptr != NULL || num_rec == 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the redirection records and get the sum of the channels per
  ** redirection record.
  */

  for( i=0; i<num_rec; i++, rec_list_ptr++ )
  {
    SD_ASSERT( rec_list_ptr != NULL );

    switch( rec_list_ptr->record_type )
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SD_REDIR_REC_AMPS:

         /* For AMPS, treat it as one channel per record.
         */
         num_chan += 1;

      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SD_REDIR_REC_CDMA:

        num_chan += rec_list_ptr->rec.cdma_rec.num_chans;

      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SD_REDIR_REC_NDSS_OFF:
      case SD_REDIR_REC_TACS:
      case SD_REDIR_REC_JTACS:
      case SD_REDIR_REC_DS41:
      case SD_REDIR_REC_MAX:
      default:
        SD_CASE_ERR("Bad red rec type %d",  rec_list_ptr->record_type, 0, 0);
        break;
    }

  }

  return num_chan;
}





/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_redir

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the mobile receives a
  redirection message containing one or more redirection recoreds that
  redirects MS to either AMPS or CDMA systems.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACCEPT   -  Accept BS proposed redirection.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_redir(

        boolean                   is_rif,
            /* RETURN_IF_FAILED. Indicates whether the MS is required to
            ** return to the system from which it is being redirected upon
            ** failure to obtain service using the redirection criteria
            ** specified by this function call.
            */

        sd_ss_redir_e_type        redir_type,
            /* Redirection type - GSRDM, SRDM, NDSS.
            */

        const sd_redir_rec_s_type *rec_list_ptr,
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        int                       num_rec,
            /* number of records in the redirection record list.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  boolean             redir_is_valid = TRUE;
  int                 num_chan =0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( redir_type == SD_SS_REDIR_NONE       ||
             redir_type == SD_SS_REDIR_CDMA_SRDM  ||
             redir_type == SD_SS_REDIR_CDMA_GSRDM ||
             redir_type == SD_SS_REDIR_CDMA_NDSS  );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store supported redirection's bands and channels in the system record
  ** table and update SDSR_REDIR_LST to reference the stored records.
  */
  sdsr_list_clr( SDSR_REDIR_LST );

  sd_ss_redir_rec_store( SDSR_REDIR_LST,
                         rec_list_ptr,
                         num_rec);

  /* Get total number of channels in the redirection records.
  */
  num_chan = sd_ss_redir_rec_get_num_chan( rec_list_ptr, num_rec );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If none of the redirection channels is valid for this target & num_chan
  ** is zero, indicate that this is an invalid redirection.
  */
  if( num_chan > 0 &&
      sdsr_list_cnt(SDSR_REDIR_LST) == 0 )
  {
    redir_is_valid = FALSE;
  }

  SD_MSG_HIGH("CDMA redir recs=%d, is_valid=%d", num_chan, redir_is_valid,
               0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the redirection type.
  */
  sdss_redir_update2( SD_SS_MAIN,          /* MAIN system selection */
                      redir_type,          /* Redirection type */
                      redir_is_valid,      /* Valid redirection indicator */
                      is_rif,              /* RETURN_IF_FAIL indicator */
                      rec_list_ptr         /* Redirection list pointer */
                    );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update SDSR_REDIR_SYS per the redirecting system.
  */
  //sdsr_list_cpy( SDSR_REDIR_SYS, SDSR_SRV_SYS );
  sdsr_list_cpy( SDSR_REDIR_SYS, SDSR_ACQ_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_REDIR, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACCEPT |
                    SD_SS_ACT_REJECT) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;



}/* sd_ss_ind_cdma_opr_redir */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_acpt_rej

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the SS-Clients is
  done with accepting/rejecting a CDMA redirection.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_acpt_rej(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_ACPT_REJ, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE, update the service indicators, per the serving
  ** system. This is necessary because we do not update the service
  ** indicators in the receiving the sd_ss_ind_cdma_opr_oh_info() if there is
  ** a pending redirection.
  */
  if( act == SD_SS_ACT_CONTINUE )
  {
    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_OH,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_PS,
                  sdsr_list_get_ref(SDSR_SRV_SYS, 0),
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_SRV
                );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_acpt_rej */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_ndss_off

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS receives a
  CDMA NDSS off message.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK- Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_ndss_off(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_NDSS_OFF, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_ndss_off */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_new_sys

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when performing a CDMA
  idle-handoff to a BS with unknown configuration.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_new_sys(

        sd_band_e_type            band,
            /* The band class of the new system.
            */

        sd_chan_type              chan,
            /* The CDMA channel of the new system.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;
  sdsr_ref_type       new_sys_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store supported new system information in the system record table and
  ** set the SDSR_NEW_SYS to reference the stored record.
  */
  sdsr_list_clr( SDSR_NEW_SYS );
  if( sdss_is_supp_sys(SD_MODE_CDMA, sdss_map_band_to_band_pref(band), chan) )
  {
    new_sys_ref = sdsr_tbl_store2( SD_MODE_CDMA,
                                   sdss_map_band_to_band_pref(band),
                                   chan );

    sdsr_list_put_ref( SDSR_NEW_SYS, new_sys_ref );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_NEW_SYS, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_new_sys */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_sys_resel

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS detects that CDMA
  pilot power is below a specified threshold (switch to AMPS).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_sys_resel(

        const sd_sys_s_type       *sys_ptr,
            /* Pointer to array of systems from which to re-select.
            */

        int                       sys_num,
            /* Number of systems in the array that is pointed to by sys_ptr.
            */


        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;
  int                 i;
  sdsr_ref_type       sr_ref;
  int                 avail_entries;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sys_num >= 0 );
  SD_ASSERT( !(sys_ptr == NULL && sys_num != 0) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out the number of system record table entries that are available
  ** for storing the reselection systems.
  */
  avail_entries = sdsr_tbl_size() - sdsr_list_cnt(SDSR_PRL_LST);
  SD_ASSERT( avail_entries >= sys_num );
  sys_num = MIN( sys_num, avail_entries );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store supported system reselection's channels in the system record table
  ** and update SDSR_SYS_RESEL_LST to reference the stored records.
  */
  sdsr_list_clr( SDSR_SYS_RESEL_LST );
  for( i=0; i < sys_num; i++ )
  {
    SD_ASSERT( sys_ptr != NULL );
    if( sdss_is_supp_sys(sys_ptr->mode,
                         sys_ptr->band,
                         sys_ptr->chan) )
    {
      sr_ref = sdsr_tbl_store2( sys_ptr->mode,
                                sys_ptr->band,
                                sys_ptr->chan );

      sdsr_list_insert_ref( SDSR_SYS_RESEL_LST,
                            SDSR_POS_NULL,
                            sr_ref,
                            TRUE );
    }
    sys_ptr++;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_SYS_RESEL, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_sys_resel */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_lock

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  lock order.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_lock(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SDSR_LOCK_SYS as per the SDSR_SRV_SYS.
  */
  sdsr_list_cpy( SDSR_LOCK_SYS, SDSR_SRV_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the CDMA locked status.
  */
  sdss_cdma_lock_update( TRUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_LOCK, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_lock */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_unlock

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  unlock order.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_unlock(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the CDMA locked status.
  */
  sdss_cdma_lock_update( FALSE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_UNLOCK, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_unlock */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_acc_term

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA access attempt
  terminates due to access denied, system lost, access state timeout, max
  access probes, reorder, intercept, BS release, MS release or normal access
  completion.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_acc_term(

        sd_ss_acc_reas_e_type     acc_reas,
            /* Access termination reason.
            */

        sd_ss_acc_term_e_type     acc_term,
            /* Access termination reason.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( acc_reas, SD_SS_ACC_REAS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acc_term, SD_SS_ACC_TERM_MAX );

  SD_MSG_HIGH( "acc_term=%d", acc_term, 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the access and termination reasons.
  */
  sdss_acc_update( SD_SS_MAIN, acc_reas, acc_term );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If access is successful, update the service indicators due to implicit
  ** registration.
  */
  if( acc_term == SD_SS_ACC_TERM_NORMAL )
  {
    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_REG,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_PS,
                  sdsr_list_get_ref(SDSR_SRV_SYS, 0),
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_SRV
                );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_ACC_TERM, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_acc_term */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_cam_err

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  channel assignment that is not supported by the MS.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_cam_err(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */

)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_CAM_ERR, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_cam_err */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_cam_cdma

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  channel assignment to a CDMA paging/traffic channel.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with BS proposed channel assignment.

    SD_SS_ACT_REJECT   -  Reject BS proposed channel assignment.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_cam_cdma(

        sd_ss_ho_e_type           ho_type,
            /* The handoff type the MS needs to perform as part of this
            ** channel assignment:
            **
            **   SD_SS_HO_CDMA_CAM_TO_CDMA_PAG
            **   SD_SS_HO_CDMA_CAM_TO_CDMA_TRF
            */

        sd_band_e_type            band,
            /* Assigned CDMA band class.
            */

        sd_chan_type              chan,
            /* Assigned CDMA traffic channel number.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */

)
{
  sd_ss_act_e_type    act;
  sdsr_ref_type       ho_sys_ref;

  /* Point at serving system.
  */
  sdsr_s_type         *srv_sys_ptr  = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );
  if(srv_sys_ptr == NULL)
  {
    SD_ERR_FATAL("srv_sys_ptr NULL",0,0,0);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ho_type == SD_SS_HO_CDMA_CAM_TO_CDMA_PAG ||
             ho_type == SD_SS_HO_CDMA_CAM_TO_CDMA_TRF )
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );
  SD_ASSERT( sdss_is_supp_sys(SD_MODE_CDMA, sdss_map_band_to_band_pref(band), chan) );

  SD_ASSERT( srv_sys_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the handoff type.
  */
  sdss_ho_update( ho_type );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the handoff system per the channel assignment and the current
  ** serving system.
  */
  ho_sys_ref = sdsr_tbl_store6( SD_MODE_CDMA,        /* mode */
                                band,                /* band */
                                chan,                /* CDMA channel */
                                srv_sys_ptr->sid.is95.sid, /* SID */
                                srv_sys_ptr->sid.is95.nid, /* NID */
                                srv_sys_ptr->sid.is95.sid, /* Expected SID */
                                srv_sys_ptr->sid.is95.nid, /* Expected NID */
                                srv_sys_ptr->sid.is95.mcc, /* MCC */
                                srv_sys_ptr->sid.is95.imsi_11_12, /*IMSI_11*/
                                srv_sys_ptr->prl,    /* PRL designation */
                                srv_sys_ptr->roam,   /* Roam designation*/
                                srv_sys_ptr->prl_assn );/* Assn designation*/

  sdsr_list_put_ref( SDSR_HO_SYS, ho_sys_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_CAM, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_REJECT) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE and CAM is not to CDMA paging, do the
  ** following:
  */
  if( act == SD_SS_ACT_CONTINUE &&
      ! sdss_is_handoff(SDSS_HO_VAL_CDMA_CAM_TO_CDMA_PAG) )
  {
    /* Update serving system per the handoff system.
    */
    sdsr_list_cpy( SDSR_SRV_SYS, SDSR_HO_SYS );

    /* Update the service indicators per the new serving system.
    */
    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_REG,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_PS,
                  sdsr_list_get_ref(SDSR_SRV_SYS, 0),
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_SRV
                );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_cam_cdma */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_cam_amps

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  channel assignment to an AMPS system/voice channel.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with BS proposed channel assignment.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_REJECT   -  Reject BS proposed channel assignment.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_cam_amps(

        sd_ss_ho_e_type           ho_type,
            /* The handoff type the MS needs to perform as part of this
            ** channel assignment:
            **
            **   SD_SS_HO_CDMA_CAM_TO_AMPS_SYS
            **   SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE
            */

        sd_cell_sys_e_type        cell_sys,
            /* The assigned cellular system (A/B). Note that this parameter
            ** is ignored when the MS is being assigned an AMPS voice
            ** channel.
            */

        word                      voice_chan,
            /* The assigned AMPS voice channel number. Note that this
            ** parameter is ignored when the MS is being assigned an AMPS
            ** system.
            */

        word                      sid,
            /* SID from channel assignment. If no SID is supplied with
            ** channel assignment, must be set to SD_WILDCARD_SID.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;
  sdsr_ref_type       ho_sys_ref;

  /* Point at serving system.
  */
  sdsr_s_type         *srv_sys_ptr  = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );
  if(srv_sys_ptr == NULL)
  {
    SD_ERR_FATAL("srv_sys_ptr should not be NULL",0,0,0);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ho_type == SD_SS_HO_CDMA_CAM_TO_AMPS_SYS ||
             ho_type == SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE );
  SD_ASSERT( sdss_is_supp_mode_band(SD_MODE_AMPS, sdss_map_band_to_band_pref(SD_BAND_CELL)) );

  SD_ASSERT( srv_sys_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the handoff type.
  */
  sdss_ho_update( ho_type );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If MS is assigned to AMPS voice channel, get the cellular system that
  ** corresponds to this voice channel.
  */
  if( ho_type == SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE )
  {
    cell_sys = sdprl_map_amps_vc_to_sys(voice_chan);
  }

  SD_ASSERT_ENUM_IS_INRANGE( cell_sys, SD_CELL_SYS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the handoff system per the channel assignment and the current
  ** serving system.
  */
  ho_sys_ref = sdsr_tbl_store6( SD_MODE_AMPS,        /* mode */
                                SD_BAND_CELL,        /* band */
                 (sd_chan_type) cell_sys,            /* AMPS system */
                                sid,                 /* SID */
                                SD_WILDCARD_NID,     /* NID */
                                sid,                 /* Expected SID */
                                SD_WILDCARD_NID,     /* Expected NID */
                                srv_sys_ptr->sid.is95.mcc, /* MCC */
                                srv_sys_ptr->sid.is95.imsi_11_12, /*IMSI_11*/
                                srv_sys_ptr->prl,    /* PRL designation */
                                srv_sys_ptr->roam,   /* Roam designation*/
                                srv_sys_ptr->prl_assn );/* Assn designation*/

  sdsr_list_put_ref( SDSR_HO_SYS, ho_sys_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the PRL designation, GEO list and more preferred list of the
  ** handoff system.
  */
  (void) sdprl_get_info(
                  SD_SS_MAIN,          /* MAIN system selection */
                  SDSR_HO_SYS,         /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_HO_GEO_LST,     /* Put GEO systems into this list */
                  SDSR_HO_PREF_LST,    /* Put PREF systems into this list */
                  SDSR_MAX,            /* Do not extract collocated sys */
                  SDPRL_EXTRACT_ALL ); /* Get info regardless of PRL match */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_CAM, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_REJECT) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE or ACQ_AMPS, do the following:
  */
  if( act == SD_SS_ACT_CONTINUE ||
      act == SD_SS_ACT_ACQ_AMPS )
  {
    /* Update the serving system per the handoff system.
    */
    sd_ss_srv_sys_update( SD_SS_MAIN,
                          SDSR_HO_SYS,
                          SDSR_HO_GEO_LST,
                          SDSR_HO_PREF_LST,
                          SDSR_MAX );

    /* Update the service indicators per the new serving system.
    */
    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_REG,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_PS,
                  sdsr_list_get_ref(SDSR_SRV_SYS, 0),
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_SRV
                );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_cam_amps */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_hdm_cdma

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  handoff direction to a CDMA traffic channel.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with BS proposed handoff direction.

    SD_SS_ACT_REJECT   -  Reject BS proposed handoff direction.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_hdm_cdma(

        sd_band_e_type            band,
            /* The assigned CDMA band class.
            */

        word                      chan,
            /* The Assigned CDMA traffic channel number.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */

)
{
  sd_ss_act_e_type    act;
  sdsr_ref_type       ho_sys_ref;

  /* Point at serving system.
  */
  sdsr_s_type         *srv_sys_ptr  = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );
  SD_ASSERT( sdss_is_supp_sys(SD_MODE_CDMA, sdss_map_band_to_band_pref(band), chan) );

  if(srv_sys_ptr == NULL)
  {
    SD_ERR_FATAL("srv_sys_ptr NULL",0,0,0);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the handoff type.
  */
  sdss_ho_update( SD_SS_HO_CDMA_HDM_TO_CDMA_TRF );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the handoff system per the handoff direction and the current
  ** serving system.
  */
  ho_sys_ref = sdsr_tbl_store6(
                           SD_MODE_CDMA,              /* Mode */
                           band,                      /* Band */
                           chan,                      /* CDMA channel */
                           srv_sys_ptr->sid.is95.sid, /* SID */
                           srv_sys_ptr->sid.is95.nid, /* NID */
                           srv_sys_ptr->sid_expected.prm.is95.sid, /* Expected SID */
                           srv_sys_ptr->sid_expected.prm.is95.nid, /* Expected NID */
                           srv_sys_ptr->sid.is95.mcc, /* MCC */
                           srv_sys_ptr->sid.is95.imsi_11_12, /* IMSI_11_12 */
                           srv_sys_ptr->prl,          /* PRL designation */
                           srv_sys_ptr->roam,         /* Roam designation*/
                           srv_sys_ptr->prl_assn );   /* Assn designation */

  sdsr_list_put_ref( SDSR_HO_SYS, ho_sys_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_HDM, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_REJECT) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE, update the serving system per the handoff
  ** system and the system indicators per the new serving system.
  */
  if( act == SD_SS_ACT_CONTINUE )
  {
    sdsr_list_cpy( SDSR_SRV_SYS, SDSR_HO_SYS );

    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_REG,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_PS,
                  sdsr_list_get_ref(SDSR_SRV_SYS, 0),
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_SRV
                );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_hdm_cdma */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_hdm_amps

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  handoff direction to an AMPS voice channel.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with BS proposed handoff direction.

    SD_SS_ACT_REJECT   -  Reject BS proposed handoff direction.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_hdm_amps(

        word                      voice_chan,
            /* The assigned AMPS voice channel number.
            */

        word                      sid,
            /* SID from handoff direction. If no SID is supplied with
            ** handoff direction message, must be set to SD_WILDCARD_SID.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;
  sdsr_ref_type       ho_sys_ref;
  sd_cell_sys_e_type  cell_sys;

  /* Point at serving system.
  */
  sdsr_s_type         *srv_sys_ptr  = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sdss_is_supp_mode_band(SD_MODE_AMPS, sdss_map_band_to_band_pref(SD_BAND_CELL)) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the handoff type.
  */
  sdss_ho_update( SD_SS_HO_CDMA_HDM_TO_AMPS_VOICE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the cellular system that corresponds to AMPS voice channel.
  */
  cell_sys = sdprl_map_amps_vc_to_sys(voice_chan);

  SD_ASSERT_ENUM_IS_INRANGE( cell_sys, SD_CELL_SYS_MAX );

  if(srv_sys_ptr == NULL)
  {
    SD_ERR_FATAL("srv_sys_ptr NULL",0,0,0);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the handoff system per the handoff direction and the current
  ** serving system.
  */
  ho_sys_ref = sdsr_tbl_store6(
                           SD_MODE_AMPS,              /* Mode */
                           SD_BAND_CELL,              /* Band */
            (sd_chan_type) cell_sys,                  /* cellular system */
                           sid,                       /* SID */
                           SD_WILDCARD_NID,           /* NID */
                           sid,                       /* Expected SID */
                           SD_WILDCARD_NID,           /* Expected NID */
                           srv_sys_ptr->sid.is95.mcc, /* MCC */
                           srv_sys_ptr->sid.is95.imsi_11_12, /* IMSI_11_12 */
                           srv_sys_ptr->prl,          /* PRL designation */
                           srv_sys_ptr->roam,         /* Roam designation*/
                           srv_sys_ptr->prl_assn );   /* Assn designation */

  sdsr_list_put_ref( SDSR_HO_SYS, ho_sys_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the PRL designation, GEO list and more preferred list of the
  ** handoff system.
  */
  (void) sdprl_get_info(
                  SD_SS_MAIN,          /* MAIN system selection */
                  SDSR_HO_SYS,         /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_HO_GEO_LST,     /* Put GEO systems into this list */
                  SDSR_HO_PREF_LST,    /* Put PREF systems into this list */
                  SDSR_MAX,            /* Do not extract collocated sys */
                  SDPRL_EXTRACT_ALL ); /* Get info regardless of PRL match */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_HDM, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_REJECT) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE, do the following:
  */
  if( act == SD_SS_ACT_CONTINUE )
  {
    /* Update the serving system per the handoff system.
    */
    sd_ss_srv_sys_update( SD_SS_MAIN,
                          SDSR_HO_SYS,
                          SDSR_HO_GEO_LST,
                          SDSR_HO_PREF_LST,
                          SDSR_MAX );

    /* Update the service indicators per the new serving system.
    */
    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_REG,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_PS,
                  sdsr_list_get_ref(SDSR_SRV_SYS, 0),
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_SRV
                );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_hdm_amps */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_itspm

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA In
  Traffic System Parameters Message or Mobile Station Registered Message.
  with new SID/NID information.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_RELEASE  -  Release call.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_itspm(

        word                      sid,
            /* SID from ITSPM.
            */

        word                      nid,
            /* NID from ITSPM.
            */
        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */

)
{
  sd_ss_act_e_type    act;
  sdsr_s_type         *srv_sys_ptr  = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );
  sdsr_ref_type       srv_sys_ref   = sdsr_list_get_ref( SDSR_SRV_SYS, 0 );
  sdsr_s_type         *acq_sys_ptr  = sdsr_list_get_ptr( SDSR_ACQ_SYS, 0 );

  SYS_ARG_NOT_USED( acq_sys_ptr );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( srv_sys_ref != SDSR_REF_NULL );
  SD_ASSERT( acq_sys_ptr != NULL );

  if(srv_sys_ptr == NULL)
  {
    SD_ERR_FATAL("srv_sys_ptr NULL",0,0,0);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SID/NID did not change, return CONTINUE.
  */
  if( srv_sys_ptr->sid.is95.sid == sid &&
      srv_sys_ptr->sid.is95.nid == nid )
  {
    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( rtrn_act_ptr != NULL )
    {
      rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID/NID of the current serving system per the ITSPM.
  */
  srv_sys_ptr->sid.is95.sid = sid;
  srv_sys_ptr->sid.is95.nid = nid;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the PRL designation, GEO list and more preferred list of the new
  ** serving system.
  */
  (void) sdprl_get_info(
                  SD_SS_MAIN,          /* MAIN system selection */
                  SDSR_SRV_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_SRV_GEO_LST,    /* Put GEO systems into this list */
                  SDSR_SRV_PREF_LST,   /* Put PREF systems into this list */
                  SDSR_SRV_COLLOC_LST, /* Put collocated sys in this list */
                  SDPRL_EXTRACT_ALL ); /* Get info regardless of PRL match */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_ITSPM, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_RELEASE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE (i.e., we have selected a new serving system),
  ** update the expected SID/NID of the serving system per the acquired
  ** SID/NID and the system indicators per the new serving system.
  */
  if( act == SD_SS_ACT_CONTINUE )
  {
    #ifdef FEATURE_JCDMA

    /* Set the expected SID of the new serving system to wildcard if the
    ** expected SID of the acq system is wildcard and redirection is on.
    */
    if( sdss_is_redir( SD_SS_MAIN, SDSS_REDIR_VAL_CDMA_ANY )
        && ( acq_sys_ptr->sid_expected.prm.is95.sid == PRL_WILDCARD_SID ))
    {
      srv_sys_ptr->sid_expected.prm.is95.sid = PRL_WILDCARD_SID;
    }
    else

    #endif /* FEATURE_JCDMA */
    {
      srv_sys_ptr->sid_expected.prm.is95.sid = sid;
      srv_sys_ptr->sid_expected.prm.is95.nid = nid;
    }

    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_REG,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_PS,
                  srv_sys_ref,
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_SRV_COLLOC_LST,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_SRV
                );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_itspm */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_release

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA call is
  released.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_release(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */

)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_RELEASE, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_release */
#endif  /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */




#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */




#ifdef FEATURE_HDR
#error code not present
#endif

/*===========================================================================

FUNCTION sd_ss_ind_hybr_1_user_ss_pref3

DESCRIPTION
  Instructs the HDR SS-Client of the SS-Action to take when the user changes
  the System Selection Preference (SS-Preference) and/or origination mode.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_hybr_1_user_ss_pref3(

        sd_ss_pref_reas_e_type    pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type    orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type    band_pref,
            /* New band preference (BC0, BC1, any, etc.)
            */

        sd_ss_prl_pref_e_type     prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type    roam_pref,
            /* New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type    hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type            otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type          otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        const sys_manual_sys_info_s_type
                                  *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
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
           /* Technology preference settings.
           */

        sys_wlan_scan_pref_e_type      wlan_scan_pref,
           /* WLAN scan preference.
           */

        sys_wlan_bss_e_type            bss_type_pref,
           /* BSS type.
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_TECH_IND_SD_STACK_ARCH
  /* If stack indenpendent is not defined, call old API's
  */
  #ifdef FEATURE_HDR
#error code not present
  #else
  return (sd_ss_ind_user_ss_pref3( pref_reason,
                                     orig_mode,
                                     mode_pref,
                                     band_pref,
                                     prl_pref,
                                     roam_pref,
                                     hybr_pref,
                                     otasp_band,
                                     otasp_blksys,
                                     SD_SS_AVOID_SYS_IDLE_IF_UZ, /* Don't care */
                                     0,                          /* Don't care */
                                     manual_sys_info_ptr,
                                     domain_pref,
                                     acq_order_pref,
                                     tech_pref,
                                     wlan_scan_pref,
                                     bss_type_pref,
                                     rtrn_act_ptr
                                   ));
  #endif

  #else
  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr         = sd_ss_ptr( SD_SS_HYBR_1 );
  sd_si_s_type        *si_ptr         = sd_si_ptr( SD_SS_HYBR_1 );
  sd_ss_act_s_type    prev_act        = ss_ptr->act;
  sdss_evt_e_type     event           = SDSS_EVT_USER_SS_PREF;
  sdss_pref_updated_e_type is_pref_change;
  dword               avoid_time      = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( pref_reason, SD_SS_PREF_REAS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( orig_mode, SD_SS_ORIG_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( roam_pref, SD_SS_ROAM_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( hybr_pref, SD_SS_HYBR_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( tech_pref, SYS_TECH_PREF_MASK_ANY );
  SD_ASSERT_ENUM_IS_INRANGE( wlan_scan_pref, SYS_WLAN_SCAN_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( bss_type_pref, SYS_WLAN_BSS_TYPE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( otasp_band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( otasp_blksys, SD_BLKSYS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If hybrid pref is none, process this as a regular SS-Pref change.
  */
  if( hybr_pref == SD_SS_HYBR_PREF_NONE &&
      sdss_is_hybr_pref(SD_SS_HYBR_PREF_NONE) )
  {
    return  sd_ss_ind_user_ss_pref3( pref_reason,
                                     orig_mode,
                                     mode_pref,
                                     band_pref,
                                     prl_pref,
                                     roam_pref,
                                     hybr_pref,
                                     otasp_band,
                                     otasp_blksys,
                                     SD_SS_AVOID_SYS_IDLE_IF_UZ, /* Don't care */
                                     0,                          /* Don't care */
                                     manual_sys_info_ptr,
                                     domain_pref,
                                     acq_order_pref,
                                     tech_pref,
                                     wlan_scan_pref,
                                     bss_type_pref,
                                     rtrn_act_ptr
                                   );
  };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  SD_MSG_HIGH( "pref_reason=%d, orig_mode=%d, mode_pref=0x%x",
                pref_reason, orig_mode, mode_pref );
  SD_MSG_HIGH( "band_pref=0x%llx, roam_pref=0x%x, hybr_pref=%d",
                band_pref, roam_pref, hybr_pref );
  SD_MSG_HIGH( "domain_pref=0x%x, acq_order_pref=0x%x prl_pref 0x%x",
                domain_pref, acq_order_pref, prl_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If switching from non-hybrid to hybrid operation:
  **
  ** 1. set the event to be processed to SDSS_EVT_HDR_USER_HYBR_START.
  ** 2. Copy SDSR_IDL_GEO_LST into SDSR_HDR_IDL_GEO_LST
  ** 3. Call on the system record to switch the SDSR_HDR_XXX lists to
  **    reference the appropriate lists in according to the new hybrid
  **    preference.
  */
  if( ss_ptr->is_start_instance )
  {
    SD_MSG_HIGH("Start Hybr_1", 0, 0, 0);
    event = SDSS_EVT_HYBR_1_USER_HYBR_START;
    sdsr_list_cpy( SDSR_HYBR_1_IDL_GEO_LST, SDSR_IDL_GEO_LST );
    sdsr_lists_hybr_pref_switch( hybr_pref );
    ss_ptr->is_start_instance = FALSE;
  }

  /* Else if switching from hybrid to non-hybrid operation:
  ** 1. Set the event to be processed to SDSS_EVT_HDR_USER_HYBR_END.
  ** 2. Call on the system record to switch the SDSR_HDR_XXX lists to
  **    reference the appropriate lists in according to the new hybrid
  **    preference.
  */
  else if( !sdss_is_hybr_pref(SD_SS_HYBR_PREF_NONE) &&
           hybr_pref == SD_SS_HYBR_PREF_NONE )
  {
    SD_MSG_HIGH("Stop Hybrid Hybr_1", 0, 0, 0);
    event = SDSS_EVT_HYBR_1_USER_HYBR_END;
    sdsr_lists_hybr_pref_switch( hybr_pref );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to update its SS-Preference setting.
  */
  is_pref_change = sdss_hybr_1_pref_update( pref_reason,
                                         orig_mode,
                                         mode_pref,
                                         band_pref,
                                         prl_pref,
                                         roam_pref,
                                         hybr_pref,
                                         domain_pref,
                                         acq_order_pref,
                                         tech_pref,
                                         wlan_scan_pref,
                                         bss_type_pref
                                       );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if prl preference changed.
  */
  if ( (is_pref_change == SDSS_PREF_UPDATED_TRUE_PREF   ||
        is_pref_change == SDSS_PREF_UPDATED_SRV_DOMAIN) &&
       sdss_is_prl_pref_chgd( SD_SS_HYBR_1 )
     )
  {
    /* Update the PRL_AVAIL status on the appropriate SYS.
    */
    if( ss_ptr->ss_state == SDSS_STATE_OPR )
    {
      sdss_update_avail_sys_status( SD_SS_HYBR_1, SDSR_HYBR_1_SRV_SYS );
    }
    else
    {
      sdss_update_avail_sys_status( SD_SS_HYBR_1, SDSR_HYBR_1_ACQ_SYS );
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Preferences is a optimized ss pref change. just return CONTINUE.
  */
  if( sd_is_opti_ss_pref_change( SD_SS_HYBR_1,
                                 is_pref_change,
                                 pref_reason,
                                 orig_mode)
    )
  {
    SD_MSG_HIGH( "SS-Pref didn't change - CONTINUE",0,0,0 );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SS-State is operation, inform the registered client (i.e., CM) that
    ** it is OK to originate as necessary.
    */
    if( ss_ptr->ss_state == SDSS_STATE_OPR )
    {
      sd_si_ok_to_orig( SD_SS_HYBR_1, si_ptr->data_suspend );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( rtrn_act_ptr != NULL )
    {
      rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( pref_reason != SD_SS_PREF_REAS_AVOID_SYS &&
           orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG &&
           sdsr_list_cnt(SDSR_ALL_PREF_LST) == 0 )
  {
    sdprl_all_pref_sys_app( SDSR_ALL_PREF_LST );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else if ( pref_reason == SD_SS_PREF_REAS_USER_RESEL &&
            sdss_is_orig_mode0( orig_mode, SD_SS_ORIG_MODE_VAL_MANUAL) )
  {

    sd_update_manual_sys( SDSR_HYBR_1_MANUAL_LST, manual_sys_info_ptr );

  } /* else if ( pref_reas == ... ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else if ( pref_reason == SD_SS_PREF_REAS_AVOID_SYS)
  {
    #ifdef FEATURE_USERZONE
    if (avoid_type == SD_SS_AVOID_SYS_IDLE_IF_UZ)
    {
      /* Check if the MS has selected a user zone currently
      */
      if (si_ptr->si_info.curr_uz_info.uz_id == SD_INVALID_UZID)
       {
         /* Currently not in user zone, so return continue.
         */
         return SD_SS_ACT_CONTINUE;
       }
     }
     #endif /* FEATURE_USERZONE */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Avoid SID/WILDCARD_NID
    */
    if ( !sdprl_add_lock_sid_nid( sdsr_list_get_ptr(SDSR_HYBR_1_IDL_SYS, 0 ), TRUE ))
    {
      SD_MSG_HIGH("SID/NID not locked, table full", 0, 0, 0 );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Avoid band/chan for 60 seconds.
    */
    avoid_time = (dword)60;
    sdss_sr_list_avoid2( SD_SS_HYBR_1, SDSR_HYBR_1_IDL_SYS, avoid_time );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If we got here, at least one of the SS-Preferences was changed or
  ** the SS-State is operation and origination mode is other than none.
  */

  act = sd_ss_event_proc( SD_SS_HYBR_1, event, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If action is equal to previous returned action, just continue - no need
  ** to instruct the SS-Client of the same action twice.
  */
  if( sd_ss_act_is_equal(&ss_ptr->act, &prev_act, ss_ptr->meas_list_is_diff))
  {
    SD_MSG_LOW( "Same action - CONTINUE",0,0,0 );

    /* If SS-Action is MEAS_XXX, set the measurement ID per the pervious
    ** SS-Action's measurement ID.
    */
    if( ss_ptr->act.act == SD_SS_ACT_MEAS_BACK ||
        ss_ptr->act.act == SD_SS_ACT_MEAS_DED )
    {
      ss_ptr->meas_id = prev_act.prm.meas.id;
    }

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( rtrn_act_ptr != NULL )
    {
      rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    act = SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-State is operation and SS-Action is CONTINUE or background
  ** measurement, inform the registered client (i.e., CM) that it is OK to
  ** originate as necessary.
  */
  if( (ss_ptr->ss_state == SDSS_STATE_OPR) &&
      (act == SD_SS_ACT_CONTINUE || act == SD_SS_ACT_MEAS_BACK) )
  {
    sd_si_ok_to_orig( SD_SS_HYBR_1, si_ptr->data_suspend );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

  #endif
} /* sd_ss_ind_hybr_1_user_ss_pref3 */


#ifdef FEATURE_HDR
#error code not present
#endif

#if (defined(FEATURE_HDR) || defined(FEATURE_TECH_IND_SD_STACK_ARCH))
/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_user_pwr_save_exit

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to exit power save mode of operation for HYBR HDR instance.

  Note that the actual SD behavior when getting this indication from the
  SS-Client is controlled by the SS-Script for this indication. This script,
  however, is expected to be fairly similar to the power-up script.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_HDR -  Acquire a HDR system as specified by the returned
                         "HDR acquisition" parameters.


    SD_SS_ACT_PWR_SAVE -  Go offline. SD instructs SS-Client to go right back
                          into power-save if the current SS-Preference
                          forbids any PRL system from being attempted
                          acquisition.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_hybr_1_user_pwr_save_exit(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_TECH_IND_SD_STACK_ARCH
  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_hdr_event_proc( SDSS_EVT_HDR_USER_PWR_SAVE_EXIT, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_HDR |
                    SD_SS_ACT_PWR_SAVE ) );
  #else

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_HYBR_1, SDSS_EVT_HYBR_1_USER_PWR_SAVE_EXIT, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #endif

  return act;

} /* sd_ss_ind_hdr_user_pwr_save_exit() */
#endif
/*===========================================================================
=============================================================================
============================ HDR HELPER FUNCTIONS ===========================
=============================================================================
===========================================================================*/
#ifdef FEATURE_HDR
#error code not present
#endif // FEATURE_HDR
/* <EJECT> */
/*===========================================================================
=============================================================================
============================ HDR MISC INDICATIONS ===========================
=============================================================================
===========================================================================*/

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*===========================================================================

FUNCTION sd_ss_ind_hybr_1_misc_timer

DESCRIPTION
  Instructs the HDR client of the SS-Action to take when the system selection
  timer expires.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_hybr_1_misc_timer(

        boolean                   resel_is_allowed,
            /* Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  #if (!defined (FEATURE_TECH_IND_SD_STACK_ARCH) && !defined(FEATURE_HDR_HYBRID))
  return sd_ss_ind_misc_timer( resel_is_allowed, rtrn_act_ptr );
  #else

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr = sd_ss_ptr(SD_SS_HYBR_1);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If currently not doing CDMA+HDR hybrid operation, return CONTINUE.
  */
  if( sdss_is_hybr_pref( SD_SS_HYBR_PREF_NONE ) )
  {
    if ( rtrn_act_ptr != NULL )
    {
      rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }
    return SD_SS_ACT_CONTINUE;
  }

  /* Indicate that the SS-Timer is expired by setting it to 0.
  */
  ss_ptr->ss_timer = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Sanity check
  */
  if(( sdss_is_hybr_pref( SD_SS_HYBR_PREF_CDMA_HDR ) ||
       sdss_is_hybr_pref( SD_SS_HYBR_PREF_CDMA__HDR_WCDMA ) )  &&
      ( SD_IS_BIT_CONTAIN( ss_ptr->hdr_act_mask, SD_SS_HDR_ACT_MASK_BCMCS )
                            ||
        SD_IS_BIT_CONTAIN( ss_ptr->hdr_act_mask, SD_SS_HDR_ACT_MASK_TRAFFIC ))
                            &&
      resel_is_allowed )
  {
    SD_MSG_ERROR( "No Resel in HDR act!!! resel %d, act 0x%x",
                  resel_is_allowed, ss_ptr->hdr_act_mask, 0 );

  }

  /* Process the SS-Event that is associated with this SS-Indication
  ** (depending whether reselection is allowed).
  */

  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
  if( resel_is_allowed )
  {
    act = sd_ss_event_proc( SD_SS_HYBR_1, SDSS_EVT_MISC_TIMER_RESEL_OK, rtrn_act_ptr );
  }
  else
  {
    act = sd_ss_event_proc( SD_SS_HYBR_1, SDSS_EVT_MISC_TIMER_RESEL_NO, rtrn_act_ptr );
  }
  #else
  /* Process the SS-Event that is associated with this SS-Indication
  ** (depending whether reselection is allowed).
  */
  if( resel_is_allowed )
  {
    act = sd_ss_hdr_event_proc( SDSS_EVT_MISC_TIMER_RESEL_OK, rtrn_act_ptr );
  }
  else
  {
    act = sd_ss_hdr_event_proc( SDSS_EVT_MISC_TIMER_RESEL_NO, rtrn_act_ptr );
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  if( resel_is_allowed )
  {
    SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                      SD_SS_ACT_ACQ_CDMA |
                      SD_SS_ACT_ACQ_AMPS |
                      SD_SS_ACT_ACQ_HDR  |
                      SD_SS_ACT_ACQ_GW   |
                      SD_SS_ACT_MEAS_DED |
                      SD_SS_ACT_MEAS_BACK|
                      SD_SS_ACT_PWR_SAVE ) );
  }
  else
  {
    SD_ASSERT( act == SD_SS_ACT_CONTINUE );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
  #endif

} /* sd_ss_ind_hybr_1_misc_timer */


#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */




/* <EJECT> */
/*===========================================================================
=============================================================================
======================== MISCELLANEOUS SS-INDICATIONS =======================
=============================================================================
===========================================================================*/

/* Indications that are not USER, CDMA or AMPS related. These indications are
** usually mode independent; or in other words, they can be received in more
** than one of the CP modes (e.g. CDMA, AMPS or PWR_SAVE).
*/



/*===========================================================================

FUNCTION sd_ss_ind_misc_timer

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the system selection
  timer expires.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_misc_timer(

        boolean                   resel_is_allowed,
            /* Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr = sd_ss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate that the SS-Timer is expired by setting it to 0.
  */
  ss_ptr->ss_timer = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication
  ** (depending whether reselection is allowed).
  */
  if( resel_is_allowed )
  {
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_TIMER_RESEL_OK, rtrn_act_ptr );
  }
  else
  {
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_TIMER_RESEL_NO, rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  if( resel_is_allowed )
  {
    SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                      SD_SS_ACT_ACQ_CDMA |
                      SD_SS_ACT_ACQ_AMPS |
                      SD_SS_ACT_ACQ_HDR  |
                      SD_SS_ACT_ACQ_GW   |
                      SD_SS_ACT_MEAS_DED |
                      SD_SS_ACT_MEAS_BACK|
                      SD_SS_ACT_PWR_SAVE ) );
  }
  else
  {
    SD_ASSERT( act == SD_SS_ACT_CONTINUE );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_misc_timer */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_misc_meas_rprt

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the channel
  strength measurement that was requested by SD is completed and reported
  back to SD.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_misc_meas_rprt(

        word                      meas_id,
            /* The measurement ID. This IS should match the ID that was
            ** provided to the SS-Client with the measurement request (i.e.,
            ** SD_SS_ACT_MEAS_BACK or SD_SS_ACT_MEAS_DED) SS-Action.
            */

        boolean                   resel_is_allowed,
            /* Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr = sd_ss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If measurement ID does not match the ID of the measurement request,
  ** instruct SS-Client to CONTINUE.
  */
  if( meas_id != ss_ptr->meas_id )
  {
    SD_MSG_HIGH( "meas_id=%d mismatch - req_id=%d",
                 meas_id,ss_ptr->meas_id,0 );

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( rtrn_act_ptr != NULL )
    {
      rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the measurement ID matches the ID of the measurement
  ** request.
  */

  /* Indicate that measurement is done being serviced.
  */
  ss_ptr->meas_is_done = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If reselection is not allowed, instruct SS-Client to CONTINUE.
  */
  if( ! resel_is_allowed )
  {
    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( rtrn_act_ptr != NULL )
    {
      rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the measurement ID matches the ID of the measurement
  ** request and reselection is allowed, so process the SS-Event that is
  ** associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_MEAS_RPRT, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_misc_meas_rprt */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_misc_orig_success

DESCRIPTION
  Inform SD of origination success.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_misc_orig_success(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_MAX;
  sd_si_s_type       *si_ptr = sd_si_ptr( SD_SS_MAIN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(act);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the origination success indication.
  */
  sdss_orig_success_update();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_ORIG_SUCCESS, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act == SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the registered client (i.e., CM) that it is OK to originate as
  ** necessary.
  */
  sd_si_ok_to_orig( SD_SS_MAIN, si_ptr->data_suspend );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SD_SS_ACT_CONTINUE;

} /* sd_ss_ind_misc_orig_success */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_misc_pwr_save_enter

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS enters power
  save mode of operation.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_misc_pwr_save_enter(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_PWR_SAVE_ENTER, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_misc_pwr_save_enter */

/*===========================================================================

FUNCTION sd_ss_ind_hybr_1_misc_pwr_save_enter

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS enters power
  save mode of operation.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_hybr_1_misc_pwr_save_enter(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if ( defined(FEATURE_TECH_IND_SD_STACK_ARCH) || defined(FEATURE_HDR_HYBRID))
  sd_ss_act_e_type    act;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If currently not doing CDMA+HDR hybrid operation, call on
  ** sd_ss_ind_misc_meas_rprt().
  */
  if(sdss_is_hybr_pref(SD_SS_HYBR_PREF_NONE) )
  {
    return sd_ss_ind_misc_pwr_save_enter( rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if ( !defined(FEATURE_TECH_IND_SD_STACK_ARCH) && !defined(FEATURE_HDR_HYBRID))
  return sd_ss_ind_misc_pwr_save_enter( rtrn_act_ptr );
  #else

  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_HYBR_1, SDSS_EVT_MISC_PWR_SAVE_ENTER, rtrn_act_ptr );
  #else
  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_hdr_event_proc( SDSS_EVT_MISC_PWR_SAVE_ENTER, rtrn_act_ptr );
  #endif /* FEATURE_TECH_IND_SD_STACK_ARCH */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

  #endif /* FEATURE_TECH_IND_SD_STACK_ARCH & FEATURE_HDR_HYBRID */
} /* sd_ss_ind_hybr_1_misc_pwr_save_enter */

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_misc_cnf

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS confirms
  the last instructed action like pwr_scan_gw

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SCAN_GW - Request to do early power scan in GSM/WCDMA.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_act_e_type          sd_ss_ind_misc_cnf(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_CNF, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE |
                    SD_SS_ACT_PWR_SCAN_GW) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_misc_cnf */


#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */


#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */


#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
========================== MISCELLANEOUS SERVICES ===========================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sd_misc_prl_ver_get

DESCRIPTION
  Returns the version number of the PRL that is stored in NV and associated
  with the currently selected NAM.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  Version number of the PRL that is stored in NV and associated with the
  currently selected NAM. PRL_DEFAULT_VER if no PRL is stored in NV or PRL
  has no version number associated with it.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  word                      sd_misc_prl_ver_get( void )
{
  return prl_hdr_nv_get_ptr()->pr_list_id;
}




/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_target_supp_mode_band

  Inform the caller to whether a specified combination of mode and band
  preference is supported by the current target.

  For example, SD_SS_MODE_PREF_AMPS is only supported by a target that is
  AMPS capable.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sd_misc_is_target_supp_mode_band(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference to be validated.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference to be validated.
            */
)
{
  /* If the specified mode and band preference combination is NOT supported
  ** by the current target, return FALSE.
  */
  if( ! sdss_is_supp_mode_band_pref(mode_pref, band_pref) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the specified mode and band preference combination is
  ** supported - return TRUE.
  */
  return TRUE;

} /* sd_misc_is_target_supp_mode_band */



/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_mode_band_capability

  Inform the called the modes and bands preference supported by the target.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                   sd_misc_get_mode_band_capability(

        sd_ss_mode_pref_e_type    *mode_pref_ptr,
            /* Mode preferences to be filled in.
            */

        sd_ss_band_pref_e_type    *band_pref_ptr
            /* Band preferences to be filled in.
            */
)
{

  sdss_get_mode_band_capability(mode_pref_ptr, band_pref_ptr);

} /* sd_misc_get_mode_band_capability */



/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_target_supp_mode_band_class

  Inform the caller to whether a specified combination of mode and band
  is supported by the current target.

  For example, SD_SS_MODE_PREF_AMPS is only supported by a target that is
  AMPS capable.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if the specified combination of mode and band is supported by the
  current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sd_misc_is_target_supp_mode_band_class(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference to be validated.
            */

        sd_band_e_type            band
            /* Band to be validated.
            */
)
{
  return ( sd_misc_is_target_supp_mode_band(
                                       mode_pref,
                                       sdss_map_band_to_band_pref(band) ) );
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_target_supp_mode_band_chan

  Inform the caller to whether a specified combination of mode , band class
  and channel number is supported by the current target.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if the specified combination of mode, band class and channel number is 
  supported by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean  sd_misc_is_target_supp_mode_band_chan(

        sd_mode_e_type            mode,
            /* Mode to be validated.
            */

        sd_band_e_type            band,
            /* Band to be validated.
            */

        sys_channel_num_type      chan
            /* Channel number to be validated.
            */
)
{
  return sdss_is_supp_sys(mode,sdss_map_band_to_band_pref(band),chan);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_supp_pref

  Inform the caller to whether a specified combination of mode and band
  preference is supported by the current target, as well as, the PRL that is
  associated with the currently selected NAM.

  For example, SD_SS_MODE_PREF_AMPS is only supported by a target that is
  AMPS capable and a PRL that contains AMPS acquisition records.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.
  sd_nam_sel() must already been called to load the PRL.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target, as well as, the PRL that is associated with the
  currently selected NAM. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sd_misc_is_supp_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference to be validated.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference to be validated.
            */
)
{
  /* If the specified mode and band preference combination is NOT supported
  ** by the current target, return FALSE.
  */
  if( ! sdss_is_supp_mode_band_pref(mode_pref, band_pref) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the specified mode and band preference combination is NOT supported
  ** by the currently loaded PRL, return FALSE.
  */
  if( ! sdprl_is_supp_mode_band_pref(mode_pref, band_pref) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the specified mode and band preference combination is
  ** supported - return TRUE.
  */
  return TRUE;

} /* sd_misc_is_supp_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_cdma_locked

DESCRIPTION
  Return the value of the CDMA locked indicator.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the CDMA locked indicator is set to TRUE. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sd_misc_is_cdma_locked( void )
{
  return sdss_is_cdma_locked();

} /* sd_misc_is_cdma_locked */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_cdma_pseudo_online

  Inform the caller if CDMA is in pseudo online state

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if CDMA was activated with pseudo online. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sd_misc_is_cdma_pseudo_online( void )
{

  sdss_pref_s_type                sdss_pref;
      /* Enumeration of system perference.
      */



  sd_ss_e_type                    ss = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
  ss = sdss_get_ss_for_mode( SD_MODE_CDMA );
  #endif

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /* Currently CDMA can run only on Main
  */
  SD_ASSERT( ss == SD_SS_MAIN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fetch the preferences from main instance.
  */
  sdss_get_pref( ss, &sdss_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( sdss_pref.pref_reason == SD_SS_PREF_REAS_PSEUDO_ONLINE );

}



/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_cdma_request_mode

DESCRIPTION
  Get the CDMA request_mode to be used in CDMA origination and page-response
  messages.

  The CDMA request mode is determined according to the current system
  selection preferences.

DEPENDENCIES
  None.

RETURN VALUE
  CDMA request_mode as defined in ANSI J-STD-008 Table 2.7.1.3.2.4-1 and
  cai.h.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  byte                      sd_misc_get_cdma_request_mode( void )
{
  return sdss_get_cdma_request_mode();
}




/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_cdma_return_cause

DESCRIPTION
  Get the CDMA return_cause to be used in CDMA registration and origination
  messages.

DEPENDENCIES
  None.

RETURN VALUE
  CDMA return_cause as defined in IS-2000-5 Table 2.7.1.3.2.1-2 and cai.h.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  byte                      sd_misc_get_cdma_return_cause( void )
{
  return sd_ss_ptr(SD_SS_MAIN)->return_cause;
}




/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_curr_acq_sys_mode

DESCRIPTION
   Returns the mode of system over which service is currently provided. If
   no service is currently provided, return the mode of the system that
   is being attempted acquisition.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sys_sys_mode_e_type            sd_misc_get_curr_acq_sys_mode( void )
{
  sd_si_s_type          *si_ptr       = sd_si_ptr(SD_SS_MAIN);
  sdsr_s_type           *sr_ptr;
  sys_sys_mode_e_type    sys_mode     = SYS_SYS_MODE_CDMA;
  sd_mode_e_type         acq_sys_mode = SD_MODE_CDMA;
  sdss_pref_s_type       pref_buf;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If service is currently being indicated, return the mode of this
  ** service, Else, return the mode of the ACQ_SYS.
  */
  if( sys_srv_status_is_srv( si_ptr->si_info.srv_status ) )
  {
    acq_sys_mode = si_ptr->si_info.mode;
  }
  else
  {
    sr_ptr = sdsr_list_get_ptr( SDSR_ACQ_SYS, 0 );
    if ( sr_ptr != NULL )
    {
      acq_sys_mode = sr_ptr->sys.mode;
    }
    else
    {
      /* Not attempting acquisitions, use the mode_pref settings.
      */
      sdss_get_pref( SD_SS_MAIN, &pref_buf );

      #if defined(FEATURE_ACP)
#error code not present
      #endif

      #if defined(FEATURE_GSM)
#error code not present
      #endif

      #if defined(FEATURE_WCDMA)
#error code not present
      #endif

      #if defined(FEATURE_HDR)
#error code not present
      #endif

      #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
      if ( SD_MODE_CONTAIN( pref_buf.true_mode_pref, BM( SD_MODE_CDMA )) )
      {
       acq_sys_mode = SD_MODE_CDMA;
      }
      #endif

    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Map SD mode to SYS mode.
  */
  switch ( acq_sys_mode )
  {
    case SD_MODE_CDMA:
      sys_mode =  SYS_SYS_MODE_CDMA;
      break;

    case SD_MODE_AMPS:
      sys_mode =  SYS_SYS_MODE_AMPS;
      break;

    case SD_MODE_HDR:
      sys_mode =  SYS_SYS_MODE_HDR;
      break;

    case SD_MODE_GSM:
    case SD_MODE_GW:
      sys_mode =  SYS_SYS_MODE_GSM;
      break;

    case SD_MODE_WCDMA:
      sys_mode =  SYS_SYS_MODE_WCDMA;
      break;

    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_GPS:
    case SD_MODE_WLAN:
    case SD_MODE_MAX:
    default:
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sys_mode;

} /* sd_misc_get_curr_acq_sys_mode */

/*===========================================================================

FUNCTION sd_misc_is_sys_same

DESCRIPTION

Checks if both the sys are same

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if sys_2 is equal to sys_1
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sd_misc_is_sys_same(

        sd_sys_s_type    sys_1,
            /* Specified sys_1
            */

        sd_sys_s_type    sys_2
            /* Specified sys_2
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( sys_1.mode == sys_2.mode &&
      sys_1.band == sys_2.band &&
      sys_1.chan == sys_2.chan )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return FALSE;

}

/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_mode_pref

DESCRIPTION
  Check whether a specified mode_pref2 complies with a specified mode_pref1.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if mode_pref2 complies with mode_pref1
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sd_misc_is_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
            /* Specified mode_Pref1
            */

        sd_ss_mode_pref_e_type    mode_pref2
            /* Mode preference against which to check the compliance.
            */
)
{

  SD_ASSERT_ENUM_IS_INRANGE( mode_pref1, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref2, SD_SS_MODE_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (int)mode_pref1 & (int)mode_pref2 )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sd_misc_is_mode_pref */


/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_remove_mode_pref

DESCRIPTION
  Remove mode_pref2 from mode_pref1.

DEPENDENCIES
  None.

RETURN VALUE
  Resultant mode_pref.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_mode_pref_e_type   sd_misc_remove_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
            /* Specified mode_Pref1.
            */

        sd_ss_mode_pref_e_type    mode_pref2
            /* Specified mode_pref2.
            */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref1, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref2, SD_SS_MODE_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ( SD_MODE_DELETE(mode_pref1, mode_pref2) );


} /* sd_misc_remove_mode_pref() */


/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_add_mode_pref

DESCRIPTION
  Add mode_pref2 with mode_pref1.

DEPENDENCIES
  None.

RETURN VALUE
  Resultant mode_pref.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_mode_pref_e_type   sd_misc_add_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
            /* Specified mode_Pref1.
            */

        sd_ss_mode_pref_e_type    mode_pref2
            /* Specified mode_pref2.
            */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref1, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref2, SD_SS_MODE_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ( SD_MODE_ADD( mode_pref1, mode_pref2) );

} /* sd_misc_add_mode_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_common_mode_pref

DESCRIPTION
  Gets the common mode preference from mode_pref1 and mode_pref2.

DEPENDENCIES
  None.

RETURN VALUE
  Resultant mode_pref.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_mode_pref_e_type   sd_misc_get_common_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
             /* Specified mode_Pref1.
             */

        sd_ss_mode_pref_e_type    mode_pref2
             /* Specified mode_pref2.
             */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref1, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref2, SD_SS_MODE_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ( SD_GET_COMMON_MODE( mode_pref1, mode_pref2) );

} /* sd_misc_get_common_mode_pref */


/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_hybr_opr

DESCRIPTION
  Function to query if we are in hybrid operation, based on SS-HDR instance.

DEPENDENCIES
  None.

RETURN VALUE
  boolean
    TRUE - hybrid operation on,
    FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                  sd_misc_is_hybr_opr(

        void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
  return !sdss_is_hybr_pref( SD_SS_HYBR_PREF_NONE );
  #elif defined FEATURE_HDR_HYBRID
#error code not present
  #else
  return FALSE;
  #endif


} /* sd_misc_is_hybr_opr() */


/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_handoff

DESCRIPTION
  Function to query if the AT is in Handoff.

DEPENDENCIES
  None.

RETURN VALUE
  boolean
    TRUE - hybrid operation on,
    FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                  sd_misc_is_handoff(

        sd_ss_ho_e_type          ho_type
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (sdss_misc_is_handoff(ho_type));

} /* sd_misc_is_hybr_opr() */

/*===========================================================================

FUNCTION sd_misc_get_ss_for_mode

DESCRIPTION
  Function to get the SS for the mode, to be used internally by CM and MMOC
  Only

DEPENDENCIES
  None.

RETURN VALUE
 ss for the mode
 SD_SS_MAX if invalid call

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_e_type             sd_misc_get_ss_for_mode(

        sys_sys_mode_e_type      sys_mode
       /*lint -esym(715, sys_mode) *//* param not referenced*/)
{
  return sdss_get_ss_for_mode( sys_mode );
}
/*lint +esym(715, sys_mode) */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_filter_out_overlap_band

DESCRIPTION
  Get band preference complies to MS radio access capability.Clear PGSM band
  pref if EGSM band pref is present. Clear both EGSM and PGSM band pref if
  RGSM band pref is present.

  SEMANTIC RULE : Among the three Access Type Technologies GSM 900-P,
  GSM 900-E and GSM 900-R only one shall be  present.

  Refer to 3GPP TS 05.05 Frequency in MHz
             Mobile Tx     Base Station Tx
  PGSM       890-915       935-960
  EGSM       880-915       925-960
  RGSM       876-915       921-960

DEPENDENCIES
  None

RETURN VALUE
  band preference enum type

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_band_pref_e_type    sd_misc_filter_out_overlap_band(

        sd_ss_band_pref_e_type    band_pref
            /* Enumeration of system selection band and sub-band preferences.*/
)
{
  return (sd_filter_out_overlap_band(band_pref));
} /* sd_misc_filter_out_overlap_band */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_band_mode

DESCRIPTION
  Filter out the band and mode not supported by the target capability.
  Filter out the band and mode that are not in part of system preference.
  Filter out any overlapped bands.
  Bands that do not have corresponding modes.
  Modes that do not have corresponding bands.

  MODE
DEPENDENCIES
  None

RETURN VALUE
  Return the filtered mode and band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sd_misc_get_band_mode(

        sd_ss_mode_pref_e_type    *mode_ptr,
            /* Pointer to the mode preference that needs to be filtered.
            */

        sd_ss_band_pref_e_type    *band_ptr,
            /* Pointer to the band preference that needs to be filtered.
            */

        sd_ss_mode_pref_e_type    sys_mode_pref,
            /* Enumeration of system mode perference.
            */

        sd_ss_band_pref_e_type    sys_band_pref
            /* Enumeration of system selection band and sub-band preferences.
            */

)
{
  sd_get_band_mode(mode_ptr, band_ptr, sys_mode_pref, sys_band_pref);
} /* sd_misc_get_gw_band_mode */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_supp_band_pref

DESCRIPTION

  Return the band preference that is based on the target capability and user
  preference.

  MODE
DEPENDENCIES
  None

RETURN VALUE
  Return the filtered band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sd_ss_band_pref_e_type    sd_misc_get_supp_band_pref(void)
{
  sd_ss_band_pref_e_type          band_pref;
      /* The band preference that needs to be filtered.
      */

  sdss_pref_s_type                sdss_pref;
      /* Enumeration of system perference.
      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fetch the preferences from main instance.
  */
  sdss_get_pref( SD_SS_MAIN, &sdss_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out bands that do not have corresponding modes and
  ** that are not supported by target.
  */
  band_pref = sdss_get_supp_mode_band_pref( sdss_pref.mode_pref,
                                            sdss_pref.band_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out overlapping bands.
  */
  band_pref = sd_filter_out_overlap_band( band_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return band_pref;

} /* sd_misc_get_supp_band_pref */

/*===========================================================================

FUNCTION sd_misc_is_fts_mode

DESCRIPTION
  Return the an boolean indication whether the system is in Full-time SHDR 
  mode or not, based on the target capability and user preference.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If the phone is in FTS Mode.
  FALSE: If the phone is not in FTS Mode.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean    sd_misc_is_fts_mode(void)
{

  #ifndef FEATURE_FULL_TIME_SHDR_3
  return FALSE;
  #else

  sd_ss_mode_pref_e_type mode_pref;
  sd_ss_mode_pref_e_type hybr_mode_pref;
  sd_ss_band_pref_e_type band_pref;

  sdss_pref_s_type                sdss_pref;
      /* Enumeration of system perference.
      */

  sdss_pref_s_type                sdss_hybr_pref;
      /* Enumeration of system perference.
      */

  /* Fetch the preferences from main instance.
  */
  sdss_get_pref( SD_SS_MAIN, &sdss_pref );
  sdss_get_pref( SD_SS_HYBR_HDR, &sdss_hybr_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If hybrid is turned off, return FALSE
  */
  if( !sd_misc_is_hybr_opr() )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out bands that do not have corresponding modes and
  ** that are not supported by target.
  */
  mode_pref      = sdss_get_supp_mode_pref( sdss_pref.mode_pref,
                                            sdss_pref.band_pref );

  hybr_mode_pref = sdss_get_supp_mode_pref( sdss_hybr_pref.mode_pref,
                                            sdss_hybr_pref.band_pref );

  mode_pref      = sd_misc_add_mode_pref( mode_pref, hybr_mode_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the supported bands
  */
  band_pref      = sdss_get_supp_band_pref( sdss_pref.band_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out the mode and band pref
  */
  mode_pref = sdss_get_supp_mode_pref( mode_pref, band_pref );
  band_pref = sdss_get_supp_mode_band_pref( mode_pref, band_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the mode doesn't have HDR, return FALSE
  */
  if( !sd_misc_is_mode_pref( SD_SS_MODE_PREF_HDR, mode_pref ))
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the mode doesn't have CDMA, return FALSE
  */
  if( !sd_misc_is_mode_pref( SD_SS_MODE_PREF_CDMA, mode_pref ))
  {
    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If there are no HDR systems in the PRL, return FALSE
  */
  if( !sdss_is_prl_support_hdr())
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* All FTS conditions satisfied
  */
  return TRUE;

  #endif
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_sys_id_print

DESCRIPTION
  This function prints the sys_id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
MMEXTN  void                      sd_misc_sys_id_print(

  sys_sys_id_s_type     sys_id
      /* Sys Id
      */

)
{

  unsigned int       i = 0;

  switch (sys_id.id_type)
    {
      case SYS_SYS_ID_TYPE_UMTS      :
        SD_MSG_MED("Plmn: ",0, 0, 0);

        for ( i =0; i < ARR_SIZE(  sys_id.id.plmn_lac.plmn.identity ); i++)
        {
            SD_MSG_MED(" Identity %d ", sys_id.id.plmn_lac.plmn.identity[i],
                                        0, 0);
        }

        SD_MSG_MED(" Lac: %d ", sys_id.id.plmn_lac.lac, 0, 0);

        break;

      case SYS_SYS_ID_TYPE_IS95      :
        SD_MSG_MED("Sid: %d Nid: %d", sys_id.id.is95.sid,
                                       sys_id.id.is95.nid, 0 );

        break;

      case SYS_SYS_ID_TYPE_IS856     :
        sdsr_hdr_sid_print( sys_id.id.is856 );
        break;

      case SYS_SYS_ID_TYPE_WLAN:
        if( sys_id.id.ssid.len == 0 )
        {
          SD_MSG_MED(" Broadcast ssid", 0,0,0 );
        }
        else
        {
          for( i=0; i < sys_id.id.ssid.len; i++)
            SD_MSG_MED(" ssid[%d] %c", i, sys_id.id.ssid.ssid[i], 0 );
        }
        break;

       case SYS_SYS_ID_TYPE_UNDEFINED :
        break;

       default                        :
         break;
     } /* switch id_type */
  return;

} /* sd_misc_sys_id_print */




/*===========================================================================

FUNCTION sd_misc_is_del_tmsi

DESCRIPTION
  This function indicates that whether tmsi should be delete or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
MMEXTN  boolean                      sd_misc_is_del_tmsi(void)
{


  sd_ss_s_type                       *ss_ptr = sd_ss_ptr( SD_SS_MAIN );
  sdsr_e_type                        list; /* reference list */
  int                                pos;  /* reference list position */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The list and position most recently attempted acquisition
  */
  list = ss_ptr->acq_list;
  pos = ss_ptr->acq_pos;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( sdsr_is_del_tmsi(list, pos) );

}/* sd_misc_is_del_tmsi */

#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_orig_mode

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
MMEXTN  boolean                       sd_misc_is_orig_mode(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HDR.
            */

        sd_ss_orig_mode_val_e_type    orig_mode_val
            /* Origination mode value.
            */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( orig_mode_val, SD_SS_ORIG_MODE_VAL_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( sdss_is_orig_mode( ss, orig_mode_val ) );

} /* sdss_is_orig_mode */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_sys_avoid_list

DESCRIPTION
  Return a list of avoiding system (channels) for a mode preference

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the avoided list and the number of avoided systems.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                       sd_misc_sys_avoid_list(

        sd_mode_e_type               mode,   
            /* Origination mode value.
            */

        sys_avoided_channel_record_type     *avoid_list_ptr,
            /* Pointer to avoided channels
            */
        
        byte                        *num_sys,

        byte                         max_entry
)
{
  sd_ss_e_type  ss; 
  sdsr_s_type   *tbl_ptr;
  byte i;
  byte num_avoided_sys = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT( avoid_list_ptr != NULL );

  SD_MSG_HIGH("Enter sd_misc_sys_avoid_list ", 0, 0, 0);

  ss = sdss_get_ss_for_mode(mode);

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  for( i=0; (tbl_ptr=sdsr_tbl_get_ptr(i)) != NULL; i++ )
  {

    /* Loop through all the system with the given mode preference */
    if( (tbl_ptr->sys.mode == mode) && sdss_sr_is_avoid( ss, tbl_ptr) )
    {
      num_avoided_sys++;
      if(num_avoided_sys <= max_entry)
      {
        SD_MSG_HIGH(" This System is avoided:  Band = %d, Mode = %d, Channel = %d ",
        sdss_map_band_pref_to_band(tbl_ptr->sys.band), tbl_ptr->sys.mode, tbl_ptr->sys.chan);

        /* if the system is avoided, append the avoided system to the list */
        avoid_list_ptr->channel.band = 
          sdss_map_band_pref_to_band(tbl_ptr->sys.band);
        avoid_list_ptr->channel.chan_num = tbl_ptr->sys.chan;
        avoid_list_ptr->mode = tbl_ptr->sys.mode;

        /* update timer - used by HDR for future optimization */
        avoid_list_ptr->uptime = clk_uptime();
        avoid_list_ptr->avoided_time = tbl_ptr->sched_prm.avoid_time;

        /* Update to next pointer */
        avoid_list_ptr++;
      }
      else
      {
        /* Keep the max count to be max number entry */
        num_avoided_sys = max_entry;
        SD_MSG_ERROR( "List Over Flow",0,0,0 );
      }
    }
  }

  *num_sys = num_avoided_sys;
  return;
} /* sd_misc_sys_avoid_list */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
================================= GENERAL ===================================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION sdtask_crit_sect_ptr

DESCRIPTION

  Return pointer to SD critical section.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
rex_crit_sect_type* sd_crit_sect_ptr(void)
{
  static rex_crit_sect_type crit_sect;
    /* The one and only SD critical section */

  return &crit_sect;

}

/*===========================================================================

FUNCTION sdtask_init_crit_sect

DESCRIPTION
  Initialize SD critical section.
  This function must be called before any other part of SD is initialized

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void sd_init_crit_sect(void)
{

  rex_init_crit_sect(sd_crit_sect_ptr());

}

/*===========================================================================

FUNCTION sd_rex_enter_crit_sect

DESCRIPTION
  Enter SD critical section.
  This function must be called before any other part of SD is initialized

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void sd_rex_enter_crit_sect(void)
{

  rex_enter_crit_sect(sd_crit_sect_ptr());
}

/*===========================================================================

FUNCTION sd_rex_leave_crit_sect

DESCRIPTION
  Leave SD critical section.
  This function must be called before any other part of SD is initialized

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void sd_rex_leave_crit_sect(void)
{

  rex_leave_crit_sect(sd_crit_sect_ptr());
}
/*===========================================================================

FUNCTION sd_init_before_app_task_start

DESCRIPTION
  Perform SD initialization that needs to be done before application tasks
  have started and NV task has been started.

  NOTE! Any initialization that must be done before SD clients tries to
  contact SD, must be done in this function, as opposed to sd_init().

  NOTE! Any initialization that result in SD contacting some other tasks
  must be done in sd_init(), as opposed to this function.

DEPENDENCIES
  This function must be called before any other SD function is ever called.
  This function shall not be called a second time prior to calling on
  sd_term().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sd_init_before_app_task_start( void )
{

  /* Initialize critical section of SD */
  sd_init_crit_sect();

  /* Initialize the system record component.
  */
  sdsr_component_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the main service indicators component.
  */
  sd_si_init( SD_SS_MAIN, sd_ss_ptr(SD_SS_MAIN), SDSR_SI_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
  /* Initialize the SS_HYBR_1 service indicators component.
  */
  sd_si_init( SD_SS_HYBR_1, sd_ss_ptr(SD_SS_HYBR_1), SDSR_HYBR_1_SI_SYS );
  #elif defined FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system selection core.
  */
  sdss_component_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the SDPRL before task start.
  */
  sdprl_init_before_task_start();

} /* sd_init_before_app_task_start */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_init

DESCRIPTION
  Perform SD initialization that needs to be done after tasks have started.

  NOTE! Any initialization that must be done before SD clients tries to
  contact SD, must be done in sd_init_before_task_start(), as opposed to
  this function.

  NOTE! Any initialization that result in SD contacting some other tasks,
  such as NV, must be done in this function, as opposed to
  sd_init_before_task_start()

  This function registers an SS-Timer callback function with SD. SD calls on
  the provided callback function to set/disable the SS-Timer. Upon SS-Timer
  expiration the SS-Client should call on the sd_ss_ind_misc_timer()
  SS-Indication function.

DEPENDENCIES
  This function must be called after sd_init_before_task_start() and
  before any other SD function is ever called. This function shall not be
  called a second time prior to calling on sd_term().

RETURN VALUE
  TRUE if SD initialization is successful (i.e. SD NV items are successfully
  read). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sd_init(
            /*lint -esym(715, hdr_timer_func_ptr) *//* param not referenced*/
            /*lint -esym(715, wlan_timer_func_ptr) *//* param not referenced*/
        rex_sigs_type             nv_sig,
            /* Rex signal to be used by SD for waiting on NV to completes a
            ** read/write operation.
            */

        rex_sigs_type             dog_sig,
            /* Rex signal to be used by SD for watchdog reporting while SD
            ** waits on NV to complete a read/write operation.
            */

        void                      (*dog_rpt_func_ptr) (void),
            /* Pointer to a watchdog reporting function to be called by SD
            ** when the watchdog sig is set while SD waits on NV to complete
            ** a read/write operation.
            */

        void                      (*timer_func_ptr) (int4  ms),
            /* Pointer to an SS-Timer callback function.
            **
            ** SD calls on this function to set/disable the SS-Timer. Upon
            ** SS-Timer expiration the SS-Client should call on the
            ** sd_ss_ind_misc_timer() SS-Indication function.
            **
            ** The argument to this function is the time in ms with which
            ** to set the SS-Timer. An input of 0 should disable the
            ** SS-Timer */


        void                      (*hdr_timer_func_ptr) (int4  ms),
            /* Pointer to an HDR SS-Timer callback function for CDMA/HDR
            ** hybrid operation (equivalent to timer_func_ptr).
            **
            ** If target is not capable of CDMA/HDR hybrid operation, this
            ** parameter should be set to NULL */


        void                      (*wlan_timer_func_ptr) (int4  ms)
            /* Pointer to an SS-HYBR_WLAN SS-Timer callback function for
            ** hybrid operation (equivalent to timer_func_ptr).
            **
            ** If target is not capable of any+WLAN hybrid operation, this
            ** parameter can be set to NULL */
)
{
  SD_ASSERT( nv_sig != 0 );
  SD_ASSERT( dog_sig != 0 );
  SD_ASSERT( dog_rpt_func_ptr != NULL );
  SD_ASSERT( timer_func_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system record component. We need to initialize here,
  ** as well, because when switching in and out of LPM/FTM
  ** sd_init_before_task_start() is not being called.
  */
  sdsr_component_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the SD NV component.
  */
  sdnv_init( nv_sig, dog_sig, dog_rpt_func_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system selection front end and core.
  */
  sd_ss_init( sd_ss_ptr(SD_SS_MAIN), timer_func_ptr );

  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
  SD_ASSERT( hdr_timer_func_ptr != NULL );
  sd_ss_init( sd_ss_ptr(SD_SS_HYBR_1), hdr_timer_func_ptr );
  #elif defined FEATURE_HDR_HYBRID
#error code not present
  #endif

  #ifdef FEATURE_WLAN
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the PRL component.
  */
  if( ! sdprl_init() )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read the configurable SD items from the NV.
  */
  sdssscr_nv_items_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the sdss NV items
  */
  sdss_nv_init( SD_SS_MAIN );

  #ifdef FEATURE_TECH_IND_SD_STACK_ARCH
  sdss_nv_init( SD_SS_HYBR_1 );
  #elif defined (FEATURE_HDR_HYBRID)
#error code not present
  #endif

  #ifdef FEATURE_WLAN
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, SD initialization is successful - return TRUE.
  */
  return TRUE;

} /* sd_init */
  /*lint +esym(715, hdr_timer_func_ptr) *//* param not referenced*/
  /*lint +esym(715, wlan_timer_func_ptr) *//* param not referenced*/


/* <EJECT> */
/*===========================================================================

FUNCTION sd_nam_sel

DESCRIPTION
  Inform SD of a new NAM selection, as well as, the selected NAM's
  SS-Preferences.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully extracted from NV). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                          sd_nam_sel(

        sd_nam_e_type                     curr_nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type             orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type             mode_pref,
            /* The mode preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              band_pref,
            /* The band preference that is associated with the selected NAM.
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /* The band preference that is associated with the selected NAM.
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type              tech_pref,
            /* Preference for Technology settings.
            */

        sys_wlan_scan_pref_e_type           wlan_scan_pref,
           /* WLAN scan preference.
           */

        boolean                             is_gw_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_1x_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */


       boolean                              is_wlan_subsc_avail
             /* Indicates if subscription is available for providing service on
             ** WLAN network.
             */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call the new sd_nam_sel2 function.
  */
  return sd_nam_sel2( curr_nam,
                      orig_mode,
                      mode_pref,
                      band_pref,
                      prl_pref,
                      roam_pref,
                      hybr_pref,
                      manual_sys_info_ptr,
                      domain_pref,
                      acq_order_pref,
                      tech_pref,
                      wlan_scan_pref,
                      SYS_WLAN_BSS_TYPE_ANY,
                      is_gw_subsc_avail,
                      is_1x_subsc_avail,
                      is_wlan_subsc_avail
                    );

}



/* <EJECT> */
/*===========================================================================

FUNCTION sd_nam_sel2

DESCRIPTION
  Inform SD of a new NAM selection, as well as, the selected NAM's
  SS-Preferences.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully extracted from NV). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                          sd_nam_sel2(

        sd_nam_e_type                     curr_nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type             orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type             mode_pref,
            /* The mode preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              band_pref,
            /* The band preference that is associated with the selected NAM.
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /* The band preference that is associated with the selected NAM.
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type              tech_pref,
            /* Preference for Technology settings.
            */

        sys_wlan_scan_pref_e_type           wlan_scan_pref,
           /* WLAN scan preference.
           */

        sys_wlan_bss_e_type                 bss_type_pref,
           /* BSS type.
           */

        boolean                             is_gw_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_1x_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */


       boolean                              is_wlan_subsc_avail
             /* Indicates if subscription is available for providing service on
             ** WLAN network.
             */
)
{
  /* Point at manual system record.
  */
  sdsr_s_type         *user_sys_ptr  = NULL;
  sd_ss_mode_pref_e_type  mode_pref_on_ss_main = SD_SS_MODE_PREF_NONE;


  int                 i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( INRANGE(curr_nam, SD_NAM_1, SD_NAM_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( curr_nam != SD_NAM_MAX )
  {
    SD_ASSERT_ENUM_IS_INRANGE( orig_mode, SD_SS_ORIG_MODE_MAX );
    SD_ASSERT( orig_mode != SD_SS_ORIG_MODE_OTASP );

    SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
    SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
    SD_ASSERT_ENUM_IS_INRANGE( roam_pref, SD_SS_ROAM_PREF_MAX );
    SD_ASSERT_ENUM_IS_INRANGE( hybr_pref, SD_SS_HYBR_PREF_MAX );
    SD_ASSERT_ENUM_IS_INRANGE_INC( tech_pref, SYS_TECH_PREF_MASK_ANY );
    SD_ASSERT_ENUM_IS_INRANGE( wlan_scan_pref, SYS_WLAN_SCAN_PREF_MAX );
    SD_ASSERT_ENUM_IS_INRANGE( bss_type_pref, SYS_WLAN_BSS_TYPE_MAX );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the subscription status in SS-Core.
  ** This portion has to be done first as SD-NV operation depended on
  ** subscription status.
  */
  sdss_subsc_avail_update( SD_SS_MAIN,
                           is_gw_subsc_avail,
                           is_1x_subsc_avail,
                           is_wlan_subsc_avail
                         );
  sdss_subsc_avail_update( SD_SS_HYBR_HDR,
                           is_gw_subsc_avail,
                           is_1x_subsc_avail,
                           is_wlan_subsc_avail
                         );
  sdss_subsc_avail_update( SD_SS_HYBR_WLAN,
                           is_gw_subsc_avail,
                           is_1x_subsc_avail,
                           is_wlan_subsc_avail
                         );


  #ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
  #endif

  /* Inform the SD-PRL component of the new NAM selection. If NAM change
  ** is not successful, return FALSE
  */
  if( ! sdprl_nam_sel( curr_nam, hybr_pref ) )
  {
    return FALSE;
  }

  /* Orig mode is Manual GW, Set mode pref to GW. 
  ** This will change ORIG mode to EMERG_ORIG when GW subs is 
  ** not avail in Manual mode
  */
  if(sdss_is_orig_mode0( orig_mode, SD_SS_ORIG_MODE_VAL_MANUAL ))
  {
    mode_pref_on_ss_main = SD_GET_COMMON_MODE(mode_pref, SD_SS_MODE_PREF_GW);
  }
  else
  {
    mode_pref_on_ss_main = mode_pref;
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If a new NAM is selected, call on the SS-Core to update its origination
  ** mode and SS-Preferences per the selected NAM's SS-Preferences.
  */
  if( curr_nam != SD_NAM_MAX )
  {
    (void)sdss_pref_update( SD_SS_PREF_REAS_USER,
                            orig_mode,
                            mode_pref_on_ss_main,
                            band_pref,
                            prl_pref,
                            roam_pref,
                            hybr_pref,
                            SD_BAND_MAX,
                            SD_BLKSYS_MAX,
                            domain_pref,
                            acq_order_pref,
                            tech_pref,
                            wlan_scan_pref,
                            bss_type_pref
                          );
   sd_ss_ptr(SD_SS_MAIN)->is_start_instance = TRUE;

   #ifdef FEATURE_TECH_IND_SD_STACK_ARCH

   (void)sdss_hybr_1_pref_update( SD_SS_PREF_REAS_USER,
                               SD_SS_ORIG_MODE_COLLOC,
                               mode_pref,
                               band_pref,
                               prl_pref,
                               roam_pref,
                               hybr_pref,
                               domain_pref,
                               acq_order_pref,
                               tech_pref,
                               wlan_scan_pref,
                               bss_type_pref
                             );
    sd_ss_ptr(SD_SS_HYBR_1)->is_start_instance = TRUE;

    /* Force the SDSR_HDR_XXX lists to reference the lists corresponding to
    **hybrid preference.
    */
    //sdsr_lists_hybr_pref_switch( hybr_pref );

   #elif defined FEATURE_HDR_HYBRID
#error code not present
    #endif /* FEATURE_HDR_HYBRID */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if manual system selection is enabled.
  */
  if ( sdss_is_orig_mode0( orig_mode, SD_SS_ORIG_MODE_VAL_MANUAL ) &&
       manual_sys_info_ptr != NULL &&
       manual_sys_info_ptr->sys_id.id_type == SYS_SYS_ID_TYPE_UMTS
     )
  {

    for( i=0 ; i<sdsr_list_cnt(SDSR_MANUAL_LST); i++ )
    {
      /* Fetch the user system record.
      */
      user_sys_ptr   = sdsr_list_get_ptr( SDSR_MANUAL_LST, i );

      /* Manual selection is enabled.
      ** Update plmn fields with propreiate values.
      */

      if(user_sys_ptr == NULL)
      {
        SD_MSG_LOW("user_sys_ptr NULL",0,0,0);
        continue;
      }

      /* Check if it GW system.
      */
      if ( user_sys_ptr->sys.mode == SD_MODE_GSM ||
           user_sys_ptr->sys.mode == SD_MODE_WCDMA ||
           user_sys_ptr->sys.mode == SD_MODE_GW
         )
      {

        switch ( manual_sys_info_ptr->prm.gw.sel_type )
        {
          case SYS_MANUAL_SEL_TYPE_HPLMN:
            user_sys_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_HPLMN;
            break;

          case SYS_MANUAL_SEL_TYPE_RPLMN:
            user_sys_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_MANUAL_SYS;
            sys_plmn_undefine_plmn_id( &user_sys_ptr->sid_expected.prm.plmn.plmn_id );
            break;

          case SYS_MANUAL_SEL_TYPE_USER_SPECIFIED:
            user_sys_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_MANUAL_SYS;
            user_sys_ptr->sid_expected.prm.plmn.plmn_id = manual_sys_info_ptr->sys_id.id.plmn;
            break;

          case SYS_MANUAL_SEL_TYPE_NONE:
          case SYS_MANUAL_SEL_TYPE_MAX:
          default:
            SD_ERR_FATAL("Invalid net sel Type", 0, 0, 0 );

        } /* switch ( manual_sys_info_ptr->prm.gw.sel_type ) */
        user_sys_ptr->prl = (byte)SDSS_PRL_MOST_PREF;
      } /* ( if ( user_sys_ptr->sys.mode ... ) */

    } /* for ( ...  */

  } /*  if ( sdss_is_orig_mode( orig_mode, SD_SS_ORIG_MODE_VAL_MANUAL) )  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset the timer
  */
  sd_ss_timer_reset( SD_SS_MAIN );
  sd_ss_timer_reset( SD_SS_HYBR_HDR );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, NAM change is successful - return TRUE.
  */
  return TRUE;

} /* sd_nam_sel */




/* <EJECT> */
/*===========================================================================

@FUNCTION sd_wlan_nam_sel

@DESCRIPTION
  Inform SD of a new WLAN preferences

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  None

@SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                          sd_wlan_nam_sel(

        sd_nam_e_type                     curr_nam,
            /**< Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type             orig_mode,
            /**< The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type             mode_pref,
            /**< The mode preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              band_pref,
            /**< The band preference that is associated with the selected NAM.
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /**< New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /**< The band preference that is associated with the selected NAM.
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /**< New hybrid preference (none, CDMA/HDR).
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type              tech_pref,
            /**< Preference for Technology settings.
            */

        sys_wlan_scan_pref_e_type           wlan_scan_pref,
           /**< WLAN scan preference.
           */

        sys_wlan_bss_e_type                 bss_type,
           /**< BSS type.
           */

        boolean                             is_gw_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_1x_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */


       boolean                              is_wlan_subsc_avail
             /**< Indicates if subscription is available for providing service on
             ** WLAN network.
             */
)
{
  SYS_ARG_NOT_USED(manual_sys_info_ptr);
  #ifdef FEATURE_WLAN 
#error code not present
  #else
  SD_MSG_HIGH("FEATURE_WLAN undefined",0,0,0);

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(curr_nam);
  SYS_ARG_NOT_USED(orig_mode);
  SYS_ARG_NOT_USED(mode_pref);
  SYS_ARG_NOT_USED(band_pref);
  SYS_ARG_NOT_USED(prl_pref);
  SYS_ARG_NOT_USED(roam_pref);
  SYS_ARG_NOT_USED(hybr_pref);
  SYS_ARG_NOT_USED(domain_pref);
  SYS_ARG_NOT_USED(acq_order_pref);
  SYS_ARG_NOT_USED(tech_pref);
  SYS_ARG_NOT_USED(wlan_scan_pref);
  SYS_ARG_NOT_USED(bss_type);
  SYS_ARG_NOT_USED(is_1x_subsc_avail);
  SYS_ARG_NOT_USED(is_gw_subsc_avail);
  SYS_ARG_NOT_USED(is_wlan_subsc_avail);

  #endif

}


/* <EJECT> */
/**===========================================================================

FUNCTION sd_term

DESCRIPTION
  Terminate SD.

  NOTE! This function must be called when powering-off the phone. After
  calling this function no other public SD function shall be called prior to
  calling sd_init().

DEPENDENCIES
  This function must be called when powering-off the phone. After calling
  this function no other public SD function shall be called prior to
  calling sd_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sd_term( void )
{
  /* Do nothing.
  */
}

/*===========================================================================

FUNCTION sd_ss_get_supp_mode_pref

DESCRIPTION
  Wrapper function for sdss_get_supp_mode_pref.
  Used to call from outside of SD.   

DEPENDENCIES
  None.

RETURN VALUE
  Return a mode preference that corresponds to all the supported modes
  preferences out of the input mode preference.

SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_mode_pref_e_type    sd_ss_get_supp_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference from which to extract supported modes.
            */

        sd_ss_band_pref_e_type    band_pref
            /* band preference from which to extract supported modes.
            */
)
{
  return sdss_get_supp_mode_pref (mode_pref, band_pref); 
}
