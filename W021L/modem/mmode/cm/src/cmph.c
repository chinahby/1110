/*====*====*====*====*====*====*====*===*====*====*====*====*====*====*====*

           C A L L   M A N A G E R   P H O N E   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Phone Object.

  The Phone Object is responsible for:
  1. Processing clients' phone commands.
  2. Processing MC replies for clients' phone commands.
  3. Processing MC phone related notifications.
  4. Notifying the client list of phone events.


EXTERNALIZED FUNCTIONS

  cmph_init
    Initializing the phone object.

  cmph_get_curr_nam
    Get the current NAM.

  cmph_autonam_is_enabled
    Check whether auto-NAM is currently enabled.

  cmph_is_valid_mode_pref
    Check whether a given mode preference is valid.

  cmph_is_valid_mode_band_pref
    Check whether a given mode preference is valid.

  cmph_force_mode_on_the_fly
    Force the mode preference on the fly.

  cmph_unforce_mode_on_the_fly
    Unforce the mode preference on the fly.

  cmph_call_start
    Does necessary phone's call start activities.

  cmph_call_end
    Does necessary phone's call end activities.

  cmph_is_waiting_for_reply
    Check whether the phone object is currently waiting for a reply.

  cmph_info_get.
    Copy the current phone state information into a specified buffer.

  cmph_client_cmd_proc.
    Process clients' phone commands

  cmph_mc_rpt_proc.
    Process MC reports.

  cmph_timer_proc
    Process timer events.


  cmph_offline
     Perform offline processing


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmph_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmph.c#22 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/17/09   em      Removed check for network_sel_mode_pref to be manual in
                   in cmph_client_cmd_check()
06/01/09   rk      Sending PDP_ABORT_REQ to NAS on DETACH/or conflicting 
                   mode change, so that NAS can deactivate PDP calls locally.
05/22/09   rm      RAT Balancing - Force EF_RAT mode when moving from LPM/FTM
                   to ONLINE.
05/06/09   aj      Reject get_networks for WLAN in absence of subscription
04/23/09   rm      Updating correct mode pref of phone when data_avaialable
                   request comes. Made misc_intersect_mode_pref non-static
04/09/09   aj      Added support for IRAT measurements 
03/26/09   ks      Added support for disable_call_type to cmnv_data_s_type
03/23/09   mh      Added support for prl preferred setting in ph info
03/09/09   rm      Updating correct mode pref of phone when data_avaialable
                   request comes 
02/13/09   rk      Removed CDMA feature flag so that end PS call when 
                   PS_DETACH is received will be applicable for GW target.
02/06/09   aj      WLAN subscription decoupling related changes
01/29/09   rm      RAT balancing phase-2 - Vote YES to EF_RAT refresh during
                   CS/PS calls - MMGSDI changes
01/19/09   sv      Updated cmph_term_pending_get_networks_req to accept SS
12/08/08   fj      Added function cmph_map_sd_mode_pref_to_sys_mode_mask();
                   Added update of ph_ptr->mode_capability and 
                   ph_ptr>band_capability in cmph_init();
                   Updated function cmph_get_mode_band_capability();
                   Added update of ph_ptr->mode_capability and 
                   ph_ptr>band_capability in cmph_info_get();
11/25/08   sv      Corrected Get Networks cmd check to check if the current
                   mode preference supports the get networks request
                   Corrected sys_sel_pref cmd check to check if network selection
                   pref and mode preference combination is valid.
11/21/08   am      Fixing compilation error for Running off-target tests
11/18/08   am      fixing RAT balancing - Honor EF_RAT file changes while 
                   power up on MMGSDI side
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
11/13/08   aj      End data calls when user changes mode pref
12/11/08   am      Changes for mode retained after power cycle.
11/11/08   ks      Resolve Klocwork defects
11/07/08   rm      RAT balancing - Honor EF_RAT file changes while power up
11/06/08   rm      RAT balancing - Block user from changing mode_pref if
                   and only if mode_pref is being changed. 
11/03/08   pk/ak   Updated the HICPS for RIM issues.
10/29/08   sv      WLAN System is written to manual netlist everytime wlan
                   system is selected manually.
10/21/08   ks      Update default threshold value for sir and pathloss and
                   add masks for RSSI event
10/02/08   aj      Add support for BC 17,18,19
10/01/08   sv      Featurizing voc.h include to remove lint errors.
09/18/08   rm      RAT Balancning enhancement- Block user from changing mode pref
                   only if ENS flag is ON and UIM_USIM_RAT file is present 
09/17/08   ks      When OOS in CELL_FACH, UE needs to follow UOOS scanning 
                   pattern to re-acquire service
09/15/08   cl      Add initialization before subscription commands are sent.
                   This allows the service domain to be sent to UI correctly.
09/12/08   fj/aj   Support Android RIL for ARM9 Reset
09/10/08   ak      Fixed scenario of data call drop during GSM scan
09/04/08   dm/sv   Implemented WPS support for WLAN
09/02/08   sv      Corrected the condition when manual wlan system preferences
                   are written to netlist.
08/28/08   aj      Update cmph_client_cmd_check for CM_PH_CMD_CLIENT_ACT 
08/20/08   st      Resolve Klockwork defects
08/05/08   aj      Check for activity on SS before queuing WAKEUP_FROM_STANDBY
08/05/08   rk      Block the mode pref change by CM clients when RAT balancing 
                   feature is enabled.
07/30/08   ak      Fix for the HICPS scenario.
06/17/08   sv      Remove compiler warnings.
06/13/08   sv      Memory Reduction for ULC
06/12/08   sv      Resolve Klocwork defects
06/11/08   fj      Added API cmph_get_mode_band_capability
06/11/08   sv      Exposing cmph_info_alloc for power collapse buffered
                   events notifications.
                   Updating the buffered events notification trigger.
06/10/08   aj      Ensuring that appropriate SD_SS_PREF_REAS is given
                   when calling cmpref_proc_cmd_pref_sys_chgd
06/6/08    rn      Added fn for deleting ps activity from prio queue
06/06/08   sv      Removing unneccesary snd.h
05/27/08   sv      Added support to trigger info_get events when moving to 
                   Power Restore State
05/22/08   rm      Making phone status to move out of EMERG_CB when the phone
                   enters LPM mode
05/22/08   vk/sv   added functionality to handle roam only preference
05/14/08   aj      Subscription change update to fix CR 146713
05/09/08   sv      Featurized msm support under FEATURE_CM_DETECT_HW
05/08/08   sv      Added support to put ph in LPM when modem is not
                   supported in msm.
04/28/08   rm      Doing get_networks with user specified network_type in
                   both AUTO and MANUAL mode
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
04/16/08   vk      changed subscription behaviour to buffer the subscription command
                   if the mode pref does not contain the technology, but will allow
                   all commands to go through if anyone subscription goes through
                   also checks buffers and sends subscription command when mode
                   pref changes
03/14/08   rm      CM-NAS interface for new service req type
03/05/08   vk      corrected code for HYBR_GW configuration
01/29/08   ks      Allow UE to set WCDMA+GSM mode using QXDM
01/24/08   vs      Updating orig_int_hybr_pref in
                   cmph_update_orig_param_incoming_call, to avoid hybr_pref
                   memory corruption.
01/02/08   sk/vs   Added support for disabling hybrid mode using
                   FEATURE_HDR_HYBRID_DISABLE.
12/21/07   vs      Fix for CR#124762 - Ending all active calls when transitioning
                   to LPM
12/20/07   ak      Removed Changes made for 'Allow CM_MODE_PREF_GSM_ONLY if
                   the hardware is EDGE_ONLY'
12/16/07   ks      Added cm_ph_cmd_signal_strength_delta
12/13/07   sk/ak   Added DCM.
12/12/07   rm      Adding RAT-balancing feature support
12/11/07   rn      Allow CM_MODE_PREF_GSM_ONLY if the hardware is EDGE_ONLY
12/07/07   ra      Added support for IMS VCC in HOMER files
11/13/07   sk/sv   SHARC 1.0 support.
11/16/07   rn      Added support for indicating clients of phone event in use
                   state when there is MT call coming in while MO call end
                   cnf is still expected
10/01/07   rm      Handling of GSDI_CARD_ERR_POLL_ERROR in CM
08/08/07   cl      Add OTASP Support for ASW Band
08/07/07   ic      Lint clean up
07/26/07   rn      Allow manual plmn selection during a PS call
07/16/07   sk      Changes to avoid garbage values in bss_type_pref when
                   ADHOC feature is not defined.
06/22/07   sk      DDTM rewrite and added support for ATOM.
06/14/07   pk/cl   Added support for power suspend
06/13/07   rp/cl   Changing logic to set srv_domain_to_be_forced
06/11/07   sk      Added memset to zero when writing WLAN manual system file.
                   Precautionary step to prevent validation errors when new
                   fields are added, but not initialized.
06/07/07   rp      Deleting and adding orig_q entry to make sure it is inserted
                   with the new priority.
06/05/07   pk      Added support to read mob_cai_rev from NV
04/17/07   rp      Setting  srv_domain_pref based on call_type for incoming call
04/03/07   sk      Added code to free a client object and remove it from
                   client list upon receiving the CM_PH_CMD_CLIENT_RELEASE
                   command.
03/09/07   sk      RAM usage reduction.
02/27/07   ic      Lint fixes
02/26/07   ka      Changes to support Voip deregistration.
02/22/07   pk      C-GPS changes
02/12/07   pk      Honouring phone's band preference for HDR systems
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
01/04/06   ic      Added missing else in ALS code for processing of
                   CM_PH_CMD_SUBSCRIPTION_AVAILABLE and
                   CM_PH_CMD_SUBSCRIPTION_CHANGED
                   Added messages for CM_PH_CMD_SUBSCRIPTION_* processing.
12/28/06   ka      Fix to ensure that NV srv domain does not change on
                   manual PLMN selection.
                   Adding persistent enums to acq order, band, roam, hyb pref
                   and srv domain enums.
11/22/06   sk      Added missing band read to WLAN manual system read.
11/17/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/11/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/09/06   rp      Added code to reject PS Attach if SIM state is invalid.
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
11/07/06   sk      Memory reduction effort.
08/29/06   sk      Added WLAN Adhoc support.
08/16/06   ka/rp   Rejecting RESET ACM and SET ACMax commands when Advice of
                   charge (aoc_info) is not ready.
08/16/06   ic      Lint fixes
08/11/06   pk      Added support for UOOS
08/08/06   sk      Updated cmph_map_cm_mode_pref_to_sd_mode_pref() and
                   cmph_map_sd_mode_pref_to_cm_mode_pref().
07/19/06   ic      Lint fixes
07/07/06   pk      If a PD session is ended while changing the operating mode
                   to ONLINE, the operating mode change command is put on
                   hold for atmost a second, or PD handshake is received
07/03/06   ic      In cmph_map_mode_pref_nv2cm() added handling for NV enum
                   NV_MODE_WLAN_ONLY
06/12/06   jqi     Kept the emerg orig mode only if there is a active emerg
                   call in cmph_force_pref_on_the_fly
06/09/06   jqi     Only cleared the GW network list pending flag if SD stops
                   manual list search.
06/05/06   ka/ic   Ignoring LPM_OK report received from MMOC when CM is not
                   waiting on it.
05/26/06   pk      Modified the pref_term for incoming to
                   CM_PREF_TERM_CM_1_CALL_PLUS
05/24/06   pk      On LPM -> Online transistion subscription changed command
                   is processed through cmpref_proc_cmd_subsc_chgd, instead
                   of directly calling MMOC API
05/17/06   pk      Added support for DDTM during BCMCS flows
05/03/06   pk      Lint Fixes
04/06/06   sk      Fixed PS_DETACH and PS_ATTACH handling when in powered up
                   in PS_ONLY mode with a voice call and data call up.
04/06/06   sk      Added cmph_map_sd_ss_type_to_cm_ss_type()
04/01/06   ka      Changes for emerg serv category during orig, aborting
                   manual search and returning current band with srv ind.
03/27/06   pk      Added support for lowering the priority of DATA calls if
                   there is a user phone activity, for e.g: sys_sel_pref
                   ps_detach etc.
03/10/06   pk      Added support for high PSIST
03/09/06   pk      Added support for POWER COLLAPSE
03/05/06   jqi     Merged the WCDMA BC3,4,8,9 support.
03/01/06   pk      Corrected cmph_cmp_act_priorities
02/17/06   ka      Adding support for videoshare
01/26/06   ka      Sending CM_PH_EVENT_AVAILABLE_NETWORKS_CONF without
                   checking for Get net pending flag
01/18/06   pk      Initializing all the DDTM srv opt list to zero before
                   writing to NV
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/10/06   pk      Modified cmph_force_pref_on_the_fly(), to update the WLAN
                   Queue after the user's sys sel pref is forced
01/06/06   ka      Reading NV_GPRS_ANITE_GCF_I and storing the value.
01/06/06   ic      Lint cleanup
12/14/05   ic      Changed featurization for cmph_otasp_update_mode() so it
                   links for JCDMA builds
12/13/05   ic      Restored cmph_otasp_update_mode()
12/11/05   ic      Lint cleanup
11/30/05   ka      PS srv component should be added to srv_domain_pref when
                   Manual PLMN request is made during a ps call.
11/30/05   pk      Fixed NV setting for WLAN network_sel_mode_pref change
                   from manual to automatic
11/30/05   ka      Resetting network_sel_mode_pref in cmd_ptr for HPLMN SRCH
                   REQ so that it does not change ph object's net_sel_mode_pref.
                   Resetting pref_term to NONE for HPLMN srch request.
10/04/05   ka      Changes to support on demand HPLMN search request.
11/21/05   pk      Corrected printing of F3 messages in
                   cmph_force_pref_on_the_fly
11/21/05   ic      Cleaned up cmph_offline()
11/10/05   pk      Added support for privacy_pref, and is_privacy in the
                   phone object
11/08/05   sk      Separated CM reports and commands.
11/07/05   jqi     Jump start HDR instance when we are in HICPS on JCDMA
                   build.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
11/01/05   ka      Helper functions to check if nam change is pending for
                   OTASP call.
10/25/05   pk      Fixed RVCT compiler warnings and added mapping functions
10/24/05   pk      Modified cmph_mo_sms_end to drop the priority to the
                   least after sms end
10/20/05   pk      Cleaned RVCT compiler warnings and added mapping functions
10/18/05   pk      Added cmph_map_sd_hybr_pref_to_cm_hybr_pref. As per the
                   new design, we no longer hold the call is a PD session
                   is ended because of a call origination. So we pass NULL
                   as the last parameter to cmpd_call_start()
10/13/05   ic      Mainlined FEATURE_HDR_HICPS20 and FEATURE_HDR_HICPS30
10/04/05   pk      Added message when we don't force preferences because of
                   a high priority activity going on.
10/03/05   pk      Added support for DDTM settings NV item
09/28/05   pk      Added support for CM_ACT_TYPE_NON_STD_OTASP
09/26/05   dk      Do not re-write WLAN Scan pref while reading Manual System
                   file for WLAN. Reported in CR 77056.
09/14/05   pk      Modifications to allow sys_sel_pref during call activity
09/15/05   pk      Added F3 message in cmph_orig_mode_reset_on_ss()
09/12/05   pk      Added checks for Dormant PD session for SYS_SEL_PREF
                   command, modified cmph_call_start to abort PD session
                   if required. Added cmph_pd_session_end(),
                   cmph_pd_session_start()
08/31/05   pk      Added support for priority Queue for the following call
                   types CM_CALL_TYPE_TEST, CM_CALL_TYPE_PD,
                   CM_CALL_TYPE_VT, CM_CALL_TYPE_VT_LOOPBACK,
                   CM_CALL_TYPE_SUPS
08/25/05   ic      Added FEATURE_HDR featurization to cmph_send_ddtm_status()
                   and cmph_ddtm_disable_cdma_tx()
08/23/05   ic      Lint fixes in cmph_force_pref_on_the_fly()
08/22/05   pk      Modified cmph_get_act_priority() to return the correct
                   priority for act_type CM_ACT_TYPE_PH_OBJ
08/15/05   pk      Added support for priority Queues for origination
                   parameters
08/15/05   pk      Moved is_hdr_kicked to a local function. Clear the
                   is_hdr_kicked flag on getting service indication
08/11/05   ka      ACM value maintained in ph cannot grow over 3 bytes, adding
                   the check for it.
08/02/05   ic      Print value that caused error in default switch case
07/21/05   sk      Set orig mode to SD_SS_ORIG_MODE_NORMAL for emergency calls
                   in JCDMA targets.
07/06/05   ka      Correcting the way protocols are being recorded during
                   subsc changed/subs avail and subs not available functions.
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/21/05   dk      Allow WLAN GetNetworks while in Voice call on 1X or GW.
06/15/05   ic      Added comment in cmph_init() on why CM defaults RTRE settings
                   instead of reading them from the NV
06/09/05   dk      Fixed some function prototypes to take void.
06/06/05   pk      Added access overlaod class information to the phone
                   object. Resetting origination throttle table on
                   Operating mode change, subscription change, NAM selection.
06/03/05   ic      Removed FEATURE_CM_INIT_PS_SERVICE_DOMAIN
                   Mainlined FEATURE_FACTORY_TESTMODE
05/11/05   ic      Moved cmph_net_list_copy() to cm.h/.c
05/04/05   jqi     Fixed for hybr data switch over.
04/14/05   dk      Fixed SysSelPref command processing where WLAN Manual
                   preferences were not being loaded correctly if user
                   indicated no change in WLAN network selection mode
                   preference and WLAN network selection mode preference is
                   Manual.
04/08/05   pk      Removed cmph_get_cur_hybr_status(void),
                   cmpref_is_hybr_status_on() is the correct function.
                   Modified handling of CM_ACQ_FAIL_F reports
04/05/05   pk      Modified cmph_info_get() to copy the current DDTM status
04/05/05   dk      Fixed Nam Sel issue to select correct nam while forwarding
                   command to MMOC.
03/25/05   dk      Fixed WLAN read/write profile by checking for NET_STATUS
                   explicitly.
03/14/05   ka      Adding support for SD_SS_HYBR_WLAN instance at required
                   places
03/09/05   pk      Added the CM_PH_EVENT_DDTM_STATUS event, it is triggered
                   when the DDTM status changes.
03/04/05   ic      Merged in DataOverSignaling (DOS) support
03/03/05   ka      Adding support to let mmoc know of protocol that has a
                   subs change when calling mmoc_cmd_subscription_chg
02/24/05   ic      In cmph_force_pref_on_the_fly() overwrite function parameters
                   to keep current preferences if we're in emergency callback
                   mode
02/23/05   pk      Turned off the assertions in cmph_call_end() to reproduce
                   CR#53684
02/17/05   pk      Modified cmph_map_sys_sys_mode_to_sd_mode() to support GW mode
02/09/10   dk      Expanded cm_band_pref to 64 bit.
02/01/05   dk      Added WLAN Support.
01/24/05   ic      Exit emergency callback mode upon emergency call origination
                   and send CM_PH_EVENT_SYS_SEL_PREF to clients.
                   Added cmph_enter_emergency_cb() and cmph_exit_emergency_cb()
                   Replaced cmph_is_ph_in_emerg_cb_mode() with
                   cmph_is_in_emergency_cb().
                   Cleaned up cmph_call_start() to send CM_PH_EVENT_IN_USE_STATE
                   only when phone state changes (it used to send it always).
01/17/05   dk      Added support to read unpacked User PLMN Lists.
01/14/05   sj      Fixed issue with powerup in hdr only mode.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
12/06/04   sj      Removed writing hybr_off during pwrup if mode_pref
                   doesn't have CDMA-HDR.
11/11/04   sj      Rearranged featurization for DDTM_CNTL and fixed issue
                   in cmph_call_end_req() which generates bogus IN_USE event.
11/16/04   sj      For DBM use preference as CM_MODE_PREF_CDMA_ONLY.
11/16/04   sj      Updated avoid_type mapping to SD types.
11/15/04   ic      Lint cleanup with CM_DEBUG on
10/22/04   ic      Featurization changes so it compiles without FEATURE_GPSONE
10/04/04   jqi     Valid band and mode againt both target and PRL for task
                   initialization and system pref change command.
09/23/04   jqi     Do not reset FTM flag when power off the phone.
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/10/04   jqi     Any systerm preference change or operation mode change
                   should terminate a pending get network request.
09/10/04   dk      Allow PLMN search request even when previous one is pending.
                   This is requried to fix CR 48462.
09/08/04   jqi     More ps dormant optimization.
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
09/03/04   dk      Added support to start PS_SIG_DOWN wait tiemr for online
                   mode transition and system selection preference change.
08/30/04   sj      Made orig_mode as an array. Added get/update orig mode funcs.
08/27/04   jqi     Added ps dormant optimization.
08/25/04   ws      Stay in Emerg_cb if already there.
08/20/04   sj      Added support for PRL Preference.
08/16/04   ic      Removed some garbage invisible characters that made
                   compilation break with cmph.c(517) : error C2018: unknown
                   character '0x10'; unicode identifiers are not supported
08/12/04   ws      Remove cm20 support.
08/10/04   ic      Removed cmlog_get_operating_mode() as it became obsolete.
08/06/04   ic      Fixed compiler error from SNAR build
                   (undeclared name, inventing 'extern int gsdi_card_apps_T)
08/04/04   sj      From LPM -> online, generate subscription available
                   command to MMOC.
                   Fixed issue with setting user_resel when network_sel_mode
                   is given as persistent.
08/04/04   ic      Removed enabling/disabling of Lint error 788 as it is
                   globally suppressed in Lint config files.
                   Enabling it locally in CM files caused inconsistent
                   Lint issues reports when a file was linted separately
                   versus when it was linted as part of *.c batch
08/04/04   ic      Updated call to cm_aoc_save_acm()
08/04/04   ic      Removed cmph_stop_mode_req() as it became obsolete
                   Fixed missed issues with existing Lint suppression.
08/04/04   ic      CM Lint clean up - cmph.c with Lint supression verified and
                   free of Lint issues in all CUTE configurations.
07/29/04   sj      Issue subsc_chgd command only when transitioning from FTM
                   to online.
07/29/04   ws      Do not overwrite emerg_orig in orig_mode when originating
                   another data call.
07/27/04   ws      Fix lint error caused by additional SUPS Call Control support.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/12/04   ic      Added support for FEATURE_ALS
                   Removed code compiled under FEATURE_HDR_TEMP
07/09/04   sj      Unforce orig pref. if temporary upon getting get_networks
                   request.
07/07/04   ka      Fixed Lint errors
07/07/04   ic      Fixed compiler warning ('statement unreachable')
07/02/04   sj      Fixed lint errors.
06/30/04   ic      Fixed obvious RVCT 2.1 compiler warnings (Jaguar build)
06/30/04   dk      Added support for PS Signaling down indication flag and uptime.
06/29/04   jqi     Fixed the manual network selection reset issue after reset.
06/25/04   dk      Allow the Network Test Mode request any time even if a call
                   is in progress. This is a requirement for the CR 43966.
06/25/04   dk      Changed reference from cmxcall_is_hybr_hdr_call() to
                   cmcall_is_hybr_hdr_call().
06/22/04   ic      Fixed obvious RVCT 2.1 compiler warnings
06/21/04   ic      Fixed some Lint errors.
                   Updated calls to cmxcall_is_hybr_hdr_call()
06/16/04   ws      Fix featurization with mmgsdi.
06/11/04   ws      Read spc during subscription/rtre config changes.
06/10/04   dk      Added cmph_call_end_req function. Also modified cmph_event
                   function to set the "in_use" flag based on the fact that
                   each call is in IDLE.
06/08/04   jqi     Upon unforce pref on the fly, set orig_mode to manual if
                   current network selection type is manual in phone object.
06/07/04   sj      Removed feature_reg_deep_sleep and functions for standby_sleep
                   & standby wakeup.
06/03/04   ka      Handling PS_ATTACH/PS_DETACH by writing to phone object
05/27/04   ic      In cmph_client_cmd_proc() call cmph_client_cmd_err()
                   irrespective of whether error occured or not (client is
                   supposed to be always notified about command processing
                   status)
05/19/04   ka      Removed unforcing system preference at end of Emergency call by
                   CM_CS_SIG_REL_IND and cmph_process_sig_rel_ind ()
05/17/04   ic      Fixed cmph_ph_cmd_sys_sel_pref_para_check() so it supports
                   1X+HDR+GSM build
05/14/04   ka      Fixed Lint errors.
05/06/04   ic      Added assertions on ph_ptr->is_in_use in cmph_call_end()
                   Added debug info for favored mode for origination computation /
                   forcing of origination mode.
04/30/04   ws      Fix CM/SD mapping functions to include more enums
04/29/04   ic      In cmph_map_cm_mode_pref_to_sd_mode_pref() added support for
                   CM_MODE_PREF_CDMA_HDR_ONLY and CM_MODE_PREF_CDMA_AMPS_HDR_ONLY,
                   removed CM_MODE_PREF_ANY_BUT_HDR_AND_AMPS.
                   In cmph_map_sd_mode_pref_to_cm_mode_pref() added
                   SD_SS_MODE_PREF_CDMA_HDR.
                   Added support for CM_MODE_PREF_CURRENT_LESS_AMPS
04/23/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.28 and MSMSHARED_CM.01.00.10.00.31
04/21/04   ka      Removed CDMA featurization in cmph_timer_proc for sending the
                   right preferences to SD on call end
04/14/04   ka      Added cmph_process_sig_rel_ind () to release signaling on
                   MM REJECT CAUSE.
04/13/04   ka      SD is informed after last call gets ended.
                   Moved cmph_unforce_ functions out of CDMA featurization.
04/12/04   sj      Added support for setting DDTM on while doing GPS fix.
04/12/04   jai     Make phone in use flag in dependent of signal release
                   /indicator.
04/08/04   ws      Merged changes from MSMSHARED_CM.01.00.32.00.03
04/02/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.23 and MSMSHARED_CM.01.00.10.00.28
03/24/04   ic      Featurization changes to support multitarget unit test compile.
                   cmph_orig_mode_reset() cleanup.
                   Fixed compiler warnings.
                   In cmph_init_orig_mode_vars() added resetting of
                   orig_srv_domain_pref.
03/23/04   ws      Merged changes from 6300.
03/18/04   sj      Added is_hybr_hdr_orig() and do not unforce pref if
                   SMS/DBM is in progress.
03/17/04   sj      Added support for ignoring user specified pages while in
                   DDTM.
03/16/04   ka      Offline signal received from tmc is used to call
                   cmlog_set_cm_offline_signal_recvd () to set phone's operating mode.
03/15/04   ka      Merged changes from 6250. Fixed Lint errors.
03/10/04   ic      Initialize rtre_config field in phone object in cmph_init()
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/18/04   ws      Added hdr & amps when checking call end to write db_conv
02/12/04   ic      Merged in changes from MSMSHARED_CM.01.00.10.00.20 to
                   MSMSHARED_CM.01.00.10.00.23
02/09/04   ic      Changes so the code compiles for unit test.
02/06/04   ws      Initial jaguar/mainline merge.
01/21/04   RI      Modified cmph_force_pref_on_the_fly() to include service
                   domain preference parameter.
01/19/04   ws      Send sys_pref_change if RelA even if the phone is in use.
01/15/04   ws      Use sd_misc_is_target_supp_mode_band instead when validating mode/band.
01/13/04   ka      Merged changes from MSM6200
12/16/03   rc/aaj  Support for FEATURE_HDR_HICPS20
12/15/03   ws      Do not set signaling_conn_rel_req since mode is not known yet.
12/11/03   ws      Maintain prst network sel mode pref when unforcing after a call.
11/25/03   ws      Support FTM or OFFLINE operational mode changes.
11/12/03   ws      Featurize srch_rude_wakeup for targets with CDMA
10/31/03   ws      Remove T_RF_ZRF6200 feature
10/23/03   ws      fix for PD sessions
10/20/03   ws      Dual Slot Support.
10/08/03   ws      Added check for USIM for subscription source
                   Added support for NV_PS_DATA_ORIG_PREF_I
10/06/03   vt      Modified to support common cm.h across all targets.
10/01/03   ws      Merged changes to compile for GW targets.
09/23/03   ws      Merged fix when SIM fails during emergency call
                   do not end the call.
09/19/03   ka      Modified get and set preferred PLMN list from SIM with events
09/18/03   ws      Dual valid card support.
09/16/03   vt      Added preferred slot handling.
09/16/03   ws      Call rude wakeup when powering down.
                   uz_id update
08/28/03   ws      Added access functions for subscription information
08/22/03   vt      Mainlined packet state handling.
08/15/03   ws      Featurized diag_otasp_update
08/14/03   vt      Redid the changes for "wakeup from power save".
08/13/03   ws      Undid the changes for "wakeup from power save".
08/08/03   vt      Made cm band pref bitmask to 32 bit enum
07/14/03   jqi     Added Extended PRL support
                   -Passing down band pref to stack for get nw sel cmd.
07/03/03   prk     Modified processing of CM_PH_CMD_GET_NETWORKS for multi-mode
                   targets.
06/26/03   ws      Check for invalid band_pref and write a valid one if necessary.
06/19/03   vt      Generate event to let clients know everytime we exit
                   emergency callback mode.
06/10/03   vt      Unwind any relaxation of mode preference for Emergency calls.
                   Removed changing auto to digital only for non-amps build.
06/03/03   wli     Modified handling of SIM access for RTRE.
06/04/03   ic      Fixed compiler warnings:
                   - removed cmph_foward_client_ftm_cmd() as it became obsolete
05/28/03   vt      Added new call events and changed roam and band enums.
05/22/03   prk     Modified is_valid_oprt_mode_switch() to allow a operating
                   mode transition from FTM to LPM.
05/05/03   prk     Fixed merge problem in cm_ph_increment_segments function
                   definition.
05/02/03   vt      Resetting voice as data fields on call end.
05/02/03   ws      Added FEATURE_UIM_RUIM for calls to diag_otasp_update
04/28/03   AT      Lint cleanup.
04/21/03   vt      Added handling of SIM access for RTRE.
04/14/03   prk     Removed references to FEATURE_AOC and FEATURE_MMGSDI.
                   Removed references to cmph_send_tc_stop_mode_req().
04/14/03   vt      Removed processing of RUIM config on ONLINE mode change.
04/07/03   ws      Fixed compiler warnings
03/21/03   hxw     Changed FEATURE_GPSONE_OOS to FEATURE_GPSONE_MSBASED.
03/18/03   sj      Removed sending of activation_req. Would be done in
                   regproxy.
03/12/03   ws      Modified timer proc to only return immediately if in GW
02/07/03   prk     Re-added call to cmph_send_activation_req()
02/28/03   prk     Mainlined FEATURE_MULTIMODE_ARCH.
02/28/03   ws      Featurized cmph_send_stop_mode_req for GW
02/27/03   prk     Removed sending of TMC_ONLINE command in
                   cmph_client_cmd_forward_mmll().  MMOC is now responsible
                   for sending this command.
02/27/03   vt      Properly featurized the ftm_mode nv update.
                   Replaced commented out mode_pref with sys_sel_pref event.
                   Allowed more transitions from FTM mode for GW.
02/27/03   ws      Fixes for MO SDB timeout problems.
02/25/03   RI      Modified processing of CM_PH_CMD_SYS_SEL_PREF to support
                   PS attach/detach on demand.
                   Added cmph_ps_attach_detach_para_check() for PS attach/detach
                   command check.
02/25/03   vt      Fixed a compile problem with HDR. Set the reply object for
                   LPM for 1x only.
02/21/03   sj      Send MMOC subsc_chgd command only in pwrup or online mode.
                   Moved shutdown_wcall_processing to cmregprx.c.
                   Calling new MMOC API to wakeup from power save.
02/21/03   vt      Fixed problems to build for HDR.
02/14/03   jqi     Added support for runtime FTM.
                   Limited it to CDMA only.
02/11/03   prk     Called cmph_shutdown_wcall_processing() if the operating
                   mode is changing to SYS_OPRT_MODE_OFFLINE_CDMA.
02/07/03   vt      Allowed FTM->OFFLINE_CDMA mode transition for GW.
                   Mainlined FEATURE_LPM. Fixed a compile error for GW for FTM.
02/06/03   vt      Replaced ph_ptr->answer_duration with ph_ptr->answer_uptime.
02/06/02   cs      Added valid card info which is used for call origination handling.
02/04/03   ws      Modified cmph_otasp_update_mode to not read from NV.
01/21/03   ic      In cmph_cmd_copy_fields() and cmph_info_get() added copying
                   of ph_ptr->answer_voice and ph_ptr->answer_duration fields.
                   In cmph_timer_proc() replaced answer_uptime field name with
                   answer_duration.
01/20/03   ws      Updated copyright information for 2003
01/20/03   prk     Featured out calls to diag_otasp_update.
01/17/03   ws      Featurized cmph_complete_sys_sel_pref_req and
                   cmph_process_get_networks_req to remove warnings.
12/12/02   vt      Added Manual PLMN fix.
12/10/02   vt      Prohibited transitioning from FTM to ONLINE operating mode.
11/27/02   vt      Fixed bugs in subscription functions for CDMA.
11/26/02   vt      Fixed compile problems with cmph_send_service_req() and
                   cmph_cmd_get_networks_para_check().
11/25/02   vt      Fixed a bug with RTRE when RUIM card is pulled out.
11/22/02   vt      Disabled issueing subscription changed command when in
                   FTM mode during startup.
11/22/02   PRK     Added support for accessing user preferred PLMN list.
11/20/02   vt      Sending NVRUIM event only if proper features are defined.
11/20/02   vt      Moved diag updating from rtre changed to subscription
                   available command.
11/19/02   vt      Added changing RTRE configuration.
                   Removed handling of powerup pref mode nv item.
11/15/02   vt      Properly featurized the packet state.
11/14/02   vt      Merged changes from GW mainline.
                   [ws]Merged in position determination changes.
                   [prk]Added call to cmph_send_activation_req().
                   [prk]Removed call to cmph_send_activation_req() due to issues in
                   the lower layers.
                   [AT]Fixed some error messages, added some F3 messages when
                   we are running out of funds.
                   [YR]Added test control type handling.
                   [PRK]Added FEATURE_REG_DEEP_SLEEP.
                   [CS]Removed static from cmph_send_service_req definition.
10/31/02   vt      Fixed a problem with mode pref change
10/30/02   vt      Fixed compiler errors and warnings.
10/24/02   vt      Merged mm build changes.
10/23/02   vt      Made changes to get GW working.
                   Removed some compiler warnings.
10/22/02   vt      Uncommented code for GPS mode handling.
10/07/02   vt      Fixed a bug to set prst_mode_pref correct on bootup
                   for multimode. FTM changes too.
09/30/02   prk     Removed cmph_map_cm_mode_pref_to_ll.
09/24/02   prk     Sent the CM_PH_EVENT_SUBSCRIPTION_AVAILABLE event after
                   a service request has been made during processing of
                   CM_SIM_AVAILABLE_CNF.
09/24/02   prk     Mainlined code under FEATURE_CM_USE_UMTS_REG_INTERFACE.
                   Send activation request in cmph_init.
09/20/02   vt      Corrected a typo in checking persistent roam pref.
09/16/02   vt      Corrected some typos and compile errors for 1x build.
09/16/02   vt      Added FTM mode handling.
09/13/02   prk     Modified code to convert NV enums to CM enums for
                   NV_NET_SEL_MODE_PREF_I, NV_ACQ_ORDER_PREF_I and
                   NV_SERVICE_DOMAIN_PREF_I NV items.
09/10/02   ws      Added initial cm20 backward compatibility.
09/03/02   prk     Modified cmph_map_cm_mode_pref_acq_order_pref_to_ll to remove
                   compiler warning.
                   Called cm_aoc_init and sent the
                   CM_PH_EVENT_SUBSCRIPTION_AVAILABLE event to the processing
                   of the CM_SIM_AVAILABLE_CNF event so phone initialization is
                   faster.
                   Processed the CM_SIM_NOT_AVAILABLE_CNF event.
08/30/02   vt      RTRE changes & subscription change phone command.
08/10/02   tjw     Modified LPM to OFFLINE handling to go offline instead of poweroff.
08/06/02   HQ      Added FEATURE_CDSMS.
07/30/02   AT      Added MSG_HIGH to help with AOC.
07/22/02   PRK     Added support for manual PLMN selection.
07/24/02   vt      Added handling for wakeup from deep sleep(standby)
07/03/02   vt      Removed procesing in cmph_offline().
07/03/02   vt      Added additional handling of offline operating mode.
06/05/02   PRK     Mainlined FEATURE_CM_USE_NV_PH_PREF.
06/04/02   PRK     Added FEATURE_CM_INIT_PS_SERVICE_DOMAIN.
05/28/02   PRK     Modified is_valid_oprt_mode_switch to process the
                   SYS_OPRT_MODE_ONLINE operating mode. Modified
                   cmph_client_cmd_forward_ll to process the
                   SYS_OPRT_MODE_ONLINE_CDMA operating mode.
                   Added FEATURE_LPM changes.
06/11/02   vt      Added convertion routines to translate enum values between
                   cm and sd for band, roam, & hybr preferences.
06/06/02   sj      Added support for Multi-mode architecture.
04/22/02   vt      Fixed a compile problem with SD1.0
04/12/02   sh      Remove analog from mode pref if FEATURE_ACP is not defined
04/09/02   sh      Added duration in cmph_mo_sms_end to allow caller to specify
                   when to unforce mode pref. Added cmph_select_dbm_mode_pref
04/03/02   RC      Remove validation of AUTO in cmph_is_valid_mode_band_pref.
04/02/02   sh      Discard duplicate oprt cmd and added first_time_camp_on_1x
03/29/02   ic      In cmph_send_ss_pref_cmd() put #ifdef FEATURE_HDR_HYBRID
                   around line
                   mc_ptr->pref_sys_changed.mode_pref    = sd_mode_pref;
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
03/21/02   sh      Update hybrid HDR serving system status if HDR is
                   deactivated due to operating mode change
03/10/02   sh      Removed IDM timer cancellation when hybrid HDR is in service
03/05/02   az      Added function call to read security code from NV upon
                   NAM change
03/03/02   sh      Set hybr pref to off and remove HDR from mode pref
                   in the online command. Simplfy the non-hybrid to hybrid
                   transition using the pref sys change command only
02/20/02   sh      Added data session handoff support for hybrid operation
02/06/02   sh      Added hybrid 3.0 support
01/25/02   sj      Added support for Tiered service - user zone feature.
01/19/02   az      Added a parameter in a call to cmph_force_mode_on_the_fly()
                   and accordingly changed where it is being called.
12/11/01   sh      Added preliminary hybrid operation NV item support
12/08/01   sh      Added preliminary hybrid operation support
05/07/02   PRK     Added processing of CM_PH_CMD_CLIENT_ACT for GSM/WCDMA.
                   Included cmxsms.h instead of cmsms.h
05/05/02   vt      Fixed compile problem with new mm cm.h
05/03/02   PRK     Updated code for WCDMA/GSM NV items.  Introduced
                   FEATURE_CM_USE_NV_PH_PREF.  Updated to support new REG-CM
                   interface.  Introduced FEATURE_CM_USE_UMTS_REG_INTERFACE.
04/26/02   ATM     Added call to cmlog_operating_mode()
04/22/02   ATM     Updating with changes for Advice of Charge.
03/01/02   ATM     Fixed TYPO: UMTS_GSM_ACCESS_ONLY should be
                   GSM_RADIO_ACCESS_ONLY.
02/25/02   PRK     Changed OVERALL_CALL_STATE_IDLE enum value to
                   CM_OVERALL_CALL_STATE_IDLE.
02/06/02   PRK     Added test control feature (FEATURE_TC).
01/28/02   PRK     Updated names of types/enums for PLMN selection/reselection.
                   Featurized future PLMN selection/reselection code with
                   FEATURE_NEW_PLMN and removed "feature" REAL_CODE.
                   Added parameter checking for the subscription (not) available
                   commands.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
12/13/01   ATT     Integrated AOC with GSDI.
11/21/01   PRK     Added REAL_CODE pre-processor "feature" to remove code not
                   needed in initial Surf testing.  Upon completion of Surf
                   testing the code should be restored to its original form.
11/17/01   sh      Added 1xhdr data session handoff support
11/15/01   ATT     Updated AOC feature to save numbers in correct units.
11/12/01   PRK     Removed reference to NV_MODE_NO_RADIO_ACCESS and made changes
                   due to nv_item_type field name changing to pref_plmn.
11/01/01   sh      Renamed cmph_force_to_colloc as cmph_is_colloc_allowed
10/29/01   sh      Added MSM5500 release 2.0 hybrid operation support
10/23/01   VT/IC   Baseline for CCS
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                     Added support for PLMN selection/reselection (FEATURE_PLMN).
                     Re-added Serving System group and moved srv_state, roam_status,
                     rssi from phone group to serving system group (FEATURE_NEWSS).
10/15/01   RI      Added support for Supplementary Services ( GSM/WCDMA ).
10/14/01   RC      More graceful handling of invalid mode preference in
                   map_mode_pref_to_sd_mode_pref().
09/14/01   HD      Added support for poweroff/offline mode change.
08/23/01   HD      NAS header file reorg changes + RSSI and Roaming ind.
08/21/01   ic      Main-lined FEATURE_NEWCM
08/14/01   WLI     With RUIM read SPC status from RUIM instead of from NV.
06/27/01   ych     Removed JCDMA specific code that was initializing srv_state
                   digital in cmph_init( )
05/31/01   RC      Added CM_CALL_TYPE_PD to map_call_type_to_sd_orig_mode()
05/24/01   RC      Added CM_MODE_PREF_CDMA_AMPS_ONLY
05/07/01   HQ      Added #include stdlib.h.
04/24/01   RC      FEATURE_NEWCM support for SD 2.0 related code.
04/13/01   SH      Added Preliminary HDR support
03/27/01   RC      Added SD 2.0 trimode support.
04/27/01   HD      Added changes for WCDMA/GSM mode.
03/08/01   RC      Fixed a problem with operating mode check.
02/16/01   SH      added CM_END_ACK_F
02/06/01   RC      Added support for System Determination 2.0.
01/23/01   ych     Merged JCDMA features.
08/23/00   vt      Added new CM interface changes.
04/07/00   CS      Saved the new NAM in NV when directed by the network.
03/27/00   HD      Added support for MODE_OFFLINE_ANALOG
03/19/00   RY      Removed OTASP_HOLD_CMD_IN_COMMIT processing from
                   CM_PH_CMD_OPRT_MODE in cmph_client_cmd_check() and
                   cm_client_cmd_forward_mc()
03/13/00   RY      Added OTAPA support
12/13/99   IT      Added handling for next data call as data.
12/03/99   VT      Merged TriMode Code.
12/02/99   VT      CR12075 - For OTASP call, auto nam is turned off.
11/04/99   SH      CR11880 - Fixed a problem with NDSS.
10/20/99   HD      Fixed the problem of DATA_ONCE setup not clearing(CR#11848)
09/14/99   SH      Added client ID checking for all client requests
09/14/99   SH      Removed CM_MODE_PREF_ANALOG_PREF
08/31/99   SH      Modified codes to featurize UASMS
06/08/99   SH      Modified codes in cmph_is_valid_pref_mode for
                   digital related modes
06/21/99   SH      added cmph_offline
06/16/99   SH      Added processing for CM_NAM_CHANGED_F
05/31/99   SH      Added CM 2.0 features
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
12/08/98   RC      Changes per second week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/



/****************************************************************************

 Organization of the file:

    The file is divided into multiple sections.
    You can jump from one sections to the other by searching for / followed
    by 2 *'s. The order of objects defined is as follows:

        includes
        forward declarations
        #defines

        functions
        - Internal [Common, 1x only, GW only]
        - External [Common, 1x only, GW only]

****************************************************************************/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "customer.h"  /* Customer configuration file */
#include "cm.h"        /* External interface to cm.c */
#include "comdef.h"    /* Definition for basic types and macros */
#include "rex.h"       /* Interface to rex services */
#include "nv.h"        /* Interface to NV services */
#include "cmph.h"      /* Interface to CM phone object */
#include "cmss.h"      /* Interface to CM serving system object */
#include "cmtask.h"    /* Interface to signal definitions */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmcall.h"    /* Interface to CM call object */
#include "cmlog.h"     /* Interface to diag services */
#include "cmnv.h"      /* Interface to CM NV services */
#include "clk.h"       /* Interface to clock services */
#include "cmxpd.h"     /* Interface to PD object */
#include "cmreply.h"
#include "cmi.h"

#ifdef FEATURE_IP_CALL
#include "cmipappi.h" /* Interface to inform cm ip app about service */
#endif

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#include "cmxsms.h"    /* Interface to SMS object */
#include "cmxdbm.h"    /* Interface to DBM object */
#include "cmxll.h"     /* MC to CM interface*/
#include "cai.h"       /* Over the air messages definitions */
#include "mc.h"        /* Interface to MC Task */
#include "db.h"        /* Interface to database services */

#ifndef FEATURE_CM_MVS_API
#include "voc.h"       /* Interface to vocoder services */
#endif

#include <string.h>    /* Interface to string library */
#include "sd.h"        /* Interface to System Determination services */

#ifdef FEATURE_JCDMA
#include "txc.h"       /* Interface to TXC subtask */
#endif /* FEATURE_JCDMA */
#endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

#if (defined FEATURE_HDR_HYBRID) || (defined FEATURE_CM_SS_MEAS)
#include "hdrmc.h"
#include "cmd.h"
#endif  /* FEATURE_HDR_HYBRID */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* FEATURE_WCDMA, FEATURE_GSM */

#include "mmoc.h"
#include "diag.h"

#include "ftm.h"

#include "cmpref.h"    /* Interface to CM pref module */

#if (defined(FEATURE_ALS) && defined(FEATURE_GSM))
#error code not present
#endif

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#endif

#ifdef FEATURE_HDR_HANDOFF
#error code not present
#endif

#include "sys.h"
#include "sd.h"
#include "net.h"

#ifdef FEATURE_BCMCS
#error code not present
#endif

#if ( defined(FEATURE_UMTS_1X_HANDOVER) || \
      defined(FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X) || \
      defined(FEATURE_IMS_VCC) )
#include "homeri.h"
#endif

#ifdef FEATURE_FULL_TIME_SHDR
#error code not present
#endif

#ifdef FEATURE_DEDICATED_C2K_MODE
#include "hdrsmp.h"
#endif

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
static db_voice_as_data_type cmph_map_voice_answer_cm2db
(
  cm_answer_voice_e_type cm_voice
);

static cm_ph_cmd_err_e_type  cmph_ph_cmd_packet_state_para_check(
  cm_ph_cmd_s_type           *ph_cmd_ptr
);
#endif /*  (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */


static cm_ph_cmd_err_e_type     cmph_cmd_get_networks_para_check(

  cmph_s_type          *ph_ptr,
      /* Pointer to a phone object */

  cm_ph_cmd_s_type           *ph_cmd_ptr
     /* Pointer at phone command component */
);

static cm_ph_cmd_err_e_type     cmph_cmd_terminate_get_networks_para_check(

  const cmph_s_type                *ph_ptr
      /* Pointer to a phone object */
);

static cm_ph_cmd_err_e_type       cmph_cmd_start_wps_para_check(

   const cmph_s_type                *ph_ptr,
      /* Pointer to a phone object */

   const cm_ph_cmd_s_type           *ph_cmd_ptr
     /* Pointer at phone command component */
);

static cm_ph_cmd_err_e_type     cmph_cmd_abort_wps_para_check(

  const cmph_s_type                *ph_ptr
      /* Pointer to a phone object */
);

#if 0
static void cmph_process_sig_rel_ind( void );
#endif



#define CMPH_RESCAN_TIME                4    /* seconds */
    /* Ballpark estimation for the time it takes the phone to do a rescan
    ** and reacquire service once the mode preference is forced */

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#ifndef FEATURE_ACP
static cm_mode_pref_e_type remove_amps_from_cm_mode_pref(

  cm_mode_pref_e_type cm_mode_pref
   /* mode preference */
);
#endif /* FEATURE_ACP */
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

#ifdef FEATURE_FULL_TIME_SHDR
#error code not present
#endif

/* Tunable phone parameters.
*/

/* Defines a datatype that maps nv band pref with cm band pref.
   NV items will always be 16-bit values.
   We intentionally use uint16 instead of nv_band_pref_enum_type
   because we want cm_band_pref and nv_band_pref fields to overlap in memory.
   We ran into problems when MS VC++ compiler expanded nv_band_pref_enum_type
   to 32 bit value.
*/
typedef union {
  //cm_band_pref_e_type     cm_band_pref;              /* CM band preference */
  uint32                  dummy;
  uint16                  nv_band_pref[2]; /* NV band prefrence */
} cm_nv_band_pref_u_type;


/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

static cmnv_data_s_type      cmnv_data_buf;
    /* Static CM NV data buffer that is big enough to hold data
    ** that is associated with CM NV items.
    **
    ** Note that we could simply use nv_item_type, but this would
    ** be a waste of ~300 bytes of RAM */

/*lint -save -e826 */
static nv_item_type  *cmnv_item_ptr = (nv_item_type*) &cmnv_data_buf;
    /* NV Item pointer to do cmnv_write() and cmnv_read(). Note that
    ** this pointer is initialized to point at a RAM buffer of adequate
    ** size to do all CM NV read/writes operations */
/*lint -restore */

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
static boolean otasp_autonam_restore = FALSE;
    /* This variable is used to restore the AUTO NAM for OTASP. AUTO NAM is
    ** disabled for OTASP call. On completion of OTASP call, this flag is
    ** used to determine if the AUTO NAM should be enabled or not. */
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* FEATURE_WCDMA, FEATURE_GSM */

/* Informs SD of no calls being currently being active */
static void  cmph_force_orig_mode_on_the_fly (

  cm_ss_e_type      ss
        /* The ss on which the orig mode is to be unforced */
);

#ifdef FEATURE_FULL_TIME_SHDR
#error code not present
#endif

/**--------------------------------------------------------------------------
** Functions - internal, common
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cmph_misc_intersect_mode_pref

DESCRIPTION
  Computes the intersection of the mode preferences

DEPENDENCIES


RETURN VALUE
Returns the mode preference

SIDE EFFECTS
  none

===========================================================================*/

cm_mode_pref_e_type cmph_misc_intersect_mode_pref(

  cm_srv_type_e_type srv_type,
    /* The srv type requested
    */

  cm_mode_pref_e_type mode_pref
    /* The current mode preference
    */
)
{
  cm_mode_pref_e_type requested_mode_pref = CM_MODE_PREF_NONE;

  switch( srv_type )
  {
    case CM_SRV_TYPE_AMPS:

      requested_mode_pref = CM_MODE_PREF_AMPS_ONLY;
      break;

    case CM_SRV_TYPE_HDR:

      requested_mode_pref = CM_MODE_PREF_HDR_ONLY;
      break;

    case CM_SRV_TYPE_CDMA_SPECIFIC:
    case CM_SRV_TYPE_CDMA_AUTOMATIC:

      requested_mode_pref = CM_MODE_PREF_CDMA_ONLY;
      break;

    case CM_SRV_TYPE_CDMA_HDR:

      requested_mode_pref = CM_MODE_PREF_CDMA_HDR_ONLY;
      break;


    case CM_SRV_TYPE_GSM_WCDMA:

      requested_mode_pref = CM_MODE_PREF_GSM_WCDMA_ONLY;
      break;

    case CM_SRV_TYPE_WCDMA:

      requested_mode_pref = CM_MODE_PREF_WCDMA_ONLY;
      break;

    case CM_SRV_TYPE_GSM:

      requested_mode_pref = CM_MODE_PREF_GSM_ONLY;
      break;

    case CM_SRV_TYPE_WLAN:

      requested_mode_pref = CM_MODE_PREF_WLAN_ONLY;
      break;

      /* This should be never occur
      */
    default:

      /*
      ** 527: Unreachable
      ** This is because CM_SWITCH_ERR() evaluates to error fatal
      ** (i.e exit program)
      ** But for different feature set it may not to, so we want to
      ** supress the warning here
      */
      /*lint -save -e527 */
      requested_mode_pref = CM_MODE_PREF_NONE;
      /*lint -restore */
      break;
  }


  /* Find the intersection and if there is no intersection then
  ** return the requested preference
  */

  if( requested_mode_pref == CM_MODE_PREF_NONE )
  {
    return ( requested_mode_pref );
  }

  mode_pref = cmcall_misc_get_common_mode_pref(
                                         requested_mode_pref, 1, mode_pref );


  return ( mode_pref );

} /* cmph_misc_intersect_mode_pref */

#if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
#endif
/*===========================================================================

FUNCTION cmph_is_wait_on_oprt_mode

DESCRIPTION
  Translates CM enum to NV enum for mode preference

DEPENDENCIES
  none

RETURN VALUE
  NV enum

SIDE EFFECTS
  none

===========================================================================*/
static boolean  cmph_is_wait_on_oprt_mode
(
  cm_cmd_s_type      *cmd_ptr
    /* Ptr to Ph cmd */

)
/*lint -e{818} cmd_ptr can't be const */
{
  #if defined(FEATURE_IP_CALL)

  cmph_s_type              *ph_ptr = cmph_ptr();
    /* Ph ptr */

  cm_ph_cmd_s_type         *ph_cmd_ptr      =   NULL;
    /* Point at phone command component */

  cm_ph_cmd_info_s_type    *cmd_info_ptr    = NULL;
    /* Point at command information component */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ph_cmd_ptr   = PH_CMD_PTR( cmd_ptr );
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If there are ipapps that have to perform
  ** deregistration, wait for them to complete
  */
  cmipapp_send_oprt_mode (cmd_info_ptr->oprt_mode);

  /* Check if this oprt_mode requires a wait for cnf from app
  */
  if (cmipapp_is_wait_on_oprt_mode_cnf (cmd_info_ptr->oprt_mode))
  {
    cm_reply_set( &ph_ptr->reply,
                   cmd_ptr,               /* command waiting on reply */
                   CM_IP_APP_ALLAPPS_OPRT_CNF, /* wait for IPAPP rpt */
                   CMPH_MAX_IPAPP_DEREG_TIME ); /* wait for this long */

    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #else
  SYS_ARG_NOT_USED (cmd_ptr);
  #endif

  return FALSE;

} /* cmph_is_wait_on_oprt_mode */


/*===========================================================================

FUNCTION cmph_map_mode_pref_cm2nv

DESCRIPTION
  Translates CM enum to NV enum for mode preference

DEPENDENCIES
  none

RETURN VALUE
  NV enum

SIDE EFFECTS
  none

===========================================================================*/
nv_mode_enum_type cmph_map_mode_pref_cm2nv( cm_mode_pref_e_type  cm_mode )
{

  switch (cm_mode)
  {
    case CM_MODE_PREF_DIGITAL_ONLY:
      return NV_MODE_DIGITAL_ONLY;

    case CM_MODE_PREF_AMPS_ONLY:
      return NV_MODE_ANALOG_ONLY;

    case CM_MODE_PREF_AUTOMATIC:
      return NV_MODE_AUTOMATIC;

    case CM_MODE_PREF_EMERGENCY:
      return NV_MODE_E911;

    case CM_MODE_PREF_CDMA_ONLY:
      return NV_MODE_CDMA_ONLY;

    case CM_MODE_PREF_HDR_ONLY:
      return NV_MODE_HDR_ONLY;

    case CM_MODE_PREF_CDMA_AMPS_ONLY:
      return NV_MODE_CDMA_AMPS_ONLY;

    case CM_MODE_PREF_GPS_ONLY:
      return NV_MODE_GPS_ONLY;

     case CM_MODE_PREF_GSM_ONLY:
      return NV_MODE_GSM_ONLY;

    case CM_MODE_PREF_WCDMA_ONLY:
      return NV_MODE_WCDMA_ONLY;

    case CM_MODE_PREF_ANY_BUT_HDR:
      return NV_MODE_ANY_BUT_HDR;

    case CM_MODE_PREF_GSM_WCDMA_ONLY:
      return NV_MODE_GSM_WCDMA_ONLY;

    case CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY:
      return NV_MODE_DIGITAL_LESS_HDR_ONLY;

    case CM_MODE_PREF_CDMA_HDR_ONLY:
      return NV_MODE_CDMA_HDR_ONLY;

    case CM_MODE_PREF_CDMA_AMPS_HDR_ONLY:
      return NV_MODE_CDMA_AMPS_HDR_ONLY;

    case CM_MODE_PREF_WLAN_ONLY:
      return NV_MODE_WLAN_ONLY;

    case CM_MODE_PREF_CDMA_WLAN:
      return NV_MODE_CDMA_WLAN_ONLY;

    case CM_MODE_PREF_HDR_WLAN:
      return NV_MODE_HDR_WLAN_ONLY;

    case CM_MODE_PREF_CDMA_HDR_WLAN:
      return NV_MODE_CDMA_HDR_WLAN_ONLY;

    case CM_MODE_PREF_GSM_WLAN:
      return NV_MODE_GSM_WLAN_ONLY;

    case CM_MODE_PREF_WCDMA_WLAN:
      return NV_MODE_WCDMA_WLAN_ONLY;

    case CM_MODE_PREF_GW_WLAN:
      return NV_MODE_GW_WLAN_ONLY;

    case CM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY:
      return NV_MODE_CDMA_AMPS_HDR_WLAN_ONLY;

    case CM_MODE_PREF_CDMA_AMPS_WLAN_ONLY:
      return NV_MODE_CDMA_AMPS_WLAN_ONLY;

    case CM_MODE_PREF_ANY_BUT_HDR_WLAN:
      return NV_MODE_ANY_BUT_HDR_WLAN;

    default:
      CM_ERR("Invalid MODE PREF %d, return AUTO",cm_mode,0,0);
      return NV_MODE_AUTOMATIC;
  }
} /* cmph_map_mode_pref_cm2nv() */



/*===========================================================================

FUNCTION cmph_map_mode_pref_nv2cm

DESCRIPTION
  Translates NV enum to CM enum for mode preference

DEPENDENCIES
  none

RETURN VALUE
  CM enum

SIDE EFFECTS
  none

===========================================================================*/
static cm_mode_pref_e_type cmph_map_mode_pref_nv2cm( nv_mode_enum_type  nv_mode )
{

  switch (nv_mode)
  {
    case NV_MODE_DIGITAL_ONLY:
      return CM_MODE_PREF_DIGITAL_ONLY;

    case NV_MODE_ANALOG_ONLY:
      return CM_MODE_PREF_AMPS_ONLY;

    case NV_MODE_AUTOMATIC:
      return CM_MODE_PREF_AUTOMATIC;

    case NV_MODE_E911:
      return CM_MODE_PREF_EMERGENCY;

    case NV_MODE_CDMA_ONLY:
      return CM_MODE_PREF_CDMA_ONLY;

    case NV_MODE_HDR_ONLY:
      return CM_MODE_PREF_HDR_ONLY;

    case NV_MODE_CDMA_AMPS_ONLY:
      return CM_MODE_PREF_CDMA_AMPS_ONLY;

    case NV_MODE_GPS_ONLY:
      return CM_MODE_PREF_GPS_ONLY;

    case NV_MODE_GSM_ONLY:
      return CM_MODE_PREF_GSM_ONLY;

    case NV_MODE_WCDMA_ONLY:
      return CM_MODE_PREF_WCDMA_ONLY;

    case NV_MODE_ANY_BUT_HDR:
      return CM_MODE_PREF_ANY_BUT_HDR;

    case NV_MODE_GSM_WCDMA_ONLY:
      return CM_MODE_PREF_GSM_WCDMA_ONLY;

    case NV_MODE_DIGITAL_LESS_HDR_ONLY:
      return CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY;

    case NV_MODE_CDMA_HDR_ONLY:
      return CM_MODE_PREF_CDMA_HDR_ONLY;

    case NV_MODE_CDMA_AMPS_HDR_ONLY:
      return CM_MODE_PREF_CDMA_AMPS_HDR_ONLY;

    case NV_MODE_WLAN_ONLY:
      return CM_MODE_PREF_WLAN_ONLY;

    case NV_MODE_CDMA_WLAN_ONLY:
      return CM_MODE_PREF_CDMA_WLAN;

    case NV_MODE_HDR_WLAN_ONLY:
      return CM_MODE_PREF_HDR_WLAN;

    case NV_MODE_CDMA_HDR_WLAN_ONLY:
      return CM_MODE_PREF_CDMA_HDR_WLAN;

    case NV_MODE_GSM_WLAN_ONLY:
      return CM_MODE_PREF_GSM_WLAN;

    case NV_MODE_WCDMA_WLAN_ONLY:
      return CM_MODE_PREF_WCDMA_WLAN;

    case NV_MODE_GW_WLAN_ONLY:
      return CM_MODE_PREF_GW_WLAN;

    case NV_MODE_CDMA_AMPS_HDR_WLAN_ONLY:
      return CM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY;

    case NV_MODE_CDMA_AMPS_WLAN_ONLY:
      return CM_MODE_PREF_CDMA_AMPS_WLAN_ONLY;

    case NV_MODE_ANY_BUT_HDR_WLAN:
      return CM_MODE_PREF_ANY_BUT_HDR_WLAN;

    default:
      CM_ERR("Invalid NV MODE %d, return AUTO",nv_mode,0,0);
      return CM_MODE_PREF_AUTOMATIC;
  }
} /* cmph_map_mode_pref_nv2cm() */


#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_WLAN))
#error code not present
#endif /* ((FEATURE_WCDMA) || (FEATURE_GSM) || (FEATURE_WLAN)) */


#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

/*===========================================================================

FUNCTION cmph_get_is_kicked_hybr_ptr

DESCRIPTION
  Returns the is_kicked_hdr value

DEPENDENCIES
  none

RETURN VALUE
  Returns the is_kicked_hdr value

SIDE EFFECTS
  None

===========================================================================*/

boolean *cmph_get_is_kicked_hybr_ptr()
{

  static boolean is_kicked_hybr = FALSE;
  return &is_kicked_hybr;
}

/*===========================================================================

FUNCTION cmph_hdr_kick_pending

DESCRIPTION
  Returns if hdr kick is pending

DEPENDENCIES
  none

RETURN VALUE
  Returns the is_kicked_hdr value

SIDE EFFECTS
  None

===========================================================================*/

boolean *cmph_hdr_kick_pending()
{
  #if defined( FEATURE_HICPS_STACK_SYNC_UP ) || defined( FEATURE_BSR2)
  static boolean is_kick_hdr_pending = FALSE;
  return &is_kick_hdr_pending;
  #else
  return FALSE;
  #endif
}

/*===========================================================================

FUNCTION cmph_set_nam_chg_pend

DESCRIPTION
  Sets the nam change pending flag in ph object.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  Ph object's field gets changed.

===========================================================================*/
void  cmph_set_nam_chg_pend (

  boolean      nam_chg_pend
    /*  Nam change pending flag */
)
{

  cmph_s_type                *ph_ptr   = cmph_ptr();

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_ptr->is_nam_change_pend_otasp_comm = nam_chg_pend;

  return;

}

/*===========================================================================

FUNCTION cmph_is_nam_chg_pend

DESCRIPTION
  Returns nam change pending flag from ph object.

DEPENDENCIES
  none

RETURN VALUE
  TRUE  - Nam change is pending after receiving an OTASP commit
  FALSE - NAM change is not pending

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmph_is_nam_chg_pend (void)
{

  cmph_s_type                *ph_ptr   = cmph_ptr();

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (ph_ptr->is_nam_change_pend_otasp_comm);

}


/*===========================================================================

FUNCTION cmph_map_call_type_to_sd_orig_mode

DESCRIPTION
  Map CM call-type type to SD origination mode type.

DEPENDENCIES
  none

RETURN VALUE
  The SD origination mode that corresponds to the input CM call-type.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_orig_mode_e_type          cmph_map_cm_call_type_to_sd_orig_mode(

    cm_call_type_e_type         call_type
        /* Call type to map */
)
{
  sd_ss_orig_mode_e_type     orig_mode = SD_SS_ORIG_MODE_NORMAL;
  cmph_s_type                *ph_ptr   = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Map CM call-type to SD origination-mode.*/

  switch( call_type )
  {
    case CM_CALL_TYPE_NONE:

      orig_mode = cmph_map_cm_network_sel_mode_pref_to_orig_mode
                  ( ph_ptr->network_sel_mode_pref );
      break;

    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_CS_DATA:
    case CM_CALL_TYPE_PS_DATA:
    case CM_CALL_TYPE_SMS:
    case CM_CALL_TYPE_PD:
    case CM_CALL_TYPE_TEST:
    case CM_CALL_TYPE_NON_STD_OTASP:
    case CM_CALL_TYPE_VS:

      if( ph_ptr->network_sel_mode_pref  == CM_NETWORK_SEL_MODE_PREF_MANUAL )
      {
        orig_mode = SD_SS_ORIG_MODE_MANUAL_ORIG;
      }
      else
      {
        orig_mode = SD_SS_ORIG_MODE_NORMAL;
      }
      break;

    case CM_CALL_TYPE_STD_OTASP:

      orig_mode = SD_SS_ORIG_MODE_OTASP;
      break;

    case CM_CALL_TYPE_EMERGENCY:

      #ifdef FEATURE_JCDMA
      orig_mode = SD_SS_ORIG_MODE_NORMAL;
      #else
      orig_mode = SD_SS_ORIG_MODE_EMERG_ORIG;
      #endif

      break;

    case CM_CALL_TYPE_OTAPA:

      orig_mode = SD_SS_ORIG_MODE_NORMAL;
      break;

    case CM_CALL_TYPE_SUPS:
    case CM_CALL_TYPE_MAX:
    default:
      CM_CASE_ERR( "unsupported call_type=%d",call_type,0,0 );
      /*Note that unsupported CM origination
      ** call-types are mapped to SD_SS_ORIG_MODE_NORMAL.
      */
      break;
  } /* switch */

  return orig_mode;

} /* map_cm_call_type_to_sd_orig_mode() */


/*===========================================================================

FUNCTION cmph_cm_map_mode_pref_to_sd_mode_pref

DESCRIPTION
  Map CM mode-preference type to SD mode-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The SD mode-preference that corresponds to the input CM mode-preference.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_mode_pref_e_type   cmph_map_cm_mode_pref_to_sd_mode_pref(

    cm_mode_pref_e_type    mode_pref
        /* Mode pref to map */
)
{
  /*
  ** Map CM mode-preference type to SD mode-preference type.
  ** Note that unsupported CM mode-preferences are mapped
  ** to SD_SS_MODE_PREF_NONE.
  */

  switch( mode_pref )
  {
    case CM_MODE_PREF_GSM_WCDMA_ONLY:
      return SD_SS_MODE_PREF_GW;

    case CM_MODE_PREF_AMPS_ONLY:
      return SD_SS_MODE_PREF_AMPS;

    case CM_MODE_PREF_CDMA_ONLY:
      return SD_SS_MODE_PREF_CDMA;

    case CM_MODE_PREF_HDR_ONLY:
      return SD_SS_MODE_PREF_HDR;

    case CM_MODE_PREF_CDMA_HDR_ONLY:
      return SD_SS_MODE_PREF_CDMA_HDR;

    case CM_MODE_PREF_CDMA_AMPS_HDR_ONLY:
      return SD_SS_MODE_PREF_CDMA_AMPS_HDR;

    case CM_MODE_PREF_GSM_ONLY:
      return SD_SS_MODE_PREF_GSM;

    case CM_MODE_PREF_WCDMA_ONLY:
      return SD_SS_MODE_PREF_WCDMA;

    case CM_MODE_PREF_DIGITAL_ONLY:
      return SD_SS_MODE_PREF_DIGITAL;

    case CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY:
      return SD_SS_MODE_PREF_DIGITAL_LESS_HDR;

    case CM_MODE_PREF_CDMA_AMPS_ONLY:
      return SD_SS_MODE_PREF_CDMA_AMPS;

    case CM_MODE_PREF_GPS_ONLY:
      return SD_SS_MODE_PREF_GPS;

    case CM_MODE_PREF_AUTOMATIC:
      return SD_SS_MODE_PREF_ANY;

    case CM_MODE_PREF_EMERGENCY:
    case CM_MODE_PREF_ANY_BUT_HDR:
      return SD_SS_MODE_PREF_ANY_BUT_HDR;

    case CM_MODE_PREF_ANY_BUT_HDR_WLAN:
      return SD_SS_MODE_PREF_ANY_BUT_HDR_WLAN;

    case CM_MODE_PREF_WLAN_ONLY:
      return SD_SS_MODE_PREF_WLAN;

    case CM_MODE_PREF_CDMA_WLAN:
      return SD_SS_MODE_PREF_CDMA_WLAN;

    case CM_MODE_PREF_HDR_WLAN:
      return SD_SS_MODE_PREF_HDR_WLAN;

    case CM_MODE_PREF_CDMA_HDR_WLAN:
      return SD_SS_MODE_PREF_CDMA_HDR_WLAN;

    case CM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY:
      return SD_SS_MODE_PREF_CDMA_AMPS_HDR_WLAN;

    case CM_MODE_PREF_CDMA_AMPS_WLAN_ONLY:
      return SD_SS_MODE_PREF_CDMA_AMPS_WLAN;

    case CM_MODE_PREF_GSM_WLAN:
      return SD_SS_MODE_PREF_GSM_WLAN;

    case CM_MODE_PREF_WCDMA_WLAN:
      return SD_SS_MODE_PREF_WCDMA_WLAN;

    case CM_MODE_PREF_GW_WLAN:
      return SD_SS_MODE_PREF_GW_WLAN;

    case CM_MODE_PREF_NONE:
      return SD_SS_MODE_PREF_NONE;

    case CM_MODE_PREF_CURRENT_LESS_AMPS:
    case CM_MODE_PREF_CURRENT_LESS_HDR:
    case CM_MODE_PREF_CURRENT_LESS_HDR_AND_AMPS:
    case CM_MODE_PREF_CURRENT_PLUS_WLAN:
    case CM_MODE_PREF_CURRENT_LESS_WLAN:
    case CM_MODE_PREF_PERSISTENT:
    case CM_MODE_PREF_NO_CHANGE:
    case CM_MODE_PREF_MAX:
    default:
      CM_ERR( "CM mode_pref %d not mapped to SD mode pref!!!",mode_pref,0,0 );
      CM_ASSERT(FALSE);
      return SD_SS_MODE_PREF_NONE;

  } /* switch */


} /* cmph_map_cm_mode_pref_to_sd_mode_pref() */

/*===========================================================================

FUNCTION cmph_map_sys_sys_mode_to_sd_mode

DESCRIPTION
  Map CM SYS mode type to SD mode type.

DEPENDENCIES
  none

RETURN VALUE
  The SD mode type that corresponds to the input CM SYS mode type or
  SD_MODE_MAX if not matched.

SIDE EFFECTS
  None

===========================================================================*/
sd_mode_e_type cmph_map_sys_sys_mode_to_sd_mode(sys_sys_mode_e_type mode)
{


  switch(mode)
  {

    case SYS_SYS_MODE_NO_SRV: return SD_MODE_INACT;
    case SYS_SYS_MODE_AMPS:   return SD_MODE_AMPS;
    case SYS_SYS_MODE_CDMA:   return SD_MODE_CDMA;
    case SYS_SYS_MODE_GSM:    return SD_MODE_GSM;
    case SYS_SYS_MODE_HDR:    return SD_MODE_HDR;
    case SYS_SYS_MODE_WCDMA:  return SD_MODE_WCDMA;
    case SYS_SYS_MODE_GPS:    return SD_MODE_GPS;
    case SYS_SYS_MODE_GW:     return SD_MODE_GW;

    default:
      CM_ERR("No match! - %d",mode,0,0);
      return SD_MODE_MAX;
  }


}


/*===========================================================================

FUNCTION cmph_map_cm_hybr_pref_to_sd_hybr_pref

DESCRIPTION
  Map CM hybr-preference type to SD hybr-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The SD hybr-preference that corresponds to the input CM hybr-preference.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_hybr_pref_e_type   cmph_map_cm_hybr_pref_to_sd_hybr_pref
(
  cm_hybr_pref_e_type      hybr_pref
    /* hybr pref to map */
)
{
  /* Map CM hybr-preference type to SD hybr-preference type. Note that
  ** unsupported CM hybr-preferences are mapped to SD_SS_HYBR_PREF_NONE.
  */

  switch( hybr_pref )
  {
    case CM_HYBR_PREF_OFF:
      return SD_SS_HYBR_PREF_NONE;

    case CM_HYBR_PREF_CDMA__HDR:
      return SD_SS_HYBR_PREF_CDMA__HDR;

    case CM_HYBR_PREF_CDMA__WCDMA:
       return SD_SS_HYBR_PREF_CDMA__WCDMA;

    case CM_HYBR_PREF_CDMA__HDR_WCDMA:
      return SD_SS_HYBR_PREF_CDMA__HDR_WCDMA;

    default:
      ERR( "cm_hybr_pref %d",hybr_pref,0,0 );
      return SD_SS_HYBR_PREF_NONE;

  } /* switch */

} /* cmph_map_cm_hybr_pref_to_sd_hybr_pref() */


/*===========================================================================

FUNCTION cmph_map_sd_hybr_pref_to_cm_hybr_pref

DESCRIPTION
  Map SD hybr-preference type to CM hybr-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The CM hybr-preference that corresponds to the input SD hybr-preference.

SIDE EFFECTS
  none

===========================================================================*/
cm_hybr_pref_e_type cmph_map_sd_hybr_pref_to_cm_hybr_pref
(
  sd_ss_hybr_pref_e_type       hybr_pref
    /* hybr pref to map */
)
{
  /* Map SD hybr-preference type to CM hybr-preference type. Note that
  ** unsuppor`ted SD hybr-preferences are mapped to CM_HYBR_PREF_NONE.
  */

  switch( hybr_pref )
  {
    case SD_SS_HYBR_PREF_NONE:       return CM_HYBR_PREF_OFF;
    case SD_SS_HYBR_PREF_CDMA__HDR:   return CM_HYBR_PREF_CDMA__HDR;
    case SD_SS_HYBR_PREF_CDMA__WCDMA: return CM_HYBR_PREF_CDMA__WCDMA;
    case SD_SS_HYBR_PREF_CDMA__HDR_WCDMA: return CM_HYBR_PREF_CDMA__HDR_WCDMA;

    default:
      ERR( "sd_hybr_pref",hybr_pref,0,0 );
      return CM_HYBR_PREF_NONE;

  } /* switch */


} /* cmph_map_sd_hybr_pref_to_cm_hybr_pref() */


/*===========================================================================

FUNCTION cmph_map_otasp_act_code_to_sd_band

DESCRIPTION
  Map CM otasp activation code to SD band.

DEPENDENCIES
  None

RETURN VALUE
  The SD band that corresponds to the input CM activation code.

SIDE EFFECTS
  None

===========================================================================*/
sd_band_e_type   cmph_map_otasp_act_code_to_sd_band(

    cm_activate_code_e_type   otasp_act_code
        /* OTASP call activation code */
)
{
  /* Map CM OTASP activation code to SD band.
  */

  switch( otasp_act_code )
  {
  #ifdef FEATURE_OTASP
    case CM_ABAND:
    case CM_BBAND:
      return SD_BAND_CELL;

    case CM_ABLOCK:
    case CM_BBLOCK:
    case CM_CBLOCK:
    case CM_DBLOCK:
    case CM_EBLOCK:
    case CM_FBLOCK:
    case CM_PREFERRED:
      return SD_BAND_PCS;

    case CM_ACTIVATE_CODE_BC4_ABAND:
    case CM_ACTIVATE_CODE_BC4_BBAND:
    case CM_ACTIVATE_CODE_BC4_CBAND:
      return SD_BAND_BC4;

    case CM_ACTIVATE_CODE_BC5_ABAND:
    case CM_ACTIVATE_CODE_BC5_BBAND:
    case CM_ACTIVATE_CODE_BC5_CBAND:
    case CM_ACTIVATE_CODE_BC5_DBAND:
    case CM_ACTIVATE_CODE_BC5_EBAND:
    case CM_ACTIVATE_CODE_BC5_FBAND:
    case CM_ACTIVATE_CODE_BC5_GBAND:
    case CM_ACTIVATE_CODE_BC5_HBAND:
      return SD_BAND_BC5;

    case CM_ACTIVATE_CODE_BC6:
      return SD_BAND_BC6;

    case CM_ACTIVATE_CODE_BC7_CBAND:
      return SD_BAND_BC7;

    case CM_ACTIVATE_CODE_BC8:
      return SD_BAND_BC8;

    case CM_ACTIVATE_CODE_BC9:
      return SD_BAND_BC9;

    case CM_ACTIVATE_CODE_BC10_ABAND:
    case CM_ACTIVATE_CODE_BC10_BBAND:
    case CM_ACTIVATE_CODE_BC10_CBAND:
    case CM_ACTIVATE_CODE_BC10_DBAND:
    case CM_ACTIVATE_CODE_BC10_EBAND:
      return SD_BAND_BC10;

    case CM_ACTIVATE_CODE_BC11_ABAND:
    case CM_ACTIVATE_CODE_BC11_BBAND:
    case CM_ACTIVATE_CODE_BC11_CBAND:
    case CM_ACTIVATE_CODE_BC11_DBAND:
    case CM_ACTIVATE_CODE_BC11_EBAND:
    case CM_ACTIVATE_CODE_BC11_FBAND:
      return SD_BAND_BC11;

    case CM_ACTIVATE_CODE_BC12_ABAND:
    case CM_ACTIVATE_CODE_BC12_BBAND:
      return SD_BAND_BC12;

    case CM_ACTIVATE_CODE_BC14_GBLOCK:
      return SD_BAND_BC14;

    case CM_ACTIVATE_CODE_BC15_ABAND:
    case CM_ACTIVATE_CODE_BC15_BBAND:
    case CM_ACTIVATE_CODE_BC15_CBAND:
    case CM_ACTIVATE_CODE_BC15_DBAND:
    case CM_ACTIVATE_CODE_BC15_EBAND:
    case CM_ACTIVATE_CODE_BC15_FBAND:
      return SD_BAND_BC15;

    case CM_ACTIVATE_CODE_BC16_ABAND:
    case CM_ACTIVATE_CODE_BC16_BBAND:
    case CM_ACTIVATE_CODE_BC16_CBAND:
    case CM_ACTIVATE_CODE_BC16_DBAND:
      return SD_BAND_BC16;

    case CM_ACTIVATE_CODE_BC19_ABAND:
    case CM_ACTIVATE_CODE_BC19_BBAND:
    case CM_ACTIVATE_CODE_BC19_CBAND:
      return SD_BAND_BC19;

    case CM_ACTIVATE_CODE_BC18_DBAND:
    case CM_ACTIVATE_CODE_BC18_PSB_BAND:
      return SD_BAND_BC18;

  #endif /* FEATURE_OTASP */


    case CM_OTASP_ACT_CODE_NONE:
      return SD_BAND_PCS;

    default:
      CM_CASE_ERR( "act_code = %d",otasp_act_code,0,0 );
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      return SD_BAND_PCS;
      /*lint -restore */
  }


} /* cmph_map_otasp_act_code_to_sd_band() */



/*===========================================================================

FUNCTION cmph_map_otasp_act_code_to_sd_blksys

DESCRIPTION
  Map CM otasp activation code to SD PCS-block/Cellular-system.

DEPENDENCIES
  None

RETURN VALUE
  The SD blksys that corresponds to the input CM activation code.

SIDE EFFECTS
  None

===========================================================================*/
sd_blksys_e_type   cmph_map_otasp_act_code_to_sd_blksys(

    cm_activate_code_e_type   otasp_act_code
        /* OTASP call activation code */
)
{
  /* Map CM OTASP activation code to SD band.
  */

  switch( otasp_act_code )
  {
  #ifdef FEATURE_OTASP
    case CM_ABAND:
        return SD_BLKSYS_CELL_A;

    case CM_BBAND:
        return SD_BLKSYS_CELL_B;

    case CM_ABLOCK:
    case CM_ACTIVATE_CODE_BC5_ABAND:
    case CM_ACTIVATE_CODE_BC6:
    case CM_ACTIVATE_CODE_BC8:
    case CM_ACTIVATE_CODE_BC9:
    case CM_ACTIVATE_CODE_BC10_ABAND:
    case CM_ACTIVATE_CODE_BC11_ABAND:
    case CM_ACTIVATE_CODE_BC12_ABAND:
    case CM_ACTIVATE_CODE_BC15_ABAND:
    case CM_ACTIVATE_CODE_BC16_ABAND:
    case CM_ACTIVATE_CODE_BC19_ABAND:
        return SD_BLKSYS_PCS_A;

    case CM_BBLOCK:
    case CM_ACTIVATE_CODE_BC4_BBAND:
    case CM_ACTIVATE_CODE_BC5_BBAND:
    case CM_ACTIVATE_CODE_BC10_BBAND:
    case CM_ACTIVATE_CODE_BC11_BBAND:
    case CM_ACTIVATE_CODE_BC12_BBAND:
    case CM_ACTIVATE_CODE_BC15_BBAND:
    case CM_ACTIVATE_CODE_BC16_BBAND:
    case CM_ACTIVATE_CODE_BC19_BBAND:
        return SD_BLKSYS_PCS_B;

    case CM_CBLOCK:
    case CM_ACTIVATE_CODE_BC4_CBAND:
    case CM_ACTIVATE_CODE_BC5_CBAND:
    case CM_ACTIVATE_CODE_BC7_CBAND:
    case CM_ACTIVATE_CODE_BC10_CBAND:
    case CM_ACTIVATE_CODE_BC11_CBAND:
    case CM_ACTIVATE_CODE_BC15_CBAND:
    case CM_ACTIVATE_CODE_BC16_CBAND:
    case CM_ACTIVATE_CODE_BC19_CBAND:
        return SD_BLKSYS_PCS_C;

    case CM_DBLOCK:
    case CM_ACTIVATE_CODE_BC5_DBAND:
    case CM_ACTIVATE_CODE_BC10_DBAND:
    case CM_ACTIVATE_CODE_BC11_DBAND:
    case CM_ACTIVATE_CODE_BC15_DBAND:
    case CM_ACTIVATE_CODE_BC16_DBAND:
    case CM_ACTIVATE_CODE_BC18_DBAND:
        return SD_BLKSYS_PCS_D;

    case CM_ACTIVATE_CODE_BC18_PSB_BAND:
        return SD_BLKSYS_PSB;
    case CM_EBLOCK:
    case CM_ACTIVATE_CODE_BC5_EBAND:
    case CM_ACTIVATE_CODE_BC10_EBAND:
    case CM_ACTIVATE_CODE_BC11_EBAND:
    case CM_ACTIVATE_CODE_BC15_EBAND:
        return SD_BLKSYS_PCS_E;

    case CM_FBLOCK:
    case CM_ACTIVATE_CODE_BC5_FBAND:
    case CM_ACTIVATE_CODE_BC11_FBAND:
    case CM_ACTIVATE_CODE_BC15_FBAND:
        return SD_BLKSYS_PCS_F;

    case CM_ACTIVATE_CODE_BC5_GBAND:
    case CM_ACTIVATE_CODE_BC14_GBLOCK:
        return SD_BLKSYS_PCS_G;

    case CM_ACTIVATE_CODE_BC5_HBAND:
        return SD_BLKSYS_PCS_H;

    case CM_PREFERRED:   return SD_BLKSYS_PCS_A;
  #endif /* FEATURE_OTASP */


    case CM_OTASP_ACT_CODE_NONE:
      return SD_BLKSYS_PCS_A;

    default:
      CM_CASE_ERR( "act_code = %d",otasp_act_code,0,0 );
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      return SD_BLKSYS_PCS_A;
      /*lint -restore */
  }


} /* cmph_map_otasp_act_code_to_sd_blksys() */



/*===========================================================================

FUNCTION cmph_map_cm_network_sel_mode_pref_to_orig_mode

DESCRIPTION
  This function translates a network selection mode preference to a
  to SD Orig mode.

DEPENDENCIES
  none

RETURN VALUE
  The  SD Orig Mode.

SIDE EFFECTS
  none
===========================================================================*/
sd_ss_orig_mode_e_type  cmph_map_cm_network_sel_mode_pref_to_orig_mode
(
  cm_network_sel_mode_pref_e_type    network_sel_mode_pref
)
{
  sd_ss_orig_mode_e_type  orig_mode;


  switch ( network_sel_mode_pref )
  {

   case CM_NETWORK_SEL_MODE_PREF_AUTOMATIC:
     orig_mode = SD_SS_ORIG_MODE_NONE;
     break;

   case CM_NETWORK_SEL_MODE_PREF_MANUAL:
         orig_mode = SD_SS_ORIG_MODE_MANUAL_IDLE;
     break;

   case CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV:
         orig_mode = SD_SS_ORIG_MODE_EMERG_ORIG;
     break;

   default:
     CM_ERR( "Invalid network sel mode pref %d", network_sel_mode_pref, 0, 0);
     orig_mode = SD_SS_ORIG_MODE_NONE;
     break;
  } /* switch ( network_sel_mode_pref ) */


  return orig_mode;

} /* cmph_map_cm_network_sel_mode_pref_to_orig_mode */



/*===========================================================================

FUNCTION cmph_map_cm_srv_domain_pref_to_sd

DESCRIPTION
  This function translates a service domain preference to a service domain
  type used by the SD20.

DEPENDENCIES
  none

RETURN VALUE
  SD service domain preference.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_srv_domain_pref_e_type cmph_map_cm_srv_domain_pref_to_sd
(
  cm_srv_domain_pref_e_type  srv_domain_pref
)
{
  sd_ss_srv_domain_pref_e_type mapped_service_domain;


  switch ( srv_domain_pref )
  {

   case CM_SRV_DOMAIN_PREF_NONE:
     mapped_service_domain = SD_SS_SRV_DOMAIN_PREF_NONE;
     break;

   case CM_SRV_DOMAIN_PREF_CS_ONLY:
     mapped_service_domain = SD_SS_SRV_DOMAIN_PREF_CS_ONLY;
     break;

   case CM_SRV_DOMAIN_PREF_PS_ONLY:
     mapped_service_domain = SD_SS_SRV_DOMAIN_PREF_PS_ONLY;
     break;

   case CM_SRV_DOMAIN_PREF_CS_PS:
     mapped_service_domain = SD_SS_SRV_DOMAIN_PREF_CS_PS;
     break;

   default:
     CM_ERR( "Invalid service domain %d", srv_domain_pref, 0, 0);
     mapped_service_domain = SD_SS_SRV_DOMAIN_PREF_CS_ONLY;
     break;
  } /* switch ( service_domain_pref ) */


  return mapped_service_domain;

} /* cmph_map_cm_srv_domain_pref_to_sd */



/*===========================================================================

FUNCTION cmph_map_cm_acq_order_pref_to_sd

DESCRIPTION
  This function translates a acquisition order preference to a acq. order
  type used by the SD20.

DEPENDENCIES
  none

RETURN VALUE
  SD acq. order preference.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_acq_order_pref_e_type  cmph_map_cm_acq_order_pref_to_sd
(
  cm_gw_acq_order_pref_e_type   acq_order_pref
)
{
  sd_ss_acq_order_pref_e_type sd_acq_order_pref;


  switch ( acq_order_pref )
  {

    case CM_GW_ACQ_ORDER_PREF_AUTOMATIC:
          sd_acq_order_pref = SD_SS_ACQ_ORDER_PREF_AUTO;
      break;

    case CM_GW_ACQ_ORDER_PREF_WCDMA_GSM:
          sd_acq_order_pref = SD_SS_ACQ_ORDER_PREF_WCDMA_GSM;
          break;

    case CM_GW_ACQ_ORDER_PREF_GSM_WCDMA:
          sd_acq_order_pref = SD_SS_ACQ_ORDER_PREF_GSM_WCDMA;
          break;

   default:
     CM_ERR( "Invalid acq_order_pref %d", acq_order_pref, 0, 0);
         sd_acq_order_pref = SD_SS_ACQ_ORDER_PREF_AUTO;
     break;
  } /* switch ( acq_order_pref ) */


  return sd_acq_order_pref;

} /* cmph_map_cm_acq_order_pref_to_sd */

/*===========================================================================
FUNCTION cmph_map_sd_mode_pref_to_sys_mode_mask

DESCRIPTION
  Map SD mode-preference type to SYS mode-mask type.

DEPENDENCIES
  None

RETURN VALUE
  The SYS mode-mask that corresponds to the input SD mode-preference
  or SYS_SYS_MODE_MASK_NONE if not mapped.

SIDE EFFECTS
  None

===========================================================================*/
static sys_sys_mode_mask_e_type cmph_map_sd_mode_pref_to_sys_mode_mask(

    sd_ss_mode_pref_e_type    mode_pref
        /* Mode pref to map */
)
{
  sys_sys_mode_mask_e_type    mode_capability = SYS_SYS_MODE_MASK_NONE;

  /* Map SD mode-preference type to SYS mode-mask type.
  */

  if(SD_MODE_CONTAIN (mode_pref, SD_SS_MODE_PREF_CDMA))
  {
    mode_capability =(sys_sys_mode_mask_e_type)
    ((uint64)mode_capability | (uint64)SYS_SYS_MODE_MASK_CDMA);
  }

  if(SD_MODE_CONTAIN (mode_pref, SD_SS_MODE_PREF_HDR))
  {
    mode_capability =(sys_sys_mode_mask_e_type)
    ((uint64)mode_capability | (uint64)SYS_SYS_MODE_MASK_HDR);
  }

  if(SD_MODE_CONTAIN (mode_pref, SD_SS_MODE_PREF_WCDMA))
  {
    mode_capability =(sys_sys_mode_mask_e_type)
    ((uint64)mode_capability | (uint64)SYS_SYS_MODE_MASK_WCDMA);
  }

  if(SD_MODE_CONTAIN (mode_pref, SD_SS_MODE_PREF_GSM))
  {
    mode_capability =(sys_sys_mode_mask_e_type)
    ((uint64)mode_capability | (uint64)SYS_SYS_MODE_MASK_GSM);
  }

  if(SD_MODE_CONTAIN (mode_pref, SD_SS_MODE_PREF_WLAN))
  {
    mode_capability =(sys_sys_mode_mask_e_type)
    ((uint64)mode_capability | (uint64)SYS_SYS_MODE_MASK_WLAN);
  }

  return (mode_capability);
}

/*===========================================================================
FUNCTION cmph_map_sd_mode_pref_to_cm_mode_pref

DESCRIPTION
  Map SD mode-preference type to CM mode-preference type.

DEPENDENCIES
  None

RETURN VALUE
  The CM mode-preference that corresponds to the input SD mode-preference
  or CM_MODE_PREF_MAX if not mapped.

SIDE EFFECTS
  None

===========================================================================*/
cm_mode_pref_e_type cmph_map_sd_mode_pref_to_cm_mode_pref(

    sd_ss_mode_pref_e_type    mode_pref
        /* Mode pref to map */
)
{
  /* Map CM mode-preference type to SD mode-preference type. Note that
  ** unsupported CM mode-preferences are mapped to CM_MODE_PREF_MAX.
  */

  switch( mode_pref )
  {
    case SD_SS_MODE_PREF_AMPS:
      return CM_MODE_PREF_AMPS_ONLY;

    case SD_SS_MODE_PREF_CDMA :
      return CM_MODE_PREF_CDMA_ONLY;

    case SD_SS_MODE_PREF_HDR :
      return CM_MODE_PREF_HDR_ONLY;

    case SD_SS_MODE_PREF_GSM :
      return CM_MODE_PREF_GSM_ONLY;

    case SD_SS_MODE_PREF_WCDMA :
      return CM_MODE_PREF_WCDMA_ONLY;

    case SD_SS_MODE_PREF_DIGITAL :
      return CM_MODE_PREF_DIGITAL_ONLY;

    case SD_SS_MODE_PREF_DIGITAL_LESS_HDR:
      return CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY;

    case SD_SS_MODE_PREF_CDMA_AMPS:
      return CM_MODE_PREF_CDMA_AMPS_ONLY;

    case SD_SS_MODE_PREF_GPS :
      return CM_MODE_PREF_GPS_ONLY;

    case SD_SS_MODE_PREF_ANY :
      return CM_MODE_PREF_AUTOMATIC;

    case SD_SS_MODE_PREF_ANY_BUT_HDR:
      return CM_MODE_PREF_ANY_BUT_HDR;

    case SD_SS_MODE_PREF_ANY_BUT_HDR_WLAN:
      return CM_MODE_PREF_ANY_BUT_HDR_WLAN;

    case SD_SS_MODE_PREF_GW:
      return CM_MODE_PREF_GSM_WCDMA_ONLY;

    case SD_SS_MODE_PREF_CDMA_AMPS_HDR:
      return CM_MODE_PREF_CDMA_AMPS_HDR_ONLY;

    case SD_SS_MODE_PREF_NONE:
      return CM_MODE_PREF_NONE;

    case SD_SS_MODE_PREF_CDMA_HDR:
      return CM_MODE_PREF_CDMA_HDR_ONLY;

    case SD_SS_MODE_PREF_WLAN:
      return CM_MODE_PREF_WLAN_ONLY;

    case SD_SS_MODE_PREF_CDMA_WLAN:
      return CM_MODE_PREF_CDMA_WLAN;

    case SD_SS_MODE_PREF_CDMA_HDR_WLAN:
      return CM_MODE_PREF_CDMA_HDR_WLAN;

    case SD_SS_MODE_PREF_HDR_WLAN:
      return CM_MODE_PREF_HDR_WLAN;

    case SD_SS_MODE_PREF_CDMA_AMPS_HDR_WLAN:
       return CM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY;

    case SD_SS_MODE_PREF_CDMA_AMPS_WLAN:
       return CM_MODE_PREF_CDMA_AMPS_WLAN_ONLY;


    case SD_SS_MODE_PREF_GSM_WLAN:
       return CM_MODE_PREF_GSM_WLAN;

    case SD_SS_MODE_PREF_WCDMA_WLAN:
       return CM_MODE_PREF_WCDMA_WLAN;

    case SD_SS_MODE_PREF_GW_WLAN:
       return CM_MODE_PREF_GW_WLAN;

    default:
      CM_ERR( "SD mode_pref not mapped to CM mode pref !!! sd = %0x",mode_pref,0,0 );
      CM_ASSERT(FALSE);
      return CM_MODE_PREF_MAX;

  } /* switch(mode_pref) */

} /* cmph_map_sd_mode_pref_to_cm_mode_pref() */


/*===========================================================================

FUNCTION cmph_map_cm_avoid_type_to_sd_avoid_type

DESCRIPTION
  This function translates a CM avoid type to SD avoid type.

DEPENDENCIES
  none

RETURN VALUE
  The SD Orig Mode.

SIDE EFFECTS
  none
===========================================================================*/
static sd_ss_avoid_sys_e_type    cmph_map_cm_avoid_type_to_sd_avoid_type
(
  cm_ss_avoid_sys_e_type  cm_avoid_type
)
{

  sd_ss_avoid_sys_e_type sd_avoid_type = SD_SS_AVOID_SYS_IDLE;

  switch ( cm_avoid_type )
  {
    #ifdef FEATURE_USERZONE
    case CM_SS_AVOID_SYS_SRV_IF_UZ:
      sd_avoid_type = SD_SS_AVOID_SYS_IDLE_IF_UZ;
      break;
    #endif

    case CM_SS_AVOID_SYS_SRV:
      sd_avoid_type = SD_SS_AVOID_SYS_IDLE;
      break;

    case CM_SS_AVOID_SYS_NONE:
    case CM_SS_AVOID_SYS_MAX:
    default:
     CM_ERR( "Invalid avoid_type %d", cm_avoid_type, 0, 0);
     break;

  } /* switch ( cm_avoid_type ) */


  return sd_avoid_type;

} /* cmph_map_cm_avoid_type_to_sd_avoid_type() */

#ifdef FEATURE_DDTM_CNTL

#if !defined(FEATURE_DDTM_ACCPT_ASYNC_FAX_SO_PAGES) && \
      (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================

@FUNCTION cmph_map_nv_ddtm_act_to_cm_ddtm_act_mask

@DESCRIPTION
  This function Maps the NV DDTM act to DDTM_ACT_MASK

@DEPENDENCIES
  none

@RETURNS
  The DDTM act_mask.

@SIDE_EFFECT
  none

@x
===========================================================================*/
static sys_ddtm_act_mask_e_type cmph_map_nv_ddtm_act_to_cm_ddtm_act_mask(

  nv_ddtm_act ddtm_act
  /* The ddtm_act
  */
)
{
  /*lint -e655 */
  /* 655: bit-wise operation uses (compatible) enum's */

  sys_ddtm_act_mask_e_type sys_ddtm_act=SYS_DDTM_ACT_MASK_EMPTY;

  if( ddtm_act & DDTM_ACT_SUPPRESS_L2ACK )
  {
    sys_ddtm_act |= SYS_DDTM_ACT_MASK_SUPPRESS_L2ACK;
  }

  if( ddtm_act & DDTM_ACT_SUPPRESS_REG )
  {
    sys_ddtm_act |= SYS_DDTM_ACT_MASK_SUPPRESS_REG;
  }

  if( ddtm_act & DDTM_ACT_IGN_SO_PAGES )
  {
    sys_ddtm_act |= SYS_DDTM_ACT_MASK_IGN_SO_PAGES;
  }

  if( ddtm_act & DDTM_ACT_SUPPRESS_MO_DBM )
  {
    sys_ddtm_act |= SYS_DDTM_ACT_MASK_SUPPRESS_MO_DBM;
  }

  if( (ddtm_act == DDTM_ACT_ALL)||(sys_ddtm_act == (SYS_DDTM_ACT_MASK_SUPPRESS_L2ACK |
                                                   SYS_DDTM_ACT_MASK_SUPPRESS_REG |
                                                   SYS_DDTM_ACT_MASK_IGN_SO_PAGES |
                                                   SYS_DDTM_ACT_MASK_SUPPRESS_MO_DBM )))
  {
    sys_ddtm_act = SYS_DDTM_ACT_MASK_ALL;
  }

  /*lint +e655 */

  return (sys_ddtm_act );

} /* cmph_map_nv_ddtm_act_to_cm_ddtm_act_mask */

/*===========================================================================

@FUNCTION cmph_map_nv_ddtm_pref_to_cm_ddtm_pref

@DESCRIPTION
  This function Maps the nv DDTM PREF to cm DDTM PREF

@DEPENDENCIES
  none

@RETURN VALUE
  The NV ddtm_act

@x
===========================================================================*/
static   cm_ddtm_pref_e_type  cmph_map_nv_ddtm_pref_to_cm_ddtm_pref(

  nv_ddtm_pref_type         ddtm_pref
  /* CM ddtm pref
  */
)
{
  switch ( ddtm_pref )
  {
    case DDTM_PREF_OFF:
      return( CM_DDTM_PREF_OFF );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case DDTM_PREF_ON:
    default:
      return( CM_DDTM_PREF_ON );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  }

} /* cmph_map_nv_ddtm_pref_to_cm_ddtm_pref */

#endif


/*===========================================================================

@FUNCTION cmph_map_cm_ddtm_act_mask_to_nv_ddtm_act

@DESCRIPTION
  This function Maps the cm DDTM_ACT_MASK to NV DDTM act

@DEPENDENCIES
  none

@RETURNS
  The NV ddtm_act

@SIDE_EFFECT
  none

@x
===========================================================================*/
static   nv_ddtm_act cmph_map_cm_ddtm_act_mask_to_nv_ddtm_act(

  sys_ddtm_act_mask_e_type ddtm_act_mask
  /* The ddtm_act
  */
)
{
  nv_ddtm_act ddtm_act = DDTM_ACT_EMPTY;

  /*lint -e655 */
  /* 655: bit-wise operation uses (compatible) enum's */

  if( ddtm_act_mask & SYS_DDTM_ACT_MASK_SUPPRESS_L2ACK )
  {
    ddtm_act |= DDTM_ACT_SUPPRESS_L2ACK ;
  }

  if( ddtm_act_mask & SYS_DDTM_ACT_MASK_SUPPRESS_REG )
  {
    ddtm_act |= DDTM_ACT_SUPPRESS_REG;
  }

  if( ddtm_act_mask & SYS_DDTM_ACT_MASK_IGN_SO_PAGES )
  {
    ddtm_act |= DDTM_ACT_IGN_SO_PAGES;
  }

  if( ddtm_act_mask & SYS_DDTM_ACT_MASK_SUPPRESS_MO_DBM )
  {
    ddtm_act |= DDTM_ACT_SUPPRESS_MO_DBM;
  }

  if( (ddtm_act_mask == SYS_DDTM_ACT_MASK_ALL )||(ddtm_act == (DDTM_ACT_SUPPRESS_L2ACK  |
                                                   DDTM_ACT_SUPPRESS_REG |
                                                   DDTM_ACT_SUPPRESS_MO_DBM |
                                                   DDTM_ACT_IGN_SO_PAGES )))
  {
    ddtm_act = DDTM_ACT_ALL;
  }

  /*lint +e655 */

  return (ddtm_act );

 } /* cmph_map_cm_ddtm_act_mask_to_nv_ddtm_act */

/*===========================================================================

@FUNCTION cmph_map_cm_ddtm_pref_to_nv_ddtm_pref

@DESCRIPTION
  This function Maps the cm DDTM PREF to NV DDTM PREF

@DEPENDENCIES
  none

@RETURNS
  The NV ddtm_act

@SIDE_EFFECT
  none

@x
===========================================================================*/
static   nv_ddtm_pref_type cmph_map_cm_ddtm_pref_to_nv_ddtm_pref(

  cm_ddtm_pref_e_type         ddtm_pref
  /* CM ddtm pref
  */
)
{
  switch ( ddtm_pref )
  {
    case CM_DDTM_PREF_OFF:
      return( DDTM_PREF_OFF );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_DDTM_PREF_ON:
    default:
      return( DDTM_PREF_ON );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  }

} /* cmph_map_cm_ddtm_pref_to_nv_ddtm_pref */

#endif

/*===========================================================================

FUNCTION is_valid_oprt_mode_switch

DESCRIPTION
  Check whether a given operating mode switch is legal.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if operating mode switch is legal, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean is_valid_oprt_mode_switch(

    sys_oprt_mode_e_type    from_oprt_mode,
        /* from this operating mode */

    sys_oprt_mode_e_type    to_oprt_mode
        /* to this operating mode */
)
{
  unsigned int i;
      /* loop counter */

  /* Table specifying valid operating mode switches
  */
  static const struct {

      sys_oprt_mode_e_type    from_oprt_mode;
          /* from operating mode */

      sys_oprt_mode_e_type    to_oprt_mode;
          /* to operating mode */

  } oprt_mode_tbl[] = {

        /* from oprt mode */         /* to oprt mode */
      { SYS_OPRT_MODE_PWROFF,        SYS_OPRT_MODE_OFFLINE_CDMA },
      { SYS_OPRT_MODE_PWROFF,        SYS_OPRT_MODE_ONLINE       },
      { SYS_OPRT_MODE_PWROFF,        SYS_OPRT_MODE_PSEUDO_ONLINE},
      { SYS_OPRT_MODE_PWROFF,        SYS_OPRT_MODE_OFFLINE_AMPS },
      { SYS_OPRT_MODE_PWROFF,        SYS_OPRT_MODE_OFFLINE      },
      { SYS_OPRT_MODE_PWROFF,        SYS_OPRT_MODE_LPM          },

      { SYS_OPRT_MODE_OFFLINE_CDMA,  SYS_OPRT_MODE_PWROFF       },
      { SYS_OPRT_MODE_OFFLINE_CDMA,  SYS_OPRT_MODE_RESET        },
      { SYS_OPRT_MODE_OFFLINE_CDMA,  SYS_OPRT_MODE_RESET_MODEM  },
      { SYS_OPRT_MODE_OFFLINE_CDMA,  SYS_OPRT_MODE_OFFLINE_AMPS },
      { SYS_OPRT_MODE_OFFLINE_CDMA,  SYS_OPRT_MODE_OFFLINE      },
 
      { SYS_OPRT_MODE_OFFLINE_AMPS,  SYS_OPRT_MODE_PWROFF       },
      { SYS_OPRT_MODE_OFFLINE_AMPS,  SYS_OPRT_MODE_RESET        },
      { SYS_OPRT_MODE_OFFLINE_AMPS,  SYS_OPRT_MODE_OFFLINE_CDMA },
      { SYS_OPRT_MODE_OFFLINE_AMPS,  SYS_OPRT_MODE_OFFLINE      },

      { SYS_OPRT_MODE_OFFLINE,       SYS_OPRT_MODE_PWROFF       },
      { SYS_OPRT_MODE_OFFLINE,       SYS_OPRT_MODE_RESET        },
      { SYS_OPRT_MODE_OFFLINE,       SYS_OPRT_MODE_RESET_MODEM  },
      { SYS_OPRT_MODE_OFFLINE,       SYS_OPRT_MODE_OFFLINE_CDMA },
      { SYS_OPRT_MODE_OFFLINE,       SYS_OPRT_MODE_OFFLINE_AMPS },

      { SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_OFFLINE_CDMA },
      { SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_OFFLINE_AMPS },
      { SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_OFFLINE      },
      { SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_LPM          },
      { SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_PWROFF       },
      { SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_PSEUDO_ONLINE},
      #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
      #endif /* FEATURE_WCDMA, FEATURE_GSM */

      { SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_FTM          },
      { SYS_OPRT_MODE_FTM,           SYS_OPRT_MODE_ONLINE       },
      { SYS_OPRT_MODE_FTM,           SYS_OPRT_MODE_PSEUDO_ONLINE},
      { SYS_OPRT_MODE_FTM,           SYS_OPRT_MODE_PWROFF       },
      { SYS_OPRT_MODE_FTM,           SYS_OPRT_MODE_OFFLINE_CDMA },
      { SYS_OPRT_MODE_FTM,           SYS_OPRT_MODE_LPM          },
      
      { SYS_OPRT_MODE_LPM,           SYS_OPRT_MODE_OFFLINE_CDMA },
      { SYS_OPRT_MODE_LPM,           SYS_OPRT_MODE_OFFLINE_AMPS },
      { SYS_OPRT_MODE_LPM,           SYS_OPRT_MODE_OFFLINE      },
      { SYS_OPRT_MODE_LPM,           SYS_OPRT_MODE_ONLINE       },
      { SYS_OPRT_MODE_LPM,           SYS_OPRT_MODE_PSEUDO_ONLINE},
      { SYS_OPRT_MODE_LPM,           SYS_OPRT_MODE_PWROFF       },
      

      { SYS_OPRT_MODE_PSEUDO_ONLINE, SYS_OPRT_MODE_OFFLINE_CDMA },
      { SYS_OPRT_MODE_PSEUDO_ONLINE, SYS_OPRT_MODE_OFFLINE_AMPS },
      { SYS_OPRT_MODE_PSEUDO_ONLINE, SYS_OPRT_MODE_OFFLINE      },
      { SYS_OPRT_MODE_PSEUDO_ONLINE, SYS_OPRT_MODE_LPM          },
      { SYS_OPRT_MODE_PSEUDO_ONLINE, SYS_OPRT_MODE_PWROFF       },
      { SYS_OPRT_MODE_PSEUDO_ONLINE, SYS_OPRT_MODE_ONLINE       }
      
  };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "is_valid_oprt_mode_switch()",0,0,0 );
  CM_ASSERT( INRANGE((int)from_oprt_mode,
                     (int)SYS_OPRT_MODE_NONE,
                     (int)(SYS_OPRT_MODE_MAX-1)));
  CM_ASSERT( BETWEEN(to_oprt_mode, SYS_OPRT_MODE_NONE, SYS_OPRT_MODE_MAX) );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Switching into the same operating mode is always valid.
  */
  if( to_oprt_mode == from_oprt_mode )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If receiving the reset modem operation mode request,and the feature 
  ** FEATURE_OPRT_MODE_MODEM_RESET is undefined, return .
  */
  if( to_oprt_mode == SYS_OPRT_MODE_RESET_MODEM )
  {
     #ifndef FEATURE_OPRT_MODE_MODEM_RESET 
     return FALSE;
     #endif
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_CM_DETECT_HW
#error code not present
  #endif /* FEATURE_CM_DETECT_HW */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check for a match in the oprt_mode_tbl to decide
  ** whether this is a valid operating mode switch.
  */
  for( i=0; i < ARR_SIZE(oprt_mode_tbl); i++ )
  {
    if( oprt_mode_tbl[i].from_oprt_mode == from_oprt_mode &&
        oprt_mode_tbl[i].to_oprt_mode   == to_oprt_mode )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, there is no match in the
  ** operating mode table - return FALSE.
  */
  return FALSE;

} /* is_valid_oprt_mode_switch() */



/*===========================================================================

FUNCTION cmph_in_valid_oprt_mode

DESCRIPTION
  This function checks whether a phone command can be accepted based on the
  operating mode.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  = The phone command can be accepted based on the operating mode.
  FALSE = The phone command can not be accepted based on the operating mode.

SIDE EFFECTS
  None
===========================================================================*/
static boolean cmph_in_valid_oprt_mode
(
  const cmph_s_type   *ph_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( ( ph_ptr->oprt_mode == SYS_OPRT_MODE_PWROFF      ) ||
      ( ph_ptr->oprt_mode == SYS_OPRT_MODE_RESET       ) ||
      ( ph_ptr->oprt_mode == SYS_OPRT_MODE_LPM         ) ||
      ( ph_ptr->oprt_mode == SYS_OPRT_MODE_FTM         ) ||
      ( ph_ptr->oprt_mode == SYS_OPRT_MODE_RESET_MODEM ) ||
      ( ph_ptr->oprt_mode == SYS_OPRT_MODE_NET_TEST_GW )    )
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* cmph_in_valid_oprt_mode() */





/*===========================================================================

FUNCTION cmph_activate_ps_service_req_timer

DESCRIPTION
  This function activates the PS service request timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmph_activate_ps_service_req_timer( void )
{

  cmph_s_type  *ph_ptr       = cmph_ptr();

  ph_ptr->ps_service_req_timer_active = TRUE;

  /* The uptime is set to 140 seconds in the future.  This is based on the
  ** worst case scenario - mobility managment (GSM) timers expiring after
  ** 120 seconds.
  */
  ph_ptr->ps_service_req_uptime       = clk_uptime() + 140;

} /* cmph_activate_ps_service_req_timer */



/*===========================================================================

FUNCTION cmph_deactivate_ps_service_req_timer

DESCRIPTION
  This function deactivates the PS service request timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void cmph_deactivate_ps_service_req_timer( void )
{

  cmph_s_type  *ph_ptr       = cmph_ptr();

  ph_ptr->ps_service_req_timer_active = FALSE;
  ph_ptr->ps_service_req_uptime       = 0;

} /* cmph_deactivate_ps_service_req_timer */



/*===========================================================================

FUNCTION cmph_info_alloc

DESCRIPTION
 Allocate a phone info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated phone info buffer.

SIDE EFFECTS
  none

===========================================================================*/
cm_ph_info_s_type  *cmph_info_alloc( void )
{
  static cm_ph_info_s_type   ph_info;
      /* Currently there is only a need for one phone info buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the length of ccbs index store
  */
  ph_info.ccbs_store_info.cm_ccbs_store_info_len = 0;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Simply return a pointer to the statically allocated
  ** phone info buffer.
  */
  return &ph_info;

} /* cmph_info_alloc() */

/*===========================================================================

FUNCTION cmph_event

DESCRIPTION
  Notify the client list of a specified phone event.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmph_event(

    cmph_s_type     *ph_ptr,
        /* pointer to a phone object.
        */

    cm_ph_event_e_type    ph_event
        /* notify client list of this phone event */
)
/*lint -esym(715,ph_ptr) */
{
  cm_ph_info_s_type *ph_info_ptr = cmph_info_alloc();
      /* Allocate phone info buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( BETWEEN(ph_event, CM_PH_EVENT_NONE, CM_PH_EVENT_MAX) );
  CM_ASSERT( ph_info_ptr != NULL );
  CM_FUNC_START( "cmph_event(), Phone event=%d",ph_event,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   CM_MSG_HIGH("CM PH Event: %d",ph_event,0,0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy phone state information into
  ** allocated buffer.
  */
  (void) cmph_info_get( ph_info_ptr );

  /* Update is_in_use_reported as well */
  ph_ptr->is_in_use_reported = ph_ptr->is_in_use;

  /*
  ** If the phone is in the process of releasing the last call
  ** but not done yet, we still want to set the "in_use" flag
  ** to false.
  **
  ** For GSM/WCDMA call, it might take a while before we get
  ** confirmation of call release from network/lower layers.
  **
  ** This delay might cause phone to look busy while from user
  ** perspective, call has been ended.
  **
  */

  if(cmcall_check_each_call_in_call_state(CM_CALL_STATE_IDLE))
  {
    #if( defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE) )
#error code not present
    #endif /* FEATURE_GPSONE */
    {
      ph_info_ptr->is_in_use= FALSE;

      /* Keep track of clients's knowledge of phone's in_use state in ph
      ** obj
      */
      ph_ptr->is_in_use_reported = FALSE;

    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the is_event_process flag to true before sending event to clients.
  */
  ph_ptr->is_event_process = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Notify the client list of the phone event.
  */
  cmclient_list_ph_event_ntfy( ph_event, ph_info_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the is_event_process flag to false after PH events have been sent to
  ** clients.
  */
  ph_ptr->is_event_process = FALSE;

} /* cmph_event() */
/*lint +esym(715,ph_ptr) */

/*===========================================================================

FUNCTION cmph_delete_update_orig_mode

DESCRIPTION
  Delete the origination parameters from the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
static void cmph_delete_update_orig_mode(

  cm_ss_e_type              ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= CM_SS_MAX.
    */

  cm_act_id_type            act_id,
      /* The unique id of the activity */

  boolean                   force_ph_pref_if_top
      /* If the top ptr is the phone ptr force the phone preferences */

)
{

  cm_orig_q_s_type  *ph_orig_ptr;
  /* Search the ptr
  */

  ss = (ss == CM_SS_MAX ? CM_SS_MAIN : ss);

  ph_orig_ptr = cmtask_orig_para_search_act_id(  ss, act_id );

  if( ph_orig_ptr != NULL )
  {
    if( cmtask_orig_para_delete( ss, ph_orig_ptr, TRUE ) )
    {
      ph_orig_ptr = cmtask_orig_para_get_top(ss);
      if( ph_orig_ptr != NULL &&
          ( ph_orig_ptr->orig.act_id != (cm_act_id_type) cmph_ptr() || force_ph_pref_if_top )
        )
      {
        cmph_force_orig_mode_on_the_fly(ss);
      }

    }
  }

  return;

} /* cmph_delete_update_orig_mode */

/*===========================================================================

FUNCTION cmph_get_emergency_cb_pref

DESCRIPTION
  Get a pointer to the emergency_cb preferences

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the strucutre having the emergency call back preferences

SIDE EFFECTS
  None

===========================================================================*/

static cm_act_orig_s_type * cmph_get_emergency_cb_pref()
{

  static cm_act_orig_s_type cmph_emer_cb_pref;

  return &cmph_emer_cb_pref;

}

/*===========================================================================

FUNCTION cmph_is_in_emergency_cb

DESCRIPTION
  Check if the phone is in emergency call back mode.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the phone is in emergency call back mode.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmph_is_in_emergency_cb
(
        void
)
{
  /* Pointer to phone object.
  */
 cmph_s_type *ph_ptr  = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( ph_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if we are in call back mode.
  */

  return( ph_ptr->ph_state == CM_PH_STATE_EMERG_CB );

} /* cmph_is_in_emergency_cb() */


/*===========================================================================

FUNCTION cmph_exit_emergency_cb

DESCRIPTION

  Exit emergency callback mode and notify clients

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void cmph_exit_emergency_cb(

    cmph_s_type             *ph_ptr
      /* Pointer to a phone object */
)
{

  cm_act_orig_s_type *emer_cb_ptr;
  /* The pointer to the element of the priority queue
  */

   cm_orig_q_s_type *ph_orig_ptr;

  CM_ASSERT( ph_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return if not in emergency callback mode
  */
  if(!cmph_is_in_emergency_cb())
  {
    return;
  }


  CM_MSG_MED("Exit emergency callback mode ...",0,0,0);

  emer_cb_ptr = cmph_get_emergency_cb_pref();

  CM_ASSERT( emer_cb_ptr != NULL );

  /* Initialize the emer_cb_ptr with default values
  */

  cmtask_orig_para_init_2( emer_cb_ptr );

  /* Delete the call back entry from the queues
  */

  cmph_delete_update_orig_mode( CM_SS_MAIN, ( cm_act_id_type )emer_cb_ptr, FALSE);
  cmph_delete_update_orig_mode( CM_SS_HDR,  ( cm_act_id_type )emer_cb_ptr, FALSE);
  cmph_delete_update_orig_mode( CM_SS_WLAN, ( cm_act_id_type )emer_cb_ptr, FALSE);


  /* Go through the orig q for MAIN and get a entry with the call_type
  ** emergency
  */

  /* If entry is found delete it from the Q
  */

  if ((ph_orig_ptr = cmtask_orig_para_search_act_type( CM_SS_MAIN,
                       CM_ACT_TYPE_EMERG_CALL )) != NULL )
  {
    (void)cmtask_orig_para_delete( CM_SS_MAIN, ph_orig_ptr, TRUE );
  }

  if ((ph_orig_ptr = cmtask_orig_para_search_act_type( CM_SS_HDR,
                       CM_ACT_TYPE_EMERG_CALL )) != NULL )
  {
    (void)cmtask_orig_para_delete( CM_SS_HDR, ph_orig_ptr, TRUE );
  }

  if ((ph_orig_ptr = cmtask_orig_para_search_act_type( CM_SS_WLAN,
                       CM_ACT_TYPE_EMERG_CALL )) != NULL )
  {
    (void)cmtask_orig_para_delete( CM_SS_WLAN, ph_orig_ptr, TRUE );
  }

  /* Let clients know that we are exiting emergency callback mode
  */

  ph_ptr->ph_state = CM_PH_STATE_NORMAL;

  /* Update the DDTM status.
  */
  cmph_update_ddtm_status(ph_ptr);

  cmph_event( ph_ptr, CM_PH_EVENT_SYS_SEL_PREF );
  cmlog_exit_emergency_cb();
}


/*===========================================================================

FUNCTION cmph_enter_emergency_cb

DESCRIPTION

  Enter emergency callback mode and notify clients

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void cmph_enter_emergency_cb(

    cmph_s_type             *ph_ptr,
        /* Pointer to a phone object */

    boolean                  is_evt_reqd
        /* Do we need to generate a event */
)
{

  cm_orig_q_s_type *ph_orig_ptr;
  cm_act_orig_s_type *emer_cb_ptr;

  /* The pointer to the element of the priority queue
  */

  CM_ASSERT( ph_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return if already in emergency callback mode
  */
  if(cmph_is_in_emergency_cb())
  {
    return;
  }

  CM_MSG_MED("Enter emergency callback mode ...",0,0,0);

  emer_cb_ptr = cmph_get_emergency_cb_pref();

  emer_cb_ptr->orig_mode_pref = CM_MODE_PREF_EMERGENCY;
  emer_cb_ptr->orig_pref_term = CM_PREF_TERM_PWR_CYCLE;

  ph_orig_ptr = cmtask_orig_para_search_act_type( CM_SS_MAIN,
                                                  CM_ACT_TYPE_EMERG_CALL );

  if( ph_orig_ptr != NULL )
  {

     emer_cb_ptr->orig_band_pref       = ph_orig_ptr->orig.orig_band_pref;
     emer_cb_ptr->orig_prl_pref        = ph_orig_ptr->orig.orig_prl_pref;
     emer_cb_ptr->orig_roam_pref       = ph_orig_ptr->orig.orig_roam_pref;
     emer_cb_ptr->orig_hybr_pref       = ph_orig_ptr->orig.orig_hybr_pref;
     emer_cb_ptr->orig_int_hybr_pref   = ph_orig_ptr->orig.orig_int_hybr_pref;
     emer_cb_ptr->orig_srv_domain_pref = ph_orig_ptr->orig.orig_srv_domain_pref;
  }
  else
  {

     emer_cb_ptr->orig_band_pref       = ph_ptr->band_pref;
     emer_cb_ptr->orig_prl_pref        = ph_ptr->prl_pref;
     emer_cb_ptr->orig_roam_pref       = ph_ptr->roam_pref;
     emer_cb_ptr->orig_hybr_pref       = ph_ptr->hybr_pref;
     emer_cb_ptr->orig_int_hybr_pref   = ph_ptr->int_hybr_pref;
     emer_cb_ptr->orig_srv_domain_pref = ph_ptr->srv_domain_pref;
     CM_ERR(" Invalid state in enter cb mode",0,0,0);
  }

  (void)cmph_insert_orig_mode (
                                CM_SS_MAIN,
                                SD_SS_ORIG_MODE_EMERG_CB,
                                CM_ACT_TYPE_EMERG_CB_MODE,
                                emer_cb_ptr->orig_pref_term,
                                emer_cb_ptr->orig_mode_pref,
                                emer_cb_ptr->orig_band_pref,
                                emer_cb_ptr->orig_prl_pref,
                                emer_cb_ptr->orig_roam_pref,
                                emer_cb_ptr->orig_hybr_pref,
                                emer_cb_ptr->orig_int_hybr_pref,
                                &ph_ptr->wlan_pref,
                                emer_cb_ptr->orig_srv_domain_pref,
                                (cm_act_id_type)emer_cb_ptr,
                                CM_ACT_UPDATE_REAS_ACT_START
                              );

  ph_ptr->ph_state = CM_PH_STATE_EMERG_CB;

  /* Update the DDTM status.
  */
  cmph_update_ddtm_status(ph_ptr);

  if( is_evt_reqd )
  {
    /*
    ** Notify clients and DIAG
    */

    cmph_event( ph_ptr, CM_PH_EVENT_SYS_SEL_PREF );
    cmlog_enter_emergency_cb();
  }

}

#ifdef FEATURE_DDTM_CNTL

/*===========================================================================

FUNCTION cmph_client_cmd_ddtm_pref_para_check

DESCRIPTION
  Check parameter errors for DDTM preference setting command.

DEPENDENCIES
  None

RETURN VALUE
  CM_PH_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  None
===========================================================================*/
static cm_ph_cmd_err_e_type       cmph_client_cmd_ddtm_pref_para_check(

       cm_ph_cmd_s_type           *ph_cmd_ptr,
         /* Phone command pointer.
         */

       const cmph_s_type                *ph_ptr
         /* Pointer to phone object.
         */
)
{
  cm_ph_cmd_info_s_type          *cmd_info_ptr = NULL;
  cm_ph_cmd_err_e_type            cmd_err      = CM_PH_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_cmd_ptr != NULL);
  CM_ASSERT(ph_ptr     != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /* Check if ddtm_pref parameter is within range.
  */
  if(!BETWEEN(cmd_info_ptr->ddtm_pref,
              CM_DDTM_PREF_NONE,
              CM_DDTM_PREF_MAX))
  {
    CM_MSG_HIGH( "Invalid ddtm_pref %d",
                 cmd_info_ptr->ddtm_pref, 0, 0
               );
    cmd_err = CM_PH_CMD_ERR_DDTM_PREF_P;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if DDTM action is not NO_CHANGE atleast one valid action
  ** is specified.
  */
  if ( cmd_info_ptr->ddtm_act_mask == SYS_DDTM_ACT_MASK_EMPTY ||
       cmd_info_ptr->ddtm_act_mask >= SYS_DDTM_ACT_MASK_MAX
     )
  {
    CM_MSG_HIGH( "Invalid ddtm_act_mask %d",
                 cmd_info_ptr->ddtm_act_mask, 0, 0
               );
    cmd_err = CM_PH_CMD_ERR_DDTM_PREF_P;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if ddtm_so_list_act is in range.
  */
  if(!BETWEEN(cmd_info_ptr->ddtm_so_list_act,
              CM_DDTM_SO_LIST_ACT_NONE,
              CM_DDTM_SO_LIST_ACT_MAX ))
  {
    CM_MSG_HIGH( "Invalid ddtm_so_list_act %d",
                 cmd_info_ptr->ddtm_so_list_act, 0, 0
               );
    cmd_err = CM_PH_CMD_ERR_DDTM_PREF_P;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if num_srv_opt is in range.
  */
  if ( cmd_info_ptr->ddtm_so_list_act != CM_DDTM_SO_LIST_ACT_NONE &&
       cmd_info_ptr->ddtm_so_list_act != CM_DDTM_SO_LIST_ACT_NO_CHANGE &&
       cmd_info_ptr->ddtm_num_srv_opt > ARR_SIZE(ph_ptr->ddtm_srv_opt_list))
  {
    cmd_err = CM_PH_CMD_ERR_DDTM_NUM_SRV_OPTIONS_P;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remove duplicates from cmd_info_ptr->srv_opt_list if already present
  ** in phone object.
  */
  if (cmd_err == CM_PH_CMD_ERR_NOERR &&
      cmd_info_ptr->ddtm_so_list_act == CM_DDTM_SO_LIST_ACT_ADD )
  {
    int i = 0;
    int j = 0;

    while ( i < ph_ptr->ddtm_num_srv_opt )
    {
      j = 0;
      while ( j < cmd_info_ptr->ddtm_num_srv_opt )
      {
        if ( cmd_info_ptr->ddtm_srv_opt_list[j] == ph_ptr->ddtm_srv_opt_list[i] )
        {
          /* Swap with the last one in the array, and reduce the count.
          */
          --cmd_info_ptr->ddtm_num_srv_opt;
          cmd_info_ptr->ddtm_srv_opt_list[j] =
               cmd_info_ptr->ddtm_srv_opt_list[cmd_info_ptr->ddtm_num_srv_opt];
        }
        else
        {
          j++;
        }
      }
      i++;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Now check if the new srv_opt_list can be added.
    */
    if ( (cmd_info_ptr->ddtm_num_srv_opt + ph_ptr->ddtm_num_srv_opt) >
                 ARR_SIZE( ph_ptr->ddtm_srv_opt_list ))
    {
      /* Exceeded the maximum size.
      */
      CM_MSG_HIGH( "Exceed max size ddtm_num_srv_opt %d",
                    cmd_info_ptr->ddtm_num_srv_opt, 0, 0
                 );
      cmd_err = CM_PH_CMD_ERR_DDTM_NUM_SRV_OPTIONS_P;
    }
  }

  return cmd_err;

} /* cmph_client_cmd_ddtm_pref_para_check() */


/*===========================================================================

FUNCTION cmph_ddtm_add_so

DESCRIPTION
  Add the service option to the DDTM service list in the Phone object.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmph_ddtm_add_so(

    cmph_s_type               *ph_ptr,
        /* Pointer to a phone object.
        */

    sys_srv_opt_type           srv_opt
        /* Service option to be added.
        */
)
{
  unsigned int i;

 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check for duplicates before adding the srv_opt.
  */
  for ( i = 0;
        i < ph_ptr->ddtm_num_srv_opt &&
          i < ARR_SIZE(ph_ptr->ddtm_srv_opt_list);
        i++
      )
  {
    if ( ph_ptr->ddtm_srv_opt_list[i] == srv_opt )
    {
      /* SO already in the list
      */
      CM_MSG_HIGH("SO %d already in list", srv_opt, 0, 0 );
      return;
    }
  } /* for () */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if maximum number of srv_opts have already been added.
  */
  if ( i >= ARR_SIZE(ph_ptr->ddtm_srv_opt_list) )
  {
    CM_MSG_HIGH("MAX added: Cannot add so %d", srv_opt, 0, 0 );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the srv_opt to the DDTM list.
  */
  CM_MSG_HIGH("SO %d added in list", srv_opt, 0, 0 );
  ph_ptr->ddtm_srv_opt_list[i] = srv_opt;
  ph_ptr->ddtm_num_srv_opt++;
} /* cmph_ddtm_add_so() */




/*===========================================================================

FUNCTION cmph_ddtm_del_so

DESCRIPTION
  Deletes the service option specified from the DDTM service list in the
  Phone object.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmph_ddtm_del_so(

    cmph_s_type               *ph_ptr,
        /* Pointer to a phone object.
        */

    sys_srv_opt_type          srv_opt
        /* Service option to be added.
        */
)
{
  unsigned int i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check for duplicates before adding the srv_opt.
  */
  for ( i = 0;
        i < ph_ptr->ddtm_num_srv_opt &&
          i < ARR_SIZE(ph_ptr->ddtm_srv_opt_list);
        i++
      )
  {
    if ( ph_ptr->ddtm_srv_opt_list[i] == srv_opt )
    {
      /* SO is in the list, delete this entry by swapping with the
      ** last in the list.
      */
      --ph_ptr->ddtm_num_srv_opt;
      CM_MSG_HIGH("Deleting srv_opt %d", srv_opt, 0, 0 );
      ph_ptr->ddtm_srv_opt_list[i] =
           ph_ptr->ddtm_srv_opt_list[ph_ptr->ddtm_num_srv_opt];
      break;

    }
  } /* for () */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug message if not in list.
  */
  if ( i >= ph_ptr->ddtm_num_srv_opt )
  {

    CM_MSG_HIGH("so %d not in list", srv_opt, 0, 0 );
    return;
  }

} /* cmph_ddtm_del_so() */


/*===========================================================================

FUNCTION cmph_ddtm_update_so_list

DESCRIPTION
  Update the DDTM srv option list based on the user specified action type.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmph_ddtm_update_so_list(

    cmph_s_type                 *ph_ptr,
        /* Pointer to a phone object.
        */

    const cm_ph_cmd_info_s_type *cmd_info_ptr
        /* Pointer to a phone command information.
        */
)
{

  int i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_ptr       != NULL);
  CM_ASSERT(cmd_info_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Processing based on action type.
  */
  CM_MSG_HIGH( "DDTM so list action %d",
               cmd_info_ptr->ddtm_so_list_act,
               0,
               0
             );

  switch ( cmd_info_ptr->ddtm_so_list_act )
  {
    case CM_DDTM_SO_LIST_ACT_REPLACE:
      ph_ptr->ddtm_num_srv_opt = 0;
      for ( i = 0; i < cmd_info_ptr->ddtm_num_srv_opt; i++ )
      {
        cmph_ddtm_add_so( ph_ptr,
                          cmd_info_ptr->ddtm_srv_opt_list[i]
                        );
      }
      break;

    case CM_DDTM_SO_LIST_ACT_ADD:
      for ( i = 0; i < cmd_info_ptr->ddtm_num_srv_opt; i++ )
      {
        cmph_ddtm_add_so( ph_ptr,
                          cmd_info_ptr->ddtm_srv_opt_list[i]
                        );
      }
      break;

    case CM_DDTM_SO_LIST_ACT_DELETE:
      for ( i = 0; i < cmd_info_ptr->ddtm_num_srv_opt; i++ )
      {
        cmph_ddtm_del_so( ph_ptr,
                          cmd_info_ptr->ddtm_srv_opt_list[i]
                        );
      }
      break;

    case CM_DDTM_SO_LIST_ACT_NO_CHANGE:
    default:
      break;

  } /* switch ( cmd_info_ptr->ddtm_so_list_act) */

} /* cmph_ddtm_update_so_list */
#endif /* FEATURE_DDTM_CNTL */


/*===========================================================================

FUNCTION cmph_cmd_copy_fields

DESCRIPTION
  Copy the active fields of a phone command into a phone object.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmph_cmd_copy_fields(

    cmph_s_type               *ph_ptr,
        /* Pointer to a phone object */

    const cm_ph_cmd_s_type    *cmd_ptr
        /* Pointer to a phone command */
)
{
  const cm_ph_cmd_info_s_type    *cmd_info_ptr = NULL;
      /* Point at phone command information */

  dword                          uptime = clk_uptime();
      /* Get the current uptime */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_ptr  != NULL);
  CM_ASSERT(cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr = CMD_INFO_PTR( cmd_ptr);

  /* Only copy active command fields in order to avoid overwriting
  ** phone fields that are irrelevant to this command.
  */

  if( cmd_info_ptr->oprt_mode             != SYS_OPRT_MODE_NONE )
  {
    ph_ptr->oprt_mode                      = cmd_info_ptr->oprt_mode;
    #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
    #endif /* FEATURE_WCDMA, FEATURE_GSM */
  }

  if( cmd_info_ptr->mode_pref             != CM_MODE_PREF_NONE )
  {
    ph_ptr->mode_pref                      = cmd_info_ptr->mode_pref;
  }

  if( cmd_info_ptr->pref_term             != CM_PREF_TERM_NONE )
  {
    ph_ptr->pref_term                      = cmd_info_ptr->pref_term;
    ph_ptr->mode_uptime = uptime + cmd_info_ptr->pref_duration;
  }

  if( cmd_info_ptr->band_pref             != CM_BAND_PREF_NONE )
  {
    ph_ptr->band_pref                      = cmd_info_ptr->band_pref;
  }


  if( cmd_info_ptr->prl_pref             != CM_PRL_PREF_NONE )
  {
    ph_ptr->prl_pref                      = cmd_info_ptr->prl_pref;
  }

  if( cmd_info_ptr->roam_pref             != CM_ROAM_PREF_NONE )
  {
    ph_ptr->roam_pref                      = cmd_info_ptr->roam_pref;
  }

  #ifdef FEATURE_HDR
#error code not present
  #endif /* FEATURE_HDR */

  if( cmd_info_ptr->nam_sel               != CM_NAM_NONE )
  {
    ph_ptr->nam_sel                        = cmd_info_ptr->nam_sel;
    if( ph_ptr->nam_sel != CM_NAM_AUTO )
    {
      ph_ptr->curr_nam = ph_ptr->nam_sel;
    }
  }

  if( cmd_info_ptr->packet_state          != CM_PACKET_STATE_NONE )
  {
    ph_ptr->packet_state                   = cmd_info_ptr->packet_state;
  }


  if( cmd_info_ptr->network_sel_mode_pref != CM_NETWORK_SEL_MODE_PREF_NONE )
  {
    ph_ptr->network_sel_mode_pref          = cmd_info_ptr->network_sel_mode_pref;
  }

  /* Only cmd_info_ptr->srv_domain_pref can come here with NO_CHANGE,
  ** others like net_sel_mode, acq_order_pref cannot come here with
  ** NO_CHANGE
  */
  if( (cmd_info_ptr->srv_domain_pref != CM_SRV_DOMAIN_PREF_NONE ) &&
      (cmd_info_ptr->srv_domain_pref != CM_SRV_DOMAIN_PREF_NO_CHANGE)
    )
  {

    #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
    #endif /* FEATURE_WCDMA || FEATURE_GSM */
    {
      ph_ptr->srv_domain_pref                = cmd_info_ptr->srv_domain_pref;
    }
  }

  if( cmd_info_ptr->acq_order_pref != CM_GW_ACQ_ORDER_PREF_NONE )
  {
    ph_ptr->acq_order_pref                 = cmd_info_ptr->acq_order_pref;
  }

  if( !sys_plmn_id_is_undefined( cmd_info_ptr->plmn ) )
  {
    ph_ptr->plmn                           = cmd_info_ptr->plmn;
  }

  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
  #endif /* FEATURE_WCDMA || FEATURE_GSM */

  if(cmd_ptr->info.answer_voice != CM_ANSWER_VOICE_NONE) {

    ph_ptr->answer_voice  = cmd_info_ptr->answer_voice;
    ph_ptr->answer_uptime = uptime + cmd_info_ptr->answer_duration;

  }

  #ifdef FEATURE_DDTM_CNTL
  if( cmd_info_ptr->ddtm_pref      != CM_DDTM_PREF_NONE  &&
      cmd_info_ptr->ddtm_pref      != CM_DDTM_PREF_NO_CHANGE
    )
  {
      ph_ptr->ddtm_pref                   = cmd_info_ptr->ddtm_pref;
  }

  if ( cmd_info_ptr->ddtm_act_mask != SYS_DDTM_ACT_MASK_EMPTY &&
       cmd_info_ptr->ddtm_act_mask != SYS_DDTM_ACT_MASK_NO_CHANGE
     )
  {
    ph_ptr->ddtm_act_mask          =  cmd_info_ptr->ddtm_act_mask;
    ph_ptr->cur_ddtm_act_mask      = SYS_DDTM_ACT_MASK_EMPTY;
  }

  if ( cmd_info_ptr->ddtm_so_list_act != CM_DDTM_SO_LIST_ACT_NONE &&
       cmd_info_ptr->ddtm_so_list_act != CM_DDTM_SO_LIST_ACT_NO_CHANGE )
  {
    cmph_ddtm_update_so_list( ph_ptr, cmd_info_ptr );
  }
  #endif /* FEATURE_DDTM_CNTL */

  #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
  #endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */

  #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
  #endif

  if( cmd_info_ptr->network_type          != CM_MODE_PREF_NONE )
  {
    ph_ptr->network_type     =    cmd_info_ptr->network_type;
  }


  if( cmd_info_ptr->wlan_pref.tech_pref != CM_WLAN_TECH_PREF_NONE )
  {
     ph_ptr->wlan_pref.tech_pref  =  cmd_info_ptr->wlan_pref.tech_pref;
  }

  #ifdef FEATURE_WLAN_ADHOC
#error code not present
  #endif

  if( cmd_info_ptr->wlan_pref.scan_mode != CM_WLAN_SCAN_PREF_NONE )
  {
     ph_ptr->wlan_pref.scan_mode  =  cmd_info_ptr->wlan_pref.scan_mode;
  }

  if( cmd_info_ptr->wlan_pref.bss_type != SYS_WLAN_BSS_TYPE_NONE )
  {
     ph_ptr->wlan_pref.bss_type  =  cmd_info_ptr->wlan_pref.bss_type;
  }

  if( cmd_info_ptr->wlan_pref.network_sel_mode_pref != CM_NETWORK_SEL_MODE_PREF_NONE )
  {
     ph_ptr->wlan_pref.network_sel_mode_pref  =
                             cmd_info_ptr->wlan_pref.network_sel_mode_pref ;

     if( cmd_info_ptr->wlan_pref.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
     {
        ph_ptr->wlan_pref.bss_id = cmd_info_ptr->wlan_pref.bss_id;

        ph_ptr->wlan_pref.chan   = cmd_info_ptr->wlan_pref.chan;

        ph_ptr->wlan_pref.band   = cmd_info_ptr->wlan_pref.band;

        ph_ptr->wlan_pref.ssid   = cmd_info_ptr->wlan_pref.ssid;

        ph_ptr->wlan_pref.profile_id = cmd_info_ptr->wlan_pref.profile_id;

        ph_ptr->wlan_pref.profile =  cmd_info_ptr->wlan_pref.profile;
     }
  }

  (void)cmph_insert_orig_mode (
                                CM_SS_MAIN,
                                SD_SS_ORIG_MODE_NONE,
                                CM_ACT_TYPE_PH_OBJ,
                                ph_ptr->pref_term,
                                ph_ptr->mode_pref,
                                ph_ptr->band_pref,
                                ph_ptr->prl_pref,
                                ph_ptr->roam_pref,
                                ph_ptr->hybr_pref,
                                ph_ptr->int_hybr_pref,
                                &ph_ptr->wlan_pref,
                                ph_ptr->srv_domain_pref,
                                (cm_act_id_type)ph_ptr,
                                CM_ACT_UPDATE_REAS_ACT_CONT
                              );

  (void)cmph_insert_orig_mode (
                                CM_SS_HDR,
                                SD_SS_ORIG_MODE_NONE,
                                CM_ACT_TYPE_PH_OBJ,
                                ph_ptr->pref_term,
                                ph_ptr->mode_pref,
                                ph_ptr->band_pref,
                                ph_ptr->prl_pref,
                                ph_ptr->roam_pref,
                                ph_ptr->hybr_pref,
                                ph_ptr->int_hybr_pref,
                                &ph_ptr->wlan_pref,
                                ph_ptr->srv_domain_pref,
                                (cm_act_id_type)ph_ptr,
                                CM_ACT_UPDATE_REAS_ACT_CONT
                              );

  (void)cmph_insert_orig_mode (
                                CM_SS_WLAN,
                                SD_SS_ORIG_MODE_NONE,
                                CM_ACT_TYPE_PH_OBJ,
                                ph_ptr->pref_term,
                                ph_ptr->mode_pref,
                                ph_ptr->band_pref,
                                ph_ptr->prl_pref,
                                ph_ptr->roam_pref,
                                ph_ptr->hybr_pref,
                                ph_ptr->int_hybr_pref,
                                &ph_ptr->wlan_pref,
                                ph_ptr->srv_domain_pref,
                                (cm_act_id_type)ph_ptr,
                                CM_ACT_UPDATE_REAS_ACT_CONT
                              );

} /* cmph_cmd_copy_fields() */


/*===========================================================================

@FUNCTION cmph_cmd_client_release

@DESCRIPTION
  Inform CM that a client was released so that CM can inform asynchronously
  free the client object.

  This is mostly important when a client release happens in the client
  callback function that is called during an event, or immediately after it
  (in case of multiprocessor builds).

@DEPENDENCIES
  cm_client_deact() should have been called for the client before calling
  this function.

@RETURNS
  none

===========================================================================*/
void cmph_cmd_client_release(

  cm_client_s_type        *client_ptr
    /**< Client object that should be released.
    */

)
{
  cm_cmd_s_type           *cmd_ptr;
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed, then free the client object
  ** synchronously. If this is an issue, the code will error fatal elsewhere.
  ** The reasoning is that freeing synchronously is less likely to be an
  ** issue when compared to error fataling all the time.
  */
  if( cmd_ptr == NULL )
  {
    /* Print debug messages for params.
    */
    CM_ERR("Client object freed sync",0,0,0);

    cmclient_list_del( client_ptr );
    cmclient_free_client_buf(client_ptr);

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Else (command buffer allocation is successful)
  */

  /* Get a reference to command header
  ** and a reference to command information.
  */
  ph_cmd_ptr = PH_CMD_PTR( cmd_ptr );
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func             = NULL;
  ph_cmd_ptr->data_block_ptr          = NULL;
  ph_cmd_ptr->client_id               = CM_CLIENT_ID_ANONYMOUS;
  ph_cmd_ptr->cmd                     = CM_PH_CMD_CLIENT_RELEASE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr->unused_client_ptr   = client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug messages for params.
  */
  CM_MSG_HIGH("Client obj freed async",0,0,0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( cmd_ptr );

}


/*===========================================================================

FUNCTION cmph_ph_cmd_answer_voice_para_check

DESCRIPTION
  Check parameter errors for answer voice command

DEPENDENCIES
  None

RETURN VALUE
  CM_PH_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  None
===========================================================================*/
static cm_ph_cmd_err_e_type  cmph_ph_cmd_answer_voice_para_check(
  cm_ph_cmd_s_type           *ph_cmd_ptr
)
{
  cm_ph_cmd_info_s_type          *cmd_info_ptr = NULL;
  cm_ph_cmd_err_e_type           cmd_err       = CM_PH_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  if( !BETWEEN(cmd_info_ptr->answer_voice, CM_ANSWER_VOICE_NONE, CM_ANSWER_VOICE_MAX) )
  {
    cmd_err = CM_PH_CMD_ERR_ANSWER_VOICE_P;
    CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_ANSWER_VOICE_P",0,0,0 );
  }
  else if( ( cmd_info_ptr->answer_voice == CM_ANSWER_VOICE_AS_FAX_ONCE ||
             cmd_info_ptr->answer_voice == CM_ANSWER_VOICE_AS_MODEM_ONCE  ) &&
           cmd_info_ptr->answer_duration == 0 )
  {
    cmd_err = CM_PH_CMD_ERR_ANSWER_DURATION_P;
    CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_ANSWER_DURATION_P",0,0,0 );
  }

  return cmd_err;

} /* cmph_ph_cmd_answer_voice_para_check() */



/*===========================================================================

FUNCTION cmph_ph_cmd_nam_sel_para_check

DESCRIPTION
  Check parameter errors for mode preference command

DEPENDENCIES
  None

RETURN VALUE
  CM_PH_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  None
===========================================================================*/
static cm_ph_cmd_err_e_type  cmph_ph_cmd_nam_sel_para_check(
  cm_ph_cmd_s_type           *ph_cmd_ptr
)
{

  cm_ph_cmd_info_s_type          *cmd_info_ptr = NULL;
  cm_ph_cmd_err_e_type           cmd_err       = CM_PH_CMD_ERR_NOERR;

  int max_num_nams = NV_MAX_NAMS;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /* Check integrity of command parameters in increasing order of
  ** priority. This scheme implies that the caller of this function
  ** is always notified of the most critical error.
  */

  if( cmd_info_ptr->nam_sel != CM_NAM_AUTO &&
      !BETWEEN((int)cmd_info_ptr->nam_sel, (int)CM_NAM_NONE, (int)NV_MAX_NAMS) )
  {
    cmd_err  = CM_PH_CMD_ERR_NAM_P;
    CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_NAM_P",0,0,0 );
  }
  /*lint -save -e774 Boolean within 'if' always evaluates to False */
  else if( max_num_nams == 1 && cmd_info_ptr->nam_sel == CM_NAM_AUTO )
  /*lint -restore */
  {
    cmd_err  = CM_PH_CMD_ERR_NAM_P;
    CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_NAM_P",0,0,0 );
  }

  return cmd_err;
} /* cmph_ph_cmd_nam_sel_para_check() */



/*===========================================================================

FUNCTION cmph_ph_cmd_oprt_mode_para_check

DESCRIPTION
  Check parameter errors for operating mode setting command

DEPENDENCIES
  None

RETURN VALUE
  CM_PH_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  None
===========================================================================*/
static cm_ph_cmd_err_e_type  cmph_ph_cmd_oprt_mode_para_check(
  cm_ph_cmd_s_type           *ph_cmd_ptr
)
{
  cm_ph_cmd_info_s_type          *cmd_info_ptr = NULL;
  cm_ph_cmd_err_e_type            cmd_err      = CM_PH_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*
  ** The clients can not make a request to set the operating mode to test.
  ** Only the network (WCDMA/GSM) can make this request.
  */
  if( ( !BETWEEN(cmd_info_ptr->oprt_mode, SYS_OPRT_MODE_NONE, SYS_OPRT_MODE_MAX ) ||
      ( cmd_info_ptr->oprt_mode == SYS_OPRT_MODE_NET_TEST_GW )))
  {
    cmd_err  = CM_PH_CMD_ERR_OPRT_MODE_P;
    CM_DBG_ERR_FATAL( "CM_CMD_ERR_BAD_OPRT_MODE_P",0,0,0 );
  }

  return cmd_err;
} /* cmph_ph_cmd_oprt_mode_para_check() */

/*===========================================================================

FUNCTION cmph_ps_attach_detach_para_check

DESCRIPTION
  Check parameter errors for PS attach detach commands

DEPENDENCIES
  None

RETURN VALUE
  CM_PH_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_ph_cmd_err_e_type.

SIDE EFFECTS
  None
===========================================================================*/
static cm_ph_cmd_err_e_type  cmph_ps_attach_detach_para_check
(
  const cm_ph_cmd_info_s_type          *cmd_info_ptr
)
/*lint -esym(715, cmd_info_ptr) */
{
  cm_ph_cmd_err_e_type           cmd_err       = CM_PH_CMD_ERR_NOERR;

  #if ( (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) \
    && !defined(FEATURE_WCDMA) && !defined(FEATURE_GSM) )
  /*
  ** 1x mode.
  */
    /* command not supported for 1X only mode  */
    cmd_err = CM_PH_CMD_ERR_SERVICE_DOMAIN_P;

  #else
  cmph_s_type                    *ph_ptr       = cmph_ptr();
  cmss_s_type                   *ss_ptr = cmss_ptr();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_info_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* If PS_Invalid or PS_CS_Invalid is set in the SIM, reject PS Attach/detach
  */
  if (ss_ptr->info.mode_info.gw_info.sim_state == SYS_SIM_STATE_PS_INVALID ||
      ss_ptr->info.mode_info.gw_info.sim_state == SYS_SIM_STATE_CS_PS_INVALID)
  {
    cmd_err = CM_PH_CMD_ERR_OTHER;
    CM_DBG_ERR_FATAL( "Invalid PS or CS_PS SIM state", 0, 0, 0 );

  }

  /* Multimode */
  if ( ph_ptr->mode_pref == CM_MODE_PREF_AMPS_ONLY )

  {
    cmd_err  = CM_PH_CMD_ERR_MODE_PREF_P;
    CM_DBG_ERR_FATAL( "Invalid phone mode pref", 0, 0, 0 );

  }

  if( cmd_info_ptr->pref_term != CM_PREF_TERM_PWR_CYCLE )
  {
    /* currently only this is supported for PS attach */
      cmd_err = CM_PH_CMD_ERR_PREF_TERM_P;
      CM_DBG_ERR_FATAL( "Invalid service domain pref parameter:",
                         cmd_info_ptr->pref_term,
                         0,
                         0 );
  }
  if( cmd_info_ptr->band_pref != CM_BAND_PREF_NO_CHANGE )
  {
    cmd_err  = CM_PH_CMD_ERR_BAND_PREF_P;
    CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_BAND_PREF_P",0,0,0 );
  }

  if( cmd_info_ptr->roam_pref != CM_ROAM_PREF_NO_CHANGE )
  {
    cmd_err  = CM_PH_CMD_ERR_ROAM_PREF_P;
    CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_ROAM_PREF_P",0,0,0 );
  }

  if( cmd_info_ptr->mode_pref != CM_MODE_PREF_NO_CHANGE )
  {
    cmd_err  = CM_PH_CMD_ERR_MODE_PREF_P;
    CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_MODE_PREF_P",0,0,0 );
  }

  if( cmd_info_ptr->hybr_pref != CM_HYBR_PREF_NO_CHANGE )
  {
   cmd_err = CM_PH_CMD_ERR_HYBR_PREF_P;
   CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_HYBR_PREF_P",0,0,0 );
  }

  if ( cmd_info_ptr->acq_order_pref != CM_GW_ACQ_ORDER_PREF_NO_CHANGE )
  {
    cmd_err = CM_PH_CMD_ERR_ACQ_ORDER_PREF_P;
    CM_DBG_ERR_FATAL( "Invalid acq order pref parameter:",
                      cmd_info_ptr->acq_order_pref,
                      0,
                      0 );
  }
  if ( cmd_info_ptr->network_sel_mode_pref != CM_NETWORK_SEL_MODE_PREF_NO_CHANGE )
  {
    cmd_err = CM_PH_CMD_ERR_NETWORK_SEL_MODE_P;
    CM_DBG_ERR_FATAL( "Invalid network sel mode pref parameter:",
                      cmd_info_ptr->network_sel_mode_pref,
                      0,
                      0 );
  }

  #endif /* FEATURE_CDMA_800) || FEATURE_CDMA_1900 && !FEATURE_WCDMA && !FEATURE_GSM */

  return cmd_err;
}
/*lint +esym(715, cmd_info_ptr) */


/*===========================================================================

FUNCTION cmph_ph_cmd_avoid_sys_para_check

DESCRIPTION
  Check parameter errors for avoid sys command

DEPENDENCIES
  None

RETURN VALUE
  CM_PH_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  None
===========================================================================*/
static cm_ph_cmd_err_e_type  cmph_ph_cmd_avoid_sys_para_check(
  cm_ph_cmd_s_type           *ph_cmd_ptr
)
{
  cm_ph_cmd_info_s_type          *cmd_info_ptr = NULL;
  cm_ph_cmd_err_e_type            cmd_err      = CM_PH_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  if( !INRANGE(cmd_info_ptr->avoid_type, CM_SS_AVOID_SYS_SRV_IF_UZ,
    CM_SS_AVOID_SYS_MAX) )
  {
    cmd_err  = CM_PH_CMD_ERR_AVOID_SYS_P;
    CM_DBG_ERR_FATAL( "CM_CMD_ERR_BAD_AVOID_SYS_P",0,0,0 );
  }

  return cmd_err;
} /* cmph_ph_cmd_avoid_sys_para_check() */

/*===========================================================================

FUNCTION  cmph_cmd_hplmn_srch_para_check

DESCRIPTION
  This function checks current CM state for hplmn search request.

DEPENDENCIES
  none

RETURN VALUE
  CM_PH_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_ph_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_ph_cmd_err_e_type     cmph_cmd_hplmn_srch_para_check(

  const cm_ph_cmd_info_s_type          *cmd_info_ptr
    /* Pointer to a cmd info object */
)
{
  cm_ph_cmd_err_e_type       cmd_err      = CM_PH_CMD_ERR_NOERR;

  #if ( (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) \
    && !defined(FEATURE_WCDMA) && !defined(FEATURE_GSM) \
    && !defined(FEATURE_GW_ON_DEMAND_HPLMN_SRCH))
  /*
  ** 1x mode.
  */
    /* command not supported for 1X only mode  */
    cmd_err = CM_PH_CMD_ERR_SERVICE_DOMAIN_P;

    /* To suppress lint */
    SYS_ARG_NOT_USED (cmd_info_ptr);

  #else
  cmph_s_type                    *ph_ptr       = cmph_ptr();

  /*----------------------------------------------------------------------*/

  /* HPLMN SRCH is not allowed when non-ps calls are present
  */
  if ( !( cmcall_count_calls_with_call_type( CM_CALL_TYPE_PS_DATA ) ==
          cmcall_call_ids_allocated()
        )
     )
  {
    cmd_err = CM_PH_CMD_ERR_IN_USE_S;
  }
  else if (ph_ptr->network_type != CM_MODE_PREF_NONE)
  {
    /* Get networks is pending, reject the HPLMN srch request
    */
    cmd_err = CM_PH_CMD_ERR_GET_NETWORKS_S;
  }
  else if( ph_ptr->network_sel_mode_pref != CM_NETWORK_SEL_MODE_PREF_AUTOMATIC)
  {
    cmd_err = CM_PH_CMD_ERR_SYS_SEL_PREF_S;
  }
  else if( cmd_info_ptr->band_pref != CM_BAND_PREF_NO_CHANGE )
  {
    cmd_err  = CM_PH_CMD_ERR_BAND_PREF_P;
  }
  else if( cmd_info_ptr->roam_pref != CM_ROAM_PREF_NO_CHANGE )
  {
    cmd_err  = CM_PH_CMD_ERR_ROAM_PREF_P;
  }
  else if( cmd_info_ptr->mode_pref != CM_MODE_PREF_NO_CHANGE )
  {
    cmd_err  = CM_PH_CMD_ERR_MODE_PREF_P;
  }
  else if( cmd_info_ptr->hybr_pref != CM_HYBR_PREF_NO_CHANGE )
  {
   cmd_err = CM_PH_CMD_ERR_HYBR_PREF_P;
  }
  else if( cmd_info_ptr->acq_order_pref != CM_GW_ACQ_ORDER_PREF_NO_CHANGE )
  {
    cmd_err = CM_PH_CMD_ERR_ACQ_ORDER_PREF_P;
  }
  else if( cmd_info_ptr->pref_term != CM_PREF_TERM_PWR_CYCLE )
  {
    cmd_err = CM_PH_CMD_ERR_PREF_TERM_P;
  }

  if (cmd_err != CM_PH_CMD_ERR_NOERR)
  {
    CM_ERR ("hplmn_srch_para_check returns %d", cmd_err, 0, 0);
  }

  #endif

  return cmd_err;

} /* cmph_cmd_hplmn_srch_para_check */



/*===========================================================================

FUNCTION cmph_ph_cmd_sys_sel_pref_para_check

DESCRIPTION
  Check parameter errors for system selection preference setting command.

DEPENDENCIES
  None

RETURN VALUE
  CM_PH_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  None
===========================================================================*/
static cm_ph_cmd_err_e_type  cmph_ph_cmd_sys_sel_pref_para_check(
  cm_ph_cmd_s_type           *ph_cmd_ptr
)
{
  cm_ph_cmd_info_s_type          *cmd_info_ptr = NULL;
  cm_ph_cmd_err_e_type            cmd_err      = CM_PH_CMD_ERR_NOERR;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  if( ( cmd_info_ptr->srv_domain_pref == CM_SRV_DOMAIN_PREF_PS_ATTACH ) ||
        ( cmd_info_ptr->srv_domain_pref == CM_SRV_DOMAIN_PREF_PS_DETACH ) )
  {
    cmd_err = cmph_ps_attach_detach_para_check( cmd_info_ptr );

    return cmd_err;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( cmd_info_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_HPLMN_SRCH)
  {
    cmd_err = cmph_cmd_hplmn_srch_para_check( cmd_info_ptr );

    return cmd_err;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ! BETWEEN(cmd_info_ptr->mode_pref,
                CM_MODE_PREF_NONE,
                CM_MODE_PREF_MAX))
  {
    cmd_err  = CM_PH_CMD_ERR_MODE_PREF_P;
    CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_MODE_PREF_P",0,0,0 );
  }

  if( cmd_info_ptr->mode_pref != CM_MODE_PREF_NO_CHANGE &&
      ! BETWEEN(cmd_info_ptr->pref_term,
                CM_PREF_TERM_NONE,
                CM_PREF_TERM_CM_1_CALL) )
  {
    cmd_err  = CM_PH_CMD_ERR_PREF_TERM_P;
    CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_PREF_TERM_P",0,0,0 );
  }

  if(cmd_info_ptr->band_pref == CM_BAND_PREF_NONE ||
     cmd_info_ptr->band_pref == CM_BAND_PREF_MAX)
  {
    cmd_err  = CM_PH_CMD_ERR_BAND_PREF_P;
    CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_BAND_PREF_P",0,0,0 );
  }

  if( !BETWEEN(cmd_info_ptr->roam_pref,
               CM_ROAM_PREF_NONE,
               CM_ROAM_PREF_MAX) )
  {
    cmd_err  = CM_PH_CMD_ERR_ROAM_PREF_P;
    CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_ROAM_PREF_P",0,0,0 );
  }

  if(!BETWEEN(cmd_info_ptr->hybr_pref,
              CM_HYBR_PREF_NONE,
              CM_HYBR_PREF_MAX))
  {
    cmd_err = CM_PH_CMD_ERR_HYBR_PREF_P;
  }

  #if defined(FEATURE_HDR) && defined(FEATURE_FULL_TIME_SHDR)
#error code not present
  #endif

  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
  #else
  /* If not NO_CHANGE or not PERSISTENT return cmd_err */
  if (!( (cmd_info_ptr->acq_order_pref == CM_GW_ACQ_ORDER_PREF_NO_CHANGE ) ||
         (cmd_info_ptr->acq_order_pref == CM_GW_ACQ_ORDER_PREF_PERSISTENT)
     )
     )
  #endif  /* FEATURE_WCDMA || FEATURE_GSM */
  {
    cmd_err = CM_PH_CMD_ERR_ACQ_ORDER_PREF_P;
    CM_DBG_ERR_FATAL( "Invalid acq order pref parameter:",
                      cmd_info_ptr->acq_order_pref,
                      0,
                      0 );
  }

  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
  #else
  if ( cmd_info_ptr->srv_domain_pref != CM_SRV_DOMAIN_PREF_NO_CHANGE )
  #endif  /* FEATURE_WCDMA || FEATURE_GSM */
  {
    cmd_err = CM_PH_CMD_ERR_SERVICE_DOMAIN_P;
    CM_DBG_ERR_FATAL( "Invalid service domain pref parameter:",
                      cmd_info_ptr->srv_domain_pref,
                      0,
                      0 );
  }

  /*
  ** For now, do not process a CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV network
  ** selection mode preference
  */
  if (( cmd_info_ptr->network_sel_mode_pref != CM_NETWORK_SEL_MODE_PREF_AUTOMATIC ) &&
      ( cmd_info_ptr->network_sel_mode_pref != CM_NETWORK_SEL_MODE_PREF_MANUAL    ) &&
      ( cmd_info_ptr->network_sel_mode_pref != CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV ) &&
      ( cmd_info_ptr->network_sel_mode_pref != CM_NETWORK_SEL_MODE_PREF_PERSISTENT ) &&
      ( cmd_info_ptr->network_sel_mode_pref != CM_NETWORK_SEL_MODE_PREF_NO_CHANGE )    )

  {
    cmd_err = CM_PH_CMD_ERR_NETWORK_SEL_MODE_P;
    CM_DBG_ERR_FATAL( "Invalid network sel mode pref parameter:",
                      cmd_info_ptr->srv_domain_pref,
                      0,
                      0 );
  }

  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
  #endif  /* FEATURE_WCDMA || FEATURE_GSM */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  #ifdef FEATURE_HYBR_GW
#error code not present
  #else
  /* If hybr GW is not defined and the hybrid preference has WCDMA, reject
  ** the command
  */

  if( cmd_info_ptr->hybr_pref == CM_HYBR_PREF_CDMA__WCDMA ||
      cmd_info_ptr->hybr_pref == CM_HYBR_PREF_CDMA__HDR_WCDMA )
  {
    cmd_err = CM_PH_CMD_ERR_HYBR_PREF_P;
  }
  #endif

  if( ! BETWEEN(cmd_info_ptr->prl_pref,
                CM_PRL_PREF_NONE,
                CM_PRL_PREF_MAX))
  {
    cmd_err  = CM_PH_CMD_ERR_PRL_PREF_P;
    CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_PRL_PREF_P",0,0,0 );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Only able to service this request if this is valid WLAN network selection request.
  */
  if(cmd_info_ptr->is_wlan_pref_included)
  {
    if (!cmph_is_valid_wlan_pref( &(cmd_info_ptr->wlan_pref)))
    {
      cmd_err = CM_PH_CMD_ERR_WLAN_PREF_P;
      CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_WLAN_PREF_P",0,0,0 );
    }
  }

  return cmd_err;
} /* cmph_ph_cmd_sys_sel_pref_para_check() */



/*===========================================================================

FUNCTION  cmph_cmd_subscription_available_cmd_para_check

DESCRIPTION
  This function validates the subscription available command parameters.

DEPENDENCIES
  none

RETURN VALUE
  CM_PH_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_ph_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_ph_cmd_err_e_type  cmph_cmd_subscription_available_cmd_para_check
(
  const cmph_s_type            *ph_ptr,
      /* Pointer to a phone object */

  const cm_ph_cmd_info_s_type  *cmd_info_ptr
      /* Pointer to the phone command */
)
{
  cm_ph_cmd_err_e_type         cmd_err = CM_PH_CMD_ERR_NOERR;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_ptr       != NULL);
  CM_ASSERT(cmd_info_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** If the client has indicated that the CDMA subscription information
  ** is now available, verify that it has not already been made available.
  */
  if ( cmd_info_ptr->cdma_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
  {
    if ( ph_ptr->is_cdma_subscription_available == TRUE && !ph_ptr->is_pending_subsc_cdma)
    {
      cmd_err = CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S;
    }
  }

  /*
  ** If the client has indicated that the GSM/WCDMA subscription information
  ** is now available, verify that it has not already been made available.
  */
  if ( cmd_info_ptr->gw_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
  {
    if ( ph_ptr->is_gw_subscription_available == TRUE && !ph_ptr->is_pending_subsc_gw)
    {
      cmd_err = CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S;
    }
  }

  /*
  ** If the client has indicated that the WLAN subscription information
  ** is now available, verify that it has not already been made available.
  */
  if ( cmd_info_ptr->wlan_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
  {
    if ( ph_ptr->is_wlan_subscription_available == TRUE && !ph_ptr->is_pending_subsc_wlan)
    {
      cmd_err = CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S;
    }
  }

  if ( cmd_err != CM_PH_CMD_ERR_NOERR )
  {
    CM_ERR( "Invalid subscription available command. "
            "Subscription is already available",
            0,
            0,
            0 );
  }

  return cmd_err;
} /* cmph_cmd_subscription_available_cmd_para_check() */



/*===========================================================================

FUNCTION  cmph_cmd_subscription_not_available_cmd_para_check

DESCRIPTION
  This function validates the subscription not available command parameters.

DEPENDENCIES
  none

RETURN VALUE
  CM_PH_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_ph_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_ph_cmd_err_e_type  cmph_cmd_subscription_not_available_cmd_para_check
(
  const cmph_s_type            *ph_ptr,
      /* Pointer to a phone object */

  const cm_ph_cmd_info_s_type  *cmd_info_ptr
      /* Pointer to the phone command */
)
{
  cm_ph_cmd_err_e_type         cmd_err = CM_PH_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_ptr       != NULL);
  CM_ASSERT(cmd_info_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** If the client has indicated that the CDMA subscription information
  ** is now not available, verify that it has not already been made not
  ** available.
  */
  if ( cmd_info_ptr->cdma_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
  {
    if ( ph_ptr->is_cdma_subscription_available == FALSE && !ph_ptr->is_pending_subsc_cdma)
    {
      cmd_err = CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S;
    }
  }

  /*
  ** If the client has indicated that the GSM/WCDMA subscription information
  ** is now not available, verify that it has already been made not available.
  */
  if ( cmd_info_ptr->gw_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
  {
    if ( ph_ptr->is_gw_subscription_available == FALSE && !ph_ptr->is_pending_subsc_gw)
    {
      cmd_err = CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S;
    }
  }

  /*
  ** If the client has indicated that the WLAN subscription information
  ** is now not available, verify that it has already been made not available.
  */
  if ( cmd_info_ptr->wlan_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
  {
    if ( ph_ptr->is_wlan_subscription_available == FALSE && !ph_ptr->is_pending_subsc_wlan)
    {
      cmd_err = CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S;
    }
  }

  /*
  ** If an error has not already occurred, verify that this command
  ** can be processed for the current operating mode.
  */
  if ( cmd_err != CM_PH_CMD_ERR_NOERR )
  {
    CM_ERR( "Invalid subscription not available command. "
            "Subscription is already not available",
            0,
            0,
            0 );
  }
  return cmd_err;
} /* cmph_cmd_subscription_not_available_cmd_para_check() */



/*===========================================================================

FUNCTION  cmph_cmd_subscription_changed_cmd_para_check

DESCRIPTION
  This function validates the subscription not available command parameters.

DEPENDENCIES
  none

RETURN VALUE
  CM_PH_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_ph_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_ph_cmd_err_e_type  cmph_cmd_subscription_changed_cmd_para_check
(
  const cmph_s_type            *ph_ptr,
      /* Pointer to a phone object */

  const cm_ph_cmd_info_s_type  *cmd_info_ptr
      /* Pointer to the phone command */
)
{
  cm_ph_cmd_err_e_type         cmd_err = CM_PH_CMD_ERR_NOERR;

 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_ptr       != NULL);
  CM_ASSERT(cmd_info_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** If the client has indicated that the CDMA subscription information
  ** is now not available, verify that it has not already been made not
  ** available.
  */
  if ( cmd_info_ptr->cdma_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE )
  {
    if ( ph_ptr->is_cdma_subscription_available == FALSE && !ph_ptr->is_pending_subsc_cdma)
    {
      cmd_err = CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S;
    }
  }

  /*
  ** If the client has indicated that the GSM/WCDMA subscription information
  ** is now not available, verify that it has already been made not available.
  */
  if ( cmd_info_ptr->gw_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE )
  {
    if ( ph_ptr->is_gw_subscription_available == FALSE && !ph_ptr->is_pending_subsc_gw)
    {
      cmd_err = CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S;
    }
  }

  /*
  ** If the client has indicated that the WLAN subscription information
  ** is now not available, verify that it has already been made not available.
  */
  if ( cmd_info_ptr->wlan_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE )
  {
    if ( ph_ptr->is_wlan_subscription_available == FALSE && !ph_ptr->is_pending_subsc_wlan)
    {
      cmd_err = CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S;
    }
  }

  /*
  ** If an error has not already occurred, verify that this command
  ** can be processed for the current operating mode.
  */
  if ( cmd_err == CM_PH_CMD_ERR_NOERR )
  {
    if ( !cmph_in_valid_oprt_mode( ph_ptr ) )
    {
      cmd_err = CM_PH_CMD_ERR_OPRT_MODE_S;
    }
  }
  else
  {
    CM_ERR( "Invalid subscription changed command. "
            "Subscription is not available",
            0,
            0,
            0 );
  }
  return cmd_err;
} /* cmph_cmd_subscription_changed_cmd_para_check() */


#if ( defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) )

/*===========================================================================

FUNCTION cmph_ph_cmd_change_rtre_config_para_check

DESCRIPTION
  Check parameter errors for change RTRE config command

DEPENDENCIES
  None

RETURN VALUE
  CM_PH_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  None
===========================================================================*/
static cm_ph_cmd_err_e_type  cmph_ph_cmd_change_rtre_config_para_check(
  cm_ph_cmd_s_type           *ph_cmd_ptr
)
{
  cm_ph_cmd_info_s_type          *cmd_info_ptr = NULL;
  cm_ph_cmd_err_e_type           cmd_err       = CM_PH_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  if( !INRANGE(cmd_info_ptr->rtre_config, CM_RTRE_CONFIG_RUIM_ONLY,
    CM_RTRE_CONFIG__MAX) )
  {
    cmd_err  = CM_PH_CMD_ERR_RTRE_CONFIG_P;
    CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_RTRE_CONFIG_P",0,0,0 );
  }

  return cmd_err;
} /* cmph_ph_cmd_change_rtre_config_para_check() */
#endif /* defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) */


#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_UMTSMSM */


/*===========================================================================

FUNCTION cmph_client_cmd_check

DESCRIPTION
  Check for phone command parameter errors and whether a specified phone
  command is allowed in the current state of the call/phone.


DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  CM_PH_CMD_ERR_NOERR if command is allowed in the current state
  of the phone, specific cm_ph_cmd_err_e_type otherwise.

SIDE EFFECTS
  Might change one or more of the command parameters pointed by cmd_ptr.

===========================================================================*/
static cm_ph_cmd_err_e_type  cmph_client_cmd_check
/*lint -esym(818,ph_ptr)
** cmph_cmd_get_networks_para_check() may modify ph_ptr
*/
(

    cmph_s_type            *ph_ptr,
        /* Pointer to a phone object */

    cm_cmd_s_type          *cmd_ptr,
        /* Pointer to a CM command */

    boolean                *network_sel_mode_pref_changed_ptr
        /* = TRUE if client changed the network sel mode pref */
)
{
  cm_ph_cmd_s_type               *ph_cmd_ptr   = NULL;
      /* Point at phone command component */

  cm_ph_cmd_info_s_type          *cmd_info_ptr = NULL;
      /* Point at command information component */

  cm_ph_cmd_err_e_type           cmd_err       = CM_PH_CMD_ERR_NOERR;
      /* Initialize command error to NO-ERROR */

  boolean                        is_curr_nam_valid = TRUE;

  cm_mode_pref_e_type            cm_mode_pref;

  #if defined FEATURE_GSM || defined FEATURE_WCDMA
#error code not present
  #endif /* FEATURE_GSM ||  FEATURE_WCDMA */

  #ifdef FEATURE_USERZONE
  cmss_s_type                   *ss_ptr = cmss_ptr();
  #endif   /* FEATURE_USERZONE */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( network_sel_mode_pref_changed_ptr != NULL );
  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_PH );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate curr_nam */
  if(!INRANGE(ph_ptr->curr_nam, 0, CM_NAM_AUTO))
  {
    CM_ERR( "invalid nam",0,0,0);
    is_curr_nam_valid = FALSE;;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *network_sel_mode_pref_changed_ptr = FALSE;
  ph_cmd_ptr   = PH_CMD_PTR( cmd_ptr );
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  CM_FUNC_START( "cmph_client_cmd_check(), cmd=%d",ph_cmd_ptr->cmd,0,0 );

  /* If an error code was already detected while queuing the command, return
  ** this error code now.
  */
  if( cmd_info_ptr->cmd_err != CM_PH_CMD_ERR_NOERR )
  {
      return cmd_info_ptr->cmd_err;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether a specified phone command is allowed in the
  ** current state of the phone.
  */

  switch( ph_cmd_ptr->cmd )
  {

                       /*---------------------------*/
                       /* Change the operating mode */
                       /*---------------------------*/


    case CM_PH_CMD_OPRT_MODE:

      /* This trick is used to put phone into appropriate mode if there is a
      ** offline command during power up.
      ** If current operation mode is FTM mode, stay in FTM mode, otherwise goto
      ** offline
      */
      if( cmd_info_ptr->oprt_mode == SYS_OPRT_MODE_OFFLINE_IF_NOT_FTM )
      {
        if( ph_ptr->oprt_mode == SYS_OPRT_MODE_FTM )
        {
          cmd_info_ptr->oprt_mode = SYS_OPRT_MODE_FTM;
        }
        else
        {
          cmd_info_ptr->oprt_mode = SYS_OPRT_MODE_OFFLINE_CDMA;
        }

      }
      /* Check command parameter errors - stop any further processing if
      ** any errors found.
      */
      cmd_err = cmph_ph_cmd_oprt_mode_para_check( ph_cmd_ptr );

      if( cmd_err != CM_PH_CMD_ERR_NOERR )
      {
        break;
      }

      /* Only able to change the operating mode if this is
      ** a valid operating mode switch.
      */
      if ( ! is_valid_oprt_mode_switch(ph_ptr->oprt_mode,
                                       cmd_info_ptr->oprt_mode) )
      {
        cmd_err = CM_PH_CMD_ERR_OPRT_MODE_S;
      }

      /* When the ph is moving into LPM, check to make sure that all
      ** the active calls are ended.
      */
      if( cmd_info_ptr->oprt_mode == SYS_OPRT_MODE_LPM
          &&  cmd_err != CM_PH_CMD_ERR_OPRT_MODE_S)
      {
        /* ending all the active calls
        */
        cmcall_offline();
      }

      #ifdef FEATURE_PLT
#error code not present
      #endif /* FEATURE_PLT */

      #if defined(FEATURE_HDR) && defined(FEATURE_FULL_TIME_SHDR)
#error code not present
      #endif

      break;


    case CM_PH_CMD_WAKEUP_FROM_STANDBY:

      /* Check command parameter errors - stop any further processing if
      ** any errors found.
      */

      /* No parameter to check */

      /* Only able to change the operating mode if this is
      ** a valid operating mode switch.
      */
      if ( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
      {
        cmd_err = CM_PH_CMD_ERR_OPRT_MODE_S;
      }

      break;


    #if ( defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) )
    case CM_PH_CMD_CHANGE_RTRE_CONFIG:

      /* Check command parameter errors - stop any further processing if
      ** any errors found.
      */

      cmd_err = cmph_ph_cmd_change_rtre_config_para_check( ph_cmd_ptr );
      if( cmd_err != CM_PH_CMD_ERR_NOERR )
      {
        break;
      }

      if ( ph_ptr->curr_nam != (cm_nam_e_type)NV_RUIM_NAM )
      {
        cmd_err = CM_PH_CMD_ERR_CURRENT_NAM_IS_NOT_RUIM_NAM_S;
        break;
      }

      break;
    #endif /* defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                       /*---------------------------*/
                       /* Handle avoid sys command  */
                       /*---------------------------*/


    case CM_PH_CMD_AVOID_SYS:

      /* Check command parameter errors - stop any further processing if
      ** any errors found.
      */
      cmd_err = cmph_ph_cmd_avoid_sys_para_check( ph_cmd_ptr );

      if( cmd_err != CM_PH_CMD_ERR_NOERR )
      {
        break;
      }

      if( cmcall_count_on_ss(CM_SS_MAIN) > 0 ||
          cmcall_count_on_ss(CM_SS_HDR) >0 )
      {
        /* Fail the command if phone is in-use,
        */
        cmd_err = CM_PH_CMD_ERR_IN_USE_S;
        break;
      }

      /* Fail the command , if phone is not online
      */
      if (ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE)
      {
        cmd_err = CM_PH_CMD_ERR_ONLINE_S;
        break;
      }
      #ifdef FEATURE_USERZONE
      /* Fail the command, if cmd is SD_SS_AVOID_SYS_IDLE_IF_UZ
      ** when the MS is not in a user zone
      */
      if (cmd_info_ptr->avoid_type == CM_SS_AVOID_SYS_SRV_IF_UZ &&
          ss_ptr->info.mode_info.cdma_info.uz_id == SD_INVALID_UZID)
      {
        cmd_err = CM_PH_CMD_ERR_SYS_NOT_IN_UZ;
      }
      #endif /* FEATURE_USERZONE */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                  /*----------------------------------------*/
                  /* Change the Measurement mode preference */
                  /*----------------------------------------*/

    case CM_PH_CMD_MEAS_MODE:

      /* Only able to service this request if phone is ONLINE or LPM,
      */
      if(  ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE &&
           ph_ptr->oprt_mode != SYS_OPRT_MODE_LPM )
      {
        cmd_err = CM_PH_CMD_ERR_OPRT_MODE_S;
        break;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                  /*----------------------------------------*/
                  /* Change the System Selection preference */
                  /*----------------------------------------*/

    case CM_PH_CMD_SYS_SEL_PREF:

      /*- - - - - - - - - P R E - C O N D I T I O N S - - - - - - - - - - -*/

      /* Only able to service this request if phone is not powering-off,
      ** resetting or in LPM.
      ** However ,If measurement feature is enabled, allow pref change in LPM mode
      */
      if( !cmph_in_valid_oprt_mode( ph_ptr ) 
        #ifdef FEATURE_CM_SS_MEAS
#error code not present
        #endif
        )
      {
        cmd_err = CM_PH_CMD_ERR_OPRT_MODE_S;
        break;
      }

      /*- - - - - C H E C K - P A R A M E T E R S - - - - - - - - - - - - -*/

      /* Check command parameter errors - stop any further processing if
      ** any errors found.
      */

      cmd_err = cmph_ph_cmd_sys_sel_pref_para_check( ph_cmd_ptr );

      /* Check whether it's OK to proceed */
      if( cmd_err != CM_PH_CMD_ERR_NOERR)
      {
        break;
      }


      /*- - - - - C H E C K - S P L - C O N D I T I O N S - - - - - - - - -*/

      #if (defined FEATURE_UICC_RAT_INDICATOR_SUPPORT) && \
          ((defined FEATURE_WCDMA) || (defined FEATURE_GSM))
      /* check if RAT balancing feature is enabled .and client is not CM itself.
      */      
      /* Block user from changing mode_pref of the phone only if
      ** UIM_USIM_RAT file is present and NV_ENS_ENABLED_I is ON.
      */
      if ((ph_ptr->ens_value) && 
          (ph_ptr->is_uim_usim_rat_present == TRUE))
      {
        /* client is not CM itself */
        if (ph_cmd_ptr->client_id != CM_CLIENT_ID_ANONYMOUS)
        {
          /* if current mode preference is different from new mode preference
          * we should block this mode pref change as client is not supposed
          * to change mode_pref when RAT balancing feature is enabled.
          */

          /* 
          ** If EF-RAT present then,
          ** 1. Manual Selection, and user selected mode pref. is subset of 
          ** EF-RAT preferences, change mode pref. to user mode pref.
          ** Else ignore the command.
          ** 2. Automatic selection: If mode pref. change happens, and 
          ** last net sel mode is not Manual, ignore the command, else
          ** replace with EF-RAT mode pref.
          ** Manual to Manual (without user selection) -net_sel shld be no_chge
          **
          */
          
          /* Read EF-RAT */
          #if (defined (FEATURE_MMGSDI) && defined (FEATURE_CM_MMGSDI))
          cmmmgsdi_rat_mode_init(&cm_mode_pref);
          #else
          (void)cmgsdi_rat_mode_init(&new_rat_mode);
          switch (new_rat_mode)
          {
            case GSDI_RAT_MODE_PREF_AUTOMATIC:
              cm_mode_pref = CM_MODE_PREF_AUTOMATIC;
              break;

            case GSDI_RAT_MODE_PREF_GSM_ONLY:
              cm_mode_pref = CM_MODE_PREF_GSM_ONLY;
              break;

            case GSDI_RAT_MODE_PREF_WCDMA_ONLY:
              cm_mode_pref = CM_MODE_PREF_WCDMA_ONLY;
              break;

            default:
              /* RAT mode can't be other than above three.
              ** Some issue in RAT file read. Exit. 
              */
              cm_mode_pref = CM_MODE_PREF_NONE;
              MSG_ERROR("Cant access RAT mode from UIM_USIM_RAT : Unknown card mode: %x", \
              new_rat_mode, 0, 0);
          }
          #endif

          if(cmd_info_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
          {
            /* Since search is performed with EF-RAT preferences, 
            ** user mode pref should always be subset of EF-RAT.
            ** Below code is a guard code */
            if((cm_mode_pref == CM_MODE_PREF_GSM_ONLY ||
                 cm_mode_pref == CM_MODE_PREF_WCDMA_ONLY) &&
                 (cmd_info_ptr->mode_pref != cm_mode_pref &&
                  cmd_info_ptr->mode_pref != CM_MODE_PREF_NO_CHANGE))
                  
            {
              cmd_err = CM_PH_CMD_ERR_MODE_PREF_CHANGE_NOT_ALLOWED_S;
              CM_MSG_HIGH("mode_pref change not allowed from %d to %d as NV_ENS_ENABLED_I is ON \
                         and UIM_USIM_RAT is present", ph_ptr->mode_pref, cmd_info_ptr->mode_pref, 0);
              break;
            }
            
          }
          else if(cmd_info_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_AUTOMATIC &&
                     ph_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
          {
            /* Auto to Manual change */
            /* Force EF-RAT mode preferences */
            cmd_info_ptr->mode_pref = cm_mode_pref;
            CM_MSG_HIGH("Using EF-RAT %d", cm_mode_pref, 0, 0);
          }
          else
          {
            /* Automatic to Automatic
            ** No network sel mode changed
            ** etc
            */
            if ((cmd_info_ptr->mode_pref != CM_MODE_PREF_NO_CHANGE) &&
              (ph_ptr->mode_pref != cmd_info_ptr->mode_pref))
            {       
              cmd_err = CM_PH_CMD_ERR_MODE_PREF_CHANGE_NOT_ALLOWED_S;
              CM_MSG_HIGH("mode_pref change not allowed from %d to %d as NV_ENS_ENABLED_I is ON \
                         and UIM_USIM_RAT is present", ph_ptr->mode_pref, cmd_info_ptr->mode_pref, 0);
              break;                   
            }
          }
        }
      }
      #endif /* (FEATURE_UICC_RAT_INDICATOR_SUPPORT) && ((WCDMA) || (GSM)) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_GPSONE
#error code not present
      #endif

      /*- - - - - - - U P D A T E - P R E F - V A L U E S - - - - - - - - -*/

      /* Initialize cm_mode_pref variable */
      cm_mode_pref = cmd_info_ptr->mode_pref;

      switch(cmd_info_ptr->mode_pref)
      {
        /*
        ** If client wishes to remove HDR from current mode preference,
        ** do it here now
        */
        case CM_MODE_PREF_CURRENT_LESS_HDR:
          cm_mode_pref = cm_remove_mode_pref_components(cmph_ptr()->mode_pref,
                                                        1,
                                                        SD_SS_MODE_PREF_HDR );
          break;

        /* If client wishes to remove AMPS from current mode preference,
        ** do it here now
        */
        case CM_MODE_PREF_CURRENT_LESS_AMPS:
          cm_mode_pref = cm_remove_mode_pref_components(cmph_ptr()->mode_pref,
                                                        1,
                                                        SD_SS_MODE_PREF_AMPS );
          break;

        /* If client wishes to remove HDR and AMPS from current mode preference,
        ** do it here now
        */
        case CM_MODE_PREF_CURRENT_LESS_HDR_AND_AMPS:
          cm_mode_pref = cm_remove_mode_pref_components( cmph_ptr()->mode_pref,
                                                         2,
                                                         SD_SS_MODE_PREF_HDR,
                                                         SD_SS_MODE_PREF_AMPS );
          break;

        case CM_MODE_PREF_CURRENT_PLUS_WLAN:
          cm_mode_pref = cm_add_mode_pref_components( cmph_ptr()->mode_pref,
                                                      1,
                                                      SD_SS_MODE_PREF_WLAN);
          break;

        case CM_MODE_PREF_CURRENT_LESS_WLAN:
          cm_mode_pref = cm_remove_mode_pref_components( cmph_ptr()->mode_pref,
                                                         1,
                                                         SD_SS_MODE_PREF_WLAN);

          break;

        /* Any other mode preference value, no processeing required for now */
        default:
          break;
      }

      if( cm_mode_pref == CM_MODE_PREF_MAX )
      {
        cmd_err = CM_PH_CMD_ERR_MODE_PREF_P;
        CM_DBG_ERR_FATAL( "Not able to update mode pref %d with %d",
                          cmph_ptr()->mode_pref, cmd_info_ptr->mode_pref,0 );
      }
      else
      {
        cmd_info_ptr->mode_pref = cm_mode_pref;
      }

      /* If client does not want to change the mode preference, use the
      ** existing mode preference. Else, if client wants to restore the
      ** persistent mode preference, use the persistent mode preference.
      */
      if( cmd_info_ptr->mode_pref == CM_MODE_PREF_NO_CHANGE )
      {
        cmd_info_ptr->mode_pref = ph_ptr->mode_pref;
      }
      else if( cmd_info_ptr->mode_pref == CM_MODE_PREF_PERSISTENT
               && is_curr_nam_valid )
      {
        cmd_info_ptr->mode_pref = CMPH_PRST_MODE_PREF( ph_ptr );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If client does not want to change the band preference, use the
      ** current band preference. Else, if client wants to restore the
      ** persistent band preference, use the persistent band preference.
      */
      if( cmd_info_ptr->band_pref == CM_BAND_PREF_NO_CHANGE )
      {
        cmd_info_ptr->band_pref = ph_ptr->band_pref;
      }
      else if( cmd_info_ptr->band_pref == CM_BAND_PREF_PERSISTENT
               && is_curr_nam_valid )
      {
        cmd_info_ptr->band_pref = CMPH_PRST_BAND_PREF( ph_ptr );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If client does not want to change the roam preference, use the
      ** current roam preference. Else, if client wants to restore the
      ** persistent roam preference, use the persistent roam preference.
      */
      if( cmd_info_ptr->roam_pref == CM_ROAM_PREF_NO_CHANGE )
      {
        cmd_info_ptr->roam_pref = ph_ptr->roam_pref;
      }
      else if( cmd_info_ptr->roam_pref == CM_ROAM_PREF_PERSISTENT
               && is_curr_nam_valid )
      {
        cmd_info_ptr->roam_pref = CMPH_PRST_ROAM_PREF( ph_ptr );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If client does not want to change the hybr preference, use the
      ** current hybrid preference. Else, if client wants to restore the
      ** persistent hybrid preference, use the persistent hyrbid preference.
      */
      if( cmd_info_ptr->hybr_pref == CM_HYBR_PREF_NO_CHANGE )
      {
        cmd_info_ptr->hybr_pref = ph_ptr->hybr_pref;
      }
      else if( cmd_info_ptr->hybr_pref == CM_HYBR_PREF_PERSISTENT
               && is_curr_nam_valid )
      {
        cmd_info_ptr->hybr_pref = CMPH_PRST_HYBR_PREF( ph_ptr );
      }

      #ifdef FEATURE_HDR_HYBRID_DISABLE
#error code not present
      #endif

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If client does not want to change the service domain preference,
      ** use the current service domain preference.
      */
      if( cmd_info_ptr->srv_domain_pref == CM_SRV_DOMAIN_PREF_NO_CHANGE )
      {
        /* cmd_info_ptr->srv_domain_pref = ph_ptr->srv_domain_pref;
        **
        ** cmd_info_ptr is typically updated so that the value can be
        ** used to update NV in mmll_reply. srv_domain_pref between ph_ptr
        ** and NV can be different during PS attach on demand call, PS attach
        ** and PS detach scenarios, so copying srv_domain_pref from ph_ptr
        ** to cmd_info_ptr and then writing to NV will erase correct NV value
        **
        **   - Phone powers us with NV-CS_PS, Ph_ptr-CS_PS
        **   - PS_DETACH is issued  NV-CS_PS, Ph_ptr-CS_ONLY
        **   - Manual net sel is recvd with cmd_info_ptr->srv.. as NO_CHANGE
        **        if cmd_info_ptr->srv.. is copied from ph_ptr and later
        **        written to NV, NV will end up with CS_ONLY.
        **   Our design has been to not update NV with PS_ATTACH, PS_DETACH
        **   requests.
        */

      }
      else if( cmd_info_ptr->srv_domain_pref == CM_SRV_DOMAIN_PREF_PERSISTENT
               && is_curr_nam_valid )
      {
        cmd_info_ptr->srv_domain_pref = CMPH_PRST_SRV_DOMAIN_PREF( ph_ptr );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If client does not want to change the network selection mode
      ** preference, use the current network selection mode preference.
      */
      if( cmd_info_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_NO_CHANGE )
      {
        cmd_info_ptr->network_sel_mode_pref = ph_ptr->network_sel_mode_pref;
      }
      else if( cmd_info_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_PERSISTENT
               && is_curr_nam_valid )
      {
        cmd_info_ptr->network_sel_mode_pref = CMPH_PRST_NETWORK_SEL_MODE_PREF( ph_ptr );
        cmd_info_ptr->pref_term = CM_PREF_TERM_PERMANENT;
        if ( cmd_info_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL )
        {
          /* In manual mode, set to TRUE.
          */
          *network_sel_mode_pref_changed_ptr = TRUE;
        }
      }

      /* Check if it is changed from manual to automatic, then we should still be on RPLMN.
      ** so do not set the network_sel_mode_pref_changed_ptr to TRUE
      */
      else if (!(cmd_info_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_AUTOMATIC &&
                 ph_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL
                )
              )
      {
        /* No, it is not switch from manual -> auto.
        */
        *network_sel_mode_pref_changed_ptr = TRUE;
      }


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If client does not want to change the acquisition order preference,
      ** use the current network selection mode preference.  Else, if client
      ** wants to restore the persistent acq order preference, use the
      ** persistent acq order preference.
      */
      if( cmd_info_ptr->acq_order_pref == CM_GW_ACQ_ORDER_PREF_NO_CHANGE )
      {
        cmd_info_ptr->acq_order_pref = ph_ptr->acq_order_pref;
      }
      else if( cmd_info_ptr->acq_order_pref == CM_GW_ACQ_ORDER_PREF_PERSISTENT
               && is_curr_nam_valid )
      {
        cmd_info_ptr->acq_order_pref = CMPH_PRST_ACQ_ORDER_PREF( ph_ptr );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If client does not want to change the prl preference,
      ** use the current prl preference.
      */
      if( cmd_info_ptr->prl_pref == CM_PRL_PREF_NO_CHANGE )
      {
        cmd_info_ptr->prl_pref = ph_ptr->prl_pref;
      }
      else if( cmd_info_ptr->prl_pref == CM_PRL_PREF_PERSISTENT
               && is_curr_nam_valid )
      {
        cmd_info_ptr->prl_pref = CMPH_PRST_PRL_PREF( ph_ptr );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if(!cmd_info_ptr->is_wlan_pref_included)
      {
        cmd_info_ptr->wlan_pref = ph_ptr->wlan_pref;
      }
      else
      {
        /* If client does not want to change the WLAN network selection mode
        ** preference, use the current WLAN network selection mode preference.
        */
        if( cmd_info_ptr->wlan_pref.network_sel_mode_pref ==
                                         CM_NETWORK_SEL_MODE_PREF_NO_CHANGE )
        {
          cmd_info_ptr->wlan_pref.network_sel_mode_pref =
                                     ph_ptr->wlan_pref.network_sel_mode_pref;
        }

        /* If client does not want to change the WLAN scan mode
        ** preference, use the current WLAN scan mode preference.
        */

        if( cmd_info_ptr->wlan_pref.scan_mode == CM_WLAN_SCAN_PREF_NO_CHANGE )
        {
          cmd_info_ptr->wlan_pref.scan_mode = ph_ptr->wlan_pref.scan_mode;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If client does not want to change the WLAN technology preference
        ** preference, use the current WLAN technology preference.
        */

        if( cmd_info_ptr->wlan_pref.tech_pref == CM_WLAN_TECH_PREF_NO_CHANGE )
        {
          cmd_info_ptr->wlan_pref.tech_pref = ph_ptr->wlan_pref.tech_pref;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        #ifdef FEATURE_WLAN_ADHOC
#error code not present
        #else
        cmd_info_ptr->wlan_pref.bss_type_pref = CM_WLAN_BSS_TYPE_PREF_ANY;
        #endif /* FEATURE_WLAN_ADHOC */

      } /* cmd_info_ptr->is_wlan_pref_included */


      /*- - - - - - V A L I D A T E - P R E F - V A L U E S - - - - - - - -*/

      /* Only able to service this request if this is valid mode and band
      ** preference setting for this target/PRL.
      */
      if( ! cmph_is_valid_mode_band_pref( cmd_info_ptr->mode_pref,
                                          cmd_info_ptr->band_pref) )
      {
        cmd_err = CM_PH_CMD_ERR_SYS_SEL_PREF_S;
        break;
      }

      /* Only able to service this request if this is valid
      ** mode and hybrid preference 
      */
      if(!cmph_is_valid_mode_hybrid_pref( cmd_info_ptr->mode_pref,
                                          cmd_info_ptr->hybr_pref)) {

        cmd_err = CM_PH_CMD_ERR_HYBR_PREF_S;
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Only able to service this request if this is valid mode and network 
      ** selection preference setting for this target/PRL.
      ** - GW Manual network selection is only valid w/ GSM/WCDMA mode pref.
      ** - WLAN Manual network selection is only valid w/ WLAN mode pref.
      **
      ** NOTE: Validate only if the network selection pref is changed in this
      ** command. If, network selection mode pref is no change then send
      ** NO_CHANGE so that it is not validated.
      */
      if (!cmph_is_valid_network_selection_mode_pref( cmd_info_ptr->mode_pref,
           ((ph_ptr->network_sel_mode_pref == cmd_info_ptr->network_sel_mode_pref)?
            CM_NETWORK_SEL_MODE_PREF_NO_CHANGE:cmd_info_ptr->network_sel_mode_pref),
            ((ph_ptr->wlan_pref.network_sel_mode_pref == cmd_info_ptr->wlan_pref.network_sel_mode_pref)?
            CM_NETWORK_SEL_MODE_PREF_NO_CHANGE:cmd_info_ptr->wlan_pref.network_sel_mode_pref)))
      {
        cmd_err = CM_PH_CMD_ERR_SYS_SEL_PREF_S;
        CM_MSG_HIGH ("Cannot set mode pref when network selection is manual.", 0, 0, 0);
        break;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_DDTM_CNTL
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                  /*-----------------------------------------*/
                  /* Change the DDTM preference            */
                  /*-----------------------------------------*/

    case CM_PH_CMD_DDTM_PREF:


      /* Check errors.
      */
      cmd_err = cmph_client_cmd_ddtm_pref_para_check( ph_cmd_ptr, ph_ptr );
      break;
    #endif /* FEATURE_DDTM_CNTL */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                    /*---------------------------------*/
                    /* Change the answer voice setting */
                    /*---------------------------------*/

    case CM_PH_CMD_ANSWER_VOICE:

      /* check command parameter errors */
      cmd_err = cmph_ph_cmd_answer_voice_para_check(ph_cmd_ptr);

      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                        /*--------------------------*/
                        /* Change the NAM selection */
                        /*--------------------------*/

    case CM_PH_CMD_NAM_SEL:

      /* Check command parameter errors - stop any further processing if
      ** any errors found.
      */
      cmd_err = cmph_ph_cmd_nam_sel_para_check( ph_cmd_ptr );
      if( cmd_err != CM_PH_CMD_ERR_NOERR )
      {
        break;
      }

      if( cmcall_count_on_ss(CM_SS_MAIN) > 0 ||
          cmcall_count_on_ss(CM_SS_HDR) > 0 )
      {
        /* Fail the command if phone is in-use,
        */
        cmd_err = CM_PH_CMD_ERR_IN_USE_S;
        break;
      }

      /* Only able to service this request if phone is not powering-off,
      ** resetting or in LPM.
      */
      if( !cmph_in_valid_oprt_mode( ph_ptr ) )
      {
        cmd_err = CM_PH_CMD_ERR_OPRT_MODE_S;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /*------------------------------*/
                      /* A CM Client activated itself */
                      /*------------------------------*/

    case CM_PH_CMD_CLIENT_ACT:

       /* check client ID */
      if( ph_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE )
      {
        cmd_err  = CM_PH_CMD_ERR_CLIENT_ID_P;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*------------------------------------*/
                   /* Get a snapshot of phone information */
                   /*------------------------------------*/

    case CM_PH_CMD_INFO_GET:
      /* check client ID */
      if( ph_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE )
      {
        cmd_err  = CM_PH_CMD_ERR_CLIENT_ID_P;
      }
      else if ((ph_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK)
      {
        cmd_err  = CM_PH_CMD_ERR_CLIENT_ID_P;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*----------------------------------*/
                   /* Set RSSI change delta threshold  */
                   /* send start full service (WCDMA)  */
                   /*----------------------------------*/

    case CM_PH_CMD_RSSI_DELTA:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*-------------------------------------------*/
                   /* Set RSSI,ECIO,IO  change delta threshold  */
                   /* send start full service (WCDMA)           */
                   /*-------------------------------------------*/

    case CM_PH_CMD_SIGNAL_STRENGTH_DELTA:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    case CM_PH_CMD_PACKET_STATE:

      /* check command parameter errors */

      if((cmd_err = cmph_ph_cmd_packet_state_para_check(ph_cmd_ptr)) != CM_PH_CMD_ERR_NOERR)
      {
        /* stop further processing if any errors found in the command */
        break;
      }

      /* Only able to service this request if phone is not powering-off,
      ** resetting or in LPM.
      */
      if( !cmph_in_valid_oprt_mode( ph_ptr ) )
      {
        cmd_err = CM_PH_CMD_ERR_OPRT_MODE_S;
      }

      break;
    #endif /*  (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */


    #ifdef FEATURE_JCDMA
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*----------------------------------*/
                   /* Enable/Disable voice activity    */
                   /*----------------------------------*/

    case CM_PH_CMD_VOC_ACTIVITY:

      break;

    #endif /* FEATURE_JCDMA */

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*------------------------*/
                   /* Subscription available */
                   /*------------------------*/
    case CM_PH_CMD_SUBSCRIPTION_AVAILABLE:
      cmd_err = cmph_cmd_subscription_available_cmd_para_check( ph_ptr,
                                                                cmd_info_ptr
                                                              );
      break;

                   /*----------------------------*/
                   /* Subscription not available */
                   /*----------------------------*/
    case CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE:
      cmd_err = cmph_cmd_subscription_not_available_cmd_para_check( ph_ptr,
                                                                    cmd_info_ptr
                                                                  );
      break;

                   /*----------------------------*/
                   /* Subscription changed       */
                   /*----------------------------*/
    case CM_PH_CMD_SUBSCRIPTION_CHANGED:
      cmd_err = cmph_cmd_subscription_changed_cmd_para_check( ph_ptr,
                                                              cmd_info_ptr
                                                            );
      break;



      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*-----------------------------------------*/
                   /* Client has requested a list of networks */
                   /*-----------------------------------------*/

    case CM_PH_CMD_GET_NETWORKS:

      /*- - - - - - - - - P R E - C O N D I T I O N S - - - - - - - - - - -*/

      /* Only able to service this request if phone is not powering-off,
      ** resetting or in LPM.
      */
      if ( !cmph_in_valid_oprt_mode( ph_ptr ) )
      {
        cmd_err = CM_PH_CMD_ERR_OPRT_MODE_S;
      }

      /*- - - - - C H E C K - P A R A M E T E R S - - - - - - - - - - - - -*/

      cmd_err = cmph_cmd_get_networks_para_check( ph_ptr, ph_cmd_ptr );
      break;

    case CM_PH_CMD_TERMINATE_GET_NETWORKS:
      cmd_err = cmph_cmd_terminate_get_networks_para_check( ph_ptr);
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*----------------------------------------------------*/
                   /* Client has requested WLAN security cred. using WPS */
                   /*----------------------------------------------------*/

      case CM_PH_CMD_WPS_START:
      cmd_err = cmph_cmd_start_wps_para_check( ph_ptr, ph_cmd_ptr );
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CM_PH_CMD_WPS_ABORT:
      cmd_err = cmph_cmd_abort_wps_para_check( ph_ptr);
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*-----------------------------------------------------*/
                   /* Client has requested to save the preferred networks */
                   /*-----------------------------------------------------*/
    #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
    #endif /* FEATURE_WCDMA, FEATURE_GSM */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
    #endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
   #endif

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PH_CMD_CLIENT_RELEASE:

      /* Nothing to check. */

      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      cmd_err = CM_PH_CMD_ERR_OTHER;
      CM_SWITCH_ERR( "Bad phone command %d", ph_cmd_ptr->cmd, 0, 0);
      break;

  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return cmd_err;

} /* cmph_client_cmd_check() */
/*lint +esym(818,ph_ptr) */

/*===========================================================================

FUNCTION cmph_client_cmd_err

DESCRIPTION
  Notify clients of a specified phone command error.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmph_client_cmd_err(

    const cm_cmd_s_type     *cmd_ptr,
        /* Pointer to a CM command */

    cm_ph_cmd_err_e_type    cmd_err
        /* Indicate the phone command error */
)
{
  const cm_ph_cmd_s_type    *ph_cmd_ptr =   NULL;
      /* Point at phone command component */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_PH );
  CM_ASSERT( BETWEEN( cmd_err, CM_PH_CMD_ERR_NONE, CM_PH_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_cmd_ptr =   PH_CMD_PTR( cmd_ptr );
  CM_FUNC_START( "cmph_client_cmd_err() ph_cmd=%d, cmd_err=%d, client=%p",
                 ph_cmd_ptr->cmd, cmd_err, ph_cmd_ptr->client_id );

  #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
  #endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */


  /* If commanding client supplied a callback function,
  ** notify this client of command error status.
  */
  if( ph_cmd_ptr->cmd_cb_func != NULL )
  {
    ph_cmd_ptr->cmd_cb_func( ph_cmd_ptr->data_block_ptr,
                             ph_cmd_ptr->cmd,
                             cmd_err );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command error is OTHER than NO ERROR,
  ** notify the client list of command error, as well.
  */
  if( cmd_err != CM_PH_CMD_ERR_NOERR )
  {
    CM_MSG_HIGH( "Phone cmd err!, cmd=%d, err=%d, client=%ld",
                 ph_cmd_ptr->cmd, cmd_err, ph_cmd_ptr->client_id );

    cmclient_list_ph_cmd_err_ntfy( cmd_err, ph_cmd_ptr );
  }
} /* cmph_client_cmd_err() */


#ifdef FEATURE_CM_SS_MEAS
#error code not present
#endif /* FEATURE_CM_SS_MEAS */

/*===========================================================================

FUNCTION cmph_read_nv_items

DESCRIPTION
  Read items from NV.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmph_read_nv_items(cmph_s_type *ph_ptr)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (!nv_rtre_use_ruim_for_nam((byte) ph_ptr->curr_nam))
  #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
   {
   /* Read from NV the SPC and remember whether the SPC is all zeros.
   */

   cmnv_read_wait( NV_SEC_CODE_I, cmnv_item_ptr );
   {
     byte zeros[NV_SEC_CODE_SIZE];

     if( memcmp((byte*) cmnv_item_ptr->sec_code.digits,
                memset( zeros, '0', NV_SEC_CODE_SIZE ),
                NV_SEC_CODE_SIZE ) == 0 )
     {
       ph_ptr->is_spc_zero = TRUE;
     }
     else
     {
       ph_ptr->is_spc_zero = FALSE;
     }
   }
  }

  cmnv_item_ptr->accolc.nam = (byte) ph_ptr->curr_nam;
  cmnv_read_wait(NV_ACCOLC_I, cmnv_item_ptr);
  ph_ptr->accolc = cmnv_item_ptr->accolc.ACCOLCpClass[NV_CDMA_MIN_INDEX];

  #endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

} /* cmph_read_nv_items() */



/*===========================================================================

FUNCTION cmph_orig_mode_reset_on_ss

DESCRIPTION
  Reset the origination mode to none.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void cmph_orig_mode_reset_on_ss(

  cmph_s_type *ph_ptr,
    /* ph_ptr
    */

  cm_ss_e_type ss,
    /* The ss on which to reset
    */

  boolean      force_top
    /* Force the top preferences
    */
)
{

  boolean is_top_ptr_changed = FALSE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_ptr != NULL);
  CM_ASSERT( ss != CM_SS_MAX );
  CM_MSG_HIGH( "cmph_orig_mode_reset_on_ss() ss %d",ss,0,0 );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmtask_orig_para_reset( ss );

  is_top_ptr_changed = cmph_insert_orig_mode (
                                  ss,
                                  SD_SS_ORIG_MODE_NONE,
                                  CM_ACT_TYPE_PH_OBJ,
                                  ph_ptr->pref_term,
                                  ph_ptr->mode_pref,
                                  ph_ptr->band_pref,
                                  ph_ptr->prl_pref,
                                  ph_ptr->roam_pref,
                                  ph_ptr->hybr_pref,
                                  ph_ptr->int_hybr_pref,
                                  &ph_ptr->wlan_pref,
                                  ph_ptr->srv_domain_pref,
                                  (cm_act_id_type)ph_ptr,
                                  CM_ACT_UPDATE_REAS_ACT_START
                                );

    if( force_top && is_top_ptr_changed )
    {
      cmph_force_orig_mode_on_the_fly(ss);
    }
}


/*===========================================================================

FUNCTION cmph_notify_mmoc_no_sim

DESCRIPTION
  After an emergency call has ended, we must notify MMOC the
  SIM is missing or failed.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_notify_mmoc_no_sim ( void )
{
  cmph_s_type  *ph_ptr = cmph_ptr();

  sd_ss_orig_mode_e_type orig_mode;

  sd_ss_mode_pref_e_type mode_pref;

  sd_ss_srv_domain_pref_e_type srv_domain_pref;
  sd_ss_acq_order_pref_e_type  acq_order_pref;

  sd_ss_tech_pref_e_type      wlan_tech_pref;

  #ifdef FEATURE_WLAN_ADHOC
#error code not present
  #endif

  sys_wlan_scan_pref_e_type   wlan_scan_pref;

  sys_manual_sys_info_s_type  sys_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate nam */
  if(!INRANGE(ph_ptr->curr_nam, 0, CM_NAM_AUTO))
  {
    CM_ERR( "invlid nam: %d", ph_ptr->curr_nam,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  orig_mode = cmph_map_cm_network_sel_mode_pref_to_orig_mode(
                                                 CMPH_PRST_NETWORK_SEL_MODE_PREF( ph_ptr ));

  mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(
                                       ph_ptr->prst_mode_pref[ph_ptr->curr_nam]);

  srv_domain_pref = cmph_map_cm_srv_domain_pref_to_sd(
                                                   CMPH_PRST_SRV_DOMAIN_PREF( ph_ptr));
  acq_order_pref = cmph_map_cm_acq_order_pref_to_sd(
                                                  CMPH_PRST_ACQ_ORDER_PREF( ph_ptr )  );

  wlan_tech_pref = cmph_map_cm_wlan_tech_pref_to_sd_pref(
                                        CMPH_PRST_WLAN_TECH_PREF( ph_ptr ));

  #ifdef FEATURE_WLAN_ADHOC
#error code not present
  #endif

  wlan_scan_pref = cmph_map_cm_wlan_scan_pref_to_sys_pref(
                                         ph_ptr->prst_wlan_pref[ph_ptr->curr_nam].scan_mode);

  sys_id.sys_id.id_type  = SYS_SYS_ID_TYPE_UMTS;
  sys_id.sys_id.id.plmn  = ph_ptr->plmn;
  sys_id.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;

  /* ADD_PROTO with both CDMA and GW makes both protocol stacks reload
  ** NV information
  */
  #ifdef FEATURE_WLAN_ADHOC
#error code not present
  #else
  mmoc_cmd_subscription_chgd(
                                MMOC_SUBSC_CHG_SUBSC_AVAIL,
                                (byte) ph_ptr->curr_nam,
                                orig_mode,
                                mode_pref,
                                cmph_map_cm_band_pref_to_sd_band_pref(ph_ptr->prst_band_pref[ph_ptr->curr_nam]),
                                cmph_map_cm_prl_pref_to_sd_prl_pref(ph_ptr->prst_prl_pref[ph_ptr->curr_nam]),
                                cmph_map_cm_roam_pref_to_sd_roam_pref(ph_ptr->prst_roam_pref[ph_ptr->curr_nam]),
                                SD_SS_HYBR_PREF_NONE,
                                &sys_id,
                                srv_domain_pref,
                                acq_order_pref,
                                wlan_tech_pref,
                                wlan_scan_pref,
                                ph_ptr->is_gw_subscription_available,
                                ph_ptr->is_cdma_subscription_available,
                                ph_ptr->is_wlan_subscription_available,
                                ADD_PROTO (PROT_SUBSC_CHG_CDMA,
                                           PROT_SUBSC_CHG_GW)
                              );
  #endif
  }

/*===========================================================================

FUNCTION cmph_proc_online_mode_oper

DESCRIPTION
  Performs following on moving to online mode

   - Sets ps_srv_req_sent flag to record that PS attach has been requested
   - Moving from LPM to online should generate

       IF GW is available     :- GW not available followed by ADD_PROTO
                                 (PROT_SUBSC_CHG_CDMA, PROT_SUBSC_CHG_GW)
                                 status
       IF GW is not available :- ADD_PROTO (PROT_SUBSC_CHG_CDMA,
                                 PROT_SUBSC_CHG_GW) status


DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void  cmph_proc_online_mode_oper (

  cmph_s_type *ph_ptr
    /* Pointer to Ph obj */

)
{
  cm_mode_pref_e_type fav_mode_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate curr_nam */
  if(!INRANGE(ph_ptr->curr_nam, 0, CM_NAM_AUTO))
  {
    CM_ERR( "curr_nam is invalid!!!",0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** ONLINE transition would trigger PS_ATTACH procedure. We need to start
  ** PS_SIG_DOWN timer when PS service is acquired.
  **
  ** Set is_ps_Srv_req_sent flag to true now so that when we receive PS
  ** service acquired indication, we can start the PS_SIG_DOWN timer.
  */
  if((ph_ptr->prst_srv_domain_pref[ph_ptr->curr_nam]
                                 == CM_SRV_DOMAIN_PREF_PS_ONLY) ||
     (ph_ptr->prst_srv_domain_pref[ph_ptr->curr_nam]
                                 == CM_SRV_DOMAIN_PREF_CS_PS))
  {
     ph_ptr->is_ps_srv_req_sent = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If not transitioning from FTM or LPM mode return
  */
  if (! (ph_ptr->oprt_mode == SYS_OPRT_MODE_FTM ||
         ph_ptr->oprt_mode == SYS_OPRT_MODE_LPM )
     )
  {
    return;
  }


  /* Transitioning from FTM to ONLINE or LPM to ONLINE.
  */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  #if (defined(FEATURE_OTASP) || defined (FEATURE_UIM_RUIM))
  diag_otasp_update();              /* Tell Diag to update its cache */
  #endif /* FEATURE_OTASP || FEATURE_UIM_RUIM */
  #endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

  /* Code to send subscription not available followed by subscription
  ** allowed for GW was added to handle LPM to ONLINE transition. Earlier
  ** stop mode reason for LPM was issued as MODE_CHANGE. With current
  ** approach of issuing stop mode reason as LPM, this
  ** is not required but it is still good to have.
  */
  if ( (ph_ptr->is_gw_subscription_available) &&
       (ph_ptr->oprt_mode == SYS_OPRT_MODE_LPM)
     )
  {
    cmpref_proc_cmd_subsc_chgd(
      MMOC_SUBSC_CHG_SUBSC_AVAIL,
      (byte) ph_ptr->curr_nam,
      cmph_map_cm_network_sel_mode_pref_to_orig_mode(CMPH_PRST_NETWORK_SEL_MODE_PREF( ph_ptr )),
      cmph_map_cm_mode_pref_to_sd_mode_pref(ph_ptr->prst_mode_pref[ph_ptr->curr_nam]),
      cmph_map_cm_band_pref_to_sd_band_pref(ph_ptr->prst_band_pref[ph_ptr->curr_nam]),
      (sd_ss_prl_pref_e_type)(ph_ptr->prst_prl_pref[ph_ptr->curr_nam]),
      (sd_ss_roam_pref_e_type)(ph_ptr->prst_roam_pref[ph_ptr->curr_nam]),
      cmph_map_cm_hybr_pref_to_sd_hybr_pref(ph_ptr->prst_hybr_pref),
      ph_ptr->plmn,
      cmph_map_cm_srv_domain_pref_to_sd(CMPH_PRST_SRV_DOMAIN_PREF( ph_ptr)),
      cmph_map_cm_acq_order_pref_to_sd(CMPH_PRST_ACQ_ORDER_PREF( ph_ptr )  ),
      FALSE, /* GW subscription is not available */
      ph_ptr->is_cdma_subscription_available,
      ph_ptr->is_wlan_subscription_available,
      &ph_ptr->wlan_pref,
      PROT_SUBSC_CHG_GW
    );

  }

  fav_mode_pref = ph_ptr->prst_mode_pref[ph_ptr->curr_nam];

  /* For RAT balancing feature: During LPM, subscription available/change might have
  ** happened. But in LPM, mode_change is not allowed in CM.
  ** So, when moving to ONLINE, read EF_RAT file and force the mode_pref of
  ** EF_RAT, only if GW subscription is available
  */

  #if ((defined FEATURE_UICC_RAT_INDICATOR_SUPPORT) && \
      ((defined FEATURE_WCDMA) || (defined FEATURE_GSM)))

  if (ph_ptr->ens_value == TRUE && 
      ph_ptr->is_gw_subscription_available == TRUE) 
  {
    cmmmgsdi_rat_mode_init( &fav_mode_pref );
    
    if (fav_mode_pref == CM_MODE_PREF_NONE)
    {
      /* Could not read EF_RAT. Force persistent mode_pref only
      */
      fav_mode_pref = ph_ptr->prst_mode_pref[ph_ptr->curr_nam];
      CM_MSG_HIGH("Forcing persistent_mode_pref %d as EF_RAT could not be read",\
                   fav_mode_pref,0,0);
    }
    else
    {
      CM_MSG_MED("Reading EF_RAT while moving from LPM to ONLINE",0,0,0);
      cmmmgsdi_update_rat_mode( CM_CLIENT_ID_ANONYMOUS,
                                cmph_map_cm_mode_pref_to_ef_rat_mode(fav_mode_pref));   
    } 
    CM_MSG_HIGH ("Phone oprt mode change from %d to ONLINE. Forcing mode_pref: %d",\
                    ph_ptr->oprt_mode, fav_mode_pref, 0);
    
  }
  else /* GW subscription is not available */
  {
    CM_MSG_HIGH ("Moving from %d to ONLINE. ens: %d gw_sub: %d", \
                  ph_ptr->oprt_mode, ph_ptr->ens_value, 
                  ph_ptr->is_gw_subscription_available);
    CM_MSG_HIGH ("EF_RAT mode_pref will not be used. Using persistent mode_pref : %d", \
                  fav_mode_pref, 0, 0);
  }
	
  #endif


  /* ADD_PROTO with both CDMA and GW makes both protocol stacks
  ** reload NV information
  */
  cmpref_proc_cmd_subsc_chgd(
    MMOC_SUBSC_CHG_SUBSC_AVAIL,
    (byte) ph_ptr->curr_nam,
    cmph_map_cm_network_sel_mode_pref_to_orig_mode(CMPH_PRST_NETWORK_SEL_MODE_PREF( ph_ptr )),
    cmph_map_cm_mode_pref_to_sd_mode_pref(fav_mode_pref),
    cmph_map_cm_band_pref_to_sd_band_pref(ph_ptr->prst_band_pref[ph_ptr->curr_nam]),
    (sd_ss_prl_pref_e_type)(ph_ptr->prst_prl_pref[ph_ptr->curr_nam]),
    (sd_ss_roam_pref_e_type)(ph_ptr->prst_roam_pref[ph_ptr->curr_nam]),
    cmph_map_cm_hybr_pref_to_sd_hybr_pref(ph_ptr->prst_hybr_pref),
    ph_ptr->plmn,
    cmph_map_cm_srv_domain_pref_to_sd(CMPH_PRST_SRV_DOMAIN_PREF( ph_ptr)),
    cmph_map_cm_acq_order_pref_to_sd(CMPH_PRST_ACQ_ORDER_PREF( ph_ptr )  ),
    ph_ptr->is_gw_subscription_available,
    ph_ptr->is_cdma_subscription_available,
    ph_ptr->is_wlan_subscription_available,
    &ph_ptr->wlan_pref,
    ADD_PROTO (PROT_SUBSC_CHG_CDMA,PROT_SUBSC_CHG_GW)
    );

  /* If transitioning from LPM to ONLINE and FEATURE_CM_SS_MEAS is enabled,
  ** force the phone preferences because a mode pref change may have been allowed
  ** while in LPM mode
  */
  #ifdef FEATURE_CM_SS_MEAS
#error code not present
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* cmph_proc_online_mode_oper () */

/*===========================================================================

FUNCTION cmph_client_cmd_forward_mmll

DESCRIPTION
  Forward a client phone commands to mmoc/ll.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  TRUE if CM needs to wait for a replay from LL before continuing
  processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmph_client_cmd_forward_mmll(

    cmph_s_type      *ph_ptr,
        /* Pointer to a phone object */

    cm_cmd_s_type      *cmd_ptr,
        /* Pointer to a CM command */

    boolean             network_sel_mode_pref_changed
        /* = TRUE if client changed the network sel mode pref */
)
{
  cm_ph_cmd_s_type         *ph_cmd_ptr      =   NULL;
    /* Point at phone command component */

  cm_ph_cmd_info_s_type    *cmd_info_ptr    = NULL;
      /* Point at command information component */

  cm_srv_domain_pref_e_type   client_req_srv_domain_pref =
                                           CM_SRV_DOMAIN_PREF_NONE;
    /* Srv domain requested by client by sys_sel_pref cmd */

  cm_srv_domain_pref_e_type   srv_domain_tobe_forced =
                                           CM_SRV_DOMAIN_PREF_NONE;
    /* srv domain to be forced on sd */

  #ifdef FEATURE_OOSC_USER_ACTION
#error code not present
  #endif

  #if defined FEATURE_GSM || defined FEATURE_WCDMA
#error code not present
  #endif /* FEATURE_GSM ||  FEATURE_WCDMA */

  #ifdef FEATURE_GPSONE
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_ptr  != NULL );
  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_PH );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ph_cmd_ptr   = PH_CMD_PTR( cmd_ptr );
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );
  CM_FUNC_START( "cmph_client_cmd_forward_mmll(),cmd=%d",ph_cmd_ptr->cmd,0,0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( ph_cmd_ptr->cmd )
  {

                       /*---------------------------*/
                       /* Change the operating mode */
                       /*---------------------------*/

    case CM_PH_CMD_OPRT_MODE:

      /* Is wait required for oprt mode request
      */
      if (cmph_is_wait_on_oprt_mode (cmd_ptr))
      {
        return TRUE;
      }

      if( cmd_info_ptr->oprt_mode == ph_ptr->oprt_mode &&
          cmd_info_ptr->oprt_mode != SYS_OPRT_MODE_PWROFF)
      {
        /* this is the same oprt mode transition and the current oprt
           mode is not poweroff (the cmph initial condition
           so do not send the command to MC.
           Sometimes MC is suspended for a long period. If we keep
           sending the same operating mode change command to MC,
           MC can run out of buffers.
        */

        /* For ONLINE - ONLINE transition, be sure to set the rtre information */
        cmd_ptr->cmd.ph.info.rtre_control = ph_ptr->rtre_control;
        cmd_ptr->cmd.ph.info.rtre_config = ph_ptr->rtre_config;

        CM_MSG_HIGH("Dup. cmd:same oprt mode(%d) switching", cmd_info_ptr->oprt_mode,0,0);
        break;
      }

      /* If the oprt_mode is not ONLINE , and there is an 911 call going on,
      ** bring the phone out of EMERG_CB
      */
      if ( cmd_info_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE &&
           cmd_info_ptr->oprt_mode != SYS_OPRT_MODE_PSEUDO_ONLINE
         )
      {
        cmph_exit_emergency_cb(ph_ptr);
      }

      switch( cmd_info_ptr->oprt_mode )
      {
        case SYS_OPRT_MODE_OFFLINE_CDMA:
          mmoc_cmd_oprt_mode_chgd( SYS_OPRT_MODE_OFFLINE_CDMA );

          break;

        case SYS_OPRT_MODE_OFFLINE_AMPS:
          mmoc_cmd_oprt_mode_chgd( SYS_OPRT_MODE_OFFLINE_AMPS );
          break;

        case SYS_OPRT_MODE_OFFLINE:
          mmoc_cmd_oprt_mode_chgd( SYS_OPRT_MODE_OFFLINE );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SYS_OPRT_MODE_ONLINE:


          /* Check if there is a PD session and the mode that we are going
          ** online has GW component. If it has, then end the PD session
          */

          #ifdef FEATURE_GPSONE
#error code not present
          #endif

          #ifdef FEATURE_CM_SS_MEAS
#error code not present
          #endif //FEATURE_CM_SS_MEAS

          /* Check if currently in any online mode.
          */
          if ( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE &&
               ph_ptr->oprt_mode != SYS_OPRT_MODE_PSEUDO_ONLINE
             )
          {
            /* No, send online command.
            */
            mmoc_cmd_oprt_mode_chgd( SYS_OPRT_MODE_ONLINE );
          }

          /* Additional operations to be completed on moving online
          */
          cmph_proc_online_mode_oper (ph_ptr);

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Check if the new mode is pseudo online
          */
          switch ( cmd_info_ptr->oprt_mode )
          {
            case SYS_OPRT_MODE_PSEUDO_ONLINE:
              /* Disable 1x TX using DDTM.
              */
              cmph_ddtm_disable_cdma_tx( ph_ptr );
              break;


            case SYS_OPRT_MODE_ONLINE:
              /* restore current DDTM settings.
              ** SK: The above comment should probably read, turn off DDTM.
              */
              cmph_send_ddtm_status( ph_ptr, FALSE, SYS_DDTM_ACT_MASK_EMPTY,
                                     ph_ptr->cur_ddtm_srv_opt_list,
                                     ph_ptr->cur_ddtm_num_srv_opt );
              break;

            default:
              break;

          } /* switch( cmd_info_ptr->oprt_mode) */
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SYS_OPRT_MODE_LPM:

          *cmph_get_is_kicked_hybr_ptr() = FALSE;

          mmoc_cmd_oprt_mode_chgd( SYS_OPRT_MODE_LPM );

          /* For LPM we need to wait for a reply command from MC
          ** in the from of CM_LPM_OK_F, so setup the reply object
          ** to indicate that we are now waiting for CM_LPM_OK_F reply.
          */
          cm_reply_set( &ph_ptr->reply,
                        cmd_ptr,               /* command waiting on reply */
                        CM_LPM_OK_F,           /* wait for this UI command */
                        CMPH_MAX_MC_REPLY_TIME );    /* wait for this long */
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SYS_OPRT_MODE_RESET:
          mmoc_cmd_oprt_mode_chgd( SYS_OPRT_MODE_RESET );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SYS_OPRT_MODE_PWROFF:
          mmoc_cmd_oprt_mode_chgd( SYS_OPRT_MODE_PWROFF );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SYS_OPRT_MODE_FTM:
          *cmph_get_is_kicked_hybr_ptr() = FALSE;
          mmoc_cmd_oprt_mode_chgd( SYS_OPRT_MODE_FTM );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        case SYS_OPRT_MODE_RESET_MODEM:
          mmoc_cmd_oprt_mode_chgd( SYS_OPRT_MODE_RESET_MODEM );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        default:
          CM_SWITCH_ERR( "Unexpected oprt mode=%d",
                         cmd_info_ptr->oprt_mode,0,0 );
          break;

      }

      if( !cmd_info_ptr->is_command_on_hold )
      {
        /* Inform CM pref module.
        */
        cmpref_proc_cmd_oprt_mode_chgd( cmd_info_ptr->oprt_mode );

        /* Reset the origination throttle table
        */
        cmss_orig_thr_table_reset( cmss_ptr()->cmss_orig_thr_tbl_ptr );

        /* Clear the is hybr kicked flag
        */
        *cmph_get_is_kicked_hybr_ptr() = FALSE;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_PH_CMD_MEAS_MODE:
      #ifdef FEATURE_CM_SS_MEAS
#error code not present
      #endif /* FEATURE_CM_SS_MEAS */
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PH_CMD_SYS_SEL_PREF:
    {
      sd_ss_pref_reas_e_type   reason;
      cm_srv_type_e_type       srv_type;
      sys_sys_mode_e_type      sys_mode;
      sd_ss_mode_pref_e_type   curr_mode_pref;
      sd_ss_band_pref_e_type   curr_band_pref;
      cm_mode_pref_e_type      supp_cm_mode_pref;
      sd_ss_mode_pref_e_type   supp_sd_mode_pref;

      cmph_exit_emergency_cb(ph_ptr);

      srv_type = cmph_get_srv_type(CM_CALL_TYPE_PS_DATA); 
    
      /* If srv_type is AUTOMATIC, convert it based on sys mode */
      if (srv_type == CM_SRV_TYPE_AUTOMATIC)
      {
        sys_mode = sd_misc_get_curr_acq_sys_mode (); 
        srv_type = cmph_map_sys_mode_to_srv_type(sys_mode); 
      }

      /* Ending all existing Data Calls, if the mode preference 
      ** has changed between incompatible RATs.
      */
      if(ph_ptr->mode_pref != cmd_info_ptr->mode_pref )
      {
        curr_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref
                                (cmd_info_ptr->mode_pref);
        curr_band_pref = cmph_map_cm_band_pref_to_sd_band_pref
                                                (ph_ptr->band_pref);
        supp_sd_mode_pref = sd_ss_get_supp_mode_pref(curr_mode_pref,
                                                     curr_band_pref);
        supp_cm_mode_pref = cmph_map_sd_mode_pref_to_cm_mode_pref
                                               (supp_sd_mode_pref);     

        CM_MSG_MED ("curr mode_pref: %d req mode_pref: %d curr band_pref: %d",\
                     ph_ptr->mode_pref, cmd_info_ptr->mode_pref, curr_band_pref);
        CM_MSG_MED ("supp sd mode_pref: %d supp cm mode_pref: %d",\
                     supp_sd_mode_pref, supp_cm_mode_pref, 0);
        
          
        if (cmph_misc_intersect_mode_pref (srv_type, supp_cm_mode_pref)
                                                  == CM_MODE_PREF_NONE)
        {
           
          #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
          #endif

          CM_MSG_HIGH ("Ending any existing data calls because of mode change \
                         from %d to %d",ph_ptr->mode_pref, cmd_info_ptr->mode_pref, 0);

          cmcall_end_each_call_with_type(CM_CALL_TYPE_DATA,
                                         CM_CALL_END_CLIENT_END);
          cmph_remove_orig_mode( CM_SS_MAIN,
                                 CM_ACT_TYPE_DATA_CALL,
                                 CM_ACT_PRIORITY_MAX );
          cmph_remove_orig_mode( CM_SS_HDR ,
                                 CM_ACT_TYPE_DATA_CALL,
                                 CM_ACT_PRIORITY_MAX);
          cmph_remove_orig_mode( CM_SS_WLAN ,
                                 CM_ACT_TYPE_DATA_CALL,
                                 CM_ACT_PRIORITY_MAX);
        }
      }

      /*If CM is not in CDMA_WCDMA hybrid operation,
      */

      if( ph_ptr->int_hybr_pref != SD_SS_HYBR_PREF_CDMA__WCDMA )
      {

        /* Enable HDR acquisitions before sending user pref sys change.
        */

        /* Since we are sending FALSE, we just update the cmpref
        ** mask and don't send the pref sys chgd down
        */

        cmpref_proc_cmd_enable_hybr_hdr( FALSE );
      }


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Gather client requested srv domain preference.
      */
      if (cmd_info_ptr->srv_domain_pref == CM_SRV_DOMAIN_PREF_NO_CHANGE)
      {
        client_req_srv_domain_pref = ph_ptr->srv_domain_pref;
      }
      else
      {
        client_req_srv_domain_pref = cmd_info_ptr->srv_domain_pref;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      /*
      ** Command the MMoC to perform pref. system selection change.
      */
      if ( ( network_sel_mode_pref_changed ) ||
           ( cmd_info_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL
           && (!(sys_plmn_id_is_undefined (cmd_info_ptr->plmn))/*PLMN ptr not NO change*/)
           )
         )
      {
        reason = SD_SS_PREF_REAS_USER_RESEL;
      }
      else
      {
        reason = SD_SS_PREF_REAS_USER;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      /* Preferred srv domain is initialized to client requested srv dom */
      srv_domain_tobe_forced = client_req_srv_domain_pref;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      #if defined FEATURE_GSM || defined FEATURE_WCDMA
#error code not present
      #endif /* FEATURE_GSM || FEATURE_WCDMA */

      {
        /* If hybrid preferences are toggled, then wipe out the DO Queue
        */

        cm_act_orig_s_type *last_pref_ptr = cmph_get_last_pref_sent_on_main();

        CM_ASSERT( last_pref_ptr != NULL );

        if( last_pref_ptr->act_priority == CM_ACT_PRIORITY_PH )
        {
          /* Last pref ptr is valid
          */
          if( last_pref_ptr->orig_hybr_pref != cmd_info_ptr->hybr_pref )
          {
            cmph_orig_mode_reset_on_ss( ph_ptr, CM_SS_HYBR_1, FALSE );
          }

        }
        else
        {
          /* Get the top ptr from MAIN Q and check if the preferences
          ** are toggled
          */
           cm_orig_q_s_type *ph_orig_top_ptr =
                                      cmtask_orig_para_get_top( CM_SS_MAIN );

           CM_ASSERT( ph_orig_top_ptr != NULL );

           /*lint -save -e774 Boolean within 'if' always evaluates to True
           **                 Not true, If asserts are turned off we need to
           **                 check for NULL pointer
           */

           if( ( ph_orig_top_ptr != NULL ) &&
                 ( ph_orig_top_ptr->orig.orig_hybr_pref !=
                                                   cmd_info_ptr->hybr_pref ))
           {
             cmph_orig_mode_reset_on_ss( ph_ptr, CM_SS_HYBR_1, FALSE );
           }

           /*lint -restore */
        }
        
        CM_MSG_HIGH ("pref_sys_chg in common sect cmd_srv_domain = %d pref_srv_domain = %d",
                       client_req_srv_domain_pref, srv_domain_tobe_forced, 0);

        (void)cmph_force_pref_on_the_fly(
                            CM_SS_MAX,
                            ph_ptr,
                            reason,
                            CM_ACT_TYPE_PH_OBJ,
                            ph_ptr->pref_term,
                            cmd_info_ptr->mode_pref,
                            cmd_info_ptr->band_pref,
                            cmd_info_ptr->prl_pref,
                            cmd_info_ptr->roam_pref,
                            cmd_info_ptr->hybr_pref,
                            ph_cmd_ptr->info.plmn,
                            srv_domain_tobe_forced,
                            CM_OTASP_ACT_CODE_NONE,
                           &cmd_info_ptr->wlan_pref,
                            cmd_info_ptr->acq_order_pref,
                            cmd_info_ptr->network_sel_mode_pref,
                            (cm_act_id_type)ph_ptr,
                             CM_ACT_UPDATE_REAS_USER,
                            FALSE );

      }

      /* CM needs to wait to receive the PS_SIG_DOWN event after PS Service is acquired.
      ** When we change the system selection prefernece to acquire a PS service, we need
      ** to start the PS_SIG_DOWN timer.
      **
      ** ps_service_req_sent flag is used to decide if we need to start the PS_SIG_DOWN
      ** timer after PS service is acquired.
      */

      if ( client_req_srv_domain_pref == CM_SRV_DOMAIN_PREF_CS_PS ||
           client_req_srv_domain_pref == CM_SRV_DOMAIN_PREF_PS_ONLY )
      {
           ph_ptr->is_ps_srv_req_sent = TRUE;
      }

      /* Check if the network selection mode is limited and if it is
      ** then enter emergency call back mode
      */
      if( cmd_info_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV )
      {
        cmph_enter_emergency_cb( ph_ptr, FALSE );
      }

      break;
    }

    #ifdef FEATURE_DDTM_CNTL
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                /*-----------------------------------------*/
                /* Change the DDTM preference            */
                /*-----------------------------------------*/

    case CM_PH_CMD_DDTM_PREF:
      break;

    #endif /* FEATURE_DDTM_CNTL */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                /*-----------------------------------------*/
                /* Change the answer voice setting */
                /*-----------------------------------------*/

    case CM_PH_CMD_ANSWER_VOICE:

      /* NO need to notify MC of this one.
      */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                        /*--------------------------*/
                        /* Change the NAM selection */
                        /*--------------------------*/

    case CM_PH_CMD_NAM_SEL:

      /* No need to notify MC of AUTO-NAM selection
      */
      if( cmd_info_ptr->nam_sel == CM_NAM_AUTO )
      {
        break;
      }

      /* Only notify MC if phone is currently online.
      */
      if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE  &&
          ph_ptr->oprt_mode != SYS_OPRT_MODE_PSEUDO_ONLINE
        )
      {
        break;
      }

      /* Inform CM Pref & MMOC that subscription changed.
      ** ADD_PROTO with both CDMA and GW makes both protocol stacks reload
      ** NV information
      */
      if(!INRANGE(cmd_info_ptr->nam_sel, 0, NV_MAX_NAMS))
      {
        CM_ERR( "invalid nam: %d",cmd_info_ptr->nam_sel,0,0);
        break;
      }

      cmpref_proc_cmd_subsc_chgd(

                                  MMOC_SUBSC_CHG_NAM_SEL,
                                  (byte) cmd_info_ptr->nam_sel,
                                  cmph_map_cm_network_sel_mode_pref_to_orig_mode(
                                    ph_ptr->prst_network_sel_mode_pref[ cmd_info_ptr->nam_sel ]),
                                  cmph_map_cm_mode_pref_to_sd_mode_pref(
                                    ph_ptr->prst_mode_pref[cmd_info_ptr->nam_sel]),
                                  cmph_map_cm_band_pref_to_sd_band_pref( ph_ptr->prst_band_pref[cmd_info_ptr->nam_sel]),
                                  cmph_map_cm_prl_pref_to_sd_prl_pref(ph_ptr->prst_prl_pref[cmd_info_ptr->nam_sel]),
                                  cmph_map_cm_roam_pref_to_sd_roam_pref(ph_ptr->prst_roam_pref[cmd_info_ptr->nam_sel]),
                                  cmph_map_cm_hybr_pref_to_sd_hybr_pref(ph_ptr->prst_hybr_pref),
                                  ph_ptr->plmn,
                                  cmph_map_cm_srv_domain_pref_to_sd(
                                    ph_ptr->prst_srv_domain_pref[ cmd_info_ptr->nam_sel  ]),
                                  cmph_map_cm_acq_order_pref_to_sd(
                                    ph_ptr->prst_acq_order_pref[ cmd_info_ptr->nam_sel  ] ),
                                  ph_ptr->is_gw_subscription_available,
                                  ph_ptr->is_cdma_subscription_available,
                                  ph_ptr->is_wlan_subscription_available,
                                  &ph_ptr->wlan_pref,
                                  PROT_SUBSC_CHG_ALL
                                );

      /* Reset the origination throttle table
      */
      cmss_orig_thr_table_reset( cmss_ptr()->cmss_orig_thr_tbl_ptr );

      /* Clear the is hybr kicked flag
      */
      *cmph_get_is_kicked_hybr_ptr() = FALSE;

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /*------------------------------*/
                      /* A CM Client activated itself */
                      /*------------------------------*/

    case CM_PH_CMD_CLIENT_ACT:

      /* NO need to notify MC of this one.
      */

      break;



    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*------------------------------------*/
                     /* Get a snapshot of call information */
                     /*------------------------------------*/

    case CM_PH_CMD_INFO_GET:

      /* do nothing - this request terminated at CM */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*----------------------------------*/
                     /* Set RSSI change delta threshold  */
                     /*----------------------------------*/

    case CM_PH_CMD_RSSI_DELTA:

      /* do nothing - this request terminated at CM */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*------------------------------------------*/
                     /* Set RSSI,ECIO,IO change delta threshold  */
                     /*------------------------------------------*/

    case CM_PH_CMD_SIGNAL_STRENGTH_DELTA:

      /* do nothing - this request terminated at CM */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*----------------------------------*/
                     /*         Set Packet State         */
                     /*----------------------------------*/

    case CM_PH_CMD_PACKET_STATE:

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if( cmd_info_ptr->packet_state == CM_PACKET_STATE_NULL )
      {

        /* There is no PPP session - phone can restore regular
          hybrid operation */
        if( ph_ptr->pref_term != CM_PREF_TERM_1_CALL &&
            ph_ptr->pref_term != CM_PREF_TERM_CM_1_CALL_PLUS &&
            ph_ptr->pref_term != CM_PREF_TERM_CM_MO_SMS )
        {
           /* Restore normal Hybrid HDR operation */
           CM_MSG_HIGH("Reselect - got CM_PACKET_STATE_NULL",0,0,0);

           /* This will send a pref sys chgd down to the HDR
           ** instance if no HDR call is going on, this bypasses the
           ** Q mechanism.
           */
           cmpref_proc_cmd_enable_hybr_hdr( TRUE );
        }
      }
      break;


    #ifdef FEATURE_JCDMA
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*----------------------------------*/
                   /* Enable/Disable voice activity    */
                   /*----------------------------------*/

    case CM_PH_CMD_VOC_ACTIVITY:

        /* not really the most appropriate function to do this in
        ** but this seemed to be the best fit without being more kludgy
        */
        txc_set_voc_activity( cmd_info_ptr->voc_act_enable,
                              cmd_info_ptr->voc_act_voc_1,
                              cmd_info_ptr->voc_act_voc_2,
                              cmd_info_ptr->voc_act_voc_8 );

     break;

    #endif /* FEATURE_JCDMA */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PH_CMD_AVOID_SYS:

      /* Only notify MC if phone is currently online.
      */
      if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
      {
        break;
      }

      /*
      ** Command the MMoC to perform the avoid system.
      */
      cmpref_proc_cmd_pref_sys_chgd(
                              SD_SS_MAIN,
                              SD_SS_PREF_REAS_AVOID_SYS,
                              cmph_map_cm_network_sel_mode_pref_to_orig_mode(
                                ph_ptr->network_sel_mode_pref      ),
                              cmph_map_cm_mode_pref_to_sd_mode_pref(
                                ph_ptr->mode_pref ),
                              cmph_map_cm_band_pref_to_sd_band_pref( ph_ptr->band_pref),
                              cmph_map_cm_prl_pref_to_sd_prl_pref(ph_ptr->prl_pref),
                              cmph_map_cm_roam_pref_to_sd_roam_pref(ph_ptr->roam_pref),
                              cmph_map_cm_hybr_pref_to_sd_hybr_pref(ph_ptr->prst_hybr_pref),
                              SD_BAND_PCS,
                              SD_BLKSYS_PCS_A,
                              cmph_map_cm_avoid_type_to_sd_avoid_type(
                                 cmd_info_ptr->avoid_type),
                              cmd_info_ptr->avoid_time,
                              ph_ptr->plmn,
                              cmph_map_cm_srv_domain_pref_to_sd(
                                ph_ptr->srv_domain_pref),
                              cmph_map_cm_acq_order_pref_to_sd(
                                cmd_info_ptr->acq_order_pref),
                              NULL
                            );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PH_CMD_WAKEUP_FROM_STANDBY:
      if( CM_CONTAIN ( cmd_info_ptr->wakeup_service,CM_WAKEUP_MAIN ))
      {
        #if (defined(FEATURE_OOSC_USER_ACTION) && defined(FEATURE_WCDMA)) 
#error code not present
        #endif
        if(!cmcall_check_for_call_state_on_ss(CM_SS_MAIN,CM_CALL_STATE_CONV)&&
           !cmcall_check_for_call_state_on_ss(CM_SS_MAIN,CM_CALL_STATE_INCOM))
        {
          mmoc_cmd_wakeup_from_pwr_save(SD_SS_MAIN);
        }
      }

      if( CM_CONTAIN ( cmd_info_ptr->wakeup_service,CM_WAKEUP_WLAN )
          && !cmcall_check_for_call_state_on_ss(CM_SS_WLAN,CM_CALL_STATE_CONV)
          && !cmcall_check_for_call_state_on_ss(CM_SS_WLAN,CM_CALL_STATE_INCOM))
      {
         mmoc_cmd_wakeup_from_pwr_save(SD_SS_HYBR_WLAN);
      }

      if( CM_CONTAIN ( cmd_info_ptr->wakeup_service,CM_WAKEUP_HDR )&&
          !cmcall_check_for_call_state_on_ss(CM_SS_HDR,CM_CALL_STATE_CONV) &&
          !cmcall_check_for_call_state_on_ss(CM_SS_HDR,CM_CALL_STATE_INCOM))
      {
         mmoc_cmd_wakeup_from_pwr_save(SD_SS_HYBR_HDR);
      }
      break;


    #if ( defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) )
    case CM_PH_CMD_CHANGE_RTRE_CONFIG:
      /*
      ** Send RTRE command to NV.
      */
      #if defined( FEATURE_UIM_RUIM )
      #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
      /*
      ** RTRE enabled.
      */
      /* Read the RTRE Config item from NV */
      switch ( cmd_ptr->cmd.ph.info.rtre_config )
      {
        case CM_RTRE_CONFIG_RUIM_ONLY:
          cmnv_item_ptr->rtre_config = NV_RTRE_CONFIG_RUIM_ONLY;
          break;

        case CM_RTRE_CONFIG_NV_ONLY:
          cmnv_item_ptr->rtre_config = NV_RTRE_CONFIG_NV_ONLY;
          break;

        case CM_RTRE_CONFIG_RUIM_OR_DROP_BACK:
          cmnv_item_ptr->rtre_config = NV_RTRE_CONFIG_RUIM_OR_DROP_BACK;
          break;

        case CM_RTRE_CONFIG_SIM_ACCESS:
          cmnv_item_ptr->rtre_config = NV_RTRE_CONFIG_SIM_ACCESS;
          break;

        default:
          ERR("Unexpected config value %d",
              cmd_ptr->cmd.ph.info.rtre_config, 0, 0);
          break;
      }

      cmnv_write_wait( NV_RTRE_CONFIG_I, cmnv_item_ptr ); /* Write to NV */
      cmnv_send_rtre_command( cmnv_item_ptr ); /* Send RTRE config to NV */

      switch ( nv_rtre_control() ) {
        case NV_RTRE_CONTROL_USE_RUIM:
          cmd_ptr->cmd.ph.info.rtre_control = CM_RTRE_CONTROL_RUIM;
          break;
        case NV_RTRE_CONTROL_NO_RUIM:
          cmd_ptr->cmd.ph.info.rtre_control = CM_RTRE_CONTROL_NV;
          break;
        #ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
#error code not present
        #endif  /* FEATURE_UIM_RUIM_W_GSM_ACCESS */
        default:
          ERR("Unexpected nv rtre control value %d",
              nv_rtre_control(), 0, 0);
      }
      #else /* defined( FEATURE_UIM_RUN_TIME_ENABLE ) */
      /*
      ** RUIM only
      */
      cmd_ptr->cmd.ph.info.rtre_control = CM_RTRE_CONTROL_RUIM;
      #endif /* else of defined( FEATURE_UIM_RUN_TIME_ENABLE ) */
      #else /* defined( FEATURE_UIM_RUIM ) */
      /*
      ** NV only
      */
      cmd_ptr->cmd.ph.info.rtre_control = CM_RTRE_CONTROL_NV;
      #endif /* else of defined( FEATURE_UIM_RUIM ) */
      break;
    #endif /* defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /*------------------------*/
                      /* Subscription Available */
                      /*------------------------*/

    case CM_PH_CMD_SUBSCRIPTION_AVAILABLE:
      {
        boolean gw_available = ph_ptr->is_gw_subscription_available;
        boolean cdma_available = ph_ptr->is_cdma_subscription_available;
        boolean wlan_available = ph_ptr->is_wlan_subscription_available;
        prot_subsc_chg_e_type prot_subsc_chg = PROT_SUBSC_CHG_NONE;
        sd_ss_mode_pref_e_type sd_mode_pref =
                cmph_map_cm_mode_pref_to_sd_mode_pref(ph_ptr->mode_pref);
        cm_mode_pref_e_type  fav_mode_pref = ph_ptr->prst_mode_pref[ph_ptr->curr_nam];
        /*does not allow the subscription to go through if the technology
        **is not in the mode pref
        **exception is all are allowed if even one subscription is allowed
        */
        int invalid_subsc_count = 0;
			
        #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
        #endif /* FEATURE_WCDMA || FEATURE_GSM */

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */       

        if ( cmd_info_ptr->wlan_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
            !(sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_WLAN)) || \
            cmd_info_ptr->wlan_sub_avail_status != CM_SUBSCRIPTION_STATUS_CHANGE)
        {
          invalid_subsc_count++;
        }

        if ( cmd_info_ptr->gw_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
            !( sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_GW))|| \
            cmd_info_ptr->gw_sub_avail_status != CM_SUBSCRIPTION_STATUS_CHANGE)
        {
          invalid_subsc_count++;
        }

        if ( cmd_info_ptr->cdma_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
            !( sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_CDMA_HDR))|| \
            cmd_info_ptr->cdma_sub_avail_status != CM_SUBSCRIPTION_STATUS_CHANGE)
        {
          invalid_subsc_count++;
        }
        /*break if no commands to go through
        */
        if(invalid_subsc_count == 3)
        {
          break;
        }

        /*change the command pointer for all technologies that are sending a
        **subscription command*/
        if(ph_ptr->is_pending_subsc_cdma &&
           ph_ptr->cdma_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE)
        {
          cmd_info_ptr->cdma_sub_avail_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }

        if(ph_ptr->is_pending_subsc_gw &&
           ph_ptr->gw_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE)
        {
          cmd_info_ptr->gw_sub_avail_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }

        if(ph_ptr->is_pending_subsc_wlan &&
           ph_ptr->wlan_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE)
        {
          cmd_info_ptr->wlan_sub_avail_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }

        if ( cmd_info_ptr->wlan_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
        {
          wlan_available = TRUE;
          prot_subsc_chg = ADD_PROTO (prot_subsc_chg, PROT_SUBSC_CHG_WLAN);
        }

        if ( cmd_info_ptr->gw_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
        {
          gw_available = TRUE;

          if (!ph_ptr->is_gw_subscription_available)
          {
            prot_subsc_chg = ADD_PROTO (prot_subsc_chg, PROT_SUBSC_CHG_GW);
          }

          #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
          #endif /* FEATURE_WCDMA || FEATURE_GSM */
        }


        if ( cmd_info_ptr->cdma_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
        {
          cdma_available = TRUE;
          if (!ph_ptr->is_cdma_subscription_available)
          {
            prot_subsc_chg = ADD_PROTO (prot_subsc_chg, PROT_SUBSC_CHG_CDMA);
          }
        }




        #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
        #if (defined(FEATURE_OTASP) || defined (FEATURE_UIM_RUIM))
        diag_otasp_update();              /* Tell Diag to update its cache */
        #endif   /* FEATURE_OTASP || FEATURE_UIM_RUIM */
        #endif   /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

        /* Reset the origination throttle table
        */
        cmss_orig_thr_table_reset( cmss_ptr()->cmss_orig_thr_tbl_ptr );

        /* Clear the is hybr kicked flag
        */
        *cmph_get_is_kicked_hybr_ptr() = FALSE;

        /* Only notify MMOC if phone is currently online or powerup.
        */
        if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE  &&
            ph_ptr->oprt_mode != SYS_OPRT_MODE_PWROFF  &&
            ph_ptr->oprt_mode != SYS_OPRT_MODE_PSEUDO_ONLINE
          )
        {
          break;

        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        
        /* Validate curr_nam */
        if(!INRANGE(ph_ptr->curr_nam, 0, CM_NAM_AUTO))
        {
          CM_ERR( "curr_nam is invalid!!!, no subsc cmd sent",0,0,0);
          break;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Inform CM Pref & MMOC that subscription changed.
        */
        cmpref_proc_cmd_subsc_chgd(
                                    MMOC_SUBSC_CHG_SUBSC_AVAIL,
                                    (byte) ph_ptr->curr_nam,
                                    cmph_map_cm_network_sel_mode_pref_to_orig_mode(
                                    CMPH_PRST_NETWORK_SEL_MODE_PREF( ph_ptr )),
                                    cmph_map_cm_mode_pref_to_sd_mode_pref(fav_mode_pref),
                                    cmph_map_cm_band_pref_to_sd_band_pref(ph_ptr->prst_band_pref[ph_ptr->curr_nam]),
                                    cmph_map_cm_prl_pref_to_sd_prl_pref(ph_ptr->prst_prl_pref[ph_ptr->curr_nam]),
                                    cmph_map_cm_roam_pref_to_sd_roam_pref(ph_ptr->prst_roam_pref[ph_ptr->curr_nam]),
                                    cmph_map_cm_hybr_pref_to_sd_hybr_pref(ph_ptr->prst_hybr_pref),
                                    ph_ptr->plmn,
                                    cmph_map_cm_srv_domain_pref_to_sd(
                                    CMPH_PRST_SRV_DOMAIN_PREF( ph_ptr)),
                                    cmph_map_cm_acq_order_pref_to_sd(
                                    CMPH_PRST_ACQ_ORDER_PREF( ph_ptr )  ),
                                    gw_available,
                                    cdma_available,
                                    wlan_available,
                                    &ph_ptr->wlan_pref,
                                    prot_subsc_chg
                                  );

      }
      break;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /*----------------------------*/
                      /* Subscription Not Available */
                      /*----------------------------*/

    case CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE:

      {
        boolean gw_available = ph_ptr->is_gw_subscription_available;
        boolean cdma_available = ph_ptr->is_cdma_subscription_available;
        boolean wlan_available = ph_ptr->is_wlan_subscription_available;
        prot_subsc_chg_e_type prot_subsc_chg = PROT_SUBSC_CHG_NONE;
        sd_ss_mode_pref_e_type sd_mode_pref =
                cmph_map_cm_mode_pref_to_sd_mode_pref(ph_ptr->mode_pref);
        /*does not allow the subscription to go through if the technology
        **is not in the mode pref
        **exception is all are allowed if even one subscription is allowed
        */
        int invalid_subsc_count = 0;

        if ( cmd_info_ptr->wlan_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
            !( sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_WLAN)) || \
            cmd_info_ptr->wlan_sub_not_avail_status != CM_SUBSCRIPTION_STATUS_CHANGE)
        {
          invalid_subsc_count++;
        }

        if ( cmd_info_ptr->gw_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
            !( sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_GW))|| \
            cmd_info_ptr->gw_sub_not_avail_status != CM_SUBSCRIPTION_STATUS_CHANGE)
        {
          invalid_subsc_count++;
        }

        if ( cmd_info_ptr->cdma_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
            !( sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_CDMA_HDR))|| \
            cmd_info_ptr->cdma_sub_not_avail_status != CM_SUBSCRIPTION_STATUS_CHANGE)
        {
          invalid_subsc_count++;
        }
        /*break if no commands to go through
        */
        if(invalid_subsc_count == 3)
        {
          break;
        }

        /*change the command pointer for all technologies that are sending a
        **subscription command*/
        if(ph_ptr->is_pending_subsc_cdma &&
           ph_ptr->cdma_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE)
        {
          cmd_info_ptr->cdma_sub_not_avail_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }

        if(ph_ptr->is_pending_subsc_gw &&
           ph_ptr->gw_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE)
        {
          cmd_info_ptr->gw_sub_not_avail_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }

        if(ph_ptr->is_pending_subsc_wlan &&
           ph_ptr->wlan_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE)
        {
          cmd_info_ptr->wlan_sub_not_avail_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }

        if ( cmd_info_ptr->gw_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
        {
          gw_available = FALSE;

          /* GW subscription information has changed, so record protocol
          */
          if (ph_ptr->is_gw_subscription_available)
          {
            prot_subsc_chg = ADD_PROTO (prot_subsc_chg, PROT_SUBSC_CHG_GW);
          }

          #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
          #endif
          
        }

        if ( cmd_info_ptr->cdma_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE)
        {
          cdma_available = FALSE;

          /* CDMA subscription information has changed, so record protocol.
          */
          if (ph_ptr->is_cdma_subscription_available)
          {
            prot_subsc_chg = ADD_PROTO (prot_subsc_chg, PROT_SUBSC_CHG_CDMA);
          }
        }

        if ( cmd_info_ptr->wlan_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE)
        {
          wlan_available = FALSE;
          prot_subsc_chg = ADD_PROTO (prot_subsc_chg, PROT_SUBSC_CHG_WLAN);
        }

        /* Reset the origination throttle table
        */
        cmss_orig_thr_table_reset( cmss_ptr()->cmss_orig_thr_tbl_ptr );

        /* Clear the is hybr kicked flag
        */
        *cmph_get_is_kicked_hybr_ptr() = FALSE;

        /* Only notify MMOC if phone is currently online or powerup.
        */
        if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE  &&
            ph_ptr->oprt_mode != SYS_OPRT_MODE_PWROFF  &&
            ph_ptr->oprt_mode != SYS_OPRT_MODE_PSEUDO_ONLINE
          )
        {
          break;

        }

        #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
        #endif /* FEATURE_WCDMA || FEATURE_GSM */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        
        /* Validate curr_nam */
        if(!INRANGE(ph_ptr->curr_nam, 0, CM_NAM_AUTO))
        {
          CM_ERR( "curr_nam is invalid!!!, no subsc cmd sent",0,0,0);
          break;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Inform CM Pref & MMOC that subscription changed.
          */
          /*
          ** 539: Did not expect positive indentation from line 5205
          ** This is due to GW features not defined for some targets
          */
          /*lint -save -e539 */
          cmpref_proc_cmd_subsc_chgd(
                                    MMOC_SUBSC_CHG_SUBSC_AVAIL,
                                    (byte) ph_ptr->curr_nam,
                                    cmph_map_cm_network_sel_mode_pref_to_orig_mode(
                                      CMPH_PRST_NETWORK_SEL_MODE_PREF( ph_ptr )),
                                    cmph_map_cm_mode_pref_to_sd_mode_pref(
                                      ph_ptr->prst_mode_pref[ph_ptr->curr_nam]),
                                    cmph_map_cm_band_pref_to_sd_band_pref(ph_ptr->prst_band_pref[ph_ptr->curr_nam]),
                                    cmph_map_cm_prl_pref_to_sd_prl_pref(ph_ptr->prst_prl_pref[ph_ptr->curr_nam]),
                                    cmph_map_cm_roam_pref_to_sd_roam_pref(ph_ptr->prst_roam_pref[ph_ptr->curr_nam]),
                                    cmph_map_cm_hybr_pref_to_sd_hybr_pref(ph_ptr->prst_hybr_pref),
                                    ph_ptr->plmn,
                                    cmph_map_cm_srv_domain_pref_to_sd(
                                      CMPH_PRST_SRV_DOMAIN_PREF( ph_ptr)),
                                    cmph_map_cm_acq_order_pref_to_sd(
                                      CMPH_PRST_ACQ_ORDER_PREF( ph_ptr )  ),
                                    gw_available,
                                    cdma_available,
                                    wlan_available,
                                    &ph_ptr->wlan_pref,
                                    prot_subsc_chg
                                  );
          /*lint -restore */

        #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
        #endif /* FEATURE_WCDMA || FEATURE_GSM */
      }
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PH_CMD_SUBSCRIPTION_CHANGED:
      /* Send subscription changed command.
      */
      {
        boolean gw_available = ph_ptr->is_gw_subscription_available;
        boolean cdma_available = ph_ptr->is_cdma_subscription_available;
        boolean wlan_available = ph_ptr->is_wlan_subscription_available;
        prot_subsc_chg_e_type prot_subsc_chg = PROT_SUBSC_CHG_NONE;
        sd_ss_mode_pref_e_type sd_mode_pref =
                cmph_map_cm_mode_pref_to_sd_mode_pref(ph_ptr->mode_pref);
        cm_mode_pref_e_type  fav_mode_pref = ph_ptr->prst_mode_pref[ph_ptr->curr_nam];
        /*does not allow the subscription to go through if the technology
        **is not in the mode pref
        **exception is all are allowed if even one subscription is allowed
        */
        int invalid_subsc_count = 0;

        #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
        #endif /* FEATURE_WCDMA || FEATURE_GSM */



        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

        if ( cmd_info_ptr->wlan_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
            !( sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_WLAN)) || \
            cmd_info_ptr->wlan_sub_changed_status != CM_SUBSCRIPTION_STATUS_CHANGE)
        {
          invalid_subsc_count++;
        }

        if ( cmd_info_ptr->gw_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
            !( sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_GW))|| \
            cmd_info_ptr->gw_sub_changed_status != CM_SUBSCRIPTION_STATUS_CHANGE)
        {
          invalid_subsc_count++;
        }

        if ( cmd_info_ptr->cdma_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
            !( sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_CDMA_HDR))|| \
            cmd_info_ptr->cdma_sub_changed_status != CM_SUBSCRIPTION_STATUS_CHANGE)
        {
          invalid_subsc_count++;
        }
        /*break if no commands to go through
        */
        if(invalid_subsc_count == 3)
        {
          break;
        }

        /*change the command pointer for all technologies that are sending a
        **subscription command*/
        if(ph_ptr->is_pending_subsc_cdma &&
           ph_ptr->cdma_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED)
        {
          cmd_info_ptr->cdma_sub_changed_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }

        if(ph_ptr->is_pending_subsc_gw &&
           ph_ptr->gw_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED)
        {
          cmd_info_ptr->gw_sub_changed_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }

        if(ph_ptr->is_pending_subsc_wlan &&
           ph_ptr->wlan_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED)
        {
          cmd_info_ptr->wlan_sub_changed_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }

        if ( cmd_info_ptr->wlan_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE)
        {
          wlan_available = TRUE;
          prot_subsc_chg = ADD_PROTO (prot_subsc_chg, PROT_SUBSC_CHG_WLAN);
        }

        if ( cmd_info_ptr->gw_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE)
        {
          gw_available = TRUE;

          prot_subsc_chg = ADD_PROTO (prot_subsc_chg, PROT_SUBSC_CHG_GW);
          
          #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
          #endif /* FEATURE_WCDMA || FEATURE_GSM */
        }


        if ( cmd_info_ptr->cdma_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE)
        {
          cdma_available = TRUE;
          prot_subsc_chg = ADD_PROTO (prot_subsc_chg, PROT_SUBSC_CHG_CDMA);
        }

        /* Reset the origination throttle table
        */
        cmss_orig_thr_table_reset( cmss_ptr()->cmss_orig_thr_tbl_ptr );

        /* Clear the is hybr kicked flag
        */
        *cmph_get_is_kicked_hybr_ptr() = FALSE;

        #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
        #if (defined(FEATURE_OTASP) || defined(FEATURE_UIM_RUIM))
        diag_otasp_update();              /* Tell Diag to update its cache */
        #endif /* FEATURE_OTASP || FEATURE_UIM_RUIM */
        #endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */


        /* Only notify MMOC if phone is currently online or powerup.
        */
        if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE  &&
            ph_ptr->oprt_mode != SYS_OPRT_MODE_PWROFF  &&
            ph_ptr->oprt_mode != SYS_OPRT_MODE_PSEUDO_ONLINE
          )
        {
          break;

        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        
        /* Validate curr_nam */
        if(!INRANGE(ph_ptr->curr_nam, 0, CM_NAM_AUTO))
        {
          CM_ERR( "curr_nam is invalid!!!, no subsc cmd sent",0,0,0);
          break;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Inform CM Pref & MMOC that subscription changed.
        */
        cmpref_proc_cmd_subsc_chgd(
                                    MMOC_SUBSC_CHG_SUBSC_AVAIL,
                                    (byte) ph_ptr->curr_nam,
                                    cmph_map_cm_network_sel_mode_pref_to_orig_mode(
                                      CMPH_PRST_NETWORK_SEL_MODE_PREF( ph_ptr )),
                                    cmph_map_cm_mode_pref_to_sd_mode_pref(fav_mode_pref),
                                    cmph_map_cm_band_pref_to_sd_band_pref(ph_ptr->prst_band_pref[ph_ptr->curr_nam]),
                                    cmph_map_cm_prl_pref_to_sd_prl_pref(ph_ptr->prst_prl_pref[ph_ptr->curr_nam]),
                                    cmph_map_cm_roam_pref_to_sd_roam_pref(ph_ptr->prst_roam_pref[ph_ptr->curr_nam]),
                                    cmph_map_cm_hybr_pref_to_sd_hybr_pref(ph_ptr->prst_hybr_pref),
                                    ph_ptr->plmn,
                                    cmph_map_cm_srv_domain_pref_to_sd(
                                      CMPH_PRST_SRV_DOMAIN_PREF( ph_ptr)),
                                    cmph_map_cm_acq_order_pref_to_sd(
                                      CMPH_PRST_ACQ_ORDER_PREF( ph_ptr )  ),
                                    gw_available,
                                    cdma_available,
                                    wlan_available,
                                    &ph_ptr->wlan_pref,
                                    prot_subsc_chg
                                  );
      }
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


                      /*-----------------------------------------*/
                      /* Client has requested a list of networks */
                      /*-----------------------------------------*/

    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_WLAN)
#error code not present
    #endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_WLAN */


     /* Client has requested WLAN security credentials vis WPS */

    case CM_PH_CMD_WPS_START:
      #ifdef FEATURE_WLAN_WPS
#error code not present
      #endif
      break;

    case CM_PH_CMD_WPS_ABORT:
      #ifdef FEATURE_WLAN_WPS
#error code not present
      #endif
      break;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                /*-----------------------------------------------------*/
                /* Client has requested to save the preferred networks */
                /*-----------------------------------------------------*/

    case CM_PH_CMD_SET_PREFERRED_NETWORKS:

      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PH_CMD_RESET_ACM:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PH_CMD_SET_ACMMAX:
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
    #endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PH_CMD_CLIENT_RELEASE:

      /* No need to notify MC of this one.
      */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      CM_SWITCH_ERR( "Bad phone command %d", ph_cmd_ptr->cmd, 0, 0);

      break;

  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the caller of this function to whether
  ** the phone object is now waiting for reply from MC.
  */
  return cm_reply_is_waiting( &ph_ptr->reply );
} /* cmph_client_cmd_forward_mmll() */

/*===========================================================================

FUNCTION cmph_get_srv_type

DESCRIPTION
  Returns srv_type for the particular call_type that 
  has been given as input.
  If such an object does not exist returns NONE.

DEPENDENCIES
  CM must be initialized

RETURN VALUE
  srv_type of teh call, else NONE

SIDE EFFECTS
  None

===========================================================================*/

cm_srv_type_e_type cmph_get_srv_type( 
  
    cm_call_type_e_type  call_type
)
{
  cmcall_s_type   *call_ptr = NULL;
  cm_iterator_type call_itr;


  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if(call_ptr->call_type == call_type)
    {
      return (call_ptr->srv_type);
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return CM_SRV_TYPE_NONE;
}


/*===========================================================================

FUNCTION cmph_client_cmd_mmll_reply

DESCRIPTION
  Process MC replies to clients phone commands.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmph_client_cmd_mmll_reply(

    cmph_s_type             *ph_ptr,
        /* Pointer to a phone object */

    const cm_cmd_s_type     *cmd_ptr,
        /* Pointer to a CM command */

    cm_ph_cmd_err_e_type    cmd_err
        /* Indicate phone command error */
)
/*lint -esym(715, cmd_err) */
{
  const cm_ph_cmd_s_type       *ph_cmd_ptr   = NULL;
      /* Point at phone command component */

  const cm_ph_cmd_info_s_type  *cmd_info_ptr = NULL;
      /* Point at command information component */

  cm_ph_info_s_type    *ph_info_ptr;
      /* pointer for phone info */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  db_items_value_type  db_item;
      /* Item to do db_get() and db_put() */
  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  sys_oprt_mode_e_type prev_oprt_mode;
    /* Previous operating mode */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( BETWEEN( cmd_err, CM_PH_CMD_ERR_NONE, CM_PH_CMD_ERR_MAX));
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_PH );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_cmd_ptr   = PH_CMD_PTR( cmd_ptr );
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );
  CM_FUNC_START( "cmph_client_cmd_mmll_reply(), cmd=%d, err=%d",
                 ph_cmd_ptr->cmd, cmd_err, 0 );

  /* If the command is on Hold, return
  */

  if( ph_cmd_ptr->info.is_command_on_hold )
  {
    return;
  }

  /*
  ** 1. Update phone fields.
  ** 2. If necessary write to NV or DB the new setting.
  ** 3. Notify clients of phone event.
  */

  /* Update phone fields by copying relevant
  ** command fields into phone struct.
  */
  prev_oprt_mode = ph_ptr->oprt_mode;         /* copy previous oprt mode.*/

  cmph_cmd_copy_fields( ph_ptr, ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If necessary, copy specific command fields into phone struct,
  ** and notify clients of event.
  */

  switch( ph_cmd_ptr->cmd )
  {
                     /*---------------------------*/
                     /* Change the operating mode */
                     /*---------------------------*/

    case CM_PH_CMD_OPRT_MODE:
      cmph_event( ph_ptr, CM_PH_EVENT_OPRT_MODE );

      /* Update nv ftm flag whenever there is a operation mode change.*/
      if ( (  prev_oprt_mode == SYS_OPRT_MODE_FTM
           && ph_ptr->oprt_mode != SYS_OPRT_MODE_PWROFF )
           || ph_ptr->oprt_mode == SYS_OPRT_MODE_FTM)
      {
        if (cmd_info_ptr->oprt_mode == SYS_OPRT_MODE_FTM)
        {
          cmnv_item_ptr->ftm_mode = (boolean)FTM_MODE;
        }
        else
        {
          cmnv_item_ptr->ftm_mode = 0;     /* DMSS_MODE / AMSS_MODE */
        }
        cmnv_write( NV_FTM_MODE_I, cmnv_item_ptr );
      } /* if ( prev_oprt_mode == SYS_OPRT_MODE_FTM */
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                     /*---------------------------- */
                     /* Change the measurement mode */
                     /*---------------------------- */

    case CM_PH_CMD_MEAS_MODE:
      #ifdef FEATURE_CM_SS_MEAS
#error code not present
      #endif /* FEATURE_CM_SS_MEAS */
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


                 /*-----------------------------------*/
                 /*     Change the mode preference    */
                 /*-----------------------------------*/

    case CM_PH_CMD_SYS_SEL_PREF:

      /* Validate curr_nam */
      if(!INRANGE(ph_ptr->curr_nam, 0, CM_NAM_AUTO))
      {
        CM_ERR( "invalid curr_nam, PH_CMD:%d",CM_PH_CMD_SYS_SEL_PREF,0,0);
        return;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
      /* If the mode-preference is changed and the change is permanent,
      ** change the persistent mode preference and also write it to NV.
      */
      if( CMPH_PRST_MODE_PREF(ph_ptr) != cmd_info_ptr->mode_pref &&
          cmd_info_ptr->pref_term == CM_PREF_TERM_PERMANENT )
      {
        CMPH_PRST_MODE_PREF( ph_ptr ) = cmd_info_ptr->mode_pref;

        cmnv_item_ptr->pref_mode.mode = cmph_map_mode_pref_cm2nv(
                                          cmd_info_ptr->mode_pref );

        cmnv_item_ptr->pref_mode.nam  = (byte) ph_ptr->curr_nam;

        cmnv_write( NV_PREF_MODE_I, cmnv_item_ptr );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If the band-preference is changed, change the persistent band
      ** preference and also write it to NV.
      */
      if( CMPH_PRST_BAND_PREF(ph_ptr) != cmd_info_ptr->band_pref &&
          cmd_info_ptr->pref_term == CM_PREF_TERM_PERMANENT )
      {
        cm_nv_band_pref_u_type temp;

        temp.dummy =  (uint32)cmd_info_ptr->band_pref;

        CMPH_PRST_BAND_PREF( ph_ptr )  = cmd_info_ptr->band_pref;
        cmnv_item_ptr->band_pref.nam   = (byte) ph_ptr->curr_nam;

        cmnv_item_ptr->band_pref.band  = (nv_band_pref_enum_type)
                                                     temp.nv_band_pref[0];
        cmnv_write( NV_BAND_PREF_I, cmnv_item_ptr );
        cmnv_item_ptr->band_pref.band  = (nv_band_pref_enum_type)
                                                     temp.nv_band_pref[1];
        cmnv_write( NV_BAND_PREF_16_31_I, cmnv_item_ptr );

        /* Move Bits 32-63 into a 32 bit variable so it can be stored.
        */
        cmnv_item_ptr->band_pref_32_63.band  = (uint32) (cmd_info_ptr->band_pref >> 32);

        cmnv_write( NV_BAND_PREF_32_63_I, cmnv_item_ptr );


      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /*
      ** If the prl preference was changed, change the persistent
      ** network selection mode preference and also write it to NV.
      */
      if ( CMPH_PRST_PRL_PREF( ph_ptr ) != cmd_info_ptr->prl_pref &&
           cmd_info_ptr->pref_term == CM_PREF_TERM_PERMANENT
         )
      {
        CMPH_PRST_PRL_PREF( ph_ptr ) = cmd_info_ptr->prl_pref;

        cmnv_item_ptr->prl_pref.prl = (uint32) ph_ptr->prl_pref;

        cmnv_item_ptr->prl_pref.nam = (byte) ph_ptr->curr_nam;

        cmnv_write( NV_PRL_PREF_I, cmnv_item_ptr );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If the roam-preference is changed, change the persistent roam
      ** preference and also write it to NV.
      */
      if( CMPH_PRST_ROAM_PREF(ph_ptr) != cmd_info_ptr->roam_pref &&
          cmd_info_ptr->pref_term == CM_PREF_TERM_PERMANENT )
      {
        CMPH_PRST_ROAM_PREF( ph_ptr )  = cmd_info_ptr->roam_pref;

        cmnv_item_ptr->roam_pref.roam  = (nv_roam_pref_enum_type)
          cmd_info_ptr->roam_pref;

        cmnv_item_ptr->roam_pref.nam   = (byte) ph_ptr->curr_nam;

        cmnv_write( NV_ROAM_PREF_I, cmnv_item_ptr );
      }

      #if (defined (FEATURE_HDR) || defined(FEATURE_HYBR_GW))
#error code not present
      #endif // #ifdef FEATURE_HDR

      /*
      ** If the network selection mode preference was changed, change the persistent
      ** network selection mode preference and also write it to NV.
      */
      if ( CMPH_PRST_NETWORK_SEL_MODE_PREF( ph_ptr ) != cmd_info_ptr->network_sel_mode_pref &&
           cmd_info_ptr->pref_term == CM_PREF_TERM_PERMANENT
         )
      {
        CMPH_PRST_NETWORK_SEL_MODE_PREF( ph_ptr ) = cmd_info_ptr->network_sel_mode_pref;

        cmnv_item_ptr->net_sel_mode_pref.net_sel_mode = (byte) ph_ptr->network_sel_mode_pref;

        cmnv_item_ptr->net_sel_mode_pref.nam          = (byte) ph_ptr->curr_nam;

        cmnv_write( NV_NET_SEL_MODE_PREF_I, cmnv_item_ptr );
      }

      /*
      ** If the acquisition order preference was changed, change the persistent
      ** acquisition order preference and also write it to NV.
      */
      if ( CMPH_PRST_ACQ_ORDER_PREF( ph_ptr ) != cmd_info_ptr->acq_order_pref &&
           cmd_info_ptr->pref_term == CM_PREF_TERM_PERMANENT )
      {
        CMPH_PRST_ACQ_ORDER_PREF( ph_ptr ) = cmd_info_ptr->acq_order_pref;

        cmnv_item_ptr->acq_order_pref.acq_order = (byte) ph_ptr->acq_order_pref;

        cmnv_item_ptr->acq_order_pref.nam       = (byte) ph_ptr->curr_nam;

        cmnv_write( NV_ACQ_ORDER_PREF_I, cmnv_item_ptr );
      }

      if( ( cmd_info_ptr->srv_domain_pref != CM_SRV_DOMAIN_PREF_PS_ATTACH ) &&
          ( cmd_info_ptr->srv_domain_pref != CM_SRV_DOMAIN_PREF_PS_DETACH)  &&
          ( cmd_info_ptr->srv_domain_pref != CM_SRV_DOMAIN_PREF_NO_CHANGE)
        )
      {
        /*
        ** If the service domain preference was changed, change the persistent
        ** service domain preference and also write it to NV.
        */
        if ( CMPH_PRST_SRV_DOMAIN_PREF( ph_ptr ) != cmd_info_ptr->srv_domain_pref &&
             cmd_info_ptr->pref_term == CM_PREF_TERM_PERMANENT )
        {
          CMPH_PRST_SRV_DOMAIN_PREF( ph_ptr ) = cmd_info_ptr->srv_domain_pref;

          cmnv_item_ptr->service_domain_pref.srv_domain = (byte) ph_ptr->srv_domain_pref;

          cmnv_item_ptr->service_domain_pref.nam        = (byte) ph_ptr->curr_nam;

          cmnv_write( NV_SERVICE_DOMAIN_PREF_I, cmnv_item_ptr );
        }
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /*
      ** If the WLAN tech preference was changed, change the persistent
      ** WLAN tech preference and also write it to NV.
      */
      if ( CMPH_PRST_WLAN_TECH_PREF( ph_ptr ) != cmd_info_ptr->wlan_pref.tech_pref &&
           cmd_info_ptr->pref_term == CM_PREF_TERM_PERMANENT
         )
      {
        CMPH_PRST_WLAN_TECH_PREF( ph_ptr ) = cmd_info_ptr->wlan_pref.tech_pref;

        cmnv_item_ptr->wlan_tech_pref.tech_pref = (uint32) ph_ptr->wlan_pref.tech_pref;

        cmnv_item_ptr->wlan_tech_pref.nam = (byte) ph_ptr->curr_nam;

        cmnv_write( NV_WLAN_TECH_PREF_I, cmnv_item_ptr );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_WLAN_ADHOC
#error code not present
      #endif /* FEATURE_WLAN_ADHOC */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /*
      ** If the WLAN scan preference was changed, change the persistent
      ** WLAN scan preference and also write it to NV.
      */
      if ( CMPH_PRST_WLAN_SCAN_PREF( ph_ptr ) != cmd_info_ptr->wlan_pref.scan_mode &&
           cmd_info_ptr->pref_term == CM_PREF_TERM_PERMANENT
         )
      {
        CMPH_PRST_WLAN_SCAN_PREF( ph_ptr ) = cmd_info_ptr->wlan_pref.scan_mode;

        cmnv_item_ptr->wlan_scan_pref.scan_mode = (byte) ph_ptr->wlan_pref.scan_mode;

        cmnv_item_ptr->wlan_scan_pref.nam = (byte) ph_ptr->curr_nam;

        cmnv_write( NV_WLAN_SCAN_PREF_I, cmnv_item_ptr );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /*
      ** If the WLAN net_sel_mode preference is manual and it is permanent,
      ** change the persistent WLAN net_sel_mode preference and also write it
      ** to NV.
      */

      if ( CMPH_PRST_WLAN_NET_SEL_MODE_PREF( ph_ptr ) !=
                                cmd_info_ptr->wlan_pref.network_sel_mode_pref &&
           cmd_info_ptr->pref_term == CM_PREF_TERM_PERMANENT
         )
      {
        CMPH_PRST_WLAN_NET_SEL_MODE_PREF( ph_ptr ) =
                               cmd_info_ptr->wlan_pref.network_sel_mode_pref;

        cmnv_item_ptr->wlan_net_sel_mode_pref.net_sel_mode =
                                         (byte) ph_ptr->wlan_pref.network_sel_mode_pref;

        cmnv_item_ptr->wlan_net_sel_mode_pref.nam = (byte) ph_ptr->curr_nam;

        cmnv_write( NV_WLAN_NET_SEL_MODE_PREF_I, cmnv_item_ptr );

        CMPH_PRST_WLAN_PREF( ph_ptr ).bss_type
                            = cmd_info_ptr->wlan_pref.bss_type;
        CMPH_PRST_WLAN_PREF( ph_ptr ).bss_id
                            = cmd_info_ptr->wlan_pref.bss_id;
        CMPH_PRST_WLAN_PREF( ph_ptr ).ssid
                            = cmd_info_ptr->wlan_pref.ssid;
        CMPH_PRST_WLAN_PREF( ph_ptr ).chan
                            = cmd_info_ptr->wlan_pref.chan;
        CMPH_PRST_WLAN_PREF( ph_ptr ).band
                            = cmd_info_ptr->wlan_pref.band;
        CMPH_PRST_WLAN_PREF( ph_ptr ).profile_id
                            = cmd_info_ptr->wlan_pref.profile_id;
        CMPH_PRST_WLAN_PREF( ph_ptr ).profile
                            = cmd_info_ptr->wlan_pref.profile;

      }

      /* Update the Manual WLAN system in netlist only when the network
      ** selection mode is changed to Manual.
      */
      if ( cmd_info_ptr->wlan_pref.network_sel_mode_pref ==
                                           CM_NETWORK_SEL_MODE_PREF_MANUAL &&
           cmd_info_ptr->pref_term == CM_PREF_TERM_PERMANENT) 
      {

        /* Store WLAN Manual System and Profile information.
        */
        cmph_write_manual_wlan_sys_profile(&ph_ptr->wlan_pref,
                                           &ph_ptr->wlan_pref.profile);
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Notify clients of phone event.
      */
      cmph_event( ph_ptr, CM_PH_EVENT_SYS_SEL_PREF );

      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                 /*-----------------------------------*/
                 /*     Change the DDTM preference  */
                 /*-----------------------------------*/

    case CM_PH_CMD_DDTM_PREF:

      {
        int i=0;

        SYS_ARG_NOT_USED(i);
        /* update the DDTM status */
        cmph_update_ddtm_status(ph_ptr);

        #ifdef FEATURE_DDTM_CNTL
        /* Write the new settings to NV
        */

        cmnv_item_ptr->ddtm_settings.ddtm_act    =
              cmph_map_cm_ddtm_act_mask_to_nv_ddtm_act( ph_ptr->ddtm_act_mask );

        cmnv_item_ptr->ddtm_settings.ddtm_pref   =
                     cmph_map_cm_ddtm_pref_to_nv_ddtm_pref( ph_ptr->ddtm_pref );

        cmnv_item_ptr->ddtm_settings.num_srv_opt = ph_ptr->ddtm_num_srv_opt;

        /* Initialize the list of service option pages to none
        */

        /* Cast is to prevent "implicit cast of pointer loses '__packed' qualifier"
        ** compiler error  C2906E
        */
        memset( (void*)cmnv_item_ptr->ddtm_settings.srv_opt_list, 0,
                sizeof(cmnv_item_ptr->ddtm_settings.srv_opt_list) );

        for( i=0; i<ph_ptr->ddtm_num_srv_opt; i++ )
        {
          cmnv_item_ptr->ddtm_settings.srv_opt_list[i] =
                                                ph_ptr->ddtm_srv_opt_list[i];
        }
        cmnv_write( NV_DDTM_SETTINGS_I, cmnv_item_ptr );
        #endif

        /* Notify clients of phone event */
        cmph_event( ph_ptr, CM_PH_EVENT_DDTM_PREF );
        CM_MSG_HIGH("CM_PH_EVENT_DDTM_PREF %d",ph_ptr->ddtm_pref,0,0);

      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

              /*-----------------------------------------*/
              /* Change the answer-voice-as-data setting */
              /*-----------------------------------------*/

    case CM_PH_CMD_ANSWER_VOICE:

      /* Put the new answer-voice setting in the data base.
      ** MC consults it upon receiving a page.
      */
      #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
      db_item.voice_as_data = cmph_map_voice_answer_cm2db( cmd_info_ptr->answer_voice );
      db_put( DB_VOICE_AS_DATA, &db_item );

      cmph_event( ph_ptr, CM_PH_EVENT_ANSWER_VOICE );
      #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /*--------------------------*/
                      /* Change the NAM selection */
                      /*--------------------------*/

    case CM_PH_CMD_NAM_SEL:

      /* Update the NAM dependent variables, such as
      ** the mode preference and the system preference
      ** in accordance with the new NAM selection.
      */
      if(!INRANGE(ph_ptr->curr_nam, 0, NV_MAX_NAMS))
      {
        CM_ERR( "invlid nam: %d", ph_ptr->curr_nam,0,0);
        break;
      }
      ph_ptr->mode_pref = CMPH_PRST_MODE_PREF( ph_ptr );
      ph_ptr->band_pref  = CMPH_PRST_BAND_PREF( ph_ptr );
      ph_ptr->roam_pref  = CMPH_PRST_ROAM_PREF( ph_ptr );

      /* If new selection is AUTO-NAM write to NV
      ** and notify clients of NAM-SELECTION event.
      */
      if( cmd_info_ptr->nam_sel == CM_NAM_AUTO )
      {
        cmnv_item_ptr->auto_nam = TRUE;
        cmnv_write( NV_AUTO_NAM_I, cmnv_item_ptr );

        cmph_event( ph_ptr, CM_PH_EVENT_NAM_SEL );
      }

      /* Else, write the new NAM selection to NV, and notify
      ** clients of NAM-SELECTION, as well as, CURR-NAM events.
      */
      else
      {
        cmnv_item_ptr->auto_nam = FALSE;
        cmnv_write( NV_AUTO_NAM_I, cmnv_item_ptr );

        cmnv_item_ptr->curr_nam = (byte) cmd_info_ptr->nam_sel;
        cmnv_write( NV_CURR_NAM_I, cmnv_item_ptr );

        cmph_event( ph_ptr, CM_PH_EVENT_NAM_SEL );
        cmph_event( ph_ptr, CM_PH_EVENT_CURR_NAM );
      }


      /* Read NV items for the new NAM
      */
      cmph_read_nv_items(ph_ptr);

      (void)cmph_insert_orig_mode (
                                    CM_SS_MAIN,
                                    SD_SS_ORIG_MODE_NONE,
                                    CM_ACT_TYPE_PH_OBJ,
                                    ph_ptr->pref_term,
                                    ph_ptr->mode_pref,
                                    ph_ptr->band_pref,
                                    ph_ptr->prl_pref,
                                    ph_ptr->roam_pref,
                                    ph_ptr->hybr_pref,
                                    ph_ptr->int_hybr_pref,
                                    &ph_ptr->wlan_pref,
                                    ph_ptr->srv_domain_pref,
                                    (cm_act_id_type)ph_ptr,
                                    CM_ACT_UPDATE_REAS_ACT_START
                                  );
      /* Add in the orig Q the ph_ptr values
      */

      (void)cmph_insert_orig_mode (
                                    CM_SS_HDR,
                                    SD_SS_ORIG_MODE_NONE,
                                    CM_ACT_TYPE_PH_OBJ,
                                    ph_ptr->pref_term,
                                    ph_ptr->mode_pref,
                                    ph_ptr->band_pref,
                                    ph_ptr->prl_pref,
                                    ph_ptr->roam_pref,
                                    ph_ptr->hybr_pref,
                                    ph_ptr->int_hybr_pref,
                                    &ph_ptr->wlan_pref,
                                    ph_ptr->srv_domain_pref,
                                    (cm_act_id_type)ph_ptr,
                                    CM_ACT_UPDATE_REAS_ACT_START
                                  );
      /* Add in the orig Q the ph_ptr values
      */

      (void)cmph_insert_orig_mode (
                                    CM_SS_WLAN,
                                    SD_SS_ORIG_MODE_NONE,
                                    CM_ACT_TYPE_PH_OBJ,
                                    ph_ptr->pref_term,
                                    ph_ptr->mode_pref,
                                    ph_ptr->band_pref,
                                    ph_ptr->prl_pref,
                                    ph_ptr->roam_pref,
                                    ph_ptr->hybr_pref,
                                    ph_ptr->int_hybr_pref,
                                    &ph_ptr->wlan_pref,
                                    ph_ptr->srv_domain_pref,
                                    (cm_act_id_type)ph_ptr,
                                    CM_ACT_UPDATE_REAS_ACT_START
                                  );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                    /*------------------------------*/
                    /* A CM Client activated itself */
                    /*------------------------------*/

    case CM_PH_CMD_CLIENT_ACT:

      /* Notify client that phone information
      ** is now available.
      */
      {
        /* Allocate phone info buffer,
        ** copy phone state information into allocated buffer,
        ** and notify client directly of event.
        */
        ph_info_ptr = cmph_info_alloc();

        CM_ASSERT( ph_info_ptr != NULL );

        (void) cmph_info_get( ph_info_ptr );

        CM_ASSERT( ph_cmd_ptr->client_ptr != NULL );
        cm_client_ph_event_ntfy( ph_cmd_ptr->client_ptr,
                                 CM_PH_EVENT_INFO_AVAIL,
                                 ph_info_ptr );
      }
      break;



    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*------------------------------------------*/
                   /* Send phone info to the requesting client */
                   /*------------------------------------------*/

    case CM_PH_CMD_INFO_GET:


      /* get a buffer for phone info */

      ph_info_ptr = cmph_info_alloc();

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy phone information into
      ** allocated phone info buffer.
      */
      (void) cmph_info_get( ph_info_ptr );


      /* notify only the requesting client
         Note: Client ID is set to the beginning address
               of the client object during the client initialization
      */
      cm_client_ph_event_ntfy(ph_cmd_ptr->client_ptr,
                              CM_PH_EVENT_INFO,
                              ph_info_ptr );

      break;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /*----------------------------------*/
      /* Set RSSI change delta threshold  */
      /*----------------------------------*/

    case CM_PH_CMD_RSSI_DELTA:

      /* get the phone object pointer */

      ph_ptr                = cmph_ptr();
      ph_ptr->rssi_delta    = cmd_info_ptr->rssi_delta;

      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      /*------------------------------------------ */
      /* Set RSSI,ECIO,IO change delta threshold   */
      /*-------------------------------------------*/

    case CM_PH_CMD_SIGNAL_STRENGTH_DELTA:

      /* get the phone object pointer */

      ph_ptr                = cmph_ptr();

      /* Update the thresholds if applicable
      */
      if( cmd_info_ptr->rssi_delta != CM_SIG_STRENGTH_DELTA_NO_CHANGE )
      {
        ph_ptr->rssi_delta    = cmd_info_ptr->rssi_delta;
      }

      if( cmd_info_ptr->ecio_delta != CM_SIG_STRENGTH_DELTA_NO_CHANGE )
      {
        ph_ptr->ecio_delta    = cmd_info_ptr->ecio_delta;
      }

      if( cmd_info_ptr->io_delta != CM_SIG_STRENGTH_DELTA_NO_CHANGE )
      {
        ph_ptr->io_delta    = cmd_info_ptr->io_delta;
      }

      if( cmd_info_ptr->sir_delta != CM_SIG_STRENGTH_DELTA_NO_CHANGE )
      {
        ph_ptr->sir_delta          = cmd_info_ptr->sir_delta;
      }

      if( cmd_info_ptr->pathloss_delta != CM_SIG_STRENGTH_DELTA_NO_CHANGE )
      {
        ph_ptr->pathloss_delta          = cmd_info_ptr->pathloss_delta;
      }

      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*---------------------*/
                     /* Change packet state */
                     /*---------------------*/

    case CM_PH_CMD_PACKET_STATE:
      cmph_event( ph_ptr, CM_PH_EVENT_PACKET_STATE );
      break;



    #ifdef FEATURE_JCDMA
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*----------------------------------*/
                   /* Enable/Disable voice activity    */
                   /*----------------------------------*/

      case CM_PH_CMD_VOC_ACTIVITY:

        /* do nothing - this request terminated at CM */
      break;

    #endif /* FEATURE_JCDMA */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PH_CMD_AVOID_SYS:
      /* do nothing - this request terminated at CM */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PH_CMD_WAKEUP_FROM_STANDBY:
      cmph_event( ph_ptr, CM_PH_EVENT_WAKEUP_FROM_STANDBY );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if ( defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) )
    case CM_PH_CMD_CHANGE_RTRE_CONFIG:
      ph_ptr->rtre_control = cmd_ptr->cmd.ph.info.rtre_control;
      ph_ptr->rtre_config = cmd_ptr->cmd.ph.info.rtre_config;

      /* Update spc
      */
      cmph_read_nv_items( ph_ptr );

      cmph_event( ph_ptr, CM_PH_EVENT_NVRUIM_CONFIG_CHANGED );

      break;
    #endif /* defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

              /*------------------------*/
              /* Subscription Available */
              /*------------------------*/

    case CM_PH_CMD_SUBSCRIPTION_AVAILABLE:
    {
      sd_ss_mode_pref_e_type sd_mode_pref =
                cmph_map_cm_mode_pref_to_sd_mode_pref(ph_ptr->mode_pref);
      /*does not allow the subscription to go through if the technology
      **is not in the mode pref and the subscription is buffered
      **exception is all are allowed if even one subscription is allowed
      */
      int invalid_subsc_count = 0;

      if(cmd_info_ptr->cdma_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
         !(sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_CDMA_HDR)))
      {
        if(ph_ptr->is_pending_subsc_cdma == TRUE &&
           ph_ptr->cdma_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE)
        {

          ph_ptr->is_pending_subsc_cdma = FALSE;
          ph_ptr->cdma_subsc_cmd = CM_PH_CMD_NONE;

        }
        else
        {
          ph_ptr->is_pending_subsc_cdma = TRUE;
          ph_ptr->cdma_subsc_cmd = CM_PH_CMD_SUBSCRIPTION_AVAILABLE;
          ph_ptr->cdma_client_id = cmd_ptr->cmd.ph.client_id;
        }
        invalid_subsc_count++;
      }
      else if (cmd_info_ptr->cdma_sub_avail_status != CM_SUBSCRIPTION_STATUS_CHANGE)
      {
        invalid_subsc_count++;
      }


      if(cmd_info_ptr->gw_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
         !(sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_GW)))
      {
        if(ph_ptr->is_pending_subsc_gw == TRUE &&
           ph_ptr->gw_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE)
        {
            ph_ptr->is_pending_subsc_gw = FALSE;
            ph_ptr->gw_subsc_cmd = CM_PH_CMD_NONE;
        }
        else
        {
          ph_ptr->is_pending_subsc_gw = TRUE;
          ph_ptr->gw_subsc_cmd = CM_PH_CMD_SUBSCRIPTION_AVAILABLE;
          ph_ptr->gw_client_id = cmd_ptr->cmd.ph.client_id;
        }

        invalid_subsc_count++;
      }
      else if (cmd_info_ptr->gw_sub_avail_status != CM_SUBSCRIPTION_STATUS_CHANGE)
      {
        invalid_subsc_count++;
      }


      if(cmd_info_ptr->wlan_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
         !(sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_WLAN)))
      {
        if(ph_ptr->is_pending_subsc_wlan == TRUE &&
           ph_ptr->wlan_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE)
        {
            ph_ptr->is_pending_subsc_wlan = FALSE;
            ph_ptr->wlan_subsc_cmd = CM_PH_CMD_NONE;

        }
        else
        {
          ph_ptr->is_pending_subsc_wlan = TRUE;
          ph_ptr->wlan_subsc_cmd = CM_PH_CMD_SUBSCRIPTION_AVAILABLE;
          ph_ptr->wlan_client_id = cmd_ptr->cmd.ph.client_id;
        }

        invalid_subsc_count++;
      }
      else if (cmd_info_ptr->wlan_sub_avail_status != CM_SUBSCRIPTION_STATUS_CHANGE)
      {
        invalid_subsc_count++;
      }
      /*break if no commands to be sent
      */
      if(invalid_subsc_count == 3)
      {
        break;
      }
      /*reset buffers if command is being sent
      */
      if(ph_ptr->is_pending_subsc_cdma &&
         ph_ptr->cdma_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE)
      {

        ph_ptr->is_pending_subsc_cdma = FALSE;
        ph_ptr->cdma_subsc_cmd = CM_PH_CMD_NONE;
      }

      if(ph_ptr->is_pending_subsc_gw &&
         ph_ptr->gw_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE)
      {

        ph_ptr->is_pending_subsc_gw = FALSE;
        ph_ptr->gw_subsc_cmd = CM_PH_CMD_NONE;
      }

      if(ph_ptr->is_pending_subsc_wlan &&
         ph_ptr->wlan_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE)
      {

        ph_ptr->is_pending_subsc_wlan = FALSE;
        ph_ptr->wlan_subsc_cmd = CM_PH_CMD_NONE;
      }


      if ( cmd_info_ptr->wlan_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
      {
        ph_ptr->is_wlan_subscription_available = TRUE;
        CM_MSG_MED("wlan sub a",0,0,0);
      }
      if ( cmd_info_ptr->gw_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
      {
        ph_ptr->is_gw_subscription_available = TRUE;
        CM_MSG_MED("gw sub a",0,0,0);
      }
      if ( cmd_info_ptr->cdma_sub_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
      {
        ph_ptr->is_cdma_subscription_available = TRUE;
        CM_MSG_MED("cdma sub a",0,0,0);
      }

      /* Update spc
      */
      cmph_read_nv_items( ph_ptr );

      #if (defined FEATURE_ALS) && (defined FEATURE_GSM) && (!defined FEATURE_HYBR_GW )
#error code not present
      #else
      cmph_event( ph_ptr, CM_PH_EVENT_SUBSCRIPTION_AVAILABLE );
      #endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */

      break;
    }

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
              /*----------------------------*/
              /* Subscription Not Available */
              /*----------------------------*/

    case CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE:
    {
      sd_ss_mode_pref_e_type sd_mode_pref =
                cmph_map_cm_mode_pref_to_sd_mode_pref(ph_ptr->mode_pref);
      /*does not allow the subscription to go through if the technology
      **is not in the mode pref and the subscription is buffered
      **exception is all are allowed if even one subscription is allowed
      */
      int invalid_subsc_count = 0;

      if(cmd_info_ptr->cdma_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
         !(sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_CDMA_HDR)))
      {
        if(ph_ptr->is_pending_subsc_cdma == TRUE &&
           (ph_ptr->cdma_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE ||
            ph_ptr->cdma_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED &&
            !ph_ptr->is_cdma_subscription_available))
        {

            ph_ptr->is_pending_subsc_cdma = FALSE;
            ph_ptr->cdma_subsc_cmd = CM_PH_CMD_NONE;
        }
        else
        {
          ph_ptr->is_pending_subsc_cdma = TRUE;
          ph_ptr->cdma_subsc_cmd = CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE;
          ph_ptr->cdma_client_id = cmd_ptr->cmd.ph.client_id;
        }

        invalid_subsc_count++;
      }
      else if (cmd_info_ptr->cdma_sub_not_avail_status != CM_SUBSCRIPTION_STATUS_CHANGE)
      {
        invalid_subsc_count++;
      }


      if(cmd_info_ptr->gw_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
         !(sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_GW)))
      {
        if(ph_ptr->is_pending_subsc_gw == TRUE &&
           (ph_ptr->gw_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE ||
            ph_ptr->gw_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED &&
            !ph_ptr->is_gw_subscription_available))
        {
            ph_ptr->is_pending_subsc_gw = FALSE;
            ph_ptr->gw_subsc_cmd = CM_PH_CMD_NONE;

        }
        else
        {
          ph_ptr->is_pending_subsc_gw = TRUE;
          ph_ptr->gw_subsc_cmd = CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE;
          ph_ptr->gw_client_id = cmd_ptr->cmd.ph.client_id;
        }

        invalid_subsc_count++;
      }
      else if (cmd_info_ptr->gw_sub_not_avail_status != CM_SUBSCRIPTION_STATUS_CHANGE)
      {
        invalid_subsc_count++;
      }


      if(cmd_info_ptr->wlan_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
         !(sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_WLAN)))
      {
        if(ph_ptr->is_pending_subsc_wlan == TRUE &&
           (ph_ptr->wlan_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE ||
            ph_ptr->wlan_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED &&
            !ph_ptr->is_wlan_subscription_available))
        {
            ph_ptr->is_pending_subsc_wlan = FALSE;
            ph_ptr->wlan_subsc_cmd = CM_PH_CMD_NONE;

        }
        else
        {
          ph_ptr->is_pending_subsc_wlan = TRUE;
          ph_ptr->wlan_subsc_cmd = CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE;
          ph_ptr->wlan_client_id = cmd_ptr->cmd.ph.client_id;
        }

        invalid_subsc_count++;
      }
      else if (cmd_info_ptr->wlan_sub_not_avail_status != CM_SUBSCRIPTION_STATUS_CHANGE)
      {
        invalid_subsc_count++;
      }
      /*break if no commands to be sent
      */
      if(invalid_subsc_count == 3)
      {
        break;
      }
      /*reset buffers if command is being sent
      */
      if(ph_ptr->is_pending_subsc_cdma &&
         ph_ptr->cdma_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE)
      {

        ph_ptr->is_pending_subsc_cdma = FALSE;
        ph_ptr->cdma_subsc_cmd = CM_PH_CMD_NONE;
      }

      if(ph_ptr->is_pending_subsc_gw &&
         ph_ptr->gw_subsc_cmd ==CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE)
      {

        ph_ptr->is_pending_subsc_gw = FALSE;
        ph_ptr->gw_subsc_cmd = CM_PH_CMD_NONE;
      }

      if(ph_ptr->is_pending_subsc_wlan &&
         ph_ptr->wlan_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE)
      {

        ph_ptr->is_pending_subsc_wlan = FALSE;
        ph_ptr->wlan_subsc_cmd = CM_PH_CMD_NONE;
      }


      if ( cmd_info_ptr->gw_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
      {
        ph_ptr->is_gw_subscription_available = FALSE;
        CM_MSG_MED("gw sub na",0,0,0);
      }
      if ( cmd_info_ptr->cdma_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
      {
        ph_ptr->is_cdma_subscription_available = FALSE;
        CM_MSG_MED("cdma sub na",0,0,0);
      }
      if ( cmd_info_ptr->wlan_sub_not_avail_status == CM_SUBSCRIPTION_STATUS_CHANGE )
      {
        ph_ptr->is_wlan_subscription_available = FALSE;
        CM_MSG_MED("wlan sub na",0,0,0);
      }

      #if (defined FEATURE_ALS) && (defined FEATURE_GSM) && (!defined FEATURE_HYBR_GW )
#error code not present
      #endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */

      #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
      #endif

      cmph_event( ph_ptr, CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE );
      break;
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
              /*------------------------*/
              /* Subscription Changed */
              /*------------------------*/

    case CM_PH_CMD_SUBSCRIPTION_CHANGED:
    {
      sd_ss_mode_pref_e_type sd_mode_pref =
                cmph_map_cm_mode_pref_to_sd_mode_pref(ph_ptr->mode_pref);
      /*does not allow the subscription to go through if the technology
        is not in the mode pref and the subscription is buffered
        exception is all are allowed if even one subscription is allowed
      */
      int invalid_subsc_count=0;

      if(cmd_info_ptr->cdma_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
         !(sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_CDMA_HDR)))
      {
          ph_ptr->is_pending_subsc_cdma = TRUE;
          ph_ptr->cdma_subsc_cmd = CM_PH_CMD_SUBSCRIPTION_CHANGED;
          ph_ptr->cdma_client_id = cmd_ptr->cmd.ph.client_id;

        invalid_subsc_count++;
      }
      else if (cmd_info_ptr->cdma_sub_changed_status != CM_SUBSCRIPTION_STATUS_CHANGE)
      {
        invalid_subsc_count++;
      }


      if(cmd_info_ptr->gw_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
         !(sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_GW)))
      {
          ph_ptr->is_pending_subsc_gw = TRUE;
          ph_ptr->gw_subsc_cmd = CM_PH_CMD_SUBSCRIPTION_CHANGED;
          ph_ptr->gw_client_id = cmd_ptr->cmd.ph.client_id;
        invalid_subsc_count++;
      }
      else if (cmd_info_ptr->gw_sub_changed_status != CM_SUBSCRIPTION_STATUS_CHANGE)
      {
        invalid_subsc_count++;
      }


      if(cmd_info_ptr->wlan_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE && \
         !(sd_misc_is_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_WLAN)))
      {
          ph_ptr->is_pending_subsc_wlan = TRUE;
          ph_ptr->wlan_subsc_cmd = CM_PH_CMD_SUBSCRIPTION_CHANGED;
          ph_ptr->wlan_client_id = cmd_ptr->cmd.ph.client_id;
        invalid_subsc_count++;
      }
      else if (cmd_info_ptr->wlan_sub_changed_status != CM_SUBSCRIPTION_STATUS_CHANGE)
      {
        invalid_subsc_count++;
      }
      /*break if no commands to be sent
      */
      if(invalid_subsc_count == 3)
      {
        break;
      }
      /*reset buffers if command is being sent
      */
      if(ph_ptr->is_pending_subsc_cdma &&
         ph_ptr->cdma_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED)
      {

        ph_ptr->is_pending_subsc_cdma = FALSE;
        ph_ptr->cdma_subsc_cmd = CM_PH_CMD_NONE;
      }

      if(ph_ptr->is_pending_subsc_gw &&
         ph_ptr->gw_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED)
      {

        ph_ptr->is_pending_subsc_gw = FALSE;
        ph_ptr->gw_subsc_cmd = CM_PH_CMD_NONE;
      }

      if(ph_ptr->is_pending_subsc_wlan &&
         ph_ptr->wlan_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED)
      {

        ph_ptr->is_pending_subsc_wlan = FALSE;
        ph_ptr->wlan_subsc_cmd = CM_PH_CMD_NONE;
      }


      if ( cmd_info_ptr->gw_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE )
      {
        ph_ptr->is_gw_subscription_available = TRUE;
        CM_MSG_MED("gw sub ch -> a",0,0,0);
      }
      if ( cmd_info_ptr->cdma_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE )
      {
        ph_ptr->is_cdma_subscription_available = TRUE;
        CM_MSG_MED("cdma sub ch -> a",0,0,0);
      }
      if ( cmd_info_ptr->wlan_sub_changed_status == CM_SUBSCRIPTION_STATUS_CHANGE )
      {
        ph_ptr->is_wlan_subscription_available = TRUE;
        CM_MSG_MED("wlan sub ch -> a",0,0,0);
      }

      /* Update spc
      */
      cmph_read_nv_items( ph_ptr );

      #if (defined FEATURE_ALS) && (defined FEATURE_GSM) && (!defined FEATURE_HYBR_GW )
#error code not present
      #else
      cmph_event( ph_ptr, CM_PH_EVENT_SUBSCRIPTION_CHANGED );
      #endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */

      break;
    }
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
              /*-----------------------------------------*/
              /* Client has requested a list of networks */
              /*-----------------------------------------*/


    case CM_PH_CMD_GET_NETWORKS:
      if ( cmd_info_ptr->network_list_type == CM_NETWORK_LIST_USER_PREFERRED ||
           cmd_info_ptr->network_list_type == CM_NETWORK_LIST_USER_PREFERRED_UNPACKED)
      {

        cmph_set_net_lists_present_flag(TRUE);

        /* Sending an event to the client notifying that the SIM values are available */
        cmph_event (ph_ptr, CM_PH_EVENT_PREFERRED_NETWORKS_CONF);

        cmph_set_net_lists_present_flag(FALSE);

        ph_ptr->network_type = CM_MODE_PREF_NONE;
      }
      break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
              /*-----------------------------------------*/
              /* Client has requested to terminate the   */
              /* acquisition of the available networks   */
              /*-----------------------------------------*/

    case CM_PH_CMD_TERMINATE_GET_NETWORKS:
      cmph_term_pending_get_networks_req(SD_SS_MAX);
      break;

     
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
              /*-----------------------------------------------*/
              /* Client has requested WPS security credentials */
              /*-----------------------------------------------*/
    case CM_PH_CMD_WPS_START:
      #ifdef FEATURE_WLAN_WPS
#error code not present
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

              /*-----------------------------------------*/
              /* Client has requested to abort the       */
              /* acquisition of the WPS credentials      */
              /*-----------------------------------------*/

    case CM_PH_CMD_WPS_ABORT:
      #ifdef FEATURE_WLAN_WPS
#error code not present
      #endif
      break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
      #endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
    #endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PH_CMD_CLIENT_RELEASE:

      /* Remove the client object from the client queue. */
      cmclient_list_del( cmd_info_ptr->unused_client_ptr );

      /* Free the client object */
      cmclient_free_client_buf( cmd_info_ptr->unused_client_ptr );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      CM_SWITCH_ERR( "Bad phone command %d", ph_cmd_ptr->cmd, 0, 0);
      break;

  } /* switch */
} /* cmph_client_cmd_mmll_reply() */
/*lint +esym(715, cmd_err) */


/*===========================================================================

FUNCTION cmph_get_last_pref_sent_on_main

DESCRIPTION
  Gets a pointer to the last pref sent to SD

DEPENDENCIES


RETURN VALUE
 Pointer to the last origination pref sent to SD

SIDE EFFECTS
  None.

===========================================================================*/
cm_act_orig_s_type* cmph_get_last_pref_sent_on_main()
{
  static cm_act_orig_s_type    last_orig_pref_sent;
    /* Last origination parameters sent to SD
    */
  return &last_orig_pref_sent;

}

/*===========================================================================

FUNCTION cmph_cmd_ptr

DESCRIPTION
  Gets a pointer to a command

DEPENDENCIES


RETURN VALUE
 Pointer to the a command

SIDE EFFECTS
  None.

===========================================================================*/
static cm_cmd_s_type *cmph_cmd_ptr(void)
{
  static cm_cmd_s_type ph_cmd_ptr;

  return &ph_cmd_ptr;
}

/*===========================================================================

FUNCTION cmph_process_pending_subscription_cmds

DESCRIPTION
  process any remaining subscription commands and sends to lower layers
  if it is allowed

DEPENDENCIES


RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
static void cmph_process_pending_subscription_cmds(

   cmph_s_type                     *ph_ptr,

   sd_ss_mode_pref_e_type          mode_pref
)
{
  cm_cmd_s_type *cmd_ptr = cmph_cmd_ptr();
  /*a command pointer to send the subscription change*/
  cm_ph_cmd_err_e_type cmd_err = CM_PH_CMD_ERR_NOERR;

  cm_orig_q_s_type  *ph_orig_ptr_main =
         cmtask_orig_para_search_act_id( CM_SS_MAIN, (cm_act_id_type)ph_ptr );

  cm_orig_q_s_type  *ph_orig_ptr_hybr =
     cmtask_orig_para_search_act_id( CM_SS_HYBR_1 , (cm_act_id_type)ph_ptr );

  cm_orig_q_s_type  *ph_orig_ptr_wlan =
           cmtask_orig_para_search_act_id(CM_SS_WLAN,(cm_act_id_type)ph_ptr);

  cm_orig_q_s_type  *ph_orig_top_ptr_main =
                                       cmtask_orig_para_get_top( CM_SS_MAIN);

  cm_orig_q_s_type  *ph_orig_top_ptr_hybr =
                                    cmtask_orig_para_get_top( CM_SS_HYBR_1 );

  cm_orig_q_s_type  *ph_orig_top_ptr_wlan =
                                       cmtask_orig_para_get_top(CM_SS_WLAN );

  CM_ASSERT( ph_orig_ptr_main != NULL );
  CM_ASSERT( ph_orig_ptr_hybr != NULL );
  CM_ASSERT( ph_orig_ptr_wlan != NULL );
  CM_ASSERT( ph_orig_top_ptr_main != NULL );
  CM_ASSERT( ph_orig_top_ptr_hybr != NULL );
  CM_ASSERT( ph_orig_top_ptr_wlan != NULL );

  /* Initialize the command buffer with the PH fields
  */
  cm_cmd_ph_init(cmd_ptr);


  /* make sure that there is no call in progress
  */
  if( ph_orig_ptr_main == ph_orig_top_ptr_main &&
      ph_orig_ptr_hybr == ph_orig_top_ptr_hybr &&
      ph_orig_ptr_wlan == ph_orig_top_ptr_wlan)
  {
    /*set the command pointer to have the right information
    */
    cmd_ptr->cmd_type = CM_CMD_TYPE_PH;
    cmd_ptr->cmd.ph.info.oprt_mode = ph_ptr->oprt_mode;
    cmd_ptr->cmd.ph.info.mode_pref = ph_ptr->mode_pref;

    /*check if we have any pending subscription commands
    */
    if(  ph_ptr->is_pending_subsc_cdma || \
         ph_ptr->is_pending_subsc_gw || \
         ph_ptr->is_pending_subsc_wlan )
    {
      cmd_err = CM_PH_CMD_ERR_NOERR;
      cmd_ptr->cmd.ph.cmd = CM_PH_CMD_NONE;

      /*go through all the technologies and check for subscription
      **available command pending
      */
      if(ph_ptr->is_pending_subsc_cdma && \
         ph_ptr->cdma_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE && \
         (sd_misc_is_mode_pref(mode_pref,SD_SS_MODE_PREF_CDMA_HDR)))
      {
        cmd_ptr->cmd.ph.cmd = CM_PH_CMD_SUBSCRIPTION_AVAILABLE;
        cmd_ptr->cmd.ph.info.cdma_sub_avail_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        cmd_ptr->cmd.ph.client_id = ph_ptr->cdma_client_id;
      }
      else
      {
        cmd_ptr->cmd.ph.info.cdma_sub_avail_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      }
      if (ph_ptr->is_pending_subsc_gw &&
          ph_ptr->gw_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE &&
          (sd_misc_is_mode_pref(mode_pref,SD_SS_MODE_PREF_GSM) ||
           sd_misc_is_mode_pref(mode_pref,SD_SS_MODE_PREF_WCDMA)))
      {
        cmd_ptr->cmd.ph.cmd = CM_PH_CMD_SUBSCRIPTION_AVAILABLE;
        cmd_ptr->cmd.ph.info.gw_sub_avail_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        cmd_ptr->cmd.ph.client_id = ph_ptr->gw_client_id;
      }
      else
      {
        cmd_ptr->cmd.ph.info.gw_sub_avail_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      }
      if ( ph_ptr->is_pending_subsc_wlan &&
           ph_ptr->wlan_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE &&
          (sd_misc_is_mode_pref(mode_pref,SD_SS_MODE_PREF_WLAN)))
      {
        cmd_ptr->cmd.ph.cmd = CM_PH_CMD_SUBSCRIPTION_AVAILABLE;
        cmd_ptr->cmd.ph.info.wlan_sub_avail_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        cmd_ptr->cmd.ph.client_id = ph_ptr->wlan_client_id;
      }
      else
      {
        cmd_ptr->cmd.ph.info.wlan_sub_avail_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      }
      /*send command if there are any subscription available commands that
      **need to be sent
      */
      if(cmd_ptr->cmd.ph.cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE)
      {
        (void) cmph_client_cmd_forward_mmll(ph_ptr,cmd_ptr,TRUE);
        cmph_client_cmd_mmll_reply(ph_ptr,cmd_ptr,cmd_err);
      }

      cmd_ptr->cmd.ph.cmd = CM_PH_CMD_NONE;

      /*go through all the technologies and check for subscription
      **not available command pending
      */
      if( ph_ptr->is_pending_subsc_cdma && \
          ph_ptr->cdma_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE && \
         (sd_misc_is_mode_pref(mode_pref,SD_SS_MODE_PREF_CDMA_HDR)))
      {
        cmd_ptr->cmd.ph.cmd = CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE;
        cmd_ptr->cmd.ph.info.cdma_sub_not_avail_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        cmd_ptr->cmd.ph.client_id = ph_ptr->cdma_client_id;
      }
      else
      {
        cmd_ptr->cmd.ph.info.cdma_sub_not_avail_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      }
      if( ph_ptr->is_pending_subsc_gw &&
          ph_ptr->gw_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE &&
          (sd_misc_is_mode_pref(mode_pref,SD_SS_MODE_PREF_GSM) ||\
           sd_misc_is_mode_pref(mode_pref,SD_SS_MODE_PREF_WCDMA)))
      {
        cmd_ptr->cmd.ph.cmd = CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE;
        cmd_ptr->cmd.ph.info.gw_sub_not_avail_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        cmd_ptr->cmd.ph.client_id = ph_ptr->gw_client_id;
      }
      else
      {
        cmd_ptr->cmd.ph.info.gw_sub_not_avail_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      }
      if( ph_ptr->is_pending_subsc_wlan && \
          ph_ptr->wlan_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE&&\
          (sd_misc_is_mode_pref(mode_pref,SD_SS_MODE_PREF_WLAN)))
      {
        cmd_ptr->cmd.ph.cmd = CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE;
        cmd_ptr->cmd.ph.info.wlan_sub_not_avail_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        cmd_ptr->cmd.ph.client_id = ph_ptr->wlan_client_id;
      }
      else
      {
        cmd_ptr->cmd.ph.info.wlan_sub_not_avail_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      }
      /*send command if there are any subscription not available commands
      **that need to be sent
      */
      if( cmd_ptr->cmd.ph.cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE)
      {
        (void) cmph_client_cmd_forward_mmll(ph_ptr,cmd_ptr,TRUE);
        cmph_client_cmd_mmll_reply(ph_ptr,cmd_ptr,cmd_err);
      }

      cmd_ptr->cmd.ph.cmd = CM_PH_CMD_NONE;

      /*go through all the technologies and check for subscription
      **changed command pending
      */
      if( ph_ptr->is_pending_subsc_cdma && \
          ph_ptr->cdma_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED && \
         (sd_misc_is_mode_pref(mode_pref,SD_SS_MODE_PREF_CDMA_HDR)))
      {
        cmd_ptr->cmd.ph.cmd = CM_PH_CMD_SUBSCRIPTION_CHANGED;
        cmd_ptr->cmd.ph.info.cdma_sub_changed_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        cmd_ptr->cmd.ph.client_id = ph_ptr->cdma_client_id;
      }
      else
      {
        cmd_ptr->cmd.ph.info.cdma_sub_changed_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      }
      if( ph_ptr->is_pending_subsc_gw &&
          ph_ptr->gw_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED&&
          (sd_misc_is_mode_pref(mode_pref,SD_SS_MODE_PREF_GSM) ||
           sd_misc_is_mode_pref(mode_pref,SD_SS_MODE_PREF_WCDMA)))
      {
        cmd_ptr->cmd.ph.cmd = CM_PH_CMD_SUBSCRIPTION_CHANGED;
        cmd_ptr->cmd.ph.info.gw_sub_changed_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        cmd_ptr->cmd.ph.client_id = ph_ptr->gw_client_id;
      }
      else
      {
        cmd_ptr->cmd.ph.info.gw_sub_changed_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      }
      if( ph_ptr->is_pending_subsc_wlan &&
          ph_ptr->wlan_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED &&
          (sd_misc_is_mode_pref(mode_pref,SD_SS_MODE_PREF_WLAN)))
      {
        cmd_ptr->cmd.ph.cmd = CM_PH_CMD_SUBSCRIPTION_CHANGED;
        cmd_ptr->cmd.ph.info.wlan_sub_changed_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        cmd_ptr->cmd.ph.client_id = ph_ptr->wlan_client_id;
      }
      else
      {
        cmd_ptr->cmd.ph.info.wlan_sub_changed_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      }
      /*send command if there are any subscription changed commands
      **that need to be sent
      */
      if(cmd_ptr->cmd.ph.cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED)
      {
        (void) cmph_client_cmd_forward_mmll(ph_ptr,cmd_ptr,TRUE);
        cmph_client_cmd_mmll_reply(ph_ptr,cmd_ptr,cmd_err);
      }
    } /* ph_ptr->is_pending_subsc_cdma */
  } /* ph_orig_ptr_main */

}

/*===========================================================================

FUNCTION cmph_force_orig_mode_on_ss

DESCRIPTION
  Unforce the origination mode on the fly for a particular ss

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void cmph_force_orig_mode_on_ss(
  cm_ss_e_type ss,
        /* The ss on which the mode is to be unforced */

  cmph_s_type      *ph_ptr
        /* Pointer to a phone object */

)
{
  cm_orig_q_s_type *ph_orig_top_ptr =
             cmtask_orig_para_get_top( ss == CM_SS_MAX ? CM_SS_MAIN : ss );
    /* The pointer to the top element of the priority queue
    */
  sd_ss_pref_reas_e_type pref_reas = SD_SS_PREF_REAS_ORIG_END;

  sd_ss_orig_mode_e_type orig_mode = SD_SS_ORIG_MODE_NONE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* See if the hybrid preferences were toggled for the origination
  ** Or user changed the preferences and we held sending the pref_sys down
  ** set ss to MAX
  */

  /* If there is a call pending and we are forcing the phone preferences
  ** set the orig mode accordingly
  */
  if( ph_orig_top_ptr != NULL   &&
      ph_orig_top_ptr->orig.act_id == (cm_act_id_type)ph_ptr )
  {
    if( cmtask_orig_para_count_2( ss,
                                  CM_ACT_PRIORITY_END,
                                  CM_CMP_CRITERIA_GREATER_THAN) >= 1 )
    {
      if( ph_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL )
      {
        orig_mode = SD_SS_ORIG_MODE_MANUAL_ORIG;
      }
      else
      {
        orig_mode = SD_SS_ORIG_MODE_NORMAL;
      }
    }
    else
    {
      if( ph_orig_top_ptr->orig.orig_mode == SD_SS_ORIG_MODE_NONE &&
          ph_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL )
      {
        orig_mode = SD_SS_ORIG_MODE_MANUAL_IDLE;
      }
      else
      {
        orig_mode = ph_orig_top_ptr->orig.orig_mode;
      }
    }
  } /* If top == phone */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( (ss == CM_SS_MAIN) && (ph_orig_top_ptr != NULL) &&
       ( ( cmph_get_last_pref_sent_on_main()->orig_hybr_pref  !=
           ph_orig_top_ptr->orig.orig_hybr_pref ) ||
         ( ph_orig_top_ptr->orig.act_update_reas == CM_ACT_UPDATE_REAS_USER )

       )
    )
  {
    ss = CM_SS_MAX;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_MSG_HIGH("cmph_force_orig_mode_on_ss ss %d",ss,0,0);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Process any pending subscription commands
  */
  cmph_process_pending_subscription_cmds(ph_ptr,
                                         cmph_map_cm_mode_pref_to_sd_mode_pref(
                                           ph_ptr->mode_pref ));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If the top is none or there is only phone object in the queue or 
  ** all other activities have ended
  */
  if( ph_orig_top_ptr == NULL || cmtask_orig_para_count_2( 
                                 ( ss==CM_SS_MAX? CM_SS_MAIN:ss), 
                                   CM_ACT_PRIORITY_END, 
                                   CM_CMP_CRITERIA_GREATER_THAN) < 1 )
  {
    if( ph_orig_top_ptr != NULL )
    {
      if( ( ph_orig_top_ptr->orig.act_update_reas == CM_ACT_UPDATE_REAS_USER ||
            ph_orig_top_ptr->orig.act_update_reas ==
                                 CM_ACT_UPDATE_REAS_ACT_PWR_SAVE ) )

      {
        pref_reas = SD_SS_PREF_REAS_USER;
        ph_orig_top_ptr->orig.act_update_reas = CM_ACT_UPDATE_REAS_ACT_START;
      }
    } /* if( ph_orig_top_ptr != NULL ) */


    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* Get the handle to the phone oject and use it to force preferences,
    ** since all the activities have ended
    */

    ph_orig_top_ptr = cmtask_orig_para_search_act_id( 
                                       ( ss == CM_SS_MAX ? CM_SS_MAIN : ss ), 
                                       ( cm_act_id_type ) ph_ptr );

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    CM_ASSERT( ph_orig_top_ptr != NULL );

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  }
  else
  {
    /*lint -save -e774 Boolean within 'if' always evaluates to False */
    if( ph_orig_top_ptr != NULL )
    /*lint -restore */
    {
      if( ph_orig_top_ptr->orig.act_update_reas == 
                                              CM_ACT_UPDATE_REAS_ACT_PWR_SAVE )
      {
        pref_reas = SD_SS_PREF_REAS_USER;
        ph_orig_top_ptr->orig.act_update_reas = CM_ACT_UPDATE_REAS_ACT_START;
        orig_mode = SD_SS_ORIG_MODE_NONE;
      }
      else
      {
        pref_reas   = ( ( ph_orig_top_ptr->act_type == CM_ACT_TYPE_DATA_CALL ||
                          ph_orig_top_ptr->act_type == CM_ACT_TYPE_GPS ) ?
                          SD_SS_PREF_REAS_ORIG_START_PS :
                          SD_SS_PREF_REAS_ORIG_START_CS );
        orig_mode =  ph_orig_top_ptr->orig.orig_mode;
      }
    }
  } /* ph obj */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*lint -save -e774 Boolean within 'if' always evaluates to False */
  if( ph_orig_top_ptr != NULL )
  /*lint -restore */
  {
    cmpref_proc_cmd_pref_sys_chgd(
      cmph_map_cm_ss_type_to_sd_ss_type(ss),
      pref_reas,
      orig_mode,
      cmph_map_cm_mode_pref_to_sd_mode_pref( ph_orig_top_ptr->orig.orig_mode_pref ),
      cmph_map_cm_band_pref_to_sd_band_pref(ph_orig_top_ptr->orig.orig_band_pref),
      cmph_map_cm_prl_pref_to_sd_prl_pref(ph_orig_top_ptr->orig.orig_prl_pref),
      cmph_map_cm_roam_pref_to_sd_roam_pref(ph_orig_top_ptr->orig.orig_roam_pref),
      ph_orig_top_ptr->orig.orig_int_hybr_pref,
      cmph_map_otasp_act_code_to_sd_band(CM_OTASP_ACT_CODE_NONE),
      cmph_map_otasp_act_code_to_sd_blksys(CM_OTASP_ACT_CODE_NONE),
      SD_SS_AVOID_SYS_IDLE,
      0,
      ph_ptr->plmn,
      cmph_map_cm_srv_domain_pref_to_sd( ph_orig_top_ptr->orig.orig_srv_domain_pref ),
      cmph_map_cm_acq_order_pref_to_sd( ph_ptr->acq_order_pref  ),
      ph_orig_top_ptr->orig.orig_wlan_pref
    );
  }
  else
  {

    cmpref_proc_cmd_pref_sys_chgd(
      cmph_map_cm_ss_type_to_sd_ss_type(ss),
      pref_reas,
      orig_mode,
      cmph_map_cm_mode_pref_to_sd_mode_pref( ph_ptr->mode_pref ),
      cmph_map_cm_band_pref_to_sd_band_pref(ph_ptr->band_pref),
      cmph_map_cm_prl_pref_to_sd_prl_pref(ph_ptr->prl_pref),
      cmph_map_cm_roam_pref_to_sd_roam_pref(ph_ptr->roam_pref),
      ph_ptr->int_hybr_pref,
      cmph_map_otasp_act_code_to_sd_band(CM_OTASP_ACT_CODE_NONE),
      cmph_map_otasp_act_code_to_sd_blksys(CM_OTASP_ACT_CODE_NONE),
      SD_SS_AVOID_SYS_IDLE,
      0,
      ph_ptr->plmn,
      cmph_map_cm_srv_domain_pref_to_sd(ph_ptr->srv_domain_pref ),
      cmph_map_cm_acq_order_pref_to_sd(ph_ptr->acq_order_pref  ),
      &ph_ptr->wlan_pref
     );

  } /* ph_orig_top_ptr */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

}/* cmph_force_orig_mode_on_ss */


/*===========================================================================

FUNCTION cmph_force_orig_mode_on_the_fly

DESCRIPTION
  Unforce the origination mode on the fly

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void  cmph_force_orig_mode_on_the_fly(

    cm_ss_e_type      ss
        /* The ss on which the orig_mode is to be unforced */
)
{

   cmph_s_type      *ph_ptr        = cmph_ptr();
        /* Pointer to a phone object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_ptr != NULL );

  CM_FUNC_START( "cmph_force_orig_mode_on_the_fly()",0,0,0 );

  CM_MSG_HIGH ("cmph_force_orig_mode_on_the_fly() ss %d",ss,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Restore the phone preferences.
  */

  CM_MSG_HIGH("Restore mode=%d time=%ld",
              ph_ptr->mode_pref, clk_uptime(),0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (ss)
  {

    case CM_SS_MAIN:

      /* SS_MAIN instance was forced during origination, send the preferences to
      ** ss = SD_SS_MAIN
      */
     cmph_force_orig_mode_on_ss(CM_SS_MAIN, ph_ptr);
      break;

    case CM_SS_HDR:

      /* SS_HDR instance was forced during origination, send the preferences to
      ** ss = SD_SS_HYBR_HDR
      */
      cmph_force_orig_mode_on_ss(CM_SS_HDR, ph_ptr);
      break;



    case CM_SS_WLAN:

      /* SD_SS_HYBR_WLAN instance was forced during origination, send the
      ** preferences to ss = SD_SS_HYBR_WLAN
      */

      /* Need to keep it as it is, too much code duplication, if
      ** we modify it here
      */

      /* Wlan orig_mode needs to be passed here insetad of orig_mode,
      ** we already have the functionality in cmpref module, no need to
      ** duplicate the efforts here
      */

      cmph_force_orig_mode_on_ss(CM_SS_WLAN, ph_ptr);
      break;

    case CM_SS_MAX:
      cmph_force_orig_mode_on_ss(CM_SS_MAX,  ph_ptr);
      break;


    default:
      CM_CASE_ERR("ss %d", ss,0,0 );
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW("cmph_force_orig_mode_on_the_fly() on ss %d", ss, 0, 0);
      /* lint -restore */
      break;

  }/* Switch ss */

} /* cmph_force_orig_mode_on_the_fly() */



#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/**--------------------------------------------------------------------------
** Functions - internal, cdma
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmph_map_voice_answer_cm2db

DESCRIPTION
  Translates CM enum to DB enum for mode preference

DEPENDENCIES
  none

RETURN VALUE
  DB enum

SIDE EFFECTS
  none

===========================================================================*/
static db_voice_as_data_type cmph_map_voice_answer_cm2db
(
  cm_answer_voice_e_type cm_voice
)
{
  db_voice_as_data_type db_voice;

  if ( cm_voice == CM_ANSWER_VOICE_AS_VOICE )
  {
    db_voice = DB_VOICE_AS_DATA_NEVER;
  }
  else if ( cm_voice == CM_ANSWER_VOICE_AS_FAX_ONCE )
  {
    db_voice = DB_VOICE_AS_FAX_ONCE;
  }
  else if ( cm_voice == CM_ANSWER_VOICE_AS_FAX_ALWAYS )
  {
    db_voice = DB_VOICE_AS_FAX_ALWAYS;
  }
  else if ( cm_voice == CM_ANSWER_VOICE_AS_MODEM_ONCE )
  {
    db_voice = DB_VOICE_AS_MODEM_ONCE;
  }
  else if ( cm_voice == CM_ANSWER_VOICE_AS_MODEM_ALWAYS )
  {
    db_voice = DB_VOICE_AS_MODEM_ALWAYS;
  }
  else
  {
    db_voice = ( db_voice_as_data_type ) cm_voice;
  }

  return( db_voice );

} /* cmph_map_voice_answer_cm2db() */


#ifndef FEATURE_ACP

/*===========================================================================

FUNCTION REMOVE_AMPS_FROM_CM_MODE_PREF

DESCRIPTION
  This function remove the AMPS component from the CM mode preference
  setting

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  SD mode preference

SIDE EFFECTS
  none

===========================================================================*/
static cm_mode_pref_e_type remove_amps_from_cm_mode_pref(

  cm_mode_pref_e_type cm_mode_pref
   /* mode preference */
)
{

  switch( cm_mode_pref)
  {
    case CM_MODE_PREF_AMPS_ONLY:
    case CM_MODE_PREF_CDMA_AMPS_ONLY:

      return CM_MODE_PREF_CDMA_ONLY;

    default:
      return cm_mode_pref;
  }


} /* remove_amps_from_cm_mode_pref() */
#endif /* ifndef FEATURE_ACP */

#ifdef FEATURE_HDR_HYBRID
#error code not present
#endif /* FEATURE_HDR_HYBRID */


/*===========================================================================

FUNCTION cmph_ph_cmd_packet_state_para_check

DESCRIPTION
  Check parameter errors for packet state setting command

DEPENDENCIES
  None

RETURN VALUE
  CM_PH_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  None
===========================================================================*/
static cm_ph_cmd_err_e_type  cmph_ph_cmd_packet_state_para_check(
  cm_ph_cmd_s_type           *ph_cmd_ptr
)
{
  cm_ph_cmd_info_s_type          *cmd_info_ptr = NULL;
  cm_ph_cmd_err_e_type           cmd_err       = CM_PH_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  if( !BETWEEN(cmd_info_ptr->packet_state,
               CM_PACKET_STATE_NONE, CM_PACKET_STATE_MAX) )
  {
    CM_DBG_ERR_FATAL( "CM_CMD_ERR_PACKET_STATE_P",0,0,0 );
    return CM_PH_CMD_ERR_PACKET_STATE_P;
  }


  return cmd_err;
} /* cmph_ph_cmd_packet_state_para_check() */

#ifdef FEATURE_DDTM_CNTL

/*===========================================================================
FUNCTION cmph_misc_add_ddtm_act_mask

DESCRIPTION

  Add given DDTM act mask to given DDTM act mask.

  Example:

  The below will add all single mode pref components from
  CM_MODE_PREF_AUTOMATIC and end up with CM_MODE_PREF_NONE.

  ph_orig_ddtm_act = cmph_misc_add_ddtm_act_mask(
                                           SYS_DDTM_ACT_MASK_EMPTY,
                                           2,// because 2 components follow
                                           SYS_DDTM_ACT_MASK_SUPPRESS_MO_DBM,
                                           SYS_DDTM_ACT_MASK_SUPPRESS_REG,
                                           );

DEPENDENCIES
  sd_misc_add_mode_pref()

RETURN VALUE
  Return new preference or SYS_DDTM_ACT_MASK_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
static sys_ddtm_act_mask_e_type cmph_misc_add_ddtm_act_mask( sys_ddtm_act_mask_e_type ddtm_act_mask,
                                                      int number_of_components_to_add,
                                                        ... )
{

  va_list arg_list;
  sys_ddtm_act_mask_e_type temp_mask = ddtm_act_mask;
  sys_ddtm_act_mask_e_type mask_to_add = SYS_DDTM_ACT_MASK_EMPTY;

  /*
  ** Initialize arg_list and make an internal pointer point
  ** at the first variable function argument.
  ** This will be the second parameter
  */
  /*lint -e{662} -e{64} -e{718} -e{746} -e{530} -e{516} */
  va_start(arg_list, number_of_components_to_add);

  /*
  ** Process second and subsequent function arguments.
  ** These are the DDTM act mask that are to be added to the current ddtm
  ** act mask
  */

  while(number_of_components_to_add--)
  {

    /* Point at next function argument */
    /*lint -e{10} -e{662} -e{78} -e{718} -e{746} -e{530} -e{826} -e{662} */
    mask_to_add = (sys_ddtm_act_mask_e_type)va_arg(arg_list, int);

    if( mask_to_add >= SYS_DDTM_ACT_MASK_MAX)
    {
      CM_ERR( "Bad ddtm_mask to add = %d",mask_to_add, 0, 0 );
      CM_ASSERT(FALSE);
    }
    /*lint -e{655} bit-wise operation uses (compatible) enum's */
    temp_mask |= mask_to_add;
  }
   /* Clean up */
  va_end(arg_list);

  if( temp_mask >= SYS_DDTM_ACT_MASK_MAX )
  {
    CM_ERR( "Bad final ddtm_mask = %d",temp_mask, 0, 0 );
    return SYS_DDTM_ACT_MASK_MAX;
  }

  return temp_mask;

} /* cmph_misc_add_ddtm_act_mask */

#endif /* FEATURE_DDTM_CNTL */

#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */



/**--------------------------------------------------------------------------
** Functions - external, common
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmph_ptr

DESCRIPTION
  Return a pointer to the one and only phone object.

  The phone object is responsible for:
  1. Processing clients' phone commands.
  2. Processing MC replies for clients' phone commands.
  3. Processing MC phone related notifications.
  4. Notifying the client list of phone events.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmph_s_type  *cmph_ptr( void )
{

  static cmph_s_type cmph_local;
    /* The one and only phone object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return &cmph_local;

} /* cmph_ptr() */


/*===========================================================================
FUNCTION cmph_dem_ptr

DESCRIPTION
  Return a pointer to the dem object


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static cm_dem_s_type  *cmph_dem_ptr( void )
{

  static cm_dem_s_type cmdem_local;
  return &cmdem_local;

} /* cmph_dem_ptr() */


/*===========================================================================

FUNCTION cmph_dem_get_apps_pwr_status

DESCRIPTION
  Return the current apps power state

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

cm_apps_pwr_status_e_type cmph_dem_get_apps_pwr_status(void)
{
  return cmph_dem_ptr()->apps_pwr_status;
}

/*===========================================================================

FUNCTION cmdem_init

DESCRIPTION
  Initialize the dem object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmph_dem_init()
{

  cm_dem_s_type *dem_ptr = cmph_dem_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( dem_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dem_ptr->apps_pwr_status = CM_APPS_PWR_STATUS_PWR_RESTORE;

  return;


} /* cmdem_init() */

/*===========================================================================

FUNCTION cmph_init

DESCRIPTION
  Initializing the phone object.

  This function must be called before the phone object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_init( void )
{

  cmph_s_type    *ph_ptr  =  cmph_ptr();
      /* Point at phone object to be initialized */

  unsigned int    i;
      /* loop counter */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  db_items_value_type  db_item;
      /* Item to do db_get() and db_put() */
  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_ptr != NULL );
  CM_FUNC_START( "cmph_init()",0,0,0 );

  /* Verify object was not already initialized.
  ** and then mark object as initialized.
  */
  CM_ASSERT( ! CM_INIT_CHECK(ph_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initially the phone is powered off/FTM.
  **
  ** Note that UI needs to tell CM its decision during power-up -
  ** i.e. offline, online or LPM. If UI says online, MC can still
  ** come and say offline.
  */
  ph_ptr->oprt_mode      = SYS_OPRT_MODE_PWROFF;

  #ifndef FEATURE_PLT
  /* Put the phone in FTM if nv says so
  */
  cmnv_read_wait( NV_FTM_MODE_I, cmnv_item_ptr );
  if ( cmnv_item_ptr->ftm_mode )
  #else  /*FEATURE_PLT*/
#error code not present
  #endif /* !FEATURE_PLT */
  {
    ph_ptr->oprt_mode = SYS_OPRT_MODE_FTM;
    mmoc_cmd_oprt_mode_chgd( SYS_OPRT_MODE_FTM );
  } /* if ( cmnv_item_ptr->ftm_mode ) */
  else
  {
    ph_ptr->oprt_mode      = SYS_OPRT_MODE_PWROFF;
  }

  #ifdef FEATURE_CM_DETECT_HW
#error code not present
  #endif /* FEATURE_CM_DETECT_HW */

  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
  #endif /* FEATURE_WCDMA, FEATURE_GSM */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Undefine the plmn.
  */
  sys_plmn_undefine_plmn_id( &ph_ptr->plmn );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the initial report delta threshold to 5dBm
  */
  ph_ptr->rssi_delta     = CMPH_DEFAULT_RSSI_DELTA;

  /* Set the initial report ecio delta threshold to 2dBm
  */
  ph_ptr->ecio_delta     = CMPH_DEFAULT_ECIO_DELTA;

  /* Set the initial report io delta threshold to 5dBm
  */
  ph_ptr->io_delta       = CMPH_DEFAULT_IO_DELTA;

  /* Set the initial report sir delta threshold to 3db
  */
  ph_ptr->sir_delta      = CMPH_DEFAULT_SIR_DELTA;

  /* Set the initial report pathloss delta threshold to 5db
  */
  ph_ptr->pathloss_delta = CMPH_DEFAULT_PATHLOSS_DELTA;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initially phone is not in use.
  */
  ph_ptr->is_in_use      = FALSE;
  ph_ptr->is_in_use_reported      = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
  #endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initially phone is not CDMA locked until power-off
  */
  ph_ptr->cdma_lock_mode = CM_CDMA_LOCK_MODE_OFF;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initially we answer incoming voice calls as voice.
  ** Also update the answer-voice setting in the database
  ** to reflect this setting - MC consults it upon receiving
  ** a page.
  */
  ph_ptr->answer_voice     = CM_ANSWER_VOICE_AS_VOICE;
  db_item.voice_as_data    = DB_VOICE_AS_DATA_NEVER;
  db_put( DB_VOICE_AS_DATA, &db_item );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read from NV the current NAM selection.
  */
  cmnv_read_wait( NV_CURR_NAM_I, cmnv_item_ptr );
  ph_ptr->nam_sel = ph_ptr->curr_nam =(cm_nam_e_type)cmnv_item_ptr->curr_nam;

  /* If AUTO-NAM is selected, update the NAM SELECTION appropriately.
  */
  cmnv_read_wait( NV_AUTO_NAM_I, cmnv_item_ptr );
  if( cmnv_item_ptr->auto_nam )
  {
    ph_ptr->nam_sel = CM_NAM_AUTO;
  }
  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate curr_nam */
  if(!INRANGE(ph_ptr->curr_nam, 0, NV_MAX_NAMS))
  {
    CM_ERR( "curr_nam is invalid, curr_nam:%d",ph_ptr->curr_nam,0,0);
    ph_ptr->curr_nam = CM_NAM_1;
    ph_ptr->nam_sel  = ph_ptr->curr_nam;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read from NV the persistent mode preference
  ** for each NAM.
  */
  for( i=0; i < NV_MAX_NAMS; i++ )
  {
    cmnv_item_ptr->pref_mode.nam = (byte) i;
    cmnv_read_wait( NV_PREF_MODE_I, cmnv_item_ptr );
    ph_ptr->prst_mode_pref[i] = cmph_map_mode_pref_nv2cm( cmnv_item_ptr->pref_mode.mode );

    /* For targets that DO not support Analog Call Processing (ACP),
    ** always default the mode to DIGITAL ONLY.
    */
    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

    /* This is for 1x only or multimode
    */
    #ifndef FEATURE_ACP
    ph_ptr->prst_mode_pref[i] = remove_amps_from_cm_mode_pref(ph_ptr->prst_mode_pref[i]);
    #endif /* FEATURE_ACP */

    #else /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

    /* This is for GW only, not multimode
    */
    #if defined(FEATURE_WCDMA) && !defined(FEATURE_GSM)
#error code not present
    #endif /* FEATURE_WCDMA && !FEATURE_GSM */

    #if defined(FEATURE_GSM) && !defined(FEATURE_WCDMA)
#error code not present
    #endif /* FEATURE_GSM, FEATURE_WCDMA */

    #if defined(FEATURE_GSM) && defined(FEATURE_WCDMA)
#error code not present
    #endif /* FEATURE_GSM, FEATURE_WCDMA */
    #endif /* else of (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
  }

  /* Copy the persistent mode preference of the current NAM
  ** to our mode preference variable.
  */
  ph_ptr->mode_pref = CMPH_PRST_MODE_PREF( ph_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if ( (defined(FEATURE_HDR_HYBRID) || defined(FEATURE_HYBR_GW)) && !defined(FEATURE_HDR_HYBRID_DISABLE))
#error code not present
  #else /* FEATURE_HDR_HYBRID */
  ph_ptr->hybr_pref      =
  ph_ptr->prst_hybr_pref = CM_HYBR_PREF_OFF;
  ph_ptr->int_hybr_pref = SD_SS_HYBR_PREF_NONE;
  #endif /* ! FEATURE_HDR_HYBRID */


  #ifdef FEATURE_FULL_TIME_SHDR
#error code not present
  #endif

  /* Read from NV the persistent band preference
  ** for each NAM.
  */
  for( i=0; i < NV_MAX_NAMS; i++ )
  {
    cm_nv_band_pref_u_type  temp;
    uint64                  band_pref_32_63;
    uint64                  temp1;

    cmnv_item_ptr->band_pref.nam = (byte) i;
    cmnv_read_wait( NV_BAND_PREF_I, cmnv_item_ptr );
    temp.nv_band_pref[0] = (uint16)cmnv_item_ptr->band_pref.band;

    cmnv_read_wait( NV_BAND_PREF_16_31_I, cmnv_item_ptr );
    temp.nv_band_pref[1] = (uint16)cmnv_item_ptr->band_pref.band;

    cmnv_read_wait( NV_BAND_PREF_32_63_I, cmnv_item_ptr );
    band_pref_32_63 = (uint32)cmnv_item_ptr->band_pref_32_63.band;

    /* Copy Bits 0-31 in temporary variable.
    */
    temp1 = temp.dummy;

    /* Move bits 32-63 to its desired position and insert bits 0-31.
    */
    ph_ptr->prst_band_pref[i] =  temp1 | (uint64)(band_pref_32_63 << 32);

    /* If the band pref is not a valid value, write a valid value into NV (ANY) */
    if (ph_ptr->prst_band_pref[i] >= CM_BAND_PREF_MAX)
    {
       ph_ptr->prst_band_pref[i] = CM_BAND_PREF_ANY;
       cmnv_item_ptr->band_pref.nam  = (byte) i;
       cmnv_item_ptr->band_pref.band = (nv_band_pref_enum_type)CM_BAND_PREF_ANY;
                                      //NV_BAND_PREF_ANY;
       cmnv_write_wait (NV_BAND_PREF_I, cmnv_item_ptr);

       cmnv_item_ptr->band_pref.band = (nv_band_pref_enum_type)(CM_BAND_PREF_ANY >> 16);
                                      //((word)NV_BAND_PREF_ANY & 0x7FFF);
       cmnv_write_wait (NV_BAND_PREF_16_31_I, cmnv_item_ptr);

       cmnv_item_ptr->band_pref_32_63.band = (uint32)(CM_BAND_PREF_ANY >> 32);
                                      //((word)NV_BAND_PREF_ANY & 0x3FFF);
       cmnv_write_wait (NV_BAND_PREF_32_63_I, cmnv_item_ptr);
    }

    /* If target or PRL oes not support the specified mode and band preference
    ** combination, set the mode and band preference to their default
    ** values.
    */
    if( ! sd_misc_is_supp_pref(
          cmph_map_cm_mode_pref_to_sd_mode_pref(ph_ptr->prst_mode_pref[i]),
             cmph_map_cm_band_pref_to_sd_band_pref(ph_ptr->prst_band_pref[i] )) )
    {
      /* Default the mode preference and write it back to NV.
      */
      ph_ptr->prst_mode_pref[i]     = CM_MODE_PREF_AUTOMATIC;
      cmnv_item_ptr->pref_mode.nam  = (byte) i;
      cmnv_item_ptr->pref_mode.mode = NV_MODE_AUTOMATIC;
      cmnv_write_wait(NV_PREF_MODE_I, cmnv_item_ptr);

      /* Default the band preference and write it back to NV.
      */
      ph_ptr->prst_band_pref[i]     = CM_BAND_PREF_ANY;
      cmnv_item_ptr->band_pref.nam  = (byte) i;
      cmnv_item_ptr->band_pref.band = (nv_band_pref_enum_type)CM_BAND_PREF_ANY;
      cmnv_write_wait(NV_BAND_PREF_I, cmnv_item_ptr);
      cmnv_item_ptr->band_pref.band = (nv_band_pref_enum_type)(CM_BAND_PREF_ANY >> 16 );
      cmnv_write_wait(NV_BAND_PREF_16_31_I, cmnv_item_ptr);
      cmnv_item_ptr->band_pref_32_63.band = (uint32)(CM_BAND_PREF_ANY >> 32 );
      cmnv_write_wait (NV_BAND_PREF_32_63_I, cmnv_item_ptr);
    }
  }

  /* Copy the persistent mode and band preferences of the current NAM
  ** to our mode and band preference variables.
  */
  ph_ptr->mode_pref = CMPH_PRST_MODE_PREF( ph_ptr );
  ph_ptr->band_pref = CMPH_PRST_BAND_PREF( ph_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read from NV the persistent prl preference
  ** for each NAM.
  */
  for( i=0; i < NV_MAX_NAMS; i++ )
  {
    cmnv_item_ptr->prl_pref.nam = (byte) i;
    cmnv_read_wait( NV_PRL_PREF_I, cmnv_item_ptr );
    ph_ptr->prst_prl_pref[i] =
     (cm_prl_pref_e_type) (cmnv_item_ptr->prl_pref.prl);

    /* If the prl preference is out of bound, set the prl preference to its
    ** deflate value.
    */
    if( ! BETWEEN( ph_ptr->prst_prl_pref[i], CM_PRL_PREF_NONE,
                   CM_PRL_PREF_NO_CHANGE
                 )
                 )
    {
      ph_ptr->prst_prl_pref[i]     = CM_PRL_PREF_ANY;
      cmnv_item_ptr->prl_pref.nam  = (byte) i;
      cmnv_item_ptr->prl_pref.prl =  (uint32) CM_PRL_PREF_ANY;

      cmnv_write_wait(NV_PRL_PREF_I, cmnv_item_ptr);
    }
  }

  /* Copy the persistent prl preference of the current NAM
  ** to our prl preference variable.
  */
  ph_ptr->prl_pref = CMPH_PRST_PRL_PREF( ph_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read from NV the persistent roam preference
  ** for each NAM.
  */
  for( i=0; i < NV_MAX_NAMS; i++ )
  {
    cmnv_item_ptr->roam_pref.nam = (byte) i;
    cmnv_read_wait( NV_ROAM_PREF_I, cmnv_item_ptr );
    ph_ptr->prst_roam_pref[i] =
     (cm_roam_pref_e_type) (cmnv_item_ptr->roam_pref.roam);

    /* If the roam preference is out of bound, set the roam preference to its
    ** deflate value.
    */
    if( ! BETWEEN(ph_ptr->prst_roam_pref[i], CM_ROAM_PREF_NONE,
      CM_ROAM_PREF_NO_CHANGE) )
    {
      ph_ptr->prst_roam_pref[i]     = CM_ROAM_PREF_ANY;
      cmnv_item_ptr->roam_pref.nam  = (byte) i;
      cmnv_item_ptr->roam_pref.roam = (nv_roam_pref_enum_type)
                                      SD_SS_ROAM_PREF_ANY;

      cmnv_write_wait(NV_ROAM_PREF_I, cmnv_item_ptr);
    }
  }

  /* Copy the persistent roam preference of the current NAM
  ** to our roam preference variable.
  */
  ph_ptr->roam_pref = CMPH_PRST_ROAM_PREF( ph_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize is_spc_zero.
  */
  ph_ptr->is_spc_zero  = FALSE;

  /*
  ** Initialize RTRE settings.
  ** Defaulting them to NV only is intentional.
  ** Once power up is complete, UI will determine what RTRE setting needs to
  ** be and set it via a call to cm_ph_cmd_change_rtre_config()
  */
  ph_ptr->rtre_config  = CM_RTRE_CONFIG_NV_ONLY;
  ph_ptr->rtre_control = CM_RTRE_CONTROL_NV;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initially packet state is NULL.
  */

  ph_ptr->packet_state = CM_PACKET_STATE_NULL;


  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read from NV the silent redial with AMPS setting.
  */
  cmnv_read_wait( NV_SRDA_ENABLED_I, cmnv_item_ptr );
  ph_ptr->is_sr_with_amps = BOOLEAN( cmnv_item_ptr->srda_enabled );
  #endif /* FEATURE_CDMA_800, FEATURE_CDMA_1900 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read mob_cai_rev (p_rev) from NV
  */
  cmnv_read_wait( NV_MOB_CAI_REV_I, cmnv_item_ptr );
  ph_ptr->mob_cai_rev = cmnv_item_ptr->mob_cai_rev;

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read from NV the persistent acquisition order preference
  ** for each NAM.
  */
  for( i=0; i < NV_MAX_NAMS; i++ )
  {
    #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
    #else /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */
    ph_ptr->prst_acq_order_pref[i] = CM_GW_ACQ_ORDER_PREF_AUTOMATIC;
    #endif /* FEATURE_WCDMA || FEATURE_GSM */
  } /* for */

  /* Copy the persistent acquisition order preference of the current NAM
  ** to our acquisition order preference variable.
  */
  ph_ptr->acq_order_pref = CMPH_PRST_ACQ_ORDER_PREF( ph_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read from NV the persistent network selection mode preference
  ** for each NAM.
  */
  for( i=0; i < NV_MAX_NAMS; i++ )
  {
    #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
    #else
    ph_ptr->prst_network_sel_mode_pref[i]         = CM_NETWORK_SEL_MODE_PREF_AUTOMATIC;
    #endif
  } /* for */


  /* Copy the persistent network selection mode preference of the current NAM
  ** to our network selection mode preference variable.
  */
  ph_ptr->network_sel_mode_pref = CMPH_PRST_NETWORK_SEL_MODE_PREF( ph_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read from NV the persistent service domain preference for each NAM.
  */
  for( i=0; i < NV_MAX_NAMS; i++ )
  {

    #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
    #else
    ph_ptr->prst_srv_domain_pref[i] = CM_SRV_DOMAIN_PREF_CS_ONLY;
    #endif /* FEATURE_WCDMA || FEATURE_GSM */
  } /* for */

  /* Copy the persistent service domain preference of the current NAM
  ** to our service domain preference variable.
  */
  ph_ptr->srv_domain_pref = CMPH_PRST_SRV_DOMAIN_PREF( ph_ptr );

  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
  #endif  /* FEATURE_WCDMA, FEATURE_GSM */

  ph_ptr->cur_ddtm_status   = FALSE;


  #if !defined(FEATURE_DDTM_ACCPT_ASYNC_FAX_SO_PAGES) && \
      (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) \
      && defined(FEATURE_DDTM_CNTL)

  cmnv_read_wait( NV_DDTM_SETTINGS_I, cmnv_item_ptr );

  /* If NV read failed, use default values and write the new values to NV
  */

  if( cmnv_item_ptr->ddtm_settings.num_srv_opt == CM_INVALID_DDTM_NUM_SRV_OPT )
  {
    ph_ptr->ddtm_act_mask     = SYS_DDTM_ACT_MASK_IGN_SO_PAGES;
    ph_ptr->ddtm_num_srv_opt  = 5;
    ph_ptr->ddtm_pref         = CM_DDTM_PREF_ON;
    ph_ptr->ddtm_srv_opt_list[0] = (sys_srv_opt_type)CAI_SO_ASYNC_DATA_PRE707;
    ph_ptr->ddtm_srv_opt_list[1] = (sys_srv_opt_type)CAI_SO_G3_FAX_PRE707;
    ph_ptr->ddtm_srv_opt_list[2] = (sys_srv_opt_type)CAI_SO_ASYNC_DATA_13K_PRE707;
    ph_ptr->ddtm_srv_opt_list[3] = (sys_srv_opt_type)CAI_SO_G3_FAX_13K_PRE707;
    ph_ptr->ddtm_srv_opt_list[4] = (sys_srv_opt_type)CAI_SO_PPP_PKT_DATA_3G;

    /* Initialize the list of service option pages to none
    */

    /* Cast is to prevent "implicit cast of pointer loses '__packed' qualifier"
    ** compiler error  C2906E
    */
    memset( (void*)cmnv_item_ptr->ddtm_settings.srv_opt_list, 0,
            sizeof(cmnv_item_ptr->ddtm_settings.srv_opt_list) );

    cmnv_item_ptr->ddtm_settings.ddtm_act             = DDTM_ACT_IGN_SO_PAGES;
    cmnv_item_ptr->ddtm_settings.ddtm_pref            = DDTM_PREF_ON;
    cmnv_item_ptr->ddtm_settings.num_srv_opt          = 5;
    cmnv_item_ptr->ddtm_settings.srv_opt_list[0] = (sys_srv_opt_type)CAI_SO_ASYNC_DATA_PRE707;
    cmnv_item_ptr->ddtm_settings.srv_opt_list[1] = (sys_srv_opt_type)CAI_SO_G3_FAX_PRE707;
    cmnv_item_ptr->ddtm_settings.srv_opt_list[2] = (sys_srv_opt_type)CAI_SO_ASYNC_DATA_13K_PRE707;
    cmnv_item_ptr->ddtm_settings.srv_opt_list[3] = (sys_srv_opt_type)CAI_SO_G3_FAX_13K_PRE707;
    cmnv_item_ptr->ddtm_settings.srv_opt_list[4] = (sys_srv_opt_type)CAI_SO_PPP_PKT_DATA_3G;

    cmnv_write_wait (NV_DDTM_SETTINGS_I, cmnv_item_ptr);
  }
  else
  {

    ph_ptr->ddtm_act_mask      = cmph_map_nv_ddtm_act_to_cm_ddtm_act_mask(
                                  cmnv_item_ptr->ddtm_settings.ddtm_act );
    ph_ptr->ddtm_pref          = cmph_map_nv_ddtm_pref_to_cm_ddtm_pref(
                                  cmnv_item_ptr->ddtm_settings.ddtm_pref );

    if( cmnv_item_ptr->ddtm_settings.num_srv_opt > SYS_DDTM_MAX_SO_LIST_SIZE )
    {
      ph_ptr->ddtm_num_srv_opt = SYS_DDTM_MAX_SO_LIST_SIZE;
    }
    else
    {
      ph_ptr->ddtm_num_srv_opt = cmnv_item_ptr->ddtm_settings.num_srv_opt;
    }

    for( i=0; i<ph_ptr->ddtm_num_srv_opt; i++ )
    {
      ph_ptr->ddtm_srv_opt_list[i] =
              (sys_srv_opt_type)cmnv_item_ptr->ddtm_settings.srv_opt_list[i];

    }

  }
  #else
  ph_ptr->ddtm_act_mask     = SYS_DDTM_ACT_MASK_IGN_SO_PAGES;
  ph_ptr->ddtm_num_srv_opt  = 0;
  ph_ptr->ddtm_pref         = CM_DDTM_PREF_OFF;
  #endif

  ph_ptr->cur_ddtm_status   = FALSE;
  ph_ptr->cur_ddtm_act_mask = SYS_DDTM_ACT_MASK_EMPTY;
  ph_ptr->cur_ddtm_num_srv_opt = 0;

  /* Read preferences for packet switched data calls.
  */
  cmnv_read_wait( NV_PS_DATA_ORIG_PREF_I, cmnv_item_ptr );
  ph_ptr->ps_data_orig_pref = (cm_ps_data_orig_prefs_e_type)
                                  cmnv_item_ptr->ps_data_orig_pref;

  if( ! BETWEEN(ph_ptr->ps_data_orig_pref, CM_PS_DATA_ORIG_PREF_NONE,
      CM_PS_DATA_ORIG_PREF_MAX) )
  {
    /*
    ** Default the ps data orig preference and write it back to NV.
    */
    ph_ptr->ps_data_orig_pref = CM_PS_DATA_ORIG_PREF_ANY;
    cmnv_item_ptr->ps_data_orig_pref = NV_PS_DATA_ORIG_PREF_ANY;
    cmnv_write_wait (NV_PS_DATA_ORIG_PREF_I, cmnv_item_ptr);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Read disabled call type
  */

  cmnv_read_wait( NV_DISABLE_CM_CALL_TYPE_I, cmnv_item_ptr );

  ph_ptr->disable_call_type_mask = (cm_disable_call_type)
                                  cmnv_item_ptr->disable_cm_call_type;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* set the PS service request timer to inactive */
  cmph_deactivate_ps_service_req_timer();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
      defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM)
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize command reply object.
  */

  cm_reply_init( &ph_ptr->reply );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Set the PS Signaling wait time to none and PS signaling down needed to
  ** FALSE.
  */

  cmph_set_ps_sig_down_wait_time(CMPH_GW_PKT_WAIT_TIME_NONE);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read from NV the persistent WLAN tech preferences
  ** for each NAM.
  */
  for( i=0; i < NV_MAX_NAMS; i++ )
  {
    #ifdef FEATURE_WLAN
#error code not present
    #else
    ph_ptr->prst_wlan_pref[i].tech_pref     = CM_WLAN_TECH_PREF_ANY;
    #endif
  }

  /* Read from NV the persistent WLAN BSS type preferences
  ** for each NAM.
  */
  for( i=0; i < NV_MAX_NAMS; i++ )
  {
    #ifdef FEATURE_WLAN
#error code not present
    #else
    ph_ptr->prst_wlan_pref[i].bss_type_pref     = CM_WLAN_BSS_TYPE_PREF_ANY;
    #endif
  }

  /* Read from NV the persistent WLAN scan preferences
  ** for each NAM.
  */
  for( i=0; i < NV_MAX_NAMS; i++ )
  {
    #ifdef FEATURE_WLAN
#error code not present
    #else
    ph_ptr->prst_wlan_pref[i].scan_mode     = CM_WLAN_SCAN_PREF_AUTO;
    #endif
  }

  /* Read from NV the persistent network selection mode preference for WLAN
  ** for each NAM.
  */
  for( i=0; i < NV_MAX_NAMS; i++ )
  {
    #ifdef FEATURE_WLAN
#error code not present
    #else
    ph_ptr->prst_wlan_pref[i].network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_AUTOMATIC;
    #endif
  } /* for */

  /* Copy the persistent WLAN preferences of the current NAM
  ** to our WLAN preference variable.
  */
  ph_ptr->wlan_pref = CMPH_PRST_WLAN_PREF( ph_ptr );

  if(ph_ptr->wlan_pref.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
  {
    cmph_read_manual_wlan_sys_profile(&ph_ptr->wlan_pref,
                                      &ph_ptr->wlan_pref.profile);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the DEM object
  */
  cmph_dem_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_CSNA
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if ( defined (FEATURE_UMTS_1X_HANDOVER) || \
        defined(FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X) || \
        defined(FEATURE_IMS_VCC) )
  /* Initialize homer
  */
  homer_init();
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Mark object as initialized.
  */
  CM_INIT_MARK( ph_ptr->init_mask );

  ph_ptr->is_gw_subscription_available   = FALSE;
  ph_ptr->is_cdma_subscription_available = FALSE;
  ph_ptr->is_wlan_subscription_available = FALSE;

  if ( ph_ptr->oprt_mode == SYS_OPRT_MODE_PWROFF ||
       ph_ptr->oprt_mode == SYS_OPRT_MODE_ONLINE
     )
  {
    /* Inform CM Pref & MMOC that subscription changed.
    ** ADD_PROTO with both CDMA and GW makes both protocol stacks reload
    ** NV information
    */
    cmpref_proc_cmd_subsc_chgd(
          MMOC_SUBSC_CHG_NAM_SEL,
           (byte) ph_ptr->curr_nam,
           cmph_map_cm_network_sel_mode_pref_to_orig_mode(
             CMPH_PRST_NETWORK_SEL_MODE_PREF( ph_ptr )),
           cmph_map_cm_mode_pref_to_sd_mode_pref(
             ph_ptr->prst_mode_pref[ph_ptr->curr_nam]),
           cmph_map_cm_band_pref_to_sd_band_pref(ph_ptr->prst_band_pref[ph_ptr->curr_nam]),
           cmph_map_cm_prl_pref_to_sd_prl_pref(ph_ptr->prst_prl_pref[ph_ptr->curr_nam]),
           cmph_map_cm_roam_pref_to_sd_roam_pref(ph_ptr->prst_roam_pref[ph_ptr->curr_nam]),
           cmph_map_cm_hybr_pref_to_sd_hybr_pref(ph_ptr->prst_hybr_pref),
           ph_ptr->plmn,
           cmph_map_cm_srv_domain_pref_to_sd(
             CMPH_PRST_SRV_DOMAIN_PREF( ph_ptr)),
           cmph_map_cm_acq_order_pref_to_sd(
             CMPH_PRST_ACQ_ORDER_PREF( ph_ptr )  ),
           ph_ptr->is_gw_subscription_available,
           ph_ptr->is_cdma_subscription_available,
           ph_ptr->is_wlan_subscription_available,
           &ph_ptr->wlan_pref,
           PROT_SUBSC_CHG_ALL
           );
  } /* if ( ph_ptr->oprt_mode != SYS_OPRT_MODE_FTM ) */

  cmph_als_reset();

  ph_ptr->is_ps_srv_req_sent = FALSE;

  /* Initialize the network type request to none for
  ** use in cm_ph_cmd_get_networks() API.
  */
  ph_ptr->network_type  = CM_MODE_PREF_NONE;

  ph_ptr->wlan_bss_list.num_bss = 0;


  cmph_set_net_lists_present_flag(FALSE);

  ph_ptr->is_data_switch_over_call_success = FALSE;

  ph_ptr->ph_state = CM_PH_STATE_NORMAL;

  /* Initialize the privacy setting to standard
  */

  ph_ptr->privacy_pref = CM_PRIVACY_PREF_STANDARD;

  /* Initialize the is_privacy to false
  */
  ph_ptr->is_privacy      = FALSE;

  /* CDMA is not Tx disabled during init.
  */
  ph_ptr->is_cdma_tx_disabled = FALSE;

  /* Add in the orig Q the ph_ptr values
  */

  (void)cmph_insert_orig_mode (
                                CM_SS_MAIN,
                                SD_SS_ORIG_MODE_NONE,
                                CM_ACT_TYPE_PH_OBJ,
                                ph_ptr->pref_term,
                                ph_ptr->mode_pref,
                                ph_ptr->band_pref,
                                ph_ptr->prl_pref,
                                ph_ptr->roam_pref,
                                ph_ptr->hybr_pref,
                                ph_ptr->int_hybr_pref,
                                &ph_ptr->wlan_pref,
                                ph_ptr->srv_domain_pref,
                                (cm_act_id_type)ph_ptr,
                                CM_ACT_UPDATE_REAS_ACT_START
                              );
  /* Add in the orig Q the ph_ptr values
  */

  (void)cmph_insert_orig_mode (
                                CM_SS_HDR,
                                SD_SS_ORIG_MODE_NONE,
                                CM_ACT_TYPE_PH_OBJ,
                                ph_ptr->pref_term,
                                ph_ptr->mode_pref,
                                ph_ptr->band_pref,
                                ph_ptr->prl_pref,
                                ph_ptr->roam_pref,
                                ph_ptr->hybr_pref,
                                ph_ptr->int_hybr_pref,
                                &ph_ptr->wlan_pref,
                                ph_ptr->srv_domain_pref,
                                (cm_act_id_type)ph_ptr,
                                CM_ACT_UPDATE_REAS_ACT_START
                              );

  /* Add in the orig Q the ph_ptr values
  */

  (void)cmph_insert_orig_mode (
                                CM_SS_WLAN,
                                SD_SS_ORIG_MODE_NONE,
                                CM_ACT_TYPE_PH_OBJ,
                                ph_ptr->pref_term,
                                ph_ptr->mode_pref,
                                ph_ptr->band_pref,
                                ph_ptr->prl_pref,
                                ph_ptr->roam_pref,
                                ph_ptr->hybr_pref,
                                ph_ptr->int_hybr_pref,
                                &ph_ptr->wlan_pref,
                                ph_ptr->srv_domain_pref,
                                (cm_act_id_type)ph_ptr,
                                CM_ACT_UPDATE_REAS_ACT_START
                              );

  /* Initialize the energency call back ptr to default values
  */

  cmtask_orig_para_init_2( cmph_get_emergency_cb_pref() );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* status of plmn list request (success/abort) */
  ph_ptr->available_networks_list_cnf = SYS_PLMN_LIST_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the hdr kicked ptr
  */
  *cmph_get_is_kicked_hybr_ptr() = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmph_set_nam_chg_pend (FALSE);

  /*initialize the subscription buffers
  */
  ph_ptr->is_pending_subsc_cdma = FALSE;
  ph_ptr->is_pending_subsc_gw = FALSE;
  ph_ptr->is_pending_subsc_wlan = FALSE;
  ph_ptr->cdma_subsc_cmd = CM_PH_CMD_NONE;
  ph_ptr->gw_subsc_cmd = CM_PH_CMD_NONE;
  ph_ptr->wlan_subsc_cmd = CM_PH_CMD_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*initialize the mode capability and band capability
  */
  ph_ptr->mode_capability = SYS_SYS_MODE_MASK_NONE;
  ph_ptr->band_capability = SYS_BAND_MASK_EMPTY;

  cmph_get_mode_band_capability(&(ph_ptr->mode_capability), &(ph_ptr->band_capability));

  /* Initialize measurement mode to OFF */
  #ifdef FEATURE_CM_SS_MEAS
#error code not present
  #endif 

} /* cmph_init() */



/*===========================================================================

FUNCTION cmph_is_pref_term_temporary

DESCRIPTION
  Check whether a specified pref_term is temporary for call purposes.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the specified pref_term is temporary.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmph_is_pref_term_temporary(

       cm_pref_term_e_type       pref_term
        /* pref_term to check for temporary */

)
{
  boolean is_pref_term_temp = FALSE;


  switch ( pref_term )
  {
    case CM_PREF_TERM_1_CALL:
    case CM_PREF_TERM_1_CALL_OR_TIME:
    case CM_PREF_TERM_CM_1_CALL:
    case CM_PREF_TERM_CM_1_CALL_PLUS:
    case CM_PREF_TERM_CM_MO_SMS:
      is_pref_term_temp = TRUE;
      break;

    #ifdef FEATURE_RPC
#error code not present
    #endif
    case CM_PREF_TERM_NONE:
    case CM_PREF_TERM_PERMANENT:
    case CM_PREF_TERM_PWR_CYCLE:
    case CM_PREF_TERM_MAX:
    default:
      break;
  } /* switch ( ph_ptr->orig_pref_term ) */


  return is_pref_term_temp;

} /* cmph_is_pref_term_temporary() */

/*===========================================================================

FUNCTION cmph_is_valid_mode_band_pref

DESCRIPTION
  Check whether a specified combination of mode and band preference is
  supported by the current target and the PRL that is associated with the
  currently selected NAM.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmph_is_valid_mode_band_pref(

    cm_mode_pref_e_type    mode_pref,
        /* Mode pref to check for validity */

    cm_band_pref_e_type    band_pref
        /* Band pref to check for validity */
)
{
  sd_ss_mode_pref_e_type  sd_mode_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* No need to validate CM_MODE_PREF_AUTOMATIC.
  */
  if( mode_pref == CM_MODE_PREF_AUTOMATIC &&
      band_pref == CM_BAND_PREF_ANY )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return a boolean indicating whether specified combination of mode and
  ** band preference is supported.
  */
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref( mode_pref );
  CM_ASSERT( sd_mode_pref != SD_SS_MODE_PREF_MAX );

  return sd_misc_is_supp_pref( sd_mode_pref, cmph_map_cm_band_pref_to_sd_band_pref(band_pref) );

} /* cmph_is_valid_mode_band_pref() */



/**===========================================================================

FUNCTION cmph_get_mode_band_capability

DESCRIPTION
  Request for all the available modes and bands supported by the current 
  target.

DEPENDENCIES
  None

RETURNS
  

SIDE_EFFECT
  None

===========================================================================*/
void cmph_get_mode_band_capability(

  sys_sys_mode_mask_e_type *    mode_capability,
    /* Mode capability supported by the current target */

  sys_band_mask_e_type *        band_capability
    /* Band capability supported by the current target */

)
{
  sd_ss_mode_pref_e_type sd_mode_pref = SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type sd_band_pref = SD_SS_BAND_PREF_NONE;

  //sd_misc_get_mode_band_capability(&sd_mode_pref, (sd_ss_band_pref_e_type *)band_pref);
  sd_misc_get_mode_band_capability(&sd_mode_pref, &sd_band_pref);

  *band_capability = (sys_band_mask_e_type)sd_band_pref;

  *mode_capability = cmph_map_sd_mode_pref_to_sys_mode_mask(sd_mode_pref);

}




/*===========================================================================

FUNCTION cmph_is_valid_network_selection_mode_pref

DESCRIPTION
  Check whether a specified combination of network selection mode and mode
  preference is supported by the current target.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the specified combination is supported.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmph_is_valid_network_selection_mode_pref(

    cm_mode_pref_e_type                          mode_pref,
        /* Mode pref to check for validity */

    cm_network_sel_mode_pref_e_type              gw_network_sel_mode_pref,
        /* Network selection mode pref to check for validity */

    cm_network_sel_mode_pref_e_type              wlan_network_sel_mode_pref
        /* WLAN Network selection mode pref to check for validity */
)
{

  /* Check if the mode preference is valid
  */
  if ( mode_pref == CM_MODE_PREF_NONE )
  {
    CM_ERR("mode pref. not valid", 0, 0, 0);
    return FALSE;
  }

  /*
  ** Manual network selection mode preference is only allowed with a mode
  ** mode pref containing GW/WLAN.
  */
  if ( gw_network_sel_mode_pref != CM_NETWORK_SEL_MODE_PREF_MANUAL &&
       wlan_network_sel_mode_pref != CM_NETWORK_SEL_MODE_PREF_MANUAL)
  {
    return TRUE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* When the GW network selection preference is Manual, check if the current 
  ** mode preference and target supports GW.
  */
  if( gw_network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL )
  {
    /*
    ** We got here so Manual is requested.
    ** Check if the target even supports GW.
    */
    if ((!sd_misc_is_target_supp_mode_band (SD_SS_MODE_PREF_GSM, 
                                            SD_SS_BAND_PREF_ANY))    
        &&
        (!sd_misc_is_target_supp_mode_band (SD_SS_MODE_PREF_WCDMA, 
                                            SD_SS_BAND_PREF_ANY)))

    {
      return FALSE;
    }

    /*
    ** We got here so Manual is requested and G/W is supported by the target.
    */
    if (sd_misc_is_mode_pref(cmph_map_cm_mode_pref_to_sd_mode_pref(mode_pref), 
                             SD_SS_MODE_PREF_GSM) 
        ||
        sd_misc_is_mode_pref(cmph_map_cm_mode_pref_to_sd_mode_pref(mode_pref), 
                             SD_SS_MODE_PREF_WCDMA))

    {
      return TRUE;
    }
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* When the WLAN network selection preference is Manual, check if the current 
  ** mode preference and target supports WLAN.
  */
  if(wlan_network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
  {
    /*
    ** We got here so Manual is requested.
    ** Check if the target even supports WLAN.
    */
    if ((!sd_misc_is_target_supp_mode_band (SD_SS_MODE_PREF_WLAN, 
                                            SD_SS_BAND_PREF_ANY)))

    {
      return FALSE;
    }

    /*
    ** We got here so Manual is requested and WLAN is supported by the target.
    */
    if (sd_misc_is_mode_pref(cmph_map_cm_mode_pref_to_sd_mode_pref(mode_pref), 
                             SD_SS_MODE_PREF_WLAN))

    {
      return TRUE;
    }
  }

  return FALSE;


} /* cmph_is_valid_network_selection_mode_pref */


/*===========================================================================

FUNCTION cmph_is_valid_mode_hybrid_pref

DESCRIPTION
  Check whether a specified combination of mode and hybrid preference is
  valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the specified combination of mode and hybrid preference is valid.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmph_is_valid_mode_hybrid_pref(

    cm_mode_pref_e_type    mode_pref,
        /* Mode pref to check for validity */

    cm_hybr_pref_e_type    hybrid_pref
        /* Hybrid pref to check for validity */
)
/*lint -esym(715,mode_pref) */
/*lint -esym(715,hybrid_pref) */
{

  // Comments as per check in Sept 5th, 2003
  // We can only do hybrid in SD_SS_MODE_PREF_DIGITAL or
  // SD_SS_MODE_PREF_ANY
  // However, CM PREF object is able to handle system selection
  // preference commands no matter what the combination of
  // mode_pref / hybrid_pref.
  // If hybrid_pref is ON but mode pref does not allow hybrid operation,
  // CM PREF will effectively move into non-hybrid operation without
  // complaining about it.
  // In addition, we have legacy issues from 5500.
  // There it was legal to set mode pref to CDMA only and still have
  // hybrid preference set to ON.
  // Therefore it was decided to simply make this function return TRUE.
  // We left the code commented out but in so it's here in case we need
  // to change it later.

  return TRUE;

//
//   cmph_s_type *ph_ptr = cmph_ptr();
//   sd_ss_mode_pref_e_type sd_mode_pref;
//
//   if( mode_pref == CM_MODE_PREF_NO_CHANGE ||
//       mode_pref == CM_MODE_PREF_PERSISTENT ) {
//
//     CM_MSG_MED("Using mode pref = %d",ph_ptr->mode_pref,0,0);
//     mode_pref = ph_ptr->mode_pref;
//
//   }
//
//   if( hybrid_pref == CM_HYBR_PREF_NO_CHANGE ) {
//
//     CM_MSG_MED("Using hybr pref = %d",ph_ptr->hybr_pref,0,0);
//     hybrid_pref = ph_ptr->hybr_pref;
//   }
//
//   sd_mode_pref = map_cm_mode_pref_to_sd_mode_pref( mode_pref );
//
//   switch(sd_mode_pref) {
//
//   case SD_SS_MODE_PREF_AMPS:
//   case SD_SS_MODE_PREF_CDMA:
//   case SD_SS_MODE_PREF_HDR:
//   case SD_SS_MODE_PREF_GSM:
//   case SD_SS_MODE_PREF_WCDMA:
//   case SD_SS_MODE_PREF_CDMA_AMPS:
//   case SD_SS_MODE_PREF_GPS:
//   case SD_SS_MODE_PREF_ANY_BUT_HDR:
//
//       /* This combination is valid iff hybrid_pref is OFF */
//       return (hybrid_pref == CM_HYBR_PREF_OFF);
//
//   case SD_SS_MODE_PREF_DIGITAL:
//   case SD_SS_MODE_PREF_ANY:
//
//       /* Hybrid operation here may be on or off */
//       return TRUE;
//
//     default:
//       CM_MSG_ERROR("Invalid mode pref",0,0,0);
//   }
//
//   return FALSE;
//

} /* cmph_is_valid_mode_hybrid_pref() */
/*lint +esym(715,mode_pref) */
/*lint +esym(715,hybrid_pref) */



/*===========================================================================

FUNCTION cmph_call_start

DESCRIPTION
  Does necessary phone's call start activities.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmph_call_start
(

    cmcall_s_type    *call_ptr
        /* Pointer to a call object */
)
/*lint -esym(818, call_ptr) */
{

  cmph_s_type        *ph_ptr = cmph_ptr();
      /* Point at phone object */

  cmss_s_type        *ss_ptr = cmss_ptr();
      /* Point at ss object */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  db_items_value_type  db_item;
      /* Item to do db_get() and db_put() */
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_ptr   != NULL );
  CM_ASSERT( call_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( !ph_ptr->is_in_use || !ph_ptr->is_in_use_reported )
  {
    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    /* For now also update the data-base.
    */
    db_item.in_use = TRUE;
    db_put( DB_IN_USE, &db_item );
    #endif

    /* Indicate that phone is now in use
    ** and notify clients of event.
    */
    ph_ptr->is_in_use = TRUE;
    cmph_event( ph_ptr, CM_PH_EVENT_IN_USE_STATE);

  }

  if ( ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA )
  {
    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

    /* If this is an OTASP call, disable AUTO-NAM if necessary.
    */
    if( ( CM_CALL_TYPE_IS_OTASP(call_ptr->call_type) ||
          CM_CALL_TYPE_IS_OTAPA(call_ptr->call_type)
        )
        &&
        ph_ptr->nam_sel == CM_NAM_AUTO )
    {
      otasp_autonam_restore = TRUE;
      ph_ptr->nam_sel = ph_ptr->curr_nam;

      cmnv_item_ptr->auto_nam = FALSE;
      cmnv_write( NV_AUTO_NAM_I, cmnv_item_ptr );
    }
    else
    {
      otasp_autonam_restore = FALSE;
    }

    #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
  } /* if ( ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA ) */
  else
  {

    #if (defined FEATURE_GSM) || (defined FEATURE_WCDMA)
#error code not present
    #endif

  } /* else of if ( ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA ) */

  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
#error code not present
  #endif /* FEATURE_GPSONE_MSBASED */

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #endif   /* FEATURE_GSM || FEATURE_WCDMA */

  /* Phone has to be in use once we're done with all of the above
  */
  CM_ASSERT( ph_ptr->is_in_use );
  CM_ASSERT( ph_ptr->is_in_use_reported );

  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
#error code not present
  #endif

  return;

} /* cmph_call_start() */
/*lint +esym(818, call_ptr) */

/*===========================================================================

FUNCTION cmph_check_and_unforce_orig_mode

DESCRIPTION
  Process the origination parameters queue, and unforce the orig modes if
  required

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


void cmph_check_and_unforce_orig_mode(

  cm_ss_e_type ss
    /* The ss on which to process the orig update
    */
)
{
  cm_orig_q_s_type      *q_ptr;

  cm_orig_q_s_type      *q_next_ptr;

  cmss_s_type           *ss_ptr       = cmss_ptr();
      /* Point at serving system object */

  dword                  uptime       = clk_uptime();
      /* Get the current uptime */

  boolean                is_q_updated    = FALSE;

  boolean                is_delete_orig_para = FALSE;
  boolean                is_act_below_ph_deleted = FALSE;
  int                    cnt_below_ph = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( ss_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Step 1: Step through the queue and delete all the modes that are marked
  **        for deletion
  ** Step 2: Force the mode that is at the top of the queue
  */

  q_ptr = cmtask_orig_para_get_first(ss);

  while(q_ptr != NULL)
  {
    q_next_ptr = cmtask_orig_para_get_next(q_ptr, ss);

    if( q_ptr->orig.orig_pref_term  == CM_PREF_TERM_CM_1_CALL_PLUS &&
        uptime >= q_ptr->orig.orig_mode_uptime )
    {

      /* Unforce the origination mode now if one of the following conditions is
      ** true:
      **
      ** 1. MS is currently not busy trying to reacquire service.
      ** 2. MS has lost service.
      ** 3. We are CMPH_HOLD_MODE_PREF_TIME or more in overtime.
      */
      if( ss == CM_SS_MAIN )
      {
        if( !ss_ptr->is_pref_srv_acq ||
             ss_ptr->main_srv_status == SYS_SRV_STATUS_NO_SRV ||
             uptime >= (q_ptr->orig.orig_mode_uptime + CMPH_HOLD_MODE_PREF_TIME) )
        {
          is_delete_orig_para = TRUE;
        }
      }
      else if( ss == CM_SS_HYBR_1 )
      {
        if( !ss_ptr->hybr_1_is_pref_srv_acq ||
             ss_ptr->hybr_1_srv_status == SYS_SRV_STATUS_NO_SRV ||
             uptime >= (q_ptr->orig.orig_mode_uptime + CMPH_HOLD_MODE_PREF_TIME) )
        {
          is_delete_orig_para = TRUE;
        }
      }
      else
      {
        is_delete_orig_para = TRUE;
      }

      if( is_delete_orig_para )
      {
        CM_MSG_LOW("cmph_force_orig_mode_on_the_fly() on %d || %d || %d",
                    (!ss_ptr->is_pref_srv_acq),
                    (ss_ptr->info.srv_status == SYS_SRV_STATUS_NO_SRV),
                    (uptime >= q_ptr->orig.orig_mode_uptime + CMPH_HOLD_MODE_PREF_TIME));
        CM_MSG_LOW("is_pref_srv_acq = %d, srv_status = %d, uptime = %d",
                    ss_ptr->is_pref_srv_acq,
                    ss_ptr->info.srv_status,
                    uptime);
        CM_MSG_LOW("orig_mode_uptime = %d,CMPH_HOLD_MODE_PREF_TIME = %d",
                    q_ptr->orig.orig_mode_uptime,
                    CMPH_HOLD_MODE_PREF_TIME,
                    0);


        if (q_ptr->orig.act_priority == CM_ACT_PRIORITY_BELOW_PH )
        {
          is_act_below_ph_deleted = TRUE;
        }
        /* Remove this q_ptr from the queue
        */
        is_q_updated = cmtask_orig_para_delete( ss, q_ptr, TRUE) || is_q_updated;

      } /* if */

    }/* if q_ptr ... */

    q_ptr = q_next_ptr;

  }/* while q_ptr ... */


  cnt_below_ph = cmtask_orig_para_count_2(ss,
                                          CM_ACT_PRIORITY_BELOW_PH,
                                          CM_CMP_CRITERIA_EQUAL_TO) ;

  /* If we deleted any origination modes, call the unforce routine
  */

 /*
  ** force if an activity with CM_ACT_PRIORITY_BELOW_PH
  ** was dequeued and no other such activities exist in the Q.
  ** example scenario:
  ** 1)PS call is made in cs+ps.
  ** 2)sys_sel_pref changes to cs only.
  ** 3)We force with cs+ps, but change the ph_ptr (top) to CS_only to be forced
  **   later.
  ** 4)Once the data call ends we need to force the top again.
  */

  if( is_q_updated || (is_act_below_ph_deleted && cnt_below_ph == 0) )
  {
     cmph_force_orig_mode_on_the_fly(ss);
  }
  return;
}


/*===========================================================================

FUNCTION cmph_remove_orig_mode

DESCRIPTION
  Process the origination parameters queue, and remove the specified orig
  modes

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


void cmph_remove_orig_mode(

  cm_ss_e_type ss,
    /* The ss on which to process the orig update
    */

  cm_act_type_e_type act_type,
    /* The activity type to check
    */

  cm_act_priority_e_type prio_type
    /* The priority type to check, use CM_ACT_PRIORITY_MAX for wildcard entry
    ** If wildcard entry is specified, then all activities of act_type will
    ** be removed regardless of act_prio
    */
)
{
  cm_orig_q_s_type      *q_ptr;

  cm_orig_q_s_type      *q_next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Step 1: Step through the queue and delete all the modes that are marked
  **        for deletion
  */

  q_ptr = cmtask_orig_para_get_first(ss);

  while(q_ptr != NULL)
  {
    q_next_ptr = cmtask_orig_para_get_next(q_ptr, ss);

    if( cmph_is_pref_term_temporary(
                            q_ptr->orig.orig_pref_term ) &&
              (q_ptr->act_type == act_type) &&
              (q_ptr->orig.act_priority == prio_type ||
               prio_type == CM_ACT_PRIORITY_MAX)
      )
    {
      CM_MSG_HIGH( "Deleting act type %d",
                   q_ptr->act_type, 0, 0
                 );

      /* Remove this q_ptr from the queue
      */
      (void)cmtask_orig_para_delete( ss, q_ptr, TRUE);

    }/* if q_ptr ... */

    q_ptr = q_next_ptr;

  }/* while q_ptr ... */

  return;
} /* cmph_check_and_unforce_data_orig_mode() */

/*===========================================================================

FUNCTION cmph_pd_session_start

DESCRIPTION
  Does necessary phone's pd session start activities.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_pd_session_start()
{

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #endif   /* FEATURE_GSM || FEATURE_WCDMA */

}/* cmph_pd_session_start */


/*===========================================================================
FUNCTION cmph_pd_session_end

DESCRIPTION
  Does necessary phone's pd session end activities.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_pd_session_end()
{

  #ifdef FEATURE_GPSONE
#error code not present
  #endif /* FEATURE_GPSONE */

} /* cmpd_session_end() */


/*===========================================================================

FUNCTION cmph_call_end

DESCRIPTION
  Does necessary phone's call end activities.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_call_end
(

  const cmcall_s_type    *call_ptr
      /* Pointer to a call object */
)
{
  cmph_s_type             *ph_ptr = cmph_ptr();
     /* Get the pointer to the phone object */

  cmss_s_type             *ss_ptr = cmss_ptr();
      /* Point at ss object */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  db_items_value_type  db_item;
    /* Item to do db_get() and db_put() */
  #endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);
  CM_ASSERT(ph_ptr   != NULL);
  CM_ASSERT(ss_ptr   != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  if ( ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA )
  {
    CM_ASSERT( call_ptr != NULL );
  }
  #endif  /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */


  #ifdef FEATURE_JCDMA
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Disable voice activity test whenever call ends
  */
  txc_set_voc_activity( FALSE, 0, 0, 0);
  #endif /* FEATURE_JCDMA */

  if (( ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA ) ||
      #ifdef FEATURE_HDR
#error code not present
      #endif  /* FEATURE_HDR */
      ( ss_ptr->info.sys_mode == SYS_SYS_MODE_AMPS ))
  {
    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If this is an OTASP call, enable AUTO-NAM if necessary. Do it only if
    ** it was enabled prior to OTASP call.
    */
    if( ( CM_CALL_TYPE_IS_OTASP(call_ptr->call_type) ||
          CM_CALL_TYPE_IS_OTAPA(call_ptr->call_type)
        )
        &&
        otasp_autonam_restore == TRUE )
    {
      otasp_autonam_restore = FALSE;
      ph_ptr->nam_sel = CM_NAM_AUTO;

      cmnv_item_ptr->auto_nam = TRUE;
      cmnv_write( NV_AUTO_NAM_I, cmnv_item_ptr );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Indicate that phone is NOT in use any more
    ** and notify clients of event.
    */

    /* For now also update the data-base ????
    */
    db_item.in_use = FALSE;
    db_put( DB_IN_USE, &db_item );

    db_item.conv = FALSE;
    db_put( DB_CONV, &db_item );

    #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
  } /* if ( ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA ) */


  #ifdef FEATURE_IS2000_REL_A
  if ( cm_number_of_allocated_calls() == 1 )
  {
    ph_ptr->is_in_use = FALSE;
  }
  #else
  ph_ptr->is_in_use = FALSE;
  #endif /* FEATURE_IS2000_REL_A */
  cmph_event( ph_ptr, CM_PH_EVENT_IN_USE_STATE);

  /* Check if we have a PD session going
  ** If a PD session is going, don't unforce the mode
  */
  #if( defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE) )
#error code not present
  #endif /* FEATURE_GPSONE */
  {

    /* Check whether we need to unforce the mode preference that was forced
    ** by the user.
    */

    switch( ph_ptr->pref_term )
    {
      case CM_PREF_TERM_1_CALL:
      case CM_PREF_TERM_1_CALL_OR_TIME:

        /* If mode was forced by a client for the duration of one call,
        ** unforce it now and notify clients of event.
        */
        (void) cmph_unforce_mode_on_the_fly( ph_ptr );

        cmph_event( ph_ptr, CM_PH_EVENT_SYS_SEL_PREF );
        break;


      case CM_PREF_TERM_CM_1_CALL:

        /* If mode preference was forced by CM on the fly for the duration
        ** of the one call, unforce the mode now.
        */
        (void) cmph_unforce_mode_on_the_fly( ph_ptr );
        break;


      case CM_PREF_TERM_CM_1_CALL_PLUS:

        /* If mode preference was forced by CM on the fly for the duration
        ** of one call plus, indicate that we should unforce the mode
        ** preference in CMPH_HOLD_MODE_PREF_TIME seconds.
        **
        ** NOTE that we delay unforcing the mode preference in order to
        ** avoid loading MC with pref sys changes in a situation where
        ** the user is pressing SEND, END, SEND, END, ...
        */
        ph_ptr->mode_uptime = clk_uptime() + CMPH_HOLD_MODE_PREF_TIME;


        break;


      default:
        break;
    }

    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
    #endif  /* FEATURE_GSM || FEATURE_WCDMA */

  } /* if */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If answer-voice setting is for one call, default it back
  ** to answer-voice-as-voice and notify clients of event.
  */
  if( (ph_ptr->answer_voice == CM_ANSWER_VOICE_AS_FAX_ONCE    ||
       ph_ptr->answer_voice == CM_ANSWER_VOICE_AS_MODEM_ONCE) &&
       !call_ptr->is_user_originated_call )
  {
    ph_ptr->answer_voice = CM_ANSWER_VOICE_AS_VOICE;
    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    db_item.voice_as_data  = DB_VOICE_AS_DATA_NEVER;
    db_put( DB_VOICE_AS_DATA, &db_item );
    #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
    cmph_event( ph_ptr, CM_PH_EVENT_ANSWER_VOICE );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform CMSS that the phone is not in use.
  */
  cmss_proc_call_ended();

  CM_ASSERT(!ph_ptr->is_in_use);
} /* cmph_call_end() */



/*===========================================================================

FUNCTION cmph_is_waiting_for_reply

DESCRIPTION
  Check whether the phone object is currently waiting for a reply.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  TRUE if phone object is currently waiting for a rely, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_waiting_for_reply( void )
{
  return( cm_reply_is_waiting(&cmph_ptr()->reply) );

} /* cmph_is_waiting_for_reply() */



/*===========================================================================

FUNCTION cmph_info_get

DESCRIPTION
  Copy the current phone state information into a specified buffer.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  TRUE if phone information is valid, FALSE otherwise.

  Note that phone information only becomes valid after cmph_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_info_get(

    cm_ph_info_s_type    *ph_info_ptr
        /* Copy phone state info into this buffer */
)
{

  cmph_s_type          *ph_ptr = cmph_ptr();
      /* Point at phone object */
  #ifdef FEATURE_DDTM_CNTL
  unsigned int i;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( ph_info_ptr != NULL );

  /* Verify that object was properly initialized.
  */
  CM_ASSERT( CM_INIT_CHECK(ph_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check integrity of phone state.
  */

  CM_ASSERT( ph_ptr->is_in_use == TRUE ||  /* check against corruption */
             ph_ptr->is_in_use == FALSE );

  CM_ASSERT( BETWEEN(ph_ptr->oprt_mode,
                     SYS_OPRT_MODE_NONE,
                     SYS_OPRT_MODE_MAX) );

  CM_ASSERT( BETWEEN(ph_ptr->cdma_lock_mode,
                     CM_CDMA_LOCK_MODE_NONE,
                     CM_CDMA_LOCK_MODE_MAX) );

  CM_ASSERT( BETWEEN(ph_ptr->mode_pref,
                     CM_MODE_PREF_NONE,
                     CM_MODE_PREF_MAX) );

  CM_ASSERT( BETWEEN(ph_ptr->pref_term,
                     CM_PREF_TERM_NONE,
                     CM_PREF_TERM_MAX) );

  CM_ASSERT( BETWEEN(ph_ptr->band_pref,
                     CM_BAND_PREF_NONE,
                     CM_BAND_PREF_MAX) );

  CM_ASSERT( BETWEEN(ph_ptr->roam_pref,
                     CM_ROAM_PREF_NONE,
                     CM_ROAM_PREF_MAX) );

  CM_ASSERT( BETWEEN(ph_ptr->nam_sel,
                     CM_NAM_NONE,
                     CM_NAM_MAX) );

  CM_ASSERT( ph_ptr->curr_nam != CM_NAM_AUTO );
  CM_ASSERT( BETWEEN(ph_ptr->curr_nam,
                     CM_NAM_NONE,
                     CM_NAM_MAX) );

  CM_ASSERT( ph_ptr->nam_sel == ph_ptr->curr_nam ||
             ph_ptr->nam_sel == CM_NAM_AUTO )

  CM_ASSERT( BETWEEN(ph_ptr->packet_state,
                     CM_PACKET_STATE_NONE,
                     CM_PACKET_STATE_MAX) );

   CM_ASSERT( BETWEEN(ph_ptr->ph_state,
                     CM_PH_STATE_NONE,
                     CM_PH_STATE_MAX) );

  CM_ASSERT( BETWEEN(ph_info_ptr->network_sel_mode_pref,
                     CM_NETWORK_SEL_MODE_PREF_NONE,
                     CM_NETWORK_SEL_MODE_PREF_MAX) );

  CM_ASSERT( BETWEEN(ph_info_ptr->acq_order_pref,
                     CM_GW_ACQ_ORDER_PREF_NONE,
                     CM_GW_ACQ_ORDER_PREF_MAX) );

  CM_ASSERT( INRANGE(ph_info_ptr->srv_domain_pref,
                     CM_SRV_DOMAIN_PREF_NONE,
                     (CM_SRV_DOMAIN_PREF_MAX-1)) );

  CM_ASSERT( BETWEEN(ph_info_ptr->answer_voice,
                     CM_ANSWER_VOICE_NONE,
                     CM_ANSWER_VOICE_MAX) );


  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

  #ifdef FEATURE_DDTM_CNTL
  CM_ASSERT( BETWEEN(ph_ptr->ddtm_pref,
                      CM_DDTM_PREF_NONE,
                      CM_DDTM_PREF_MAX) );
  #endif /* FEATURE_DDTM_CNTL */

  CM_ASSERT( INRANGE(ph_ptr->line,
                     CM_ALS_LINE_NONE,
                     CM_ALS_LINE_MAX) );

  CM_ASSERT( INRANGE(ph_ptr->line_switching,
                     CM_ALS_LINE_SWITCHING_NONE,
                     CM_ALS_LINE_SWITCHING_MAX));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy appropriate phone parameters into target buffer.
  */

  ph_info_ptr->is_in_use           = ph_ptr->is_in_use;
  ph_info_ptr->oprt_mode           = ph_ptr->oprt_mode;
  ph_info_ptr->test_control_type   = ph_ptr->test_control_type;
  ph_info_ptr->cdma_lock_mode      = ph_ptr->cdma_lock_mode;

  ph_info_ptr->answer_voice        = ph_ptr->answer_voice;

  ph_info_ptr->srv_domain_pref     = ph_ptr->srv_domain_pref;

  ph_info_ptr->band_pref           = ph_ptr->band_pref;
  ph_info_ptr->roam_pref           = ph_ptr->roam_pref;

  ph_info_ptr->nam_sel             = ph_ptr->nam_sel;
  ph_info_ptr->curr_nam            = ph_ptr->curr_nam;
  ph_info_ptr->packet_state        = ph_ptr->packet_state;

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

  ph_info_ptr->network_sel_mode_pref
                                   = ph_ptr->network_sel_mode_pref;
  ph_info_ptr->acq_order_pref      = ph_ptr->acq_order_pref;
  ph_info_ptr->plmn                = ph_ptr->plmn;
  ph_info_ptr->wlan_bss_info       = ph_ptr->wlan_bss_info;
  #ifdef FEATURE_WLAN_WPS
#error code not present
  #endif

  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
  #endif /* FEATURE_WCDMA, FEATURE_GSM */

  ph_info_ptr->wlan_bss_list       = ph_ptr->wlan_bss_list;

  ph_info_ptr->is_gw_subscription_available
                                   = ph_ptr->is_gw_subscription_available;

  ph_info_ptr->is_cdma_subscription_available
                                   = ph_ptr->is_cdma_subscription_available;

  ph_info_ptr->is_wlan_subscription_available
                                   = ph_ptr->is_wlan_subscription_available;

  ph_info_ptr->rtre_control        = ph_ptr->rtre_control;
  ph_info_ptr->rtre_config         = ph_ptr->rtre_config;

  #ifdef FEATURE_DDTM_CNTL
  ph_info_ptr->ddtm_pref           = ph_ptr->ddtm_pref;
  ph_info_ptr->ddtm_act_mask       = ph_ptr->ddtm_act_mask;
  ph_info_ptr->orig_ddtm_act_mask  = ph_ptr->cur_ddtm_act_mask;
  ph_info_ptr->ddtm_num_srv_opt    = ph_ptr->ddtm_num_srv_opt;
  ph_info_ptr->cur_ddtm_status     = ph_ptr->cur_ddtm_status;

  for ( i= 0;
        i < ph_ptr->ddtm_num_srv_opt && i < ARR_SIZE(ph_info_ptr->ddtm_srv_opt_list);
        i++
      )
  {
    ph_info_ptr->ddtm_srv_opt_list[i] = ph_ptr->ddtm_srv_opt_list[i];
  }
  #endif  /* FEATURE_DDTM_CNTL */

  ph_info_ptr->cphs_allowed   = ph_ptr->cphs_allowed;
  ph_info_ptr->als_allowed    = ph_ptr->als_allowed;
  ph_info_ptr->line_switching = ph_ptr->line_switching;
  ph_info_ptr->line           = ph_ptr->line;


  #if defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
      defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM)
#error code not present
  #endif /* FEATURE_UMTS_1X_HANDOVER */

  ph_info_ptr->is_net_lists_present = ph_ptr->is_net_lists_present;

  ph_info_ptr->wlan_pref            = ph_ptr->wlan_pref;

  ph_info_ptr->ph_state         = ph_ptr->ph_state;

  if( ph_info_ptr->ph_state != CM_PH_STATE_EMERG_CB )
  {
    ph_info_ptr->mode_pref          = ph_ptr->mode_pref;
    ph_info_ptr->pref_term          = ph_ptr->pref_term;
  }
  else
  {
    ph_info_ptr->mode_pref          = CM_MODE_PREF_EMERGENCY;
    ph_info_ptr->pref_term          = CM_PREF_TERM_PWR_CYCLE;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Gives the status of plmn list request (success/abort).
  */
  ph_info_ptr->available_networks_list_cnf =
                                       ph_ptr->available_networks_list_cnf;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Gives the mode capability and band capability.
  */
  ph_info_ptr->mode_capability = ph_ptr->mode_capability;
  ph_info_ptr->band_capability = ph_ptr->band_capability;

  /* Copy the current prl preferred setting.
  */
  ph_info_ptr->prl_pref        = ph_ptr->prl_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Gives the phone's measurement mode - HDR access enabled /disabled 
  */
  #ifdef FEATURE_CM_SS_MEAS
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return boolean indicating whether phone information is valid.
  */
  return BOOLEAN( CM_INIT_CHECK(ph_ptr->init_mask) );

} /* cmph_info_get() */



/*===========================================================================

FUNCTION cmph_client_cmd_proc

DESCRIPTION
  Process clients' phone commands.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_client_cmd_proc(

    cm_cmd_s_type  *cmd_ptr
        /* pointer to a CM command */
)
{

  cmph_s_type             *ph_ptr       = cmph_ptr();
      /* Point at phone object */

  cm_ph_cmd_err_e_type     ph_cmd_err;
  boolean                  wait_for_reply;
  boolean                  network_sel_mode_pref_changed;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmph_client_cmd_proc()",0,0,0 );

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_PH );

  /* We are NOT supposed to process commands while
  ** waiting for MC reply.
  */
  CM_ASSERT( !cm_reply_is_waiting(&ph_ptr->reply) )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start by checking whether this is a valid command
  ** relative to the current state of the phone.
  */
  ph_cmd_err = cmph_client_cmd_check( ph_ptr,
                                      cmd_ptr,
                                      &network_sel_mode_pref_changed );
  /*
  ** Notify clients of command processing status.
  ** If error found, return.
  */
  cmph_client_cmd_err( cmd_ptr, ph_cmd_err );
  if( ph_cmd_err != CM_PH_CMD_ERR_NOERR )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no errors were found,
  ** so forward the client command to MC.
  */
  wait_for_reply = cmph_client_cmd_forward_mmll( ph_ptr,
                                                 cmd_ptr,
                                                 network_sel_mode_pref_changed );

  /* If NO need to wait for replay from Lower Layer,
  ** call the reply function right a way.
  */
  if( ! wait_for_reply )
  {
    cmph_client_cmd_mmll_reply( ph_ptr,
                              cmd_ptr,
                              CM_PH_CMD_ERR_NOERR );
  }

} /* cmph_client_cmd_proc() */


/*===========================================================================

FUNCTION cmph_sd_rpt_proc

DESCRIPTION
  Process System Determination reports


DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD reports */
)
{

  /*lint -e{826} */
  const cm_sd_rpt_u_type         *sd_rpt_ptr = (cm_sd_rpt_u_type *) rpt_ptr;
    /* Pointer to SD reports */

  cmph_s_type              *ph_ptr     = cmph_ptr();
      /* Point at phone object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sd_rpt_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmph_sd_rpt_proc(), cmd=%d", sd_rpt_ptr->hdr.cmd, 0, 0);

  /* Do command specific processing as follows:
  ** 1. Updating appropriate phone object fields.
  ** 2. Notifying clients of event if necessary.
  */

  switch( sd_rpt_ptr->hdr.cmd )
  {
    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    /* Auto-NAM change is appropriate.
    */
    case CM_NAM_CHANGED_F:

      /* If NAM selection is set to auto-NAM and current origination mode is
      ** none or normal, change the current NAM to the proposed NAM selection
      ** and command MC to switch to the new NAM selection.
      */
      {

        cm_orig_q_s_type  *ph_orig_ptr;
        ph_orig_ptr = cmtask_orig_para_get_top( CM_SS_MAIN );

        if( ph_ptr->nam_sel == CM_NAM_AUTO
                            &&

            ( ph_orig_ptr == NULL                                   ||
            /* No need to check for ph_orig_ptr for not NULL
            ** as due to short circuit evaluation, we will dereference if
            ** only if the first condition is false
            */
            ph_orig_ptr->orig.orig_mode  == SD_SS_ORIG_MODE_NONE  ||
              ph_orig_ptr->orig.orig_mode  == SD_SS_ORIG_MODE_NORMAL
          )
          )
        {
          /* Change the current NAM to the proposed NAM selection.
          */
          ph_ptr->curr_nam = (cm_nam_e_type )sd_rpt_ptr->nam_changed.nam;
          cmph_event( ph_ptr, CM_PH_EVENT_CURR_NAM );

          /* Write to MV the new NAM selection.
          */
          cmnv_item_ptr->curr_nam = (byte) sd_rpt_ptr->nam_changed.nam;
          cmnv_write( NV_CURR_NAM_I, cmnv_item_ptr );

          /* Command MC to switch to the new NAM selection.
          */
          cmph_send_nam_change_to_mc( ph_ptr, ph_ptr->curr_nam );

          /* Read NV items for the new NAM
          */
          cmph_read_nv_items(ph_ptr);
        }
        break;
      }
    #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Enter E911 callback mode.
    */
    case CM_E911_CALLBACK_F:
    {

      cm_orig_q_s_type *ph_orig_top_ptr = cmtask_orig_para_get_top( CM_SS_MAIN );

      if( ph_orig_top_ptr != NULL &&
          ph_orig_top_ptr->orig.orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG )
      {
        ph_orig_top_ptr->orig.orig_mode = SD_SS_ORIG_MODE_EMERG_CB;
      }

    } /* case CM_E911_CALLBACK_F */

      /* Enter emergency callback mode
      */
      cmph_enter_emergency_cb( ph_ptr, TRUE );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_ACQ_FAIL_F:
    {
      #ifdef FEATURE_HICPS_STACK_SYNC_UP
#error code not present
      #endif
      boolean is_gw_mode          = FALSE;
      boolean is_cdma_mode        = FALSE;
      boolean is_hdr_mode         = FALSE;
      boolean is_ph_obj_cdma_mode = FALSE;
      boolean is_ph_obj_gw_mode   = FALSE;
      boolean is_ph_ptr_gw_mode   = FALSE;
      boolean is_ph_obj_hdr_mode  = FALSE;
      boolean is_kick_main        = FALSE;
      cm_orig_q_s_type  *ph_orig_ptr = 
                            cmtask_orig_para_search_act_type( CM_SS_MAIN, 
                                                              CM_ACT_TYPE_PH_OBJ);

      SYS_ARG_NOT_USED( is_kick_main );
      SYS_ARG_NOT_USED( is_hdr_mode );
      SYS_ARG_NOT_USED( is_ph_obj_hdr_mode );
      SYS_ARG_NOT_USED( is_cdma_mode );
      SYS_ARG_NOT_USED( is_ph_obj_cdma_mode );
      SYS_ARG_NOT_USED( is_gw_mode );
      SYS_ARG_NOT_USED( is_ph_obj_gw_mode );
      SYS_ARG_NOT_USED( is_ph_ptr_gw_mode );

      /* 
      ** CHECK HARDWARE CAPABILITY.
      */
      /* Check the CDMA mode against the phone's band pref
      */
      is_cdma_mode = cmph_is_valid_mode_pref( CM_MODE_PREF_CDMA_ONLY );

      /* Check the HDR mode against the phone's band pref
      */
      is_hdr_mode = cmph_is_valid_mode_pref( CM_MODE_PREF_HDR_ONLY );

      /* Check the GW mode against the phone's band pref
      */
      is_gw_mode = cmph_is_valid_mode_pref( CM_MODE_PREF_GSM_WCDMA_ONLY );

      /* 
      ** CHECK USER PREFERENCE ON THE PHN OBJECT.
      */
      /* Check if the phone has CDMA mode
      */
      is_ph_obj_cdma_mode = 
      cmcall_misc_is_mode_pref( ph_orig_ptr->orig.orig_mode_pref ,CM_MODE_PREF_CDMA_ONLY );

      /* Check if the phone has HDR mode
      */
      is_ph_obj_hdr_mode = 
        cmcall_misc_is_mode_pref( ph_orig_ptr->orig.orig_mode_pref,CM_MODE_PREF_HDR_ONLY );


      /* Check if the phone has GW mode
      */
      is_ph_obj_gw_mode = 
        cmcall_misc_is_mode_pref(ph_orig_ptr->orig.orig_mode_pref,CM_MODE_PREF_GSM_WCDMA_ONLY);

      /*
      ** Check if phone ptr has GW.
      */
      is_ph_ptr_gw_mode = 
        cmcall_misc_is_mode_pref(ph_ptr->mode_pref ,CM_MODE_PREF_GSM_WCDMA_ONLY);


      /* Acquisition fail report.
      */
      CM_MSG_HIGH("CM_ACQ_FAIL_F, ss=%d, hybr=%d, kicked=%d",
                   sd_rpt_ptr->acq_fail.ss, ph_ptr->hybr_pref,
                   *cmph_get_is_kicked_hybr_ptr() );

      /* If this report is for main system determination, we did not kick
      ** HDR so far and hybrid is on, kick HDR into acquisition.
      */
      #ifdef FEATURE_HICPS_STACK_SYNC_UP
#error code not present
      #else /* HICPS */

      if( sd_rpt_ptr->acq_fail.ss == SD_SS_MAIN &&
          #ifndef FEATURE_UNIFORM_SCAN_OOS
          !*cmph_get_is_kicked_hybr_ptr() &&
          #endif
          cmpref_is_hybr_status_on()
          #ifndef FEATURE_HYBR_GW
          && ph_ptr->is_cdma_subscription_available
          #endif
      )
      {

        CM_MSG_HIGH("---Starting HYBR_1 without 1x",0,0,0 );

        cmpref_proc_cmd_pref_sys_chgd(
                             SD_SS_HYBR_HDR,
                             ( cmcall_is_hybr_hdr_orig()?
                                          SD_SS_PREF_REAS_ORIG_START_PS:
                                          SD_SS_PREF_REAS_USER ),
                             ( ph_ptr->int_hybr_pref ==
                                          SD_SS_HYBR_PREF_CDMA__HDR ?
                                          SD_SS_ORIG_MODE_COLLOC :
                                          SD_SS_ORIG_MODE_NONE ),
                             cmph_map_cm_mode_pref_to_sd_mode_pref( ph_ptr->mode_pref ),
                             cmph_map_cm_band_pref_to_sd_band_pref( ph_ptr->band_pref ),
                             cmph_map_cm_prl_pref_to_sd_prl_pref( ph_ptr->prl_pref ),
                             cmph_map_cm_roam_pref_to_sd_roam_pref( ph_ptr->roam_pref ),
                             ph_ptr->int_hybr_pref,
                             cmph_map_otasp_act_code_to_sd_band(CM_OTASP_ACT_CODE_NONE),
                             cmph_map_otasp_act_code_to_sd_blksys(CM_OTASP_ACT_CODE_NONE),
                             SD_SS_AVOID_SYS_IDLE,
                             0,
                             ph_ptr->plmn,
                             cmph_map_cm_srv_domain_pref_to_sd(ph_ptr->srv_domain_pref ),
                             cmph_map_cm_acq_order_pref_to_sd(ph_ptr->acq_order_pref  ),
                             NULL
                           );

        *cmph_get_is_kicked_hybr_ptr() = TRUE;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      } /* ACQ FAIL SS MAIN*/

      #endif /* HICPS */

      break;

    } /* case ACQ_FAIL */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Entering power-down sleep mode
    */
    case CM_STANDBY_SLEEP_F:
      cmph_event( ph_ptr, CM_PH_EVENT_STANDBY_SLEEP );
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Exiting power-down sleep mode
    */
    case CM_STANDBY_WAKE_F:
      cmph_event( ph_ptr, CM_PH_EVENT_STANDBY_WAKE );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Enter Low Power Mode (LPM)
    */
    case CM_LPM_OK_F:

      /* NOTE! CM_LPM_OK_F is a reply command to an earlier MC_LPM_F
      ** command sent by CM to MC and should be processed by
      ** cmph_client_cmd_mc_reply(), being called at the end of this
      ** function.
      **
      ** If, however, we are currently NOT waiting for such a reply from MC
      ** (i.e. CM never sent the MC_LPM_F to MC in the first place or MC
      ** responded after the max reply time is expired ), do not respond
      ** with a new event at this point. Once CM has issued LPM event due
      **
      ** Removed setting oprt_mode when unexpected LPM_OK arrives.
      **
      ** - Client sends oprt_mode change to LPM
      ** - CM times out on CMPH_MAX_MC_REPLY_TIME and sends LPM in
      **   CM_PH_EVENT_OPRT_MODE
      ** - CM gets oprt_mode change to ONLINE.
      ** - MMOC sends LPM_OK for the initial LPM request.
      ** - CM changes oprt_mode to LPM, which is incorrect (CR 90176)
      **
      */
      if(! cm_reply_check(&ph_ptr->reply, sd_rpt_ptr->hdr.cmd) )
      {
        CM_ERR( "Received CM_LPM_OK_F when not expecting, Ignore",0,0,0 );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SRV_IND_INFO_F:

      /*
      ** Cancel the PS service request timer if the phone was able to acquire
      ** PS service.
      */
      if ( ( ph_ptr->ps_service_req_timer_active       ) &&
           ( sd_rpt_ptr->srv_ind_info.ss == SD_SS_MAIN )    )
      {
        if ( ( sd_rpt_ptr->srv_ind_info.si_info.srv_domain == SYS_SRV_DOMAIN_PS_ONLY ) ||
             ( sd_rpt_ptr->srv_ind_info.si_info.srv_domain == SYS_SRV_DOMAIN_CS_PS   )    )
        {
          cmph_deactivate_ps_service_req_timer();

        }
      }

      /*
      ** We need to hold the PS call origination after PS service is acquired to avoid
      ** the race condition that Network is releasing the PS Signaling connection for
      ** PS_ATTACH while mobile is sending PDP_CONTEXT ACTIVATE request. PS_ATTCH
      ** happens whenever PS service is required.
      **
      ** CM needs to wait to receive the PS_SIG_DOWN event after PS Service is acquired.
      ** When we change the system selection prefernece to acquire a PS service, we need
      ** to start the PS_SIG_DOWN timer.
      **
      ** ps_service_req_sent flag is used to decide if we need to start the PS_SIG_DOWN
      ** timer after PS service is acquired.
      */

      if ( ( sd_rpt_ptr->srv_ind_info.si_info.srv_domain == SYS_SRV_DOMAIN_PS_ONLY ) ||
           ( sd_rpt_ptr->srv_ind_info.si_info.srv_domain == SYS_SRV_DOMAIN_CS_PS   )    )
      {
         if(ph_ptr->is_ps_srv_req_sent)
         {
           ph_ptr->is_ps_srv_req_sent = FALSE;

           cmph_set_ps_sig_down_wait_time(CMPH_GW_PKT_SIGNALING_DOWN_WAIT_TIME);
         }
      }

      /* Do not kick SD HDR instance if we got service on SD main instance
      ** and we are no longer looking for preferred service.       */
      if( sd_rpt_ptr->srv_ind_info.ss == SD_SS_MAIN &&
          sys_srv_status_is_srv(sd_rpt_ptr->srv_ind_info.si_info.srv_status) &&
          !sd_rpt_ptr->srv_ind_info.si_info.is_pref_srv_acq
        )
      {
        *cmph_get_is_kicked_hybr_ptr() = TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_WLAN_BSS_LIST_F:

        cmph_process_wlan_bss_list(&(sd_rpt_ptr->wlan_bss_list));
     break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     
    case CM_WLAN_WPS_STATUS_F:
      
         cmph_process_wlan_wps_result(&(sd_rpt_ptr->wlan_wps_result));
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    default:
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  } /* switch( sd_rpt_ptr->hdr.cmd ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is a reply to an earlier command we sent to MC,
  ** also do reply processing.
  */
  if( cm_reply_check(&ph_ptr->reply, sd_rpt_ptr->hdr.cmd) )
  {
    /* Point at client command waiting for reply
    */
    cm_cmd_s_type  *reply_cmd_ptr = cm_reply_get(&ph_ptr->reply);
    CM_ASSERT( reply_cmd_ptr != NULL );

    /* Invoke the reply function with the appropriate error code.
    */
    cmph_client_cmd_mmll_reply( ph_ptr,
                              reply_cmd_ptr,
                              CM_PH_CMD_ERR_NOERR );

    /* We are done with processing the command that was waiting
    ** for reply - deallocate its command buffer.
    */
    cm_cmd_dealloc( reply_cmd_ptr );

    /* reset the reply info */
    cm_reply_init( &ph_ptr->reply );

    /* Turn on the signal that causes us to process any pending
    ** clients commands. This is necessary since we were NOT
    ** processing any clients commands while waiting for a reply
    ** from MC.
    */
    (void) rex_set_sigs( rex_self(), CM_CLIENT_CMD_Q_SIG );

  } /* if( cm_reply_check(&ph_ptr->reply, sd_rpt_ptr->hdr.cmd) ) */
} /* cmph_sd_rpt_proc() */


/*===========================================================================

FUNCTION cmph_ip_rpt_proc

DESCRIPTION
  Process IP specific reports

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_ip_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD reports */
)
{
  #if defined(FEATURE_IP_CALL)

  /*lint -e{826} */
  const cmipapp_rpt_s_type  *cmipapp_rpt_ptr = (cmipapp_rpt_s_type *) rpt_ptr;
    /* Pointer to a IP app report */

  cmph_s_type              *ph_ptr     = cmph_ptr();
      /* Point at phone object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( ph_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH ( "cmph_ip_rpt_proc, cmd=%d", cmipapp_rpt_ptr->hdr.cmd, 0, 0);

  /* Do command specific processing as follows:
  ** 1. Updating appropriate phone object fields.
  ** 2. Notifying clients of event if necessary.
  */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is a reply to an earlier command we sent to MC,
  ** also do reply processing.
  */
  if( cm_reply_check(&ph_ptr->reply, cmipapp_rpt_ptr->hdr.cmd ))
  {
    /* Point at client command waiting for reply
    */
    cm_cmd_s_type  *reply_cmd_ptr = cm_reply_get(&ph_ptr->reply);
    CM_ASSERT( reply_cmd_ptr != NULL );

    /* This check need not be made in other places where cm_reply_check() is
    ** called because CM_IP_APP_ALLAPPS_OPRT_CNF is a SD report and
    ** cmph_sd_rpt_proc is the only one having a reply_check called from
    ** cm_sdrpt_handle.
    */
    #if defined(FEATURE_IP_CALL)
    if (cmipapp_rpt_ptr->hdr.cmd == CM_IP_APP_ALLAPPS_OPRT_CNF)
    {
      /* Reset all IPAPP information
      ** Useful in reply_too_long() function
      ** where it is goes to make all ipapps go to
      ** deregister.
      **
      ** network_sel_mode_pref_changed does not mean
      ** anything for cmds other than SYS_SEL_PREF.
      ** Defaulting it to TRUE
      */
      if (cmph_client_cmd_forward_mmll (ph_ptr, reply_cmd_ptr, TRUE))
      {
        CM_ERR ("Wait for reply is TRUE", 0, 0, 0);
      }


    }
    #endif

    /* Invoke the reply function with the appropriate error code.
    */
    cmph_client_cmd_mmll_reply( ph_ptr,
                              reply_cmd_ptr,
                              CM_PH_CMD_ERR_NOERR );

    /* We are done with processing the command that was waiting
    ** for reply - deallocate its command buffer.
    */
    cm_cmd_dealloc( reply_cmd_ptr );

    /* reset the reply info */
    cm_reply_init( &ph_ptr->reply );

    /* Turn on the signal that causes us to process any pending
    ** clients commands. This is necessary since we were NOT
    ** processing any clients commands while waiting for a reply
    ** from MC.
    */
    (void) rex_set_sigs( rex_self(), CM_CLIENT_CMD_Q_SIG );

  } /* if( cm_reply_check(&ph_ptr->reply, sd_rpt_ptr->hdr.cmd) ) */

  #else /* FEATURE_IP_CALL */
  SYS_ARG_NOT_USED (rpt_ptr);
  #endif

} /* cmph_ip_rpt_proc() */



/*===========================================================================

FUNCTION cmph_generic_rpt_proc

DESCRIPTION
  Process Generic reports


DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_generic_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to generic reports */
)
{
  /*lint -save -e826 */
  const cm_generic_rpt_s_type         *gen_rpt_ptr = (cm_generic_rpt_s_type *) rpt_ptr;
  /*lint -restore */
    /* Pointer to SD reports */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( gen_rpt_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmph_generic_rpt_proc(), cmd=%d rpt=%d", gen_rpt_ptr->hdr.cmd ,gen_rpt_ptr->generic_rpt.dem_rpt.dem_rpt_type , 0);

  switch(gen_rpt_ptr->hdr.cmd)
  {

    case CM_DEM_RPT:
    {

      cm_dem_s_type *dem_ptr = cmph_dem_ptr();

      CM_ASSERT( dem_ptr != NULL );

      switch(gen_rpt_ptr->generic_rpt.dem_rpt.dem_rpt_type)
      {
        case CM_DEM_RPT_APPS_PWR_COLLAPSE:

          dem_ptr->apps_pwr_status =  CM_APPS_PWR_STATUS_PWR_COLLAPSE;
          break;

       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_DEM_RPT_APPS_PWR_RESTORED:

          if(dem_ptr->apps_pwr_status == CM_APPS_PWR_STATUS_PWR_COLLAPSE ||
             dem_ptr->apps_pwr_status == CM_APPS_PWR_STATUS_PWR_SUSPEND)
          {
            dem_ptr->apps_pwr_status =  CM_APPS_PWR_STATUS_PWR_RESTORE;

            #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
            #endif
          }

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_DEM_RPT_APPS_SUSPEND:

          dem_ptr->apps_pwr_status =  CM_APPS_PWR_STATUS_PWR_SUSPEND;
          break;

       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        default:
          break;
      } /* Switch rpt_type */

      break;
    } /* case CM_DEM_RPT */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      break;
  } /* switch hdr.cmd */
} /* cmph_generic_rpt_proc */


/*===========================================================================

FUNCTION cmph_timer_proc

DESCRIPTION
  Process timer events.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_timer_proc(

  cm_timer_event_type    timer_event
    /* Indicate specific timer event.
    **
    ** Note that currently timer event is not being used
    */
)
/*lint -esym(715,timer_event) */
{

  cmph_s_type           *ph_ptr = cmph_ptr();
      /* Point at phone object */

  dword                  uptime = clk_uptime();
      /* Get the current uptime */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

  db_items_value_type    db_item;
      /* Item to do db_get() and db_put() */
  #endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** If we receive PS Singaling down indication before timer
  ** expiry, we will stop the timer.
  **
  ** If the timer expires, we will stop waiting for the
  ** PS signaling down indication.
  **
  ** We can make the GW Pkt call only if we are not waiting
  ** for the Signaling down indicaiton.
  */

  if(ph_ptr->ps_signaling_down_wait_time <= uptime)
  {
    cmph_set_ps_sig_down_wait_time(CMPH_GW_PKT_WAIT_TIME_NONE);
  }


  /*
  ** If the PS service request timer expired, then notify the client(s)
  ** that the PS attach failed./
  */
  if ( ( ph_ptr->ps_service_req_timer_active    ) &&
       ( uptime > ph_ptr->ps_service_req_uptime )    )
  {
    cmph_event( ph_ptr, CM_PH_EVENT_PS_ATTACH_FAILED );
    cmph_deactivate_ps_service_req_timer();
  }


  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If answer voice setting is temporary, check whether the answer
  ** duration expired, in which case we need to restore the normal
  ** CM_ANSWER_VOICE_AS_VOICE setting and notify clients of event.
  **
  ** Note that we also update the answer-voice setting in the database
  ** to reflect this setting since MC consults it upon receiving a page.
  */

  CM_ASSERT( ph_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ph_ptr->answer_voice == CM_ANSWER_VOICE_AS_FAX_ONCE ||
      ph_ptr->answer_voice == CM_ANSWER_VOICE_AS_MODEM_ONCE )
  {
    if( uptime > ph_ptr->answer_uptime )
    {
      ph_ptr->answer_voice      = CM_ANSWER_VOICE_AS_VOICE;
      db_item.voice_as_data  = DB_VOICE_AS_DATA_NEVER;
      db_put( DB_VOICE_AS_DATA, &db_item );
      cmph_event( ph_ptr, CM_PH_EVENT_ANSWER_VOICE );
    }
  }
  #endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are waiting too long on a reply from MC, end the
  ** waiting period by forcing a MC reply with an error
  ** code indicating on a MC communication problems.
  */
  if( cm_reply_too_long(&ph_ptr->reply) )
  {
    /* Point at client command waiting for reply
    */
    cm_cmd_s_type  *cmd_ptr = cm_reply_get( &ph_ptr->reply );

    CM_ASSERT( cmd_ptr != NULL );

      /* Check if this report is specific to IP app deregistration
      ** and call forward_mm_ll() followed by mmll_reply()
      */
      #if defined(FEATURE_IP_CALL)

      if (ph_ptr->reply.cm_mc_rpt == CM_IP_APP_ALLAPPS_OPRT_CNF)
      {
        /* Since timeout is kicking in before all
        ** apps have responded, clear transaction
        ** ids
        */
        cmipapp_reset_trans_id ();


        /*
        ** network_sel_mode_pref_changed does not mean
        ** anything for cmds other than SYS_SEL_PREF.
        ** Defaulting it to TRUE
        */
        if (cmph_client_cmd_forward_mmll (ph_ptr, cmd_ptr, TRUE))
        {
          CM_ERR ("Wait for reply is TRUE", 0, 0, 0);
        }
      }

      #endif

      /* Invoke the reply function with the appropriate error code.
      */
      cmph_client_cmd_mmll_reply( ph_ptr,
                                cmd_ptr,
                                CM_PH_CMD_ERR_MC_COM_L );

      /* We are done with processing the command that was waiting
      ** for reply - deallocate its command buffer.
      */
      cm_cmd_dealloc( cmd_ptr );

      /* reset the reply info */
      cm_reply_init( &ph_ptr->reply );

      /* Turn on the signal that cause us to process any pending
      ** clients commands. This is necessary since we were NOT
      ** processing any clients commands while waiting on a reply
      ** from MC.
      */
      (void) rex_set_sigs( rex_self(), CM_CLIENT_CMD_Q_SIG );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are waiting on a reply from MC, do NOT continue with the
  ** rest of this function. This is important because MC might not
  ** be able to process commands from CM at the moment.
  */
  if( cm_reply_is_waiting(&ph_ptr->reply) )
  {
    return;
  }

  /* If origination mode is still forced and it is time to consider unforcing
  ** the origination mode, do so.
  */

  /* Search all the origination mode queues and unforce the modes
  ** That needs to be unforced
  */

  cmph_check_and_unforce_orig_mode( CM_SS_MAIN );
  cmph_check_and_unforce_orig_mode( CM_SS_HDR  );
  cmph_check_and_unforce_orig_mode( CM_SS_WLAN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If mode term is temporary, check whether we need to unforce
  ** the mode preference back to its persistent setting.
  */
  if( ph_ptr->pref_term == CM_PREF_TERM_1_CALL_OR_TIME ||
      ph_ptr->pref_term == CM_PREF_TERM_CM_1_CALL_PLUS )
  {

    /* If mode duration expired and phone is not in-use,
    ** unforce the mode preference back to its persistent setting.
    */
    if( uptime >= ph_ptr->mode_uptime && ! ph_ptr->is_in_use )
    {
      /* Remember the current mode term.
      */
      cm_pref_term_e_type  pref_term = ph_ptr->pref_term;

      /* Unforce the mode back to it persistent setting,
      */
      (void) cmph_unforce_mode_on_the_fly( ph_ptr );

      /* If we unforce the mode as a result of a client explicitly
      ** forcing the mode earlier, notify clients of event.
      */
      if( pref_term == CM_PREF_TERM_1_CALL_OR_TIME )
      {
        cmph_event( ph_ptr, CM_PH_EVENT_SYS_SEL_PREF );
      }
    }
  } /* if */

} /* cmph_timer_proc() */

/*lint +esym(715,timer_event) */



/*===========================================================================

FUNCTION cmph_get_curr_nam

DESCRIPTION
  Get the current NAM.

DEPENDENCIES
  Phone object must be initialized.

RETURNS
  The current NAM. If phone was not yet initialized, return CM_NAM_NONE.

SIDE EFFECTS
  None

===========================================================================*/
cm_nam_e_type  cmph_get_curr_nam( void )
{
  cmph_s_type    *ph_ptr  =  cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure object was properly initialized.
  */
  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( CM_INIT_CHECK(ph_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( CM_INIT_CHECK(ph_ptr->init_mask) )
  {
    return ph_ptr->curr_nam;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return CM_NAM_NONE;

} /* cmph_get_curr_nam() */



/*===========================================================================

FUNCTION cmph_autonam_is_enabled

DESCRIPTION
  Check whether auto-NAM is currently enabled.

DEPENDENCIES
  Phone object must be initialized

RETURNS
  TRUE if auto NAM is enabled. FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmph_autonam_is_enabled( void )
{
  cmph_s_type    *ph_ptr  =  cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure object was properly initialized.
  */
  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( CM_INIT_CHECK(ph_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If phone object was initialized and nam_sel = CM_NAM_AUTO, return TRUE.
  ** Else, return FALSE.
  */
  if( CM_INIT_CHECK(ph_ptr->init_mask) &&
      ph_ptr->nam_sel == CM_NAM_AUTO )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* cmph_autonam_is_enabled() */



/*===========================================================================
FUNCTION CMPH_OFFLINE

DESCRIPTION
  This function performs offline processing for the phone object

DEPENDENCIES
  Phone object must be initialized

RETURNS
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_offline (void)
{
  cmlog_set_cm_offline_signal_recvd();
} /* cmph_offline() */


/*===========================================================================

FUNCTION cmph_orig_mode_reset

DESCRIPTION
  Reset the origination mode to none.
  Exit emergency callback mode.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmph_orig_mode_reset(

    cmph_s_type             *ph_ptr,
        /* Pointer to a phone object */

    boolean                  force_top
        /* Force the top preferences */
)
{

  CM_ASSERT( ph_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Exit emergency callback mode
  */
  cmph_exit_emergency_cb(ph_ptr);

  cmph_orig_mode_reset_on_ss( ph_ptr, CM_SS_MAIN, force_top );
  cmph_orig_mode_reset_on_ss( ph_ptr, CM_SS_HDR, force_top );
  cmph_orig_mode_reset_on_ss( ph_ptr, CM_SS_WLAN, force_top );


} /* cmph_orig_mode_reset() */



/*===========================================================================

FUNCTION cmph_map_cm_ss_type_to_sd_ss_type

DESCRIPTION
  Maps cm_ss_e_type to sd_ss_e_type

DEPENDENCIES
  None

RETURN VALUE
  The sd_ss_e_type

SIDE EFFECTS
  None.

NOTE:

===========================================================================*/

sd_ss_e_type cmph_map_cm_ss_type_to_sd_ss_type(

  cm_ss_e_type cm_ss
    /* cm_ss_e_type */
)
{
  switch( cm_ss )
  {
    case CM_SS_MAIN:
      return( SD_SS_MAIN );

    case CM_SS_HDR:
      return( SD_SS_HYBR_HDR );

    case CM_SS_WLAN:
      return( SD_SS_HYBR_WLAN );

    case CM_SS_MAX:
      return( SD_SS_MAX );

    default:
      CM_SWITCH_ERR( "ss %d", cm_ss, 0, 0 );
      /*
      ** 527: Unreachable
      ** This is because CM_SWITCH_ERR() evaluates to error fatal
      ** (i.e exit program)
      ** But for different feature set it may not to, so we want to
      ** supress the warning here
      */
      /*lint -e527 */
      return( SD_SS_MAIN );
  }
} /* cmph_map_cm_ss_type_to_sd_ss_type */



/*===========================================================================

FUNCTION cmph_map_sd_ss_type_to_cm_ss_type

DESCRIPTION
  Maps sd_ss_e_type to cm_ss_e_type

DEPENDENCIES
  None

RETURN VALUE
  The cm_ss_e_type

SIDE EFFECTS
  None.

NOTE:

===========================================================================*/

cm_ss_e_type cmph_map_sd_ss_type_to_cm_ss_type(

  sd_ss_e_type sd_ss

)
{
  switch( sd_ss )
  {
    case SD_SS_MAIN:
      return( CM_SS_MAIN );

    case SD_SS_HYBR_HDR:
      return( CM_SS_HDR );

    case SD_SS_HYBR_WLAN:
      return( CM_SS_WLAN );

    case SD_SS_MAX:
      return( CM_SS_MAX );

    default:
      CM_SWITCH_ERR( "ss %d", sd_ss, 0, 0 );
      /*
      ** 527: Unreachable
      ** This is because CM_SWITCH_ERR() evaluates to error fatal
      ** (i.e exit program)
      ** But for different feature set it may not to, so we want to
      ** supress the warning here
      */
      /*lint -e527 */
      return( CM_SS_NONE );
  }
} /* cmph_map_sd_ss_type_to_cm_ss_type */


/*===========================================================================

FUNCTION cmph_map_cm_prl_pref_to_sd_prl_pref_e_type

DESCRIPTION
  Maps cm_prl_pref to sd_ss_prl_pref_e_type

DEPENDENCIES
  None

RETURN VALUE
  The sd_ss_prl_pref_e_type

SIDE EFFECTS
  None.

NOTE:

===========================================================================*/

sd_ss_prl_pref_e_type cmph_map_cm_prl_pref_to_sd_prl_pref(

  cm_prl_pref_e_type cm_prl_pref
    /* cm prl pref */
)
{

  switch( cm_prl_pref )
  {
    case CM_PRL_PREF_AVAIL_BC0_A:
      return( SD_SS_PRL_PREF_AVAIL_BC0_A );

    case CM_PRL_PREF_AVAIL_BC0_B:
      return( SD_SS_PRL_PREF_AVAIL_BC0_B );

    case CM_PRL_PREF_ANY:
      return( SD_SS_PRL_PREF_ANY );

    default:
      CM_SWITCH_ERR( "cm_prl_pref %d", cm_prl_pref, 0, 0 );
      /*
      ** 527: Unreachable
      ** This is because CM_SWITCH_ERR() evaluates to error fatal
      ** (i.e exit program)
      ** But for different feature set it may not to, so we want to
      ** supress the warning here
      */
      /*lint -e527 */
      return( SD_SS_PRL_PREF_NONE );
  }

} /* cmph_map_cm_prl_pref_to_sd_prl_pref */


/*===========================================================================

FUNCTION cmph_map_sd_prl_pref_to_cm_prl_pref

DESCRIPTION
  Maps sd_ss_prl_pref_e_type to cm_prl_pref_e_type

DEPENDENCIES
  None

RETURN VALUE
  The sd_ss_prl_pref_e_type

SIDE EFFECTS
  None.

NOTE:

===========================================================================*/

cm_prl_pref_e_type cmph_map_sd_prl_pref_to_cm_prl_pref(

  sd_ss_prl_pref_e_type sd_prl_pref
    /* sd prl pref */
)
{

  switch( sd_prl_pref )
  {
    case SD_SS_PRL_PREF_AVAIL_BC0_A :
      return( CM_PRL_PREF_AVAIL_BC0_A );

    case SD_SS_PRL_PREF_AVAIL_BC0_B :
      return( CM_PRL_PREF_AVAIL_BC0_B );

    case SD_SS_PRL_PREF_ANY :
      return( CM_PRL_PREF_ANY );

    default:
      CM_SWITCH_ERR( "sd_ss_prl_pref %d", sd_prl_pref, 0, 0 );
      /*
      ** 527: Unreachable
      ** This is because CM_SWITCH_ERR() evaluates to error fatal
      ** (i.e exit program)
      ** But for different feature set it may not to, so we want to
      ** supress the warning here
      */
      /*lint -e527 */
      return( CM_PRL_PREF_NONE );
  }

} /* cmph_map_sd_prl_pref_to_cm_prl_pref */


/*===========================================================================

FUNCTION cmph_map_cm_roam_pref_to_sd_roam_pref

DESCRIPTION
  Maps cm_roam_pref to sd_roam_pref

DEPENDENCIES
  None

RETURN VALUE
  The sd_ss_roam_pref_e_type

SIDE EFFECTS
  None.

NOTE:

===========================================================================*/

sd_ss_roam_pref_e_type cmph_map_cm_roam_pref_to_sd_roam_pref(

  cm_roam_pref_e_type cm_roam_pref
    /* cm_roam_pref */
)
{

  switch( cm_roam_pref )
  {
    case CM_ROAM_PREF_HOME:
      return( SD_SS_ROAM_PREF_HOME );

    case CM_ROAM_PREF_ROAM_ONLY:
      return(SD_SS_ROAM_PREF_ROAM_ONLY);

    case CM_ROAM_PREF_AFFIL:
      return( SD_SS_ROAM_PREF_AFFIL );

    case CM_ROAM_PREF_ANY:
      return( SD_SS_ROAM_PREF_ANY );

    default:
      CM_SWITCH_ERR( "cm_roam_pref %d", cm_roam_pref, 0, 0 );
      /*
      ** 527: Unreachable
      ** This is because CM_SWITCH_ERR() evaluates to error fatal
      ** (i.e exit program)
      ** But for different feature set it may not to, so we want to
      ** supress the warning here
      */
      /*lint -e527 */
      return( SD_SS_ROAM_PREF_AFFIL );
  }

} /* cmph_map_cm_roam_pref_to_sd_roam_pref */


/*===========================================================================

FUNCTION cmph_map_sd_roam_pref_to_cm_roam_pref

DESCRIPTION
  Maps sd_ss_roam_pref_e_type to cm_roam_pref

DEPENDENCIES
  None

RETURN VALUE
  The cm_roam_pref

SIDE EFFECTS
  None.

NOTE:

===========================================================================*/

cm_roam_pref_e_type cmph_map_sd_roam_pref_to_cm_roam_pref(

   sd_ss_roam_pref_e_type  sd_roam_pref
    /* sd_roam_pref */
)
{

  switch( sd_roam_pref )
  {
    case SD_SS_ROAM_PREF_HOME:
      return( CM_ROAM_PREF_HOME );

    case SD_SS_ROAM_PREF_ROAM_ONLY:
      return (CM_ROAM_PREF_ROAM_ONLY);

    case SD_SS_ROAM_PREF_AFFIL:
      return( CM_ROAM_PREF_AFFIL );

    case SD_SS_ROAM_PREF_ANY:
      return( CM_ROAM_PREF_ANY );

    default:
      CM_SWITCH_ERR( "sd_ss_roam_pref %d", sd_roam_pref, 0, 0 );
      /*
      ** 527: Unreachable
      ** This is because CM_SWITCH_ERR() evaluates to error fatal
      ** (i.e exit program)
      ** But for different feature set it may not to, so we want to
      ** supress the warning here
      */
      /*lint -e527 */
      return( CM_ROAM_PREF_NONE );
  }

} /* cmph_map_sd_roam_pref_to_cm_roam_pref */


/*===========================================================================

FUNCTION cmph_map_act_type_to_call_type

DESCRIPTION
  Maps the act_type to call type

DEPENDENCIES
  None

RETURN VALUE
  The call type cm_call_type_e_type

SIDE EFFECTS
  None.

NOTE:

act_type CM_ACT_TYPE_DATA_CALL maps to call_type CM_CALL_TYPE_PS_DATA
act_type CM_ACT_TYPE_DBM  maps to call_type CM_CALL_TYPE_SMS

===========================================================================*/

static cm_call_type_e_type cmph_map_act_type_to_call_type(

  cm_act_type_e_type act_type
    /* Act Type */
)
{

  switch(act_type)
  {

    case  CM_ACT_TYPE_VOICE_CALL:
      return( CM_CALL_TYPE_VOICE );

    case CM_ACT_TYPE_DATA_CALL:
    case CM_ACT_TYPE_GPS:
      return( CM_CALL_TYPE_PS_DATA );

    case CM_ACT_TYPE_SMS_CALL:
    case CM_ACT_TYPE_DBM:
      return(  CM_CALL_TYPE_SMS );

    case CM_ACT_TYPE_OTAPA_CALL :
      return( CM_CALL_TYPE_OTAPA );

    case CM_ACT_TYPE_STD_OTASP:
      return( CM_CALL_TYPE_STD_OTASP );

    case CM_ACT_TYPE_NON_STD_OTASP:
      return( CM_CALL_TYPE_NON_STD_OTASP );

    case CM_ACT_TYPE_EMERG_CALL:
      return( CM_CALL_TYPE_EMERGENCY );

    case CM_ACT_TYPE_TEST_CALL:
      return( CM_CALL_TYPE_TEST );

    case CM_ACT_TYPE_VT_CALL:
      return( CM_CALL_TYPE_VT );

    case CM_ACT_TYPE_VT_LOOPBACK:
      return( CM_CALL_TYPE_VT_LOOPBACK );

    case CM_ACT_TYPE_PD_CALL:
      return( CM_CALL_TYPE_PD );

    case CM_ACT_TYPE_DEFAULT_CALL:
      return( CM_CALL_TYPE_VOICE );

    case CM_ACT_TYPE_NONE:
    default:
      return( CM_CALL_TYPE_NONE );

  }/* switch(call_type) */

} /* cmph_map_act_type_to_call_type */

/*===========================================================================

FUNCTION cmph_map_call_type_to_act_type

DESCRIPTION
  Maps the call_type to activity type

DEPENDENCIES
  None

RETURN VALUE
  The activity type cm_act_type_e_type

SIDE EFFECTS
  None.

===========================================================================*/

cm_act_type_e_type cmph_map_call_type_to_act_type(

  cm_call_type_e_type call_type
    /* Call Type */

)
{

  switch(call_type)
  {

    case CM_CALL_TYPE_VOICE:
      return( CM_ACT_TYPE_VOICE_CALL );

    case CM_CALL_TYPE_CS_DATA:
    case CM_CALL_TYPE_PS_DATA:
    /* Videoshare call is very similar to VideoTelephony
    ** call and so grouping it with CALL_TYPE_CS_DATA
    */
    case CM_CALL_TYPE_VS:
      return( CM_ACT_TYPE_DATA_CALL );

    case CM_CALL_TYPE_SMS:
      return( CM_ACT_TYPE_SMS_CALL );

    case CM_CALL_TYPE_OTAPA:
      return( CM_ACT_TYPE_OTAPA_CALL );

    case CM_CALL_TYPE_STD_OTASP:
      return( CM_ACT_TYPE_STD_OTASP );

    case CM_CALL_TYPE_NON_STD_OTASP:
      return( CM_ACT_TYPE_NON_STD_OTASP );

    case CM_CALL_TYPE_EMERGENCY:
      return( CM_ACT_TYPE_EMERG_CALL );

    case CM_CALL_TYPE_TEST:
      return (CM_ACT_TYPE_TEST_CALL);

    case CM_CALL_TYPE_VT:
      return (CM_ACT_TYPE_VT_CALL);

    case CM_CALL_TYPE_VT_LOOPBACK:
      return (CM_ACT_TYPE_VT_LOOPBACK);

    case CM_CALL_TYPE_PD:
      return (CM_ACT_TYPE_PD_CALL);

    case CM_CALL_TYPE_SUPS:
    default:
      return( CM_ACT_TYPE_DEFAULT_CALL );

  }/* switch(call_type) */
} /* cmph_map_call_type_to_act_type */


/*===========================================================================

FUNCTION cmph_get_act_priority

DESCRIPTION
  Maps the act_type to act_priority

DEPENDENCIES
  None

RETURN VALUE
  The activity type cm_act_type_e_type

SIDE EFFECTS
  None.

===========================================================================*/

cm_act_priority_e_type cmph_get_act_priority(

  cm_act_type_e_type act_type
    /* Activity Type */

)
{
  switch(act_type)
  {

    case CM_ACT_TYPE_EMERG_CALL:
      return( CM_ACT_PRIORITY_0 );

    case CM_ACT_TYPE_EMERG_CB_MODE:
      return( CM_ACT_PRIORITY_10 );

    case CM_ACT_TYPE_TEST_CALL:
      return( CM_ACT_PRIORITY_20 );

    case CM_ACT_TYPE_VOICE_CALL:
      return( CM_ACT_PRIORITY_30 );

    case CM_ACT_TYPE_VT_LOOPBACK:
      return( CM_ACT_PRIORITY_40 );

    case CM_ACT_TYPE_VT_CALL:
      return( CM_ACT_PRIORITY_50 );

    case CM_ACT_TYPE_PD_CALL:
      return( CM_ACT_PRIORITY_60 );

    case CM_ACT_TYPE_SMS_CALL:
      return( CM_ACT_PRIORITY_70 );

    case CM_ACT_TYPE_DATA_CALL:
      return( CM_ACT_PRIORITY_80 );

    case CM_ACT_TYPE_OTAPA_CALL:
      return( CM_ACT_PRIORITY_90 );

    case CM_ACT_TYPE_STD_OTASP:
    case CM_ACT_TYPE_NON_STD_OTASP:
      return( CM_ACT_PRIORITY_100 );

    case CM_ACT_TYPE_DBM:
      return( CM_ACT_PRIORITY_110 );

    case CM_ACT_TYPE_GPS:
      return( CM_ACT_PRIORITY_120 );

    case CM_ACT_TYPE_DEFAULT_CALL:
      return( CM_ACT_PRIORITY_200 );

    case CM_ACT_TYPE_NONE:
    case CM_ACT_TYPE_PH_OBJ:
      return( CM_ACT_PRIORITY_PH );

    default:
      return( CM_ACT_PRIORITY_MAX );

  }/* switch(call_type) */

} /* cmph_map_act_type_act_priority */

/*===========================================================================

FUNCTION cmph_cmp_act_priorities

DESCRIPTION
  Compares the act_priorities

DEPENDENCIES


RETURN VALUE
  -ve if act_priority_1 <  act_priority_2
   0  if act_priority_1 == act_priority_2
  +ve if act_priority_1 >  act_priority_2

SIDE EFFECTS
  None.

===========================================================================*/

int cmph_cmp_act_priorities(

  cm_act_priority_e_type act_priority_1,
    /* 1st activity's priority */

  cm_act_priority_e_type act_priority_2
    /* 2nd activity's priority */
)
{
  return( act_priority_2 - act_priority_1 ); /*lint !e641 !e656 */

}/* cmph_cmp_act_priorities */


/*===========================================================================

FUNCTION cmph_insert_orig_mode

DESCRIPTION
  Inserts the origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean cmph_insert_orig_mode(

  cm_ss_e_type              ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  sd_ss_orig_mode_e_type    orig_mode,
      /* Origination Mode */

  cm_act_type_e_type    act_type,
        /* Type of CM activity */

  cm_pref_term_e_type       force_term,
      /* Term for which to force the preferences - one-call, etc. */

  cm_mode_pref_e_type       mode_pref,
      /* Mode preference which to force */

  cm_band_pref_e_type       band_pref,
      /* Band preference which to force */

  cm_prl_pref_e_type        prl_pref,
      /* PRL preference which to force */

  cm_roam_pref_e_type       roam_pref,
      /* Roam preference which to force */

  cm_hybr_pref_e_type       hybr_pref,
      /* hybr preference which to force */

  sd_ss_hybr_pref_e_type    int_hybr_pref,
      /* Hybrid preferences used internally by CM */

  const cm_wlan_pref_s_type      *wlan_pref,
      /* WLAN preference */

  cm_srv_domain_pref_e_type srv_domain_pref,
     /* service domain preference to force */

  cm_act_id_type            act_id,
      /* The uniques id of the activity that is forcing the mode */

  cm_act_update_reas_e_type act_update_reas
      /* Reason for the update */

)
{

  cm_orig_q_s_type  *ph_orig_ptr;
  /* Populate the queue structure and insert it in the priority queue
  */

  cm_orig_q_s_type  *ph_orig_top_ptr;

  boolean is_found           = FALSE;
  boolean is_top_ptr_changed = FALSE;

  ph_orig_ptr = cmtask_orig_para_search_act_id( ss == CM_SS_MAX ? CM_SS_MAIN : ss, act_id );
  ph_orig_top_ptr = cmtask_orig_para_get_top( ss == CM_SS_MAX ? CM_SS_MAIN : ss );

  if(ph_orig_ptr == ph_orig_top_ptr)
  {
    is_top_ptr_changed = TRUE;
  }

  if( ph_orig_ptr != NULL )
  {
    is_found = TRUE;
  }

  switch ( ss )
  {
    case CM_SS_MAIN:
    case CM_SS_HDR:
    case CM_SS_WLAN:

      if(!is_found)
      {
        if ((ph_orig_ptr = cmtask_orig_para_alloc(ss)) ==
                                            (cm_orig_q_s_type *)NULL)
        {
           CM_ERR_FATAL("cmtask_orig_para_alloc() %d failed!!",ss,0,0 );
           break;
        }

      }
      else
      {
         /* Delete it for now. We'll re-queue soon */
         (void)cmtask_orig_para_delete( ss, ph_orig_ptr, FALSE );
      }
      /* Insert into the priority Queue
      */
      if( ph_orig_ptr != NULL )
      {
        ph_orig_ptr->act_type                  = act_type;
        ph_orig_ptr->orig.act_id               = act_id;
        ph_orig_ptr->orig.act_priority         = cmph_get_act_priority(  ph_orig_ptr->act_type );
        ph_orig_ptr->orig.orig_mode            = orig_mode;
        ph_orig_ptr->orig.orig_pref_term       = force_term;
        ph_orig_ptr->orig.orig_mode_pref       = mode_pref;
        ph_orig_ptr->orig.orig_band_pref       = band_pref;
        ph_orig_ptr->orig.orig_prl_pref        = prl_pref;
        ph_orig_ptr->orig.orig_roam_pref       = roam_pref;
        ph_orig_ptr->orig.orig_hybr_pref       = hybr_pref;
        ph_orig_ptr->orig.orig_int_hybr_pref   = int_hybr_pref;
        ph_orig_ptr->orig.orig_wlan_pref       = wlan_pref;
        ph_orig_ptr->orig.orig_srv_domain_pref = srv_domain_pref;

        if( act_update_reas != CM_ACT_UPDATE_REAS_ACT_CONT )
        {
          ph_orig_ptr->orig.act_update_reas      = act_update_reas;
        }

        if( act_update_reas != CM_ACT_UPDATE_REAS_ACT_END )
        {
          ph_orig_ptr->orig.orig_mode_uptime = CM_PH_ORIG_MODE_UPTIME_NONE;

        }

        if( force_term == CM_PREF_TERM_CM_MO_SMS )
        {
          ph_orig_ptr->orig.orig_mode_uptime =
                                    clk_uptime() + CMPH_HOLD_MODE_PREF_TIME;

          CM_MSG_LOW("force MO SMS with uptime of %d",
                         ph_orig_ptr->orig.orig_mode_uptime, 0, 0);
        }


        /* Enqueue the orig
        ** onto the orig Q.
        */
        is_top_ptr_changed = cmtask_orig_para_insert( ss, ph_orig_ptr ) || is_top_ptr_changed;
       } /*  if( ph_orig_ptr != NULL ) */

      return( is_top_ptr_changed );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SS_MAX:

      is_found = FALSE;
      is_top_ptr_changed = FALSE;

      ph_orig_ptr = cmtask_orig_para_search_act_id( CM_SS_MAIN, act_id );
      ph_orig_top_ptr = cmtask_orig_para_get_top( CM_SS_MAIN );

      if( ph_orig_ptr == ph_orig_top_ptr )
      {
        is_top_ptr_changed = TRUE;
      }

      if( ph_orig_ptr != NULL )
      {
        is_found = TRUE;
      }


      if(!is_found)
      {
        if ((ph_orig_ptr = cmtask_orig_para_alloc(CM_SS_MAIN)) ==
                                            (cm_orig_q_s_type *)NULL)
        {
           CM_ERR_FATAL("cmtask_orig_para_alloc() SD_SS_MAIN failed!!",0,0,0 );
           break;
        }

      }
      else
      {
         /* Delete it for now. We'll re-queue soon */
         (void)cmtask_orig_para_delete( CM_SS_MAIN, ph_orig_ptr, FALSE );
      }
      if( ph_orig_ptr != NULL )
      {

        /* Insert into the priority Queue
        */
        ph_orig_ptr->act_type                  = act_type;
        ph_orig_ptr->orig.act_id               = act_id;
        ph_orig_ptr->orig.act_priority         =
                   cmph_get_act_priority(  ph_orig_ptr->act_type );
        ph_orig_ptr->orig.orig_mode            = orig_mode;
        ph_orig_ptr->orig.orig_pref_term       = force_term;
        ph_orig_ptr->orig.orig_mode_pref       = mode_pref;
        ph_orig_ptr->orig.orig_band_pref       = band_pref;
        ph_orig_ptr->orig.orig_prl_pref        = prl_pref;
        ph_orig_ptr->orig.orig_roam_pref       = roam_pref;
        ph_orig_ptr->orig.orig_hybr_pref       = hybr_pref;
        ph_orig_ptr->orig.orig_int_hybr_pref   = int_hybr_pref;
        ph_orig_ptr->orig.orig_wlan_pref       = wlan_pref;
        ph_orig_ptr->orig.orig_srv_domain_pref = srv_domain_pref;

        if( act_update_reas != CM_ACT_UPDATE_REAS_ACT_CONT )
        {
          ph_orig_ptr->orig.act_update_reas    = act_update_reas;
        }

        if( act_update_reas != CM_ACT_UPDATE_REAS_ACT_END )
        {
          ph_orig_ptr->orig.orig_mode_uptime = CM_PH_ORIG_MODE_UPTIME_NONE;

        }

        if( force_term == CM_PREF_TERM_CM_MO_SMS )
        {
          ph_orig_ptr->orig.orig_mode_uptime =
                                     clk_uptime() + CMPH_HOLD_MODE_PREF_TIME;

          CM_MSG_LOW("force MO SMS with uptime of %d",
                         ph_orig_ptr->orig.orig_mode_uptime, 0, 0);
        }

        /* Enqueue the orig
        ** onto the orig Q.
        */
        is_top_ptr_changed = cmtask_orig_para_insert( CM_SS_MAIN, ph_orig_ptr ) || is_top_ptr_changed;


      } /* if( ph_orig_ptr != NULL ) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      is_found = FALSE;

      ph_orig_ptr = cmtask_orig_para_search_act_id( CM_SS_HDR, act_id );
      ph_orig_top_ptr = cmtask_orig_para_get_top( CM_SS_HDR );

      if( ph_orig_ptr == ph_orig_top_ptr )
      {
        is_top_ptr_changed = TRUE;
      }

      if( ph_orig_ptr != NULL )
      {
        is_found = TRUE;
      }

      if(!is_found)
      {
        if ((ph_orig_ptr = cmtask_orig_para_alloc(CM_SS_HDR)) ==
                                            (cm_orig_q_s_type *)NULL)
        {
           CM_ERR_FATAL("cmtask_orig_para_alloc() CM_SS_HDR failed!!",0,0,0 );
           break;
        }

      }
      else
      {
         /* Delete it for now. We'll re-queue soon */
         (void)cmtask_orig_para_delete( CM_SS_HDR, ph_orig_ptr, FALSE );
      }

      if( ph_orig_ptr != NULL )
      {

        /* Insert into the priority Queue
        */
        ph_orig_ptr->act_type                  = act_type;
        ph_orig_ptr->orig.act_id               = act_id;
        ph_orig_ptr->orig.act_priority         =
                   cmph_get_act_priority(  ph_orig_ptr->act_type );
        ph_orig_ptr->orig.orig_mode            = orig_mode;
        ph_orig_ptr->orig.orig_pref_term       = force_term;
        ph_orig_ptr->orig.orig_mode_pref       = mode_pref;
        ph_orig_ptr->orig.orig_band_pref       = band_pref;
        ph_orig_ptr->orig.orig_prl_pref        = prl_pref;
        ph_orig_ptr->orig.orig_roam_pref       = roam_pref;
        ph_orig_ptr->orig.orig_hybr_pref       = hybr_pref;
        ph_orig_ptr->orig.orig_int_hybr_pref   = int_hybr_pref;
        ph_orig_ptr->orig.orig_wlan_pref       = wlan_pref;
        ph_orig_ptr->orig.orig_srv_domain_pref = srv_domain_pref;

        if( act_update_reas != CM_ACT_UPDATE_REAS_ACT_CONT )
        {
          ph_orig_ptr->orig.act_update_reas      = act_update_reas;
        }

        if( act_update_reas != CM_ACT_UPDATE_REAS_ACT_END )
        {
          ph_orig_ptr->orig.orig_mode_uptime = CM_PH_ORIG_MODE_UPTIME_NONE;

        }

        if( force_term == CM_PREF_TERM_CM_MO_SMS )
        {
          ph_orig_ptr->orig.orig_mode_uptime =
                                     clk_uptime() + CMPH_HOLD_MODE_PREF_TIME;

          CM_MSG_LOW("force MO SMS with uptime of %d",
                         ph_orig_ptr->orig.orig_mode_uptime, 0, 0);
        }


        /* Enqueue the orig
        ** onto the orig Q.
        */
        is_top_ptr_changed= cmtask_orig_para_insert( CM_SS_HDR, ph_orig_ptr ) || is_top_ptr_changed;

      } /* if( ph_orig_ptr != NULL ) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      is_found = FALSE;

      ph_orig_ptr = cmtask_orig_para_search_act_id( CM_SS_WLAN, act_id );
      ph_orig_top_ptr = cmtask_orig_para_get_top( CM_SS_WLAN );

      if( ph_orig_ptr == ph_orig_top_ptr )
      {
        is_top_ptr_changed = TRUE;
      }

      if( ph_orig_ptr != NULL )
      {
        is_found = TRUE;
      }

      if(!is_found)
      {
        if ((ph_orig_ptr = cmtask_orig_para_alloc(CM_SS_WLAN)) ==
                                            (cm_orig_q_s_type *)NULL)
        {
           CM_ERR_FATAL("cmtask_orig_para_alloc() CM_SS_HDR failed!!",0,0,0 );
           break;
        }

      }
      else
      {
         /* Delete it for now. We'll re-queue soon */
         (void)cmtask_orig_para_delete( CM_SS_WLAN, ph_orig_ptr, FALSE );
      }


      if( ph_orig_ptr != NULL )
      {

        /* Insert into the priority Queue
        */
        ph_orig_ptr->act_type                  = act_type;
        ph_orig_ptr->orig.act_id               = act_id;
        ph_orig_ptr->orig.act_priority         =
                   cmph_get_act_priority(  ph_orig_ptr->act_type );
        ph_orig_ptr->orig.orig_mode            = orig_mode;
        ph_orig_ptr->orig.orig_pref_term       = force_term;
        ph_orig_ptr->orig.orig_mode_pref       = mode_pref;
        ph_orig_ptr->orig.orig_band_pref       = band_pref;
        ph_orig_ptr->orig.orig_prl_pref        = prl_pref;
        ph_orig_ptr->orig.orig_roam_pref       = roam_pref;
        ph_orig_ptr->orig.orig_hybr_pref       = hybr_pref;
        ph_orig_ptr->orig.orig_int_hybr_pref   = int_hybr_pref;
        ph_orig_ptr->orig.orig_wlan_pref       = wlan_pref;
        ph_orig_ptr->orig.orig_srv_domain_pref = srv_domain_pref;

        if( act_update_reas != CM_ACT_UPDATE_REAS_ACT_CONT )
        {
          ph_orig_ptr->orig.act_update_reas      = act_update_reas;
        }

        if( act_update_reas != CM_ACT_UPDATE_REAS_ACT_END )
        {
          ph_orig_ptr->orig.orig_mode_uptime = CM_PH_ORIG_MODE_UPTIME_NONE;

        }

        if( force_term == CM_PREF_TERM_CM_MO_SMS )
        {
          ph_orig_ptr->orig.orig_mode_uptime =
                                     clk_uptime() + CMPH_HOLD_MODE_PREF_TIME;

          CM_MSG_LOW("force MO SMS with uptime of %d",
                         ph_orig_ptr->orig.orig_mode_uptime, 0, 0);
        }

        /* Enqueue the orig
        ** onto the orig Q.
        */
        is_top_ptr_changed= cmtask_orig_para_insert( CM_SS_WLAN, ph_orig_ptr ) || is_top_ptr_changed;


      } /* if( ph_orig_ptr != NULL ) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      return( is_top_ptr_changed );

    default:
      CM_ERR_FATAL("Incorrect ss %d", ss, 0, 0 );
      return(FALSE); /*lint !e527 */

  } /* switch ( ss ) */

  return(FALSE); /*lint !e527 */

} /* cmph_insert_orig_mode */



/*===========================================================================

FUNCTION cmph_update_orig_param

DESCRIPTION
  Updates the origination parameters in the priority queue.
  Currently only CM_ACT_UPDATE_REAS_END is supported

DEPENDENCIES
 None

RETURN VALUE
  True if the act_id object is found in the queue and updated
  False otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean cmph_update_orig_param(

  cm_act_id_type            act_id,
    /* The id of the activity whose orig_params are to be updated
    */
  cm_ss_e_type              ss,
    /* The ss on which the activity was going on
    */
  cm_act_update_reas_e_type act_update_reas
    /* The reason for updating the activity
    */
)
{

  cm_orig_q_s_type *q_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( BETWEEN( act_update_reas,
                      CM_ACT_UPDATE_REAS_NONE,
                      CM_ACT_UPDATE_REAS_MAX ) );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(act_update_reas)
  {

    case CM_ACT_UPDATE_REAS_ACT_END:

      /* Step 1: Go through the queue and get a pointer to the object of
      **         Interest
      ** Step 2: If the pointer is not NULL, then update the value and
      **         Return true
      ** Step 3: Else return FALSE
      */

      q_ptr = cmtask_orig_para_search_act_id(ss, act_id);

      if(q_ptr == NULL)
      {
        return FALSE;
      }

      q_ptr->orig.act_update_reas = CM_ACT_UPDATE_REAS_ACT_END;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Check whether we need to unforce the system selection preference
      ** that was forced automatically by CM during origination.
      */
      if( q_ptr->orig.orig_mode == SD_SS_ORIG_MODE_NONE )
      {
        /* Set origination mode term to none.
        */
        q_ptr->orig.orig_pref_term  = CM_PREF_TERM_NONE;

      } /* if( q_ptr->orig.orig_mode == SD_SS_ORIG_MODE_NONE ) */
      else
      {

        switch( q_ptr->orig.orig_pref_term )
        {
          case CM_PREF_TERM_CM_1_CALL:
            /* If preference was forced by CM on the fly for the duration
            ** of the one call, unforce the mode now.
            */
            CM_MSG_LOW("cmph_force_orig_mode_on_the_fly() on %d",
                                      q_ptr->orig.orig_pref_term, 0, 0);

            /* Remove this q_ptr from the queue
            */
            if (cmtask_orig_para_delete( ss, q_ptr, TRUE ) )
            {
              cmph_force_orig_mode_on_the_fly(ss);
            }

            break;

          case CM_PREF_TERM_CM_1_CALL_PLUS:
            /* If preference was forced by CM on the fly for the duration
            ** of one call plus, indicate that we should unforce the preference
            ** in CMPH_HOLD_MODE_PREF_TIME seconds.
            **
            ** NOTE that we delay unforcing the mode preference in order to
            ** avoid loading MC with pref sys changes in a situation where
            ** the user is pressing SEND, END, SEND, END, ...
            **
            ** If there are other active calls, preference is forced per
            ** the active call based on the order of priority queue.
            **
            */
            if( cmtask_orig_para_count_2( ss,
                                          CM_ACT_PRIORITY_END,
                                          CM_CMP_CRITERIA_GREATER_THAN) > 1 )
            {
              /* Remove this q_ptr from the queue
              */
              if (cmtask_orig_para_delete( ss, q_ptr, TRUE ) )
              {
                cmph_force_orig_mode_on_the_fly(ss);
              }

            }
            else
            {
              q_ptr->orig.orig_mode_uptime = clk_uptime() +
                                                      CMPH_HOLD_MODE_PREF_TIME;

              if( cmph_cmp_act_priorities(q_ptr->orig.act_priority, CM_ACT_PRIORITY_END ) > 0)
              {
                q_ptr->orig.act_priority   = CM_ACT_PRIORITY_END;
              }
            }
            break;


          #ifdef FEATURE_DBM
          case CM_PREF_TERM_CM_MO_SMS:
            /* do nothing when DBM is ended CMSMS or CMPD will schedule
            ** when to unforce the mode.
            */
            CM_MSG_HIGH("CM MO SMS",0,0,0);
            if( !cmxsms_is_mo_sms_in_progress() &&
                !cmxdbm_is_mo_dbm_in_progress()
              )
            {
              /* If there is no DBM in progress then this is an
              ** error case.
              */
              CM_ERR( "orig_mode_term %d", 0,0,0 );
              CM_MSG_LOW("cmph_force_orig_mode_on_the_fly() on %d",
                                      q_ptr->orig.orig_pref_term, 0, 0);

              /* Remove this q_ptr from the queue
              */
              if (cmtask_orig_para_delete( ss, q_ptr, TRUE ) )
              {
                cmph_force_orig_mode_on_the_fly(ss);
              }

            }
            break;
            #endif

          case CM_PREF_TERM_PWR_CYCLE:
            break;

          case CM_PREF_TERM_NONE:
          case CM_PREF_TERM_PERMANENT:
          case CM_PREF_TERM_1_CALL:
          case CM_PREF_TERM_1_CALL_OR_TIME:
          case CM_PREF_TERM_MAX:
          default:
            CM_CASE_ERR( "orig_pref_term",0,0,0 );
            /*lint -save -e527 Unreachable
            **     CM_CASE_ERR exits when CM_DEBUG is on
            */
            CM_MSG_LOW("cmph_force_orig_mode_on_the_fly() on %d",
                                      q_ptr->orig.orig_pref_term, 0, 0);
            /* Remove this q_ptr from the queue
            */
            if (cmtask_orig_para_delete( ss, q_ptr, TRUE ) )
            {
              cmph_force_orig_mode_on_the_fly(ss);
            }

            /*lint -restore */
            break;

        } /* switch( q_ptr->orig.orig_pref_term ) */

      } /* else ( q_ptr->orig.orig_mode != SD_SS_ORIG_MODE_NONE ) */

    break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_ACT_UPDATE_REAS_ACT_PWR_SAVE:

      /* Step 1: Go through the queue and get a pointer to the object of
      **         Interest
      ** Step 2: If the pointer is not NULL, then update the value and
      **         Return true
      ** Step 3: Else return FALSE
      */

      q_ptr = cmtask_orig_para_search_act_id(ss, act_id);

      if(q_ptr == NULL)
      {
        return FALSE;
      }
      q_ptr->orig.orig_mode_pref = CM_MODE_PREF_NONE;
      q_ptr->orig.act_update_reas = CM_ACT_UPDATE_REAS_ACT_PWR_SAVE;
      cmph_force_orig_mode_on_the_fly( ss );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      return FALSE;

  } /* switch(act_update_reas) */

  return( TRUE );

} /* cmph_update_orig_param */


/*===========================================================================

FUNCTION cmph_update_orig_para_incoming_call

DESCRIPTION
  Updates the origination parameters, for the incoming call.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init(), the orig queues must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

boolean cmph_update_orig_param_incoming_call
(
  const cmcall_s_type *call_ptr
    /* Pointer to the call object
    */
)
{

  /* Step 0: Check if we already have this on the Q
  ** Step 1: Get the top pointer, for this ss
  ** Step 2: Create a new entry with the current call type
  ** Step 3: Push the parameters on the stack
  */

  cm_orig_q_s_type *q_top_ptr      = NULL;
  cm_orig_q_s_type *ph_orig_ptr    = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( call_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( cmtask_orig_para_search_act_id( call_ptr->ss, call_ptr->call_id ) != NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((ph_orig_ptr = cmtask_orig_para_alloc(call_ptr->ss)) == (cm_orig_q_s_type *)NULL)
  {
    CM_ERR_FATAL("cmtask_orig_para_alloc() %d failed!!",call_ptr->ss,0,0 );
    return FALSE; /*lint !e527 */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Irrespective of the top pointer, these fields are set to the
  ** current call parameters
  */

  ph_orig_ptr->act_type              =
                          cmph_map_call_type_to_act_type( call_ptr->call_type );
  ph_orig_ptr->orig.act_id           = call_ptr->call_id;
  ph_orig_ptr->orig.act_priority     =
                             cmph_get_act_priority(  ph_orig_ptr->act_type );

  ph_orig_ptr->orig.orig_mode_uptime = CM_PH_ORIG_MODE_UPTIME_NONE;
  ph_orig_ptr->orig.act_update_reas  = CM_ACT_UPDATE_REAS_ACT_START;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  q_top_ptr = cmtask_orig_para_get_top( call_ptr->ss );

  if( q_top_ptr != NULL && q_top_ptr->orig.act_id != (cm_act_id_type)cmph_ptr() )
  {

    /* Use the top's origination parameters
    */
    ph_orig_ptr->orig.orig_mode            = q_top_ptr->orig.orig_mode;
    ph_orig_ptr->orig.orig_pref_term       = CM_PREF_TERM_CM_1_CALL_PLUS;
    ph_orig_ptr->orig.orig_mode_pref       = q_top_ptr->orig.orig_mode_pref;
    ph_orig_ptr->orig.orig_band_pref       = q_top_ptr->orig.orig_band_pref;
    ph_orig_ptr->orig.orig_prl_pref        = q_top_ptr->orig.orig_prl_pref;
    ph_orig_ptr->orig.orig_roam_pref       = q_top_ptr->orig.orig_roam_pref;
    ph_orig_ptr->orig.orig_hybr_pref       = q_top_ptr->orig.orig_hybr_pref;
    ph_orig_ptr->orig.orig_wlan_pref       = q_top_ptr->orig.orig_wlan_pref;
    ph_orig_ptr->orig.orig_int_hybr_pref   = q_top_ptr->orig.orig_int_hybr_pref;

    /* set srv_domain_pref based on call_type
    ** setting it to top's srv_domain_pref can be wrong (eg:- if top is PS_ONLY
    ** and the call under consideration is VOICE)
    */
    ph_orig_ptr->orig.orig_srv_domain_pref = cmcall_get_favored_srv_domain_pref(
                                             call_ptr->call_type);


  }
  else
  {
    /* Use the ph objects parameters
    ** pref_term is one call
    */
    cmph_s_type *ph_ptr = cmph_ptr();

    CM_ASSERT( ph_ptr != NULL );

    ph_orig_ptr->orig.orig_mode            =
                cmph_map_cm_call_type_to_sd_orig_mode( call_ptr->call_type );
    ph_orig_ptr->orig.orig_pref_term       = CM_PREF_TERM_CM_1_CALL_PLUS;
    ph_orig_ptr->orig.orig_mode_pref       = ph_ptr->mode_pref;
    ph_orig_ptr->orig.orig_band_pref       = ph_ptr->band_pref;
    ph_orig_ptr->orig.orig_prl_pref        = ph_ptr->prl_pref;
    ph_orig_ptr->orig.orig_roam_pref       = ph_ptr->roam_pref;
    ph_orig_ptr->orig.orig_hybr_pref       = ph_ptr->hybr_pref;
    ph_orig_ptr->orig.orig_wlan_pref       = &ph_ptr->wlan_pref;
    ph_orig_ptr->orig.orig_srv_domain_pref = ph_ptr->srv_domain_pref;
    ph_orig_ptr->orig.orig_int_hybr_pref   = ph_ptr->int_hybr_pref;

  }

  /* Enqueue the orig
  ** onto the orig Q.
  */
  return cmtask_orig_para_insert( call_ptr->ss , ph_orig_ptr );

} /* cmph_update_orig_para_incoming_call*/

/*===========================================================================

FUNCTION cmph_add_srv_domain_pref

DESCRIPTION
  Adds the srv_domain_pref

DEPENDENCIES
 CM_SRV_DOMAIN_PREF_PS_ATTACH, CM_SRV_DOMAIN_PREF_PS_DETACH not supported


RETURN VALUE
 The union of the srv_domain_pref

SIDE EFFECTS
  None.

===========================================================================*/
cm_srv_domain_pref_e_type cmph_add_srv_domain_pref(

  cm_srv_domain_pref_e_type orig_srv_domain_pref,
    /* The original srv_domain pref
    */

  cm_srv_domain_pref_e_type srv_domain_pref_to_add
    /* The srv_domain_pref to add
    */
)
{
  switch( srv_domain_pref_to_add )
  {

    case CM_SRV_DOMAIN_PREF_NONE:
    case CM_SRV_DOMAIN_PREF_NO_CHANGE:
      return( orig_srv_domain_pref );

    case CM_SRV_DOMAIN_PREF_PS_ATTACH:
    case CM_SRV_DOMAIN_PREF_PS_DETACH:
      CM_ERR( "CM Invalid type %d cmph_add_srv_domain_pref",
                                              srv_domain_pref_to_add, 0, 0 );
      CM_ASSERT(FALSE);
      return( orig_srv_domain_pref );

    case CM_SRV_DOMAIN_PREF_CS_ONLY:
      if( orig_srv_domain_pref == CM_SRV_DOMAIN_PREF_PS_ONLY ||
          orig_srv_domain_pref == CM_SRV_DOMAIN_PREF_CS_PS )
      {
        return( CM_SRV_DOMAIN_PREF_CS_PS );
      }
      return( CM_SRV_DOMAIN_PREF_CS_ONLY );

    case CM_SRV_DOMAIN_PREF_PS_ONLY:
      if( orig_srv_domain_pref == CM_SRV_DOMAIN_PREF_CS_ONLY ||
          orig_srv_domain_pref == CM_SRV_DOMAIN_PREF_CS_PS )
      {
        return( CM_SRV_DOMAIN_PREF_CS_PS );
      }
      return( CM_SRV_DOMAIN_PREF_PS_ONLY );

    case CM_SRV_DOMAIN_PREF_CS_PS:
      return( CM_SRV_DOMAIN_PREF_CS_PS );

    default:
       return( CM_SRV_DOMAIN_PREF_CS_ONLY );

  }/* switch( srv_domain_pref_to_add )*/

}/* cmph_add_srv_domain_pref */

/*===========================================================================

FUNCTION cmph_force_pref_on_the_fly

DESCRIPTION
  Force the system selection preference on the fly.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
dword cmph_force_pref_on_the_fly(

    cm_ss_e_type                     ss,
         /* SS to use for origination, if hybrid operation is changed,
         ** then ss= CM_SS_MAX.
         */

    cmph_s_type                     *ph_ptr,
        /* Pointer to a phone object */

    sd_ss_pref_reas_e_type           force_reason,
        /* Reason for forcing the SS-preference */

    cm_act_type_e_type               act_type,
        /* Call type for which to force the preferences */

    cm_pref_term_e_type              force_term,
        /* Term for which to force the preferences - one-call, etc. */

    cm_mode_pref_e_type              mode_pref,
        /* Mode preference which to force */

    cm_band_pref_e_type              band_pref,
        /* Band preference which to force */

    cm_prl_pref_e_type               prl_pref,
        /* PRL preference which to force */

    cm_roam_pref_e_type              roam_pref,
        /* Roam preference which to force */

    cm_hybr_pref_e_type              hybr_pref,
        /* hybr preference which to force */

    sys_plmn_id_s_type               plmn_id,
       /* The PLMN id to be acquired, valid when the net_sel_mode_pref
       ** is MANUAL.
       ** Applies only for GSM/WCDMA modes.
       */

    cm_srv_domain_pref_e_type        srv_domain_pref,
       /* service domain preference to force */

    cm_activate_code_e_type          otasp_act_code,
        /* OTASP call activation code */

    const cm_wlan_pref_s_type       *wlan_pref,
        /* WLAN preference */

    cm_gw_acq_order_pref_e_type      acq_order_pref,
      /* The current acquisition order preference setting */

    cm_network_sel_mode_pref_e_type  network_sel_mode_pref,
      /* The current network sel mode */

    cm_act_id_type                   act_id,
        /* The uniques id of the activity that is forcing the mode */

    cm_act_update_reas_e_type        act_update_reas,
        /* Reason for the update */

    boolean                          force_top_pref
        /* Force the top ptreven if the top didn't change */

)
{
  unsigned int            rescan_time = 0;
    /* Ballpark estimation for the number of seconds it takes
    ** the phone to reacquire service
    */

  sd_ss_orig_mode_e_type  orig_mode;
    /* Origination mode
    */

  cm_orig_q_s_type *ph_orig_top_ptr;
    /* The pointer to the top element of the priority queue
    */

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif

  boolean is_top_ptr_changed      = FALSE;
  boolean is_main_top_ptr_changed = FALSE;
  boolean is_hybr_1_top_ptr_changed = FALSE;
  boolean is_wlan_top_ptr_changed = FALSE;
  sd_ss_hybr_pref_e_type int_hybr_pref   = SD_SS_HYBR_PREF_NONE;

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( wlan_pref != NULL );

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  CM_ASSERT( force_reason < SD_SS_PREF_REAS_MAX );
  #endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

  CM_ASSERT( BETWEEN(act_type, CM_ACT_TYPE_NONE, CM_ACT_TYPE_MAX) ||
                     act_type == CM_ACT_TYPE_NONE );

  CM_ASSERT( BETWEEN(force_term, CM_PREF_TERM_NONE, CM_PREF_TERM_MAX) );
  CM_ASSERT( BETWEEN(mode_pref,  CM_MODE_PREF_NONE, CM_MODE_PREF_MAX) );
  CM_ASSERT(band_pref != CM_BAND_PREF_NONE );
  CM_ASSERT(band_pref != CM_BAND_PREF_MAX );

  CM_ASSERT( BETWEEN(prl_pref,   CM_PRL_PREF_NONE,  CM_PRL_PREF_MAX) );
  CM_ASSERT( BETWEEN(roam_pref,  CM_ROAM_PREF_NONE, CM_ROAM_PREF_MAX) );
  CM_ASSERT( BETWEEN(hybr_pref,  CM_HYBR_PREF_NONE, CM_HYBR_PREF_MAX) );
  CM_ASSERT( INRANGE(srv_domain_pref, CM_SRV_DOMAIN_PREF_NONE,
                                      (CM_SRV_DOMAIN_PREF_MAX-1)));
  CM_ASSERT( INRANGE(otasp_act_code,  0, CM_OTASP_ACT_CODE_NONE) );

  CM_FUNC_START( "cmph_force_pref_on_the_fly()",0,0,0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Map the call type to system determination's origination mode.
  */
  if( network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_NONE )
  {
    orig_mode = cmph_map_cm_call_type_to_sd_orig_mode( cmph_map_act_type_to_call_type(act_type) );
  }
  else
  {
    orig_mode = cmph_map_cm_network_sel_mode_pref_to_orig_mode(
                                                    network_sel_mode_pref );
  }

  CM_MSG_LOW("orig_mode (computed new) = %d",orig_mode,0,0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If there is a activity and we are setting the mode to none or manual_idle,
  ** update the orig mode accordingly
  */

  ph_orig_top_ptr = cmtask_orig_para_get_top( CM_SS_MAIN );

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif

  if ( (act_update_reas ==  CM_ACT_UPDATE_REAS_USER  ) &&
     ( (
          ( ( ph_orig_top_ptr != NULL   &&
              ph_orig_top_ptr->orig.orig_mode != SD_SS_ORIG_MODE_NONE &&
              ph_orig_top_ptr->orig.orig_mode != SD_SS_ORIG_MODE_MANUAL_IDLE
      )
            ||
            ( cmtask_orig_para_count_2( CM_SS_MAIN,
                                     CM_ACT_PRIORITY_END,
                                     CM_CMP_CRITERIA_GREATER_THAN) >= 1
        )
          )
       )
     #ifdef FEATURE_HYBR_GW
#error code not present
    #endif
       )

   )
  {
    if( orig_mode ==  SD_SS_ORIG_MODE_MANUAL_IDLE )
    {
       orig_mode =  SD_SS_ORIG_MODE_MANUAL_ORIG;
    }
    else
    {
       orig_mode = SD_SS_ORIG_MODE_NORMAL;
    }

  }

  /* If the current origination mode is EMERG_ORIG and there is an active
  ** emergency call, don't overwrite it. Leave it as is, for the case of SVD,
  ** where emergency and data calls can go through at the same time.
  */
  if( ph_orig_top_ptr                 != NULL &&
      ph_orig_top_ptr->orig.orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG &&
      ph_orig_top_ptr->orig.act_priority == CM_ACT_PRIORITY_0 )
  {
    CM_MSG_LOW("orig_mode (keep current) = SD_SS_ORIG_MODE_EMERG_ORIG",0,0,0);
    orig_mode = SD_SS_ORIG_MODE_EMERG_ORIG;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If current origination mode is EMERG_CB and we're originating a call
  ** other than emrgency (for example, PD), set new orig mode to
  ** SD_SS_ORIG_MODE_EMERG_CB
  */
  if( act_type                        != CM_ACT_TYPE_EMERG_CALL &&
      ph_orig_top_ptr                 != NULL                   &&
      ph_orig_top_ptr->orig.orig_mode == SD_SS_ORIG_MODE_EMERG_CB )
  {
    CM_MSG_LOW("orig_mode (in callback) = SD_SS_ORIG_MODE_EMERG_CB",0,0,0);
    orig_mode = SD_SS_ORIG_MODE_EMERG_CB;
  }

  /*
  ** Now figure out whether we need to exit emergency callback mode
  ** or continue in callback but with current preferences
  */
  if(orig_mode != SD_SS_ORIG_MODE_EMERG_CB)
  {

    /*
    ** If orig_mode is anything other than SD_SS_ORIG_MODE_EMERG_CB,
    ** exit emergency callback mode
    */
    cmph_exit_emergency_cb(ph_ptr);
  }

  #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
  #endif  /* FEATURE_WCDMA || FEATURE_GSM */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the caller of this function wants to use existing system selection
  ** preferences, do so.
  */

  /* Top element of MAIN already accessed
  */

  if( ss != CM_SS_MAX )
  {
    ph_orig_top_ptr = cmtask_orig_para_get_top( ss );

  }

  if( ph_orig_top_ptr != NULL &&
      ph_orig_top_ptr->orig.orig_mode == SD_SS_ORIG_MODE_NONE )
  {
    if( mode_pref == CM_MODE_PREF_NO_CHANGE )
    {
      mode_pref = ph_ptr->mode_pref;
    }

    if( band_pref == CM_BAND_PREF_NO_CHANGE )
    {
      band_pref = ph_ptr->band_pref;
    }

    if( prl_pref == CM_PRL_PREF_NO_CHANGE )
    {
      prl_pref = ph_ptr->prl_pref;
    }

    if( roam_pref == CM_ROAM_PREF_NO_CHANGE )
    {
      roam_pref = ph_ptr->roam_pref;
    }

    if( hybr_pref == CM_HYBR_PREF_NO_CHANGE )
    {
      hybr_pref     = ph_ptr->hybr_pref;
      int_hybr_pref = ph_ptr->int_hybr_pref;
    }
    else
    {
      int_hybr_pref =
      cmph_map_cm_hybr_pref_to_sd_hybr_pref( hybr_pref );
    }

    if ( srv_domain_pref == CM_SRV_DOMAIN_PREF_NO_CHANGE )
    {
      srv_domain_pref = ph_ptr->srv_domain_pref;
    }

  }
  else
  {
    if( mode_pref == CM_MODE_PREF_NO_CHANGE )
    {
      mode_pref = ( ph_orig_top_ptr != NULL                   ?
                    ph_orig_top_ptr->orig.orig_mode_pref :
                    ph_ptr->mode_pref );
    }

    if( band_pref == CM_BAND_PREF_NO_CHANGE )
    {
      band_pref = ( ph_orig_top_ptr != NULL                   ?
                    ph_orig_top_ptr->orig.orig_band_pref :
                    ph_ptr->band_pref );
    }

    if( prl_pref == CM_PRL_PREF_NO_CHANGE )
    {
      prl_pref = ( ph_orig_top_ptr != NULL                   ?
                    ph_orig_top_ptr->orig.orig_prl_pref :
                    ph_ptr->prl_pref );
    }

    if( roam_pref == CM_ROAM_PREF_NO_CHANGE )
    {
      roam_pref = ( ph_orig_top_ptr != NULL                   ?
                    ph_orig_top_ptr->orig.orig_roam_pref :
                    ph_ptr->roam_pref );
    }

    if( hybr_pref == CM_HYBR_PREF_NO_CHANGE )
    {
      hybr_pref = ( ph_orig_top_ptr != NULL                   ?
                    ph_orig_top_ptr->orig.orig_hybr_pref :
                    ph_ptr->hybr_pref );

      int_hybr_pref = ( ph_orig_top_ptr != NULL                ?
                        ph_orig_top_ptr->orig.orig_int_hybr_pref :
                        ph_ptr->int_hybr_pref );
    }
    else
    {
      int_hybr_pref = cmph_map_cm_hybr_pref_to_sd_hybr_pref( hybr_pref );
    }

    if ( srv_domain_pref == CM_SRV_DOMAIN_PREF_NO_CHANGE )
    {
      srv_domain_pref = ( ph_orig_top_ptr != NULL                         ?
                          ph_orig_top_ptr->orig.orig_srv_domain_pref :
                          ph_ptr->srv_domain_pref );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If specified mode and band preference combination is not valid for this
  ** target or not supported by the currently loaded PRL, use existing mode
  ** and band preferences.
  */
  if( ! cmph_is_valid_mode_band_pref(mode_pref, band_pref) )
  {
    CM_ERR( "Invalid mode/band pref mp=%d, bp=%d", mode_pref, band_pref, 0 );
    mode_pref = ph_ptr->mode_pref;
    band_pref = ph_ptr->band_pref;
    CM_MSG_HIGH("Switch to use current mp=%d and bp=%d instead", mode_pref, band_pref,0);

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if hybrid is toggled for origination, if it is toggled
  ** as a result of sys_pref_change, then clear the HDR Q and send
  ** the sys_pref down
  */

  /* If system selection preference is forced in order to originate a call
  ** (i.e., the origination mode is other than none), only change the
  ** origination mode preference.
  */
  if( act_update_reas != CM_ACT_UPDATE_REAS_USER &&
      act_update_reas != CM_ACT_UPDATE_REAS_ACT_END &&
      act_update_reas != CM_ACT_UPDATE_REAS_ACT_PWR_SAVE )
  {

    CM_MSG_LOW("Forced to originate a call ...",0,0,0);

    /* Insert the origination parameters in the priority queue
    */

    is_top_ptr_changed = cmph_insert_orig_mode( ss,
                           orig_mode,
                           act_type,
                           force_term,
                           mode_pref,
                           band_pref,
                           prl_pref,
                           roam_pref,
                           hybr_pref,
                           int_hybr_pref,
                           wlan_pref,
                           srv_domain_pref,
                           act_id,
                           act_update_reas
                         );

  }
  /* Else, update the phone state to reflect the new preferences.
  */
  else if( act_update_reas == CM_ACT_UPDATE_REAS_USER ||
         act_update_reas == CM_ACT_UPDATE_REAS_ACT_END )
  {
    CM_MSG_LOW("Forced per user request ...",0,0,0);

    ph_ptr->pref_term       = force_term;
    ph_ptr->mode_pref       = mode_pref;
    ph_ptr->band_pref       = band_pref;
    ph_ptr->prl_pref        = prl_pref;
    ph_ptr->roam_pref       = roam_pref;
    ph_ptr->hybr_pref       = hybr_pref;
    ph_ptr->int_hybr_pref   = int_hybr_pref;
    ph_ptr->srv_domain_pref = srv_domain_pref;
    ph_ptr->wlan_pref       = *wlan_pref;

    /* This is because of a user activity
    ** bump down the priority of all data calls and then insert the
    ** phone object
    */

    (void)cmtask_orig_para_change_act_priority( CM_SS_MAIN,
                                CM_ACT_TYPE_DATA_CALL,
                                CM_ACT_PRIORITY_80,
                                CM_ACT_PRIORITY_BELOW_PH,
                                TRUE );

    (void)cmtask_orig_para_change_act_priority( CM_SS_MAIN,
                                CM_ACT_TYPE_DATA_CALL,
                                CM_ACT_PRIORITY_END,
                                CM_ACT_PRIORITY_BELOW_PH,
                                TRUE );

    #ifdef FEATURE_HYBR_GW
#error code not present
    #endif


    /* If main Q changed then send pref_sys changed to both MAIN and HDR
    ** If only HDR changed, and Hybrid is toggled, then don't send
    ** pref sys changed down
    ** WLAN is independent of MAIN and HDR, send it down only if WLAN
    ** preferences changed
    */

    is_main_top_ptr_changed = cmph_insert_orig_mode (
                                  CM_SS_MAIN,
                                  SD_SS_ORIG_MODE_NONE,
                                  CM_ACT_TYPE_PH_OBJ,
                                  ph_ptr->pref_term,
                                  ph_ptr->mode_pref,
                                  ph_ptr->band_pref,
                                  ph_ptr->prl_pref,
                                  ph_ptr->roam_pref,
                                  ph_ptr->hybr_pref,
                                  ph_ptr->int_hybr_pref,
                                  &ph_ptr->wlan_pref,
                                  ph_ptr->srv_domain_pref,
                                  (cm_act_id_type)ph_ptr,
                                  act_update_reas
                                );

    is_wlan_top_ptr_changed = cmph_insert_orig_mode (
                                  CM_SS_WLAN,
                                  SD_SS_ORIG_MODE_NONE,
                                  CM_ACT_TYPE_PH_OBJ,
                                  ph_ptr->pref_term,
                                  ph_ptr->mode_pref,
                                  ph_ptr->band_pref,
                                  ph_ptr->prl_pref,
                                  ph_ptr->roam_pref,
                                  ph_ptr->hybr_pref,
                                  ph_ptr->int_hybr_pref,
                                  &ph_ptr->wlan_pref,
                                  ph_ptr->srv_domain_pref,
                                  (cm_act_id_type)ph_ptr,
                                  act_update_reas
                              );

    is_hybr_1_top_ptr_changed =  cmph_insert_orig_mode (
                                  CM_SS_HDR,
                                  SD_SS_ORIG_MODE_NONE,
                                  CM_ACT_TYPE_PH_OBJ,
                                  ph_ptr->pref_term,
                                  ph_ptr->mode_pref,
                                  ph_ptr->band_pref,
                                  ph_ptr->prl_pref,
                                  ph_ptr->roam_pref,
                                  ph_ptr->hybr_pref,
                                  ph_ptr->int_hybr_pref,
                                  &ph_ptr->wlan_pref,
                                  ph_ptr->srv_domain_pref,
                                  (cm_act_id_type)ph_ptr,
                                  act_update_reas
                                 );

    #ifndef FEATURE_HYBR_GW
    is_hybr_1_top_ptr_changed = TRUE;
    #endif
  }
  else if( act_update_reas == CM_ACT_UPDATE_REAS_ACT_PWR_SAVE )
  {
    mode_pref = CM_MODE_PREF_NONE;

    /* Insert the origination parameters in the priority queue
    */
    (void) cmph_insert_orig_mode( ss,
                           orig_mode,
                           act_type,
                           force_term,
                           mode_pref,
                           band_pref,
                           prl_pref,
                           roam_pref,
                           hybr_pref,
                           int_hybr_pref,
                           wlan_pref,
                           srv_domain_pref,
                           act_id,
                           act_update_reas
                         );
    is_top_ptr_changed = TRUE;
  }
  /*call subscription processor only if it is a user call
  */
  if(orig_mode != SD_SS_ORIG_MODE_EMERG_CB &&
       orig_mode != SD_SS_ORIG_MODE_EMERG_ORIG &&
       orig_mode != SD_SS_ORIG_MODE_OTASP )
  {
    cmph_process_pending_subscription_cmds( ph_ptr,
                         cmph_map_cm_mode_pref_to_sd_mode_pref( mode_pref ));
  }

  /* Notify MC of mode preference change.
  */
  if( is_top_ptr_changed ||
      ( is_main_top_ptr_changed && is_wlan_top_ptr_changed &&
      is_hybr_1_top_ptr_changed))
  {

    CM_MSG_MED( "Calling cmpref_proc_cmd_pref_sys_chgd() with:",0,0,0);
    CM_MSG_MED( "ss = %d, force_reason = %d, act_type = %d", ss,
                                                    force_reason, act_type );

    CM_MSG_MED( "force_term = %d, mode_pref = %d, band_pref = %ld",
                                          force_term, mode_pref, band_pref );

    CM_MSG_MED( "prl_pref = %d, roam_pref = %d, hybr_pref = %ld", prl_pref,
                                                      roam_pref, hybr_pref );

    CM_MSG_MED( "srv_domain_pref = %d, otasp_act_code = %d", srv_domain_pref,
                                                         otasp_act_code, 0 );

    cmpref_proc_cmd_pref_sys_chgd(
                  cmph_map_cm_ss_type_to_sd_ss_type( ss ),
                  force_reason,
                  orig_mode,
                  cmph_map_cm_mode_pref_to_sd_mode_pref( mode_pref ),
                  cmph_map_cm_band_pref_to_sd_band_pref( band_pref ),
                  cmph_map_cm_prl_pref_to_sd_prl_pref( prl_pref ),
                  cmph_map_cm_roam_pref_to_sd_roam_pref( roam_pref ),
                  int_hybr_pref,
                  cmph_map_otasp_act_code_to_sd_band( otasp_act_code ),
                  cmph_map_otasp_act_code_to_sd_blksys( otasp_act_code ),
                  SD_SS_AVOID_SYS_IDLE,
                  0,
                  plmn_id,
                  cmph_map_cm_srv_domain_pref_to_sd( srv_domain_pref ),
                  cmph_map_cm_acq_order_pref_to_sd( acq_order_pref  ),
                  wlan_pref
                );


    /* Phone is going to do a rescan, so indicate a ballpark estimation
    ** for the number of seconds it takes the phone to do the rescan and
    ** reacquire service.
    */

    rescan_time = CMPH_RESCAN_TIME;

    /* Search the Q for the phone object and change the update reason to
    ** act_start, if we don't do this, then every time the ph pref are forced
    ** the reason will be user
    */

    if( act_update_reas ==  CM_ACT_UPDATE_REAS_USER )
    {
      cm_orig_q_s_type *queue_entry;

      queue_entry = cmtask_orig_para_search_act_id( CM_SS_MAIN, (cm_act_id_type)ph_ptr );
      if(queue_entry != NULL)
      {
        queue_entry->orig.act_update_reas = CM_ACT_UPDATE_REAS_ACT_START;
      }
      else
      {
        CM_ERR( "MAIN queue has no entry for ph!!",0,0,0);
      }

      queue_entry = cmtask_orig_para_search_act_id( CM_SS_HDR, (cm_act_id_type )ph_ptr);
      if(queue_entry != NULL)
      {
        queue_entry->orig.act_update_reas = CM_ACT_UPDATE_REAS_ACT_START;
      }
      else
      {
        CM_ERR( "HDR queue has no entry for ph!!",0,0,0);
      }

      queue_entry = cmtask_orig_para_search_act_id( CM_SS_WLAN, (cm_act_id_type )ph_ptr);
      if(queue_entry != NULL)
      {
        queue_entry->orig.act_update_reas = CM_ACT_UPDATE_REAS_ACT_START;
      }
      else
      {
        CM_ERR( "WLAN queue has no entry for ph!!",0,0,0);
      }
    }


  }
  else if( force_top_pref )
  {
    /* Use the top ptr and force the mode, because we wait for OK to orig
    */
    CM_ASSERT( ph_orig_top_ptr != NULL );
    CM_MSG_MED( "Calling cmpref_proc_cmd_pref_sys_chgd() with:",0,0,0);
    CM_MSG_MED( "ss = %d, force_reason = %d, act_type = %d", ss,
                                                    force_reason, act_type );

    CM_MSG_MED( "force_term = %d, mode_pref = %d, band_pref = %ld",
                force_term, ph_orig_top_ptr->orig.orig_mode_pref,
                                       ph_orig_top_ptr->orig.orig_band_pref );

    CM_MSG_MED( "prl_pref = %d, roam_pref = %d, hybr_pref = %ld",
                  ph_orig_top_ptr->orig.orig_prl_pref,
                  ph_orig_top_ptr->orig.orig_roam_pref,
                  ph_orig_top_ptr->orig.orig_hybr_pref );

    CM_MSG_MED( "srv_domain_pref = %d, otasp_act_code = %d",
             ph_orig_top_ptr->orig.orig_srv_domain_pref, otasp_act_code, 0 );

    cmpref_proc_cmd_pref_sys_chgd(
              cmph_map_cm_ss_type_to_sd_ss_type(ss),
              force_reason,
              ph_orig_top_ptr->orig.orig_mode,
              cmph_map_cm_mode_pref_to_sd_mode_pref( ph_orig_top_ptr->orig.orig_mode_pref ),
              cmph_map_cm_band_pref_to_sd_band_pref( ph_orig_top_ptr->orig.orig_band_pref ),
              cmph_map_cm_prl_pref_to_sd_prl_pref( ph_orig_top_ptr->orig.orig_prl_pref ),
              cmph_map_cm_roam_pref_to_sd_roam_pref( ph_orig_top_ptr->orig.orig_roam_pref ),
              ph_orig_top_ptr->orig.orig_int_hybr_pref,
              cmph_map_otasp_act_code_to_sd_band( otasp_act_code),
              cmph_map_otasp_act_code_to_sd_blksys(otasp_act_code),
              SD_SS_AVOID_SYS_IDLE,
              0,
              plmn_id,
              cmph_map_cm_srv_domain_pref_to_sd(ph_orig_top_ptr->orig.orig_srv_domain_pref ),
              cmph_map_cm_acq_order_pref_to_sd( acq_order_pref  ),
              wlan_pref
            );

  }
  else if( is_main_top_ptr_changed )
  {
    CM_MSG_MED( "Calling cmpref_proc_cmd_pref_sys_chgd() with:",0,0,0);
    CM_MSG_MED( "ss = %d, force_reason = %d, act_type = %d",CM_SS_MAX,force_reason,act_type);
    CM_MSG_MED( "force_term = %d, mode_pref = %d, band_pref = %ld",force_term, mode_pref, band_pref);
    CM_MSG_MED( "prl_pref = %d, roam_pref = %d, hybr_pref = %ld", prl_pref, roam_pref, hybr_pref);
    CM_MSG_MED( "srv_domain_pref = %d, otasp_act_code = %d",srv_domain_pref, otasp_act_code,0);

    cmpref_proc_cmd_pref_sys_chgd(
                  SD_SS_MAX,
                  force_reason,
                  orig_mode,
                  cmph_map_cm_mode_pref_to_sd_mode_pref( mode_pref ),
                  cmph_map_cm_band_pref_to_sd_band_pref( band_pref),
                  cmph_map_cm_prl_pref_to_sd_prl_pref( prl_pref ),
                  cmph_map_cm_roam_pref_to_sd_roam_pref( roam_pref ),
                  int_hybr_pref,
                  cmph_map_otasp_act_code_to_sd_band( otasp_act_code ),
                  cmph_map_otasp_act_code_to_sd_blksys( otasp_act_code ),
                  SD_SS_AVOID_SYS_IDLE,
                  0,
                  plmn_id,
                  cmph_map_cm_srv_domain_pref_to_sd( srv_domain_pref ),
                  cmph_map_cm_acq_order_pref_to_sd( acq_order_pref  ),
                  NULL
                );

  }
  else if( is_wlan_top_ptr_changed )
  {
    CM_MSG_MED( "Calling cmpref_proc_cmd_pref_sys_chgd() with:",0,0,0);
    CM_MSG_MED( "ss = %d, force_reason = %d, act_type = %d",CM_SS_WLAN,force_reason,act_type);
    CM_MSG_MED( "force_term = %d, mode_pref = %d, band_pref = %ld",force_term, mode_pref, band_pref);
    CM_MSG_MED( "prl_pref = %d, roam_pref = %d, hybr_pref = %ld", prl_pref, roam_pref, hybr_pref);
    CM_MSG_MED( "srv_domain_pref = %d, otasp_act_code = %d",srv_domain_pref, otasp_act_code,0);

    cmpref_proc_cmd_pref_sys_chgd(
                  SD_SS_HYBR_WLAN,
                  force_reason,
                  orig_mode,
                  cmph_map_cm_mode_pref_to_sd_mode_pref( mode_pref ),
                  cmph_map_cm_band_pref_to_sd_band_pref( band_pref ),
                  cmph_map_cm_prl_pref_to_sd_prl_pref( prl_pref ),
                  cmph_map_cm_roam_pref_to_sd_roam_pref( roam_pref ),
                  int_hybr_pref,
                  cmph_map_otasp_act_code_to_sd_band( otasp_act_code ),
                  cmph_map_otasp_act_code_to_sd_blksys( otasp_act_code ),
                  SD_SS_AVOID_SYS_IDLE,
                  0,
                  plmn_id,
                  cmph_map_cm_srv_domain_pref_to_sd( srv_domain_pref ),
                  cmph_map_cm_acq_order_pref_to_sd( acq_order_pref  ),
                  wlan_pref
                );
  }
  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif
  else
  {
    CM_MSG_HIGH("Didn't force pref, low priority act.",0,0,0);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return rescan_time;

} /* cmph_force_pref_on_the_fly() */


/*===========================================================================

FUNCTION cmph_unforce_mode_on_the_fly

DESCRIPTION
  Unforce the mode preference on the fly

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  A ballpark estimation for the number of seconds it takes the phone to
  do a rescan and reacquiring service.

SIDE EFFECTS
  none

===========================================================================*/
dword cmph_unforce_mode_on_the_fly(

    cmph_s_type    *ph_ptr
        /* Pointer to a phone object */
)
{

  CM_ASSERT( ph_ptr != NULL );

  CM_FUNC_START( "cmph_unforce_mode_on_the_fly()",0,0,0 );

  CM_MSG_MED ( "cmph_unforce_mode_on_the_fly()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For targets with System Determination 2.0 support do the following
  ** processing.
  */
  return cmph_force_pref_on_the_fly( CM_SS_MAX,
                                     ph_ptr,
                                     SD_SS_PREF_REAS_ORIG_END,
                                     CM_ACT_TYPE_NONE,
                                     CM_PREF_TERM_PERMANENT,
                                     CMPH_PRST_MODE_PREF(ph_ptr),
                                     CMPH_PRST_BAND_PREF(ph_ptr),
                                     CMPH_PRST_PRL_PREF(ph_ptr),
                                     CMPH_PRST_ROAM_PREF(ph_ptr),
                                     CMPH_PRST_HYBR_PREF(ph_ptr),
                                     ph_ptr->plmn,
                                     CMPH_PRST_SRV_DOMAIN_PREF(ph_ptr),
                                     CM_OTASP_ACT_CODE_NONE,
                                     &CMPH_PRST_WLAN_PREF(ph_ptr),
                                     ph_ptr->acq_order_pref,
                                     CM_NETWORK_SEL_MODE_PREF_NONE,
                                     (cm_act_id_type)ph_ptr,
                                     CM_ACT_UPDATE_REAS_ACT_END,
                                     FALSE);


} /* cmph_unforce_mode_on_the_fly() */


//#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/**--------------------------------------------------------------------------
** Functions - external, cdma
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmph_is_valid_mode_pref

DESCRIPTION
  Check whether a given mode preference is valid.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if mode preference is valid, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_valid_mode_pref(

    cm_mode_pref_e_type    mode_pref
        /* mode preference to check */
)
{

  CM_ASSERT( BETWEEN(mode_pref, CM_MODE_PREF_NONE, CM_MODE_PREF_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For targets with System Determination 2.0 support, do the following:
  */
  return cmph_is_valid_mode_band_pref( mode_pref, cmph_ptr()->band_pref );

} /* cmph_is_valid_mode_pref() */


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================

FUNCTION cmph_mo_sms_end

DESCRIPTION
  Does necessary phone's MO SMS end activities.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_mo_sms_end(

    dword          duration,
       /* duration in seconds to delay unforcing the mode */

    cm_act_id_type   act_id
       /* The id of the activity, used to access the orig parameter Q */
)
{

  cm_orig_q_s_type *computed_orig =  cmtask_orig_para_search_act_id  ( CM_SS_MAIN,
                                                                 act_id );
  CM_FUNC_START( "cmph_mo_sms_end()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( computed_orig != NULL )
  {
    /* Wait <duration> seconds for retransmission or message center ack for the
    ** sms then allow system determination to do it's normal thing.
    */

    if( cmtask_orig_para_count_2( CM_SS_MAIN,
                                  CM_ACT_PRIORITY_END,
                                  CM_CMP_CRITERIA_GREATER_THAN) > 1)
    {
      /* Remove this q_ptr from the queue
      */
      if (cmtask_orig_para_delete( CM_SS_MAIN, computed_orig, TRUE ) )
      {
        cmph_force_orig_mode_on_the_fly(CM_SS_MAIN);
      }

    }
    else
    {

      computed_orig->orig.orig_pref_term   = CM_PREF_TERM_CM_1_CALL_PLUS;
      computed_orig->orig.orig_mode_uptime = clk_uptime() + duration;
      if( cmph_cmp_act_priorities(computed_orig->orig.act_priority, CM_ACT_PRIORITY_END ))
      {
        computed_orig->orig.act_priority   = CM_ACT_PRIORITY_END;
      }
    }

    CM_MSG_LOW("orig_pref_term = %d, orig_mode_uptime = %d",
                computed_orig->orig.orig_pref_term,
                computed_orig->orig.orig_mode_uptime,
                0);
  }

} /* cmph_mo_sms_end() */



/*===========================================================================

FUNCTION cmph_mc_rpt_proc

DESCRIPTION
  Process MC reports (i.e. notifications of phone activity or
  replies to clients phone commands).


DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
)
{
  /*lint -save -e826 */
  const cm_mc_rpt_type    *cm_mc_rpt_ptr = (cm_mc_rpt_type *) rpt_ptr;
  /*lint -restore */
      /* Pointer to MC report */

  cmph_s_type                    *ph_ptr        = cmph_ptr();
      /* Point at phone object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_mc_rpt_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmph_mc_rpt_proc(), cmd=%d",
                  cm_mc_rpt_ptr->hdr.cmd, 0, 0);

  /* Do command specific processing as follows:
  ** 1. Updating appropriate phone object fields.
  ** 2. Notifying clients of event if necessary.
  */

  switch( cm_mc_rpt_ptr->hdr.cmd )
  {
    /* CDMA Lock order.
    */
    case CM_LOCK_F:
      ph_ptr->cdma_lock_mode = CM_CDMA_LOCK_MODE_ON;
      cmph_event( ph_ptr, CM_PH_EVENT_CDMA_LOCK_MODE );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* CDMA unlock.
    */
    case CM_UNLOCK_F:
      ph_ptr->cdma_lock_mode = CM_CDMA_LOCK_MODE_OFF;
      cmph_event( ph_ptr, CM_PH_EVENT_CDMA_LOCK_MODE );

      break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #ifdef FEATURE_CM_SS_MEAS
#error code not present
    #endif /* FEATURE_CM_SS_MEAS */
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* CDMA maintenance required command
    */
    case CM_MAINTREQ_F:
      cmph_event( ph_ptr, CM_PH_EVENT_MAINTREQ );
      break;

    #ifdef  FEATURE_HDR_HYBRID
#error code not present
    #endif /* FEATURE_HDR_HYBRID*/


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      break;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is a reply to an earlier command we sent to MC,
  ** also do reply processing.
  */
  if( cm_reply_check(&ph_ptr->reply, cm_mc_rpt_ptr->hdr.cmd) )
  {
    /* Point at client command waiting for reply
    */
    cm_cmd_s_type  *reply_cmd_ptr = cm_reply_get(&ph_ptr->reply);
    CM_ASSERT( reply_cmd_ptr != NULL );


    {
      /* Invoke the reply function with the appropriate error code.
      */
      cmph_client_cmd_mmll_reply( ph_ptr,
                                reply_cmd_ptr,
                                CM_PH_CMD_ERR_NOERR );

      /* We are done with processing the command that was waiting
      ** for reply - deallocate its command buffer.
      */
      cm_cmd_dealloc( reply_cmd_ptr );

      /* reset the reply info */
      cm_reply_init( &ph_ptr->reply );

      /* Turn on the signal that causes us to process any pending
      ** clients commands. This is necessary since we were NOT
      ** processing any clients commands while waiting for a reply
      ** from MC.
      */
      (void) rex_set_sigs( rex_self(), CM_CLIENT_CMD_Q_SIG );
    }
  }
} /* cmph_mc_rpt_proc() */



/*===========================================================================

FUNCTION cmph_send_nam_change_to_mc

DESCRIPTION
  Forward a MC_NAM_CHANGED_F command to MC.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmph_send_nam_change_to_mc(

    const cmph_s_type    *ph_ptr,
        /* Pointer to a phone object */

    cm_nam_e_type  nam
        /* The current NAM */
)
{
  cm_orig_q_s_type     *ph_orig_ptr   = cmtask_orig_para_get_top( CM_SS_MAIN );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_ptr      != NULL );
  CM_ASSERT( ph_orig_ptr != NULL );
  CM_ASSERT( BETWEEN(nam, CM_NAM_NONE, CM_NAM_AUTO) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate nam */
  if(!INRANGE(nam, 0, CM_NAM_AUTO))
  {
    CM_ERR( "nam is invalid, nam change not sent, nam:%d",nam,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform CM Pref & MMOC that subscription changed.
  ** Only CDMA gets a nam change so only cdma protocol stack has to
  ** reload NV information
  */
  if(!INRANGE(ph_ptr->curr_nam, 0, NV_MAX_NAMS))
  {
    CM_ERR( "invalid nam: %d", ph_ptr->curr_nam,0,0);
    return;
  }

  cmpref_proc_cmd_subsc_chgd(
    MMOC_SUBSC_CHG_NAM_SEL,
    (byte) nam,
    (ph_orig_ptr == NULL ? SD_SS_ORIG_MODE_NONE : ph_orig_ptr->orig.orig_mode ),
    cmph_map_cm_mode_pref_to_sd_mode_pref( ph_ptr->prst_mode_pref[nam] ),
    cmph_map_cm_band_pref_to_sd_band_pref(ph_ptr->prst_band_pref[nam]),
    cmph_map_cm_prl_pref_to_sd_prl_pref(ph_ptr->prst_prl_pref[nam]),
    cmph_map_cm_roam_pref_to_sd_roam_pref(ph_ptr->prst_roam_pref[nam]),
    ph_ptr->int_hybr_pref,
    ph_ptr->plmn,
    cmph_map_cm_srv_domain_pref_to_sd(
      CMPH_PRST_SRV_DOMAIN_PREF( ph_ptr)),
    cmph_map_cm_acq_order_pref_to_sd(
      CMPH_PRST_ACQ_ORDER_PREF( ph_ptr )  ),
    ph_ptr->is_gw_subscription_available,
    ph_ptr->is_cdma_subscription_available,
    FALSE,
    &ph_ptr->wlan_pref,
    PROT_SUBSC_CHG_CDMA
    );
} /* cmph_send_nam_change_to_mc() */



/*===========================================================================
FUNCTION cmph_get_user_mode_pref

DESCRIPTION
  This function get the user mode preference

DEPENDENCIES
  Phone object must be initialized

RETURNS
  persistent mode preference

SIDE EFFECTS
  none

===========================================================================*/
cm_mode_pref_e_type cmph_get_user_mode_pref (void)
{
  cmph_s_type          *ph_ptr  =  cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If mode preference was temporarily forced by CM
  ** (i.e. mode term is orig or call), use persistent
  ** mode preference, else use current mode preference.
  */
  if( ph_ptr->pref_term == CM_PREF_TERM_CM_1_CALL ||
      ph_ptr->pref_term == CM_PREF_TERM_CM_1_CALL_PLUS ||
      ph_ptr->pref_term == CM_PREF_TERM_CM_MO_SMS )
  {
    if(INRANGE(ph_ptr->curr_nam, 0, NV_MAX_NAMS))
    {
      return CMPH_PRST_MODE_PREF( ph_ptr );
    }
    else
    {
      CM_ERR( "Invalid value in curr_nam: return mode_pref", 0,0,0);
      return ph_ptr->mode_pref;
    }
  }
  else
  {
    return ph_ptr->mode_pref;
  }

} /* cmph_get_user_mode_pref() */


#ifdef FEATURE_HDR_HYBRID
#error code not present
#endif /* FEATURE_HDR_HYBRID */
#endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */



/*===========================================================================

@FUNCTION cmph_send_ddtm_status

@DESCRIPTION
  Send a DDTM status to 1xCP.

@DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

@RETURNS
  None

@SIDE_EFFECT
  Updates the cur_ddtm_* fields of the phone object to keep track of the DDTM
  settings sent down to 1xCP.

===========================================================================*/
void cmph_send_ddtm_status
(

  cmph_s_type          *ph_ptr,
    /**< Pointer to phone object.
    */

  boolean                     ddtm_status,
    /**< DDTM status to send to MC
    */

  sys_ddtm_act_mask_e_type    ddtm_act_mask,
    /**< DDTM mask to send to MC
    */

  sys_srv_opt_type const      *ddtm_srv_opt_list,
    /**< Service options to ignore if ignore SO bit is set in ddtm_act_mask.
    */

  uint16                      ddtm_num_srv_opt
    /**< No. of service option lists to ignore.
    */

)
{
  #if (defined FEATURE_DDTM_CNTL) && (defined FEATURE_HDR)
#error code not present
  #else
  SYS_ARG_NOT_USED(ddtm_act_mask);
  SYS_ARG_NOT_USED(ddtm_status);
  SYS_ARG_NOT_USED(ddtm_srv_opt_list);
  SYS_ARG_NOT_USED(ddtm_num_srv_opt);
  SYS_ARG_NOT_USED(ph_ptr);
  SYS_ARG_NOT_CONST(ph_ptr);
  #endif /* FEATURE_DDTM_CNTL && FEATURE_HDR */
}


/*===========================================================================

FUNCTION cmph_ddtm_disable_cdma_tx

DESCRIPTION
  Send cmd to CDMA to disable TX.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmph_ddtm_disable_cdma_tx
(

       cmph_s_type          *ph_ptr
         /* Pointer to phone object.
         */
)
/*lint -esym(715, ph_ptr) */
{
  #if (defined FEATURE_DDTM_CNTL) && (defined FEATURE_HDR)
#error code not present
  #else
  SYS_ARG_NOT_CONST(ph_ptr);
  #endif
}
/*lint +esym(715, ph_ptr) */


/*===========================================================================

FUNCTION cmph_update_ddtm_status

DESCRIPTION
  Send a DDTM status to MC

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmph_update_ddtm_status (

    cmph_s_type      *ph_ptr
        /* Pointer to a phone object */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef FEATURE_DDTM_CNTL

  cmss_s_type *ss_ptr = cmss_ptr();
  #ifdef FEATURE_BCMCS
#error code not present
  #endif

  /* List of conditions used in DDTM algorithm.
  */
  boolean new_ddtm_status             = FALSE;
  boolean is_bcmcs_flow_monitoring    = FALSE;
  boolean is_cdma_voice_or_emerg_call = FALSE;
  boolean is_call_on_ss_hdr           = FALSE;
  boolean is_call_connected_on_ss_hdr = FALSE;
  boolean is_mo_sms_or_dbm_in_prog    = FALSE;
  boolean is_pd_fix                   = FALSE;
  boolean is_high_psist               = FALSE;

  #ifdef FEATURE_ATOM
  boolean is_voip_call_on_ss_hdr      = FALSE;
  boolean is_voip_available_on_hdr    = FALSE;
  #endif

  boolean is_all_so_ignored_cur       = FALSE;
  boolean is_all_so_ignored_new       = FALSE;
  boolean is_so_list_diff             = FALSE;

  /* Arrays below is static to save stack usage.
  */
  static  sys_srv_opt_type new_ddtm_so_list [ SYS_DDTM_MAX_SO_LIST_SIZE ];
  uint16                   new_ddtm_num_so_list   = 0;
  sys_ddtm_act_mask_e_type new_ddtm_act_mask = SYS_DDTM_ACT_MASK_EMPTY;
  boolean is_emerg_call_or_mode       = FALSE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( ss_ptr != NULL );
  #ifdef FEATURE_BCMCS
#error code not present
  #endif
  SYS_ARG_NOT_USED(is_emerg_call_or_mode);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Precompute all the conditions.
  */
  #ifdef FEATURE_BCMCS
#error code not present
  #endif

  #if (defined FEATURE_DDTM_ON_DURING_GPS_FIX) && (defined FEATURE_GPSONE)
#error code not present
  #endif

  is_high_psist = ( ss_ptr->info.psist == CM_PSIST_HIGH );

  is_mo_sms_or_dbm_in_prog = ( cmxsms_is_mo_sms_in_progress()
                               || cmxdbm_is_mo_dbm_in_progress() );

  is_all_so_ignored_cur = ( ph_ptr->cur_ddtm_num_srv_opt == 1
                            && ph_ptr->cur_ddtm_srv_opt_list[0] == CAI_SO_NULL
                            && SYS_IS_BIT_CONTAIN(
                                  ph_ptr->cur_ddtm_act_mask,
                                  SYS_DDTM_ACT_MASK_IGN_SO_PAGES )
                          );

  #ifdef FEATURE_ATOM
  is_voip_available_on_hdr = cmcall_is_active_ip_app( SYS_SYS_MODE_HDR,
                                               CM_CALL_TYPE_VOICE );
  #endif

  is_emerg_call_or_mode = cmph_is_in_emergency_cb();

  CMCALL_FOR_EACH_CALL_DO
    if( this_call->ss == CM_SS_HDR )
    {
      is_call_on_ss_hdr = TRUE;

      #ifdef FEATURE_ATOM
      if( this_call->call_type == CM_CALL_TYPE_VOICE )
      {
        is_voip_call_on_ss_hdr = TRUE;
      }
      #endif

      if( this_call->call_state  != CM_CALL_STATE_IDLE
          && this_call->cmcall_mode_info.info.cdma_info.srv_opt
                                            == CAI_SO_HDR_PKT_DATA )
      {
        is_call_connected_on_ss_hdr = TRUE;
      }
    }
    if( ( this_call->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA
          && this_call->call_type == CM_CALL_TYPE_VOICE
          && this_call->ss == CM_SS_MAIN )
        || this_call->call_type == CM_CALL_TYPE_EMERGENCY )
    {
      is_cdma_voice_or_emerg_call = TRUE;
    }
    #ifdef FEATURE_ATOM
    if( this_call->call_type == CM_CALL_TYPE_EMERGENCY )
    {
      is_emerg_call_or_mode = TRUE;
    }
    #endif
  CMCALL_END_FOR

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef FEATURE_DEDICATED_C2K_MODE
  if( ss_ptr->is_hdr_session_open && !is_emerg_call_or_mode )
  {
    if( ph_ptr->is_cdma_tx_disabled == FALSE )
    {
      cmph_ddtm_disable_cdma_tx( ph_ptr );

      /* The line below was put here instead of cmph_ddtm_disable_cdma_tx() to
      ** avoid the risk of changing the existing behavior.
      */
      ph_ptr->is_cdma_tx_disabled = TRUE;
    }

    return;
  }

  ph_ptr->is_cdma_tx_disabled = FALSE;

  #endif

  /* Compute the DDTM settings to be send to 1xCP. We start with an empty
  ** DDTM mask and add corresponding suppression for each condition that is
  ** satisfied.
  */

  /* If in high psist and we are in a HDR call or monitoring BCMCS then
  ** suppress MO DBM and registrations (since they will not succeed anyway)
  */
  if( is_high_psist
      && (is_call_on_ss_hdr
          || is_bcmcs_flow_monitoring ) )
  {
    new_ddtm_act_mask = cmph_misc_add_ddtm_act_mask(
                      new_ddtm_act_mask,
                      2,
                      SYS_DDTM_ACT_MASK_SUPPRESS_MO_DBM,
                      SYS_DDTM_ACT_MASK_SUPPRESS_REG );
  }

  /* If DDTM pref is ON:
  **
  ** Note:
  ** 1. If High PSIST is on, then a HDR orig is equivalent to a connected HDR
  **    call.
  ** 2. If High PSIST is on, we ignore MO SMS/DBM in progess.
  **
  ** Turn on user DDTM pref under any one of the following conditions:
  ** 1. Hybrid HDR connected data call and no MO SMS/DBM (See note 1 & 2).
  ** 2. Monitoring BCMCS but no voice/emerg/MO SMS/DBM (See note 2).
  ** 3. PD fix is in progress.
  */
  if(ph_ptr->ddtm_pref == CM_DDTM_PREF_ON
                  &&
     (
       (
         ( is_call_connected_on_ss_hdr
           || ( is_call_on_ss_hdr && is_high_psist)
           || ( is_bcmcs_flow_monitoring && !is_cdma_voice_or_emerg_call ) )
                  &&
         ( !is_mo_sms_or_dbm_in_prog || is_high_psist )
       )
       /*lint -e{774} Boolean within 'if' always evaluates to False */
       || is_pd_fix
     )
    )
  {
    new_ddtm_act_mask = cmph_misc_add_ddtm_act_mask( new_ddtm_act_mask, 1,
                                                 ph_ptr->ddtm_act_mask );
  }

  #ifdef FEATURE_ATOM
  /* If a VoIP/HDR call is going on, then make sure no 1x activity interrupts
  ** it.
  */
  if( is_voip_call_on_ss_hdr && !is_emerg_call_or_mode )
  {
    new_ddtm_act_mask = SYS_DDTM_ACT_MASK_ALL;

    /* Ignore ALL SO pages.
    */
    new_ddtm_num_so_list = 1;
    new_ddtm_so_list[0]  = CAI_SO_NULL;
    is_all_so_ignored_new = TRUE;
  }

  /* If VoIP/HDR is available, then turn off 1x registration.
  */
  if( is_voip_available_on_hdr && !is_emerg_call_or_mode )
  {
    new_ddtm_act_mask = cmph_misc_add_ddtm_act_mask(
                      new_ddtm_act_mask,
                      1,
                      SYS_DDTM_ACT_MASK_SUPPRESS_REG );
  }
  #endif

  if(new_ddtm_act_mask != SYS_DDTM_ACT_MASK_EMPTY)
  {
    new_ddtm_status = TRUE;
  }

  /* Compute the new SO list if.
  **
  ** 1. Ignoring SO pages are turned on.
  ** 2. All SO pages are not already ignored by using CAI_SO_NULL.
  */
  /*lint -e{774} */
  if( SYS_IS_BIT_CONTAIN( new_ddtm_act_mask, SYS_DDTM_ACT_MASK_IGN_SO_PAGES )
      && !is_all_so_ignored_new
    )
  {
    uint16 i;

    /* We always just copy the DDTM SO list pref as of now.
    ** Can change this in the future as requirements change.
    */
    for( i=0; i < ph_ptr->ddtm_num_srv_opt; i++ )
    {
      new_ddtm_so_list[i] = ph_ptr->ddtm_srv_opt_list[i];
      if( new_ddtm_so_list[i] == CAI_SO_NULL )
      {
        is_all_so_ignored_new = TRUE;
      }
    }
    new_ddtm_num_so_list = i;
  }


  /* If ignoring pages is not turned on, the set to SO list count to zero
  ** just for the sake of cleaning up garbage values.
  */
  if( !SYS_IS_BIT_CONTAIN( new_ddtm_act_mask,
                           SYS_DDTM_ACT_MASK_IGN_SO_PAGES ) )
  {
    new_ddtm_num_so_list = 0;
  }

  /* Check if the SO lists changed.
  **
  ** Should catch all differences ONLY IF the possible SO list values are
  ** limited to:
  ** 1. All SOs are blocked with CAI_SO_NULL.
  ** 2. All the SOs stored in NV (ph_ptr->ddtm_srv_opt_list).
  */
  if( is_all_so_ignored_cur != is_all_so_ignored_new )
  {
    is_so_list_diff = TRUE;
  }

  if( new_ddtm_act_mask != ph_ptr->cur_ddtm_act_mask
      || new_ddtm_status != ph_ptr->cur_ddtm_status
      || is_so_list_diff )
  {
    cmph_send_ddtm_status( ph_ptr, new_ddtm_status, new_ddtm_act_mask,
                           new_ddtm_so_list, new_ddtm_num_so_list );
  }

  #else

  SYS_ARG_NOT_USED(ph_ptr);
  SYS_ARG_NOT_CONST(ph_ptr);

  #endif
}

#ifdef FEATURE_DEDICATED_C2K_MODE
/*===========================================================================

FUNCTION cmph_send_hdrmc_session_close_cmd

DESCRIPTION
  Send an end command to either MC or HDRMC task

DEPENDENCIES
  Call object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_send_hdrmc_session_close_cmd( void )
{
  hdrsmp_deactivate(HDRHAI_CM, FALSE);
}
#endif


#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */



/*===========================================================================

FUNCTION cmph_update_orig_mode_uptime

DESCRIPTION
  This function increases the orig mode uptime of ph by CMPH_HOLD_MODE_PREF_TIME
  plus clock uptime.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_update_orig_mode_uptime (
  cm_ss_e_type ss,
    /* The ss to search this activcity for
    */
  cm_act_id_type act_id
    /* The id of the activity
    */
)
{
  cm_orig_q_s_type *computed_orig =  cmtask_orig_para_search_act_id  ( ss, act_id );

  CM_FUNC_START( "cmph_update_orig_mode_uptime()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( computed_orig != NULL )
  {
    computed_orig->orig.orig_mode_uptime = clk_uptime() +
                                                    CMPH_HOLD_MODE_PREF_TIME;
  }

} /* cmph_update_orig_mode_uptime () */



/*===========================================================================

FUNCTION cmph_is_cdma_subscription_available

DESCRIPTION
  This function returns a boolean indicating whether cdma subscription is available.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_cdma_subscription_available (void)
{
  cmph_s_type                *ph_ptr   = cmph_ptr();

  /*taking the pending flag into account on subscription availability
  */
  return (ph_ptr->is_cdma_subscription_available ||
          (ph_ptr->is_pending_subsc_cdma &&
           (ph_ptr->cdma_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE ||
            ph_ptr->cdma_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED)));
}


/*===========================================================================

FUNCTION cmph_is_gw_subscription_available

DESCRIPTION
  This function returns a boolean indicating whether GSM/WCDMA subscription is available.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE^M
  boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_gw_subscription_available (void)
{
  cmph_s_type                *ph_ptr   = cmph_ptr();

  /*taking the pending flag into account on subscription availability
  */
  return (ph_ptr->is_gw_subscription_available||
          (ph_ptr->is_pending_subsc_gw &&
           (ph_ptr->gw_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE ||
            ph_ptr->gw_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED)));
}


/*===========================================================================^M
^M
FUNCTION cmph_get_cdma_subscription_source
^M
DESCRIPTION^M
  This function will determine where cdma subscription is coming from.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_cc_subscription_source_e_type cmph_get_cdma_subscription_source (void)
{
  #ifdef FEATURE_MMGSDI
  gsdi_card_apps_T               card_apps;
  uint8                          slot_number;
  #endif

  cmph_s_type                   *ph_ptr  =  cmph_ptr();


  /* Check if there is a cdma subscription */
  if (ph_ptr->is_cdma_subscription_available == FALSE)
  {
    return CM_CC_SUBSCRIPTION_SOURCE_NONE;
  }

  /* Check if cdma subscription is from NV */
  if (ph_ptr->rtre_control == CM_RTRE_CONTROL_NV)
  {
    return CM_CC_SUBSCRIPTION_SOURCE_NV;
  }

  #ifdef FEATURE_MMGSDI
  /* Then we must be getting subscription from the SIM ...
     Figure out which one
  */

  gsdi_get_apps_available (&card_apps);
  /* Bits 2-3 of operational slot is the slot number for CDMA */
  slot_number = card_apps.operational_slot;

  if ((slot_number & GSDI_CDMA_SLOT_1_MASK) ==  GSDI_CDMA_SLOT_1_MASK)
  {
    return CM_CC_SUBSCRIPTION_SOURCE_SIM_1;
  }
  else if ((slot_number & GSDI_CDMA_SLOT_2_MASK) == GSDI_CDMA_SLOT_2_MASK)
  {
    return CM_CC_SUBSCRIPTION_SOURCE_SIM_2;
  }
  #endif

  return CM_CC_SUBSCRIPTION_SOURCE_NONE;

}   /* cmph_get_cdma_subscription_source */


/*===========================================================================

FUNCTION cmph_get_gw_subscription_source

DESCRIPTION
  This function will determine where gw subscription is coming from

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_cc_subscription_source_e_type cmph_get_gw_subscription_source (void)
{

  #if ( defined (FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
  #endif

  return CM_CC_SUBSCRIPTION_SOURCE_NONE;

}   /* cmph_get_gw_subscription_source */


/*===========================================================================

FUNCTION cmph_get_ps_data_orig_prefs

DESCRIPTION
  This function will return where the preferences for packet switched data calls.

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  preferences for ps data originations

SIDE EFFECTS
  none

===========================================================================*/
cm_ps_data_orig_prefs_e_type cmph_get_ps_data_orig_prefs (void)
{

  cmph_s_type                *ph_ptr = cmph_ptr();

  return ph_ptr->ps_data_orig_pref;

}   /* cmph_get_ps_data_orig_prefs */


/*===========================================================================

FUNCTION cmph_call_end_req

DESCRIPTION
  Does necessary phone's call end req activities. It is applicable
  for GSM/WCDMA only.

  This function should be called from cmwcall_end_req while sending
  CM_CALL_EVENT_END_REQ to its clients.


DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_call_end_req
(
    const cmcall_s_type    *call_ptr
        /* Pointer to a call object */
)
/*lint -esym(715, call_ptr)*/
{
  /* Get the pointer to the phone object */
  cmph_s_type             *ph_ptr = cmph_ptr();

  /* Check if all calls are in idle.
  */
  if(cmcall_check_each_call_in_call_state(CM_CALL_STATE_IDLE))
  {

    /* Update DDTM status.
    */
    cmph_update_ddtm_status(ph_ptr);

    /* Generate In use event.
    */
    cmph_event( ph_ptr, CM_PH_EVENT_IN_USE_STATE);
  }

} /* cmph_call_end_req */
/*lint +esym(715, call_ptr)*/

/*===========================================================================

FUNCTION cmph_last_gw_pkt_call

DESCRIPTION

  This function initializes the wait timer for the PS signaling down
  indication. This fuction also sets the flag indcating that we are
  waiting for the PS singaling down indication from lower layers.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_last_gw_pkt_call(void)
{

  cmph_set_ps_sig_down_wait_time(CMPH_GW_PKT_SIGNALING_DOWN_WAIT_TIME);



} /* cmph_last_gw_pkt_call */

/*===========================================================================

FUNCTION cmph_set_ps_sig_down_wait_time

DESCRIPTION

   This function sets the is_ps_signaling_down_wait_time
   with the value passed in the argument.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_set_ps_sig_down_wait_time(dword time)
{
  /* Get the pointer to the phone object */
  cmph_s_type             *ph_ptr = cmph_ptr();

  if(time != CMPH_GW_PKT_WAIT_TIME_NONE)
  {
    ph_ptr->ps_signaling_down_wait_time = clk_uptime() + time;
  }
  else
  {
    ph_ptr->ps_signaling_down_wait_time = time;
  }
} /* cmph_set_ps_sig_down_ind_wait_flag */

/*===========================================================================

FUNCTION cmph_is_waiting_for_ps_sig_down

DESCRIPTION

   This function return TRUE if CM is waiting for
   the PS Signaling down indication from lower layers.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE  : If CM is waiting for the PS Sig Down Ind from Lower Layers.
 FALSE : If CM is not waiting for the PS Sig Down Ind from Lower Layers.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_waiting_for_ps_sig_down(void)
{
  /* Get the pointer to the phone object */
  cmph_s_type             *ph_ptr = cmph_ptr();


  if(ph_ptr->ps_signaling_down_wait_time >= clk_uptime() )
  {
    return TRUE;
  }

  return FALSE;


} /* cmph_set_ps_sig_down_ind_wait_flag */

/*===========================================================================

FUNCTION cmph_als_reset

DESCRIPTION

  ALS custom object was reset due to PH receiving subscription available,
  not available or changed event, so reset ALS settings in PH object.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmph_als_reset(void)
{

  cmph_s_type     *ph_ptr = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_ptr != NULL );
  CM_FUNC_START( "cmph_als_reset()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Reset ALS info after GW subscription changed.
  ** If GW subscription is still available,
  ** the phone will re-read ALS settings and send
  ** appropriate events later
  */
  ph_ptr->cphs_allowed   =  CM_CPHS_PROC_NOT_ALLOWED;
  ph_ptr->als_allowed    =  CM_ALS_PROC_NOT_ALLOWED;
  ph_ptr->line_switching =  CM_ALS_LINE_SWITCHING_NONE;
  ph_ptr->line           =  CM_ALS_LINE_NONE;

}

/*===========================================================================

FUNCTION cmph_als_supported

DESCRIPTION

  ALS is supported when:

  - CPHS is allowed
  - ALS is allowed

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean cmph_als_supported(void)
{

  cmph_s_type                *ph_ptr  = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( CM_INIT_CHECK(ph_ptr->init_mask) );

  CM_FUNC_START( "cmph_als_supported()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return BOOLEAN((ph_ptr->cphs_allowed == CM_CPHS_PROC_ALLOWED) &&
                 (ph_ptr->als_allowed  == CM_ALS_PROC_ALLOWED));
}


/*===========================================================================

FUNCTION cmph_term_pending_get_networks_req

DESCRIPTION

  This function should be called when the phone preferences have changed or
  There is operation mode change other than online.
  - If there is a pending get network request, send a terminate event to
    clients and reset the flag.

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmph_term_pending_get_networks_req
(
    sd_ss_e_type                   ss
        /* Indicates the SS-Instance ( SS_MAIN or SS_HDR) based on which
        ** the corresponding getNetworks requests are terminated.
        */
)
{

  cmph_s_type                *ph_ptr  = cmph_ptr();
  /* Pointer to a phone object */

  /* Terminate any pending get network list request and reset the flag.
  ** based on the Stack.
  ** SD_SS_MAIN - GW request can be terminated
  ** SD_SS_WLAN - WLAN request can be terminated
  ** SD_SS_MAX  - Any (GW/WLAN) request can be terminated
  */
  if( ((ph_ptr->network_type == CM_MODE_PREF_GSM_ONLY       ||
        ph_ptr->network_type == CM_MODE_PREF_WCDMA_ONLY     ||
        ph_ptr->network_type == CM_MODE_PREF_GSM_WCDMA_ONLY)
        && (ss == SD_SS_MAIN || ss == SD_SS_HYBR_1 || ss == SD_SS_MAX)
      )
      ||
      ( ph_ptr->network_type == CM_MODE_PREF_WLAN_ONLY &&
        (ss == SD_SS_HYBR_WLAN || ss == SD_SS_MAX)
      )
    )
  {
    cmph_event( ph_ptr, CM_PH_EVENT_TERMINATE_GET_NETWORKS );
    ph_ptr->network_type = CM_MODE_PREF_NONE;
  }

  return;

}


/*===========================================================================

FUNCTION cmph_is_valid_wlan_pref

DESCRIPTION
  Check whether a specified combination of wlan network selection parametrs
  are valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the specified combination is supported.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmph_is_valid_wlan_pref
(
    const cm_wlan_pref_s_type               *wlan_pref_ptr
      /* WLAN Network selection mode pref to check for validity
      */
)
{

  if(wlan_pref_ptr == NULL)
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check for valid WLAN network selection mode.
  */

  if(wlan_pref_ptr->network_sel_mode_pref != CM_NETWORK_SEL_MODE_PREF_AUTOMATIC &&
     wlan_pref_ptr->network_sel_mode_pref != CM_NETWORK_SEL_MODE_PREF_MANUAL &&
     wlan_pref_ptr->network_sel_mode_pref != CM_NETWORK_SEL_MODE_PREF_NO_CHANGE)
  {
     CM_MSG_LOW("Bad WLAN Net Sel Mode",0,0,0);
     return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check for valid WLAN params in MANUAL mode.
  */

  if(wlan_pref_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
  {
    if( wlan_pref_ptr->ssid.len > ARR_SIZE(wlan_pref_ptr->ssid.ssid) )
    {
      return FALSE;
    }

    if( wlan_pref_ptr->bss_id   == 0 &&
        wlan_pref_ptr->ssid.len == 0 )
     {
       CM_MSG_LOW("Invalid BSSID/SSID",0,0,0);
       return FALSE;
     }

    /* Check for valid WLAN BSS TYPE.
    */

    if (!BETWEEN((int)wlan_pref_ptr->bss_type, (int)SYS_WLAN_BSS_TYPE_NONE,
                                               (int)SYS_WLAN_BSS_TYPE_MAX))
    {
      CM_MSG_LOW(" Invalid WLAN BSS TYPE",0,0,0);
      return FALSE;
    }

    if( (wlan_pref_ptr->band & SYS_BAND_MASK_WLAN_ANY)
                                                    == SYS_BAND_MASK_EMPTY )
    {
      CM_MSG_LOW("Invalid WLAN manual band info",0,0,0);
      return FALSE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check WLAN Technology Preference
  */

  if (!BETWEEN((int)wlan_pref_ptr->tech_pref, (int)CM_WLAN_TECH_PREF_NONE,
                                              (int)CM_WLAN_TECH_PREF_MAX))
  {
       CM_MSG_LOW(" Invalid WLAN Tech",0,0,0);
       return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check for valid WLAN Scan preference.
  */

  if (!BETWEEN((int)wlan_pref_ptr->scan_mode, (int)CM_WLAN_SCAN_PREF_NONE,
                                              (int)CM_WLAN_SCAN_PREF_MAX))
  {
    CM_MSG_LOW(" Invalid WLAN Scan Pref",0,0,0);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN_ADHOC
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* cmph_is_valid_wlan_pref */

/*===========================================================================

FUNCTION cmph_is_wlan_subscription_available

DESCRIPTION
  This function returns a boolean indicating whether WLAN subscription is available.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE^M
  boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_wlan_subscription_available (void)
{
  cmph_s_type                *ph_ptr   = cmph_ptr();

  /*taking the pending flag into account on subscription availability
  */
  return (ph_ptr->is_wlan_subscription_available||
          (ph_ptr->is_pending_subsc_wlan &&
           (ph_ptr->wlan_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE ||
            ph_ptr->wlan_subsc_cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED)));
}


/*===========================================================================

FUNCTION cmph_process_wlan_bssid_list

DESCRIPTION
  This function processes a WLAN BSS list received from SD.

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_process_wlan_bss_list
(
  const cm_wlan_bss_list_type    *list_ptr
    /* Pointer to lower level report data */
)
{
  /* Pointer to phone object.
  */
  cmph_s_type                *ph_ptr = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( list_ptr != NULL)
  CM_ASSERT( ph_ptr   != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(list_ptr == NULL)  /*lint !e774 */
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ph_ptr->network_type != CM_MODE_PREF_WLAN_ONLY)
  {
    /* No one requested a list, lets not copy.
    */
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_ptr->wlan_bss_list   = list_ptr->list;

  cmph_set_net_lists_present_flag(TRUE);

  cmph_event( ph_ptr, CM_PH_EVENT_AVAILABLE_NETWORKS_CONF );

  cmph_set_net_lists_present_flag(FALSE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_ptr->network_type = CM_MODE_PREF_NONE;

  return;

} /* cmph_process_wlan_bss_list() */

/*===========================================================================

FUNCTION cmph_process_wlan_wps_result

DESCRIPTION
  This function processes a WLAN WPS credentials list received from SD.

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_process_wlan_wps_result
(
  const cm_wlan_wps_result_type    *result_ptr
    /* Pointer to lower level report data */
)
{
  #ifdef FEATURE_WLAN_WPS
#error code not present
  #else
  SYS_ARG_NOT_USED (result_ptr);
  #endif /* FEATURE_WLAN_WPS */

  return;

} /* cmph_process_wlan_wps_list() */


/*===========================================================================

FUNCTION  cmph_cmd_get_networks_para_check

DESCRIPTION
  This function validates the get networks command parameters.

  If the network list type is CM_NETWORK_LIST_USER_PREFERRED, then
  an attempt is made to read the list from the (U)SIM.

DEPENDENCIES
  none

RETURN VALUE
  CM_PH_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_ph_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_ph_cmd_err_e_type     cmph_cmd_get_networks_para_check
/*lint -esym(818, ph_ptr) */
(

  cmph_s_type                *ph_ptr,
      /* Pointer to a phone object */

  cm_ph_cmd_s_type           *ph_cmd_ptr
     /* Pointer at phone command component */
)
{
  cm_ph_cmd_info_s_type      *cmd_info_ptr = NULL;
  cm_ph_cmd_err_e_type        cmd_err      = CM_PH_CMD_ERR_NOERR;


  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_ptr     != NULL);
  CM_ASSERT(ph_cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  if(ph_ptr->network_type != CM_MODE_PREF_NONE &&
     ph_ptr->network_type != cmd_info_ptr->network_type)
  {
    /* Currently we support one type of get network request at a time
    ** If we come here, it means one type of request is pending while
    ** GetNetwork request for a different type of network arrives.
    */

    cmd_err = CM_PH_CMD_ERR_GET_NETWORKS_S;
    return cmd_err;
  }

  if( cmd_info_ptr->network_type != CM_MODE_PREF_WLAN_ONLY     &&
      cmd_info_ptr->network_type != CM_MODE_PREF_GSM_ONLY      &&
      cmd_info_ptr->network_type != CM_MODE_PREF_WCDMA_ONLY    &&
      cmd_info_ptr->network_type != CM_MODE_PREF_GSM_WCDMA_ONLY )
  {
    /* Unsupported network list requested.
    */
    cmd_err = CM_PH_CMD_ERR_NET_TYPE_P;
    return cmd_err;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (! sd_misc_is_target_supp_mode_band (
        cmph_map_cm_mode_pref_to_sd_mode_pref(cmd_info_ptr->network_type),
                                              SD_SS_BAND_PREF_ANY)
     )
  {
    /* Target does not support the network type requested to search for.
    */
    cmd_err = CM_PH_CMD_ERR_NET_TYPE_P;
    return cmd_err;
  }

  /*
  ** Check if the phones current mode supports the get network request.
  */
  if(!cmcall_misc_is_mode_pref(ph_ptr->mode_pref, 
                               cmd_info_ptr->network_type))
  {
    /* Target does not support the network type requested to search for.
    */
    cmd_err = CM_PH_CMD_ERR_NET_TYPE_P;
    return cmd_err;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( cmd_info_ptr->network_type == CM_MODE_PREF_WLAN_ONLY  &&
      cmcall_is_active_on_mode(SYS_SYS_MODE_WLAN) )
  {
    cmd_err = CM_PH_CMD_ERR_IN_USE_S;
    return cmd_err;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( (cmd_info_ptr->network_type == CM_MODE_PREF_GSM_WCDMA_ONLY  ||
       cmd_info_ptr->network_type == CM_MODE_PREF_GSM_ONLY        ||
       cmd_info_ptr->network_type == CM_MODE_PREF_WCDMA_ONLY)
             &&
       ! cmcall_is_no_call_in_gw_cs() )

  {
    cmd_err = CM_PH_CMD_ERR_IN_USE_S;
    return cmd_err;
  }

  if ( cmd_info_ptr->network_list_type == CM_NETWORK_LIST_USER_PREFERRED ||
       cmd_info_ptr->network_list_type == CM_NETWORK_LIST_USER_PREFERRED_UNPACKED )
  {
    /*
    ** Attempt to read the User Preferred PLMN List from the (U)SIM.
    */

    #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
    #endif

    /* This request is allowed only for GW*/

    if( cmd_info_ptr->network_type != CM_MODE_PREF_GSM_WCDMA_ONLY &&
        cmd_info_ptr->network_type != CM_MODE_PREF_GSM_ONLY       &&
        cmd_info_ptr->network_type != CM_MODE_PREF_WCDMA_ONLY)
    {
      cmd_err = CM_PH_CMD_ERR_MODE_PREF_S;
      return cmd_err;
    }
    #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
    #endif
  }
  else if ( cmd_info_ptr->network_list_type != CM_NETWORK_LIST_AVAILABLE )
  {
    cmd_err = CM_PH_CMD_ERR_NETWORK_LIST_TYPE_P;
    CM_ERR( "Invalid get networks cmd network list type param %d",
            cmd_info_ptr->network_list_type,
            0,
            0 );
    return cmd_err;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( cmd_info_ptr->network_type == CM_MODE_PREF_WLAN_ONLY  &&
      !cmph_is_wlan_subscription_available() )
  {
    cmd_err = CM_PH_CMD_ERR_NO_SUBSCRIPTION_S;
    return cmd_err;
  }

  return cmd_err;

} /* cmph_cmd_get_networks_para_check */
/*lint +esym(818, ph_ptr) */


/*===========================================================================

FUNCTION  cmph_cmd_terminate_get_networks_para_check

DESCRIPTION
  This function validates the terminate get networks command parameters.

DEPENDENCIES
  none

RETURN VALUE
  CM_PH_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_ph_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_ph_cmd_err_e_type     cmph_cmd_terminate_get_networks_para_check(

  const cmph_s_type                *ph_ptr
      /* Pointer to a phone object */
)
{
  cm_ph_cmd_err_e_type        cmd_err      = CM_PH_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( ph_ptr->network_type != CM_MODE_PREF_GSM_WCDMA_ONLY &&
      ph_ptr->network_type != CM_MODE_PREF_GSM_ONLY       &&
      ph_ptr->network_type != CM_MODE_PREF_WCDMA_ONLY     &&
      ph_ptr->network_type != CM_MODE_PREF_WLAN_ONLY)
  {
    /*
    ** The request to terminate a get network list is only valid if
    ** there is a pending request for the list of available networks.
    **/
    cmd_err = CM_PH_CMD_ERR_GET_NETWORKS_S;

  }
  else if ( !cmph_in_valid_oprt_mode( ph_ptr ) )
  {
    cmd_err = CM_PH_CMD_ERR_OPRT_MODE_S;
  }

  return cmd_err;
} /* cmph_cmd_terminate_get_networks_para_check */

/*===========================================================================
FUNCTION cmph_cmd_start_wps_para_check

DESCRIPTION
  Check parameter errors for WPS.

DEPENDENCIES
  None

RETURN VALUE
  CM_PH_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  None
===========================================================================*/
static cm_ph_cmd_err_e_type       cmph_cmd_start_wps_para_check(

  const cmph_s_type                *ph_ptr,
      /* Pointer to a phone object */

  const cm_ph_cmd_s_type           *ph_cmd_ptr
     /* Pointer at phone command component */
)
{
  cm_ph_cmd_err_e_type            cmd_err      = CM_PH_CMD_ERR_NOERR;
  #ifdef FEATURE_WLAN_WPS
#error code not present
  #else
  SYS_ARG_NOT_USED (ph_ptr);
  SYS_ARG_NOT_USED (ph_cmd_ptr);
  #endif /* FEATURE_WLAN_WPS */

  return cmd_err;

} /* cmph_cmd_start_wps_para_check() */

/*===========================================================================

FUNCTION  cmph_cmd_abort_wps_para_check

DESCRIPTION
  This function validates the terminate get networks command parameters.

DEPENDENCIES
  none

RETURN VALUE
  CM_PH_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_ph_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_ph_cmd_err_e_type     cmph_cmd_abort_wps_para_check(

  const cmph_s_type                *ph_ptr
      /* Pointer to a phone object */
)
{
  cm_ph_cmd_err_e_type        cmd_err      = CM_PH_CMD_ERR_NOERR;

  #ifdef FEATURE_WLAN_WPS
#error code not present
  #else
  SYS_ARG_NOT_USED (ph_ptr);
  #endif  

  return cmd_err;
} /* cmph_cmd_abort_wps_para_check */
/*===========================================================================

FUNCTION cmph_map_cm_wlan_scan_pref_to_sys_pref

DESCRIPTION
  Map CM WLAN SCAN Pref to SYS WLAN Scan Pref.

DEPENDENCIES
  none

RETURN VALUE
  The SYS type that corresponds to the input CM WLAN SCAN PREF

SIDE EFFECTS
  None

===========================================================================*/
sys_wlan_scan_pref_e_type cmph_map_cm_wlan_scan_pref_to_sys_pref
(
  cm_wlan_scan_pref_e_type cm_pref
)
{

  switch(cm_pref) {

    case CM_WLAN_SCAN_PREF_ACTIVE:  return SYS_WLAN_SCAN_PREF_ACTIVE;
    case CM_WLAN_SCAN_PREF_PASSIVE: return SYS_WLAN_SCAN_PREF_PASSIVE;
    case CM_WLAN_SCAN_PREF_AUTO:    return SYS_WLAN_SCAN_PREF_AUTO;

    default:
      CM_ERR("No match! - %d",cm_pref,0,0);
      return SYS_WLAN_SCAN_PREF_AUTO;
  }

}

/*===========================================================================

FUNCTION cmph_map_cm_wlan_bss_type_pref_to_sys_pref

DESCRIPTION
  Map CM WLAN BSS type Pref to SYS WLAN BSS type Pref.

DEPENDENCIES
  none

RETURN VALUE
  The SYS type that corresponds to the input CM WLAN BSS type PREF

SIDE EFFECTS
  None

===========================================================================*/
sys_wlan_bss_e_type cmph_map_cm_wlan_bss_type_pref_to_sys_pref
(
  cm_wlan_bss_type_pref_e_type cm_pref
)
{

  switch(cm_pref) {

    case CM_WLAN_BSS_TYPE_PREF_ADHOC:  return SYS_WLAN_BSS_TYPE_ADHOC;
    case CM_WLAN_BSS_TYPE_PREF_INFRA:  return SYS_WLAN_BSS_TYPE_INFRA;
    case CM_WLAN_BSS_TYPE_PREF_ANY:    return SYS_WLAN_BSS_TYPE_ANY;

    default:
      CM_CASE_ERR("Invalid cm_bss_type_pref! - %d",cm_pref,0,0);
      return SYS_WLAN_BSS_TYPE_ANY; /*lint !e527 */
  }

}

/*===========================================================================

FUNCTION cmph_map_cm_wlan_tech_pref_to_sd_pref

DESCRIPTION
  Map CM WLAN TECH Pref to SD WLAN Tech Pref.

DEPENDENCIES
  none

RETURN VALUE
  The SD type that corresponds to the input CM WLAN TECH PREF

SIDE EFFECTS
  None

===========================================================================*/
sd_ss_tech_pref_e_type cmph_map_cm_wlan_tech_pref_to_sd_pref
(
  cm_wlan_tech_pref_e_type cm_pref
)
{

  switch(cm_pref) {

    case CM_WLAN_TECH_PREF_A:       return SYS_TECH_PREF_MASK_80211A;

    case CM_WLAN_TECH_PREF_B:       return SYS_TECH_PREF_MASK_80211B;

    case CM_WLAN_TECH_PREF_G_ONLY:  return SYS_TECH_PREF_MASK_80211G;

    case CM_WLAN_TECH_PREF_G:       return (SYS_TECH_PREF_MASK_80211G |
                                            SYS_TECH_PREF_MASK_80211B);

    case CM_WLAN_TECH_PREF_ANY:
    default:
         return ( SYS_TECH_PREF_MASK_80211G |
                  SYS_TECH_PREF_MASK_80211A |
                  SYS_TECH_PREF_MASK_80211B);
  }

}

#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */

/*===========================================================================

FUNCTION cmph_set_net_lists_present_flag

DESCRIPTION
  Set is_net_lists_present flag.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  Modifies is_net_lists_present flag.

===========================================================================*/
void cmph_set_net_lists_present_flag
(
  boolean flag
)
{
  cmph_s_type                *ph_ptr = cmph_ptr();
      /* Pointer to a phone object */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(ph_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_ptr->is_net_lists_present = flag;

  return;
}

/*===========================================================================

FUNCTION cmph_read_manual_wlan_sys

DESCRIPTION
  Read WLAN Manual system from NET.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmph_read_manual_wlan_sys_profile
(
  cm_wlan_pref_s_type      *wlan_ptr,
  net_profile_s_type       *profile_ptr
)
/*lint -esym(818, wlan_ptr) */
/*lint -esym(818, profile_ptr) */
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   #ifdef FEATURE_WLAN
#error code not present
   #else

   SYS_ARG_NOT_USED(wlan_ptr);
   SYS_ARG_NOT_USED(profile_ptr);

   #endif /* FEATURE_WLAN */

   return;
}

/*===========================================================================

FUNCTION cmph_write_manual_wlan_sys_profile

DESCRIPTION
  Write WLAN Manual system into NET.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmph_write_manual_wlan_sys_profile
(
  const cm_wlan_pref_s_type      *wlan_ptr,
  net_profile_s_type       *profile_ptr
)
/*lint -esym(818, wlan_ptr) */
/*lint -esym(818, profile_ptr) */
{
  #ifdef FEATURE_WLAN
#error code not present
  #else

  SYS_ARG_NOT_USED(wlan_ptr);
  SYS_ARG_NOT_USED(profile_ptr);

  #endif /* FEATURE_WLAN */

  return;
}

/*===========================================================================

FUNCTION cmph_get_net_parser_ptr

DESCRIPTION
  None

DEPENDENCIES
  none

RETURN VALUE
  Pointer to NET parser

SIDE EFFECTS
  None

===========================================================================*/
net_file_parser_s_type* cmph_get_net_parser_ptr(void)
{
  static net_file_parser_s_type  parser;

  return &parser;
}

/*===========================================================================

FUNCTION cmph_get_net_wlan_file_buf

DESCRIPTION
  None

DEPENDENCIES
  none

RETURN VALUE
  Pointer to char buffer

SIDE EFFECTS
  None

===========================================================================*/
byte* cmph_get_net_wlan_file_buf(void)
{
  static byte buf[NET_MANUAL_SYS_FILE_SIZE];

  return &buf[0];
}

/*===========================================================================

FUNCTION cmph_get_wlan_profile_ptr

DESCRIPTION
  None

DEPENDENCIES
  none

RETURN VALUE
  Pointer to WLAN Manual System Profile.

SIDE EFFECTS
  None

===========================================================================*/
net_profile_s_type* cmph_get_wlan_profile_ptr(void)
{
  cmph_s_type                *ph_ptr = cmph_ptr();

  CM_ASSERT(ph_ptr  !=  NULL );

  return  &ph_ptr->wlan_pref.profile;
}

/*===========================================================================

FUNCTION cmph_cm_map_band_pref_to_sd_band_pref

DESCRIPTION
  Map CM band-preference type to SD band-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The SD band-preference that corresponds to the input CM band-preference.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_band_pref_e_type   cmph_map_cm_band_pref_to_sd_band_pref(

    cm_band_pref_e_type    band_pref
        /* Band pref to map */
)
{
  /*
  ** Map CM band-preference type to SD band-preference type.
  ** Note that unsupported CM band-preferences are mapped
  ** to SD_SS_BAND_PREF_ANY.
  */

  if(band_pref == CM_BAND_PREF_ANY)
  {
    return SD_SS_BAND_PREF_ANY;
  }
  else
  {
    return ((sd_ss_band_pref_e_type)band_pref);
  } /* switch */

} /* cmph_map_cm_band_pref_to_sd_band_pref() */


/*===========================================================================

FUNCTION cmph_map_sd_band_pref_to_cm_band_pref

DESCRIPTION
  Map SD band-preference type to CM band-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The CM band-preference that corresponds to the input SD band-preference.

SIDE EFFECTS
  none

===========================================================================*/
cm_band_pref_e_type  cmph_map_sd_band_pref_to_cm_band_pref(

    sd_ss_band_pref_e_type    band_pref
        /* Band pref to map */
)
{
  /*
  ** Map CM band-preference type to SD band-preference type.
  ** Note that unsupported CM band-preferences are mapped
  ** to SD_SS_BAND_PREF_ANY.
  */

  if(band_pref == SD_SS_BAND_PREF_ANY)
  {
    return CM_BAND_PREF_ANY;
  }
  else
  {
    return ((cm_band_pref_e_type)band_pref);
  } /* switch */

} /* cmph_map_sd_band_pref_to_cm_band_pref() */


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
/*===========================================================================

FUNCTION cmph_otasp_update_mode

DESCRIPTION
  Check the mode in NV and update the mode

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

  CM will not write this value to NV.  It is expected that this value
  has been written to NV already.

RETURN VALUE
  none

SIDE EFFECTS
  Phone mode may be changed to reflect the NV item

===========================================================================*/
void cmph_otasp_update_mode(

   cm_nam_e_type        nam,
   /* nam for which this mode is set for */

   nv_mode_enum_type    nv_mode
   /* mode to set to */
)
{
  cm_mode_pref_e_type    mode_pref;
  cmph_s_type           *ph_ptr = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the nam given matches the current nam then update the mode
  ** otherwise don't need to do anything right now.  We'll pick up
  ** the mode later if we switch nams. But go ahead and print out an error.
  */
  if(nam == ph_ptr->curr_nam)
  {
    mode_pref = cmph_map_mode_pref_nv2cm(nv_mode);
    if( mode_pref != ph_ptr->mode_pref )
    {
      /* Change the mode and inform MC.
      */
      {
        (void) cmph_force_pref_on_the_fly(
                      CM_SS_MAX,
                      ph_ptr,
                      SD_SS_PREF_REAS_ORIG_END,
                      CM_ACT_TYPE_NONE,
                      CM_PREF_TERM_PERMANENT,
                      mode_pref,
                      CMPH_PRST_BAND_PREF(ph_ptr),
                      CMPH_PRST_PRL_PREF(ph_ptr),
                      CMPH_PRST_ROAM_PREF(ph_ptr),
                      CMPH_PRST_HYBR_PREF(ph_ptr),
                      ph_ptr->plmn,
                      CMPH_PRST_SRV_DOMAIN_PREF(ph_ptr),
                      CM_OTASP_ACT_CODE_NONE,
                      &ph_ptr->wlan_pref,
                      ph_ptr->acq_order_pref,
                      CM_NETWORK_SEL_MODE_PREF_NONE,
                      (cm_act_id_type)ph_ptr,
                      CM_ACT_UPDATE_REAS_ACT_END,
                      FALSE);
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Send event reports to all registered clients on change in mode.
      */
      cmph_event( ph_ptr, CM_PH_EVENT_SYS_SEL_PREF );
    }
  }
  else
  {
    MSG_ERROR("Otasp Update Mode - NAM mismatch (%d and %d)", ph_ptr->curr_nam, nam, 0);
  }
} /* cmph_otasp_update_mode() */
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

#if defined( FEATURE_HICPS_STACK_SYNC_UP ) || defined( FEATURE_BSR2)
/*===========================================================================

FUNCTION cmph_kick_hdr_if_pending

DESCRIPTION
  Kick Hybrid stack if a kick is pending with the initial intended values.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  Phone mode may be changed to reflect the NV item

===========================================================================*/
void cmph_kick_hdr_if_pending()
{
  cmph_s_type             *ph_ptr          = cmph_ptr();
  cm_orig_q_s_type        *stack_top_ptr   = cmtask_orig_para_get_top( CM_SS_HYBR_1 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED( stack_top_ptr );

  CM_ASSERT( ph_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined( FEATURE_HICPS_STACK_SYNC_UP )
  if (*cmph_hdr_kick_pending())
  {
    cmpref_proc_cmd_pref_sys_chgd
    ( 
      SD_SS_HYBR_HDR,
      ( cmcall_is_hybr_hdr_orig()?
             SD_SS_PREF_REAS_ORIG_START_PS:
             SD_SS_PREF_REAS_USER ),
      ( ph_ptr->int_hybr_pref ==
             SD_SS_HYBR_PREF_CDMA__HDR ?
             SD_SS_ORIG_MODE_COLLOC :
             SD_SS_ORIG_MODE_NONE ),
      cmph_map_cm_mode_pref_to_sd_mode_pref( ph_ptr->mode_pref ),
      cmph_map_cm_band_pref_to_sd_band_pref( ph_ptr->band_pref ),
      cmph_map_cm_prl_pref_to_sd_prl_pref( ph_ptr->prl_pref ),
      cmph_map_cm_roam_pref_to_sd_roam_pref( ph_ptr->roam_pref ),
      ph_ptr->int_hybr_pref,
      cmph_map_otasp_act_code_to_sd_band(CM_OTASP_ACT_CODE_NONE),
      cmph_map_otasp_act_code_to_sd_blksys(CM_OTASP_ACT_CODE_NONE),
      SD_SS_AVOID_SYS_IDLE,
      0,
      ph_ptr->plmn,
      cmph_map_cm_srv_domain_pref_to_sd(ph_ptr->srv_domain_pref ),
      cmph_map_cm_acq_order_pref_to_sd(ph_ptr->acq_order_pref  ),
      NULL
    );
  }
  #endif

  #if defined( FEATURE_BSR2) && !defined( FEATURE_HICPS_STACK_SYNC_UP )
  if ( *cmph_hdr_kick_pending()||
       stack_top_ptr->act_type != CM_ACT_TYPE_PH_OBJ)
  {
    cmpref_proc_cmd_pref_sys_chgd( SD_SS_HYBR_HDR,
                                     ( cmcall_is_hybr_hdr_orig()?
                                          SD_SS_PREF_REAS_ORIG_START_PS:
                                          SD_SS_PREF_REAS_USER ),
                                     ( stack_top_ptr->orig.orig_int_hybr_pref ==
                                          SD_SS_HYBR_PREF_CDMA__HDR ?
                                          SD_SS_ORIG_MODE_COLLOC :
                                          SD_SS_ORIG_MODE_NONE ),
                                     cmph_map_cm_mode_pref_to_sd_mode_pref( 
                                       stack_top_ptr->orig.orig_mode_pref ),
                                     cmph_map_cm_band_pref_to_sd_band_pref( 
                                       stack_top_ptr->orig.orig_band_pref ),
                                     cmph_map_cm_prl_pref_to_sd_prl_pref( 
                                       stack_top_ptr->orig.orig_prl_pref ),
                                     cmph_map_cm_roam_pref_to_sd_roam_pref( 
                                       stack_top_ptr->orig.orig_roam_pref ),
                                     stack_top_ptr->orig.orig_int_hybr_pref,
                                     cmph_map_otasp_act_code_to_sd_band(
                                       CM_OTASP_ACT_CODE_NONE),
                                     cmph_map_otasp_act_code_to_sd_blksys(
                                       CM_OTASP_ACT_CODE_NONE),
                                     SD_SS_AVOID_SYS_IDLE,
                                     0,
                                     ph_ptr->plmn,
                                     cmph_map_cm_srv_domain_pref_to_sd(
                                       stack_top_ptr->orig.orig_srv_domain_pref ),
                                     cmph_map_cm_acq_order_pref_to_sd(
                                       ph_ptr->acq_order_pref ),
                                     NULL
                               );

  }
  #endif

  *cmph_hdr_kick_pending() = FALSE;
  return;
}  
#endif

/*===========================================================================

FUNCTION cmph_map_sys_mode_to_srv_type

DESCRIPTION
  Maps phone's sys_mode to service type.

DEPENDENCIES
  CM must be initialized

RETURN VALUE
  srv_type of teh call, else NONE

SIDE EFFECTS
  None

===========================================================================*/

cm_srv_type_e_type cmph_map_sys_mode_to_srv_type
(
         sys_sys_mode_e_type sys_mode
)
{
  cm_srv_type_e_type srv_type = CM_SRV_TYPE_NONE;

  CM_ASSERT (BETWEEN(sys_mode, SYS_SYS_MODE_NONE, SYS_SYS_MODE_MAX));

  switch (sys_mode)
  {
    case SYS_SYS_MODE_AMPS:
      srv_type = CM_SRV_TYPE_AMPS;
      break;
 
    case SYS_SYS_MODE_CDMA:
      srv_type = CM_SRV_TYPE_CDMA_AUTOMATIC;
      break;

    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_GW:
      srv_type = CM_SRV_TYPE_GSM_WCDMA;
      break;

    case SYS_SYS_MODE_HDR:
      srv_type = CM_SRV_TYPE_CDMA_HDR;
      break;

    case SYS_SYS_MODE_WLAN:
      srv_type = CM_SRV_TYPE_WLAN;
      break;

    default:
      break;

  }

  return srv_type;
}
