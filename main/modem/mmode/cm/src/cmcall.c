/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   C A L L   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Call Object for WCDMA/GSM.

  The Call Object is responsible for:
  1. Processing clients' call commands.
  2. Processing Lower Layer replies for clients' call commands.
  3. Processing Lower Layer call related notifications.
  4. Notifying the client list of call events.


EXTERNALIZED FUNCTIONS

  cmcall_init
    Initializing the call object.

  cmwcall_info_get
    Copy the current call state information into a specified buffer.

  cmcall_client_cmd_proc
    Process clients' call commands.

  cmcall_timer_proc
    Process call timer events


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 1991 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmcall.c#18 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/26/09   ks      added default values for ext_max_bit_rate_uplink and 
                   ext_gtd_bit_rate_uplink fields of qos structure
06/01/09   rk      Reverting the change for unreserve connection id only if orig 
                   req already not sent to NAS.
05/30/09   aj      Adding support for FEATURE_OTASP_STATUS
05/27/09   aj      Adding support for passing otasp status to clients
05/14/09   aj      Code cleanup in cmcall_proc_srv_status
05/11/09   mh      Remove WLAN for SR after CSNA hand-down
04/24/09   rk      For PS call Unreserve connection id only if orig req already 
                   not sent to NAS.
04/23/09   aj      Prevent referencing deallocated memory
03/26/09   ks      Added support for blocking MO/MT voice call on Data Card
03/20/09   ks      Removing FEATURE_CM_DELAY_SETUPRES functionality
                   to avoid tear down of sec pdp due to MT voice call. 
03/09/09   rm      Made cmcall_misc_get_common_mode_pref non-static
02/23/09   rk      Fixing silent redial issue by restoring ok to orig value
                   after scheduling next redial.
02/12/09   mh      Added support for no SR after MAP for IS-707B
01/26/09   fj      Added FEATURE_MM_SUPERSET for data structures
01/29/09   fj      Remove FEATURE_HDR_HYBRID for call events
12/28/08   rn      FDN checking option in call control
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
11/13/08   aj      Added API cmcall_end_each_call_with_type
10/31/08   rn      On going offline, send PDP_ABORT to NAS
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
10/15/08   rn      Corrected max line chars
10/09/08   fj      Fixed klocwork defects
10/01/08   sv      Corrected featurization in cmcall_gsdi_card_status_ind_proc
                   to remove lint errors.
09/10/08   ak      Fixed scenario of data call drop during GSM scan
09/05/08   sv      Memory Reduction for ULC
08/25/08   ak      Fixed the general compiler warnings.
08/20/08   st      Resolve Klockwork defects
08/21/08   rk      Scheduling the SR for E911 call after AS or MM reject.
08/08/08   pk      Not changing the srv_domain pref and gw_acq_order for
                   Silent redials
08/05/08   aj      Added api cmcall_check_for_call_state_on_ss()
06/12/08   sv      Resolve Klocwork defects
06/10/08   aj      For a ps data call in hybrid mode with uoos, force
                   preferences on both main and hybrid stack
06/06/08   rn      Added cmcall_overall_ps_call_end()
06/06/08   sv      Removing unneccesary snd.h
05/28/08   ks      Updating the associated primary profile number when
                   secondary PDP is promoted to Primary
05/22/08   vk/sv   added functionality to handle roam only preference
05/12/08   sg      AMR WB vocoder support for GERAN
05/08/08   sv      Featurizing snd.h under CDMA
04/29/08   ks      Reverting the changes of Updating the associated primary
                   profile number when secondary PDP is promoted to Primary
03/31/08   ks      Updating the associated primary profile number
                   when secondary PDP is promoted to Primary
03/13/08   sk      Fix to allow CDMA scans during dormant GW PS data calls
                   without tearing down the PS data call.
03/05/08   vk      corrected code for HYBR_GW configuration
03/05/08   vs      Handle CM_CALL_MODE_INFO_NONE for HDR call in
                   CM_ACQ_FAIL_F report
02/21/08   rm      Added code for not tearing down secondary PDP when
                   a VoIP call comes
12/13/07   sk/ak   Added DCM.
12/12/07   rm      For - Adding RAT-balancing feature support
                   Correcting the error caused by ens_alpha_string.
12/12/07   th      Addition of CM_CALL_EVENT_END to wake up events in
                   cmcall_dem_is_apps_wakeup_from_suspend_event()
12/07/07   ra      Added support for IMS VCC in HOMER files
11/26/07   sv      Removing Feature FEATURE_GW_CNAP_CODING_SCHEME
                   and add initialization to enc_alpha.coding_scheme
11/19/07   rn      Added support for UE initiated dormancy
11/05/07   sg/vs   Renaming FEATURE_CM_TEMP_NAS_DEP_TBD To
                   FEATURE_GW_CNAP_CODING_SCHEME
10/31/07   rm      Modifying- CM attempts silent-redial when call is still
                   in progress in NAS.
10/01/07   rm      Handling of GSDI_CARD_ERR_POLL_ERROR in CM
09/10/07   rn      Updated field enc_alpha of cmcall_s_type to send
                   coding scheme
08/07/07   ic      Lint clean up
06/22/07   sk      DDTM rewrite and added support for ATOM.
06/14/07   pk      Added support for power suspend
05/11/07   rp/cl   Deallocating cmd_ptr if call_ptr is already deallocated
                   in call control complete call back function.
04/17/07   rp      Making cmcall_get_favored_srv_domain_pref() public
04/13/07   rp      Filtering non-voice calls in cmcall_incom_proc
04/13/06   rp      Returning correct cmd_err from cmcall_process_orig_mode
03/09/07   sk      RAM usage reduction.
02/02/07   pk      Featurizing cmcall_get_voip_call_id under FEATURE_IP_CALL
02/01/07   pk      Removed extra cmcall_client_cmd_err from
                   cmcall_send_mode_spec_end
01/22/07   rp      Support for copying result from cc to call_info_ptr
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
12/19/06   pk      Modified forcing of preferences for PS_DATA calls
11/17/06   jqi     Added support for HDR active protocol revision information.
11/17/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/16/06   pk      Lint Fixes
11/11/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
10/12/06   ka/rp   Rejecting call originations when Phone is waiting for
                   LPM_OK
10/03/06   ka      Adding support for IP call on WCDMA
09/14/06   ka      Adding support for call control on secondary ps calls
08/28/06   ka      Adding pdp header comp and data comp enums.
08/25/08   ka      Adding support for delaying setup response to let QTV
                   PAUSE streaming session.
08/22/06   ic      Unused variables compiler warning fixes
08/16/06   ic      Lint fixes
08/11/06   pk      Added support for UOOS
08/11/06   ka      Modifying cmcall_check_if_gw_cs_call_active() to check
                   for calls in any state.
08/08/06   sk      Added redial action CMCALL_ACT_FORCE_NON_IP.
                   Made sure mode_usage computed in cmcall_client_orig_proc()
                   is stored in the call object (field main_ss_mode).
                   Added cmcall_remove_mode_and_force()
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
                   VoIP. Forcing force_top to TRUE for VoIP calls.
07/19/06   ic      Lint fixes
07/18/06   ka      Increasing silent redial time for AS_REJ_LOW_LEVEL_FAIL.
                   Fixing the issue where call cmd cb for call end api was
                   being called more than once.
07/07/06   pk      Removed cmcall_add_cmd_to_hold_q
07/06/06   pk      Added support for FEATURE_CM_TRIBAND_ORIG_PLAN_TBL
07/03/06   jqi     Only send the data avail when there is a PS call in orig
                   state and the data is suspend.
07/06/06   ka/ic   Featurization fixes for PS call control
07/04/06   ka      Changes to support call control on PS calls.
07/03/06   ic      Lint fixes in cmcall_gstk_evt_cb
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
06/15/06   sk      Fixed memory leak that occurred when number classification
                   is out of memory and returns FALSE.
06/05/06   ka/ic   Adding cmcall_is_gw_cs_call_type(). Increasing silent
                   redial for AS_REJ_LOW_LEVEL_FAIL under GCF NV flag.
06/02/06   pk      For CMCALL_ACT_WAIT_OTHER_END setting the orig sent on to
                   NONE
05/24/06   pk      Changed FEATURE_VOICE_CALL_LOW_POWER to FEATURE_DCVS_LPM
05/10/06   ka      mode_spec_end used to bunch calls of particular info_type
                   and then send them to wcall, ipcall or xcall. Modifying
                   wcall, ipcall and xcall to end calls of their typeonly.
05/08/06   pk      Modified cmcall_is_there_a_cdma_call_in_conv, added check
                   for sys_mode
05/03/06   pk      Lint Fixes
05/02/06   pk      Modified the featurization for clk regime function calls
04/20/06   pk      Added clk_regime_register_for_cpu_resource and
                   clk_regime_deregister_for_cpu_resource
04/19/06   pk      Added meta comments for 7200
04/14/06   pk      Added support for CMCALL_ACT_WAIT_OK_TO_ORIG_SHORT
04/12/06   pk      Initialized the cmcall_mode_info.info_type for the GW
                   calls in cmcall_process_orig_mode to the correct type
04/03/06   pk      If HDR starts throttling during a Silent Redial, give it
                   one more chance
                   Once the system is throttled, don't send call end
                   to clients immediately, buffer the call end for 1 second
04/03/06   jqi     Mainline FEATURE_SILENT_REDIAL_20
04/01/06   ka      Changes for emerg serv category during orig, aborting
                   manual search and returning current band with srv ind.
03/27/06   pk      Modified cmcall_get_favored_srv_domain_pref to return
                   CS_ONLY or PS_ONLY depending on the call type
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
03/14/06   pk      Added support for high PSIST
03/13/06   sk      End Hold orig period when we are in a non-usable HDR
                   system and we are faking no service to clients.
03/02/06   pk      Modified redial actions for KDDI, based on the latest
                   requirements
03/01/06   pk      Corrected cmcall_hybr_redial for the case when SD was not
                   forced because a more preferred acquisition was going and
                   CM gets a acq_fail. The priority queue get fails and CM
                   forces the preferences by calling
                   cmcall_force_pref_on_the_fly
02/17/06   ka      Adding support for videoshare
01/30/06   sk      Added debug info to help catch CM cmd buffer memory leaks.
01/12/06   ka      Removing error fatal when all call ids get used up.
                   Handling call end request with incorrect info_type.
01/12/06   ic      Trimmed to minimum the list of lint errors that were
                   suppressed for va_list, va_start(), va_arg(), va_end()
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/09/06   ic      Adjusted for cm_num_init() / cm_num_copy() changes in cm.h
01/06/06   ka      Increasing silent redial timer for AS_REJ_LOW_LEVEL_FAIL
                   based on GCF NV setting.
01/06/06   ic      Lint cleanup
12/13/05   ic      To fix compiler warnings, put cmcall_check_if_orig_thr*()
                   functions under FEATURE_CALL_THROTTLE
12/11/05   ic      Lint cleanup
12/09/05   ic      Lint cleanup
11/23/05   ka      Modification to allow hold/active for ip calls.
11/17/05   pk      Added checks for 1X data calls while SMS is waiting for
                   confirmation
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
10/27/05   pk      Added support for CM_CALL_EVENT_CALL_ORIG_THR
10/23/05   jqi     Hold ps data call orig or data avail request if there is a
                   pending get network list request.
10/21/05   sk      Removed stripping of the "+" from the start of a number
                   and the modification of the digit_mode.
10/20/05   pk      Cleaned RVCT compiler warnings and added mapping functions
10/20/05   pk      Added support for CM_MODE_PREF_INTERSECT_OR_FORCE
10/20/05   pk      Added support for CM_CALL_END_ACCESS_BLOCK_ALL
10/20/05   sk      Added hook in cmcall_event() to estimate base station IDM.
10/18/05   pk      Added check for cmpd_call_start, before forcing the mode
                   for the call
10/13/05   ic      Replaced INTLOCK / INTFREE with rex_enter_crit_sect() /
                   rex_leave_crit_sect()
10/02/05   ic      Corrected handling of end_params in call object.
09/14/05   pk      Modified cmcall_hybr_redial, if call origination
                   parameters are not found on the Queue, the call is ended
09/12/05   pk      Added check for GPS session clean up in call originations
                   GPS session related changes
09/08/05   ka      Support for VoIP over HDR on the Main instance.
08/26/05   pk      Modified cmcall_hybr_redial(), when switching to 1X,
                   the call_end reason in not immediately reset to NONE.
08/25/05   ic      NULL ptr check fix in cmcall_call_control_complete_cb()
08/23/05   sk      Removing support for data calls while in emergency
                   callback mode or in emergency call.
08/19/05   pk      Modified the handling of ACQ_FAIL end reason in
                   cmcall_orig_proc. The hold_orig time is reset only if we
                   have 1X service and the next_orig_time is not reset
08/16/05   pk      Added support for priority Queues for origination
                   parameters
08/16/05   ka      Adding support for voice call on HDR (using VoIP app) on
                   the hybrid instance. Support for VoIP on HDR in the Main
                   instance needs to be done.
08/12/05   ka      Functions to check voice/emerg in origination and decide
                   on call types.
08/11/05   ka      Added silent redial for AS_REJ_LOW_LEVEL_IMMED_RETRY
08/02/05   ic      Print value that caused error in default switch case
08/02/05   pk      Modified redial action table for HDR call failures.
                   For all connection deny call failures the redial action
                   is CMCALL_ACT_REDIAL_SHORT.
07/30/05   ka      Reverting modifications in cmcall_orig_start done for CCBS
07/29/05   dk      Added support to not let HDR call send pref_sys_Chgd while
                   sms activity is in progress.
07/27/05   ic      Removed from origination tables duplicate listings for
                   CM_CALL_END_NO_RESPONSE_FROM_BS and CM_CALL_END_INCOMPATIBLE
07/21/05   pk      Modified the printing of Silent redial count and the
                   loging of the redial event
07/21/05   sk      Removed internal use of is_jcdma_emergency flag.
                   Changed mode_pref_for_orig_table so that hybrid is not
                   turned off during an emergency call for JCDMA targets.
07/12/05   ic      Print call type that caused error in switch default in
                   cmcall_is_call_type_valid()
07/11/05   pk      Modified call to cmss_check_if_orig_thr_true(), added
                   packet_zone id as additional parameter
07/08/05   pk      Modified the silent redial behavoir for HDR
                   Added helper functions for the redial enhancements
07/08/05   ic      Report highest layer call origination failure as part of
                   call events.
                   Removed dead code
06/28/05   pk      Modified cmcall_check_if_orig_thr_and_add_entry().
                   In HDR only mode, throttle table entry is not made for
                   DRS = 0 call failures
06/27/05   ic      Print call_id that cmcall_ptr() failed to look up
06/27/05   ic      Removed misleading comment from cm_call_id_allocate()
06/26/05   ic      Fixed Lint and compiler warnings
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/21/05   dk      Added cmcall_is_active_on_mode() to find if there is any
                   call on a given mode.
06/13/06   ic      Fixed Lint warnings - removed external declaration of
                   cm_unreserve_connection_id()
06/10/05   ic      Lint clean up
06/09/05   ic      Removed all varieties of EJECT comment
                   Cleaned up cmcall_cmd_copy_fields()
06/08/06   pk      Modified cmcall_check_if_orig_thr(). Not throttling for
                   DRS=0 even for DO systems.
06/06/05   pk      Added call throttling support. Modified cmcall_orig_proc.
06/03/05   ic      Mainlined FEATURE_SILENT_REDIAL_20
                   Removed FEATURE_FAVOR_REV_LINK_ROAM_IND
05/18/05   ka      Merging CIPHER_IND feature and GCF fix.
05/17/05   sk      Allow data calls while in emergency callback mode when
                   SVD is on.
05/04/05   jqi     Fixed for hybr data switch over.
04/18/05   dk      Changed Silent Redial action from REDIAL_SHORT to
                   REDIAL_SHORT_SAME_SR for HOLD_DBM_IN_PROG
04/12/05   ic      Removed FEATURE_HDR_HICPS20 featurization from processing
                   of CM_ACQ_FAIL_F
04/06/05   ka      Adding ASSERT's and header files
03/31/05   ic      Fixed Lint warning (negative indentation)
03/24/05   ic      Fixed processing of CM_CALL_CMD_INFO_GET (CR#59955)
                   Rewrote cmcall_client_cmd_proc to use switch()
03/14/05   ka      Handling CM_CALL_MODE_INFO_IP
02/24/05   dk      Added cmcall_count_on_ss() function to address CR 57350.
02/24/05   ic      Removed cmcall_overall_call_state_get()
02/23/05   pk      Changed assertion CM_MSG_ERROR to CM_ERR in function
                   cmcall_client_orig_proc() to reproduce the CR# 53864
                   This test case verifies the integrity of the call ids i.e
                   verifies the allocation and deallocation routines for
                   call ids.
02/22/05   ka      Changing is_user_data_present flag value in cmcall_event()
02/15/05   pk      Initialized the call event info buffer
                   Initialized the cmcall_mode_info.info_type for the GW calls
                   to the correct type
02/11/05   jqi     Check if Hybird is allowed in loading the orig plan for
                   none hybird mode.
02/09/05   ic      Lint cleanup
02/03/05   dk      Allow PS_DATA call while SMS call is in progress in
                   SVD.
02/01/05   dk      Added WLAN Support.
01/24/05   ic      Replaced cmph_is_ph_in_emerg_cb_mode() with
                   cmph_is_in_emergency_cb()
01/10/05   sj      Adding call failure handling for no hdr service & no hdr
                   rf lock.
12/20/04   ic      Fixed RVCT 2.1 compiler warning 'variable "client_id" was
                   set but never used'.
                   Fixed lint warning related to CM_DEBUG being off on target
12/19/04   ic      Added cmcall_assert() and cmcall_info_get()
                   In cmcall_reset() initialized fields favored_prl_pref and
                   favored_roam_pref fields
12/14/04   sj      Moved "determine_ss.." after xcall & wcall check fns.
12/13/04   sj      Always start with call_id allocation of 0.
12/02/04   dk      Copy call substate in cmcall_get_call_info_list_proc().
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
11/20/04   ic      Fixed typo found at lint suppression verification (list to
                   lint)
11/20/04   ic      Fixed compiler warning in cmcall_call_cmd_answer_para_check(),
                   removed unnecessary Lint comment, added assertion
11/19/04   ka      Adding support for user to user signaling
11/18/04   ka      Adding support for MT PDP feature
11/15/04   ic      Lint cleanup with CM_DEBUG on
11/11/04   ic      Formatting fixes only
11/09/04   dk      Increased Silent Redial Next Orig Uptime to 5 seconds for
                   LOW_LEVEL_FAIL error code for GW Voice Call Failure. This
                   is required to address issue in CR 50990.
11/04/04   sj      Fixed issue in force_pref_on_the_fly() preferences
                   computation during 4 secs unforce period.
10/21/04   ic      Fixed code that caused e774 Lint complaints
10/18/04   ka      Adding call origination plan for PS calls with
                   CMCALL_PS_DATA_SR_MAX_TIME redial time.
10/19/04   jqi     Sent the data available request upon the ps call orig
                   when the data is suspended.
10/11/04   ka      For concurrent calls using system selection corresp to
                   the call to check for orig_mode.
10/11/04   ka      Added gw_voice_init, gw_ps_init and cdma_init functions.
10/10/04   sj      Fixed issue with ending the hold orig upon no srv.
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/14/04   ic      Call direction clean up
09/08/04   jqi     More ps dormant optimization.
09/07/04   ka      Setting orig_sent_on_mode to None when call gets rejected by lower
                   layers and as a consequence CM decides to redial. Replacing
                   is_orig_sent and mode_in_use with orig_sent_on_mode
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
08/30/04   sj      Added orig_mode in call object and subsequent changes for
                   it. Call determine_ss after CC is over.
08/27/04   jqi     Added ps dormant optimization.
08/26/04   dk      Fixed CR 47179. Added Redial for CC Cause CNM_MM_REL_PENDING
                   and removed redial for TEMPORARY FAILURE.
08/20/04   sj      Added support for PRL Preference in sys_sel_pref().
08/19/04   dk      Added check for SMS calls while making 1X or HDR data calls
                   in cmcall_orig_proc().
08/18/04   ka      Call direction now is common to system modes.
                   Setting call direction using a function.
08/17/04   ic      Undoing P4 change list #121638
08/11/04   ka      Updating call system mode for incoming origination.
08/09/04   ic      Lint clean up for SNAR configuration
08/09/04   ws      Delay fatal error when running out of call/connection ids.
08/05/04   dk      Kick the GSDI at the call start for CDMA. Remove the check
                   for UIM presence during call.
08/04/04   dk      Added support to hold Packet Data Call origination in HDR
                   if SMS/DBM is in progress.
08/02/04   ic      CM Lint clean up - cmcall.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/28/04   ka      Added support for secondary PDP context
07/30/04   sj      Added support for origination on camped_ind.
07/27/04   ws      Call Control support for SUPS.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added (void) in front of CM_DEC() macro to get rid of
                   ADS 1.2 compiler warning
                   Fixed ADS 1.2 compiler warning 'unsigned comparison with 0
                   is always true' by not using INRANGE() macro (casting its
                   parameters to int32 does the trick on RVCT 2.1 compiler but
                   not on ADS 1.2)
07/22/04   ic      Rewrote cmxcall_cdma_orig_possible() as generic
                   cmcall_misc_is_mode_pref().
                   Fixed cmcall_set_favored_mode_pref_for_origination() so
                   that DRS = 0 originations use correctly current IDM
                   (CR#45216)
07/21/04   ic/sj   Moved the call to call_ptr->ss =
                   cmcall_determine_ss_for_origination(...) from
                   cmxcall_client_orig_proc() in cmxcall.c to
                   cmcall_client_orig_proc() in cmcall.c so that
                   call_ptr->ss field gets correctly populated for GW calls.
07/21/04   sj      Added support for umts to 1x handover.
07/20/04   ka      Added function to clean up call id and connection id if
                   call origination cmd fails during one of its steps.
                   Using TEMPORARY_FAILURE instead of
                   RESOURCES_UNAVAILABLE_UNSPECIFIED for redial.
07/20/04   ic      Fixed RVCT 2.1 compiler warning
07/15/04   sj      Specified pref_reason as orig_start_cs or orig_start_ps
                   depending on type of call.
07/15/04   sj      Added logic to end call if we get ACQ_FAIL_F while in
                   limited service.
07/14/04   ws      Fixed GSTK dependencies.
07/12/04   ic      Added support for FEATURE_ALS
07/07/04   dk      Added wait for PS Signaling Down indication in
                   cmcall_orig_proc() for CR 43512.
07/06/04   dk      Modified signature of cm_find_connection_id().
07/06/04   ws      Delay 1x origination for hdr call end.
07/05/04   dk      Changed signature of cmcall_update_gsdi_and_log_call_state().
07/05/04   dk      Changed implementation of Call Object Iterator to refer
                   to the next in the Iterator Type.
07/01/04   dk      Added Call Object Iterator.
07/01/04   sj      Fixed issue with orig_failure no_session.
07/01/04   ws      Remove duplicate calls to client cmd callbacks.
06/30/04   ic      Fixed obvious RVCT 2.1 compiler warnings (Jaguar build)
06/30/04   dk      Beautify function header description of call_id_in_range
                   function.
06/30/04   dk      Lint Cleanup.
06/29/04   sj/ic   In cmcall_orig_proc() test call_ptr->mode_in_use instead of
                   sr_ptr->is_orig_sent to check whether origination was sent
                   (CR#45468)
06/25/04   dk      Modified call object so that call_obj_array just contains
                   the pointer to the call object. Moved all call relevent
                   information inside the call object.
                   Removed explicit call object allocation/de-allocation.
                   Now it is done as part of call_id allocate/deallocate.
                   Modified in general to support this new behavior.
                   Moved certain generic call obj array walk through functions
                   from cmxcall.c
06/22/04   ic      Comments clean up
06/21/04   ws      Mainlined FEATURE_GPSONE_DBM
06/16/04   ic      Restored cmcall_event() to not modify call object in any
                   way, moved all the modifications needed to outside of the
                   function.
                   Added cmcall_reset() to reset common call fields.
                   Cleaned up cmxcall_reset() to call cmcall_reset() and reset
                   only 1X specific fields.
                   Added calls to cmcall_update_call_end_info() before
                   cmcall_event() is called for CM_CALL_EVENT_END event.
                   Fixed obvious lint errors (-e715 - variable not referenced;
                   bad enum type used)
06/15/04   dk      Removed cmcall_update_overall_state() and added
                   cmcall_update_gsdi_and_log_call_state(). Modified
                   cmcall_overall_call_state to get rid of FEATURE
                   dependency.
06/10/04   dk      Added cmcall_check_each_call_in_call_state function.
06/09/04   dk      Initialized info_type in call object for MODE_ANY in
                   cmcall_client_orig_proc function. Also initialized
                   info_type in cmcall_callobject_allocate.
06/09/04   dk      Replaced mem_malloc with cm_mem_malloc to support CM Heap.
06/09/04   ka      Corrected call_end_time check
06/04/04   ka      Fixed Lint errors.
06/03/04   ic      Fixed featurization error in cmcall_gstk_evt_cb()
06/02/04   ka      Added function to check if a call of particular type is in
                   a particular state.
06/01/04   ws      Handle '+' dialing for 1x.
05/27/04   ic      Fixed featurization errors so it compiles without
                   FEATURE_HDR defined.
                   In cmcall_client_cmd_proc() call cmcall_client_cmd_err()
                   irrespective of whether error occured or not (client is
                   supposed to be always notified about command processing
                   status)
05/26/04   ka      Added shorter redial for CM_CALL_END_NO_GW_SRV and added
                   unreserving connection ids when silent redial fails
05/19/04   ka      System mode updated in calls for ORIG and IDLE states.
05/17/04   ic      Cleaned up cmcall_callobject_allocate() and
                   cmcall_client_orig_proc()
05/12/04   ka      Added check for client supplied call type when answering MT call.
05/10/04   ka      Fixed Lint errors.
05/06/04   ic      Removed sr_ptr->is_orig_sent = TRUE from cmcall_orig_proc()
                   as it needs to be set in cmwcall_send_orig() / cmxcall_send_orig()
                   respectively.
                   Added debug info for favored mode for origination computation /
                   forcing of origination mode.
                   Used cmcall_remove_mode_pref_components() to remove SD mode
                   preference components in cmcall_client_orig_proc()
04/30/04   ws      Merge FDN Restriction fixes from Leoaprd.
04/30/04   ws      num type and num plan modifications
04/29/04   ic      Fixes in cmcall_set_favored_mode_pref_for_origination() to
                   allow for origination of PS data calls in WCDMA, GSM and
                   WCDMA/GSM mode only.
                   Added support for CM_MODE_PREF_CURRENT_LESS_AMPS
                   Added assertions in cmcall_remove_mode_pref_components()
04/29/04   ic      Fixes in cmcall_set_favored_mode_pref_for_origination() to
                   allow for origination of PS data calls in WCDMA, GSM and
                   WCDMA/GSM mode only.
                   Added support for CM_MODE_PREF_CURRENT_LESS_AMPS
                   Added assertions in cmcall_remove_mode_pref_components()
                   In cmcall_gsdi_card_status_ind_proc() removed comments that
                   were not formatted correctly and caused stripsrc tool to break.
04/28/04   ka      Fixed Lint errors.
04/21/04   ic      In cmcall_orig_start() fixed change that speeds up
                   origination of the second call
04/08/04   ws      Merged from MSMSHARED_CM.01.00.32.00.03
04/06/04   jqi     Fixed a typo.
04/13/04   ka      Fixing delay in origination of second concurrent call.
                   Setting favored_mode_pref to EMERGENCY for emergency calls.
04/03/04   jqi     Populate favored service domain for both orig mode none
                   and normal.
04/02/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.23 and MSMSHARED_CM.01.00.10.00.28
04/01/04   ic      Changed calls to cmdbg_print_fav_mode_pref_for_origination()
                   as per function definition changes
04/01/04   ws      Fixed favored_mode_pref for GW.
04/01/04   ws      Added cmcall_ptr_callobject_allocate function.
03/31/04   ic      Mainlined FEATURE_REX_DYNA_MEM in CM and unit test.
                   Removed all code related to static allocation of call
                   objects.
03/30/04   jqi     Populated favored srv domain on every origination trial in
                   cmcall_force_pref_on_the_fly.
03/26/04   ic      Undoing changes to cmcall_init() from 03/24/04 as it was
                   not correct.
                   Fixed compiler warning - removed unused local variable
                   from cmcall_orig_proc()
03/26/04   ka      Added a new function for processing CMCALL_ACT_PROC_END_REASON
03/24/04   ic      In cmcall_force_pref_on_the_fly() replaced
                   ph_ptr->orig_srv_domain_pref with ph_ptr->srv_domain_pref
                   in call to cmph_force_pref_on_the_fly() as
                   ph_ptr->orig_srv_domain_pref is not supposed to be used
                   until actually set inside of cmph_force_pref_on_the_fly().
                   Removed unnecessary comments.
                   In cmcall_init() made sure we allocate memory only if
                   call_obj_array[i] == NULL.
03/23/04   ws      Merged chagnes from 6300 branch.
03/19/04   ka      Updating the current system mode for all calls in
                   CONVERSATION.
03/18/04   sj      Fixed featurization problems.
03/15/04   ka      Merged Changes from 6250 branch
                   Fixed call reject in case of MO back to back calls.
                   Use service domain form service indicator for call orig.
                   Fixed Lint errors
                   Propagated service domain for call origination.
                   Fixed forcing service domain on the fly for GW system.
                   Unreserve connection ids even if call is not originated.
                   Reject PS data call origination when MB is in GSM voice
                   call.
                   Fixed Lint errors.
03/12/04   jqi     Use service domain form service indicator for call orig.
03/10/04   jqi     Propagated service domain for call origination.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/13/04   ws      FEATURE_SILENT_REDIAL_20 support.
02/12/04   ic      Merged in changes from MSMSHARED_CM.01.00.10.00.20 to
                   MSMSHARED_CM.01.00.10.00.23
02/10/04   ws      Modified gsdi call to include mode with tc status.
02/09/04   ic      Changes so the code compiles for unit test.
02/06/04   ws      Initial Jaguar/Mainline merge.
01/29/04   ws      Modified gsdi call to include mode with tc status.
01/29/04   ws      Added function for handling gsdi events in both modes.
01/23/04   ws      Fixed bug with handling restricted calls.
01/21/04   RI      Modifications for attach on demand for GSM/WCDMA modes.
01/19/04   ka      Getting traffic in bytes from data services on call end.
                   Function to get call info list on CM command. Registration
                   function for data services to register a call back.
                   Multiple PDP support.
01/16/04   ws      Handle NULL gw_cc_modified_info for CDMA only calls.
01/13/04   ka      Merged changes from MSM6200 and CM Quad Band.
01/13/04   sj      Fixed bug in checking for sufficient service.
01/09/04   ws      Take a slot number when sending gstk voice envelopes.
12/16/03   rc/aaj  Support for FEATURE_HDR_HICPS20
12/16/03   ws      Return end status even if there is no mode yet.
                   Unreserve connection id regardless of call type.
                   Mapping between GSTK and CM NPI, TON types.
12/16/03   prk     Removed processing of CNM_REJ_NO_RESOURCES cause during
                   silent redial due to 11.1.2 test case failure.
12/15/03   tml     Added revision history comment
12/15/03   tml     Fixed call control
12/11/03   ic      Replaced is_hybr_redialled with is_hybr_redialed
12/11/03   ws      Check for call type instead of mode when sending otasp_act_code.
12/08/03   ws      Reset cmd to orig after call control.
11/06/03   ws      Check for rejected calls from call control.
11/03/03   ws      Fix LTK compile errors.
10/30/03   ws      Merged ACT_END_ORIG handling from Cheetah
                   Merged ACT_PROC_END_REASON handling from Cheetah
                   Picked up call type changes in cc processing.
10/20/03   ws      Dual Slot Support.
                   Added calls to cmcall_client_cmd_err for success cases too.
                   Added privacy_pref handling without call ids.
09/03/03   jds     Correct bug when SIM fails (or is removed) during an
                   emergency call (do NOT end the call).
08/08/03   RI      Modified allocation of call ID for an incoming call in GSM
10/06/03   vt      Modified to support common cm.h across all targets.
10/01/03   ws      Merged changes from GW Targets
09/23/03   ws      Merged Emergency call dropped when SIM card removed fix.
09/18/03   ws      Updated handling of proc_end_reason.
09/10/03   ws      Fixed problem where if no mode info, calls not ending correctly.
08/12/03   ws      Support for MM silent redial and automatic mode
07/22/03   vt      Fixed the function to call the right overall state function.
05/14/03   ic      Under FEATURE_JCDMA_CM set actions for CM_CALL_END_MC_ABORT
                   and CM_CALL_END_PSIST_NG to CMCALL_ACT_END_ORIG
04/22/03   prk     Modified cmcall_proc_srv_status so only GSM/WCDMA calls
                   are processed.
04/11/03   prk     Removed references to FEATURE_AOC and FEATURE_MMGSDI.
03/25/03   kar     Fixed compile error on LTK
03/21/03   RI      Modified cmcall_proc_srv_status() to end the PS data calls
                   if there is no service.
03/05/03   RI      Modified cmcall_check_if_cs_voice_calls_active() to check for
                   calls in the active state instead of idle state.
02/27/03   cs      Added support for PS Continuity.
02/27/03   ws      Fixed cmcall_answer_para_check range
02/25/03   RI      Added cmcall_check_if_cs_voice_calls_active() and
                   cmcall_check_if_ps_call_active().
                   Modified cmcall_proc_srv_status() for PS attach on demand.
02/24/03   vt      Fixed a compile problem with HDR.
02/18/03   vt      Fixed problems to build for HDR.
02/13/03   prk     Mainlined the FEATURE_CONC_SRVC_NEW_API feature.
02/11/03   prk     Modified cmcall_client_cmd_proc() to handle supplementary
                   service commands.
02/04/03   prk     Modified cmcall_proc_srv_status() to end the emergency call
                   if a no service status is received from the lower layer.
           cs      Fixed cm_gsdi_in_traffic to use overall call state.
           cs      Extended service state change handling for regular call
                   silent origination.
01/24/03   ws      Merged changes for E911 calls to not tell searcher that origination
                   is pending.
01/21/03   ws      Updated copyright information for 2003
12/17/02   vt      Fixed a INTLOCK bug and a typo.
11/20/02   vt      Changed returned error code on invalid call id in client's
                   command.
11/19/02   vt      Changed error fatal to just error when call id is invalid
                   for a client command.
11/14/02   vt      Merged changes from GW mainline.
                   [PRK]Re-featured support for invalid SIM handling so it get
                   compiled for WCDMA/GSM only.
                   [ws]Merged skipped resetting orig error count if it is OTHER (not L3).
                   [CS]Added support for invalid SIM handling. Terminate active
                   calls when SIM becomes invalid.
10/28/02   vt      Removed handling of NDSS origination failure.
10/22/02   ic      Under FEATURE_JCDMA_CM added special handling of
                   CM_CALL_END_REORDER call end in silent redial tables.
10/22/02   vt      Created a function to return call_id from call object ptr.
                   Removed is_allocated and call id from call object for
                   dynamic memory allocation.
10/21/02   vt      Replaced checking of call id with call object.
10/11/02   vt      Fixed couple of typo's for RUIM build.
10/09/02   vt      Decoupled call object reset from call id allocation.
                   Removed current mode determination functions.
10/05/02   vt      Modified to call the right handler(1x/GW) on orig command.
                   Added debug messages when allocating and deallocating a
                   call_id.
06/14/02   PRK     Removed initialization of call_type_ptr in
                   cm_find_connection_id.
06/11/02   RI      Modified (for GSM/WCDMA modes)
                    - cmcall_init() to check for call object array
                   allocation error.
                    - cm_new_incom_call_id() to set NULL ptr if call
                   object allocation fails.
                    - cm_call_id_allocate() to be called in different
                   modes for call ID allocation.
                    - cm_call_id_deallocate() to deallocate only the
                   call IDs.
                    - cmcall_client_cmd_check(). Deleted the call object
                   pointer parameter to this function.
                    - cm_find_connection_id() to set invalid values
                   for connection ID and call types if there is no
                   connection ID to call ID mapping.
                   Added cmcall_object_deallocate() to deallocate a
                   call object.
                   Renamed cm_new_call_id() to cm_new_connection_id().
05/28/02   RI      Fixed the debug message in cmcall_event() to print
                   the correct call event and call ID.
05/16/02   PRK     Checked the ptr returned in cmcall_ptr for null.  The
                   check was made in only the mode independent calls of
                   cmcall_ptr.
                   Modified cmcall_call_cmd_get_call_info_para_check to check
                   client_ptr and not client.
05/05/02   vt      Fixed compile problem with new mm cm.h
04/26/02   ATM     Added calls to cmlog_call_state().
03/11/02   RI      Used default value defintions for cause value and
                   CLIR from mn_cm_exp.h
02/19/02   PRK     Removed check of service state for CM_CALL_CMD_ANSWER
                   commands.  Client is allowed to answer incoming call with
                   no service.
02/06/02   PRK     Added function cmcall_overall_call_state.
01/28/02   PRK     Replaced cmutil_ prefix with cm_util_ prefix.
                   Modified cmcall_client_cmd_check to check is_subscription_-
                   available flag.
01/25/02   RI      Modified NotifySS processing for Call Forwarding
                   Removed the parameter check for N SEND and
                   N X SEND commands.
01/04/02   HD      Merge to Mainline
                   Do not reset call at end_req and copy cc_cause for Disc msg.
11/21/01   RI      Modified cmwcall_info_get() to copy rab_id for CS Data.
                   Modified PS Data ind/conf processing.
                   Modified cm_find_call_id() to include the connection ID src.
11/15/01   AT      Updated AOC to respond to MN.
11/06/01   RI      Modified cmwcall_cmd_copy_fields().
11/01/01   AT      Cleaning up AOC at end of call.
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                   Modified code due to srv_state moving from phone group
                   to serving system group (FEATURE_NEWSS).
10/17/01   AT      Added Support for Advice of Charge (AOC)
10/01/01   AT      Added check for E911 changes.
08/23/01   HD      NAS header file reorg changes.
04/27/01   HD      Initial release for WCDMA/GSM.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
#include "customer.h"  /* Customer configuration file */
#include "cm.h"        /* External interface to cm.c */
#include "cmi.h"       /* Internal interface to cm.c */

#include <string.h>    /* Interface to string library */
#include "rex.h"       /* Interface to rex services */
#include "comdef.h"    /* Definition for basic types and macros */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmph.h"      /* Interface to CM phone object */
#include "cmlog.h"     /* Interface to diag services */
#include "cmreply.h"
#include "cmtask.h"
#include "clk.h"       /* Interface to clock services */
#include "prl.h"       /* Interface to PRL services */
#include "cmcall.h"    /* Interface to CM call object */

#if( defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#include "cmxcall.h"   /* Interface to CM CDMA call object */
#include "cmxsms.h"    /* Interface to CM CDMA SMS */
#include "srch.h"      /* Interface to Search Task */
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

#ifdef FEATURE_UIM_RUIM
#include "cmnv.h"
#endif /* FEATURE_UIM_RUIM */

/*lint -esym(766,cmgsdi.h)*/
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
#endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

#ifdef FEATURE_GSTK
#include "cmtask.h"
#include "gstki.h"
#endif

#include "cmutil.h"
#include "cmss.h"
#include "cmpref.h"
#include "cmxdbm.h"

#ifdef FEATURE_HDR_HANDOFF
#error code not present
#endif

#ifdef FEATURE_MMGSDI_CDMA
#include "cmgsdi.h"
#endif

#ifdef FEATURE_GPSONE
#error code not present
#endif

#ifdef FEATURE_IP_CALL
#include "cmipcall.h"    /* CM IP call interface */
#include "cmipappi.h"    /* Interface to query registered apps */
#endif

#ifdef FEATURE_DCVS_LPM
#include "clkregim.h"
#endif

#if (defined(FEATURE_HYBR_GW) || \
     defined(FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X) || \
     defined(FEATURE_IMS_VCC))
#include "homeri.h"     /* Interface to handoff manager */
#endif

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif
#if ((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI))
#include "cmmmgsdi.h"
#endif /* ((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI)) */
/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

static cm_band_pref_e_type  cmcall_get_favored_band_pref(
    cm_call_type_e_type    call_type,
    cm_srv_type_e_type     srv_type,
    cm_band_pref_e_type    band_pref
);

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/
#define CMCALL_TIMER_INACTIVE                ((dword)(-1))
   /* This is the max value of the timer.  Indicates the timer is not
   ** currently used right now.
   */

/*---------------------------------------------------------------------------
                         TUNABLE CALL PARAMETERS
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Powerup-hold-origination feature to hold origination right after powerup
** giving service a chance to appear.
*/
#ifdef FEATURE_CM_PWRUP_HOLD_ORIG

#define CMCALL_PWRUP_HOLD_ORIG_UPTIME        12  /* seconds */
    /* Max uptime (i.e. time since powerup) to hold origination
    ** giving service a chance to appear right after powerup */

#else /* #ifdef FEATURE_CM_PWRUP_HOLD_ORIG */

#define CMCALL_PWRUP_HOLD_ORIG_UPTIME        0  /* seconds */

#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Preferred-service-hold-origination feature to hold origination after
** powerup or mode preference change giving preferred service a chance
** to appears once sufficient service is available.
*/
#ifdef FEATURE_CM_PREF_SRV_HOLD_ORIG

#define CMCALL_PREF_SRV_HOLD_ORIG_TIME       5  /* seconds */
    /* Max time to hold origination giving preferred service a chance
    ** to appears once sufficient service is available */

#else /* #ifdef FEATURE_CM_PREF_SRV_HOLD_ORIG */

#define CMCALL_PREF_SRV_HOLD_ORIG_TIME       0  /* seconds */

#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define CMCALL_ACQ_HOLD_SS_PREF_TIME         3  /* seconds */
    /* Max time to hold system selection preference change during origination
    ** in case system determination is currently trying to acquire service */

#define CMCALL_ACC_HOLD_ORIG_TIME            20  /* seconds */
    /* Max time to hold origination after receiving origination failure
    ** reason of CM_CALL_ORIG_ACC_IN_PROG (i.e., origination fails do to
    ** redirection, handoff, etc) */

#define CMCALL_MAX_CALL_OBJ      CM_CALL_ID_MAX

#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */

#if defined(FEATURE_GPSONE)
#error code not present
#else
#define CMCALL_PD_SESSION_ACK_HOLD_ORIG_TIME           0 /* seconds */
#endif /* FEATURE_GPSONE */

#if((defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) && \
     defined(FEATURE_CDSMS))

#define CMCALL_SMS_ACK_HOLD_ORIG_TIME                  6 /* seconds */
  /* Time to wait for SMS ack
  */
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* array of call objects */
static cmcall_s_type        *call_obj_array[CMCALL_MAX_CALL_OBJ];

#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
#endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */


static cm_get_data_session_total_byte_ctrs_cb_f_type
                    *cmcall_data_session_total_byte_ctrs_cb_func_ptr = NULL;
/* Pointer to the function that is called to get the total number of bytes transmitted in a data call */

#ifdef FEATURE_CM_PWRUP_HOLD_ORIG
#ifdef FEATURE_CM_PREF_SRV_HOLD_ORIG
#if defined( FEATURE_GSTK ) && (defined( FEATURE_GSM ) || defined( FEATURE_WCDMA ))
#error code not present
#endif
#endif
#endif

/**--------------------------------------------------------------------------
** Foward declerations
** -------------------------------------------------------------------------
*/

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */


/**--------------------------------------------------------------------------
** Functions - internal: silent redial
** --------------------------------------------------------------------------
*/




/*===========================================================================

FUNCTION sr_reset

DESCRIPTION
  Reset a silent redial component, so it is ready for next call.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void sr_reset(

    cm_sr_s_type  *sr_ptr
        /* pointer to silent redial object to reset */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sr_ptr != NULL );
  CM_MSG_LOW( "Silent redial reset, uptime=%ld",clk_uptime(),0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ptr->orig_plan_ptr              = NULL;
  sr_ptr->sr_start_uptime            = 0;
  sr_ptr->sr_max_uptime              = 0;

  sr_ptr->is_ok_to_orig              = FALSE;

  sr_ptr->orig_sent_on_mode          = SYS_SYS_MODE_NONE;
  sr_ptr->hold_orig_uptime           = 0;
  sr_ptr->next_orig_uptime           = 0;
  sr_ptr->sr_left                    = 0;

  #ifdef FEATURE_IS2000_REL_A
  sr_ptr->is_re_orig                 = FALSE;
  sr_ptr->re_orig_reason             = FALSE;
  sr_ptr->re_orig_count              = 0;
  sr_ptr->orig_fail_layer            = CM_CALL_ORIG_ERR_LAYER_OTHER;
  #endif /* FEATURE_IS2000_REL_A */


  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

  sr_ptr->is_hybr_redialed          = FALSE;

  sr_ptr->wlan_is_ok_to_orig        = FALSE;

  #ifdef FEATURE_CALL_THROTTLE
  sr_ptr->is_call_throttled         = FALSE;
  #endif

} /* sr_reset() */


#if (defined( FEATURE_UMTS_1X_HANDOVER_1XMSM ) || defined( FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X ))
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_MSM1X */


/*===========================================================================

FUNCTION sr_is_more_redial

DESCRIPTION
  Check whether more silent redials are left.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if more silent redials are left. FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean sr_is_more_redial(

    const cm_sr_s_type  *sr_ptr,
        /* pointer to silent redial object
        ** which to check for more redials */

    dword         uptime
        /* The current uptime */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If more silent redial attempts left and we are not over the silent
  ** redial max time, return TRUE. Else return FALSE.
  */
  if( sr_ptr->sr_left > 1 && uptime <= sr_ptr->sr_max_uptime )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;
} /* sr_is_more_redial() */



/*===========================================================================

FUNCTION sr_sched_next

DESCRIPTION
  Schedule next silent redial.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void sr_sched_next(


    cm_sr_s_type           *sr_ptr,
        /* Pointer to silent redial object */

    dword                   sr_period,
        /* Silent redial period (i.e. when next redial attempt should take
        ** place */

    dword                   hold_orig_period,
        /* Hold orig period */

    cm_sr_act_e_type        sr_act,
        /* Indicate whether the silent redial sequence should be restarted,
        ** decremented or left as is.
        */
    cm_call_end_e_type      end_status,
        /* the call end status */

    cmcall_act_e_type       redial_act
        /* The redial action */

)
/*lint -esym(715, redial_act) */
/*lint -esym(715, end_status) */
{
  dword                    uptime               = clk_uptime();
    /* get the current uptime */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Schedule the next silent redial uptime.
  */
  sr_ptr->next_orig_uptime = uptime + sr_period;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If hold orig != 0, set the hold-orig uptime.
  */
  if( hold_orig_period != 0 )
  {
    sr_ptr->hold_orig_uptime = MAX( sr_ptr->hold_orig_uptime,
                                    uptime + hold_orig_period );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the silent redial sequence should be restarted, reset the number of
  ** silent redial left. Else, decrement the number of silent redial left.
  */
  switch ( sr_act )
  {
    case CM_SR_ACT_RESTART:
      sr_ptr->sr_left = sr_ptr->orig_plan_ptr->sr_max_dial;
      break;

    case CM_SR_ACT_DEC:
      (void)(CM_DEC( sr_ptr->sr_left ));
      break;

    case CM_SR_ACT_NO_CHANGE:
      break;

    case CM_SR_ACT_NONE:
    case CM_SR_ACT_MAX:
    default:
      CM_ERR_FATAL("Bad sr action = %d", sr_act, 0, 0);
      break;

  } /* switch ( sr_act ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset is_ok_to_orig and orig_sent_on_mode to FALSE.
  */
  sr_ptr->is_ok_to_orig   = FALSE;

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif

  sr_ptr->orig_sent_on_mode = SYS_SYS_MODE_NONE;

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif

  if ( redial_act != CMCALL_ACT_END_ORIG )
  {
    cmlog_call_orig_redial( end_status,
                            redial_act,
                            sr_ptr->sr_left );
  }

  sr_ptr->wlan_is_ok_to_orig = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH( "SRFailed EndStatus=%d Redial act %d SRLeft=%d",
                end_status, redial_act, sr_ptr->sr_left );

  CM_MSG_LOW( "Next SR orig uptime=%ld",
               MAX(sr_ptr->next_orig_uptime, sr_ptr->hold_orig_uptime),
               0,0 );
} /* sr_sched_next() */
/*lint +esym(715, redial_act) */
/*lint +esym(715, end_status) */

/*===========================================================================

FUNCTION cmcall_check_if_no_srv
DESCRIPTION
  Function to check if we both the HDR and the main instance have no service

DEPENDENCIES

RETURN VALUE
  True  If both HDR and 1X do not have service
  False Otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_check_if_no_srv( boolean check_hdr )
{

  cmss_s_type *ss_ptr = cmss_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( ss_ptr   != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ss_ptr->info.hdr_hybrid && check_hdr )
  {
    return ( ( ss_ptr->main_srv_status == SYS_SRV_STATUS_NO_SRV ||
               ss_ptr->main_srv_status == SYS_SRV_STATUS_PWR_SAVE ) &&
             ( ss_ptr->hybr_1_srv_status == SYS_SRV_STATUS_NO_SRV ||
               ss_ptr->hybr_1_srv_status == SYS_SRV_STATUS_PWR_SAVE ) );
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return( ss_ptr->main_srv_status == SYS_SRV_STATUS_NO_SRV ||
          ss_ptr->main_srv_status == SYS_SRV_STATUS_PWR_SAVE );

} /* cmcall_check_if_no_srv */

/*===========================================================================

FUNCTION cmcall_misc_is_mode_pref

DESCRIPTION

Check whether specified fav_mode_pref_1 complies with fav_mode_pref_2

DEPENDENCIES

   None

RETURN VALUE

  TRUE if so,
  FALSE otherwise and when any of mapping of CM enum to SD enum failed

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_misc_is_mode_pref(

   cm_mode_pref_e_type fav_mode_pref_1,

   cm_mode_pref_e_type fav_mode_pref_2

)
{

  sd_ss_mode_pref_e_type sd_mode_pref_1 = SD_SS_MODE_PREF_NONE;
  sd_ss_mode_pref_e_type sd_mode_pref_2 = SD_SS_MODE_PREF_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmcall_misc_is_mode_pref()",0,0,0 );
  CM_ASSERT( BETWEEN(fav_mode_pref_1, CM_MODE_PREF_NONE, CM_MODE_PREF_MAX ));
  CM_ASSERT( BETWEEN(fav_mode_pref_2, CM_MODE_PREF_NONE, CM_MODE_PREF_MAX ));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ----- - - - - -*/

  sd_mode_pref_1 = cmph_map_cm_mode_pref_to_sd_mode_pref(fav_mode_pref_1);

  if( sd_mode_pref_1 == SD_SS_MODE_PREF_NONE )
  {
    CM_ERR("Mapping failed for %d! mode pref!", fav_mode_pref_1 ,0,0);
    return FALSE;
  }

  sd_mode_pref_2 = cmph_map_cm_mode_pref_to_sd_mode_pref(fav_mode_pref_2);

  if( sd_mode_pref_2 == SD_SS_MODE_PREF_NONE )
  {
    CM_ERR("Mapping failed for %d! mode pref!", fav_mode_pref_2 ,0,0);
    return FALSE;
  }

  return sd_misc_is_mode_pref( sd_mode_pref_1,
                               sd_mode_pref_2 ) ;
}




/*===========================================================================
FUNCTION cmcall_misc_get_common_mode_pref

DESCRIPTION

  Gets the common SD mode preference component(s) from given CM mode preference.

  Example:

  The below will add all single mode pref components from
  CM_MODE_PREF_AUTOMATIC and end up with CM_MODE_PREF_NONE.

  cm_mode_pref = cmcall_misc_get_common_mode_pref(
                                           CM_MODE_PREF_AUTOMATIC,
                                           3,// because 6 components follow
                                           CM_MODE_PREF_CDMA_ONLY,
                                           CM_MODE_PREF_CDMA_HDR_ONLY,
                                           CM_MODE_PREF_DIGITAL_ONLY,
                                           );

DEPENDENCIES
  sd_misc_get_common_mode_pref()

RETURN VALUE
  Return new preference or CM_MODE_PREF_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
cm_mode_pref_e_type cmcall_misc_get_common_mode_pref
(
  cm_mode_pref_e_type cm_mode_pref,
    /* CM mode pref
    */

  int number_of_components,
  ...
)
{

  va_list arg_list;
  sd_ss_mode_pref_e_type sd_mode_pref;
  sd_ss_mode_pref_e_type sd_mode_pref_common = SD_SS_MODE_PREF_NONE;
  cm_mode_pref_e_type    cm_mode_pref_common = CM_MODE_PREF_NONE;

  /*
  ** First map given CM mode pref to SD mode pref
  */
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(cm_mode_pref);
  if( sd_mode_pref == SD_SS_MODE_PREF_NONE)
  {
    CM_ERR( "Mapping #1 failed !!!",0,0,0 );
    return CM_MODE_PREF_NONE;
  }


  /*
  ** Initialize arg_list and make an internal pointer point
  ** at the first variable function argument.
  ** This will be the second parameter
  */
  /*lint -e{64} -e{718} -e{530} -e{516} */
  va_start(arg_list, number_of_components);

  /*
  ** Process second and subsequent function arguments.
  ** These are the SD components to be removed from given
  ** CM mode pref
  */

  while(number_of_components--)
  {

    /*lint -e{10} -e{662} -e{826} -e{718} */
    cm_mode_pref_common = (cm_mode_pref_e_type)va_arg(arg_list, int);

    if(!BETWEEN( cm_mode_pref_common,
                 CM_MODE_PREF_NONE,
                 CM_MODE_PREF_MAX))
    {
      CM_ERR( "Bad cm_mode_pref = %d",cm_mode_pref_common,0,0 );
      CM_ASSERT(FALSE);
    }

    sd_mode_pref_common = cmph_map_cm_mode_pref_to_sd_mode_pref(cm_mode_pref_common);

    if( sd_mode_pref_common == SD_SS_MODE_PREF_NONE)
    {
      CM_ERR( "Mapping failed !!!",0,0,0 );
      return CM_MODE_PREF_NONE;
    }

    /* Remove it */
    sd_mode_pref = sd_misc_get_common_mode_pref( sd_mode_pref,
                                                 sd_mode_pref_common );
    /*
    ** Why INRANGE - if we start from CM_MODE_PREF_AUTOMATIC
    ** and want to remove all SD components one by one,
    ** from SD_SS_MODE_PREF_WCDMA all the way through SD_SS_MODE_PREF_AMPS,
    ** we'll end up with SD_SS_MODE_PREF_NONE and that's fine
    */
    if(!INRANGE( sd_mode_pref,
                 SD_SS_MODE_PREF_NONE,
                 SD_SS_MODE_PREF_MAX))
    {
      CM_ERR( "Bad sd_mode_pref = %d",sd_mode_pref,0,0 );
      CM_ASSERT(FALSE);
    }
  }

  /* Clean up */
  va_end(arg_list);

  /* Now map result back to CM mode pref enum value */
  cm_mode_pref = cmph_map_sd_mode_pref_to_cm_mode_pref(sd_mode_pref);
  if(cm_mode_pref == CM_MODE_PREF_MAX) {
    CM_ERR( "Mapping #2 failed !!!",0,0,0 );
  }

  return cm_mode_pref;
}

/*===========================================================================

FUNCTION cmcall_intersect_or_force_mode_pref

DESCRIPTION
  Computes the intersection of the mode preferences

DEPENDENCIES


RETURN VALUE
Returns the mode preference

SIDE EFFECTS
  none

===========================================================================*/

static cm_mode_pref_e_type cmcall_misc_intersect_or_force_mode_pref(

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

      CM_SWITCH_ERR( "srv_type %d", srv_type, 0, 0 );
      /*
      ** 527: Unreachable
      ** This is because CM_SWITCH_ERR() evaluates to error fatal
      ** (i.e exit program)
      ** But for different feature set it may not to, so we want to
      ** supress the warning here
      */
      /*lint -save -e527 */
      requested_mode_pref = mode_pref;
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


  if( mode_pref == CM_MODE_PREF_NONE || mode_pref >= CM_MODE_PREF_MAX )
  {
    return ( requested_mode_pref );
  }

  return ( mode_pref );


} /* cmcall_misc_intersect_or_force_mode_pref */



/*===========================================================================

FUNCTION cmcall_copy_ss_info

DESCRIPTION
  Copies the current serving system info to the call object

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void cmcall_copy_ss_info(
   cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  cmss_s_type *ss_ptr      = cmss_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( ss_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_ptr->prev_srv_status[CM_SS_MAIN] = ss_ptr->info.srv_status;

  if (ss_ptr->info.hdr_hybrid )
  {
    call_ptr->prev_srv_status[CM_SS_HDR] =  ss_ptr->info.hdr_srv_status;
  }
  return;

}/* cmcall_copy_ss_info */

#ifdef FEATURE_HDR_HYBRID
#error code not present
#endif /* FEATURE_HDR_HYBRID */



/*===========================================================================

FUNCTION cmcall_is_ok_to_end_hold_orig_period

DESCRIPTION
  Determine whetehr it is ok to end hold orig period for call orig

DEPENDENCIES


RETURN VALUE
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_is_ok_to_end_hold_orig_period
(
  const cmcall_s_type    *call_ptr
)
{
  cmss_s_type   *ss_ptr = cmss_ptr();

  boolean       status = FALSE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(call_ptr->ss == CM_SS_WLAN)
  {
    if( ss_ptr->info.wlan_srv_status != SYS_SRV_STATUS_NO_SRV &&
        !ss_ptr->wlan_is_pref_srv_acq)
    {
      status = TRUE;
    }
  }

  #if (defined (FEATURE_HDR_HYBRID) || defined(FEATURE_HYBR_GW))
#error code not present
  #endif
  {
    #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
    if( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS ||
        call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS )
    {
      if( (sys_srv_status_is_srv(ss_ptr->info.gw_srv_status) &&
          !ss_ptr->is_pref_srv_acq ) ||
          /* Check for faked no service due to not usable system.
          */
          (ss_ptr->info.gw_srv_status == SYS_SRV_STATUS_NO_SRV &&
           ss_ptr->ss_main_usable == SD_SYS_USABLE_NOT_OK) ||
          (ss_ptr->info.gw_srv_status == SYS_SRV_STATUS_PWR_SAVE) ||
          /* Satisfying GCF 26.7.4.3.4 to end hold orig period for
          ** call during camped indication. This solves only
          ** power up acquisiton cases, need to think about
          ** camped indication in between full service.
          */
          (ss_ptr->info.gw_srv_domain == SYS_SRV_DOMAIN_CAMPED &&
           ss_ptr->is_pref_srv_acq)
        )
      {
        status = TRUE;
      }
    }
    else
    #endif
    if( (sys_srv_status_is_srv(ss_ptr->main_srv_status) &&
        !ss_ptr->is_pref_srv_acq ) ||
        /* Check for faked no service due to not usable system.
        */
        (ss_ptr->info.srv_status == SYS_SRV_STATUS_NO_SRV &&
         ss_ptr->ss_main_usable == SD_SYS_USABLE_NOT_OK) ||
        (ss_ptr->info.srv_status == SYS_SRV_STATUS_PWR_SAVE) ||
        /* Satisfying GCF 26.7.4.3.4 to end hold orig period for
        ** call during camped indication. This solves only
        ** power up acquisiton cases, need to think about
        ** camped indication in between full service.
        */
        (ss_ptr->info.srv_domain == SYS_SRV_DOMAIN_CAMPED &&
         ss_ptr->is_pref_srv_acq)
      )
    {
      status = TRUE;
    }
  }

  return status;
} /* cmcall_is_ok_to_end_hold_orig_period() */



/*===========================================================================

FUNCTION cmcall_is_ok_to_orig

DESCRIPTION
  Determine whetehr it is ok to orig

DEPENDENCIES


RETURN VALUE
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_is_ok_to_orig
(
  const cmcall_s_type   *call_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(call_ptr->ss == CM_SS_WLAN)
  {
    #ifdef FEATURE_CM_EXTERNAL_WLAN
#error code not present
    #else
    return call_ptr->sr.wlan_is_ok_to_orig;
    #endif
  }

  #if (defined (FEATURE_HDR_HYBRID) || defined(FEATURE_HYBR_GW))
#error code not present
  #endif  /* FEATURE_HDR_HYBRID */
  {
    return call_ptr->sr.is_ok_to_orig;
  }
} /* cmcall_is_ok_to_orig() */

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================

FUNCTION cmcall_is_redir_or_ho

DESCRIPTION
  Determine whetehr it is ok to orig

DEPENDENCIES


RETURN VALUE
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_is_redir_or_ho
(
  const cmcall_s_type   *call_ptr
)
/*lint -esym(715, call_ptr) */
{
  cmss_s_type   *ss_ptr = cmss_ptr();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif  /* FEATURE_HDR_HYBRID */
  {
    return ss_ptr->is_redir_or_ho;
  }
} /* cmcall_is_redir_or_ho() */
/*lint +esym(715, call_ptr) */
#endif


/*===========================================================================

FUNCTION cmcall_is_sufficient_srv

DESCRIPTION
  Check whether the specified service is sufficient relatively
  to the specified mode preference in order to originate a call.

  E.g. if mode preference is DIGITAL-ONLY, digital service is sufficient
  to originate a call, whereas analog service is NOT.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if the specified service is sufficient relatively to the specified
  mode preference in order to originate a call. FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(765, cmcall_is_sufficient_srv)
** Can't be static, unit test uses it
*/
boolean cmcall_is_sufficient_srv
(
  const cmcall_s_type *call_ptr,
  cm_mode_pref_e_type mode_pref,
  sys_sys_mode_e_type sys_mode

)
{
  cmss_s_type   *ss_ptr = cmss_ptr();

  sd_ss_mode_pref_e_type sd_mode_pref;
  sd_mode_e_type         sd_mode;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref( mode_pref );
  sd_mode = cmph_map_sys_sys_mode_to_sd_mode( sys_mode );

  if(( sd_mode_pref == SD_SS_MODE_PREF_NONE ) ||
     ( sd_mode == SD_MODE_MAX ))
  {

    /* Mapping function failed */
    return FALSE;

  }

  if(call_ptr->ss == CM_SS_WLAN)
  {
    if( ss_ptr->info.wlan_srv_status != SYS_SRV_STATUS_SRV )
    {
      return FALSE;
    }

    sd_mode = SD_MODE_WLAN;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* We have 3 fields in ss that tell us the current service status.
     ss_ptr->info.srv_status tells whether we have service on MAIN.
     If we do, ss_ptr->sys_mode tell us what that service is (1x/GSM/etc).
     ss_ptr->info.hdr_srv_status tells whether we have service on HDR.
     We don't need corresponding hdr_sys_mode field since having service
     means HDR only.
  */
  #if (defined( FEATURE_HDR ) || defined(FEATURE_HYBR_GW) )
#error code not present
  #endif  /* FEATURE_HDR */

  /* Print what we got */
  CM_MSG_MED("mode_pref = %d, sys_mode = %d",mode_pref,sys_mode,0);

  /* This checks whether given SD mode is sufficient for given SD mode pref */
  return ((BM( sd_mode ) & (dword)sd_mode_pref) != 0);
} /* cmcall_is_sufficient_srv() */


/*===========================================================================

FUNCTION cmcall_is_call_type_supp_for_ipcall

DESCRIPTION

  Returns TRUE or FALSE depending on if given call type can be originated as
  a ip call. This gives CM control over what call types can get
  originated as ip call.

DEPENDENCIES

   None

RETURN VALUE

  TRUE  - Given call type can be originated as ip call.
  FALSE - Given call type cannot be originated as ip call.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_is_call_type_supp_for_ipcall (

  cm_call_type_e_type   call_type
    /* call_type Voice, CS_DATA */

)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( BETWEEN(call_type, CM_CALL_TYPE_NONE, CM_CALL_TYPE_MAX ));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch (call_type)
  {
    case CM_CALL_TYPE_VOICE:                          return TRUE;

    case CM_CALL_TYPE_VS:                             return TRUE;

    default:                                          return FALSE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

} /* cmcall_is_call_type_supp_for_ipcall () */

/*===========================================================================

FUNCTION cmcall_is_ipcall_over_mode_poss

DESCRIPTION

  Returns TRUE if given call can be placed on given mode as a IP call.

DEPENDENCIES

   None

RETURN VALUE

  TRUE  -  Call can be placed as a IP call on given mode
  FALSE -  Call cannot be placed as a IP call.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_ipcall_over_mode_poss(

  sys_sys_mode_e_type              sys_mode,
     /* Specific technology - WCDMA, WLAN */

  const cmcall_s_type              *call_ptr
    /* Call object */
)
{

  sd_ss_mode_pref_e_type   sd_mode_pref         = SD_SS_MODE_PREF_NONE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if given call type is allowed to be originated as a ip call */
  if (!cmcall_is_call_type_supp_for_ipcall (call_ptr->call_type))
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sd_mode_pref =
        cmph_map_cm_mode_pref_to_sd_mode_pref( call_ptr->favored_mode_pref );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (sys_mode)
  {

    case SYS_SYS_MODE_HDR:
     /* Check made to see if current call can be originated using a VoIP
     ** app on given sys_mode.
     */
     if( sd_misc_is_mode_pref   (sd_mode_pref, SD_SS_MODE_PREF_HDR)  &&
         cmcall_is_active_ip_app(SYS_SYS_MODE_HDR, call_ptr->call_type)
       )
     {
       return TRUE;
     }

     break;

   case SYS_SYS_MODE_WCDMA:
     /* Check made to see if current call can be originated using a VoIP
     ** app on given sys_mode.
     */
     if( sd_misc_is_mode_pref   (sd_mode_pref, SD_SS_MODE_PREF_WCDMA)  &&
         cmcall_is_active_ip_app(SYS_SYS_MODE_WCDMA, call_ptr->call_type)
       )
     {
       return TRUE;
     }

     break;


   case SYS_SYS_MODE_WLAN:
     /* Check made to see if current call can be originated using a VoIP
     ** app on given sys_mode.
     */
     if( sd_misc_is_mode_pref   (sd_mode_pref, SD_SS_MODE_PREF_WLAN)  &&
         cmcall_is_active_ip_app(SYS_SYS_MODE_WLAN, call_ptr->call_type)
       )
     {
       return TRUE;
     }

     break;

   default:

     CM_MSG_HIGH ("ipcall not possible on sys %d", sys_mode, 0, 0);
     break;

  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;
} /* cmcall_is_ipcall_over_mode_poss () */


/*===========================================================================

FUNCTION cmcall_instance_for_ipcall

DESCRIPTION

 Returns a valid instance if the given call can be originated over IP on the
 given mode.

DEPENDENCIES

   None

RETURN VALUE

  Instance.

SIDE EFFECTS
  none

===========================================================================*/
static cm_ss_e_type cmcall_instance_for_ipcall(

   const cmcall_s_type        *call_ptr
     /* Call object */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  CM_ASSERT( BETWEEN(call_ptr->call_type, CM_CALL_TYPE_NONE, CM_CALL_TYPE_MAX ));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check if given call type is allowed to be originated as a ip call */
  if (!cmcall_is_call_type_supp_for_ipcall (call_ptr->call_type))
  {
    return CM_SS_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* IP call is currently possible on only these technologies
  **
  ** - HDR
  ** - WLAN
  ** - WCDMA
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If there is a registered VoIP app on hdr and if the voice call has
  ** HDR in it's favored mode preference, return the instance that has
  ** HDR service
  */
  if (cmcall_is_ipcall_over_mode_poss (SYS_SYS_MODE_HDR, call_ptr))
  {
    if (cmpref_is_hybr_status_on())
    {
      return CM_SS_HDR;
    }

    return CM_SS_MAIN;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If there is a registered VoIP app on WCDMA and if the voice call has
  ** WCDMA in it's favored mode preference, return the instance that has
  ** WCDMA service
  */
  if (cmcall_is_ipcall_over_mode_poss (SYS_SYS_MODE_WCDMA, call_ptr))
  {
    /* Determine the ss for WCDMA
    */
    if( cmpref_is_hybr_gw_call_allowed() )
    {
      return CM_SS_HYBR_1;
    }
    return CM_SS_MAIN;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If there is a registered VoIP app on WLAN and if the voice call has
  ** WLAN in it's favored mode preference, return the instance that has
  ** WLAN service
  */
  if (cmcall_is_ipcall_over_mode_poss (SYS_SYS_MODE_WLAN, call_ptr))
  {
    return CM_SS_WLAN;
  }


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* No instance matches for ipcall */
  return CM_SS_NONE;

} /* cmcall_instance_for_ipcall () */


/*===========================================================================

FUNCTION cmcall_send_ipcall_orig

DESCRIPTION
  Send call Origination on IP system.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_send_ipcall_orig
(
  cmcall_s_type       *call_ptr
    /* Call object */
)
{
  sys_sys_mode_e_type orig_mode = SYS_SYS_MODE_NO_SRV;
    /* Holds the mode on given instance */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );

  CM_ASSERT(BETWEEN(call_ptr->call_type, CM_CALL_TYPE_NONE, CM_CALL_TYPE_MAX));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call is sent to this function only after receiving ok_to_orig on
  ** call_ptr->ss. Mode present on call_ptr's instance will be the one
  ** on which origination gets sent.
  */
  if ((orig_mode = cmcall_det_sys_mode_from_ss (call_ptr->ss))
                                                  == SYS_SYS_MODE_NO_SRV)
  {
    CM_ERR (" NO srv, IP origination not possible", 0, 0, 0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_ptr->sr.orig_sent_on_mode = orig_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_IP_CALL)

  switch (call_ptr->call_type)
  {
    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_VS:

      cmipcall_send_orig (call_ptr);
      break;

    default:

      CM_ERR ("Unhandled call type %d", call_ptr->call_type, 0, 0);
      break;

  }

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}  /* cmcall_send_ipcall_orig */


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================

FUNCTION cmcall_force_analog_on_the_fly

DESCRIPTION
  Force the mode preference to analog-only on the fly if necessary
  conditions apply.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

  This function is expected to be called during the origination process
  before call is connected.

RETURN VALUE
  A ballpark estimation for the number of seconds the phone is going
  to be without service. If mode is not forced, return 0.

SIDE EFFECTS
  none

===========================================================================*/
static dword  cmcall_force_analog_on_the_fly(

    const cmcall_s_type    *call_ptr,
        /* pointer to a call object */

    word                    rssi_thr
        /* RSSI threshold for deciding whether to force analog.
        ** 0 implies force to AMPS regardless of RSSI reading */
)
{
  const cdma_cmcall_type *call_mode_info_ptr = NULL;
      /* Pointer to call mode info */

  cmph_s_type           *ph_ptr               = cmph_ptr();
      /* Point at phone object */

  cmss_s_type           *ss_ptr               = cmss_ptr();
      /* Point at serving system object */

  boolean               is_force_analog_mode  = TRUE;
      /* Indicate whether analog mode should be forced */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmcall_force_analog_on_the_fly()",0,0,0 );
  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

  /* Do NOT force to analog if this target does not support Analog Call
  ** Processing (ACP)
  */
  #if( !defined(FEATURE_ACP) )
  {
    is_force_analog_mode = FALSE;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do NOT force to analog if analog service is not sufficient
  ** to originate the call in question.
  */

  #ifdef FEATURE_HDR
#error code not present
  #endif /* FEATURE_HDR */
  {
    if(!cmcall_is_sufficient_srv(call_ptr,
                                 call_ptr->favored_mode_pref,
                                 SYS_SYS_MODE_AMPS))
    {
      is_force_analog_mode = FALSE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do NOT force to analog if RSSI level is high enough.
  ** Note that 0 rssi_thr implies force to AMPS regardless of RSSI reading.
  **
  ** NOTE! RSSI is implicitly negative, so smaller number means
  ** bigger value.
  */
  if( (word) srch_get_best_rssi() < rssi_thr )
  {
    is_force_analog_mode = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do NOT force to analog if mode preference is already analog.
  */
  if( ph_ptr->mode_pref == CM_MODE_PREF_AMPS_ONLY )
  {
    is_force_analog_mode = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For targets with System Determination 2.0 support do the following
  ** processing.
  */
  {
    /* If forcing to analog is required, force to analog. Else force with
    ** no mode preference change - this is necessary in order for us to get
    ** the ok to originate indication once again.
    */
    /*
    ** 774: is_force_analog_mode Boolean within 'if' always evaluates to False
    ** This happens because of particular target not having FEATURE_ACP
    ** defined so we want to supress it in those cases
    */

    /*lint -save -e774 */
    if( is_force_analog_mode )
    /*lint -restore */
    {
      cm_band_pref_e_type   favored_band_pref;
      cm_roam_pref_e_type   favored_roam_pref;

      /* Get the favored band preference to be used for the force AMPS.
      */
      favored_band_pref = cmcall_get_favored_band_pref( call_ptr->call_type,
                                                 call_ptr->srv_type,
                                                 ph_ptr->band_pref );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Get the favored roam preference to be used for the force AMPS.
      */
      favored_roam_pref = cmcall_get_favored_roam_pref( call_ptr->call_type,
                                                 ss_ptr->info.sys_mode,
                                                 ss_ptr->info.roam_status,
                                                 ph_ptr->roam_pref );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      CM_MSG_HIGH( "forcing analog",0,0,0 );
      return cmph_force_pref_on_the_fly(
                                         call_ptr->ss,
                                         ph_ptr,
                                         SD_SS_PREF_REAS_REDIAL_NORM,
                                         cmph_map_call_type_to_act_type( call_ptr->call_type ),
                                         CM_PREF_TERM_CM_1_CALL_PLUS,
                                         CM_MODE_PREF_AMPS_ONLY,
                                         favored_band_pref,
                                         call_ptr->favored_prl_pref,
                                         favored_roam_pref,
                                         CM_HYBR_PREF_NO_CHANGE,
                                         ph_ptr->plmn,
                                         ph_ptr->srv_domain_pref,
                                         call_mode_info_ptr->otasp_info.otasp_act_code,
                                         &ph_ptr->wlan_pref,
                                         ph_ptr->acq_order_pref,
                                         CM_NETWORK_SEL_MODE_PREF_NONE,
                                         (cm_act_id_type) call_ptr->call_id,
                                         CM_ACT_UPDATE_REAS_ACT_CONT,
                                         call_ptr->force_top_pref
                                        );
    }
    else
    {
      (void) cmph_force_pref_on_the_fly(
                                         call_ptr->ss,
                                         ph_ptr,
                                         SD_SS_PREF_REAS_REDIAL_NORM,
                                         cmph_map_call_type_to_act_type( call_ptr->call_type ),
                                         CM_PREF_TERM_CM_1_CALL_PLUS,
                                         CM_MODE_PREF_NO_CHANGE,
                                         CM_BAND_PREF_NO_CHANGE,
                                         CM_PRL_PREF_NO_CHANGE,
                                         CM_ROAM_PREF_NO_CHANGE,
                                         CM_HYBR_PREF_NO_CHANGE,
                                         ph_ptr->plmn,
                                         ph_ptr->srv_domain_pref,
                                         call_mode_info_ptr->otasp_info.otasp_act_code,
                                         &ph_ptr->wlan_pref,
                                         ph_ptr->acq_order_pref,
                                         CM_NETWORK_SEL_MODE_PREF_NONE,
                                         (cm_act_id_type) call_ptr->call_id,
                                         CM_ACT_UPDATE_REAS_ACT_CONT,
                                         call_ptr->force_top_pref
                                         );
      return 0;
    }

  }

} /* cmcall_force_analog_on_the_fly() */



/*===========================================================================

FUNCTION cmcall_force_sr_on_the_fly

DESCRIPTION
  Force optimized silent redial if necessary conditions apply.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

  This function is expected to be called during the origination process
  before call is connected.

RETURN VALUE
  A ballpark estimation for the number of seconds the phone is going
  to be without service. If mode is not forced, return 0.

SIDE EFFECTS
  none

===========================================================================*/
static dword  cmcall_force_sr_on_the_fly(

    const cmcall_s_type    *call_ptr,
        /* pointer to a call object */

    word                   rssi_thr,
        /* RSSI threshold for deciding whether to force optimized silent
        ** redial. 0 implies force optimized silent redial regardless of RSSI
        ** reading */

    boolean                is_force_roam_pref
        /* Indicate whether the roam preference should be forced to ensure
        ** no change of roaming indication.
        */
)
{
  const cdma_cmcall_type *call_mode_info_ptr = NULL;
      /* Pointer to call mode info */

  cmph_s_type           *ph_ptr              = call_ptr->ph_ptr;
      /* Point at phone object */

  boolean               is_optimized_sr      = TRUE;
      /* Indicate whether optimized silent redial should be forced */

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif  /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmcall_force_sr_on_the_fly()",0,0,0 );
  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

  /* Do NOT force optimized silent redial if mode preference is analog only.
  */
  if( ph_ptr->mode_pref == CM_MODE_PREF_AMPS_ONLY )
  {
    is_optimized_sr = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do NOT force optimized silent redial if this is HDR only call
     or the current system is HDR
  */

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Do NOT force optimized silent redial if RSSI level is high enough.
  ** Note that 0 rssi_thr implies force to AMPS regardless of RSSI reading.
  **
  ** NOTE! RSSI is implicitly negative, so smaller number means
  ** bigger value.
  */

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif
  {
    if( (word) srch_get_best_rssi() < rssi_thr )
    {
      is_optimized_sr = FALSE;
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If optimized silent redial is required, force the SS-Preference with
  ** optimized silent redial.
  */
  if( is_optimized_sr )
  {
    cm_roam_pref_e_type   favored_roam_pref;

    CM_MSG_HIGH( "Optimized SR",0,0,0 );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the roam preference should be forced to ensure no change of
    ** roaming indication, get the favored roam preference. Else use existing
    ** roaming preference.
    */
    if( is_force_roam_pref )
    {
      favored_roam_pref = call_ptr->favored_roam_pref;
    }
    else
    {
      favored_roam_pref = CM_ROAM_PREF_NO_CHANGE;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    return cmph_force_pref_on_the_fly(
                                       call_ptr->ss,
                                       ph_ptr,
                                       SD_SS_PREF_REAS_REDIAL_OPTI,
                                       cmph_map_call_type_to_act_type( call_ptr->call_type ),
                                       CM_PREF_TERM_CM_1_CALL_PLUS,
                                       CM_MODE_PREF_NO_CHANGE,
                                       CM_BAND_PREF_NO_CHANGE,
                                       CM_PRL_PREF_NO_CHANGE,
                                       favored_roam_pref,
                                       CM_HYBR_PREF_NO_CHANGE,
                                       ph_ptr->plmn,
                                       CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                       call_mode_info_ptr->otasp_info.otasp_act_code,
                                       &ph_ptr->wlan_pref,
                                       CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                       CM_NETWORK_SEL_MODE_PREF_NONE,
                                       (cm_act_id_type) call_ptr->call_id,
                                       CM_ACT_UPDATE_REAS_ACT_CONT,
                                       call_ptr->force_top_pref
                                      );
  } /* if( optimized silent redial */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (no optimized silent redial) force the SS-Preference with normal
  ** silent redial.
  */
  (void) cmph_force_pref_on_the_fly(
                                    call_ptr->ss,
                                     ph_ptr,
                                     SD_SS_PREF_REAS_REDIAL_NORM,
                                     cmph_map_call_type_to_act_type( call_ptr->call_type ),
                                     CM_PREF_TERM_CM_1_CALL_PLUS,
                                     CM_MODE_PREF_NO_CHANGE,
                                     CM_BAND_PREF_NO_CHANGE,
                                     CM_PRL_PREF_NO_CHANGE,
                                     CM_ROAM_PREF_NO_CHANGE,
                                     CM_HYBR_PREF_NO_CHANGE,
                                     ph_ptr->plmn,
                                     CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                     call_mode_info_ptr->otasp_info.otasp_act_code,
                                     &ph_ptr->wlan_pref,
                                     CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                     CM_NETWORK_SEL_MODE_PREF_NONE,
                                     (cm_act_id_type) call_ptr->call_id,
                                     CM_ACT_UPDATE_REAS_ACT_CONT,
                                     call_ptr->force_top_pref
                                   );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return 0;

} /* cmcall_force_sr_on_the_fly() */
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */



/*===========================================================================

FUNCTION cmcall_act_get

DESCRIPTION
  Indicating the action to take when call origination fails (due to
  no-service, fade, reorder, intercept, or release) in accordance with
  the currently loaded call origination plan.

DEPENDENCIES
  Origination plan must have already been loaded with
  cmcall_load_orig_plan()

RETURN VALUE
  Origination action to take, such as REDIAL, FORCE_AMPS, etc.

SIDE EFFECTS
  none

===========================================================================*/
cmcall_act_e_type  cmcall_act_get(

    const cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  const cmcall_orig_plan_s_type  *orig_plan_ptr = NULL;
      /* pointer to origination plan component */

  unsigned int i;
      /* loop counter */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( call_ptr->sr.orig_plan_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  orig_plan_ptr = call_ptr->sr.orig_plan_ptr;

  /* Look for a match in the origination plan action table
  ** and return the corresponding action.
  */
  for( i=0; i < orig_plan_ptr->action_table->action_table_entry_count; i++ )
  {
    if( orig_plan_ptr->action_table->action_table_entry[i].end_status == call_ptr->end_status )
    {
      return orig_plan_ptr->action_table->action_table_entry[i].action;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* No match was found - this should never happen!
  ** Return NONE.
  */
  MSG_HIGH("NO MATCH FOUND for call end status %d",call_ptr->end_status,0,0);
  CM_ASSERT( FALSE );
  return CMCALL_ORIG_ACT_NONE;

}

#ifdef FEATURE_CALL_THROTTLE
/*===========================================================================

FUNCTION cmcall_check_if_orig_thr

DESCRIPTION
  Checks if the call origination needs to be throttled. Sets the end status
  to CM_CALL_END_ORIG_THR if the call is throttled

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(714, cmcall_check_if_orig_thr) not referenced */
/*lint -esym(528, cmcall_check_if_orig_thr) not referenced */
/*
** called when FEATURE_CALL_THROTTLE on
*/
static void cmcall_check_if_orig_thr(

  cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  cmss_s_type *ss_ptr      = cmss_ptr();
  boolean      is_orig_thr = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(call_ptr != NULL);
  CM_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is a DRS=0 call, don't throttle
  */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  if( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA &&
      !call_ptr->cmcall_mode_info.info.cdma_info.drs_bit )
  {
    return;
  }
  #endif

  if( ( call_ptr->ss == CM_SS_HDR ) ||
                      ( ss_ptr->info.hdr_hybrid                   &&
                        call_ptr->ss          == CM_SS_MAIN       &&
                        ss_ptr->info.sys_mode == SYS_SYS_MODE_HDR
                      )
       #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
       || ( ss_ptr->info.sys_mode == SYS_SYS_MODE_HDR )
       #endif
    )
  {
    is_orig_thr = cmss_check_if_orig_thr_true(
                            ss_ptr->info.hdr_sys_id,
                            ss_ptr->info.mode_info.cdma_info.band_class,
                            ss_ptr->cmss_orig_thr_tbl_ptr,
                            call_ptr->call_type,
                            ss_ptr->info.mode_info.cdma_info.packet_zone );
  }
  else
  {
    is_orig_thr = cmss_check_if_orig_thr_true(
                            ss_ptr->info.sys_id,
                            ss_ptr->info.mode_info.cdma_info.band_class,
                            ss_ptr->cmss_orig_thr_tbl_ptr,
                            call_ptr->call_type,
                            ss_ptr->info.mode_info.cdma_info.packet_zone );

  } /* else  call_ptr->ss == CM_SS_HDR ...*/

  if( is_orig_thr )
  {
    call_ptr->end_status           = CM_CALL_END_ORIG_THR;
    cmcall_event( call_ptr, CM_CALL_EVENT_CALL_ORIG_THR );
    CM_MSG_HIGH(" Call Orig throttled call_id %d",call_ptr->call_id,0,0);
  }
  else
  {
    /* Origination will be successful reset the throttle flag
    */
    if( ss_ptr->info.main_thr_status.ss_thr_status )
    {
      cmss_misc_set_ss_info_thr_status( FALSE );
    }
  }
  return;
}/* cmcall_check_if_orig_thr */

/*===========================================================================

FUNCTION cmcall_check_if_orig_thr_and_add_entry

DESCRIPTION
  Checks if the call needs to be throttled. Sets the end status to
  CM_CALL_END_ORIG_THR if the call is throttled

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(714, cmcall_check_if_orig_thr_and_add_entry) not referenced */
/*lint -esym(528, cmcall_check_if_orig_thr_and_add_entry) not referenced */
/*
** called when FEATURE_CALL_THROTTLE on
*/
static void cmcall_check_if_orig_thr_and_add_entry(

  cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  cmss_s_type *ss_ptr      = cmss_ptr();
  boolean      is_orig_thr = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( call_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the end status is CM_CALL_END_ORIG_THR, return as the entry will
  ** already present in the table
  */

  if( call_ptr->end_status == CM_CALL_END_ORIG_THR )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is a DRS=0 call, don't add throttle entry
  */
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  if( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA &&
     !call_ptr->cmcall_mode_info.info.cdma_info.drs_bit )
  {
    return;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( call_ptr->ss == CM_SS_HDR ||
                               ( ss_ptr->info.hdr_hybrid                  &&
                                 call_ptr->ss          == CM_SS_MAIN      &&
                                 ss_ptr->info.sys_mode == SYS_SYS_MODE_HDR
                               )
      #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
      || ( ss_ptr->info.sys_mode == SYS_SYS_MODE_HDR )
      #endif
    )

  {
    /* Check if we need to add a entry in the origination throttle table
    ** if needed a new entry is added to the table
    ** This was a HDR call, add the hdr_sys_id to the table
    */
    is_orig_thr = cmss_check_if_orig_thr_and_add_entry(
                                                      ss_ptr,
                                                      call_ptr->call_type,
                                                      call_ptr->end_status,
                                                      TRUE );
  }/* */
  else
  {

   is_orig_thr  = cmss_check_if_orig_thr_and_add_entry(
                                                        ss_ptr,
                                                        call_ptr->call_type,
                                                        call_ptr->end_status,
                                                        FALSE );
  }/* else */

  if( is_orig_thr )
  {
    call_ptr->end_status           = CM_CALL_END_ORIG_THR;
    cmcall_event( call_ptr, CM_CALL_EVENT_CALL_ORIG_THR );
  }
  return;
}/* cmcall_check_if_orig_thr_and_add_entry */
#endif // FEATURE_CALL_THROTTLE

/*===========================================================================

FUNCTION cmcall_orig_proc

DESCRIPTION
  Perform origination processing in accordance with the loaded
  call origination plan.

  During the origination process this function is being called as follows:
  1. When the call is first originated.
  2. Periodically while the origination is in process.
  3. When receiving an origination failure indication from MC, such as
     a FADE or a REORDER.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_orig_proc(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  cm_sr_s_type             *sr_ptr              = NULL;
      /* point at silent redial component */

  cmph_s_type              *ph_ptr              = cmph_ptr();
      /* point at phone object */

  const cmcall_orig_plan_s_type  *orig_plan_ptr = NULL;
      /* Pointer to origination plan component */

  cmcall_act_e_type        redial_act           = CMCALL_ORIG_ACT_NONE;
      /* action to be taken on silent redial for given call end */

  dword                    no_srv_time          = 0;
      /* Ballpark estimation for no-service-time due to forcing the mode */

  dword                    uptime               = clk_uptime();
      /* get the current uptime */

  cmss_s_type              *ss_ptr              = cmss_ptr();

  boolean                   is_call_ended       = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmcall_orig_proc(), uptime=%ld",uptime,0,0 );

  CM_ASSERT( call_ptr      != NULL );
  CM_ASSERT( call_ptr->sr.orig_plan_ptr != NULL );
  CM_ASSERT( ph_ptr        != NULL);
  CM_ASSERT( ss_ptr        != NULL);

  CM_MSG_HIGH( "cmcall_orig_proc start", 0, 0, 0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ptr        = &call_ptr->sr;
  orig_plan_ptr = sr_ptr->orig_plan_ptr;

  /* If we did not force the origination mode yet, check whether we need to
  ** shorten the hold orig period as follows:
  */
  if( call_ptr->orig_mode == SD_SS_ORIG_MODE_NONE )
  {
    /* If we have service and are not looking for more preferred service, end
    ** the hold orig period now.
    */
    if( ( cmcall_is_ok_to_end_hold_orig_period(call_ptr) ) &&
         ( call_ptr->pd_status != CMCALL_PD_STATUS_HOLD_ORIG )
          #ifdef FEATURE_HDR_HYBRID
#error code not present
          #endif
          #ifdef FEATURE_CDSMS
          && ( call_ptr->sms_status != CMCALL_SMS_STATUS_HOLD_ORIG )
          #endif
       )
    {
      sr_ptr->hold_orig_uptime = uptime;
      CM_MSG_LOW( "Ending hold-orig period, uptime=%ld", uptime, 0, 0 );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else, if we have sufficient service to originate, only give a limited
    ** amount of additional time for the preferred service to be acquired.
    */
    else if( ( cmcall_is_sufficient_srv(call_ptr,
                                     call_ptr->favored_mode_pref,
                                     cmss_ptr()->info.sys_mode) ) &&
              ( call_ptr->pd_status != CMCALL_PD_STATUS_HOLD_ORIG )
             #ifdef FEATURE_HDR_HYBRID
#error code not present
             #endif
             #ifdef FEATURE_CDSMS
             && ( call_ptr->sms_status != CMCALL_SMS_STATUS_HOLD_ORIG )
             #endif
           )
    {
      sr_ptr->hold_orig_uptime = MIN(sr_ptr->hold_orig_uptime,
                                     uptime+CMCALL_PREF_SRV_HOLD_ORIG_TIME );

      CM_MSG_HIGH( "Shortening hold-orig period %ld, uptime=%ld",
                   sr_ptr->hold_orig_uptime, uptime, 0 );
    }

  }

  /*- - - -  - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is the first origination attempt, set the
  ** silent redial start time to the current uptime
  */
  if( sr_ptr->sr_start_uptime == 0 )
  {
    sr_ptr->sr_start_uptime = uptime;
    sr_ptr->sr_max_uptime   = uptime + orig_plan_ptr->sr_max_time;

    CM_MSG_LOW( "Silent redial starting, uptime=%ld",
                sr_ptr->sr_start_uptime, 0, 0 );

    #ifdef FEATURE_IS2000_REL_A
    sr_ptr->is_re_orig             = FALSE;
    sr_ptr->re_orig_reason         = FALSE;
    sr_ptr->re_orig_count          = 0;
    sr_ptr->orig_fail_layer        = CM_CALL_ORIG_ERR_LAYER_OTHER;
    #endif /* FEATURE_IS2000_REL_A */
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If colloc or full service acq. failed shorten the orig_uptime
  ** only if you have 1x service
  */
  if( ( call_ptr->end_status == CM_CALL_END_CM_COLLOC_ACQ_FAIL ) &&
      ( call_ptr->ss == CM_SS_HDR ))
  {
    #ifdef FEATURE_UNIFORM_SCAN_OOS
    /* Update the acq_fail flag
    */
    if( call_ptr->is_hdr_acq_fail ||
        ( !sys_srv_status_is_srv(call_ptr->prev_srv_status[CM_SS_MAIN]) &&
          !sys_srv_status_is_srv( call_ptr->prev_srv_status[CM_SS_HDR]) )
      )
    {

      if( cmcall_check_if_no_srv( FALSE ) )
      {
        CM_MSG_HIGH(" Ending HDR call orig in UOOS %d",call_ptr->call_id, 0, 0 );

        call_ptr->end_status = CM_CALL_END_NO_HYBR_HDR_SRV;
        cmcall_end( call_ptr );
        return;
      }

    }
    else
    {
      call_ptr->is_hdr_acq_fail = TRUE;
    }
    #endif

    if( ! cmcall_check_if_no_srv( FALSE ))
    {
      /* can not find any collocated HDR system so do silent redial
      ** right away
      */
      sr_ptr->hold_orig_uptime  = uptime;

      CM_MSG_HIGH( "CollocAcqFail next uptime %d uptime %d",
                                         sr_ptr->next_orig_uptime, uptime, 0 );
    }
  }

  if( call_ptr->end_status == CM_CALL_END_NO_FULL_SRV )
  {
     sr_ptr->hold_orig_uptime  = uptime;
     sr_ptr->next_orig_uptime  = uptime;
     CM_MSG_HIGH( "No_full_srv ",0, 0, 0 );
  }

  /*- - - -  - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are still in the hold-orig period and already received the ok to
  ** originate indication, end the hold-orig period now.
  */
  if(( uptime < sr_ptr->hold_orig_uptime &&
       cmcall_is_ok_to_orig(call_ptr) ) &&
        ( call_ptr->pd_status != CMCALL_PD_STATUS_HOLD_ORIG )
       #ifdef FEATURE_HDR_HYBRID
#error code not present
       #endif
       #ifdef FEATURE_CDSMS
       && ( call_ptr->sms_status != CMCALL_SMS_STATUS_HOLD_ORIG )
       #endif
    )
  {
    sr_ptr->hold_orig_uptime = uptime;
    CM_MSG_LOW( "Ending hold-orig period, uptime=%ld", uptime, 0, 0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are still in the hold-orig period, return now.
  */
  if( uptime < sr_ptr->hold_orig_uptime )
  {
    CM_MSG_LOW("Still in HO period, ho_uptime=%ld, uptime=%ld",
               sr_ptr->hold_orig_uptime, uptime, 0);
    return;
  }

  #ifdef FEATURE_DEDICATED_C2K_MODE
  if( ss_ptr->is_hdr_session_open
      && call_ptr->waiting_for_hdr_session_close )
  {
    CM_MSG_HIGH( "DCM: Call is waiting for HDR session close!",0,0,0 );
    return;
  }
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
  #endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

  /*-------------------------------------------------------------------------
               If we got here, the hold-orig period is over.
  -------------------------------------------------------------------------*/


  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif   /* FEATURE_HDR_HYBRID */

  call_ptr->pd_status = CMCALL_PD_STATUS_NONE;
  call_ptr->sms_status = CMCALL_SMS_STATUS_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** If we are still waiting for the PS Signaling down indication
  ** for a Packet Call in GW Mode, return now.
  */
  if((call_ptr->call_type == CM_CALL_TYPE_PS_DATA) &&
      cmph_is_waiting_for_ps_sig_down())
  {
    if(uptime <= ph_ptr->ps_signaling_down_wait_time)
    {
       CM_MSG_HIGH("Still in PS Signaling Wait period, sig_uptime=%ld, uptime=%ld",
                    ph_ptr->ps_signaling_down_wait_time, uptime, 0);
       return;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined (FEATURE_CDMA_800)|| defined (FEATURE_CDMA_1900)
  /*
  **
  **  If SMS/DBM is in progress while making PS Data in progress, fail
  **  the attempt and wait till SMS/DBM is over. Enter into Silent Redial.
  **
  **
  */

  if(call_ptr->call_type == CM_CALL_TYPE_PS_DATA)
  {
    /* If PS_DATA call is being made over MAIN, then only allow if
    ** SVD is supported.
    */
    if((cmcall_is_there_a_call_type(CM_CALL_TYPE_SMS,CM_CALL_ID_INVALID)
                            != CM_CALL_ID_INVALID) &&
         call_ptr->ss == CM_SS_MAIN &&
         !cmss_is_cdma_svd_supported())
    {
        CM_MSG_HIGH("Hold Data Orig while in SMS/DBM",0,0,0);
        call_ptr->end_status = CM_CALL_END_HOLD_OTHER_IN_PROG;
    }

    /* If the DO call is attempted in a low psist system, check
    ** if SMS or DBM is pending
    */
    else if( call_ptr->call_type == CM_CALL_TYPE_PS_DATA &&
             call_ptr->ss        == CM_SS_HDR            &&
             ss_ptr->info.psist  == CM_PSIST_LOW
           )
    {
      /* Check if SMS/DBM is in progress.
      */
      if ( cmxsms_is_mo_sms_in_progress() ||
           cmxdbm_is_mo_dbm_in_progress() ||
           (cmcall_is_there_a_call_type(CM_CALL_TYPE_SMS,CM_CALL_ID_INVALID)
                                != CM_CALL_ID_INVALID)
         )
      {
        CM_MSG_HIGH("Hold Data Orig while in SMS/DBM",0,0,0);
        call_ptr->end_status = CM_CALL_END_HOLD_OTHER_IN_PROG;
      }
    }
  }
  #endif

  #ifdef FEATURE_CM_EXTERNAL_WLAN
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we did not force the origination mode yet, do it now.
  */
  if( call_ptr->orig_mode == SD_SS_ORIG_MODE_NONE &&
      call_ptr->end_status == CM_CALL_END_NONE)
  {
    no_srv_time = cmcall_force_pref_on_the_fly
                  ( call_ptr,
                    (call_ptr->call_type == CM_CALL_TYPE_PS_DATA) ?
                                    SD_SS_PREF_REAS_ORIG_START_PS :
                                    SD_SS_PREF_REAS_ORIG_START_CS
                  );

    /* If forcing to mode preference is causing a short period of
    ** no service (during which MC is acquiring the new service),
    ** hold the origination for the appropriate amount of time.
    */
    sr_ptr->hold_orig_uptime = MAX( sr_ptr->hold_orig_uptime,
                                    uptime + no_srv_time );

    CM_MSG_HIGH( "Hold orig=%d, uptime=%d",
                  sr_ptr->hold_orig_uptime, uptime, 0 );

    return;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are currently waiting for the next silent redial period, return
  ** now.
  */
  if( uptime < sr_ptr->next_orig_uptime )
  {
    CM_MSG_HIGH("Wait for SR period=%ld, uptime=%ld, OK=%d",
                sr_ptr->next_orig_uptime, uptime, sr_ptr->is_ok_to_orig );
    return;
  }


  /*- - - -  - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If call end status is none, do the following:
  **
  ** For CDMA Mode:
  **
  **   - If NOT ok_to_orig, fail the orignination attempt and wait for
  **     for the service to appear. Enter into Silent Redial.
  **
  **   - If bad Service Option, End the Call attempt.
  **
  ** For GSM/WCDMA
  **
  **   - If NOT ok_to_orig, fail the origination attempt and wait
  **     for the service to appear. Enter into Silent Redial.
  **
  **
  ** For CM_CALL_MODE_INFO_IP
  **
  **   - Call mode is set to IP only when ok_to_orig is recvd from HDR
  **     or WLAN system. Other info_type's are set even before waiting for
  **     ok_to_orig. So "if check" is not required for INFO_IP.
  */

  if( call_ptr->end_status == CM_CALL_END_NONE )
  {

    if (call_ptr->ss == CM_SS_WLAN)
    {
      if(!cmcall_is_ok_to_orig(call_ptr))
      {
        call_ptr->end_status = CM_CALL_END_NO_WLAN_SRV;
      }
    }
    else if (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA)
    {
      #if defined (FEATURE_CDMA_800)|| defined (FEATURE_CDMA_1900)

      if ( !cmss_is_cdma_svd_supported())
      {
        if (cmxcall_is_conv_pc_otapa_commit())
        {
          call_ptr->end_status = CM_CALL_END_OTASP_COMMIT_IN_PROG;
        }
      }

      /*
      ** Check if OK_TO_ORIG received or not.
      */

      if( ! cmcall_is_ok_to_orig(call_ptr) )
      {
        call_ptr->end_status = CM_CALL_END_NO_CDMA_SRV;
        if ( call_ptr->ss == CM_SS_HYBR_1 &&
             call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA )
        {
          call_ptr->end_status = CM_CALL_END_NO_HYBR_HDR_SRV;
        }
      }

      /*
      ** Check whether origination fails due to failing a redirection or
      ** handoff.
      */

      if( sr_ptr->sr_left == 0 &&
          ! cmcall_is_redir_or_ho(call_ptr) )
      {
        call_ptr->end_status = CM_CALL_END_MC_ABORT;
      }

      /*
      ** Check for Service Option Validity.
      */

      if (!cmcall_determine_orig_srv_opt(call_ptr))
      {
        CM_MSG_HIGH("Unable to determine service option for the origination!",0,0,0);
        call_ptr->end_status = CM_CALL_END_NO_SRV;
        cmcall_end( call_ptr );
        return;
      }

      #if defined (FEATURE_GPSONE)
#error code not present
      #endif /* #if (defined FEATURE_GPSONE ) */

      #endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */
    }
    else if( (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS)
             ||
             (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS)
            )
    {
      if( ( ( call_ptr->ss == CM_SS_MAIN && !sr_ptr->is_ok_to_orig ) ||
            ( call_ptr->ss == CM_SS_HYBR_1 && !sr_ptr->hdr_is_ok_to_orig )
          )
          &&
          ( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE )
        )
      {
        call_ptr->end_status = CM_CALL_END_NO_GW_SRV;
      }
    }

    /* If there is a GPS session and we end it due to emergency call, we will
    ** hit this case
    */
    else if( call_ptr->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG )
    {
      /* Default this to CM_CALL_END_NO_CDMA_SRV, this will redial
      ** immediately
      */
      if( ! cmcall_is_ok_to_orig(call_ptr) )
      {
        call_ptr->end_status = CM_CALL_END_NO_CDMA_SRV;
        if ( call_ptr->ss == CM_SS_HDR )
        {
          call_ptr->end_status = CM_CALL_END_NO_HYBR_HDR_SRV;
        }
      }
    } /*  else if( call_ptr->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG ) */
    /* mode info not set, we have to fake no service call end here
    */
    else
    {
      if( ! cmcall_is_ok_to_orig(call_ptr) )
      {
        #ifdef FEATURE_HYBR_GW
#error code not present
        #else
        call_ptr->end_status = CM_CALL_END_NO_CDMA_SRV;
        #endif
      }
    }
  } /* if( call_ptr->end_status == CM_CALL_END_NONE ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If origination attempt did not fail yet, send origination as necessary
  ** and return.
  ** The origination has not failed if the end status is NONE
  ** and
  ** either we got ok to orig
  ** or
  ** the call origination is for a GW call and the call origination is
  ** alredy sent to the lower layers ( NAS )
  */

  if( ( call_ptr->end_status == CM_CALL_END_NONE )
      &&
      (
        ( cmcall_is_ok_to_orig(call_ptr) )
        ||
        ( ( ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS )
            ||
            ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS )
          )
          &&
          ( call_ptr->sr.orig_sent_on_mode != SYS_SYS_MODE_NONE )
        )
      )
    )
  {

    /* If origination was already sent, return now.
    */
    if( call_ptr->sr.orig_sent_on_mode != SYS_SYS_MODE_NONE )
    {
      return;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_CALL_THROTTLE

    /* Check if call needs to be throttled
    */
    cmcall_check_if_orig_thr( call_ptr );

    #endif /* FEATURE_CALL_THROTTLE*/

    /* If origination command was not already sent and it is ok to originate,
    ** send the origination now.
    */
    if( call_ptr->end_status != CM_CALL_END_ORIG_THR )
    {
      CM_MSG_HIGH( "Send orig cmd, uptime=%ld", uptime, 0, 0 );

      if ( (call_ptr->ss == CM_SS_WLAN) &&
           (call_ptr->call_type == CM_CALL_TYPE_PS_DATA)
         )
      {
        /* Handle PS call orig on WLAN */
        cmcall_send_wlan_orig(call_ptr);
      }
      else if (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP)
      {

        /* Since info_type is IP and OK_TO_ORIG has been received, call is
        ** sent to ip origination.
        */
        cmcall_send_ipcall_orig (call_ptr);

      }
      else if (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA)
      {

        #ifdef FEATURE_IS2000_REL_A
        if( sr_ptr->is_re_orig==TRUE )
        {
          if ( sr_ptr->orig_fail_layer == CM_CALL_ORIG_ERR_LAYER_L2 )
          {
            sr_ptr->re_orig_reason = TRUE;
            sr_ptr->re_orig_count ++;
          }
          else if ( sr_ptr->orig_fail_layer == CM_CALL_ORIG_ERR_LAYER_L3 )
          {
            sr_ptr->re_orig_reason = FALSE;
            sr_ptr->re_orig_count = 0;
          }
        }
        sr_ptr->is_re_orig    = TRUE;
        #endif /* FEATURE_IS2000_REL_A */

        #if defined (FEATURE_CDMA_800)|| defined (FEATURE_CDMA_1900)
        cmxcall_send_orig( call_ptr );
        #endif
      }
      else
      {
        #if (defined FEATURE_GSM) || (defined FEATURE_WCDMA)
#error code not present
        #endif //     #if (defined FEATURE_GSM) || (defined FEATURE_WCDMA)
      } /* else */

      return;

    } /* call_ptr->end_status != CM_CALL_END_ORIG_THR */

  }/* call_ptr->end_status == CM_CALL_END_NONE */

  /*-------------------------------------------------------------------------
                 If we got here, origination attempt failed
  -------------------------------------------------------------------------*/

  #ifdef FEATURE_CALL_THROTTLE

  /* Check if call throttle table needs to be modified
  */
  cmcall_check_if_orig_thr_and_add_entry( call_ptr );

  #endif /*FEATURE_CALL_THROTTLE*/

  /* Figure out what to do next now that origination attempt failed */
  redial_act = cmcall_act_get(call_ptr);

  CM_MSG_HIGH( "SRFailed EndStatus=%d Redial act=%d",
               call_ptr->end_status, redial_act, 0 );

  /* Take the next step as indicated by the origination plan.
  */
  switch( redial_act )
  {
    case CMCALL_ORIG_ACT_NONE:

      /* We found no action in silent redial table for the received
         call end status.
         This is an error condition and should never happen.
         But if it does, flag an error and end the call.
      */
      CM_ERR("No action found for call end status %d", call_ptr->end_status, 0, 0);
      call_ptr->end_status = CM_CALL_END_INTERNAL_ERROR;
      cmcall_end( call_ptr );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMCALL_ACT_END_ORIG:

      /* End the call origination.
      */
      #if (defined FEATURE_GSM) || (defined FEATURE_WCDMA)
#error code not present
      #endif  /* FEATURE_GSM || FEATURE_WCDMA */
      {
        cmcall_end( call_ptr );
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMCALL_ACT_REDIAL:

      /* If no more silent redials are left, end the call origination.
      */
      if( ! sr_is_more_redial(sr_ptr, uptime) )
      {
        #if (defined FEATURE_GSM) || (defined FEATURE_WCDMA)
#error code not present
        #endif  /* FEATURE_GSM || FEATURE_WCDMA */
        cmcall_end( call_ptr );
        break;
      }

      /* 1. Reset call end status to none.
      ** 2. Force the mode preference (to get the ok_to_orig on more time).
      ** 3. Schedule next silent redial.
      */

      #ifdef FEATURE_HDR_HYBRID
#error code not present
      #endif /* FEATURE_HDR_HYBRID */
      {

        (void) cmcall_force_pref_on_the_fly( call_ptr,
                                           SD_SS_PREF_REAS_REDIAL_NORM );

        sr_sched_next( sr_ptr,
                       orig_plan_ptr->sr_period_time, /* SR off period */
                       0,                             /* Hold orig period */
                       CM_SR_ACT_DEC,
                       call_ptr->end_status,
                       redial_act
                     );
        is_call_ended = FALSE;
      }

      /*lint -save -e774 Boolean within 'if' always evaluates to True
      **                 Not true, Depending on feature flags it might
      **                 not be true always
      */

      if( !is_call_ended )
      {
        call_ptr->end_status = CM_CALL_END_NONE;
      }

      /*lint -restore */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    case CMCALL_ACT_REDIAL_SHORT:

      /* If no more silent redials are left, end the call origination.
      */
      if( ! sr_is_more_redial(sr_ptr, uptime) )
      {
        #if (defined FEATURE_GSM) || (defined FEATURE_WCDMA)
#error code not present
        #endif  /* FEATURE_GSM || FEATURE_WCDMA */
        cmcall_end( call_ptr );

        break;
      }

      /* 1. Reset call end status to none.
      ** 2. Force the mode preference. (to get the ok_to_orig on more time).
      ** 3. Schedule next silent redial.
      */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_HDR_HYBRID
#error code not present
      #endif /* FEATURE_HDR_HYBRID */
      {
        #if ( defined (FEATURE_UNIFORM_SCAN_OOS) || \
              defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING) )
        if( !( call_ptr->end_status == CM_CALL_END_CM_COLLOC_ACQ_FAIL &&
                 cmcall_check_if_no_srv(TRUE) ))
        {
        #endif

          (void) cmcall_force_pref_on_the_fly( call_ptr,
                                             SD_SS_PREF_REAS_REDIAL_NORM );

        #if ( defined (FEATURE_UNIFORM_SCAN_OOS) || \
              defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING) )
        }

        if( call_ptr->ss == CM_SS_HDR && cmcall_check_if_no_srv(TRUE) )
        {
          sr_sched_next( sr_ptr,
                       1,                              /* SR off period */
                       orig_plan_ptr->sr_period_time,  /* Hold orig period */
                       CM_SR_ACT_NO_CHANGE,
                       call_ptr->end_status,
                       redial_act
                     );

        }
        else
        #endif
        {

          sr_sched_next( sr_ptr,
                       1,                              /* SR off period */
                       orig_plan_ptr->sr_period_time,  /* Hold orig period */
                       CM_SR_ACT_DEC,
                       call_ptr->end_status,
                       redial_act
                     );
        }
        is_call_ended = FALSE;

      }

      /*lint -save -e774 Boolean within 'if' always evaluates to True
      **                 Not true, Depending on feature flags it might
      **                 not be true always
      */

      if( !is_call_ended )
      {
        call_ptr->end_status = CM_CALL_END_NONE;
      }

      /*lint -restore */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    case CMCALL_ACT_REDIAL_SHORT_SAME_SR:


      /* If no more silent redials are left, end the call origination.
      */
      if( ! sr_is_more_redial(sr_ptr, uptime) )
      {
        #if (defined FEATURE_GSM) || (defined FEATURE_WCDMA)
#error code not present
        #endif  /* FEATURE_GSM || FEATURE_WCDMA */
        cmcall_end( call_ptr );

        break;
      }


      /* 1. Reset call end status to none.
      ** 2. Force the mode preference. (to get the ok_to_orig on more time).
      ** 3. Schedule next silent redial.
      */

      #if ( defined (FEATURE_UNIFORM_SCAN_OOS) || \
          defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING) )
      if( !( call_ptr->end_status == CM_CALL_END_NO_HYBR_HDR_SRV &&
             cmcall_check_if_no_srv(TRUE) ))
      #endif
      {
        (void) cmcall_force_pref_on_the_fly( call_ptr,
                                          SD_SS_PREF_REAS_REDIAL_NORM );
      }

      sr_sched_next( sr_ptr,
                     1,                              /* SR off period */
                     orig_plan_ptr->sr_period_time,  /* Hold orig period */
                     CM_SR_ACT_NO_CHANGE,
                     call_ptr->end_status,
                     redial_act
                   );
       call_ptr->end_status = CM_CALL_END_NONE;

      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMCALL_ACT_WAIT_OK_TO_ORIG:

      /* 1. Reset call end status to none.
      ** 2. Schedule next silent redial without decrementing SRLeft.
      */

      sr_sched_next( sr_ptr,
                     1,                              /* SR off period */
                     CMCALL_ACC_HOLD_ORIG_TIME,      /* Hold orig period */
                     CM_SR_ACT_NO_CHANGE,
                     call_ptr->end_status,
                     redial_act
                   );
      call_ptr->end_status = CM_CALL_END_NONE;


      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMCALL_ACT_WAIT_OK_TO_ORIG_SHORT:

      /* If no more silent redials are left, end the call origination.
      */
      if( ! sr_is_more_redial(sr_ptr, uptime) )
      {
        #if (defined FEATURE_GSM) || (defined FEATURE_WCDMA)
#error code not present
        #endif  /* FEATURE_GSM || FEATURE_WCDMA */
        cmcall_end( call_ptr );

        break;
      }

      /* 1. Reset call end status to none.
      ** 2. Schedule next silent redial decrementing SRLeft.
      */

      sr_sched_next( sr_ptr,
                     1,                              /* SR off period */
                     orig_plan_ptr->sr_period_time,  /* Hold orig period */
                     CM_SR_ACT_DEC,
                     call_ptr->end_status,
                     redial_act
                   );

      call_ptr->end_status = CM_CALL_END_NONE;

      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMCALL_ACT_FORCE_REDIAL:

      /* 1. Reset call end status to none.
      ** 2. Schedule next silent redial decrementing SRLeft.
      */

      sr_sched_next( sr_ptr,
                     1,                              /* SR off period */
                     CMCALL_ACC_HOLD_ORIG_TIME,      /* Hold orig period */
                     CM_SR_ACT_DEC,
                     call_ptr->end_status,
                     redial_act
                   );
      call_ptr->end_status = CM_CALL_END_NONE;

      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
    case CMCALL_ACT_FORCE_AMPS:

      /* If no more silent redials are left, end the call origination.
      */
      if( ! sr_is_more_redial(sr_ptr, uptime) )
      {
        cmcall_end( call_ptr );
        break;
      }

      /* 1. Reset call end status to none.
      ** 2. Force analog as necessary.
      ** 3. Schedule next silent redial.
      */



      no_srv_time = cmcall_force_analog_on_the_fly( call_ptr, 0 );

      sr_sched_next( sr_ptr,
                     orig_plan_ptr->sr_period_time, /* SR off period */
                     no_srv_time,                   /* Hold orig period */
                     BOOLEAN(no_srv_time)?CM_SR_ACT_RESTART:CM_SR_ACT_DEC,
                          /* Restart SR if forcing AMPS */
                     call_ptr->end_status,
                     redial_act
                    );

      call_ptr->end_status = CM_CALL_END_NONE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMCALL_ACT_FORCE_AMPS_RSSI:

      /* If no more silent redials are left, end the call origination.
      */
      if( ! sr_is_more_redial(sr_ptr, uptime) )
      {
        cmcall_end( call_ptr );
        break;
      }

      /* 1. Reset call end status to none.
      ** 2. Force analog as necessary.
      ** 3. Schedule next silent redial.
      */

      no_srv_time = cmcall_force_analog_on_the_fly(
                                           call_ptr,
                                           CMCALL_SR_FORCE_AMPS_RSSI_THR );

      sr_sched_next( sr_ptr,
                     orig_plan_ptr->sr_period_time, /* SR off period */
                     no_srv_time,                   /* Hold orig period */
                     BOOLEAN(no_srv_time)?CM_SR_ACT_RESTART:CM_SR_ACT_DEC,
                          /* Restart SR if forcing AMPS */
                     call_ptr->end_status,
                     redial_act
                    );

      call_ptr->end_status = CM_CALL_END_NONE;

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMCALL_ACT_FORCE_AUTO:


      /* If no more silent redials are left, end the call origination.
      */
      if( ! sr_is_more_redial(sr_ptr, uptime) )
      {
        cmcall_end( call_ptr );
        break;
      }

      /* 1. Reset call end status to none.
      ** 2. Force optimized silent redial.
      ** 3. Schedule next silent redial.
      */

      #ifdef FEATURE_HDR_HYBRID
#error code not present
      #endif /* FEATURE_HDR_HYBRID */
      {

        no_srv_time = cmcall_force_sr_on_the_fly(
                                          call_ptr,
                                          0,        /* Ignore RSSI reading */
                                          FALSE );  /* Don't force roam pref */

        sr_sched_next( sr_ptr,
                       orig_plan_ptr->sr_period_time, /* SR off period */
                       no_srv_time,                   /* Hold orig period */
                       BOOLEAN(no_srv_time)?CM_SR_ACT_RESTART:CM_SR_ACT_DEC,
                            /* Restart SR if forcing Optimized SR */
                       call_ptr->end_status,
                       redial_act
                     );
        is_call_ended = FALSE;
      }

      /*lint -save -e774 Boolean within 'if' always evaluates to True
      **                 Not true, Depending on feature flags it might
      **                 not be true always
      */

      if( !is_call_ended )
      {
        call_ptr->end_status = CM_CALL_END_NONE;
      }

      /*lint -restore */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMCALL_ACT_FORCE_AUTO_RSSI:

      /* If no more silent redials are left, end the call origination.
      */
      if( ! sr_is_more_redial(sr_ptr, uptime) )
      {
        cmcall_end( call_ptr );
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* 1. Reset call end status to none.
      ** 2. Force optimized silent redial (as necessary).
      ** 3. Schedule next silent redial.
      */


      #ifdef FEATURE_HDR_HYBRID
#error code not present
      #endif /* FEATURE_HDR_HYBRID */
      {
        no_srv_time = cmcall_force_sr_on_the_fly(
                                          call_ptr,
                                          CMCALL_SR_FORCE_AMPS_RSSI_THR,
                                          FALSE );  /* Don't force roam pref */

        sr_sched_next( sr_ptr,
                       orig_plan_ptr->sr_period_time, /* SR off period */
                       no_srv_time,                   /* Hold orig period */
                       BOOLEAN(no_srv_time)?CM_SR_ACT_RESTART:CM_SR_ACT_DEC,
                            /* Restart SR if forcing Optimized SR */
                       call_ptr->end_status,
                       redial_act
                     );
        is_call_ended = FALSE;
      }
      /*lint -save -e774 Boolean within 'if' always evaluates to True
      **                 Not true, Depending on feature flags it might
      **                 not be true always
      */

      if( !is_call_ended )
      {
        call_ptr->end_status = CM_CALL_END_NONE;
      }

      /*lint -restore */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


   #ifdef FEATURE_HDR_HYBRID
#error code not present
    #endif /* FEATURE_HDR_HYBRID */

    #ifdef FEATURE_CALL_THROTTLE

    case CMCALL_ACT_REDIAL_HDR_THR:

      /* If the call was throttled on 1X end the call now
      */
      if(!( call_ptr->ss == CM_SS_HDR ||
                        ( ss_ptr->info.hdr_hybrid                   &&
                          call_ptr->ss          == CM_SS_MAIN       &&
                          ss_ptr->info.sys_mode == SYS_SYS_MODE_HDR
                        )
        ))
      {
        /* Check is the origination was sent to 1X or CM is throttling the
        ** call origination
        ** If CM is throttling the call origination, then hold the call_end
        ** for atmost 1 second
        */

        if( sr_ptr->orig_sent_on_mode == SYS_SYS_MODE_NONE )
        {
          if( !sr_ptr->is_call_throttled )
          {
            /* Wait CMCALL_ORIG_THR_HOLD_UPTIME before sending call
            ** end to clients
            */
            sr_ptr->is_call_throttled = TRUE;
            sr_ptr->sr_max_uptime     = uptime + CMCALL_ORIG_THR_HOLD_UPTIME;
            break;

          }
          else if( uptime < sr_ptr->sr_max_uptime )
          {
            /* We are still in the hold timer
            */
            break;
          }

        }

        /* End the call
        */
        cmcall_end( call_ptr );
        break;
      }
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_HDR_HYBRID
#error code not present
      #else
      cmcall_end( call_ptr );
      break;
      #endif /* FEATURE_HDR_HYBRID */

    #endif /* FEATURE_CALL_THROTTLE */
    #endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (defined FEATURE_GSM) || (defined FEATURE_WCDMA)
#error code not present
    #endif  /* FEATURE_GSM || FEATURE_WCDMA */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (defined(FEATURE_IP_CALL) && defined(FEATURE_WLAN))
#error code not present
    #endif

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if defined(FEATURE_IP_CALL)
    case CMCALL_ACT_FORCE_NON_IP:

      /* If no more silent redials are left, end the call origination.
      */
      if( ! sr_is_more_redial(sr_ptr, uptime) )
      {
        cmcall_end( call_ptr );
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* 1. Reset call end status to none.
      ** 2. Use CDMA or GW for orig.
      */

      no_srv_time = cmcall_remove_mode_and_force(call_ptr,
                                                 CM_MODE_PREF_HDR_WLAN);

      /* Schedule the next SR.
      */
      sr_sched_next( &call_ptr->sr,
                     orig_plan_ptr->sr_period_time,    /* SR off period */
                     no_srv_time,                      /* Hold orig period */
                     BOOLEAN(no_srv_time)?CM_SR_ACT_RESTART:CM_SR_ACT_DEC,
                     call_ptr->end_status,
                     redial_act
                   );
      call_ptr->end_status = CM_CALL_END_NONE;

      break;
    #endif /* FEATURE_IP_CALL */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CMCALL_ACT_WAIT_OTHER_END:

      /* If no more silent redials are left, end the call origination.
      */
      if( ! sr_is_more_redial(sr_ptr, uptime) )
      {
        cmcall_end( call_ptr );
        break;
      }

      call_ptr->end_status = CM_CALL_END_NONE;
      call_ptr->sr.orig_sent_on_mode = SYS_SYS_MODE_NONE;

      #ifdef FEATURE_HDR_HYBRID
#error code not present
      #endif

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMCALL_ORIG_ACT_MAX:
    default:
      CM_SWITCH_ERR("Bad redial_act = %d",redial_act,0,0);
      /*
      ** 527: Unreachable
      ** This is because CM_SWITCH_ERR() evaluates to error fatal
      ** (i.e exit program)
      ** But for different feature set it may not to, so we want to
      ** supress the warning here
      */
      /*lint -save -e527 */
      cmcall_end( call_ptr );
      /*lint -restore */
      break;
  } /* switch( cmcall_act_get(call_ptr) ) */

} /* cmcall_orig_proc() */



/*===========================================================================

FUNCTION cmcall_load_orig_plan

DESCRIPTION
  Load an origination plan in accordance with the call type and/or
  a particular dialed number we are trying to originate.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_load_orig_plan(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  const cmph_s_type        *ph_ptr        = cmph_ptr();
      /* point at phone object */

  cm_sr_s_type             *sr_ptr        = NULL;
      /* point silent redial component */

  unsigned int i;
      /* loop counter */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Call origination plan table.
  **
  ** This table indicate the SR parameters and origination action that
  ** should be used for the various call types and or dial strings.
  */
  static const cmcall_action_table_entry_s_type e911_action_table_entry[] = {

        { CM_CALL_END_HDR_CS_TIMEOUT,          CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_CD_GEN_OR_BUSY,          CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CD_BILL_OR_AUTH,         CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CHG_HDR,                 CMCALL_ACT_WAIT_OK_TO_ORIG },
        { CM_CALL_END_EXIT_HDR,                CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_HDR_NO_SESSION,          CMCALL_ACT_WAIT_OK_TO_ORIG_SHORT },
        { CM_CALL_END_CDMA_LOCK,               CMCALL_ACT_REDIAL              },
        { CM_CALL_END_NO_CDMA_SRV,             CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NO_HYBR_HDR_SRV,         CMCALL_ACT_REDIAL_SHORT_SAME_SR},
        { CM_CALL_END_FADE,                    CMCALL_ACT_REDIAL              },
        #ifdef FEATURE_JCDMA_CM
        { CM_CALL_END_REORDER,                 CMCALL_ACT_END_ORIG            },
        #else
        { CM_CALL_END_REORDER,                 CMCALL_ACT_REDIAL              },
        #endif /* FEATURE_JCDMA_CM */
        { CM_CALL_END_INTERCEPT,               CMCALL_ACT_REDIAL              },
        { CM_CALL_END_REL_NORMAL,              CMCALL_ACT_REDIAL              },
        { CM_CALL_END_REL_SO_REJ,              CMCALL_ACT_REDIAL              },
        #ifdef FEATURE_JCDMA_CM
        { CM_CALL_END_MC_ABORT,                CMCALL_ACT_END_ORIG            },
        #else
        { CM_CALL_END_MC_ABORT,                CMCALL_ACT_REDIAL              },
        #endif
        { CM_CALL_END_MAX_ACCESS_PROBE,        CMCALL_ACT_REDIAL              },
        { CM_CALL_END_PSIST_NG,                CMCALL_ACT_REDIAL              },
        /* New for SD 2.0 */
        { CM_CALL_END_ACC_IN_PROG,             CMCALL_ACT_WAIT_OK_TO_ORIG     },
        { CM_CALL_END_ACC_FAIL,                CMCALL_ACT_REDIAL              },
        { CM_CALL_END_ACCESS_BLOCK,            CMCALL_ACT_REDIAL              },
        { CM_CALL_END_ACCESS_BLOCK_ALL,        CMCALL_ACT_REDIAL              },
        { CM_CALL_END_NO_RESPONSE_FROM_BS,     CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS, CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REJECTED_BY_BS,          CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_ALREADY_IN_TC,           CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_INCOMPATIBLE,            CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_NO_GW_SRV,               CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NETWORK_END,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REDIR_OR_HANDOFF,        CMCALL_ACT_FORCE_REDIAL        },
        { CM_CALL_END_NO_FULL_SRV,             CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_HDR_NO_LOCK_GRANTED,     CMCALL_ACT_REDIAL_SHORT_SAME_SR},
        { CM_CALL_END_LL_CAUSE,                CMCALL_ACT_PROC_END_REASON     },
        { CM_CALL_END_NO_WLAN_SRV,             CMCALL_ACT_REDIAL              },
        { CM_CALL_END_VOIP_FAIL_WLAN,          CMCALL_ACT_FORCE_NON_WLAN      },
        { CM_CALL_END_IP_FAIL,                 CMCALL_ACT_FORCE_NON_IP        },
        { CM_CALL_END_CM_COLLOC_ACQ_FAIL,      CMCALL_ACT_FORCE_NON_IP        }          
  };

  static const cmcall_orig_plan_action_table_s_type e911_action_table = {
     ARR_SIZE(e911_action_table_entry),
     (cmcall_action_table_entry_s_type*) e911_action_table_entry
  };

#if defined ( FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
  static const cmcall_action_table_entry_s_type otasp_action_table_entry[] = {
        { CM_CALL_END_HDR_CS_TIMEOUT,          CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_CD_GEN_OR_BUSY,          CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CD_BILL_OR_AUTH,         CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CHG_HDR,                 CMCALL_ACT_WAIT_OK_TO_ORIG },
        { CM_CALL_END_EXIT_HDR,                CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_HDR_NO_SESSION,          CMCALL_ACT_WAIT_OK_TO_ORIG_SHORT },
        { CM_CALL_END_CDMA_LOCK,               CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_NO_CDMA_SRV,             CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NO_HYBR_HDR_SRV,         CMCALL_ACT_REDIAL_SHORT_SAME_SR},
        { CM_CALL_END_FADE,                    CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REORDER,                 CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_INTERCEPT,               CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REL_NORMAL,              CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REL_SO_REJ,              CMCALL_ACT_END_ORIG            },
        #ifdef FEATURE_JCDMA_CM
        { CM_CALL_END_MC_ABORT,                CMCALL_ACT_END_ORIG            },
        #else
        { CM_CALL_END_MC_ABORT,                CMCALL_ACT_REDIAL              },
        #endif
        { CM_CALL_END_MAX_ACCESS_PROBE,        CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_PSIST_NG,                CMCALL_ACT_END_ORIG            },
        /* New for SD 2.0 */
        { CM_CALL_END_ACC_IN_PROG,             CMCALL_ACT_WAIT_OK_TO_ORIG     },
        { CM_CALL_END_ACC_FAIL,                CMCALL_ACT_REDIAL              },
        { CM_CALL_END_ACCESS_BLOCK,            CMCALL_ACT_REDIAL              },
        { CM_CALL_END_ACCESS_BLOCK_ALL,        CMCALL_ACT_REDIAL              },
        { CM_CALL_END_NO_RESPONSE_FROM_BS,     CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS, CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REJECTED_BY_BS,          CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_ALREADY_IN_TC,           CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_INCOMPATIBLE,            CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_NO_GW_SRV,               CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NETWORK_END,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REDIR_OR_HANDOFF,        CMCALL_ACT_FORCE_REDIAL        },
        { CM_CALL_END_NO_FULL_SRV,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_HDR_NO_LOCK_GRANTED,     CMCALL_ACT_REDIAL_SHORT_SAME_SR},
        { CM_CALL_END_LL_CAUSE,                CMCALL_ACT_PROC_END_REASON     },
        { CM_CALL_END_NO_WLAN_SRV,             CMCALL_ACT_REDIAL              },
        { CM_CALL_END_VOIP_FAIL_WLAN,          CMCALL_ACT_FORCE_NON_WLAN      },
        { CM_CALL_END_IP_FAIL,                 CMCALL_ACT_FORCE_NON_IP        },
        { CM_CALL_END_CM_COLLOC_ACQ_FAIL,      CMCALL_ACT_FORCE_NON_IP        }
  };

  static const cmcall_orig_plan_action_table_s_type otasp_action_table = {
    ARR_SIZE(otasp_action_table_entry),
    (cmcall_action_table_entry_s_type*) otasp_action_table_entry
  };

    #ifdef FEATURE_OTASP_2
#error code not present
    #endif /* FEATURE_OTASP_2 */

  static const cmcall_action_table_entry_s_type hdr_pkt_data_action_table_entry[] = {
        { CM_CALL_END_HDR_CS_TIMEOUT,          CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_CD_GEN_OR_BUSY,          CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CD_BILL_OR_AUTH,         CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_CHG_HDR,                 CMCALL_ACT_WAIT_OK_TO_ORIG     },
        { CM_CALL_END_EXIT_HDR,                CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_HDR_NO_SESSION,          CMCALL_ACT_WAIT_OK_TO_ORIG_SHORT },
        { CM_CALL_END_CDMA_LOCK,               CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_NO_CDMA_SRV,             CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NO_HYBR_HDR_SRV,         CMCALL_ACT_REDIAL_SHORT_SAME_SR},
        { CM_CALL_END_FADE,                    CMCALL_ACT_FORCE_AUTO_RSSI     },
        { CM_CALL_END_HOLD_OTHER_IN_PROG,      CMCALL_ACT_WAIT_OTHER_END      },
        { CM_CALL_END_OTASP_COMMIT_IN_PROG,    CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_REORDER,                 CMCALL_ACT_FORCE_AUTO_RSSI     },
        { CM_CALL_END_INTERCEPT,               CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REL_NORMAL,              CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REL_SO_REJ,              CMCALL_ACT_END_ORIG            },
        #ifdef FEATURE_JCDMA_CM
        { CM_CALL_END_MC_ABORT,                CMCALL_ACT_END_ORIG            },
        #else
        { CM_CALL_END_MC_ABORT,                CMCALL_ACT_REDIAL              },
        #endif
        { CM_CALL_END_PSIST_NG,                CMCALL_ACT_REDIAL              },

        /* New for SD 2.0 */
        { CM_CALL_END_ACC_IN_PROG,             CMCALL_ACT_WAIT_OK_TO_ORIG     },
        { CM_CALL_END_ACC_FAIL,                CMCALL_ACT_FORCE_AUTO          },
        { CM_CALL_END_ACCESS_BLOCK,            CMCALL_ACT_REDIAL              },
        { CM_CALL_END_ACCESS_BLOCK_ALL,        CMCALL_ACT_REDIAL              },
        { CM_CALL_END_NO_RESPONSE_FROM_BS,     CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS, CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REJECTED_BY_BS,          CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_ALREADY_IN_TC,           CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_INCOMPATIBLE,            CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_CM_COLLOC_ACQ_FAIL,      CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NO_GW_SRV,               CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NETWORK_END,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REDIR_OR_HANDOFF,        CMCALL_ACT_FORCE_REDIAL        },
        { CM_CALL_END_NO_FULL_SRV,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_HDR_NO_LOCK_GRANTED,     CMCALL_ACT_REDIAL_SHORT_SAME_SR},
        { CM_CALL_END_LL_CAUSE,                CMCALL_ACT_PROC_END_REASON     },
        { CM_CALL_END_NO_WLAN_SRV,             CMCALL_ACT_REDIAL              },
        { CM_CALL_END_VOIP_FAIL_WLAN,          CMCALL_ACT_FORCE_NON_WLAN      },
        { CM_CALL_END_ORIG_THR,                CMCALL_ACT_REDIAL_HDR_THR      },
        { CM_CALL_END_IP_FAIL,                 CMCALL_ACT_FORCE_NON_IP        },
        { CM_CALL_END_IS707B_MAX_ACC,          CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_MAX_ACCESS_PROBE,        CMCALL_ACT_FORCE_AUTO          }
  };

  static const cmcall_orig_plan_action_table_s_type hdr_pkt_data_action_table = {
    ARR_SIZE(hdr_pkt_data_action_table_entry),
    (cmcall_action_table_entry_s_type*) hdr_pkt_data_action_table_entry
  };


  static const cmcall_action_table_entry_s_type amps_action_table_entry [] = {
        { CM_CALL_END_HDR_CS_TIMEOUT,          CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_CD_GEN_OR_BUSY,          CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CD_BILL_OR_AUTH,         CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CHG_HDR,                 CMCALL_ACT_WAIT_OK_TO_ORIG },
        { CM_CALL_END_EXIT_HDR,                CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_HDR_NO_SESSION,          CMCALL_ACT_WAIT_OK_TO_ORIG_SHORT },
        { CM_CALL_END_CDMA_LOCK,               CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_NO_CDMA_SRV,             CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NO_HYBR_HDR_SRV,         CMCALL_ACT_REDIAL_SHORT_SAME_SR},
        { CM_CALL_END_FADE,                    CMCALL_ACT_FORCE_AMPS_RSSI     },
        { CM_CALL_END_HOLD_OTHER_IN_PROG,      CMCALL_ACT_WAIT_OTHER_END      },
        { CM_CALL_END_OTASP_COMMIT_IN_PROG,    CMCALL_ACT_REDIAL_SHORT        },
        #ifdef FEATURE_JCDMA_CM
        { CM_CALL_END_REORDER,                 CMCALL_ACT_END_ORIG            },
        #else
        { CM_CALL_END_REORDER,                 CMCALL_ACT_FORCE_AMPS_RSSI     },
        #endif /* FEATURE_JCDMA_CM */
        { CM_CALL_END_INTERCEPT,               CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REL_NORMAL,              CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REL_SO_REJ,              CMCALL_ACT_END_ORIG            },
        #ifdef FEATURE_JCDMA_CM
        { CM_CALL_END_MC_ABORT,                CMCALL_ACT_END_ORIG            },
        #else
        { CM_CALL_END_MC_ABORT,                CMCALL_ACT_REDIAL              },
        #endif
        #if (defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900))
        { CM_CALL_END_MAX_ACCESS_PROBE,        CMCALL_ACT_FORCE_AUTO          },
        #else
        { CM_CALL_END_MAX_ACCESS_PROBE,        CMCALL_ACT_REDIAL              },
        #endif
        { CM_CALL_END_PSIST_NG,                CMCALL_ACT_REDIAL              },
        /* New for SD 2.0 */
        { CM_CALL_END_ACC_IN_PROG,             CMCALL_ACT_WAIT_OK_TO_ORIG     },
        #if (defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900))
        { CM_CALL_END_ACC_FAIL,                CMCALL_ACT_FORCE_AUTO          },
        #else
        { CM_CALL_END_ACC_FAIL,                CMCALL_ACT_REDIAL              },
        #endif
        { CM_CALL_END_ACCESS_BLOCK,            CMCALL_ACT_REDIAL              },
        { CM_CALL_END_ACCESS_BLOCK_ALL,        CMCALL_ACT_REDIAL              },
        { CM_CALL_END_NO_RESPONSE_FROM_BS,     CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS, CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REJECTED_BY_BS,          CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_ALREADY_IN_TC,           CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_INCOMPATIBLE,            CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_NO_GW_SRV,               CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NETWORK_END,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REDIR_OR_HANDOFF,        CMCALL_ACT_FORCE_REDIAL        },
        { CM_CALL_END_NO_FULL_SRV,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_HDR_NO_LOCK_GRANTED,     CMCALL_ACT_REDIAL_SHORT_SAME_SR},
        { CM_CALL_END_LL_CAUSE,                CMCALL_ACT_PROC_END_REASON     },
        { CM_CALL_END_NO_WLAN_SRV,             CMCALL_ACT_REDIAL              },
        { CM_CALL_END_VOIP_FAIL_WLAN,          CMCALL_ACT_FORCE_NON_WLAN      },
        { CM_CALL_END_IP_FAIL,                 CMCALL_ACT_FORCE_NON_IP        },
        { CM_CALL_END_CM_COLLOC_ACQ_FAIL,      CMCALL_ACT_FORCE_NON_IP        }
  };

  static const cmcall_orig_plan_action_table_s_type amps_action_table = {
    ARR_SIZE(amps_action_table_entry),
    (cmcall_action_table_entry_s_type*) amps_action_table_entry
  };

#endif /* (FEATURE_CDMA_800) || (FEATURE_CDMA_1900) */

#if ( ( defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900) ) || \
      ( defined (FEATURE_CM_TRIBAND_ORIG_PLAN_TBL) ))
  static const cmcall_action_table_entry_s_type trimode_no_amps_action_table_entry [] = {
        { CM_CALL_END_HDR_CS_TIMEOUT,          CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_CD_GEN_OR_BUSY,          CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CD_BILL_OR_AUTH,         CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CHG_HDR,                 CMCALL_ACT_WAIT_OK_TO_ORIG },
        { CM_CALL_END_EXIT_HDR,                CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_HDR_NO_SESSION,          CMCALL_ACT_WAIT_OK_TO_ORIG_SHORT },
        { CM_CALL_END_NO_HYBR_HDR_SRV,         CMCALL_ACT_REDIAL_SHORT_SAME_SR},
        { CM_CALL_END_CDMA_LOCK,               CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_NO_CDMA_SRV,             CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_FADE,                    CMCALL_ACT_FORCE_AUTO_RSSI     },
        { CM_CALL_END_HOLD_OTHER_IN_PROG,      CMCALL_ACT_WAIT_OTHER_END      },
        { CM_CALL_END_OTASP_COMMIT_IN_PROG,    CMCALL_ACT_REDIAL_SHORT        },
        #ifdef FEATURE_JCDMA_CM
        { CM_CALL_END_REORDER,                 CMCALL_ACT_END_ORIG            },
        #else
        { CM_CALL_END_REORDER,                 CMCALL_ACT_FORCE_AUTO_RSSI     },
        #endif /* FEATURE_JCDMA_CM */
        { CM_CALL_END_INTERCEPT,               CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REL_NORMAL,              CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REL_SO_REJ,              CMCALL_ACT_END_ORIG            },
        #ifdef FEATURE_JCDMA_CM
        { CM_CALL_END_MC_ABORT,                CMCALL_ACT_END_ORIG            },
        #else
        { CM_CALL_END_MC_ABORT,                CMCALL_ACT_REDIAL              },
        #endif
        { CM_CALL_END_MAX_ACCESS_PROBE,        CMCALL_ACT_FORCE_AUTO          },
        { CM_CALL_END_PSIST_NG,                CMCALL_ACT_REDIAL              },
        { CM_CALL_END_ACC_IN_PROG,             CMCALL_ACT_WAIT_OK_TO_ORIG     },
        { CM_CALL_END_ACC_FAIL,                CMCALL_ACT_FORCE_AUTO          },
        { CM_CALL_END_ACCESS_BLOCK,            CMCALL_ACT_REDIAL              },
        { CM_CALL_END_ACCESS_BLOCK_ALL,        CMCALL_ACT_REDIAL              },
        { CM_CALL_END_NO_RESPONSE_FROM_BS,     CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS, CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REJECTED_BY_BS,          CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_ALREADY_IN_TC,           CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_INCOMPATIBLE,            CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_NO_GW_SRV,               CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NETWORK_END,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REDIR_OR_HANDOFF,        CMCALL_ACT_FORCE_REDIAL        },
        { CM_CALL_END_NO_FULL_SRV,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_HDR_NO_LOCK_GRANTED,     CMCALL_ACT_REDIAL_SHORT_SAME_SR},
        { CM_CALL_END_LL_CAUSE,                CMCALL_ACT_PROC_END_REASON     },
        { CM_CALL_END_NO_WLAN_SRV,             CMCALL_ACT_REDIAL              },
        { CM_CALL_END_VOIP_FAIL_WLAN,          CMCALL_ACT_FORCE_NON_WLAN      },
        { CM_CALL_END_ORIG_THR,                CMCALL_ACT_REDIAL_HDR_THR      },
        { CM_CALL_END_IP_FAIL,                 CMCALL_ACT_FORCE_NON_IP        },
        { CM_CALL_END_IS707B_MAX_ACC,          CMCALL_ACT_END_ORIG            },          
        { CM_CALL_END_CM_COLLOC_ACQ_FAIL,      CMCALL_ACT_FORCE_NON_IP        }
  };

  static const cmcall_orig_plan_action_table_s_type trimode_no_amps_action_table = {
    ARR_SIZE(trimode_no_amps_action_table_entry),
    (cmcall_action_table_entry_s_type*) trimode_no_amps_action_table_entry
  };

#elif (defined (FEATURE_WCDMA) || defined (FEATURE_GSM)) && !defined (FEATURE_CDMA_800) && !defined (FEATURE_CDMA_1900)
#error code not present
#else

#if !defined (FEATURE_GSM) && !defined (FEATURE_WCDMA)

  static const cmcall_action_table_entry_s_type non_trimode_no_amps_action_table_entry [] = {
        { CM_CALL_END_HDR_CS_TIMEOUT,          CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_CD_GEN_OR_BUSY,          CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CD_BILL_OR_AUTH,         CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CHG_HDR,                 CMCALL_ACT_WAIT_OK_TO_ORIG },
        { CM_CALL_END_EXIT_HDR,                CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_HDR_NO_SESSION,          CMCALL_ACT_WAIT_OK_TO_ORIG_SHORT },
        { CM_CALL_END_CDMA_LOCK,               CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_NO_CDMA_SRV,             CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NO_HYBR_HDR_SRV,         CMCALL_ACT_REDIAL_SHORT_SAME_SR},
        { CM_CALL_END_FADE,                    CMCALL_ACT_REDIAL              },
        { CM_CALL_END_HOLD_OTHER_IN_PROG,      CMCALL_ACT_WAIT_OTHER_END      },
        { CM_CALL_END_OTASP_COMMIT_IN_PROG,    CMCALL_ACT_REDIAL_SHORT        },
        #ifdef FEATURE_JCDMA_CM
        { CM_CALL_END_REORDER,                 CMCALL_ACT_END_ORIG            },
        #else
        { CM_CALL_END_REORDER,                 CMCALL_ACT_REDIAL              },
        #endif /* FEATURE_JCDMA_CM */
        { CM_CALL_END_INTERCEPT,               CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REL_NORMAL,              CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REL_SO_REJ,              CMCALL_ACT_END_ORIG            },
        #ifdef FEATURE_JCDMA_CM
        { CM_CALL_END_MC_ABORT,                CMCALL_ACT_END_ORIG            },
        #else
        { CM_CALL_END_MC_ABORT,                CMCALL_ACT_REDIAL              },
        #endif
        { CM_CALL_END_MAX_ACCESS_PROBE,        CMCALL_ACT_REDIAL              },
        { CM_CALL_END_PSIST_NG,                CMCALL_ACT_REDIAL              },
        /* New for SD 2.0 */
        { CM_CALL_END_ACC_IN_PROG,             CMCALL_ACT_WAIT_OK_TO_ORIG     },
        { CM_CALL_END_ACC_FAIL,                CMCALL_ACT_REDIAL              },
        { CM_CALL_END_ACCESS_BLOCK,            CMCALL_ACT_REDIAL              },
        { CM_CALL_END_ACCESS_BLOCK_ALL,        CMCALL_ACT_REDIAL              },
        { CM_CALL_END_NO_RESPONSE_FROM_BS,     CMCALL_ACT_REDIAL              },
        { CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS, CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REJECTED_BY_BS,          CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_ALREADY_IN_TC,           CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_INCOMPATIBLE,            CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_NO_GW_SRV,               CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NETWORK_END,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_REDIR_OR_HANDOFF,        CMCALL_ACT_FORCE_REDIAL        },
        { CM_CALL_END_NO_FULL_SRV,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_HDR_NO_LOCK_GRANTED,     CMCALL_ACT_REDIAL_SHORT_SAME_SR},
        { CM_CALL_END_LL_CAUSE,                CMCALL_ACT_PROC_END_REASON     },
        { CM_CALL_END_NO_WLAN_SRV,             CMCALL_ACT_REDIAL              },
        { CM_CALL_END_VOIP_FAIL_WLAN,          CMCALL_ACT_FORCE_NON_WLAN      },
        { CM_CALL_END_ORIG_THR,                CMCALL_ACT_REDIAL_HDR_THR      },
        { CM_CALL_END_IP_FAIL,                 CMCALL_ACT_FORCE_NON_IP        },
        { CM_CALL_END_IS707B_MAX_ACC,          CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_CM_COLLOC_ACQ_FAIL,      CMCALL_ACT_FORCE_NON_IP        }
  };

  static const cmcall_orig_plan_action_table_s_type non_trimode_no_amps_action_table = {
    ARR_SIZE(non_trimode_no_amps_action_table_entry),
    (cmcall_action_table_entry_s_type*) non_trimode_no_amps_action_table_entry
  };

#endif /* !FEATURE_GSM && !FEATURE_WCDMA */

#endif  /* FEATURE_CDMA_800 && FEATURE_CDMA_1900 */

  static const cmcall_orig_plan_s_type orig_plan_tbl[] = {

    /* E911 call origination plan.
    */
    {
      /* Condition */
      CM_FEATURE_NONE,               /* SR with AMPS feature (don't care) */
      CM_CALL_TYPE_EMERGENCY,             /* call type (E911)*/
      NULL,                          /* particular dial string (none) */
      FALSE,                         /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */
      /* Origination plan */
      TRUE,                          /* honor MC hold-orig */
      0,                             /* power-up hold orig uptime */
      4,                             /* silent redial off period */
      1000000,                       /* max numbers of dial attempts */
      4000000,                       /* max time for silent redial sequence */
      (cmcall_orig_plan_action_table_s_type*) &e911_action_table             },

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if defined (FEATURE_CDMA_800)|| defined (FEATURE_CDMA_1900)
    /* Standard OTASP call origination plan for targets with System
    ** Determination 2.0 support.
    */
    {
      /* Condition */
      CM_FEATURE_NONE,               /* SR with AMPS feature (don't care) */
      CM_CALL_TYPE_STD_OTASP,        /* call type ( standard OTASP)*/
      NULL,                          /* particular dial string (none) */
      FALSE,                         /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */

      /* Origination plan */
      FALSE,                         /* Honor MC hold-orig */
      0,                             /* power-up hold orig uptime */
      4,                             /* silent redial off period */
      1000000,                       /* max numbers of dial attempts */
      4000000,                       /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &otasp_action_table           },

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_OTASP_2
#error code not present
    #endif /* FEATURE_OTASP_2 */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Force to collocated HDR call origination plan
    **
    */
    {
      /* Condition */
      CM_FEATURE_NONE,               /* SR with AMPS feature NONE */
      CM_CALL_TYPE_PKT_DATA,          /* call type (any) */
      NULL,                          /* particular dial string(none) */
      TRUE,                          /* hybrid call orig for pkt data call
                                        is allowed or not */
      CM_CALL_MODE_INFO_GW_PS,        /* Ignore the call mode */

      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_PS_DATA_SR_MAX_TIME,    /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &trimode_no_amps_action_table },


    /* Force to collocated HDR call origination plan
    **
    */
    {
      /* Condition */
      CM_FEATURE_NONE,               /* SR with AMPS feature NONE */
      CM_CALL_TYPE_PKT_DATA,          /* call type (any) */
      NULL,                          /* particular dial string(none) */
      TRUE,                          /* hybrid call orig for pkt data call
                                        is allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */

      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_SR_MAX_TIME,            /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &hdr_pkt_data_action_table    },


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Default call origination plan for silent redial with AMPS.
    */
    {
      /* Condition */
      CM_FEATURE_ON,                 /* SR with AMPS feature (ON) */
      CM_CALL_TYPE_NONE,             /* call type (any) */
      NULL,                          /* particular dial string (none) */
      FALSE,                         /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */

      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_SR_MAX_TIME,            /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &amps_action_table            },

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #endif /* FEATURE_CDMA_800) || (FEATURE_CDMA_1900)*/
    #if ( ( defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900) ) || \
        ( defined (FEATURE_CM_TRIBAND_ORIG_PLAN_TBL) ))
    /* Default call origination plan for silent redial with NO AMPS, Trimode.
    */
    {
      /* Condition */
      CM_FEATURE_OFF,                /* SR with AMPS feature (OFF) */
      CM_CALL_TYPE_PS_DATA,          /* call type (any) */
      NULL,                          /* particular (none) */
      FALSE,                          /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_GW_PS,       /* GW_PS call */


      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_PS_DATA_SR_MAX_TIME,    /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &trimode_no_amps_action_table },

    /* Default call origination plan for silent redial with NO AMPS, Trimode.
    */
    {
      /* Condition */
      CM_FEATURE_OFF,                /* SR with AMPS feature (OFF) */
      CM_CALL_TYPE_NONE,             /* call type (any) */
      NULL,                          /* particular (none) */
      FALSE,                         /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */


      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_SR_MAX_TIME,            /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &trimode_no_amps_action_table },

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #elif (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && \
          !defined (FEATURE_CDMA_800) && !defined (FEATURE_CDMA_1900)
#error code not present
    #elif defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
    /* Default call origination plan for silent redial with NO AMPS,
    ** non-Trimode.
    */
    {
      /* Condition */
      CM_FEATURE_OFF,                /* SR with AMPS feature (OFF) */
      CM_CALL_TYPE_NONE,             /* call type (any) */
      NULL,                          /* particular (none) */
      FALSE,                         /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */

      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_SR_MAX_TIME,            /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &non_trimode_no_amps_action_table },
    #endif /* ! TriMode */

  };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(call_ptr != NULL);
  CM_ASSERT(ph_ptr   != NULL );

  CM_MSG_MED( "Loading orig plan", 0, 0, 0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ptr        = &call_ptr->sr;

  /* Look for a match in the origination plan table.
  */
  for( i=0; i < ARR_SIZE(orig_plan_tbl); i++ )
  {
    if( (orig_plan_tbl[i].sr_with_amps == CM_FEATURE_NONE ||
         BOOLEAN(orig_plan_tbl[i].sr_with_amps) == ph_ptr->is_sr_with_amps)
                                   &&
        (orig_plan_tbl[i].call_type == CM_CALL_TYPE_NONE ||
         orig_plan_tbl[i].call_type == call_ptr->call_type)
                                   &&
        (orig_plan_tbl[i].dial_str == NULL ||
         (cm_num_cmp_exact(&call_ptr->num,
                           orig_plan_tbl[i].dial_str,
                           strlen((char*)orig_plan_tbl[i].dial_str))))
                                   &&
        #if (defined (FEATURE_HDR_HYBRID) || defined(FEATURE_HYBR_GW))
#error code not present
        #else
        ( !orig_plan_tbl[i].is_hybr_allowed )
        #endif
        &&
        ( orig_plan_tbl[i].call_mode_info == CM_CALL_MODE_INFO_NONE ||
          orig_plan_tbl[i].call_mode_info ==
                                       call_ptr->cmcall_mode_info.info_type )
      )
    {
      break;
    }
  }

  /* Make sure we find a match in the table and point the call
  ** origination plan pointer at the selected origination plan.
  */
  CM_ASSERT( i < ARR_SIZE(orig_plan_tbl) );
  i = MIN( i, ARR_SIZE(orig_plan_tbl) );
  sr_ptr->orig_plan_ptr = &orig_plan_tbl[i];

  CM_MSG_HIGH( "Load orig plan #%d",i,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Setup the power-up hold orig time and the max number
  ** of dials in accordance with the origination plan.
  */
  sr_ptr->hold_orig_uptime   = sr_ptr->orig_plan_ptr->pwrup_hold_orig_time;
  sr_ptr->sr_left            = sr_ptr->orig_plan_ptr->sr_max_dial;

} /* cmcall_load_orig_plan() */



/*===========================================================================

FUNCTION cmcall_orig_start

DESCRIPTION
  Kick-start the origination process.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_orig_start(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  cmph_s_type                 *ph_ptr             = cmph_ptr();
    /* Point at phone object */

  cmss_s_type                 *ss_ptr             = cmss_ptr();
    /* point at SS object */

  cm_sr_s_type                *sr_ptr             = NULL;
    /* Point silent redial component */

  dword                       uptime              = clk_uptime();
    /* Get the current uptime */

  dword                       no_srv_time;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( ss_ptr != NULL );
  CM_MSG_HIGH( "Orig Start",0,0,0 );

  #ifdef CM_DEBUG
#error code not present
  #endif  /* CM_DEBUG */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ptr       = &call_ptr->sr;

  /* Load the call origination plan
  ** and start the origination process.
  */
  cmcall_load_orig_plan( call_ptr );

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif    /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_ptr->call_state = CM_CALL_STATE_ORIG;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Inform GSDI about the call start.
  */

  #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
  #endif

  #ifdef FEATURE_MMGSDI_CDMA
  #if ((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI))
  cm_mmgsdi_in_traffic_channel_w_mode(TRUE, cmss_ptr()->info.sys_mode );
  #else /* !((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI)) */
  cm_gsdi_in_traffic_channel_w_mode(TRUE, cmss_ptr()->info.sys_mode );
  #endif /* !((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI)) */
  #endif /* FEATURE_MMGSDI_CDMA */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  CM_MSG_HIGH("MERGED cmcall_orig_start", 0, 0, 0);

  #if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)) && !defined (FEATURE_GSM) && !defined (FETAURE_WCDMA)
  CM_MSG_HIGH("CallType=%d SrvTyep=%d SO=%d",
               call_ptr->call_type,
               call_ptr->srv_type,
               call_ptr->cmcall_mode_info.info.cdma_info.srv_opt );
  #endif

  #if defined( FEATURE_HDR_HANDOFF)  && defined(FEATURE_IS2000)
#error code not present
  #endif /* FEATURE_HDR_HYBRID && FEATURE_IS2000 */

  CM_MSG_HIGH("Favord mode=%d hybr=%d",
               call_ptr->favored_mode_pref,
               call_ptr->favored_hybr_pref,
               0);

  cmlog_call_orig_start(call_ptr);

  /* For targets with System Determination 2.0 support do the following
  ** processing.
  */

  call_ptr->favored_roam_pref = cmcall_get_favored_roam_pref( call_ptr->call_type,
                                                              ss_ptr->info.sys_mode,
                                                              ss_ptr->info.roam_status,
                                                              ph_ptr->roam_pref );

  /* Get the favored prl preference to be used for origination.
  */
  call_ptr->favored_prl_pref = cmcall_get_favored_prl_pref( call_ptr->call_type );


  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
#error code not present
  #endif /* FEATURE_GPSONE_MSBASED */


  /* If the favored mode preference is not in the current mode preference,
  ** force over to the favored mode preference now.
  */
  if( !(cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref,
                                  ph_ptr->mode_pref))
      #ifdef FEATURE_HDR_HYBRID
#error code not present
      #endif
      #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) \
                && defined( FEATURE_BSR2)
      || ( cmregprx_get_info_ptr()->state == CMREGPRX_STATE_ACTIVE )
      #endif
  )
  {

    CM_MSG_HIGH("Force mode pref, call's=%d,current=%d",
                 call_ptr->favored_mode_pref,
                 ph_ptr->mode_pref,0);
    no_srv_time = cmcall_force_pref_on_the_fly
                    ( call_ptr,
                      (call_ptr->call_type == CM_CALL_TYPE_PS_DATA) ?
                                      SD_SS_PREF_REAS_ORIG_START_PS :
                                      SD_SS_PREF_REAS_ORIG_START_CS
                    );

    /* If forcing to mode preference is causing a short period of
    ** no service (during which MC is acquiring the new service),
    ** hold the origination for the appropriate amount of time.
    */
    sr_ptr->hold_orig_uptime = MAX( sr_ptr->hold_orig_uptime,
                                    uptime + no_srv_time );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is not a standard OTASP call origination, hold the origination
  ** (and the system selection preference change) for up to
  ** CMCALL_ACQ_HOLD_SS_PREF_TIME in case system determination is currently
  ** trying to acquire service.
  */
  if( call_ptr->call_type != CM_CALL_TYPE_STD_OTASP )
  {
    sr_ptr->hold_orig_uptime = MAX( sr_ptr->hold_orig_uptime,
                                    uptime + CMCALL_ACQ_HOLD_SS_PREF_TIME);

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the flag about this call being a user originated one.
  */
  call_ptr->is_user_originated_call = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Tell phone about call start.
  */
  cmph_call_start( call_ptr );

  if( call_ptr->pd_status == CMCALL_PD_STATUS_END_ORIG )
  {
    cmcall_end( call_ptr );
    return;
  }

  if( call_ptr->pd_status == CMCALL_PD_STATUS_HOLD_ORIG )
  {

    sr_ptr->hold_orig_uptime = MAX( sr_ptr->hold_orig_uptime,
                             uptime + CMCALL_PD_SESSION_ACK_HOLD_ORIG_TIME );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If a PS data call on 1X is attempted and SMS is waiting for confirmation
  ** from 1X, wait till SMS gets the ACK and then send the origination down
  */

  #if((defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) && defined(FEATURE_CDSMS))
  {
    cmsms_s_type  *sms_ptr = cmsms_ptr();
    CM_ASSERT( sms_ptr != NULL );
    if( ( sms_ptr->sms_state != CM_SMS_STATE_IDLE ) &&
        ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA ) &&
        ( call_ptr->ss == CM_SS_MAIN ) )
    {
      sr_ptr->hold_orig_uptime = MAX( sr_ptr->hold_orig_uptime,
                                      uptime + CMCALL_SMS_ACK_HOLD_ORIG_TIME );
      call_ptr->sms_status     = CMCALL_SMS_STATUS_HOLD_ORIG;
    }

  }

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Notify client list of origination event.
  */
  cmcall_event( call_ptr, CM_CALL_EVENT_ORIG );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmph_update_ddtm_status(ph_ptr);                   /* update DDTM status */

  /* Actually start the origination process.
  */
  cmcall_orig_proc( call_ptr );

} /* cmcall_orig_start() */


#ifdef FEATURE_IP_CALL

/*===========================================================================

FUNCTION cmcall_get_voip_call_id

DESCRIPTION
  Return the call id of the existing VoIP call, if present
  Else, return invalid call id

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Return the call id of the existing VoIP call, if present
  Else, return invalid call id


SIDE EFFECTS
  None

===========================================================================*/
cm_call_id_type cmcall_get_voip_call_id()
{

  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
      if ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP
           #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
           #endif
         )
      {
         return call_ptr->call_id;
      }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return CM_CALL_ID_INVALID;
} /* cmcall_get_voip_call_id */

#endif


/*===========================================================================

FUNCTION cmcall_check_if_gw_cs_call_active

DESCRIPTION
  Checks if there is a cs call in given call state. CM_CALL_STATE_NONE
  checks for call in any state.

  NOTE:- Typically there will not be a call object with call type SUPS.
  cmcall_is_gw_cs_call_type() checks SUPS call type for completeness.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if CS voice calls active
  FALSE: otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_check_if_gw_cs_call_active(

  cm_call_state_e_type call_state
    /* Call state */
)
{

  cmcall_s_type  *call_ptr;
    /* Pointer to the call object */

  cm_iterator_type call_itr;

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {

    /* check the call state and the call type */
    if( ( cmcall_is_gw_cs_call_type (call_ptr->call_type)) &&
        (( call_state           == CM_CALL_STATE_NONE) ||
         ( call_ptr->call_state == call_state ))
      )
    {
      return TRUE;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return FALSE;

} /* cmcall_check_if_gw_cs_call_active () */

/*===========================================================================

FUNCTION cmcall_check_voice_emerg_in_orig

DESCRIPTION
  Checks if current call is voice or emergency in origination

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE : if voice or emergency call is in origination
  FALSE: otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean cmcall_check_voice_emerg_in_orig(

    const cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  CM_ASSERT (call_ptr != NULL);

  if( ( (call_ptr->call_type == CM_CALL_TYPE_VOICE ) ||
        ( call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
      )
      &&

      ( call_ptr->call_state == CM_CALL_STATE_ORIG)
    )
  {
    return TRUE;
  }

  return FALSE;

} /* cmcall_check_voice_emerg_in_orig */


/*===========================================================================

FUNCTION cmcall_check_if_csdata_and_voice

DESCRIPTION
  Checks if one of the call type is CM_CALL_TYPE_VOICE and the other
  CM_CALL_TYPE_CS_DATA

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE : one is voice and the other is CS_DATA
  FALSE: otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean cmcall_check_if_csdata_and_voice(

    cm_call_type_e_type  call_type_1,
        /* call type */

    cm_call_type_e_type  call_type_2
        /* call type */
)
{

    if (((call_type_1 == CM_CALL_TYPE_VOICE) ||
                                    (call_type_1 == CM_CALL_TYPE_EMERGENCY))
        &&

        (call_type_2 == CM_CALL_TYPE_CS_DATA)
       )
    {
      return TRUE;
    }

    if (((call_type_2 == CM_CALL_TYPE_VOICE) ||
                                    (call_type_2 == CM_CALL_TYPE_EMERGENCY))
        &&
        (call_type_1 == CM_CALL_TYPE_CS_DATA)
       )
    {
      return TRUE;
    }

    return FALSE;

} /* cmcall_check_if_csdata_and_voice() */


/*===========================================================================

FUNCTION cmcall_check_if_ps_call_active

DESCRIPTION
  Checks if any PS call is active

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if PS call active
  FALSE: otherwise

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cmcall_check_if_ps_call_active( void )
{
  boolean ps_call_active = FALSE;
    /* flag to indicate whether PS call is active
       or not
    */

  cmcall_s_type  *call_ptr;
    /* Pointer to the call object */

  cm_iterator_type call_itr;

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {

    /* check the call state and the call type */

    if( ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA ) &&
        ( call_ptr->call_state != CM_CALL_STATE_IDLE ) )
    {
      ps_call_active = TRUE;
      break;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return ps_call_active;

}

/*===========================================================================

FUNCTION cmcall_get_ps_call_obj_in_orig_mode

DESCRIPTION
  Get the call object corresponding to the PS call.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Pointer to the call object if there is a PS call in origination state.
  NULL otherwise.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
cmcall_s_type *cmcall_get_ps_call_obj_in_orig_mode( void )
{
  cmcall_s_type  *call_ptr = NULL;
  boolean found=FALSE;
  cm_iterator_type call_itr;

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    /* check the call state and the call type */

    if(( call_ptr->call_type == CM_CALL_TYPE_PS_DATA ) &&
       ( call_ptr->call_state == CM_CALL_STATE_ORIG ) )
    {
         found = TRUE;
      break;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  if(found)
  {
    return call_ptr;
  }
  else
  {
    return NULL;
  }
}


/*===========================================================================

FUNCTION cmcall_process_ipcall_params

DESCRIPTION
  Remove HDR component from favored mode pref of a voice call if ss
  instance of voice call is different from that of HDR service that has
  VoIP.

  Setting force_top to TRUE for voice calls that can be possibly sent
  over IP. This takes care of multi call scenarios (MPTY, ECT on voip)
  where more than one voice call needs to be connected.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  Given call_ptr is modified.

===========================================================================*/
static void cmcall_process_ipcall_params (

  cmcall_s_type    *call_ptr
    /* pointer to a call object */
)
{

  sd_ss_mode_pref_e_type sd_mode_pref = SD_SS_MODE_PREF_NONE;
    /* Mode preference */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* checks if call type is supported on IP */
  if (!cmcall_is_call_type_supp_for_ipcall (call_ptr->call_type))
  {
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Mode preference for the current call */
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref
                                       (call_ptr->favored_mode_pref);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Forces the top if current call can be made as a IP call
  */
  if ((cmcall_instance_for_ipcall (call_ptr)) != CM_SS_NONE)
  {
    call_ptr->force_top_pref = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If there is no ipapp (on HDR )for voice origination, remove HDR
  ** component.
  **
  ** Favored mode preference for Voice calls now carry HDR mode. If there is
  ** no ipapp registered on HDR then HDR has to be removed from favored
  ** mode preference. This is done so that SD does not end up camping on
  ** HDR when voice call is pending and when there is no HDR based ipapp.
  */
  if(! (sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_HDR)  &&
        cmcall_is_active_ip_app(SYS_SYS_MODE_HDR, call_ptr->call_type)
       )
    )

  {

    call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                     call_ptr->favored_mode_pref,
                                     1,
                                     SD_SS_MODE_PREF_HDR );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return;

} /* cmcall_process_ipcall_params */

/**--------------------------------------------------------------------------
** Functions - internal: other than silent redial
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmcall_is_call_type_valid

DESCRIPTION
  Check whether a call type is valid

DEPENDENCIES
  none

RETURN VALUE
  TRUE if call type is valid, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_call_type_valid(

    cm_call_type_e_type    call_type
        /* call type */
)
{

  switch( call_type )
  {
    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_EMERGENCY:
    case CM_CALL_TYPE_CS_DATA:
    case CM_CALL_TYPE_PS_DATA:
    case CM_CALL_TYPE_SMS:
    case CM_CALL_TYPE_TEST:

    #ifdef FEATURE_OTASP
    case CM_CALL_TYPE_STD_OTASP:
    case CM_CALL_TYPE_NON_STD_OTASP:
    #endif

    #ifdef FEATURE_GPSONE
#error code not present
    #endif

    case CM_CALL_TYPE_VS:

      return TRUE;

    default:

      CM_ERR( "Unexpected call type = %d",call_type,0,0 );

  } /* switch( call_type ) */

  return FALSE;

} /* cmcall_is_call_type_valid() */


/*===========================================================================

FUNCTION cmcall_is_mobile_in_hdr_call()

DESCRIPTION
  Checks to see if any call is in HDR.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_mobile_in_hdr_call
(
  void
)
{
  boolean         ret_stat = FALSE;

  #ifdef FEATURE_HDR
#error code not present
  #endif  /* FEATURE_HDR */

  return ret_stat;

} /* cmcall_is_mobile_in_hdr_call() */



/*===========================================================================

FUNCTION cmcall_overall_call_state

DESCRIPTION
  Returns overall call state.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_overall_call_state_e_type  cmcall_overall_call_state
(
    void
)
{
    cm_call_id_type call_id;                        /* Loop index */
    cmcall_s_type *call_ptr;                        /* Call object pointer */
    cm_overall_call_state_e_type overall_call_state
        = CM_OVERALL_CALL_STATE_IDLE;/* Initialize to default return value */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    for ( call_id = 0; call_id < ARR_SIZE(call_obj_array); call_id++)
    {
        call_ptr = call_obj_array[call_id];
        if ( call_ptr == NULL )
        {
            continue;
        }

        if ( call_ptr->call_state == CM_CALL_STATE_CONV )
        {
            overall_call_state = CM_OVERALL_CALL_STATE_CONV;
            break;
        }
        else if ( call_ptr->call_state == CM_CALL_STATE_ORIG )
        {
            if ( overall_call_state == CM_OVERALL_CALL_STATE_INCOM )
            {
                overall_call_state = CM_OVERALL_CALL_STATE_ORIG_INCOM;
            }
            else if ( (overall_call_state == CM_OVERALL_CALL_STATE_IDLE)
                      ||
                      /* When call obj array has a smaller call id in
                      ** RELEASE state and higher call id in orig, the
                      ** overall state should be ORIG
                      */
                      (overall_call_state == CM_OVERALL_CALL_STATE_RELEASE)
                    )
            {
                overall_call_state = CM_OVERALL_CALL_STATE_ORIG;
            }
        }
        else if ( call_ptr->call_state == CM_CALL_STATE_INCOM )
        {
            if ( overall_call_state == CM_OVERALL_CALL_STATE_ORIG )
            {
                overall_call_state = CM_OVERALL_CALL_STATE_ORIG_INCOM;
            }
            else if ( overall_call_state == CM_OVERALL_CALL_STATE_IDLE )
            {
                overall_call_state = CM_OVERALL_CALL_STATE_INCOM;
            }
        }
        else if ( call_ptr->call_state == CM_CALL_STATE_RECALL_RSP_PEND)
        {
           if ( overall_call_state == CM_OVERALL_CALL_STATE_IDLE )
           {
             overall_call_state = CM_OVERALL_CALL_STATE_RECALL_RSP_PEND;
           }
        }
        else if (call_ptr->call_state == CM_CALL_STATE_IDLE)
        {
          if(overall_call_state > CM_OVERALL_CALL_STATE_IDLE)
          {
              continue;
          }
          else if(call_ptr->call_subst.idle == CM_CALL_IDLE_SUBST_ENDWAIT)
          {
              overall_call_state = CM_OVERALL_CALL_STATE_RELEASE;
          }
        }
        /* ( call_ptr->call_state == CM_CALL_STATE_CONV ) */
    } /* for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++) */

    return overall_call_state;

} /* cmcall_overall_call_state() */


/*===========================================================================

FUNCTION cmcall_set_call_object

DESCRIPTION

  Sets the call type.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
void cmcall_set_type
(
  cmcall_s_type             *call_ptr,
  cm_call_type_e_type     call_type
)
{
  CM_ASSERT(call_ptr != NULL);

  call_ptr->call_type  = call_type;

} /* cmcall_set_type() */


/*===========================================================================

FUNCTION cmcall_print_all_calls

DESCRIPTION
  Prints call type and call state of all calls in CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_print_all_calls (void)
{

  cmcall_s_type   *call_ptr;
  cm_iterator_type call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH ("Printing all calls in CM ...", 0, 0, 0);

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {

    CM_MSG_HIGH   (" Call %d of type %d is in state %d",
                     call_ptr->call_id, call_ptr->call_type,
                     call_ptr->call_state );

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

} /* cmcall_print_all_calls () */


/*===========================================================================

FUNCTION cmcall_update_call_cmd_end

DESCRIPTION
  Update call cmd end with latest call information from call object

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Modifies parameters in end cmd at certain conditions

===========================================================================*/
static void cmcall_update_call_cmd_end
(
   cm_cmd_s_type *cmd_ptr
     /* client cmd */
)
{

  cm_call_cmd_s_type              *call_cmd_ptr  = CALL_CMD_PTR( cmd_ptr );
    /* Point at call command component */

  int                              end_call_loop = 0;
    /* Loops around calls that have to be ended
    */

  cm_end_params_s_type             *call_end_params_ptr = NULL;
    /* Points to end cmd parameters of a particular call
    */

  cm_call_mode_info_e_type          cm_call_info_type    = CM_CALL_MODE_INFO_NONE;
    /* info type as carried by CM
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (call_cmd_ptr->info.num_end_call_ids > CM_CALL_ID_MAX)
  {
    CM_ERR (" Number of calls to be ended %d exceeds CM_CALL_ID_MAX",
              call_cmd_ptr->info.num_end_call_ids, 0, 0);

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop through calls that have been listed for an end
  */
  for (end_call_loop=0; end_call_loop < call_cmd_ptr->info.num_end_call_ids;
       ++end_call_loop)
  {

    call_end_params_ptr = &call_cmd_ptr->info.end_params[end_call_loop];

    /* end cmd array can have call ids belonging to different info_type's.
    ** For ex - CDMA call id and WLAN call id can be ended with same end cmd
    */

    /* info_type of the call from CM
    */
    cm_call_info_type = cmcall_call_id_mode (call_end_params_ptr->call_id);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if (cm_call_info_type == call_end_params_ptr->info_type)
    {
      /* No processing required if info_type matches */
      continue;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* - If call in CM has a non NONE info_type populate default end parameters
    **   in call end command
    ** - info_type given by client != CM's info_type
    */
    if (cm_call_info_type != CM_CALL_MODE_INFO_NONE)
    {

      CM_ERR ("info_type mismatch on call %d, client %d, CM %d",
               call_end_params_ptr->call_id,
               call_end_params_ptr->info_type,
               cm_call_info_type);

      /* Populate default end parameters depending on info type
      */
      switch (cm_call_info_type)
      {

        #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
        #endif /* #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA) */


        #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
        case CM_CALL_MODE_INFO_CDMA:

          call_end_params_ptr->end_params.cdma_end.end_reason_included = TRUE;
          call_end_params_ptr->end_params.cdma_end.end_reason
                                            = (uint8) CAI_REL_NORMAL;
          break;

        #endif /* #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

        case CM_CALL_MODE_INFO_IP:

          #if defined(FEATURE_IP_CALL)
          call_end_params_ptr->end_params.ip_cause.cause =
                                           (uint32) CMIPAPP_END_CAUSE_NORMAL;
          #endif

          break;

        case CM_CALL_MODE_INFO_NONE:
        default:
          CM_ERR ("info_type %d invalid for call %d",
             cm_call_info_type, call_cmd_ptr->info.end_params[0].call_id, 0);

          break;
      }

    }
    /* If call in CM has a NONE as info_type clear the end params for the call
    ** id
    */
    else
    {

      memset ( &call_end_params_ptr->end_params, 0,
                                  sizeof(call_end_params_ptr->end_params));


    }

    /* info_type given by client is overwritten to overcome mismatch
    ** between client supplied info_type and the one maintained by CM
    */
    call_end_params_ptr->info_type = cm_call_info_type;


  } /* for loop */


} /* cmcall_update_call_cmd_end () */


/*===========================================================================

FUNCTION cmcall_send_mode_spec_end

DESCRIPTION
  End calls with mode_info INFO_NONE by calling cmcall_end. GW, 1x and ipcall
  specific functions filter out call id of interest and process call end.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Modifies parameters in end cmd.

===========================================================================*/
static void cmcall_send_mode_spec_end
(
   cm_cmd_s_type *cmd_ptr
     /* client cmd */
)

{

  cm_call_cmd_s_type              *call_cmd_ptr  = CALL_CMD_PTR( cmd_ptr );
    /* Point at call command component */

  int                              end_call_loop = 0;
    /* Loops around calls that have to be ended
    */

  cmcall_s_type                   *call_ptr      = NULL;
   /* call_ptr for call objects
   */

  cm_call_cmd_err_e_type           call_cmd_err  = CM_CALL_CMD_ERR_NOERR;
    /* Call cmd error */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT (cmd_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop through calls that have been listed for an end
  */
  for (end_call_loop=0; end_call_loop < call_cmd_ptr->info.num_end_call_ids;
       ++end_call_loop)
  {

    CM_MSG_HIGH (" End cmd for call %d info_type = %d",
                     call_cmd_ptr->info.end_params[end_call_loop].call_id,
                     call_cmd_ptr->info.end_params[end_call_loop].info_type,
                     0);


    call_ptr =
       cmcall_ptr (call_cmd_ptr->info.end_params[end_call_loop].call_id);

    #ifdef FEATURE_IMS_VCC
    /** END the switch in call as well */
    homer_end_switch_in_call(call_ptr);
    #endif

    /* End calls of info_type NONE by directly calling cmcall_end().
    */
    if (call_cmd_ptr->info.end_params[end_call_loop].info_type !=
                                                   CM_CALL_MODE_INFO_NONE)
    {
      continue;
    }

    if (call_ptr != NULL)
    {
      call_ptr->end_status = CM_CALL_END_CLIENT_END;
      cmcall_end( call_ptr );
    }


  } /* for () */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Cmd check procedure
  ** - If any access technology returns error, cmd cb is immed called
  **   and control returns.
  **
  ** - If there is no error in any access tech then cmd cb is called
  **   with cb func ptr forced to be NULL
  */


  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif

  /* Checks CDMA call cmd errors */
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  call_cmd_err = cmxcall_client_cmd_check( cmd_ptr );

  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    cmcall_client_cmd_err( cmd_ptr, call_cmd_err );
    return;
  }
  #endif

  /* Checks IP call cmd errors */
  #if defined(FEATURE_IP_CALL)
  call_cmd_err = cmipcall_client_cmd_check( cmd_ptr );

  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    cmcall_client_cmd_err( cmd_ptr, call_cmd_err );
    return;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* No cmd error is present if code reaches this point */
  cmcall_client_cmd_err( cmd_ptr, call_cmd_err );

  /* Reset the cmd cb func ptr so that further calls to cmcall_client_cmd_err
  ** does not call the cmd cb again
  */
  cmd_ptr->cmd.call.cmd_cb_func = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extracts calls of info_type GW_CS, GW_PS to end them */
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif

  /* Extracts calls of info_type CDMA to end */
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  cmxcall_client_cmd_proc(cmd_ptr);
  #endif

  #if defined(FEATURE_IP_CALL)
  /* Extracts calls of info_type IP to end */
  cmipcall_client_cmd_proc(cmd_ptr);
  #endif



} /* cmcall_send_mode_spec_end () */


/*===========================================================================

FUNCTION cmcall_is_gw_cs_call_type

DESCRIPTION
  Checks if the call type is used on cs domain

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - Call type used on gw cs domain
  FALSE - Call type not being used on gw cs domain

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_gw_cs_call_type
(
  cm_call_type_e_type call_type
)
{

  if ( (call_type == CM_CALL_TYPE_VOICE) ||

       (call_type == CM_CALL_TYPE_EMERGENCY) ||

       (call_type == CM_CALL_TYPE_SUPS) ||

       (call_type == CM_CALL_TYPE_CS_DATA) ||

       (call_type == CM_CALL_TYPE_VT) ||

       (call_type == CM_CALL_TYPE_VT_LOOPBACK)
     )
  {

    return TRUE;
  }

  return FALSE;

} /* cmcall_is_gw_cs_call_type () */

/*===========================================================================

FUNCTION cmcall_fill_ps_cc_params

DESCRIPTION
  Fills parameters required for performing call control on ps data calls

DEPENDENCIES
  nsapi and connection id for the call should be present in call obj

RETURN VALUE
  None

SIDE EFFECTS
  nsapi and trans_id fields in cmd_ptr are populated.

===========================================================================*/
/*lint -esym(818, cmd_ptr)*/
/*lint -esym(818, ps_cc_params)*/
static void cmcall_fill_ps_cc_params
(
   cm_cmd_s_type                *cmd_ptr,
     /* client cmd */

   cm_gw_ps_cc_params_s_type    *ps_cc_params
     /* Carries PDP activ in L3 format */
)
{
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #else

  SYS_ARG_NOT_USED (cmd_ptr);
  SYS_ARG_NOT_USED (ps_cc_params);

  #endif  /* #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

} /* cmcall_fill_ps_cc_params */
/*lint +esym(818, cmd_ptr)*/
/*lint +esym(818, ps_cc_params)*/


/*===========================================================================

FUNCTION cmcall_get_sec_pdp_callid

DESCRIPTION
  Gives the call id of a secondary PDP .

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  call id of secondary pdp call.

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_get_sec_pdp_callid (void)
{

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

  return CM_CALL_ID_INVALID;

} /* cmcall_get_sec_pdp_callid () */



/**--------------------------------------------------------------------------
** Functions - external: call state, call object, & call id
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION  cmcall_get_call_obj_in_call_state

DESCRIPTION
  Returns the call obj in call_state.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to the call obj in call_state if present
  NULL otherwise

SIDE EFFECTS
  none

===========================================================================*/

cmcall_s_type* cmcall_get_call_obj_in_call_state (

                  cm_call_state_e_type   call_state
                  /* Given call state */
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
    {
      if (call_ptr->call_state == call_state)
      {
        return call_ptr;
      }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while  */

  return NULL;
} /* cmcall_get_call_obj_in_call_state () */

#if (defined FEATURE_OTASP && defined FEATURE_OTASP_STATUS)
/*===========================================================================

FUNCTION  cmcall_get_otasp_call_obj

DESCRIPTION
  Returns the OTASP/OTAPA call obj. There can be only one call obj of this type

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to the call obj of type OTASP/OTAPA if present
  NULL otherwise

SIDE EFFECTS
  none

===========================================================================*/

cmcall_s_type* cmcall_get_otasp_call_obj()
{
  cmcall_s_type     *call_ptr = NULL;   
      /* Call object pointer  */
  cm_iterator_type  call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if (CM_CALL_TYPE_IS_OTASP(call_ptr->call_type) ||
        CM_CALL_TYPE_IS_OTAPA(call_ptr->call_type)) 
    {
      return call_ptr;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while  */
  return NULL;
} /* cmcall_get_otasp_call_obj() */
#endif // FEATURE_OTASP_STATUS
/*===========================================================================

FUNCTION cmcall_call_id_get

DESCRIPTION
  Get the call id corresponding to a call object pointer.

DEPENDENCIES
  None

RETURN VALUE
  call_id

SIDE EFFECTS
  None

===========================================================================*/
cm_call_id_type cmcall_call_id_get( const cmcall_s_type *call_ptr )
{
  if(call_ptr != NULL)
  {
    return call_ptr->call_id;
  }
  else
    {
    return CM_CALL_ID_INVALID;
  }
} /* cmcall_call_id_get() */

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */



/*===========================================================================
FUNCTION CMCALL_CALL_IDS_ALLOCATED

DESCRIPTION
  This function returns the number of call IDs allocated.

DEPENDENCIES
  Call object must be initialized

RETURNS
  none

SIDE EFFECTS
  none

===========================================================================*/
byte cmcall_call_ids_allocated
(
  void
)
{

  byte              num_allocated_call_ids = 0;
  cmcall_s_type    *call_ptr = NULL;
  cm_iterator_type  call_itr;

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);


  while(call_ptr != NULL)
  {
    if(call_ptr->call_id != CM_CALL_ID_INVALID)
    {
      num_allocated_call_ids++;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }
  return num_allocated_call_ids;

} /* cmcall_call_ids_allocated() */



/*===========================================================================

FUNCTION cm_number_of_allocated_calls

DESCRIPTION

  Returns number of allocated calls.

DEPENDENCIES


RETURN VALUE
  Number of allocated calls

SIDE EFFECTS
  none

===========================================================================*/
int cm_number_of_allocated_calls( void )
{
  return cmcall_call_ids_allocated();

} /* cm_number_of_allocated_calls() */



/*===========================================================================

FUNCTION cm_call_id_is_allocated

DESCRIPTION

  Returns TRUE if given call id is allocated and FALSE otherwise.

DEPENDENCIES
  none

RETURN VALUE
  Success/Failure

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_call_id_is_allocated( cm_call_id_type call_id )
{

  cmcall_s_type *call_ptr = NULL;

  call_ptr = cmcall_ptr(call_id);

  if(call_ptr != NULL)
  {
    return TRUE;
  }

  return FALSE;

} /* cm_call_id_is_allocated() */


#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif /* defined (FEATURE_WCDMA) || defined (FEATURE_GSM) */






/*===========================================================================

FUNCTION cm_call_id_allocate

DESCRIPTION

  Allocate a call Id and get a call object.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
cm_call_id_type cm_call_id_allocate
(
  void
)
{
  cmcall_s_type            *call_ptr;
  unsigned int             i,j;
  cm_call_id_type          call_id=CM_CALL_ID_INVALID;

  static dword             call_id_failure_uptime = CMCALL_TIMER_INACTIVE;
                                                  /* Time since last failure */

  /* Initialized to "arr_size() - 1", so that first call id allocation
  ** starts with 0. Some AST scripts are dependent on this.
  */
  static cm_call_id_type last_call_id = ARR_SIZE(call_obj_array) - 1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Dynamically allocate a call object
  */
  if( (call_ptr = (cmcall_s_type *)cm_mem_malloc(
                                          sizeof(cmcall_s_type))) == NULL )
  {
    CM_ERR_FATAL("Could not allocate memory for Call object!", 0, 0, 0);
    return call_id;            /*lint !e527 */
  }

  cmcall_reset(call_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Search through call object array for an unallocated call object
  ** starting from last call_id allocated.
  */
  rex_enter_crit_sect(cmtask_crit_sect_ptr());
  i= last_call_id;
  for ( j = 0; j < ARR_SIZE(call_obj_array); j++ )
  {
    i = (i+1)%ARR_SIZE(call_obj_array);
    if (call_obj_array[i] == NULL )
    {
      call_id = (cm_call_id_type)i;
      last_call_id = call_id;
      call_ptr->call_id = call_id;
      call_obj_array[i] = call_ptr;
      break;
    }

  }
  rex_leave_crit_sect(cmtask_crit_sect_ptr());

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we're out of call ids, and this is the first time we are out of call ids
  ** since a successful allocation, let's wait some time before error fataling.
  ** If time is up, we should error fatal, otherwise return the allocated call id.
  */
  if(!cmcall_is_call_id_in_range(call_id))
  {
    call_id = CM_CALL_ID_INVALID;
    cm_mem_free(call_ptr);
    if( call_id_failure_uptime < clk_uptime ())
    {
      /* CM still does not have a free call id to allocate. Error fatal for
      ** this condition has been removed. This function returns
      ** CM_CALL_ID_INVALID for this case.
      */

      CM_ERR ( "CM is out of call ids -- Fatal Error!", 0, 0, 0 );

      cmcall_print_all_calls ();
    }
    else if( call_id_failure_uptime == CMCALL_TIMER_INACTIVE )
    {
      /* First failure after a successful allocation.  Set the timer.
      */
      MSG_ERROR( "CM is out of call ids -- Holding on for 5 ...", 0, 0, 0 );
      call_id_failure_uptime = clk_uptime() + CMCALL_CALL_ID_HOLD_FATAL_ERR_TIME;
    }
    else
    {
      /* Failure again but we are still holding on, since the timer is not up yet...
      */
      MSG_ERROR( "CM is out of call ids again -- Still holding on...", 0, 0, 0 );
    }
  }
  else
  {
    /* Reset the timer to show a successful allocation.
    */
    call_id_failure_uptime  = CMCALL_TIMER_INACTIVE;

    CM_MSG_HIGH( "Object[0x%x] allocated for id[%d]", call_ptr, call_id, 0 );
  }

  return call_id;

} /* cm_call_id_allocate() */


/*===========================================================================

FUNCTION cm_call_id_deallocate

DESCRIPTION
  Deallocates a call ID

DEPENDENCIES
  none

RETURN VALUE
  Success/Failure

SIDE EFFECTS
  none

===========================================================================*/
void cm_call_id_deallocate
(
    cm_call_id_type call_id
        /* Call id */
)
{
  cmcall_s_type            *call_ptr;

  call_ptr = cmcall_ptr(call_id);

  if(call_ptr == NULL)
  {
    CM_ERR("Null Call Ptr for Call ID %d, Deallocate Failed!", call_id, 0, 0 );
    return;
  }

  call_obj_array[call_id] = NULL;
  cm_mem_free(call_ptr);

  CM_MSG_HIGH( "Call id (%d) deallocated", call_id, 0, 0 );

} /* cm_call_id_deallocate() */


/**--------------------------------------------------------------------------
** Functions - external:commands from other CM objects (ph,ss,etc.)
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmcall_call_id_mode

DESCRIPTION

  Returns call mode if given call id is allocated.

DEPENDENCIES
  none

RETURN VALUE
  cm_call_mode_info_e_type

SIDE EFFECTS
  None

===========================================================================*/
cm_call_mode_info_e_type cmcall_call_id_mode
(
  cm_call_id_type call_id
)
{
  cmcall_s_type   *call_ptr = cmcall_ptr( call_id );

  /*-----------------------------------------------------------------------*/

  if ( call_ptr == NULL )
  {
    return CM_CALL_MODE_INFO_NONE;
  }
  else
  {
    return( call_ptr->cmcall_mode_info.info_type );
  }

} /* cmcall_call_id_mode() */



/*===========================================================================

FUNCTION cmcall_overall_call_end

DESCRIPTION
  Ends all calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_overall_call_end
(
  void
)
{

  cmcall_s_type *call_ptr;                        /* Call object pointer */
  cm_iterator_type  call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
    {
      if ( call_ptr->call_state != CM_CALL_STATE_IDLE )
      {
        call_ptr->end_status = CM_CALL_END_OFFLINE;
        cmcall_end( call_ptr );
      } /* if ( call_ptr->call_state != CM_CALL_STATE_IDLE ) */

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

} /* cmcall_overall_call_end() */

/*===========================================================================

FUNCTION cmcall_end_each_call_with_mode

DESCRIPTION
  Ends calls based on mode info.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_end_each_call_with_mode
(

  cm_call_mode_info_e_type call_mode_info,
  /* Given call mode info */

  cm_call_end_e_type call_end_reason
  /* Given reason for ending the call */
)
{

  cmcall_s_type *call_ptr;                        /* Call object pointer */
  cm_iterator_type  call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
    {
      if ( call_ptr->call_state != CM_CALL_STATE_IDLE &&
           call_ptr->cmcall_mode_info.info_type == call_mode_info )
      {
        call_ptr->end_status = call_end_reason;
        cmcall_end( call_ptr );
      }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

} /* cmcall_end_each_call_with_mode() */

/*===========================================================================

FUNCTION cmcall_end_each_call_with_type

DESCRIPTION
  Ends calls based on call type

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_end_each_call_with_type
(

  cm_call_type_e_type call_type,
  /* Given call mode info */

  cm_call_end_e_type call_end_reason
  /* Given reason for ending the call */
)
{

  /* Call object pointer */
  cmcall_s_type     *call_ptr;
  
  cm_iterator_type  call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_type == call_type )
    {
      call_ptr->end_status = call_end_reason;
      cmcall_end( call_ptr );
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

} /* cmcall_end_each_call_with_type() */

/**--------------------------------------------------------------------------
** Functions - external:commands from bottom (cm?call)
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmcall_ptr

DESCRIPTION
  Return a pointer to call object with given call id or NULL
  if such an object does not exist if there is a call Id mismatch.


DEPENDENCIES
  CM must be initialized

RETURN VALUE
  Pointer to requested call object or NULL if not found

SIDE EFFECTS
  None

===========================================================================*/
cmcall_s_type  *cmcall_ptr( cm_call_id_type call_id )
{

  cmcall_s_type   *call_ptr = NULL;
  cm_iterator_type call_itr;
  boolean          found = FALSE;


  if(!cmcall_is_call_id_in_range(call_id))
  {
    CM_ERR("cmcall_ptr Failed. Call Id out of range - %d",call_id,0,0);
    return NULL;
  }

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if(call_ptr->call_id == call_id)
    {
      found = TRUE;
      break;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  if(found)
  {
    return call_ptr;
  }
  else
  {
    return NULL;
  }
} /* cmcall_ptr() */


/*===========================================================================

FUNCTION cmcall_info_alloc

DESCRIPTION
  Allocate a call info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated call info struct.

SIDE EFFECTS
  none

===========================================================================*/
cm_mm_call_info_s_type  *cmcall_info_alloc( void )
{
  static cm_mm_call_info_s_type  call_info;
    /* Currently there is only a need for one call info buffer */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* First initialize the entire buffer to 0
  */
  memset( &call_info,
           CM_CALL_EVENT_DEFAULT_FIELD_VALUE,
           sizeof(call_info) );

  /* Initialize the fields to default values
  */
  call_info.call_client_id              = CM_CLIENT_ID_ANONYMOUS;
  call_info.call_id                     = CM_CALL_ID_UNASSIGNED;
  call_info.call_state                  = CM_CALL_STATE_NONE;
  call_info.call_type                   = CM_CALL_TYPE_NONE;

  cm_num_init(&call_info.num);
  #ifdef FEATURE_IP_CALL
  cm_num_init(&call_info.waiting_num);
  #endif

  call_info.alpha.len                   = CM_ALPHA_LEN_NONE;
  memset(call_info.alpha.buf,0,sizeof( call_info.alpha.buf) );

  call_info.enc_alpha.num_bytes         = CM_ALPHA_LEN_NONE;
  call_info.enc_alpha.coding_scheme     = CM_ALPHA_CODING_SCHEME_NONE;
  memset(call_info.enc_alpha.buf,0,sizeof( call_info.enc_alpha.buf) );

  cm_num_init(&call_info.redirecting_number.num);

  call_info.signal.is_signal_info_avail = FALSE;
  call_info.signal.signal_type          = 0;
  call_info.signal.alert_pitch          = 0;
  call_info.signal.signal               = 0;


  call_info.end_status                  = CM_CALL_END_NONE;
  call_info.num_call_ids_ended          = 0;

  memset(&call_info.end_params, 0,  sizeof( cm_end_params_s_type) );
  call_info.end_params.call_id          = CM_CALL_ID_UNASSIGNED;
  call_info.end_params.info_type        = CM_CALL_MODE_INFO_NONE;

  call_info.sys_mode                    = SYS_SYS_MODE_NONE;
  call_info.call_connect_time           = 0;
  call_info.call_end_time               = 0;
  call_info.bytes_received              = 0;
  call_info.bytes_transmitted           = 0;

  memset(&call_info.mode_info, 0, sizeof( call_info.mode_info) );
  call_info.mode_info.info_type         = CM_CALL_MODE_INFO_NONE;


  call_info.line                        = CM_ALS_LINE_NONE;
  call_info.requesting_client_id        = CM_CLIENT_ID_ANONYMOUS;

  #if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
       defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM))
#error code not present
  #endif

  call_info.direction                   = CM_CALL_DIRECTION_NONE;
  call_info.modify                      = CM_CALL_MODIFY_NONE;

  call_info.mode_info.info.cdma_call.line_ctrl_polarity_included
                                        = FALSE;
  call_info.mode_info.info.cdma_call.line_ctrl_toggle
                                        = FALSE;
  call_info.mode_info.info.cdma_call.line_ctrl_reverse
                                        = FALSE;
  call_info.mode_info.info.cdma_call.line_ctrl_power_denial
                                        = 0;
  call_info.mode_info.info.cdma_call.orig_fail_layer
                                        = CM_CALL_ORIG_ERR_LAYER_NONE;

  /* Simply return a pointer to the statically allocated
  ** call info buffer.
  */
  return &call_info;

} /* cmcall_info_alloc() */



/*===========================================================================

FUNCTION cmcall_update_call_end_info

DESCRIPTION
  This function updates the call object information related to the
  CM_CALL_EVENT_END_REQ and CM_CALL_EVENT_END call events.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_update_call_end_info(

  cmcall_s_type           *call_ptr
      /* pointer to a call object. */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** In GSM/WCDMA, it is possible that the CM_CALL_EVENT_END_REQ and
  ** the CM_CALL_EVENT_END events will be sent during the end of a
  ** call.  If the call end time is not zero, then this implies that
  ** the CM_CALL_EVENT_END_REQ call event was previously sent.  If so,
  ** then is is not necessary to update the call object information.
  */
  if ( call_ptr->call_end_time == 0 )
  {
    call_ptr->bytes_received    = 0;
    call_ptr->bytes_transmitted = 0;
    call_ptr->call_end_time     = clk_uptime();

    if ( ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA ) ||
         ( call_ptr->call_type == CM_CALL_TYPE_CS_DATA )    )
    {
       if ( cmcall_data_session_total_byte_ctrs_cb_func_ptr != NULL )
       {
         cm_call_id_type        call_id = cmcall_call_id_get( call_ptr );

         if ( call_id != CM_CALL_ID_INVALID )
         {
             if (!(cmcall_data_session_total_byte_ctrs_cb_func_ptr
             ( call_id,
               &call_ptr->bytes_received,
               &call_ptr->bytes_transmitted
             )))
             {
               MSG_ERROR ("cmcall_data_session_total_byte_ctrs_cb_func failed", 0, 0, 0);
             }

         }
         else
         {
           CM_ERR( "Invalid call ID.  Total byte ctrs not calculated.",0,0,0 );
         }
       }  /* if (func_ptr != NULL */
    } /* if call_ptr->call_type == PS or CS data */
  } /* if ( call_ptr->call_end_time != 0 */

} /* cmcall_update_call_end_info */

/*===========================================================================

FUNCTION cmcall_assert

DESCRIPTION
  Check a series of assertions over the call object to make sure that
  none of its fields are corrupted and to validate its state.

  Check fields that are common across modes of operation.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_assert(

    const cmcall_s_type    *call_ptr
        /* pointer to a call object */
)
/*lint -esym(715, call_ptr)
** NOTE -save/-restore do not work with -esym()
*/
{

  #ifdef CM_DEBUG
#error code not present
  #endif /* CM_DEBUG */

} /* cmcall_assert() */
/*lint +esym(715, call_ptr)
** NOTE -save/-restore do not work with -esym()
*/

/*===========================================================================

FUNCTION cmcall_copy_cc_result

DESCRIPTION
  Copy the call caontrol result into
  call_ptr


DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_copy_cc_result
(
  const cmcall_s_type          *call_ptr,
      /* Point at call object */

  cm_mm_call_info_s_type       *call_info_ptr
      /* Copy call state info into this buffer */

)

{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_info_ptr != NULL );
  CM_ASSERT( call_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( call_ptr->cmcall_mode_info.info_type )
  {
    #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
    #endif

    case CM_CALL_MODE_INFO_CDMA:
      call_info_ptr->result_from_cc.alpha = call_ptr->cdma_cc_modified_info.alpha;
      call_info_ptr->result_from_cc.num = call_ptr->cdma_cc_modified_info.num;
      call_info_ptr->result_from_cc.call_control_result =
        call_ptr->cdma_cc_modified_info.call_control_result;
      break;

    default:
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
}

/*===========================================================================

FUNCTION cmcall_info_get

DESCRIPTION
  Copy the current call state information into a specified buffer.
  Copy fields that are common accross modes of operation.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_info_get(

    const cmcall_s_type          *call_ptr,
        /* Point at call object */

    cm_mm_call_info_s_type       *call_info_ptr
        /* Copy call state info into this buffer */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_info_ptr != NULL );
  CM_ASSERT( call_ptr != NULL );

  /* Verify that object was properly initialized.
  */
  CM_ASSERT( CM_INIT_CHECK(call_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify the integrity of call object's common fields.
  */
  cmcall_assert( call_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy common call parameters into call_info struct.
  */
  call_info_ptr->call_client_id         = cmclient_get_client_id( call_ptr->call_client_ptr );
  call_info_ptr->call_id                = cmcall_call_id_get( call_ptr );
  call_info_ptr->call_state             = call_ptr->call_state;
  call_info_ptr->call_type              = call_ptr->call_type;
  call_info_ptr->num                    = call_ptr->num;
  
  #ifdef FEATURE_IP_CALL
  call_info_ptr->waiting_num            = call_ptr->waiting_num;
  #endif

  call_info_ptr->redirecting_number     = call_ptr->redirecting_number;
  call_info_ptr->alpha                  = call_ptr->alpha;

  call_info_ptr->enc_alpha              = call_ptr->enc_alpha;

  call_info_ptr->signal                 = call_ptr->signal;
  call_info_ptr->end_status             = call_ptr->end_status;
  call_info_ptr->num_call_ids_ended     = 1;
  call_info_ptr->sys_mode               = call_ptr->sys_mode;
  call_info_ptr->call_connect_time      = call_ptr->call_connect_time;
  call_info_ptr->call_end_time          = call_ptr->call_end_time;
  call_info_ptr->bytes_received         = call_ptr->bytes_received;
  call_info_ptr->bytes_transmitted      = call_ptr->bytes_transmitted;
  call_info_ptr->line                   = call_ptr->line;
  call_info_ptr->direction              = call_ptr->direction;
  call_info_ptr->mode_info.info_type    = call_ptr->cmcall_mode_info.info_type;
  call_info_ptr->end_params.call_id     = call_ptr->end_params.call_id;
  call_info_ptr->end_params.info_type   = call_ptr->end_params.info_type;
  call_info_ptr->end_params             = call_ptr->end_params;

  cmcall_copy_cc_result(call_ptr,call_info_ptr);


  switch ( call_ptr->cmcall_mode_info.info_type )
  {
    case CM_CALL_MODE_INFO_NONE:
      break;

    #if defined (FEATURE_IP_CALL)
    case CM_CALL_MODE_INFO_IP:
      /* copying info specific to ip call
      */
      call_info_ptr->mode_info.info.ip_call.call_prog_info =
         call_ptr->cmcall_mode_info.info.ip_info.call_prog_info;
      break;
    #endif /* FEATURE_IP_CALL */

    #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
    #endif

    #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    case CM_CALL_MODE_INFO_CDMA:
      (void) cmxcall_info_get( call_ptr, call_info_ptr );
      break;
    #endif

    default:
      CM_ERR_FATAL( "Bad mode = %d",
                     call_ptr->cmcall_mode_info.info_type, 0, 0 );
      /*
      ** 527: Unreachable
      ** This is because CM_ERR_FATAL() evaluates to error fatal
      ** (i.e exit program)
      ** But for different feature set it may not to, so we want to
      ** supress the warning here
      */
      /*lint -save -e527 */
      return FALSE;
      /*lint -restore */
  } /* switch ( call_ptr->cmcall_mode_info.info_type ) */

  /* Return boolean indicating whether call information is valid.
  */
  return BOOLEAN( CM_INIT_CHECK(call_ptr->init_mask) );

} /* cmcall_info_get() */


/*===========================================================================

FUNCTION cmcall_event

DESCRIPTION
  Notify the client list of a specified call event.
  This function is for events that apply to a particular call
  (call with a specific call id)

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_event(

    cmcall_s_type    *call_ptr,
        /* pointer to a call object. */

    cm_call_event_e_type    call_event
        /* notify client list of this call event */
)
/*lint -esym(818, call_ptr) could be declared as pointing to const
** Nope, not when GW flags are defined as then we set fields in call_ptr
** through wcall_mode_ptr and info_mode_ptr
*/
{
  cm_mm_call_info_s_type     *call_info_ptr = NULL;

  #ifdef FEATURE_IMS_VCC
  boolean                     notify_client = TRUE;
  #endif

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( BETWEEN(call_event, CM_CALL_EVENT_NONE, CM_CALL_EVENT_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_info_ptr = cmcall_info_alloc();
  CM_ASSERT( call_info_ptr != NULL );

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy call information from call object into call info structure
  */
  (void) cmcall_info_get(call_ptr, call_info_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For backwards compatibility issues....                                */
  if ((call_info_ptr->end_status == CM_CALL_END_NO_GW_SRV) ||
      (call_info_ptr->end_status == CM_CALL_END_NO_CDMA_SRV))
  {
    call_info_ptr->end_status = CM_CALL_END_NO_SRV;
  }

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_DCVS_LPM
  if( call_event == CM_CALL_EVENT_CONNECT && call_ptr->call_type ==  CM_CALL_TYPE_VOICE )
  {
    clk_regime_register_for_cpu_resource( CLKRGM_CDMA_VOICE_CALL );
  }

  if(call_event == CM_CALL_EVENT_END && call_ptr->call_type ==  CM_CALL_TYPE_VOICE )
  {
    clk_regime_deregister_for_cpu_resource( CLKRGM_CDMA_VOICE_CALL );
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Notify clients, list of call event.
  */
  #ifdef FEATURE_IMS_VCC

  /** Process IMS Homer related events.
  ** And check if the event should be delivered to all the clients or not.
  */
  notify_client = homer_umts_wlan_handover_proc(call_ptr, call_event);

  /**
  *  If the event is not to be given to all clients, then for certain
  *  handoff call events map the call event to another call event
  *  and then give to all clients. This is done to ensure that no
  *  special handling of call events is required for IMS Homer.
  */
  if( !notify_client )
  {
      switch( call_event )
      {

        case CM_CALL_EVENT_ORIG:
          call_event = CM_CALL_EVENT_HANDOFF_CALL_ORIG;
          notify_client = TRUE;
          break;

        case CM_CALL_EVENT_CONNECT:
          call_event = CM_CALL_EVENT_HANDOFF_CALL_CONNECT;
          notify_client = TRUE;
          break;

        case CM_CALL_EVENT_END:
          call_event = CM_CALL_EVENT_HANDOFF_CALL_END;
          if( homer_chk_orig_abort_in_switch_in_domain(call_ptr) )
          {
             homer_update_handoff_info(CM_CALL_ID_INVALID, CM_CALL_ID_INVALID);
          }
          notify_client = TRUE;
          break;

      default:
          break;
      }
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_IMS_VCC
  if( notify_client )
  #endif
  {
    cmclient_list_call_event_ntfy( call_event, call_info_ptr );
  }

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif
} /* cmcall_event() */
/*lint +esym(818, call_ptr) could be declared as pointing to const */

/*===========================================================================
FUNCTION cmcall_unreserve_connection_id

DESCRIPTION
   Unreserve SM/MN connection ids

DEPENDENCIES
  Call object must have already been initialized with cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(715, call_ptr) */
static void cmcall_unreserve_connection_id(

    const cmcall_s_type                  *call_ptr
        /* pointer to a call object */
)
{
  /*lint -esym(550,nsapi)*/

  #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
  #endif
}  /* cmcall_unreserve_connection_id */
/*lint +esym(715, call_ptr) */
/*lint +esym(550,nsapi) */


/*===========================================================================

FUNCTION cmcall_end

DESCRIPTION
  Does call end necessary activities.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_end(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  #ifdef FEATURE_GPSONE
#error code not present
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if (defined FEATURE_GSM) || (defined FEATURE_WCDMA)
#error code not present
  #endif  /* FEATURE_GSM || FEATURE_WCDMA */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef FEATURE_IMS_VCC
  /** Copy the relevant call information from switch out call
   *  to switch in call obj before switch out call obj is
   *  deallocated */
  homer_copy_call_info(call_ptr);
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( call_ptr->cmcall_mode_info.info_type )
  {
    #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
    #endif /* defined (FEATURE_GSM) || defined (FEATURE_WCDMA) */

    #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    case CM_CALL_MODE_INFO_CDMA:
      cmxcall_end(call_ptr);
      break;
    #endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */


    case CM_CALL_MODE_INFO_IP:

      #if defined(FEATURE_IP_CALL)
      /* WLAN VoIP calls are made using CM IP interface
      */
      cmipcall_end (call_ptr);
      #endif
      break;

    case CM_CALL_MODE_INFO_NONE:
      /* In this case, no actual call was made.
      ** We were in the origination process and had to end the call
      ** before mode was determined.
      ** Since this is the case, just deallocate callobject and clean
      ** up the common, non-mode specific stuff.
      */

      /* Change the call state to idle.
      */
      call_ptr->call_state = CM_CALL_STATE_IDLE;
      call_ptr->call_subst.idle= CM_CALL_IDLE_SUBST_NONE;

      /* Update the queues for this activity
      ** search the Queues and if the activity is found, update its uptime a
      ** and the update reason
      */

      (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                     CM_SS_MAIN,
                                     CM_ACT_UPDATE_REAS_ACT_END );

      (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                     CM_SS_HDR,
                                     CM_ACT_UPDATE_REAS_ACT_END );

      (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                     CM_SS_WLAN,
                                     CM_ACT_UPDATE_REAS_ACT_END );


      /* Tell phone that call is over.
         Number of allocated calls equal to 1 means that this is the
         last call that is being ended, so we need to notify the phone now.
      */
      if(cm_number_of_allocated_calls() == 1) {
        cmph_call_end( call_ptr );
      }

      /* Notify clients of call end event.
      */
      cmcall_update_call_end_info( call_ptr );
      cmcall_event( call_ptr, CM_CALL_EVENT_END );

      cm_call_id_deallocate( cmcall_call_id_get( call_ptr ) );

      /* update DDTM status */
      cmph_update_ddtm_status(cmph_ptr());

      break;

    default:
      CM_ERR_FATAL( "Bad mode = %d", call_ptr->cmcall_mode_info.info_type, 0, 0 );
  } /* switch ( call_ptr->cmcall_mode_info.info_type ) */

  #ifdef FEATURE_GPSONE
#error code not present
  #endif

} /* cmcall_end() */



/*===========================================================================

FUNCTION cmcall_client_cmd_err_deallocate

DESCRIPTION
  Wrapper around cmcall_client_cmd_err () that deallocates call id and .
  unreserves connection id. Added to free call id and connection id
  when CM_CALL_CMD_ORIG fails

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  call id and connection id get released.

===========================================================================*/
static void cmcall_client_cmd_err_deallocate(

    const cm_cmd_s_type       *cmd_ptr,
        /* Pointer to a CM command */

    cm_call_cmd_err_e_type    cmd_err
        /* Indicate the call command error */
)
{
  cm_call_id_type call_id  = CM_CALL_ID_INVALID;
  cmcall_s_type   *call_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_id = cmd_ptr->cmd.call.info.call_id;

  if( (call_ptr = cmcall_ptr (call_id)) != NULL )
  {
    cmcall_unreserve_connection_id (call_ptr);
    cm_call_id_deallocate (call_id);
  }

  cmcall_client_cmd_err (cmd_ptr, cmd_err);

} /* cmcall_client_cmd_err_deallocate () */


/*===========================================================================

FUNCTION cmcall_client_cmd_err

DESCRIPTION
  Notify clients of a specified call command error.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_client_cmd_err(

    const cm_cmd_s_type       *cmd_ptr,
        /* Pointer to a CM command */

    cm_call_cmd_err_e_type    cmd_err
        /* Indicate the call command error */
)
{
  const cm_call_cmd_s_type    *call_cmd_ptr = NULL;
      /* Point at call command component */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_CALL );
  CM_ASSERT( BETWEEN( cmd_err, CM_CALL_CMD_ERR_NONE, CM_CALL_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at call command component */
  call_cmd_ptr = CALL_CMD_PTR( cmd_ptr );
  CM_FUNC_START( "cmcall_client_cmd_err() call_cmd=%d, cmd_err=%d, client=%p",
                 call_cmd_ptr->cmd, cmd_err, call_cmd_ptr->client_id );

  /* If commanding client supplied a callback function,
  ** notify this client of command error status.
  */
  if( call_cmd_ptr->cmd_cb_func != NULL )
  {
    call_cmd_ptr->cmd_cb_func( call_cmd_ptr->data_block_ptr,
                               call_cmd_ptr->cmd,
                               cmd_err );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If error status is other than NO ERROR,
  ** notify the client list of command error, as well.
  */
  if( cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    CM_ERR( "call cmd err, cmd=%d, err=%d, client=%ld",
            call_cmd_ptr->cmd, cmd_err, call_cmd_ptr->client_id );

    /* Notify clients about the error */
    cmclient_list_call_cmd_err_ntfy( cmd_err, call_cmd_ptr );
  }

} /* cmcall_client_cmd_err() */



/*===========================================================================

FUNCTION  cmcall_call_cmd_answer_para_check

DESCRIPTION
  Check parameter errors for call answer command

DEPENDENCIES
  none
RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type  cmcall_call_cmd_answer_para_check
(
  const cm_call_cmd_s_type       *call_cmd_ptr
)
{
   #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
   #endif

   cm_call_cmd_err_e_type   cmd_err       = CM_CALL_CMD_ERR_NOERR;
   cmcall_s_type            *call_ptr     = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(call_cmd_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check integrity of command parameters */

 if( cmcall_ptr( call_cmd_ptr->info.call_id ) == NULL )
 {
   cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
   CM_DBG_ERR_FATAL( "CM_CALL_CMD_ERR_CALL_ID_P",0,0,0 );
 }
 else if( !BETWEEN(call_cmd_ptr->info.call_type, CM_CALL_TYPE_NONE, CM_CALL_TYPE_MAX) )
 {
    cmd_err  = CM_CALL_CMD_ERR_CALL_TYPE_P;
    CM_DBG_ERR_FATAL( "CM_CALL_CMD_ERR_CALL_TYPE_P",0,0,0 );
 }
 else if( (call_ptr = cmcall_ptr( call_cmd_ptr->info.call_id )) != NULL )
 {
   /* Check if the client answers the call with the right call type */
   if (call_ptr->call_type != call_cmd_ptr->info.call_type)
   {
     cmd_err = CM_CALL_CMD_ERR_CALL_TYPE_P;
     MSG_ERROR ("call type mismatch between answering client and CM \n", 0, 0, 0);
   }
 }

 #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
 #endif /* if defined (FEATURE_GSM) || defined (FEATURE_WCDMA) */

 return cmd_err;

} /* cmcall_call_cmd_answer_para_check() */



/*===========================================================================

FUNCTION  cmcall_call_cmd_get_call_info_para_check

DESCRIPTION
  Check parameter errors for get call info command

DEPENDENCIES
  none

RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type  cmcall_call_cmd_get_call_info_para_check
(
  const cm_call_cmd_s_type       *call_cmd_ptr
)
{
  cm_call_cmd_err_e_type   cmd_err       = CM_CALL_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check integrity of command parameters */

  if( cmcall_ptr( call_cmd_ptr->info.call_id ) == NULL )
  {
    cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
    CM_DBG_ERR_FATAL( "CM_CALL_CMD_ERR_CALL_ID_P",0,0,0 );
  }
  else if( call_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE )
  {
    cmd_err  = CM_CALL_CMD_ERR_CLIENT_ID_P;
    CM_DBG_ERR_FATAL( "CM_CALL_CMD_ERR_CLIENT_ID_P",0,0,0 );
  }
  else if( (call_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK )
  {
    cmd_err  = CM_CALL_CMD_ERR_CLIENT_ID_P;
    CM_DBG_ERR_FATAL( "CM_CALL_CMD_ERR_CLIENT_ID_P",0,0,0 );
  }

  return cmd_err;
} /* cmcall_call_cmd_get_call_info_para_check() */



/*===========================================================================

FUNCTION cmcall_get_favored_roam_pref

DESCRIPTION
  Return the favored roam preference to be used for a specified call-type,
  service-state, roam-status and a roam-preference.

DEPENDENCIES
  None.

RETURN VALUE
  The favored roam preference to be used with a specified call-type,
  service-state, roam-status and a roam-preference.

SIDE EFFECTS
  None.

===========================================================================*/
cm_roam_pref_e_type  cmcall_get_favored_roam_pref(

    cm_call_type_e_type    call_type,
        /* Call which we try to originate */

    sys_sys_mode_e_type    srv_mode,
        /* The current service mode */

    sys_roam_status_e_type roam_status,
        /* The current roaming status */

    cm_roam_pref_e_type    roam_pref
        /* The current roaming preference */

)
{
  /* If this is an emergency call, relax the roaming preference.
  */
  if( call_type == CM_CALL_TYPE_EMERGENCY )
  {
    return CM_ROAM_PREF_ANY;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we don't have service at the moment, use the existing roaming
  ** preference.
  */
  if( srv_mode == SYS_SYS_MODE_NO_SRV )
  {
    return roam_pref;
  }

  /*-------------------------------------------------------------------------
      If we got here, this is not an emergency call and we have service.
  -------------------------------------------------------------------------*/

  /* To ensures that the phone will not suddenly switch to a less preferred
  ** roaming while doing silent redials, limit the roaming preference in
  ** accordance with the current roaming status and roaming preference, as
  ** necessary.
  */
  switch( roam_pref )
  {
    case CM_ROAM_PREF_HOME:
      break;

    case CM_ROAM_PREF_ROAM_ONLY:
      break;

    case CM_ROAM_PREF_AFFIL:
      if( roam_status == SYS_ROAM_STATUS_OFF )
      {
        return CM_ROAM_PREF_HOME;
      }
      break;

    case CM_ROAM_PREF_ANY:
      if( roam_status == SYS_ROAM_STATUS_OFF )
      {
        return CM_ROAM_PREF_HOME;
      }
      if( roam_status == SYS_ROAM_STATUS_ON )
      {
        return CM_ROAM_PREF_AFFIL;
      }
      break;

    case CM_ROAM_PREF_NONE:
    case CM_ROAM_PREF_NO_CHANGE:
    case CM_ROAM_PREF_MAX:
    default:
      CM_CASE_ERR( "bad roam_pref = %d",roam_pref,0,0 );
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, it is not necessary to limit the roaming preference -
  ** return the existing roaming preference.
  */
  return roam_pref;

} /* cm_call_get_favored_roam_pref() */



/*===========================================================================

FUNCTION cmcall_get_favored_prl_pref

DESCRIPTION
  Return the favored prl preference to be used for a specified call-type.


DEPENDENCIES
  None.

RETURN VALUE
  The favored prl preference to be used with a specified call-type.


SIDE EFFECTS
  None.

===========================================================================*/
cm_prl_pref_e_type         cmcall_get_favored_prl_pref(

    cm_call_type_e_type    call_type
        /* Call which we try to originate */

)
{
  /* If this is an emergency call, relax the prl preference.
  */
  if( call_type == CM_CALL_TYPE_EMERGENCY )
  {
    return CM_PRL_PREF_ANY;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else use the current prl preference setting.
  */
  return CM_PRL_PREF_NO_CHANGE;

} /* cm_call_get_favored_prl_pref() */



/*===========================================================================

FUNCTION cmcall_get_favored_band_pref

DESCRIPTION
  Return the favored band preference to be use with a specified
  call type.

DEPENDENCIES
  none

RETURN VALUE
  The favored band preference to be use with a specified call type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_band_pref_e_type  cmcall_get_favored_band_pref(

    cm_call_type_e_type    call_type,
        /* Call type */

    cm_srv_type_e_type     srv_type,
        /* Service type */

    cm_band_pref_e_type    band_pref
        /* The current band preference */
)
{
  /* For standard-OTASP or emergency call types relax the band preference to
  ** ensure that service can be acquired.
  */
  if( call_type == CM_CALL_TYPE_STD_OTASP ||
      call_type == CM_CALL_TYPE_EMERGENCY )
  {
    return CM_BAND_PREF_ANY;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For AMPS service type relax the band preference to insure that AMPS
  ** service can be acquired (e.g. current band preference might be PCS).
  */
  if( srv_type == CM_SRV_TYPE_AMPS )
  {
    return CM_BAND_PREF_ANY;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, it is ok to use the existing band preference.
  */
  return band_pref;

} /* cmcall_get_favored_band_pref() */


/*===========================================================================

FUNCTION cmcall_get_favored_srv_domain_pref

DESCRIPTION
  Return the favored service domain preference to be use with a specified
  call type.

DEPENDENCIES
  none

RETURN VALUE
  The favored service domain preference to be use with a specified call type.

SIDE EFFECTS
  none

===========================================================================*/
cm_srv_domain_pref_e_type        cmcall_get_favored_srv_domain_pref(

    cm_call_type_e_type          call_type
        /* The Call type enum type in call object*/
)
{

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Force the service domain preferences in accordance with the
  ** call-type and service domain preferences.
  */
  if( call_type ==  CM_CALL_TYPE_PS_DATA )
  {
    return CM_SRV_DOMAIN_PREF_PS_ONLY;
  }
  else
  {
    return CM_SRV_DOMAIN_PREF_CS_ONLY;
  }
} /* cmcall_get_favored_srv_domain_pref() */

#if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA))
#error code not present
#endif /* (defined (FEATURE_GSM) || defined (FEATURE_WCDMA)) */


#if( defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))


/*===========================================================================

FUNCTION cmcall_cmd_info_cdma_init

DESCRIPTION
   Function to initialize default cdma voice call origination parameters.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_cmd_info_cdma_init (

  cm_cmd_s_type  *cmd_ptr             /* pointer to a CM command */

)
{

  cm_call_cmd_s_type              *call_cmd_ptr  = NULL;
  cm_call_cmd_info_cdma_s_type    *cdma_info_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( cmd_ptr != NULL );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_cmd_ptr                      = CALL_CMD_PTR( cmd_ptr );
  cdma_info_ptr                     = CALL_CMD_MODE_INFO_PTR (call_cmd_ptr);

  cdma_info_ptr->srv_opt            = CM_SRV_OPT_NONE;
  cdma_info_ptr->otasp_act_code     = CM_OTASP_ACT_CODE_NONE;

  #ifdef FEATURE_IS2000
  cdma_info_ptr->drs_bit            = TRUE;
  /* set the DRS bit or not */
  #endif /* FEATURE_IS2000 */

  cdma_info_ptr->privacy_pref       = CM_PRIVACY_PREF_NONE;

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif /* FEATURE_HDR_HANDOFF */

  #ifdef FEATURE_IS2000_REL_A
  cdma_info_ptr->sr_id_included     = FALSE;
  /* sr_id indicator */

  cdma_info_ptr->qos_parms_incl     = FALSE;
  /* Qos Parameters indicator */

  cdma_info_ptr->qos_parms_len      = 0;
  /* Length of QOS parameters */

  #endif /* FEATURE_IS2000_REL_A */

  cdma_info_ptr->flash_type         = CM_CALL_SUPS_TYPE_NONE;
  /* type of flash */

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
  #endif

} /* cmcall_cmd_info_cdma_init () */

#endif // #if( defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))


/*===========================================================================

FUNCTION cmcall_is_hybr_data_switchover_call

DESCRIPTION
 Check if the ps call is a hybrid data switchover call.

DEPENDENCIES
 None

RETURN VALUE
 TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
static boolean  cmcall_is_hybr_data_switch_over_call(

  const cmcall_s_type               *call_ptr,
      /* Pointer to a phone object */
  cm_mode_pref_e_type         favored_mode_pref
  /* Favored mode preference for this call if it is the only call being originated */
)
{

  cmph_s_type               *ph_ptr = cmph_ptr();
      /* Pointer to a phone object */

  cmss_s_type               *ss_ptr = cmss_ptr();
      /* Pointer to a serving system object */

  boolean                   is_hybr_data_switch_over;
      /* Indicate if this is a hybrid data switch over call. */


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_FUNC_START( "cmcall_is_hybr_data_switchover_call()",0,0,0 );

  CM_ASSERT( call_ptr != NULL );

  CM_ASSERT( favored_mode_pref < CM_MODE_PREF_MAX );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If call type is PS data and the call is originated on hdr and
  ** current service is GW and the mode preference does not contain CDMA and
  ** HDR and the favored mode preference does not contain GW,
  ** set the hybrid data switch over to TRUE.
  */
  if( ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA ) &&

      ( call_ptr->ss == CM_SS_HDR )                   &&

      ( ss_ptr->info.sys_mode == SYS_SYS_MODE_GSM ||
        ss_ptr->info.sys_mode == SYS_SYS_MODE_WCDMA ) &&

      ( ph_ptr->mode_pref  == cmcall_remove_mode_pref_components(
          ph_ptr->mode_pref, 1, SD_SS_MODE_PREF_CDMA_HDR)
                                                      &&
        favored_mode_pref == cmcall_remove_mode_pref_components(
          favored_mode_pref, 1, SD_SS_MODE_PREF_GW) ))
  {
     is_hybr_data_switch_over = TRUE;
  }
  else
  {
    is_hybr_data_switch_over = FALSE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_MSG_HIGH( "Hybrid Data switch over= %d, call state = %d",
               is_hybr_data_switch_over,
               call_ptr->call_state,
               0);

  return is_hybr_data_switch_over;

}





/*===========================================================================

FUNCTION cmcall_force_pref_on_the_fly

DESCRIPTION
  Force the system selection preferences in accordance with the call-type
  and its favored mode preferences.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

  This function is expected to be called during the origination process
  before call is connected.

RETURN VALUE
  A ballpark estimation for the number of seconds the phone is going
  to be without service.

SIDE EFFECTS
  None.

===========================================================================*/
dword  cmcall_force_pref_on_the_fly(

    cmcall_s_type             *call_ptr,
        /* Pointer to a call object */

    sd_ss_pref_reas_e_type    force_reason
        /* Reason for forcing the SS-preference.
        */
)
{

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  cdma_cmcall_type *call_mode_info_ptr = NULL;
    /* Pointer to call mode info */
  #endif /* else of (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))*/

  cmph_s_type              *ph_ptr            = cmph_ptr();
    /* Point at phone object */

  cm_band_pref_e_type       favored_band_pref;
    /* Favored band preference for this call */

  cm_roam_pref_e_type       favored_roam_pref;
    /* Favored roam preference for this call */

  cm_srv_domain_pref_e_type favored_srv_pref  = ph_ptr->srv_domain_pref;
    /* Favored service domain preference for this call */

  cm_activate_code_e_type   otasp_code        = CM_OTASP_ACT_CODE_NONE;
    /* otasp activation code */

  cm_mode_pref_e_type       favored_mode_pref = CM_MODE_PREF_NONE;
    /* Favored mode preference for this call if it is the only call being originated */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmcall_force_pref_on_the_fly()",0,0,0 );
  CM_ASSERT( call_ptr    != NULL );
  CM_ASSERT( force_reason < SD_SS_PREF_REAS_MAX );
  CM_ASSERT( ph_ptr      != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );
  #endif

  /* Set the ok to orig indication to FALSE.
  */
  call_ptr->sr.is_ok_to_orig      = FALSE;

  call_ptr->sr.wlan_is_ok_to_orig = FALSE;

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    otasp_code =
      ( call_ptr->call_type == CM_CALL_TYPE_STD_OTASP ||
        call_ptr->call_type == CM_CALL_TYPE_NON_STD_OTASP ) ?
      call_mode_info_ptr->otasp_info.otasp_act_code : CM_OTASP_ACT_CODE_NONE;
  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is NOT the first time we force the preferences for this
  ** origination attempt, just use the existing call origination preferences.
  */

  if(BETWEEN(call_ptr->ss, CM_SS_NONE, CM_SS_MAX))
  {

    cm_orig_q_s_type *computed_orig_para =  cmtask_orig_para_search_act_id  (
                                                         call_ptr->ss,
                                         (cm_act_id_type)call_ptr->call_id );
    if( computed_orig_para != NULL &&
        computed_orig_para->orig.orig_mode != SD_SS_ORIG_MODE_NONE )
    {
      /* fetch the current srv_pref in use.
      */
      favored_srv_pref = computed_orig_para->orig.orig_srv_domain_pref;
    }
    else
    {
      favored_srv_pref = ph_ptr->srv_domain_pref;
    }

    if( call_ptr->orig_mode != SD_SS_ORIG_MODE_NONE ||
        ( computed_orig_para != NULL                &&
          computed_orig_para->orig.orig_mode != SD_SS_ORIG_MODE_NONE &&
          cmcall_count_on_ss(call_ptr->ss) > 1
        )
      )
      {
        /* Force the service domain preferences in accordance with the
        ** call-type and originate service domain preferences.
        */
        favored_srv_pref = cmcall_get_favored_srv_domain_pref( call_ptr->call_type );

        call_ptr->orig_mode =
                     cmph_map_cm_call_type_to_sd_orig_mode( call_ptr->call_type );

        if( computed_orig_para != NULL )
        {
          return cmph_force_pref_on_the_fly(
                               call_ptr->ss,
                               ph_ptr,
                               force_reason,
                               cmph_map_call_type_to_act_type( call_ptr->call_type ),
                               CM_PREF_TERM_CM_1_CALL_PLUS,
                               computed_orig_para->orig.orig_mode_pref,
                               computed_orig_para->orig.orig_band_pref,
                               computed_orig_para->orig.orig_prl_pref,
                               computed_orig_para->orig.orig_roam_pref,
                               computed_orig_para->orig.orig_hybr_pref,
                               ph_ptr->plmn,
                               favored_srv_pref,
                               otasp_code,
                               &ph_ptr->wlan_pref,
                               ph_ptr->acq_order_pref,
                               CM_NETWORK_SEL_MODE_PREF_NONE,
                               (cm_act_id_type) call_ptr->call_id,
                               CM_ACT_UPDATE_REAS_ACT_START,
                               call_ptr->force_top_pref );
        } /* if( computed_orig_para != NULL ) */
        else
        {
          CM_ERR(" Error in cmcall_force_pref_on_th_fly()",0,0,0);
          return 0;
        }

      } /* call_ptr->orig_mode */

  } /* if((call_ptr->ss > CM_SS_NONE) && (call_ptr->ss < CM_SS_MAX)) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, this is the first time we force the preferences for this
  ** call origination.
  */

  /* Get the favored band preference to be used for this call-type and
  ** service-type.
  */
  favored_band_pref = cmcall_get_favored_band_pref(
                                             call_ptr->call_type,
                                             call_ptr->srv_type,
                                             ph_ptr->band_pref
                                                  );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For standard-OTASP or emergency call types relax the roam preference to
  ** ensure that service is acquired regardless of roaming indication. For
  ** all other call types use the existing roam preference.
  */
  if( call_ptr->call_type == CM_CALL_TYPE_STD_OTASP ||
      call_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
  {
    favored_roam_pref = CM_ROAM_PREF_ANY;
  }
  else
  {
    favored_roam_pref = ph_ptr->roam_pref;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Force the service domain preferences in accordance with the
  ** call-type and service domain preferences.
  */

  favored_srv_pref = cmcall_get_favored_srv_domain_pref( call_ptr->call_type );

  if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
    favored_mode_pref = CM_MODE_PREF_EMERGENCY;
  }
  else
  {
    favored_mode_pref = call_ptr->favored_mode_pref;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Force the system selection preferences in accordance with the
  ** call-type and its favored mode and band preferences.
  */
  call_ptr->orig_mode =
               cmph_map_cm_call_type_to_sd_orig_mode( call_ptr->call_type );

  /* If this is a hybr data switch over call, jump start 1x and HDR.
  */
  if( cmcall_is_hybr_data_switch_over_call( call_ptr, favored_mode_pref ) )
  {

    (void) cmph_force_pref_on_the_fly(
                                       CM_SS_MAIN,
                                       ph_ptr,
                                       force_reason,
                                       cmph_map_call_type_to_act_type( call_ptr->call_type ),
                                       CM_PREF_TERM_CM_1_CALL_PLUS,
                                       favored_mode_pref,
                                       favored_band_pref,
                                       call_ptr->favored_prl_pref,
                                       favored_roam_pref,
                                       call_ptr->favored_hybr_pref,
                                       ph_ptr->plmn,
                                       favored_srv_pref,
                                       otasp_code,
                                       &ph_ptr->wlan_pref,
                                       ph_ptr->acq_order_pref,
                                       CM_NETWORK_SEL_MODE_PREF_NONE,
                                       (cm_act_id_type) call_ptr->call_id,
                                       CM_ACT_UPDATE_REAS_ACT_START,
                                       call_ptr->force_top_pref
                                     );

    (void) cmph_force_pref_on_the_fly(
                                       CM_SS_HDR,
                                       ph_ptr,
                                       SD_SS_PREF_REAS_USER,
                                       cmph_map_call_type_to_act_type( call_ptr->call_type ),
                                       CM_PREF_TERM_CM_1_CALL_PLUS,
                                       favored_mode_pref,
                                       favored_band_pref,
                                       call_ptr->favored_prl_pref,
                                       favored_roam_pref,
                                       call_ptr->favored_hybr_pref,
                                       ph_ptr->plmn,
                                       favored_srv_pref,
                                       otasp_code,
                                       &ph_ptr->wlan_pref,
                                       ph_ptr->acq_order_pref,
                                       CM_NETWORK_SEL_MODE_PREF_NONE,
                                       (cm_act_id_type) call_ptr->call_id,
                                       CM_ACT_UPDATE_REAS_ACT_START,
                                       call_ptr->force_top_pref
                                     );


  }
  else if( call_ptr->call_type == CM_CALL_TYPE_PS_DATA &&
           call_ptr->ss        != CM_SS_MAIN           &&
           #ifndef FEATURE_UNIFORM_SCAN_OOS
           cmcall_misc_is_mode_pref( call_ptr->favored_mode_pref,
                                     CM_MODE_PREF_CDMA_ONLY ) &&
           #endif
           #ifdef FEATURE_HYBR_GW
#error code not present
           #endif
           (cmcall_check_if_no_srv(TRUE)
            #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) \
                && defined( FEATURE_BSR2)
            || cmregprx_get_info_ptr()->state == CMREGPRX_STATE_ACTIVE
            #endif
            )
         )
  {

    /* If this is a PS_DATA call and 1X is present in the mode_pref,
    ** and we are not kicking 1x, and both 1x and HDR are in powersave
    ** then jumpstart 1x
    */

    dword no_srv_time = 0;
    SYS_ARG_NOT_USED(no_srv_time);

    #ifdef FEATURE_UNIFORM_SCAN_OOS
    if( ! cmcall_misc_is_mode_pref( call_ptr->favored_mode_pref,
                                     CM_MODE_PREF_CDMA_ONLY ) &&
          cmcall_misc_is_mode_pref ( call_ptr->favored_mode_pref,
                                     CM_MODE_PREF_HDR_ONLY))
    {
      favored_mode_pref = cmcall_add_mode_pref_components( favored_mode_pref, 1, SD_SS_MODE_PREF_CDMA );
    }
    #endif

    no_srv_time = cmph_force_pref_on_the_fly(
                                       CM_SS_MAIN,
                                       ph_ptr,
                                       force_reason,
                                       cmph_map_call_type_to_act_type( call_ptr->call_type ),
                                       CM_PREF_TERM_CM_1_CALL_PLUS,
                                       favored_mode_pref,
                                       favored_band_pref,
                                       call_ptr->favored_prl_pref,
                                       favored_roam_pref,
                                       call_ptr->favored_hybr_pref,
                                       ph_ptr->plmn,
                                       favored_srv_pref,
                                       otasp_code,
                                       &ph_ptr->wlan_pref,
                                       ph_ptr->acq_order_pref,
                                       CM_NETWORK_SEL_MODE_PREF_NONE,
                                       (cm_act_id_type) call_ptr->call_id,
                                       CM_ACT_UPDATE_REAS_ACT_START,
                                       call_ptr->force_top_pref
                                     );

  }

  return cmph_force_pref_on_the_fly(
                                       call_ptr->ss,
                                       ph_ptr,
                                       force_reason,
                                       cmph_map_call_type_to_act_type( call_ptr->call_type ),
                                       CM_PREF_TERM_CM_1_CALL_PLUS,
                                       favored_mode_pref,
                                       favored_band_pref,
                                       call_ptr->favored_prl_pref,
                                       favored_roam_pref,
                                       call_ptr->favored_hybr_pref,
                                       ph_ptr->plmn,
                                       favored_srv_pref,
                                       otasp_code,
                                       &ph_ptr->wlan_pref,
                                       ph_ptr->acq_order_pref,
                                       CM_NETWORK_SEL_MODE_PREF_NONE,
                                       (cm_act_id_type) call_ptr->call_id,
                                       CM_ACT_UPDATE_REAS_ACT_START,
                                       call_ptr->force_top_pref
                                     );

} /* cmcall_force_pref_on_the_fly() */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */


/**--------------------------------------------------------------------------
** Functions - external:commands from top (cmtask)
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmcall_timer_proc

DESCRIPTION
  Process timer events

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_timer_proc(

  cm_timer_event_type    timer_event
    /* Indicate specific timer event.
    **
    ** Note that currently timer event is not being used
    */
)
/*lint -esym(715,timer_event) */
{
  cmcall_s_type       *call_ptr;
      /* Point at call object */
  cm_iterator_type    call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmcall_timer_proc()",0,0,0 );

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
    {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Do call state specific timer processing.
    */
    switch( call_ptr->call_state )
    {
      case CM_CALL_STATE_IDLE:
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CM_CALL_STATE_ORIG:

        /* Call the orig function periodically to check the
        ** no-service, hold-orig, and silent redial situation.
        */
        cmcall_orig_proc( call_ptr );
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CM_CALL_STATE_INCOM:
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CM_CALL_STATE_CC_IN_PROGRESS:
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CM_CALL_STATE_CONV:
        break; /* Nothing for GW; Processed after the switch for 1x */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CM_CALL_STATE_RECALL_RSP_PEND:
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      default:
        CM_SWITCH_ERR( "Call state=%d",call_ptr->call_state,0,0 );
    } /* switch( call_ptr->call_state ) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    /* Do NON call state specific timer processing (and conv state).
    */
    if ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA )
    {
      cmxcall_timer_proc( call_ptr );
    } /* if ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA ) */
    #endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

} /* cmcall_timer_proc() */
/*lint +esym(715,timer_event) */



/*===========================================================================
FUNCTION CMCALL_OFFLINE

DESCRIPTION
  This function performs offline processing for the call object

DEPENDENCIES
  Call object must be initialized

RETURNS
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_offline (void)
{
  cmcall_s_type       *call_ptr;
      /* Point at call object */
  cm_iterator_type    call_itr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  /*
  ** Send PDP_ABORT_REQ so that all PDP calls are locally deactivated by NAS,
  ** before CM unreserves SM connection id.
  */
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

  while(call_ptr != NULL)
  {

    if(call_ptr->call_state != CM_CALL_STATE_IDLE)
    {
      call_ptr->end_status = CM_CALL_END_OFFLINE;
      cmcall_end( call_ptr );
    } /* if(call_ptr->call_state != CM_CALL_STATE_IDLE) */

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

} /* cmcall_offline() */



/*===========================================================================

FUNCTION cmcall_get_call_info_list_proc

DESCRIPTION
  This function processes the get call info list command.  This function
  returns to the requesting cilent a sanpshot of the state of all call objects
  maintained by CM.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  This function returns the state of the call objects in the info parameter.

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_get_call_info_list_proc
(
  cm_cmd_s_type  *cmd_ptr                       /* pointer to a CM command */
)
{


  cm_call_state_info_list_s_type  info_list;
    /* The state of the call objects maintained by CM. */

  cm_call_state_info_list_s_type  *info_list_ptr = &info_list;
      /* The state of the call objects maintained by CM. */

  cm_call_cmd_s_type    *call_cmd_ptr;
      /* call_cmd_ptr to extract client_ptr from cmd_ptr */

  cmcall_s_type    *call_ptr;
  uint8             active_call_index = 0;
  cm_iterator_type  call_itr;

  info_list_ptr->number_of_active_calls = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( cmd_ptr != NULL );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_cmd_ptr = CALL_CMD_PTR( cmd_ptr );

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {



      info_list_ptr->number_of_active_calls++;
      info_list_ptr->info[active_call_index].call_id           = cmcall_call_id_get( call_ptr );
      info_list_ptr->info[active_call_index].call_type         = call_ptr->call_type;
      info_list_ptr->info[active_call_index].call_state        = call_ptr->call_state;
      info_list_ptr->info[active_call_index].call_subst        = call_ptr->call_subst;
      info_list_ptr->info[active_call_index].sys_mode          = call_ptr->sys_mode;
      info_list_ptr->info[active_call_index].call_connect_time = call_ptr->call_connect_time;
      info_list_ptr->info[active_call_index].alpha             = call_ptr->alpha;
      info_list_ptr->info[active_call_index].call_direction    = CM_CALL_DIRECTION_NONE;
      info_list_ptr->info[active_call_index].primary_pdp       = FALSE;

      #if defined(FEATURE_SECONDARY_PDP)
#error code not present
      #else
      info_list_ptr->info[active_call_index].primary_call_id       = CM_CALL_ID_INVALID;
      #endif /* FEATURE_SECONDARY_PDP */

      info_list_ptr->info[active_call_index].profile_number        = 0;

      #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
      #endif /* defined (FEATURE_GSM ) || defined (FEATURE_WCDMA ) */

      ++active_call_index;

      call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

  /* Callback to inform client with call info list */
  cmclient_call_info_list_ntfy (call_cmd_ptr->client_ptr, info_list_ptr);

} /* cmcall_get_call_info_list_proc */


#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)

/*===========================================================================

FUNCTION cmcall_mc_rpt_proc

DESCRIPTION
  Process MC reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
)
{
  cmxcall_mc_rpt_proc( rpt_ptr );

} /* cmcall_mc_rpt_proc() */
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */



/*===========================================================================

FUNCTION cmcall_rpt_proc

DESCRIPTION
  Process lower layer reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
)
/*lint -esym(715, rpt_ptr)*/
{
  #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
  #else
  CM_ERR_FATAL("Illegal mode. CM not compiled to support GSM/CDMA", 0, 0, 0);
  #endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

} /* cmcall_rpt_proc() */
/*lint +esym(715,rpt_ptr)*/

/*===========================================================================

FUNCTION cmcall_process_ok_to_orig_gw

DESCRIPTION
  Process ok_to_orig for GW mode.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  info_type in call_ptr gets modified.

===========================================================================*/
static void cmcall_process_ok_to_orig_gw(

  sys_sys_mode_e_type ok_to_orig_mode,
    /* mode CDMA, WCDMA ... */

  cmcall_s_type       *call_ptr
    /* Call object */
)
{



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT (call_ptr != NULL);

  CM_ASSERT (BETWEEN (ok_to_orig_mode, SYS_SYS_MODE_NONE, SYS_SYS_MODE_MAX));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Given call can be originated as a ip call */
  if (cmcall_is_ipcall_over_mode_poss (ok_to_orig_mode, call_ptr))
  {
    call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Given call will be originated as Packet call */
  if (call_ptr->call_type == CM_CALL_TYPE_PS_DATA)
  {
    call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_PS;

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* All default call types to be originated as CS calls */
  call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;

  return;

} /* cmcall_process_ok_to_orig_gw () */

/*===========================================================================

FUNCTION cmcall_sd_rpt_proc

DESCRIPTION
  Process system determination reports

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
)
{
  /*lint -save -e826 */
  const cm_sd_rpt_u_type  *sd_rpt_ptr = (cm_sd_rpt_u_type *) rpt_ptr;
  /*lint -restore */
    /* Pointer to SD report */

  cmcall_s_type   *call_ptr = NULL;
      /* Pointer to call object */

  cmss_s_type   *ss_ptr = cmss_ptr();
      /* Serving System pointer */

  cm_iterator_type call_itr;
      /* call Object Iterator */

  sd_mode_e_type   ok_to_orig_mode = SD_MODE_NONE;
     /* Carries mode on which SD is giving ok to orig */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sd_rpt_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at SD report component */
  CM_FUNC_START( "cmcall_sd_rpt_proc, cmd=%d", sd_rpt_ptr->hdr.cmd, 0, 0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ok_to_orig_mode = sd_rpt_ptr->ok_to_orig.si_ok_to_orig.mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( sd_rpt_ptr->hdr.cmd )
  {
                 /*----------------------------------------*/
                 /*  OK to send origination command to LL  */
                 /*----------------------------------------*/

    case CM_OK_TO_ORIG_F:

      CM_MSG_HIGH("ok_to_orig, ss %d, mode %d, srv_domain %d",
                  sd_rpt_ptr->ok_to_orig.ss,
                  sd_rpt_ptr->ok_to_orig.si_ok_to_orig.mode,
                  sd_rpt_ptr->ok_to_orig.si_ok_to_orig.srv_domain
                 );

      CM_MSG_LOW("ok_to_orig, ps_data_suspend %d, srv_status %d",
                  sd_rpt_ptr->ok_to_orig.si_ok_to_orig.ps_data_suspend,
                  sd_rpt_ptr->ok_to_orig.si_ok_to_orig.srv_status,
                  0
                 );

      cmcall_obj_iterator_init(&call_itr);

      call_ptr = cmcall_obj_get_next(&call_itr);

      while(call_ptr != NULL)
      {

        /* If call state is not origination, discard this command.
        */

        /* Need to verify that the ok to orig is for this call
        ** with the priority Queues, it might happen that we force to the
        ** top of the Queue and the top is not conforming with the call
        ** No checks for roam and band as CM doesn't get those values witjh
        ** the OK to ORIG report
        */

        #ifdef FEATURE_HYBR_GW
#error code not present
        #endif

        if( call_ptr->call_state != CM_CALL_STATE_ORIG ||
            ! cmcall_misc_is_mode_pref(
                call_ptr->favored_mode_pref,
                cmph_map_sd_mode_pref_to_cm_mode_pref(
                  (sd_ss_mode_pref_e_type)BM(sd_rpt_ptr->ok_to_orig.si_ok_to_orig.mode)))
          )
        {
          call_ptr = cmcall_obj_get_next(&call_itr);
          continue;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


        /* Indicate that is now ok to send the origination command to MC
        ** and call on the origination processing function to process this
        ** indication.
        */

        switch(sd_rpt_ptr->ok_to_orig.ss)
        {

          case SD_SS_HYBR_WLAN:
            call_ptr->sr.wlan_is_ok_to_orig = TRUE;
            break;

          #if (defined (FEATURE_HDR_HYBRID) || defined( FEATURE_HYBR_GW ))
#error code not present
          #endif

          case SD_SS_MAIN:
          default:

            if (ss_ptr != NULL)
            {
              /* We are already camped, so go ahead with origination.
              */
              if ( sd_rpt_ptr->ok_to_orig.si_ok_to_orig.srv_domain ==
                                                      SYS_SRV_DOMAIN_CAMPED )
              {
                if( sd_rpt_ptr->ok_to_orig.ss == SD_SS_MAIN )
                {
                  call_ptr->sr.is_ok_to_orig = TRUE;
                }
                else
                {
                  call_ptr->sr.hdr_is_ok_to_orig = TRUE;
                }
              }
              else
              {
                /* If PS call and service domain is cs only, does not allow to
                ** origination.
                */
                if(! (call_ptr->call_type == CM_CALL_TYPE_PS_DATA   &&
                      sd_rpt_ptr->ok_to_orig.si_ok_to_orig.srv_domain ==
                                                      SYS_SRV_DOMAIN_CS_ONLY)
                )
                {
                  /*
                  ** Does not allow origination if call type is Voice or CS_DATA
                  ** and ok_to_orig srv_domain is PS_ONLY
                  */
                  if ( !(((call_ptr->call_type == CM_CALL_TYPE_CS_DATA) ||
                          (call_ptr->call_type == CM_CALL_TYPE_VOICE))  &&
                          (sd_rpt_ptr->ok_to_orig.si_ok_to_orig.srv_domain ==
                                                    SYS_SRV_DOMAIN_PS_ONLY)))
                  {
                    if( sd_rpt_ptr->ok_to_orig.ss == SD_SS_MAIN )
                    {
                      call_ptr->sr.is_ok_to_orig = TRUE;
                    }
                    else
                    {
                      call_ptr->sr.hdr_is_ok_to_orig = TRUE;
                    }
                  }
                  /* Exception to the check above is when hdr is present on
                  ** Main and call can be originated through a Voip app
                  ** registered on HDR
                  **
                  ** MODE_INFO_IP and (ok_to_orig == TRUE) are checked to send
                  ** call orig on HDR as Voip. cmcall_is_voice_over_hdr_poss()
                  ** is not required here, since it is being done before
                  ** setting info_type.
                  */
                  else if ( (call_ptr->call_type == CM_CALL_TYPE_VOICE) &&
                            (call_ptr->ss        == CM_SS_MAIN)         &&
                            (sd_rpt_ptr->ok_to_orig.si_ok_to_orig.mode ==
                                                    SD_MODE_HDR )
                          )
                  {
                    call_ptr->sr.is_ok_to_orig = TRUE;
                  }

                }

              } /* else */

            } /* if (ss_ptr != NULL) */

            break; /* case SD_SS_MAIN: default: */

        }

        switch(ok_to_orig_mode)
        {

          case SD_MODE_HDR:

            if (cmcall_is_ipcall_over_mode_poss (ok_to_orig_mode, call_ptr))
            {
              call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
            }
            else
            {
              call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
            }
            break;

          case SD_MODE_AMPS:
          case SD_MODE_CDMA:
            call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
            break;

          case SD_MODE_GSM:

            cmcall_process_ok_to_orig_gw (SYS_SYS_MODE_GSM, call_ptr);
            break;

          case SD_MODE_WCDMA:

            cmcall_process_ok_to_orig_gw (SYS_SYS_MODE_WCDMA, call_ptr);
            break;

          case SD_MODE_WLAN:
            if(cmcall_is_ipcall_over_mode_poss (ok_to_orig_mode, call_ptr))
            {
              call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
            }
            break;

          case SD_MODE_GPS:
          default:
            MSG_ERROR ("OK_TO_ORIG returned invalid mode %d",
                 call_ptr->cmcall_mode_info.info_type, 0, 0);
            cmcall_end (call_ptr);
            call_ptr = cmcall_obj_get_next(&call_itr);
            continue;
        }

        CM_MSG_HIGH ("OK_TO_ORIG in mode %d", call_ptr->cmcall_mode_info.info_type, 0, 0);

        cmcall_orig_proc( call_ptr );

        call_ptr = cmcall_obj_get_next(&call_itr);
      } /* while */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

               /*-------------------------------------------*/
               /*   Acquisition on all systems has failed   */
               /*-------------------------------------------*/

    case CM_ACQ_FAIL_F:

      CM_MSG_HIGH("ACQ_FAIL_F, ss %d", sd_rpt_ptr->acq_fail.ss, 0, 0 );
      cmcall_obj_iterator_init(&call_itr);

      call_ptr = cmcall_obj_get_next(&call_itr);

      while(call_ptr != NULL)
      {

        /* If this report is not for this call, continue.
        */
        if( sd_rpt_ptr->acq_fail.ss != cmph_map_cm_ss_type_to_sd_ss_type(call_ptr->ss) )
        {
          call_ptr = cmcall_obj_get_next(&call_itr);
          continue;
        }

        /* If standard OTASP origination is in progress, indicate the
        ** call-end status and end the call.
        */
        if( call_ptr->call_state  == CM_CALL_STATE_ORIG &&
            call_ptr->call_type   == CM_CALL_TYPE_STD_OTASP )
        {
          call_ptr->end_status = CM_CALL_END_ACTIVATION;
          cmcall_end( call_ptr );
        }
        #if ( defined (FEATURE_UNIFORM_SCAN_OOS) || \
              defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING) )
        else if( call_ptr->call_state  == CM_CALL_STATE_ORIG     &&
                 call_ptr->call_type   != CM_CALL_TYPE_EMERGENCY &&
                 sd_rpt_ptr->acq_fail.ss == SD_SS_MAIN )
        {
          call_ptr->end_status = CM_CALL_END_NO_SRV;
          cmcall_end( call_ptr );
        }
        #endif
        /* If we get ACQ_FAIL when we have LIMITED service,
        ** it means that full service was not found, so inform
        ** orig_proc of NO_GW_SRV.
        */
        else if( call_ptr->call_state  == CM_CALL_STATE_ORIG &&
                 sd_rpt_ptr->acq_fail.ss == SD_SS_MAIN &&
                 ( cmss_ptr()->main_srv_status == SYS_SRV_STATUS_LIMITED ||
                   cmss_ptr()->main_srv_status == SYS_SRV_STATUS_LIMITED_REGIONAL
                 )
               )
        {
          call_ptr->end_status = CM_CALL_END_NO_FULL_SRV;
          cmcall_orig_proc( call_ptr );
        }
        else if( call_ptr->call_state  == CM_CALL_STATE_ORIG &&
                 sd_rpt_ptr->acq_fail.ss == SD_SS_HYBR_1 &&
                 ( cmss_ptr()->hybr_1_srv_status == SYS_SRV_STATUS_LIMITED ||
                   cmss_ptr()->hybr_1_srv_status == SYS_SRV_STATUS_LIMITED_REGIONAL
                 )
               )
        {
          call_ptr->end_status = CM_CALL_END_NO_FULL_SRV;
          cmcall_orig_proc( call_ptr );
        }
        #ifdef FEATURE_HDR_HYBRID
#error code not present
        #endif

        call_ptr = cmcall_obj_get_next(&call_itr);
      } /* while */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SRV_IND_INFO_F:

    if( sd_rpt_ptr->srv_ind_info.ss == SD_SS_HYBR_WLAN &&
        sd_rpt_ptr->srv_ind_info.si_info.srv_status != SYS_SRV_STATUS_SRV )
    {
      cmcall_obj_iterator_init(&call_itr);

      call_ptr = cmcall_obj_get_next(&call_itr);

      while(call_ptr != NULL)
      {
        if(call_ptr->ss == CM_SS_WLAN &&
           call_ptr->call_type == CM_CALL_TYPE_PS_DATA)
        {
          cmcall_end(call_ptr);
        }

        call_ptr = cmcall_obj_get_next(&call_itr);

      }
    }
    break;

    default:
      call_ptr = NULL;
      break;

  } /* switch( cm_mc_rpt_ptr->hdr.cmd ) */

} /* cmcall_sd_rpt_proc() */

/*===========================================================================
FUNCTION cmcall_remove_mode_pref_components

DESCRIPTION

  Remove given SD mode preference component(s) from given CM mode preference.

  Example:

  The below will remove all single mode pref components from
  CM_MODE_PREF_AUTOMATIC and end up with CM_MODE_PREF_NONE.

  cm_mode_pref = cmcall_remove_mode_pref_components(
                                           CM_MODE_PREF_AUTOMATIC,
                                           6,// because 6 components follow
                                           SD_SS_MODE_PREF_WCDMA,
                                           SD_SS_MODE_PREF_GSM,
                                           SD_SS_MODE_PREF_GPS,
                                           SD_SS_MODE_PREF_HDR,
                                           SD_SS_MODE_PREF_CDMA,
                                           SD_SS_MODE_PREF_AMPS );

DEPENDENCIES
  sd_misc_remove_mode_pref()

RETURN VALUE
  Return new preference or CM_MODE_PREF_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
cm_mode_pref_e_type cmcall_remove_mode_pref_components(cm_mode_pref_e_type cm_mode_pref,
                                                        int number_of_components_to_remove,
                                                        ... )
{

  va_list arg_list;
  sd_ss_mode_pref_e_type sd_mode_pref;
  sd_ss_mode_pref_e_type sd_mode_pref_to_remove = SD_SS_MODE_PREF_NONE;

  /*
  ** First map given CM mode pref to SD mode pref
  */
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(cm_mode_pref);
  if( sd_mode_pref == SD_SS_MODE_PREF_NONE) {
    CM_ERR( "Mapping #1 failed !!!",0,0,0 );
    return CM_MODE_PREF_NONE;
  }

  /*
  ** Initialize arg_list and make an internal pointer point
  ** at the first variable function argument.
  ** This will be the second parameter
  */
  /*lint -e{64} -e{530} -e{516} */
  va_start(arg_list, number_of_components_to_remove);

  /*
  ** Process second and subsequent function arguments.
  ** These are the SD components to be removed from given
  ** CM mode pref
  */

  while(number_of_components_to_remove--)
  {

    /* Point at next function argument */
    /*lint -e{10} -e{662} -e{826} */
    sd_mode_pref_to_remove = (sd_ss_mode_pref_e_type)va_arg(arg_list, int);

    if(!BETWEEN( sd_mode_pref_to_remove,
                 SD_SS_MODE_PREF_NONE,
                 SD_SS_MODE_PREF_MAX))
    {
      CM_ERR( "Bad sd_mode_pref = %d",sd_mode_pref_to_remove,0,0 );
      CM_ASSERT(FALSE);
    }

    /* Remove it */
    sd_mode_pref = sd_misc_remove_mode_pref( sd_mode_pref,
                                             sd_mode_pref_to_remove );
    /*
    ** Why INRANGE - if we start from CM_MODE_PREF_AUTOMATIC
    ** and want to remove all SD components one by one,
    ** from SD_SS_MODE_PREF_WCDMA all the way through SD_SS_MODE_PREF_AMPS,
    ** we'll end up with SD_SS_MODE_PREF_NONE and that's fine
    */
    if(!INRANGE( sd_mode_pref,
                 SD_SS_MODE_PREF_NONE,
                 SD_SS_MODE_PREF_MAX))
    {
      CM_ERR( "Bad sd_mode_pref = %d",sd_mode_pref,0,0 );
      CM_ASSERT(FALSE);
    }
  }

  /* Clean up */
  va_end(arg_list);

  /* Now map result back to CM mode pref enum value */
  cm_mode_pref = cmph_map_sd_mode_pref_to_cm_mode_pref(sd_mode_pref);
  if(cm_mode_pref == CM_MODE_PREF_MAX) {
    CM_ERR( "Mapping #2 failed !!!",0,0,0 );
  }

  return cm_mode_pref;
}

/*===========================================================================
FUNCTION cmcall_add_mode_pref_components

DESCRIPTION

  Add given SD mode preference component(s) from given CM mode preference.

  Example:

  The below will add all single mode pref components from
  CM_MODE_PREF_AUTOMATIC and end up with CM_MODE_PREF_NONE.

  cm_mode_pref = cmcall_add_mode_pref_components(
                                           CM_MODE_PREF_AUTOMATIC,
                                           6,// because 6 components follow
                                           SD_SS_MODE_PREF_WCDMA,
                                           SD_SS_MODE_PREF_GSM,
                                           SD_SS_MODE_PREF_GPS,
                                           SD_SS_MODE_PREF_HDR,
                                           SD_SS_MODE_PREF_CDMA,
                                           SD_SS_MODE_PREF_AMPS );

DEPENDENCIES
  sd_misc_add_mode_pref()

RETURN VALUE
  Return new preference or CM_MODE_PREF_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
cm_mode_pref_e_type cmcall_add_mode_pref_components(cm_mode_pref_e_type cm_mode_pref,
                                                        int number_of_components_to_add,
                                                        ... )
{
  va_list arg_list;
  sd_ss_mode_pref_e_type sd_mode_pref;
  sd_ss_mode_pref_e_type sd_mode_pref_to_add = SD_SS_MODE_PREF_NONE;

  /*
  ** First map given CM mode pref to SD mode pref
  */
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(cm_mode_pref);
  if( sd_mode_pref == SD_SS_MODE_PREF_NONE) {
    CM_ERR( "Mapping #1 failed !!!",0,0,0 );
    return CM_MODE_PREF_NONE;
  }


  /*
  ** Initialize arg_list and make an internal pointer point
  ** at the first variable function argument.
  ** This will be the second parameter
  */
  /*lint -e{64} -e{530} -e{516} */
  va_start(arg_list, number_of_components_to_add);

  /*
  ** Process second and subsequent function arguments.
  ** These are the SD components to be removed from given
  ** CM mode pref
  */

  while(number_of_components_to_add--)
  {

    /* Point at next function argument */
    /*lint -e{10} -e{662} -e{826} */
    sd_mode_pref_to_add = (sd_ss_mode_pref_e_type)va_arg(arg_list, int);

    if(!BETWEEN( sd_mode_pref_to_add,
                 SD_SS_MODE_PREF_NONE,
                 SD_SS_MODE_PREF_MAX))
    {
      CM_ERR( "Bad sd_mode_pref = %d",sd_mode_pref_to_add,0,0 );
      CM_ASSERT(FALSE);
    }

    /* Remove it */
    sd_mode_pref = sd_misc_add_mode_pref( sd_mode_pref,
                                             sd_mode_pref_to_add );
    /*
    ** Why INRANGE - if we start from CM_MODE_PREF_AUTOMATIC
    ** and want to remove all SD components one by one,
    ** from SD_SS_MODE_PREF_WCDMA all the way through SD_SS_MODE_PREF_AMPS,
    ** we'll end up with SD_SS_MODE_PREF_NONE and that's fine
    */
    if(!INRANGE( sd_mode_pref,
                 SD_SS_MODE_PREF_NONE,
                 SD_SS_MODE_PREF_MAX))
    {
      CM_ERR( "Bad sd_mode_pref = %d",sd_mode_pref,0,0 );
      CM_ASSERT(FALSE);
    }
  }

  /* Clean up */
  va_end(arg_list);

  /* Now map result back to CM mode pref enum value */
  cm_mode_pref = cmph_map_sd_mode_pref_to_cm_mode_pref(sd_mode_pref);
  if(cm_mode_pref == CM_MODE_PREF_MAX) {
    CM_ERR( "Mapping #2 failed !!!",0,0,0 );
  }

  return cm_mode_pref;

}

/*===========================================================================

FUNCTION cmcall_set_favored_mode_pref_for_origination

DESCRIPTION

  Given call type, srv_type, DRS bit, compute favored mode preference and
  favored hybrid preference.

DEPENDENCIES

  None

RETURN VALUE

  TRUE if favored mode and hybrid preference were computed successfully,
  in which case they are written into fav_mode_pref_ptr and
  fav_hybr_pref_ptr, FALSE otherwise.

  If the function retured FALSE then CM wrote the values of CM_HYBR_PREF_NONE
  and CM_MODE_PREF_NONE into fav_hybr_pref_ptr and fav_mode_pref_ptr

SIDE EFFECTS

  None

===========================================================================*/
boolean  cmcall_set_favored_mode_pref_for_origination(

                                cm_call_type_e_type    call_type,
                                cm_srv_type_e_type     srv_type,
                                boolean                drs_bit,
                                word                   srv_opt,
                                /* In */

                                cm_mode_pref_e_type   *fav_mode_pref_ptr,
                                cm_hybr_pref_e_type   *fav_hybr_pref_ptr
                                /* Out */

)
/*lint -esym(715, drs_bit)*/
/*lint -esym(715, srv_opt)*/
{

  /* Table entry type */
  typedef struct {

    /* In */
    cm_call_type_e_type    call_type;
    cm_srv_type_e_type     srv_type;
    cm_mode_pref_e_type    mode_pref;

    /* Out */
    cm_mode_pref_e_type    favored_mode_pref;
    cm_hybr_pref_e_type    favored_hybr_pref;

  } cm_fav_mode_pref_table_entry;

  int i;

  const cmph_s_type   *ph_ptr = cmph_ptr();
    /* point at phone object */

  const cm_fav_mode_pref_table_entry *tep;
    /* point at table entry */

  boolean found_and_set;
    /* whether we found a match in the table and set favored_mode_pref
       and favored_hybr_pref in cmd_info_ptr */

  /*

    Conventions used in the table:

    - special cases are pulled out of the table so they are processed first,
      without the need of having to search the entire table
    - *_NONE means "Don't care" (wildcard), we don't look at it while searching the table
    - *_NO_CHANGE is mapped to current, it means "Don't change anything"
    - CM_MODE_PREF_CURRENT_LESS_HDR means current mode preference with HDR component removed

   */
  static const cm_fav_mode_pref_table_entry mode_pref_for_orig_table[] =
  {

    /* Special call types, where we know instantly what favored mode and hybrid preference for origination should be */
    #ifdef FEATURE_JCDMA
    { CM_CALL_TYPE_EMERGENCY,     CM_SRV_TYPE_NONE,           CM_MODE_PREF_NONE,           CM_MODE_PREF_EMERGENCY,                 CM_HYBR_PREF_NO_CHANGE },
    #else
    { CM_CALL_TYPE_EMERGENCY,     CM_SRV_TYPE_NONE,           CM_MODE_PREF_NONE,           CM_MODE_PREF_EMERGENCY,                 CM_HYBR_PREF_OFF       },
    #endif
    { CM_CALL_TYPE_STD_OTASP,     CM_SRV_TYPE_NONE,           CM_MODE_PREF_NONE,           CM_MODE_PREF_CDMA_ONLY,                 CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NON_STD_OTASP, CM_SRV_TYPE_NONE,           CM_MODE_PREF_NONE,           CM_MODE_PREF_CDMA_ONLY,                 CM_HYBR_PREF_NO_CHANGE },

     /* DATA calls, With automatic service type */

    { CM_CALL_TYPE_PS_DATA,       CM_SRV_TYPE_AUTOMATIC,      CM_MODE_PREF_AMPS_ONLY,      CM_MODE_PREF_DIGITAL_ONLY,              CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_PS_DATA,       CM_SRV_TYPE_AUTOMATIC,      CM_MODE_PREF_NONE,           CM_MODE_PREF_CURRENT_LESS_AMPS,         CM_HYBR_PREF_NO_CHANGE },

     /* SMS calls, With automatic service type */
    { CM_CALL_TYPE_SMS,           CM_SRV_TYPE_AUTOMATIC,      CM_MODE_PREF_NONE,           CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY,     CM_HYBR_PREF_NO_CHANGE },

     /* Voice calls with automatic service type requested, last one is 'catch all' case for voice calls */
    { CM_CALL_TYPE_VOICE,         CM_SRV_TYPE_AUTOMATIC,      CM_MODE_PREF_HDR_ONLY,       CM_MODE_PREF_AUTOMATIC,                 CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_VOICE,         CM_SRV_TYPE_AUTOMATIC,      CM_MODE_PREF_NONE,           CM_MODE_PREF_NO_CHANGE,                 CM_HYBR_PREF_NO_CHANGE },

    /* Videoshare calls are allowed only on WDMA only */
    { CM_CALL_TYPE_VS,            CM_SRV_TYPE_AUTOMATIC,      CM_MODE_PREF_NONE,           CM_MODE_PREF_WCDMA_ONLY,                CM_HYBR_PREF_NO_CHANGE },

    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_AMPS,           CM_MODE_PREF_NONE,           CM_MODE_PREF_AMPS_ONLY,                 CM_HYBR_PREF_OFF       },

    /*-----paragk
    ** Commented the entries on 10/20/2005, the entries here are for reference, the INTERSECT_OR_FORCE entry takes
    ** care of all the special conditions
    */

    /*
    ** User being picky about service type required, so we don't care about call type
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_HDR,            CM_MODE_PREF_NONE,           CM_MODE_PREF_HDR_ONLY,                  CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_CDMA_SPECIFIC,  CM_MODE_PREF_NONE,           CM_MODE_PREF_CDMA_ONLY,                 CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_CDMA_AUTOMATIC, CM_MODE_PREF_NONE,           CM_MODE_PREF_CDMA_ONLY,                 CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_CDMA_HDR,       CM_MODE_PREF_CDMA_ONLY,      CM_MODE_PREF_CDMA_ONLY,                 CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_CDMA_HDR,       CM_MODE_PREF_CDMA_AMPS_ONLY, CM_MODE_PREF_CDMA_ONLY,                 CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_CDMA_HDR,       CM_MODE_PREF_HDR_ONLY,       CM_MODE_PREF_HDR_ONLY,                  CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_CDMA_HDR,       CM_MODE_PREF_NONE,           CM_MODE_PREF_CDMA_HDR_ONLY,             CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_WCDMA,          CM_MODE_PREF_NONE,           CM_MODE_PREF_WCDMA_ONLY,                CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_GSM_WCDMA,      CM_MODE_PREF_GSM_ONLY,       CM_MODE_PREF_GSM_ONLY,                  CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_GSM_WCDMA,      CM_MODE_PREF_WCDMA_ONLY,     CM_MODE_PREF_WCDMA_ONLY,                CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_GSM_WCDMA,      CM_MODE_PREF_NONE,           CM_MODE_PREF_GSM_WCDMA_ONLY,            CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_GSM,            CM_MODE_PREF_NONE,           CM_MODE_PREF_GSM_ONLY,                  CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_WLAN,           CM_MODE_PREF_NONE,           CM_MODE_PREF_WLAN_ONLY,                 CM_HYBR_PREF_NO_CHANGE },

    */

    /* We need to compute the intersection of the service requested and the current mode.
    ** If the intersection is not NONE then we need to use the intersection, otherwise we need to set the mode to what
    ** the user is requesting
    */

    /* Catch all when user specifies a particular service type */
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_NON_AUTOMATIC,  CM_MODE_PREF_NONE,           CM_MODE_PREF_INTERSECT_OR_FORCE,        CM_HYBR_PREF_NO_CHANGE },

    /* Catch all */
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_NONE,           CM_MODE_PREF_NONE,           CM_MODE_PREF_NO_CHANGE,                 CM_HYBR_PREF_NO_CHANGE }
  };

  /*-------------------------------------------------------------------------*/

  CM_ASSERT(fav_mode_pref_ptr != NULL);
  CM_ASSERT(fav_hybr_pref_ptr != NULL);

  /*-------------------------------------------------------------------------*/

  /* For PS data calls, check if the mode requested is a sub-set of the
  ** current mode, if it is then set the mode to the subset
  */

  /* Find match in the table */
  found_and_set = FALSE;
  for(i = 0; i < (int) (ARR_SIZE(mode_pref_for_orig_table)); i++)
  {

    /* Point at table entry */
    tep = &mode_pref_for_orig_table[i];

    /* Match against wildcard or specific value */
    if( ((tep->call_type == CM_CALL_TYPE_NONE) || (tep->call_type  == call_type)) &&
        ((tep->srv_type  == CM_SRV_TYPE_NONE)  || (tep->srv_type   == srv_type)
         ||((tep->srv_type == CM_SRV_TYPE_NON_AUTOMATIC) && (srv_type != CM_SRV_TYPE_AUTOMATIC))) &&
        ((tep->mode_pref == CM_MODE_PREF_NONE) || (tep->mode_pref  == ph_ptr->mode_pref)))
    {

      if(tep->favored_mode_pref == CM_MODE_PREF_INTERSECT_OR_FORCE )
      {
        /* Compute the intersection of the current mode and the service type
        ** requested and if the intersection is not NULL then use the intersection.
        ** If the intersection is NULL, then use the type requested by the client
        */
        cm_mode_pref_e_type cm_mode_pref;

        cm_mode_pref = cmcall_misc_intersect_or_force_mode_pref( srv_type, ph_ptr->mode_pref );

        if(cm_mode_pref == CM_MODE_PREF_NONE)
        {

          /*
          ** Quit if we failed
          */
          found_and_set = FALSE;
          break;
        }
        else
        {
          *fav_mode_pref_ptr = cm_mode_pref;
        }

      }
      /* Found a match, so set appropriate fields in the command and quit the loop */
      else if(tep->favored_mode_pref == CM_MODE_PREF_CURRENT_PLUS_WLAN)
      {

        /* Process CM_MODE_PREF_CURRENT_PLUS_WLAN here, compute favored_mode_pref
           by adding WLAN component in current mode pref */
        cm_mode_pref_e_type cm_mode_pref;
        cm_mode_pref = cm_add_mode_pref_components( ph_ptr->mode_pref,
                                                            1,
                                                            SD_SS_MODE_PREF_WLAN );
        if(cm_mode_pref == CM_MODE_PREF_NONE)
        {

          /*
          ** Quit if we failed
          */
          found_and_set = FALSE;
          break;
        }
        else
        {
          *fav_mode_pref_ptr = cm_mode_pref;
        }
      }
      else if(tep->favored_mode_pref == CM_MODE_PREF_CURRENT_LESS_HDR)
      {

        /* Process CM_MODE_PREF_CURRENT_LESS_HDR here, compute favored_mode_pref
        ** by removing HDR component from current mode pref
        */
        cm_mode_pref_e_type cm_mode_pref;
        cm_mode_pref = cm_remove_mode_pref_components( ph_ptr->mode_pref,
                                                            1,
                                                            SD_SS_MODE_PREF_HDR );
        if(cm_mode_pref == CM_MODE_PREF_NONE)
        {

          /*
          ** Quit if we failed
          */
          found_and_set = FALSE;
          break;
        }
        else
        {
          *fav_mode_pref_ptr = cm_mode_pref;
        }
      }
      else if(tep->favored_mode_pref == CM_MODE_PREF_CURRENT_LESS_AMPS)
      {

        /*
        ** Process CM_MODE_PREF_CURRENT_LESS_AMPS here.
        ** Compute favored_mode_pref by removing AMPS component
        ** from current mode pref
        */
        cm_mode_pref_e_type cm_mode_pref;
        cm_mode_pref = cm_remove_mode_pref_components( ph_ptr->mode_pref,
                                                           1,
                                                           SD_SS_MODE_PREF_AMPS );

        if(cm_mode_pref == CM_MODE_PREF_NONE ||
          cm_mode_pref == CM_MODE_PREF_MAX )
        {

          /* Quit if we failed */
          found_and_set = FALSE;
          break;
        }
        else
        {
          *fav_mode_pref_ptr = cm_mode_pref;
        }

      }
      else if(tep->favored_mode_pref == CM_MODE_PREF_CURRENT_LESS_HDR_AND_AMPS)
      {

        /* Process CM_MODE_PREF_CURRENT_LESS_HDR_AND_AMPS here.
        ** Compute favored_mode_pref by removing HDR and AMPS components
        ** from current mode pref
        */
        cm_mode_pref_e_type cm_mode_pref;
        cm_mode_pref = cm_remove_mode_pref_components( ph_ptr->mode_pref,
                                                           2,
                                                           SD_SS_MODE_PREF_HDR,
                                                           SD_SS_MODE_PREF_AMPS );

        if(cm_mode_pref == CM_MODE_PREF_NONE ||
           cm_mode_pref == CM_MODE_PREF_MAX )
        {

          /* Quit if we failed */
          found_and_set = FALSE;
          break;
        }
        else
        {
          *fav_mode_pref_ptr = cm_mode_pref;
        }

      } else if(tep->favored_mode_pref == CM_MODE_PREF_NO_CHANGE)
      {

          /* _NO_CHANGE means "use current".
          ** Gotta set it to current so it can be correctly mapped to SD enum later
          */
        *fav_mode_pref_ptr = ph_ptr->mode_pref;
      }
      else
      {

        /* Copy any other value directly */
        *fav_mode_pref_ptr = tep->favored_mode_pref;

      }

      /* _NO_CHANGE means "use current".
      ** Gotta set it to current so it can be correctly mapped to SD enum later
      */
      if(tep->favored_hybr_pref == CM_HYBR_PREF_NO_CHANGE)
      {
        *fav_hybr_pref_ptr = ph_ptr->hybr_pref;
      }
      else
      {
        *fav_hybr_pref_ptr = tep->favored_hybr_pref;
      }

      /* Indicate that we found and processed it */
      found_and_set = TRUE;
      break;
    }
  }


  /* If match not found by the code above, flag an error and return */
  if(!found_and_set)
  {

    *fav_hybr_pref_ptr = CM_HYBR_PREF_NONE;
    *fav_mode_pref_ptr = CM_MODE_PREF_NONE;

    CM_ERR("No match !!!",0,0,0);
    return found_and_set;

  }

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif

  #ifdef CM_DEBUG
#error code not present
  #endif

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  /* Check if favored mode_pref has to be updated for QNC call.
  */
  if ( call_type == CM_CALL_TYPE_PS_DATA &&
       srv_opt == CAI_SO_ASYNC_DATA_13K_PRE707
     )
  {
    *fav_mode_pref_ptr = CM_MODE_PREF_CDMA_ONLY;
  }
  #endif

  return found_and_set;

}
/*lint +esym(715,drs_bit)*/
/*lint +esym(715, srv_opt) */


/*===========================================================================

FUNCTION cmcall_determine_mode_usage

DESCRIPTION
  This function given mode_pref, info_present in the command, target_capability,
  and srv_type will determine which mode is possible for origination.

DEPENDENCIES
  none

RETURN VALUE
  mode usage

SIDE EFFECTS
  none

===========================================================================*/
static cm_cc_mode_pref_e_type cmcall_determine_mode_usage
(
  cm_cmd_s_type  *cmd_ptr                       /* pointer to a CM command */
)
{

  cm_call_cmd_s_type           *call_cmd_ptr      = NULL;
  cm_call_cmd_info_s_type      *cmd_info_ptr      = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_cmd_ptr      = CALL_CMD_PTR( cmd_ptr );
  cmd_info_ptr      = CMD_INFO_PTR( call_cmd_ptr );

  /* First, check on the information given by the client.  If client only
  ** gives CM information for one mode, than we use that mode and there is no
  ** need to check any conditions further.
  */
  if ((cmd_info_ptr->is_gsm_wcdma_ps_info_present ||
       cmd_info_ptr->is_gsm_wcdma_cs_info_present)
       && (!cmd_info_ptr->is_cdma_info_present)
     )
  {
    return CM_CC_MODE_PREF_GW_ONLY;
  }

  if ( cmd_info_ptr->is_cdma_info_present
    && !cmd_info_ptr->is_gsm_wcdma_ps_info_present
    && !cmd_info_ptr->is_gsm_wcdma_cs_info_present
    )
  {
    return CM_CC_MODE_PREF_CDMA_ONLY;
  }

  /*-----------------------------------------------------------------------*/
  /* Second, the client sent CM both sets of parameters.
  ** Check the srv_type, and if that limits it to one mode than there is no
  ** need to check any conditions further.
  */

  /* Make sure srv_type is valid first
  */
  if (!BETWEEN(cmd_info_ptr->srv_type, CM_SRV_TYPE_NONE, CM_SRV_TYPE_MAX))
  {
    MSG_ERROR ("Invalid srv type in originaion %d", cmd_info_ptr->srv_type, 0, 0);
    return CM_CC_MODE_PREF_NONE;
  }

  switch (cmd_info_ptr->srv_type)
  {
     case CM_SRV_TYPE_AMPS:
     case CM_SRV_TYPE_HDR:
     case CM_SRV_TYPE_CDMA_SPECIFIC:
     case CM_SRV_TYPE_CDMA_AUTOMATIC:
     case CM_SRV_TYPE_CDMA_HDR:
       return CM_CC_MODE_PREF_CDMA_ONLY;

     case CM_SRV_TYPE_GSM_WCDMA:
       return CM_CC_MODE_PREF_GW_ONLY;

     case CM_SRV_TYPE_AUTOMATIC:
     default:
        break;
  }

  /*-----------------------------------------------------------------------*/
  /* Third, the srv_type is automatic, so lets check favored mode pref to see
  ** if we can can limit it to one mode.  If so, than there is no need to
  ** check any more conditions.
  */

  /* To do this, we need to resolve the call type first
  ** We don't want to actually change anything in the call object,
  ** but we need to figure out whether this is a special emergency or otasp call.
  */

  switch (cmd_info_ptr->favored_mode_pref)
  {
    case CM_MODE_PREF_NONE:
      /* Reject the call
      */
      return CM_CC_MODE_PREF_NONE;

    case CM_MODE_PREF_AMPS_ONLY:
    case CM_MODE_PREF_CDMA_ONLY:
    case CM_MODE_PREF_HDR_ONLY:
    case CM_MODE_PREF_CDMA_AMPS_ONLY:
      return CM_CC_MODE_PREF_CDMA_ONLY;

    case CM_MODE_PREF_GSM_ONLY:
    case CM_MODE_PREF_WCDMA_ONLY:
      return CM_CC_MODE_PREF_GW_ONLY;

    case CM_MODE_PREF_EMERGENCY:
    case CM_MODE_PREF_GPS_ONLY:
    case CM_MODE_PREF_DIGITAL_ONLY:
    case CM_MODE_PREF_AUTOMATIC:
    default:
     break;
  }

  /*-----------------------------------------------------------------------*/
  /* Fourth, the target capability is checked.
  ** If we still can not narrow it down
  ** to one mode, than it will be automatic.
  */

  /* Check whether GSM/WCDMA is supported
  */
  if (!sd_misc_is_supp_pref (SD_SS_MODE_PREF_GW, SD_SS_BAND_PREF_ANY))
  {
    return CM_CC_MODE_PREF_CDMA_ONLY;
  }

  if (!sd_misc_is_supp_pref (SD_SS_MODE_PREF_CDMA_AMPS, SD_SS_BAND_PREF_ANY))
  {
    return CM_CC_MODE_PREF_GW_ONLY;
  }

  return CM_CC_MODE_PREF_ANY;

} /* cmcall_determine_mode_usage */

#if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
#endif  /* FEATURE_GSM || FEATURE_WCDMA */


/*===========================================================================

FUNCTION cmcall_client_orig_proc

DESCRIPTION

  This function handles the origination command.
  This function first determines what mode is possible to originate the call on.
  Then depending on the possible modes, call the appropriate 1X and/or GW calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_client_orig_proc
(
  cm_cmd_s_type  *cmd_ptr                       /* pointer to a CM command */
)
{
  cm_call_cmd_s_type           *call_cmd_ptr   = NULL;
  cm_call_cmd_info_s_type      *cmd_info_ptr   = NULL;
  cmss_s_type                  *ss_ptr         = cmss_ptr();

  cm_call_cmd_err_e_type        cmd_err        = CM_CALL_CMD_ERR_NOERR;

  cmcall_s_type                *call_ptr       = NULL;
  cm_cc_mode_pref_e_type        mode_usage     = CM_CC_MODE_PREF_NONE;
  cmph_s_type                  *ph_ptr         = cmph_ptr();
  cm_cc_subscription_source_e_type cdma_subscription_source;
  cm_cc_subscription_source_e_type gw_subscription_source;

  boolean                       cm_nc_status = TRUE;
  word                          srv_opt = 0;

  cm_gw_ps_cc_params_s_type     ps_cc_params;
    /* Carries pdp activation in L3 format */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr != NULL);
  CM_FUNC_START( "cmcall_client_orig_proc()",0,0,0 );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_cmd_ptr         = CALL_CMD_PTR( cmd_ptr );
  cmd_info_ptr         = CMD_INFO_PTR( call_cmd_ptr );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Clearing PDP activation params for CC
  */
  memset (&ps_cc_params, 0, sizeof(ps_cc_params));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** Get a pointer to call Object.
  */
  call_ptr = cmcall_ptr(cmd_info_ptr->call_id);
  if (call_ptr == NULL)
  {
    CM_MSG_HIGH("ORIG: Can not retrieve call pointer for call id = %d",
       cmd_info_ptr->call_id, 0, 0);
    cmd_err = CM_CALL_CMD_ERR_NO_CALL_ID_S;
    cmcall_client_cmd_err_deallocate (cmd_ptr, cmd_err);
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef FEATURE_IMS_VCC
  /**
  **  Update the switch out & switch in call ids in the homer data
  **  structure
  */
  if( cmd_info_ptr->switch_out_call_id != CM_CALL_ID_INVALID )
  {
    homer_update_handoff_info(cmd_info_ptr->switch_out_call_id,
                              cmd_info_ptr->call_id);
  }
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set the call type initially.
  */
  cmcall_set_type(call_ptr,cmd_info_ptr->call_type);

  /* This check will eliminate the need for call control for non-CDMA
  ** voice calls, this check needs to be repeated for CDMA and automatic
  ** calls after call control returnns
  */
  if( ( ph_ptr->disable_call_type_mask & CM_CALL_TYPE_VOICE_MASK ) &&
      ( cmd_info_ptr->srv_type  == CM_SRV_TYPE_WCDMA  ||
        cmd_info_ptr->srv_type  == CM_SRV_TYPE_GSM    ||
        cmd_info_ptr->srv_type  == CM_SRV_TYPE_GSM_WCDMA ) &&
      ( cmd_info_ptr->call_type == CM_CALL_TYPE_VOICE ||
        cmd_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
    )
  {
    /* Block this MO voice call */
      cmd_err =  CM_CALL_CMD_ERR_SRV_TYPE_S;
      cmcall_client_cmd_err_deallocate (cmd_ptr, cmd_err);
      return;
  }

  /*-----------------------------------------------------------------------*/

  /*
  ** This function will be called on two occassions.....  When there is an
  ** origination and again when call control is complete.  First, we check
  ** which case it is .....
  ** If this is the first time CM has seen this origination, it will call
  ** cm_nc_call_control_processing and then exit this function (deallocating
  ** the cmd_ptr as well)....  Once call control is complete, the client will
  ** call a CM callback function which will requeue another origination
  ** (CM_CALL_CMD_CC_COMPLETE_REORIG).  Then CM will pick up and continue
  ** with the origination process.
  */


  CM_FUNC_START( "cmcall_client_orig_proc()",0,0,0 );

  /* Check if call origination parameters (GW PS, GW CS, CDMA) have
  ** been supplied by the client. If origination parameters are missing
  ** intialize the fields with default values
  */
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

  if (cmd_info_ptr->is_cdma_info_present == FALSE)
  {
    /* Init cs call origination parameters */
    cmcall_cmd_info_cdma_init (cmd_ptr);
  }

  srv_opt = cmd_info_ptr->cdma_info.srv_opt;

  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */


  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */


  /* First determine which modes we can originate on */

  /* This function will write CM_MODE_PREF_NONE into favored_mode_pref
     if it was not able to compute it */
  if (!cmcall_set_favored_mode_pref_for_origination ( cmd_info_ptr->call_type,
                                                      cmd_info_ptr->srv_type,
                                                      #ifdef FEATURE_IS2000
                                                      cmd_info_ptr->cdma_info.drs_bit,
                                                      #else
                                                      TRUE,
                                                      #endif
                                                      srv_opt,
                                                      &cmd_info_ptr->favored_mode_pref,
                                                      &cmd_info_ptr->favored_hybr_pref ))
  {
    cmd_err = CM_CALL_CMD_ERR_FAVORED_MODE_PREF_FOR_ORIGINATION;
    cmcall_client_cmd_err_deallocate (cmd_ptr, cmd_err);
    return;
  }


  CM_MSG_HIGH("Fav mode pref as computed:",0,0,0);
  cmdbg_print_fav_mode_pref_for_origination( cmd_info_ptr->call_type,
                                             cmd_info_ptr->srv_type,
                                             cmph_ptr()->mode_pref,
                                             cmd_info_ptr->favored_mode_pref,
                                             cmd_info_ptr->favored_hybr_pref );

  /*-----------------------------------------------------------------------*/

  /* Only able to originate if the phone/PRL supports the
  ** originating mode preference.
  */
  if( !cmph_is_valid_mode_pref(cmd_info_ptr->favored_mode_pref) )
  {
      cmd_err =  CM_CALL_CMD_ERR_SRV_TYPE_S;
      cmcall_client_cmd_err_deallocate (cmd_ptr, cmd_err);
      return;
  }

  /*-----------------------------------------------------------------------*/
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif

  /*-----------------------------------------------------------------------*/

  mode_usage = cmcall_determine_mode_usage (cmd_ptr);

  /*-----------------------------------------------------------------------*/

  if (cmd_ptr->cmd.call.cmd == CM_CALL_CMD_ORIG)
  {
    /* Save favored mode prefs.
    */
    call_ptr->favored_mode_pref = cmd_info_ptr->favored_mode_pref;
    call_ptr->favored_hybr_pref = cmd_info_ptr->favored_hybr_pref;

    /* Pulled out of cmwcall_client_cmd_proc(). trans_id and nsapi are tied
    ** to connection id and are needed by Call control.
    **
    ** Return cmd_err only if there are no CDMA origination parameters and
    ** connection id cannot be obtained.
    */
    if(!cmcall_generate_connection_id(call_ptr) &&
       !cmd_info_ptr->is_cdma_info_present
      )
    {
      CM_ERR("ORIG: Can not set connection Id Info",call_ptr->call_id, 0, 0);

      cmcall_client_cmd_err_deallocate (cmd_ptr, CM_CALL_CMD_ERR_CONNECTION_ID_OUT_L);
      return;
    }

    /*--------------------------------------------------------------------*/


    /* Check if we need to do any call control */
    if (!cmd_ptr->cmd.call.info.call_control_processing_complete)
    {

      /* if we can't do automatic mode, no use have call control do any
         processing it need not do.
      */
      if (mode_usage == CM_CC_MODE_PREF_CDMA_ONLY)
      {
        cdma_subscription_source = cmph_get_cdma_subscription_source ();
        gw_subscription_source = CM_CC_SUBSCRIPTION_SOURCE_NONE;
      }
      else if (mode_usage == CM_CC_MODE_PREF_GW_ONLY)
      {
        cdma_subscription_source = CM_CC_SUBSCRIPTION_SOURCE_NONE;
        gw_subscription_source = cmph_get_gw_subscription_source ();
      }
      else
      {
        cdma_subscription_source = cmph_get_cdma_subscription_source ();
        gw_subscription_source = cmph_get_gw_subscription_source ();
      }

      /* Fill CC params for PS call
      */
      cmcall_fill_ps_cc_params (cmd_ptr, &ps_cc_params);

      cm_nc_status = cm_nc_call_control_processing
      (
        cmd_info_ptr->call_id,
        cdma_subscription_source,
        gw_subscription_source,
        cmd_info_ptr->call_type,
        &call_cmd_ptr->info.num,
        (void*) cmd_ptr,
        cmcall_call_control_complete_cb,
        &ps_cc_params,
        &(cmd_info_ptr->cc_generic_params)
      );

      if (cm_nc_status == TRUE)
      {
        call_ptr->call_state = CM_CALL_STATE_CC_IN_PROGRESS;
        CM_MSG_HIGH("ORIG: Starting Call Control Processing .... ", 0, 0, 0);
      }
      else
      {
        CM_ERR("cm_nc_call_control_processing returns FALSE \n", 0, 0, 0);
        /*lint -save -e527 Unreachable
        **            When CM_DEBUG is defined, CM_MSG_ERROR evaluates to exit(1)
        */
        cmcall_end (call_ptr);

        /* Since Number classification returned false, it's upto CM to free
        ** the command buffer that was sent to it.
        */
        cmd_ptr->is_reused = FALSE;

        /*lint -restore */
      }
      return;
    }

    #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
    #endif
    call_ptr->cdma_cc_modified_info.call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;

  }
  else if (cmd_ptr->cmd.call.cmd == CM_CALL_CMD_CC_COMPLETE_REORIG)
  {
    /* Reset the cmd to be the original origination command.
    ** If an error should occur, CM should not send out the cmd as a REORIG.
    */
    cmd_ptr->cmd.call.cmd = CM_CALL_CMD_ORIG;


    /* If call is not allowed on either modes,
    ** Or if the call is not allowed on the one technology that is available for use.....
    ** then the call is effectively rejected by call control.
    */
    if (((call_ptr->cdma_cc_modified_info.call_control_result ==
                           CM_CC_RESULT_NOT_ALLOWED)
         #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
         #endif
        )
                                   ||
        ((call_ptr->cdma_cc_modified_info.call_control_result ==
                           CM_CC_RESULT_NOT_ALLOWED) &&
         (mode_usage == CM_CC_MODE_PREF_CDMA_ONLY))
        #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
        #endif
       )
    {
      cmd_info_ptr->call_control_cmd_err = CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED;
    }

    /* If the call is rejected by call control, no need to proceed,
    ** stop the origination right away and return an error.
    */
    if (cmd_info_ptr->call_control_cmd_err == CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED)
    {
      cmd_err = cmd_info_ptr->call_control_cmd_err;
      CM_MSG_HIGH("ORIG: CC Rejected %d", cmd_info_ptr->call_id, 0, 0);

      /* Make sure the connection ids are unreserved.
      ** Clean up gets done in cmcall_client_cmd_err_deallocate ().
      */

      cmcall_client_cmd_err_deallocate (cmd_ptr, cmd_err);
      return;
    }


    /* Call the client registered preferences function.
    */
    mode_usage = cm_nc_call_control_preferences
    (
      call_ptr->cdma_cc_modified_info.call_control_result == CM_CC_RESULT_NOT_ALLOWED ? NULL: &call_ptr->cdma_cc_modified_info,
      #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
      #else
      NULL,
      #endif
      &call_cmd_ptr->info.num,
      cmd_info_ptr->call_type,
      ss_ptr->info.sys_mode
    );

    if( mode_usage == CM_CC_MODE_PREF_NONE )
    {
      cmd_err =  CM_CALL_CMD_ERR_MODE_PREF_P;
      cmcall_client_cmd_err_deallocate (cmd_ptr, cmd_err);
      return;

    }

    #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
    #endif  /* FEATURE_WCDMA || FEATURE_GSM */

  }

  /* By this time CC is done and calltype (OTA/CDMA type) is resolved */

  if( ( ph_ptr->disable_call_type_mask & CM_CALL_TYPE_VOICE_MASK ) &&
      ( call_ptr->call_type == CM_CALL_TYPE_VOICE ||
        call_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
    )
  {
      /* Block this MO voice call */
      cmd_err =  CM_CALL_CMD_ERR_SRV_TYPE_S;
      cmcall_client_cmd_err_deallocate (cmd_ptr, cmd_err);
      return;
  }
   /*-----------------------------------------------------------------------*/

  /* Reset the call state */
  call_ptr->call_state      = CM_CALL_STATE_IDLE;
  call_ptr->call_subst.idle = CM_CALL_IDLE_SUBST_NONE;
  call_ptr->call_type       = cmd_info_ptr->call_type;

  /*
  ** All MO voice calls go on currently selected line.
  ** For all other call types use the default line
  ** the call object was initialized with
  */
  if((call_ptr->call_type == CM_CALL_TYPE_VOICE) ||
      (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY))
  {
      call_ptr->line = cmph_ptr()->line;
  }

  /* If the call type is an emergency number,
  ** we should use any mode
  */
  if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
      mode_usage = CM_CC_MODE_PREF_ANY;
  }

  /* If the call type returned from call control is OTASP,
  ** then force the mode over to CDMA only.
  */
  if((call_ptr->call_type == CM_CALL_TYPE_NON_STD_OTASP) ||
      (call_ptr->call_type == CM_CALL_TYPE_STD_OTASP))
  {
    mode_usage = CM_CC_MODE_PREF_CDMA_ONLY;
  }

  /*-----------------------------------------------------------------------*/

  /* Compute ss for origination.
  */
  call_ptr->ss = cmcall_determine_ss_for_origination( cmd_info_ptr->favored_mode_pref,
                                                      cmd_info_ptr->favored_hybr_pref,
                                                       mode_usage,
                                                       call_ptr
                                                    );


  /*-----------------------------------------------------------------------*/

  if (call_ptr->ss == CM_SS_NONE)
  {
    cmd_err = CM_CALL_CMD_ERR_SRV_STATE_S;
    CM_MSG_HIGH ("Call type %d cannot be placed, err %d",
                  call_ptr->call_type, cmd_err, 0);
    cmcall_client_cmd_err_deallocate (cmd_ptr, cmd_err);
    return;
  }

  /*-----------------------------------------------------------------------*/


  cmd_err = cmcall_cmd_orig_para_check(call_cmd_ptr);

  /*-----------------------------------------------------------------------*/

  if (cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    CM_MSG_HIGH("call orig cmd err = %d",cmd_err,0,0);
    cmcall_client_cmd_err_deallocate (cmd_ptr, cmd_err);
    return;
  }

  /*-----------------------------------------------------------------------*/

  /* Copy cmd into call object.
  */

  cmcall_cmd_copy_fields( call_ptr, call_cmd_ptr );

  /*-----------------------------------------------------------------------*/

  /* Copy the current serving system status
  */

  cmcall_copy_ss_info( call_ptr );

  /*-----------------------------------------------------------------------*/

  /*
  ** Changes have to be made to favored_mode_pref and force_top_pref
  ** depending on if can be made over IP.
  */
  cmcall_process_ipcall_params (call_ptr);


  /*-----------------------------------------------------------------------*/

  if(call_ptr->ss != CM_SS_WLAN)
  {
    cmd_err = cmcall_process_orig_mode(mode_usage,call_ptr);
  }
  else
  {
    /* These two variables are set inside cmcall_process_orig_mode(), so set
    ** them here if we are not calling it.
    */
    call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
    call_ptr->main_ss_mode = mode_usage;
  }

  /*-----------------------------------------------------------------------*/


  if (cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    cmcall_client_cmd_err_deallocate (cmd_ptr, cmd_err);
  }
  else
  {
    /* No errors, we can try to originate the call now */
    /* Always notify client of command error code
    ** (even if command is successful).
    */
    cmcall_client_cmd_err( cmd_ptr, cmd_err );
    cmcall_orig_start (call_ptr);
  }

}  /* cmcall_client_orig_proc () */

/*===========================================================================

FUNCTION  cmcall_client_cmd_check

DESCRIPTION

  Process commands that are generic, ie. processed in the same way no matter
  whether it's a GW or 1X call.

  Check for call command parameter errors and whether a specified call
  command is allowed in the current state of the call/phone.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if command is allowed in the current state
  of the call/phone, specific cm_call_cmd_err_e_type otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static cm_call_cmd_err_e_type  cmcall_client_cmd_check(

    cm_cmd_s_type          *cmd_ptr
        /* Pointer to a CM command */
)
{
  cm_call_cmd_s_type          *call_cmd_ptr  = NULL;
    /* Point at call command component */

  cm_call_cmd_err_e_type       cmd_err       = CM_CALL_CMD_ERR_NOERR;
    /* Command error (if any) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr           != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_CALL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_cmd_ptr  = CALL_CMD_PTR( cmd_ptr );

  /* Check whether command is possible.
  */
  switch( call_cmd_ptr->cmd )
  {

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*------------------------------------*/
                     /* Get a snapshot of call information */
                     /*------------------------------------*/

    case CM_CALL_CMD_INFO_GET:

      /* Check command parameter errors
    */
      cmd_err = cmcall_call_cmd_get_call_info_para_check(call_cmd_ptr);
      break;

    default:

      cmd_err = CM_CALL_CMD_ERR_OTHER;
      CM_SWITCH_ERR( "Bad call command %d", call_cmd_ptr->cmd, 0, 0);
      break;

  }

  return cmd_err;
} /* cmcall_client_cmd_check() */

/*===========================================================================

FUNCTION cmcall_client_cmd_forward_ll

DESCRIPTION

  Process commands that are generic, ie. processed in the same way no matter
  whether it's a GW or 1X call.

  Forward a client call commands to lower layer.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if CM needs to wait for a replay from MC before continuing
  processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_client_cmd_forward_ll(

    const cm_cmd_s_type    *cmd_ptr
      /* Pointer to a CM command */
)
{
  const cm_call_cmd_s_type      *call_cmd_ptr = CALL_CMD_PTR( cmd_ptr );
    /* Point at call command component */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_CALL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_cmd_ptr = CALL_CMD_PTR( cmd_ptr );

  switch( call_cmd_ptr->cmd )
  {

                     /*------------------------------------*/
                     /* Get a snapshot of call information */
                     /*------------------------------------*/

    case CM_CALL_CMD_INFO_GET:

      /* Do nothing - this request terminated at CM
      */
      break;

    default:
      CM_SWITCH_ERR( "Bad call cmd = %d",call_cmd_ptr->cmd,0,0);
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the caller of this function to whether
  ** the call object is now waiting for reply from MC.
  */
  return FALSE;

} /* cmcall_client_cmd_forward_ll() */


/*===========================================================================

FUNCTION cmcall_client_cmd_ll_reply

DESCRIPTION
  Process commands that are generic, ie. processed in the same way no matter
  whether it's a GW or 1X call.

  Process Lower Layer reply to clients call commands.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_client_cmd_ll_reply(

    const cm_cmd_s_type       *cmd_ptr,
        /* Pointer to a CM command */

    cm_call_cmd_err_e_type     cmd_err
        /* Indicate call command error */
)
{
  cmcall_s_type               *call_ptr      = NULL;
    /* Pointer to a call object */

  cm_mm_call_info_s_type      *call_info_ptr = NULL;
    /* Call information pointer */

  const cm_call_cmd_s_type    *call_cmd_ptr  = NULL;
    /* Point at call command component */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( BETWEEN( cmd_err, CM_CALL_CMD_ERR_NONE, CM_CALL_CMD_ERR_MAX));
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_CALL );

  SYS_ARG_NOT_USED(cmd_err);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_cmd_ptr  = CALL_CMD_PTR( cmd_ptr );
  CM_FUNC_START( "cmcall_client_cmd_ll_reply, cmd=%d, err=%d",
                  call_cmd_ptr->cmd, 0, 0);

  /* If necessary, copy specific command fields into call struct,
  ** update call state, and notify clients of event.
  */
  switch( call_cmd_ptr->cmd )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*-----------------------------------------*/
                     /* Send call info to the requesting client */
                     /*-----------------------------------------*/

    case CM_CALL_CMD_INFO_GET:

      /*
      ** No need to copy any fields from command to call object
      ** Copy call state information into allocated call info buffer.
      */
      call_ptr = cmcall_ptr( call_cmd_ptr->info.call_id );

      if(call_ptr == NULL)
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
        break;
      }

      /* Allocate and Initialize the info */
      call_info_ptr = cmcall_info_alloc();
      (void) cmcall_info_get( call_ptr, call_info_ptr );

      /* Notify only the requesting client
      */
      cm_client_call_event_ntfy(call_cmd_ptr->client_ptr,
                                CM_CALL_EVENT_INFO,
                                call_info_ptr );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      default:
        CM_SWITCH_ERR( "Bad call command %d", call_cmd_ptr->cmd, 0, 0);
        break;

  } /* switch */
} /* cmcall_client_cmd_ll_reply() */


#ifdef FEATURE_IMS_VCC
/*===========================================================================
@FUNCTION cmcall_switch_call_id

@DESCRIPTION
  Switch call object id from call_id2 to call_id1. Call id 1 is store at
  the call pointer for call id 2.

@DEPENDENCIES
  None.

@RETURN VALUE
  TRUE: success; FALSE: failure

@SIDE EFFECTS
  none

@x
===========================================================================*/
boolean cmcall_switch_call_id(

    cm_call_id_type     call_id1,
    /**< call id to be stored*/

    cm_call_id_type     call_id2
    /**< Current call id */

    )
{
   cmcall_s_type    *call_ptr= NULL;
   cm_orig_q_s_type   *q_ptr = NULL;

   if ( call_id1 >= CMCALL_MAX_CALL_OBJ ||
           call_id2 >= CMCALL_MAX_CALL_OBJ )
   {
       return FALSE;
   }

   call_ptr = cmcall_ptr(call_id2);

   if( call_ptr != NULL )
   {
       call_ptr->call_id = call_id1;
       call_obj_array[call_id1] = call_ptr;
       call_obj_array[call_id2] = NULL;

       call_ptr = cmcall_ptr(call_id1);

       if ( call_ptr != NULL )
       {
           q_ptr = cmtask_orig_para_search_act_id(call_ptr->ss, call_id2);

           if ( q_ptr != NULL )
           {
               q_ptr->orig.act_id = call_id1;
           }
       }

       return TRUE;
   }

   return FALSE;
}

/*===========================================================================
@FUNCTION cmcall_handover_cmd_proc

@DESCRIPTION
  Processes handover request.

@DEPENDENCIES
  None.

@RETURN VALUE
  none

@SIDE EFFECTS
  none
@x
===========================================================================*/
static void cmcall_handover_cmd_proc(

    cm_cmd_s_type  *cmd_ptr
   /*< pointer to a CM command */
)
/*lint -esym(818, cmd_ptr) */
{

   cmcall_s_type                      *call_ptr = NULL;
   /**< Pointer to a call object */

   const cm_call_cmd_s_type           *call_cmd_ptr = NULL;
   /**< Point at call command component */

   const cm_call_cmd_info_s_type      *cmd_info_ptr = NULL;
   /**< Pointer to the command info */


   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   CM_ASSERT( cmd_ptr != NULL );

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /** Get the pointer to call command */

   call_cmd_ptr = CALL_CMD_PTR( cmd_ptr );

   /** Get the pointer to call command info */
   cmd_info_ptr      = CALL_CMD_INFO_PTR(call_cmd_ptr);

   /** Get the Call pointer */
   call_ptr = cmcall_ptr( cmd_info_ptr->call_id );

   if(call_ptr == NULL)
   {
       cmcall_client_cmd_err( cmd_ptr, CM_CALL_CMD_ERR_CALL_ID_P );
       return;
   }
   /** Update call fields by copying relevant
   ** command fields into call struct.
   */
   cmcall_cmd_copy_fields( call_ptr, call_cmd_ptr );


   /** Generate the HANDOVER_START event to clients.
   */
   cmcall_event( call_ptr, CM_CALL_EVENT_HANDOFF_MANUAL_CALL_ORIG);

   /** Deallocate the call id as this was required only to pass
   *  the handoff call event to client */
   cm_call_id_deallocate(call_ptr->call_id);

}
/*lint +esym(818, cmd_ptr) */

#endif


/*===========================================================================

FUNCTION CMCALL_CLIENT_CMD_PROC

DESCRIPTION
  Dispatches to the right call cmd handler based on system operating mode

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_client_cmd_proc
(
  cm_cmd_s_type  *cmd_ptr                       /* pointer to a CM command */
)
{
  cm_call_cmd_s_type           *call_cmd_ptr = NULL;
  cm_call_cmd_info_s_type      *cmd_info_ptr = NULL;
  cm_call_cmd_err_e_type        call_cmd_err;
  boolean                       wait_for_reply;
  cm_call_mode_info_e_type      call_mode;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr != NULL);
  CM_FUNC_START( "cmcall_client_cmd_proc()",0,0,0 );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_cmd_ptr = CALL_CMD_PTR( cmd_ptr );
  cmd_info_ptr = CMD_INFO_PTR( call_cmd_ptr );

  #ifdef CM_DEBUG
#error code not present
  #endif

  /* If error found, notify clients of command error and return.
  */
  if (cmd_info_ptr->cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    cmcall_client_cmd_err_deallocate (cmd_ptr, cmd_info_ptr->cmd_err);
    return;
  }

  switch(cmd_ptr->cmd.call.cmd)
  {

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* Process here all generic commands, ie. those
    ** that are processed the same way for both GW and 1X calls
    */
    case CM_CALL_CMD_INFO_GET:

      /* Start by checking whether this is a valid command
      ** relative to the current state of the call/phone.
      */
      call_cmd_err = cmcall_client_cmd_check( cmd_ptr );

      /*
      ** Notify clients of command processing status.
      ** If error found, return.
      */
      cmcall_client_cmd_err( cmd_ptr, call_cmd_err );
      if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
      {
        return;
      }

      /* If we got here, no errors were found,
      ** so forward the client command to MC.
      */
      wait_for_reply = cmcall_client_cmd_forward_ll( cmd_ptr );

      /* If NO need to wait for replay from MC,
      ** call the reply function right a way.
      */
      if( ! wait_for_reply )
      {
        cmcall_client_cmd_ll_reply( cmd_ptr, CM_CALL_CMD_ERR_NOERR );
      }

      /* We're done for generic commands
      */
      break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CALL_CMD_ORIG:
    case CM_CALL_CMD_CC_COMPLETE_REORIG:

      /* Origination is a bit more complicated, so we'll handle it here
      */
      cmcall_client_orig_proc (cmd_ptr);
      break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CALL_CMD_SUPS:

      /*
      ** Find out what mode we are in
      */
      call_mode = cmcall_call_id_mode( cmd_info_ptr->call_id );

      /* Process command based on call mode
      */
      switch(call_mode)
      {
        case CM_CALL_MODE_INFO_GW_CS:
        case CM_CALL_MODE_INFO_GW_PS:
          #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
          #endif
          break;

        case CM_CALL_MODE_INFO_CDMA:
          #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
          cmxcall_client_cmd_proc( cmd_ptr );
          #endif
          break;

        case CM_CALL_MODE_INFO_IP:
          #if (defined(FEATURE_IP_CALL))
          cmipcall_client_cmd_proc ( cmd_ptr );
          #endif
          break;

          /* NOTE - Fall through on purpose */

        default:
          cmcall_client_cmd_err( cmd_ptr, CM_CALL_CMD_ERR_CALL_ID_P );
          CM_ERR("Invalid call id %d with sups cmd",
                  cmd_info_ptr->call_id, 0, 0);
          break;

      } /*  switch(call_mode)  */

      break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CALL_CMD_INFO_LIST_GET:

      cmcall_get_call_info_list_proc( cmd_ptr );
      break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CALL_CMD_PRIVACY_PREF:
      #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
      cmxcall_client_cmd_proc( cmd_ptr );
      #endif
      break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CALL_CMD_PS_DATA_AVAILABLE:
      #if ( (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && \
            !defined( FEATURE_PS_DORMANT_PWR_SAVE) )
#error code not present
      #endif
      break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   case CM_CALL_CMD_PS_SIG_REL:
      #if((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && \
           defined(FEATURE_UE_INITIATED_DORMANCY))
#error code not present
      #endif
      break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
    #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    #ifdef FEATURE_IMS_VCC
    case CM_CALL_CMD_HANDOVER_REQ:
      cmcall_handover_cmd_proc( cmd_ptr );
      break;
    #endif

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CALL_CMD_END:
      /* Update the end cmd with current state of calls
      */
      cmcall_update_call_cmd_end (cmd_ptr);

      cmcall_send_mode_spec_end  (cmd_ptr);

      break;


    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    default:
      /*
      ** Find out what mode we are in
      */

      call_mode = cmcall_call_id_mode( call_cmd_ptr->info.call_id );


      /* Process command based on call mode
      */
      switch(call_mode)
      {
        case CM_CALL_MODE_INFO_GW_CS:
        case CM_CALL_MODE_INFO_GW_PS:
          #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
          #endif
          break;

        case CM_CALL_MODE_INFO_CDMA:
          #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
          cmxcall_client_cmd_proc( cmd_ptr );
          #endif
          break;

        case CM_CALL_MODE_INFO_IP:

          #if defined(FEATURE_IP_CALL)
          cmipcall_client_cmd_proc ( cmd_ptr );
          #endif
          break;

          /*lint -fallthrough */
          /* NOTE - Fall through on purpose for cmds other than call end
          */

        default:
          cmcall_client_cmd_err( cmd_ptr, CM_CALL_CMD_ERR_CALL_ID_P );
          CM_ERR("Invalid call ptr for call id %d for call ending",
                  cmd_ptr->cmd.call.info.end_params[0].call_id, 0, 0);
          break;

      } /*  switch(call_mode)  */
  } /* switch(cmd_ptr->cmd.call.cmd) */
} /* cmcall_client_cmd_proc() */



/*===========================================================================

FUNCTION cmcall_init

DESCRIPTION
  Initializing the call object.

  This function must be called before the call object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_init( void )
{

  unsigned int i;
      /* Loop index */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmcall_init()",0,0,0 );

  /* Initialize all call object array
  */
  for ( i = 0; i < ARR_SIZE(call_obj_array); i++ )
  {
    call_obj_array[i] = NULL;
  } /* for  */

  #if defined (FEATURE_GSM ) || defined (FEATURE_WCDMA )
#error code not present
  #endif /* defined (FEATURE_GSM ) || defined (FEATURE_WCDMA ) */

} /* cmcall_init() */

/*===========================================================================
FUNCTION cmcall_hybr_pref_was_toggled_for_origination

DESCRIPTION

  Determine whether hybrid preference was toggled for origination.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybrid preference was toggled for origination,
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean cmcall_hybr_pref_was_toggled_for_origination(

     cm_hybr_pref_e_type requested_hybr_pref
     /* Hybrid preference requested for the next call */
)
{

  /* Step 1: Check MAIN Q's, top_ptr's hybr_pref
  ** Step 2: Do the procession
  */

  cm_orig_q_s_type *ph_orig_para_top_ptr =
                                  cmtask_orig_para_get_top( CM_SS_MAIN );

  if( ph_orig_para_top_ptr != NULL &&
      ph_orig_para_top_ptr->orig.orig_hybr_pref != CM_HYBR_PREF_NONE )
  {
    return requested_hybr_pref != ph_orig_para_top_ptr->orig.orig_hybr_pref;
  }
  else
  {
     /* We're forcing mode for origination */
    return requested_hybr_pref != cmph_ptr()->hybr_pref;
  }
}


/*===========================================================================

FUNCTION cmcall_determine_ss_for_origination

DESCRIPTION

  Based on mode and hybrid preference, determine ss for call object /
  cmpref_proc_cmd_pref_sys_chgd() function.

  The assumption is that for call originations sys pref change will be sent
  to either 1X or HDR (depending on where the origination will go) but not both.
  Sys pref change goes to both only when hybr_pref is toggled from on
  to off or vice versa.

DEPENDENCIES
  None

RETURN VALUE
  ss

SIDE EFFECTS
  None

===========================================================================*/
cm_ss_e_type cmcall_determine_ss_for_origination(
  cm_mode_pref_e_type    mode_pref,

  cm_hybr_pref_e_type    hybr_pref,

  cm_cc_mode_pref_e_type mode_usage,

  const cmcall_s_type          *call_ptr
)
{
  cm_ss_e_type               ss;
  sd_ss_mode_pref_e_type     sd_mode_pref;
  cmcall_s_type   *call_ptr_temp = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Determine mode preference for the given call */
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref( mode_pref );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Find if given call can be made over IP
  */
  if ((ss = cmcall_instance_for_ipcall (call_ptr)) != CM_SS_NONE)
  {
    return ss;
  }
  else
  {
    /* Video share calls are possible only on IP */
    if (call_ptr->call_type == CM_CALL_TYPE_VS)
    {
      return CM_SS_NONE;
    }
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If sd_mode_pref has WLAN component, then allow calls over
  ** WLAN in following rules:
  **
  **   - PS DATA Call Request and Pref_SRV_TYPE for PKT_ORIG is WLAN.
  */


  if(sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_WLAN))
  {
    if(call_ptr->call_type == CM_CALL_TYPE_PS_DATA &&
       (cm_get_pref_srv_type_for_packet_orig() == SYS_SYS_MODE_WLAN))
    {
         return CM_SS_WLAN;
    }
  }


  /* If we toggle hybr_pref from ON to OFF or from OFF to ON,
     set ss to SD_SS_MAX and return.

     Otherwise
     - if mode_pref has CDMA component only, use SD_SS_MAIN
     - if mode_pref has HDR component only, use SD_SS_HDR
     - if it has both, call cmpref_is_hybr_hdr_call_allowed()
                       - yes: use SD_SS_HDR
                       - no:  use SD_SS_MAIN
     - if it has none, use SD_SS_MAIN
  */
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref( mode_pref );
  ss = CM_SS_MAIN;

  if(cmcall_hybr_pref_was_toggled_for_origination(hybr_pref)) {

    CM_MSG_MED("Hybr pref toggled to %d",hybr_pref,0,0);
    return CM_SS_MAX;

  }

  if ( !cmpref_is_hybr_status_on() )
  {
    /* Hybrid is off, have to attempt on SS_MAIN.
    */
    return ss;

  }

  if( sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_HDR) &&
      !(sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_CDMA )))
  {
    /* HDR system bit set and CDMA system bit not set */
    CM_MSG_MED("HDR system bit set and CDMA system bit not set",0,0,0);
    ss = CM_SS_HDR;
  }
  else if( (sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_CDMA)) &&
           (sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_HDR)))
  {
    /* CDMA system bit set and HDR system bit set */
    CM_MSG_MED("CDMA system bit set and HDR system bit set",0,0,0);

    /* If packet call is getting orig and hybrid hdr call is allowed, put
    ** the call on HYBR HDR instance
    */
    if( cmpref_is_hybr_hdr_call_allowed() &&
        (call_ptr->call_type == CM_CALL_TYPE_PS_DATA)
      )
    {
      CM_MSG_MED("Hybr HDR call allowed",0,0,0);
      ss = CM_SS_HDR;
    }
  }

  /* If we are in hybrid and already have a voice call up on 1X system,
       then the data call should go by definition on 1X system */
  if( (ss == CM_SS_HDR) &&
      (call_ptr->call_type == CM_CALL_TYPE_PS_DATA) &&
      cmpref_is_hybr_status_on())
  {

      cm_call_id_type voice_call_id;
      voice_call_id = cmcall_is_there_a_call_type
                           ( CM_CALL_TYPE_VOICE,CM_CALL_ID_INVALID);


      if(voice_call_id != CM_CALL_ID_INVALID)
      {
        call_ptr_temp = cmcall_ptr(voice_call_id) ;
        if(call_ptr_temp == NULL)
        {
            CM_MSG_HIGH( "call_ptr_NULL",0,0,0);
            return ss;
        }
        if(call_ptr_temp->ss == CM_SS_MAIN)
        {
          ss = CM_SS_MAIN;
        }
      }
  }

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif
  SYS_ARG_NOT_USED (mode_usage);
  return ss;
}


/*===========================================================================

FUNCTION cmcall_data_session_reg

DESCRIPTION
  Register data session callback functions with CM

  When call manager ends a data call, it consults the data session callback
  functions for the following information:

     1. Total number of bytes received and transmitted during the data call.

DEPENDENCIES
  CM must have already been initialized with cm_init_before_task_start().

RETURN VALUE
  Status of registration

SIDE EFFECTS
  none

===========================================================================*/
cm_data_session_status_e_type cmcall_data_session_reg(

  cm_get_data_session_total_byte_ctrs_cb_f_type
                              *data_session_total_byte_ctrs_cb_func_ptr
    /* Pointer to data session total byte counters callback function */

)
{
  if ( data_session_total_byte_ctrs_cb_func_ptr == NULL )
  {
    return  CM_DATA_SESSION_ERR_NULL_DATA_SESSION_TOTAL_BYTE_CTRS_FUNC;
  }
  else if ( cmcall_data_session_total_byte_ctrs_cb_func_ptr != NULL )
  {
    return  CM_DATA_SESSION_ERR_DATA_SESSION_TOTAL_BYTE_CTRS_FUNC_PREV_REG;
  }
  else
  {
    cmcall_data_session_total_byte_ctrs_cb_func_ptr =
                                    data_session_total_byte_ctrs_cb_func_ptr;
    return CM_DATA_SESSION_CB_OK;
  }

}  /* cm_data_session_reg */


/*===========================================================================

FUNCTION cmcall_call_control_complete_cb

DESCRIPTION
  Callback function provided to a client that is performing call control for CM.
  Is called when call control is complete.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_call_control_complete_cb(

  cm_call_id_type                         call_id,
    /* call id */

  cm_cc_status_e_type                     status,
    /* status of command */

  cm_call_type_e_type                     call_type,
    /* call type */

  const cm_orig_call_control_s_type      *cdma_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on cdma */

  const cm_orig_call_control_s_type       *gw_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on gw */

  void                                    *user_data
    /* data passed from original call */

)
/*lint -esym(715, status) */
{
  cm_cmd_s_type              *cmd_ptr;
  cmcall_s_type              *call_ptr;
  cm_call_cmd_s_type         *call_cmd_ptr;
  cm_call_cmd_info_s_type    *cmd_info_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cdma_call_control_info_ptr != NULL);
  CM_ASSERT(gw_call_control_info_ptr   != NULL);
  CM_ASSERT(user_data                  != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*lint -save -e774 Boolean within 'if' always evaluates to False
  ** Yup, but only when CM_DEBUG is on
  */
  /* We expect user_data to point to a call command
  */
  if (user_data == NULL)
  {
    ERR("NULL cmd_ptr in cc callback for %d", call_id, 0, 0);
    return;
  }
  /*lint -restore */

  cmd_ptr = (cm_cmd_s_type *) user_data;
  call_cmd_ptr = CALL_CMD_PTR (cmd_ptr);
  cmd_info_ptr = CMD_INFO_PTR (call_cmd_ptr);

  /* The command buffer has been returned to CM. */
  cmd_ptr->passed_for_cc = 2;

  /* Copy info to call object and queue it back to CM */
  call_ptr = cmcall_ptr (call_id);
  if (call_ptr == NULL)
  {
    ERR("No call object found for call id %d in cc callback", call_id, 0, 0);
    /* Deallocate cmd_ptr. Otherwise leads to memory leak */
    cm_cmd_dealloc( cmd_ptr );
    return;
  }
  cmd_ptr->cmd.call.info.call_control_processing_complete = TRUE;


  cmd_info_ptr->call_type = call_type;
  #ifdef FEATURE_OTASP
  /* If this is an otasp call then we gotta copy the activation code as well. */
  if ((call_type == CM_CALL_TYPE_STD_OTASP) ||
      (call_type == CM_CALL_TYPE_NON_STD_OTASP))
  {
       cmd_info_ptr->cdma_info.otasp_act_code = cdma_call_control_info_ptr->activate_code;
  }
  #endif  /* FEATURE_OTASP */

  /*lint -save -e774 Boolean within 'if' always evaluates to False
  ** Yup, but only when CM_DEBUG is on
  */
  if (cdma_call_control_info_ptr != NULL)
  /*lint -restore */
  {
      memcpy(&call_ptr->cdma_cc_modified_info,
        cdma_call_control_info_ptr,
        sizeof(cm_orig_call_control_s_type));
  }
  else
  {
      call_ptr->cdma_cc_modified_info.call_control_result =
        CM_CC_RESULT_NOT_ALLOWED;
  }

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #else
  SYS_ARG_NOT_USED(gw_call_control_info_ptr);
  #endif


  cmd_ptr->cmd_type = CM_CMD_TYPE_CALL;
  cmd_ptr->cmd.call.cmd = CM_CALL_CMD_CC_COMPLETE_REORIG;

  cm_cmd_queue (cmd_ptr);
}  /* cmcall_call_control_complete_cb */
/*lint +esym(715, status) */

/*===========================================================================

FUNCTION cmcall_is_cc_required

DESCRIPTION
  Check whether call control is required.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_cc_required
(
  cm_cc_subscription_source_e_type     subscription_source
)
/*lint -esym(715, subscription_source) */
{
  #ifdef FEATURE_GSTK
  if (subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1)
    #ifdef FEATURE_DUAL_SLOTS
    return gstk_slot_is_cc_required (GSTK_SLOT_1);
    #else
    return gstk_is_cc_required ();
    #endif
  if (subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2)
    #ifdef FEATURE_DUAL_SLOTS
    return gstk_slot_is_cc_required (GSTK_SLOT_2);
  #else
    return gstk_is_cc_required ();
    #endif
  #endif
  return FALSE;
}
/*lint +esym(715, subscription_source) */

#if defined( FEATURE_GSTK ) && (defined( FEATURE_GSM ) || defined( FEATURE_WCDMA ))
#error code not present
#endif /* FEATURE_GSTK && ( FEATURE_GSM || FEATURE_WCDMA ) */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif // #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

/*===========================================================================

FUNCTION  cmcall_check_call_type_in_call_state

DESCRIPTION
  Checks if there is a call of the given type and in the given state.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if a call can be found with the requested parameters.

SIDE EFFECTS
  none

===========================================================================*/

boolean cmcall_check_call_type_in_call_state (

                  cm_call_type_e_type   call_type,
                  /* Given call type */

                  cm_call_state_e_type   call_state
                  /* Given call state */
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
    {
      if ((call_ptr->call_type == call_type) && (call_ptr->call_state == call_state))
      {
        return TRUE;
      }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while  */

  return FALSE;
} /* cmcall_check_call_type_in_call_state () */

/*===========================================================================

FUNCTION  cmcall_check_for_call_state_on_ss

DESCRIPTION
  Checks if there is a call in a particular state on a given ss.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if there is a call in a particular state on a specified Serving system

SIDE EFFECTS
  none

===========================================================================*/

boolean cmcall_check_for_call_state_on_ss (
                  cm_ss_e_type    ss,
                  /* Given call type   */

                  cm_call_state_e_type   call_state
                  /* Given call state   */
)
{
  cmcall_s_type   *call_ptr;
  /* Call object pointer
   */
  cm_iterator_type call_itr;
   /* call Object Iterator
    */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ((call_ptr->ss == ss ) && (call_ptr->call_state == call_state))
    {
      return TRUE;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while  */

  return FALSE;
} /* cmcall_check_for_call_state_on_ss() */


/*===========================================================================

FUNCTION  cmcall_check_each_call_in_call_state

DESCRIPTION
  Checks if each call is in the given state.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if each call can be found in the requested state.

SIDE EFFECTS
  none

===========================================================================*/

boolean cmcall_check_each_call_in_call_state (

                  cm_call_state_e_type   call_state
                  /* Given call state */
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  /* Search all the calls to find the first match with call_type and call_state */
  while(call_ptr != NULL)
  {
      if (call_ptr->call_state != call_state)
      {
        return FALSE;
      }

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while  */

  return TRUE;
} /* cmcall_check_each_call_in_call_state () */

/*===========================================================================

FUNCTION cmcall_reset

DESCRIPTION
  Reset common call fields to default values reflecting a call in idle state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_reset(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  unsigned int i=0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL )
  CM_FUNC_START( "cmcall_reset()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Mark object as initialized.
  */
  CM_INIT_MARK( call_ptr->init_mask );

  /* Initialize common call fields
  */

  call_ptr->call_id                      = CM_CALL_ID_INVALID;
  call_ptr->call_type                    = CM_CALL_TYPE_NONE;
  call_ptr->call_state                   = CM_CALL_STATE_IDLE;
  call_ptr->call_subst.incom             = CM_CALL_INCOM_SUBST_NONE;
  call_ptr->call_subst.conv              = CM_CALL_CONV_SUBST_NONE;
  call_ptr->call_subst.idle              = CM_CALL_IDLE_SUBST_NONE;
  call_ptr->call_client_ptr              = CM_CLIENT_PTR_NONE;
  call_ptr->call_type                    = CM_CALL_TYPE_NONE;
  call_ptr->alpha.len                    = CM_ALPHA_LEN_NONE;
  call_ptr->enc_alpha.num_bytes          = CM_ALPHA_LEN_NONE;
  call_ptr->signal.is_signal_info_avail  = FALSE;
  call_ptr->end_status                   = CM_CALL_END_NONE;
  call_ptr->sys_mode                     = SYS_SYS_MODE_NO_SRV;
  call_ptr->srv_type                     = CM_SRV_TYPE_NONE;
  call_ptr->orig_func                    = NULL;
  call_ptr->is_user_originated_call      = FALSE;
  call_ptr->orig_mode                    = SD_SS_ORIG_MODE_NONE;
  call_ptr->call_connect_time            = 0;
  call_ptr->call_end_time                = 0;
  call_ptr->bytes_received               = 0;
  call_ptr->bytes_transmitted            = 0;
  call_ptr->send_call_event_end          = TRUE;
  call_ptr->ss                           = CM_SS_MAIN;
  call_ptr->favored_mode_pref            = CM_MODE_PREF_NONE;
  call_ptr->favored_hybr_pref            = CM_HYBR_PREF_NONE;
  call_ptr->favored_prl_pref             = CM_PRL_PREF_NONE;
  call_ptr->favored_roam_pref            = CM_ROAM_PREF_NONE;
  call_ptr->line                         = CM_ALS_LINE_NONE;
  call_ptr->ho_type                      = SYS_HO_TYPE_NONE;
  call_ptr->ho_status                    = SYS_HO_STATUS_NONE;
  call_ptr->umts_cdma_ho_status          = SYS_UMTS_CDMA_HANDOVER_NONE;
  call_ptr->direction                    = CM_CALL_DIRECTION_NONE;
  call_ptr->force_top_pref               = FALSE;
  call_ptr->pd_status                    = CMCALL_PD_STATUS_NONE;
  call_ptr->sms_status                   = CMCALL_SMS_STATUS_NONE;
  call_ptr->is_hdr_acq_fail              = FALSE;
  call_ptr->waiting_for_hdr_session_close = FALSE;
  call_ptr->is_707b_ps_data              = FALSE;  

  /* Reset end_params
  */
  memset(&call_ptr->end_params,
         CM_CALL_CMD_PARAM_DEFAULT_VALUE,
         sizeof(cm_end_params_s_type));

  call_ptr->end_params.call_id   = CM_CALL_ID_UNASSIGNED;
  call_ptr->end_params.info_type = CM_CALL_MODE_INFO_NONE;

  /* Reset call mode info fields.
  */
  memset(&call_ptr->cmcall_mode_info,
         CM_CALL_CMD_PARAM_DEFAULT_VALUE,
         sizeof(cmcall_mode_info_type));

  /* Initialize the number component.
  */
  cm_num_init( &call_ptr->num );
  cm_num_init( &call_ptr->calling_num );
  cm_num_init( &call_ptr->redirecting_number.num );
  
  #ifdef FEATURE_IP_CALL
  cm_num_init( &call_ptr->waiting_num );
  #endif
  /* Reset the silent redial component.
  */
  sr_reset( &call_ptr->sr );

  /* Initialize the phone pointer to point at the phone object.
  */
  call_ptr->ph_ptr = cmph_ptr();

  /* Initialize command reply object
  */
  cm_reply_init ( &call_ptr->reply );

  for( i=0; i < ARR_SIZE(call_ptr->prev_srv_status); i++)
  {
    call_ptr->prev_srv_status[i] = SYS_SRV_STATUS_NONE;
  }

  call_ptr->main_ss_mode  = CM_CC_MODE_PREF_NONE;

  #if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900))
  cmxcall_reset(call_ptr);
  #endif

  #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
  #endif

  #if defined(FEATURE_IP_CALL)
  cmipcall_reset(call_ptr);
  #endif

}

/*===========================================================================

FUNCTION cmcall_is_call_id_in_range

DESCRIPTION
  Checks if a given Call Id is in a valid range from 0 to CALL_ID_MAX.

DEPENDENCIES
  Call Id must have already been allocated.

RETURN VALUE
  TRUE if valid.
  FALSE if not valid.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmcall_is_call_id_in_range(cm_call_id_type call_id)
{
  /*
  ** cm_call_id_type is an unsigned type so smallest value that call_id
  ** can have is 0
  ** 0 is a valid call id so it's sufficient to test against CM_CALL_ID_MAX
  ** only
  */
  return (call_id < CM_CALL_ID_MAX);
}/* cmcall_is_call_id_in_range() */


/*===========================================================================

FUNCTION cmcall_is_hdr_call

DESCRIPTION
  Checks whether there is an HDR call up.

DEPENDENCIES
  None

RETURN VALUE
  Valid call id of a HDR call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  None

===========================================================================*/
 cm_call_id_type cmcall_is_hdr_call
(
  void
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  /*-----------------------------------------------------------------------*/
  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);


  while(call_ptr != NULL)
  {

      if ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA  &&
           call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_HDR
         )
      {
        return (call_ptr->call_id);
      }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

  return CM_CALL_ID_INVALID;

} /* cmcall_is_there_a_hdrdata_call() */


/*===========================================================================
FUNCTION CMCALL_IS_HYBR_HDR_CALL

DESCRIPTION
  Check whether this is a hybrid HDR call

DEPENDENCIES
  Call object must be initialized

RETURNS
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_hybr_hdr_call (cm_call_id_type *call_id_ptr)
{

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif

  /*-----------------------------------------------------------------------*/

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif // FEATURE_HDR_HYBRID

  if(call_id_ptr != NULL)
  {
    *call_id_ptr = (cm_call_id_type) CM_CALL_ID_NONE;
  }
  return FALSE;

} /* cmxcall_is_hybr_hdr_call() */


/*===========================================================================
FUNCTION CMCALL_IS_HYBR_HDR_ORIG

DESCRIPTION
  Check whether there is a hybrid HDR call origination

DEPENDENCIES
  Call object must be initialized

RETURNS
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
cmcall_s_type* cmcall_is_hybr_hdr_orig (void)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  /*-----------------------------------------------------------------------*/

  if ( cmpref_is_hybr_status_on() )
  {

    cmcall_obj_iterator_init(&call_itr);

    call_ptr = cmcall_obj_get_next(&call_itr);

    while(call_ptr != NULL)
    {
      if( call_ptr->call_state  == CM_CALL_STATE_ORIG   &&
          call_ptr->ss          == CM_SS_HDR            &&
          call_ptr->call_type   == CM_CALL_TYPE_PS_DATA
        )
      {
        return call_ptr;
      }

      call_ptr = cmcall_obj_get_next(&call_itr);
    }
  } /* if ( cmpref_is_hybr_status_on() ) */

  return NULL;

} /* cmxcall_is_hybr_hdr_orig() */


/*===========================================================================

FUNCTION cmcall_is_there_a_cdma_call_in_conv

DESCRIPTION
  Checks is there is a call object allocated for CDMA.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_a_cdma_call_in_conv ()
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
      if ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA &&
           call_ptr->sys_mode == SYS_SYS_MODE_CDMA )
      {
        if (call_ptr->call_state == CM_CALL_STATE_CONV)
        {
          return call_ptr->call_id;
        }
      } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return CM_CALL_ID_INVALID;

} /*  */


/*===========================================================================

FUNCTION cmcall_is_there_a_call_type

DESCRIPTION
  Checks is there is a requested call type.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_a_call_type
(
  cm_call_type_e_type call_type,

  cm_call_id_type     ignore_call_id

)
{

  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
      if ( call_ptr->call_type == call_type )
      {
        if (ignore_call_id == CM_CALL_ID_INVALID)
          return call_ptr->call_id;
        else
        {
          if (ignore_call_id != call_ptr->call_id)
          {
            return call_ptr->call_id;
          }
        }
      } /* if ( call_ptr->call_type == call_type ) */

      call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return CM_CALL_ID_INVALID;

} /* cmcall_is_there_a_call_type() */


/*===========================================================================

FUNCTION cmcall_generate_connection_id

DESCRIPTION
  This function generates a Connection Id and sets the connection Id and its
  Source in the call Objet.

  Please note that Connection Id makes sense only for the GSM/WCDMA modes.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
boolean cmcall_generate_connection_id
/*lint -esym(715, call_ptr) */
/*lint -esym(818, call_ptr) could be declared as pointing to const
** Nope, not for GW flags
*/
(
  cmcall_s_type             *call_ptr
)
{
  #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
  #endif // (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
  return TRUE;
} /* cmcall_generate_connection_id() */
/*lint +esym(715, call_ptr) */
/*lint +esym(818, call_ptr) */


/*===========================================================================

FUNCTION cmcall_set_cdma_cc_result

DESCRIPTION
  This function sets the Call Control (CC) result to the specifid value for a
  given call object.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
void cmcall_set_cdma_cc_result
(
  cmcall_s_type             *call_ptr,
  cm_cc_result_e_type       cc_result
)
{
  if(call_ptr == NULL)
  {
    CM_ERR("Can not set CDMA CC Result, Null call_ptr",0,0,0);
    return;
  }
  call_ptr->cdma_cc_modified_info.call_control_result = cc_result;
}/* cmcall_set_cdma_cc_result() */


/*===========================================================================

FUNCTION cmcall_set_gw_cc_result

DESCRIPTION
  This function sets the Call Control (CC) result to the specifid value for a
  given call object.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
void cmcall_set_gw_cc_result
(
  cmcall_s_type             *call_ptr,
  cm_cc_result_e_type       cc_result
)
{
  SYS_ARG_NOT_CONST(call_ptr);

  if(call_ptr == NULL)
  {
    CM_ERR("Can not set GW CC Result, Null call_ptr",0,0,0);
    return;
  }
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #else
  SYS_ARG_NOT_USED(cc_result);
  #endif
} /* cmcall_set_gw_cc_result() */


/*===========================================================================

FUNCTION cmcall_obj_iterator_init

DESCRIPTION
  This function initializes the Call Object Iterator.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object Iterator Current Index will be modified.

===========================================================================*/
void cmcall_obj_iterator_init(cm_iterator_type *iterator_ptr)
{
  CM_ASSERT(iterator_ptr != NULL);

  iterator_ptr->next = 0;
  return;
}/* cmcall_obj_iterator_init() */


/*===========================================================================

FUNCTION cmcall_obj_get_next

DESCRIPTION
  This function gets the next call object in the collection of call objects.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Pointer to Call Object structure.

SIDE EFFECTS
  Call Object Iterator current index will be modified.

===========================================================================*/
cmcall_s_type* cmcall_obj_get_next(cm_iterator_type *iterator_ptr)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  unsigned int   index;
  cmcall_s_type  *call_ptr = NULL;
  boolean found = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(iterator_ptr != NULL)
  CM_ASSERT(iterator_ptr->next <= ARR_SIZE(call_obj_array))

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  index = iterator_ptr->next;


  /*
  ** if we have exhausted all call objects, we always
  ** return NULL.
  */
  if(index >= ARR_SIZE(call_obj_array))
  {
    return NULL;
  }


  while((index < ARR_SIZE(call_obj_array)) && !found)
  {
     call_ptr = call_obj_array[index];
     if(call_ptr != NULL)
     {
       found = TRUE;
     }
     index++;
     iterator_ptr->next = index;
  }

  return call_ptr;
}/* cmcall_obj_get_next() */


/*===========================================================================

FUNCTION cmcall_set_call_direction

DESCRIPTION

  Sets the call direction.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
void cmcall_set_call_direction
(
  cmcall_s_type             *call_ptr,
  cm_call_direction_e_type   call_direction
)
{

  CM_ASSERT(call_ptr != NULL);

  CM_ASSERT( BETWEEN(call_direction,
                     CM_CALL_DIRECTION_NONE,
                     CM_CALL_DIRECTION_MAX));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_ptr->direction  = call_direction;

} /* cmcall_set_call_direction () */


/*===========================================================================

FUNCTION cmcall_count_calls_with_call_type

DESCRIPTION

  Counts total number of calls of a particular call type


DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  total number of calls

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
int cmcall_count_calls_with_call_type (

  cm_call_type_e_type call_type
    /* call type */
)
{
  int total_calls = 0;
      /* counts the total number of calls */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  cmcall_s_type   *call_ptr = NULL;
      /* call_ptr */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
      if ( call_ptr->call_type == call_type )
      {
        ++total_calls;

      } /* if ( call_ptr->call_type == call_type ) */

      call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return total_calls;

} /* cmcall_count_calls_with_call_type () */


/*===========================================================================

FUNCTION cmcall_get_call_obj_mo_in_orig

DESCRIPTION

  Returns


DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  call pointer for the call type in origination state or NULL if none

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
cmcall_s_type *cmcall_get_call_obj_mo_in_orig (

  cm_call_type_e_type call_type
    /* call type */
)
{
  cm_iterator_type call_itr;
      /* call Object Iterator */
  cmcall_s_type   *call_ptr = NULL;
      /* call_ptr */


  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
      if (( call_ptr->call_type == call_type ) &&
          (call_ptr->call_state == CM_CALL_STATE_ORIG))
      {
        return call_ptr;
      } /* if ( call_ptr->call_type == call_type ) */

      call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return NULL;
} /* cmcall_get_call_obj_mo_in_orig () */


#if (defined(FEATURE_IP_CALL) && defined(FEATURE_WLAN))
#error code not present
#endif

#if !defined(FEATURE_LOW_MEM_TARGET)
/*===========================================================================

FUNCTION cmcall_remove_mode_and_force

DESCRIPTION
  Removes the given mode_pref from the call's favored_mode_pref and forces
  the appropriate SS for call origination.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

  This function is expected to be called during the origination process
  before call is connected.

RETURN VALUE
  A ballpark estimation for the number of seconds the phone is going
  to be without service. If mode is not forced, return 0.

SIDE EFFECTS
  none

===========================================================================*/
dword  cmcall_remove_mode_and_force(

  cmcall_s_type    *call_ptr,
        /* pointer to a call object */

  cm_mode_pref_e_type  mode_pref_to_remove
        /* CM mode pref to be removed */
)
{
  cmph_s_type           *ph_ptr             = cmph_ptr();

  sd_ss_pref_reas_e_type pref_reas          = SD_SS_PREF_REAS_REDIAL_NORM;

  boolean                force_new_mode     = TRUE;

  cm_orig_q_s_type      *computed_orig_para = NULL;

  cm_mode_pref_e_type    new_mode_pref    = CM_MODE_PREF_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr           != NULL );
  CM_ASSERT( ph_ptr             != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Compute new mode pref */
  new_mode_pref = cm_remove_mode_pref_components(
                      call_ptr->favored_mode_pref,
                      1,
                      cmph_map_cm_mode_pref_to_sd_mode_pref(
                        mode_pref_to_remove
                      )
                    );

  if( new_mode_pref == CM_MODE_PREF_NONE )
  {
    force_new_mode = FALSE;
  }

  /* If we decide to force with the new mode pref, check if the other mode
  ** prefs can be used for origination without any errors.
  */
  if( force_new_mode )
  {
    if( cmcall_process_orig_mode(call_ptr->main_ss_mode, call_ptr) !=
          CM_CALL_CMD_ERR_NOERR )
    {
      force_new_mode = FALSE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find the previously used origination parameters */

  computed_orig_para =  cmtask_orig_para_search_act_id  (
                                                     call_ptr->ss,
                                    (cm_act_id_type) call_ptr->call_id );

  CM_ASSERT( computed_orig_para != NULL);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no other mode left for origination, might as well retry on the
  ** original mode pref
  */
  if( !force_new_mode )
  {

    return cmph_force_pref_on_the_fly
          (
           call_ptr->ss,
           ph_ptr,
           pref_reas,
           cmph_map_call_type_to_act_type( call_ptr->call_type ),
           CM_PREF_TERM_CM_1_CALL_PLUS,
           CM_MODE_PREF_NO_CHANGE,
           CM_BAND_PREF_NO_CHANGE,
           CM_PRL_PREF_NO_CHANGE,
           CM_ROAM_PREF_NO_CHANGE,
           CM_HYBR_PREF_NO_CHANGE,
           ph_ptr->plmn,
           ( computed_orig_para != NULL ?
                       computed_orig_para->orig.orig_srv_domain_pref :
                       ph_ptr->srv_domain_pref
           ),
           CM_OTASP_ACT_CODE_NONE,
           &ph_ptr->wlan_pref,
           ph_ptr->acq_order_pref,
           CM_NETWORK_SEL_MODE_PREF_NONE,
           (cm_act_id_type) call_ptr->call_id,
           CM_ACT_UPDATE_REAS_ACT_START,
           call_ptr->force_top_pref
         );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Recompute favored mode pref for call after cmcall_process_orig_mode() */
  call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                  call_ptr->favored_mode_pref,
                                  1,
                                  cmph_map_cm_mode_pref_to_sd_mode_pref(
                                    mode_pref_to_remove
                                  )
                                );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find the SS to use for call origination */

  call_ptr->ss = cmcall_determine_ss_for_origination(
                                                 call_ptr->favored_mode_pref,
                                                 call_ptr->favored_hybr_pref,
                                                 CM_CC_MODE_PREF_MAX,
                                                 //call_ptr->cmcall_mode_info.info_type,
                                                 call_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH( "Using ss=%d for Redial after removing mode_pref %d",
                                                       call_ptr->ss,
                                                       mode_pref_to_remove,
                                                       0 );

  return cmph_force_pref_on_the_fly
         (
           call_ptr->ss,
           ph_ptr,
           pref_reas,
           cmph_map_call_type_to_act_type( call_ptr->call_type ),
           CM_PREF_TERM_CM_1_CALL_PLUS,
           call_ptr->favored_mode_pref,
           ( computed_orig_para != NULL ?
                       computed_orig_para->orig.orig_band_pref :
                       ph_ptr->band_pref
           ),
           ( computed_orig_para != NULL ?
                       computed_orig_para->orig.orig_prl_pref :
                       ph_ptr->prl_pref
           ),
           ( computed_orig_para != NULL ?
                       computed_orig_para->orig.orig_roam_pref :
                       ph_ptr->roam_pref
           ),
           ( computed_orig_para != NULL ?
                       computed_orig_para->orig.orig_hybr_pref :
                       ph_ptr->hybr_pref
           ),
           ph_ptr->plmn,
           ( computed_orig_para != NULL ?
                       computed_orig_para->orig.orig_srv_domain_pref :
                       ph_ptr->srv_domain_pref
           ),
           CM_OTASP_ACT_CODE_NONE,
           &ph_ptr->wlan_pref,
           ph_ptr->acq_order_pref,
           CM_NETWORK_SEL_MODE_PREF_NONE,
           (cm_act_id_type) call_ptr->call_id,
           CM_ACT_UPDATE_REAS_ACT_CONT,
           TRUE
         );
}
#endif /* !FEATURE_LOW_MEM_TARGET */

/*===========================================================================

FUNCTION  cmcall_cmd_orig_para_check

DESCRIPTION
  Check parameter errors for call origination command

DEPENDENCIES
  none
RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type  cmcall_cmd_orig_para_check
(
  cm_call_cmd_s_type       *call_cmd_ptr

)
{
  #if (defined (FEATURE_GPSONE) && defined (FEATURE_ALLOW_IS801_IN_IDLE_EMERG_MODE))
#error code not present
  #endif
  cm_call_cmd_err_e_type  cmd_err       = CM_CALL_CMD_ERR_NOERR;
  cm_call_cmd_err_e_type  cdma_cmd_err  = CM_CALL_CMD_ERR_NOERR;
  cm_call_cmd_err_e_type  gw_cmd_err    = CM_CALL_CMD_ERR_NOERR;

  cm_call_cmd_info_s_type *cmd_info_ptr = NULL;

  cmph_p_type ph_ptr                    = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr     != NULL );
  CM_ASSERT( ph_ptr           != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr = CALL_CMD_INFO_PTR(call_cmd_ptr);;


  /* Perform basic checks for a call origination.
  */

  if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE ||
           cm_reply_check (&ph_ptr->reply, CM_LPM_OK_F))
  {
    return CM_CALL_CMD_ERR_OFFLINE_S;
  }
  else if( cmcall_ptr( call_cmd_ptr->info.call_id ) == NULL )
  {
    return CM_CALL_CMD_ERR_CALL_ID_P;
  }
  else if( !cmcall_is_call_type_valid(call_cmd_ptr->info.call_type) )
  {
    return CM_CALL_CMD_ERR_CALL_TYPE_P;
  }
  else if( cmph_is_in_emergency_cb() &&
           (cmd_info_ptr->call_type != CM_CALL_TYPE_EMERGENCY) )
  {
      #if (defined (FEATURE_GPSONE) && defined (FEATURE_ALLOW_IS801_IN_IDLE_EMERG_MODE))
#error code not present
      #endif  /* FEATURE_GPSONE || FEATURE_ALLOW_IS801_IN_IDLE_EMERG_MODE */
      {
        return CM_CALL_CMD_ERR_EMERGENCY_MODE_S;
      }
  }

  if(call_cmd_ptr->info.srv_type == CM_SRV_TYPE_WLAN)
  {
    if ( !cmph_is_valid_mode_band_pref(CM_MODE_PREF_WLAN_ONLY,
                                       CM_BAND_PREF_ANY))
    {
      return CM_CALL_CMD_ERR_SRV_TYPE_P;
    }
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( call_cmd_ptr->info.is_cdma_info_present)
  {
    #if (defined (FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    cdma_cmd_err =  cmxcall_call_cmd_orig_para_check(call_cmd_ptr);
    #endif

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( (call_cmd_ptr->info.is_gsm_wcdma_ps_info_present ||
       call_cmd_ptr->info.is_gsm_wcdma_cs_info_present))
  {
    #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
    #endif
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( cdma_cmd_err == CM_CALL_CMD_ERR_NOERR &&   /*lint !e774 */
      gw_cmd_err   != CM_CALL_CMD_ERR_NOERR)     /*lint !e774 */
  {
    cmd_err = gw_cmd_err;
  }
  else
  {
    cmd_err = cdma_cmd_err;
  }

  return cmd_err;
}


/*===========================================================================

FUNCTION cmcall_process_orig_mode

DESCRIPTION
  Process call command pertaining to CDMA or GW Orig Mode.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CMD ERR

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type cmcall_process_orig_mode
(
  cm_cc_mode_pref_e_type    mode_usage,
  cmcall_s_type            *call_ptr

)
{
  cm_call_cmd_err_e_type  cmd_err      = CM_CALL_CMD_ERR_NOERR;

  #if ((defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)) && \
        ((defined (FEATURE_GSM) || defined (FEATURE_WCDMA)) ) )
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (mode_usage)
  {
    #if (defined (FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    case CM_CC_MODE_PREF_CDMA_ONLY:
      call_ptr->main_ss_mode = CM_CC_MODE_PREF_CDMA_ONLY;
      call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
      cmd_err = cmxcall_client_orig_proc (call_ptr);

      /* Remove GW from the favored mode pref.
      */
      call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                         call_ptr->favored_mode_pref,
                                         1,
                                         SD_SS_MODE_PREF_GW );

      break;
    #endif  /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

    #if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA))
#error code not present
    #endif /* FEATURE_GSM || FEATURE_WCDMA */

    case CM_CC_MODE_PREF_ANY:
      #if ((defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)) && \
            ((defined (FEATURE_GSM) || defined (FEATURE_WCDMA)) ) )
#error code not present
      #elif(!defined (FEATURE_CDMA_800) && !defined (FEATURE_CDMA_1900) && \
              (defined (FEATURE_GSM) || defined (FEATURE_WCDMA)) )
#error code not present
      #elif(defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900) && \
            !defined(FEATURE_GSM) && !defined(FEATURE_WCDMA))

      call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
      call_ptr->main_ss_mode = CM_CC_MODE_PREF_CDMA_ONLY;

      cmd_err = cmxcall_client_orig_proc (call_ptr);

      #endif
      break;

    default:
      CM_ERR ("bad mode_usage = %d", mode_usage, 0, 0);
      cmd_err = CM_CALL_CMD_ERR_CALL_ORIG_FAIL_S;
  }

  return cmd_err;
}

/*===========================================================================

FUNCTION  cmcall_is_active_ip_app

DESCRIPTION
  Determines if there is a active VOIP app for a given System.

DEPENDENCIES
  none
RETURN VALUE
  BOOLEAN
  TRUE  - If VOIP app is active.
  FALSE - Otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_active_ip_app
(
  sys_sys_mode_e_type  mode,
    /* system mode than can be serviced by app */

  cm_call_type_e_type  call_type
    /* call type that can be serviced by app */
)
/*lint -esym(715, call_type) */
/*lint -esym(715, mode) */
{

  #if (defined(FEATURE_IP_CALL))
  if (cmipapp_find_active_app (mode, call_type) != CMIPAPP_ID_INVALID)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
  #else
  CM_MSG_HIGH ("FEATURE_IP_CALL not defined, did not attempt VoIP", 0, 0, 0);
  return FALSE;
  #endif
}
/*lint +esym(715, call_type) */
/*lint +esym(715, mode) */

/*===========================================================================

FUNCTION cmcall_cmd_copy_fields

DESCRIPTION

  Copy the active fields of a call command into a call object.
  Copy common fields and then call cmxcall_cmd_copy_fields() and
  cmwcall_cmd_copy_fields() to copy mode specific fields

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_cmd_copy_fields(

    cmcall_s_type               *call_ptr,
        /* Pointer to a call object */

    const cm_call_cmd_s_type    *cmd_ptr
        /* Pointer to a call command */

)
{
  /* Point at call command information.
  */
  const cm_call_cmd_info_s_type *cmd_info_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL )
  CM_ASSERT( cmd_ptr  != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr = CALL_CMD_INFO_PTR(cmd_ptr);
  call_ptr->call_client_ptr = cmd_ptr->client_ptr;

  /* Only copy active command fields in order to avoid overwriting
  ** call fields that are irrelevant to this command.
  */
  if( cmd_info_ptr->call_type     != CM_CALL_TYPE_NONE )
  {
    call_ptr->call_type        = cmd_info_ptr->call_type;
  }

  if( cmd_info_ptr->srv_type     != CM_SRV_TYPE_NONE )
  {
    call_ptr->srv_type         = cmd_info_ptr->srv_type;
  }

  if( cmd_info_ptr->favored_mode_pref  != CM_MODE_PREF_NONE )
  {
    call_ptr->favored_mode_pref = cmd_info_ptr->favored_mode_pref;
  }

  if( CM_NUM_LEN(&cmd_info_ptr->num)    != CM_NUMBER_LEN_NONE )
  {
    call_ptr->num              = cmd_info_ptr->num;
  }

  if( CM_NUM_LEN(&cmd_info_ptr->calling_num)    != CM_NUMBER_LEN_NONE )
  {
    call_ptr->calling_num      = cmd_info_ptr->calling_num;
  }

  if( cmd_info_ptr->alpha.len     != CM_ALPHA_LEN_NONE )
  {
    call_ptr->alpha            = cmd_info_ptr->alpha;
  }

  if( cmd_info_ptr->end_status    != CM_CALL_END_NONE )
  {
     call_ptr->end_status      = cmd_info_ptr->end_status;
  }

  if( cmd_ptr->info.info_type != CM_CALL_MODE_INFO_NONE )
  {
    call_ptr->cmcall_mode_info.info_type = cmd_ptr->info.info_type;

  }

  if( cmd_ptr->info.favored_hybr_pref != CM_HYBR_PREF_NONE )
  {
    call_ptr->favored_hybr_pref = cmd_ptr->info.favored_hybr_pref;
  }

  #if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA))
#error code not present
  #endif

  #if (defined (FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  cmxcall_cmd_copy_fields(call_ptr,cmd_ptr);
  #endif
} /* cmcall_cmd_copy_fields() */

/*===========================================================================

FUNCTION cmcall_send_wlan_orig

DESCRIPTION
  Send Origination on WLAN.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_send_wlan_orig
(
  cmcall_s_type  *call_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_ptr->sr.orig_sent_on_mode = SYS_SYS_MODE_WLAN;

  if( call_ptr->call_type    ==   CM_CALL_TYPE_PS_DATA )
  {
     /* For a WLAN PS DATA Call, there is nothing more required.
     ** We are already up on WLAN. It is Always-on. Just let client
     ** know that call is connected.
     */

     call_ptr->call_connect_time    = clk_uptime();
     call_ptr->call_state           = CM_CALL_STATE_CONV;

     cmcall_event( call_ptr, CM_CALL_EVENT_CONNECT );

     return;
  }
  else
  {
    /* Function handles PS calls only, IP calls over WLAN (Voice, VS) are
    ** handled in cmipcall_send_orig().
    */
    CM_ASSERT (FALSE);

    CM_MSG_HIGH ("cmcall_send_wlan_orig - call type %d not supported",
                  call_ptr->call_type, 0, 0);
  }
}  /* cmcall_send_wlan_orig */


/*===========================================================================

FUNCTION cmcall_is_hdr_waiting_for_end_req

DESCRIPTION

  Checks if any call is waiting to received END_CNF from HDR stack.
DEPENDENCIES

RETURN VALUE
  TRUE: If there exist a call for which we are waiting for END_CNF from
        HDR stack.
  FALSE: Otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_hdr_waiting_for_end_req
(
  void
)
{

  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  /*-----------------------------------------------------------------------*/
  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA  &&
         call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_HDR &&
         call_ptr->call_state == CM_CALL_STATE_IDLE &&
         call_ptr->call_subst.idle == CM_CALL_IDLE_SUBST_ENDWAIT
       )
    {
      if(call_ptr->end_req_wait_uptime <= clk_uptime())
      {
        return TRUE;
      }
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

  return FALSE;
}


/*===========================================================================

FUNCTION cmcall_count_on_ss

DESCRIPTION

  Returns number of calls on a given SS.

DEPENDENCIES


RETURN VALUE
  Number of calls on a given SS.

SIDE EFFECTS
  none

===========================================================================*/
int cmcall_count_on_ss
(
  cm_ss_e_type    ss
)
{
  byte              num_allocated_call_ids = 0;
  cmcall_s_type    *call_ptr = NULL;
  cm_iterator_type  call_itr;

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);


  while(call_ptr != NULL)
  {
    if(call_ptr->call_id != CM_CALL_ID_INVALID  &&
       call_ptr->ss == ss)
    {
      num_allocated_call_ids++;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }
  return num_allocated_call_ids;

} /* cmcall_count_on_ss () */




/*===========================================================================

FUNCTION cmcall_is_no_call_in_gw_cs

DESCRIPTION
  Checks if there are no calls in gw cs domain

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  - No calls in cs domain
  FALSE - There is/are calls in cs domain

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_no_call_in_gw_cs (void)

{

  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS)
    {
      return FALSE;
    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  /*-----------------------------------------------------------------------*/

  return TRUE;

} /* cmcall_is_no_call_in_gw_cs() */


/*===========================================================================

FUNCTION cmcall_is_no_call_in_gw_ps

DESCRIPTION
  Checks if there are no calls in gw ps domain

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  - No calls in ps domain
  FALSE - There is/are calls in ps domain

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_no_call_in_gw_ps (void)

{

  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS)
    {
      return FALSE;
    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  /*-----------------------------------------------------------------------*/

  return TRUE;

} /* cmcall_is_no_call_in_gw_ps () */

/*===========================================================================

FUNCTION cmcall_is_active_on_mode

DESCRIPTION
  Checks if there is a call on a given mode.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  FASLE  - No calls
  TRUE   - Otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_active_on_mode
(
  sys_sys_mode_e_type  sys_mode
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
     /* call Object Iterator */

 /*-----------------------------------------------------------------------*/

 cmcall_obj_iterator_init(&call_itr);

 call_ptr = cmcall_obj_get_next(&call_itr);

 while(call_ptr != NULL)
 {
   if(sys_mode == SYS_SYS_MODE_GSM   ||
      sys_mode == SYS_SYS_MODE_WCDMA ||
      sys_mode == SYS_SYS_MODE_GW)
   {
       if(call_ptr->sys_mode ==  SYS_SYS_MODE_GSM    ||
          call_ptr->sys_mode ==  SYS_SYS_MODE_WCDMA  ||
          call_ptr->sys_mode ==  SYS_SYS_MODE_GW)
       {
         return TRUE;
       }
   }
   else if(call_ptr->sys_mode == sys_mode)
   {
       return TRUE;
   }

   call_ptr = cmcall_obj_get_next(&call_itr);
 } /* while */

 /*-----------------------------------------------------------------------*/

 return FALSE;

}

/*===========================================================================

FUNCTION cmcall_get_prim_with_groupid

DESCRIPTION
  Gives the conn id of primary pdp with given group id.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Connection id of primary pdp. Unknown id if no primary pdp is found.

SIDE EFFECTS
  none

===========================================================================*/
byte cmcall_get_prim_with_groupid  (

  byte pdp_group_id
    /* Group id of primary pdp */
)
{

  #if ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && \
         defined(FEATURE_SECONDARY_PDP))
#error code not present
  #else

  SYS_ARG_NOT_USED(pdp_group_id);

  #endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
            && defined(FEATURE_SECONDARY_PDP) */

  /*-----------------------------------------------------------------------*/

  return CM_UNKNOWN_CONN_ID;

} /* cmcall_is_no_call_in_gw_ps () */


/*===========================================================================

FUNCTION cmcall_det_sys_mode_from_ss

DESCRIPTION
  Returns sys mode that is currently present in the given ss

DEPENDENCIES
  None.

RETURN VALUE
  SS instance

SIDE EFFECTS
  none

===========================================================================*/
sys_sys_mode_e_type  cmcall_det_sys_mode_from_ss(

    cm_ss_e_type   ss
      /* system instance, MAIN, HYBR_HDR ... */
)
{
  cmss_s_type                *ss_ptr              = cmss_ptr();
  /* Pointer to the serving system object */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT (BETWEEN(ss, CM_SS_NONE, CM_SS_MAX));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Returns system mode that can be carried by a ss */


  switch( ss )
  {
    case CM_SS_MAIN:
      return ss_ptr->info.sys_mode;

    case CM_SS_HYBR_1:

      #ifdef FEATURE_HYBR_GW
#error code not present
      #else
      return SYS_SYS_MODE_HDR;
      #endif

    case CM_SS_WLAN:
      return SYS_SYS_MODE_WLAN;

    case CM_SS_MAX:
    default:

      CM_ERR ("Unknown ss %d", ss, 0, 0);
      return SYS_SYS_MODE_NONE;
   }
} /* cmcall_det_sys_mode_from_ss () */

/*===========================================================================
FUNCTION cmcall_det_ss_from_sys_mode

DESCRIPTION
  Determines ss instance on which the given sys_mode is present. currently
  supports WCDMA, HDR, WLAN only.

DEPENDENCIES
  None.

RETURN VALUE
  SS instance

SIDE EFFECTS
  none

===========================================================================*/
cm_ss_e_type  cmcall_det_ss_from_sys_mode(

    sys_sys_mode_e_type   sys_mode
        /* sys mode */
)
{
  cmss_s_type                *ss_ptr              = cmss_ptr();
    /* Pointer to the serving system object */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT (BETWEEN(sys_mode, SYS_SYS_MODE_NONE, SYS_SYS_MODE_MAX));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Function should return valid instance for NO_SRV. Function is typically
  ** called after receiving a msg on a system, so chances of NO_SRV should
  ** be low.
  */
  if (sys_mode == SYS_SYS_MODE_NO_SRV)
  {
    CM_MSG_HIGH ("sys mode passed is NO_SRV, ret MAIN", 0, 0, 0);
    return CM_SS_MAIN;
  }


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the instance carrying given sys mode
  */
  if (ss_ptr->info.sys_mode == sys_mode)
  {
    return CM_SS_MAIN;
  }
  else if( sys_mode == SYS_SYS_MODE_HDR
           #ifdef FEATURE_HYBR_GW
#error code not present
           #endif
         )
  {
    return CM_SS_HDR;
  }
  else if (sys_mode == SYS_SYS_MODE_WLAN)
  {
    return CM_SS_WLAN;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ERR ("Unknown sys mode %d", sys_mode, 0, 0);
  return CM_SS_NONE;

} /* cmcall_det_ss_from_sys_mode () */

/*===========================================================================
FUNCTION cmcall_dem_is_apps_wakeup_from_suspend_event

DESCRIPTION
  Determines if the event should wakeup the processor from the suspend state

DEPENDENCIES
  None.

RETURN VALUE
  True : If the apps processor needs to be woken up
  False: Otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmcall_dem_is_apps_wakeup_from_suspend_event(

    cm_call_event_e_type   call_event
        /* Call event */
)
/*lint -esym(715, call_event) */
{
  #ifndef FEATURE_APPS_POWER_COLLAPSE
  /* If Apps power collapse is not defined, return ture
  */
  return TRUE;
  #else
#error code not present
  #endif
} /* cmcall_dem_is_apps_wakeup_from_suspend_event */
/*lint +esym(715, call_event) */

