/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   W C D M A   C A L L   M O D U L E

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

  cmcall_is_waiting_for_reply
    Check whether the call object is currently waiting for a reply.

  cmwcall_info_get
    Copy the current call state information into a specified buffer.

  cmwcall_client_cmd_proc
    Process clients' call commands.

  cmwcall_rpt_proc
    Process Lower Layer call commands.

  cmcall_timer_proc
    Process call timer events


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2001 - 2008 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmwcall.c#11 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/09   ks      Added fix to indicate call type as CS_DATA on when 
                   empty bearer capability is received in MT setup message.
06/10/09   rm      CM should not error fatal for non-EF_RAT refresh.
                   Also CM should handle MMGSDI_REFRESH_NAA_INIT_FCN
06/01/09   rk      correcting cmwcall_send_pdp_abort_req adding condition to 
                   check if GW PS call exist,then only send PDP abort req.
03/26/09   ks      Added support for blocking MO/MT voice call on Data Card
03/20/09   ks      Removing FEATURE_CM_DELAY_SETUPRES functionality
                   to avoid tear down of sec pdp due to MT voice call. 
02/13/09   rn      Corrected print statement
01/29/09   rm      RAT balancing phase-2 - Vote YES to EF_RAT refresh during
                   CS/PS calls - MMGSDI changes
01/29/09   rk      Fixing the compilation issue when both FEATURE_GPSONE and
                   FEATURE_MULTIMEDIA_FALLBACK features are defined.
01/22/09   rm      Fixing compilation issue when FEATURE_SECONDARY_PDP
                   is disabled
11/21/08   am      Fixing compilation error for Running off-target tests
11/18/08   am      If FEATURE_DATA_REFRESH_VOTING is defined CM will not
                   vote for PS calls.(moving this from GSDI to MMGSDI)
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
10/31/08   rn      On going offline, send PDP_ABORT to NAS
08/20/08   st      Klockwork fixes
08/05/08   rk      Reverting the call_type and BC when call modification failed.
06/12/08   sv      Resolve Klocwork defects
05/28/08   ks      Updating the associated primary profile number when
                   secondary PDP is promoted to Primary
05/19/08   rk      Added multiparty call indication CM_CALL_EVENT_MPTY_CALL_IND
05/12/08   sg      AMR WB vocoder support for GERAN
05/08/08   sv      Removed snd.h as it is not required for GW
05/02/08   vk      fixed messages for debug
04/29/08   ks      Reverting the changes of Updating the associated primary
                   profile number when secondary PDP is promoted to Primary
04/17/08   ks      allow voice->VT switch MODIFY based on
                   bc_repeat_indicator.repeat_indication
04/17/08   ks      Allow 2nd Primary PDP to use the default profile when
                   1st Primary PDP uses it
03/31/08   ks      Updating the associated primary profile number
                   when secondary PDP is promoted to Primary
03/05/08   vk      corrected code for HYBR_GW configuration
12/12/07   rm      Adding RAT-balancing feature support.
12/07/07   ra      Added support for IMS VCC in HOMER files
11/27/07   sv      Removing Feature FEATURE_GW_CNAP_CODING_SCHEME
11/22/07   sg      Updated handling of CM_MT_MODIFY_IND for nw_initiated_ind
11/19/07   rn      Updated call type to Voice for ITC_AUX_SPEECH
11/19/07   rn      Added support for UE initiated dormancy
11/05/07   sg/vs   Renaming FEATURE_CM_TEMP_NAS_DEP_TBD To
                   FEATURE_GW_CNAP_CODING_SCHEME
10/01/07   rm      Handling of GSDI_CARD_ERR_POLL_ERROR in CM
09/17/07   rn      Updated field enc_alpha of cmcall_s_type to send
                   coding scheme
09/12/07   pk/rm   Merge to mainline - Adding support for handling calls in origination state
                   while service is lost
08/07/07   ic      Lint clean up
06/27/07   cl      Add new CM API to expose cmwcall_get_user_data() to RPC
06/22/07   sk      DDTM rewrite.
06/07/07   rp      Making pdp_info static
04/15/07   rp      Revising logic to set reverse_call_setup_direction
04/15/07   rp      Updating call_ptr->sys_mode
                   with ss_ptr->last_system_reported.
04/13/07   rp      Initializing gw_ps_info.
03/29/07   rp      Setting mode_info.info_type for CM_MANAGE_CALLS_CONF
12/28/06   ka      Correcting logic to set is_call_mod_possible field
11/21/06   ka      Passing rab_id details in CM_CALL_PROGRESS_INFO_IND.
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
10/12/06   ka/rp   Fixing issue where MO_CALL_CONF changes call type from
                   EMERGENCY to VOICE.
10/03/06   ka      Adding support for IP call on WCDMA.
09/14/06   ka      Adding support for call control on secondary ps calls
08/25/08   ka      Adding support for delaying setup response to let QTV
                   PAUSE streaming session.
08/11/06   ka      Adding CS_DATA call check to the list of calls that are
                   reviewed before sending PS_DATA origination in GSM.
07/06/06   ka/ic   Featurization fixes for PS call control
07/04/06   ka      Changes to support call control on PS calls.
06/05/06   ka/ic   Extending logic for sending user busy with setup response
                   to CS_DATA calls.
05/26/06   pk      Forcing the top origination prerefences for ccbs calls
05/10/06   ka      mode_spec_end used to bunch calls of particular info_type
                   and then send them to wcall, ipcall or xcall. Modifying
                   wcall, ipcall and xcall to end calls of their typeonly.
04/17/06   ka      Allowing PS calls during voice call for GSM in DTM mode.
04/13/06   ka      Adding Config options to deactivate req and pdp_act_rej
                   request. PDP activate reject moves the call to end wait.
                   Explicit call end is received to clear the call.
04/01/06   ka      Changes for emerg serv category during orig, aborting
                   manual search and returning current band with srv ind.
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
02/08/06   ka      Call end sent for MT ps call that has been answered and is
                   waiting to be connected translates to
                   SMREG_PDP_DEACTIVATE_REQ.
01/13/06   ka      Removed logic to reject MT PS call when MO voice call is
                   in origination.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ka      Adding Release 5 changes including emerg call list,
                   traffic flow template for PDP modify and protocol config
                   options for some of the GW PS call reports.
01/06/06   ic      Lint cleanup
11/30/05   ka      Removed sending nsapi with pdp modify request.
                   Modifying check for user to user data length to be within
                   MAX_USER_USER_DATA_LENGTH only for gw cs originations.
11/23/05   ka      Modification to allow hold/active for ip calls.
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
10/24/05   ka      Send call end request to MN on receipt of bearer release
                   indication for CS calls.
09/12/05   pk      Calling PD session grant algorithm, if a session is
                   pending, afer CM_CALL_EVENT_CONNECT
08/16/05   pk      Added support for priority Queues for origination
                   parameters
08/16/05   ka      Correcting favored_mode_pref computation for CCBS recall.
08/02/05   ic      Print value that caused error in default switch case
07/30/05   ka      Handling silent redial for CCBS.
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/06/05   ka      Setting correct end_status for PDP_DEACTIVATE_CNF.
                   Sending CALL_EVENT_PROGRESS_INFO_IND for all GW CS calls.
06/13/06   ic      Fixed Lint warnings - removed external declaration of
                   cm_unreserve_connection_id()
06/02/05   ic      Cleaned up cmwcall_sups_cmd_check()
                   Cleaned up cmwcall_cmd_copy_fields()
                   Use cmcall_event() to send CM_CALL_EVENT_SUPS
                   Copy SUPS information in cmwcall_info_get()
06/02/05   ic      Mainlined FEATURE_CIPHER_IND
05/31/05   ic      Fix Lint/compiler warnings in cmwcall_print_pdp_details()
05/19/05   ka      Merging CIPHER_IND feature.
03/30/05   dk      Generate CALL_RETRIEVED event whenever MN informs CM.
03/24/05   ic      Moved processing of CM_CALL_CMD_INFO_GET to cmcall.c
                   (CR#59955)
03/17/05   ic      Send CM_MO_END_X_CALL_REQ for CM_CALL_TYPE_CS_DATA and
                   CM_CALL_TYPE_EMERGENCY
02/22/05   ka      Cleaned up ps orig state check function. Removed copying
                   of cmd fields into call object for rab reestablish
                   request. checking for is_user_data_present in
                   cmwcall_get_user_data.
02/01/05   dk      Added WLAN Support.
01/21/05   ka      end_status in call object is used to record the requestor
                   of call end for a data call (UE terminated or NW
                   terminated). LL_CAUSE refers to UE ending the call and
                   NETWORK_END refers to network ending the call.
01/24/05   ic      Replaced cmph_is_ph_in_emerg_cb_mode() with
                   cmph_is_in_emergency_cb()
01/20/05   ka      Removing condition check where non PS calls were not
                   allowed when SS reported PS_ONLY service. There are cases
                   when PS_ONLY service is reported but emergency call has
                   to be tried for origination.
12/19/04   ic      Moved common call fields from cmwcall_assert() to cmcall.c
                   cmcall_assert()
                   In cmwcall_end_req() coupled setting of call state together
                   with setting of call substate (or otherwise cmcall_assert()
                   would assert that call state is idle but call substate is
                   not set correctly)
12/15/04   jqi     Merged Changing MT_SETUP_IND reject cause to USER_BUSY
                   from NORMAL_CALL_CLEARING.
12/10/04   ic      Removed GET_NSAPI() now that all targets have it defined
                   in NAS
12/06/04   ic      Restored GET_NSAPI(), suppressed Lint warning as
                   depending on target, NAS sm.h may or may not have it
                   defined
12/06/04   ic      Lint cleanup (build 6500 TNAR), removed GET_NSAPI
12/02/04   dk      Added call substates for Packet Data Calls.
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
11/20/04   ic      Corrected lint warnings
11/19/04   ka      Adding support for user to user signaling
11/18/04   ka      Adding support for MT PDP feature
11/15/04   ic      Lint cleanup with CM_DEBUG on
11/11/04   ic      Lint cleanup
09/28/04   ka      Unforcing preferences at end of all calls for all cases.
                   ps detach for attach on demand was being done at the end of
                   last ps call. Modified this behavior to end of all calls.
                   Removed functions force and unforce_pref_on_the_fly.
09/22/04   ka      Call substate should not be changed to either HOLD or
                   RETRIEVE depending on SS_NOTIFY indications. Should be
                   changed only by CM_MANAGE_CALLS_CONF.
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/14/04   ic      Call direction clean up.
                   Added calls to cmcall_set_call_direction()
09/07/04   ka      Removed usage of is_orig_sent, mode_in_use and added
                   orig_sent_on_mode
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
09/03/04   dk      Removed call to ps_sig_down_wait_flag set function.
08/30/04   ka      Added code for handling Sec PDP reject message.
08/27/04   jqi     Added ps dormant optimization.
08/18/04   ka      Call direction now is common to system modes.
                   Setting call direction using a function.
08/11/04   ka      Bearer capability for Anonymous clients (Diag) is populated
                   by CM. System mode for a call is not updated when in
                   NO_SERVICE.
08/05/04   ic      Lint clean up - removed !e641
08/04/04   ic      CM Lint clean up - cmwaoc.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
08/02/04   dk      Initilaize mode_in_use for a particular call while sending
                   origination.
07/28/04   ka      Added support for Secondary PDP context
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added cmwcall_dummy() to get rid of ADS 1.2 compiler warning
07/21/04   sj      Added support for umts to 1x handover.
07/19/04   ka      Removed handling of error check for CM_CALL_CMD_ORIG, since
                   it is being done in cmcall_client_orig_proc ()
07/14/04   ic      In cmwcall_rpt_proc() moved call to ALS function
                   resolve_mt_call_ind_func() to after call type for the
                   incoming call has been determined
07/12/04   ic      Added support for FEATURE_ALS
07/07/04   dk      Activated support for handling PS_SIG_REL_INDf for CR 43512.
07/06/04   dk      Modified signature of cm_find_connection_id().
07/05/04   dk      Changed signature of cmcall_update_gsdi_and_log_call_state().
                   Changed cmcall_update_gsdi_and_log_call_state() to indicate
                   that CM is not in traffic only when Overall Call state is IDLE.
07/01/04   dk      Added Call Object Iterator.
06/30/04   dk      Lint cleanup and support for handling PS_SIG_REL_IND report.
06/25/04   ws      Preserve number type/number plan after modifications from CC.
06/25/04   dk      Modified to support new call object structure. Removed
                   references to call_object_deallocate() since it is
                   part of call_id_deallcoate. Also changed the references
                   to cmcall_id_deallocate().
06/16/04   ic      Added calls to cmcall_update_call_end_info() before
                   cmcall_event() is called for CM_CALL_EVENT_END_REQ and
                   CM_CALL_EVENT_END events.
                   Set call_connect_time field in call object before cmcall_event()
                   with CM_CALL_EVENT_CONNECT is called.
                   Cleaned up cmwcall_reset() to call cmcall_reset() and reset
                   only GW specific fields.
                   Fixed obvious lint errors (-e715 - variable not referenced;
                   bad enum type used)
06/15/04   dk      Removed calls to cmcall_update_overall_state() and replaced
                   with cmcall_update_gsdi_and_log_call_state(). Initialized
                   IDLE_SUBSTATE of call to NONE during cmwcall_end.
06/10/04   dk      Modified cmwcall_end_req to call the cmph_call_end_req.
06/09/04   ka      Removed conditions that checked for atleast one PS call to
                   be up before PS_DATA_AVAILABLE command can be issued.
06/01/04   npr     Added IP Header and Data compression requirements to GW PS
                   call origination and information structures for GPRS SNDCP.
05/27/04   ic      In cmwcall_client_cmd_proc() call cmcall_client_cmd_err()
                   irrespective of whether error occured or not (client is
                   supposed to be always notified about command processing
                   status)
05/26/04   ka      Removed check that stopped simultaneous CS-DATA + PS_DATA calls.
05/19/04   ka      Removed calls and commented function defintions for forcing and unforcing
                   preferences on the fly.
05/17/04   ic      In cmwcall_reset() removed assertion on call_ptr->init_mask
                   to match behavior of cmxcall_reset()
                   [ needed for CM unit test ].
05/14/04   ka      Fixed Lint errors.
05/06/04   ic      Added assertions on call_ptr->sr.is_orig_sent in
                   cmwcall_send_orig().
                   In cmwcall_reset() initialized srv_type, favored_mode_pref
                   and favored_hybr_pref fields.
                   In cmwcall_cmd_copy_fields() added copying of favored_mode_pref
                   and favored_hybr_pref fields.
04/20/04   ka      Passing the right rab_id down to rabm for a rab-reestablish
04/13/04   ic      CM_ASSERT() clean up
04/08/04   ws      Merged from MSMSHARED_CM.01.00.32.00.03
03/31/04   ka      Removed setting orig_srv_domain_pref to None to
                   avoid failing subsequent PS calls
03/16/04   ka      Merged changes from 6250
                   Signaling connection is set only after CM_CALL_CONNECTED_IND is recvd.
                   Reject PS data call origination when MB is in GSM voice call.
                   Fixed Lint errors.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/29/04   ws      Added call of gsdi event handling for both modes.
01/20/04   ka      Added support for Multiple PDP contexts.
01/13/04   ka      Merged changes from MSM6200
01/06/03   ws      Added handling of gsdi events for card 2.
12/15/03   ws      Set the bcd number present when sending called number to nas.
                   Set signaling_conn_rel_req during originations.
12/12/03   ws      Added cmwcall_set_lmtd_srvc_req
12/08/03   ws      Mainlined umts concurrent services feature.
10/31/03   ws      Check call direction in handling CM_MT_DISC_IND.
                   Merged 6200 changes for handling proc_end_reason.
10/30/03   prk     Updated sys_mode field in call object when the
                   CM_CALL_EVENT_SETUP_IND or CM_CALL_EVENT_CALL_CONF event
                   is sent.
10/27/03   ws      Fix compiler warnings.
10/21/03   ws      Dual Slot Support.
10/21/03   ws      Fixed kc's fix for LTK compiler error.
10/07/03   prk     Added processing of CNM_MN_REJECT_CAUSEs and
                   AS_REJ_RRC_REL_IND during silent redial.
10/01/03   ws      Merged from GW Targets
                   Check for call type instead of mode info in cmd copy fields
09/23/03   ws      Merged fix for SIM fails during an emergency call
                   do not end the call.
09/22/03   RI      Removed AS REL IND (RR/RRC) for Silent redial.
09/18/03   prk     Removed references to ph_ptr in cmwcall_end() to fix
                   compiler warning.
09/18/03   RI      Changes to support CS signaling release during call end.
                   Added AS reasons for silent redial.
09/17/03   ws      Dual valid card support.
09/15/03   tml     call control GCF fixes
09/04/03   RI      Modified SR not to end the origination, when the call is
                   ended by the lower layers.
09/03/03   jds     Correct bug when SIM fails (or is removed) during an
                   emergency call (do NOT end the call).
08/26/03   ws      Picked up call control functions that were lost in Automatic Mode
                   Support Changes.
08/25/03   RI      Modified silent redial, not to end the call if
                   origination request is sent to lower layers.
08/21/03   ws      Added support for Silent Redial and Automatic mode.
08/20/03   AT      Fixed the problem where we tried to reuse the cm_cmd_ptr
                   during GSTK.  This resulted in a double free.
08/18/03   RI      Added checks to determine if call is allowed for
                   MT and MO calls.
08/12/03   prk     Added processing of minimum_qos field.
08/08/03   prk     Called cmwcall_unforce_pref_on_the_fly( ) whenever a PDP
                   deactivation occurs.
07/28/03   vt      Set sys mode on incoming call.
07/25/03   prk     Removed the check of service status during processing of
                   the data available command.
07/21/03   sun     Silent Redial should occur only for MM errors
07/21/03   RI      Added normal call clearing cause for CM_MO_END_X_CALL_REQ.
07/09/03   prk     Checked for a valid SIM state during call origination.
07/08/03   vt      Set sys mode for call events.
06/10/03   RI      Deleted the check for active CS voice calls when a PS data
                   call is originated.
06/09/03   AT      In multimode, the 1x version of cmcall_overall_call_state
                   changed the timing of when the overall call state changes
                   from IDLE to originating.  Made a change to reset the CCM
                   whenever we have a new call and are not in a CONVERSATION
                   instead of when we are in IDLE.  This allows the CCM to
                   reset to 0 before the first call, but removes the race
                   condition of overall_call_state changing from IDLE to
                   either incoming or originating.
06/04/03   RI      Modified the processing of ECT information in the
                   NotifySS message.
                   Added checks to wake up the protocol stack when a RAB
                   re-establish request is received from the client in the
                   deep sleep mode.
05/22/03   sun     Made gw_cs and gw_ps a union in call_cmd_info_s_type
                   Check for CM_CALL_CMD_PS_DATA_AVAILABLE in client_cmd_ll
                   check for rej values instead of cc_cause values for SR
05/13/03   sun     Fixed Silent redial reason. Check for cc reject type instead
                   of cc reject value
05/06/03   kar     Fixed compile error on LTK
05/01/03   jar     Added Handling of CM_GSDI_REFRESH_FCN_IND
04/29/03   sun     Added support for cc reject
04/28/03   AT      Lint cleanup.
04/25/03   prk     Fixed problem where a pending call origination was not
                   being completely being ended if the client ends the call.
04/11/03   prk     Removed references to FEATURE_SUPS, FEATURE_AOC and
                   FEATURE_MMGSDI.
03/27/03   CS      Checked overall state for CM_CALL_CMD_PS_DATA_AVAILABLE.
03/21/03   RI      Call origination request is sent without checking the
                   service status ( CS domain )
03/19/03   RI      Modified the call end handling for emergency call.
                    - restores the original service preference when the
                      emergency call is ended in the limited service mode.
                   Modified PS attach on demand in the limited service mode.
03/18/03   PRK     Modified cmwcall_force_pref_on_the_fly() to allow emergency
                   calls to be made if there is full/limited service.
03/14/03   AT      Added the gstk_is_cc_required function to determine if we
                   should preform call control.
03/13/03   RI      Added sm_unreserve_connection_id() to unreserve connection
                   IDs for PS data calls.
                   Modified cmwcall_unforce_pref_on_the_fly() to change the
                   service domain only in CS_ONLY preference.
03/11/03   ATT     Added support for call control performed by CM clients.
03/06/03   PRK     Fixed a bug in the call origination parameter checking.
                   The call info type was incorrectly being set for PS calls.
02/28/03   AT      Added call control for voice calls.
02/27/03   CS      Added MT/MO indication and transaction ID in CS GSM/WCDMA
                   call info for USAT support.
02/25/03   RI      Modified processing of CM_CALL_CMD_ORIG to support
                   PS attach/detach on demand.
                   Added cmwcall_force_pref_on_the_fly() and
                   cmwcall_unforce_pref_on_the_fly().
                   Modified handling of multiple PS data call origination.
02/13/03   PRK     Mainlined the FEATURE_CONC_SRVC_NEW_API feature.
02/10/03   AT      Added support for CAT_APP clients
02/11/03   PRK     Updated cmwcall_cmd_copy_fields() to not copy the mode info
                   type if it is set to"none".
02/04/03   PRK     Removed static from cmwcall_end_req().
           RI      Corrected the PS command information when ending the call.
           RI      Added support for concurrent services.
           CS      Added silent redial and call blocking for non-emergency calls
                   when card is invalid.
                   Set CM_CALL_END_LL_CAUSE for all call rejection cases.
11/22/02   CS      Added support to allow emergency call to go through when
                   there is a voice call.
01/21/03   ws      Updated copyright information for 2003
12/05/02   PRK     Removed usage of is_allocated  in call object under the
                   FEATURE_CM_ABORT_X_CALL.
11/26/02   vt      Forward declared a function to fix a compile problem.
11/14/02   vt      Merged MM and GW versions
                   Merged mm build changes.
                   Fixed a compile error when RUIM is defined.
                   Set the mode to GW when processing CM_CALL_CONNECTED_IND.
                   Removed usage of is_allocated & call_id in call object.
                   Replaced checking of call id with call object.
                   Added mode param to cmcall_callobject_allocate().
11/5/02    RI      Fixed problem where emergency calls could not be ended.
11/05/02   PRK     Modified cmwcall_client_cmd_check to allow origination of
                   non-emergency calls in a limited service state.  The lower
                   layers are responsible for handling this case.
10/23/02   RI      Modified the handling of CM_CALL_CMD_END.
10/30/02   AT      Added additional support for handling no funds situations.
10/17/02   PRK     Modified cmwcall_end to clear cm_active_ps_call_id if the
                   PS data call is being ended.
10/14/02   PRK     Initialized SMREG_PDP_DEACTIVATE_REQ message in
                   cmwcall_client_cmd_forward_ll().
10/11/02   CS      Added support for invalid SIM handling. Terminate active
                   calls when SIM becomes invalid.
                   Modified silent redial handling.
                   Block out normal calls in limisted service mode.
10/11/02   RI      Modified processing of call barring notification.
09/30/02   RI      Updated the processing of SS Notifications for
                   call forwarding.
09/16/02   PRK     Added FEATURE_UI_CM_NON_SVD.
09/12/02   RI      Updated the copying of forward_cug_info in
                   cmwcall_cmd_copy_fields().
09/11/02   RI      Updated the processing of NotifySS message.
09/06/02   RI      Modified the call cmd handling for CS data calls.
08/30/02   vt      RTRE changes.
08/15/02   RI      Modified the max. forwarded to number size for ECT.
08/13/02   RI      Updated cmwcall_rpt_proc to process CD and ECT.
08/12/02   RI      Added support for ECT CD features.
08/08/02   AT      Added support for CUG and CNAP features.
07/29/02   RI      Modified processing of CM_MT_CALL_SS_IND.
                    - the NotifySS is received only after the Setup Indication.
                      message.
                   Added support for processing cause of no CLI in
                   Setup Indication.
                   Modified processing of CM_CALL_PROGRESS_INFO_IND.
                   to support inband information.
07/23/02   RI      Modified cmwcall_client_cmd_ll_reply()
                    - call ptr is not required for CM_CALL_CMD_SUPS.
07/11/02   PRK     Included cmll.h.
06/27/02   RI      Modified cmwcall_client_cmd_ll_reply()
                    - call ptr is not required for CM_CALL_CMD_SUPS.
06/14/02   AT      Removed silent redial for WCDMA.
06/11/02   RI      Deleted the pointer to the call object parameter in
                   cmwcall_client_cmd_forward_ll(), cmwcall_client_cmd_ll_reply()
                   and cmwcall_client_cmd_check();
                   Updated the procesing of call commands and call event
                   notifications.
                   Modified cmwcall_end() to deallocate call object.

05/31/02   RI      Modified cmwcall_client_cmd_ll_reply to copy sups parameters.
05/28/02   RI      Modified the cmwcall_end() to include the deallocation
                   of call IDs and freeing of call object.
05/23/02   RI      Removed the presentation allowed check in CM_MT_SETUP_IND and
                   CM_CALL_CONNECTED_NUM_IND.
05/16/02   PRK     Changed reference to cmd_info_ptr->call_type to
                   call_cmd_ptr->info.call_type due to FEATURE_AOC being
                   defined.
05/10/02   PRK     Made modifications to support multi-mode.
04/26/02   PRK     Modified cmwcall_reset to "zero out" all fields in the
                   cmcall_s_type structure.
04/24/02   RI      Added processing of CM_RAB_REESTAB_FAIL in cmcall_rpt_proc()
                   Renamed CM_RAB_ESTAB_IND to CM_RAB_REESTAB_IND.
                   Temporary fix in cmcall_client_cmd_check() to bypass the
                   client cmd check for PS call cmds.
04/16/02   CS      Block calls when the current serving system does not suppor the call.
                   Block calls with different service when a call is in progress since
                   simultaneous voice and data calls are not supported.
03/14/02   AT      Modified processing of CM_CALL_CMD_END event in
                   cmwcall_client_cmd_ll_reply to call cmcall_end if
                   no origination is pending.
09/17/01   VT      Main-lined FEATURE_NEWCM
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

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "customer.h"  /* Customer configuration file */

#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
#else /* FEATURE_WCDMA || FEATURE_GSM */

/*===========================================================================
FUNCTION cmwcall_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmwcall_dummy( void )
/*lint -esym(714,cmwcall_dummy) */
/*lint -esym(765,cmwcall_dummy)
** Can't be static as it would result in compiler warnings
*/
{
}


#endif /* FEATURE_GSM || FEATURE_WCDMA */
